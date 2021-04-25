
NDIS_STATUS
HwInitializeBus(
    __in PADAPTER adapter
    )  ;

VOID
HwDeInitBus(
    __in PADAPTER adapter
    );

NDIS_STATUS
HwReadAdapterInfo(
    __in PADAPTER adapter
    ) ;

NDIS_STATUS
HwInitializeAdapter(
    __in PADAPTER adapter
    );

NDIS_STATUS
ReadNicInfo8187(
    PADAPTER adapter
    );

VOID
InitializeVariables8187(
    PADAPTER adapter
    );
 
NDIS_STATUS
HwBusStartDevice(
                __in PADAPTER adapter
                 );


NDIS_STATUS
HwBusFreeRecvResources(
    __in PADAPTER adapter
    );


NDIS_STATUS
HwBusAllocateRecvResources(
    __in PADAPTER adapter
    );

NDIS_STATUS
HwBusAllocateXmitResources(
    __in PADAPTER adapter ,
    __in  ULONG NumTxd
    )  ;

VOID
HwBusFreeXmitResources(
    __in PADAPTER adapter
    ) ;

NDIS_STATUS
HwBusFindAdapterAndResources(
    __in PADAPTER adapter
    ) ;


NDIS_STATUS
HwBusSendPacketAsync(
    __in PADAPTER adapter,
    __in UCHAR            QueueType,
    __in ULONG            BufferLength,
    __in PVOID            Buffer 
    ) ;

VOID
HwBusStopNotification(
    __in PADAPTER adapter
    );

VOID
HwBusFreeRecvFragment(
                __in  PADAPTER adapter,
                __in  PNIC_RX_FRAGMENT NicFragment
                ) ;


