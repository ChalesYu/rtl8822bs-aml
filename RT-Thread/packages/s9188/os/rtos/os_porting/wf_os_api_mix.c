
/* include */
#include "common.h"

/* macro */

/* type */

/* function declaration */

static wlan_dev_security_t wf_wlan_security_mode_get(wf_wlan_mgmt_scan_que_node_t *pscan_que_node)
{
  wf_u8 mode;
  wf_80211_mgmt_ie_t *pie;
  wf_u8 pie_len;
  wf_u32 group_cipher = 0, pairwise_cipher = 0;

  if(pscan_que_node->privacy) 
  	{
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
	nic_info_st *pnic_info = arg;
	wlan_dev_t *wdev = pnic_info->dev;
	wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
    wf_wlan_mgmt_scan_que_node_t *pscan_que_node;
	scan_node_t *scan_node = NULL;
	wf_u32 max_rate, rate, i;
    wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
    {
    	if(wdev->scan_list == NULL)
    	{
    		return ;
		}
		scan_node = os_malloc(sizeof(scan_node_t));
		if(scan_node == NULL)
		{
			return ;
		}
		os_memset(scan_node, 0, sizeof(scan_node_t));
		scan_node->scan_data->channel = pscan_que_node->channel;
		scan_node->scan_data->band = RT_802_11_BAND_2_4GHZ;
		scan_node->scan_data->security =
			wf_wlan_security_mode_get(pscan_que_node);
		scan_node->scan_data->rssi = pscan_que_node->signal_strength_scale;
		os_memcpy(scan_node->scan_data->ssid.val,pscan_que_node->ssid.data, pscan_que_node->ssid.length);
		scan_node->scan_data->ssid.len = pscan_que_node->ssid.length;
		os_memcpy(scan_node->scan_data->bssid,pscan_que_node->bssid,WF_ETH_ALEN);
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
    	scan_node->scan_data->datarate = max_rate;
		if(pscan_que_node->ssid_type == WF_80211_HIDDEN_SSID_NOT_IN_USE) 
		{
		  scan_node->scan_data->hidden = 1;
		} 
		else 
		{
		  scan_node->scan_data->hidden = 0;
		}
		wdev->scan_list->list.pnext = scan_node->list.prev;
		wdev->scan_list->list.prev = NULL;
		scan_node->list.pnext = NULL;
	}	
    wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);
	
}

void wf_os_api_ind_connect (void *arg, wf_u8 arg1)
{
#if 0
    nic_info_st *pnic_info = arg;
    wf_mlme_framework_e framework = arg1;

#ifdef CONFIG_WIRELESS_EXT
    if (framework == WF_MLME_FRAMEWORK_WEXT)
    {
        wf_wlan_mgmt_info_t *wlan_mgmt_info = pnic_info->wlan_mgmt_info;
        wf_wlan_network_t *pcur_network = &wlan_mgmt_info->cur_network;
        union iwreq_data wrqu;

        wf_memset(&wrqu, 0, sizeof(union iwreq_data));

        wrqu.ap_addr.sa_family = ARPHRD_ETHER;
        wf_memcpy(wrqu.ap_addr.sa_data, pcur_network->bssid, ETH_ALEN);
        wireless_send_event(pnic_info->ndev, SIOCGIWAP, &wrqu, NULL);
    }
#endif
#ifdef CONFIG_IOCTL_CFG80211
    if (framework == WF_MLME_FRAMEWORK_NETLINK)
    {
#ifdef CFG_ENABLE_ADHOC_MODE
        if(get_sys_work_mode(pnic_info) == WF_ADHOC_MODE)
        {
            wf_cfg80211_ibss_indicate_connect(pnic_info);
        }
        else
#endif
        {
            wf_cfg80211_indicate_connect(pnic_info);
        }
    }
#endif
#endif
}

void wf_os_api_ind_disconnect (void *arg, wf_u8 arg1)
{
#if 0
    nic_info_st *pnic_info = arg;
    wf_mlme_framework_e framework = arg1;

#ifdef CONFIG_WIRELESS_EXT
    if (framework == WF_MLME_FRAMEWORK_WEXT)
    {
        union iwreq_data wrqu;

        wf_memset(&wrqu, 0, sizeof(union iwreq_data));

        wrqu.ap_addr.sa_family = ARPHRD_ETHER;
        wf_memset(wrqu.ap_addr.sa_data, 0, ETH_ALEN);
        wireless_send_event(pnic_info->ndev, SIOCGIWAP, &wrqu, NULL);
    }
#endif
#ifdef CONFIG_IOCTL_CFG80211
    if (framework == WF_MLME_FRAMEWORK_NETLINK)
    {
        wf_cfg80211_indicate_disconnect(pnic_info);
    }
#endif
#endif
}

#ifdef CFG_ENABLE_ADHOC_MODE
void wf_os_api_cfg80211_unlink_ibss(void *arg)
{
#if 0
#ifdef CONFIG_IOCTL_CFG80211
    nic_info_st *pnic_info = arg;
    wf_cfg80211_unlink_ibss(pnic_info);
#endif
#endif
}
#endif


#ifdef CFG_ENABLE_AP_MODE
void wf_os_api_ap_ind_assoc (void *arg, void *arg1, void *arg2, wf_u8 arg3)
{
#if 0
    nic_info_st *pnic_info = arg;
    wdn_net_info_st *pwdn_info = arg1;
    wf_mlme_framework_e framework = arg3;

#ifdef CONFIG_WIRELESS_EXT
    if (framework == WF_MLME_FRAMEWORK_WEXT)
    {
        union iwreq_data wrqu;

        wrqu.addr.sa_family = ARPHRD_ETHER;
        wf_memcpy(wrqu.addr.sa_data, pwdn_info->mac, WF_ETH_ALEN);
        wireless_send_event(pnic_info->ndev, IWEVREGISTERED, &wrqu, NULL);
    }
#endif
#ifdef CONFIG_IOCTL_CFG80211
    if (framework == WF_MLME_FRAMEWORK_NETLINK)
    {
        wf_ap_msg_t *pmsg = arg2;

        wf_ap_cfg80211_assoc_event_up(pnic_info, (wf_u8 *)&pmsg->mgmt, pmsg->len);
    }
#endif
#endif
}

void wf_os_api_ap_ind_disassoc (void *arg, void *arg1, wf_u8 arg2)
{
#if 0
    nic_info_st *pnic_info = arg;
    wdn_net_info_st *pwdn_info = arg1;
    wf_mlme_framework_e framework = arg2;

#ifdef CONFIG_WIRELESS_EXT
    if (framework == WF_MLME_FRAMEWORK_WEXT)
    {
        union iwreq_data wrqu;

        wrqu.addr.sa_family = ARPHRD_ETHER;
        wf_memcpy(wrqu.addr.sa_data, pwdn_info->mac, WF_ETH_ALEN);
        wireless_send_event(pnic_info->ndev, IWEVEXPIRED, &wrqu, NULL);
    }
#endif
#ifdef CONFIG_IOCTL_CFG80211
    if (framework == WF_MLME_FRAMEWORK_NETLINK)
    {
        wf_ap_cfg80211_disassoc_event_up(pnic_info, pwdn_info);
    }
#endif
#endif
}
#endif


void wf_os_api_enable_all_data_queue (void *arg)
{
#if 0
    struct net_device *ndev = arg;

    netif_carrier_on(ndev);

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,35))
    netif_tx_start_all_queues(ndev);
#else
    netif_start_queue(ndev);
#endif
#endif
}

void wf_os_api_disable_all_data_queue (void *arg)
{
#if 0
    struct net_device *ndev = arg;

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,35))
    netif_tx_stop_all_queues(ndev);
#else
    netif_stop_queue(ndev);
#endif

    netif_carrier_off(ndev);
    LOG_W("The netif carrier off");
#endif
}

OS_INLINE wf_u32 wf_os_api_rand32 (void)
{
    return OS_RANDOM32();
}

//void *wf_os_api_get_ars(void *nic_info)
//{
//    nic_info_st * ni        = nic_info;
//    hif_node_t *hif_info   = ni->hif_node;
//    return hif_info->ars;
//}
//void  wf_os_api_set_ars(void *nic_info,void *ars)
//{
//    nic_info_st * ni        = nic_info;
//    hif_node_t *hif_info   = ni->hif_node;
//    hif_info->ars           = ars;
//}
//
//
//void *wf_os_api_get_odm(void *nic_info)
//{
//    nic_info_st * ni        = nic_info;
//    hif_node_t *hif_info   = ni->hif_node;
//    return hif_info->odm;
//}
//void  wf_os_api_set_odm(void *nic_info,void *odm)
//{
//    nic_info_st * ni        = nic_info;
//    hif_node_t *hif_info   = ni->hif_node;
//    hif_info->odm           = odm;
//}


wf_s32 wf_os_api_get_cpu_id(void)
{
    bsp_cpu_id_t cpu_id;

    bsp_cpu_id_get(&cpu_id);

    return cpu_id.w0;
}

