
/* include */
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL  WF_DEBUG_MASK
#include "common.h"
#include "hif/hif.h"
#include "trx/tx_rtos.h"
#include "sec/wpa.h"
#include "sec/utils/common.h"
#include "sec/common/wpa_common.h"
#include "sec/crypto/crypto.h"
#include "wifi/wf_wifi.h"

/* macro */
#define WIFI_DBG(fmt, ...)      OS_LOG_D("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WIFI_INFO(fmt, ...)     OS_LOG_I("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WIFI_WARN(fmt, ...)     OS_LOG_W("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WIFI_ERROR(fmt, ...)    OS_LOG_E("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

/* type */

/* variable declaration */

static const wf_wifi_ops_t *gwifi_ops_reg = NULL;
struct wpa_supplicant wpa_s_obj;
struct wpa_config wpaconfig;
const struct wpa_driver_ops wpa_driver_bsd_ops =
{
    .name = "bsd",
    .desc = "BSD 802.11 support",
    .find_bssid = wf_get_bssid,
    .get_ssid = wf_get_ssid,
    .set_key = wf_add_key,
};

/* function declaration */

static int
wf_wifi_encryption(nic_info_st *pnic_info, ieee_param *param, wf_u32 param_len)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u32 wep_key_idx, wep_key_len;
    wf_u32 res = 0;

    param->u.crypt.err = 0;
    param->u.crypt.alg[IEEE_CRYPT_ALG_NAME_LEN - 1] = '\0';

    if (param_len !=
        WF_OFFSETOF(ieee_param, u.crypt.key) + param->u.crypt.key_len)
    {
        res = -1;
        goto exit;
    }

    if (is_bcast_addr(param->sta_addr))
    {
        if (param->u.crypt.idx >= WEP_KEYS)
        {
            res = -1;
            goto exit;
        }
    }
    else
    {
        res = -1;
        goto exit;
    }

    if (!os_strcmp((char const *)param->u.crypt.alg, "WEP"))
    {
        wep_key_idx = param->u.crypt.idx;
        wep_key_len = param->u.crypt.key_len;

        if ((wep_key_idx > WEP_KEYS) || (wep_key_len == 0))
        {
            res = -1;
            goto exit;
        }

        psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;

        wep_key_len = wep_key_len <= 5 ? 5 : 13;
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
        os_memcpy(psec_info->dot11DefKey[wep_key_idx].skey,
                  param->u.crypt.key, wep_key_len);
        psec_info->dot11DefKeylen[wep_key_idx] = wep_key_len;
        psec_info->key_mask |= BIT(wep_key_idx);
    }
exit:
    return res;
}

static void wf_wlan_set_ccmp(wf_u8 *pie, size_t *ielen)
{
#define WPA_OUI_TYPE_SIZE                       4
#define WPA_VERSION_SIZE                        2
#define WPA_MULTICAST_CIPHER_SUITE_SIZE         4
#define WPA_UNICAST_CIPHER_SUITE_COUNT_SIZE     2
#define WPA_UNICAST_CIPHER_SUITE_LIST_SIZE      4
#define WPA_AUTHS_NUM_LIST_SIZE                 20

    wf_80211_mgmt_ie_t *wpa_ie;
    wf_u8 wpa_count;
    wf_u8 *pos;
    wf_u8 ccmp_ie[4] = {0x00, 0x50, 0xf2, 0x04};

    wpa_ie = (wf_80211_mgmt_ie_t *)pie;
    pos = wpa_ie->data;
    pos = pos + WPA_OUI_TYPE_SIZE;
    pos = pos + WPA_VERSION_SIZE;
    pos = pos + WPA_MULTICAST_CIPHER_SUITE_SIZE;
    wpa_count = pos[0];
    LOG_D ("wpa_count:%d",wpa_count);

    if (wpa_count == 2)
    {
        pos[0] = 0x01;
        pos = pos + WPA_UNICAST_CIPHER_SUITE_COUNT_SIZE;
        wf_u8 *buf = NULL;
        buf = wf_kzalloc(wpa_ie->len);
        if (buf == NULL)
        {
          return;
        }
        wf_memcpy(buf, wpa_ie->data, wpa_ie->len);
        wf_memcpy(pos, ccmp_ie, WPA_UNICAST_CIPHER_SUITE_LIST_SIZE);
        pos = pos + WPA_UNICAST_CIPHER_SUITE_LIST_SIZE;
        wf_memcpy(pos, buf + WPA_AUTHS_NUM_LIST_SIZE,
                    wpa_ie->len - WPA_AUTHS_NUM_LIST_SIZE);
        wpa_ie->len = wpa_ie->len - WPA_UNICAST_CIPHER_SUITE_LIST_SIZE;
        *ielen = *ielen - WPA_UNICAST_CIPHER_SUITE_LIST_SIZE;

        wf_kfree(buf);

    }
}


static int
wf_wifi_set_wpa_ie (nic_info_st *pnic_info, wf_u8 *pie, size_t ielen)
{
    sec_info_st *sec_info = pnic_info->sec_info;
    wf_u8 *buf = NULL;
    wf_u32 group_cipher = 0, pairwise_cipher = 0;
    wf_u8 eid, wps_oui[4] = { 0x0, 0x50, 0xf2, 0x04};
    wf_u16 cnt = 0;
    wf_u32 res = 0;

    if (pie == NULL)
    {
        goto exit;
    }

    if (ielen > WF_MAX_WPA_IE_LEN)
    {
        res = -1;
        goto exit;
    }

    if (ielen)
    {
        buf = wf_kzalloc(ielen);
        if (buf == NULL)
        {
            res = -1;
            goto exit;
        }
        os_memcpy(buf, pie, ielen);

        if (ielen < RSN_HEADER_LEN)
        {
            res = -1;
            goto exit;
        }

        if (!wf_80211_mgmt_wpa_parse(buf, ielen, &group_cipher,
                                     &pairwise_cipher))
        {
            wf_wlan_set_ccmp(buf, &ielen);
            sec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
            sec_info->ndisauthtype = wf_ndis802_11AuthModeWPAPSK;
            sec_info->wpa_enable = wf_true;
            os_memcpy(sec_info->supplicant_ie, &buf[0], ielen);
        }
        else if (!wf_80211_mgmt_rsn_parse(buf, ielen, &group_cipher,
                                          &pairwise_cipher))
        {
            sec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
            sec_info->ndisauthtype = wf_ndis802_11AuthModeWPA2PSK;
            sec_info->rsn_enable = wf_true;
            os_memcpy(sec_info->supplicant_ie, &buf[0], ielen);
        }

        switch (group_cipher)
        {
            case CIPHER_SUITE_TKIP:
                sec_info->dot118021XGrpPrivacy = _TKIP_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption2Enabled;
                break;
            case CIPHER_SUITE_CCMP:
                sec_info->dot118021XGrpPrivacy = _AES_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
                break;
        }
        switch (pairwise_cipher)
        {
            case CIPHER_SUITE_NONE:
//            sec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
//            sec_info->ndisencryptstatus = wf_ndis802_11EncryptionDisabled;
//            IW_FUNC_DBG("dot11PrivacyAlgrthm=_NO_PRIVACY_");
                break;
            case CIPHER_SUITE_TKIP:
                sec_info->dot11PrivacyAlgrthm = _TKIP_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption2Enabled;
                break;
            case CIPHER_SUITE_CCMP:
                sec_info->dot11PrivacyAlgrthm = _AES_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
                break;
            default:
                sec_info->dot11PrivacyAlgrthm = _AES_;
                sec_info->ndisencryptstatus = wf_ndis802_11Encryption3Enabled;
                break;
        }

        while (cnt < ielen)
        {
            eid = buf[cnt];
            if (eid == WF_80211_MGMT_EID_VENDOR_SPECIFIC &&
                !os_memcmp(&buf[cnt + 2], wps_oui, 4))
            {
                sec_info->wps_ie_len = WF_MIN(buf[cnt + 1] + 2, 512);
                os_memcpy(sec_info->wps_ie, &buf[cnt], sec_info->wps_ie_len);
                cnt += buf[cnt + 1] + 2;
                break;
            }
            else
            {
                cnt += buf[cnt + 1] + 2;
            }
        }
    }

exit :
    if (buf)
    {
        wf_kfree(buf);
    }
    return res;
}

static void
wf_wifi_wpa_config(nic_info_st *pnic_info, const wf_u8 * hwaddr,
                   wf_u32 len, wf_u8 *essid, wf_u8 *password, wf_u32 sec_mode)
{
    struct wpa_supplicant *wpa_s = &wpa_s_obj;
    struct wpa_sm *sm = wpa_s->wpa;
    static int wpa_init = 0;
    struct wpa_ssid *entry;
    if (!wpa_init)
    {
        wpa_init = 1;
        os_memset(wpa_s, 0, sizeof(struct wpa_supplicant));
        wpa_s->wpa = wf_wpa_sm_init(wpa_s);
        wpa_s->wpa->pnic_info = pnic_info;
        if (wpa_s->wpa == NULL)
        {
            return;
        }
        wf_wpa_sm_set_param(wpa_s->wpa, WPA_PARAM_PROTO, WPA_RSN_PROTO);
        wpa_s->driver = &wpa_driver_bsd_ops;

        wpa_s->confname = "sci-wifi";
        wpa_s->conf = wf_wpa_config_read(wpa_s->confname, NULL);
        if (wpa_s->conf == NULL)
        {
            return;
        }
        wf_wpa_supplicant_ctrl_iface_add_network(wpa_s);
    }
    os_memcpy(wpa_s->wpa->own_addr, hwaddr, len);
    entry = wpa_s->conf->ssid;
    entry->key_mgmt = WPA_PSK_KEY_MGMT;
    entry->ssid = (wf_u8 *) essid;
    entry->ssid_len = os_strlen((char const *)essid);
    entry->passphrase = password;
    entry->disabled = 0;
    os_memset(entry->psk, 0, 32);
    wf_pbkdf2_sha1(password, (wf_u8 const *)essid, entry->ssid_len,
                   4096, entry->psk, PMK_LEN);
    entry->psk_set = 1;
    wpa_s->confanother = NULL;
    sm->pmk_len = 32;

    switch (sec_mode)
    {
        case AES_ENABLED:
            entry->pairwise_cipher = 16;
            break;
        case TKIP_ENABLED:
            entry->pairwise_cipher = 8;
            break;
        default:
            break;
    }
}

static void
wf_wifi_set_psk(nic_info_st *pnic_info, wf_wifi_conn_info_t *conn_info,
                ieee_param *param, wf_u32 *ccmp_and_tkip)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    hw_info_st *phw_info = pnic_info->hw_info;

    /* clear CAM */
    wf_mcu_set_dk_cfg(pnic_info, psec_info->dot11AuthAlgrthm, wf_true);
    wf_mcu_set_on_rcr_am(pnic_info, wf_true);
//    wf_mcu_set_hw_invalid_all(pnic_info);

    wf_memset(psec_info, 0, sizeof(sec_info_st));

    os_memset(param->sta_addr, 0xff, WF_ETH_ALEN);
    switch (conn_info->sec & 0xfff)
    {
        case WEP_ENABLED:
            os_memcpy(param->u.crypt.alg, "WEP",
                      IEEE_CRYPT_ALG_NAME_LEN);
            param->u.crypt.set_tx = 1;
            psec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;
            break;
        case AES_ENABLED:
            wf_memcpy(param->u.crypt.alg, "ccmp", IEEE_CRYPT_ALG_NAME_LEN);
            wf_wifi_wpa_config(pnic_info,(const wf_u8 *)phw_info->macAddr,
                               WF_ETH_ALEN, (wf_u8 *)conn_info->ssid.data,
                               (wf_u8 *)conn_info->key.data, AES_ENABLED);
            break;
        case TKIP_ENABLED:
            wf_memcpy(param->u.crypt.alg, "tkip", IEEE_CRYPT_ALG_NAME_LEN);
            wf_wifi_wpa_config(pnic_info, (const wf_u8 *)phw_info->macAddr,
                               WF_ETH_ALEN, (wf_u8 *)conn_info->ssid.data,
                               (wf_u8 *)conn_info->key.data, TKIP_ENABLED);
            break;
        case AES_AND_TKIP_ENABLED:
            OS_LOG_D ("AES_AND_TKIP_ENABLED");
            *ccmp_and_tkip = 1;
            wf_memcpy(param->u.crypt.alg, "ccmp_and_tkip",
                      IEEE_CRYPT_ALG_NAME_LEN);
            wf_wifi_wpa_config(pnic_info,(const wf_u8 *)phw_info->macAddr,
                               WF_ETH_ALEN,(wf_u8 *)conn_info->ssid.data,
                               (wf_u8 *)conn_info->key.data, AES_ENABLED);
            break;
        default:
            wf_memcpy(param->u.crypt.alg, "none",
                      IEEE_CRYPT_ALG_NAME_LEN);
            break;
    }

}

int wf_wifi_eapol_handle(wf_wifi_hd_t wifi_hd, void *buffer, wf_u16 len)
{
    nic_info_st *pnic_info = wifi_hd;
    wpa_s_obj.pnic_info = (void *)pnic_info;
    struct wl_ieee80211_hdr_3addr * pwlanhdr = NULL;
    pwlanhdr = (struct wl_ieee80211_hdr_3addr *)buffer;
    int i=0;

    os_memcpy(wpa_s_obj.wpa->pmk,wpa_s_obj.conf->ssid->psk,32);
    wpa_s_obj.wpa->pmk_len = 32;
    wpa_s_obj.wpa->pairwise_cipher = wpa_s_obj.conf->ssid->pairwise_cipher;
    wpa_s_obj.wpa->key_mgmt = wpa_s_obj.conf->ssid->key_mgmt;
    os_memcpy(wpa_s_obj.wpa->bssid,wf_wlan_get_cur_bssid(pnic_info),6);

    wf_wpa_supplicant_rx_eapol(&wpa_s_obj,wf_wlan_get_cur_bssid(pnic_info),(wf_u8 *)buffer,len);
    return 0;
}


wf_wifi_sec_t wf_wifi_sec_get(wf_wlan_mgmt_scan_que_node_t *pscan_que_node)
{
    wf_80211_mgmt_ie_t *pie;
    wf_u8 pie_len;
    wf_u32 group_cipher = 0, pairwise_cipher = 0;

    if (pscan_que_node->privacy)
    {
        /* parsing WPA/WPA2 */
        pie = (wf_80211_mgmt_ie_t *)pscan_que_node->wpa_ie;
        pie_len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
        if (!wf_80211_mgmt_wpa_parse(pie, pie_len,
                                     &group_cipher, &pairwise_cipher))
        {
            if (pairwise_cipher == (CIPHER_SUITE_TKIP | CIPHER_SUITE_CCMP))
            {
                pairwise_cipher = CIPHER_SUITE_CCMP;
            }
            if ((group_cipher == CIPHER_SUITE_TKIP) &&
                (pairwise_cipher == CIPHER_SUITE_TKIP))
            {
                return WF_WIFI_SEC_WPA_TKIP_PSK;
            }
            else if ((group_cipher == CIPHER_SUITE_CCMP) &&
                     (pairwise_cipher == CIPHER_SUITE_CCMP))
            {
                return WF_WIFI_SEC_WPA_AES_PSK;
            }
            else if ((group_cipher == CIPHER_SUITE_TKIP) &&
                     (pairwise_cipher == CIPHER_SUITE_CCMP))
            {
                return WF_WIFI_SEC_WPA_MIXED_PSK;
            }
        }

        /* parsing rsn */
        pie = (wf_80211_mgmt_ie_t *)pscan_que_node->rsn_ie;
        pie_len = WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len;
        if (!wf_80211_mgmt_rsn_parse(pie, pie_len,
                                     &group_cipher, &pairwise_cipher))
        {
            if (pairwise_cipher == (CIPHER_SUITE_TKIP | CIPHER_SUITE_CCMP))
            {
                pairwise_cipher = CIPHER_SUITE_CCMP;
            }
            if (group_cipher == CIPHER_SUITE_TKIP &&
                pairwise_cipher == CIPHER_SUITE_TKIP)
            {
                return WF_WIFI_SEC_WPA2_TKIP_PSK;
            }
            else if (group_cipher == CIPHER_SUITE_CCMP &&
                     pairwise_cipher == CIPHER_SUITE_CCMP)
            {
                return WF_WIFI_SEC_WPA2_AES_PSK;
            }
            else if (group_cipher == CIPHER_SUITE_TKIP &&
                     pairwise_cipher == CIPHER_SUITE_CCMP)
            {
                return WF_WIFI_SEC_WPA2_MIXED_PSK;
            }
        }

        return WF_WIFI_SEC_WEP_SHARED;
    }
    else
    {
        return WF_WIFI_SEC_OPEN;
    }
}

static int
wf_wifi_updata_conn_info(nic_info_st *pnic_info, wf_wifi_conn_info_t *conn_info)
{
    wf_wlan_mgmt_scan_que_for_rst_e scan_for_rst;
    wf_wlan_mgmt_scan_que_node_t *pscan_node;
    wf_wlan_ssid_t node_ssid;

    wf_memcpy(node_ssid.data, conn_info->ssid.data,
              node_ssid.length = conn_info->ssid.len);
    wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_node)
    {
        if (wf_wlan_is_same_ssid(&pscan_node->ssid, &node_ssid))
        {
            conn_info->channel = pscan_node->channel;
            conn_info->sec = wf_wifi_sec_get(pscan_node);
            os_memcpy(conn_info->bssid, pscan_node->bssid, 6);

            break;
        }
    }
    wf_wlan_mgmt_scan_que_for_end(scan_for_rst);
    if (scan_for_rst != WF_WLAN_MGMT_SCAN_QUE_FOR_RST_BREAK)
    {
        return -1;
    }
    return 0;
}

int nic_priv_init (nic_info_st *pnic_info)
{
    if (!gwifi_ops_reg)
    {
        return -1;
    }

    {
        nic_priv_t *pnic_priv = nic_priv(pnic_info);
        pnic_priv->ops = gwifi_ops_reg;
        pnic_priv->parent = pnic_info;
    }

    return 0;
}

int wf_wifi_init (const wf_wifi_ops_t *ops)
{
    int ret;

    ret = hif_init();
    if (ret)
    {
        return -1;
    }
    gwifi_ops_reg = ops;

    return 0;
}

void wf_wifi_uninit (void)
{
    hif_exit();
    gwifi_ops_reg = NULL;
}


int wf_wifi_enable (wf_wifi_hd_t wifi_hd)
{
    nic_info_st *pnic_info = wifi_hd;

    if (OS_WARN_ON(!pnic_info))
    {
        return -1;
    }

    if (nic_enable(pnic_info) == WF_RETURN_FAIL)
    {
        return -2;
    }

    return 0;
}

int wf_wifi_disable (wf_wifi_hd_t wifi_hd)
{
    nic_info_st *pnic_info = wifi_hd;

    if (OS_WARN_ON(!pnic_info))
    {
        return -1;
    }

    if (nic_disable(pnic_info) == WF_RETURN_FAIL)
    {
        return -2;
    }

    return 0;
}


int wf_wifi_set_mode (wf_wifi_hd_t wifi_hd, wf_wifi_mode_t mode)
{
    nic_info_st *pnic_info = wifi_hd;
    wf_wlan_mgmt_info_t *wlan_mgmt_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *cur_network = &wlan_mgmt_info->cur_network;
    wf_bool is_connected;

    wf_local_cfg_set_work_mode(pnic_info, (sys_work_mode_e)mode);

    wf_mlme_get_connect(pnic_info, &is_connected);
    if (is_connected)
    {
        wf_mlme_deauth(pnic_info, wf_true);
    }

    wf_mcu_set_op_mode(pnic_info, (sys_work_mode_e)mode);

    switch(mode)
    {
        case WF_WIFI_MODE_MASTER :
            cur_network->join_res = -1;
            wf_mlme_abort(pnic_info);
            break;
    }

    return 0;
}


int wf_wifi_scan (wf_wifi_hd_t wifi_hd, wf_wifi_scan_info_t *scan_info)
{
    nic_info_st *pnic_info = wifi_hd;
    wf_bool is_connected;
    wf_wlan_ssid_t ssid;
    int ret;

    wf_mlme_get_connect(pnic_info, &is_connected);
    if (is_connected)
    {
        wf_bool is_busy;
        wf_mlme_get_traffic_busy(pnic_info, &is_busy);
        if (is_busy)
        {
            return -1;
        }
    }

    if(scan_info->ssid_num)
    {
        wf_memcpy(ssid.data, scan_info->ssids[0].data, scan_info->ssids[0].len);
        ssid.length = scan_info->ssids[0].len;
        ret = wf_mlme_scan_start(pnic_info,
                                 SCAN_TYPE_ACTIVE,
                                 &ssid,
                                 1, NULL,
                                 0, WF_MLME_FRAMEWORK_NONE);
    }
    else
    {
        ret = wf_mlme_scan_start(pnic_info,
                                 SCAN_TYPE_ACTIVE,
                                 NULL,
                                 0, NULL,
                                 0, WF_MLME_FRAMEWORK_NONE);
    }
    if (ret)
    {
        return -2;
    }

    return 0;
}

int wf_wifi_scan_stop (wf_wifi_hd_t wifi_hd)
{
    nic_info_st *pnic_info = wifi_hd;

    return wf_mlme_scan_abort(pnic_info);
}


int wf_wifi_conn (wf_wifi_hd_t wifi_hd, wf_wifi_conn_info_t *conn_info)
{
    nic_info_st *pnic_info = wifi_hd;
    sec_info_st *psec_info = pnic_info->sec_info;
    hw_info_st *phw_info = pnic_info->hw_info;
    wf_u8 ccmp_tkip_rsn[4] = {0x00, 0x0f, 0xac, 0x04};
    wf_80211_mgmt_ie_t *wpa_pie = NULL;
    wf_80211_mgmt_ie_t *rsn_pie = NULL;
    ieee_param *param = NULL;
    wf_u32 ccmp_and_tkip = 0;
    wf_u32 param_len = 0;
    wf_u8 len;
    wf_u8 *rsn;

    if (wf_local_cfg_get_work_mode(pnic_info) == WF_MASTER_MODE)
    {
        return -1;
    }

    if(wf_wifi_updata_conn_info(pnic_info, conn_info))
    {
        return -1;
    }

    param_len = WF_OFFSETOF(ieee_param, u.crypt.key) + conn_info->key.len;
    param = (ieee_param *)wf_kzalloc(param_len);
    if (param == NULL)
    {
      return -1;
    }

    wf_wifi_set_psk(pnic_info, conn_info, param, &ccmp_and_tkip);

    param->u.crypt.key_len = conn_info->key.len;

    os_memcpy(param->u.crypt.key, conn_info->key.data, conn_info->key.len);
    param->u.crypt.idx = 0;

    len = conn_info->ssid.len;
    if (len > 32)
    {
        wf_kfree(param);
        return -1;
    }

    if (len == 32)
    {
        wf_kfree(param);
        return 0;
    }

    if (conn_info->ssid.len)
    {
        wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
        wf_wlan_mgmt_scan_que_node_t *pscan_que_node;
        wf_wlan_ssid_t ssid;
        wf_u8 *pbssid = NULL;

        wf_memcpy(ssid.data, conn_info->ssid.data, ssid.length = len);
        ssid.data[ssid.length] = '\0';

        wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
        {
            if (wf_wlan_is_same_ssid(&pscan_que_node->ssid, &ssid))
            {
                OS_LOG_D("FIND SSID");
                pbssid = pscan_que_node->bssid;
                wpa_pie = (wf_80211_mgmt_ie_t *)pscan_que_node->wpa_ie;
                rsn_pie = (wf_80211_mgmt_ie_t *)pscan_que_node->rsn_ie;
                if(rsn_pie->element_id == WF_80211_MGMT_EID_RSN)
                {
                    if((ccmp_and_tkip == 1) && (rsn_pie->len == 24))
                    {
                        wf_memcpy(rsn,rsn_pie->data,rsn_pie->len);
                        wf_memset(rsn_pie->data,0,rsn_pie->len);
                        wf_memcpy(rsn_pie->data,rsn,8);
                        wf_memcpy(rsn_pie->data + 8,ccmp_tkip_rsn,4);
                        wf_memcpy(rsn_pie->data + 12,rsn + 16,rsn_pie->len -16);
                        rsn_pie->len = rsn_pie->len - 4;
                        *(rsn_pie->data + 6) = 1;
                    }

                    wf_wifi_set_wpa_ie(pnic_info, pscan_que_node->rsn_ie,
                                       sizeof(pscan_que_node->rsn_ie));
                }
                else if(wpa_pie->element_id == WF_80211_MGMT_EID_VENDOR_SPECIFIC)
                {
                    wf_wifi_set_wpa_ie(pnic_info, pscan_que_node->wpa_ie,
                                       sizeof(pscan_que_node->wpa_ie));
                }
                break;
            }
        }
        wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);

        wf_wifi_encryption(pnic_info,param,param_len);
        if (scan_que_for_rst == WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL)
        {
            wf_kfree(param);
            return -1;
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
                    wf_mlme_set_connect(pnic_info, wf_false);
                    wf_os_api_ind_connect(pnic_info, WF_MLME_FRAMEWORK_NONE);
                    wf_kfree(param);
                    return 0;
                }
            }
            /* start connection */
            wf_mlme_conn_start(pnic_info, pbssid, &ssid,
                               WF_MLME_FRAMEWORK_NONE, wf_true);
        }
    }
    wf_kfree(param);
    return 0;

}

int wf_wifi_disconn (wf_wifi_hd_t wifi_hd)
{
    nic_info_st *pnic_info = wifi_hd;
    wf_bool conn;

    if (wf_local_cfg_get_work_mode(pnic_info) == WF_INFRA_MODE)
    {
        wf_mlme_get_connect(pnic_info, &conn);

        if(conn == wf_true)
        {
            wf_mlme_conn_abort(pnic_info, wf_true);
            return 0;
        }
    }
    else if (wf_local_cfg_get_work_mode(pnic_info) == WF_MASTER_MODE)
    {
        //wf_ap_deauth_all_sta(pnic_info, 0);
        return 1;
    }

    return -1;
}

static OS_U16 hostapd_own_capab_info(nic_info_st *pnic_info)
{
    int capab = WLAN_CAPABILITY_ESS;
    int privacy;
    int i;

    capab |= WLAN_CAPABILITY_SHORT_PREAMBLE;

    //capab |= WLAN_CAPABILITY_SHORT_SLOT_TIME;

    return capab;
}

static int ieee802_11_build_ap_params(nic_info_st *pnic_info,
                                      char *ssid, int ssid_len, wf_u8 *ie)
{
    hw_info_st *hw_info = pnic_info->hw_info;
    int i=0;
    wf_u8 *tail = NULL;
    int head_len = 0;
    wf_u8 *resp = NULL;
    size_t resp_len = 0;


    wf_u16 capab_info;
    wf_u8 *pos, *tailpos, *csa_pos;
    wf_u8 *head = ie;
    le16 intv = wf_le16_to_cpu(100);
    le16 capainfo;
    head = head + 8;
    head_len += 8;
    wf_memcpy(head,&intv,2);
    head = head + 2;
    head_len += 2;

    capab_info = hostapd_own_capab_info(pnic_info);
    capainfo = wf_le16_to_cpu(capab_info);
    wf_memcpy(head,&capainfo,2);
    head = head + 2;
    head_len += 2;

    *head++ = WLAN_EID_SSID;
    *head++ = ssid_len;
    wf_memcpy(head, ssid, ssid_len);
    head += ssid_len;
    head_len = head_len + ssid_len + 2;

    *head++ = WLAN_EID_SUPP_RATES;
    *head++ = 8;
    for(i = 0; i < 8; i++)
    {
        *head++ = hw_info->datarate[i];
    }
    head_len += (i + 2);

    *head++ = WLAN_EID_DS_PARAMS;
    *head++ = 1;
    *head++ = 6;
    head_len += 3;

    *head++ = WLAN_EID_EXT_SUPP_RATES;
    *head++ = 4;
    for(i = 0; i < 4; i++)
    {
        *head++ = hw_info->datarate[8 + i];
    }
    head_len += 6;

    return head_len;
}

static int wf_wlan_set_beacon(nic_info_st *pnic_info, char *ssid, int ssid_len)
{
#ifdef CFG_ENABLE_AP_MODE
    wf_u8 *ie;
    int ie_len = 0;

#define BEACON_HEAD_BUF_SIZE 256

    ie = os_malloc(BEACON_HEAD_BUF_SIZE);
    if (OS_WARN_ON(!ie))
    {
        OS_BUG();
        return -OS_RERROR;
    }
    ie_len = ieee802_11_build_ap_params(pnic_info, ssid, ssid_len, ie);
    wf_ap_set_beacon(pnic_info, (void *)ie, ie_len, WF_MLME_FRAMEWORK_WEXT);
    os_free(ie);
#endif

    return OS_ROK;
}

int wf_wifi_ap_start (wf_wifi_hd_t wifi_hd, wf_wifi_ap_info_t *ap_info)
{
#ifdef CFG_ENABLE_AP_MODE
    wf_wifi_ap_info_t *info;
    nic_info_st *pnic_info = wifi_hd;
    int ret = 0;

    WIFI_DBG("ssid_val:%s  ssid_len:%d", ap_info->ssid.data, ap_info->ssid.len);
    ret = wf_wlan_set_beacon(pnic_info, (char *)ap_info->ssid.data, ap_info->ssid.len);
    if (ret == -1)
    {
        WIFI_ERROR("set beacon fail!!!");
        return OS_FALSE;
    }
    /* start scan */
    ret = wf_ap_work_start(pnic_info);
    if (ret)
    {
        WIFI_ERROR("wf_ap_work_start fail!!!");
        return OS_FALSE;
    }
#endif

    return 0;
}

int wf_wifi_ap_get_result (wf_wifi_hd_t wifi_hd)
{
#ifdef CFG_ENABLE_AP_MODE
    nic_info_st *pnic_info = wifi_hd;
    wlan_dev_t *wdev = wf_wifi_priv(wifi_hd);
    wdn_net_info_st *pwdn = NULL;
    wlan_dev_info_t *wlan_dev_info = NULL;
    int i = 0;

    for(i=0; i<32; i++)
    {
        pwdn = wf_wdn_find_info_by_id(pnic_info, i);
        if(pwdn != NULL)
        {
            wlan_dev_info = osZMalloc(sizeof(wlan_dev_info_t));
            os_memcpy(wlan_dev_info->bssid, pwdn->mac, 6);
            wlan_dev_info_push(wdev, wlan_dev_info);
        }
    }
    wlan_dev_event_post(wdev, WLAN_DEV_EVT_AP_GET_RESULT);

    return 0;
#endif
}

int wf_wifi_ap_stop (wf_wifi_hd_t wifi_hd)
{
#ifdef CFG_ENABLE_AP_MODE
    nic_info_st *pnic_info = wifi_hd;

    wf_ap_deauth_all_sta(pnic_info, 0);
    wf_ap_status_set(pnic_info, WF_AP_STATUS_UNINITILIZED);
    wf_mcu_disable_ap_mode(pnic_info);
    wf_os_api_ind_disconnect(pnic_info, WF_MLME_FRAMEWORK_WEXT);
#endif

    return 0;
}

int wf_wifi_ap_deauth (wf_wifi_hd_t wifi_hd)
{
    return 0;
}


int wf_wifi_set_powersave (wf_wifi_hd_t wifi_hd, int level)
{
    return 0;
}

int wf_wifi_get_powersave (wf_wifi_hd_t wifi_hd)
{
    return 0;
}


int wf_wifi_set_channel (wf_wifi_hd_t wifi_hd, int channel)
{
    return 0;
}

int wf_wifi_get_channel (wf_wifi_hd_t wifi_hd)
{
    return 0;
}


int wf_wifi_set_country (wf_wifi_hd_t wifi_hd,
                         wf_wifi_country_code_t country_code)
{
    return 0;
}

wf_wifi_country_code_t wf_wifi_get_country (wf_wifi_hd_t wifi_hd)
{
    return WF_WIFI_COUNTRY_CHINA;
}


int wf_wifi_set_mac (wf_wifi_hd_t wifi_hd, wf_u8 mac[6])
{
    return -1;
}

wf_inline
int wf_wifi_get_mac (wf_wifi_hd_t wifi_hd, wf_u8 *mac[6])
{
    nic_info_st *pnic_info = wifi_hd;

    *mac = nic_to_local_addr(pnic_info);

    return 0;
}


int wf_wifi_get_rssi (wf_wifi_hd_t wifi_hd)
{
    return 0;
}


int wf_wifi_data_xmit (wf_wifi_hd_t wifi_hd, void *data, wf_u32 len)
{
    nic_info_st *pnic_info = wifi_hd;

    /* new xmit frame */
    wf_u8 *frame = wf_kzalloc(len);
    if (!frame)
    {
        WIFI_ERROR("malloc out_buf fail");
        return -1;
    }
    wf_memcpy(frame, data, len);

    /* send frame */
    if (wf_tx_msdu(pnic_info, frame, len, frame))
    {
        wf_free(frame);
        return -2;
    }
    else
    {
        tx_work_wake(pnic_info);
    }

    return 0;
}


