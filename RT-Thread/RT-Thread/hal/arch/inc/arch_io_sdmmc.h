/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_sdmmc.h
** Last modified Date:  2019-10-28
** Last Version:        v1.0
** Description:         SDMMC模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2019-10-28
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
#ifndef __ARCH_IO_SDMMC_H__
#define __ARCH_IO_SDMMC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
**  相关参数的宏定义
*********************************************************************************************************/
/*********************************************************************************************************
**  函数SdmmcInit的参数
*********************************************************************************************************/
// SDMMC 时钟的产生原则
#define SDMMC_CLOCK_EDGE_RISING      ((rt_uint32_t)0x00000000)
#define SDMMC_CLOCK_EDGE_FALLING     ((rt_uint32_t)0x00010000) 

// SDMMC是否处于省电模式
#define SDMMC_POWER_SAVE_DISABLE     ((rt_uint32_t)0x00000000)
#define SDMMC_POWER_SAVE_ENABLE      ((rt_uint32_t)0x00001000) 
  
// SDMMC时钟控制
#define SDMMC_HIGHSPEED_DISABLE     ((rt_uint32_t)0x00000000)
#define SDMMC_HIGHSPEED_ENABLE      ((rt_uint32_t)0x00080000)

// SDMMC总线宽度
#define SDMMC_BUS_WIDE_1B            ((rt_uint32_t)0x00000000)
#define SDMMC_BUS_WIDE_4B            ((rt_uint32_t)0x00004000)
#define SDMMC_BUS_WIDE_8B            ((rt_uint32_t)0x00008000)

// SDMMC 流控
#define SDMMC_HW_FLOW_CTRL_DISABLE    ((rt_uint32_t)0x00000000)
#define SDMMC_HW_FLOW_CTRL_ENABLE     ((rt_uint32_t)0x00020000)

// SDMMC分频系数, SDMMC_CK frequency = sdmmc_ker_ck / [2 * CLKDIV].
#define SDMMC_CLOCK_DIV_NUM(x)        (((rt_uint32_t)(x - 1)) * 2)

/*********************************************************************************************************
**  函数SdioCMDSend的参数
*********************************************************************************************************/
// 命令号
#define SDMMC_CMD_INDEX(x)           ((rt_uint32_t)x)

// 回应类型
#define SDMMC_RESPONSE_NO            ((rt_uint32_t)0x00000000)
#define SDMMC_RESPONSE_SHORT         ((rt_uint32_t)0x00000100)
#define SDMMC_RESPONSE_LONG          ((rt_uint32_t)0x00000300)

// SDMMC等待条件
#define SDMMC_WAIT_NO                ((rt_uint32_t)0x00000000)
#define SDMMC_WAIT_INT               ((rt_uint32_t)0x00000400)
#define SDMMC_WAIT_PEND              ((rt_uint32_t)0x00000800)

// SDMMC CPSM状态
#define SDMMC_CPSM_DISABLE           ((rt_uint32_t)0x00000000)
#define SDMMC_CPSM_ENABLE            ((rt_uint32_t)0x00001000)

/*********************************************************************************************************
**  函数SdioDataConfigInit的参数
*********************************************************************************************************/
// SDMMC数据块大小
#define SDMMC_DATABLOCK_SIZE_1B      ((rt_uint32_t)0x00000000)
#define SDMMC_DATABLOCK_SIZE_2B      ((rt_uint32_t)0x00000010)
#define SDMMC_DATABLOCK_SIZE_4B      ((rt_uint32_t)0x00000020)
#define SDMMC_DATABLOCK_SIZE_8B      ((rt_uint32_t)0x00000030)
#define SDMMC_DATABLOCK_SIZE_16B     ((rt_uint32_t)0x00000040)
#define SDMMC_DATABLOCK_SIZE_32B     ((rt_uint32_t)0x00000050)
#define SDMMC_DATABLOCK_SIZE_64B     ((rt_uint32_t)0x00000060)
#define SDMMC_DATABLOCK_SIZE_128B    ((rt_uint32_t)0x00000070)
#define SDMMC_DATABLOCK_SIZE_256B    ((rt_uint32_t)0x00000080)
#define SDMMC_DATABLOCK_SIZE_512B    ((rt_uint32_t)0x00000090)
#define SDMMC_DATABLOCK_SIZE_1024B   ((rt_uint32_t)0x000000A0)
#define SDMMC_DATABLOCK_SIZE_2048B   ((rt_uint32_t)0x000000B0)
#define SDMMC_DATABLOCK_SIZE_4096B   ((rt_uint32_t)0x000000C0)
#define SDMMC_DATABLOCK_SIZE_8192B   ((rt_uint32_t)0x000000D0)
#define SDMMC_DATABLOCK_SIZE_16384B  ((rt_uint32_t)0x000000E0)

// SDMMC数据传输方向
#define SDMMC_TRANSFERDIR_TO_CARD    ((rt_uint32_t)0x00000000)
#define SDMMC_TRANSFERDIR_TO_SDMMC    ((rt_uint32_t)0x00000002)

// SDMMC传输类型
#define SDMMC_TRANSFER_MODE_BLOCK               ((rt_uint32_t)0x00000000)
#define SDMMC_TRANSFER_MODE_SDIO_MULTIBYTE      ((rt_uint32_t)0x00000004)
#define SDMMC_TRANSFER_MODE_STREAM              ((rt_uint32_t)0x00000008)
#define SDMMC_TRANSFER_MODE_BLOCK_STOP          ((rt_uint32_t)0x0000000C)

// SDMMC_DPSM_State 
#define SDMMC_DPSM_DISABLE           ((rt_uint32_t)0x00000000)
#define SDMMC_DPSM_ENABLE            ((rt_uint32_t)0x00000001)

/*********************************************************************************************************
**  函数SdmmcReadWaitModeSet的参数
*********************************************************************************************************/
#define SDMMC_READ_WAIT_MODE_CLK     ((rt_uint32_t)0x00000400)
#define SDMMC_READ_WAIT_MODE_DATA2  ~((rt_uint32_t)0x00000400)

/*********************************************************************************************************
**  函数SdmmcIdmaConfig的参数
*********************************************************************************************************/
#define SDMMC_IDMA_DISABLE                  ((rt_uint32_t)0x00000000)
#define SDMMC_IDMA_ENABLE_SINGLE_BUFFER     ((rt_uint32_t)0x00000001)
#define SDMMC_IDMA_ENABLE_DOUBLE_BUFFER0    ((rt_uint32_t)0x00000003)
#define SDMMC_IDMA_ENABLE_DOUBLE_BUFFER1    ((rt_uint32_t)0x00000007)

/*********************************************************************************************************
**  SDMMC中断源
*********************************************************************************************************/
#define SDMMC_INT_CCRCFAIL           ((rt_uint32_t)0x00000001)
#define SDMMC_INT_DCRCFAIL           ((rt_uint32_t)0x00000002)
#define SDMMC_INT_CTIMEOUT           ((rt_uint32_t)0x00000004)
#define SDMMC_INT_DTIMEOUT           ((rt_uint32_t)0x00000008)
#define SDMMC_INT_TXUNDERR           ((rt_uint32_t)0x00000010)
#define SDMMC_INT_RXOVERR            ((rt_uint32_t)0x00000020)
#define SDMMC_INT_CMDREND            ((rt_uint32_t)0x00000040)
#define SDMMC_INT_CMDSENT            ((rt_uint32_t)0x00000080)
#define SDMMC_INT_DATAEND            ((rt_uint32_t)0x00000100)
#define SDMMC_INT_DHOLD              ((rt_uint32_t)0x00000200)
#define SDMMC_INT_DBCKEND            ((rt_uint32_t)0x00000400)
#define SDMMC_INT_DABORT             ((rt_uint32_t)0x00000800)
#define SDMMC_INT_CPSMACT            ((rt_uint32_t)0x00001000)
#define SDMMC_INT_DPSMACT            ((rt_uint32_t)0x00002000)
#define SDMMC_INT_TXFIFOHE           ((rt_uint32_t)0x00004000)
#define SDMMC_INT_RXFIFOHF           ((rt_uint32_t)0x00008000)
#define SDMMC_INT_TXFIFOF            ((rt_uint32_t)0x00010000)
#define SDMMC_INT_RXFIFOF            ((rt_uint32_t)0x00020000)
#define SDMMC_INT_TXFIFOE            ((rt_uint32_t)0x00040000)
#define SDMMC_INT_RXFIFOE            ((rt_uint32_t)0x00080000)
#define SDMMC_INT_BUSYD0END          ((rt_uint32_t)0x00200000)
#define SDMMC_INT_SDIOIT             ((rt_uint32_t)0x00400000)
#define SDMMC_INT_ACKFAIL            ((rt_uint32_t)0x00800000)
#define SDMMC_INT_ACKTIMEOUT         ((rt_uint32_t)0x01000000)
#define SDMMC_INT_VSWEND             ((rt_uint32_t)0x02000000)
#define SDMMC_INT_CKSTOP             ((rt_uint32_t)0x04000000)
#define SDMMC_INT_IDMABTC            ((rt_uint32_t)0x10000000)

/*********************************************************************************************************
**  SDMMC Flag的参数值，与寄存器SDMMC_STA对应
*********************************************************************************************************/
#define SDMMC_FLAG_CCRCFAIL          ((rt_uint32_t)0x00000001)
#define SDMMC_FLAG_DCRCFAIL          ((rt_uint32_t)0x00000002)
#define SDMMC_FLAG_CTIMEOUT          ((rt_uint32_t)0x00000004)
#define SDMMC_FLAG_DTIMEOUT          ((rt_uint32_t)0x00000008)
#define SDMMC_FLAG_TXUNDERR          ((rt_uint32_t)0x00000010)
#define SDMMC_FLAG_RXOVERR           ((rt_uint32_t)0x00000020)
#define SDMMC_FLAG_CMDREND           ((rt_uint32_t)0x00000040)
#define SDMMC_FLAG_CMDSENT           ((rt_uint32_t)0x00000080)
#define SDMMC_FLAG_DATAEND           ((rt_uint32_t)0x00000100)
#define SDMMC_FLAG_DHOLD             ((rt_uint32_t)0x00000200)
#define SDMMC_FLAG_DBCKEND           ((rt_uint32_t)0x00000400)
#define SDMMC_FLAG_DABORT            ((rt_uint32_t)0x00000800)
#define SDMMC_FLAG_CPSMACT           ((rt_uint32_t)0x00001000)
#define SDMMC_FLAG_DPSMACT           ((rt_uint32_t)0x00002000)
#define SDMMC_FLAG_TXFIFOHE          ((rt_uint32_t)0x00004000)
#define SDMMC_FLAG_RXFIFOHF          ((rt_uint32_t)0x00008000)
#define SDMMC_FLAG_TXFIFOF           ((rt_uint32_t)0x00010000)
#define SDMMC_FLAG_RXFIFOF           ((rt_uint32_t)0x00020000)
#define SDMMC_FLAG_TXFIFOE           ((rt_uint32_t)0x00040000)
#define SDMMC_FLAG_RXFIFOE           ((rt_uint32_t)0x00080000)
#define SDMMC_FLAG_BUSYD0            ((rt_uint32_t)0x00100000)
#define SDMMC_FLAG_BUSYD0END         ((rt_uint32_t)0x00200000)
#define SDMMC_FLAG_SDIOIT            ((rt_uint32_t)0x00400000)
#define SDMMC_FLAG_ACKFAIL           ((rt_uint32_t)0x00800000)
#define SDMMC_FLAG_ACKTIMEOUT        ((rt_uint32_t)0x01000000)
#define SDMMC_FLAG_VSWEND            ((rt_uint32_t)0x02000000)
#define SDMMC_FLAG_CKSTOP            ((rt_uint32_t)0x04000000)
#define SDMMC_FLAG_IDMATE            ((rt_uint32_t)0x08000000)
#define SDMMC_FLAG_IDMABTC           ((rt_uint32_t)0x10000000)

/*********************************************************************************************************
** 外部函数的声明
*********************************************************************************************************/
extern void SdmmcInit(rt_uint32_t u32Base, rt_uint32_t u32Config);
extern rt_uint32_t SdmmcDataRead(rt_uint32_t u32Base);
extern void SdmmcDataWrite(rt_uint32_t u32Base, rt_uint32_t u32Data);
extern void SdmmcPowerStateOn(rt_uint32_t u32Base);
extern void SdmmcPowerStateCycle(rt_uint32_t u32Base);
extern void SdmmcPowerStateOff(rt_uint32_t u32Base);
extern rt_uint32_t SdmmcPowerStateStatus(rt_uint32_t u32Base);
extern void SdmmcCommandSend(rt_uint32_t u32Base, rt_uint32_t u32SdmmcArg, rt_uint32_t u32SendConfig);
extern rt_uint8_t SdmmcCommandResponseGet(rt_uint32_t u32Base);
extern rt_uint32_t SdmmcResponseGet(rt_uint32_t u32Base, rt_uint32_t u32RESP);
extern void SdmmcDataConfig(rt_uint32_t u32Base, rt_uint32_t u32DataTimeOut, rt_uint32_t u32DataLen, rt_uint32_t u32DataCtrl);
extern rt_uint32_t SdmmcDataCounterGet(rt_uint32_t u32Base);
extern rt_uint32_t SdmmcFIFOCounterGet(rt_uint32_t u32Base);
extern void SdmmcReadWaitSdioStartCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet);
extern void SdmmcReadWaitSdioStopCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet);
extern void SdmmcReadWaitSdmmcModeSet(rt_uint32_t u32Base, rt_uint32_t u32Mode);
extern void SdmmcSdioModeCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet);
extern void SdmmcSdioSuspendCmdCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet);
extern void SdmmcCmdTransCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet);
extern void SdmmcCmdStopCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet);
extern void SdmmcIdmaConfig(rt_uint32_t u32Base, rt_uint32_t u32Config);
extern void SdmmcIdmaBuffer0Set(rt_uint32_t u32Base, rt_uint32_t u32Addr);
extern void SdmmcIdmaBuffer1Set(rt_uint32_t u32Base, rt_uint32_t u32Addr);
extern void SdmmcIntEnable(rt_uint32_t u32Base, rt_uint32_t u32IntFlag);
extern void SdmmcIntDisable(rt_uint32_t u32Base, rt_uint32_t u32IntFlag);
extern rt_uint32_t SdmmcFlagStatus(rt_uint32_t u32Base);
extern void SdmmcFlagClear(rt_uint32_t u32Base, rt_uint32_t u32Flag);
extern rt_uint32_t SdmmcIntStatus(rt_uint32_t u32Base);
extern void SdmmcIntClear(rt_uint32_t u32Base, rt_uint32_t u32IntFlag);

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_SDMMC_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
