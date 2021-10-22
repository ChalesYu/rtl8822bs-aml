/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           stm32_usb_otg_host.c
** Last modified Date:  2020-05-20
** Last Version:        v1.00
** Description:         USB主机操作功能函数实现
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2020-05-20
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "hal/cortexm/hal_cortexm_api.h"

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"

#include "stm32_usb_otg.h"
#include "stm32_usb_otg_host.h"

/*********************************************************************************************************
** 函数声明
*********************************************************************************************************/
static void usb_otg_data_send(stm32_usb_host_t* host, uint8_t hc_num);

/*********************************************************************************************************
** Function name:       usb_host_port_changed_callback
** Descriptions:        主机Port状态改变回调函数
** input parameters:    host:       USB主机结构
**                      new_state： 改变后的状态
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
RT_WEAK void usb_host_port_changed_callback(stm32_usb_host_t* host, host_port_state_t new_state)
{
}

/*********************************************************************************************************
** Function name:       usb_host_sof_callback
** Descriptions:        主机sof回调
** input parameters:    hcd:    USB主机结构
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
RT_WEAK void usb_host_sof_callback(stm32_usb_host_t* host)
{
}

/*********************************************************************************************************
** Function name:       usb_host_notify_urb_finish_callback
** Descriptions:        主机一次urb完成回调
** input parameters:    host:    USB主机结构
**                      chnum:  主机通道编号
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void usb_host_notify_urb_finish_callback(stm32_usb_host_t *host, uint8_t chnum)
{
   stm32_usb_host_channel_t* hc = &host->hc[chnum];
    if(hc->xfer_done){
        rt_completion_done(&hc->urb_completion);
    }
}

/*********************************************************************************************************
** Function name:       usb_otg_phy_clock_set
** Descriptions:        设置USB OTG的Phy clock
** input parameters:    u32Base:    USB在CPU总线的基地址
**                      u8Freq：    时钟频率，可以选下列值中的一个
**                             HCFG_48_MHZ      // 48 MHz Clock 
**                             HCFG_6_MHZ       // 6 MHz Clock       
** output parameters:   NONE
** Returned value:      -1: 初始化失败； 0：初始化成功
*********************************************************************************************************/
static void usb_otg_phy_clock_set(uint32_t u32Base, uint8_t u8Freq)
{
  HWREG32(u32Base + USB_OTG_HCFG) &= ~(3U << 0);
  HWREG32(u32Base + USB_OTG_HCFG) |= ((3U << 0) & (uint32_t)(u8Freq));
  
  if (u8Freq ==  HCFG_48_MHZ)
  {
    HWREG32(u32Base + USB_OTG_HFIR) = 48000U;
  }
  else if (u8Freq ==  HCFG_6_MHZ)
  {
     HWREG32(u32Base + USB_OTG_HFIR) = 6000U;
  }
}

/*********************************************************************************************************
** Function name:       usb_otg_host_stop
** Descriptions:        终止usb host
** input parameters:    host:        USB主机特征结构
**                      u8Freq：    时钟频率，可以选下列值中的一个
**                             HCFG_48_MHZ      // 48 MHz Clock 
**                             HCFG_6_MHZ       // 6 MHz Clock       
** output parameters:   NONE
** Returned value:      -1: 初始化失败； 0：初始化成功
*********************************************************************************************************/
static void usb_otg_host_stop(stm32_usb_host_t* host)
{
  uint32_t u32Base = host->base;
  uint32_t value;
  uint32_t i;

  /* Disable global int */
  HWREG32(u32Base + USB_OTG_GAHBCFG) &= ~USB_OTG_GAHBCFG_GINT;
  /* Flush FIFO */
  usb_flush_tx(u32Base, 0x10U);
  usb_flush_rx(u32Base);
  
  /* Flush out any leftover queued requests. */
  for (i = 0U; i <= 15U; i++)
  {
    value = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i);
    value |=  USB_OTG_HCCHAR_CHDIS;
    value &= ~USB_OTG_HCCHAR_CHENA;
    value &= ~USB_OTG_HCCHAR_EPDIR;
    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i) = value;
  }

  /* Halt all channels to put them into a known state. */
  for (i = 0U; i <= 15U; i++)
  {
    uint32_t count = 0U;
    value = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i);
    value |= USB_OTG_HCCHAR_CHDIS;
    value |= USB_OTG_HCCHAR_CHENA;
    value &= ~USB_OTG_HCCHAR_EPDIR;
    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i) = value;
    do{
      if (++count > 2000U) break;
    }while ((HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i) & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
  }

  /* Clear any pending Host interrupts */
  HWREG32(u32Base + USB_OTG_HAINT) = 0xFFFFFFFFU;
  HWREG32(u32Base + USB_OTG_GINTSTS) = 0xFFFFFFFFU;
  /* Enable global int */
  HWREG32(u32Base + USB_OTG_GAHBCFG) |= USB_OTG_GAHBCFG_GINT;
}

/*********************************************************************************************************
** Function name:       usb_otg_host_disconnect
** Descriptions:        usb host 断开连接处理函数
** input parameters:    host:       USB主机特征结构     
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void usb_otg_host_disconnect(stm32_usb_host_t* host)
{
  uint32_t u32Base = host->base;
  
  usb_otg_host_stop(host);
  host->state = USB_HOST_PORT_DISCONNECTED;
  
  memset(host->hc, 0, sizeof(host->hc));
  
  HWREG32(u32Base + USB_OTG_GAHBCFG) |= USB_OTG_GAHBCFG_GINT;
  usb_otg_vbus_drive(u32Base, 1);
  
  usb_host_port_changed_callback(host, USB_HOST_PORT_DISCONNECTED);
}

/*********************************************************************************************************
** Function name:       usb_host_port_handler
** Descriptions:        usb host port中断处理
** input parameters:    host:       USB主机特征结构     
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void usb_host_port_handler(stm32_usb_host_t* host)
{
  uint32_t u32Base = host->base;
  uint32_t hprt0 = HWREG32(u32Base + USB_OTG_HPRT);
  uint32_t hprt0_dup = HWREG32(u32Base + USB_OTG_HPRT);
  
  hprt0_dup &= ~((1UL << 2) | (1UL << 1) | (1UL << 3) | (1UL << 5));
  
  if( hprt0 & (1UL << 1) ) {
    // Port connect detect
    if( hprt0 & (1UL << 0) ){
      // A device is attached
      HWREG32(u32Base + USB_OTG_GINTMSK) &= ~(USB_OTG_GINTSTS_DISCINT);
      // Post connect message
      host->state = USB_HOST_PORT_CONNECTED;
      usb_host_port_changed_callback(host, USB_HOST_PORT_CONNECTED);
    }
    // write 1 to clear the PCDET int flag
    hprt0_dup  |= (1UL << 1);
  }
  
  if( hprt0 & (1UL << 3) ) {
    // Port enable changed
    if( hprt0 & (1UL << 2) ){
      if(host->phy_interface  == USB_OTG_EMBEDDED_FS_PHY) {
        if ((hprt0 & (3UL << 17)) == (USB_OTG_SPEED_LOW << 17)){
          // Low speed
          usb_otg_phy_clock_set(u32Base, HCFG_6_MHZ);
        }else{
          // Full speed
          usb_otg_phy_clock_set(u32Base, HCFG_48_MHZ);
        }
      }else{
        if(host->phy_speed == USB_OTG_SPEED_FULL) {
          // FS phy
          HWREG32(u32Base + USB_OTG_HFIR) = 6000U;
        } else {
          HWREG32(u32Base + USB_OTG_HCFG) &= ~(3U << 0);
          HWREG32(u32Base + USB_OTG_HFIR) = 60000U;
        }
      }
      host->state = USB_HOST_PORT_ENABLED;
      /* Flush FIFO */
      usb_flush_tx(u32Base, 0x10U);
      usb_flush_rx(u32Base);
      usb_host_port_changed_callback(host, USB_HOST_PORT_ENABLED);
      // Port enabled
    }else{
      // Port disabled
      HWREG32(u32Base + USB_OTG_HPRT) &= ~((1UL << 2) | (1UL << 1) | (1UL << 3) | (1UL << 5) );
      HWREG32(u32Base + USB_OTG_GINTMSK) |= USB_OTG_GINTSTS_DISCINT;
      host->state = USB_HOST_PORT_DISABLED;
      usb_host_port_changed_callback(host, USB_HOST_PORT_DISABLED);
    }
    hprt0_dup |= (1UL << 3);
  }
  
  if(hprt0 & (1UL << 5))
  {
    hprt0_dup |= (1UL << 5);
  }
  HWREG32(u32Base + USB_OTG_HPRT) = hprt0_dup;
}

/*********************************************************************************************************
** Function name:       usb_port_reset
** Descriptions:        usb host复位
** input parameters:    host:   USB主机特征结构     
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void usb_port_reset(stm32_usb_host_t* host)
{
  uint32_t u32Base = host->base;
  volatile uint32_t hprt0;
  
  hprt0 = HWREG32(u32Base + USB_OTG_HPRT);
  hprt0 &= ~((1UL << 2) | (1UL << 1) | (1UL << 3) | (1UL << 5));
  HWREG32(u32Base + USB_OTG_HPRT) = ((1UL << 8) | hprt0); 
  rt_thread_mdelay(100);
  HWREG32(u32Base + USB_OTG_HPRT) = ((~(1UL << 8)) & hprt0); 
  rt_thread_mdelay(10);
}

/*********************************************************************************************************
** Function name:       usb_otg_host_rx_handler
** Descriptions:        usb host接收数据中断服务处理函数
** input parameters:    host:   USB主机特征结构     
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void usb_otg_host_rx_handler(stm32_usb_host_t* host)
{
  uint32_t u32Base = host->base;
  uint32_t sts = HWREG32(u32Base + USB_OTG_GRXSTSP);
  uint8_t ch_num = sts & 0x0FU;
  uint32_t len = (sts & (0x7FFU << 4)) >> 4;
  
  if(((sts & (0x0FU << 17)) >> 17) ==  2U){
    stm32_usb_host_channel_t* hc = &host->hc[ch_num];
    if(hc->ch_buf && hc->count < hc->size){
      usb_otg_data_read(u32Base, hc->ch_buf + hc->count, len);
      hc->count += len;
    }else{
      usb_otg_data_read(u32Base, 0, len);
    }
    if(HWREG32(u32Base + USB_OTG_HCTSIZ0 + 0x20UL * ch_num) & (0x3FFU << 19)){
      uint32_t tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num);
      tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
      tmpreg |= USB_OTG_HCCHAR_CHENA;
      HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) = tmpreg;
      //hc->toggle_in ^= 1;
      hc->toggle = ((HWREG32(u32Base + USB_OTG_HCTSIZ0 + 0x20UL * ch_num) >> 29) & 0x03UL);
    }
  }
}

/*********************************************************************************************************
** Function name:       usb_otg_channel_halt
** Descriptions:        usb host停止一个cahhnel
** input parameters:    u32Base:    USB在CPU总线的基地址 
**                      ch_num:     待停止的channel号
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void usb_otg_channel_halt(uint32_t u32Base, uint8_t ch_num)
{
  uint32_t  hc_char;
  uint32_t  ep_type;
  uint32_t tx_q_space = 0;
  
  hc_char = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num);
  hc_char |= (USB_OTG_HCCHAR_CHDIS | USB_OTG_HCCHAR_CHENA);
  ep_type = (hc_char & USB_OTG_HCCHAR_EPTYP) >> 18u;
  /* Check for space in the request queue to issue the halt. */
  if ((ep_type == EP_TYPE_CTRL) || (ep_type == EP_TYPE_BULK)) {
    tx_q_space = HWREG32(u32Base + USB_OTG_GNPTXSTS) & (0xFFU << 16);
  }else{
    tx_q_space = HWREG32(u32Base + USB_OTG_HPTXSTS) & (0xFFU << 16);
  }
  
  if(tx_q_space == 0) {
    hc_char &= ~USB_OTG_HCCHAR_CHENA;
  }
  
  HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20UL * ch_num) |= USB_OTG_HCINTMSK_CHHM;
  HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) = hc_char;
  
  
//  uint32_t HcEpType = (HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) & USB_OTG_HCCHAR_EPTYP) >> 18;
//  uint32_t tx_q_space = 0;
//  uint32_t dir = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) & USB_OTG_HCCHAR_EPDIR;
//  
//  /* Check for space in the request queue to issue the halt. */
//  if ((HcEpType == EP_TYPE_CTRL) || (HcEpType == EP_TYPE_BULK)) {
//    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) |= USB_OTG_HCCHAR_CHDIS;
//    tx_q_space = HWREG32(u32Base + USB_OTG_GNPTXSTS) & (0xFFU << 16);
//  }else{
//    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) |= USB_OTG_HCCHAR_CHDIS;
//    tx_q_space = HWREG32(u32Base + USB_OTG_HPTXSTS) & (0xFFU << 16);
//  }
//  if(tx_q_space == 0){
//    uint32_t count = 0U;
//    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) &= ~USB_OTG_HCCHAR_CHENA;
//    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) |= USB_OTG_HCCHAR_CHENA;
//    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) &= ~USB_OTG_HCCHAR_EPDIR;
//    do {
//      if (++count > 1000U) {
//        break;
//      }
//    } while ((HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
//  }else{
//    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) |= USB_OTG_HCCHAR_CHENA;
//  }
//  
//  HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) |= dir;
}

/*********************************************************************************************************
** Function name:       usb_otg_in_channel_handler
** Descriptions:        usb host in channel中断处理函数
** input parameters:    host:   USB主机特征结构   
**                      ch_num: 指定处理channel号
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
#define  USB_OTG_CLEAR_HC_INT(base, chnum,interrupt)    (HWREG32(base + USB_OTG_HCINT0 + 0x20UL * chnum) = (interrupt))
#define  USB_OTG_UNMASK_HALT_HC_INT(base,chnum)         (HWREG32(base + USB_OTG_HCINTMSK0 + 0x20UL * chnum) |= (USB_OTG_HCINT_CHH))
#define  USB_OTG_MASK_HALT_HC_INT(base,chnum)           (HWREG32(base + USB_OTG_HCINTMSK0 + 0x20UL * chnum) &= ~(USB_OTG_HCINT_CHH))

static void usb_otg_in_channel_handler(stm32_usb_host_t* host, uint8_t ch_num)
{
  uint32_t u32Base = host->base;
  uint32_t u32Channel = u32Base + 0x20UL * ch_num;
  stm32_usb_host_channel_t* hc = &host->hc[ch_num];
  uint32_t tmpreg;
  
  if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_AHBERR) == USB_OTG_HCINT_AHBERR)
  {
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_AHBERR);
    USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
  }
  else if((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_BBERR) == USB_OTG_HCINT_BBERR)
  {
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_BBERR);
    USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
  }
  else if((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_ACK) == USB_OTG_HCINT_ACK)
  {
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_ACK);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_STALL) == USB_OTG_HCINT_STALL)
  {
    USB_OTG_UNMASK_HALT_HC_INT(u32Base,ch_num);
    hc->state = USB_CS_STALL;
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_NAK);
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_STALL);
    usb_otg_channel_halt(u32Base, ch_num);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_DTERR) == USB_OTG_HCINT_DTERR)
  {
    USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
    usb_otg_channel_halt(u32Base, ch_num);
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_NAK);
    hc->state = USB_CS_DT_ERROR;
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_DTERR);
  }
  else
  {
    /* ... */
  }

  if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_FRMOR) == USB_OTG_HCINT_FRMOR)
  {
    USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
    usb_otg_channel_halt(u32Base, ch_num);
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_FRMOR);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_XFRC) == USB_OTG_HCINT_XFRC)
  {
    uint32_t HcEpType = (HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) & USB_OTG_HCCHAR_EPTYP) >> 18;
    (void)HcEpType;
    if (HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN){
      uint32_t mps = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) & (0x7FFUL << 0);
      uint32_t pkt_cnt = (hc->size + mps - 1) / mps;
      uint32_t remain =  HWREG32(u32Base + USB_OTG_HCTSIZ0 + 0x20UL * ch_num) & (0x7FFFFUL << 0);
      hc->count = mps*pkt_cnt - remain;
    }

    hc->state = USB_CS_TRANSFER_COMPLETE;
    hc->error_count = 0U;
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_XFRC);
    
    USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
    usb_otg_channel_halt(u32Base, ch_num);
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_NAK);    
    //hc->toggle_in ^= 1U;
    hc->toggle = ((HWREG32(u32Channel + USB_OTG_HCTSIZ0) >> 29) & 0x03UL);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_CHH) == USB_OTG_HCINT_CHH)
  {
    USB_OTG_MASK_HALT_HC_INT(u32Base, ch_num);
    if(hc->is_cancel){
      hc->xfer_done = 1;
      hc->is_cancel = 0;
      hc->state = USB_CS_XFER_CANCEL;
    }else if(hc->state == USB_CS_TRANSFER_COMPLETE)
    {
      hc->xfer_done = 1;
    }
    else if (hc->state == USB_CS_STALL)
    {
      hc->xfer_done = 1;
    }
    else if(hc->state == USB_CS_TRANSACTION_ERROR)
    {
      hc->error_count++;
      if(hc->error_count > MAX_ERROR_RETRY_TIME){
        hc->xfer_done = 1;
      }else{
        /* re-activate the channel  */
        tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num);
        tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
        tmpreg |= USB_OTG_HCCHAR_CHENA;
        HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) = tmpreg;
      }
    }
    else if (hc->state == USB_CS_DT_ERROR)
    {
        hc->xfer_done = 1;
    }
    else if (hc->state == USB_CS_NAK)
    {
      /* re-activate the channel  */
      hc->nak_count++;
      hc->error_count = 0;
//      if(hc->nak_count > MAX_NAK_RETRY_TIME){
//        hc->xfer_done = 1;
//      }else{
        if (HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN){
          tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num);
          tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
          tmpreg |= USB_OTG_HCCHAR_CHENA;
          HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) = tmpreg;
        }else{
          //usb_otg_host_submit(host, ch_num);
          tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num);
          tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
          tmpreg |= USB_OTG_HCCHAR_CHENA;
          HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) = tmpreg;
        }
//      }
    }
    else
    {
      /* ... */
      if (HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN){
        if(host->state == USB_HOST_PORT_ENABLED) {
          usb_otg_host_submit(host, ch_num);
          //        USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
          //        tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num);
          //        tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
          //        tmpreg |= USB_OTG_HCCHAR_CHENA;
          //        HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) = tmpreg;
        }
      }
    }
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_CHH);
    usb_host_notify_urb_finish_callback(host, ch_num);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_TXERR) == USB_OTG_HCINT_TXERR)
  {
    USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
    hc->state = USB_CS_TRANSACTION_ERROR;
    usb_otg_channel_halt(u32Base, ch_num);
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_TXERR);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_NAK) == USB_OTG_HCINT_NAK)
  {
    uint32_t HcEpType = (HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) & USB_OTG_HCCHAR_EPTYP) >> 18;
    if(HcEpType == EP_TYPE_INTR)
    {
      hc->xfer_done = 1;
      hc->state = USB_CS_INT_NAK;
      USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
      usb_otg_channel_halt(u32Base, ch_num);
    }
    else if ((HcEpType == EP_TYPE_CTRL)|| (HcEpType == EP_TYPE_BULK))
    {
       hc->error_count = 0U;
       if ((HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN) == 0)
       {
         hc->state = USB_CS_NAK;
         USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
         usb_otg_channel_halt(u32Base, ch_num);
       }
    }
    else
    {
      /* ... */
    }
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_NAK);
  }
  else
  {
    /* ... */
  }
}

/*********************************************************************************************************
** Function name:       usb_otg_out_channel_handler
** Descriptions:        usb host out channel中断处理函数
** input parameters:    host:    USB主机特征结构   
**                      ch_num:  指定处理channel号
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void usb_otg_out_channel_handler(stm32_usb_host_t* host, uint8_t ch_num)
{
  uint32_t u32Base = host->base;
  uint32_t u32Channel = u32Base + 0x20UL * ch_num;
  stm32_usb_host_channel_t* hc = &host->hc[ch_num];
  uint32_t tmpreg;

  if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_AHBERR) == USB_OTG_HCINT_AHBERR)
  {
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_AHBERR);
    USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_ACK) == USB_OTG_HCINT_ACK)
  {
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_ACK);
    if( hc->do_ping == 1U)
    {
      hc->state = USB_CS_ACK;
      hc->do_ping = 0U;
      USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
      usb_otg_channel_halt(u32Base, ch_num);
    }
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_NYET) == USB_OTG_HCINT_NYET)
  {
    hc->state = USB_CS_NYET;
    USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
    usb_otg_channel_halt(u32Base, ch_num);
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_NYET);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_FRMOR) == USB_OTG_HCINT_FRMOR)
  {
    USB_OTG_UNMASK_HALT_HC_INT(u32Base,ch_num);
    usb_otg_channel_halt(u32Base, ch_num);
    USB_OTG_CLEAR_HC_INT(u32Base,ch_num, USB_OTG_HCINT_FRMOR);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_XFRC) == USB_OTG_HCINT_XFRC)
  {
    USB_OTG_UNMASK_HALT_HC_INT(u32Base,ch_num);
    usb_otg_channel_halt(u32Base, ch_num);
    USB_OTG_CLEAR_HC_INT(u32Base,ch_num, USB_OTG_HCINT_XFRC);
    hc->state = USB_CS_TRANSFER_COMPLETE;
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_STALL) == USB_OTG_HCINT_STALL)
  {
    hc->state = USB_CS_STALL;
    USB_OTG_CLEAR_HC_INT(u32Base,ch_num, USB_OTG_HCINT_STALL);
    USB_OTG_UNMASK_HALT_HC_INT(u32Base,ch_num);
    usb_otg_channel_halt(u32Base, ch_num);
    
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_NAK) == USB_OTG_HCINT_NAK)
  {
    if (!(HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN)){
      hc->error_count = 0U;
      hc->state = USB_CS_NAK;
      if (hc->do_ping == 0){
        if (hc->speed == USB_OTG_SPEED_HIGH){
//          if (!(HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN)){
            hc->do_ping = 1;
//          }
        }
      }
      USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
      usb_otg_channel_halt(u32Base, ch_num);
    }
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_NAK);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_TXERR) == USB_OTG_HCINT_TXERR)
  {
    USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
    usb_otg_channel_halt(u32Base, ch_num);
    hc->state = USB_CS_TRANSACTION_ERROR;
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_TXERR);
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_DTERR) == USB_OTG_HCINT_DTERR)
  {
    USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
    usb_otg_channel_halt(u32Base, ch_num);
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_NAK);
    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_DTERR);
    hc->state = USB_CS_DT_ERROR;
  }
  else if ((HWREG32(u32Channel + USB_OTG_HCINT0) & USB_OTG_HCINT_CHH) == USB_OTG_HCINT_CHH)
  {
    USB_OTG_MASK_HALT_HC_INT(u32Base, ch_num);
    if(hc->is_cancel){
      hc->xfer_done = 1;
      hc->state = USB_CS_XFER_CANCEL;
    }else if (hc->state == USB_CS_XFER_ONGOING){
      hc->state = USB_CS_INIT;
      usb_otg_host_submit(host, ch_num);
    }else if (hc->state == USB_CS_TRANSFER_COMPLETE){
      uint32_t HcEpType = (HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) & USB_OTG_HCCHAR_EPTYP) >> 18;
      uint32_t len = (HWREG32(u32Base + USB_OTG_HCTSIZ0 + 0x20UL * ch_num) & (0x7FFFFUL << 0));
      if(HcEpType == EP_TYPE_BULK || HcEpType == EP_TYPE_INTR){
        //hc->toggle_out ^= 1U;
        hc->toggle = ((HWREG32(u32Channel + USB_OTG_HCTSIZ0) >> 29) & 0x03UL);
      }
      hc->error_count = 0;
      hc->nak_count = 0;
      if (HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN){
        hc->count = hc->size;
        hc->xfer_done = 1;
      } else {
        hc->count = hc->count + len;
        if(hc->count>= hc->size){
          hc->xfer_done = 1;
        }else{
          hc->state = USB_CS_XFER_ONGOING;
          USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
          usb_otg_channel_halt(u32Base, ch_num);
          //tusb_otg_host_submit(host, ch_num);
        }
      }
    }
    else if (hc->state == USB_CS_NAK)
    {
      hc->nak_count++;
      hc->error_count = 0;
//      if(hc->nak_count > MAX_NAK_RETRY_TIME){
//        hc->xfer_done = 1;
//      }else{
        hc->state = USB_CS_INIT;
        //USB_OTG_UNMASK_HALT_HC_INT(ch_num);
        //usb_otg_channel_halt(u32Base, ch_num);
        usb_otg_host_submit(host, ch_num);
        /*
        if (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN){
          tmpreg = HC->HCCHAR;
          tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
          tmpreg |= USB_OTG_HCCHAR_CHENA;
          HC->HCCHAR = tmpreg;
        }else{
          tusb_otg_host_submit(host, ch_num);
        }
        */
//        tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num);
//        tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
//        tmpreg |= USB_OTG_HCCHAR_CHENA;
//        HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) = tmpreg;
//      }
    }
    else if (hc->state == USB_CS_ACK)
    {
      if(hc->do_ping){
        hc->do_ping = 0;
        hc->state = USB_CS_XFER_ONGOING;
        USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
        usb_otg_channel_halt(u32Base, ch_num);
      } else {
        if (HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN){
          if(host->state == USB_HOST_PORT_ENABLED) {
            USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
            tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num);
            tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
            tmpreg |= USB_OTG_HCCHAR_CHENA;
            HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) = tmpreg;
          }
        }
      }
    }
    else if (hc->state == USB_CS_NYET)
    {
      // not yet, do ping
      hc->do_ping = 1;
      hc->state = USB_CS_XFER_ONGOING;
      USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
      usb_otg_channel_halt(u32Base, ch_num);
    }
    else if (hc->state == USB_CS_STALL)
    {
      hc->xfer_done = 1;
    }
    else if (hc->state == USB_CS_TRANSACTION_ERROR)
    {
      hc->error_count++;
      if(hc->error_count > MAX_ERROR_RETRY_TIME){
        hc->xfer_done = 1;
      }else{
        /* re-activate the channel  */
          tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num);
          tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
          tmpreg |= USB_OTG_HCCHAR_CHENA;
          HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) = tmpreg;
      }
    }
    else if (hc->state == USB_CS_DT_ERROR)
    {
        hc->xfer_done = 1;
    }
    else
    {
      /* ... */
      if (HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN){
        if(host->state == USB_HOST_PORT_ENABLED) {
          usb_otg_host_submit(host, ch_num);
//          USB_OTG_UNMASK_HALT_HC_INT(u32Base, ch_num);
//          tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num);
//          tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
//          tmpreg |= USB_OTG_HCCHAR_CHENA;
//          HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) = tmpreg;
        }
      }
    }

    USB_OTG_CLEAR_HC_INT(u32Base, ch_num, USB_OTG_HCINT_CHH);
    usb_host_notify_urb_finish_callback(host, ch_num);
  }
  else
  {
     /* ... */
  }
}

/*********************************************************************************************************
** Function name:       usb_otg_host_handler
** Descriptions:        usb host channel中断处理函数
** input parameters:    host:    USB主机特征结构   
**                      ch_num:  指定处理channel号
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_inline uint32_t usb_host_interrupt(uint32_t u32Base)
{
  uint32_t v;
  
  v = HWREG32(u32Base + USB_OTG_GINTSTS);
  v &= HWREG32(u32Base + USB_OTG_GINTMSK);
  
  return v;  
}
#define  USB_HOST_INTR()   usb_host_interrupt(u32Base)
void usb_otg_host_handler(stm32_usb_host_t* host)
{
  uint32_t u32Base = host->base;
  
  if(USB_HOST_INTR() == 0){
    return;
  }
  if(USB_HOST_INTR() & USB_OTG_GINTSTS_PXFR_INCOMPISOOUT ){
    HWREG32(u32Base + USB_OTG_GINTSTS) = USB_OTG_GINTSTS_PXFR_INCOMPISOOUT;
  }
  if(USB_HOST_INTR() & USB_OTG_GINTSTS_IISOIXFR){
    HWREG32(u32Base + USB_OTG_GINTSTS) = USB_OTG_GINTSTS_IISOIXFR;
  }
  if(USB_HOST_INTR() & USB_OTG_GINTSTS_PTXFE){
    uint32_t mask = host->ptx_pending;
    uint8_t ch_num = 0;
    HWREG32(u32Base + USB_OTG_GINTSTS) = USB_OTG_GINTSTS_PTXFE;
    while(mask){
      if(mask & 1){
        usb_otg_data_send(host, ch_num);
        break;
      }
      ch_num++;
      mask>>=1;
    }
    if(!host->ptx_pending){
      HWREG32(u32Base + USB_OTG_GINTMSK) &= ~USB_OTG_GINTMSK_PTXFEM;
    }
    
  }
  if(USB_HOST_INTR() & USB_OTG_GINTSTS_NPTXFE){
    uint32_t mask = host->nptx_pending;
    uint8_t ch_num = 0;
    HWREG32(u32Base + USB_OTG_GINTSTS) = USB_OTG_GINTSTS_NPTXFE;
    while(mask){
      if(mask & 1){
        usb_otg_data_send(host, ch_num);
        break;
      }
      ch_num++;
      mask>>=1;
    }
    if(!host->nptx_pending){
      HWREG32(u32Base + USB_OTG_GINTMSK) &= ~USB_OTG_GINTMSK_NPTXFEM;
    }
  }
  if(USB_HOST_INTR() & USB_OTG_GINTSTS_MMIS){
    HWREG32(u32Base + USB_OTG_GINTSTS) = USB_OTG_GINTSTS_MMIS;
  }
  /* Handle Host Disconnect Interrupts */
  if(USB_HOST_INTR() & USB_OTG_GINTSTS_DISCINT){
    /* Cleanup HPRT */
    HWREG32(u32Base + USB_OTG_HPRT) &= ~((1UL << 2) | (1UL << 1) | (1UL << 3) | (1UL << 5));
    /* Handle Host Port Interrupts */
    usb_otg_host_disconnect(host);
    usb_otg_phy_clock_set(u32Base, HCFG_48_MHZ);
    
    HWREG32(u32Base + USB_OTG_GINTSTS) = USB_OTG_GINTSTS_DISCINT;
  }
  /* Handle Host Port Interrupts */
  if( USB_HOST_INTR() & USB_OTG_GINTSTS_HPRTINT ){
    usb_host_port_handler(host);
  }
  /* Handle Host SOF Interrupts */
  if( USB_HOST_INTR() & USB_OTG_GINTSTS_SOF ){
      HWREG32(u32Base + USB_OTG_GINTSTS) = USB_OTG_GINTSTS_SOF;
      usb_host_sof_callback(host);
  }
  /* Handle Host channel Interrupts */
  if(USB_HOST_INTR() & USB_OTG_GINTSTS_HCINT ){
    uint32_t hc_intr = (HWREG32(u32Base + USB_OTG_HAINT) & 0xFFFFU);
    uint32_t ch_num = 0;
    while(hc_intr){
      if(hc_intr & 1){
        if(HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) & USB_OTG_HCCHAR_EPDIR){
          usb_otg_in_channel_handler(host, ch_num);
        }else{
          usb_otg_out_channel_handler(host, ch_num);
        }
        //tusb_otg_host_channel_handler(host, ch_num);
      }
      ch_num++;
      hc_intr>>=1;
      if(ch_num > host->channel_num) break;
    }
    HWREG32(u32Base + USB_OTG_GINTSTS) = USB_OTG_GINTSTS_HCINT;
  }
  /* Handle Rx Queue Level Interrupts */
  if( USB_HOST_INTR() & USB_OTG_GINTSTS_RXFLVL ){
    HWREG32(u32Base + USB_OTG_GINTMSK) &= ~(1UL << USB_OTG_GINTSTS_RXFLVL);
    usb_otg_host_rx_handler(host);
    HWREG32(u32Base + USB_OTG_GINTMSK) |= (1UL << USB_OTG_GINTSTS_RXFLVL);
    
  }
}

/*********************************************************************************************************
** Function name:       usb_host_channel_init
** Descriptions:        usb host初始化一个channel
** input parameters:    host:    USB主机特征结构   
**                      ch_num:  指定处理channel号
**                      dev_addr：设备地址
**                      ep_addr： 设备端点地址
**                      ep_type： 断点类型
**                      mps：     最大包长度
**                      speed：   channel速度
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void usb_host_channel_init(stm32_usb_host_t* host, uint8_t ch_num, uint8_t dev_addr, 
                           uint8_t ep_addr, uint8_t ep_type, uint16_t mps, uint8_t speed)
{
  uint32_t u32Base = host->base;
  uint32_t u32Channel = u32Base + 0x20UL * ch_num;
  stm32_usb_host_channel_t* hc = &host->hc[ch_num];

  // clear old interrupt flags
  HWREG32(u32Channel + USB_OTG_HCINT0) = 0xFFFFFFFFU;
  switch(ep_type){
    case EP_TYPE_CTRL:
    case EP_TYPE_BULK:
      HWREG32(u32Channel + USB_OTG_HCINTMSK0) = USB_OTG_HCINTMSK_XFRCM  |
                     USB_OTG_HCINTMSK_STALLM |
                     USB_OTG_HCINTMSK_TXERRM |
                     USB_OTG_HCINTMSK_DTERRM |
                     USB_OTG_HCINTMSK_AHBERR |
                     USB_OTG_HCINTMSK_NAKM;
      if (ep_addr & 0x80){
        HWREG32(u32Channel + USB_OTG_HCINTMSK0) |= USB_OTG_HCINTMSK_BBERRM;
      }else{
       // if(u32Base != USB2_OTG_FS_BASE){
        if(host->phy_speed == USB_OTG_SPEED_HIGH){
          HWREG32(u32Channel + USB_OTG_HCINTMSK0) |= (USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
        }
      }
      break;
    case EP_TYPE_INTR:
      HWREG32(u32Channel + USB_OTG_HCINTMSK0) = USB_OTG_HCINTMSK_XFRCM  |
                     USB_OTG_HCINTMSK_STALLM |
                     USB_OTG_HCINTMSK_TXERRM |
                     USB_OTG_HCINTMSK_DTERRM |
                     USB_OTG_HCINTMSK_NAKM   |
                     USB_OTG_HCINTMSK_AHBERR |
                     USB_OTG_HCINTMSK_FRMORM;

    if (ep_addr & 0x80){
      HWREG32(u32Channel + USB_OTG_HCINTMSK0) |= USB_OTG_HCINTMSK_BBERRM;
    }
    break;
    case EP_TYPE_ISOC:
      HWREG32(u32Channel + USB_OTG_HCINTMSK0) = USB_OTG_HCINTMSK_XFRCM  |
                     USB_OTG_HCINTMSK_ACKM   |
                     USB_OTG_HCINTMSK_AHBERR |
                     USB_OTG_HCINTMSK_FRMORM;
    if ( ep_addr & 0x80U ){
      HWREG32(u32Channel + USB_OTG_HCINTMSK0) |= (USB_OTG_HCINTMSK_TXERRM | USB_OTG_HCINTMSK_BBERRM);
    }
    break;
  }
  
  HWREG32(u32Base + USB_OTG_HAINTMSK) |= 1UL << (ch_num & 0xFU);
  /* Make sure host channel interrupts are enabled. */
  HWREG32(u32Base + USB_OTG_GINTMSK) |= USB_OTG_GINTMSK_HCIM;
  HWREG32(u32Channel + USB_OTG_HCTSIZ0) = 0;
  HWREG32(u32Channel + USB_OTG_HCCHAR0) = ( ((uint32_t)dev_addr << 22 ) |
                                           ((uint32_t)(ep_addr&0x7f) << 11 ) | 
                                           ((uint32_t)ep_type  << 18 ) |
                                          ((uint32_t)mps & (0x7FFUL << 0)));
  
                    
  if(ep_addr & 0x80){
    HWREG32(u32Channel + USB_OTG_HCCHAR0) |= USB_OTG_HCCHAR_EPDIR;
  }
  if( speed == USB_OTG_SPEED_LOW){
    HWREG32(u32Channel + USB_OTG_HCCHAR0) |= (1UL << 17);
  }
  
  if (ep_type == EP_TYPE_INTR){
    HWREG32(u32Channel + USB_OTG_HCCHAR0) |= (1UL << 29) ;
  }
  
  memset(hc, 0, sizeof(*hc));
  hc->speed = speed;
  hc->is_use = 1;
}

/*********************************************************************************************************
** Function name:       usb_port_get_speed
** Descriptions:        usb host速度读取
** input parameters:    host:    USB主机特征结构   
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
uint8_t usb_port_get_speed(stm32_usb_host_t* host)
{
  uint32_t u32Base = host->base;
  uint32_t u32Temp;
  
  u32Temp = (HWREG32(u32Base + USB_OTG_HPRT) >> 17) & 0x03U;
  switch(u32Temp)
  {
  case 0:
    return USB_OTG_SPEED_HIGH;
  case 1:
    return USB_OTG_SPEED_FULL;
  case 2:
    return USB_OTG_SPEED_LOW;
  default:
    return USB_OTG_SPEED_FULL;
  }
}

/*********************************************************************************************************
** Function name:       usb_host_channel_deinit
** Descriptions:        usb host channel反初始化
** input parameters:    host:    USB主机特征结构   
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void usb_host_channel_deinit(stm32_usb_host_t* host, uint8_t ch_num)
{
  uint32_t u32Base = host->base;
  uint32_t u32Channel = u32Base + 0x20UL * ch_num;
  stm32_usb_host_channel_t* hc = &host->hc[ch_num];
  
  // close all interrupt mask
  HWREG32(u32Channel + USB_OTG_HCINTMSK0) = 0;
  HWREG32(u32Channel + USB_OTG_HCCHAR0) = 0;
  memset(hc, 0, sizeof(*hc));
}

/*********************************************************************************************************
** Function name:       usb_otg_host_get_free_ch
** Descriptions:        usb host获取一个空闲的channel
** input parameters:    host:    USB主机特征结构   
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
uint8_t  usb_otg_host_get_free_ch(stm32_usb_host_t* host)
{
  uint32_t u32Base = host->base;
  uint32_t i;
  for(i=0;i<host->channel_num;i++){
    if(!host->hc[i].is_use){
      if( (HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i) & USB_OTG_HCCHAR_CHENA) == 0 ) {
        return i;
      }
    }
  }
  return 0xff;
}

/*********************************************************************************************************
** Function name:       usb_otg_data_send
** Descriptions:        usb host发送数据
** input parameters:    host:    USB主机特征结构   
**                      ch_num:  发送数据的channel
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void usb_otg_data_send(stm32_usb_host_t* host, uint8_t ch_num)
{
  uint32_t u32Base = host->base;
  uint32_t u32Channel = u32Base + 0x20UL * ch_num;
  uint32_t mps = HWREG32(u32Channel + USB_OTG_HCCHAR0) & (0x7FFUL);
  uint8_t xfer_type = (HWREG32(u32Channel + USB_OTG_HCCHAR0) >> 18) & 0x03UL;
  stm32_usb_host_channel_t* hc = &host->hc[ch_num];
  uint32_t i;
  uint32_t act_len;
  uint32_t req_size;
  uint32_t *pSrc = (uint32_t *)(hc->ch_buf + hc->count);
  uint32_t len = (HWREG32(u32Base + USB_OTG_HCTSIZ0 + 0x20UL * ch_num) & 0x7FFFFUL);
  // len = (len+3)/4;
  if(xfer_type == EP_TYPE_CTRL || xfer_type == EP_TYPE_BULK){
    act_len = HWREG32(u32Base + USB_OTG_GNPTXSTS);
  }else{
    act_len = HWREG32(u32Base + USB_OTG_HPTXSTS);
  }
  // calculate useful FIFO size, depend on two factor
  // 1. remain [request queue space] * [max packet size]
  req_size = ((act_len & 0xff0000) >> 16) * mps;
  // 2. remain FIFO buffer size, round to max packet size boundary
  act_len = ( (act_len & 0xffff) *4 / mps )  * mps;
  if(act_len > req_size) act_len = req_size;
  if(len > act_len){
    len = len - act_len;
  }else{
    act_len = len;
    len = 0;
  }
  act_len = (act_len+3)/4;
  for(i=0;i<act_len;i++){
    HWREG32(u32Base + USB_OTG_FIFO_BASE + (ch_num * USB_OTG_FIFO_SIZE)) = *((__packed rt_uint32_t *)pSrc);
    pSrc++;
  }
  if(len){
    if(xfer_type == EP_TYPE_CTRL || xfer_type == EP_TYPE_BULK){
      HWREG32(u32Base + USB_OTG_GINTMSK) |= USB_OTG_GINTMSK_NPTXFEM;
      host->nptx_pending |= (1<<ch_num);
    }else{
      HWREG32(u32Base + USB_OTG_GINTMSK) |= USB_OTG_GINTMSK_PTXFEM;
      host->ptx_pending |= (1<<ch_num);
    }
  }else{
    host->nptx_pending &= ~(1<<ch_num);
    host->ptx_pending &= ~(1<<ch_num);
  }
}

/*********************************************************************************************************
** Function name:       usb_otg_host_submit
** Descriptions:        usb host发送一次urb事物
** input parameters:    host:    USB主机特征结构   
**                      ch_num:  发送数据的channel
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
uint32_t usb_otg_host_submit(stm32_usb_host_t* host, uint8_t ch_num)
{
  uint32_t u32Base = host->base;
  uint32_t u32Channel = u32Base + 0x20UL * ch_num;
  stm32_usb_host_channel_t* hc = &host->hc[ch_num];
  uint32_t mps = HWREG32(u32Channel + USB_OTG_HCCHAR0) & (0x7FFUL);
  uint8_t xfer_type = (HWREG32(u32Channel + USB_OTG_HCCHAR0) >> 18) & 0x03UL;
  uint32_t pkt_cnt;
  uint32_t len = hc->size - hc->count;
  uint32_t data_pid = HC_PID_DATA1;
  uint8_t is_in = (HWREG32(u32Channel + USB_OTG_HCCHAR0) & USB_OTG_HCCHAR_EPDIR) != 0;
  uint8_t is_dma = (HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN) != 0;
  uint8_t is_data = hc->is_data;
  
  pkt_cnt = (len + mps - 1) / mps;
  if(!pkt_cnt) pkt_cnt = 1;
  if(is_in){
    len = pkt_cnt * mps;
  }else{
    if(!is_dma){
      if(len>mps){
        len = mps;
        pkt_cnt = 1;
      }
    }
  }
  len = (len & (0x7FFFFUL << 0));
  
  switch( xfer_type ){
    case EP_TYPE_CTRL:
      if(is_data){
        if(len == 0){
          if(is_in){
            // control in
          }else{
            // control out
            hc->toggle = 1;
            data_pid = HC_PID_DATA1;
          }
        }
      }else{
        // otherwise setup packet
        data_pid = HC_PID_SETUP;
      }
      break;
    case EP_TYPE_BULK:
    case EP_TYPE_INTR:
      data_pid = ((HWREG32(u32Channel + USB_OTG_HCTSIZ0) >> 29) & 0x03UL);
#if 0
      if(is_in){
        data_pid = hc->toggle_in ? HC_PID_DATA1 : HC_PID_DATA0;
      }else{
        data_pid = hc->toggle_out ? HC_PID_DATA1 : HC_PID_DATA0;
      }
#endif
      break;
    case EP_TYPE_ISOC:
      data_pid = HC_PID_DATA0;
      break;
  }
  
  if(hc->do_ping){
    HWREG32(u32Channel + USB_OTG_HCTSIZ0) = ((1 << 19) | (1UL << 31)) ;
  } else{
    HWREG32(u32Channel + USB_OTG_HCTSIZ0) = ((len << 0) | (pkt_cnt << 19) | (data_pid  << 29));
    
    if (HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN){
      HWREG32(u32Channel + USB_OTG_HCINTMSK0) |= USB_OTG_HCINTMSK_CHHM;
      /* xfer_buff MUST be 32-bits aligned */
      rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, (void *)hc->ch_buf, len);
      HWREG32(u32Channel + USB_OTG_HCDMA0) = (uint32_t)hc->ch_buf;
      if(((uint32_t)hc->ch_buf) & 0x03) {
        rt_kprintf("usb aligned error\r\n");
      }
    }
    
    if(HWREG32(u32Base + USB_OTG_HFNUM) & 0x01U){
      HWREG32(u32Channel + USB_OTG_HCCHAR0) &= ~USB_OTG_HCCHAR_ODDFRM;
    }else{
      HWREG32(u32Channel + USB_OTG_HCCHAR0) |= USB_OTG_HCCHAR_ODDFRM;
    }
  }
  {
    uint32_t tmpreg = HWREG32(u32Channel + USB_OTG_HCCHAR0);
    tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
    tmpreg |= USB_OTG_HCCHAR_CHENA;
    HWREG32(u32Channel + USB_OTG_HCCHAR0) = tmpreg;
  }
  
  if( (!is_dma) && (!is_in) && len && (!hc->do_ping) ){
    usb_otg_data_send(host, ch_num);
  }
  
  return 0;
}

/*********************************************************************************************************
** Function name:       usb_otg_host_xfer_data
** Descriptions:        usb host传输一次数据
** input parameters:    host:    USB主机特征结构   
**                      ch_num:  发送数据的channel
**                      is_data：1：传输数据；0：传输setup
**                      data:    传输数据存放地址
**                      len：    数据缓冲区大小
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
uint32_t usb_otg_host_xfer_data(stm32_usb_host_t* host, uint8_t ch_num, uint8_t is_data, uint8_t* data, uint32_t len)
{
  uint32_t u32Base = host->base;
  uint32_t u32Channel = u32Base + 0x20UL * ch_num;
  stm32_usb_host_channel_t* hc = &host->hc[ch_num];
//  uint32_t mps = HWREG32(u32Channel + USB_OTG_HCCHAR0) & (0x7FFUL);
//  uint8_t is_in = (HWREG32(u32Channel + USB_OTG_HCCHAR0) & USB_OTG_HCCHAR_EPDIR) != 0;
//  (void)is_in;
//  (void)mps;
  uint8_t is_dma = (HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN) != 0;

  hc->ch_buf = data;
  hc->size = len;
  hc->count = 0;
  hc->state = USB_CS_INIT;
  hc->do_ping = 0;
  hc->is_data = is_data;
  hc->xfer_done = 0;
  hc->error_count = 0;
  hc->nak_count = 0;
  
  if(((HWREG32(u32Channel + USB_OTG_HCCHAR0) >> 18) & 0x03UL) == EP_TYPE_BULK){
    if(is_dma){
      // DMA enabled, not ping
      HWREG32(u32Channel + USB_OTG_HCINTMSK0) &= ~(USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
    }else if (hc->speed == USB_OTG_SPEED_HIGH ) {
      // PORT speed is HIGH
      hc->do_ping = 1;
    }
  }
  rt_completion_init(&hc->urb_completion);
  usb_otg_host_submit(host, ch_num);
  return 0;
}

/*********************************************************************************************************
** Function name:       usb_host_xfer_wait
** Descriptions:        usb host传输等待
** input parameters:    host:    USB主机特征结构   
**                      ch_num:  发送数据的channel
**                      timeout：超时时间，以tick为单位
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int usb_host_xfer_wait(stm32_usb_host_t* host, uint8_t ch_num, int32_t timeout)
{
  if(rt_completion_wait(&host->hc[ch_num].urb_completion, timeout) != RT_EOK) {
    usb_otg_channel_halt(host->base, ch_num);
    return -1;
  }
  
  return 0;
}
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
