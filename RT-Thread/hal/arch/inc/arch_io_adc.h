/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_adc.h
** Last modified Date:  2017-01-30
** Last Version:        v1.0
** Description:         adc模块寄存器封装函数实现声明，适用于STM32H7xx
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2017-01-30
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
#ifndef __ARCH_IO_ADC_H__
#define __ARCH_IO_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** 时钟分频系数定义
*********************************************************************************************************/
//　同步模式时钟定义
#define ADC_CLOCK_SYNC_PCLK_DIV1      (1UL<<16)  /*!< ADC synchronous clock derived from AHB clock not divided  */
#define ADC_CLOCK_SYNC_PCLK_DIV2      (2UL<<16)  /*!< ADC synchronous clock derived from AHB clock divided by 2 */
#define ADC_CLOCK_SYNC_PCLK_DIV4      (3UL<<16)    /*!< ADC synchronous clock derived from AHB clock divided by 4 */

//#define ADC_CLOCKPRESCALER_PCLK_DIV1   ADC_CLOCK_SYNC_PCLK_DIV1     /*!< Obsolete naming, kept for compatibility with some other devices */
//#define ADC_CLOCKPRESCALER_PCLK_DIV2   ADC_CLOCK_SYNC_PCLK_DIV2     /*!< Obsolete naming, kept for compatibility with some other devices */
//#define ADC_CLOCKPRESCALER_PCLK_DIV4   ADC_CLOCK_SYNC_PCLK_DIV4     /*!< Obsolete naming, kept for compatibility with some other devices */

#define ADC_CLOCK_ASYNC_DIV1      (0UL<<18)
#define ADC_CLOCK_ASYNC_DIV2      (1UL<<18)
#define ADC_CLOCK_ASYNC_DIV4      (2UL<<18)
#define ADC_CLOCK_ASYNC_DIV6      (3UL<<18)
#define ADC_CLOCK_ASYNC_DIV8      (4UL<<18)
#define ADC_CLOCK_ASYNC_DIV10     (5UL<<18)
#define ADC_CLOCK_ASYNC_DIV12     (6UL<<18)
#define ADC_CLOCK_ASYNC_DIV16     (7UL<<18)
#define ADC_CLOCK_ASYNC_DIV32     (8UL<<18)
#define ADC_CLOCK_ASYNC_DIV64     (9UL<<18)
#define ADC_CLOCK_ASYNC_DIV128    (10UL<<18)
#define ADC_CLOCK_ASYNC_DIV256    (11UL<<18)
  
/*********************************************************************************************************
** 配置参数1定义
*********************************************************************************************************/
#define ADC_REGULAR_MODE          (0UL << 0)
#define ADC_DMA_ONE_SHOT_MODE     (1UL << 0)
#define ADC_DFSDM_MODE            (2UL << 0)
#define ADC_DMA_CIRCULAR_MODE     (3UL << 0)
  
#define ADC_RESOLUTION_16B        (0UL<<2)
#define ADC_RESOLUTION_14B        (1UL<<2)
#define ADC_RESOLUTION_12B        (2UL<<2)
#define ADC_RESOLUTION_10B        (3UL<<2)
#define ADC_RESOLUTION_8B         (4UL<<2)
  
#define ADC_EXTERNALTRIGCONVEDGE_SOFTWARE       (0UL<<10)
#define ADC_EXTERNALTRIGCONVEDGE_RISING         (1UL<<10)
#define ADC_EXTERNALTRIGCONVEDGE_FALLING        (2UL<<10)
#define ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING  (3UL<<10) 
  
/* External triggers of ADC regular group */  
#define ADC_EXTERNALTRIG_T1_CC1           (0UL<<5)
#define ADC_EXTERNALTRIG_T1_CC2           (1UL<<5)
#define ADC_EXTERNALTRIG_T1_CC3           (2UL<<5)
#define ADC_EXTERNALTRIG_T2_CC2           (3UL<<5)
#define ADC_EXTERNALTRIG_T3_TRGO          (4UL<<5)
#define ADC_EXTERNALTRIG_T4_CC4           (5UL<<5)
#define ADC_EXTERNALTRIG_EXT_IT11         (6UL<<5)
#define ADC_EXTERNALTRIG_T8_TRGO          (7UL<<5)
#define ADC_EXTERNALTRIG_T8_TRGO2         (8UL<<5)
#define ADC_EXTERNALTRIG_T1_TRGO          (9UL<<5)
#define ADC_EXTERNALTRIG_T1_TRGO2         (10UL<<5)
#define ADC_EXTERNALTRIG_T2_TRGO          (11UL<<5)
#define ADC_EXTERNALTRIG_T4_TRGO          (12UL<<5)
#define ADC_EXTERNALTRIG_T6_TRGO          (13UL<<5)
#define ADC_EXTERNALTRIG_T15_TRGO         (14UL<<5)
#define ADC_EXTERNALTRIG_T3_CC4           (15UL<<5)  
#define ADC_EXTERNALTRIG_HR1_ADCTRG1      (16UL<<5)
#define ADC_EXTERNALTRIG_HR1_ADCTRG3      (17UL<<5)
#define ADC_EXTERNALTRIG_LPTIM1_OUT       (18UL<<5)
#define ADC_EXTERNALTRIG_LPTIM2_OUT       (19UL<<5)
#define ADC_EXTERNALTRIG_LPTIM3_OUT       (20UL<<5)
  
#define ADC_OVR_DATA_PRESERVED            (0UL<<12)      /*!< Data preserved in case of overrun   */
#define ADC_OVR_DATA_OVERWRITTEN          (1UL<<12)      /*!< Data overwritten in case of overrun */
  
  
#define ADC_Single_Conversion_Mode        (0UL<<13)
#define ADC_Continuous_Conversion_Mode    (1UL<<13)
  
#define ADC_Discontinuous_Mode_Enable     (1UL<<16)
#define ADC_Discontinuous_Mode_Disable    (0UL<<16)
  
#define ADC_Auto_Delayed_Mode_Off         (0UL<<14)
#define ADC_Auto_Delayed_Mode_On          (1UL<<14)
  
  
/*********************************************************************************************************
** 配置参数2定义
*********************************************************************************************************/
#define ADC_Regular_Oversampling_Enable   (1UL<<0)
#define ADC_Regular_Oversampling_Disable  (0UL<<0) 

#define ADC_Injected_Oversampling_Enable  (1UL<<1)
#define ADC_Injected_Oversampling_Disable (0UL<<1)
  
#define ADC_Left_Shift(x)                  (x << 28)

#define ADC_Oversampling_Ratio(x)          (x << 16)

#define ADC_Oversampleing_Right_Shift(x)   (x << 5)
  
#define ADC_TROVS_Follow_A_Trigger        (0UL<<9)
#define ADC_TROVS_Need_New_Trigger        (1UL<<9)
  
#define ADC_ROVSM_Continued_Mode          (0UL<<10)
#define ADC_ROVSM_Resumed_Mode            (1UL<<10)
  
/*********************************************************************************************************
** 外部函数的声明
*********************************************************************************************************/
extern void AdcInit(rt_uint32_t u32Base, rt_uint32_t u32ClkPrescaler, rt_uint32_t u32Cfg, rt_uint32_t u32Cfg2);
extern void AdcEnable(rt_uint32_t u32Base);
extern void AdcDisable(rt_uint32_t u32Base);
extern rt_uint16_t AdcSoftWareSampleOnceGet(rt_uint32_t u32Base, rt_uint8_t  u8Channel);

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_ADC_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
