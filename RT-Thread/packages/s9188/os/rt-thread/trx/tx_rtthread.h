/*
 * tx_rtthread.h
 *
 * data frame tx xmit function declare.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __TX_RTTHREAD__
#define __TX_RTTHREAD__

void tx_work_init(struct rt_wlan_device *wlan);
void tx_work_term(struct rt_wlan_device *wlan);
void tx_work_wake(struct rt_wlan_device *wlan);

#endif
