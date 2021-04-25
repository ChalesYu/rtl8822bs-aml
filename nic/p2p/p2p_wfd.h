#ifndef __P2P_WFD_H__
#define __P2P_WFD_H__

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
};

#endif
