/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    usb_recv.c

Abstract:
    Bus dependant layer USB specific function
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    01-01-2006    Created
Notes:

--*/

#include "wf_os_api.h"
#include "wf_debug.h"
//#include "pcomp.h"

#include "pcomp.h"

#include "wf_oids_adapt.h"


extern void print_buffer(PUCHAR title, LONG idx, PUCHAR src, LONG length);

// It is possible to change this table.
ULONG DSSS_Freq_Channel[] = {
	0,
	2412,
	2417,
	2422,
	2427,
	2432,
	2437,
	2442,
	2447,
	2452,
	2457,
	2462,
	2467,
	2472,
	2484
};

static unsigned char WPA_OUI[4] = { 0x00, 0x50, 0xf2, 0x01 };
static unsigned char WMM_OUI[4] = { 0x00, 0x50, 0xf2, 0x02 };


NDIS_STATUS wf_set_start_scan(void *nic_info)
{
	nic_info_st *pnic_info = nic_info;
	wf_bool is_connected, is_busy;
	int ret;
#ifdef CONFIG_CONCURRENT_MODE
	nic_info_st *pvir_nic = pnic_info->vir_nic;
	mlme_state_e state;
#endif

	pnic_info->is_up = 1;

#ifdef CONFIG_CONCURRENT_MODE
	wf_mlme_get_state(pvir_nic, &state);
	if(state <= MLME_STATE_ASSOC) {
		LOG_D("another nic is scanning");
		wf_os_api_ind_scan_done(pnic_info, wf_true);
		return 0;
	}
#endif
    
	wf_mlme_get_connect(pnic_info, &is_connected);
	if (is_connected) {
        wf_mlme_get_traffic_busy(pnic_info, &is_busy);
        if (is_busy) {
            wf_os_api_ind_scan_done(pnic_info, wf_true, 0);

            return 0;
        }
        ret = wf_mlme_scan_start(pnic_info, SCAN_TYPE_PASSIVE,
                           NULL, 0, NULL, 0, WF_MLME_FRAMEWORK_NDIS);
    } else {
        ret = wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                           NULL, 0, NULL, 0, WF_MLME_FRAMEWORK_NDIS);
    }

	if(ret != 0) {
		LOG_D("[%s-%d] scan start failed! ret=%d\n", __FUNCTION__, __LINE__, ret);
		ret = NDIS_STATUS_FAILURE;
	} else {
		ret = NDIS_STATUS_SUCCESS;
	}

	return ret;

}


void wf_bss_info_fill(BSS_FILTER_INFO *list, wf_80211_bssid_t bssid, wf_wlan_ssid_t *ssid)
{
	if (list->cnt >= DEFAULT_MAX_BSS_NUM) {
		return;
	}

	wf_memcpy(list->bssid[list->cnt], bssid, sizeof(wf_80211_bssid_t));
	wf_memcpy(&list->ssid[list->cnt], ssid, sizeof(wf_wlan_ssid_t));
	list->cnt++;
}

NDIS_STATUS wf_bss_info_search(BSS_FILTER_INFO *list, wf_80211_bssid_t bssid, wf_wlan_ssid_t *ssid)
{
	int i;

	for (i = 0; i < list->cnt; i++) {
		if (memcmp(list->bssid[list->cnt], bssid, sizeof(wf_80211_bssid_t)) == 0 ||
			memcmp(&list->ssid[list->cnt], ssid, sizeof(wf_wlan_ssid_t)) == 0) {
			return NDIS_STATUS_SUCCESS;
		}
	}

	return NDIS_STATUS_FAILURE;
}

void wf_update_bss_list(void  *      adapter)
{
	PADAPTER padapter = adapter;
	nic_info_st *pnic_info = (nic_info_st *)padapter->nic_info;
	int scanned_ret;
	wf_wlan_scanned_info_t *pscanned_info;
	//wf_bss_info_t *bss_node;
	wf_mib_info_t 		*mib = &padapter->MibInfo;
	BSS_FILTER_INFO filter_list = {0};

	WdfSpinLockAcquire(mib->bss_lock);
	do {
		mib->bss_cnt = 0;
		wf_wlan_scanned_each_begin(pscanned_info, pnic_info) {
//			if (wf_bss_info_search(&filter_list, pscanned_info->bssid, &pscanned_info->ssid) == NDIS_STATUS_SUCCESS) {
//				continue;
//			}
//			else {
//				wf_bss_info_fill(&filter_list, pscanned_info->bssid, &pscanned_info->ssid);
//			}
			mib->bss_node[mib->bss_cnt++] = pscanned_info;
		}
		wf_wlan_scanned_each_end(pnic_info, &scanned_ret);
	}while(scanned_ret);

	//LOG_D("scan_ret=%d, bss_cnt=%d", scanned_ret, mib->bss_cnt);
	WdfSpinLockRelease(mib->bss_lock);
}

NDIS_STATUS wf_enum_bss_list(PADAPTER       pAdapter, PDOT11_BYTE_ARRAY pDot11ByteArray, ULONG TotalLength)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	wf_wlan_scanned_info_t *pscanned_info;
	PUCHAR              pCurrPtr = pDot11ByteArray->ucBuffer;
	ULONG               RemainingBytes = 0;
	ULONG               BSSEntrySize = 0;
	PDOT11_BSS_ENTRY    pDot11BSSEntry = NULL;
    wf_no_ie_info_t *no_ie_info;
	ULONG ie_len_local, idx;
	ULONGLONG			ullHostTimeStamp;
	wf_mib_info_t 		*mib = &pAdapter->MibInfo;

	NdisGetCurrentSystemTime((PLARGE_INTEGER)&ullHostTimeStamp);
	pDot11ByteArray->uNumOfBytes = 0;
	pDot11ByteArray->uTotalNumOfBytes = 0;
	RemainingBytes = TotalLength - FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer);

	WdfSpinLockAcquire(mib->bss_lock);
	for(idx=0; idx<mib->bss_cnt; idx++) {
		pscanned_info = mib->bss_node[idx];
		ie_len_local = pscanned_info->ie_len - 12;
		BSSEntrySize = FIELD_OFFSET(DOT11_BSS_ENTRY, ucBuffer) + ie_len_local;
		pDot11ByteArray->uTotalNumOfBytes += BSSEntrySize;
		pDot11BSSEntry = (PDOT11_BSS_ENTRY)pCurrPtr;

		if (RemainingBytes >= BSSEntrySize) {
			pDot11BSSEntry->uPhyId = pAdapter->MibInfo.SelectedPhyId;
			pDot11BSSEntry->ullHostTimestamp = ullHostTimeStamp;
			pDot11BSSEntry->dot11BSSType = dot11_BSS_type_infrastructure;
			pDot11BSSEntry->lRSSI = pscanned_info->signal_strength_scale - 100;
			pDot11BSSEntry->uLinkQuality = pscanned_info->signal_qual;
			if (pscanned_info->channel >= 15) {
				// 11a channel
				pDot11BSSEntry->PhySpecificInfo.uChCenterFrequency = 5000 + 5 * pscanned_info->channel;
			}
			else {
				pDot11BSSEntry->PhySpecificInfo.uChCenterFrequency = DSSS_Freq_Channel[pscanned_info->channel];
			}

			// Assume that country string we received must exist and be correct. 
			// TODO: set the following properly.
			pDot11BSSEntry->bInRegDomain = TRUE;

			NdisMoveMemory(pDot11BSSEntry->dot11BSSID, pscanned_info->bssid, sizeof(DOT11_MAC_ADDRESS));

			// Copy the IEs
			no_ie_info = pscanned_info->ies;
			pDot11BSSEntry->usBeaconPeriod = no_ie_info->intv;// pStaBSSEntry->BeaconInterval;
			pDot11BSSEntry->ullTimestamp = no_ie_info->timestamp;// pStaBSSEntry->BeaconTimestamp;
			pDot11BSSEntry->usCapabilityInformation = no_ie_info->capab;

			NdisMoveMemory(pDot11BSSEntry->ucBuffer, pscanned_info->ies + 12, ie_len_local);

			//wf_parser_ie_info(pscanned_info->ies, pscanned_info->ie_len);

			pDot11BSSEntry->uBufferLength = ie_len_local;
			pDot11ByteArray->uNumOfBytes += BSSEntrySize;
			pCurrPtr += BSSEntrySize;
			RemainingBytes -= BSSEntrySize;
		} else {
			ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
			RemainingBytes = 0;
		}
	}
	WdfSpinLockRelease(mib->bss_lock);
	//LOG_D("enum_bss_cnt=%d", mib->bss_cnt);

	return ndisStatus;
}


VOID wf_fill_indicate_info(PADAPTER padapter, NDIS_STATUS StatusCode, PVOID RequestID, PVOID pStatusBuffer, ULONG StatusBufferSize)
{
	NDIS_STATUS_INDICATION	indicate_info;
	
	NdisZeroMemory(&indicate_info, sizeof(NDIS_STATUS_INDICATION));
	
	// Fill in object header
	indicate_info.Header.Type = NDIS_OBJECT_TYPE_STATUS_INDICATION;
	indicate_info.Header.Revision = NDIS_STATUS_INDICATION_REVISION_1;
	indicate_info.Header.Size = sizeof(NDIS_STATUS_INDICATION);

	// Fill in the rest of the field
	indicate_info.StatusCode = StatusCode;
	indicate_info.SourceHandle = padapter->MiniportAdapterHandle;
	indicate_info.DestinationHandle = NULL;
	indicate_info.RequestId = RequestID;
	
	indicate_info.StatusBuffer = pStatusBuffer;
	indicate_info.StatusBufferSize = StatusBufferSize;

	DbgPrint("############################dot11 submit status = %x\n", StatusCode);
	// Indicate the status to NDIS
	NdisMIndicateStatusEx(padapter->MiniportAdapterHandle, &indicate_info);
}

NDIS_STATUS wf_submit_scan_complete(PADAPTER pAdapter)
{
	NDIS_STATUS ndis_status = NDIS_STATUS_SUCCESS;
	wf_update_bss_list(pAdapter);
	if(pAdapter->bRequestedScan == TRUE)
	{
		wf_fill_indicate_info(pAdapter, NDIS_STATUS_DOT11_SCAN_CONFIRM,
    	pAdapter->CurrentRequestID_Scan, &ndis_status, sizeof(NDIS_STATUS));
		pAdapter->bRequestedScan = FALSE;
		pAdapter->CurrentRequestID_Scan = NULL;
		LOG_D("Indicate NDIS scan complete\n");
	}
	else
	{
		LOG_D("Periodic scan complete\n");
	}
	//Mp11CompletePendedRequest(pAdapter, NDIS_STATUS_SUCCESS);
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_submit_connect_start(PADAPTER pAdapter, void *pscan_info)
{
	wf_wlan_scanned_info_t *scan_info = pscan_info;
	DOT11_CONNECTION_START_PARAMETERS   parameter;

    NdisZeroMemory(&parameter, sizeof(DOT11_CONNECTION_START_PARAMETERS));

	//fill connection parameters
    MP_ASSIGN_NDIS_OBJECT_HEADER(parameter.Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_CONNECTION_START_PARAMETERS_REVISION_1,
                                 sizeof(DOT11_CONNECTION_START_PARAMETERS));
    parameter.BSSType = pAdapter->MibInfo.BSSType;

	wf_fill_indicate_info(pAdapter, NDIS_STATUS_DOT11_CONNECTION_START,
    	NULL, &parameter, sizeof(DOT11_CONNECTION_START_PARAMETERS));
	
	LOG_D("indicate NDIS start connection");
	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS wf_submit_connect_complete(PADAPTER pAdapter, ULONG status)
{
	DOT11_CONNECTION_COMPLETION_PARAMETERS   parameter;

	NdisZeroMemory(&parameter, sizeof(DOT11_CONNECTION_COMPLETION_PARAMETERS));

	//fill connection complete parameters
	MP_ASSIGN_NDIS_OBJECT_HEADER(parameter.Header,
		NDIS_OBJECT_TYPE_DEFAULT,
		DOT11_CONNECTION_COMPLETION_PARAMETERS_REVISION_1,
		sizeof(DOT11_CONNECTION_COMPLETION_PARAMETERS));
	parameter.uStatus = status;//DOT11_CONNECTION_STATUS_SUCCESS;

	//
	// Indicate the status to NDIS
	//
	wf_fill_indicate_info(pAdapter, NDIS_STATUS_DOT11_CONNECTION_COMPLETION,
    	pAdapter->PendedRequest->RequestId, &parameter, sizeof(DOT11_CONNECTION_COMPLETION_PARAMETERS));
	LOG_D("indicate NDIS assoc complete");
	
	//Mp11CompletePendedRequest(pAdapter, NDIS_STATUS_SUCCESS);
	return NDIS_STATUS_SUCCESS;
}



NDIS_STATUS wf_submit_assoc_start(PADAPTER pAdapter,void *pscan_info)
{
	DOT11_ASSOCIATION_START_PARAMETERS  start_assoc;
	wf_wlan_scanned_info_t *scan_info = pscan_info;

	NdisZeroMemory(&start_assoc, sizeof(DOT11_ASSOCIATION_START_PARAMETERS));
	
	MP_ASSIGN_NDIS_OBJECT_HEADER(start_assoc.Header,
		NDIS_OBJECT_TYPE_DEFAULT,
		DOT11_ASSOCIATION_START_PARAMETERS_REVISION_1,
		sizeof(DOT11_ASSOCIATION_START_PARAMETERS));
	start_assoc.uIHVDataOffset = 0;
	start_assoc.uIHVDataSize = 0;
	
	start_assoc.SSID.uSSIDLength = scan_info->ssid.length;
	NdisMoveMemory(start_assoc.SSID.ucSSID, scan_info->ssid.data, scan_info->ssid.length);
	NdisMoveMemory(start_assoc.MacAddr, scan_info->bssid, sizeof(wf_80211_bssid_t));
	
	wf_fill_indicate_info(pAdapter, NDIS_STATUS_DOT11_ASSOCIATION_START,
    	NULL, &start_assoc, sizeof(DOT11_ASSOCIATION_START_PARAMETERS));

	LOG_D("indicate NDIS start assoc");
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_submit_assoc_complete(PADAPTER padapter, ULONG status)
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PUCHAR              pStatusBuffer = NULL;
    ULONG               BufferLength = 0;
    BOOLEAN             bUseBackupBuffer = FALSE;
    PDOT11_ASSOCIATION_COMPLETION_PARAMETERS    parameter = NULL;
    PUCHAR              pTempPtr = NULL;
    ULONG               CurrentOffset = sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS);
	wf_ap_info_t *ap_info = padapter->ap_info;

	LOG_D("indicate assoc complete");

	if(ap_info == NULL || ap_info->scan_info == NULL) {
		LOG_E("parameter is NULL!\n");
		return NDIS_STATUS_FAILURE;
	}
    nic_info_st *pnic_info = padapter->nic_info;

    //wf_memcpy(padapter->MibInfo.BssId,  wf_wlan_get_cur_bssid(pnic_info), sizeof(DOT11_MAC_ADDRESS));

    // Determine length of buffer to use for filling completion parameters.
    BufferLength = sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS) + sizeof(ULONG);

	BufferLength += ap_info->bcn_len;
	BufferLength += ap_info->assoc_req_len;
	BufferLength += ap_info->assoc_resp_len;
	
	// Theoretically, this length is exactly we need. However memory corruption is sometimes detected.
	// In case, increase the length.
	//	BufferLength -= 3 * DOT11_MGMT_HEADER_SIZE;

	DbgPrint("bcn_len=%d, assoc_req=%d, assoc_resp=%d\n", 
		ap_info->bcn_len, ap_info->assoc_req_len, ap_info->assoc_resp_len);

    MP_ALLOCATE_MEMORY(padapter->MiniportAdapterHandle, &pStatusBuffer, BufferLength, 'SltR');
    if (pStatusBuffer == NULL) {
		LOG_E("malloc assoc parameter failed!\n");
		return NDIS_STATUS_FAILURE;
    }

    NdisZeroMemory(pStatusBuffer, BufferLength);

    parameter = (PDOT11_ASSOCIATION_COMPLETION_PARAMETERS)pStatusBuffer;
    MP_ASSIGN_NDIS_OBJECT_HEADER(parameter->Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_ASSOCIATION_COMPLETION_PARAMETERS_REVISION_1,
                                 sizeof(DOT11_ASSOCIATION_COMPLETION_PARAMETERS));
    NdisMoveMemory(parameter->MacAddr, ap_info->scan_info->bssid, sizeof(wf_80211_bssid_t));

    parameter->uStatus = status;
    if (parameter->uStatus == DOT11_ASSOC_STATUS_SUCCESS) {
        //
        // Fill success state parameters
        //
        parameter->AuthAlgo = padapter->MibInfo.AuthAlgorithm;     // ENCRYPT:
        parameter->UnicastCipher = padapter->MibInfo.UnicastCipherAlgorithm;
        parameter->MulticastCipher = padapter->MibInfo.MulticastCipherAlgorithm;
        parameter->bFourAddressSupported = FALSE;       // No support for 4 address detection
        parameter->bPortAuthorized = FALSE;
        parameter->DSInfo = DOT11_DS_UNKNOWN;
        parameter->uEncapTableOffset = 0;
        parameter->uEncapTableSize = 0;

        parameter->bReAssocReq = FALSE;                             // ROAM:
        parameter->bReAssocResp = FALSE;
    }

	parameter->uActivePhyListOffset = CurrentOffset;
    parameter->uActivePhyListSize = sizeof(ULONG);
    pTempPtr = pStatusBuffer + CurrentOffset;
    *((ULONG UNALIGNED *)pTempPtr) = padapter->MibInfo.SelectedPhyId;
    CurrentOffset += sizeof(ULONG);

	//we must fill bcn info
	if(ap_info->bcn_len != 0) {
		parameter->uBeaconOffset = CurrentOffset;
	    parameter->uBeaconSize = ap_info->bcn_len - DOT11_MGMT_HEADER_SIZE;
	    pTempPtr = pStatusBuffer + CurrentOffset;
	    NdisMoveMemory(pTempPtr, ap_info->beacon + DOT11_MGMT_HEADER_SIZE,
			parameter->uBeaconSize);
	    CurrentOffset += parameter->uBeaconSize;
		//print_buffer("beacon", 0, ap_info->beacon, ap_info->bcn_len);
	}

    if (ap_info->assoc_req_len != 0) {
	    parameter->uAssocReqOffset = CurrentOffset;
	    parameter->uAssocReqSize = ap_info->assoc_req_len - DOT11_MGMT_HEADER_SIZE;
	    pTempPtr = pStatusBuffer + CurrentOffset;
	    NdisMoveMemory(pTempPtr, ap_info->assoc_req + DOT11_MGMT_HEADER_SIZE, 
	    	parameter->uAssocReqSize);
	    CurrentOffset += parameter->uAssocReqSize;
		//print_buffer("assoc_req", 0, ap_info->assoc_req, ap_info->assoc_req_len);
    }

	if (ap_info->assoc_resp_len != 0) {
        parameter->uAssocRespOffset = CurrentOffset;
        parameter->uAssocRespSize = ap_info->assoc_resp_len - DOT11_MGMT_HEADER_SIZE;
        pTempPtr = pStatusBuffer + CurrentOffset;
        NdisMoveMemory(pTempPtr, ap_info->assoc_resp + DOT11_MGMT_HEADER_SIZE, 
        	parameter->uAssocRespSize);
        CurrentOffset += parameter->uAssocRespSize;
		//print_buffer("assoc_resp", 0, ap_info->assoc_resp, ap_info->assoc_resp_len);
    }
	ap_info->assoc_req_len = 0;
	ap_info->assoc_resp_len = 0;
	
    // Now indicate the status indicaiton
    wf_fill_indicate_info(padapter, NDIS_STATUS_DOT11_ASSOCIATION_COMPLETION,
    	NULL, pStatusBuffer, CurrentOffset);

    // Free the status buffer if we allocated it
    if (bUseBackupBuffer == FALSE) {
        MP_FREE_MEMORY(pStatusBuffer);
        pStatusBuffer = NULL;
    }

    return ndisStatus;
}

VOID wf_submit_disassoc_complete(PADAPTER pAdapter, ULONG Reason)
{
	nic_info_st *nic_info = pAdapter->nic_info;
	DOT11_DISASSOCIATION_PARAMETERS disassocParameters;
	wf_ap_info_t *ap_info = pAdapter->ap_info;

	if(ap_info == NULL || ap_info->scan_info == NULL) {
		LOG_D("AP info is NULL");
		return;
	}
	
	NdisZeroMemory(&disassocParameters, sizeof(DOT11_DISASSOCIATION_PARAMETERS));
	MP_ASSIGN_NDIS_OBJECT_HEADER(disassocParameters.Header,
		NDIS_OBJECT_TYPE_DEFAULT,
		DOT11_DISASSOCIATION_PARAMETERS_REVISION_1,
		sizeof(DOT11_DISASSOCIATION_PARAMETERS));
	disassocParameters.uIHVDataOffset = 0;
	disassocParameters.uIHVDataSize = 0;
	// Copy the MAC address from the AP entry
	//NdisMoveMemory(&(disassocParameters.MacAddr), pAdapter->MibInfo.BssId, sizeof(DOT11_MAC_ADDRESS));
	NdisMoveMemory(&(disassocParameters.MacAddr), ap_info->scan_info->bssid, sizeof(DOT11_MAC_ADDRESS));

	disassocParameters.uReason = Reason;

	//clear AP info
	ap_info->scan_info = NULL;
	ap_info->bcn_len = 0;
	ap_info->assoc_req_len = 0;
	ap_info->assoc_resp_len = 0;
	NdisZeroMemory(&pAdapter->MibInfo.curr_ssid, sizeof(DOT11_SSID_LIST));

	wf_fill_indicate_info(
		pAdapter,
		NDIS_STATUS_DOT11_DISASSOCIATION,
		NULL,
		&disassocParameters,
		sizeof(DOT11_DISASSOCIATION_PARAMETERS)
	);
}


NDIS_STATUS wf_submit_link_qual(PADAPTER pAdapter)
{
	wf_ap_info_t *ap_info = pAdapter->ap_info;
	wf_wlan_scanned_info_t *scan_info = ap_info->scan_info;
	UCHAR                           buffer[sizeof(DOT11_LINK_QUALITY_PARAMETERS) + sizeof(DOT11_LINK_QUALITY_ENTRY)];
    ULONG                           bufferLength = sizeof(buffer);
    DOT11_LINK_QUALITY_PARAMETERS*  pLinkQualityParams = (DOT11_LINK_QUALITY_PARAMETERS*)&buffer[0];
    DOT11_LINK_QUALITY_ENTRY*       pEntry = (DOT11_LINK_QUALITY_ENTRY*)&buffer[sizeof(DOT11_LINK_QUALITY_PARAMETERS)];

    // initialize indication buffer
    NdisZeroMemory(&buffer[0], bufferLength);

    MP_ASSIGN_NDIS_OBJECT_HEADER(pLinkQualityParams->Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_LINK_QUALITY_PARAMETERS_REVISION_1,
                                 sizeof(DOT11_LINK_QUALITY_PARAMETERS));

    pLinkQualityParams->uLinkQualityListSize = 1;
    pLinkQualityParams->uLinkQualityListOffset = sizeof(DOT11_LINK_QUALITY_PARAMETERS);

    // previous NdisZeroMemory already set pEntry->PeerMacAddr to all 0x00, which
    // means the link quality is for current network
    pEntry->ucLinkQuality = scan_info->signal_qual;
	
	wf_fill_indicate_info(pAdapter, NDIS_STATUS_DOT11_LINK_QUALITY,
    	NULL, &buffer[0], bufferLength);

	LOG_D("indicate NDIS link qual\n");
	return NDIS_STATUS_SUCCESS;
}


wf_wlan_scanned_info_t *wf_find_scan_info_by_ssid(PADAPTER adapter, PDOT11_SSID_LIST pDot11SSIDList)
{

    nic_info_st *pnic_info = adapter->nic_info;
    wf_wlan_scanned_info_t *pscanned_info;
	wf_wlan_scanned_info_t *srch_end = NULL;

    wf_u32 res = 0;
    int scanned_ret;
    wf_u16 apCount = 0;

    /* Check if there is space for one more entry */
    wf_wlan_scanned_each_begin(pscanned_info, pnic_info)
    {
        if(memcmp(pscanned_info->ssid.data, pDot11SSIDList->SSIDs[0].ucSSID, pDot11SSIDList->SSIDs[0].uSSIDLength) == 0) {
			srch_end = pscanned_info;
			break;
		}
    }
    wf_wlan_scanned_each_end(pnic_info, &scanned_ret);

    return srch_end;
}


NDIS_STATUS wf_set_start_assoc (PADAPTER adapter, PDOT11_SSID_LIST pDot11SSIDList)
{
    
    nic_info_st *pnic_info = adapter->nic_info;
    sec_info_st *psec_info = pnic_info->sec_info;
	wf_wlan_scanned_info_t *scan_info;
	wf_ap_info_t *ap_info;
	wf_u8 *mac_addr;
	char ssid[64] = {0};
    wf_bool bconnect = wf_false;
	wf_oids_timer_ctx_t *timer_ctx;
#if 0
	ap_info = adapter->ap_info;
	if(ap_info == NULL) {
		LOG_E("error!!! ap info is NULL!!!\n");
		return;
	}

	LOG_D("[%s-%d] start\n", __FUNCTION__, __LINE__);

	scan_info = wf_find_scan_info_by_ssid(adapter, pDot11SSIDList);
	if(scan_info == NULL) {
		LOG_E("wf_find_scan_info_by_ssid fail\n");
 		return NDIS_STATUS_FAILURE; 
	}

	ap_info->scan_info = scan_info;

	wf_submit_connect_start(adapter, scan_info);

	mac_addr = scan_info->bssid;
	memcpy(ssid, scan_info->ssid.data, scan_info->ssid.length);
	LOG_D("ssid=%s\n", ssid);
	//LOG_D("bssid=%02x:%02x:%02x:%02x:%02x:%02x\n", mac_addr[0], mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);

    if (is_bcast_addr(mac_addr) ||
             is_zero_addr(mac_addr))
    {
        DbgPrint("clear current connection");

        wf_memset(psec_info, 0x0, sizeof(sec_info_st));
        wf_mlme_get_connect(pnic_info, &bconnect);
        if (bconnect)
        {
            wf_deauth_xmit_frame(pnic_info, wf_wlan_get_cur_bssid(pnic_info),
                              WF_80211_REASON_DEAUTH_LEAVING);
            wf_mlme_set_connect(pnic_info, wf_false);
            wf_os_api_ind_disconnect(pnic_info);
        }

        return NDIS_STATUS_FAILURE;
    }

    wf_memcpy(adapter->MibInfo.DesiredBSSID, mac_addr, sizeof(wf_80211_addr_t));
    wf_check_PMKID_candidate(adapter);
    wf_wlan_set_cur_bssid(pnic_info, mac_addr);
	wf_wlan_set_cur_ssid(pnic_info, &scan_info->ssid);

	//indicate
	wf_submit_assoc_start(adapter, scan_info);

    // probe and connect
    //LOG_D("start connect to: "WF_MAC_FMT, WF_MAC_ARG(mac_addr));
	LOG_D("curr channel=%d", scan_info->channel);
    //wf_scan_stop(pnic_info);
	wf_scan_wait_done(pnic_info, wf_true, 1000);

	wf_hw_info_set_channnel_bw(pnic_info, scan_info->channel, CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

    // Since setting connect msg in wf_scan_start, wpa_enable should be set before sending the connect msg.
#if 0
    wdn_net_info_st *wdn_info = wf_wdn_find_info(pnic_info,wf_wlan_get_cur_bssid(pnic_info));
    if(wdn_info == NULL)DbgPrint("---ZY_TEST----No wdn_info.\n");
    wdn_info->wpa_enable = wf_true;
    wdn_info->rsn_enable = wf_true;
#endif
	wf_set_auth(adapter);
	wf_set_wpa_ie(adapter);
    wf_set_auth_cipher(adapter);
    
    wf_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                  mac_addr,
                  wf_wlan_get_cur_ssid(pnic_info), 1,
                  NULL, 0,
                  wf_true);
#else
#ifdef CONFIG_CONCURRENT_MODE
	nic_info_st *pvir_nic = pnic_info->vir_nic;
	mlme_state_e state;

	wf_mlme_get_state(pvir_nic, &state);
	if(state == MLME_STATE_ASSOC || state == MLME_STATE_AUTH || state == MLME_STATE_SCAN) {
		LOG_E("another nic is scanning");
		return NDIS_STATUS_FAILURE;
	}
#endif

	ap_info = adapter->ap_info;
	if(ap_info == NULL) {
		LOG_E("error!!! ap info is NULL!!!\n");
		return NDIS_STATUS_FAILURE;
	}

	LOG_D("start");

	scan_info = wf_find_scan_info_by_ssid(adapter, pDot11SSIDList);
	if(scan_info == NULL) {
		LOG_E("wf_find_scan_info_by_ssid fail\n");
 		return NDIS_STATUS_FAILURE; 
	}

	ap_info->scan_info = scan_info;
	ap_info->bcn_len = 0;
	ap_info->assoc_req_len = 0;
	ap_info->assoc_resp_len = 0;

	wf_submit_connect_start(adapter, scan_info);

	mac_addr = scan_info->bssid;

    wf_mlme_get_connect(pnic_info, &bconnect);

    if (is_bcast_addr(mac_addr) || is_zero_addr(mac_addr)) {
        LOG_D("clear current connection");
        if (bconnect) {
            wf_mlme_deauth(pnic_info);
        }
        return NDIS_STATUS_FAILURE;
    }

    if (mac_addr_equal(mac_addr, wf_wlan_get_cur_bssid(pnic_info))) {
        if (bconnect) {
            LOG_D("it's has already connected");
            return NDIS_STATUS_SUCCESS;
        }
    }

    /* retrive bssid */
    wf_wlan_set_cur_bssid(pnic_info, mac_addr);
	wf_wlan_set_cur_ssid(pnic_info, &scan_info->ssid);

	wf_set_auth(adapter);
	wf_set_wpa_ie(adapter);
    wf_set_auth_cipher(adapter);

	wf_submit_assoc_start(adapter, scan_info);

    // probe and connect
    LOG_D("connect bss: ssid(%s), bssid("WF_MAC_FMT")",
                wf_wlan_get_cur_ssid(pnic_info)->data,
                WF_MAC_ARG(wf_wlan_get_cur_bssid(pnic_info)));
    wf_mlme_conn_start(pnic_info,
                       wf_wlan_get_cur_bssid(pnic_info),
                       wf_wlan_get_cur_ssid(pnic_info),
					   WF_MLME_FRAMEWORK_NDIS);
#endif

	timer_ctx = wf_get_timer_context(adapter->MibInfo.exception_timer);
	timer_ctx->msg_type = OIDS_PEND_REQ_ASSOC;
	WdfTimerStart(adapter->MibInfo.exception_timer, WDF_REL_TIMEOUT_IN_MS(5000));
    return NDIS_STATUS_PENDING;
}

NDIS_STATUS wf_oids_deassoc(void *adapter)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    PADAPTER pAdapter = adapter;

	nic_info_st *nic_info = pAdapter->nic_info;
#ifdef CONFIG_LPS
    if(WF_RETURN_FAIL == wf_lps_wakeup(nic_info, LPS_CTRL_SCAN, 0))
    {
        return wf_false;
    }
#endif

	if(!wf_mlme_deauth(nic_info)) return ndisStatus;
	return NDIS_STATUS_FAILURE;

}

VOID wf_set_auth(PADAPTER pAdapter)
{
	nic_info_st *pnic_info = pAdapter->nic_info;
	sec_info_st *psec_info = pnic_info->sec_info;

	DOT11_AUTH_ALGORITHM current_auth_type = pAdapter->MibInfo.AuthAlgorithm;
	switch (current_auth_type)
	{
	case DOT11_AUTH_ALGO_80211_OPEN:
		psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
		psec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;
		break;
	case DOT11_AUTH_ALGO_80211_SHARED_KEY:
		psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;
		psec_info->ndisauthtype = wf_ndis802_11AuthModeShared;
		psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;
		break;
	case DOT11_AUTH_ALGO_WPA:
	case DOT11_AUTH_ALGO_WPA_PSK:
	case DOT11_AUTH_ALGO_RSNA:
	case DOT11_AUTH_ALGO_RSNA_PSK:
		psec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;
		psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
		break;
	default:
		psec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;
		psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
	}
	return;
}

VOID wf_set_auth_cipher(PADAPTER pAdapter)
{
	nic_info_st *pnic_info = pAdapter->nic_info;
	sec_info_st *psec_info = pnic_info->sec_info;

	return;
}

VOID wf_set_wpa_ie(PADAPTER pAdapter)
{
	/**
	*	We need the WPA/WAP2 IEs from probe response frame.
	*   According to NIC module's logic, this should be called after sending wf_scan_start() and filling cur_network because
	*	Linux has the corresponding system interface.
	*	However, NDIS doesn't has this kind of OID. So I try to use the information in scanned_info.   2021/03/31
	*/
	nic_info_st *pnic_info = pAdapter->nic_info;
	sec_info_st *psec_info = pnic_info->sec_info;

	wf_wlan_info_t *wlan_info = pnic_info->wlan_info;

	wf_u8 *buf = NULL;
	wf_u32 left;
	int group_cipher = 0, pairwise_cipher = 0;
	wf_u32 null_addr[] = { 0, 0, 0, 0, 0, 0 };
	wf_u16 cnt = 0;

	wf_u8 eid, wps_oui[4] = { 0x0, 0x50, 0xf2, 0x04 };
	int res = 0;

	wf_u8 *pIEs = NULL;
	wf_u16 IEslen = 0;
	wf_80211_mgmt_ie_t *current_pie = NULL;
	wf_u8 *pie = NULL;
	// "ielen" is not the payload length plus header length.
	wf_u16 ielen = 0;

	wf_wlan_scanned_info_t *pscanned_info;
	int scanned_ret;
	BOOLEAN FlagFindInfo = FALSE;
	wf_wlan_scanned_each_begin(pscanned_info, pnic_info)
	{
		if (!memcmp(pscanned_info->bssid, wlan_info->cur_network.bssid, sizeof(wf_80211_bssid_t)) &&
			!memcmp(&pscanned_info->ssid, &wlan_info->cur_network.ssid, sizeof(wf_wlan_ssid_t)))
		{
			LOG_D("Find network information in scanned infomation.");
			FlagFindInfo = TRUE;
			pIEs = pscanned_info->ies + 12; // Exclude probe stamp, beacon interval and capability info.
			IEslen = pscanned_info->ie_len -12;
			break;
		}
	}
	wf_wlan_scanned_each_end(pnic_info, &scanned_ret);
	if (FlagFindInfo == FALSE)
	{
		LOG_E("Can't find current network in scanned infomation!");
		return;
	}

	LOG_D("Set WPA/RSN IEs.");
	// RSN has a higher priority compared with WPA.
	if (wf_80211_mgmt_ies_search_with_oui(pIEs, IEslen, WF_80211_MGMT_EID_VENDOR_SPECIFIC, WPA_OUI, &current_pie) == WF_RETURN_OK)
	{
		pie = current_pie;
		ielen = current_pie->len + 2;
	}
	else
	{
		LOG_E("Can not find WPA IE.");
	}

	if (wf_80211_mgmt_ies_search(pIEs, IEslen, WF_80211_MGMT_EID_RSN, &current_pie) == WF_RETURN_OK)
	{
		pie = current_pie;
		ielen = current_pie->len + 2;
	}
	else
	{
		LOG_E("Can not find RSN IE.");
	}
	if (pie == NULL)
	{
		LOG_D("Not WPA or WPA2.");
		goto exit;
	}

	if (ielen > WF_MAX_WPA_IE_LEN)
	{
		goto exit;
	}

	if (ielen)
	{
		buf = wf_malloc(ielen);
		if (buf == NULL)
		{
			res = -1;
			goto exit;
		}
		memcpy(buf, pie, ielen); 

		if (ielen < RSN_HEADER_LEN)
		{
			res = -1;
			goto exit;
		}
		
		// Adjust pairwise_cipher by NDIS information.
		if(pAdapter->MibInfo.AuthAlgorithm == DOT11_AUTH_ALGO_WPA_PSK ||
			pAdapter->MibInfo.AuthAlgorithm == DOT11_AUTH_ALGO_WPA)
		{
			wf_ndis_wpa_buf_adjust(buf, &ielen, pAdapter->MibInfo.UnicastCipherAlgorithm);
		}
		if(pAdapter->MibInfo.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK ||
			pAdapter->MibInfo.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA)
		{
			wf_ndis_rsn_buf_adjust(buf, &ielen, pAdapter->MibInfo.UnicastCipherAlgorithm);
		}


		
		if (!wf_80211_mgmt_wpa_parse(buf, ielen, &group_cipher, &pairwise_cipher))
		{
		    psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
        	psec_info->ndisauthtype = WF_WLAN_SEC_AUTHMODEWPAPSK;
			memcpy(psec_info->supplicant_ie, buf, ielen);
		}
		else if (!wf_80211_mgmt_rsn_parse(buf, ielen, &group_cipher, &pairwise_cipher))
		{
		    psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
        	psec_info->ndisauthtype = wf_ndis802_11AuthModeWPA2PSK;
			memcpy(psec_info->supplicant_ie, buf, ielen);
		}
		
		switch (group_cipher)
		{
			case CIPHER_SUITE_TKIP:
				psec_info->dot118021XGrpPrivacy = _TKIP_;
				psec_info->ndisencryptstatus = wf_ndis802_11Encryption2Enabled;
				pAdapter->MibInfo.MulticastCipherAlgorithm = DOT11_CIPHER_ALGO_TKIP;
				LOG_D("dot118021XGrpPrivacy=_TKIP_");
				break;
			case CIPHER_SUITE_CCMP:
				psec_info->dot118021XGrpPrivacy = _AES_;
				psec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
				pAdapter->MibInfo.MulticastCipherAlgorithm = DOT11_CIPHER_ALGO_CCMP;
				LOG_D("dot118021XGrpPrivacy=_AES_");
				break;
		}

		switch (pairwise_cipher)
		{
			case CIPHER_SUITE_NONE:
				break;
			case CIPHER_SUITE_TKIP:
				psec_info->dot11PrivacyAlgrthm = _TKIP_;
				psec_info->ndisencryptstatus = wf_ndis802_11Encryption2Enabled;
				LOG_D("dot11PrivacyAlgrthm=_TKIP_");
				break;
			case CIPHER_SUITE_CCMP:
				psec_info->dot11PrivacyAlgrthm = _AES_;
				psec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
				LOG_D("dot11PrivacyAlgrthm=_AES_");
				break;
		}
		
		while (cnt < ielen)
		{
			eid = buf[cnt];
			if (eid == _VENDOR_SPECIFIC_IE_ && !memcmp(&buf[cnt + 2], wps_oui, 4))
			{
				LOG_D("SET WPS_IE");
				psec_info->wps_ie_len = WF_MIN(buf[cnt + 1] + 2, 512);
				memcpy(psec_info->wps_ie, &buf[cnt], psec_info->wps_ie_len);
				cnt += buf[cnt + 1] + 2;
				break;
			}
			else
			{
				cnt += buf[cnt + 1] + 2;
			}
		}

		wf_mcu_set_on_rcr_am(pnic_info, wf_false);
		wf_mcu_set_hw_invalid_all(pnic_info);
		wf_mcu_set_sec_cfg(pnic_info, psec_info->dot11AuthAlgrthm);
	}

exit:
	if (buf)
	{
		wf_free(buf);
	}
	return;
}

VOID wf_check_PMKID_candidate(PADAPTER pAdapter)
{
	/**
	*	Roaming feature has not yet been implemented.    2021/04/02 
	*/
#if 0
	nic_info_st *nic_info = pAdapter->nic_info;
	assoc_state_e AssocState = wf_assoc_get_state(nic_info);
	if (AssocState == ASSOC_STATE_SUCCESS) return;

	DOT11_BSSID_CANDIDATE CurrentCandidate;
	wf_memcpy(&(CurrentCandidate.BSSID), pAdapter->MibInfo.DesiredBSSID,sizeof(DOT11_MAC_ADDRESS));
	// Set this value to zero for not supporting roam.
	CurrentCandidate.uFlags = 0;

	UCHAR Buffer[sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS) +
		 sizeof(DOT11_BSSID_CANDIDATE)];
	ULONG Size;
	PDOT11_PMKID_CANDIDATE_LIST_PARAMETERS  PMKIDParam;

	PMKIDParam = (PDOT11_PMKID_CANDIDATE_LIST_PARAMETERS)Buffer;
	MP_ASSIGN_NDIS_OBJECT_HEADER(PMKIDParam->Header,
		NDIS_OBJECT_TYPE_DEFAULT,
		DOT11_PMKID_CANDIDATE_LIST_PARAMETERS_REVISION_1,
		sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS));
	PMKIDParam->uCandidateListSize = 1;
	PMKIDParam->uCandidateListOffset = sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS);

	Size = sizeof(DOT11_BSSID_CANDIDATE);
	NdisMoveMemory(Add2Ptr(PMKIDParam, sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS)),
		&CurrentCandidate,
		Size);
	wf_fill_indicate_info(pAdapter,
		NDIS_STATUS_DOT11_PMKID_CANDIDATE_LIST,
		NULL,
		Buffer,
		sizeof(DOT11_PMKID_CANDIDATE_LIST_PARAMETERS) + Size);
#endif
	return;
}

void wf_oids_exception_handle(WDFTIMER WdfTimer)
{
	wf_oids_timer_ctx_t *timer_ctx;
	PADAPTER padapter;
	wf_mib_info_t 		*mib;
	nic_info_st *pnic_info;
	mlme_state_e state;
	PNDIS_OID_REQUEST   request;

	timer_ctx = wf_get_timer_context(WdfTimer);
	padapter = timer_ctx->padapter;
	mib = &padapter->MibInfo;
	pnic_info = padapter->nic_info;

	LOG_D("exception timer execed! msg=%d", timer_ctx->msg_type);

	switch(timer_ctx->msg_type) {
	case OIDS_PEND_REQ_ASSOC:
		wf_mlme_get_state(pnic_info, &state);
		if(state == MLME_STATE_ASSOC || state == MLME_STATE_AUTH) {
			LOG_D("mlme is still execed");
			WdfTimerStart(padapter->MibInfo.exception_timer, WDF_REL_TIMEOUT_IN_MS(5000));
			return;
		} else if(padapter->PendedRequest != NULL){
			request = padapter->PendedRequest;
			if(request->RequestType == NdisRequestSetInformation &&
				request->DATA.SET_INFORMATION.Oid == OID_DOT11_CONNECT_REQUEST) {
				LOG_D("submit assoc failed!");
				wf_submit_assoc_complete(padapter, DOT11_ASSOC_STATUS_FAILURE);
				wf_submit_connect_complete(padapter, DOT11_CONNECTION_STATUS_FAILURE);
				Mp11CompletePendedRequest(padapter, NDIS_STATUS_FAILURE);
			}
		}
		break;
	default:
		break;
	}

	timer_ctx->msg_type = 0;
}

NDIS_STATUS wf_oids_exception_timer_create(void *adapter)
{
	PADAPTER 			padapter = adapter;
	wf_mib_info_t 		*mib = &padapter->MibInfo;
	WDF_TIMER_CONFIG    config;
	NTSTATUS        	ntStatus;
	NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
	WDF_OBJECT_ATTRIBUTES  attributes;
	wf_oids_timer_ctx_t *timer_ctx;

	WDF_TIMER_CONFIG_INIT(&config, wf_oids_exception_handle);
	//config.Period = 1;
	
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                                   &attributes,
                                   wf_oids_timer_ctx_t
                                   );
    attributes.ParentObject = padapter->WdfDevice;

    ntStatus = WdfTimerCreate(
                             &config,
                             &attributes,
                             &(mib->exception_timer)     // Output handle
                             );
	
	if (!NT_SUCCESS(ntStatus)) {
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
		LOG_D("create failed");
        return ndisStatus;
    }

	LOG_D("create success");

	timer_ctx = wf_get_timer_context(mib->exception_timer);
    timer_ctx->padapter = padapter;
	timer_ctx->msg_type = 0;

	WdfTimerStart(mib->exception_timer, WDF_REL_TIMEOUT_IN_MS(1000));

	return ndisStatus;
}

void wf_oids_adapt_init(void *adapter)
{
	PADAPTER padapter = adapter;
	WDF_OBJECT_ATTRIBUTES   attributes;
	NTSTATUS                ntStatus;

	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = padapter->WdfDevice;

    ntStatus = WdfSpinLockCreate(&attributes, &padapter->MibInfo.bss_lock);
    if (ntStatus != STATUS_SUCCESS) {
        LOG_D("create bss lock failed");
        return;
    }

	padapter->ap_info = wf_malloc(sizeof(wf_ap_info_t));
	if(padapter->ap_info == NULL){
		LOG_E("malloc ap info failed!!!\n");
		return;
	}

	memset(padapter->ap_info, 0, sizeof(wf_ap_info_t));

	wf_oids_exception_timer_create(padapter);
}

void wf_oids_adapt_deinit(void *adapter)
{
	PADAPTER padapter = adapter;
	wf_mib_info_t 		*mib = &padapter->MibInfo;
	wf_u8 ret;

	if(padapter->ap_info != NULL) {
		wf_free(padapter->ap_info);
	}

	ret = WdfTimerStop(mib->exception_timer, FALSE);
	LOG_D("exception timer exit %d", ret);
}

VOID wf_ndis_rsn_buf_adjust(wf_u8 *buf, wf_u16 *ielen, wf_u32 uniAlgo)
{
	wf_u8 uniAlgoCount = *(buf+8);
	*(buf + 8) = 1;
	if(uniAlgoCount < 2) return;
	// We only keep one unicast cipher algorithm in memory.  If its number is greater than 1,
	// remove the rest.
	wf_u8 rmv_buf_len = 4 * (uniAlgoCount - 1);
	if(uniAlgo == DOT11_CIPHER_ALGO_TKIP) *(buf + 13) = 2;// TKIP
	if(uniAlgo == DOT11_CIPHER_ALGO_CCMP) *(buf + 13) = 4;// CCMP
	wf_memcpy(buf + 14, buf + 14 + rmv_buf_len, *ielen - 14 + rmv_buf_len);
	*(buf + 1) -= rmv_buf_len;
	*ielen -= rmv_buf_len;
	return;
}

VOID wf_ndis_wpa_buf_adjust(wf_u8 *buf, wf_u16 *ielen, wf_u32 uniAlgo)
{
	// TODO: Finish this function if RSN succeed.
	return;
}


