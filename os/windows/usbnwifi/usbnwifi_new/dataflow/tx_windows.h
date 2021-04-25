#ifndef __TX_WINDOWS_H__
#define __TX_WINDOWS_H__

#include "wf_os_api.h"

#define XMIT_QUEUE_DEPTH  64
#define USB_REQ_QUE_DEPTH 72

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

typedef struct wf_usb_req_s{
	UCHAR *data_buf;
	ULONG data_len;

	wf_timer_t timer;

	USHORT addr;
	int (*tx_callback_func)(void*, void*);
	void *tx_info;
	void *param;
	LIST_ENTRY list;
}wf_usb_req_t;

typedef struct wf_xmit_info_s{
	wf_xmit_pkt_t packet[XMIT_QUEUE_DEPTH];
	KSPIN_LOCK data_pend_lock;
	KSPIN_LOCK data_free_lock;
	LIST_ENTRY pend_head;
	LIST_ENTRY free_head;
	LONG data_pend_cnt;
	LONG data_free_cnt;
	WDFWORKITEM tx_workitem;
	wf_thread_t *tx_thread;
	KEVENT tx_evt;

	wf_thread_t *usb_thread;
	KEVENT usb_evt;
	void *padapter;

	wf_usb_req_t usb_request[USB_REQ_QUE_DEPTH];
	
	KSPIN_LOCK usb_free_lock;
	LIST_ENTRY usb_free_head;
	LONG usb_free_cnt;

	KSPIN_LOCK usb_data_lock;
	LIST_ENTRY usb_data_head;
	LONG usb_data_cnt;

	KSPIN_LOCK usb_mgmt_lock;
	LIST_ENTRY usb_mgmt_head;
	LONG usb_mgmt_cnt;
}wf_xmit_info_t;

void wf_xmit_init(void *param);
void wf_xmit_deinit(void *param);
void wf_xmit_ndis_nbl(void *dev_ctx, void *nbl, ULONG flag);

int wf_xmit_list_empty(void *adapter);
int wf_xmit_list_insert(void *adapter,wf_u8 agg_num,char *buff, wf_u32 buff_len, wf_u32 addr, int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param);

void wf_xmit_complet_callback(void *adapter, void *frame);


#endif // #ifndef __USB_TRANSMIT_H

