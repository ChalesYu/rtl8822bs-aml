/*
 * ndev_linux.c
 *
 * impliment linux framework net device regiest
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
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
/* use linux netdev ioctl framework */
#include <net/ieee80211_radiotap.h>
#include "ndev_linux.h"
#include "wf_cfg80211.h"
#include "hif.h"
#include "rx.h"
#include "common.h"
#ifdef CONFIG_IOCTL_CFG80211
#include <linux/nl80211.h>
#include <net/cfg80211.h>
#endif
#ifdef CONFIG_OS_ANDROID
extern int wf_android_priv_cmd_ioctl(struct net_device *net, struct ifreq *ifr, int cmd);
#endif


#ifndef IEEE80211_BAND_2GHZ
#define IEEE80211_BAND_2GHZ NL80211_BAND_2GHZ
#endif

#define NDEV_DBG(fmt, ...)      LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define NDEV_INFO(fmt, ...)     LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define NDEV_WARN(fmt, ...)     LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define NDEV_ERROR(fmt, ...)    LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)

static int ndev_init(struct net_device *ndev)
{
    ndev_priv_st *ndev_priv;
    hw_info_st *hw_info;

    NDEV_DBG("[NDEV]%p ndev_init ", ndev);

    ndev_priv = netdev_priv(ndev);

    if (nic_init(ndev_priv->nic) < 0)
        return -1;

    tx_work_init(ndev);

    hw_info = (hw_info_st *)ndev_priv->nic->hw_info;
    if (hw_info)
    {
        if (!is_valid_ether_addr(hw_info->macAddr))
        {
            NDEV_ERROR("[NDEV]%p mac addr is invalid ", ndev);
            //return -1;
        }

        NDEV_INFO("efuse_macaddr:"WF_MAC_FMT, WF_MAC_ARG(hw_info->macAddr));
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
	eth_hw_addr_set(ndev, hw_info->macAddr);
#else
	memcpy(ndev->dev_addr, hw_info->macAddr, WF_ETH_ALEN);
#endif

        NDEV_INFO("[%d] macaddr:"WF_MAC_FMT, ndev_priv->nic->ndev_id, WF_MAC_ARG(hw_info->macAddr));
    }

    return 0;
}

static void ndev_uninit(struct net_device *ndev)
{
    ndev_priv_st *ndev_priv;

    NDEV_DBG("[NDEV] ndev_uninit - start");

    ndev_priv = netdev_priv(ndev);

    tx_work_term(ndev);

    nic_term(ndev_priv->nic);

    NDEV_DBG("[NDEV] ndev_uninit - end");
}

#ifdef CONFIG_IOCTL_CFG80211
static void wiphy_cap_ext_init (nic_info_st *pnic_info,
                                struct ieee80211_sta_ht_cap *ht_cap,
                                enum nl80211_band band, wf_u8 rf_type)
{
    hw_info_st *phw_info = pnic_info->hw_info;

    if (phw_info->ldpc_support)
    {
        ht_cap->cap |= WF_80211_MGMT_HT_CAP_LDPC_CODING;
    }

    if (phw_info->tx_stbc_support)
    {
        ht_cap->cap |= WF_80211_MGMT_HT_CAP_TX_STBC;
    }

    if (phw_info->rx_stbc_support)
    {
        if (NL80211_BAND_2GHZ == band)
        {
            if (rf_type == 3)
            {
                ht_cap->cap |= WF_80211_MGMT_HT_CAP_RX_STBC_1R;
            }
        }
    }
}

static void wiphy_cap_init (nic_info_st *pnic_info,
                            struct ieee80211_sta_ht_cap *pht_cap,
                            enum nl80211_band band,
                            wf_u8 rf_type)
{
    NDEV_DBG();

    pht_cap->ht_supported = wf_true;
    pht_cap->cap = IEEE80211_HT_CAP_SUP_WIDTH_20_40 |
                   IEEE80211_HT_CAP_SGI_40 |
                   IEEE80211_HT_CAP_SGI_20 |
                   IEEE80211_HT_CAP_DSSSCCK40 |
                   IEEE80211_HT_CAP_MAX_AMSDU;
    wiphy_cap_ext_init(pnic_info, pht_cap, band, rf_type);
    pht_cap->ampdu_factor = IEEE80211_HT_MAX_AMPDU_64K;
    pht_cap->ampdu_density = IEEE80211_HT_MPDU_DENSITY_16;
    pht_cap->mcs.tx_params = IEEE80211_HT_MCS_TX_DEFINED;
    if (rf_type == 3)
    {
        pht_cap->mcs.rx_mask[0] = 0xFF;
        pht_cap->mcs.rx_highest = 150;
    }
    else
    {
        NDEV_INFO("error rf_type=%d\n", rf_type);
    }
}

static void wiphy_apply_flag (nic_info_st *pnic_info)
{
    struct wiphy *pwiphy;
    hw_info_st *phw_info;
    wf_u8 i, j;
    struct ieee80211_supported_band *pbands;
    struct ieee80211_channel *pch;
    wf_u8 max_chan_nums;
    wf_u16 channel;
    wf_channel_info_t *channel_set;

    if (WF_CANNOT_RUN(pnic_info))
    {
        NDEV_WARN("WF_CANNOT_RUN!!!!!!!!!!!!!!");
        return;
    }

    pwiphy = pnic_info->pwiphy;
    phw_info = (hw_info_st *)pnic_info->hw_info;
    channel_set = phw_info->channel_set;

    for (i = 0; i <  NUM_NL80211_BANDS /* WF_ARRAY_SIZE(pwiphy->bands)*/; i++)
    {
        pbands = pwiphy->bands[i];

	if (!pbands) continue;

        for (j = 0; j < pbands->n_channels ; j++)
        {
		pch = &pbands->channels[j];
		if (!pch) continue;
		pch->flags = IEEE80211_CHAN_DISABLED;
	}

    }

    max_chan_nums = 14;  //phw_info->max_chan_nums;
    for (i = 0; i < max_chan_nums; i++)
    {
        channel = channel_set[i].channel_num;
        pch = ieee80211_get_channel(pwiphy, wf_ch_2_freq(channel));


	if (!pch) continue;

	pch->flags = 0;

            if (channel_set[i].scan_type == SCAN_TYPE_PASSIVE)
            {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
                pch->flags |=
                    (IEEE80211_CHAN_NO_IBSS | IEEE80211_CHAN_PASSIVE_SCAN);
#else
                pch->flags |= IEEE80211_CHAN_NO_IR;
#endif
            }


    }
}

static inline nic_info_st *wiphy_to_nic_info (struct wiphy *pwiphy)
{
    return *(nic_info_st **)wiphy_priv(pwiphy);
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0))
int wiphy_reg_notifier (struct wiphy *pwiphy, struct regulatory_request *request)
#else
void wiphy_reg_notifier (struct wiphy *pwiphy, struct regulatory_request *request)
#endif
{
    nic_info_st *pnic_info;

    NDEV_DBG();

    pnic_info = wiphy_to_nic_info(pwiphy);
    if (pnic_info)
    {
        wiphy_apply_flag(pnic_info);
    }
    else
    {
        LOG_E("pnic_info null point !!!");
    }

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0))
    return 0;
#endif
}

#define WL_2GHZ_CH01_11 \
    REG_RULE(2412-10, 2462+10, 40, 0, 20, 0)

#define WL_2GHZ_CH12_13 \
    REG_RULE(2467-10, 2472+10, 40, 0, 20,   \
    NL80211_RRF_PASSIVE_SCAN)

#define WL_2GHZ_CH14    \
    REG_RULE(2484-10, 2484+10, 40, 0, 20,   \
    NL80211_RRF_PASSIVE_SCAN | NL80211_RRF_NO_OFDM)

static const struct ieee80211_regdomain regdom_rd =
{
    .n_reg_rules = 2,
    .alpha2 = "99",
    .reg_rules =
    {
        WL_2GHZ_CH01_11,
        WL_2GHZ_CH12_13,
    }
};

static void wiphy_regd_init (nic_info_st *pnic_info)
{
    struct wiphy *pwiphy = pnic_info->pwiphy;

    NDEV_DBG();

    pwiphy->reg_notifier = wiphy_reg_notifier;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
    pwiphy->flags |= WIPHY_FLAG_CUSTOM_REGULATORY;
    pwiphy->flags &= ~WIPHY_FLAG_STRICT_REGULATORY;
    pwiphy->flags &= ~WIPHY_FLAG_DISABLE_BEACON_HINTS;
#else
    pwiphy->regulatory_flags |= REGULATORY_CUSTOM_REG;
    pwiphy->regulatory_flags &= ~REGULATORY_STRICT_REG;
    pwiphy->regulatory_flags &= ~REGULATORY_DISABLE_BEACON_HINTS;
#endif
	NDEV_DBG();
    wiphy_apply_custom_regulatory(pwiphy, &regdom_rd);
	NDEV_DBG();
    wiphy_apply_flag(pnic_info);
    NDEV_DBG();
}

void wf_wiphy_init(nic_info_st *pnic_info)
{
    struct ieee80211_supported_band *pband;
    struct wiphy *pwiphy = pnic_info->pwiphy;
    hw_info_st *phw_info = pnic_info->hw_info;

    NDEV_DBG();

    pband = pwiphy->bands[IEEE80211_BAND_2GHZ];
    if (pband)
    {
        wiphy_cap_init(pnic_info, &pband->ht_cap,
                       IEEE80211_BAND_2GHZ, phw_info->rf_type);
    }

    wiphy_regd_init(pnic_info);
    wf_memcpy(pwiphy->perm_addr, nic_to_local_addr(pnic_info), sizeof(wf_80211_addr_t));
}
#endif
int ndev_open(struct net_device *ndev)
{
    ndev_priv_st *ndev_priv = NULL;
    nic_info_st *pnic_info  = NULL;


    ndev_priv = netdev_priv(ndev);
    pnic_info = ndev_priv->nic;
    if (NULL == pnic_info)
    {
        return -1;
    }

    NDEV_DBG("[%d] ndev_open ", pnic_info->ndev_id);
    if (nic_enable(pnic_info) == WF_RETURN_FAIL)
    {
        return -1;
    }
#if 0
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 35))
    netif_tx_start_all_queues(ndev);
#else
    netif_start_queue(ndev);
#endif
#endif

#ifdef CONFIG_IOCTL_CFG80211
    wf_wiphy_init(pnic_info);
#endif
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 35))
    netif_tx_wake_all_queues(ndev);
#else
    netif_wake_queue(ndev);
#endif

#if 1
    {
        nic_info_st *buddy_nic = NULL;
        buddy_nic = pnic_info->buddy_nic;
        if (buddy_nic && buddy_nic->is_up == 0)
        {
            if (buddy_nic->ndev)
            {
                ndev_open(buddy_nic->ndev);
            }
            else
            {
                NDEV_DBG("[NDEV] buddy_nic->ndev is null");
            }
        }
    }
#endif
    NDEV_DBG("[%d] ndev_open - end", pnic_info->ndev_id);
    return  0;
}

static int ndev_stop(struct net_device *ndev)
{
    ndev_priv_st *ndev_priv = NULL;
    nic_info_st  *pnic_info = NULL;

    ndev_priv = netdev_priv(ndev);
    pnic_info = ndev_priv->nic;

    if (NULL == pnic_info)
    {
        return -1;
    }

    //NDEV_DBG("[%d] ndev_stop", pnic_info->ndev_id);

    if (nic_disable(pnic_info) == WF_RETURN_FAIL)
    {
        return -1;
    }

#if 0
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 35))
    netif_tx_stop_all_queues(ndev);
#else
    netif_stop_queue(ndev);
#endif
#endif

    NDEV_DBG("[%d] ndev_stop - end", pnic_info->ndev_id);

    return 0;
}

void ndev_tx_resource_enable (struct net_device *ndev, wf_pkt *pkt)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
    wf_u16 qidx;

    qidx = skb_get_queue_mapping(pkt);

    netif_tx_wake_all_queues(ndev);
#else
    netif_wake_queue(ndev);
#endif
}

void ndev_tx_resource_disable (struct net_device *ndev, wf_pkt *pkt)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
    wf_u16 qidx;

    qidx = skb_get_queue_mapping(pkt);

    netif_tx_stop_all_queues(ndev);
#else
    netif_stop_queue(ndev);
#endif
}

wf_u32 total_cnt = 0;

static int ndev_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
    ndev_priv_st *ndev_priv;
    wf_bool bRet = wf_false;

    if (!skb)
    {
        return 0;
    }

    ndev_priv = netdev_priv(ndev);

    if (WF_CANNOT_RUN(ndev_priv->nic))
    {
        return 0;
    }

    if (ndev_priv->nic->tx_info == NULL)
    {
        return 0;
    }

    if (wf_mlme_check_mode(ndev_priv->nic, WF_MONITOR_MODE) == wf_true)
    {
        //work_monitor_tx_entry(pnetdev, (struct sk_buff *)pkt);
    }
    else
    {
        if (wf_false == wf_tx_data_check(ndev_priv->nic))
        {
            dev_kfree_skb_any(skb);
        }
        else
        {
            /* tx resource check */
            bRet = wf_need_stop_queue(ndev_priv->nic);
            if (bRet == wf_true)
            {
                tx_info_st *tx_info = ndev_priv->nic->tx_info;
                if (tx_info)
                {
                    LOG_W(">>>>ndev tx stop queue, free:%d, pending:%d", tx_info->free_xmitframe_cnt, tx_info->pending_frame_cnt);
                }
                ndev_tx_resource_disable(ndev, skb);
            }

            /* actually xmit */
            if (0 != wf_tx_msdu(ndev_priv->nic, skb->data, skb->len, skb))
            {
                /* failed xmit, must release the resource */
                dev_kfree_skb_any(skb);
            }
            else
            {
                tx_work_wake(ndev);
            }
        }
    }

    return 0;
}

static const wf_u16 select_queue[8] = { 2, 3, 3, 2, 1, 1, 0, 0 };

static unsigned int classify8021d(struct sk_buff *skb)
{
    unsigned int dscp;

    if (skb->priority >= 256 && skb->priority <= 263)
        return skb->priority - 256;

    switch (skb->protocol)
    {
        case htons(ETH_P_IP):
            dscp = ip_hdr(skb)->tos & 0xfc;
            break;
        default:
            return 0;
    }

    return dscp >> 5;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
static wf_u16 ndev_select_queue(struct net_device *ndev, struct sk_buff *skb, struct net_device *sb_dev)
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
static wf_u16 ndev_select_queue(struct net_device *ndev, struct sk_buff *skb, struct net_device *sb_dev, select_queue_fallback_t fallback)
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
static wf_u16 ndev_select_queue(struct net_device *ndev, struct sk_buff *skb, void *accel_priv, select_queue_fallback_t fallback)
#else
static wf_u16 ndev_select_queue(struct net_device *ndev, struct sk_buff *skb)
#endif
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_u8 *curBssid;
    wdn_net_info_st *pwdn_info;

    skb->priority = classify8021d(skb);

    if (pnic_info == NULL)
    {
        return 0;
    }

    curBssid = wf_wlan_get_cur_bssid(pnic_info);
    if (curBssid== NULL)
    {
        return 0;
    }

    pwdn_info = wf_wdn_find_info(pnic_info, curBssid);
    if (pwdn_info == NULL)
    {
        return 0;
    }

    if (pwdn_info->acm_mask != 0)
    {
        skb->priority = wf_chk_qos(pwdn_info->acm_mask, skb->priority, 1);
    }

    return select_queue[skb->priority];
}

static int ndev_set_mac_addr(struct net_device *pnetdev, void *addr)
{
    struct sockaddr *sock_addr = addr;
    ndev_priv_st *ndev_priv = NULL;
    nic_info_st *pnic_info = NULL;
    ndev_priv = netdev_priv(pnetdev);
    pnic_info = ndev_priv->nic;

    NDEV_DBG("ndev_set_mac_addr:" WF_MAC_FMT ,WF_MAC_ARG(sock_addr->sa_data));

    if (!is_valid_ether_addr(sock_addr->sa_data))
    return -EADDRNOTAVAIL;

    if(pnic_info->is_up)
    {
        NDEV_ERROR("The interface is not in down state");
        return -1;
    }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
    wf_memcpy(nic_to_local_addr(pnic_info), sock_addr->sa_data, MAC_ADDR_LEN);

    eth_hw_addr_set(pnetdev, sock_addr->sa_data);
    //wf_memcpy(pnetdev->dev_addr, sock_addr->sa_data, WF_ETH_ALEN);

    wf_mcu_set_macaddr(pnic_info, pnetdev->dev_addr);
#else
    wf_memcpy(nic_to_local_addr(pnic_info), sock_addr->sa_data, MAC_ADDR_LEN);
    wf_memcpy(pnetdev->dev_addr, sock_addr->sa_data, WF_ETH_ALEN);

    wf_mcu_set_macaddr(pnic_info, pnetdev->dev_addr);
#endif
    return 0;
}

static struct net_device_stats *ndev_get_stats(struct net_device *ndev)
{
    ndev_priv_st *ndev_priv;
    nic_info_st *nic_info;
    tx_info_st *tx_info;
    rx_info_t *rx_info;

    //NDEV_DBG("ndev_get_stats ");

    ndev_priv = netdev_priv(ndev);
    if (ndev_priv == NULL)
        return NULL;

    nic_info = ndev_priv->nic;
    if (nic_info == NULL)
        return &ndev_priv->stats;

    tx_info = nic_info->tx_info;
    if (tx_info == NULL)
        return &ndev_priv->stats;

    rx_info = nic_info->rx_info;
    if (rx_info == NULL)
        return &ndev_priv->stats;

    ndev_priv->stats.tx_packets = tx_info->tx_pkts;
    ndev_priv->stats.rx_packets = rx_info->rx_pkts;
    ndev_priv->stats.tx_dropped = tx_info->tx_drop;
    ndev_priv->stats.rx_dropped = rx_info->rx_drop;
    ndev_priv->stats.tx_bytes = tx_info->tx_bytes;
    ndev_priv->stats.rx_bytes = rx_info->rx_bytes;

    return &ndev_priv->stats;
}

#ifdef CFG_ENABLE_AP_MODE
int wf_hostapd_ioctl(struct net_device *dev, struct iw_point *p)
{
    ndev_priv_st *ndev_priv = netdev_priv(dev);
    nic_info_st *nic_info = ndev_priv->nic;
    ieee_param *param ;
    wf_u32 ret = 0;
    wf_ap_status ap_sta;

    NDEV_DBG("[IOCTL]");
    if (wf_local_cfg_get_work_mode(nic_info) != WF_MASTER_MODE)
    {
        return -1;
    }

    param = (ieee_param *)wf_vmalloc(p->length);
    if (param == NULL)
    {
        return -1;
    }

    if (copy_from_user(param, p->pointer, p->length))
    {
        NDEV_DBG("memcpy error");
        wf_vfree(param);
        return -1;
    }
    NDEV_DBG("[IOCTL] param->cmd:%d ", param->cmd);

    ap_sta = wf_ap_status_get(nic_info);

    switch(param->cmd)
    {
        case WF_HOSTAPD_FLUSH :
            if (ap_sta == WF_AP_STATUS_ESTABLISHED)
            {
                wf_ap_deauth_all_sta(nic_info, WF_80211_REASON_DEAUTH_LEAVING);
            }
            if (ap_sta > WF_AP_STATUS_UNINITILIZED)
            {
                ret = wf_ap_work_stop(nic_info);
            }
            wf_mcu_set_hw_invalid_all(nic_info);
            break;

        case WF_SET_ENCRYPTION :
            ret = wf_ap_set_encryption(nic_info, param, p->length);
            break;

        case WF_HOSTAPD_SET_BEACON :
            if (ap_sta == WF_AP_STATUS_UNINITILIZED)
            {
                ret = wf_ap_pre_set_beacon(nic_info, param, p->length);
                if (!ret)
                {
                    ret = wf_ap_work_start(nic_info);
                }
            }
            else
            {
                NDEV_WARN("[IOCTL]""AP is runing, need stop first");
                ret = -1;
            }
            break;

        case WF_HOSTAPD_GET_WPAIE_STA :
            ret = wf_ap_get_sta_wpaie(nic_info, param, p->length);
            break;

        case WF_HOSTAPD_REMOVE_STA :
            ret = wf_ap_remove_sta(nic_info, param, p->length);
            break;

        default:
            break;
    }

    if (ret == 0 && copy_to_user(p->pointer, param, p->length))
    {
        ret = -EFAULT;
    }

    wf_vfree(param);

    return ret;
}
#endif


static int ndev_ioctl(struct net_device *dev, struct ifreq *req, int cmd)
{
    ndev_priv_st *ndev_priv;
#ifdef CFG_ENABLE_AP_MODE
    struct iwreq *wrq = (struct iwreq *)req;
#endif
    int ret = 0;

    ndev_priv = netdev_priv(dev);
    if (WF_CANNOT_RUN(ndev_priv->nic))
    {
        return 0;
    }

    //NDEV_DBG("ndev_ioctl cmd:%x", cmd);

    switch (cmd)
    {
        case IW_IOC_WPA_SUPPLICANT:
            NDEV_DBG("ndev_ioctl IW_PRV_WPA_SUPPLICANT");
            break;

        case IW_IOC_HOSTAPD:
            NDEV_DBG("ndev_ioctl IW_PRV_HOSTAPD");
#ifdef CFG_ENABLE_AP_MODE
            wf_hostapd_ioctl(dev, &wrq->u.data);
#endif
            break;

        case (SIOCDEVPRIVATE + 1):     /* Android ioctl */
#ifdef CONFIG_OS_ANDROID
            NDEV_DBG("ndev_ioctl SIOCDEVPRIVATE");
            ret = wf_android_priv_cmd_ioctl(dev, req, cmd);
#endif
            break;

        default:
            ret = -EOPNOTSUPP;
            break;
    }

    return ret;
}

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 29))
static const struct net_device_ops ndev_ops =
{
    .ndo_init = ndev_init,
    .ndo_uninit = ndev_uninit,
    .ndo_open = ndev_open,
    .ndo_stop = ndev_stop,
    .ndo_start_xmit = ndev_start_xmit,
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 35))
    .ndo_select_queue = ndev_select_queue,
#endif
    .ndo_set_mac_address = ndev_set_mac_addr,
    .ndo_get_stats = ndev_get_stats,
    .ndo_do_ioctl = ndev_ioctl,
};
#endif

static void ndev_ops_init(struct net_device *ndev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
    ndev->netdev_ops = &ndev_ops;
#else
    ndev->init = ndev_init;
    ndev->uninit = ndev_uninit;
    ndev->open = ndev_open;
    ndev->stop = ndev_stop;
    ndev->hard_start_xmit = ndev_start_xmit;
    ndev->set_mac_address = ndev_set_mac_addr;
    ndev->get_stats = ndev_get_stats;
    ndev->do_ioctl = ndev_ioctl;
#endif
}

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 29))
static const struct net_device_ops ndev_vir_ops =
{
    .ndo_init = ndev_init,
    .ndo_uninit = ndev_uninit,
    .ndo_open = ndev_open,
    .ndo_stop = ndev_stop,
    .ndo_start_xmit = ndev_start_xmit,
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 35))
    .ndo_select_queue = ndev_select_queue,
#endif
    .ndo_set_mac_address = ndev_set_mac_addr,
    .ndo_get_stats = ndev_get_stats,
    .ndo_do_ioctl = ndev_ioctl,
};
#endif

static void ndev_vir_ops_init(struct net_device *ndev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
    ndev->netdev_ops = &ndev_vir_ops;
#else
    ndev->init = ndev_init;
    ndev->uninit = ndev_uninit;
    ndev->open = ndev_vir_open;
    ndev->stop = ndev_vir_stop;
    ndev->hard_start_xmit = ndev_start_xmit;
    ndev->set_mac_address = ndev_set_mac_addr;
    ndev->get_stats = ndev_get_stats;
    ndev->do_ioctl = ndev_ioctl;
#endif
}

extern const struct iw_handler_def wl_handlers_def;
extern char *ifname;
extern char *if2name;

static int _ndev_notifier_cb(struct notifier_block *nb,
                             wf_ptr state, void *ptr)
{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(3, 11, 0))
    struct net_device *ndev = netdev_notifier_info_to_dev(ptr);
#else
    struct net_device *ndev = ptr;
#endif

    NDEV_DBG("ndev:%p", ndev);

    if (ndev == NULL)
    {
        return NOTIFY_DONE;
    }

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 29))
    if (ndev->netdev_ops == NULL)
    {
        return NOTIFY_DONE;
    }

    if (ndev->netdev_ops->ndo_do_ioctl == NULL)
    {
        return NOTIFY_DONE;
    }

    if (ndev->netdev_ops->ndo_do_ioctl != ndev_ioctl)
#else
    if (ndev->do_ioctl == NULL)
    {
        return NOTIFY_DONE;
    }

    if (ndev->do_ioctl != ndev_ioctl)
#endif
    {
        return NOTIFY_DONE;
    }

    NDEV_DBG("state == %lu", state);

    switch (state)
    {
        case NETDEV_CHANGENAME:
            break;
    }

    return NOTIFY_DONE;

}

static struct notifier_block wf_ndev_notifier =
{
    .notifier_call = _ndev_notifier_cb,
};

int ndev_notifier_register(void)
{
    return register_netdevice_notifier(&wf_ndev_notifier);
}

void ndev_notifier_unregister(void)
{
    unregister_netdevice_notifier(&wf_ndev_notifier);
}

int ndev_register (nic_info_st *pnic_info)
{
    int ret = 0;
    struct net_device *pndev;
    wf_u8 dev_name[16];
    ndev_priv_st *pndev_priv;

    NDEV_DBG("[NDEV] ndev_register --start <node_id:%d> <ndev_id:%d>",
             pnic_info->hif_node_id, pnic_info->ndev_id);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
    pndev = alloc_etherdev_mq(sizeof(ndev_priv_st), 4);
#else
    pndev = alloc_etherdev(sizeof(ndev_priv_st));
#endif
    if (pndev == NULL)
    {
        NDEV_WARN("alloc_etherdev error [ret:%d]", ret);
        return -1;
    }

    pndev_priv      = netdev_priv(pndev);
    pndev_priv->nic = pnic_info;
    pnic_info->ndev = pndev;
    pnic_info->widev_priv = &pndev_priv->widev_priv;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
    SET_MODULE_OWNER(pndev);
#endif

    /* regiest ethernet operation */
    if (pnic_info->nic_num == 0)
    {
        ndev_ops_init(pndev);
    }
    else
    {
        ndev_vir_ops_init(pndev);
    }

    /* set watchdog timeout */
    pndev->watchdog_timeo = HZ * 3;

#ifdef CONFIG_WIRELESS_EXT
    /* regiest wireless extension */
    pndev->wireless_handlers = (struct iw_handler_def *)&wl_handlers_def;
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0)
    SET_NETDEV_DEV(pndev, pnic_info->dev);
#endif

#ifdef CONFIG_IOCTL_CFG80211
    /* alloc nl80211 object */
    ret = wf_cfg80211_alloc(pnic_info);
    if (ret)
    {
        NDEV_WARN("nl80211 device alloc fail [ret:%d]", ret);
        return ret;
    }
    ret = wf_cfg80211_reg(pnic_info->pwiphy);
    if (ret)
    {
        NDEV_WARN("nl80211 wiphy regiest fail [ret:%d]", ret);
        return ret;
    }
#endif

#ifdef CONFIG_ANDROID_HIF
    if (1 == pnic_info->hif_node_id)
    {
        sprintf(dev_name, if2name[0] ? if2name : "p2p0");
        NDEV_DBG("dev_name:%s", dev_name);
    }
    else
#endif
        if (pnic_info->nic_type == NIC_USB)
        {
            if (pnic_info->virNic)
            {
                sprintf(dev_name, if2name[0] ? if2name : "vir%d_u%d", pnic_info->hif_node_id, pnic_info->ndev_id);
            }
            else
            {
#ifdef CONFIG_MP_MODE
                sprintf(dev_name, ifname[0] ? ifname : "wlan%d", pnic_info->hif_node_id);
#else
                sprintf(dev_name, ifname[0] ? ifname : "wlan%d_u%d", pnic_info->hif_node_id, pnic_info->ndev_id);
#endif
            }
        }
        else
        {
            if (pnic_info->virNic)
            {
                sprintf(dev_name, if2name[0] ? if2name : "vir%d_s%d", pnic_info->hif_node_id, pnic_info->ndev_id);
            }
            else
            {
#ifdef CONFIG_MP_MODE
                sprintf(dev_name, ifname[0] ? ifname : "wlan%d", pnic_info->hif_node_id);
#else
                sprintf(dev_name, ifname[0] ? ifname : "wlan%d_s%d", pnic_info->hif_node_id, pnic_info->ndev_id);
#endif
            }
        }

    if (dev_alloc_name(pndev, dev_name) < 0)
    {
        NDEV_WARN("dev_alloc_name, fail!");
    }
//    ether_setup(pndev); /* no work ??????? */

    netif_carrier_off(pndev);
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 35))
    netif_tx_stop_all_queues(pndev);
#else
    netif_stop_queue(pndev);
#endif

    ret = register_netdev(pndev);
    if (ret)
    {
#ifdef CONFIG_IOCTL_CFG80211
        wf_cfg80211_wiphy_unreg(pnic_info);
        wf_cfg80211_widev_free(pnic_info);
        wf_cfg80211_wiphy_free(pnic_info);
#endif

        free_netdev(pndev);
        pndev = NULL;
        pnic_info->ndev = NULL;
        NDEV_WARN("register_netdev error [ret:%d]", ret);
        return ret;
    }

    NDEV_DBG("[NDEV] ndev_register --end");

    return 0;
}



int ndev_unregister(nic_info_st *pnic_info)
{
    if (pnic_info->ndev != NULL)
    {
        NDEV_DBG("[NDEV] --ndev_unregister - start");
#ifdef CONFIG_IOCTL_CFG80211
        wf_cfg80211_widev_unreg(pnic_info);
#endif
        NDEV_DBG("[%d]", pnic_info->ndev_id);
        unregister_netdev(pnic_info->ndev);
#ifdef CONFIG_IOCTL_CFG80211
        wf_cfg80211_wiphy_unreg(pnic_info);
        wf_cfg80211_widev_free(pnic_info);
        wf_cfg80211_wiphy_free(pnic_info);
#endif
        free_netdev(pnic_info->ndev);
        pnic_info->ndev = NULL;

        NDEV_DBG("[NDEV] --ndev_unregister - end");
    }

    return 0;
}


ndev_priv_st * ndev_get_priv(nic_info_st *pnic_info)
{
    struct net_device *ndev         = (struct net_device *)pnic_info->ndev;
    return netdev_priv(ndev);
}


int ndev_shutdown(nic_info_st *pnic_info)
{
    NDEV_DBG();

    if (pnic_info == NULL)
    {
        return 0;
    }

    nic_shutdown(pnic_info);

    return 0;
}

int ndev_unregister_all (nic_info_st *nic_info[], wf_u8 nic_num)
{
    wf_u8 i;

    for (i = 0; i < nic_num; i++)
    {
        nic_info_st *pnic_info = nic_info[i];
        if (!pnic_info || pnic_info->is_surprise_removed)
        {
            continue;
        }

        LOG_D("ndev unregister: ndev_id: %d", pnic_info->ndev_id);
        pnic_info->is_surprise_removed = wf_true;
        ndev_shutdown(pnic_info);
        if (wf_false == pnic_info->is_init_commplete)
        {
            nic_term(pnic_info);
        }
        ndev_unregister(pnic_info);

        /* if current is real nic, all virtual nic's buddy pointer need clear */
        if (0 == i)
        {
            wf_u8 j = 0;
            for (j = 1; j < nic_num; j++)
            {
                if (nic_info[j])
                {
                    nic_info[j]->buddy_nic = NULL;
                }
            }
        }

        wf_kfree(pnic_info);
        nic_info[i] = NULL;
    }

    return 0;
}


