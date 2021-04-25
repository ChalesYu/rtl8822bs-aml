
#ifndef __WF_CFG80211_H__
#define __WF_CFG80211_H__

#include "common.h"
#include <net/cfg80211.h>

#ifdef CONFIG_IOCTL_CFG80211
#define NUM_PRE_AUTH_KEY 16
#define NUM_PMKID_CACHE NUM_PRE_AUTH_KEY
#define STATION_INFO_ASSOC_REQ_IES 0

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

#define SET_CFG80211_REPORT_MGMT(w, t, v) (w->report_mgmt |= (v?BIT(t >> 4):0))
#define CLR_CFG80211_REPORT_MGMT(w, t, v) (w->report_mgmt &= (~BIT(t >> 4)))


void wf_cfg80211_scan_complete(nic_info_st *pnic_info);
int wf_cfg80211_alloc (nic_info_st *pnic_info);
void wf_cfg80211_wiphy_unreg (nic_info_st *pnic_info);
void wf_cfg80211_wiphy_free (nic_info_st *pnic_info);
void wf_cfg80211_widev_free (nic_info_st *pnic_info);
int wf_cfg80211_reg (struct wiphy *pwiphy);
void wf_cfg80211_widev_unreg (nic_info_st *pnic_info);
int wf_cfg80211_mgmt_tx(nic_info_st *pnic_info, wf_u8 tx_ch, wf_u8 no_cck, const wf_u8 *buf, size_t len, int wait_ack);
void wf_cfg80211_indicate_connect (nic_info_st *pnic_info);
void wf_cfg80211_ibss_indicate_connect (nic_info_st *pnic_info);
void wf_cfg80211_indicate_disconnect(nic_info_st *pnic_info);
void wf_cfg80211_indicate_disconnect(nic_info_st *pnic_info);
void wf_ap_cfg80211_assoc_event_up(nic_info_st *pnic_info, 	wf_u8 *passoc_req, wf_u32 assoc_req_len);
void wf_ap_cfg80211_disassoc_event_up(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info );
void wf_cfg80211_scan_done_event_up(nic_info_st *pnic_info, wf_bool aborted);
struct cfg80211_bss *inform_bss (nic_info_st *pnic_info, wf_wlan_scanned_info_t *pscaned_info);
void wf_cfg80211_vir_nic_scan_finish(nic_info_st *pnic_info, wf_bool babort);

#define ndev_to_wdev(n) ((n)->ieee80211_ptr)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
#define rtw_cfg80211_mgmt_tx_status(adapter, cookie, buf, len, ack, gfp) cfg80211_mgmt_tx_status((adapter)->pnetdev, cookie, buf, len, ack, gfp)
#else
#define rtw_cfg80211_mgmt_tx_status(adapter, cookie, buf, len, ack, gfp) cfg80211_mgmt_tx_status((adapter)->rtw_wdev, cookie, buf, len, ack, gfp)
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
