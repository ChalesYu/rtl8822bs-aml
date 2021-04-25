#include "common.h"
#include "wf_debug.h"
#ifdef CFG_ENABLE_ADHOC_MODE

#if 0
#define ADHOC_DBG(fmt, ...)      LOG_D("[%s]"fmt, __func__, ##__VA_ARGS__)
#define ADHOC_ARRAY(data, len)   log_array(data, len)
#else
#define ADHOC_DBG(fmt, ...)
#define ADHOC_ARRAY(data, len)
#endif
#define ADHOC_INFO(fmt, ...)     LOG_I("[%s]"fmt, __func__, ##__VA_ARGS__)
#define ADHOC_WARN(fmt, ...)     LOG_E("[%s]"fmt, __func__, ##__VA_ARGS__)

wf_bool get_adhoc_master (nic_info_st *pnic_info)
{
    local_info_st *plocal = (local_info_st *)pnic_info->local_info;
    return plocal->adhoc_master;
}

wf_bool set_adhoc_master (nic_info_st *pnic_info,wf_bool status)
{
    local_info_st *plocal = (local_info_st *)pnic_info->local_info;
    return plocal->adhoc_master = status;
}

inline wf_u32 wf_adhoc_andom32(void)
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

static void wf_make_random_ibss(wf_u8 * pibss)
{
    *((u32 *) (&pibss[2])) = wf_adhoc_andom32();
    pibss[0] = 0x02;
    pibss[1] = 0x11;
    pibss[2] = 0x22;
}

int adhoc_work (nic_info_st *pnic_info, wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    wdn_net_info_st *pwdn_info;

    if (mgmt_len > WF_80211_MGMT_BEACON_SIZE_MAX)
    {
        ADHOC_WARN("auth frame length too long");
        return -1;
    }

    if (wf_80211_get_frame_type(pmgmt->frame_control) != WF_80211_FRM_BEACON)
    {
        return -2;
    }

    pwdn_info = wf_wdn_find_info(pnic_info, pmgmt->sa);
    if(pwdn_info)
    {
        return 0;
    }

    /* create wdn if no find the node */
    if (pwdn_info == NULL)
    {
        ADHOC_DBG("create new wdn");
        ADHOC_DBG("pmgmt->sa:"WF_MAC_FMT,WF_MAC_ARG(pmgmt->sa));
        pwdn_info = wf_wdn_add(pnic_info, pmgmt->sa);
        if (pwdn_info == NULL)
        {
            ADHOC_WARN("wdn alloc fail");
            return -3;
        }
        adhoc_wdn_info_update(pnic_info, pwdn_info);
    }

    if (wf_ap_msg_load(pnic_info, &pwdn_info->ap_msg,
                       WF_AP_MSG_TAG_BEACON_FRAME, pmgmt, mgmt_len))
    {
        ADHOC_WARN("beacon msg enque fail");
        return -5;
    }

    return 0;
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
}

static void ap_set_sta_ratid (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                              wf_u8 bw, wf_u8 sgi, wf_u64 bitmap)
{
    wf_u32 rate_bitmap = (wf_u32) bitmap;
    wf_u32 mask = rate_bitmap & 0x0FFFFFFF;
    wf_odm_set_rfconfig(pnic_info, pwdn_info->wdn_id, pwdn_info->raid, bw, sgi, mask);
}

wf_pt_rst_t wf_beacon_adhoc_thrd (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_pt_t *pt = &pwdn_info->sub_thrd_pt;
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
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
    wf_80211_statuscode_e status_code;
    wf_u8 wpa_ie_len;

    PT_BEGIN(pt);

    pmsg = wf_ap_msg_get(&pwdn_info->ap_msg);
    if (pmsg == NULL)
    {
        /* abort thread */
        PT_EXIT(pt);
    }
    ADHOC_INFO("beacon frame received");
    if (pmsg->tag != WF_AP_MSG_TAG_BEACON_FRAME)
    {
        wf_ap_msg_free(pnic_info, &pwdn_info->ap_msg, pmsg);
        ADHOC_DBG("no beacon frame received");
        /* abort thread */
        PT_EXIT(pt);
    }

    ADHOC_DBG("beacon begin->"WF_MAC_FMT, WF_MAC_ARG(pwdn_info->mac));

    /* parse assocation frame */
    ADHOC_DBG("beacon arrived");
    ADHOC_ARRAY((void *)&pmsg->mgmt, pmsg->len);

    /* retrive frame type */
    frame_type = wf_80211_get_frame_type(pmsg->mgmt.frame_control);

    /* retrive element fiexd fields */
    pmgmt = &pmsg->mgmt;
    pwdn_info->cap_info = wf_le16_to_cpu(pmgmt->beacon.capab);
    pwdn_info->bcn_interval = wf_le16_to_cpu(pmgmt->beacon.intv);

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
    if (frame_type == WF_80211_FRM_BEACON)
    {
        pvar = pmsg->mgmt.beacon.variable;
        var_len = pmsg->len - WF_OFFSETOF(wf_80211_mgmt_t, beacon.variable);
    }
    else
    {
        pvar = pmsg->mgmt.reassoc_req.variable;
        var_len = pmsg->len - WF_OFFSETOF(wf_80211_mgmt_t, beacon.variable);
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
                ADHOC_WARN("ssid error(len=%d): ", pie->len);
                if (pie->len)
                {
                    ADHOC_ARRAY(pie->data, pie->len);
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
                ADHOC_WARN("rates(%d) list size over limite", pie->len);
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
                        break;
                    if (pie->data[i] == pcur_network->rate[j])
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
                            ADHOC_WARN("beyond support rate upper limit");
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
                ADHOC_WARN("invalid support extend rates");
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_ASSOC_DENIED_RATES);
                /* abort thread */
                PT_EXIT(pt);
            }
            ADHOC_DBG("data rate(Mbps): %d, %d, %d, %d, %d, %d, %d, %d",
                      (pwdn_info->datarate[0] & 0x7F) / 2,
                      (pwdn_info->datarate[1] & 0x7F) / 2,
                      (pwdn_info->datarate[2] & 0x7F) / 2,
                      (pwdn_info->datarate[3] & 0x7F) / 2,
                      (pwdn_info->datarate[4] & 0x7F) / 2,
                      (pwdn_info->datarate[5] & 0x7F) / 2,
                      (pwdn_info->datarate[6] & 0x7F) / 2,
                      (pwdn_info->datarate[7] & 0x7F) / 2);
        }

        else if (pie->element_id == WF_80211_MGMT_EID_EXT_SUPP_RATES)
        {
            if (pie->len > WF_ARRAY_SIZE(pwdn_info->ext_datarate))
            {
                ADHOC_WARN("support extend rates(%d) list size over limite", pie->len);
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
                        break;
                    if (pie->data[i] == pcur_network->rate[j])
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
                            ADHOC_WARN("beyond support rate upper limit");
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
                ADHOC_WARN("invalid support extend rates");
                status_error(pnic_info, pwdn_info,
                             pmsg, frame_type,
                             WF_80211_STATUS_ASSOC_DENIED_RATES);
                /* abort thread */
                PT_EXIT(pt);
            }
            ADHOC_DBG("extend data rate(Mbps): %d, %d, %d, %d",
                      (pwdn_info->ext_datarate[0] & 0x7F) / 2,
                      (pwdn_info->ext_datarate[1] & 0x7F) / 2,
                      (pwdn_info->ext_datarate[2] & 0x7F) / 2,
                      (pwdn_info->ext_datarate[3] & 0x7F) / 2);

        }

        else if (pie->element_id == WF_80211_MGMT_EID_DS_PARAMS)
        {
            if(pie->data[0] == pcur_network->channel)
            {
                pwdn_info->channel = pie->data[0];
            }
        }
    }

    /* free message */
    wf_ap_msg_free(pnic_info, &pwdn_info->ap_msg, pmsg);

    /* notify connection establish */
    wf_ap_odm_connect_media_status(pnic_info, pwdn_info);

    wf_assoc_ap_event_up(pnic_info, pwdn_info, pmsg);
    ADHOC_DBG("beacon end->"WF_MAC_FMT, WF_MAC_ARG(pwdn_info->mac));

//    wf_mcu_set_mlme_join(pnic_info, 2);

    wf_ap_add_sta_ratid(pnic_info, pwdn_info);

//    wf_mcu_set_mlme_scan(pnic_info, wf_false);
    /* thread end */
    PT_END(pt);
}


void adhoc_wdn_info_update (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;


    /* ap message queue initilize */
    wf_que_init(&pwdn_info->ap_msg, WF_LOCK_TYPE_SPIN);
    /* ap trhead initilize */
    PT_INIT(&pwdn_info->ap_thrd_pt);
    /* generate wdn aid */
    pwdn_info->aid = pwdn_info->wdn_id + 1;
    /* retreive bssid into wdn_info */
    wf_memcpy(pwdn_info->bssid, pcur_network->bssid,
              WF_ARRAY_SIZE(pwdn_info->bssid));
    /* reset connection rx packets statistics */
    pwdn_info->rx_pkt_stat = 0;
    /* set mode */
    pwdn_info->mode = WF_ADHOC_MODE;
    pwdn_info->ieee8021x_blocked = wf_true;
}

int wf_adhoc_do_probrsp (nic_info_st *pnic_info,
                 wf_80211_mgmt_t *pframe, wf_u16 frame_len)
{
    struct xmit_buf *pxmit_buf;
    tx_info_st *ptx_info;
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_80211_mgmt_t *pmgmt;
    wf_bool bConnect;

    if (wf_80211_get_frame_type(pframe->frame_control) != WF_80211_FRM_PROBE_REQ)
    {
        return -1;
    }

    if (!(mac_addr_equal(pframe->da, pwlan_info->cur_network.mac_addr) ||
          is_bcast_addr(pframe->da)))
    {
        ADHOC_WARN("probe request target address invalid");
        return -2;
    }

    /* alloc xmit_buf */
    ptx_info = (tx_info_st *)pnic_info->tx_info;
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if(pxmit_buf == NULL)
    {
        ADHOC_WARN("pxmit_buf is NULL");
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

    /* set pies fiexd field */
    pmgmt->beacon.intv = wf_cpu_to_le16(pcur_network->bcn_interval);
    pmgmt->beacon.capab = wf_cpu_to_le16(pcur_network->cap_info);

    /* set pies variable fields */
    wf_memcpy(pmgmt->beacon.variable,
              pcur_network->ies, pcur_network->ies_length);

    /* send packet */
    pxmit_buf->pkt_len = WF_OFFSETOF(wf_80211_mgmt_t, probe_resp.variable) +
                         pcur_network->ies_length;
    wf_nic_mgmt_frame_xmit(pnic_info, NULL, pxmit_buf, pxmit_buf->pkt_len);

    return 0;
}

int proc_start_create_ibss_func(nic_info_st *pnic_info)
{
    wf_u8 val8;
    wf_u32 bxmitok = wf_false;
    wf_u8 join_type;
    unsigned short caps;
    wf_80211_mgmt_capab_t capab;
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    wf_u8 null_addr[ETH_ALEN] = { 0, 0, 0, 0, 0, 0 };
    struct beacon_ie *pbcn = (struct beacon_ie *)pcur_network->ies;
    capab = wf_le16_to_cpu(pbcn->capab);

    if (WF_80211_CAPAB_IS_IBSS(capab))
    {
        // set media status
        wf_mcu_set_media_status(pnic_info, WIFI_FW_ADHOC_STATE & 03);

        if (pcur_network->ssid.data && pcur_network->ssid.length)
        {
            if(wf_adhoc_send_beacon(pnic_info))
            {
                ADHOC_INFO("ADHOC send beacon fall!!!!");
                wf_mlme_set_connect(pnic_info, wf_false);
                wf_mcu_set_mlme_join(pnic_info, 1);
                wf_mcu_set_bssid(pnic_info, null_addr);
                return -1;
            }
            else
            {
               wf_mcu_set_bssid(pnic_info, pwlan_info->cur_network.mac_addr);
               wf_mcu_set_mlme_join(pnic_info, 0);

               wf_mlme_set_connect(pnic_info, wf_true);
            }
        }
    }

    return 0;
}

wf_u32 get_rate_len(wf_u8 *rate)
{
    int i = 0;

    while(1)
    {
        if(rate[i] == 0)
        {
            break;
        }

        if(i > 12)
        {
            break;
        }

        i++;
    }

    return i;
}

static void adhoc_init_reg (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u32 tmp32u;

    ADHOC_DBG();

    /* set bssid */
    wf_mcu_set_bssid(pnic_info, (void *)pcur_network->bssid);

    wf_mcu_set_mlme_join(pnic_info, 0);

    /* set channel basebond */
    wf_hw_info_set_channnel_bw(pnic_info,
                               pcur_network->channel, CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

    /* set SISF */
    wf_mcu_set_sifs(pnic_info, 0x0a0a0808); /* 0x0808 -> for CCK, 0x0a0a -> for OFDM */

    /* set preamble */
    wf_mcu_set_preamble(pnic_info,
                        (pcur_network->cap_info & WF_80211_MGMT_CAPAB_SHORT_PREAMBLE) ?
                        wf_true : wf_false);

    /* set slot time */
    wf_mcu_set_slot_time(pnic_info, pcur_network->short_slot);

    /* set ADHOC mode */
    wf_mcu_set_media_status(pnic_info, WIFI_FW_ADHOC_STATE);

    /* Set Security */
    wf_mcu_set_sec_cfg(pnic_info, 0xcf);
}

static void adhoc_update_reg (nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_u16 br_cfg;

    /* set basic rate */
    get_bratecfg_by_support_dates(pcur_network->rate, pcur_network->rate_len,
                                  &br_cfg);
    wf_mcu_set_basic_rate(pnic_info, br_cfg);

    /* set beacon interval */
    wf_mcu_set_bcn_intv(pnic_info, pcur_network->bcn_interval * 100);

    /* set slot time */
    wf_mcu_set_slot_time(pnic_info, pcur_network->short_slot);

    /* set AC_PARAM */
    wf_mcu_set_ac_vo(pnic_info);
    wf_mcu_set_ac_vi(pnic_info);
    wf_mcu_set_ac_be(pnic_info);
    wf_mcu_set_ac_bk(pnic_info);

    wf_mcu_set_mlme_join(pnic_info, 2);
}

wf_u32 rate_len_to_get_func(wf_u8 * rateset)
{
    wf_u32 i = 0;
    while (1) {
        if ((rateset[i]) == 0)
        {
            break;
        }

        if (i > 12)
        {
            break;
        }

        i++;
    }
    return i;
}

void wf_adhoc_genarate_ie(nic_info_st *pnic_info)
{
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    hw_info_st *hw_info = pnic_info->hw_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u8 wireless_mode = 0;
    const wf_u16 atimwindow = 0;
    wf_u32 sz = 0, rateLen;
    wf_u8 *ie = pcur_network->ies;

    ie = set_ie(ie, WF_80211_MGMT_EID_SSID, pcur_network->ssid.length,
                                 pcur_network->ssid.data, &sz);

    rateLen = rate_len_to_get_func(hw_info->datarate);


    pcur_network->rate_len = rateLen;
    wf_memcpy(pcur_network->rate, hw_info->datarate, pcur_network->rate_len);

    if (rateLen > 8)
    {
        ie = set_ie(ie, WF_80211_MGMT_EID_SUPP_RATES, 8,
                                        hw_info->datarate, &sz);
    }
    else
    {
        ie = set_ie(ie, WF_80211_MGMT_EID_SUPP_RATES, rateLen,
                                         hw_info->datarate, &sz);
    }

    ie = set_ie(ie, WF_80211_MGMT_EID_DS_PARAMS, 1,
                                 (wf_u8 *) & (pcur_network->channel), &sz);

    ie = set_ie(ie, WF_80211_MGMT_EID_IBSS_PARAMS, 2,
                                 (wf_u8 *) & atimwindow, &sz);

    if (rateLen > 8)
    {
        ie = set_ie(ie, WF_80211_MGMT_EID_EXT_SUPP_RATES, (rateLen - 8),
                                         (hw_info->datarate + 8), &sz);
    }

    pcur_network->ies_length =sz;

 }

int wf_adhoc_send_beacon(nic_info_st *pnic_info)
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
        ADHOC_WARN("pxmit_buf is NULL");
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
    pmgmt->beacon.intv = wf_cpu_to_le16(pcur_network->bcn_interval * 100);
    pmgmt->beacon.capab = wf_cpu_to_le16(pcur_network->cap_info);

    /* set ie variable fields */
    wf_memcpy(pmgmt->beacon.variable,
              pcur_network->ies, pcur_network->ies_length);

    /* send packet */
    pxmit_buf->pkt_len =
        WF_OFFSETOF(wf_80211_mgmt_t, beacon.variable) + pcur_network->ies_length;

    ADHOC_DBG("xmit beacon!!!!");
    wf_nic_beacon_xmit(pnic_info, pxmit_buf, pxmit_buf->pkt_len);

    return 0;
}

int wf_proc_ibss_join(nic_info_st * pnic_info)
{
    wf_bool bBusy;
    wf_bool  fscanned_empty;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_wlan_info_t *wlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &wlan_info->cur_network;
    wf_80211_mgmt_t *pmgmt;
    wf_u32  ret = 0;
    wf_u16 mgmt_len;
    int scanned_ret;
    wf_wlan_scanned_info_t *pscanned_info;

    wf_wlan_scanned_each_begin(pscanned_info, pnic_info)
    {
    	if((pscanned_info->opr_mode == WF_WLAN_OPR_MODE_ADHOC) &&
            (pscanned_info->privacy == wf_false) &&
            pscanned_info->ssid.length == pcur_network->ssid.length &&
            !wf_memcmp(pscanned_info->ssid.data, pcur_network->ssid.data, pcur_network->ssid.length) &&
            (pcur_network->channel == pscanned_info->channel))
        {
            wf_memcpy(pcur_network->bssid, pscanned_info->bssid, ETH_ALEN);
            pcur_network->timestamp = pscanned_info->timestamp;
            break;
        }
    }
    wf_wlan_scanned_each_end(pnic_info, &scanned_ret);

    if(WF_WLAN_SCANNED_EACH_RET_BREAK == scanned_ret)
    {
		ADHOC_INFO("find ibss, join ibss network!!!");
    }
    else
    {
        ADHOC_INFO("no same ibss network, create ibss!!!");
        wf_make_random_ibss(wlan_info->cur_network.bssid);
    }

    wf_adhoc_genarate_ie(pnic_info);
    adhoc_init_reg(pnic_info);
    wf_os_api_ind_connect(pnic_info, WF_MLME_FRAMEWORK_NETLINK);
    wf_mlme_set_connect(pnic_info, wf_true);
    set_adhoc_master(pnic_info, wf_true);

    /* enable data queue */
    wf_os_api_enable_all_data_queue(pnic_info->ndev);

    wf_mcu_set_correct_tsf(pnic_info, 0);

    if(wf_adhoc_send_beacon(pnic_info))
    {
        ADHOC_WARN("send beacon fail!!!");
        ret = -1;
        goto exit;
    }

    wf_mcu_set_mlme_join(pnic_info, 2);
    adhoc_update_reg(pnic_info);

    ret = wf_ap_work_start(pnic_info);

exit:

    return ret;
}

#endif
