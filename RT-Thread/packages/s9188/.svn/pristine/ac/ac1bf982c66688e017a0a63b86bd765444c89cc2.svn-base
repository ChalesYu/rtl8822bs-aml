/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Oids.c

Abstract:
    Oid processing code
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "pcomp.h"
#include "wf_debug.h"

#ifndef OID_DOT11_HIDDEN_NETWORK_ENABLED
//
// This OID is not yet in the DDK
//
#define OID_DOT11_HIDDEN_NETWORK_ENABLED            NWF_DEFINE_OID(158,NWF_OPERATIONAL_OID,NWF_MANDATORY_OID)
#endif

     
PCHAR 
DbgGetOidName(
    ULONG Oid
    );

NDIS_OID MPSupportedOids[] =
{
    //
    // Private OIDs
    //
    MP_PRIVATE_OID_LIST
    
    OID_GEN_SUPPORTED_LIST,
    OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED,
    OID_GEN_MEDIA_IN_USE,
    OID_GEN_PHYSICAL_MEDIUM,
    OID_GEN_MAXIMUM_LOOKAHEAD,
    OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_LINK_SPEED,
    OID_GEN_TRANSMIT_BUFFER_SPACE,
    OID_GEN_RECEIVE_BUFFER_SPACE,
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,
    OID_GEN_VENDOR_ID,
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_VENDOR_DRIVER_VERSION,
    OID_GEN_CURRENT_PACKET_FILTER,
    OID_GEN_CURRENT_LOOKAHEAD,
    OID_GEN_DRIVER_VERSION,
    OID_GEN_MAXIMUM_TOTAL_SIZE,
    OID_GEN_PROTOCOL_OPTIONS,
    OID_GEN_MAC_OPTIONS,
    OID_GEN_MEDIA_CONNECT_STATUS,
    OID_GEN_XMIT_OK,
    OID_GEN_RCV_OK,
    OID_GEN_XMIT_ERROR,
    OID_GEN_RCV_ERROR,
    OID_GEN_RCV_NO_BUFFER,
    OID_GEN_RCV_CRC_ERROR,
    OID_GEN_TRANSMIT_QUEUE_LENGTH,
    OID_802_3_PERMANENT_ADDRESS,
    OID_DOT11_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_DOT11_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_DOT11_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
    OID_PNP_SET_POWER,
    OID_PNP_QUERY_POWER,
    OID_GEN_INTERRUPT_MODERATION,
    OID_GEN_LINK_PARAMETERS,

    // For Native Wi-Fi drivers these OIDs are handled by the Native Wi-Fi filter
    /*
    OID_GEN_STATISTICS,
    */
    
    // Miniport does not support Wakeup today
    /*
    OID_PNP_CAPABILITIES,
    OID_PNP_ADD_WAKE_UP_PATTERN,
    OID_PNP_REMOVE_WAKE_UP_PATTERN,
    OID_PNP_ENABLE_WAKE_UP,
    */
   
    //
    // 802.11 specific OIDs
    //
    OID_DOT11_MPDU_MAX_LENGTH,
    OID_DOT11_OPERATION_MODE_CAPABILITY,
    OID_DOT11_CURRENT_OPERATION_MODE,
    OID_DOT11_ATIM_WINDOW,
    OID_DOT11_SCAN_REQUEST,
    OID_DOT11_RESET_REQUEST,
    OID_DOT11_NIC_POWER_STATE,
    OID_DOT11_OPTIONAL_CAPABILITY,
    OID_DOT11_CURRENT_OPTIONAL_CAPABILITY,
    OID_DOT11_STATION_ID,
    OID_DOT11_CF_POLLABLE,
    OID_DOT11_OPERATIONAL_RATE_SET,
    OID_DOT11_BEACON_PERIOD,
    OID_DOT11_MAC_ADDRESS,
    OID_DOT11_RTS_THRESHOLD,
    OID_DOT11_SHORT_RETRY_LIMIT,
    OID_DOT11_LONG_RETRY_LIMIT,
    OID_DOT11_FRAGMENTATION_THRESHOLD,
    OID_DOT11_MAX_TRANSMIT_MSDU_LIFETIME,
    OID_DOT11_MAX_RECEIVE_LIFETIME,
    OID_DOT11_SUPPORTED_PHY_TYPES,
    OID_DOT11_CURRENT_REG_DOMAIN,
    OID_DOT11_TEMP_TYPE,
   // OID_DOT11_CURRENT_TX_ANTENNA,
    OID_DOT11_DIVERSITY_SUPPORT,
   // OID_DOT11_CURRENT_RX_ANTENNA,
    OID_DOT11_SUPPORTED_POWER_LEVELS,
    OID_DOT11_CURRENT_TX_POWER_LEVEL,
    OID_DOT11_CURRENT_CHANNEL,
    OID_DOT11_CCA_MODE_SUPPORTED,
    OID_DOT11_CURRENT_CCA_MODE,
    OID_DOT11_ED_THRESHOLD,
    OID_DOT11_REG_DOMAINS_SUPPORT_VALUE,
    OID_DOT11_SUPPORTED_TX_ANTENNA,
    OID_DOT11_SUPPORTED_RX_ANTENNA,
    OID_DOT11_DIVERSITY_SELECTION_RX,
    OID_DOT11_SUPPORTED_DATA_RATES_VALUE,
    OID_DOT11_RF_USAGE,
    OID_DOT11_RECV_SENSITIVITY_LIST,
    OID_DOT11_MULTI_DOMAIN_CAPABILITY_IMPLEMENTED,
    OID_DOT11_MULTI_DOMAIN_CAPABILITY_ENABLED,
    OID_DOT11_COUNTRY_STRING,
    OID_DOT11_CURRENT_FREQUENCY,
    OID_DOT11_FREQUENCY_BANDS_SUPPORTED,
    OID_DOT11_SHORT_PREAMBLE_OPTION_IMPLEMENTED,
    OID_DOT11_PBCC_OPTION_IMPLEMENTED,
    OID_DOT11_ERP_PBCC_OPTION_IMPLEMENTED,
    OID_DOT11_ERP_PBCC_OPTION_ENABLED,
    OID_DOT11_DSSS_OFDM_OPTION_IMPLEMENTED,
    OID_DOT11_DSSS_OFDM_OPTION_ENABLED,
    OID_DOT11_SHORT_SLOT_TIME_OPTION_IMPLEMENTED,
    OID_DOT11_SHORT_SLOT_TIME_OPTION_ENABLED,

    OID_DOT11_ENUM_BSS_LIST,
    OID_DOT11_FLUSH_BSS_LIST,
    OID_DOT11_POWER_MGMT_REQUEST,
    OID_DOT11_DESIRED_SSID_LIST,
    OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST,
    OID_DOT11_EXCLUDE_UNENCRYPTED,
    OID_DOT11_DESIRED_BSSID_LIST,
    OID_DOT11_DESIRED_BSS_TYPE,
    OID_DOT11_CONNECT_REQUEST,
    OID_DOT11_STATISTICS,
    OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM,
    OID_DOT11_SUPPORTED_UNICAST_ALGORITHM_PAIR,
    OID_DOT11_SUPPORTED_MULTICAST_ALGORITHM_PAIR,
    OID_DOT11_ENABLED_UNICAST_CIPHER_ALGORITHM,
    OID_DOT11_ENABLED_MULTICAST_CIPHER_ALGORITHM,   
    OID_DOT11_CIPHER_DEFAULT_KEY_ID,
    OID_DOT11_CIPHER_DEFAULT_KEY,
    OID_DOT11_CIPHER_KEY_MAPPING_KEY,
    OID_DOT11_ENUM_ASSOCIATION_INFO,
    OID_DOT11_DISCONNECT_REQUEST,
    OID_DOT11_HARDWARE_PHY_STATE,
    OID_DOT11_DESIRED_PHY_LIST,
    OID_DOT11_CURRENT_PHY_ID,
    OID_DOT11_MEDIA_STREAMING_ENABLED,
    OID_DOT11_UNREACHABLE_DETECTION_THRESHOLD,
    OID_DOT11_ACTIVE_PHY_LIST,
    OID_DOT11_EXTSTA_CAPABILITY,
    OID_DOT11_DATA_RATE_MAPPING_TABLE,
    OID_DOT11_PRIVACY_EXEMPTION_LIST,
    OID_DOT11_IBSS_PARAMS,
    OID_DOT11_PMKID_LIST,
    OID_DOT11_AUTO_CONFIG_ENABLED,
    OID_DOT11_UNICAST_USE_GROUP_ENABLED,
    OID_DOT11_SAFE_MODE_ENABLED,
    OID_DOT11_HIDDEN_NETWORK_ENABLED
    

};

NDIS_STATUS
MpQuerySupportedPHYTypes(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_SUPPORTED_PHY_TYPES pDot11SupportedPhyTypes = InformationBuffer;
    ULONG uNumMaxEntries = 0;
	USHORT    Index;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_SUPPORTED_PHY_TYPES, dot11PHYType))
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = FIELD_OFFSET(DOT11_SUPPORTED_PHY_TYPES, dot11PHYType);
            break;
        }

        InformationBufferLength -= FIELD_OFFSET(DOT11_SUPPORTED_PHY_TYPES, dot11PHYType);
        uNumMaxEntries = InformationBufferLength / sizeof(DOT11_PHY_TYPE);

		

		if (uNumMaxEntries < mib_info->pSupportedPhyTypes->uNumOfEntries) {
			pDot11SupportedPhyTypes->uTotalNumOfEntries = mib_info->pSupportedPhyTypes->uNumOfEntries;
			pDot11SupportedPhyTypes->uNumOfEntries = uNumMaxEntries;

			for (Index = 0; Index < uNumMaxEntries; Index++)
				pDot11SupportedPhyTypes->dot11PHYType[Index] = mib_info->pSupportedPhyTypes->dot11PHYType[Index];

			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
		} else {
			pDot11SupportedPhyTypes->uTotalNumOfEntries = mib_info->pSupportedPhyTypes->uNumOfEntries;
			pDot11SupportedPhyTypes->uNumOfEntries = mib_info->pSupportedPhyTypes->uNumOfEntries;

			for (Index = 0; Index < mib_info->pSupportedPhyTypes->uNumOfEntries; Index++)
				pDot11SupportedPhyTypes->dot11PHYType[Index] = mib_info->pSupportedPhyTypes->dot11PHYType[Index];

			ndisStatus = NDIS_STATUS_SUCCESS;
		}
        //ndisStatus = Hw11QuerySupportedPHYTypes(pAdapter, uNumMaxEntries, pDot11SupportedPhyTypes);

        *BytesWritten = FIELD_OFFSET(DOT11_SUPPORTED_PHY_TYPES, dot11PHYType) +
                        pDot11SupportedPhyTypes->uNumOfEntries * sizeof(DOT11_PHY_TYPE);
        
        *BytesNeeded = FIELD_OFFSET(DOT11_SUPPORTED_PHY_TYPES, dot11PHYType) +
                        pDot11SupportedPhyTypes->uTotalNumOfEntries * sizeof(DOT11_PHY_TYPE);
    } while(FALSE);
    
    return ndisStatus;
}

VOID
MpDot11ResetComplete(
    __in  PADAPTER                pAdapter,
    __in  PDOT11_RESET_REQUEST    pDot11ResetRequest
    )
{
    MpEntry;
    
    //
    // Reset completed successfully.
    // The adapter goes back to Init State
    //
    pAdapter->State = INIT_STATE;
    pAdapter->Dot11RunningMode = RUNNING_MODE_UNKNOWN;
    pAdapter->StartRequestCalled = FALSE;

    //
    // Reset MIBs if needed
    // 
    if (pDot11ResetRequest->bSetDefaultMIB)
    {
        if (pDot11ResetRequest->dot11ResetType == dot11_reset_type_mac)
        {
            MpResetMacMIBs(pAdapter);
#if 0         
            Hw11SetMACAddress(
                pAdapter->Nic,
                pAdapter->PermanentAddress
                );
#endif

        }
        else if (pDot11ResetRequest->dot11ResetType == dot11_reset_type_phy)
        {
            MpResetPhyMIBs(pAdapter);
        }
        else
        {
            MPASSERT(pDot11ResetRequest->dot11ResetType == dot11_reset_type_phy_and_mac);
            MpResetMacMIBs(pAdapter);
            MpResetPhyMIBs(pAdapter);
#if 0           
            Hw11SetMACAddress(
                pAdapter->Nic,
                pAdapter->PermanentAddress
                );
#endif
#if 0
			FWSetMACAddress(
				pAdapter->Nic,
				NULL
			);
#endif

        }
    }
    else
    {
        //
        // Reset MAC address to the one OS specifies if MAC has been reset
        //
        if (pDot11ResetRequest->dot11ResetType != dot11_reset_type_phy)
        {
            if (pDot11ResetRequest->dot11MacAddress[0] != 0 ||
                pDot11ResetRequest->dot11MacAddress[1] != 0 ||
                pDot11ResetRequest->dot11MacAddress[2] != 0 ||
                pDot11ResetRequest->dot11MacAddress[3] != 0 ||
                pDot11ResetRequest->dot11MacAddress[4] != 0 ||
                pDot11ResetRequest->dot11MacAddress[5] != 0)
            {
                ETH_COPY_NETWORK_ADDRESS(pAdapter->CurrentAddress, pDot11ResetRequest->dot11MacAddress);
				
#if 0
                Hw11SetMACAddress(
                    pAdapter->Nic,
                    pDot11ResetRequest->dot11MacAddress
                    );
#endif
            }
        }
    }

    //
    // Reset all statistics
    //
#if 0
    Hw11ResetStatistics(
        pAdapter->Nic
        );
#endif
    MpExit;
}


VOID
MpQuerySupportedOidsList(
    __inout PNDIS_OID *SupportedOidList,
    __inout PULONG    SupportedOidListLength
    )
{
    *SupportedOidList = MPSupportedOids;
    *SupportedOidListLength = sizeof(MPSupportedOids);
}



VOID
MpDot11ResetWorkItem(
    __in WDFWORKITEM  WorkItem
    )
{
    NDIS_STATUS                 ResetStatus;
    PADAPTER                    pAdapter;
    ULONG                       CurrentStatus;
    PDOT11_STATUS_INDICATION    pDot11StatusIndication;
    PDOT11_RESET_REQUEST        pDot11ResetRequest;
    PNDIS_OID_REQUEST           PendingRequest;
    PADAPTER_WORKITEM_CONTEXT adapterWorkItemContext;

    adapterWorkItemContext = GetAdapterWorkItemContext(WorkItem);
    pAdapter = adapterWorkItemContext->Adapter;

    pDot11StatusIndication = pAdapter->PendedRequest->DATA.SET_INFORMATION.InformationBuffer;
    pDot11ResetRequest = pAdapter->PendedRequest->DATA.SET_INFORMATION.InformationBuffer;
    
    MPVERIFY(pAdapter->PendedRequest->DATA.SET_INFORMATION.Oid == OID_DOT11_RESET_REQUEST);

    //
    // Synchronize Reset operation with Pause, Restart etc.
    //
    MP_ACQUIRE_RESET_PNP_LOCK(pAdapter);
    
    //
    // Perform the reset as requested. This request will be pended and
    // completed asynchronously if there are sends in progress, or there
    // is already a reset or scan outstanding on the miniport
    //
    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

    //
    // Reset Step 0 - We set flags so that the other routines 
    // stop proceeding forward
    //    

    CurrentStatus = MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_DOT11_RESET_IN_PROGRESS);
    
    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);

    //
    // Reset Step 1 - Wait for things to cleanup
    //
    StaResetCipherAbilities(pAdapter);



    //
    // If the scan operation is in progress, cancel it.
    //
    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HW_IS_SCANNING))
    {
        //Hw11CancelScan(pAdapter->Nic);
    }
    MPVERIFY(!MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HW_IS_SCANNING));

    //
    // Stop the recv. notification
    //
    wf_usb_dev_stop(pAdapter);

    //
    // Dont redo the work that is already done by
    // the Reset/Pause routines (protected by PNP lock)
    //
    if (MP_INCREMENT_RESET_PAUSE_COUNT(pAdapter) == 1)
    {
        //
        // Since we are resetting the miniport, remove the Active Senders count
        //
        MP_DECREMENT_ACTIVE_SENDERS(pAdapter);
        NdisWaitEvent(&pAdapter->ActiveSendersDoneEvent, MP_NEVER_TIMEOUT);
        //
        // Remove the Active Receivers count added during Initialize
        // We can Reset when the count reaches 1 or less.
        //
        MP_DECREMENT_ACTIVE_RECEIVERS(pAdapter);
        while(pAdapter->NumActiveReceivers >= 1)
        {
            NdisMSleep(10 * 1000);  // 10 msec
        }
    }

    //
    // Reset station (PNP lock held, but send lock not acquired)
    //
    //Sta11ResetStep1(pAdapter->Station, pDot11ResetRequest->dot11ResetType);

    //
    // Call the Custom Interfaces
    //
//    MpEventDot11Reset(pAdapter, pDot11ResetRequest->dot11ResetType);
    
    //
    // Reset the Miniport/hardware/station
    //
    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
    ResetStatus = MpResetInternalRoutine(pAdapter, pDot11ResetRequest->dot11ResetType);
    
    //
    // The request is completed synchronously.
    //
    MpDot11ResetComplete(pAdapter, pDot11ResetRequest);

    //
    // Undo the pause/reset work from before
    //
    if (MP_DECREMENT_RESET_PAUSE_COUNT(pAdapter) == 0)
    {
        //
        // Reset the ActiveSenders Event and put the reference back on
        //
        MP_INCREMENT_ACTIVE_SENDERS(pAdapter);
        NdisResetEvent(&pAdapter->ActiveSendersDoneEvent);
        MP_INCREMENT_ACTIVE_RECEIVERS(pAdapter);        
    }    

    if (ResetStatus == NDIS_STATUS_SUCCESS || ResetStatus == NDIS_STATUS_SOFT_ERRORS)
    {
        MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_DOT11_RESET_IN_PROGRESS | MP_ADAPTER_HARDWARE_ERROR | MP_ADAPTER_NON_RECOVER_ERROR);
    }
    else
    {
        MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_DOT11_RESET_IN_PROGRESS);
    }

    //
    // Pass the reset complete to the station
    //
    //Sta11ResetStep4(pAdapter->Station, pDot11ResetRequest->dot11ResetType);

    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);

    //
    // Complete the reset request with appropriate status to NDIS
    //
    pDot11StatusIndication->uStatusType = DOT11_STATUS_RESET_CONFIRM;
    pDot11StatusIndication->ndisStatus = ResetStatus;
    
    MP_RELEASE_RESET_PNP_LOCK(pAdapter);
    
    LOG_D("Reset completed asynchronously with status %x", ResetStatus);

    PendingRequest = pAdapter->PendedRequest;
    pAdapter->PendedRequest = NULL;
    
    NdisMOidRequestComplete(
        pAdapter->MiniportAdapterHandle,
        PendingRequest,
        NDIS_STATUS_SUCCESS
        );

    // notify hw about disconnected status
    //Hw11NotifyEventConnectionState(pAdapter->Nic, FALSE);
}


NDIS_STATUS
MpResetRequest(
    __in  PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in  ULONG InputBufferLength,
    __in  ULONG OutputBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    )
{
    PDOT11_RESET_REQUEST pDot11ResetRequest = InformationBuffer;

    LOG_D("****  DOT11 RESET __in PROGRESS ****");

    //
    // First make sure the input buffer is large enough to
    // hold a RESET_CONFIRM
    //
    if (OutputBufferLength < sizeof(DOT11_STATUS_INDICATION)) {
        *BytesNeeded = sizeof(DOT11_STATUS_INDICATION);
        return NDIS_STATUS_INVALID_LENGTH;
    }
    
    //
    // Validate the buffer length
    //
    if (InputBufferLength < sizeof(DOT11_RESET_REQUEST)) {
        *BytesNeeded = sizeof(DOT11_RESET_REQUEST);
        return NDIS_STATUS_INVALID_LENGTH;
    }
    
    //
    // Validate the buffer
    //
    switch (pDot11ResetRequest->dot11ResetType) {
        case dot11_reset_type_phy:
        case dot11_reset_type_mac:
        case dot11_reset_type_phy_and_mac:
            break;

        default:
            return NDIS_STATUS_INVALID_DATA;
    }
    
    *BytesRead = sizeof(DOT11_RESET_REQUEST);
    *BytesWritten = sizeof(DOT11_STATUS_INDICATION);

    //
    // Reset implies disconnect if the current adapter state is OP_STATE
    //

    if (MP_VERIFY_STATE(pAdapter, OP_STATE)) {
        
        //
        // If we are running in Ad Hoc mode, disconnect from the ad hoc network. 
        //

        if (pAdapter->Dot11RunningMode == RUNNING_MODE_STA_ADHOC) {
            //Sta11DisconnectAdHoc(pAdapter->Station);
        }
    }
	
	// TODO: Finish reset request.   2021/04/23
    LOG_D("Dot11 Reset in progress");

	wf_mlme_deauth (pAdapter->nic_info, wf_true);

	StaResetCipherAbilities(pAdapter);

	wf_mib_info_t *mib_info = pAdapter->mib_info;
	mib_info->OperatingPhyId = WF_PHY_ID_DEFAULT;	
	mib_info->OperatingPhyMIB = &mib_info->PhyMIB[mib_info->OperatingPhyId];

	
    /*NdisQueueIoWorkItem(
        pAdapter->Dot11ResetWorkItem,
        MpDot11ResetWorkItem,
        pAdapter        
        );
        */
#if 0
	WdfWorkItemEnqueue(pAdapter->Dot11ResetWorkItem);
#endif
	pAdapter->State = INIT_STATE;
    pAdapter->Dot11RunningMode = RUNNING_MODE_UNKNOWN;
    pAdapter->StartRequestCalled = FALSE;

	return NDIS_STATUS_SUCCESS;
    //return NDIS_STATUS_PENDING;
}



NDIS_STATUS
MpSetMulticastList(
    __in PADAPTER pAdapter,
    __inout PVOID pvBuffer,
    __in ULONG uBufferLength,
    __inout PULONG puBytesRead,
    __inout PULONG puBytesNeeded
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(pvBuffer);
    //
    // Initialize.
    //
    *puBytesNeeded = sizeof(DOT11_MAC_ADDRESS);
    *puBytesRead = 0;

    if (uBufferLength % sizeof(DOT11_MAC_ADDRESS)) {
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        BAIL_ON_NDIS_ERROR(ndisStatus);
    }

    //
    // Verify that we can hold the multicast list
    //
    if (uBufferLength > (HW_MAX_MCAST_LIST_SIZE * sizeof(DOT11_MAC_ADDRESS))) {
        ndisStatus = NDIS_STATUS_MULTICAST_FULL;
        *puBytesNeeded = HW_MAX_MCAST_LIST_SIZE * sizeof(DOT11_MAC_ADDRESS);
        BAIL_ON_NDIS_ERROR(ndisStatus);
    }

    *puBytesRead = uBufferLength;

    //
    // Transfer the new multicast list to the card.
    // We need to serialize the call.
    //
    #if 0
    ndisStatus = Hw11SetMCastList(
                pAdapter,
                pvBuffer,
                uBufferLength
                );
	#endif
error:
    return (ndisStatus);
}

NDIS_STATUS
MpQueryMPDUMaxLength(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        *BytesWritten = sizeof(ULONG);
        NdisMoveMemory(InformationBuffer, &pAdapter->MPDUMaxLength, sizeof(ULONG));
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryOperationModeCapability(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_OPERATION_MODE_CAPABILITY pDot11OpModeCap = InformationBuffer;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {

        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_OPERATION_MODE_CAPABILITY))
        {
            *BytesNeeded = sizeof(DOT11_OPERATION_MODE_CAPABILITY);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        //Hw11QueryOpModeCap(pAdapter, pDot11OpModeCap);

		
		NdisMoveMemory(pDot11OpModeCap,
			&mib_info->OperationModeCapability,
			sizeof(DOT11_OPERATION_MODE_CAPABILITY));

        //
        // Please set the major and minor version appropriately
        // as defined by in HW_Interface.h.
        //
        MPVERIFY(pDot11OpModeCap->uMajorVersion == MP_OPERATION_MODE_CAPABILITY_MAJOR_VERSION);
        MPVERIFY(pDot11OpModeCap->uMinorVersion == MP_OPERATION_MODE_CAPABILITY_MINOR_VERSION);

        *BytesWritten = sizeof(DOT11_OPERATION_MODE_CAPABILITY);
    } while (FALSE);

    return ndisStatus;

}

NDIS_STATUS
MpQueryCurrentOperationMode(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_CURRENT_OPERATION_MODE pCurrentOperationMode = InformationBuffer;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_CURRENT_OPERATION_MODE))
        {
            *BytesNeeded = sizeof(DOT11_CURRENT_OPERATION_MODE);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

		pCurrentOperationMode->uCurrentOpMode = mib_info->CurrentOperationMode.uCurrentOpMode;

        //Hw11QueryCurrentOperationMode(pAdapter, pCurrentOperationMode);

        *BytesWritten = sizeof(DOT11_CURRENT_OPERATION_MODE);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryATIMWindow(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG uValue = 0;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
                
        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        uValue = mib_info->AtimWindow;//Hw11QueryATIMWindow(pAdapter);
        NdisMoveMemory(
            InformationBuffer,
            &uValue,
            sizeof(ULONG)
            );
        *BytesWritten = sizeof(ULONG);
    }while(FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
MpQueryNicPowerState(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN     value;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(BOOLEAN))
        {
            *BytesNeeded = sizeof(BOOLEAN);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        value = TRUE;//Hw11QueryNicPowerState(pAdapter, TRUE);

        NdisMoveMemory(InformationBuffer, &value, sizeof(BOOLEAN));
        *BytesWritten = sizeof(BOOLEAN);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryOptionalCapability(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_OPTIONAL_CAPABILITY pDot11OptionalCapability = InformationBuffer;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        if (InformationBufferLength < sizeof(DOT11_OPTIONAL_CAPABILITY))
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = sizeof(DOT11_OPTIONAL_CAPABILITY);
            break;
        }

        MPVERIFY (pDot11OptionalCapability->uReserved == 0);

		pDot11OptionalCapability->bDot11PCF = TRUE;
		pDot11OptionalCapability->bDot11PCFMPDUTransferToPC = TRUE;
		pDot11OptionalCapability->bStrictlyOrderedServiceClass = TRUE;
        //ndisStatus = Hw11QueryOptionalCapability(pAdapter, pDot11OptionalCapability);
        *BytesWritten = sizeof(DOT11_OPTIONAL_CAPABILITY);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryCurrentOptionalCapability(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_CURRENT_OPTIONAL_CAPABILITY pDot11CurrentOptionalCapability = InformationBuffer;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        
        if (InformationBufferLength < sizeof(DOT11_CURRENT_OPTIONAL_CAPABILITY))
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = sizeof(DOT11_CURRENT_OPTIONAL_CAPABILITY);
            break;
        }
        
        MPVERIFY (pDot11CurrentOptionalCapability->uReserved == 0);

		pDot11CurrentOptionalCapability->bDot11CFPollable = TRUE;
		pDot11CurrentOptionalCapability->bDot11PCF = TRUE;
		pDot11CurrentOptionalCapability->bDot11PCFMPDUTransferToPC = TRUE;
		pDot11CurrentOptionalCapability->bStrictlyOrderedServiceClass = TRUE;
        //ndisStatus = Hw11QueryCurrentOptionalCapability(pAdapter, pDot11CurrentOptionalCapability);
        
        *BytesWritten = sizeof(DOT11_CURRENT_OPTIONAL_CAPABILITY);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryCFPollable(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        *((PBOOLEAN)InformationBuffer) = FALSE;//Hw11QueryCFPollable(pAdapter);

        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpPnPQueryPower(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_DEVICE_POWER_STATE     DevicePowerState;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        if (InformationBufferLength < sizeof(NDIS_DEVICE_POWER_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        
        NdisMoveMemory(&DevicePowerState, InformationBuffer, sizeof(NDIS_DEVICE_POWER_STATE));
        
        if (DevicePowerState <= NdisDeviceStateUnspecified || DevicePowerState >= NdisDeviceStateMaximum)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        //ndisStatus = Hw11CanTransitionPower(pAdapter->Nic, DevicePowerState);
    } while(FALSE);

    return ndisStatus;
}



NDIS_STATUS
MpQueryPacketFilter(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        
        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        
        *BytesWritten = sizeof(ULONG);

        NdisStoreUlong(InformationBuffer, pAdapter->PacketFilter);
    } while(FALSE);

    return(ndisStatus);
}

NDIS_STATUS
MpQueryGenStatistics(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNDIS_STATISTICS_INFO   pGenStats;

    UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(NDIS_STATISTICS_INFO))
        {
            *BytesNeeded = sizeof(NDIS_STATISTICS_INFO);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pGenStats = (PNDIS_STATISTICS_INFO)InformationBuffer;

        NdisZeroMemory(pGenStats, sizeof(NDIS_STATISTICS_INFO));
        pGenStats->Header.Revision = NDIS_OBJECT_REVISION_1;
        pGenStats->Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
        pGenStats->Header.Size = sizeof(NDIS_STATISTICS_INFO);

        // TODO: Add support to collect statistics

        *BytesWritten = sizeof(NDIS_STATISTICS_INFO);
    } while(FALSE);

    return ndisStatus;
}


NDIS_STATUS
MpQueryOperationalRateSet(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_RATE_SET pDot11RateSet = NULL;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	PNICPHYMIB  PhyMib = mib_info->SelectedPhyMIB;
	ULONG       Index;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        
        if (InformationBufferLength < sizeof(DOT11_RATE_SET))
        {
            *BytesNeeded = sizeof(DOT11_RATE_SET);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        
        *BytesWritten = sizeof(DOT11_RATE_SET);
        pDot11RateSet = InformationBuffer;

		for (Index = 0; Index < PhyMib->OperationalRateSet.uRateSetLength; Index++) {
			pDot11RateSet->ucRateSet[Index] = PhyMib->OperationalRateSet.ucRateSet[Index];// & 0x7F;
		}

		pDot11RateSet->uRateSetLength = PhyMib->OperationalRateSet.uRateSetLength;  
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryBeaconPeriod(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

		*puValue = pAdapter->DupCurrentInfo.BeaconInterval;
        
        //*puValue = Hw11QueryBeaconPeriod(pAdapter);
        
        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryRTSThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        *puValue = pAdapter->DupCurrentInfo.RTS_Threshold;//Hw11QueryRTSThreshold(pAdapter);

        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryShortRetryLimit(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        
        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        
        *puValue = pAdapter->DupCurrentInfo.ShortRetryLimit;//Hw11QueryShortRetryLimit(pAdapter);
        
        *BytesWritten = sizeof(ULONG);
    } while(FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
MpQueryLongRetryLimit(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        *puValue = pAdapter->DupCurrentInfo.LongRetryLimit;//Hw11QueryLongRetryLimit(pAdapter);

        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryFragmentationThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG uFragmentationThreshold = 0;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        uFragmentationThreshold = pAdapter->DupCurrentInfo.Frag_Threshold;//Hw11GetFragmentationThreshold(pAdapter);
        NdisMoveMemory(InformationBuffer, &uFragmentationThreshold, sizeof(ULONG));
        
        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}


NDIS_STATUS
MpQueryMaxReceiveLifeTime(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        *puValue = pAdapter->DupCurrentInfo.MaxRxLifeTime;//Hw11QueryMaxReceiveLifeTime(pAdapter);

        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}


NDIS_STATUS
MpQueryMaxTXMSDULifeTime(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        *puValue = pAdapter->DupCurrentInfo.MaxTxLifeTime;//Hw11QueryMaxTXMSDULifeTime(pAdapter);

        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}


NDIS_STATUS
MpQueryCurrentRegDomain(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        /*
		DOT11_REG_DOMAIN_OTHER,
	    DOT11_REG_DOMAIN_FCC,
	    DOT11_REG_DOMAIN_DOC,
	    DOT11_REG_DOMAIN_ETSI,
	    DOT11_REG_DOMAIN_SPAIN,
	    DOT11_REG_DOMAIN_FRANCE,
	    DOT11_REG_DOMAIN_MKK,
	    HW_REG_DOMAIN_MKK1,
	    HW_REG_DOMAIN_ISRAEL
		*/
		//may we should return country code
        *puValue = 0;//Hw11QueryCurrentRegDomain(pAdapter);
        
        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryTempType(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_TEMP_TYPE pDot11TempType = InformationBuffer;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_TEMP_TYPE))
        {
            *BytesNeeded = sizeof(DOT11_TEMP_TYPE);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        *pDot11TempType = dot11_temp_type_1;

        *BytesWritten = sizeof(DOT11_TEMP_TYPE);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryCurrentTXAntenna(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    //PULONG puValue = InformationBuffer;
	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(InformationBuffer);
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        //*puValue = Hw11QueryCurrentTXAntenna(pAdapter->Nic);

        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryDiversitySupport(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_DIVERSITY_SUPPORT pDot11DiversitySupport = InformationBuffer;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_DIVERSITY_SUPPORT))
        {
            *BytesNeeded = sizeof(DOT11_DIVERSITY_SUPPORT);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        *pDot11DiversitySupport = mib_info->DiversitySupport;//Hw11QueryDiversitySupport(pAdapter);

        *BytesWritten = sizeof(DOT11_DIVERSITY_SUPPORT);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryCurrentRXAntenna(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    //PULONG puValue = InformationBuffer;
	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(InformationBuffer);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        //*puValue = Hw11QueryCurrentRXAntenna(pAdapter->Nic);

        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQuerySupportedPowerLevels(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_SUPPORTED_POWER_LEVELS pDot11SupportedPowerLevels = InformationBuffer;
	USHORT    Index;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_SUPPORTED_POWER_LEVELS))
        {
            *BytesNeeded = sizeof(DOT11_SUPPORTED_POWER_LEVELS);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

		pDot11SupportedPowerLevels->uNumOfSupportedPowerLevels = mib_info->SupportedPowerLevels.uNumOfSupportedPowerLevels;
		for (Index = 0; Index < mib_info->SupportedPowerLevels.uNumOfSupportedPowerLevels; Index++) {
			pDot11SupportedPowerLevels->uTxPowerLevelValues[Index] = mib_info->SupportedPowerLevels.uTxPowerLevelValues[Index];
		}

        //Hw11QuerySupportedPowerLevels(pAdapter, pDot11SupportedPowerLevels);

        *BytesWritten = FIELD_OFFSET(DOT11_SUPPORTED_POWER_LEVELS, uTxPowerLevelValues) + 
            pDot11SupportedPowerLevels->uNumOfSupportedPowerLevels * sizeof(ULONG);
    } while(FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
MpQueryCurrentTXPowerLevel(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        *puValue = mib_info->CurrentTxPowerLevel;//Hw11QueryCurrentTXPowerLevel(pAdapter);

        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryCurrentChannel(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG      puValue = InformationBuffer;
	PNICPHYMIB PhyMib;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        if (InformationBufferLength < sizeof(ULONG))
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = sizeof(ULONG);
            break;
        }
		
		//may we should use operating phy mib
		PhyMib = mib_info->OperatingPhyMIB;

		if (PhyMib->PhyType == dot11_phy_type_dsss ||
			PhyMib->PhyType == dot11_phy_type_hrdsss ||
			PhyMib->PhyType == dot11_phy_type_erp ||
			PhyMib->PhyType == dot11_phy_type_ht) {
			*puValue = (ULONG)PhyMib->Channel;
		} else {
			ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
		}
        
        //ndisStatus = Hw11GetChannel(pAdapter, puValue, TRUE);
        *BytesWritten = sizeof(ULONG);
    } while(FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
MpQueryCCAModeSupported(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG * puValue = InformationBuffer;
	PNICPHYMIB PhyMib;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        if (InformationBufferLength < sizeof(ULONG))
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = sizeof(ULONG);
            break;
        }

		PhyMib = mib_info->OperatingPhyMIB;

		if (PhyMib->PhyType == dot11_phy_type_dsss ||
			PhyMib->PhyType == dot11_phy_type_hrdsss ||
			PhyMib->PhyType == dot11_phy_type_erp ||
			PhyMib->PhyType == dot11_phy_type_ht) {
			*puValue = mib_info->CCAModeSupported;
		} else {
			ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
		}

        //ndisStatus = Hw11QueryCCAModeSupported(pAdapter, puValue, TRUE);
        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

#define PHY_TYPE_IS_SUPPORT(mib) (mib->PhyType == dot11_phy_type_dsss ||\
									mib->PhyType == dot11_phy_type_hrdsss ||\
									mib->PhyType == dot11_phy_type_erp ||\
									mib->PhyType == dot11_phy_type_ht)

NDIS_STATUS
MpQueryCurrentCCA(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG * puValue = InformationBuffer;
	PNICPHYMIB PhyMib;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        if (InformationBufferLength < sizeof(ULONG)) {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = sizeof(ULONG);
            break;
        }

		PhyMib = mib_info->OperatingPhyMIB;

		if (PHY_TYPE_IS_SUPPORT(PhyMib)) {
			*puValue = mib_info->CurrentCCAMode;
		} else {
			ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
		}
        //ndisStatus = (ULONG)Hw11QueryCurrentCCA(pAdapter, puValue, TRUE);
        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryEDThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG * puValue = InformationBuffer;
	PNICPHYMIB PhyMib;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        if (InformationBufferLength < sizeof(ULONG)) {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = sizeof(ULONG);
            break;
        }

		PhyMib = mib_info->OperatingPhyMIB;

		if (PHY_TYPE_IS_SUPPORT(PhyMib)) {
			*puValue = mib_info->EDThreshold;
		} else {
			ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
		}

        //ndisStatus = Hw11QueryEdThreshold(pAdapter, puValue, TRUE);
        *BytesWritten = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryCurrentFrequency(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG      puValue = InformationBuffer;
	PNICPHYMIB PhyMib;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        if (InformationBufferLength < sizeof(ULONG))
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = sizeof(ULONG);
            break;
        }

		PhyMib = mib_info->OperatingPhyMIB;

		if (PhyMib->PhyType == dot11_phy_type_ofdm) {
			*puValue = (ULONG)PhyMib->Channel;
		} else {
			ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
		}
        
        //ndisStatus = Hw11GetOfdmFrequency(pAdapter, puValue, TRUE);
        *BytesWritten = sizeof(ULONG);
    } while(FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
MpQueryFrequencyBandsSupported(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG      puValue = InformationBuffer;
	PNICPHYMIB PhyMib;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        if (InformationBufferLength < sizeof(ULONG)) {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = sizeof(ULONG);
            break;
        }

		PhyMib = mib_info->OperatingPhyMIB;

		if (PhyMib->PhyType == dot11_phy_type_ofdm) {
			*puValue = DOT11_FREQUENCY_BANDS_LOWER | DOT11_FREQUENCY_BANDS_MIDDLE;
		} else {
			ndisStatus = NDIS_STATUS_INVALID_STATE;
		}
        
        //ndisStatus = Hw11FrequencyBandsSupported(pAdapter, puValue, TRUE);
        *BytesWritten = sizeof(ULONG);
    } while(FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
MpQueryRegDomainsSupportValue(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_REG_DOMAINS_SUPPORT_VALUE pDot11RegDomainsSupportValue = InformationBuffer;
    ULONG uNumMaxEntries = 0;
	USHORT    Index;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_REG_DOMAINS_SUPPORT_VALUE, dot11RegDomainValue))
        {
            *BytesNeeded = sizeof(DOT11_REG_DOMAINS_SUPPORT_VALUE);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        InformationBufferLength -= FIELD_OFFSET(DOT11_REG_DOMAINS_SUPPORT_VALUE, dot11RegDomainValue);
        uNumMaxEntries = InformationBufferLength / sizeof(DOT11_REG_DOMAIN_VALUE);

		if (uNumMaxEntries < mib_info->pRegDomainsSupportValue->uNumOfEntries) {
			pDot11RegDomainsSupportValue->uNumOfEntries = uNumMaxEntries;
			pDot11RegDomainsSupportValue->uTotalNumOfEntries = mib_info->pRegDomainsSupportValue->uNumOfEntries;
			for (Index = 0; Index<uNumMaxEntries; Index++) {
				pDot11RegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex = mib_info->pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex;
				pDot11RegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportValue = mib_info->pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportValue;
			}

			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
		} else {
			pDot11RegDomainsSupportValue->uNumOfEntries = mib_info->pRegDomainsSupportValue->uNumOfEntries;
			pDot11RegDomainsSupportValue->uTotalNumOfEntries = mib_info->pRegDomainsSupportValue->uNumOfEntries;

			for (Index = 0; Index < mib_info->pRegDomainsSupportValue->uNumOfEntries; Index++)
			{
				pDot11RegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex = mib_info->pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex;
				pDot11RegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportValue = mib_info->pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportValue;
			}

			ndisStatus = NDIS_STATUS_SUCCESS;
		}

        //ndisStatus = Hw11QueryRegDomainsSupportValue(pAdapter, uNumMaxEntries, pDot11RegDomainsSupportValue);

        *BytesWritten = pDot11RegDomainsSupportValue->uNumOfEntries * sizeof(DOT11_REG_DOMAIN_VALUE) +
                        FIELD_OFFSET(DOT11_REG_DOMAINS_SUPPORT_VALUE, dot11RegDomainValue);
        *BytesNeeded = pDot11RegDomainsSupportValue->uTotalNumOfEntries * sizeof(DOT11_REG_DOMAIN_VALUE) +
                        FIELD_OFFSET(DOT11_REG_DOMAINS_SUPPORT_VALUE, dot11RegDomainValue);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQuerySupportedTXAntenna(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_SUPPORTED_ANTENNA_LIST pDot11SupportedAntennaList = InformationBuffer;
    ULONG uNumMaxEntries = 0;
	ULONG    i;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_SUPPORTED_ANTENNA_LIST, dot11SupportedAntenna))
        {
            *BytesNeeded = sizeof(DOT11_SUPPORTED_ANTENNA_LIST);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        InformationBufferLength -= FIELD_OFFSET(DOT11_SUPPORTED_ANTENNA_LIST, dot11SupportedAntenna);
        uNumMaxEntries = InformationBufferLength / sizeof(DOT11_SUPPORTED_ANTENNA);

		if (uNumMaxEntries < 2) {
			pDot11SupportedAntennaList->uNumOfEntries = 0;
			pDot11SupportedAntennaList->uTotalNumOfEntries = 2;

			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
		} else {
			pDot11SupportedAntennaList->uTotalNumOfEntries = 2;

			for (i = 0; i<2 && i<uNumMaxEntries; i++) {
				pDot11SupportedAntennaList->dot11SupportedAntenna[i].uAntennaListIndex = i + 1;
				pDot11SupportedAntennaList->dot11SupportedAntenna[i].bSupportedAntenna = TRUE;
			}

			pDot11SupportedAntennaList->uNumOfEntries = 2;
		}
        //ndisStatus = Hw11QuerySupportedTXAntenna(pAdapter, uNumMaxEntries, pDot11SupportedAntennaList);

        *BytesWritten = FIELD_OFFSET(DOT11_SUPPORTED_ANTENNA_LIST, dot11SupportedAntenna) +
                        pDot11SupportedAntennaList->uNumOfEntries * sizeof(DOT11_SUPPORTED_ANTENNA);
        *BytesNeeded = FIELD_OFFSET(DOT11_SUPPORTED_ANTENNA_LIST, dot11SupportedAntenna) +
                        pDot11SupportedAntennaList->uTotalNumOfEntries * sizeof(DOT11_SUPPORTED_ANTENNA);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQuerySupportedRXAntenna(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_SUPPORTED_ANTENNA_LIST pDot11SupportedAntennaList = InformationBuffer;
    ULONG uNumMaxEntries = 0;
	ULONG	 i;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_SUPPORTED_ANTENNA_LIST, dot11SupportedAntenna))
        {
            *BytesNeeded = sizeof(DOT11_SUPPORTED_ANTENNA_LIST);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        InformationBufferLength -= FIELD_OFFSET(DOT11_SUPPORTED_ANTENNA_LIST, dot11SupportedAntenna);
        uNumMaxEntries = InformationBufferLength / sizeof(DOT11_SUPPORTED_ANTENNA);

		if (uNumMaxEntries < 2) {
			pDot11SupportedAntennaList->uNumOfEntries = 0;
			pDot11SupportedAntennaList->uTotalNumOfEntries = 2;

			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
		} else {
			pDot11SupportedAntennaList->uTotalNumOfEntries = 2;

			for (i = 0; i<2 && i<uNumMaxEntries; i++) {
				pDot11SupportedAntennaList->dot11SupportedAntenna[i].uAntennaListIndex = i + 1;
				pDot11SupportedAntennaList->dot11SupportedAntenna[i].bSupportedAntenna = TRUE;
			}

			pDot11SupportedAntennaList->uNumOfEntries = 2;
		}
        //ndisStatus = Hw11QuerySupportedRXAntenna(pAdapter, uNumMaxEntries, pDot11SupportedAntennaList);

        *BytesWritten = FIELD_OFFSET(DOT11_SUPPORTED_ANTENNA_LIST, dot11SupportedAntenna) +
                        pDot11SupportedAntennaList->uNumOfEntries * sizeof(DOT11_SUPPORTED_ANTENNA);
        *BytesNeeded = FIELD_OFFSET(DOT11_SUPPORTED_ANTENNA_LIST, dot11SupportedAntenna) +
                        pDot11SupportedAntennaList->uTotalNumOfEntries * sizeof(DOT11_SUPPORTED_ANTENNA);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryDiversitySelectionRX(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_DIVERSITY_SELECTION_RX_LIST pDot11DiversitySelectionRXList = InformationBuffer;
    ULONG uNumMaxEntries = 0;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_DIVERSITY_SELECTION_RX_LIST, dot11DiversitySelectionRx))
        {
            *BytesNeeded = sizeof(DOT11_SUPPORTED_ANTENNA_LIST);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        InformationBufferLength -= FIELD_OFFSET(DOT11_DIVERSITY_SELECTION_RX_LIST, dot11DiversitySelectionRx);
        uNumMaxEntries = InformationBufferLength / sizeof(DOT11_DIVERSITY_SELECTION_RX);

        //ndisStatus = Hw11QueryDiversitySelectionRX(pAdapter->Nic, uNumMaxEntries, pDot11DiversitySelectionRXList);

        *BytesWritten = FIELD_OFFSET(DOT11_DIVERSITY_SELECTION_RX_LIST, dot11DiversitySelectionRx) +
                        pDot11DiversitySelectionRXList->uNumOfEntries * sizeof(DOT11_DIVERSITY_SELECTION_RX);
        *BytesNeeded = FIELD_OFFSET(DOT11_DIVERSITY_SELECTION_RX_LIST, dot11DiversitySelectionRx) +
                        pDot11DiversitySelectionRXList->uTotalNumOfEntries * sizeof(DOT11_DIVERSITY_SELECTION_RX);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQuerySupportedDataRatesValue(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	PDOT11_SUPPORTED_DATA_RATES_VALUE_V2    pDot11SupportedDataRatesValue = InformationBuffer;
	PNICPHYMIB PhyMib;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    ULONG   Index;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2))
        {
            *BytesNeeded = sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

	    PhyMib = mib_info->OperatingPhyMIB;// : pNic->ManagementInfo.OperatingPhyMIB;

	    for (Index = 0; Index < MAX_NUM_SUPPORTED_RATES_V2; Index++) {
	        pDot11SupportedDataRatesValue->ucSupportedTxDataRatesValue[Index] = PhyMib->SupportedDataRatesValue.ucSupportedTxDataRatesValue[Index];
	        pDot11SupportedDataRatesValue->ucSupportedRxDataRatesValue[Index] = PhyMib->SupportedDataRatesValue.ucSupportedRxDataRatesValue[Index];
	    }
	
//        Hw11QuerySupportedDataRatesValue(
//            pAdapter->Nic,
//            (PDOT11_SUPPORTED_DATA_RATES_VALUE_V2) InformationBuffer,
//            TRUE
//            );
		
        *BytesWritten = sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2);
    } while(FALSE);

    return ndisStatus;
}


NDIS_STATUS
MpQueryRecvSensitivityList(
    __in PADAPTER     pAdapter,
    __inout PVOID    InformationBuffer,
    __in ULONG        InputBufferLength,
    __in ULONG        OutputBufferLength,
    __inout PULONG   BytesWritten,
    __inout PULONG   BytesNeeded
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_RECV_SENSITIVITY_LIST    pDot11RecvSensitivityList = InformationBuffer;
    ULONG           uMaxEntries;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        if (InputBufferLength < (ULONG)FIELD_OFFSET(DOT11_RECV_SENSITIVITY_LIST, dot11RecvSensitivity))
        {
            *BytesNeeded = sizeof(DOT11_RECV_SENSITIVITY_LIST);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        if (OutputBufferLength < (ULONG)FIELD_OFFSET(DOT11_RECV_SENSITIVITY_LIST, dot11RecvSensitivity))
        {
            *BytesNeeded = sizeof(DOT11_RECV_SENSITIVITY_LIST);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        uMaxEntries = ((OutputBufferLength - (FIELD_OFFSET(DOT11_RECV_SENSITIVITY_LIST, dot11RecvSensitivity))) / sizeof(DOT11_RECV_SENSITIVITY));

		#if 0
        ndisStatus = Hw11QueryRecvSensitivityList(
                        pAdapter->Nic,
                        uMaxEntries,
                        pDot11RecvSensitivityList
                        );
		 #endif
		 
        *BytesWritten = FIELD_OFFSET(DOT11_RECV_SENSITIVITY_LIST, dot11RecvSensitivity) + (sizeof(DOT11_RECV_SENSITIVITY) * pDot11RecvSensitivityList->uNumOfEntries);
        *BytesWritten = FIELD_OFFSET(DOT11_RECV_SENSITIVITY_LIST, dot11RecvSensitivity) + (sizeof(DOT11_RECV_SENSITIVITY) * pDot11RecvSensitivityList->uTotalNumOfEntries);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryLinkParameters(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNDIS_LINK_PARAMETERS      pLinkParams;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(NDIS_LINK_PARAMETERS))
        {
            *BytesNeeded = sizeof(NDIS_LINK_PARAMETERS);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pLinkParams = (PNDIS_LINK_PARAMETERS)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pLinkParams->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     NDIS_LINK_PARAMETERS_REVISION_1,
                                     sizeof(NDIS_LINK_PARAMETERS));

		pLinkParams->MediaDuplexState = MediaDuplexStateHalf;
		pLinkParams->XmitLinkSpeed = mib_info->CurrentTXDataRate * 500000;
		pLinkParams->RcvLinkSpeed = mib_info->CurrentTXDataRate * 500000;
		pLinkParams->PauseFunctions = NdisPauseFunctionsUnsupported;
		pLinkParams->AutoNegotiationFlags = NDIS_LINK_STATE_DUPLEX_AUTO_NEGOTIATED;
		#if 0
        ndisStatus = Hw11QueryLinkParameters(
                        pAdapter->Nic, 
                        pLinkParams
                        );
		#endif
		
        *BytesWritten = sizeof(NDIS_LINK_PARAMETERS);
    } while (FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetPacketFilter(
    __in PADAPTER               pAdapter,
    __in ULONG                PacketFilter
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    do
    {
        //
        // Any bits not supported?
        //
        if (PacketFilter & ~NDIS_PACKET_TYPE_ALL_802_11_FILTERS)
        {
        	LOG_D("NDIS_PACKET_TYPE_ALL_802_11_FILTERS");
            ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
            break;
        }
		//need to modify
		#if 0
        ndisStatus = Hw11SetPacketFilter(
                pAdapter->Nic,
                PacketFilter
                );
		#endif
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }
        
        // Save the new packet filter value
        pAdapter->PacketFilter = PacketFilter;
        //InterlockedExchange((PLONG)&pAdapter->PacketFilter, PacketFilter);
    } while(FALSE);

    return(ndisStatus);
}

NDIS_STATUS
MpSetLinkParameters(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNDIS_LINK_PARAMETERS      pLinkParams;
	UCHAR           dataRate;
	PDOT11_RATE_SET STARateSet;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	ULONG           index;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(NDIS_LINK_PARAMETERS))
        {
            *BytesNeeded = sizeof(NDIS_LINK_PARAMETERS);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pLinkParams = (PNDIS_LINK_PARAMETERS)InformationBuffer;

        if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pLinkParams->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          NDIS_LINK_PARAMETERS_REVISION_1,
                                          sizeof(NDIS_LINK_PARAMETERS)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

		if (pLinkParams->RcvLinkSpeed != pLinkParams->XmitLinkSpeed) {
	        // For us the TX and RX rates must be the same
	        return NDIS_STATUS_INVALID_DATA;
	    }

		dataRate = (UCHAR)(pLinkParams->XmitLinkSpeed/500000);

	    STARateSet = &mib_info->OperatingPhyMIB->OperationalRateSet;

	    for (index = 0; index < STARateSet->uRateSetLength; index++) {
	        if (STARateSet->ucRateSet[index] == dataRate) {
	            mib_info->CurrentTXDataRate = STARateSet->ucRateSet[index];
	            
	            wf_submit_link_speed(pAdapter, mib_info->CurrentTXDataRate*500000);
	            return NDIS_STATUS_SUCCESS;
	        }
	    }

		#if 0
        ndisStatus = Hw11SetLinkParameters(
                        pAdapter->Nic, 
                        pLinkParams
                        );
		#endif
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetATIMWindow(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;
    ULONG uValue = 0;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_MODE_AND_STATE(pAdapter, DOT11_OPERATION_MODE_STATION, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        NdisMoveMemory(&uValue, puValue, sizeof(ULONG));

		mib_info->AtimWindow = (USHORT)uValue;
        //ndisStatus = Hw11SetATIMWindow(pAdapter->Nic, uValue);

        *BytesRead = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}


NDIS_STATUS
MpSetCurrentOperationMode(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    DOT11_CURRENT_OPERATION_MODE CurrentOperationMode;
    PDOT11_CURRENT_OPERATION_MODE pCurrentOperationMode = &CurrentOperationMode;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    do
    {
        *BytesRead = 0;

        //
        // This request should only come when we are in INIT state
        // This Oid is preceeded by a reset which puts us in INIT
        //
        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            LOG_E("SetOperationRequest failed! NIC is not in INIT state");
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }
        
        if (InformationBufferLength < sizeof(DOT11_CURRENT_OPERATION_MODE))
        {
            *BytesNeeded = sizeof(DOT11_CURRENT_OPERATION_MODE);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        
        *BytesRead = sizeof(DOT11_CURRENT_OPERATION_MODE);
        
        NdisMoveMemory(&CurrentOperationMode, InformationBuffer, sizeof(DOT11_CURRENT_OPERATION_MODE));
        
        if (pCurrentOperationMode->uCurrentOpMode != DOT11_OPERATION_MODE_EXTENSIBLE_STATION &&
            pCurrentOperationMode->uCurrentOpMode != DOT11_OPERATION_MODE_NETWORK_MONITOR)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

		#if 0
        ndisStatus = Sta11SetOperationMode(
                    pAdapter->Station,
                    pCurrentOperationMode
                    );
        #endif   
		//TBD
		//pCurrentOperationMode->uCurrentOpMode = DOT11_OPERATION_MODE_STATION;
		mib_info->CurrentOperationMode.uCurrentOpMode = pCurrentOperationMode->uCurrentOpMode;
		if (pCurrentOperationMode->uCurrentOpMode == DOT11_OPERATION_MODE_NETWORK_MONITOR) {
            wf_set_encryption(pAdapter, TRUE, DOT11_CIPHER_ALGO_NONE);
        }

        if (ndisStatus == NDIS_STATUS_SUCCESS) {
            pAdapter->OperationMode = pCurrentOperationMode->uCurrentOpMode;
            pAdapter->State = INIT_STATE;
            if (pAdapter->OperationMode == DOT11_OPERATION_MODE_NETWORK_MONITOR) {
                MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_NETWORK_MONITOR_MODE);
                pAdapter->AutoConfigEnabled = 0;
            } else {
                MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_NETWORK_MONITOR_MODE);
            }
        }
    }while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetNicPowerState(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG uValue;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
              
        NdisMoveMemory(&uValue, InformationBuffer, sizeof(ULONG));
        *BytesRead = sizeof(ULONG);
        
        //ndisStatus = Hw11SetNicPowerState(pAdapter->Nic, (BOOLEAN)uValue, TRUE);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryStationId(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_MAC_ADDRESS))
        {
            *BytesNeeded = sizeof(DOT11_MAC_ADDRESS);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

		NdisMoveMemory(InformationBuffer, pAdapter->PermanentAddress, 6);
        //Hw11QueryStationId(pAdapter, InformationBuffer);

        *BytesWritten = sizeof(DOT11_MAC_ADDRESS);
    } while(FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
MpSetOperationalRateSet(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_RATE_SET pDot11RateSet = NULL;
    DWORD dwRequiredSize = 0;
	PNICPHYMIB PhyMib;
	ULONG	Index;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
		

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        dwRequiredSize = FIELD_OFFSET(DOT11_RATE_SET, ucRateSet);
        if (InformationBufferLength < dwRequiredSize)
        {
            *BytesNeeded = dwRequiredSize;
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        pDot11RateSet = InformationBuffer;
        
        if (pDot11RateSet->uRateSetLength > DOT11_RATE_SET_MAX_LENGTH ||
            pDot11RateSet->uRateSetLength == 0)
        {
            *BytesNeeded = dwRequiredSize;
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        dwRequiredSize += pDot11RateSet->uRateSetLength;
        if (InformationBufferLength < dwRequiredSize)
        {
            *BytesNeeded = dwRequiredSize;
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        *BytesRead = dwRequiredSize;
		
		PhyMib = mib_info->OperatingPhyMIB; //: pNic->ManagementInfo.OperatingPhyMIB;
		
		for (Index = 0; Index < pDot11RateSet->uRateSetLength; Index++) {
			PhyMib->OperationalRateSet.ucRateSet[Index] = pDot11RateSet->ucRateSet[Index];
		}
		
		PhyMib->OperationalRateSet.uRateSetLength = pDot11RateSet->uRateSetLength;
        
        //ndisStatus = Hw11SetOperationalRateSet(pAdapter, pDot11RateSet, TRUE);
        
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetBeaconPeriod(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;
    ULONG uValue = 0;

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        uValue = *puValue;

        if (uValue < 1 || uValue > 65535)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

		pAdapter->DupCurrentInfo.BeaconInterval = uValue;
        //ndisStatus = Hw11SetBeaconPeriod(pAdapter->Nic, uValue);

        *BytesRead = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetRTSThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;
    ULONG uValue = 0;

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;
        
        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        // If autoconfig is enabled, we dont allow this to be set
        if (pAdapter->AutoConfigEnabled & DOT11_MAC_AUTO_CONFIG_ENABLED_FLAG)
        {
            ndisStatus = NDIS_STATUS_DOT11_AUTO_CONFIG_ENABLED;
            break;
        }

        uValue = *puValue;
        
        if (uValue > 2347)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            //break;
        } else {
			pAdapter->DupCurrentInfo.RTS_Threshold=uValue;
		}
        
        //ndisStatus = Hw11SetRTSThreshold(pAdapter->Nic, uValue);
        
        *BytesRead = sizeof(ULONG);
    } while(FALSE);
    
    return ndisStatus;
}

NDIS_STATUS
MpSetFragmentationThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;
    ULONG uValue = 0;

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        // If autoconfig is enabled, we dont allow this to be set
        if (pAdapter->AutoConfigEnabled & DOT11_MAC_AUTO_CONFIG_ENABLED_FLAG)
        {
            ndisStatus = NDIS_STATUS_DOT11_AUTO_CONFIG_ENABLED;
            break;
        }

        uValue = *puValue;

        if (uValue < 256 || uValue > 2346)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

		pAdapter->DupCurrentInfo.Frag_Threshold=uValue;
        //ndisStatus = Hw11SetFragmentationThreshold(pAdapter->Nic, uValue);

        *BytesRead = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetCurrentRegDomain(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;
    ULONG uValue = 0;

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        uValue = *puValue;

        //ndisStatus = Hw11SetCurrentRegDomain(pAdapter->Nic, uValue);

        *BytesRead = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetCurrentChannel(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;
    ULONG uValue = 0;
	mlme_state_e state;

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if(MP_NIC_POWER_STATE_IS_OFF(pAdapter))
        {
            ndisStatus = NDIS_STATUS_DOT11_POWER_STATE_INVALID;
            break;
        }

        // A scan operation is in progress. Setting the channel could interfere
        // with the scanning process. We will not honor this request.
        wf_mlme_get_state(pAdapter->nic_info, &state);
		if(state <= MLME_STATE_CONN_SCAN) {
			ndisStatus = NDIS_STATUS_DOT11_MEDIA_IN_USE;
            break;
		}
        
        // If autoconfig is enabled, we dont allow this to be set
        if (pAdapter->AutoConfigEnabled & DOT11_PHY_AUTO_CONFIG_ENABLED_FLAG) {
            ndisStatus = NDIS_STATUS_DOT11_AUTO_CONFIG_ENABLED;
            break;
        }

        if (InformationBufferLength < sizeof(ULONG)) {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        
        uValue = *puValue;
        
        if (uValue < HW_MIN_CHANNEL_NUMBER || uValue > HW_MAX_CHANNEL_NUMBER)
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }
        
        *BytesRead = sizeof(ULONG);
        
        //ndisStatus = Hw11SetChannel(pAdapter->Nic, (UCHAR)uValue, TRUE);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetCurrentFrequency(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PULONG puValue = InformationBuffer;
	mlme_state_e state;

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if(MP_NIC_POWER_STATE_IS_OFF(pAdapter))
        {
            ndisStatus = NDIS_STATUS_DOT11_POWER_STATE_INVALID ;
            break;
        }

        // A scan operation is in progress. Setting the channel could interfere
        // with the scanning process. We will not honor this request.
        wf_mlme_get_state(pAdapter->nic_info, &state);
		if(state <= MLME_STATE_CONN_SCAN) {
			ndisStatus = NDIS_STATUS_DOT11_MEDIA_IN_USE;
            break;
		}

        // If autoconfig is enabled, we dont allow this to be set
        if (pAdapter->AutoConfigEnabled & DOT11_PHY_AUTO_CONFIG_ENABLED_FLAG)
        {
            ndisStatus = NDIS_STATUS_DOT11_AUTO_CONFIG_ENABLED;
            break;
        }

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        
        if (*puValue < HW_MIN_OFDM_FREQ || *puValue > HW_MAX_OFDM_FREQ)
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }
        
        *BytesRead = sizeof(ULONG);
        //ndisStatus = Hw11SetOfdmFrequency(pAdapter->Nic, *puValue, TRUE);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpValidateScanRequest(
    PADAPTER                pAdapter,
    PDOT11_SCAN_REQUEST_V2  pDot11ScanRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PHY_TYPE_INFO    pDot11PhyTypeInfo;
    ULONG                   i, BytesParsed = 0;
    PDOT11_SSID             pDot11SSID;

    UNREFERENCED_PARAMETER(pAdapter);
    
    //
    // Perform some validation on the scan request.
    //
    __try
    {
        if (pDot11ScanRequest->uNumOfdot11SSIDs == 0)
        {
            LOG_E("No SSID found in the scan data");
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            __leave;
        }
        
        for (i=0; i<pDot11ScanRequest->uNumOfdot11SSIDs; i++)
        {
            pDot11SSID = (PDOT11_SSID) (pDot11ScanRequest->ucBuffer + pDot11ScanRequest->udot11SSIDsOffset + BytesParsed);
            if (pDot11SSID->uSSIDLength > DOT11_SSID_MAX_LENGTH)
            {
                LOG_D("The SSID length provided (%d) is greater than max SSID length (%d)", pDot11SSID->uSSIDLength, DOT11_SSID_MAX_LENGTH);
                ndisStatus = NDIS_STATUS_INVALID_LENGTH;
                __leave;
            }
            BytesParsed += sizeof(DOT11_SSID);
        }
        
        if (pDot11ScanRequest->dot11BSSType != dot11_BSS_type_infrastructure &&
            pDot11ScanRequest->dot11BSSType != dot11_BSS_type_independent &&
            pDot11ScanRequest->dot11BSSType != dot11_BSS_type_any)
        {
            LOG_E("BSS Type %d not supported", pDot11ScanRequest->dot11BSSType);
            ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
            __leave;
        }

        switch (pDot11ScanRequest->dot11ScanType)
        {
            case dot11_scan_type_active:
            case dot11_scan_type_active | dot11_scan_type_forced:
            case dot11_scan_type_passive:
            case dot11_scan_type_passive | dot11_scan_type_forced:
            case dot11_scan_type_auto:
            case dot11_scan_type_auto | dot11_scan_type_forced:
                break;

            default:
                LOG_E("Dot11 scan type %d not supported", pDot11ScanRequest->dot11ScanType);
                ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
                __leave;
        }

        BytesParsed = 0;
        
        for(i=0; i<pDot11ScanRequest->uNumOfPhyTypeInfos; i++)
        {
            pDot11PhyTypeInfo = (PDOT11_PHY_TYPE_INFO) 
                (pDot11ScanRequest->ucBuffer + pDot11ScanRequest->uPhyTypeInfosOffset + BytesParsed);

            // ExtSTA mode, the OS does not control PHY specific parameters
            MPASSERT(pDot11PhyTypeInfo->bUseParameters == FALSE);
            BytesParsed += (FIELD_OFFSET(DOT11_PHY_TYPE_INFO, ucChannelListBuffer) + pDot11PhyTypeInfo->uChannelListSize);
        }
    }
    __finally
    {
    }

    return ndisStatus;
}

//extern void wf_set_start_scan(void *nic_info);


NDIS_STATUS
MpScanRequest(
    PADAPTER pAdapter,
    __in PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_SCAN_REQUEST_V2      pDot11ScanRequest = InformationBuffer;
     
    LOG_D("********* SCANNING FOR BSS **********");
    if(pAdapter->bRequestedScan == TRUE) {
		LOG_D("Another scan request is processing.");
		return NDIS_STATUS_DOT11_MEDIA_IN_USE;
	}
	pAdapter->bRequestedScan = TRUE;
	pAdapter->CurrentRequestID_Scan = pAdapter->PendedRequest->RequestId;
    
    do
    {
        *BytesRead = 0;

		#if 0
        if (MP_NIC_POWER_STATE_IS_OFF(pAdapter))
        {
            MpTrace(COMP_SCAN, DBG_SERIOUS, ("Nic is currently turned off\n"));
            ndisStatus = NDIS_STATUS_DOT11_POWER_STATE_INVALID ;
            break;
        }
		#endif

        *BytesNeeded = sizeof(DOT11_SCAN_REQUEST_V2);
        if (InformationBufferLength < sizeof(DOT11_SCAN_REQUEST_V2)) {
            LOG_E("Bad length provided for scan. Length needed: %d, Length provided: %d", sizeof(DOT11_SCAN_REQUEST_V2), InformationBufferLength);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesRead = InformationBufferLength;
            break;
        }
        *BytesRead = sizeof(DOT11_SCAN_REQUEST_V2);
        
        //
        // Validate the input parameter
        //
        ndisStatus = MpValidateScanRequest(pAdapter, pDot11ScanRequest);
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            break;
        }

        //
        // Pass the scan request to the station
        //
        #if 0
        ndisStatus = Sta11StartScan(
            pAdapter->Station,
            pAdapter->PendedRequest->RequestId,
            pDot11ScanRequest,
            InformationBufferLength
            );
		#endif
		ndisStatus = wf_set_start_scan(pAdapter->nic_info);
        if (ndisStatus != NDIS_STATUS_SUCCESS) {
            break;
        }
    } while(FALSE);

    return ndisStatus;
}

VOID
Mp11CompletePendedRequest(
    __in  PADAPTER        pAdapter,
    __in  NDIS_STATUS     ndisStatus
    )
{
    PNDIS_OID_REQUEST           PendingRequest;
    
    MpEntry;
    
    MPVERIFY(pAdapter->PendedRequest != NULL);

    //
    // Complete this OID to the OS
    //
    
    PendingRequest = pAdapter->PendedRequest;
    pAdapter->PendedRequest = NULL;
    NdisMOidRequestComplete(
        pAdapter->MiniportAdapterHandle,
        PendingRequest,
        ndisStatus
        );

    MpExit;
}



NDIS_STATUS
MpEnumerateBSSList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InputBufferLength,
    __in ULONG OutputBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_BYTE_ARRAY   pDot11ByteArray = NULL;
	//BOOLEAN ScanFlag = FALSE;
    UNREFERENCED_PARAMETER(InputBufferLength);
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
		
        //
        // Check enough space for the 3 fields of the DOT11_BYTE_ARRAY
        //
        if (OutputBufferLength < (ULONG)FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer))
        {
            *BytesNeeded = sizeof(DOT11_BYTE_ARRAY);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pDot11ByteArray = (PDOT11_BYTE_ARRAY)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pDot11ByteArray->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_BSS_ENTRY_BYTE_ARRAY_REVISION_1,
                                     sizeof(DOT11_BYTE_ARRAY));

        pDot11ByteArray->uNumOfBytes = 0;
        pDot11ByteArray->uTotalNumOfBytes = 0;

        ndisStatus = wf_get_bss_list(
            pAdapter,
            pDot11ByteArray,
            OutputBufferLength
            );
		 
        *BytesWritten = pDot11ByteArray->uNumOfBytes + 
            FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer);
            
        *BytesNeeded = pDot11ByteArray->uTotalNumOfBytes +
            FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer);
    } while(FALSE);
	if(ndisStatus != NDIS_STATUS_SUCCESS && pDot11ByteArray) {
		*BytesWritten = 0;
		pDot11ByteArray->uNumOfBytes = 0;
		pDot11ByteArray->uTotalNumOfBytes = 0;
	}
    return ndisStatus;
}

NDIS_STATUS
MpQueryAutoConfigOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;
        
        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        
        *BytesWritten = sizeof(ULONG);

        NdisStoreUlong(InformationBuffer, pAdapter->AutoConfigEnabled);
    } while(FALSE);

    return(ndisStatus);
}


NDIS_STATUS
MpQueryDesiredSSIDList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_SSID_LIST    pDot11SSIDList;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_SSID_LIST))
        {
            *BytesNeeded = sizeof(DOT11_SSID_LIST);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pDot11SSIDList = (PDOT11_SSID_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pDot11SSIDList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_SSID_LIST_REVISION_1,
                                     sizeof(DOT11_SSID_LIST));
        pDot11SSIDList->uNumOfEntries = 0;
        pDot11SSIDList->uTotalNumOfEntries = 0;

		#if 0
        // Pass to the station
        ndisStatus = Sta11QueryDesiredSSIDList(
            pAdapter->Station,
            pDot11SSIDList,
            InformationBufferLength
            );
        #endif
		 
        *BytesWritten = pDot11SSIDList->uNumOfEntries * sizeof(DOT11_SSID) + 
            FIELD_OFFSET(DOT11_SSID_LIST, SSIDs);
            
        *BytesNeeded = pDot11SSIDList->uTotalNumOfEntries * sizeof(DOT11_SSID) +
            FIELD_OFFSET(DOT11_SSID_LIST, SSIDs);
        
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryExcludedMACAddressList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in  ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_MAC_ADDRESS_LIST    pDot11MacAddrList;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pDot11MacAddrList = (PDOT11_MAC_ADDRESS_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pDot11MacAddrList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_MAC_ADDRESS_LIST_REVISION_1,
                                     sizeof(DOT11_MAC_ADDRESS_LIST));
        pDot11MacAddrList->uNumOfEntries = 0;
        pDot11MacAddrList->uTotalNumOfEntries = 0;

		#if 0
        // Pass to the station
        ndisStatus = Sta11QueryExcludedMACAddressList(
            pAdapter->Station,
            pDot11MacAddrList,
            InformationBufferLength
            );
        #endif
		
        *BytesWritten = pDot11MacAddrList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS_LIST) + 
            FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs);
            
        *BytesNeeded = pDot11MacAddrList->uTotalNumOfEntries * sizeof(DOT11_MAC_ADDRESS_LIST) +
            FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs);
        
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryExcludeUnencrypted(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(BOOLEAN))
        {
            *BytesNeeded = sizeof(BOOLEAN);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

		#if 0
        // Pass to the station
        ndisStatus = Sta11QueryExcludeUnencrypted(
            pAdapter->Station,
            InformationBuffer
            );
         #endif
		 
        *BytesWritten = sizeof(BOOLEAN);
        *BytesNeeded = sizeof(BOOLEAN);
        
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryPrivacyExemptionList(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PRIVACY_EXEMPTION_LIST   list;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        list = (PDOT11_PRIVACY_EXEMPTION_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(list->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_PRIVACY_EXEMPTION_LIST_REVISION_1,
                                     sizeof(DOT11_PRIVACY_EXEMPTION_LIST));
        list->uNumOfEntries = 0;
        list->uTotalNumOfEntries = 0;

        // Pass to the station
        //ndisStatus = Sta11QueryPrivacyExemptionList(pAdapter, list, InformationBufferLength);
            
        *BytesWritten = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) +
                        list->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION);
            
        *BytesNeeded = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) +
                       list->uTotalNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION);
        
    } while(FALSE);
    return ndisStatus;
}

NDIS_STATUS
MpSetAutoConfigOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pAdapter->AutoConfigEnabled = *((PULONG)InformationBuffer);

        *BytesRead = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryDesiredBSSIDList(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_BSSID_LIST    pDot11BSSIDList;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pDot11BSSIDList = (PDOT11_BSSID_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pDot11BSSIDList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_BSSID_LIST_REVISION_1,
                                     sizeof(DOT11_BSSID_LIST));
        pDot11BSSIDList->uNumOfEntries = 0;
        pDot11BSSIDList->uTotalNumOfEntries = 0;

		#if 0
        // Pass to the station
        ndisStatus = Sta11QueryDesiredBSSIDList(
            pAdapter->Station,
            pDot11BSSIDList,
            InformationBufferLength
            );
         #endif
		 
        *BytesWritten = pDot11BSSIDList->uNumOfEntries * sizeof(DOT11_BSSID_LIST) + 
            FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs);
            
        *BytesNeeded = pDot11BSSIDList->uTotalNumOfEntries * sizeof(DOT11_BSSID_LIST) +
            FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs);
        
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryDesiredBSSType(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_BSS_TYPE     pDot11DesiredBSSType;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_BSS_TYPE))
        {
            *BytesNeeded = sizeof(DOT11_BSS_TYPE);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pDot11DesiredBSSType = (PDOT11_BSS_TYPE)InformationBuffer;
		
		if(pAdapter->Dot11RunningMode == RUNNING_MODE_STA_ADHOC)
		{
			*pDot11DesiredBSSType = dot11_BSS_type_independent;
		}
		else
		{
			*pDot11DesiredBSSType = dot11_BSS_type_infrastructure;
		}


        *BytesWritten = sizeof(DOT11_BSS_TYPE);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryDot11Statistics(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_STATISTICS       pDot11Stats;

    UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_STATISTICS))
        {
            *BytesNeeded = sizeof(DOT11_STATISTICS);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pDot11Stats = (PDOT11_STATISTICS)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pDot11Stats->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_STATISTICS_REVISION_1,
                                     sizeof(DOT11_STATISTICS));

        *BytesWritten = 0;
        *BytesNeeded = 0;

		#if 0
        ndisStatus = Hw11QueryDot11Statistics(
            pAdapter,
            pDot11Stats,
            InformationBufferLength,
            BytesWritten,
            BytesNeeded
            );
		#endif
		
		//TBD
		//BytesWritten= sizeof(DOT11_STATISTICS) +(pAdapter->MibInfo.pSupportedPhyTypes->uNumOfEntries - 1) * sizeof(DOT11_PHY_FRAME_STATISTICS);
		*BytesWritten = sizeof(DOT11_STATISTICS) + 0 * sizeof(DOT11_PHY_FRAME_STATISTICS);
		*BytesNeeded = *BytesWritten;

    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryEnabledAuthenticationAlgorithm(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_AUTH_ALGORITHM_LIST    pAuthAlgoList = NULL;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_AUTH_ALGORITHM_LIST))
        {
            *BytesNeeded = sizeof(DOT11_AUTH_ALGORITHM_LIST);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);
        pAuthAlgoList = (PDOT11_AUTH_ALGORITHM_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pAuthAlgoList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_AUTH_ALGORITHM_LIST_REVISION_1,
                                     sizeof(DOT11_AUTH_ALGORITHM_LIST));
        pAuthAlgoList->uNumOfEntries = 0;
        pAuthAlgoList->uTotalNumOfEntries = 0;

		*BytesNeeded = FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds)
			+ 1 * sizeof(DOT11_AUTH_ALGORITHM);

		if (InformationBufferLength < *BytesNeeded)
		{
			pAuthAlgoList->uNumOfEntries = 0;
			pAuthAlgoList->uTotalNumOfEntries = 1;
			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
			break;
		}

		pAuthAlgoList->uNumOfEntries = 1;
		pAuthAlgoList->uTotalNumOfEntries = 1;
		pAuthAlgoList->AlgorithmIds[0] = mib_info->AuthAlgorithm;
        
#if 0
        ndisStatus = Sta11QueryEnabledAuthenticationAlgorithm(
            pAdapter,
            pAuthAlgoList,
            InformationBufferLength
        );
#endif
         *BytesWritten = pAuthAlgoList->uNumOfEntries * sizeof(DOT11_AUTH_ALGORITHM) +
             FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds);
         *BytesNeeded = pAuthAlgoList->uTotalNumOfEntries * sizeof(DOT11_AUTH_ALGORITHM) +
             FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds);
     } while (FALSE);

     
     return ndisStatus;
}


NDIS_STATUS
MpQuerySupportedUnicastAlgorithmPair(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList = NULL;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pAuthCipherList = (PDOT11_AUTH_CIPHER_PAIR_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pAuthCipherList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_AUTH_CIPHER_PAIR_LIST_REVISION_1,
                                     sizeof(DOT11_AUTH_CIPHER_PAIR_LIST));

        pAuthCipherList->uNumOfEntries = 0;    
        pAuthCipherList->uTotalNumOfEntries = 0;    

		#if 1
        ndisStatus = wf_get_unicast_alg_pair(
                        pAdapter, 
                        pAuthCipherList, 
                        InformationBufferLength
                        );
		#endif
		
        *BytesWritten = pAuthCipherList->uNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR) + 
            FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs);
            
        *BytesNeeded = pAuthCipherList->uTotalNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR) +
            FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQuerySupportedMulticastAlgorithmPair(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList = NULL;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pAuthCipherList = (PDOT11_AUTH_CIPHER_PAIR_LIST)InformationBuffer;
        MP_ASSIGN_NDIS_OBJECT_HEADER(pAuthCipherList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_AUTH_CIPHER_PAIR_LIST_REVISION_1,
                                     sizeof(DOT11_AUTH_CIPHER_PAIR_LIST));
        pAuthCipherList->uNumOfEntries = 0;     
        pAuthCipherList->uTotalNumOfEntries = 0;     

		#if 1
        ndisStatus = wf_get_multicast_alg_pair(
                        pAdapter, 
                        pAuthCipherList, 
                        InformationBufferLength
                        );
		#endif
		
        *BytesWritten = pAuthCipherList->uNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR) + 
            FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs);
            
        *BytesNeeded = pAuthCipherList->uTotalNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR) +
            FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryEnabledUnicastCipherAlgorithm(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_CIPHER_ALGORITHM_LIST    pAuthCipherList = NULL;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_CIPHER_ALGORITHM_LIST))
        {
            *BytesNeeded = sizeof(DOT11_AUTH_CIPHER_PAIR_LIST);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pAuthCipherList = (PDOT11_CIPHER_ALGORITHM_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pAuthCipherList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_CIPHER_ALGORITHM_LIST_REVISION_1,
                                     sizeof(DOT11_CIPHER_ALGORITHM_LIST));
        pAuthCipherList->uNumOfEntries = 0;      
        pAuthCipherList->uTotalNumOfEntries = 0;      
		
		{
			*BytesNeeded = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
				1 * sizeof(DOT11_CIPHER_ALGORITHM);

			if (InformationBufferLength < *BytesNeeded)
			{
				pAuthCipherList->uNumOfEntries = 0;
				pAuthCipherList->uTotalNumOfEntries = 1;
				ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
				*BytesNeeded = 0;
				break;
			}
			pAuthCipherList->AlgorithmIds[0] = mib_info->UnicastCipherAlgorithm;
			pAuthCipherList->uNumOfEntries = 1;
			pAuthCipherList->uTotalNumOfEntries = 1;
		}
        *BytesWritten = pAuthCipherList->uNumOfEntries * sizeof(DOT11_CIPHER_ALGORITHM) + 
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);
            
        *BytesNeeded = pAuthCipherList->uTotalNumOfEntries * sizeof(DOT11_CIPHER_ALGORITHM) +
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryEnabledMulticastCipherAlgorithm(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_CIPHER_ALGORITHM_LIST    pAuthCipherList = NULL;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_CIPHER_ALGORITHM_LIST))
        {
            *BytesNeeded = sizeof(DOT11_CIPHER_ALGORITHM_LIST);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pAuthCipherList = (PDOT11_CIPHER_ALGORITHM_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pAuthCipherList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_CIPHER_ALGORITHM_LIST_REVISION_1,
                                     sizeof(DOT11_CIPHER_ALGORITHM_LIST));
        pAuthCipherList->uNumOfEntries = 0;    
        pAuthCipherList->uTotalNumOfEntries = 0;  


		// Integer overflow
		if (FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) >
			FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
			mib_info->MulticastCipherAlgorithmCount * sizeof(DOT11_CIPHER_ALGORITHM)) {
			return NDIS_STATUS_FAILURE;
		}

		// Ensure enough space for one entry (though this would
		// get saved as part of the DOT11_CIPHER_ALGORITHM_LIST structure itself)
		*BytesNeeded = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
			mib_info->MulticastCipherAlgorithmCount * sizeof(DOT11_CIPHER_ALGORITHM);

		if (InformationBufferLength < *BytesNeeded) {
			pAuthCipherList->uNumOfEntries = 0;
			pAuthCipherList->uTotalNumOfEntries = mib_info->MulticastCipherAlgorithmCount;
			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
			break;
		}

		pAuthCipherList->uNumOfEntries = mib_info->MulticastCipherAlgorithmCount;
		pAuthCipherList->uTotalNumOfEntries = mib_info->MulticastCipherAlgorithmCount;
		NdisMoveMemory(pAuthCipherList->AlgorithmIds,
			mib_info->MulticastCipherAlgorithmList,
			mib_info->MulticastCipherAlgorithmCount * sizeof(DOT11_CIPHER_ALGORITHM));

		#if 0		
        ndisStatus = Sta11QueryEnabledMulticastCipherAlgorithm(
                        pAdapter, 
                        pAuthCipherList, 
                        InformationBufferLength
                        );
		#endif
        *BytesWritten = pAuthCipherList->uNumOfEntries * sizeof(DOT11_CIPHER_ALGORITHM) + 
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);
            
        *BytesNeeded = pAuthCipherList->uTotalNumOfEntries * sizeof(DOT11_CIPHER_ALGORITHM) +
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);
    } while(FALSE);


    return ndisStatus;
}


NDIS_STATUS
MpQueryCipherDefaultKeyID(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	PULONG keyid = InformationBuffer;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

		#if 0
        ndisStatus = Sta11QueryCipherDefaultKeyID(
                        pAdapter->Station, 
                        (PULONG)InformationBuffer
                        );
		#endif
		*keyid = mib_info->DefaultKeyID;

        *BytesWritten = sizeof(ULONG);
        *BytesNeeded = sizeof(ULONG);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpEnumerateAssociationInformation(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_ASSOCIATION_INFO_LIST   pAssocInfoList;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_ASSOCIATION_INFO_LIST))
        {
            *BytesNeeded = sizeof(DOT11_ASSOCIATION_INFO_LIST);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pAssocInfoList = (PDOT11_ASSOCIATION_INFO_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pAssocInfoList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_ASSOCIATION_INFO_LIST_REVISION_1,
                                     sizeof(DOT11_ASSOCIATION_INFO_LIST));
        pAssocInfoList->uNumOfEntries = 0;        
        pAssocInfoList->uTotalNumOfEntries = 0;

		#if 0
        if (pAdapter->Dot11RunningMode == RUNNING_MODE_STA_INFRASTRUCTURE) 
        {
            ndisStatus = Sta11EnumerateAssociationInfoInfra(
                pAdapter->Station,
                pAssocInfoList,
                InformationBufferLength                
                );
        }
        else if (pAdapter->Dot11RunningMode == RUNNING_MODE_STA_ADHOC) 
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            ndisStatus = Sta11EnumerateAssociationInfoAdHoc(
                pAdapter->Station,
                pAssocInfoList,
                InformationBufferLength
                );
        }
        else
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }
		#endif

        *BytesWritten = pAssocInfoList->uTotalNumOfEntries * sizeof(DOT11_ASSOCIATION_INFO_EX) + 
            FIELD_OFFSET(DOT11_ASSOCIATION_INFO_LIST, dot11AssocInfo);
            
        *BytesNeeded = pAssocInfoList->uTotalNumOfEntries * sizeof(DOT11_ASSOCIATION_INFO_EX) +
            FIELD_OFFSET(DOT11_ASSOCIATION_INFO_LIST, dot11AssocInfo);

    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryHardwarePhyState(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	PBOOLEAN pPhyState = InformationBuffer;

	UNREFERENCED_PARAMETER(pAdapter);

    *BytesWritten = 0;
    *BytesNeeded = sizeof(BOOLEAN);

    if (InformationBufferLength < sizeof(BOOLEAN)) {
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }
	
	*pPhyState = TRUE;
    if (ndisStatus == NDIS_STATUS_SUCCESS) {
        *BytesWritten = sizeof(BOOLEAN);
    }

    return ndisStatus;
}

NDIS_STATUS
MpQueryDesiredPhyList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PHY_ID_LIST      PhyIdList;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_PHY_ID_LIST))
        {
            *BytesNeeded = sizeof(DOT11_PHY_ID_LIST);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        PhyIdList = (PDOT11_PHY_ID_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(PhyIdList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_PHY_ID_LIST_REVISION_1,
                                     sizeof(DOT11_PHY_ID_LIST));
        PhyIdList->uNumOfEntries = 0; 
        PhyIdList->uTotalNumOfEntries = 0; 

		#if 0
        ndisStatus = Sta11QueryDesiredPhyList(
                        pAdapter->Station, 
                        PhyIdList, 
                        InformationBufferLength
                        );
		#endif
		
        *BytesWritten = PhyIdList->uNumOfEntries * sizeof(ULONG) + 
                        FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId);
            
        *BytesNeeded = PhyIdList->uTotalNumOfEntries * sizeof(ULONG) +
                       FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId);
    } while (FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryCurrentPhyId(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	PULONG phy_id = InformationBuffer;

    *BytesWritten = 0;
    *BytesNeeded = sizeof(ULONG);

    if (InformationBufferLength < sizeof(ULONG))
    {
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    //ndisStatus = Sta11QueryCurrentPhyId(pAdapter->Station, InformationBuffer);
	*phy_id = mib_info->SelectedPhyId;
	if (ndisStatus == NDIS_STATUS_SUCCESS) {
        *BytesWritten = sizeof(ULONG);
    }

    return ndisStatus;
}

NDIS_STATUS
MpQueryPMKIDList(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __range(sizeof(DOT11_PMKID_LIST) - sizeof(DOT11_PMKID_ENTRY), 0x7FFFFFFF) ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PMKID_LIST      PMKIDList;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        PMKIDList = (PDOT11_PMKID_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(PMKIDList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_PMKID_LIST_REVISION_1,
                                     sizeof(DOT11_PMKID_LIST));
        PMKIDList->uNumOfEntries = 0; 
        PMKIDList->uTotalNumOfEntries = 0; 

		#if 0
        ndisStatus = Sta11QueryPMKIDList(
                        pAdapter->Station, 
                        PMKIDList, 
                        InformationBufferLength
                        );
		#endif

        *BytesWritten = PMKIDList->uNumOfEntries * sizeof(DOT11_PMKID_ENTRY) + 
                        FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId);
            
        *BytesNeeded = PMKIDList->uTotalNumOfEntries * sizeof(DOT11_PMKID_ENTRY) +
                       FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId);
    } while (FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryMediaSteamingOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(InformationBuffer);

    *BytesWritten = 0;
    *BytesNeeded = sizeof(BOOLEAN);

    if (InformationBufferLength < sizeof(BOOLEAN))
    {
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    //ndisStatus = Sta11QueryMediaStreamingOption(pAdapter->Station, InformationBuffer);
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        *BytesWritten = sizeof(BOOLEAN);
    }

    return ndisStatus;

}

NDIS_STATUS
MpQueryUnreachableDetectionThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(InformationBuffer);

    *BytesWritten = 0;
    *BytesNeeded = sizeof(ULONG);

    if (InformationBufferLength < sizeof(ULONG))
    {
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    //ndisStatus = Sta11QueryUnreachableDetectionThreshold(pAdapter->Station, InformationBuffer);
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        *BytesWritten = sizeof(ULONG);
    }

    return ndisStatus;
}

NDIS_STATUS
MpQueryPowerMgmtRequest(
    __in  PADAPTER pAdapter,
    __out PVOID InformationBuffer,
    __in  ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(InformationBuffer);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        // Pass to the station
        *BytesWritten = sizeof(ULONG);
        //ndisStatus = Sta11QueryPowerMgmtRequest(pAdapter->Station, (PULONG)InformationBuffer);
    }
    while (FALSE);


    return ndisStatus;
}

NDIS_STATUS
MpQueryActivePhyList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PHY_ID_LIST      PhyIdList;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, OP_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(DOT11_PHY_ID_LIST))
        {
            *BytesNeeded = sizeof(DOT11_PHY_ID_LIST);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        PhyIdList = (PDOT11_PHY_ID_LIST)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(PhyIdList->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_PHY_ID_LIST_REVISION_1,
                                     sizeof(DOT11_PHY_ID_LIST));
        PhyIdList->uNumOfEntries = 0; 
        PhyIdList->uTotalNumOfEntries = 0; 

		#if 0
        ndisStatus = Sta11QueryActivePhyList(
                        pAdapter->Station, 
                        PhyIdList, 
                        InformationBufferLength
                        );
		#endif
		
        *BytesWritten = PhyIdList->uNumOfEntries * sizeof(ULONG) + 
                        FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId);
            
        *BytesNeeded = PhyIdList->uTotalNumOfEntries * sizeof(ULONG) +
                       FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId);
    } while (FALSE);

    return ndisStatus;
}
    
NDIS_STATUS
MpQueryDot11TableSize(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_EXTSTA_CAPABILITY   pDot11ExtStaCap = NULL;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_EXTSTA_CAPABILITY))
        {
            *BytesNeeded = sizeof(DOT11_EXTSTA_CAPABILITY);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pDot11ExtStaCap = (PDOT11_EXTSTA_CAPABILITY)InformationBuffer;

        MP_ASSIGN_NDIS_OBJECT_HEADER(pDot11ExtStaCap->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_EXTSTA_CAPABILITY_REVISION_1,
                                     sizeof(DOT11_EXTSTA_CAPABILITY));
		
		pDot11ExtStaCap->uScanSSIDListSize = 4;	 // minimum required.
		pDot11ExtStaCap->uDesiredBSSIDListSize = STA_DESIRED_BSSID_MAX_COUNT;
		pDot11ExtStaCap->uDesiredSSIDListSize = 1;
		pDot11ExtStaCap->uExcludedMacAddressListSize = STA_EXCLUDED_MAC_ADDRESS_MAX_COUNT;
		pDot11ExtStaCap->uPrivacyExemptionListSize = 32;
		pDot11ExtStaCap->uKeyMappingTableSize = KEY_TABLE_SIZE - DOT11_MAX_NUM_DEFAULT_KEY;
		pDot11ExtStaCap->uDefaultKeyTableSize = DOT11_MAX_NUM_DEFAULT_KEY;
		pDot11ExtStaCap->uWEPKeyValueMaxLength = 104 / 8;
		pDot11ExtStaCap->uPMKIDCacheSize = STA_PMKID_MAX_COUNT;
		pDot11ExtStaCap->uMaxNumPerSTADefaultKeyTables = PER_STA_KEY_TABLE_SIZE;
		#if 0
        ndisStatus = Sta11QueryExtStaCapability(
            pAdapter->Station,
            pDot11ExtStaCap
            );
		#endif
		
        *BytesWritten = sizeof(DOT11_EXTSTA_CAPABILITY);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryDataRateMappingTable(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_DATA_RATE_MAPPING_TABLE  pDataRateMappingTable = NULL;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_DATA_RATE_MAPPING_TABLE))
        {
            *BytesNeeded = sizeof(DOT11_DATA_RATE_MAPPING_TABLE);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        NdisZeroMemory(InformationBuffer, InformationBufferLength);

        pDataRateMappingTable = (PDOT11_DATA_RATE_MAPPING_TABLE)InformationBuffer;

       MP_ASSIGN_NDIS_OBJECT_HEADER(pDataRateMappingTable->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_DATA_RATE_MAPPING_TABLE_REVISION_1,
                                     sizeof(DOT11_DATA_RATE_MAPPING_TABLE));
		#if 1
        ndisStatus = wf_get_data_rate_map(
                        pAdapter,
                        pDataRateMappingTable,
                        InformationBufferLength
                        );    
		#endif

        *BytesWritten = pDataRateMappingTable->uDataRateMappingLength * sizeof(DOT11_DATA_RATE_MAPPING_ENTRY) + 
            FIELD_OFFSET(DOT11_DATA_RATE_MAPPING_TABLE, DataRateMappingEntries);
            
        *BytesNeeded = pDataRateMappingTable->uDataRateMappingLength * sizeof(DOT11_DATA_RATE_MAPPING_ENTRY) +
            FIELD_OFFSET(DOT11_DATA_RATE_MAPPING_TABLE, DataRateMappingEntries);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryIBSSParameters(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_IBSS_PARAMS))
        {
            *BytesNeeded = sizeof(DOT11_IBSS_PARAMS);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

#ifdef USE_NWIFI_SPEC_1_65
        ((PDOT11_IBSS_PARAMS)InformationBuffer)->uVersion = 0;
#else
        MP_ASSIGN_NDIS_OBJECT_HEADER(((PDOT11_IBSS_PARAMS)InformationBuffer)->Header,
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_IBSS_PARAMS_REVISION_1,
                                     sizeof(DOT11_IBSS_PARAMS));
#endif

		#if 0
        ndisStatus = Sta11QueryIBSSParameters(
            pAdapter->Station,
            InformationBuffer,
            InformationBufferLength,
            BytesWritten,
            BytesNeeded
            );
		#endif

    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryUnicastUseGroupOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(InformationBuffer);

    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(BOOLEAN))
        {
            *BytesNeeded = sizeof(BOOLEAN);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

		#if 0
        ndisStatus = Sta11QueryUnicastUseGroupOption(
                        pAdapter->Station, 
                        (PBOOLEAN)InformationBuffer
                        );
		#endif
		
        *BytesWritten = sizeof(BOOLEAN);
        *BytesNeeded = sizeof(BOOLEAN);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQuerySafeModeOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	PBOOLEAN safe_mode_option = InformationBuffer;
    
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(BOOLEAN))
        {
            *BytesNeeded = sizeof(BOOLEAN);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

		*safe_mode_option = mib_info->SafeModeEnabled;
		#if 0
        ndisStatus = Sta11QuerySafeModeOption(
                        pAdapter->Station, 
                        (PBOOLEAN)InformationBuffer
                        );
		#endif
        *BytesWritten = sizeof(BOOLEAN);
        *BytesNeeded = sizeof(BOOLEAN);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpQueryHiddenNetworkOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(InformationBuffer);
    do
    {
        *BytesWritten = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(BOOLEAN))
        {
            *BytesNeeded = sizeof(BOOLEAN);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

		#if 0
        ndisStatus = Sta11QueryHiddenNetworkOption(
                        pAdapter->Station, 
                        (PBOOLEAN)InformationBuffer
                        );
		#endif

		*((PBOOLEAN)InformationBuffer) = mib_info->hidden_network_enable;
			
        *BytesWritten = sizeof(BOOLEAN);
        *BytesNeeded = sizeof(BOOLEAN);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpFlushBSSList(
    __in PADAPTER pAdapter
    )
{
	UNREFERENCED_PARAMETER(pAdapter);

	return NDIS_STATUS_SUCCESS;//Sta11FlushBSSList(pAdapter->Station);
}

NDIS_STATUS
MpSetPowerMgmtRequest(
    __in  PADAPTER pAdapter,
    __in  PVOID InformationBuffer,
    __in  ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(InformationBuffer);

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        // Pass to the station
        *BytesRead = sizeof(ULONG);
        //ndisStatus = Sta11SetPowerMgmtRequest(pAdapter->Station, *((PULONG)InformationBuffer));
    }
    while (FALSE);
        
    return ndisStatus;
}

NDIS_STATUS
MpSetDesiredSSIDList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_SSID_LIST    pDot11SSIDList = NULL;
	pwf_mib_info_t mib_info = pAdapter->mib_info;
	nic_info_st *nic_info = pAdapter->nic_info;
	sec_info_st *sec_info;

	if(nic_info == NULL) {
		LOG_E("nic info is NULL");
		return NDIS_STATUS_SOFT_ERRORS;
	}

	sec_info = nic_info->sec_info;
	if(sec_info == NULL) {
		LOG_E("sec info is NULL");
		return NDIS_STATUS_SOFT_ERRORS;
	}
	
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_SSID_LIST, SSIDs))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_SSID_LIST, SSIDs);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pDot11SSIDList = (PDOT11_SSID_LIST)InformationBuffer;


        if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pDot11SSIDList->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_SSID_LIST_REVISION_1,
                                          sizeof(DOT11_SSID_LIST)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // Must have atleast one SSID in the list
        if (pDot11SSIDList->uNumOfEntries < 1)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // Verify length/number of entries match up
        *BytesNeeded = pDot11SSIDList->uNumOfEntries * sizeof(DOT11_SSID) +
            FIELD_OFFSET(DOT11_SSID_LIST, SSIDs);

        if (InformationBufferLength < (*BytesNeeded))
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

		memcpy(&mib_info->curr_ssid, pDot11SSIDList, sizeof(DOT11_SSID_LIST));
		wf_memset(sec_info, 0, sizeof(sec_info_st));
		
		//pAdapter->MibInfo.curr_ssid = pDot11SSIDList;
        // Pass to the station
        if (pDot11SSIDList->uNumOfEntries != 1)
        {
        	LOG_E("Only support 1 desired SSID entry.");
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }
        *BytesRead =  FIELD_OFFSET(DOT11_SSID_LIST, SSIDs)
                        + 1 * sizeof(DOT11_SSID);

    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetExcludedMACAddressList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_MAC_ADDRESS_LIST    pDot11MacAddrList = NULL;

	UNREFERENCED_PARAMETER(pAdapter);
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pDot11MacAddrList = (PDOT11_MAC_ADDRESS_LIST)InformationBuffer;

        if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pDot11MacAddrList->Header,  
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_MAC_ADDRESS_LIST_REVISION_1,
                                          sizeof(DOT11_MAC_ADDRESS_LIST)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        if (pDot11MacAddrList->uNumOfEntries > 0)
        {
            //
            // Ensure enough space for all the entries
            //
            *BytesNeeded = pDot11MacAddrList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS) +
                FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs);
            
            if (InformationBufferLength < (*BytesNeeded))
            {
                ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
                break;
            }
        }

        // Pass to the station
        #if 0
        ndisStatus = Sta11SetExcludedMACAddressList(
            pAdapter,
            pDot11MacAddrList,
            BytesRead
            );
        #endif
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetExcludeUnencrypted(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	PBOOLEAN param = InformationBuffer;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(BOOLEAN))
        {
            *BytesNeeded = sizeof(BOOLEAN);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        // Pass to the station
        *BytesRead = sizeof(BOOLEAN);

		mib_info->ExcludeUnencrypted = *param;
		#if 0
        ndisStatus = Sta11SetExcludeUnencrypted(
            pAdapter,
            *((PBOOLEAN)InformationBuffer)
            );
        #endif
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetPrivacyExemptionList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PRIVACY_EXEMPTION_LIST   list;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        list = (PDOT11_PRIVACY_EXEMPTION_LIST)InformationBuffer;

        if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(list->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_PRIVACY_EXEMPTION_LIST_REVISION_1,
                                          sizeof(DOT11_PRIVACY_EXEMPTION_LIST)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        *BytesNeeded = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) +
                       list->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION);
            
        if (InformationBufferLength < (*BytesNeeded))
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        // Pass to the station
        //ndisStatus = Sta11SetPrivacyExemptionList(pAdapter, list, BytesRead);
        
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetDesiredBSSIDList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_BSSID_LIST    pDot11BSSIDList = NULL;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs))        {
            *BytesNeeded = FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pDot11BSSIDList = (PDOT11_BSSID_LIST)InformationBuffer;

        if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pDot11BSSIDList->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_BSSID_LIST_REVISION_1,
                                          sizeof(DOT11_BSSID_LIST)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }
        
        if (pDot11BSSIDList->uNumOfEntries > 0)
        {
            //
            // Ensure enough space for all the entries
            //
            *BytesNeeded = pDot11BSSIDList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS) +
                FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs);
            
            if (InformationBufferLength < (*BytesNeeded))
            {
                ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
                break;
            }
        }

        // Pass to the station
        #if 0
        ndisStatus = Sta11SetDesiredBSSIDList(
            pAdapter->Station,
            pDot11BSSIDList,
            BytesRead
            );
        #endif
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetDesiredBSSType(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_BSS_TYPE     pDot11DesiredBSSType;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(DOT11_BSS_TYPE))
        {
            *BytesNeeded = sizeof(DOT11_BSS_TYPE);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pDot11DesiredBSSType = (PDOT11_BSS_TYPE)InformationBuffer;

        //
        // Validate
        //
        if (//(*pDot11DesiredBSSType != dot11_BSS_type_independent) &&
            (*pDot11DesiredBSSType != dot11_BSS_type_infrastructure) &&
            (*pDot11DesiredBSSType != dot11_BSS_type_any)
            )
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        //
        // Pass to station
        //
        #if 0
        ndisStatus = Sta11SetDesiredBSSType(
            pAdapter,
            *pDot11DesiredBSSType            
            );
		
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }
		#endif

		//StaSetPowerSavingLevel();
		StaResetCipherAbilities(pAdapter);
        
        if (*pDot11DesiredBSSType == dot11_BSS_type_independent)
            pAdapter->Dot11RunningMode = RUNNING_MODE_STA_ADHOC;
        else if (*pDot11DesiredBSSType == dot11_BSS_type_infrastructure ||
			*pDot11DesiredBSSType == dot11_BSS_type_any)
            pAdapter->Dot11RunningMode = RUNNING_MODE_STA_INFRASTRUCTURE;
        else
            ndisStatus = NDIS_STATUS_NOT_SUPPORTED;

        *BytesRead = sizeof(DOT11_BSS_TYPE);
    } while(FALSE);

    return ndisStatus;
}

//void print_buffer(PUCHAR title, LONG idx, PUCHAR src, LONG length);


NDIS_STATUS
MpConnectRequest(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	//UCHAR *ptr = InformationBuffer;

	//DbgPrint("connect_req: len=%d\n", InformationBufferLength);
	//if(InformationBufferLength != 0) {
	//	print_buffer("connect_req", 0, ptr, InformationBufferLength);
	//}
	
    UNREFERENCED_PARAMETER(InformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(BytesRead);
    UNREFERENCED_PARAMETER(BytesNeeded);

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }
		
        //
        // If a reset is in progress on the NIC, fail this request
        //
        if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS)) 
        {
            LOG_E("Connection failed as a reset is in progress on this adapter");
            return NDIS_STATUS_RESET_IN_PROGRESS;
        }

        //
        // If a halt is in progress on the NIC, fail this request
        //
        if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HALT_IN_PROGRESS)) 
        {
            LOG_E("Connection failed as a halt is in progress on this adapter");
            return NDIS_STATUS_DOT11_MEDIA_IN_USE;
        }           

        //
        // If the NIC has unrecoverable error, fail this request
        //
        if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_NON_RECOVER_ERROR)) 
        {
            LOG_E("Connection failed as this adapter has an unrecoverable error");
            return NDIS_STATUS_HARD_ERRORS;
        }      

        //
        // If NIC is in off power state, start it
        //
        //if (MP_NIC_POWER_STATE_IS_OFF(pAdapter))
        //{
        //    MpTrace(COMP_OID, DBG_SERIOUS,  ("Connect failed as hardware is in power off state\n"));
        //    return NDIS_STATUS_DOT11_POWER_STATE_INVALID;
        //}

        if (pAdapter->Dot11RunningMode != RUNNING_MODE_STA_ADHOC) {
            //
            // Pass the request to the station
            //
            //ndisStatus = Sta11ConnectInfra(pAdapter->Station);
			if (mib_info->curr_ssid.SSIDs[0].uSSIDLength == 0)
			{
				DbgPrint("get ssid fail\n");
				return NDIS_STATUS_FAILURE;
			}
			ndisStatus = wf_set_start_assoc(pAdapter, &mib_info->curr_ssid);
            if ((ndisStatus == NDIS_STATUS_SUCCESS) || (ndisStatus == NDIS_STATUS_PENDING))
            {
                pAdapter->State = OP_STATE;
                pAdapter->Dot11RunningMode = RUNNING_MODE_STA_INFRASTRUCTURE;
            }
        }

        if (pAdapter->Dot11RunningMode != RUNNING_MODE_STA_INFRASTRUCTURE) {
            //
            // Pass the request to the station
            //
            //ndisStatus = Sta11ConnectAdHoc(pAdapter->Station);
            
            if ((ndisStatus == NDIS_STATUS_SUCCESS) || (ndisStatus == NDIS_STATUS_PENDING))
            {
                pAdapter->State = OP_STATE;
                pAdapter->Dot11RunningMode = RUNNING_MODE_STA_ADHOC; 
            }
        }

    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpDisconnectRequest(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    UNREFERENCED_PARAMETER(InformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(BytesRead);
    UNREFERENCED_PARAMETER(BytesNeeded);

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, OP_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (pAdapter->Dot11RunningMode == RUNNING_MODE_STA_INFRASTRUCTURE) {
            // Pass the request to the station
            ndisStatus = wf_set_start_deassoc(pAdapter);//Sta11DisconnectInfra(pAdapter);
            
            pAdapter->State = INIT_STATE;
        }
        else {
            //ndisStatus = Sta11DisconnectAdHoc(pAdapter->Station);
			LOG_D("---Adhoc not supported yet.");
            pAdapter->State = INIT_STATE;
        }

    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetEnabledAuthenticationAlgorithm(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_AUTH_ALGORITHM_LIST pAuthAlgoList = NULL;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pAuthAlgoList = (PDOT11_AUTH_ALGORITHM_LIST)InformationBuffer;

        if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pAuthAlgoList->Header, 
                                                  NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_AUTH_ALGORITHM_LIST_REVISION_1,
                                          sizeof(DOT11_AUTH_ALGORITHM_LIST)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // Must have atleast one entry in the list
        if (pAuthAlgoList->uNumOfEntries < 1)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // Verify length/number of entries match up
        *BytesNeeded = pAuthAlgoList->uNumOfEntries * sizeof(DOT11_AUTH_ALGORITHM) +
            FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds);

        if (InformationBufferLength < *BytesNeeded)
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        ndisStatus = wf_set_auth_alg( //Sta11SetEnabledAuthenticationAlgorithm(
                        pAdapter, 
                        pAuthAlgoList, 
                        BytesRead
                        );
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetEnabledUnicastCipherAlgorithm(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_CIPHER_ALGORITHM_LIST    pCipherAlgoList = NULL;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pCipherAlgoList = (PDOT11_CIPHER_ALGORITHM_LIST)InformationBuffer;

        if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pCipherAlgoList->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_CIPHER_ALGORITHM_LIST_REVISION_1,
                                          sizeof(DOT11_CIPHER_ALGORITHM_LIST)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        
        // Must have atleast one entry in the list
        if (pCipherAlgoList->uNumOfEntries < 1)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // Verify length/number of entries match up
        *BytesNeeded = pCipherAlgoList->uNumOfEntries * sizeof(DOT11_CIPHER_ALGORITHM) +
                       FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);

        if (InformationBufferLength < *BytesNeeded)
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

		#if 0
        ndisStatus = Sta11SetEnabledUnicastCipherAlgorithm(
                        pAdapter, 
                        pCipherAlgoList, 
                        BytesRead
                        );
		#endif
		ndisStatus = wf_set_unicast_cipher_alg(
						pAdapter, 
                        pCipherAlgoList, 
                        BytesRead
                        );
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetEnabledMulticastCipherAlgorithm(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_CIPHER_ALGORITHM_LIST    pCipherAlgoList = NULL;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pCipherAlgoList = (PDOT11_CIPHER_ALGORITHM_LIST)InformationBuffer;

        if (!MP_VERIFY_NDIS_OBJECT_HEADER(pCipherAlgoList->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_CIPHER_ALGORITHM_LIST_REVISION_1,
                                          sizeof(DOT11_CIPHER_ALGORITHM_LIST)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        
        // Must have atleast one entry in the list
        if (pCipherAlgoList->uNumOfEntries < 1)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // Verify length/number of entries match up
        *BytesNeeded = pCipherAlgoList->uNumOfEntries * sizeof(DOT11_CIPHER_ALGORITHM) +
                       FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);

        if (InformationBufferLength < *BytesNeeded)
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

		#if 0
        ndisStatus = Sta11SetEnabledMulticastCipherAlgorithm(
                        pAdapter->Station, 
                        pCipherAlgoList, 
                        BytesRead
                        );
		#endif
		ndisStatus = wf_set_multicast_cipher_alg(
						pAdapter, 
                        pCipherAlgoList, 
                        BytesRead);
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetCipherDefaultKeyID(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       keyID;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        keyID = *((PULONG)InformationBuffer);
        *BytesRead = sizeof(ULONG);

		#if 0
        ndisStatus = Sta11SetCipherDefaultKeyID(
                        pAdapter->Station, 
                        keyID
                        );
		#endif
		if(keyID >= DOT11_MAX_NUM_DEFAULT_KEY) {
			ndisStatus = NDIS_STATUS_INVALID_DATA;
		}
		mib_info->DefaultKeyID = (UCHAR)keyID;
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetCipherDefaultKey(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_CIPHER_DEFAULT_KEY_VALUE defaultKey;
	nic_info_st *pnic_info = pAdapter->nic_info;
	wdn_net_info_st *pwdn_info = wf_wdn_find_info(pnic_info,
                                         wf_wlan_get_cur_bssid(pnic_info));
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_CIPHER_DEFAULT_KEY_VALUE, ucKey))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_CIPHER_DEFAULT_KEY_VALUE, ucKey);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        defaultKey = (PDOT11_CIPHER_DEFAULT_KEY_VALUE)InformationBuffer;
		*BytesNeeded = FIELD_OFFSET(DOT11_CIPHER_DEFAULT_KEY_VALUE, ucKey) + 
                       defaultKey->usKeyLength;

         if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(defaultKey->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_CIPHER_DEFAULT_KEY_VALUE_REVISION_1,
                                          sizeof(DOT11_CIPHER_DEFAULT_KEY_VALUE)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        if (InformationBufferLength < *BytesNeeded)
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
        ndisStatus = Sta11SetCipherDefaultKey(
                        pAdapter, 
                        defaultKey
                        );
        *BytesRead = *BytesNeeded;
    } while(FALSE);

	if(ndisStatus == NDIS_STATUS_SUCCESS) {
		if(pwdn_info) pwdn_info->ieee8021x_blocked = wf_false;
	}

    return ndisStatus;
}

NDIS_STATUS
MpSetCipherKeyMappingKey(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_BYTE_ARRAY KeyData;

	do
    {
        *BytesRead = 0;
        *BytesNeeded = FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer);

        if (InformationBufferLength < sizeof(DOT11_BYTE_ARRAY))
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        KeyData = (PDOT11_BYTE_ARRAY)InformationBuffer;
        *BytesNeeded = FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer) + 
                       KeyData->uNumOfBytes;

        if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(KeyData->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_CIPHER_KEY_MAPPING_KEY_VALUE_BYTE_ARRAY_REVISION_1,
                                          sizeof(DOT11_BYTE_ARRAY)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        if (InformationBufferLength < *BytesNeeded)
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        ndisStatus = Sta11SetCipherKeyMappingKey(
                        pAdapter, 
                        (PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE)KeyData->ucBuffer,
                        KeyData->uNumOfBytes
                        );
        *BytesRead = *BytesNeeded;
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetDesiredPhyList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PHY_ID_LIST      PhyIdList;

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(DOT11_PHY_ID_LIST))
        {
            *BytesNeeded = sizeof(DOT11_PHY_ID_LIST);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        PhyIdList = (PDOT11_PHY_ID_LIST)InformationBuffer;

        if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(PhyIdList->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_PHY_ID_LIST_REVISION_1,
                                          sizeof(DOT11_PHY_ID_LIST)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // Must have atleast one entry in the list
        if (PhyIdList->uNumOfEntries < 1)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // Verify length/number of entries match up
        *BytesNeeded = PhyIdList->uNumOfEntries * sizeof(ULONG) +
                       FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId);

        if (InformationBufferLength < *BytesNeeded)
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

		#if 0
        ndisStatus = Sta11SetDesiredPhyList(
                        pAdapter->Station, 
                        PhyIdList, 
                        BytesRead
                        );
		#endif
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetCurrentPhyId(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG       PhyId;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        *BytesRead = sizeof(ULONG);
        PhyId = *((PULONG)InformationBuffer);
        //ndisStatus = Sta11SetCurrentPhyId(pAdapter->Station, PhyId);
		if (PhyId >= mib_info->pSupportedPhyTypes->uNumOfEntries)
		{
			return NDIS_STATUS_INVALID_DATA;
		}
		else
		{
			//LOG_D(">>> Selected PhyId: %d <<<", PhyId);
			mib_info->SelectedPhyId = PhyId;
			mib_info->SelectedPhyMIB = &(mib_info->PhyMIB[PhyId]);
			return NDIS_STATUS_SUCCESS;
		}
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetPMKIDList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PMKID_LIST      PMKIDList;

	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < (ULONG)FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs))
        {
            *BytesNeeded = FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        PMKIDList = (PDOT11_PMKID_LIST)InformationBuffer;

         if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(PMKIDList->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_PMKID_LIST_REVISION_1,
                                          sizeof(DOT11_PMKID_LIST)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // Must have atleast one entry in the list
        if (PMKIDList->uNumOfEntries < 1)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;
        }

        // Verify length/number of entries match up
        *BytesNeeded = PMKIDList->uNumOfEntries * sizeof(DOT11_PMKID_ENTRY) +
                       FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs);

        if (InformationBufferLength < *BytesNeeded)
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

		#if 0
        ndisStatus = Sta11SetPMKIDList(
                        pAdapter->Station, 
                        PMKIDList, 
                        BytesRead
                        );
		#endif
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetMediaStreamingOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN     bMediaStreamingEnabled = FALSE;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesRead = 0;
        *BytesNeeded = sizeof(BOOLEAN);

        if (InformationBufferLength < sizeof(BOOLEAN))
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        *BytesRead = sizeof(BOOLEAN);
        bMediaStreamingEnabled = *((PBOOLEAN)InformationBuffer);
        //ndisStatus = Sta11SetMediaStreamingOption(pAdapter->Station, bMediaStreamingEnabled);
		{
			//this is only do not scan, may we should not call function wf_mcu_set_media_status
			//wf_mcu_set_media_status(pAdapter->nic_info, WIFI_FW_STATION_STATE);
		}
		
    } while(FALSE);

    return ndisStatus;
}
    
NDIS_STATUS
MpSetUnreachableDetectionThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(InformationBuffer);

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        *BytesRead = sizeof(ULONG);
        //ndisStatus = Sta11SetUnreachableDetectionThreshold(pAdapter->Station, *((PULONG)InformationBuffer));
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetIBSSParameters(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_IBSS_PARAMS  pDot11IBSSParams = NULL;
    
	UNREFERENCED_PARAMETER(pAdapter);

    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (InformationBufferLength < sizeof(DOT11_IBSS_PARAMS))
        {
            *BytesNeeded = sizeof(DOT11_IBSS_PARAMS);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        pDot11IBSSParams = (PDOT11_IBSS_PARAMS)InformationBuffer;

        if (!MP_VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pDot11IBSSParams->Header, 
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_IBSS_PARAMS_REVISION_1,
                                          sizeof(DOT11_IBSS_PARAMS)))
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            break;            
        }

        //
        // Verify IE blob length
        //
        if ((pDot11IBSSParams->uIEsOffset + pDot11IBSSParams->uIEsLength) > InformationBufferLength)
        {
            *BytesNeeded = pDot11IBSSParams->uIEsOffset + pDot11IBSSParams->uIEsLength;
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }
		#if 0
        ndisStatus = Sta11SetIBSSParameters(
            pAdapter->Station,
            pDot11IBSSParams,
            BytesRead
            );
		#endif
    } while(FALSE);

    return ndisStatus;
}

NDIS_STATUS
MpSetUnicastUseGroupOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN     bUnicastUseGroupEnabled;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(BOOLEAN))
        {
            *BytesNeeded = sizeof(BOOLEAN);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        *BytesRead = sizeof(BOOLEAN);
        bUnicastUseGroupEnabled = *((PBOOLEAN)InformationBuffer);
		#if 0
        ndisStatus = Sta11SetUnicastUseGroupOption(
            pAdapter->Station, 
            bUnicastUseGroupEnabled
            );
		#endif
    } while(FALSE);

    return ndisStatus;
}



NDIS_STATUS
MpSetSafeModeOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN     bSafeModeEnabled;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(BOOLEAN))
        {
            *BytesNeeded = sizeof(BOOLEAN);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        *BytesRead = sizeof(BOOLEAN);
        bSafeModeEnabled = *((PBOOLEAN)InformationBuffer);
		#if 0
        ndisStatus = Sta11SetSafeModeOption(
            pAdapter->Station, 
            bSafeModeEnabled
            );
		#endif
		if(bSafeModeEnabled) {
			wf_set_encryption(pAdapter, TRUE, DOT11_CIPHER_ALGO_NONE);
        	wf_set_encryption(pAdapter, FALSE, DOT11_CIPHER_ALGO_NONE);
		} else {
			MPVERIFY(mib_info->HwUnicastCipher == DOT11_CIPHER_ALGO_NONE);
		}
		
		mib_info->SafeModeEnabled = bSafeModeEnabled;
		
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            //MpAdjustReceiveHandler(pAdapter);
        }
    } while(FALSE);

    return ndisStatus;
}


NDIS_STATUS
MpSetHiddenNetworkOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN     bHiddenNetworkEnabled;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
    
    do
    {
        *BytesRead = 0;
        *BytesNeeded = 0;

        if (!MP_VERIFY_STATE(pAdapter, INIT_STATE))
        {
            ndisStatus = NDIS_STATUS_INVALID_STATE;
            break;
        }

        if (InformationBufferLength < sizeof(BOOLEAN))
        {
            *BytesNeeded = sizeof(BOOLEAN);
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            break;
        }

        *BytesRead = sizeof(BOOLEAN);
       mib_info->hidden_network_enable = *((PBOOLEAN)InformationBuffer);
		#if 0
        ndisStatus = Sta11SetHiddenNetworkOption(
            pAdapter->Station, 
            bHiddenNetworkEnabled
            );
		#endif
    } while(FALSE);

    return ndisStatus;
}


NDIS_STATUS
MpQueryInformation(
    __in    PADAPTER                  pAdapter,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InformationBufferLength,
    __out   PULONG                    BytesWritten,
    __out   PULONG                    BytesNeeded
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_HARDWARE_STATUS        HardwareStatus = NdisHardwareStatusReady;
    NDIS_MEDIUM                 Medium = MP_MEDIA_TYPE;
    UCHAR                       VendorDesc[] = MP_VENDOR_DESC;
    ULONG                       uInfo = 0;
    ULONG64                     ullInfo64 = 0;
    USHORT                      usInfo = 0;
    PVOID                       pInfo = (PVOID)&uInfo;
    ULONG                       uInfoLen = 0;

//    MpEntry;

    // Initialize the result
    *BytesWritten = 0;
    *BytesNeeded = 0;

	if(Oid != OID_DOT11_STATISTICS) {
    	LOG_D("Querying OID: 0x%08x %s", Oid, DbgGetOidName(Oid));
	}
#if 0

	return NDIS_STATUS_SUCCESS;
#endif // 1

    
//	ndisStatus = MpEventQueryInformation(
//                    pAdapter,
//                    Oid,
//                    InformationBuffer,
//                    InformationBufferLength,
//                    BytesWritten,
//                    BytesNeeded
//                    );
//
//    if (ndisStatus != NDIS_STATUS_INVALID_OID)
//    {
//        //
//        // This Oid has been responded to by the MpEventQueryInformation function
//        // We will not handle this Oid and return to the OS.
//        //
//        LOG_E("Oid intercepted by MpEventQueryInformation! Status 0x%08x", ndisStatus);
//        return ndisStatus;
//    }

    //
    // Assume OID succeeds by default. Failure cases will set it as failure.
    //
    ndisStatus = NDIS_STATUS_SUCCESS;

    
    switch (Oid)
    {

    case OID_DOT11_MPDU_MAX_LENGTH:
        ndisStatus = MpQueryMPDUMaxLength(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_OPERATION_MODE_CAPABILITY:
        ndisStatus = MpQueryOperationModeCapability(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CURRENT_OPERATION_MODE:
        ndisStatus = MpQueryCurrentOperationMode(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ATIM_WINDOW:
        ndisStatus = MpQueryATIMWindow(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_NIC_POWER_STATE:
        ndisStatus = MpQueryNicPowerState(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_OPTIONAL_CAPABILITY:
        ndisStatus = MpQueryOptionalCapability(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CURRENT_OPTIONAL_CAPABILITY:
        ndisStatus = MpQueryCurrentOptionalCapability(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CF_POLLABLE:
        ndisStatus = MpQueryCFPollable(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_OPERATIONAL_RATE_SET:
        ndisStatus = MpQueryOperationalRateSet(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_BEACON_PERIOD:
        ndisStatus = MpQueryBeaconPeriod(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_RTS_THRESHOLD:
        ndisStatus = MpQueryRTSThreshold(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_SHORT_RETRY_LIMIT:
        ndisStatus = MpQueryShortRetryLimit(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_LONG_RETRY_LIMIT:
        ndisStatus = MpQueryLongRetryLimit(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_FRAGMENTATION_THRESHOLD:
        ndisStatus = MpQueryFragmentationThreshold(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_MAX_TRANSMIT_MSDU_LIFETIME:
        ndisStatus = MpQueryMaxTXMSDULifeTime(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_MAX_RECEIVE_LIFETIME:
        ndisStatus = MpQueryMaxReceiveLifeTime(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_SUPPORTED_PHY_TYPES:
        ndisStatus = MpQuerySupportedPHYTypes(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CURRENT_REG_DOMAIN:
        ndisStatus = MpQueryCurrentRegDomain(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_TEMP_TYPE:
        ndisStatus = MpQueryTempType(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DIVERSITY_SUPPORT:
        ndisStatus = MpQueryDiversitySupport(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;


    case OID_DOT11_SUPPORTED_POWER_LEVELS:
        ndisStatus = MpQuerySupportedPowerLevels(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CURRENT_TX_POWER_LEVEL:
        ndisStatus = MpQueryCurrentTXPowerLevel(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    //
    // * IHV ALERT *
    // The next four OIDs are DSSS phy specific.
    // This is the only phy supportedby RTL 8180.
    // If the hardware supports a different PHY,
    // corresponding OIDs have to be supported
    //
    case OID_DOT11_CURRENT_CHANNEL:
        ndisStatus = MpQueryCurrentChannel(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CCA_MODE_SUPPORTED:
        ndisStatus = MpQueryCCAModeSupported(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CURRENT_CCA_MODE:
        ndisStatus = MpQueryCurrentCCA(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ED_THRESHOLD:
        ndisStatus = MpQueryEDThreshold(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    //
    // OFDM PHY
    //
    case OID_DOT11_CURRENT_FREQUENCY:
        ndisStatus = MpQueryCurrentFrequency(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_FREQUENCY_BANDS_SUPPORTED:
        ndisStatus = MpQueryFrequencyBandsSupported(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_REG_DOMAINS_SUPPORT_VALUE:
        ndisStatus = MpQueryRegDomainsSupportValue(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_SUPPORTED_TX_ANTENNA:
        ndisStatus = MpQuerySupportedTXAntenna(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_SUPPORTED_RX_ANTENNA:
        ndisStatus = MpQuerySupportedRXAntenna(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DIVERSITY_SELECTION_RX:
        ndisStatus = MpQueryDiversitySelectionRX(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_SUPPORTED_DATA_RATES_VALUE:
        ndisStatus = MpQuerySupportedDataRatesValue(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_RF_USAGE:
        //uInfo = Hw11QueryRFUsage(pAdapter->Nic);
        uInfoLen = sizeof(ULONG);
        break;

    case OID_DOT11_MAX_MAC_ADDRESS_STATES:
        uInfo = HW_MAX_MAC_ADDRESS_STATES;
        uInfoLen = sizeof(ULONG);
        break;

    case OID_DOT11_MULTI_DOMAIN_CAPABILITY_IMPLEMENTED:
    case OID_DOT11_MULTI_DOMAIN_CAPABILITY_ENABLED:
    case OID_DOT11_COUNTRY_STRING:
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        break;

    case OID_DOT11_SHORT_PREAMBLE_OPTION_IMPLEMENTED:    
        //ndisStatus = Hw11QueryShortPreambleOptionImplemented(pAdapter->Nic, (PBOOLEAN)&uInfo, TRUE);
        uInfoLen = sizeof(BOOLEAN);
        break;

    case OID_DOT11_PBCC_OPTION_IMPLEMENTED:
        //ndisStatus = Hw11QueryPbccOptionImplemented(pAdapter->Nic, (PBOOLEAN)&uInfo, TRUE);
        uInfoLen = sizeof(BOOLEAN);
        break;

    case OID_DOT11_ERP_PBCC_OPTION_IMPLEMENTED:
        //ndisStatus = Hw11QueryErpPbccOptionImplemented(pAdapter->Nic, (PBOOLEAN)&uInfo, TRUE);
        uInfoLen = sizeof(BOOLEAN);
        break;

    case OID_DOT11_ERP_PBCC_OPTION_ENABLED:
        //ndisStatus = Hw11QueryErpPbccOptionEnabled(pAdapter->Nic, (PBOOLEAN)&uInfo, TRUE);
        uInfoLen = sizeof(BOOLEAN);
        break;

    case OID_DOT11_DSSS_OFDM_OPTION_IMPLEMENTED:
        //ndisStatus = Hw11QueryDsssOfdmOptionImplemented(pAdapter->Nic, (PBOOLEAN)&uInfo, TRUE);
        uInfoLen = sizeof(BOOLEAN);
        break;
        
    case OID_DOT11_DSSS_OFDM_OPTION_ENABLED:
        //ndisStatus = Hw11QueryDsssOfdmOptionEnabled(pAdapter->Nic, (PBOOLEAN)&uInfo, TRUE);
        uInfoLen = sizeof(BOOLEAN);
        break;

    case OID_DOT11_SHORT_SLOT_TIME_OPTION_IMPLEMENTED:
        //ndisStatus = Hw11QueryShortSlotTimeOptionImplemented(pAdapter->Nic, (PBOOLEAN)&uInfo, TRUE);
        uInfoLen = sizeof(BOOLEAN);
        break;

    case OID_DOT11_SHORT_SLOT_TIME_OPTION_ENABLED:
        //ndisStatus = Hw11QueryShortSlotTimeOptionEnabled(pAdapter->Nic, (PBOOLEAN)&uInfo, TRUE);
        uInfoLen = sizeof(BOOLEAN);
        break;        


    case OID_DOT11_AUTO_CONFIG_ENABLED:
        ndisStatus = MpQueryAutoConfigOption(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );        
        break;
        
    case OID_DOT11_POWER_MGMT_REQUEST:
        ndisStatus = MpQueryPowerMgmtRequest(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DESIRED_SSID_LIST:
        ndisStatus = MpQueryDesiredSSIDList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST:
        ndisStatus = MpQueryExcludedMACAddressList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_EXCLUDE_UNENCRYPTED:
        ndisStatus = MpQueryExcludeUnencrypted(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DESIRED_BSSID_LIST:
        ndisStatus = MpQueryDesiredBSSIDList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DESIRED_BSS_TYPE:
        ndisStatus = MpQueryDesiredBSSType(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_STATISTICS:
        ndisStatus = MpQueryDot11Statistics(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM:
        ndisStatus = MpQueryEnabledAuthenticationAlgorithm(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_SUPPORTED_UNICAST_ALGORITHM_PAIR:
        ndisStatus = MpQuerySupportedUnicastAlgorithmPair(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_SUPPORTED_MULTICAST_ALGORITHM_PAIR:
        ndisStatus = MpQuerySupportedMulticastAlgorithmPair(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ENABLED_UNICAST_CIPHER_ALGORITHM:
        ndisStatus = MpQueryEnabledUnicastCipherAlgorithm(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ENABLED_MULTICAST_CIPHER_ALGORITHM:
        ndisStatus = MpQueryEnabledMulticastCipherAlgorithm(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CIPHER_DEFAULT_KEY_ID:
        ndisStatus = MpQueryCipherDefaultKeyID(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;
        
    case OID_DOT11_ENUM_ASSOCIATION_INFO:
        ndisStatus = MpEnumerateAssociationInformation(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_HARDWARE_PHY_STATE:
        ndisStatus = MpQueryHardwarePhyState(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DESIRED_PHY_LIST:
        ndisStatus = MpQueryDesiredPhyList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CURRENT_PHY_ID:
        ndisStatus = MpQueryCurrentPhyId(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_PMKID_LIST:
        ndisStatus = MpQueryPMKIDList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_MEDIA_STREAMING_ENABLED:
        ndisStatus = MpQueryMediaSteamingOption(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_UNREACHABLE_DETECTION_THRESHOLD:
        ndisStatus = MpQueryUnreachableDetectionThreshold(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ACTIVE_PHY_LIST:
        ndisStatus = MpQueryActivePhyList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_EXTSTA_CAPABILITY:
        ndisStatus = MpQueryDot11TableSize(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DATA_RATE_MAPPING_TABLE:
        ndisStatus = MpQueryDataRateMappingTable(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );

        break;

    case OID_DOT11_PRIVACY_EXEMPTION_LIST:
        ndisStatus = MpQueryPrivacyExemptionList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_IBSS_PARAMS:
        ndisStatus = MpQueryIBSSParameters(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_UNICAST_USE_GROUP_ENABLED:
        ndisStatus = MpQueryUnicastUseGroupOption(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_SAFE_MODE_ENABLED:
        ndisStatus = MpQuerySafeModeOption(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_HIDDEN_NETWORK_ENABLED:
        ndisStatus = MpQueryHiddenNetworkOption(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

        

    case OID_GEN_SUPPORTED_LIST:
        pInfo = (PVOID)&MPSupportedOids;
        uInfoLen = sizeof(MPSupportedOids);
        break;

    case OID_GEN_SUPPORTED_GUIDS:
        //
        // If the driver supports any GUIDS, they will
        // have to be reported here. By default, driver should
        // report NDIS_STATUS_NOT_SUPPORTED
        //
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;//Hw11QuerySupportedGUIDS(pAdapter);
        break;

    case OID_GEN_HARDWARE_STATUS:
        pInfo = (PVOID) &HardwareStatus;
        uInfoLen = sizeof(NDIS_HARDWARE_STATUS);
        break;

    case OID_GEN_MEDIA_SUPPORTED:
    case OID_GEN_MEDIA_IN_USE:
        pInfo = (PVOID) &Medium;
        uInfoLen = sizeof(NDIS_MEDIUM);
        break;

    case OID_GEN_PHYSICAL_MEDIUM:
        uInfo = MP_PHYSICAL_MEDIA_TYPE;
        uInfoLen = sizeof(NDIS_PHYSICAL_MEDIUM);
        break;

    case OID_GEN_CURRENT_LOOKAHEAD:
    case OID_GEN_MAXIMUM_LOOKAHEAD:
        uInfo = MP_802_11_MAXIMUM_LOOKAHEAD;
        uInfoLen = sizeof(uInfo);
        break;

    case OID_GEN_MAXIMUM_FRAME_SIZE:
        uInfo = MP_802_11_MAX_FRAME_SIZE - MP_802_11_SHORT_HEADER_SIZE;
        uInfoLen = sizeof(uInfo);
        break;

    case OID_GEN_CURRENT_PACKET_FILTER:
        ndisStatus = MpQueryPacketFilter(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_GEN_MAXIMUM_TOTAL_SIZE:
    case OID_GEN_TRANSMIT_BLOCK_SIZE:
    case OID_GEN_RECEIVE_BLOCK_SIZE:
        uInfo = (ULONG) MP_802_11_MAX_FRAME_SIZE;
        uInfoLen = sizeof(uInfo);
        break;
        
    case OID_GEN_MAC_OPTIONS:
        uInfo = NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA | 
                 NDIS_MAC_OPTION_TRANSFERS_NOT_PEND |
                 NDIS_MAC_OPTION_NO_LOOPBACK;
        uInfoLen = sizeof(uInfo);
        break;

    case OID_GEN_LINK_SPEED:
        //uInfo = (ULONG)(Hw11GetLinkSpeed(pAdapter->Nic) / 100);    // OID expects 100 bps
        uInfoLen = sizeof(uInfo);
        break;

    case OID_GEN_TRANSMIT_BUFFER_SPACE:
        uInfo = MP_802_11_MAX_FRAME_SIZE * pAdapter->TotalTxMSDUs;
        uInfoLen = sizeof(uInfo);
        break;

    case OID_GEN_RECEIVE_BUFFER_SPACE:
        uInfo = MP_802_11_MAX_FRAME_SIZE * pAdapter->TotalRxMSDU;
        uInfoLen = sizeof(uInfo);
        break;

    case OID_GEN_VENDOR_ID:
        uInfo = MP_VENDOR_ID;
        uInfoLen = sizeof(uInfo);
        break;

    case OID_GEN_VENDOR_DESCRIPTION:
        pInfo = VendorDesc;
        uInfoLen = sizeof(VendorDesc);
        break;
        
    case OID_GEN_VENDOR_DRIVER_VERSION:
        uInfo = MP_VENDOR_ID;
        uInfoLen = sizeof(uInfo);
        break;

    case OID_GEN_DRIVER_VERSION:
        usInfo = (USHORT) MP_NDIS_DRIVER_VERSION;
        pInfo = (PVOID) &usInfo;
        uInfoLen = sizeof(usInfo);
        break;

    case OID_802_3_PERMANENT_ADDRESS:
    case OID_DOT11_PERMANENT_ADDRESS:
        pInfo = pAdapter->PermanentAddress;
        uInfoLen = ETH_LENGTH_OF_ADDRESS;
        break;

    case OID_802_3_CURRENT_ADDRESS:
    case OID_DOT11_CURRENT_ADDRESS:
    case OID_DOT11_MAC_ADDRESS:
        pInfo = pAdapter->CurrentAddress;
        uInfoLen = ETH_LENGTH_OF_ADDRESS;
        break;

    case OID_DOT11_STATION_ID:
        ndisStatus = MpQueryStationId(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_802_3_MAXIMUM_LIST_SIZE:
        uInfo = HW_MAX_MCAST_LIST_SIZE;
        uInfoLen = sizeof(uInfo);
        break;

    case OID_GEN_MEDIA_CONNECT_STATUS:
        //
        // We will always report connected. The nwifi IM driver
        // takes care of the logic of this for the miniport.
        //
        uInfo = NdisMediaStateConnected;
        uInfoLen = sizeof(NDIS_STATUS);
        break;

    case OID_PNP_QUERY_POWER:
        ndisStatus = MpPnPQueryPower(
            pAdapter,
            InformationBuffer,
            InformationBufferLength
            );
        break;

    case OID_PNP_CAPABILITIES:
        ndisStatus = MpQueryPnPCapabilities(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_GEN_XMIT_OK:
    case OID_GEN_RCV_OK:
    case OID_GEN_XMIT_ERROR:
    case OID_GEN_RCV_ERROR:
    case OID_GEN_RCV_NO_BUFFER:
    case OID_GEN_RCV_CRC_ERROR:
        pInfo = &ullInfo64;
        ndisStatus = wf_get_statistics(
            pAdapter,
            Oid,
            &ullInfo64,
            BytesWritten
            );
        uInfoLen = *BytesWritten;
        break;

    case OID_GEN_TRANSMIT_QUEUE_LENGTH:
        uInfo = pAdapter->PendingSends;
        
        uInfoLen = sizeof(uInfo);
        break;

    case OID_GEN_LINK_PARAMETERS:
        ndisStatus = MpQueryLinkParameters(
            pAdapter,
            InformationBuffer,
            InformationBufferLength,
            BytesWritten,
            BytesNeeded
             );
          break;

    default:
        ndisStatus = NDIS_STATUS_INVALID_OID;
        break;
    }


    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        if (uInfoLen <= InformationBufferLength)
        {
            if(uInfoLen)
            {
                // Copy result into InformationBuffer
                *BytesWritten = uInfoLen;
                NdisMoveMemory(InformationBuffer, pInfo, uInfoLen);
            }
            else 
            {
                //
                // the result is either zero length of has been copied
                // by the OID specific handler
                //
                // Dont overwrite the BytesWritten and BytesNeeded
                // as they are also appropriately set
                //
            }
        }
        else
        {
            //
            // The buffer length is not sufficient
            //
            *BytesNeeded = uInfoLen;
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        }
    }
    else
    {
        LOG_D("MPQueryInformation completed! ADAPTER %p, OID 0x%08x %s, ndisStatus = 0x%08x",
        	pAdapter, Oid, DbgGetOidName(Oid), ndisStatus);
    }

//    MpExit;
    return(ndisStatus);
}



NDIS_STATUS
MpSetInformation(
    __in    PADAPTER                  pAdapter,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InformationBufferLength,
    __out   PULONG                    BytesRead,
    __out   PULONG                    BytesNeeded
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;

//    MpEntry;

    *BytesRead = 0;
    *BytesNeeded = 0;

    LOG_D("Setting OID: 0x%08x  %s", Oid, DbgGetOidName(Oid));
   
//    ndisStatus = MpEventSetInformation(
//                    pAdapter,
//                    Oid,
//                    InformationBuffer,
//                    InformationBufferLength,
//                    BytesRead,
//                    BytesNeeded
//                    );
//
//    if (ndisStatus != NDIS_STATUS_INVALID_OID)
//    {
//        //
//        // This Oid has been responded to by the MpEventQueryInformation function
//        // We will not handle this Oid and return to the OS.
//        //
//        LOG_E("Oid intercepted by MpEventSetInformation! Status 0x%08x", ndisStatus);
//        return ndisStatus;
//    }

    //
    // Assume OID succeeds by default. Failure cases will set it as failure.
    //
    ndisStatus = NDIS_STATUS_SUCCESS;

    switch (Oid)
    {
    case OID_PNP_SET_POWER:
        ndisStatus = MpSetPower(
            pAdapter,
            InformationBuffer,
            InformationBufferLength,
            BytesNeeded,
            BytesRead
            );
        break;

    case OID_802_3_MULTICAST_LIST:
        ndisStatus = MpSetMulticastList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_GEN_CURRENT_LOOKAHEAD:
        if (InformationBufferLength < sizeof (ULONG))
        {
            *BytesNeeded = sizeof(ULONG);
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            break;
        }

        NdisMoveMemory(&pAdapter->LookAhead, InformationBuffer, sizeof (ULONG));
        *BytesRead = sizeof(ULONG);
        break;

    case OID_GEN_CURRENT_PACKET_FILTER:
        if (InformationBufferLength != sizeof(ULONG))
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = sizeof(ULONG);
            break;
        }

        {
            ULONG PacketFilter;
            NdisMoveMemory(&PacketFilter, InformationBuffer, sizeof(ULONG));
            *BytesRead = sizeof(ULONG);

            ndisStatus = MpSetPacketFilter(
                                    pAdapter,
                                    PacketFilter
                                    );
        }

        break;

    case OID_GEN_LINK_PARAMETERS:
        ndisStatus = MpSetLinkParameters(
            pAdapter,
            InformationBuffer,
            InformationBufferLength,
            BytesRead,
            BytesNeeded
            );
        break;
        

    case OID_DOT11_CURRENT_OPERATION_MODE:
        ndisStatus = MpSetCurrentOperationMode(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ATIM_WINDOW:
        ndisStatus = MpSetATIMWindow(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_SCAN_REQUEST:
        ndisStatus = MpScanRequest(
                        pAdapter,
                        InformationBuffer,
                        InformationBufferLength,
                        BytesRead,
                        BytesNeeded
                        );
        break;

    case OID_DOT11_NIC_POWER_STATE:
        ndisStatus = MpSetNicPowerState(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_OPERATIONAL_RATE_SET:
        ndisStatus = MpSetOperationalRateSet(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_BEACON_PERIOD:
        ndisStatus = MpSetBeaconPeriod(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_RTS_THRESHOLD:
        ndisStatus = MpSetRTSThreshold(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_FRAGMENTATION_THRESHOLD:
        ndisStatus = MpSetFragmentationThreshold(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CURRENT_REG_DOMAIN:
        ndisStatus = MpSetCurrentRegDomain(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    //
    // This hardware is DSSS PHY. Other PHYs will have to support
    // a different set of OIDs. Refer to the documentation.
    //
    case OID_DOT11_CURRENT_CHANNEL:
        ndisStatus = MpSetCurrentChannel(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    //
    // For OFDM PHY
    //
    case OID_DOT11_CURRENT_FREQUENCY:
        ndisStatus = MpSetCurrentFrequency(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_MULTI_DOMAIN_CAPABILITY_ENABLED:
    case OID_DOT11_COUNTRY_STRING:
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        break;
    
    case OID_DOT11_AUTO_CONFIG_ENABLED:
        ndisStatus = MpSetAutoConfigOption(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );        
        break;

    case OID_DOT11_FLUSH_BSS_LIST:
        ndisStatus = MpFlushBSSList(
                    pAdapter
                    );
        break;

    case OID_DOT11_POWER_MGMT_REQUEST:
        ndisStatus = MpSetPowerMgmtRequest(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DESIRED_SSID_LIST:
        ndisStatus = MpSetDesiredSSIDList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST:
        ndisStatus = MpSetExcludedMACAddressList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_EXCLUDE_UNENCRYPTED:
        ndisStatus = MpSetExcludeUnencrypted(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DESIRED_BSSID_LIST:
        ndisStatus = MpSetDesiredBSSIDList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DESIRED_BSS_TYPE:
        ndisStatus = MpSetDesiredBSSType(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CONNECT_REQUEST:
        ndisStatus = MpConnectRequest(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM:
        ndisStatus = MpSetEnabledAuthenticationAlgorithm(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ENABLED_UNICAST_CIPHER_ALGORITHM:
        ndisStatus = MpSetEnabledUnicastCipherAlgorithm(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ENABLED_MULTICAST_CIPHER_ALGORITHM:
        ndisStatus = MpSetEnabledMulticastCipherAlgorithm(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CIPHER_DEFAULT_KEY_ID:
        ndisStatus = MpSetCipherDefaultKeyID(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CIPHER_DEFAULT_KEY:
        ndisStatus = MpSetCipherDefaultKey(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CIPHER_KEY_MAPPING_KEY:
        ndisStatus = MpSetCipherKeyMappingKey(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DISCONNECT_REQUEST:
        ndisStatus = MpDisconnectRequest(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_DESIRED_PHY_LIST:
        ndisStatus = MpSetDesiredPhyList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_CURRENT_PHY_ID:
        ndisStatus = MpSetCurrentPhyId(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_PMKID_LIST:
        ndisStatus = MpSetPMKIDList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_MEDIA_STREAMING_ENABLED:
        ndisStatus = MpSetMediaStreamingOption(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_UNREACHABLE_DETECTION_THRESHOLD:
        ndisStatus = MpSetUnreachableDetectionThreshold(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_PRIVACY_EXEMPTION_LIST:
        ndisStatus = MpSetPrivacyExemptionList(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_IBSS_PARAMS:
        ndisStatus = MpSetIBSSParameters(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;
        
    case OID_DOT11_UNICAST_USE_GROUP_ENABLED:
        ndisStatus = MpSetUnicastUseGroupOption(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_SAFE_MODE_ENABLED:
        ndisStatus = MpSetSafeModeOption(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_HIDDEN_NETWORK_ENABLED:
        ndisStatus = MpSetHiddenNetworkOption(
                    pAdapter,
                    InformationBuffer,
                    InformationBufferLength,
                    BytesRead,
                    BytesNeeded
                    );
        break;

    default:
        ndisStatus = NDIS_STATUS_INVALID_OID;
        break;    
    }

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        LOG_E("SetInformation oid  0x%08x %s returned: 0x%08x", Oid, DbgGetOidName(Oid), ndisStatus);
    }

//    MpExit;
    return ndisStatus;
}


NDIS_STATUS
MpQuerySetInformation(
    __in    PADAPTER                  pAdapter,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InputBufferLength,
    __in    ULONG                     OutputBufferLength,
    __in    ULONG                     MethodId,
    __out   PULONG                    BytesWritten,
    __out   PULONG                    BytesRead,
    __out   PULONG                    BytesNeeded
    )
{
    NDIS_STATUS     ndisStatus;

	UNREFERENCED_PARAMETER(MethodId);

#if 1
	if(Oid != OID_DOT11_ENUM_BSS_LIST) {
    	LOG_D("Querying/Setting OID: 0x%08x %s", Oid, DbgGetOidName(Oid));
	}
#else
	LOG_D("Querying/Setting OID: 0x%08x %s", Oid, DbgGetOidName(Oid));
#endif
    
//    ndisStatus = MpEventQuerySetInformation(
//                    pAdapter,
//                    Oid,
//                    InformationBuffer,
//                    InputBufferLength,
//                    OutputBufferLength,
//                    MethodId,
//                    BytesWritten,
//                    BytesRead,
//                    BytesNeeded
//                    );
//
//    if (ndisStatus != NDIS_STATUS_INVALID_OID)
//    {
//        //
//        // This Oid has been responded to by the MpEventQueryInformation function
//        // We will not handle this Oid and return to the OS.
//        //
//        LOG_E("Oid intercepted by MpEventQuerySetInformation! Status 0x%08x", ndisStatus);
//        return ndisStatus;
//    }

    //
    // Assume OID succeeds by default. Failure cases will set it as failure.
    //
    ndisStatus = NDIS_STATUS_SUCCESS;
    *BytesWritten = 0;
    *BytesRead = 0;
    
    switch(Oid)
    {

    case OID_DOT11_RESET_REQUEST:
        ndisStatus = MpResetRequest(
                    pAdapter,
                    InformationBuffer,
                    InputBufferLength,
                    OutputBufferLength,
                    BytesRead,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_ENUM_BSS_LIST:
        ndisStatus = MpEnumerateBSSList(
                    pAdapter,
                    InformationBuffer,
                    InputBufferLength,
                    OutputBufferLength,
                    BytesWritten,
                    BytesNeeded
                    );
        break;

    case OID_DOT11_RECV_SENSITIVITY_LIST:
        ndisStatus = MpQueryRecvSensitivityList(
                        pAdapter,
                        InformationBuffer,
                        InputBufferLength,
                        OutputBufferLength,
                        BytesWritten,
                        BytesNeeded
                        );

        break;

    default:
            ndisStatus = NDIS_STATUS_INVALID_OID;
            break;
    }

    return ndisStatus;
}

PCHAR 
DbgGetOidName(
    ULONG Oid
    )
{
    PCHAR oidName;

    switch (Oid){

        #undef MAKECASE
        #define MAKECASE(oidx) case oidx: oidName = #oidx; break;
       
        MAKECASE(OID_GEN_SUPPORTED_LIST)
        MAKECASE(OID_GEN_HARDWARE_STATUS)
        MAKECASE(OID_GEN_MEDIA_SUPPORTED)
        MAKECASE(OID_GEN_MEDIA_IN_USE)
        MAKECASE(OID_GEN_MAXIMUM_LOOKAHEAD)
        MAKECASE(OID_GEN_MAXIMUM_FRAME_SIZE)
        MAKECASE(OID_GEN_LINK_SPEED)
        MAKECASE(OID_GEN_TRANSMIT_BUFFER_SPACE)
        MAKECASE(OID_GEN_RECEIVE_BUFFER_SPACE)
        MAKECASE(OID_GEN_TRANSMIT_BLOCK_SIZE)
        MAKECASE(OID_GEN_RECEIVE_BLOCK_SIZE)
        MAKECASE(OID_GEN_VENDOR_ID)
        MAKECASE(OID_GEN_VENDOR_DESCRIPTION)
        MAKECASE(OID_GEN_CURRENT_PACKET_FILTER)
        MAKECASE(OID_GEN_CURRENT_LOOKAHEAD)
        MAKECASE(OID_GEN_DRIVER_VERSION)
        MAKECASE(OID_GEN_MAXIMUM_TOTAL_SIZE)
        MAKECASE(OID_GEN_PROTOCOL_OPTIONS)
        MAKECASE(OID_GEN_MAC_OPTIONS)
        MAKECASE(OID_GEN_MEDIA_CONNECT_STATUS)
        MAKECASE(OID_GEN_MAXIMUM_SEND_PACKETS)
        MAKECASE(OID_GEN_VENDOR_DRIVER_VERSION)
        MAKECASE(OID_GEN_SUPPORTED_GUIDS)
        MAKECASE(OID_GEN_NETWORK_LAYER_ADDRESSES)
        MAKECASE(OID_GEN_TRANSPORT_HEADER_OFFSET)
        MAKECASE(OID_GEN_MEDIA_CAPABILITIES)
        MAKECASE(OID_GEN_PHYSICAL_MEDIUM)
        MAKECASE(OID_GEN_XMIT_OK)
        MAKECASE(OID_GEN_RCV_OK)
        MAKECASE(OID_GEN_XMIT_ERROR)
        MAKECASE(OID_GEN_RCV_ERROR)
        MAKECASE(OID_GEN_RCV_NO_BUFFER)
        MAKECASE(OID_GEN_RCV_CRC_ERROR)
        MAKECASE(OID_GEN_TRANSMIT_QUEUE_LENGTH)
        MAKECASE(OID_802_3_PERMANENT_ADDRESS)
        MAKECASE(OID_DOT11_PERMANENT_ADDRESS)
        MAKECASE(OID_802_3_CURRENT_ADDRESS)
        MAKECASE(OID_DOT11_CURRENT_ADDRESS)
        MAKECASE(OID_802_3_MULTICAST_LIST)
        MAKECASE(OID_DOT11_MULTICAST_LIST)
        MAKECASE(OID_802_3_MAXIMUM_LIST_SIZE)
        MAKECASE(OID_PNP_SET_POWER)
        MAKECASE(OID_PNP_QUERY_POWER)
        // For Native Wi-Fi drivers these OIDs are handled by the Native Wi-Fi filter
        /*
        OID_GEN_STATISTICS,
        */

        // Miniport does not support Wakeup today
        /*
        OID_PNP_CAPABILITIES,
        OID_PNP_ADD_WAKE_UP_PATTERN,
        OID_PNP_REMOVE_WAKE_UP_PATTERN,
        OID_PNP_ENABLE_WAKE_UP,
        */
        MAKECASE(OID_DOT11_MPDU_MAX_LENGTH)
        MAKECASE(OID_DOT11_OPERATION_MODE_CAPABILITY)
        MAKECASE(OID_DOT11_CURRENT_OPERATION_MODE)
        MAKECASE(OID_DOT11_ATIM_WINDOW)
        MAKECASE(OID_DOT11_SCAN_REQUEST)
        MAKECASE(OID_DOT11_RESET_REQUEST)
        MAKECASE(OID_DOT11_NIC_POWER_STATE)
        MAKECASE(OID_DOT11_OPTIONAL_CAPABILITY)
        MAKECASE(OID_DOT11_STATION_ID)
        MAKECASE(OID_DOT11_CF_POLLABLE)
        MAKECASE(OID_DOT11_OPERATIONAL_RATE_SET)
        MAKECASE(OID_DOT11_BEACON_PERIOD)
        MAKECASE(OID_DOT11_MAC_ADDRESS)
        MAKECASE(OID_DOT11_RTS_THRESHOLD)
        MAKECASE(OID_DOT11_SHORT_RETRY_LIMIT)
        MAKECASE(OID_DOT11_LONG_RETRY_LIMIT)
        MAKECASE(OID_DOT11_FRAGMENTATION_THRESHOLD)
        MAKECASE(OID_DOT11_MAX_TRANSMIT_MSDU_LIFETIME)
        MAKECASE(OID_DOT11_MAX_RECEIVE_LIFETIME)
        MAKECASE(OID_DOT11_SUPPORTED_PHY_TYPES)
        MAKECASE(OID_DOT11_CURRENT_REG_DOMAIN)
        MAKECASE(OID_DOT11_TEMP_TYPE)
        MAKECASE(OID_DOT11_DIVERSITY_SUPPORT)
        MAKECASE(OID_DOT11_SUPPORTED_POWER_LEVELS)
        MAKECASE(OID_DOT11_CURRENT_TX_POWER_LEVEL)
        MAKECASE(OID_DOT11_CURRENT_CHANNEL)
        MAKECASE(OID_DOT11_CCA_MODE_SUPPORTED)
        MAKECASE(OID_DOT11_CURRENT_CCA_MODE)
        MAKECASE(OID_DOT11_ED_THRESHOLD)
        MAKECASE(OID_DOT11_REG_DOMAINS_SUPPORT_VALUE)
        MAKECASE(OID_DOT11_SUPPORTED_TX_ANTENNA)
        MAKECASE(OID_DOT11_SUPPORTED_RX_ANTENNA)
        MAKECASE(OID_DOT11_DIVERSITY_SELECTION_RX)
        MAKECASE(OID_DOT11_SUPPORTED_DATA_RATES_VALUE)
        MAKECASE(OID_DOT11_RF_USAGE)
        MAKECASE(OID_DOT11_RECV_SENSITIVITY_LIST)
        MAKECASE(OID_DOT11_MULTI_DOMAIN_CAPABILITY_IMPLEMENTED)
        MAKECASE(OID_DOT11_MULTI_DOMAIN_CAPABILITY_ENABLED)
        MAKECASE(OID_DOT11_COUNTRY_STRING)
        MAKECASE(OID_DOT11_CURRENT_FREQUENCY)
        MAKECASE(OID_DOT11_FREQUENCY_BANDS_SUPPORTED)
        MAKECASE(OID_DOT11_SHORT_PREAMBLE_OPTION_IMPLEMENTED)
        MAKECASE(OID_DOT11_PBCC_OPTION_IMPLEMENTED)
        MAKECASE(OID_DOT11_ERP_PBCC_OPTION_ENABLED)
        MAKECASE(OID_DOT11_DSSS_OFDM_OPTION_IMPLEMENTED)
        MAKECASE(OID_DOT11_DSSS_OFDM_OPTION_ENABLED)
        MAKECASE(OID_DOT11_SHORT_SLOT_TIME_OPTION_IMPLEMENTED)
        MAKECASE(OID_DOT11_SHORT_SLOT_TIME_OPTION_ENABLED)
        MAKECASE(OID_DOT11_ENUM_BSS_LIST)
        MAKECASE(OID_DOT11_FLUSH_BSS_LIST)
        MAKECASE(OID_DOT11_POWER_MGMT_REQUEST)
        MAKECASE(OID_DOT11_DESIRED_SSID_LIST)
        MAKECASE(OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST)
        MAKECASE(OID_DOT11_EXCLUDE_UNENCRYPTED)
        MAKECASE(OID_DOT11_DESIRED_BSSID_LIST)
        MAKECASE(OID_DOT11_DESIRED_BSS_TYPE)
        MAKECASE(OID_DOT11_CONNECT_REQUEST)
        MAKECASE(OID_DOT11_STATISTICS)
        MAKECASE(OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM)
        MAKECASE(OID_DOT11_SUPPORTED_UNICAST_ALGORITHM_PAIR)
        MAKECASE(OID_DOT11_SUPPORTED_MULTICAST_ALGORITHM_PAIR)
        MAKECASE(OID_DOT11_ENABLED_UNICAST_CIPHER_ALGORITHM)
        MAKECASE(OID_DOT11_ENABLED_MULTICAST_CIPHER_ALGORITHM)
        MAKECASE(OID_DOT11_CIPHER_DEFAULT_KEY_ID)
        MAKECASE(OID_DOT11_CIPHER_DEFAULT_KEY)
        MAKECASE(OID_DOT11_CIPHER_KEY_MAPPING_KEY)
        MAKECASE(OID_DOT11_ENUM_ASSOCIATION_INFO)
        MAKECASE(OID_DOT11_DISCONNECT_REQUEST)
        MAKECASE(OID_DOT11_HARDWARE_PHY_STATE)
        MAKECASE(OID_DOT11_DESIRED_PHY_LIST)
        MAKECASE(OID_DOT11_CURRENT_PHY_ID)
        MAKECASE(OID_DOT11_MEDIA_STREAMING_ENABLED)
        MAKECASE(OID_DOT11_UNREACHABLE_DETECTION_THRESHOLD)
        MAKECASE(OID_DOT11_ACTIVE_PHY_LIST)
        MAKECASE(OID_DOT11_EXTSTA_CAPABILITY)
        MAKECASE(OID_DOT11_DATA_RATE_MAPPING_TABLE)
        MAKECASE(OID_DOT11_PRIVACY_EXEMPTION_LIST)
        MAKECASE(OID_DOT11_IBSS_PARAMS)
        MAKECASE(OID_DOT11_PMKID_LIST)
        MAKECASE(OID_DOT11_AUTO_CONFIG_ENABLED)
        MAKECASE(OID_DOT11_UNICAST_USE_GROUP_ENABLED)
        MAKECASE(OID_DOT11_SAFE_MODE_ENABLED)
        MAKECASE(OID_DOT11_HIDDEN_NETWORK_ENABLED)
       
        MAKECASE(OID_GEN_DIRECTED_BYTES_XMIT)
        MAKECASE(OID_GEN_DIRECTED_FRAMES_XMIT)
        MAKECASE(OID_GEN_MULTICAST_BYTES_XMIT)
        MAKECASE(OID_GEN_MULTICAST_FRAMES_XMIT)
        MAKECASE(OID_GEN_BROADCAST_BYTES_XMIT)
        MAKECASE(OID_GEN_BROADCAST_FRAMES_XMIT)
        MAKECASE(OID_GEN_DIRECTED_BYTES_RCV)
        MAKECASE(OID_GEN_DIRECTED_FRAMES_RCV)
        MAKECASE(OID_GEN_MULTICAST_BYTES_RCV)
        MAKECASE(OID_GEN_MULTICAST_FRAMES_RCV)
        MAKECASE(OID_GEN_BROADCAST_BYTES_RCV)
        MAKECASE(OID_GEN_BROADCAST_FRAMES_RCV)        
        MAKECASE(OID_GEN_GET_TIME_CAPS)
        MAKECASE(OID_GEN_GET_NETCARD_TIME)
        MAKECASE(OID_GEN_NETCARD_LOAD)
        MAKECASE(OID_GEN_DEVICE_PROFILE)
        MAKECASE(OID_GEN_INIT_TIME_MS)
        MAKECASE(OID_GEN_RESET_COUNTS)
        MAKECASE(OID_GEN_MEDIA_SENSE_COUNTS)
        MAKECASE(OID_PNP_CAPABILITIES)
        MAKECASE(OID_PNP_ADD_WAKE_UP_PATTERN)
        MAKECASE(OID_PNP_REMOVE_WAKE_UP_PATTERN)
        MAKECASE(OID_PNP_ENABLE_WAKE_UP)
        MAKECASE(OID_802_3_MAC_OPTIONS)
        MAKECASE(OID_802_3_RCV_ERROR_ALIGNMENT)
        MAKECASE(OID_802_3_XMIT_ONE_COLLISION)
        MAKECASE(OID_802_3_XMIT_MORE_COLLISIONS)
        MAKECASE(OID_802_3_XMIT_DEFERRED)
        MAKECASE(OID_802_3_XMIT_MAX_COLLISIONS)
        MAKECASE(OID_802_3_RCV_OVERRUN)
        MAKECASE(OID_802_3_XMIT_UNDERRUN)
        MAKECASE(OID_802_3_XMIT_HEARTBEAT_FAILURE)
        MAKECASE(OID_802_3_XMIT_TIMES_CRS_LOST)
        MAKECASE(OID_802_3_XMIT_LATE_COLLISIONS)

        default: 
            oidName = "<** UNKNOWN OID **>";
            break;
    }

    return oidName;
}


NDIS_STATUS wf_mp_oids_init(PADAPTER	   pAdapter)
{
	wf_u32 size;
	wf_mib_info_t *mib_info;

	mib_info = wf_malloc(sizeof(wf_mib_info_t));
	if(mib_info == NULL) {
		LOG_E("malloc mib info failed!");
		goto ERROR_DEAL;
	}
	wf_memset(mib_info, 0, sizeof(wf_mib_info_t));
	pAdapter->mib_info = mib_info;

	size = sizeof(DOT11_SUPPORTED_PHY_TYPES) + (MAX_NUM_PHY_TYPES - 1) * sizeof(DOT11_PHY_TYPE);
	mib_info->pSupportedPhyTypes = wf_malloc(size);
	if (mib_info->pSupportedPhyTypes == NULL) {
		LOG_E("malloc support phy types failed");
		goto ERROR_DEAL;
	}
	wf_memset(mib_info->pSupportedPhyTypes, 0, size);

	size = sizeof(DOT11_REG_DOMAINS_SUPPORT_VALUE) +
		(MAX_NUM_DOT11_REG_DOMAINS_VALUE - 1) * sizeof(DOT11_REG_DOMAIN_VALUE);
	mib_info->pRegDomainsSupportValue = wf_malloc(size);
	if (mib_info->pRegDomainsSupportValue == NULL) {
		LOG_E("malloc reg domain support value failed");
		goto ERROR_DEAL;
	}
	wf_memset(mib_info->pRegDomainsSupportValue, 0, size);

	size = sizeof(DOT11_DIVERSITY_SELECTION_RX_LIST) +
		(MAX_NUM_DIVERSITY_SELECTION_RX_LIST - 1) * sizeof(DOT11_DIVERSITY_SELECTION_RX);
	mib_info->pDiversitySelectionRxList = wf_malloc(size);
	if (mib_info->pDiversitySelectionRxList == NULL) {
		LOG_E("malloc diversity selection rx list failed");
		goto ERROR_DEAL;
	}
	wf_memset(mib_info->pDiversitySelectionRxList, 0, size);

	wf_oids_adapt_init(pAdapter);

	return NDIS_STATUS_SUCCESS;

ERROR_DEAL:
	
	wf_mp_oids_deinit(pAdapter);
	return NDIS_STATUS_FAILURE;
}

void wf_mp_oids_deinit(PADAPTER	  pAdapter)
{
	wf_mib_info_t *mib_info = pAdapter->mib_info;

	if(pAdapter->mib_info != NULL) {
		if (mib_info->pDiversitySelectionRxList != NULL) {
			wf_free(mib_info->pDiversitySelectionRxList);
			mib_info->pDiversitySelectionRxList = NULL;
		}
		
		if (mib_info->pRegDomainsSupportValue != NULL) {
			wf_free(mib_info->pRegDomainsSupportValue);
			mib_info->pRegDomainsSupportValue = NULL;
		}
		
		if (mib_info->pSupportedPhyTypes != NULL) {
			wf_free(mib_info->pSupportedPhyTypes);
			mib_info->pSupportedPhyTypes = NULL;
		}

		wf_oids_adapt_deinit(pAdapter);
		
		wf_free(pAdapter->mib_info);
		pAdapter->mib_info = NULL;
	}
}



