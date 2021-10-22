/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_syscfg.c
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
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_syscfg.h"


/*********************************************************************************************************
  �ֲ���������
*********************************************************************************************************/


/*********************************************************************************************************
** Function name:       SyscfgEthMACInterfaceConfig
** Descriptions:        ������̫��mac�ĺ�phy֮��Ľӿڷ�ʽ
** input parameters:    u32WhichInterface���ӿڷ�ʽ���ã� ����ȡ����ֵ��
**                      ETH_MAC_INTERFACE_MII��         MAC������MIIģʽ
**                      ETH_MAC_INTERFACE_RMII��        MAC������RMIIģʽ
** output parameters:   NONE
** Returned value:      NONE
** ע�⣺ �ú�����������̫��MAC��ʱ��ʹ��֮ǰ����
*********************************************************************************************************/
void SyscfgEthMACInterfaceConfig(rt_uint32_t u32WhichInterface)
{
    if(ETH_MAC_INTERFACE_RMII ==  u32WhichInterface) {
        HWREG32(SYSCFG_BASE + SYSCFG_PMCR) |= ETH_MAC_INTERFACE_RMII;
    } else {
        HWREG32(SYSCFG_BASE + SYSCFG_PMCR) &= ETH_MAC_INTERFACE_MII;
    }

}

/*********************************************************************************************************
** Function name:       SyscfgCompensationCellCtrl
** Descriptions:        GPIO��Compensation Cell����
** input parameters:    bCtrl: ����ģʽ������ȡ����ֵ��
**                      RT_TRUE:   ʹ��Compensation Cell
**                      RT_FALSE:  ����Compensation Cell
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SyscfgCompensationCellCtrl(rt_bool_t bCtrl)
{
    if(RT_TRUE == bCtrl) {
        HWREG32(SYSCFG_BASE + SYSCFG_CCCSR) |= (rt_uint32_t)0x000000001;
    } else {
        HWREG32(SYSCFG_BASE + SYSCFG_CCCSR) &= ~((rt_uint32_t)0x000000001);
    }
}

/*********************************************************************************************************
** Function name:       SyscfgCompensationCellCtrl
** Descriptions:        GPIO��Compensation Cell����
** input parameters:    u32GPIOLinex:   GPIOA....GPIOI
**                      u32Pin:         ָ�����õ�GPIO���ţ�ȡ0-15
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SyscfgExtiLineConfig(rt_uint32_t u32GPIOLinex, rt_uint32_t u32Pin)
{
    rt_uint32_t u32Temp = u32Pin / 4;
    rt_uint32_t u32Value = u32Pin % 4;

    u32Temp *= 4;
    u32Value *= 4;
    HWREG32(SYSCFG_BASE + SYSCFG_EXTICR1 + u32Temp) &= ~(((rt_uint32_t)0x0000000F) << u32Value);
    HWREG32(SYSCFG_BASE + SYSCFG_EXTICR1 + u32Temp) |= (u32GPIOLinex << u32Value);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
