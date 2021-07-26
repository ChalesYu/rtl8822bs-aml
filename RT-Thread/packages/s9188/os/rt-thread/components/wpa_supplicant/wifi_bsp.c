



#define THREAD_PRORTIY		6			//优先级
#define	THREAD_STACK_SIZE	512		//栈大小
#define THREAD_TIMESLICE	5			//线程时间片
#include "utils/os.h"
#include "common.h"
#include "defs.h"
#include "wifi_bsp.h"
#include "ieee802_11_defs.h"
#include "cfg80211.h"
#include "wpa.h"
#include "sha1.h"

static struct rt_messagequeue wifi_msg_queue = NULL;
static u8	msg_pool[1024];

extern struct wpa_supplicant wpa_s_obj;
extern struct wpa_config wpaconfig;
int wf_rt_get_bssid(void *pnic_info,u8 *bssid)
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


u32 wpaalg_to_alg(u32 alg)
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


int wf_rt_add_key(void *pnic_info,int key_idx, int pairwise, const u8 * addr, u32 alg,
				   const u8 * seq, size_t seq_len,
				   const u8 * key, size_t key_len)
{
	nic_info_st *nic_info = pnic_info;
	struct key_params key_para;

	key_para.seq = (u8 *) seq;
	key_para.seq_len = seq_len;
	key_para.key = (u8 *) key;
	key_para.key_len = key_len;
	key_para.cipher = wpaalg_to_alg(alg);

	wf_rt_key(pnic_info,key_idx,addr,&key_para);
	return 0;
}


int wf_rt_get_ssid(void *pnic_info,u8 *ssid)
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


struct wpa_config *wpa_config_alloc_empty(const char *ctrl_interface,
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
#ifdef NOT_FIXED
	if (ctrl_interface)
		config->ctrl_interface = os_strdup(ctrl_interface);
	if (driver_param)
		config->driver_param = os_strdup(driver_param);
#endif
	return config;
}


struct wpa_config *wpa_config_read(const char *name, struct wpa_config *cfgp)
{
	struct wpa_config *config;

	if (name == NULL)
		return NULL;
	if (cfgp)
		config = cfgp;
	else
		config = wpa_config_alloc_empty(NULL, NULL);
	if (config == NULL)
		return NULL;

	return config;
}


void PRF( u8 * key,
		  int key_len,
		  u8 * prefix,
		  int prefix_len,
		  u8 * data,  int data_len,  u8 * output,  int len)
{
	int i;
	u8 input[1024];
	int currentindex = 0;
	int total_len;

	os_memcpy(input, prefix, prefix_len);
	input[prefix_len] = 0;
	os_memcpy(&input[prefix_len + 1], data, data_len);
	total_len = prefix_len + 1 + data_len;
	input[total_len] = 0;
	total_len++;
	for (i = 0; i < (len + 19) / 20; i++) {
		hmac_sha1(input, total_len, key, key_len, &output[currentindex]);
		currentindex += 20;
		input[total_len - 1]++;
	}
}


void WpaCountGTK(u8 * GMK,
				  u8 * GNonce,
				  u8 * AA,  u8 * output,  u32 len)
{
	u8 concatenation[38];
	u32 CurrPos = 0;
	u8 Prefix[] = { 'G', 'r', 'o', 'u', 'p', ' ', 'k', 'e', 'y', ' ',
		'e', 'x', 'p', 'a', 'n', 's', 'i', 'o', 'n'
	};
	os_memcpy(concatenation, AA, 6);
	CurrPos += 6;
	os_memcpy(&concatenation[CurrPos], GNonce, 32);
	CurrPos += 32;
	PRF(GMK, 32, Prefix, 19, concatenation, 38, output, len);
}

