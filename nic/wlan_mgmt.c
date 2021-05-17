/* 802.11 bgn managment frame compose and parse */
#include "common.h"
#include "wf_debug.h"

#if 0
#define WLAN_DBG(fmt, ...)      LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define WLAN_WARN(fmt, ...)     LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define WLAN_DBG(fmt, ...)
#define WLAN_WARN(fmt, ...)
#endif

#define BEACON_FRAME_SIZE_MAX               (WF_OFFSETOF(wf_80211_mgmt_t, beacon.variable) + WF_80211_IES_SIZE_MAX) /* for beacon variable max size */
#define PROBE_RESP_FRAME_SIZE_MAX           (WF_OFFSETOF(wf_80211_mgmt_t, probe_resp.variable) + WF_80211_IES_SIZE_MAX) /* for probe response variable max size */

#define RX_FRAME_BEACON_QUE_DEEP            8
#define RX_FRAME_BEACON_QUE_NODE_SIZE       (sizeof(wf_wlan_queue_node_t) + sizeof(wf_wlan_msg_t) + BEACON_FRAME_SIZE_MAX)
#define RX_FRAME_PROBERSP_QUE_DEEP          8
#define RX_FRAME_PROBERSP_QUE_NODE_SIZE     (sizeof(wf_wlan_queue_node_t) + sizeof(wf_wlan_msg_t) + PROBE_RESP_FRAME_SIZE_MAX)

#define NOTIFY_QUE_DEEP                     1
#define NOTIFY_QUE_NODE_SIZE                (sizeof(wf_wlan_queue_node_t) + WF_FIELD_SIZEOF(wf_wlan_msg_t, tag))

#define SCANNED_INFO_QUE_DEEP               64
#define SCANNED_INFO_QUE_NODE_SIZE          (sizeof(wf_wlan_queue_node_t) + sizeof(wf_wlan_scanned_info_t))

/*
 * Queue function
 */
static wf_inline void queue_lock (wf_wlan_queue_t *pque)
{
    wf_lock_lock(&pque->lock);
}
static wf_inline void queue_unlock (wf_wlan_queue_t *pque)
{
    wf_lock_unlock(&pque->lock);
}
static wf_inline wf_wlan_queue_node_t *queue_head (wf_wlan_queue_t *pque)
{
    return WF_CONTAINER_OF(pque->head.pnext, wf_wlan_queue_node_t, list);
}
static wf_inline wf_wlan_queue_node_t *queue_tail (wf_wlan_queue_t *pque)
{
    return WF_CONTAINER_OF(pque->head.prev, wf_wlan_queue_node_t, list);
}
static wf_inline wf_bool queue_empty (wf_wlan_queue_t *pque)
{
    return wf_list_is_empty(&pque->head);
}
static wf_inline void _enqueue (wf_wlan_queue_t *pque,
                                wf_list_t *phead,
                                wf_wlan_queue_node_t *pnode, wf_bool flag)
{
    queue_lock(pque);
    if (flag)
    {
        wf_list_insert_tail(&pnode->list, phead);
    }
    else
    {
        wf_list_insert_head(&pnode->list, phead);
    }
    pque->count++;
    queue_unlock(pque);
}
static wf_inline void enqueue_head (wf_wlan_queue_t *pque,
                                    wf_wlan_queue_node_t *pnode)
{
    _enqueue(pque, &pque->head, pnode, 0);
}
static wf_inline void enqueue_tail (wf_wlan_queue_t *pque,
                                    wf_wlan_queue_node_t *pnode)
{
    _enqueue(pque, &pque->head, pnode, 1);
}
static wf_inline void enqueue_next (wf_wlan_queue_t *pque,
                                    wf_wlan_queue_node_t *point,
                                    wf_wlan_queue_node_t *pnode)
{
    _enqueue(pque, &point->list, pnode, 0);
}
static wf_inline void enqueue_prev (wf_wlan_queue_t *pque,
                                    wf_wlan_queue_node_t *point,
                                    wf_wlan_queue_node_t *pnode)
{
    _enqueue(pque, &point->list, pnode, 1);
}
static int dequeue (wf_wlan_queue_t *pque, wf_wlan_queue_node_t *pnode)
{
    if (queue_empty(pque))
    {
        return -1;
    }

    queue_lock(pque);
    wf_list_delete(&pnode->list);
    pque->count--;
    queue_unlock(pque);

    return 0;
}
static wf_inline void queue_init (wf_wlan_queue_t *pque)
{
    wf_list_init(&pque->head);
    wf_lock_init(&pque->lock, WF_LOCK_TYPE_IRQ);
    pque->count = 0;
}
static wf_inline void queue_deinit (wf_wlan_queue_t *pque)
{
    wf_lock_term(&pque->lock);
}


/*
 * scanned
 */
int wf_wlan_scanned_acce_try (wf_wlan_scanned_t *pscanned)
{
    int ret = 0;

    if (pscanned == NULL)
    {
        ret = -1;
        goto exit;
    }

    wf_lock_lock(&pscanned->lock);
    if (pscanned->count == 0xFF)
    {
        ret = -2;
    }
    else
    {
        if (pscanned->count)
        {
            pscanned->count++;
        }
        else
        {
            wf_lock_unlock(&pscanned->lock);
            if (!wf_os_api_sema_try(&pscanned->sema))
            {
                wf_lock_lock(&pscanned->lock);
                pscanned->count++;
            }
            else
            {
                ret = -3;
                goto exit;
            }
        }
    }
    wf_lock_unlock(&pscanned->lock);

exit :
    return ret;
}

int wf_wlan_scanned_acce_post (wf_wlan_scanned_t *pscanned)
{
    int ret = 0;

    if (pscanned == NULL)
    {
        ret = -1;
        goto exit;
    }

    wf_lock_lock(&pscanned->lock);
    if (pscanned->count > 0)
    {
        pscanned->count--;
        if (pscanned->count == 0)
        {
            wf_lock_unlock(&pscanned->lock);
            wf_os_api_sema_post(&pscanned->sema);
            goto exit;
        }
    }
    else
    {
        ret = -2;
    }
    wf_lock_unlock(&pscanned->lock);

exit :
    return ret;
}

int wf_wlan_scanned_mdfy_pend (wf_wlan_scanned_t *pscanned)
{
    int ret = 0;

    if (pscanned == NULL)
    {
        ret = -1;
        goto exit;
    }

    if (wf_os_api_sema_wait(&pscanned->sema))
    {
        ret = -2;
    }

exit:
    return ret;
}

int wf_wlan_scanned_mdfy_post (wf_wlan_scanned_t *pscanned)
{
    int ret = 0;

    if (pscanned == NULL)
    {
        ret = -1;
        goto exit;
    }

    wf_os_api_sema_post(&pscanned->sema);

exit :
    return ret;
}

/*
 * message
 */
int msg_que_send (wf_wlan_info_t *pwlan_info, wf_wlan_queue_node_t *pnode)
{
    wf_wlan_msg_que_t *pmsg_que = &pwlan_info->msg_que;

    enqueue_tail(&pmsg_que->que, pnode);
    wf_os_api_sema_post(&pmsg_que->sema);

    return 0;
}

int msg_que_wait (wf_wlan_info_t *pwlan_info, wf_wlan_queue_node_t **ppnode)
{
    wf_wlan_msg_que_t *pmsg_que = &pwlan_info->msg_que;
    wf_wlan_queue_node_t *pnode;

    if (wf_os_api_sema_wait(&pmsg_que->sema))
    {
        return -2;
    }

    if (queue_empty(&pmsg_que->que))
    {
        return -3;
    }

    pnode = queue_head(&pmsg_que->que);
    dequeue(&pmsg_que->que, pnode);
    *ppnode = pnode;

    return 0;
}


static int is_asoc_bssid (nic_info_st *pnic_info, wf_80211_bssid_t bssid)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_bool fconnect;

    wf_mlme_get_connect(pnic_info, &fconnect);
    return (fconnect && mac_addr_equal(pwlan_info->cur_network.bssid, bssid));
}

static wf_inline int scanned_node_pull (nic_info_st *pnic_info,
                                        msg_frame_t *pframe,
                                        wf_wlan_queue_node_t **ppnode)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_scanned_t *pscanned = &pwlan_info->scanned;
    wf_bool fscanned_empty;
    wf_bool fscanned_full;
    wf_bool frssi_valid;
    wf_wlan_signal_strength_t signal_strength;
    wf_bool fmatch = wf_false;
    wf_wlan_queue_node_t *pnode = NULL;
    wf_wlan_queue_node_t *pnode_mark = NULL;
    wf_wlan_scanned_info_t *pscanned_info;
    int scanned_ret;
    int ret;

    /* todo:
       1、scanned queue head node's rssi is the strongest one, tail node's rssi
       is the worst, all of node in the scanned queue is order by rssi value.
       2、the income frame's rssi maybe invalid, if scanned queue is full, ignore
       the frame, else use the default rssi value(-128) replase it, and node insert
       to queue tail. */
    fscanned_empty  = queue_empty(&pscanned->que);
    fscanned_full   = queue_empty(&pscanned->free);
    frssi_valid     = pframe->phy_sta.valid_flag;
    signal_strength = pframe->phy_sta.signal_strength;
    if (!fscanned_empty)
    {
        /* find node which has the same mac address in scanned queue */
        wf_wlan_scanned_each_begin(pscanned_info, pnic_info)
        {
            if (mac_addr_equal(pframe->mgmt.bssid, pscanned_info->bssid))
            {
                /* match it */
                pnode = pnode_;
                fmatch = wf_true;
                break;
            }
            /* todo: the head node's rssi is strongest in all of the scanned
            queue, so the findout an node whitch rssi the smallest as the mask
            node, except the current association bssid  */
            if (frssi_valid && signal_strength >= pscanned_info->signal_strength &&
                !is_asoc_bssid(pnic_info, pscanned_info->bssid))
            {
                pnode_mark = pnode_;
            }
        }
        wf_wlan_scanned_each_end(pnic_info, &scanned_ret);

        if (scanned_ret == WF_WLAN_SCANNED_EACH_RET_FAIL)
        {
            ret = -1;
            goto exit;
        }
    }

    if (wf_wlan_scanned_mdfy_pend(pscanned))
    {
        ret = -2;
        goto exit;
    }
    /* fscanned_empty fscanned_full fmatch frssi_valid */

    if (fscanned_empty)
    {
        /* if no node in scanned queue and the new bss's rssi is valid,
        write a worst rssi as default value */
        if (!frssi_valid)
        {
            pframe->phy_sta.rssi            = -128;
            pframe->phy_sta.signal_strength = 0;
            pframe->phy_sta.signal_qual     = 0;
            pframe->phy_sta.valid_flag      = wf_true;
        }
        pnode = queue_head(&pscanned->free);
        dequeue(&pscanned->free, pnode);
        ret = 0;
    }
    else if (fmatch)
    {
        /* update the match node, no matter how rssi value,
        because the invalid rssi value will no be used in
        the flow make information process. */
        dequeue(&pscanned->que, pnode);
        ret = 1;
    }
    else if (fscanned_full)
    {
        /* find a new bss frame and scanned queue has full, only with
        valid rssi node will be use */
        if (frssi_valid)
        {
            /* if no mark node is find, that is to say the frame's rssi,
            is small than all of node's rssi in the scanned queue, and
            replace the worst node in the scanned queue. else use the mark
            node. */
            if (pnode_mark == NULL)
            {
                /* ignore invalid frame. */
                pnode = NULL;
                ret = -3;
            }
            else
            {
                pnode = pnode_mark;
                dequeue(&pscanned->que, pnode);
                ret = 2;
            }
        }
        else
        {
            /* ignore invalid frame. */
            pnode = NULL;
            ret = -4;
        }
    }
    else
    {
        if (!frssi_valid)
        {
            pframe->phy_sta.rssi            = -128;
            pframe->phy_sta.signal_strength = 0;
            pframe->phy_sta.signal_qual     = 0;
            pframe->phy_sta.valid_flag      = wf_true;
        }
        pnode = queue_head(&pscanned->free);
        dequeue(&pscanned->free, pnode);
        ret = 3;
    }

    wf_wlan_scanned_mdfy_post(pscanned);

    *ppnode = pnode;

exit :
    return ret;
}


static wf_inline int scanned_node_push (nic_info_st *pnic_info,
                                        wf_wlan_queue_node_t *pnode)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_scanned_t *pscanned = &pwlan_info->scanned;
    wf_wlan_signal_strength_t signal_strength;
    wf_wlan_scanned_info_t *pscanned_info;
    wf_wlan_queue_node_t *point = NULL;
    int scanned_ret;

    /* insert node to scanned queue, under right order */
    signal_strength = ((wf_wlan_scanned_info_t *)pnode->data)->signal_strength;
try_again :
    wf_wlan_scanned_each_begin(pscanned_info, pnic_info)
    {
        point = pnode_;
        if (signal_strength >= pscanned_info->signal_strength)
        {
            break;
        }
    }
    wf_wlan_scanned_each_end(pnic_info, &scanned_ret);

    if (scanned_ret == WF_WLAN_SCANNED_EACH_RET_FAIL)
    {
        if (wf_wlan_is_uninstalling(pnic_info))
        {
            return -1;
        }
        else
        {
            wf_msleep(1);
            goto try_again;
        }
    }
    else
    {
        while (wf_wlan_scanned_mdfy_pend(pscanned)) wf_msleep(1);
        if (scanned_ret == WF_WLAN_SCANNED_EACH_RET_BREAK)
        {
            enqueue_prev(&pscanned->que, point, pnode);
        }
        else
        {
            enqueue_tail(&pscanned->que, pnode);
        }
        wf_wlan_scanned_mdfy_post(pscanned);
    }

    return 0;
}


static int make_scanned_info (msg_frame_t *pmsg_frame, wf_u16 msg_frame_len,
                              wf_wlan_scanned_info_t *pscanned_info)
{
    wf_80211_mgmt_t *pmgmt = (wf_80211_mgmt_t *)&pmsg_frame->mgmt;
    wf_u16 mgmt_len = msg_frame_len - WF_OFFSETOF(msg_frame_t, mgmt);
    wf_80211_frame_e frame_type;
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

    WLAN_DBG("----------------------------");

    frame_type = wf_80211_get_frame_type(pmgmt->frame_control);
    if (!(frame_type == WF_80211_FRM_BEACON ||
          frame_type == WF_80211_FRM_PROBE_RESP))
    {
        WLAN_WARN("invalid frame type=%d", frame_type);
    }

    /* get frame type*/
    pscanned_info->frame_type = frame_type;
    /* update timestamp */
    pscanned_info->timestamp = wf_os_api_timestamp();

    /* get the phy status */
    if (pmsg_frame->phy_sta.valid_flag)
    {
        pscanned_info->signal_strength = pmsg_frame->phy_sta.signal_strength;
        pscanned_info->signal_strength_scale =
            signal_scale_mapping(pmsg_frame->phy_sta.signal_strength);
        pscanned_info->signal_qual = pmsg_frame->phy_sta.signal_qual;
    }
    WLAN_DBG("RSSI=%d, signal stregth=%d, signal quality=%d",
             translate_percentage_to_dbm(pscanned_info->signal_strength_scale),
             pscanned_info->signal_strength_scale,
             pscanned_info->signal_qual);

    /* get bssid */
    wf_memcpy(pscanned_info->bssid, pmgmt->bssid, sizeof(wf_80211_bssid_t));
    WLAN_DBG("BSSID=%02X:%02X:%02X:%02X:%02X:%02X",
             pscanned_info->bssid[0], pscanned_info->bssid[1],
             pscanned_info->bssid[2], pscanned_info->bssid[3],
             pscanned_info->bssid[4], pscanned_info->bssid[5] );

    /* get operation mode */
    if (!WF_80211_CAPAB_IS_MESH_STA_BSS(pmgmt->beacon.capab))
    {
        pscanned_info->opr_mode =
            (pmgmt->beacon.capab & WF_80211_MGMT_CAPAB_ESS) ? WF_WLAN_OPR_MODE_MASTER :
            WF_WLAN_OPR_MODE_ADHOC;
    }
    else
    {
        WLAN_DBG("operation mode is mesh");
        return WF_RETURN_FAIL;
    }

    /* get privacy */
    pscanned_info->privacy = !!(pmgmt->beacon.capab & WF_80211_MGMT_CAPAB_PRIVACY);

    /* ies formation */
    pscanned_info->ie_len = mgmt_len - WF_OFFSETOF(wf_80211_mgmt_t, beacon);
    wf_memcpy(&pscanned_info->ies[0], &pmgmt->beacon, pscanned_info->ie_len);

    pele_start = &pmgmt->beacon.variable[0];
    pele_end = &pele_start[mgmt_len - WF_OFFSETOF(wf_80211_mgmt_t, beacon.variable)];
    do
    {
        pie = (wf_80211_mgmt_ie_t *)pele_start;
        switch (pie->element_id)
        {
            case WF_80211_MGMT_EID_SSID :
                if (pie->len > sizeof(pscanned_info->ssid.data) ||
                    pie->len == 0)
                {
                    WLAN_DBG("invalid SSID length");
                    return WF_RETURN_FAIL;
                }
                else
                {
                    pscanned_info->ssid.length = pie->len;
                    wf_memset(pscanned_info->ssid.data, '\0',
                              sizeof(pscanned_info->ssid.data));
                    wf_memcpy(pscanned_info->ssid.data, pie->data, pie->len);
                    WLAN_DBG("SSID: %s", pscanned_info->ssid.data);
                }
                break;

            case WF_80211_MGMT_EID_SUPP_RATES :
            case WF_80211_MGMT_EID_EXT_SUPP_RATES :
                /* basic rate */
                if (support_rate_cnt == 0)
                {
                    wf_memset(pscanned_info->spot_rate, 0,
                              sizeof(pscanned_info->spot_rate));
                }
                if (pie->len == 0 || pie->len + support_rate_cnt >
                    sizeof(pscanned_info->spot_rate))
                {
                    WLAN_DBG("support rate number over limit");
                    return WF_RETURN_FAIL;
                }
                else
                {
                    wf_memcpy(&pscanned_info->spot_rate[support_rate_cnt],
                              pie->data, pie->len);
                    support_rate_cnt += pie->len;
                }
                break;

            case WF_80211_MGMT_EID_DS_PARAMS :
                pdsss_para = (wf_80211_mgmt_dsss_parameter_t *)pie->data;
                pscanned_info->channel = pdsss_para->current_channel;
                break;

            case WF_80211_MGMT_EID_HT_OPERATION :
                /* for 5G AP */
                pht_opt = (wf_80211_mgmt_ht_operation_t *)pie->data;
                pscanned_info->channel = pht_opt->primary_chan;
                break;

            case WF_80211_MGMT_EID_HT_CAPABILITY :
                pscanned_info->ht_spot = wf_true;
                pht_cap = (wf_80211_mgmt_ht_cap_t *)pie->data;
                wf_memcpy(&pscanned_info->mcs, pht_cap->mcs_info.rx_mask,
                          sizeof(pscanned_info->mcs));
                pscanned_info->bw_40mhz =
                    !!(pht_cap->cap_info & WF_80211_MGMT_HT_CAP_SUP_WIDTH_20_40);
                pscanned_info->short_gi =
                    !!(pht_cap->cap_info & (WF_80211_MGMT_HT_CAP_SGI_20 |
                                            WF_80211_MGMT_HT_CAP_SGI_40));
                break;

            case WF_80211_MGMT_EID_VENDOR_SPECIFIC :
                if (!wf_memcmp(pie->data, wpa_oui, sizeof(wpa_oui)))
                {
                    if (pie->len <= sizeof(pscanned_info->wpa_ie))
                    {
                        wf_memcpy(&pscanned_info->wpa_ie, pie,
                                  WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len);
                    }
                    else
                    {
                        WLAN_DBG("wpa length=%d over limite", pie->len);
                    }
                }
                else if (!wf_memcmp(pie->data, wps_oui, sizeof(wps_oui)))
                {
                    if (pie->len <= sizeof(pscanned_info->wps_ie))
                    {
                        wf_memcpy(&pscanned_info->wps_ie, pie,
                                  WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len);
                    }
                    else
                    {
                        WLAN_DBG("wps length=%d over limite", pie->len);
                    }
                }
                break;

            case WF_80211_MGMT_EID_RSN :
                if (pie->len <= sizeof(pscanned_info->rsn_ie))
                {
                    wf_memcpy(&pscanned_info->rsn_ie, pie,
                              WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len);
                }
                else
                {
                    WLAN_DBG("rsn length=%d over limite", pie->len);
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
        switch (pscanned_info->spot_rate[i] & 0x7F)
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
        pscanned_info->name = pscanned_info->ht_spot ? WF_WLAN_BSS_NAME_IEEE80211_BGN :
                              WF_WLAN_BSS_NAME_IEEE80211_BG;
    }
    else if (cck_spot)
    {
        pscanned_info->name = pscanned_info->ht_spot ? WF_WLAN_BSS_NAME_IEEE80211_BN :
                              WF_WLAN_BSS_NAME_IEEE80211_B;
    }
    else
    {
        pscanned_info->name = pscanned_info->ht_spot ? WF_WLAN_BSS_NAME_IEEE80211_GN :
                              WF_WLAN_BSS_NAME_IEEE80211_G;
    }

    return WF_RETURN_OK;
}

static int scanned_flush (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_scanned_t *pscanned = &pwlan_info->scanned;
    wf_list_t *pcur, *pnext;
    int ret = 0;

    while (wf_wlan_scanned_mdfy_pend(pscanned)) wf_msleep(1);

    wf_list_for_each_safe(pcur, pnext, &pscanned->que.head)
    {
        wf_wlan_queue_node_t *pnode =
            (void *)wf_list_entry(pcur, wf_wlan_queue_node_t, list);
        dequeue(&pscanned->que, pnode);
        enqueue_head(&pscanned->free, pnode);
    }

    wf_wlan_scanned_mdfy_post(pscanned);

    return ret;
}

static int rx_frame_handle (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_rx_frame_t *prx_frame = &pwlan_info->rx_frame;
    wf_wlan_scanned_t *pscanned = &pwlan_info->scanned;
    wf_wlan_scanned_info_t *pscanned_info;
    wf_wlan_queue_node_t *pnode;
    wf_wlan_msg_t *pmsg;
    msg_frame_t *pmsg_frame;
    wf_wlan_queue_node_t *pscanned_node = NULL;
    wf_bool uninstalling = wf_false;

    wf_os_api_thread_affinity(DEFAULT_CPU_ID);

    while (1)
    {
        if (uninstalling)
        {
            break;
        }

        /* block here until received an message */
        if (msg_que_wait(pwlan_info, &pnode))
        {
            continue;
        }

        pmsg = (wf_wlan_msg_t *)pnode->data;
        switch (pmsg->tag)
        {
            case WF_WLAN_MSG_TAG_BEACON :
            case WF_WLAN_MSG_TAG_PROBERSP :
                /* todo: fillup data to a free scanned node which from
                scanned queue, */
                pmsg_frame = (msg_frame_t *)pmsg->value;
                if (scanned_node_pull(pnic_info, pmsg_frame, &pscanned_node) >= 0)
                {
                    pscanned_info = (wf_wlan_scanned_info_t *)pscanned_node->data;
                    /* get bss information */
                    if (make_scanned_info(pmsg_frame, pmsg->len, pscanned_info)
                        == WF_RETURN_OK)
                    {
                        scanned_node_push(pnic_info, pscanned_node);
                    }
                    else
                    {
                        /* give up scanned queue update, put pscanned_node back
                        to free queue */
                        enqueue_head(&pscanned->free, pscanned_node);
                    }
                }

                /* push node back to queue  */
                enqueue_tail(pmsg->tag == WF_WLAN_MSG_TAG_BEACON ?
                             &prx_frame->beacon : &prx_frame->probersp, pnode);
                break;

            case WF_WLAN_MSG_TAG_PROBEREQ :
                break;

            case WF_WLAN_MSG_TAG_NTFY_SCANNED_FLUSH :
                WLAN_DBG("WF_WLAN_MSG_TAG_NTFY_SCANNED_FLUSH");
                scanned_flush(pnic_info);
                enqueue_tail(&pwlan_info->notify, pnode);
                break;

            case WF_WLAN_MSG_TAG_NTFY_UNINSTALL :
                WLAN_DBG("WF_WLAN_MSG_TAG_NTFY_UNINSTALL");
                enqueue_tail(&pwlan_info->notify, pnode);
                while (wf_wlan_scanned_mdfy_pend(pscanned)) wf_msleep(1);
                uninstalling = wf_true;
                break;

            default :
                WLAN_WARN("message tag unknown !!!");
                break;
        }
    }

    if(wf_os_api_thread_wait_stop(pwlan_info->tid) == wf_true)
    {
        wf_os_api_thread_exit(pwlan_info->tid);
    }

    return 0;
}

int wf_wlan_mgmt_rx_frame (void *pin)
{
    prx_pkt_t prx_pkt = (prx_pkt_t)pin;
    nic_info_st *pnic_info = NULL;
    wf_wlan_info_t *pwlan_info;
    wf_wlan_rx_frame_t *prx_frame;
    wf_80211_mgmt_t *pmgmt;
    wf_wlan_network_t *pcur_network;
    wf_u32 mgmt_len;
    wf_wlan_queue_node_t *pnode;
    wf_wlan_msg_t *pmsg;
    msg_frame_t *pmsg_frame;
    phy_status_st *pkt_phy_sta;
    wdn_net_info_st *pwdn_info = NULL;
    wf_80211_frame_e sub_type;
    int rst;

    if (prx_pkt == NULL || prx_pkt->p_nic_info == NULL || prx_pkt->pdata == NULL)
    {
        return WF_RETURN_FAIL;
    }

    if (prx_pkt->p_nic_info == NULL)
    {
        return WF_RETURN_FAIL;
    }

    /* if uninstall process is going, after process must be stop */
    if (wf_wlan_is_uninstalling(prx_pkt->p_nic_info))
    {
        return WF_RETURN_FAIL;
    }
    pnic_info       = prx_pkt->p_nic_info;
    pwlan_info      = pnic_info->wlan_info;
    pcur_network    = &pwlan_info->cur_network;
    prx_frame       = &pwlan_info->rx_frame;
    pmgmt           = (wf_80211_mgmt_t *)prx_pkt->pdata;
    mgmt_len        = prx_pkt->len;
    pkt_phy_sta     = &prx_pkt->phy_status;
    pwdn_info       = prx_pkt->wdn_info;

    if (pwdn_info)
    {
        pwdn_info->wdn_stats.rx_mgnt_pkts++;
    }

    switch (sub_type = wf_80211_get_frame_type(pmgmt->frame_control))
    {
        case WF_80211_FRM_BEACON :
            if (mgmt_len == 0 || mgmt_len > BEACON_FRAME_SIZE_MAX)
            {
                WLAN_WARN("beacon response frame length=%d out of range "
                          "BSSID=%02X:%02X:%02X:%02X:%02X:%02X", mgmt_len,
                          pmgmt->bssid[0], pmgmt->bssid[1],
                          pmgmt->bssid[2], pmgmt->bssid[3],
                          pmgmt->bssid[4], pmgmt->bssid[5] );
                break;
            }

            rst = wf_scan_filter(pnic_info, pmgmt, mgmt_len);
            if (rst)
            {
                WLAN_WARN("scan filter fail, error code: %d", rst);
                break;
            }

            /* get free node */
            pnode = queue_head(&prx_frame->beacon);
            if (dequeue(&prx_frame->beacon, pnode))
            {
                WLAN_WARN("beacon queue empty");
                break;
            }

            /* make message */
            pmsg        = (wf_wlan_msg_t *)pnode->data;
            pmsg_frame  = (msg_frame_t *)pmsg->value;
            /* get phy status */
            pmsg_frame->phy_sta.signal_strength = pkt_phy_sta->signal_strength;
            pmsg_frame->phy_sta.signal_qual     = pkt_phy_sta->signal_qual;
            pmsg_frame->phy_sta.rssi            = pkt_phy_sta->rssi;
            pmsg_frame->phy_sta.valid_flag      = prx_pkt->pkt_info.phy_status ? wf_true : wf_false;
            /* copy frame data */
            wf_memcpy(&pmsg_frame->mgmt, pmgmt, mgmt_len);
            /* set message length */
            pmsg->len = sizeof(pmsg_frame->phy_sta) + mgmt_len;
            /* update becon timestamp*/
            if(mac_addr_equal(pmgmt->bssid, wf_wlan_get_cur_bssid(pnic_info)))
            {
//              LOG_I("update beacon timestamp");
                pcur_network->timestamp = pmgmt->beacon.timestamp;
                pcur_network->bcn_interval = pmgmt->beacon.intv;
//              LOG_I("beacon.timestamp:%lld",pcur_network->timestamp);
            }
            /* send message */
            msg_que_send(pwlan_info, pnode);
#ifdef CFG_ENABLE_ADHOC_MODE
            if(get_sys_work_mode(pnic_info) == WF_ADHOC_MODE)
            {
                wf_adhoc_work(pnic_info, (void *)pmgmt, mgmt_len);
            }
#endif
            break;

        case WF_80211_FRM_PROBE_RESP :
            if (mgmt_len == 0 || mgmt_len > PROBE_RESP_FRAME_SIZE_MAX)
            {
                WLAN_WARN("probe response frame length=%d out of range "
                          "BSSID=%02X:%02X:%02X:%02X:%02X:%02X", mgmt_len,
                          pmgmt->bssid[0], pmgmt->bssid[1],
                          pmgmt->bssid[2], pmgmt->bssid[3],
                          pmgmt->bssid[4], pmgmt->bssid[5] );
                break;
            }

            rst = wf_scan_filter(pnic_info, pmgmt, mgmt_len);
            if (rst)
            {
                WLAN_WARN("scan filter fail, error code: %d", rst);
                break;
            }

            /* get free node */
            pnode = queue_head(&prx_frame->probersp);
            if (dequeue(&prx_frame->probersp, pnode))
            {
                WLAN_WARN("probe respone queue empty");
                break;
            }

            /* make message */
            pmsg        = (wf_wlan_msg_t *)pnode->data;
            pmsg_frame  = (msg_frame_t *)pmsg->value;
            /* get phy status */
            pmsg_frame->phy_sta.signal_strength = pkt_phy_sta->signal_strength;
            pmsg_frame->phy_sta.signal_qual     = pkt_phy_sta->signal_qual;
            pmsg_frame->phy_sta.rssi            = pkt_phy_sta->rssi;
            pmsg_frame->phy_sta.valid_flag      = prx_pkt->pkt_info.phy_status ? wf_true : wf_false;
            /* copy frame data */
            wf_memcpy(&pmsg_frame->mgmt, pmgmt, mgmt_len);
            /* set message length */
            pmsg->len = sizeof(pmsg_frame->phy_sta) + mgmt_len;
            /* send message */
            msg_que_send(pwlan_info, pnode);
            break;

        case WF_80211_FRM_AUTH :
            wf_auth_frame_parse(pnic_info, pwdn_info, pmgmt, mgmt_len);
            break;

        case WF_80211_FRM_DEAUTH :
            wf_deauth_frame_parse(pnic_info, pwdn_info, pmgmt, mgmt_len);
            break;

        case WF_80211_FRM_DISASSOC :
            wf_disassoc_frame_parse(pnic_info, pwdn_info, pmgmt, mgmt_len);
            break;

        case WF_80211_FRM_REASSOC_RESP :
        case WF_80211_FRM_ASSOC_RESP :
            wf_assoc_frame_parse(pnic_info, pwdn_info, pmgmt, mgmt_len);
            break;

        case WF_80211_FRM_ACTION :
            wf_action_frame_process(pnic_info, pwdn_info, pmgmt, mgmt_len);
            break;

        case WF_80211_FRM_PROBE_REQ :
#if defined(CFG_ENABLE_ADHOC_MODE)
            if(get_sys_work_mode(pnic_info) == WF_ADHOC_MODE)
            {
                wf_adhoc_do_probrsp(pnic_info, pmgmt, mgmt_len);
                break;
            }
#endif
#ifdef CFG_ENABLE_AP_MODE
            if(get_sys_work_mode(pnic_info) == WF_MASTER_MODE)
            {
                wf_ap_probe(pnic_info, pmgmt, mgmt_len);
            }
#endif
            break;
#ifdef CFG_ENABLE_AP_MODE
        case WF_80211_FRM_REASSOC_REQ :
        case WF_80211_FRM_ASSOC_REQ :
            wf_assoc_ap_work(pnic_info, pwdn_info, (void *)pmgmt, mgmt_len);
            break;
#endif
        default :
            //LOG_I("sub_type:%x",sub_type);
            break;
    }

    return WF_RETURN_OK;
}


static wf_inline int scanned_init (wf_wlan_info_t *pwlan_info)
{
    wf_wlan_scanned_t *pscanned = &pwlan_info->scanned;
    wf_wlan_queue_node_t *pnode;
    int i;

    if (pscanned == NULL)
    {
        WLAN_WARN("null point");
        return -1;
    }

    WLAN_DBG("scan queue alloc");
    queue_init(&pscanned->que);
    queue_init(&pscanned->free);
    /* alloc scan free object */
    for (i = 0; i < SCANNED_INFO_QUE_DEEP; i++)
    {
        pnode = wf_kzalloc(SCANNED_INFO_QUE_NODE_SIZE);
        if (pnode == NULL)
        {
            WLAN_WARN("kzalloc failed ,check!!!!");
            return -2;
        }
        enqueue_head(&pscanned->free, pnode);
    }

    wf_lock_init(&pscanned->lock, WF_LOCK_TYPE_IRQ);
    pscanned->count = 0;
    wf_os_api_sema_init(&pscanned->sema, 1);

    return 0;
}


static wf_inline int rx_frame_init (wf_wlan_info_t *pwlan_info)
{
    wf_wlan_rx_frame_t *prx_frame = &pwlan_info->rx_frame;
    wf_wlan_queue_node_t *pnode;
    wf_wlan_msg_t *pmsg;
    int i;

    if (prx_frame == NULL)
    {
        WLAN_WARN("null point");
        return -1;
    }

    WLAN_DBG("beacon frame alloc");
    queue_init(&prx_frame->beacon);
    /* alloc beacon free object */
    for (i = 0; i < RX_FRAME_BEACON_QUE_DEEP; i++)
    {
        pnode = wf_kzalloc(RX_FRAME_BEACON_QUE_NODE_SIZE);
        if (pnode == NULL)
        {
            WLAN_WARN("kzalloc failed ,check!!!!");
            return -2;
        }
        pmsg        = (wf_wlan_msg_t *)pnode->data;
        pmsg->tag   = WF_WLAN_MSG_TAG_BEACON;
        pmsg->len   = 0;
        enqueue_head(&prx_frame->beacon, pnode);
    }

    WLAN_DBG("probe response frame alloc");
    queue_init(&prx_frame->probersp);
    /* alloc probe respond free object */
    for (i = 0; i < RX_FRAME_PROBERSP_QUE_DEEP; i++)
    {
        pnode = wf_kzalloc(RX_FRAME_PROBERSP_QUE_NODE_SIZE);
        if (pnode == NULL)
        {
            WLAN_WARN("kzalloc failed ,check!!!!");
            return -2;
        }
        pmsg        = (wf_wlan_msg_t *)pnode->data;
        pmsg->tag   = WF_WLAN_MSG_TAG_PROBERSP;
        pmsg->len   = 0;
        enqueue_head(&prx_frame->probersp, pnode);
    }

    return 0;
}

static wf_inline int notify_init (wf_wlan_info_t *pwlan_info)
{
    wf_wlan_queue_t *pnotify = &pwlan_info->notify;
    wf_wlan_queue_node_t *pnode;
    int i;

    if (pnotify == NULL)
    {
        WLAN_WARN("null point");
        return -1;
    }

    queue_init(pnotify);
    /* alloc beacon free object */
    WLAN_DBG("command alloc");
    for (i = 0; i < NOTIFY_QUE_DEEP; i++)
    {
        pnode = wf_kzalloc(NOTIFY_QUE_NODE_SIZE);
        if (pnode == NULL)
        {
            WLAN_WARN("kzalloc failed ,check!!!!");
            return -2;
        }
        enqueue_head(pnotify, pnode);
    }

    return 0;
}

static wf_inline int msg_que_init (wf_wlan_info_t *pwlan_info)
{
    wf_wlan_msg_que_t *pmsg_que = &pwlan_info->msg_que;

    if (pmsg_que == NULL)
    {
        WLAN_WARN("null point");
        return -1;
    }

    queue_init(&pmsg_que->que);
    wf_os_api_sema_init(&pmsg_que->sema, 0);

    return 0;
}

int wf_wlan_init (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info;

#ifdef CONFIG_CONCURRENT_MODE
    nic_info_st *pvir_nic = pnic_info->vir_nic;
    if(pnic_info->nic_num == 1)
    {
        pnic_info->wlan_info = pvir_nic->wlan_info;
    }
#endif

    /* wlan info alloc */
    pwlan_info = wf_kzalloc(sizeof(wf_wlan_info_t));
    if (pwlan_info == NULL)
    {
        WLAN_WARN("malloc failed");
        return WF_RETURN_FAIL;
    }

    /* scanned queue initilize */
    if (scanned_init(pwlan_info))
    {
        WLAN_WARN("scanned queue initilize");
        return WF_RETURN_FAIL;
    }

    /* rx frame queue initilize */
    if (rx_frame_init(pwlan_info))
    {
        WLAN_WARN("rx frame queue initilize");
        return WF_RETURN_FAIL;
    }

    /* notify queue initilize */
    if (notify_init(pwlan_info))
    {
        WLAN_WARN("nofity initilize");
        return WF_RETURN_FAIL;
    }

    /* message queue initilize */
    if (msg_que_init(pwlan_info))
    {
        WLAN_WARN("mssage queue initilize");
        return WF_RETURN_FAIL;
    }

    /* wlan struct initilize done */
    pnic_info->wlan_info = pwlan_info;

    /* create thread for rx frame handle */
    sprintf(pwlan_info->name,
            pnic_info->virNic ? "wlan_info:vir%d_s%d" : "wlan_info:wlan%d_s%d",
            pnic_info->hif_node_id, pnic_info->ndev_id);
    if (NULL ==(pwlan_info->tid = wf_os_api_thread_create(&pwlan_info->tid, pwlan_info->name,
                                  rx_frame_handle, pnic_info)))
    {
        WLAN_WARN("create wlan info parse thread failed");
        return WF_RETURN_FAIL;
    }
    if (!pwlan_info->tid)
    {
        WLAN_WARN("tid error");
    }
    wf_os_api_thread_wakeup(pwlan_info->tid);

    return WF_RETURN_OK;
}


static wf_inline int scanned_deinit (wf_wlan_info_t *pwlan_info)
{
    wf_wlan_scanned_t *pscanned = &pwlan_info->scanned;
    wf_wlan_queue_node_t *pnode;

    WLAN_DBG();

    if (pscanned == NULL)
    {
        WLAN_WARN("null point");
        return -1;
    }

    /* scanned free queue */
    while (!queue_empty(&pscanned->free))
    {
        pnode = queue_head(&pscanned->free);
        dequeue(&pscanned->free, pnode);
        wf_kfree(pnode);
    }

    /* scanned queue */
    while (!queue_empty(&pscanned->que))
    {
        pnode = queue_head(&pscanned->que);
        dequeue(&pscanned->que, pnode);
        wf_kfree(pnode);
    }

    queue_deinit(&pscanned->que);
    wf_os_api_sema_free(&pscanned->sema);

    return 0;
}

static wf_inline int rx_frame_deinit (wf_wlan_info_t *pwlan_info)
{
    wf_wlan_rx_frame_t *prx_frame = &pwlan_info->rx_frame;
    wf_wlan_queue_node_t *pnode;

    WLAN_DBG();

    if (prx_frame == NULL)
    {
        WLAN_WARN("null point");
        return -1;
    }

    /* free beacon queue */
    WLAN_DBG("free beacon queue");
    while (!queue_empty(&prx_frame->beacon))
    {
        pnode = queue_head(&prx_frame->beacon);
        dequeue(&prx_frame->beacon, pnode);
        wf_kfree(pnode);
    }

    /* free probe rsponse queue */
    WLAN_DBG("free probe rsponse queue");
    while (!queue_empty(&prx_frame->probersp))
    {
        pnode = queue_head(&prx_frame->probersp);
        dequeue(&prx_frame->probersp, pnode);
        wf_kfree(pnode);
    }

    queue_deinit(&prx_frame->beacon);

    return 0;
}

int wf_notify_send (nic_info_st *pnic_info, wf_wlan_msg_tag_e tag)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_queue_t *pnotify = &pwlan_info->notify;
    wf_wlan_queue_node_t *pnode;
    wf_wlan_msg_t *pmsg;

    if (queue_empty(pnotify))
    {
        return -2;
    }

    pnode = queue_head(pnotify);
    dequeue(pnotify, pnode);
    pmsg        = (wf_wlan_msg_t *)pnode->data;
    pmsg->tag   = tag;
    msg_que_send(pwlan_info, pnode);

    return 0;
}

static wf_inline int nofity_deinit (wf_wlan_info_t *pwlan_info)
{
    wf_wlan_queue_t *pnotify = &pwlan_info->notify;
    wf_wlan_queue_node_t *pnode;

    WLAN_DBG();

    if (pnotify == NULL)
    {
        WLAN_WARN("null point");
        return -1;
    }

    while (!queue_empty(pnotify))
    {
        pnode = queue_head(pnotify);
        dequeue(pnotify, pnode);
        wf_kfree(pnode);
    }

    queue_deinit(pnotify);

    return 0;
}

static wf_inline int msg_que_deinit (wf_wlan_info_t *pwlan_info)
{
    wf_wlan_msg_que_t *pmsg_que = &pwlan_info->msg_que;
    wf_wlan_queue_node_t *pnode;

    WLAN_DBG();

    if (pmsg_que == NULL)
    {
        WLAN_WARN("null point");
        return -1;
    }

    while (!queue_empty(&pmsg_que->que))
    {
        pnode = queue_head(&pmsg_que->que);
        dequeue(&pmsg_que->que, pnode);
        wf_kfree(pnode);
    }

    queue_deinit(&pmsg_que->que);
    wf_os_api_sema_free(&pmsg_que->sema);

    return 0;
}

int wf_wlan_term (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    if (pwlan_info == NULL)
    {
        WLAN_WARN("Null point");
        return WF_RETURN_FAIL;
    }

    WLAN_DBG();

    /* destory command send */
    wf_notify_send(pnic_info, WF_WLAN_MSG_TAG_NTFY_UNINSTALL);
    /* destory thread */
    if (pwlan_info->tid)
    {
        wf_os_api_thread_destory(pwlan_info->tid);
        pwlan_info->tid = 0;
    }

    /* free wlan info */
    scanned_deinit(pwlan_info);
    rx_frame_deinit(pwlan_info);
    nofity_deinit(pwlan_info);
    msg_que_deinit(pwlan_info);
    pnic_info->wlan_info = NULL;
    wf_kfree(pwlan_info);

    return WF_RETURN_OK;
}

void wf_wlan_set_cur_ssid (nic_info_st *nic_info, wf_wlan_ssid_t *ssid)
{
    wf_wlan_info_t *wlan_info = (wf_wlan_info_t *)nic_info->wlan_info;

    wf_memcpy(&wlan_info->cur_network.ssid, ssid, sizeof(wf_wlan_ssid_t));
}

wf_wlan_ssid_t *wf_wlan_get_cur_ssid (nic_info_st *nic_info)
{
    wf_wlan_info_t *wlan_info = (wf_wlan_info_t *)nic_info->wlan_info;

    return &wlan_info->cur_network.ssid;
}

void wf_wlan_set_cur_bssid (nic_info_st *nic_info, wf_u8 *bssid)
{
    wf_wlan_info_t *wlan_info = (wf_wlan_info_t *)nic_info->wlan_info;

    wf_memcpy(wlan_info->cur_network.bssid, bssid, MAC_ADDR_LEN);
}


inline wf_u8 *wf_wlan_get_cur_bssid (nic_info_st *nic_info)
{
    wf_wlan_info_t *wlan_info = (wf_wlan_info_t *)nic_info->wlan_info;

    return (wf_u8 *)wlan_info->cur_network.bssid;
}


void wf_wlan_set_cur_channel (nic_info_st *nic_info, wf_u8 channel)
{
    wf_wlan_info_t *wlan_info = (wf_wlan_info_t *)nic_info->wlan_info;

    wlan_info->cur_network.channel = channel;
}


wf_u8 wf_wlan_get_cur_channel (nic_info_st *nic_info)
{
    wf_wlan_info_t *wlan_info = (wf_wlan_info_t *)nic_info->wlan_info;

    return wlan_info->cur_network.channel;
}

void wf_wlan_set_cur_bw (nic_info_st *nic_info, CHANNEL_WIDTH bw)
{
    wf_wlan_info_t *wlan_info = (wf_wlan_info_t *)nic_info->wlan_info;

    wlan_info->cur_network.bw = bw;
}

CHANNEL_WIDTH wf_wlan_get_cur_bw (nic_info_st *nic_info)
{
    wf_wlan_info_t *wlan_info = (wf_wlan_info_t *)nic_info->wlan_info;

    return wlan_info->cur_network.bw;
}

static wf_u16 mcs_rate_func(wf_u8 bw_40MHz, wf_u8 short_GI, wf_u8 *MCS_rate)
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

int wf_wlan_get_max_rate(nic_info_st *pnic_info,wf_u8 *mac,wf_u16 *max_rate)
{
    wf_u16 rate = 0;
    wf_u8 short_GI = 0;
    int i = 0;
    wdn_net_info_st *pwdn_info = NULL;

    pwdn_info = wf_wdn_find_info(pnic_info, mac);
    if(NULL == pwdn_info)
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


int wf_wlan_get_signal_and_qual (nic_info_st *pnic_info,wf_u8 *qual, wf_u8 *level)
{
    wf_wlan_scanned_info_t *pscanned_info = NULL;
    wf_wlan_info_t *wlan_info             = (wf_wlan_info_t *)pnic_info->wlan_info;
    int scanned_ret = 0;

    wf_wlan_scanned_each_begin(pscanned_info, pnic_info)
    {
        if(!wf_memcmp(pscanned_info->bssid,wlan_info->cur_network.bssid, MAC_ADDR_LEN))
        {
            *qual = pscanned_info->signal_qual;
            *level = pscanned_info->signal_strength;
            break;
        }
    }
    wf_wlan_scanned_each_end(pnic_info, &scanned_ret);

    return scanned_ret;
}

