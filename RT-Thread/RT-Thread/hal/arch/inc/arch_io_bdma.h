/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_bdma.h
** Last modified Date:  2016-01-19
** Last Version:        v1.0
** Description:         BDMA模块寄存器封装函数实现声明，适用于STM32L4xx
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2016-01-19
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
#ifndef __ARCH_IO_BDMA_H__
#define __ARCH_IO_BDMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** 需要的宏定义
*********************************************************************************************************/
//  DMA配置相关参数，为函数DMAModeSet的参数ulConfig
#define BDMA_DIR_M_READ      0x00000010      //  从存储器读，反之为外设读

#define BDMA_CIRC_EN         0x00000020      //  执行循环操作模式
#define BDMA_PINC_EN         0x00000040      //  使能外设地址增量模式
#define BDMA_MINC_EN         0x00000080      //  使能存储器地址增量模式

#define BDMA_PSIZE_8         0x00000000      //  外设数据宽度为8位
#define BDMA_PSIZE_16        0x00000100      //  外设数据宽度为16位
#define BDMA_PSIZE_32        0x00000200      //  外设数据宽度为32位
#define BDMA_PSIZE_RESERVED  0x00000300      //  外设数据宽度保留

#define BDMA_MSIZE_8         0x00000000      //  存储器数据宽度为8位
#define BDMA_MSIZE_16        0x00000400      //  存储器数据宽度为16位
#define BDMA_MSIZE_32        0x00000800      //  存储器数据宽度为32位
#define BDMA_MSIZE_RESERVED  0x00000C00      //  存储器数据宽度保留
  
#define BDMA_MODE_NORMAL         ((rt_uint32_t)0x00000000)   //  正常传输模式
#define BDMA_MODE_CIRCULAR       ((rt_uint32_t)0x00000020)   //  循环传输模式

#define BDMA_PL_LOW          0x00000000      //  通道优先级为低
#define BDMA_PL_MEDIUM       0x00001000      //  通道优先级为中
#define BDMA_PL_HIGH         0x00002000      //  通道优先级为高
#define BDMA_PL_HITHEST      0x00003000      //  通道优先级为最高

#define DMA_MTOM_EN         0x00004000      //  存储器到存储器传输模式使能
//  相关的中断信号定义，为中断相关函数的参数或返回值
#define BDMA_INT_TC          0x00000002      //  传输完成中断
#define BDMA_INT_HT          0x00000004      //  传输一半时的中断
#define BDMA_INT_TE          0x00000008      //  传输错误中断
#define BDMA_INT_GLOBAL      0x00000001      //  全局中断，只要上面三个有一个成立，则就成立
#define BDMA_INT_UAUAL       0x0000000A      //  传输一半以外的中断 

/*********************************************************************************************************
** 外部函数的声明
*********************************************************************************************************/
extern void BDMAModeSet(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulConfig);
extern void BDMAAddrSet(rt_uint32_t ulBase, rt_uint8_t ucChannel, 
                rt_uint32_t ulMemAddr, rt_uint32_t ulPeriAddr);
extern void BDMAMemAddr1Set(rt_uint32_t ulBase, rt_uint8_t ucChannel, rt_uint32_t ulMemAddr);
extern void BDMABufferSizeSet(rt_uint32_t ulBase, rt_uint8_t ucChannel, rt_uint16_t usSize);
extern rt_uint32_t BDMABufferSizeGet(rt_uint32_t ulBase, rt_uint8_t ucChannel);
extern void BDMAEnable(rt_uint32_t ulBase, rt_uint8_t ucChannel);
extern void BDMADisable(rt_uint32_t ulBase, rt_uint8_t ucChannel);
extern rt_bool_t BDMAIsEnabled(rt_uint32_t u32Base, rt_uint8_t ucChannel);
extern void BDMAIntEnable(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulFlags);
extern void BDMAIntDisable(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulFlags);
extern rt_uint32_t BDMAIntStatus(rt_uint32_t ulBase, rt_uint8_t ucChannel);
extern void BDMAIntClear(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulIntFlags);

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_BDMA_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
