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

#if 1
#define P2P_PROTO_DBG(fmt, ...)      LOG_D("P2P_PROTO[%s:%d]"fmt, __func__,__LINE__, ##__VA_ARGS__)
#define P2P_PROTO_ARRAY(data, len)   log_array(data, len)
#else
#define P2P_PROTO_DBG(fmt, ...)
#define P2P_PROTO_ARRAY(data, len)
#endif

#define P2P_PROTO_INFO(fmt, ...)     LOG_I("P2P_PROTO[%s:%d]"fmt, __func__,__LINE__, ##__VA_ARGS__)
#define P2P_PROTO_WARN(fmt, ...)     LOG_E("P2P_PROTO[%s:%d]"fmt, __func__,__LINE__, ##__VA_ARGS__)

 wf_s32 wf_p2p_proto_proc_remain_channel(nic_info_st *pnic_info)
{
    wf_u8 ch, bw, offset;
    p2p_info_st *p2p_info = pnic_info->p2p;
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = NULL;
    wf_wlan_network_t *pcur_network = NULL;

    P2P_PROTO_DBG("start");
    
    pwlan_mgmt_info = (wf_wlan_mgmt_info_t *)pnic_info->wlan_mgmt_info;
    if(NULL == pwlan_mgmt_info)
    {
        return -1;
    }
    
    pcur_network = &pwlan_mgmt_info->cur_network;
    
    if (p2p_info->p2p_enabled && p2p_info->listen_channel) 
    {
        ch = p2p_info->listen_channel;
        bw = CHANNEL_WIDTH_20;
        offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
        P2P_PROTO_INFO(" back to listen ch - ch:%u, bw:%u, offset:%u\n",
                 ch, bw, offset);
    } 
    else 
    {
        ch = p2p_info->restore_channel;
        bw = CHANNEL_WIDTH_20;
        offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
        P2P_PROTO_INFO(" back to restore ch - ch:%u, bw:%u, offset:%u\n",
                     ch, bw, offset);
    }
    
    pcur_network->channel = ch;
    wf_hw_info_set_channnel_bw(pnic_info, ch, offset, bw);

    p2p_info->is_ro_ch = wf_false;
    p2p_info->last_ro_ch_time = wf_os_api_timestamp();
#if 1
    if(NULL != p2p_info->scb.remain_on_channel_expired)
    {
        p2p_info->scb.remain_on_channel_expired(pnic_info,NULL,0);
    }
#endif
    return 0;
}

wf_s32 p2p_mgnt_nego_tx(nic_info_st *pnic_info, wf_u8 tx_ch, wf_u8 *buf, wf_s32 len)
{
    struct xmit_buf *pxmit_buf;
    tx_info_st *ptx_info;
    wf_80211_mgmt_t *pmgmt;
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = (wf_wlan_mgmt_info_t *)pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_mgmt_info->cur_network;

    // wf_lps_deny(pnic_info, 1000);
    wf_scan_wait_done(pnic_info, wf_true, 200);

    /* if (tx_ch != pmlmeext->cur_channel) { */
    if (tx_ch != pcur_network->channel)
    {
        pcur_network->channel = tx_ch;
        wf_hw_info_set_channnel_bw(pnic_info, tx_ch, CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE);
    }

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

    wf_nic_mgmt_frame_xmit(pnic_info, NULL, pxmit_buf, len);
    return 1;

}


wf_s32 wf_p2p_mgnt_nego(nic_info_st *pnic_info,void *param)
{
    p2p_nego_param_st *pn = param;
    wf_s32 type         = 0;
    wf_timer_t timer;
    wf_u32 dump_limit   = 8;
    wf_u32 dump_cnt     = 0;
    wf_s32 tx_ret       = 0;

    if(NULL == pnic_info ||NULL == param)
    {
        return -1;
    }
    
    pn = param;
    P2P_PROTO_DBG("action:%d,tx_ch:%d,len:0x%x",pn->action,pn->tx_ch,pn->len);
    type = wf_p2p_check_frames(pnic_info,pn->buf,pn->len,wf_true,1);
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
        tx_ret = p2p_mgnt_nego_tx(pnic_info, pn->tx_ch, pn->buf, pn->len);
        if(WF_WLAN_ACTION_PUBLIC_GAS_INITIAL_REQ == pn->action || 
            WF_WLAN_ACTION_PUBLIC_GAS_INITIAL_RSP == pn->action)
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
    if (p2p_info->is_ro_ch == wf_true)
    {
        P2P_PROTO_INFO("cancel ro ch timer\n");
        wf_p2p_msg_timer_stop(pnic_info,WF_P2P_MSG_TAG_TIMER_RO_CH_STOP);
        wf_p2p_proto_proc_remain_channel(pnic_info);
    }

    wf_p2p_set_state(p2p_info, p2p_info->pre_p2p_state);
    P2P_PROTO_INFO("role:%s, state:%s",
        wf_p2p_role_to_str(p2p_info->role),wf_p2p_state_to_str(p2p_info->p2p_state));
    p2p_info->is_ro_ch = wf_false;
    p2p_info->last_ro_ch_time = wf_os_api_timestamp();
    
    return 0;
}

wf_s32 wf_p2p_remain_on_channel(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info                   = NULL;
    wf_wlan_mgmt_info_t *pwlan_mgmt_info    = NULL;
    wf_wlan_network_t *pcur_network         = NULL;

    if(NULL == pnic_info)
    {
        return -1;
    }
    
    p2p_info                   = pnic_info->p2p;
    pwlan_mgmt_info = (wf_wlan_mgmt_info_t *)pnic_info->wlan_mgmt_info;
    pcur_network    = &pwlan_mgmt_info->cur_network;
    P2P_PROTO_DBG(" ch:%u duration:%d\n",p2p_info->remain_ch, p2p_info->ro_ch_duration);
    if (p2p_info->is_ro_ch == wf_true)
    {
        P2P_PROTO_INFO("cancel ro ch timer\n");
        wf_p2p_msg_timer_stop(pnic_info,WF_P2P_MSG_TAG_TIMER_RO_CH_STOP);
    }

    p2p_info->is_ro_ch = wf_true;
    p2p_info->last_ro_ch_time = wf_os_api_timestamp();

    if (p2p_info->p2p_state == P2P_STATE_NONE)
    {
        wf_p2p_enable(pnic_info, P2P_ROLE_DEVICE);
        p2p_info->p2p_enabled = wf_true;
        p2p_info->listen_channel = p2p_info->remain_ch;
    }
    else if (p2p_info->p2p_state == P2P_STATE_LISTEN)
    {
        P2P_PROTO_INFO("listen_channel:%d, remain_ch:%d",p2p_info->listen_channel,p2p_info->remain_ch);
        if(p2p_info->listen_channel != p2p_info->remain_ch)
        {
            p2p_info->listen_channel = p2p_info->remain_ch;
        }

    }
    else
    {
        wf_p2p_set_pre_state(p2p_info, p2p_info->p2p_state);
        P2P_PROTO_INFO("role=%d, p2p_state=%d\n", p2p_info->role,p2p_info->p2p_state);
    }

    wf_p2p_set_state(p2p_info, P2P_STATE_LISTEN);
    P2P_PROTO_INFO("role=%d, p2p_state=%d  listen_channel=%d\n", p2p_info->role,p2p_info->p2p_state,p2p_info->listen_channel);

    p2p_info->restore_channel = pcur_network->channel;
    pcur_network->channel = p2p_info->remain_ch;
    P2P_PROTO_DBG("current channel:%d",pcur_network->channel);
    wf_hw_info_set_channnel_bw(pnic_info, p2p_info->remain_ch, CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

    wf_p2p_msg_timer_start(pnic_info,WF_P2P_MSG_TAG_TIMER_RO_CH_START,p2p_info->ro_ch_duration);

    if(p2p_info->scb.ready_on_channel)
    {
        p2p_info->scb.ready_on_channel(pnic_info,NULL,0);
    }

    return 0;
}

static wf_pt_rst_t p2p_core_thrd (nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info           = pnic_info->p2p;
    p2p_proto_mgt_st *p2p_proto_mgt = &p2p_info->p2p_proto_mgt;
    p2p_timer_st *p2p_timers        = &p2p_info->p2p_timers;
    wf_pt_t *pt             = &p2p_proto_mgt->pt[0];
//    wf_pt_t *pt_sub         = &p2p_proto_mgt->pt[1];
    wf_msg_que_t *pmsg_que  = &p2p_proto_mgt->msg_que;
    wf_msg_t *pmsg          = NULL;
    
    PT_BEGIN(pt);

    while (wf_true)
    {
        if (p2p_proto_mgt->thrd_abort)
        {
            P2P_PROTO_INFO("thread abort");
            PT_EXIT(pt);
        }
        if (!wf_msg_pop(pmsg_que, &pmsg))
        {
            break;
        }
        PT_YIELD(pt);
    }

    P2P_PROTO_INFO("tag:0x%x",pmsg->tag);
    if (WF_P2P_MSG_TAG_TIMER_RO_CH_START == pmsg->tag )
    {
        p2p_timer_param_st *p2p_tp = (void*)pmsg->value;
        p2p_proto_mgt->pmsg = pmsg;
        P2P_PROTO_INFO("WF_P2P_MSG_TAG_TIMER_RO_CH_START(%d)",p2p_tp->duration);
        /* wait until timeout */
        wf_timer_set(&p2p_timers->remain_on_ch_timer, p2p_tp->duration);
        do
        {
            PT_WAIT_UNTIL(pt, !wf_msg_get(pmsg_que, &pmsg) ||
                          wf_timer_expired(&p2p_timers->remain_on_ch_timer));
            if (pmsg && WF_P2P_MSG_TAG_TIMER_RO_CH_STOP == pmsg->tag)
            {
                P2P_PROTO_INFO("WF_P2P_MSG_TAG_TIMER_RO_CH_STOP...");
            }
            else
            {
                if(pmsg)
                {
                    P2P_PROTO_INFO("tag: 0x%x...",pmsg->tag);
                }
                else
                {
                    P2P_PROTO_INFO("tag: 0x%x...",p2p_proto_mgt->pmsg->tag);
                }
            }
            wf_msg_del(pmsg_que, p2p_proto_mgt->pmsg);
            break;
        }
        while (wf_true);
    }
    else if (WF_P2P_MSG_TAG_TIMER_RO_CH_STOP == pmsg->tag )
    {
        P2P_PROTO_INFO("WF_P2P_MSG_TAG_TIMER_RO_CH_STOP...");
        p2p_proto_mgt->pmsg = pmsg;
        wf_msg_del(pmsg_que, p2p_proto_mgt->pmsg);
    }
    else if (WF_P2P_MSG_TAG_NEGO == pmsg->tag )
    {
        P2P_PROTO_INFO("WF_P2P_MSG_TAG_NEGO...");
        p2p_proto_mgt->pmsg = pmsg;
        wf_p2p_mgnt_nego(pnic_info,pmsg->value);
        wf_p2p_proto_thrd_post(pnic_info);
        wf_msg_del(pmsg_que, p2p_proto_mgt->pmsg);
    }
    else if (WF_P2P_MSG_TAG_RO_CH == pmsg->tag )
    {
        P2P_PROTO_INFO("WF_P2P_MSG_TAG_RO_CH...");
        p2p_proto_mgt->pmsg = pmsg;
        wf_p2p_remain_on_channel(pnic_info);
        wf_p2p_proto_thrd_post(pnic_info);
        wf_msg_del(pmsg_que, p2p_proto_mgt->pmsg);
    }
    else if (WF_P2P_MSG_TAG_RO_CH_CANNEL == pmsg->tag )
    {
        P2P_PROTO_INFO("WF_P2P_MSG_TAG_RO_CH_CANNEL...");
        p2p_proto_mgt->pmsg = pmsg;
        wf_p2p_cannel_remain_on_channel(pnic_info);
        wf_p2p_proto_thrd_post(pnic_info);
        wf_msg_del(pmsg_que, p2p_proto_mgt->pmsg);
    }
    else
    {
        P2P_PROTO_INFO("drop unsuited message(tag: %d)", pmsg->tag);
        wf_msg_del(pmsg_que, pmsg);
    }

    /* restart thread */
    PT_RESTART(pt);

    PT_END(pt);
}

static wf_s32 p2p_core (nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info           = NULL;
    p2p_proto_mgt_st *p2p_proto_mgt = NULL;
    P2P_PROTO_DBG();

    wf_os_api_thread_affinity(DEFAULT_CPU_ID);

    while (1)
    {
        if(WF_CANNOT_RUN(pnic_info))
        {
            P2P_PROTO_WARN("WF_CANNOT_RUN");
            break;
        }
        p2p_info        = pnic_info->p2p;
        if(NULL == p2p_info)
        {
            P2P_PROTO_WARN("p2p_info is NULL");
            break;
        }
        
        p2p_proto_mgt   = &p2p_info->p2p_proto_mgt;
        /* poll mlme core */
        
        PT_INIT(&p2p_proto_mgt->pt[0]);
        while (PT_SCHEDULE(p2p_core_thrd(pnic_info)))
        {
            wf_msleep(1);
        }
        P2P_PROTO_DBG("");
    }

    P2P_PROTO_DBG("wait for thread destory...");
    p2p_proto_mgt->thrd_abort_rsp = wf_true;
    while (!wf_os_api_thread_wait_stop(p2p_proto_mgt->proto_tid))
    {
        wf_msleep(1);
    }

    wf_os_api_thread_exit(p2p_proto_mgt->proto_tid);

    return 0;
}


static wf_s32 p2p_proto_msg_init(wf_msg_que_t *pmsg_que)
{
    wf_msg_init(pmsg_que);
    return (wf_msg_alloc(pmsg_que, WF_P2P_MSG_TAG_TIMER_RO_CH_START, sizeof(p2p_timer_param_st), 2) ||
            wf_msg_alloc(pmsg_que, WF_P2P_MSG_TAG_TIMER_RO_CH_STOP, 0, 2) ||
            wf_msg_alloc(pmsg_que, WF_P2P_MSG_TAG_NEGO, sizeof(p2p_nego_param_st), 2) ||
             wf_msg_alloc(pmsg_que, WF_P2P_MSG_TAG_RO_CH, 0, 2) || 
             wf_msg_alloc(pmsg_que, WF_P2P_MSG_TAG_RO_CH_CANNEL, 0, 2)) ? -1 : 0;

}
wf_inline static void p2p_proto_msg_deinit (wf_msg_que_t *pmsg_que)
{
    wf_msg_deinit(pmsg_que);
}

static int p2p_msg_send (nic_info_st *pnic_info, wf_msg_tag_t tag, void *value, wf_u32 len)
{
    p2p_info_st *p2p_info   = NULL;
    p2p_proto_mgt_st *pp    = NULL;
    wf_msg_que_t *pmsg_que  = NULL;
    wf_msg_t *pmsg          = NULL;
    int rst;

    if (pnic_info == NULL)
    {
        return -1;
    }

    p2p_info = pnic_info->p2p;
    if (p2p_info == NULL)
    {
        return -2;
    }

    P2P_PROTO_INFO("tag:0x%x",tag);
    pp  = &p2p_info->p2p_proto_mgt;
    pmsg_que = &pp->msg_que;
    rst = wf_msg_new(pmsg_que, tag, &pmsg);
    if (rst)
    {
        P2P_PROTO_WARN("wf_msg_new fail error code: %d", rst);
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
        P2P_PROTO_WARN("wf_msg_push fail error code: %d", rst);
        return -4;
    }

    return 0;
}

wf_s32 wf_p2p_msg_timer_stop(nic_info_st *pnic_info,wf_msg_tag_t tag)
{
    return p2p_msg_send(pnic_info,tag,NULL,0);
}
wf_s32 wf_p2p_msg_timer_start(nic_info_st *pnic_info,wf_msg_tag_t tag,wf_u32 duration)
{
    p2p_timer_param_st param;
    param.duration = duration;
    return p2p_msg_send(pnic_info,tag,&param,sizeof(p2p_timer_param_st));
}

wf_s32 wf_p2p_msg_send(nic_info_st *pnic_info,wf_msg_tag_t tag,void *value,wf_u32 len)
{
    return p2p_msg_send(pnic_info,tag,value,len);
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
    
    wf_que_init(&p2p_proto_mgt->proto_queue,WF_LOCK_TYPE_SPIN);
    p2p_proto_mgt->thrd_abort = wf_false;
    p2p_proto_msg_init(&p2p_proto_mgt->msg_que);
    wf_os_api_sema_init(&p2p_proto_mgt->thrd_sync_sema, 0);
    sprintf((char *)p2p_proto_mgt->proto_name,
            nic_info->virNic ? "p2p_proto_mgt:vir%d_%d" : "p2p_proto_mgt:wlan%d_%d",
            nic_info->hif_node_id, nic_info->ndev_id);
    if (NULL ==(p2p_proto_mgt->proto_tid=wf_os_api_thread_create(p2p_proto_mgt->proto_tid, (char *)p2p_proto_mgt->proto_name, (void *)p2p_core, nic_info)))
    {
        LOG_E("[%s] create thread failed",__func__);
        return -1;
    }
    else
    {
        wf_os_api_thread_wakeup(p2p_proto_mgt->proto_tid);
    }

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
    if (p2p_proto_mgt && p2p_proto_mgt->proto_tid)
    {
        p2p_proto_mgt->thrd_abort = wf_true;
        while(wf_false == p2p_proto_mgt->thrd_abort_rsp)
        {
            wf_msleep(1);
        }
        wf_os_api_thread_destory(p2p_proto_mgt->proto_tid);
        p2p_proto_msg_deinit(&p2p_proto_mgt->msg_que);
        p2p_proto_mgt->proto_tid = NULL;
    }

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

