#ifndef __TX_WINDOWS_H__
#define __TX_WINDOWS_H__

#define XMIT_QUEUE_DEPTH  64

typedef struct wf_xmit_pkt_s{
	ULONG data_len;
	UCHAR buffer[2500];
	UCHAR tempbuffer[2500];
	UCHAR *ptempbuf;
	UCHAR is_preproc;

	struct xmit_frame xframe;
	PNET_BUFFER_LIST nbl;
	BOOLEAN dispatchlevel;
	LIST_ENTRY list;
}wf_xmit_pkt_t;

typedef struct wf_data_que_t{
	KSPIN_LOCK lock;
	LIST_ENTRY head;
	LONG cnt;
}wf_data_que_t;

typedef struct wf_xmit_info_s{
	wf_xmit_pkt_t packet[XMIT_QUEUE_DEPTH];

	wf_data_que_t data_pend;
	wf_data_que_t data_free;

	wf_thread_t *tx_thread;
	KEVENT tx_evt;
	void *padapter;	
}wf_xmit_info_t;

void wf_data_que_init(wf_data_que_t *que);

void wf_xmit_init(void *param);
void wf_xmit_deinit(void *param);
void wf_xmit_ndis_nbl(NDIS_HANDLE mp_ctx, PNET_BUFFER_LIST nbl, NDIS_PORT_NUMBER port_number, ULONG flag);
void wf_xmit_cancel_send(NDIS_HANDLE mp_ctx, PVOID CancelId);

void wf_xmit_complet_callback(void *adapter, void *frame);


#endif // #ifndef __USB_TRANSMIT_H

