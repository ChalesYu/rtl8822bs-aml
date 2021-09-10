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
    WF_P2P_MSG_TAG_TIMER_RO_CH = WF_MSG_TAG_SET(0, 0, 0),
    WF_P2P_MSG_TAG_NEGO,
    WF_P2P_MSG_TAG_RO_CH,
    WF_P2P_MSG_TAG_RO_CH_CANNEL,
};
#define THD_NAME_LEN    (32)

typedef struct
{
    wf_os_api_sema_t thrd_sync_sema;
   
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

wf_s32 wf_p2p_cannel_remain_on_channel(nic_info_st *pnic_info);
wf_s32 p2p_proto_mgt_init(void *p2p);
wf_s32 p2p_proto_mgt_term(void *p2p);
wf_pt_rst_t wf_p2p_core_thrd (wf_pt_t *pt,nic_info_st *pnic_info,void *param);
wf_s32 wf_p2p_proto_thrd_wait(nic_info_st *pnic_info);
wf_s32 wf_p2p_proto_thrd_post(nic_info_st *pnic_info);
wf_s32 wf_p2p_mgnt_nego(nic_info_st *pnic_info,void *param);

#endif
