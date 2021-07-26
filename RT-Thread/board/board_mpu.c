/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_mpu.c
** Last modified Date:  2019-04-08
** Last Version:        V1.00
** Description:         MPU配置
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2019-04-08
** Version:             V1.00
** Descriptions:
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
*********************************************************************************************************/
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <string.h>

#include "hal/cortexm/hal_cortexm_api.h"

/*********************************************************************************************************
** Function name:       rt_hw_mpu_init
** Descriptions:        MPU初始化
** Input parameters:    NONE
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static int rt_hw_mpu_init(void)
{
  hal_mpu_disable();
  
  // 第1个MPU保护区
  hal_mpu_region_config(0, 0x24000000, MPU_REGION_SIZE_512KB | 0x00UL |
                        MPU_ACCESS_NOT_SHAREABLE | MPU_ACCESS_CACHEABLE |
                          MPU_ACCESS_BUFFERABLE | MPU_REGION_FULL_ACCESS |
                            MPU_TEX_LEVEL0 | MPU_INSTRUCTION_ACCESS_ENABLE | MPU_REGION_ENABLE);
  
  // 第1个MPU保护区， 以太网DMA描述符用的该区域
  hal_mpu_region_config(1, 0x30040000, MPU_REGION_SIZE_512B | 0x00UL |
                        MPU_ACCESS_SHAREABLE | MPU_ACCESS_NOT_CACHEABLE |
                          MPU_ACCESS_BUFFERABLE | MPU_REGION_FULL_ACCESS |
                            MPU_TEX_LEVEL0 | MPU_INSTRUCTION_ACCESS_ENABLE | MPU_REGION_ENABLE);
  
  hal_mpu_enable(MPU_PRIVILEGED_DEFAULT);

  return 0;
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_BOARD_EXPORT(rt_hw_mpu_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/

