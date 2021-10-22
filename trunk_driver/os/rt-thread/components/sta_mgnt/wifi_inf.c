
#include "utils/os.h"
#include "common.h"
#include "defs.h"
#include "wifi_inf.h"
#include "wpa_common.h"
#include "cfg80211.h"
#include "wpa.h"
#include "sha1.h"

extern struct wpa_supplicant wpa_s_obj;
extern struct wpa_config wpaconfig;
int wf_rt_get_bssid(void *pnic_info,wf_u8 *bssid)
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


int wf_rt_add_key(void *pnic_info,int key_idx, int pairwise, const wf_u8 * addr, wf_u32 alg,
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
  
  wf_rt_key(pnic_info,key_idx,addr,&key_para);
  return 0;
}


int wf_rt_get_ssid(void *pnic_info,wf_u8 *ssid)
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
  config->ap_scan = 1;
  config->user_mpm = DEFAULT_USER_MPM;
  config->max_peer_links = DEFAULT_MAX_PEER_LINKS;
  config->mesh_max_inactivity = DEFAULT_MESH_MAX_INACTIVITY;
  config->dot11RSNASAERetransPeriod = DEFAULT_DOT11_RSNA_SAE_RETRANS_PERIOD;
  config->fast_reauth = DEFAULT_FAST_REAUTH;
  config->bss_max_count = DEFAULT_BSS_MAX_COUNT;
  config->bss_expiration_age = DEFAULT_BSS_EXPIRATION_AGE;
  config->bss_expiration_scan_count = DEFAULT_BSS_EXPIRATION_SCAN_COUNT;
  config->max_num_sta = DEFAULT_MAX_NUM_STA;
  config->access_network_type = DEFAULT_ACCESS_NETWORK_TYPE;
  config->scan_cur_freq = DEFAULT_SCAN_CUR_FREQ;
  config->p2p_search_delay = DEFAULT_P2P_SEARCH_DELAY;
  config->rand_addr_lifetime = DEFAULT_RAND_ADDR_LIFETIME;
  config->key_mgmt_offload = DEFAULT_KEY_MGMT_OFFLOAD;
  config->cert_in_cb = DEFAULT_CERT_IN_CB;
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


