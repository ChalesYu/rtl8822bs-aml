/*
 * action.h
 *
 * used for xmit action frame
 *
 * Author: renhaibo
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
#ifndef _ACTION_H
#define _ACTION_H

#define WF_GET_LE16(a) ((wf_u16) (((a)[1] << 8) | (a)[0]))

#define IEEE80211_ADDBA_PARAM_POLICY_MASK       0x0002
#define IEEE80211_ADDBA_PARAM_TID_MASK          0x003C
#define IEEE80211_ADDBA_PARAM_BUF_SIZE_MASK     0xFFC0
#define IEEE80211_DELBA_PARAM_TID_MASK          0xF000
#define IEEE80211_DELBA_PARAM_INITIATOR_MASK    0x0800


#define WF_WLAN_20_40_BSS_COEX_INFO_REQ            BIT(0)
#define WF_WLAN_20_40_BSS_COEX_40MHZ_INTOL         BIT(1)
#define WF_WLAN_20_40_BSS_COEX_20MHZ_WIDTH_REQ     BIT(2)
#define WF_WLAN_20_40_BSS_COEX_OBSS_EXEMPT_REQ     BIT(3)
#define WF_WLAN_20_40_BSS_COEX_OBSS_EXEMPT_GRNT    BIT(4)


enum wf_action_frame_category
{
    WF_WLAN_CATEGORY_SPECTRUM_MGMT = 0,
    WF_WLAN_CATEGORY_QOS = 1,
    WF_WLAN_CATEGORY_DLS = 2,
    WF_WLAN_CATEGORY_BACK = 3,
    WF_WLAN_CATEGORY_PUBLIC = 4,
    WF_WLAN_CATEGORY_RADIO_MEASUREMENT = 5,
    WF_WLAN_CATEGORY_FT = 6,
    WF_WLAN_CATEGORY_HT = 7,
    WF_WLAN_CATEGORY_SA_QUERY = 8,
    WF_WLAN_CATEGORY_UNPROTECTED_WNM = 11,
    WF_WLAN_CATEGORY_TDLS = 12,
    WF_WLAN_CATEGORY_SELF_PROTECTED = 15,
    WF_WLAN_CATEGORY_WMM = 17,
    WF_WLAN_CATEGORY_P2P = 0x7f,
};

enum wf_action_frame_spectrum_actioncode
{
    WF_WLAN_ACTION_SPCT_MSR_REQ = 0,
    WF_WLAN_ACTION_SPCT_MSR_RPRT = 1,
    WF_WLAN_ACTION_SPCT_TPC_REQ = 2,
    WF_WLAN_ACTION_SPCT_TPC_RPRT = 3,
    WF_WLAN_ACTION_SPCT_CHL_SWITCH = 4,
    WF_WLAN_ACTION_SPCT_EXT_CHL_SWITCH = 5,
};

enum wf_action_block_ack_actioncode
{
    WF_WLAN_ACTION_ADDBA_REQ = 0,
    WF_WLAN_ACTION_ADDBA_RESP = 1,
    WF_WLAN_ACTION_DELBA = 2,
};

enum wf_public_action
{
    WF_WLAN_ACTION_PUBLIC_BSSCOEXIST = 0,
    WF_WLAN_ACTION_PUBLIC_DSE_ENABLE = 1,
    WF_WLAN_ACTION_PUBLIC_DSE_DEENABLE = 2,
    WF_WLAN_ACTION_PUBLIC_DSE_REG_LOCATION = 3,
    WF_WLAN_ACTION_PUBLIC_EXT_CHL_SWITCH = 4,
    WF_WLAN_ACTION_PUBLIC_DSE_MSR_REQ = 5,
    WF_WLAN_ACTION_PUBLIC_DSE_MSR_RPRT = 6,
    WF_WLAN_ACTION_PUBLIC_MP = 7,
    WF_WLAN_ACTION_PUBLIC_DSE_PWR_CONSTRAINT = 8,
    WF_WLAN_ACTION_PUBLIC_VENDOR = 9,
    WF_WLAN_ACTION_PUBLIC_GAS_INITIAL_REQ = 10,
    WF_WLAN_ACTION_PUBLIC_GAS_INITIAL_RSP = 11,
    WF_WLAN_ACTION_PUBLIC_GAS_COMEBACK_REQ = 12,
    WF_WLAN_ACTION_PUBLIC_GAS_COMEBACK_RSP = 13,
    WF_WLAN_ACTION_PUBLIC_TDLS_DISCOVERY_RSP = 14,
    WF_WLAN_ACTION_PUBLIC_LOCATION_TRACK = 15,
    WF_WLAN_ACTION_PUBLIC_MAX
};



int wf_action_frame_process (nic_info_st *nic_info, wdn_net_info_st *pwdn_info,
                             wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
int wf_action_frame_ba_to_issue (nic_info_st *nic_info, wf_u8 action);
int wf_action_frame_add_ba_request (nic_info_st *nic_info, struct xmit_frame *pxmitframe);
int wf_action_frame_del_ba_request (nic_info_st *nic_info);
wf_u8 *wf_action_public_to_str(wf_u8 action);
wf_s32 wf_action_frame_parse(wf_u8 * frame, wf_u32 frame_len, wf_u8 * category, wf_u8 * action);

#endif
