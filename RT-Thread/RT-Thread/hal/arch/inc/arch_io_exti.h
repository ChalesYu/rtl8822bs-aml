/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_exti.h
** Last modified Date:  2013-04-04
** Last Version:        v1.0
** Description:         extiģ��Ĵ�����װ����ʵ������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2013-04-04
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
#ifndef __ARCH_IO_EXTI_H__
#define __ARCH_IO_EXTI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** �ⲿ����������
*********************************************************************************************************/
extern void ExtiIntEnable(rt_uint32_t u32Line);
extern void ExtiIntDisable(rt_uint32_t u32Line);
extern void ExtiEventEnable(rt_uint32_t u32Line);
extern void ExtiEventDisable(rt_uint32_t u32Line);
extern void ExtiIntEventRisingEnable(rt_uint32_t u32Line);
extern void ExtiIntEventRisingDisable(rt_uint32_t u32Line);
extern void ExtiIntEventFallingEnable(rt_uint32_t u32Line);
extern void ExtiIntEventFallingDisable(rt_uint32_t u32Line);
extern void ExtiSoftInterruptTrigger(rt_uint32_t u32Line);
extern void ExtiIntEventClear(rt_uint32_t u32Line);


#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_EXTI_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
