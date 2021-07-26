#ifndef __ADHOC_H_
#define __ADHOC_H_
#ifdef CFG_ENABLE_ADHOC_MODE

#define ADHOC_KEEPALIVE_TIMEOUT           (10 * 1000)

typedef struct
{
    wf_os_api_sema_t      sema;
    wf_timer_t            timer;
    wf_bool               adhoc_master;
    wf_u8                 asoc_sta_count;
    wf_mlme_framework_e   framework;
} wf_adhoc_info_t;

typedef wf_u8 beacon_frame_t[WF_80211_MGMT_BEACON_SIZE_MAX];


wf_bool wf_get_adhoc_master (nic_info_st *pnic_info);
wf_bool wf_set_adhoc_master (nic_info_st *pnic_info,wf_bool status);
int wf_adhoc_work (nic_info_st *pnic_info, wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
wf_pt_rst_t adhoc_thrd (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);

wf_pt_rst_t wf_adhoc_prc_bcn (nic_info_st *pnic_info, wf_msg_t *pmsg, wdn_net_info_st *pwdn_info);
void wf_adhoc_wdn_info_update (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);
void wf_adhoc_flush_wdn(nic_info_st *pnic_info);
void wf_adhoc_flush_all_resource(nic_info_st *pnic_info, sys_work_mode_e network_type);
int wf_adhoc_do_probrsp (nic_info_st *pnic_info,
                 wf_80211_mgmt_t *pframe, wf_u16 frame_len);
int wf_adhoc_new_boradcast_wdn (nic_info_st *pnic_info);
int wf_adhoc_ibss_join (nic_info_st * pnic_info, wf_u8 framework, int reason);
int wf_adhoc_send_beacon(nic_info_st *pnic_info);
int wf_adhoc_keepalive_thrd (nic_info_st *pnic_info);
int wf_adhoc_leave_ibss_msg_send (nic_info_st *pnic_info);
int wf_adhoc_term (nic_info_st *pnic_info);
int wf_adhoc_init (nic_info_st *pnic_info);

#endif
#endif
