
#include "common.h"
#include "wf_debug.h"

#if 0
#define AP_DBG(fmt, ...)        LOG_D("[%s]"fmt, __func__, ##__VA_ARGS__)
#define AP_WARN(fmt, ...)       LOG_E("[%s]"fmt, __func__, ##__VA_ARGS__)
#define AP_ARRAY(data, len)     log_array(data, len)
#else
#define AP_DBG(fmt, ...)
#define AP_WARN(fmt, ...)
#define AP_ARRAY(data, len)
#endif

#ifdef CFG_ENABLE_AP_MODE
inline void wf_ap_status_set (nic_info_st *pnic_info, wf_ap_status ap_state)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    pcur_network->ap_state = ap_state;
}

inline wf_ap_status wf_ap_status_get (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    return pcur_network->ap_state;
}

static int ap_hw_set_unicast_key (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_u8 cam_id;
    wf_u16 ctrl;
    int ret;

    cam_id = pwdn_info->wdn_id + 4;

    AP_DBG("cam_id == %d", cam_id);
    ctrl = BIT(15) | (pwdn_info->dot118021XPrivacy << 2);
    ret = wf_mcu_set_sec_cam(pnic_info,
                             cam_id, ctrl,
                             pwdn_info->mac, pwdn_info->dot118021x_UncstKey.skey);

    return ret;

}

static int ap_hw_set_group_key (nic_info_st *pnic_info, wf_u8 *pmac)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u8 cam_id;
    wf_u16 ctrl;
    wf_u32 buf[2];
    int ret;

    cam_id = psec_info->dot118021XGrpKeyid & 0x03; /* cam_id0~3 8021x group key */
    ctrl = BIT(15) | BIT(6) | BIT(7) |
           (psec_info->dot118021XGrpPrivacy << 2) |
           psec_info->dot118021XGrpKeyid;
    ret = wf_mcu_set_sec_cam(pnic_info, cam_id, ctrl, pmac,
                             psec_info->dot118021XGrpKey[psec_info->dot118021XGrpKeyid].skey);

    return ret;
}

int wf_ap_msg_load (nic_info_st *pnic_info, wf_que_t *pque_tar,
                    wf_ap_msg_tag_e tag, void *pdata, wf_u16 len)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_que_t *pque;
    wf_que_list *pnode;
    wf_ap_msg_t *pmsg;

    /* pop message from queue free */
    pque = &pwlan_info->cur_network.ap_msg_free[tag];
    if ((pnode = wf_deque_head(pque)) == NULL)
    {
        AP_WARN("queue empty");
        return -1;
    }

    /* fill message */
    pmsg = WF_CONTAINER_OF(pnode, wf_ap_msg_t, list);
    pmsg->tag = tag;
    pmsg->len = len;
    if (pdata && len)
    {
        wf_memcpy(&pmsg->mgmt, pdata, len);
    }

    /* message push into queue load */
    wf_enque_tail(pnode, pque_tar);

    return 0;
}

wf_ap_msg_t *wf_ap_msg_get (wf_que_t *pque)
{
    wf_que_list *pnode_list;

    if (wf_que_is_empty(pque))
    {
        return NULL;
    }

    pnode_list = wf_list_next(wf_que_head(pque));
    return WF_CONTAINER_OF(pnode_list, wf_ap_msg_t, list);
}


int wf_ap_msg_free (nic_info_st *pnic_info, wf_que_t *pque, wf_ap_msg_t *pmsg)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;

    /* pop message from load queue */
    if (wf_deque(&pmsg->list, pque) == NULL)
    {
        AP_WARN("queue empty");
        return -1;
    }

    /* push the message back into free queue */
    wf_enque_head(&pmsg->list, &pwlan_info->cur_network.ap_msg_free[pmsg->tag]);

    return 0;
}

static
void ap_msg_que_clearup (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_que_t *pque = &pwdn_info->ap_msg;
    wf_ap_msg_t *pmsg;

    while (wf_true)
    {
        pmsg = wf_ap_msg_get(pque);
        if (pmsg)
        {
            wf_ap_msg_free(pnic_info, pque, pmsg);
        }
        else
        {
            break;
        }
    }
}

static int ap_msg_que_init (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_u8 i;
    wf_que_t *pque;
    wf_ap_msg_t *pnode;

    AP_DBG();

    /* beacon queue */
#define WF_ADHOC_MSG_BCN_QUE_DEEP     (WDN_NUM_MAX * 3)
#define WF_ADHOC_MSG_BCN_SIZE_MAX     (WF_OFFSETOF(wf_ap_msg_t, mgmt) + \
                                     WF_80211_MGMT_BEACON_SIZE_MAX)
    pque = &pwlan_info->cur_network.ap_msg_free[WF_AP_MSG_TAG_BEACON_FRAME];
    wf_que_init(pque, WF_LOCK_TYPE_SPIN);
    for (i = 0; i < WF_ADHOC_MSG_BCN_QUE_DEEP; i++)
    {
        pnode = wf_kzalloc(WF_ADHOC_MSG_BCN_SIZE_MAX);
        if (pnode == NULL)
        {
            AP_WARN("malloc failed");
            return -1;
        }
        wf_enque_head(&pnode->list, pque);
    }

    /* auth queue */
#define WF_AP_MSG_AUTH_QUE_DEEP     (WDN_NUM_MAX * 3)
#define WF_AP_MSG_AUTH_SIZE_MAX     (WF_OFFSETOF(wf_ap_msg_t, mgmt) + \
                                     WF_80211_MGMT_AUTH_SIZE_MAX)
    pque = &pwlan_info->cur_network.ap_msg_free[WF_AP_MSG_TAG_AUTH_FRAME];
    wf_que_init(pque, WF_LOCK_TYPE_SPIN);
    for (i = 0; i < WF_AP_MSG_AUTH_QUE_DEEP; i++)
    {
        pnode = wf_kzalloc(WF_AP_MSG_AUTH_SIZE_MAX);
        if (pnode == NULL)
        {
            AP_WARN("malloc failed");
            return -1;
        }
        wf_enque_head(&pnode->list, pque);
    }

    /* deauth queue */
#define WF_AP_MSG_DEAUTH_QUE_DEEP   WDN_NUM_MAX
#define WF_AP_MSG_DEAUTH_SIZE_MAX   (WF_OFFSETOF(wf_ap_msg_t, mgmt) + \
                                     WF_80211_MGMT_DEAUTH_SIZE_MAX)
    pque = &pwlan_info->cur_network.ap_msg_free[WF_AP_MSG_TAG_DEAUTH_FRAME];
    wf_que_init(pque, WF_LOCK_TYPE_SPIN);
    for (i = 0; i < WF_AP_MSG_DEAUTH_QUE_DEEP; i++)
    {
        pnode = wf_kzalloc(WF_AP_MSG_DEAUTH_SIZE_MAX);
        if (pnode == NULL)
        {
            AP_WARN("malloc failed");
            return -1;
        }
        wf_enque_head(&pnode->list, pque);
    }

    /* asoc queue */
#define WF_AP_MSG_ASSOC_REQ_QUE_DEEP    WDN_NUM_MAX
#define WF_AP_MSG_ASSOC_REQ_SIZE_MAX    (WF_OFFSETOF(wf_ap_msg_t, mgmt) + \
                                         WF_80211_MGMT_ASSOC_SIZE_MAX)
    pque = &pwlan_info->cur_network.ap_msg_free[WF_AP_MSG_TAG_ASSOC_REQ_FRAME];
    wf_que_init(pque, WF_LOCK_TYPE_SPIN);
    for (i = 0; i < WF_AP_MSG_ASSOC_REQ_QUE_DEEP; i++)
    {
        pnode = wf_kzalloc(WF_AP_MSG_ASSOC_REQ_SIZE_MAX);
        if (pnode == NULL)
        {
            AP_WARN("malloc failed");
            return -2;
        }
        wf_enque_head(&pnode->list, pque);
    }

    /* disassoc queue */
#define WF_AP_MSG_DISASSOC_QUE_DEEP   WDN_NUM_MAX
#define WF_AP_MSG_DISASSOC_SIZE_MAX   (WF_OFFSETOF(wf_ap_msg_t, mgmt) + \
                                       WF_80211_MGMT_DISASSOC_SIZE_MAX)
    pque = &pwlan_info->cur_network.ap_msg_free[WF_AP_MSG_TAG_DISASSOC_FRAME];
    wf_que_init(pque, WF_LOCK_TYPE_SPIN);
    for (i = 0; i < WF_AP_MSG_DISASSOC_QUE_DEEP; i++)
    {
        pnode = wf_kzalloc(WF_AP_MSG_DISASSOC_SIZE_MAX);
        if (pnode == NULL)
        {
            AP_WARN("malloc failed");
            return -1;
        }
        wf_enque_head(&pnode->list, pque);
    }

    return 0;
}

static int ap_msg_deinit (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_u8 i;
    wf_que_t *pque;
    wf_ap_msg_t *pap_msg;
    wf_que_list *pnode;
    wf_list_t *pos, *pos_next;
    wdn_list *pwdn = (wdn_list *)pnic_info->wdn;
    wdn_net_info_st *pwdn_info;
    wdn_node_st *pwdn_node;

    /* free message queue in cur_network */
    AP_DBG("free ap_msg_free");
    for (i = 0; i < WF_AP_MSG_TAG_MAX; i++)
    {
        pque = &pwlan_info->cur_network.ap_msg_free[i];
        while ((pnode = wf_deque_head(pque)))
        {
            pap_msg = WF_CONTAINER_OF(pnode, wf_ap_msg_t, list);
            wf_kfree(pap_msg);
        }
    }

    /* free message queue in wdn_info */
    AP_DBG("free wdn_ap_msg");
    wf_list_for_each_safe(pos, pos_next, &pwdn->head)
    {
        pwdn_node = wf_list_entry(pos, wdn_node_st, list);
        pwdn_info = &pwdn_node->info;
        if (pwdn_info->mode == WF_MASTER_MODE)
        {
            /* free message queue */
            pque = &pwdn_info->ap_msg;
            while ((pnode = wf_deque_head(pque)))
            {
                pap_msg = WF_CONTAINER_OF(pnode, wf_ap_msg_t, list);
                wf_kfree(pap_msg);
            }
            if(!MacAddr_isBcst(pwdn_info->mac))
            {
                wf_deauth_xmit_frame(pnic_info, pwdn_info->mac,
                              WF_80211_REASON_QSTA_TIMEOUT);
            }
            /* free the wdn */
            wf_wdn_remove(pnic_info, pwdn_info->mac);
        }
    }
    wf_mcu_set_media_status(pnic_info, WIFI_FW_STATION_STATE);

    return 0;
}

static
wf_pt_rst_t maintain_thrd (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_pt_t *pt = &pwdn_info->sub_thrd_pt;
    wf_ap_msg_t *pmsg;

    PT_BEGIN(pt);

    AP_DBG("established->"WF_MAC_FMT, WF_MAC_ARG(pwdn_info->mac));

    /* state set to established */
    pwdn_info->state =
        psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X ?
        E_WDN_AP_STATE_8021X_BLOCK : E_WDN_AP_STATE_8021X_UNBLOCK;
    wf_ap_status_set(pnic_info, WF_AP_STATUS_ESTABLISHED);

    /* initilize rx packet statistic */
    pwdn_info->rx_pkt_stat_last = pwdn_info->rx_pkt_stat;
    pwdn_info->rx_idle_timeout = 0;
    wf_timer_set(&pwdn_info->ap_timer, 1000);

    for (;;)
    {
        PT_WAIT_UNTIL(pt, (pmsg = wf_ap_msg_get(&pwdn_info->ap_msg)) ||
                           wf_timer_expired(&pwdn_info->ap_timer));
        /* one second timeon */
        if (pmsg == NULL)
        {
            wf_timer_reset(&pwdn_info->ap_timer);
            /* todo: compare packet statistics with last value, if equal mains
            current connection has idle during the last one second */
            if (pwdn_info->rx_pkt_stat != pwdn_info->rx_pkt_stat_last)
            {
                pwdn_info->rx_pkt_stat_last = pwdn_info->rx_pkt_stat;
                continue;
            }

            /* idle timeout, force break the current connection */
#define RX_IDLE_TIMEOUT_SECONDS     (20 * 60) /* 20 minutes */
            if (pwdn_info->rx_idle_timeout++ >= RX_IDLE_TIMEOUT_SECONDS)
            {
                AP_WARN("STA timeout idle->"WF_MAC_FMT, WF_MAC_ARG(pwdn_info->mac));
                /* send deauth frame to STA */
                wf_deauth_xmit_frame(pnic_info, pwdn_info->mac,
                                  WF_80211_REASON_QSTA_TIMEOUT);
                break;
            }
        }

        else if (pmsg->tag == WF_AP_MSG_TAG_AUTH_FRAME)
        {
            AP_DBG("reauth->"WF_MAC_FMT, WF_MAC_ARG(pwdn_info->mac));
            break;
        }

        else if (pmsg->tag == WF_AP_MSG_TAG_DEAUTH_FRAME)
        {
            AP_DBG("deauth->"WF_MAC_FMT, WF_MAC_ARG(pwdn_info->mac));
            /* send deauth frame to STA as respond */
            wf_deauth_xmit_frame(pnic_info, pwdn_info->mac,
                              WF_80211_REASON_DEAUTH_LEAVING);
            break;
        }

        else if (pmsg->tag == WF_AP_MSG_TAG_DISASSOC_FRAME)
        {
            AP_DBG("disassoc->"WF_MAC_FMT, WF_MAC_ARG(pwdn_info->mac));
            /* free message */
            wf_ap_msg_free(pnic_info, &pwdn_info->ap_msg, pmsg);
            /* status 2 timeout */
            wf_timer_set(&pwdn_info->ap_timer, 5 * 1000);
            PT_YIELD_UNTIL(pt, (pmsg = wf_ap_msg_get(&pwdn_info->ap_msg)) ||
                               wf_timer_expired(&pwdn_info->ap_timer));
            if (pmsg && pmsg->tag == WF_AP_MSG_TAG_ASSOC_REQ_FRAME)
            {
                /* back to process assoc */
                pwdn_info->state = E_WDN_AP_STATE_ASSOC;
                /* notify connection break */
                wf_os_api_ap_ind_disassoc(pnic_info, pwdn_info,
                                          WF_MLME_FRAMEWORK_NETLINK);
				wf_ap_odm_disconnect_media_status(pnic_info, pwdn_info);
            }
            break;
        }

        else
        {
            wf_ap_msg_free(pnic_info, &pwdn_info->ap_msg, pmsg);
        }
    }

    PT_END(pt);
}

static void clearup (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wdn_list *pwdn = pnic_info->wdn;

    /* up event to hostapd */
    if (pwdn_info->state > E_WDN_AP_STATE_ASSOC)
    {
        wf_os_api_ap_ind_disassoc(pnic_info, pwdn_info, WF_MLME_FRAMEWORK_NETLINK);
		wf_ap_odm_disconnect_media_status(pnic_info, pwdn_info);
    }

    /* clearup message queue in the wdn */
    ap_msg_que_clearup(pnic_info, pwdn_info);
    /* free the wdn */
    wf_wdn_remove(pnic_info, pwdn_info->mac);

    /* if no any one connection has established, ap status set back to runing */
    if (pwdn->cnt == 0)
    {
        wf_ap_status_set(pnic_info, WF_AP_STATUS_STANBY);
    }
}

#ifdef CFG_ENABLE_ADHOC_MODE
static wf_pt_rst_t adhoc_thrd (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_pt_t *pt = &pwdn_info->ap_thrd_pt;
    wf_pt_rst_t thrd_ret;

    PT_BEGIN(pt);

        /* beacon */
        PT_SPAWN(pt, &pwdn_info->sub_thrd_pt,
                 thrd_ret = wf_beacon_adhoc_thrd(pnic_info, pwdn_info));


    PT_END(pt);
}
#endif

static wf_pt_rst_t ap_thrd (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_pt_t *pt = &pwdn_info->ap_thrd_pt;
    wf_pt_rst_t thrd_ret;

    PT_BEGIN(pt);

    for (;;)
    {
        /* auth */
        PT_SPAWN(pt, &pwdn_info->sub_thrd_pt,
                 thrd_ret = wf_auth_ap_thrd(pnic_info, pwdn_info));
        if (thrd_ret == PT_EXITED)
        {
            break;
        }

        /* assoc */
    assoc_entry:
        PT_SPAWN(pt, &pwdn_info->sub_thrd_pt,
                 thrd_ret = wf_assoc_ap_thrd(pnic_info, pwdn_info));
        if (thrd_ret == PT_EXITED)
        {
            break;
        }

        /* maintain */
        PT_SPAWN(pt, &pwdn_info->sub_thrd_pt,
                 thrd_ret = maintain_thrd(pnic_info, pwdn_info));
        if (pwdn_info->state == E_WDN_AP_STATE_ASSOC)
        {
            goto assoc_entry;
        }
        else
        {
            break;
        }
    }

    clearup(pnic_info, pwdn_info);

    PT_END(pt);
}

static wf_inline int new_boradcast_wdn (nic_info_st *pnic_info)
{
    wdn_net_info_st *pwdn_info;
    wf_u8 bc_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    LOG_D("new_boradcast_wdn!!!!!!!!");
    pwdn_info = wf_wdn_add(pnic_info, bc_addr);
    if (pwdn_info == NULL)
    {
        AP_WARN("alloc bmc wdn error");
        return -1;
    }
    wf_que_init(&pwdn_info->ap_msg, WF_LOCK_TYPE_SPIN);
    pwdn_info->aid = 0;
    pwdn_info->qos_option = 0;
    pwdn_info->state = E_WDN_AP_STATE_8021X_UNBLOCK;
    pwdn_info->ieee8021x_blocked = wf_true;
    if (get_sys_work_mode(pnic_info) == WF_ADHOC_MODE)
    {
        pwdn_info->mode = WF_ADHOC_MODE;

    }
    else if (get_sys_work_mode(pnic_info) == WF_MASTER_MODE)
    {
        pwdn_info->mode = WF_MASTER_MODE;
    }

    /* notify connection establish */
    wf_ap_odm_connect_media_status(pnic_info, pwdn_info);
    wf_ap_add_sta_ratid(pnic_info, pwdn_info);
    return 0;
}

static void ap_poll (nic_info_st *pnic_info)
{
    wdn_list *pwdn = (wdn_list *)pnic_info->wdn;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_list_t *pos, *pos_next;
    wdn_node_st *pwdn_node;
    wdn_net_info_st *pwdn_info;
    wf_ap_msg_t *pmsg;
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    AP_DBG();

    if (new_boradcast_wdn(pnic_info))
    {
        AP_WARN("create boardcast wdn_info fail");
        return;
    }

	wf_os_api_thread_affinity(DEFAULT_CPU_ID);

    /* poll ap thread according to each wdn_info */
    while (wf_os_api_thread_wait_stop(pcur_network->ap_tid) == wf_false)
    {
        /* if uninstall process is detected, stop ap process immediately */
        if (WF_CANNOT_RUN(pnic_info))
        {
            wf_yield();
            continue;
        }
        
        if(get_sys_work_mode(pnic_info) == WF_MASTER_MODE)
        {
            /* process ap threads in each wdn class */
            wf_list_for_each_safe(pos, pos_next, &pwdn->head)
            {
                pwdn_node = wf_list_entry(pos, wdn_node_st, list);
                pwdn_info = &pwdn_node->info;
                ap_thrd(pnic_info, pwdn_info);
            }
        }
        else if (get_sys_work_mode(pnic_info) == WF_ADHOC_MODE)
        {
#ifdef CFG_ENABLE_ADHOC_MODE
            wf_list_for_each_safe(pos, pos_next, &pwdn->head)
            {
                pwdn_node = wf_list_entry(pos, wdn_node_st, list);
                pwdn_info = &pwdn_node->info;
                adhoc_thrd(pnic_info, pwdn_info);
            }
#endif
        }
        else
        {
            wf_yield();
            continue;
        }

        wf_yield();
    }

	wf_os_api_thread_exit(pcur_network->ap_tid);
}

int wf_ap_init (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_u8 i;
    sec_info_st *psec_info = pnic_info->sec_info;

    for (i = 0; i < WF_AP_MSG_TAG_MAX; i++)
    {
        wf_que_init(&pcur_network->ap_msg_free[i], WF_LOCK_TYPE_SPIN);
    }
    pcur_network->ap_tid = NULL;
    psec_info->busetkipkey = wf_false;
    wf_ap_status_set(pnic_info, WF_AP_STATUS_UNINITILIZED);

    return 0;
}

int wf_ap_probe (nic_info_st *pnic_info,
                 wf_80211_mgmt_t *pframe, wf_u16 frame_len)
{
    struct xmit_buf *pxmit_buf;
    tx_info_st *ptx_info;
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_80211_mgmt_t *pmgmt;

    if (!pnic_info->is_up)
    {
        return -1;
    }
    if (wf_ap_status_get(pnic_info) == WF_AP_STATUS_UNINITILIZED)
    {
        return -2;
    }

    if (wf_80211_get_frame_type(pframe->frame_control) != WF_80211_FRM_PROBE_REQ)
    {
        return -3;
    }

    if (!(mac_addr_equal(pframe->da, pwlan_info->cur_network.mac_addr) ||
          is_bcast_addr(pframe->da)))
    {
        AP_WARN("probe request target address invalid");
        return -4;
    }

    AP_DBG("receive probe request");

    /* alloc xmit_buf */
    ptx_info = (tx_info_st *)pnic_info->tx_info;
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if(pxmit_buf == NULL)
    {
        AP_WARN("pxmit_buf is NULL");
        return -1;
    }

    /* clear frame head(txd + 80211head) */
    wf_memset(pxmit_buf->pbuf, 0,
              TXDESC_OFFSET + WF_OFFSETOF(wf_80211_mgmt_t, body));

    /* set frame type */
    pmgmt = (void *)&pxmit_buf->pbuf[TXDESC_OFFSET];
    wf_80211_set_frame_type(&pmgmt->frame_control, WF_80211_FRM_PROBE_RESP);
    pmgmt->frame_control = wf_cpu_to_le16(pmgmt->frame_control);

    /* set address */
    wf_memcpy(pmgmt->da, pframe->sa, WF_ARRAY_SIZE(pmgmt->da));
    wf_memcpy(pmgmt->sa, pcur_network->mac_addr, WF_ARRAY_SIZE(pmgmt->sa));
    wf_memcpy(pmgmt->bssid, pcur_network->bssid, WF_ARRAY_SIZE(pmgmt->bssid));

    /* set ie fiexd field */
    pmgmt->beacon.intv = wf_cpu_to_le16(pcur_network->bcn_interval);
    pmgmt->beacon.capab = wf_cpu_to_le16(pcur_network->cap_info);

    switch(pcur_network->hidden_ssid_mode)
    {
        case 0 :
            /* set ie variable fields */
            wf_memcpy(pmgmt->beacon.variable,
                      pcur_network->ies, pcur_network->ies_length);
            /* send packet */
            pxmit_buf->pkt_len = WF_OFFSETOF(wf_80211_mgmt_t, probe_resp.variable) +
                                 pcur_network->ies_length;
            break;

        case 1 :
            if(!wf_memcmp(pframe->probe_req.variable + 2, 
                          pcur_network->hidden_ssid.data, 
                          pcur_network->hidden_ssid.length))
            {
                wf_wlan_set_cur_ssid(pnic_info, &pcur_network->hidden_ssid);
                pmgmt->beacon.variable[1] = pcur_network->hidden_ssid.length;
                wf_memcpy(pmgmt->beacon.variable + 2, pcur_network->hidden_ssid.data, 
                          pcur_network->hidden_ssid.length);

                wf_memcpy(pmgmt->beacon.variable + 2 + pcur_network->hidden_ssid.length, 
                          pcur_network->ies + 2 + pcur_network->ies[1], 
                          pcur_network->ies_length -  2 - pcur_network->ies[1]);
                /* send packet */
                pxmit_buf->pkt_len = WF_OFFSETOF(wf_80211_mgmt_t, probe_resp.variable) +
                                     pcur_network->ies_length + pcur_network->hidden_ssid.length;
            }
            else 
            {
                wf_xmit_extbuf_delete(ptx_info, pxmit_buf);
                return 0;
            }
            break;
        case 2 :
            if(!wf_memcmp(pframe->probe_req.variable + 2, 
                          pcur_network->hidden_ssid.data, 
                          pcur_network->hidden_ssid.length))
            {
                wf_wlan_set_cur_ssid(pnic_info, &pcur_network->hidden_ssid);
                wf_memcpy(pmgmt->beacon.variable,
                      pcur_network->ies, pcur_network->ies_length);
                wf_memcpy(pmgmt->beacon.variable + 2, pcur_network->hidden_ssid.data, 
                          pcur_network->hidden_ssid.length);
                /* send packet */
                pxmit_buf->pkt_len = WF_OFFSETOF(wf_80211_mgmt_t, probe_resp.variable) +
                                     pcur_network->ies_length;
            }
            else 
            {
                wf_xmit_extbuf_delete(ptx_info, pxmit_buf);
                return 0;
            }
            break;
        default :
            return -2;
    }

    /* send packet */
    pxmit_buf->pkt_len = WF_OFFSETOF(wf_80211_mgmt_t, probe_resp.variable) +
                         pcur_network->ies_length + pcur_network->hidden_ssid.length;
    wf_nic_mgmt_frame_xmit(pnic_info, NULL, pxmit_buf, pxmit_buf->pkt_len);

    return 0;
}

static int ap_launch_beacon (nic_info_st *pnic_info)
{
    struct xmit_buf *pxmit_buf;
    tx_info_st *ptx_info;
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_80211_mgmt_t *pmgmt;

    ptx_info = (tx_info_st *)pnic_info->tx_info;

    /* alloc xmit_buf */
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if(pxmit_buf == NULL)
    {
        AP_WARN("pxmit_buf is NULL");
        return -1;
    }

    /* clear frame head(txd + 80211head) */
    wf_memset(pxmit_buf->pbuf, 0,
              TXDESC_OFFSET + WF_OFFSETOF(wf_80211_mgmt_t, body));

    /* set frame type */
    pmgmt = (void *)&pxmit_buf->pbuf[TXDESC_OFFSET];
    wf_80211_set_frame_type(&pmgmt->frame_control, WF_80211_FRM_BEACON);
    pmgmt->frame_control = wf_cpu_to_le16(pmgmt->frame_control);

    /* set address */
    wf_memset(pmgmt->da, 0xff, ETH_ALEN);
    wf_memcpy(pmgmt->sa, pcur_network->mac_addr, ETH_ALEN);
    wf_memcpy(pmgmt->bssid, pcur_network->bssid, ETH_ALEN);

    /* set ie fiexd field */
    pmgmt->beacon.intv = wf_cpu_to_le16(pcur_network->bcn_interval);
    pmgmt->beacon.capab = wf_cpu_to_le16(pcur_network->cap_info);
    
    /* set ie variable fields */
    wf_memcpy(pmgmt->beacon.variable,
              pcur_network->ies, pcur_network->ies_length);

    /* send packet */
    pxmit_buf->pkt_len =
        WF_OFFSETOF(wf_80211_mgmt_t, beacon.variable) + pcur_network->ies_length;
    wf_nic_beacon_xmit(pnic_info, pxmit_buf, pxmit_buf->pkt_len);

    return 0;
}

static void ap_update_reg (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u32 tmp32u;
    wf_u16 br_cfg;

    AP_DBG();

    /* set ht paramete */
    if (pcur_network->ht_enable == wf_true)
    {
        wf_mcu_set_max_ampdu_len(pnic_info,
                                 pcur_network->pht_cap.ampdu_params_info &
                                 WF_80211_MGMT_HT_AMPDU_PARM_FACTOR);
        wf_mcu_set_min_ampdu_space(pnic_info,
                                   (pcur_network->pht_cap.ampdu_params_info &
                                    WF_80211_MGMT_HT_AMPDU_PARM_DENSITY) >>
                                   WF_80211_MGMT_HT_AMPDU_PARM_DENSITY_SHIFT);
    }

    /* clear CAM */
    wf_mcu_set_dk_cfg(pnic_info, psec_info->dot11AuthAlgrthm, wf_false);
    wf_mcu_set_on_rcr_am(pnic_info, wf_false);
    wf_mcu_set_hw_invalid_all(pnic_info);

    /* set AP mode */
    wf_mcu_set_ap_mode(pnic_info);

    /* set bssid */
    wf_mcu_set_bssid(pnic_info, (void *)pcur_network->bssid);

    /* set AC_PARAM */
    wf_mcu_set_ac_vo(pnic_info);
    wf_mcu_set_ac_vi(pnic_info);
    wf_mcu_set_ac_be(pnic_info);
    wf_mcu_set_ac_bk(pnic_info);

    /* Set Security */
#define TX_USE_DEF_KEY              BIT(0)
#define RX_USE_DEF_KEY              BIT(1)
#define TX_ENC_ENABLE               BIT(2)
#define RX_DEC_ENABLE               BIT(3)
#define SEACH_KEY_BY_A2             BIT(4)
#define NO_SEACH_MULTICAST          BIT(5)
#define TX_BROADCAST_USE_DEF_KEY    BIT(6)
#define RX_BROADCAST_USE_DEF_KEY    BIT(7)
    switch (psec_info->dot11AuthAlgrthm)
    {
        case dot11AuthAlgrthm_Shared :
            tmp32u = TX_USE_DEF_KEY | RX_USE_DEF_KEY |
                     TX_ENC_ENABLE | RX_DEC_ENABLE |
                     TX_BROADCAST_USE_DEF_KEY | RX_BROADCAST_USE_DEF_KEY;
            break;
        case dot11AuthAlgrthm_8021X :
            tmp32u = TX_ENC_ENABLE | RX_DEC_ENABLE |
                     TX_BROADCAST_USE_DEF_KEY | RX_BROADCAST_USE_DEF_KEY;
            break;
        case dot11AuthAlgrthm_Open :
        default :
            tmp32u = 0x0;
            break;
    }
    wf_mcu_set_sec_cfg(pnic_info, tmp32u);

    /* set beacon interval */
    wf_mcu_set_bcn_intv(pnic_info, pcur_network->bcn_interval);

    /* set SISF */
    wf_mcu_set_sifs(pnic_info, 0x0a0a0808); /* 0x0808 -> for CCK, 0x0a0a -> for OFDM */

    /* set wireless mode */

    /* set basic rate */
    get_bratecfg_by_support_dates(pcur_network->rate, pcur_network->rate_len,
                                  &br_cfg);
    wf_mcu_set_basic_rate(pnic_info, br_cfg);

    /* set preamble */
    wf_mcu_set_preamble(pnic_info,
                        (pcur_network->cap_info & WF_80211_MGMT_CAPAB_SHORT_PREAMBLE) ?
                        wf_true : wf_false);

    /* set slot time */
    wf_mcu_set_slot_time(pnic_info, pcur_network->short_slot);

    /* set channel basebond */
    wf_hw_info_set_channnel_bw(pnic_info,
                               pcur_network->channel, pcur_network->bw, pcur_network->channle_offset);
}

int wf_ap_set_beacon (nic_info_st *pnic_info, wf_u8 *pbuf, wf_u32 len)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    hw_info_st *phw_info = (hw_info_st *)pnic_info->hw_info;
    auth_info_t *pauth_info = pnic_info->auth_info;
    wf_u32 ies_len;
    struct beacon_ie *pies;
    wf_u16 var_len, ofs;
    wf_u8 *pvar;
    wf_80211_mgmt_ie_t *pie;
    wf_80211_wmm_param_ie_t *pwmm_ie;
    wf_80211_mgmt_ht_cap_t *pht_cap = NULL;
    wf_80211_mgmt_ht_operation_t *pht_oper;
    wf_u16 i, j;
#ifdef CONFIG_CONCURRENT_MODE
	wf_bool bConnect;
	wdn_net_info_st *pwdn = NULL;
#endif
    AP_DBG();

    ies_len = len;
    if (ies_len <= WF_OFFSETOF(struct beacon_ie, variable))
    {
        AP_WARN("ie data corrupt");
        return -1;
    }

    /* initilize value */
    pcur_network->ssid.length = 0;
    pcur_network->channel = 0;
    pcur_network->rate_len = 0;
	pcur_network->ht_enable = wf_false;
	wf_memset(&pcur_network->pwmm, 0, sizeof(wf_80211_wmm_param_ie_t));
	wf_memset(&pcur_network->pht_cap, 0, sizeof(wf_80211_mgmt_ht_cap_t));
	wf_memset(&pcur_network->pht_oper, 0, sizeof(wf_80211_mgmt_ht_operation_t));
//    pcur_network->pht_cap = NULL;
//    pcur_network->pht_oper = NULL;

    /* update mac/bssid */
    wf_memcpy(pcur_network->mac_addr, nic_to_local_addr(pnic_info), MAC_ADDR_LEN);
    wf_memcpy(pcur_network->bssid, nic_to_local_addr(pnic_info), MAC_ADDR_LEN);
    AP_DBG("bssid: "WF_MAC_FMT, WF_MAC_ARG(pcur_network->bssid));

    /* get ie fixed field */
    pies = (void *)pbuf;
    pcur_network->bcn_interval   = wf_le16_to_cpu(pies->intv);
    pcur_network->cap_info       = wf_le16_to_cpu(pies->capab);
    AP_DBG("beacon interval: (%d), capability information: (0x%04X)",
           pcur_network->bcn_interval, pcur_network->cap_info);

    /* save ies variable field */
    pvar = pies->variable;
    var_len = ies_len - WF_OFFSETOF(struct beacon_ie, variable);
    wf_memcpy(pcur_network->ies, pvar, var_len);
    pcur_network->ies_length = var_len;
//    AP_ARRAY(pcur_network->ies, ies_len);

    /* checkout ie variable field */
    for (ofs = 0; ofs < var_len;
         ofs += WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len)
    {
        pie = (void *)&pvar[ofs];

        switch(pie->element_id)
        {
            case WF_80211_MGMT_EID_SSID :
                if (pie->len >= sizeof(pcur_network->ssid.data))
                {
                    AP_WARN("ssid length(%d) over limite", pie->len);
                    break;
                }
                pcur_network->ssid.length = pie->len;
                wf_memcpy(pcur_network->ssid.data, pie->data,
                          pcur_network->ssid.length);
                pcur_network->ssid.data[pcur_network->ssid.length] = '\0';
                AP_DBG("ssid: %s", pcur_network->ssid.data);
                break;

            case WF_80211_MGMT_EID_DS_PARAMS :
#ifdef CONFIG_CONCURRENT_MODE
				wf_mlme_get_connect(pnic_info->vir_nic, &bConnect);
				if(bConnect == wf_true && wf_local_cfg_get_work_mode(pnic_info->vir_nic) == WF_INFRA_MODE)
				{
					pwdn = wf_wdn_find_info_by_id(pnic_info->vir_nic, 0);
					if(pwdn)
					{
						AP_DBG("sta channel is : %d", pwdn->channel);
						pcur_network->channel = pwdn->channel;
						wf_ap_reset_beacon_channel(pnic_info, pwdn->channel);
					}

				}
				else
				{
					pcur_network->channel = pie->data[0];
				}
#else
                pcur_network->channel = pie->data[0];
#endif
                AP_DBG("channel: %d", pcur_network->channel);
                break;

            case WF_80211_MGMT_EID_EXT_SUPP_RATES :
            case WF_80211_MGMT_EID_SUPP_RATES :
                if (pie->len >= sizeof(pcur_network->rate) - pcur_network->rate_len)
                {
                    AP_WARN("support rates size over limite");
                    break;
                }
                /* check and retrieve rate */
                if (pcur_network->rate_len == 0)
                    wf_memset(pcur_network->rate, 0x0,
                              WF_ARRAY_SIZE(pcur_network->rate));
                for (i = 0; i < pie->len; i++)
                {
                    for (j = 0; j < WF_ARRAY_SIZE(phw_info->datarate); j++)
                    {
                        if (phw_info->datarate[j] == 0x0)
                            break;
                        if (pie->data[i] == phw_info->datarate[j])
                        {
                            pcur_network->rate[pcur_network->rate_len++] =
                                pie->data[i];
                            break;
                        }
                    }
                }
                AP_DBG("data rate(Mbps): %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
                       (pcur_network->rate[0] & 0x7F) / 2,
                       (pcur_network->rate[1] & 0x7F) / 2,
                       (pcur_network->rate[2] & 0x7F) / 2,
                       (pcur_network->rate[3] & 0x7F) / 2,
                       (pcur_network->rate[4] & 0x7F) / 2,
                       (pcur_network->rate[5] & 0x7F) / 2,
                       (pcur_network->rate[6] & 0x7F) / 2,
                       (pcur_network->rate[7] & 0x7F) / 2,
                       (pcur_network->rate[8] & 0x7F) / 2,
                       (pcur_network->rate[9] & 0x7F) / 2,
                       (pcur_network->rate[10] & 0x7F) / 2,
                       (pcur_network->rate[11] & 0x7F) / 2,
                       (pcur_network->rate[12] & 0x7F) / 2,
                       (pcur_network->rate[13] & 0x7F) / 2,
                       (pcur_network->rate[14] & 0x7F) / 2,
                       (pcur_network->rate[15] & 0x7F) / 2);
                break;

            case WF_80211_MGMT_EID_ERP_INFO :
                break;

            case WF_80211_MGMT_EID_RSN :
                if (!wf_80211_mgmt_rsn_parse(pie,
                                             WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len,
                                             &psec_info->rsn_group_cipher,
                                             &psec_info->rsn_pairwise_cipher))
                {
                    /* PSK */
                    psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
                    psec_info->wpa_psk |= BIT(1);
                    AP_DBG("RSN element");
                    break;
                }

            case WF_80211_MGMT_EID_VENDOR_SPECIFIC :
                if (!wf_80211_mgmt_wpa_parse(pie,
                                             WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len,
                                             &psec_info->wpa_multicast_cipher,
                                             &psec_info->wpa_unicast_cipher))
                {
                    psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
                    psec_info->wpa_psk |= BIT(0);
                    AP_DBG("WPA element");
                }
                else if (!wf_80211_mgmt_wmm_parse(pie,
                                                  WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len))
                {
                	wf_memcpy(&pcur_network->pwmm, pie, sizeof(pcur_network->pwmm));
//                    pcur_network->pwmm = (void *)pie;
                    /* adjust WMM information */
                    pwmm_ie = (void *)pie;
                    pwmm_ie->qos_info |= WF_80211_MGMT_WMM_IE_AP_QOSINFO_UAPSD;
                    /* disable ACM for BE BK VI VO */
                    for (i = 0; i < WF_ARRAY_SIZE(pwmm_ie->ac); i++)
                    {
                        pwmm_ie->ac[i].aci_aifsn &= ~ BIT(4);
                    }
                    AP_DBG("WMM element");
                }
                break;

            case WF_80211_MGMT_EID_HT_CAPABILITY :
//                pcur_network->pht_cap = (void *)pie->data;
				wf_memcpy(&pcur_network->pht_cap, pie->data, pie->len);
                pht_cap = &pcur_network->pht_cap;
				pcur_network->ht_enable = wf_true;
                AP_DBG("ht capatility");
                break;

            case WF_80211_MGMT_EID_HT_OPERATION :
//                pcur_network->pht_oper = (void *)pie->data;
				wf_memcpy(&pcur_network->pht_oper, pie->data, pie->len);
                pht_oper = &pcur_network->pht_oper;
                AP_DBG("ht operation");
                break;
        }
    }

    /* checkout and adjust ht capability field */
    if (pht_cap)
    {
        /* cap_info */

        if ((pht_cap->cap_info & WF_80211_MGMT_HT_CAP_SUP_WIDTH_20_40) &&
            phw_info->cbw40_support)
        {
            pcur_network->bw = CHANNEL_WIDTH_40;
        }
		else
        {
            pcur_network->bw = CHANNEL_WIDTH_20;
            pht_cap->cap_info &= ~WF_80211_MGMT_HT_CAP_SGI_40;
        }
#ifdef CONFIG_CONCURRENT_MODE
		if(bConnect == wf_true && wf_local_cfg_get_work_mode(pnic_info->vir_nic) == WF_INFRA_MODE)
		{
			pwdn = wf_wdn_find_info_by_id(pnic_info->vir_nic, 0);
			if(pwdn)
			{
				AP_DBG("sta bw is : %d", pwdn->bw_mode);
				pcur_network->bw = pwdn->bw_mode;
			}
		}
#endif

        if (!phw_info->ldpc_support)
            pht_cap->cap_info &= ~WF_80211_MGMT_HT_CAP_LDPC_CODING;

        if (!phw_info->tx_stbc_support)
            pht_cap->cap_info &= ~WF_80211_MGMT_HT_CAP_TX_STBC;

        if (!phw_info->rx_stbc_support)
            pht_cap->cap_info &= ~WF_80211_MGMT_HT_CAP_RX_STBC;
        else if ((phw_info->rx_stbc_num & WF_80211_MGMT_HT_CAP_RX_STBC) <
                 (pht_cap->cap_info & WF_80211_MGMT_HT_CAP_RX_STBC))
        {
            pht_cap->cap_info &= ~WF_80211_MGMT_HT_CAP_RX_STBC;
            pht_cap->cap_info |= (phw_info->rx_stbc_num <<
                                  WF_80211_MGMT_HT_CAP_RX_STBC_SHIFT) &
                                 WF_80211_MGMT_HT_CAP_RX_STBC;
        }

        /* ampdu_params_info */
        if ((phw_info->max_rx_ampdu_factor & WF_80211_MGMT_HT_AMPDU_PARM_FACTOR) <
            (pht_cap->ampdu_params_info & WF_80211_MGMT_HT_AMPDU_PARM_FACTOR))
        {
            pht_cap->ampdu_params_info &= ~WF_80211_MGMT_HT_AMPDU_PARM_FACTOR;
            pht_cap->ampdu_params_info |=
                (phw_info->max_rx_ampdu_factor & WF_80211_MGMT_HT_AMPDU_PARM_FACTOR);
        }

        if ((psec_info->wpa_unicast_cipher & CIPHER_SUITE_CCMP) ||
            (psec_info->rsn_pairwise_cipher & CIPHER_SUITE_CCMP))
        {
            if ((phw_info->best_ampdu_density & WF_80211_MGMT_HT_AMPDU_PARM_DENSITY) >
                (pht_cap->ampdu_params_info & WF_80211_MGMT_HT_AMPDU_PARM_DENSITY))
            {
                pht_cap->ampdu_params_info &= ~WF_80211_MGMT_HT_AMPDU_PARM_DENSITY;
                pht_cap->ampdu_params_info |=
                    (phw_info->best_ampdu_density << WF_80211_MGMT_HT_AMPDU_PARM_DENSITY_SHIFT) &
                    WF_80211_MGMT_HT_AMPDU_PARM_DENSITY; /* 16usec */
            }
        }
        else
            pht_cap->ampdu_params_info &= ~WF_80211_MGMT_HT_AMPDU_PARM_DENSITY;

        /* support mcs set */
        for (i = 0; i < WF_ARRAY_SIZE(pht_cap->mcs_info.rx_mask); i++)
        {
            pht_cap->mcs_info.rx_mask[i] &=
                phw_info->default_supported_mcs_set[i];
        }
    }

    /* checkout and adjust ht option field */
    if (pcur_network->ht_enable == wf_true)
    {
        if (pcur_network->channel == 0x0)
            pcur_network->channel = pcur_network->pht_oper.primary_chan;
        else if (pcur_network->channel != pcur_network->pht_oper.primary_chan)
            AP_WARN("primary channel(%d) inconsistent with DSSS(%d)",
                    pcur_network->pht_oper.primary_chan, pcur_network->channel);

        if (!(pcur_network->pht_oper.ht_param &
              WF_80211_MGMT_HT_OP_PARAM_CHAN_WIDTH_ANY))
            pcur_network->bw = CHANNEL_WIDTH_20;

        if (pcur_network->bw == CHANNEL_WIDTH_40)
            pcur_network->channle_offset =
                pcur_network->pht_oper.ht_param &
                WF_80211_MGMT_HT_OP_PARAM_CHA_SEC_OFFSET;
    }
#ifdef CONFIG_CONCURRENT_MODE
	if(bConnect == wf_true && wf_local_cfg_get_work_mode(pnic_info->vir_nic) == WF_INFRA_MODE)
	{
		pwdn = wf_wdn_find_info_by_id(pnic_info->vir_nic, 0);
		if(pwdn)
		{
			AP_DBG("sta channel_offset is : %d", pwdn->channle_offset);
			pcur_network->channle_offset = pwdn->channle_offset;
		}
	}
#endif
    /* get network type */
    if (pcur_network->channel > 14)
        pcur_network->cur_wireless_mode = WIRELESS_INVALID;
    else
    {
        if (only_cckrates(pcur_network->rate, pcur_network->rate_len))
            pcur_network->cur_wireless_mode = WIRELESS_11B;
        else if (have_cckrates(pcur_network->rate, pcur_network->rate_len))
            pcur_network->cur_wireless_mode = WIRELESS_11BG;
        else
            pcur_network->cur_wireless_mode = WIRELESS_11G;

        if (pcur_network->ht_enable == wf_true)
            pcur_network->cur_wireless_mode |= WIRELESS_11_24N;
    }

    /* get short slot time */
    if (pcur_network->cap_info & WF_80211_MGMT_CAPAB_IBSS)
    {
        pcur_network->short_slot = NON_SHORT_SLOT_TIME;
    }
	else if (pcur_network->cur_wireless_mode & WIRELESS_11_24N)
	{
		pcur_network->short_slot = SHORT_SLOT_TIME;
	}
	else if (pcur_network->cur_wireless_mode & WIRELESS_11G)
    {
        if (pcur_network->cap_info & WF_80211_MGMT_CAPAB_SHORT_SLOT_TIME)
            pcur_network->short_slot = SHORT_SLOT_TIME;
        else
            pcur_network->short_slot = NON_SHORT_SLOT_TIME;
    }
    else
    {
        pcur_network->short_slot = NON_SHORT_SLOT_TIME;
    }

    /* update beacon related regiest */
    ap_update_reg(pnic_info);

    /* send beacon */
    ap_launch_beacon(pnic_info);

    /* enable data queue */
    wf_os_api_enable_all_data_queue(pnic_info->ndev);

    return 0;
}

int wf_ap_pre_set_beacon (nic_info_st *pnic_info, ieee_param *param, wf_u32 len)
{
	return wf_ap_set_beacon(pnic_info, param->u.bcn_ie.buf, (len - WF_OFFSETOF(ieee_param, u.bcn_ie.buf)));
}

int wf_ap_work_start (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    AP_DBG();

    /* initilize ap message queue */
    if (ap_msg_que_init(pnic_info))
    {
        AP_WARN("AP message initilize failed");
        return -2;
    }

    /* create thread for AP process */
    pcur_network = &pwlan_info->cur_network;
    sprintf(pcur_network->ap_name,
            pnic_info->virNic ? "wlan_info:vir%d_s%d" : "wlan_info:wlan%d_s%d",
            pnic_info->hif_node_id, pnic_info->ndev_id);
    if (NULL == (pcur_network->ap_tid = wf_os_api_thread_create(pcur_network->ap_tid, pcur_network->ap_name,
                                ap_poll, pnic_info)))
    {
        AP_WARN("create wlan info parse thread failed");
        return -3;
    }

    if(!pcur_network->ap_tid)
    {
        AP_WARN("tid error");
    }

    /* update AP status */
    wf_ap_status_set(pnic_info, WF_AP_STATUS_STANBY);

    wf_os_api_thread_wakeup(pcur_network->ap_tid);

    return 0;
}

int wf_ap_work_stop (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    sec_info_st *psec_info = pnic_info->sec_info;

    AP_DBG();

    /* destory thread */
    if (pcur_network->ap_tid)
    {
        wf_os_api_thread_destory(pcur_network->ap_tid);
        pcur_network->ap_tid = NULL;
    }

    ap_msg_deinit(pnic_info);
    memset(psec_info, 0, sizeof(sec_info_st));
    /* update ap status */
    wf_ap_status_set(pnic_info, WF_AP_STATUS_UNINITILIZED);

    return 0;
}

void ap_deauth_frame_wlan_hdr (nic_info_st *pnic_info, struct xmit_buf *pxmit_buf)
{
    wf_u8 *pframe;
    struct wl_ieee80211_hdr *pwlanhdr;
    mlme_info_t *pmlme_info = (mlme_info_t *)pnic_info->mlme_info;;
    pframe = pxmit_buf->pbuf + TXDESC_OFFSET;
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

    pwlanhdr->frame_ctl = 0;
    SetFrameType(pframe, WIFI_MGT_TYPE);
    SetFrameSubType(pframe, WIFI_DEAUTH);  /* set subtype */
}

int wf_ap_deauth_all_sta (nic_info_st *pnic_info, wf_u16 reason)
{
    wf_u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    wf_u8 *pframe;
    wf_u16 start_seq;
    struct wl_ieee80211_hdr *pwlanhdr;
    struct xmit_buf *pxmit_buf;
    wf_u32 pkt_len, i;
    tx_info_st *ptx_info;
    wdn_net_info_st *wdn_info;
    mlme_info_t *mlme_info;
    hw_info_st *phw_info = (hw_info_st *)pnic_info->hw_info;
    wf_u8 category = WF_WLAN_CATEGORY_BACK;

    AP_DBG();

    wdn_info = wf_wdn_find_info(pnic_info, bc_addr);
    if (wdn_info == NULL)
    {
        return -1;
    }

    ptx_info    = (tx_info_st *)pnic_info->tx_info;
    mlme_info   = (mlme_info_t *)pnic_info->mlme_info;

    /* alloc xmit_buf */
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if(pxmit_buf == NULL)
    {
        AP_WARN("pxmit_buf is NULL");
        return -1;
    }
    wf_memset(pxmit_buf->pbuf, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

    /* type of management is 0100 */
    ap_deauth_frame_wlan_hdr(pnic_info, pxmit_buf);

    /* set txd at tx module */
    pframe = pxmit_buf->pbuf + TXDESC_OFFSET; /* pframe point to wlan_hdr */
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

    /* copy addr1/2/3 */
    wf_memcpy(pwlanhdr->addr1, bc_addr, MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr2, nic_to_local_addr(pnic_info), MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr3, wdn_info->bssid, MAC_ADDR_LEN);

    pkt_len = sizeof(struct wl_ieee80211_hdr_3addr);
    pframe += pkt_len; /* point to iv or frame body */

    reason = wf_cpu_to_le16(reason);
    pframe = set_fixed_ie(pframe, 2, (wf_u8 *)&reason, &pkt_len);
    wf_nic_mgmt_frame_xmit(pnic_info, wdn_info, pxmit_buf, pxmit_buf->pkt_len);

    return 0;
}

int wf_ap_set_encryption (nic_info_st *pnic_info,
                          ieee_param *param, wf_u32 param_len)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    int res = 0;
    wf_u32 wep_key_idx, wep_key_len;
    wdn_net_info_st *pwdn_info = NULL;
    wf_u8 bc_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    AP_DBG();

    param->u.crypt.err = 0;
    param->u.crypt.alg[IEEE_CRYPT_ALG_NAME_LEN - 1] = '\0';

    if (param_len <
        WF_FIELD_SIZEOF(ieee_param, u.crypt.key) + param->u.crypt.key_len)
    {
        AP_WARN("param_len invalid !!!!!!!");
        res = -EINVAL;
        goto exit;
    }

    if (is_bcast_addr(param->sta_addr))
    {
        AP_DBG("set with boardcast address");
        if (param->u.crypt.idx >= WEP_KEYS)
        {
            res = -EINVAL;
            goto exit;
        }
    }
    else
    {
        pwdn_info = wf_wdn_find_info(pnic_info, param->sta_addr);
        if(pwdn_info == NULL)
        {
            AP_DBG("sta has already been removed or never been added");
            goto exit;
        }
    }

    /* clear default key before ap start work */
    if (!strcmp(param->u.crypt.alg, "none") && !pwdn_info)
    {
        psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
        psec_info->ndisencryptstatus = wf_ndis802_11EncryptionDisabled;
        psec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
        psec_info->dot118021XGrpPrivacy = _NO_PRIVACY_;
        AP_DBG("clear default encryption keys, keyid=%d", param->u.crypt.idx);
        goto exit;
    }

    /* set group key(for wpa/wpa2) or default key(for wep) before establish */
    if (pwdn_info == NULL)
    {
        /* for wep key */
        if (!strcmp(param->u.crypt.alg, "WEP"))
        {
            AP_DBG("crypt.alg = WEP");

            if (psec_info->wpa_unicast_cipher || psec_info->wpa_multicast_cipher ||
                psec_info->rsn_group_cipher || psec_info->rsn_pairwise_cipher)
            {
                AP_WARN("wep no support 8021x !!!");
                res = -EINVAL;
            }

#ifdef WF_WEP_AUTH_ENABLE_WEXT
            /* set auth algorithm
               todo :if WF_WEP_AUTH_ENABLE_WEXT = 1, param->u.wpa_param.value must
               be assigned in hostapd. The assignment position is in
               (hostapd---> driver_wf_scics.c---> set_key_ops)*/
            AP_DBG("param->u.wpa_param.value == %d\n", param->u.wpa_param.value);
            switch (param->u.wpa_param.value)
            {
                case 0x1 :
                    psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
                    break;

                case 0x2 :
                    psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;
                    break;

                case 0x3 :
                default :
                    psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
                    break;
            }
#else
            psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
#endif

            wep_key_idx = param->u.crypt.idx;
            wep_key_len = param->u.crypt.key_len;
            AP_DBG("wep_key_idx=%d, len=%d\n", wep_key_idx,
                   wep_key_len);

            /* check key idex and key len */
            if (wep_key_idx >= WEP_KEYS || wep_key_len == 0)
            {
                res = -EINVAL;
                goto exit;
            }
            wep_key_len = wep_key_len <= 5 ? 5 : 13; /* 5B for wep40 and 13B for wep104 */

            /* TODO: tx=1, the key only used to encrypt data in data send process,
            that is to say no used for boradcast */
            if (param->u.crypt.set_tx)
            {
                AP_DBG("wep, set_tx=1");
                /* update encrypt algorithm */
                psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;
                if (wep_key_len == 13)
                {
                    psec_info->dot11PrivacyAlgrthm = _WEP104_;
                    psec_info->dot118021XGrpPrivacy = _WEP104_;
                }
                else
                {
                    psec_info->dot11PrivacyAlgrthm = _WEP40_;
                    psec_info->dot118021XGrpPrivacy = _WEP40_;
                }

                /* todo: force wep key id set to 0, other id value no used by STA */
                if (wep_key_idx == 0)
                {
                    /* update default key(for wep) */
                    psec_info->dot11PrivacyKeyIndex = wep_key_idx;
                    wf_memcpy(&psec_info->dot11DefKey[wep_key_idx].skey[0],
                              param->u.crypt.key, wep_key_len);
                    psec_info->dot11DefKeylen[wep_key_idx] = wep_key_len;
                }
            }

            res = -EINVAL;
            goto exit;
        }

        /* for group key */
        if (!strcmp(param->u.crypt.alg, "TKIP"))
        {
            AP_DBG("set group_key, TKIP");

            psec_info->dot118021XGrpPrivacy = _TKIP_;
            AP_DBG("key_idx=%d", param->u.crypt.idx);
            AP_ARRAY(param->u.crypt.key, param->u.crypt.key_len);

            /* KCK PTK0~127 */
            psec_info->dot118021XGrpKeyid = param->u.crypt.idx;
            wf_memcpy(psec_info->dot118021XGrpKey[param->u.crypt.idx].skey,
                      param->u.crypt.key, WF_MIN(param->u.crypt.key_len, 16));
            /* set mic key */
            /* KEK PTK128~255 */
            wf_memcpy(psec_info->dot118021XGrptxmickey[param->u.crypt.idx].skey,
                      &param->u.crypt.key[16], 8); /* PTK128~191 */
            wf_memcpy(psec_info->dot118021XGrprxmickey[param->u.crypt.idx].skey,
                      &param->u.crypt.key[24], 8); /* PTK192~255 */

            psec_info->busetkipkey = wf_true;
        }
        else if(!strcmp(param->u.crypt.alg, "CCMP"))
        {
            AP_DBG("set group_key, CCMP");
            psec_info->dot118021XGrpPrivacy = _AES_;
            /* KCK PTK0~127 */
            psec_info->dot118021XGrpKeyid = param->u.crypt.idx;
            AP_DBG("set group_key id(%d), CCMP", psec_info->dot118021XGrpKeyid);
            wf_memcpy(psec_info->dot118021XGrpKey[param->u.crypt.idx].skey,
                      param->u.crypt.key, WF_MIN(param->u.crypt.key_len, 16));
            ap_hw_set_group_key(pnic_info, param->sta_addr);
        }
        else
        {
            AP_DBG("set group_key, none");
            goto exit;
        }

        psec_info->dot11PrivacyAlgrthm = psec_info->dot118021XGrpPrivacy;
        psec_info->binstallGrpkey = wf_true;

        /* set boardcast wdn */
        pwdn_info = wf_wdn_find_info(pnic_info, param->sta_addr);
        if(pwdn_info)
        {
            pwdn_info->dot118021XPrivacy = psec_info->dot118021XGrpPrivacy;
            pwdn_info->ieee8021x_blocked = wf_false;
        }

        goto exit;
    }

    /* set key(for wpa/wpa2) after establish */
    else if (psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X)
    {
        if (param->u.crypt.set_tx == 1)
        {
            AP_DBG("set unicastkey");

            wf_memcpy(pwdn_info->dot118021x_UncstKey.skey, param->u.crypt.key,
                      WF_MIN(param->u.crypt.key_len, 16));

            if (!strcmp(param->u.crypt.alg, "TKIP"))
            {
                AP_DBG("set pairwise key, TKIP");
                pwdn_info->dot118021XPrivacy = _TKIP_;
                /* set mic key */
                wf_memcpy(pwdn_info->dot11tkiptxmickey.skey,
                          &param->u.crypt.key[16], 8);
                wf_memcpy(pwdn_info->dot11tkiprxmickey.skey,
                          &param->u.crypt.key[24], 8);
                psec_info->busetkipkey = wf_true;
            }
            else if (!strcmp(param->u.crypt.alg, "CCMP"))
            {
                AP_DBG("set pairwise key, CCMP");
                pwdn_info->dot118021XPrivacy = _AES_;
                /* enable hardware encrypt */
                ap_hw_set_unicast_key(pnic_info, pwdn_info);
                wf_mcu_set_dk_cfg(pnic_info, psec_info->dot11AuthAlgrthm, wf_true);
                wf_mcu_set_on_rcr_am(pnic_info, wf_true);
            }
            else if (!strcmp(param->u.crypt.alg, "none"))
            {
                AP_DBG("crypt.alo: none");
                pwdn_info->dot118021XPrivacy = _NO_PRIVACY_;
            }
        }

        pwdn_info->ieee8021x_blocked = wf_false;
    }

exit:
    return res;
}

int wf_ap_get_sta_wpaie (nic_info_st *pnic_info, ieee_param *param, wf_u32 len)
{
    wdn_net_info_st *pwdn_info = NULL;
    wf_80211_mgmt_ie_t *pie;

    AP_DBG();

    if(is_bcast_addr(param->sta_addr))
    {
        return -EINVAL;
    }

    pwdn_info = wf_wdn_find_info(pnic_info, param->sta_addr);
    if(!pwdn_info)
    {
        AP_DBG("sta has already been removed or never been added");
        return -EPERM;
    }

    if (pwdn_info->state <= E_WDN_AP_STATE_ASSOC)
    {
        return -EINVAL;
    }

    pie = (void *)pwdn_info->wpa_ie;
    if (pie->element_id == WF_80211_MGMT_EID_RSN ||
        pie->element_id == WF_80211_MGMT_EID_VENDOR_SPECIFIC)
    {
        param->u.wpa_ie.len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
        wf_memcpy(param->u.wpa_ie.reserved, pie, param->u.wpa_ie.len);
    }
    else
    {
        AP_DBG("STA no wpa_ie element");
    }

    return 0;
}

int wf_ap_remove_sta (nic_info_st *pnic_info, ieee_param *param, wf_u32 len)
{
    wdn_net_info_st *pwdn_info = NULL;

    if (is_bcast_addr(param->sta_addr))
    {
        return -EINTR;
    }

    pwdn_info = wf_wdn_find_info(pnic_info, param->sta_addr);
    if(!pwdn_info)
    {
        AP_DBG("sta has already been removed or never been added");
        return -EPERM;
    }

    return wf_ap_msg_load(pnic_info, &pwdn_info->ap_msg,
                          WF_AP_MSG_TAG_DEAUTH_FRAME, NULL, 0);
}


#if defined(CONFIG_CONCURRENT_MODE) && defined(CFG_ENABLE_AP_MODE)

void wf_ap_reset_beacon_channel(nic_info_st *pnic_info,wf_u8 channel)
{
	wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_80211_mgmt_ie_t *pie;
    wf_u16  ofs;
    LOG_I("%s nic_num:%d  channel:%d",__func__,pnic_info->nic_num,channel);

    if(pnic_info == NULL)
    {
        LOG_E("pnic NULL");
        return;
    }
    if(pcur_network->ies == NULL)
    {
        LOG_E("ie NULL");
        return ;
    }

    for (ofs = 0; ofs < pcur_network->ies_length;
         ofs += WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len)
    {
        pie = (void *)&pcur_network->ies[ofs];

        if(pie->element_id == WF_80211_MGMT_EID_DS_PARAMS)
        {
            pie->data[0] = channel;
            LOG_D("%s  channel:%d",__func__,channel);
            break;
        }

    }
}

void wf_resend_bcn(nic_info_st *pnic_info,wf_u8 channel)
{
	wf_ap_reset_beacon_channel(pnic_info, channel);
    /* send beacon */
    ap_launch_beacon(pnic_info);

}
#endif

#endif

