/*
 * p2p.c
 *
 * used for .....
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
#include "common.h"
#include "wf_debug.h"

#if 1
#define P2P_DBG(fmt, ...)      LOG_D("P2P[%s:%d][%d]"fmt, __func__,__LINE__,pnic_info->ndev_id, ##__VA_ARGS__)
#define P2P_ARRAY(data, len)   log_array(data, len)
#else
#define P2P_DBG(fmt, ...)
#define P2P_ARRAY(data, len)
#endif
#define P2P_INFO(fmt, ...)     LOG_I("P2P[%s:%d][%d]"fmt, __func__,__LINE__,pnic_info->ndev_id, ##__VA_ARGS__)
#define P2P_WARN(fmt, ...)     LOG_E("P2P[%s:%d][%d]"fmt, __func__,__LINE__, pnic_info->ndev_id, ##__VA_ARGS__)


static wf_u8 wl_basic_rate_cck[4] =
{
    IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK
};

static wf_u8 wl_basic_rate_ofdm[3] =
{
    IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK
};

static wf_u8 wl_basic_rate_mix[7] =
{
    IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK
};

wf_bool wf_p2p_check_buddy_linkstate(nic_info_st *pnic_info)
{
    wf_bool bconnect = wf_false;
    mlme_state_e  state = MLME_STATE_IDLE;
    wf_mlme_get_state(pnic_info->buddy_nic,&state);
    if(state == MLME_STATE_ASSOC)
    {
        bconnect = wf_true;
    }
    
    return bconnect;
}
wf_u8 wf_p2p_get_buddy_channel(nic_info_st *pnic_info)
{
    nic_info_st *pbuddy_nic = NULL;
    mlme_info_t *pmlme_info = NULL;
    wdn_net_info_st *pwdn_info = NULL;
    if(NULL == pnic_info)
    {
        P2P_WARN("input param is null");
        return 0;
    }
    pbuddy_nic = pnic_info->buddy_nic;
    if(NULL == pbuddy_nic)
    {
        P2P_WARN("pbuddy_nic is null");
        return 0;
    }
    pmlme_info = pbuddy_nic->mlme_info;
    if(NULL == pbuddy_nic)
    {
        P2P_WARN("pmlme_info is null");
        return 0;
    }
    
    pwdn_info = pmlme_info->pwdn_info;
    if(NULL == pwdn_info)
    {
        P2P_WARN("pwdn_info is null");
        return 0;
    }
    
    return pwdn_info->channel;
}

static char *p2p_attr_to_str(wf_u8 attr_id)
{
    switch(attr_id)
    {
        case P2P_ATTR_STATUS                 : {return to_str(P2P_ATTR_STATUS);}
        case P2P_ATTR_MINOR_REASON_CODE      : {return to_str(P2P_ATTR_MINOR_REASON_CODE);}
        case P2P_ATTR_CAPABILITY             : {return to_str(P2P_ATTR_CAPABILITY);}
        case P2P_ATTR_DEVICE_ID              : {return to_str(P2P_ATTR_DEVICE_ID);}
        case P2P_ATTR_GO_INTENT              : {return to_str(P2P_ATTR_GO_INTENT);}
        case P2P_ATTR_CONF_TIMEOUT           : {return to_str(P2P_ATTR_CONF_TIMEOUT);}
        case P2P_ATTR_LISTEN_CH              : {return to_str(P2P_ATTR_LISTEN_CH);}
        case P2P_ATTR_GROUP_BSSID            : {return to_str(P2P_ATTR_GROUP_BSSID);}
        case P2P_ATTR_EX_LISTEN_TIMING       : {return to_str(P2P_ATTR_EX_LISTEN_TIMING);}
        case P2P_ATTR_INTENTED_IF_ADDR       : {return to_str(P2P_ATTR_INTENTED_IF_ADDR);}
        case P2P_ATTR_MANAGEABILITY          : {return to_str(P2P_ATTR_MANAGEABILITY);}
        case P2P_ATTR_CH_LIST                : {return to_str(P2P_ATTR_CH_LIST);}
        case P2P_ATTR_NOA                    : {return to_str(P2P_ATTR_NOA);}
        case P2P_ATTR_DEVICE_INFO            : {return to_str(P2P_ATTR_DEVICE_INFO);}
        case P2P_ATTR_GROUP_INFO             : {return to_str(P2P_ATTR_GROUP_INFO);}
        case P2P_ATTR_GROUP_ID               : {return to_str(P2P_ATTR_GROUP_ID);}
        case P2P_ATTR_INTERFACE              : {return to_str(P2P_ATTR_INTERFACE);}
        case P2P_ATTR_OPERATING_CH           : {return to_str(P2P_ATTR_OPERATING_CH);}
        case P2P_ATTR_INVITATION_FLAGS       : {return to_str(P2P_ATTR_INVITATION_FLAGS);}
        default:return "unknown p2p attr";
    }
    return NULL;
}
static void p2p_show_attr(wf_u8 *attr)
{
    wf_u8 attr_id = *attr;
    wf_u16 attr_data_len = WF_GET_LE16(attr + 1);
    
    if(1 == attr_data_len)
    {
        LOG_I("-p2p-[%s] %u(%u): %u",p2p_attr_to_str(attr_id), attr_id, attr_data_len,attr[3]);
    }
    else if(2 == attr_data_len)
    {
        LOG_I("-p2p-[%s] %u(%u): %u",p2p_attr_to_str(attr_id), attr_id, attr_data_len,*(wf_u16*)&attr[3]);
    }
    else if(4 == attr_data_len)
    {
        LOG_I("-p2p-[%s] %u(%u): %d",p2p_attr_to_str(attr_id), attr_id, attr_data_len,*(wf_u32*)&attr[3]);
    }
    else
    {
        LOG_I("-p2p-[%s] %u(%u):",p2p_attr_to_str(attr_id), attr_id, attr_data_len);
    }
}
wf_s32 wf_p2p_dump_attrs(wf_u8 * p2p_ie, wf_u32 p2p_ielen)
{
    wf_u8 *attr_ptr         = NULL;
    
    if (!p2p_ie || p2p_ielen <= 6 || (p2p_ie[0] != WF_80211_MGMT_EID_VENDOR_SPECIFIC) || (wf_memcmp(p2p_ie + 2, P2P_OUI, 4) != 0))
    {
        return -1;
    }

    attr_ptr = p2p_ie + 6;

    while ((attr_ptr - p2p_ie + 3) <= p2p_ielen)
    {
        wf_u16 attr_data_len = WF_GET_LE16(attr_ptr + 1);
        wf_u16 attr_len = attr_data_len + 3;

        p2p_show_attr(attr_ptr);
        if ((attr_ptr - p2p_ie + attr_len) > p2p_ielen)
        {
            break;
        }

       attr_ptr += attr_len;
        
    }

    return 0;
}

char* wf_p2p_role_to_str(P2P_ROLE role)
{
    switch(role)
    {
        case P2P_ROLE_DISABLE: {return to_str(P2P_ROLE_DISABLE);}
        case P2P_ROLE_DEVICE: {return to_str(P2P_ROLE_DEVICE);}
        case P2P_ROLE_CLIENT: {return to_str(P2P_ROLE_CLIENT);}
        case P2P_ROLE_GO : {return to_str(P2P_ROLE_GO);}
        default:return "Unknown_p2p_role";
    }
    return NULL;
}
char *wf_p2p_state_to_str(P2P_STATE state)
{
    switch(state)
    {
        case P2P_STATE_NONE                        : {return to_str(P2P_STATE_NONE);}
        case P2P_STATE_IDLE                         : {return to_str(P2P_STATE_IDLE);}
        case P2P_STATE_LISTEN                             : {return to_str(P2P_STATE_LISTEN);}
        case P2P_STATE_SCAN                        : {return to_str(P2P_STATE_SCAN);}
        case P2P_STATE_FIND_PHASE_LISTEN             : {return to_str(P2P_STATE_FIND_PHASE_LISTEN);}
        case P2P_STATE_FIND_PHASE_SEARCH            : {return to_str(P2P_STATE_FIND_PHASE_SEARCH);}
        case P2P_STATE_TX_PROVISION_DIS_REQ          : {return to_str(P2P_STATE_TX_PROVISION_DIS_REQ);}
        case P2P_STATE_RX_PROVISION_DIS_RSP         : {return to_str(P2P_STATE_RX_PROVISION_DIS_RSP);}
        case P2P_STATE_RX_PROVISION_DIS_REQ         : {return to_str(P2P_STATE_RX_PROVISION_DIS_REQ);}
        case P2P_STATE_GONEGO_ING                   : {return to_str(P2P_STATE_GONEGO_ING);}
        case P2P_STATE_GONEGO_OK                  : {return to_str(P2P_STATE_GONEGO_OK);}
        case P2P_STATE_GONEGO_FAIL                  : {return to_str(P2P_STATE_GONEGO_FAIL);}
        case P2P_STATE_RECV_INVITE_REQ_MATCH      : {return to_str(P2P_STATE_RECV_INVITE_REQ_MATCH);}
        case P2P_STATE_PROVISIONING_ING          : {return to_str(P2P_STATE_PROVISIONING_ING);}
        case P2P_STATE_PROVISIONING_DONE           : {return to_str(P2P_STATE_PROVISIONING_DONE);}
        case P2P_STATE_TX_INVITE_REQ               : {return to_str(P2P_STATE_TX_INVITE_REQ);}
        case P2P_STATE_RX_INVITE_RESP_OK            : {return to_str(P2P_STATE_RX_INVITE_RESP_OK);}
        case P2P_STATE_RECV_INVITE_REQ_DISMATCH     : {return to_str(P2P_STATE_RECV_INVITE_REQ_DISMATCH);}
        case P2P_STATE_RECV_INVITE_REQ_GO          : {return to_str(P2P_STATE_RECV_INVITE_REQ_GO);}
        case P2P_STATE_RECV_INVITE_REQ_JOIN       : {return to_str(P2P_STATE_RECV_INVITE_REQ_JOIN);}
        case P2P_STATE_RX_INVITE_RESP_FAIL         : {return to_str(P2P_STATE_RX_INVITE_RESP_FAIL);}
        case P2P_STATE_RX_INFOR_NOREADY            : {return to_str(P2P_STATE_RX_INFOR_NOREADY);}
        case P2P_STATE_TX_INFOR_NOREADY            : {return to_str(P2P_STATE_TX_INFOR_NOREADY);}
        default:return "Unknown_p2p_state";
    }
    return NULL;
}

wf_s32 reg_classes_full_count_to_get_func(struct p2p_channels channel_list)
{
    wf_s32 cnt  = 0;
    wf_s32 i    = 0;

    for (i = 0; i < channel_list.reg_classes; i++) 
    {
        cnt += channel_list.reg_class[i].channels;
    }

    return cnt;
}

wf_u32 p2p_merged_ies_len_tp_get_func(wf_u8 * in_ie, wf_u32 in_len, wf_u8 flag)
{
    wf_80211_var_ie_st *pIE = NULL;
    wf_u8 OUI[4] = { 0x50, 0x6f, 0x9a, 0x09 };
    wf_s32 i = 0;
    wf_s32 len = 0;

    if (flag) 
    {
        while (i < in_len) 
        {
            pIE = (wf_80211_var_ie_st*) (in_ie + i);

            if (pIE->ElementID == WF_80211_MGMT_EID_VENDOR_SPECIFIC && 0 == wf_memcmp(pIE->data, OUI, 4)) 
            {
                len += pIE->Length - 4;
            }

            i += (pIE->Length + 2);
        }
    }
    return len + 4;
}

wf_s32 proc_p2p_merge_ies_func(wf_u8 * in_ie, wf_u32 in_len, wf_u8 * merge_ie, wf_u8 flag)
{
    wf_80211_var_ie_st *pIE;
    wf_u8 len = 0;
    wf_u8 OUI[4] = { 0x50, 0x6f, 0x9a, 0x09 };
    wf_u8 ELOUI[6] = { 0xDD, 0x00, 0x50, 0x6f, 0x9a, 0x09 };
    wf_s32 i = 0;

    if (merge_ie != NULL) 
    {
        if (flag) 
        {
            wf_memcpy(merge_ie, ELOUI, 6);
            merge_ie += 6;
        }
        while (i < in_len) {
            pIE = (wf_80211_var_ie_st*) (in_ie + i);

            if (pIE->ElementID == WF_80211_MGMT_EID_VENDOR_SPECIFIC
                && 0 == wf_memcmp(pIE->data, OUI, 4)) 
            {
                wf_memcpy(merge_ie, pIE->data + 4, pIE->Length - 4);
                len += pIE->Length - 4;
                merge_ie += pIE->Length - 4;
            }

            i += (pIE->Length + 2);
        }

        return len + 4;

    }

    return 0;
}

static void p2p_do_renew_tx_rate(nic_info_st *pnic_info, wf_u8 wirelessmode)
{
    p2p_info_st *p2p_info           = pnic_info->p2p;
    wf_u8 *supported_rates = NULL;
    wf_u8 support_rate_cnt = 0;
    wf_u16 basic_dr_cfg;
    wf_s32 ret = 0;
    if (P2P_STATE_NONE !=p2p_info->p2p_state)
    {
        return;
    }

    if ((wirelessmode & WIRELESS_11B) && (wirelessmode == WIRELESS_11B))
    {
        supported_rates =  wl_basic_rate_cck;
        support_rate_cnt = sizeof(wl_basic_rate_cck)/sizeof(wl_basic_rate_cck[0]);
    }
    else if (wirelessmode & WIRELESS_11B)
    {
        supported_rates =  wl_basic_rate_mix;
        support_rate_cnt = sizeof(wl_basic_rate_mix)/sizeof(wl_basic_rate_mix[0]);
    }
    else
    {
        supported_rates =  wl_basic_rate_ofdm;
        support_rate_cnt = sizeof(wl_basic_rate_ofdm)/sizeof(wl_basic_rate_ofdm[0]);
    }

    if (wirelessmode & WIRELESS_11B)
    {
        p2p_info->mgnt_tx_rate = IEEE80211_CCK_RATE_1MB;
    }
    else
    {
        p2p_info->mgnt_tx_rate = IEEE80211_OFDM_RATE_6MB;
    }

    get_bratecfg_by_support_dates(supported_rates, support_rate_cnt, &basic_dr_cfg);
    ret = wf_mcu_set_basic_rate(pnic_info,  basic_dr_cfg);
    if(WF_RETURN_OK != ret)
    {
        P2P_WARN("wf_mcu_set_basic_rate failed");
    }
}


static void p2p_info_init(p2p_info_st *p2p_info)
{
    /*  Use the OFDM rate in the P2P probe response frame. ( 6(B), 9(B), 12, 18, 24, 36, 48, 54 )    */
    p2p_info->p2p_support_rate[0] = 0x8c;    /*  6(B) */
    p2p_info->p2p_support_rate[1] = 0x92;    /*  9(B) */
    p2p_info->p2p_support_rate[2] = 0x18;    /*  12 */
    p2p_info->p2p_support_rate[3] = 0x24;    /*  18 */
    p2p_info->p2p_support_rate[4] = 0x30;    /*  24 */
    p2p_info->p2p_support_rate[5] = 0x48;    /*  36 */
    p2p_info->p2p_support_rate[6] = 0x60;    /*  48 */
    p2p_info->p2p_support_rate[7] = 0x6c;    /*  54 */

    p2p_info->p2p_state = P2P_STATE_NONE;
    wf_memcpy((void *)p2p_info->p2p_wildcard_ssid, "DIRECT-", 7);

    p2p_info->find_phase_state_exchange_cnt = P2P_FINDPHASE_EX_NONE;
    wf_memset(&p2p_info->wfd_info,0,sizeof(struct wifi_display_info));
    p2p_info->supported_wps_cm = WPS_CONFIG_METHOD_DISPLAY | WPS_CONFIG_METHOD_PBC | WPS_CONFIG_METHOD_KEYPAD;
    p2p_info->ext_listen_interval    = 1000;

    wf_widev_invit_info_init(&p2p_info->invit_info);
    wf_widev_nego_info_init(&p2p_info->nego_info);


}

wf_s32 wf_p2p_init(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info = NULL;

    P2P_INFO("start");
    if(NULL != pnic_info->p2p)
    {
        P2P_WARN("pnic_info->p2p is not null");
        return 0;
    }

    p2p_info = wf_kzalloc(sizeof(p2p_info_st));
    if (p2p_info == NULL)
    {
        P2P_WARN("malloc p2p_info_st failed");
        return WF_RETURN_FAIL;
    }

    pnic_info->p2p = p2p_info;
    p2p_info->nic_info = pnic_info;
    wf_memcpy(p2p_info->device_addr,nic_to_local_addr(pnic_info),WF_ETH_ALEN);
    wf_memcpy(p2p_info->interface_addr,nic_to_local_addr(pnic_info),WF_ETH_ALEN);
    p2p_proto_mgt_init(p2p_info);
    p2p_info_init(p2p_info);
    p2p_info->p2p_enabled = wf_false;
    p2p_info->full_ch_in_p2p_handshake = wf_false;
    
    if (wifi_display_info_to_init_func(pnic_info, 1) != wf_true)
    {
        P2P_WARN("\n Can't init init_wifi_display_info\n");
    }
    return 0;

}



wf_s32 wf_p2p_term(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info = NULL;

    P2P_INFO("start");
    p2p_info = pnic_info->p2p;
    if(NULL == p2p_info)
    {
        return 0;
    }

    p2p_proto_mgt_term(p2p_info);
    wf_kfree(p2p_info);
    p2p_info = NULL;
    return 0;
}



wf_inline void wf_p2p_set_role(p2p_info_st *p2p_info, enum P2P_ROLE role)
{
    if (p2p_info->role != role)
    {
        p2p_info->role = role;
    }
}
wf_inline P2P_ROLE p2p_get_role(p2p_info_st *p2p_info)
{
    return p2p_info->role;
}

wf_inline void wf_p2p_set_state(p2p_info_st *p2p_info, enum P2P_STATE state)
{
    if (p2p_info->p2p_state != state)
    {
        p2p_info->p2p_state = state;
    }
}


wf_inline void wf_p2p_set_pre_state(p2p_info_st *p2p_info, enum P2P_STATE state)
{
    if (p2p_info->pre_p2p_state != state)
    {
        p2p_info->pre_p2p_state = state;
    }
}


static wf_s32 p2p_scan_ie_parse(nic_info_st *pnic_info, char *buf, wf_u32 len)
{
    wf_s32 ret = 0;
    wf_u32 wps_ielen = 0;
    wf_u8 *wps_ie = NULL;
    wf_u32 p2p_ielen = 0;
    wf_u8 *p2p_ie   = NULL;
    
    wf_u32 wfd_ielen = 0;
    wf_u8 *wfd_ie;
    
    mlme_info_t *pmlme_info = (mlme_info_t *)pnic_info->mlme_info;
    p2p_info_st *p2p_info    = pnic_info->p2p;
    
    P2P_DBG("start! ielen = %d", len);

    if(len > 0)
    {
        if((wps_ie = wf_wlan_get_wps_ie((wf_u8 *)buf, len, NULL, &wps_ielen)))
        {
            P2P_DBG("probereq_wps_ie_len : %d", wps_ielen);
            if(pmlme_info->probereq_wps_ie)
            {
                wf_kfree(pmlme_info->probereq_wps_ie);
                pmlme_info->probereq_wps_ie= NULL;
                pmlme_info->wps_ie_len = 0;
            }

            pmlme_info->probereq_wps_ie = wf_kzalloc(wps_ielen);
            if(pmlme_info->probereq_wps_ie == NULL)
            {
                P2P_DBG("wf_kzmalloc probereq_wps_ie failed");
                return -1;
            }
            wf_memcpy(pmlme_info->probereq_wps_ie, wps_ie, wps_ielen);
            pmlme_info->wps_ie_len = wps_ielen;
        }

        if((p2p_ie = wf_p2p_get_ie((wf_u8 *)buf, len, NULL, &p2p_ielen)))
        {
            wf_u32 attr_contentlen = 0;
            wf_u8 listen_ch_attr[5];
            
            P2P_DBG("probereq p2p_ielen : %d", p2p_ielen);
            wf_p2p_dump_attrs(p2p_ie,p2p_ielen);
            wf_p2p_parse_p2pie(pnic_info, p2p_ie, p2p_ielen, WF_P2P_IE_PROBE_REQ);
            if (wf_p2p_get_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_LISTEN_CH, (wf_u8 *) listen_ch_attr, (wf_u32 *) & attr_contentlen)
                && attr_contentlen == 5)
            {
                if (p2p_info->listen_channel != listen_ch_attr[4])
                {
                    P2P_DBG(" listen channel - country:%c%c%c, class:%u, ch:%u\n",
                          listen_ch_attr[0],
                          listen_ch_attr[1], listen_ch_attr[2],
                          listen_ch_attr[3], listen_ch_attr[4]);
                    p2p_info->listen_channel = listen_ch_attr[4];
                }
            }
        }

        wfd_ie = wfd_ie_to_get_func(1, (wf_u8 *)buf, len, NULL, &wfd_ielen);
        if(wfd_ie)
        {
            P2P_DBG("probe_req_wfdielen=%d",wfd_ielen);
            p2p_info->wfd_info.wfd_enable = wf_true;
            if (wf_p2p_wfd_update_ie(pnic_info, WF_WFD_IE_PROBE_REQ, wfd_ie, wfd_ielen, 1) != wf_true)
            {
                return -1;
            }
            
            
        }

    }
    /* this func is mainly build for p2p*/

    return ret;
}

wf_s32 wf_p2p_scan_entry(nic_info_st *pnic_info,wf_u8 social_channel,wf_u8 *ies,wf_s32 ieslen)
 {
     p2p_info_st *p2p_info = pnic_info->p2p;

     if (p2p_info->p2p_state == P2P_STATE_NONE)
     {
         wf_p2p_enable(pnic_info, P2P_ROLE_DEVICE);
     }
     else
     {
         wf_p2p_set_pre_state(p2p_info, p2p_info->p2p_state);
     }
     
     P2P_DBG("%s",wf_p2p_state_to_str(p2p_info->p2p_state) );
     wf_p2p_set_state(p2p_info, P2P_STATE_LISTEN);

     if (p2p_info->p2p_state!=P2P_STATE_NONE && p2p_info->p2p_state != P2P_STATE_IDLE )
     {
         wf_p2p_set_state(p2p_info, P2P_STATE_FIND_PHASE_SEARCH);
         //do_network_queue_unnew(pwadptdata, _TRUE, 1);
         if (social_channel == 0)
         {
             p2p_info->find_phase_state_exchange_cnt = P2P_FINDPHASE_EX_NONE;

         }
         else
         {
             p2p_info->find_phase_state_exchange_cnt = P2P_FINDPHASE_EX_SOCIAL_LAST;
         }
     }

     /*p2p wps ie get func*/
    if(ies && ieslen > 0)
    {
        p2p_scan_ie_parse(pnic_info, (char *)ies, ieslen);
    }

    return 0;
 }


wf_s32 wf_p2p_connect_entry(nic_info_st *pnic_info,wf_u8 *ie, wf_u32 ie_len)
{

    wf_u32 p2p_ielen = 0;
    wf_u8 *p2p_ie = NULL;
    wf_u32 wfd_ielen = 0;
    wf_u8 *wfd_ie = NULL;
    
    p2p_ie = wf_p2p_get_ie(ie, ie_len, NULL, &p2p_ielen);
    if(NULL == p2p_ie)
    {
        return -1;
    }
   
    P2P_INFO("p2p_assoc_req_ielen=%d\n",p2p_ielen);
    wf_p2p_dump_attrs(p2p_ie,p2p_ielen);
    wf_p2p_parse_p2pie(pnic_info, p2p_ie, p2p_ielen, WF_P2P_IE_ASSOC_REQ);

    wfd_ie = wfd_ie_to_get_func(1, ie, ie_len, NULL, &wfd_ielen);
    if (wfd_ie)
    {
        P2P_INFO(" wfd_assoc_req_ielen=%d\n", wfd_ielen);
        if (wf_p2p_wfd_update_ie(pnic_info, WF_WFD_IE_ASSOC_REQ, wfd_ie, wfd_ielen,1) != wf_true)
        {
            P2P_INFO("wfd_assoc_req_ie update failed\n");
        }
    }

    return 0;
}

wf_s32 wf_p2p_enable(nic_info_st *pnic_info,P2P_ROLE role)
{

    wf_s32 ret = WF_RETURN_OK;
    p2p_info_st *p2p_info = pnic_info->p2p;
    nic_info_st *buddy_nic                 = pnic_info->buddy_nic;
    wf_wlan_mgmt_info_t *pother_wlan_info        = NULL;
    wf_wlan_network_t *pother_cur_network   = NULL;
    p2p_info_st *other_p2p_info = NULL;
    wf_bool other_bconnect= wf_false;


    P2P_INFO("%s",wf_p2p_role_to_str(role));
    if (role == P2P_ROLE_DEVICE || role == P2P_ROLE_CLIENT || role == P2P_ROLE_GO)
    {

        if(buddy_nic)
        {
            other_p2p_info      = buddy_nic->p2p;
            pother_wlan_info    = buddy_nic->wlan_mgmt_info;
            pother_cur_network  = &pother_wlan_info->cur_network;
            
            if (other_p2p_info->p2p_state != P2P_STATE_NONE)
            {
                return ret;
            }
            wf_mlme_get_connect(buddy_nic,&other_bconnect);
            if (other_bconnect == wf_true &&
                (1 == pother_cur_network->channel || 6 == pother_cur_network->channel|| 11 == pother_cur_network->channel))
            {
                p2p_info->listen_channel = pother_cur_network->channel;
            }
        }
        else 
        {
            p2p_info->listen_channel = 11;
        }

        p2p_info->p2p_enabled = wf_true;
        p2p_do_renew_tx_rate(pnic_info, WIRELESS_11G_24N);
        
        if (role == P2P_ROLE_DEVICE)
        {
            wf_p2p_set_role(p2p_info, P2P_ROLE_DEVICE);
            if (other_bconnect == wf_true)
            {
                wf_p2p_set_state(p2p_info, P2P_STATE_IDLE);
            }
            else
            {
                wf_p2p_set_state(p2p_info, P2P_STATE_LISTEN);
            }
            
            p2p_info->intent = 1;
            wf_p2p_set_pre_state(p2p_info, P2P_STATE_LISTEN);
        }
        else if (role == P2P_ROLE_CLIENT)
        {
            wf_p2p_set_role(p2p_info, P2P_ROLE_CLIENT);
            wf_p2p_set_state(p2p_info, P2P_STATE_GONEGO_OK);
            p2p_info->intent = 1;
            wf_p2p_set_pre_state(p2p_info, P2P_STATE_GONEGO_OK);
        }
        else if (role == P2P_ROLE_GO)
        {
            wf_p2p_set_role(p2p_info, P2P_ROLE_GO);
            wf_p2p_set_state(p2p_info, P2P_STATE_GONEGO_OK);
            p2p_info->intent = 15;
            wf_p2p_set_pre_state(p2p_info, P2P_STATE_GONEGO_OK);
        }

        wf_mcu_msg_body_sync(pnic_info,HAL_MSG_P2P_STATE,wf_true);

        if(wf_p2p_wfd_is_valid(pnic_info))
        {
            p2p_info->supported_wps_cm = WPS_CONFIG_METHOD_DISPLAY | WPS_CONFIG_METHOD_PBC;
            wf_mcu_msg_body_sync(pnic_info,HAL_MSG_WIFI_DISPLAY_STATE,wf_true);
        }
    }

    return ret;

}

wf_s32 wf_p2p_disable(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info = pnic_info->p2p;

    p2p_info->p2p_enabled = wf_false;

    if(p2p_info->p2p_state != P2P_STATE_NONE)
    {
        wf_p2p_set_state(p2p_info, P2P_STATE_NONE);
        wf_p2p_set_pre_state(p2p_info, P2P_STATE_NONE);
        wf_p2p_set_role(p2p_info, P2P_ROLE_DISABLE);
    }

    wf_mcu_msg_body_sync(pnic_info,HAL_MSG_P2P_STATE,wf_false);
    
    if(wf_p2p_wfd_is_valid(pnic_info))
    {
        wf_mcu_msg_body_sync(pnic_info,HAL_MSG_WIFI_DISPLAY_STATE,wf_false);
    }
#if 0
    if (_FAIL == wl_pwr_wakeup(pwadptdata)) {
    ret = _FAIL;
    goto exit;
    }
#endif
    p2p_do_renew_tx_rate(pnic_info, WIRELESS_11BG_24N);

    return 0;
}

wf_bool wf_p2p_is_valid(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info   = pnic_info->p2p;

    if(p2p_info)
    {
        if(P2P_STATE_NONE != p2p_info->p2p_state)
        {
            return wf_true;
        }
    }

    return wf_false;
}


/* do not use the beacon ie of this func parse */
static int p2p_beacon_parse(nic_info_st *pnic_info, char *buf,int len)
{
    wf_u8 *wps_ie       = NULL;
    wf_u32 wps_ielen    = 0;
    wf_u8 *wfd_ie       = NULL;
    wf_u32 wfd_ielen    = 0;

    wf_u8 *p2p_ie       = NULL;
    wf_u32 p2p_ielen    = 0;
    mlme_info_t *pmlme_info = (mlme_info_t *)pnic_info->mlme_info;

    P2P_DBG("start set beacon wps p2pie! ielen = %d",len);

    if(len <= 0)
     {
        return -1;
     }

    if ((wps_ie = wf_wlan_get_wps_ie((wf_u8 *)buf, len, NULL, &wps_ielen)))
    {
        P2P_DBG("bcn_wps_ielen=%d\n", wps_ielen);
        if(pmlme_info->wps_beacon_ie)
        {
            wf_kfree(pmlme_info->wps_beacon_ie);
            pmlme_info->wps_beacon_ie = NULL;
            pmlme_info->wps_beacon_ie_len = 0;
        }
        pmlme_info->wps_beacon_ie = wf_kzalloc(wps_ielen);
        if(pmlme_info->wps_beacon_ie == NULL)
        {
            P2P_WARN("wps_beacon_ie wf_kzalloc failed! ");
            return -1;
        }
        wf_memcpy(pmlme_info->wps_beacon_ie, wps_ie, wps_ielen);
        pmlme_info->wps_beacon_ie_len = wps_ielen;
        #ifdef CONFIG_IOCTL_CFG80211
        #ifdef CFG_ENABLE_AP_MODE
        update_beacon(pnic_info, WF_80211_MGMT_EID_VENDOR_SPECIFIC, WPS_OUI,wf_true);
        #endif
        #endif
    }
    
    if((p2p_ie = wf_p2p_get_ie((wf_u8 *)buf, len, NULL, &p2p_ielen)))
    {
         P2P_DBG("bcn_p2p_ielen=%d\n", p2p_ielen);
         wf_p2p_dump_attrs(p2p_ie,p2p_ielen);
         wf_p2p_parse_p2pie(pnic_info, p2p_ie, p2p_ielen, WF_P2P_IE_BEACON);
    }


    if(wf_p2p_wfd_is_valid(pnic_info))
    {
        wfd_ie = wfd_ie_to_get_func(1, (wf_u8 *)buf, len, NULL, &wfd_ielen);
        if (wfd_ie) 
        {
            P2P_DBG("bcn_wfd_ielen=%d\n", wfd_ielen);

            if (wf_p2p_wfd_update_ie(pnic_info, WF_WFD_IE_BEACON, wfd_ie, wfd_ielen, 1) != wf_true)
            {
                return -1;
            }
        }
    }

   return 0;
}

static wf_bool st_check_nic_state(nic_info_st *pnic_info, wf_u32 check_state)
{
    wf_bool ret = wf_false;
    if (pnic_info->nic_state & check_state)
    {
        ret = wf_true;
    }
    return ret;
}


static int p2p_probe_rsp_parse(nic_info_st *pnic_info, char *buf, int len)
{
    int ret = 0;
    wf_u32 wps_ielen    = 0;
    wf_u8 *wps_ie       = NULL;
    wf_u32 p2p_ielen    = 0;
    wf_u8 *p2p_ie           = NULL;
    p2p_info_st *p2p_info    = pnic_info->p2p;
    wf_u32 wfd_ielen = 0;
    wf_u8 *wfd_ie    = NULL;
    mlme_info_t *pmlme_info = (mlme_info_t *)pnic_info->mlme_info;

    P2P_DBG(" ielen=%d\n", len);
    //P2P_ARRAY(buf,  len);
    if (len > 0)
    {
        if ((wps_ie = wf_wlan_get_wps_ie((wf_u8 *)buf, len, NULL, &wps_ielen)))
        {
            wf_u32 attr_contentlen = 0;
            wf_u16 uconfig_method, *puconfig_method = NULL;


            P2P_INFO("probe_resp_wps_ielen=%d\n", wps_ielen);

            if (st_check_nic_state(pnic_info, WIFI_UNDER_WPS))
            {
                wf_u8 sr = 0;
                wf_wlan_get_wps_attr_content(1, wps_ie, wps_ielen, WPS_ATTR_SELECTED_REGISTRAR, (wf_u8 *) (&sr), NULL);

                if (sr != 0)
                {
                    P2P_INFO("got sr\n");
                }
                else
                {
                    P2P_INFO("GO mode process WPS under site-survey,  sr no set\n");
                    return ret;
                }
            }

            if (pmlme_info->wps_probe_resp_ie)
            {
                pmlme_info->wps_probe_resp_ie_len = 0;
                wf_kfree(pmlme_info->wps_probe_resp_ie);
                pmlme_info->wps_probe_resp_ie = NULL;
            }

            pmlme_info->wps_probe_resp_ie = wf_kzalloc(wps_ielen);
            if (pmlme_info->wps_probe_resp_ie == NULL)
            {
                P2P_INFO("wps_probe_resp_ie kzalloc() ERROR!\n");
                return -1;

            }

            if ((puconfig_method =
                     (wf_u16 *) wf_wlan_get_wps_attr_content(1, wps_ie, wps_ielen,
                             WPS_ATTR_CONF_METHOD, NULL,
                             &attr_contentlen)) != NULL)
            {
                if (wf_p2p_is_valid(pnic_info))
                {
                    if(p2p_info->role == P2P_ROLE_GO)
                    {
                        uconfig_method = WPS_CM_PUSH_BUTTON;
                        uconfig_method = wf_cpu_to_be16(uconfig_method);
                        *puconfig_method &= ~uconfig_method;
                    }
                }
            }

            wf_memcpy(pmlme_info->wps_probe_resp_ie, wps_ie, wps_ielen);
            pmlme_info->wps_probe_resp_ie_len = wps_ielen;

        }

        if ((p2p_ie = wf_p2p_get_ie((wf_u8 *)buf, len, NULL, &p2p_ielen)))
        {
            wf_u8 is_GO = wf_false;
            wf_u32 attr_contentlen = 0;
            wf_u16 cap_attr = 0;

            P2P_INFO("probe_resp_p2p_ielen=%d\n", p2p_ielen);
            wf_p2p_dump_attrs(p2p_ie,p2p_ielen);
            if (wf_p2p_get_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_CAPABILITY, (wf_u8 *) & cap_attr, (wf_u32 *) & attr_contentlen))
            {
                wf_u8 grp_cap = 0;
                cap_attr = wf_le16_to_cpu(cap_attr);
                grp_cap = (wf_u8) ((cap_attr >> 8) & 0xff);

                is_GO = (grp_cap & BIT(0)) ? wf_true : wf_false;

                if (is_GO)
                {
                    P2P_INFO("Got P2P Capability Attr, grp_cap=0x%x, is_GO\n",grp_cap);
                }
            }

            wf_p2p_parse_p2pie(pnic_info, p2p_ie, p2p_ielen, WF_P2P_IE_PROBE_RSP);

        }
        //P2P_ARRAY(p2p_info->p2p_ie[WF_P2P_IE_PROBE_RSP],p2p_info->p2p_ie_len[WF_P2P_IE_PROBE_RSP]);

        if(wf_p2p_wfd_is_valid(pnic_info))
        {
            wfd_ie = wfd_ie_to_get_func(1, (wf_u8 *)buf, len, NULL, &wfd_ielen);
            P2P_DBG("probe_resp_wfd_ielen=%d , wfd_ie=%p\n", wfd_ielen,wfd_ie);
            if (wfd_ie) 
            {
                if (wf_p2p_wfd_update_ie(pnic_info, WF_WFD_IE_PROBE_RSP, wfd_ie, wfd_ielen,  1) != wf_true)
                {
                    return -1;
                }
            }
        }
    }

    return ret;

}


static int p2p_assoc_rsp_parse(nic_info_st *pnic_info, char *buf, int len)
{
    wf_u8 *wps_ie = NULL;
    wf_u32 wps_ielen = 0;
    wf_u8 *p2p_ie = NULL;
    wf_u32 p2p_ielen = 0;
    wf_u8 *wfd_ie = NULL;
    wf_u32 wfd_ielen = 0;

    mlme_info_t *pmlme_info = (mlme_info_t *)pnic_info->mlme_info;

    if(len <= 0)
    {
        return -1;
    }
    if((wps_ie = wf_wlan_get_wps_ie((wf_u8 *)buf, len, NULL, &wps_ielen)))
    {
        P2P_DBG("assoc_resp_wps_ie_len : %d", wps_ielen);
        if(pmlme_info->wps_assoc_resp_ie)
        {
            wf_kfree(pmlme_info->wps_assoc_resp_ie);
            pmlme_info->wps_assoc_resp_ie = NULL;
            pmlme_info->wps_assoc_resp_ie_len = 0;
        }
        pmlme_info->wps_assoc_resp_ie = wf_kzalloc(wps_ielen);
        if(pmlme_info->wps_assoc_resp_ie == NULL)
        {
            P2P_WARN("wf_kzalloc wps_assoc_resp_ie failed!");
            return -1;
        }
        wf_memcpy(pmlme_info->wps_assoc_resp_ie, wps_ie, wps_ielen);
        pmlme_info->wps_assoc_resp_ie_len = wps_ielen;
    }

    if((p2p_ie = wf_p2p_get_ie((wf_u8 *)buf, len, NULL, &p2p_ielen)))
    {
        P2P_DBG("assoc_resp_p2p_ie_len : %d", p2p_ielen);
        wf_p2p_dump_attrs(p2p_ie,p2p_ielen);
        wf_p2p_parse_p2pie(pnic_info, p2p_ie, p2p_ielen, WF_P2P_IE_ASSOC_RSP);
    }

    if(wf_p2p_wfd_is_valid( pnic_info))
    {
        wfd_ie = wfd_ie_to_get_func(1, (wf_u8 *)buf, len, NULL, &wfd_ielen);
        if (wf_p2p_wfd_update_ie(pnic_info, WF_WFD_IE_ASSOC_RSP, wfd_ie, wfd_ielen, 1) != wf_true)
        {
            return -1;
        }
    }

    return 0;
}


wf_s32 wf_p2p_parse_ie(nic_info_st *pnic_info, wf_u8 *buf, wf_s32 len, wf_s32 type)
{
    wf_s32 ret = 0;
    wf_u32 wps_ielen = 0;
    wf_u32 p2p_ielen = 0;
    
    P2P_DBG(" ielen=%d\n", len);
    if ((wf_wlan_get_wps_ie(buf, len, NULL, &wps_ielen) && (wps_ielen > 0))
        || (wf_p2p_get_ie(buf, len, NULL, &p2p_ielen) && (p2p_ielen > 0))
       )
    {
        if (pnic_info != NULL)
        {
            switch (type)
            {
                case 0x1:
                    ret = p2p_beacon_parse(pnic_info, (char *)buf, len);
                    break;
                case 0x2:
                    ret = p2p_probe_rsp_parse(pnic_info, (char *)buf, len);
                    break;
                case 0x4:
                    ret = p2p_assoc_rsp_parse(pnic_info, (char *)buf, len);
                    break;
            }
        }
    }

    return ret;

}

