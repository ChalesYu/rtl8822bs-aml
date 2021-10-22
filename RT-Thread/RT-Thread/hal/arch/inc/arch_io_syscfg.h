/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_syscfg.h
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         syscfgģ��Ĵ�����װ����ʵ������
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
#ifndef __ARCH_IO_SYSCFG_H__
#define __ARCH_IO_SYSCFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** ��̫���ӿ����ò�������
*********************************************************************************************************/
#define ETH_MAC_INTERFACE_MII       ~((rt_uint32_t)7 << 21)
#define ETH_MAC_INTERFACE_RMII      ((rt_uint32_t)4 << 21)

/*********************************************************************************************************
** EXTI�ж�Line�ӿڶ���
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
** �ⲿ����������
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
