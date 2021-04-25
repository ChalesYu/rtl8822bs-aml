#ifndef _HW_OIDS_H_

#define _HW_OIDS_H_

#define STA11_MEMORY_TAG                    'SltR'
VOID
Hw11QueryStationId(
	__in PADAPTER	pAdapter,
	__in DOT11_MAC_ADDRESS Dot11MacAddress
);

NDIS_STATUS
Hw11Fill80211Attributes(
	__in  PADAPTER	pAdapter,
	__out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
);

VOID
Hw11Cleanup80211Attributes(
	__in PADAPTER	pAdapter,
	__in PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
);

VOID
Hw11QueryOpModeCap(
	__in  PADAPTER	pAdapter,
	__out PDOT11_OPERATION_MODE_CAPABILITY pDot11OpModeCap
);

VOID
Hw11QueryCurrentOperationMode(
	__in PADAPTER	pAdapter,
	__out PDOT11_CURRENT_OPERATION_MODE pDot11CurrentOperationMode
);

ULONG
Hw11QueryATIMWindow(
	__in PADAPTER	pAdapter
);

BOOLEAN
Hw11QueryNicPowerState(
	__in PADAPTER	pAdapter,
	__in BOOLEAN selectedPhy
);

NDIS_STATUS
Hw11QueryOptionalCapability(
	__in  PADAPTER	pAdapter,
	__out PDOT11_OPTIONAL_CAPABILITY pDot11OptionalCapability
);

NDIS_STATUS
Hw11QueryCurrentOptionalCapability(
	__in PADAPTER	pAdapter,
	__in PDOT11_CURRENT_OPTIONAL_CAPABILITY pDot11CurrentOptionalCapability
);

BOOLEAN
Hw11QueryCFPollable(
	__in PADAPTER	pAdapter
);

VOID
Hw11QueryOperationalRateSet(
	__in  PADAPTER	pAdapter,
	__out PDOT11_RATE_SET pDot11RateSet,
	__in  BOOLEAN         selectedPhy
);

ULONG
Hw11QueryBeaconPeriod(
	__in PADAPTER	pAdapter
);

ULONG
Hw11QueryRTSThreshold(
	__in PADAPTER	pAdapter
);

ULONG
Hw11QueryShortRetryLimit(
	__in PADAPTER	pAdapter
);

ULONG
Hw11QueryLongRetryLimit(
	__in PADAPTER	pAdapter
);

ULONG
Hw11GetFragmentationThreshold(
	__in PADAPTER pAdapter
);

ULONG
Hw11QueryMaxTXMSDULifeTime(
	__in PADAPTER pAdapter
);

ULONG
Hw11QueryMaxReceiveLifeTime(
	__in PADAPTER pAdapter
);

NDIS_STATUS
Hw11QuerySupportedPHYTypes(
	__in  PADAPTER pAdapter,
	__in  ULONG uNumMaxEntries,
	__out PDOT11_SUPPORTED_PHY_TYPES pDot11SupportedPhyTypes
);

DOT11_DIVERSITY_SUPPORT
Hw11QueryDiversitySupport(
	__in  PADAPTER pAdapter
);

VOID
Hw11QuerySupportedPowerLevels(
	__in  PADAPTER pAdapter,
	__out PDOT11_SUPPORTED_POWER_LEVELS pDot11SupportedPowerLevels
);

ULONG
Hw11QueryCurrentTXPowerLevel(
	__in PADAPTER pAdapter
);

NDIS_STATUS
Hw11GetChannel(
	__in  PADAPTER pAdapter,
	__out PULONG       channel,
	__in  BOOLEAN      selectedPhy
);

NDIS_STATUS
Hw11QueryCCAModeSupported(
	__in  PADAPTER pAdapter,
	__out PULONG      value,
	__in  BOOLEAN     selectedPhy
);

NDIS_STATUS
Hw11QueryCurrentCCA(
	__in  PADAPTER pAdapter,
	__out PULONG      value,
	__in  BOOLEAN     selectedPhy
);

NDIS_STATUS
Hw11QueryEdThreshold(
	__in  PADAPTER pAdapter,
	__out PULONG       value,
	__in  BOOLEAN      selectedPhy
);

NDIS_STATUS
Hw11GetOfdmFrequency(
	__in  PADAPTER pAdapter,
	__out PULONG      freq,
	__in  BOOLEAN     selectedPhy
);

NDIS_STATUS
Hw11FrequencyBandsSupported(
	__in  PADAPTER		pAdapter,
	__out PULONG		value,
	__in  BOOLEAN		selectedPhy
);

NDIS_STATUS
Hw11QueryRegDomainsSupportValue(
	__in  PADAPTER	pAdapter,
	__in  ULONG		uNumMaxEntries,
	__out_ecount(uNumMaxEntries) PDOT11_REG_DOMAINS_SUPPORT_VALUE pDot11RegDomainsSupportValue
);

NDIS_STATUS
Hw11QuerySupportedTXAntenna(
	__in PADAPTER	pAdapter,
	__in ULONG		uNumMaxEntries,
	__out_ecount(uNumMaxEntries) PDOT11_SUPPORTED_ANTENNA_LIST pDot11SupportedAntennaList
);

NDIS_STATUS
Hw11QuerySupportedRXAntenna(
	__in PADAPTER	pAdapter,
	__in ULONG		uNumMaxEntries,
	__out_ecount(uNumMaxEntries) PDOT11_SUPPORTED_ANTENNA_LIST pDot11SupportedAntennaList
);

NDIS_STATUS
Hw11QuerySupportedGUIDS(
	__in  PADAPTER	pAdapter
);

NDIS_STATUS
Hw11QueryStatistics(
	__in PADAPTER	pAdapter,
	__in NDIS_OID         Oid,
	__inout PVOID        StatBuffer,
	__inout PULONG       BytesWritten
);

NDIS_STATUS
Sta11QuerySupportedUnicastAlgorithmPair(
	__in  PADAPTER        pAdapter,
	__out_bcount(TotalLength) PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList,
	__in __range(sizeof(DOT11_AUTH_CIPHER_PAIR_LIST) - sizeof(DOT11_AUTH_CIPHER_PAIR), 0x7FFFFFFF) ULONG           TotalLength
);

NDIS_STATUS
Sta11QuerySupportedMulticastAlgorithmPair(
	__in  PADAPTER        pAdapter,
	__out_bcount(TotalLength) PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList,
	__in __range(sizeof(DOT11_AUTH_CIPHER_PAIR_LIST) - sizeof(DOT11_AUTH_CIPHER_PAIR), 0x7FFFFFFF)  ULONG           TotalLength
);

NDIS_STATUS Sta11DisconnectInfra(PADAPTER pAdapter);

NDIS_STATUS
Sta11SetEnabledAuthenticationAlgorithm(
	__in  PADAPTER       pAdapter,
	__in  PDOT11_AUTH_ALGORITHM_LIST  pAuthAlgoList,
	__out PULONG          pBytesRead
);

VOID
StaSetDefaultCipher(
	__in PADAPTER pAdapter
);

VOID
Hw11SetEncryption(
	__in  PADAPTER pAdapter,
	__in  BOOLEAN isUnicast,
	__in  DOT11_CIPHER_ALGORITHM  algoId
);

NDIS_STATUS
Sta11QueryEnabledAuthenticationAlgorithm(
	__in  PADAPTER        pAdapter,
	__out PDOT11_AUTH_ALGORITHM_LIST  pAuthAlgoList,
	__in  ULONG           TotalLength
);

NDIS_STATUS
Sta11QueryEnabledMulticastCipherAlgorithm(
	__in  PADAPTER        pAdapter,
	__out PDOT11_CIPHER_ALGORITHM_LIST  pAuthCipherList,
	__in  ULONG           TotalLength
);

NDIS_STATUS
Sta11SetEnabledUnicastCipherAlgorithm(
	__in  PADAPTER        pAdapter,
	__in  PDOT11_CIPHER_ALGORITHM_LIST  pCipherAlgoList,
	__out PULONG          pBytesRead
);

BOOLEAN
StaValidateUnicastAuthCipherPair(
	__in PADAPTER pAdapter,
	__in DOT11_AUTH_ALGORITHM AuthAlgo,
	__in DOT11_CIPHER_ALGORITHM CipherAlgo
);

NDIS_STATUS
Sta11SetExcludeUnencrypted(
	__in   PADAPTER  pAdapter,
	__in   BOOLEAN param
);

NDIS_STATUS
Sta11SetPrivacyExemptionList(
	__in   PADAPTER                     pAdapter,
	__in   PDOT11_PRIVACY_EXEMPTION_LIST list,
	__out  PULONG                        pBytesRead
);

NDIS_STATUS
Sta11SetExcludedMACAddressList(
	__in  PADAPTER        pAdapter,
	__in  PDOT11_MAC_ADDRESS_LIST pDot11MacAddrList,
	__out PULONG          pBytesRead
);

NDIS_STATUS
Sta11SetCipherKeyMappingKey(
	__in  PADAPTER pAdapter,
	__in  PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE pKeyMappingKeys,
	__in  ULONG TotalLength
);

NDIS_STATUS
Sta11SetEncryption();

NDIS_STATUS  Sta11SetDesiredBSSType(
	PADAPTER pAdapter,
	DOT11_BSS_TYPE  bssType
);

NDIS_STATUS
Sta11QueryPrivacyExemptionList(
	__in   PADAPTER                      pAdapter,
	__out_bcount(TotalLength) PDOT11_PRIVACY_EXEMPTION_LIST list,
	__in  __range(sizeof(DOT11_PRIVACY_EXEMPTION_LIST) - sizeof(DOT11_PRIVACY_EXEMPTION), 0x7FFFFFFF) ULONG                         TotalLength
);
#endif