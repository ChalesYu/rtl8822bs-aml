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
#ifndef __STM32_USB_OTG_HOST_H__
#define __STM32_USB_OTG_HOST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
  
/*********************************************************************************************************
** 基本配置
*********************************************************************************************************/
// channel最大数量
#define  MAX_CHANNEL_NUM                    16
/** USB host channel max retry time when error occur */
#define MAX_ERROR_RETRY_TIME                16
/** USB host channel max retry time when get NAK, 
    this value is used to avoid infinite wait in Bulk/Control transfer */
#define MAX_NAK_RETRY_TIME                  0xFFFFFFF0
  
/*********************************************************************************************************
** USB特征结构定义
*********************************************************************************************************/
/** Type define for Host Channel Data */
typedef struct stm32_usb_host_channel {
  uint8_t*  ch_buf;                  /**< channel data buffer                                              */
  uint16_t  size;                    /**< total size of the channel buffer                                 */
  uint16_t  count;                   /**< current xfered data count of the channel buffer                  */
  uint32_t  nak_count;               /**< channel NAK count                                                */
  uint32_t  error_count;             /**< channel error count                                              */
  uint32_t  error_reason;            /**< channel error reason                                             */
  uint8_t   state;                   /**< channel state   \ref  channel_state_t                            */
  uint8_t   toggle:2;             /**< toggle bit for IN pipe                                           */
  //uint8_t   toggle_out:1;            /**< toggle bit for OUT pipe                                          */
  uint8_t   do_ping:1;               /**< do ping flag                                                     */
  uint8_t   is_use:1;                /**< used flag:  1-channel is used, 0-channel is free                 */
  uint8_t   is_data:1;               /**< Channel data type flag: 1-data packet, 0-setup packet            */
  uint8_t   is_cancel:1;             /**< Cancel current transfer                                          */
  uint8_t   xfer_done:1;             /**< 1: xfer done, 0: xfer on goning                                  */
  uint8_t   padding:1;               /**< 1 bit field padding                                              */
  uint8_t   speed;                   /**< port speed,                                                      */
  uint8_t   padding32;               /**< padding to 32bit boundary                                        */
  struct rt_completion urb_completion; /**< used for urb completetion                                      */
} stm32_usb_host_channel_t;

/** Type define for USB Host */
typedef struct stm32_usb_host {
  const uint32_t        base;                     /** cpu bus address for usb otg */
  const uint8_t         port;                     /** port in usb root hub */
  const uint8_t         phy_interface;            /** phy interface */
  const uint8_t         phy_speed;                /** phy speed     */
  const uint8_t         channel_num;              /** usb host channel num */
  
  uint32_t        state;                          /**< current host state */
  stm32_usb_host_channel_t  hc[MAX_CHANNEL_NUM];  /**< host channel array */
  
  uint32_t        nptx_pending;                   /**< Non periodic pending trasmit/out pipe */
  uint32_t        ptx_pending;                    /**< Periodic pending trasmit/out pipe     */
  void*           user_data;                      /**< User data for host */
} stm32_usb_host_t;

/*********************************************************************************************************
** USB主机状态状态机定义
*********************************************************************************************************/
typedef enum {
  USB_HOST_PORT_DUMMY = 0,     /**< Initial state */
  USB_HOST_PORT_DISCONNECTED,  /**< Host port is disconnected */
  USB_HOST_PORT_CONNECTED,     /**< Host port connected */
  USB_HOST_PORT_ENABLED,       /**< Host port enabled, this is done by fire a reset when in connected state */
  USB_HOST_PORT_DISABLED,      /**< Host port disabled */
}host_port_state_t;

/** Enums for \ref tusb_hc_data_t state field
 *  indicating the host channel state
 */
typedef enum {
  USB_CS_INIT = 0,                 /**< Initial state */
  USB_CS_TRANSFER_COMPLETE = 1,    /**< Channel transfer complete */
  USB_CS_NAK,                      /**< Channel naked */
  USB_CS_ACK,                      /**< Channel acked */
  SUB_CS_PING_SUCCESS,             /**< Channel ping acked */
  USB_CS_NYET,                     /**< Channel ping not yet */
  USB_CS_STALL,                    /**< Channel stalled */
  USB_CS_INT_NAK,                  /**< Interrupt channel naked */
  
  USB_CS_AHB_ERROR = 10,           /**< Channel AHB error */
  USB_CS_DT_ERROR,                 /**< Channel data toggle error */
  USB_CS_TRANSACTION_ERROR,        /**< Channel transaction error, CRC/Bit stuff/timeout/False EOP */
  USB_CS_FRAMEOVERRUN_ERROR,       /**< Channel frame overrun */
  USB_CS_BABBLE_ERROR,             /**< Channel babble error */
  
  USB_CS_XFER_ONGOING,             /**< Channel data transfer is on going */
  USB_CS_XFER_CANCEL,              /**< Channel data transfer is canceled */
  USB_CS_UNKNOWN_ERROR,            /**< Channel Unknown error */
}channel_state_t;
/*********************************************************************************************************
**  实现的外部函数声明
*********************************************************************************************************/
extern void usb_port_reset(stm32_usb_host_t* host);
extern void usb_otg_host_handler(stm32_usb_host_t* host);
extern void usb_host_channel_init(stm32_usb_host_t* host, uint8_t ch_num, uint8_t dev_addr, 
                           uint8_t ep_addr, uint8_t ep_type, uint16_t mps, uint8_t speed);
extern uint8_t usb_port_get_speed(stm32_usb_host_t* host);
extern void usb_host_channel_deinit(stm32_usb_host_t* host, uint8_t ch_num);
extern uint8_t  usb_otg_host_get_free_ch(stm32_usb_host_t* host);
extern uint32_t usb_otg_host_submit(stm32_usb_host_t* host, uint8_t ch_num);
extern uint32_t usb_otg_host_xfer_data(stm32_usb_host_t* host, uint8_t ch_num, uint8_t is_data, uint8_t* data, uint32_t len);
extern int usb_host_xfer_wait(stm32_usb_host_t* host, uint8_t ch_num, int32_t timeout);

#ifdef __cplusplus
    }
#endif      // __cplusplus
    
#endif      // __STM32_USB_OTG_HOST_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
