#include "common.h"

#define WIFI_BSP_STARTSTA  0
#define WIFI_BSP_STOPSTA  1
#define WIFI_BSP_STARTAP  2
#define WIFI_BSP_STOPAP  3
#define WIFI_BSP_STARTPROMISC  4
#define WIFI_BSP_STOPPROMISC  5
#define WIFI_BSP_SCAN  6
#define WIFI_BSP_GOODCHANEL 7
#define WIFI_BSP_SMARTLINK_ENTER 8
#define WIFI_BSP_SMARTLINK_EXIT 9
#define WIFI_BSP_SLINK_ENTER 10
#define WIFI_BSP_SLINK_EXIT  11
#define WIFI_BSP_REGEVENT	12
#define WIFI_BSP_WLAN_STATUS	13
#define WIFI_BSP_STA_LIST	14

#define WPA_WPA_PROTO BIT(0)
#define WPA_RSN_PROTO BIT(1)
#define WPA_PROTO_WAPI BIT(2)
#define WPA_PROTO_OSEN BIT(3)

#define WPA_IEEE8021X_KEY_MGMT BIT(0)
#define WPA_PSK_KEY_MGMT BIT(1)
#define WPA_NONE_KEY_MGMT BIT(2)
#define WPA_IEEE8021X_NO_WPA_KEY_MGMT BIT(3)
#define WPA_WPA_NONE_KEY_MGMT BIT(4)
#define WPA_KEY_MGMT_FT_IEEE8021X BIT(5)
#define WPA_KEY_MGMT_FT_PSK BIT(6)
#define WPA_KEY_MGMT_IEEE8021X_SHA256 BIT(7)
#define WPA_KEY_MGMT_PSK_SHA256 BIT(8)
#define WPA_KEY_MGMT_WPS BIT(9)
#define WPA_KEY_MGMT_SAE BIT(10)
#define WPA_KEY_MGMT_FT_SAE BIT(11)
#define WPA_KEY_MGMT_WAPI_PSK BIT(12)
#define WPA_KEY_MGMT_WAPI_CERT BIT(13)
#define WPA_KEY_MGMT_CCKM BIT(14)
#define WPA_KEY_MGMT_OSEN BIT(15)
#define WPA_KEY_MGMT_IEEE8021X_SUITE_B BIT(16)
#define WPA_KEY_MGMT_IEEE8021X_SUITE_B_192 BIT(17)


#define IW_AUTH_CIPHER_NONE		0x00000001
#define IW_AUTH_CIPHER_WEP40	0x00000002
#define IW_AUTH_CIPHER_TKIP		0x00000004
#define IW_AUTH_CIPHER_CCMP		0x00000008
#define IW_AUTH_CIPHER_WEP104	0x00000010
#define IW_AUTH_CIPHER_AES_CMAC	0x00000020
int wf_rt_get_bssid(void *pnic_info,wf_u8 *bssid);
int wf_rt_get_ssid(void *pnic_info,wf_u8 *ssid);
int wf_rt_add_key(void *pnic_info,int key_idx, int pairwise, const wf_u8 * addr, wf_u32 alg,
                  const wf_u8 * seq, size_t seq_len,
                  const wf_u8 * key, size_t key_len);
struct wpa_config *wf_wpa_config_read(const char *name, struct wpa_config *cfgp);
void WpaCountGTK(wf_u8 * GMK,wf_u8 * GNonce,wf_u8 * AA,  wf_u8 * output,  wf_u32 len);

