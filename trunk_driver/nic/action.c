/*
 * action.c
 *
 * used for xmit action frame
 *
 * Author: renhaibo
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

/* macro */
#define ACTION_DBG(fmt, ...)        LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define ACTION_ARRAY(data, len)     log_array(data, len)
#define ACTION_INFO(fmt, ...)       LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define ACTION_WARN(fmt, ...)       LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define ACTION_ERROR(fmt, ...)      LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)

static
wf_u8 get_rx_ampdu_size(nic_info_st *nic_info)
{
    wf_u8 size          = 0;
    hw_info_st *hw_info = nic_info->hw_info;

    switch (hw_info->max_rx_ampdu_factor)
    {
        case MAX_AMPDU_FACTOR_64K:
            size = 64;
            break;
        case MAX_AMPDU_FACTOR_32K:
            size = 32;
            break;
        case MAX_AMPDU_FACTOR_16K:
            size = 16;
            break;
        case MAX_AMPDU_FACTOR_8K:
            size = 8;
            break;
        default:
            size = 64;
            break;
    }

    return size;
}

static
int action_frame_add_ba_response(nic_info_st *nic_info,
                                 wf_add_ba_parm_st *barsp_parm)
{
    int rst;

    barsp_parm->size = get_rx_ampdu_size(nic_info);
    rst = wf_mlme_add_ba_rsp(nic_info, barsp_parm);
    if (rst)
    {
        ACTION_WARN("wf_mlme_add_ba_rsp fail, error code: %d", rst);
        return -1;
    }

    return 0;
}

#ifdef CFG_ENABLE_AP_MODE
static
int action_ba_req_work_ap(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    if (pwdn_info == NULL)
    {
        ACTION_DBG("wdn_info null");
        return -1;
    }

    ACTION_DBG("action ba request received");

    if (wf_ap_msg_load(pnic_info, &pwdn_info->ap_msg,
                       WF_AP_MSG_TAG_BA_REQ_FRAME, NULL, 0))
    {
        ACTION_WARN("action ba msg enque fail");
        return -3;
    }

    return 0;
}

#endif

static
int action_frame_block_ack (nic_info_st *nic_info, wdn_net_info_st *pwdn_info,
                            wf_u8 *pkt, wf_u16 pkt_len)
{
    wf_80211_mgmt_t *pmgmt      = (wf_80211_mgmt_t *)pkt;
    wf_u16 status, tid          = 0;
    wf_u16 reason_code          = 0;
    wf_u8 *frame_body           = NULL;
    struct ADDBA_request *preq  = NULL;
    wf_u8 action                = 0;
    wf_u16 param                = 0;
    mlme_info_t *mlme_info      = nic_info->mlme_info;

    if (pkt_len == 0)
    {
        return -1;
    }

    frame_body = &pmgmt->action.variable[0];
    action = pmgmt->action.action_field;
    if (pwdn_info == NULL)
    {
        return -1;
    }

    switch (action)
    {
        case WF_WLAN_ACTION_ADDBA_REQ:
        {
            if(wf_local_cfg_get_work_mode(nic_info) == WF_INFRA_MODE)
            {
                wf_add_ba_parm_st parm;
                wf_add_ba_parm_st *barsp_parm = &parm;

                frame_body = &pmgmt->action.variable[0];
                preq = ( struct ADDBA_request *)frame_body;
                barsp_parm->dialog = preq->dialog_token;
                param = wf_le16_to_cpu(preq->BA_para_set);
                barsp_parm->param = param;
                barsp_parm->tid = (param & 0x3c) >> 2;
                barsp_parm->policy = (param & 0x2) >> 1;
                barsp_parm->size = (wf_u8)(param & (~0xe03f)) >> 6;

                barsp_parm->timeout = wf_le16_to_cpu(preq->BA_timeout_value);
                barsp_parm->start_seq = wf_le16_to_cpu(preq->ba_starting_seqctrl) >> 4;
                barsp_parm->status = 0;

                ACTION_DBG("WF_WLAN_ACTION_ADDBA_REQ TID:%d dialog:%d size:%d policy:%d start_req:%d timeout:%d",
                           barsp_parm->tid, barsp_parm->dialog, barsp_parm->size, barsp_parm->policy, barsp_parm->start_seq, barsp_parm->timeout);
                action_frame_add_ba_response(nic_info, barsp_parm);
            }
#ifdef CFG_ENABLE_AP_MODE
            else if(wf_local_cfg_get_work_mode(nic_info) == WF_MASTER_MODE)
            {
                action_ba_req_work_ap(nic_info, pwdn_info);
            }
#endif

        }
        break;

        case WF_WLAN_ACTION_ADDBA_RESP:
        {
            status = WF_GET_LE16(&frame_body[1]);
            tid = ((frame_body[3] >> 2) & 0x7);
            if (status == 0)
            {
                pwdn_info->htpriv.agg_enable_bitmap |= 1 << tid;
                pwdn_info->htpriv.candidate_tid_bitmap &= ~(BIT(tid));

                if (frame_body[3] & 1)
                    pwdn_info->htpriv.tx_amsdu_enable = wf_true;
            }
            else
            {
                pwdn_info->htpriv.agg_enable_bitmap &= ~(BIT(tid));
            }

            mlme_info->baCreating = 0;

            ACTION_DBG("WF_WLAN_ACTION_ADDBA_RESP status:%d tid:%d  (agg_enable_bitmap:%d candidate_tid_bitmap:%d)",
                       status, tid, pwdn_info->htpriv.agg_enable_bitmap, pwdn_info->htpriv.candidate_tid_bitmap);

        }
        break;

        case WF_WLAN_ACTION_DELBA:
        {
            if ((frame_body[0] & BIT(3)) == 0)
            {
                tid = (frame_body[0] >> 4) & 0x0F;

                pwdn_info->htpriv.agg_enable_bitmap &= ~(1 << tid);
                pwdn_info->htpriv.candidate_tid_bitmap &= ~(1 << tid);

                reason_code = WF_GET_LE16(&frame_body[2]);
            }
            else if ((frame_body[0] & BIT(3)) == BIT(3))
            {
                tid = (frame_body[0] >> 4) & 0x0F;
            }

            ACTION_DBG("WF_WLAN_ACTION_DELBA reason_code:%d tid:%d", reason_code, tid);
        }
        break;
        default:
            break;
    }

    return 0;
}

static
void action_frame_wlan_hdr (nic_info_st *pnic_info, struct xmit_buf *pxmit_buf)
{
    wf_u8 *pframe                       = NULL;
    struct wl_ieee80211_hdr *pwlanhdr   = NULL;

    if (pnic_info == NULL)
    {
        return;
    }

    //nic_unused_check(pnic_info);
    //ACTION_DBG("[action]%s",__func__);
    pframe = pxmit_buf->pbuf + TXDESC_OFFSET;
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

    pwlanhdr->frame_ctl = 0;
    SetFrameType(pframe, WIFI_MGT_TYPE);
    SetFrameSubType(pframe, WIFI_ACTION);  /* set subtype */

}


wf_s32 proc_on_action_public_vendor_func(nic_info_st *pnic_info, wf_u8 *pkt,wf_u16 pkt_len)
{
    wf_s32 ret = 0;
    wf_u8 *frame_body = pkt + sizeof(struct wl_ieee80211_hdr_3addr);

    if ((pnic_info == NULL) || (pkt_len == 0))
    {
        return -1;
    }

    if (wf_memcmp(frame_body + 2, P2P_OUI, 4) == 0)
    {
        if(wf_p2p_is_valid(pnic_info))
        {
            ret = wf_p2p_proc_action_public(pnic_info, pkt,pkt_len);
        }
    }
    else
    {
        ACTION_DBG("OUI:");
        ACTION_ARRAY(frame_body + 2, 4);
    }

    return ret;
}

wf_s32 wf_action_frame_public(nic_info_st *nic_info, wf_u8 *pdata, wf_u16 pkt_len)
{
    wf_80211_mgmt_t *pmgmt = (wf_80211_mgmt_t *)pdata;
    hw_info_st *hw_info = nic_info->hw_info;
    wf_u8 action;
    wf_u8 category;
    wf_s32 ret = 0;
    ACTION_DBG("[%d] "WF_MAC_FMT,nic_info->ndev_id,WF_MAC_ARG(hw_info->macAddr));

    category = pmgmt->action.action_category;
    action = pmgmt->action.action_field;

    if(wf_memcmp(pmgmt->da,  hw_info->macAddr, WF_ETH_ALEN))
    {
        ACTION_DBG("[%d] "WF_MAC_FMT,nic_info->ndev_id,WF_MAC_ARG(pmgmt->da));
        goto exit;
    }

    ACTION_DBG("[%d] category=%d,action=%d",nic_info->ndev_id,category,action);
    if(category != WF_WLAN_CATEGORY_PUBLIC )
    {
        goto exit;
    }

    if (action == WF_WLAN_ACTION_PUBLIC_VENDOR)
    {
        ret = proc_on_action_public_vendor_func(nic_info, pdata,pkt_len);
    }
exit:
    return ret;

}

wf_s32 wf_action_frame_parse(wf_u8 * frame, wf_u32 frame_len, wf_u8 * category, wf_u8 * action)
{
    wf_u8 *frame_body = frame + sizeof(struct wl_ieee80211_hdr_3addr);
    wf_u16 fc;
    wf_u8 c;
    wf_u8 a = WF_WLAN_ACTION_PUBLIC_MAX;

    fc = wf_le16_to_cpu(((wf_80211_hdr_3addr_t *)frame)->frame_control);

    if ((fc & (WF_80211_FCTL_FTYPE | WF_80211_FCTL_STYPE))
        != (WF_80211_FTYPE_MGMT | WF_80211_STYPE_ACTION))
    {
        return -1;
    }

    c = frame_body[0];
    switch (c)
    {
        case WF_80211_CATEGORY_P2P:
            break;
        default:
            a = frame_body[1];
    }

    if (category)
        *category = c;
    if (action)
        *action = a;

    return 0;
}

static wf_s32 action_frame_p2p_proc(nic_info_st *pnic_info, wf_u8 *pframe, wf_u32 frame_len)
{
    if ((pnic_info == NULL) || (pframe == NULL) || (frame_len == 0))
    {
        return -1;
    }

    if(wf_p2p_is_valid(pnic_info))
    {
        wf_u8 *frame_body   = NULL;
        wf_u8 category      = 0;
        wf_u32 len          = frame_len;
        if (0 != wf_memcmp(nic_to_local_addr(pnic_info), GetAddr1Ptr(pframe), WF_ETH_ALEN))
        {
            return WF_RETURN_OK;
        }

        frame_body =(wf_u8 *)(pframe + sizeof(struct wl_ieee80211_hdr_3addr));

        category = frame_body[0];
        if (category != WF_WLAN_CATEGORY_P2P)
        {
            return WF_RETURN_OK;
        }

        if (wf_cpu_to_be32(*((wf_u32 *) (frame_body + 1))) != P2POUI)
        {
            return WF_RETURN_OK;
        }

        return wf_p2p_proc_rx_action(pnic_info,pframe,len);
    }

    return WF_RETURN_OK;

}

int wf_action_frame_process (nic_info_st *nic_info, wdn_net_info_st *pwdn_info,
                             wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    mlme_info_t *mlme_info  = (mlme_info_t *)nic_info->mlme_info;
    wf_u8 category          = 0;

    category = pmgmt->action.action_category;

    if ((mlme_info->state & 0x03) != WIFI_FW_AP_STATE)
    {
        if (mlme_info->connect == wf_false &&  nic_info->p2p == NULL)
            return 0;
    }

    switch (category)
    {
        case WF_WLAN_CATEGORY_BACK:
        {
            hw_info_st *hw_info = nic_info->hw_info;
            if (hw_info->ba_enable == wf_true)
            {
                action_frame_block_ack(nic_info, pwdn_info, (wf_u8 *)pmgmt, mgmt_len);
            }
        }
        break;
        case WF_WLAN_CATEGORY_SPECTRUM_MGMT:
        {
            //wf_action_frame_spectrum(nic_info, pkt, pkt_len);
        }
        break;
        case WF_WLAN_CATEGORY_PUBLIC:
        {
            wf_action_frame_public(nic_info,(wf_u8 *)pmgmt,mgmt_len);
        }
        break;
        case WF_WLAN_CATEGORY_HT:
        {
            //wf_action_frame_ht(nic_info,pkt,pkt_len);
        }
        break;
        case WF_WLAN_CATEGORY_SA_QUERY:
        {

        }
        break;
        case WF_WLAN_CATEGORY_P2P:
        {
            action_frame_p2p_proc(nic_info,(wf_u8 *)pmgmt,mgmt_len);
        }
        break;
        default:
        {

        }
        break;
    }

    return 0;

}

int wf_action_frame_ba_to_issue (nic_info_st *nic_info, wf_u8 action)
{
    int rst                             = 0;
    wf_u8 *pframe                       = NULL;
    wf_u16 ba_para_set                  = 0;
    wf_u16 ba_timeout_value             = 0;
    wf_u16 ba_starting_seqctrl          = 0;
    wf_u16 start_seq                    = 0;
    struct wl_ieee80211_hdr *pwlanhdr   = NULL;
    struct xmit_buf *pxmit_buf          = NULL;
    wf_u16 pkt_len                      = 0;
    tx_info_st  *ptx_info               = NULL;
    wdn_net_info_st *pwdn_info          = NULL;
    mlme_info_t *mlme_info              = NULL;
    wf_u8 initiator                     = 0;
    wf_u8 category                      = WF_WLAN_CATEGORY_BACK;
    wf_add_ba_parm_st *barsp_info       = NULL;
    wf_add_ba_parm_st *bareq_info       = NULL;

    pwdn_info = wf_wdn_find_info(nic_info, wf_wlan_get_cur_bssid(nic_info));
    if (pwdn_info == NULL)
    {
        return -1;
    }

    ptx_info =  (tx_info_st *)nic_info->tx_info;
    mlme_info = (mlme_info_t *)nic_info->mlme_info;
    barsp_info = &mlme_info->barsp_parm;
    bareq_info = &mlme_info->bareq_parm;

    /* alloc xmit_buf */
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if (pxmit_buf == NULL)
    {
        ACTION_ERROR("pxmit_buf is NULL");
        return -1;
    }
    wf_memset(pxmit_buf->pbuf, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

    action_frame_wlan_hdr(nic_info, pxmit_buf);

    pframe = pxmit_buf->pbuf + TXDESC_OFFSET;
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

    wf_memcpy(pwlanhdr->addr1, pwdn_info->mac, MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr2, nic_to_local_addr(nic_info), MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr3, pwdn_info->bssid, MAC_ADDR_LEN);

    pkt_len = sizeof(struct wl_ieee80211_hdr_3addr);
    pframe += pkt_len;

    pframe = set_fixed_ie(pframe, 1, &(category), &pkt_len);
    pframe = set_fixed_ie(pframe, 1, &(action), &pkt_len);

    switch (action)
    {

        case WF_WLAN_ACTION_ADDBA_RESP:
        {
            pframe = set_fixed_ie(pframe, 1, &(barsp_info->dialog), &pkt_len);
            pframe = set_fixed_ie(pframe, 2, (wf_u8 *) & (barsp_info->status), &pkt_len);

            ba_para_set = barsp_info->param;
            ba_para_set &= ~IEEE80211_ADDBA_PARAM_TID_MASK;
            ba_para_set |= (barsp_info->tid << 2) & IEEE80211_ADDBA_PARAM_TID_MASK;
            ba_para_set &= ~IEEE80211_ADDBA_PARAM_BUF_SIZE_MASK;
            ba_para_set |= (barsp_info->size << 6) & IEEE80211_ADDBA_PARAM_BUF_SIZE_MASK;
            ba_para_set &= ~(BIT(0));
            ba_para_set = wf_cpu_to_le16(ba_para_set);

            pframe = set_fixed_ie(pframe, 2, (wf_u8 *)(&(ba_para_set)), &pkt_len);
            pframe = set_fixed_ie(pframe, 2, (wf_u8 *)(&(barsp_info->timeout)), &pkt_len);

            ACTION_INFO("tid:%d dialog:%d  ba_para_set:0x%x  timeout:%d  status:%d",
                        barsp_info->tid, barsp_info->dialog, ba_para_set,
                        barsp_info->timeout, barsp_info->status);
        }
        break;

        case WF_WLAN_ACTION_ADDBA_REQ:
        {
            wf_u8 dialog;

            mlme_info->baCreating = 1;

            dialog = pwdn_info->dialogToken[bareq_info->tid] + 1;
            if (dialog > 7)
            {
                dialog = 1;
            }

            pwdn_info->dialogToken[bareq_info->tid] = dialog;
            pframe = set_fixed_ie(pframe, 1, &(dialog), &pkt_len);

            ba_para_set = (0x1002 | ((bareq_info->tid & 0xf) << 2));
            ba_para_set = wf_cpu_to_le16(ba_para_set);
            pframe = set_fixed_ie(pframe, 2, (wf_u8 *)(&(ba_para_set)), &pkt_len);

            ba_timeout_value = 5000;
            ba_timeout_value = wf_cpu_to_le16(ba_timeout_value);
            pframe = set_fixed_ie(pframe, 2, (wf_u8 *)(&(ba_timeout_value)), &pkt_len);

            if (pwdn_info != NULL)
            {
                start_seq = (pwdn_info->wdn_xmitpriv.txseq_tid[bareq_info->tid] & 0xfff) + 1;

                pwdn_info->ba_starting_seqctrl[bareq_info->tid] = start_seq;
                ba_starting_seqctrl = start_seq << 4;
            }

            ba_starting_seqctrl = wf_cpu_to_le16(ba_starting_seqctrl);
            pframe = set_fixed_ie(pframe, 2, (unsigned char *)(&(ba_starting_seqctrl)), &pkt_len);
            ACTION_INFO("[action request] TID:%d  dialog:%d  ba_para_set:0x%x  start_req:%d", bareq_info->tid, dialog, ba_para_set, start_seq);
        }
        break;

        case WF_WLAN_ACTION_DELBA:
            ba_para_set = 0;
            ba_para_set |= (barsp_info->tid << 12) & IEEE80211_DELBA_PARAM_TID_MASK;
            ba_para_set |= (initiator << 11) & IEEE80211_DELBA_PARAM_INITIATOR_MASK;

            ba_para_set = wf_cpu_to_le16(ba_para_set);
            pframe = set_fixed_ie(pframe, 2, (unsigned char *)(&(ba_para_set)), &pkt_len);
            barsp_info->status = wf_cpu_to_le16(barsp_info->status);
            pframe = set_fixed_ie(pframe, 2, (unsigned char *)(&(barsp_info->status)), &pkt_len);

            ACTION_DBG("[action delete] reason:%d  ba_para_set:0x%x", barsp_info->status, ba_para_set);
            break;
        default:
            break;

    }
    pxmit_buf->pkt_len = pkt_len;

    //rst = wf_nic_mgmt_frame_xmit_with_ack(nic_info, pwdn_info, pxmit_buf, pxmit_buf->pkt_len);
    rst = wf_nic_mgmt_frame_xmit(nic_info, pwdn_info, pxmit_buf,pxmit_buf->pkt_len);

    return rst;
}

int wf_action_frame_add_ba_request(nic_info_st *nic_info, struct xmit_frame *pxmitframe)
{
    wf_u8 issued                = 0;
    mlme_info_t *mlme_info      = NULL;
    wdn_net_info_st *pwdn_info  = NULL;

    if (pxmitframe->bmcast)
    {
        return -1;
    }

    mlme_info = (mlme_info_t *)nic_info->mlme_info;

    pwdn_info = pxmitframe->pwdn;
    if (pwdn_info == NULL)
    {
        return -1;
    }

    if (pwdn_info->ba_enable_flag[pxmitframe->priority] == wf_true)
    {
        return -1;
    }

    if ((pwdn_info->htpriv.ht_option == wf_true) && (pwdn_info->htpriv.ampdu_enable == wf_true))
    {
        issued = (pwdn_info->htpriv.agg_enable_bitmap >> pxmitframe->priority) & 0x1;
        issued |= (pwdn_info->htpriv.candidate_tid_bitmap >> pxmitframe->priority) & 0x1;
        if(issued == 0)
        {
            if ((pxmitframe->frame_tag == DATA_FRAMETAG) && (pxmitframe->ether_type != 0x0806) &&
                (pxmitframe->ether_type != 0x888e) && (pxmitframe->dhcp_pkt != 1))
            {
                pwdn_info->htpriv.candidate_tid_bitmap |= WF_BIT(pxmitframe->priority);
                mlme_info->bareq_parm.tid = pxmitframe->priority;
                pwdn_info->ba_enable_flag[pxmitframe->priority] = wf_true;
                wf_mlme_add_ba_req(nic_info);
                return 0;
            }
        }
    }

    return -1;
}

int wf_action_frame_del_ba_request(nic_info_st *nic_info)
{
    wdn_net_info_st *wdn_net_info   = NULL;
    mlme_info_t *mlme_info          = (mlme_info_t *)nic_info->mlme_info;

    wdn_net_info  = wf_wdn_find_info(nic_info, wf_wlan_get_cur_bssid(nic_info));
    if (NULL != wdn_net_info)
    {
        if (wf_false == wf_wdn_is_alive(wdn_net_info, 1))
        {
            unsigned int tid;
            for (tid = 0; tid < TID_NUM; tid++)
            {
                if (wdn_net_info->ba_started_flag[tid] == 1)
                {
                    mlme_info->barsp_parm.tid = tid;
                    wf_action_frame_ba_to_issue(nic_info, WF_WLAN_ACTION_DELBA);

                    wdn_net_info->ba_started_flag[tid] = wf_false;
                    wdn_net_info->ba_enable_flag[tid]  = wf_false;
                }
            }
            wdn_net_info->htpriv.agg_enable_bitmap = 0;
            wdn_net_info->htpriv.candidate_tid_bitmap = 0;
        }
    }

    return 0;
}

wf_u8 *wf_action_public_to_str(wf_u8 action)
{
    switch(action)
    {
        case WF_WLAN_ACTION_PUBLIC_BSSCOEXIST           :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_BSSCOEXIST);
        }
        case WF_WLAN_ACTION_PUBLIC_DSE_ENABLE           :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_DSE_ENABLE);
        }
        case WF_WLAN_ACTION_PUBLIC_DSE_DEENABLE         :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_DSE_DEENABLE);
        }
        case WF_WLAN_ACTION_PUBLIC_DSE_REG_LOCATION     :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_DSE_REG_LOCATION);
        }
        case WF_WLAN_ACTION_PUBLIC_EXT_CHL_SWITCH       :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_EXT_CHL_SWITCH);
        }
        case WF_WLAN_ACTION_PUBLIC_DSE_MSR_REQ          :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_DSE_MSR_REQ);
        }
        case WF_WLAN_ACTION_PUBLIC_DSE_MSR_RPRT         :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_DSE_MSR_RPRT);
        }
        case WF_WLAN_ACTION_PUBLIC_MP                   :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_MP);
        }
        case WF_WLAN_ACTION_PUBLIC_DSE_PWR_CONSTRAINT   :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_DSE_PWR_CONSTRAINT);
        }
        case WF_WLAN_ACTION_PUBLIC_VENDOR               :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_VENDOR);
        }
        case WF_WLAN_ACTION_PUBLIC_GAS_INITIAL_REQ      :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_GAS_INITIAL_REQ);
        }
        case WF_WLAN_ACTION_PUBLIC_GAS_INITIAL_RSP      :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_GAS_INITIAL_RSP);
        }
        case WF_WLAN_ACTION_PUBLIC_GAS_COMEBACK_REQ     :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_GAS_COMEBACK_REQ);
        }
        case WF_WLAN_ACTION_PUBLIC_GAS_COMEBACK_RSP     :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_GAS_COMEBACK_RSP);
        }
        case WF_WLAN_ACTION_PUBLIC_TDLS_DISCOVERY_RSP   :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_TDLS_DISCOVERY_RSP);
        }
        case WF_WLAN_ACTION_PUBLIC_LOCATION_TRACK       :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_LOCATION_TRACK);
        }
        case WF_WLAN_ACTION_PUBLIC_MAX                  :
        {
            return to_str(WF_WLAN_ACTION_PUBLIC_MAX);
        }
        default:
        {
            return "Unknown-action-public";
        }
    }
}

