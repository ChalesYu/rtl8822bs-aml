/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_syscfg.c
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
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_syscfg.h"


/*********************************************************************************************************
  局部变量定义
*********************************************************************************************************/


/*********************************************************************************************************
** Function name:       SyscfgEthMACInterfaceConfig
** Descriptions:        配置以太网mac的和phy之间的接口方式
** input parameters:    u32WhichInterface：接口方式配置， 可以取下列值：
**                      ETH_MAC_INTERFACE_MII：         MAC工作在MII模式
**                      ETH_MAC_INTERFACE_RMII：        MAC工作在RMII模式
** output parameters:   NONE
** Returned value:      NONE
** 注意： 该函数必须在以太网MAC的时钟使能之前调用
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
** Descriptions:        GPIO的Compensation Cell控制
** input parameters:    bCtrl: 控制模式，可以取下列值：
**                      RT_TRUE:   使能Compensation Cell
**                      RT_FALSE:  禁能Compensation Cell
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
** Descriptions:        GPIO的Compensation Cell控制
** input parameters:    u32GPIOLinex:   GPIOA....GPIOI
**                      u32Pin:         指定配置的GPIO引脚，取0-15
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
