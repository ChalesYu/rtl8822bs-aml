/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_bdma.c
** Last modified Date:  2016-01-19
** Last Version:        v1.0
** Description:         dma模块寄存器封装函数实现声明，适用于STM32L4xx
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2016-01-19
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
#include "hal/arch/inc/arch_io_bdma.h"


/*********************************************************************************************************
** Function name:       BDMAModeSet
** Descriptions:        BDMA模块工作模式配置
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
**                      ulConfig        DMA配置参数，可以取下列几组参数的或
**                      BDMA_DIR_M_READ            //  从存储器读，反之为外设读
**                      BDMA_CIRC_EN               //  执行循环操作模式
**                      BDMA_PINC_EN               //  使能外设地址增量模式
**                      BDMA_MINC_EN               //  使能存储器地址增量模式
**                      //  下列四个必须取其中的一个与其它值进行或运算
**                      BDMA_PSIZE_8               //  外设数据宽度为8位
**                      BDMA_PSIZE_16              //  外设数据宽度为16位
**                      BDMA_PSIZE_32              //  外设数据宽度为32位
**                      BDMA_PSIZE_RESERVED        //  外设数据宽度保留
**                      //  下列四个必须取其中的一个与其它值进行或运算
**                      BDMA_MSIZE_8               //  存储器数据宽度为8位
**                      BDMA_MSIZE_16              //  存储器数据宽度为16位
**                      BDMA_MSIZE_32              //  存储器数据宽度为32位
**                      BDMA_MSIZE_RESERVED        //  存储器数据宽度保留
**                      //  下列四个必须取其中的一个与其它值进行或运算
**                      BDMA_PL_LOW                //  通道优先级为低
**                      BDMA_PL_MEDIUM             //  通道优先级为中
**                      BDMA_PL_HIGH               //  通道优先级为高
**                      BDMA_PL_HITHEST            //  通道优先级为最高
**                      //  下列2个必须取其中的一个与其它值进行或运算
**                      BDMA_MODE_NORMAL           //  正常传输模式
**                      BDMA_MODE_CIRCULAR         //  循环传输模式
**
**                      BDMA_MTOM_EN               //  存储器到存储器传输模式使能
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAModeSet(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulConfig)
{
    rt_uint32_t ulTemp;
    ulTemp = HWREG32(ulBase + BDMA_CCR1 + (ucChannel)*0x14);
    ulTemp &= 0xFFFF800F;
    ulTemp |= ulConfig;
    HWREG32(ulBase + BDMA_CCR1+ (ucChannel)*0x14)= ulTemp;
}

/*********************************************************************************************************
** Function name:       BDMAAddrSet
** Descriptions:        BDMA传输存储器地址及外设地址设置
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
**                      ulMemAddr       指定DMA传输的存储器地址
**                      ulPeriAddr      指定DMA传输的外设地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAAddrSet(rt_uint32_t ulBase, rt_uint8_t ucChannel, 
                rt_uint32_t ulMemAddr, rt_uint32_t ulPeriAddr)
{
    HWREG32(ulBase + BDMA_CPAR1 + (ucChannel)*0x14)= ulPeriAddr;
    HWREG32(ulBase + BDMA_CM0AR1 + (ucChannel)*0x14)= ulMemAddr;
}

/*********************************************************************************************************
** Function name:       BDMAMemAddr1Set
** Descriptions:        BDMA传输存储器地址1设置
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
**                      ulMemAddr       指定DMA传输的存储器地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAMemAddr1Set(rt_uint32_t ulBase, rt_uint8_t ucChannel, rt_uint32_t ulMemAddr)
{
    HWREG32(ulBase + BDMA_CM1AR1 + (ucChannel)*0x14)= ulMemAddr;
}

/*********************************************************************************************************
** Function name:       BDMABufferSizeSet
** Descriptions:        BDMA模块传输数据缓冲区大小设置
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
**                      usSize          指定传输的大小
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMABufferSizeSet(rt_uint32_t ulBase, rt_uint8_t ucChannel, rt_uint16_t usSize)
{
    HWREG32(ulBase + BDMA_CNDTR1 + (ucChannel)*0x14)= usSize;
}

/*********************************************************************************************************
** Function name:       BDMABufferSizeGet
** Descriptions:        BDMA模块传输数据缓冲区大小设置
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t BDMABufferSizeGet(rt_uint32_t ulBase, rt_uint8_t ucChannel)
{
    return HWREG32(ulBase + BDMA_CNDTR1 + (ucChannel)*0x14);
}

/*********************************************************************************************************
** Function name:       BDMAEnable
** Descriptions:        BDMA模块使能
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAEnable(rt_uint32_t ulBase, rt_uint8_t ucChannel)
{
    HWREG32(ulBase + BDMA_CCR1 + (ucChannel)*0x14) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       BDMAModeSet
** Descriptions:        BDMA模块禁能
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMADisable(rt_uint32_t ulBase, rt_uint8_t ucChannel)
{
    HWREG32(ulBase + BDMA_CCR1 + (ucChannel)*0x14) &= 0xFFFFFFFE;
}

/*********************************************************************************************************
** Function name:       BDMAIsEnabled
** Descriptions:        判断BDMA是否使能传输
** input parameters:    u32Base         DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
** output parameters:   NONE
** Returned value:      RT_TRUE:   使能了DMA传输
**                      RT_FALSE:  禁制了DMA传输
*********************************************************************************************************/
rt_bool_t BDMAIsEnabled(rt_uint32_t u32Base, rt_uint8_t ucChannel)
{
    if(HWREG32(u32Base + BDMA_CCR1 + (ucChannel)*0x14) & 0x00000001) {
        return RT_TRUE;
    } else {
        return RT_FALSE;
    }
}

/*********************************************************************************************************
** Function name:       DMAIntEnable
** Descriptions:        DMA中断使能
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
**                      ulFlags         指定使能的中断，可以取下列值的或
**                      DMA_INT_TC      //  传输完成中断
**                      DMA_INT_HT      //  传输一半时的中断
**                      DMA_INT_TE      //  传输错误中断
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAIntEnable(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulFlags)
{
    HWREG32(ulBase + BDMA_CCR1 + (ucChannel)*0x14) |= ulFlags;
}

/*********************************************************************************************************
** Function name:       BDMAIntDisable
** Descriptions:        BDMA中断禁能
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
**                      ulFlags         指定使能的中断，可以取下列值的或
**                      BDMA_INT_TC      //  传输完成中断
**                      BDMA_INT_HT      //  传输一半时的中断
**                      BDMA_INT_TE      //  传输错误中断
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAIntDisable(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulFlags)
{
    HWREG32(ulBase + BDMA_CCR1 + (ucChannel)*0x14) &= ~ulFlags;
}

/*********************************************************************************************************
** Function name:       BDMAIntStatus
** Descriptions:        BDMA中断状态获取
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
** output parameters:   NONE
** Returned value:      当前通道的中断状态，可以为下列三个值或者其或运算后，再与DMA_INT_GLOBAL的或，
**                      BDMA_INT_GLOBAL，表示全局中断状态，只要下列三个值有一个成立，则就成立
**                      BDMA_INT_TC      //  传输完成中断
**                      BDMA_INT_HT      //  传输一半时的中断
**                      BDMA_INT_TE      //  传输错误中断
*********************************************************************************************************/
rt_uint32_t BDMAIntStatus(rt_uint32_t ulBase, rt_uint8_t ucChannel)
{
    rt_uint32_t ulTemp;
    ulTemp = HWREG32(ulBase + BDMA_ISR);
    ulTemp >>= ((ucChannel) * 4);
    
    return (ulTemp);
}

/*********************************************************************************************************
** Function name:       BDMAIntStatus
** Descriptions:        BDMA中断清除
** input parameters:    ulBase          DMA模块的基地址
**                      ucChannel       DMA通道编号，DMA1为0-7，DMA2为0-7
**                      ulIntFlags      当前通道的中断状态，可以为下列三个值的或后，再与DMA_INT_GLOBAL的或，
**                      BDMA_INT_GLOBAL，表示全局中断状态，只要下列三个值有一个成立，则就成立
**                      BDMA_INT_TC      //  传输完成中断
**                      BDMA_INT_HT      //  传输一半时的中断
**                      BDMA_INT_TE      //  传输错误中断
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAIntClear(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulIntFlags)
{
    rt_uint32_t ulTemp = (ulIntFlags << ((ucChannel) * 4));
    
    HWREG32(ulBase + BDMA_IFCR) |= ulTemp;
    
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
