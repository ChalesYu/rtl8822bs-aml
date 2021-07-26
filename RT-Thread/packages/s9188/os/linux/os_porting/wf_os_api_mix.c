
/* include */
#include "common.h"
#ifdef CONFIG_IOCTL_CFG80211
#include "wf_cfg80211.h"
#endif
#include "hif.h"
#include "wf_debug.h"

/* macro */

/* type */

/* function declaration */

void wf_os_api_ind_scan_done (void *arg, wf_bool arg1, wf_u8 arg2)
{
    nic_info_st *pnic_info = arg;
#ifdef CONFIG_IOCTL_CFG80211
    wf_u8 babort = arg1;
#endif
    wf_mlme_framework_e framework = arg2;

#ifdef CONFIG_WIRELESS_EXT
    if (framework == WF_MLME_FRAMEWORK_WEXT)
    {
        union iwreq_data wrqu;

        wf_memset(&wrqu, 0, sizeof(union iwreq_data));
        wireless_send_event(pnic_info->ndev, SIOCGIWSCAN, &wrqu, NULL);
    }
#endif
#ifdef CONFIG_IOCTL_CFG80211
    if (framework == WF_MLME_FRAMEWORK_NETLINK)
    {
        wf_cfg80211_scan_complete(pnic_info);
        wf_cfg80211_scan_done_event_up(pnic_info, babort);
    }
#endif
}

void wf_os_api_ind_connect (void *arg, wf_u8 arg1)
{
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
}

void wf_os_api_ind_disconnect (void *arg, wf_u8 arg1)
{
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
}

#ifdef CFG_ENABLE_ADHOC_MODE
void wf_os_api_cfg80211_unlink_ibss(void *arg)
{
#ifdef CONFIG_IOCTL_CFG80211
    nic_info_st *pnic_info = arg;
    wf_cfg80211_unlink_ibss(pnic_info);
#endif
}
#endif


#ifdef CFG_ENABLE_AP_MODE
void wf_os_api_ap_ind_assoc (void *arg, void *arg1, void *arg2, wf_u8 arg3)
{
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
}

void wf_os_api_ap_ind_disassoc (void *arg, void *arg1, wf_u8 arg2)
{
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
}
#endif


void wf_os_api_enable_all_data_queue (void *arg)
{
    struct net_device *ndev = arg;

    netif_carrier_on(ndev);

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,35))
    netif_tx_start_all_queues(ndev);
#else
    netif_start_queue(ndev);
#endif
}

void wf_os_api_disable_all_data_queue (void *arg)
{
    struct net_device *ndev = arg;

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,35))
    netif_tx_stop_all_queues(ndev);
#else
    netif_stop_queue(ndev);
#endif

    netif_carrier_off(ndev);
    LOG_W("The netif carrier off");
}

wf_u32 wf_os_api_rand32 (void)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
    return prandom_u32();
#elif (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18))
    u32 random_int;
    get_random_bytes(&random_int, 4);
    return random_int;
#else
    return random32();
#endif
}

void *wf_os_api_get_ars(void *nic_info)
{
    nic_info_st * ni        = nic_info;
    hif_node_st *hif_info   = ni->hif_node;
    return hif_info->ars;
}
void  wf_os_api_set_ars(void *nic_info,void *ars)
{
    nic_info_st * ni        = nic_info;
    hif_node_st *hif_info   = ni->hif_node;
    hif_info->ars           = ars;
}


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
    return smp_processor_id();
}

