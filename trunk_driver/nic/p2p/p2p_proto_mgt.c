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
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

#define P2P_PROTO_DBG(fmt, ...)      LOG_D("P2P_PROTO[%s:%d][%d] "fmt, __func__,__LINE__, pnic_info->ndev_id,##__VA_ARGS__)
#define P2P_PROTO_ARRAY(data, len)   log_array(data, len)
#define P2P_PROTO_INFO(fmt, ...)     LOG_I("P2P_PROTO[%s:%d][%d] "fmt, __func__,__LINE__, pnic_info->ndev_id,##__VA_ARGS__)
#define P2P_PROTO_WARN(fmt, ...)     LOG_E("P2P_PROTO[%s:%d][%d] "fmt, __func__,__LINE__, pnic_info->ndev_id,##__VA_ARGS__)
#define P2P_NEGO_TIME (15000) //ms

wf_s32 wf_p2p_nego_timer_set(nic_info_st *pnic_info,wf_u32 timeout)
{
    p2p_info_st *p2p_info       = NULL;
    if(NULL == pnic_info)
    {
        return -1;
    }
    p2p_info = pnic_info->p2p;
    if(P2P_CONN_NEGO_TIME == timeout)
    {
        if(p2p_info->go_negoing)
        {
            wf_os_api_timer_set(&p2p_info->nego_timer, P2P_CONN_NEGO_TIME);
        }
    }
    else if(P2P_SCAN_NEGO_TIME == timeout)
    {
        if(p2p_info->go_negoing)
        {
            wf_os_api_timer_set(&p2p_info->nego_timer, P2P_SCAN_NEGO_TIME);
        }
    }
    else if(P2P_EAPOL_NEGO_TIME == timeout)
    {
        wf_os_api_timer_set(&p2p_info->nego_timer, P2P_EAPOL_NEGO_TIME);
    }
    return 0;
}
wf_s32 wf_p2p_cannel_remain_on_channel(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info       = NULL;
    CHANNEL_WIDTH cw            = CHANNEL_WIDTH_20;
    HAL_PRIME_CH_OFFSET offset  = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
    wf_u8 hw_ch                 = 0;
    wf_u8 buddy_ch              = wf_p2p_get_buddy_channel(pnic_info);

    if(NULL == pnic_info)
    {
        return -1;
    }

    p2p_info = pnic_info->p2p;
    if(wf_false == p2p_info->is_ro_ch)
    {
        return 0;
    }
    p2p_info->is_ro_ch = wf_false;
    if( 0 == p2p_info->ro_ch_again)
    {
        buddy_ch = wf_p2p_get_buddy_channel(pnic_info);
        wf_hw_info_get_ch_bw(pnic_info, &hw_ch,&cw, &offset);

        P2P_PROTO_INFO("(%d,%d,%d) hw_ch[%d]  listen_ch:%u, remain_ch:%d, buddy_ch:%d,peer_listen_ch:%d, link_ch:%d,nego:0x%x\n"
                       ,p2p_info->role,p2p_info->p2p_state,p2p_info->pre_p2p_state,hw_ch
                       ,p2p_info->listen_channel,p2p_info->remain_ch,buddy_ch
                       ,p2p_info->peer_listen_channel,p2p_info->link_channel,p2p_info->go_negoing);
#if 0
        if(p2p_info->go_negoing & WF_BIT(P2P_GO_NEGO_CONF) || p2p_info->go_negoing & WF_BIT(P2P_INVIT_RESP))
        {
            wf_hw_info_set_channnel_bw(pnic_info,  p2p_info->link_channel, cw, offset);
        }
        else
        {
            if(buddy_ch && 0 == p2p_info->go_negoing)
            {
                wf_hw_info_set_channnel_bw(pnic_info,  buddy_ch, cw, offset);
            }
#if 0
            else if(p2p_info->remain_ch == p2p_info->listen_channel && 0 != p2p_info->peer_listen_channel)
            {
                wf_hw_info_set_channnel_bw(pnic_info,  p2p_info->peer_listen_channel, cw, offset);
            }
            else
            {
                wf_hw_info_set_channnel_bw(pnic_info,  p2p_info->listen_channel, cw, offset);
            }
#else
            else if(hw_ch != p2p_info->remain_ch)
            {
                //wf_hw_info_set_channnel_bw(pnic_info,  p2p_info->remain_ch, cw, offset);
            }
#endif
        }
#else
        if(buddy_ch)
        {
            if(0 == p2p_info->go_negoing)
            {
                if(hw_ch != buddy_ch)
                {
                    wf_hw_info_set_channnel_bw(pnic_info,  buddy_ch, cw, offset);
                }
            }
            else if(p2p_info->go_negoing & WF_BIT(P2P_GO_NEGO_CONF) || p2p_info->go_negoing & WF_BIT(P2P_INVIT_RESP))
            {
                if(hw_ch != buddy_ch)
                {
                    wf_hw_info_set_channnel_bw(pnic_info,  buddy_ch, cw, offset);
                }
            }
            else
            {
                if(hw_ch != buddy_ch)
                {
                    //wf_hw_info_set_channnel_bw(pnic_info,  buddy_ch, cw, offset);
                }
            }
        }
        else
        {
            if(0 == p2p_info->go_negoing)
            {
                //do nothing
            }
            if(p2p_info->go_negoing & WF_BIT(P2P_GO_NEGO_CONF) || p2p_info->go_negoing & WF_BIT(P2P_INVIT_RESP))
            {
                //wf_hw_info_set_channnel_bw(pnic_info,  p2p_info->link_channel, cw, offset);
            }
            else
            {
                if(hw_ch != p2p_info->remain_ch)
                {
                    //wf_hw_info_set_channnel_bw(pnic_info,  p2p_info->remain_ch, cw, offset);
                }
            }
        }
        #endif
    }
    else
    {
        p2p_info->ro_ch_again = 0;
    }

    if(NULL != p2p_info->scb.remain_on_channel_expired)
    {
        p2p_info->scb.remain_on_channel_expired(pnic_info,NULL,0);
    }

    wf_p2p_set_state(p2p_info, p2p_info->pre_p2p_state);

    p2p_info->last_ro_ch_time = 0;


    return 0;
}

wf_s32 wf_p2p_remain_on_channel(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info                   = NULL;
    CHANNEL_WIDTH cw = CHANNEL_WIDTH_20;
    HAL_PRIME_CH_OFFSET offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
    wf_u8 hw_ch = 0;
    wf_u8 buddy_ch = wf_p2p_get_buddy_channel(pnic_info);
    if(NULL == pnic_info)
    {
        return -1;
    }

    p2p_info                   = pnic_info->p2p;
    p2p_info->is_ro_ch = wf_true;

    wf_hw_info_get_ch_bw(pnic_info, &hw_ch,&cw, &offset);
    P2P_PROTO_INFO("(%d,%d,%d) hw_ch[%d]  listen_ch:%u, remain_ch:%d, buddy_ch:%d,peer_listen_ch:%d, link_ch:%d,nego:0x%x\n"
                   ,p2p_info->role,p2p_info->p2p_state,p2p_info->pre_p2p_state,hw_ch
                   ,p2p_info->listen_channel,p2p_info->remain_ch,buddy_ch
                   ,p2p_info->peer_listen_channel,p2p_info->link_channel,p2p_info->go_negoing);

    if(hw_ch != p2p_info->remain_ch)
    {
        wf_wlan_set_cur_channel(pnic_info, p2p_info->remain_ch);
        wf_hw_info_set_channnel_bw(pnic_info, p2p_info->remain_ch, cw, offset);
    }

    wf_p2p_set_state(p2p_info, P2P_STATE_LISTEN);

    while(p2p_info->ro_ch_duration > 0 && p2p_info->ro_ch_duration<400)
    {
        p2p_info->ro_ch_duration = p2p_info->ro_ch_duration*3;
    }
    wf_os_api_timer_set(&p2p_info->remain_ch_timer, p2p_info->ro_ch_duration);
    p2p_info->last_ro_ch_time = wf_os_api_timestamp();
    return 0;
}

wf_pt_ret_t wf_p2p_core_thrd (wf_pt_t *pt, nic_info_st *pnic_info,void *param)
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
        //wf_p2p_mgnt_nego(pnic_info,p2p_param);
    }
    else if (WF_P2P_MSG_TAG_RO_CH == p2p_param->tag )
    {
        P2P_PROTO_INFO("WF_P2P_MSG_TAG_RO_CH...");
        wf_p2p_remain_on_channel(pnic_info);
        //wf_p2p_proto_thrd_post(pnic_info);
    }
    else if (WF_P2P_MSG_TAG_RO_CH_CANNEL == p2p_param->tag )
    {
        P2P_PROTO_INFO("WF_P2P_MSG_TAG_RO_CH_CANNEL...");
        wf_p2p_cannel_remain_on_channel(pnic_info);
        //wf_p2p_proto_thrd_post(pnic_info);
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

