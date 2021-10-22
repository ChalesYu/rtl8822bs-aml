/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_spi.c
** Last modified Date:  2019-03-18
** Last Version:        v1.0
** Description:         spi及i2s模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2019-03-18
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
#include "hal/arch/inc/arch_io_spi.h"


/*********************************************************************************************************
** Function name:       SPIInit
** Descriptions:        SPI模块工作模式配置
** input parameters:    u32Base      SPI模块的基地址
**                      u32Config1:    配置参数1，可以取下列几组数字的或
**                      
**                      // SPI波特率分频器选择
**                      SPI_Baud_Prescaler_2            //  波特率分配系数为2
**                      SPI_Baud_Prescaler_4            //  波特率分配系数为4
**                      SPI_Baud_Prescaler_8            //  波特率分配系数为8
**                      SPI_Baud_Prescaler_16           //  波特率分配系数为16
**                      SPI_Baud_Prescaler_32           //  波特率分配系数为32
**                      SPI_Baud_Prescaler_64           //  波特率分配系数为64
**                      SPI_Baud_Prescaler_128          //  波特率分配系数为128
**                      SPI_Baud_Prescaler_256          //  波特率分配系数为256
**                        
**                      // 硬件CRC计算控制
**                      SPI_CRC_ENABLE                  // 使能CRC校验
**                      SPI_CRC_DISABLE                 // 禁止CRC校验
**                      
**                      // 计算的CRC Length，必须是DSIZE的整数倍,x取值4-32
**                      SPI_CRC_LENGTH_BIT(x) 
**                      
**                      // 数据位长度，x取值4-32
**                      SPI_DATA_LEGNTH_BIT(x) 
**                      
**                      u32Config2:    配置参数1，可以取下列几组数字的或
**                      // SSOE使能控制
**                      SPI_SSOE_ENABLE                  // SSOE使能
**                      SPI_SSOE_DISABLE                 // SSOE禁止  
**                        
**                      // SS信号有效电平配置
**                      SPI_SSIOP_HIGH                  // SS信号高有效
**                      SPI_SSIOP_LOW                   // SS信号低有效
**                       
**                      // 无数据收发时，SPI_CLK极性选择
**                      SPI_CPOL_LOW                    // CPOL低
**                      SPI_CPOL_HIGH                   // CPOL高
**                       
**                      // 时钟采样的相位选择
**                      SPI_CPHA_1EDGE                  // 第1个时钟采集数据
**                      SPI_CPHA_2EDGE                  // 第2个时钟采集数据
**                       
**                      // 发送数据位从MSB开始，还是从LSB开始选择
**                      SPI_FIRST_MSB                   // 先传输高位
**                      SPI_FIRST_LSB                   // 先传输低位
**                      
**                      //  工作主从选择
**                      SPI_MODE_MASTER                 // 主机模式
**                      SPI_MODE_SLAVE                  // 从机模式
**                       
**                      // SPI协议控制
**                      SPI_PROTOCOL_MOTOROLA           // motorola格式的协议
**                      SPI_PROTOCOL_TI                 // TI格式的协议
**                      SPI_PROTOCOL_OTHERS             // 其它协议
** output parameters:   NONE
** Returned value:      NONE
** 注意：寄存器写的顺序很重要，顺序错误会导致寄存器写入无效
*********************************************************************************************************/
void SPIInit(rt_uint32_t u32Base,rt_uint32_t u32Config1,rt_uint32_t u32Config2)
{
    rt_uint32_t u32Temp;
    
    if(u32Config2 & SPI_MODE_MASTER) {
      HWREG32(u32Base + SPI2S_CR1) |= ((1UL << 12));
    }
    
    u32Temp = HWREG32(u32Base + SPI_CFG1);
    
    u32Temp &= 0x0007C000; //　先清除在配置
    u32Temp |= u32Config1;
    
    /* Write to SPI_CFG1 */
    HWREG32(u32Base + SPI_CFG1) = u32Temp;
    
    u32Temp = HWREG32(u32Base + SPI_CFG2);
    u32Temp = 0;
    u32Temp |= u32Config2;
    if(u32Config2 & SPI_MODE_MASTER) {
      u32Temp |= ((1UL << 26) | (0UL << 29) | (1UL << 31));
    }
    HWREG32(u32Base + SPI_CFG2) = u32Temp;
}

///*********************************************************************************************************
//** Function name:       I2SInit
//** Descriptions:        I2S模式初始化
//** input parameters:    u32Base      I2S模块的基地址
//**                      u32Config    I2S模块工作模块初始化配置，低16位用于配置I2S_CFGR寄存器；搞16位用于
//**                                   配置I2SPRR寄存器，可以取下列几组数字的或
//**                                   SPI_I2S_Mode参数
//**                                   I2S_Mode_SlaveTx                //  从机发送
//**                                   I2S_Mode_SlaveRx                //  从机接收
//**                                   I2S_Mode_MasterTx               //  主机发送
//**                                   I2S_Mode_MasterRx               //  主机接收 
//**
//**                                   SPI_I2S_Standard参数
//**                                   I2S_Standard_Phillips           //飞利浦标准
//**                                   I2S_Standard_MSB                // MSB标准
//**                                   I2S_Standard_LSB                // LSB标准
//**                                   I2S_Standard_PCMShort           // PCM Short标准
//**                                   I2S_Standard_PCMLong            // PCM Long标准
//  
//**                                   SPI_I2S_Data_Format参数
//**                                   I2S_DataFormat_16b              // 16位格式
//**                                   I2S_DataFormat_16bextended      // 扩展16位格式
//**                                   I2S_DataFormat_24b              // 24位格式
//**                                   I2S_DataFormat_32b              // 32位格式
//  
//**                                   SPI_I2S_Clock_Polarity 参数
//**                                   I2S_CPOL_Low                    // 稳定状态电平低
//**                                   I2S_CPOL_High                   // 稳定状态电平高
//  
//**                                   SPI_I2S_MCLK_Output参数，使用时需要 右移16位
//**                                   I2S_MCLKOutput_Enable           // MCLK输出使能
//**                                   2S_MCLKOutput_Disable           // MCLK输出禁止
//**                      i2sclk       I2S时钟，当为0时，使用内部时钟；反之，使用频率为该值的外部时钟
//**                      audio_freq   音频采样频率，建议去下列值：
//**                                   I2S_AudioFreq_192k          //  192K
//**                                   I2S_AudioFreq_96k           //  96K
//**                                   I2S_AudioFreq_48k           //  48K
//**                                   I2S_AudioFreq_44k           //  44.1K
//**                                   I2S_AudioFreq_32k           //  32K
//**                                   I2S_AudioFreq_22k           //  22.05K
//**                                   I2S_AudioFreq_16k           //  16K
//**                                   I2S_AudioFreq_11k           //  11.025K
//**                                   I2S_AudioFreq_8k            //  8K
//**                                   I2S_AudioFreq_Default       //  默认值
//** output parameters:   NONE
//** Returned value:      NONE
//*********************************************************************************************************/
//void I2SInit(rt_uint32_t u32Base,rt_uint32_t u32Config, rt_uint32_t i2sclk, rt_uint32_t audio_freq)
//{
//  rt_uint16_t u16I2SCFGR = u32Config & 0xFFFF;
//  rt_uint16_t u16I2SPR = (u32Config >> 16) & 0xFFFF;
//  
//  rt_uint16_t tmpreg = 0, i2sdiv = 2, i2sodd = 0, packetlength = 1;
//  rt_uint32_t tmp = 0;
//  rt_uint32_t pllm = 0, plln = 0, pllr = 0;
//  
//  /*----------------------- SPIx I2SCFGR & I2SPR Configuration -----------------*/
//  /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
//  HWREG16(u32Base + SPI_I2SCFGR) &=  ((rt_uint16_t)0xF040);
//  HWREG16(u32Base + SPI_I2SPR) = 0x00002;
//
//  /* Get the I2SCFGR register value */
//  tmpreg = HWREG16(u32Base + SPI_I2SCFGR);
//  
//  /* If the default value has to be written, reinitialize i2sdiv and i2sodd*/
//  if(audio_freq == I2S_AudioFreq_Default)
//  {
//    i2sodd = (rt_uint16_t)0;
//    i2sdiv = (rt_uint16_t)2;   
//  }
//  /* If the requested audio frequency is not the default, compute the prescaler */
//  else
//  {
//    /* Check the frame length (For the Prescaler computing) *******************/
//    if((u16I2SCFGR & (rt_uint16_t)0x0007) == I2S_DataFormat_16b)
//    {
//      /* Packet length is 16 bits */
//      packetlength = 1;
//    }
//    else
//    {
//      /* Packet length is 32 bits */
//      packetlength = 2;
//    }
//    
//    /* Get I2S source Clock frequency  ****************************************/
//    
//    /* If an external I2S clock has to be used, this define should be set  
//    in the project configuration or in the stm32f4xx_conf.h file */
//    if(i2sclk > 0) {    
//      /* Set external clock as I2S clock source */
//      if ((HWREG32(RCC_BASE + RCC_CFGR) & 0x00800000) == 0)
//      {
//        HWREG32(RCC_BASE + RCC_CFGR) |= (rt_uint32_t)0x00800000;
//      }
//    }
//    else { /* There is no define for External I2S clock source */
//      /* Set PLLI2S as I2S clock source */
//      if ((HWREG32(RCC_BASE + RCC_CFGR) & 0x00800000) != 0)
//      {
//        HWREG32(RCC_BASE + RCC_CFGR) &= ~((rt_uint32_t)0x00800000);
//      }    
//      
//      /* Get the PLLI2SN value */
//      plln = (rt_uint32_t)(((HWREG32(RCC_BASE + RCC_PLLI2SCFGR) & 0x00007FC0) >> 6) & 0x1FF);
//      
//      /* Get the PLLI2SR value */
//      pllr = (rt_uint32_t)(((HWREG32(RCC_BASE + RCC_PLLI2SCFGR) & 0x70000000) >> 28) & 0x07);
//      
//      /* Get the PLLM value */
//      pllm = (rt_uint32_t)(HWREG32(RCC_BASE + RCC_PLLCFGR) & 0x3F);      
//      
//      /* Get the I2S source clock value */
//      i2sclk = (rt_uint32_t)(((RCC_HSE_INPUT_CLOCK / pllm) * plln) / pllr);
//    }
//    
//    /* Compute the Real divider depending on the MCLK output state, with a floating point */
//    if(u32Config & I2S_MCLKOutput_Enable)
//    {
//      /* MCLK output is enabled */
//      tmp = (rt_uint16_t)(((((i2sclk / 256) * 10) / audio_freq)) + 5);
//    }
//    else
//    {
//      /* MCLK output is disabled */
//      tmp = (rt_uint16_t)(((((i2sclk / (32 * packetlength)) *10 ) / audio_freq)) + 5);
//    }
//    
//    /* Remove the flatting point */
//    tmp = tmp / 10;  
//    
//    /* Check the parity of the divider */
//    i2sodd = (rt_uint16_t)(tmp & (rt_uint16_t)0x0001);
//    
//    /* Compute the i2sdiv prescaler */
//    i2sdiv = (rt_uint16_t)((tmp - i2sodd) / 2);
//    
//    /* Get the Mask for the Odd bit (SPI_I2SPR[8]) register */
//    i2sodd = (rt_uint16_t) (i2sodd << 8);
//  }
//  
//  /* Test if the divider is 1 or 0 or greater than 0xFF */
//  if ((i2sdiv < 2) || (i2sdiv > 0xFF))
//  {
//    /* Set the default values */
//    i2sdiv = 2;
//    i2sodd = 0;
//  }
//  
//  /* Write to SPIx I2SPR register the computed value */
//  HWREG16(u32Base + SPI_I2SPR) = (rt_uint16_t)((rt_uint16_t)i2sdiv | (rt_uint16_t)(i2sodd | u16I2SPR));
//  
//  /* Configure the I2S with the SPI_InitStruct values */
//  tmpreg |= (rt_uint16_t)((rt_uint16_t)0x0800 | u16I2SCFGR);
//  
//  /* Write to SPIx I2SCFGR */  
//  HWREG16(u32Base + SPI_I2SCFGR) = tmpreg;
//}

/*********************************************************************************************************
** Function name:       SPIEnable
** Descriptions:        SPI工作使能
** input parameters:    u32Base      SPI模块的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SPIEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + SPI2S_CR1) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       SPIDisable
** Descriptions:        SPI工作禁能
** input parameters:    u32Base      SPI模块的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SPIDisable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + SPI2S_CR1) &= ~((rt_uint32_t)(0x00000001));
}

///*********************************************************************************************************
//** Function name:       I2SEnable
//** Descriptions:        I2S工作使能
//** input parameters:    u32Base      I2S模块的基地址
//** output parameters:   NONE
//** Returned value:      NONE
//*********************************************************************************************************/
//void I2SEnable(rt_uint32_t u32Base)
//{
//    HWREG32(u32Base + SPI2S_CR1) |= (1<<0);
//}
//
///*********************************************************************************************************
//** Function name:       I2SDisable
//** Descriptions:        I2S工作禁能
//** input parameters:    u32Base      I2S模块的基地址
//** output parameters:   NONE
//** Returned value:      NONE
//*********************************************************************************************************/
//void I2SDisable(rt_uint32_t u32Base)
//{
//    HWREG32(u32Base + SPI2S_CR1) &= ~((rt_uint32_t)(1<<0));
//}

///*********************************************************************************************************
//** Function name:       I2SFullDuplexConfig
//** Descriptions:        I2S全双工模式配置
//** input parameters:    u32Base      I2S模块的基地址
//**                      u32Config    I2S模块工作模块初始化配置，低16位用于配置I2S_CFGR寄存器；搞16位用于
//**                                   配置I2SPRR寄存器，可以取下列几组数字的或
//**                                   SPI_I2S_Mode参数
//**                                   I2S_Mode_SlaveTx                //  从机发送
//**                                   I2S_Mode_SlaveRx                //  从机接收
//**                                   I2S_Mode_MasterTx               //  主机发送
//**                                   I2S_Mode_MasterRx               //  主机接收 
//**
//**                                   SPI_I2S_Standard参数
//**                                   I2S_Standard_Phillips           //飞利浦标准
//**                                   I2S_Standard_MSB                // MSB标准
//**                                   I2S_Standard_LSB                // LSB标准
//**                                   I2S_Standard_PCMShort           // PCM Short标准
//**                                   I2S_Standard_PCMLong            // PCM Long标准
//  
//**                                   SPI_I2S_Data_Format参数
//**                                   I2S_DataFormat_16b              // 16位格式
//**                                   I2S_DataFormat_16bextended      // 扩展16位格式
//**                                   I2S_DataFormat_24b              // 24位格式
//**                                   I2S_DataFormat_32b              // 32位格式
//  
//**                                   SPI_I2S_Clock_Polarity 参数
//**                                   I2S_CPOL_Low                    // 稳定状态电平低
//**                                   I2S_CPOL_High                   // 稳定状态电平高
//** output parameters:   NONE
//** Returned value:      NONE
//*********************************************************************************************************/
//void I2SFullDuplexConfig(rt_uint32_t u32Base,rt_uint16_t u16Config)
//{
//  rt_uint16_t tmpreg = 0, tmp = 0;
//  
//  /*----------------------- SPIx I2SCFGR & I2SPR Configuration -----------------*/
//  /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
//  HWREG16(u32Base + SPI_I2SCFGR) &=  ((rt_uint16_t)0xF040);
//  HWREG16(u32Base + SPI_I2SPR) = 0x00002;
//  
//  /* Get the I2SCFGR register value */
//  tmpreg = HWREG16(u32Base + SPI_I2SCFGR);
//  
//  /* Get the mode to be configured for the extended I2S */
//  if (((u16Config & 0x0300) == I2S_Mode_MasterTx) || ((u16Config & 0x0300) == I2S_Mode_SlaveTx))
//  {
//    tmp = I2S_Mode_SlaveRx;
//  }
//  else
//  {
//    if (((u16Config & 0x0300) == I2S_Mode_MasterRx) || ((u16Config & 0x0300) == I2S_Mode_SlaveRx))
//    {
//      tmp = I2S_Mode_SlaveTx;
//    }
//  }
//  
//  u16Config &= 0xFCFF;
//  
//  /* Configure the I2S with the SPI_InitStruct values */
//  tmpreg |= (rt_uint16_t)((rt_uint16_t)0x0800 | (rt_uint16_t)(tmp | u16Config));
//  
//  /* Write to SPIx I2SCFGR */  
//  HWREG16(u32Base + SPI_I2SCFGR) = tmpreg;
//}

///*********************************************************************************************************
//** Function name:       SPI_I2S_IntEnable
//** Descriptions:        SPI、I2S中断使能
//** input parameters:    u32Base      SPI模块的基地址
//**                      u32IntFlag   使能的中断源，可以取下列三个数值的的或
//**                      SPI_I2S_INT_TXE             //  发送缓冲区空中断
//**                      SPI_I2S_INT_RXNE            //  接收缓冲区有数据中断
//**                      SPI_I2S_INT_ERR             //  错误中断
//** output parameters:   NONE
//** Returned value:      NONE
//*********************************************************************************************************/
//void SPI_I2S_IntEnable(rt_uint32_t u32Base,rt_uint32_t u32IntFlag)
//{
//    HWREG32(u32Base + SPI_CR2) |= u32IntFlag;
//}
//
///*********************************************************************************************************
//** Function name:       SPI_I2S_IntEDisable
//** Descriptions:        SPI、I2S中断禁能
//** input parameters:    u32Base      SPI模块的基地址
//**                      u32IntFlag   禁能的中断源，可以取下列三个数值的的或
//**                      SPI_I2S_INT_TXE             //  发送缓冲区空中断
//**                      SPI_I2S_INT_RXNE            //  接收缓冲区有数据中断
//**                      SPI_I2S_INT_ERR             //  错误中断
//** output parameters:   NONE
//** Returned value:      NONE
//*********************************************************************************************************/
//void SPI_I2S_IntEDisable(rt_uint32_t u32Base,rt_uint32_t u32IntFlag)
//{
//    HWREG32(u32Base + SPI_CR2) &= ~u32IntFlag;
//}

/*********************************************************************************************************
** Function name:       SPI_I2S_DMAEnable
** Descriptions:        SPI、I2SDMA模式使能
** input parameters:    u32Base      SPI模块的基地址
**                      u32DMAFlag   指定使能的DMA功能是发送还是接收，可以去下列两个值的或
**                      SPI_I2S_DMA_TX               //  发送DMA
**                      SPI_I2S_DMA_RX               //  接受DMA
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SPI_I2S_DMAEnable(rt_uint32_t u32Base,rt_uint32_t u32DMAFlag)
{
    HWREG32(u32Base + SPI_CFG1) |= u32DMAFlag;
}

/*********************************************************************************************************
** Function name:       SPI_I2S_DMADisable
** Descriptions:        SPI、I2SDMA模式禁能
** input parameters:    u32Base      SPI模块的基地址
**                      u32DMAFlag   指定禁能的DMA功能是发送还是接收，可以去下列两个值的或
**                      SPI_I2S_DMA_TX               //  发送DMA
**                      SPI_I2S_DMA_RX               //  接受DMA
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SPI_I2S_DMADisable(rt_uint32_t u32Base,rt_uint32_t u32DMAFlag)
{
    HWREG32(u32Base + SPI_CFG1) &= ~u32DMAFlag;
}

/*********************************************************************************************************
** Function name:       SPI_I2S_DataPut
** Descriptions:        SPI、I2S发送数据
** input parameters:    u32Base      SPI模块的基地址
**                      usData      发送的数据，当工作在8位模式时，只有低8位有效
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SPI_I2S_DataPut(rt_uint32_t u32Base,rt_uint16_t u16Data)
{
    HWREG32(u32Base + SPI2S_TXDR) = u16Data;
}

/*********************************************************************************************************
** Function name:       SPI_I2S_DataPut
** Descriptions:        SPI、I2S接收数据
** input parameters:    u32Base      SPI模块的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint16_t SPI_I2S_DataGet(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + SPI2S_RXDR));
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
