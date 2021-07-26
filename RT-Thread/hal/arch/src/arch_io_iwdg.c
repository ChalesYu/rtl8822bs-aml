/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_iwdg.c
** Last modified Date:  2013-04-02
** Last Version:        v1.0
** Description:         gpio模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2013-04-02
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
#include "hal/arch/inc/arch_io_iwdg.h"

/*********************************************************************************************************
** Function name:       iwdgAccessEnable
** Descriptions:        使能独立看门狗访问，该函数必须首先调用才能访问别的寄存器
** input parameters:    u32Base:     独立看门狗模块的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void iwdgAccessEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + IWDG_KR) = 0x5555;
}

/*********************************************************************************************************
** Function name:       iwdgAccessEnable
** Descriptions:        禁能独立看门狗访问
** input parameters:    u32Base:     独立看门狗模块的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void iwdgAccessDisable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + IWDG_KR) = 0x0000;
}

/*********************************************************************************************************
** Function name:       iwdgPrescalerSet
** Descriptions:        设置预分频值
** input parameters:    u32Base:     独立看门狗模块的基地址
**                      u8Div:      预分频值，可以去下列值中的一个：
**                      IWDG_DIV_4              // 4分频
**                      IWDG_DIV_8              // 8分频
**                      IWDG_DIV_16             // 16分频
**                      IWDG_DIV_32             // 32分频
**                      IWDG_DIV_64             // 64分频
**                      IWDG_DIV_128            // 128分频
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void iwdgPrescalerSet(rt_uint32_t u32Base, rt_uint8_t u8Div)
{
    HWREG32(u32Base + IWDG_PR) = u8Div;
}

/*********************************************************************************************************
** Function name:       iwdgReloadSet
** Descriptions:        设置独立看门狗的重装载值
** input parameters:    u32Base:     独立看门狗模块的基地址
**                      u16Count:    重新装载的值
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void iwdgReloadSet(rt_uint32_t u32Base,rt_uint16_t u16Count)
{
    HWREG16(u32Base + IWDG_RLR) = u16Count;
}

/*********************************************************************************************************
** Function name:       iwdgReloadEnable
** Descriptions:        使能重新装载独立看门狗的计数值
** input parameters:    u32Base:     独立看门狗模块的基地址
**                      ulCount:    重新装载的值
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void iwdgReloadEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + IWDG_KR) = 0xAAAA;
}

/*********************************************************************************************************
** Function name:       idwgEnable
** Descriptions:        使能独立看门狗，看门狗开始工作
** input parameters:    u32Base:     独立看门狗模块的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void idwgEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + IWDG_KR) = 0xCCCC;
}

/*********************************************************************************************************
** Function name:       idwgStatusGet
** Descriptions:        获取独立看门狗的状态，是否使能
** input parameters:    u32Base:     独立看门狗模块的基地址
** output parameters:   NONE
** Returned value:      返回独立看门狗的状态，如果第0位为1，表示看门狗预分频值更新，反之，未更新；
**                      如果第1未为1，表示重装载值更新，反之，未更新。
*********************************************************************************************************/
rt_uint8_t idwgStatusGet(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + IWDG_SR) & 0x03);
}
/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/

