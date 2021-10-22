/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_gpio.h
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         gpioģ��Ĵ�����װ����ʵ������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2019-03-15
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
#ifndef __ARCH_IO_GPIO_H__
#define __ARCH_IO_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** GPIO���Ŷ���
*********************************************************************************************************/
#define GPIO_PIN_0      0x0001
#define GPIO_PIN_1      0x0002
#define GPIO_PIN_2      0x0004
#define GPIO_PIN_3      0x0008
#define GPIO_PIN_4      0x0010
#define GPIO_PIN_5      0x0020
#define GPIO_PIN_6      0x0040
#define GPIO_PIN_7      0x0080
#define GPIO_PIN_8      0x0100
#define GPIO_PIN_9      0x0200
#define GPIO_PIN_10     0x0400
#define GPIO_PIN_11     0x0800
#define GPIO_PIN_12     0x1000
#define GPIO_PIN_13     0x2000
#define GPIO_PIN_14     0x4000
#define GPIO_PIN_15     0x8000
#define GPIO_PIN_All    0xFFFF

/*********************************************************************************************************
** GPIO����ģʽ����
*********************************************************************************************************/
#define GPIO_MODE_IN        0x00    // GPIO����ģʽ
#define GPIO_MODE_OUT       0x01    // GPIO���ģʽ
#define GPIO_MODE_AF        0x02    // GPIO���踴��ģʽ
#define GPIO_MODE_AN        0x03    // GPIOģ��ģʽ��������ADC��DAC����ʱ�õ�

/*********************************************************************************************************
** GPIO�������ģʽ����
*********************************************************************************************************/
#define GPIO_OUT_PP         0x00    // GPIO����ģʽ
#define GPIO_OUT_OD         0x01    // GPIO��©ģʽ

/*********************************************************************************************************
** GPIO��������ٶȶ���
*********************************************************************************************************/
#define GPIO_SPEED_LOW          0x00    // ����
#define GPIO_SPEED_Medium       0x01    // ����
#define GPIO_SPEED_HIGH         0x02    // ����
#define GPIO_SPEED_VERY_HIGH    0x03    // ���� on 30 pF (80 MHz Output max speed on 15 pF)

/*********************************************************************************************************
** GPIO����������������
*********************************************************************************************************/
#define GPIO_PUPD_NOPULL    0x00    // ������������
#define GPIO_PUPD_UP        0x01    // ����
#define GPIO_PUPD_DOWN      0x02    // ����

/*********************************************************************************************************
** ���ù��ܣ� AF0ѡ��
*********************************************************************************************************/
#define GPIO_AF_RTC_50Hz      0x00  // RTC_50Hz Alternate Function mapping
#define GPIO_AF_MCO           0x00  // MCO (MCO1 and MCO2) Alternate Function mapping
#define GPIO_AF_TAMPER        0x00  // TAMPER (TAMPER_1 and TAMPER_2) Alternate Function mapping
#define GPIO_AF_SWJ           0x00  // SWJ (SWD and JTAG) Alternate Function mapping
#define GPIO_AF_TRACE         0x00  // TRACE Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF1ѡ��
*********************************************************************************************************/
#define GPIO_AF_TIM1          0x01  // TIM1 Alternate Function mapping
#define GPIO_AF_TIM2          0x01  // TIM2 Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF2ѡ��
*********************************************************************************************************/
#define GPIO_AF_TIM3          0x02  // TIM3 Alternate Function mapping
#define GPIO_AF_TIM4          0x02  // TIM4 Alternate Function mapping
#define GPIO_AF_TIM5          0x02  // TIM5 Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF3ѡ��
*********************************************************************************************************/
#define GPIO_AF_TIM8          0x03  // TIM8 Alternate Function mapping
#define GPIO_AF_TIM9          0x03  // TIM9 Alternate Function mapping
#define GPIO_AF_TIM10         0x03  // TIM10 Alternate Function mapping
#define GPIO_AF_TIM11         0x03  // TIM11 Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF4ѡ��
*********************************************************************************************************/
#define GPIO_AF_I2C1          0x04  // I2C1 Alternate Function mapping
#define GPIO_AF_I2C2          0x04  // I2C2 Alternate Function mapping
#define GPIO_AF_I2C3          0x04  // I2C3 Alternate Function mapping
#define GPIO_AF4_USART1       0x04  // USART1 Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF5ѡ��
*********************************************************************************************************/
#define GPIO_AF_SPI1          0x05  // SPI1 Alternate Function mapping
#define GPIO_AF_SPI2          0x05  // SPI2/I2S2 Alternate Function mapping
#define GPIO_AF_SPI4          0x05  // SPI4 Alternate Function mapping
#define GPIO_AF_SPI5          0x05  // SPI5 Alternate Function mapping
#define GPIO_AF_SPI6          0x05  // SPI6 Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF6ѡ��
*********************************************************************************************************/
#define GPIO_AF_SPI3          0x06  // SPI3/I2S3 Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF7ѡ��
*********************************************************************************************************/
#define GPIO_AF7_USART1       0x07  // USART1 Alternate Function mapping
#define GPIO_AF7_USART2       0x07  // USART2 Alternate Function mapping
#define GPIO_AF7_USART3       0x07  // USART3 Alternate Function mapping
#define GPIO_AF7_I2S3ext      0x07  // I2S3ext Alternate Function mapping
#define GPIO_AF7_USART6       0x07  // USART6 Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF8ѡ��
*********************************************************************************************************/
#define GPIO_AF8_UART4        0x08  // UART4 Alternate Function mapping
#define GPIO_AF8_UART5        0x08  // UART5 Alternate Function mapping
#define GPIO_AF8_UART8        0x08  // UART8 Alternate Function mapping
#define GPIO_AF8_LPUART1      0x08  // LPUART1 Alternate Function mapping
  
/*********************************************************************************************************
** ���ù��ܣ� AF9ѡ��
*********************************************************************************************************/
#define GPIO_AF9_QSPI         0x09  // CAN1 Alternate Function mapping
#define GPIO_AF_CAN1          0x09  // CAN1 Alternate Function mapping
#define GPIO_AF_CAN2          0x09  // CAN2 Alternate Function mapping
#define GPIO_AF_TIM12         0x09  // TIM12 Alternate Function mapping
#define GPIO_AF_TIM13         0x09  // TIM13 Alternate Function mapping
#define GPIO_AF_TIM14         0x09  // TIM14 Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF10ѡ��
*********************************************************************************************************/
#define GPIO_AF_OTG_FS        0x0A  // OTG_FS Alternate Function mapping
#define GPIO_AF_OTG_HS        0x0A  // OTG_HS Alternate Function mapping
#define GPIO_AF10_ULPI        0x0A  // OTG_ULPI Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF11ѡ��
*********************************************************************************************************/
#define GPIO_AF_ETH           0x0B  // ETHERNET Alternate Function mapping
#define GPIO_AF11_UART7       0x0B  // UART Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF12ѡ��
*********************************************************************************************************/
#define GPIO_AF_FSMC          0x0C  // FSMC Alternate Function mapping
#define GPIO_AF_OTG_HS_FS     0x0C  // OTG HS configured in FS, Alternate Function mapping
#define GPIO_AF_SDMMC         0x0C  // SDIO Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF13ѡ��
*********************************************************************************************************/
#define GPIO_AF_DCMI          0x0D  // DCMI Alternate Function mapping

/*********************************************************************************************************
** ���ù��ܣ� AF15ѡ��
*********************************************************************************************************/
#define GPIO_AF_EVENTOUT      0x0F  // EVENTOUT Alternate Function mapping

/*********************************************************************************************************
** �ⲿ����������
*********************************************************************************************************/
extern void GPIOModeSet(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint32_t u32Mode);
extern void GPIOOutputTypeSet(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint32_t u32Mode);
extern void GPIOOutputSpeedSet(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint32_t u32Mode);
extern void GPIOPullUpDownSet(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint32_t u32Mode);
extern rt_uint32_t GPIOPinInputRead(rt_uint32_t u32Base, rt_uint32_t u32Pins);
extern rt_uint32_t GPIOPinOutputRead(rt_uint32_t u32Base, rt_uint32_t u32Pins);
extern void GPIOPinWrite(rt_uint32_t u32Base, rt_uint32_t u32Value);
extern void GPIOPinSetBit(rt_uint32_t u32Base, rt_uint32_t u32Pins);
extern void GPIOPinResetBit(rt_uint32_t u32Base, rt_uint32_t u32Pins);
extern void GPIOPinToggleBit(rt_uint32_t u32Base, rt_uint32_t u32Pins);
extern void GPIOPinWriteBit(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint8_t u8State);
extern rt_uint8_t GPIOPinReadBit(rt_uint32_t u32Base, rt_uint32_t u32Pins);
extern void GPIOPinAFConfig(rt_uint32_t u32Base, rt_uint32_t u32Pin, rt_uint32_t u32AFMode);
extern void GPIOPinTypeGPIOOutput(rt_uint32_t u32Base, rt_uint32_t u32Pins);
extern void GPIOPinTypeGPIOInput(rt_uint32_t u32Base, rt_uint32_t u32Pins);
extern void GPIOPinTypeGPIOAF(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint32_t u32Pin, rt_uint32_t u32AFMode);


#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_GPIO_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
