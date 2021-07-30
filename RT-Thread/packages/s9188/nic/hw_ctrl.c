/*
 * hw_ctrl.c
 *
 * used for M0 init
 *
 * Author: songqiang
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
#include "common.h"
#include "wf_debug.h"


int wf_hw_mcu_disable(nic_info_st *nic_info)
{
    int ret;
    
#ifdef CONFIG_RICHV200
    wf_u8 value8 = 0;

    value8 = wf_io_read8(nic_info,0x94,NULL);
    ret = wf_io_write8(nic_info,0x94,value8&0x18);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }
#else
    ret = wf_io_write32(nic_info,0xe4,0);
    if (WF_RETURN_FAIL == ret )
    {
    	LOG_E("[%s] WF_EN_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

	wf_io_read32(nic_info,0x94,NULL);
	ret = wf_io_write32(nic_info,0x94,0);
    if (WF_RETURN_FAIL == ret)
    {
    	LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

#endif


    return WF_RETURN_OK;
}

int wf_hw_mcu_enable(nic_info_st *nic_info)
{
    int ret;

#ifdef CONFIG_RICHV200
    wf_u8 value8 = 0;

    value8 = wf_io_read8(nic_info,0x94,NULL);
    ret = wf_io_write8(nic_info,0x94,value8|0x6);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }
#else
    wf_io_read32(nic_info,0x94,NULL);

    ret = wf_io_write32(nic_info,0x94,0x6);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

    wf_io_read32(nic_info,0xe4,NULL);

    ret = wf_io_write32(nic_info,0xe4,0x1);
    if (WF_RETURN_FAIL == ret)
    {
    	LOG_E("[%s] WF_EN_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }
#endif

    return WF_RETURN_OK;
}


int wf_hw_mcu_startup(nic_info_st *nic_info)
{
    int ret;

#ifdef CONFIG_RICHV200
    wf_u8 value8 = 0;

    value8 = wf_io_read8(nic_info,0x94,NULL);
    ret = wf_io_write8(nic_info,0x94,value8|0x1);
    if (WF_RETURN_FAIL == ret)
    {
    	LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

#else
    ret = wf_io_write32(nic_info,0xe4,0x2);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_EN_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

    ret = wf_io_write32(nic_info,0x94,0x7);
    if (WF_RETURN_FAIL == ret)
    {
    	LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }   
#endif
    return WF_RETURN_OK;
}


