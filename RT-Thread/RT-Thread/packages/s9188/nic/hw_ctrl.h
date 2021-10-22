/*
 * hw_ctrl.h
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
#ifndef __MCU_CTRL_H__
#define __MCU_CTRL_H__


int wf_hw_mcu_disable(nic_info_st *nic_info);
int wf_hw_mcu_enable(nic_info_st *nic_info);
int wf_hw_mcu_startup(nic_info_st *nic_info);


#endif
