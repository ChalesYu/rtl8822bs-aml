#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"
#include "common.h"
#include "pcomp.h"
#include "wf_oids_adapt.h"
#include "mp_intf.h"
#define DELAY_ONE_MICROSECOND   (-10)
#define DELAY_ONE_MILLISECOND   (DELAY_ONE_MICROSECOND*1000)
extern NDIS_STATUS wf_indicate_assoc_complete_status(padapter);
void *wf_malloc(wf_u32 size)
{
	return ExAllocatePool(NonPagedPool, size);
}

void wf_free(void *ptr)
{
	if(ptr) {
		ExFreePool(ptr);
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
	rand_val = time.QuadPart;
	rand_val += local_cnt;

	return rand_val;
}

void wf_os_api_set_scan_finish_flag(void *arg)
{
#if 0
    union iwreq_data wrqu;

    wf_memset(&wrqu, 0, sizeof(union iwreq_data));

    #ifndef CONFIG_IOCTL_CFG80211
    wireless_send_event(ndev, SIOCGIWSCAN, &wrqu, NULL);
    #endif
	#endif
}

void wf_os_api_set_connect_flag(void *arg, wf_u8 *bssid)
{
#if 0
    union iwreq_data wrqu;

    wf_memset(&wrqu, 0, sizeof(union iwreq_data));

    wrqu.ap_addr.sa_family = ARPHRD_ETHER;
    wf_memcpy(wrqu.ap_addr.sa_data, bssid, ETH_ALEN);

    #ifndef CONFIG_IOCTL_CFG80211
    wireless_send_event(ndev, SIOCGIWAP, &wrqu, NULL);
    #endif
	
	#endif
}

void wf_os_api_ind_disconnect(void *arg, wf_u8 arg1)
{
#if 0
    union iwreq_data wrqu;

    wf_memset(&wrqu, 0, sizeof(union iwreq_data));

    wrqu.ap_addr.sa_family = ARPHRD_ETHER;
    wf_memset(wrqu.ap_addr.sa_data, 0, ETH_ALEN);

    #ifndef CONFIG_IOCTL_CFG80211
    wireless_send_event(ndev, SIOCGIWAP, &wrqu, NULL);
    #endif
#endif
	UNREFERENCED_PARAMETER(arg1);
	nic_info_st *pnic_info = (nic_info_st *)arg;
	PADAPTER pAdapter = (PADAPTER) pnic_info->hif_node;
	wf_submit_disassoc_complete(pAdapter, DOT11_DISASSOC_REASON_OS);
	return;
}
void wf_os_api_enable_all_data_queue(void *arg)
{
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
#if 0
    #if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,35))
        netif_tx_stop_all_queues(ndev);
    #else
        netif_stop_queue(ndev);
    #endif

    netif_carrier_off(ndev);
	#endif
}

void wf_os_api_ie_record (wf_u8 **pbuf, wf_u32 *pbuf_len,
                                 void *pie, wf_u32 ie_len)
{
    
}

void wf_os_api_ind_scan_done(void *arg,wf_bool arg1, wf_u8 arg2)
{
	UNREFERENCED_PARAMETER(arg2);
	nic_info_st *pnic_info = arg;
	PADAPTER padapter = pnic_info->hif_node;
	wf_submit_scan_complete(padapter);
    if (padapter->MibInfo.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA ||
		padapter->MibInfo.AuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK)
    {
        wf_check_PMKID_candidate(padapter);
    }
}

void wf_os_api_ind_connect(void *arg, wf_u8 arg1)
{
	UNREFERENCED_PARAMETER(arg1);
	nic_info_st *pnic_info = arg;
	PADAPTER padapter = pnic_info->hif_node;
	wf_submit_assoc_complete(padapter, DOT11_ASSOC_STATUS_SUCCESS);
	wf_submit_connect_complete(padapter, DOT11_CONNECTION_STATUS_SUCCESS);
	Mp11CompletePendedRequest(padapter, NDIS_STATUS_SUCCESS);
	//wf_submit_link_qual(padapter);
}
#ifdef CFG_ENABLE_AP_MODE
void wf_os_api_ap_ind_assoc(void *arg, void *arg1, void *arg2)
{

}
void wf_os_api_ap_ind_disassoc(void *arg, void *arg1)
{

}
#endif

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
    return KeGetCurrentProcessorIndex();
}
