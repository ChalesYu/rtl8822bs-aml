/*
 * ap.h
 *
 * This file contains all the prototypes for the ap.c file
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __AP_H__
#define __AP_H__

#define BEACON_IE_OFFSET      12
#define _PUBLIC_ACTION_IE_OFFSET_   8
#define _FIXED_IE_LENGTH_ BEACON_IE_OFFSET
#ifdef CFG_ENABLE_AP_MODE

typedef enum
{
    WF_AP_STATUS_UNINITILIZED = 0,
    WF_AP_STATUS_STANBY,
    WF_AP_STATUS_ESTABLISHED,
} wf_ap_status;

typedef enum
{
    WF_AP_MSG_TAG_AUTH_FRAME,
    WF_AP_MSG_TAG_DEAUTH_FRAME,
    WF_AP_MSG_TAG_ASSOC_REQ_FRAME,
    WF_AP_MSG_TAG_DISASSOC_FRAME,
    WF_AP_MSG_TAG_BA_REQ_FRAME,
    WF_AP_MSG_TAG_BEACON_FRAME,

    WF_AP_MSG_TAG_MAX,
} wf_ap_msg_tag_e;

typedef struct
{
    wf_que_list_t list;
    wf_ap_msg_tag_e tag;
    wf_u16 len;
    union
    {
        wf_80211_mgmt_t mgmt;
    };
} wf_ap_msg_t;

void wf_ap_status_set (nic_info_st *pnic_info, wf_ap_status ap_state);
wf_ap_status wf_ap_status_get (nic_info_st *pnic_info);
int wf_ap_msg_load (nic_info_st *pnic_info, wf_que_t *pque_tar,
                    wf_ap_msg_tag_e tag, void *pdata, wf_u16 len);
wf_ap_msg_t *wf_ap_msg_get (wf_que_t *pque);
int wf_ap_msg_free (nic_info_st *pnic_info, wf_que_t *pque, wf_ap_msg_t *pmsg);
int wf_ap_init (nic_info_st *pnic_info);
int wf_ap_probe (nic_info_st *pnic_info,
                 wf_80211_mgmt_t *pframe, wf_u16 frame_len);
int wf_ap_set_beacon (nic_info_st *pnic_info, wf_u8 *pbuf, wf_u32 len, wf_u8 framework);
int wf_ap_pre_set_beacon (nic_info_st *pnic_info, ieee_param *param, wf_u32 len);
void update_beacon(nic_info_st *pnic_info, wf_u8 ie_id, wf_u8 * oui, wf_u8 tx);

int wf_ap_work_start (nic_info_st *pnic_info);
int wf_ap_work_stop (nic_info_st *pnic_info);
int wf_ap_set_encryption(nic_info_st *pnic_info, ieee_param *param, wf_u32 param_len);
int wf_ap_get_sta_wpaie (nic_info_st *pnic_info, ieee_param *param, wf_u32 len);
int wf_ap_remove_sta (nic_info_st *pnic_info, ieee_param *param, wf_u32 len);
int wf_ap_deauth_all_sta (nic_info_st *pnic_info, wf_u16 reason);
void wf_resend_bcn(nic_info_st *pnic_info,wf_u8 channel);
void wf_ap_reset_beacon_channel(nic_info_st *pnic_info,wf_u8 channel);
int wf_ap_deauth_all_sta (nic_info_st *pnic_info, wf_u16 reason);

#endif

#endif/* __AP_H__ */

