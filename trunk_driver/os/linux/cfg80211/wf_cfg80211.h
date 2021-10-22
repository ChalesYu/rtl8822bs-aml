/*
 * wf_cfg80211.h
 *
 * used for netlink framework interface
 *
 * Author: houchuang
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
#ifndef __WF_CFG80211_H__
#define __WF_CFG80211_H__

#include "common.h"
#include <net/cfg80211.h>

#ifdef CONFIG_IOCTL_CFG80211
#define NUM_PRE_AUTH_KEY 16
#define NUM_PMKID_CACHE NUM_PRE_AUTH_KEY

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0))  && !defined(COMPAT_KERNEL_RELEASE)
#define wf_cfg80211_rx_mgmt(pnic_info, freq, sig_dbm, buf, len, gfp) cfg80211_rx_mgmt(pnic_info->ndev, freq, buf, len, gfp)
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
#define wf_cfg80211_rx_mgmt(pnic_info, freq, sig_dbm, buf, len, gfp) cfg80211_rx_mgmt(pnic_info->ndev, freq, sig_dbm, buf, len, gfp)
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3,12,0))
#define wf_cfg80211_rx_mgmt(pnic_info, freq, sig_dbm, buf, len, gfp) cfg80211_rx_mgmt((pnic_info)->pwidev, freq, sig_dbm, buf, len, gfp)
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3 , 18 , 0))
#define wf_cfg80211_rx_mgmt(pnic_info , freq , sig_dbm , buf , len , gfp) cfg80211_rx_mgmt((pnic_info)->pwidev , freq , sig_dbm , buf , len , 0 , gfp)
#else
#define wf_cfg80211_rx_mgmt(pnic_info , freq , sig_dbm , buf , len , gfp) cfg80211_rx_mgmt((pnic_info)->pwidev , freq , sig_dbm , buf , len , 0)
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0))  && !defined(COMPAT_KERNEL_RELEASE)
#define wf_cfg80211_send_rx_assoc(pnic_info, bss, buf, len) cfg80211_send_rx_assoc(pnic_info->ndev, buf, len)
#else
#define wf_cfg80211_send_rx_assoc(pnic_info, bss, buf, len) cfg80211_send_rx_assoc(pnic_info->ndev, bss, buf, len)
#endif


void wf_cfg80211_scan_complete(nic_info_st *pnic_info);
int wf_cfg80211_alloc (nic_info_st *pnic_info);
void wf_cfg80211_wiphy_unreg (nic_info_st *pnic_info);
void wf_cfg80211_wiphy_free (nic_info_st *pnic_info);
void wf_cfg80211_widev_free (nic_info_st *pnic_info);
int wf_cfg80211_reg (struct wiphy *pwiphy);
void wf_cfg80211_widev_unreg (nic_info_st *pnic_info);
void wf_cfg80211_indicate_connect (nic_info_st *pnic_info);
#ifdef CFG_ENABLE_ADHOC_MODE
void wf_cfg80211_ibss_indicate_connect (nic_info_st *pnic_info);
void wf_cfg80211_unlink_ibss(nic_info_st *pnic_info);
#endif
void wf_cfg80211_indicate_disconnect(nic_info_st *pnic_info);
void wf_ap_cfg80211_assoc_event_up(nic_info_st *pnic_info, 	wf_u8 *passoc_req, wf_u32 assoc_req_len);
void wf_ap_cfg80211_disassoc_event_up(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info );
void wf_cfg80211_scan_done_event_up(nic_info_st *pnic_info, wf_bool aborted);
struct cfg80211_bss *inform_bss (nic_info_st *pnic_info, wf_wlan_mgmt_scan_que_node_t *pscaned_info);
void wf_cfg80211_vir_nic_scan_finish(nic_info_st *pnic_info, wf_bool babort);

int wf_cfg80211_p2p_cb_reg(nic_info_st *pnic_info);

#define ndev_to_wdev(n) ((n)->ieee80211_ptr)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
#define wf_cfg80211_ready_on_channel(ndev, cookie, chan, channel_type, duration, gfp)  cfg80211_ready_on_channel(ndev, cookie, chan, channel_type, duration, gfp)
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
#define wf_cfg80211_ready_on_channel(ndev, cookie, chan, channel_type, duration, gfp)  cfg80211_ready_on_channel(ndev, cookie, chan, channel_type, duration, gfp)
#else
#define wf_cfg80211_ready_on_channel(ndev, cookie, chan, channel_type, duration, gfp)  cfg80211_ready_on_channel(ndev, cookie, chan, duration, gfp)
#endif




struct wf_wdev_info
{
    struct wireless_dev *wf_wdev;
    nic_info_st *pnic_info;

    struct cfg80211_scan_request *scan_req;
    wf_lock_spin scan_req_lock;

    struct net_device *pndev;

    bool scan_block;
    bool block;
};
#endif

struct wf_netdev_priv
{
	void *priv;
	wf_u32 priv_size;
};

enum wf_cfg80211_channel_flags
{
    WF_CFG80211_CHANNEL_DISABLED = 1 << 0,
    WF_CFG80211_CHANNEL_PASSIVE_SCAN = 1 << 1,
    WF_CFG80211_CHANNEL_NO_IBSS = 1 << 2,
    WF_CFG80211_CHANNEL_RADAR = 1 << 3,
    WF_CFG80211_CHANNEL_NO_HT40PLUS = 1 << 4,
    WF_CFG80211_CHANNEL_NO_HT40MINUS = 1 << 5,
};

#endif
