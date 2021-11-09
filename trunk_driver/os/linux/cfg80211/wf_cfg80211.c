/*
 * wf_cfg80211.c
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
#include <linux/nl80211.h>
#include <net/cfg80211.h>
#include <linux/decompress/mm.h>

#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"
#include "hif.h"
#include "wf_cfg80211.h"
#ifdef CONFIG_OS_ANDROID
#include "android/android_priv_cmd.h"
#endif

#ifdef CONFIG_IOCTL_CFG80211

#define CFG80211_DBG(fmt, ...)      LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define CFG80211_ARRAY(data, len)   log_array(data, len)
#define CFG80211_INFO(fmt, ...)     LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define CFG80211_WARN(fmt, ...)     LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define CFG80211_ERROR(fmt, ...)    LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)

wf_s32 wf_cfg80211_p2p_rx_mgmt(void * nic_info, void* param, wf_u32 param_len);
wf_s32 wf_cfg80211_remain_on_channel_expired (void *nic, void *param, wf_u32 param_len);
wf_s32 wf_cfg80211_p2p_ready_on_channel(void * nic_info, void* param, wf_u32 param_len);

char *cfg80211_frame_to_str(wf_80211_frame_e type)
{
    switch(type)
    {
        case WF_80211_FRM_ASSOC_REQ          :
        {
            return to_str(WF_80211_FRM_ASSOC_REQ);
        }
        case WF_80211_FRM_ASSOC_RESP         :
        {
            return to_str(WF_80211_FRM_ASSOC_RESP);
        }
        case WF_80211_FRM_REASSOC_REQ        :
        {
            return to_str(WF_80211_FRM_REASSOC_REQ);
        }
        case WF_80211_FRM_REASSOC_RESP       :
        {
            return to_str(WF_80211_FRM_REASSOC_RESP);
        }
        case WF_80211_FRM_PROBE_REQ          :
        {
            return to_str(WF_80211_FRM_PROBE_REQ);
        }
        case WF_80211_FRM_PROBE_RESP         :
        {
            return to_str(WF_80211_FRM_PROBE_RESP);
        }
        case WF_80211_FRM_BEACON             :
        {
            return to_str(WF_80211_FRM_BEACON);
        }
        case WF_80211_FRM_ATIM               :
        {
            return to_str(WF_80211_FRM_ATIM);
        }
        case WF_80211_FRM_DISASSOC           :
        {
            return to_str(WF_80211_FRM_DISASSOC);
        }
        case WF_80211_FRM_AUTH               :
        {
            return to_str(WF_80211_FRM_AUTH);
        }
        case WF_80211_FRM_DEAUTH             :
        {
            return to_str(WF_80211_FRM_DEAUTH);
        }
        case WF_80211_FRM_ACTION             :
        {
            return to_str(WF_80211_FRM_ACTION);
        }
        /* control frame */
        case WF_80211_FRM_CTL_EXT            :
        {
            return to_str(WF_80211_FRM_CTL_EXT);
        }
        case WF_80211_FRM_BACK_REQ           :
        {
            return to_str(WF_80211_FRM_BACK_REQ);
        }
        case WF_80211_FRM_BACK               :
        {
            return to_str(WF_80211_FRM_BACK);
        }
        case WF_80211_FRM_PSPOLL             :
        {
            return to_str(WF_80211_FRM_PSPOLL);
        }
        case WF_80211_FRM_RTS                :
        {
            return to_str(WF_80211_FRM_RTS);
        }
        case WF_80211_FRM_CTS                :
        {
            return to_str(WF_80211_FRM_CTS);
        }
        case WF_80211_FRM_ACK                :
        {
            return to_str(WF_80211_FRM_ACK);
        }
        case WF_80211_FRM_CFEND              :
        {
            return to_str(WF_80211_FRM_CFEND);
        }
        case WF_80211_FRM_CFENDACK           :
        {
            return to_str(WF_80211_FRM_CFENDACK);
        }
        /* data frame */
        case WF_80211_FRM_DATA               :
        {
            return to_str(WF_80211_FRM_DATA);
        }
        case WF_80211_FRM_DATA_CFACK         :
        {
            return to_str(WF_80211_FRM_DATA_CFACK);
        }
        case WF_80211_FRM_DATA_CFPOLL        :
        {
            return to_str(WF_80211_FRM_DATA_CFPOLL);
        }
        case WF_80211_FRM_DATA_CFACKPOLL     :
        {
            return to_str(WF_80211_FRM_DATA_CFACKPOLL);
        }
        case WF_80211_FRM_NULLFUNC           :
        {
            return to_str(WF_80211_FRM_NULLFUNC);
        }
        case WF_80211_FRM_CFACK              :
        {
            return to_str(WF_80211_FRM_CFACK);
        }
        case WF_80211_FRM_CFPOLL             :
        {
            return to_str(WF_80211_FRM_CFPOLL);
        }
        case WF_80211_FRM_CFACKPOLL          :
        {
            return to_str(WF_80211_FRM_CFACKPOLL);
        }
        case WF_80211_FRM_QOS_DATA           :
        {
            return to_str(WF_80211_FRM_QOS_DATA);
        }
        case WF_80211_FRM_QOS_DATA_CFACK     :
        {
            return to_str(WF_80211_FRM_QOS_DATA_CFACK);
        }
        case WF_80211_FRM_QOS_DATA_CFPOLL    :
        {
            return to_str(WF_80211_FRM_QOS_DATA_CFPOLL);
        }
        case WF_80211_FRM_QOS_DATA_CFACKPOLL :
        {
            return to_str(WF_80211_FRM_QOS_DATA_CFACKPOLL);
        }
        case WF_80211_FRM_QOS_NULLFUNC       :
        {
            return to_str(WF_80211_FRM_QOS_NULLFUNC);
        }
        case WF_80211_FRM_QOS_CFACK          :
        {
            return to_str(WF_80211_FRM_QOS_CFACK);
        }
        case WF_80211_FRM_QOS_CFPOLL         :
        {
            return to_str(WF_80211_FRM_QOS_CFPOLL);
        }
        case WF_80211_FRM_QOS_CFACKPOLL      :
        {
            return to_str(WF_80211_FRM_QOS_CFACKPOLL);
        }
        default:
        {
            return "Unknown 80211 frame";
        }
    }
}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
#define STATION_INFO_INACTIVE_TIME  BIT(NL80211_STA_INFO_INACTIVE_TIME)
#define STATION_INFO_LLID           BIT(NL80211_STA_INFO_LLID)
#define STATION_INFO_PLID           BIT(NL80211_STA_INFO_PLID)
#define STATION_INFO_PLINK_STATE    BIT(NL80211_STA_INFO_PLINK_STATE)
#define STATION_INFO_SIGNAL         BIT(NL80211_STA_INFO_SIGNAL)
#define STATION_INFO_TX_BITRATE     BIT(NL80211_STA_INFO_TX_BITRATE)
#define STATION_INFO_RX_PACKETS     BIT(NL80211_STA_INFO_RX_PACKETS)
#define STATION_INFO_TX_PACKETS     BIT(NL80211_STA_INFO_TX_PACKETS)
#define STATION_INFO_TX_FAILED      BIT(NL80211_STA_INFO_TX_FAILED)
#define STATION_INFO_LOCAL_PM       BIT(NL80211_STA_INFO_LOCAL_PM)
#define STATION_INFO_PEER_PM        BIT(NL80211_STA_INFO_PEER_PM)
#define STATION_INFO_NONPEER_PM     BIT(NL80211_STA_INFO_NONPEER_PM)
#define STATION_INFO_ASSOC_REQ_IES  0
#endif


#define WF_SSID_SCAN_AMOUNT     9
#define WF_SCAN_IE_LEN_MAX      2304

#define WF_MAX_NUM_PMKIDS       4

#define WF_MAX_REMAIN_ON_CHANNEL_DURATION   5000

#define _ASOCREQ_IE_OFFSET_     4
#define _ASOCRSP_IE_OFFSET_     6
#define _REASOCREQ_IE_OFFSET_   10
#define _REASOCRSP_IE_OFFSET_   6

static const wf_u32 cipher_suites[] =
{
    WLAN_CIPHER_SUITE_WEP40,
    WLAN_CIPHER_SUITE_WEP104,
    WLAN_CIPHER_SUITE_TKIP,
    WLAN_CIPHER_SUITE_CCMP,
};


#define CH_2G4_VAL(_channel, _freq, _flags)     \
{                                               \
    .band               = NL80211_BAND_2GHZ,    \
    .center_freq        = (_freq),              \
    .hw_value           = (_channel),           \
    .flags              = (_flags),             \
    .max_antenna_gain   = 0,                    \
    .max_power          = 30,                   \
}

static struct ieee80211_channel wf_channels_2g4[] =
{
    CH_2G4_VAL(1, 2412, 0),
    CH_2G4_VAL(2, 2417, 0),
    CH_2G4_VAL(3, 2422, 0),
    CH_2G4_VAL(4, 2427, 0),
    CH_2G4_VAL(5, 2432, 0),
    CH_2G4_VAL(6, 2437, 0),
    CH_2G4_VAL(7, 2442, 0),
    CH_2G4_VAL(8, 2447, 0),
    CH_2G4_VAL(9, 2452, 0),
    CH_2G4_VAL(10, 2457, 0),
    CH_2G4_VAL(11, 2462, 0),
    CH_2G4_VAL(12, 2467, 0),
    CH_2G4_VAL(13, 2472, 0),
    CH_2G4_VAL(14, 2484, 0),
};
#define WF_CHANNELS_2G4         (&wf_channels_2g4[0])
#define WF_CHANNELS_2G4_NUM     WF_ARRAY_SIZE(wf_channels_2g4)


#define RATE_VAL(_rate, _rateid, _flags)    \
{                                           \
    .bitrate    = (_rate),                  \
    .hw_value   = (_rateid),                \
    .flags      = (_flags),                 \
}

static struct ieee80211_rate wf_rates[] =
{
    RATE_VAL(10, 0x1, 0),
    RATE_VAL(20, 0x2, 0),
    RATE_VAL(55, 0x4, 0),
    RATE_VAL(110, 0x8, 0),
    RATE_VAL(60, 0x10, 0),
    RATE_VAL(90, 0x20, 0),
    RATE_VAL(120, 0x40, 0),
    RATE_VAL(180, 0x80, 0),
    RATE_VAL(240, 0x100, 0),
    RATE_VAL(360, 0x200, 0),
    RATE_VAL(480, 0x400, 0),
    RATE_VAL(540, 0x800, 0),
};
#define WF_RATES_B              (&wf_rates[0])
#define WF_RATES_B_NUM          4
#define WF_RATES_BG             (&wf_rates[0])
#define WF_RATES_BG_NUM         12
#define WF_RATES_A              (&wf_rates[4])
#define WF_RATES_A_NUM          8


static struct ieee80211_supported_band wf_band_2ghz =
{
    .channels   = WF_CHANNELS_2G4,
    .n_channels = WF_CHANNELS_2G4_NUM,
    .bitrates   = WF_RATES_BG,
    .n_bitrates = WF_RATES_BG_NUM,
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
static const struct ieee80211_txrx_stypes
    wl_cfg80211_default_mgmt_stypes[NUM_NL80211_IFTYPES] =
{
    [NL80211_IFTYPE_ADHOC] = {
        .tx = 0xffff,
        .rx = BIT(IEEE80211_STYPE_ACTION >> 4)
    },
    [NL80211_IFTYPE_STATION] = {
        .tx = 0xffff,
        .rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
        BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
    },
    [NL80211_IFTYPE_AP] = {
        .tx = 0xffff,
        .rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
        BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
        BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
        BIT(IEEE80211_STYPE_DISASSOC >> 4) |
        BIT(IEEE80211_STYPE_AUTH >> 4) |
        BIT(IEEE80211_STYPE_DEAUTH >> 4) |
        BIT(IEEE80211_STYPE_ACTION >> 4)
    },
    [NL80211_IFTYPE_AP_VLAN] = {

        .tx = 0xffff,
        .rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
        BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
        BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
        BIT(IEEE80211_STYPE_DISASSOC >> 4) |
        BIT(IEEE80211_STYPE_AUTH >> 4) |
        BIT(IEEE80211_STYPE_DEAUTH >> 4) |
        BIT(IEEE80211_STYPE_ACTION >> 4)
    },
    [NL80211_IFTYPE_P2P_CLIENT] = {
        .tx = 0xffff,
        .rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
        BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
    },
    [NL80211_IFTYPE_P2P_GO] = {
        .tx = 0xffff,
        .rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
        BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
        BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
        BIT(IEEE80211_STYPE_DISASSOC >> 4) |
        BIT(IEEE80211_STYPE_AUTH >> 4) |
        BIT(IEEE80211_STYPE_DEAUTH >> 4) |
        BIT(IEEE80211_STYPE_ACTION >> 4)
    },


#if defined(RTW_DEDICATED_P2P_DEVICE)
    [NL80211_IFTYPE_P2P_DEVICE] = {
        .tx = 0xffff,
        .rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
        BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
    },
#endif

};
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0))
struct ieee80211_iface_limit wf_limits[] =
{
    {
        .max = 2,
        .types = BIT(NL80211_IFTYPE_STATION)
#if defined(WF_CONFIG_P2P) && ((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE))
        | BIT(NL80211_IFTYPE_P2P_CLIENT)
#endif
    },
#ifdef CONFIG_AP_MODE
    {
        .max = 1,
        .types = BIT(NL80211_IFTYPE_AP)
#if defined(WF_CONFIG_P2P) && ((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE))
        | BIT(NL80211_IFTYPE_P2P_GO)
#endif
    },
#endif
};

struct ieee80211_iface_combination wf_combinations[] =
{
    {
        .limits = wf_limits,
        .n_limits = ARRAY_SIZE(wf_limits),
        .max_interfaces = 2,
        .num_different_channels = 1,
    },
};
#endif

static int wiphy_cfg(struct wiphy *pwiphy)
{
    int ret = 0;

    CFG80211_DBG();

    pwiphy->signal_type     = CFG80211_SIGNAL_TYPE_MBM;

    pwiphy->max_scan_ssids  = WF_SSID_SCAN_AMOUNT;
    pwiphy->max_scan_ie_len = WF_SCAN_IE_LEN_MAX;
    pwiphy->max_num_pmkids  = WF_MAX_NUM_PMKIDS;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)) || defined(COMPAT_KERNEL_RELEASE)
    pwiphy->max_remain_on_channel_duration = WF_MAX_REMAIN_ON_CHANNEL_DURATION;
#endif

    pwiphy->interface_modes = BIT(NL80211_IFTYPE_STATION)
                              | BIT(NL80211_IFTYPE_ADHOC)
#ifdef CFG_ENABLE_AP_MODE
                              | BIT(NL80211_IFTYPE_AP)
#endif
#ifdef CONFIG_WIFI_MONITOR
                              | BIT(NL80211_IFTYPE_MONITOR)
#endif
#if defined(WF_CONFIG_P2P) && ((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE))
                              | BIT(NL80211_IFTYPE_P2P_CLIENT)
                              | BIT(NL80211_IFTYPE_P2P_GO)
#endif

                              ;
    LOG_I("interface_modes:0x%x", pwiphy->interface_modes);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)  || defined(COMPAT_KERNEL_RELEASE)) /*&& LINUX_VERSION_CODE < KERNEL_VERSION(3, 0, 0))*/
#ifdef CFG_ENABLE_AP_MODE
    pwiphy->mgmt_stypes = wl_cfg80211_default_mgmt_stypes;
#endif
#endif

#if defined(WL_SINGLE_WIPHY) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0))
    wiphy->iface_combinations = wf_combinations;
    wiphy->n_iface_combinations = ARRAY_SIZE(wf_combinations);
#endif

    pwiphy->cipher_suites = cipher_suites;
    pwiphy->n_cipher_suites = WF_ARRAY_SIZE(cipher_suites);

    pwiphy->bands[NL80211_BAND_2GHZ] = &wf_band_2ghz;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38) && LINUX_VERSION_CODE < KERNEL_VERSION(3, 0, 0))
    pwiphy->flags |= WIPHY_FLAG_SUPPORTS_SEPARATE_DEFAULT_KEYS;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0))
    pwiphy->flags |= WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL;
    pwiphy->flags |= WIPHY_FLAG_HAVE_AP_SME;
#endif

#if defined(CONFIG_PM) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0))
    pwiphy->flags |= WIPHY_FLAG_SUPPORTS_SCHED_SCAN;
#endif

    return ret;
}

#ifdef CFG_ENABLE_AP_MODE
static int cfg80211_ap_hw_set_unicast_key (nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_u8 cam_id;
    wf_u16 ctrl;
    int ret;

    cam_id = pwdn_info->wdn_id + 4;

    CFG80211_DBG("cam_id == %d", cam_id);
    ctrl = BIT(15) | (pwdn_info->dot118021XPrivacy << 2);
    ret = wf_mcu_set_sec_cam(pnic_info,
                             cam_id, ctrl,
                             pwdn_info->mac, pwdn_info->dot118021x_UncstKey.skey);

    return ret;

}

static int cfg80211_ap_hw_set_group_key (nic_info_st *pnic_info, wf_u8 *pmac)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u8 cam_id;
    wf_u16 ctrl;
    int ret;

    cam_id = psec_info->dot118021XGrpKeyid & 0x03; /* cam_id0~3 8021x group key */
    ctrl = BIT(15) | BIT(6) | BIT(7) |
           (psec_info->dot118021XGrpPrivacy << 2) |
           psec_info->dot118021XGrpKeyid;
    ret = wf_mcu_set_sec_cam(pnic_info, cam_id, ctrl, pmac,
                             psec_info->dot118021XGrpKey[psec_info->dot118021XGrpKeyid].skey);

    return ret;
}

static int cfg80211_ap_set_encryption (nic_info_st *pnic_info,
                                       ieee_param *param, wf_u32 param_len)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    int res = 0;
    wf_u32 wep_key_idx, wep_key_len;
    wdn_net_info_st *pwdn_info = NULL;

    CFG80211_DBG();

    param->u.crypt.err = 0;
    param->u.crypt.alg[IEEE_CRYPT_ALG_NAME_LEN - 1] = '\0';

    if (param_len <
        WF_FIELD_SIZEOF(ieee_param, u.crypt.key) + param->u.crypt.key_len)
    {
        CFG80211_WARN("param_len invalid !!!!!!!");
        res = -EINVAL;
        goto exit;
    }

    CFG80211_DBG("sta_addr: "WF_MAC_FMT, WF_MAC_ARG(param->sta_addr));
    if (is_bcast_addr(param->sta_addr))
    {
        CFG80211_DBG("set with boardcast address");
        if (param->u.crypt.idx >= WEP_KEYS)
        {
            res = -EINVAL;
            goto exit;
        }
    }
    else
    {
        pwdn_info = wf_wdn_find_info(pnic_info, param->sta_addr);
        if (pwdn_info == NULL)
        {
            CFG80211_DBG("sta has already been removed or never been added");
            goto exit;
        }
    }

    /* clear default key before ap start work */
    if (!strcmp(param->u.crypt.alg, "none") && !pwdn_info)
    {
        psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
        psec_info->ndisencryptstatus = wf_ndis802_11EncryptionDisabled;
        psec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
        psec_info->dot118021XGrpPrivacy = _NO_PRIVACY_;
        CFG80211_DBG("clear default encryption keys, keyid=%d", param->u.crypt.idx);
        goto exit;
    }

    /* set group key(for wpa/wpa2) or default key(for wep) before establish */
    if (pwdn_info == NULL)
    {
        /* for wep key */
        if (!strcmp(param->u.crypt.alg, "WEP"))
        {
            CFG80211_DBG("crypt.alg = WEP");

            if (psec_info->wpa_unicast_cipher || psec_info->wpa_multicast_cipher ||
                psec_info->rsn_group_cipher || psec_info->rsn_pairwise_cipher)
            {
                CFG80211_WARN("wep no support 8021x !!!");
                res = -EINVAL;
                goto exit;
            }

            wep_key_idx = param->u.crypt.idx;
            wep_key_len = param->u.crypt.key_len;
            CFG80211_DBG("wep_key_idx=%d, len=%d\n", wep_key_idx,
                         wep_key_len);

            /* check key idex and key len */
            if (wep_key_idx >= WEP_KEYS || wep_key_len == 0)
            {
                res = -EINVAL;
                goto exit;
            }
            if (wep_key_len > 0)
            {
                wep_key_len = wep_key_len <= 5 ? 5 : 13; /* 5B for wep40 and 13B for wep104 */
            }

            /* TODO: tx=1, the key only used to encrypt data in data send process,
            that is to say no used for boradcast */

            if (psec_info->bWepDefaultKeyIdxSet == 0)
            {
                CFG80211_DBG("wep, bWepDefaultKeyIdxSet=0");
                /* update encrypt algorithm */
                psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;
                if (wep_key_len == 13)
                {
                    psec_info->dot11PrivacyAlgrthm = _WEP104_;
                    psec_info->dot118021XGrpPrivacy = _WEP104_;
                }
                else
                {
                    psec_info->dot11PrivacyAlgrthm = _WEP40_;
                    psec_info->dot118021XGrpPrivacy = _WEP40_;
                }
                psec_info->dot11PrivacyKeyIndex = wep_key_idx;
            }

            /* todo: force wep key id set to 0, other id value no used by STA */
            if (wep_key_idx == 0)
            {
                /* update default key(for wep) */
                CFG80211_DBG("crypt.key=");
                CFG80211_ARRAY((void *)param->u.crypt.key, wep_key_len);
                psec_info->dot11PrivacyKeyIndex = wep_key_idx;
                wf_memcpy(&psec_info->dot11DefKey[wep_key_idx].skey[0],
                          param->u.crypt.key, wep_key_len);
                psec_info->dot11DefKeylen[wep_key_idx] = wep_key_len;
            }

            goto exit;
        }

        /* for group key */
        if (!strcmp(param->u.crypt.alg, "TKIP"))
        {
            CFG80211_DBG("set group_key, TKIP");

            psec_info->dot118021XGrpPrivacy = _TKIP_;
            CFG80211_DBG("key_idx=%d", param->u.crypt.idx);
            CFG80211_ARRAY(param->u.crypt.key, param->u.crypt.key_len);

            /* KCK PTK0~127 */
            psec_info->dot118021XGrpKeyid = param->u.crypt.idx;
            wf_memcpy(psec_info->dot118021XGrpKey[param->u.crypt.idx].skey,
                      param->u.crypt.key, WF_MIN(param->u.crypt.key_len, 16));
            /* set mic key */
            /* KEK PTK128~255 */
            wf_memcpy(psec_info->dot118021XGrptxmickey[param->u.crypt.idx].skey,
                      &param->u.crypt.key[16], 8); /* PTK128~191 */
            wf_memcpy(psec_info->dot118021XGrprxmickey[param->u.crypt.idx].skey,
                      &param->u.crypt.key[24], 8); /* PTK192~255 */

            psec_info->busetkipkey = wf_true;
        }
        else if (!strcmp(param->u.crypt.alg, "CCMP"))
        {
            CFG80211_DBG("set group_key, CCMP");
            psec_info->dot118021XGrpPrivacy = _AES_;
            /* KCK PTK0~127 */
            psec_info->dot118021XGrpKeyid = param->u.crypt.idx;
            CFG80211_DBG("set group_key id(%d), CCMP", psec_info->dot118021XGrpKeyid);
            wf_memcpy(psec_info->dot118021XGrpKey[param->u.crypt.idx].skey,
                      param->u.crypt.key, WF_MIN(param->u.crypt.key_len, 16));
            cfg80211_ap_hw_set_group_key(pnic_info, param->sta_addr);
        }
        else
        {
            CFG80211_DBG("set group_key, none");
            goto exit;
        }

        psec_info->dot11PrivacyAlgrthm = psec_info->dot118021XGrpPrivacy;
        psec_info->binstallGrpkey = wf_true;

        /* set boardcast wdn */
        pwdn_info = wf_wdn_find_info(pnic_info, param->sta_addr);
        if (pwdn_info)
        {
            pwdn_info->dot118021XPrivacy = psec_info->dot118021XGrpPrivacy;
            pwdn_info->ieee8021x_blocked = wf_false;
        }

        goto exit;
    }

    /* set key(for wpa/wpa2) after establish */
    else if (psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X)
    {
        if (param->u.crypt.set_tx == 1)
        {
            CFG80211_DBG("set unicastkey");

            wf_memcpy(pwdn_info->dot118021x_UncstKey.skey, param->u.crypt.key,
                      WF_MIN(param->u.crypt.key_len, 16));

            if (!strcmp(param->u.crypt.alg, "TKIP"))
            {
                CFG80211_DBG("set pairwise key, TKIP");
                pwdn_info->dot118021XPrivacy = _TKIP_;
                /* set mic key */
                wf_memcpy(pwdn_info->dot11tkiptxmickey.skey,
                          &param->u.crypt.key[16], 8);
                wf_memcpy(pwdn_info->dot11tkiprxmickey.skey,
                          &param->u.crypt.key[24], 8);
                psec_info->busetkipkey = wf_true;
            }
            else if (!strcmp(param->u.crypt.alg, "CCMP"))
            {
                CFG80211_DBG("set pairwise key, CCMP");
                pwdn_info->dot118021XPrivacy = _AES_;
                /* enable hardware encrypt */
                cfg80211_ap_hw_set_unicast_key(pnic_info, pwdn_info);
                wf_mcu_set_dk_cfg(pnic_info, psec_info->dot11AuthAlgrthm, wf_true);
                wf_mcu_set_on_rcr_am(pnic_info, wf_true);
            }
            else if (!strcmp(param->u.crypt.alg, "none"))
            {
                CFG80211_DBG("crypt.alo: none");
                pwdn_info->dot118021XPrivacy = _NO_PRIVACY_;
            }
        }

        pwdn_info->ieee8021x_blocked = wf_false;
    }

exit:
    return res;
}
#endif

/* dk:      default key for wep
   grpk:    group key for tkip/ccmp
   unik:    unnicast key for tkip/ccmp

    |-------cam_id------|
    |0~3            4~31|
     ________WEP________
STA  dk              -
AP   dk              -
     _____TKIP/CCMP_____
STA  grpk           unik
AP   grpk           unik
*/

int cfg80211_sta_hw_set_unicast_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_u8 cam_id;
    wf_u16 ctrl;
    int ret;

    cam_id = 4 + pwdn_info->wdn_id;
    ctrl = BIT(15) | (pwdn_info->dot118021XPrivacy << 2);
    ret = wf_mcu_set_sec_cam(pnic_info,
                             cam_id, ctrl,
                             pwdn_info->mac, pwdn_info->dot118021x_UncstKey.skey);

    return ret;
}

int cfg80211_sta_hw_set_group_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u8 cam_id;
    wf_u16 ctrl;
    int ret;

    cam_id = psec_info->dot118021XGrpKeyid & 0x03; /* cam_id0~3 8021x group key */
    ctrl = BIT(15) | BIT(6) |
           (psec_info->dot118021XGrpPrivacy << 2) |
           psec_info->dot118021XGrpKeyid;
    ret = wf_mcu_set_sec_cam(pnic_info, cam_id, ctrl,
                             pwdn_info->bssid,
                             psec_info->dot118021XGrpKey[psec_info->dot118021XGrpKeyid].skey);
    wf_mcu_set_on_rcr_am(pnic_info, wf_true);

    return ret;
}

static int cfg80211_sta_set_encryption(struct net_device *dev,
                                       ieee_param *param, wf_u32 param_len)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;
    wdn_net_info_st *pwdn_info;
    int res = 0;

    CFG80211_DBG();

    param->u.crypt.err = 0;
    param->u.crypt.alg[IEEE_CRYPT_ALG_NAME_LEN - 1] = '\0';

    if (param_len !=
        WF_OFFSETOF(ieee_param, u.crypt.key) + param->u.crypt.key_len)
    {
        CFG80211_WARN("param_len invalid !!!!!!!");
        res = -EINVAL;
        goto exit;
    }

    if (is_bcast_addr(param->sta_addr))
    {
        if (param->u.crypt.idx >= WEP_KEYS)
        {
            res = -EINVAL;
            goto exit;
        }
    }
    else
    {
        res = -EINVAL;
        goto exit;
    }

    if (psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) /* 802_1x */
    {
        local_info_st *plocal_info = pnic_info->local_info;
        if (plocal_info->work_mode == WF_INFRA_MODE) /* sta mode */
        {
            pwdn_info = wf_wdn_find_info(pnic_info,
                                         wf_wlan_get_cur_bssid(pnic_info));
            if (pwdn_info == NULL)
            {
                CFG80211_WARN("pwdn_info NULL !!!!!!");
                goto exit;
            }

            if (strcmp(param->u.crypt.alg, "none") != 0)
            {
                pwdn_info->ieee8021x_blocked = wf_false;
            }

            if (psec_info->ndisencryptstatus == wf_ndis802_11Encryption2Enabled ||
                psec_info->ndisencryptstatus == wf_ndis802_11Encryption3Enabled)
            {
                pwdn_info->dot118021XPrivacy = psec_info->dot11PrivacyAlgrthm;
            }
            CFG80211_DBG("pwdn_info->dot118021XPrivacy = %d", pwdn_info->dot118021XPrivacy);

            wf_mcu_set_sec_cfg(pnic_info, psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X ? 0xcf : 0xcc);

            /* PTK: param->u.crypt.key */
            if (param->u.crypt.set_tx == 1) /* pairwise key */
            {
                CFG80211_DBG("set unicastkey");
                /* KCK PTK0~127 */
                memcpy(pwdn_info->dot118021x_UncstKey.skey, param->u.crypt.key,
                       min_t(u16, param->u.crypt.key_len, 16));

                if (strcmp(param->u.crypt.alg, "TKIP") == 0) /* set mic key */
                {
                    /* KEK PTK128~255 */
                    memcpy(pwdn_info->dot11tkiptxmickey.skey,
                           &(param->u.crypt.key[16]), 8); /* PTK128~191 */
                    memcpy(pwdn_info->dot11tkiprxmickey.skey,
                           &(param->u.crypt.key[24]), 8); /* PTK192~255 */
                    psec_info->busetkipkey = wf_true;
                }
                if (pwdn_info->dot118021XPrivacy == _AES_)
                {
                    CFG80211_DBG("sta_hw_set_unicast_key");
                    cfg80211_sta_hw_set_unicast_key(pnic_info, pwdn_info);
                }
            }
            else /* group key */
            {
                CFG80211_DBG("set groupkey");
                memcpy(psec_info->dot118021XGrpKey[param->u.crypt.idx].skey,
                       param->u.crypt.key,
                       min_t(u16, param->u.crypt.key_len, 16));
                memcpy(psec_info->dot118021XGrptxmickey[param->u.crypt.idx].skey,
                       &param->u.crypt.key[16], 8);
                memcpy(psec_info->dot118021XGrprxmickey[param->u.crypt.idx].skey,
                       &param->u.crypt.key[24], 8);
                psec_info->binstallGrpkey = true;
                psec_info->dot118021XGrpKeyid = param->u.crypt.idx;
                if (psec_info->dot118021XGrpPrivacy == _AES_)
                {
                    CFG80211_DBG("sta_hw_set_group_key");
                    cfg80211_sta_hw_set_group_key(pnic_info, pwdn_info);
                }

                if (wf_p2p_is_valid(pnic_info))
                {
                    p2p_info_st *p2p_info = pnic_info->p2p;
                    CFG80211_INFO("state:%s", wf_p2p_state_to_str(p2p_info->p2p_state));
                    if (p2p_info->p2p_state == P2P_STATE_GONEGO_OK)
                    {
                        wf_p2p_set_state(p2p_info, P2P_STATE_EAPOL_DONE);
                        wf_p2p_nego_timer_set(pnic_info, P2P_EAPOL_NEGO_TIME);
                    }
                }

            }
        }
    }

exit:
    return res;
}

void wf_cfg80211_wiphy_unreg (nic_info_st *pnic_info)
{
    CFG80211_INFO("wiphy unregiester");
    wiphy_unregister(pnic_info->pwiphy);
}

void wf_cfg80211_wiphy_free (nic_info_st *pnic_info)
{
    struct wiphy *pwiphy;

    if (NULL == pnic_info)
    {
        return;
    }

    pwiphy = pnic_info->pwiphy;
    if (NULL == pwiphy)
    {
        return;
    }

    wiphy_free(pwiphy);
}

void wf_cfg80211_widev_free (nic_info_st *pnic_info)
{
    struct wireless_dev *pwidev;

    if (NULL == pnic_info)
    {
        return;
    }

    pwidev = pnic_info->pwidev;
    if (pwidev)
    {
        wf_kfree(pwidev);
    }
}

static inline void set_wiphy_pirv (struct wiphy *pwiphy, void *priv)
{
    *(void **)wiphy_priv(pwiphy) = priv;
}

static int _add_key_cb(struct wiphy *wiphy, struct net_device *ndev,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
                       wf_u8 key_index, bool pairwise,
                       const wf_u8 * mac_addr,
#else
                       wf_u8 key_index, const wf_u8 * mac_addr,
#endif
                       struct key_params *params)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    char *alg_name;
    wf_u32 param_len;
    ieee_param *param = NULL;
    int res = 0;

    CFG80211_INFO("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
    CFG80211_DBG("pairwise=%d\n", pairwise);
#endif

    param_len = WF_OFFSETOF(ieee_param, u.crypt.key) + params->key_len;
    param = (ieee_param *)wf_vmalloc(param_len);
    if (param == NULL)
    {
        res = -EPERM;
        goto exit;
    }
    memset(param, 0, param_len);

    memset(param->sta_addr, 0xff, ETH_ALEN);

    switch (params->cipher)
    {
        case IW_AUTH_CIPHER_NONE:
            alg_name = "none";
            break;

        case WLAN_CIPHER_SUITE_WEP40:
        case WLAN_CIPHER_SUITE_WEP104:
            alg_name = "WEP";
            break;

        case WLAN_CIPHER_SUITE_TKIP:
            alg_name = "TKIP";
            break;

        case WLAN_CIPHER_SUITE_CCMP:
            alg_name = "CCMP";
            break;

        default:
            res = -ENOTSUPP;
            goto exit;
    }

    strncpy((char *)param->u.crypt.alg, alg_name, IEEE_CRYPT_ALG_NAME_LEN);
    CFG80211_DBG("alg_name=%s", alg_name);

    if (mac_addr == NULL || is_bcast_addr(mac_addr))
    {
        param->u.crypt.set_tx = 0;
    }
    else
    {
        param->u.crypt.set_tx = 1;
    }

    param->u.crypt.idx = key_index;
    CFG80211_DBG("crypt.idx=%d", param->u.crypt.idx);

    if (params->seq_len && params->seq)
    {
        wf_memcpy(param->u.crypt.seq, (wf_u8 *) params->seq, params->seq_len);
    }

    if (params->key_len && params->key)
    {
        param->u.crypt.key_len = params->key_len;
        wf_memcpy(param->u.crypt.key, (wf_u8 *) params->key, params->key_len);
    }
    if (wf_local_cfg_get_work_mode(pnic_info) == WF_INFRA_MODE)
    {
        res = cfg80211_sta_set_encryption(ndev, param, param_len);
    }
#ifdef CFG_ENABLE_AP_MODE
    else if (wf_local_cfg_get_work_mode(pnic_info) == WF_MASTER_MODE)
    {
        if (mac_addr)
        {
            wf_memcpy(param->sta_addr, (void *)mac_addr, ETH_ALEN);
        }
        res = cfg80211_ap_set_encryption(pnic_info, param, param_len);
    }
#endif
    else
    {
        CFG80211_WARN("mode error!");
    }

exit :
    if (param)
    {
        wf_vfree((wf_u8 *)param);
    }

    return res;

}

static int _get_key_cb(struct wiphy *wiphy, struct net_device *ndev,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
                       wf_u8 key_index, bool pairwise,
                       const wf_u8 * mac_addr,
#else
                       wf_u8 key_index, const wf_u8 * mac_addr,
#endif
                       void *cookie,
                       void (*callback) (void *cookie,
                               struct key_params *))
{
    if (mac_addr)
    {
        CFG80211_INFO("mac addr: "WF_MAC_FMT, WF_MAC_ARG(mac_addr));
    }
    else
    {
        CFG80211_INFO();
    }
    return 0;
}


static int _del_key_cb(struct wiphy *wiphy, struct net_device *ndev,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
                       wf_u8 key_index, bool pairwise,
                       const wf_u8 * mac_addr)
#else
                       wf_u8 key_index, const wf_u8 * mac_addr)
#endif
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;

    CFG80211_INFO("key_index = %d", key_index);
    CFG80211_INFO("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    if (key_index == psec_info->dot11PrivacyKeyIndex)
    {
        psec_info->bWepDefaultKeyIdxSet = 0;
    }
    return 0;
}


static int _set_default_key_cb(struct wiphy *wiphy,
                               struct net_device *ndev, wf_u8 key_index
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)) || defined(COMPAT_KERNEL_RELEASE)
                               , bool unicast, bool multicast
#endif
                              )
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;

    CFG80211_INFO("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    if (key_index < WEP_KEYS &&
        (psec_info->dot11PrivacyAlgrthm == _WEP40_ ||
         psec_info->dot11PrivacyAlgrthm == _WEP104_))
    {
        psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;

        psec_info->dot11PrivacyKeyIndex = key_index;

        psec_info->dot11PrivacyAlgrthm = _WEP40_;
        psec_info->dot118021XGrpPrivacy = _WEP40_;
        if (psec_info->dot11DefKeylen[key_index] == 13)
        {
            psec_info->dot11PrivacyAlgrthm = _WEP104_;
            psec_info->dot118021XGrpPrivacy = _WEP104_;
        }

        psec_info->bWepDefaultKeyIdxSet = 1;
    }

    return 0;
}


static int _cfg80211_get_station(struct wiphy *wiphy,
                                 struct net_device *ndev,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 16, 0))
                                 wf_u8 * mac,
#else
                                 const wf_u8 * mac,
#endif
                                 struct station_info *sinfo)
{
    ndev_priv_st *pndev_priv        = NULL;
    nic_info_st *pnic_info          = NULL;
    wdn_net_info_st *pwdn_net_info  = NULL;
    wf_u8 qual, level;
    wf_u16 max_rate = 0;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(mac));
    if (NULL == mac)
    {
        CFG80211_DBG("mac is null");
        return -ENOENT;
    }
    pndev_priv = netdev_priv(ndev);
    if (NULL == pndev_priv)
    {
        CFG80211_DBG("pndev_priv is null");
        return -ENOENT;
    }

    pnic_info = pndev_priv->nic;
    if (NULL == pnic_info)
    {
        CFG80211_DBG("pnic_info is null");
        return -ENOENT;
    }

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    pwdn_net_info = wf_wdn_find_info(pnic_info, (wf_u8 *)mac);
    if (NULL == pwdn_net_info)
    {
        return -ENOENT;
    }

    wf_wlan_get_signal_and_qual(pnic_info, &qual, &level);
    wf_wlan_get_max_rate(pnic_info, (wf_u8*)mac, &max_rate);

    sinfo->filled = 0;

    sinfo->filled |= STATION_INFO_SIGNAL;
    sinfo->signal = translate_percentage_to_dbm(level);

    sinfo->filled |= STATION_INFO_TX_BITRATE;
    sinfo->txrate.legacy = max_rate;

    sinfo->filled |= STATION_INFO_RX_PACKETS;
    sinfo->rx_packets = pwdn_net_info->wdn_stats.rx_pkts;

    sinfo->filled |= STATION_INFO_TX_PACKETS;
    sinfo->tx_packets = pwdn_net_info->wdn_stats.tx_pkts;

    sinfo->filled |= STATION_INFO_TX_FAILED;
    sinfo->tx_failed = pwdn_net_info->wdn_stats.tx_drops;

    return 0;
}



static int _cfg80211_change_iface(struct wiphy *wiphy,
                                  struct net_device *ndev,
                                  enum nl80211_iftype type,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0))
                                  wf_u32 *flags,
#endif
                                  struct vif_params *params)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
#ifdef CFG_ENABLE_AP_MODE
    sec_info_st *psec_info = pnic_info->sec_info;
#endif
    struct wireless_dev *pwidev = pnic_info->pwidev;
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    enum nl80211_iftype old_type;
    sys_work_mode_e network_type = WF_INFRA_MODE;
    wf_u8 change;
    p2p_info_st *p2p_info   = pnic_info->p2p;

#ifdef CFG_ENABLE_ADHOC_MODE
    wf_bool bConnected;
    wf_bool bAdhocMaster;
#endif
    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

#ifdef CFG_ENABLE_AP_MODE
    psec_info->wpa_unicast_cipher = 0;
    psec_info->wpa_multicast_cipher = 0;
    psec_info->rsn_group_cipher = 0;
    psec_info->rsn_pairwise_cipher = 0;
#endif

    if (WF_CANNOT_RUN(pnic_info))
    {
        return -1;
    }

    if (ndev_open(ndev) != 0)
    {
        return -1;
    }

#ifdef CONFIG_LPS
    if (WF_RETURN_FAIL == wf_lps_wakeup(pnic_info, LPS_CTRL_SCAN, 0))
    {
        return -1;
    }
#endif

    old_type = pwidev->iftype;
    if (old_type != type)
    {
        change = wf_true;
        pmlme_info->action_public_dialog_token = 0xff;
        pmlme_info->action_public_rxseq = 0xffff;
    }
    CFG80211_INFO("old_type:%d, type:%d", old_type, type);

    ndev->type = ARPHRD_ETHER;

    if (type == NL80211_IFTYPE_MONITOR)
    {
    }
    else if (old_type == NL80211_IFTYPE_MONITOR)
    {
    }

    switch(type)
    {
        case NL80211_IFTYPE_ADHOC:
            network_type = WF_ADHOC_MODE;
            break;

        case NL80211_IFTYPE_STATION:
            network_type = WF_INFRA_MODE;
            if (wf_p2p_is_valid(pnic_info))
            {
                CFG80211_INFO("DRIVER_CFG80211: %s", wf_p2p_role_to_str(p2p_info->role));
                wf_p2p_reset( pnic_info);
                if (change && P2P_ROLE_GO == p2p_info->role)
                {
                    wf_p2p_set_role(p2p_info, P2P_ROLE_DEVICE);
                    wf_p2p_set_state(p2p_info, p2p_info->pre_p2p_state);
                    CFG80211_INFO("role=%d, p2p_state=%d, pre_p2p_state=%d\n",
                                  p2p_info->role, p2p_info->p2p_state, p2p_info->pre_p2p_state);
                }

#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE))
                if (p2p_info->role == P2P_ROLE_CLIENT)
                {
                    wf_p2p_set_role(p2p_info, P2P_ROLE_DEVICE);
                }

#endif

            }
            break;
        case NL80211_IFTYPE_AP:
            network_type = WF_MASTER_MODE;
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE))
            if (wf_p2p_is_valid(pnic_info))
            {
                CFG80211_INFO("DRIVER_CFG80211, %s", wf_p2p_role_to_str(p2p_info->role));
                if (change && p2p_info->p2p_state != P2P_STATE_NONE)
                {
                    wf_p2p_set_role(p2p_info, P2P_ROLE_GO);
                }

            }
#endif
            wf_mlme_abort(pnic_info);
            break;
        case NL80211_IFTYPE_MONITOR:
            network_type = WF_MONITOR_MODE;
            ndev->type = ARPHRD_IEEE80211_RADIOTAP;
            break;
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE))
        case NL80211_IFTYPE_P2P_CLIENT:
            CFG80211_INFO("NL80211_IFTYPE_P2P_CLIENT");
            network_type = WF_INFRA_MODE;
            if (wf_p2p_is_valid(pnic_info))
            {
                CFG80211_INFO("DRIVER_CFG80211, %s", wf_p2p_role_to_str(p2p_info->role));
                if (change && P2P_ROLE_GO == p2p_info->role)
                {
                    wf_p2p_set_role(p2p_info, P2P_ROLE_DEVICE);
                    wf_p2p_set_state(p2p_info, p2p_info->pre_p2p_state);
                    CFG80211_INFO("%s, role=%d, p2p_state=%d, pre_p2p_state=%d\n", __func__, p2p_info->role, p2p_info->p2p_state, p2p_info->pre_p2p_state);
                }
                wf_p2p_set_role(p2p_info, P2P_ROLE_CLIENT);

            }
            break;
        case NL80211_IFTYPE_P2P_GO:
        {
            CFG80211_INFO("NL80211_IFTYPE_P2P_GO, %s", wf_p2p_role_to_str(p2p_info->role));
            network_type = WF_MASTER_MODE;

            if (change && p2p_info->p2p_state != P2P_STATE_NONE)
            {
                wf_p2p_set_role(p2p_info, P2P_ROLE_GO);
            }

        }
        break;
#endif

        default:
            CFG80211_ERROR("op type error");
            break;
    }
    pwidev->iftype = type;

#ifdef CFG_ENABLE_ADHOC_MODE
    if ( NL80211_IFTYPE_ADHOC == old_type && NL80211_IFTYPE_ADHOC != type)
    {
        wf_mlme_get_connect(pnic_info, &bConnected);
        bAdhocMaster = wf_get_adhoc_master(pnic_info);
        if (bConnected && bAdhocMaster)
        {
            wf_adhoc_leave_ibss_msg_send(pnic_info);
            wf_yield();
        }
        wf_adhoc_flush_all_resource(pnic_info, network_type);
    }
#endif
    CFG80211_DBG("mode == %d", type);
    set_sys_work_mode(pnic_info, network_type);
    wf_mcu_set_op_mode(pnic_info, network_type);
    return 0;
}

void wf_cfg80211_scan_done_event_up (nic_info_st *pnic_info, wf_bool babort)
{
    struct wf_widev_priv *pwdev_info = pnic_info->widev_priv;
#if (KERNEL_VERSION(4, 7, 0) <= LINUX_VERSION_CODE)
    struct cfg80211_scan_info info;

    memset(&info, 0, sizeof(info));
    info.aborted = babort;
#endif
    wf_lock_lock(&pwdev_info->scan_req_lock);
    if (pwdev_info->pscan_request != NULL)
    {
        CFG80211_DBG("with scan req");
        if (pwdev_info->pscan_request->wiphy == pnic_info->pwiphy)
        {
#if (KERNEL_VERSION(4, 7, 0) <= LINUX_VERSION_CODE)
            cfg80211_scan_done(pwdev_info->pscan_request, &info);
#else
            cfg80211_scan_done(pwdev_info->pscan_request, babort);
#endif
        }
        else
        {
            CFG80211_WARN("wrong compare");
        }
        pwdev_info->pscan_request = NULL;
    }
    else
    {
        CFG80211_DBG("scan req is NULL");
    }
    wf_lock_unlock(&pwdev_info->scan_req_lock);
}

static wf_u64 cfg80211_get_timestamp_us(void)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0))
    return ktime_to_us(ktime_get_boottime());
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39))
    struct timespec ts;
    get_monotonic_boottime(&ts);
    return ((wf_u64) ts.tv_sec * 1000000) + ts.tv_nsec / 1000;
#else
    struct timeval tv;
    do_gettimeofday(&tv);
    return ((wf_u64) tv.tv_sec * 1000000) + tv.tv_usec;
#endif
}

#define MAX_BSSINFO_LEN 1000
struct cfg80211_bss *inform_bss (nic_info_st *pnic_info,
                                 wf_wlan_mgmt_scan_que_node_t *pscan_que_node)
{
    wf_u8 *pbuf;
    wf_u16 frame_len;
    wf_80211_frame_e frame_type;
    wf_80211_mgmt_t *pframe;
    struct ieee80211_channel *channel;
    wf_s32 signal_dbm;
    struct cfg80211_bss *pbss = NULL;
    wf_u8 bc_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    struct wireless_dev *pwdev = pnic_info->pwidev;
    struct wiphy *pwiphy = pwdev->wiphy;

    pbuf = wf_kzalloc(MAX_BSSINFO_LEN);
    if (pbuf == NULL)
    {
        CFG80211_WARN("buffer alloc failed!");
        return pbss;
    }
    wf_memset(pbuf, 0, MAX_BSSINFO_LEN);

    frame_len = WF_OFFSETOF(wf_80211_mgmt_t, beacon) + pscan_que_node->ie_len;
    if (frame_len > MAX_BSSINFO_LEN)
    {
        CFG80211_WARN("ie_length is too long");
        goto exit;
    }

    /**
     * make frame
     */
    pframe = (void *)pbuf;
    /* frame control */
    pframe->frame_control = 0;
    /* frame type */
    frame_type = pscan_que_node->frame_type;
    wf_80211_set_frame_type(&pframe->frame_control, frame_type);
    /* address */
    wf_memcpy(pframe->da,
              frame_type == WF_80211_FRM_BEACON ?
              bc_addr : nic_to_local_addr(pnic_info),
              ETH_ALEN);
    wf_memcpy(pframe->sa, pscan_que_node->bssid, ETH_ALEN);
    wf_memcpy(pframe->bssid, pscan_que_node->bssid, ETH_ALEN);
    /* sequence control */
    pframe->seq_ctrl = 0;
    /* element */
    wf_memcpy(&pframe->beacon, pscan_que_node->ies, pscan_que_node->ie_len);
    /* if the beacon frame is send from a hidden ap, therefore should convert
    frame type to probe respone with valid ssid information. */
    if (pscan_que_node->frame_type == WF_80211_FRM_BEACON &&
        pscan_que_node->ssid_type != WF_80211_HIDDEN_SSID_NOT_IN_USE &&
        pscan_que_node->ssid.length)
    {
        wf_80211_mgmt_ie_t *pie_ssid;
        int ret;
        ret = wf_80211_mgmt_ies_search(pframe->beacon.variable,
                                       pscan_que_node->ie_len,
                                       WF_80211_MGMT_EID_SSID,
                                       &pie_ssid);
        if (ret)
        {
            CFG80211_ERROR("no ssid element!!!");
            goto exit;
        }
        if (pscan_que_node->ssid_type == WF_80211_HIDDEN_SSID_ZERO_LEN)
        {
            wf_u8 *m_start, *m_pos;
            m_start = pie_ssid->data;
            m_pos = &pframe->beacon.variable[pscan_que_node->ie_len];
            while (m_start < m_pos)
            {
                --m_pos;
                m_pos[pscan_que_node->ssid.length] = m_pos[0];
            }
            pie_ssid->len = pscan_que_node->ssid.length;
            frame_len += pie_ssid->len;
        }
        wf_memcpy(pie_ssid->data,
                  pscan_que_node->ssid.data,
                  pscan_que_node->ssid.length);
    }
    /* timestamp */
    pframe->beacon.timestamp = cpu_to_le64(cfg80211_get_timestamp_us());

    channel =
        ieee80211_get_channel(pwiphy, wf_ch_2_freq((int)pscan_que_node->channel));
    signal_dbm = translate_percentage_to_dbm(pscan_que_node->signal_strength) * 100; /* mBm (100*dBm) */
    pbss = cfg80211_inform_bss_frame(pwiphy, channel,
                                     (void *)pframe, frame_len,
                                     signal_dbm, GFP_ATOMIC);
    if (unlikely(!pbss))
    {
        CFG80211_WARN("pbss is NULL %s channel %d", pscan_que_node->ssid.data, pscan_que_node->channel);
        goto exit;
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38) && !defined COMPAT_KERNEL_RELEASE
    if (frame_type == WF_80211_FRM_BEACON)
    {
        if (pbss->len_information_elements != pbss->len_beacon_ies)
        {
            pbss->information_elements = pbss->beacon_ies;
            pbss->len_information_elements = pbss->len_beacon_ies;
        }
    }
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
    cfg80211_put_bss(pwiphy, pbss);
#else
    cfg80211_put_bss(pbss);
#endif

exit:
    if (pbuf)
    {
        wf_kfree(pbuf);
    }

    return pbss;
}

int wf_cfg80211_inform_check_bss(nic_info_st *pnic_info)
{
    wf_bool privacy;
    wf_wlan_mgmt_info_t *wlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &wlan_mgmt_info->cur_network;
    struct cfg80211_bss *bss = NULL;
    struct ieee80211_channel *channel = NULL;
    struct wireless_dev *pwdev = pnic_info->pwidev;
    struct wiphy *pwiphy = pwdev->wiphy;

    privacy = !!(pcur_network->cap_info & WF_80211_MGMT_CAPAB_PRIVACY);
    channel = ieee80211_get_channel(pwiphy, wf_ch_2_freq((int)pcur_network->channel));
    bss = cfg80211_get_bss(pwiphy, channel,
                           pcur_network->bssid, pcur_network->ssid.data,
                           pcur_network->ssid.length,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
                           (WF_80211_CAPAB_IS_IBSS(pcur_network->cap_info) ?
                            IEEE80211_BSS_TYPE_IBSS : IEEE80211_BSS_TYPE_ESS),
                           IEEE80211_PRIVACY(privacy));
#else
                           WF_80211_MGMT_CAPAB_ESS,
                           WF_80211_MGMT_CAPAB_IBSS);
#endif


#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
    cfg80211_put_bss(pwiphy, bss);
#else
    cfg80211_put_bss(bss);
#endif

    return (bss != NULL);
}

#ifdef CFG_ENABLE_ADHOC_MODE
void wf_cfg80211_unlink_ibss(nic_info_st *pnic_info)
{
    wf_bool privacy;
    wf_wlan_mgmt_info_t *wlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &wlan_mgmt_info->cur_network;
    struct cfg80211_bss *bss = NULL;
    struct ieee80211_channel *channel = NULL;
    struct wireless_dev *pwdev = pnic_info->pwidev;
    struct wiphy *pwiphy = pwdev->wiphy;

    privacy = !!(pcur_network->cap_info & WF_80211_MGMT_CAPAB_PRIVACY);
    channel = ieee80211_get_channel(pwiphy, wf_ch_2_freq((int)pcur_network->channel));
    bss = cfg80211_get_bss(pwiphy, channel,
                           pcur_network->bssid, pcur_network->ssid.data,
                           pcur_network->ssid.length,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
                           (WF_80211_CAPAB_IS_IBSS(pcur_network->cap_info) ?
                            IEEE80211_BSS_TYPE_IBSS : IEEE80211_BSS_TYPE_ESS),
                           IEEE80211_PRIVACY(privacy));
#else
                           WF_80211_MGMT_CAPAB_ESS,
                           WF_80211_MGMT_CAPAB_IBSS);
#endif

    if (bss)
    {
        cfg80211_unlink_bss(pwiphy, bss);
        CFG80211_INFO("%s(): cfg80211_unlink %s!! () ", __func__,
                      pcur_network->ssid.data);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
        cfg80211_put_bss(pwiphy, bss);
#else
        cfg80211_put_bss(bss);
#endif
    }
    return;
}

void wf_cfg80211_ibss_indicate_connect (nic_info_st *pnic_info)
{
    struct wireless_dev *pwdev = (struct wireless_dev *)pnic_info->pwidev;
    wf_wlan_mgmt_info_t *wlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &wlan_mgmt_info->cur_network;
    wf_wlan_mgmt_scan_que_node_t *pscan_que_node = NULL;
    wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0))
    struct wiphy *wiphy = pwdev->wiphy;
    wf_u32 freq = 2412;
    struct ieee80211_channel *notify_channel;
#endif

    CFG80211_DBG();

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0))
    freq = wf_ch_2_freq((int)pcur_network->channel);
    LOG_D("freq = %d", freq);
#endif
    pwdev->iftype = NL80211_IFTYPE_ADHOC;

    if (wf_local_cfg_get_work_mode(pnic_info) != WF_MONITOR_MODE)
    {
        wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
        {
            if (!wf_memcmp(pscan_que_node->bssid, pcur_network->bssid, ETH_ALEN) &&
                !wf_memcmp(pscan_que_node->ssid.data, pcur_network->ssid.data, pcur_network->ssid.length))
            {
                CFG80211_DBG("INFORM BSS before event up, ssid %s, ssid %s", pscan_que_node->ssid.data,
                             pcur_network->ssid.data);
                inform_bss(pnic_info, pscan_que_node);
            }
        }
        wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);
    }

    if (!wf_cfg80211_inform_check_bss(pnic_info))
    {
        CFG80211_DBG("bss not found!!");
    }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0))
    notify_channel = ieee80211_get_channel(wiphy, freq);
    cfg80211_ibss_joined(pnic_info->ndev, pcur_network->bssid,
                         notify_channel, GFP_ATOMIC);
#else
    cfg80211_ibss_joined(pnic_info->ndev, pcur_network->bssid,
                         GFP_ATOMIC);
#endif
}
#endif

void wf_cfg80211_indicate_connect (nic_info_st *pnic_info)
{
    struct wireless_dev *pwdev = (struct wireless_dev *)pnic_info->pwidev;
    wf_wlan_mgmt_info_t *wlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &wlan_mgmt_info->cur_network;
    wf_wlan_mgmt_scan_que_node_t *pscan_que_node = NULL;
    wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;

    CFG80211_DBG();

    if (pwdev->iftype != NL80211_IFTYPE_STATION
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
        && pwdev->iftype != NL80211_IFTYPE_P2P_CLIENT
#endif
       )
    {
        return;
    }

    if (wf_local_cfg_get_work_mode(pnic_info) == WF_MASTER_MODE)
    {
        return;
    }

#ifdef CFG_ENABLE_MONITOR_MODE
    if (wf_local_cfg_get_work_mode(pnic_info) != WF_MONITOR_MODE)
#endif
    {
        wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
        {
            if (!wf_memcmp(pscan_que_node->bssid, pcur_network->bssid, ETH_ALEN) &&
                !wf_memcmp(pscan_que_node->ssid.data, pcur_network->ssid.data, pcur_network->ssid.length))
            {
                CFG80211_INFO("INFORM BSS before event up, ssid %s, ssid %s, bss_ch:%d, scan_ch:%d, bssid:"WF_MAC_FMT,
                              pscan_que_node->ssid.data
                              ,pcur_network->ssid.data
                              ,pcur_network->channel
                              ,pscan_que_node->channel
                              ,WF_MAC_ARG(pscan_que_node->bssid)
                             );
                inform_bss(pnic_info, pscan_que_node);
            }
        }
        wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);
    }

    if (!wf_cfg80211_inform_check_bss(pnic_info))
    {
        CFG80211_INFO("bss not found!!");
    }
    if (wf_p2p_is_valid(pnic_info))
    {
        p2p_info_st *p2p_info           = pnic_info->p2p;
        wf_p2p_set_pre_state(p2p_info, p2p_info->p2p_state);
        wf_p2p_set_role(p2p_info, P2P_ROLE_CLIENT);
        wf_p2p_set_state(p2p_info, P2P_STATE_GONEGO_OK);
        if (p2p_info->go_negoing & WF_BIT(P2P_INVIT_RESP))
        {
            //p2p_info->go_negoing  = 0;
            wf_p2p_nego_timer_set(pnic_info, P2P_CONN_NEGO_TIME);
        }
        else if (p2p_info->go_negoing & WF_BIT(P2P_GO_NEGO_CONF))
        {
            wf_p2p_nego_timer_set(pnic_info, P2P_CONN_NEGO_TIME);
        }
        CFG80211_INFO("role=%s, p2p_state=%s, pre_p2p_state=%s\n",
                      wf_p2p_role_to_str(p2p_info->role), wf_p2p_state_to_str(p2p_info->p2p_state),
                      wf_p2p_state_to_str(p2p_info->pre_p2p_state));

    }
    CFG80211_INFO("req ie_len:%d, resp ie_len:%d",
                  pcur_network->assoc_req.ie_len, pcur_network->assoc_resp.ie_len);
    cfg80211_connect_result(pnic_info->ndev,
                            pcur_network->mac_addr,
                            pcur_network->assoc_req.ie,
                            pcur_network->assoc_req.ie_len,
                            pcur_network->assoc_resp.ie,
                            pcur_network->assoc_resp.ie_len,
                            WLAN_STATUS_SUCCESS, GFP_ATOMIC);
}

void wf_cfg80211_indicate_disconnect(nic_info_st *pnic_info)
{
    struct wireless_dev *pwdev = (struct wireless_dev *)pnic_info->pwidev;
    struct net_device *ndev = pnic_info->ndev;
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_mlme_conn_res_t *pconn_res = &pmlme_info->conn_res;

    if (pwdev->iftype != NL80211_IFTYPE_STATION
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
        && pwdev->iftype != NL80211_IFTYPE_P2P_CLIENT
#endif
       )
    {
        return;
    }

    if (wf_local_cfg_get_work_mode(pnic_info) == WF_MASTER_MODE)
    {
        return;
    }

    wf_os_api_disable_all_data_queue(ndev);
    if (wf_p2p_is_valid(pnic_info))
    {
        p2p_info_st *p2p_info = pnic_info->p2p;

        wf_p2p_set_state(p2p_info, p2p_info->pre_p2p_state);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
        if (pwdev->iftype != NL80211_IFTYPE_P2P_CLIENT)
#endif
            wf_p2p_set_role(p2p_info, P2P_ROLE_DEVICE);
        wf_p2p_nego_timer_set(pnic_info, P2P_CONN_NEGO_TIME);
        CFG80211_INFO("role=%s, p2p_state=%s, pre_p2p_state=%s\n",
                      wf_p2p_role_to_str(p2p_info->role), wf_p2p_state_to_str(p2p_info->p2p_state),
                      wf_p2p_state_to_str(p2p_info->pre_p2p_state));

    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0) || defined(COMPAT_KERNEL_RELEASE)
    CFG80211_DBG("pwdev->sme_state(b)=%d\n", pwdev->sme_state);

    if (pwdev->sme_state == CFG80211_SME_CONNECTING)
    {
        cfg80211_connect_result(ndev,
                                NULL,
                                NULL, 0,
                                NULL, 0,
                                WLAN_STATUS_UNSPECIFIED_FAILURE,
                                GFP_ATOMIC);
    }
    else if (pwdev->sme_state == CFG80211_SME_CONNECTED)
    {
#   if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 2, 0))
        cfg80211_disconnected(ndev,
                              pconn_res->reason_code,
                              NULL, 0,
                              pconn_res->local_disconn,
                              GFP_ATOMIC);
#   else
        cfg80211_disconnected(ndev, 0, NULL, 0, GFP_ATOMIC);
#   endif
    }

    CFG80211_DBG("pwdev->sme_state(a)=%d", pwdev->sme_state);
#else
    {
        wf_bool bConnect;
        wf_mlme_get_connect(pnic_info, &bConnect);
        CFG80211_INFO("call cfg80211_disconnected, reason:%d, local_generite=%d",
                      pconn_res->reason_code, pconn_res->local_disconn);

        if (bConnect == wf_true)
        {
#       if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 2, 0))
#           if 0
            if (!pconn_res->local_disconn)
            {
                cfg80211_disconnected(ndev, pconn_res->reason_code, NULL, 0, pconn_res->local_disconn, GFP_ATOMIC);
            }
            else
            {
                cfg80211_disconnected(ndev, 0, NULL, 0, pconn_res->local_disconn, GFP_ATOMIC);
            }
#           else
            cfg80211_disconnected(ndev,
                                  pconn_res->reason_code,
                                  NULL, 0,
                                  pconn_res->local_disconn,
                                  GFP_ATOMIC);
#endif
#       else
            cfg80211_disconnected(ndev, 0, NULL, 0, GFP_ATOMIC);
#       endif
        }
        else
        {
            cfg80211_connect_result(ndev,
                                    NULL,
                                    NULL, 0,
                                    NULL, 0,
                                    WLAN_STATUS_UNSPECIFIED_FAILURE,
                                    GFP_ATOMIC);
        }
    }
#endif
}
void wf_cfg80211_scan_complete(nic_info_st *pnic_info)
{
    wf_wlan_mgmt_scan_que_node_t *pscan_que_node = NULL;
    wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
//    wf_bool bConnect = wf_false;

    CFG80211_DBG("[%d] scan complete", pnic_info->ndev_id);

    wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
    {
        struct wf_widev_priv *pwdev_info = pnic_info->widev_priv;
        struct cfg80211_scan_request *req = pwdev_info->pscan_request;

        CFG80211_DBG(" pscan_que_node have results");

        /* checkout channel */
        if (req->n_channels)
        {
            wf_u8 i;
            for (i = 0; i < req->n_channels; i++)
            {
                if (req->channels[i]->hw_value == pscan_que_node->channel)
                {
                    break;
                }
            }
            if (i == req->n_channels)
            {
                continue;
            }
        }

        /* inform bss */
        inform_bss(pnic_info, pscan_que_node);
        CFG80211_DBG("%s"" "WF_MAC_FMT" %s",
                     pscan_que_node->frame_type == WF_80211_FRM_BEACON ? "BCN " : "PROB",
                     WF_MAC_ARG(pscan_que_node->bssid),
                     pscan_que_node->ssid.data);
    }
    wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);
}

int wf_cfg80211_p2p_cb_reg(nic_info_st *pnic_info)
{
    p2p_info_st *p2p_info = pnic_info->p2p;
    if (wf_false == p2p_info->scb.init_flag)
    {
        p2p_info->scb.remain_on_channel_expired = wf_cfg80211_remain_on_channel_expired;
        p2p_info->scb.rx_mgmt           = wf_cfg80211_p2p_rx_mgmt;
        p2p_info->scb.ready_on_channel  = wf_cfg80211_p2p_ready_on_channel;
        p2p_info->scb.init_flag = wf_true;
    }

    return 0;
}

static int cfg80211_p2p_nego_ctl_scan(nic_info_st *pnic_info, wf_u8 req_ch_nums, wf_u8 buddy_flag)
{
    p2p_info_st *p2p_info = NULL;
    if (NULL == pnic_info)
    {
        return 0;
    }

    p2p_info = pnic_info->p2p;
    if (wf_p2p_is_valid(pnic_info))
    {
        CFG80211_INFO("[%d] buddy:%d, nego:0x%x, scan_deny:%d",
                      pnic_info->ndev_id, buddy_flag, p2p_info->go_negoing, p2p_info->scan_deny);
        if (buddy_flag)
        {
            if (p2p_info->scan_deny)
            {
                return 1;
            }
            if (p2p_info->go_negoing)
            {
                if (0 == p2p_info->nego_timer_flag)
                {
                    p2p_info->nego_timer_flag = 1;

                }
                return 1;//scan_done = true
            }

        }
        else
        {
            if (p2p_info->scan_deny)
            {
                return 1;
            }
        }
    }

    return 0;
}
static int _call_scan_cb (struct wiphy *wiphy
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
                          , struct net_device *ndev
#endif
                          , struct cfg80211_scan_request *req)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
    struct net_device *ndev;
#endif
    int i, ret = 0;
    ndev_priv_st *pndev_priv;
    nic_info_st *pnic_info;
    mlme_info_t *pmlme_info;
    wf_scan_info_t *pscan_info;
    struct wf_widev_priv *pwdev_info;
    mlme_state_e state;
    wf_bool bConnect, bBusy;
    wf_bool scan_done = wf_false;
    wf_wlan_ssid_t ssids[WF_SCAN_REQ_SSID_NUM];
    wf_u8 ssid_num = 0;
    wf_u8 current_ch[WF_SCAN_REQ_CHANNEL_NUM];
    wf_u8 scan_time_for_one_ch = 6;
    wf_u8 scan_time = 1;
    wf_u8 social_channel = 0;
    wf_u8 req_ch_nums    = req->n_channels;

    CFG80211_DBG("scan start!");

    if (req == NULL)
    {
        ret = -EINVAL;
        return ret;
    }

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
    if (ndev == NULL)
    {
        ret = -EINVAL;
        return ret;
    }
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
    ndev = req->wdev->netdev;
#endif

    pndev_priv = netdev_priv(ndev);
    pnic_info = pndev_priv->nic;

    CFG80211_INFO("[%d] mac addr: "WF_MAC_FMT, pnic_info->ndev_id, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    pmlme_info = (mlme_info_t *)pnic_info->mlme_info;
    pscan_info = pnic_info->scan_info;
    pwdev_info = pnic_info->widev_priv;

    wf_lock_lock(&pwdev_info->scan_req_lock);
    pwdev_info->pscan_request = req;
    wf_lock_unlock(&pwdev_info->scan_req_lock);

    if (pnic_info->buddy_nic)
    {
        mlme_state_e state;
        wf_mlme_get_state((nic_info_st *)(pnic_info->buddy_nic), &state);
        if (state == MLME_STATE_CONN_SCAN ||
            state == MLME_STATE_AUTH ||
            state == MLME_STATE_ASSOC)
        {
            CFG80211_DBG("buddy interface is under linking !");
            scan_done = wf_true;
            goto exit;
        }
    }

    if (pwdev_info->block_scan == wf_true)
    {
        CFG80211_DBG("[%d] scan is blocked", pnic_info->ndev_id);
        scan_done = wf_true;
        goto exit;
    }

    if (req->ssids->ssid != NULL && 0 == wf_memcmp(req->ssids->ssid, "DIRECT-", 7)
        && wf_p2p_get_ie((wf_u8 *) req->ie, req->ie_len, NULL, NULL))
    {

        p2p_info_st *p2p_info = pnic_info->p2p;

        CFG80211_DBG(" p2p ie_len:%zu", req->ie_len);
        for(i=0; i<req_ch_nums; i++)
        {
            CFG80211_INFO("[%d] channel[%d]: hw_value[%d]", pnic_info->ndev_id, i, req->channels[i]->hw_value);
        }

        if (req->n_channels == 3 && req->channels[0]->hw_value == 1
            && req->channels[1]->hw_value == 6 && req->channels[2]->hw_value == 11)
        {
            social_channel = 1;
        }

        if (wf_p2p_is_valid(pnic_info))
        {
            if (cfg80211_p2p_nego_ctl_scan(pnic_info, req_ch_nums, wf_false))
            {
                scan_done = wf_true;
                goto exit;
            }
            wf_p2p_set_pre_state(p2p_info, p2p_info->p2p_state);

        }
        else
        {
            /*register callback function*/
            wf_cfg80211_p2p_cb_reg(pnic_info);
            wf_p2p_enable(pnic_info, P2P_ROLE_DEVICE);
        }

        wf_p2p_scan_entry(pnic_info, social_channel, (wf_u8*)req->ie, req->ie_len);

    }
    else if (cfg80211_p2p_nego_ctl_scan(pnic_info->buddy_nic, req_ch_nums, wf_true))
    {
        scan_done = wf_true;
        goto exit;
    }

    /* if traffic busy been detected, the current scan request should ignore,
    with the purpose of no interference traffic, unless timeout occurs from
    the start of traffic busy is detected. */
    wf_mlme_get_connect(pndev_priv->nic, &bConnect);
    if (bConnect == wf_true)
    {
        static wf_bool on_check = wf_false;
        static wf_timer_t timer;

        wf_mlme_get_traffic_busy(pndev_priv->nic, &bBusy);
        if (bBusy == wf_true)
        {
            if (!on_check)
            {
                on_check = wf_true;
                wf_timer_set(&timer, 12 * 1000);
            }
            if (wf_timer_expired(&timer))
            {
                wf_timer_restart(&timer);
            }
//            else
            {
                scan_done = wf_true;
                goto exit;
            }
        }
        else if (on_check)
        {
            on_check = wf_false;
        }
    }
    wf_mlme_get_connect(pndev_priv->nic->buddy_nic, &bConnect);
    if (bConnect == wf_true)
    {
        wf_mlme_get_traffic_busy(pndev_priv->nic->buddy_nic, &bBusy);
        if (bBusy == wf_true)
        {
            scan_done = wf_true;
            goto exit;
        }
    }

    wf_mlme_get_state(pnic_info, &state);
    if (state <= MLME_STATE_ASSOC)
    {
        scan_done = wf_true;
        goto exit;
    }

    CFG80211_DBG("req->n_ssids:%d", req->n_ssids);
    wf_memset(ssids, 0, sizeof(ssids));
    if (req->ssids)
    {
        for (i = 0; i < req->n_ssids && i < WF_ARRAY_SIZE(ssids); i++)
        {
            if (req->ssids[i].ssid_len)
            {
                CFG80211_DBG("ssid = %s", req->ssids[i].ssid);
                wf_memcpy(ssids[i].data, req->ssids[i].ssid, req->ssids[i].ssid_len);
                ssids[i].length = req->ssids[i].ssid_len;
            }
            else
            {
                break;
            }
        }
        ssid_num = i;
    }

    CFG80211_DBG("n_channels:%d", req->n_channels);
    wf_memset(current_ch, 0, sizeof(current_ch));
    if (req->channels[0])
    {
        for(i = 0; i < req->n_channels && i < WF_ARRAY_SIZE(current_ch); i++)
        {
            CFG80211_DBG("hw_value:%u flags:0x%08x",
                         req->channels[i]->hw_value, req->channels[i]->flags);
            current_ch[i] = req->channels[i]->hw_value;
        }
    }

    if (req->n_channels == 1)
    {
        for(i = 1; i < scan_time_for_one_ch; i++)
        {
            wf_memcpy(&current_ch[i], &current_ch[0], sizeof(current_ch[0]));
        }
        ret = wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                                 ssids, ssid_num,
                                 current_ch, scan_time_for_one_ch,
                                 WF_MLME_FRAMEWORK_NETLINK);
    }
    else if (req->n_channels <= 4)
    {
        wf_s8 j;
        for(j = req->n_channels - 1; j >= 0; j--)
        {
            for(i = 0; i < scan_time; i++)
            {
                wf_memcpy(&current_ch[j * scan_time + i], &current_ch[j],
                          sizeof(current_ch[0]));
            }
        }
        ret = wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                                 ssids, ssid_num,
                                 current_ch, scan_time * req->n_channels,
                                 WF_MLME_FRAMEWORK_NETLINK);
    }
    else
    {
        ret = wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                                 ssids, ssid_num,
                                 current_ch, req->n_channels,
                                 WF_MLME_FRAMEWORK_NETLINK);
    }

exit:
    if (scan_done == wf_true)
    {
        wf_cfg80211_scan_complete(pnic_info);
        wf_cfg80211_scan_done_event_up(pnic_info, wf_true);
    }

    return ret;
}


static int _set_wiphy_params(struct wiphy *wiphy, wf_u32 changed)
{
    CFG80211_DBG();

    return 0;
}

static int cfg80211_set_auth_type (sec_info_st *psec_info,
                                   enum nl80211_auth_type sme_auth_type)
{
    CFG80211_DBG("nl80211 auth type=%d", sme_auth_type);

    switch (sme_auth_type)
    {
        case NL80211_AUTHTYPE_AUTOMATIC:
            psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
            break;

        case NL80211_AUTHTYPE_OPEN_SYSTEM:
            psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
            if (psec_info->ndisauthtype > wf_ndis802_11AuthModeWPA)
                psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
            break;
        case NL80211_AUTHTYPE_SHARED_KEY:
            psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;
            psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;
            break;
        default:
            psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
    }

    return 0;
}

static int cfg80211_set_cipher (sec_info_st *psec_info,
                                wf_u32 nl_cipher, bool ucast)
{
    wf_u32 sec_status, cipher;

    switch(nl_cipher)
    {
        case 0:
        case IW_AUTH_CIPHER_NONE:
            cipher = _NO_PRIVACY_;
            sec_status = wf_ndis802_11EncryptionDisabled;
            break;
        case WLAN_CIPHER_SUITE_WEP40:
            cipher = _WEP40_;
            sec_status = wf_ndis802_11Encryption1Enabled;
            break;
        case WLAN_CIPHER_SUITE_WEP104:
            cipher = _WEP104_;
            sec_status = wf_ndis802_11Encryption1Enabled;
            break;
        case WLAN_CIPHER_SUITE_TKIP:
            cipher = _TKIP_;
            sec_status = wf_ndis802_11Encryption2Enabled;
            break;
        case WLAN_CIPHER_SUITE_CCMP:
            cipher = _AES_;
            sec_status = wf_ndis802_11Encryption3Enabled;
            break;
        default:
            CFG80211_DBG("Unsupported cipher: 0x%x, ucast: %d", nl_cipher, ucast);
            return -ENOTSUPP;
    }

    if (ucast)
    {
        psec_info->dot11PrivacyAlgrthm = cipher;
        psec_info->ndisencryptstatus = sec_status;
    }
    else
    {
        psec_info->dot118021XGrpPrivacy = cipher;
    }

    return 0;
}

static int cfg80211_set_wep_key(nic_info_st *pnic_info, struct cfg80211_connect_params *sme)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u32 wep_key_idx, wep_key_len;
    wf_u32 res = 0;
    wep_key_idx = sme->key_idx;
    wep_key_len = sme->key_len;

    CFG80211_DBG("wep_key_idx = %d, wep_key_len = %d, ", wep_key_idx, wep_key_len);
    CFG80211_ARRAY((wf_u8 *)sme->key, wep_key_len);

    if (sme->key_idx > WEP_KEYS)
    {
        res = -EINVAL;
        goto exit;
    }

    if (wep_key_len > 0)
    {
        wep_key_len = wep_key_len <= 5 ? 5 : 13;

        if (wep_key_len == 13)
        {
            psec_info->dot11PrivacyAlgrthm = _WEP104_;
        }
        else
        {
            psec_info->dot11PrivacyAlgrthm = _WEP40_;
        }
    }
    else
    {
        res = -EINVAL;
        goto exit;
    }

    wf_memcpy(psec_info->dot11DefKey[wep_key_idx].skey, sme->key, wep_key_len);
    psec_info->dot11DefKeylen[wep_key_idx] = wep_key_len;
    psec_info->key_mask |= BIT(wep_key_idx);

exit:
    return res;
}

static int cfg80211_set_wpa_ie (nic_info_st *pnic_info, wf_u8 *pie, size_t ielen)
{
    sec_info_st *sec_info = pnic_info->sec_info;
    wf_u8 *buf = NULL;
    int group_cipher = 0, pairwise_cipher = 0;
    u16 cnt = 0;
    wf_u8 eid, wps_oui[4] = { 0x0, 0x50, 0xf2, 0x04 };
    int res = 0;

    CFG80211_DBG("[WLAN_IW]");

    if (pie == NULL)
    {
        goto exit;
    }

    if (ielen > WF_MAX_WPA_IE_LEN)
    {
        res = -EINVAL;
        goto exit;
    }

    if (ielen)
    {
        buf = wf_kzalloc(ielen);
        if (buf == NULL)
        {
            res = -ENOMEM;
            goto exit;
        }
        memcpy(buf, pie, ielen);
        CFG80211_INFO("wpa_ie(length=%d)=", (wf_u16)ielen);
        CFG80211_ARRAY(buf, ielen);

        if (ielen < RSN_HEADER_LEN)
        {
            CFG80211_WARN("Ie len too short(%d)", (wf_u16)ielen);
            res = -EINVAL;
            goto exit;
        }

        {
            void *pdata;
            wf_u16 data_len;

            if (!wf_80211_mgmt_wpa_survey(buf, ielen, &pdata, &data_len,
                                          &group_cipher, &pairwise_cipher))
            {
                sec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
                sec_info->ndisauthtype = wf_ndis802_11AuthModeWPAPSK;
                sec_info->wpa_enable = wf_true;
                memcpy(sec_info->supplicant_ie, pdata, data_len);
            }
            else if (!wf_80211_mgmt_rsn_survey(buf, ielen, &pdata, &data_len,
                                               &group_cipher, &pairwise_cipher))
            {
                sec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
                sec_info->ndisauthtype = wf_ndis802_11AuthModeWPA2PSK;
                sec_info->rsn_enable = wf_true;
                memcpy(sec_info->supplicant_ie, pdata, data_len);
            }
        }

        switch (group_cipher)
        {
            case CIPHER_SUITE_TKIP:
                sec_info->dot118021XGrpPrivacy = _TKIP_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption2Enabled;
                CFG80211_DBG("dot118021XGrpPrivacy=_TKIP_");
                break;
            case CIPHER_SUITE_CCMP:
                sec_info->dot118021XGrpPrivacy = _AES_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
                CFG80211_DBG("dot118021XGrpPrivacy=_AES_");
                break;
        }

        switch (pairwise_cipher)
        {
            case CIPHER_SUITE_NONE:
                break;
            case CIPHER_SUITE_TKIP:
                sec_info->dot11PrivacyAlgrthm = _TKIP_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption2Enabled;
                CFG80211_DBG("dot11PrivacyAlgrthm=_TKIP_");
                break;
            case CIPHER_SUITE_CCMP:
                sec_info->dot11PrivacyAlgrthm = _AES_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
                CFG80211_DBG("dot11PrivacyAlgrthm=_AES_");
                break;
        }

        while (cnt < ielen)
        {
            eid = buf[cnt];
            if (eid == WF_80211_MGMT_EID_VENDOR_SPECIFIC && !memcmp(&buf[cnt + 2], wps_oui, 4))
            {
                CFG80211_DBG("SET WPS_IE");
                sec_info->wps_ie_len = WF_MIN(buf[cnt + 1] + 2, 512);
                memcpy(sec_info->wps_ie, &buf[cnt], sec_info->wps_ie_len);
                cnt += buf[cnt + 1] + 2;
                break;
            }
            else
            {
                cnt += buf[cnt + 1] + 2;
            }
        }

        wf_mcu_set_on_rcr_am(pnic_info, wf_false);
//        wf_mcu_set_hw_invalid_all(pnic_info);
    }

exit :
    if (buf)
    {
        wf_kfree(buf);
    }
    return res;
}

static int _connect_cb(struct wiphy *wiphy, struct net_device *ndev,
                       struct cfg80211_connect_params *sme)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_u32 wpa_version, key_mgmt;
    int res = 0;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    if (!sme->ssid || sme->ssid_len == 0)
    {
        res = -EINVAL;
        goto exit;
    }

    if (sme->ssid_len > IW_ESSID_MAX_SIZE)
    {
        res = -E2BIG;
        goto exit;
    }

#ifdef CONFIG_LPS
    if (WF_RETURN_FAIL == wf_lps_wakeup(pnic_info, LPS_CTRL_SCAN, 0))
    {
        return -1;
    }
#endif
    CFG80211_INFO("privacy=%d, key=%p, key_len=%d, key_idx=%d, auth_type=%d, wpa:%d",
                  sme->privacy, sme->key, sme->key_len, sme->key_idx, sme->auth_type, sme->crypto.wpa_versions);

    /* connection request no work best on master mode  */
    if (wf_local_cfg_get_work_mode(pnic_info) == WF_MASTER_MODE)
    {
        res = -EPERM;
        goto exit;
    }

    if (wf_p2p_is_valid(pnic_info))
    {
        wf_scan_stop(pnic_info->buddy_nic);
        wf_assoc_stop(pnic_info->buddy_nic);
    }

    /* if buddy interfase is under linking, ignore current request */
    if (pnic_info->buddy_nic)
    {
        mlme_state_e state;
        wf_mlme_get_state((nic_info_st *)(pnic_info->buddy_nic), &state);
        if (state == MLME_STATE_CONN_SCAN ||
            state == MLME_STATE_AUTH ||
            state == MLME_STATE_ASSOC)
        {
            CFG80211_WARN("buddy interface is under linking !");
            res = -EPERM;
            goto exit;
        }
    }

    /* if connection is on building, ignore current request */
    {
        mlme_state_e state;
        wf_mlme_get_state(pnic_info, &state);
        if (state <= MLME_STATE_ASSOC)
        {
            res = -EBUSY;
            goto exit;
        }
    }

    /* checkout if system scanning, abort scan process with timeout */
    if (wf_scan_wait_done(pnic_info, wf_true, 200))
    {
        res = -EBUSY;
        goto exit;
    }

    wf_memset(pcur_network->ssid.data, 0, WF_80211_MAX_SSID_LEN + 1);
    wf_memcpy(pcur_network->ssid.data, sme->ssid, sme->ssid_len);
    pcur_network->ssid.length = sme->ssid_len;
    CFG80211_DBG("ssid = %s, len = %d",
                 pcur_network->ssid.data, pcur_network->ssid.length);

    if (!sme->bssid ||
        is_bcast_addr(sme->bssid) || is_zero_addr(sme->bssid) ||
        is_mcast_addr(sme->bssid))
    {
        CFG80211_DBG("[WLAN_IW] : [sa_data is boradcast or zero ether]");
        res = -EPERM;
        goto exit;
    }
    wf_wlan_set_cur_bssid(pnic_info, (wf_u8 *)sme->bssid);

    /* cleap up sec info */
    wf_memset(pnic_info->sec_info, 0x0, sizeof(sec_info_st));

    psec_info->ndisencryptstatus    = wf_ndis802_11EncryptionDisabled;
    psec_info->dot11PrivacyAlgrthm  = _NO_PRIVACY_;
    psec_info->dot118021XGrpPrivacy = _NO_PRIVACY_;
    psec_info->dot11AuthAlgrthm     = dot11AuthAlgrthm_Open;
    psec_info->ndisauthtype         = wf_ndis802_11AuthModeOpen;
    psec_info->busetkipkey          = wf_false;

    /* parse auth mode */
    wpa_version = sme->crypto.wpa_versions;
    if (!wpa_version)
    {
        psec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;
    }
    else if (wpa_version & (NL80211_WPA_VERSION_1 | NL80211_WPA_VERSION_2))
    {
        psec_info->ndisauthtype = wf_ndis802_11AuthModeWPAPSK;
    }

    cfg80211_set_auth_type(psec_info, sme->auth_type);

    /* parse ie */
    res = cfg80211_set_wpa_ie(pnic_info, (wf_u8 *)sme->ie, sme->ie_len);
    if (res < 0)
    {
        goto exit;
    }

    /* parse crypto for wep */
    if (sme->key_len > 0 && sme->key)
    {
        res = cfg80211_set_wep_key(pnic_info, sme);
        if (res < 0)
        {
            res = -EOPNOTSUPP;
            goto exit;
        }
    }

    /* parse crypto for wpa */
    if (sme->crypto.n_ciphers_pairwise)
    {
        res = cfg80211_set_cipher(psec_info, sme->crypto.ciphers_pairwise[0], wf_true);
        if (res < 0)
        {
            goto exit;
        }
    }

    res = cfg80211_set_cipher(psec_info, sme->crypto.cipher_group, wf_false);
    if (res < 0)
    {
        goto exit;
    }

    if (sme->crypto.n_akm_suites)
    {
        key_mgmt = sme->crypto.akm_suites[0];
        if (key_mgmt == WLAN_AKM_SUITE_8021X || key_mgmt == WLAN_AKM_SUITE_PSK)
        {
            psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
        }
        else
        {
            CFG80211_DBG("Invalid key mgmt: 0x%x", key_mgmt);
        }
    }

    if (wf_p2p_is_valid(pnic_info))
    {
        wf_p2p_connect_entry(pnic_info, (wf_u8 *)sme->ie, sme->ie_len);
    }

    {
        int ret = wf_mlme_conn_start(pnic_info,
                                     wf_wlan_get_cur_bssid(pnic_info),
                                     wf_wlan_get_cur_ssid(pnic_info),
                                     WF_MLME_FRAMEWORK_NETLINK,
                                     wf_true);
        if (ret < 0)
        {
            CFG80211_WARN("connect start fail, fail code: %d", ret);
        }
    }

exit:
    return res;
}


static int _disconnect_cb (struct wiphy *wiphy, struct net_device *ndev,
                           wf_u16 reason_code)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;

    CFG80211_INFO("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    wf_mlme_deauth(pnic_info, wf_false, reason_code);
#ifdef CONFIG_LPS
    if (WF_RETURN_FAIL == wf_lps_wakeup(pnic_info, LPS_CTRL_DISCONNECT, 0))
    {
        return -1;
    }
#endif

    return 0;
}

static int _join_ibss_cb(struct wiphy *wiphy, struct net_device *ndev,
                         struct cfg80211_ibss_params *params)
{
    wf_s32 res = 0;
#ifdef CFG_ENABLE_ADHOC_MODE
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_wlan_mgmt_info_t *wlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &wlan_mgmt_info->cur_network;
    wf_bool bConnected = wf_false;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
    struct cfg80211_chan_def *pch_def;
#endif
    struct ieee80211_channel *pch;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
    pch_def = (struct cfg80211_chan_def *)(&params->chandef);
    pch = (struct ieee80211_channel *)pch_def->chan;
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31))
    pch = (struct ieee80211_channel *)(params->channel);
#endif
    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

#ifdef CONFIG_LPS
    if (WF_RETURN_FAIL == wf_lps_wakeup(pnic_info, LPS_CTRL_SCAN, 0))
    {
        return -1;
    }
#endif

    if (wf_get_adhoc_master(pnic_info)== wf_true)
    {
        res = 0;
        goto exit;
    }

    if (get_sys_work_mode(pnic_info) != WF_ADHOC_MODE)
    {
        res = -EPERM;
        goto exit;
    }

    if (!params->ssid || !params->ssid_len)
    {
        res = -EINVAL;
        goto exit;
    }

    if (params->ssid_len > IW_ESSID_MAX_SIZE)
    {
        res = -E2BIG;
        goto exit;
    }

    pcur_network->bcn_interval = params->beacon_interval;
    pcur_network->cap_info = 0;
    pcur_network->cap_info |= WF_80211_MGMT_CAPAB_IBSS;
    pcur_network->short_slot = NON_SHORT_SLOT_TIME;

    wf_memcpy(pcur_network->mac_addr, nic_to_local_addr(pnic_info), ETH_ALEN);

    psec_info->ndisencryptstatus = wf_ndis802_11EncryptionDisabled;
    psec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
    psec_info->dot118021XGrpPrivacy = _NO_PRIVACY_;
    psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
    psec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;

    res  = cfg80211_set_auth_type(psec_info, NL80211_AUTHTYPE_OPEN_SYSTEM);
    pcur_network->channel = freq_2_ch(pch->center_freq);
    wf_wlan_set_cur_bw(pnic_info, CHANNEL_WIDTH_20);

    wf_mlme_get_connect(pnic_info, &bConnected);
    if (bConnected)
    {
        res = 0;
        goto exit;
    }
    else
    {
        wf_wlan_ssid_t ssid ;
        wf_memset(&ssid, 0, sizeof(wf_wlan_ssid_t));
        ssid.length = params->ssid_len;
        wf_memcpy(ssid.data, (wf_u8 *)params->ssid, ssid.length);
        CFG80211_INFO("start connect to: %s", ssid.data);

        wf_wlan_set_cur_ssid(pnic_info, &ssid);
        wf_scan_wait_done(pnic_info, wf_true, 1000);

        wf_mlme_scan_ibss_start(pnic_info,
                                &pcur_network->ssid,
                                &pcur_network->channel,
                                WF_MLME_FRAMEWORK_NETLINK);
    }

exit:
#endif
    return res;
}


static int _leave_ibss_cb(struct wiphy *wiphy, struct net_device *ndev)
{
#ifdef CFG_ENABLE_ADHOC_MODE
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct wireless_dev *pwdev = pnic_info->pwidev;
    enum nl80211_iftype old_iftype;
    wf_bool bConnected;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));
    old_iftype = pwdev->iftype;
    wf_mlme_get_connect(pnic_info, &bConnected);

    if (bConnected && old_iftype == NL80211_IFTYPE_ADHOC)
    {
        wf_adhoc_leave_ibss_msg_send(pnic_info);
        wf_yield();
        pwdev->iftype = NL80211_IFTYPE_STATION;
        /* free message queue in wdn_info */
        CFG80211_DBG("free resource");
        wf_adhoc_flush_all_resource(pnic_info, WF_INFRA_MODE);

    }
#endif
    return 0;
}

static int _call_set_txpower(struct wiphy *wiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
                             struct wireless_dev *wdev,
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)) || defined(COMPAT_KERNEL_RELEASE)
                             enum nl80211_tx_power_setting type, int mbm)
#else
                             enum tx_power_setting type, int dbm)
#endif
{
    CFG80211_DBG();

    return 0;
}


static int _call_get_txpower(struct wiphy *wiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
                             struct wireless_dev *wdev,
#endif
                             int *dbm)
{
    CFG80211_DBG();

    *dbm = (12);

    return 0;
}


static int _cfg80211_set_power_mgmt(struct wiphy *wiphy,
                                    struct net_device *ndev, bool enabled, int timeout)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct  wf_widev_priv *pwdev_info = pnic_info->widev_priv;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));
    CFG80211_DBG("power management %s", enabled ? "enabled" : "disabled");

    pwdev_info->power_mgmt = enabled;

    return 0;
}


static int _set_pmksa_cb(struct wiphy *wiphy,
                         struct net_device *ndev,
                         struct cfg80211_pmksa *pmksa)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u8 index, blInserted = wf_false;
    wf_u8 strZeroMacAddress[ETH_ALEN] = { 0x00 };
    wf_bool bConnect;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    if (wf_memcmp((wf_u8 *) pmksa->bssid, strZeroMacAddress, ETH_ALEN) == wf_true)
    {
        return -EINVAL;
    }

    wf_mlme_get_connect(pnic_info, &bConnect);

    if (bConnect == wf_false)
    {
        CFG80211_DBG(" not set pmksa cause not in linked state");
        return -EINVAL;
    }


    blInserted = wf_false;

    for (index = 0; index < NUM_PMKID_CACHE; index++)
    {
        if (wf_memcmp(psec_info->PMKIDList[index].Bssid, (wf_u8 *) pmksa->bssid,
                      ETH_ALEN) == wf_true)
        {
            CFG80211_DBG(" BSSID exists in the PMKList.");

            wf_memcpy(psec_info->PMKIDList[index].PMKID,
                      (wf_u8 *) pmksa->pmkid, WLAN_PMKID_LEN);
            psec_info->PMKIDList[index].bUsed = wf_true;
            psec_info->PMKIDIndex = index + 1;
            blInserted = wf_true;
            break;
        }
    }

    if (!blInserted)
    {
        CFG80211_DBG(" Use the new entry index = %d for this PMKID.",
                     psec_info->PMKIDIndex);

        wf_memcpy(psec_info->PMKIDList[psec_info->PMKIDIndex].Bssid,
                  (wf_u8 *) pmksa->bssid, ETH_ALEN);
        wf_memcpy(psec_info->PMKIDList[psec_info->PMKIDIndex].PMKID,
                  (wf_u8 *) pmksa->pmkid, WLAN_PMKID_LEN);

        psec_info->PMKIDList[psec_info->PMKIDIndex].bUsed = wf_true;
        psec_info->PMKIDIndex++;
        if (psec_info->PMKIDIndex == 16)
        {
            psec_info->PMKIDIndex = 0;
        }
    }

    return 0;
}


static int _del_pmksa_cb(struct wiphy *wiphy,
                         struct net_device *ndev,
                         struct cfg80211_pmksa *pmksa)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u8 index, bMatched = wf_false;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    for (index = 0; index < NUM_PMKID_CACHE; index++)
    {
        if (wf_memcmp(psec_info->PMKIDList[index].Bssid, (wf_u8 *) pmksa->bssid,
                      ETH_ALEN) == wf_true)
        {
            wf_memset(psec_info->PMKIDList[index].Bssid, 0x00, ETH_ALEN);
            wf_memset(psec_info->PMKIDList[index].PMKID, 0x00, WLAN_PMKID_LEN);
            psec_info->PMKIDList[index].bUsed = wf_false;
            bMatched = wf_true;
            CFG80211_DBG(" clear id:%hhu", index);
            break;
        }
    }

    if (wf_false == bMatched)
    {
        return -EINVAL;
    }

    return 0;
}


static int _flush_pmksa_cb(struct wiphy *wiphy,
                           struct net_device *ndev)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    wf_memset(&psec_info->PMKIDList[0], 0x00, sizeof(SEC_PMKID_LIST) * NUM_PMKID_CACHE);
    psec_info->PMKIDIndex = 0;

    return 0;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
static int _set_monitor_channel(struct wiphy *wiphy
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
                                , struct cfg80211_chan_def *chandef
#else
                                , struct ieee80211_channel *chan, enum nl80211_channel_type channel_type
#endif
                               )
{

    return 0;
}
#endif


static int _cfg80211_Mgmt_Tx(struct wiphy *wiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
                             struct wireless_dev *wdev,
#else
                             struct net_device *ndev,
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0)) || defined(COMPAT_KERNEL_RELEASE)
                             struct ieee80211_channel *chan,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)) || defined(COMPAT_KERNEL_RELEASE)
                             bool offchan,
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0))
                             enum nl80211_channel_type channel_type,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
                             bool channel_type_valid,
#endif
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)) || defined(COMPAT_KERNEL_RELEASE)
                             unsigned int wait,
#endif
                             const wf_u8 * buf, size_t len,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0))
                             bool no_cck,
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0))
                             bool dont_wait_for_ack,
#endif
#else
                             struct cfg80211_mgmt_tx_params *params,
#endif
                             u64 * cookie)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)) || defined(COMPAT_KERNEL_RELEASE)
    struct ieee80211_channel *chan = params->chan;
    const wf_u8 *buf = params->buf;
    size_t len = params->len;
#endif
    int wait_ack = 0;
    bool ack = wf_true;
    wf_u8 tx_ch;
    wf_u8 frame_styp;
    ndev_priv_st *pndev_priv;
    nic_info_st *pnic_info;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
    if (wdev->netdev)
    {
        pndev_priv = netdev_priv(wdev->netdev);
        pnic_info = pndev_priv->nic;
    }
    else
    {
        return -1;
    }
#else
    struct wireless_dev *wdev;
    if (ndev == NULL)
    {
        return -1;
    }
    pndev_priv = netdev_priv(ndev);
    pnic_info = pndev_priv->nic;
    wdev = ndev_to_wdev(ndev);
#endif

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    if (chan == NULL)
    {
        return -1;
    }
    tx_ch = (wf_u8)ieee80211_frequency_to_channel(chan->center_freq);

    /* cookie generation */
    *cookie = (unsigned long) buf;

    /* indicate ack before issue frame to avoid racing with rsp frame */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
    cfg80211_mgmt_tx_status(ndev, *cookie, buf, len, ack, GFP_KERNEL);
#else
    cfg80211_mgmt_tx_status(wdev, *cookie, buf, len, ack, GFP_KERNEL);
#endif
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34) && LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 36))
    cfg80211_action_tx_status(ndev, *cookie, buf, len, ack, GFP_KERNEL);
#endif



    frame_styp = wf_le16_to_cpu(((struct wl_ieee80211_hdr_3addr *)buf)->frame_ctl) & IEEE80211_FCTL_STYPE;
    if (IEEE80211_STYPE_PROBE_RESP == frame_styp)
    {
        wait_ack = 0;
        CFG80211_INFO("IEEE80211_STYPE_PROBE_RESP");
#ifdef CONFIG_LPS
        if (WF_RETURN_FAIL == wf_lps_wakeup(pnic_info, LPS_CTRL_SCAN, 0))
        {
            return -1;
        }
#endif

    }

    if (wf_p2p_is_valid(pnic_info))
    {
        CFG80211_DBG("ch=%d", tx_ch);
        wf_p2p_proc_tx_action(pnic_info, (wf_u8*)buf, len, tx_ch, wait_ack);

    }
    else
    {
        CFG80211_DBG("need to do for not p2p");
    }
    return 0;
}

static void mgmt_frame_register(struct wiphy *wiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
                                struct wireless_dev *wdev,
#else
                                struct net_device *ndev,
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0))
                                struct mgmt_frame_regs *upd)
#else
                                wf_u16 frame_type, bool reg)
#endif
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
    struct net_device *ndev = wdev->netdev;
#endif
    nic_info_st *pnic_info;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0))
    wf_u16 frame_type = WF_BIT(upd->global_stypes << 4);
    bool reg = false;
#endif

    wf_widev_priv_t *pwdev;
    ndev_priv_st *pndev_priv;

    CFG80211_DBG("frame_type:0x%x, reg:%d", frame_type, reg);
    if (ndev == NULL)
    {
        goto exit;
    }

    return;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0))
    if (frame_type != (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_REQ))
        return;
#else
    pndev_priv = netdev_priv(ndev);
    pnic_info = pndev_priv->nic;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    pwdev = pnic_info->pwidev;

    switch (frame_type)
    {
        case IEEE80211_STYPE_AUTH:
            if (reg > 0)
            {
                SET_CFG80211_REPORT_MGMT(pwdev, IEEE80211_STYPE_AUTH, reg);
            }
            else
            {
                CLR_CFG80211_REPORT_MGMT(pwdev, IEEE80211_STYPE_AUTH, reg);
            }
        case IEEE80211_STYPE_PROBE_REQ:
            if (reg > 0)
            {
                SET_CFG80211_REPORT_MGMT(pwdev, IEEE80211_STYPE_PROBE_REQ, reg);
            }
            else
            {
                CLR_CFG80211_REPORT_MGMT(pwdev, IEEE80211_STYPE_PROBE_REQ, reg);
            }
            break;
        case IEEE80211_STYPE_ACTION:
            if (reg > 0)
            {
                SET_CFG80211_REPORT_MGMT(pwdev, IEEE80211_STYPE_ACTION, reg);
            }
            else
            {
                CLR_CFG80211_REPORT_MGMT(pwdev, IEEE80211_STYPE_ACTION, reg);
            }

            break;
        default:
            break;
    }
#endif
exit:
    return;
}

void wf_cfg80211_ndev_destructor(struct net_device *ndev)
{
    CFG80211_DBG();

#ifdef CONFIG_IOCTL_CFG80211
    if (ndev->ieee80211_ptr)
    {
        wf_kfree(ndev->ieee80211_ptr);
    }
#endif
    free_netdev(ndev);
}

void wf_ap_cfg80211_assoc_event_up(nic_info_st *pnic_info,  wf_u8 *passoc_req, wf_u32 assoc_req_len)
{
    struct net_device *ndev = pnic_info->ndev;
    struct wireless_dev *pwdev = pnic_info->pwidev;
#if defined(WF_USE_CFG80211_STA_EVENT) || defined(COMPAT_KERNEL_RELEASE)
#else
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_s32 freq;
    wf_u32 channel;
#endif
    CFG80211_DBG();

    if (pwdev->wiphy == NULL)
    {
        CFG80211_WARN("wiphy is null!");
        return;
    }

    if (passoc_req && assoc_req_len > 0)
    {
#if defined(WF_USE_CFG80211_STA_EVENT) || defined(COMPAT_KERNEL_RELEASE)
        struct station_info sinfo;
        wf_u8 ie_offset;
        if (GetFrameSubType(passoc_req) == WIFI_ASSOCREQ)
        {
            ie_offset = _ASOCREQ_IE_OFFSET_;
        }
        else
        {
            ie_offset = _REASOCREQ_IE_OFFSET_;
        }
        memset(&sinfo, 0, sizeof(sinfo));
//        sinfo.filled = STATION_INFO_ASSOC_REQ_IES;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 1)
        sinfo.assoc_req_ies = passoc_req + WLAN_HDR_A3_LEN + ie_offset;
        sinfo.assoc_req_ies_len = assoc_req_len - WLAN_HDR_A3_LEN - ie_offset;
#endif
        cfg80211_new_sta(ndev, GetAddr2Ptr(passoc_req), &sinfo, GFP_ATOMIC);
#else
        channel = pcur_network->channel;
        freq = wf_ch_2_freq(channel);

#ifdef COMPAT_KERNEL_RELEASE
        wf_cfg80211_rx_mgmt(pnic_info, freq, 0, passoc_req, assoc_req_len, GFP_ATOMIC);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) && !defined(CONFIG_CFG80211_FORCE_COMPATIBLE_2_6_37_UNDER)
        wf_cfg80211_rx_mgmt(pnic_info, freq, 0, passoc_req, assoc_req_len, GFP_ATOMIC);
#else
        pwdev->iftype = NL80211_IFTYPE_STATION;
        CFG80211_DBG("iftype=%d before call cfg80211_send_rx_assoc()", pwdev->iftype);
        wf_cfg80211_send_rx_assoc(pnic_info, NULL, passoc_req, assoc_req_len);
        CFG80211_DBG("iftype=%d after call cfg80211_send_rx_assoc()", pwdev->iftype);
        pwdev->iftype = NL80211_IFTYPE_AP;
#endif
#endif
    }
}

void wf_ap_cfg80211_disassoc_event_up(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    struct net_device *ndev = pnic_info->ndev;
#if defined(WF_USE_CFG80211_STA_EVENT) || defined(COMPAT_KERNEL_RELEASE)
#else
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    u8 mgmt_buf[128] = { 0 };
    wf_u16 *frame_ctrl;
    struct wl_ieee80211_hdr *pwlanhdr;
    wf_s32 freq;
    wf_u32 channel;
    wf_u8 *pmgmt_frame;
    wf_u16 frame_len;
    wf_u16 reason;
#endif
    CFG80211_DBG();
#ifdef CFG_ENABLE_AP_MODE
    pwdn_info->state = E_WDN_AP_STATE_DASSOC;
#endif
#if defined(WF_USE_CFG80211_STA_EVENT) || defined(COMPAT_KERNEL_RELEASE)
    cfg80211_del_sta(ndev, pwdn_info->mac, GFP_ATOMIC);
#else
    channel = pcur_network->channel;
    freq = wf_ch_2_freq(channel);
    reason = WLAN_REASON_DEAUTH_LEAVING;
    pmgmt_frame = mgmt_buf;
    pwlanhdr = (struct wl_ieee80211_hdr *)pmgmt_frame;

    frame_ctrl = &(pwlanhdr->frame_ctl);
    *(frame_ctrl) = 0;

    wf_memcpy(pwlanhdr->addr1, pwdn_info->mac, MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr2, nic_to_local_addr(pnic_info), MAC_ADDR_LEN);
    wf_memcpy(pwlanhdr->addr3, wf_wlan_get_cur_bssid(pnic_info), MAC_ADDR_LEN);

    SetSeqNum(pwlanhdr, pwdn_info->mgmt_seq);
    pwdn_info->mgmt_seq++;
    SetFrameSubType(pmgmt_frame, WIFI_DEAUTH);

    pmgmt_frame += sizeof(struct wl_ieee80211_hdr_3addr);
    frame_len = sizeof(struct wl_ieee80211_hdr_3addr);

    reason = wf_cpu_to_le16(reason);
    pmgmt_frame = wf_80211_set_fixed_ie(pmgmt_frame, _RSON_CODE_, (wf_u8 *)&reason, &frame_len);

#ifdef COMPAT_KERNEL_RELEASE
    wf_cfg80211_rx_mgmt(pnic_info, freq, 0, mgmt_buf, frame_len, GFP_ATOMIC);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) && !defined(CONFIG_CFG80211_FORCE_COMPATIBLE_2_6_37_UNDER)
    wf_cfg80211_rx_mgmt(pnic_info, freq, 0, mgmt_buf, frame_len, GFP_ATOMIC);
#else
    cfg80211_send_disassoc(ndev, mgmt_buf, frame_len);
#endif
#endif
}

#ifdef CFG_ENABLE_AP_MODE
static int  monitor_open(struct net_device *ndev)
{
    CFG80211_DBG();

    return 0;
}

static int  monitor_close(struct net_device *ndev)
{
    CFG80211_DBG();

    return 0;
}

static int  monitor_xmit_entry(struct sk_buff *skb, struct net_device *ndev)
{
    CFG80211_DBG();

    return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0))
static void  monitor_set_multicast_list(struct net_device *ndev)
{
    CFG80211_DBG();

    return ;
}
#endif

static int  monitor_set_mac_address(struct net_device *ndev, void *addr)
{
    CFG80211_DBG();

    return 0;
}

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 29))
static const struct net_device_ops wf_cfg80211_monitor_if_ops =
{
    .ndo_open = monitor_open,
    .ndo_stop = monitor_close,
    .ndo_start_xmit = monitor_xmit_entry,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0))
    .ndo_set_multicast_list = monitor_set_multicast_list,
#endif
    .ndo_set_mac_address = monitor_set_mac_address,
};
#endif

static int add_monitor(nic_info_st *pnic_info, char *name, struct net_device **ndev)
{
    int ret = 0;
    struct net_device *mon_ndev = NULL;
    struct wireless_dev *mon_wdev = NULL;
    struct wf_netdev_priv *pnpi;
    struct wf_widev_priv *pwdev_priv = pnic_info->widev_priv;

    if (!name)
    {
        CFG80211_WARN(" without specific name");
        ret = -EINVAL;
        goto out;
    }

    if (pwdev_priv->pmon_ndev)
    {
        CFG80211_DBG("monitor interface exist");
        ret = -EBUSY;
        goto out;
    }

    mon_ndev = alloc_etherdev(sizeof(struct wf_netdev_priv));
    if (!mon_ndev)
    {
        CFG80211_WARN(" allocate ndev fail");
        ret = -ENOMEM;
        goto out;
    }

    mon_ndev->type = ARPHRD_IEEE80211_RADIOTAP;
    strncpy(mon_ndev->name, name, IFNAMSIZ);
    mon_ndev->name[IFNAMSIZ - 1] = 0;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 11, 8))
    mon_ndev->priv_destructor = wf_cfg80211_ndev_destructor;
#else
    mon_ndev->destructor = wf_cfg80211_ndev_destructor;
#endif

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 29))
    mon_ndev->netdev_ops = &wf_cfg80211_monitor_if_ops;
#else
    mon_ndev->open = monitor_open;
    mon_ndev->stop = monitor_close;
    mon_ndev->hard_start_xmit = monitor_xmit_entry;
    mon_ndev->set_mac_address = monitor_set_mac_address;
#endif

    pnpi = netdev_priv(mon_ndev);
    pnpi->priv = pnic_info;
    pnpi->priv_size = sizeof(pnic_info);

    mon_wdev = (struct wireless_dev *)wf_kzalloc(sizeof(struct wireless_dev));
    if (!mon_wdev)
    {
        CFG80211_WARN(" allocate mon_wdev fail");
        ret = -ENOMEM;
        goto out;
    }

    mon_wdev->wiphy = ((struct wireless_dev *)pnic_info->pwidev)->wiphy;
    mon_wdev->netdev = mon_ndev;
    mon_wdev->iftype = NL80211_IFTYPE_MONITOR;
    mon_ndev->ieee80211_ptr = mon_wdev;

    ret = register_netdevice(mon_ndev);
    if (ret)
    {
        goto out;
    }

    *ndev = pwdev_priv->pmon_ndev = mon_ndev;
    wf_memcpy(pwdev_priv->ifname_mon, name, IFNAMSIZ + 1);

out:
    if (ret && mon_wdev)
    {
        wf_kfree(mon_wdev);
        mon_wdev = NULL;
    }

    if (ret && mon_ndev)
    {
        free_netdev(mon_ndev);
        *ndev = mon_ndev = NULL;
    }

    return ret;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
static struct wireless_dev *
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)) || defined(COMPAT_KERNEL_RELEASE)
static struct net_device *
#else
static int
#endif
add_virtual_intf(struct wiphy *wiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 0))
                 const char *name,
#else
                 char *name,
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
                 unsigned char name_assign_type,
#endif
                 enum nl80211_iftype type,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0))
                 wf_u32 *flags,
#endif
                 struct vif_params *params)
{
    int ret = 0;
    struct net_device *ndev = NULL;
    nic_info_st *pnic_info = *((nic_info_st **)wiphy_priv(wiphy));

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));
    CFG80211_DBG("wiphy:%s, name:%s, type:%d\n", wiphy_name(wiphy), name, type);

    switch (type)
    {
        case NL80211_IFTYPE_ADHOC:
        case NL80211_IFTYPE_AP_VLAN:
        case NL80211_IFTYPE_WDS:
        case NL80211_IFTYPE_MESH_POINT:
            ret = -ENODEV;
            break;
        case NL80211_IFTYPE_MONITOR:
            ret = add_monitor(pnic_info, (char *)name, &ndev);
            break;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
        case NL80211_IFTYPE_P2P_CLIENT:
#endif
        case NL80211_IFTYPE_STATION:
            ret = -ENODEV;
            break;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
        case NL80211_IFTYPE_P2P_GO:
#endif
        case NL80211_IFTYPE_AP:
            ret = -ENODEV;
            break;
        default:
            ret = -ENODEV;
            CFG80211_WARN("Unsupported interface type\n");
            break;
    }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
    return ndev ? ndev->ieee80211_ptr : ERR_PTR(ret);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38)) || defined(COMPAT_KERNEL_RELEASE)
    return ndev ? ndev : ERR_PTR(ret);
#else
    return ret;
#endif
}

static int del_virtual_intf(struct wiphy *wiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
                            struct wireless_dev *wdev
#else
                            struct net_device *ndev
#endif
                           )
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
    struct net_device *ndev = wdev->netdev;
#endif
    int ret = 0;
    nic_info_st *pnic_info;
    struct wf_widev_priv *pwdev;
    ndev_priv_st *pndev_priv;

    CFG80211_DBG();

    if (!ndev)
    {
        ret = -EINVAL;
        goto exit;
    }

    pndev_priv = netdev_priv(ndev);
    pnic_info = pndev_priv->nic;
    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));
    pwdev = pnic_info->widev_priv;



    unregister_netdevice(ndev);

    if (ndev == pwdev->pmon_ndev)
    {
        pwdev->pmon_ndev = NULL;
        pwdev->ifname_mon[0] = '\0';
        CFG80211_DBG(" remove monitor interface");
    }

exit:
    return ret;
}

static int add_beacon(nic_info_st *pnic_info, const wf_u8 * head, size_t head_len,
                      const wf_u8 * tail, size_t tail_len)
{

    int ret = 0;
    u8 *pbuf = NULL;
    uint len = 0;
    uint wps_ielen = 0;
    //wf_u8 *p2p_ie;
    wf_u32 p2p_ielen = 0;
    wf_u8 got_p2p_ie = wf_false;
    p2p_info_st *p2p_info = pnic_info->p2p;


    CFG80211_DBG("beacon_head_len=%zu, beacon_tail_len=%zu", head_len, tail_len);

    if (wf_mlme_check_mode(pnic_info, WF_MASTER_MODE) != wf_true)
    {
        return -EINVAL;
    }
    if (head_len < 24)
    {
        return -EINVAL;
    }
    pbuf = wf_kzalloc(head_len + tail_len);
    if (!pbuf)
    {
        return -ENOMEM;
    }
    wf_memcpy(pbuf, (void *)head + 24, head_len - 24);
    wf_memcpy(pbuf + head_len - 24, (void *)tail, tail_len);

    len = head_len + tail_len - 24;

    if (wf_wlan_get_wps_ie(pbuf + 12, len - 12, NULL, &wps_ielen))
        CFG80211_INFO("add bcn, wps_ielen=%d\n", wps_ielen);

    if (wf_p2p_is_valid(pnic_info))
    {
        if (wf_p2p_get_ie(pbuf + _FIXED_IE_LENGTH_, len - _FIXED_IE_LENGTH_, NULL, &p2p_ielen))
        {
            got_p2p_ie = wf_true;
            CFG80211_INFO("got p2p_ie, len = %d\n", p2p_ielen);

            if (p2p_info->p2p_state == P2P_STATE_NONE)
            {
                CFG80211_INFO("Enable P2P for the first time\n");
                wf_p2p_enable(pnic_info, P2P_ROLE_GO);
            }
            else
            {
                CFG80211_INFO("enter GO mode , p2p_ielen=%d\n", p2p_ielen);
                wf_p2p_set_role(p2p_info, P2P_ROLE_GO);
                wf_p2p_set_state(p2p_info, P2P_STATE_GONEGO_OK);
                p2p_info->intent = 15;
            }

        }

    }

    if (wf_ap_set_beacon(pnic_info, pbuf, len, WF_MLME_FRAMEWORK_NETLINK) == 0)
    {
        ret = 0;
    }
    else
    {
        ret = -EINVAL;
    }

    wf_kfree(pbuf);

    return ret;
}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0)) && !defined(COMPAT_KERNEL_RELEASE)
static int add_beacon_cb(struct wiphy *wiphy, struct net_device *ndev,
                         struct beacon_parameters *info)
{
    int ret = 0;
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    ret =
        add_beacon(pnic_info, info->head, info->head_len, info->tail,
                   info->tail_len);

    return ret;
}

static int set_beacon_cb(struct wiphy *wiphy, struct net_device *ndev,
                         struct beacon_parameters *info)
{
    CFG80211_DBG();

    add_beacon_cb(wiphy, ndev, info);

    return 0;
}

static int del_beacon_cb(struct wiphy *wiphy, struct net_device *ndev)
{
    CFG80211_DBG();

    return 0;
}
#else
static int cfg80211_start_ap(struct wiphy *wiphy, struct net_device *ndev,
                             struct cfg80211_ap_settings *settings)
{
    int ret = 0;
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    CFG80211_DBG(" hidden_ssid:%d, auth_type:%d\n", settings->hidden_ssid, settings->auth_type);

    ret =
        add_beacon(pnic_info, settings->beacon.head,
                   settings->beacon.head_len, settings->beacon.tail,
                   settings->beacon.tail_len);

    pcur_network->hidden_ssid_mode =(wf_80211_hidden_ssid_e )settings->hidden_ssid;
    if (settings->ssid && settings->ssid_len)
    {
        pcur_network->hidden_ssid.length = settings->ssid_len;
        wf_memcpy(pcur_network->hidden_ssid.data, settings->ssid, settings->ssid_len);
    }
    wf_ap_work_start(pnic_info);
    return ret;
}

static int cfg80211_change_beacon(struct wiphy *wiphy,
                                  struct net_device *ndev,
                                  struct cfg80211_beacon_data *info)
{
    int ret = 0;
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    ret =
        add_beacon(pnic_info, info->head, info->head_len, info->tail,
                   info->tail_len);
    return ret;
}

static int cfg80211_stop_ap(struct wiphy *wiphy, struct net_device *ndev)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    wf_ap_work_stop(pnic_info);
    return 0;
}
#endif
static int add_station(struct wiphy *wiphy,
                       struct net_device *ndev,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 16, 0))
                       u8 * mac,
#else
                       const u8 * mac,
#endif
                       struct station_parameters *params)
{
    int ret = 0;
    CFG80211_DBG();

    return ret;
}

static int del_station(struct wiphy *wiphy,
                       struct net_device *ndev,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 16, 0))
                       u8 * mac
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0))
                       const u8 * mac
#else
                       struct station_del_parameters *params
#endif
                      )
{
    const wf_u8 *target_mac;
    wdn_net_info_st *pwdn_info = NULL;
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;

    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0))
    target_mac = mac;
#else
    target_mac = params->mac;
#endif
    if (wf_mlme_check_mode(pnic_info, WF_MASTER_MODE) != wf_true)
    {
        CFG80211_WARN("sys mode is not WIFI_AP_STATE");
        return -EINVAL;
    }

    if (!target_mac)
    {
        CFG80211_DBG("flush all sta, and cam_entry");

        wf_ap_deauth_all_sta(pnic_info, WF_80211_REASON_DEAUTH_LEAVING);

        return 0;
    }

    CFG80211_DBG("free sta macaddr =" WF_MAC_FMT, WF_MAC_ARG(target_mac));

    if (is_bcast_addr(target_mac))
    {
        return -EINVAL;
    }

    pwdn_info = wf_wdn_find_info(pnic_info, (wf_u8 *)target_mac);
    if (pwdn_info != NULL)
    {
        CFG80211_DBG("wdn state:%d", pwdn_info->state);
        if (E_WDN_AP_STATE_DAUTH != pwdn_info->state && E_WDN_AP_STATE_DASSOC != pwdn_info->state)
        {
            CFG80211_DBG("do not del station "WF_MAC_FMT, WF_MAC_ARG(target_mac));
        }
        else
        {
            CFG80211_DBG("free psta, aid=%d\n", pwdn_info->aid);
            if (wf_mlme_check_mode(pnic_info, WF_MASTER_MODE) == wf_true)
            {
                wf_ap_msg_load(pnic_info, &pwdn_info->ap_msg,
                               WF_AP_MSG_TAG_DEAUTH_FRAME, NULL, 0);
            }
            CFG80211_INFO("wdn_remove :"WF_MAC_FMT, WF_MAC_ARG(target_mac));
            wf_wdn_remove(pnic_info, (wf_u8 *)target_mac);
            if (wf_p2p_is_valid(pnic_info))
            {
                if (pwdn_info->is_p2p_device && 1>=wf_wdn_get_cnt(pnic_info))
                {
                    CFG80211_INFO("p2p restart");
                    wf_p2p_cannel_remain_on_channel(pnic_info);
                    wf_p2p_enable(pnic_info, P2P_ROLE_DEVICE);
                }
            }

        }
    }
    else
    {
        CFG80211_DBG("the wdn has never been added");
    }

    return 0;
}

static int change_station(struct wiphy *wiphy,
                          struct net_device *ndev,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 16, 0))
                          u8 * mac,
#else
                          const u8 * mac,
#endif
                          struct station_parameters *params)
{
    CFG80211_DBG("mac addr:"WF_MAC_FMT, WF_MAC_ARG(mac));
    CFG80211_DBG("aid:%d", params->aid);

    return 0;
}


static int dump_station(struct wiphy *wiphy,
                        struct net_device *ndev, int idx, u8 * mac,
                        struct station_info *sinfo)
{

    int ret = 0;
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;

    wdn_net_info_st *pwdn_info = NULL;
    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    pwdn_info = wf_wdn_find_info_by_id(pnic_info, (wf_u8)idx);

    if (pwdn_info == NULL)
    {
        CFG80211_DBG("Station is not found\n");
        ret = -ENOENT;
        goto exit;
    }
    wf_memcpy(mac, pwdn_info->mac, ETH_ALEN);
    sinfo->filled = 0;
    sinfo->filled |= BIT(NL80211_STA_INFO_SIGNAL);

exit:
    return ret;
}

static int change_bss(struct wiphy *wiphy, struct net_device *ndev,
                      struct bss_parameters *params)
{
    CFG80211_DBG();

    return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
static int set_channel(struct wiphy *wiphy
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
                       , struct net_device *ndev
#endif
                       , struct ieee80211_channel *chan
                       , enum nl80211_channel_type channel_type)
{
    int chan_target = (u8) ieee80211_frequency_to_channel(chan->center_freq);
    int chan_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
    int chan_width = CHANNEL_WIDTH_20;
    nic_info_st *pnic_info = *((nic_info_st **)wiphy_priv(wiphy));

    CFG80211_DBG();

    switch (channel_type)
    {
        case NL80211_CHAN_NO_HT:
        case NL80211_CHAN_HT20:
            chan_width = CHANNEL_WIDTH_20;
            chan_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
            break;
        case NL80211_CHAN_HT40MINUS:
            chan_width = CHANNEL_WIDTH_40;
            chan_offset = HAL_PRIME_CHNL_OFFSET_UPPER;
            break;
        case NL80211_CHAN_HT40PLUS:
            chan_width = CHANNEL_WIDTH_40;
            chan_offset = HAL_PRIME_CHNL_OFFSET_LOWER;
            break;
        default:
            chan_width = CHANNEL_WIDTH_20;
            chan_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
            break;
    }

    wf_hw_info_set_channnel_bw(pnic_info, chan_target, chan_width, chan_offset);

    return 0;
}

#endif
#endif



#if defined(CONFIG_PNO_SUPPORT) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0))
static int _sched_scan_start(struct wiphy *wiphy,
                             struct net_device *dev,
                             struct cfg80211_sched_scan_request
                             *request)
{
    CFG80211_DBG();

    return 0;
}


static int _sched_scan_stop(struct wiphy *wiphy,
                            struct net_device *dev)
{
    CFG80211_DBG();

    return 0;
}
#endif

#ifdef WF_CONFIG_P2P
static wf_s32 cfg80211_remain_on_channel_cb(struct wiphy *wiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
        struct wireless_dev *wdev,
#else
        struct net_device *ndev,
#endif
        struct ieee80211_channel *channel,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0))
        enum nl80211_channel_type
        channel_type,
#endif
        unsigned int duration, u64 * cookie)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
    struct net_device *ndev = wdev->netdev;
#endif

    wf_u8 remain_ch = (u8) ieee80211_frequency_to_channel(channel->center_freq);

    struct cfg80211_wifidirect_info *pcfg80211_wdinfo;
    ndev_priv_st *pndev_priv;
    p2p_info_st *p2p_info;
    nic_info_st *pnic_info;
    wf_u8 is_p2p_find = wf_false;

    CFG80211_DBG("start");
#ifndef CONFIG_RADIO_WORK
#define WL_ROCH_DURATION_ENLARGE
#define WL_ROCH_BACK_OP
#endif

    if (ndev == NULL)
    {
        return -EINVAL;
    }

    pndev_priv = netdev_priv(ndev);
    pnic_info = pndev_priv->nic;
    p2p_info = pnic_info->p2p;
    pcfg80211_wdinfo = &pndev_priv->cfg80211_wifidirect;
    if (p2p_info && p2p_info->is_ro_ch)
    {
        CFG80211_INFO("it is already remain on channel");
        //p2p_info->ro_ch_again = 1;
        //wf_p2p_cannel_remain_on_channel(pnic_info);
    }
    is_p2p_find = (duration < (p2p_info->ext_listen_interval)) ? wf_true : wf_false;
    *cookie = atomic_inc_return(&pcfg80211_wdinfo->ro_ch_cookie_gen);
    CFG80211_INFO("[%d] mac addr: "WF_MAC_FMT", cookie:%llx, remain_ch:%d, duration=%d, nego:0x%x",
                  pnic_info->ndev_id, WF_MAC_ARG(nic_to_local_addr(pnic_info)), *cookie, remain_ch, duration, p2p_info->go_negoing);
    wf_memcpy(&pcfg80211_wdinfo->remain_on_ch_channel, channel, sizeof(struct ieee80211_channel));
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0))
    pcfg80211_wdinfo->remain_on_ch_type = channel_type;
#endif
    pcfg80211_wdinfo->remain_on_ch_cookie = *cookie;

#if 0
    while (0 != duration && duration < 400)
    {
        duration = duration * 3;
    }
#endif
    pcfg80211_wdinfo->duration = duration;
    if (wf_false == wf_p2p_is_valid(pnic_info))
    {
        CFG80211_DBG("[%d] mac addr: "WF_MAC_FMT ", not support p2p", pnic_info->ndev_id, WF_MAC_ARG(nic_to_local_addr(pnic_info)));
        return 0;
    }

    p2p_info->ro_ch_duration = duration;
    p2p_info->remain_ch = remain_ch;

    wf_scan_wait_done(pnic_info, wf_true, 200);
    wf_scan_wait_done(pnic_info->buddy_nic, wf_true, 200);
    wf_mlme_scan_abort( pnic_info);
    wf_mlme_scan_abort(pnic_info->buddy_nic);

    if (p2p_info->scb.ready_on_channel)
    {
        p2p_info->scb.ready_on_channel(pnic_info, NULL, 0);
    }

#if 0
    if (WF_RETURN_OK == wf_mlme_p2p_msg(pnic_info, WF_P2P_MSG_TAG_RO_CH, NULL, 0))
    {
        //wf_p2p_proto_thrd_wait(pnic_info);
        wf_msleep(100);
    }
    else
    {
        CFG80211_WARN("wf_mlme_p2p_msg failed, %d", ret);
    }
#else
    wf_p2p_remain_on_channel(pnic_info);
#endif
    CFG80211_INFO("end");
    return 0;

}

static wf_s32 cfg80211_cancel_remain_on_channel_cb(struct wiphy *wiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
        struct wireless_dev *wdev,
#else
        struct net_device *ndev,
#endif
        u64 cookie)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
    struct net_device *ndev = wdev->netdev;
#endif

    p2p_info_st *p2p_info   = NULL;
    ndev_priv_st *pndev_priv = NULL;
    nic_info_st *pnic_info   = NULL;
    pndev_priv = netdev_priv(ndev);

    pnic_info = pndev_priv->nic;
    p2p_info = pnic_info->p2p;
    CFG80211_DBG("mac addr: "WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(pnic_info)));
    if (ndev == NULL)
    {
        return -EINVAL;
    }

    if (wf_false == wf_p2p_is_valid(pnic_info))
    {
        CFG80211_DBG("[%d] mac addr: "WF_MAC_FMT ", not support p2p", pnic_info->ndev_id, WF_MAC_ARG(nic_to_local_addr(pnic_info)));
        return 0;
    }

    if (wf_false == p2p_info->is_ro_ch)
    {
        return 0;
    }

    //wf_os_api_timer_unreg(&p2p_info->remain_ch_timer);

    CFG80211_INFO("cookie:0x%llx~ 0x%llx, nego:0x%x\n",
                  cookie, pndev_priv->cfg80211_wifidirect.remain_on_ch_cookie, p2p_info->go_negoing);
#if 0
    if (WF_RETURN_OK == wf_mlme_p2p_msg(pnic_info, WF_P2P_MSG_TAG_RO_CH_CANNEL, NULL, 0))
    {
        //wf_p2p_proto_thrd_wait(pnic_info);
    }
    else
    {
        CFG80211_WARN("wf_mlme_p2p_msg failed:%d", ret);
    }
#else
    wf_p2p_cannel_remain_on_channel(pnic_info);
#endif
    CFG80211_DBG("end");
    return 0;

}
#endif

static struct cfg80211_ops wf_cfg80211_ops =
{
    .change_virtual_intf = _cfg80211_change_iface,

    .add_key = _add_key_cb,
    .get_key = _get_key_cb, //
    .del_key = _del_key_cb,
    .set_default_key = _set_default_key_cb,

    .get_station = _cfg80211_get_station,
    .scan = _call_scan_cb,
    .set_wiphy_params = _set_wiphy_params,
    .connect = _connect_cb,
    .disconnect = _disconnect_cb,

    .join_ibss = _join_ibss_cb, //
    .leave_ibss = _leave_ibss_cb, //

    .set_tx_power = _call_set_txpower,
    .get_tx_power = _call_get_txpower,
    .set_power_mgmt = _cfg80211_set_power_mgmt, //

    .set_pmksa = _set_pmksa_cb, //
    .del_pmksa = _del_pmksa_cb, //
    .flush_pmksa = _flush_pmksa_cb, //

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
    .set_monitor_channel = _set_monitor_channel, //
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
    .mgmt_tx = _cfg80211_Mgmt_Tx, //
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
    .update_mgmt_frame_registrations = mgmt_frame_register,
#else
    .mgmt_frame_register = mgmt_frame_register, //
#endif
#elif  (LINUX_VERSION_CODE>=KERNEL_VERSION(2, 6, 34) && LINUX_VERSION_CODE<=KERNEL_VERSION(2, 6, 35))
    .action = _cfg80211_Mgmt_Tx,
#endif

#if defined(CONFIG_PNO_SUPPORT) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0))
    .sched_scan_start = _sched_scan_start, //
    .sched_scan_stop = _sched_scan_stop, //
#endif

#ifdef CFG_ENABLE_AP_MODE
    .add_virtual_intf = add_virtual_intf,
    .del_virtual_intf = del_virtual_intf,

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0)) && !defined(COMPAT_KERNEL_RELEASE)
    .add_beacon = add_beacon_cb,
    .set_beacon = set_beacon_cb,
    .del_beacon = del_beacon_cb,
#else
    .start_ap = cfg80211_start_ap,
    .change_beacon = cfg80211_change_beacon,
    .stop_ap = cfg80211_stop_ap,
#endif

    .add_station = add_station,
    .del_station = del_station,
    .change_station = change_station,
    .dump_station = dump_station,
    .change_bss = change_bss,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
    .set_channel = set_channel,
#endif
#endif

#ifdef WF_CONFIG_P2P
    .remain_on_channel = cfg80211_remain_on_channel_cb,
    .cancel_remain_on_channel = cfg80211_cancel_remain_on_channel_cb,
#endif

};

int wf_cfg80211_alloc (nic_info_st *pnic_info)
{
    struct net_device *pndev = pnic_info->ndev;
    struct wiphy *pwiphy;
    struct wireless_dev *pwidev;
    wf_widev_priv_t *pwidev_priv;

    CFG80211_DBG();

    pnic_info->pwiphy = NULL;
    pnic_info->pwidev = NULL;

    /* alloc wireless phy object */
    pwiphy = wiphy_new(&wf_cfg80211_ops, sizeof(hif_node_st *));
    if (NULL == pwiphy)
    {
        CFG80211_WARN("allocate wiphy device fail !!");
        return -1;
    }
    if (wiphy_cfg(pwiphy))
    {
        CFG80211_WARN("wiphy config fail !!");
        return -2;
    }
#ifdef CONFIG_OS_ANDROID
    pnic_info->setband = WIFI_FREQUENCY_BAND_AUTO;
#endif
    set_wiphy_dev(pwiphy, pnic_info->dev);
    set_wiphy_pirv(pwiphy, pnic_info);
    pnic_info->pwiphy = pwiphy;

    /* alloc wireless device */
    pwidev = (void *)wf_kzalloc(sizeof(struct wireless_dev));
    if (NULL == pwidev)
    {
        CFG80211_INFO("allocate wireless device fail !!");
        return -3;
    }
    pwidev->wiphy           = pwiphy;
    pwidev->iftype          = NL80211_IFTYPE_STATION;
    pndev->ieee80211_ptr    = pwidev;
    pwidev->netdev          = pndev;
    pnic_info->pwidev       = pwidev;

    /* initialize wireless private data */
    pwidev_priv                 = pnic_info->widev_priv;
    pwidev_priv->pwidev         = pwidev;
    pwidev_priv->pmon_ndev      = NULL;
    pwidev_priv->ifname_mon[0]  = '\0';
    pwidev_priv->pnic_info      = pnic_info;
    pwidev_priv->pscan_request  = NULL;
    wf_lock_init(&pwidev_priv->scan_req_lock, WF_LOCK_TYPE_BH);
    pwidev_priv->power_mgmt     = wf_false;

    pwidev_priv->bandroid_scan = wf_true;

    atomic_set(&pwidev_priv->ro_ch_to, 1);
    atomic_set(&pwidev_priv->switch_ch_to, 1);

    return 0;
}

int wf_cfg80211_reg (struct wiphy *pwiphy)
{
    CFG80211_DBG();

    return wiphy_register(pwiphy);
}

void wf_cfg80211_widev_unreg (nic_info_st *pnic_info)
{
    struct wireless_dev *pwidev;
    struct net_device *pndev;

    if (NULL == pnic_info)
    {
        CFG80211_WARN("pnic_info null");
        return;
    }
    CFG80211_DBG("ndev_id:%d", pnic_info->ndev_id);

    wf_scan_wait_done(pnic_info, wf_true, 400);

    pwidev = pnic_info->pwidev;
    pndev = pnic_info->ndev;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 2, 0))
    if (pwidev->current_bss)
    {
        wf_u8 is_local_disc = 1;
        CFG80211_INFO("clear current_bss by cfg80211_disconnected");
        cfg80211_disconnected(pndev, 0, NULL, 0, is_local_disc, GFP_ATOMIC);
    }
#elif ((LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)) && \
       (LINUX_VERSION_CODE < KERNEL_VERSION(4, 2, 0))) || \
      defined(COMPAT_KERNEL_RELEASE)
    if (pwidev->current_bss)
    {
        CFG80211_INFO("clear current_bss by cfg80211_disconnected");
        cfg80211_disconnected(pndev, 0, NULL, 0, GFP_ATOMIC);
    }
#endif
}

wf_s32 wf_cfg80211_remain_on_channel_expired (void *nic, void *param, wf_u32 param_len)
{

    nic_info_st *pnic_info  = NULL;
    ndev_priv_st *ndev_priv = NULL;
    cfg80211_wifidirect_info_st *cfg_wfdirect_info = NULL;
    wf_widev_priv_t *pwidev = NULL;
    CFG80211_DBG("start");

    if (NULL == nic)
    {
        LOG_E("[%s, %d] input param is null", __func__, __LINE__);
        return WF_RETURN_FAIL;
    }

    pnic_info = nic;
    ndev_priv = netdev_priv(pnic_info->ndev);
    if (NULL == ndev_priv)
    {
        LOG_E("[%s, %d] input param is null", __func__, __LINE__);
        return WF_RETURN_FAIL;
    }

    cfg_wfdirect_info = &ndev_priv->cfg80211_wifidirect;
    pwidev = pnic_info->widev_priv;
    CFG80211_INFO("cookie:0x%llx", cfg_wfdirect_info->remain_on_ch_cookie);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
    cfg80211_remain_on_channel_expired(pnic_info->ndev, cfg_wfdirect_info->remain_on_ch_cookie, &cfg_wfdirect_info->remain_on_ch_channel, cfg_wfdirect_info->remain_on_ch_type, GFP_KERNEL);
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0))
    cfg80211_remain_on_channel_expired(pwidev->pwidev, cfg_wfdirect_info->remain_on_ch_cookie, &cfg_wfdirect_info->remain_on_ch_channel, cfg_wfdirect_info->remain_on_ch_type, GFP_KERNEL);
#else
    cfg80211_remain_on_channel_expired(pwidev->pwidev, cfg_wfdirect_info->remain_on_ch_cookie, &cfg_wfdirect_info->remain_on_ch_channel, GFP_ATOMIC);
#endif

    return WF_RETURN_OK;

}

wf_s32 wf_cfg80211_p2p_rx_mgmt(void * nic_info, void* param, wf_u32 param_len)
{
    wf_s32 freq = 0;
    nic_info_st *pnic_info = NULL;
    wf_u8 * pmgmt_frame = param;
    wf_u32 frame_len = param_len;
    p2p_info_st *p2p_info = NULL;

    CFG80211_DBG("start");

    if (NULL == nic_info)
    {
        LOG_E("[%s, %d] input param is null", __func__, __LINE__);
        return WF_RETURN_FAIL;
    }
    pnic_info   = nic_info;
    p2p_info    = pnic_info->p2p;
    freq = wf_ch_2_freq(p2p_info->report_ch);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) || defined(COMPAT_KERNEL_RELEASE)
    wf_cfg80211_rx_mgmt(pnic_info, freq, 0, pmgmt_frame, frame_len, GFP_ATOMIC);
#else
    cfg80211_rx_action(pnic_info->ndev, freq, pmgmt_frame, frame_len, GFP_ATOMIC);
#endif
    CFG80211_INFO("report_ch:%d, freq:%d", p2p_info->report_ch, freq);
    return 0;
}

wf_s32 wf_cfg80211_p2p_ready_on_channel(void * nic_info, void* param, wf_u32 param_len)
{
    nic_info_st *pnic_info  = NULL;
    ndev_priv_st *ndev_priv = NULL;
    wf_widev_priv_t *pwidev = NULL;
    cfg80211_wifidirect_info_st *cfg_wifi_info = NULL;

    CFG80211_DBG("start");
    if (NULL == nic_info)
    {
        LOG_E("[%s, %d] input param is null", __func__, __LINE__);
        return WF_RETURN_FAIL;
    }

    pnic_info = nic_info;
    ndev_priv = netdev_priv(pnic_info->ndev);
    if (NULL == ndev_priv)
    {
        LOG_E("[%s, %d] input param is null", __func__, __LINE__);
        return WF_RETURN_FAIL;
    }
    pwidev = &ndev_priv->widev_priv;
    cfg_wifi_info = &ndev_priv->cfg80211_wifidirect;
    CFG80211_DBG("[%d] remain_on_ch_cookie:%lld", pnic_info->ndev_id, cfg_wifi_info->remain_on_ch_cookie);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
    wf_cfg80211_ready_on_channel(pnic_info->ndev, cfg_wifi_info->remain_on_ch_cookie, &cfg_wifi_info->remain_on_ch_channel, cfg_wifi_info->remain_on_ch_type, cfg_wifi_info->duration, GFP_KERNEL);
#else
    wf_cfg80211_ready_on_channel(pwidev->pwidev, cfg_wifi_info->remain_on_ch_cookie, &cfg_wifi_info->remain_on_ch_channel, cfg_wifi_info->remain_on_ch_type, cfg_wifi_info->duration, GFP_KERNEL);
#endif
    return 0;
}

#endif

