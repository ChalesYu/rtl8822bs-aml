/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Oids.h

Abstract:
    OID processing code
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef __WF_OIDS_H__
#define __WF_OIDS_H__

// NIC mangement related macros.
#define MAX_NUM_PHY_TYPES                    4
#define MAX_NUM_DOT11_REG_DOMAINS_VALUE      10
#define MAX_NUM_DIVERSITY_SELECTION_RX_LIST  256

#define NATIVE_802_11_MAX_SCAN_SSID				64
#define NATIVE_802_11_MAX_DESIRED_BSSID			8
#define NATIVE_802_11_MAX_DESIRED_SSID			1
#define NATIVE_802_11_MAX_EXCLUDED_MACADDR		4
#define NATIVE_802_11_MAX_PRIVACY_EXEMPTION		32
#define NATIVE_802_11_MAX_KEY_MAPPING_ENTRY		32
#define NATIVE_802_11_MAX_KEY_MAPPING_ENTRY_HCT	8
#define NATIVE_802_11_MAX_DEFAULT_KEY_ENTRY		DOT11_MAX_NUM_DEFAULT_KEY
#define NATIVE_802_11_MAX_WEP_KEY_LENGTH		13
#define NATIVE_802_11_WEP40_KEY_LENGTH			5
#define NATIVE_802_11_WEP104_KEY_LENGTH			13
#define NATIVE_802_11_MAX_PMKID_CACHE			3
#define NATIVE_802_11_MAX_PER_STA_DEFAULT_KEY	32
#define PER_STA_KEY_TABLE_SIZE					NATIVE_802_11_MAX_KEY_MAPPING_ENTRY - DOT11_MAX_NUM_DEFAULT_KEY
#define KEY_TABLE_SIZE							32

#define WF_WIN_80211_IES_SIZE_MAX               768

typedef enum wf_phy_id_e{
	WF_PHY_ID_B,
	WF_PHY_ID_G,
	WF_PHY_ID_N,
	WF_PHY_ID_MAX
}wf_phy_id_e;

#define WF_PHY_ID_DEFAULT WF_PHY_ID_B

VOID
MpQuerySupportedOidsList(
    __inout PNDIS_OID *SupportedOidList,
    __inout PULONG    SupportedOidListLength
    );

NDIS_STATUS
MpQuerySupportedPHYTypes(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpResetRequest(
    __in  PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in  ULONG InputBufferLength,
    __in  ULONG OutputBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
MpSetMulticastList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryMPDUMaxLength(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryOperationModeCapability(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentOperationMode(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryATIMWindow(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryIndicateTXStatus(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryNicPowerState(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryOptionalCapability(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentOptionalCapability(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCFPollable(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryPowerMgmtMode(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryOperationalRateSet(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryBeaconPeriod(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryRTSThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryShortRetryLimit(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryLongRetryLimit(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryFragmentationThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryMaxTXMSDULifeTime(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryMaxReceiveLifeTime(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentRegDomain(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryTempType(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentTXAntenna(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDiversitySupport(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentRXAntenna(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedPowerLevels(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpPnPQueryPower(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength
    );

NDIS_STATUS
MpQueryCurrentTXPowerLevel(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentChannel(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCCAModeSupported(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentCCA(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryEDThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryRegDomainsSupportValue(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedTXAntenna(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedRXAntenna(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDiversitySelectionRX(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedDataRatesValue(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetATIMWindow(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetIndicateTXStatus(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetNicPowerState(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryStationId(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryPnPCapabilities(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetPower(
    __in  PADAPTER        pAdapter,
    __in  PVOID           InformationBuffer,
    __in  ULONG           InformationBufferLength,
    __in  PULONG          pulBytesNeeded,
    __in  PULONG          pulBytesRead
    );

NDIS_STATUS
MpAddWakeUpPattern(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpRemoveWakeUpPattern(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetOperationalRateSet(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetBeaconPeriod(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetRTSThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetFragmentationThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetCurrentRegDomain(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );
    
NDIS_STATUS
MpSetCurrentChannel(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetCurrentOperationMode(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpScanRequest(
    PADAPTER pAdapter,
    __in PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesRead,
    __out PULONG BytesNeeded
    );



NDIS_STATUS
MpSetPacketFilter(
    __in PADAPTER pAdapter,
    __in ULONG PacketFilter
    );

NDIS_STATUS
MpEnumerateBSSList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InputBufferLength,
    __in ULONG OutputBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDesiredSSIDList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryExcludedMACAddressList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDesiredBSSIDList(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpQueryDesiredBSSType(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDot11Statistics(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryEnabledAuthenticationAlgorithm(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedUnicastAlgorithmPair(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpQuerySupportedMulticastAlgorithmPair(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpQueryEnabledUnicastCipherAlgorithm(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpQueryEnabledMulticastCipherAlgorithm(
    __in PADAPTER pAdapter,
    __out_bcount(InformationBufferLength) PVOID  InformationBuffer,
    __in ULONG InformationBufferLength,
    __out PULONG                  BytesWritten,
    __out PULONG                  BytesNeeded
    );

NDIS_STATUS
MpEnumerateAssociationInformation(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryHardwarePHYState(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDesiredPHYList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryCurrentPHYID(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryMediaSteamingOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryUnreachableDetectionThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryActivePHYList(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );
    
NDIS_STATUS
MpQueryDot11TableSize(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpQueryDataRateMappingTable(
    __in PADAPTER pAdapter,
    __inout_bcount(InformationBufferLength) PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpFlushBSSList(
    __in PADAPTER pAdapter
    );

NDIS_STATUS
MpSetPowerMgmtMode(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetDesiredSSIDList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetExcludedMACAddressList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetDesiredBSSIDList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetDesiredBSSType(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpConnectRequest(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );


NDIS_STATUS
MpDisconnectRequest(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetEnabledAuthenticationAlgorithm(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetDesiredPHYList(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );

NDIS_STATUS
MpSetCurrentPHYID(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );
    
NDIS_STATUS
MpSetMediaStreamingOption(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );
    
NDIS_STATUS
MpSetUnreachableDetectionThreshold(
    __in PADAPTER pAdapter,
    __inout PVOID InformationBuffer,
    __in ULONG InformationBufferLength,
    __inout PULONG BytesRead,
    __inout PULONG BytesNeeded
    );


NDIS_STATUS
MpQueryInformation(
    __in    NDIS_HANDLE               MiniportAdapterContext,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InformationBufferLength,
    __out   PULONG                    BytesWritten,
    __out   PULONG                    BytesNeeded
    );


NDIS_STATUS
MpSetInformation(
    __in    NDIS_HANDLE               MiniportAdapterContext,
    __in    NDIS_OID                  Oid,
    __in    PVOID                     InformationBuffer,
    __in    ULONG                     InformationBufferLength,
    __out   PULONG                    BytesRead,
    __out   PULONG                    BytesNeeded
    );

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
    );

//
// PHY specific MIB. The MIB could be different for different vendors.
//
typedef struct _NICPHYMIB
{
	DOT11_PHY_TYPE                      PhyType;
	ULONG                               PhyID;
	DOT11_RATE_SET                      OperationalRateSet;
	DOT11_RATE_SET                      ActiveRateSet;
	UCHAR                               Channel;
	DOT11_SUPPORTED_DATA_RATES_VALUE_V2 SupportedDataRatesValue;
} NICPHYMIB, *PNICPHYMIB;

typedef enum _SCAN_STATE
{
	ScanStateSwChnl = 0,
	ScanStatePerformScan_Pass1,
	ScanStateMaxState
} SCAN_STATE;

typedef struct _OctetString
{
    PUCHAR  Octet;
    USHORT  Length;
}OctetString,*pOctetString;


typedef struct _TIM{
    UCHAR   DtimCount;
    UCHAR   DtimPeriod;
    UCHAR   BitmapCtrl;
    UCHAR   PartialVirtualBitmap[251];
}TIM,*pTIM;


typedef struct wf_cache_info_s{
	wf_u8  dev_mac[6];
	wf_u8  ap_valid;
	DOT11_SSID_LIST ap_ssid;
	wf_u8  ap_mac[6];
}wf_cache_info_t;


typedef struct wf_mib_info_s
{
	KEVENT 								halt_deauth_finish;
	BOOLEAN								connect_state;
	KEVENT 								scan_hidden_finish;
	BOOLEAN								hidden_network_enable;
	WDFSPINLOCK 						bss_lock;
	ULONG								bss_cnt;	
	wf_wlan_mgmt_scan_que_node_t        bss_node[64];

	WDFTIMER                			exception_timer;
	// switch channel related variable
	BOOLEAN 							SwChnlInProgress;		// indicate if switch channel is in progress
	UCHAR								DestChannel;			// destination channel in channel switch
	//HW_CHANNEL_SWITCH_COMPLETE_HANDLER	ChannelSwitchedHandler;
	WDFWORKITEM 						SwChnlWorkItem;
	UCHAR								SwChnlStage;
	UCHAR								SwChnlStep;    

	// Scan related variable
	WDFTIMER							Timer_Scan;
	BOOLEAN 							bScanInProgress;
	BOOLEAN 							ScanCancelOperation;		  // indicates if scan should be stopped
#if     NDIS60_MINIPORT
	PDOT11_SCAN_REQUEST_V2				ScanRequest;
	ULONG								ScanProbeDelay;
	ULONG								ScanChannelCount;
	PULONG								ScanChannelList;
	ULONG								ScanChannelTime;
	ULONG								ScanCurrentChannelIndex;
	SCAN_STATE							ScanCurrentState;
	ULONGLONG							ScanChannelSwitchTime;
	UCHAR								ScanCurrentChannel;
#else
	DOT11_MAC_ADDRESS					ScanBssid;				// Scan Bssid
	DOT11_SSID							ScanSsid;				// Scan SSID
	DOT11_SCAN_TYPE 					ScanType;				// Scan type
	UCHAR								ScanAppendIEByteArray[256]; // IE byte array that should be appended at the end of Probe Request
	USHORT								ScanAppendIEByteArrayLength;	// IE byte array length
	USHORT								ScanState;				// State of scan
#endif  // NDIS60_MINIPORT

	// 802.11 Power management
	DOT11_POWER_MGMT_MODE				PowerMgmtMode;
	WDFTIMER							Timer_Awake;
	WDFTIMER							Timer_PeriodicCallback;
	//PTX_DESC_8187						pPowerMgmtChangePktDesc;
	BOOLEAN 							bPowerMgmtChangePktSent;
	BOOLEAN 							bATIMPacketReceived;
	BOOLEAN 							bUnicastDataAvailable;
	ULONGLONG							WakeupTime;
	NDIS_SPIN_LOCK						PowerMgmtLock;
	BOOLEAN 							bSleepOnInterruptComplete;
	NDIS_DEVICE_POWER_STATE 			NextDevicePowerState;

	UCHAR								SSID[33];				// 1 byte for 0
	UCHAR								BssId[6];
    UCHAR								DesiredBSSID[6];
	UCHAR								APJoinBSSID[6]; 		//	For AP join mode.

	USHORT								CapInfo;
	USHORT								AtimWindow;

	TIM 								Tim;
	
	USHORT								DtimCount;
	USHORT								DtimPeriod;

	CHAR								IEMap[256]; 			// Used to indicate if an IE is available(This array size should not be changed)
	//IE									IEs[MAX_IE_NUMBER]; 	// Store all information elements
	USHORT								WPAIECount;

	UCHAR								NextBeaconIndex;

	WDFTIMER							Timer_JoinTimeout;
	ULONG								JoinWaitBeaconToSynchronize;

	ULONG								CCAModeSupported;
	ULONG								CurrentCCAMode;

	BOOLEAN 							bScanCompleteIndicated;
	BOOLEAN 							bAdvertiseBSS;

	ULONG								uMPDUMaxLength;

	LONG								EDThreshold;
	BOOLEAN 							ShortSlotTimeOptionEnabled;

	LARGE_INTEGER						DelaySleepTimeStamp;
	ULONG								DelaySleepValue;

	DOT11_OFFLOAD_CAPABILITY			OffloadCapability;
	DOT11_CURRENT_OFFLOAD_CAPABILITY	CurrentOffloadCapability;

	DOT11_OPERATION_MODE_CAPABILITY 	OperationModeCapability;
	DOT11_CURRENT_OPERATION_MODE		CurrentOperationMode;
	DOT11_BSS_TYPE						CurrentBSSType;

	DOT11_OPTIONAL_CAPABILITY			OptionalCapability;
	DOT11_CURRENT_OPTIONAL_CAPABILITY	CurrentOptionalCapability;

	USHORT								CurrentBeaconRate;		// Used for Beacon and Probe request
	USHORT								CurrentTXDataRate;
	USHORT								CurrentTXMgmtRate;

	DOT11_DIVERSITY_SUPPORT 			DiversitySupport;

	DOT11_SUPPORTED_POWER_LEVELS		SupportedPowerLevels;
	ULONG								CurrentTxPowerLevel; // 1..8

	PDOT11_REG_DOMAINS_SUPPORT_VALUE	pRegDomainsSupportValue;

	PDOT11_DIVERSITY_SELECTION_RX_LIST	pDiversitySelectionRxList;
	BOOLEAN 							SafeModeEnabled;

	UCHAR								dot11CurrentWirelessMode;
	UCHAR								Regdot11OperationalRateBuf[64];

	// Auto rate fallback control
	BOOLEAN 							bRegAutoRateFallback;
	UCHAR								RegAutoRateFallbackStep;
		
	// Encryption related info
	//		STA & HW
	NICKEY								KeyTable[NATIVE_802_11_MAX_KEY_MAPPING_ENTRY];	// 0-3 is default key, 4+ is key mapping key 
	UCHAR								DefaultKeyID;
	UCHAR								KeyMappingKeyCount;

	// If we want to support IBSS in the future, we have to use per-sta key.
	//NIC_PER_STA_KEY 					PerStaKey[PER_STA_KEY_TABLE_SIZE];
	//UCHAR								PerStaKeyCount;
	DOT11_CIPHER_ALGORITHM				MulticastCipherAlgorithmList[STA_MULTICAST_CIPHER_MAX_COUNT];
	ULONG								MulticastCipherAlgorithmCount;
	DOT11_CIPHER_ALGORITHM				MulticastCipherAlgorithm;
	DOT11_CIPHER_ALGORITHM				UnicastCipherAlgorithmList[STA_UNICAST_CIPHER_MAX_COUNT];
	ULONG								UnicastCipherAlgorithmCount;
	DOT11_CIPHER_ALGORITHM				UnicastCipherAlgorithm;
	DOT11_AUTH_ALGORITHM				AuthAlgorithm;
	DOT11_AUTH_ALGORITHM				HwAuthAlgorithm;
	DOT11_CIPHER_ALGORITHM				HwUnicastCipher;
	DOT11_CIPHER_ALGORITHM				HwMulticastCipher;
	NPAGED_LOOKASIDE_LIST				EncryptionInfoList;
	NPAGED_LOOKASIDE_LIST				MICDataList;
	NICKEY								KeyProcTable;
	UCHAR								KeyProcKeyID;
	NICKEY								DefKeyProcTable;
	UCHAR								DefKeyProcKeyID;

	// PHY specific MIB
	PDOT11_SUPPORTED_PHY_TYPES			pSupportedPhyTypes;
	ULONG								OperatingPhyId; 		// ID of currently operating PHY
	ULONG								SelectedPhyId;			// index of PHY that any PHY specific OID is applied to

	NDIS_SPIN_LOCK						PhyMIBConfigLock;		// currently used when configuring ActiveRateSet in PhyMIB
	NICPHYMIB							PhyMIB[MAX_NUM_PHY_TYPES];
	PNICPHYMIB							OperatingPhyMIB;		// for currently operating PHY (by OperatingPhyId)
	PNICPHYMIB							SelectedPhyMIB; 		// for currently selected PHY (by SelectedPhyId)
	BOOLEAN 							RegRfOff;

	//default setting ==================
	OctetString 		Regdot11OperationalRateSet;
	UCHAR				dot11CurrentChannelNumber;

	// For turbo mode. 
	// -----------------------------------------
	BOOLEAN 			bInTurboMode;			// 8187 Turbo mode: Currently we are in turbo mode or not. (used when bSupportTurboMode is TRUE)
	//PRIV_8185			Info8185;
	BOOLEAN 			bSetKeyed;
	OctetString 		SupportedRates;
	UCHAR				SupportedRatesBuf[128];
	USHORT				SequenceNumber;

	// current device's connection state
	BOOLEAN 							bDeviceConnected;
	
	/** Total data send retry count for Tx rate negotiation*/
	LONG								TotalRetryCount;
	ULONG								PacketsSentForTxRateCheck;
	UCHAR								MinPacketsSentForTxRateUpdate;
	USHORT								PrevTxDataRate;
	USHORT								TxRateIncreaseWaitCount;
	USHORT								TxRateIncreaseWaitRequired;

	BOOLEAN 							bEnableSendCTSToSelf;

	DOT11_BSS_TYPE						BSSType;

	DOT11_SSID_LIST						curr_ssid;

	/** Current setting related to acceptance of unencrypted data */
	BOOLEAN                 ExcludeUnencrypted;
	PDOT11_PRIVACY_EXEMPTION_LIST   PrivacyExemptionList;

} wf_mib_info_t, *pwf_mib_info_t;

#define OID_RTL_SAMPLE_OID       0xDEEF1201
#define OID_RTL_SAMPLE2_OID      0xDEEF1202


#define MP_PRIVATE_OID_LIST         \
    OID_RTL_SAMPLE_OID,               \
    OID_RTL_SAMPLE2_OID,

VOID
Mp11CompletePendedRequest(
	__in  PADAPTER		  pAdapter,
	__in  NDIS_STATUS	  ndisStatus
	);

NDIS_STATUS wf_mp_oids_init(PADAPTER	   pAdapter);
void wf_mp_oids_deinit(PADAPTER	  pAdapter);

#endif  // _MP_OIDS_H_

