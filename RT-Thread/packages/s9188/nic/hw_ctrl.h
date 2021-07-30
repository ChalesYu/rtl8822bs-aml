/*
 * hw_ctrl.h
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
#ifndef __MCU_CTRL_H__
#define __MCU_CTRL_H__


int wf_hw_mcu_disable(nic_info_st *nic_info);
int wf_hw_mcu_enable(nic_info_st *nic_info);
int wf_hw_mcu_startup(nic_info_st *nic_info);


#endif
