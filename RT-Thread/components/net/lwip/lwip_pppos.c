/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwip_pppos.h
** Last modified Date:  2017-11-02
** Last Version:        v1.00
** Description:         lwip通过GPRS拨号的PPPOS应用
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2017-11-02
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/serial.h>
#include "netif/ppp/pppapi.h"
#include "netif/ppp/pppos.h"
#include "netif/ppp/pppoe.h"
#include "netif/ppp/pppol2tp.h"

#include <lwip_if.h>
#include <lwip/sockets.h>
#include <lwip/dns.h>

#include "lwip_pppos.h"

#include "board_led.h"

#ifdef RT_USE_PPPOS

/*********************************************************************************************************
**  拨号次数配置
*********************************************************************************************************/
#ifndef AT_TRY_NUM
#define AT_TRY_NUM               10
#endif

#ifndef PPP_TRY_NUM
#define PPP_TRY_NUM              3
#endif

/*********************************************************************************************************
** 调试输出宏定义
*********************************************************************************************************/
#ifndef PPPOS_DEBUG
#define PPPOS_DEBUG   1
#endif

#if (PPPOS_DEBUG > 0)
#define pppos_trace(fmt, ...)           rt_kprintf(fmt, ##__VA_ARGS__)
#else
#define pppos_trace(fmt, ...)
#endif

/*********************************************************************************************************
**  定义驱动结构
*********************************************************************************************************/
typedef struct {
  ppp_pcb *ppp;
  const char *tty_name;
  char apn_name[16];
  char user_name[32];
  char user_password[32];
  rt_device_t serial;
  rt_uint8_t  ppp_status;       //0：AT指令连接模式  1：PPP自协商模式   2：拨号成功，数据通信模式
  rt_uint16_t try_connect_counter;
  struct netif netif;
  rt_uint8_t buffer[1536];
  void(*power_ctrl)(void);      // 开机控制,如果多少次重新拨号不成功，可能modem死机，需要重启modem
  int(*modem_at_dial)(rt_device_t handle,uint8_t *buffer,const char *apn_name);
}pppos_device;

/*********************************************************************************************************
**  定义全局变量
*********************************************************************************************************/


/*********************************************************************************************************
** Function name:       pppos_open_tty
** Descriptions:        打开tty串口
** input parameters:    device:	ppp网络接口结构
** output parameters:   NONE
** Returned value:      打开的串口句柄
*********************************************************************************************************/
static rt_device_t pppos_open_tty(pppos_device *device)
{
  uint32_t count = 0;
  rt_device_t handle;
  
  while(1)
  {
    handle = rt_device_find(device->tty_name);
    if(handle != RT_NULL) {
      if(handle->ref_count == 0) {
        rt_device_open(handle,RT_DEVICE_OFLAG_RDWR);
      }
      return handle;
    }
    // 超过一定时间(3分钟)识别不到
    count++;
    if(count > 60 * 3) {
      count = 0;
      device->power_ctrl();
    }
    rt_thread_delay(RT_TICK_PER_SECOND);
  }
}

/*********************************************************************************************************
** Function name:       pppos_status_cb
** Descriptions:        PPP status callback
** input parameters:    netif:	网络接口标志
**                      p:      发送数据缓冲区
** output parameters:   无
** Returned value:      发送结果
*********************************************************************************************************/
static void pppos_status_cb(ppp_pcb *pcb, int err_code, void *ctx) 
{
  struct netif *pppif = ppp_netif(pcb);
  pppos_device *device = ctx;
  
  LWIP_UNUSED_ARG(ctx);
  
  pppif = pppif;

  switch(err_code) {
    case PPPERR_NONE: {
#if LWIP_DNS
      const ip_addr_t *ns;
#endif /* LWIP_DNS */
      pppos_trace("status_cb: Connected\n");
#if PPP_IPV4_SUPPORT
      pppos_trace("   our_ipaddr  = %s\n", ipaddr_ntoa(&pppif->ip_addr));
      pppos_trace("   his_ipaddr  = %s\n", ipaddr_ntoa(&pppif->gw));
      pppos_trace("   netmask     = %s\n", ipaddr_ntoa(&pppif->netmask));
#if LWIP_DNS
      ns = dns_getserver(0);
      ns = ns;
      pppos_trace("   dns1        = %s\n", ipaddr_ntoa(ns));
      ns = dns_getserver(1);
      pppos_trace("   dns2        = %s\n", ipaddr_ntoa(ns));
#endif /* LWIP_DNS */
#endif /* PPP_IPV4_SUPPORT */
#if PPP_IPV6_SUPPORT
      pppos_trace("   our6_ipaddr = %s\n", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */
      break;
    }
    case PPPERR_PARAM: {
      pppos_trace("status_cb: Invalid parameter\n");
      break;
    }
    case PPPERR_OPEN: {
      pppos_trace("status_cb: Unable to open PPP session\n");
      break;
    }
    case PPPERR_DEVICE: {
      pppos_trace("status_cb: Invalid I/O device for PPP\n");
      break;
    }
    case PPPERR_ALLOC: {
      pppos_trace("status_cb: Unable to allocate resources\n");
      break;
    }
    case PPPERR_USER: {
      pppos_trace("status_cb: User interrupt\n");
      break;
    }
    case PPPERR_CONNECT: {
      pppos_trace("status_cb: Connection lost\n");
      break;
    }
    case PPPERR_AUTHFAIL: {
      pppos_trace("status_cb: Failed authentication challenge\n");
      break;
    }
    case PPPERR_PROTOCOL: {
      pppos_trace("status_cb: Failed to meet protocol\n");
      break;
    }
    case PPPERR_PEERDEAD: {
      pppos_trace("status_cb: Connection timeout\n");
      break;
    }
    case PPPERR_IDLETIMEOUT: {
      pppos_trace("status_cb: Idle Timeout\n");
      break;
    }
    case PPPERR_CONNECTTIME: {
      pppos_trace("status_cb: Max connect time reached\n");
      break;
    }
    case PPPERR_LOOPBACK: {
      pppos_trace("status_cb: Loopback detected\n");
      break;
    }
    default: {
      pppos_trace("status_cb: Unknown error code %d\n", err_code);
      break;
    }
  }

/*
 * This should be in the switch case, this is put outside of the switch
 * case for example readability.
 */

  if (err_code == PPPERR_NONE) {
    device->try_connect_counter = 0;
#ifdef ppp_net_on
    ppp_net_on();
#endif
    return;
  }

  /* ppp_close() was previously called, don't reconnect */
  if (err_code == PPPERR_USER) {
    /* ppp_free(); -- can be called here */
    return;
  }

  /*
   * Try to reconnect in 30 seconds, if you need a modem chatscript you have
   * to do a much better signaling here ;-)
   */
#ifdef ppp_net_off
    ppp_net_off();
#endif
  pppapi_connect(pcb, 30);
  device->try_connect_counter++;
  if(device->try_connect_counter >= PPP_TRY_NUM) {
    // 复位modem
    pppapi_close(pcb,0);
    device->ppp_status = 0;
  }
  /* OR ppp_listen(pcb); */
}

/*********************************************************************************************************
** Function name:       pppos_output_cb
** Descriptions:        ppp发送数据函数
** input parameters:    pcb:	ppp接口标志
**                      data:   发送数据缓冲区
**                      len：   发送数据长度 
**                      ctx：   传递上下文
** output parameters:   无
** Returned value:      实际发送数据长度
*********************************************************************************************************/
static u32_t pppos_output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx) 
{
  pppos_device *device = ctx;
  
  // 如果设备不存在，被注销了
  if(rt_object_get_type(&device->serial->parent) != RT_Object_Class_Device) {
    return 0;
  }
  
  return rt_device_write(device->serial, 0, data, len);
}

/*********************************************************************************************************
** Function name:       pppos_rx_thread
** Descriptions:        ppp接收数据线程
** input parameters:    netif:	网络接口标志
**                      p:      发送数据缓冲区
** output parameters:   无
** Returned value:      发送结果
*********************************************************************************************************/
static void pppos_rx_thread(void *parg)
{
  rt_uint32_t len;
  rt_uint32_t at_try_count;
  int status;
  pppos_device *device = parg;
  rt_device_t handle;

  /*
  ** Step 1, 一个PPP网络建立，开始启动PPP协议栈
  */
  device->ppp_status = 0;
  device->try_connect_counter = 0;
  device->ppp = pppapi_pppos_create(&device->netif, pppos_output_cb, pppos_status_cb, device);
#if (PPP_NETIF_DEFAULT > 0)
  pppapi_set_default(device->ppp);
#endif
  /* Auth configuration, this is pretty self-explanatory */
  ppp_set_auth(device->ppp, PPPAUTHTYPE_ANY, device->user_name, device->user_password);
#if defined(PPP_DNS_REQ) && (PPP_DNS_REQ > 0)
  /* Ask the peer for up to 2 DNS server addresses. */
  ppp_set_usepeerdns(device->ppp, 1);
#else 
  ppp_set_usepeerdns(device->ppp, 0);
#endif

  for(;;)
  {
    /*
    ** Step 2, 执行拨号流程
    */
    switch(device->ppp_status)
    {
    case 0:
      device->power_ctrl();
      device->ppp_status = 1;
#ifdef ppp_net_off
      ppp_net_off();
#endif
      break;
    case 1:
      handle = pppos_open_tty(device);
      device->serial = handle;
      device->ppp_status = 2;
      break;
    case 2:
      at_try_count = 0;      
      do {
        status = device->modem_at_dial(handle, device->buffer, device->apn_name);
        at_try_count++;
        if(at_try_count > AT_TRY_NUM) {
          device->power_ctrl();
          device->ppp_status = 0;
          break;
        }
        rt_thread_delay(RT_TICK_PER_SECOND/2);
      } while(status < 1);
      if(status > 0) {
        device->ppp_status = 3;
      }
      break;
    case 3:
      pppapi_connect(device->ppp, 5);
      len = 2000;
      rt_device_control(handle, RT_DEVICE_CRTL_TIMEOUT, &len);
      len = 0;
      rt_device_control(handle, RT_DEVICE_CTRL_INTERVAL, &len);
      device->ppp_status = 4;
      break;
    case 4:
      while(device->ppp_status >= 4)
      {
        // 如果设备不存在，被注销了, 直接卸载网卡
        if(rt_object_get_type(&handle->parent) != RT_Object_Class_Device) {
          device->ppp_status = 0;
          pppapi_close(device->ppp, 1);
          break;
        }
        len = rt_device_read(handle, 0, device->buffer, 1536);
        if(len > 0) {
          pppos_input_tcpip(device->ppp, device->buffer, len);
        }
      }
      break;
    default:
      break;
    }
  }
}

/*********************************************************************************************************
** Function name:       lwip_pppos_init
** Descriptions:        PPPOS拨号流程初始化
** input parameters:    tty_name:       PPPOS使用的串口
**                      power_ctrl:     执行modem开机流程
**                      modem_at_dial： 拨号连接的at指令执行流程
**                      apn_name:       APN名称
**                      user_name:      用户名
**                      user_password:   APN密码
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int lwip_pppos_init(const char *tty_name,  
                         void(*power_ctrl)(void), 
                         int(*modem_at_dial)(rt_device_t handle,uint8_t *buffer, const char *apn_name), 
                         const char *apn_name,const char * user_name, const char *user_password)
{
  int len;
  rt_thread_t tid;
  rt_device_t handle;
  pppos_device *device;
  
  /*
  ** Step 1, 先分配ppp设备的结构
  */
  device = rt_malloc(sizeof(pppos_device));
  if(device == RT_NULL) {
    return -1;
  }
  memset(device, 0, sizeof(pppos_device));
  
  /*
  ** Step 2, 初始化ppp结构，创建拨号线程
  */
  device->tty_name = tty_name;
  memset(device->apn_name, 0, 16);
  memset(device->user_name, 0, 32);
  memset(device->user_password, 0, 32);
  
  len = strlen(apn_name);
  if(len > 16) {
    len = 16;
  }
  strncpy(device->apn_name, apn_name, len);
  
  len = strlen(user_name);
  if(len > 32) {
    len = 32;
  }
  strncpy(device->user_name, user_name,len);
  
  len = strlen(user_name);
  if(len > 32) {
    len = 32;
  }
  strncpy(device->user_password, user_password, len);
  
  device->serial = RT_NULL;
  device->power_ctrl = power_ctrl;
  device->modem_at_dial = modem_at_dial;

  tid = rt_thread_create("ppp", pppos_rx_thread, device, RT_LWIP_PPPOS_STACKSIZE, RT_LWIP_PPPOS_PRIORITY, 20);
  
  if (tid != RT_NULL) {
    rt_thread_startup(tid);
  }
  return 1;
}

#endif   // end of RT_USE_PPPOS
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
