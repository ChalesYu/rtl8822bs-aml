/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_bus_interface.c

Abstract:
    
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/

#include "pcomp.h"
#include "wf_debug.h"

#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int


NDIS_STATUS
HwInitializeBus(
    __in PADAPTER adapter
    )
{
    NDIS_STATUS ndisStatus;

    ndisStatus =  HwUSBSpecificInit(adapter);
    //InitializeVariables here
    //tod do i need this here 
    //InitializeVariables(Nic);
    return ndisStatus ;
}

VOID
HwDeInitBus(
    __in PADAPTER adapter
    )
{
    HwUsbDeInit(adapter);
}

NDIS_STATUS
HwReadAdapterInfo(
    __in PADAPTER adapter
    )
{
    NDIS_STATUS  ndisStatus = 0;
    
    //ndisStatus = ReadNicInfo8187(adapter);
    return ndisStatus;
}



NDIS_STATUS
HwInitializeAdapter(
    __in PADAPTER adapter
    )
{
    //
    //channel will come from reg stetings   . Initialize to channel 1
    //
	return 0;//InitializeNic8187(adapter, 1);
}

NDIS_STATUS
HwBusAllocateRecvResources(
    __in PADAPTER adapter
    )

{
    return HwUsbAllocateRecvResources(adapter);
}

NDIS_STATUS
HwBusAllocateXmitResources(
    __in PADAPTER adapter ,
    __in  ULONG NumTxd
    )

{
    return HwUsbAllocateXmitResources(adapter, NumTxd);
}



VOID
HwBusFreeXmitResources(
    __in PADAPTER adapter
    )
{
    HwUsbFreeXmitMemory(adapter);
    return ;

}


NDIS_STATUS
HwBusFreeRecvResources(
    __in PADAPTER adapter
    )
{
    return HwUsbFreeRecvResources(adapter);
}

NDIS_STATUS
HwBusFindAdapterAndResources(
    __in PADAPTER adapter
    )
{
    UNREFERENCED_PARAMETER(adapter);
    
    return NDIS_STATUS_SUCCESS;
    //no op for usb
}

NDIS_STATUS
HwBusStartDevice(
                __in PADAPTER adapter
                 )
{
    NDIS_STATUS ndisStatus;
    ndisStatus = HwUsbRecvStart(adapter);
    if (ndisStatus != NDIS_STATUS_SUCCESS) {
        LOG_E("Failed to start Recv IoTarget.");
    }
    ndisStatus = HwUsbXmitStart(adapter);
    if (ndisStatus != NDIS_STATUS_SUCCESS) {
        LOG_E("Failed to  start Transmit IoTarget.");
    }
    LOG_D(" Start all pipes!");
    return ndisStatus;
}

NDIS_STATUS
HwBusSendPacketAsync(
    __in PADAPTER adapter,
    __in UCHAR            QueueType,
    __in ULONG            BufferLength,
    __in PVOID            Buffer
    )
{
    NTSTATUS ntStatus;
    NDIS_STATUS ndisStatus;

    ntStatus =  HwUsbSendPacketAsync(adapter,  QueueType, BufferLength, Buffer);

    NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
    return    ndisStatus;    
}

VOID
HwBusStopNotification(
    __in PADAPTER adapter
    )
{
    HwUsbStopAllPipes(adapter);
}


VOID
HwBusFreeRecvFragment(
                __in  PADAPTER adapter,
                __in  PNIC_RX_FRAGMENT NicFragment
                ) 
{
    HwUsbFreeRecvFragment(adapter,NicFragment );
}



