/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Power.h

Abstract:
    NDIS Power management handling interfaces
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "pcomp.h"
#include "wf_debug.h"
#ifdef DOT11_TRACE_ENABLED
#include "Mp_Power.tmh"
#endif

// Called via SynchronizeWithInterrupt (ie. at high IRQL)
VOID
MpSetHardwarePowerState(
    __in  PADAPTER        pAdapter
    )
{
    NDIS_DEVICE_POWER_STATE DevicePowerState;

    DevicePowerState = pAdapter->NextDevicePowerState;

    //
    // The current DevicePowerState has been set to NewDeviceState before
    // calling the hardware intentionally. The call to Hw11SetDevicePowerState
    // cannot fail. Before the call returns, Interrupts may be enabled for this
    // device (when going to D0) and the MPISR will reject it if it thinks that
    // the hardware is still in low power state.
    //
    pAdapter->DevicePowerState = DevicePowerState;
    LOG_D("Setting device to power state D%d", DevicePowerState);
    //Hw11SetDevicePowerState(pAdapter->Nic, DevicePowerState);
}


NDIS_STATUS
MpSetPower(
    __in  PADAPTER        pAdapter,
    __in  PVOID           InformationBuffer,
    __in  ULONG           InformationBufferLength,
    __in  PULONG          pulBytesNeeded,
    __in  PULONG          pulBytesRead
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_DEVICE_POWER_STATE     NewDeviceState;

    do
    {
        //
        // Store new power state and succeed the request.
        //
        *pulBytesNeeded = sizeof(NDIS_DEVICE_POWER_STATE);
        if (InformationBufferLength < *pulBytesNeeded)
        {
            //
            // Too little data. Bail out.
            //
            LOG_E("Bad length received for the SetPower request");
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        
        NdisMoveMemory(&NewDeviceState, InformationBuffer, sizeof(NDIS_DEVICE_POWER_STATE));
        *pulBytesRead = sizeof(NDIS_DEVICE_POWER_STATE);
        
        if (NewDeviceState <= NdisDeviceStateUnspecified || NewDeviceState >= NdisDeviceStateMaximum)
        {
            //
            // An invalid device state has been specified. Bail out.
            //
            LOG_E("Invalid data received in Set Power request");
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }
        
        if (NewDeviceState == pAdapter->DevicePowerState)
        {
            //
            // We are already in the device state being set. Succeed the call.
            //
            MPASSERTMSG(
                "The device is already in the Power state it is being asked to go to\n",
                FALSE
                );
            LOG_D("Device is already in power state D%d", NewDeviceState);
            ndisStatus = NDIS_STATUS_SUCCESS;
            break;
        }

        //
        // Remember the device power state we are transitioning to.
        //
        pAdapter->NextDevicePowerState = NewDeviceState;

        if (NewDeviceState != NdisDeviceStateD0)
        {
            //
            // We are going to sleep, inform the station. This happens before we go to sleep
            //
            //Sta11PowerSleep(pAdapter->Station, NewDeviceState);
        }

        MpSetHardwarePowerState(pAdapter);
        
        if (NewDeviceState == NdisDeviceStateD0)
        {
            //
            // Do an internal reset to start fresh
            //
            Mp11InternalReset(pAdapter, MP_RESET_TYPE_INTERNAL_RESET);

            //
            // Inform the station that we just came up from a power save state
            //
            //Sta11PowerWakeup(pAdapter->Station);
        }

    } while(FALSE);
    
    return ndisStatus;
}


NDIS_STATUS
MpQueryPnPCapabilities(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    *BytesWritten = 0;
    *BytesNeeded = 0;

    if (InformationBufferLength < sizeof(NDIS_PNP_CAPABILITIES)) {
        *BytesNeeded = sizeof(NDIS_PNP_CAPABILITIES);
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        BAIL_ON_NDIS_ERROR(ndisStatus);
    }    

    //Hw11QueryPnPCapabilities(pAdapter->Nic, (PNDIS_PNP_CAPABILITIES)InformationBuffer);
    
    *BytesWritten = sizeof(NDIS_PNP_CAPABILITIES);
    
error:
    return ndisStatus;
}



NDIS_STATUS
MpAddWakeUpPattern(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(InformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(BytesNeeded);
    UNREFERENCED_PARAMETER(BytesRead);

    return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS
MpRemoveWakeUpPattern(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(InformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(BytesNeeded);
    UNREFERENCED_PARAMETER(BytesRead);

    return NDIS_STATUS_SUCCESS;
}

