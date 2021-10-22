/*
 * scan.h
 *
 * This file contains all the prototypes for the scan.c file
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __SCAN_H__
#define __SCAN_H__

#define WF_SCAN_REQ_SSID_NUM            9
#define WF_SCAN_REQ_CHANNEL_NUM         (14 + 37)

enum
{
    WF_SCAN_TAG_ABORT   = WF_MSG_TAG_SET(0, 0, 0),
    WF_SCAN_TAG_START   = WF_MSG_TAG_SET(0, 1, 0),
    WF_SCAN_TAG_DONE,
};

typedef struct
{
    scan_type_e type;
    wf_80211_addr_t bssid;
    wf_wlan_ssid_t ssids[WF_SCAN_REQ_SSID_NUM];
    wf_u8 ssid_num;
    wf_u8 ch_map[MAX_CHANNEL_NUM];
    wf_u8 ch_num;
} wf_scan_req_t;

typedef struct
{
    wf_os_api_sema_t sema;
    wf_u8 ch_idx;
    wf_u8 retry_cnt;
    wf_bool brun;
    wf_timer_t timer, pass_time;
    wf_os_api_sema_t req_lock;
    wf_scan_req_t *preq;
    wf_msg_que_t msg_que;

    struct
    {
        wf_u8 number;
        CHANNEL_WIDTH width;
        HAL_PRIME_CH_OFFSET offset;
    } chnl_bak;
} wf_scan_info_t;


wf_pt_ret_t wf_scan_thrd (wf_pt_t *pt, nic_info_st *pnic_info, int *pres);
int wf_scan_probe_send (nic_info_st *pnic_info);
//int wf_scan_probe_rsp_recv (nic_info_st *pnic_info,
//                            wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
int wf_scan_filter (nic_info_st *pnic_info,
                    wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
int wf_scan_start (nic_info_st *pnic_info, scan_type_e type,
                   wf_80211_bssid_t bssid,
                   wf_wlan_ssid_t ssids[], wf_u8 ssid_num,
                   wf_u8 chs[], wf_u8 ch_num);
int wf_scan_stop (nic_info_st *pnic_info);
int wf_scan_wait_done (nic_info_st *pnic_info, wf_bool babort, wf_u16 to_ms);
wf_bool wf_is_scanning (nic_info_st *pnic_info);
int wf_scan_init (nic_info_st *pnic_info);
int wf_scan_term (nic_info_st *pnic_info);

#endif

