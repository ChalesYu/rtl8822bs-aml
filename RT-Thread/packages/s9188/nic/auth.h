/*
 * auth.h
 *
 * This file contains all the prototypes for the auth.c file
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
#ifndef __AUTH_H__
#define __AUTH_H__

/* macro */
#define _CAPABILITY_            2
#define _BEACON_ITERVAL_        2
#define _RSON_CODE_             2
#define _TIMESTAMP_             8

#define _HW_STATE_NOLINK_       0x00
#define _HW_STATE_ADHOC_        0x01
#define _HW_STATE_STATION_      0x02
#define _HW_STATE_AP_           0x03
#define _HW_STATE_MONITOR_      0x04

#define WIFI_FW_NULL_STATE      _HW_STATE_NOLINK_
#define WIFI_FW_STATION_STATE   _HW_STATE_STATION_
#define WIFI_FW_AP_STATE        _HW_STATE_AP_
#define WIFI_FW_ADHOC_STATE     _HW_STATE_ADHOC_

/* type define */
enum
{
    WF_AUTH_TAG_RSP     = WF_MSG_TAG_SET(0, 0, 0),
    WF_AUTH_TAG_ABORT   = WF_MSG_TAG_SET(0, 1, 0),
    WF_AUTH_TAG_START   = WF_MSG_TAG_SET(0, 2, 0),
    WF_AUTH_TAG_DONE,
};

typedef wf_u8 auth_rsp_t[WF_80211_MGMT_AUTH_SIZE_MAX];
typedef struct
{
    wdn_net_info_st *pwdn_info;
    wf_u8 retry_cnt;
    wf_timer_t timer;
    wf_bool brun;
    wf_msg_que_t msg_que;
} auth_info_t;

/* function declaration */
#ifdef CFG_ENABLE_AP_MODE
wf_pt_rst_t wf_auth_ap_thrd (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);
#endif
wf_pt_rst_t wf_auth_sta_thrd (wf_pt_t *pt, nic_info_st *pnic_info, int *pres);
int wf_auth_sta_start (nic_info_st *pnic_info);
int wf_auth_sta_stop (nic_info_st *pnic_info);
int wf_auth_frame_parse (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                         wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
int wf_auth_init (nic_info_st *pnic_info);
int wf_auth_term (nic_info_st *pnic_info);
sys_work_mode_e get_sys_work_mode (nic_info_st *pnic_info);
void set_sys_work_mode (nic_info_st *pnic_info, sys_work_mode_e mode);
int wf_deauth_xmit_frame (nic_info_st *pnic_info, wf_u8 *pmac, wf_u16 reason_code);
int wf_deauth_frame_parse (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                           wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);

#endif

