/*++

Copyright (c) ZTOPMICRO Co., Ltd. All rights reserved.

Module Name:
    wf_oids_adapt.c

Abstract:
    Functions to adapt the OIDs interfaces.
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    03-06-2021    Created
Notes:

--*/

#include "wf_debug.h"
#include "pcomp.h"

#define MP_MAX_DEVICE_NUM (16)

#ifdef _X86_
#define WF_CACHE_FILE  L"\\??\\C:\\Program Files\\ZTOPMICRO\\scics_info.txt"
#else
#define WF_CACHE_FILE  L"\\??\\C:\\Program Files (x86)\\ZTOPMICRO\\scics_info.txt"
#endif

static void wf_update_bss_list(void* adapter)
{
	PADAPTER padapter = adapter;
	nic_info_st* pnic_info = (nic_info_st*)padapter->nic_info;
	wf_wlan_mgmt_scan_que_for_rst_e scanned_ret;
	wf_wlan_mgmt_scan_que_node_t* pscanned_info = NULL;
	wf_mib_info_t* mib_info = padapter->mib_info;
	wf_u32  bss_cnt_backup, wait_cnt;
	bss_cnt_backup = mib_info->bss_cnt;
	wait_cnt = 0;
	WdfSpinLockAcquire(mib_info->bss_lock);

	while (!WF_CANNOT_RUN(pnic_info)) {
		mib_info->bss_cnt = 0;
		wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscanned_info) {
			memcpy(&mib_info->bss_node[mib_info->bss_cnt++], pscanned_info, sizeof(wf_wlan_mgmt_scan_que_node_t));
		}
		wf_wlan_mgmt_scan_que_for_end(scanned_ret);

		if (scanned_ret != WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL) {
			break;
		}
		else {
			wf_msleep(1);
			wait_cnt++;
			if(wait_cnt >= 1000) {
				mib_info->bss_cnt = bss_cnt_backup;
				LOG_E("Error, give up updating bss list.");
				break;
			}
		}
	}
	LOG_D("scan_ret=%d, bss_cnt=%d", scanned_ret, mib_info->bss_cnt);
	
	WdfSpinLockRelease(mib_info->bss_lock);
}

VOID Mp11CompletePendedRequest(PADAPTER           pAdapter, NDIS_STATUS ndisStatus)
{
	KIRQL oldIrql;
	wf_bool irqlFlag;
	if(pAdapter->oid == NULL) return;

	irqlFlag = wf_false;	
	oldIrql = KeGetCurrentIrql();
	if(oldIrql < DISPATCH_LEVEL) irqlFlag = wf_true;
	if(irqlFlag == wf_true) KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);
	LOG_D("Pended OID has been completed.")
	if(pAdapter->beSetOid == wf_true)
	{
		NdisMSetInformationComplete(pAdapter->MiniportAdapterHandle, ndisStatus);
	}else
	{
		NdisMQueryInformationComplete(pAdapter->MiniportAdapterHandle, ndisStatus);
	}
	pAdapter->oid = NULL;
	if(irqlFlag == wf_true) KeLowerIrql(oldIrql);
	return;
}


wf_u64 wf_get_speed_by_raid(wf_u8 raid)
{

}

VOID wf_submit_disassoc_complete(PADAPTER pAdapter, ULONG Reason)
{
	P_GLUE_INFO_T     prGlueInfo;
	prGlueInfo = (P_GLUE_INFO_T)pAdapter->parent;
	prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
	wf_indicate_StatusAndComplete(prGlueInfo, NDIS_STATUS_MEDIA_DISCONNECT, NULL, 0);
	wf_set_hw_default_cipher(pAdapter);
	return;
}

NDIS_STATUS wf_submit_assoc_complete(PADAPTER padapter, ULONG status)
{
	if(status == NDIS_STATUS_SUCCESS)
	{
		LOG_D("association complete.");
		wf_assoc_req_info_update(padapter);
		wf_save_assoc_ssid(padapter, wf_true);
	}
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_submit_connect_complete(PADAPTER prAdapter, ULONG status)
{
	P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T)prAdapter->parent;

	if(status == NDIS_STATUS_SUCCESS)
	{
		wf_indicate_StatusAndComplete(prGlueInfo,
	        NDIS_STATUS_MEDIA_CONNECT, NULL, 0);

		prAdapter->State = OP_STATE;
	    prAdapter->Dot11RunningMode = NET_TYPE_INFRA;
		prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_CONNECTED;
	}
	return NDIS_STATUS_SUCCESS;
}

void wf_submit_link_speed(PADAPTER padapter, ULONG64  linkSpeed)    
{
	
}

void wf_submit_link_qual(PADAPTER padapter)    
{
	
}


VOID
wf_sec_set_cipher_suite (
    IN PADAPTER prAdapter,
    IN wf_u32     u4CipherSuitesFlags
    )
{
	wf_mib_info_t* mib_info = prAdapter->mib_info;
	mib_info->u4CipherSuitesFlags = u4CipherSuitesFlags;
	return;
}

ENUM_PARAM_MEDIA_STATE_T
wf_get_media_state_indicated(
	IN P_GLUE_INFO_T	prGlueInfo
	)
{
	ASSERT(prGlueInfo);

	return prGlueInfo->eParamMediaStateIndicated;
}

NDIS_STATUS
wf_set_scan(PADAPTER prAdapter)
{
	NDIS_STATUS ndisStatus = wf_set_start_scan(prAdapter->nic_info);
	return ndisStatus;
}


NDIS_STATUS wf_set_start_scan(void *nic_info)
{
	nic_info_st *pnic_info = nic_info;
	wf_bool is_connected, is_busy;
	int ret;
#ifdef CONFIG_CONCURRENT_MODE
	nic_info_st *pvir_nic = pnic_info->vir_nic;
	mlme_state_e state;
#endif
	wf_wlan_ssid_t ssid = {0};
	wf_wlan_ssid_t *pssid;
	static wf_u8 first_scan = 1;
	wf_u8 ssid_num;

	pnic_info->is_up = 1;

#ifdef CONFIG_CONCURRENT_MODE
	wf_mlme_get_state(pvir_nic, &state);
	if(state <= MLME_STATE_ASSOC) {
		LOG_D("another nic is scanning");
		wf_os_api_ind_scan_done(pnic_info, wf_true);
		return 0;
	}
#endif

	if(nic_info == NULL) {
		LOG_E("param is NULL");
		return NDIS_STATUS_FAILURE;
	}

	if(first_scan) {
		if(wf_get_assoc_ssid(pnic_info->hif_node, &ssid)) {
			pssid = &ssid;
			ssid_num = 1;
		} else {
			pssid = NULL;
			ssid_num = 0;
		}
		first_scan = 0;
	} else {
		pssid = NULL;
		ssid_num = 0;
	}

	if(pssid != NULL) {
		LOG_D("------->>>>scan with ssid!!!");
	}


	wf_mlme_get_connect(pnic_info, &is_connected);
	if (is_connected) {
        wf_mlme_get_traffic_busy(pnic_info, &is_busy);
        if (is_busy) {
            wf_os_api_ind_scan_done(pnic_info, wf_true, 0);

            return 0;
        }
        ret = wf_mlme_scan_start(pnic_info, SCAN_TYPE_PASSIVE,
                           NULL, ssid_num, NULL, 0, WF_MLME_FRAMEWORK_NDIS);
    } else {
        ret = wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                           NULL, ssid_num, NULL, 0, WF_MLME_FRAMEWORK_NDIS);
    }

	if(ret != 0) {
		LOG_D("[%s-%d] scan start failed! ret=%d\n", __FUNCTION__, __LINE__, ret);
		ret = NDIS_STATUS_FAILURE;
	} else {
		ret = NDIS_STATUS_SUCCESS;
	}

	return ret;

}

NDIS_STATUS wf_submit_scan_complete(PADAPTER pAdapter)
{
	wf_mib_info_t* 	mib_info;
	NDIS_STATUS ndis_status = NDIS_STATUS_SUCCESS;

	mib_info = pAdapter->mib_info;
	KeSetEvent(&mib_info->scan_hidden_finish, 0, FALSE);
	
	wf_update_bss_list(pAdapter);
	if(pAdapter->bRequestedScan == TRUE)
	{
		pAdapter->bRequestedScan = FALSE;
		pAdapter->CurrentRequestID_Scan = NULL;
	}

	LOG_D("NDIS scan complete. BSS list is updated.");
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_mp_oids_init(void *adapter)
{
	PADAPTER padapter = adapter;
	wf_mib_info_t *mib_info;
	P_PARAM_BSSID_EX_T curApInfo;
	WDF_OBJECT_ATTRIBUTES   attributes;
	NTSTATUS                ntStatus;
	NDIS_STATUS				ndisStatus;
	ndisStatus = NDIS_STATUS_SUCCESS;

	
	
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = padapter->WdfDevice;

	mib_info = wf_malloc(sizeof(wf_mib_info_t));
	if(mib_info == NULL)
	{
		LOG_E("Allocate mib_info failed.");
		ndisStatus = NDIS_STATUS_FAILURE;
		goto out;
	}
	padapter->mib_info = mib_info;
	wf_memset(mib_info, 0, sizeof(wf_mib_info_t));
	
	curApInfo = wf_malloc(sizeof(PARAM_BSSID_EX_T));
	if(curApInfo == NULL)
	{
		LOG_E("Allocate curApInfo failed.");
		ndisStatus = NDIS_STATUS_FAILURE;
		goto out;
	}
	mib_info->curApInfo = curApInfo;
	wf_memset(curApInfo, 0, sizeof(PARAM_BSSID_EX_T));

	// Initialize the writeKeyLock which is used to avoid writing more than one key
	// on the same time.
	mib_info->writeKeyLock.lock_type = WF_LOCK_TYPE_MUTEX;
	wf_lock_mutex_init(&mib_info->writeKeyLock.lock_mutex);

	
    ntStatus = WdfSpinLockCreate(&attributes, &mib_info->bss_lock);
    if (ntStatus != STATUS_SUCCESS) {
        LOG_D("create bss lock failed");
		NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
		goto out;
    }
	
	padapter->ap_info = wf_malloc(sizeof(wf_ap_info_t));
	
	if(padapter->ap_info == NULL){
		LOG_E("malloc ap info failed!!!\n");
		NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
		goto out;
	}
	memset(padapter->ap_info, 0, sizeof(wf_ap_info_t));

	KeInitializeEvent(&mib_info->scan_hidden_finish, SynchronizationEvent, FALSE);
	
	KeInitializeEvent(&mib_info->halt_deauth_finish, SynchronizationEvent, FALSE);

	//wf_reset_cipher_abilities(padapter);

	
	/*
	*  Abandon this because nic has maintained connection status.
	*/
	//wf_oids_exception_timer_create(padapter);

	return ndisStatus;
out:
	wf_mp_oids_deinit(padapter);
	return ndisStatus;
}

VOID wf_mp_oids_deinit(PADAPTER	pAdapter)
{
	wf_mib_info_t *mib_info = pAdapter->mib_info;

	if(mib_info->curApInfo != NULL)
	{
		wf_free(mib_info->curApInfo);
		mib_info->curApInfo = NULL;
	}

	wf_free(mib_info);
	pAdapter->mib_info = NULL;
	
	return;
}


NDIS_STATUS wf_set_start_deassoc(void *adapter, wf_bool en_ind)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	P_GLUE_INFO_T prGlueInfo;
    PADAPTER pAdapter = adapter;
	nic_info_st *nic_info = pAdapter->nic_info;
	prGlueInfo = (P_GLUE_INFO_T)pAdapter->parent;
	prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
#ifdef CONFIG_LPS
    if(WF_RETURN_FAIL == wf_lps_wakeup(nic_info, LPS_CTRL_SCAN, 0))
    {
        return wf_false;
    }
#endif

	if(!wf_mlme_deauth(nic_info, en_ind)) return ndisStatus;
	return NDIS_STATUS_FAILURE;

}

NDIS_STATUS wf_set_start_assoc (PADAPTER adapter, P_PARAM_SSID_T pDot11SSID)
{
    
    nic_info_st *pnic_info = adapter->nic_info;
	P_GLUE_INFO_T prGlueInfo = adapter->parent;
    //sec_info_st *psec_info = pnic_info->sec_info;
	wf_wlan_mgmt_scan_que_node_t *scan_info;
	//wf_ap_info_t *ap_info;
	wf_u8 *mac_addr;
	//char ssid[64] = {0};
    wf_bool bconnect = wf_false;
	wf_mib_info_t *mib_info = adapter->mib_info;
	P_PARAM_BSSID_EX_T 			curApInfo = mib_info->curApInfo;
	int i;
	
#ifdef CONFIG_CONCURRENT_MODE
	nic_info_st *pvir_nic = pnic_info->vir_nic;
	mlme_state_e state;

	wf_mlme_get_state(pvir_nic, &state);
	if(state == MLME_STATE_ASSOC || state == MLME_STATE_AUTH || state == MLME_STATE_SCAN) {
		LOG_E("another nic is scanning");
		return NDIS_STATUS_FAILURE;
	}
#endif

	LOG_D("start association");
	wf_reset_pkt_statistics_info(adapter);
	//prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
	prGlueInfo->bWaitGroupKeyState = FALSE;
	scan_info = wf_find_scan_info_by_ssid(adapter, pDot11SSID);
	if(scan_info == NULL) {
		LOG_D("Can't find the specified ssid");
		if(wf_set_scan_hidden_network(adapter, pDot11SSID) != NDIS_STATUS_SUCCESS) {
				LOG_E("scan hidden network fail");
				return NDIS_STATUS_FAILURE; 
		}
		scan_info = wf_find_scan_info_by_ssid(adapter, pDot11SSID);
		if(scan_info == NULL) {
				LOG_E("wf_find_scan_info_by_ssid fail");
				return NDIS_STATUS_FAILURE; 
		} 
	}
	mib_info->rRssi = scan_info->signal_strength;
	if(adapter->bBssidLocked == TRUE)
	{
		if(!wf_memcmp(adapter->SpecifiedMacAddr, scan_info->bssid, MAC_ADDR_LEN))
	    {
	    	LOG_D("This SSID matches the specified BSSID.");
		}
		else{
			LOG_D("This SSID doesn't match the specified BSSID.");
			return NDIS_STATUS_FAILURE;
		}
	}
		
    wf_mlme_get_connect(pnic_info, &bconnect);

	mac_addr = scan_info->bssid;
	wf_memcpy(curApInfo->arMacAddress, mac_addr, MAC_ADDR_LEN);

	if (is_bcast_addr(mac_addr) || is_zero_addr(mac_addr)) {
		LOG_D("clear current connection");
		if (bconnect) {
			wf_mlme_deauth(pnic_info, wf_true);
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

    // probe and connect
    LOG_D("connect bss: ssid(%s), bssid("WF_MAC_FMT")",
                wf_wlan_get_cur_ssid(pnic_info)->data,
                WF_MAC_ARG(wf_wlan_get_cur_bssid(pnic_info)));
    wf_mlme_conn_start(pnic_info,
                       wf_wlan_get_cur_bssid(pnic_info),
                       wf_wlan_get_cur_ssid(pnic_info),
					   WF_MLME_FRAMEWORK_NDIS, wf_true);		
	// If the media state is not set immediatly, NDIS may send another OID_802_11_SSID.
	return NDIS_STATUS_SUCCESS;
}


static wf_wlan_mgmt_scan_que_node_t *wf_find_scan_info_by_ssid(PADAPTER adapter, P_PARAM_SSID_T pDot11SSID)
{
	nic_info_st *pnic_info = adapter->nic_info;
    wf_wlan_mgmt_scan_que_node_t *pscanned_info = NULL;
	wf_wlan_mgmt_scan_que_node_t *srch_end = NULL;

    //wf_u32 res = 0;
    wf_wlan_mgmt_scan_que_for_rst_e scanned_ret;
    //wf_u16 apCount = 0;
    /* Check if there is space for one more entry */
    wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscanned_info)
    {
        if(wf_memcmp(pscanned_info->ssid.data, pDot11SSID->aucSsid, pDot11SSID->u4SsidLen) == 0
			&& pscanned_info->ssid.length == pDot11SSID->u4SsidLen) {
			srch_end = pscanned_info;
			break;
		}
    }
    wf_wlan_mgmt_scan_que_for_end(scanned_ret);

    return srch_end;
}

VOID wf_set_auth(PADAPTER pAdapter)
{
	nic_info_st *pnic_info = pAdapter->nic_info;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	sec_info_st *psec_info = NULL;
	ENUM_PARAM_AUTH_MODE_T current_auth_type;

	if(pnic_info == NULL || mib_info == NULL) {
		LOG_E("param is NULL");
		return;
	}

	psec_info = pnic_info->sec_info;
	if(psec_info == NULL) {
		LOG_E("param is NULL");
		return;
	}

	current_auth_type = mib_info->auth_mode;
	
	switch (current_auth_type)
	{
	case AUTH_MODE_OPEN:
		psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
		psec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;
		break;
	case AUTH_MODE_SHARED:
		psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;
		psec_info->ndisauthtype = wf_ndis802_11AuthModeShared;
		psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;
		break;
	case AUTH_MODE_WPA:
	case AUTH_MODE_WPA_PSK:
	case AUTH_MODE_WPA2:
	case AUTH_MODE_WPA2_PSK:
		psec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;
		psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
		break;
	default:
		psec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;
		psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
	}
	return;
}

static VOID wf_ndis_wpa_buf_adjust(wf_u8 *buf, wf_u16 *ielen, wf_u32 uniAlgo)
{
	// TODO: Finish this function if WPA/CCMP failed.   2021/07/09
	UNREFERENCED_PARAMETER(buf);
	UNREFERENCED_PARAMETER(ielen);
	UNREFERENCED_PARAMETER(uniAlgo);

	return;
}


static VOID wf_ndis_rsn_buf_adjust(wf_u8 *buf, wf_u16 *ielen, wf_u32 uniAlgo)
{
	wf_u8 rmv_buf_len;
	wf_u8 uniAlgoCount = *(buf+8);
	*(buf + 8) = 1;
	if(uniAlgoCount < 2) return;
	// We only keep one unicast cipher algorithm in memory.  If its number is greater than 1,
	// remove the rest.
	rmv_buf_len = 4 * (uniAlgoCount - 1);
	if(uniAlgo == ENUM_ENCRYPTION2_ENABLED) *(buf + 13) = 2;// TKIP
	if(uniAlgo == ENUM_ENCRYPTION3_ENABLED) *(buf + 13) = 4;// CCMP
	wf_memcpy(buf + 14, buf + 14 + rmv_buf_len, *ielen - 14 + rmv_buf_len);
	*(buf + 1) -= rmv_buf_len;
	*ielen -= rmv_buf_len;
	return;
}


VOID wf_set_wpa_ie(PADAPTER pAdapter)
{
	nic_info_st *pnic_info = pAdapter->nic_info;
	sec_info_st *psec_info = NULL;
	wf_wlan_mgmt_info_t *wlan_info = NULL;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

	wf_u8 *buf = NULL;
	wf_u32 group_cipher = 0, pairwise_cipher = 0;
	wf_u16 cnt = 0;

	wf_u8 eid, wps_oui[4] = { 0x0, 0x50, 0xf2, 0x04 };
	int res = 0;

	wf_u8 *pIEs = NULL;
	wf_u16 IEslen = 0;
	wf_80211_mgmt_ie_t *current_pie = NULL;
	wf_u8 *pie = NULL;
	// "ielen" is not the payload length plus header length.
	wf_u16 ielen = 0;

	wf_wlan_mgmt_scan_que_node_t *pscanned_info = NULL;
	wf_wlan_mgmt_scan_que_for_rst_e scanned_ret;
	BOOLEAN FlagFindInfo = FALSE;

	if(pnic_info == NULL || mib_info == NULL) {
		LOG_E("param is NULL");
		return NDIS_STATUS_FAILURE;
	}

	psec_info = pnic_info->sec_info;
	wlan_info = pnic_info->wlan_mgmt_info;
	if(psec_info == NULL || wlan_info == NULL) {
		LOG_E("param is NULL");
		return NDIS_STATUS_FAILURE;
	}
	
	wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscanned_info)
	{
		if (!wf_memcmp(pscanned_info->bssid, wlan_info->cur_network.bssid, sizeof(wf_80211_bssid_t)) &&
			!wf_memcmp(&pscanned_info->ssid, &wlan_info->cur_network.ssid, sizeof(wf_wlan_ssid_t)))
		{
			LOG_D("Find network information in scanned infomation.");
			FlagFindInfo = TRUE;
			pIEs = pscanned_info->ies + 12; // Exclude probe stamp, beacon interval and capability info.
			IEslen = (wf_u16)(pscanned_info->ie_len -12);
			break;
		}
	}
	wf_wlan_mgmt_scan_que_for_end(scanned_ret);
	if (FlagFindInfo == FALSE)
	{
		LOG_E("Can't find current network in scanned infomation!");
		return;
	}

	LOG_D("Set WPA/RSN IEs.");
	// RSN has a higher priority compared with WPA.
	if (wf_80211_mgmt_ies_search_with_oui(pIEs, IEslen, WF_80211_MGMT_EID_VENDOR_SPECIFIC, WPA_OUI, &current_pie) == WF_RETURN_OK)
	{
		pie = (wf_u8 *)current_pie;
		ielen = current_pie->len + 2;
	}
	else
	{
		LOG_E("Can not find WPA IE.");
	}

	if (wf_80211_mgmt_ies_search(pIEs, IEslen, WF_80211_MGMT_EID_RSN, &current_pie) == WF_RETURN_OK)
	{
		pie = (wf_u8 *)current_pie;
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
		wf_memcpy(buf, pie, ielen); 

		if (ielen < RSN_HEADER_LEN)
		{
			res = -1;
			goto exit;
		}
		
		// Adjust pairwise_cipher by NDIS information.
		if(mib_info->auth_mode == AUTH_MODE_WPA_PSK ||
			mib_info->auth_mode == AUTH_MODE_WPA)
		{
			wf_ndis_wpa_buf_adjust(buf, &ielen, mib_info->eEncStatus);
		}
		if(mib_info->auth_mode== AUTH_MODE_WPA2_PSK ||
			mib_info->auth_mode == AUTH_MODE_WPA2)
		{
			wf_ndis_rsn_buf_adjust(buf, &ielen, mib_info->eEncStatus);
		}


		
		if (!wf_80211_mgmt_wpa_parse(buf, ielen, &group_cipher, &pairwise_cipher))
		{
			psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
			psec_info->ndisauthtype = wf_ndis802_11AuthModeWPAPSK;
			psec_info->wpa_enable = wf_true;
			psec_info->rsn_enable = wf_false;
			wf_memcpy(psec_info->supplicant_ie, buf, ielen);
		}
		else if (!wf_80211_mgmt_rsn_parse(buf, ielen, &group_cipher, &pairwise_cipher))
		{
			psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
			psec_info->ndisauthtype = wf_ndis802_11AuthModeWPA2PSK;
			psec_info->wpa_enable = wf_false;
			psec_info->rsn_enable = wf_true;
			wf_memcpy(psec_info->supplicant_ie, buf, ielen);
		}
		
		switch (group_cipher)
		{
			case CIPHER_SUITE_TKIP:
				psec_info->dot118021XGrpPrivacy = _TKIP_;
				psec_info->ndisencryptstatus = wf_ndis802_11Encryption2Enabled;
				mib_info->HwMulticastCipher = WF_DOT11_CIPHER_ALGO_TKIP;
				LOG_D("dot118021XGrpPrivacy=_TKIP_");
				break;
			case CIPHER_SUITE_CCMP:
				psec_info->dot118021XGrpPrivacy = _AES_;
				psec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
				mib_info->HwMulticastCipher = WF_DOT11_CIPHER_ALGO_CCMP;
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
				mib_info->HwUnicastCipher = WF_DOT11_CIPHER_ALGO_TKIP;			
				LOG_D("dot11PrivacyAlgrthm=_TKIP_");
				break;
			case CIPHER_SUITE_CCMP:
				psec_info->dot11PrivacyAlgrthm = _AES_;
				psec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
				mib_info->HwUnicastCipher = WF_DOT11_CIPHER_ALGO_CCMP;							
				LOG_D("dot11PrivacyAlgrthm=_AES_");
				break;
		}
		
		while (cnt < ielen)
		{
			eid = buf[cnt];
			if (eid == WF_80211_MGMT_EID_VENDOR_SPECIFIC && !wf_memcmp(&buf[cnt + 2], wps_oui, 4))
			{
				LOG_D("SET WPS_IE");
				psec_info->wps_ie_len = WF_MIN(buf[cnt + 1] + 2, 512);
				wf_memcpy(psec_info->wps_ie, &buf[cnt], psec_info->wps_ie_len);
				cnt += buf[cnt + 1] + 2;
				break;
			}
			else
			{
				cnt += buf[cnt + 1] + 2;
			}
		}
		WdfWorkItemEnqueue(pAdapter->setWpaWorkitem);
	}

exit:
	if (buf)
	{
		wf_free(buf);
	}
	return;
}

VOID wf_set_auth_cipher(PADAPTER pAdapter)
{
	UNREFERENCED_PARAMETER(pAdapter);
	return;
}

VOID
wf_indicate_StatusAndComplete(
    IN P_GLUE_INFO_T prGlueInfo,
    IN NDIS_STATUS eStatus,
    IN PVOID       pvBuf,
    IN wf_u32     u4BufLen
    )
{
	PADAPTER pAdapter;
	wf_mib_info_t *mib_info;
    ASSERT(prGlueInfo);

    /* Indicate the protocol that the media state was changed. */
    NdisMIndicateStatus(prGlueInfo->rMiniportAdapterHandle,
                        (NDIS_STATUS) eStatus,
                        (PVOID) pvBuf,
                        u4BufLen);

    /* NOTE: have to indicate status complete every time you indicate status */
    NdisMIndicateStatusComplete(prGlueInfo->rMiniportAdapterHandle);

    if (eStatus == NDIS_STATUS_MEDIA_CONNECT || eStatus == NDIS_STATUS_MEDIA_DISCONNECT) {

        if (eStatus == NDIS_STATUS_MEDIA_CONNECT) {
            prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_CONNECTED;
        }
        else if (eStatus == NDIS_STATUS_MEDIA_DISCONNECT) {
            prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
			pAdapter = prGlueInfo->prAdapter;
			mib_info = pAdapter->mib_info;
			NdisZeroMemory(mib_info->curApInfo->rSsid.aucSsid, PARAM_MAX_LEN_SSID);
			mib_info->curApInfo->rSsid.u4SsidLen = 0;
        }

        if(wf_reset_media_stream_mode(prGlueInfo->prAdapter) == TRUE) {
            MEDIA_STREAMING_INDICATIONS_T rMediaStreamIndication;

            // following MSDN for Media Streaming Indication
            rMediaStreamIndication.StatusType = Ndis802_11StatusType_MediaStreamMode;
            rMediaStreamIndication.MediaStreamMode = Ndis802_11MediaStreamOff;
			LOG_D("Reset media stream mode.");
            NdisMIndicateStatus(prGlueInfo->rMiniportAdapterHandle,
                    NDIS_STATUS_MEDIA_SPECIFIC_INDICATION,
                    (PVOID) &rMediaStreamIndication,
                    sizeof(MEDIA_STREAMING_INDICATIONS_T));
        }
    }
} /* kalIndicateStatusAndComplete */

BOOLEAN
wf_reset_media_stream_mode(
	IN PADAPTER prAdapter
	)
{
	wf_mib_info_t *mib_info;
	ASSERT(prAdapter);
	mib_info = prAdapter->mib_info;
	if(mib_info->eLinkAttr.ucMediaStreamMode != 0) {
		mib_info->eLinkAttr.ucMediaStreamMode = 0;
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOLEAN
wf_is_broadcast(wf_u8* inputAddr)
{
	wf_u8 bc_addr[MAC_ADDR_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	if(!wf_memcmp(inputAddr, bc_addr, MAC_ADDR_LEN)){
		return TRUE;
	}
	else{
		return FALSE;
	}
	return FALSE;
}

VOID
wf_attr_reset(PADAPTER pAdapter)
{
	// TODO: Finish all the reset work. This can also be used in
	// mpChar.ResetHandler.   2021/07/08

	wf_reset_cipher_abilities(pAdapter);

	wf_reset_assoc_info(pAdapter);
	
	return;
}

NDIS_STATUS wf_wpa_workitem_init(PADAPTER pAdapter)
{
	WDF_OBJECT_ATTRIBUTES	attributes;
	WDF_WORKITEM_CONFIG     workitemConfig;
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	NTSTATUS	ntStatus;
	PADAPTER_WORKITEM_CONTEXT   adapterWorkitemContext;
    
    // Allocate the work item.
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = pAdapter->WdfDevice;
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&attributes, ADAPTER_WORKITEM_CONTEXT);

    WDF_WORKITEM_CONFIG_INIT(&workitemConfig, wf_set_Wpa_Workitem);
    ntStatus = WdfWorkItemCreate(&workitemConfig,
                                &attributes,
                                &pAdapter->setWpaWorkitem);

    if (!NT_SUCCESS(ntStatus))
    {
        
        LOG_E("Failed to allocate setWpaWorkitem");
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
        goto out;
    } 

    adapterWorkitemContext = GetAdapterWorkItemContext(pAdapter->setWpaWorkitem);
    adapterWorkitemContext->Adapter = pAdapter;
	
out:
	return ndisStatus;
}

VOID
wf_set_Wpa_Workitem(
    __in WDFWORKITEM  WorkItem
    )
{
	PADAPTER                    pAdapter;
    PADAPTER_WORKITEM_CONTEXT adapterWorkItemContext;
	nic_info_st*	pnic_info;
	sec_info_st* psec_info;
    adapterWorkItemContext = GetAdapterWorkItemContext(WorkItem);
    pAdapter = adapterWorkItemContext->Adapter;

	pnic_info = pAdapter->nic_info;
	psec_info = pnic_info->sec_info;
	wf_mcu_set_on_rcr_am(pnic_info, wf_false);
	wf_mcu_set_hw_invalid_all(pnic_info);
	wf_mcu_set_sec_cfg(pnic_info, (wf_u8)psec_info->dot11AuthAlgrthm);
	LOG_D("WPA write mailbox success.");
}

NDIS_STATUS wf_writeKey_workitem_init(PADAPTER pAdapter)
{
	WDF_OBJECT_ATTRIBUTES	attributes;
	WDF_WORKITEM_CONFIG 	workitemConfig;
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	NTSTATUS	ntStatus;
	PADAPTER_WORKITEM_CONTEXT	adapterWorkitemContext;
	
	// Allocate the work item.
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.ParentObject = pAdapter->WdfDevice;
	WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&attributes, ADAPTER_WORKITEM_CONTEXT);

	WDF_WORKITEM_CONFIG_INIT(&workitemConfig, wf_write_key_Workitem);
	
	ntStatus = WdfWorkItemCreate(&workitemConfig,
								&attributes,
								&pAdapter->writeKeyWorkitem);

	if (!NT_SUCCESS(ntStatus))
	{
		LOG_E("Failed to allocate writeKeyWorkitem.");
		NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
		goto out;
	} 

	adapterWorkitemContext = GetAdapterWorkItemContext(pAdapter->writeKeyWorkitem);
	adapterWorkitemContext->Adapter = pAdapter;
	
out:
	return ndisStatus;
}

VOID
wf_write_key_Workitem(
    __in WDFWORKITEM  WorkItem
    )
{
	PADAPTER                    pAdapter;
    PADAPTER_WORKITEM_CONTEXT adapterWorkItemContext;
	wf_mib_info_t *mib_info;
	wf_u32 curKeyIndex;
	P_WF_NICKEY pCurNicKey = NULL;

	wf_u32 wep_key_idx, wep_key_len;
	nic_info_st *pnic_info;
	sec_info_st *psec_info;
	wdn_net_info_st *pwdn_info;
	
    adapterWorkItemContext = GetAdapterWorkItemContext(WorkItem);
    pAdapter = adapterWorkItemContext->Adapter;
	mib_info = pAdapter->mib_info;
	pnic_info = pAdapter->nic_info;
	if(mib_info == NULL || pnic_info == NULL)
	{
		LOG_E("mib_info or pnic_info is NULL.");
		return;
	}
	psec_info = pnic_info->sec_info;
	
	curKeyIndex = 0;

	wf_lock_lock(&mib_info->writeKeyLock);
	for(curKeyIndex; curKeyIndex < MAX_KEY_NUM; curKeyIndex++)
	{
		if(mib_info->KeyTable[curKeyIndex].ToBeWritten == TRUE) break;
	}
	if(curKeyIndex == MAX_KEY_NUM) return;
	mib_info->KeyTable[curKeyIndex].ToBeWritten = FALSE;
	pCurNicKey = mib_info->KeyTable + curKeyIndex;

	if (pCurNicKey->AlgoId == WF_DOT11_CIPHER_ALGO_WEP40 ||
		pCurNicKey->AlgoId == WF_DOT11_CIPHER_ALGO_WEP104 ||
		pCurNicKey->AlgoId == WF_DOT11_CIPHER_ALGO_WEP)
	{
		if (pCurNicKey->KeyIndex >= WF_DOT11_MAX_NUM_DEFAULT_KEY ||
			pCurNicKey->KeyLength == 0)
		{
			LOG_E("Set WEP key error!");
			return;
		}
		wep_key_idx = pCurNicKey->KeyIndex;
		wep_key_len = pCurNicKey->KeyLength;
		psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;

		wep_key_len = wep_key_len <= 5 ? 5 : 13; // 5 bytes for WEP40, 13 bytes for WEP104.
		if (wep_key_len == 13)
		{
			psec_info->dot11PrivacyAlgrthm = _WEP104_;
		}
		else
		{
			psec_info->dot11PrivacyAlgrthm = _WEP40_;
		}
		psec_info->dot11PrivacyKeyIndex = wep_key_idx;
		wf_memcpy(psec_info->dot11DefKey[wep_key_idx].skey,
			pCurNicKey->KeyValue, wep_key_len);
		psec_info->dot11DefKeylen[wep_key_idx] = wep_key_len;
		psec_info->key_mask |= BIT(wep_key_idx);
	}
	else if (psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) /* 802_1x */
	{
		local_info_st *plocal_info = pnic_info->local_info;
		if (plocal_info->work_mode == WF_INFRA_MODE) /* sta mode */
		{
			pwdn_info = wf_wdn_find_info(pnic_info,
				wf_wlan_get_cur_bssid(pnic_info));
			if (pwdn_info == NULL)
			{
				LOG_E("pwdn_info NULL! ");
				return;
			}

			if (psec_info->ndisencryptstatus == wf_ndis802_11Encryption2Enabled ||
				psec_info->ndisencryptstatus == wf_ndis802_11Encryption3Enabled)
			{
				pwdn_info->dot118021XPrivacy = (wf_u16)psec_info->dot11PrivacyAlgrthm;
			}
			/* PTK */
			if (mib_info->HwUnicastCipher == pCurNicKey->AlgoId
				&& pCurNicKey->bePairwise == TRUE) /* pairwise key */
			{
				LOG_D("pwdn_info->dot118021XPrivacy = %d", pwdn_info->dot118021XPrivacy);
				/* KCK PTK0~127 */
				wf_memcpy(pwdn_info->dot118021x_UncstKey.skey, pCurNicKey->KeyValue, CCMP_KEY_LEN);

				if (pCurNicKey->AlgoId == WF_DOT11_CIPHER_ALGO_TKIP) /* set mic key */
				{
					/* KEK PTK128~255 */
					// The first 8 bytes will be the MIC key used for received packets and the 
					// last 8 bytes will be the MIC key used for transmitted packets. 
					wf_memcpy(pwdn_info->dot11tkiprxmickey.skey, pCurNicKey->RxMICKey, 8);
					wf_memcpy(pwdn_info->dot11tkiptxmickey.skey, pCurNicKey->TxMICKey, 8);
					psec_info->busetkipkey = wf_true;
				}
				if (pwdn_info->dot118021XPrivacy == _AES_)
				{
					LOG_D("sta_hw_set_unicast_key");
					wf_sta_hw_set_unicast_key(pnic_info, pwdn_info);
				}
				pwdn_info->ieee8021x_blocked = wf_false;
			}
			if (mib_info->HwMulticastCipher == pCurNicKey->AlgoId
				&& pCurNicKey->bePairwise == FALSE) /* group key */
			{
				LOG_D("psec_info->dot118021XGrpPrivacy = %d", psec_info->dot118021XGrpPrivacy);
				wf_memcpy(psec_info->dot118021XGrpKey[pCurNicKey->KeyIndex].skey,
					pCurNicKey->KeyValue, CCMP_KEY_LEN);
				wf_memcpy(psec_info->dot118021XGrprxmickey[pCurNicKey->KeyIndex].skey,
					pCurNicKey->RxMICKey, TKIP_MIC_KEY_LEN);
				wf_memcpy(psec_info->dot118021XGrptxmickey[pCurNicKey->KeyIndex].skey,
					pCurNicKey->TxMICKey, TKIP_MIC_KEY_LEN);
				psec_info->binstallGrpkey = wf_true;
				psec_info->dot118021XGrpKeyid = pCurNicKey->KeyIndex;
				if (psec_info->dot118021XGrpPrivacy == _AES_)
				{
					LOG_D("sta_hw_set_group_key");
					wf_sta_hw_set_group_key(pnic_info, pwdn_info);
				}
			}
		}
	}
	mib_info->KeyTable[curKeyIndex].Valid = FALSE;
	wf_lock_unlock(&mib_info->writeKeyLock);
	
	LOG_D("Write key(s) to mailbox success.");
}

NDIS_STATUS wf_asychOID_workitem_init(PADAPTER pAdapter)
{
	WDF_OBJECT_ATTRIBUTES	attributes;
	WDF_WORKITEM_CONFIG 	workitemConfig;
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	NTSTATUS	ntStatus;
	PADAPTER_WORKITEM_CONTEXT	adapterWorkitemContext;
	
	// Allocate the work item.
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.ParentObject = pAdapter->WdfDevice;
	WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&attributes, ADAPTER_WORKITEM_CONTEXT);

	WDF_WORKITEM_CONFIG_INIT(&workitemConfig, wf_asych_OID_Workitem);
	
	ntStatus = WdfWorkItemCreate(&workitemConfig,
								&attributes,
								&pAdapter->asychOidWorkitem);

	if (!NT_SUCCESS(ntStatus))
	{
		LOG_E("Failed to allocate writeKeyWorkitem.");
		NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
		goto out;
	} 

	adapterWorkitemContext = GetAdapterWorkItemContext(pAdapter->asychOidWorkitem);
	adapterWorkitemContext->Adapter = pAdapter;
	
out:
	return ndisStatus;
}

VOID
wf_asych_OID_Workitem(
    __in WDFWORKITEM  WorkItem
    )
{
	PADAPTER                    pAdapter;
    PADAPTER_WORKITEM_CONTEXT adapterWorkItemContext;
	NDIS_STATUS ndisStatus;
	wf_u32              u4SetInfoLen;
	
    adapterWorkItemContext = GetAdapterWorkItemContext(WorkItem);
    pAdapter = adapterWorkItemContext->Adapter;
	ndisStatus = NDIS_STATUS_SUCCESS;
	if(pAdapter->oid == NULL)
	{
		LOG_E("Error! Lose the pended OID.");
		return;
	}
	if(pAdapter->beSetOid == wf_true){
		if(pAdapter->oid == OID_802_11_SSID){
			ndisStatus = wf_wlan_oid_set_ssid(
				pAdapter,
				pAdapter->pvInfomationBuffer,
				pAdapter->u4InformationBufferLength,
				&u4SetInfoLen);
		}
		else if(pAdapter == OID_802_11_BSSID){
			ndisStatus = wf_wlan_oid_set_bssid(
				pAdapter,
				pAdapter->pvInfomationBuffer,
				pAdapter->u4InformationBufferLength,
				&u4SetInfoLen);
		}
		switch (ndisStatus) {
		case NDIS_STATUS_SUCCESS:
		     *pAdapter->pu4ByteWrittenOrRead = u4SetInfoLen;
		     break;

		case NDIS_STATUS_INVALID_LENGTH:
		     *pAdapter->pu4ByteNeeded = u4SetInfoLen;
		     LOG_E("Set OID asychronously failed for invalid length.");
		     break;
		}
	}
	else{
		// If any query OID should be excuted asychronously, add it to here.  2021/08/16
		
	}
	
	if(ndisStatus != NDIS_STATUS_SUCCESS)
	{
		Mp11CompletePendedRequest(pAdapter, ndisStatus);
	}
	return;
}

VOID
wf_assoc_resp_info_update(PADAPTER padapter, rx_pkt_t *nic_pkt)
{
	P_GLUE_INFO_T prGlueInfo = padapter->parent;
	PNDIS_802_11_ASSOCIATION_INFORMATION pAssocInfo = &prGlueInfo->rNdisAssocInfo;
	wf_u16 parsed_len;
	wf_u16 ie_len;
	wf_u8 *pbuf = nic_pkt->pdata;

	parsed_len = 24; // management frame header length.
	pbuf += parsed_len;

	pAssocInfo->AvailableResponseFixedIEs = 
			NDIS_802_11_AI_RESFI_CAPABILITIES |
            NDIS_802_11_AI_RESFI_STATUSCODE |
            NDIS_802_11_AI_RESFI_ASSOCIATIONID;
	wf_memcpy(&pAssocInfo->ResponseFixedIEs.Capabilities, pbuf, 2);
	wf_memcpy(&pAssocInfo->ResponseFixedIEs.StatusCode, pbuf + 2, 2);
	wf_memcpy(&pAssocInfo->ResponseFixedIEs.AssociationId, pbuf + 4, 2);

	parsed_len += 6;
	pbuf += 6;   // fixed fields length.

	prGlueInfo->aucAssocRespIE_Len = nic_pkt->len - parsed_len;
	pAssocInfo->ResponseIELength = prGlueInfo->aucAssocRespIE_Len;
	wf_memcpy(prGlueInfo->aucAssocRespIE, pbuf, prGlueInfo->aucAssocRespIE_Len);
}

VOID
wf_assoc_req_info_update(PADAPTER padapter)
{
	P_GLUE_INFO_T prGlueInfo = padapter->parent;
	nic_info_st *nic_info = padapter->nic_info;
	PNDIS_802_11_ASSOCIATION_INFORMATION pAssocInfo = &prGlueInfo->rNdisAssocInfo;
	wf_wlan_mgmt_info_t *wlan_mgmt_info = nic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &wlan_mgmt_info->cur_network;
	wdn_net_info_st *wdn_info;
	
	wdn_info = wf_wdn_find_info(nic_info,wf_wlan_get_cur_bssid(nic_info));
	if(wdn_info == NULL)
	{
		LOG_E("Can't find the corresponding wdn_info.");
	}
	
	pAssocInfo->AvailableRequestFixedIEs = 
			NDIS_802_11_AI_REQFI_CAPABILITIES |
            NDIS_802_11_AI_REQFI_LISTENINTERVAL |
            NDIS_802_11_AI_REQFI_CURRENTAPADDRESS;
	wf_memcpy(&pAssocInfo->RequestFixedIEs.Capabilities, (wf_u8*)&wdn_info->cap_info, 2);
	wf_memcpy(&pAssocInfo->RequestFixedIEs.ListenInterval, (wf_u8*)&wdn_info->listen_interval, 2);
	wf_memcpy(pAssocInfo->RequestFixedIEs.CurrentAPAddress, wf_wlan_get_cur_bssid(nic_info), MAC_ADDR_LEN);

	prGlueInfo->aucAssocReqIE_Len = pcur_network->assoc_req.ie_len;
	pAssocInfo->RequestIELength = prGlueInfo->aucAssocReqIE_Len;
	wf_memcpy(prGlueInfo->aucAssocReqIE, pcur_network->assoc_req.ie, prGlueInfo->aucAssocRespIE_Len);
}

VOID
wf_reset_assoc_info(PADAPTER pAdapter)
{
	P_GLUE_INFO_T prGlueInfo = pAdapter->parent;
	
    /* Initialize the structure used to query and set
       OID_802_11_ASSOCIATION_INFORMATION. */
   	wf_memset(&prGlueInfo->rNdisAssocInfo, 0, sizeof(NDIS_802_11_ASSOCIATION_INFORMATION));
	
    prGlueInfo->rNdisAssocInfo.Length =
        sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);
    prGlueInfo->rNdisAssocInfo.OffsetRequestIEs =
        sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);
    prGlueInfo->rNdisAssocInfo.OffsetResponseIEs =
        sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);

	return;
}

OS_SYSTIME
wf_get_time_tick (
    VOID
    )
{
    ULONG u4SystemUpTime;

    NdisGetSystemUpTime(&u4SystemUpTime);

    return (OS_SYSTIME)u4SystemUpTime;
} /* kalGetTimeTick */

wf_bool wf_check_ssid_valid(PADAPTER prAdapter, P_PARAM_SSID_T pDot11SSID)
{
	wf_bool findFlag;
	wf_mib_info_t* mib_info = prAdapter->mib_info;
	wf_wlan_mgmt_scan_que_node_t *pscanned_info;
	wf_u32 ssidIndex = 0;
	findFlag = wf_false;
	for(ssidIndex; ssidIndex < mib_info->bss_cnt; ssidIndex++)
	{
		pscanned_info = (wf_wlan_mgmt_scan_que_node_t*) &mib_info->bss_node[ssidIndex];
		if(EQUAL_SSID(pscanned_info->ssid.data,
                    pscanned_info->ssid.length,
                    pDot11SSID->aucSsid,
                    pDot11SSID->u4SsidLen))
		{
			findFlag = wf_true;
			break;
		}
	}
	return findFlag;
}

VOID
wf_reset_pkt_statistics_info(PADAPTER prAdapter)
{
	wf_mib_info_t *mib_info = prAdapter->mib_info;
	mib_info->num_recv_error.QuadPart = 0;
	mib_info->num_recv_ok.QuadPart = 0;
	mib_info->num_xmit_error.QuadPart = 0;
	mib_info->num_xmit_ok.QuadPart = 0;
}

NDIS_STATUS wf_oids_exception_timer_create(void *adapter)
{
	PADAPTER 			padapter = adapter;
	wf_mib_info_t 		*mib_info = padapter->mib_info;
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
                             &(mib_info->exception_timer)     // Output handle
                             );
	
	if (!NT_SUCCESS(ntStatus)) {
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
		LOG_D("create failed");
        return ndisStatus;
    }

	LOG_D("create success");

	timer_ctx = wf_get_timer_context(mib_info->exception_timer);
    timer_ctx->padapter = padapter;
	//timer_ctx->msg_type = 0;

	//WdfTimerStart(mib_info->exception_timer, WDF_REL_TIMEOUT_IN_MS(1000));

	return ndisStatus;
}

void wf_oids_exception_handle(WDFTIMER WdfTimer)
{
	wf_oids_timer_ctx_t *timer_ctx = wf_get_timer_context(WdfTimer);
	PADAPTER padapter = timer_ctx->padapter;
	wf_mib_info_t 		*mib_info;
	nic_info_st *pnic_info;
	mlme_state_e state;
	NDIS_OID	cur_oid;
//	timer_ctx = wf_get_timer_context(WdfTimer);
//	padapter = timer_ctx->padapter;

	if(padapter == NULL) {
		return;
	}

	mib_info = padapter->mib_info;
	pnic_info = padapter->nic_info;
	cur_oid = padapter->oid;
	if(cur_oid == NULL)
	{
		LOG_E("Error! Can't find the pended OID.");
		return;
	}

	if(MP_TEST_STATUS_FLAG(padapter, MP_ADAPTER_SURPRISE_REMOVED)) {
		return;
	}
	MP_SET_STATUS_FLAG(padapter, MP_ADAPTER_EXCEPTION_HANDLE);

	//if system is in deinit device process, the exception timer do nothing 
	if(mib_info == NULL || pnic_info == NULL ||  WF_CANNOT_RUN(pnic_info)) {
		LOG_D("the param is null! exit exception timer proc");
        return;
    }

	do {
		if(cur_oid == OID_802_11_SSID || cur_oid == OID_802_11_BSSID)
		{
			wf_mlme_get_state(pnic_info, &state);
			if(state == MLME_STATE_ASSOC || state == MLME_STATE_AUTH) {
				LOG_D("mlme is still execed");
				WdfTimerStart(mib_info->exception_timer, WDF_REL_TIMEOUT_IN_MS(30000));
				return;
			}
			else {
				LOG_D("submit assoc failed!");
				wf_save_assoc_ssid(padapter, wf_false);
				wf_submit_assoc_complete(padapter, DOT11_ASSOC_STATUS_FAILURE);
				wf_submit_connect_complete(padapter, DOT11_CONNECTION_STATUS_FAILURE);
				Mp11CompletePendedRequest(padapter, NDIS_STATUS_FAILURE);
			}
			break;
		}
		// To add more pended OID.
	}while(0);

	MP_CLEAR_STATUS_FLAG(padapter, MP_ADAPTER_EXCEPTION_HANDLE);
	padapter->oid = NULL;
}

void wf_save_assoc_ssid(PADAPTER padapter, wf_bool type)
{
	wf_mib_info_t *mib_info = padapter->mib_info;
	wf_ap_info_t *ap_info = padapter->ap_info;
	nic_info_st *pnic_info = padapter->nic_info;
	wf_file *file;
	int pos, i, set_pos;
	wf_u8 try_cnt = 0;
	wf_cache_info_t cache_info = {0};
	NDIS_STATUS ret;

	if(mib_info == NULL || ap_info == NULL || pnic_info ==NULL) {
		LOG_E("param is NULL");
		return;
	}

	#if 0
	if(!mib_info->hidden_network_enable) {
		LOG_D("hidden network is not enable");
		return;
	}
	#endif
 
	if(KeGetCurrentIrql() != PASSIVE_LEVEL) {
		LOG_E("irql is too high %d", KeGetCurrentIrql());
		return;
	}

	try_cnt = 0;
	while(try_cnt < 3) {
		try_cnt++;
		wf_msleep(100);
	
		file = wf_os_api_file_open(WF_CACHE_FILE);
		if(file == NULL) {
			LOG_E("open file failed!");
			continue;
		}

		pos = 0;
		set_pos = 0;
		for(i = 0; i < MP_MAX_DEVICE_NUM; i++) {
			ret = wf_os_api_file_read(file, pos, (unsigned char *)&cache_info, sizeof(cache_info));
			if(ret != NDIS_STATUS_SUCCESS) {
				break;
			}

			if(cache_info.ap_valid != TRUE && set_pos == 0) {
				set_pos = pos;
			}
			
			if(wf_memcmp(cache_info.dev_mac, nic_to_local_addr(padapter->nic_info), 6) == 0) {
				set_pos = pos;
				break;
			}
			
			pos += sizeof(cache_info);
		}

		LOG_D("------->>>start write assoc info");

		//no matter is find device info or not, we always need modify device info
		wf_memcpy(cache_info.dev_mac, padapter->PermanentAddress, 6);
		wf_memcpy(&cache_info.ap_ssid, wf_wlan_get_cur_ssid(pnic_info), sizeof(PARAM_SSID_T));
		wf_memcpy(cache_info.ap_mac, wf_wlan_get_cur_bssid(pnic_info), 6);
		cache_info.ap_valid = wf_true;
	
		ret = wf_os_api_file_write(file, set_pos, (unsigned char *)&cache_info, sizeof(cache_info));
		wf_os_api_file_close(file);
		if(ret == NDIS_STATUS_SUCCESS) {
			break;
		}
	}
}

int wf_get_assoc_ssid(PADAPTER padapter, wf_wlan_ssid_t *ssid)
{
	wf_file *file;
	int pos, i;
	wf_u8 try_cnt = 0;
	wf_cache_info_t cache_info = {0};
	NDIS_STATUS ret;
	wf_u8 find_flag = FALSE;

	//return find_flag;

	if(KeGetCurrentIrql() != PASSIVE_LEVEL) {
		LOG_E("irql is too high");
		return find_flag;
	}

	LOG_D("start parser file");

	try_cnt = 0;
	while(try_cnt < 3) {
		try_cnt++;
		wf_msleep(100);
	
		file = wf_os_api_file_open(WF_CACHE_FILE);
		if(file == NULL) {
			LOG_E("open file failed!");
			continue;
		}

		pos = 0;
		for(i=0; i<MP_MAX_DEVICE_NUM; i++) {
			ret = wf_os_api_file_read(file, pos, (unsigned char *)&cache_info, sizeof(cache_info));
			if(ret != NDIS_STATUS_SUCCESS ) {
				//LOG_D("read info failed!");
				break;
			}

			if(wf_memcmp(cache_info.dev_mac, padapter->PermanentAddress, 6) == 0) {
				ssid->length = cache_info.ap_ssid.u4SsidLen;
				if(ssid->length <= WF_80211_MAX_SSID_LEN) {
					wf_memcpy(ssid->data, cache_info.ap_ssid.aucSsid, ssid->length);
				}
				find_flag = TRUE;
				break;
			}
			
			pos += sizeof(cache_info);
		}

		wf_os_api_file_close(file);
		if(find_flag) {
			break;
		}
	}

	return find_flag;
}

NDIS_STATUS wf_set_scan_hidden_network(PADAPTER padapter, P_PARAM_SSID_T pDot11SSID)
{
	nic_info_st *pnic_info = padapter->nic_info;
	wf_mib_info_t *mib_info = padapter->mib_info;
	wf_bool is_connected, is_busy;
	wf_wlan_ssid_t ssid = {0};
	int ret;
#ifdef CONFIG_CONCURRENT_MODE
	nic_info_st *pvir_nic = pnic_info->vir_nic;
	mlme_state_e state;
#endif
	LARGE_INTEGER timeout = { 0 };

	timeout.QuadPart = DELAY_ONE_MILLISECOND;
	timeout.QuadPart *= 15000;

	pnic_info->is_up = 1;

#ifdef CONFIG_CONCURRENT_MODE
	wf_mlme_get_state(pvir_nic, &state);
	if(state <= MLME_STATE_ASSOC) {
		LOG_D("another nic is scanning");
		wf_os_api_ind_scan_done(pnic_info, wf_true);
		return NDIS_STATUS_FAILURE;
	}
#endif

	if(pnic_info == NULL || mib_info == NULL) {
		LOG_E("param is NULL");
		return NDIS_STATUS_FAILURE;
	}

	if(pDot11SSID == NULL) {
		LOG_E("param pDot11SSID is NULL");
		return NDIS_STATUS_FAILURE;
	} else {
		ssid.length = pDot11SSID->u4SsidLen;
		if(ssid.length <= WF_80211_MAX_SSID_LEN) {
			wf_memcpy(ssid.data, pDot11SSID->aucSsid , ssid.length);
		} else {
			LOG_D("ssid length is out of range!! len=%d", ssid.length);
			return NDIS_STATUS_FAILURE;
		}
	}

	KeClearEvent(&mib_info->scan_hidden_finish);
	wf_mlme_get_connect(pnic_info, &is_connected);
	if (is_connected) {
        wf_mlme_get_traffic_busy(pnic_info, &is_busy);
        if (is_busy) {
            wf_os_api_ind_scan_done(pnic_info, wf_true, 0);

            return 0;
        }
        ret = wf_mlme_scan_start(pnic_info, SCAN_TYPE_PASSIVE,
                           &ssid, 1, NULL, 0, WF_MLME_FRAMEWORK_NDIS);
    } else {
        ret = wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                           &ssid, 1, NULL, 0, WF_MLME_FRAMEWORK_NDIS);
    }

	if(ret != 0) {
		LOG_D("[%s-%d] scan start failed! ret=%d\n", __FUNCTION__, __LINE__, ret);
		return NDIS_STATUS_FAILURE;
	}

	if(KeWaitForSingleObject(&mib_info->scan_hidden_finish, Executive, KernelMode, TRUE, &timeout) != STATUS_SUCCESS) {
		LOG_E("wait scan hidden network timeout!");
		return NDIS_STATUS_FAILURE;
	}

	return NDIS_STATUS_SUCCESS;

}

