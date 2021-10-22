/*
 * wdn.h
 *
 * This file contains all the prototypes for the wdn.c file
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
#ifndef __WDN_H__
#define __WDN_H__
/*
File name: Wireless Device Node
*/
#define SESSION_TRACKER_REG_ID_NUM 1
#define WDN_NUM_MAX         5


typedef wf_bool(*wf_st_match_rule) (nic_info_st *nic_info, wf_u8 * local_naddr,
                                    wf_u8 * local_port, wf_u8 * remote_naddr,
                                    wf_u8 * remote_port);
struct wf_ht_priv
{
    wf_u8 ht_option;
    wf_u8 ampdu_enable;
    wf_u8 tx_amsdu_enable;
    wf_u8 bss_coexist;

    wf_u32 tx_amsdu_maxlen;
    wf_u32 rx_ampdu_maxlen;

    wf_u8 rx_ampdu_min_spacing;

    wf_u8 ldpc;
    wf_u8 both_20m_40m;
    wf_u8 ch_offset;
    wf_u8 sgi_20m;
    wf_u8 sgi_40m;
    wf_u8 dssck_40m;
    wf_u8 smps;
    wf_u8 sm_ps_mode;
    wf_u8 rx_stbc;
    wf_u8 tx_stbc;

    wf_u8 agg_enable_bitmap;
    wf_u8 candidate_tid_bitmap;

    wf_80211_mgmt_ht_cap_t ht_cap;
};


typedef struct
{
    wf_u8 s_proto;
    wf_st_match_rule rule;
} wf_st_register_st;

typedef struct
{
    wf_st_register_st reg[SESSION_TRACKER_REG_ID_NUM];
    wf_que_t tracker_q;
} wf_st_ctl_st;

struct wdninfo_stats
{

    wf_u64 rx_mgnt_pkts;
    wf_u64 rx_beacon_pkts;
    wf_u64 rx_probereq_pkts;
    wf_u64 rx_probersp_pkts;
    wf_u64 rx_probersp_bm_pkts;
    wf_u64 rx_probersp_uo_pkts;
    wf_u64 rx_ctrl_pkts;
    wf_u64 rx_data_pkts;
    wf_u64 rx_data_qos_pkts[WF_TID_NUM];
    wf_u64 last_rx_mgnt_pkts;
    wf_u64 last_rx_beacon_pkts;
    wf_u64 last_rx_probereq_pkts;
    wf_u64 last_rx_probersp_pkts;
    wf_u64 last_rx_probersp_bm_pkts;
    wf_u64 last_rx_probersp_uo_pkts;
    wf_u64 last_rx_ctrl_pkts;
    wf_u64 last_rx_data_pkts;
    wf_u64 last_rx_data_qos_pkts[WF_TID_NUM];


    wf_u64 tx_bytes;
    wf_u64 tx_pkts;
    wf_u64 tx_drops;
    wf_u64 cur_tx_bytes;
    wf_u64 last_tx_bytes;
    wf_u32 cur_tx_tp;

    wf_u64 rx_bytes;
    wf_u64 rx_pkts;
    wf_u64 rx_drops;
    wf_u64 cur_rx_bytes;
    wf_u64 last_rx_bytes;
    wf_u32 cur_rx_tp;
};

struct wdn_xmit_priv
{
    wf_lock_spin lock;
    wf_s32 option;
    wf_s32 apsd_setting;

    // struct tx_servq be_q;
    // struct tx_servq bk_q;
    // struct tx_servq vi_q;
    // struct tx_servq vo_q;
    wf_list_t legacy_dz;
    wf_list_t apsd;

    wf_u16 txseq_tid[16];

};

typedef void (*upload_to_kernel)(nic_info_st *nic_info, void *data);
typedef void (*free_skb_cb)(nic_info_st *nic_info, void *skb);
/* for Rx reordering buffer control */
typedef struct recv_ba_ctrl_
{
    void *nic_node;
    wf_u8  tid;
    wf_u8  enable;
    wf_u16 indicate_seq;/* =wstart_b, init_value = 0xffff */
    wf_u16 wend_b;
    wf_u8  wsize_b;
    wf_u8  ampdu_size;
    wf_que_t pending_reorder_queue;
    wf_que_t free_order_queue;
    wf_os_api_timer_t reordering_ctrl_timer;
    wf_u8 wait_timeout;
    wf_bool timer_start;
    wf_irq  val_irq;
    wf_u16  last_seq_num;
    //void *kernel_id;//kernel thread id
    //char ba_reorder_name[32];
    wf_timer_t expect_time_start;
    int ba_reorder_state;
    wf_u64  drop_pkts;
    wf_u32 timeout_cnt;
    upload_to_kernel upload_func;
    free_skb_cb free_skb;
    wf_lock_t pending_get_de_queue_lock; //for muti-cpu core
} recv_ba_ctrl_st;

typedef enum
{
    E_WDN_AP_STATE_AUTH,
    E_WDN_AP_STATE_ASSOC,
    E_WDN_AP_STATE_8021X_BLOCK,
    E_WDN_AP_STATE_8021X_UNBLOCK,
    E_WDN_AP_STATE_DAUTH,
    E_WDN_AP_STATE_DASSOC,
} wdn_state_e;

typedef struct wdn_net_info_
{
    wf_u8  mac[WF_ETH_ALEN];
    wf_u8 wdn_id;

#ifdef CFG_ENABLE_AP_MODE
    wdn_state_e state;
    wf_pt_t ap_thrd_pt, sub_thrd_pt;
    wf_que_t ap_msg;
    wf_timer_t ap_timer;
    wf_u32 wpa_unicast_cipher;
    wf_u32 rsn_pairwise_cipher;
    wf_u8 wpa_ie[32];
    wf_u16 wpa_ie_len;
    wf_u32 rx_idle_timeout;
#endif
    wf_u32 rx_pkt_stat;
    wf_u32 rx_pkt_stat_last;
    sys_work_mode_e mode;

    wf_u8  bssid[WF_ETH_ALEN];
    wf_u8  ssid[WF_SSID_LEN];
    wf_u8  ssid_len;
    wf_u64 tsf;

    wf_u16  cap_info;
    wf_u8   ess_net;
    wf_u8   ibss_net;
    wf_u8   privacy;
    wf_u8   short_preamble;
    wf_u8   short_slot;
    wf_u8   radio_measure;

    wf_u16  bcn_interval;
    wf_u16  listen_interval;

    wf_u8   datarate[8];
    wf_u8   datarate_len;

    wf_u8   ext_datarate[4];
    wf_u8   ext_datarate_len;

    wf_bool   ht_enable;
    wf_80211_mgmt_ht_cap_t ht_cap;
    struct  wf_ht_priv htpriv;
    wf_u8   HT_protection;
    wf_wlan_ht_op_info_st *ht_info;

    wf_u8   channel;
    CHANNEL_WIDTH   bw_mode;
    wf_u8   channle_offset;

    wf_bool         wmm_enable;
    wf_wmm_para_st wmm_info;
    wf_u8          acm_mask;

    wf_bool   wep_enable;
    wf_bool   wpa_enable;
    wf_bool   rsn_enable;

    wf_bool   erp_enable;
    wf_u8   erp_flag;

//    wf_80211_auth_algo_e auth_algo;
    wf_u32 auth_algo;
    wf_80211_auth_seq_e auth_seq;
    wf_u16  qos_option;
    wf_u32  iv;
    wf_u32  key_index;
    wf_u8   chlg_txt[128];
    wf_bool  ieee8021x_blocked;
    wf_u16  dot118021XPrivacy;
    wf_u16  cam_id;
    union Keytype dot11tkiptxmickey;
    union Keytype dot11tkiprxmickey;
    union Keytype dot118021x_UncstKey;
    union pn48 dot11txpn;
    union pn48 dot11rxpn;

    wf_u32  network_type;

    wf_u8   raid;
    wf_u8   tx_rate;
    que_t   defrag_q;
    wf_u8   defrag_flag;
    wf_u8   defrag_cnt;
    wf_u8   cts2self;
    wf_u8   rtsen;
    wf_u8   ldpc;
    wf_u8   stbc;
    wf_u32  aid;
    struct wdn_xmit_priv wdn_xmitpriv;
    struct wdninfo_stats wdn_stats;

    wf_u8            dialogToken[TID_NUM];
    wf_u8            ba_enable_flag[TID_NUM];
    wf_u8            ba_started_flag[TID_NUM];
    wf_u16           ba_starting_seqctrl[TID_NUM];
    recv_ba_ctrl_st  *ba_ctl;

    wf_u16 seq_ctrl_recorder[16];
    wf_u8 is_p2p_device;
    wf_u8 p2p_status_code;

    wf_u8 dev_addr[WF_ETH_ALEN];
    wf_u8 dev_cap;
    wf_u16 config_methods;
    wf_u8 primary_dev_type[8];
    wf_u8 num_of_secdev_type;
    wf_u8 secdev_types_list[32];
    wf_u16 dev_name_len;
    wf_u8 dev_name[32];

    wf_u8 op_wfd_mode;

} wdn_net_info_st;

typedef struct
{
    wf_list_t list;
    wdn_net_info_st info;
} wdn_node_st;

typedef struct
{
    wf_list_t head; // member is wdn_node_st
    wf_list_t free;
    wf_u8 cnt; /* head node count */
    wf_u32 id_bitmap; /* alloc wdn id bit map */
} wdn_list;

extern wf_u8 WPA_OUI[4];
extern wf_u8 WMM_OUI[4];
extern wf_u8 WPS_OUI[4];
extern wf_u8 P2P_OUI[4];
extern wf_u8 WFD_OUI[4];

wf_u8 wf_wdn_get_raid_by_network_type (wdn_net_info_st *pwdn_info);
wdn_net_info_st *wf_wdn_find_info(nic_info_st *nic_info, wf_u8 *mac);
wdn_net_info_st *wf_wdn_find_info_by_id(nic_info_st *nic_info, wf_u8 wdn_id);
wdn_net_info_st *wf_wdn_add(nic_info_st *nic_info, wf_u8 *mac);
int wf_wdn_remove(nic_info_st *nic_info, wf_u8 *mac);
int wf_wdn_init(nic_info_st *nic_info);
int wf_wdn_term(nic_info_st *nic_info);
int wf_wdn_data_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info);
wf_u8 wf_wdn_is_alive(wdn_net_info_st *wdn_net_info, wf_u8 update_tag);
void get_bratecfg_by_support_dates(wf_u8 *pdataRate, wf_u8 dataRate_len, wf_u16 *pBrateCfg);
wf_u8 wf_wdn_get_cnt(nic_info_st *pnic_info);

#endif

