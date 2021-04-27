/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

   Mp_main.C

Abstract:

    This sample provides an example of minimal driver intended for education
    purposes. Neither the driver or its sample test programs are intended
    for use in a production environment.

Author:  Vishal Manan (Oct 10, 2005)

Revision History:

    Converted the RealText PCI driver to USB 

Notes:

--*/
#include "pcomp.h"
#include "common.h"
#include "wf_debug.h"
#include "tx_windows.h"
#include "rx_windows.h"
#include "wf_oids_adapt.h"
#include "hw_init.h"
#if DOT11_TRACE_ENABLED
#include "Mp_Main.tmh"
#endif


//
// Functions that are not tagged PAGEABLE by the pragma as shown below are
// by default resident (NONPAGEABLE) in memory. Make sure you don't acquire
// spinlock or raise the IRQL in a pageable function. It's okay to call
// another nonpageable function that runs at DISPATCH_LEVEL from a
// pageable function.
//
#pragma NDIS_INIT_FUNCTION(DriverEntry)
#pragma NDIS_PAGEABLE_FUNCTION(MPInitialize)
#pragma NDIS_PAGEABLE_FUNCTION(MPHalt)
#pragma NDIS_PAGEABLE_FUNCTION(DriverUnload)

NDIS_HANDLE     NdisMiniportDriverHandle = NULL;
NDIS_HANDLE     GlobalDriverContext = NULL;

MP_REG_ENTRY MPRegTable[] = {
// reg value name                           Offset in ADAPTER               Field size                   Default Value          Min                 Max
    {NDIS_STRING_CONST("*ReceiveBuffers"),  MP_OFFSET(TotalRxMSDU),         MP_SIZE(TotalRxMSDU),       HW_DEF_RX_MSDUS,  HW_MIN_RX_MSDUS,  HW_MAX_RX_MSDUS},
    {NDIS_STRING_CONST("MinRxBuffers"),     MP_OFFSET(MinRxMSDU),           MP_SIZE(MinRxMSDU),         HW_MIN_RX_MSDUS,  HW_MIN_RX_MSDUS,  HW_MIN_RX_MSDUS},
    {NDIS_STRING_CONST("MaxRxBuffers"),     MP_OFFSET(MaxRxMSDU),           MP_SIZE(MaxRxMSDU),         HW_MAX_RX_MSDUS,  HW_MAX_RX_MSDUS,  HW_MAX_RX_MSDUS},
    {NDIS_STRING_CONST("*TransmitBuffers"), MP_OFFSET(TotalTxMSDUs),        MP_SIZE(TotalTxMSDUs),      HW_DEF_TX_MSDUS,  HW_MIN_TX_MSDUS,  HW_MAX_TX_MSDUS},
};

#define MP_NUM_REG_PARAMS (sizeof (MPRegTable) / sizeof(MP_REG_ENTRY))



NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
    )
/*++
Routine Description:

    In the context of its DriverEntry function, a miniport driver associates
    itself with NDIS, specifies the NDIS version that it is using, and
    registers its entry points.


Arguments:
    PVOID DriverObject - pointer to the driver object.
    PVOID RegistryPath - pointer to the driver registry path.

    Return Value:

    NDIS_STATUS_xxx code

--*/
{

    NDIS_STATUS                             Status;
    NDIS_MINIPORT_DRIVER_CHARACTERISTICS    MPChar;
    WDF_DRIVER_CONFIG                       config;
    NTSTATUS                                ntStatus;
    WDFDRIVER                       hDriver; //vm control
    

    #if DOT11_TRACE_ENABLED
        WPP_INIT_TRACING(DriverObject, RegistryPath);
    #endif
	LOG_D("---> DriverEntry");
    LOG_D("---> WifiUSB WDF sample built on "__DATE__" at "__TIME__);
    LOG_D("---> Sample is built with NDIS Version %d.%d",
                            MP_MAJOR_NDIS_VERSION, MP_MINOR_NDIS_VERSION);

    //
    // Make sure we are compatible with the version of NDIS supported by OS.
    //
    if(NdisGetVersion() < ((MP_MAJOR_NDIS_VERSION << 16) | MP_MINOR_NDIS_VERSION)){
        LOG_E("This version of driver is not support on this OS");
        return NDIS_STATUS_FAILURE;
    }


    WDF_DRIVER_CONFIG_INIT(&config, WDF_NO_EVENT_CALLBACK);
    //
    // Set WdfDriverInitNoDispatchOverride flag to tell the framework
    // not to provide dispatch routines for the driver. In other words,
    // the framework must not intercept IRPs that the I/O manager has
    // directed to the driver. In this case, it will be handled by NDIS
    // port driver.
    //
    config.DriverInitFlags |= WdfDriverInitNoDispatchOverride;

    ntStatus = WdfDriverCreate(DriverObject,
                               RegistryPath,
                               WDF_NO_OBJECT_ATTRIBUTES,
                               &config,                
                               &hDriver); //vm control
    if(!NT_SUCCESS(ntStatus)){
        LOG_E("WdfDriverCreate failed");
        return NDIS_STATUS_FAILURE;
    }
    
    //
    // Fill in the Miniport characteristics structure with the version numbers
    // and the entry points for driver-supplied MiniportXxx
    //
    NdisZeroMemory(&MPChar, sizeof(MPChar));


    MPChar.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_DRIVER_CHARACTERISTICS,
    MPChar.Header.Size = sizeof(NDIS_MINIPORT_DRIVER_CHARACTERISTICS);
    MPChar.Header.Revision = NDIS_MINIPORT_DRIVER_CHARACTERISTICS_REVISION_1;
    MPChar.MajorNdisVersion = MP_MAJOR_NDIS_VERSION;
    MPChar.MinorNdisVersion = MP_MINOR_NDIS_VERSION;
    MPChar.MajorDriverVersion           = HW_MAJOR_DRIVER_VERSION;
    MPChar.MinorDriverVersion           = HW_MINOR_DRIVER_VERSION;

    //
    // Init/PnP handlers
    //
    MPChar.InitializeHandlerEx      = MPInitialize;
    MPChar.RestartHandler           = MPRestart;
    MPChar.PauseHandler             = MPPause;

    MPChar.ShutdownHandlerEx        = MPAdapterShutdown; 
    MPChar.DevicePnPEventNotifyHandler  = MPDevicePnPEvent;
    MPChar.HaltHandlerEx            = MPHalt;
    MPChar.UnloadHandler            = DriverUnload;
    
    //
    // Query/Set/Method requests handlers
    //
    MPChar.OidRequestHandler        = MPRequest;
    MPChar.CancelOidRequestHandler  = MPCancelRequest;

    //
    // Set optional miniport services handler
    //
    MPChar.SetOptionsHandler        = MPSetOptions;

    //
    // Send/Receive handlers
    //
	MPChar.SendNetBufferListsHandler =  MPSendNetBufferLists;
    MPChar.CancelSendHandler            = MPCancelSendNetBufferLists;
    MPChar.ReturnNetBufferListsHandler  = MPReturnNetBufferLists;
    
    //
    // Fault handling handlers
    //
    //MPChar.CheckForHangHandlerEx        = MPCheckForHang;
    MPChar.ResetHandlerEx               = MPReset;
    MPChar.Flags = NDIS_WDM_DRIVER;      

    LOG_D("Calling NdisMRegisterMiniportDriver...");

    //
    // Registers miniport's entry points with the NDIS library as the first
    // step in NIC driver initialization. The NDIS will call the
    // MiniportInitialize when the device is actually started by the PNP
    // manager.
    //
    Status = NdisMRegisterMiniportDriver(DriverObject,
                                         RegistryPath,
                                         (PNDIS_HANDLE)GlobalDriverContext,
                                         &MPChar,
                                         &NdisMiniportDriverHandle);

    if (Status != NDIS_STATUS_SUCCESS) {
        LOG_E("NdisMRegisterMiniportDriver Status = 0x%08x", Status);
        #if DOT11_TRACE_ENABLED
            WPP_CLEANUP(DriverObject);
        #endif
    }
    
    LOG_D("<--- DriverEntry");
    return Status;

}

INLINE NDIS_STATUS
MpSet80211Attributes(
    __in  PADAPTER                        pAdapter
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES  attr;

    __try
    {
        NdisZeroMemory(&attr, sizeof(NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES));

        attr.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES;
        attr.Header.Revision = NDIS_MINIPORT_ADAPTER_802_11_ATTRIBUTES_REVISION_2;
        attr.Header.Size = NDIS_SIZEOF_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES_REVISION_2;
        attr.OpModeCapability = (DOT11_OPERATION_MODE_EXTENSIBLE_STATION |
                                 DOT11_OPERATION_MODE_NETWORK_MONITOR);   
	

		SetNic80211Attributes(pAdapter, &attr);
		SetSta80211Attributes(pAdapter, &attr);

        //
        // Register the 802.11 miniport attributes with NDIS
        //
		// TODO: Make sure all the attributes are set correctly before sending to NDIS,
		// Or else the system will crash.     2021/03/12
		// Essential!!
		
		ndisStatus = NdisMSetMiniportAttributes(
            pAdapter->MiniportAdapterHandle,
            (PNDIS_MINIPORT_ADAPTER_ATTRIBUTES)&attr
            );
			

    }
    __finally
    {
        //
        // Clean up the attribute structure.
        //
		if (attr.SupportedPhyAttributes)
		{
			MP_FREE_MEMORY(attr.SupportedPhyAttributes);
		}
        
        if (attr.ExtSTAAttributes)
        {
			if (attr.ExtSTAAttributes->pSupportedCountryOrRegionStrings)
			{
			    MP_FREE_MEMORY(attr.ExtSTAAttributes->pSupportedCountryOrRegionStrings);
			}
			if (attr.ExtSTAAttributes->pInfraSupportedUcastAlgoPairs)
			{
			    MP_FREE_MEMORY(attr.ExtSTAAttributes->pInfraSupportedUcastAlgoPairs);
			}

			if (attr.ExtSTAAttributes->pInfraSupportedMcastAlgoPairs)
			{
			    MP_FREE_MEMORY(attr.ExtSTAAttributes->pInfraSupportedMcastAlgoPairs);
			}

			if (attr.ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs)
			{
			    MP_FREE_MEMORY(attr.ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs);
			}

			if (attr.ExtSTAAttributes->pAdhocSupportedMcastAlgoPairs)
			{
			    MP_FREE_MEMORY(attr.ExtSTAAttributes->pAdhocSupportedMcastAlgoPairs);
			}

			MP_FREE_MEMORY(attr.ExtSTAAttributes);

	    }

    }

    return ndisStatus;

}


/**
 * This function Deallocates an adapter object, returning all the resources back to
 * the system. Any allocations that are made for Adapter, must be deallocated from
 * here
 * 
 * \param pAdapter The adapter object to deallocate
 * \sa MpAllocateAdapter
 */
VOID
MpFreeAdapter(
    __in __drv_freesMem(Pool) PADAPTER                   pAdapter
    )
{
    //
    // Free the resources allocated for station information
    // 
    if(pAdapter->nic_info != NULL) {
		wf_free(pAdapter->nic_info);
		pAdapter->nic_info = NULL;
	}

	if (pAdapter->AdapterFwInfo.fw0_usb != NULL) {
		wf_free(pAdapter->AdapterFwInfo.fw0_usb);
		pAdapter->AdapterFwInfo.fw0_usb = NULL;
	}

    if(pAdapter->NdisResetWorkItem)
    {  
        WdfObjectDelete(pAdapter->NdisResetWorkItem);
        pAdapter->NdisResetWorkItem = NULL;
    }

    if (pAdapter->Dot11ResetWorkItem)
    {  
        WdfObjectDelete(pAdapter->Dot11ResetWorkItem);
        pAdapter->Dot11ResetWorkItem = NULL;
    }

    //
    // Free the Packet queue
    //
    MpDeinitNBLQueue(&pAdapter->TxQueue);

    if (pAdapter->WdfDevice){
        WdfObjectDelete(pAdapter->WdfDevice);
    }
	// Free the NIC_INFO resources.
	if (pAdapter->MibInfo.pDiversitySelectionRxList != NULL)
		MP_FREE_MEMORY(pAdapter->MibInfo.pDiversitySelectionRxList);
	if (pAdapter->MibInfo.pRegDomainsSupportValue != NULL)
		MP_FREE_MEMORY(pAdapter->MibInfo.pRegDomainsSupportValue);
	if (pAdapter->MibInfo.pSupportedPhyTypes != NULL)
		MP_FREE_MEMORY(pAdapter->MibInfo.pSupportedPhyTypes);

    MP_FREE_MEMORY(pAdapter);
}

NDIS_STATUS
MpAllocateAdapterWorkItem(
    PADAPTER pAdapter
    )
{
    WDF_OBJECT_ATTRIBUTES   attributes;
    WDF_WORKITEM_CONFIG     workitemConfig;    
    NTSTATUS                ntStatus;
    NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
    PADAPTER_WORKITEM_CONTEXT   adapterWorkitemContext;
    //
    // Allocate the work item (MpDot11ResetWorkItem)
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = pAdapter->WdfDevice;
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&attributes, ADAPTER_WORKITEM_CONTEXT);

    WDF_WORKITEM_CONFIG_INIT(&workitemConfig, MpDot11ResetWorkItem);
    
    ntStatus = WdfWorkItemCreate(&workitemConfig,
                                &attributes,
                                &pAdapter->Dot11ResetWorkItem);

    if (!NT_SUCCESS(ntStatus))
    {
        
        LOG_D("Failed to allocate Dot11 Reset Workitem ");
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
        goto out;
    } 

    adapterWorkitemContext = GetAdapterWorkItemContext(pAdapter->Dot11ResetWorkItem);
    adapterWorkitemContext->Adapter = pAdapter;
    //
    // Specify the context type for the WDF workitem object.
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, ADAPTER_WORKITEM_CONTEXT);
    attributes.ParentObject = pAdapter->WdfDevice;

    WDF_WORKITEM_CONFIG_INIT(&workitemConfig, MpNdisResetWorkItem);
    ntStatus = WdfWorkItemCreate(&workitemConfig,
                                &attributes,
                                &pAdapter->NdisResetWorkItem);
    if (!NT_SUCCESS(ntStatus))
    {
        
        LOG_D("Failed to allocate Ndis Reset Workitem ");
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
        goto out;
    }
    adapterWorkitemContext = GetAdapterWorkItemContext(pAdapter->NdisResetWorkItem);
    adapterWorkitemContext->Adapter = pAdapter;

out:        
    return    ndisStatus;
}


/**
 * This routine allocates and initializes an Adapter data structure.
 * This DS represents a Physical Network Adapter present on the
 * system and allows us to work with it.
 * 
 * \param MiniportAdapterHandle     The handle needed to call NDIS API on this adapter
 * \param ppAdapter     The pointer to newly created adapter is returned in this variable
 * \return NDIS_STATUS_SUCCESS if all goes well, else appropriate error code
 * \sa MpFreeAdapter, MpInitialize
 */
NDIS_STATUS
MpAllocateAdapter(
	__in NDIS_HANDLE MiniportAdapterHandle,
	__deref_out_opt PADAPTER* ppAdapter
)
/*++

Routine Description:
	This routine allocates and initialize an adapter structure.

Arguments:

Return Value:

--*/

{
	PADAPTER                pAdapter = NULL;
	PSTATION                pStation = NULL;
	NTSTATUS                ntStatus;
	NDIS_STATUS             ndisStatus = NDIS_STATUS_SUCCESS;
	ULONG                   nameLength;
	WDF_OBJECT_ATTRIBUTES   attributes;

	*ppAdapter = NULL;

	do
	{
		//
		// Allocate a ADAPTER data structure.
		//
		MP_ALLOCATE_MEMORY(MiniportAdapterHandle, &pAdapter, sizeof(ADAPTER), MP_MEMORY_TAG);
		if (pAdapter == NULL)
		{
			LOG_E("MpAllocateAdapter: Failed to allocate %d bytes for ADAPTER", sizeof(ADAPTER));
			ndisStatus = NDIS_STATUS_RESOURCES;
			break;
		}

		//
		// Initialize it.
		//
		NdisZeroMemory(pAdapter, sizeof(ADAPTER));
		NdisInitializeListHead(&pAdapter->Link);

		NdisInitializeEvent(&pAdapter->MiniportRefCountZeroEvent);  // Will be reset during Restart
		NdisInitializeEvent(&pAdapter->ActiveSendersDoneEvent);
		NdisResetEvent(&pAdapter->ActiveSendersDoneEvent);
		NdisInitializeEvent(&pAdapter->HaltWaitEvent);
		NdisResetEvent(&pAdapter->HaltWaitEvent);
		NdisInitializeEvent(&pAdapter->AllReceivesHaveReturnedEvent);
		NdisResetEvent(&pAdapter->AllReceivesHaveReturnedEvent);
		NDIS_INIT_MUTEX(&pAdapter->ResetPnPMutex);
		//
		// Set power state of miniport to D0.
		//

		//
		// NdisMGetDeviceProperty function enables us to get the:
		// PDO - created by the bus driver to represent our device.
		// FDO - created by NDIS to represent our miniport as a function
		//              driver.
		// NextDeviceObject - deviceobject of another driver (filter)
		//                      attached to us at the bottom.
		//  But if we were to talk to a driver that we
		// are attached to as part of the devicestack then NextDeviceObject
		// would be our target DeviceObject for sending read/write Requests.
		//

		NdisMGetDeviceProperty(MiniportAdapterHandle,
			&pAdapter->Pdo,
			&pAdapter->Fdo,
			&pAdapter->NextDeviceObject,
			NULL,
			NULL);

		ntStatus = IoGetDeviceProperty(pAdapter->Pdo,
			DevicePropertyDeviceDescription,
			NIC_ADAPTER_NAME_SIZE,
			pAdapter->AdapterDesc,
			&nameLength);

		if (!NT_SUCCESS(ntStatus))
		{
			LOG_E("IoGetDeviceProperty failed (0x%x)", ntStatus);
			ndisStatus = NDIS_STATUS_FAILURE;
			break;
		}

		WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, WDF_DEVICE_INFO);

		ntStatus = WdfDeviceMiniportCreate(WdfGetDriver(),
			&attributes,
			pAdapter->Fdo,
			pAdapter->NextDeviceObject,
			pAdapter->Pdo,
			&pAdapter->WdfDevice);
		if (!NT_SUCCESS(ntStatus))
		{
			LOG_E("WdfDeviceMiniportCreate failed (0x%x)", ntStatus);
			ndisStatus = NDIS_STATUS_FAILURE;
			break;
		}

		WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
		attributes.ParentObject = pAdapter->WdfDevice;

		ntStatus = WdfSpinLockCreate(&attributes, &pAdapter->SendLock);
		NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
		if (ndisStatus != NDIS_STATUS_SUCCESS) {
			LOG_E("WdfSpinLockCreate   fail!");
			break;
		}

		//
		// Get WDF miniport device context.
		//
		GetWdfDeviceInfo(pAdapter->WdfDevice)->Adapter = pAdapter;

		ndisStatus = MpAllocateAdapterWorkItem(pAdapter);
		if (ndisStatus != NDIS_STATUS_SUCCESS)
		{
			break;
		}

        
        
		ntStatus = HwInitializeBus(pAdapter);

		if (ndisStatus != NDIS_STATUS_SUCCESS) {
			LOG_E("HwInitializeBus fail!");
			break;
		}

		MpInitNBLQueue(&(pAdapter->TxQueue));

	} while (FALSE);

    
	// After initializing bus successfully, initialize hardware.

	//ndisStatus = InitializeHw(pAdapter);
	
	ndisStatus = hw_init(pAdapter);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pAdapter)
        {
            MpFreeAdapter(pAdapter);
            pAdapter = NULL;
        }

        *ppAdapter = NULL;
    }
    else
    {    
        //
        // Return the allocated Adapter refernce
        //
        *ppAdapter = pAdapter;
    }
    
    return ndisStatus;
}

VOID
MpReadRegistryConfiguration(
    __inout PADAPTER pAdapter
    )
/**
 * This helper function uses NDIS API to read keys and values from the registry.
 * Use these values to make any configurations that the user may want.
 * It is always recommended to access the registry and initialize everything while
 * in the context on MiniportInitialize. Reading and writing to registry while NIC is
 * up and running always causes a performace hit.
 * 
 * \param pAdapter          The adapter whose configuration will be read
 * \sa MpInitialize
 */

{
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG                           i, ValueRead;
    PUCHAR                          pucDestination;
    PMP_REG_ENTRY                   pRegEntry;
    PNDIS_CONFIGURATION_PARAMETER   Parameter = NULL;
    BOOLEAN                         bRegistryOpened;
    NDIS_HANDLE                     RegistryConfigurationHandle;
    NDIS_CONFIGURATION_OBJECT       ConfigObject;

    ConfigObject.Header.Type = NDIS_OBJECT_TYPE_CONFIGURATION_OBJECT;
    ConfigObject.Header.Revision = NDIS_CONFIGURATION_OBJECT_REVISION_1;
    ConfigObject.Header.Size = sizeof( NDIS_CONFIGURATION_OBJECT );
    ConfigObject.NdisHandle = pAdapter->MiniportAdapterHandle;
    ConfigObject.Flags = 0;

    ndisStatus = NdisOpenConfigurationEx(
                    &ConfigObject,
                    &RegistryConfigurationHandle
                    );

    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        bRegistryOpened = TRUE;
    }
    else
    {
        LOG_E("Unable to Open Configuration. Will revert to defaults for all values");
        bRegistryOpened = FALSE;
    }


    for(i=0; i<MP_NUM_REG_PARAMS; i++)
    {
        //
        // Get the registry entry we will be reading
        //
        pRegEntry= &MPRegTable[i];

        //
        // Figure out where in the adapter structure this value will be placed
        //
        pucDestination = (PUCHAR) pAdapter + pRegEntry->FieldOffset;

        //
        // Read this entry from the registry. All parameters under NT are DWORDs
        //
        if (bRegistryOpened == TRUE)
        {
            NdisReadConfiguration(
                &ndisStatus,
                &Parameter,
                RegistryConfigurationHandle,
                &pRegEntry->RegName,
                NdisParameterInteger
                );
        }
        else
        {
            //
            // Report failure of reading registry. Will revert to defaults
            //
            ndisStatus = NDIS_STATUS_FAILURE;
        }

        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            if(Parameter->ParameterData.IntegerData < pRegEntry->Min || 
                Parameter->ParameterData.IntegerData > pRegEntry->Max)
            {
                LOG_E("A bad value %d read from registry. Reverting to default value %d",
                	Parameter->ParameterData.IntegerData, pRegEntry->Default);
                ValueRead = pRegEntry->Default;
            }
            else
            {
                ValueRead = Parameter->ParameterData.IntegerData;
            }
        }
        else
        {
            LOG_E("Unable to read from registry. Reverting to default value: %d",
            	pRegEntry->Default);
            ValueRead = pRegEntry->Default;
        }

        //
        // Moving the registry values in the adapter data structure
        //
        switch(pRegEntry->FieldSize)
        {
            case 1:
                *((PUCHAR) pucDestination)  = (UCHAR) ValueRead;
                break;
            case 2:
                *((PUSHORT) pucDestination) = (USHORT) ValueRead;
                break;
            case 4:
                *((PULONG) pucDestination)  = (ULONG) ValueRead;
                break;
            default:
                LOG_E("Bogus field size %d", pRegEntry->FieldSize);
                break;
        }
    }


    //
    // Close the handle to the registry
    //
    if (RegistryConfigurationHandle)
    {
        NdisCloseConfiguration(RegistryConfigurationHandle);
    }

    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        LOG_E("Failed to read from registry. Status = 0x%08x", ndisStatus);
    }
    
}

INLINE NDIS_STATUS
MpSetGeneralAttributes(
    __in  PADAPTER                        pAdapter
    )
{
    NDIS_MINIPORT_ADAPTER_ATTRIBUTES        MiniportAttributes;
    NDIS_PNP_CAPABILITIES                   PnpCapabilities;
    
    NdisZeroMemory(&MiniportAttributes, sizeof(MiniportAttributes));
    MiniportAttributes.GeneralAttributes.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES;
    MiniportAttributes.GeneralAttributes.Header.Revision = NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES_REVISION_1;
    MiniportAttributes.GeneralAttributes.Header.Size = sizeof(NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES);

    MiniportAttributes.GeneralAttributes.MediaType = MP_MEDIA_TYPE;
    MiniportAttributes.GeneralAttributes.PhysicalMediumType = MP_PHYSICAL_MEDIA_TYPE;    
    MiniportAttributes.GeneralAttributes.MtuSize = MP_802_11_MAX_FRAME_SIZE - MP_802_11_SHORT_HEADER_SIZE;
    MiniportAttributes.GeneralAttributes.MaxXmitLinkSpeed = HW_LINK_SPEED;
    MiniportAttributes.GeneralAttributes.MaxRcvLinkSpeed = HW_LINK_SPEED;
    MiniportAttributes.GeneralAttributes.XmitLinkSpeed = NDIS_LINK_SPEED_UNKNOWN;
    MiniportAttributes.GeneralAttributes.RcvLinkSpeed = NDIS_LINK_SPEED_UNKNOWN;
    MiniportAttributes.GeneralAttributes.MediaConnectState = MediaConnectStateConnected;
    MiniportAttributes.GeneralAttributes.MediaDuplexState = MediaDuplexStateFull;
    MiniportAttributes.GeneralAttributes.LookaheadSize = MP_802_11_MAXIMUM_LOOKAHEAD;
    MiniportAttributes.GeneralAttributes.PowerManagementCapabilities = &PnpCapabilities;
	
    //Hw11QueryPnPCapabilities(pAdapter->Nic, MiniportAttributes.GeneralAttributes.PowerManagementCapabilities);
    
    MiniportAttributes.GeneralAttributes.MacOptions = NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA | 
                                         NDIS_MAC_OPTION_TRANSFERS_NOT_PEND |
                                         NDIS_MAC_OPTION_NO_LOOPBACK;
    
    MiniportAttributes.GeneralAttributes.SupportedPacketFilters = NDIS_PACKET_TYPE_DIRECTED |
                                                     NDIS_PACKET_TYPE_MULTICAST |
                                                     NDIS_PACKET_TYPE_ALL_MULTICAST |
                                                     NDIS_PACKET_TYPE_BROADCAST;
    
    MiniportAttributes.GeneralAttributes.MaxMulticastListSize = HW_MAX_MCAST_LIST_SIZE;
    MiniportAttributes.GeneralAttributes.MacAddressLength = ETH_LENGTH_OF_ADDRESS;
    
    NdisMoveMemory(
        &MiniportAttributes.GeneralAttributes.PermanentMacAddress,
        pAdapter->PermanentAddress,
        ETH_LENGTH_OF_ADDRESS
        );
    
    NdisMoveMemory(
        &MiniportAttributes.GeneralAttributes.CurrentMacAddress,
        pAdapter->CurrentAddress,
        ETH_LENGTH_OF_ADDRESS
        );
    
    MiniportAttributes.GeneralAttributes.RecvScaleCapabilities = NULL;
    MiniportAttributes.GeneralAttributes.AccessType = NET_IF_ACCESS_BROADCAST;
    MiniportAttributes.GeneralAttributes.DirectionType = NET_IF_DIRECTION_SENDRECEIVE;
    MiniportAttributes.GeneralAttributes.IfType = IF_TYPE_IEEE80211;
    MiniportAttributes.GeneralAttributes.IfConnectorPresent = TRUE;

    MiniportAttributes.GeneralAttributes.DataBackFillSize = HW_REQUIRED_BACKFILL_SIZE;
    MpQuerySupportedOidsList(
        &MiniportAttributes.GeneralAttributes.SupportedOidList,
        &MiniportAttributes.GeneralAttributes.SupportedOidListLength
        );
    
    //
    // Register the Generic miniport attributes with NDIS
    //
    return NdisMSetMiniportAttributes(
        pAdapter->MiniportAdapterHandle,
        &MiniportAttributes
        );
}


INLINE VOID
MpResetPhyMIBs(
    __in  PADAPTER    pAdapter
    )
{
    //Hw11ResetPhyMIBs(pAdapter->Nic);
}

INLINE VOID
MpResetMacMIBs(
    __in  PADAPTER    pAdapter
    )
{

    ETH_COPY_NETWORK_ADDRESS(pAdapter->CurrentAddress, pAdapter->PermanentAddress);
    pAdapter->OperationMode     = DOT11_OPERATION_MODE_EXTENSIBLE_STATION;
    pAdapter->State             = INIT_STATE;
    pAdapter->Dot11RunningMode  = RUNNING_MODE_UNKNOWN;
    pAdapter->MaxRxLifeTime     = 512;

    MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_NETWORK_MONITOR_MODE);

    // Choose the correct receive processing handler
    MpAdjustReceiveHandler(pAdapter);

    //
    // Reset all counters. Some of them are not MIBs but we will reset
    // them anyways
    //
    pAdapter->TotalTransmits = 0;
    pAdapter->TotalTransmitsSnapShot = 0;
    pAdapter->NumStalledSendTicks = 0;
    pAdapter->TotalGoodTransmits = 0;
    pAdapter->TotalGoodReceives = 0;
    pAdapter->ReassemblyFailedReceives = 0;
    pAdapter->TotalBadTransmits = 0;
    pAdapter->TotalBadReceives = 0;
    pAdapter->MPDUMaxLength = 1500; /*Hw11GetMPDUMaxLength(pAdapter->Nic);*/
}

NDIS_STATUS
MPInitialize(
    __in  NDIS_HANDLE                        MiniportAdapterHandle,
    __in  NDIS_HANDLE                        MiniportDriverContext,
    __in  PNDIS_MINIPORT_INIT_PARAMETERS     MiniportInitParameters
    )
/*++
Routine Description:

    The MiniportInitialize function is a required function. Here is the
    list of things this function typically performs:

        Set the miniport attributes.
        Read configuration parameters from the registry.
        Allocate memory.
        Allocate the NET_BUFFER_LIST pool and the NET_BUFFER pool.
        Do hardware specific initialization like registering interrupt handlers/ set DMA resources etc.
        Read and write to the bus-specific configuration space.

    MiniportInitialize runs at IRQL = PASSIVE_LEVEL.

Arguments:

    MiniportAdapterHandle   The handle NDIS uses to refer to us
    MiniportDriverContext   Handle passed to NDIS when we registered the driver
    MiniportInitParameters  Initialization parameters contains a pointer
                            to a list of the allocated hardware resources
                            for the miniport adapter

Return Value:

    NDIS_STATUS_xxx code

--*/
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PADAPTER            pAdapter = NULL;
    NDIS_MINIPORT_ADAPTER_ATTRIBUTES    MiniportAttributes;                             
    BOOLEAN                        bHw11Started;
    BOOLEAN                        bSta11Started;
    BOOLEAN                        bHw11Initialized;
    NDIS_ERROR_CODE                 ErrorCode = NDIS_STATUS_SUCCESS;    
    ULONG                           ErrorValue = 0;
    BOOLEAN                            bSendEngineInitialized = FALSE, bReceiveEngineInitialized = FALSE;
    BOOLEAN                             bCustomInterfacesInitialized = FALSE;

    UNREFERENCED_PARAMETER(MiniportDriverContext);
    UNREFERENCED_PARAMETER(MiniportInitParameters);

    PAGED_CODE();

    bHw11Started = FALSE;
    bSta11Started = FALSE;
    bHw11Initialized = FALSE;
    LOG_D("---> MPInitialize");
    LOG_D("MiniportAdapterHandle: %p", MiniportAdapterHandle);
        
    do {
        //
        // Allocate adapter context structure and initialize all the
        // memory resources for sending and receiving packets.
        //
        ndisStatus = MpAllocateAdapter (MiniportAdapterHandle, &pAdapter);
        if(ndisStatus != NDIS_STATUS_SUCCESS)
        {
            pAdapter = NULL;
            break;
        }
		bHw11Started = TRUE;

        pAdapter->MiniportAdapterHandle = MiniportAdapterHandle;


        MpInitializeNicVariable(pAdapter);

		wf_oids_adapt_init(pAdapter);

		wf_xmit_init(pAdapter);
		wf_recv_init(pAdapter);

        //
        // Set the miniport registration attributes with NDIS
        //
        NdisZeroMemory(&MiniportAttributes, sizeof(MiniportAttributes));
        MiniportAttributes.RegistrationAttributes.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES;
        MiniportAttributes.RegistrationAttributes.Header.Revision = NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES_REVISION_1;
        MiniportAttributes.RegistrationAttributes.Header.Size = sizeof(NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES);
        MiniportAttributes.RegistrationAttributes.MiniportAdapterContext = pAdapter;
        MiniportAttributes.RegistrationAttributes.AttributeFlags = NDIS_MINIPORT_ATTRIBUTES_NDIS_WDM ;
        MiniportAttributes.RegistrationAttributes.CheckForHangTimeInSeconds = 64;//HW11_CHECK_FOR_HANG_TIME_IN_SECONDS;
        MiniportAttributes.RegistrationAttributes.InterfaceType = HW11_BUS_INTERFACE_TYPE;

        NdisMSetMiniportAttributes(
            MiniportAdapterHandle,
            &MiniportAttributes
            );
#ifdef USE_SAMPLE_PACKET_BUFFERING_IMPLEMENTATION
        //
        // We precalculate this value as we will be using it frequently in the driver
        //
        pAdapter->NumTicksPer100TU = ((10240 * 100) / KeQueryTimeIncrement());
        
#endif
        MP_SET_FLAG(pAdapter, fMP_INIT_IN_PROGRESS);

        //
        // Read Advanced configuration information from the registry
        //

        MpReadRegistryConfiguration(
                    pAdapter
                    );
		
        //
        // Fill the Miniport 802.11 Attributes, we can do so as soon as NIC is initialized.
        //
        ndisStatus = MpSet80211Attributes(pAdapter);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            LOG_E("Failed to set 80211 attributes");
            break;
        }
        //
        // Initialize the Mp Send Engine. This will also initialize Hw11 Send engine
        // 
        ndisStatus = MpInitializeSendEngine(pAdapter);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            LOG_E("Failed to initialize the Send Engine");
            break;
        }
        bSendEngineInitialized = TRUE;

        //
        // TODO: Initialize the Hw11 Receive Engine. This will also initialize Mp Receive engine.  2021/03/03
        // 
        ndisStatus = HwBusAllocateRecvResources(pAdapter);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            LOG_E("Failed to initialize the Receive Engine");
            break;
        }
        bReceiveEngineInitialized = TRUE;

        //
        // Initialize Custom Interfaces for this miniport driver
        //
        ndisStatus = MpEventInitialize(pAdapter);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            LOG_E("Failed to initialize the Custom Interfaces");
            break;
        }
          
        pAdapter->AdapterState = NicRunning;  
       
        //
        //register an ioctl interface
        //vm control
        //
        ndisStatus = NICRegisterDevice(NdisMiniportDriverHandle, pAdapter);
        if (!NT_SUCCESS(ndisStatus)) {
            LOG_E("NdisProtCreateControlDevice failed with status 0x%x", ndisStatus);
            break;
        }
        //
        // Initialize the MIB values we maintain at MP level
        //
		
		nic_info_st *pNic_info = (nic_info_st*)pAdapter->nic_info;
		hw_info_st *phw_info = pNic_info->hw_info;
        ETH_COPY_NETWORK_ADDRESS(pAdapter->PermanentAddress, (PDOT11_MAC_ADDRESS)phw_info->macAddr);


        MpResetMacMIBs(pAdapter);
        MpResetPhyMIBs(pAdapter);
		// TODO: Move this to reset workitem. 
		StaResetCipherAbilities(pAdapter);
        //
        // Fill the Miniport Generic Attributes
        //
        ndisStatus = MpSetGeneralAttributes(pAdapter);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            LOG_E("Failed to set general attributes");
            break;
        }

        //
        // The miniport will start in the Paused PnP state        
        // MpRestart is called after MpInitialize so the state wil change to MINIPORT_RUNNING there
        //
        MP_SET_NDIS_PNP_STATE(pAdapter, MINIPORT_PAUSED);

        //
        // We are started as paused. Add the paused counter. When we restart
        // again, we will be adding refcounts for active receives, pending
        // SG operations, etc
        //
        MP_INCREMENT_RESET_PAUSE_COUNT(pAdapter);        

        //
        // This Ref count should be incremented if there are any
        // operations outstanding on this miniport that should prevent
        // miniport from halting
        //
        MP_INCREMENT_REF(pAdapter);
        LOG_D("Starting USB pipes!");
        
        // Since the pipe_lock is a WDFWAITLOCK, DISPATICH_LEVEL is not needed.        
        ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
        HwBusStartDevice(pAdapter);
        LOG_D("USB pipes has been started successfully!");
        //
    } WHILE (FALSE);

    if(pAdapter && ndisStatus != NDIS_STATUS_SUCCESS) {
        LOG_D("MPInitialize failed! Error code: 0x%08x", ndisStatus);
        
        //
        // Initialization failed. Free up any resources that are currently allocated
        //
        if (pAdapter)
        {
            
            if (bSendEngineInitialized) {            
                MpTerminateSendEngine(pAdapter);            // Free all send resources
            }
            if(bCustomInterfacesInitialized) {            
                MpEventTerminate(pAdapter); 
            }
            //
            // Now free allocated resources. This also frees resources allocated by NIC
            //
            MpFreeAdapter(pAdapter);
        }

        if (ErrorCode != NDIS_STATUS_SUCCESS)
        {
            NdisWriteErrorLogEntry(
                MiniportAdapterHandle,
                ErrorCode,
                1,
                ErrorValue
                );
        }
        
    }

    LOG_D("<--- MPInitialize ndisStatus = 0x%08x%", ndisStatus);

    return ndisStatus;

}

INLINE VOID
MpOnSurpriseRemoval(
    __in  PADAPTER        pAdapter
    )
{
    //
    // Set the surprise removal flag. We need to remember this
    // in case other requests get submitted to the miniport
    //
    MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED);

	nic_info_st *nic_info = pAdapter->nic_info;

	nic_info->is_surprise_removed = wf_true;
}


VOID
MPHalt(
    __in  NDIS_HANDLE             MiniportAdapterContext,
    __in  NDIS_HALT_ACTION        HaltAction
    )
/*++

Routine Description:

    Halt handler is called when NDIS receives IRP_MN_STOP_DEVICE,
    IRP_MN_SUPRISE_REMOVE or IRP_MN_REMOVE_DEVICE requests from the
    PNP manager. Here, the driver should free all the resources acquired
    in MiniportInitialize and stop access to the hardware. NDIS will
    not submit any further request once this handler is invoked.

    1) Free and unmap all I/O resources.
    2) Disable notification(interrupts) and deregister notification(interrupt) handler.
    3) Cancel all queued up timer callbacks.
    4) Finally wait indefinitely for all the outstanding receive
        packets indicated to the protocol to return.

    MiniportHalt runs at IRQL = PASSIVE_LEVEL.


Arguments:

    MiniportAdapterContext  Pointer to our adapter
    HaltAction              The reason adapter is being halted

Return Value:

    None.

--*/
{
    
//    BOOLEAN           bDone=TRUE;
//    LONG              nHaltCount = 0;
    PADAPTER          pAdapter = (PADAPTER) MiniportAdapterContext;
    ULONG           CurrentStatus;

//    UNREFERENCED_PARAMETER(HaltAction);
//    UNREFERENCED_PARAMETER(MiniportAdapterContext);

    LOG_D("---> MPHalt");
    LOG_D("Calling MpHalt with action 0x%x",HaltAction);
    PAGED_CODE();

//    MP_SET_FLAG(pAdapter, fMP_HALT_IN_PROGRESS);
//
//    MP_CLEAR_FLAG(pAdapter, fMP_POST_WRITES);
//    MP_CLEAR_FLAG(pAdapter, fMP_POST_READS);
//    
    //
    // Mark the miniport as going in halt
    //
//    CurrentStatus = MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_HALT_IN_PROGRESS);
//    if (CurrentStatus & MP_ADAPTER_RESET_IN_PROGRESS)
//    {
//        //
//        // A reset operation is currently occuring. We cannot halt till it completes
//        //
//
//        // TODO: Determine if I need to wait for reset to finish or NDIS guarantees
//        // halt doesnt happen with reset
//        while (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS) == TRUE)
//        {
//            NdisMSleep(20 * 1000);      // 20 seconds
//        }
//    }
    
    //MPASSERTMSG("No sends should have been pended on the miniport when halt is called\n",
    //              (pAdapter->PendingSends == 0));
    
    //
    // Scan should not be in progress during halt
    //
    //MPASSERT(MP_DOT11_SCAN_IN_PROGRESS(pAdapter) == FALSE);

    //
    // Unregister the ioctl interface.  vm control
    //
    
    
    //
    // First issue a shutdown turn off hardware
    //
    //MPAdapterShutdown(MiniportAdapterContext, NdisShutdownPowerOff);
    
    //
    // PCI -- Deregister interrupts as soon as possible
    //   
    
//    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED) == FALSE)
//    {
//         //
//        // Pause the beaconing
//        //
//        //Hw11StopAdHoc(pAdapter->Nic);
//    }
    
    //
    // Drop any Rx MSDUs that have exceeded RxLifeTime value. We cannot
    // free all receive resources without this.
    // TODO: Can this be done in MPPause?
    //MpExpireReassemblingPackets(pAdapter, FALSE);
    
    //
    // Invoke the terminate event
    //
    //MpEventTerminate(pAdapter);

    
    //
    // If ref count is not zero yet that implies:
    // 1. A pending Async Shared memory allocation
    // 2. We are in the middle of a scan
    //
//    MP_DECREMENT_REF(pAdapter);
//    if (pAdapter->RefCount > 0)
//    {
//        do
//        {
//            if(NdisWaitEvent(&pAdapter->HaltWaitEvent, 2000))
//            {
//                break;
//            }
//
//            LOG_D("Waiting 2 more seconds for all Miniport Ref Count to go to zero");
//        } while(TRUE);
//    }
//    MPVERIFY(pAdapter->NumActiveReceivers == 0);
    
    //
    // By the time Halt is called as indicated received should have been returned
    //
//    MPASSERT(pAdapter->PendingReceives == 0);

    
//    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED) == FALSE)
//    {
//        //
//        // Inform the hardware that it is being halted.
//        //
//        //Hw11HaltNic(pAdapter->Nic);
//    }
    //
    // Release all resources kept for send and receive
    //
    
    
    //MpTerminateSendEngine(pAdapter);
    //
    // Uninitialize the NIC
    //
    //Hw11TerminateNic(pAdapter->Nic);
	nic_info_st *nic_info;
	nic_info = (nic_info_st*)pAdapter->nic_info;

	NICDeregisterDevice(pAdapter);
	
	nic_disable(nic_info);
	nic_shutdown(nic_info);
	nic_term(nic_info);

	wf_xmit_deinit(pAdapter);
	wf_recv_deinit(pAdapter);
	
	HwBusStopNotification(pAdapter);
	HwBusFreeRecvResources(pAdapter);

	wf_oids_adapt_deinit(pAdapter);
    //
    // Free adapter resources. Also frees up NIC resources.
    //
    MpFreeAdapter(pAdapter);

    LOG_D("<--- MPHalt");
}

NDIS_STATUS
MpGetAdapterStatus(
    __in PADAPTER         pAdapter
    )
{
    NDIS_STATUS ndisStatus;

    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_NDIS_PAUSE_IN_PROGRESS))
        ndisStatus = NDIS_STATUS_PAUSED;
    else if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS))
        ndisStatus = NDIS_STATUS_RESET_IN_PROGRESS;
    else if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HALT_IN_PROGRESS))
        ndisStatus = NDIS_STATUS_CLOSING;
    else if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED))
        ndisStatus = NDIS_STATUS_CLOSING;
    else if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_REMOVE_IN_PROGRESS))
        ndisStatus = NDIS_STATUS_CLOSING;
    else if MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HARDWARE_ERROR)
        ndisStatus = NDIS_STATUS_DEVICE_FAILED;
    else
        ndisStatus = NDIS_STATUS_FAILURE;       // return a generc error

    return ndisStatus;
}


INLINE 
BOOLEAN
MpRemoveAdapter(
    __in PADAPTER pAdapter
    )
{
    if (!(MP_SET_CLEAR_STATUS_FLAG(
            pAdapter,
            MP_ADAPTER_REMOVE_IN_PROGRESS,      // Set this bit
            MP_ADAPTER_RESET_IN_PROGRESS        // Clear this bit
            ) & MP_ADAPTER_REMOVE_IN_PROGRESS))  // Test this bit
    {
        //
        // Request a removal.
        //
        NdisWriteErrorLogEntry(
            pAdapter->MiniportAdapterHandle,
            NDIS_ERROR_CODE_HARDWARE_FAILURE,
            1,
            ERRLOG_REMOVE_MINIPORT
            );
        
        NdisMRemoveMiniport(pAdapter->MiniportAdapterHandle);        
        return TRUE;
    }
    else
    {
        //
        // Adapter is already in removal. No need to request one.
        //
        return FALSE;
    }
}


NDIS_STATUS
MPReset(
    __in  NDIS_HANDLE     MiniportAdapterContext,
    __out PBOOLEAN        AddressingReset
    )
/*++

Routine Description:

    An NDIS 6.0 miniport driver's reset function can cancel pending OID
    requests and send requests in the context of a reset.
    NDIS no longer performs such cancellations. Alternatively,
    the miniport driver can complete pending OID requests
    and send requests after the reset completes.

    MiniportReset function can be called at either IRQL PASSIVE_LEVEL
    or IRQL = DISPATCH_LEVEL.

Arguments:

    AddressingReset         To let NDIS know whether we need help from it with our reset
    MiniportAdapterContext  Pointer to our adapter

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING
    NDIS_STATUS_RESET_IN_PROGRESS
    NDIS_STATUS_HARD_ERRORS

Note:

--*/
{
    PADAPTER       pAdapter = (PADAPTER) MiniportAdapterContext;
    NDIS_STATUS    ndisStatus = NDIS_STATUS_PENDING;
    ULONG           CurrentStatus;

//    UNREFERENCED_PARAMETER(AddressingReset);
//    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    
    LOG_D("---> MPReset");


    LOG_D("The miniport driver has been reset");
    
    //
    // Request NDIS to add the addresses on the miniport through OIDs
    // once the reset is complete.
    //
    *AddressingReset = HW_REQUIRES_ADDRESS_RESET;
    
    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
    
    do
    {
        CurrentStatus = MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS);
                
        if (CurrentStatus & MP_ADAPTER_RESET_IN_PROGRESS)
        {
            //
            // The "in reset" bit is already set. Tell NDIS we are already in reset and
            // return immediately.
            //
            ndisStatus = NDIS_STATUS_RESET_IN_PROGRESS;
            break;
        }
        
        //
        // If our halt handler has been called, we should not reset
        //
        if (CurrentStatus & MP_ADAPTER_HALT_IN_PROGRESS)
        {
            MPASSERT(FALSE);    // Would be an interesting scenario to investigate
            MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS);
            ndisStatus = NDIS_STATUS_SUCCESS;
            break;
        }
        
        //
        // If the adapter has Non-Recoverable hardware errors, request removal
        //
        if (CurrentStatus & MP_ADAPTER_NON_RECOVER_ERROR)
        {
            MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS);
            ndisStatus = NDIS_STATUS_HARD_ERRORS;
            MpRemoveAdapter(pAdapter);
            break;
        }
    } while(FALSE);
    
    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    
    if (ndisStatus == NDIS_STATUS_PENDING)
    {
        LOG_D("Scheduling a work item to complete the reset");

        WdfWorkItemEnqueue(pAdapter->NdisResetWorkItem);
    }
    LOG_D("<--- MPReset");
    return ndisStatus;
}

VOID
DriverUnload(
    __in  PDRIVER_OBJECT  DriverObject
    )
/*++

Routine Description:

    The unload handler is called during driver unload to free up resources
    acquired in DriverEntry. Note that an unload handler differs from
    a MiniportHalt function in that the unload handler has a more global
    scope, whereas the scope of the MiniportHalt function is restricted
    to a particular miniport driver instance.

    During an unload operation, an NDIS 6.0 miniport driver must deregister
    the miniport driver by calling NdisMDeregisterMiniportDriver
    and passing the MiniportDriverHandle as a parameter.
    The driver obtained the MiniportDriverHandle when it called
    NdisMRegisterMiniportDriver.

    Runs at IRQL = PASSIVE_LEVEL.

Arguments:

    DriverObject        Not used

Return Value:

    None

--*/
{
    LOG_D("--> DriverUnload");

    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE();

    WdfDriverMiniportUnload(WdfGetDriver());

    NdisMDeregisterMiniportDriver(NdisMiniportDriverHandle);

    #if DOT11_TRACE_ENABLED
        WPP_CLEANUP(DriverObject);
    #endif
    LOG_D("<--- DriverUnload");
}

BOOLEAN
MPCheckForHang(
    __in NDIS_HANDLE MiniportAdapterContext
    )
/*++

Routine Description:

    The MiniportCheckForHang handler is called to report the state of the
    NIC, or to monitor the responsiveness of an underlying device driver.
    This is an optional function. If this handler is not specified, NDIS
    judges the driver unresponsive when the driver holds
    MiniportQueryInformation or MiniportSetInformation requests for a
    time-out interval (deafult 4 sec), and then calls the driver's
    MiniportReset function. A NIC driver's MiniportInitialize function can
    extend NDIS's time-out interval by calling NdisMSetAttributesEx to
    avoid unnecessary resets.

    Always runs at IRQL = DISPATCH_LEVEL.

Arguments:

    MiniportAdapterContext  Pointer to our adapter

Return Value:

    TRUE    NDIS calls the driver's MiniportReset function.
    FALSE   Everything is fine

Note:
    CheckForHang handler is called in the context of a timer DPC.
    take advantage of this fact when acquiring/releasing spinlocks

--*/
{
    PADAPTER        pAdapter = (PADAPTER) MiniportAdapterContext;
    BOOLEAN         bNeedReset = FALSE;
//    UNREFERENCED_PARAMETER(MiniportAdapterContext);
	
    do
    {
        //
        // Invoke MpEventCheckForHang. See if reset is needed.
        //
        if (MpEventCheckForHang(pAdapter))
        {
            LOG_D("MpEventCheckForHang has requested a reset");
			bNeedReset = TRUE;
            break;            
        }
        //
        // If a hardware error has occured, ask NDIS to reset us
        //
        if (MP_TEST_STATUS_FLAG(pAdapter, (MP_ADAPTER_NON_RECOVER_ERROR | MP_ADAPTER_HARDWARE_ERROR)))
        {
            LOG_E("Requesting reset since a hardware error has occured");
            bNeedReset = TRUE;
            break;
        }
        
        //
        // Check to see if packet are not getting sent. Ask for reset if stalled.
        //
        if (pAdapter->PendingSends > 0)
        {
            if (pAdapter->TotalTransmitsSnapShot == 0)
            {
                //
                // Take a snap shot of the total transmits done so far. Sampling begins now
                //
                LOG_E("Restart monitoring the total transmit count");
                pAdapter->TotalTransmitsSnapShot = pAdapter->TotalTransmits;
            }
            else if (pAdapter->TotalTransmitsSnapShot == pAdapter->TotalTransmits)
            {
                LOG_D("%d sample periods of total transmits have occured", pAdapter->NumStalledSendTicks);
                //
                // If the number of sends pended on the Hw11 has not changed we will
                // count a tick interval of stalled send period.
                //
                pAdapter->NumStalledSendTicks++;

                MPASSERTMSG(
                    "If one more ChecForHang detects stalled sends, we will reset! Investigate\n",
                    pAdapter->NumStalledSendTicks != MP_SENDS_HAVE_STALLED_PERIOD - 1
                    );
                
                if(pAdapter->NumStalledSendTicks >= MP_SENDS_HAVE_STALLED_PERIOD)
                {
                    LOG_D("Send Engine seems to be stalled. Requesting reset");
                    LOG_D("Requesting reset from NDIS");
                    MPASSERTMSG("Reset should not occur normally! Investigate\n", FALSE);
                    bNeedReset = TRUE;
                    break;
                }
            }
            else
            {
                //
                // Everything is ok. Pended sends have been completing.
                //
                LOG_D("Transmissions are working properly.");
                MPVERIFY(pAdapter->TotalTransmitsSnapShot < pAdapter->TotalTransmits);
                pAdapter->TotalTransmitsSnapShot = 0;
                pAdapter->NumStalledSendTicks = 0;
            }
        }
            
        //
        // Sample the usage of Rx MSDU list. Will be used during MpReturnPackets to determine
        // if we need to shrink the Rx MSDU list.
        //
        if (pAdapter->RxMSDUListSampleTicks < MP_RX_MSDU_LIST_SAMPLING_PERIOD)
        {
            pAdapter->RxMSDUListSampleTicks++;
            pAdapter->NumRxMSDUNotUtilized += (pAdapter->TotalRxMSDUAllocated - pAdapter->PendingReceives);
			//DbgPrint("********NumRxMSDUNotUtilized=%d\n", pAdapter->NumRxMSDUNotUtilized);
			//DbgPrint("********TotalRxMSDUAllocated=%d\n", pAdapter->TotalRxMSDUAllocated);
			//DbgPrint("********PendingReceives=%d\n", pAdapter->PendingReceives);
        }
        else
        {
            LOG_D("Percentage of Under Utilization = %d", (pAdapter->NumRxMSDUNotUtilized * 100) / (pAdapter->RxMSDUListSampleTicks * pAdapter->TotalRxMSDUAllocated));
            pAdapter->RxMSDUListSampleTicks = 0;
            pAdapter->NumRxMSDUNotUtilized = 0;
        }
        LOG_D("%d sample periods of Rx MSDU list have passed", pAdapter->RxMSDUListSampleTicks);
    } while(FALSE);
    return bNeedReset;
}

VOID 
MpInternalPause(
    __in  PADAPTER                            pAdapter,
    __in  PNDIS_MINIPORT_PAUSE_PARAMETERS     MiniportPauseParameters
    )
{
    UNREFERENCED_PARAMETER(MiniportPauseParameters);
    
    MPVERIFY(MP_GET_NDIS_PNP_STATE(pAdapter) == MINIPORT_PAUSING);

    //
    // Notify hardware about pause. As an optimization, hardware can
    // turn off send receive units etc.
    //

    HwBusStopNotification(pAdapter);
    
    //
    // Send lock to serialize against other operations
    //
    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

    //
    // Cancel all pending sends
    //
    //MpCompleteQueuedTxMSDUs(pAdapter);
    
    //
    // Complete all packets pending in the TxQueue
    //
    //MpCompleteQueuedNBL(pAdapter);

    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);

}

NDIS_STATUS
MPPause(
    __in  NDIS_HANDLE                         MiniportAdapterContext,
    __in  PNDIS_MINIPORT_PAUSE_PARAMETERS     MiniportPauseParameters
    )
/*++

Routine Description:

    Miniport pause and restart are new features introduced in NDIS 6.0.
    NDIS calls a miniport driver's MiniportPause function to stop
    data flow before a Plug and Play operation, such as adding or
    removing a filter driver or binding or unbinding a protocol driver,
    is performed. The adapter remains in the Pausing state until the
    pause operation has completed.

    When in the Paused state, a miniport driver can indicate status by
    calling the NdisMIndicateStatusEx function. In addition, the miniport
    driver should do the following:
    - Wait for all calls to the NdisMIndicateReceiveNetBufferLists
        function to return.
    - Wait for NDIS to return the ownership of all NET_BUFFER_LIST
        structures from outstanding receive indications to the miniport
        driver's MiniportReturnNetBufferLists function.
    - Complete all outstanding send requests by calling NdisMSendNetBufferListsComplete.
    - Reject all new send requests made to its MiniportSendNetBufferLists
        function by immediately calling the NdisMSendNetBufferListsComplete
        function. The driver should set the completion status in each
        NET_BUFFER_LIST structure to NDIS_STATUS_PAUSED.
    - Continue to handle OID requests in its MiniportOidRequest function.
    - Not stop the adapter completely if doing so prevents the driver
        from handling OID requests or providing status indications.
    - Not free the resources that the driver allocated during initialization.
    - Continue to handle DevicePnPEvent notifications.

    NDIS calls the miniport driver's MiniportRestart function to cause
    the miniport to return the adapter to the Running state.
    During the restart, the miniport driver must complete any tasks
    that are required to resume send and receive operations before
    completing the restart request.

Argument:

    MiniportAdapterContext  Pointer to our adapter

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING

NOTE: A miniport can't fail the pause request

--*/
{
    PADAPTER         pAdapter = (PADAPTER) MiniportAdapterContext;
    NDIS_STATUS      ndisStatus;
    ULONG           CurrentStatus, uReceiveCount;

    UNREFERENCED_PARAMETER(MiniportPauseParameters);
    UNREFERENCED_PARAMETER(pAdapter);

    LOG_D("---> MPPause");

    MP_VERIFY_PASSIVE_IRQL();
    
    MPASSERT(MP_GET_NDIS_PNP_STATE(pAdapter) == MINIPORT_RUNNING);


    MP_ACQUIRE_RESET_PNP_LOCK(pAdapter);
    
    do
    {
        //
        // Acquire the lock and make sure that all sends will fail from this point on
        //
        MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

        //
        // Mark adapter as going in pause. The lock helps synchronize with send operations
        //
        MP_SET_NDIS_PNP_STATE(pAdapter, MINIPORT_PAUSING);
        CurrentStatus = MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_NDIS_PAUSE_IN_PROGRESS);
        
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);

        //
        // If a scan operation is in progress, cancel it first! All kinds
        // of sends and receives must cease.
        //
        if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HW_IS_SCANNING))
        {
            //Hw11CancelScan(pAdapter->Nic);
        }
        MPVERIFY(!MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_HW_IS_SCANNING));

        //
        // If we are already performing a Reset/Dot11Reset, the active
        // senders, etc is down
        //
        if (MP_INCREMENT_RESET_PAUSE_COUNT(pAdapter) == 1)
        {
            //
            // Remove the Active Sender count we added during MPRestart
            //
            MP_DECREMENT_ACTIVE_SENDERS(pAdapter);
            NdisWaitEvent(&pAdapter->ActiveSendersDoneEvent, MP_NEVER_TIMEOUT);
            
            //
            // Remove the Active Receivers count added during Restart.
            // We can pause when the count reaches 1 or less.
            //
            MP_DECREMENT_ACTIVE_RECEIVERS(pAdapter);
            while(pAdapter->NumActiveReceivers >= 1)
            {
                NdisMSleep(10 * 1000);  // 10 sec
            }
        }
        
        //
        // Remove the ref we added in MPRestart on receives indicated up to OS
        //
        if ((uReceiveCount = MP_DECREMENT_PENDING_RECEIVES(pAdapter)) == 0)
        {
            NdisSetEvent(&pAdapter->AllReceivesHaveReturnedEvent);
        }
        NdisWaitEvent(&pAdapter->AllReceivesHaveReturnedEvent, MP_NEVER_TIMEOUT);
        
        //
        // Pause the miniport by dropping pending send packets etc.
        //
        //MpInternalPause(pAdapter, MiniportPauseParameters);
        
        //
        // Set the miniport state as paused
        // 
        MP_SET_NDIS_PNP_STATE(pAdapter, MINIPORT_PAUSED);
        
        ndisStatus = NDIS_STATUS_SUCCESS;
    } while(FALSE);
    
    MP_RELEASE_RESET_PNP_LOCK(pAdapter);  

    LOG_D("<--- MPPause 0x%x", ndisStatus);
    return ndisStatus;
}

NDIS_STATUS
MPRestart(
    __in  NDIS_HANDLE                         MiniportAdapterContext,
    __in  PNDIS_MINIPORT_RESTART_PARAMETERS   MiniportRestartParameters
    )
/*++

Routine Description:

    NDIS calls the miniport driver's MiniportRestart function to cause
    the miniport to return the adapter to the Running state.
    During the restart, the miniport driver must complete any tasks
    that are required to resume send and receive operations before
    completing the restart request.

Argument:

    MiniportAdapterContext  Pointer to our adapter

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING  Can it return pending
    NDIS_STATUS_XXX      The driver fails to restart


--*/
{
    PADAPTER                  pAdapter = (PADAPTER)MiniportAdapterContext;
    PNDIS_RESTART_ATTRIBUTES     NdisRestartAttributes;
    //PNDIS_RESTART_GENERAL_ATTRIBUTES  NdisGeneralAttributes;
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
    
    MP_VERIFY_PASSIVE_IRQL();

	
    MPASSERT(pAdapter->NdisPnPState == MINIPORT_PAUSED);

    LOG_D("---> MPRestart");
    NdisRestartAttributes = MiniportRestartParameters->RestartAttributes;

    //
    // Synchronize the Restart operation with Pause and Reset. 
    // This is not a spin lock as these functions are going to be called at 
    // PASSIVE_LEVEL only. 
    //
    MP_ACQUIRE_RESET_PNP_LOCK(pAdapter);

    do
    {
        // Reset properties.
        StaResetCipherAbilities(pAdapter);
        
        //
        // Notify the hardware so it can undone any ops done during Pause
        //
        
        // Restart usb pipes.
        ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
        ndisStatus = HwBusStartDevice(pAdapter);

        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            LOG_D("Hw11MiniportRestart failed with status 0x%08x", ndisStatus);
            break;
        }
        
        
        MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);

        //
        // If no Reset/Dot11Reset is pending, we can restart
        //
        if (MP_DECREMENT_RESET_PAUSE_COUNT(pAdapter) == 0)
        {
            //
            // This Ref Count will be removed on Pause. Needed to make sure no receives
            // are occuring when Pause occurs
            //
            MP_INCREMENT_ACTIVE_RECEIVERS(pAdapter);
            
            //
            // We always keep one reference on active senders unless we are pausing/halting
            // or resetting. This avoid repeated signal of the associated event. We will
            // be resetting that event as well
            //
            MP_INCREMENT_ACTIVE_SENDERS(pAdapter);
            NdisResetEvent(&pAdapter->ActiveSendersDoneEvent);
        }
        
        //
        // Increment the receive Ref count on the miniport. The reciprocal for this
        // call is in MiniportPause. This way we will not signal AllReceivesHaveReturned
        // event till MiniportPause is called.
        //
        MP_INCREMENT_PENDING_RECEIVES(pAdapter);
        NdisResetEvent(&pAdapter->AllReceivesHaveReturnedEvent);
        
        //
        // The miniport is now operational
        //
        MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_NDIS_PAUSE_IN_PROGRESS);
        MP_SET_NDIS_PNP_STATE(pAdapter, MINIPORT_RUNNING);
    
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    }while(FALSE);
    
    MP_RELEASE_RESET_PNP_LOCK(pAdapter);    
	LOG_D("<--- MPRestart");

    
    return ndisStatus;

}


VOID
MPPnPEventNotify(
    __in  NDIS_HANDLE             MiniportAdapterContext,
    __in  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent
    )
/*++

Routine Description:

    MPPnPEventNotify is to handle PnP notification messages.
    An NDIS miniport driver with a WDM lower edge should export a
    MiniportPnPEventNotify function so that it can be notified
    when its NIC is removed without prior notification through
    the user interface. When a miniport driver receives
    notification of a surprise removal, it should note internally
    that the device has been removed and cancel any pending Requests
    that it sent down to the underlying bus driver. After calling
    the MiniportPnPEventNotify function to indicate the surprise
    removal, NDIS calls the miniport’s MiniportHalt function.
    If the miniport driver receives any requests to send packets
    or query or set OIDs before its MiniportHalt function is
    called, it should immediately complete such requests with a
    status value of NDIS_STATUS_NOT_ACCEPTED.

    All NDIS 5.1 miniport drivers must export a MPPnPEventNotify
    function.

    Runs at IRQL = PASSIVE_LEVEL in the context of system thread.

    Available - NDIS5.1 (WinXP) and later.

Arguments:

    MiniportAdapterContext      Pointer to our adapter
    PnPEvent                    Self-explanatory

Return Value:

    None

--*/
{
    PADAPTER             Adapter;
    PNDIS_POWER_PROFILE     NdisPowerProfile;
    NDIS_DEVICE_PNP_EVENT   PnPEvent;
    PVOID                   InformationBuffer;
    ULONG                   InformationBufferLength;

    LOG_D("---> MPPnPEventNotify");


    Adapter = (PADAPTER)MiniportAdapterContext;
    PnPEvent = NetDevicePnPEvent->DevicePnPEvent;
    InformationBuffer = NetDevicePnPEvent->InformationBuffer;
    InformationBufferLength = NetDevicePnPEvent->InformationBufferLength;

    //
    // NDIS currently sends only SurpriseRemoved and
    // PowerProfileChange Notification events.
    //
    switch (PnPEvent)
    {
        case NdisDevicePnPEventQueryRemoved:
            //
            // Called when NDIS receives IRP_MN_QUERY_REMOVE_DEVICE.
            //
            LOG_D("MPPnPEventNotify: NdisDevicePnPEventQueryRemoved");
            break;

        case NdisDevicePnPEventRemoved:
            //
            // Called when NDIS receives IRP_MN_REMOVE_DEVICE.
            // NDIS calls MiniportHalt function after this call returns.
            //
            LOG_D("MPPnPEventNotify: NdisDevicePnPEventRemoved");
            break;

        case NdisDevicePnPEventSurpriseRemoved:
            //
            // Called when NDIS receives IRP_MN_SUPRISE_REMOVAL.
            // NDIS calls MiniportHalt function after this call returns.
            //
            MP_SET_FLAG(Adapter, fMP_SURPRISE_REMOVED);
            LOG_D("MPPnPEventNotify: NdisDevicePnPEventSurpriseRemoved");
            break;

        case NdisDevicePnPEventQueryStopped:
            //
            // Called when NDIS receives IRP_MN_QUERY_STOP_DEVICE. ??
            //
            LOG_D("MPPnPEventNotify: NdisDevicePnPEventQueryStopped");
            break;

        case NdisDevicePnPEventStopped:
            //
            // Called when NDIS receives IRP_MN_STOP_DEVICE.
            // NDIS calls MiniportHalt function after this call returns.
            //
            //
            LOG_D("MPPnPEventNotify: NdisDevicePnPEventStopped");
            break;

        case NdisDevicePnPEventPowerProfileChanged:
            //
            // After initializing a miniport driver and after miniport driver
            // receives an OID_PNP_SET_POWER notification that specifies
            // a device power state of NdisDeviceStateD0 (the powered-on state),
            // NDIS calls the miniport's MiniportPnPEventNotify function with
            // PnPEvent set to NdisDevicePnPEventPowerProfileChanged.
            //
            LOG_D("MPPnPEventNotify: NdisDevicePnPEventPowerProfileChanged");

            if(InformationBufferLength == sizeof(NDIS_POWER_PROFILE)) {
                NdisPowerProfile = (PNDIS_POWER_PROFILE)InformationBuffer;
                if(*NdisPowerProfile == NdisPowerProfileBattery) {
                    LOG_D("The host system is running on battery power");
                }
                if(*NdisPowerProfile == NdisPowerProfileAcOnLine) {
                    LOG_D("The host system is running on AC power");
               }
            }
            break;

        default:
            LOG_D("MPPnPEventNotify: unknown PnP event %x ", PnPEvent);
            break;
    }

    LOG_D("<--- MPPnPEventNotify");

}

NDIS_STATUS
MPSetOptions(
    NDIS_HANDLE  NdisDriverHandle,
    NDIS_HANDLE  DriverContext
    )
/*++

Routine Description:

    MiniportSetOptions function is called by NDIS to ask the
    miniport driver to register its optional handlers.
    A miniport driver must call NdisSetOptionalHandler 
    from its MiniportSetOptions function to register its
    optional handlers with NDIS.

Arguments:


Return Value:

    None

--*/
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    
    MpEntry;

    UNREFERENCED_PARAMETER(NdisDriverHandle);
	UNREFERENCED_PARAMETER(DriverContext);
    
    MpExit;
    
    return ndisStatus;
}

NDIS_STATUS
MPRequest(
    __in  NDIS_HANDLE         MiniportAdapterContext,
    __in  PNDIS_OID_REQUEST   NdisRequest
    )
/*++
Routine Description:

    NDIS calls a miniport driver's MiniportOidRequest function to send an OID
    request to the driver. These OID requests are serialized so the
    MiniportOidRequest function is not called until the pending OID request
    has completed. MiniportOidRequest can be called at either IRQL 
    PASSIVE_LEVEL or IRQL DISPATCH_LEVEL. 
    
    If the MiniportOidRequest function returns NDIS_STATUS_PENDING, 
    the miniport driver must subsequently call the NdisMOidRequestComplete
    function to complete the request. NdisMOidRequestComplete can be called 
    at either IRQL DISPATCH_LEVEL or IRQL PASSIVE_LEVEL. 

Arguments:

    MiniportAdapterContext  Pointer to the adapter structure
    NdisRequest             Pointer to NDIS_OID_REQUEST
    
Return Value:
    
    NDIS_STATUS_SUCCESS
    NDIS_STATUS_NOT_SUPPORTED
    NDIS_STATUS_BUFFER_TOO_SHORT
    
--*/
{
    PADAPTER        pAdapter = (PADAPTER)   MiniportAdapterContext;
    NDIS_STATUS     ndisStatus;
	nic_info_st *pnic_info = pAdapter->nic_info;

    // If the adapter has been surprise removed, fail request
    if (WF_CANNOT_RUN(pnic_info)) {
        ndisStatus = NDIS_STATUS_FAILURE;
        LOG_D("NdisRequest failed as surprise removal is in progress");
        return ndisStatus;
    }

    // Assume by default, the request will pend. Covers the worst case
    pAdapter->PendedRequest = NdisRequest;

    switch(NdisRequest->RequestType)
    {
        case NdisRequestQueryInformation:
        case NdisRequestQueryStatistics:
            ndisStatus = MpQueryInformation(
                            pAdapter,
                            NdisRequest->DATA.QUERY_INFORMATION.Oid,
                            NdisRequest->DATA.QUERY_INFORMATION.InformationBuffer,
                            NdisRequest->DATA.QUERY_INFORMATION.InformationBufferLength,
                            (PULONG)&NdisRequest->DATA.QUERY_INFORMATION.BytesWritten,
                            (PULONG)&NdisRequest->DATA.QUERY_INFORMATION.BytesNeeded
                            );
            break;

        case NdisRequestSetInformation:
            ndisStatus = MpSetInformation(
                            pAdapter,
                            NdisRequest->DATA.SET_INFORMATION.Oid,
                            NdisRequest->DATA.SET_INFORMATION.InformationBuffer,
                            NdisRequest->DATA.SET_INFORMATION.InformationBufferLength,
                            (PULONG)&NdisRequest->DATA.SET_INFORMATION.BytesRead,
                            (PULONG)&NdisRequest->DATA.SET_INFORMATION.BytesNeeded
                            );
            break;

        case NdisRequestMethod:
            ndisStatus = MpQuerySetInformation(
                            pAdapter,
                            NdisRequest->DATA.METHOD_INFORMATION.Oid,
                            NdisRequest->DATA.METHOD_INFORMATION.InformationBuffer,
                            NdisRequest->DATA.METHOD_INFORMATION.InputBufferLength,
                            NdisRequest->DATA.METHOD_INFORMATION.OutputBufferLength,
                            NdisRequest->DATA.METHOD_INFORMATION.MethodId,
                            (PULONG)&NdisRequest->DATA.METHOD_INFORMATION.BytesWritten,
                            (PULONG)&NdisRequest->DATA.METHOD_INFORMATION.BytesRead,
                            (PULONG)&NdisRequest->DATA.METHOD_INFORMATION.BytesNeeded
                            );
            break;

        default:
            ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
            break;
    }
    
    if (ndisStatus != NDIS_STATUS_PENDING)
    {
        // Request has completed
        pAdapter->PendedRequest = NULL;
    }
    else
    {
        LOG_D("Request has been pended. Will complete asynchronously");
    }
    return ndisStatus;
}

VOID MPCancelRequest(
    __in NDIS_HANDLE hMiniportAdapterContext,
    __in PVOID       RequestId
)
/*++
Routine Description:

    An NDIS 6.0 miniport driver must provide a MiniportCancelOidRequest
    function if it pends any OID request. MiniportCancelOidRequest 
    is called when NDIS or an overlying driver calls NdisCancelOidRequest 
    or NdisFCancelOidRequest. If the miniport driver finds the OID 
    request to be cancelled in its queue, it should complete the OID request
    by calling NdisMOidRequestComplete with NDIS_STATUS_REQUEST_ABORTED.

Arguments:

    MiniportAdapterContext  Pointer to the adapter structure
    RequestId               Specify the request to be cancelled.
    
Return Value:
    
--*/
{
    UNREFERENCED_PARAMETER(hMiniportAdapterContext);
    UNREFERENCED_PARAMETER(RequestId);

}




VOID MPAdapterShutdown(
    __in  NDIS_HANDLE             MiniportAdapterContext,
    __in  NDIS_SHUTDOWN_ACTION    ShutdownAction
    )
/*++

Routine Description:

    The MiniportShutdown handler restores a NIC to its initial state when
    the system is shut down, whether by the user or because an unrecoverable
    system error occurred. This is to ensure that the NIC is in a known
    state and ready to be reinitialized when the machine is rebooted after
    a system shutdown occurs for any reason, including a crash dump.

    Here just disable the interrupt and stop the DMA engine.
    Do not free memory resources or wait for any packet transfers
    to complete.


    Runs at an arbitrary IRQL <= DIRQL. So do not call any passive-level
    function.

Arguments:

    MiniportAdapterContext  Pointer to our adapter

Return Value:

    None

--*/
{
    PADAPTER        pAdapter = (PADAPTER) MiniportAdapterContext;
//    UNREFERENCED_PARAMETER(MiniportAdapterContext);
    UNREFERENCED_PARAMETER(ShutdownAction);

    //
    // TODO: Any place where we are reading registers and making major
    // decisions should consider protecting against FFFF for surprise 
    // removal case
    //

    MpEntry;

    if (MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED) == FALSE) {
        //
        // NOTe: PCI Disable Interrupts only if adapter has not been removed
        //
        //
        // Issue a shutdown to the NIC. NIC should go into a known state
        // and shut off power to the antenna. If surprise removal has
        // occurred, we will not do this.
        //

        //TODO: Halt Nic here! (necessary). 2021/03/03
        
        HwBusStopNotification(pAdapter);
    }       

    MpExit;
}

VOID
MPDevicePnPEvent(
    __in NDIS_HANDLE                  MiniportAdapterContext,
    __in PNET_DEVICE_PNP_EVENT        NetDevicePnPEvent
    )
{
    PADAPTER            pAdapter = (PADAPTER) MiniportAdapterContext;
    NDIS_DEVICE_PNP_EVENT        DevicePnPEvent = NetDevicePnPEvent->DevicePnPEvent;
    PVOID                        InformationBuffer = NetDevicePnPEvent->InformationBuffer;
    ULONG                        InformationBufferLength = NetDevicePnPEvent->InformationBufferLength;

    MpEntry;

    switch (DevicePnPEvent)
    {
        case NdisDevicePnPEventQueryRemoved:
            LOG_D("MPDevicePnPEventNotify: NdisDevicePnPEventQueryRemoved");
            break;

        case NdisDevicePnPEventRemoved:
            LOG_D("MPDevicePnPEventNotify: NdisDevicePnPEventRemoved");
            break;       

        case NdisDevicePnPEventSurpriseRemoved:
            LOG_D("MPDevicePnPEventNotify: NdisDevicePnPEventSurpriseRemoved");
            MpOnSurpriseRemoval(pAdapter);
            break;

        case NdisDevicePnPEventQueryStopped:
            LOG_D("MPDevicePnPEventNotify: NdisDevicePnPEventQueryStopped");
            break;

        case NdisDevicePnPEventStopped:
            LOG_D("MPDevicePnPEventNotify: NdisDevicePnPEventStopped");
            break;      
            
        case NdisDevicePnPEventPowerProfileChanged:
            LOG_D("MPDevicePnPEventNotify: NdisDevicePnPEventPowerProfileChanged");
            break;      
            
        default:
            LOG_D("MPDevicePnPEventNotify: unknown PnP event %x ", DevicePnPEvent);
            MpExit;
            return;
    }

	#if 0
    //
    // This is a valid PnPEvent. Notify Hw11 about it.
    //
    Hw11DevicePnPEvent(
        pAdapter->Nic,
        DevicePnPEvent,
        InformationBuffer,
        InformationBufferLength
        );
	#endif
	
    MpExit;
}


WDFDEVICE
Mp11GetWdfDevice(
    __in  PADAPTER        pAdapter
    )
{
    return   pAdapter->WdfDevice;
}


/**
 * This routine resets the miniport. As part of reset, the NIC is asked to stop its
 * interrupts, send and receive units (and any other units NIC feels should be reset).
 * All submitted TX_MSDUs that have not yet been sent are failed. Any packets waiting in
 * the TxQueue are also failed.
 *
 * \param pAdapter      The adapter being reset
 * \param ResetType     The type of reset to perform 
 * \sa MpReset, MpNdisResetWorkItem
 */
__drv_requiresIRQL(DISPATCH_LEVEL)
NDIS_STATUS
MpResetInternalRoutine(
    __in PADAPTER             pAdapter,
    __in MP_RESET_TYPE        ResetType
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    MpEntry;
#if 0

    do
    {
        //
        // Note : we release the send lock. Because Stop Notificaton causes the WriteCOmpletionROutine to 
        // be invoked for cancelled IO which call's the MP Tx completion handler and in turn acquires the same lock . 
        //
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);    
        HwBusStopNotification(pAdapter);
        //
        // Issue a reset start to the NIC. Nic should suspend transmit and receive units
        // among other things.
        // NOTE: For USB, this requires sending sync. request to the device hence acquire the lock later.
        //
        //ndisStatus = Hw11ResetStep2(pAdapter->Nic);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            LOG_E("An error 0x%08x occured during Hw11ResetStart. Will attempt to continue", ndisStatus);
            if (ndisStatus != NDIS_STATUS_SOFT_ERRORS)
            {
                LOG_E("Unable to recover from the error. Aborting reset request");
                break;
            }
        }

        MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);      
        //
        // Notify CustomInterfaces about the reset operation
        //
        MpEventMiniportReset(pAdapter); 

        //
        // Ask the station to reset
        //
        //Sta11ResetStep2(pAdapter->Station, ResetType);

        //
        // Try to free the unsent packets that have been submitted to the hardware
        //
        MpCompleteQueuedTxMSDUs(pAdapter);

        //
        // Free the packets that are waiting in the TxQueue.
        //
        MpCompleteQueuedNBL(pAdapter);

        //
        // Reintialize the send engine to bring to init state
        //
        MpReinitializeSendEngine(pAdapter);
       
        MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    } WHILE(FALSE);

    //
    // Everything has been cleaned up, we dont need the lock to reset the hardware
    //

    //
    // Reset Step 3 - Reinitialize MAC and PHY state
    //
    //ndisStatus = Hw11ResetStep3(pAdapter->Nic, ResetType);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        LOG_E("Failure occured while resetting the NIC. Reset Type: %d, Error: 0x%08x", ResetType, ndisStatus);

        HwBusStartDevice(pAdapter);     
        MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
        return ndisStatus;  // Send lock held
    }

    //
    // Reset Step 4 - Restart anything that may have been stopped
    //
       
    //
    // Issue a reset end to the NIC. Hw11 should reinitialize all the Rx descriptors
    // and restart send/receive units among other things.  
    // NOTE: For USB, this requires sending sync. request to the device hence acquire the lock later.
    //
    //ndisStatus = Hw11ResetStep4(pAdapter->Nic);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        LOG_E("An error 0x%08x occured during Hw11ResetStep4.", ndisStatus);
    }
    //Hw11EnableNotification(pAdapter->Nic);

    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
#endif
    MpExit;
    return ndisStatus;
}


/**
 * This is the work item in which NDIS reset operation will be executed if
 * it was pended.
 *
 * This routine waits for any active senders doing send to complete. When
 * they do complete, MpResetInternalRoutine is invoked which executes the
 * actual reset of the miniport driver and the hardware.
 * 
 * \param Context       The context we registered when initializing the NIC
 * \sa MpReset, MpResetInternalRoutine
 */
VOID
MpNdisResetWorkItem(
    __in WDFWORKITEM  WorkItem
    )
{
    NDIS_STATUS     ndisStatus;
    PADAPTER        pAdapter ;
    PADAPTER_WORKITEM_CONTEXT adapterWorkItemContext;

    adapterWorkItemContext = GetAdapterWorkItemContext(WorkItem);
    pAdapter = adapterWorkItemContext->Adapter;

    //
    // This lock synchronizes between Reset/Pause/Restart
    //
    MP_ACQUIRE_RESET_PNP_LOCK(pAdapter);

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
    HwBusStopNotification(pAdapter);

    //
    // Reset Step 1 - Wait for things to cleanup
    //

    //
    // Dont redo the work that is already done by
    // the Dot11Reset/Pause routines (protected by PNP lock)
    //
    if (MP_INCREMENT_RESET_PAUSE_COUNT(pAdapter) == 1)
    {
        //
        // Remove the Active Sender count we added during Initialize
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
            NdisMSleep(10 * 1000);  // 10 sec
        }
    }
    
    //
    // Have the station reset
    //
    //Sta11ResetStep1(pAdapter->Station, MP_RESET_TYPE_NDIS_RESET);

    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);
    ndisStatus = MpResetInternalRoutine(pAdapter, MP_RESET_TYPE_NDIS_RESET);

    //
    // Reset the variables that may have potentially caused the reset to occur
    //
    pAdapter->TotalTransmitsSnapShot = 0;
    pAdapter->NumStalledSendTicks = 0;

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

        //
        // Place the Ref Count back on ActiveReceivers.
        //
        MP_INCREMENT_ACTIVE_RECEIVERS(pAdapter);
    }

    //
    // After NdisReset, the station may reassociate if it was already
    // associated
    //
    //Sta11ResetStep4(pAdapter->Station, MP_RESET_TYPE_NDIS_RESET);

    if (ndisStatus == NDIS_STATUS_SUCCESS || ndisStatus == NDIS_STATUS_SOFT_ERRORS)
    {
        //
        // Reset completed successfully. Soft errors are recoverable.
        // Reset the NumResetAttempts counter and clear any error flags if set.
        //
        pAdapter->NumResetAttempts = 0;
        MP_CLEAR_STATUS_FLAG(pAdapter, (MP_ADAPTER_RESET_IN_PROGRESS | MP_ADAPTER_HARDWARE_ERROR | MP_ADAPTER_NON_RECOVER_ERROR));
        ndisStatus = NDIS_STATUS_SUCCESS;
    }
    else
    {
        //
        // Reset did not occur successfully.
        //
        if (pAdapter->NumResetAttempts < MP_RESET_ATTEMPTS_THRESHOLD)
        {
            //
            // Another attempt failed. Will try again later.
            //
            pAdapter->NumResetAttempts++;
        }
        else
        {
            //
            // Repeated attempts to reset have failed. Remove this adapter.
            //
            MpRemoveAdapter(pAdapter);
        }

        //
        // Reset completed but not successfully. Leave error bits on
        //
        MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_RESET_IN_PROGRESS);
    }

    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
    
    MP_RELEASE_RESET_PNP_LOCK(pAdapter);

    //
    // notify NDIS that reset is complete
    //
    NdisMResetComplete(
        pAdapter->MiniportAdapterHandle,
        ndisStatus,
        HW_REQUIRES_ADDRESS_RESET
        );
}

VOID
Mp11InternalReset(
    __in  PADAPTER        pAdapter,
    __in  MP_RESET_TYPE   ResetType
    )
{
    MP_ACQUIRE_SEND_LOCK(pAdapter, FALSE);    
    MpResetInternalRoutine(pAdapter, ResetType);
    MP_RELEASE_SEND_LOCK(pAdapter, FALSE);
}

VOID
Mp11IndicateStatusIndication(
    __in  PADAPTER        pAdapter,
    __in  PNDIS_STATUS_INDICATION pStatusIndication
    )
{
    NdisMIndicateStatusEx(
        pAdapter->MiniportAdapterHandle,
        pStatusIndication
        );
}


#define HW_REG_DOMAIN_MKK1          0x00000041
#define HW_REG_DOMAIN_ISRAEL        0x00000042

#define cESS   (0x01)

#define FillOctetString(_os,_octet,_len)        \
    (_os).Octet=(PUCHAR)(_octet);                   \
    (_os).Length=(_len)


static NDIS_STATUS
StaGetAlgorithmPair(
    __in  PADAPTER                    adapter,
    __in  STA_QUERY_ALGO_PAIR_FUNC    QueryFunction,
    __deref_out_opt PDOT11_AUTH_CIPHER_PAIR    *AlgoPairs,
    __out PULONG                      NumAlgoPairs
    )
{
    DOT11_AUTH_CIPHER_PAIR_LIST     CipherPairList;
    PDOT11_AUTH_CIPHER_PAIR_LIST    FullPairList;
    NDIS_STATUS                     ndisStatus;
    ULONG                           size;

    *AlgoPairs = NULL;
    *NumAlgoPairs = 0;

    //
    // First get the total size of the algorithm pair list.
    //
    ndisStatus = (*QueryFunction)(adapter, &CipherPairList, sizeof(CipherPairList));
    if (ndisStatus != NDIS_STATUS_SUCCESS && ndisStatus != NDIS_STATUS_BUFFER_OVERFLOW)
    {
        return ndisStatus;
    }

    // Integer overflow
    if (FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) > 
            FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) + 
            CipherPairList.uTotalNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR))
    {
        return NDIS_STATUS_FAILURE;
    }

    size = FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) +
           CipherPairList.uTotalNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR);

    MP_ALLOCATE_MEMORY(adapter->MiniportAdapterHandle, 
                       &FullPairList,
                       size,
                       'SltR');
    if (FullPairList == NULL)
    {
        return NDIS_STATUS_RESOURCES;
    }

    //
    // Get the size of the list and copy the algorithm pair list data. Note that we over-allocated a little
    // bit for convenience.
    //

    ndisStatus = (*QueryFunction)(adapter, FullPairList, size);
    ASSERT(ndisStatus == NDIS_STATUS_SUCCESS && FullPairList->uTotalNumOfEntries == FullPairList->uNumOfEntries);

    *AlgoPairs = (PDOT11_AUTH_CIPHER_PAIR) FullPairList;
    *NumAlgoPairs = FullPairList->uNumOfEntries;

    //
    // Copy the algorithm pair to the beginning of the allocated buffer. Note that we cannot
    // use NdisMoveMemory as the source and destination overlap.
    //
    RtlMoveMemory(FullPairList,
                  FullPairList->AuthCipherPairs,
                  FullPairList->uNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR));

    return NDIS_STATUS_SUCCESS;
}


/**
 * This function is written with reference to the previous function HwInitializeVariable.
 * Just initialize NIC related information required by the OS. 
 */
NDIS_STATUS
MpInitializeNicVariable(__in  PADAPTER pAdapter)
{
	NDIS_STATUS ndisStatus;
	ULONG       size;
	ndisStatus = NDIS_STATUS_RESOURCES;
	USHORT    Index;
	NDIS_HANDLE MiniportAdapterHandle = pAdapter->MiniportAdapterHandle;

	pAdapter->DupCurrentInfo.ShortRetryLimit = 7;
	pAdapter->DupCurrentInfo.LongRetryLimit = 7;
	pAdapter->DupCurrentInfo.EarlyRxThreshold = 7;
    pAdapter->MibInfo.BSSType = dot11_BSS_type_infrastructure;

    __try
	{


		size = sizeof(DOT11_SUPPORTED_PHY_TYPES) + (MAX_NUM_PHY_TYPES - 1) * sizeof(DOT11_PHY_TYPE);
		MP_ALLOCATE_MEMORY(MiniportAdapterHandle,
			&(pAdapter->MibInfo.pSupportedPhyTypes),
			size,
			HW11_MEMORY_TAG);
		if (pAdapter->MibInfo.pSupportedPhyTypes == NULL) {
			LOG_E("Failed to allocate memory for NIC->MibInfo.pSupportedPhyTypes");
			__leave;
		}
		NdisZeroMemory(pAdapter->MibInfo.pSupportedPhyTypes, size);

		size = sizeof(DOT11_REG_DOMAINS_SUPPORT_VALUE) +
			(MAX_NUM_DOT11_REG_DOMAINS_VALUE - 1) * sizeof(DOT11_REG_DOMAIN_VALUE);
		MP_ALLOCATE_MEMORY(MiniportAdapterHandle,
			&(pAdapter->MibInfo.pRegDomainsSupportValue),
			size,
			HW11_MEMORY_TAG);

		if (pAdapter->MibInfo.pRegDomainsSupportValue == NULL) {
			LOG_E("Failed to allocate memory for NIC->MibInfo.pRegDomainsSupportValue");
			__leave;
		}
		NdisZeroMemory(pAdapter->MibInfo.pRegDomainsSupportValue, size);

		size = sizeof(DOT11_DIVERSITY_SELECTION_RX_LIST) +
			(MAX_NUM_DIVERSITY_SELECTION_RX_LIST - 1) * sizeof(DOT11_DIVERSITY_SELECTION_RX);
		MP_ALLOCATE_MEMORY(MiniportAdapterHandle,
			&(pAdapter->MibInfo.pDiversitySelectionRxList),
			size,
			HW11_MEMORY_TAG);
		if (pAdapter->MibInfo.pDiversitySelectionRxList == NULL) {
			LOG_E("Failed to allocate memory for NIC->MibInfo.pDiversitySelectionRxList");
			__leave;
		}

		NdisZeroMemory(pAdapter->MibInfo.pDiversitySelectionRxList, size);

		ndisStatus = NDIS_STATUS_SUCCESS;
	}
	__finally
	{
	}
	wf_mib_info_t *MibInfo = &pAdapter->MibInfo;
	ULONG curPhyId;
	//
	// management info
	//
	RtlCopyMemory((PCHAR)MibInfo->SSID, "Realtek AP", sizeof("Realtek AP"));
	FillOctetString(MibInfo->SupportedRates, MibInfo->SupportedRatesBuf, 0);
	FillOctetString(MibInfo->Regdot11OperationalRateSet, MibInfo->Regdot11OperationalRateBuf, 0);
	MibInfo->pSupportedPhyTypes->uNumOfEntries = 0;
	MibInfo->pSupportedPhyTypes->uTotalNumOfEntries = MAX_NUM_PHY_TYPES;
	//Set operation mode.
	MibInfo->CurrentOperationMode.uCurrentOpMode = DOT11_OPERATION_MODE_EXTENSIBLE_STATION;
	MibInfo->CurrentOperationMode.uReserved = 0;

	MibInfo->OperationModeCapability.uOpModeCapability = DOT11_OPERATION_MODE_EXTENSIBLE_STATION
		| DOT11_OPERATION_MODE_NETWORK_MONITOR;
	//G mode	To do:need to determine which wirelessmode
	{
		MibInfo->pSupportedPhyTypes->uNumOfEntries++;
		curPhyId = MibInfo->pSupportedPhyTypes->uNumOfEntries - 1;
		MibInfo->pSupportedPhyTypes->dot11PHYType[curPhyId] = dot11_phy_type_erp;
		MibInfo->PhyMIB[curPhyId].PhyID = 0;
		MibInfo->PhyMIB[curPhyId].PhyType = dot11_phy_type_erp; //6
	
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[0] = 0x2;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[1] = 0x4;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[2] = 0xB;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[3] = 0x16;// 0x0c;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[4] = 0x0c;// 0x12;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[5] = 0x12;// 0x16;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[6] = 0x18;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[7] = 0x24;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[8] = 0x30;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[9] = 0x48;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[10] = 0x60;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[11] = 0x6c;

		MibInfo->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength = 12;
		MibInfo->PhyMIB[curPhyId].BasicRateSet.uRateSetLength = 12;
		//
		NdisZeroMemory(
			&(MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue),
			sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2));
		NdisMoveMemory(MibInfo->PhyMIB[curPhyId].BasicRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].BasicRateSet.uRateSetLength);
		NdisMoveMemory(MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedTxDataRatesValue,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].BasicRateSet.uRateSetLength);
		NdisMoveMemory(MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedRxDataRatesValue,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].BasicRateSet.uRateSetLength);
	}
	//B mode
	{
		MibInfo->pSupportedPhyTypes->uNumOfEntries++;
		curPhyId = MibInfo->pSupportedPhyTypes->uNumOfEntries - 1;
		MibInfo->pSupportedPhyTypes->dot11PHYType[curPhyId] = dot11_phy_type_hrdsss;
		MibInfo->PhyMIB[curPhyId].PhyID = curPhyId;
		MibInfo->PhyMIB[curPhyId].PhyType = dot11_phy_type_hrdsss; //5
		
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength = 4;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[0] = 2;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[1] = 4;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[2] = 11;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[3] = 22;

		NdisZeroMemory(
			&(MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue),
			sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2));
		NdisMoveMemory(MibInfo->PhyMIB[curPhyId].BasicRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].BasicRateSet.uRateSetLength);
		NdisMoveMemory(
			MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedTxDataRatesValue,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
		NdisMoveMemory(
			MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedRxDataRatesValue,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
	}
	//A mode	
	{	
		MibInfo->pSupportedPhyTypes->uNumOfEntries++;
		curPhyId = MibInfo->pSupportedPhyTypes->uNumOfEntries - 1;
		MibInfo->pSupportedPhyTypes->dot11PHYType[curPhyId] = dot11_phy_type_ofdm;
		MibInfo->PhyMIB[curPhyId].PhyID = curPhyId;
		MibInfo->PhyMIB[curPhyId].PhyType = dot11_phy_type_ofdm; //4

		MibInfo->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength = 8;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[0] = 12;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[1] = 18;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[2] = 24;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[3] = 36;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[4] = 48;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[5] = 72;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[6] = 96;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[7] = 108;

		NdisZeroMemory(
			&(MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue),
			sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2));
		NdisMoveMemory(
			MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedTxDataRatesValue,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
		NdisMoveMemory(
			MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedRxDataRatesValue,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
	}
	//N mode
	{
		MibInfo->pSupportedPhyTypes->uNumOfEntries++;
		curPhyId = MibInfo->pSupportedPhyTypes->uNumOfEntries - 1;
		MibInfo->pSupportedPhyTypes->dot11PHYType[curPhyId] = dot11_phy_type_ht;
		MibInfo->PhyMIB[curPhyId].PhyID = curPhyId;
		MibInfo->PhyMIB[curPhyId].PhyType = dot11_phy_type_ht; //6
	
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength = 16;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[0] = 30;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[1] = 60;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[2] = 90;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[3] = 120;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[4] = 180;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[5] = 240;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[6] = (UCHAR)270;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[7] = (UCHAR)300;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[8] = (UCHAR)60;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[9] = (UCHAR)120;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[10] = (UCHAR)180;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[11] = (UCHAR)240;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[12] = (UCHAR)360;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[13] = (UCHAR)480;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[14] = (UCHAR)540;
		MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[15] = (UCHAR)600;

		NdisZeroMemory(
			&(MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue),
			sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2));
		NdisMoveMemory(
			MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedTxDataRatesValue,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
		NdisMoveMemory(
			MibInfo->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedRxDataRatesValue,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			MibInfo->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);	//
	}

#if 1
	//pAdapter->RxInfo.ReceiveProcessingFunction = HwProcessReceiveSTAMode;

	pAdapter->DupCurrentInfo.RTS_Threshold = 2346;	   //2346 vs 2347	 dot11RtsThreshold

	pAdapter->DupCurrentInfo.Frag_Threshold = 2346;// 2346 vs 2400  FragThreshold
#endif
	MibInfo->PhyMIB[0].Channel = 11;  //==> Regdot11ChannelNumber
	MibInfo->DtimPeriod = 2;    

	pAdapter->DupCurrentInfo.BeaconInterval = 100;	 //==> pMgntInfo->Regdot11BeaconPeriod 

	MibInfo->CapInfo = cESS |  cIBSS /*|cShortPreamble*/;	   //=> RegmCap mitesh cESS | cIBSS  vs    cESS


	MibInfo->bScanCompleteIndicated = FALSE ;
	MibInfo->bScanInProgress = FALSE;

	MibInfo->uMPDUMaxLength = MAX_MPDU_LENGTH;

	MibInfo->EDThreshold = -65;
	MibInfo->ShortSlotTimeOptionEnabled = FALSE;
#if 1
	pAdapter->DupCurrentInfo.MediaOccupancyLimit = 100;
	pAdapter->DupCurrentInfo.CFPPeriod = 255;
	pAdapter->DupCurrentInfo.CFPMaxDuration = 100;

	pAdapter->DupCurrentInfo.MaxTxLifeTime = 512;
	pAdapter->DupCurrentInfo.MaxRxLifeTime = 512;
	pAdapter->DupCurrentInfo.DefaultBeaconRate = 2;		  // Set default beacon rate to 2 Mbps

#endif
	MibInfo->SwChnlInProgress = FALSE;

	MibInfo->PowerMgmtMode.dot11PowerMode = dot11_power_mode_active;
	MibInfo->PowerMgmtMode.usAID = 0;
	//NdisAllocateSpinLock(&Nic->ManagementInfo.PowerMgmtLock);

	MibInfo->AtimWindow = 0;    /* set only when power management is enabled */

	MibInfo->DtimCount = MibInfo->DtimPeriod - 1;

	MibInfo->NextBeaconIndex = 0;

	MibInfo->Tim.BitmapCtrl = MibInfo->Tim.PartialVirtualBitmap[0] = 0;

#if 0
	// These variable should initialize from registry
	for (Index = 0; Index < DUPLICATE_DETECTION_CACHE_LENGTH; Index++) {
		NdisZeroMemory(&Nic->RxInfo.DupePacketCache[Index], sizeof(DUPE_CACHE_ENTRY));
	}
	Nic->RxInfo.NextDupeCacheIndex = 0;


	if (Nic->DupCurrentInfo.EarlyRxThreshold == 7)				   // Turn OnlyErlPkt when early Rx is turn off
		Nic->RxInfo.ReceiveConfig |= RCR_OnlyErlPkt;
#endif

	MibInfo->CCAModeSupported = DOT11_CCA_MODE_CS_ONLY;
	MibInfo->CurrentCCAMode = DOT11_CCA_MODE_CS_ONLY;

	MibInfo->OffloadCapability.uReserved = 0;
	MibInfo->OffloadCapability.uFlags = 0;
	MibInfo->OffloadCapability.uSupportedWEPAlgorithms = 0;
	MibInfo->OffloadCapability.uNumOfReplayWindows = 0;
	MibInfo->OffloadCapability.uMaxWEPKeyMappingLength = 0;
	MibInfo->OffloadCapability.uSupportedAuthAlgorithms = 0;
	MibInfo->OffloadCapability.uMaxAuthKeyMappingLength = 0;

	MibInfo->CurrentOffloadCapability.uReserved = 0;
	MibInfo->CurrentOffloadCapability.uFlags = 0;

	MibInfo->OperationModeCapability.uReserved = 0;
	MibInfo->OperationModeCapability.uMajorVersion = MP_OPERATION_MODE_CAPABILITY_MAJOR_VERSION;
	MibInfo->OperationModeCapability.uMinorVersion = MP_OPERATION_MODE_CAPABILITY_MINOR_VERSION;
	// We can buffer as many packets as OS sends.
	MibInfo->OperationModeCapability.uNumOfTXBuffers = 0;
	MibInfo->OperationModeCapability.uNumOfRXBuffers = 64;

	MibInfo->OptionalCapability.uReserved = 0;
	MibInfo->OptionalCapability.bDot11PCF = FALSE;
	MibInfo->OptionalCapability.bDot11PCFMPDUTransferToPC = FALSE;
	MibInfo->OptionalCapability.bStrictlyOrderedServiceClass = FALSE;

	MibInfo->CurrentOptionalCapability.uReserved = 0;
	MibInfo->CurrentOptionalCapability.bDot11CFPollable = FALSE;
	MibInfo->CurrentOptionalCapability.bDot11PCF = FALSE;
	MibInfo->CurrentOptionalCapability.bDot11PCFMPDUTransferToPC = FALSE;
	MibInfo->CurrentOptionalCapability.bStrictlyOrderedServiceClass = FALSE;

#if 0
	MibInfo->pSupportedPhyTypes->dot11PHYType[0] = dot11_phy_type_erp;
	MibInfo->pSupportedPhyTypes->dot11PHYType[1] = dot11_phy_type_hrdsss;
	MibInfo->pSupportedPhyTypes->dot11PHYType[2] = dot11_phy_type_ht;
	MibInfo->PhyMIB[0].PhyType = dot11_phy_type_erp;
	MibInfo->PhyMIB[1].PhyType = dot11_phy_type_hrdsss;
	MibInfo->PhyMIB[2].PhyType = dot11_phy_type_ht;
#endif

	for (Index = 4; Index < MAX_NUM_SUPPORTED_RATES_V2; Index++) {
		MibInfo->PhyMIB[0].SupportedDataRatesValue.ucSupportedTxDataRatesValue[Index] = 0;
		MibInfo->PhyMIB[0].SupportedDataRatesValue.ucSupportedRxDataRatesValue[Index] = 0;
	}

	MibInfo->CurrentBeaconRate = 2 * 2;	  // 1Mbps = 2, 2Mbps = 4
	MibInfo->CurrentTXDataRate = 4;		  // Set default data rate to 2 Mbps

	MibInfo->OperatingPhyId = 0;
	MibInfo->SelectedPhyId = 0;
	MibInfo->OperatingPhyMIB = MibInfo->PhyMIB;
	MibInfo->SelectedPhyMIB = MibInfo->PhyMIB;

	MibInfo->DiversitySupport = dot11_diversity_support_dynamic;

	MibInfo->SupportedPowerLevels.uNumOfSupportedPowerLevels = 4;
	MibInfo->SupportedPowerLevels.uTxPowerLevelValues[0] = 10;
	MibInfo->SupportedPowerLevels.uTxPowerLevelValues[1] = 20;
	MibInfo->SupportedPowerLevels.uTxPowerLevelValues[2] = 30;
	MibInfo->SupportedPowerLevels.uTxPowerLevelValues[3] = 50;
	MibInfo->CurrentTxPowerLevel = 1;	// 1 based

	MibInfo->pRegDomainsSupportValue->uNumOfEntries = 7;
	MibInfo->pRegDomainsSupportValue->uTotalNumOfEntries = MAX_NUM_DOT11_REG_DOMAINS_VALUE;
	for (Index=0; Index < MibInfo->pRegDomainsSupportValue->uNumOfEntries; Index++)
		MibInfo->pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex = Index;

	MibInfo->pRegDomainsSupportValue->dot11RegDomainValue[0].uRegDomainsSupportValue = DOT11_REG_DOMAIN_OTHER;
	MibInfo->pRegDomainsSupportValue->dot11RegDomainValue[1].uRegDomainsSupportValue = DOT11_REG_DOMAIN_FCC;
	MibInfo->pRegDomainsSupportValue->dot11RegDomainValue[2].uRegDomainsSupportValue = DOT11_REG_DOMAIN_DOC;
	MibInfo->pRegDomainsSupportValue->dot11RegDomainValue[3].uRegDomainsSupportValue = DOT11_REG_DOMAIN_ETSI;
	MibInfo->pRegDomainsSupportValue->dot11RegDomainValue[4].uRegDomainsSupportValue = DOT11_REG_DOMAIN_SPAIN;
	MibInfo->pRegDomainsSupportValue->dot11RegDomainValue[5].uRegDomainsSupportValue = DOT11_REG_DOMAIN_FRANCE;
	MibInfo->pRegDomainsSupportValue->dot11RegDomainValue[6].uRegDomainsSupportValue = DOT11_REG_DOMAIN_MKK;
	// These two are supported, but the OS doesnt recognize these and hence we wont report these to the OS (NumOfEntries = 7)
	MibInfo->pRegDomainsSupportValue->dot11RegDomainValue[7].uRegDomainsSupportValue = HW_REG_DOMAIN_MKK1;
	MibInfo->pRegDomainsSupportValue->dot11RegDomainValue[8].uRegDomainsSupportValue = HW_REG_DOMAIN_ISRAEL;

	MibInfo->pDiversitySelectionRxList->uNumOfEntries = 2; 
	MibInfo->pDiversitySelectionRxList->uTotalNumOfEntries = MAX_NUM_DIVERSITY_SELECTION_RX_LIST;
	MibInfo->pDiversitySelectionRxList->dot11DiversitySelectionRx[0].uAntennaListIndex = 1;
	MibInfo->pDiversitySelectionRxList->dot11DiversitySelectionRx[0].bDiversitySelectionRX = TRUE;
	MibInfo->pDiversitySelectionRxList->dot11DiversitySelectionRx[1].uAntennaListIndex = 2;
	MibInfo->pDiversitySelectionRxList->dot11DiversitySelectionRx[1].bDiversitySelectionRX = TRUE;

	MibInfo->DelaySleepValue = 0;

	//NdisAllocateSpinLock(&Nic->ManagementInfo.PhyMIBConfigLock);
	MibInfo->bDeviceConnected = FALSE;
	
	// initialization for rate negotiation
	MibInfo->TotalRetryCount = 0;
	MibInfo->PacketsSentForTxRateCheck = 0;
	MibInfo->PrevTxDataRate = 0;
	MibInfo->TxRateIncreaseWaitCount = 0;
	MibInfo->TxRateIncreaseWaitRequired = 1;
	
	// initialization for g-mode protection
	MibInfo->bEnableSendCTSToSelf = FALSE;
	{
		pAdapter->MibInfo.CurrentBSSType = dot11_BSS_type_infrastructure;
		pAdapter->MibInfo.SelectedPhyId = 0;
		pAdapter->MibInfo.SelectedPhyMIB = pAdapter->MibInfo.PhyMIB;
	}
	return ndisStatus;
}

/**
* This function is to set NIC hardware attributes which will be sent to NDIS.
*/
NDIS_STATUS SetNic80211Attributes(__in  PADAPTER pAdapter,
	__out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr)
{
#if 0
	PDOT11_PHY_ATTRIBUTES   PhyAttr;
	ULONG                   PhyId;
	ULONG                   index;
	UCHAR                   rate;
	attr->NumOfTXBuffers = pAdapter->MibInfo.OperationModeCapability.uNumOfTXBuffers;
	attr->NumOfRXBuffers = pAdapter->MibInfo.OperationModeCapability.uNumOfRXBuffers;
	attr->MultiDomainCapabilityImplemented = TRUE;
	attr->NumSupportedPhys = pAdapter->MibInfo.pSupportedPhyTypes->uNumOfEntries;

	MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle,
		&attr->SupportedPhyAttributes,
		attr->NumSupportedPhys * sizeof(DOT11_PHY_ATTRIBUTES),
		HW11_MEMORY_TAG);
	if (attr->SupportedPhyAttributes == NULL)
	{
		return NDIS_STATUS_RESOURCES;
	}

	for (PhyId = 0, PhyAttr = attr->SupportedPhyAttributes; PhyId < attr->NumSupportedPhys; PhyId++, PhyAttr++)
	{
		MP_ASSIGN_NDIS_OBJECT_HEADER(PhyAttr->Header,
			NDIS_OBJECT_TYPE_DEFAULT,
			DOT11_PHY_ATTRIBUTES_REVISION_1,
			sizeof(DOT11_PHY_ATTRIBUTES));

		PhyAttr->PhyType = pAdapter->MibInfo.PhyMIB[PhyId].PhyType;
		PhyAttr->bHardwarePhyState = TRUE;
		PhyAttr->bSoftwarePhyState = TRUE;
		PhyAttr->bCFPollable = FALSE;
		PhyAttr->uMPDUMaxLength = pAdapter->MibInfo.uMPDUMaxLength;
		PhyAttr->TempType = dot11_temp_type_1;
		PhyAttr->DiversitySupport = pAdapter->MibInfo.DiversitySupport;

		PhyAttr->uNumberSupportedPowerLevels = pAdapter->MibInfo.SupportedPowerLevels.uNumOfSupportedPowerLevels;
		for (index = 0; index < PhyAttr->uNumberSupportedPowerLevels; index++)
		{
			PhyAttr->TxPowerLevels[index] = pAdapter->MibInfo.SupportedPowerLevels.uTxPowerLevelValues[index];
		}

		switch (PhyAttr->PhyType)
		{
		case dot11_phy_type_ofdm:
            PhyAttr->OFDMAttributes.uFrequencyBandsSupported = DOT11_FREQUENCY_BANDS_LOWER | 
                                                           DOT11_FREQUENCY_BANDS_MIDDLE;
            break;
		case dot11_phy_type_hrdsss:
			PhyAttr->HRDSSSAttributes.bShortPreambleOptionImplemented = FALSE;
			PhyAttr->HRDSSSAttributes.bPBCCOptionImplemented = FALSE;
			PhyAttr->HRDSSSAttributes.bChannelAgilityPresent = FALSE;
			PhyAttr->HRDSSSAttributes.uHRCCAModeSupported = pAdapter->MibInfo.CCAModeSupported;
			break;
		case dot11_phy_type_erp:
			PhyAttr->ERPAttributes.bShortPreambleOptionImplemented = TRUE;
			PhyAttr->ERPAttributes.bPBCCOptionImplemented = FALSE;
			PhyAttr->ERPAttributes.bChannelAgilityPresent = FALSE;
			PhyAttr->ERPAttributes.uHRCCAModeSupported = pAdapter->MibInfo.CCAModeSupported;
			PhyAttr->ERPAttributes.bERPPBCCOptionImplemented = FALSE;
			PhyAttr->ERPAttributes.bDSSSOFDMOptionImplemented = FALSE;
			PhyAttr->ERPAttributes.bShortSlotTimeOptionImplemented = TRUE;
			break;
			// TODO: Fill the n mode attributes.    2021/03/12
		case dot11_phy_type_ht:
            
			break;

		default:
			break;
		}
		// TODO:
		/*NdisMoveMemory(&PhyAttr->SupportedDataRatesValue,
			&pAdapter->MibInfo.PhyMIB[PhyId].SupportedDataRatesValue,
			sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2));
			*/
		index = 0;
		while ((rate = PhyAttr->SupportedDataRatesValue.ucSupportedTxDataRatesValue[index]) != 0 &&
			index < DOT11_RATE_SET_MAX_LENGTH)
		{
			PhyAttr->DataRateMappingEntries[index].ucDataRateIndex = rate;
			PhyAttr->DataRateMappingEntries[index].ucDataRateFlag = 0;
			PhyAttr->DataRateMappingEntries[index].usDataRateValue = (USHORT)rate;

			index++;
		}
		PhyAttr->uNumDataRateMappingEntries = index;
	}

	return NDIS_STATUS_SUCCESS;
#else
	PDOT11_PHY_ATTRIBUTES   PhyAttr;
    ULONG                   PhyId;
    ULONG                   index;
    UCHAR                   rate;
	wf_mib_info_t *mib_info = &pAdapter->MibInfo;

    attr->NumOfTXBuffers = mib_info->OperationModeCapability.uNumOfTXBuffers;
    attr->NumOfRXBuffers = mib_info->OperationModeCapability.uNumOfRXBuffers;
    attr->MultiDomainCapabilityImplemented = TRUE;
    attr->NumSupportedPhys = mib_info->pSupportedPhyTypes->uNumOfEntries;

    MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle, 
                       &attr->SupportedPhyAttributes,
                       attr->NumSupportedPhys * sizeof(DOT11_PHY_ATTRIBUTES),
                       HW11_MEMORY_TAG);
    if (attr->SupportedPhyAttributes == NULL)
    {
        return NDIS_STATUS_RESOURCES;
    }

    for (PhyId = 0, PhyAttr = attr->SupportedPhyAttributes; PhyId < attr->NumSupportedPhys; PhyId++, PhyAttr++)
    {
        MP_ASSIGN_NDIS_OBJECT_HEADER(PhyAttr->Header, 
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     DOT11_PHY_ATTRIBUTES_REVISION_1,
                                     sizeof(DOT11_PHY_ATTRIBUTES));

        PhyAttr->PhyType = mib_info->PhyMIB[PhyId].PhyType;
        PhyAttr->bHardwarePhyState = TRUE;
        PhyAttr->bSoftwarePhyState = TRUE;
        PhyAttr->bCFPollable = FALSE;
        PhyAttr->uMPDUMaxLength = mib_info->uMPDUMaxLength;
        PhyAttr->TempType = dot11_temp_type_1;
        PhyAttr->DiversitySupport = mib_info->DiversitySupport;

        PhyAttr->uNumberSupportedPowerLevels = mib_info->SupportedPowerLevels.uNumOfSupportedPowerLevels;
        for (index = 0; index < PhyAttr->uNumberSupportedPowerLevels; index++)
        {
            PhyAttr->TxPowerLevels[index] = mib_info->SupportedPowerLevels.uTxPowerLevelValues[index];
        }

        switch (PhyAttr->PhyType)
        {
            case dot11_phy_type_hrdsss:
                PhyAttr->HRDSSSAttributes.bShortPreambleOptionImplemented = FALSE;
                PhyAttr->HRDSSSAttributes.bPBCCOptionImplemented = FALSE;
                PhyAttr->HRDSSSAttributes.bChannelAgilityPresent = FALSE;
                PhyAttr->HRDSSSAttributes.uHRCCAModeSupported = mib_info->CCAModeSupported;
                break;

            case dot11_phy_type_ofdm:
                PhyAttr->OFDMAttributes.uFrequencyBandsSupported = DOT11_FREQUENCY_BANDS_LOWER | 
                                                                   DOT11_FREQUENCY_BANDS_MIDDLE;
                break;

            case dot11_phy_type_erp:
                PhyAttr->ERPAttributes.bShortPreambleOptionImplemented = TRUE;
                PhyAttr->ERPAttributes.bPBCCOptionImplemented = FALSE;
                PhyAttr->ERPAttributes.bChannelAgilityPresent = FALSE;
                PhyAttr->ERPAttributes.uHRCCAModeSupported = mib_info->CCAModeSupported;
                PhyAttr->ERPAttributes.bERPPBCCOptionImplemented = FALSE;
                PhyAttr->ERPAttributes.bDSSSOFDMOptionImplemented = FALSE;
                PhyAttr->ERPAttributes.bShortSlotTimeOptionImplemented = TRUE;
                break;

            default:
                break;
        }
        NdisMoveMemory(&PhyAttr->SupportedDataRatesValue,
                       &mib_info->PhyMIB[PhyId].SupportedDataRatesValue,
                       sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2));

        index = 0;
        while ((rate = PhyAttr->SupportedDataRatesValue.ucSupportedTxDataRatesValue[index]) != 0 && 
               index < DOT11_RATE_SET_MAX_LENGTH)
        {
            PhyAttr->DataRateMappingEntries[index].ucDataRateIndex = rate;
            PhyAttr->DataRateMappingEntries[index].ucDataRateFlag = 0;
            PhyAttr->DataRateMappingEntries[index].usDataRateValue = (USHORT)rate;

            index++;
        }

        PhyAttr->uNumDataRateMappingEntries = index;
    }

    return NDIS_STATUS_SUCCESS;
#endif
}

/**
* This function is to set Staion attributes which will be sent to NDIS.
*/
NDIS_STATUS SetSta80211Attributes(__in  PADAPTER pAdapter,
	__out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr)
{
#if 0
    DOT11_EXTSTA_CAPABILITY         ExtStaCap;
    NDIS_STATUS                     ndisStatus = NDIS_STATUS_SUCCESS;
    DOT11_BSS_TYPE                  savedBssType;
    
    MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle, 
                       &attr->ExtSTAAttributes,
                       sizeof(DOT11_EXTSTA_ATTRIBUTES),
					   HW11_MEMORY_TAG);
    if (attr->ExtSTAAttributes == NULL)
    {
        return NDIS_STATUS_RESOURCES;
    }
    NdisZeroMemory(attr->ExtSTAAttributes, sizeof(DOT11_EXTSTA_ATTRIBUTES));
    
    //
    // First part of the attribute is the same as the capability. Get it
    // from Sta11QueryExtStaCapability.
    //
	{
		ExtStaCap.uScanSSIDListSize = 4;     // minimum required.
		ExtStaCap.uDesiredBSSIDListSize = STA_DESIRED_BSSID_MAX_COUNT;
		ExtStaCap.uDesiredSSIDListSize = 1;
		ExtStaCap.uExcludedMacAddressListSize = STA_EXCLUDED_MAC_ADDRESS_MAX_COUNT;
		ExtStaCap.uPrivacyExemptionListSize = 32;
		ExtStaCap.uKeyMappingTableSize = KEY_TABLE_SIZE - DOT11_MAX_NUM_DEFAULT_KEY;
		ExtStaCap.uDefaultKeyTableSize = DOT11_MAX_NUM_DEFAULT_KEY;
		ExtStaCap.uWEPKeyValueMaxLength = 104 / 8;
		ExtStaCap.uPMKIDCacheSize = STA_PMKID_MAX_COUNT;
		ExtStaCap.uMaxNumPerSTADefaultKeyTables = PER_STA_KEY_TABLE_SIZE;
	}

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        return ndisStatus;
    }
    MP_ASSIGN_NDIS_OBJECT_HEADER(attr->ExtSTAAttributes->Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_EXTSTA_ATTRIBUTES_REVISION_1,
                                 sizeof(DOT11_EXTSTA_ATTRIBUTES));
    attr->ExtSTAAttributes->uScanSSIDListSize = ExtStaCap.uScanSSIDListSize;
    attr->ExtSTAAttributes->uDesiredBSSIDListSize = ExtStaCap.uDesiredBSSIDListSize;
    attr->ExtSTAAttributes->uDesiredSSIDListSize = ExtStaCap.uDesiredSSIDListSize;
    attr->ExtSTAAttributes->uExcludedMacAddressListSize = ExtStaCap.uExcludedMacAddressListSize;
    attr->ExtSTAAttributes->uPrivacyExemptionListSize = ExtStaCap.uPrivacyExemptionListSize;
    attr->ExtSTAAttributes->uKeyMappingTableSize = ExtStaCap.uKeyMappingTableSize;
    attr->ExtSTAAttributes->uDefaultKeyTableSize = ExtStaCap.uDefaultKeyTableSize;
    attr->ExtSTAAttributes->uWEPKeyValueMaxLength = ExtStaCap.uWEPKeyValueMaxLength;
    attr->ExtSTAAttributes->uPMKIDCacheSize = ExtStaCap.uPMKIDCacheSize;
    attr->ExtSTAAttributes->uMaxNumPerSTADefaultKeyTables = ExtStaCap.uMaxNumPerSTADefaultKeyTables;
    attr->ExtSTAAttributes->bStrictlyOrderedServiceClassImplemented = FALSE;
    //
    // Safe mode enabled
    //
    attr->ExtSTAAttributes->bSafeModeImplemented = TRUE;
    
    //
    // 11d stuff.
    //
    attr->ExtSTAAttributes->uNumSupportedCountryOrRegionStrings = 0;
    attr->ExtSTAAttributes->pSupportedCountryOrRegionStrings = NULL;
    //
    // We are about to query the supported algorithm pairs for both infrastructure and ad hoc.
    // We save our current bss type and temporarily set bss type to infrastructure and ad hoc
    // before each query. After we are done, we restore our bss type.
    //

    savedBssType = pAdapter->MibInfo.BSSType;

	//
	// TODO: The below codes describes station enc/dec abilities. Consider to implement them in Query OID
	// related functions to avoid codes repeated.       2021/03/15
	//

    __try 
    {
		//
		// Set bss type to infra
		//
		pAdapter->MibInfo.BSSType = dot11_BSS_type_infrastructure;

		//
		// Get unicast algorithm pair list for infrastructure
		//
		ndisStatus = StaGetAlgorithmPair(pAdapter,
			Sta11QuerySupportedUnicastAlgorithmPair,
			&attr->ExtSTAAttributes->pInfraSupportedUcastAlgoPairs,
			&attr->ExtSTAAttributes->uInfraNumSupportedUcastAlgoPairs);
		if (ndisStatus != NDIS_STATUS_SUCCESS)
			__leave;

		//
		// Get multicast algorithm pair list for infrastructure
		//
		ndisStatus = StaGetAlgorithmPair(pAdapter,
			Sta11QuerySupportedMulticastAlgorithmPair,
			&attr->ExtSTAAttributes->pInfraSupportedMcastAlgoPairs,
			&attr->ExtSTAAttributes->uInfraNumSupportedMcastAlgoPairs);
		if (ndisStatus != NDIS_STATUS_SUCCESS)
			__leave;

		//
		// Now set bss type to adhoc
		//
		pAdapter->MibInfo.BSSType = dot11_BSS_type_independent;

		//
		// Get unicast algorithm pair list for ad hoc
		//
		ndisStatus = StaGetAlgorithmPair(pAdapter,
			Sta11QuerySupportedUnicastAlgorithmPair,
			&attr->ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs,
			&attr->ExtSTAAttributes->uAdhocNumSupportedUcastAlgoPairs);
		if (ndisStatus != NDIS_STATUS_SUCCESS)
			__leave;

		//
		// Get multicast algorithm pair list for ad hoc
		//
		ndisStatus = StaGetAlgorithmPair(pAdapter,
			Sta11QuerySupportedMulticastAlgorithmPair,
			&attr->ExtSTAAttributes->pAdhocSupportedMcastAlgoPairs,
			&attr->ExtSTAAttributes->uAdhocNumSupportedMcastAlgoPairs);
    }
    __finally
    {
        //
        // Restore our bss type 
        //
		pAdapter->MibInfo.BSSType = savedBssType;
    }
    return ndisStatus;
#else
	DOT11_EXTSTA_CAPABILITY         ExtStaCap;
    NDIS_STATUS                     ndisStatus;
    DOT11_BSS_TYPE                  savedBssType;
	wf_mib_info_t *mib_info = &pAdapter->MibInfo;

    MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle, 
                       &attr->ExtSTAAttributes,
                       sizeof(DOT11_EXTSTA_ATTRIBUTES),
                       'SltR');
    if (attr->ExtSTAAttributes == NULL)
    {
        return NDIS_STATUS_RESOURCES;
    }

    NdisZeroMemory(attr->ExtSTAAttributes, sizeof(DOT11_EXTSTA_ATTRIBUTES));

    //
    // First part of the attribute is the same as the capability. Get it
    // from Sta11QueryExtStaCapability.
    //

    MP_ASSIGN_NDIS_OBJECT_HEADER(attr->ExtSTAAttributes->Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_EXTSTA_ATTRIBUTES_REVISION_1,
                                 sizeof(DOT11_EXTSTA_ATTRIBUTES));

    attr->ExtSTAAttributes->uScanSSIDListSize = NATIVE_802_11_MAX_SCAN_SSID;				//8
    attr->ExtSTAAttributes->uDesiredBSSIDListSize = NATIVE_802_11_MAX_DESIRED_BSSID;
    attr->ExtSTAAttributes->uDesiredSSIDListSize = NATIVE_802_11_MAX_DESIRED_SSID;
    attr->ExtSTAAttributes->uExcludedMacAddressListSize = NATIVE_802_11_MAX_EXCLUDED_MACADDR;
    attr->ExtSTAAttributes->uPrivacyExemptionListSize = NATIVE_802_11_MAX_PRIVACY_EXEMPTION;
    attr->ExtSTAAttributes->uKeyMappingTableSize = NATIVE_802_11_MAX_KEY_MAPPING_ENTRY - DOT11_MAX_NUM_DEFAULT_KEY;
    attr->ExtSTAAttributes->uDefaultKeyTableSize = NATIVE_802_11_MAX_DEFAULT_KEY_ENTRY;
    attr->ExtSTAAttributes->uWEPKeyValueMaxLength = NATIVE_802_11_MAX_WEP_KEY_LENGTH;
    // When considering about adding roaming feature, modify this value.   2021/04/02
    attr->ExtSTAAttributes->uPMKIDCacheSize = NATIVE_802_11_MAX_PMKID_CACHE;
    attr->ExtSTAAttributes->uMaxNumPerSTADefaultKeyTables =  NATIVE_802_11_MAX_PER_STA_DEFAULT_KEY;	//32
    attr->ExtSTAAttributes->bStrictlyOrderedServiceClassImplemented = FALSE;
	attr->ExtSTAAttributes->ucSupportedQoSProtocolFlags = DOT11_QOS_PROTOCOL_FLAG_WMM ;
    //
    // Safe mode enabled
    //
    attr->ExtSTAAttributes->bSafeModeImplemented = TRUE;
    //
    // 11d stuff.
    //
    attr->ExtSTAAttributes->uNumSupportedCountryOrRegionStrings = 0;
    attr->ExtSTAAttributes->pSupportedCountryOrRegionStrings = NULL;

    //
    // We are about to query the supported algorithm pairs for both infrastructure and ad hoc.
    // We save our current bss type and temporarily set bss type to infrastructure and ad hoc
    // before each query. After we are done, we restore our bss type.
    //
    savedBssType = mib_info->BSSType;
	#if 1
    __try 
    {
        //
        // Set bss type to infra
        //
        mib_info->BSSType = dot11_BSS_type_infrastructure;

        //
        // Get unicast algorithm pair list for infrastructure
        //
        ndisStatus = StaGetAlgorithmPair(pAdapter,
                                         Sta11QuerySupportedUnicastAlgorithmPair,
                                         &attr->ExtSTAAttributes->pInfraSupportedUcastAlgoPairs,
                                         &attr->ExtSTAAttributes->uInfraNumSupportedUcastAlgoPairs);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;

        //
        // Get multicast algorithm pair list for infrastructure
        //
        ndisStatus = StaGetAlgorithmPair(pAdapter,
                                         Sta11QuerySupportedMulticastAlgorithmPair,
                                         &attr->ExtSTAAttributes->pInfraSupportedMcastAlgoPairs,
                                         &attr->ExtSTAAttributes->uInfraNumSupportedMcastAlgoPairs);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;

        //
        // Now set bss type to adhoc
        //
        mib_info->BSSType = dot11_BSS_type_independent;

        //
        // Get unicast algorithm pair list for ad hoc
        //
        ndisStatus = StaGetAlgorithmPair(pAdapter,
                                         Sta11QuerySupportedUnicastAlgorithmPair,
                                         &attr->ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs,
                                         &attr->ExtSTAAttributes->uAdhocNumSupportedUcastAlgoPairs);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;

        //
        // Get multicast algorithm pair list for ad hoc
        //
        ndisStatus = StaGetAlgorithmPair(pAdapter,
                                         Sta11QuerySupportedMulticastAlgorithmPair,
                                         &attr->ExtSTAAttributes->pAdhocSupportedMcastAlgoPairs,
                                         &attr->ExtSTAAttributes->uAdhocNumSupportedMcastAlgoPairs);
    }
    __finally
    {
        //
        // Restore our bss type 
        //
        mib_info->BSSType = savedBssType;
    }
	#endif

    return ndisStatus;
#endif
}
