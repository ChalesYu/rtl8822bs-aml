#ifndef __RX_WINDOWS_H__
#define __RX_WINDOWS_H__

#define MAX_RECEIVE_BUFFER_SIZE 32768

#define RECV_MGMT_QUE_DEPTH 8
#define RECV_DATA_QUE_DEPTH 64
#define RECV_QUEUE_DEPTH    (RECV_MGMT_QUE_DEPTH+RECV_DATA_QUE_DEPTH)

#define WF_RX_NB_POOL_TAG 'rxnb'
#define WF_RX_NBL_POOL_TAG 'rnbl'



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

	wf_data_que_t mgmt_pend;
	wf_data_que_t data_pend;
	wf_data_que_t comm_free;
	
	NDIS_HANDLE nbl_pool;
	NDIS_HANDLE nb_pool;

	wf_thread_t *rx_thread;
	KEVENT rx_evt;
	void *padapter;
}wf_recv_info_t;


void wf_recv_init(void *param);
void wf_recv_deinit(void *param);
void wf_recv_complete_callback(void *adapter, WDFMEMORY BufferHdl, ULONG data_len, ULONG offset);
void wf_recv_release_nbl(NDIS_HANDLE        mp_ctx, PNET_BUFFER_LIST nbl, ULONG flag);



#endif // #ifndef __INC_RECEIVE_H

