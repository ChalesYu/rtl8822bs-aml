/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_qspi.c
** Last modified Date:  2019-03-22
** Last Version:        v1.0
** Description:         quad spi模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2019-03-22
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
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_qspi.h"


/*********************************************************************************************************
** Function name:       QSPIInit
** Descriptions:        QSPI模块工作模式配置
** input parameters:    u32Base      QSPI模块的基地址
**                      u32Config:   基本配置参数，可以取下列几组数字的或
**                      
**                      // QSPI波特率分频器选择
**                      QSPI_CLOCK_PRESCALER(x)         //  工作时钟分频，取1-256
**                      // Polling match mode设置
**                      QSPI_AND_MATCH_MODE             // Polling match mode，与的方式
**                      QSPI_OR_MATCH_MODE              // Polling match mode，或的方式
**                      // Pool mode stop停止条件设置
**                      QSPI_POLL_STOP_BY_DISABLE_QSPI  // Pool mode stop，通过禁止QSPI
**                      QSPI_POLL_STOP_BY_MATCH         // Pool mode stop，通过Poll Match
**                      // FIFO threshold level设置
**                      QSPI_FIFO_THRESHOLD(x)          // FIFO深度配置，取1-32
**                      // Flash Memory选择，当使用双通道Flash时用到
**                      QSPI_SELECTED_FLASH1            // 选择Flash1
**                      QSPI_SELECTED_FLASH2            // 选择Flash2
**                      //  双通道Flash是否使能配置
**                      QSPI_DUAL_FLASH_DISABLE         // 双通道Flash禁止
**                      QSPI_DUAL_FLASH_ENABLE          // 双通道Flash使能
**                      // 采样移位配置， DDR模式必须配置为NO SHIFT
**                      QSPI_SAMPLE_NO_SHIFT            // 采样不移位
**                      QSPI_SAMPLE_HALF_SHIFT          // 采用移位半个时钟， DDR模式不可用
**                      // 超时计数器使能配置
**                      QSPI_TIMEOUT_DISABLE            // Timeout counter禁止
**                      QSPI_TIMEOUT_ENABLE             // Timeout counter使能
**                      
** output parameters:   NONE
** Returned value:      NONE
** 初始化时调用1次，调用后QSPI是禁止的
*********************************************************************************************************/
void QSPIInit(rt_uint32_t u32Base,rt_uint32_t u32Config)
{ 
    // 先清空配置
    HWREG32(u32Base + QUADSPI_CR) = 0;
    HWREG32(u32Base + QUADSPI_CR) = u32Config;
}

/*********************************************************************************************************
** Function name:       QSPIInit
** Descriptions:        QSPI模块工作模式配置
** input parameters:    u32Base      QSPI模块的基地址
**                      u32Config:   基本配置参数，可以取下列几组数字的或
**                      
**                      // Flash存储器容量配置
**                      QSPI_FLASH_MEMORY_SIZE(x)       // 存储器容量，取1-32，即2个字节到4G字节的容量
**                      // 片选信号高电平实际配置
**                      QSPI_CS_HIGH_TIME(x)            // 片选高电平维持时间，取1-8
**                      // QSPI工作模式配置
**                      QSPI_CKMODE_0                   // 工作模式0
**                      QSPI_CKMODE_3                   // 工作模式3
**                      
** output parameters:   NONE
** Returned value:      NONE
** 必须在禁止QSPI条件下调用
*********************************************************************************************************/
void QSPIDeviceConfig(rt_uint32_t u32Base,rt_uint32_t u32Config)
{ 
    // 先清空配置
    HWREG32(u32Base + QUADSPI_DCR) = 0;
    HWREG32(u32Base + QUADSPI_DCR) = u32Config;
}

/*********************************************************************************************************
** Function name:       QSPIInit
** Descriptions:        QSPI模块工作模式配置
** input parameters:    u32Base      QSPI模块的基地址
**                      u32Config:   基本配置参数，可以取下列几组数字的或
**                      
**                      // DDR使能控制
**                      QSPI_DDR_MODE_DISABLE            // 禁止DDR模式
**                      QSPI_DDR_MODE_ENABLE             // 使能DDR模式
**                      // DDR Hold控制
**                      QSPI_DDR_HOLD_BY_ANALOG_DELAY   // 靠模拟方式延时DDR Hold
**                      QSPI_DDR_HOLD_BY_CLOCK          // 靠时钟信号延时DDR Hold
**                      // Runing Mode配置
**                      QSPI_NORMAL_CLOCK_MODE          // 正常模式
**                      QSPI_FREE_CLOCK_MODE            // Free running clock mode
**                      // 发送指令控制
**                      QSPI_SEND_INSTRUCTION_EVERY     // 每次传输都发送指令
**                      QSPI_SEND_INSTRUCTION_FIRST     // 只有第一次通信才发送指令
**                      //  Functional mode配置
**                      QSPI_INDIRECT_WRITE_MODE        // 间接写模式 
**                      QSPI_INDIRECT_READ_MODE         // 间接读模式
**                      QSPI_AUTO_POLLING_MODE          // 自动轮训模式
**                      QSPI_MEMMORY_MAP_MODE           // 存储器映射模式
**                      // 数据格式控制
**                      QSPI_DATA_MODE_NO               // 无数据传输
**                      QSPI_DATA_MODE_1LINE            // 数据传输在one line
**                      QSPI_DATA_MODE_2LINE            // 数据传输在double line
**                      QSPI_DATA_MODE_4LINE            // 数据传输在Four line
**                      // Dummy周期配置
**                      QSPI_DUMMY_CYCLES(x)            // Dummy周期，取0-31
**                      //  Alternate bytes size设置
**                      QSPI_ALTERNATE_BYTES_8BIT       // Alternate bytes 8Bit
**                      QSPI_ALTERNATE_BYTES_16BIT      // Alternate bytes 16Bit
**                      QSPI_ALTERNATE_BYTES_24BIT      // Alternate bytes 24Bit
**                      QSPI_ALTERNATE_BYTES_32BIT      // Alternate bytes 32Bit
**                      // Alternate Mode设置
**                      QSPI_ALTERNATE_MODE_NO          // 无Alternate Bytes传输
**                      QSPI_ALTERNATE_MODE_1LINE       // Alternate Bytes传输在one line
**                      QSPI_ALTERNATE_MODE_2LINE       // Alternate Bytes传输在double line
**                      QSPI_ALTERNATE_MODE_4LINE       // Alternate Bytes传输在Four line
**                      // Address bytes size设置
**                      QSPI_ADDRESS_BYTES_8BIT         // 地址 bytes 8Bit
**                      QSPI_ADDRESS_BYTES_16BIT        // 地址 bytes 8Bit
**                      QSPI_ADDRESS_BYTES_24BIT        // 地址 bytes 8Bit
**                      QSPI_ADDRESS_BYTES_32BIT        // 地址 bytes 8Bit
**                      // Address Mode设置
**                      QSPI_ADDRESS_MODE_NO            // 无Address Bytes传输
**                      QSPI_ADDRESS_MODE_1LINE         // Address Bytes传输在one line
**                      QSPI_ADDRESS_MODE_2LINE         // Address Bytes传输在double line
**                      QSPI_ADDRESS_MODE_4LINE         // Address Bytes传输在Four line
**                      // Instruction Mode设置
**                      QSPI_INSTRUCTION_MODE_NO        // 无Instruction Bytes传输
**                      QSPI_INSTRUCTION_MODE_1LINE     // Instruction Bytes传输在one line
**                      QSPI_INSTRUCTION_MODE_2LINE     // Instruction Bytes传输在double line
**                      QSPI_INSTRUCTION_MODE_4LINE     // Instruction Bytes传输在Four line
**                      // 发送的Instruction
**                      QSPI_INSTRUCTION(x)             // 发送的指令，具体参考Memory的手册
**                      
** output parameters:   NONE
** Returned value:      NONE
** 必须在禁止QSPI条件下调用
*********************************************************************************************************/
void QSPICommunicationConfig(rt_uint32_t u32Base,rt_uint32_t u32Config)
{ 
    // 先清空配置
    HWREG32(u32Base + QUADSPI_CCR) = 0;
    HWREG32(u32Base + QUADSPI_CCR) = u32Config;
}

/*********************************************************************************************************
** Function name:       QSPIEnable
** Descriptions:        QSPI工作使能
** input parameters:    u32Base      QSPI模块的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void QSPIEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + QUADSPI_CR) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       QSPIDisable
** Descriptions:        QSPI工作禁能
** input parameters:    u32Base      QSPI模块的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void QSPIDisable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + QUADSPI_CR) &= ~((rt_uint32_t)(0x00000001));
}

/*********************************************************************************************************
** Function name:       QSPIAbortRequest
** Descriptions:        QSPI终止传输
** input parameters:    u32Base      QSPI模块的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void QSPIAbortRequest(rt_uint32_t u32Base)
{
    HWREG32(u32Base + QUADSPI_CR) |= 0x00000002;
    HWREG32(u32Base + QUADSPI_CR) &= ~((rt_uint32_t)(0x00000002));
}

/*********************************************************************************************************
** Function name:       QSPITransfer
** Descriptions:        QSPI一次传输
** input parameters:    u32Base      QSPI模块的基地址
**                      u32Config:   传输配置
**                      u32Address:  传输的flash地址
**                      size：       传输数据大小
**                      data:        传输的数据
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void QSPITransfer(rt_uint32_t u32Base, rt_uint32_t u32Config, 
                  rt_uint32_t u32Address, rt_uint32_t size, void * data)
{
  while(0x00000020 & HWREG32(u32Base + QUADSPI_SR));
  /*set transfer size*/
  HWREG32(u32Base + QUADSPI_DLR) = size - 1;
  /*alternate byte set. Currently not supported*/
  HWREG32(u32Base + QUADSPI_ABR) = 0xFFFFFFFF;
  /*set transfer this will start the transfer if the IMODE != IMODE_NO*/
  HWREG32(u32Base + QUADSPI_CCR) = u32Config;
  /*Set address*/
  HWREG32(u32Base + QUADSPI_AR) = u32Address;
  
  /*Read or write data. Only if Size > 0*/
  for(uint32_t i = 0;i < size;++i)
  { /*Wait fifo threshold*/
    while(!(0x00000004 & HWREG32(u32Base + QUADSPI_SR)));
    if(0x00000000 == (u32Config & 0x0C000000))
    {/*Write */
      HWREG8(u32Base + QUADSPI_DR) = ((rt_uint8_t *)data)[i];
    }    
    else
    {/*Read*/
      ((rt_uint8_t *)data)[i] = HWREG8(u32Base + QUADSPI_DR);
    }
  }
  while(!(0x00000002 & HWREG32(u32Base + QUADSPI_SR)));
  HWREG32(u32Base + QUADSPI_FCR) |= 0x00000002;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
