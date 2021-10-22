 /*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    usb_main.h

Abstract:

    Contains USB structure definitions and function prototypes.

Environment:

    Kernel mode

Author:
--*/


#ifndef __USB_DEV_H__
#define __USB_DEV_H__

//for ctrl trans
#define USB_CTRL_REQ_DEPTH (20)

//for bulk trans
#define USB_DATA_REQ_DEPTH XMIT_QUEUE_DEPTH
#define USB_MGMT_REQ_DEPTH 8 
#define USB_REQ_QUE_DEPTH (USB_DATA_REQ_DEPTH+USB_MGMT_REQ_DEPTH)

#define MAX_PIPES 3
#define MAX_READS 2

#define SIDE_ROAD_BUF_SIZE (256)

typedef struct _USB_CONTROL_REQ_CONTEXT {
    ULONG_PTR         VirtualAddress; // va for next segment of xfer.
    PADAPTER          NdisContext;
    UCHAR             QueueType;  
    UCHAR             Data[sizeof(ULONG)];  
} USB_CONTROL_REQ_CONTEXT, *PUSB_CONTROL_REQ_CONTEXT  ;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USB_CONTROL_REQ_CONTEXT, GetControlRequestContext)

typedef struct _USB_CONTROL_RESOURCES {
    WDFREQUEST      RequestArray[USB_CTRL_REQ_DEPTH];
    WDFSPINLOCK     RequestArrayLock;
    USHORT          MaxOutstandingRequests;
    USHORT          NextAvailableRequestIndex;     
} USB_CONTROL_RESOURCES, *PUSB_CONTROL_RESOURCES;

//
// put this in the NIC data structure
//
typedef struct _USB_DEVICE_CONTEXT {
    WDFUSBPIPE                      InputPipe;
    WDFUSBPIPE                      OutputPipeNormal;
    WDFUSBPIPE                      OutputPipeHigh;
    WDFWAITLOCK                     PipeStateLock;
    size_t                          NumberOfPipes;
    WDFUSBINTERFACE                 UsbInterface;
//    USB_WRITE_RESOURCES             HighWrites;
//    USB_WRITE_RESOURCES             LowWrites;
//	USB_WRITE_RESOURCES				SyncWrites;
    USB_CONTROL_RESOURCES           ControlResources;

    // We don't need this timer in the future.
    //WDFTIMER                        PendRequestTimer;
    ULONG                           ReaderMdlSize;
    //LONG                            ReaderCount;
    //KEVENT                          ReaderWaitEvent;
    BOOLEAN                         DeviceRundown;

} USB_DEVICE_CONTEXT, *PUSB_DEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USB_DEVICE_CONTEXT, GetUsbDeviceContext)


typedef struct _USB_WRITE_REQ_CONTEXT {
    WDFMEMORY         UrbMemory;
    PURB              Urb;
    PMDL              Mdl;
    USBD_PIPE_HANDLE  UsbdPipeHandle ;
    WDFUSBPIPE        UsbPipe;
    PADAPTER          NdisContext;
	void			  *frame;
	void			  *nic_info;
    UCHAR             QueueType;  
    WDFIOTARGET       IoTarget;
} USB_WRITE_REQ_CONTEXT, *PUSB_WRITE_REQ_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USB_WRITE_REQ_CONTEXT, GetWriteRequestContext)


typedef enum wf_queue_map_e{
	WIN_QUE_VO 		= 0,
	WIN_QUE_VI 		= 1,
	WIN_QUE_BE 		= 2,
	WIN_QUE_BK 		= 3,

	WIN_QUE_BCN 	= 4,
	WIN_QUE_MGMT 	= 5,
	WIN_QUE_HIGH 	= 6,
	WIN_QUE_CMD 	= 8
}wf_queue_map_e;



typedef struct wf_usb_req_s{
	UCHAR *data_buf;
	ULONG data_len;

	WDFREQUEST request;
	//USB_WRITE_REQ_CONTEXT context;

	wf_timer_t timer;

	UCHAR addr;
	int (*tx_callback_func)(void*, void*);
	void *tx_info;
	void *param;
	LIST_ENTRY list;
}wf_usb_req_t;

typedef struct wf_usb_info_s{
	wf_usb_req_t async_request[USB_REQ_QUE_DEPTH];
	WDF_REQUEST_SEND_OPTIONS async_req_option;

	wf_usb_req_t sync_request;
	WDF_REQUEST_SEND_OPTIONS sync_req_option;

	WDF_REQUEST_SEND_OPTIONS ctrl_req_option;

	wf_data_que_t mgmt_pend;
	wf_data_que_t mgmt_free;

	wf_data_que_t data_pend;
	wf_data_que_t data_free;

	LONG proc_cnt;
	LONG tx_pending;
	LONG rx_pending;
	//WDFREQUEST ctrl_request[20];

	wf_thread_t *usb_thread;
	KEVENT usb_evt;
	void *padapter;

	KEVENT cmd_finish;
	wf_u8 cmd_buffer[SIDE_ROAD_BUF_SIZE];
	wf_u32 cmd_len;
	
	KEVENT fw_finish;
	wf_u8 fw_buffer[SIDE_ROAD_BUF_SIZE];
	wf_u32 fw_len;
	
	KEVENT reg_finish;
	wf_u8 reg_buffer[SIDE_ROAD_BUF_SIZE];
	wf_u32 reg_len;
}wf_usb_info_t;

UCHAR wf_usb_addr2pipe(UCHAR addr);

NTSTATUS wf_usb_xmit_pkt_sync(PADAPTER adapter, wf_usb_req_t *req);
int wf_usb_xmit_empty(void *adapter);
int wf_usb_xmit_insert(void *adapter,wf_u8 agg_num,char *buff, wf_u32 buff_len, wf_u32 addr, int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param);

void wf_usb_xmit_complet_callback(void *pnic_info, void *frame);


NDIS_STATUS wf_usb_dev_start(PADAPTER padapter);
NDIS_STATUS wf_usb_dev_stop(PADAPTER adapter);
NDIS_STATUS wf_usb_dev_init(void *adapter);
NDIS_STATUS wf_usb_dev_deinit(void *adapter);


#endif


