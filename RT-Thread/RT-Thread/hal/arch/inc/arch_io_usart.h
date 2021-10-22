/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_usart.h
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         usart及uart模块寄存器封装函数实现声明
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
#ifndef __ARCH_IO_USART_H__
#define __ARCH_IO_USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** 需要的宏定义
*********************************************************************************************************/
//  串口配置函数UsartConfigSet的参数定义,该宏左移12位后可以判断写入寄存器
#define USART_STOP_0_5      0x00000001
#define USART_STOP_1        0x00000000
#define UASRT_STOP_1_5      0x00000003
#define USART_STOP_2        0x00000002

#define USART_WLEN_7        0x10000000
#define USART_WLEN_8        0x00000000
#define USART_WLEN_9        0x00001000

#define USART_PAR_NONE      0x00000000
#define USART_PAR_EVEN      0x00000400
#define USART_PAR_ODD       0x00000600

#define USART_CTSE_ENABLE   0x00000020
#define USART_CTSE_DISABLE  0x00000000

#define USART_RTSE_ENABLE   0x00000010
#define USART_RTSE_DISABLE  0x00000000

#define USART_DMA_TX        0x00000080          //  DMA发送功能
#define USART_DMA_RX        0x00000040          //  DMA接受功能
//  常用中断源，为函数UsartIntEnable()的参数
#define USART_INT_EIE       0x00000200          //  帧错误中断
#define USART_INT_PE        0x00000100          //  校验错误
#define USART_INT_TX_EMPTY  0x00000080          //  发送缓冲区空中断
#define USART_INT_TX_FINISH 0x00000040          //  发送完成中断
#define USART_INT_RX        0x00000020          //  接收中断使能，说明接收寄存器中有数据了
#define USART_INT_IDLE      0x00000010          //  IDLE中断使能

/*********************************************************************************************************
** 外部函数的声明
*********************************************************************************************************/
extern void UsartConfigSet(rt_uint32_t u32Base, rt_uint32_t u32Clock,
                            rt_uint32_t u32Baud, rt_uint32_t u32Config);
extern void UsartEnable(rt_uint32_t u32Base);
extern void UsartDisable(rt_uint32_t u32Base);
extern void UsartDMAEnable(rt_uint32_t u32Base,rt_uint32_t u32Flags);
extern void UsartDMADisable(rt_uint32_t u32Base, rt_uint32_t u32Flags);
extern void UsartIntEnable(rt_uint32_t u32Base,rt_uint32_t u32IntFlags);
extern void UsartIntDisable(rt_uint32_t u32Base,rt_uint32_t u32IntFlags);
extern rt_bool_t UsartSpaceAvial(rt_uint32_t u32Base);
extern void UsartCharPut(rt_uint32_t u32Base,rt_uint8_t u8Data);
extern rt_bool_t UsartCharsAvial(rt_uint32_t u32Base);
extern rt_uint32_t  UsartCharGet(rt_uint32_t u32Base);
extern void UsartPutS(rt_uint32_t u32Base,const char *s);
extern void UsartPutN(rt_uint32_t u32Base, const  char  *pStr , rt_uint16_t  u16Num);



#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_USART_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
