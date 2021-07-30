/*
 * p2p_proto_mgt.h
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
#ifndef __P2P_PROTO_MGT_H__
#define __P2P_PROTO_MGT_H__

enum
{
    /* priority level 0 */
    WF_P2P_MSG_TAG_TIMER_RO_CH_START = WF_MSG_TAG_SET(0, 0, 0),
    WF_P2P_MSG_TAG_TIMER_RO_CH_STOP,
    WF_P2P_MSG_TAG_NEGO,
    WF_P2P_MSG_TAG_RO_CH,
    WF_P2P_MSG_TAG_RO_CH_CANNEL,
};
#define THD_NAME_LEN    (32)

typedef struct
{
    wf_que_t  proto_queue; //mcu cmd handle queue
    wf_s8 proto_name[THD_NAME_LEN];
    void *proto_tid;
    wf_os_api_sema_t thrd_sync_sema;
    
    wf_bool thrd_abort;
    wf_bool thrd_abort_rsp;
    wf_pt_t pt[10];
    wf_msg_que_t msg_que;
    wf_msg_t *pmsg;
}p2p_proto_mgt_st;

typedef struct
{
    wf_u8 action;
    wf_u8 tx_ch;
    wf_u32 len;
    wf_u8 buf[512];
}p2p_nego_param_st;

wf_s32 wf_p2p_proto_proc_remain_channel(nic_info_st *pnic_info);
wf_s32 p2p_proto_mgt_init(void *p2p);
wf_s32 p2p_proto_mgt_term(void *p2p);
wf_s32 wf_p2p_msg_send(nic_info_st *pnic_info,wf_msg_tag_t tag,void *value,wf_u32 len);
wf_s32 wf_p2p_msg_timer_stop(nic_info_st *pnic_info,wf_msg_tag_t tag);
wf_s32 wf_p2p_msg_timer_start(nic_info_st *pnic_info,wf_msg_tag_t tag,wf_u32 duration);
wf_s32 wf_p2p_proto_thrd_wait(nic_info_st *pnic_info);
wf_s32 wf_p2p_proto_thrd_post(nic_info_st *pnic_info);

#endif
