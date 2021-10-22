/*
* WPA definitions shared between hostapd and wpa_supplicant
* Copyright (c) 2002-2015, Jouni Malinen <j@w1.fi>
*
* This software may be distributed under the terms of the BSD license.
* See README for more details.
*/

#ifndef WPA_COMMON_H
#define WPA_COMMON_H
#include "utils/common.h"

#define WLAN_TIMEOUT_REASSOC_DEADLINE 1
#define WLAN_TIMEOUT_KEY_LIFETIME 2
#define WLAN_TIMEOUT_ASSOC_COMEBACK 3

#define WLAN_EID_SSID 0
#define WLAN_EID_SUPP_RATES 1
#define WLAN_EID_FH_PARAMS 2
#define WLAN_EID_DS_PARAMS 3
#define WLAN_EID_CF_PARAMS 4
#define WLAN_EID_TIM 5
#define WLAN_EID_IBSS_PARAMS 6
#define WLAN_EID_COUNTRY 7
#define WLAN_EID_BSS_LOAD 11
#define WLAN_EID_CHALLENGE 16

#define WLAN_EID_PWR_CONSTRAINT 32
#define WLAN_EID_PWR_CAPABILITY 33
#define WLAN_EID_TPC_REQUEST 34
#define WLAN_EID_TPC_REPORT 35
#define WLAN_EID_SUPPORTED_CHANNELS 36
#define WLAN_EID_CHANNEL_SWITCH 37
#define WLAN_EID_MEASURE_REQUEST 38
#define WLAN_EID_MEASURE_REPORT 39
#define WLAN_EID_QUITE 40
#define WLAN_EID_IBSS_DFS 41

#define WLAN_EID_ERP_INFO 42
#define WLAN_EID_HT_CAP 45
#define WLAN_EID_QOS 46
#define WLAN_EID_RSN 48
#define WLAN_EID_EXT_SUPP_RATES 50
#define WLAN_EID_NEIGHBOR_REPORT 52
#define WLAN_EID_MOBILITY_DOMAIN 54
#define WLAN_EID_FAST_BSS_TRANSITION 55
#define WLAN_EID_TIMEOUT_INTERVAL 56
#define WLAN_EID_RIC_DATA 57
#define WLAN_EID_SUPPORTED_OPERATING_CLASSES 59
#define WLAN_EID_EXT_CHANSWITCH_ANN 60
#define WLAN_EID_HT_OPERATION 61
#define WLAN_EID_SECONDARY_CHANNEL_OFFSET 62
#define WLAN_EID_WAPI 68
#define WLAN_EID_TIME_ADVERTISEMENT 69
#define WLAN_EID_RRM_ENABLED_CAPABILITIES 70
#define WLAN_EID_20_40_BSS_COEXISTENCE 72
#define WLAN_EID_20_40_BSS_INTOLERANT 73
#define WLAN_EID_OVERLAPPING_BSS_SCAN_PARAMS 74
#define WLAN_EID_MMIE 76
#define WLAN_EID_SSID_LIST 84
#define WLAN_EID_BSS_MAX_IDLE_PERIOD 90
#define WLAN_EID_TFS_REQ 91
#define WLAN_EID_TFS_RESP 92
#define WLAN_EID_WNMSLEEP 93
#define WLAN_EID_TIME_ZONE 98
#define WLAN_EID_LINK_ID 101
#define WLAN_EID_INTERWORKING 107
#define WLAN_EID_ADV_PROTO 108
#define WLAN_EID_QOS_MAP_SET 110
#define WLAN_EID_ROAMING_CONSORTIUM 111
#define WLAN_EID_MESH_CONFIG 113
#define WLAN_EID_MESH_ID 114
#define WLAN_EID_PEER_MGMT 117
#define WLAN_EID_EXT_CAPAB 127
#define WLAN_EID_AMPE 139
#define WLAN_EID_MIC 140
#define WLAN_EID_CCKM 156
#define WLAN_EID_MULTI_BAND 158
#define WLAN_EID_SESSION_TRANSITION 164
#define WLAN_EID_VHT_CAP 191
#define WLAN_EID_VHT_OPERATION 192
#define WLAN_EID_VHT_EXTENDED_BSS_LOAD 193
#define WLAN_EID_VHT_WIDE_BW_CHSWITCH  194
#define WLAN_EID_VHT_TRANSMIT_POWER_ENVELOPE 195
#define WLAN_EID_VHT_CHANNEL_SWITCH_WRAPPER 196
#define WLAN_EID_VHT_AID 197
#define WLAN_EID_VHT_QUIET_CHANNEL 198
#define WLAN_EID_VHT_OPERATING_MODE_NOTIFICATION 199
#define WLAN_EID_VENDOR_SPECIFIC 221

#define WLAN_AUTH_CHALLENGE_LEN 128

#define OUI_MICROSOFT 0x0050f2

#define WPA_IE_VENDOR_TYPE 0x0050f201
#define WMM_IE_VENDOR_TYPE 0x0050f202
#define WPS_IE_VENDOR_TYPE 0x0050f204
#define OUI_WFA 0x506f9a
#define P2P_IE_VENDOR_TYPE 0x506f9a09
#define WFD_IE_VENDOR_TYPE 0x506f9a0a
#define WFD_OUI_TYPE 10
#define HS20_IE_VENDOR_TYPE 0x506f9a10
#define OSEN_IE_VENDOR_TYPE 0x506f9a12
#define MBO_IE_VENDOR_TYPE 0x506f9a16
#define MBO_OUI_TYPE 22

#define WLAN_AKM_SUITE_8021X		0x000FAC01
#define WLAN_AKM_SUITE_PSK		0x000FAC02
#define WLAN_AKM_SUITE_FT_8021X		0x000FAC03
#define WLAN_AKM_SUITE_FT_PSK		0x000FAC04
#define WLAN_AKM_SUITE_8021X_SHA256	0x000FAC05
#define WLAN_AKM_SUITE_PSK_SHA256	0x000FAC06
#define WLAN_AKM_SUITE_8021X_SUITE_B	0x000FAC11
#define WLAN_AKM_SUITE_8021X_SUITE_B_192	0x000FAC12
#define WLAN_AKM_SUITE_CCKM		0x00409600
#define WLAN_AKM_SUITE_OSEN		0x506f9a01

#define WLAN_CIPHER_SUITE_USE_GROUP	0x000FAC00
#define WLAN_CIPHER_SUITE_WEP40		0x000FAC01
#define WLAN_CIPHER_SUITE_TKIP		0x000FAC02

#define WLAN_CIPHER_SUITE_CCMP		0x000FAC04
#define WLAN_CIPHER_SUITE_WEP104	0x000FAC05
#define WLAN_CIPHER_SUITE_AES_CMAC	0x000FAC06
#define WLAN_CIPHER_SUITE_NO_GROUP_ADDR	0x000FAC07
#define WLAN_CIPHER_SUITE_GCMP		0x000FAC08
#define WLAN_CIPHER_SUITE_GCMP_256	0x000FAC09
#define WLAN_CIPHER_SUITE_CCMP_256	0x000FAC0A
#define WLAN_CIPHER_SUITE_BIP_GMAC_128	0x000FAC0B
#define WLAN_CIPHER_SUITE_BIP_GMAC_256	0x000FAC0C
#define WLAN_CIPHER_SUITE_BIP_CMAC_256	0x000FAC0D

#define WLAN_CIPHER_SUITE_SMS4		0x00147201

#define WLAN_CIPHER_SUITE_CKIP		0x00409600
#define WLAN_CIPHER_SUITE_CKIP_CMIC	0x00409601
#define WLAN_CIPHER_SUITE_CMIC		0x00409602
#define WLAN_CIPHER_SUITE_KRK		0x004096FF

#define WLAN_CAPABILITY_ESS BIT(0)
#define WLAN_CAPABILITY_IBSS BIT(1)
#define WLAN_CAPABILITY_CF_POLLABLE BIT(2)
#define WLAN_CAPABILITY_CF_POLL_REQUEST BIT(3)
#define WLAN_CAPABILITY_PRIVACY BIT(4)
#define WLAN_CAPABILITY_SHORT_PREAMBLE BIT(5)
#define WLAN_CAPABILITY_PBCC BIT(6)
#define WLAN_CAPABILITY_CHANNEL_AGILITY BIT(7)
#define WLAN_CAPABILITY_SPECTRUM_MGMT BIT(8)
#define WLAN_CAPABILITY_QOS BIT(9)
#define WLAN_CAPABILITY_SHORT_SLOT_TIME BIT(10)
#define WLAN_CAPABILITY_APSD BIT(11)
#define WLAN_CAPABILITY_RADIO_MEASUREMENT BIT(12)
#define WLAN_CAPABILITY_DSSS_OFDM BIT(13)
#define WLAN_CAPABILITY_DELAYED_BLOCK_ACK BIT(14)
#define WLAN_CAPABILITY_IMM_BLOCK_ACK BIT(15)

#define PMKID_LEN 16
#define PMK_LEN 32
#define PMK_LEN_SUITE_B_192 48
#define PMK_LEN_MAX 48
#define WPA_REPLAY_COUNTER_LEN 8
#define WPA_NONCE_LEN 32
#define WPA_KEY_RSC_LEN 8
#define WPA_GMK_LEN 32
#define WPA_GTK_MAX_LEN 32
#define SHA384_MAC_LEN 48

#define WPA_ALLOWED_PAIRWISE_CIPHERS \
(WPA_CIPHER_CCMP | WPA_CIPHER_GCMP | WPA_CIPHER_TKIP | WPA_CIPHER_NONE | \
  WPA_CIPHER_GCMP_256 | WPA_CIPHER_CCMP_256)
#define WPA_ALLOWED_GROUP_CIPHERS \
(WPA_CIPHER_CCMP | WPA_CIPHER_GCMP | WPA_CIPHER_TKIP | \
  WPA_CIPHER_GCMP_256 | WPA_CIPHER_CCMP_256 | \
    WPA_CIPHER_GTK_NOT_USED)

#define WPA_SELECTOR_LEN 4
#define WPA_VERSION 1
#define RSN_SELECTOR_LEN 4
#define RSN_VERSION 1

#define RSN_SELECTOR(a, b, c, d) \
((((wf_u32) (a)) << 24) | (((wf_u32) (b)) << 16) | (((wf_u32) (c)) << 8) | \
  (wf_u32) (d))

#define WPA_AUTH_KEY_MGMT_NONE RSN_SELECTOR(0x00, 0x50, 0xf2, 0)
#define WPA_AUTH_KEY_MGMT_UNSPEC_802_1X RSN_SELECTOR(0x00, 0x50, 0xf2, 1)
#define WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X RSN_SELECTOR(0x00, 0x50, 0xf2, 2)
#define WPA_AUTH_KEY_MGMT_CCKM RSN_SELECTOR(0x00, 0x40, 0x96, 0)
#define WPA_CIPHER_SUITE_NONE RSN_SELECTOR(0x00, 0x50, 0xf2, 0)
#define WPA_CIPHER_SUITE_TKIP RSN_SELECTOR(0x00, 0x50, 0xf2, 2)
#define WPA_CIPHER_SUITE_CCMP RSN_SELECTOR(0x00, 0x50, 0xf2, 4)


#define RSN_AUTH_KEY_MGMT_UNSPEC_802_1X RSN_SELECTOR(0x00, 0x0f, 0xac, 1)
#define RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X RSN_SELECTOR(0x00, 0x0f, 0xac, 2)
#ifdef CONFIG_IEEE80211R
#define RSN_AUTH_KEY_MGMT_FT_802_1X RSN_SELECTOR(0x00, 0x0f, 0xac, 3)
#define RSN_AUTH_KEY_MGMT_FT_PSK RSN_SELECTOR(0x00, 0x0f, 0xac, 4)
#endif
#define RSN_AUTH_KEY_MGMT_802_1X_SHA256 RSN_SELECTOR(0x00, 0x0f, 0xac, 5)
#define RSN_AUTH_KEY_MGMT_PSK_SHA256 RSN_SELECTOR(0x00, 0x0f, 0xac, 6)
#define RSN_AUTH_KEY_MGMT_TPK_HANDSHAKE RSN_SELECTOR(0x00, 0x0f, 0xac, 7)
#define RSN_AUTH_KEY_MGMT_SAE RSN_SELECTOR(0x00, 0x0f, 0xac, 8)
#define RSN_AUTH_KEY_MGMT_FT_SAE RSN_SELECTOR(0x00, 0x0f, 0xac, 9)
#define RSN_AUTH_KEY_MGMT_802_1X_SUITE_B RSN_SELECTOR(0x00, 0x0f, 0xac, 11)
#define RSN_AUTH_KEY_MGMT_802_1X_SUITE_B_192 RSN_SELECTOR(0x00, 0x0f, 0xac, 12)
#define RSN_AUTH_KEY_MGMT_FT_802_1X_SUITE_B_192 \
RSN_SELECTOR(0x00, 0x0f, 0xac, 13)
#define RSN_AUTH_KEY_MGMT_CCKM RSN_SELECTOR(0x00, 0x40, 0x96, 0x00)
#define RSN_AUTH_KEY_MGMT_OSEN RSN_SELECTOR(0x50, 0x6f, 0x9a, 0x01)

#define RSN_CIPHER_SUITE_NONE RSN_SELECTOR(0x00, 0x0f, 0xac, 0)
#define RSN_CIPHER_SUITE_TKIP RSN_SELECTOR(0x00, 0x0f, 0xac, 2)
#define RSN_CIPHER_SUITE_CCMP RSN_SELECTOR(0x00, 0x0f, 0xac, 4)
#define RSN_CIPHER_SUITE_AES_128_CMAC RSN_SELECTOR(0x00, 0x0f, 0xac, 6)
#define RSN_CIPHER_SUITE_NO_GROUP_ADDRESSED RSN_SELECTOR(0x00, 0x0f, 0xac, 7)
#define RSN_CIPHER_SUITE_GCMP RSN_SELECTOR(0x00, 0x0f, 0xac, 8)
#define RSN_CIPHER_SUITE_GCMP_256 RSN_SELECTOR(0x00, 0x0f, 0xac, 9)
#define RSN_CIPHER_SUITE_CCMP_256 RSN_SELECTOR(0x00, 0x0f, 0xac, 10)
#define RSN_CIPHER_SUITE_BIP_GMAC_128 RSN_SELECTOR(0x00, 0x0f, 0xac, 11)
#define RSN_CIPHER_SUITE_BIP_GMAC_256 RSN_SELECTOR(0x00, 0x0f, 0xac, 12)
#define RSN_CIPHER_SUITE_BIP_CMAC_256 RSN_SELECTOR(0x00, 0x0f, 0xac, 13)

#define RSN_KEY_DATA_GROUPKEY RSN_SELECTOR(0x00, 0x0f, 0xac, 1)
#define RSN_KEY_DATA_MAC_ADDR RSN_SELECTOR(0x00, 0x0f, 0xac, 3)
#define RSN_KEY_DATA_PMKID RSN_SELECTOR(0x00, 0x0f, 0xac, 4)

#define RSN_KEY_DATA_SMK RSN_SELECTOR(0x00, 0x0f, 0xac, 5)
#define RSN_KEY_DATA_NONCE RSN_SELECTOR(0x00, 0x0f, 0xac, 6)
#define RSN_KEY_DATA_LIFETIME RSN_SELECTOR(0x00, 0x0f, 0xac, 7)
#define RSN_KEY_DATA_ERROR RSN_SELECTOR(0x00, 0x0f, 0xac, 8)

#define RSN_KEY_DATA_KEYID RSN_SELECTOR(0x00, 0x0f, 0xac, 10)
#define RSN_KEY_DATA_MULTIBAND_GTK RSN_SELECTOR(0x00, 0x0f, 0xac, 11)
#define RSN_KEY_DATA_MULTIBAND_KEYID RSN_SELECTOR(0x00, 0x0f, 0xac, 12)

#define WFA_KEY_DATA_IP_ADDR_REQ RSN_SELECTOR(0x50, 0x6f, 0x9a, 4)
#define WFA_KEY_DATA_IP_ADDR_ALLOC RSN_SELECTOR(0x50, 0x6f, 0x9a, 5)

#define WPA_OUI_TYPE RSN_SELECTOR(0x00, 0x50, 0xf2, 1)

#define RSN_SELECTOR_PUT(a, val) WPA_PUT_BE32((wf_u8 *) (a), (val))
#define RSN_SELECTOR_GET(a) WPA_GET_BE32((const wf_u8 *) (a))

#define RSN_NUM_REPLAY_COUNTERS_1 0
#define RSN_NUM_REPLAY_COUNTERS_2 1
#define RSN_NUM_REPLAY_COUNTERS_4 2
#define RSN_NUM_REPLAY_COUNTERS_16 3


#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

#define WPA_CAPABILITY_PREAUTH BIT(0)
#define WPA_CAPABILITY_NO_PAIRWISE BIT(1)

#define WPA_CAPABILITY_MFPR BIT(6)
#define WPA_CAPABILITY_MFPC BIT(7)

#define WPA_CAPABILITY_PEERKEY_ENABLED BIT(9)
#define WPA_CAPABILITY_SPP_A_MSDU_CAPABLE BIT(10)
#define WPA_CAPABILITY_SPP_A_MSDU_REQUIRED BIT(11)
#define WPA_CAPABILITY_PBAC BIT(12)
#define WPA_CAPABILITY_EXT_KEY_ID_FOR_UNICAST BIT(13)

#define MOBILITY_DOMAIN_ID_LEN 2
#define FT_R0KH_ID_MAX_LEN 48
#define FT_R1KH_ID_LEN 6
#define WPA_PMK_NAME_LEN 16

#define WPA_KEY_INFO_TYPE_MASK ((wf_u16) (BIT(0) | BIT(1) | BIT(2)))
#define WPA_KEY_INFO_TYPE_AKM_DEFINED 0
#define WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 BIT(0)
#define WPA_KEY_INFO_TYPE_HMAC_SHA1_AES BIT(1)
#define WPA_KEY_INFO_TYPE_AES_128_CMAC 3
#define WPA_KEY_INFO_KEY_TYPE BIT(3)

#define WPA_KEY_INFO_KEY_INDEX_MASK (BIT(4) | BIT(5))
#define WPA_KEY_INFO_KEY_INDEX_SHIFT 4
#define WPA_KEY_INFO_INSTALL BIT(6)
#define WPA_KEY_INFO_TXRX BIT(6)
#define WPA_KEY_INFO_ACK BIT(7)
#define WPA_KEY_INFO_MIC BIT(8)
#define WPA_KEY_INFO_SECURE BIT(9)
#define WPA_KEY_INFO_ERROR BIT(10)
#define WPA_KEY_INFO_REQUEST BIT(11)
#define WPA_KEY_INFO_ENCR_KEY_DATA BIT(12)
#define WPA_KEY_INFO_SMK_MESSAGE BIT(13)


struct wpa_eapol_key {
  wf_u8 type;
  wf_u8 key_info[2];
  wf_u8 key_length[2];
  wf_u8 replay_counter[WPA_REPLAY_COUNTER_LEN];
  wf_u8 key_nonce[WPA_NONCE_LEN];
  wf_u8 key_iv[16];
  wf_u8 key_rsc[WPA_KEY_RSC_LEN];
  wf_u8 key_id[8];
  wf_u8 key_mic[16];
  wf_u8 key_data_length[2];
} STRUCT_PACKED;

struct wpa_eapol_key_192 {
  wf_u8 type;
  wf_u8 key_info[2];
  wf_u8 key_length[2];
  wf_u8 replay_counter[WPA_REPLAY_COUNTER_LEN];
  wf_u8 key_nonce[WPA_NONCE_LEN];
  wf_u8 key_iv[16];
  wf_u8 key_rsc[WPA_KEY_RSC_LEN];
  wf_u8 key_id[8];
  wf_u8 key_mic[24];
  wf_u8 key_data_length[2];
} STRUCT_PACKED;

#define WPA_EAPOL_KEY_MIC_MAX_LEN 24
#define WPA_KCK_MAX_LEN 24
#define WPA_KEK_MAX_LEN 32
#define WPA_TK_MAX_LEN 32

struct wpa_ptk {
  wf_u8 kck[WPA_KCK_MAX_LEN];
  wf_u8 kek[WPA_KEK_MAX_LEN];
  wf_u8 tk[WPA_TK_MAX_LEN];
  size_t kck_len;
  size_t kek_len;
  size_t tk_len;
};

struct wpa_ie_hdr {
  wf_u8 elem_id;
  wf_u8 len;
  wf_u8 oui[4];
  wf_u8 version[2];
} STRUCT_PACKED;

struct rsn_ie_hdr {
  wf_u8 elem_id;
  wf_u8 len;
  wf_u8 version[2];
} STRUCT_PACKED;


enum {
  STK_MUI_4WAY_STA_AP = 1,
  STK_MUI_4WAY_STAT_STA = 2,
  STK_MUI_GTK = 3,
  STK_MUI_SMK = 4
};

enum {
  STK_ERR_STA_NR = 1,
  STK_ERR_STA_NRSN = 2,
  STK_ERR_CPHR_NS = 3,
  STK_ERR_NO_STSL = 4
};

struct ieee80211_ht_capabilities {
  le16 ht_capabilities_info;
  wf_u8 a_mpdu_params;
  wf_u8 supported_mcs_set[16];
  le16 ht_extended_capabilities;
  le32 tx_bf_capability_info;
  wf_u8 asel_capabilities;
} STRUCT_PACKED;

struct ieee80211_vht_capabilities {
  le32 vht_capabilities_info;
  struct {
    le16 rx_map;
    le16 rx_highest;
    le16 tx_map;
    le16 tx_highest;
  } vht_supported_mcs_set;
} STRUCT_PACKED;


struct rsn_error_kde {
  be16 mui;
  be16 error_type;
} STRUCT_PACKED;


struct rsn_mdie {
  wf_u8 mobility_domain[MOBILITY_DOMAIN_ID_LEN];
  wf_u8 ft_capab;
} STRUCT_PACKED;

#define RSN_FT_CAPAB_FT_OVER_DS BIT(0)
#define RSN_FT_CAPAB_FT_RESOURCE_REQ_SUPP BIT(1)

struct rsn_ftie {
  wf_u8 mic_control[2];
  wf_u8 mic[16];
  wf_u8 anonce[WPA_NONCE_LEN];
  wf_u8 snonce[WPA_NONCE_LEN];
} STRUCT_PACKED;

#define FTIE_SUBELEM_R1KH_ID 1
#define FTIE_SUBELEM_GTK 2
#define FTIE_SUBELEM_R0KH_ID 3
#define FTIE_SUBELEM_IGTK 4

struct rsn_rdie {
  wf_u8 id;
  wf_u8 descr_count;
  le16 status_code;
} STRUCT_PACKED;


#ifdef _MSC_VER
#pragma pack(pop)
#endif


int wf_wpa_eapol_key_mic(const wf_u8 *key, size_t key_len, int akmp, int ver,
                         const wf_u8 *buf, size_t len, wf_u8 *mic);
int wf_wpa_pmk_to_ptk(const wf_u8 *pmk, size_t pmk_len, const char *label,
                      const wf_u8 *addr1, const wf_u8 *addr2,
                      const wf_u8 *nonce1, const wf_u8 *nonce2,
                      struct wpa_ptk *ptk, int akmp, int cipher);


struct wpa_ie_data {
  int proto;
  int pairwise_cipher;
  int group_cipher;
  int key_mgmt;
  int capabilities;
  size_t num_pmkid;
  const wf_u8 *pmkid;
  int mgmt_group_cipher;
};


int wf_wpa_parse_wpa_ie_rsn(const wf_u8 *rsn_ie, size_t rsn_ie_len,
                            struct wpa_ie_data *data);
int wf_wpa_parse_wpa_ie_wpa(const wf_u8 *wpa_ie, size_t wpa_ie_len,
                            struct wpa_ie_data *data);

void wf_wpa_rsn_pmkid(const wf_u8 *pmk, size_t pmk_len, const wf_u8 *aa, const wf_u8 *spa,
                      wf_u8 *pmkid, int use_sha256);

static inline int rsn_pmkid_suite_b(const wf_u8 *kck, size_t kck_len, const wf_u8 *aa,
				    const wf_u8 *spa, wf_u8 *pmkid)
{
  return -1;
}


static inline int rsn_pmkid_suite_b_192(const wf_u8 *kck, size_t kck_len,
					const wf_u8 *aa, const wf_u8 *spa, wf_u8 *pmkid)
{
  return -1;
}


const char * wf_wpa_cipher_txt(int cipher);
const char * wf_wpa_key_mgmt_txt(int key_mgmt, int proto);
wf_u32 wf_wpa_akm_to_suite(int akm);
int wpa_compare_rsn_ie(int ft_initial_assoc,
		       const wf_u8 *ie1, size_t ie1len,
		       const wf_u8 *ie2, size_t ie2len);
int wpa_insert_pmkid(wf_u8 *ies, size_t *ies_len, const wf_u8 *pmkid);

struct wpa_ft_ies {
  const wf_u8 *mdie;
  size_t mdie_len;
  const wf_u8 *ftie;
  size_t ftie_len;
  const wf_u8 *r1kh_id;
  const wf_u8 *gtk;
  size_t gtk_len;
  const wf_u8 *r0kh_id;
  size_t r0kh_id_len;
  const wf_u8 *rsn;
  size_t rsn_len;
  const wf_u8 *wf_wpa_rsn_pmkid;
  const wf_u8 *tie;
  size_t tie_len;
  const wf_u8 *igtk;
  size_t igtk_len;
  const wf_u8 *ric;
  size_t ric_len;
};

int wpa_ft_parse_ies(const wf_u8 *ies, size_t ies_len, struct wpa_ft_ies *parse);

int wf_wpa_cipher_key_len(int cipher);
int wf_wpa_cipher_rsc_len(int cipher);
enum wpa_alg wf_wpa_cipher_to_alg(int cipher);
int wf_wpa_cipher_valid_group(int cipher);
int wf_wpa_cipher_valid_pairwise(int cipher);
int wpa_cipher_valid_mgmt_group(int cipher);
wf_u32 wf_wpa_cipher_to_suite(int proto, int cipher);
int wf_wpa_pick_pairwise_cipher(int ciphers, int none_allowed);
unsigned int wf_wpa_mic_len(int akmp);

#endif
