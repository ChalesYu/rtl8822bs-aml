/*
 * p2p_wfd.c
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

#if 0
#define WFD_DBG(fmt, ...)      LOG_D("WFD[%s:%d]"fmt, __func__,__LINE__, ##__VA_ARGS__)
#define WFD_ARRAY(data, len)   log_array(data, len)
#else
#define WFD_DBG(fmt, ...)
#define WFD_ARRAY(data, len)
#endif
#define WFD_INFO(fmt, ...)     LOG_I("WFD[%s:%d]"fmt, __func__,__LINE__, ##__VA_ARGS__)
#define WFD_WARN(fmt, ...)     LOG_E("WFD[%s:%d]"fmt, __func__,__LINE__, ##__VA_ARGS__)

int wf_p2p_wfd_init(nic_info_st *pnic_info, wf_u8 flag)
{
    int res = wf_true;
    wf_u8 i=0;
    p2p_info_st *p2p_info = pnic_info->p2p;
    wfd_info_st *pwfd_info = &p2p_info->wfd_info;

    for(i=0;i<WF_WFD_IE_MAX;i++)
    {
        pwfd_info->wfd_ie[i] = wf_kzalloc(P2P_IE_BUF_LEN);
        if(NULL == pwfd_info->wfd_ie[i])
        {
            WFD_WARN("wf_kzalloc p2p_ie failed");
            return WF_RETURN_FAIL;
        }
    }
    
    pwfd_info->init_rtsp_ctrlport = 554;


    pwfd_info->rtsp_ctrlport = 0;
  

    pwfd_info->peer_rtsp_ctrlport = 0;
    pwfd_info->wfd_enable = wf_false;
    pwfd_info->wfd_device_type = WFD_DEVINFO_PSINK;
    pwfd_info->scan_result_type = SCAN_RESULT_P2P_ONLY;

    pwfd_info->peer_session_avail = wf_true;
    pwfd_info->wfd_pc = wf_false;
    
    if (flag) 
    {
        wf_memset(pwfd_info->ip_address, 0x00, 4);
        wf_memset(pwfd_info->peer_ip_address, 0x00, 4);
    }

    return res;

}

void wf_p2p_wfd_enable(nic_info_st *pnic_info, wf_bool on)
{

    p2p_info_st *p2p_info = pnic_info->p2p;
    wfd_info_st *pwfd_info = &p2p_info->wfd_info; 
    if (on) 
    {
        pwfd_info->rtsp_ctrlport = pwfd_info->init_rtsp_ctrlport;
        pwfd_info->wfd_enable = wf_true;

    } 
    else 
    {
        pwfd_info->wfd_enable = wf_false;
        pwfd_info->rtsp_ctrlport = 0;
    }
 
}

void wf_p2p_wfd_set_ctrl_port(nic_info_st *pnic_info, wf_u16 port)
{

    p2p_info_st *p2p_info = pnic_info->p2p;
    wfd_info_st *pwfd_info = &p2p_info->wfd_info;

    pwfd_info->init_rtsp_ctrlport = port;
    if(pwfd_info->wfd_enable == wf_true)
    {
        pwfd_info->rtsp_ctrlport = port;
    }

}



wf_u8 *wf_p2p_wfd_get_ie(wf_u8 flag, wf_u8 * in_ie, int in_len, wf_u8 * wfd_ie, wf_u32 * wfd_ielen)
{
    wf_u32 cnt = 0;
    wf_u8 *wfd_ie_ptr = NULL;
    wf_u8 eid;

    if (flag) 
    {
        if (wfd_ielen)
            *wfd_ielen = 0;
    }
    if (!in_ie || in_len <= 0) 
    {
        WFD_WARN("in_ie=NULL");
        return wfd_ie_ptr;
    }

    while (cnt + 1 + 4 < in_len) 
    {
        eid = in_ie[cnt];
        /* cnt + 1 is the bit of length, + 4 means that atlest include wfdoui */
        if (cnt + 1 + 4 >= MAX_IE_SZ)
        {
            WFD_WARN(" NOT FIND WFD IE");
            return NULL;
        }
        
        if (eid == WF_80211_MGMT_EID_VENDOR_SPECIFIC && wf_memcmp(&in_ie[cnt + 2], WFD_OUI, 4) == 0) 
        {
            WFD_DBG("find wdf ie");   
            wfd_ie_ptr = in_ie + cnt;

            if (wfd_ie)
            {
                wf_memcpy(wfd_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);
            }

            if (wfd_ielen)
            {
                *wfd_ielen = in_ie[cnt + 1] + 2;
            }

            break;
        } 
        else 
        {
            cnt += in_ie[cnt + 1] + 2;
        }

    }

    return wfd_ie_ptr;
}

static wf_u8 *wfd_get_attr(wf_u8 * wfd_ie, wf_u32 wfd_ielen, wf_u8 target_attr_id,
                     wf_u8 * buf_attr, wf_u32 * len_attr, wf_u8 flag)
{
    wf_u8 *attr_ptr = NULL;
    wf_u8 *target_attr_ptr = NULL;

    if (flag && len_attr) 
    {
       *len_attr = 0;
    }
    
    if (!wfd_ie || wfd_ielen <= 6 || (wfd_ie[0] != WF_80211_MGMT_EID_VENDOR_SPECIFIC)
        || (wf_memcmp(wfd_ie + 2, WFD_OUI, 4) != wf_true))
    {
        return attr_ptr;
    }

    attr_ptr = wfd_ie + 6;

    while ((attr_ptr - wfd_ie + 3) <= wfd_ielen) 
    {
        wf_u8 attr_id = *attr_ptr;
        wf_u16 attr_data_len = WF_GET_BE16(attr_ptr + 1);
        wf_u16 attr_len = attr_data_len + 3;

        WFD_INFO("%s attr_ptr:%p, id:%u, length:%u\n", __func__, attr_ptr,attr_id, attr_data_len);

        if ((attr_ptr - wfd_ie + attr_len) > wfd_ielen)
        {
            break;
        }

        if (attr_id == target_attr_id) 
        {
            target_attr_ptr = attr_ptr;

            if (buf_attr)
            {
                wf_memcpy(buf_attr, attr_ptr, attr_len);
            }

            if (len_attr)
            {
                *len_attr = attr_len;
            }

            break;
        } 
        else 
        {
            attr_ptr += attr_len;
        }
    }

    return target_attr_ptr;
}


wf_u8 *wfd_get_attr_content(wf_u8 * wfd_ie, wf_u32 wfd_ielen, wf_u8 target_attr_id,
                             wf_u8 * buf_content, wf_u32 * len_content, wf_u8 flag)
{
    wf_u8 *attr_ptr;
    wf_u32 attr_len;

    if (flag && len_content) 
    {
        *len_content = 0;
    }
    attr_ptr = wfd_get_attr(wfd_ie, wfd_ielen, target_attr_id, NULL, &attr_len, 1);

    if (attr_ptr && attr_len) 
    {
        if (buf_content)
        {
            wf_memcpy(buf_content, attr_ptr + 3, attr_len - 3);
        }

        if (len_content)
        {
            *len_content = attr_len - 3;
        }

        return attr_ptr + 3;
    }

    return NULL;
}

wf_u32 wfd_build_probe_req_ie(nic_info_st * pnic_info, wf_u8 * pbuf)
{
    wf_u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
    wf_u16 val16 = 0;
    wf_u32 len = 0, wfdielen = 0;
    p2p_info_st *p2p_info = pnic_info->p2p; 
    wfd_info_st *pwfd_info = &p2p_info->wfd_info;
    wdn_list *pwdn = (wdn_list *)pnic_info->wdn;
    wdn_net_info_st *pwdn_info = NULL;
    wdn_node_st *pwdn_node = NULL;

    wfdielen = 0;
    wfdie[wfdielen++] = 0x50;
    wfdie[wfdielen++] = 0x6F;
    wfdie[wfdielen++] = 0x9A;
    wfdie[wfdielen++] = 0x0A;

    wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

    WF_PUT_BE16(wfdie + wfdielen, 0x0006);
    wfdielen += 2;

    val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
    WF_PUT_BE16(wfdie + wfdielen, val16);
    wfdielen += 2;
    
    WF_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
    wfdielen += 2;

    WF_PUT_BE16(wfdie + wfdielen, 300);
    wfdielen += 2;

    wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

    WF_PUT_BE16(wfdie + wfdielen, 0x0006);
    wfdielen += 2;

    if (pwdn->cnt) 
    {
        pwdn_node = wf_list_entry(pwdn->head.pnext, wdn_node_st, list);
        pwdn_info = &pwdn_node->info;
        wf_memcpy(wfdie + wfdielen, &pwdn_info->bssid, WF_ETH_ALEN);
    } 
    else 
    {
        wf_memset(wfdie + wfdielen, 0x00, WF_ETH_ALEN);
    }

    wfdielen += WF_ETH_ALEN;

    wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

    WF_PUT_BE16(wfdie + wfdielen, 0x0007);
    wfdielen += 2;

    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;

    set_ie(pbuf, WF_80211_MGMT_EID_VENDOR_SPECIFIC, wfdielen, (unsigned char *)wfdie,&len);

    return len;
}


wf_u32 wfd_build_probe_resp_ie(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 tunneled, wf_u8 flag)
{
    wf_u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
    wf_u32 len = 0, wfdielen = 0;
    p2p_info_st *p2p_info = pnic_info->p2p;
    wfd_info_st *pwfd_info = &p2p_info->wfd_info;
    wdn_list *pwdn = (wdn_list *)pnic_info->wdn;
    wdn_net_info_st *pwdn_info = NULL;
    wdn_node_st *pwdn_node = NULL;

    wfdielen = 0;
    wfdie[wfdielen++] = 0x50;
    wfdie[wfdielen++] = 0x6F;
    wfdie[wfdielen++] = 0x9A;
    wfdie[wfdielen++] = 0x0A;

    wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

    WF_PUT_BE16(wfdie + wfdielen, 0x0006);
    wfdielen += 2;

    if (wf_true == p2p_info->session_available) 
    {
        if (P2P_ROLE_GO == p2p_info->role) {
            if (pwdn->cnt) {
                {
                    WF_PUT_BE16(wfdie + wfdielen,
                                 pwfd_info->
                                 wfd_device_type | WFD_DEVINFO_WSD |
                                 WFD_DEVINFO_HDCP_SUPPORT);
                }
            } else {
                {
                    WF_PUT_BE16(wfdie + wfdielen,
                                 pwfd_info->
                                 wfd_device_type | WFD_DEVINFO_SESSION_AVAIL |
                                 WFD_DEVINFO_WSD | WFD_DEVINFO_HDCP_SUPPORT);
                }
            }
        } else {
            {

                WF_PUT_BE16(wfdie + wfdielen,
                             pwfd_info->
                             wfd_device_type | WFD_DEVINFO_SESSION_AVAIL |
                             WFD_DEVINFO_WSD | WFD_DEVINFO_HDCP_SUPPORT);
            }
        }
    } else {
        {
            WF_PUT_BE16(wfdie + wfdielen,
                         pwfd_info->
                         wfd_device_type | WFD_DEVINFO_WSD |
                         WFD_DEVINFO_HDCP_SUPPORT);
        }

    }

    if (flag) {
        wfdielen += 2;
    }
    WF_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
    wfdielen += 2;

    WF_PUT_BE16(wfdie + wfdielen, 300);
    wfdielen += 2;

    wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

    WF_PUT_BE16(wfdie + wfdielen, 0x0006);
    wfdielen += 2;

    if (pwdn->cnt) 
    {
        pwdn_node = wf_list_entry(pwdn->head.pnext, wdn_node_st, list);
        pwdn_info = &pwdn_node->info;
        wf_memcpy(wfdie + wfdielen, &pwdn_info->bssid, WF_ETH_ALEN);
    } 
    else 
    {
        wf_memset(wfdie + wfdielen, 0x00, WF_ETH_ALEN);
    }

    wfdielen += WF_ETH_ALEN;

    wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

    WF_PUT_BE16(wfdie + wfdielen, 0x0007);
    wfdielen += 2;

    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;

    if (p2p_info->role == P2P_ROLE_GO) 
    {
        wfdie[wfdielen++] = WFD_ATTR_SESSION_INFO;
        WF_PUT_BE16(wfdie + wfdielen, 0x0000);
        wfdielen += 2;
    }
    
    set_ie(pbuf, WF_80211_MGMT_EID_VENDOR_SPECIFIC, wfdielen, (unsigned char *)wfdie,&len);

    return len;
}


static wf_u32 wfd_build_assoc_req_ie(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 flag)
{
    wf_u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
    wf_u16 val16 = 0;
    wf_u32 len = 0, wfdielen = 0;
    wfd_info_st *pwfd_info = NULL;    
    wdn_net_info_st *pwdn_info = NULL;
    wdn_node_st *pwdn_node = NULL;
    wdn_list *pwdn = (wdn_list *)pnic_info->wdn;
    p2p_info_st *p2p_info = pnic_info->p2p;

    pwfd_info = &p2p_info->wfd_info;
    

    if(p2p_info->p2p_state == P2P_STATE_NONE && p2p_info->p2p_state == P2P_STATE_IDLE)
        goto exit;

    wfdielen = 0;
    wfdie[wfdielen++] = 0x50;
    wfdie[wfdielen++] = 0x6F;
    wfdie[wfdielen++] = 0x9A;
    wfdie[wfdielen++] = 0x0A;

    wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

    WF_PUT_BE16(wfdie + wfdielen, 0x0006);
    wfdielen += 2;

    val16 =
        pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
    WF_PUT_BE16(wfdie + wfdielen, val16);
    wfdielen += 2;

    WF_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
    wfdielen += 2;

    WF_PUT_BE16(wfdie + wfdielen, 300);
    wfdielen += 2;

    wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

    WF_PUT_BE16(wfdie + wfdielen, 0x0006);
    wfdielen += 2;

    if (pwdn->cnt) {
        pwdn_node = wf_list_entry(pwdn->head.pnext, wdn_node_st, list);
        pwdn_info = &pwdn_node->info;
        wf_memcpy(wfdie + wfdielen, &pwdn_info->bssid, WF_ETH_ALEN);
    } else {
        wf_memset(wfdie + wfdielen, 0x00, WF_ETH_ALEN);
    }

    if (flag) {
        wfdielen += WF_ETH_ALEN;
    }
    wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

    WF_PUT_BE16(wfdie + wfdielen, 0x0007);
    wfdielen += 2;

    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;

    set_ie(pbuf, WF_80211_MGMT_EID_VENDOR_SPECIFIC, wfdielen, (unsigned char *)wfdie,
               &len);

exit:
    return len;
}


wf_u32 wfd_build_assoc_resp_ie(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 flag)
{
    wf_u8 wfdie[MAX_WFD_IE_LEN] = { 0x00 };
    wf_u32 len = 0, wfdielen = 0;
    wf_u16 val16 = 0;
    p2p_info_st *p2p_info = pnic_info->p2p;
    wfd_info_st *pwfd_info = &p2p_info->wfd_info;
    wdn_list *pwdn = (wdn_list *)pnic_info->wdn;
    wdn_net_info_st *pwdn_info = NULL;
    wdn_node_st *pwdn_node = NULL;

    wfdie[wfdielen++] = 0x50;
    wfdie[wfdielen++] = 0x6F;
    wfdie[wfdielen++] = 0x9A;
    wfdie[wfdielen++] = 0x0A;

    wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;

    WF_PUT_BE16(wfdie + wfdielen, 0x0006);
    wfdielen += 2;

    val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
    WF_PUT_BE16(wfdie + wfdielen, val16);
    wfdielen += 2;

    WF_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
    wfdielen += 2;

    WF_PUT_BE16(wfdie + wfdielen, 300);
    wfdielen += 2;

    wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;

    WF_PUT_BE16(wfdie + wfdielen, 0x0006);
    wfdielen += 2;

    if (pwdn->cnt) {
        pwdn_node = wf_list_entry(pwdn->head.pnext, wdn_node_st, list);
        pwdn_info = &pwdn_node->info;
        wf_memcpy(wfdie + wfdielen, &pwdn_info->bssid, WF_ETH_ALEN);

    } else {
        wf_memset(wfdie + wfdielen, 0x00, WF_ETH_ALEN);
    }

    if (flag) {
        wfdielen += WF_ETH_ALEN;
    }
    wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;

    WF_PUT_BE16(wfdie + wfdielen, 0x0007);
    wfdielen += 2;

    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;
    wfdie[wfdielen++] = 0;

    set_ie(pbuf, WF_80211_MGMT_EID_VENDOR_SPECIFIC, wfdielen, (unsigned char *)wfdie, &len);

    return len;
}


wf_u32 wf_p2p_wfd_append_probe_req_ie(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 flag)
{
    p2p_info_st *p2p_info               = pnic_info->p2p;
    wfd_info_st *wfd_info  = &p2p_info->wfd_info;
    
    wf_u32 len = 0;
    
    WFD_DBG("adding wdf probe_req ie wfd_enable =%d",wfd_info->wfd_enable);
    
    if (wfd_info->wfd_enable)
    {
        if ( wfd_info->wfd_ie[WF_WFD_IE_PROBE_REQ] && wfd_info->wfd_ie_len[WF_WFD_IE_PROBE_REQ] > 0) 
        {
            len = wfd_info->wfd_ie_len[WF_WFD_IE_PROBE_REQ];
            wf_memcpy(pbuf, wfd_info->wfd_ie[WF_WFD_IE_PROBE_REQ], len);
        }
        else 
        {   
            WFD_DBG("build wfd probe_req by self");
            len = wfd_build_probe_req_ie(pnic_info, pbuf);
        }

    }

    return len;
}


wf_u32 wf_p2p_wfd_append_probe_resp_ie(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 flag)
{
    p2p_info_st *p2p_info               = pnic_info->p2p;
    wfd_info_st *wfd_info  = &p2p_info->wfd_info;
    wf_u32 len = 0;

    if (wfd_info->wfd_enable)
    {
        if (wfd_info->wfd_ie[WF_WFD_IE_PROBE_RSP] && wfd_info->wfd_ie_len[WF_WFD_IE_PROBE_RSP] > 0) 
        {
            len = wfd_info->wfd_ie_len[WF_WFD_IE_PROBE_RSP];
            wf_memcpy(pbuf, wfd_info->wfd_ie[WF_WFD_IE_PROBE_RSP], len);
        }
        else
        {
            WFD_INFO("build wfd probe_resp by self");
            len = wfd_build_probe_resp_ie(pnic_info, pbuf, 0, 1);
        }

    }   

    WFD_DBG("adding wdf probe_resp ie len:%d",len);

    return len;
}


wf_u32 wf_p2p_wfd_append_assoc_req_ie(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 flag)
{
    p2p_info_st *p2p_info  = pnic_info->p2p;
    wfd_info_st *wfd_info = &p2p_info->wfd_info;
    
    wf_u32 len = 0;

    WFD_INFO("adding wdf assoc_req ie wfd_enable =%d",wfd_info->wfd_enable);

    if (p2p_info->wfd_info.wfd_enable)
    {
        if (wfd_info->wfd_ie[WF_WFD_IE_ASSOC_REQ] && wfd_info->wfd_ie_len[WF_WFD_IE_ASSOC_REQ] > 0) 
        {
            len = wfd_info->wfd_ie_len[WF_WFD_IE_ASSOC_REQ];
            wf_memcpy(pbuf, wfd_info->wfd_ie[WF_WFD_IE_ASSOC_REQ], len);
        }
        else 
        {
            WFD_INFO("build wfd assoc_req by self");
            len = wfd_build_assoc_req_ie(pnic_info, pbuf, 1);
        }

    }

    return len;
}

wf_u32 wf_p2p_wfd_append_assoc_resp_ie(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 flag)
{
    p2p_info_st *p2p_info               = pnic_info->p2p;
    wfd_info_st *wfd_info  = &p2p_info->wfd_info;
    wf_u32 len = 0;

    WFD_INFO("adding wdf assoc_resp ie wfd_enable =%d",wfd_info->wfd_enable);
    
    if (wfd_info->wfd_enable)
    {
        if (wfd_info->wfd_ie[WF_WFD_IE_ASSOC_RSP] && wfd_info->wfd_ie_len[WF_WFD_IE_ASSOC_RSP] > 0) 
        {
            len = wfd_info->wfd_ie_len[WF_WFD_IE_ASSOC_RSP];
            wf_memcpy(pbuf, wfd_info->wfd_ie[WF_WFD_IE_ASSOC_RSP], len);
        }
        else
        {
            WFD_INFO("build wfd assoc_resp by self");
            len = wfd_build_assoc_resp_ie(pnic_info, pbuf, 1);
        }

    }
    
    return len;
}

wf_s32 wf_p2p_wfd_update_ie(nic_info_st *pnic_info, WF_WFD_IE_E ie_type, wf_u8 * ie,wf_u32 ie_len, wf_u8 tag)
{
    p2p_info_st *p2p_info               = pnic_info->p2p;
    wfd_info_st *wfd_info  = &p2p_info->wfd_info;
    wf_u8 clear = 0;
    
    if (tag)
    {

        if (wfd_info->wfd_enable == wf_false)
        {
            return wf_true;
        }

        if (!ie && !ie_len)
        {
            clear = 1;
        }
        else if (!ie || !ie_len)
        {
            WFD_WARN(" type:%u, ie:%p, ie_len:%u", ie_type, ie, ie_len);
            return wf_false;
        }
        if(WF_WFD_IE_MAX <= ie_type)
        {
            WFD_WARN("unknown ie type:%d",ie_type);
        }
        
        if(NULL != wfd_info->wfd_ie[ie_type] &&  wfd_info->wfd_ie_len[ie_type] > 0)
        {
            wf_memset(wfd_info->wfd_ie[ie_type],0,P2P_IE_BUF_LEN);
            wfd_info->wfd_ie_len[ie_type] = 0;
        }

        if (!clear)
        {
            wf_memcpy(wfd_info->wfd_ie[ie_type], ie, ie_len);
            wfd_info->wfd_ie_len[ie_type] = ie_len;
        }

        if (wfd_info->wfd_ie[ie_type] && wfd_info->wfd_ie_len[ie_type])
        {
            wf_u8 *attr_content;
            wf_u32 attr_contentlen = 0;

            attr_content = wfd_get_attr_content(wfd_info->wfd_ie[ie_type], wfd_info->wfd_ie_len[ie_type],
                                             WFD_ATTR_DEVICE_INFO, NULL, &attr_contentlen, 1);
            if (attr_content && attr_contentlen)
            {
                if (WF_GET_BE16(attr_content + 2) != wfd_info->rtsp_ctrlport)
                {
                    wfd_info->rtsp_ctrlport = WF_GET_BE16(attr_content + 2);
                    WFD_WARN( " ie_type:%u, RTSP CTRL port = %u\n", ie_type, wfd_info->rtsp_ctrlport);
                }
            }
        }
    }
    
    return wf_true;

}


wf_bool wf_p2p_wfd_is_valid(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info = NULL;
    
    if(wf_p2p_is_valid(pnic_info))
    {
        p2p_info = pnic_info->p2p;
        return p2p_info->wfd_info.wfd_enable;
    }

    return wf_false;
}
