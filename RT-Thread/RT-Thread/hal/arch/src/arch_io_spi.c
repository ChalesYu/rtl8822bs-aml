/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_spi.c
** Last modified Date:  2019-03-18
** Last Version:        v1.0
** Description:         spi��i2sģ��Ĵ�����װ����ʵ������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2019-03-18
** Version:             v1.0
** Descriptions:        The original version ��ʼ�汾
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
** Descriptions:        SPIģ�鹤��ģʽ����
** input parameters:    u32Base      SPIģ��Ļ���ַ
**                      u32Config1:    ���ò���1������ȡ���м������ֵĻ�
**                      
**                      // SPI�����ʷ�Ƶ��ѡ��
**                      SPI_Baud_Prescaler_2            //  �����ʷ���ϵ��Ϊ2
**                      SPI_Baud_Prescaler_4            //  �����ʷ���ϵ��Ϊ4
**                      SPI_Baud_Prescaler_8            //  �����ʷ���ϵ��Ϊ8
**                      SPI_Baud_Prescaler_16           //  �����ʷ���ϵ��Ϊ16
**                      SPI_Baud_Prescaler_32           //  �����ʷ���ϵ��Ϊ32
**                      SPI_Baud_Prescaler_64           //  �����ʷ���ϵ��Ϊ64
**                      SPI_Baud_Prescaler_128          //  �����ʷ���ϵ��Ϊ128
**                      SPI_Baud_Prescaler_256          //  �����ʷ���ϵ��Ϊ256
**                        
**                      // Ӳ��CRC�������
**                      SPI_CRC_ENABLE                  // ʹ��CRCУ��
**                      SPI_CRC_DISABLE                 // ��ֹCRCУ��
**                      
**                      // �����CRC Length��������DSIZE��������,xȡֵ4-32
**                      SPI_CRC_LENGTH_BIT(x) 
**                      
**                      // ����λ���ȣ�xȡֵ4-32
**                      SPI_DATA_LEGNTH_BIT(x) 
**                      
**                      u32Config2:    ���ò���1������ȡ���м������ֵĻ�
**                      // SSOEʹ�ܿ���
**                      SPI_SSOE_ENABLE                  // SSOEʹ��
**                      SPI_SSOE_DISABLE                 // SSOE��ֹ  
**                        
**                      // SS�ź���Ч��ƽ����
**                      SPI_SSIOP_HIGH                  // SS�źŸ���Ч
**                      SPI_SSIOP_LOW                   // SS�źŵ���Ч
**                       
**                      // �������շ�ʱ��SPI_CLK����ѡ��
**                      SPI_CPOL_LOW                    // CPOL��
**                      SPI_CPOL_HIGH                   // CPOL��
**                       
**                      // ʱ�Ӳ�������λѡ��
**                      SPI_CPHA_1EDGE                  // ��1��ʱ�Ӳɼ�����
**                      SPI_CPHA_2EDGE                  // ��2��ʱ�Ӳɼ�����
**                       
**                      // ��������λ��MSB��ʼ�����Ǵ�LSB��ʼѡ��
**                      SPI_FIRST_MSB                   // �ȴ����λ
**                      SPI_FIRST_LSB                   // �ȴ����λ
**                      
**                      //  ��������ѡ��
**                      SPI_MODE_MASTER                 // ����ģʽ
**                      SPI_MODE_SLAVE                  // �ӻ�ģʽ
**                       
**                      // SPIЭ�����
**                      SPI_PROTOCOL_MOTOROLA           // motorola��ʽ��Э��
**                      SPI_PROTOCOL_TI                 // TI��ʽ��Э��
**                      SPI_PROTOCOL_OTHERS             // ����Э��
** output parameters:   NONE
** Returned value:      NONE
** ע�⣺�Ĵ���д��˳�����Ҫ��˳�����ᵼ�¼Ĵ���д����Ч
*********************************************************************************************************/
void SPIInit(rt_uint32_t u32Base,rt_uint32_t u32Config1,rt_uint32_t u32Config2)
{
    rt_uint32_t u32Temp;
    
    if(u32Config2 & SPI_MODE_MASTER) {
      HWREG32(u32Base + SPI2S_CR1) |= ((1UL << 12));
    }
    
    u32Temp = HWREG32(u32Base + SPI_CFG1);
    
    u32Temp &= 0x0007C000; //�������������
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
//** Descriptions:        I2Sģʽ��ʼ��
//** input parameters:    u32Base      I2Sģ��Ļ���ַ
//**                      u32Config    I2Sģ�鹤��ģ���ʼ�����ã���16λ��������I2S_CFGR�Ĵ�������16λ����
//**                                   ����I2SPRR�Ĵ���������ȡ���м������ֵĻ�
//**                                   SPI_I2S_Mode����
//**                                   I2S_Mode_SlaveTx                //  �ӻ�����
//**                                   I2S_Mode_SlaveRx                //  �ӻ�����
//**                                   I2S_Mode_MasterTx               //  ��������
//**                                   I2S_Mode_MasterRx               //  �������� 
//**
//**                                   SPI_I2S_Standard����
//**                                   I2S_Standard_Phillips           //�����ֱ�׼
//**                                   I2S_Standard_MSB                // MSB��׼
//**                                   I2S_Standard_LSB                // LSB��׼
//**                                   I2S_Standard_PCMShort           // PCM Short��׼
//**                                   I2S_Standard_PCMLong            // PCM Long��׼
//  
//**                                   SPI_I2S_Data_Format����
//**                                   I2S_DataFormat_16b              // 16λ��ʽ
//**                                   I2S_DataFormat_16bextended      // ��չ16λ��ʽ
//**                                   I2S_DataFormat_24b              // 24λ��ʽ
//**                                   I2S_DataFormat_32b              // 32λ��ʽ
//  
//**                                   SPI_I2S_Clock_Polarity ����
//**                                   I2S_CPOL_Low                    // �ȶ�״̬��ƽ��
//**                                   I2S_CPOL_High                   // �ȶ�״̬��ƽ��
//  
//**                                   SPI_I2S_MCLK_Output������ʹ��ʱ��Ҫ ����16λ
//**                                   I2S_MCLKOutput_Enable           // MCLK���ʹ��
//**                                   2S_MCLKOutput_Disable           // MCLK�����ֹ
//**                      i2sclk       I2Sʱ�ӣ���Ϊ0ʱ��ʹ���ڲ�ʱ�ӣ���֮��ʹ��Ƶ��Ϊ��ֵ���ⲿʱ��
//**                      audio_freq   ��Ƶ����Ƶ�ʣ�����ȥ����ֵ��
//**                                   I2S_AudioFreq_192k          //  192K
//**                                   I2S_AudioFreq_96k           //  96K
//**                                   I2S_AudioFreq_48k           //  48K
//**                                   I2S_AudioFreq_44k           //  44.1K
//**                                   I2S_AudioFreq_32k           //  32K
//**                                   I2S_AudioFreq_22k           //  22.05K
//**                                   I2S_AudioFreq_16k           //  16K
//**                                   I2S_AudioFreq_11k           //  11.025K
//**                                   I2S_AudioFreq_8k            //  8K
//**                                   I2S_AudioFreq_Default       //  Ĭ��ֵ
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
** Descriptions:        SPI����ʹ��
** input parameters:    u32Base      SPIģ��Ļ���ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SPIEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + SPI2S_CR1) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       SPIDisable
** Descriptions:        SPI��������
** input parameters:    u32Base      SPIģ��Ļ���ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SPIDisable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + SPI2S_CR1) &= ~((rt_uint32_t)(0x00000001));
}

///*********************************************************************************************************
//** Function name:       I2SEnable
//** Descriptions:        I2S����ʹ��
//** input parameters:    u32Base      I2Sģ��Ļ���ַ
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
//** Descriptions:        I2S��������
//** input parameters:    u32Base      I2Sģ��Ļ���ַ
//** output parameters:   NONE
//** Returned value:      NONE
//*********************************************************************************************************/
//void I2SDisable(rt_uint32_t u32Base)
//{
//    HWREG32(u32Base + SPI2S_CR1) &= ~((rt_uint32_t)(1<<0));
//}

///*********************************************************************************************************
//** Function name:       I2SFullDuplexConfig
//** Descriptions:        I2Sȫ˫��ģʽ����
//** input parameters:    u32Base      I2Sģ��Ļ���ַ
//**                      u32Config    I2Sģ�鹤��ģ���ʼ�����ã���16λ��������I2S_CFGR�Ĵ�������16λ����
//**                                   ����I2SPRR�Ĵ���������ȡ���м������ֵĻ�
//**                                   SPI_I2S_Mode����
//**                                   I2S_Mode_SlaveTx                //  �ӻ�����
//**                                   I2S_Mode_SlaveRx                //  �ӻ�����
//**                                   I2S_Mode_MasterTx               //  ��������
//**                                   I2S_Mode_MasterRx               //  �������� 
//**
//**                                   SPI_I2S_Standard����
//**                                   I2S_Standard_Phillips           //�����ֱ�׼
//**                                   I2S_Standard_MSB                // MSB��׼
//**                                   I2S_Standard_LSB                // LSB��׼
//**                                   I2S_Standard_PCMShort           // PCM Short��׼
//**                                   I2S_Standard_PCMLong            // PCM Long��׼
//  
//**                                   SPI_I2S_Data_Format����
//**                                   I2S_DataFormat_16b              // 16λ��ʽ
//**                                   I2S_DataFormat_16bextended      // ��չ16λ��ʽ
//**                                   I2S_DataFormat_24b              // 24λ��ʽ
//**                                   I2S_DataFormat_32b              // 32λ��ʽ
//  
//**                                   SPI_I2S_Clock_Polarity ����
//**                                   I2S_CPOL_Low                    // �ȶ�״̬��ƽ��
//**                                   I2S_CPOL_High                   // �ȶ�״̬��ƽ��
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
//** Descriptions:        SPI��I2S�ж�ʹ��
//** input parameters:    u32Base      SPIģ��Ļ���ַ
//**                      u32IntFlag   ʹ�ܵ��ж�Դ������ȡ����������ֵ�ĵĻ�
//**                      SPI_I2S_INT_TXE             //  ���ͻ��������ж�
//**                      SPI_I2S_INT_RXNE            //  ���ջ������������ж�
//**                      SPI_I2S_INT_ERR             //  �����ж�
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
//** Descriptions:        SPI��I2S�жϽ���
//** input parameters:    u32Base      SPIģ��Ļ���ַ
//**                      u32IntFlag   ���ܵ��ж�Դ������ȡ����������ֵ�ĵĻ�
//**                      SPI_I2S_INT_TXE             //  ���ͻ��������ж�
//**                      SPI_I2S_INT_RXNE            //  ���ջ������������ж�
//**                      SPI_I2S_INT_ERR             //  �����ж�
//** output parameters:   NONE
//** Returned value:      NONE
//*********************************************************************************************************/
//void SPI_I2S_IntEDisable(rt_uint32_t u32Base,rt_uint32_t u32IntFlag)
//{
//    HWREG32(u32Base + SPI_CR2) &= ~u32IntFlag;
//}

/*********************************************************************************************************
** Function name:       SPI_I2S_DMAEnable
** Descriptions:        SPI��I2SDMAģʽʹ��
** input parameters:    u32Base      SPIģ��Ļ���ַ
**                      u32DMAFlag   ָ��ʹ�ܵ�DMA�����Ƿ��ͻ��ǽ��գ�����ȥ��������ֵ�Ļ�
**                      SPI_I2S_DMA_TX               //  ����DMA
**                      SPI_I2S_DMA_RX               //  ����DMA
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SPI_I2S_DMAEnable(rt_uint32_t u32Base,rt_uint32_t u32DMAFlag)
{
    HWREG32(u32Base + SPI_CFG1) |= u32DMAFlag;
}

/*********************************************************************************************************
** Function name:       SPI_I2S_DMADisable
** Descriptions:        SPI��I2SDMAģʽ����
** input parameters:    u32Base      SPIģ��Ļ���ַ
**                      u32DMAFlag   ָ�����ܵ�DMA�����Ƿ��ͻ��ǽ��գ�����ȥ��������ֵ�Ļ�
**                      SPI_I2S_DMA_TX               //  ����DMA
**                      SPI_I2S_DMA_RX               //  ����DMA
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SPI_I2S_DMADisable(rt_uint32_t u32Base,rt_uint32_t u32DMAFlag)
{
    HWREG32(u32Base + SPI_CFG1) &= ~u32DMAFlag;
}

/*********************************************************************************************************
** Function name:       SPI_I2S_DataPut
** Descriptions:        SPI��I2S��������
** input parameters:    u32Base      SPIģ��Ļ���ַ
**                      usData      ���͵����ݣ���������8λģʽʱ��ֻ�е�8λ��Ч
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SPI_I2S_DataPut(rt_uint32_t u32Base,rt_uint16_t u16Data)
{
    HWREG32(u32Base + SPI2S_TXDR) = u16Data;
}

/*********************************************************************************************************
** Function name:       SPI_I2S_DataPut
** Descriptions:        SPI��I2S��������
** input parameters:    u32Base      SPIģ��Ļ���ַ
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
