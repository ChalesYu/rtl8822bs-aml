/*
 * local_config.c
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
#include "common.h"
#include "wf_debug.h"


static local_info_st default_cfg[] =
{
    {
        .work_mode  =WF_AUTO_MODE,
        .channel    = 1,
        .bw         = CHANNEL_WIDTH_20,
        .adhoc_master   = wf_false,
        .ssid        = "SCI-AUTO",
        .channel_plan = WF_CHPLAN_CHINA,
        .ba_enable = 1,
#ifdef CONFIG_ARS_DRIVER_SUPPORT
        0,                  //adaptivity_en
        0,                  //antenna_diversity
        0,                  //beamforming_support
        0,                  //force_igi
        0,                  //adaptivity_mode
        0,                  //adaptivity_dc_backoff
        0,                  //adaptivity_dml
        0,                  //adaptivity_th_l2h_ini
        0,                  //adaptivity_th_edcca_hl_diff
#endif
        .scan_ch_to = 50,
        .scan_prb_times = 3,
        .scan_que_deep = 64,
        .scan_que_node_ttl = 10,
    },
    {
        .work_mode  =WF_ADHOC_MODE,
        .channel    = 1,
        .bw         = CHANNEL_WIDTH_20,
        .adhoc_master   = wf_false,
        .ssid        = "SCI-ADHOC",
        .channel_plan = WF_CHPLAN_CHINA,
        .ba_enable = 1,
#ifdef CONFIG_ARS_DRIVER_SUPPORT
        0,                  //adaptivity_en
        0,                  //antenna_diversity
        0,                  //beamforming_support
        0,                  //force_igi
        0,                  //adaptivity_mode
        0,                  //adaptivity_dc_backoff
        0,                  //adaptivity_dml
        0,                  //adaptivity_th_l2h_ini
        0,                  //adaptivity_th_edcca_hl_diff
#endif
        .scan_ch_to = 50,
        .scan_prb_times = 3,
        .scan_que_deep = 64,
        .scan_que_node_ttl = 10,
    },
    {
        .work_mode  =WF_INFRA_MODE,
        .channel    = 1,
        .bw         = CHANNEL_WIDTH_20,
        .adhoc_master   = wf_false,
        .ssid        = "SCI-STA",
        .channel_plan = WF_CHPLAN_CHINA,
        .ba_enable = 1,
#ifdef CONFIG_ARS_DRIVER_SUPPORT
        0,                  //adaptivity_en
        0,                  //antenna_diversity
        0,                  //beamforming_support
        0,                  //force_igi
        0,                  //adaptivity_mode
        0,                  //adaptivity_dc_backoff
        0,                  //adaptivity_dml
        0,                  //adaptivity_th_l2h_ini
        0,                  //adaptivity_th_edcca_hl_diff
#endif
        .scan_ch_to = 50,
        .scan_prb_times = 3,
        .scan_que_deep = 64,
        .scan_que_node_ttl = 10,
    },
    {
        .work_mode  =WF_MASTER_MODE,
        .channel    = 1,
        .bw         = CHANNEL_WIDTH_20,
        .adhoc_master   = wf_false,
        .ssid        = "SCI-AP",
        .channel_plan = WF_CHPLAN_CHINA,
        .ba_enable = 1,
#ifdef CONFIG_ARS_DRIVER_SUPPORT
        0,                  //adaptivity_en
        0,                  //antenna_diversity
        0,                  //beamforming_support
        0,                  //force_igi
        0,                  //adaptivity_mode
        0,                  //adaptivity_dc_backoff
        0,                  //adaptivity_dml
        0,                  //adaptivity_th_l2h_ini
        0,                  //adaptivity_th_edcca_hl_diff
#endif
        .scan_ch_to = 50,
        .scan_prb_times = 3,
        .scan_que_deep = 64,
        .scan_que_node_ttl = 10,
    },
    {
        .work_mode  =WF_REPEAT_MODE,
        .channel    = 1,
        .bw         = CHANNEL_WIDTH_20,
        .adhoc_master   = wf_false,
        .ssid        = "SCI-REPEAT",
        .channel_plan = WF_CHPLAN_CHINA,
        .ba_enable = 1,
#ifdef CONFIG_ARS_DRIVER_SUPPORT
        0,                  //adaptivity_en
        0,                  //antenna_diversity
        0,                  //beamforming_support
        0,                  //force_igi
        0,                  //adaptivity_mode
        0,                  //adaptivity_dc_backoff
        0,                  //adaptivity_dml
        0,                  //adaptivity_th_l2h_ini
        0,                  //adaptivity_th_edcca_hl_diff
#endif
        .scan_ch_to = 50,
        .scan_prb_times = 3,
        .scan_que_deep = 64,
        .scan_que_node_ttl = 10,
    },
    {
        .work_mode  =WF_SECOND_MODES,
        .channel    = 1,
        .bw         = CHANNEL_WIDTH_20,
        .adhoc_master   = wf_false,
        .ssid        = "SCI-SECOND",
        .channel_plan = WF_CHPLAN_CHINA,
        .ba_enable = 1,
#ifdef CONFIG_ARS_DRIVER_SUPPORT
        0,                  //adaptivity_en
        0,                  //antenna_diversity
        0,                  //beamforming_support
        0,                  //force_igi
        0,                  //adaptivity_mode
        0,                  //adaptivity_dc_backoff
        0,                  //adaptivity_dml
        0,                  //adaptivity_th_l2h_ini
        0,                  //adaptivity_th_edcca_hl_diff
#endif
        .scan_ch_to = 50,
        .scan_prb_times = 3,
        .scan_que_deep = 64,
        .scan_que_node_ttl = 10,
    },
    {
        .work_mode  =WF_MONITOR_MODE,
        .channel    = 1,
        .bw         = CHANNEL_WIDTH_20,
        .adhoc_master   = wf_false,
        .ssid        = "SCI-NONITOR",
        .channel_plan = WF_CHPLAN_CHINA,
        .ba_enable = 1,
#ifdef CONFIG_ARS_DRIVER_SUPPORT
        0,                  //adaptivity_en
        0,                  //antenna_diversity
        0,                  //beamforming_support
        0,                  //force_igi
        0,                  //adaptivity_mode
        0,                  //adaptivity_dc_backoff
        0,                  //adaptivity_dml
        0,                  //adaptivity_th_l2h_ini
        0,                  //adaptivity_th_edcca_hl_diff
#endif
        .scan_ch_to = 50,
        .scan_prb_times = 3,
        .scan_que_deep = 64,
        .scan_que_node_ttl = 10,
    },
    {
        .work_mode  =WF_MESH_MODE,
        .channel    = 1,
        .bw         = CHANNEL_WIDTH_20,
        .adhoc_master   = wf_false,
        .ssid        = "SCI-MESH",
        .channel_plan = WF_CHPLAN_CHINA,
        .ba_enable = 1,
#ifdef CONFIG_ARS_DRIVER_SUPPORT
        0,                  //adaptivity_en
        0,                  //antenna_diversity
        0,                  //beamforming_support
        0,                  //force_igi
        0,                  //adaptivity_mode
        0,                  //adaptivity_dc_backoff
        0,                  //adaptivity_dml
        0,                  //adaptivity_th_l2h_ini
        0,                  //adaptivity_th_edcca_hl_diff
#endif
        .scan_ch_to = 50,
        .scan_prb_times = 3,
        .scan_que_deep = 64,
        .scan_que_node_ttl = 10,
    },

};

int wf_local_cfg_init(nic_info_st *nic_info)
{
    nic_info->local_info = (local_info_st *)wf_kzalloc(sizeof(local_info_st));
    if (nic_info->local_info == NULL)
    {
        return -1;
    }
    wf_memcpy(nic_info->local_info, &default_cfg[WF_INFRA_MODE], sizeof(local_info_st));

    return 0;
}

int wf_local_cfg_term(nic_info_st *nic_info)
{
    if (nic_info->local_info != NULL)
    {
        wf_kfree(nic_info->local_info);
    }

    return 0;
}


int wf_local_cfg_get_default(nic_info_st *nic_info)
{
    local_info_st *local_info = nic_info->local_info;
    hw_info_st *hw_info = nic_info->hw_info;

    if (nic_info->nic_cfg_file_read != NULL)
    {
        if (nic_info->nic_cfg_file_read((void *)nic_info) == 0)
        {
            hw_info->channel_plan = local_info->channel_plan;
            hw_info->ba_enable = local_info->ba_enable;
        }
    }

    /* set channel plan */
    channel_init(nic_info);

    return 0;
}

wf_s32 wf_rx_config_agg(nic_info_st *nic_info)
{
    wf_s32 ret = 0;
    ret = wf_mcu_set_agg_param(nic_info, 0x8, 0x5, 1);
    if (ret != WF_RETURN_OK)
    {
        return WF_RETURN_FAIL;
    }

    return 0;
}

int wf_local_cfg_set_default(nic_info_st *nic_info)
{
    local_info_st *local_info = nic_info->local_info;
    int ret = 0;

    LOG_D("[LOCAL_CFG] work_mode: %d",local_info->work_mode);
    LOG_D("[LOCAL_CFG] channel: %d",local_info->channel);
    LOG_D("[LOCAL_CFG] bw: %d",local_info->bw);
    LOG_D("[LOCAL_CFG] ssid: %s",local_info->ssid);

    ret = wf_hw_info_set_channnel_bw(nic_info,local_info->channel,local_info->bw, HAL_PRIME_CHNL_OFFSET_DONT_CARE);
    if (ret != WF_RETURN_OK)
    {
        return WF_RETURN_FAIL;
    }
    ret = wf_mcu_handle_rf_iq_calibrate(nic_info, local_info->channel);
    if (ret != WF_RETURN_OK)
    {
        return WF_RETURN_FAIL;
    }

    ret = wf_mcu_update_thermal(nic_info);
    if (ret != WF_RETURN_OK)
    {
        return WF_RETURN_FAIL;
    }

    ret = wf_mcu_set_op_mode(nic_info,local_info->work_mode);
    if (ret != WF_RETURN_OK)
    {
        return WF_RETURN_FAIL;
    }

    wf_rx_config_agg(nic_info);


    return WF_RETURN_OK;
}


sys_work_mode_e wf_local_cfg_get_work_mode (nic_info_st *pnic_info)
{
    local_info_st *plocal = (local_info_st *)pnic_info->local_info;
    return plocal->work_mode;
}

void wf_local_cfg_set_work_mode (nic_info_st *pnic_info, sys_work_mode_e mode)
{
    if (NULL == pnic_info)
    {
        LOG_E("param is null");
        return;
    }

    if (WF_AUTO_MODE > mode || mode > WF_MESH_MODE)
    {
        LOG_E("[%s] mode(%d) is not support",__func__,mode);
        return;
    }

    pnic_info->local_info = &default_cfg[mode];
}


