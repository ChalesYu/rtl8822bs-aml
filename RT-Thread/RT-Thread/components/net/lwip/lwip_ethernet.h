/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwip_ethernet.h
** Last modified Date:  2015-01-09
** Last Version:        v1.00
** Description:         lwip的以太网驱动应用框架
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-01-09
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#ifndef __LWIP_ETHERNET_H__
#define __LWIP_ETHERNET_H__

#ifdef __cplusplus
extern "C" {
#endif
  
#include <rtthread.h>
#include "lwip/netif.h"
  
  /*********************************************************************************************************
  **  基本配置
  *********************************************************************************************************/
#ifndef RT_LWIP_ETH_MTU
#define ETHERNET_MTU		1500
#else
#define ETHERNET_MTU		RT_LWIP_ETH_MTU
#endif
  
  /*********************************************************************************************************
  **  定义以太网驱动框架结构
  *********************************************************************************************************/
  struct eth_device
  {
    /* inherit from rt_device */
    struct rt_device parent;
    
    /* network interface for lwip */
    struct netif *netif;
    struct rt_semaphore tx_ack;
    
    rt_uint8_t  flags;
    rt_uint8_t  link_changed;
    rt_uint16_t link_status;
    
    /* eth device interface */
    struct pbuf* (*eth_rx)(rt_device_t dev);
    rt_err_t (*eth_tx)(rt_device_t dev, struct pbuf* p);
  };
  
  /*********************************************************************************************************
  **  外部函数声明
  *********************************************************************************************************/
  rt_err_t eth_device_ready(struct eth_device* dev);
  rt_err_t eth_device_init(struct eth_device * dev, char *name);
  void eth_device_deinit(struct eth_device *dev);
  rt_err_t eth_device_linkchange(struct eth_device* dev, rt_bool_t up);
  
  /*********************************************************************************************************
  **  以太网框架设备驱动初始化
  *********************************************************************************************************/
  int eth_system_device_init(void);
  
#ifdef __cplusplus
}
#endif      // __cplusplus

#endif      // __LWIP_ETHERNET_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
