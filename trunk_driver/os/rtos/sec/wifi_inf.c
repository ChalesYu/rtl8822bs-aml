#include "common.h"

#include "utils/os.h"
#include "common/defs.h"
#include "common/wpa_common.h"
#include "crypto/sha1.h"
#include "wifi_inf.h"
#include "cfg80211.h"
#include "sec/wpa.h"

extern struct wpa_supplicant wpa_s_obj;
extern struct wpa_config wpaconfig;
int wf_get_bssid(void *pnic_info,wf_u8 *bssid)
{
    nic_info_st *nic_info = pnic_info;
    wf_wlan_mgmt_info_t *pwlan_info = nic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    if(pcur_network->bssid == NULL)
    {
        return -1;
    }
    wf_memcpy(bssid, pcur_network->bssid, WF_ETH_ALEN);
    return 0;
}


wf_u32 wf_wpa_alg_to_alg(wf_u32 alg)
{
    enum wpa_alg temp = (enum wpa_alg)alg;

    if (temp == WPA_ALG_NONE)
        return IW_AUTH_CIPHER_NONE;
    else if (temp == WPA_ALG_WEP)
        return WLAN_CIPHER_SUITE_WEP40;
    else if (temp == WPA_ALG_TKIP)
        return WLAN_CIPHER_SUITE_TKIP;
    else if (temp == WPA_ALG_CCMP)
        return WLAN_CIPHER_SUITE_CCMP;

    return 0;
}


int wf_add_key(void *pnic_info,int key_idx, int pairwise, const wf_u8 * addr, wf_u32 alg,
               const wf_u8 * seq, size_t seq_len,
               const wf_u8 * key, size_t key_len)
{
    nic_info_st *nic_info = pnic_info;
    struct key_params key_para;

    key_para.seq = (wf_u8 *) seq;
    key_para.seq_len = seq_len;
    key_para.key = (wf_u8 *) key;
    key_para.key_len = key_len;
    key_para.cipher = wf_wpa_alg_to_alg(alg);

    wf_key(pnic_info,key_idx,addr,&key_para);
    return 0;
}


int wf_get_ssid(void *pnic_info,wf_u8 *ssid)
{
    nic_info_st *nic_info = pnic_info;
    wf_wlan_mgmt_info_t *pwlan_info = nic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    if(pcur_network->ssid.data == NULL)
    {
        return -1;
    }
    wf_memcpy(ssid, pcur_network->ssid.data, WF_ETH_ALEN);
    return 0;
}


struct wpa_config *wf_wpa_config_alloc_empty(const char *ctrl_interface,
        const char *driver_param)
{
    struct wpa_config *config;

    config = &wpaconfig;
    if (config == NULL)
        return NULL;
    config->eapol_version = EAPOL_VERSION;
    return config;
}


struct wpa_config *wf_wpa_config_read(const char *name, struct wpa_config *cfgp)
{
    struct wpa_config *config;

    if (name == NULL)
        return NULL;
    if (cfgp)
        config = cfgp;
    else
        config = wf_wpa_config_alloc_empty(NULL, NULL);
    if (config == NULL)
        return NULL;

    return config;
}


