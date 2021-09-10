/*
 * p2p_proto_mgt.c
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
#define P2P_PROTO_DBG(fmt, ...)      LOG_D("P2P_PROTO[%s:%d][%d] "fmt, __func__,__LINE__, pnic_info->ndev_id,##__VA_ARGS__)
#define P2P_PROTO_ARRAY(data, len)   log_array(data, len)
#else
#define P2P_PROTO_DBG(fmt, ...)
#define P2P_PROTO_ARRAY(data, len)
#endif

#define P2P_PROTO_INFO(fmt, ...)     LOG_I("P2P_PROTO[%s:%d][%d] "fmt, __func__,__LINE__, pnic_info->ndev_id,##__VA_ARGS__)
#define P2P_PROTO_WARN(fmt, ...)     LOG_E("P2P_PROTO[%s:%d][%d] "fmt, __func__,__LINE__, pnic_info->ndev_id,##__VA_ARGS__)

 wf_s32 p2p_proto_cannel_remain_channel(nic_info_st *pnic_info)
{
    wf_u8 ch, bw, offset;
    p2p_info_st *p2p_info = pnic_info->p2p;
    
    ch = (0==p2p_info->restore_channel)?p2p_info->listen_channel : p2p_info->restore_channel;
    bw = CHANNEL_WIDTH_20;
    offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
    P2P_PROTO_INFO(" ch[%d] back to restore ch[%u], bw:%u, offset:%u\n",wf_wlan_get_cur_channel(pnic_info),ch, bw, offset);

    wf_wlan_set_cur_channel(pnic_info, ch);
    wf_hw_info_set_channnel_bw(pnic_info, ch, offset, bw);
    
    if(NULL != p2p_info->scb.remain_on_channel_expired)
    {
        p2p_info->scb.remain_on_channel_expired(pnic_info,NULL,0);
    }
    P2P_PROTO_DBG("end");
    return 0;
}

wf_s32 p2p_mgnt_nego_tx(nic_info_st *pnic_info, wf_u8 tx_ch, wf_u8 *buf, wf_s32 len)
{
    struct xmit_buf *pxmit_buf;
    tx_info_st *ptx_info;
    wf_80211_mgmt_t *pmgmt;

    // wf_lps_deny(pnic_info, 1000);
    wf_scan_wait_done(pnic_info, wf_true, 200);

    /* if (tx_ch != pmlmeext->cur_channel) { */
    
    wf_wlan_set_cur_channel(pnic_info, tx_ch);
    wf_hw_info_set_channnel_bw(pnic_info, tx_ch, CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

    /* alloc xmit_buf */
    ptx_info = (tx_info_st *)pnic_info->tx_info;
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if(pxmit_buf == NULL)
    {
        P2P_PROTO_WARN("pxmit_buf is NULL");
        return -1;
    }

    /* clear frame head(txd + 80211head) */
    wf_memset(pxmit_buf->pbuf, 0,
              TXDESC_OFFSET + WF_OFFSETOF(wf_80211_mgmt_t, beacon));

    /* set frame type */
    pmgmt = (void *)&pxmit_buf->pbuf[TXDESC_OFFSET];
    wf_memcpy(pmgmt, buf,len);
    P2P_PROTO_DBG("send");
    wf_nic_mgmt_frame_xmit(pnic_info, NULL, pxmit_buf, len);
    return 1;

}


wf_s32 wf_p2p_mgnt_nego(nic_info_st *pnic_info,void *param)
{
    p2p_msg_param_st *p2p_msg = param;
    wf_s32 type         = 0;
    wf_timer_t timer;
    wf_u32 dump_limit   = 8;
    wf_u32 dump_cnt     = 0;
    wf_s32 tx_ret       = 0;
    p2p_info_st *p2p_info = NULL;
    if(NULL == pnic_info ||NULL == param)
    {
        return -1;
    }

    p2p_info = pnic_info->p2p;
    P2P_PROTO_DBG("action:%d,tx_ch:%d,len:0x%x",p2p_info->action,p2p_info->operating_channel,p2p_msg->len);
    type = wf_p2p_check_frames(pnic_info,p2p_msg->u.buf,p2p_msg->len,wf_true,1);
    if(type < 0)
    {
        P2P_PROTO_WARN("wf_p2p_check_frames return %d",type);
        return -2;
    }

    wf_timer_set(&timer, 0);
    while(1)
    {
        wf_u32 sleep_ms = 0;
        dump_cnt++;
        tx_ret = p2p_mgnt_nego_tx(pnic_info, p2p_info->operating_channel, p2p_msg->u.buf, p2p_msg->len);
        if(WF_WLAN_ACTION_PUBLIC_GAS_INITIAL_REQ == p2p_info->action || 
            WF_WLAN_ACTION_PUBLIC_GAS_INITIAL_RSP == p2p_info->action)
        {
                sleep_ms = 50;
                wf_timer_mod(&timer, 500);
        }
        if (tx_ret == wf_true ||
            (dump_cnt >= dump_limit && wf_timer_expired(&timer)))
        {
            break;
        }
        if (sleep_ms > 0)
        {
            wf_msleep(sleep_ms);
        }
    }

    if (P2P_GO_NEGO_CONF == type)
    {

            P2P_PROTO_INFO("P2P_GO_NEGO_CONF");
            //do_clear_scan_deny(pwadptdata);
    }
    else if(P2P_INVIT_RESP == type)
    {
            p2p_info_st *p2p_info = pnic_info->p2p;
            wf_widev_invit_info_t *invit_info = &p2p_info->invit_info;
            P2P_PROTO_INFO("P2P_INVIT_RESP");
            if (invit_info->flags & BIT(0) && invit_info->status == 0)
            {
                P2P_PROTO_INFO(" agree with invitation of persistent group\n");
                //wl_scan_deny_set(pwadptdata, 5000);
                //wl_pwr_wakeup_ex(pwadptdata, 5000);
                //do_clear_scan_deny(pwadptdata);
            }
       
    }
    else
    {
    }
    
    P2P_PROTO_DBG();
    // wf_lps_deny_cancel(pnic_info,PS_DENY_MGNT_TX);

    return 0;
}
wf_s32 wf_p2p_cannel_remain_on_channel(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info   = pnic_info->p2p;

    if(wf_false == p2p_info->is_ro_ch)
    {
        return 0;
    }
    P2P_PROTO_DBG("cancel ro ch timer\n");
    p2p_info->is_ro_ch = wf_false;
    p2p_proto_cannel_remain_channel(pnic_info);
    wf_p2p_set_state(p2p_info, p2p_info->pre_p2p_state);
    P2P_PROTO_INFO("role:%s, state:%s",
        wf_p2p_role_to_str(p2p_info->role),wf_p2p_state_to_str(p2p_info->p2p_state));
    return 0;
}

wf_s32 wf_p2p_remain_on_channel(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info                   = NULL;
    CHANNEL_WIDTH cw = CHANNEL_WIDTH_20;
    HAL_PRIME_CH_OFFSET offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
    if(NULL == pnic_info)
    {
        return -1;
    }
    
    p2p_info                   = pnic_info->p2p;
    P2P_PROTO_DBG(" ch:%u duration:%d\n",p2p_info->remain_ch, p2p_info->ro_ch_duration);
    if (p2p_info->is_ro_ch == wf_true)
    {
        P2P_PROTO_INFO("cancel ro ch timer\n");
        wf_p2p_cannel_remain_on_channel(pnic_info);
    }
    
    if (p2p_info->p2p_state != P2P_STATE_LISTEN)
    {
        P2P_PROTO_INFO("role=%d, p2p_state=%d\n", p2p_info->role,p2p_info->p2p_state);
        wf_p2p_set_pre_state(p2p_info, p2p_info->p2p_state);
    }
    
    p2p_info->listen_channel = p2p_info->remain_ch;
    wf_p2p_set_state(p2p_info, P2P_STATE_LISTEN);

    P2P_PROTO_INFO("role=%d, p2p_state=%d  listen_channel=%d, channel:%d\n", 
            p2p_info->role,p2p_info->p2p_state,p2p_info->listen_channel,wf_wlan_get_cur_channel(pnic_info));
    
    wf_wlan_set_cur_channel(pnic_info, p2p_info->listen_channel);
    wf_hw_info_set_channnel_bw(pnic_info, p2p_info->listen_channel, cw, offset);

    p2p_info->is_ro_ch = wf_true;
    p2p_info->last_ro_ch_time = wf_os_api_timestamp();

    //wf_mlme_p2p_msg(pnic_info, WF_P2P_MSG_TAG_TIMER_RO_CH, &p2p_info->ro_ch_duration, sizeof(p2p_info->ro_ch_duration));
    
    return 0;
}

wf_pt_rst_t wf_p2p_core_thrd (wf_pt_t *pt, nic_info_st *pnic_info,void *param)
{
    p2p_info_st *p2p_info           = pnic_info->p2p;
    p2p_timer_st *p2p_timers        = &p2p_info->p2p_timers;
    p2p_msg_param_st *p2p_param = param;
    PT_BEGIN(pt);

    P2P_PROTO_DBG("tag:0x%x",p2p_param->tag);
    if (WF_P2P_MSG_TAG_TIMER_RO_CH == p2p_param->tag )
    {
        P2P_PROTO_INFO("WF_P2P_MSG_TAG_TIMER_RO_CH(%d)",p2p_param->u.duration);
        /* wait until timeout */
        wf_timer_set(&p2p_timers->remain_on_ch_timer, p2p_param->u.duration);
        
        PT_WAIT_UNTIL(pt,  wf_timer_expired(&p2p_timers->remain_on_ch_timer));
        P2P_PROTO_INFO("ro_ch timeout tag: 0x%x...",p2p_param->tag);
        wf_p2p_cannel_remain_on_channel(pnic_info);

    }
    else if (WF_P2P_MSG_TAG_NEGO == p2p_param->tag )
    {
        P2P_PROTO_DBG("WF_P2P_MSG_TAG_NEGO...");
        wf_p2p_mgnt_nego(pnic_info,p2p_param);
    }
    else if (WF_P2P_MSG_TAG_RO_CH == p2p_param->tag )
    {
        P2P_PROTO_INFO("WF_P2P_MSG_TAG_RO_CH...");
        wf_p2p_remain_on_channel(pnic_info);
        wf_p2p_proto_thrd_post(pnic_info);
    }
    else if (WF_P2P_MSG_TAG_RO_CH_CANNEL == p2p_param->tag )
    {
        P2P_PROTO_INFO("WF_P2P_MSG_TAG_RO_CH_CANNEL...");
        wf_p2p_cannel_remain_on_channel(pnic_info);
        wf_p2p_proto_thrd_post(pnic_info);
    }
    else
    {
        P2P_PROTO_DBG("unknown msg");
    }
    
    PT_END(pt);
}

wf_s32 p2p_proto_mgt_init(void *p2p)
{
    p2p_info_st *p2p_info           = NULL;
    p2p_proto_mgt_st *p2p_proto_mgt = NULL;
    nic_info_st *nic_info           = NULL;
    
    if(NULL == p2p)
    {
        LOG_E("[%s,%d] input param is null",__func__,__LINE__);
        return WF_RETURN_FAIL;
    }
    p2p_info = p2p;
    nic_info                        = p2p_info->nic_info;
    p2p_proto_mgt                   = &p2p_info->p2p_proto_mgt;
    
    wf_os_api_sema_init(&p2p_proto_mgt->thrd_sync_sema, 0);
    
    return 0;
}

wf_s32 p2p_proto_mgt_term(void *p2p)
{
    p2p_proto_mgt_st *p2p_proto_mgt = NULL;
    p2p_info_st *p2p_info           = NULL;
    
    if(NULL == p2p)
    {
        return 0;
    }
    p2p_info = p2p;
    p2p_proto_mgt = &p2p_info->p2p_proto_mgt;
    

    return 0;
}

wf_s32 wf_p2p_proto_thrd_wait(nic_info_st *pnic_info)
{
    p2p_proto_mgt_st *p2p_proto_mgt = NULL;
    p2p_info_st *p2p_info           = NULL;

    if(NULL == pnic_info)
    {
        return -1;
    }
    p2p_info = pnic_info->p2p;
    p2p_proto_mgt = &p2p_info->p2p_proto_mgt;
    wf_os_api_sema_wait(&p2p_proto_mgt->thrd_sync_sema);
    return 0;
}
wf_s32 wf_p2p_proto_thrd_post(nic_info_st *pnic_info)
{
    p2p_proto_mgt_st *p2p_proto_mgt = NULL;
    p2p_info_st *p2p_info           = NULL;

    if(NULL == pnic_info)
    {
        return -1;
    }
    p2p_info = pnic_info->p2p;
    p2p_proto_mgt = &p2p_info->p2p_proto_mgt;
    wf_os_api_sema_post(&p2p_proto_mgt->thrd_sync_sema);

    return 0;
}

