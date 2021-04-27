#ifndef __RX_WINDOWS_H__
#define __RX_WINDOWS_H__

#define MAX_RECEIVE_BUFFER_SIZE 32768

#define RECV_QUEUE_DEPTH  64

#define WF_RX_NB_POOL_TAG 'rxnb'
#define WF_RX_NBL_POOL_TAG 'rnbl'


typedef struct data_frame_header_s{
	struct {
		wf_u16 protocol :2;
		wf_u16 type		:2;
		wf_u16 subtype	:4;
		wf_u16 to_ds	:1;
		wf_u16 from_ds	:1;
		wf_u16 more_frag:1;
		wf_u16 retry	:1;
		wf_u16 pwr_mgmt	:1;
		wf_u16 more_data:1;
		wf_u16 protected:1;
		wf_u16 htc		:1;
	}frame_ctrl;
	
	wf_u16 duration;
	wf_u8 addr1[MAC_ADDR_LEN];
	wf_u8 addr2[MAC_ADDR_LEN];
	wf_u8 addr3[MAC_ADDR_LEN];
	wf_u16 seq_ctrl;
	wf_u8 addr4[MAC_ADDR_LEN];
	
	struct {
		wf_u16 tid			:4;
		wf_u16 eosp			:1;
		wf_u16 ack_policy	:2;
		wf_u16 amsdu_present:1;
		wf_u16 amsdu_type	:1;
		wf_u16 rdg			:1;
		wf_u16 buffered_ac	:4;
		wf_u16 reserved		:1;
		wf_u16 ac_constraint:1;
	}qos_ctrl;

	wf_u32 ht_ctrl;
}data_frame_header_t;


typedef struct wf_recv_pkt_s{
	PMDL  mdl;
	PNET_BUFFER nb;
	PNET_BUFFER_LIST nbl;
	DOT11_EXTSTA_RECV_CONTEXT  rx_ctx;
	UCHAR *src_buffer;
	UCHAR *buffer;
	UCHAR rpt_sel;
	rx_pkt_t nic_pkt;
	UCHAR tmp_data[2500];
	WDFMEMORY buf_hdl;

	void *net_if;
	LIST_ENTRY list;
}wf_recv_pkt_t;

typedef struct wf_recv_info_s{
	wf_recv_pkt_t packet[RECV_QUEUE_DEPTH];
	KSPIN_LOCK rxque_lock;
	LIST_ENTRY pend_head;
	LIST_ENTRY free_head;
	NDIS_HANDLE nbl_pool;
	NDIS_HANDLE nb_pool;

	wf_thread_t *rx_thread;
	KEVENT rx_evt;
	WDFWORKITEM rx_workitem;
	void *padapter;
}wf_recv_info_t;


void wf_recv_init(void *param);
void wf_recv_deinit(void *param);
void wf_recv_complete_callback(void *adapter, WDFMEMORY BufferHdl, ULONG data_len, UCHAR offset);



#endif // #ifndef __INC_RECEIVE_H

