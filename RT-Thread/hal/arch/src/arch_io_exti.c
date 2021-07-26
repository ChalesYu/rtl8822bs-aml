/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_exti.c
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         extiģ��Ĵ�����װ����ʵ������
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
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_exti.h"


/*********************************************************************************************************
  �ֲ���������
*********************************************************************************************************/
static const rt_uint32_t __gExtiRTSR[3] = {EXTI_RTSR1, EXTI_RTSR2, EXTI_RTSR3};
static const rt_uint32_t __gExtiFTSR[3] = {EXTI_FTSR1, EXTI_FTSR2, EXTI_FTSR3};
static const rt_uint32_t __gExtiSWIER[3] = {EXTI_SWIER1, EXTI_SWIER2, EXTI_SWIER3};
// D3��Event�Ժ��õ���д
//static const rt_uint32_t __gExtiD3PMR[3] = {EXTI_D3PMR1, EXTI_D3PMR2, EXTI_D3PMR3};
//static const rt_uint32_t __gExtiD3PCRL[3] = {EXTI_D3PCR1L, EXTI_D3PCR2L, EXTI_D3PCR3L};
//static const rt_uint32_t __gExtiD3PCRH[3] = {EXTI_D3PCR1H, EXTI_D3PCR2H, EXTI_D3PCR3H};

static const rt_uint32_t __gExtiCPUIMR[3] = {EXTI_CPUIMR1, EXTI_CPUIMR2, EXTI_CPUIMR3};
static const rt_uint32_t __gExtiCPUEMR[3] = {EXTI_CPUEMR1, EXTI_CPUEMR2, EXTI_CPUEMR3};
static const rt_uint32_t __gExtiCPUPR[3] = {EXTI_CPUPR1, EXTI_CPUPR2, EXTI_CPUPR3};

/*********************************************************************************************************
** Function name:       ExtiIntEnable
** Descriptions:        EXTI�ж�ʹ��
** input parameters:    u32Line:    �ⲿӲ���жϺţ�ȡ0-88
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void ExtiIntEnable(rt_uint32_t u32Line)
{
  rt_uint32_t index = u32Line / 32;
  rt_uint32_t pos = u32Line % 32;
  
    HWREG32(EXTI_BASE + __gExtiCPUIMR[index]) |= ((rt_uint32_t)1 << pos);
}

/*********************************************************************************************************
** Function name:       ExtiIntDisable
** Descriptions:        EXTI�жϽ�ֹ
** input parameters:    u32Line:    �ⲿӲ���жϺţ�ȡ0-88
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void ExtiIntDisable(rt_uint32_t u32Line)
{
  rt_uint32_t index = u32Line / 32;
  rt_uint32_t pos = u32Line % 32;
  
  HWREG32(EXTI_BASE + __gExtiCPUIMR[index]) &= ~((rt_uint32_t)1 << pos);
}

/*********************************************************************************************************
** Function name:       ExtiEventEnable
** Descriptions:        EXTI�¼�ʹ��
** input parameters:    u32Line:    �ⲿӲ���жϺţ�ȡ0-88
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void ExtiEventEnable(rt_uint32_t u32Line)
{
  rt_uint32_t index = u32Line / 32;
  rt_uint32_t pos = u32Line % 32;
  
  HWREG32(EXTI_BASE + __gExtiCPUEMR[index]) |= ((rt_uint32_t)1 << pos);
}

/*********************************************************************************************************
** Function name:       ExtiEventDisable
** Descriptions:        EXTI�¼���ֹ
** input parameters:    u32Line:    �ⲿӲ���жϺţ�ȡ0-88
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void ExtiEventDisable(rt_uint32_t u32Line)
{
  rt_uint32_t index = u32Line / 32;
  rt_uint32_t pos = u32Line % 32;
  
  HWREG32(EXTI_BASE + __gExtiCPUEMR[index]) &= ~((rt_uint32_t)1 << pos);
}

/*********************************************************************************************************
** Function name:       ExtiIntEventRisingEnable
** Descriptions:        EXTI�жϼ��¼�������ʹ��
** input parameters:    u32Line:    �ⲿӲ���жϺţ�ȡ0-88
** output parameters:   NONE
** Returned value:      NONE
** ע�⣺�������غ��½���ͬʱʹ�ܵ�ʱ����ͬʱ��Ч
*********************************************************************************************************/
void ExtiIntEventRisingEnable(rt_uint32_t u32Line)
{
  rt_uint32_t index = u32Line / 32;
  rt_uint32_t pos = u32Line % 32;
  
  HWREG32(EXTI_BASE + __gExtiRTSR[index]) |= ((rt_uint32_t)1 << pos);
}

/*********************************************************************************************************
** Function name:       ExtiIntEventRisingDisable
** Descriptions:        EXTI�жϼ��¼������ؽ�ֹ
** input parameters:    u32Line:    �ⲿӲ���жϺţ�ȡ0-88
** output parameters:   NONE
** Returned value:      NONE
** ע�⣺�������غ��½���ͬʱʹ�ܵ�ʱ����ͬʱ��Ч
*********************************************************************************************************/
void ExtiIntEventRisingDisable(rt_uint32_t u32Line)
{
  rt_uint32_t index = u32Line / 32;
  rt_uint32_t pos = u32Line % 32;
  
  HWREG32(EXTI_BASE + __gExtiRTSR[index]) &= ~((rt_uint32_t)1 << pos);
}

/*********************************************************************************************************
** Function name:       ExtiIntEventFallingEnable
** Descriptions:        EXTI�жϼ��¼��½���ʹ��
** input parameters:    u32Line:    �ⲿӲ���жϺţ�ȡ0-88
** output parameters:   NONE
** Returned value:      NONE
** ע�⣺�������غ��½���ͬʱʹ�ܵ�ʱ����ͬʱ��Ч
*********************************************************************************************************/
void ExtiIntEventFallingEnable(rt_uint32_t u32Line)
{
  rt_uint32_t index = u32Line / 32;
  rt_uint32_t pos = u32Line % 32;
  
  HWREG32(EXTI_BASE + __gExtiFTSR[index]) |= ((rt_uint32_t)1 << pos);
}

/*********************************************************************************************************
** Function name:       ExtiIntEventFallingDisable
** Descriptions:        EXTI�жϼ��¼��½��ؽ�ֹ
** input parameters:    u32Line:    �ⲿӲ���жϺţ�ȡ0-88
** output parameters:   NONE
** Returned value:      NONE
** ע�⣺�������غ��½���ͬʱʹ�ܵ�ʱ����ͬʱ��Ч
*********************************************************************************************************/
void ExtiIntEventFallingDisable(rt_uint32_t u32Line)
{
  rt_uint32_t index = u32Line / 32;
  rt_uint32_t pos = u32Line % 32;
  
  HWREG32(EXTI_BASE + __gExtiFTSR[index]) &= ~((rt_uint32_t)1 << pos);
}

/*********************************************************************************************************
** Function name:       ExtiSoftInterruptTrigger
** Descriptions:        EXTI�жϼ��¼��������
** input parameters:    u32Line:    �ⲿӲ���жϺţ�ȡ0-88
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void ExtiSoftInterruptTrigger(rt_uint32_t u32Line)
{
  rt_uint32_t index = u32Line / 32;
  rt_uint32_t pos = u32Line % 32;
  
  HWREG32(EXTI_BASE + __gExtiSWIER[index]) |= ((rt_uint32_t)1 << u32Line);
}

/*********************************************************************************************************
** Function name:       ExtiIntEventClear
** Descriptions:        EXTI�жϼ��¼����
** input parameters:    u32Line:    �ⲿӲ���жϺţ�ȡ0-88
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void ExtiIntEventClear(rt_uint32_t u32Line)
{
  rt_uint32_t index = u32Line / 32;
  rt_uint32_t pos = u32Line % 32;
  
  HWREG32(EXTI_BASE + __gExtiCPUPR[index]) |= ((rt_uint32_t)1 << u32Line);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
