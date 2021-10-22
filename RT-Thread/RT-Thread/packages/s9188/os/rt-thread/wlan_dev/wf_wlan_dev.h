/*
 * wf_wlan_dev.h
 *
 * used for wlan dev ops.
 *
 * Author: songqiang
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __WF_WLAN_DEV_H__
#define __WF_WLAN_DEV_H__

#ifdef __cplusplus
extern "C" {
#endif
  
#include "common.h"

typedef struct rt_wlan_priv {
  wf_u8 hw_addr[WF_ETH_ALEN];
  nic_info_st *nic;
  wf_workqueue_mgnt_st wlan_tx_wq;
}rt_wlan_priv_st;

typedef enum { 
	SEC_MODE_NONE , 
	SEC_MODE_CCMP ,
	SEC_MODE_TKIP
}rt_sec_mode;

int rt_wlan_register (nic_info_st *pnic_info, const char *name);
int rt_wlan_shutdown(nic_info_st *pnic_info);
int rt_wlan_unregister (nic_info_st *pnic_info);
int wf_wlan_dev_report_eapol(struct rt_wlan_device *wlan, void *buffer, wf_u16 len);

#ifdef __cplusplus
    }
#endif

#endif


