/****************************************Copyright (c)****************************************************
**                           湖 南 国 科 微 电 子 股 份 有 限 公 司
**                                http://www.gokemicro.com
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           wlan_rtthread.c
** Last modified Date:  2020-01-07
** Last Version:        V1.00
** Description:         MT7668 WLAN网卡驱动
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

#include <lwip_ethernet.h>
#include <netif/etharp.h>

#include "./../config.h"
#include "wlan_rtthread.h"
#include "./../os/gl_typedef.h"
#include "./../nic/mtx6xx_reg.h"
#include "./../nic/hal.h"
#include "./../sdio/gk_sdio.h"

/*********************************************************************************************************
** 调试输出宏定义
*********************************************************************************************************/
#define DBG_TAG         "WLAN_OS"
#define DBG_LVL         DBG_INFO    // DBG_LOG   DBG_WARNING   DBG_INFO
#include <rtdbg.h>

/*********************************************************************************************************
** 全局变量定义
*********************************************************************************************************/
//const static wwd_event_num_t cypress_wifi_events[]  = { WLC_E_LINK, WLC_E_DEAUTH_IND, WLC_E_NONE};
//const wiced_mac_t null_mac = {{0, 0, 0, 0, 0, 0}};

/*********************************************************************************************************
** Function name:       security_map
** Descriptions:        wiced和rt-thread的wifi加密方式映射
** Input parameters:    wlan： wifi框架结构
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
//wiced_security_t security_map(rt_wlan_security_t rt_security)
//{
//  wiced_security_t security = WICED_SECURITY_OPEN;
//  
//  switch (rt_security)
//  {
//  case SECURITY_OPEN:
//    security = WICED_SECURITY_OPEN;
//    break;
//  case SECURITY_WEP_PSK:
//    security = WICED_SECURITY_WEP_PSK;
//    break;
//  case SECURITY_WEP_SHARED:
//    security = WICED_SECURITY_WEP_SHARED;
//    break;
//  case SECURITY_WPA_TKIP_PSK:
//    security = WICED_SECURITY_WPA_TKIP_PSK;
//    break;
//  case SECURITY_WPA_AES_PSK:
//    security = WICED_SECURITY_WPA_AES_PSK;
//    break;
//  case SECURITY_WPA2_AES_PSK:
//    security = WICED_SECURITY_WPA2_AES_PSK;
//    break;
//  case SECURITY_WPA2_TKIP_PSK:
//    security = WICED_SECURITY_WPA2_TKIP_PSK;
//    break;
//  case SECURITY_WPA2_MIXED_PSK:
//    security = WICED_SECURITY_WPA2_MIXED_PSK;
//    break;
//  case SECURITY_WPS_OPEN:
//    security = WICED_SECURITY_WPS_OPEN;
//    break;
//  case SECURITY_WPS_SECURE:
//    security = WICED_SECURITY_WPS_SECURE;
//    break;
//  default:
//    break;
//  }
//  
//  return security;
//}

/*********************************************************************************************************
** Function name:       security_map_from_wiced
** Descriptions:        wiced和rt-thread的wifi加密方式反映射
** Input parameters:    wlan： wifi框架结构
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
//rt_wlan_security_t security_map_from_wiced(wiced_security_t security)
//{
//  rt_wlan_security_t result = SECURITY_OPEN;
//  
//  switch (security)
//  {
//  case WICED_SECURITY_OPEN:
//    result = SECURITY_OPEN;
//    break;
//  case WICED_SECURITY_WEP_PSK:
//    result = SECURITY_WEP_PSK;
//    break;
//  case WICED_SECURITY_WEP_SHARED:
//    result = SECURITY_WEP_SHARED;
//    break;
//  case WICED_SECURITY_WPA_TKIP_PSK:
//    result = SECURITY_WPA_TKIP_PSK;
//    break;
//  case WICED_SECURITY_WPA_AES_PSK:
//    result = SECURITY_WPA_AES_PSK;
//    break;
//  case WICED_SECURITY_WPA2_AES_PSK:
//    result = SECURITY_WPA2_AES_PSK;
//    break;
//  case WICED_SECURITY_WPA2_TKIP_PSK:
//    result = SECURITY_WPA2_TKIP_PSK;
//    break;
//  case WICED_SECURITY_WPA2_MIXED_PSK:
//    result = SECURITY_WPA2_MIXED_PSK;
//    break;
//  case WICED_SECURITY_WPS_OPEN:
//    result = SECURITY_WPS_OPEN;
//    break;
//  case WICED_SECURITY_WPS_SECURE:
//    result = SECURITY_WPS_SECURE;
//    break;
//  default:
//    break;
//  }
//  
//  return result;
//}

/*********************************************************************************************************
** Function name:       rthw_wlan_init
** Descriptions:        wlan初始化
** Input parameters:    wlan： wifi框架结构
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_init(struct rt_wlan_device *wlan)
{
  //  static uint8_t status = 0;
  //  rt_err_t result = -RT_EIO;
  //  
  //  if (status == 0) {
  //    wwd_result_t ret;
  //    
  //    ret = wwd_management_init(WICED_COUNTRY_CHINA, RT_NULL);
  //    if (ret != WWD_SUCCESS) {
  //      rt_kprintf("wifi initialization failed: %d\n", result);
  //    } else {
  //      status = 1;
  //      result = RT_EOK;
  ////#if LWIP_IGMP
  ////      netif_set_igmp_mac_filter(device->parent.parent.netif, bcm_igmp_mac_filter);
  ////#endif
  //    }
  //  }
  
  //  return result;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_mode
** Descriptions:        wlan设置工作模式
** Input parameters:    wlan： wifi框架结构
**                      mode： 工作模式，可以取RT_WLAN_STATION或RT_WLAN_AP
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_mode(struct rt_wlan_device *wlan, rt_wlan_mode_t mode)
{
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  if (mode == RT_WLAN_STATION)
  //  {
  //    if (wifi->interface != WWD_STA_INTERFACE)
  //    {
  //      return -RT_ERROR;
  //    }
  //  }
  //  else if (mode == RT_WLAN_AP)
  //  {
  //    if (wifi->interface != WWD_AP_INTERFACE)
  //    {
  //      return -RT_ERROR;
  //    }
  //  }
  
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       wifi_scan_result_handler
** Descriptions:        wlan扫描回调函数
** Input parameters:    result_ptr： 扫描结果
**                      user_data：  扫描数据
**                      
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
//static void wifi_scan_result_handler(wiced_scan_result_t **result_ptr,
//        void *user_data, wiced_scan_status_t status)
//{
//  struct rt_wlan_info wlan_info;
//  struct rt_wlan_buff buff;
//  wifi_wiced_info *wifi = user_data;
//  wiced_scan_result_t *result_info = *result_ptr;
//  
//  if (result_ptr == RT_NULL)
//  {
//    rt_wlan_dev_indicate_event_handle(wifi->wlan, RT_WLAN_DEV_EVT_SCAN_DONE, RT_NULL);
//    return;
//  }
//  
//  memset(&wlan_info, 0, sizeof(wlan_info));
//  memcpy(&wlan_info.bssid[0], result_info->BSSID.octet, 6);
//  strncpy((char *)wlan_info.ssid.val, (char const *)result_info->SSID.value, result_info->SSID.length);
//  wlan_info.ssid.len = result_info->SSID.length;
//  wlan_info.band = (result_info->band == WICED_802_11_BAND_2_4GHZ ? RT_802_11_BAND_2_4GHZ : RT_802_11_BAND_5GHZ);
//  wlan_info.channel = result_info->channel;
//  wlan_info.datarate = result_info->max_data_rate * 1000;
//  wlan_info.security = security_map_from_wiced(result_info->security);
//  wlan_info.rssi = result_info->signal_strength;
//  
//  buff.data = &wlan_info;
//  buff.len = sizeof(wlan_info);
//  rt_wlan_dev_indicate_event_handle(wifi->wlan, RT_WLAN_DEV_EVT_SCAN_REPORT, &buff);
//}

/*********************************************************************************************************
** Function name:       rthw_wlan_scan
** Descriptions:        wlan扫描
** Input parameters:    wlan： wifi框架结构
**                      scan_info：  wifi扫描结果存放结构
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_scan(struct rt_wlan_device *wlan, struct rt_scan_info *scan_info)
{
  //  int index;
  //  wwd_result_t ret = WWD_SUCCESS;
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  wiced_scan_result_t * update_result_ptr = (wiced_scan_result_t *)&wifi->scan_result;
  //  
  //  if (wifi->interface != WWD_STA_INTERFACE)
  //  {
  //    return -RT_ERROR;
  //  }
  //  
  //  ret = wwd_wifi_scan(WICED_SCAN_TYPE_ACTIVE,     // scan_type
  //                      WICED_BSS_TYPE_ANY,                     // bss_type
  //                      RT_NULL,                                // optional_ssid
  //                      RT_NULL,                                // optional_mac
  //                      RT_NULL,                                // optional_channel_list
  //                      RT_NULL,                                // optional_extended_params
  //                      wifi_scan_result_handler,               // callback
  //                      (wiced_scan_result_t **)(&update_result_ptr),   // result_ptr
  //                      (void *)wifi, WWD_STA_INTERFACE);     // user_data
  //  
  //  if (ret != WWD_SUCCESS) {
  //    return -RT_ERROR;
  //  }
  
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_join
** Descriptions:        wlan加入指定的热点
** Input parameters:    wlan： wifi框架结构
**                      sta_info: 加入的热点信息结构 
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_join(struct rt_wlan_device *wlan, struct rt_sta_info *sta_info)
{
  //  rt_err_t result;
  //  wwd_result_t ret;
  //  wiced_security_t security;
  //  wiced_ssid_t ssid;
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  if (wifi->interface != WWD_STA_INTERFACE)
  //  {
  //    return -RT_ERROR;
  //  }
  //  
  //  ssid.length = sta_info->ssid.len;
  //  memset(ssid.value, 0 , SSID_NAME_SIZE);
  //  memcpy(ssid.value, sta_info->ssid.val, sta_info->ssid.len);
  //  security = security_map(sta_info->security);
  //  
  //  
  //  ret = wwd_wifi_join (&ssid, security, sta_info->key.val,
  //                       sta_info->key.len, RT_NULL);
  //  
  //  if (ret != WWD_SUCCESS)
  //  {
  //    result = -RT_ERROR;
  //  }
  //  else
  //  {
  ////    /* set event handler */
  ////    wwd_management_set_event_handler(cypress_wifi_events, _bcm_wifi_event_handler, wifi, wifi->interface);
  //    result = RT_EOK;
  //  }
  
  //  return result;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_softap
** Descriptions:        wlan实现软件ap功能
** Input parameters:    wlan： wifi框架结构
**                      ap_info: 软热点信息结构 
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_softap(struct rt_wlan_device *wlan, struct rt_ap_info *ap_info)
{
  //  wwd_result_t ret;
  //  wiced_security_t security;
  //  wiced_ssid_t ssid;
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  if (wifi->interface != WWD_AP_INTERFACE)
  //  {
  //    return -RT_ERROR;
  //  }
  //  
  //  ssid.length = ap_info->ssid.len;
  //  memset(ssid.value, 0 , SSID_NAME_SIZE);
  //  memcpy(ssid.value, ap_info->ssid.val, ap_info->ssid.len);
  //  security = security_map(ap_info->security);
  //  
  //  /* startup AP mode */
  //  ret = wwd_wifi_start_ap(&ssid, security, ap_info->key.val,
  //                          ap_info->key.len, ap_info->channel);
  //  
  //  if (ret == WWD_SUCCESS)
  //  {
  //    return RT_EOK;
  //  }
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_disconnect
** Descriptions:        wlan断开连接
** Input parameters:    wlan： wifi框架结构
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_disconnect(struct rt_wlan_device *wlan)
{
  //  wwd_result_t ret;
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  if(wifi->interface != WWD_STA_INTERFACE) {
  //    return -RT_EIO;
  //  }
  //  
  //  ret = wwd_wifi_leave(WWD_STA_INTERFACE);
  //  
  //  if (ret == WWD_SUCCESS)
  //  {
  //    return RT_EOK;
  //  }
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_ap_stop
** Descriptions:        wlan停止ap功能
** Input parameters:    wlan： wifi框架结构
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_ap_stop(struct rt_wlan_device *wlan)
{
  //  wwd_result_t ret;
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  if(wifi->interface != WWD_AP_INTERFACE) {
  //    return -RT_EIO;
  //  }
  //  
  //  ret = wwd_wifi_stop_ap();
  //  if (WWD_SUCCESS !=  ret)
  //  {
  //    rt_kprintf("Failed to stop WICED access point\n");
  //  }
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_ap_deauth
** Descriptions:        wlan ap攻击
** Input parameters:    wlan： wifi框架结构
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_ap_deauth(struct rt_wlan_device *wlan, rt_uint8_t mac[])
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_scan_stop
** Descriptions:        wlan停止扫描
** Input parameters:    wlan： wifi框架结构
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_scan_stop(struct rt_wlan_device *wlan)
{
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  if(wifi->interface != WWD_STA_INTERFACE) {
  //    return -RT_EIO;
  //  }
  //  
  //  wwd_wifi_abort_scan();
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_get_rssi
** Descriptions:        wlan读取RSSI
** Input parameters:    wlan： wifi框架结构
** Output parameters:   None 无
** Returned value:      读取到的RSSI
*********************************************************************************************************/
static int rthw_wlan_get_rssi(struct rt_wlan_device *wlan)
{
  //  wwd_result_t ret;
  //  int32_t rssi;
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  if(wifi->interface != WWD_STA_INTERFACE) {
  //    return -RT_EIO;
  //  }
  //  
  //  ret = wwd_wifi_get_rssi(&rssi);
  //  if (ret == WWD_SUCCESS)
  //  {
  //    return rssi;
  //  }
  return 0;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_set_powersave
** Descriptions:        wlan设置进入低功耗模式
** Input parameters:    wlan： wifi框架结构
**                      level: 低功耗等级
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_set_powersave(struct rt_wlan_device *wlan, int level)
{
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  if (level > 0)
  //    wwd_wifi_enable_powersave();
  //  else
  //    wwd_wifi_disable_powersave();
  //  
  //  wifi->power_mode = level;
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_set_powersave
** Descriptions:        wlan功耗模式获取
** Input parameters:    wlan： wifi框架结构
** Output parameters:   None 无
** Returned value:      获取到的低功耗等级
*********************************************************************************************************/
static int rthw_wlan_get_powersave(struct rt_wlan_device *wlan)
{
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  return wifi->power_mode;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_cfg_promisc
** Descriptions:        wlan混杂模式配置
** Input parameters:    wlan： wifi框架结构
**                      start: 配置方式
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_cfg_promisc(struct rt_wlan_device *wlan, rt_bool_t start)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_cfg_filter
** Descriptions:        wlan配置滤波
** Input parameters:    wlan：  wifi框架结构
**                      filter: 滤波参数
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_cfg_filter(struct rt_wlan_device *wlan, struct rt_wlan_filter *filter)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_set_channel
** Descriptions:        wlan信道配置
** Input parameters:    wlan：  wifi框架结构
**                      channel: 待配置的信道
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_set_channel(struct rt_wlan_device *wlan, int channel)
{
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  wwd_wifi_set_channel(wifi->interface, channel);
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_get_channel
** Descriptions:        wlan信道获取
** Input parameters:    wlan：  wifi框架结构
** Output parameters:   None 无
** Returned value:      当前的信道
*********************************************************************************************************/
static int rthw_wlan_get_channel(struct rt_wlan_device *wlan)
{
  //  uint32_t channel;
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  wwd_wifi_get_channel(wifi->interface, &channel);
  //  return channel;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_set_country
** Descriptions:        wlan设置国家代码
** Input parameters:    wlan：         wifi框架结构
country_code： 国家代码方式 
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_set_country(struct rt_wlan_device *wlan, rt_country_code_t country_code)
{
  //  if(country_code != RT_COUNTRY_CHINA) {
  //    return -RT_ERROR;
  //  }
  
  //wwd_wifi_set_ccode(RT_COUNTRY_CHINA);
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_get_country
** Descriptions:        wlan获取国家代码
** Input parameters:    wlan：         wifi框架结构
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_country_code_t rthw_wlan_get_country (struct rt_wlan_device *wlan)
{
  return RT_COUNTRY_CHINA;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_set_mac
** Descriptions:        wlan设置mac地址
** Input parameters:    wlan：   wifi框架结构
**                      mac：    mac地址
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_set_mac(struct rt_wlan_device *wlan, rt_uint8_t mac[])
{
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  if(wifi->interface != WWD_STA_INTERFACE) {
  //    return -RT_ERROR;
  //  }
  //  
  //  if(WWD_SUCCESS == wwd_wifi_set_mac_address(*((wiced_mac_t *)mac))) {
  //    return RT_EOK;
  //  }
  //  
  //   return -RT_ERROR;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_get_mac
** Descriptions:        wlan获取mac地址
** Input parameters:    wlan：   wifi框架结构
**                      mac：    mac地址
** Output parameters:   None 无
** Returned value:      执行结果
*********************************************************************************************************/
static rt_err_t rthw_wlan_get_mac(struct rt_wlan_device *wlan, rt_uint8_t mac[])
{
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  
  //  if (wwd_wifi_get_mac_address((wiced_mac_t*)mac, wifi->interface) != WWD_SUCCESS ) {
  //    return RT_EOK;
  //  }
  //  
  //  return -RT_ERROR;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_recv
** Descriptions:        wlan接收数据
** Input parameters:    wlan：   wifi框架结构
**                      buff：   接收数据缓存地址
**                      len：    接收数据缓冲区大小
** Output parameters:   None 无
** Returned value:      实际收到的数据大小
*********************************************************************************************************/
static int rthw_wlan_recv(struct rt_wlan_device *wlan, void *buff, int len)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rthw_wlan_recv
** Descriptions:        wlan发送数据
** Input parameters:    wlan：   wifi框架结构
**                      buff：   发送数据缓存地址
**                      len：    发送数据缓冲区大小
** Output parameters:   None 无
** Returned value:      实际发送的数据大小
*********************************************************************************************************/
static int rthw_wlan_send(struct rt_wlan_device *wlan, void *buff, int len)
{
  //  wifi_wiced_info *wifi = (wifi_wiced_info *)(wlan->user_data);
  //  struct pbuf* p;
  //  
  //  if (wwd_wifi_is_ready_to_transceive((wwd_interface_t)wifi->interface) != WWD_SUCCESS)
  //  {
  //    return -RT_ERROR;
  //  }
  //  
  //#define WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME ( sizeof(wwd_buffer_header_t) + 12 + 4 + 2 )
  //  p = pbuf_alloc(PBUF_RAW, len + WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME, PBUF_POOL);
  //  if (p != RT_NULL)
  //  {
  //    pbuf_header(p, (s16_t)- WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME);
  //    pbuf_take(p, buff, len);
  //    
  //    wwd_network_send_ethernet_data((wiced_buffer_t)p, wifi->interface);
  //  }
  //  else
  //  {
  //    rt_kprintf("out of pbuf!\n");
  //  }
  
  return RT_EOK;
}

/*********************************************************************************************************
**  定义wifi驱动结构
*********************************************************************************************************/
static const struct rt_wlan_dev_ops ops = 
{
  .wlan_init             =     rthw_wlan_init,
  .wlan_mode             =     rthw_wlan_mode,
  .wlan_scan             =     rthw_wlan_scan,
  .wlan_join             =     rthw_wlan_join,
  .wlan_softap           =     rthw_wlan_softap,
  .wlan_disconnect       =     rthw_wlan_disconnect,
  .wlan_ap_stop          =     rthw_wlan_ap_stop,
  .wlan_ap_deauth        =     rthw_wlan_ap_deauth,
  .wlan_scan_stop        =     rthw_wlan_scan_stop,
  .wlan_get_rssi         =     rthw_wlan_get_rssi,
  .wlan_set_powersave    =     rthw_wlan_set_powersave,
  .wlan_get_powersave    =     rthw_wlan_get_powersave,
  .wlan_cfg_promisc      =     rthw_wlan_cfg_promisc,
  .wlan_cfg_filter       =     rthw_wlan_cfg_filter,
  .wlan_set_channel      =     rthw_wlan_set_channel,
  .wlan_get_channel      =     rthw_wlan_get_channel,
  .wlan_set_country      =     rthw_wlan_set_country,
  .wlan_get_country      =     rthw_wlan_get_country,
  .wlan_set_mac          =     rthw_wlan_set_mac,
  .wlan_get_mac          =     rthw_wlan_get_mac,
  .wlan_recv             =     rthw_wlan_recv,
  .wlan_send             =     rthw_wlan_send,
};

//static wifi_wiced_info   wiced_info_sta = 
//{
//  RT_NULL,                      // WIFI驱动结构
//  WWD_STA_INTERFACE,            // 指向SDIO的设备接口
//  {0},                          // 扫描结果结构
//  1                             // 功耗模式
//};
//
//static wifi_wiced_info   wiced_info_ap = 
//{
//  RT_NULL,                      // WIFI驱动结构
//  WWD_AP_INTERFACE,             // 指向SDIO的设备接口
//  {0},                          // 扫描结果结构
//  1                             // 功耗模式
//};

/* WiFi RxInterface */
//void host_network_process_ethernet_data(wiced_buffer_t buffer, wwd_interface_t interface)
//{
//  if(interface == WWD_STA_INTERFACE) {
//    rt_wlan_dev_report_data(wiced_info_sta.wlan, buffer, buffer->tot_len);
//  } else {
//    rt_wlan_dev_report_data(wiced_info_ap.wlan, buffer, buffer->tot_len);
//  }
//}


gk_wlan_info __gwifi_info;
/*********************************************************************************************************
** Function name:       rt_wifi_probe
** Descriptions:        WIFI网卡枚举到后的注册函数
** Input parameters:    pvData: 驱动传入参数，如果是SDIO，是func
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
int rt_wifi_probe(void *pvData)
{
//  struct wireless_dev *prWdev = NULL;
//  P_WLANDEV_INFO_T prWlandevInfo = NULL;
//  INT_32 i4DevIdx = 0;
//  P_GLUE_INFO_T prGlueInfo = NULL;
//  P_ADAPTER_T prAdapter = NULL;
  int i32Status = 0;
  rt_bool_t bRet = RT_FALSE;
//  P_REG_INFO_T prRegInfo;
//#if (MTK_WCN_HIF_SDIO && CFG_WMT_WIFI_PATH_SUPPORT)
//  INT_32 i4RetVal = 0;
//#endif
//  
//#if 0
//  PUINT_8 pucConfigBuf = NULL, pucCfgBuf = NULL;
//  UINT_32 u4ConfigReadLen = 0;
//#endif
  
  __gwifi_info.func = pvData;
  
  do {
    /* 4 <1> Initialize the IO port of the interface */
    /*  GeorgeKuo: pData has different meaning for _HIF_XXX:
    * _HIF_EHPI: pointer to memory base variable, which will be
    *      initialized by glBusInit().
    * _HIF_SDIO: bus driver handle
    */
    
    LOG_I("enter wlanProbe");
    
    bRet = glBusInit(pvData);
    
    /* Cannot get IO address from interface */
    if (bRet == RT_FALSE) {
      LOG_E( "wlanProbe: glBusInit() fail");
      i32Status = -EIO;
      break;
    }
//    /* 4 <2> Create network device, Adapter, KalInfo, prDevHandler(netdev) */
//    prWdev = wlanNetCreate(pvData, pvDriverData);
//    if (prWdev == NULL) {
//      LOG_E("wlanProbe: No memory for dev and its private");
//      i32Status = -ENOMEM;
//      break;
//    }
//    /* 4 <2.5> Set the ioaddr to HIF Info */
//    prGlueInfo = (P_GLUE_INFO_T) wiphy_priv(prWdev->wiphy);
//    gPrDev = prGlueInfo->prDevHandler;
//    
//    /* 4 <4> Setup IRQ */
//    prWlandevInfo = &arWlanDevInfo[i4DevIdx];
    
    i32Status = glBusSetIrq(&__gwifi_info, NULL);
    if (i32Status != RT_EOK) {
      LOG_E("wlanProbe: Set IRQ error");
      break;
    }
    
//    prGlueInfo->i4DevIdx = i4DevIdx;
//    
//    prAdapter = prGlueInfo->prAdapter;
//    
//    prGlueInfo->u4ReadyFlag = 0;
//    
//#if CFG_TCP_IP_CHKSUM_OFFLOAD
//    prAdapter->u4CSUMFlags = (CSUM_OFFLOAD_EN_TX_TCP | CSUM_OFFLOAD_EN_TX_UDP | CSUM_OFFLOAD_EN_TX_IP);
//#endif
    
#if CFG_SUPPORT_CFG_FILE
    wlanGetConfig(prAdapter);
#endif
    /* Default support 2.4/5G MIMO */
    prAdapter->rWifiFemCfg.u2WifiPath =
      (WLAN_FLAG_2G4_WF0 | WLAN_FLAG_5G_WF0 | WLAN_FLAG_2G4_WF1 | WLAN_FLAG_5G_WF1);
    
    LOG_I("WifiPath Init=0x%x", prAdapter->rWifiFemCfg.u2WifiPath);
    
#if (MTK_WCN_HIF_SDIO && CFG_WMT_WIFI_PATH_SUPPORT)
    i4RetVal = mtk_wcn_wmt_wifi_fem_cfg_report((PVOID)&prAdapter->rWifiFemCfg);
    
    if (i4RetVal)
      LOG_W("Get WifiPath from WMT drv FAIL");
    else
      LOG_I("Get WifiPath from WMT drv Success WifiPath=0x%x",
             prAdapter->rWifiFemCfg.u2WifiPath);
#endif
    /* 4 <5> Start Device */
    prRegInfo = &prGlueInfo->rRegInfo;
    
    /* P_REG_INFO_T prRegInfo = (P_REG_INFO_T) kmalloc(sizeof(REG_INFO_T), GFP_KERNEL); */
    kalMemSet(prRegInfo, 0, sizeof(REG_INFO_T));
    
    /* Trigger the action of switching Pwr state to drv_own */
    prAdapter->fgIsFwOwn = TRUE;
    
    nicpmWakeUpWiFi(prAdapter);
    
    /* Load NVRAM content to REG_INFO_T */
    glLoadNvram(prGlueInfo, prRegInfo);
    
    /* kalMemCopy(&prGlueInfo->rRegInfo, prRegInfo, sizeof(REG_INFO_T)); */
    
    prRegInfo->u4PowerMode = CFG_INIT_POWER_SAVE_PROF;
#if 0
    prRegInfo->fgEnArpFilter = TRUE;
#endif
    
    if (wlanAdapterStart(prAdapter, prRegInfo) != WLAN_STATUS_SUCCESS)
      i32Status = -EIO;
    
    /* kfree(prRegInfo); */
    
    if (i32Status < 0)
      break;
    
    INIT_WORK(&prGlueInfo->rTxMsduFreeWork, kalFreeTxMsduWorker);
    INIT_DELAYED_WORK(&prGlueInfo->rRxPktDeAggWork, halDeAggRxPktWorker);
    
    prGlueInfo->main_thread = kthread_run(main_thread, prGlueInfo->prDevHandler, "main_thread");
#if CFG_SUPPORT_MULTITHREAD
    prGlueInfo->hif_thread = kthread_run(hif_thread, prGlueInfo->prDevHandler, "hif_thread");
    prGlueInfo->rx_thread = kthread_run(rx_thread, prGlueInfo->prDevHandler, "rx_thread");
#endif
    
    
    /* TODO the change schedule API shall be provided by OS glue layer */
    /* Switch the Wi-Fi task priority to higher priority and change the scheduling method */
    if (prGlueInfo->prAdapter->rWifiVar.ucThreadPriority > 0) {
      struct sched_param param = {.sched_priority = prGlueInfo->prAdapter->rWifiVar.ucThreadPriority
      };
      sched_setscheduler(prGlueInfo->main_thread,
                         prGlueInfo->prAdapter->rWifiVar.ucThreadScheduling, &param);
#if CFG_SUPPORT_MULTITHREAD
      sched_setscheduler(prGlueInfo->hif_thread,
                         prGlueInfo->prAdapter->rWifiVar.ucThreadScheduling, &param);
      sched_setscheduler(prGlueInfo->rx_thread,
                         prGlueInfo->prAdapter->rWifiVar.ucThreadScheduling, &param);
#endif
      LOG_I("Set pri = %d, sched = %d\n",
             prGlueInfo->prAdapter->rWifiVar.ucThreadPriority,
             prGlueInfo->prAdapter->rWifiVar.ucThreadScheduling);
    }
    
    /* Disable 5G band for AIS */
    if (prAdapter->fgEnable5GBand == FALSE)
      prWdev->wiphy->bands[IEEE80211_BAND_5GHZ] = NULL;
    
    g_u4HaltFlag = 0;
    
#if CFG_SUPPORT_BUFFER_MODE
#if (CFG_EFUSE_BUFFER_MODE_DELAY_CAL == 1)
    if (wlanDownloadBufferBin(prAdapter) != WLAN_STATUS_SUCCESS)
      return -1;
    
#endif
#endif
    /* set MAC address */
    {
      WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
      struct sockaddr MacAddr;
      UINT_32 u4SetInfoLen = 0;
      
      rStatus = kalIoctl(prGlueInfo,
                         wlanoidQueryCurrentAddr,
                         &MacAddr.sa_data, PARAM_MAC_ADDR_LEN, TRUE, TRUE, TRUE, &u4SetInfoLen);
      
      if (rStatus != WLAN_STATUS_SUCCESS) {
        LOG_W("set MAC addr fail 0x%lx\n", rStatus);
        prGlueInfo->u4ReadyFlag = 0;
      } else {
        kalMemCopy(prGlueInfo->prDevHandler->dev_addr, &MacAddr.sa_data, ETH_ALEN);
        kalMemCopy(prGlueInfo->prDevHandler->perm_addr,
                   prGlueInfo->prDevHandler->dev_addr, ETH_ALEN);
        
        /* card is ready */
        prGlueInfo->u4ReadyFlag = 1;
#if CFG_SHOW_MACADDR_SOURCE
        LOG_I("MAC address: " MACSTR, MAC2STR(&MacAddr.sa_data));
#endif
      }
    }
    
#if CFG_TCP_IP_CHKSUM_OFFLOAD
    /* set HW checksum offload */
    {
      WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
      UINT_32 u4CSUMFlags = CSUM_OFFLOAD_EN_ALL;
      UINT_32 u4SetInfoLen = 0;
      
      rStatus = kalIoctl(prGlueInfo,
                         wlanoidSetCSUMOffload,
                         (PVOID) &u4CSUMFlags, sizeof(UINT_32), FALSE, FALSE, TRUE, &u4SetInfoLen);
      
      if (rStatus != WLAN_STATUS_SUCCESS)
        LOG_W("set HW checksum offload fail 0x%lx", rStatus);
    }
#endif
    
    /* 4 <3> Register the card */
    i4DevIdx = wlanNetRegister(prWdev);
    if (i4DevIdx < 0) {
      i32Status = -ENXIO;
      LOG_E("wlanProbe: Cannot register the net_device context to the kernel");
      break;
    }
    /* 4 <4> Register early suspend callback */
#if CFG_ENABLE_EARLY_SUSPEND
    glRegisterEarlySuspend(&wlan_early_suspend_desc, wlan_early_suspend, wlan_late_resume);
#endif
    
    /* 4 <5> Register Notifier callback */
    wlanRegisterNotifier();
    
    /* 4 <6> Initialize /proc filesystem */
#ifdef WLAN_INCLUDE_PROC
    i32Status = procCreateFsEntry(prGlueInfo);
    if (i32Status < 0) {
      LOG_E("wlanProbe: init procfs failed");
      break;
    }
#endif /* WLAN_INCLUDE_PROC */
    
#if CFG_MET_PACKET_TRACE_SUPPORT
    kalMetInit(prGlueInfo);
#endif
    
#if CFG_ENABLE_BT_OVER_WIFI
    prGlueInfo->rBowInfo.fgIsNetRegistered = FALSE;
    prGlueInfo->rBowInfo.fgIsRegistered = FALSE;
    glRegisterAmpc(prGlueInfo);
#endif
    
#if (CFG_ENABLE_WIFI_DIRECT && MTK_WCN_HIF_SDIO && CFG_SUPPORT_MTK_ANDROID_KK)
    register_set_p2p_mode_handler(set_p2p_mode_handler);
#elif (CFG_ENABLE_WIFI_DIRECT)
    if (prAdapter->rWifiVar.u4RegP2pIfAtProbe) {
      PARAM_CUSTOM_P2P_SET_STRUCT_T rSetP2P;
      
      rSetP2P.u4Enable = 1;
      
#ifdef CFG_DRIVER_INITIAL_RUNNING_MODE
      rSetP2P.u4Mode = CFG_DRIVER_INITIAL_RUNNING_MODE;
#else
      rSetP2P.u4Mode = RUNNING_P2P_MODE;
#endif /* CFG_DRIVER_RUNNING_MODE */
      if (set_p2p_mode_handler(prWdev->netdev, rSetP2P) == 0)
        LOG_I("%s: p2p device registered", __func__);
      else
        LOG_E("%s: Failed to register p2p device", __func__);
    }
#endif
  } while (RT_FALSE);
  
  if (i32Status == 0) {
#if CFG_SUPPORT_AGPS_ASSIST
    kalIndicateAgpsNotify(prAdapter, AGPS_EVENT_WLAN_ON, NULL, 0);
#endif
    
    
    
#if CFG_SUPPORT_EASY_DEBUG
    
#if 0
    wlanGetParseConfig(prGlueInfo->prAdapter);
    /*wlanGetParseConfig would reparsing the config file,
    *and then, sent to firmware
    *use wlanFeatureToFw to take it(won't be reparsing)
    */
#endif
    
    /* move before reading file
    *wlanLoadDefaultCustomerSetting(prAdapter);
    */
    
    wlanFeatureToFw(prGlueInfo->prAdapter);
    
#endif
    wlanCfgSetSwCtrl(prGlueInfo->prAdapter);
    
    wlanCfgSetChip(prGlueInfo->prAdapter);
    
    wlanCfgSetCountryCode(prGlueInfo->prAdapter);
    
#if (CFG_MET_PACKET_TRACE_SUPPORT == 1)
    LOG_I("init MET procfs...");
    i32Status = kalMetInitProcfs(prGlueInfo);
    if (i32Status < 0)
      LOG_E("wlanProbe: init MET procfs failed");
#endif
    
#if CFG_MET_TAG_SUPPORT
    if (met_tag_init() != 0)
      LOG_E("MET_TAG_INIT error!");
#endif
    
#if CFG_SUPPORT_CAL_RESULT_BACKUP_TO_HOST
    /* Calibration Backup Flow */
    if (!g_fgIsCalDataBackuped) {
      if (rlmTriggerCalBackup(prGlueInfo->prAdapter, g_fgIsCalDataBackuped) == WLAN_STATUS_FAILURE) {
        LOG_E("Error : Boot Time Wi-Fi Enable Fail........");
        return -1;
      }
      
      g_fgIsCalDataBackuped = TRUE;
    } else {
      if (rlmTriggerCalBackup(prGlueInfo->prAdapter, g_fgIsCalDataBackuped) == WLAN_STATUS_FAILURE) {
        LOG_I("Error : Normal Wi-Fi Enable Fail........");
        return -1;
      }
    }
#endif
    
    LOG_I("wlanProbe: probe success");
    if(1) { // Deren: ZTE force init
      {// Enable async IRQ
        CMD_ACCESS_REG rCmdAccessReg;
        WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
        UINT_32 u4BufLen = 0;
        rCmdAccessReg.u4Address = 0x8;
        rCmdAccessReg.u4Data				= 0xf;
        rStatus = kalIoctl(prGlueInfo,wlanoidSetDrvMcrWrite,&rCmdAccessReg, sizeof(rCmdAccessReg), FALSE, FALSE, TRUE, &u4BufLen);
        if(rStatus == WLAN_STATUS_SUCCESS)
          LOG_I("ZTE async cmd success");
        else
          LOG_E("ZTE async cmd fail");
      }
    }
    
    
  } 
  else {
    glBusFreeIrq(&__gwifi_info);
    LOG_E("wlanProbe: probe failed");
  }
  
  return i32Status;
}

/*********************************************************************************************************
** Function name:       rthw_wifi_low_init
** Descriptions:        WIFI网卡卸载函数
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
void rt_wifi_remove(void)
{
  __gwifi_info.func = RT_NULL;
}

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
