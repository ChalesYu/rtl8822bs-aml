/*
 * power.h
 *
 * power on and power off operate api declare.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __POWER_H__
#define __POWER_H__

int power_on(struct hif_node_ *node);
int power_off(struct hif_node_ *node);
int side_road_cfg(struct hif_node_ *node);
int power_suspend(struct hif_node_ *node);
int power_resume (struct hif_node_ *node);

#endif

