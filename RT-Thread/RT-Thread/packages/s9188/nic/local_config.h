/*
 * local_config.h
 *
 * used for local information
 *
 * Author: songqiang
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __LOCAL_CFG_H__
#define __LOCAL_CFG_H__


typedef enum 
{
	WF_AUTO_MODE,
	WF_ADHOC_MODE,
	WF_INFRA_MODE,
	WF_MASTER_MODE,
	WF_REPEAT_MODE,
	WF_SECOND_MODES,
	WF_MONITOR_MODE,
	WF_MESH_MODE,
}sys_work_mode_e;


typedef struct  
{
    sys_work_mode_e    	work_mode;
    wf_u8         		channel;
    CHANNEL_WIDTH  		bw;
    wf_bool         	adhoc_master;
    wf_u8               ssid[32];
    wf_u8               channel_plan;
    wf_u8               ba_enable;
#ifdef CONFIG_ARS_DRIVER_SUPPORT
    wf_u8               adaptivity_en;
    wf_u8               antenna_diversity;
    wf_u8               beamforming_support;
    wf_u8               force_igi;
    wf_u8               adaptivity_mode;
    wf_u8               adaptivity_dc_backoff;
    wf_u8               adaptivity_dml;
    wf_s8               adaptivity_th_l2h_ini;
    wf_s8               adaptivity_th_edcca_hl_diff;
#endif
    wf_u8               scan_ch_to;
    wf_u8               scan_prb_times;
    wf_u8               scan_que_deep;
    wf_u8               scan_que_node_ttl;
}local_info_st;

#define NIC_INFO_2_WORK_MODE(nic) ((local_info_st *)nic->local_info)->work_mode

int wf_local_cfg_init(nic_info_st *nic_info);
int wf_local_cfg_term(nic_info_st *nic_info);
int wf_local_cfg_set_default(nic_info_st *nic_info);
int wf_local_cfg_get_default(nic_info_st *nic_info);
sys_work_mode_e wf_local_cfg_get_work_mode (nic_info_st *pnic_info);
void wf_local_cfg_set_work_mode (nic_info_st *pnic_info, sys_work_mode_e mode);
wf_s32 wf_rx_config_agg(nic_info_st *nic_info);


#endif
