
#ifndef __WF_WLAN_DEV_H__
#define __WF_WLAN_DEV_H__

#ifdef __cplusplus
extern "C" {
#endif
  
#include "common.h"

typedef struct rt_wlan_priv {
  wf_u8 hw_addr[WF_ETH_ALEN];
  nic_info_st *nic;
  wf_workqueue_mgnt_st tx_wq;
}rt_wlan_priv_st;

int rt_wlan_register (nic_info_st *pnic_info, const char *name);
int rt_wlan_shutdown(nic_info_st *pnic_info);
int rt_wlan_unregister (nic_info_st *pnic_info);
int wf_wlan_dev_report_eapol(struct rt_wlan_device *wlan, void *buffer, wf_u16 len);

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif      // __WF_WLAN_DEV_H__


