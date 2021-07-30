/*
 * rx_linux.h
 *
 * used for frame xmit for linux
 *
 * Author: renhaibo
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
#ifndef __RX_LINUX__
#define __RX_LINUX__

int rx_work(struct net_device *ndev, struct sk_buff *skb);

#endif
