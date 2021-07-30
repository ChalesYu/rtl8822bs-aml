/*
 * ndev_linux.h
 *
 * This file contains all the prototypes for the ndev_linux.c file
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
#ifndef __NEDV_LINUX_H__
#define __NEDV_LINUX_H__

#include "wf_config.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/rtnetlink.h>
#include <linux/skbuff.h>
#include <linux/wireless.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/ieee80211.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>

#ifdef CONFIG_IOCTL_CFG80211
#include <linux/nl80211.h>
#include <net/cfg80211.h>
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 0, 8)
#include <linux/export.h>
#else
#include <linux/nfsd/export.h>
#endif

#include <linux/types.h>
#include <linux/uaccess.h>
#include <net/iw_handler.h>
#include <net/rtnetlink.h>
#include <net/net_namespace.h>


#include "iw.h"

#include "common.h"

#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
extern rx_pkt_t pkt_temp2;


#if (LINUX_VERSION_CODE>=KERNEL_VERSION(3,0,0))
#include <linux/u64_stats_sync.h>
#endif

typedef struct sk_buff   wf_pkt;

#ifdef CONFIG_IOCTL_CFG80211
typedef struct cfg80211_wifidirect_info
{

    struct ieee80211_channel remain_on_ch_channel;
    enum nl80211_channel_type remain_on_ch_type;
    atomic_t ro_ch_cookie_gen;
    wf_u64 remain_on_ch_cookie;
    wf_u32 duration;
} cfg80211_wifidirect_info_st;
#endif

typedef struct wf_widev_priv
{
    struct wireless_dev *pwidev;
    void *pnic_info;

    struct cfg80211_scan_request *pscan_request;
    wf_lock_t scan_req_lock;

    struct net_device *pmon_ndev;
    char ifname_mon[IFNAMSIZ + 1];

    wf_bool bandroid_scan;
    wf_bool block;
    wf_bool block_scan;
    wf_bool power_mgmt;

    wf_u16 report_mgmt;

    atomic_t ro_ch_to;
    atomic_t switch_ch_to;
} wf_widev_priv_t;


typedef struct
{
    nic_info_st *nic;

    wf_widev_priv_t widev_priv;
    struct iw_statistics  iw_stats;
    struct tasklet_struct recv_task;
    struct tasklet_struct send_task;
    struct tasklet_struct get_tx_data_task;
    struct net_device_stats stats;

#ifdef CONFIG_IOCTL_CFG80211
    cfg80211_wifidirect_info_st cfg80211_wifidirect;
#endif
} ndev_priv_st;

int ndev_notifier_register(void);
void ndev_notifier_unregister(void);
int ndev_unregister(nic_info_st *nic_info);
int ndev_register(nic_info_st *nic_info);
int ndev_shutdown(nic_info_st *nic_info);

int ndev_rx(nic_info_st *nic_info, struct sk_buff *skb);
ndev_priv_st *ndev_get_priv(nic_info_st *nic_info);
int ndev_open(struct net_device *ndev);

void ndev_tx_resource_enable (struct net_device *ndev, wf_pkt *pkt);
void ndev_tx_resource_disable (struct net_device *ndev, wf_pkt *pkt);



#endif
