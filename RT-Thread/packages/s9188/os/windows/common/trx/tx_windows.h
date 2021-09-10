#ifndef __TX_WINDOWS_H__
#define __TX_WINDOWS_H__

#define XMIT_QUEUE_DEPTH  64

#define MAC_HDR_LEN_SHORT  24

typedef enum wf_que_pos_e{
	QUE_POS_HEAD,
	QUE_POS_TAIL,
	QUE_POS_MAX
}wf_que_pos_e;


typedef struct wf_data_que_t{
	KSPIN_LOCK lock;
	LIST_ENTRY head;
	LONG cnt;
}wf_data_que_t;


typedef struct wf_xmit_pkt_s{
	ULONG data_len;
	UCHAR buffer[2500];
	UCHAR tempbuffer[2500];
	UCHAR *ptempbuf;
	UCHAR is_preproc;
	VOID  *net_intf;

	struct xmit_frame 	xframe;
#if defined(NDIS60_MINIPORT)
	PNET_BUFFER_LIST 	ndis_pkt;
	BOOLEAN				is_last_nb;

#else
	PNDIS_PACKET		ndis_pkt;
#endif
	NDIS_HANDLE 		mp_handle;
	ULONG 				xmit_flag;
	BOOLEAN 			dispatchlevel;
	LIST_ENTRY 			list;
}wf_xmit_pkt_t;

typedef struct wf_xmit_info_s{
	wf_xmit_pkt_t packet[XMIT_QUEUE_DEPTH];

	wf_data_que_t data_pend;
	wf_data_que_t data_free;

	LONG proc_cnt;

	KSPIN_LOCK send_lock;

	wf_thread_t *tx_thread;
	KEVENT tx_evt;
	void *padapter;	

	ULONG		tx_byte;
}wf_xmit_info_t;

void wf_data_que_init(wf_data_que_t *que);
NDIS_STATUS wf_pkt_data_enque(wf_data_que_t *que, PLIST_ENTRY list, wf_u8 pos);
PLIST_ENTRY wf_pkt_data_deque(wf_data_que_t *que, wf_u8 pos);
int wf_pkt_get_hdr_len(wf_u8 *pbuf, wf_u16 pkt_len, wf_u16 *peth_type, wf_u16 *phdr_len);


NDIS_STATUS wf_xmit_init(void *param);
void wf_xmit_deinit(void *param);

#if defined(MP_USE_NET_BUFFER_LIST)

void wf_xmit_ndis_pkt(NDIS_HANDLE mp_ctx, PNET_BUFFER_LIST nbl, NDIS_PORT_NUMBER port_number, ULONG flag);
void wf_xmit_cancel_send(NDIS_HANDLE mp_ctx, PVOID CancelId);
#else
void wf_xmit_ndis_pkt(NDIS_HANDLE       mp_ctx, PPNDIS_PACKET pkt_array, UINT pkt_num);

#endif

void wf_xmit_complet_callback(void *adapter, void *frame);
int wf_xmit_send_complete(nic_info_st *nic_info, wf_xmit_pkt_t *pkt);


#endif // #ifndef __USB_TRANSMIT_H

