/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_qspi.h
** Last modified Date:  2019-03-22
** Last Version:        v1.0
** Description:         quad spi模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2019-03-22
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
#ifndef __ARCH_IO_QSPI_H__
#define __ARCH_IO_QSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** 需要的宏定义
*********************************************************************************************************/
// 函数QSPIInit参数u32Config宏定义,可以取下列几组参数的或
// QSPI波特率分频器选择
#define QSPI_CLOCK_PRESCALER(x)         (((rt_uint32_t)(x - 1)) << 24)
// Polling match mode设置
#define QSPI_AND_MATCH_MODE             (((rt_uint32_t)0) << 23)
#define QSPI_OR_MATCH_MODE              (((rt_uint32_t)1) << 23)
// Pool mode stop停止条件设置
#define QSPI_POLL_STOP_BY_DISABLE_QSPI  (((rt_uint32_t)0) << 22)
#define QSPI_POLL_STOP_BY_MATCH         (((rt_uint32_t)1) << 22)
// FIFO threshold level设置
#define QSPI_FIFO_THRESHOLD(x)          (((rt_uint32_t)(x - 1)) << 8)
// Flash Memory选择，当使用双通道Flash时用到
#define QSPI_SELECTED_FLASH1            (((rt_uint32_t)0) << 7)
#define QSPI_SELECTED_FLASH2            (((rt_uint32_t)1) << 7)
//  双通道Flash是否使能配置
#define QSPI_DUAL_FLASH_DISABLE         (((rt_uint32_t)0) << 6)
#define QSPI_DUAL_FLASH_ENABLE          (((rt_uint32_t)1) << 6)
// 采样移位配置， DDR模式必须配置为NO SHIFT
#define QSPI_SAMPLE_NO_SHIFT            (((rt_uint32_t)0) << 4)
#define QSPI_SAMPLE_HALF_SHIFT          (((rt_uint32_t)1) << 4) 
// 超时计数器使能配置
#define QSPI_TIMEOUT_DISABLE            (((rt_uint32_t)0) << 3)
#define QSPI_TIMEOUT_ENABLE             (((rt_uint32_t)1) << 3)

// 函数QSPIDeviceConfig参数u32Config宏定义,可以取下列几组参数的或 
// Flash存储器容量配置
#define QSPI_FLASH_MEMORY_SIZE(x)       (((rt_uint32_t)(x - 1)) << 16)
// 片选信号高电平实际配置
#define QSPI_CS_HIGH_TIME(x)            (((rt_uint32_t)(x - 1)) << 8)
// QSPI工作模式配置
#define QSPI_CKMODE_0                   (((rt_uint32_t)0) << 0)
#define QSPI_CKMODE_3                   (((rt_uint32_t)1) << 0)

// 函数QSPICommunicationConfig参数u32Config宏定义,可以取下列几组参数的或 
// DDR使能控制
#define QSPI_DDR_MODE_DISABLE            (((rt_uint32_t)0) << 31)
#define QSPI_DDR_MODE_ENABLE             (((rt_uint32_t)1) << 31)
// DDR Hold控制
#define QSPI_DDR_HOLD_BY_ANALOG_DELAY   (((rt_uint32_t)0) << 30)
#define QSPI_DDR_HOLD_BY_CLOCK          (((rt_uint32_t)1) << 30)
// Runing Mode配置
#define QSPI_NORMAL_CLOCK_MODE          (((rt_uint32_t)0) << 29)
#define QSPI_FREE_CLOCK_MODE            (((rt_uint32_t)1) << 29)
// 发送指令控制
#define QSPI_SEND_INSTRUCTION_EVERY     (((rt_uint32_t)0) << 28)
#define QSPI_SEND_INSTRUCTION_FIRST     (((rt_uint32_t)1) << 28)
//  Functional mode配置
#define QSPI_INDIRECT_WRITE_MODE        (((rt_uint32_t)0) << 26)
#define QSPI_INDIRECT_READ_MODE         (((rt_uint32_t)1) << 26)
#define QSPI_AUTO_POLLING_MODE          (((rt_uint32_t)2) << 26)
#define QSPI_MEMMORY_MAP_MODE           (((rt_uint32_t)3) << 26)
// 数据格式控制
#define QSPI_DATA_MODE_NO               (((rt_uint32_t)0) << 24)
#define QSPI_DATA_MODE_1LINE            (((rt_uint32_t)1) << 24)
#define QSPI_DATA_MODE_2LINE            (((rt_uint32_t)2) << 24)
#define QSPI_DATA_MODE_4LINE            (((rt_uint32_t)3) << 24)
// Dummy周期配置
#define QSPI_DUMMY_CYCLES(x)            (((rt_uint32_t)(x)) << 18)
//  Alternate bytes size设置
#define QSPI_ALTERNATE_BYTES_8BIT       (((rt_uint32_t)0) << 16)
#define QSPI_ALTERNATE_BYTES_16BIT      (((rt_uint32_t)1) << 16)
#define QSPI_ALTERNATE_BYTES_24BIT      (((rt_uint32_t)2) << 16)
#define QSPI_ALTERNATE_BYTES_32BIT      (((rt_uint32_t)3) << 16)
// Alternate Mode设置
#define QSPI_ALTERNATE_MODE_NO          (((rt_uint32_t)0) << 14)
#define QSPI_ALTERNATE_MODE_1LINE       (((rt_uint32_t)1) << 14)
#define QSPI_ALTERNATE_MODE_2LINE       (((rt_uint32_t)2) << 14)
#define QSPI_ALTERNATE_MODE_4LINE       (((rt_uint32_t)3) << 14)
// Address bytes size设置
#define QSPI_ADDRESS_BYTES_8BIT         (((rt_uint32_t)0) << 12)
#define QSPI_ADDRESS_BYTES_16BIT        (((rt_uint32_t)1) << 12)
#define QSPI_ADDRESS_BYTES_24BIT        (((rt_uint32_t)2) << 12)
#define QSPI_ADDRESS_BYTES_32BIT        (((rt_uint32_t)3) << 12)
// Address Mode设置
#define QSPI_ADDRESS_MODE_NO            (((rt_uint32_t)0) << 10)
#define QSPI_ADDRESS_MODE_1LINE         (((rt_uint32_t)1) << 10)
#define QSPI_ADDRESS_MODE_2LINE         (((rt_uint32_t)2) << 10)
#define QSPI_ADDRESS_MODE_4LINE         (((rt_uint32_t)3) << 10)
// Instruction Mode设置
#define QSPI_INSTRUCTION_MODE_NO        (((rt_uint32_t)0) << 8)
#define QSPI_INSTRUCTION_MODE_1LINE     (((rt_uint32_t)1) << 8)
#define QSPI_INSTRUCTION_MODE_2LINE     (((rt_uint32_t)2) << 8)
#define QSPI_INSTRUCTION_MODE_4LINE     (((rt_uint32_t)3) << 8)
// 发送的Instruction
#define QSPI_INSTRUCTION(x)             (((rt_uint32_t)(x)) << 0)

/*********************************************************************************************************
** 外部函数的声明
*********************************************************************************************************/
extern void QSPIInit(rt_uint32_t u32Base,rt_uint32_t u32Config);
extern void QSPIDeviceConfig(rt_uint32_t u32Base,rt_uint32_t u32Config);
extern void QSPICommunicationConfig(rt_uint32_t u32Base,rt_uint32_t u32Config);
extern void QSPIEnable(rt_uint32_t u32Base);
extern void QSPIDisable(rt_uint32_t u32Base);
extern void QSPIAbortRequest(rt_uint32_t u32Base);
extern void QSPITransfer(rt_uint32_t u32Base, rt_uint32_t u32Config, 
                  rt_uint32_t u32Address, rt_uint32_t size, void * data);

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_QSPI_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
