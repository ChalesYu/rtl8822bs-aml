#ifndef __P2P_WFD_H__
#define __P2P_WFD_H__

#define MAX_WFD_IE_LEN (128)
typedef enum
{
    WF_WFD_IE_BEACON        = 0,
    WF_WFD_IE_PROBE_REQ     = 1,
    WF_WFD_IE_PROBE_RSP     = 2,
    WF_WFD_IE_GO_PROBE_RSP  = 3,
    WF_WFD_IE_ASSOC_REQ     = 4,
    WF_WFD_IE_ASSOC_RSP     = 5,
    WF_WFD_IE_MAX,
}WF_WFD_IE_E;


enum SCAN_RESULT_TYPE {
	SCAN_RESULT_P2P_ONLY = 0,
	SCAN_RESULT_ALL = 1,
	SCAN_RESULT_WFD_TYPE = 2
};

struct wifi_display_info {
	wf_u16 wfd_enable;
    wf_u16 init_rtsp_ctrlport;
    wf_u16 rtsp_ctrlport;
    wf_u16 peer_rtsp_ctrlport;

    wf_u8 peer_session_avail;
    wf_u8 ip_address[4];
    wf_u8 peer_ip_address[4];
    wf_u8 wfd_pc;

    wf_u8 wfd_device_type;
	enum SCAN_RESULT_TYPE scan_result_type;
    wf_u8 op_wfd_mode;
    wf_u8 stack_wfd_mode;

    wf_u8 *wfd_ie[WF_WFD_IE_MAX];
    wf_u32 wfd_ie_len[WF_WFD_IE_MAX];
};

void wfd_enable(nic_info_st *pnic_info, wf_bool on);
void wfd_set_ctrl_port(nic_info_st *pnic_info, wf_u16 port);
int wifi_display_info_to_init_func(nic_info_st *pnic_info, wf_u8 flag);
wf_u8 *wfd_attr_content_to_get_func(wf_u8 * wfd_ie, wf_u32 wfd_ielen, wf_u8 target_attr_id,wf_u8 * buf_content, wf_u32 * len_content, wf_u8 flag);
wf_u8 *wfd_ie_to_get_func(wf_u8 flag, wf_u8 * in_ie, int in_len, wf_u8 * wfd_ie, wf_u32 * wfd_ielen);
wf_u32 probe_req_wfd_ie_to_append_func(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 flag);
wf_u32 probe_resp_wfd_ie_to_append_func(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 flag);
wf_u32 assoc_req_wfd_ie_to_append_func(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 flag);
wf_u32 assoc_resp_wfd_ie_to_append_func(nic_info_st * pnic_info, wf_u8 * pbuf, wf_u8 flag);
wf_s32 wf_p2p_wfd_update_ie(nic_info_st *pnic_info, WF_WFD_IE_E ie_type, wf_u8 * ie,wf_u32 ie_len, wf_u8 tag);
wf_bool wf_p2p_wfd_is_valid(nic_info_st *pnic_info);

#endif
