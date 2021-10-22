#ifndef __RX_WINDOWS_H__
#define __RX_WINDOWS_H__

#define MAX_RECEIVE_BUFFER_SIZE 32768
#define MAX_RECV_PKT_SIZE 2000

#define RECV_MGMT_QUE_DEPTH 8
#define RECV_DATA_QUE_DEPTH 64
#define RECV_QUEUE_DEPTH    (RECV_MGMT_QUE_DEPTH+RECV_DATA_QUE_DEPTH)

#define WF_RX_NB_POOL_TAG 'rxnb'
#define WF_RX_NBL_POOL_TAG 'rnbl'

/*
typedef struct _NDIS_PACKET {
    UCHAR MiniportReservedEx[16];
} NDIS_PACKET, *PNDIS_PACKET;
*/

typedef struct wf_recv_preproc_s{
	WDFMEMORY buf_hdl;
	wf_u8 pkt_type;
	wf_u8 rpt_sel;
	wf_u16 pkt_len;
	wf_u8 *src_buf;
	wf_u8 *pkt_buf;
	wf_u8 hdr_len;
}wf_recv_preproc_t;


typedef struct wf_recv_pkt_s{
	
#if defined(MP_USE_NET_BUFFER_LIST)
	PMDL  mdl;
	PNET_BUFFER nb;
	PNET_BUFFER_LIST nbl;
#else
	PNDIS_PACKET          prPktDescriptor;
#endif
	DOT11_EXTSTA_RECV_CONTEXT  rx_ctx;
	UCHAR *src_buffer;
	UCHAR *buffer;
	UCHAR rpt_sel;
	rx_pkt_t nic_pkt;
	wf_u8* tmp_data;
	WDFMEMORY buf_hdl;

	ULONG idx;
	void *net_if;
	LIST_ENTRY list;
}wf_recv_pkt_t;

typedef struct wf_recv_info_s{
	wf_thread_t *rx_thread;
	wf_thread_t *rx_release_thread;
	KEVENT rx_evt;
	KEVENT rx_release_evt;
	void *padapter;

	wf_data_que_t mgmt_pend;
	wf_data_que_t data_pend;
	wf_data_que_t comm_free;
	wf_data_que_t to_be_released;
		
	LONG proc_cnt;

	wf_recv_pkt_t packet[RECV_QUEUE_DEPTH];

#if defined(MP_USE_NET_BUFFER_LIST)	
	NDIS_HANDLE nbl_pool;
	NDIS_HANDLE nb_pool;
#else
	//NDIS_HANDLE hBufPool;
#endif

}wf_recv_info_t;


NDIS_STATUS wf_recv_init(void *param);
void wf_recv_deinit(void *param);
#pragma LOCKEDCODE 
void wf_recv_complete_callback(void *adapter, WDFMEMORY BufferHdl, ULONG data_len, ULONG offset);

NDIS_STATUS wf_recv_release_source(PADAPTER          padapter, wf_recv_pkt_t *pkt);


#if defined(NDIS60_MINIPORT)
void wf_recv_release_nbl(NDIS_HANDLE        mp_ctx, PNET_BUFFER_LIST nbl, ULONG flag);
#endif


#endif // #ifndef __INC_RECEIVE_H

