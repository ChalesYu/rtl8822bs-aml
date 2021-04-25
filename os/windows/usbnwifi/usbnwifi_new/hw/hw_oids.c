#include "pcomp.h"
#include "common.h"
#include "wf_oids_adapt.h"
#define HW11_MEMORY_TAG                     'HltR'
extern int get_efuse_mac(nic_info_st *nic_info, wf_u8 *mac);
VOID
Hw11QueryStationId(
	__in PADAPTER	pAdapter,
	__in DOT11_MAC_ADDRESS Dot11MacAddress
)
{
	NdisMoveMemory(Dot11MacAddress, pAdapter->PermanentAddress, 6);
}
#if 0
NDIS_STATUS
Hw11Fill80211Attributes(
	__in  PADAPTER	pAdapter,
	__out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
)
{
	PDOT11_PHY_ATTRIBUTES   PhyAttr;
	ULONG                   PhyId;
	ULONG                   index;
	UCHAR                   rate;

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
		//PhyAttr->bSoftwarePhyState = !pNic->RfInfo.RadioOff;
		PhyAttr->bCFPollable = FALSE;
		PhyAttr->uMPDUMaxLength = pAdapter->MibInfo.uMPDUMaxLength;		//
		PhyAttr->TempType = dot11_temp_type_1;
		PhyAttr->DiversitySupport = pAdapter->MibInfo.DiversitySupport;	//

		PhyAttr->uNumberSupportedPowerLevels = pAdapter->MibInfo.SupportedPowerLevels.uNumOfSupportedPowerLevels;
		for (index = 0; index < PhyAttr->uNumberSupportedPowerLevels; index++)
		{
			PhyAttr->TxPowerLevels[index] = pAdapter->MibInfo.SupportedPowerLevels.uTxPowerLevelValues[index];
		}

		switch (PhyAttr->PhyType)
		{
		case dot11_phy_type_hrdsss:
			PhyAttr->HRDSSSAttributes.bShortPreambleOptionImplemented = FALSE;
			PhyAttr->HRDSSSAttributes.bPBCCOptionImplemented = FALSE;
			PhyAttr->HRDSSSAttributes.bChannelAgilityPresent = FALSE;
			PhyAttr->HRDSSSAttributes.uHRCCAModeSupported = pAdapter->MibInfo.CCAModeSupported;
			break;

		case dot11_phy_type_ofdm:
			PhyAttr->OFDMAttributes.uFrequencyBandsSupported = DOT11_FREQUENCY_BANDS_LOWER |
				DOT11_FREQUENCY_BANDS_MIDDLE;
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

		default:
			break;
		}
		NdisMoveMemory(&PhyAttr->SupportedDataRatesValue,
			&pAdapter->MibInfo.PhyMIB[PhyId].SupportedDataRatesValue,
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
}
#endif
VOID
Hw11Cleanup80211Attributes(
	__in PADAPTER	pAdapter,
	__in PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
)
{
	UNREFERENCED_PARAMETER(pAdapter);

	if (attr->SupportedPhyAttributes)
	{
		MP_FREE_MEMORY(attr->SupportedPhyAttributes);
	}
}

VOID
Hw11QueryOpModeCap(
	__in  PADAPTER	pAdapter,
	__out PDOT11_OPERATION_MODE_CAPABILITY pDot11OpModeCap
)
{
	NdisMoveMemory(pDot11OpModeCap,
		&pAdapter->MibInfo.OperationModeCapability,
		sizeof(DOT11_OPERATION_MODE_CAPABILITY));
}

VOID
Hw11QueryCurrentOperationMode(
	__in PADAPTER	pAdapter,
	__out PDOT11_CURRENT_OPERATION_MODE pDot11CurrentOperationMode
)
{
#if 0
	NdisMoveMemory(
		&pDot11CurrentOperationMode->uCurrentOpMode,
		&pAdapter->MibInfo.CurrentOperationMode.uCurrentOpMode,
		sizeof(ULONG)
	);
#endif
	pDot11CurrentOperationMode->uCurrentOpMode = DOT11_OPERATION_MODE_STATION;
}

ULONG
Hw11QueryATIMWindow(
	__in PADAPTER	pAdapter
)
{
	return (ULONG)pAdapter->MibInfo.AtimWindow;
}

BOOLEAN
Hw11QueryNicPowerState(
	__in PADAPTER	pAdapter,
	__in BOOLEAN selectedPhy
)
{
	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(selectedPhy);
	return TRUE;
	//return (!pAdapter->RadioOff);
}

NDIS_STATUS
Hw11QueryOptionalCapability(
	__in  PADAPTER	pAdapter,
	__out PDOT11_OPTIONAL_CAPABILITY pDot11OptionalCapability
)
{
#if 0
	NdisMoveMemory(pDot11OptionalCapability,
		&pAdapter->MibInfo.OptionalCapability,
		sizeof(DOT11_OPTIONAL_CAPABILITY));
#endif
	pDot11OptionalCapability->bDot11PCF = TRUE;
	pDot11OptionalCapability->bDot11PCFMPDUTransferToPC = TRUE;
	pDot11OptionalCapability->bStrictlyOrderedServiceClass = TRUE;
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Hw11QueryCurrentOptionalCapability(
	__in PADAPTER	pAdapter,
	__in PDOT11_CURRENT_OPTIONAL_CAPABILITY pDot11CurrentOptionalCapability
)
{
#if 0
	NdisMoveMemory(
		pDot11CurrentOptionalCapability,
		&pAdapter->MibInfo.CurrentOptionalCapability,
		sizeof(DOT11_CURRENT_OPTIONAL_CAPABILITY));
#endif
	pDot11CurrentOptionalCapability->bDot11CFPollable = TRUE;
	pDot11CurrentOptionalCapability->bDot11PCF = TRUE;
	pDot11CurrentOptionalCapability->bDot11PCFMPDUTransferToPC = TRUE;
	pDot11CurrentOptionalCapability->bStrictlyOrderedServiceClass = TRUE;
	return NDIS_STATUS_SUCCESS;
}

BOOLEAN
Hw11QueryCFPollable(
	__in PADAPTER	pAdapter
)
{
	UNREFERENCED_PARAMETER(pAdapter);
	return FALSE;
}

VOID
Hw11QueryOperationalRateSet(
	__in  PADAPTER	pAdapter,
	__out PDOT11_RATE_SET pDot11RateSet,
	__in  BOOLEAN         selectedPhy
)
{
	PNICPHYMIB  PhyMib;
	ULONG       Index;

	PhyMib = selectedPhy ? pAdapter->MibInfo.SelectedPhyMIB : pAdapter->MibInfo.OperatingPhyMIB;

	for (Index = 0; Index < PhyMib->OperationalRateSet.uRateSetLength; Index++)
	{
		pDot11RateSet->ucRateSet[Index] = PhyMib->OperationalRateSet.ucRateSet[Index];
	}

	pDot11RateSet->uRateSetLength = Index;
}

ULONG
Hw11QueryBeaconPeriod(
	__in PADAPTER	pAdapter
)
{
	return pAdapter->DupCurrentInfo.BeaconInterval;
}

ULONG
Hw11QueryRTSThreshold(
	__in PADAPTER	pAdapter
)
{
	return pAdapter->DupCurrentInfo.RTS_Threshold;
}

ULONG
Hw11QueryShortRetryLimit(
	__in PADAPTER	pAdapter
)
{
	return pAdapter->DupCurrentInfo.ShortRetryLimit;
}

ULONG
Hw11QueryLongRetryLimit(
	__in PADAPTER	pAdapter
)
{
	return pAdapter->DupCurrentInfo.LongRetryLimit;
}

ULONG
Hw11GetFragmentationThreshold(
	__in PADAPTER pAdapter
)
{
	return pAdapter->DupCurrentInfo.Frag_Threshold;
}

ULONG
Hw11QueryMaxTXMSDULifeTime(
	__in PADAPTER pAdapter
)
{
	return pAdapter->DupCurrentInfo.MaxTxLifeTime;
}

ULONG
Hw11QueryMaxReceiveLifeTime(
	__in PADAPTER pAdapter
)
{
	return pAdapter->DupCurrentInfo.MaxRxLifeTime;
}

NDIS_STATUS
Hw11QuerySupportedPHYTypes(
	__in  PADAPTER pAdapter,
	__in  ULONG uNumMaxEntries,
	__out PDOT11_SUPPORTED_PHY_TYPES pDot11SupportedPhyTypes
)
{
	USHORT    Index;

	if (uNumMaxEntries < pAdapter->MibInfo.pSupportedPhyTypes->uNumOfEntries)
	{
		pDot11SupportedPhyTypes->uTotalNumOfEntries = pAdapter->MibInfo.pSupportedPhyTypes->uNumOfEntries;
		pDot11SupportedPhyTypes->uNumOfEntries = uNumMaxEntries;

		for (Index = 0; Index < uNumMaxEntries; Index++)
			pDot11SupportedPhyTypes->dot11PHYType[Index] = pAdapter->MibInfo.pSupportedPhyTypes->dot11PHYType[Index];

		return NDIS_STATUS_BUFFER_OVERFLOW;
	}
	else
	{
		pDot11SupportedPhyTypes->uTotalNumOfEntries = pAdapter->MibInfo.pSupportedPhyTypes->uNumOfEntries;
		pDot11SupportedPhyTypes->uNumOfEntries = pAdapter->MibInfo.pSupportedPhyTypes->uNumOfEntries;

		for (Index = 0; Index < pAdapter->MibInfo.pSupportedPhyTypes->uNumOfEntries; Index++)
			pDot11SupportedPhyTypes->dot11PHYType[Index] = pAdapter->MibInfo.pSupportedPhyTypes->dot11PHYType[Index];

		return NDIS_STATUS_SUCCESS;
	}
}

PDOT11_MAC_ADDRESS
Hw11GetMACAddress(
	PADAPTER pAdapter
)
{
	UNREFERENCED_PARAMETER(pAdapter);
	wf_u8 mac[WF_ETH_ALEN]={ 0xb4, 0x04, 0x18, 0xc6, 0x75, 0xf7 };
	nic_info_st *nic_info = wf_kzalloc(sizeof(nic_info_st));
	if (nic_info == NULL) {
		DbgPrint("malloc nic info failed!\n");
		return NDIS_STATUS_FAILURE;
	}
	pAdapter->nic_info = nic_info;
	hw_info_st *hw_info = wf_kzalloc(sizeof(hw_info_st));
	if (hw_info == NULL) {
		DbgPrint("malloc hw info failed!\n");
		return NDIS_STATUS_FAILURE;
	}
	hw_info = nic_info->hw_info;
	//get_efuse_mac(nic_info, hw_info->macAddr);//TBD
	//wf_memcpy(mac, hw_info->macAddr, WF_ETH_ALEN);
	return mac;
}

DOT11_DIVERSITY_SUPPORT
Hw11QueryDiversitySupport(
	__in  PADAPTER pAdapter
)
{
	return pAdapter->MibInfo.DiversitySupport;
}

VOID
Hw11QuerySupportedPowerLevels(
	__in  PADAPTER pAdapter,
	__out PDOT11_SUPPORTED_POWER_LEVELS pDot11SupportedPowerLevels
)
{
	USHORT    Index;

	pDot11SupportedPowerLevels->uNumOfSupportedPowerLevels = pAdapter->MibInfo.SupportedPowerLevels.uNumOfSupportedPowerLevels;
	for (Index = 0; Index < pAdapter->MibInfo.SupportedPowerLevels.uNumOfSupportedPowerLevels; Index++)
		pDot11SupportedPowerLevels->uTxPowerLevelValues[Index] = pAdapter->MibInfo.SupportedPowerLevels.uTxPowerLevelValues[Index];
}

ULONG
Hw11QueryCurrentTXPowerLevel(
	__in PADAPTER pAdapter
)
{
	return pAdapter->MibInfo.CurrentTxPowerLevel;
}

NDIS_STATUS
Hw11GetChannel(
	__in  PADAPTER pAdapter,
	__out PULONG       channel,
	__in  BOOLEAN      selectedPhy
)
{
	PNICPHYMIB PhyMib;

	PhyMib = selectedPhy ? pAdapter->MibInfo.SelectedPhyMIB : pAdapter->MibInfo.OperatingPhyMIB;

	if (PhyMib->PhyType == dot11_phy_type_dsss ||
		PhyMib->PhyType == dot11_phy_type_hrdsss ||
		PhyMib->PhyType == dot11_phy_type_erp ||
		PhyMib->PhyType == dot11_phy_type_ht)
	{
		*channel = (ULONG)PhyMib->Channel;
		return NDIS_STATUS_SUCCESS;
	}
	else
	{
		return NDIS_STATUS_NOT_SUPPORTED;
	}
}

NDIS_STATUS
Hw11QueryCCAModeSupported(
	__in  PADAPTER		pAdapter,
	__out PULONG		value,
	__in  BOOLEAN		selectedPhy
)
{
	PNICPHYMIB PhyMib;

	PhyMib = selectedPhy ? pAdapter->MibInfo.SelectedPhyMIB : pAdapter->MibInfo.OperatingPhyMIB;

	if (PhyMib->PhyType == dot11_phy_type_dsss ||
		PhyMib->PhyType == dot11_phy_type_hrdsss ||
		PhyMib->PhyType == dot11_phy_type_erp ||
		PhyMib->PhyType == dot11_phy_type_ht)
	{
		*value = pAdapter->MibInfo.CCAModeSupported;
		return NDIS_STATUS_SUCCESS;
	}
	else
	{
		return NDIS_STATUS_NOT_SUPPORTED;
	}
}

NDIS_STATUS
Hw11QueryCurrentCCA(
	__in  PADAPTER		pAdapter,
	__out PULONG		value,
	__in  BOOLEAN		selectedPhy
)
{
	PNICPHYMIB PhyMib;

	PhyMib = selectedPhy ? pAdapter->MibInfo.SelectedPhyMIB : pAdapter->MibInfo.OperatingPhyMIB;

	if (PhyMib->PhyType == dot11_phy_type_dsss ||
		PhyMib->PhyType == dot11_phy_type_hrdsss ||
		PhyMib->PhyType == dot11_phy_type_erp ||
		PhyMib->PhyType == dot11_phy_type_ht)
	{
		*value = pAdapter->MibInfo.CurrentCCAMode;
		return NDIS_STATUS_SUCCESS;
	}
	else
	{
		return NDIS_STATUS_NOT_SUPPORTED;
	}
}

NDIS_STATUS
Hw11QueryEdThreshold(
	__in  PADAPTER		pAdapter,
	__out PULONG		value,
	__in  BOOLEAN		selectedPhy
)
{
	PNICPHYMIB PhyMib;

	PhyMib = selectedPhy ? pAdapter->MibInfo.SelectedPhyMIB : pAdapter->MibInfo.OperatingPhyMIB;

	if (PhyMib->PhyType == dot11_phy_type_dsss ||
		PhyMib->PhyType == dot11_phy_type_hrdsss ||
		PhyMib->PhyType == dot11_phy_type_erp ||
		PhyMib->PhyType == dot11_phy_type_ht)
	{
		*value = pAdapter->MibInfo.EDThreshold;
		return NDIS_STATUS_SUCCESS;
	}
	else
	{
		return NDIS_STATUS_NOT_SUPPORTED;
	}
}

NDIS_STATUS
Hw11GetOfdmFrequency(
	__in  PADAPTER		pAdapter,
	__out PULONG		freq,
	__in  BOOLEAN		selectedPhy
)
{
	PNICPHYMIB PhyMib;

	PhyMib = selectedPhy ? pAdapter->MibInfo.SelectedPhyMIB : pAdapter->MibInfo.OperatingPhyMIB;

	if (PhyMib->PhyType == dot11_phy_type_ofdm)
	{
		*freq = (ULONG)PhyMib->Channel;
		return NDIS_STATUS_SUCCESS;
	}
	else
	{
		return NDIS_STATUS_NOT_SUPPORTED;
	}
}

NDIS_STATUS
Hw11FrequencyBandsSupported(
	__in  PADAPTER		pAdapter,
	__out PULONG		value,
	__in  BOOLEAN		selectedPhy
)
{
	PNICPHYMIB PhyMib;

	PhyMib = selectedPhy ? pAdapter->MibInfo.SelectedPhyMIB : pAdapter->MibInfo.OperatingPhyMIB;

	if (PhyMib->PhyType == dot11_phy_type_ofdm)
	{
		*value = DOT11_FREQUENCY_BANDS_LOWER | DOT11_FREQUENCY_BANDS_MIDDLE;
		return NDIS_STATUS_SUCCESS;
	}
	else
	{
		return NDIS_STATUS_INVALID_STATE;
	}
}

NDIS_STATUS
Hw11QueryRegDomainsSupportValue(
	__in  PADAPTER	pAdapter,
	__in  ULONG		uNumMaxEntries,
	__out_ecount(uNumMaxEntries) PDOT11_REG_DOMAINS_SUPPORT_VALUE pDot11RegDomainsSupportValue
)

{
	USHORT    Index;

	if (uNumMaxEntries < pAdapter->MibInfo.pRegDomainsSupportValue->uNumOfEntries)
	{
		pDot11RegDomainsSupportValue->uNumOfEntries = uNumMaxEntries;
		pDot11RegDomainsSupportValue->uTotalNumOfEntries = pAdapter->MibInfo.pRegDomainsSupportValue->uNumOfEntries;
		for (Index = 0; Index<uNumMaxEntries; Index++)
		{
			pDot11RegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex = pAdapter->MibInfo.pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex;
			pDot11RegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportValue = pAdapter->MibInfo.pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportValue;
		}

		return NDIS_STATUS_BUFFER_OVERFLOW;
	}
	else
	{
		pDot11RegDomainsSupportValue->uNumOfEntries = pAdapter->MibInfo.pRegDomainsSupportValue->uNumOfEntries;
		pDot11RegDomainsSupportValue->uTotalNumOfEntries = pAdapter->MibInfo.pRegDomainsSupportValue->uNumOfEntries;

		for (Index = 0; Index < pAdapter->MibInfo.pRegDomainsSupportValue->uNumOfEntries; Index++)
		{
			pDot11RegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex = pAdapter->MibInfo.pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex;
			pDot11RegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportValue = pAdapter->MibInfo.pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportValue;
		}

		return NDIS_STATUS_SUCCESS;
	}
}

NDIS_STATUS
Hw11QuerySupportedTXAntenna(
	__in PADAPTER	pAdapter,
	__in ULONG		uNumMaxEntries,
	__out_ecount(uNumMaxEntries) PDOT11_SUPPORTED_ANTENNA_LIST pDot11SupportedAntennaList
)
{    
	ULONG    i;

	UNREFERENCED_PARAMETER(pAdapter);

	if (uNumMaxEntries < 2) {
		pDot11SupportedAntennaList->uNumOfEntries = 0;
		pDot11SupportedAntennaList->uTotalNumOfEntries = 2;

		return NDIS_STATUS_BUFFER_OVERFLOW;
	}
	else {
		pDot11SupportedAntennaList->uTotalNumOfEntries = 2;

		for (i = 0; i<2 && i<uNumMaxEntries; i++)
		{
			pDot11SupportedAntennaList->dot11SupportedAntenna[i].uAntennaListIndex = i + 1;
			pDot11SupportedAntennaList->dot11SupportedAntenna[i].bSupportedAntenna = TRUE;
		}

		pDot11SupportedAntennaList->uNumOfEntries = 2;

		return NDIS_STATUS_SUCCESS;
	}
}

NDIS_STATUS
Hw11QuerySupportedRXAntenna(
	__in PADAPTER	pAdapter,
	__in ULONG		uNumMaxEntries,
	__out_ecount(uNumMaxEntries) PDOT11_SUPPORTED_ANTENNA_LIST pDot11SupportedAntennaList
)
{    
	return Hw11QuerySupportedTXAntenna(pAdapter, uNumMaxEntries, pDot11SupportedAntennaList);
}

NDIS_STATUS
Hw11QuerySupportedGUIDS(
	__in  PADAPTER	pAdapter
)
{
	UNREFERENCED_PARAMETER(pAdapter);
	return NDIS_STATUS_NOT_SUPPORTED;
}

NDIS_STATUS
Hw11QueryStatistics(
	__in PADAPTER	pAdapter,
	__in NDIS_OID         Oid,
	__inout PVOID        StatBuffer,
	__inout PULONG       BytesWritten
)
{
	ULONG               PhyId;
	ULONGLONG           StatValue = 0;
	NDIS_STATISTICS_INFO    StatStruct;

	NdisZeroMemory(&StatStruct, sizeof(NDIS_STATISTICS_INFO));
	StatStruct.Header.Revision = NDIS_OBJECT_REVISION_1;
	StatStruct.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
	StatStruct.Header.Size = sizeof(NDIS_STATISTICS_INFO);

	StatStruct.SupportedStatistics = NDIS_STATISTICS_FLAGS_VALID_RCV_DISCARDS |
		NDIS_STATISTICS_FLAGS_VALID_RCV_ERROR |
		NDIS_STATISTICS_FLAGS_VALID_XMIT_ERROR |
		NDIS_STATISTICS_FLAGS_VALID_DIRECTED_FRAMES_RCV |
		NDIS_STATISTICS_FLAGS_VALID_DIRECTED_FRAMES_XMIT;

	//
	// Merge phy counters
	//
	for (PhyId = 0;
		PhyId < pAdapter->MibInfo.pSupportedPhyTypes->uNumOfEntries;
		PhyId++
		)
	{
		switch (Oid)
		{
		case OID_GEN_XMIT_OK:
			//StatValue += pAdapter->StatisticsInfo.PhyCounters[PhyId].ullTransmittedFragmentCount;
			StatValue = 0;
			break;

		case OID_GEN_XMIT_ERROR:
			//StatValue += pAdapter->StatisticsInfo.PhyCounters[PhyId].ullFailedCount;
			StatValue = 0;
			break;

		case OID_GEN_RCV_OK:
			//StatValue += pAdapter->StatisticsInfo.PhyCounters[PhyId].ullReceivedFragmentCount;
			StatValue = 0;
			break;

		case OID_GEN_RCV_ERROR:
			//StatValue += pAdapter->StatisticsInfo.PhyCounters[PhyId].ullFCSErrorCount;
			StatValue = 0;
			break;

		case OID_GEN_RCV_CRC_ERROR:
			//StatValue += pAdapter->StatisticsInfo.PhyCounters[PhyId].ullFCSErrorCount;
			StatValue = 0;
			break;

		case OID_GEN_STATISTICS:
			//StatStruct.ifInDiscards += pAdapter->StatisticsInfo.PhyCounters[PhyId].ullFCSErrorCount;
			//StatStruct.ifInErrors += pAdapter->StatisticsInfo.PhyCounters[PhyId].ullFCSErrorCount;
			StatStruct.ifInDiscards = 0;
			StatStruct.ifInErrors = 0;
			//StatStruct.ifHCInUcastPkts += pAdapter->StatisticsInfo.PhyCounters[PhyId].ullReceivedFragmentCount;
			//StatStruct.ifHCOutUcastPkts += pAdapter->StatisticsInfo.PhyCounters[PhyId].ullTransmittedFragmentCount;
			StatStruct.ifHCInUcastPkts = 0;
			StatStruct.ifHCOutUcastPkts = 0;
			//StatStruct.ifOutErrors += pAdapter->StatisticsInfo.PhyCounters[PhyId].ullFailedCount;
			StatStruct.ifOutErrors = 0;
			break;
		}
	}

	//
	// For some OIDs, we add other stats
	//

	switch (Oid)
	{
	case OID_GEN_XMIT_ERROR:
#ifdef USE_NWIFI_SPEC_1_66
		StatValue += pNic->StatisticsInfo.TxError;
#else
		//StatValue += pAdapter->StatisticsInfo.UcastCounters.ullTransmittedFailureFrameCount;
		//StatValue += pAdapter->StatisticsInfo.McastCounters.ullTransmittedFailureFrameCount;
#endif
		break;

	case OID_GEN_RCV_ERROR:
#ifdef USE_NWIFI_SPEC_1_66
		StatValue += pNic->StatisticsInfo.RxError;
#else
		//StatValue += pAdapter->StatisticsInfo.UcastCounters.ullReceivedFailureFrameCount;
		//StatValue += pAdapter->StatisticsInfo.McastCounters.ullReceivedFailureFrameCount;
#endif
		break;

	case OID_GEN_RCV_NO_BUFFER:
		//StatValue = pAdapter->StatisticsInfo.RxNoBuf;
		break;

	case OID_GEN_STATISTICS:
		//StatStruct.ifInDiscards += (pAdapter->StatisticsInfo.RxNoBuf +
		//	pAdapter->StatisticsInfo.UcastCounters.ullReceivedFailureFrameCount +
		//	pAdapter->StatisticsInfo.McastCounters.ullReceivedFailureFrameCount
		//	);

		//StatStruct.ifInErrors += (pAdapter->StatisticsInfo.UcastCounters.ullReceivedFailureFrameCount +
		//	pAdapter->StatisticsInfo.McastCounters.ullReceivedFailureFrameCount
		//	);

		//StatStruct.ifOutErrors += (pAdapter->StatisticsInfo.UcastCounters.ullTransmittedFailureFrameCount +
		//	pAdapter->StatisticsInfo.McastCounters.ullTransmittedFailureFrameCount
		//	);

		break;
	}

	if (Oid == OID_GEN_STATISTICS)
	{
		NdisMoveMemory(StatBuffer, &StatStruct, sizeof(NDIS_STATISTICS_INFO));
		*BytesWritten = sizeof(NDIS_STATISTICS_INFO);
	}
	else
	{
		NdisMoveMemory(StatBuffer, &StatValue, sizeof(ULONGLONG));
		*BytesWritten = sizeof(ULONGLONG);
	}

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QuerySupportedUnicastAlgorithmPair(
	__in  PADAPTER        pAdapter,
	__out_bcount(TotalLength) PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList,
	__in __range(sizeof(DOT11_AUTH_CIPHER_PAIR_LIST) - sizeof(DOT11_AUTH_CIPHER_PAIR), 0x7FFFFFFF) ULONG           TotalLength
)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	ULONG       BytesNeeded = 0;
	ULONG       count;
	BOOLEAN     WEP40Implemented = TRUE;
	BOOLEAN     WEP104Implemented = TRUE;
	BOOLEAN     TKIPImplemented = TRUE;
	BOOLEAN     CCMPImplemented = TRUE;

	do
	{
		count = 1;
		if (WEP40Implemented)
		{
			count++;
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
				count++;
		}

		if (WEP104Implemented)
		{
			count++;
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
				count++;
		}

		if (WEP40Implemented || WEP104Implemented)
		{
			count++;
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
				count++;
		}

		if (TKIPImplemented && pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
		{
			count += 4;
		}

		if (CCMPImplemented)
		{
			count++;
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
				count += 3;
		}

		// Ensure enough space for one entry (though this would
		// get saved as part of the DOT11_AUTH_CIPHER_PAIR_LIST structure
		// itself)
		BytesNeeded = FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) +
			count * sizeof(DOT11_AUTH_CIPHER_PAIR);

		pAuthCipherList->uNumOfEntries = 0;
		pAuthCipherList->uTotalNumOfEntries = count;

		if (TotalLength < BytesNeeded)
		{
			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
			break;
		}

		pAuthCipherList->uNumOfEntries = count;

		count = 0;
		pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
		pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_NONE;

		if (WEP40Implemented)
		{
			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
			{
				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;
			}
		}

		if (WEP104Implemented)
		{
			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
			{
				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;
			}
		}

		if (WEP40Implemented || WEP104Implemented)
		{
			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP;

			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
			{
				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP;
			}
		}

		if (TKIPImplemented && pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
		{
			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;
		}

		if (CCMPImplemented)
		{
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
			{
				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;

				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;

				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;
			}

			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;
		}

	} while (FALSE);

	return ndisStatus;
}

NDIS_STATUS
Sta11QuerySupportedMulticastAlgorithmPair(
	__in  PADAPTER        pAdapter,
	__out_bcount(TotalLength) PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList,
	__in __range(sizeof(DOT11_AUTH_CIPHER_PAIR_LIST) - sizeof(DOT11_AUTH_CIPHER_PAIR), 0x7FFFFFFF)  ULONG           TotalLength
)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	ULONG       BytesNeeded = 0;
	ULONG       count;
	BOOLEAN     WEP40Implemented = TRUE;
	BOOLEAN     WEP104Implemented = TRUE;
	BOOLEAN     TKIPImplemented = TRUE;
	BOOLEAN     CCMPImplemented = TRUE;

	do
	{
		count = 1;

		if (WEP40Implemented)
		{
			count++;
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
			{
				count++;
				if (TKIPImplemented || CCMPImplemented)
					count += 4;
			}
		}

		if (WEP104Implemented)
		{
			count++;
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
			{
				count++;
				if (TKIPImplemented || CCMPImplemented)
					count += 4;
			}
		}

		if (WEP40Implemented || WEP104Implemented)
		{
			count++;
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
				count++;
		}

		if (TKIPImplemented && pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
		{
			count += 4;
		}

		if (CCMPImplemented)
		{
			count++;
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
				count += 3;
		}

		// Ensure enough space for one entry (though this would
		// get saved as part of the DOT11_AUTH_CIPHER_PAIR_LIST structure
		// itself)
		BytesNeeded = FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) +
			count * sizeof(DOT11_AUTH_CIPHER_PAIR);

		pAuthCipherList->uNumOfEntries = 0;
		pAuthCipherList->uTotalNumOfEntries = count;

		if (TotalLength < BytesNeeded)
		{
			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
			break;
		}

		pAuthCipherList->uNumOfEntries = count;

		count = 0;
		pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
		pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_NONE;

		if (WEP40Implemented)
		{
			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
			{
				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

				if (TKIPImplemented || CCMPImplemented)
				{
					count++;
					pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
					pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

					count++;
					pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
					pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

					count++;
					pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
					pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

					count++;
					pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
					pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP40;

				}
			}
		}

		if (WEP104Implemented)
		{
			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
			{
				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

				if (TKIPImplemented || CCMPImplemented)
				{
					count++;
					pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
					pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

					count++;
					pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
					pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

					count++;
					pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
					pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

					count++;
					pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
					pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP104;

				}
			}
		}

		if (WEP40Implemented || WEP104Implemented)
		{
			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_OPEN;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP;

			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
			{
				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_80211_SHARED_KEY;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_WEP;
			}
		}

		if (TKIPImplemented && pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
		{
			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;

			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_TKIP;
		}

		if (CCMPImplemented)
		{
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_infrastructure)
			{
				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;

				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_WPA_PSK;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;

				count++;
				pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA;
				pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;
			}

			count++;
			pAuthCipherList->AuthCipherPairs[count].AuthAlgoId = DOT11_AUTH_ALGO_RSNA_PSK;
			pAuthCipherList->AuthCipherPairs[count].CipherAlgoId = DOT11_CIPHER_ALGO_CCMP;
		}

	} while (FALSE);

	return ndisStatus;
}


NDIS_STATUS Sta11DisconnectInfra(PADAPTER pAdapter)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	ndisStatus = wf_oids_deassoc(pAdapter);

    return ndisStatus;
}

NDIS_STATUS
Sta11QueryEnabledAuthenticationAlgorithm(
	__in  PADAPTER        pAdapter,
	__out PDOT11_AUTH_ALGORITHM_LIST  pAuthAlgoList,
	__in  ULONG           TotalLength
)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	ULONG       BytesNeeded = 0;

	do
	{
		// Ensure enough space for one entry (though this would
		// get saved as part of the DOT11_AUTH_ALGORITHM_LIST structure
		// itself)

		BytesNeeded = FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds)
			+ 1 * sizeof(DOT11_AUTH_ALGORITHM);

		if (TotalLength < BytesNeeded)
		{
			pAuthAlgoList->uNumOfEntries = 0;
			pAuthAlgoList->uTotalNumOfEntries = 1;
			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
			break;
		}

		pAuthAlgoList->uNumOfEntries = 1;
		pAuthAlgoList->uTotalNumOfEntries = 1;
		pAuthAlgoList->AlgorithmIds[0] = pAdapter->MibInfo.AuthAlgorithm;
	} while (FALSE);

	return ndisStatus;
}

NDIS_STATUS
Sta11QueryEnabledMulticastCipherAlgorithm(
	__in  PADAPTER        pAdapter,
	__out PDOT11_CIPHER_ALGORITHM_LIST  pAuthCipherList,
	__in  ULONG           TotalLength
)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	ULONG       BytesNeeded = 0;
	do
	{
		// Integer overflow
		if (FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) >
			FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
			pAdapter->MibInfo.MulticastCipherAlgorithmCount * sizeof(DOT11_CIPHER_ALGORITHM))
		{
			return NDIS_STATUS_FAILURE;
		}

		// Ensure enough space for one entry (though this would
		// get saved as part of the DOT11_CIPHER_ALGORITHM_LIST structure
		// itself)

		BytesNeeded = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
			pAdapter->MibInfo.MulticastCipherAlgorithmCount * sizeof(DOT11_CIPHER_ALGORITHM);

		if (TotalLength < BytesNeeded)
		{
			pAuthCipherList->uNumOfEntries = 0;
			pAuthCipherList->uTotalNumOfEntries = pAdapter->MibInfo.MulticastCipherAlgorithmCount;
			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
			break;
		}

		pAuthCipherList->uNumOfEntries = pAdapter->MibInfo.MulticastCipherAlgorithmCount;
		pAuthCipherList->uTotalNumOfEntries = pAdapter->MibInfo.MulticastCipherAlgorithmCount;
		NdisMoveMemory(pAuthCipherList->AlgorithmIds,
			pAdapter->MibInfo.MulticastCipherAlgorithmList,
			pAdapter->MibInfo.MulticastCipherAlgorithmCount * sizeof(DOT11_CIPHER_ALGORITHM));

	} while (FALSE);

	return ndisStatus;
}



VOID
StaSetDefaultCipher(
	__in PADAPTER pAdapter
)
{
	pwf_mib_info_t     pMibInfo = &pAdapter->MibInfo;
	BOOLEAN                 WEP40Implemented = TRUE;
	BOOLEAN                 WEP104Implemented = TRUE;
	BOOLEAN                 TKIPImplemented = TRUE;
	BOOLEAN                 CCMPImplemented = TRUE;
	UINT					index_uni = 0;
	ULONG                   index = 0;

	switch (pMibInfo->AuthAlgorithm)
	{
	case DOT11_AUTH_ALGO_80211_OPEN:
		if (WEP104Implemented || WEP40Implemented)
		{
			pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_WEP;
			pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP;
		}
		else
		{
			pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_NONE;
		}

		if (WEP104Implemented)
		{
			pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_WEP104;
			pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP104;
		}

		if (WEP40Implemented)
		{
			pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_WEP40;
			pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP40;
		}

		pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_NONE;

		break;

	case DOT11_AUTH_ALGO_80211_SHARED_KEY:
		ASSERT(WEP104Implemented || WEP40Implemented);
		ASSERT(pMibInfo->BSSType == dot11_BSS_type_infrastructure);

		pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_WEP;
		pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP;

		if (WEP104Implemented)
		{
			pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_WEP104;
			pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP104;
		}

		if (WEP40Implemented)
		{
			pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_WEP40;
			pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP40;
		}

		break;

	case DOT11_AUTH_ALGO_WPA:
	case DOT11_AUTH_ALGO_WPA_PSK:
	case DOT11_AUTH_ALGO_RSNA:
		ASSERT(pMibInfo->BSSType == dot11_BSS_type_infrastructure);
		// fall through

	case DOT11_AUTH_ALGO_RSNA_PSK:
		ASSERT(TKIPImplemented || CCMPImplemented);
		if (CCMPImplemented)
		{
			pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_CCMP;
		}

		if (CCMPImplemented)
		{
			pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_CCMP;
		}

		if (pMibInfo->BSSType == dot11_BSS_type_infrastructure)
		{
			if (TKIPImplemented)
			{
				pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_TKIP;
				pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_TKIP;
			}

			if (WEP104Implemented)
			{
				pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP104;
				pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_WEP104;
			}

			if (WEP40Implemented)
			{
				pMibInfo->MulticastCipherAlgorithmList[index++] = DOT11_CIPHER_ALGO_WEP40;
				pMibInfo->UnicastCipherAlgorithmList[index_uni++] = DOT11_CIPHER_ALGO_WEP40;
			}
		}

		break;

	default:
		ASSERT(0);
		return;
	}

	pMibInfo->MulticastCipherAlgorithmCount = index;
	pMibInfo->UnicastCipherAlgorithmCount = index_uni;
	if (index > 1)
	{
		pMibInfo->MulticastCipherAlgorithm = DOT11_CIPHER_ALGO_NONE;
	}
	else
	{
		pMibInfo->MulticastCipherAlgorithm = pMibInfo->MulticastCipherAlgorithmList[0];
	}

	if (index_uni > 1)
	{
		pMibInfo->UnicastCipherAlgorithm = DOT11_CIPHER_ALGO_NONE;
	}
	else
	{
		pMibInfo->UnicastCipherAlgorithm = pMibInfo->UnicastCipherAlgorithmList[0];
	}

	Hw11SetEncryption(pAdapter, TRUE, pMibInfo->UnicastCipherAlgorithm);
	Hw11SetEncryption(pAdapter, FALSE, pMibInfo->MulticastCipherAlgorithm);
}

VOID
Hw11SetEncryption(
	__in  PADAPTER pAdapter,
	__in  BOOLEAN isUnicast,
	__in  DOT11_CIPHER_ALGORITHM  algoId
)
{
	// TODO: Set to hardware. (Shared key)   2021/04/17

	
	nic_info_st *pnic_info = pAdapter->nic_info;
	sec_info_st *psec_info = pnic_info->sec_info;

	BOOLEAN useDefaultKey;
	/*** Valid combinations of multicast/unicast cipher algorithms

	Multicast\Unicast       WEP      WEP40      WEP104      TKIP     CCMP      NONE
	WEP                  Y
	WEP40                          Y*                     Y        Y
	WEP104                                    Y*          Y        Y
	TKIP                                                  Y        Y
	CCMP                                                           Y
	NONE                                                                    Y

	Y*: Not currently used by Microsoft 802.11 Security module but can be supported by vendors
	if they want to connect to WPA AP that does not support pairwise keys.

	***/

	//
	// Update the encryption algorithm. When set unicast cipher, also clear all keys. 
	//
	if (isUnicast) {
		pAdapter->MibInfo.HwUnicastCipher = algoId;
		psec_info->dot11PrivacyAlgrthm = pAdapter->MibInfo.UnicastCipherAlgorithm;
		useDefaultKey = (BOOLEAN)(!(pAdapter->MibInfo.CurrentBSSType == dot11_BSS_type_independent &&
			pAdapter->MibInfo.HwAuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK));
		DbgPrint("%s===>algoId=%d DefaultKey=%d\n", __func__, algoId, useDefaultKey);
	}
	else {
		pAdapter->MibInfo.HwMulticastCipher = algoId;
		psec_info->dot118021XGrpPrivacy = pAdapter->MibInfo.MulticastCipherAlgorithm;
	}
}

NDIS_STATUS
Sta11SetEnabledAuthenticationAlgorithm(
	__in  PADAPTER       pAdapter,
	__in  PDOT11_AUTH_ALGORITHM_LIST  pAuthAlgoList,
	__out PULONG          pBytesRead
)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	__try
	{
		// Only support one authentication algorithm
		if (pAuthAlgoList->uNumOfEntries != 1)
		{
			*pBytesRead = FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds);
			ndisStatus = NDIS_STATUS_INVALID_LENGTH;
			__leave;
		}

		*pBytesRead = FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds) +
			1 * sizeof(DOT11_AUTH_ALGORITHM);

		// 
		// Check if we support the specified auth algorithm.
		//
		switch (pAuthAlgoList->AlgorithmIds[0])
		{
		case DOT11_AUTH_ALGO_80211_OPEN:
			break;
		case DOT11_AUTH_ALGO_80211_SHARED_KEY:
			break;

		case DOT11_AUTH_ALGO_WPA:
		case DOT11_AUTH_ALGO_WPA_PSK:
		case DOT11_AUTH_ALGO_RSNA:
			if (pAdapter->MibInfo.BSSType == dot11_BSS_type_independent)
			{
				ndisStatus = NDIS_STATUS_INVALID_DATA;
				__leave;
			}
		case DOT11_AUTH_ALGO_RSNA_PSK:
			break;

		default:
			ndisStatus = NDIS_STATUS_INVALID_DATA;
			__leave;
		}

		// Copy the data locally
		if (pAdapter->MibInfo.AuthAlgorithm != pAuthAlgoList->AlgorithmIds[0])
		{
			pAdapter->MibInfo.AuthAlgorithm = pAuthAlgoList->AlgorithmIds[0];
			// reload enabled unicast and multicast cipher based on current bss type and auth algo.
			StaSetDefaultCipher(pAdapter);
		}

		//
		// Tell HW layer of the auth algorithm
		//
		//Hw11SetAuthentication(pStation->pNic, pMibInfo->AuthAlgorithm);
		pAdapter->MibInfo.HwAuthAlgorithm = pAdapter->MibInfo.AuthAlgorithm;

		// We dont need to process anything just yet
		// store it and we will use it when the connect
		// request comes in        
		ndisStatus = NDIS_STATUS_SUCCESS;
	}
	__finally
	{
	}

	return ndisStatus;
}

NDIS_STATUS
Sta11SetEnabledUnicastCipherAlgorithm(
	__in  PADAPTER        pAdapter,
	__in  PDOT11_CIPHER_ALGORITHM_LIST  pCipherAlgoList,
	__out PULONG          pBytesRead
)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	__try
	{

		// Support at most 6 ciphers.
		if (pCipherAlgoList->uNumOfEntries > 6)
		{
			*pBytesRead = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);
			ndisStatus = NDIS_STATUS_INVALID_LENGTH;
			__leave;
		}

		if(pCipherAlgoList->uTotalNumOfEntries < pAdapter->MibInfo.UnicastCipherAlgorithmCount)
			pAdapter->MibInfo.UnicastCipherAlgorithmCount = pCipherAlgoList->uTotalNumOfEntries;
		
		*pBytesRead = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
			pCipherAlgoList->uNumOfEntries * sizeof(DOT11_CIPHER_ALGORITHM);

		//
		// Check if we can support the cipher algorithms given current auth algorithm.
		// Not neccessary.
		//
		if (!StaValidateUnicastAuthCipherPair(pAdapter,
			pAdapter->MibInfo.AuthAlgorithm,
			pCipherAlgoList->AlgorithmIds[0]))
		{
			ndisStatus = NDIS_STATUS_INVALID_DATA;
			__leave;
		}
		for (int id_num=0; id_num < pCipherAlgoList->uNumOfEntries; id_num++)
		{
			// Copy the data locally
			pAdapter->MibInfo.UnicastCipherAlgorithmList[id_num] = pCipherAlgoList->AlgorithmIds[id_num];
		}
		pAdapter->MibInfo.UnicastCipherAlgorithmCount = pCipherAlgoList->uNumOfEntries;
		pAdapter->MibInfo.UnicastCipherAlgorithm = pCipherAlgoList->AlgorithmIds[0];
		Hw11SetEncryption(pAdapter, TRUE, pCipherAlgoList->AlgorithmIds[0]);

		ndisStatus = NDIS_STATUS_SUCCESS;
	}
	__finally
	{
	}

	return ndisStatus;
}

NDIS_STATUS
Sta11SetExcludeUnencrypted(
	__in   PADAPTER  pAdapter,
	__in   BOOLEAN param
)
{
	pAdapter->MibInfo.ExcludeUnencrypted = param;
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11SetPrivacyExemptionList(
	__in   PADAPTER                     pAdapter,
	__in   PDOT11_PRIVACY_EXEMPTION_LIST list,
	__out  PULONG                        pBytesRead
)
{
	PDOT11_PRIVACY_EXEMPTION_LIST   PrivacyExemptionList = pAdapter->MibInfo.PrivacyExemptionList;
	ULONG                           size;

	//
	// Check if we already have the buffer allocated for storing privacy exemption list.
	// If we don't, or if the buffer isn't big enough, allocate a buffer.
	//

	size = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) +
		list->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION);

	if (!PrivacyExemptionList ||
		PrivacyExemptionList && PrivacyExemptionList->uTotalNumOfEntries < list->uNumOfEntries)
	{
		MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle,
			&PrivacyExemptionList,
			size,
			STA11_MEMORY_TAG);
		if (PrivacyExemptionList == NULL)
		{
			*pBytesRead = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries);
			return NDIS_STATUS_RESOURCES;
		}

		PrivacyExemptionList->uTotalNumOfEntries = list->uNumOfEntries;
	}

	//
	// Copy the new privacy exemption list
	//

	PrivacyExemptionList->uNumOfEntries = list->uNumOfEntries;
	if (list->uNumOfEntries)
	{
		NdisMoveMemory(PrivacyExemptionList->PrivacyExemptionEntries,
			list->PrivacyExemptionEntries,
			list->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION));
	}

	//
	// If new buffer is allocated, free the existing buffer if any.
	//

	if (pAdapter->MibInfo.PrivacyExemptionList && pAdapter->MibInfo.PrivacyExemptionList != PrivacyExemptionList)
	{
		MP_FREE_MEMORY(pAdapter->MibInfo.PrivacyExemptionList);
	}

	//
	// Set the new buffer as the one for the current privacy exemption list.
	//

	pAdapter->MibInfo.PrivacyExemptionList = PrivacyExemptionList;

	*pBytesRead = size;
	return (NDIS_STATUS_SUCCESS);
}

BOOLEAN
StaValidateUnicastAuthCipherPair(
	__in PADAPTER pAdapter,
	__in DOT11_AUTH_ALGORITHM AuthAlgo,
	__in DOT11_CIPHER_ALGORITHM CipherAlgo
)
{
	BOOLEAN                 WEP40Implemented = TRUE;
	BOOLEAN                 WEP104Implemented = TRUE;
	BOOLEAN                 TKIPImplemented = TRUE;
	BOOLEAN                 CCMPImplemented = TRUE;
	UNREFERENCED_PARAMETER(pAdapter);
	switch (AuthAlgo)
	{
	case DOT11_AUTH_ALGO_80211_OPEN:
		return (BOOLEAN)((CipherAlgo == DOT11_CIPHER_ALGO_WEP && (WEP40Implemented || WEP104Implemented)) ||
			(CipherAlgo == DOT11_CIPHER_ALGO_WEP104 && WEP104Implemented) ||
			(CipherAlgo == DOT11_CIPHER_ALGO_WEP40 && WEP40Implemented) ||
			(CipherAlgo == DOT11_CIPHER_ALGO_NONE));

	case DOT11_AUTH_ALGO_80211_SHARED_KEY:
		return (BOOLEAN)((CipherAlgo == DOT11_CIPHER_ALGO_WEP && (WEP40Implemented || WEP104Implemented)) ||
			(CipherAlgo == DOT11_CIPHER_ALGO_WEP104 && WEP104Implemented) ||
			(CipherAlgo == DOT11_CIPHER_ALGO_WEP40 && WEP40Implemented));

	case DOT11_AUTH_ALGO_WPA:
	case DOT11_AUTH_ALGO_WPA_PSK:
	case DOT11_AUTH_ALGO_RSNA:
	case DOT11_AUTH_ALGO_RSNA_PSK:
		return (BOOLEAN)((CipherAlgo == DOT11_CIPHER_ALGO_TKIP && TKIPImplemented) ||
			(CipherAlgo == DOT11_CIPHER_ALGO_CCMP && CCMPImplemented));

	default:
		ASSERT(0);
		return FALSE;
	}
}

NDIS_STATUS
Sta11SetExcludedMACAddressList(
	__in  PADAPTER        pAdapter,
	__in  PDOT11_MAC_ADDRESS_LIST pDot11MacAddrList,
	__out PULONG          pBytesRead
)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(pDot11MacAddrList);
	UNREFERENCED_PARAMETER(pBytesRead);
	// The excluded MAC address list is not needed now. 
	// If the querying OID OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST is found in the future,
	// we will have to complete it.     2021/03/25

	return ndisStatus;
}

NDIS_STATUS  Sta11SetDesiredBSSType(
	PADAPTER pAdapter,
	DOT11_BSS_TYPE  bssType
)
{
	if (bssType == dot11_BSS_type_any)
		bssType = dot11_BSS_type_infrastructure;
	if (bssType != dot11_BSS_type_infrastructure)
	{
		DbgPrint("Only infrastructure is supported.\n");
		return NDIS_STATUS_FAILURE;
	}
	//StaSetPowerSavingLevel();
	StaResetCipherAbilities(pAdapter);
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Sta11QueryPrivacyExemptionList(
	__in   PADAPTER                      pAdapter,
	__out_bcount(TotalLength) PDOT11_PRIVACY_EXEMPTION_LIST list,
	__in  __range(sizeof(DOT11_PRIVACY_EXEMPTION_LIST) - sizeof(DOT11_PRIVACY_EXEMPTION), 0x7FFFFFFF) ULONG                         TotalLength
)
{
	PDOT11_PRIVACY_EXEMPTION_LIST   PrivacyExemptionList = pAdapter->MibInfo.PrivacyExemptionList;

	//
	// If we don't have privacy exemption list, simply return success with number of entries set to 0.
	//

	list->uNumOfEntries = 0;
	if (!PrivacyExemptionList || !PrivacyExemptionList->uNumOfEntries) {
		list->uTotalNumOfEntries = 0;
		return NDIS_STATUS_SUCCESS;
	}

	// Integer overflow
	if (FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) >
		FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) +
		PrivacyExemptionList->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION))
	{
		return NDIS_STATUS_FAILURE;
	}

	//
	// Check if we have enough space to copy all lists. If not, simply fail the request.
	// we don't copy partial list.
	//

	list->uTotalNumOfEntries = PrivacyExemptionList->uNumOfEntries;
	if (TotalLength < FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) +
		PrivacyExemptionList->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION))
	{
		return NDIS_STATUS_BUFFER_OVERFLOW;
	}

	//
	// We have enough space, copy all lists.
	//

	list->uNumOfEntries = PrivacyExemptionList->uNumOfEntries;
	NdisMoveMemory(list->PrivacyExemptionEntries,
		PrivacyExemptionList->PrivacyExemptionEntries,
		PrivacyExemptionList->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION));

	return NDIS_STATUS_SUCCESS;
}

