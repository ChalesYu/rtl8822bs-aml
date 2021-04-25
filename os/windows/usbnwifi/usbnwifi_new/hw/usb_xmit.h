#ifndef __USB_TRANSMIT_H
#define __USB_TRANSMIT_H




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
} USB_WRITE_REQ_CONTEXT, *PUSB_WRITE_REQ_CONTEXT  ;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USB_WRITE_REQ_CONTEXT, GetWriteRequestContext)


NTSTATUS
HwUsbSendPacketAsync (
	PADAPTER    adapter,
    UCHAR           QueueType,
    ULONG           BufferLength,   // Number of bytes in pData buffer
    PVOID           Buffer
    );

BOOL
HwUsbAllocateWriteRequests(
	PADAPTER    adapter,
    BYTE    MaxOutstanding      
    );

VOID
HwUsbFreeXmitMemory(
	PADAPTER    adapter
    );


NDIS_STATUS
HwUsbAllocateXmitResources(
    __in PADAPTER    adapter,
    __in  ULONG NumTxd
    );


VOID
HwUsbFreeWriteRequests(
	PADAPTER    adapter
    );


NDIS_STATUS
HwUsbXmitStart( 
	PADAPTER    adapter
    );

#endif // #ifndef __USB_TRANSMIT_H

