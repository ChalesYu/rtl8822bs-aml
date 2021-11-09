/*
 * mlme.h
 *
 * This file contains all the prototypes for the mlme.c file
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
#ifndef __MLME_H__
#define __MLME_H__

/* macro */

#define WF_NAME_MAX_LEN         (32)

/* type define */
typedef enum
{
    WF_MLME_FRAMEWORK_WEXT,
    WF_MLME_FRAMEWORK_NETLINK,
    WF_MLME_FRAMEWORK_NDIS,
    WF_MLME_FRAMEWORK_NONE,
} wf_mlme_framework_e;

enum
{
    /* priority level 0 */
    WF_MLME_TAG_SCAN_ABORT      = WF_MSG_TAG_SET(0, 0, 0),
    WF_MLME_TAG_SCAN_RSP,
    WF_MLME_TAG_IBSS_LEAVE,
    WF_MLME_TAG_IBSS_BEACON_FRAME,
    WF_MLME_TAG_CONN_ABORT,
    WF_MLME_TAG_DEAUTH,
    WF_MLME_TAG_DEASSOC,
    WF_MLME_TAG_P2P,

    /* priority level 1 */
    WF_MLME_TAG_SCAN            = WF_MSG_TAG_SET(0, 1, 0),
    WF_MLME_TAG_CONN,
    WF_MLME_TAG_CONN_IBSS,


    /* priority level 2 */
    WF_MLME_TAG_ADD_BA_REQ      = WF_MSG_TAG_SET(0, 2, 0),
    WF_MLME_TAG_ADD_BA_RSP,
    WF_MLME_TAG_KEEPALIVE,

#ifdef CONFIG_LPS
    WF_MLME_TAG_LPS,
#endif
};

typedef enum
{
    /* bss scan */
    MLME_STATE_SCAN,

    /* bss connection */
    MLME_STATE_CONN_SCAN,
    MLME_STATE_AUTH,
    MLME_STATE_ASSOC,
    MLME_STATE_DEAUTH,
    MLME_STATE_DEASSOC,
    MLME_STATE_ADD_BA_RESP,
    MLME_STATE_ADD_BA_REQ,

    /* ibss */
    MLME_STATE_IBSS_CONN_SCAN,

    MLME_STATE_IDLE,
} mlme_state_e;

typedef struct
{
    wf_u32  num_tx_ok_in_period_with_tid[TID_NUM];
    wf_u32  num_tx_ok_in_period;
    wf_u32  num_rx_ok_in_period;
    wf_u32  num_rx_unicast_ok_in_period;
    wf_bool busy_traffic;
} wf_link_info_st;

typedef struct
{
    unsigned int tid;
    unsigned int start_seq;
    wf_u16 param;
    wf_u16 timeout;
    wf_u8 policy;
    wf_u8 addr[MAC_ADDR_LEN];
    wf_u16 status;
    wf_u8 size;
    wf_u8 dialog;
} wf_add_ba_parm_st;

typedef struct
{
    wf_bool local_disconn;
    wf_80211_reasoncode_e reason_code;
} wf_mlme_conn_res_t;

typedef struct
{
    nic_info_st *parent;
    void *tid;
    char mlmeName[WF_NAME_MAX_LEN];
    mlme_state_e state;
    wf_bool connect;
    wf_lock_t state_lock;
    wf_lock_t connect_lock;
    wf_link_info_st link_info;

    wf_add_ba_parm_st  barsp_parm;
    wf_add_ba_parm_st  bareq_parm;
    wf_u8              baCreating;

    wf_pt_t pt[10];
    wf_msg_que_t msg_que;
    wf_timer_t traffic_timer;
    wf_timer_t keep_alive_timer;
    wdn_net_info_st *pwdn_info;
    wf_u8 try_cnt;

    wf_msg_t *pconn_msg;
    wf_msg_t *pscan_msg;
    wf_msg_t *pba_rsp_msg;
    wf_msg_t *pp2p_msg;

    wf_mlme_conn_res_t conn_res;

    wf_bool babort_thrd;
    wf_bool abort_thrd_finished;

    wf_u8 *probereq_wps_ie;
    wf_u8 *wps_beacon_ie;
    wf_u8 *wps_probe_resp_ie;
    wf_u8 *wps_assoc_resp_ie;

    wf_u32 wps_ie_len;
    wf_u32 wps_beacon_ie_len;
    wf_u32 wps_probe_resp_ie_len;
    wf_u32 wps_assoc_resp_ie_len;

    wf_u8 action_public_dialog_token;
    wf_u16 action_public_rxseq;

    wf_bool vir_scanning_intf;
} mlme_info_t;

/* function declaration */
wf_inline
static wf_bool wf_mlme_check_mode(nic_info_st* pnic_info, sys_work_mode_e mode)
{
    local_info_st* plocal_info = pnic_info->local_info;

    return (wf_bool)(plocal_info->work_mode == mode);
}

int wf_mlme_p2p_msg(nic_info_st *pnic_info,wf_msg_tag_t p2p_tag,void *value,wf_u32 len);
int wf_mlme_get_state (nic_info_st *pnic_info, mlme_state_e *state);
int wf_mlme_set_connect (nic_info_st *pnic_info, wf_bool bconnect);
int wf_mlme_get_connect (nic_info_st *pnic_info, wf_bool *bconnect);
int wf_mlme_get_traffic_busy (nic_info_st *pnic_info, wf_bool *bbusy);
int wf_mlme_abort (nic_info_st *pnic_info);
int wf_mlme_scan_start (nic_info_st *pnic_info, scan_type_e type,
                        wf_wlan_ssid_t ssids[], wf_u8 ssid_num,
                        wf_u8 chs[], wf_u8 ch_num,
                        wf_mlme_framework_e frm_work);

#ifdef CFG_ENABLE_ADHOC_MODE

int wf_mlme_scan_ibss_start (nic_info_st *pnic_info,
                             wf_wlan_ssid_t *pssid,
                             wf_u8 *pch,
                             wf_mlme_framework_e frm_work);
#endif

int wf_mlme_scan_abort (nic_info_st *pnic_info);
int wf_mlme_conn_scan_rsp (nic_info_st *pnic_info,
                           wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
int wf_mlme_conn_start (nic_info_st *pnic_info, wf_80211_bssid_t bssid,
                        wf_wlan_ssid_t *pssid, wf_mlme_framework_e frm_work,
                        wf_bool indicate_en);
int wf_mlme_conn_abort (nic_info_st *pnic_info,
                        wf_bool local_gen,
                        wf_80211_reasoncode_e reason);
int wf_mlme_deauth (nic_info_st *pnic_info,
                    wf_bool local_gen,
                    wf_80211_reasoncode_e reason);
int wf_mlme_deassoc (nic_info_st *pnic_info,
                     wf_bool local_gen,
                     wf_80211_reasoncode_e reason);
int wf_mlme_add_ba_req (nic_info_st *pnic_info);
int wf_mlme_add_ba_rsp (nic_info_st *pnic_info, wf_add_ba_parm_st *barsp_parm);
int wf_mlme_init (nic_info_st *pnic_info);
int wf_mlme_term (nic_info_st *pnic_info);

#endif

