
/* 802.11 bgn managment frame compose and parse */
#include "common.h"
#include "wf_debug.h"

/* macro */
#if 0
#define WLAN_MGMT_DBG(fmt, ...)     LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define WLAN_MGMT_DBG(fmt, ...)
#endif
#define WLAN_MGMT_INFO(fmt, ...)    LOG_I("[%s:%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WLAN_MGMT_WARN(fmt, ...)    LOG_W("[%s:%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WLAN_MGMT_ERROR(fmt, ...)   LOG_E("[%s:%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define WLAN_MGMT_SCAN_QUE_DEEP     64
#define WLAN_MGMT_SCAN_NODE_TTL     5

/* type define */
typedef struct
{
    phy_status_st phy_sta;
    wf_bool is_phy_sta_valid;
    wf_80211_mgmt_t mgmt_frm;
} rx_frm_msg_t;

typedef struct
{
    wf_u8 ch_num;
    wf_u8 ch_map[MAX_CHANNEL_NUM];
} scan_que_refresh_msg_t;

/* function declaration */
int wf_wlan_mgmt_scan_que_read_try (wf_wlan_mgmt_scan_que_t *pscan_que)
{
    if (pscan_que == NULL)
    {
        return -1;
    }

    wf_lock_lock(&pscan_que->lock);
    if (pscan_que->read_cnt == 0xFF)
    {
        wf_lock_unlock(&pscan_que->lock);
        return -2;
    }
    if (!pscan_que->read_cnt)
    {
        if (wf_os_api_sema_try(&pscan_que->sema))
        {
            wf_lock_unlock(&pscan_que->lock);
            return -3;
        }
    }
    pscan_que->read_cnt++;
    wf_lock_unlock(&pscan_que->lock);

    return 0;
}

int wf_wlan_mgmt_scan_que_read_post (wf_wlan_mgmt_scan_que_t *pscan_que)
{
    if (pscan_que == NULL)
    {
        return -1;
    }

    wf_lock_lock(&pscan_que->lock);
    if (!pscan_que->read_cnt)
    {
        wf_lock_unlock(&pscan_que->lock);
        WLAN_MGMT_WARN("no read pend");
        return -2;
    }
    pscan_que->read_cnt--;
    if (!pscan_que->read_cnt)
    {
        wf_os_api_sema_post(&pscan_que->sema);
    }
    wf_lock_unlock(&pscan_que->lock);

    return 0;
}

wf_inline static
int wlan_mgmt_scan_que_write_try (wf_wlan_mgmt_scan_que_t *pscan_que)
{
    return wf_os_api_sema_try(&pscan_que->sema);
}

wf_inline static
int wlan_mgmt_scan_que_write_post (wf_wlan_mgmt_scan_que_t *pscan_que)
{
    wf_os_api_sema_post(&pscan_que->sema);

    return 0;
}

wf_inline static int is_cur_bss (nic_info_st *pnic_info, wf_80211_bssid_t bssid)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_bool bconnected;

    wf_mlme_get_connect(pnic_info, &bconnected);
    return (bconnected && mac_addr_equal(pwlan_mgmt_info->cur_network.bssid, bssid));
}

static int
wlan_mgmt_scan_que_node_new (nic_info_st *pnic_info, rx_frm_msg_t *pfrm_msg,
                             wf_wlan_mgmt_scan_que_node_t **pnew_node)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_mgmt_scan_que_t *pscan_que = &pwlan_mgmt_info->scan_que;
    wf_wlan_mgmt_scan_que_node_t *pscan_que_node = NULL;
    wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
    int rst;

    wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
    {
        if (mac_addr_equal(pfrm_msg->mgmt_frm.bssid, pscan_que_node->bssid))
        {
            break;
        }
    }
    wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);

    if (scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL)
    {
        return -1;
    }

    if (scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_BREAK)
    {
        /* match bssid in the scan ready queue */
        WLAN_MGMT_DBG("update scan node");
        rst = 1;
    }
    else
    {
        /* if scan free queue is no empty, get node from free queue
        no matter whether the freame's phy_state is valid or not */
        wf_que_list_t *pnode = wf_deque_head(&pscan_que->free);
        if (pnode)
        {
            WLAN_MGMT_DBG("new scan node");
            pscan_que_node =
                wf_list_entry(pnode, wf_wlan_mgmt_scan_que_node_t, list);
            rst = 2;
            goto end;
        }

        /* scan free queue is empty, new node get from scan ready queue */
        if (!pfrm_msg->is_phy_sta_valid)
        {
            /* ignore invalid phy_sta */
            WLAN_MGMT_DBG("signal strength invalid");
            return -2;
        }
        /* new node get from scan ready queue tail, which signed strength is
        worest in scan ready queue, but the signal strength with the frame
        must better than signal strength with in the new node. */
        pscan_que_node = wf_list_entry(wf_que_tail(&pscan_que->ready),
                                       wf_wlan_mgmt_scan_que_node_t, list);
        if (pfrm_msg->phy_sta.signal_strength < pscan_que_node->signal_strength)
        {
            /* only accept the frame which signal strength better than
            new node's signal strength from ready queue*/
            WLAN_MGMT_DBG("signal strength(%d, %d) too low",
                          pfrm_msg->phy_sta.signal_strength,
                          pscan_que_node->signal_strength);
            return -3;
        }
        /* ignore new node if it is the current associated bssid */
        if (is_cur_bss(pnic_info, pscan_que_node->bssid))
        {
            WLAN_MGMT_WARN("it's the associated bss");
            return -4;
        }
        WLAN_MGMT_INFO("replase scan node");
        rst = 3;
    }

    /* node dequeue from ready queue */
    if (wlan_mgmt_scan_que_write_try(pscan_que))
    {
        return -5;
    }
    wf_deque(&pscan_que_node->list, &pscan_que->ready);
    wlan_mgmt_scan_que_write_post(pscan_que);

    /* clearup scan infomation, except node update operate */
    if (rst != 1)
    {
        wf_memset(pscan_que_node, 0x0, sizeof(wf_wlan_mgmt_scan_que_node_t));
    }

end:
    /* initialize node information */
    pscan_que_node->parent = NULL;
    pscan_que_node->ttl = WLAN_MGMT_SCAN_NODE_TTL;
    pscan_que_node->updated = wf_true;
    *pnew_node = pscan_que_node;
    return rst;
}

#define NODE_INFO_DBG(...)  //WLAN_MGMT_DBG(__VA_ARGS__)
wf_inline static
int wlan_mgmt_scan_node_info (nic_info_st *pnic_info,
                              rx_frm_msg_t *pfrm_msg, wf_u16 frm_msg_len,
                              wf_wlan_mgmt_scan_que_node_t *pscan_que_node,
                              wf_bool bupdate)
{
    wf_80211_mgmt_t *pmgmt = &pfrm_msg->mgmt_frm;
    wf_u16 mgmt_len = frm_msg_len - WF_OFFSETOF(rx_frm_msg_t, mgmt_frm);
    wf_u8 *pele_start, *pele_end;
    wf_80211_mgmt_ie_t *pie;
    wf_80211_mgmt_dsss_parameter_t *pdsss_para;
    wf_80211_mgmt_ht_operation_t *pht_opt;
    wf_u8 support_rate_cnt = 0;
    wf_80211_mgmt_ht_cap_t *pht_cap;
    wf_u8 i;
    wf_bool cck_spot = wf_false, ofdm_spot = wf_false;
    wf_u8 wpa_oui[4] = {0x0, 0x50, 0xf2, 0x01};
    wf_u8 wps_oui[4] = {0x0, 0x50, 0xf2, 0x04};
    wf_80211_frame_e frame_type;

    /* if a probe respone record has exist, and the income beacon frame
    is hidden, ignore this update */
    frame_type = wf_80211_get_frame_type(pmgmt->frame_control);
    if (bupdate && frame_type == WF_80211_FRM_BEACON &&
        pscan_que_node->frame_type == WF_80211_FRM_PROBE_RESP &&
        pscan_que_node->ssid_type != WF_80211_HIDDEN_SSID_NOT_IN_USE)
    {
        return 0;
    }

    NODE_INFO_DBG("----------------------------");

    /* get frame type*/
    pscan_que_node->frame_type = frame_type;
    /* update timestamp */
    pscan_que_node->timestamp = wf_os_api_timestamp();

    /* get the phy status */
    if (pfrm_msg->is_phy_sta_valid)
    {
        pscan_que_node->signal_strength = pfrm_msg->phy_sta.signal_strength;
        pscan_que_node->signal_strength_scale =
            signal_scale_mapping(pfrm_msg->phy_sta.signal_strength);
        pscan_que_node->signal_qual = pfrm_msg->phy_sta.signal_qual;
    }
    NODE_INFO_DBG("RSSI=%d, signal stregth=%d, signal quality=%d",
                  translate_percentage_to_dbm(pscan_que_node->signal_strength_scale),
                  pscan_que_node->signal_strength_scale,
                  pscan_que_node->signal_qual);

    /* get bssid */
    wf_memcpy(pscan_que_node->bssid, pmgmt->bssid, sizeof(wf_80211_bssid_t));
    NODE_INFO_DBG("BSSID=%02X:%02X:%02X:%02X:%02X:%02X",
                  pscan_que_node->bssid[0], pscan_que_node->bssid[1],
                  pscan_que_node->bssid[2], pscan_que_node->bssid[3],
                  pscan_que_node->bssid[4], pscan_que_node->bssid[5]);

    /* get operation mode */
    if (!WF_80211_CAPAB_IS_MESH_STA_BSS(pmgmt->beacon.capab))
    {
        pscan_que_node->opr_mode =
            (pmgmt->beacon.capab & WF_80211_MGMT_CAPAB_ESS) ? WF_WLAN_OPR_MODE_MASTER :
            WF_WLAN_OPR_MODE_ADHOC;
    }
    else
    {
        if(wf_p2p_is_valid(pnic_info))
        {
            p2p_info_st *p2p_info = pnic_info->p2p;
            p2p_wd_info_st *pwdinfo = &(p2p_info->wdinfo);
            if (!wf_memcmp(&pmgmt->beacon.variable[2],
                           pwdinfo->p2p_wildcard_ssid, P2P_WILDCARD_SSID_LEN))
            {
                NODE_INFO_DBG(" This is a p2p device role = %d", pwdinfo->role);
            }
        }
        else
        {
            WLAN_MGMT_WARN("operation mode is mesh");
            return -1;
        }
    }

    /* get privacy */
    pscan_que_node->privacy = (wf_bool)(!!(pmgmt->beacon.capab & WF_80211_MGMT_CAPAB_PRIVACY));

    /* ies formation */
    pscan_que_node->ie_len = mgmt_len - WF_OFFSETOF(wf_80211_mgmt_t, beacon);
    if (pscan_que_node->ie_len >= sizeof(pscan_que_node->ies))
    {
        WLAN_MGMT_ERROR("ie data length too long");
        return -2;
    }
    wf_memcpy(&pscan_que_node->ies[0], &pmgmt->beacon, pscan_que_node->ie_len);
    pscan_que_node->ies[pscan_que_node->ie_len] = 0x0;

    pele_start = &pmgmt->beacon.variable[0];
    pele_end = &pele_start[mgmt_len - WF_OFFSETOF(wf_80211_mgmt_t, beacon.variable)];
    do
    {
        pie = (wf_80211_mgmt_ie_t *)pele_start;
        switch (pie->element_id)
        {
            case WF_80211_MGMT_EID_SSID :
                if (pie->len >= sizeof(pscan_que_node->ssid.data))
                {
                    NODE_INFO_DBG("invalid SSID length(%d)", pie->len);
                    return -3;
                }
                if (frame_type == WF_80211_FRM_PROBE_RESP)
                {
                    if (!pie->len)
                    {
                        WLAN_MGMT_WARN("ssid null");
                        return -4;
                    }
                }
                else
                {
                    pscan_que_node->ssid_type =
                        pie->len == 0 ? WF_80211_HIDDEN_SSID_ZERO_LEN :
                        pie->data[0] == '\0' ? WF_80211_HIDDEN_SSID_ZERO_CONTENTS :
                                               WF_80211_HIDDEN_SSID_NOT_IN_USE;
                }
                pscan_que_node->ssid.length = pie->len;
                wf_memcpy(pscan_que_node->ssid.data, pie->data, pie->len);
                pscan_que_node->ssid.data[pie->len] = '\0';
                NODE_INFO_DBG("SSID: %s", pscan_que_node->ssid.data);
                break;

            case WF_80211_MGMT_EID_SUPP_RATES :
            case WF_80211_MGMT_EID_EXT_SUPP_RATES :
                /* basic rate */
                if (support_rate_cnt == 0)
                {
                    wf_memset(pscan_que_node->spot_rate, 0,
                              sizeof(pscan_que_node->spot_rate));
                }
                if (pie->len == 0 || pie->len + support_rate_cnt >
                    sizeof(pscan_que_node->spot_rate))
                {
                    NODE_INFO_DBG("support rate number over limit");
                    return -4;
                }
                else
                {
                    wf_memcpy(&pscan_que_node->spot_rate[support_rate_cnt],
                              pie->data, pie->len);
                    support_rate_cnt += pie->len;
                }
                break;

            case WF_80211_MGMT_EID_DS_PARAMS :
                pdsss_para = (wf_80211_mgmt_dsss_parameter_t *)pie->data;
                pscan_que_node->channel = pdsss_para->current_channel;
                break;

            case WF_80211_MGMT_EID_HT_OPERATION :
                /* for 5G AP */
                pht_opt = (wf_80211_mgmt_ht_operation_t *)pie->data;
                pscan_que_node->channel = pht_opt->primary_chan;
                break;

            case WF_80211_MGMT_EID_HT_CAPABILITY :
                pscan_que_node->ht_spot = wf_true;
                pht_cap = (wf_80211_mgmt_ht_cap_t *)pie->data;
                wf_memcpy(&pscan_que_node->mcs, pht_cap->mcs_info.rx_mask,
                          sizeof(pscan_que_node->mcs));
                pscan_que_node->bw_40mhz = (wf_bool)
                    (!!(pht_cap->cap_info & WF_80211_MGMT_HT_CAP_SUP_WIDTH_20_40));
                pscan_que_node->short_gi = (wf_bool)
                    (!!(pht_cap->cap_info & (WF_80211_MGMT_HT_CAP_SGI_20 |
                                            WF_80211_MGMT_HT_CAP_SGI_40)));
                break;

            case WF_80211_MGMT_EID_VENDOR_SPECIFIC :
                if (!wf_memcmp(pie->data, wpa_oui, sizeof(wpa_oui)))
                {
                    if (pie->len <= sizeof(pscan_que_node->wpa_ie))
                    {
                        wf_memcpy(&pscan_que_node->wpa_ie, pie,
                                  WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len);
                    }
                    else
                    {
                        NODE_INFO_DBG("wpa length=%d over limite", pie->len);
                    }
                }
                else if (!wf_memcmp(pie->data, wps_oui, sizeof(wps_oui)))
                {
                    if (pie->len <= sizeof(pscan_que_node->wps_ie))
                    {
                        wf_memcpy(&pscan_que_node->wps_ie, pie,
                                  WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len);
                    }
                    else
                    {
                        NODE_INFO_DBG("wps length=%d over limite", pie->len);
                    }
                }
                break;

            case WF_80211_MGMT_EID_RSN :
                if (pie->len <= sizeof(pscan_que_node->rsn_ie))
                {
                    wf_memcpy(&pscan_que_node->rsn_ie, pie,
                              WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len);
                }
                else
                {
                    NODE_INFO_DBG("rsn length=%d over limite", pie->len);
                }
                break;
        }
        /* get next element point */
        pele_start = &pele_start[WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len];
    }
    while (pele_start < pele_end);

    /* get name information */
    for (i = 0; i < support_rate_cnt; i++)
    {
        switch (pscan_que_node->spot_rate[i] & 0x7F)
        {
            /* 0.5Mbps unit */
            case 2 : /* 2*0.5=1Mbps*/
            case 4 :
            case 11 :
            case 22 :
                cck_spot = wf_true;
                break;
            default :
                ofdm_spot = wf_true;
                break;
        }
    }
    if (cck_spot && ofdm_spot)
    {
        pscan_que_node->name =
            pscan_que_node->ht_spot ? WF_WLAN_BSS_NAME_IEEE80211_BGN :
            WF_WLAN_BSS_NAME_IEEE80211_BG;
    }
    else if (cck_spot)
    {
        pscan_que_node->name =
            pscan_que_node->ht_spot ? WF_WLAN_BSS_NAME_IEEE80211_BN :
            WF_WLAN_BSS_NAME_IEEE80211_B;
    }
    else
    {
        pscan_que_node->name =
            pscan_que_node->ht_spot ? WF_WLAN_BSS_NAME_IEEE80211_GN :
            WF_WLAN_BSS_NAME_IEEE80211_G;
    }

    return 0;
}
#undef NODE_INFO_DBG

static int
wlan_mgmt_scan_que_node_push (nic_info_st *pnic_info,
                              wf_wlan_mgmt_scan_que_node_t *pnew_node)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_mgmt_scan_que_t *pscan_que = &pwlan_mgmt_info->scan_que;
    wf_wlan_mgmt_scan_que_node_t *pscan_que_node;
    wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;

    wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
    {
        if (pnew_node->signal_strength > pscan_que_node->signal_strength)
        {
            break;
        }
    }
    wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);

    if (scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL)
    {
        return -1;
    }

    if (wlan_mgmt_scan_que_write_try(pscan_que))
    {
        return -2;
    }

    if (scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_BREAK)
    {
        wf_enque_prev(&pnew_node->list, &pscan_que_node->list, &pscan_que->ready);
    }
    else
    {
        wf_enque_tail(&pnew_node->list, &pscan_que->ready);
    }
    pnew_node->parent = &pscan_que->ready;

    wlan_mgmt_scan_que_write_post(pscan_que);

    return 0;
}

static int
wlan_mgmt_scan_que_node_del (nic_info_st *pnic_info,
                             wf_wlan_mgmt_scan_que_node_t *pnode)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_mgmt_scan_que_t *pscan_que = &pwlan_mgmt_info->scan_que;

    if (pnode->parent == &pscan_que->ready)
    {
        if (wlan_mgmt_scan_que_write_try(pscan_que))
        {
            return -1;
        }
        wf_deque(&pnode->list, &pscan_que->ready);
        wlan_mgmt_scan_que_write_post(pscan_que);
    }

    pnode->parent = &pscan_que->free;
    wf_enque_tail(&pnode->list, &pscan_que->free);

    return 0;
}

wf_inline static int
wlan_mgmt_scan_que_node_flush (nic_info_st *pnic_info)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_mgmt_scan_que_t *pscan_que = &pwlan_mgmt_info->scan_que;

    if (wf_que_is_empty(&pscan_que->ready))
    {
        WLAN_MGMT_INFO("scan queue is empty");
        return 0;
    }

    if (wlan_mgmt_scan_que_write_try(pscan_que))
    {
        return -1;
    }

    {
        wf_list_t *pos, *n;
        wf_list_for_each_safe(pos, n, wf_deque_head(&pscan_que->ready))
        {
            wf_wlan_mgmt_scan_que_node_t *pscan_que_node =
                wf_list_entry(pos, wf_wlan_mgmt_scan_que_node_t, list);
            /* flush all node except the bss whitch is current connected */
            if (is_cur_bss(pnic_info, pscan_que_node->bssid))
            {
                continue;
            }
            wf_enque_tail(pos, &pscan_que->free);
        }
    }

    wlan_mgmt_scan_que_write_post(pscan_que);

    return 0;
}

static int
wlan_mgmt_scan_que_refresh (nic_info_st *pnic_info, scan_que_refresh_msg_t *preq)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_mgmt_scan_que_t *pscan_que = &pwlan_mgmt_info->scan_que;
    wf_list_t *pos, *n;

    wf_list_for_each_safe(pos, n, wf_que_list_head(&pscan_que->ready))
    {
        wf_wlan_mgmt_scan_que_node_t *pscan_que_node =
            wf_list_entry(pos, wf_wlan_mgmt_scan_que_node_t, list);
        wf_u8 i;
        for (i = 0; i < preq->ch_num; i++)
        {
            if (pscan_que_node->channel == preq->ch_map[i])
            {
                break;
            }
        }
        if (i == preq->ch_num)
        {
            continue;
        }

        if (!pscan_que_node->updated)
        {
            if (pscan_que_node->ttl)
            {
                pscan_que_node->ttl--;
            }
            if (!pscan_que_node->ttl)
            {
                int rst;
                rst = wlan_mgmt_scan_que_node_del(pnic_info, pscan_que_node);
                if (rst)
                {
                    continue;
                }
            }
#if 0
            if (!pscan_que_node->ttl)
            {
                WLAN_MGMT_ERROR("----%s, 0", pscan_que_node->ssid.data);
            }
            else
            {
                WLAN_MGMT_WARN("!!!!%s, %d", pscan_que_node->ssid.data, pscan_que_node->ttl);
            }
#endif
        }
        else
        {
            pscan_que_node->updated = wf_false;
        }
    }

    return 0;
}

static int rx_frame_handle (nic_info_st *pnic_info)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_mgmt_scan_que_t *pscan_que = &pwlan_mgmt_info->scan_que;
    wf_msg_que_t *pmsg_que = &pwlan_mgmt_info->msg_que;
    wf_msg_t *pmsg;
    wf_bool uninstalling = wf_false;
    int rst;

    wf_os_api_thread_affinity(DEFAULT_CPU_ID);

    while (wf_os_api_thread_wait_stop(pwlan_mgmt_info->tid) == wf_false)
    {
        if (uninstalling)
        {
            continue;
        }

        /* wait new message */
        if (wf_msg_pop(pmsg_que, &pmsg))
        {
            wf_msleep(1);
            continue;
        }

        switch (pmsg->tag)
        {
            case WF_WLAN_MGMT_TAG_BEACON_FRAME :
            case WF_WLAN_MGMT_TAG_PROBERSP_FRAME :
            {
                rx_frm_msg_t *pfrm_msg = (void *)pmsg->value;
                wf_u16 frm_msg_len = pmsg->len;
                wf_wlan_mgmt_scan_que_node_t *pnew_node;

                rst = wlan_mgmt_scan_que_node_new(pnic_info, pfrm_msg, &pnew_node);
                if (rst < 0)
                {
                    WLAN_MGMT_WARN("new node fail, error code: %d", rst);
                    break;
                }
                rst = wlan_mgmt_scan_node_info(pnic_info, pfrm_msg, frm_msg_len,
                                               pnew_node, (wf_bool)(rst == 1));
                if (rst)
                {
                    WLAN_MGMT_DBG("make info fail, error code: %d", rst);
                    wlan_mgmt_scan_que_node_del(pnic_info, pnew_node);
                    break;
                }
                rst = wlan_mgmt_scan_que_node_push(pnic_info, pnew_node);
                if (rst)
                {
                    WLAN_MGMT_WARN("node input scan queue fail, error code: %d", rst);
                    wlan_mgmt_scan_que_node_del(pnic_info, pnew_node);
                }
                break;
            }

            case WF_WLAN_MGMT_TAG_SCAN_QUE_FLUSH :
            {
                WLAN_MGMT_INFO("scan queue flush");
                rst = wlan_mgmt_scan_que_node_flush(pnic_info);
                if (rst)
                {
                    WLAN_MGMT_WARN("scan queue flush fail, error code %d", rst);
                }
                break;
            }

            case WF_WLAN_MGMT_TAG_SCAN_QUE_REFRESH :
            {
                WLAN_MGMT_INFO("scan queue refresh");
                rst = wlan_mgmt_scan_que_refresh(pnic_info, (void *)pmsg->value);
                if (rst)
                {
                    WLAN_MGMT_WARN("scan queue refresh fail, error code %d", rst);
                }
                break;
            }

            case WF_WLAN_MGMT_TAG_UNINSTALL :
            {
                WLAN_MGMT_INFO("prepare to uninstall");
                while (wlan_mgmt_scan_que_write_try(pscan_que)) wf_msleep(1);
                uninstalling = wf_true;
                break;
            }

            default :
                WLAN_MGMT_ERROR("unknown message tag %d", pmsg->tag);
                break;
        }

        wf_msg_del(pmsg_que, pmsg);
    }

    wf_os_api_thread_exit(pwlan_mgmt_info->tid);

    return 0;
}

static phy_status_st *get_phy_status (prx_pkt_t ppkt)
{
    /* todo: some received manage frame may no contain phy_status filed,
    judge by pkt_info.phy_status, if true mains contain phy status, otherwise
    phy_status is invalid */
    return ppkt->pkt_info.phy_status ? &ppkt->phy_status : NULL;
}

static int frm_msg_send (wf_wlan_mgmt_info_t *pwlan_mgmt_info, wf_msg_tag_t tag,
                         phy_status_st *phy_sta,
                         wf_80211_mgmt_t *pmgmt, wf_u32 mgmt_len)
{
    wf_msg_que_t *pmsg_que = &pwlan_mgmt_info->msg_que;
    wf_msg_t *pmsg;
    int rst;

    /* new message entity */
    rst = wf_msg_new(pmsg_que, tag, &pmsg);
    if (rst)
    {
        WLAN_MGMT_WARN("msg new fail error code: %d", rst);
        return -1;
    }

    /* fill message value */
    {
        rx_frm_msg_t *prx_frm_msg = (void *)pmsg->value;
        wf_u16 value_size = WF_OFFSETOF(rx_frm_msg_t, mgmt_frm) + mgmt_len;
        if (value_size > pmsg->alloc_value_size)
        {
            WLAN_MGMT_ERROR("frame size(%d) over limite(%d)",
                            mgmt_len, pmsg->alloc_value_size);
            return -2;
        }
        pmsg->len = value_size;
        if (phy_sta)
        {
            prx_frm_msg->phy_sta            = *phy_sta;
            prx_frm_msg->is_phy_sta_valid   = wf_true;
        }
        else
        {
            prx_frm_msg->phy_sta.rssi               = -128;
            prx_frm_msg->phy_sta.signal_strength    = 0;
            prx_frm_msg->phy_sta.signal_qual        = 0;
            prx_frm_msg->is_phy_sta_valid           = wf_false;
        }
        wf_memcpy(&prx_frm_msg->mgmt_frm, pmgmt, mgmt_len);
    }

    /* new message entity */
    rst = wf_msg_push(pmsg_que, pmsg);
    if (rst)
    {
        WLAN_MGMT_WARN("wf_msg_push fail error code: %d", rst);
        return -3;
    }

    return 0;
}

int wf_wlan_mgmt_rx_frame (void *ptr)
{
    prx_pkt_t ppkt = ptr;
    nic_info_st *pnic_info;
    wf_wlan_mgmt_info_t *pwlan_mgmt_info;
    wf_80211_mgmt_t *pmgmt;
    wf_u32 mgmt_len;
    wdn_net_info_st *pwdn_info;
    wf_80211_frame_e frm_type;
    int rst;

    if (ppkt == NULL || ppkt->p_nic_info == NULL || ppkt->pdata == NULL)
    {
        WLAN_MGMT_ERROR("null pointer");
        return -1;
    }
    pnic_info = ppkt->p_nic_info;

    if (WF_CANNOT_RUN(pnic_info))
    {
        return -2;
    }

    pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    if (pwlan_mgmt_info == NULL)
    {
        WLAN_MGMT_ERROR("wlan_mgmt_info null pointer");
        return -3;
    }

    mgmt_len = ppkt->len;
    if (mgmt_len == 0)
    {
        WLAN_MGMT_ERROR("frame length zero");
        return -5;
    }
    pmgmt = (void *)ppkt->pdata;

//    WLAN_MGMT_DBG();

    pwdn_info = ppkt->wdn_info;
    if (pwdn_info)
    {
        pwdn_info->wdn_stats.rx_mgnt_pkts++;
    }

    frm_type = wf_80211_get_frame_type(pmgmt->frame_control);
    switch (frm_type)
    {
        case WF_80211_FRM_BEACON :
            if (mgmt_len > WF_80211_MGMT_BEACON_SIZE_MAX)
            {
                WLAN_MGMT_ERROR("beacon frame length(%d) over limited", mgmt_len);
                return -6;
            }

            /* scan process filter */
            rst = wf_scan_filter(pnic_info, pmgmt, mgmt_len);
            if (rst)
            {
                WLAN_MGMT_DBG("scan filter fail, error code: %d", rst);
                return -7;
            }

            /* send frame message */
            rst = frm_msg_send(pwlan_mgmt_info, WF_WLAN_MGMT_TAG_BEACON_FRAME,
                               get_phy_status(ppkt), pmgmt, mgmt_len);
            if (rst)
            {
                WLAN_MGMT_WARN("scan frame message send fail, error code: %d", rst);
                return -8;
            }

            /* update becon timestamp*/
            if (mac_addr_equal(pmgmt->bssid, wf_wlan_get_cur_bssid(pnic_info)))
            {
                wf_wlan_network_t *pcur_network = &pwlan_mgmt_info->cur_network;
//              WLAN_MGMT_DBG("update beacon timestamp");
                pcur_network->timestamp = pmgmt->beacon.timestamp;
                pcur_network->bcn_interval = pmgmt->beacon.intv;
//              WLAN_MGMT_DBG("beacon.timestamp:%lld",pcur_network->timestamp);
            }

#ifdef CFG_ENABLE_ADHOC_MODE
            if (get_sys_work_mode(pnic_info) == WF_ADHOC_MODE)
            {
                wf_adhoc_work(pnic_info, (void *)pmgmt, mgmt_len);
            }
#endif
            break;

        case WF_80211_FRM_PROBE_REQ :
            if(wf_p2p_is_valid(pnic_info))
            {
                wf_p2p_proc_probereq(pnic_info, pmgmt, mgmt_len);
            }
#if defined(CFG_ENABLE_ADHOC_MODE)
        if (get_sys_work_mode(pnic_info) == WF_ADHOC_MODE)
        {
            wf_adhoc_do_probrsp(pnic_info, pmgmt, mgmt_len);
            break;
        }
#endif

#ifdef CFG_ENABLE_AP_MODE
        if (get_sys_work_mode(pnic_info) == WF_MASTER_MODE)
        {
            wf_ap_probe(pnic_info, pmgmt, mgmt_len);
        }
#endif
        break;

        case WF_80211_FRM_PROBE_RESP :
            if (mgmt_len > WF_80211_MGMT_PROBERSP_SIZE_MAX)
            {
                WLAN_MGMT_ERROR("probersp frame length(%d) over limited", mgmt_len);
                return -9;
            }

            /* scan process filter */
            rst = wf_scan_filter(pnic_info, pmgmt, mgmt_len);
            if (rst)
            {
                WLAN_MGMT_DBG("scan filter fail, error code: %d", rst);
                return -10;
            }

            /* send frame message */
            rst = frm_msg_send(pwlan_mgmt_info, WF_WLAN_MGMT_TAG_PROBERSP_FRAME,
                               get_phy_status(ppkt), pmgmt, mgmt_len);
            if (rst)
            {
                WLAN_MGMT_WARN("scan frame message send fail, error code: %d", rst);
                return -11;
            }
            break;

        case WF_80211_FRM_AUTH :
            wf_auth_frame_parse(pnic_info, pwdn_info, pmgmt, mgmt_len);
            break;

        case WF_80211_FRM_DEAUTH :
            wf_deauth_frame_parse(pnic_info, pwdn_info, pmgmt, mgmt_len);
            break;

#ifdef CFG_ENABLE_AP_MODE
        case WF_80211_FRM_ASSOC_REQ :
        case WF_80211_FRM_REASSOC_REQ :
            wf_assoc_ap_work(pnic_info, pwdn_info, (void *)pmgmt, mgmt_len);
            break;
#endif

        case WF_80211_FRM_ASSOC_RESP :
        case WF_80211_FRM_REASSOC_RESP :
            wf_assoc_frame_parse(pnic_info, pwdn_info, pmgmt, mgmt_len);
            break;

        case WF_80211_FRM_DISASSOC :
            wf_disassoc_frame_parse(pnic_info, pwdn_info, pmgmt, mgmt_len);
            break;

        case WF_80211_FRM_ACTION :
            wf_action_frame_process(pnic_info, pwdn_info, pmgmt, mgmt_len);
            break;

        default :
            WLAN_MGMT_WARN("untreated frame type: %d", frm_type);
            break;
    }

    return 0;
}

static int wlan_mgmt_scan_que_init (wf_wlan_mgmt_info_t *pwlan_mgmt_info)
{
    wf_wlan_mgmt_scan_que_t *pscan_que = &pwlan_mgmt_info->scan_que;
    int i;

    WLAN_MGMT_DBG();

    wf_que_init(&pscan_que->free, WF_LOCK_TYPE_NONE);
    wf_que_init(&pscan_que->ready, WF_LOCK_TYPE_NONE);
    for (i = 0; i < WLAN_MGMT_SCAN_QUE_DEEP; i++)
    {
        wf_wlan_mgmt_scan_que_node_t *pnode
            = wf_kzalloc(sizeof(wf_wlan_mgmt_scan_que_node_t));
        if (pnode == NULL)
        {
            WLAN_MGMT_ERROR("wf_kzalloc failed");
            return -1;
        }
        wf_enque_head(&pnode->list, &pscan_que->free);
        pnode->parent = &pscan_que->free;
    }
    wf_lock_init(&pscan_que->lock, WF_LOCK_TYPE_IRQ);
    pscan_que->read_cnt = 0;
    wf_os_api_sema_init(&pscan_que->sema, 1);

    return 0;
}

static int wlan_mgmt_scan_que_deinit (wf_wlan_mgmt_info_t *pwlan_mgmt_info)
{
    wf_wlan_mgmt_scan_que_t *pscan_que = &pwlan_mgmt_info->scan_que;

    WLAN_MGMT_DBG();

    while (wf_true)
    {
        wf_que_list_t *pque_list = wf_deque_head(&pscan_que->free);
        if (pque_list == NULL)
        {
            break;
        }
        wf_kfree(wf_list_entry(pque_list, wf_wlan_mgmt_scan_que_node_t, list));
    }
    wf_que_deinit(&pscan_que->free);

    while (wf_true)
    {
        wf_que_list_t *pque_list = wf_deque_head(&pscan_que->ready);
        if (pque_list == NULL)
        {
            break;
        }
        wf_kfree(wf_list_entry(pque_list, wf_wlan_mgmt_scan_que_node_t, list));
    }
    wf_que_deinit(&pscan_que->ready);

    wf_lock_term(&pscan_que->lock);
    wf_os_api_sema_free(&pscan_que->sema);

    return 0;
}

static int wlan_mgmt_msg_init (wf_wlan_mgmt_info_t *pwlan_mgmt_info)
{
    wf_msg_que_t *pmsg_que = &pwlan_mgmt_info->msg_que;

    wf_msg_init(pmsg_que);
    return (wf_msg_alloc(pmsg_que, WF_WLAN_MGMT_TAG_BEACON_FRAME,
                         WF_OFFSETOF(rx_frm_msg_t, mgmt_frm) +
                         WF_80211_MGMT_BEACON_SIZE_MAX, 8) ||
            wf_msg_alloc(pmsg_que, WF_WLAN_MGMT_TAG_PROBERSP_FRAME,
                         WF_OFFSETOF(rx_frm_msg_t, mgmt_frm) +
                         WF_80211_MGMT_PROBERSP_SIZE_MAX, 8) ||
            wf_msg_alloc(pmsg_que, WF_WLAN_MGMT_TAG_SCAN_QUE_FLUSH, 0, 1) ||
            wf_msg_alloc(pmsg_que, WF_WLAN_MGMT_TAG_UNINSTALL, 0, 1) ||
            wf_msg_alloc(pmsg_que, WF_WLAN_MGMT_TAG_SCAN_QUE_REFRESH,
                         sizeof(scan_que_refresh_msg_t), 2)) ? -1 : 0;
}

wf_inline static void wlan_mgmt_msg_deinit (wf_wlan_mgmt_info_t *pwlan_mgmt_info)
{
    wf_msg_deinit(&pwlan_mgmt_info->msg_que);
}

int wf_wlan_mgmt_send_msg (nic_info_st *pnic_info, wf_msg_tag_t tag)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_msg_que_t *pmsg_que = &pwlan_mgmt_info->msg_que;
    wf_msg_t *pmsg;
    int rst;

    /* new message entity */
    rst = wf_msg_new(pmsg_que, tag, &pmsg);
    if (rst)
    {
        WLAN_MGMT_WARN("msg new fail error code: %d", rst);
        return -1;
    }

    /* new message entity */
    rst = wf_msg_push(pmsg_que, pmsg);
    if (rst)
    {
        WLAN_MGMT_WARN("wf_msg_push fail error code: %d", rst);
        return -2;
    }

    return 0;
}

int wf_wlan_mgmt_scan_que_refresh (nic_info_st *pnic_info,
                                   wf_u8 *pch, wf_u8 ch_num)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_msg_que_t *pmsg_que = &pwlan_mgmt_info->msg_que;
    wf_msg_t *pmsg;
    int rst;

    if (pnic_info == NULL || pch == NULL)
    {
        WLAN_MGMT_ERROR("null point");
        return -1;
    }

    if (ch_num == 0 || ch_num > WF_FIELD_SIZEOF(scan_que_refresh_msg_t, ch_map))
    {
        WLAN_MGMT_ERROR("invalid channel number %d", ch_num);
        return -2;
    }

    /* new message entity */
    rst = wf_msg_new(pmsg_que, WF_WLAN_MGMT_TAG_SCAN_QUE_REFRESH, &pmsg);
    if (rst)
    {
        WLAN_MGMT_WARN("refresh msg new fail error code: %d", rst);
        return -3;
    }

    /* load value */
    if (sizeof(scan_que_refresh_msg_t) > pmsg->alloc_value_size)
    {
        WLAN_MGMT_ERROR("msg->value length(%d) error", pmsg->alloc_value_size);
        return -4;
    }
    {
        scan_que_refresh_msg_t *ptr = (void *)pmsg->value;
        ptr->ch_num = ch_num;
        wf_memcpy(ptr->ch_map, pch, ch_num);
    }

    /* new message entity */
    rst = wf_msg_push(pmsg_que, pmsg);
    if (rst)
    {
        WLAN_MGMT_WARN("wf_msg_push fail error code: %d", rst);
        return -5;
    }

    return 0;
}

int wf_wlan_mgmt_init (nic_info_st *pnic_info)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info;
    int rst;

    WLAN_MGMT_INFO("ndev_id:%d",pnic_info->ndev_id);

    pwlan_mgmt_info = wf_kzalloc(sizeof(wf_wlan_mgmt_info_t));
    if (pwlan_mgmt_info == NULL)
    {
        WLAN_MGMT_ERROR("wf_kzalloc failed");
        return -1;
    }
    pnic_info->wlan_mgmt_info = pwlan_mgmt_info;

    rst = wlan_mgmt_scan_que_init(pwlan_mgmt_info);
    if (rst)
    {
        WLAN_MGMT_ERROR("scan queue initilize fail, error code: %d", rst);
        return -2;
    }

    rst = wlan_mgmt_msg_init(pwlan_mgmt_info);
    if (rst)
    {
        WLAN_MGMT_ERROR("message queue initilize fail, error code: %d", rst);
        return -3;
    }

    /* create thread for rx frame handle */
    sprintf(pwlan_mgmt_info->name,
            pnic_info->virNic ? "wlan_mgmt_info:vir%d_s%d" : "wlan_mgmt_info:wlan%d_s%d",
            pnic_info->hif_node_id, pnic_info->ndev_id);
    pwlan_mgmt_info->tid =
        wf_os_api_thread_create(&pwlan_mgmt_info->tid, pwlan_mgmt_info->name,
                                (void *)rx_frame_handle, pnic_info);
    if (pwlan_mgmt_info->tid == NULL)
    {
        WLAN_MGMT_ERROR("create thread failed");
        return -4;
    }
    wf_os_api_thread_wakeup(pwlan_mgmt_info->tid);

    return 0;
}

int wf_wlan_mgmt_term (nic_info_st *pnic_info)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;

    if (pwlan_mgmt_info == NULL)
    {
        WLAN_MGMT_ERROR("null point");
        return 0;
    }

    WLAN_MGMT_INFO();

    {
        wf_msg_que_t *pmsg_que = &pwlan_mgmt_info->msg_que;
        wf_msg_t *pnew_msg;
        int rst;

        rst = wf_msg_new(pmsg_que, WF_WLAN_MGMT_TAG_UNINSTALL, &pnew_msg);
        if (rst)
        {
            WLAN_MGMT_ERROR("new message fail, error code: %d", rst);
        }
        wf_msg_push(pmsg_que, pnew_msg);
    }

    /* destory thread */
    if (pwlan_mgmt_info->tid)
    {
        wf_os_api_thread_destory(pwlan_mgmt_info->tid);
        pwlan_mgmt_info->tid = 0;
    }

    /* free wlan info */
    wlan_mgmt_scan_que_deinit(pwlan_mgmt_info);
    wlan_mgmt_msg_deinit(pwlan_mgmt_info);
    wf_kfree(pwlan_mgmt_info);
    pnic_info->wlan_mgmt_info = NULL;

    return 0;
}

void wf_wlan_set_cur_ssid (nic_info_st *pnic_info, wf_wlan_ssid_t *pssid)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;

    wf_memcpy(&pwlan_mgmt_info->cur_network.ssid, pssid, sizeof(wf_wlan_ssid_t));
}

wf_wlan_ssid_t *wf_wlan_get_cur_ssid (nic_info_st *pnic_info)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;

    return &pwlan_mgmt_info->cur_network.ssid;
}

void wf_wlan_set_cur_bssid (nic_info_st *pnic_info, wf_u8 *bssid)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;

    wf_memcpy(pwlan_mgmt_info->cur_network.bssid, bssid, MAC_ADDR_LEN);
}


wf_inline wf_u8 *wf_wlan_get_cur_bssid (nic_info_st *pnic_info)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;

    return (wf_u8 *)pwlan_mgmt_info->cur_network.bssid;
}


void wf_wlan_set_cur_channel (nic_info_st *pnic_info, wf_u8 channel)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;

    pwlan_mgmt_info->cur_network.channel = channel;
}


wf_u8 wf_wlan_get_cur_channel (nic_info_st *pnic_info)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;

    return pwlan_mgmt_info->cur_network.channel;
}

void wf_wlan_set_cur_bw (nic_info_st *pnic_info, CHANNEL_WIDTH bw)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;

    pwlan_mgmt_info->cur_network.bw = bw;
}

CHANNEL_WIDTH wf_wlan_get_cur_bw (nic_info_st *pnic_info)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;

    return pwlan_mgmt_info->cur_network.bw;
}

static wf_u16 mcs_rate_func (wf_u8 bw_40MHz, wf_u8 short_GI, wf_u8 *MCS_rate)
{
    wf_u16 max_rate = 0;

    if (MCS_rate[0] & BIT(7))
    {
        max_rate = (bw_40MHz) ? ((short_GI) ? 1500 : 1350) : ((short_GI) ? 722 : 650);
    }
    else if (MCS_rate[0] & BIT(6))
    {
        max_rate = (bw_40MHz) ? ((short_GI) ? 1350 : 1215) : ((short_GI) ? 650 : 585);
    }
    else if (MCS_rate[0] & BIT(5))
    {
        max_rate = (bw_40MHz) ? ((short_GI) ? 1200 : 1080) : ((short_GI) ? 578 : 520);
    }
    else if (MCS_rate[0] & BIT(4))
    {
        max_rate = (bw_40MHz) ? ((short_GI) ? 900 : 810) : ((short_GI) ? 433 : 390);
    }
    else if (MCS_rate[0] & BIT(3))
    {
        max_rate = (bw_40MHz) ? ((short_GI) ? 600 : 540) : ((short_GI) ? 289 : 260);
    }
    else if (MCS_rate[0] & BIT(2))
    {
        max_rate = (bw_40MHz) ? ((short_GI) ? 450 : 405) : ((short_GI) ? 217 : 195);
    }
    else if (MCS_rate[0] & BIT(1))
    {
        max_rate = (bw_40MHz) ? ((short_GI) ? 300 : 270) : ((short_GI) ? 144 : 130);
    }
    else if (MCS_rate[0] & BIT(0))
    {
        max_rate = (bw_40MHz) ? ((short_GI) ? 150 : 135) : ((short_GI) ? 72 : 65);
    }

    return max_rate;
}

int wf_wlan_get_max_rate (nic_info_st *pnic_info, wf_u8 *mac, wf_u16 *max_rate)
{
    wf_u16 rate = 0;
    wf_u8 short_GI = 0;
    int i = 0;
    wdn_net_info_st *pwdn_info = NULL;

    pwdn_info = wf_wdn_find_info(pnic_info, mac);
    if (NULL == pwdn_info)
    {
        return -1;
    }

    short_GI = wf_ra_sGI_get(pwdn_info, 1);
    if ((pwdn_info->network_type) & (WIRELESS_11_24N))
    {
        *max_rate = mcs_rate_func(((pwdn_info->bw_mode == CHANNEL_WIDTH_40) ? 1 : 0),
                                  short_GI, pwdn_info->datarate);
    }
    else
    {
        for (i = 0; i < pwdn_info->datarate_len; i++)
        {
            rate = pwdn_info->datarate[i] & 0x7F;
            if (rate > *max_rate)
            {
                *max_rate = rate;
            }
        }

        for (i = 0; i < pwdn_info->ext_datarate_len; i++)
        {
            rate = pwdn_info->ext_datarate[i] & 0x7F;
            if (rate > *max_rate)
            {
                *max_rate = rate;
            }
        }

        *max_rate = *max_rate * 10 / 2;
    }

    return 0;
}


int wf_wlan_get_signal_and_qual (nic_info_st *pnic_info, wf_u8 *qual, wf_u8 *level)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = (wf_wlan_mgmt_info_t *)pnic_info->wlan_mgmt_info;
    wf_wlan_mgmt_scan_que_node_t *pscan_que_node = NULL;
    wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst = WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL;

    wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
    {
        if (!wf_memcmp(pscan_que_node->bssid, pwlan_mgmt_info->cur_network.bssid, MAC_ADDR_LEN))
        {
            *qual = pscan_que_node->signal_qual;
            *level = pscan_que_node->signal_strength;
            break;
        }
    }
    wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);

    return scan_que_for_rst;
}

