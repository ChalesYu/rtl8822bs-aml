#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"
#include "pcomp.h"


extern NDIS_HANDLE NdisMiniportDriverHandle;

void *wf_malloc(wf_u32 size)
{
#if 1
	void *ptr = NULL;

	if(NdisAllocateMemoryWithTag(&ptr, size, MP_MEMORY_TAG) != NDIS_STATUS_SUCCESS) {
		return NULL;
	}

	if(ptr == NULL) {
		LOG_E("malloc memory failed! size=%d", size);
	} else {
		NdisZeroMemory(ptr, size);
	}

	return ptr;
#else
	return ExAllocatePoolWithTagPriority(NonPagedPool, size, MP_MEMORY_TAG, NormalPoolPriority);
#endif
}

void wf_free(void *ptr)
{
	if(ptr) {
		NdisFreeMemory(ptr, 0, 0);
		ptr = NULL;
	}
}


void wf_Msleep(wf_u32 MilliSecond)
{
	LARGE_INTEGER Interval = { 0 };
	Interval.QuadPart = DELAY_ONE_MILLISECOND;
	Interval.QuadPart *= MilliSecond;
	KeDelayExecutionThread(KernelMode, 0, &Interval);
}

void wf_Usleep(wf_u32 MilliSecond)
{
	LARGE_INTEGER Interval = { 0 };
	Interval.QuadPart = DELAY_ONE_MICROSECOND;
	Interval.QuadPart *= MilliSecond;
	KeDelayExecutionThread(KernelMode, 0, &Interval);
}


wf_u32 wf_os_api_rand32(void)
{
	LARGE_INTEGER time = { 0 };
	static wf_u32 local_cnt = 0;
	wf_u32 rand_val;

	//tick conut is update per 10 ms, so we add a local bias cnt
	KeQueryTickCount(&time);
	local_cnt++;
	rand_val = (wf_u32)time.QuadPart;
	rand_val += local_cnt;

	return rand_val;
}

void wf_os_api_ind_disconnect(void *arg, wf_u8 arg1)
{
	nic_info_st *pnic_info = (nic_info_st *)arg;
	PADAPTER padapter = (PADAPTER) pnic_info->hif_node;
	wf_mib_info_t *mib_info;
	ULONG reason_code, status_code;
	PNDIS_OID_REQUEST   request;

	UNREFERENCED_PARAMETER(arg1);

	if(pnic_info == NULL || pnic_info->hif_node == NULL) {
		LOG_E("nic or padapter is NULL");
		return;
	}

	padapter = pnic_info->hif_node;

	if(padapter->mib_info == NULL) {
		LOG_E("mib is NULL");
		return;
	}
	
	mib_info = padapter->mib_info;
	//connect state is false, means the assoc complete is not submit
	if(mib_info->connect_state == FALSE) {
		if(padapter->PendedRequest != NULL){
			request = padapter->PendedRequest;
			if(request->RequestType == NdisRequestSetInformation &&
				request->DATA.SET_INFORMATION.Oid == OID_DOT11_CONNECT_REQUEST) {
				status_code = WF_MLME_INFO_STATUS_CODE(pnic_info);
				wf_save_assoc_ssid(padapter, FALSE);
				wf_submit_assoc_complete(padapter, DOT11_ASSOC_STATUS_FAILURE);
				//DOT11_CONNECTION_STATUS_SUCCESS
				wf_submit_connect_complete(padapter, DOT11_CONNECTION_STATUS_FAILURE);
				Mp11CompletePendedRequest(padapter, NDIS_STATUS_SUCCESS);
			}
		}
	} else {
		reason_code = WF_MLME_INFO_REASON_CODE(pnic_info);

		switch(reason_code) {
		case WF_80211_REASON_4WAY_HANDSHAKE_TIMEOUT:
		case WF_80211_REASON_MIC_FAILURE:
		case WF_80211_REASON_GROUP_KEY_HANDSHAKE_TIMEOUT:
		case WF_80211_REASON_INVALID_GROUP_CIPHER:
		case WF_80211_REASON_INVALID_PAIRWISE_CIPHER:
		case WF_80211_REASON_IEEE8021X_FAILED:
		case WF_80211_REASON_CIPHER_SUITE_REJECTED:
			reason_code |= DOT11_ASSOC_STATUS_PEER_DEAUTHENTICATED;
			break;
		case WF_80211_REASON_DISASSOC_DUE_TO_INACTIVITY:
			reason_code |= DOT11_ASSOC_STATUS_PEER_DISASSOCIATED;
			break;
		}
		
		wf_submit_disassoc_complete(padapter, reason_code);
		mib_info->connect_state = FALSE;
		KeSetEvent(&mib_info->halt_deauth_finish, 0, FALSE);
	}

	return;
}

void wf_os_api_enable_all_data_queue(void *arg)
{
	UNREFERENCED_PARAMETER(arg);
#if 0
    netif_carrier_on(ndev);

    #if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,35))
        netif_tx_start_all_queues(ndev);
    #else
        netif_start_queue(ndev);
    #endif
	#endif
}

void wf_os_api_disable_all_data_queue(void *arg)
{
	UNREFERENCED_PARAMETER(arg);
#if 0
    #if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,35))
        netif_tx_stop_all_queues(ndev);
    #else
        netif_stop_queue(ndev);
    #endif

    netif_carrier_off(ndev);
	#endif
}


void wf_os_api_ind_scan_done(void *arg,wf_bool arg1, wf_u8 arg2)
{
	nic_info_st *pnic_info = arg;
	PADAPTER padapter = pnic_info->hif_node;
	//wf_mib_info_t *mib_info = padapter->mib_info;

	UNREFERENCED_PARAMETER(arg1);
	UNREFERENCED_PARAMETER(arg2);
	
	wf_submit_scan_complete(padapter);

}

void wf_os_api_ind_connect(void *arg, wf_u8 arg1)
{
	nic_info_st *pnic_info = arg;
	PADAPTER padapter = NULL;
	wdn_net_info_st *pwdn_info;
	wf_mib_info_t *mib_info = NULL;

	if(pnic_info == NULL || pnic_info->hif_node == NULL) {
		LOG_E("nic or padapter is NULL");
		return;
	}

	padapter = pnic_info->hif_node;

	if(padapter->mib_info == NULL) {
		LOG_E("mib is NULL");
		return;
	}
	
	mib_info = padapter->mib_info;

	UNREFERENCED_PARAMETER(arg1);
	LOG_D("Indicate connection seccuss.");
	if(padapter->PendedRequest != NULL) {
		wf_submit_assoc_complete(padapter, DOT11_ASSOC_STATUS_SUCCESS);
		wf_submit_connect_complete(padapter, DOT11_CONNECTION_STATUS_SUCCESS);
		Mp11CompletePendedRequest(padapter, NDIS_STATUS_SUCCESS);

		mib_info->connect_state = TRUE;

		pwdn_info = wf_wdn_find_info(pnic_info, wf_wlan_get_cur_bssid(pnic_info));
		if(pwdn_info != NULL) {
			wf_submit_link_qual(padapter);
			wf_submit_link_speed(padapter, wf_get_speed_by_raid(pwdn_info->raid));
		} else {
			LOG_E("pwdn is NULL");
		}
	}
}

#ifdef CFG_ENABLE_AP_MODE
void wf_os_api_ap_ind_assoc(void *arg, void *arg1, void *arg2)
{

}
void wf_os_api_ap_ind_disassoc(void *arg, void *arg1)
{

}
#endif

void *wf_os_api_get_ars(void *nic_info)
{
    nic_info_st *nic_node = nic_info;
    ADAPTER *padt = nic_node->hif_node;
    return padt->ars;
}
void  wf_os_api_set_ars(void *nic_info, void *ars)
{
    nic_info_st *nic_node = nic_info;
    ADAPTER *padt = nic_node->hif_node;
    padt->ars = ars;
}
void *wf_os_api_get_odm(void *nic_info)
{
    nic_info_st *nic_node = nic_info;
    ADAPTER *padt = nic_node->hif_node;
    return padt->odm;
}
void  wf_os_api_set_odm(void *nic_info,void *odm)
{
    nic_info_st *nic_node = nic_info;
    ADAPTER *padt = nic_node->hif_node;
    padt->odm           = odm;
}

wf_s32 wf_os_api_get_cpu_id(void)
{
	return KeGetCurrentProcessorNumber();
}

