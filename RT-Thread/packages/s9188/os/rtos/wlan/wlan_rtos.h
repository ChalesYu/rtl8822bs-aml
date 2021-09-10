
#ifndef __WLAN_RTOS_H__
#define __WLAN_RTOS_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    nic_info_st *nic;
    wf_workqueue_mgnt_st tx_wq;
} wlan_priv_t;


OS_RET wlan_register (nic_info_st *pnic_info, const char *name);
OS_RET wlan_unregister(nic_info_st *pnic_info);

#ifdef __cplusplus
}
#endif

#endif

