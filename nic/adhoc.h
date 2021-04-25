#ifndef __ADHOC_H_
#define __ADHOC_H_
#ifdef CFG_ENABLE_ADHOC_MODE

wf_bool get_adhoc_master (nic_info_st *pnic_info);
wf_bool set_adhoc_master (nic_info_st *pnic_info,wf_bool status);
int adhoc_work (nic_info_st *pnic_info, wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
wf_pt_rst_t wf_beacon_adhoc_thrd (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);
void adhoc_wdn_info_update (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);
int wf_adhoc_do_probrsp (nic_info_st *pnic_info,
                 wf_80211_mgmt_t *pframe, wf_u16 frame_len);
int wf_proc_ibss_join(nic_info_st * pnic_info);
int wf_adhoc_send_beacon(nic_info_st *pnic_info);
#endif
#endif
