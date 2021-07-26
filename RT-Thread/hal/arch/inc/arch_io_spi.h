/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_spi.h
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
#ifndef __ARCH_IO_SPI_H__
#define __ARCH_IO_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** ��Ҫ�ĺ궨��
*********************************************************************************************************/
// ����SPI_Init����u32Config1�궨��,����ȡ���м�������Ļ�
// SPI�����ʷ�Ƶ��ѡ��
#define SPI_Baud_Prescaler_2            0x00000000
#define SPI_Baud_Prescaler_4            0x10000000
#define SPI_Baud_Prescaler_8            0x20000000
#define SPI_Baud_Prescaler_16           0x30000000
#define SPI_Baud_Prescaler_32           0x40000000
#define SPI_Baud_Prescaler_64           0x50000000
#define SPI_Baud_Prescaler_128          0x60000000
#define SPI_Baud_Prescaler_256          0x70000000
  
// Ӳ��CRC�������
#define SPI_CRC_ENABLE                  0x00400000
#define SPI_CRC_DISABLE                 0x00000000
  
// �����CRC Length��������DSIZE��������,ȡֵ4-32
#define SPI_CRC_LENGTH_BIT(x)           (((rt_uint32_t)(x - 1)) << 16)

// ����λ���ȣ�ȡֵ4-32
#define SPI_DATA_LEGNTH_BIT(x)          (((rt_uint32_t)(x - 1)) << 0)

// ����SPI_Init����u32Config2�궨��,����ȡ���м�������Ļ� 
// SSOEʹ�ܿ���
#define SPI_SSOE_ENABLE                  0x20000000
#define SPI_SSOE_DISABLE                 0x00000000  
  
// SS�ź���Ч��ƽ����
#define SPI_SSIOP_HIGH                  0x10000000
#define SPI_SSIOP_LOW                   0x00000000
  
// �������շ�ʱ��SPI_CLK����ѡ��
#define SPI_CPOL_LOW                    0x00000000
#define SPI_CPOL_HIGH                   0x02000000 
  
// ʱ�Ӳ�������λѡ��
#define SPI_CPHA_1EDGE                  0x00000000
#define SPI_CPHA_2EDGE                  0x01000000
  
// ��������λ��MSB��ʼ�����Ǵ�LSB��ʼѡ��
#define SPI_FIRST_MSB                   0x00000000
#define SPI_FIRST_LSB                   0x00800000

//  ��������ѡ��
#define SPI_MODE_MASTER                 0x00400000
#define SPI_MODE_SLAVE                  0x00000000
  
// SPIЭ�����
#define SPI_PROTOCOL_MOTOROLA           0x00000000
#define SPI_PROTOCOL_TI                 0x00080000
#define SPI_PROTOCOL_OTHERS             0x00380000
  
//// SPI_I2S_Mode����
//#define I2S_Mode_SlaveTx                ((rt_uint32_t)0x0000)
//#define I2S_Mode_SlaveRx                ((rt_uint32_t)0x0100)
//#define I2S_Mode_MasterTx               ((rt_uint32_t)0x0200)
//#define I2S_Mode_MasterRx               ((rt_uint32_t)0x0300)  
//  
//// SPI_I2S_Standard����
//#define I2S_Standard_Phillips           ((rt_uint32_t)0x0000)
//#define I2S_Standard_MSB                ((rt_uint32_t)0x0010)
//#define I2S_Standard_LSB                ((rt_uint32_t)0x0020)
//#define I2S_Standard_PCMShort           ((rt_uint32_t)0x0030)
//#define I2S_Standard_PCMLong            ((rt_uint32_t)0x00B0)
//  
//// SPI_I2S_Data_Format����
//#define I2S_DataFormat_16b              ((rt_uint32_t)0x0000)
//#define I2S_DataFormat_16bextended      ((rt_uint32_t)0x0001)
//#define I2S_DataFormat_24b              ((rt_uint32_t)0x0003)
//#define I2S_DataFormat_32b              ((rt_uint32_t)0x0005)
//  
//// SPI_I2S_Clock_Polarity ����
//#define I2S_CPOL_Low                    ((rt_uint32_t)0x0000)
//#define I2S_CPOL_High                   ((rt_uint32_t)0x0008)
//  
//// SPI_I2S_MCLK_Output������ʹ��ʱ��Ҫ ����16λ
//#define I2S_MCLKOutput_Enable           ((rt_uint32_t)0x02000000)
//#define I2S_MCLKOutput_Disable          ((rt_uint32_t)0x0000)
//    
//// SPI_I2S_Audio_Frequency����
//#define I2S_AudioFreq_192k               ((rt_uint32_t)192000)
//#define I2S_AudioFreq_96k                ((rt_uint32_t)96000)
//#define I2S_AudioFreq_48k                ((rt_uint32_t)48000)
//#define I2S_AudioFreq_44k                ((rt_uint32_t)44100)
//#define I2S_AudioFreq_32k                ((rt_uint32_t)32000)
//#define I2S_AudioFreq_22k                ((rt_uint32_t)22050)
//#define I2S_AudioFreq_16k                ((rt_uint32_t)16000)
//#define I2S_AudioFreq_11k                ((rt_uint32_t)11025)
//#define I2S_AudioFreq_8k                 ((rt_uint32_t)8000)
//#define I2S_AudioFreq_Default            ((rt_uint32_t)2)
  

// ����SPI_I2S_IntEnable��SPI_I2S_IntDisable�Ĳ������壬ָ��ʹ�ܵ��ж�Դ
#define SPI_I2S_INT_TXE                 (1<<7)
#define SPI_I2S_INT_RXNE                (1<<6)
#define SPI_I2S_INT_ERR                 (1<<5)

// ����SPI_I2S_DMAEnable��SPI_I2S_DMADisable�Ĳ�������
#define SPI_I2S_DMA_TX                  (1UL<<15)
#define SPI_I2S_DMA_RX                  (1UL<<14)

/*********************************************************************************************************
** �꺯����һЩ����
*********************************************************************************************************/
#define SPI_Busy(u32Base)                (HWREG32(u32Base+SPI_SR) & 0x80)  // Ϊ0����ʾSPI���߲�æ����0����ʾæ
#define SPISpaceAvial(u32Base)           (!(HWREG32(u32Base+SPI_SR) & 0x02))// 0�����ͻ������գ���0�����ͻ���������
#define SPICharsAvial(u32Base)           (HWREG32(u32Base+SPI_SR) & 0x01)  // 0,���ջ������գ���0�����ջ������ǿ�

// ����������ͨ���������Ƭѡ���������ֻ��Ƭѡ��������Ϊ�������ʱ����Ч
#define SPI_SSSet(u32Base)               (HWREG32(u32Base+SPI_CR1) |= 0x0100)
#define SPI_SSReset(u32Base)             (HWREG32(u32Base+SPI_CR1) &= 0xFEFF)
/*********************************************************************************************************
** �ⲿ����������
*********************************************************************************************************/
extern void SPIInit(rt_uint32_t u32Base,rt_uint32_t u32Config1,rt_uint32_t u32Config2);
extern void I2SInit(rt_uint32_t u32Base,rt_uint32_t u32Config, rt_uint32_t i2sclk, rt_uint32_t audio_freq);
extern void SPI_SSOutEnable(rt_uint32_t u32Base);
extern void SPI_SSOutDisable(rt_uint32_t u32Base);
extern void SPIEnable(rt_uint32_t u32Base);
extern void SPIDisable(rt_uint32_t u32Base);
extern void I2SEnable(rt_uint32_t u32Base);
extern void I2SDisable(rt_uint32_t u32Base);
extern void I2SFullDuplexConfig(rt_uint32_t u32Base,rt_uint16_t u16Config);
extern void SPI_I2S_IntEnable(rt_uint32_t u32Base,rt_uint32_t u32IntFlag);
extern void SPI_I2S_IntEDisable(rt_uint32_t u32Base,rt_uint32_t u32IntFlag);
extern void SPI_I2S_DMAEnable(rt_uint32_t u32Base,rt_uint32_t u32DMAFlag);
extern void SPI_I2S_DMADisable(rt_uint32_t u32Base,rt_uint32_t u32DMAFlag);
extern void SPI_I2S_DataPut(rt_uint32_t u32Base,rt_uint16_t u16Data);
extern rt_uint16_t SPI_I2S_DataGet(rt_uint32_t u32Base);



#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_SPI_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
