/*
 * rx_rtthread.h
 *
 * rx frame handle api declare.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __RX_RTTHREAD__
#define __RX_RTTHREAD__

int wf_rx_work(struct rt_wlan_device *wlan, struct hif_netbuf *netbuf);

#endif
