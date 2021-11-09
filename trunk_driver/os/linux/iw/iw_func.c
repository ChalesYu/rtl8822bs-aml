/*
 * iw_func.c
 *
 * used for wext framework interface
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
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "wf_os_api.h"
#include "iw_func.h"
#include <linux/decompress/mm.h>
#include "mp.h"

#define IW_FUNC_DBG(fmt, ...)       LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define IW_FUNC_ARRAY(data, len)    log_array(data, len)
#define IW_FUNC_INFO(fmt, ...)      LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define IW_FUNC_WARN(fmt, ...)      LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define IW_FUNC_ERROR(fmt, ...)     LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)

#define MIN_FRAG_THRESHOLD     256U
#define MAX_FRAG_THRESHOLD     2346U

#define MAX_WPA_IE_LEN          (WF_FIELD_SIZEOF(wf_wlan_mgmt_scan_que_node_t, wpa_ie))
#define MAX_RSN_IE_LEN          (WF_FIELD_SIZEOF(wf_wlan_mgmt_scan_que_node_t, rsn_ie))

#define AUTH_ALG_OPEN_SYSTEM        0x1
#define AUTH_ALG_SHARED_KEY         0x2
#define AUTH_ALG_LEAP               0x4


const channel_info_st chan_info_tab[14] =
{
    { 1,  2412 },
    { 2,  2417 },
    { 3,  2422 },
    { 4,  2427 },
    { 5,  2432 },
    { 6,  2437 },
    { 7,  2442 },
    { 8,  2447 },
    { 9,  2452 },
    { 10, 2457 },
    { 11, 2462 },
    { 12, 2467 },
    { 13, 2472 },
    { 14, 2484 },
};
wireless_info_st def_wireless_info =
{
#define RATE_COUNT              4
#define MIN_FRAG_THRESHOLD      256U
#define MAX_FRAG_THRESHOLD      2346U
    .throughput = 5 * 1000 * 1000, /* ~5 Mb/s real (802.11b) */

    /* percent values between 0 and 100. */
    .max_qual.qual = 100,
    .max_qual.level = 100,
    .max_qual.noise = 100,
    .max_qual.updated = IW_QUAL_ALL_UPDATED, /* Updated all three */

    .avg_qual.qual = 92, /* > 8% missed beacons is 'bad' */
    /* TODO: Find real 'good' to 'bad' threshol value for RSSI */
    .avg_qual.level = (wf_u8) - 70, /* -70dbm */
    .avg_qual.noise = (wf_u8) - 256,
    .avg_qual.updated = IW_QUAL_ALL_UPDATED, /* Updated all three */

    .num_bitrates = RATE_COUNT,
    .bitrate = { 1000000, 2000000, 5500000, 11000000, },

    .min_frag = MIN_FRAG_THRESHOLD,
    .max_frag = MAX_FRAG_THRESHOLD,

    .num_channels = ARRAY_SIZE(chan_info_tab),
    .pchannel_tab = (channel_info_st *)chan_info_tab,

    /*  The following code will proivde the security capability to network manager. */
    /*  If the driver doesn't provide this capability to network manager, */
    /*  the WPA/WPA2 routers can't be chosen in the network manager. */
#if WIRELESS_EXT > 17
    .enc_capa = IW_ENC_CAPA_WPA | IW_ENC_CAPA_WPA2 |
    IW_ENC_CAPA_CIPHER_TKIP | IW_ENC_CAPA_CIPHER_CCMP,
#endif

#ifdef IW_SCAN_CAPA_ESSID
    .scan_capa = IW_SCAN_CAPA_ESSID | IW_SCAN_CAPA_TYPE | IW_SCAN_CAPA_BSSID |
    IW_SCAN_CAPA_CHANNEL | IW_SCAN_CAPA_MODE | IW_SCAN_CAPA_RATE,
#endif
};

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

static int sta_hw_set_unicast_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
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

static int sta_hw_set_group_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
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

static int set_encryption(struct net_device *dev,
                          ieee_param *param, wf_u32 param_len)
{
    wf_u32 wep_key_idx, wep_key_len;
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;
    wdn_net_info_st *pwdn_info;
    int res = 0;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    param->u.crypt.err = 0;
    param->u.crypt.alg[IEEE_CRYPT_ALG_NAME_LEN - 1] = '\0';

    if (param_len !=
        WF_OFFSETOF(ieee_param, u.crypt.key) + param->u.crypt.key_len)
    {
        IW_FUNC_ERROR("param_len invalid !!!!!!!");
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

    if (!strcmp(param->u.crypt.alg, "WEP"))
    {
        wep_key_idx = param->u.crypt.idx;
        wep_key_len = param->u.crypt.key_len;

        if ((wep_key_idx > WEP_KEYS) || (wep_key_len == 0))
        {
            res = -EINVAL;
            goto exit;
        }

        psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;

        wep_key_len = wep_key_len <= 5 ? 5 : 13; /* 5B for wep40 and 13B for wep104 */
        if (wep_key_len == 13)
        {
            psec_info->dot11PrivacyAlgrthm = _WEP104_;
        }
        else
        {
            psec_info->dot11PrivacyAlgrthm = _WEP40_;
        }

        if (param->u.crypt.set_tx)
        {
            psec_info->dot11PrivacyKeyIndex = wep_key_idx;
        }
        memcpy(psec_info->dot11DefKey[wep_key_idx].skey,
               param->u.crypt.key, wep_key_len);
        psec_info->dot11DefKeylen[wep_key_idx] = wep_key_len;
        psec_info->key_mask |= BIT(wep_key_idx);
    }
    else if (psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) /* 802_1x */
    {
        local_info_st *plocal_info = pnic_info->local_info;
        if (plocal_info->work_mode == WF_INFRA_MODE) /* sta mode */
        {
            pwdn_info = wf_wdn_find_info(pnic_info,
                                         wf_wlan_get_cur_bssid(pnic_info));
            if (pwdn_info == NULL)
            {
                IW_FUNC_WARN("clear default key !!!");
                goto exit;
            }

            if (strcmp(param->u.crypt.alg, "none") != 0)
            {
                pwdn_info->ieee8021x_blocked = false;
            }

            if (psec_info->ndisencryptstatus == wf_ndis802_11Encryption2Enabled ||
                psec_info->ndisencryptstatus == wf_ndis802_11Encryption3Enabled)
            {
                pwdn_info->dot118021XPrivacy = psec_info->dot11PrivacyAlgrthm;
            }
            IW_FUNC_DBG("pwdn_info->dot118021XPrivacy = %d", pwdn_info->dot118021XPrivacy);

            wf_mcu_set_sec_cfg(pnic_info, psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X ? 0xcf : 0xcc);

            /* PTK: param->u.crypt.key */
            if (param->u.crypt.set_tx == 1) /* pairwise key */
            {
                IW_FUNC_DBG("set unicastkey");
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
                    IW_FUNC_DBG("sta_hw_set_unicast_key");
                    sta_hw_set_unicast_key(pnic_info, pwdn_info);
                }
            }
            else /* group key */
            {
                IW_FUNC_DBG("set groupkey");
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
                    IW_FUNC_DBG("sta_hw_set_group_key");
                    sta_hw_set_group_key(pnic_info, pwdn_info);
                }
            }
        }
    }

exit:
    return res;
}


static char *translate_scan_info(nic_info_st *pnic_info,
                                 struct iw_request_info *pinfo,
                                 wf_wlan_mgmt_scan_que_node_t *pscan_que_node,
                                 char *pstart, char *pstop)
{
    char *pstart_last = pstart;
    struct iw_event *piwe;
    wf_u16 max_rate = 0, rate;
    wf_u16 i = 0;
    wf_u8 ch;
    wf_u8 *p;
    wf_80211_mgmt_ie_t *pie;
    wf_u32 pie_len;
    char *buf = NULL;

    piwe = wf_vmalloc(sizeof(struct iw_event));
    if (piwe == NULL)
    {
        IW_FUNC_ERROR("\"struct iw_event\" malloc fail !!!");
        goto error;
    }

    /* AP MAC ADDRESS */
    piwe->cmd = SIOCGIWAP;
    piwe->u.ap_addr.sa_family = ARPHRD_ETHER;

    wf_memcpy(piwe->u.ap_addr.sa_data, pscan_que_node->bssid, ETH_ALEN);
    pstart = iwe_stream_add_event(pinfo, pstart, pstop, piwe, IW_EV_ADDR_LEN);

    /* add the ESSID */
    piwe->cmd = SIOCGIWESSID;
    piwe->u.data.flags = 1;
    piwe->u.data.length = (wf_u16)pscan_que_node->ssid.length;
    pstart = iwe_stream_add_point(pinfo, pstart, pstop, piwe,
                                  pscan_que_node->ssid.data);

    /* Add the protocol name */
    piwe->cmd = SIOCGIWNAME;
    switch (pscan_que_node->name)
    {
        case WF_WLAN_BSS_NAME_IEEE80211_B :
            snprintf(piwe->u.name, IFNAMSIZ, "IEEE 802.11b");
            break;
        case WF_WLAN_BSS_NAME_IEEE80211_G :
            snprintf(piwe->u.name, IFNAMSIZ, "IEEE 802.11g");
            break;
        case WF_WLAN_BSS_NAME_IEEE80211_BG :
            snprintf(piwe->u.name, IFNAMSIZ, "IEEE 802.11bg");
            break;
        case WF_WLAN_BSS_NAME_IEEE80211_BN :
            snprintf(piwe->u.name, IFNAMSIZ, "IEEE 802.11bn");
            break;
        case WF_WLAN_BSS_NAME_IEEE80211_GN :
            snprintf(piwe->u.name, IFNAMSIZ, "IEEE 802.11gn");
            break;
        case WF_WLAN_BSS_NAME_IEEE80211_BGN :
            snprintf(piwe->u.name, IFNAMSIZ, "IEEE 802.11bgn");
            break;
        case WF_WLAN_BSS_NAME_IEEE80211_A :
        case WF_WLAN_BSS_NAME_IEEE80211_AN :
        default :
            break;
    }
    pstart = iwe_stream_add_event(pinfo, pstart, pstop, piwe, IW_EV_CHAR_LEN);

    /* add mode */
    piwe->cmd = SIOCGIWMODE;
    if (pscan_que_node->opr_mode != WF_WLAN_OPR_MODE_MESH)
    {
        piwe->u.mode = pscan_que_node->opr_mode;
        pstart = iwe_stream_add_event(pinfo, pstart, pstop, piwe, IW_EV_UINT_LEN);
    }

    /* Add frequency/channel */
    piwe->cmd = SIOCGIWFREQ;
    ch = pscan_que_node->channel;
    if (ch >= 1 && ch <= 14)
    {
        if (ch == 14)
        {
            piwe->u.freq.m = 2484 * 100000;
        }
        else if (ch < 14)
        {
            piwe->u.freq.m = (2407 + ch * 5) * 100000;
        }
    }
    piwe->u.freq.e = 1;
    piwe->u.freq.i = pscan_que_node->channel;
    pstart = iwe_stream_add_event(pinfo, pstart, pstop, piwe, IW_EV_FREQ_LEN);

    /* Add encryption capability */
    piwe->cmd = SIOCGIWENCODE;
    if (pscan_que_node->privacy)
    {
        piwe->u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
    }
    else
    {
        piwe->u.data.flags = IW_ENCODE_DISABLED;
    }
    piwe->u.data.length = 0;
    pstart = iwe_stream_add_point(pinfo, pstart, pstop, piwe, pscan_que_node->ssid.data);

    /*Add basic and extended rates */
    max_rate = 0;
    while (pscan_que_node->spot_rate[i] != 0)
    {
        rate = pscan_que_node->spot_rate[i] & 0x7F;
        if (rate > max_rate)
        {
            max_rate = rate;
        }
        i++;
    }
    if (pscan_que_node->mcs & 0x8000)  /* MCS15 */
    {
        max_rate = pscan_que_node->bw_40mhz ?
                   (pscan_que_node->short_gi ? 300 : 270) :
                   (pscan_que_node->short_gi ? 144 : 130);
    }
    else if (pscan_que_node->mcs & 0x0080)     /* MCS7 */
    {
    }
    else     /* default MCS7 */
    {
        max_rate = (pscan_que_node->bw_40mhz) ?
                   (pscan_que_node->short_gi ? 150 : 135) :
                   (pscan_que_node->short_gi ? 72 : 65);
    }
    max_rate = max_rate * 2; /* Mbps/2; */

    piwe->cmd = SIOCGIWRATE;
    piwe->u.bitrate.fixed = 0;
    piwe->u.bitrate.disabled = 0;
    piwe->u.bitrate.value = max_rate * 500000;
    pstart = iwe_stream_add_event(pinfo, pstart, pstop, piwe, IW_EV_PARAM_LEN);

#define BUF_SIZE     (10 + WF_MAX(MAX_WPA_IE_LEN, MAX_RSN_IE_LEN) * 2) /* 2 for hex to string translate */
    buf = wf_vmalloc(BUF_SIZE);
    if (buf == NULL)
    {
        IW_FUNC_ERROR("wf_vmalloc fail !!!");
        goto error;
    }

    if (pscan_que_node->privacy)
    {
        /* parsing WPA/WPA2 */
        pie = (wf_80211_mgmt_ie_t *)pscan_que_node->wpa_ie;
        pie_len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
        if (pie->len > 0)
        {
            wf_memset(buf, '\0', BUF_SIZE);
            p = buf;
            p += sprintf(p, "wpa_ie=");
            for (i = 0; i < pie_len; i++)
            {
                p += sprintf(p, "%02x", ((wf_u8 *)pie)[i]);
            }

            wf_memset(piwe, 0, sizeof(struct iw_event));
            piwe->cmd = IWEVCUSTOM;
            piwe->u.data.length = strlen(buf);
            pstart = iwe_stream_add_point(pinfo, pstart, pstop, piwe, buf);

            wf_memset(piwe, 0, sizeof(struct iw_event));
            piwe->cmd = IWEVGENIE;
            piwe->u.data.length = pie_len;
            pstart =
                iwe_stream_add_point(pinfo, pstart, pstop, piwe, (char *)pie);
        }

        /* parsing rsn */
        pie = (wf_80211_mgmt_ie_t *)pscan_que_node->rsn_ie;
        pie_len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
        if (pie->len > 0)
        {
            wf_memset(buf, '\0', BUF_SIZE);
            p = buf;
            p += sprintf(p, "rsn_ie=");
            for (i = 0; i < pie_len; i++)
            {
                p += sprintf(p, "%02x", ((wf_u8 *)pie)[i]);
            }

            wf_memset(piwe, 0, sizeof(struct iw_event));
            piwe->cmd = IWEVCUSTOM;
            piwe->u.data.length = strlen(buf);
            pstart = iwe_stream_add_point(pinfo, pstart, pstop, piwe, buf);

            wf_memset(piwe, 0, sizeof(struct iw_event));
            piwe->cmd = IWEVGENIE;
            piwe->u.data.length = pie_len;
            pstart =
                iwe_stream_add_point(pinfo, pstart, pstop, piwe, (char *)pie);
        }

        /* parsing WPS */
        pie = (wf_80211_mgmt_ie_t *)pscan_que_node->wps_ie;
        pie_len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
        if (pie->len)
        {
            piwe->cmd = IWEVGENIE;
            piwe->u.data.length = pie_len;
            pstart =
                iwe_stream_add_point(pinfo, pstart, pstop, piwe, (char *)pie);
        }
    }

    /* Add rssi statistics */
    wf_memset(buf, '\0', BUF_SIZE);
    p = buf;
    p += sprintf(p, "rssi=");
    p += sprintf(p, "%hd ",
                 pscan_que_node->signal_strength_scale);
    wf_memset(piwe, 0, sizeof(struct iw_event));
    piwe->cmd = IWEVCUSTOM;
    piwe->u.data.length = strlen(buf);
    pstart = iwe_stream_add_point(pinfo, pstart, pstop, piwe, buf);

    /* Add quality statistics */
    piwe->cmd = IWEVQUAL;
    piwe->u.qual.updated =
        IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_UPDATED | IW_QUAL_NOISE_INVALID;
#ifdef CONFIG_OS_ANDROID
    piwe->u.qual.level = translate_percentage_to_dbm(pscan_que_node->signal_strength);
#else
#ifdef CONFIG_SIGNAL_SCALE_MAPPING
    piwe->u.qual.level = pscan_que_node->signal_strength;
#else
    piwe->u.qual.level = pscan_que_node->signal_strength_scale;
#endif
#endif
    piwe->u.qual.level = pscan_que_node->signal_strength_scale;
    piwe->u.qual.qual = pscan_que_node->signal_qual; /*  signal quality */
    piwe->u.qual.noise = 0; /*  noise level */
    pstart = iwe_stream_add_event(pinfo, pstart, pstop, piwe, IW_EV_QUAL_LEN);

    wf_vfree(piwe);
    wf_vfree(buf);

    return pstart;

error :
    if (piwe)
    {
        wf_vfree(piwe);
    }
    if (buf)
    {
        wf_vfree(buf);
    }

    return pstart_last;
}



int wf_iw_setCommit(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_getName(struct net_device *ndev, struct iw_request_info *info,
                  union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_bool is_connected = wf_false;
    wf_wlan_mgmt_scan_que_node_t *pscan_que_node;
    wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    wf_mlme_get_connect(pnic_info, &is_connected);
    if (is_connected == wf_true)
    {
        if (wf_local_cfg_get_work_mode(pnic_info) == WF_ADHOC_MODE)
        {
            snprintf(wrqu->name, IFNAMSIZ, "UNKNOWN");
            return 0;
        }
#ifdef CFG_ENABLE_AP_MODE
        else if (wf_local_cfg_get_work_mode(pnic_info) == WF_MASTER_MODE)
        {
            switch (pcur_network->cur_wireless_mode)
            {
                case WIRELESS_11B :
                    snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11b");
                    break;
                case WIRELESS_11G :
                    snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11g");
                    break;
                case WIRELESS_11BG :
                    snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bg");
                    break;
                case WIRELESS_11B_24N :
                    snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bn");
                    break;
                case WIRELESS_11G_24N :
                    snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11gn");
                    break;
                case WIRELESS_11BG_24N :
                    snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bgn");
                    break;
                case WF_WLAN_BSS_NAME_IEEE80211_A :
                case WF_WLAN_BSS_NAME_IEEE80211_AN :
                default :
                    break;
            }
            return 0;
        }
#endif
        else
        {
            wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
            {
                if (!wf_memcmp(pcur_network->bssid, pscan_que_node->bssid, ETH_ALEN))
                {
                    switch (pscan_que_node->name)
                    {
                        case WF_WLAN_BSS_NAME_IEEE80211_B :
                            snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11b");
                            break;
                        case WF_WLAN_BSS_NAME_IEEE80211_G :
                            snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11g");
                            break;
                        case WF_WLAN_BSS_NAME_IEEE80211_BG :
                            snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bg");
                            break;
                        case WF_WLAN_BSS_NAME_IEEE80211_BN :
                            snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bn");
                            break;
                        case WF_WLAN_BSS_NAME_IEEE80211_GN :
                            snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11gn");
                            break;
                        case WF_WLAN_BSS_NAME_IEEE80211_BGN :
                            snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bgn");
                            break;
                        case WF_WLAN_BSS_NAME_IEEE80211_A :
                        case WF_WLAN_BSS_NAME_IEEE80211_AN :
                        default :
                            break;
                    }
                    break;
                }
            }
            wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);

            if(scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_BREAK)
            {
                return 0;
            }
        }
    }

    snprintf(wrqu->name, IFNAMSIZ, "unassociated");

    return 0;
}

int wf_iw_setNetworkId(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_getNetworkId(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_ch_set_search_ch(wf_channel_info_t *ch_set, const wf_u32 ch)
{
    int i;
    for (i = 0; ch_set[i].channel_num != 0; i++)
    {
        if (ch == ch_set[i].channel_num)
        {
            break;
        }
    }

    if (i >= ch_set[i].channel_num)
    {
        return -1;
    }

    return i;
}

int wf_iw_setFrequency(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    hw_info_st *phw_info = pnic_info->hw_info;
    local_info_st *local_info = pnic_info->local_info;
    int exp = 1, freq = 0, div = 0;
    wf_u8 channel;

    if (wrqu->freq.m <= 1000)
    {
        if (wrqu->freq.flags == IW_FREQ_AUTO)
        {
            if(wf_iw_ch_set_search_ch(phw_info->channel_set, wrqu->freq.m) > 0)
            {
                IW_FUNC_DBG("channel is auto, set to channel %d", wrqu->freq.m);
                channel = wrqu->freq.m;
            }
            else
            {
                channel = 1;
                IW_FUNC_DBG("channel is auto, channelset not match just set to channel 1");
            }
        }
        else
        {
            channel = wrqu->freq.m;
            IW_FUNC_DBG("channel is't auto, set to channel == %d", channel);
        }
    }
    else
    {
        while (wrqu->freq.e)
        {
            exp *= 10;
            wrqu->freq.e--;
        }

        freq = wrqu->freq.m;

        while (!(freq % 10))
        {
            freq /= 10;
            exp *= 10;
        }

        div = 1000000 / exp;

        if (div)
        {
            freq /= div;
        }
        else
        {
            div = exp / 1000000;
            freq *= div;
        }
        if (freq == 2484)
        {
            channel = 14;
        }
        else if (freq < 2484)
        {
            channel = ((freq - 2407) / 5);
        }
        else
        {
            channel = 0;
        }
    }

    local_info->channel = channel;
    wf_hw_info_set_channnel_bw(pnic_info, channel, CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

    return 0;
}


int wf_iw_getFrequency(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    local_info_st *local_info = pnic_info->local_info;
    wf_bool is_connected = wf_false;
    wf_u8 cur_channel;

    wf_mlme_get_connect(pnic_info, &is_connected);

    if (((is_connected == wf_false)  &&
         (wf_local_cfg_get_work_mode(pnic_info) != WF_ADHOC_MODE)) ||
        (wf_local_cfg_get_work_mode(pnic_info) == WF_MONITOR_MODE))
    {
        cur_channel = local_info->channel;
    }
    else
    {
        cur_channel = wf_wlan_get_cur_channel(pnic_info);
    }

    if (cur_channel == 14)
    {
        wrqu->freq.m = 2484 * 100000;
    }
    else
    {
        wrqu->freq.m = (2407 + cur_channel * 5) * 100000;
    }

    wrqu->freq.e = 1;
    wrqu->freq.i = cur_channel;

    return 0;
}

int wf_iw_setOperationMode(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    local_info_st * plocal = (local_info_st *)pnic_info->local_info;
    wf_bool bConnect = wf_false;
#ifdef CFG_ENABLE_AP_MODE
    wf_wlan_mgmt_info_t *wlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *cur_network = &wlan_mgmt_info->cur_network;
#endif

    IW_FUNC_DBG("OpMode:%d", wrqu->mode);
    IW_FUNC_DBG("[wf_iw_setOperationMode]:mac:"WF_MAC_FMT,WF_MAC_ARG(ndev->dev_addr));

#ifdef CONFIG_LPS
    if(WF_RETURN_FAIL == wf_lps_wakeup(pnic_info, LPS_CTRL_SCAN, 0))
    {
        return -1;
    }
#endif
    if(plocal->work_mode == wrqu->mode)
    {
        return 0;
    }

    set_sys_work_mode(pnic_info, wrqu->mode);

    wf_mlme_get_connect(pnic_info, &bConnect);
    if(bConnect)
    {
        wf_mlme_deauth(pnic_info, wf_false, WF_80211_REASON_DEAUTH_LEAVING);
    }

    wf_mcu_set_op_mode(pnic_info, wrqu->mode);

    ndev->type = ARPHRD_ETHER;

    switch(wrqu->mode)
    {
        case WF_MASTER_MODE :
#ifdef CFG_ENABLE_AP_MODE
            cur_network->join_res = -1;
            wf_mlme_abort(pnic_info);
            break;
#endif

        case WF_MONITOR_MODE :
#ifdef CFG_ENABLE_MONITOR_MODE
            ndev->type = ARPHRD_IEEE80211_RADIOTAP;
            wf_mlme_abort(pnic_info);
            IW_FUNC_DBG("WF_MONITOR_MODE");
            break;
#endif

        case WF_INFRA_MODE :
        case WF_AUTO_MODE :
        default :
            break;
    }

#ifdef CFG_ENABLE_MONITOR_MODE
    if(wf_local_cfg_get_work_mode(pnic_info) == WF_MONITOR_MODE)
    {
        wf_mlme_set_connect(pnic_info, wf_true);
        wf_os_api_ind_connect(pnic_info, WF_MLME_FRAMEWORK_WEXT);
    }
#endif

    return 0;
}

int wf_iw_getOperationMode(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    local_info_st *local_info = pnic_info->local_info;

    wrqu->mode = local_info->work_mode;

    return 0;
}

int wf_iw_setSensitivity(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_getSensitivity(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));
    wrqu->sens.value = 0;
    wrqu->sens.fixed = 0;
    wrqu->sens.disabled = 1;

    return 0;
}


int wf_iw_setRange(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}


int wf_iw_getRange(struct net_device *ndev, struct iw_request_info *info,
                   union iwreq_data *wrqu, char *extra)
{
    struct iw_range *range = (struct iw_range *)extra;
    wireless_info_st *pwirl = NULL;
    int i;

    pwirl = &def_wireless_info;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    wrqu->data.length = sizeof(*range);
    wf_memset(range, 0, sizeof(*range));

    range->throughput = pwirl->throughput;

    range->max_qual.qual = pwirl->max_qual.qual;
    range->max_qual.level = pwirl->max_qual.level;
    range->max_qual.noise = pwirl->max_qual.noise;

    range->avg_qual.qual = pwirl->avg_qual.qual;
    range->avg_qual.level = pwirl->avg_qual.level;
    range->avg_qual.noise = pwirl->avg_qual.noise;

    range->num_bitrates = pwirl->num_bitrates;
    for (i = 0; i < pwirl->num_bitrates && i < IW_MAX_BITRATES; i++)
    {
        range->bitrate[i] = pwirl->bitrate[i];
    }

    range->min_frag = pwirl->min_frag;
    range->max_frag = pwirl->max_frag;

    range->pm_capa = 0;

    range->we_version_compiled = WIRELESS_EXT;
    range->we_version_source = 16;

    for (i = 0; i < pwirl->num_channels; i++)
    {
        range->freq[i].i = pwirl->pchannel_tab[i].num;
        range->freq[i].m = pwirl->pchannel_tab[i].freq * 100000;
        range->freq[i].e = 1;

        if (i == IW_MAX_FREQUENCIES)
        {
            break;
        }
    }

    range->num_channels = i;
    range->num_frequency = i;

    /*  The following code will proivde the security capability to network manager. */
    /*  If the driver doesn't provide this capability to network manager, */
    /*  the WPA/WPA2 routers can't be chosen in the network manager. */
#if WIRELESS_EXT > 17
    range->enc_capa = pwirl->enc_capa;
#endif

#ifdef IW_SCAN_CAPA_ESSID
    range->scan_capa = pwirl->scan_capa;
#endif

    return 0;
}


int wf_iw_setPriv(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
#if 1
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
#else
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;

    int res = 0, len = 0;
    char *ext;
    struct iw_point *dwrq = (struct iw_point *)wrqu;

    IW_FUNC_DBG();

    if (dwrq->length == 0)
        return -EFAULT;

    len = dwrq->length;
    if (!(ext = wf_vmalloc(len)))
        return -ENOMEM;

    if (copy_from_user(ext, dwrq->pointer, len))
    {
        wf_vfree(ext);
        return -EFAULT;
    }

    /* added for wps2.0 */
    if (dwrq->flags == 0x8766 && len > 8)
    {
        wf_u32 cp_sz;
        wf_wlan_mgmt_info_t *wps_priv = pnic_info->wlan_mgmt_info;
        wf_wlan_remote_t *probe_priv = &wps_priv->remote;
        wf_u8 *probereq_wpsie = ext;
        wf_u32 probereq_wpsie_len = len;
        wf_u8 wps_oui[4] = {0x0, 0x50, 0xf2, 0x04};

        if ((WF_80211_MGMT_EID_VENDOR_SPECIFIC == probereq_wpsie[0]) && (!wf_memcmp(&probereq_wpsie[2], wps_oui, 4)))
        {
            cp_sz = probereq_wpsie_len > WF_MAX_WPS_IE_LEN ? WF_MAX_WPS_IE_LEN : probereq_wpsie_len;
            probe_priv->security.wps_probe_req_ie_len = 0;
            wf_kfree(probe_priv->security.wps_probe_req_ie);
            probe_priv->security.wps_probe_req_ie = NULL;

            probe_priv->security.wps_probe_req_ie = wf_kzalloc(cp_sz);
            if (probe_priv->security.wps_probe_req_ie == NULL)
            {
                wf_vfree(ext);
                return -EINVAL;
            }

            wf_memcpy(probe_priv->security.wps_probe_req_ie, probereq_wpsie, cp_sz);
            probe_priv->security.wps_probe_req_ie_len = cp_sz;
        }
        wf_vfree(ext);
        return res;
    }
    if (len >= WLAN_CSCAN_HEADER_SIZE && !wf_memcmp(ext, WLAN_CSCAN_HEADER, WLAN_CSCAN_HEADER_SIZE))
    {
        res = wf_iw_setScan(ndev, info, wrqu, ext);
        wf_vfree(ext);
        return res;
    }

    wf_vfree(ext);
    return res;
#endif
}

int wf_iw_getPriv(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));


    return 0;
}

int wf_iw_setWirelessStats(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_getWirelessStats(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_statistics *piwstats = pnic_info->iwstats;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    piwstats->qual.qual = 0;
    piwstats->qual.level = 0;
    piwstats->qual.noise = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14))
    piwstats->qual.updated = IW_QUAL_ALL_UPDATED;
#else
    piwstats->qual.updated = 0x0f;
#endif

#ifdef CONFIG_SIGNAL_DISPLAY_DBM
    piwstats->qual.updated = piwstats->qual.updated | IW_QUAL_DBM;
#endif

    return 0;
}

int wf_iw_setSpyAddresses(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_getSpyInfo(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_setSpyThreshold(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_getSpyThreshold(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_setWap (struct net_device *ndev, struct iw_request_info *info,
                  union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct sockaddr *awrq = (struct sockaddr *)wrqu;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    if (pnic_info->buddy_nic)
    {
        mlme_state_e state;
        wf_mlme_get_state((nic_info_st *)(pnic_info->buddy_nic), &state);
        if (state == MLME_STATE_SCAN ||
            state == MLME_STATE_CONN_SCAN ||
            state == MLME_STATE_AUTH ||
            state == MLME_STATE_ASSOC)
        {
            IW_FUNC_INFO("buddy interface is under linking !");
            return -EINVAL;
        }
    }

    if (awrq->sa_family != ARPHRD_ETHER)
    {
        return -EINVAL;
    }

    if (!wf_80211_is_valid_bssid(awrq->sa_data))
    {
        IW_FUNC_DBG("clear bssid");
        wf_mlme_conn_abort(pnic_info, wf_false, WF_80211_REASON_DEAUTH_LEAVING);
        goto exit;
    }

    {
        wf_bool is_connected;
        wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
        wf_wlan_mgmt_scan_que_node_t *pscan_que_node;
        wf_wlan_ssid_t ssid;
        wf_u8 *pbssid = awrq->sa_data;

        wf_mlme_get_connect(pnic_info, &is_connected);
        if (is_connected)
        {
            if (wf_80211_is_same_addr(pbssid, wf_wlan_get_cur_bssid(pnic_info)))
            {
                IW_FUNC_DBG("the bssid as same as the current associate bssid");
                wf_os_api_ind_connect(pnic_info, WF_MLME_FRAMEWORK_WEXT);
                goto exit;
            }
        }

        IW_FUNC_DBG("bssid: "WF_MAC_FMT, WF_MAC_ARG(pbssid));

        wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
        {
            if (!wf_memcmp(pbssid, pscan_que_node->bssid, ETH_ALEN))
            {
                ssid.length = pscan_que_node->ssid.length;
                wf_memcpy(ssid.data, pscan_que_node->ssid.data, ssid.length);
                break;
            }
        }
        wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);

        if (scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_BREAK)
        {
            /* start connect */
            wf_mlme_conn_start(pnic_info, pbssid, &ssid,
                               WF_MLME_FRAMEWORK_WEXT, wf_true);
        }
        else if(scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_END)
        {
            /* start connect */
            wf_mlme_conn_start(pnic_info, pbssid, NULL,
                               WF_MLME_FRAMEWORK_WEXT, wf_true);
        }
    }

exit:
    return 0;
}

int wf_iw_getWap(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_bool is_connected;
    wf_u8 *curBssid;
    wdn_net_info_st *pwdn_info;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    wrqu->ap_addr.sa_family = ARPHRD_ETHER;

    wf_mlme_get_connect(pnic_info, &is_connected);
    if (is_connected)
    {
        if (wf_local_cfg_get_work_mode(pnic_info) == WF_ADHOC_MODE ||
            wf_local_cfg_get_work_mode(pnic_info) == WF_MASTER_MODE)
        {
            curBssid = wf_wlan_get_cur_bssid(pnic_info);
            wf_memcpy(wrqu->ap_addr.sa_data, curBssid, ETH_ALEN);
            IW_FUNC_DBG("bssid: "WF_MAC_FMT, WF_MAC_ARG(curBssid));
            return 0;
        }
        else if (wf_local_cfg_get_work_mode(pnic_info) == WF_INFRA_MODE)
        {
            /*check bssid in wdn */
            curBssid = pwlan_mgmt_info->cur_network.mac_addr;
            pwdn_info = wf_wdn_find_info(pnic_info, curBssid);
            if (pwdn_info == NULL)
            {
                IW_FUNC_ERROR("connection establishment, but can't find in wdn_info");
                return -1;
            }

            wf_memcpy(wrqu->ap_addr.sa_data, curBssid, ETH_ALEN);
            IW_FUNC_DBG("bssid: "WF_MAC_FMT, WF_MAC_ARG(curBssid));
            return 0;
        }
    }

    wf_memset(wrqu->ap_addr.sa_data, 0, ETH_ALEN);

    return 0;
}

int wf_iw_setMlme(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    struct iw_mlme *mlme = (struct iw_mlme *)extra;
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;

    if (mlme == NULL)
    {
        return -1;
    }

    IW_FUNC_DBG("cmd=%d, reason=%d", mlme->cmd, cpu_to_le16(mlme->reason_code));

    /* cleap up sec info */
    wf_memset(pnic_info->sec_info, 0x0, sizeof(sec_info_st));

#ifdef CONFIG_LPS
    if(WF_RETURN_FAIL == wf_lps_wakeup(pnic_info, LPS_CTRL_SCAN, 0))
    {
        return -1;
    }
#endif

    switch (mlme->cmd)
    {
        case IW_MLME_DEAUTH:
            IW_FUNC_DBG("IW_MLME_DEAUTH");
            wf_mlme_conn_abort(pnic_info, wf_false, WF_80211_REASON_DEAUTH_LEAVING);
            break;

        case IW_MLME_DISASSOC:
            IW_FUNC_DBG("IW_MLME_DISASSOC");
            wf_mlme_conn_abort(pnic_info, wf_false, WF_80211_REASON_DEAUTH_LEAVING);
            break;

        default:
            return -EOPNOTSUPP;
    }

    return 0;
}

int wf_iw_getWapList(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}


int wf_iw_setScan(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_bool is_connected, is_busy;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    if(pnic_info->buddy_nic)
    {
        mlme_state_e state;
        wf_mlme_get_state((nic_info_st *)(pnic_info->buddy_nic), &state);
        if (state == MLME_STATE_CONN_SCAN ||
            state == MLME_STATE_AUTH ||
            state == MLME_STATE_ASSOC)
        {
            IW_FUNC_INFO("interface or buddy interface is under linking !");
            wf_os_api_ind_scan_done(pnic_info, wf_true, WF_MLME_FRAMEWORK_WEXT);
            return 0;
        }
    }

    wf_mlme_get_connect(pndev_priv->nic, &is_connected);
    if (is_connected)
    {
        wf_mlme_get_traffic_busy(pndev_priv->nic, &is_busy);
        if (is_busy)
        {
            wf_os_api_ind_scan_done(pnic_info, wf_true, WF_MLME_FRAMEWORK_WEXT);

            return 0;
        }
        wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                           NULL, 0, NULL, 0,
                           WF_MLME_FRAMEWORK_WEXT);
    }
    else
    {
        wf_wlan_ssid_t ssids[WF_SCAN_REQ_SSID_NUM];
        wf_memset(ssids, 0, sizeof(ssids));
        if(wrqu->data.length == sizeof(struct iw_scan_req))
        {
            struct iw_scan_req *req = (struct iw_scan_req *)extra;
            if (wrqu->data.flags & IW_SCAN_THIS_ESSID)
            {
                int len = min((int)req->essid_len, IW_ESSID_MAX_SIZE);
                wf_memcpy(ssids[0].data, req->essid, len);
                ssids[0].length = len;

                IW_FUNC_DBG("ssid = %s, ssid_len = %d", ssids[0].data, ssids[0].length);

                wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                                   ssids, 1, NULL, 0,
                                   WF_MLME_FRAMEWORK_WEXT);
            }
        }
        else
        {
            wf_mlme_scan_start(pnic_info, SCAN_TYPE_ACTIVE,
                               NULL, 0, NULL, 0,
                               WF_MLME_FRAMEWORK_WEXT);
        }
    }

    return 0;
}

int wf_iw_getScan(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_wlan_mgmt_scan_que_node_t *pscan_que_node = NULL;
    char *ev = extra;
    char *stop = ev + wrqu->data.length;
    wf_u32 res = 0;
    wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
    wf_u16 apCount = 0;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));
    /* Check if there is space for one more entry */
    wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
    {
        if ((stop - ev) < WF_FIELD_SIZEOF(wf_wlan_mgmt_scan_que_node_t, ies))
        {
            res = -E2BIG;
            break;
        }
        ev = translate_scan_info(pnic_info, info, pscan_que_node, ev, stop);
        apCount++;
    }
    wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);

    wrqu->data.length = ev - extra;
    wrqu->data.flags = 0;

    if (scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL)
    {
        res = -EAGAIN;
    }
    else
    {
        IW_FUNC_DBG("<ap count = %d / scaned_list count=%d>", apCount,
                    wf_que_count(&((wf_wlan_mgmt_info_t *)pnic_info->wlan_mgmt_info)->scan_que.ready));
    }

    return res;
}

static wf_bool is_8021x_auth (wf_80211_mgmt_ie_t *pies, wf_u16 ies_len)
{
    wf_80211_mgmt_ie_t *pie;
    wf_u16 offset_len = 0;
    wf_u32 pmulticast_cipher, punicast_cipher;

    pie = pies;

    while(offset_len < ies_len)
    {
        switch(pie->element_id)
        {
            case WF_80211_MGMT_EID_RSN:
                IW_FUNC_DBG("RSN");
                return wf_true;
            case WF_80211_MGMT_EID_VENDOR_SPECIFIC:
                if (!wf_80211_mgmt_wpa_survey(pie,
                                              WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len,
                                              NULL, NULL,
                                              &pmulticast_cipher,
                                              &punicast_cipher))
                {
                    IW_FUNC_INFO("WPA");
                    return wf_true;
                }
                break;
            default:
                break;
        }

        offset_len += pie->len + WF_OFFSETOF(wf_80211_mgmt_ie_t, data);
        pie = (wf_80211_mgmt_ie_t *)(pie->data + pie->len);
    }

    IW_FUNC_INFO("NO 8021X");
    return wf_false;
}

int wf_iw_setEssid (struct net_device *ndev, struct iw_request_info *info,
                    union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u8 len;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    if (pnic_info->buddy_nic)
    {
        mlme_state_e state;
        wf_mlme_get_state((nic_info_st *)(pnic_info->buddy_nic), &state);
        if (state == MLME_STATE_SCAN ||
            state == MLME_STATE_CONN_SCAN ||
            state == MLME_STATE_AUTH ||
            state == MLME_STATE_ASSOC)
        {
            IW_FUNC_INFO("buddy interface is under linking !");
            return -EINVAL;
        }
    }

    if (wf_local_cfg_get_work_mode(pnic_info) == WF_MASTER_MODE)
    {
        IW_FUNC_ERROR("ap no support set_essid");
        return -EPERM;
    }

#if WIRELESS_EXT <= 20
    len = wrqu->essid.length - 1;
#else
    len = wrqu->essid.length;
#endif
    if (len > IW_ESSID_MAX_SIZE)
    {
        IW_FUNC_ERROR("ssid length %d too long", len);
        return -E2BIG;
    }

    if (len == IW_ESSID_MAX_SIZE)
    {
        IW_FUNC_DBG("clear essid");
        goto exit;
    }

    if (wrqu->essid.flags && wrqu->essid.length)
    {
        wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
        wf_wlan_mgmt_scan_que_node_t *pscan_que_node;
        wf_wlan_ssid_t ssid;
        wf_u8 *pbssid = NULL;

        /* retrive ssid */
        if (len >= sizeof(ssid.data))
        {
            return -EINVAL;
        }
        wf_memcpy(ssid.data, extra, ssid.length = len);
        ssid.data[ssid.length] = '\0';

        wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
        {
            if (wf_wlan_is_same_ssid(&pscan_que_node->ssid, &ssid))
            {
                pbssid = pscan_que_node->bssid;
                break;
            }
        }
        wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);

        if (scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL)
        {
            IW_FUNC_WARN("scan queue for each fail");
            return -EAGAIN;
        }
        else
        {
            wf_bool is_connected;
            wf_mlme_get_connect(pnic_info, &is_connected);
            if (is_connected)
            {
                if (pbssid &&
                    wf_80211_is_same_addr(wf_wlan_get_cur_bssid(pnic_info), pbssid))
                {
                    IW_FUNC_INFO("the essid as same as the current associate ssid");
                    wf_os_api_ind_connect(pnic_info, WF_MLME_FRAMEWORK_WEXT);
                    goto exit;
                }
            }

            if (psec_info->dot11PrivacyAlgrthm == _NO_PRIVACY_)
            {
                wf_memset(pnic_info->sec_info, 0x0, sizeof(sec_info_st));
            }
            else if (scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_BREAK)
            {
                wf_80211_mgmt_ie_t *pies =
                    (void *)((struct beacon_ie *)pscan_que_node->ies)->variable;
                wf_u16 ies_len = pscan_que_node->ie_len -
                                 WF_OFFSETOF(struct beacon_ie, variable);
                if (!is_8021x_auth(pies, ies_len) &&
                    psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X)
                {
                    IW_FUNC_DBG("clean sec info!!!");
                    wf_memset(pnic_info->sec_info, 0x0, sizeof(sec_info_st));
                }
            }

            /* start connection */
            wf_mlme_conn_start(pnic_info, pbssid, &ssid,
                               WF_MLME_FRAMEWORK_WEXT, wf_true);
        }
    }

exit :
    return 0;
}

int wf_iw_getEssid(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_bool is_connected = wf_false;
    wf_wlan_ssid_t *curSsid;
    wf_u8 *curBssid;
    wdn_net_info_st *pwdn_info;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    wf_mlme_get_connect(pnic_info, &is_connected);
    if (is_connected == wf_true)
    {
        /*check bssid in wdn */

        if(wf_local_cfg_get_work_mode(pnic_info) != WF_ADHOC_MODE)
        {
            curBssid = wf_wlan_get_cur_bssid(pnic_info);
            pwdn_info = wf_wdn_find_info(pnic_info, curBssid);
            if (pwdn_info == NULL)
            {
                return -1;
            }
            IW_FUNC_DBG("<ssid:%s>", pwdn_info->ssid);

            wrqu->essid.flags = 1;
            wrqu->essid.length = pwdn_info->ssid_len;
            wf_memcpy(extra, pwdn_info->ssid, wrqu->essid.length);
        }
        else
        {
            curSsid = wf_wlan_get_cur_ssid(pnic_info);

            wrqu->essid.flags = 1;
            wrqu->essid.length = curSsid->length;
            wf_memcpy(extra, curSsid->data, wrqu->essid.length);
            IW_FUNC_DBG("<ssid:%s>", extra);
        }
    }
    else
    {
        IW_FUNC_DBG("<ssid:NULL>");
        return -1;
    }

    return 0;
}

int wf_iw_getNick(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    if(extra)
    {
        wrqu->data.length = 12;
        wrqu->data.flags = 1;
        wf_memcpy(extra, "<WIFI@SCICS>", 12);
    }

    return 0;
}

int wf_iw_setRate(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    wf_u8 datarates[13];
    wf_u32 target_rate = wrqu->bitrate.value;
    wf_u32 fixed = wrqu->bitrate.fixed;
    wf_u32 ratevalue = 0;
    wf_u8 mpdatarate[13] = { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff };
    wf_u32 i = 0;
    wf_u32 res = 0;

    IW_FUNC_DBG("target_rate:%d  fixed:%d", target_rate, fixed);

    if (target_rate == -1)
    {
        ratevalue = 11;
        goto set_rate;
    }
    target_rate = target_rate / 100000;

    switch (target_rate)
    {
        case 10:
            ratevalue = 0;
            break;
        case 20:
            ratevalue = 1;
            break;
        case 55:
            ratevalue = 2;
            break;
        case 60:
            ratevalue = 3;
            break;
        case 90:
            ratevalue = 4;
            break;
        case 110:
            ratevalue = 5;
            break;
        case 120:
            ratevalue = 6;
            break;
        case 180:
            ratevalue = 7;
            break;
        case 240:
            ratevalue = 8;
            break;
        case 360:
            ratevalue = 9;
            break;
        case 480:
            ratevalue = 10;
            break;
        case 540:
            ratevalue = 11;
            break;
        default:
            ratevalue = 11;
            break;
    }

set_rate:

    for (i = 0; i < 13; i++)
    {
        if (ratevalue == mpdatarate[i])
        {
            datarates[i] = mpdatarate[i];
            if (fixed == 0)
            {
                break;
            }
        }
        else
        {
            datarates[i] = 0xff;
        }

    }

    /*if (proc_setdatarate_cmd_func(pnic_info, datarates, 1) != wf_true) {
        res = -1;
    }*/

    return res;
}


int wf_iw_getRate(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    u16 max_rate = 0;
    int ret = 0;
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;

    ret = wf_wlan_get_max_rate(pnic_info, wf_wlan_get_cur_bssid(pnic_info),&max_rate);
    if(-1 == ret)
    {
        return -EPERM;
    }

    //IW_FUNC_DBG("max_rate = %d", max_rate);

    if (max_rate == 0)
    {
        return -EPERM;
    }

    wrqu->bitrate.fixed = 0;
    wrqu->bitrate.value = max_rate * 100000;


    return 0;
}

int wf_iw_setRts(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv  = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    hw_info_st *rts_priv = (hw_info_st *)pnic_info->hw_info;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));


    if (wrqu->rts.disabled)
    {
        rts_priv->rts_thresh = 2347;
    }
    else
    {
        if ((wrqu->rts.value < 0) ||
            (wrqu->rts.value > 2347))
        {
            return -EINVAL;
        }

        rts_priv->rts_thresh = wrqu->rts.value;
    }

    IW_FUNC_DBG("rts_thresh=%d", rts_priv->rts_thresh);

    return 0;
}

int wf_iw_getRts(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv  = netdev_priv(ndev);
    hw_info_st *rts_priv = pndev_priv->nic->hw_info;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pndev_priv->nic)));

    IW_FUNC_DBG("rts_thresh=%d", rts_priv->rts_thresh);
    wrqu->rts.value = rts_priv->rts_thresh;
    wrqu->rts.fixed = 0;

    return 0;
}

int wf_iw_setFragmentation(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    hw_info_st *frag_priv = (hw_info_st *)pnic_info->hw_info;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    if (wrqu->frag.disabled)
    {
        frag_priv->frag_thresh = MAX_FRAG_THRESHOLD;
    }
    else
    {
        if (wrqu->frag.value < MIN_FRAG_THRESHOLD ||
            wrqu->frag.value > MAX_FRAG_THRESHOLD)
        {
            return  -EINVAL;
        }

        frag_priv->frag_thresh = wrqu->frag.value & ~0x1;
    }

    IW_FUNC_DBG("frag_len=%d", frag_priv->frag_thresh);

    return 0;
}

int wf_iw_getFragmentation(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    hw_info_st *frag_priv = pndev_priv->nic->hw_info;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pndev_priv->nic)));

    IW_FUNC_DBG("frag_len=%d", frag_priv->frag_thresh);
    wrqu->frag.value = frag_priv->frag_thresh;
    wrqu->frag.fixed = 0;

    return 0;
}


int wf_iw_getTransmitPower(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_setRetry(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_getRetry(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    wrqu->retry.value = 7;
    wrqu->retry.fixed = 0;
    wrqu->retry.disabled = 1;

    return 0;
}

int wf_iw_setEnc(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    wf_u32 key;
    wf_u32 keyindex_provided;
    signed int keyid;
    wl_ndis_802_11_wep_st wep;
    wf_ndis_802_11_auth_mode_e authmode;

    struct iw_point *erq = &(wrqu->encoding);
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *sec_info = pnic_info->sec_info;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));
    IW_FUNC_DBG(",flags=0x%x", erq->flags);

    memset(&wep, 0, sizeof(wl_ndis_802_11_wep_st));

    key = erq->flags & IW_ENCODE_INDEX;


    if (erq->flags & IW_ENCODE_DISABLED)
    {
        IW_FUNC_DBG("EncryptionDisabled");
        sec_info->ndisencryptstatus = wf_ndis802_11EncryptionDisabled;
        sec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
        sec_info->dot118021XGrpPrivacy = _NO_PRIVACY_;
        sec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
        authmode = wf_ndis802_11AuthModeOpen;
        sec_info->ndisauthtype = authmode;

        return 0;
    }

    if (key)
    {
        if (key > WEP_KEYS)
        {
            return -EINVAL;
        }
        key--;
        keyindex_provided = 1;
    }
    else
    {
        keyindex_provided = 0;
        key = sec_info->dot11PrivacyKeyIndex;
        IW_FUNC_DBG(", key=%d", key);
    }

    if (erq->flags & IW_ENCODE_OPEN)
    {
        IW_FUNC_DBG("IW_ENCODE_OPEN");
        sec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;

        sec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;

        sec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
        sec_info->dot118021XGrpPrivacy = _NO_PRIVACY_;
        authmode = wf_ndis802_11AuthModeOpen;
        sec_info->ndisauthtype = authmode;
    }
    else if (erq->flags & IW_ENCODE_RESTRICTED)
    {
        IW_FUNC_DBG("IW_ENCODE_RESTRICTED");
        sec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;

        sec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;

        sec_info->dot11PrivacyAlgrthm = _WEP40_;
        sec_info->dot118021XGrpPrivacy = _WEP40_;
        authmode = wf_ndis802_11AuthModeShared;
        sec_info->ndisauthtype = authmode;
    }
    else
    {
        IW_FUNC_DBG(",erq->flags=0x%x", erq->flags);

        sec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;
        sec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
        sec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
        sec_info->dot118021XGrpPrivacy = _NO_PRIVACY_;
        authmode = wf_ndis802_11AuthModeOpen;
        sec_info->ndisauthtype = authmode;
    }

    wep.KeyIndex = key;
    if (erq->length > 0)
    {
        wep.KeyLength = erq->length <= 5 ? 5 : 13;

        wep.Length = wep.KeyLength + WF_OFFSETOF(wl_ndis_802_11_wep_st, KeyMaterial);
    }
    else
    {
        wep.KeyLength = 0;

        if (keyindex_provided == 1)
        {
            sec_info->dot11PrivacyKeyIndex = key;

            IW_FUNC_DBG(",(keyindex_provided == 1), keyid=%d, key_len=%d", key,
                        sec_info->dot11DefKeylen[key]);

            switch (sec_info->dot11DefKeylen[key])
            {
                case 5:
                    sec_info->dot11PrivacyAlgrthm = _WEP40_;
                    break;
                case 13:
                    sec_info->dot11PrivacyAlgrthm = _WEP104_;
                    break;
                default:
                    sec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
                    break;
            }

            return 0;

        }

    }

    wep.KeyIndex |= 0x80000000;

    memcpy(wep.KeyMaterial, extra, wep.KeyLength);

    keyid = wep.KeyIndex & 0x3fffffff;

    if (keyid >= 4)
    {
        IW_FUNC_ERROR("keyid >= 4,false");
        return -1;
    }

    switch (wep.KeyLength)
    {
        case 5:
            sec_info->dot11PrivacyAlgrthm = _WEP40_;
            break;
        case 13:
            sec_info->dot11PrivacyAlgrthm = _WEP104_;
            break;
        default:
            sec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
            break;
    }

    memcpy(&(sec_info->dot11DefKey[keyid].skey[0]),
           &(wep.KeyMaterial), wep.KeyLength);

    sec_info->dot11DefKeylen[keyid] = wep.KeyLength;

    sec_info->dot11PrivacyKeyIndex = keyid;

    return 0;
}

int wf_iw_getEnc(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    wf_u32 key;
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    struct iw_point *erq = &(wrqu->encoding);
    sec_info_st *sec_info = pnic_info->sec_info;
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    key = erq->flags & IW_ENCODE_INDEX;

    if (key)
    {
        if (key > WEP_KEYS)
        {
            return -EINVAL;
        }
        key--;
    }
    else
    {
        key = sec_info->dot11PrivacyKeyIndex;
    }

    erq->flags = key + 1;

    switch (sec_info->ndisencryptstatus)
    {
        case wf_ndis802_11EncryptionNotSupported:
        case wf_ndis802_11EncryptionDisabled:

            erq->length = 0;
            erq->flags |= IW_ENCODE_DISABLED;

            break;

        case wf_ndis802_11Encryption1Enabled:

            erq->length = sec_info->dot11DefKeylen[key];

            if (erq->length)
            {
                memcpy(extra, sec_info->dot11DefKey[key].skey,
                       sec_info->dot11DefKeylen[key]);

                erq->flags |= IW_ENCODE_ENABLED;

                if (sec_info->ndisauthtype == wf_ndis802_11AuthModeOpen)
                {
                    erq->flags |= IW_ENCODE_OPEN;
                }
                else if (sec_info->ndisauthtype ==
                         wf_ndis802_11AuthModeShared)
                {
                    erq->flags |= IW_ENCODE_RESTRICTED;
                }
            }
            else
            {
                erq->length = 0;
                erq->flags |= IW_ENCODE_DISABLED;
            }

            break;

        case wf_ndis802_11Encryption2Enabled:
        case wf_ndis802_11Encryption3Enabled:

            erq->length = 16;
            erq->flags |= (IW_ENCODE_ENABLED | IW_ENCODE_OPEN | IW_ENCODE_NOKEY);

            break;

        default:
            erq->length = 0;
            erq->flags |= IW_ENCODE_DISABLED;

            break;

    }

    return 0;
}

int wf_iw_setPower(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_getPower(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    wrqu->power.value = 0;
    wrqu->power.fixed = 0;
    wrqu->power.disabled = 1;

    return 0;
}

int wf_iw_set_wpa_ie (nic_info_st *pnic_info, wf_u8 *pie, size_t ielen)
{
    sec_info_st *sec_info = pnic_info->sec_info;
    wf_u8 *buf = NULL;
    int group_cipher = 0, pairwise_cipher = 0;
    u16 cnt = 0;
    wf_u8 eid, wps_oui[4] = { 0x0, 0x50, 0xf2, 0x04 };
    int res = 0;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

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
        IW_FUNC_DBG("wpa_ie(length=%d)=", (wf_u16)ielen);
        IW_FUNC_ARRAY(buf, ielen);

        if (ielen < RSN_HEADER_LEN)
        {
            IW_FUNC_ERROR("Ie len too short(%d)", (wf_u16)ielen);
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
                IW_FUNC_DBG("dot118021XGrpPrivacy=_TKIP_");
                break;
            case CIPHER_SUITE_CCMP:
                sec_info->dot118021XGrpPrivacy = _AES_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
                IW_FUNC_DBG("dot118021XGrpPrivacy=_AES_");
                break;
        }

        switch (pairwise_cipher)
        {
            case CIPHER_SUITE_NONE:
                //                sec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
                //                sec_info->ndisencryptstatus = wf_ndis802_11EncryptionDisabled;
                //                IW_FUNC_DBG("dot11PrivacyAlgrthm=_NO_PRIVACY_");
                break;
            case CIPHER_SUITE_TKIP:
                sec_info->dot11PrivacyAlgrthm = _TKIP_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption2Enabled;
                IW_FUNC_DBG("dot11PrivacyAlgrthm=_TKIP_");
                break;
            case CIPHER_SUITE_CCMP:
                sec_info->dot11PrivacyAlgrthm = _AES_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
                IW_FUNC_DBG("dot11PrivacyAlgrthm=_AES_");
                break;
        }

        while (cnt < ielen)
        {
            eid = buf[cnt];
            if (eid == WF_80211_MGMT_EID_VENDOR_SPECIFIC &&
                !memcmp(&buf[cnt + 2], wps_oui, 4))
            {
                IW_FUNC_DBG("SET WPS_IE");
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

int wf_iw_setGenIe(struct net_device *ndev, struct iw_request_info *info,
                   union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    int res = 0;

    LOG_D("VALUE:%s",extra);
    LOG_D("len:%d",wrqu->data.length);
    res= wf_iw_set_wpa_ie(pnic_info, extra, wrqu->data.length);

    return res;
}

int wf_iw_getGenIe(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

static int wpa_set_auth_algs(struct net_device *ndev, wf_u32 value)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;
    int res = 0;

    if ((value & AUTH_ALG_SHARED_KEY) && (value & AUTH_ALG_OPEN_SYSTEM))
    {
        IW_FUNC_DBG("AUTH_ALG_SHARED_KEY and  AUTH_ALG_OPEN_SYSTEM [value:0x%x]", value);
        psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;
        psec_info->ndisauthtype = wf_ndis802_11AuthModeAutoSwitch;
        psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
    }
    else if (value & AUTH_ALG_SHARED_KEY)
    {
        IW_FUNC_DBG("AUTH_ALG_SHARED_KEY  [value:0x%x]", value);
        psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;
        psec_info->ndisauthtype = wf_ndis802_11AuthModeShared;
        psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;
    }
    else if (value & AUTH_ALG_OPEN_SYSTEM)
    {
        IW_FUNC_DBG("AUTH_ALG_OPEN_SYSTEM  [value:0x%x]", value);
        psec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;
        psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
    }
    else if (value & AUTH_ALG_LEAP)
    {
        IW_FUNC_DBG("AUTH_ALG_LEAP  [value:0x%x]", value);
    }
    else
    {
        IW_FUNC_DBG("error!");
        res = -EINVAL;
    }

    return res;
}

int wf_iw_setAuth(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;
    sec_info_st *psec_info = pnic_info->sec_info;
    struct iw_param *param = (struct iw_param *)&wrqu->param;
    int res = 0;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(pnic_info)));

    switch (param->flags & IW_AUTH_INDEX)
    {
        case IW_AUTH_MFP:
            IW_FUNC_DBG("IW_AUTH_MFP");
            break;

        case IW_AUTH_WPA_VERSION:
            IW_FUNC_DBG("IW_AUTH_WPA_VERSION");
            break;

        case IW_AUTH_CIPHER_PAIRWISE:
            IW_FUNC_DBG("IW_AUTH_CIPHER_PAIRWISE");
            break;

        case IW_AUTH_CIPHER_GROUP:
            IW_FUNC_DBG("IW_AUTH_CIPHER_GROUP");
            break;

        case IW_AUTH_KEY_MGMT:
            IW_FUNC_DBG("IW_AUTH_KEY_MGMT");
            break;

        case IW_AUTH_TKIP_COUNTERMEASURES:
            IW_FUNC_DBG("IW_AUTH_TKIP_COUNTERMEASURES");
            if (param->value)
            {
                /*  wpa_supplicant is enabling the tkip countermeasure. */
                psec_info->btkip_countermeasure = true;
            }
            else
            {
                /*  wpa_supplicant is disabling the tkip countermeasure. */
                psec_info->btkip_countermeasure = false;
            }
            break;

        case IW_AUTH_DROP_UNENCRYPTED:
            IW_FUNC_DBG("IW_AUTH_DROP_UNENCRYPTED");
            /* HACK:
             *
             * wpa_supplicant calls set_wpa_enabled when the driver
             * is loaded and unloaded, regardless of if WPA is being
             * used.  No other calls are made which can be used to
             * determine if encryption will be used or not prior to
             * association being expected.  If encryption is not being
             * used, drop_unencrypted is set to false, else true -- we
             * can use this to determine if the CAP_PRIVACY_ON bit should
             * be set.
             */

            if (psec_info->ndisencryptstatus == wf_ndis802_11Encryption1Enabled)
            {
                break;/* it means init value, or using wep,
                         ndisencryptstatus = wf_ndis802_11Encryption1Enabled, */
            }

            /*  then it needn't reset it; */
            if (param->value)
            {
                psec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;
                psec_info->ndisencryptstatus = wf_ndis802_11EncryptionDisabled;
                psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
                psec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
                psec_info->dot118021XGrpPrivacy = _NO_PRIVACY_;
            }
            break;

        case IW_AUTH_80211_AUTH_ALG:
            IW_FUNC_DBG("IW_AUTH_80211_AUTH_ALG");
            res = wpa_set_auth_algs(ndev, (wf_u32)param->value);
            break;

        case IW_AUTH_WPA_ENABLED:
            IW_FUNC_DBG("IW_AUTH_WPA_ENABLED");
            break;

        case IW_AUTH_RX_UNENCRYPTED_EAPOL:
            IW_FUNC_DBG("IW_AUTH_RX_UNENCRYPTED_EAPOL");
            break;

        case IW_AUTH_PRIVACY_INVOKED:
            IW_FUNC_DBG("IW_AUTH_PRIVACY_INVOKED");
            break;

        default:
            res = -EOPNOTSUPP;
    }

    return res;
}

int wf_iw_getAuth(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_setTransmitPower(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

int wf_iw_setEncExt(struct net_device *ndev,
                    struct iw_request_info *info,
                    union iwreq_data *wrqu, char *extra)
{
    char *alg_name;
    wf_u32 param_len;
    ieee_param *param = NULL;
    struct iw_point *pencoding = &wrqu->encoding;
    struct iw_encode_ext *pext = (struct iw_encode_ext *)extra;
    int res = 0;

    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    param_len = WF_OFFSETOF(ieee_param, u.crypt.key) + pext->key_len;
    param = (ieee_param *)wf_vmalloc(param_len);
    if (param == NULL)
    {
        LOG_E("[%s]: no memory for param", __func__);
        res = -EPERM;
        goto exit;
    }
    memset(param, 0, param_len);
    memset(param->sta_addr, 0xff, ETH_ALEN);

    switch (pext->alg)
    {
        case IW_ENCODE_ALG_NONE:
            alg_name = "none";
            break;
        case IW_ENCODE_ALG_WEP:
            alg_name = "WEP";
            break;
        case IW_ENCODE_ALG_TKIP:
            alg_name = "TKIP";
            break;
        case IW_ENCODE_ALG_CCMP:
            alg_name = "CCMP";
            break;
        default:
            if (param)
            {
                LOG_E("[%s]: Invalid alg name.", __func__);
                res = -EPERM;
                goto exit;
            }
    }

    strncpy((char *)param->u.crypt.alg, alg_name, IEEE_CRYPT_ALG_NAME_LEN);
    IW_FUNC_DBG("alg_name=%s", alg_name);

    if (pext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY)
    {
        param->u.crypt.set_tx = 1;
    }

    if (pext->alg != IW_ENCODE_ALG_WEP &&
        pext->ext_flags & IW_ENCODE_EXT_GROUP_KEY)
    {
        param->u.crypt.set_tx = 0;
    }

    param->u.crypt.idx = (pencoding->flags & 0x00FF) - 1;
    IW_FUNC_DBG("crypt.idx=%d", param->u.crypt.idx);

    if (pext->ext_flags & IW_ENCODE_EXT_RX_SEQ_VALID)
    {
        memcpy(param->u.crypt.seq, pext->rx_seq, sizeof(param->u.crypt.seq));
    }

    if (pext->key_len)
    {
        param->u.crypt.key_len = pext->key_len;
        memcpy(param->u.crypt.key, pext->key, pext->key_len);
        IW_FUNC_DBG("crypt.key=");
        IW_FUNC_ARRAY(param->u.crypt.key, pext->key_len);
    }

    res = set_encryption(ndev, param, param_len);

exit :
    if (param)
    {
        wf_vfree((wf_u8 *)param);
    }

    return res;
}

int wf_iw_getEncExt(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
}

/*
 *  There are the BSSID information in the bssid.sa_data array.
 *  If cmd is IW_PMKSA_FLUSH, it means the wpa_supplicant wants to clear
 *  all the PMKID information. If cmd is IW_PMKSA_ADD, it means the
 *  wpa_supplicant wants to add a PMKID/BSSID to driver.
 *  If cmd is IW_PMKSA_REMOVE, it means the wpa_supplicant wants to
 *  remove a PMKID/BSSID from driver.
 */
int wf_iw_setPmkid(struct net_device *ndev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
#if 1
    IW_FUNC_DBG("mac addr : "WF_MAC_FMT,WF_MAC_ARG(nic_to_local_addr(((ndev_priv_st *)netdev_priv(ndev))->nic)));

    return 0;
#else
    ndev_priv_st *ndev_priv = netdev_priv(ndev);
    sec_info_st *psec_info = ndev_priv->nic->sec_info;
    wf_u8   j, blInserted = false;
    struct iw_pmksa *pPMK = (struct iw_pmksa *)extra;
    wf_u8 strZeroMacAddress[ETH_ALEN] = {0x00};
    wf_u8 strIssueBssid[ETH_ALEN] = {0x00};

    IW_FUNC_DBG();

    //have oops error ,need check - joy
    return 0;

    wf_memcpy(strIssueBssid, pPMK->bssid.sa_data, ETH_ALEN);
    if (pPMK->cmd == IW_PMKSA_ADD)
    {
        if (!wf_memcmp(strIssueBssid, strZeroMacAddress, ETH_ALEN))
            return false;
        blInserted = false;

        /* overwrite PMKID */
        for (j = 0; j < WF_NUM_PMKID_CACHE; j++)
        {
            if (!wf_memcmp(psec_info->PMKIDList[j].Bssid, strIssueBssid, ETH_ALEN))
            {
                /*  BSSID is matched, the same AP => rewrite with new PMKID. */
                wf_memcpy(psec_info->PMKIDList[j].PMKID, pPMK->pmkid, IW_PMKID_LEN);
                psec_info->PMKIDList[j].bUsed = true;
                psec_info->PMKIDIndex = j + 1;
                blInserted = true;
                break;
            }
        }

        if (!blInserted)
        {
            /*  Find a new entry */
            wf_memcpy(psec_info->PMKIDList[psec_info->PMKIDIndex].Bssid, strIssueBssid, ETH_ALEN);
            wf_memcpy(psec_info->PMKIDList[psec_info->PMKIDIndex].PMKID, pPMK->pmkid, IW_PMKID_LEN);

            psec_info->PMKIDList[psec_info->PMKIDIndex].bUsed = true;
            psec_info->PMKIDIndex++;
            if (psec_info->PMKIDIndex == 16)
                psec_info->PMKIDIndex = 0;
        }
        return true;
    }
    else if (pPMK->cmd == IW_PMKSA_REMOVE)
    {
        for (j = 0; j < WF_NUM_PMKID_CACHE; j++)
        {
            if (!wf_memcmp(psec_info->PMKIDList[j].Bssid, strIssueBssid, ETH_ALEN))
            {
                memset(psec_info->PMKIDList[j].Bssid, 0x00, ETH_ALEN);
                psec_info->PMKIDList[j].bUsed = false;
                break;
            }
        }
    }
    else if (pPMK->cmd == IW_PMKSA_FLUSH)
    {
        wf_memset(&(psec_info->PMKIDList[0]), 0x00, (sizeof(SEC_PMKID_LIST) * WF_NUM_PMKID_CACHE));
        psec_info->PMKIDIndex = 0;
        return true;
    }

    return false;
#endif
}


#if 0
static int wf_iw_p2p_set(struct net_device *dev,struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
    int ret = 0;
    ndev_priv_st *pndev_priv = netdev_priv(ndev);
    nic_info_st *pnic_info = pndev_priv->nic;

    wl_dev *wl_adpt = (wl_dev *) WF_WLAN_NETDEV_PRIV(dev);
    struct wl_mgt_data *pmgt_data = &(wl_adpt->wl_mgtdata);
    struct iw_point *pdata = &wrqu->data;
    p2p_wd_info_st *pwdinfo = &(wl_adpt->wdinfo);
    struct mgt_data_ext *mgtdata_ext = &wl_adpt->mgtext_data;

    IW_FUNC_DBG("[%s] extra = %s\n", __FUNCTION__, extra);

    if (0 == wf_memcmp(extra, "enable=", 7))
    {
        wl_wext_p2p_enable(dev, info, wrqu, &extra[7]);
    }
    else if (0 == wf_memcmp(extra, "setDN=", 6))
    {
        wrqu->data.length -= 6;
        wl_p2p_put_up_dn(dev, info, wrqu, &extra[6]);
    }
    else if (0 == wf_memcmp(extra, "profilefound=", 13))
    {
        wrqu->data.length -= 13;
        wl_p2p_profilefound(dev, info, wrqu, &extra[13]);
    }
    else if (0 == wf_memcmp(extra, "prov_disc=", 10))
    {
        wrqu->data.length -= 10;
        wl_p2p_prov_disc(dev, info, wrqu, &extra[10]);
    }
    else if (0 == wf_memcmp(extra, "nego=", 5))
    {
        wrqu->data.length -= 5;
        wl_p2p_connect(dev, info, wrqu, &extra[5]);
    }
    else if (0 == wf_memcmp(extra, "intent=", 7))
    {
        wrqu->data.length -= 8;
        wl_p2p_put_up_intent(dev, info, wrqu, &extra[7]);
    }
    else if (0 == wf_memcmp(extra, "ssid=", 5))
    {
        wrqu->data.length -= 5;
        wl_p2p_put_up_go_nego_ssid(dev, info, wrqu, &extra[5]);
    }
    else if (0 == wf_memcmp(extra, "got_wpsinfo=", 12))
    {
        wrqu->data.length -= 12;
        wl_p2p_got_wpsinfo(dev, info, wrqu, &extra[12]);
    }
    else if (0 == wf_memcmp(extra, "listen_ch=", 10))
    {
        wrqu->data.length -= 11;
        wl_p2p_put_up_listen_ch(dev, info, wrqu, &extra[10]);
    }
    else if (0 == wf_memcmp(extra, "op_ch=", 6))
    {
        wrqu->data.length -= 7;
        wl_p2p_put_up_op_ch(dev, info, wrqu, &extra[6]);
    }
    else if (0 == wf_memcmp(extra, "invite=", 7))
    {
        wrqu->data.length -= 8;
        wl_p2p_invite_req(dev, info, wrqu, &extra[7]);
    }
    else if (0 == wf_memcmp(extra, "persistent=", 11))
    {
        wrqu->data.length -= 11;
        wl_p2p_put_up_persistent(dev, info, wrqu, &extra[11]);
    }
    else if (0 == wf_memcmp(extra, "uuid=", 5))
    {
        wrqu->data.length -= 5;
        ret = wl_p2p_put_up_wps_uuid(dev, info, wrqu, &extra[5]);
    }
#ifdef CONFIG_WFD
    if (wl_get_func_hand(wl_adpt, WL_FUNC_MIRACAST))
    {
        if (0 == wf_memcmp(extra, "sa=", 3))
        {
            wrqu->data.length -= 3;
            wl_p2p_put_up_sa(dev, info, wrqu, &extra[3]);
        }
        else if (0 == wf_memcmp(extra, "pc=", 3))
        {
            wrqu->data.length -= 3;
            wl_p2p_put_up_pc(dev, info, wrqu, &extra[3]);
        }
        else if (0 == wf_memcmp(extra, "wfd_type=", 9))
        {
            wrqu->data.length -= 9;
            wl_p2p_put_up_wfd_device_type(dev, info, wrqu, &extra[9]);
        }
        else if (0 == wf_memcmp(extra, "wfd_enable=", 11))
        {
            wrqu->data.length -= 11;
            wl_p2p_put_up_wfd_enable(dev, info, wrqu, &extra[11]);
        }
        else if (0 == wf_memcmp(extra, "driver_iface=", 13))
        {
            wrqu->data.length -= 13;
            wl_p2p_put_up_driver_iface(dev, info, wrqu, &extra[13]);
        }
    }
#endif
    return ret;

}
#endif

