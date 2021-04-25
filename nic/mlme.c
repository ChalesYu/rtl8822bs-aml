
#include "common.h"
#include "wf_debug.h"

/* macro */
#if 0
#define MLME_DBG(fmt, ...)      LOG_D("[%s]"fmt, __func__, ##__VA_ARGS__)
#define MLME_ARRAY(data, len)   log_array(data, len)
#else
#define MLME_DBG(fmt, ...)
#define MLME_ARRAY(data, len)
#endif
#define MLME_INFO(fmt, ...)     LOG_I("[%s]"fmt, __func__, ##__VA_ARGS__)
#define MLME_WARN(fmt, ...)     LOG_W("[%s]"fmt, __func__, ##__VA_ARGS__)
#define MLME_ERROR(fmt, ...)    LOG_E("[%s]"fmt, __func__, ##__VA_ARGS__)

/* type define */
typedef struct
{
    wf_mlme_framework_e framework;
    scan_type_e type;
    wf_wlan_ssid_t ssids[WF_SCAN_REQ_SSID_NUM];
    wf_u8 ssid_num;
    wf_u8 chs[WF_SCAN_REQ_CHANNEL_NUM];
    wf_u8 ch_num;
} mlme_scan_t;
typedef wf_u8 mlme_scan_rsp_t[WF_80211_MGMT_PROBERSP_SIZE_MAX];
typedef struct
{
    wf_mlme_framework_e framework;
    wf_80211_bssid_t bssid;
    wf_wlan_ssid_t ssid;
} mlme_conn_t;

/* function declaration */
static int mlme_msg_send (nic_info_st *pnic_info,
                          wf_msg_tag_t tag, void *value, wf_u8 len);

wf_inline static
int hw_cfg (nic_info_st *pnic_info, wdn_net_info_st *wdn_info)
{
    int ret = 0;
    wf_u16 basic_dr_cfg = 0;

    /* hardware configure */
    if (wdn_info->short_preamble)
    {
        ret |= wf_mcu_set_preamble(pnic_info, PREAMBLE_SHORT);
    }
    else
    {
        ret |= wf_mcu_set_preamble(pnic_info, PREAMBLE_LONG);
    }

    if (wdn_info->short_slot)
    {
        ret |= wf_mcu_set_slot_time(pnic_info, SHORT_SLOT_TIME);
    }
    else
    {
        ret |= wf_mcu_set_slot_time(pnic_info, NON_SHORT_SLOT_TIME);
    }

    if (wdn_info->wmm_enable)
    {
        ret |= wf_mcu_set_wmm_para_enable(pnic_info, wdn_info);
    }
    else
    {
        ret |= wf_mcu_set_wmm_para_disable(pnic_info, wdn_info);
    }

    ret |= wf_mcu_set_correct_tsf(pnic_info, wdn_info->tsf);

    ret |= wf_mcu_set_sifs(pnic_info, 0x0a0a0808);

    ret |= wf_mcu_set_macid_wakeup(pnic_info, wdn_info->wdn_id);

    ret |= wf_mcu_set_max_ampdu_len(pnic_info, wdn_info->htpriv.rx_ampdu_maxlen);
    ret |= wf_mcu_set_min_ampdu_space(pnic_info, wdn_info->htpriv.rx_ampdu_min_spacing);
    ret |= wf_mcu_set_config_xmit(pnic_info, WF_XMIT_AMPDU_DENSITY, wdn_info->htpriv.rx_ampdu_min_spacing);

    get_bratecfg_by_support_dates(wdn_info->datarate, wdn_info->datarate_len, &basic_dr_cfg);
    get_bratecfg_by_support_dates(wdn_info->ext_datarate, wdn_info->ext_datarate_len, &basic_dr_cfg);
    ret |= wf_mcu_set_basic_rate(pnic_info,  basic_dr_cfg);
    ret |= wf_odm_set_ability(pnic_info, ODM_FUNC_CLR, ODM_BB_DYNAMIC_TXPWR);

    return ret;
}

wf_inline static int build_wdn (nic_info_st *pnic_info)
{
    wdn_net_info_st *pwdn_info;

    MLME_DBG();

    pwdn_info = wf_wdn_add(pnic_info, wf_wlan_get_cur_bssid(pnic_info));
    if (pwdn_info == NULL)
    {
        MLME_WARN("new wdn fail");
        return -1;
    }

    if (wf_wdn_data_update(pnic_info, pwdn_info))
    {
        MLME_WARN("wdn update fail");
        return -2;
    }

    if (hw_cfg(pnic_info, pwdn_info))
    {
        MLME_WARN("hw config fail");
        return -3;
    }

    return 0;
}

static
wf_pt_rst_t core_scan_thrd (wf_pt_t *pt, nic_info_st *pnic_info,
                            mlme_scan_t *preq, int *prsn)
{
    wf_pt_t *pt_sub = &pt[1];
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_msg_que_t *pmsg_que = &pmlme_info->msg_que;
    wf_msg_t *pmsg;
    int rst;

    PT_BEGIN(pt);

    MLME_DBG();

    if (preq == NULL)
    {
        MLME_WARN("invalid scan request");
        *prsn = -1;
        PT_EXIT(pt);
    }

    /* start scan */
    rst = wf_scan_start(pnic_info, preq->type,
                        NULL,
                        preq->ssids, preq->ssid_num,
                        preq->chs, preq->ch_num);
    if (rst)
    {
        *prsn = -2;
        MLME_WARN("start fail, error code: %d", rst);
        PT_EXIT(pt);
    }

    /* scan process */
    MLME_INFO("scan...");
    PT_INIT(pt_sub);
    while (PT_SCHEDULE(wf_scan_thrd(pt_sub, pnic_info, prsn)))
    {
        if (!wf_msg_get(pmsg_que, &pmsg) && pmsg->tag == WF_MLME_TAG_SCAN_ABORT)
        {
            wf_msg_del(pmsg_que, pmsg);
            MLME_DBG("abort scanning...");
            wf_scan_stop(pnic_info);
            PT_WAIT_THREAD(pt, wf_scan_thrd(pt_sub, pnic_info, prsn));
            break;
        }
        PT_YIELD(pt);
    }

#ifdef CFG_ENABLE_ADHOC_MODE
    if((get_sys_work_mode(pnic_info) == WF_ADHOC_MODE) && (*prsn == WF_SCAN_TAG_DONE))
    {
        LOG_I("*** [MLME] Join Ibss ***");
        if(wf_proc_ibss_join(pnic_info))
        {
            *prsn = -2;
            MLME_WARN("Join ibss fall!!!");
            PT_EXIT(pt);
        }
    }
#endif

    /* notify system scan result */
    MLME_DBG("report scan result");
    wf_os_api_ind_scan_done(pnic_info, *prsn == WF_SCAN_TAG_ABORT,
                            preq->framework);
    *prsn = 0;

    PT_END(pt);
}

wf_inline static
int set_cur_network (nic_info_st *pnic_info,
                     wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_u16 var_len;
    wf_u8 bss_ch;

    /* get bss channel number */
    {
        wf_u8 *pies = &pmgmt->probe_resp.variable[0];
        wf_u16 ies_len = mgmt_len - WF_OFFSETOF(struct beacon_ie, variable);
        wf_80211_mgmt_ie_t *pie;
        if (wf_80211_mgmt_ies_search(pies, ies_len,
                                     WF_80211_MGMT_EID_DS_PARAMS, &pie))
        {
            MLME_WARN("no DS element field");
            return -1;
        }
        {
            wf_80211_mgmt_dsss_parameter_t *pds = (void *)pie->data;
            bss_ch = pds->current_channel;
        }
    }

    /* set channel */
    MLME_INFO("channel: %d", bss_ch);
    wf_wlan_set_cur_channel(pnic_info, bss_ch);
    /* retrive address */
    wf_memcpy(pcur_network->mac_addr, pmgmt->sa, sizeof(wf_80211_addr_t));
    wf_wlan_set_cur_bssid(pnic_info, pmgmt->bssid);
    /* retrive ssid */
    {
        wf_u8 *pies = &pmgmt->probe_resp.variable[0];
        wf_u16 ies_len = mgmt_len - WF_OFFSETOF(struct beacon_ie, variable);
        wf_80211_mgmt_ie_t *pie;
        int rst = wf_80211_mgmt_ies_search(pies, ies_len,
                                           WF_80211_MGMT_EID_SSID, &pie);
        if (rst)
        {
            MLME_ERROR("ies search fail, error code: %d", rst);
            return -2;
        }
        {
            wf_wlan_ssid_t ssid;
            ssid.length = pie->len;
            wf_memcpy(ssid.data, pie->data, pie->len);
            wf_wlan_set_cur_ssid(pnic_info, &ssid);
        }
    }
    /* retrive no elements field */
    pcur_network->timestamp = wf_le64_to_cpu(pmgmt->probe_resp.timestamp);
    pcur_network->bcn_interval = pmgmt->probe_resp.intv;
    pcur_network->cap_info = pmgmt->probe_resp.capab;
    /* copy ies */
    var_len = mgmt_len - WF_OFFSETOF(wf_80211_mgmt_t, probe_resp.variable);
    if (var_len > sizeof(pcur_network->ies))
    {
        MLME_WARN("mangnet frame body size beyond limit");
        return -2;
    }
    pcur_network->ies_length = var_len;
    wf_memcpy(&pcur_network->ies[0], &pmgmt->probe_resp.variable[0], var_len);

    return 0;
}

static
wf_pt_rst_t core_conn_scan_thrd (wf_pt_t *pt, nic_info_st *pnic_info,
                                 mlme_conn_t *preq, int *prsn)
{
    wf_pt_t *pt_sub = &pt[1];
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_msg_que_t *pmsg_que = &pmlme_info->msg_que;
    wf_msg_t *pmsg;
    int rst;

    PT_BEGIN(pt);

    if (preq == NULL)
    {
        MLME_WARN("invalid scan request");
        *prsn = -1;
        PT_EXIT(pt);
    }

    /* start scan */
    rst = wf_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                        preq->bssid,
                        &preq->ssid, preq->ssid.length ? 1 : 0,
                        NULL, 0);
    if (rst)
    {
        *prsn = -2;
        MLME_WARN("start fail, error code: %d", rst);
        PT_EXIT(pt);
    }
    MLME_INFO("wait probe respone...");

    /* scan process */
    PT_INIT(pt_sub);
    do
    {
        if (!PT_SCHEDULE(wf_scan_thrd(pt_sub, pnic_info, &rst)))
        {
            MLME_WARN("scan end, reason code: %d", rst);
//            *prsn = -3;
            PT_RESTART(pt);
            break;
        }

        if (!wf_msg_pop(pmsg_que, &pmsg))
        {
            if (pmsg->tag == WF_MLME_TAG_CONN_ABORT ||
                pmsg->tag == WF_MLME_TAG_SCAN_ABORT)
            {
                wf_msg_del(pmsg_que, pmsg);
                MLME_INFO("abort scanning...");
                wf_scan_stop(pnic_info);
                PT_WAIT_THREAD(pt, wf_scan_thrd(pt_sub, pnic_info, &rst));
                *prsn = -4;
                PT_EXIT(pt);
            }
            if (pmsg->tag == WF_MLME_TAG_SCAN_RSP)
            {
                wf_80211_mgmt_t *pmgmt = (void *)pmsg->value;
                wf_u16 mgmt_len = pmsg->len;

                rst = set_cur_network(pnic_info, pmgmt, mgmt_len);
                if (rst)
                {
                    MLME_WARN("set cur_network fail, error code: %d", rst);
                }
                else
                {
                    wf_msg_del(pmsg_que, pmsg);
                    MLME_INFO("probe respone ok");
                    wf_scan_stop(pnic_info);
                    PT_WAIT_THREAD(pt, wf_scan_thrd(pt_sub, pnic_info, &rst));
                    *prsn = 0;
                    break;
                }
            }
            wf_msg_del(pmsg_que, pmsg);
        }

        PT_YIELD(pt);
    }
    while (wf_true);

    PT_END(pt);
}


static
wf_pt_rst_t core_conn_auth_thrd (wf_pt_t *pt, nic_info_st *pnic_info, int *prsn)
{
    wf_pt_t *pt_sub = &pt[1];
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_msg_que_t *pmsg_que = &pmlme_info->msg_que;
    wf_msg_t *pmsg;
    int rst;

    PT_BEGIN(pt);

    rst = wf_auth_sta_start(pnic_info);
    if (rst)
    {
        MLME_WARN("start fail, error code: %d", rst);
        *prsn = -1;
        PT_EXIT(pt);
    }

    PT_INIT(pt_sub);
    while (PT_SCHEDULE(wf_auth_sta_thrd(pt_sub, pnic_info, prsn)))
    {
        if (!wf_msg_pop(pmsg_que, &pmsg))
        {
            if (pmsg->tag == WF_MLME_TAG_CONN_ABORT ||
                pmsg->tag == WF_MLME_TAG_DEAUTH)
            {
                wf_msg_del(pmsg_que, pmsg);
                MLME_INFO("abort auth...");
                wf_auth_sta_stop(pnic_info);
                PT_WAIT_THREAD(pt, wf_auth_sta_thrd(pt_sub, pnic_info, prsn));
                break;
            }
            wf_msg_del(pmsg_que, pmsg);
        }
        PT_YIELD(pt);
    }
    if (*prsn != WF_AUTH_TAG_DONE)
    {
        PT_EXIT(pt);
    }

    PT_END(pt);
}

static
wf_pt_rst_t core_conn_assoc_thrd (wf_pt_t *pt, nic_info_st *pnic_info, int *prsn)
{
    wf_pt_t *pt_sub = &pt[1];
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_msg_que_t *pmsg_que = &pmlme_info->msg_que;
    wf_msg_t *pmsg;
    int rst;

    PT_BEGIN(pt);

    rst = wf_assoc_start(pnic_info);
    if (rst)
    {
        MLME_WARN("start fail, error code: %d", rst);
        *prsn = -1;
        PT_EXIT(pt);
    }

    PT_INIT(pt_sub);
    while (PT_SCHEDULE(wf_assoc_sta_thrd(pt_sub, pnic_info, prsn)))
    {
        if (!wf_msg_pop(pmsg_que, &pmsg))
        {
            if (pmsg->tag == WF_MLME_TAG_CONN_ABORT ||
                pmsg->tag == WF_MLME_TAG_DEAUTH)
            {
                wf_msg_del(pmsg_que, pmsg);
                MLME_INFO("abort assoc...");
                wf_assoc_stop(pnic_info);
                PT_WAIT_THREAD(pt, wf_assoc_sta_thrd(pt_sub, pnic_info, prsn));
                break;
            }
            wf_msg_del(pmsg_que, pmsg);
        }
        PT_YIELD(pt);
    }
    if (*prsn != WF_AUTH_TAG_DONE)
    {
        wf_deauth_xmit_frame(pnic_info, wf_wlan_get_cur_bssid(pnic_info),
                             WF_80211_REASON_DEAUTH_LEAVING);
        PT_EXIT(pt);
    }

    PT_END(pt);
}

int core_conn_preconnect (nic_info_st *pnic_info)
{
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wdn_net_info_st *pwdn_info;
    hw_info_st *phw_info = pnic_info->hw_info;
    wf_u8 channel, bw_mode, offset;

    if (phw_info->ba_enable == wf_true)
    {
        wf_rx_action_ba_ctl_init(pnic_info,
            wf_wdn_find_info(pnic_info, wf_wlan_get_cur_bssid(pnic_info)));
    }

#if defined(CONFIG_CONCURRENT_MODE) && defined(CFG_ENABLE_AP_MODE)
    if(get_sys_work_mode(pnic_info->vir_nic) == WF_MASTER_MODE)
    {
        channel = pwdn_info->channel;
        bw_mode = pwdn_info->bw_mode;
        offset = pwdn_info->channle_offset;
        /* concurrent mode ap reset channel*/
        LOG_I("sta nic_num:%d",pnic_info->nic_num);
        LOG_I("channel:%d bw_mode:%d offset:%d",channel,bw_mode,offset);
        if(pnic_info->nic_num == 0)
        {
            wf_hw_info_set_channnel_bw(pnic_info->vir_nic, channel, bw_mode, offset);
            wf_resend_bcn(pnic_info->vir_nic, channel);
        }
        else if(pnic_info->nic_num == 1)
        {
            wf_hw_info_set_channnel_bw(pnic_info->vir_nic, channel, bw_mode, offset);
            wf_resend_bcn(pnic_info->vir_nic, channel);
        }
    }
#endif
    pwdn_info = wf_wdn_find_info(pnic_info, wf_wlan_get_cur_bssid(pnic_info));
    if (wf_hw_info_set_channnel_bw(pnic_info,
                                   pwdn_info->channel,
                                   pwdn_info->bw_mode,
                                   pwdn_info->channle_offset))
    {
        MLME_WARN("UMSG_OPS_HAL_CHNLBW_MODE failed");
        return -1;
    }
    if (wf_odm_update(pnic_info, pwdn_info))
    {
        MLME_WARN("ODM Update Failed");
        return -2;
    }

    wf_mcu_set_user_info(pnic_info, wf_true);
    wf_action_frame_del_ba_request(pnic_info);
    wf_mcu_set_mlme_join(pnic_info, 2);
    wf_os_api_enable_all_data_queue(pnic_info->ndev);
    {
        mlme_conn_t *pconn_req = (mlme_conn_t *)pmlme_info->pcur_msg->value;
        wf_os_api_ind_connect(pnic_info, pconn_req->framework);
    }

    return 0;
}

static
wf_pt_rst_t core_conn_maintain_scan_thrd (wf_pt_t *pt, nic_info_st *pnic_info,
                                          int *prsn)
{
    wf_pt_t *pt_sub = &pt[1];
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_msg_que_t *pmsg_que = &pmlme_info->msg_que;
    wf_msg_t *pmsg;
    int rst;

    PT_BEGIN(pt);

    rst = wf_scan_start(pnic_info, SCAN_TYPE_PASSIVE,
                        NULL,
                        NULL, 0,
                        NULL, 0);
    if (rst)
    {
        MLME_WARN("start fail error code: %d", *prsn);
        *prsn = -1;
        PT_EXIT(pt);
    }

    PT_INIT(pt_sub);
    while (PT_SCHEDULE(wf_scan_thrd(pt_sub, pnic_info, prsn)))
    {
        if (!wf_msg_get(pmsg_que, &pmsg))
        {
            if (pmsg->tag == WF_MLME_TAG_SCAN_ABORT)
            {
                MLME_DBG("abort scanning...");
                wf_scan_stop(pnic_info);
                wf_msg_del(pmsg_que, pmsg);
                PT_WAIT_THREAD(pt, wf_scan_thrd(pt_sub, pnic_info, prsn));
                break;
            }
            else if (pmsg->tag == WF_MLME_TAG_CONN_ABORT ||
                     pmsg->tag == WF_MLME_TAG_DEAUTH ||
                     pmsg->tag == WF_MLME_TAG_DEASSOC)
            {
                wf_scan_stop(pnic_info);
                PT_WAIT_THREAD(pt, wf_scan_thrd(pt_sub, pnic_info, prsn));
                break;
            }
        }
        PT_YIELD(pt);
    }
    /* notify system scan result */
    {
        mlme_conn_t *pconn_req = (mlme_conn_t *)pmlme_info->pcur_msg->value;
        wf_os_api_ind_scan_done(pnic_info, *prsn == WF_SCAN_TAG_ABORT,
                                pconn_req->framework);
    }

    PT_END(pt);
}

static
wf_pt_rst_t core_conn_maintain_probe_thrd (wf_pt_t *pt, nic_info_st *pnic_info,
                                           int *prsn)
{
    wf_pt_t *pt_sub = &pt[1];
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_msg_que_t *pmsg_que = &pmlme_info->msg_que;
    wf_msg_t *pmsg;
    int rst;

    PT_BEGIN(pt);

    {
        wf_wlan_ssid_t ssid;
        wdn_net_info_st *pwdn_info =
            wf_wdn_find_info(pnic_info, wf_wlan_get_cur_bssid(pnic_info));
        if (pwdn_info == NULL)
        {
            MLME_ERROR("wdn null");
            *prsn = -1;
            PT_EXIT(pt);
        }
        wf_memcpy(&ssid.data, pwdn_info->ssid,
                  WF_MIN(sizeof(wf_80211_mgmt_ssid_t), WF_SSID_LEN));
        rst = wf_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                            (void *)pwdn_info->bssid,
                            &ssid, 1,
                            &pwdn_info->channel, 1);
    }
    if (rst)
    {
        MLME_WARN("start fail error code: %d", *prsn);
        *prsn = -2;
        PT_EXIT(pt);
    }

    PT_INIT(pt_sub);
    while (PT_SCHEDULE(wf_scan_thrd(pt_sub, pnic_info, prsn)))
    {
        if (!wf_msg_get(pmsg_que, &pmsg))
        {
            if (pmsg->tag == WF_MLME_TAG_SCAN_ABORT ||
                pmsg->tag == WF_MLME_TAG_SCAN_RSP)
            {
                wf_msg_del(pmsg_que, pmsg);
                MLME_DBG("abort scanning...");
                wf_scan_stop(pnic_info);
                PT_WAIT_THREAD(pt, wf_scan_thrd(pt_sub, pnic_info, prsn));
                break;
            }
            else if (pmsg->tag == WF_MLME_TAG_CONN_ABORT ||
                     pmsg->tag == WF_MLME_TAG_DEAUTH ||
                     pmsg->tag == WF_MLME_TAG_DEASSOC)
            {
                wf_scan_stop(pnic_info);
                PT_WAIT_THREAD(pt, wf_scan_thrd(pt_sub, pnic_info, prsn));
                break;
            }
        }
        PT_YIELD(pt);
    }

    PT_END(pt);
}

wf_inline int core_conn_maintain_deauth (nic_info_st *pnic_info)
{
    hw_info_st *phw_info = pnic_info->hw_info;

    wf_mcu_set_user_info(pnic_info, wf_false);
    wf_action_frame_del_ba_request(pnic_info);
    wf_deauth_xmit_frame(pnic_info, wf_wlan_get_cur_bssid(pnic_info),
                         WF_80211_REASON_DEAUTH_LEAVING);
    {
        mlme_info_t *pmlme_info = pnic_info->mlme_info;
        mlme_conn_t *pconn_req = (mlme_conn_t *)pmlme_info->pcur_msg->value;
        wf_os_api_ind_disconnect(pnic_info, pconn_req->framework);
    }

    if (phw_info->ba_enable == wf_true)
    {

        wf_rx_action_ba_ctl_deinit(wf_wdn_find_info(pnic_info,
                                   wf_wlan_get_cur_bssid(pnic_info)));
    }

    wf_mcu_set_user_info(pnic_info, wf_false);

    return 0;
}

wf_inline int core_conn_maintain_deassoc (nic_info_st *pnic_info)
{
    hw_info_st *phw_info = pnic_info->hw_info;

    wf_mcu_set_user_info(pnic_info, wf_false);
    wf_action_frame_del_ba_request(pnic_info);
    {
        mlme_info_t *pmlme_info = pnic_info->mlme_info;
        mlme_conn_t *pconn_req = (mlme_conn_t *)pmlme_info->pcur_msg->value;
        wf_os_api_ind_disconnect(pnic_info, pconn_req->framework);
    }

    if (phw_info->ba_enable == wf_true)
    {
        wf_rx_action_ba_ctl_deinit(wf_wdn_find_info(pnic_info,
                                   wf_wlan_get_cur_bssid(pnic_info)));
    }

    wf_mcu_set_user_info(pnic_info, wf_false);

    return 0;
}

wf_inline int core_conn_maintain_ba_req (nic_info_st *pnic_info)
{
    hw_info_st *phw_info = pnic_info->hw_info;
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wdn_net_info_st *pwdn_info =
        wf_wdn_find_info(pnic_info, wf_wlan_get_cur_bssid(pnic_info));
    if (pwdn_info == NULL)
    {
        return -1;
    }

    if (phw_info->ba_enable)
    {
        if (wf_action_frame_ba_to_issue(pnic_info,
                                        WF_WLAN_ACTION_ADDBA_REQ) < 0)
        {
            MLME_WARN("*** wf_action_frame_ba_to_issue(WF_WLAN_ACTION_ADDBA_REQ) failed***");
        }
        pwdn_info->ba_started_flag[pmlme_info->bareq_parm.tid] = wf_true;
    }

    return 0;
}

wf_inline int core_conn_maintain_ba_rsp (nic_info_st *pnic_info)
{
    hw_info_st *phw_info = pnic_info->hw_info;
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wdn_net_info_st *pwdn_info =
        wf_wdn_find_info(pnic_info, wf_wlan_get_cur_bssid(pnic_info));
    if (pwdn_info == NULL)
    {
        return -1;
    }

    if (phw_info->ba_enable)
    {
        if (wf_action_frame_ba_to_issue(pnic_info,
                                        WF_WLAN_ACTION_ADDBA_RESP) < 0)
        {
            pwdn_info->ba_ctl[pmlme_info->barsp_parm.tid].enable = wf_false;
            MLME_WARN("*** wf_action_frame_ba_to_issue(WF_WLAN_ACTION_ADDBA_RESP) failed***");
        }
        else
        {
            pwdn_info->ba_ctl[pmlme_info->barsp_parm.tid].enable = wf_true;
            pwdn_info->ba_ctl[pmlme_info->barsp_parm.tid].wait_timeout =
                pmlme_info->bareq_parm.timeout;
        }
    }

    return 0;
}

static
wf_pt_rst_t core_conn_maintain_msg_thrd (wf_pt_t *pt, nic_info_st *pnic_info)
{
    wf_pt_t *pt_sub = &pt[1];
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_msg_que_t *pmsg_que = &pmlme_info->msg_que;
    wf_msg_t *pmsg;
    int reason;

    PT_BEGIN(pt);

    for (;;)
    {
        mlme_set_state(pnic_info, MLME_STATE_IDLE);
        /* wait new message */
        PT_YIELD_UNTIL(pt, !wf_msg_pop(pmsg_que, &pmsg));

        if (pmsg->tag == WF_MLME_TAG_SCAN)
        {
            wf_msg_del(pmsg_que, pmsg);
            MLME_INFO("scan...");
            mlme_set_state(pnic_info, MLME_STATE_SCAN);
            PT_SPAWN(pt, pt_sub,
                     core_conn_maintain_scan_thrd(pt_sub, pnic_info, &reason));
        }

        else if (pmsg->tag == WF_MLME_TAG_KEEPALIVE)
        {
            wf_msg_del(pmsg_que, pmsg);
            MLME_INFO("keepalive...");
            mlme_set_state(pnic_info, MLME_STATE_SCAN);
            PT_SPAWN(pt, pt_sub,
                     core_conn_maintain_probe_thrd(pt_sub, pnic_info, &reason));
        }

        else if (pmsg->tag == WF_MLME_TAG_DEAUTH ||
                 pmsg->tag == WF_MLME_TAG_CONN_ABORT)
        {
            wf_msg_del(pmsg_que, pmsg);
            MLME_INFO("deauth");
            mlme_set_state(pnic_info, MLME_STATE_DEAUTH);
            core_conn_maintain_deauth(pnic_info);
            break;
        }

        else if (pmsg->tag == WF_MLME_TAG_DEASSOC)
        {
            wf_msg_del(pmsg_que, pmsg);
            MLME_INFO("deassoc");
            mlme_set_state(pnic_info, MLME_STATE_DEASSOC);
            core_conn_maintain_deassoc(pnic_info);
            break;
        }

        else if (pmsg->tag == WF_MLME_TAG_ADD_BA_REQ)
        {
            wf_msg_del(pmsg_que, pmsg);
            MLME_INFO("ba request");
            mlme_set_state(pnic_info, MLME_STATE_ADD_BA_REQ);
            core_conn_maintain_ba_req(pnic_info);
        }

        else if (pmsg->tag == WF_MLME_TAG_ADD_BA_RSP)
        {
            wf_msg_del(pmsg_que, pmsg);
            MLME_INFO("ba respone");
            mlme_set_state(pnic_info, MLME_STATE_ADD_BA_RESP);
            core_conn_maintain_ba_rsp(pnic_info);
        }

        else
        {
            wf_msg_del(pmsg_que, pmsg);
            MLME_INFO("unsuited message(tag: %d)", pmsg->tag);
        }
    }

    PT_END(pt);
}

static int core_conn_maintain_traffic (nic_info_st *pnic_info)
{
    mlme_info_t *pmlme_info  = (mlme_info_t*)pnic_info->mlme_info;
    tx_info_st *ptx_info = pnic_info->tx_info;
    wf_u16 BusyThreshold;

#if 0
    MLME_DBG("num_tx_ok_in_period=%d  num_rx_ok_in_period=%d",
             pmlme_info->link_info.num_tx_ok_in_period,
             pmlme_info->link_info.num_rx_ok_in_period);
#endif

    if (ptx_info == NULL)
    {
        MLME_WARN("tx_info NULL");
    }

    if (!wf_timer_expired(&pmlme_info->traffic_timer))
    {
        return 0;
    }
    wf_timer_reset(&pmlme_info->traffic_timer);

    {
        wf_u16 BusyThresholdHigh    = 100;
        wf_u16 BusyThresholdLow     = 75;
        BusyThreshold = pmlme_info->link_info.busy_traffic ?
                        BusyThresholdLow : BusyThresholdHigh;
        if (pmlme_info->link_info.num_rx_ok_in_period > BusyThreshold ||
            pmlme_info->link_info.num_tx_ok_in_period > BusyThreshold)
        {
            pmlme_info->link_info.busy_traffic = wf_true;
        }
        else
        {
            pmlme_info->link_info.busy_traffic = wf_false;
        }
    }

    {
        int i;
        for (i = 0; i < TID_NUM; i++)
        {
            pmlme_info->link_info.num_tx_ok_in_period_with_tid[i] = 0;
        }
        pmlme_info->link_info.num_rx_ok_in_period = 0;
        pmlme_info->link_info.num_tx_ok_in_period = 0;
        pmlme_info->link_info.num_rx_unicast_ok_in_period = 0;
    }

#ifdef CONFIG_LPS
    wf_lps_sleep_mlme_monitor(pnic_info);
#endif

    return 0;
}

static wf_pt_rst_t
core_conn_maintain_keepalive_thrd (wf_pt_t *pt,  nic_info_st *pnic_info)
{
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wdn_net_info_st *pwdn_info = pmlme_info->pwdn_info;
    int rst;

    PT_BEGIN(pt);

    pwdn_info = pmlme_info->pwdn_info =
        wf_wdn_find_info(pnic_info, wf_wlan_get_cur_bssid(pnic_info));
    if (pwdn_info == NULL)
    {
        return -1;
    }

    for (;;)
    {
        PT_WAIT_WHILE(pt, pmlme_info->link_info.busy_traffic);

        pwdn_info->rx_pkt_stat_last = pwdn_info->rx_pkt_stat;
        wf_timer_set(&pmlme_info->keep_alive_timer, 10 * 1000);
        PT_WAIT_UNTIL(pt, wf_timer_expired(&pmlme_info->keep_alive_timer));
        if (pwdn_info->rx_pkt_stat_last != pwdn_info->rx_pkt_stat)
        {
            continue;
        }
        rst = mlme_msg_send(pnic_info, WF_MLME_TAG_KEEPALIVE, NULL, 0);
        if (rst)
        {
            MLME_WARN("mlme_msg_send fail, error code: %d", rst);
            continue;
        }

        wf_timer_set(&pmlme_info->keep_alive_timer, 10 * 1000);
        PT_WAIT_UNTIL(pt, wf_timer_expired(&pmlme_info->keep_alive_timer));
        if (pwdn_info->rx_pkt_stat_last != pwdn_info->rx_pkt_stat)
        {
            continue;
        }
        rst = wf_mlme_deauth(pnic_info);
        if (rst)
        {
            MLME_WARN("wf_mlme_deauth fail, error code: %d", rst);
            continue;
        }

        break;
    }

    PT_END(pt);
}

wf_inline static
int core_conn_maintain (wf_pt_t *pt, nic_info_st *pnic_info)
{
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_pt_t *pt_keepalive = &pt[1];
    wf_pt_t *pt_msg = &pt[2];

    PT_BEGIN(pt);

    PT_INIT(pt_keepalive);
    PT_INIT(pt_msg);
    wf_timer_set(&pmlme_info->traffic_timer, 1000);
    do
    {
        core_conn_maintain_traffic(pnic_info);
        core_conn_maintain_keepalive_thrd(pt_keepalive, pnic_info);
        PT_YIELD(pt);
    }
    while (PT_SCHEDULE(core_conn_maintain_msg_thrd(pt_msg, pnic_info)));

    PT_END(pt);
}

wf_inline static int mlme_conn_clearup (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = (wf_wlan_info_t *)pnic_info->wlan_info;
    sec_info_st *psec_info = pnic_info->sec_info;

    MLME_INFO();

    wf_wdn_remove(pnic_info, pwlan_info->cur_network.bssid);
    wf_memset(psec_info, 0x0, sizeof(sec_info_st));
    wf_memset(&pwlan_info->cur_network.ssid, '\0', sizeof(wf_wlan_ssid_t));
    wf_memset(pwlan_info->cur_network.bssid, 0x0, sizeof(wf_80211_bssid_t));

    return 0;
}

static wf_pt_rst_t mlme_core_thrd (nic_info_st *pnic_info)
{
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_pt_t *pt = &pmlme_info->pt[0], *pt_sub = &pt[1];
    wf_msg_que_t *pmsg_que = &pmlme_info->msg_que;
    wf_msg_t *pmsg;
    int reason;

    PT_BEGIN(pt);

    PT_WAIT_UNTIL(pt, !wf_msg_pop(pmsg_que, &pmsg));

    if (pmsg->tag == WF_MLME_TAG_SCAN)
    {
        mlme_set_state(pnic_info, MLME_STATE_SCAN);
        /* do scan process */
        pmlme_info->pcur_msg = pmsg;
        PT_SPAWN(pt, pt_sub,
                 core_scan_thrd(pt_sub, pnic_info,
                                (void *)pmlme_info->pcur_msg->value, &reason));
        /* delete scan request message */
        wf_msg_del(pmsg_que, pmlme_info->pcur_msg);
    }

    else if (pmsg->tag == WF_MLME_TAG_CONN)
    {
        mlme_conn_t *preq = (mlme_conn_t *)pmsg->value;
        MLME_INFO("start conneting to bss: \"%s\"", preq->ssid.data);

        /* launch probe request to find target bss */
        MLME_INFO("search bss...");
        mlme_set_state(pnic_info, MLME_STATE_CONN_SCAN);
        pmlme_info->pcur_msg = pmsg;
        PT_SPAWN(pt, pt_sub,
                 core_conn_scan_thrd(pt_sub, pnic_info,
                                     (void *)pmlme_info->pcur_msg->value,
                                     &reason));
        /* delete connect request message */
        wf_msg_del(pmsg_que, pmlme_info->pcur_msg);
        if (reason)
        {
            MLME_WARN("search bss fail, error code: %d", reason);
            goto exit;
        }
        MLME_INFO("found bss");

        /* make a new wdn */
        MLME_INFO("build wdn infomation");
        reason = build_wdn(pnic_info);
        if (reason)
        {
            MLME_WARN("new wdn fail, error code: %d", reason);
            goto conn_clearup;
        }

        /* auth process */
        MLME_INFO("auth...");
        mlme_set_state(pnic_info, MLME_STATE_AUTH);
        PT_SPAWN(pt, pt_sub, core_conn_auth_thrd(pt_sub, pnic_info, &reason));
        if (reason != WF_AUTH_TAG_DONE)
        {
            MLME_WARN("auth fail: auth error code: %d", reason);
            goto conn_clearup;
        }
        MLME_INFO("auth success");

        /* assoc process */
        MLME_INFO("assoc...");
        mlme_set_state(pnic_info, MLME_STATE_ASSOC);
        PT_SPAWN(pt, pt_sub, core_conn_assoc_thrd(pt_sub, pnic_info, &reason));
        if (reason != WF_ASSOC_TAG_DONE)
        {
            MLME_WARN("assoc fail: assoc error code: %d", reason);
            goto conn_clearup;
        }
        MLME_INFO("assoc success");

        /* prepare connect handle */
        reason = core_conn_preconnect(pnic_info);
        if (reason)
        {
            MLME_WARN("connect fail: preconnect error code: %d", reason);
            goto conn_clearup;
        }
        wf_mlme_set_connect(pnic_info, wf_true);
        MLME_INFO("connect success");

        /* connection maintain handler */
        MLME_INFO("connection maintain");
        PT_SPAWN(pt, pt_sub, core_conn_maintain(pt_sub, pnic_info));
        wf_mlme_set_connect(pnic_info, wf_false);
        MLME_INFO("connection break");

conn_clearup:
        mlme_conn_clearup(pnic_info);
    }

    else
    {
        MLME_INFO("drop unsuited message(tag: %d)", pmsg->tag);
        wf_msg_del(pmsg_que, pmsg);
    }

exit:
    mlme_set_state(pnic_info, MLME_STATE_IDLE);
    /* restart thread */
    PT_RESTART(pt);

    PT_END(pt);
}

static int mlme_core (nic_info_st *pnic_info)
{
    mlme_info_t *pmlme_info = pnic_info->mlme_info;

    MLME_DBG();

    PT_INIT(&pmlme_info->pt[0]);

	wf_os_api_thread_affinity(DEFAULT_CPU_ID);

    for (;;)
    {
        if (WF_CANNOT_RUN(pnic_info))
        {
            if (wf_os_api_thread_wait_stop(pmlme_info->tid))
            {
                MLME_DBG("thread destory...");
                return 0;
            }
            wf_yield();
            continue;
        }

        if ((get_sys_work_mode(pnic_info) != WF_INFRA_MODE &&
              get_sys_work_mode(pnic_info) != WF_ADHOC_MODE) || !pnic_info->is_up)
        {
            wf_yield();
            continue;
        }

        if (!PT_SCHEDULE(mlme_core_thrd(pnic_info)))
        {
            MLME_WARN("mlme thread termination");
            break;
        }
        wf_yield();
    }

    MLME_DBG("wait for thread destory...");
    while (!wf_os_api_thread_wait_stop(pmlme_info->tid))
    {
        wf_yield();
    }

	wf_os_api_thread_exit(pmlme_info->tid);

    return 0;
}

static int mlme_msg_send (nic_info_st *pnic_info,
                          wf_msg_tag_t tag, void *value, wf_u8 len)
{
    mlme_info_t *pmlme_info;
    wf_msg_que_t *pmsg_que;
    wf_msg_t *pmsg;
    int rst;

    if (pnic_info == NULL)
    {
        return -1;
    }

    pmlme_info = pnic_info->mlme_info;
    if (pmlme_info == NULL)
    {
        return -2;
    }

    pmsg_que = &pmlme_info->msg_que;
    rst = wf_msg_new(pmsg_que, tag, &pmsg);
    if (rst)
    {
        MLME_WARN("wf_msg_new fail error code: %d", rst);
        return -3;
    }
    if (value && len)
    {
        pmsg->len = len;
        wf_memcpy(pmsg->value, value, len);
    }

    rst = wf_msg_push(pmsg_que, pmsg);
    if (rst)
    {
        wf_msg_del(pmsg_que, pmsg);
        MLME_WARN("wf_msg_push fail error code: %d", rst);
        return -4;
    }

    return 0;
}

int wf_mlme_scan_start (nic_info_st *pnic_info, scan_type_e type,
                        wf_wlan_ssid_t ssids[], wf_u8 ssid_num,
                        wf_u8 chs[], wf_u8 ch_num,
                        wf_mlme_framework_e frm_work)
{
    mlme_info_t *pmlme_info;
    wf_msg_que_t *pmsg_que;
    wf_msg_t *pmsg;
    int rst;

    if (pnic_info == NULL)
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        MLME_WARN("ndev is down");
        return -2;
    }

    MLME_DBG();

    pmlme_info = pnic_info->mlme_info;
    if (pmlme_info == NULL)
    {
        return -3;
    }

    pmsg_que = &pmlme_info->msg_que;
    rst = wf_msg_new(pmsg_que, WF_MLME_TAG_SCAN, &pmsg);
    if (rst)
    {
        MLME_WARN("msg new fail error code: %d", rst);
        return -4;
    }

    {
        mlme_scan_t *param = (mlme_scan_t *)pmsg->value;
        param->type = type;
        param->ssid_num = ssid_num;
        if (ssid_num)
        {
            wf_memcpy(param->ssids, ssids, ssid_num * sizeof(param->ssids[0]));
        }
        param->ch_num = ch_num;
        if (ch_num)
        {
            wf_memcpy(param->chs, chs, ch_num * sizeof(param->chs[0]));
        }
        param->framework = frm_work;
    }

    rst = wf_msg_push(pmsg_que, pmsg);
    if (rst)
    {
        wf_msg_del(pmsg_que, pmsg);
        MLME_WARN("msg push fail error code: %d", rst);
        return -5;
    }

    return 0;
}

int wf_mlme_scan_abort (nic_info_st *pnic_info)
{
    int rst;

    if (pnic_info == NULL)
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        MLME_WARN("ndev down");
        return -2;
    }

    MLME_DBG();

    rst = mlme_msg_send(pnic_info, WF_MLME_TAG_SCAN_ABORT, NULL, 0);
    if (rst)
    {
        return -3;
    }

    return 0;
}

int wf_mlme_conn_scan_rsp (nic_info_st *pnic_info,
                           wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    mlme_info_t *pmlme_info;
    wf_msg_que_t *pmsg_que;
    wf_msg_t *pmsg;
    mlme_conn_t *param;
    int rst;

    if (pnic_info == NULL || (pmgmt == NULL && mgmt_len == 0))
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        MLME_WARN("ndev down");
        return -2;
    }

    if (!wf_is_scanning(pnic_info))
    {
        return -3;
    }
    pmlme_info = pnic_info->mlme_info;
    pmsg_que = &pmlme_info->msg_que;

    MLME_DBG();

    rst = wf_msg_new(pmsg_que, WF_MLME_TAG_SCAN_RSP, &pmsg);
    if (rst)
    {
        MLME_WARN("msg new fail error code: %d", rst);
        return -4;
    }
    /* copy frame */
    pmsg->len = mgmt_len;
    wf_memcpy(pmsg->value, pmgmt, mgmt_len);
    rst = wf_msg_push(pmsg_que, pmsg);
    if (rst)
    {
        wf_msg_del(pmsg_que, pmsg);
        MLME_WARN("msg push fail error code: %d", rst);
        return -5;
    }

    return 0;
}

int wf_mlme_conn_start (nic_info_st *pnic_info, wf_80211_bssid_t bssid,
                        wf_wlan_ssid_t *pssid, wf_mlme_framework_e frm_work)
{
    mlme_info_t *pmlme_info;
    wf_msg_que_t *pmsg_que;
    wf_msg_t *pmsg;
    mlme_conn_t *param;
    int rst;

    if (pnic_info == NULL || (bssid == NULL && pssid == NULL))
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        MLME_WARN("ndev down");
        return -2;
    }
    pmlme_info = pnic_info->mlme_info;
    pmsg_que = &pmlme_info->msg_que;

    MLME_DBG();

    if (!wf_msg_get(pmsg_que, &pmsg) && pmsg->tag == WF_MLME_TAG_CONN)
    {
        wf_msg_del(pmsg_que, pmsg);
    }
    else
    {
        wf_mlme_conn_abort(pnic_info);
    }

    rst = wf_msg_new(pmsg_que, WF_MLME_TAG_CONN, &pmsg);
    if (rst)
    {
        MLME_WARN("msg new fail error code: %d", rst);
        return -3;
    }
    param = (mlme_conn_t *)pmsg->value;
    /* set bssid */
    if (bssid)
    {
        wf_memcpy(param->bssid, bssid, sizeof(param->bssid));
    }
    else
    {
        wf_memset(param->bssid, 0, sizeof(param->bssid));
    }
    /* set ssid */
    if (pssid && pssid->length)
    {
        wf_memcpy(&param->ssid, pssid, sizeof(param->ssid));
    }
    else
    {
        param->ssid.length = 0;
    }
    /* set framework */
    param->framework = frm_work;

    rst = wf_msg_push(pmsg_que, pmsg);
    if (rst)
    {
        wf_msg_del(pmsg_que, pmsg);
        MLME_WARN("msg push fail error code: %d", rst);
        return -4;
    }

    return 0;
}

int wf_mlme_conn_abort (nic_info_st *pnic_info)
{
    int rst;

    if (pnic_info == NULL)
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        MLME_WARN("ndev down");
        return -2;
    }

    MLME_DBG();

    rst = mlme_msg_send(pnic_info, WF_MLME_TAG_CONN_ABORT, NULL, 0);
    if (rst)
    {
        return -3;
    }

    return 0;
}

int wf_mlme_deauth (nic_info_st *pnic_info)
{
    int rst;

    if (pnic_info == NULL)
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        MLME_WARN("ndev down");
        return -2;
    }

    MLME_DBG();

    rst = mlme_msg_send(pnic_info, WF_MLME_TAG_DEAUTH, NULL, 0);
    if (rst)
    {
        return -3;
    }

    return 0;
}

int wf_mlme_deassoc (nic_info_st *pnic_info)
{
    int rst;

    if (pnic_info == NULL)
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        MLME_WARN("ndev down");
        return -2;
    }

    MLME_DBG();

    rst = mlme_msg_send(pnic_info, WF_MLME_TAG_DEASSOC, NULL, 0);
    if (rst)
    {
        return -3;
    }

    return 0;
}

int wf_mlme_add_ba_req (nic_info_st *pnic_info)
{
    int rst;

    if (pnic_info == NULL)
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        MLME_WARN("ndev down");
        return -2;
    }

    MLME_DBG();

    rst = mlme_msg_send(pnic_info, WF_MLME_TAG_ADD_BA_REQ, NULL, 0);
    if (rst)
    {
        return -3;
    }

    return 0;
}

int wf_mlme_add_ba_rsp (nic_info_st *pnic_info)
{
    int rst;

    if (pnic_info == NULL)
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        MLME_WARN("ndev down");
        return -2;
    }

    MLME_DBG();

    rst = mlme_msg_send(pnic_info, WF_MLME_TAG_ADD_BA_RSP, NULL, 0);
    if (rst)
    {
        return -3;
    }

    return 0;
}

int mlme_set_state (nic_info_st *pnic_info, mlme_state_e state)
{
    mlme_info_t *pmlme_info;
    wf_irq irq;

    if (pnic_info == NULL)
    {
        return -1;
    }

    pmlme_info = pnic_info->mlme_info;
    if (pmlme_info == NULL)
    {
        return -2;
    }

    wf_lock_irq_lock(&pmlme_info->state_lock, &irq);
    pmlme_info->state = state;
    wf_lock_irq_unlock(&pmlme_info->state_lock, &irq);

    return 0;
}

int wf_mlme_get_state (nic_info_st *pnic_info, mlme_state_e *state)
{
    mlme_info_t *pmlme_info;
    wf_irq irq;

    if (pnic_info == NULL)
    {
        return -1;
    }

    pmlme_info = pnic_info->mlme_info;
    if (pmlme_info == NULL)
    {
        return -2;
    }

    wf_lock_irq_lock(&pmlme_info->state_lock, &irq);
    *state = pmlme_info->state;
    wf_lock_irq_unlock(&pmlme_info->state_lock, &irq);

    return 0;
}

int wf_mlme_set_connect (nic_info_st *pnic_info, wf_bool bconnect)
{
    mlme_info_t *pmlme_info;

    if (pnic_info == NULL)
    {
        return -1;
    }

    pmlme_info = pnic_info->mlme_info;
    if (pmlme_info == NULL)
    {
        return -2;
    }

    wf_lock_spin_lock(&pmlme_info->connect_lock);
    pmlme_info->connect = bconnect;
    wf_lock_spin_unlock(&pmlme_info->connect_lock);

    return 0;
}

int wf_mlme_get_connect (nic_info_st *pnic_info, wf_bool *bconnect)
{
    mlme_info_t *pmlme_info;

    if (pnic_info == NULL)
    {
        return -1;
    }

    pmlme_info = pnic_info->mlme_info;
    if (pmlme_info == NULL)
    {
        return -2;
    }

    wf_lock_spin_lock(&pmlme_info->connect_lock);
    *bconnect = pmlme_info->connect;
    wf_lock_spin_unlock(&pmlme_info->connect_lock);

    return 0;
}

int wf_mlme_get_traffic_busy (nic_info_st *pnic_info, wf_bool *bbusy)
{
    mlme_info_t *pmlme_info;

    if (pnic_info == NULL || bbusy == NULL)
    {
        return -1;
    }

    pmlme_info = pnic_info->mlme_info;
    if (pmlme_info == NULL)
    {
        return -2;
    }

    *bbusy = pmlme_info->link_info.busy_traffic;

    return 0;
}

wf_inline static int mlme_msg_init (wf_msg_que_t *pmsg_que)
{
    wf_msg_init(pmsg_que);
    return (wf_msg_alloc(pmsg_que, WF_MLME_TAG_SCAN_ABORT, 0, 2) ||
            wf_msg_alloc(pmsg_que, WF_MLME_TAG_CONN_ABORT, 0, 2) ||
            wf_msg_alloc(pmsg_que, WF_MLME_TAG_DEAUTH, 0, 1) ||
            wf_msg_alloc(pmsg_que, WF_MLME_TAG_DEASSOC, 0, 1) ||
            wf_msg_alloc(pmsg_que, WF_MLME_TAG_SCAN, sizeof(mlme_scan_t), 2) ||
            wf_msg_alloc(pmsg_que, WF_MLME_TAG_SCAN_RSP, sizeof(mlme_scan_rsp_t), 1) ||
            wf_msg_alloc(pmsg_que, WF_MLME_TAG_CONN, sizeof(mlme_conn_t), 2) ||
            wf_msg_alloc(pmsg_que, WF_MLME_TAG_ADD_BA_REQ, 0, 1) ||
            wf_msg_alloc(pmsg_que, WF_MLME_TAG_ADD_BA_RSP, 0, 1) ||
            wf_msg_alloc(pmsg_que, WF_MLME_TAG_KEEPALIVE, 0, 1)) ? -1 : 0;
}

wf_inline static int mlme_msg_deinit (wf_msg_que_t *pmsg_que)
{
    return wf_msg_free(pmsg_que);
}

int wf_mlme_init (nic_info_st *pnic_info)
{
    mlme_info_t *pmlme_info;

    MLME_DBG();

    pmlme_info = wf_kzalloc(sizeof(mlme_info_t));
    if (pmlme_info == NULL)
    {
        MLME_WARN("malloc mlme_info_t failed");
        return -1;
    }
    pnic_info->mlme_info = pmlme_info;

    wf_lock_spin_init(&pmlme_info->state_lock);
    wf_lock_spin_init(&pmlme_info->connect_lock);
    wf_mlme_set_connect(pnic_info, wf_false);
    mlme_set_state(pnic_info, MLME_STATE_IDLE);
    if (mlme_msg_init(&pmlme_info->msg_que))
    {
        MLME_WARN("malloc msg init failed");
        return -2;
    }
    sprintf(pmlme_info->mlmeName,
            pnic_info->virNic ? "mlme_sm:vir%d_s%d" : "mlme_sm:wlan%d_s%d",
            pnic_info->hif_node_id, pnic_info->ndev_id);

    pmlme_info->tid = wf_os_api_thread_create(pmlme_info->tid,
                                              pmlme_info->mlmeName,
                                              mlme_core,
                                              pnic_info);
    if (pmlme_info->tid == NULL)
    {
        MLME_WARN("create mlme thread failed");
        return -3;
    }
    wf_os_api_thread_wakeup(pmlme_info->tid);

    return 0;
}

int wf_mlme_term (nic_info_st *pnic_info)
{
    mlme_info_t *pmlme_info;

    pmlme_info = pnic_info->mlme_info;
    if (pmlme_info == NULL)
    {
        return 0;
    }

    MLME_DBG("destory thread");
    if (pmlme_info->tid)
    {
        wf_os_api_thread_destory(pmlme_info->tid);
        pmlme_info->tid = 0;
    }

    MLME_DBG("del msg que");
    mlme_msg_deinit(&pmlme_info->msg_que);

    MLME_DBG("del lock");
    wf_lock_spin_free(&pmlme_info->state_lock);
    wf_lock_spin_free(&pmlme_info->connect_lock);

    MLME_DBG("free pmlme_info");
    wf_kfree(pmlme_info);
    pnic_info->mlme_info = NULL;

    MLME_DBG("end");

    return 0;
}

