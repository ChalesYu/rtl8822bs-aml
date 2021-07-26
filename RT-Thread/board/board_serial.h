/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_serial.h
** Last modified Date:  2014-12-13
** Last Version:        v1.00
** Description:         串口驱动总的初始化文件
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2014-12-13
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#ifndef __BOARD_SERIAL_H__
#define __BOARD_SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>

/*********************************************************************************************************
**  串口驱动配置
*********************************************************************************************************/
/* USART driver select. */
//#define RT_USING_ALL_DMA_UART1
#define RT_USING_FRAME_UART1

//#define RT_USING_ALL_DMA_UART2
//#define RT_USING_FRAME_UART2

//#define RT_USING_ALL_DMA_UART3
//#define RT_USING_FRAME_UART3

//#define RT_USING_ALL_DMA_UART4
//#define RT_USING_FRAME_UART4

//#define RT_USING_ALL_DMA_UART5
//#define RT_USING_FRAME_UART5

//#define RT_USING_ALL_DMA_UART6
//#define RT_USING_FRAME_UART6

//#define RT_USING_ALL_DMA_UART7
//#define RT_USING_FRAME_UART7



#ifdef __cplusplus
    }
#endif      // __cplusplus
    
#endif      // __BOARD_SERIAL_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
