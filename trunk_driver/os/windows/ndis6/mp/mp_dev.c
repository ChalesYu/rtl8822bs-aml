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
#include "wf_debug.h"

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


#define HW_REG_DOMAIN_MKK1          0x00000041
#define HW_REG_DOMAIN_ISRAEL        0x00000042

#define cESS   (0x01)

#define FillOctetString(_os,_octet,_len)        \
    (_os).Octet=(PUCHAR)(_octet);                   \
    (_os).Length=(_len)

/**
 * This function is written with reference to the previous function HwInitializeVariable.
 * Just initialize NIC related information required by the OS. 
 */
NDIS_STATUS
MpInitializeNicVariable(__in  PADAPTER pAdapter)
{
	NDIS_STATUS ndisStatus;
	//ULONG       size;
	ndisStatus = NDIS_STATUS_RESOURCES;
	USHORT    Index;
	//NDIS_HANDLE MiniportAdapterHandle = pAdapter->MiniportAdapterHandle;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

	pAdapter->DupCurrentInfo.ShortRetryLimit = 7;
	pAdapter->DupCurrentInfo.LongRetryLimit = 7;
	pAdapter->DupCurrentInfo.EarlyRxThreshold = 7;
    mib_info->BSSType = dot11_BSS_type_infrastructure;

	//wf_mib_info_t *MibInfo = &pAdapter->MibInfo;
	ULONG curPhyId;
	//
	// management info
	//
	RtlCopyMemory((PCHAR)mib_info->SSID, "SCI ad-hoc", sizeof("SCI ad-hoc"));
	FillOctetString(mib_info->SupportedRates, mib_info->SupportedRatesBuf, 0);
	FillOctetString(mib_info->Regdot11OperationalRateSet, mib_info->Regdot11OperationalRateBuf, 0);
	mib_info->pSupportedPhyTypes->uNumOfEntries = 0;
	mib_info->pSupportedPhyTypes->uTotalNumOfEntries = MAX_NUM_PHY_TYPES;
	//Set operation mode.
	mib_info->CurrentOperationMode.uCurrentOpMode = DOT11_OPERATION_MODE_EXTENSIBLE_STATION;
	mib_info->CurrentOperationMode.uReserved = 0;

	mib_info->OperationModeCapability.uOpModeCapability = DOT11_OPERATION_MODE_EXTENSIBLE_STATION
		| DOT11_OPERATION_MODE_NETWORK_MONITOR;

	//B mode
	{
		curPhyId = WF_PHY_ID_B;
		mib_info->pSupportedPhyTypes->dot11PHYType[curPhyId] = dot11_phy_type_hrdsss;
		mib_info->PhyMIB[curPhyId].PhyID = curPhyId;
		mib_info->PhyMIB[curPhyId].PhyType = dot11_phy_type_hrdsss; //5
		
		mib_info->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength = 4;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[0] = 2;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[1] = 4;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[2] = 11;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[3] = 22;

		NdisZeroMemory(
			&(mib_info->PhyMIB[curPhyId].SupportedDataRatesValue),
			sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2));
		NdisMoveMemory(
			mib_info->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedTxDataRatesValue,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
		NdisMoveMemory(
			mib_info->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedRxDataRatesValue,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
		mib_info->pSupportedPhyTypes->uNumOfEntries++;
	}
	//G mode	To do:need to determine which wirelessmode
	{
		curPhyId = WF_PHY_ID_G;
		mib_info->pSupportedPhyTypes->dot11PHYType[curPhyId] = dot11_phy_type_erp;
		mib_info->PhyMIB[curPhyId].PhyID = 0;
		mib_info->PhyMIB[curPhyId].PhyType = dot11_phy_type_erp; //6
	
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[0] = 0x2;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[1] = 0x4;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[2] = 0xB;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[3] = 0x16;// 0x0c;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[4] = 0x0c;// 0x12;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[5] = 0x12;// 0x16;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[6] = 0x18;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[7] = 0x24;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[8] = 0x30;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[9] = 0x48;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[10] = 0x60;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[11] = 0x6c;

		mib_info->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength = 12;
		//
		NdisZeroMemory(
			&(mib_info->PhyMIB[curPhyId].SupportedDataRatesValue),
			sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2));
		NdisMoveMemory(mib_info->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedTxDataRatesValue,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
		NdisMoveMemory(mib_info->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedRxDataRatesValue,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
		mib_info->pSupportedPhyTypes->uNumOfEntries++;
	}
	
	//N mode
	{
		curPhyId = WF_PHY_ID_N;
		mib_info->pSupportedPhyTypes->dot11PHYType[curPhyId] = dot11_phy_type_ht;
		mib_info->PhyMIB[curPhyId].PhyID = curPhyId;
		mib_info->PhyMIB[curPhyId].PhyType = dot11_phy_type_ht; //6
	
		mib_info->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength = 16;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[0] = 30;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[1] = 60;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[2] = 90;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[3] = 120;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[4] = 180;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[5] = 240;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[6] = (UCHAR)270;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[7] = (UCHAR)300;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[8] = (UCHAR)60;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[9] = (UCHAR)120;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[10] = (UCHAR)180;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[11] = (UCHAR)240;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[12] = (UCHAR)360;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[13] = (UCHAR)480;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[14] = (UCHAR)540;
		mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet[15] = (UCHAR)600;

		NdisZeroMemory(
			&(mib_info->PhyMIB[curPhyId].SupportedDataRatesValue),
			sizeof(DOT11_SUPPORTED_DATA_RATES_VALUE_V2));
		NdisMoveMemory(
			mib_info->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedTxDataRatesValue,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
		NdisMoveMemory(
			mib_info->PhyMIB[curPhyId].SupportedDataRatesValue.ucSupportedRxDataRatesValue,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.ucRateSet,
			mib_info->PhyMIB[curPhyId].OperationalRateSet.uRateSetLength);
		mib_info->pSupportedPhyTypes->uNumOfEntries++;
	}

#if 1
	//pAdapter->RxInfo.ReceiveProcessingFunction = HwProcessReceiveSTAMode;

	pAdapter->DupCurrentInfo.RTS_Threshold = 2346;	   //2346 vs 2347	 dot11RtsThreshold

	pAdapter->DupCurrentInfo.Frag_Threshold = 2346;// 2346 vs 2400  FragThreshold
#endif
	mib_info->PhyMIB[0].Channel = 11;  //==> Regdot11ChannelNumber
	mib_info->DtimPeriod = 2;    

	pAdapter->DupCurrentInfo.BeaconInterval = 100;	 //==> pMgntInfo->Regdot11BeaconPeriod 

	mib_info->CapInfo = cESS |  cIBSS /*|cShortPreamble*/;	   //=> RegmCap mitesh cESS | cIBSS  vs    cESS


	mib_info->bScanCompleteIndicated = FALSE ;
	mib_info->bScanInProgress = FALSE;

	mib_info->uMPDUMaxLength = MAX_MPDU_LENGTH;

	mib_info->EDThreshold = -65;
	mib_info->ShortSlotTimeOptionEnabled = FALSE;
#if 1
	pAdapter->DupCurrentInfo.MediaOccupancyLimit = 100;
	pAdapter->DupCurrentInfo.CFPPeriod = 255;
	pAdapter->DupCurrentInfo.CFPMaxDuration = 100;

	pAdapter->DupCurrentInfo.MaxTxLifeTime = 512;
	pAdapter->DupCurrentInfo.MaxRxLifeTime = 512;
	pAdapter->DupCurrentInfo.DefaultBeaconRate = 2;		  // Set default beacon rate to 2 Mbps

#endif
	mib_info->SwChnlInProgress = FALSE;

	mib_info->PowerMgmtMode.dot11PowerMode = dot11_power_mode_active;
	mib_info->PowerMgmtMode.usAID = 0;
	//NdisAllocateSpinLock(&Nic->ManagementInfo.PowerMgmtLock);

	mib_info->AtimWindow = 0;    /* set only when power management is enabled */

	mib_info->DtimCount = mib_info->DtimPeriod - 1;

	mib_info->NextBeaconIndex = 0;

	mib_info->Tim.BitmapCtrl = mib_info->Tim.PartialVirtualBitmap[0] = 0;

#if 0
	// These variable should initialize from registry
	for (Index = 0; Index < DUPLICATE_DETECTION_CACHE_LENGTH; Index++) {
		NdisZeroMemory(&Nic->RxInfo.DupePacketCache[Index], sizeof(DUPE_CACHE_ENTRY));
	}
	Nic->RxInfo.NextDupeCacheIndex = 0;


	if (Nic->DupCurrentInfo.EarlyRxThreshold == 7)				   // Turn OnlyErlPkt when early Rx is turn off
		Nic->RxInfo.ReceiveConfig |= RCR_OnlyErlPkt;
#endif

	mib_info->CCAModeSupported = DOT11_CCA_MODE_CS_ONLY;
	mib_info->CurrentCCAMode = DOT11_CCA_MODE_CS_ONLY;

	mib_info->OffloadCapability.uReserved = 0;
	mib_info->OffloadCapability.uFlags = 0;
	mib_info->OffloadCapability.uSupportedWEPAlgorithms = 0;
	mib_info->OffloadCapability.uNumOfReplayWindows = 0;
	mib_info->OffloadCapability.uMaxWEPKeyMappingLength = 0;
	mib_info->OffloadCapability.uSupportedAuthAlgorithms = 0;
	mib_info->OffloadCapability.uMaxAuthKeyMappingLength = 0;

	mib_info->CurrentOffloadCapability.uReserved = 0;
	mib_info->CurrentOffloadCapability.uFlags = 0;

	mib_info->OperationModeCapability.uReserved = 0;
	mib_info->OperationModeCapability.uMajorVersion = MP_OPERATION_MODE_CAPABILITY_MAJOR_VERSION;
	mib_info->OperationModeCapability.uMinorVersion = MP_OPERATION_MODE_CAPABILITY_MINOR_VERSION;
	// We can buffer as many packets as OS sends.
	mib_info->OperationModeCapability.uNumOfTXBuffers = 0;
	mib_info->OperationModeCapability.uNumOfRXBuffers = 64;

	mib_info->OptionalCapability.uReserved = 0;
	mib_info->OptionalCapability.bDot11PCF = FALSE;
	mib_info->OptionalCapability.bDot11PCFMPDUTransferToPC = FALSE;
	mib_info->OptionalCapability.bStrictlyOrderedServiceClass = FALSE;

	mib_info->CurrentOptionalCapability.uReserved = 0;
	mib_info->CurrentOptionalCapability.bDot11CFPollable = FALSE;
	mib_info->CurrentOptionalCapability.bDot11PCF = FALSE;
	mib_info->CurrentOptionalCapability.bDot11PCFMPDUTransferToPC = FALSE;
	mib_info->CurrentOptionalCapability.bStrictlyOrderedServiceClass = FALSE;

#if 0
	MibInfo->pSupportedPhyTypes->dot11PHYType[0] = dot11_phy_type_erp;
	MibInfo->pSupportedPhyTypes->dot11PHYType[1] = dot11_phy_type_hrdsss;
	MibInfo->pSupportedPhyTypes->dot11PHYType[2] = dot11_phy_type_ht;
	MibInfo->PhyMIB[0].PhyType = dot11_phy_type_erp;
	MibInfo->PhyMIB[1].PhyType = dot11_phy_type_hrdsss;
	MibInfo->PhyMIB[2].PhyType = dot11_phy_type_ht;
#endif

	for (Index = 4; Index < MAX_NUM_SUPPORTED_RATES_V2; Index++) {
		mib_info->PhyMIB[0].SupportedDataRatesValue.ucSupportedTxDataRatesValue[Index] = 0;
		mib_info->PhyMIB[0].SupportedDataRatesValue.ucSupportedRxDataRatesValue[Index] = 0;
	}

	mib_info->CurrentBeaconRate = 2 * 2;	  // 1Mbps = 2, 2Mbps = 4
	mib_info->CurrentTXDataRate = 4;		  // Set default data rate to 2 Mbps

	mib_info->OperatingPhyId = WF_PHY_ID_DEFAULT;
	mib_info->SelectedPhyId = WF_PHY_ID_DEFAULT;
	mib_info->OperatingPhyMIB = &mib_info->PhyMIB[mib_info->OperatingPhyId];
	mib_info->SelectedPhyMIB = &mib_info->PhyMIB[mib_info->SelectedPhyId];

	mib_info->DiversitySupport = dot11_diversity_support_dynamic;

	mib_info->SupportedPowerLevels.uNumOfSupportedPowerLevels = 4;
	mib_info->SupportedPowerLevels.uTxPowerLevelValues[0] = 10;
	mib_info->SupportedPowerLevels.uTxPowerLevelValues[1] = 20;
	mib_info->SupportedPowerLevels.uTxPowerLevelValues[2] = 30;
	mib_info->SupportedPowerLevels.uTxPowerLevelValues[3] = 50;
	mib_info->CurrentTxPowerLevel = 1;	// 1 based

	mib_info->pRegDomainsSupportValue->uNumOfEntries = 7;
	mib_info->pRegDomainsSupportValue->uTotalNumOfEntries = MAX_NUM_DOT11_REG_DOMAINS_VALUE;
	for (Index=0; Index < mib_info->pRegDomainsSupportValue->uNumOfEntries; Index++)
		mib_info->pRegDomainsSupportValue->dot11RegDomainValue[Index].uRegDomainsSupportIndex = Index;

	mib_info->pRegDomainsSupportValue->dot11RegDomainValue[0].uRegDomainsSupportValue = DOT11_REG_DOMAIN_OTHER;
	mib_info->pRegDomainsSupportValue->dot11RegDomainValue[1].uRegDomainsSupportValue = DOT11_REG_DOMAIN_FCC;
	mib_info->pRegDomainsSupportValue->dot11RegDomainValue[2].uRegDomainsSupportValue = DOT11_REG_DOMAIN_DOC;
	mib_info->pRegDomainsSupportValue->dot11RegDomainValue[3].uRegDomainsSupportValue = DOT11_REG_DOMAIN_ETSI;
	mib_info->pRegDomainsSupportValue->dot11RegDomainValue[4].uRegDomainsSupportValue = DOT11_REG_DOMAIN_SPAIN;
	mib_info->pRegDomainsSupportValue->dot11RegDomainValue[5].uRegDomainsSupportValue = DOT11_REG_DOMAIN_FRANCE;
	mib_info->pRegDomainsSupportValue->dot11RegDomainValue[6].uRegDomainsSupportValue = DOT11_REG_DOMAIN_MKK;
	// These two are supported, but the OS doesnt recognize these and hence we wont report these to the OS (NumOfEntries = 7)
	mib_info->pRegDomainsSupportValue->dot11RegDomainValue[7].uRegDomainsSupportValue = HW_REG_DOMAIN_MKK1;
	mib_info->pRegDomainsSupportValue->dot11RegDomainValue[8].uRegDomainsSupportValue = HW_REG_DOMAIN_ISRAEL;

	mib_info->pDiversitySelectionRxList->uNumOfEntries = 2; 
	mib_info->pDiversitySelectionRxList->uTotalNumOfEntries = MAX_NUM_DIVERSITY_SELECTION_RX_LIST;
	mib_info->pDiversitySelectionRxList->dot11DiversitySelectionRx[0].uAntennaListIndex = 1;
	mib_info->pDiversitySelectionRxList->dot11DiversitySelectionRx[0].bDiversitySelectionRX = TRUE;
	mib_info->pDiversitySelectionRxList->dot11DiversitySelectionRx[1].uAntennaListIndex = 2;
	mib_info->pDiversitySelectionRxList->dot11DiversitySelectionRx[1].bDiversitySelectionRX = TRUE;

	mib_info->DelaySleepValue = 0;

	//NdisAllocateSpinLock(&Nic->ManagementInfo.PhyMIBConfigLock);
	mib_info->bDeviceConnected = FALSE;
	
	// initialization for rate negotiation
	mib_info->TotalRetryCount = 0;
	mib_info->PacketsSentForTxRateCheck = 0;
	mib_info->PrevTxDataRate = 0;
	mib_info->TxRateIncreaseWaitCount = 0;
	mib_info->TxRateIncreaseWaitRequired = 1;
	
	// initialization for g-mode protection
	mib_info->bEnableSendCTSToSelf = FALSE;

	mib_info->CurrentBSSType = dot11_BSS_type_infrastructure;

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
	wf_mib_info_t *mib_info = pAdapter->mib_info;

    attr->NumOfTXBuffers = mib_info->OperationModeCapability.uNumOfTXBuffers;
    attr->NumOfRXBuffers = mib_info->OperationModeCapability.uNumOfRXBuffers;
    attr->MultiDomainCapabilityImplemented = TRUE;
    attr->NumSupportedPhys = mib_info->pSupportedPhyTypes->uNumOfEntries;

//    MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle, 
//                       &attr->SupportedPhyAttributes,
//                       attr->NumSupportedPhys * sizeof(DOT11_PHY_ATTRIBUTES),
//                       HW11_MEMORY_TAG);

	attr->SupportedPhyAttributes = wf_malloc(attr->NumSupportedPhys * sizeof(DOT11_PHY_ATTRIBUTES));
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
	//DOT11_EXTSTA_CAPABILITY         ExtStaCap;
    NDIS_STATUS                     ndisStatus;
    DOT11_BSS_TYPE                  savedBssType;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

//    MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle, 
//                       &attr->ExtSTAAttributes,
//                       sizeof(DOT11_EXTSTA_ATTRIBUTES),
//                       'SltR');
	
	attr->ExtSTAAttributes = wf_malloc(sizeof(DOT11_EXTSTA_ATTRIBUTES));
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
    attr->ExtSTAAttributes->bSafeModeImplemented = FALSE;
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
                                         wf_get_unicast_alg_pair,
                                         &attr->ExtSTAAttributes->pInfraSupportedUcastAlgoPairs,
                                         &attr->ExtSTAAttributes->uInfraNumSupportedUcastAlgoPairs);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;

        //
        // Get multicast algorithm pair list for infrastructure
        //
        ndisStatus = StaGetAlgorithmPair(pAdapter,
                                         wf_get_multicast_alg_pair,
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
                                         wf_get_unicast_alg_pair,
                                         &attr->ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs,
                                         &attr->ExtSTAAttributes->uAdhocNumSupportedUcastAlgoPairs);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            __leave;

        //
        // Get multicast algorithm pair list for ad hoc
        //
        ndisStatus = StaGetAlgorithmPair(pAdapter,
                                         wf_get_multicast_alg_pair,
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


INLINE NDIS_STATUS
MpSet80211Attributes(
	__in  PADAPTER						  pAdapter
	)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES	attr = {0};

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
		// Or else the system will crash.	  2021/03/12
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
			wf_free(attr.SupportedPhyAttributes);
		}
		
		if (attr.ExtSTAAttributes)
		{
			if (attr.ExtSTAAttributes->pSupportedCountryOrRegionStrings)
			{
				wf_free(attr.ExtSTAAttributes->pSupportedCountryOrRegionStrings);
			}
			if (attr.ExtSTAAttributes->pInfraSupportedUcastAlgoPairs)
			{
				wf_free(attr.ExtSTAAttributes->pInfraSupportedUcastAlgoPairs);
			}

			if (attr.ExtSTAAttributes->pInfraSupportedMcastAlgoPairs)
			{
				wf_free(attr.ExtSTAAttributes->pInfraSupportedMcastAlgoPairs);
			}

			if (attr.ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs)
			{
				wf_free(attr.ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs);
			}

			if (attr.ExtSTAAttributes->pAdhocSupportedMcastAlgoPairs)
			{
				wf_free(attr.ExtSTAAttributes->pAdhocSupportedMcastAlgoPairs);
			}

			wf_free(attr.ExtSTAAttributes);

		}

	}

	return ndisStatus;

}


INLINE NDIS_STATUS
MpSetGeneralAttributes(
	__in  PADAPTER						  pAdapter
	)
{
	NDIS_MINIPORT_ADAPTER_ATTRIBUTES		MiniportAttributes;
	NDIS_PNP_CAPABILITIES					PnpCapabilities;
	
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
 * \param pAdapter			The adapter whose configuration will be read
 * \sa MpInitialize
 */

{
	NDIS_STATUS 					ndisStatus = NDIS_STATUS_SUCCESS;
	ULONG							i, ValueRead;
	PUCHAR							pucDestination;
	PMP_REG_ENTRY					pRegEntry;
	PNDIS_CONFIGURATION_PARAMETER	Parameter = NULL;
	BOOLEAN 						bRegistryOpened;
	NDIS_HANDLE 					RegistryConfigurationHandle;
	NDIS_CONFIGURATION_OBJECT		ConfigObject;

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
				*((PUCHAR) pucDestination)	= (UCHAR) ValueRead;
				break;
			case 2:
				*((PUSHORT) pucDestination) = (USHORT) ValueRead;
				break;
			case 4:
				*((PULONG) pucDestination)	= (ULONG) ValueRead;
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


INLINE VOID
MpResetPhyMIBs(
	__in  PADAPTER	  pAdapter
	)
{
	UNREFERENCED_PARAMETER(pAdapter);
	//Hw11ResetPhyMIBs(pAdapter->Nic);
}

INLINE VOID
MpResetMacMIBs(
	__in  PADAPTER	  pAdapter
	)
{

	ETH_COPY_NETWORK_ADDRESS(pAdapter->CurrentAddress, pAdapter->PermanentAddress);
	pAdapter->OperationMode 	= DOT11_OPERATION_MODE_EXTENSIBLE_STATION;
	pAdapter->State 			= INIT_STATE;
	pAdapter->Dot11RunningMode	= RUNNING_MODE_UNKNOWN;
	pAdapter->MaxRxLifeTime 	= 512;

	MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_NETWORK_MONITOR_MODE);

	// Choose the correct receive processing handler
	//MpAdjustReceiveHandler(pAdapter);

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




ULONG
FASTCALL
MpInterlockedSetClearBits (
	__inout PULONG Flags,
	__in ULONG sFlag,
	__in ULONG cFlag
	)
{

	ULONG NewFlags, OldFlags;

	OldFlags = * (volatile ULONG *) Flags;
	NewFlags = (OldFlags | sFlag) & ~cFlag;
	while (NewFlags != OldFlags) {
		NewFlags = InterlockedCompareExchange ((PLONG) Flags, (LONG) NewFlags, (LONG) OldFlags);
		if (NewFlags == OldFlags) {
			break;
		}

		OldFlags = NewFlags;
		NewFlags = (NewFlags | sFlag) & ~cFlag;
	}

	return OldFlags;
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
 * This function Deallocates an adapter object, returning all the resources back to
 * the system. Any allocations that are made for Adapter, must be deallocated from
 * here
 * 
 * \param pAdapter The adapter object to deallocate
 * \sa MpAllocateAdapter
 */
VOID
MpFreeAdapter(
	__in __drv_freesMem(Pool) PADAPTER					 pAdapter
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

	if (pAdapter->WdfDevice){
		WdfObjectDelete(pAdapter->WdfDevice);
	}

	wf_free(pAdapter);
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
	//PSTATION                pStation = NULL;
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
		
//		MP_ALLOCATE_MEMORY(MiniportAdapterHandle, &pAdapter, sizeof(ADAPTER), MP_MEMORY_TAG);
		pAdapter = wf_malloc(sizeof(ADAPTER));
		if (pAdapter == NULL)
		{
			LOG_E("MpAllocateAdapter: Failed to allocate %d bytes for ADAPTER", sizeof(ADAPTER));
			ndisStatus = NDIS_STATUS_RESOURCES;
			break;
		} else {
			NdisZeroMemory(pAdapter, sizeof(ADAPTER));
		}

		MP_CLEAR_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED);

		pAdapter->nic_info = wf_kzalloc(sizeof(nic_info_st));
		if (pAdapter->nic_info == NULL) {
			LOG_E("malloc nic info failed!\n");
			break;
		} else {
			NdisZeroMemory(pAdapter->nic_info, sizeof(nic_info_st));
		}
		
		pAdapter->MiniportAdapterHandle = MiniportAdapterHandle;
		//pAdapter->dev_state = WF_DEV_STATE_INIT;
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

//		ndisStatus = MpAllocateAdapterWorkItem(pAdapter);
//		if (ndisStatus != NDIS_STATUS_SUCCESS)
//		{
//			break;
//		}
	} while (FALSE);

    if (ndisStatus != NDIS_STATUS_SUCCESS) {
        if (pAdapter) {
            MpFreeAdapter(pAdapter);
            pAdapter = NULL;
        }

        *ppAdapter = NULL;
    } else {    
        *ppAdapter = pAdapter;
    }
    
    return ndisStatus;
}


INLINE 
BOOLEAN
MpRemoveAdapter(
	__in PADAPTER pAdapter
	)
{
	if (!(MpInterlockedSetClearBits(
			&pAdapter->Status,
			MP_ADAPTER_REMOVE_IN_PROGRESS,		// Set this bit
			MP_ADAPTER_RESET_IN_PROGRESS		// Clear this bit
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



NDIS_STATUS wf_mp_attributes_init(PADAPTER padapter)
{
	NDIS_MINIPORT_ADAPTER_ATTRIBUTES MiniportAttributes;
	NDIS_STATUS ndisStatus;
	nic_info_st *pNic_info = (nic_info_st*)padapter->nic_info;
	hw_info_st *phw_info = pNic_info->hw_info;

	NdisZeroMemory(&MiniportAttributes, sizeof(MiniportAttributes));
    MiniportAttributes.RegistrationAttributes.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES;
    MiniportAttributes.RegistrationAttributes.Header.Revision = NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES_REVISION_1;
    MiniportAttributes.RegistrationAttributes.Header.Size = sizeof(NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES);
    MiniportAttributes.RegistrationAttributes.MiniportAdapterContext = padapter;
    MiniportAttributes.RegistrationAttributes.AttributeFlags = NDIS_MINIPORT_ATTRIBUTES_NDIS_WDM ;
    MiniportAttributes.RegistrationAttributes.CheckForHangTimeInSeconds = 64;//HW11_CHECK_FOR_HANG_TIME_IN_SECONDS;
    MiniportAttributes.RegistrationAttributes.InterfaceType = HW11_BUS_INTERFACE_TYPE;

    NdisMSetMiniportAttributes(padapter->MiniportAdapterHandle, &MiniportAttributes);

	MpInitializeNicVariable(padapter);
	
    ETH_COPY_NETWORK_ADDRESS(padapter->PermanentAddress, (PDOT11_MAC_ADDRESS)phw_info->macAddr);
    MpResetMacMIBs(padapter);
    MpResetPhyMIBs(padapter);
	// TODO: Move this to reset workitem. 
	StaResetCipherAbilities(padapter);

    //MP_SET_FLAG(padapter, fMP_INIT_IN_PROGRESS);

    // Read Advanced configuration information from the registry
    MpReadRegistryConfiguration(padapter);
	
    // Fill the Miniport 802.11 Attributes, we can do so as soon as NIC is initialized.
    ndisStatus = MpSet80211Attributes(padapter);
    if (ndisStatus != NDIS_STATUS_SUCCESS) {
        LOG_E("Failed to set 80211 attributes");
        return ndisStatus;
    }

	ndisStatus = MpSetGeneralAttributes(padapter);
    if (ndisStatus != NDIS_STATUS_SUCCESS) {
        LOG_E("Failed to set general attributes");
        return ndisStatus;
    }

	return ndisStatus;
}


void wf_mp_dev_stop(PADAPTER      pAdapter)
{
	wf_usb_info_t *usb_info = pAdapter->usb_info;
	wf_xmit_info_t *xmit_info = pAdapter->xmit_info;
	wf_recv_info_t *recv_info = pAdapter->recv_info;
	wf_data_que_t 		*queue = NULL;
	wf_usb_req_t 		*usb_req = NULL;
	PLIST_ENTRY 		plist = NULL;
	wf_recv_pkt_t *pkt = NULL;
	int timeout;

	wf_usb_dev_stop(pAdapter);

	//complete the tx data
	queue = &usb_info->data_pend;
	while(!IsListEmpty(&queue->head)) {
		plist = wf_pkt_data_deque(queue, QUE_POS_HEAD);
		if(plist == NULL) {
			LOG_E("get usb req list failed");
			continue;
		}
		LOG_D("complete the xmit data to kernel");
		usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);
		InterlockedIncrement(&usb_info->proc_cnt);
		wf_usb_xmit_complet_callback(pAdapter->nic_info, usb_req);
	}

	queue = &usb_info->mgmt_pend;
	while(!IsListEmpty(&queue->head)) {
		plist = wf_pkt_data_deque(queue, QUE_POS_HEAD);
		if(plist == NULL) {
			LOG_E("get usb req list failed");
			continue;
		}
		LOG_D("complete the xmit mgmt to kernel");
		usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);
		InterlockedIncrement(&usb_info->proc_cnt);
		wf_usb_xmit_complet_callback(pAdapter->nic_info, usb_req);
	}

	//complete the rx data
	queue = &recv_info->mgmt_pend;
	while(!IsListEmpty(&queue->head)) {
		plist = wf_pkt_data_deque(queue, QUE_POS_HEAD);
		if(plist == NULL) {
			LOG_E("get usb req list failed");
			continue;
		}

		LOG_D("complete the recv mgmt to kernel");
		pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
		wf_recv_release_source(pAdapter, pkt);
	}

	queue = &recv_info->data_pend;
	while(!IsListEmpty(&queue->head)) {
		plist = wf_pkt_data_deque(queue, QUE_POS_HEAD);
		if(plist == NULL) {
			LOG_E("get usb req list failed");
			continue;
		}

		LOG_D("complete the recv data to kernel");
		pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
		wf_recv_release_source(pAdapter, pkt);
	}

	timeout=0;
	while(xmit_info->proc_cnt && timeout<10) {
		LOG_E("tx_pending=%d", xmit_info->proc_cnt);
		wf_msleep(1000);
		timeout++;
	}

	timeout=0;
	while(recv_info->proc_cnt && timeout<10) {
		LOG_E("rx_pending=%d", recv_info->proc_cnt);
		wf_msleep(1000);
		timeout++;
	}

	timeout=0;
	while(usb_info->proc_cnt && timeout<10) {
		LOG_E("usb_pending=%d", usb_info->proc_cnt);
		wf_msleep(1000);
		timeout++;
	}
}


INLINE VOID
MpOnSurpriseRemoval(
    __in  PADAPTER        pAdapter
    )
{
	PNDIS_OID_REQUEST   request;
	//wf_usb_info_t 		*usb_info = NULL;
	nic_info_st 		*nic_info = NULL;
	//wf_data_que_t 		*pdata_pend = NULL;
	//wf_usb_req_t 		*usb_req = NULL;
	//PLIST_ENTRY 		plist = NULL;

	if(pAdapter == NULL) {
		return;
	}

	if(pAdapter->usb_info == NULL || pAdapter->nic_info == NULL) {
		return;
	}

	LOG_D("start");

	MP_SET_STATUS_FLAG(pAdapter, MP_ADAPTER_SURPRISE_REMOVED);
	
	nic_info = pAdapter->nic_info;
	nic_info->is_surprise_removed = wf_true;

	if(!MP_TEST_STATUS_FLAG(pAdapter, MP_ADAPTER_EXCEPTION_HANDLE) && pAdapter->PendedRequest != NULL) {
		request = pAdapter->PendedRequest;
		if(request->RequestType == NdisRequestSetInformation &&
			request->DATA.SET_INFORMATION.Oid == OID_DOT11_CONNECT_REQUEST) {
			LOG_D("submit assoc failed!");
			wf_submit_assoc_complete(pAdapter, DOT11_ASSOC_STATUS_FAILURE);
			wf_submit_connect_complete(pAdapter, DOT11_CONNECTION_STATUS_FAILURE);
		}

		Mp11CompletePendedRequest(pAdapter, NDIS_STATUS_FAILURE);
	}

	wf_mp_dev_stop(pAdapter);
	#if 0
	usb_info = pAdapter->usb_info;
	pdata_pend = &usb_info->data_pend;
	while(!IsListEmpty(&pdata_pend->head)) {
		plist = wf_pkt_data_deque(pdata_pend, QUE_POS_HEAD);
		if(plist == NULL) {
			LOG_E("get usb req list failed");
			continue;
		}

		LOG_D("complete the xmit pkt to kernel");
		
		usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);
		InterlockedIncrement(&usb_info->proc_cnt);
		
		wf_usb_xmit_complet_callback(pAdapter->nic_info, usb_req);
	}
	#endif
}

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
    
	LOG_D("Driver Init!!!");
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
	MPChar.SendNetBufferListsHandler    = wf_xmit_ndis_pkt;//MPSendNetBufferLists;
    MPChar.CancelSendHandler            = wf_xmit_cancel_send;
    MPChar.ReturnNetBufferListsHandler  = wf_recv_release_nbl;
    
    //
    // Fault handling handlers
    //
    MPChar.CheckForHangHandlerEx        = MPCheckForHang;
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
NDIS_STATUS
MPInitialize(
    __in  NDIS_HANDLE                        MiniportAdapterHandle,
    __in  NDIS_HANDLE                        MiniportDriverContext,
    __in  PNDIS_MINIPORT_INIT_PARAMETERS     MiniportInitParameters
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PADAPTER            pAdapter = NULL;
    //NDIS_MINIPORT_ADAPTER_ATTRIBUTES    MiniportAttributes;                             
    //BOOLEAN                        bHw11Started;
    //BOOLEAN                        bSta11Started;
    //BOOLEAN                        bHw11Initialized;
    //ULONG                           ErrorValue = 0;
    //BOOLEAN                            bSendEngineInitialized = FALSE, bReceiveEngineInitialized = FALSE;
    //BOOLEAN                             bCustomInterfacesInitialized = FALSE;

    UNREFERENCED_PARAMETER(MiniportDriverContext);
    UNREFERENCED_PARAMETER(MiniportInitParameters);

    PAGED_CODE();

    LOG_D("---> MPInitialize");
    LOG_D("MiniportAdapterHandle: %p", MiniportAdapterHandle);
        
    do {
        //
        // Allocate adapter context structure and initialize all the
        // memory resources for sending and receiving packets.
        //
        ndisStatus = MpAllocateAdapter(MiniportAdapterHandle, &pAdapter);
        if(ndisStatus != NDIS_STATUS_SUCCESS) {
            break;
        }

		pAdapter->dev_state = WF_DEV_STATE_INIT;

		ndisStatus = wf_usb_dev_init(pAdapter);
		if(ndisStatus != NDIS_STATUS_SUCCESS) {
            break;
        }

		ndisStatus = wf_nic_dev_init(pAdapter);
		if(ndisStatus != NDIS_STATUS_SUCCESS) {
            break;
        }

		ndisStatus = wf_mp_oids_init(pAdapter);
		if(ndisStatus != NDIS_STATUS_SUCCESS) {
            break;
        }
		
		ndisStatus = wf_mp_attributes_init(pAdapter);
		if(ndisStatus != NDIS_STATUS_SUCCESS) {
            break;
        }

		ndisStatus = wf_dbg_init(pAdapter);
		if(ndisStatus != NDIS_STATUS_SUCCESS) {
            break;
        }

        ndisStatus = NICRegisterDevice(NdisMiniportDriverHandle, pAdapter);
        if (!NT_SUCCESS(ndisStatus)) {
            LOG_E("NdisProtCreateControlDevice failed with status 0x%x", ndisStatus);
            break;
        }

        // The miniport will start in the Paused PnP state        
        // MpRestart is called after MpInitialize so the state wil change to MINIPORT_RUNNING there
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
        
		ndisStatus = wf_usb_dev_start(pAdapter);
        if(ndisStatus != NDIS_STATUS_SUCCESS) {
            LOG_E("USB device start failed, status: 0x%x", ndisStatus);
            break;
        }

		pAdapter->dev_state = WF_DEV_STATE_RUN;
    } WHILE (FALSE);

    if(pAdapter && ndisStatus != NDIS_STATUS_SUCCESS) {
        LOG_D("MPInitialize failed! Error code: 0x%08x", ndisStatus);
        if (pAdapter) {
            MpFreeAdapter(pAdapter);
        }
		//NdisWriteErrorLogEntry can add a error log
    }

    LOG_D("<--- ndisStatus = 0x%08x%", ndisStatus);

    return ndisStatus;

}


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

	MiniportAdapterContext	Pointer to our adapter
	HaltAction				The reason adapter is being halted

Return Value:

	None.

--*/
VOID
MPHalt(
    __in  NDIS_HANDLE             MiniportAdapterContext,
    __in  NDIS_HALT_ACTION        HaltAction
    )

{
    
    PADAPTER          pAdapter = (PADAPTER) MiniportAdapterContext;
	nic_info_st *nic_info;
    //ULONG           CurrentStatus;

//    UNREFERENCED_PARAMETER(HaltAction);
//    UNREFERENCED_PARAMETER(MiniportAdapterContext);
	pAdapter->dev_state = WF_DEV_STATE_DEINIT;
	
    LOG_D("---> MPHalt");
    LOG_D("Calling MpHalt with action 0x%x",HaltAction);
    PAGED_CODE();

	nic_info = pAdapter->nic_info;

	nic_info->is_surprise_removed = wf_true;
	wf_mp_dev_stop(pAdapter);
	
	NICDeregisterDevice(pAdapter);

	wf_dbg_deinit(pAdapter);
	
	wf_nic_dev_deinit(pAdapter);

	wf_usb_dev_deinit(pAdapter);

	wf_mp_oids_deinit(pAdapter);

	pAdapter->dev_state = WF_DEV_STATE_STOP;

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
	BOOLEAN			bHanged;
	bHanged = TRUE;
	if(pAdapter->dev_state == WF_DEV_STATE_RUN) bHanged = FALSE;
    return bHanged;
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

    wf_usb_dev_stop(pAdapter);
    
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
    NDIS_STATUS      ndisStatus = NDIS_STATUS_SUCCESS;
	wf_mib_info_t *mib_info;
	nic_info_st *nic_info;
	LARGE_INTEGER timeout = { 0 };

	timeout.QuadPart = DELAY_ONE_MILLISECOND;
	timeout.QuadPart *= 2000;
    //ULONG           CurrentStatus, uReceiveCount;

    UNREFERENCED_PARAMETER(MiniportPauseParameters);
    UNREFERENCED_PARAMETER(pAdapter);

    LOG_D("---> MPPause");

    MP_VERIFY_PASSIVE_IRQL();
    
    //MPASSERT(MP_GET_NDIS_PNP_STATE(pAdapter) == MINIPORT_RUNNING);

    MP_ACQUIRE_RESET_PNP_LOCK(pAdapter);

	nic_info = pAdapter->nic_info;
	mib_info = pAdapter->mib_info;
	if(mib_info != NULL && nic_info != NULL) {
		if(mib_info->connect_state == TRUE) {
			//wf_usb_dev_start(pAdapter);
			KeClearEvent(&mib_info->halt_deauth_finish);
			if(!wf_mlme_deauth(nic_info, wf_true)) {
				//return ndisStatus;
			}
			LOG_D("start deauth from ap");

			if(KeWaitForSingleObject(&mib_info->halt_deauth_finish, Executive, KernelMode, TRUE, &timeout) != STATUS_SUCCESS) {
				LOG_E("wait scan hidden network timeout!");
				//return NDIS_STATUS_FAILURE;
			}
			//wf_usb_dev_stop(pAdapter);
		}
	}

	wf_mp_dev_stop(pAdapter);

	MP_RELEASE_RESET_PNP_LOCK(pAdapter);  
#if 0    
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
#endif
    LOG_D("<--- MPPause");
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
		
		ndisStatus = wf_usb_dev_start(pAdapter);
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
    removal, NDIS calls the miniport�s MiniportHalt function.
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
        ndisStatus = NDIS_STATUS_HARD_ERRORS;
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
	int ret = 0;
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
        
        wf_usb_dev_stop(pAdapter);

#ifdef CONFIG_RICHV200
		ret += hw_pwr_off_v200(pAdapter);
#else

#endif
		if(ret != 0) {
			LOG_E("dev deinit failed!!!!");
		}
		
		wf_msleep(100);
		
#ifdef CONFIG_RICHV200
		ret += hw_pwr_on_v200(pAdapter);
#else
		
#endif
		if(ret != 0) {
			LOG_E("dev deinit failed!!!!");
		}
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
    //PVOID                        InformationBuffer = NetDevicePnPEvent->InformationBuffer;
    //ULONG                        InformationBufferLength = NetDevicePnPEvent->InformationBufferLength;

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

	UNREFERENCED_PARAMETER(pAdapter);
	UNREFERENCED_PARAMETER(ResetType);

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
    wf_usb_dev_stop(pAdapter);

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




