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

#define HW_BSSID_NUM				4

EVT_WDF_DRIVER_DEVICE_ADD wf_EvtWdfDriverDeviceAdd;
EVT_WDF_DRIVER_UNLOAD wf_EvtWdfDriverUnload;

typedef enum _ENUM_NET_PORT_IDX_T {
    NET_PORT_WLAN_IDX = 0,
    NET_PORT_P2P0_IDX,
    NET_PORT_P2P1_IDX,
    NET_PORT_P2P2_IDX,
    NET_PORT_BOW_IDX,
    NET_PORT_NUM
} ENUM_NET_PORT_IDX_T;

typedef enum _ENUM_NET_DEV_IDX_T {
    NET_DEV_WLAN_IDX = 0,
    NET_DEV_P2P_IDX,
    NET_DEV_BOW_IDX,
    NET_DEV_NUM
} ENUM_NET_DEV_IDX_T;

typedef enum _ENUM_PARAM_MEDIA_STATE_T {
    PARAM_MEDIA_STATE_CONNECTED,
    PARAM_MEDIA_STATE_DISCONNECTED,
    PARAM_MEDIA_STATE_TO_BE_INDICATED // for following MSDN re-association behavior
} ENUM_PARAM_MEDIA_STATE_T, *P_ENUM_PARAM_MEDIA_STATE_T;


typedef struct _NET_INTERFACE_INFO_T {
    wf_u8              ucBssIndex;
    PVOID               pvNetInterface;
} NET_INTERFACE_INFO_T, *P_NET_INTERFACE_INFO_T;



typedef struct _GLUE_INFO_T {
    NDIS_HANDLE             rMiniportAdapterHandle; /* Device handle */

    NDIS_WORK_ITEM          rWorkItem;

    wf_u32                  ucEmuWorkItemId;

    /* driver description read from registry */
    wf_u8                  ucDriverDescLen;
    wf_u8                  aucDriverDesc[80]; /* should be moved to os private */

#if CFG_TCP_IP_CHKSUM_OFFLOAD
    // Add for checksum offloading
//    NIC_TASK_OFFLOAD            rNicTaskOffload;
//    NIC_CHECKSUM_OFFLOAD        rNicChecksumOffload;
//    NDIS_ENCAPSULATION_FORMAT   rEncapsulationFormat;
#endif

    wf_u16                 u2NdisVersion;
    wf_s32                  exitRefCount;
    wf_u32                 u4Flags;

    /* TxService related info */
    HANDLE                  hTxService;         /* TxService Thread Handle */
    NDIS_EVENT              rTxReqEvent;         /* Event to wake up TxService */
//#if defined (WINDOWS_DDK)
//    PKTHREAD                pvKThread;
//#endif

//    QUE_T                   rTxQueue;
//    QUE_T                   rReturnQueue;
//    QUE_T                   rCmdQueue;

    /* spinlock to protect Tx Queue Operation */
//    NDIS_SPIN_LOCK          arSpinLock[SPIN_LOCK_NUM];


    /* Number of pending frames, also used for debuging if any frame is
     * missing during the process of unloading Driver.
     */
    LONG            i4TxPendingFrameNum;
    LONG            i4TxPendingSecurityFrameNum;

    LONG            i4RxPendingFrameNum;

    /* for port to Bss index mapping */
    wf_u8          aucNetInterfaceToBssIdx[NET_PORT_NUM];

    BOOLEAN         fgIsCardRemoved;

    /* Host interface related information */
    /* defined in related hif header file */
//    GL_HIF_INFO_T   rHifInfo;

    NDIS_802_11_ASSOCIATION_INFORMATION rNdisAssocInfo;
    /* Pointer to ADAPTER_T - main data structure of internal protocol stack */
    PADAPTER prAdapter;

    /* Indicated media state */
    wf_u8 eParamMediaStateIndicated;

    /* registry info*/
//    REG_INFO_T rRegInfo;

    /* TX/RX: Interface to BSS Index mapping */
    NET_INTERFACE_INFO_T    arNetInterfaceInfo[NET_DEV_NUM];
    P_NET_INTERFACE_INFO_T  aprBssIdxToNetInterfaceInfo[HW_BSSID_NUM];

    /* OID related */
    BOOLEAN             fgSetOid;
    void               *pvInformationBuffer;
    wf_u32             u4InformationBufferLength;
    wf_u32             *pu4BytesReadOrWritten;
    wf_u32             *pu4BytesNeeded;
    void               *pvOidEntry;
    BOOLEAN             fgIsGlueExtension;

#if CFG_SUPPORT_WAPI
    /* Should be large than the PARAM_WAPI_ASSOC_INFO_T */
    wf_u8                  aucWapiAssocInfoIEs[42];
    wf_u16                 u2WapiAssocInfoIESz;
#endif

    LONG                i4OidPendingCount;

    /* Timer related */
    void               *pvTimerFunc;
    NDIS_MINIPORT_TIMER rMasterTimer;

    BOOLEAN                 fgWpsActive;
	wf_u8                  aucWSCIE[500]; /*for probe req*/
    wf_u16                 u2WSCIELen;
	wf_u8               aucAssocReqIE[WF_80211_IES_SIZE_MAX]; /*for Assoc req*/
    wf_u16              aucAssocReqIE_Len;
	wf_u8				aucAssocRespIE[WF_80211_IES_SIZE_MAX]; /* for Assoc resp */
	wf_u16				aucAssocRespIE_Len;

	BOOLEAN				bWaitGroupKeyState;
}GLUE_INFO_T, *P_GLUE_INFO_T;

//
// Driver Entry and Exit Points
//
//DRIVER_INITIALIZE DriverEntry;

//MINIPORT_UNLOAD DriverUnload;


VOID
mpPnPEventNotify (
	IN NDIS_HANDLE			 miniportAdapterContext,
	IN NDIS_DEVICE_PNP_EVENT pnpEvent,
	IN PVOID				 informationBuffer_p,
	IN wf_u32				 informationBufferLength
);

//
// Handlers for Entry Points from NDIS
//

NDIS_STATUS
MPInitialize(
    OUT PNDIS_STATUS prOpenErrorStatus,
    OUT PUINT        prSelectedMediumIndex,
    IN  PNDIS_MEDIUM prMediumArray,
    IN  UINT         u4MediumArraySize,
    IN  NDIS_HANDLE  rMiniportAdapterHandle,
    IN  NDIS_HANDLE  rWrapperConfigurationContext
);

VOID mpHalt(NDIS_HANDLE miniportAdapterContext);

void wf_mp_dev_stop(PADAPTER      pAdapter);

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

//NDIS_STATUS
//SetNic80211Attributes
//(__in  PADAPTER pAdapter,
//	__out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr);
//
//NDIS_STATUS SetSta80211Attributes(__in  PADAPTER pAdapter,
//	__out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr);

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

NDIS_STATUS
MpAllocateAdapter(
    __in NDIS_HANDLE MiniportAdapterHandle,
    __deref_out_opt PADAPTER* ppAdapter
);

VOID
MpFreeAdapter(
    __in __drv_freesMem(Pool) PADAPTER					 pAdapter
);

VOID
MpFreeGlue(P_GLUE_INFO_T prGlueInfo
);

NTSTATUS
WfEvtDeviceAdd(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
);

NTSTATUS
WfEvtDeviceUnload(
    IN WDFDRIVER        Driver
);

NDIS_STATUS wf_attr_init(void* pGlueInfo);

NDIS_STATUS MpInitializeWorkitem(PADAPTER pAdapter);

VOID wf_mp_suprise_removed(PADAPTER prAdapter);

#endif  // _NATIVE_WIFI_MAIN_H_
