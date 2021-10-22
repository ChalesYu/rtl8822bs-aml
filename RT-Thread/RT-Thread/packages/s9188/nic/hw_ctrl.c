/*
 * hw_ctrl.c
 *
 * used for M0 init
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

int wf_hw_mcu_disable(nic_info_st *nic_info)
{
    int ret;
    
    wf_u8 value8 = 0;

    value8 = wf_io_read8(nic_info,0x94,NULL);
    ret = wf_io_write8(nic_info,0x94,value8&0x18);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

int wf_hw_mcu_enable(nic_info_st *nic_info)
{
    int ret;

    wf_u8 value8 = 0;

    value8 = wf_io_read8(nic_info,0x94,NULL);
    ret = wf_io_write8(nic_info,0x94,value8|0x6);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}


int wf_hw_mcu_startup(nic_info_st *nic_info)
{
    int ret;

    wf_u8 value8 = 0;

    value8 = wf_io_read8(nic_info,0x94,NULL);
    ret = wf_io_write8(nic_info,0x94,value8|0x1);
    if (WF_RETURN_FAIL == ret)
    {
    	LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}


