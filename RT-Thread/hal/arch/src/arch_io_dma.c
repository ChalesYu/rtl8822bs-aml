/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_dma.c
** Last modified Date:  2013-04-23
** Last Version:        v1.0
** Description:         dma模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2013-04-23
** Version:             v1.0
** Descriptions:        The original version 初始版本
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_dma.h"


/*********************************************************************************************************
  局部变量定义
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       DMAModeConfigSet
** Descriptions:        DMA模块工作模式配置
** input parameters:    u32Base:    DMA模块的基地址
**                      u8Stream:   DMA的Stream编号，为0-7
**                      u32Config:  配置值，可以取下列几组参数的或
**                      // 1、数据传输方向
**                      DMA_DIR_PeripheralToMemory        外设到内存
**                      DMA_DIR_MemoryToPeripheral        内存到外设
**                      DMA_DIR_MemoryToMemory            内存到内存
**                      // 2、外设地址增长控制
**                      DMA_PeripheralInc_Enable          外设地址增长使能
**                      DMA_PeripheralInc_Disable         外设地址增长禁制
**                      // 3、内存地址增长控制
**                      DMA_MemoryInc_Enable              内存地址增长使能
**                      DMA_MemoryInc_Disable             内存地址增长禁制
**                      // 4、外设数据长度
**                      DMA_PeripheralDataSize_Byte       1个字节长
**                      DMA_PeripheralDataSize_HalfWord   2个字节长
**                      DMA_PeripheralDataSize_Word       4个字节长
**                      // 5、内存数据长度
**                      DMA_MemoryDataSize_Byte           1个字节长
**                      DMA_MemoryDataSize_HalfWord       2个字节长
**                      DMA_MemoryDataSize_Word           4个字节长
**                      // 6、DMA环路模式控制
**                      DMA_Mode_Normal                   正常模式
**                      DMA_Mode_Circu32ar                 环路模式
**                      // 7、DMA优先级
**                      DMA_Priority_Low                  低优先级
**                      DMA_Priority_Medium               中优先级
**                      DMA_Priority_High                 高优先级
**                      DMA_Priority_VeryHigh             超高优先级
**                      // 8、内存的burst控制
**                      DMA_MemoryBurst_Single            单次传输
**                      DMA_MemoryBurst_INC4              4次
**                      DMA_MemoryBurst_INC8              8次
**                      DMA_MemoryBurst_INC16             16次
**                      // 9、外设的burst控制
**                      DMA_PeripheralBurst_Single        单次传输
**                      DMA_PeripheralBurst_INC4          4次
**                      DMA_PeripheralBurst_INC8          8次
**                      DMA_PeripheralBurst_INC16         16次
**                      DMA_Double_Buffer_Mode_Enable     使能Double Buffer mode 
**                      DMA_Double_Buffer_Mode_Disable    禁止Double Buffer mode 
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAModeConfigSet(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32Config)
{
    rt_uint32_t u32Temp;
    u32Temp = HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18);
    u32Temp &= ~0xFFF37FC0;
    u32Temp |= u32Config;
    HWREG32(u32Base + DMA_SxCR+ ((rt_uint32_t)u8Stream)*0x18) = u32Temp;
}

/*********************************************************************************************************
** Function name:       DMAFIFOConfigSet
** Descriptions:        DMA的FIFO配置
** input parameters:    u32Base:    DMA模块的基地址
**                      u8Stream:   DMA的Stream编号，为0-7
**                      u32Config:  配置值，可以取下列几组参数的或
**                      // 1、DMA的FIFO控制
**                      DMA_FIFOMode_Disable              关闭FIFO
**                      DMA_FIFOMode_Enable               打开FIFO
**                      // 2、DMA的FIFO级别控制
**                      DMA_FIFOThreshold_1QuarterFull    1/4  FIFO
**                      DMA_FIFOThreshold_HalfFull        2/4  FIFO
**                      DMA_FIFOThreshold_3QuartersFull   3/4  FIFO
**                      DMA_FIFOThreshold_Full            1    FIFO
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAFIFOConfigSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32Config)
{
    rt_uint32_t u32Temp;
    u32Temp = HWREG32(u32Base + DMA_SxFCR + ((rt_uint32_t)u8Stream)*0x18);
    u32Temp &= ~0xFFFFFFF7;
    u32Temp |= u32Config;
    HWREG32(u32Base + DMA_SxFCR+ ((rt_uint32_t)u8Stream)*0x18) = u32Temp;
}

/*********************************************************************************************************
** Function name:       DMAAddrSet
** Descriptions:        DMA传输存储器地址及外设地址设置
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
**                      u32MemAddr      指定DMA传输的存储器地址
**                      u32PeriAddr     指定DMA传输的外设地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAAddrSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32MemAddr, rt_uint32_t u32PeriAddr)
{
    HWREG32(u32Base + DMA_SxPAR + ((rt_uint32_t)u8Stream)*0x18) = u32PeriAddr;
    HWREG32(u32Base + DMA_SxMOAR + ((rt_uint32_t)u8Stream)*0x18) = u32MemAddr;
}

/*********************************************************************************************************
** Function name:       DMAMemoryAddrSet
** Descriptions:        DMA传输存储器地址设置，Double buffer模式下使用，可以选择使用的缓冲区
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
**                      u8BufferNumber：指定设置的缓冲区，可以为0或者1
**                      u32MemAddr      指定DMA传输的存储器地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAMemoryAddrSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, 
                      rt_uint8_t u8BufferNumber, rt_uint32_t u32MemAddr)
{
  if(u8BufferNumber == 0) {
    HWREG32(u32Base + DMA_SxMOAR + ((rt_uint32_t)u8Stream)*0x18) = u32MemAddr;
  } else {
    HWREG32(u32Base + DMA_SxM1AR + ((rt_uint32_t)u8Stream)*0x18) = u32MemAddr;
  }
}


/*********************************************************************************************************
** Function name:       DMAMemoryAddrGet
** Descriptions:        DMA传输存储器地址获取
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
**                      u8BufferNumber：指定设置的缓冲区，可以为0或者1
**                      u32MemAddr      指定DMA传输的存储器地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t DMAMemoryAddrGet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint8_t u8BufferNumber)
{
  if(u8BufferNumber == 0) {
    return HWREG32(u32Base + DMA_SxMOAR + ((rt_uint32_t)u8Stream)*0x18);
  } else {
    return HWREG32(u32Base + DMA_SxM1AR + ((rt_uint32_t)u8Stream)*0x18);
  }
}

/*********************************************************************************************************
** Function name:       DMACurrentMemoryTargetSet
** Descriptions:        双缓冲下配置当前使用的DMA缓冲区，必须在DMA Disable下调用
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
**                      u8BufferNumber：指定设置的缓冲区，可以为0或者1
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMACurrentMemoryTargetSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint8_t u8BufferNumber)
{
  if(u8BufferNumber == 0) {
     HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) &= ~(0x00080000UL);
  } else {
     HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) |= 0x00080000UL;
  }
}

/*********************************************************************************************************
** Function name:       DMACurrentMemoryTargetGet
** Descriptions:        双缓冲下获取当前使用的DMA缓冲区
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
** output parameters:   NONE
** Returned value:      0或1
*********************************************************************************************************/
uint8_t DMACurrentMemoryTargetGet(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
  if(HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) & 0x00080000UL) {
    return 1;
  } else {
    return 0;
  }
}

/*********************************************************************************************************
** Function name:       DMABufferSizeSet
** Descriptions:        DMA模块传输数据缓冲区大小设置
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
**                      u16Size         指定传输的大小
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMABufferSizeSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint16_t u16Size)
{
    HWREG32(u32Base + DMA_SxNDTR  + ((rt_uint32_t)u8Stream)*0x18) = u16Size;
}

/*********************************************************************************************************
** Function name:       DMAEnable
** Descriptions:        DMA模块使能
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAEnable(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    HWREG32(u32Base + DMA_SxCR+ ((rt_uint32_t)u8Stream)*0x18) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       DMADisable
** Descriptions:        DMA模块禁能
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMADisable(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    HWREG32(u32Base + DMA_SxCR+ ((rt_uint32_t)u8Stream)*0x18) &= 0xFFFFFFFE;
}

/*********************************************************************************************************
** Function name:       DMAPeriphIncOffsetSizeConfig
** Descriptions:        Configures, when the PINC (Peripheral Increment address mode) bit is
**                      set, if the peripheral address shouled be incremented with the data
**                      size (configured with PSIZE bits) or by a fixed offset equal to 4
**                      (32-bit aligned addresses).
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
**                      u32Config       DMA外设地址增长对齐配置参数，可以去下列值中的一个：
**                      DMA_PINCOS_Psize            按照外设增长PSIZE对齐
**                      DMA_PINCOS_WordAligned      按照字对齐(4字节)
** output parameters:   NONE
** Returned value:      NONE
** This function has no effect if the Peripheral Increment mode is disabled.
*********************************************************************************************************/
void DMAPeriphIncOffsetSizeConfig(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32Config)
{
    rt_uint32_t u32Temp;

    u32Temp = HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18);
    u32Temp &= ~((rt_uint32_t)0x00008000);
    u32Temp |= u32Config;
    HWREG32(u32Base + DMA_SxCR+ ((rt_uint32_t)u8Stream)*0x18) = u32Temp;
}
/*********************************************************************************************************
** Function name:       DMAFlowControllerConfig
** Descriptions:        Configures, when the DMAy Streamx is disabled, the flow controller for
**                      the next transactions (Peripheral or Memory).
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
**                      u32Config       DMA外设地址增长对齐配置参数，可以去下列值中的一个：
**                      DMA_FlowCtrl_DMA            DMA流控
**                      DMA_FlowCtrl_Peripheral     外设流控
** output parameters:   NONE
** Returned value:      NONE
**                      Before enabling this feature, check if the used peripheral supports
**                      the Flow Controller mode or not.
*********************************************************************************************************/
void DMAFlowControllerConfig(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32Config)
{
    rt_uint32_t u32Temp;

    u32Temp = HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18);
    u32Temp &= ~(((rt_uint32_t)0x00000020));
    u32Temp |= u32Config;
    HWREG32(u32Base + DMA_SxCR+ ((rt_uint32_t)u8Stream)*0x18) = u32Temp;
}

/*********************************************************************************************************
** Function name:       DMACurrDataCounterGet
** Descriptions:        获取当前的DMA计数器的值
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
** output parameters:   NONE
** Returned value:      当前的DMA计数器的值
*********************************************************************************************************/
rt_uint16_t DMACurrDataCounterGet(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    return ((rt_uint16_t)(HWREG32(u32Base + DMA_SxNDTR  + ((rt_uint32_t)u8Stream)*0x18)));
}

/*********************************************************************************************************
** Function name:       DMACurrDataCounterSet
** Descriptions:        设置当前的DMA计数器的值
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
**                      u16Count        待设置的计数器的值
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMACurrDataCounterSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint16_t u16Count)
{
    HWREG32(u32Base + DMA_SxNDTR  + ((rt_uint32_t)u8Stream)*0x18) = u16Count;
}

/*********************************************************************************************************
** Function name:       DMAIsEnabled
** Descriptions:        判断DMA是否使能传输，the status of EN bit for the specified DMAy Streamx.
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
** output parameters:   NONE
** Returned value:      RT_TRUE:   使能了DMA传输
**                      RT_FALSE:  禁制了DMA传输
*********************************************************************************************************/
rt_bool_t DMAIsEnabled(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    if(HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) & 0x00000001) {
        return RT_TRUE;
    } else {
        return RT_FALSE;
    }
}

/*********************************************************************************************************
** Function name:       DMAIntEnable
** Descriptions:        DMA中断使能
** input parameters:    u32Base             DMA模块的基地址
**                      u8Stream            DMA的Stream编号，为0-7
**                      u32Flags            指定使能的中断，可以取下列值的或
**                      DMA_INT_CONFIG_TC          传输完成中断
**                      DMA_INT_CONFIG_HT          传输一半时的中断
**                      DMA_INT_CONFIG_TE          传输错误中断
**                      DMA_INT_CONFIG_DME         直接模式错误中断
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAIntEnable(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32Flags)
{
    HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) |= u32Flags;
}

/*********************************************************************************************************
** Function name:       DMAIntDisable
** Descriptions:        DMA中断禁能
** input parameters:    u32Base          DMA模块的基地址
**                      u8Stream         DMA的Stream编号，为0-7
**                      u32Flags         指定使能的中断，可以取下列值的或
**                      DMA_INT_CONFIG_TC          传输完成中断
**                      DMA_INT_CONFIG_HT          传输一半时的中断
**                      DMA_INT_CONFIG_TE          传输错误中断
**                      DMA_INT_CONFIG_DME         直接模式错误中断
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAIntDisable(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32Flags)
{
    HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) &= ~u32Flags;
}

/*********************************************************************************************************
** Function name:       DMAFIFOIntEnable
** Descriptions:        DMA的FIFO中断使能
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAFIFOIntEnable(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    HWREG32(u32Base + DMA_SxFCR + ((rt_uint32_t)u8Stream)*0x18) |= (rt_uint32_t)0x00000080;
}

/*********************************************************************************************************
** Function name:       DMAFIFOIntDisable
** Descriptions:        DMA的FIFO中断禁能
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAFIFOIntDisable(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    HWREG32(u32Base + DMA_SxFCR + ((rt_uint32_t)u8Stream)*0x18) &= ~((rt_uint32_t)0x00000080);
}

/*********************************************************************************************************
** Function name:       DMAIntStatus
** Descriptions:        DMA中断状态获取
** input parameters:    u32Base         DMA模块的基地址
**                      u8Stream        DMA的Stream编号，为0-7
** output parameters:   NONE
** Returned value:      当前通道的中断状态，可以为下列几个值或者其或运算
**                      DMA_INT_STATUS_TC          传输完成中断
**                      DMA_INT_STATUS_HT          传输一半时的中断
**                      DMA_INT_STATUS_TE          传输错误中断
**                      DMA_INT_STATUS_DME         直接模式错误中断
**                      DMA_INT_STATUS_FIFO        FIFO错误中断
*********************************************************************************************************/
rt_uint32_t DMAIntStatus(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    rt_uint8_t u8RegOdd = u8Stream / 4;
    rt_uint8_t u8RegOffset = u8RegOdd * 4;
    u8RegOdd =  u8Stream % 4;
    rt_uint32_t u32Temp;

    u32Temp = HWREG32(u32Base + DMA_LISR + u8RegOffset);
    switch(u8RegOdd)
    {
        case 0:
            u32Temp = (u32Temp & (rt_uint32_t)0x0000003D);
            break;
        case 1:
            u32Temp = ((u32Temp >> 6) & (rt_uint32_t)0x0000003D);
            break;
        case 2:
            u32Temp = ((u32Temp >> 16) & (rt_uint32_t)0x0000003D);
            break;
        case 3:
            u32Temp = ((u32Temp >> 22) & (rt_uint32_t)0x0000003D);
            break;
        default:
            break;
    }

    return (u32Temp);
}

/*********************************************************************************************************
** Function name:       DMAIntStatus
** Descriptions:        DMA中断禁能
** input parameters:    u32Base          DMA模块的基地址
**                      u8Stream         DMA的Stream编号，为0-7
**                      u32IntFlags      当前通道的中断状态，可以为下列几个值的或
**                      DMA_INT_STATUS_TC          传输完成中断
**                      DMA_INT_STATUS_HT          传输一半时的中断
**                      DMA_INT_STATUS_TE          传输错误中断
**                      DMA_INT_STATUS_DME         直接模式错误中断
**                      DMA_INT_STATUS_FIFO        FIFO错误中断
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAIntClear(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32IntFlags)
{
    rt_uint8_t u8RegOdd = u8Stream / 4;
    rt_uint8_t u8RegOffset = u8RegOdd * 4;
    u8RegOdd =  u8Stream % 4;
    rt_uint32_t u32Temp;

    u32Temp = HWREG32(u32Base + DMA_LIFCR + u8RegOffset);
    switch(u8RegOdd)
    {
        case 0:
            u32Temp |= (u32IntFlags & (rt_uint32_t)0x0000003D);
            break;
        case 1:
            u32Temp |= ((u32IntFlags & (rt_uint32_t)0x0000003D) << 6);
            break;
        case 2:
            u32Temp |= ((u32IntFlags & (rt_uint32_t)0x0000003D) << 16);
            break;
        case 3:
            u32Temp |= ((u32IntFlags & (rt_uint32_t)0x0000003D) << 22);
            break;
        default:
            break;
    }
    HWREG32(u32Base + DMA_LIFCR + u8RegOffset) = u32Temp;

}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
