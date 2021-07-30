/*
 * power.h
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
#ifndef __POWER_H__
#define __POWER_H__



int power_on(struct hif_node_ *node);
int power_off(struct hif_node_ *node);
int side_road_cfg(struct hif_node_ *node);
int power_suspend(struct hif_node_ *node);
int power_resume (struct hif_node_ *node);

#endif

