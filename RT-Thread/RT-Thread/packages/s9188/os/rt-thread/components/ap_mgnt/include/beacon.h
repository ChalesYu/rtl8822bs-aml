/*
 * beacon.h
 *
 * ap beacon handle
 *
 * Author: songqiang
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __BEACON_H__
#define __BEACON_H__

struct ap_beacon_ie{
  wf_u16 beacon_int;
  wf_u16 capab_info;
  wf_u8 variable[];
};

int ieee802_11_build_ap_params(nic_info_st *pnic_info,
			       char *ssid, int ssid_len, wf_u8 *ie);

#endif

