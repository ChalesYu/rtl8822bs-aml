
/* include */
#include "common.h"
#include "wifi/wf_wifi.h"

/* macro */

/* type */

/* function declaration */

extern
wf_wifi_sec_t wf_wifi_sec_type_parse(wf_wlan_mgmt_scan_que_node_t *pscan_que_node);

void wf_os_api_ind_scan_done (void *arg, wf_bool arg1, wf_u8 arg2)
{
    nic_info_st *pnic_info = arg;
    wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
    wf_wlan_mgmt_scan_que_node_t *pscan_que_node;

    WF_UNUSED(arg1);
    WF_UNUSED(arg2);

    /* mark scan done */
    nic_priv(pnic_info)->scan.done = wf_true;

    /* check if should report scan result */
    if (!nic_priv(pnic_info)->scan.report_en)
    {
        return;
    }

    /* report scan result */
    wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
    {
        wf_wifi_scan_report_t scan_rpo;
        wf_u32 max_rate = 0, rate, i = 0;

        /* clearup scan report */
        wf_memset(&scan_rpo, 0, sizeof(scan_rpo));

        /* ssid */
        wf_memcpy(scan_rpo.ssid.data,
                  pscan_que_node->ssid.data,
                  pscan_que_node->ssid.length);
        scan_rpo.ssid.len = pscan_que_node->ssid.length;
        /* hidden */
        scan_rpo.hidden =
            (wf_bool)(pscan_que_node->ssid_type == WF_80211_HIDDEN_SSID_NOT_IN_USE);
        /* bssid */
        wf_memcpy(scan_rpo.bssid, pscan_que_node->bssid, WF_ETH_ALEN);
        /* sec */
        scan_rpo.sec = wf_wifi_sec_type_parse(pscan_que_node);
        /* channel */
        scan_rpo.channel = pscan_que_node->channel;
        /* rate */
        while (pscan_que_node->spot_rate[i] != 0)
        {
            rate = pscan_que_node->spot_rate[i] & 0x7F;
            if (rate > max_rate)
            {
                max_rate = rate;
            }
            i++;
        }
        if (pscan_que_node->mcs & 0x8000) /* MCS15 */
        {
            max_rate = pscan_que_node->bw_40mhz ?
                       (pscan_que_node->short_gi ? 300 : 270) :
                       (pscan_que_node->short_gi ? 144 : 130);
        }
        else /* default MCS7 */
        {
            max_rate = (pscan_que_node->bw_40mhz) ?
                       (pscan_que_node->short_gi ? 150 : 135) :
                       (pscan_que_node->short_gi ? 72 : 65);
        }
        max_rate = max_rate * 1000000;
        scan_rpo.datarate = max_rate;
        /* band */
        scan_rpo.band = WF_WLAN_BAND_2_4GHZ;
        /* rssi */
        scan_rpo.rssi = pscan_que_node->signal_strength_scale;

        /* report bss information */
        nic_priv(pnic_info)->ops->scan_report(pnic_info, &scan_rpo);
    }
    wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);

    /* scan done */
    nic_priv(pnic_info)->ops->scan_done(pnic_info);
}

void wf_os_api_ind_connect (void *arg, wf_u8 arg1)
{
    nic_info_st *pnic_info = arg;
    wdn_net_info_st *pwdn_info = NULL;
    wf_wifi_conn_info_t conn_info;
    sys_work_mode_e work_mode;

    WF_UNUSED(arg1);

    work_mode = get_sys_work_mode(pnic_info);
    if (work_mode == WF_INFRA_MODE)
    {
        pwdn_info = wf_wdn_find_info(pnic_info, wf_wlan_get_cur_bssid(pnic_info));

        conn_info.channel = pwdn_info->channel;
        conn_info.ssid.len = pwdn_info->ssid_len;
        wf_memcpy(conn_info.ssid.data, pwdn_info->ssid, pwdn_info->ssid_len);
        wf_memcpy(conn_info.bssid, pwdn_info->bssid, WF_ETH_ALEN);
        /* connect done */
        nic_priv(pnic_info)->ops->conn_done(pnic_info, &conn_info);
    }
}

void wf_os_api_ind_disconnect (void *arg, wf_u8 arg1)
{
    nic_info_st *pnic_info = arg;
    WF_UNUSED(arg1);

    nic_priv(pnic_info)->ops->disconn_evt(pnic_info);
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
    nic_info_st *pnic_info = arg;
    wdn_net_info_st *pwdn_info = arg1;

    WF_UNUSED(arg2);
    WF_UNUSED(arg3);

    nic_priv(pnic_info)->ops->assoc_evt(pwdn_info);
}

void wf_os_api_ap_ind_disassoc (void *arg, void *arg1, wf_u8 arg2)
{
    nic_info_st *pnic_info = arg;
    wdn_net_info_st *pwdn_info = arg1;

    WF_UNUSED(arg2);

    nic_priv(pnic_info)->ops->disassoc_evt(pwdn_info);
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

void wf_os_api_thrd_cfg_get (const char *name,
                             wf_u32 *rpriority, wf_u32 *rtask_size)
{
    const static struct
    {
        const char *name;
        wf_u32 stack_size; /* bytes */
    } thrd_cfg[] =
    {
        { "xmit_00",        500  },
        { "hif_tx",         805  },

        { "hif_rx",         2065 },

        { "wlan_mgmt_00",   500  },
        { "wlan_mgmt_01",   500  },

        { "mlme_00",        1075 }, /* highst */
        { "mlme_01",        1075 },

        { "ap_00",          2000 },
        { "ap_01",          2000 },
    };
    wf_u8 i;

    for (i = 0; i < WF_ARRAY_SIZE(thrd_cfg); i++)
    {
        if (!wf_strcmp(name, thrd_cfg[i].name))
        {
            *rpriority = i;
            *rtask_size = thrd_cfg[i].stack_size;
            return;
        }
    }

    /* default set */
    *rpriority = i;
    *rtask_size = 2000;
}


