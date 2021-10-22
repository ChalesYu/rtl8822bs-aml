/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_sdmmc.c
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
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_sdmmc.h"

/*========================================================================================================
                        Initialization/de-initialization functions
========================================================================================================*/

/*********************************************************************************************************
** Function name:       SdmmcInit
** Descriptions:        SDMMC模块初始化配置
** input parameters:    u32Base:  SDMMC基地址
**                      u32Config: 配置参数，可以取下列几组参数的或：
**                      SDMMC时钟的产生原则
**                      SDMMC_CLOCK_EDGE_RISING          // SDMMC CLK上升沿产生
**                      SDMMC_CLOCK_EDGE_FALLING         // SDMMC CLK下降沿产生
**
**                      SDMMC处于省电模式
**                      SDMMC_POWER_SAVE_DISABLE         // 省电模式禁止
**                      SDMMC_POWER_SAVE_ENABLE          // 省电模式使能
**
**                      SDMMC总线宽度
**                      SDMMC_BUS_WIDE_1B                // 1位数据宽度
**                      SDMMC_BUS_WIDE_4B                // 4位数据宽度
**                      SDMMC_BUS_WIDE_8B                // 8位数据宽度
**
**                      SDMMC流控
**                      SDMMC_HW_FLOW_CTRL_DISABLE       // 流控禁止
**                      SDMMC_HW_FLOW_CTRL_ENABLE        // 流控使能
**
**                      SDMMC分频系数
**                      SDMMC_CLOCK_DIV_NUM(x)           // 分频系数
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcInit(rt_uint32_t u32Base, rt_uint32_t u32Config)
{
    rt_uint32_t u32Temp;
    
    u32Temp = HWREG32(u32Base + SDMMC_CLKCR);
    u32Temp &= ((rt_uint32_t)0xFFFC0000);
    u32Temp |= u32Config;
    HWREG32(u32Base + SDMMC_CLKCR) = u32Temp;
}

/*========================================================================================================
                                  I/O operation functions
========================================================================================================*/

/*********************************************************************************************************
** Function name:       SdmmcDataRead
** Descriptions:        SDMMC数据读取
** input parameters:    u32Base:  SDMMC基地址
** output parameters:   NONE
** Returned value:      读取到的数据
*********************************************************************************************************/
rt_uint32_t SdmmcDataRead(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + SDMMC_FIFO));
}

/*********************************************************************************************************
** Function name:       SdmmcDataWrite
** Descriptions:        SDMMC数据写
** input parameters:    u32Base:  SDMMC基地址
**                      u32Data:  写的数据
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcDataWrite(rt_uint32_t u32Base, rt_uint32_t u32Data)
{
    HWREG32(u32Base + SDMMC_FIFO) = u32Data;
}

/*========================================================================================================
                             Peripheral Control functions
========================================================================================================*/

/*********************************************************************************************************
** Function name:       SdmmcPowerStateOn
** Descriptions:        SDMMC电源控制。打开
** input parameters:    u32Base:  SDMMC基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcPowerStateOn(rt_uint32_t u32Base)
{
    HWREG32(u32Base + SDMMC_POWER) |= ((rt_uint32_t)0x000000003);
    rt_thread_delay(2);
}

/*********************************************************************************************************
** Function name:       SdmmcPowerStateCycle
** Descriptions:        SDMMC电源控制。Power Cycle
** input parameters:    u32Base:  SDMMC基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcPowerStateCycle(rt_uint32_t u32Base)
{
    HWREG32(u32Base + SDMMC_POWER) |= ((rt_uint32_t)0x000000002);
}

/*********************************************************************************************************
** Function name:       SdmmcPowerStateOff
** Descriptions:        SDMMC电源控制。关闭
** input parameters:    u32Base:  SDMMC基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcPowerStateOff(rt_uint32_t u32Base)
{
    HWREG32(u32Base + SDMMC_POWER) &= ~((rt_uint32_t)0x000000003);
}

/*********************************************************************************************************
** Function name:       SdmmcPowerStateStatus
** Descriptions:        SDMMC电源状态
** input parameters:    u32Base:  SDMMC基地址
** output parameters:   NONE
** Returned value:      电源状态。可以取以下值：
**                      POWER_OFF 
**                      POWER_UP
**                      POWER_ON
*********************************************************************************************************/
rt_uint32_t SdmmcPowerStateStatus(rt_uint32_t u32Base)
{
  return (HWREG32(u32Base + SDMMC_POWER) & ((rt_uint32_t)0x000000003));
}
/*========================================================================================================
                    Command path state machine (CPSM) management functions
========================================================================================================*/
/*********************************************************************************************************
** Function name:       SdmmcCommandSend
** Descriptions:        SDMMC发送命令
** input parameters:    u32Base:            SDMMC基地址
**                      u32SdmmcArg:         命令参数
**                      uint32SendConfig:   发送的配置参数, 可以取下列几组参数的或
**                      命令号
**                      SDMMC_CMD_INDEX(x)           // 命令号

**                      回应类型
**                      SDMMC_RESPONSE_NO            // 无回应
**                      SDMMC_RESPONSE_SHORT         // 短回应
**                      SDMMC_RESPONSE_LONG          // 长回应

**                      SDMMC等待条件
**                      SDMMC_WAIT_NO                // 无等待
**                      SDMMC_WAIT_INT               // 等待中断
**                      SDMMC_WAIT_PEND              // 等待传输结束

**                      SDMMC CPSM状态
**                      SDMMC_CPSM_DISABLE           // CPSM禁止
**                      SDMMC_CPSM_ENABLE            // CPSM使能
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcCommandSend(rt_uint32_t u32Base, rt_uint32_t u32SdmmcArg, rt_uint32_t u32SendConfig)
{
    rt_uint32_t u32Temp;
    
    HWREG32(u32Base + SDMMC_ARG) = u32SdmmcArg;
    
    u32Temp = HWREG32(u32Base + SDMMC_CMD);
    u32Temp &= 0xFFFEE0C0;
    u32Temp |= u32SendConfig;
    HWREG32(u32Base + SDMMC_CMD) = u32Temp;
}

/*********************************************************************************************************
** Function name:       SdmmcCommandResponseGet
** Descriptions:        获取上一次的命令回应
** input parameters:    u32Base:  SDMMC基地址
** output parameters:   NONE
** Returned value:      Returns command index of last command for which response received.
*********************************************************************************************************/
rt_uint8_t SdmmcCommandResponseGet(rt_uint32_t u32Base)
{
    return ((rt_uint8_t)HWREG32(u32Base + SDMMC_RESPCMD));
}

/*********************************************************************************************************
** Function name:       SdmmcResponseGet
** Descriptions:        获取上一次的命令回应
** input parameters:    u32Base:  SDMMC基地址
**                      u32RESP:  获取的RESP寄存器,可以取下列值：
**                      SDMMC_RESP1          // RESP1寄存器
**                      SDMMC_RESP2          // RESP2寄存器
**                      SDMMC_RESP3          // RESP3寄存器
**                      SDMMC_RESP4          // RESP4寄存器
** output parameters:   NONE
** Returned value:      返回回应值
*********************************************************************************************************/
rt_uint32_t SdmmcResponseGet(rt_uint32_t u32Base, rt_uint32_t u32RESP)
{
    return (HWREG32(u32Base + SDMMC_RESP_BASE + u32RESP));
}

/*========================================================================================================
                    Data path state machine (DPSM) management functions
========================================================================================================*/

/*********************************************************************************************************
** Function name:       SdmmcDataConfig
** Descriptions:        SDMMC Data Path配置
** input parameters:    u32Base:        SDMMC基地址
**                      u32DataTimeOut: SDMMC Data TimeOut value
**                      u32DataLen:     SDMMC DataLength value
**                      u32DataCtrl:    SDMMC DCTRL value, 可以取下列几组参数的或：
**                      SDMMC数据块大小
**                      SDMMC_DATABLOCK_SIZE_1B          // 1个字节
**                      SDMMC_DATABLOCK_SIZE_2B          // 2个字节
**                      SDMMC_DATABLOCK_SIZE_4B          // 4个字节
**                      SDMMC_DATABLOCK_SIZE_8B          // 8个字节
**                      SDMMC_DATABLOCK_SIZE_16B         // 16个字节
**                      SDMMC_DATABLOCK_SIZE_32B         // 32个字节
**                      SDMMC_DATABLOCK_SIZE_64B         // 64个字节
**                      SDMMC_DATABLOCK_SIZE_128B        // 128个字节
**                      SDMMC_DATABLOCK_SIZE_256B        // 256个字节
**                      SDMMC_DATABLOCK_SIZE_512B        // 512个字节
**                      SDMMC_DATABLOCK_SIZE_1024B       // 1024个字节
**                      SDMMC_DATABLOCK_SIZE_2048B       // 2048个字节
**                      SDMMC_DATABLOCK_SIZE_4096B       // 4096个字节
**                      SDMMC_DATABLOCK_SIZE_8192B       // 8192个字节
**                      SDMMC_DATABLOCK_SIZE_16384B      // 16384个字节
**
**                      SDMMC数据传输方向
**                      SDMMC_TRANSFERDIR_TO_CARD        // 数据传向CARD
**                      SDMMC_TRANSFERDIR_TO_SDMMC        // 数据传向SDMMC
**
**                      SDMMC传输类型
**                      SDMMC_TRANSFER_MODE_BLOCK            // Block模式
**                      SDMMC_TRANSFER_MODE_SDIO_MULTIBYTE  // SDIO多字节传输
**                      SDMMC_TRANSFER_MODE_STREAM          // Stream模式
**
**                      SDMMC_DPSM_State 
**                      SDMMC_DPSM_DISABLE               // DPSM禁止
**                      SDMMC_DPSM_ENABLE                // DPSM使能
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcDataConfig(rt_uint32_t u32Base, rt_uint32_t u32DataTimeOut, rt_uint32_t u32DataLen, rt_uint32_t u32DataCtrl)
{
    rt_uint32_t u32Temp;
    
    HWREG32(u32Base + SDMMC_DTIMER) = u32DataTimeOut;
    HWREG32(u32Base + SDMMC_DLEN) = u32DataLen;
    
    u32Temp = HWREG32(u32Base + SDMMC_DCTRL);
    u32Temp &= ((rt_uint32_t)0xFFFFFF00);
    u32Temp |= u32DataCtrl;
    HWREG32(u32Base + SDMMC_DCTRL) = u32Temp;
}

/*********************************************************************************************************
** Function name:       SdmmcDataCounterGet
** Descriptions:        获取传输的剩余数据数量
** input parameters:    u32Base:  SDMMC基地址
** output parameters:   NONE
** Returned value:      剩余传输的数据数量
*********************************************************************************************************/
rt_uint32_t SdmmcDataCounterGet(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + SDMMC_DCOUNT));
}

/*********************************************************************************************************
** Function name:       SdmmcFIFOCounterGet
** Descriptions:        获取传输的FIFO剩余数量
** input parameters:    u32Base:  SDMMC基地址
** output parameters:   NONE
** Returned value:      Returns the number of words left to be written to or read from FIFO
*********************************************************************************************************/
rt_uint32_t SdmmcFIFOCounterGet(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + SDMMC_FIFO));
}

/*========================================================================================================
                    SDMMC IO Cards mode management functions
========================================================================================================*/

/*********************************************************************************************************
** Function name:       SdmmcReadWaitSdioStartCtrl
** Descriptions:        Enable or Disable Starts the SD I/O Read Wait operation
** input parameters:    u32Base:  SDMMC基地址
**                      bIsSet:   控制状态，可以取RT_TRUE或者RT_FALSE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcReadWaitSdioStartCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet)
{
    if(RT_TRUE == bIsSet) {
        HWREG32(u32Base + SDMMC_DCTRL) |= ((rt_uint32_t)0x00000100);
    } 
    else {
        HWREG32(u32Base + SDMMC_DCTRL) &= ~((rt_uint32_t)0x00000100);
    }
}

/*********************************************************************************************************
** Function name:       SdmmcReadWaitSdioStopCtrl
** Descriptions:        Enable or Disable Stops the SD I/O Read Wait operation.
** input parameters:    u32Base:  SDMMC基地址
**                      bIsSet:   控制状态，可以取RT_TRUE或者RT_FALSE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcReadWaitSdioStopCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet)
{
    if(RT_TRUE == bIsSet) {
        HWREG32(u32Base + SDMMC_DCTRL) |= ((rt_uint32_t)0x00000200);
    } 
    else {
        HWREG32(u32Base + SDMMC_DCTRL) &= ~((rt_uint32_t)0x00000200);
    }
}

/*********************************************************************************************************
** Function name:       SdmmcReadWaitSdmmcModeSet
** Descriptions:        Sets one of the two options of inserting read wait interval.
** input parameters:    u32Base:  SDMMC基地址
**                      u32Mode:  等待模式，可以取下列值：
**                      SDMMC_READ_WAIT_MODE_CLK         // Read Wait control using SDMMC_CK
**                      SDMMC_READ_WAIT_MODE_DATA2       // Read Wait control stopping SDMMC_D2
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcReadWaitSdmmcModeSet(rt_uint32_t u32Base, rt_uint32_t u32Mode)
{
    HWREG32(u32Base + SDMMC_DCTRL) &= ~((rt_uint32_t)0x00000400);
    HWREG32(u32Base + SDMMC_DCTRL) |= u32Mode;
}

/*********************************************************************************************************
** Function name:       SdmmcSdioModeCtrl
** Descriptions:        Enables or disables the SDIO Mode Operation.
** input parameters:    u32Base:  SDMMC基地址
**                      bIsSet:   控制状态，可以取RT_TRUE或者RT_FALSE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcSdioModeCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet)
{
    if(RT_TRUE == bIsSet) {
        HWREG32(u32Base + SDMMC_DCTRL) |= ((rt_uint32_t)0x00000800);
    } 
    else {
        HWREG32(u32Base + SDMMC_DCTRL) &= ~((rt_uint32_t)0x00000800);
    }
}

/*********************************************************************************************************
** Function name:       SdmmcSdioSuspendCmdCtrl
** Descriptions:        Enable or Disable the SD I/O Suspend command sending.
** input parameters:    u32Base:  SDMMC基地址
**                      bIsSet:   控制状态，可以取RT_TRUE或者RT_FALSE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcSdioSuspendCmdCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet)
{
    if(RT_TRUE == bIsSet) {
        HWREG32(u32Base + SDMMC_CMD) |= ((rt_uint32_t)0x00010000);
    } 
    else {
        HWREG32(u32Base + SDMMC_CMD) &= ~((rt_uint32_t)0x00010000);
    }
}

/*********************************************************************************************************
** Function name:       SdmmcCmdTransCtrl
** Descriptions:        Enable or Disable the CMDTRANS mode.
** input parameters:    u32Base:  SDMMC基地址
**                      bIsSet:   控制状态，可以取RT_TRUE或者RT_FALSE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcCmdTransCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet)
{
    if(RT_TRUE == bIsSet) {
        HWREG32(u32Base + SDMMC_CMD) |= ((rt_uint32_t)0x00000004);
    } 
    else {
        HWREG32(u32Base + SDMMC_CMD) &= ~((rt_uint32_t)0x00000004);
    }
}

/*********************************************************************************************************
** Function name:       SdmmcCmdStopCtrl
** Descriptions:        Enable or Disable the CMDSTOP mode.
** input parameters:    u32Base:  SDMMC基地址
**                      bIsSet:   控制状态，可以取RT_TRUE或者RT_FALSE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcCmdStopCtrl(rt_uint32_t u32Base, rt_bool_t bIsSet)
{
    if(RT_TRUE == bIsSet) {
        HWREG32(u32Base + SDMMC_CMD) |= ((rt_uint32_t)0x00000008);
    } 
    else {
        HWREG32(u32Base + SDMMC_CMD) &= ~((rt_uint32_t)0x00000008);
    }
}

/*========================================================================================================
                    DMA transfers management functions
========================================================================================================*/
/*********************************************************************************************************
** Function name:       SdmmcIdmaConfig
** Descriptions:        IDMA配置
** input parameters:    u32Base:  SDMMC基地址
**                      u32Config:  取下列参数的或
**                       工作模式：
**                       SDMMC_IDMA_DISABLE                     // 禁止IDMA
**                       SDMMC_IDMA_ENABLE_SINGLE_BUFFER        // 单缓冲模式
**                       SDMMC_IDMA_ENABLE_DOUBLE_BUFFER0       // 双缓冲模式使用BUFFER0
**                       SDMMC_IDMA_ENABLE_DOUBLE_BUFFER1       // 双缓冲模式使用BUFFER1    
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcIdmaConfig(rt_uint32_t u32Base, rt_uint32_t u32Config)
{
  HWREG32(u32Base + SDMMC_IDMACTRL) &= ~((rt_uint32_t)0x00000007);
  HWREG32(u32Base + SDMMC_IDMACTRL) |= u32Config;
}

/*********************************************************************************************************
** Function name:       SdmmcIdmaBuffer0Set
** Descriptions:        IDMA模式缓冲区0配置
** input parameters:    u32Base:  SDMMC基地址
**                      u32Addr:  缓冲区地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcIdmaBuffer0Set(rt_uint32_t u32Base, rt_uint32_t u32Addr)
{
  HWREG32(u32Base + SDMMC_IDMABASE0) = u32Addr;
}

/*********************************************************************************************************
** Function name:       SdmmcIdmaBuffer1Set
** Descriptions:        IDMA模式缓冲区1配置
** input parameters:    u32Base:  SDMMC基地址
**                      u32Addr:  缓冲区地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcIdmaBuffer1Set(rt_uint32_t u32Base, rt_uint32_t u32Addr)
{
  HWREG32(u32Base + SDMMC_IDMABASE1) = u32Addr;
}

/*========================================================================================================
                    Interrupts and flags management functions
========================================================================================================*/
/*********************************************************************************************************
** Function name:       SdmmcIntEnable
** Descriptions:        Enable the SDMMC interrupts.
** input parameters:    u32Base:    SDMMC基地址
**                      u32IntFlag: 控制的中断源, 可以取下列值中的一个或几个的或
**                      SDMMC_INT_CCRCFAIL: Command response received (CRC check failed) interrupt
**                      SDMMC_INT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
**                      SDMMC_INT_CTIMEOUT: Command response timeout interrupt
**                      SDMMC_INT_DTIMEOUT: Data timeout interrupt
**                      SDMMC_INT_TXUNDERR: Transmit FIFO underrun error interrupt
**                      SDMMC_INT_RXOVERR:  Received FIFO overrun error interrupt
**                      SDMMC_INT_CMDREND:  Command response received (CRC check passed) interrupt
**                      SDMMC_INT_CMDSENT:  Command sent (no response required) interrupt
**                      SDMMC_INT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
**                      SDMMC_INT_DHOLD:    Data transfer Hold interrupt
**                      SDMMC_INT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
**                      SDMMC_INT_DABORT:   Data transfer aborted by CMD12 interrupt
**                      SDMMC_INT_TXFIFOHE: Transmit FIFO Half Empty interrupt
**                      SDMMC_INT_RXFIFOHF: Receive FIFO Half Full interrupt
**                      SDMMC_INT_RXFIFOF:  Receive FIFO full interrupt
**                      SDMMC_INT_TXFIFOE:  Transmit FIFO empty interrupt
**                      SDMMC_INT_BUSYD0END: End of SDMMC_D0 Busy following a CMD response detected interrupt
**                      SDMMC_INT_SDIOIT:    SDIO interrupt received interrupt
**                      SDMMC_INT_ACKFAIL:   Boot Acknowledgment received interrupt
**                      SDMMC_INT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
**                      SDMMC_INT_VSWEND:   Voltage switch critical timing section completion interrupt
**                      SDMMC_INT_CKSTOP:   SDMMC_CK stopped in Voltage switch procedure interrupt
**                      SDMMC_INT_IDMABTC:  IDMA buffer transfer complete interrupt
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcIntEnable(rt_uint32_t u32Base, rt_uint32_t u32IntFlag)
{
    HWREG32(u32Base + SDMMC_MASK) |= u32IntFlag;
}

/*********************************************************************************************************
** Function name:       SdmmcIntDisable
** Descriptions:        Disable the SDMMC interrupts.
** input parameters:    u32Base:    SDMMC基地址
**                      u32IntFlag: 控制的中断源, 可以取下列值中的一个或几个的或
**                      SDMMC_INT_CCRCFAIL: Command response received (CRC check failed) interrupt
**                      SDMMC_INT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
**                      SDMMC_INT_CTIMEOUT: Command response timeout interrupt
**                      SDMMC_INT_DTIMEOUT: Data timeout interrupt
**                      SDMMC_INT_TXUNDERR: Transmit FIFO underrun error interrupt
**                      SDMMC_INT_RXOVERR:  Received FIFO overrun error interrupt
**                      SDMMC_INT_CMDREND:  Command response received (CRC check passed) interrupt
**                      SDMMC_INT_CMDSENT:  Command sent (no response required) interrupt
**                      SDMMC_INT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
**                      SDMMC_INT_DHOLD:    Data transfer Hold interrupt
**                      SDMMC_INT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
**                      SDMMC_INT_DABORT:   Data transfer aborted by CMD12 interrupt
**                      SDMMC_INT_TXFIFOHE: Transmit FIFO Half Empty interrupt
**                      SDMMC_INT_RXFIFOHF: Receive FIFO Half Full interrupt
**                      SDMMC_INT_RXFIFOF:  Receive FIFO full interrupt
**                      SDMMC_INT_TXFIFOE:  Transmit FIFO empty interrupt
**                      SDMMC_INT_BUSYD0END: End of SDMMC_D0 Busy following a CMD response detected interrupt
**                      SDMMC_INT_SDIOIT:    SDIO interrupt received interrupt
**                      SDMMC_INT_ACKFAIL:   Boot Acknowledgment received interrupt
**                      SDMMC_INT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
**                      SDMMC_INT_VSWEND:   Voltage switch critical timing section completion interrupt
**                      SDMMC_INT_CKSTOP:   SDMMC_CK stopped in Voltage switch procedure interrupt
**                      SDMMC_INT_IDMABTC:  IDMA buffer transfer complete interrupt
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcIntDisable(rt_uint32_t u32Base, rt_uint32_t u32IntFlag)
{
    HWREG32(u32Base + SDMMC_MASK) &= ~u32IntFlag;
}

/*********************************************************************************************************
** Function name:       SdmmcFlagStatus
** Descriptions:        获取Flag状态
** input parameters:    u32Base:    SDMMC基地址
** output parameters:   NONE
** Returned value:      flag状态值，为下列值中的一个或几个的或：
**                      SDMMC_FLAG_CCRCFAIL: Command response received (CRC check failed)
**                      SDMMC_FLAG_DCRCFAIL: Data block sent/received (CRC check failed)
**                      SDMMC_FLAG_CTIMEOUT: Command response timeout
**                      SDMMC_FLAG_DTIMEOUT: Data timeout
**                      SDMMC_FLAG_TXUNDERR: Transmit FIFO underrun error
**                      SDMMC_FLAG_RXOVERR:  Received FIFO overrun error
**                      SDMMC_FLAG_CMDREND:  Command response received (CRC check passed)
**                      SDMMC_FLAG_CMDSENT:  Command sent (no response required)
**                      SDMMC_FLAG_DATAEND:  Data end (data counter, SDIDCOUNT, is zero)
**                      SDMMC_FLAG_DHOLD:    Data transfer Hold
**                      SDMMC_FLAG_DBCKEND:  Data block sent/received (CRC check passed)
**                      SDMMC_FLAG_DABORT:   Data transfer aborted by CMD12
**                      SDMMC_FLAG_DPSMACT:  Data path state machine active
**                      SDMMC_FLAG_CPSMACT:  Command path state machine active
**                      SDMMC_FLAG_TXFIFOHE: Transmit FIFO Half Empty
**                      SDMMC_FLAG_RXFIFOHF: Receive FIFO Half Full
**                      SDMMC_FLAG_TXFIFOF:  Transmit FIFO full
**                      SDMMC_FLAG_RXFIFOF:  Receive FIFO full
**                      SDMMC_FLAG_TXFIFOE:  Transmit FIFO empty
**                      SDMMC_FLAG_RXFIFOE:  Receive FIFO empty
**                      SDMMC_FLAG_BUSYD0:   Inverted value of SDMMC_D0 line (Busy)
**                      SDMMC_FLAG_BUSYD0END:End of SDMMC_D0 Busy following a CMD response detected
**                      SDMMC_FLAG_SDIOIT:   SDIO interrupt received
**                      SDMMC_FLAG_ACKFAIL:  Boot Acknowledgment received
**                      SDMMC_FLAG_ACKTIMEOUT: Boot Acknowledgment timeout
**                      SDMMC_FLAG_VSWEND:   Voltage switch critical timing section completion
**                      SDMMC_FLAG_CKSTOP:   SDMMC_CK stopped in Voltage switch procedure
**                      SDMMC_FLAG_IDMATE:   IDMA transfer error
**                      SDMMC_FLAG_IDMABTC:  IDMA buffer transfer complete
*********************************************************************************************************/
rt_uint32_t SdmmcFlagStatus(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + SDMMC_STA));
}

/*********************************************************************************************************
** Function name:       SdmmcFlagClear
** Descriptions:        清除Flag状态
** input parameters:    u32Base:    SDMMC基地址
**                      u32Flag: 控制的Flag值 可以取下列值中的一个或几个的或
**                      SDMMC_FLAG_CCRCFAIL: Command response received (CRC check failed)
**                      SDMMC_FLAG_DCRCFAIL: Data block sent/received (CRC check failed)
**                      SDMMC_FLAG_CTIMEOUT: Command response timeout
**                      SDMMC_FLAG_DTIMEOUT: Data timeout
**                      SDMMC_FLAG_TXUNDERR: Transmit FIFO underrun error
**                      SDMMC_FLAG_RXOVERR:  Received FIFO overrun error
**                      SDMMC_FLAG_CMDREND:  Command response received (CRC check passed)
**                      SDMMC_FLAG_CMDSENT:  Command sent (no response required)
**                      SDMMC_FLAG_DATAEND:  Data end (data counter, SDIDCOUNT, is zero)
**                      SDMMC_FLAG_DHOLD:    Data transfer Hold
**                      SDMMC_FLAG_DBCKEND:  Data block sent/received (CRC check passed)
**                      SDMMC_FLAG_BUSYD0END: End of SDMMC_D0 Busy following a CMD response detected
**                      SDMMC_FLAG_SDIOIT:   SDIO interrupt received
**                      SDMMC_FLAG_ACKFAIL:  Boot Acknowledgment received
**                      SDMMC_FLAG_ACKTIMEOUT: Boot Acknowledgment timeout
**                      SDMMC_FLAG_VSWEND:   Voltage switch critical timing section completion
**                      SDMMC_FLAG_CKSTOP:   SDMMC_CK stopped in Voltage switch procedure
**                      SDMMC_FLAG_IDMATE:   IDMA transfer error
**                      SDMMC_FLAG_IDMABTC:  IDMA buffer transfer complete
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcFlagClear(rt_uint32_t u32Base, rt_uint32_t u32Flag)
{
    HWREG32(u32Base + SDMMC_ICR) = u32Flag;
}

/*********************************************************************************************************
** Function name:       SdmmcIntStatus
** Descriptions:        获取Int状态
** input parameters:    u32Base:    SDMMC基地址
** output parameters:   NONE
** Returned value:      返回的中断状态值，可以取下列值中的一个或几个的或
**                      SDMMC_INT_CCRCFAIL: Command response received (CRC check failed) interrupt
**                      SDMMC_INT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
**                      SDMMC_INT_CTIMEOUT: Command response timeout interrupt
**                      SDMMC_INT_DTIMEOUT: Data timeout interrupt
**                      SDMMC_INT_TXUNDERR: Transmit FIFO underrun error interrupt
**                      SDMMC_INT_RXOVERR:  Received FIFO overrun error interrupt
**                      SDMMC_INT_CMDREND:  Command response received (CRC check passed) interrupt
**                      SDMMC_INT_CMDSENT:  Command sent (no response required) interrupt
**                      SDMMC_INT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
**                      SDMMC_INT_DHOLD:   Data transfer Hold interrupt
**                      SDMMC_INT_DBCKEND: Data block sent/received (CRC check passed) interrupt
**                      SDMMC_INT_DABORT:  Data transfer aborted by CMD12 interrupt
**                      SDMMC_INT_CPSMACT: Command path state machine active interrupt
**                      SDMMC_INT_DPSMACT: Data path state machine active interrupt
**                      SDMMC_INT_TXFIFOHE: Transmit FIFO Half Empty interrupt
**                      SDMMC_INT_RXFIFOHF: Receive FIFO Half Full interrupt
**                      SDMMC_INT_TXFIFOF:  Transmit FIFO full interrupt
**                      SDMMC_INT_RXFIFOF:  Receive FIFO full interrupt
**                      SDMMC_INT_TXFIFOE:  Transmit FIFO empty interrupt
**                      SDMMC_INT_RXFIFOE:  Receive FIFO empty interrupt
**                      SDMMC_INT_BUSYD0:   Inverted value of SDMMC_D0 line (Busy)
**                      SDMMC_INT_BUSYD0END:End of SDMMC_D0 Busy following a CMD response detected interrupt
**                      SDMMC_INT_SDIOIT:   SDIO interrupt received interrupt
**                      SDMMC_INT_ACKFAIL:  Boot Acknowledgment received interrupt
**                      SDMMC_INT_ACKTIMEOUT:Boot Acknowledgment timeout interrupt
**                      SDMMC_INT_VSWEND:   Voltage switch critical timing section completion interrupt
**                      SDMMC_INT_CKSTOP:   SDMMC_CK stopped in Voltage switch procedure interrupt
**                      SDMMC_INT_IDMATE:   IDMA transfer error interrupt
**                      SDMMC_INT_IDMABTC:  IDMA buffer transfer complete interrupt
*********************************************************************************************************/
rt_uint32_t SdmmcIntStatus(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + SDMMC_STA));
}

/*********************************************************************************************************
** Function name:       SdmmcIntClear
** Descriptions:        清除Int状态
** input parameters:    u32Base:    SDMMC基地址
**                      u32IntFlag: 控制的中断源，可以取下列值中的一个或几个的或
**                      SDMMC_IT_CCRCFAIL:   Command response received (CRC check failed) interrupt
**                      SDMMC_IT_DCRCFAIL:   Data block sent/received (CRC check failed) interrupt
**                      SDMMC_IT_CTIMEOUT:   Command response timeout interrupt
**                      SDMMC_IT_DTIMEOUT:   Data timeout interrupt
**                      SDMMC_IT_TXUNDERR:   Transmit FIFO underrun error interrupt
**                      SDMMC_IT_RXOVERR:    Received FIFO overrun error interrupt
**                      SDMMC_IT_CMDREND:    Command response received (CRC check passed) interrupt
**                      SDMMC_IT_CMDSENT:    Command sent (no response required) interrupt
**                      SDMMC_IT_DATAEND:    Data end (data counter, DATACOUNT, is zero) interrupt
**                      SDMMC_IT_DHOLD:      Data transfer Hold interrupt
**                      SDMMC_IT_DBCKEND:    Data block sent/received (CRC check passed) interrupt
**                      SDMMC_IT_DABORT:     Data transfer aborted by CMD12 interrupt
**                      SDMMC_IT_BUSYD0END:  End of SDMMC_D0 Busy following a CMD response detected interrupt
**                      SDMMC_IT_SDIOIT:     SDIO interrupt received interrupt
**                      SDMMC_IT_ACKFAIL:    Boot Acknowledgment received interrupt
**                      SDMMC_IT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
**                      SDMMC_IT_VSWEND:     Voltage switch critical timing section completion interrupt
**                      SDMMC_IT_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure interrupt
**                      SDMMC_IT_IDMATE:     IDMA transfer error interrupt
**                      SDMMC_IT_IDMABTC:    IDMA buffer transfer complete interrupt
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcIntClear(rt_uint32_t u32Base, rt_uint32_t u32IntFlag)
{
    HWREG32(u32Base + SDMMC_ICR) = u32IntFlag;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
