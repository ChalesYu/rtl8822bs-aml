/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_io_ctrl.h
** Last modified Date:  2018-06-16
** Last Version:        v1.00
** Description:         IO控制驱动
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2018-06-16
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#ifndef __BOARD_IO_CTRL_H__
#define __BOARD_IO_CTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>

/*********************************************************************************************************
**  实现的外部函数声明
*********************************************************************************************************/
extern void rt_hw_io_ctrl_init(void);
extern void rt_hw_io_ctrl_set(rt_uint8_t u8IO);
extern void rt_hw_io_ctrl_reset(rt_uint8_t u8IO);

#define rt_hw_wifi_pwr_on()             rt_hw_io_ctrl_reset(0)
#define rt_hw_wifi_pwr_off()           rt_hw_io_ctrl_set(0)  


#ifdef __cplusplus
    }
#endif      // __cplusplus
    
#endif      // __BOARD_IO_CTRL_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
