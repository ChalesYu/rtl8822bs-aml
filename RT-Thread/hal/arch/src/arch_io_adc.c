/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_adc.c
** Last modified Date:  2017-01-30
** Last Version:        v1.0
** Description:         adcģ��Ĵ�����װ����ʵ�֣�������STM32H7xx
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2017-01-30
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
#include <rtthread.h>
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_adc.h"

/*********************************************************************************************************
  �ֲ���������
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       AdcInit
** Descriptions:        ADC��ʼ��
** input parameters:    u32Base:    ָ��������ADCģ�����ַ
**                      u32ClkPrescaler: ʱ�ӷ�Ƶϵ��������ȡ����ֵ��
**                      �첽ģʽȡ����ֵ��
**                      ADC_CLOCK_SYNC_PCLK_DIV1      
**                      ADC_CLOCK_SYNC_PCLK_DIV2     
**                      ADC_CLOCK_SYNC_PCLK_DIV4  
**                      ͬ��ģʽȡ����ֵ��
**                      ADC_CLOCK_ASYNC_DIV1      ����Ƶ
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
**                      u32Cfg:  ���ò���1,����ȡ���漸������Ļ�
**                      ADC_REGULAR_MODE          ��ͨģʽ
**                      ADC_DMA_ONE_SHOT_MODE     DMA����ģʽ
**                      ADC_DFSDM_MODE            DFSDMģʽ
**                      ADC_DMA_CIRCULAR_MODE     DMA����ģʽ
**
**                      ADC_RESOLUTION_16B        16λADC
**                      ADC_RESOLUTION_14B        14λADC
**                      ADC_RESOLUTION_12B        12λADC
**                      ADC_RESOLUTION_10B        10λADC
**                      ADC_RESOLUTION_8B         8λADC
**                      
**                      ADC_EXTERNALTRIGCONVEDGE_SOFTWARE       �����������
**                      ADC_EXTERNALTRIGCONVEDGE_RISING         Ӳ�������ش���
**                      ADC_EXTERNALTRIGCONVEDGE_FALLING        Ӳ���½��ش���
**                      ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING  Ӳ���������½���ͬʱ���� 
**                                                                         
**                      ADC_EXTERNALTRIG_T1_CC1           �ⲿ����T1_CC1
**                      ADC_EXTERNALTRIG_T1_CC2           �ⲿ����T1_CC2
**                      ADC_EXTERNALTRIG_T1_CC3           �ⲿ����T1_CC3
**                      ADC_EXTERNALTRIG_T2_CC2           �ⲿ����T2_CC2
**                      ADC_EXTERNALTRIG_T3_TRGO          �ⲿ����T3_TRGO
**                      ADC_EXTERNALTRIG_T4_CC4           �ⲿ����T4_CC4
**                      ADC_EXTERNALTRIG_EXT_IT11         �ⲿ����EXT_IT11
**                      ADC_EXTERNALTRIG_T8_TRGO          �ⲿ����T8_TRGO 
**                      ADC_EXTERNALTRIG_T8_TRGO2         �ⲿ����T8_TRGO2 
**                      ADC_EXTERNALTRIG_T1_TRGO          �ⲿ����T1_TRGO 
**                      ADC_EXTERNALTRIG_T1_TRGO2         �ⲿ����T1_TRGO2
**                      ADC_EXTERNALTRIG_T2_TRGO          �ⲿ����T2_TRGO 
**                      ADC_EXTERNALTRIG_T4_TRGO          �ⲿ����T4_TRGO
**                      ADC_EXTERNALTRIG_T6_TRGO          �ⲿ����T6_TRGO 
**                      ADC_EXTERNALTRIG_T15_TRGO         �ⲿ����T15_TRGO 
**                      ADC_EXTERNALTRIG_T3_CC4           �ⲿ����T3_CC4  
**                      
**                      ADC_OVR_DATA_PRESERVED            ����Ϊ��ȡʱֹͣ����
**                      ADC_OVR_DATA_OVERWRITTEN          ����Ϊ��ȡʱ����ԭ����ֵ
**                      
**                      ADC_Single_Conversion_Mode        ���β���ģʽ
**                      ADC_Continuous_Conversion_Mode    ��������ģʽ
**                      
**                      ADC_Auto_Delayed_Mode_Off         �ر��Զ���ʱ
**                      ADC_Auto_Delayed_Mode_On          �����Զ���ʱ
**
**
**                      u32Cfg2: ���ò���2,����ȡ���м�������Ļ�
**                      ADC_Regular_Oversampling_Enable   Regular������ģʽʹ��
**                      ADC_Regular_Oversampling_Disable  Regular������ģʽ��ֹ 
**                      
**                      ADC_Injected_Oversampling_Enable  Injected������ģʽʹ��
**                      ADC_Injected_Oversampling_Disable Injected������ģʽʹ��
**
**                      ADC_Left_Shift(x)             ADC���ƶ�λ�����������뷽ʽ������뻹���Ҷ���
**
**                      ADC_Oversampling_Ratio(x)      ����������
**
**                      ADC_Oversampleing_Right_Shift(x)   ���������ƶ�λ����
  
**                      ADC_TROVS_Follow_A_Trigger        һ�δ�������ͨ������
**                      ADC_TROVS_Need_New_Trigger        ÿ��ͨ����Ҫһ�ε����Ĵ���
  
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
** ע�⣺�ú���������ADC Disbale��״̬�²��ܵ��ã�ͨ����ע���豸����������׶ε��ã��Ժ������³�ʼ��
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
  // ����ʱ��
  HWREG32(adc_common_base + ADC_CCR) &= ~((rt_uint32_t)(0x003F0000));
  HWREG32(adc_common_base + ADC_CCR) |= u32ClkPrescaler; 
  
  
  HWREG32(u32Base + ADC_CFGR) = 0x80000000;
  HWREG32(u32Base + ADC_CFGR) |= u32Cfg;
  HWREG32(u32Base + ADC_CFGR2) = 0x0000000;
  HWREG32(u32Base + ADC_CFGR2) |= u32Cfg2;
  
  // ��������У׼
  HWREG32(u32Base + ADC_CR) &= ~((rt_uint32_t)(1<<30));
  HWREG32(u32Base + ADC_CR) |= (1UL << 31);
  while(HWREG32(u32Base + ADC_CR) & (1UL << 31));
  //˫������У׼
  HWREG32(u32Base + ADC_CR) |= (rt_uint32_t)(1<<30);
  HWREG32(u32Base + ADC_CR) |= (1UL << 31);
  while(HWREG32(u32Base + ADC_CR) & (1UL << 31));   

  HWREG32(u32Base + ADC_CR) &= ~((rt_uint32_t)(1<<30));  
}

/*********************************************************************************************************
** Function name:       AdcBoostModeEnable
** Descriptions:        ADCʹ��Boost Mode, ADC clock����20Mʱ����ʹ��
** input parameters:    u32Base:    ָ��ʹ�ܵ�ADCģ��
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void AdcBoostModeEnable(rt_uint32_t u32Base)
{
  HWREG32(u32Base + ADC_CR) |= (1UL << 8);
}

/*********************************************************************************************************
** Function name:       AdcBoostModeEnable
** Descriptions:        ADC��ֹBoost Mode, ADC clock����20Mʱ�����ֹ
** input parameters:    u32Base:    ָ��ʹ�ܵ�ADCģ��
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void AdcBoostModeDisable(rt_uint32_t u32Base)
{
  HWREG32(u32Base + ADC_CR) &= ~(1UL << 8);
}

/*********************************************************************************************************
** Function name:       AdcEnable
** Descriptions:        ADCʹ��
** input parameters:    u32Base:    ָ��ʹ�ܵ�ADCģ��
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
** Descriptions:        ADC��ֹ
** input parameters:    u32Base:    ָ����ֹ��ADCģ��
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
** Descriptions:        ADC�����������һ�����Ӻ���
** input parameters:    u32Base:    ָ��ʹ�ܵ�ADCģ��
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
  HWREG32(u32Base + ADC_SQR1) &= 0xFFFFF7CF; //��������1
  HWREG32(u32Base + ADC_SQR1) |= (((rt_uint32_t)u8Channel) << 6); //��������1
  AdcEnable(u32Base);
  
  while(!(HWREG32(u32Base + ADC_ISR) & 0x00000001));  // �ȴ�ADC Ready
  HWREG32(u32Base + ADC_CR) |= 0x00000004;  // ����ת��
  while(!(HWREG32(u32Base + ADC_ISR) & 0x00000004));  // �ȴ�ת�����
  
  return HWREG16(u32Base + ADC_DR);           //����adcֵ	
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
