/*
 * wf_os_api_mix.h
 *
 * used for .....
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#ifndef __WF_OS_API_MIX_H__
#define __WF_OS_API_MIX_H__

#define wf_le16_to_cpu            le16_to_cpu
#define wf_cpu_to_le16            cpu_to_le16
#define wf_be16_to_cpu            be16_to_cpu
#define wf_cpu_to_be16            cpu_to_be16

#define wf_le32_to_cpu            le32_to_cpu
#define wf_cpu_to_le32            cpu_to_le32
#define wf_be32_to_cpu            be32_to_cpu
#define wf_cpu_to_be32            cpu_to_be32

#define wf_le64_to_cpu            le64_to_cpu
#define wf_cpu_to_le64            cpu_to_le64
#define wf_be64_to_cpu            be64_to_cpu
#define wf_cpu_to_be64            cpu_to_be64

void wf_os_api_ind_scan_done (void *arg, wf_bool arg1, wf_u8 arg2);
void wf_os_api_ind_connect(void *arg, wf_u8 arg1);
void wf_os_api_ind_disconnect(void *arg, wf_u8 arg1);
#ifdef CFG_ENABLE_ADHOC_MODE
void wf_os_api_cfg80211_unlink_ibss(void *arg);
#endif
#ifdef CFG_ENABLE_AP_MODE
void wf_os_api_ap_ind_assoc(void *arg, void *arg1, void *arg2, wf_u8 arg3);
void wf_os_api_ap_ind_disassoc(void *arg, void *arg1, wf_u8 arg2);
#endif
void wf_os_api_enable_all_data_queue(void *arg);
void wf_os_api_disable_all_data_queue(void *arg);
wf_u32 wf_os_api_rand32 (void);

void *wf_os_api_get_ars(void *nic_info);
void  wf_os_api_set_ars(void *nic_info,void *ars);

void *wf_os_api_get_odm(void *nic_info);
void  wf_os_api_set_odm(void *nic_info,void *odm);



wf_s32 wf_os_api_get_cpu_id(void);
#endif

