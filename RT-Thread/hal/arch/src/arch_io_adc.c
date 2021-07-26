/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_adc.c
** Last modified Date:  2017-01-30
** Last Version:        v1.0
** Description:         adc模块寄存器封装函数实现，适用于STM32H7xx
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
#include <rtthread.h>
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_adc.h"

/*********************************************************************************************************
  局部变量定义
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       AdcInit
** Descriptions:        ADC初始化
** input parameters:    u32Base:    指定操作的ADC模块基地址
**                      u32ClkPrescaler: 时钟分频系数，可以取下列值：
**                      异步模式取下列值：
**                      ADC_CLOCK_SYNC_PCLK_DIV1      
**                      ADC_CLOCK_SYNC_PCLK_DIV2     
**                      ADC_CLOCK_SYNC_PCLK_DIV4  
**                      同步模式取下列值：
**                      ADC_CLOCK_ASYNC_DIV1      不分频
**                      ADC_CLOCK_ASYNC_DIV2      (1UL<<18)
**                      ADC_CLOCK_ASYNC_DIV4      (2UL<<18)
**                      ADC_CLOCK_ASYNC_DIV6      (3UL<<18)
**                      ADC_CLOCK_ASYNC_DIV8      (4UL<<18)
**                      ADC_CLOCK_ASYNC_DIV10     (5UL<<18)
**                      ADC_CLOCK_ASYNC_DIV12     (6UL<<18)
**                      ADC_CLOCK_ASYNC_DIV16     (7UL<<18)
**                      ADC_CLOCK_ASYNC_DIV32     (8UL<<18)
**                      ADC_CLOCK_ASYNC_DIV64     (9UL<<18)
**                      ADC_CLOCK_ASYNC_DIV128    (10UL<<18)
**                      ADC_CLOCK_ASYNC_DIV256    (11UL<<18)
**
**
**                      u32Cfg:  配置参数1,可以取下面几组参数的或
**                      ADC_REGULAR_MODE          普通模式
**                      ADC_DMA_ONE_SHOT_MODE     DMA单次模式
**                      ADC_DFSDM_MODE            DFSDM模式
**                      ADC_DMA_CIRCULAR_MODE     DMA环形模式
**
**                      ADC_RESOLUTION_16B        16位ADC
**                      ADC_RESOLUTION_14B        14位ADC
**                      ADC_RESOLUTION_12B        12位ADC
**                      ADC_RESOLUTION_10B        10位ADC
**                      ADC_RESOLUTION_8B         8位ADC
**                      
**                      ADC_EXTERNALTRIGCONVEDGE_SOFTWARE       软件触发采样
**                      ADC_EXTERNALTRIGCONVEDGE_RISING         硬件上升沿触发
**                      ADC_EXTERNALTRIGCONVEDGE_FALLING        硬件下降沿触发
**                      ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING  硬件上升沿下降沿同时触发 
**                                                                         
**                      ADC_EXTERNALTRIG_T1_CC1           外部触发T1_CC1
**                      ADC_EXTERNALTRIG_T1_CC2           外部触发T1_CC2
**                      ADC_EXTERNALTRIG_T1_CC3           外部触发T1_CC3
**                      ADC_EXTERNALTRIG_T2_CC2           外部触发T2_CC2
**                      ADC_EXTERNALTRIG_T3_TRGO          外部触发T3_TRGO
**                      ADC_EXTERNALTRIG_T4_CC4           外部触发T4_CC4
**                      ADC_EXTERNALTRIG_EXT_IT11         外部触发EXT_IT11
**                      ADC_EXTERNALTRIG_T8_TRGO          外部触发T8_TRGO 
**                      ADC_EXTERNALTRIG_T8_TRGO2         外部触发T8_TRGO2 
**                      ADC_EXTERNALTRIG_T1_TRGO          外部触发T1_TRGO 
**                      ADC_EXTERNALTRIG_T1_TRGO2         外部触发T1_TRGO2
**                      ADC_EXTERNALTRIG_T2_TRGO          外部触发T2_TRGO 
**                      ADC_EXTERNALTRIG_T4_TRGO          外部触发T4_TRGO
**                      ADC_EXTERNALTRIG_T6_TRGO          外部触发T6_TRGO 
**                      ADC_EXTERNALTRIG_T15_TRGO         外部触发T15_TRGO 
**                      ADC_EXTERNALTRIG_T3_CC4           外部触发T3_CC4  
**                      
**                      ADC_OVR_DATA_PRESERVED            数据为读取时停止采样
**                      ADC_OVR_DATA_OVERWRITTEN          数据为读取时覆盖原采样值
**                      
**                      ADC_Single_Conversion_Mode        单次采样模式
**                      ADC_Continuous_Conversion_Mode    连续采样模式
**                      
**                      ADC_Auto_Delayed_Mode_Off         关闭自动延时
**                      ADC_Auto_Delayed_Mode_On          开启自动延时
**
**
**                      u32Cfg2: 配置参数2,可以取下列几组参数的或
**                      ADC_Regular_Oversampling_Enable   Regular过采样模式使能
**                      ADC_Regular_Oversampling_Disable  Regular过采样模式禁止 
**                      
**                      ADC_Injected_Oversampling_Enable  Injected过采样模式使能
**                      ADC_Injected_Oversampling_Disable Injected过采样模式使能
**
**                      ADC_Left_Shift(x)             ADC左移动位数，决定对齐方式，左对齐还是右对齐
**
**                      ADC_Oversampling_Ratio(x)      过采样因数
**
**                      ADC_Oversampleing_Right_Shift(x)   过采样右移动位数，
  
**                      ADC_TROVS_Follow_A_Trigger        一次触发各个通道采样
**                      ADC_TROVS_Need_New_Trigger        每个通道需要一次单独的触发
  
**                      ADC_ROVSM_Continued_Mode          When injected conversions are triggered, the oversampling is temporary
**                                                        stopped and continued after the injection sequence (oversampling buffer 
**                                                        is maintained during injected sequence)
**                      ADC_ROVSM_Resumed_Mode            When injected conversions are triggered, the current oversampling is aborted
**                                                        and resumed from start after the injection sequence (oversampling buffer is 
**                                                        zeroed by injected sequence start)
  
**                      ADC_ROVSM_Continued_Mode          When injected conversions are triggered, the oversampling is temporary
**                                                        stopped and continued after the injection sequence (oversampling buffer 
**                                                        is maintained during injected sequence)
**                      ADC_ROVSM_Resumed_Mode            When injected conversions are triggered, the current oversampling is aborted
**                                                        and resumed from start after the injection sequence (oversampling buffer is 
**                                                        zeroed by injected sequence start)
** output parameters:   NONE
** Returned value:      NONE
** 注意：该函数必须在ADC Disbale的状态下才能调用，通常在注册设备驱动的最初阶段调用，以后不再重新初始化
*********************************************************************************************************/
void AdcInit(rt_uint32_t u32Base, rt_uint32_t u32ClkPrescaler, rt_uint32_t u32Cfg, rt_uint32_t u32Cfg2)
{
  rt_uint32_t adc_common_base;
  
  /* - Exit from deep-power-down mode and ADC voltage regulator enable        */    
  /*  Exit deep power down mode if still in that state */
  if(HWREG32(u32Base + ADC_CR) & (rt_uint32_t)(1 << 29)) {
    /* Exit deep power down mode */ 
    HWREG32(u32Base + ADC_CR) &= ~((rt_uint32_t)(1 << 29));
    /* System was in deep power down mode, calibration must
    be relaunched or a previously saved calibration factor 
    re-applied once the ADC voltage regulator is enabled */ 
  }
  
  if(!(HWREG32(u32Base + ADC_CR) & (rt_uint32_t)(1 << 28))) {
    HWREG32(u32Base + ADC_CR) |= (rt_uint32_t)(1 << 28);
    /* Enable ADC internal voltage regulator  then 
     wait for start-up time */
    rt_thread_delay(2);
  }
  
  if((u32Base == ADC1_BASE) || (u32Base == ADC2_BASE)) {
    adc_common_base = ADC12_COMMON_BASE;
  } else {
    adc_common_base = ADC3_COMMON_BASE;
  }
  // 配置时钟
  HWREG32(adc_common_base + ADC_CCR) &= ~((rt_uint32_t)(0x003F0000));
  HWREG32(adc_common_base + ADC_CCR) |= u32ClkPrescaler; 
  
  
  HWREG32(u32Base + ADC_CFGR) = 0x80000000;
  HWREG32(u32Base + ADC_CFGR) |= u32Cfg;
  HWREG32(u32Base + ADC_CFGR2) = 0x0000000;
  HWREG32(u32Base + ADC_CFGR2) |= u32Cfg2;
  
  // 单端输入校准
  HWREG32(u32Base + ADC_CR) &= ~((rt_uint32_t)(1<<30));
  HWREG32(u32Base + ADC_CR) |= (1UL << 31);
  while(HWREG32(u32Base + ADC_CR) & (1UL << 31));
  //双端输入校准
  HWREG32(u32Base + ADC_CR) |= (rt_uint32_t)(1<<30);
  HWREG32(u32Base + ADC_CR) |= (1UL << 31);
  while(HWREG32(u32Base + ADC_CR) & (1UL << 31));   

  HWREG32(u32Base + ADC_CR) &= ~((rt_uint32_t)(1<<30));  
}

/*********************************************************************************************************
** Function name:       AdcBoostModeEnable
** Descriptions:        ADC使能Boost Mode, ADC clock大于20M时必须使能
** input parameters:    u32Base:    指定使能的ADC模块
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void AdcBoostModeEnable(rt_uint32_t u32Base)
{
  HWREG32(u32Base + ADC_CR) |= (1UL << 8);
}

/*********************************************************************************************************
** Function name:       AdcBoostModeEnable
** Descriptions:        ADC禁止Boost Mode, ADC clock大于20M时必须禁止
** input parameters:    u32Base:    指定使能的ADC模块
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void AdcBoostModeDisable(rt_uint32_t u32Base)
{
  HWREG32(u32Base + ADC_CR) &= ~(1UL << 8);
}

/*********************************************************************************************************
** Function name:       AdcEnable
** Descriptions:        ADC使能
** input parameters:    u32Base:    指定使能的ADC模块
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void AdcEnable(rt_uint32_t u32Base)
{
  /* - Exit from deep-power-down mode and ADC voltage regulator enable        */    
  /*  Exit deep power down mode if still in that state */
  if(HWREG32(u32Base + ADC_CR) & (rt_uint32_t)(1 << 29)) {
    /* Exit deep power down mode */ 
    HWREG32(u32Base + ADC_CR) &= ~((rt_uint32_t)(1 << 29));
    /* System was in deep power down mode, calibration must
    be relaunched or a previously saved calibration factor 
    re-applied once the ADC voltage regulator is enabled */ 
  }
  
  if(!(HWREG32(u32Base + ADC_CR) & (rt_uint32_t)(1 << 28))) {
    HWREG32(u32Base + ADC_CR) |= (rt_uint32_t)(1 << 28);
    /* Enable ADC internal voltage regulator  then 
    wait for start-up time */
    rt_thread_delay(2);
  }
  HWREG32(u32Base + ADC_CR) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       AdcDisable
** Descriptions:        ADC禁止
** input parameters:    u32Base:    指定禁止的ADC模块
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void AdcDisable(rt_uint32_t u32Base)
{
  HWREG32(u32Base + ADC_CR) &= 0xFFFFFFFE;
  if(HWREG32(u32Base + ADC_CR) &= 0x00000001) {
    HWREG32(u32Base + ADC_CR) |= 0x00000002;
    while(HWREG32(u32Base + ADC_CR) & 0x00000002);
  }
}

/*********************************************************************************************************
** Function name:       AdcSoftWareSampleOnceGet
** Descriptions:        ADC软件触发采样一次例子函数
** input parameters:    u32Base:    指定使能的ADC模块
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint16_t AdcSoftWareSampleOnceGet(rt_uint32_t u32Base, rt_uint8_t  u8Channel)
{
  rt_uint32_t u32Temp;
  
  AdcDisable(u32Base);
  
  u32Temp = HWREG32(u32Base + ADC_ISR);
  if(u32Temp != 0) {
    HWREG32(u32Base + ADC_ISR) = u32Temp;
  }
  if(u8Channel < 10) {
    HWREG32(u32Base + ADC_SMPR1) &= ~((rt_uint32_t)(7UL<<(u8Channel * 3))); 
  } else {
    HWREG32(u32Base + ADC_SMPR2) &= ~((rt_uint32_t)(7UL<<((u8Channel - 10) * 3))); 
  }
  HWREG32(u32Base + ADC_SQR1) &= 0xFFFFFFF0;
  HWREG32(u32Base + ADC_SQR1) &= 0xFFFFF7CF; //规则序列1
  HWREG32(u32Base + ADC_SQR1) |= (((rt_uint32_t)u8Channel) << 6); //规则序列1
  AdcEnable(u32Base);
  
  while(!(HWREG32(u32Base + ADC_ISR) & 0x00000001));  // 等待ADC Ready
  HWREG32(u32Base + ADC_CR) |= 0x00000004;  // 启动转换
  while(!(HWREG32(u32Base + ADC_ISR) & 0x00000004));  // 等待转换完成
  
  return HWREG16(u32Base + ADC_DR);           //返回adc值	
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
