/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lan8720.c
** Last modified Date:  2018-09-29
** Last Version:        v1.00
** Description:         LAN8720驱动
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2018-09-29
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "arch/inc/arch_io_memmap.h"
#include "arch/inc/arch_io_ethernet.h"

/*********************************************************************************************************
** 调试输出宏定义
*********************************************************************************************************/
#define LAN8720_DEBUG       0

#if (LAN8720_DEBUG > 0)
#define lan8720_trace(fmt, ...)           rt_kprintf(fmt, ##__VA_ARGS__)
#else
#define lan8720_trace(fmt, ...)
#endif

/*********************************************************************************************************
** PHY地址定义
*********************************************************************************************************/
#define PHYAddress                  0               // 使用的PHY的地址

/*********************************************************************************************************
** Function name:       lan8720_init
** Descriptions:        初始化LAN8720
** Input parameters:    NONE
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void lan8720_init(void)
{
  uint32_t u32Temp;
  uint32_t u32PhyRegValue;
  
  // 读取PHY LAN8720 的ID，如果错误，直接返回
  u32PhyRegValue = EthernetPHYRead(ETH0_BASE, PHYAddress, 2);
  if(u32PhyRegValue != 0x0007) {
    return;
  }
  
  EthernetPHYWrite(ETH0_BASE, PHYAddress, 0, 0x8000);           //  复位PHY
  while(EthernetPHYRead(ETH0_BASE, PHYAddress, 0) & 0x8000);   //  等待复位完成
  
  //u32PhyRegValue = EthernetPHYRead(ETH0_BASE, PHYAddress, 18);
  
  //  开启PHY的自协商功能
  u32Temp = EthernetPHYRead(ETH0_BASE, PHYAddress, 0);
  u32Temp |= 0x1000;
  EthernetPHYWrite(ETH0_BASE, PHYAddress, 0, u32Temp);
}
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
