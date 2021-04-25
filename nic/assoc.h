
#ifndef __ASSOC_H__
#define __ASSOC_H__

/* macro */

/* type define */
enum
{
    WF_ASSOC_TAG_RSP     = WF_MSG_TAG_SET(0, 0, 0),
    WF_ASSOC_TAG_ABORT   = WF_MSG_TAG_SET(0, 1, 0),
    WF_ASSOC_TAG_START   = WF_MSG_TAG_SET(0, 2, 0),
    WF_ASSOC_TAG_DONE,
};

typedef wf_u8 assoc_rsp_t[WF_80211_MGMT_ASSOC_SIZE_MAX];
typedef struct
{
    wdn_net_info_st *wdn_info;
    wf_os_api_sema_t sema;
    wf_msg_que_t msg_que;
    wf_u8 retry_cnt;
    wf_timer_t timer;
    wf_bool brun;
} assoc_info_t;

/* function declaration */
#ifdef CFG_ENABLE_AP_MODE
int wf_assoc_ap_work (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                      wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
wf_pt_rst_t wf_assoc_ap_thrd (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);
void wf_ap_add_sta_ratid (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);
void wf_assoc_ap_event_up (nic_info_st *nic_info, wdn_net_info_st *pwdn_info,
                        wf_ap_msg_t *pmsg);
#endif
int wf_disassoc_frame_parse (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                             wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
int wf_assoc_frame_parse (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info,
                          wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
wf_pt_rst_t wf_assoc_sta_thrd (wf_pt_t *pt, nic_info_st *pnic_info, int *pres);
int wf_assoc_start (nic_info_st *pnic_info);
int wf_assoc_stop (nic_info_st *pnic_info);
int wf_assoc_init (nic_info_st *pnic_info);
int wf_assoc_term (nic_info_st *pnic_info);

#endif

