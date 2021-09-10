
#include "common.h"
#include "tx_rtos.h"
#include "wlan_rtos.h"

#if 1
#define WLAN_DBG(fmt, ...)      OS_LOG_D("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define WLAN_DBG(fmt, ...)
#endif
#define WLAN_INFO(fmt, ...)     OS_LOG_I("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WLAN_WARN(fmt, ...)     OS_LOG_W("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WLAN_ERROR(fmt, ...)    OS_LOG_E("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

static OS_RET wlan_init (wlan_dev_t *wdev)
{
    wlan_priv_t *wlan_priv = wlan_dev_priv(wdev);
    nic_info_st *pnic_info = wlan_priv->nic;
    hw_info_st *hw_info;

    WLAN_DBG();

    if (nic_init(pnic_info) != WF_RETURN_OK)
    {
        return -OS_RERROR;
    }

    tx_work_init(wdev);

    hw_info = pnic_info->hw_info;
    if (wf_80211_is_zero_addr(hw_info->macAddr))
    {
        WLAN_ERROR("invalid mac addr: " WF_MAC_FMT, WF_MAC_ARG(hw_info->macAddr));
        return -OS_RERROR;
    }
    WLAN_INFO("efuse_macaddr: " WF_MAC_FMT, WF_MAC_ARG(hw_info->macAddr));

	wdev->event_queue = osMessageQueueNew(1,4,NULL);
    return OS_ROK;
}

OS_RET wlan_uninit (wlan_dev_t *wdev)
{
	osMessageQueueDelete(wdev->event_queue);
    return OS_ROK;
}

static OS_RET wlan_open (wlan_dev_t *wdev)
{
    wlan_priv_t *wlan_priv = wlan_dev_priv(wdev);
    nic_info_st *pnic_info = wlan_priv->nic;

    if (OS_WARN_ON(!pnic_info))
    {
        return -OS_RERROR;
    }

    if (nic_enable(pnic_info) == WF_RETURN_FAIL)
    {
        return -OS_RERROR;
    }

    return OS_ROK;
}

static OS_RET wlan_close (struct wlan_dev_ *wdev)
{
    return OS_ROK;
}

static OS_RET wlan_send(wlan_dev_t *wdev, void *data, OS_U32 len)
{
    wlan_priv_t *wlan_priv = wlan_dev_priv(wdev);
    struct pbuf *p = data;

    if (OS_WARN_ON(!data || !len))
    {
        return -OS_RINVAL;
    }

    if (WF_CANNOT_RUN(wlan_priv->nic))
    {
        return -OS_RIO;
    }

    if (!wlan_priv->nic->tx_info)
    {
        return -OS_RIO;
    }

    if (wf_mlme_check_mode(wlan_priv->nic, WF_MONITOR_MODE) == wf_true)
    {
        //work_monitor_tx_entry(pnetdev, (struct sk_buff *)pkt);
    }
    else
    {
        if (wf_false == wf_tx_data_check(wlan_priv->nic))
        {
            ;
        }
        else
        {
            wf_u8 *frame;
            /* tx resource check */
            if (wf_need_stop_queue(wlan_priv->nic) == wf_true)
            {
                WLAN_WARN(">>>>wdev dev tx stop queue");
                tx_work_wake(wdev);

                return -OS_RIO;
            }

            /* actually xmit */
            frame = osZMalloc(p->tot_len);
            if (OS_WARN_ON(!frame))
            {
                return -OS_RNOMEM;
            }
            os_memcpy(frame, data, len);
            /*copy pbuf -> data dat*/
//            pbuf_copy_partial(p, frame, p->tot_len, 0);
            if (wf_tx_msdu(wlan_priv->nic, frame, p->tot_len, frame))
            {
                osFree(frame);
                return -OS_RERROR;
            }
            else
            {
                tx_work_wake(wdev);
            }
        }
    }

    return OS_ROK;
}

static OS_RET wlan_mode (wlan_dev_t *wdev, wlan_dev_mode_t mode)
{
    wlan_priv_t *wlan_priv = wlan_dev_priv(wdev);
    nic_info_st *pnic_info = wlan_priv->nic;
    sys_work_mode_e cur_mode;
    wf_bool bConnect;
    wf_wlan_mgmt_info_t *wlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *cur_network = &wlan_mgmt_info->cur_network;

    WLAN_DBG("mode: %d", mode);
    switch(mode)
    {
        case WLAN_DEV_MODE_STATION:
            cur_mode = WF_INFRA_MODE;
            break;
        case WLAN_DEV_MODE_AP:
            cur_mode = WF_MASTER_MODE;
            break;
        default:
            break;
    }

    wf_local_cfg_set_work_mode(pnic_info, cur_mode);

    wf_mlme_get_connect(pnic_info, &bConnect);
    if (bConnect)
    {
        wf_mlme_deauth(pnic_info, wf_true);
    }

    wf_mcu_set_op_mode(pnic_info, cur_mode);

    switch(cur_mode)
    {
        case WF_MASTER_MODE :
            cur_network->join_res = -1;
            wf_mlme_abort(pnic_info);
            break;

        case WF_MONITOR_MODE :
        case WF_INFRA_MODE :
        case WF_AUTO_MODE :
        default :
            break;
    }

    return OS_ROK;
}

static OS_RET wlan_scan(wlan_dev_t *wdev, wlan_dev_scan_info_t *scan_info)
{
    wlan_priv_t *wlan_priv = wlan_dev_priv(wdev);
    nic_info_st *pnic_info = wlan_priv->nic;
	wlan_event_e event;
    wf_bool is_connected;

    wf_mlme_get_connect(pnic_info, &is_connected);
    if (is_connected)
    {
        wf_bool is_busy;
        wf_mlme_get_traffic_busy(pnic_info, &is_busy);
        if (is_busy)
        {
            return -OS_RBUSY;
        }
        wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                           NULL, 0, NULL, 0, 0);
    }
    else
    {
        if (scan_info)
        {
            if (scan_info->ssid.len)
            {
                wf_wlan_ssid_t ssids[1];
                if (!OS_WARN_ON(scan_info->ssid.len >= sizeof(ssids[0].data)))
                {
                    wf_memcpy(ssids[0].data, scan_info->ssid.val,
                              scan_info->ssid.len);
                    ssids[0].data[scan_info->ssid.len] = '\0';
                    ssids[0].length = scan_info->ssid.len;
                    WLAN_DBG("ssid = %s", ssids[0].data);
                }
                /* start scan with ssid */
                wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                                   ssids, OS_ARRAY_SIZE(ssids),
                                   NULL, 0, 0);
            }
        }
        else
        {
            /* start scan */
            wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                               NULL, 0, NULL, 0, 0);
        }
    }
	wf_os_api_ind_scan_done(pnic_info, 0, 0);
	event = WLAN_SCAN_DONE;
	osMessageQueuePut(wdev->event_queue, &event, 0, 0);
    return OS_ROK;
}

static OS_RET wlan_scan_stop(wlan_dev_t *wdev)
{
#if 0
    wlan_priv_t *wlan_priv = wdev->priv;
    nic_info_st *pnic_info = wlan_priv->nic;

    wf_scan_stop(pnic_info);
#endif
    return OS_ROK;
}

static OS_RET wlan_join(wlan_dev_t *wdev, wlan_dev_sta_info_t *sta_info)
{
#if 0
    wlan_priv_t *wlan_priv = wdev->priv;
    nic_info_st *pnic_info = wlan_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;
    hw_info_st *phw_info = pnic_info->hw_info;
    wf_u8 len;
    ieee_param *param = NULL;
    wf_u32 param_len;
    if (wf_local_cfg_get_work_mode(pnic_info) == WF_MASTER_MODE)
    {
        WLAN_ERROR("ap no support wf_wlan_join");
        return -RT_ERROR;
    }

    param_len = WF_OFFSETOF(ieee_param, u.crypt.key) + sta_info->key.len;
    param = (ieee_param *)wf_vmalloc(param_len);
    if (param == NULL)
    {
        return -1;
    }

    memset(param->sta_addr, 0xff, WF_ETH_ALEN);
    WLAN_DBG("sta_info->security:%x", sta_info->security);
    switch (sta_info->security & 0xffff)
    {
        case WEP_ENABLED:
            strncpy((char *)param->u.crypt.alg, "WEP", IEEE_CRYPT_ALG_NAME_LEN);
            param->u.crypt.set_tx = 1;
            break;
        case AES_ENABLED:
            wf_memcpy((char *)param->u.crypt.alg, "ccmp", IEEE_CRYPT_ALG_NAME_LEN);
            Wpa_Supplicant_Config(pnic_info, (const char *)phw_info->macAddr, WF_ETH_ALEN, (char *)sta_info->ssid.val, (char *)sta_info->key.val, 0);
            break;
        case TKIP_ENABLED:
            wf_memcpy((char *)param->u.crypt.alg, "tkip", IEEE_CRYPT_ALG_NAME_LEN);
            WLAN_DBG("KEY:%s", sta_info->key.val);
            Wpa_Supplicant_Config(pnic_info, (const char *)phw_info->macAddr, WF_ETH_ALEN, (char *)sta_info->ssid.val, (char *)sta_info->key.val, 0);
            WLAN_DBG("KEY:%s", sta_info->key.val);
            break;
        default:
            wf_memcpy((char *)param->u.crypt.alg, "none", IEEE_CRYPT_ALG_NAME_LEN);
            break;
    }

    param->u.crypt.key_len = sta_info->key.len;

    memcpy(param->u.crypt.key, sta_info->key.val, sta_info->key.len);
    param->u.crypt.idx = 0;
    wf_wlan_encryption(pnic_info, param, param_len);

    len = sta_info->ssid.len;
    if (len > 32)
    {
        WLAN_ERROR("ssid length %d too long", len);
        return -RT_ERROR;
    }

    if (len == 32)
    {
        WLAN_DBG("clear essid");
        return OS_ROK;
    }

    if (sta_info->ssid.len)
    {
        wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
        wf_wlan_mgmt_scan_que_node_t *pscan_que_node;
        wf_wlan_ssid_t ssid;
        wf_u8 *pbssid = NULL;

        wf_memcpy(ssid.data, sta_info->ssid.val, ssid.length = len);
        ssid.data[ssid.length] = '\0';

        wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
        {
            if (wf_wlan_is_same_ssid(&pscan_que_node->ssid, &ssid))
            {
                pbssid = pscan_que_node->bssid;
                if (pscan_que_node->wpa_ie != NULL)
                {
                    wf_wlan_set_wpa_ie(pnic_info, pscan_que_node->wpa_ie, sizeof(pscan_que_node->wpa_ie));
                }
                else if (pscan_que_node->rsn_ie != NULL)
                {
                    wf_wlan_set_wpa_ie(pnic_info, pscan_que_node->rsn_ie, sizeof(pscan_que_node->rsn_ie));
                }
                else
                {
                    WLAN_ERROR("IE NULL");
                    return -1;
                }
                break;
            }
        }
        wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);

        if (scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL)
        {
            WLAN_WARN("scan queue for each fail");
            return -RT_ERROR;
        }
        else
        {
            wf_bool is_connected;
            wf_mlme_get_connect(pnic_info, &is_connected);
            if (is_connected)
            {
                if (pbssid &&
                    wf_80211_is_same_addr(wf_wlan_get_cur_bssid(pnic_info), pbssid))
                {
                    WLAN_DBG("the essid as same as the current associate ssid");
                    wf_os_api_ind_connect(pnic_info, WF_MLME_FRAMEWORK_WEXT);
                    return OS_ROK;
                }
            }

            /* start connection */
            wf_mlme_conn_start(pnic_info, pbssid, &ssid,
                               WF_MLME_FRAMEWORK_WEXT, wf_true);
        }
    }
#endif
    return OS_ROK;
}

static OS_RET wlan_disconnect(wlan_dev_t *wdev)
{
#if 0
    wlan_priv_t *wlan_priv = wdev->priv;
    nic_info_st *pnic_info = wlan_priv->nic;

    wf_mlme_deassoc(pnic_info);
#endif
    return OS_ROK;
}

static OS_RET wlan_ap_start(wlan_dev_t *wdev, wlan_dev_ap_info_t *ap_info)
{
    return OS_ROK;
}

static OS_RET wlan_ap_stop(wlan_dev_t *wdev)
{
    return OS_ROK;
}

static OS_RET wlan_ap_deauth(wlan_dev_t *wdev, OS_U8 mac[])
{
    return OS_ROK;
}

static int wlan_get_rssi(wlan_dev_t *wdev)
{
    return OS_ROK;
}

static OS_RET wlan_set_powersave(wlan_dev_t *wdev, int level)
{
    return OS_ROK;
}

static int wlan_get_powersave (wlan_dev_t *wdev)
{
    return 0;
}

static OS_RET wlan_set_channel(wlan_dev_t *wdev, int channel)
{
    wlan_priv_t *wlan_priv = wlan_dev_priv(wdev);
    nic_info_st *pnic_info = wlan_priv->nic;
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    /* update channel, but no with bw */
    pcur_network->channel = channel;
    wf_hw_info_set_channnel_bw(pnic_info,
                               pcur_network->channel,
                               pcur_network->bw,
                               HAL_PRIME_CHNL_OFFSET_DONT_CARE);

    return OS_ROK;
}

static int wlan_get_channel(wlan_dev_t *wdev)
{
    wlan_priv_t *wlan_priv = wlan_dev_priv(wdev);
    nic_info_st *pnic_info = wlan_priv->nic;
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    return pcur_network->channel;
}

static OS_RET wlan_set_country(wlan_dev_t *wdev,
                               wlan_dev_country_code_t country_code)
{
    wlan_priv_t *wlan_priv = wlan_dev_priv(wdev);
    nic_info_st *pnic_info = wlan_priv->nic;
    hw_info_st *phw_info = pnic_info->hw_info;

    /* update channel plan */
    phw_info->channel_plan = country_code;
    if (channel_init(pnic_info))
    {
        return -OS_RERROR;
    }

    return OS_ROK;
}

static wlan_dev_country_code_t wlan_get_country(wlan_dev_t *wdev)
{
    wlan_priv_t *wlan_priv = wlan_dev_priv(wdev);
    nic_info_st *pnic_info = wlan_priv->nic;
    hw_info_st *phw_info = pnic_info->hw_info;

    return (wlan_dev_country_code_t)phw_info->channel_plan;
}

static OS_RET wlan_set_mac(wlan_dev_t *wdev, OS_U8 mac[])
{
    return OS_ROK;
}

static OS_RET wlan_get_mac(wlan_dev_t *wdev, OS_U8 mac[])
{
    wlan_priv_t *wlan_priv = wlan_dev_priv(wdev);
    nic_info_st *pnic_info = wlan_priv->nic;
    hw_info_st *phw_info = pnic_info->hw_info;

    wf_memcpy(mac, phw_info->macAddr, sizeof(phw_info->macAddr));

    return OS_ROK;
}

const static wlan_dev_ops_t ops =
{
    .init               = wlan_init,
    .uninit             = wlan_uninit,
    .open               = wlan_open,
    .close              = wlan_close,
    .send               = wlan_send,
    .mode               = wlan_mode,
    .scan               = wlan_scan,
    .scan_stop          = wlan_scan_stop,
    .join               = wlan_join,
    .disconnect         = wlan_disconnect,
    .ap_start           = wlan_ap_start,
    .ap_stop            = wlan_ap_stop,
    .ap_deauth          = wlan_ap_deauth,
    .get_rssi           = wlan_get_rssi,
    .set_powersave      = wlan_set_powersave,
    .get_powersave      = wlan_get_powersave,
    .set_channel        = wlan_set_channel,
    .get_channel        = wlan_get_channel,
    .set_country        = wlan_set_country,
    .get_country        = wlan_get_country,
    .set_mac            = wlan_set_mac,
    .get_mac            = wlan_get_mac,
};

OS_RET wlan_register (nic_info_st *pnic_info, const char *name)
{
    wlan_dev_t *wdev;
    wlan_priv_t *wdev_priv;

    wdev = wlan_dev_new(&ops, sizeof(*wdev_priv));
    if (!wdev)
    {
        return -OS_RNOMEM;
    }
    os_strcpy(wdev->name, name);
    wdev_priv = wlan_dev_priv(wdev);
    wdev_priv->nic = pnic_info;
    pnic_info->ndev = wdev;

    /* register wdev device */
    return wlan_dev_register(wdev);
}

OS_RET wlan_unregister(nic_info_st *pnic_info)
{
    if (OS_WARN_ON(!pnic_info->ndev))
    {
        return -OS_RERROR;
    }

    WLAN_DBG("[%d]", pnic_info->ndev_id);
    wlan_dev_unregister(pnic_info->ndev);
    pnic_info->ndev = NULL;

    return OS_ROK;
}

