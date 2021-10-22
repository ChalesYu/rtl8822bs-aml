/*
 * scan.c
 *
 * impliment of IEEE80211 management frame scan stage processing
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "common.h"
#include "wf_debug.h"

/* macro */
#if 0
#define SCAN_DBG(fmt, ...)      LOG_D("[%s:%d][%d]"fmt, __func__, __LINE__, pnic_info->ndev_id, ##__VA_ARGS__)
#define SCAN_ARRAY(data, len)   log_array(data, len)
#else
#define SCAN_DBG(fmt, ...)
#define SCAN_ARRAY(data, len)
#endif
#define SCAN_INFO(fmt, ...)     LOG_I("[%s:%d][%d]"fmt, __func__, __LINE__, pnic_info->ndev_id, ##__VA_ARGS__)
#define SCAN_WARN(fmt, ...)     LOG_W("[%s:%d][%d]"fmt, __func__, __LINE__, pnic_info->ndev_id, ##__VA_ARGS__)
#define SCAN_ERROR(fmt, ...)    LOG_E("[%s:%d][%d]"fmt, __func__, __LINE__, pnic_info->ndev_id, ##__VA_ARGS__)

#define LOCAL_INFO                  ((local_info_st *)pnic_info->local_info)
#define SCAN_CH_TIMEOUT             LOCAL_INFO->scan_ch_to
#define SCAN_PROBE_RESEND_TIMES     LOCAL_INFO->scan_prb_times

wf_inline static int tx_cutoff (nic_info_st *pnic_info)
{
    wf_tx_xmit_stop(pnic_info);

    return 0;
}

wf_inline static int tx_resume (nic_info_st *pnic_info)
{
    wf_tx_xmit_start(pnic_info);

    return 0;
}

wf_inline static wf_bool is_tx_empty (nic_info_st *pnic_info)
{
    return !!wf_tx_xmit_hif_queue_empty(pnic_info);
}

wf_inline static
int scan_setting (nic_info_st *pnic_info)
{
    wf_scan_info_t *pscan_info = pnic_info->scan_info;
    wf_bool bch_spec = !!pscan_info->preq->ch_num;

    if (pscan_info->preq->type == SCAN_TYPE_PASSIVE)
    {
        if (wf_mcu_set_media_status(pnic_info, WIFI_FW_NULL_STATE))
        {
            return -2;
        }

        SCAN_INFO("Disbale BSSID Filter");
        if (wf_mcu_set_mlme_scan(pnic_info, wf_true))
        {
            return -3;
        }
    }
    else
    {
        wf_bool bfix_ch = wf_false;
        if (wf_80211_is_valid_bssid(pscan_info->preq->bssid))
        {
            wf_wlan_mgmt_scan_que_node_t *pscan_que_node;
            wf_wlan_mgmt_scan_que_for_rst_e rst;
            SCAN_DBG("target bssid: "WF_MAC_FMT,
                     WF_MAC_ARG(pscan_info->preq->bssid));
            wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
            {
                if (!wf_memcmp(pscan_que_node->bssid, pscan_info->preq->bssid,
                               sizeof(pscan_que_node->bssid)))
                {
                    SCAN_DBG("found bss in scan queue");
                    break;
                }
            }
            wf_wlan_mgmt_scan_que_for_end(rst);

            if (rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL)
            {
                SCAN_WARN("get semphone fail!!!!!!!!!!!!!!!!!!!!!!!!!");
//                return -4;
            }

            if (rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_BREAK)
            {
                if (bch_spec)
                {
                    wf_u8 i;
                    for (i = 0; i < pscan_info->preq->ch_num; i++)
                    {
                        if (pscan_que_node->channel == pscan_info->preq->ch_map[i])
                        {
                            bfix_ch = wf_true;
                            break;
                        }
                    }
                }
                else
                {
                    bfix_ch = wf_true;
                }

                if (bfix_ch)
                {
                    SCAN_DBG("fix scan channel number: %d",
                             pscan_que_node->channel);
                    pscan_info->preq->ch_num = 1;
                    pscan_info->preq->ch_map[0] = pscan_que_node->channel;
                }
            }

            if (wf_mcu_set_bssid(pnic_info, pscan_info->preq->bssid))
            {
                return -5;
            }
        }
        else
        {
            SCAN_INFO("Disbale BSSID Filter");
            if (wf_mcu_set_mlme_scan(pnic_info, wf_true))
            {
                return -6;
            }
        }
    }

    if (pscan_info->preq->ch_num == 0)
    {
        hw_info_st *phw_info = (hw_info_st *)pnic_info->hw_info;
        wf_u8 i;
        pscan_info->preq->ch_num = phw_info->max_chan_nums;
        for (i = 0; i < phw_info->max_chan_nums; i++)
        {
            pscan_info->preq->ch_map[i] = phw_info->channel_set[i].channel_num;
        }
    }

    if (bch_spec)
    {
//        wf_wlan_mgmt_scan_que_flush(pnic_info);
    }

    if (wf_hw_info_get_channnel_bw(pnic_info,
                                   &pscan_info->chnl_bak.number,
                                   &pscan_info->chnl_bak.width,
                                   &pscan_info->chnl_bak.offset) != WF_RETURN_OK)
    {
        return -7;
    }
#ifdef CONFIG_LPS
    {
        wf_bool bConnected = wf_false;
        wf_mlme_get_connect(pnic_info, &bConnected);

        if(bConnected == wf_true)
        {
            wf_lps_wakeup(pnic_info, LPS_CTRL_SCAN, 0);
            if (pnic_info->buddy_nic)
            {
                wf_lps_wakeup((nic_info_st *)(pnic_info->buddy_nic), LPS_CTRL_SCAN, 0);
            }
        }
    }
#endif

    return 0;
}

wf_inline static int scan_setting_recover (nic_info_st *pnic_info)
{
    wf_scan_info_t *pscan_info = pnic_info->scan_info;

    wf_mcu_set_media_status(pnic_info, WIFI_FW_STATION_STATE);

    if (wf_hw_info_set_channnel_bw(pnic_info,
                                   pscan_info->chnl_bak.number,
                                   pscan_info->chnl_bak.width,
                                   pscan_info->chnl_bak.offset) == WF_RETURN_FAIL)
    {
        SCAN_WARN("UMSG_OPS_HAL_CHNLBW_MODE failed");
        return -1;
    }

    wf_mcu_set_mlme_scan(pnic_info, wf_false);
    SCAN_INFO("Enable BSSID Filter");

    return 0;
}

int wf_scan_probe_send (nic_info_st *pnic_info)
{
    struct xmit_buf *pxmit_buf;
    wf_80211_mgmt_t *pframe;
    wf_scan_info_t *pscan_info;
    hw_info_st *hw_info;
    wf_u32 var_len;
    wf_u8 *pvar;

    SCAN_DBG();

    if (pnic_info == NULL)
    {
        return -1;
    }
    pscan_info  = pnic_info->scan_info;
    hw_info     = pnic_info->hw_info;

    /* alloc xmit_buf */
    {
        tx_info_st *ptx_info = pnic_info->tx_info;
        pxmit_buf = wf_xmit_extbuf_new(ptx_info);
        if (pxmit_buf == NULL)
        {
            SCAN_WARN("pxmit_buf is NULL");
            return -2;
        }
    }

    wf_memset(pxmit_buf->pbuf, 0,
              TXDESC_OFFSET + WF_OFFSETOF(wf_80211_mgmt_t, probe_req));
    pframe = (void *)&pxmit_buf->pbuf[TXDESC_OFFSET];

    wf_80211_set_frame_type(&pframe->frame_control, WF_80211_FRM_PROBE_REQ);

    wf_memset(pframe->da, 0xff, sizeof(pframe->da));
    wf_memcpy(pframe->sa, nic_to_local_addr(pnic_info), sizeof(pframe->sa));
    wf_memset(pframe->bssid, 0xff, sizeof(pframe->bssid));

    var_len = 0;
    pvar = &pframe->probe_req.variable[0];
    {
        if (pscan_info->preq->ssid_num)
        {
            pvar = set_ie(pvar, WF_80211_MGMT_EID_SSID,
                          pscan_info->preq->ssids[0].length,
                          pscan_info->preq->ssids[0].data,
                          &var_len);
        }
        else
        {
            pvar = set_ie(pvar, WF_80211_MGMT_EID_SSID, 0, NULL, &var_len);
        }
    }
    pvar = set_ie(pvar, WF_80211_MGMT_EID_SUPP_RATES,
                  8, &hw_info->datarate[0], &var_len);
    pvar = set_ie(pvar, WF_80211_MGMT_EID_EXT_SUPP_RATES,
                  4, &hw_info->datarate[8], &var_len);

    pxmit_buf->pkt_len =
        WF_OFFSETOF(wf_80211_mgmt_t, probe_req.variable) + var_len;
    if (wf_nic_mgmt_frame_xmit(pnic_info, NULL, pxmit_buf, pxmit_buf->pkt_len))
    {
        SCAN_WARN("probe frame send fail");
        return -3;
    }

    return 0;
}

wf_inline static
int check_bssid (wf_scan_info_t *pscan_info, wf_80211_mgmt_t *pmgmt)
{
    if (wf_80211_is_valid_bssid(pscan_info->preq->bssid))
    {
        if (wf_memcmp(pscan_info->preq->bssid,
                      pmgmt->bssid, sizeof(pscan_info->preq->bssid)))
        {
            return 1;
        }
    }

    return 0;
}

wf_inline static
int check_ssid (wf_scan_info_t *pscan_info, wf_u8 *pies, wf_u16 ies_len)
{
    wf_u8 i;
    wf_80211_mgmt_ie_t *pie;

    if (pscan_info->preq->ssid_num)
    {
        if (wf_80211_mgmt_ies_search(pies, ies_len, WF_80211_MGMT_EID_SSID, &pie))
        {
//            SCAN_WARN("no ssid element field");
            return -1;
        }
        for (i = 0; i < pscan_info->preq->ssid_num; i++)
        {
            if (pscan_info->preq->ssids[i].length == pie->len ||
                !wf_memcmp(pscan_info->preq->ssids[i].data, pie->data, pie->len))
            {
                break;
            }
        }
        if (i == pscan_info->preq->ssid_num)
        {
            return 1;
        }
    }

    return 0;
}

wf_inline static
int check_channel (wf_scan_info_t *pscan_info, wf_u8 *pies, wf_u16 ies_len)
{
    wf_80211_mgmt_ie_t *pie;

    if (wf_80211_mgmt_ies_search(pies, ies_len,
                                 WF_80211_MGMT_EID_DS_PARAMS, &pie))
    {
//        SCAN_WARN("no DS element field");
        return -1;
    }
    {
        wf_80211_mgmt_dsss_parameter_t *pds = (void *)pie->data;
        wf_u8 ch = pds->current_channel;
        wf_u8 i;
        for (i = 0; i < pscan_info->preq->ch_num; i++)
        {
            if (pscan_info->preq->ch_map[i] == ch)
            {
                break;
            }
        }
        if (i == pscan_info->preq->ch_num)
        {
            return 1;
        }
    }

    return 0;
}

int wf_scan_filter (nic_info_st *pnic_info,
                    wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    wf_scan_info_t *pscan_info;
    int rst = 0;

    if (pnic_info == NULL || pmgmt == NULL || mgmt_len == 0)
    {
        SCAN_WARN("invalid paramete");
        return -1;
    }
    pscan_info = pnic_info->scan_info;

    if (WF_CANNOT_RUN(pnic_info))
    {
        return -2;
    }

    if (!wf_is_scanning(pnic_info))
    {
        return 0;
    }

    if (wf_os_api_sema_try(&pscan_info->req_lock))
    {
        return 0;
    }

    {
        wf_u8 *pies = &pmgmt->probe_resp.variable[0];
        wf_u16 ies_len = mgmt_len - WF_OFFSETOF(struct beacon_ie, variable);

        rst = check_bssid(pscan_info, pmgmt);
        if (rst)
        {
            rst = rst < 0 ? -3 : 0;
            goto exit;
        }
        rst = check_ssid(pscan_info, pies, ies_len);
        if (rst)
        {
            rst = rst < 0 ? -4 : 0;
            goto exit;
        }
        rst = check_channel(pscan_info, pies, ies_len);
        if (rst)
        {
            rst = rst < 0 ? -5 : 0;
            goto exit;
        }
    }

    if (pscan_info->preq->type == SCAN_TYPE_ACTIVE &&
        wf_80211_get_frame_type(pmgmt->frame_control) == WF_80211_FRM_PROBE_RESP &&
        mac_addr_equal(pmgmt->da, nic_to_local_addr(pnic_info)))
    {
        mlme_state_e state;
        wf_mlme_get_state(pnic_info, &state);
        if (state == MLME_STATE_CONN_SCAN || state == MLME_STATE_IBSS_CONN_SCAN)
        {
            wf_mlme_conn_scan_rsp(pnic_info, pmgmt, mgmt_len);
        }
    }

exit:
    wf_os_api_sema_post(&pscan_info->req_lock);
    return rst;
}

wf_pt_ret_t wf_scan_thrd (wf_pt_t *pt, nic_info_st *pnic_info, int *prsn)
{
    wf_scan_info_t *pscan_info;
    wf_msg_que_t *pmsg_que;
    wf_msg_t *pmsg;
    int reason = WF_SCAN_TAG_DONE;
    int rst;
    p2p_info_st *p2p_info = pnic_info->p2p;

    if (pt == NULL || pnic_info == NULL || prsn == NULL)
    {
        PT_EXIT(pt);
    }
    pscan_info = pnic_info->scan_info;
    pmsg_que = &pscan_info->msg_que;

    PT_BEGIN(pt);

    SCAN_DBG();

    PT_WAIT_WHILE(pt, nic_mlme_hw_access_trylock(pnic_info));
    do
    {
        rst = wf_msg_pop(pmsg_que, &pmsg);
        if (rst)
        {
            SCAN_WARN("wf_msg_pop fail, error code: %d", rst);
            nic_mlme_hw_access_unlock(pnic_info);
            *prsn = -1;
            PT_EXIT(pt);
        }
        if (pmsg->tag != WF_SCAN_TAG_START)
        {
            SCAN_DBG("undesired message");
            wf_msg_del(pmsg_que, pmsg);
            PT_YIELD(pt);
            continue;
        }
        break;
    }
    while (wf_true);
    pscan_info->preq = (wf_scan_req_t *)pmsg->value;

#ifdef CONFIG_ARS_DRIVER_SUPPORT
   if (pnic_info->ars)
   {
       ars_st *pars = pnic_info->ars;

       wf_lock_lock(&pars->ctlLock);
   }
#endif
   wf_mcu_set_user_info(pnic_info, wf_false);

    /* stop framework data send behavior come into */
    tx_cutoff(pnic_info);
    SCAN_DBG("wait until tx cache empty......");
    wf_timer_set(&pscan_info->timer, 1000);
    while (!is_tx_empty(pnic_info))
    {
        if (!wf_msg_pop(pmsg_que, &pmsg))
        {
            if (pmsg->tag == WF_SCAN_TAG_ABORT)
            {
                wf_msg_del(pmsg_que, pmsg);
                SCAN_DBG("scan aborted");
                reason = -2;
                goto exit;
            }
        }
        if (wf_timer_expired(&pscan_info->timer))
        {
            SCAN_WARN("wait timeout");
            reason = -3;
            goto exit;
        }
        wf_msg_del(pmsg_que, pmsg);
        PT_YIELD(pt);
    }

    SCAN_DBG("scan setting...");
    rst = scan_setting(pnic_info);
    if (rst)
    {
        SCAN_WARN("scan setting fail, error code: %d", rst);
        reason = -4;
        goto exit;
    }

    SCAN_INFO("scanning...");
    pscan_info->brun = wf_true;
    wf_timer_set(&pscan_info->pass_time, 0);
    for (pscan_info->ch_idx = 0;
         pscan_info->ch_idx < pscan_info->preq->ch_num;
         pscan_info->ch_idx++)
    {
        SCAN_DBG("channel: %d", pscan_info->preq->ch_map[pscan_info->ch_idx]);
        rst = wf_hw_info_set_channnel_bw(pnic_info,
                                         pscan_info->preq->ch_map[pscan_info->ch_idx],
                                         CHANNEL_WIDTH_20,
                                         HAL_PRIME_CHNL_OFFSET_DONT_CARE);
        if (rst)
        {
            SCAN_WARN("set channel fail, error code: %d", rst);
            reason = -5;
            goto exit;
        }

        if (pscan_info->preq->type == SCAN_TYPE_ACTIVE)
        {
            for (pscan_info->retry_cnt = 0;
                 pscan_info->retry_cnt < SCAN_PROBE_RESEND_TIMES;
                 pscan_info->retry_cnt++)
            {
                if(wf_p2p_is_valid(pnic_info))
                {
                    //LOG_D("[%s,%d] p2p_state:%s",__func__,__LINE__,wf_p2p_state_to_str(pwdinfo->p2p_state));
                    if(p2p_info->p2p_state == P2P_STATE_SCAN ||
                       p2p_info->p2p_state == P2P_STATE_FIND_PHASE_SEARCH)
                    {
                        wf_wlan_set_cur_channel(pnic_info, pscan_info->preq->ch_map[pscan_info->ch_idx]);
                        rst = wf_p2p_issue_probereq(pnic_info,NULL);
                    }
                }
                else
                {
                    rst = wf_scan_probe_send(pnic_info);
                }

                if (rst)
                {
                    SCAN_WARN("wf_scan_probe_send failed, error code: %d", rst);
                    reason = -6;
                    goto exit;
                }

                wf_timer_set(&pscan_info->timer, SCAN_CH_TIMEOUT);
                do
                {
                    PT_WAIT_UNTIL(pt, !wf_msg_pop(pmsg_que, &pmsg) ||
                                  wf_timer_expired(&pscan_info->timer));
                    if (pmsg == NULL)
                    {
                        wf_timer_reset(&pscan_info->timer);
                        break;
                    }
                    if (pmsg->tag == WF_SCAN_TAG_ABORT)
                    {
                        wf_msg_del(pmsg_que, pmsg);
                        reason = pmsg->tag;
                        goto done;
                    }
                    wf_msg_del(pmsg_que, pmsg);
                }
                while (wf_true);
            }
        }
        else
        {
            wf_timer_set(&pscan_info->timer, SCAN_CH_TIMEOUT);
            do
            {
                PT_WAIT_UNTIL(pt, !wf_msg_pop(pmsg_que, &pmsg) ||
                              wf_timer_expired(&pscan_info->timer));
                if (pmsg == NULL)
                {
                    break;
                }
                if (pmsg->tag == WF_SCAN_TAG_ABORT)
                {
                    wf_msg_del(pmsg_que, pmsg);
                    reason = WF_SCAN_TAG_ABORT;
                    goto done;
                }
                wf_msg_del(pmsg_que, pmsg);
            }
            while (wf_true);
        }
    }
    reason = WF_SCAN_TAG_DONE;

done:
    SCAN_INFO("scan done pass time: %dms",
              wf_timer_elapsed(&pscan_info->pass_time));
    wf_wlan_mgmt_scan_que_refresh(pnic_info,
                                  pscan_info->preq->ch_map,
                                  pscan_info->preq->ch_num);

exit:
    if (pscan_info->brun)
    {
        scan_setting_recover(pnic_info);
        pscan_info->brun = wf_false;
    }


    #ifdef CONFIG_ARS_DRIVER_SUPPORT
    if (pnic_info->ars)
    {
        ars_st *pars = pnic_info->ars;
        
        wf_lock_unlock(&pars->ctlLock);
    }
    #endif

    {
       wf_bool is_connected;
       wf_mlme_get_connect(pnic_info, &is_connected);
       if (is_connected)
       {
           wf_mcu_set_user_info(pnic_info, wf_true);
       }
    }

    /* resume tx */
    tx_resume(pnic_info);
    
    /* free scan request infomation */
    PT_WAIT_WHILE(pt, wf_os_api_sema_try(&pscan_info->req_lock));
    wf_msg_del(pmsg_que,
               WF_CONTAINER_OF((void *)pscan_info->preq, wf_msg_t, value));
    pscan_info->preq = NULL;
    wf_os_api_sema_post(&pscan_info->req_lock);
    nic_mlme_hw_access_unlock(pnic_info);

    *prsn = reason;
    if (reason < 0)
    {
        SCAN_WARN("scan fail, error code: %d", reason);
        PT_EXIT(pt);
    }

    PT_END(pt);
}

int wf_scan_start (nic_info_st *pnic_info, scan_type_e type,
                   wf_80211_bssid_t bssid,
                   wf_wlan_ssid_t ssids[], wf_u8 ssid_num,
                   wf_u8 chs[], wf_u8 ch_num)
{
    wf_scan_info_t *pscan_info;
    wf_msg_que_t *pmsg_que;
    wf_msg_t *pmsg;
    wf_scan_req_t *pscan_req;
    int rst;

    SCAN_DBG();

    if (pnic_info == NULL || WF_CANNOT_RUN(pnic_info))
    {
        return -1;
    }


    if (!pnic_info->is_up)
    {
        return -2;
    }
    pscan_info = pnic_info->scan_info;
    pmsg_que = &pscan_info->msg_que;

    if (1)
    {
        char *bssid_str = wf_kzalloc(20);
        char *ssid_str = wf_kzalloc(WF_SCAN_REQ_SSID_NUM * sizeof(ssids[0]));
        char *ch_str = wf_kzalloc(20 * 3 + 1);
        wf_u8 i;
        if (bssid && wf_80211_is_valid_bssid(bssid))
        {
            sprintf(bssid_str, WF_MAC_FMT, WF_MAC_ARG(bssid));
        }
        else
        {
            sprintf(bssid_str, " ");
        }
        for (i = 0; i < ssid_num; i++)
        {
            strncat(ssid_str, (const char *)&ssids[i].data, ssids[i].length);
        }
        if (i == 0)
        {
            sprintf(ssid_str, " ");
        }
        for (i = 0; i < ch_num; i++)
        {
            char tmp[5];
            sprintf(tmp, "%02d ", chs[i]);
            strncat(ch_str, tmp, 3);
        }
        if (i == 0)
        {
            sprintf(ch_str, " ");
        }
        SCAN_DBG("type(%s) bssid(%s) ssid(%s), channel(%s)",
                 type == SCAN_TYPE_ACTIVE ? "active" : "passive",
                 bssid_str,
                 ssid_str,
                 ch_str);

        wf_kfree(bssid_str);
        wf_kfree(ssid_str);
        wf_kfree(ch_str);
    }
    rst = wf_msg_new(pmsg_que, WF_SCAN_TAG_START, &pmsg);
    if (rst)
    {
        SCAN_WARN("msg new fail error code: %d", rst);
        return -3;
    }
    pmsg->len = sizeof(wf_scan_req_t);
    pscan_req = (wf_scan_req_t *)pmsg->value;

    pscan_req->type = type;
    if (bssid && wf_80211_is_valid_bssid(bssid))
    {
        wf_memcpy(pscan_req->bssid, bssid, sizeof(pscan_req->bssid));
    }
    else
    {
        wf_memset(pscan_req->bssid, 0x0, sizeof(pscan_req->bssid));
    }
    pscan_req->ssid_num = WF_MIN(ssid_num, WF_ARRAY_SIZE(pscan_req->ssids));
    if (ssid_num && ssids)
    {
        wf_memset(pscan_req->ssids, '\0', sizeof(pscan_req->ssids));
        wf_memcpy(pscan_req->ssids, ssids,
                  pscan_req->ssid_num * sizeof(pscan_req->ssids[0]));
    }

    pscan_req->ch_num = WF_MIN(ch_num, WF_ARRAY_SIZE(pscan_req->ch_map));
    if (chs && ch_num)
    {
        wf_memcpy(pscan_req->ch_map, chs, ch_num);
    }

    rst = wf_msg_push(pmsg_que, pmsg);
    if (rst)
    {
        wf_msg_del(pmsg_que, pmsg);
        SCAN_WARN("msg push fail, error code: %d", rst);
        return -4;
    }

    return 0;
}

int wf_scan_stop (nic_info_st *pnic_info)
{
    if (pnic_info == NULL || WF_CANNOT_RUN(pnic_info))
    {
        return -1;
    }

    SCAN_DBG();

    if (!wf_is_scanning(pnic_info))
    {
        return 0;
    }

    {
        wf_scan_info_t *pscan_info = pnic_info->scan_info;
        wf_msg_que_t *pmsg_que = &pscan_info->msg_que;
        wf_msg_t *pmsg;
        int rst;

        rst = wf_msg_new(pmsg_que, WF_SCAN_TAG_ABORT, &pmsg);
        if (rst)
        {
            SCAN_WARN("msg new, error code: %d", rst);
            return -2;
        }
        rst = wf_msg_push(pmsg_que, pmsg);
        if (rst)
        {
            wf_msg_del(pmsg_que, pmsg);
            SCAN_WARN("msg push, error code: %d", rst);
            return -3;
        }
    }

    return 0;
}

int wf_scan_wait_done (nic_info_st *pnic_info, wf_bool babort, wf_u16 to_ms)
{
    wf_timer_t timer;

    if (pnic_info == NULL || WF_CANNOT_RUN(pnic_info))
    {
        return -1;
    }

    if (!wf_is_scanning(pnic_info))
    {
        return 0;
    }

    if (babort)
    {
        wf_scan_stop(pnic_info);
    }

    wf_timer_set(&timer, to_ms);
    do
    {
        wf_msleep(1);
        if (wf_timer_expired(&timer))
        {
            return -2;
        }
    }
    while (wf_is_scanning(pnic_info));

    return 0;
}

wf_bool wf_is_scanning (nic_info_st *pnic_info)
{
    wf_scan_info_t *pscan_info;

    if (pnic_info == NULL)
    {
        return -1;
    }
    pscan_info = pnic_info->scan_info;

    return pscan_info->brun;
}

wf_inline static int scan_msg_init (wf_msg_que_t *pmsg_que)
{
    wf_msg_init(pmsg_que);
    return (wf_msg_alloc(pmsg_que, WF_SCAN_TAG_ABORT, 0, 1) ||
            wf_msg_alloc(pmsg_que, WF_SCAN_TAG_START, sizeof(wf_scan_req_t), 1)) ? -1 : 0;
}

wf_inline static void scan_msg_deinit (wf_msg_que_t *pmsg_que)
{
    wf_msg_free(pmsg_que);
}

int wf_scan_init (nic_info_st *pnic_info)
{
    wf_scan_info_t *pscan_info;

    if (pnic_info == NULL)
    {
        return -1;
    }

    SCAN_DBG();

    pscan_info = wf_kzalloc(sizeof(wf_scan_info_t));
    if (pscan_info == NULL)
    {
        SCAN_WARN("malloc scan_param_st failed");
        return -2;
    }
    pnic_info->scan_info = pscan_info;
    pscan_info->brun = wf_false;
    pscan_info->preq = NULL;
    if (scan_msg_init(&pscan_info->msg_que))
    {
        SCAN_WARN("scan msg init failed");
        return -3;
    }
    wf_os_api_sema_init(&pscan_info->req_lock, 1);

    return 0;
}

int wf_scan_term (nic_info_st *pnic_info)
{
    wf_scan_info_t *pscan_info;

    if (pnic_info == NULL)
    {
        return 0;
    }

    SCAN_DBG();

    pscan_info = pnic_info->scan_info;
    if (pscan_info)
    {
        scan_msg_deinit(&pscan_info->msg_que);
        wf_kfree(pscan_info);
        pnic_info->scan_info = NULL;
    }

    return 0;
}

