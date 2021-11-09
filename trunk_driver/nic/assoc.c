/*
 * assoc.c
 *
 * impliment of IEEE80211 management frame association stage processing
 *
 * Author: luozhi
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
#define ASSOC_DBG(fmt, ...)         LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define ASSOC_ARRAY(data, len)      log_array(data, len)
#define ASSOC_INFO(fmt, ...)        LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define ASSOC_WARN(fmt, ...)        LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define ASSOC_ERROR(fmt, ...)       LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)

#define ASSOC_REQ_RESEND_TIMES      3
#define ASSOC_RSP_TIMEOUT           50

/* function declaration */

#ifdef CFG_ENABLE_AP_MODE
static void disassoc_wlan_hdr (nic_info_st *pnic_info, struct xmit_buf *pxmit_buf)
{
    wf_u8 *pframe;
    struct wl_ieee80211_hdr *pwlanhdr;

    pframe = pxmit_buf->pbuf + TXDESC_OFFSET;
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

    pwlanhdr->frame_ctl = 0;
    SetFrameType(pframe, WIFI_MGT_TYPE);
    SetFrameSubType(pframe, WIFI_DISASSOC);  /* set subtype */
}

static int disassoc_xmit_frame (nic_info_st *pnic_info,
                                wf_u8 *pmac, wf_u16 reason_code)
{
    wf_u8 *pframe;
    struct wl_ieee80211_hdr *pwlanhdr;
    struct xmit_buf *pxmit_buf;
    wf_u16 pkt_len;
    tx_info_st      *ptx_info;
    wdn_net_info_st *pwdn_info;

    ptx_info = (tx_info_st *)pnic_info->tx_info;

    pwdn_info = wf_wdn_find_info(pnic_info, pmac);
    if (pwdn_info == NULL)
    {
        ASSOC_ERROR("wdn is NULL");
        return -1;
    }

    /* alloc xmit_buf */
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if (pxmit_buf == NULL)
    {
        ASSOC_ERROR("pxmit_buf is NULL");
        return -1;
    }
    wf_memset(pxmit_buf->pbuf, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

    /* type of management is 1010 */
    disassoc_wlan_hdr(pnic_info, pxmit_buf);

    /* set txd at tx module */
    pframe = pxmit_buf->pbuf + TXDESC_OFFSET; /* pframe point to wlan_hdr */
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

    pkt_len = sizeof(struct wl_ieee80211_hdr_3addr);
    pframe += pkt_len; /* point to iv or frame body */

    wf_memcpy(pwlanhdr->addr1, pwdn_info->mac, MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr2, nic_to_local_addr(pnic_info), MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr3, wf_wlan_get_cur_bssid(pnic_info), MAC_ADDR_LEN);
    /*1.add reason_code*/
    pframe = set_fixed_ie(pframe, 2, (wf_u8*)&reason_code, &pkt_len);

    pxmit_buf->pkt_len = pkt_len;
    wf_nic_mgmt_frame_xmit(pnic_info, pwdn_info, pxmit_buf, pxmit_buf->pkt_len);

    return 0;
}

static int disassoc_work_ap (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                             wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    if (pwdn_info == NULL)
    {
        ASSOC_DBG("wdn_info null");
        return -1;
    }

    if (mgmt_len > WF_80211_MGMT_DISASSOC_SIZE_MAX)
    {
        ASSOC_ERROR("deauth frame length too long");
        return -3;
    }

    if (wf_80211_get_frame_type(pmgmt->frame_control) != WF_80211_FRM_DISASSOC)
    {
        ASSOC_WARN("disassoc frame type error");
        return -4;
    }

    ASSOC_DBG("disassoc received");

    if (wf_ap_msg_load(pnic_info, &pwdn_info->ap_msg,
                       WF_AP_MSG_TAG_DISASSOC_FRAME, pmgmt, mgmt_len))
    {
        ASSOC_WARN("disassoc msg enque fail");
        return -5;
    }

    return 0;
}

static
int assoc_ap_xmit_frame (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                         wf_80211_frame_e type, wf_80211_statuscode_e status)
{
    tx_info_st *ptx_info = pnic_info->tx_info;
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    struct xmit_buf *pxmit_buf;
    wf_80211_mgmt_t *passoc_frame;
    wf_u16 tmp_16;
    wf_u8 *pvar;
    wf_u16 var_len = 0;
    wf_80211_mgmt_ie_t *pie;

    /* alloc xmit_buf */
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if (pxmit_buf == NULL)
    {
        ASSOC_ERROR("xmit_buf alloc fail");
        return -1;
    }
    wf_memset(pxmit_buf->pbuf, 0,
              TXDESC_OFFSET + WF_OFFSETOF(wf_80211_mgmt_t, assoc_resp));

    /* set frame type */
    passoc_frame = (void *)&pxmit_buf->pbuf[TXDESC_OFFSET];
    if (type == WF_80211_FRM_ASSOC_REQ)
        type = WF_80211_FRM_ASSOC_RESP;
    else if (type == WF_80211_FRM_REASSOC_REQ)
        type = WF_80211_FRM_REASSOC_RESP;
    else
        ASSOC_WARN("invalid frame type");
    wf_80211_set_frame_type(&passoc_frame->frame_control, type);
    passoc_frame->frame_control = wf_cpu_to_le16(passoc_frame->frame_control);

    /* set mac address */
    wf_memcpy(passoc_frame->da, pwdn_info->mac, sizeof(passoc_frame->da));
    wf_memcpy(passoc_frame->sa, pcur_network->mac_addr, sizeof(passoc_frame->da));
    wf_memcpy(passoc_frame->bssid, pcur_network->bssid, sizeof(passoc_frame->bssid));

    /*
     * set fiexd fields
     */

    /* set capability */
    passoc_frame->assoc_resp.capab_info = wf_cpu_to_le16(pcur_network->cap_info);

    /* set status code */
    passoc_frame->assoc_resp.status_code = wf_cpu_to_le16(status);

    /* set AID */
    tmp_16 = pwdn_info->aid | BIT(14) | BIT (15);
    passoc_frame->assoc_resp.aid = wf_cpu_to_le16(tmp_16);

    /*
     * set variable fields
     */
    pvar = passoc_frame->assoc_resp.variable;

    /* set support rate */
    pie = (void *)&pvar[var_len];
    pie->element_id = WF_80211_MGMT_EID_SUPP_RATES;
    pie->len = NDIS_802_11_LENGTH_RATES;
    wf_memcpy(pie->data, pwdn_info->datarate, pie->len);
    var_len += WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;

    /* set extend support rate */
    if (pcur_network->rate_len > NDIS_802_11_LENGTH_RATES)
    {
        pie = (void *)&pvar[var_len];
        pie->element_id = WF_80211_MGMT_EID_EXT_SUPP_RATES;
        pie->len = pcur_network->rate_len - NDIS_802_11_LENGTH_RATES;
        wf_memcpy(pie->data, pwdn_info->ext_datarate, pie->len);
        var_len += WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
    }

    /* set ht capability */
    if (pwdn_info->ht_enable)
    {
        pie = (void *)&pvar[var_len];
        pie->element_id = WF_80211_MGMT_EID_HT_CAPABILITY;
        pie->len = sizeof(wf_80211_mgmt_ht_cap_t);

        wf_memcpy(pie->data, &pcur_network->pht_cap, pie->len);
        var_len += WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;


        pie = (void *)&pvar[var_len];
        pie->element_id = WF_80211_MGMT_EID_HT_OPERATION;
        pie->len = sizeof(wf_80211_mgmt_ht_operation_t);
        wf_memcpy(pie->data, &pcur_network->pht_oper, pie->len);
        var_len += WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
    }

    if(pwdn_info->wpa_enable || pwdn_info->rsn_enable)
    {
        ASSOC_DBG("wpa_enable");
        wf_memcpy(&pvar[var_len], &pwdn_info->wpa_ie, pwdn_info->wpa_ie_len);
        var_len += pwdn_info->wpa_ie_len;
    }
    /* set wmm */
//    if (pwdn_info->wmm_enable)
//    {
//        ASSOC_DBG("wmm_enable");
//        pvar[var_len++] = WF_80211_MGMT_EID_VENDOR_SPECIFIC;
//        pvar[var_len++] = sizeof(wf_80211_wmm_param_ie_t);
//        wf_memcpy(&pvar[var_len], &pwdn_info->wmm_info,
//                  sizeof(wf_80211_wmm_param_ie_t)-2);
//        var_len += sizeof(wf_80211_wmm_param_ie_t);
//    }

    /*p2p*/
    if(wf_p2p_is_valid(pnic_info))
    {
        mlme_info_t *pmlme_info = (mlme_info_t *)pnic_info->mlme_info;

        if(pmlme_info->wps_assoc_resp_ie && pmlme_info->wps_assoc_resp_ie_len > 0 )
        {
            pie = (void *)&pvar[var_len];
            wf_memcpy(pie, pmlme_info->wps_assoc_resp_ie, pmlme_info->wps_assoc_resp_ie_len);
            var_len += pmlme_info->wps_assoc_resp_ie_len;
            ASSOC_DBG("assoc rsp set wps ie\n");
        }

        wf_p2p_fill_assoc_rsp(pnic_info,&pvar[var_len],&var_len,WF_P2P_IE_ASSOC_REQ);
    }

    /* dump frame data */
    pxmit_buf->pkt_len = WF_OFFSETOF(wf_80211_mgmt_t, assoc_resp.variable) + var_len;
    wf_nic_mgmt_frame_xmit(pnic_info, pwdn_info, pxmit_buf,pxmit_buf->pkt_len);

    return 0;
}

void wf_ap_add_sta_ratid (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    ASSOC_DBG();

    if (pwdn_info->aid < 32)
    {
        pwdn_info->raid = wf_wdn_get_raid_by_network_type(pwdn_info);
        wf_mcu_rate_table_update(pnic_info, pwdn_info);
    }
    else
    {
        ASSOC_ERROR("aid exceed the max number");
    }
}

static
void status_error (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                   wf_ap_msg_t *pmsg, wf_80211_frame_e frame_type,
                   wf_80211_statuscode_e status_code)
{
    /* free message */
    if (pmsg)
    {
        wf_ap_msg_free(pnic_info, &pwdn_info->ap_msg, pmsg);
    }
    /* send respond with error code */
    assoc_ap_xmit_frame(pnic_info, pwdn_info, frame_type, status_code);
}

void wf_assoc_ap_event_up (nic_info_st *nic_info, wdn_net_info_st *pwdn_info,
                           wf_ap_msg_t *pmsg)
{
    ASSOC_DBG();

    if(pwdn_info == NULL)
    {
        ASSOC_ERROR("wdn_info null");
        return;
    }

    if(pwdn_info->aid > 2007)
    {
        ASSOC_ERROR("aid(%d) error", pwdn_info->aid);
        return;
    }
    else
    {
        ASSOC_DBG("aid: %d", pwdn_info->aid);
    }

    wf_os_api_ap_ind_assoc(nic_info, pwdn_info, pmsg, WF_MLME_FRAMEWORK_NETLINK);
}

wf_pt_ret_t wf_assoc_ap_thrd (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_pt_t *pt = &pwdn_info->sub_thrd_pt;
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_ap_msg_t *pmsg;
    wf_80211_mgmt_t *pmgmt;
    wf_u16 var_len, ofs;
    wf_u8 *pvar;
    wf_80211_mgmt_ie_t *pie;
    wf_u32 wpa_multicast_cipher;
    wf_u32 wpa_unicast_cipher;
    wf_u32 rsn_group_cipher;
    wf_u32 rsn_pairwise_cipher;
    wf_u8 i, j;
    int ret;
    wf_80211_frame_e frame_type;
    wf_80211_statuscode_e status_code = WF_80211_STATUS_SUCCESS;
    wf_u8 wpa_ie_len;

    PT_BEGIN(pt);

    /* after auth success, wait receive a assoc request */
    pmsg = wf_ap_msg_get(&pwdn_info->ap_msg);
    if (pmsg == NULL)
    {
        ASSOC_ERROR("no assocation frame received");
        /* abort thread */
        PT_EXIT(pt);
    }
    if (pmsg->tag != WF_AP_MSG_TAG_ASSOC_REQ_FRAME)
    {
        wf_ap_msg_free(pnic_info, &pwdn_info->ap_msg, pmsg);
        ASSOC_DBG("no assocation frame received");
        disassoc_xmit_frame(pnic_info, pwdn_info->mac,
                            WF_80211_REASON_DISASSOC_DUE_TO_INACTIVITY);
        /* abort thread */
        PT_EXIT(pt);
    }

    ASSOC_DBG("assoc begin->"WF_MAC_FMT, WF_MAC_ARG(pwdn_info->mac));
    pwdn_info->state = E_WDN_AP_STATE_ASSOC;

    /* parse assocation frame */
    ASSOC_DBG("assoc arrived");
    ASSOC_ARRAY((void *)&pmsg->mgmt, pmsg->len);

    /* retrive frame type */
    frame_type = wf_80211_get_frame_type(pmsg->mgmt.frame_control);

    /* retrive element fiexd fields */
    pmgmt = &pmsg->mgmt;
    pwdn_info->cap_info = wf_le16_to_cpu(pmgmt->assoc_req.capab_info);
    pwdn_info->listen_interval = wf_le16_to_cpu(pmgmt->assoc_req.listen_interval);

    /* initilize */
    pwdn_info->rsn_pairwise_cipher = 0;
    pwdn_info->wmm_enable = wf_false;
    pwdn_info->wpa_enable = wf_false;
    pwdn_info->rsn_enable = wf_false;
    pwdn_info->datarate_len = 0;
    pwdn_info->ext_datarate_len = 0;
    pwdn_info->ssid_len = 0;
    wf_memset(pwdn_info->wpa_ie, 0x0, sizeof(pwdn_info->wpa_ie));

    /* element variable fields */
    if (frame_type == WF_80211_FRM_ASSOC_REQ)
    {
        pvar = pmsg->mgmt.assoc_req.variable;
        var_len = pmsg->len - WF_OFFSETOF(wf_80211_mgmt_t, assoc_req.variable);
    }
    else
    {
        pvar = pmsg->mgmt.reassoc_req.variable;
        var_len = pmsg->len - WF_OFFSETOF(wf_80211_mgmt_t, reassoc_req.variable);
    }

    for (ofs = 0; ofs < var_len;
         ofs += WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len)
    {
        pie = (void *)&pvar[ofs];
        if (pie->element_id == WF_80211_MGMT_EID_SSID)
        {
            if (pie->len == pcur_network->ssid.length &&
                !wf_memcmp(pie->data, pcur_network->ssid.data, pie->len))
            {
                pwdn_info->ssid_len = pcur_network->ssid.length;
                wf_memcpy(pwdn_info->ssid, pcur_network->ssid.data,
                          pcur_network->ssid.length);
                pwdn_info->ssid[pwdn_info->ssid_len] = '\0';
            }
            else
            {
                ASSOC_WARN("ssid error(len=%d): ", pie->len);
                if (pie->len)
                {
                    ASSOC_ARRAY(pie->data, pie->len);
                }
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_INVALID_IE);
                /* abort thread */
                PT_EXIT(pt);
            }
        }

        else if (pie->element_id == WF_80211_MGMT_EID_SUPP_RATES)
        {
            if (pie->len > WF_ARRAY_SIZE(pwdn_info->datarate))
            {
                ASSOC_ERROR("rates(%d) list size over limite", pie->len);
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_INVALID_IE);
                /* abort thread */
                PT_EXIT(pt);
            }
            /* check and retrieve rate */
            wf_memset(pwdn_info->datarate, 0x0, sizeof(pwdn_info->datarate));
            for (i = 0; i < pie->len; i++)
            {
                for (j = 0; j < WF_ARRAY_SIZE(pcur_network->rate); j++)
                {
                    if (pcur_network->rate[j] == 0x0)
                    {
                        break;
                    }
                    if ((pie->data[i] & (~IEEE80211_BASIC_RATE_MASK)) ==
                        (pcur_network->rate[j] & (~IEEE80211_BASIC_RATE_MASK)))
                    {
                        if (pwdn_info->datarate_len <
                            WF_ARRAY_SIZE(pwdn_info->datarate))
                        {
                            pwdn_info->datarate[pwdn_info->datarate_len++] =
                                pie->data[i];
                            break;
                        }
                        else
                        {
                            ASSOC_ERROR("beyond support rate upper limit");
                            status_error(pnic_info, pwdn_info,
                                         pmsg, frame_type,
                                         WF_80211_STATUS_ASSOC_DENIED_RATES);
                            /* abort thread */
                            PT_EXIT(pt);
                        }
                    }
                }
            }
            if (pwdn_info->datarate_len == 0)
            {
                ASSOC_ERROR("invalid support extend rates");
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_ASSOC_DENIED_RATES);
                /* abort thread */
                PT_EXIT(pt);
            }
            ASSOC_DBG("data rate(Mbps): %d, %d, %d, %d, %d, %d, %d, %d",
                      (pwdn_info->datarate[0] & 0x7F) / 2,
                      (pwdn_info->datarate[1] & 0x7F) / 2,
                      (pwdn_info->datarate[2] & 0x7F) / 2,
                      (pwdn_info->datarate[3] & 0x7F) / 2,
                      (pwdn_info->datarate[4] & 0x7F) / 2,
                      (pwdn_info->datarate[5] & 0x7F) / 2,
                      (pwdn_info->datarate[6] & 0x7F) / 2,
                      (pwdn_info->datarate[7] & 0x7F) / 2);

            /* get network type */
            if ((only_cckrates(pwdn_info->datarate, pwdn_info->datarate_len)) == 1)
            {
                pwdn_info->network_type |= WIRELESS_11B;
            }
            else if ((have_cckrates(pwdn_info->datarate, pwdn_info->datarate_len)) == 1)
            {
                pwdn_info->network_type |= WIRELESS_11BG;
            }
            else
            {
                pwdn_info->network_type |= WIRELESS_11G;
            }
        }

        else if (pie->element_id == WF_80211_MGMT_EID_EXT_SUPP_RATES)
        {
            if (pie->len > WF_ARRAY_SIZE(pwdn_info->ext_datarate))
            {
                ASSOC_ERROR("support extend rates(%d) list size over limite", pie->len);
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_ASSOC_DENIED_RATES);
                /* abort thread */
                PT_EXIT(pt);
            }
            /* check and retrieve rate */
            wf_memset(pwdn_info->ext_datarate, 0x0,
                      WF_ARRAY_SIZE(pwdn_info->ext_datarate));
            for (i = 0; i < pie->len; i++)
            {
                for (j = 0; j < WF_ARRAY_SIZE(pcur_network->rate); j++)
                {
                    if (pcur_network->rate[j] == 0x0)
                    {
                        break;
                    }
                    if ((pie->data[i] & (~IEEE80211_BASIC_RATE_MASK)) ==
                        (pcur_network->rate[j] & (~IEEE80211_BASIC_RATE_MASK)))
                    {
                        if (pwdn_info->ext_datarate_len <
                            WF_ARRAY_SIZE(pwdn_info->ext_datarate))
                        {
                            pwdn_info->ext_datarate[pwdn_info->ext_datarate_len++] =
                                pie->data[i];
                            break;
                        }
                        else
                        {
                            ASSOC_ERROR("beyond support rate upper limit");
                            status_error(pnic_info, pwdn_info,
                                         pmsg, frame_type,
                                         WF_80211_STATUS_ASSOC_DENIED_RATES);
                            /* abort thread */
                            PT_EXIT(pt);
                        }
                    }
                }
            }
            /* no find support ext rate */
            if (pwdn_info->ext_datarate_len == 0)
            {
                ASSOC_ERROR("invalid support extend rates");
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_ASSOC_DENIED_RATES);
                /* abort thread */
                PT_EXIT(pt);
            }
            ASSOC_DBG("extend data rate(Mbps): %d, %d, %d, %d",
                      (pwdn_info->ext_datarate[0] & 0x7F) / 2,
                      (pwdn_info->ext_datarate[1] & 0x7F) / 2,
                      (pwdn_info->ext_datarate[2] & 0x7F) / 2,
                      (pwdn_info->ext_datarate[3] & 0x7F) / 2);

            /* get network type */
            if ((only_cckrates(pwdn_info->ext_datarate, pwdn_info->ext_datarate_len)) == 1)
            {
                pwdn_info->network_type |= WIRELESS_11B;
            }
            else if ((have_cckrates(pwdn_info->ext_datarate, pwdn_info->ext_datarate_len)) == 1)
            {
                pwdn_info->network_type |= WIRELESS_11BG;
            }
            else
            {
                pwdn_info->network_type |= WIRELESS_11G;
            }
        }

        else if (pie->element_id == WF_80211_MGMT_EID_RSN)
        {
            wpa_ie_len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
            if (!wf_80211_mgmt_rsn_parse(pie, wpa_ie_len,
                                         &rsn_group_cipher, &rsn_pairwise_cipher))
            {
                /* checkout group cipher */
                if (rsn_group_cipher != psec_info->rsn_group_cipher)
                {
                    ASSOC_WARN("RSN group cipher error");
                    status_error(pnic_info, pwdn_info,
                                 pmsg, frame_type,
                                 WF_80211_STATUS_INVALID_GROUP_CIPHER);
                    /* abort thread */
                    PT_EXIT(pt);
                }
                /* checkout pairwise cipher */
                pwdn_info->rsn_pairwise_cipher =
                    rsn_pairwise_cipher & psec_info->rsn_pairwise_cipher;
                if (!pwdn_info->rsn_pairwise_cipher)
                {
                    ASSOC_WARN("RSN pairwise cipher error");
                    status_error(pnic_info, pwdn_info,
                                 pmsg, frame_type,
                                 WF_80211_STATUS_INVALID_PAIRWISE_CIPHER);
                    /* abort thread */
                    PT_EXIT(pt);
                }
                /* checkout rsn ie */
                wpa_ie_len =
                    WF_MIN(WF_ARRAY_SIZE(pwdn_info->wpa_ie), wpa_ie_len);
                wf_memcpy(pwdn_info->wpa_ie, pie, wpa_ie_len);
                pwdn_info->wpa_ie_len = wpa_ie_len;
                pwdn_info->rsn_enable = wf_true;
            }
            else
            {
                ASSOC_WARN("RSN element error");
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_INVALID_IE);
                /* abort thread */
                PT_EXIT(pt);
            }
        }

        else if (pie->element_id == WF_80211_MGMT_EID_VENDOR_SPECIFIC)
        {
            /* wpa parse */
            wpa_ie_len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
            ret = wf_80211_mgmt_wpa_parse(pie, wpa_ie_len,
                                          &wpa_multicast_cipher, &wpa_unicast_cipher);
            if (ret < 0 && ret >= -3)
            {
                ASSOC_WARN("vendor ie corrupt");
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_INVALID_IE);
                /* abort thread */
                PT_EXIT(pt);
            }
            if (!ret)
            {
                ASSOC_DBG("WPA element");
                /* checkout group cipher */
                if (wpa_multicast_cipher != psec_info->wpa_multicast_cipher)
                {
                    ASSOC_WARN("wpa multicast cipher error");
                    status_error(pnic_info, pwdn_info,
                                 pmsg, frame_type,
                                 WF_80211_STATUS_INVALID_GROUP_CIPHER);
                    /* abort thread */
                    PT_EXIT(pt);
                }
                /* checkout pairwise cipher */
                pwdn_info->wpa_unicast_cipher =
                    wpa_unicast_cipher & psec_info->wpa_unicast_cipher;
                if (!pwdn_info->wpa_unicast_cipher)
                {
                    ASSOC_WARN("wpa pairwise cipher error");
                    status_error(pnic_info, pwdn_info,
                                 pmsg, frame_type,
                                 WF_80211_STATUS_INVALID_PAIRWISE_CIPHER);
                    /* abort thread */
                    PT_EXIT(pt);
                }
                /* checkout wpa ie */
                wpa_ie_len =
                    WF_MIN(sizeof(pwdn_info->wpa_ie), wpa_ie_len);
                wf_memcpy(pwdn_info->wpa_ie, pie, wpa_ie_len);
                pwdn_info->wpa_ie_len = wpa_ie_len;
                pwdn_info->wpa_enable = wf_true;
            }

            /* wmm parse */
            ret = wf_80211_mgmt_wmm_parse(pie,
                                          WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len);
            if (ret < 0 && ret >= -2)
            {
                ASSOC_WARN("vendor ie corrupt");
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_INVALID_IE);
                /* abort thread */
                PT_EXIT(pt);
            }
            if (!ret)
            {
                ASSOC_DBG("WMM element");
                pwdn_info->wmm_enable = wf_true;
                wf_memcpy(&pwdn_info->wmm_info, pie, WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len);
            }

            if(wf_p2p_is_valid(pnic_info))
            {
                ret = wf_p2p_parse_p2pie(pnic_info, pie, WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len,WF_P2P_IE_ASSOC_REQ);
                if(0 == ret)
                {
                    ASSOC_DBG("p2p element");
                    pwdn_info->is_p2p_device = 1;
                    wf_p2p_proc_assoc_req(pnic_info,(wf_u8*)pie,WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len,pwdn_info,1);
                }
            }
        }

        else if (pie->element_id == WF_80211_MGMT_EID_HT_CAPABILITY)
        {
            if (pcur_network->ht_enable == wf_false)
            {
                ASSOC_WARN("no support ht capability");
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_UNSPECIFIED_FAILURE);
                /* abort thread */
                PT_EXIT(pt);
            }
            pwdn_info->ht_enable = wf_true;
            wf_memcpy(&pwdn_info->ht_cap, pie->data, pie->len);
            /* set network type support N */
            pwdn_info->network_type |= WIRELESS_11_24N;
        }

        else if (pie->element_id == WF_80211_MGMT_EID_HT_OPERATION)
        {
            if (pcur_network->ht_enable == wf_false)
            {
                ASSOC_WARN("no support ht operation");
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_UNSPECIFIED_FAILURE);
                /* abort thread */
                PT_EXIT(pt);
            }
            wf_memcpy(&pwdn_info->ht_info, pie->data, pie->len);
        }
    }

    /* free message */
    wf_ap_msg_free(pnic_info, &pwdn_info->ap_msg, pmsg);

    /* send assoc success frame */
    assoc_ap_xmit_frame(pnic_info, pwdn_info, frame_type, status_code);
    /* notify connection establish */
    wf_ap_odm_connect_media_status(pnic_info, pwdn_info);

    /* set rate id */
    wf_ap_add_sta_ratid(pnic_info, pwdn_info);

    wf_assoc_ap_event_up(pnic_info, pwdn_info, pmsg);
    ASSOC_DBG("assoc end->"WF_MAC_FMT, WF_MAC_ARG(pwdn_info->mac));

    /* thread end */
    PT_END(pt);
}

int wf_assoc_ap_work (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                      wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    wf_80211_frame_e frame_type;

    if (pwdn_info == NULL)
    {
        ASSOC_ERROR("wdn_info null");
        return -1;
    }

    if (mgmt_len > WF_80211_MGMT_ASSOC_SIZE_MAX)
    {
        ASSOC_ERROR("association request frame length too long");
        return -2;
    }

    frame_type = wf_80211_get_frame_type(pmgmt->frame_control);
    if (frame_type != WF_80211_FRM_ASSOC_REQ &&
        frame_type != WF_80211_FRM_REASSOC_REQ)
    {
        return -3;
    }

//    ASSOC_DBG("assoc received");

    if (pwdn_info->mode != WF_MASTER_MODE)
    {
        ASSOC_WARN("the wdn no used for master mode");
        return -4;
    }

    if (wf_ap_msg_load(pnic_info, &pwdn_info->ap_msg,
                       WF_AP_MSG_TAG_ASSOC_REQ_FRAME, pmgmt, mgmt_len))
    {
        ASSOC_WARN("assoc msg enque fail");
        return -5;
    }

    return 0;
}
#endif

static int associate_xmit_frame (nic_info_st *nic_info)
{
    wf_u8 * pframe;
    struct wl_ieee80211_hdr * pwlanhdr = NULL;
    struct xmit_buf * pxmit_buf;
    wf_wlan_mgmt_info_t *wlan_mgmt_info = nic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &wlan_mgmt_info->cur_network;
    wf_u32 pkt_len;
    wf_80211_mgmt_ie_t *pie;
    wdn_net_info_st *wdn_info;
    tx_info_st *tx_info =   (tx_info_st *)nic_info->tx_info;
    hw_info_st *hw_info = nic_info->hw_info;
    sec_info_st *sec_info = nic_info->sec_info;
    unsigned char WMM_IE[] = { 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x00 };
    int ret = 0;
    //ASSOC_DBG();

    /* alloc xmit_buf */
    pxmit_buf = wf_xmit_extbuf_new(tx_info);
    if(pxmit_buf == NULL)
    {
        ASSOC_ERROR("wf_xmit_extbuf_new error");
        ret = -1;
        goto exit;
    }
    wf_memset(pxmit_buf->pbuf, 0, WLANHDR_OFFSET + TXDESC_OFFSET );//MAX_XMIT_EXTBUF_SZ
    pframe = pxmit_buf->pbuf + TXDESC_OFFSET;
    pwlanhdr = (struct wl_ieee80211_hdr *)pframe;
    pwlanhdr->frame_ctl = 0;

    wdn_info = wf_wdn_find_info(nic_info,wf_wlan_get_cur_bssid(nic_info));
    if (wdn_info == NULL)
    {
        ASSOC_ERROR("wdn null");
        ret = -2;
        goto exit;
    }

    /* sta mode */
    SetFrameType(pframe,WIFI_MGT_TYPE);
    SetFrameSubType(pframe, WIFI_ASSOCREQ);     /* set subtype */

    /* copy addr1/2/3 */
    wf_memcpy(pwlanhdr->addr1, wdn_info->mac, MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr2, nic_to_local_addr(nic_info), MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr3, wdn_info->bssid, MAC_ADDR_LEN);

    if (0)
    {
        ASSOC_DBG("wlanhdr:addr1="WF_MAC_FMT, WF_MAC_ARG(pwlanhdr->addr1));
        ASSOC_DBG("wlanhdr:addr2="WF_MAC_FMT, WF_MAC_ARG(pwlanhdr->addr2));
        ASSOC_DBG("wlanhdr:addr3="WF_MAC_FMT, WF_MAC_ARG(pwlanhdr->addr3));
    }

    pkt_len = sizeof(struct wl_ieee80211_hdr_3addr);
    pframe += pkt_len; /* point to iv or frame body */

    /* capability */
    wf_memcpy(pframe, (wf_u8*)&wdn_info->cap_info, 2);
    pkt_len += 2;
    pframe += 2;

    /* listen interval */
    wf_memcpy(pframe, (wf_u8*)&wdn_info->listen_interval, 2);
    pkt_len += 2;
    pframe += 2;

    /* ssid */
    pframe = set_ie(pframe, WF_80211_MGMT_EID_SSID, wdn_info->ssid_len, wdn_info->ssid, &pkt_len);

    /* support rates */
    pframe = set_ie(pframe, WF_80211_MGMT_EID_SUPP_RATES, wdn_info->datarate_len, wdn_info->datarate, &pkt_len);

    /* extend support rates */
    if (wdn_info->ext_datarate_len > 0)
        pframe = set_ie(pframe, WF_80211_MGMT_EID_EXT_SUPP_RATES, wdn_info->ext_datarate_len, wdn_info->ext_datarate, &pkt_len);

    /* power cabability */

    /* Supported Channels */

    /* WPA */
    if (sec_info->wpa_enable)
    {
        pie = (wf_80211_mgmt_ie_t *)sec_info->supplicant_ie;
        pframe = set_ie(pframe, WF_80211_MGMT_EID_VENDOR_SPECIFIC, pie->len, pie->data, &pkt_len);
    }

    /* RSN */
    else if (sec_info->rsn_enable)
    {
        pie = (wf_80211_mgmt_ie_t *)sec_info->supplicant_ie;
        pframe = set_ie(pframe, WF_80211_MGMT_EID_RSN, pie->len, pie->data, &pkt_len);
    }

    /* QoS Capability */
    if (wdn_info->wmm_enable)
    {
        pframe = set_ie(pframe, WF_80211_MGMT_EID_VENDOR_SPECIFIC, 7,(wf_u8 *)WMM_IE, &pkt_len);
    }

    /* RM Enabled Capabilities */

    /* Mobility Domain */

    /* Supported Operating Classes */

    /* HT Capabilities */
    if (wdn_info->ht_enable && hw_info->dot80211n_support)
        pframe = set_ie(pframe, WF_80211_MGMT_EID_HT_CAPABILITY, sizeof(wdn_info->ht_cap), (wf_u8 *)&wdn_info->ht_cap, &pkt_len);

    /* 20/40 BSS Coexistence */

    /* Extended Capabilities */

    /* QoS Traffic Capability */

    /* TIM Broadcast Request */

    /* Interworking */

    /* Vendor Specific*/

    /*p2p*/
    if(wf_p2p_is_valid(nic_info))
    {
        pframe = wf_p2p_fill_assoc_req(nic_info,pframe,&pkt_len,WF_P2P_IE_ASSOC_REQ);
    }

    pxmit_buf->pkt_len = (wf_u16)pkt_len;
    wf_nic_mgmt_frame_xmit(nic_info, wdn_info, pxmit_buf, pxmit_buf->pkt_len);
//      pcur_network->sta_state |= WIFI_ASOC_STATE;

exit:
    if (!ret)
    {
        wf_80211_mgmt_t *pmgmt = (void *)pwlanhdr;
        pcur_network->assoc_req.ie_len =
            pkt_len - WF_OFFSETOF(wf_80211_mgmt_t, assoc_req.listen_interval);
        wf_memcpy(pcur_network->assoc_req.ie,
                  &pmgmt->assoc_req.listen_interval, pcur_network->assoc_req.ie_len);
    }

    return ret;
}

int wf_assoc_frame_parse (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                          wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    if (pnic_info == NULL || pmgmt == NULL || mgmt_len == 0)
    {
        return -1;
    }

    //ASSOC_DBG();

    {
        mlme_state_e state;
        wf_mlme_get_state(pnic_info, &state);
        ASSOC_DBG("state:%d",state);
        if (state != MLME_STATE_ASSOC)
        {
            ASSOC_WARN("mlme state:%d",state);
            return -2;
        }
    }

    if (pwdn_info == NULL)
    {
        ASSOC_WARN("pwdn_info None pointer");
        return -3;
    }

    if (!mac_addr_equal(pmgmt->da, nic_to_local_addr(pnic_info)) ||
        !mac_addr_equal(pwdn_info->bssid, wf_wlan_get_cur_bssid(pnic_info)))
    {
        ASSOC_WARN("mac addr is not equl");
        return -4;
    }
    if (mgmt_len > sizeof(assoc_rsp_t))
    {
        ASSOC_WARN("mgmt_len:%d",mgmt_len);
        return -5;
    }

    /* send message */
    {
        assoc_info_t *passoc_info = pnic_info->assoc_info;
        wf_msg_que_t *pmsg_que = &passoc_info->msg_que;
        wf_msg_t *pmsg;
        int rst;

        rst = wf_msg_new(pmsg_que, WF_ASSOC_TAG_RSP, &pmsg);
        if (rst)
        {
            ASSOC_WARN("msg new fail error fail: %d", rst);
            return -6;
        }
        pmsg->len = mgmt_len;
        wf_memcpy(pmsg->value, pmgmt, mgmt_len);
        rst = wf_msg_push(pmsg_que, pmsg);
        if (rst)
        {
            wf_msg_del(pmsg_que, pmsg);
            ASSOC_WARN("msg push fail error fail: %d", rst);
            return -7;
        }
    }

    return 0;
}


int wf_disassoc_frame_parse (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                             wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    int rst;

    switch (wf_local_cfg_get_work_mode(pnic_info))
    {
        case WF_INFRA_MODE :
            if(pwdn_info)
            {
                ASSOC_DBG("WF_80211_FRM_DISASSOC[%d] frame get, reason:%d",
                          pnic_info->ndev_id,pmgmt->disassoc.reason_code);
                rst = wf_mlme_deauth(pnic_info,
                                     wf_false,
                                     (wf_80211_reasoncode_e)pmgmt->disassoc.reason_code);
                if (rst)
                {
                    ASSOC_WARN("wf_mlme_deauth fail, reason code: %d", rst);
                    return -1;
                }
            }
            break;

#ifdef CFG_ENABLE_AP_MODE
        case WF_MASTER_MODE :
            rst = disassoc_work_ap(pnic_info, pwdn_info, pmgmt, mgmt_len);
            if (rst)
            {
                ASSOC_WARN("disassoc_work_ap fail, reason code: %d", rst);
                return -2;
            }
            break;
#endif

        default :
            return -3;
    }

    return 0;
}

wf_pt_ret_t wf_assoc_sta_thrd (wf_pt_t *pt, nic_info_st *pnic_info, int *prsn)
{
    assoc_info_t *passoc_info;
    wf_msg_que_t *pmsg_que;
    wf_msg_t *pmsg = NULL;
    int reason = WF_ASSOC_TAG_DONE;
    int rst;

    if (pt == NULL || pnic_info == NULL || prsn == NULL)
    {
        PT_EXIT(pt);
    }
    passoc_info = pnic_info->assoc_info;
    pmsg_que = &passoc_info->msg_que;

    PT_BEGIN(pt);

    PT_WAIT_WHILE(pt, nic_mlme_hw_access_trylock(pnic_info));
    /* get assoc start message. */
    do
    {
        if (wf_msg_pop(pmsg_que, &pmsg))
        {
            /* no message */
            ASSOC_WARN("no request message");
            nic_mlme_hw_access_unlock(pnic_info);
            *prsn = -1;
            PT_EXIT(pt);
        }
        if (pmsg->tag != WF_ASSOC_TAG_START)
        {
            /* undesired message */
            ASSOC_DBG("unsuited message, tag: %d", pmsg->tag);
            wf_msg_del(pmsg_que, pmsg);
            PT_YIELD(pt);
            continue;
        }
        wf_msg_del(pmsg_que, pmsg);
        break;
    }
    while (wf_true);
    passoc_info->brun = wf_true;

    for (passoc_info->retry_cnt = 0;
         passoc_info->retry_cnt < ASSOC_REQ_RESEND_TIMES;
         passoc_info->retry_cnt++)
    {
        ASSOC_DBG("send assoc request");
        rst = associate_xmit_frame(pnic_info);
        if (rst)
        {
            ASSOC_WARN("assoc xmit fail, error code: %d", rst);
            reason = -2;
            break;
        }

        /* wait until receive assoc respone */
        wf_timer_set(&passoc_info->timer, ASSOC_RSP_TIMEOUT);
    wait_msg :
        PT_WAIT_UNTIL(pt, !wf_msg_pop(pmsg_que, &pmsg) ||
                      wf_timer_expired(&passoc_info->timer));
        if (pmsg == NULL)
        {
            /* timeout, resend again */
            continue;
        }

        if (pmsg->tag == WF_ASSOC_TAG_ABORT)
        {
            wf_msg_del(pmsg_que, pmsg);
            reason = WF_ASSOC_TAG_ABORT;
            ASSOC_DBG("assoc abort");
            break;
        }
        else if (pmsg->tag == WF_ASSOC_TAG_RSP)
        {
            wf_80211_mgmt_t *pmgmt = (wf_80211_mgmt_t *)pmsg->value;
            wf_u16 mgmt_len = (wf_u16)pmsg->len;
            if (!pmgmt->assoc_resp.status_code)
            {
                wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
                wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
                /* retrive AID and IE */
                pcur_network->aid = pmgmt->assoc_resp.aid;
                pcur_network->assoc_resp.ie_len =
                    mgmt_len - WF_OFFSETOF(wf_80211_mgmt_t, assoc_resp.variable);
                wf_memcpy(pcur_network->assoc_resp.ie,
                          &pmgmt->assoc_resp.variable,
                          pcur_network->assoc_resp.ie_len);
                /* assoc success */
                ASSOC_INFO("assoc success");
                wf_msg_del(pmsg_que, pmsg);
                reason = WF_ASSOC_TAG_DONE;
                break;
            }
            else
            {
                ASSOC_WARN("assoc status_code:0x%x",pmgmt->assoc_resp.status_code);
            }
            wf_msg_del(pmsg_que, pmsg);
        }
        else
        {
            ASSOC_WARN("unsutied message tag(%d)", pmsg->tag);
            wf_msg_del(pmsg_que, pmsg);
            goto wait_msg;
        }
    }
    if (passoc_info->retry_cnt == ASSOC_REQ_RESEND_TIMES)
    {
        ASSOC_DBG("no respone receive");
        reason = -3;
    }

    passoc_info->brun = wf_false;
    nic_mlme_hw_access_unlock(pnic_info);

    *prsn = reason;
    if (reason)
    {
        PT_EXIT(pt);
    }
    PT_END(pt);
}

int wf_assoc_start (nic_info_st *pnic_info)
{
    assoc_info_t *passoc_info;

    if (pnic_info == NULL || WF_CANNOT_RUN(pnic_info))
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        return -2;
    }
    passoc_info = pnic_info->assoc_info;

    ASSOC_DBG();

    /* new message information */
    {
        wf_msg_que_t *pmsg_que = &passoc_info->msg_que;
        wf_msg_t *pmsg;
        int rst;

        rst = wf_msg_new(pmsg_que, WF_ASSOC_TAG_START, &pmsg);
        if (rst)
        {
            ASSOC_WARN("msg new fail error fail: %d", rst);
            return -3;
        }
        rst = wf_msg_push(pmsg_que, pmsg);
        if (rst)
        {
            ASSOC_WARN("msg push fail error fail: %d", rst);
            return -4;
        }
    }

    return 0;
}

int wf_assoc_stop (nic_info_st *pnic_info)
{
    assoc_info_t *passoc_info;

    if (pnic_info == NULL || WF_CANNOT_RUN(pnic_info))
    {
        return -1;
    }

    ASSOC_DBG();

    if (!pnic_info->is_up)
    {
        return -2;
    }

    passoc_info = pnic_info->assoc_info;
    if (!passoc_info->brun)
    {
        return -3;
    }

    {
        wf_msg_que_t *pmsg_que = &passoc_info->msg_que;
        wf_msg_t *pmsg;
        int rst;

        rst = wf_msg_new(pmsg_que, WF_ASSOC_TAG_ABORT, &pmsg);
        if (rst)
        {
            ASSOC_WARN("msg new fail error fail: %d", rst);
            return -4;
        }
        rst = wf_msg_push(pmsg_que, pmsg);
        if (rst)
        {
            wf_msg_del(pmsg_que, pmsg);
            ASSOC_WARN("msg push fail error fail: %d", rst);
            return -5;
        }
    }

    return 0;
}

wf_inline static int assoc_msg_init (wf_msg_que_t *pmsg_que)
{
    wf_msg_init(pmsg_que);
    return (wf_msg_alloc(pmsg_que, WF_ASSOC_TAG_RSP, sizeof(assoc_rsp_t), 2) ||
            wf_msg_alloc(pmsg_que, WF_ASSOC_TAG_ABORT, 0, 1) ||
            wf_msg_alloc(pmsg_que, WF_ASSOC_TAG_START, 0, 1)) ? -1 : 0;
}

wf_inline static void assoc_msg_deinit (wf_msg_que_t *pmsg_que)
{
    wf_msg_deinit(pmsg_que);
}

int wf_assoc_init (nic_info_st *pnic_info)
{
    assoc_info_t *passoc_info;

    if (pnic_info == NULL)
    {
        return -1;
    }

    ASSOC_DBG();

    passoc_info = wf_kzalloc(sizeof(assoc_info_t));
    if (passoc_info == NULL)
    {
        ASSOC_ERROR("malloc assoc_info failed");
        return -2;
    }
    pnic_info->assoc_info = passoc_info;
    passoc_info->brun = wf_false;
    if (assoc_msg_init(&passoc_info->msg_que))
    {
        ASSOC_ERROR("assoc msg init failed");
        return -3;
    }

    return 0;
}

int wf_assoc_term (nic_info_st *pnic_info)
{
    assoc_info_t *passoc_info;

    if (pnic_info == NULL)
    {
        return 0;
    }
    passoc_info = pnic_info->assoc_info;

    ASSOC_DBG();

    if (passoc_info)
    {
        assoc_msg_deinit(&passoc_info->msg_que);
        wf_kfree(passoc_info);
        pnic_info->assoc_info = NULL;
    }

    return 0;
}

