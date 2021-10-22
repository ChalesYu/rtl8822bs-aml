/*
 * wf_os_api_mix.c
 *
 * os wlan mix operate realization.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
/* include */
#include "common.h"
#include "hif/hif.h"
#include "wf_debug.h"

/* macro */

/* type */
static rt_wlan_security_t wf_wlan_security_mode_get(wf_wlan_mgmt_scan_que_node_t *pscan_que_node)
{
  wf_u8 mode;
  wf_80211_mgmt_ie_t *pie;
  size_t pie_len;
  wf_u32 group_cipher = 0, pairwise_cipher = 0;

  if(pscan_que_node->privacy) {
    mode = 0;
    
    /* parsing WPA/WPA2 */
    pie = (wf_80211_mgmt_ie_t *)pscan_que_node->wpa_ie;
    pie_len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
    if(!wf_80211_mgmt_wpa_parse(pie, pie_len, &group_cipher, &pairwise_cipher))
    {
    	if(pairwise_cipher == (CIPHER_SUITE_TKIP | CIPHER_SUITE_CCMP))
        {
            pairwise_cipher = CIPHER_SUITE_CCMP;
        }
        if ((group_cipher == CIPHER_SUITE_TKIP) && (pairwise_cipher == CIPHER_SUITE_TKIP))
        {
            mode = 0x01;
        }
        else if ((group_cipher == CIPHER_SUITE_CCMP) && (pairwise_cipher == CIPHER_SUITE_CCMP))
        {
            mode = 0x02;
        } 
		else if ((group_cipher == CIPHER_SUITE_TKIP) && (pairwise_cipher == CIPHER_SUITE_CCMP))
        {
            mode = 0x08;
        }
    }
    
    /* parsing rsn */
    pie = (wf_80211_mgmt_ie_t *)pscan_que_node->rsn_ie;
    pie_len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
    if(!wf_80211_mgmt_rsn_parse(pie, pie_len, &group_cipher, &pairwise_cipher))
    {
    	if(pairwise_cipher == (CIPHER_SUITE_TKIP | CIPHER_SUITE_CCMP))
        {
            pairwise_cipher = CIPHER_SUITE_CCMP;
        }
        if ((group_cipher == CIPHER_SUITE_TKIP) && (pairwise_cipher == CIPHER_SUITE_TKIP))
        {
            mode = 0x03;
        }
        else if ((group_cipher == CIPHER_SUITE_CCMP) && (pairwise_cipher == CIPHER_SUITE_CCMP))
        {
            mode = 0x04;
        }
        else if ((group_cipher == CIPHER_SUITE_TKIP) && (pairwise_cipher == CIPHER_SUITE_CCMP))
        {
            mode = 0x05;
        }
    }
    
    /* parsing WPS */
    //pie = (wf_80211_mgmt_ie_t *)pscan_que_node->wps_ie;
    //pie_len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
    //if (pie->len) {
      //mode = 0x04;
    //}
    
    switch(mode) {
    case 0:
      return SECURITY_WEP_SHARED;
    case 1:
      return SECURITY_WPA_TKIP_PSK;
    case 2:
      return SECURITY_WPA_AES_PSK;
    case 3:
      return SECURITY_WPA2_TKIP_PSK;
    case 4:
      return SECURITY_WPA2_AES_PSK;
    case 5:
      return SECURITY_WPA2_MIXED_PSK;
    case 6:
      return SECURITY_WPS_OPEN;
    case 7:
      return SECURITY_WPS_SECURE;
	case 8:
      return SECURITY_WPA_MIXED_PSK;
    default:
      return SECURITY_UNKNOWN;
    }
    
  } else {
    return SECURITY_OPEN;
  }
}

void wf_os_api_ind_scan_done (void *arg, wf_bool arg1, wf_u8 arg2)
{
  wf_u32 max_rate, rate, i;
  nic_info_st *pnic_info = arg;
  struct rt_wlan_device *wlan_dev = pnic_info->ndev;
  wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
  wf_wlan_mgmt_scan_que_node_t *pscan_que_node = NULL;
  struct rt_wlan_info wlan_info;
  struct rt_wlan_buff buff;
  
  wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
  {
    memset(&wlan_info,0,sizeof(struct rt_wlan_info));
    wlan_info.security = wf_wlan_security_mode_get(pscan_que_node);
    wlan_info.band = RT_802_11_BAND_2_4GHZ;
    /*Add basic and extended rates */
    max_rate = 0;
    i = 0;
    while (pscan_que_node->spot_rate[i] != 0)
    {
      rate = pscan_que_node->spot_rate[i] & 0x7F;
      if (rate > max_rate)
      {
        max_rate = rate;
      }
      i++;
    }
    if (pscan_que_node->mcs & 0x8000)  /* MCS15 */
    {
      max_rate = pscan_que_node->bw_40mhz ?
        (pscan_que_node->short_gi ? 300 : 270) :
        (pscan_que_node->short_gi ? 144 : 130);
    }
    else     /* default MCS7 */
    {
      max_rate = (pscan_que_node->bw_40mhz) ?
        (pscan_que_node->short_gi ? 150 : 135) :
        (pscan_que_node->short_gi ? 72 : 65);
    }
    max_rate = max_rate * 1000000;
    wlan_info.datarate = max_rate;
    
    wlan_info.channel = pscan_que_node->channel;
    wlan_info.rssi = pscan_que_node->signal_strength_scale;
    memcpy(wlan_info.ssid.val,pscan_que_node->ssid.data, pscan_que_node->ssid.length);
    wlan_info.ssid.len = pscan_que_node->ssid.length;
    memcpy(wlan_info.bssid,pscan_que_node->bssid,WF_ETH_ALEN);
    if(pscan_que_node->ssid_type == WF_80211_HIDDEN_SSID_NOT_IN_USE) {
      wlan_info.hidden = 1;
    } else {
      wlan_info.hidden = 0;
    }
    
    buff.data = &wlan_info;
    buff.len = sizeof(struct rt_wlan_info);
    /* indicate scan report event */
    rt_wlan_dev_indicate_event_handle(wlan_dev, RT_WLAN_DEV_EVT_SCAN_REPORT, &buff);
  }
  wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);
}

void wf_os_api_ind_connect (void *arg, wf_u8 arg1)
{
  nic_info_st *pnic_info = arg;
  struct rt_wlan_device *wlan_dev = pnic_info->ndev;
  
  switch(NIC_INFO_2_WORK_MODE(pnic_info))
  {
  case WF_INFRA_MODE :  //sta
    rt_wlan_dev_indicate_event_handle(wlan_dev, RT_WLAN_DEV_EVT_CONNECT, NULL);
    break;
  case WF_MASTER_MODE : // ap
    rt_wlan_dev_indicate_event_handle(wlan_dev, RT_WLAN_DEV_EVT_AP_START, NULL);
    break;
  case WF_ADHOC_MODE :
  case WF_AUTO_MODE :
  case WF_REPEAT_MODE :
  case WF_SECOND_MODES :
  case WF_MONITOR_MODE :
  case WF_MESH_MODE :
  default:
    LOG_W("[%s]: error work mode", __func__);
    break;
  }
}

void wf_os_api_ind_disconnect (void *arg, wf_u8 arg1)
{
  nic_info_st *pnic_info = arg;
  struct rt_wlan_device *wlan_dev = pnic_info->ndev;
  
  wf_os_api_disable_all_data_queue(pnic_info->ndev);
  
  switch(NIC_INFO_2_WORK_MODE(pnic_info))
  {
  case WF_INFRA_MODE :  //sta
    rt_wlan_dev_indicate_event_handle(wlan_dev, RT_WLAN_DEV_EVT_DISCONNECT, NULL);
    break;
  case WF_MASTER_MODE : // ap
    rt_wlan_dev_indicate_event_handle(wlan_dev, RT_WLAN_DEV_EVT_AP_STOP, NULL);
    break;
  case WF_ADHOC_MODE :
  case WF_AUTO_MODE :
  case WF_REPEAT_MODE :
  case WF_SECOND_MODES :
  case WF_MONITOR_MODE :
  case WF_MESH_MODE :
  default:
    LOG_W("[%s]: error work mode", __func__);
    break;
  }
}

#ifdef CFG_ENABLE_ADHOC_MODE
void wf_os_api_cfg80211_unlink_ibss(void *arg)
{
    LOG_W("[%s]: enter", __func__);
}
#endif


void wf_os_api_ap_ind_assoc (void *arg, void *arg1, void *arg2, wf_u8 arg3)
{
  nic_info_st *pnic_info = arg;
  struct rt_wlan_device *wlan_dev = pnic_info->ndev;
  
  LOG_W("[%s]: enter", __func__);
  rt_wlan_dev_indicate_event_handle(wlan_dev, RT_WLAN_DEV_EVT_AP_ASSOCIATED, NULL);
  
}

void wf_os_api_ap_ind_disassoc (void *arg, void *arg1, wf_u8 arg2)
{
  nic_info_st *pnic_info = arg;
  struct rt_wlan_device *wlan_dev = pnic_info->ndev;
  
  LOG_W("[%s]: enter", __func__);
  rt_wlan_dev_indicate_event_handle(wlan_dev, RT_WLAN_DEV_EVT_AP_DISASSOCIATED, NULL);
}

void wf_os_api_enable_all_data_queue (void *arg)
{
    LOG_W("The netif link up");
}

void wf_os_api_disable_all_data_queue (void *arg)
{
    LOG_W("The netif link down");
}

wf_u32 wf_os_api_rand32 (void)
{
   time_t t;
   
   srand((unsigned) time(&t));
   
   return rand();
}

//void *wf_os_api_get_ars(void *nic_info)
//{
//    nic_info_st * ni        = nic_info;
//    hif_node_st *hif_info   = ni->hif_node;
//    return hif_info->ars;
//}
//void  wf_os_api_set_ars(void *nic_info,void *ars)
//{
//    nic_info_st * ni        = nic_info;
//    hif_node_st *hif_info   = ni->hif_node;
//    hif_info->ars           = ars;
//}
//

void *wf_os_api_get_odm(void *nic_info)
{
    nic_info_st * ni        = nic_info;
    hif_node_st *hif_info   = ni->hif_node;
    return hif_info->odm;
}
void  wf_os_api_set_odm(void *nic_info,void *odm)
{
    nic_info_st * ni        = nic_info;
    hif_node_st *hif_info   = ni->hif_node;
    hif_info->odm           = odm;
}


wf_s32 wf_os_api_get_cpu_id(void)
{
#ifdef RT_USING_SMP
    return rt_hw_cpu_id();
#else 
    return 0;
#endif
}

