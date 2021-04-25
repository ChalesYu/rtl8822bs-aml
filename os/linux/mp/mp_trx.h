#ifndef __MP_TRX_H__
#define __MP_TRX_H__

#ifdef CONFIG_MP_MODE
enum WF_NEW_MGN_RATE {
	WL_MGN_1M = 0x00,
	WL_MGN_2M = 0x01,
	WL_MGN_5_5M = 0x02,
	WL_MGN_11M = 0x3,
	WL_MGN_6M = 0x80,
	WL_MGN_9M = 0x81,
	WL_MGN_12M = 0x82,
	WL_MGN_18M = 0x83,
	WL_MGN_24M = 0x84,
	WL_MGN_36M = 0x85,
	WL_MGN_48M = 0x86,
	WL_MGN_54M = 0x87,
	WL_MGN_MCS0 = 0x100,
	WL_MGN_MCS1 = 0x101,
	WL_MGN_MCS2 = 0x102,
	WL_MGN_MCS3 = 0x103,
	WL_MGN_MCS4 = 0x104,
	WL_MGN_MCS5 = 0x105,
	WL_MGN_MCS6 = 0x106,
	WL_MGN_MCS7 = 0x107,
};


typedef struct _wf_mp_tx {
	wf_u8 stop;
	wf_u32 count, sended;
	wf_u8 payload;
	struct xmit_frame attrib;
	wf_u8 desc[TXDESC_SIZE];
	wf_u8 *pallocated_buf;
	wf_u8 *buf;
	wf_u32 buf_size, write_size;
	void *PktTxThread;
}wf_mp_tx;


int wf_mp_test_tx(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
int wf_mp_test_rx(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
int wf_mp_stats(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);

int wf_mp_rx_common_process( nic_info_st *pnic_info, wf_u8 *pktBuf, wf_u32 pktLen);
int wf_mp_rate_to_rateidx(wf_u32 rate);
int wf_mp_fw_download(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
int wf_mp_cmd_download(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);

#endif

#endif
