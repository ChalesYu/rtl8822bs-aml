/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_exti.h
** Last modified Date:  2013-04-04
** Last Version:        v1.0
** Description:         exti模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2013-04-04
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
#ifndef __ARCH_IO_EXTI_H__
#define __ARCH_IO_EXTI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** 外部函数的声明
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
