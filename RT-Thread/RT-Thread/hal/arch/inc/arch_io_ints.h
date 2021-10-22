/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_ints.h
** Last modified Date:  2013-04-01
** Last Version:        v1.0
** Description:         中断源相关定义
** 
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2013-04-01
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

#ifndef __ARCH_IO_INTS_H__
#define __ARCH_IO_INTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/cortexm/hal_cortexm_api.h"

/*********************************************************************************************************
** 本平台支持的外部中断编号定义
*********************************************************************************************************/
#define ARCH_INT_WWDG1               (HAL_EXTERN_INT_BASE + 0)
#define ARCH_INT_PVD_PVM             (HAL_EXTERN_INT_BASE + 1)
#define ARCH_INT_RTC_TAMP_STAMP      (HAL_EXTERN_INT_BASE + 2)
#define ARCH_INT_RTC_WKUP            (HAL_EXTERN_INT_BASE + 3)
#define ARCH_INT_FLASH               (HAL_EXTERN_INT_BASE + 4)
#define ARCH_INT_RCC                 (HAL_EXTERN_INT_BASE + 5)
#define ARCH_INT_EXTI0               (HAL_EXTERN_INT_BASE + 6)
#define ARCH_INT_EXTI1               (HAL_EXTERN_INT_BASE + 7)
#define ARCH_INT_EXTI2               (HAL_EXTERN_INT_BASE + 8)
#define ARCH_INT_EXTI3               (HAL_EXTERN_INT_BASE + 9)
#define ARCH_INT_EXTI4               (HAL_EXTERN_INT_BASE + 10)
#define ARCH_INT_DMA1_STREAM0        (HAL_EXTERN_INT_BASE + 11)
#define ARCH_INT_DMA1_STREAM1        (HAL_EXTERN_INT_BASE + 12)
#define ARCH_INT_DMA1_STREAM2        (HAL_EXTERN_INT_BASE + 13)
#define ARCH_INT_DMA1_STREAM3        (HAL_EXTERN_INT_BASE + 14)
#define ARCH_INT_DMA1_STREAM4        (HAL_EXTERN_INT_BASE + 15)
#define ARCH_INT_DMA1_STREAM5        (HAL_EXTERN_INT_BASE + 16)
#define ARCH_INT_DMA1_STREAM6        (HAL_EXTERN_INT_BASE + 17)
#define ARCH_INT_ADC1_2              (HAL_EXTERN_INT_BASE + 18)
#define ARCH_INT_FDCAN1_IT0          (HAL_EXTERN_INT_BASE + 19)
#define ARCH_INT_FDCAN2_IT0          (HAL_EXTERN_INT_BASE + 20)
#define ARCH_INT_FDCAN1_IT1          (HAL_EXTERN_INT_BASE + 21)
#define ARCH_INT_FDCAN2_IT1          (HAL_EXTERN_INT_BASE + 22)
#define ARCH_INT_EXTI9_5             (HAL_EXTERN_INT_BASE + 23)
#define ARCH_INT_TIM1_BRK            (HAL_EXTERN_INT_BASE + 24)
#define ARCH_INT_TIM1_UP             (HAL_EXTERN_INT_BASE + 25)
#define ARCH_INT_TIM1_TRG_COM        (HAL_EXTERN_INT_BASE + 26)
#define ARCH_INT_TIM1_CC             (HAL_EXTERN_INT_BASE + 27)
#define ARCH_INT_TIM2                (HAL_EXTERN_INT_BASE + 28)
#define ARCH_INT_TIM3                (HAL_EXTERN_INT_BASE + 29)
#define ARCH_INT_TIM4                (HAL_EXTERN_INT_BASE + 30)
#define ARCH_INT_I2C1_EV             (HAL_EXTERN_INT_BASE + 31)
#define ARCH_INT_I2C1_ER             (HAL_EXTERN_INT_BASE + 32)
#define ARCH_INT_I2C2_EV             (HAL_EXTERN_INT_BASE + 33)
#define ARCH_INT_I2C2_ER             (HAL_EXTERN_INT_BASE + 34)
#define ARCH_INT_SPI1                (HAL_EXTERN_INT_BASE + 35)
#define ARCH_INT_SPI2                (HAL_EXTERN_INT_BASE + 36)
#define ARCH_INT_USART1              (HAL_EXTERN_INT_BASE + 37)
#define ARCH_INT_USART2              (HAL_EXTERN_INT_BASE + 38)
#define ARCH_INT_USART3              (HAL_EXTERN_INT_BASE + 39)
#define ARCH_INT_EXTI15_10           (HAL_EXTERN_INT_BASE + 40)
#define ARCH_INT_RTC_ALARM           (HAL_EXTERN_INT_BASE + 41)
//#define ARCH_INT_NC                (HAL_EXTERN_INT_BASE + 42) // 保留
#define ARCH_INT_TIM8_BRK_TIM12      (HAL_EXTERN_INT_BASE + 43)
#define ARCH_INT_TIM8_UP_TIM13       (HAL_EXTERN_INT_BASE + 44)
#define ARCH_INT_TIM8_TRG_COM_TIM14  (HAL_EXTERN_INT_BASE + 45)
#define ARCH_INT_TIM8_CC             (HAL_EXTERN_INT_BASE + 46)
#define ARCH_INT_DMA1_STREAM7        (HAL_EXTERN_INT_BASE + 47)
#define ARCH_INT_FMC                 (HAL_EXTERN_INT_BASE + 48)
#define ARCH_INT_SDMMC1              (HAL_EXTERN_INT_BASE + 49)
#define ARCH_INT_TIM5                (HAL_EXTERN_INT_BASE + 50)
#define ARCH_INT_SPI3                (HAL_EXTERN_INT_BASE + 51)
#define ARCH_INT_UART4               (HAL_EXTERN_INT_BASE + 52)
#define ARCH_INT_UART5               (HAL_EXTERN_INT_BASE + 53)
#define ARCH_INT_TIM6_DAC            (HAL_EXTERN_INT_BASE + 54)
#define ARCH_INT_TIM7                (HAL_EXTERN_INT_BASE + 55)
#define ARCH_INT_DMA2_STREAM0        (HAL_EXTERN_INT_BASE + 56)
#define ARCH_INT_DMA2_STREAM1        (HAL_EXTERN_INT_BASE + 57)
#define ARCH_INT_DMA2_STREAM2        (HAL_EXTERN_INT_BASE + 58)
#define ARCH_INT_DMA2_STREAM3        (HAL_EXTERN_INT_BASE + 59)
#define ARCH_INT_DMA2_STREAM4        (HAL_EXTERN_INT_BASE + 60)
#define ARCH_INT_ETH                 (HAL_EXTERN_INT_BASE + 61)
#define ARCH_INT_ETH_WKUP            (HAL_EXTERN_INT_BASE + 62)
#define ARCH_INT_FDCAN_CAL           (HAL_EXTERN_INT_BASE + 63)
//#define ARCH_INT_NC                (HAL_EXTERN_INT_BASE + 64) // 保留
//#define ARCH_INT_NC                (HAL_EXTERN_INT_BASE + 65) // 保留
//#define ARCH_INT_NC                (HAL_EXTERN_INT_BASE + 66) // 保留
//#define ARCH_INT_NC                (HAL_EXTERN_INT_BASE + 67) // 保留
#define ARCH_INT_DMA2_STREAM5        (HAL_EXTERN_INT_BASE + 68)
#define ARCH_INT_DMA2_STREAM6        (HAL_EXTERN_INT_BASE + 69)
#define ARCH_INT_DMA2_STREAM7        (HAL_EXTERN_INT_BASE + 70)
#define ARCH_INT_USART6              (HAL_EXTERN_INT_BASE + 71)
#define ARCH_INT_I2C3_EV             (HAL_EXTERN_INT_BASE + 72)
#define ARCH_INT_I2C3_ER             (HAL_EXTERN_INT_BASE + 73)
#define ARCH_INT_OTG_HS_EP1_OUT      (HAL_EXTERN_INT_BASE + 74)
#define ARCH_INT_OTG_HS_EP1_IN       (HAL_EXTERN_INT_BASE + 75)
#define ARCH_INT_OTG_HS_WKUP         (HAL_EXTERN_INT_BASE + 76)
#define ARCH_INT_OTG_HS              (HAL_EXTERN_INT_BASE + 77)
#define ARCH_INT_DCMI                (HAL_EXTERN_INT_BASE + 78)
#define ARCH_INT_CRYP                (HAL_EXTERN_INT_BASE + 79)
#define ARCH_INT_HASH_RNG            (HAL_EXTERN_INT_BASE + 80)
#define ARCH_INT_FPU                 (HAL_EXTERN_INT_BASE + 81)
#define ARCH_INT_UART7               (HAL_EXTERN_INT_BASE + 82)
#define ARCH_INT_UART8               (HAL_EXTERN_INT_BASE + 83)
#define ARCH_INT_SPI4                (HAL_EXTERN_INT_BASE + 84)
#define ARCH_INT_SPI5                (HAL_EXTERN_INT_BASE + 85)
#define ARCH_INT_SPI6                (HAL_EXTERN_INT_BASE + 86)
#define ARCH_INT_SAI1                (HAL_EXTERN_INT_BASE + 87)
#define ARCH_INT_LTDC                (HAL_EXTERN_INT_BASE + 88)
#define ARCH_INT_LTDC_ER             (HAL_EXTERN_INT_BASE + 89)
#define ARCH_INT_DMA2D               (HAL_EXTERN_INT_BASE + 90)
#define ARCH_INT_SAI2                (HAL_EXTERN_INT_BASE + 91)
#define ARCH_INT_QUADSPI             (HAL_EXTERN_INT_BASE + 92)
#define ARCH_INT_LPTIM1              (HAL_EXTERN_INT_BASE + 93)
#define ARCH_INT_HDMI_CEC            (HAL_EXTERN_INT_BASE + 94)
#define ARCH_INT_I2C4_EV             (HAL_EXTERN_INT_BASE + 95)
#define ARCH_INT_I2C4_ER             (HAL_EXTERN_INT_BASE + 96)
#define ARCH_INT_SPDIF               (HAL_EXTERN_INT_BASE + 97)
#define ARCH_INT_OTG_FS_EP1_OUT      (HAL_EXTERN_INT_BASE + 98)
#define ARCH_INT_OTG_FS_EP1_IN       (HAL_EXTERN_INT_BASE + 99)
#define ARCH_INT_OTG_FS_WKUP         (HAL_EXTERN_INT_BASE + 100)
#define ARCH_INT_OTG_FS              (HAL_EXTERN_INT_BASE + 101)
#define ARCH_INT_DMAMUX1_OV          (HAL_EXTERN_INT_BASE + 102)
#define ARCH_INT_HRTIM1_MST          (HAL_EXTERN_INT_BASE + 103)
#define ARCH_INT_HRTIM1_TIMA         (HAL_EXTERN_INT_BASE + 104)
#define ARCH_INT_HRTIM1_TIMB         (HAL_EXTERN_INT_BASE + 105)
#define ARCH_INT_HRTIM1_TIMC         (HAL_EXTERN_INT_BASE + 106)
#define ARCH_INT_HRTIM1_TIMD         (HAL_EXTERN_INT_BASE + 107)
#define ARCH_INT_HRTIM1_TIME         (HAL_EXTERN_INT_BASE + 108)
#define ARCH_INT_HRTIM1_FLT          (HAL_EXTERN_INT_BASE + 109)
#define ARCH_INT_DFSDM1_FLT0         (HAL_EXTERN_INT_BASE + 110)
#define ARCH_INT_DFSDM1_FLT1         (HAL_EXTERN_INT_BASE + 111)
#define ARCH_INT_DFSDM1_FLT2         (HAL_EXTERN_INT_BASE + 112)
#define ARCH_INT_DFSDM1_FLT3         (HAL_EXTERN_INT_BASE + 113)
#define ARCH_INT_SAI3                (HAL_EXTERN_INT_BASE + 114)
#define ARCH_INT_SWPMI1              (HAL_EXTERN_INT_BASE + 115)
#define ARCH_INT_TIM15               (HAL_EXTERN_INT_BASE + 116)
#define ARCH_INT_TIM16               (HAL_EXTERN_INT_BASE + 117)
#define ARCH_INT_TIM17               (HAL_EXTERN_INT_BASE + 118)
#define ARCH_INT_MDIOS_WKUP          (HAL_EXTERN_INT_BASE + 119)
#define ARCH_INT_MDIOS               (HAL_EXTERN_INT_BASE + 120)
#define ARCH_INT_JPEG                (HAL_EXTERN_INT_BASE + 121)
#define ARCH_INT_MDMA                (HAL_EXTERN_INT_BASE + 122)
//#define ARCH_INT_NC                (HAL_EXTERN_INT_BASE + 123) // 保留
#define ARCH_INT_SDMMC2              (HAL_EXTERN_INT_BASE + 124)
#define ARCH_INT_HSEM0               (HAL_EXTERN_INT_BASE + 125)
//#define ARCH_INT_NC                (HAL_EXTERN_INT_BASE + 126) // 保留
#define ARCH_INT_ADC3                (HAL_EXTERN_INT_BASE + 127)
#define ARCH_INT_DMAMUX2_OVR         (HAL_EXTERN_INT_BASE + 128)
#define ARCH_INT_BDMA_CH0            (HAL_EXTERN_INT_BASE + 129)
#define ARCH_INT_BDMA_CH1            (HAL_EXTERN_INT_BASE + 130)
#define ARCH_INT_BDMA_CH2            (HAL_EXTERN_INT_BASE + 131)
#define ARCH_INT_BDMA_CH3            (HAL_EXTERN_INT_BASE + 132)
#define ARCH_INT_BDMA_CH4            (HAL_EXTERN_INT_BASE + 133)
#define ARCH_INT_BDMA_CH5            (HAL_EXTERN_INT_BASE + 134)
#define ARCH_INT_BDMA_CH6            (HAL_EXTERN_INT_BASE + 135)
#define ARCH_INT_BDMA_CH7            (HAL_EXTERN_INT_BASE + 136)
#define ARCH_INT_COMP                (HAL_EXTERN_INT_BASE + 137)
#define ARCH_INT_LPTIM2              (HAL_EXTERN_INT_BASE + 138)
#define ARCH_INT_LPTIM3              (HAL_EXTERN_INT_BASE + 139)
#define ARCH_INT_LPTIM4              (HAL_EXTERN_INT_BASE + 140)
#define ARCH_INT_LPTIM5              (HAL_EXTERN_INT_BASE + 141)
#define ARCH_INT_LPUART1             (HAL_EXTERN_INT_BASE + 142)
#define ARCH_INT_WWDG1_RST           (HAL_EXTERN_INT_BASE + 143)
#define ARCH_INT_CRS                 (HAL_EXTERN_INT_BASE + 144)
#define ARCH_INT_RAMECC1_2_3         (HAL_EXTERN_INT_BASE + 145)
#define ARCH_INT_SAI4                (HAL_EXTERN_INT_BASE + 146)
//#define ARCH_INT_NC                (HAL_EXTERN_INT_BASE + 147) // 保留
//#define ARCH_INT_NC                (HAL_EXTERN_INT_BASE + 148) // 保留
#define ARCH_INT_WKUP                (HAL_EXTERN_INT_BASE + 149)

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_INTS_H__
/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/
