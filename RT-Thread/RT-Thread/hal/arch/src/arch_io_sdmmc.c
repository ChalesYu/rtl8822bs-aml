/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_sdmmc.c
** Last modified Date:  2019-10-28
** Last Version:        v1.0
** Description:         SDMMCģ��Ĵ�����װ����ʵ������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2019-10-28
** Version:             v1.0
** Descriptions:        The original version ��ʼ�汾
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
** Descriptions:        SDMMCģ���ʼ������
** input parameters:    u32Base:  SDMMC����ַ
**                      u32Config: ���ò���������ȡ���м�������Ļ�
**                      SDMMCʱ�ӵĲ���ԭ��
**                      SDMMC_CLOCK_EDGE_RISING          // SDMMC CLK�����ز���
**                      SDMMC_CLOCK_EDGE_FALLING         // SDMMC CLK�½��ز���
**
**                      SDMMC����ʡ��ģʽ
**                      SDMMC_POWER_SAVE_DISABLE         // ʡ��ģʽ��ֹ
**                      SDMMC_POWER_SAVE_ENABLE          // ʡ��ģʽʹ��
**
**                      SDMMC���߿��
**                      SDMMC_BUS_WIDE_1B                // 1λ���ݿ��
**                      SDMMC_BUS_WIDE_4B                // 4λ���ݿ��
**                      SDMMC_BUS_WIDE_8B                // 8λ���ݿ��
**
**                      SDMMC����
**                      SDMMC_HW_FLOW_CTRL_DISABLE       // ���ؽ�ֹ
**                      SDMMC_HW_FLOW_CTRL_ENABLE        // ����ʹ��
**
**                      SDMMC��Ƶϵ��
**                      SDMMC_CLOCK_DIV_NUM(x)           // ��Ƶϵ��
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
** Descriptions:        SDMMC���ݶ�ȡ
** input parameters:    u32Base:  SDMMC����ַ
** output parameters:   NONE
** Returned value:      ��ȡ��������
*********************************************************************************************************/
rt_uint32_t SdmmcDataRead(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + SDMMC_FIFO));
}

/*********************************************************************************************************
** Function name:       SdmmcDataWrite
** Descriptions:        SDMMC����д
** input parameters:    u32Base:  SDMMC����ַ
**                      u32Data:  д������
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
** Descriptions:        SDMMC��Դ���ơ���
** input parameters:    u32Base:  SDMMC����ַ
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
** Descriptions:        SDMMC��Դ���ơ�Power Cycle
** input parameters:    u32Base:  SDMMC����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcPowerStateCycle(rt_uint32_t u32Base)
{
    HWREG32(u32Base + SDMMC_POWER) |= ((rt_uint32_t)0x000000002);
}

/*********************************************************************************************************
** Function name:       SdmmcPowerStateOff
** Descriptions:        SDMMC��Դ���ơ��ر�
** input parameters:    u32Base:  SDMMC����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcPowerStateOff(rt_uint32_t u32Base)
{
    HWREG32(u32Base + SDMMC_POWER) &= ~((rt_uint32_t)0x000000003);
}

/*********************************************************************************************************
** Function name:       SdmmcPowerStateStatus
** Descriptions:        SDMMC��Դ״̬
** input parameters:    u32Base:  SDMMC����ַ
** output parameters:   NONE
** Returned value:      ��Դ״̬������ȡ����ֵ��
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
** Descriptions:        SDMMC��������
** input parameters:    u32Base:            SDMMC����ַ
**                      u32SdmmcArg:         �������
**                      uint32SendConfig:   ���͵����ò���, ����ȡ���м�������Ļ�
**                      �����
**                      SDMMC_CMD_INDEX(x)           // �����

**                      ��Ӧ����
**                      SDMMC_RESPONSE_NO            // �޻�Ӧ
**                      SDMMC_RESPONSE_SHORT         // �̻�Ӧ
**                      SDMMC_RESPONSE_LONG          // ����Ӧ

**                      SDMMC�ȴ�����
**                      SDMMC_WAIT_NO                // �޵ȴ�
**                      SDMMC_WAIT_INT               // �ȴ��ж�
**                      SDMMC_WAIT_PEND              // �ȴ��������

**                      SDMMC CPSM״̬
**                      SDMMC_CPSM_DISABLE           // CPSM��ֹ
**                      SDMMC_CPSM_ENABLE            // CPSMʹ��
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
** Descriptions:        ��ȡ��һ�ε������Ӧ
** input parameters:    u32Base:  SDMMC����ַ
** output parameters:   NONE
** Returned value:      Returns command index of last command for which response received.
*********************************************************************************************************/
rt_uint8_t SdmmcCommandResponseGet(rt_uint32_t u32Base)
{
    return ((rt_uint8_t)HWREG32(u32Base + SDMMC_RESPCMD));
}

/*********************************************************************************************************
** Function name:       SdmmcResponseGet
** Descriptions:        ��ȡ��һ�ε������Ӧ
** input parameters:    u32Base:  SDMMC����ַ
**                      u32RESP:  ��ȡ��RESP�Ĵ���,����ȡ����ֵ��
**                      SDMMC_RESP1          // RESP1�Ĵ���
**                      SDMMC_RESP2          // RESP2�Ĵ���
**                      SDMMC_RESP3          // RESP3�Ĵ���
**                      SDMMC_RESP4          // RESP4�Ĵ���
** output parameters:   NONE
** Returned value:      ���ػ�Ӧֵ
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
** Descriptions:        SDMMC Data Path����
** input parameters:    u32Base:        SDMMC����ַ
**                      u32DataTimeOut: SDMMC Data TimeOut value
**                      u32DataLen:     SDMMC DataLength value
**                      u32DataCtrl:    SDMMC DCTRL value, ����ȡ���м�������Ļ�
**                      SDMMC���ݿ��С
**                      SDMMC_DATABLOCK_SIZE_1B          // 1���ֽ�
**                      SDMMC_DATABLOCK_SIZE_2B          // 2���ֽ�
**                      SDMMC_DATABLOCK_SIZE_4B          // 4���ֽ�
**                      SDMMC_DATABLOCK_SIZE_8B          // 8���ֽ�
**                      SDMMC_DATABLOCK_SIZE_16B         // 16���ֽ�
**                      SDMMC_DATABLOCK_SIZE_32B         // 32���ֽ�
**                      SDMMC_DATABLOCK_SIZE_64B         // 64���ֽ�
**                      SDMMC_DATABLOCK_SIZE_128B        // 128���ֽ�
**                      SDMMC_DATABLOCK_SIZE_256B        // 256���ֽ�
**                      SDMMC_DATABLOCK_SIZE_512B        // 512���ֽ�
**                      SDMMC_DATABLOCK_SIZE_1024B       // 1024���ֽ�
**                      SDMMC_DATABLOCK_SIZE_2048B       // 2048���ֽ�
**                      SDMMC_DATABLOCK_SIZE_4096B       // 4096���ֽ�
**                      SDMMC_DATABLOCK_SIZE_8192B       // 8192���ֽ�
**                      SDMMC_DATABLOCK_SIZE_16384B      // 16384���ֽ�
**
**                      SDMMC���ݴ��䷽��
**                      SDMMC_TRANSFERDIR_TO_CARD        // ���ݴ���CARD
**                      SDMMC_TRANSFERDIR_TO_SDMMC        // ���ݴ���SDMMC
**
**                      SDMMC��������
**                      SDMMC_TRANSFER_MODE_BLOCK            // Blockģʽ
**                      SDMMC_TRANSFER_MODE_SDIO_MULTIBYTE  // SDIO���ֽڴ���
**                      SDMMC_TRANSFER_MODE_STREAM          // Streamģʽ
**
**                      SDMMC_DPSM_State 
**                      SDMMC_DPSM_DISABLE               // DPSM��ֹ
**                      SDMMC_DPSM_ENABLE                // DPSMʹ��
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
** Descriptions:        ��ȡ�����ʣ����������
** input parameters:    u32Base:  SDMMC����ַ
** output parameters:   NONE
** Returned value:      ʣ�ഫ�����������
*********************************************************************************************************/
rt_uint32_t SdmmcDataCounterGet(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + SDMMC_DCOUNT));
}

/*********************************************************************************************************
** Function name:       SdmmcFIFOCounterGet
** Descriptions:        ��ȡ�����FIFOʣ������
** input parameters:    u32Base:  SDMMC����ַ
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
** input parameters:    u32Base:  SDMMC����ַ
**                      bIsSet:   ����״̬������ȡRT_TRUE����RT_FALSE
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
** input parameters:    u32Base:  SDMMC����ַ
**                      bIsSet:   ����״̬������ȡRT_TRUE����RT_FALSE
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
** input parameters:    u32Base:  SDMMC����ַ
**                      u32Mode:  �ȴ�ģʽ������ȡ����ֵ��
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
** input parameters:    u32Base:  SDMMC����ַ
**                      bIsSet:   ����״̬������ȡRT_TRUE����RT_FALSE
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
** input parameters:    u32Base:  SDMMC����ַ
**                      bIsSet:   ����״̬������ȡRT_TRUE����RT_FALSE
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
** input parameters:    u32Base:  SDMMC����ַ
**                      bIsSet:   ����״̬������ȡRT_TRUE����RT_FALSE
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
** input parameters:    u32Base:  SDMMC����ַ
**                      bIsSet:   ����״̬������ȡRT_TRUE����RT_FALSE
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
** Descriptions:        IDMA����
** input parameters:    u32Base:  SDMMC����ַ
**                      u32Config:  ȡ���в����Ļ�
**                       ����ģʽ��
**                       SDMMC_IDMA_DISABLE                     // ��ֹIDMA
**                       SDMMC_IDMA_ENABLE_SINGLE_BUFFER        // ������ģʽ
**                       SDMMC_IDMA_ENABLE_DOUBLE_BUFFER0       // ˫����ģʽʹ��BUFFER0
**                       SDMMC_IDMA_ENABLE_DOUBLE_BUFFER1       // ˫����ģʽʹ��BUFFER1    
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
** Descriptions:        IDMAģʽ������0����
** input parameters:    u32Base:  SDMMC����ַ
**                      u32Addr:  ��������ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SdmmcIdmaBuffer0Set(rt_uint32_t u32Base, rt_uint32_t u32Addr)
{
  HWREG32(u32Base + SDMMC_IDMABASE0) = u32Addr;
}

/*********************************************************************************************************
** Function name:       SdmmcIdmaBuffer1Set
** Descriptions:        IDMAģʽ������1����
** input parameters:    u32Base:  SDMMC����ַ
**                      u32Addr:  ��������ַ
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
** input parameters:    u32Base:    SDMMC����ַ
**                      u32IntFlag: ���Ƶ��ж�Դ, ����ȡ����ֵ�е�һ���򼸸��Ļ�
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
** input parameters:    u32Base:    SDMMC����ַ
**                      u32IntFlag: ���Ƶ��ж�Դ, ����ȡ����ֵ�е�һ���򼸸��Ļ�
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
** Descriptions:        ��ȡFlag״̬
** input parameters:    u32Base:    SDMMC����ַ
** output parameters:   NONE
** Returned value:      flag״ֵ̬��Ϊ����ֵ�е�һ���򼸸��Ļ�
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
** Descriptions:        ���Flag״̬
** input parameters:    u32Base:    SDMMC����ַ
**                      u32Flag: ���Ƶ�Flagֵ ����ȡ����ֵ�е�һ���򼸸��Ļ�
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
** Descriptions:        ��ȡInt״̬
** input parameters:    u32Base:    SDMMC����ַ
** output parameters:   NONE
** Returned value:      ���ص��ж�״ֵ̬������ȡ����ֵ�е�һ���򼸸��Ļ�
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
** Descriptions:        ���Int״̬
** input parameters:    u32Base:    SDMMC����ַ
**                      u32IntFlag: ���Ƶ��ж�Դ������ȡ����ֵ�е�һ���򼸸��Ļ�
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
