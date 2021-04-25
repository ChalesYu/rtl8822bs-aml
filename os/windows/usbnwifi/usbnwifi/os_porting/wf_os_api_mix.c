#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"

#define DELAY_ONE_MICROSECOND   (-10)
#define DELAY_ONE_MILLISECOND   (DELAY_ONE_MICROSECOND*1000)
void *wf_malloc(wf_u32 size)
{
	return ExAllocatePool(NonPagedPool, size);
}

void wf_free(void *ptr)
{
	ExFreePool(ptr);
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

void wf_os_api_set_disconnect_flag(void *arg)
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
