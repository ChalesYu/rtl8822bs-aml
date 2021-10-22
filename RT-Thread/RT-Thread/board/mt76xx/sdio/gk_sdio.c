/****************************************Copyright (c)****************************************************
**                           湖 南 国 科 微 电 子 股 份 有 限 公 司
**                                http://www.gokemicro.com
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           gk_sdio.c
** Last modified Date:  2020-01-07
** Last Version:        V1.00
** Description:         驱动sdio接口实现
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2020-01-07
** Version:             V1.00
** Descriptions:
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
*********************************************************************************************************/
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <stdint.h>

#include "./../config.h"
#include "./../os/wlan_rtthread.h"
#include "./../os/gl_typedef.h"
#include "./../nic/mtx6xx_reg.h"
#include "./../nic/hal.h"

/*********************************************************************************************************
** 调试输出宏定义
*********************************************************************************************************/
#define DBG_TAG         "SDIO_HAL"
#define DBG_LVL         DBG_INFO    // DBG_LOG   DBG_WARNING   DBG_INFO
#include <rtdbg.h>

/*********************************************************************************************************
** 一些配置
*********************************************************************************************************/
// sdio访问重试次数
#define HIF_SDIO_ACCESS_RETRY_LIMIT             3
// sdio中断等待超时
#define HIF_SDIO_INTERRUPT_RESPONSE_TIMEOUT     (15000)

/*********************************************************************************************************
** Function name:       gk_sdio_interrupt
** Descriptions:        sdio中断处理程序
** Input parameters:    func：   sdio功能结构
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void gk_sdio_interrupt(struct rt_sdio_function *func)
{
  LOG_I("enter gk sdio interrupt");
  //  P_GLUE_INFO_T prGlueInfo = NULL;
  //  
  //  int ret = 0;
  //  
  //  prGlueInfo = sdio_get_drvdata(func);
  //  /* ASSERT(prGlueInfo); */
  //  
  //  if (!prGlueInfo) {
  //    /* printk(KERN_INFO DRV_NAME"No glue info in mtk_sdio_interrupt()\n"); */
  //    return;
  //  }
  //  
  //  if (prGlueInfo->ulFlag & GLUE_FLAG_HALT) {
  //    sdio_writeb(prGlueInfo->rHifInfo.func, WHLPCR_INT_EN_CLR, MCR_WHLPCR, &ret);
  //    /* printk(KERN_INFO DRV_NAME"GLUE_FLAG_HALT skip INT\n"); */
  //    return;
  //  }
  //  
  //  sdio_writeb(prGlueInfo->rHifInfo.func, WHLPCR_INT_EN_CLR, MCR_WHLPCR, &ret);
  //  
  //  prGlueInfo->rHifInfo.fgIsPendingInt = FALSE;
  //  
  //  kalSetIntEvent(prGlueInfo);
}

/*********************************************************************************************************
** Function name:       glSetHifInfo
** Descriptions:        设置hif接口结构
** Input parameters:    prwlan_info:   wlan驱动结构
**                      u32Cookie：    驱动接口标志
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void glSetHifInfo(gk_wlan_info *prwlan_info, uint32_t u32Cookie)
{
  prwlan_info->func = (struct rt_sdio_function *)u32Cookie;
  sdio_set_drvdata(prwlan_info->func, prwlan_info);
}

/*********************************************************************************************************
** Function name:       glBusInit
** Descriptions:        初始化总线配置
** Input parameters:    pvData:   总线信息结构
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_bool_t glBusInit(void *pvData)
{
  int ret = 0;
  struct rt_sdio_function *func = NULL;
  
  func = (struct rt_sdio_function *)pvData;
  
  sdio_claim_host(func);
  
  ret = sdio_set_block_size(func, 512);
  sdio_release_host(func);
  
  return RT_TRUE;
}

/*********************************************************************************************************
** Function name:       glBusSetIrq
** Descriptions:        注册中断服务函数
** Input parameters:    prwlan_info:   wlan驱动结构
**                      pfisr：        中断服务函数
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int glBusSetIrq(gk_wlan_info *prwlan_info, void *pfisr)
{
  int ret;
  
  sdio_claim_host(prwlan_info->func);
  ret = sdio_attach_irq(prwlan_info->func, gk_sdio_interrupt);
  sdio_release_host(prwlan_info->func);

//  prHifInfo->fgIsPendingInt = FALSE;
  
  return ret;
}

/*********************************************************************************************************
** Function name:       glBusFreeIrq
** Descriptions:        注销中断服务函数
** Input parameters:    prwlan_info:   wlan驱动结构
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void glBusFreeIrq(gk_wlan_info *prwlan_info)
{
  sdio_claim_host(prwlan_info->func);
  sdio_detach_irq(prwlan_info->func);
  sdio_release_host(prwlan_info->func);
}

/*********************************************************************************************************
** Function name:       glIsReadClearReg
** Descriptions:        查阅指定的寄存器是否被清除
** Input parameters:    u32Address:  清除寄存器地址
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_bool_t glIsReadClearReg(uint32_t u32Address)
{
  switch (u32Address) {
  case MCR_WHISR:
  case MCR_WASR:
  case MCR_D2HRM0R:
  case MCR_D2HRM1R:
  case MCR_WTQCR0:
  case MCR_WTQCR1:
  case MCR_WTQCR2:
  case MCR_WTQCR3:
  case MCR_WTQCR4:
  case MCR_WTQCR5:
  case MCR_WTQCR6:
  case MCR_WTQCR7:
    return RT_TRUE;
    
  default:
    return RT_FALSE;
  }
}

/*********************************************************************************************************
** Function name:       kalDevRegRead
** Descriptions:        通过sdio读取一个寄存器，读取寄存器位于SDIO host driver domian
** Input parameters:    prwlan_info:   wlan驱动结构
**                      u32Register:   读的寄存器地址                   
** Output parameters:   pu32Value:     读到的寄存器值存放地址
** Returned value:      RT_TRUE: 读取成功；  RT_FALSE：读取失败
*********************************************************************************************************/
rt_bool_t kalDevRegRead(gk_wlan_info *prwlan_info, uint32_t u32Register, uint32_t *pu32Value)
{
  int ret = 0;
  uint8_t u8RetryCount = 0;
  
  do {
    sdio_claim_host(prwlan_info->func);
    *pu32Value = sdio_io_readl(prwlan_info->func, u32Register, &ret);
    sdio_release_host(prwlan_info->func);
    
    if (ret || u8RetryCount) {
      /* DBGLOG(HAL, ERROR,
      *  ("sdio_io_readl() addr: 0x%08x value: 0x%08x status: %x retry: %u\n",
      *  u4Register, (unsigned int)*pu4Value, (unsigned int)ret, ucRetryCount));
      */
      
      if (glIsReadClearReg(u32Register) && (u8RetryCount == 0)) {
        /* Read Snapshot CR instead */
        u32Register = MCR_WSR;
      }
    }
    
    u8RetryCount++;
    if (u8RetryCount > HIF_SDIO_ACCESS_RETRY_LIMIT)
      break;
  } while (ret);
  
  if (ret) {
    LOG_E("sdio_io_readl() reports error: %x retry: %u", ret, u8RetryCount);
  }
  
  return (ret) ? RT_FALSE : RT_TRUE;
}

/*********************************************************************************************************
** Function name:       kalDevRegWrite
** Descriptions:        通过sdio写一个寄存器，读取寄存器位于SDIO host driver domian
** Input parameters:    prwlan_info:   wlan驱动结构
**                      u32Register:   写的寄存器地址                   
** Output parameters:   32Value:       写到的寄存器值
** Returned value:      RT_TRUE: 写入成功；  RT_FALSE：写入失败
*********************************************************************************************************/
rt_bool_t kalDevRegWrite(gk_wlan_info *prwlan_info, uint32_t u32Register, uint32_t u32Value)
{
  int ret = 0;
  uint8_t u8RetryCount = 0;
  
  do {

    sdio_claim_host(prwlan_info->func);
    ret = sdio_io_writel(prwlan_info->func, u32Value, u32Register);
    sdio_release_host(prwlan_info->func);
    
    if (ret || u8RetryCount) {
      /* DBGLOG(HAL, ERROR,
      *  ("sdio_writel() addr: 0x%x status: %x retry: %u\n", u4Register,
      *  ret, u8RetryCount));
      */
    }
    
    u8RetryCount++;
    if (u8RetryCount > HIF_SDIO_ACCESS_RETRY_LIMIT)
      break;
    
  } while (ret);
  
  if (ret) {
    LOG_E("sdio_io_writel() reports error: %x retry: %u", ret, u8RetryCount);
  }
  
  return (ret) ? RT_FALSE : RT_TRUE;
}

/*********************************************************************************************************
** Function name:       kalDevRegRead_mac
** Descriptions:        通过sdio读取一个寄存器，读取寄存器位于chip firmware register domain
** Input parameters:    prwlan_info:   wlan驱动结构
**                      u32Register:   读的寄存器地址                   
** Output parameters:   pu32Value:     读到的寄存器值存放地址
** Returned value:      RT_TRUE: 读取成功；  RT_FALSE：读取失败
*********************************************************************************************************/
rt_bool_t kalDevRegRead_mac(gk_wlan_info *prwlan_info, uint32_t u32Register, uint32_t *pu32Value)
{
  uint32_t value;
  uint32_t u32Time, u32Current;
  
  /* progrqm h2d mailbox0 as interested register address */
  kalDevRegWrite(prwlan_info, MCR_H2DSM0R, u32Register);
  
  /* set h2d interrupt to notify firmware. bit16 */
  kalDevRegWrite(prwlan_info, MCR_WSICR, SDIO_MAILBOX_FUNC_READ_REG_IDX);
  
  /* polling interrupt status asserted. bit16 */
  
  /* first, disable interrupt enable for SDIO_MAILBOX_FUNC_READ_REG_IDX */
  kalDevRegRead(prwlan_info, MCR_WHIER, &value);
  kalDevRegWrite(prwlan_info, MCR_WHIER, (value & ~SDIO_MAILBOX_FUNC_READ_REG_IDX));
  
  u32Time = rt_tick_get();
  
  do {
    /* check bit16 of WHISR assert for read register response */
    kalDevRegRead(prwlan_info, MCR_WHISR, &value);
    
    if (value & SDIO_MAILBOX_FUNC_READ_REG_IDX) {
      /* read d2h mailbox0 for interested register address */
      kalDevRegRead(prwlan_info, MCR_D2HRM0R, &value);
      
      if (value != u32Register) {
        LOG_E("ERROR! kalDevRegRead_mac():register address mis-match");
        LOG_E("(u32Register = 0x%08x, reported register = 0x%08x)",u32Register, value);
        return  FALSE;
      }
      
      /* read d2h mailbox1 for the value of the register */
      kalDevRegRead(prwlan_info, MCR_D2HRM1R, &value);
      *pu32Value = value;
      return	TRUE;
    }
    
    /* timeout exceeding check */
    u32Current = rt_tick_get();
    
    if (((u32Current > u32Time) && ((u32Current - u32Time) > HIF_SDIO_INTERRUPT_RESPONSE_TIMEOUT))
        || (u32Current < u32Time && ((u32Current + (0xFFFFFFFF - u32Time))
                                   > HIF_SDIO_INTERRUPT_RESPONSE_TIMEOUT))) {
                                     LOG_E("ERROR: kalDevRegRead_mac(): response timeout");
                                     return	FALSE;
                                   }
    
    /* Response packet is not ready */
    rt_hw_us_delay(50);
  } while (1);
}

/*********************************************************************************************************
** Function name:       kalDevRegWrite_mac
** Descriptions:        通过sdio写一个寄存器，读取寄存器位于chip firmware register domain
** Input parameters:    prwlan_info:   wlan驱动结构
**                      u32Register:   写的寄存器地址                   
** Output parameters:   u32Value:      写的寄存器值
** Returned value:      RT_TRUE: 写入成功；  RT_FALSE：写入失败
*********************************************************************************************************/
rt_bool_t kalDevRegWrite_mac(gk_wlan_info *prwlan_info, uint32_t u32Register, uint32_t u32Value)
{
  uint32_t value;
  uint32_t u32Time, u32Current;
  
  /* progrqm h2d mailbox0 as interested register address */
  kalDevRegWrite(prwlan_info, MCR_H2DSM0R, u32Register);
  
  /* progrqm h2d mailbox1 as the value to write */
  kalDevRegWrite(prwlan_info, MCR_H2DSM1R, u32Value);
  
  /*  set h2d interrupt to notify firmware bit17 */
  kalDevRegWrite(prwlan_info, MCR_WSICR, SDIO_MAILBOX_FUNC_WRITE_REG_IDX);
  
  /* polling interrupt status asserted. bit17 */
  
  /* first, disable interrupt enable for SDIO_MAILBOX_FUNC_WRITE_REG_IDX */
  kalDevRegRead(prwlan_info, MCR_WHIER, &value);
  kalDevRegWrite(prwlan_info, MCR_WHIER, (value & ~SDIO_MAILBOX_FUNC_WRITE_REG_IDX));
  
  u32Time = rt_tick_get();
  
  do {
    /* check bit17 of WHISR assert for response */
    kalDevRegRead(prwlan_info, MCR_WHISR, &value);
    
    if (value & SDIO_MAILBOX_FUNC_WRITE_REG_IDX) {
      /* read d2h mailbox0 for interested register address */
      kalDevRegRead(prwlan_info, MCR_D2HRM0R, &value);
      
      if (value != u32Register) {
        LOG_E("ERROR! kalDevRegWrite_mac():register address mis-match");
        LOG_E("(u32Register = 0x%08x, reported register = 0x%08x)", u32Register, value);
        return  FALSE;
      }
      return	TRUE;
    }
    
    /* timeout exceeding check */
    u32Current = rt_tick_get();
    
    if (((u32Current > u32Time) && ((u32Current - u32Time) > HIF_SDIO_INTERRUPT_RESPONSE_TIMEOUT))
        || (u32Current < u32Time && ((u32Current + (0xFFFFFFFF - u32Time))
                                   > HIF_SDIO_INTERRUPT_RESPONSE_TIMEOUT))) {
                                     LOG_E("ERROR: kalDevRegWrite_mac(): response timeout");
                                     return	FALSE;
                                   }
    
    /* Response packet is not ready */
    rt_hw_us_delay(50);
  } while (1);
}

/*********************************************************************************************************
** Function name:       kalDevPortRead
** Descriptions:        Read device I/O port
** Input parameters:    prwlan_info:   wlan驱动结构
**                      u16Port:       I/O port offset
**                      u32Len:        Length to be read    
**                      u32ValidOutBufSize:  Length of the buffer valid to be accessed            
** Output parameters:   pu8Buf:     Pointer to read buffer
** Returned value:      RT_TRUE: 读取成功；  RT_FALSE：读取失败
*********************************************************************************************************/
rt_bool_t kalDevPortRead(gk_wlan_info *prwlan_info,
	       uint16_t u16Port, uint32_t u32Len,uint8_t *pu8Buf, uint32_t u32ValidOutBufSize)
{
  uint8_t *pu8Dst = NULL;
  int count = u32Len;
  int ret = 0;
  int bNum = 0;
  struct rt_sdio_function *func = NULL;
  
  pu8Dst = pu8Buf;
  func = prwlan_info->func;
  
  sdio_claim_host(func);
  
  /* Split buffer into multiple single block to workaround hifsys */
  while (count >= func->cur_blk_size) {
    count -= func->cur_blk_size;
    bNum++;
  }
  if (count > 0 && bNum > 0)
    bNum++;
  
  if (bNum > 0) {
    ret = sdio_io_read_multi_fifo_b(func, u16Port, pu8Dst, func->cur_blk_size * bNum);
    
#ifdef CONFIG_X86
    /* ENE workaround */
    {
      int tmp;
      
      sdio_io_writel(func, 0x0, SDIO_X86_WORKAROUND_WRITE_MCR, &tmp);
    }
#endif
    
  } else {
    ret = sdio_io_read_multi_fifo_b(func, u16Port, pu8Dst, count);
  }
  
  sdio_release_host(func);

  
  if (ret) {
    LOG_E("sdio_io_read_multi_fifo_b() reports error: %x", ret);
  }
  
  return (ret) ? RT_FALSE : RT_TRUE;
}

/*********************************************************************************************************
** Function name:       kalDevPortWrite
** Descriptions:        Write device I/O port
** Input parameters:    prwlan_info:   wlan驱动结构
**                      u16Port:       I/O port offset
**                      u32Len:        Length to be write    
**                      u32ValidOutBufSize:  Length of the buffer valid to be accessed            
** Output parameters:   pu8Buf:     Pointer to write buffer
** Returned value:      RT_TRUE: 写入成功；  RT_FALSE：写入失败
*********************************************************************************************************/
rt_bool_t kalDevPortWrite(gk_wlan_info *prwlan_info,
		uint16_t u16Port, uint32_t u32Len, uint8_t *pu8Buf, uint32_t u32ValidInBufSize)
{
  uint8_t *pu8Src = NULL;
  int count = u32Len;
  int ret = 0;
  int bNum = 0;
  struct rt_sdio_function *func = NULL;
  
  pu8Src = pu8Buf;
  func = prwlan_info->func;
  
  sdio_claim_host(func);
  
  /* Split buffer into multiple single block to workaround hifsys */
  while (count >= func->cur_blk_size) {
    count -= func->cur_blk_size;
    bNum++;
  }
  if (count > 0 && bNum > 0)
    bNum++;
  
  if (bNum > 0) {		/* block mode */
    ret = sdio_io_write_multi_fifo_b(func, u16Port, pu8Src, func->cur_blk_size * bNum);
    
#ifdef CONFIG_X86
    /* ENE workaround */
    {
      int tmp;
      
      sdio_writel(func, 0x0, SDIO_X86_WORKAROUND_WRITE_MCR, &tmp);
    }
#endif
    
  } else {		/* byte mode */
    
    ret = sdio_io_write_multi_fifo_b(func, u16Port, pu8Src, count);
  }
  
  sdio_release_host(func);
  
  if (ret) {
    LOG_E("sdio_io_write_multi_fifo_b() reports error: %x", ret);
  }
  
  return (ret) ? FALSE : TRUE;
}

/*********************************************************************************************************
** Function name:       kalDevReadIntStatus
** Descriptions:        Read interrupt status from hardware
** Input parameters:    prwlan_info:   wlan驱动结构
**                      pu32IntStatus: the interrupts        
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void kalDevReadIntStatus(gk_wlan_info *prwlan_info, uint32_t *pu32IntStatus)
{
//#if CFG_SDIO_INTR_ENHANCE
////  P_SDIO_CTRL_T prSDIOCtrl;
////  P_SDIO_STAT_COUNTER_T prStatCounter;
//  
////  SDIO_TIME_INTERVAL_DEC();
//  
////  prSDIOCtrl = prAdapter->prGlueInfo->rHifInfo.prSDIOCtrl;
//  
////  prStatCounter = &prAdapter->prGlueInfo->rHifInfo.rStatCounter;
//  
//  /* There are pending interrupt to be handled */
//  if (prAdapter->prGlueInfo->rHifInfo.fgIsPendingInt)
//    prAdapter->prGlueInfo->rHifInfo.fgIsPendingInt = FALSE;
//  else {
////    SDIO_REC_TIME_START();
//    HAL_PORT_RD(prwlan_info, MCR_WHISR, sizeof(ENHANCE_MODE_DATA_STRUCT_T),
//                (PUINT_8) prSDIOCtrl, sizeof(ENHANCE_MODE_DATA_STRUCT_T));
//    SDIO_REC_TIME_END();
//    SDIO_ADD_TIME_INTERVAL(prStatCounter->u4IntReadTime);
//    prStatCounter->u4IntReadCnt++;
//  }
//  
//  prStatCounter->u4IntCnt++;
//  
//  if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE || fgIsBusAccessFailed == TRUE) {
//    *pu4IntStatus = 0;
//    return;
//  }
//  
//  halProcessEnhanceInterruptStatus(prAdapter);
//  
//  *pu4IntStatus = prSDIOCtrl->u4WHISR;
//#else
//  HAL_MCR_RD(prAdapter, MCR_WHISR, pu4IntStatus);
//#endif /* CFG_SDIO_INTR_ENHANCE */
//  
//  if (*pu4IntStatus & ~(WHIER_DEFAULT | WHIER_FW_OWN_BACK_INT_EN)) {
//    LOG_I("Un-handled HISR %#lx, HISR = %#lx (HIER:0x%lx)",
//           (*pu32IntStatus & ~WHIER_DEFAULT), *pu4IntStatus, WHIER_DEFAULT);
//    *pu4IntStatus &= WHIER_DEFAULT;
//  }
}

/*********************************************************************************************************
** Function name:       kalDevWriteWithSdioCmd52
** Descriptions:        Write device I/O port in byte with CMD52
** Input parameters:    prwlan_info:   wlan驱动结构
**                      u32Addr: I/O port offset 
**                      u8Data:  Single byte of data to be written
** Output parameters:   NONE
** Returned value:      RT_TRUE: 写入成功；  RT_FALSE：写入失败
*********************************************************************************************************/
rt_bool_t kalDevWriteWithSdioCmd52(gk_wlan_info *prwlan_info, uint32_t u32Addr, uint8_t u8Data)
{
  int ret = 0;
  
  sdio_claim_host(prwlan_info->func);
  ret = sdio_io_writeb(prwlan_info->func, u8Data, u32Addr);
  sdio_release_host(prwlan_info->func);
  
  if (ret) {
    LOG_E("sdio_writeb() reports error: %x", ret);
  }
  
  return (ret) ? FALSE : TRUE;
  
}

//VOID glSetPowerState(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 ePowerMode)
//{
//}


/*********************************************************************************************************
** Function name:       kalDevWriteData
** Descriptions:        Write data to device
** Input parameters:    prwlan_info:   wlan驱动结构
**                      prMsduInfo:    msdu info 
** Output parameters:   NONE
** Returned value:      RT_TRUE: 写入成功；  RT_FALSE：写入失败
*********************************************************************************************************/
//rt_bool_t kalDevWriteData(gk_wlan_info *prwlan_info, IN P_MSDU_INFO_T prMsduInfo)
//{
//  P_ADAPTER_T prAdapter = prGlueInfo->prAdapter;
//  P_GL_HIF_INFO_T prHifInfo = &prGlueInfo->rHifInfo;
//  P_TX_CTRL_T prTxCtrl;
//  PUINT_8 pucOutputBuf = (PUINT_8) NULL;
//  UINT_32 u4PaddingLength;
//  struct sk_buff *skb;
//  UINT_8 *pucBuf;
//  UINT_32 u4Length;
//  UINT_8 ucTC;
//  
//  SDIO_TIME_INTERVAL_DEC();
//  
//  skb = (struct sk_buff *)prMsduInfo->prPacket;
//  pucBuf = skb->data;
//  u4Length = skb->len;
//  ucTC = prMsduInfo->ucTC;
//  
//  prTxCtrl = &prAdapter->rTxCtrl;
//  pucOutputBuf = prTxCtrl->pucTxCoalescingBufPtr;
//  
//  if (prTxCtrl->u4WrIdx + ALIGN_4(u4Length) > prAdapter->u4CoalescingBufCachedSize) {
//    if ((prAdapter->u4CoalescingBufCachedSize - ALIGN_4(prTxCtrl->u4WrIdx)) >= HIF_TX_TERMINATOR_LEN) {
//      /* fill with single dword of zero as TX-aggregation termination */
//      *(PUINT_32) (&((pucOutputBuf)[ALIGN_4(prTxCtrl->u4WrIdx)])) = 0;
//    }
//    
//    if (HAL_TEST_FLAG(prAdapter, ADAPTER_FLAG_HW_ERR) == FALSE) {
//      if (kalDevPortWrite(prGlueInfo, MCR_WTDR1, prTxCtrl->u4WrIdx,
//                          pucOutputBuf, prAdapter->u4CoalescingBufCachedSize) == FALSE) {
//                            HAL_SET_FLAG(prAdapter, ADAPTER_FLAG_HW_ERR);
//                            fgIsBusAccessFailed = TRUE;
//                          }
//      prHifInfo->rStatCounter.u4DataPortWriteCnt++;
//    }
//    prTxCtrl->u4WrIdx = 0;
//  }
//  
//  SDIO_REC_TIME_START();
//  memcpy(pucOutputBuf + prTxCtrl->u4WrIdx, pucBuf, u4Length);
//  SDIO_REC_TIME_END();
//  SDIO_ADD_TIME_INTERVAL(prHifInfo->rStatCounter.u4TxDataCpTime);
//  
//  prTxCtrl->u4WrIdx += u4Length;
//  
//  u4PaddingLength = (ALIGN_4(u4Length) - u4Length);
//  if (u4PaddingLength) {
//    memset(pucOutputBuf + prTxCtrl->u4WrIdx, 0, u4PaddingLength);
//    prTxCtrl->u4WrIdx += u4PaddingLength;
//  }
//  
//  SDIO_REC_TIME_START();
//  if (!prMsduInfo->pfTxDoneHandler)
//    kalFreeTxMsdu(prAdapter, prMsduInfo);
//  SDIO_REC_TIME_END();
//  SDIO_ADD_TIME_INTERVAL(prHifInfo->rStatCounter.u4TxDataFreeTime);
//  
//  /* Update pending Tx done count */
//  prHifInfo->au4PendingTxDoneCount[ucTC]++;
//  
//  prHifInfo->rStatCounter.u4DataPktWriteCnt++;
//  
//  return TRUE;
//}

///*----------------------------------------------------------------------------*/
///*!
//* \brief Kick Tx data to device
//*
//* \param[in] prGlueInfo         Pointer to the GLUE_INFO_T structure.
//*
//* \retval TRUE          operation success
//* \retval FALSE         operation fail
//*/
///*----------------------------------------------------------------------------*/
//BOOL kalDevKickData(IN P_GLUE_INFO_T prGlueInfo)
//{
//  P_ADAPTER_T prAdapter = prGlueInfo->prAdapter;
//  P_GL_HIF_INFO_T prHifInfo = &prGlueInfo->rHifInfo;
//  P_TX_CTRL_T prTxCtrl;
//  PUINT_8 pucOutputBuf = (PUINT_8) NULL;
//  
//  prTxCtrl = &prAdapter->rTxCtrl;
//  pucOutputBuf = prTxCtrl->pucTxCoalescingBufPtr;
//  
//  if (prTxCtrl->u4WrIdx == 0)
//    return FALSE;
//  
//  if ((prAdapter->u4CoalescingBufCachedSize - ALIGN_4(prTxCtrl->u4WrIdx)) >= HIF_TX_TERMINATOR_LEN) {
//    /* fill with single dword of zero as TX-aggregation termination */
//    *(PUINT_32) (&((pucOutputBuf)[ALIGN_4(prTxCtrl->u4WrIdx)])) = 0;
//  }
//  
//  if (HAL_TEST_FLAG(prAdapter, ADAPTER_FLAG_HW_ERR) == FALSE) {
//    if (kalDevPortWrite(prGlueInfo, MCR_WTDR1, prTxCtrl->u4WrIdx,
//                        pucOutputBuf, prAdapter->u4CoalescingBufCachedSize) == FALSE) {
//                          HAL_SET_FLAG(prAdapter, ADAPTER_FLAG_HW_ERR);
//                          fgIsBusAccessFailed = TRUE;
//                        }
//    prHifInfo->rStatCounter.u4DataPortWriteCnt++;
//  }
//  
//  prTxCtrl->u4WrIdx = 0;
//  
//  prHifInfo->rStatCounter.u4DataPortKickCnt++;
//  
//  return TRUE;
//}
//
///*----------------------------------------------------------------------------*/
///*!
//* \brief Write command to device
//*
//* \param[in] prGlueInfo         Pointer to the GLUE_INFO_T structure.
//* \param[in] u4Addr             I/O port offset
//* \param[in] ucData             Single byte of data to be written
//*
//* \retval TRUE          operation success
//* \retval FALSE         operation fail
//*/
///*----------------------------------------------------------------------------*/
//BOOL kalDevWriteCmd(IN P_GLUE_INFO_T prGlueInfo, IN P_CMD_INFO_T prCmdInfo, IN UINT_8 ucTC)
//{
//  P_ADAPTER_T prAdapter = prGlueInfo->prAdapter;
//  /*	P_GL_HIF_INFO_T prHifInfo = &prGlueInfo->rHifInfo; */
//  P_TX_CTRL_T prTxCtrl;
//  PUINT_8 pucOutputBuf = (PUINT_8) NULL;
//  UINT_16 u2OverallBufferLength = 0;
//  /*	WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS; */
//  
//  prTxCtrl = &prAdapter->rTxCtrl;
//  pucOutputBuf = prTxCtrl->pucTxCoalescingBufPtr;
//  
//  if (TFCB_FRAME_PAD_TO_DW(prCmdInfo->u4TxdLen + prCmdInfo->u4TxpLen) >
//      prAdapter->u4CoalescingBufCachedSize) {
//        DBGLOG(HAL, ERROR, "Command TX buffer underflow!\n");
//        return FALSE;
//      }
//  if (prCmdInfo->u4TxdLen) {
//    memcpy((pucOutputBuf + u2OverallBufferLength), prCmdInfo->pucTxd, prCmdInfo->u4TxdLen);
//    u2OverallBufferLength += prCmdInfo->u4TxdLen;
//  }
//  
//  if (prCmdInfo->u4TxpLen) {
//    memcpy((pucOutputBuf + u2OverallBufferLength), prCmdInfo->pucTxp, prCmdInfo->u4TxpLen);
//    u2OverallBufferLength += prCmdInfo->u4TxpLen;
//  }
//  
//  memset(pucOutputBuf + u2OverallBufferLength, 0,
//         (TFCB_FRAME_PAD_TO_DW(u2OverallBufferLength) - u2OverallBufferLength));
//  
//  if ((prAdapter->u4CoalescingBufCachedSize - ALIGN_4(u2OverallBufferLength)) >= HIF_TX_TERMINATOR_LEN) {
//    /* fill with single dword of zero as TX-aggregation termination */
//    *(PUINT_32) (&((pucOutputBuf)[ALIGN_4(u2OverallBufferLength)])) = 0;
//  }
//  if (HAL_TEST_FLAG(prAdapter, ADAPTER_FLAG_HW_ERR) == FALSE) {
//    if (kalDevPortWrite(prGlueInfo, MCR_WTDR1, TFCB_FRAME_PAD_TO_DW(u2OverallBufferLength),
//                        pucOutputBuf, prAdapter->u4CoalescingBufCachedSize) == FALSE) {
//                          HAL_SET_FLAG(prAdapter, ADAPTER_FLAG_HW_ERR);
//                          fgIsBusAccessFailed = TRUE;
//                        }
//    prGlueInfo->rHifInfo.rStatCounter.u4CmdPortWriteCnt++;
//  }
//  
//  /* Update pending Tx done count */
//  prGlueInfo->rHifInfo.au4PendingTxDoneCount[ucTC]++;
//  
//  prGlueInfo->rHifInfo.rStatCounter.u4CmdPktWriteCnt++;
//  return TRUE;
//}

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
