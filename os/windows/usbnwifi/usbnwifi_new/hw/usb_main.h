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


#pragma once

#define TOTAL_DESC_NORMAL_QUEUE 20
#define TOTAL_DESC_HIGH_QUEUE   4

#define MAX_PIPES 3
#define MAX_READS 2
#define MAX_WRITE_USB_REQUESTS (HW_MAX_TX_MSDUS + TOTAL_DESC_NORMAL_QUEUE + TOTAL_DESC_HIGH_QUEUE)
#define MAX_CONTROL_REQUESTS 20
#define USB_HIGH_SPEED_BULK_SIZE	512
#define USB_FULL_SPEED_BULK_SIZE	64

typedef struct _USB_WRITE_RESOURCES {
    WDFREQUEST      WriteRequestArray[MAX_WRITE_USB_REQUESTS];
    WDFSPINLOCK     WriteRequestArrayLock;
    USHORT          MaxOutstandingWrites;
    USHORT          NextAvailableRequestIndex;   
} USB_WRITE_RESOURCES, *PUSB_WRITE_RESOURCES;


typedef struct _USB_CONTROL_REQ_CONTEXT {
    ULONG_PTR         VirtualAddress; // va for next segment of xfer.
    PADAPTER          NdisContext;
    UCHAR             QueueType;  
    UCHAR             Data[sizeof(ULONG)];  
} USB_CONTROL_REQ_CONTEXT, *PUSB_CONTROL_REQ_CONTEXT  ;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USB_CONTROL_REQ_CONTEXT, GetControlRequestContext)

typedef struct _USB_CONTROL_RESOURCES {
    WDFREQUEST      RequestArray[MAX_CONTROL_REQUESTS];
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
    USB_WRITE_RESOURCES             HighWrites;
    USB_WRITE_RESOURCES             LowWrites;
	USB_WRITE_RESOURCES				SyncWrites;
    USB_CONTROL_RESOURCES           ControlResources;

    // We don't need this timer in the future.
    //WDFTIMER                        PendRequestTimer;
    ULONG                           ReaderMdlSize;
    LONG                            ReaderCount;
    KEVENT                          ReaderWaitEvent;
    BOOLEAN                         DeviceRundown;

} USB_DEVICE_CONTEXT, *PUSB_DEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USB_DEVICE_CONTEXT, GetUsbDeviceContext)

typedef struct _PIPE_CONTEXT {
    size_t      MaxPacketLength;
    BYTE        MaxOutstandingWrites;
    WDFREQUEST* pAvailableRequests;
    WDFSPINLOCK RequestArrayLock;
    BYTE        NextAvailableRequestIndex;
    BOOL        IsStarted;
    LONG        ReferenceCount;    
}   PIPE_CONTEXT, *PPIPE_CONTEXT;



NDIS_STATUS
HwUSBSpecificInit(
    __in PADAPTER adapter
    );

NTSTATUS
HwUsbCreateInitializeUsbDevice(
    __in PADAPTER adapter
    );

VOID
HwUsbStopAllPipes(
    __in PADAPTER adapter
    );

VOID
HwUsbDeInit(
    __in PADAPTER adapter
    );

NTSTATUS
HwUsbSendVendorControlPacketAsync (
    __in PADAPTER adapter,
    __in BYTE Request, 
    __in USHORT Value,
    __in ULONG Length,
    __in PVOID Buffer,
    __in USHORT Index    
    );






