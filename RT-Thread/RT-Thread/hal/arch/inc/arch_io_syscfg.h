/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_syscfg.h
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         syscfg模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2019-03-15
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
#ifndef __ARCH_IO_SYSCFG_H__
#define __ARCH_IO_SYSCFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** 以太网接口配置参数定义
*********************************************************************************************************/
#define ETH_MAC_INTERFACE_MII       ~((rt_uint32_t)7 << 21)
#define ETH_MAC_INTERFACE_RMII      ((rt_uint32_t)4 << 21)

/*********************************************************************************************************
** EXTI中断Line接口定义
*********************************************************************************************************/
#define EXTI_GPIO_A                 0x00000000
#define EXTI_GPIO_B                 0x00000001
#define EXTI_GPIO_C                 0x00000002
#define EXTI_GPIO_D                 0x00000003
#define EXTI_GPIO_E                 0x00000004
#define EXTI_GPIO_F                 0x00000005
#define EXTI_GPIO_G                 0x00000006
#define EXTI_GPIO_H                 0x00000007
#define EXTI_GPIO_I                 0x00000008
#define EXTI_GPIO_J                 0x00000009
#define EXTI_GPIO_K                 0x0000000A

/*********************************************************************************************************
** 外部函数的声明
*********************************************************************************************************/
extern void SyscfgEthMACInterfaceConfig(rt_uint32_t u32WhichInterface);
extern void SyscfgCompensationCellCtrl(rt_bool_t bCtrl);
extern void SyscfgExtiLineConfig(rt_uint32_t u32GPIOLinex, rt_uint32_t u32Pin);



#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_SYSCFG_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
