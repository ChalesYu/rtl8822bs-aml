/*
 * p2p_frame_mgt.c
 *
 * used for .....
 *
 * Author: zenghua
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __P2P_FRAME_MGT_H__
#define __P2P_FRAME_MGT_H__

#define MAX_IE_SZ   768
#define PROBE_REQUEST_IE_SSID       0
#define PROBE_REQUEST_IE_RATE       1
#define WF_GET_BE16(a) ((wf_u16) (((a)[0] << 8) | (a)[1]))
#define WF_GET_LE16(a) ((wf_u16) (((a)[1] << 8) | (a)[0]))
#define WF_PUT_LE16(a, val) \
    do {    \
        (a)[1] = ((wf_u16) (val)) >> 8;\
        (a)[0] = ((wf_u16) (val)) & 0xff;\
    } while (0)

#define WF_PUT_BE16(a, val)         \
        do {                    \
            (a)[0] = ((wf_u16) (val)) >> 8;    \
            (a)[1] = ((wf_u16) (val)) & 0xff;  \
        } while (0)

#define WF_GET_BE32(a) ((((wf_u32) (a)[0]) << 24) | (((wf_u32) (a)[1]) << 16) | \
                         (((wf_u32) (a)[2]) << 8) | ((wf_u32) (a)[3]))
#define WF_PUT_BE32(a, val)                 \
                do {                            \
                    (a)[0] = (wf_u8) ((((wf_u32) (val)) >> 24) & 0xff);   \
                    (a)[1] = (wf_u8) ((((wf_u32) (val)) >> 16) & 0xff);   \
                    (a)[2] = (wf_u8) ((((wf_u32) (val)) >> 8) & 0xff);    \
                    (a)[3] = (wf_u8) (((wf_u32) (val)) & 0xff);       \
                } while (0)


typedef enum
{
    WF_P2P_IE_BEACON        = 0,
    WF_P2P_IE_PROBE_REQ     = 1,
    WF_P2P_IE_PROBE_RSP     = 2,
    WF_P2P_IE_ASSOC_REQ     = 3,
    WF_P2P_IE_ASSOC_RSP     = 4,
    WF_P2P_IE_MAX,
}WF_P2P_IE_E;

typedef struct wf_widev_invit_info
{
    wf_u8 state;
    wf_u8 peer_mac[WF_ETH_ALEN];
    wf_u8 active;
    wf_u8 token;
    wf_u8 flags;
    wf_u8 status;
    wf_u8 req_op_ch;
    wf_u8 rsp_op_ch;
} wf_widev_invit_info_t;

#define wf_widev_invit_info_init(invit_info) \
    do { \
        (invit_info)->state = 0xff; \
        wf_memset((invit_info)->peer_mac, 0, WF_ETH_ALEN); \
        (invit_info)->active = 0xff; \
        (invit_info)->token = 0; \
        (invit_info)->flags = 0x00; \
        (invit_info)->status = 0xff; \
        (invit_info)->req_op_ch = 0; \
        (invit_info)->rsp_op_ch = 0; \
    } while (0)

typedef struct wf_widev_nego_info
{
    wf_u8 state;
    wf_u8 peer_mac[WF_ETH_ALEN];
    wf_u8 active;
    wf_u8 token;
    wf_u8 status;
    wf_u8 req_intent;
    wf_u8 req_op_ch;
    wf_u8 req_listen_ch;
    wf_u8 rsp_intent;
    wf_u8 rsp_op_ch;
    wf_u8 conf_op_ch;
} wf_widev_nego_info_t;

#define wf_widev_nego_info_init(nego_info) \
    do { \
        (nego_info)->state = 0xff; \
        wf_memset((nego_info)->peer_mac, 0, WF_ETH_ALEN); \
        (nego_info)->active = 0xff; \
        (nego_info)->token = 0; \
        (nego_info)->status = 0xff; \
        (nego_info)->req_intent = 0xff; \
        (nego_info)->req_op_ch = 0; \
        (nego_info)->req_listen_ch = 0; \
        (nego_info)->rsp_intent = 0xff; \
        (nego_info)->rsp_op_ch = 0; \
        (nego_info)->conf_op_ch = 0; \
    } while (0)


typedef struct p2p_frame_check_param_st_
{
    wf_u8 *buf;
    wf_u32 len;
    
    wf_u8 *frame_body;
    wf_u32 frame_body_len;
    wf_u8 *p2p_ie;
    wf_u32 p2p_ielen;
    wf_u8 dialogToken;
    wf_bool is_tx;
} p2p_frame_check_param_st;

wf_u8 *wf_p2p_get_ie(wf_u8 * in_ie, wf_s32 in_len, wf_u8 * p2p_ie, wf_u32 * p2p_ielen);
wf_s32 wf_p2p_issue_probereq(nic_info_st *nic_info, wf_u8 * da);
wf_u8 *wf_p2p_get_attr_content(wf_u8 * p2p_ie, wf_u32 p2p_ielen, wf_u8 target_attr_id, wf_u8 * buf_content, wf_u32 * len_content);
wf_s32 wf_p2p_proc_probereq(nic_info_st *pnic_info,wf_80211_mgmt_t *pframe, wf_u16 frame_len);
wf_s32 wf_p2p_check_frames(nic_info_st *nic_info, const wf_u8 * buf, wf_u32 len, wf_bool tx,wf_u8 flag);
wf_s32 wf_p2p_proc_action_public(nic_info_st *pnic_info, wf_u8 *pframe, wf_u16 frame_len);
wf_s32 wf_p2p_fill_assoc_rsp(nic_info_st *pnic_info, wf_u8 *pframe, wf_u16 *pkt_len,WF_P2P_IE_E pie_type);
wf_u8* wf_p2p_fill_assoc_req(nic_info_st *pnic_info, wf_u8 *pframe, wf_u32 *pkt_len,WF_P2P_IE_E pie_type);
wf_s32 wf_p2p_parse_p2pie(nic_info_st *pnic_info, void *p2p, wf_u16 len,WF_P2P_IE_E ie_type);
wf_s32 wf_p2p_proc_assoc_req(nic_info_st *pnic_info, wf_u8 * p2p_ie,wf_u32 p2p_ielen, wdn_net_info_st *pwdn_info, wf_u8 flag);
wf_u8 *wf_p2p_ie_to_str(WF_P2P_IE_E ie_type);
wf_s32 wf_p2p_proc_rx_action(nic_info_st *pnic_info, wf_u8 *frame, wf_u32 len);
wf_s32 wf_p2p_proc_tx_action(nic_info_st *pnic_info,wf_u8 *frame, wf_u32 len,wf_u8 ch, wf_u8 wait_ack);

#endif
