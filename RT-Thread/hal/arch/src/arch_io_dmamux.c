/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_dmamux.c
** Last modified Date:  2019-03-24
** Last Version:        v1.0
** Description:         DMA MUX模块函数，用于实现DMA通道的输入选择
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2019-03-24
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
#include "hal/arch/inc/arch_io_dmamux.h"

/*********************************************************************************************************
** Function name:       DMAMuxRequestIDSet
** Descriptions:        DMA请求ID设置，用于设置DMA的请求源，比如DMA1 Stream0为USART1的TX功能
** input parameters:    ulBase:         DMA模块的基地址
**                      u32Channel:     DMA通道编号，DMAMUX1为0-15，DMAMUX2为0-7
**                      u32RequestID:   DMA的请求ID，具体请参考手册
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAMuxRequestIDSet(rt_uint32_t u32Base, rt_uint32_t u32Channel,rt_uint32_t u32RequestID)
{ 
  HWREG32(u32Base + DMAMUX_C0CR + 4 * u32Channel) = u32RequestID;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
