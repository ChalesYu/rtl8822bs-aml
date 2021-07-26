/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Main.h

Abstract:
    Mp layer major functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef __MP_DEV_H__

#define __MP_DEV_H__

#define MAX_MPDU_LENGTH                      2346
//
// Driver Entry and Exit Points
//
DRIVER_INITIALIZE DriverEntry;

MINIPORT_UNLOAD DriverUnload;



//
// Handlers for Entry Points from NDIS
//

MINIPORT_INITIALIZE MPInitialize;

MINIPORT_RESTART MPRestart;

MINIPORT_PAUSE MPPause;

MINIPORT_HALT MPHalt;

MINIPORT_OID_REQUEST MPRequest;

MINIPORT_CANCEL_OID_REQUEST MPCancelRequest;

MINIPORT_SET_OPTIONS MPSetOptions;

MINIPORT_SEND_NET_BUFFER_LISTS MPSendNetBufferLists;

MINIPORT_RETURN_NET_BUFFER_LISTS MPReturnNetBufferLists;

MINIPORT_CHECK_FOR_HANG MPCheckForHang;

MINIPORT_RESET MPReset;

MINIPORT_CANCEL_SEND MPCancelSendNetBufferLists;

MINIPORT_DEVICE_PNP_EVENT_NOTIFY MPDevicePnPEvent;

MINIPORT_SHUTDOWN MPAdapterShutdown;


INLINE 
BOOLEAN
MpRemoveAdapter(
    __in PADAPTER pAdapter
    );


//
// Functions that will be used by Miniport internally.
// Mostly consist of helper API. Their implementation
// is spread around in other files.
//


NDIS_STATUS
MpInitializeSendEngine(
    __in PADAPTER            Adapter
    );

VOID
MpReinitializeSendEngine(
    __in PADAPTER     Adapter
    );

VOID
MpTerminateSendEngine(
    __in PADAPTER Adapter
    );

BOOL
MPCanTransmit(
    __in PADAPTER         Adapter
    );

//PMP_TX_MSDU
//MPReserveTxResources(
//    __in PADAPTER                 Adapter,
//    __in PNET_BUFFER_LIST         NetBufferList
//    );
//
//VOID
//MpSendTxMSDUs(
//    __in PADAPTER         Adapter,
//    __in PMP_TX_MSDU      pTxd,
//    __in ULONG            NumTxd,
//    __in BOOLEAN          DispatchLevel
//    );
//
//NDIS_STATUS
//MpSendSingleTxMSDU(
//    __in PADAPTER         Adapter,
//    __in PMP_TX_MSDU      pTxd,
//    __in BOOLEAN          DispatchLevel
//    );
//
//NDIS_STATUS
//MpTransmitTxMSDU(
//    __in PADAPTER         Adapter,
//    __in PMP_TX_MSDU      pTxd,
//    __in BOOLEAN          DispatchLevel
//    );

VOID
MpSendReadyTxMSDUs(
    __in  PADAPTER        Adapter,
    __in BOOLEAN          DispatchLevel
    );

VOID
MpHandleSendCompleteInterrupt(
    __in PADAPTER         Adapter
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
MpCompleteQueuedTxMSDUs(
    __in PADAPTER    Adapter
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
MpCompleteQueuedNBL(
    __in PADAPTER    Adapter
    );

NDIS_STATUS
MpGetAdapterStatus(
    __in PADAPTER         Adapter
    );

 
 __drv_requiresIRQL(DISPATCH_LEVEL)
NDIS_STATUS
MpResetInternalRoutine(
    __in PADAPTER             Adapter,
    __in MP_RESET_TYPE        ResetType
    ) ;

EVT_WDF_WORKITEM MpDot11ResetWorkItem;

EVT_WDF_WORKITEM MpNdisResetWorkItem;

VOID
MpDot11ResetComplete(
    __in  PADAPTER                Adapter,
    __in  PDOT11_RESET_REQUEST    pDot11ResetRequest
    );


//VOID
//MpHandleDefaultReceiveInterrupt(
//    __in PADAPTER         Adapter,
//    PNIC_RX_FRAGMENT    NicFragment,
//    __in size_t           Size  
//    );
//
//VOID
//MpHandleRawReceiveInterrupt(
//    __in PADAPTER         Adapter,
//    PNIC_RX_FRAGMENT    NicFragment,
//    __in size_t           Size
//    ) ;
//
//VOID
//MpHandleSafeModeReceiveInterrupt(
//    __in PADAPTER         Adapter,
//    PNIC_RX_FRAGMENT    NicFragment,
//    __in size_t           Size
//    );

VOID
MpExpireReassemblingPackets(
    __in  PADAPTER    Adapter,
    __in  BOOLEAN     DispatchLevel
    );

VOID
MpPSPacketsManagerDeliverDTIMPackets(
    __in  PADAPTER                    Adapter
    );

VOID
MpResetMacMIBs(
    __in  PADAPTER    Adapter
    );

VOID
MpResetPhyMIBs(
    __in  PADAPTER    Adapter
    );

VOID
MpAdjustReceiveHandler(
    __in PADAPTER         pAdapter
    );

NDIS_STATUS  InitializeHw(PADAPTER pAdapter);

static int read_fw_file(PADAPTER                pAdapter);

NDIS_STATUS
MpInitializeNicVariable
(
	__in  PADAPTER pAdapter);

NDIS_STATUS
SetNic80211Attributes
(__in  PADAPTER pAdapter,
	__out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr);

NDIS_STATUS SetSta80211Attributes(__in  PADAPTER pAdapter,
	__out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr);

WDFDEVICE
Mp11GetWdfDevice(
	__in  PADAPTER		  pAdapter
	);

#define HW11_MEMORY_TAG                     'RICH'


NTSTATUS
MpDriverEntryNdis6(
	PDRIVER_OBJECT DriverObject,
	PUNICODE_STRING RegistryPath
	);


#endif  // _NATIVE_WIFI_MAIN_H_

