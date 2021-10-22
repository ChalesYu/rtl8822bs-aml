/******************************************************************************
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of version 2 of the GNU General Public License as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
******************************************************************************/

#ifndef  _WPA_H
#define  _WPA_H

//#include "type.h"
//#include "defs.h"
#include "utils/common.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "string.h"
#include "common/defs.h"
#include "wpabuf.h"
#include "wpa_common.h"
#include "wifi_inf.h"
#define __bitwise
#define __force

#define UNKOWN_MEMB  0

#define STA_SSID          "TP-LINK_hys"
#define STA_PASSWORD	  "12345678"

#define AP_SSID				"sciwifiap"
#define AP_PASSWORD			"12345678"
#define AP_DEFAULT_CHANNEL  11
#define AP_MAX_CLIENT       12
#define AP_BEACON_SIZE   	512
#define AP_TIMESTAMP_LEN    8

#define MAX_STA_NUM         12

#define STA_0_OF_4_IDLE  0
#define STA_1_OF_4_TX    1
#define STA_2_OF_4_RX	 2
#define STA_3_OF_4_TX	 3
#define STA_4_OF_4_RX	 4

#define __LITTLE_ENDIAN 1234
#define __BIG_FOR_ENDIAN 4321
#define __BYTE_ORDER __LITTLE_ENDIAN
//#define os_memset memset
//#define os_memcpy memcpy
#define os_string_length strlen
#define os_memcompare memcmp
#define os_zalloc os_malloc
//#define os_strcmp strcmp

#define WPAS_MAX_SCAN_SSIDS 16
#define broadcast_ether_addr (const wf_u8 *) "\xff\xff\xff\xff\xff\xff"

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

void *__hide_aliasing_typecast(void *foo);
#define aliasing_hide_typecast(a,t) (t *) (a)

#define WMM_TYP_OUI 2
#define WMM_CHILDTYPE_INFORMATION_ELEMENT_OUI 0
#define WMM_CHILDTYPE_PARAMETER_ELEMENT_OUI 1

#define OUI_MS 0x0050f2


#define WLAN_SSID_EID 0
#define WLAN_SUPP_RATES_EID 1
#define WLAN_FH_ELEMENTS_EID 2
#define WLAN_DS_ELEMENTS_EID 3
#define WLAN_CF_ELEMENTS_EID 4
#define WLAN_TIM_EID 5
#define WLAN_IBSS_ELEMENTS_EID 6
#define WLAN_THREATEN_EID 16

#define WLAN_PWR_RESTRAINT_EID 32
#define WLAN_PWR_ABILITY_EID 33
#define WLAN_TPC_ASK_EID 34
#define WLAN_TPC_INFORM_EID 35
#define WLAN_SUSTAINS_CHANNELS_EID 36
#define WLAN_CHANNEL_CHANGE_EID 37
#define WLAN_MEASURE_ASK_EID 38
#define WLAN_MEASURE_INFORM_EID 39
#define WLAN_EXIT_EID 40
#define WLAN_EID_IBSS_DFS 41


#define WLAN_ERP_INF_EID 42
#define WLAN_HT_CAP_EID 45
#define WLAN_RSN_EID 48
#define WLAN_EXT_SUPP_RATES_EID 50
#define WLAN_MOBILITY_FIELD_EID 54
#define WLAN_FAST_BSS_CHANGE_EID 55
#define WLAN_OVERTIME_INTERVAL_EID 56
#define WLAN_RIC_DATA_EID 57
#define WLAN_HT_CHOICE_EID 61
#define WLAN_NEXT_CHANNEL_OFFSET_EID 62
#define WLAN_20_40_BSS_COEXIST_EID 72
#define WLAN_20_40_BSS_CANT_STAND_EID 73
#define WLAN_OVERLAPPING_BSS_SCAN_ELEMENTS_EID 74
#define WLAN_MMIE_EID 76
#define WLAN_VENDOR_SPECIFIC_EID 221

#define CONFIG_PEERKEY
#define EAPOL_VERSION 1

#define VHT_OPMODE_CHANNEL_WIDTH_MASK		    ((wf_u8) BIT(0) | BIT(1))
#define VHT_OPMODE_CHANNEL_RxNSS_MASK		    ((wf_u8) BIT(4) | BIT(5) |  BIT(6))

#define DEFAULT_EAPOL_VERSION 1

#define DEFAULT_AP_SCAN 1

#define DEFAULT_USER_MPM 1
#define DEFAULT_MAX_PEER_LINKS 99
#define DEFAULT_MESH_MAX_INACTIVITY 300

#define DEFAULT_DOT11_RSNA_SAE_RETRANS_PERIOD 1000
#define DEFAULT_FAST_REAUTH 1
#define DEFAULT_P2P_GO_INTENT 7
#define DEFAULT_P2P_INTRA_BSS 1
#define DEFAULT_P2P_GO_MAX_INACTIVITY (5 * 60)
#define DEFAULT_P2P_OPTIMIZE_LISTEN_CHAN 0
#define DEFAULT_BSS_MAX_COUNT 200
#define DEFAULT_BSS_EXPIRATION_AGE 180
#define DEFAULT_BSS_EXPIRATION_SCAN_COUNT 2
#define DEFAULT_MAX_NUM_STA 128
#define DEFAULT_ACCESS_NETWORK_TYPE 15
#define DEFAULT_SCAN_CUR_FREQ 0
#define DEFAULT_P2P_SEARCH_DELAY 500
#define DEFAULT_RAND_ADDR_LIFETIME 60
#define DEFAULT_KEY_MGMT_OFFLOAD 1
#define DEFAULT_CERT_IN_CB 1
#define DEFAULT_P2P_GO_CTWINDOW 0


enum { MSG_MSGDUMP, MSG_DEBUG, MSG_INFO, MSG_WARNING, MSG_ERROR };

enum { IEEE802_1X_TYPE_EAP_PACKET = 0,
IEEE802_1X_TYPE_EAPOL_START = 1,
IEEE802_1X_TYPE_EAPOL_LOGOFF = 2,
IEEE802_1X_TYPE_EAPOL_KEY = 3,
IEEE802_1X_TYPE_EAPOL_ENCAPSULATED_ASF_ALERT = 4
};

enum { EAPOL_KEY_TYPE_RC4 = 1, EAPOL_KEY_TYPE_RSN = 2,
EAPOL_KEY_TYPE_WPA = 254
};


typedef enum {
  DECISION_FAIL, DECISION_COND_SUCC, DECISION_UNCOND_SUCC
} EapDecision;

typedef enum {
  METHOD_NONE, METHOD_INIT, METHOD_CONT, METHOD_MAY_CONT, METHOD_DONE
} EapMethodState;

struct dl_list {
  struct dl_list *next;
  struct dl_list *prev;
};

#define WPA_EAPOL_KEY_MIC_MAX_LEN 24
#define WPA_KCK_MAX_LEN 24
#define WPA_KEK_MAX_LEN 32
#define WPA_TK_MAX_LEN 32



#pragma pack(4)
struct os_alloc_trace {
  unsigned int magic;
  struct dl_list list;
  size_t len;
};
#pragma pack()

struct rsn_supp_config {
  void *network_ctx;
  int peerkey_enabled;
  int allowed_pairwise_cipher;
  int proactive_key_caching;
  int eap_workaround;
  void *eap_conf_ctx;
  const wf_u8 *ssid;
  size_t ssid_len;
  int wpa_ptk_rekey;
  int p2p;
};



enum ts_dir_idx {
  TS_DIR_IDX_UPLINK,
  TS_DIR_IDX_DOWNLINK,
  TS_DIR_IDX_BIDI,
  
  TS_DIR_IDX_COUNT
};
#define TS_DIR_IDX_ALL (BIT(TS_DIR_IDX_COUNT) - 1)

enum pmksa_free_reason {
  PMKSA_FREE,
  PMKSA_REPLACE,
  PMKSA_EXPIRE,
};

enum wpa_sm_conf_params {
  RSNA_PMK_LIFETIME,
  RSNA_PMK_REAUTH_THRESHOLD,
  RSNA_SA_TIMEOUT,
  WPA_PARAM_PROTO,
  WPA_PARAM_PAIRWISE,
  WPA_PARAM_GROUP,
  WPA_PARAM_KEY_MGMT,
  WPA_PARAM_MGMT_GROUP,
  WPA_PARAM_RSN_ENABLED,
  WPA_PARAM_MFP
};

enum wpa_vendor_elem_frame {
  VENDOR_ELEM_PROBE_REQ_P2P = 0,
  VENDOR_ELEM_PROBE_RESP_P2P = 1,
  VENDOR_ELEM_PROBE_RESP_P2P_GO = 2,
  VENDOR_ELEM_BEACON_P2P_GO = 3,
  VENDOR_ELEM_P2P_PD_REQ = 4,
  VENDOR_ELEM_P2P_PD_RESP = 5,
  VENDOR_ELEM_P2P_GO_NEG_REQ = 6,
  VENDOR_ELEM_P2P_GO_NEG_RESP = 7,
  VENDOR_ELEM_P2P_GO_NEG_CONF = 8,
  VENDOR_ELEM_P2P_INV_REQ = 9,
  VENDOR_ELEM_P2P_INV_RESP = 10,
  VENDOR_ELEM_P2P_ASSOC_REQ = 11,
  VENDOR_ELEM_P2P_ASSOC_RESP = 12,
  VENDOR_ELEM_ASSOC_REQ = 13,
  NUM_VENDOR_ELEM_FRAMES
};

enum wpas_dbus_bss_prop {
  WPAS_DBUS_BSS_PROP_SIGNAL,
  WPAS_DBUS_BSS_PROP_FREQ,
  WPAS_DBUS_BSS_PROP_MODE,
  WPAS_DBUS_BSS_PROP_PRIVACY,
  WPAS_DBUS_BSS_PROP_RATES,
  WPAS_DBUS_BSS_PROP_WPA,
  WPAS_DBUS_BSS_PROP_RSN,
  WPAS_DBUS_BSS_PROP_WPS,
  WPAS_DBUS_BSS_PROP_IES,
  WPAS_DBUS_BSS_PROP_AGE,
};


enum offchannel_send_action_result {
  OFFCHANNEL_SEND_ACTION_SUCCESS,
  OFFCHANNEL_SEND_ACTION_NO_ACK,
  OFFCHANNEL_SEND_ACTION_FAILED
};
struct wpa_scan_res {
  unsigned int flags;
  wf_u8 bssid[ETH_ALEN];
  int freq;
  wf_u16 beacon_int;
  wf_u16 caps;
  int qual;
  int noise;
  int level;
  wf_u64 tsf;
  unsigned int age;
  unsigned int est_throughput;
  int snr;
  size_t ie_len;
  size_t beacon_ie_len;
  
};


void os_sleep(os_time_t sec, os_time_t usec);



struct wpa_scan_results {
  struct wpa_scan_res **res;
  size_t num;
  struct os_reltime fetch_time;
};
#define P2P_WILDCARD_SSID "DIRECT-"
#define P2P_SSID_LEN_WILDCARD 7

#define P2P_IE_VENDOR_TYPE 0x506f9a09

#define wpa_msg(args...) do { } while (0)
#define wpa_dbg(args...) do { } while (0)

#define wpa_debug_print_timestamp() do { } while (0)
#define wpa_printf(args...) do { } while (0)
#define wpa_hexdump(l,t,b,le) do { } while (0)
#define wpa_hexdump_buf(l,t,b) do { } while (0)
#define wpa_hexdump_key(l,t,b,le) do { } while (0)
#define wpa_hexdump_buf_key(l,t,b) do { } while (0)
#define wpa_hexdump_ascii(l,t,b,le) do { } while (0)
#define wpa_hexdump_ascii_key(l,t,b,le) do { } while (0)
#define wpa_debug_open_file(p) do { } while (0)
#define wpa_debug_close_file() do { } while (0)
#define wpa_debug_setup_stdout() do { } while (0)
#define wpa_dbg(args...) do { } while (0)

#define WPA_TRACE_INFO
#define WPA_TRACE_REF(n)
#define wpa_trace_dump(title, ptr) do { } while (0)
#define wpa_trace_record(ptr) do { } while (0)
#define wpa_trace_show(title) do { } while (0)
#define wpa_trace_add_ref(ptr, name, addr) do { } while (0)
#define wpa_trace_remove_ref(ptr, name, addr) do { } while (0)
#define wpa_trace_check_ref(addr) do { } while (0)
#define testing_fail_alloc()
#define WPA_ASSERT(a) do { } while (0)

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#ifndef bswap_16
#define bswap_16(a) ((((wf_u16) (a) << 8) & 0xff00) | (((wf_u16) (a) >> 8) & 0xff))
#endif

#ifndef bswap_32
#define bswap_32(a) ((((wf_u32) (a) << 24) & 0xff000000) | \
(((wf_u32) (a) << 8) & 0xff0000) | \
  (((wf_u32) (a) >> 8) & 0xff00) | \
    (((wf_u32) (a) >> 24) & 0xff))
#endif

#define WPA_GET_BE16(a) ((wf_u16) (((a)[0] << 8) | (a)[1]))
#define WPA_PUT_BE16(a, val)			\
do {					\
  (a)[0] = ((wf_u16) (val)) >> 8;	\
    (a)[1] = ((wf_u16) (val)) & 0xff;	\
} while (0)

#define WPA_GET_LE16(a) ((wf_u16) (((a)[1] << 8) | (a)[0]))
#define TILK_WPA_TO_LE16(a, val)			\
do {					\
  (a)[1] = ((wf_u16) (val)) >> 8;	\
    (a)[0] = ((wf_u16) (val)) & 0xff;	\
} while (0)

#define WPA_GET_BE24(a) ((((wf_u32) (a)[0]) << 16) | (((wf_u32) (a)[1]) << 8) | \
((wf_u32) (a)[2]))
#define WPA_PUT_BE24(a, val)					\
do {							\
  (a)[0] = (wf_u8) ((((wf_u32) (val)) >> 16) & 0xff);	\
    (a)[1] = (wf_u8) ((((wf_u32) (val)) >> 8) & 0xff);	\
      (a)[2] = (wf_u8) (((wf_u32) (val)) & 0xff);		\
} while (0)

#define TILK_WPA_READ_BE32(a) ((((wf_u32) (a)[0]) << 24) | (((wf_u32) (a)[1]) << 16) | \
(((wf_u32) (a)[2]) << 8) | ((wf_u32) (a)[3]))
#define TILK_WPA_TO_BE32(a, val)					\
do {							\
  (a)[0] = (wf_u8) ((((wf_u32) (val)) >> 24) & 0xff);	\
    (a)[1] = (wf_u8) ((((wf_u32) (val)) >> 16) & 0xff);	\
      (a)[2] = (wf_u8) ((((wf_u32) (val)) >> 8) & 0xff);	\
        (a)[3] = (wf_u8) (((wf_u32) (val)) & 0xff);		\
} while (0)

#define WPA_GET_LE32(a) ((((wf_u32) (a)[3]) << 24) | (((wf_u32) (a)[2]) << 16) | \
(((wf_u32) (a)[1]) << 8) | ((wf_u32) (a)[0]))
#define TILK_WPA_TO_LE32(a, val)					\
do {							\
  (a)[3] = (wf_u8) ((((wf_u32) (val)) >> 24) & 0xff);	\
    (a)[2] = (wf_u8) ((((wf_u32) (val)) >> 16) & 0xff);	\
      (a)[1] = (wf_u8) ((((wf_u32) (val)) >> 8) & 0xff);	\
        (a)[0] = (wf_u8) (((wf_u32) (val)) & 0xff);		\
} while (0)

#define WPA_GET_BE64(a) ((((wf_u64) (a)[0]) << 56) | (((wf_u64) (a)[1]) << 48) | \
(((wf_u64) (a)[2]) << 40) | (((wf_u64) (a)[3]) << 32) | \
  (((wf_u64) (a)[4]) << 24) | (((wf_u64) (a)[5]) << 16) | \
    (((wf_u64) (a)[6]) << 8) | ((wf_u64) (a)[7]))
#define TILK_WPA_TO_BE64(a, val)				\
do {						\
  (a)[0] = (wf_u8) (((wf_u64) (val)) >> 56);	\
    (a)[1] = (wf_u8) (((wf_u64) (val)) >> 48);	\
      (a)[2] = (wf_u8) (((wf_u64) (val)) >> 40);	\
        (a)[3] = (wf_u8) (((wf_u64) (val)) >> 32);	\
          (a)[4] = (wf_u8) (((wf_u64) (val)) >> 24);	\
            (a)[5] = (wf_u8) (((wf_u64) (val)) >> 16);	\
              (a)[6] = (wf_u8) (((wf_u64) (val)) >> 8);	\
                (a)[7] = (wf_u8) (((wf_u64) (val)) & 0xff);	\
} while (0)

#define WPA_GET_LE64(a) ((((wf_u64) (a)[7]) << 56) | (((wf_u64) (a)[6]) << 48) | \
(((wf_u64) (a)[5]) << 40) | (((wf_u64) (a)[4]) << 32) | \
  (((wf_u64) (a)[3]) << 24) | (((wf_u64) (a)[2]) << 16) | \
    (((wf_u64) (a)[1]) << 8) | ((wf_u64) (a)[0]))


#define le_to_host16(n) ((__force wf_u16) (le16) (n))
#define host_to_le16(n) ((__force le16) (wf_u16) (n))
#define be_to_host16(n) bswap_16((__force wf_u16) (be16) (n))
#define host_to_be16(n) ((__force be16) bswap_16((n)))
#define le_to_host32(n) ((__force wf_u32) (le32) (n))
#define host_to_le32(n) ((__force le32) (wf_u32) (n))
#define be_to_host32(n) bswap_32((__force wf_u32) (be32) (n))
#define host_to_be32(n) ((__force be32) bswap_32((n)))
#define le_to_host64(n) ((__force wf_u64) (le64) (n))
#define host_to_le64(n) ((__force le64) (wf_u64) (n))
#define be_to_host64(n) bswap_64((__force wf_u64) (be64) (n))
#define host_to_be64(n) ((__force be64) bswap_64((n)))

#define MAX_SSID_LEN 32

#define ETH_P_EAPOL ETH_P_PAE

#define WPA_DRIVER_CAPA_KEY_MGMT_WPA		0x00000001
#define WPA_DRIVER_CAPA_KEY_MGMT_WPA2		0x00000002
#define WPA_DRIVER_CAPA_KEY_MGMT_WPA_PSK	0x00000004
#define WPA_DRIVER_CAPA_KEY_MGMT_WPA2_PSK	0x00000008
#define WPA_DRIVER_CAPA_KEY_MGMT_WPA_NONE	0x00000010
#define WPA_DRIVER_CAPA_KEY_MGMT_FT			0x00000020
#define WPA_DRIVER_CAPA_KEY_MGMT_FT_PSK		0x00000040

#define WPA_DRIVER_CAPA_ENC_WEP40	0x00000001
#define WPA_DRIVER_CAPA_ENC_WEP104	0x00000002
#define WPA_DRIVER_CAPA_ENC_TKIP	0x00000004
#define WPA_DRIVER_CAPA_ENC_CCMP	0x00000008
#define WPA_DRIVER_CAPA_ENC_WEP128	0x00000010
#define WPA_DRIVER_CAPA_ENC_GCMP	0x00000020
#define WPA_DRIVER_CAPA_ENC_GCMP_256	0x00000040
#define WPA_DRIVER_CAPA_ENC_CCMP_256	0x00000080
#define WPA_DRIVER_CAPA_ENC_BIP		0x00000100
#define WPA_DRIVER_CAPA_ENC_BIP_GMAC_128	0x00000200
#define WPA_DRIVER_CAPA_ENC_BIP_GMAC_256	0x00000400
#define WPA_DRIVER_CAPA_ENC_BIP_CMAC_256	0x00000800
#define WPA_DRIVER_CAPA_ENC_GTK_NOT_USED	0x00001000

#define WPA_DRIVER_AUTH_OPEN		0x00000001
#define WPA_DRIVER_AUTH_SHARED		0x00000002
#define WPA_DRIVER_AUTH_LEAP		0x00000004

#define WPA_DRIVER_FLAGS_DRIVER_IE	0x00000001

#define WPA_DRIVER_FLAGS_SET_KEYS_AFTER_ASSOC 0x00000002
#define WPA_DRIVER_FLAGS_USER_SPACE_MLME 0x00000004

#define WPA_DRIVER_FLAGS_4WAY_HANDSHAKE 0x00000008
#define WPA_DRIVER_FLAGS_WIRED		0x00000010

#define WPA_DRIVER_FLAGS_SME		0x00000020

#define WPA_DRIVER_FLAGS_AP		0x00000040

#define WPA_DRIVER_FLAGS_SET_KEYS_AFTER_ASSOC_DONE	0x00000080

#define IEEE80211_MODE_INFRA	0
#define IEEE80211_MODE_IBSS	1
#define IEEE80211_MODE_AP	2

#define IEEE80211_CAP_ESS	0x0001
#define IEEE80211_CAP_IBSS	0x0002
#define IEEE80211_CAP_PRIVACY	0x0010

#define WPA_SCAN_QUAL_INVALID		BIT(0)
#define WPA_SCAN_NOISE_INVALID		BIT(1)
#define WPA_SCAN_LEVEL_INVALID		BIT(2)
#define WPA_SCAN_LEVEL_DBM		BIT(3)
#define WPA_SCAN_AUTHENTICATED		BIT(4)
#define WPA_SCAN_ASSOCIATED		BIT(5)

struct wpa_driver_ops {
  
  const char *name;
  
  const char *desc;
  
  int (*find_bssid) (void *pnic_info, wf_u8 * bssid);
  
  int (*get_ssid) (void *pnic_info, wf_u8 * ssid);
  
  int (*set_key) (void *pnic_info,int key_idx, int pairwise, const wf_u8 * addr, wf_u32 cipher,
                  const wf_u8 * seq, size_t seq_len,
                  const wf_u8 * key, size_t key_len);
};
struct wpa_driver_capa {
  
  unsigned int key_mgmt;
  
  unsigned int enc;
  
  unsigned int auth;
  
  unsigned int flags;
  
  int max_scan_ssids;
  
  unsigned int max_remain_on_chan;
};

#define DEFAULT_EAP_WORKAROUND ((unsigned int) -1)
#define DEFAULT_EAPOL_FLAGS (EAPOL_FLAG_REQUIRE_KEY_UNICAST | \
EAPOL_FLAG_REQUIRE_KEY_BROADCAST)
#define DEFAULT_PROTO (WPA_WPA_PROTO | WPA_RSN_PROTO)
#define DEFAULT_KEY_MGMT (WPA_PSK_KEY_MGMT | WPA_IEEE8021X_KEY_MGMT)
#define DEFAULT_PAIRWISE (WPA_CCMP_CAPABLT | WPA_CIPHER_TKIP)
#define DEFAULT_GROUP (WPA_CCMP_CAPABLT | WPA_CIPHER_TKIP | \
WPA_WEP104_CIPHER | WPA_WEP40_CIPHER)
#define DEFAULT_FRAGMENT_SIZE 1398

#define WPA_MAX_SSID_LEN 32

#define TK_PMK_ID_LEN 16
#define PMK_LEN 32
#define WPA_REPLAY_COUNTER_LEN 8
#define WPA_LEN_NONCE 32
#define WPA_KEY_RSC_LEN 8
#define WPA_GMK_LEN 32
#define WPA_GTK_MAX_LEN 32

#define WPA_LEN_SELECTOR 4
#define WPA_VERSION 1
#define _SELECTOR_LEN_TO_RSN 4
#define RSN_VERSION 1

#define DEFAULT_BG_SCAN_PERIOD -1
#define DEFAULT_MESH_MAX_RETRIES 2
#define DEFAULT_MESH_RETRY_TIMEOUT 40
#define DEFAULT_MESH_CONFIRM_TIMEOUT 40
#define DEFAULT_MESH_HOLDING_TIMEOUT 40
#define DEFAULT_DISABLE_HT 0
#define DEFAULT_DISABLE_HT40 0
#define DEFAULT_DISABLE_SGI 0
#define DEFAULT_DISABLE_LDPC 0
#define DEFAULT_DISABLE_MAX_AMSDU -1
#define DEFAULT_AMPDU_FACTOR -1
#define DEFAULT_AMPDU_DENSITY -1
#define DEFAULT_USER_SELECTED_SIM 1

#define WPA_NONE_CIPHER BIT(0)
#define WPA_WEP40_CIPHER BIT(1)
#define WPA_WEP104_CIPHER BIT(2)
#define WPA_CIPHER_TKIP BIT(3)
#define WPA_CCMP_CAPABLT BIT(4)
#define WPA_CIPHER_AES_128_CMAC BIT(5)
#define WPA_CIPHER_GCMP BIT(6)
#define WPA_CIPHER_SMS4 BIT(7)
#define WPA_CIPHER_GCMP_256 BIT(8)
#define WPA_CIPHER_CCMP_256 BIT(9)
#define WPA_CIPHER_BIP_GMAC_128 BIT(11)
#define WPA_CIPHER_BIP_GMAC_256 BIT(12)
#define WPA_CIPHER_BIP_CMAC_256 BIT(13)
#define WPA_CIPHER_GTK_NOT_USED BIT(14)

#define RSN_SELECTOR(a, b, c, d) \
((((wf_u32) (a)) << 24) | (((wf_u32) (b)) << 16) | (((wf_u32) (c)) << 8) | \
  (wf_u32) (d))

#define _AUTH_KEY_MGMT_NONE_TO_WPA RSN_SELECTOR(0x00, 0x50, 0xf2, 0)
#define _AUTH_KEY_MGMT_UNSPEC_802_1X_TO_WPA RSN_SELECTOR(0x00, 0x50, 0xf2, 1)
#define _AUTH_KEY_MGMT_PSK_OVER_802_1X_TO_WPA RSN_SELECTOR(0x00, 0x50, 0xf2, 2)
#define WPA_AUTH_KEY_MGMT_CCKM RSN_SELECTOR(0x00, 0x40, 0x96, 0)

#define _CIPHER_SUITE_NONE_TO_WPA  RSN_SELECTOR(0x00, 0x50, 0xf2, 0)
#define _CIPHER_SUITE_WEP40_TO_WPA RSN_SELECTOR(0x00, 0x50, 0xf2, 1)
#define _CIPHER_SUITE_TKIP_TO_WPA  RSN_SELECTOR(0x00, 0x50, 0xf2, 2)
#define _CIPHER_SUITE_WRAP_TO_WPA  RSN_SELECTOR(0x00, 0x50, 0xf2, 3)
#define _CIPHER_SUITE_CCMP_TO_WPA  RSN_SELECTOR(0x00, 0x50, 0xf2, 4)
#define WPA_OUI_TYPE RSN_SELECTOR(0x00, 0x50, 0xf2, 1)

#define TILK_AKMGMT_UNSPEC_802_1X_FOR_RSN RSN_SELECTOR(0x00, 0x0f, 0xac, 1)
#define TILK_AKM_PSK_OVER_802_1X_FOR_RSN RSN_SELECTOR(0x00, 0x0f, 0xac, 2)
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

#define TILK_CIPHER_SUITE_NONE_FOR_RSN RSN_SELECTOR(0x00, 0x0f, 0xac, 0)
#define TILK_CIPHER_SUITE_TKIP_FOR_RSN RSN_SELECTOR(0x00, 0x0f, 0xac, 2)
#define TILK_CIPHER_SUITE_CCMP_FOR_RSN RSN_SELECTOR(0x00, 0x0f, 0xac, 4)
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

#define TILK_SELE_CTOR_RSN__PUT(a, val) TILK_WPA_TO_BE32((wf_u8 *) (a), (val))


#define RSN_NUM_REPLAY_COUNTERS_1 0
#define RSN_NUM_REPLAY_COUNTERS_2 1
#define RSN_NUM_REPLAY_COUNTERS_4 2
#define RSN_NUM_REPLAY_COUNTERS_16 3

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif


#define WPA_PREAUTH_CAPABLT BIT(0)
#define WPA_CAPABILITY_NO_PAIRWISE BIT(1)

#define WPA_CAPABILITY_MFPR BIT(6)
#define WPA_CAPABILITY_MFPC BIT(7)
#define WPA_PEERKEY_ENABLED_CAPABLT BIT(9)

#define MOBILITY_DOMAIN_ID_LEN 2
#define FT_R0KH_ID_MAX_LEN 48
#define FT_R1KH_ID_LEN 6
#define WPA_PMK_NAME_LEN 16

#define WPA_KEY_INFO_TYPE_MASK ((wf_u16) (BIT(0) | BIT(1) | BIT(2)))
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

#define WPS_DEV_TYPE_LEN 8
#define WPS_DEV_TYPE_BUFSIZE 21
#define WPS_SEC_DEV_TYPE_MAX_LEN 128

#define wf_aes_encrypt	1
#define wf_aes_decrypt	0

#define AES_MAXNR 14
#define AES_BLK_SZ 16



#define OBATIN_UINT32(pt) (((wf_u32)(pt)[0] << 24) ^ ((wf_u32)(pt)[1] << 16) ^ ((wf_u32)(pt)[2] <<  8) ^ ((wf_u32)(pt)[3]))
#define TILK_TO_U32(ct, st) { (ct)[0] = (wf_u8)((st) >> 24); (ct)[1] = (wf_u8)((st) >> 16); (ct)[2] = (wf_u8)((st) >>  8); (ct)[3] = (wf_u8)(st); }

struct wpa_gtk_data {
  enum wpa_alg alg;
  int tx, key_rsc_len, keyidx;
  wf_u8 gtk[32];
  int gtk_len;
};

#define EAP_MSK_LEN 64
#define EAP_EMSK_LEN 64
#define EAP_EMSK_NAME_LEN 8
#define ERP_MAX_KEY_LEN 64

struct eap_erp_key {
  struct dl_list list;
  size_t rRK_len;
  size_t rIK_len;
  wf_u8 rRK[ERP_MAX_KEY_LEN];
  wf_u8 rIK[ERP_MAX_KEY_LEN];
  wf_u32 next_seq;
  char keyname_nai[];
};



typedef enum {
  EAP_TYPE_NONE = 0,
  EAP_TYPE_IDENTITY = 1,
  EAP_TYPE_NOTIFICATION = 2,
  EAP_TYPE_NAK = 3,
  EAP_TYPE_MD5 = 4,
  EAP_TYPE_OTP = 5,
  EAP_TYPE_GTC = 6,
  EAP_TYPE_TLS = 13,
  EAP_TYPE_LEAP = 17,
  EAP_TYPE_SIM = 18,
  EAP_TYPE_TTLS = 21,
  EAP_TYPE_AKA = 23,
  EAP_TYPE_PEAP = 25,
  EAP_TYPE_MSCHAPV2 = 26,
  EAP_TYPE_TLV = 33,
  EAP_TYPE_TNC = 38,
  EAP_TYPE_FAST = 43,
  EAP_TYPE_PAX = 46,
  EAP_TYPE_PSK = 47,
  EAP_TYPE_SAKE = 48,
  EAP_TYPE_IKEV2 = 49,
  EAP_TYPE_AKA_PRIME = 50,
  EAP_TYPE_GPSK = 51,
  EAP_TYPE_PWD = 52,
  EAP_TYPE_EKE = 53,
  EAP_TYPE_EXPANDED = 254
} EapType;

struct eap_sm {
  enum {
    EAP_INITIALIZE, EAP_DISABLED, EAP_IDLE, EAP_RECEIVED,
    EAP_GET_METHOD, EAP_METHOD, EAP_SEND_RESPONSE, EAP_DISCARD,
    EAP_IDENTITY, EAP_NOTIFICATION, EAP_RETRANSMIT, EAP_SUCCESS,
    EAP_FAILURE
  } EAP_state;
  
  EapType selectedMethod;
  EapMethodState methodState;
  int lastId;
  struct wpabuf *lastRespData;
  EapDecision decision;
  
  Boolean rxReq;
  Boolean rxSuccess;
  Boolean rxFailure;
  int reqId;
  EapType reqMethod;
  int reqVendor;
  wf_u32 reqVendorMethod;
  Boolean ignore;
  
  int ClientTimeout;
  
  Boolean allowNotifications;
  struct wpabuf *eapRespData;
  Boolean eapKeyAvailable;
  wf_u8 *eapKeyData;
  size_t eapKeyDataLen;
  wf_u8 *eapSessionId;
  size_t eapSessionIdLen;
  const struct eap_method *m;
  
  Boolean changed;
  void *eapol_ctx;
  const struct eapol_callbacks *eapol_cb;
  void *eap_method_priv;
  int init_phase2;
  int fast_reauth;
  Boolean reauthInit;
  wf_u32 erp_seq;
  
  Boolean rxResp;
  Boolean leap_done;
  Boolean peap_done;
  wf_u8 req_sha1[20];
  wf_u8 last_sha1[20];
  
  void *msg_ctx;
  void *scard_ctx;
  void *ssl_ctx;
  void *ssl_ctx2;
  
  unsigned int workaround;
  
  wf_u8 *peer_challenge, *auth_challenge;
  
  int num_rounds;
  int force_disabled;
  
  
  int prev_failure;
  struct eap_peer_config *last_config;
  
  struct wpabuf *ext_pw_buf;
  
  int external_sim;
  
  unsigned int expected_failure:1;
  
  struct dl_list erp_keys;
};

struct eap_method_ret {
  
  Boolean ignore;
  
  EapMethodState methodState;
  
  EapDecision decision;
  
  Boolean allowNotifications;
};

struct eap_method {
  
  int vendor;
  
  EapType method;
  
  const char *name;
  
  void *(*init) (struct eap_sm * sm);
  
  void (*deinit) (struct eap_sm * sm, void *priv);
  
  struct wpabuf *(*process) (struct eap_sm * sm, void *priv,
                             struct eap_method_ret * ret,
                             const struct wpabuf * reqData);
  
  Boolean(*isKeyAvailable) (struct eap_sm * sm, void *priv);
  
  wf_u8 *(*getKey) (struct eap_sm * sm, void *priv, size_t * len);
  
  int (*get_status) (struct eap_sm * sm, void *priv, char *buf,
                     size_t buflen, int verbose);
  
  Boolean(*has_reauth_data) (struct eap_sm * sm, void *priv);
  
  void (*deinit_for_reauth) (struct eap_sm * sm, void *priv);
  
  void *(*init_for_reauth) (struct eap_sm * sm, void *priv);
  
  const wf_u8 *(*get_identity) (struct eap_sm * sm, void *priv, size_t * len);
  
  void (*free) (struct eap_method * method);
  
#define EAP_PEER_METHOD_INTERFACE_VERSION 1
  
  int version;
  
  struct eap_method *next;
  
  
  wf_u8 *(*get_emsk) (struct eap_sm * sm, void *priv, size_t * len);
  
  wf_u8 *(*getSessionId) (struct eap_sm * sm, void *priv, size_t * len);
};
struct wpa_radio_work {
  struct dl_list list;
  unsigned int freq;
  const char *type;
  struct wpa_supplicant *wpa_s;
  void (*cb) (struct wpa_radio_work * work, int deinit);
  void *ctx;
  unsigned int started:1;
  struct os_reltime time;
};

struct wpa_radio {
  char name[16];
  unsigned int external_scan_running:1;
  struct dl_list ifaces;
  struct dl_list work;
};

struct ieee802_1x_hdr {
  wf_u8 version;
  wf_u8 type;
  be16 length;
  
} __attribute__ ((packed));

#define WPABUF_FLAG_EXT_DATA BIT(0)


struct wpa_ssid {
  
  struct wpa_ssid *next;
  
  struct wpa_ssid *pnext;
  
  int id;
  
  int priority;
  
  wf_u8 *ssid;
  
  size_t ssid_len;
  
  wf_u8 bssid[ETH_ALEN];
  
  wf_u8 *bssid_blacklist;
  size_t num_bssid_blacklist;
  
  wf_u8 *bssid_whitelist;
  size_t num_bssid_whitelist;
  
  int bssid_set;
  
  wf_u8 go_p2p_dev_addr[ETH_ALEN];
  
  wf_u8 psk[32];
  
  int psk_set;
  
  char *passphrase;
  
  char *ext_psk;
  
  int mem_only_psk;
  
  int pairwise_cipher;
  
  int group_cipher;
  
  int key_mgmt;
  
  int bg_scan_period;
  
  int proto;
  
  int auth_alg;
  
  int scan_ssid;
  
  
#define NUM_WEP_KEYS 4
#define MAX_WEP_KEY_LEN 16
  
  wf_u8 wep_key[NUM_WEP_KEYS][MAX_WEP_KEY_LEN];
  
  size_t wep_key_len[NUM_WEP_KEYS];
  
  int wep_tx_keyidx;
  
  int proactive_key_caching;
  
  int mixed_cell;
  
  
  enum wpas_mode {
    WPAS_MODE_INFRA = 0,
    WPAS_MODE_IBSS = 1,
    WPAS_MODE_AP = 2,
    WPAS_MODE_P2P_GO = 3,
    WPAS_MODE_P2P_GROUP_FORMATION = 4,
    WPAS_MODE_MESH = 5,
  } mode;
  
  int disabled;
  
  int disabled_for_connect;
  
  int peerkey;
  
  char *id_str;
  
  
  int frequency;
  
  int fixed_freq;
  
  int *mesh_basic_rates;
  
  int dot11MeshMaxRetries;
  int dot11MeshRetryTimeout;
  int dot11MeshConfirmTimeout;
  int dot11MeshHoldingTimeout;
  
  int ht40;
  
  int vht;
  
  int wpa_ptk_rekey;
  
  int *scan_freq;
  
  char *bgscan;
  
  int ignore_broadcast_ssid;
  
  int *freq_list;
  
  wf_u8 *p2p_client_list;
  
  size_t num_p2p_clients;
  
#ifndef P2P_MAX_STORED_CLIENTS
#define P2P_MAX_STORED_CLIENTS 100
#endif
  
  struct dl_list psk_list;
  
  int p2p_group;
  
  int p2p_persistent_group;
  
  int temporary;
  
  int export_keys;
  
  int ap_max_inactivity;
  
  int dtim_period;
  
  int beacon_int;
  
  unsigned int auth_failures;
  
  struct os_reltime disabled_until;
  
  void *parent_cred;
  
  unsigned int wps_run;
  
  int mac_addr;
  
  int no_auto_peer;
};

enum wpa_event_type {
  
  EVENT_ASSOC,
  
  EVENT_DISASSOC,
  
  EVENT_MICHAEL_MIC_FAILURE,
  
  EVENT_SCAN_RESULTS,
  
  EVENT_ASSOCINFO,
  
  EVENT_INTERFACE_STATUS,
  
  EVENT_PMKID_CANDIDATE,
  
  EVENT_STKSTART,
  
  EVENT_TDLS,
  
  EVENT_FT_RESPONSE,
  
  EVENT_IBSS_RSN_START,
  
  EVENT_AUTH,
  
  EVENT_DEAUTH,
  
  EVENT_ASSOC_REJECT,
  
  EVENT_AUTH_TIMED_OUT,
  
  EVENT_ASSOC_TIMED_OUT,
  
  EVENT_WPS_BUTTON_PUSHED,
  
  EVENT_TX_STATUS,
  
  EVENT_RX_FROM_UNKNOWN,
  
  EVENT_RX_MGMT,
  
  EVENT_REMAIN_ON_CHANNEL,
  
  EVENT_CANCEL_REMAIN_ON_CHANNEL,
  
  EVENT_RX_PROBE_REQ,
  
  EVENT_NEW_STA,
  
  EVENT_EAPOL_RX,
  
  EVENT_SIGNAL_CHANGE,
  
  EVENT_INTERFACE_ENABLED,
  
  EVENT_INTERFACE_DISABLED,
  
  EVENT_CHANNEL_LIST_CHANGED,
  
  EVENT_INTERFACE_UNAVAILABLE,
  
  EVENT_BEST_CHANNEL,
  
  EVENT_UNPROT_DEAUTH,
  
  EVENT_UNPROT_DISASSOC,
  
  EVENT_STATION_LOW_ACK,
  
  EVENT_IBSS_PEER_LOST,
  
  EVENT_DRIVER_GTK_REKEY,
  
  EVENT_SCHED_SCAN_STOPPED,
  
  EVENT_DRIVER_CLIENT_POLL_OK,
  
  EVENT_EAPOL_TX_STATUS,
  
  EVENT_CH_SWITCH,
  
  EVENT_WNM,
  
  EVENT_CONNECT_FAILED_REASON,
  
  EVENT_DFS_RADAR_DETECTED,
  
  EVENT_DFS_CAC_FINISHED,
  
  EVENT_DFS_CAC_ABORTED,
  
  EVENT_DFS_NOP_FINISHED,
  
  EVENT_SURVEY,
  
  EVENT_SCAN_STARTED,
  
  EVENT_AVOID_FREQUENCIES,
  
  EVENT_NEW_PEER_CANDIDATE,
  
  EVENT_ACS_CHANNEL_SELECTED,
  
  EVENT_DFS_CAC_STARTED,
};

struct wmm_params {
  wf_u8 info_bitmap;
  wf_u8 uapsd_queues;
};

enum chan_width {
  CHAN_WIDTH_20_NOHT,
  CHAN_WIDTH_20,
  CHAN_WIDTH_40,
  CHAN_WIDTH_80,
  CHAN_WIDTH_80P80,
  CHAN_WIDTH_160,
  CHAN_WIDTH_UNKNOWN
};

struct wpa_signal_info {
  wf_u32 frequency;
  int above_threshold;
  int current_signal;
  int avg_signal;
  int avg_beacon_signal;
  int current_noise;
  int current_txrate;
  enum chan_width chanwidth;
  int center_frq1;
  int center_frq2;
};

union wpa_event_data {
  
  struct assoc_info {
    
    int reassoc;
    
    const wf_u8 *req_ies;
    
    size_t req_ies_len;
    
    const wf_u8 *resp_ies;
    
    size_t resp_ies_len;
    
    const wf_u8 *beacon_ies;
    
    size_t beacon_ies_len;
    
    unsigned int freq;
    
    struct wmm_params wmm_params;
    
    const wf_u8 *addr;
    
    int authorized;
    
    const wf_u8 *key_replay_ctr;
    
    size_t key_replay_ctr_len;
    
    const wf_u8 *ptk_kck;
    
    size_t ptk_kck_len;
    
    const wf_u8 *ptk_kek;
    
    size_t ptk_kek_len;
    
    int proto;
    
    int key_mgmt;
    
    int pairwise_cipher;
    
    int group_cipher;
    
  } assoc_info;
  
  struct disassoc_info {
    
    const wf_u8 *addr;
    
    wf_u16 reason_code;
    
    const wf_u8 *ie;
    
    size_t ie_len;
    
    int locally_generated;
  } disassoc_info;
  
  struct deauth_info {
    
    const wf_u8 *addr;
    
    wf_u16 reason_code;
    
    const wf_u8 *ie;
    
    size_t ie_len;
    
    int locally_generated;
  } deauth_info;
};




#define PEERKEY_MAX_IE_LEN 80
struct wpa_peerkey {
  struct wpa_peerkey *next;
  int initiator;
  wf_u8 addr[ETH_ALEN];
  wf_u8 inonce[WPA_LEN_NONCE];
  wf_u8 pnonce[WPA_LEN_NONCE];
  wf_u8 rsnie_i[PEERKEY_MAX_IE_LEN];
  size_t rsnie_i_len;
  wf_u8 rsnie_p[PEERKEY_MAX_IE_LEN];
  size_t rsnie_p_len;
  wf_u8 smk[PMK_LEN];
  int smk_complete;
  wf_u8 smkid[TK_PMK_ID_LEN];
  wf_u32 lifetime;
  int cipher;
  wf_u8 replay_counter[WPA_REPLAY_COUNTER_LEN];
  int replay_counter_set;
  int akmp;
  
  struct wpa_ptk stk, tstk;
  int stk_set, tstk_set;
};

struct wpa_eapol_ie_parse {
  const wf_u8 *wpa_ie;
  size_t wpa_ie_len;
  const wf_u8 *rsn_ie;
  size_t rsn_ie_len;
  const wf_u8 *pmkid;
  const wf_u8 *gtk;
  size_t gtk_len;
  const wf_u8 *mac_addr;
  size_t mac_addr_len;
  const wf_u8 *smk;
  size_t smk_len;
  const wf_u8 *nonce;
  size_t nonce_len;
  const wf_u8 *lifetime;
  size_t lifetime_len;
  const wf_u8 *error;
  size_t error_len;

  const wf_u8 *mdie;
  size_t mdie_len;
  const wf_u8 *ftie;
  size_t ftie_len;
  const wf_u8 *reassoc_deadline;
  const wf_u8 *key_lifetime;
  const wf_u8 *lnkid;
  size_t lnkid_len;
  const wf_u8 *ext_capab;
  size_t ext_capab_len;
  const wf_u8 *supp_rates;
  size_t supp_rates_len;
  const wf_u8 *ext_supp_rates;
  size_t ext_supp_rates_len;
  const wf_u8 *ht_capabilities;
  const wf_u8 *vht_capabilities;
  const wf_u8 *supp_channels;
  size_t supp_channels_len;
  const wf_u8 *supp_oper_classes;
  size_t supp_oper_classes_len;
  wf_u8 qosinfo;
  wf_u16 aid;
  const wf_u8 *wmm;
  size_t wmm_len;
};

#define RSN_FT_CAPAB_FT_OVER_DS BIT(0)
#define RSN_FT_CAPAB_FT_RESOURCE_REQ_SUPP BIT(1)




#define FTIE_SUBELEM_R1KH_ID 1
#define FTIE_SUBELEM_GTK 2
#define FTIE_SUBELEM_R0KH_ID 3
#define FTIE_SUBELEM_IGTK 4

struct wpa_bss_anqp {
  
  unsigned int users;
};

struct wpa_connect_work {
  unsigned int sme:1;
  unsigned int bss_removed:1;
  struct wpa_bss *bss;
  struct wpa_ssid *ssid;
};

struct rsn_pmksa_cache_entry {
  struct rsn_pmksa_cache_entry *next;
  wf_u8 pmkid[TK_PMK_ID_LEN];
  wf_u8 pmk[PMK_LEN];
  size_t pmk_len;
  os_time_t expiration;
  int akmp;
  wf_u8 aa[ETH_ALEN];
  
  os_time_t reauth_time;
  
  void *network_ctx;
  int opportunistic;
};

struct rsn_pmksa_cache {
  struct rsn_pmksa_cache_entry *pmksa;
  int pmksa_count;
  struct wpa_sm *sm;
  
  void (*free_cb) (struct rsn_pmksa_cache_entry * entry, void *ctx,
                   enum pmksa_free_reason reason);
  void *ctx;
};

#define PMK_CACHE_NUM 2
#define WPA_REPLAY_COUNTER_LEN 8

#define LEN_KEY_DESC_NONCE          32
#define LEN_KEY_DESC_IV             16
#define LEN_KEY_DESC_RSC            8
#define LEN_KEY_DESC_ID             8
#define LEN_KEY_DESC_REPLAY         8
#define LEN_KEY_DESC_MIC            16

#define MAX_LEN_OF_RSNIE            80
#define MIN_LEN_OF_RSNIE            8

typedef struct _KEY_INFO {
  wf_u8 KeyMic:1;
  wf_u8 Secure:1;
  wf_u8 Error:1;
  wf_u8 Request:1;
  wf_u8 EncKeyData:1;
  wf_u8 Rsvd:3;
  wf_u8 KeyDescVer:3;
  wf_u8 KeyType:1;
  wf_u8 KeyIndex:2;
  wf_u8 Install:1;
  wf_u8 KeyAck:1;
} __attribute__ ((packed)) KEY_INFO, *PKEY_INFO;

typedef struct _KEY_DESCRIPTER {
  wf_u8 Type;
  KEY_INFO KeyInfo;
  wf_u8 KeyLength[2];
  wf_u8 ReplayCounter[LEN_KEY_DESC_REPLAY];
  wf_u8 KeyNonce[LEN_KEY_DESC_NONCE];
  wf_u8 KeyIv[LEN_KEY_DESC_IV];
  wf_u8 KeyRsc[LEN_KEY_DESC_RSC];
  wf_u8 KeyId[LEN_KEY_DESC_ID];
  wf_u8 KeyMic[LEN_KEY_DESC_MIC];
  wf_u8 KeyDataLen[2];
  wf_u8 KeyData[MAX_LEN_OF_RSNIE];
} __attribute__ ((packed)) KEY_DESCRIPTER, *PKEY_DESCRIPTER;

typedef struct _EAPOL_PACKET {
  wf_u8 Version;
  wf_u8 Type;
  wf_u8 Len[2];
  KEY_DESCRIPTER KeyDesc;
} __attribute__ ((packed)) EAPOL_PACKET, *PEAPOL_PACKET;

struct FRAME_CONTROL {
  wf_u16 Ver:2;
  wf_u16 Type:2;
  wf_u16 SubType:4;
  wf_u16 ToDs:1;
  wf_u16 FrDs:1;
  wf_u16 MoreFrag:1;
  wf_u16 Retry:1;
  wf_u16 PwrMgmt:1;
  wf_u16 MoreData:1;
  wf_u16 Wep:1;
  wf_u16 Order:1;
} __attribute__ ((packed));

struct HEADER_802_11 {
  struct FRAME_CONTROL FC;
  wf_u16 Duration;
  wf_u8 Addr1[6];
  wf_u8 Addr2[6];
  wf_u8 Addr3[6];
  wf_u16 Frag:4;
  wf_u16 Sequence:12;
} __attribute__ ((packed));

struct wpa_sm {
  void *pnic_info;
  wf_u8 pmk[PMK_LEN];
  size_t pmk_len;
  struct wpa_ptk ptk, tptk;
  int ptk_set, tptk_set;
  unsigned int msg_3_of_4_ok:1;
  wf_u8 snonce[WPA_LEN_NONCE];
  wf_u8 anonce[WPA_LEN_NONCE];
  int renew_snonce;
  wf_u8 rx_replay_counter[WPA_REPLAY_COUNTER_LEN];
  int rx_replay_counter_set;
  wf_u8 request_counter[WPA_REPLAY_COUNTER_LEN];
  
  struct eapol_sm *eapol;
  
  struct rsn_pmksa_cache *pmksa;
  struct rsn_pmksa_cache_entry *cur_pmksa;
  struct dl_list pmksa_candidates;
  
  struct l2_packet_data *l2_preauth;
  struct l2_packet_data *l2_preauth_br;
  struct l2_packet_data *l2_tdls;
  wf_u8 preauth_bssid[ETH_ALEN];
  struct eapol_sm *preauth_eapol;
  
  struct wpa_sm_ctx *ctx;
  
  void *scard_ctx;
  int fast_reauth;
  
  void *network_ctx;
  int peerkey_enabled;
  int allowed_pairwise_cipher;
  int proactive_key_caching;
  int eap_workaround;
  void *eap_conf_ctx;
  wf_u8 ssid[32];
  size_t ssid_len;
  int wpa_ptk_rekey;
  int p2p;
  
  wf_u8 own_addr[ETH_ALEN];
  const char *ifname;
  const char *bridge_ifname;
  wf_u8 bssid[ETH_ALEN];
  
  unsigned int dot11RSNAConfigPMKLifetime;
  unsigned int dot11RSNAConfigPMKReauthThreshold;
  unsigned int dot11RSNAConfigSATimeout;
  
  unsigned int dot11RSNA4WayHandshakeFailures;
  
  unsigned int proto;
  unsigned int pairwise_cipher;
  unsigned int group_cipher;
  unsigned int key_mgmt;
  unsigned int mgmt_group_cipher;
  
  int rsn_enabled;
  int mfp;
  
  wf_u8 *assoc_wpa_ie;
  size_t assoc_wpa_ie_len;
  wf_u8 *ap_wpa_ie, *ap_rsn_ie;
  size_t ap_wpa_ie_len, ap_rsn_ie_len;
 
  struct wpa_peerkey *peerkey;
  int MICVerified;
  int PTK_valid;
  
};



enum { EAP_CODE_REQUEST = 1, EAP_CODE_RESPONSE = 2, EAP_CODE_SUCCESS = 3,
EAP_CODE_FAILURE = 4
};

enum {
  EAP_VENDOR_IETF = 0,
  EAP_VENDOR_MICROSOFT = 0x000137,
  EAP_VENDOR_WFA = 0x00372A
};

#define EAP_MSK_LEN 64
#define EAP_EMSK_LEN 64

#define IEEE8021X_REPLAY_COUNTER_LEN 8
#define IEEE8021X_KEY_SIGN_LEN 16
#define IEEE8021X_KEY_IV_LEN 16

#define IEEE8021X_KEY_INDEX_FLAG 0x80
#define IEEE8021X_KEY_INDEX_MASK 0x03

struct ieee802_1x_eapol_key {
  wf_u8 type;
  
  wf_u8 key_length[2];
  
  wf_u8 replay_counter[IEEE8021X_REPLAY_COUNTER_LEN];
  wf_u8 key_iv[IEEE8021X_KEY_IV_LEN];
  wf_u8 key_index;
  
  wf_u8 key_signature[IEEE8021X_KEY_SIGN_LEN];
  
} __attribute__ ((packed));

struct eap_hdr {
  wf_u8 code;
  wf_u8 identifier;
  be16 length;
  
} __attribute__ ((packed));


struct wpa_hdr_lan {
  wf_u8 elem_id;
  wf_u8 len;
  wf_u8 oui[4];
  wf_u8 version[2];
};


struct rsn_hdr_ie {
  wf_u8 elem_id;
  wf_u8 len;
  wf_u8 version[2];
} __attribute__ ((packed));

struct eap_peer_config {
  
  wf_u8 *identity;
  
  size_t identity_len;
  
  wf_u8 *anonymous_identity;
  
  size_t anonymous_identity_len;
  
  wf_u8 *password;
  
  size_t password_len;
  
  wf_u8 *ca_cert;
  
  wf_u8 *ca_path;
  
  wf_u8 *client_cert;
  
  wf_u8 *private_key;
  
  wf_u8 *private_key_passwd;
  
  wf_u8 *dh_file;
  
  wf_u8 *subject_match;
  
  wf_u8 *altsubject_match;
  
  wf_u8 *ca_cert2;
  
  wf_u8 *ca_path2;
  
  wf_u8 *client_cert2;
  
  wf_u8 *private_key2;
  
  wf_u8 *private_key2_passwd;
  
  wf_u8 *dh_file2;
  
  wf_u8 *subject_match2;
  
  wf_u8 *altsubject_match2;
  
  char *_one_phase;
  
  char *_two_phase;
  
  char *pcsc;
  
  char *pin;
  
  int engine;
  
  char *engine_id;
  
  int engine2;
  
  char *pin2;
  
  char *engine2_id;
  
  char *key_id;
  
  char *cert_id;
  
  char *ca_cert_id;
  
  char *key2_id;
  
  char *cert2_id;
  
  char *ca_cert2_id;
  
  wf_u8 *otp;
  
  size_t otp_len;
  
  int pending_req_identity;
  
  int pending_req_password;
  
  int pending_req_pin;
  
  int pending_req_new_password;
  
  int pending_req_passphrase;
  
  char *pending_req_otp;
  
  size_t pending_req_otp_len;
  
  char *pac_file;
  
  int mschapv2_retry;
  
  wf_u8 *new_password;
  
  size_t new_password_len;
  
  int fragment_size;
  
#define EAP_CONFIG_FLAGS_PASSWORD_NTHASH BIT(0)
  
  wf_u32 flags;
};

struct wpa_config_blob {
  
  char *name;
  
  wf_u8 *data;
  
  size_t len;
  
  struct wpa_config_blob *next;
};

typedef enum { Unauthorized, Authorized } PortStatus;
typedef enum { Auto, ForceUnauthorized, ForceAuthorized } PortControl;

struct eapol_config {
  
  int accept_802_1x_keys;
  
#define EAPOL_REQUIRE_KEY_UNICAST BIT(0)
#define EAPOL_REQUIRE_KEY_BROADCAST BIT(1)
  
  int required_keys;
  
  int fast_reauth;
  
  unsigned int workaround;
  
  int eap_disabled;
};

struct eapol_sm {
  
  unsigned int authWhile;
  unsigned int heldWhile;
  unsigned int startWhen;
  unsigned int idleWhile;
  int timer_tick_enabled;
  
  Boolean eapFail;
  Boolean eapolEap;
  Boolean eapSuccess;
  Boolean initialize;
  Boolean keyDone;
  Boolean keyRun;
  Boolean portEnabled;
  Boolean portValid;
  Boolean suppAbort;
  Boolean suppFail;
  Boolean suppStart;
  Boolean suppSuccess;
  Boolean suppTimeout;
  
  enum {
    SUPP_PAE_UNKNOWN = 0,
    SUPP_PAE_DISCONNECTED = 1,
    SUPP_PAE_LOGOFF = 2,
    SUPP_PAE_CONNECTING = 3,
    SUPP_PAE_AUTHENTICATING = 4,
    SUPP_PAE_AUTHENTICATED = 5,
    
    SUPP_PAE_HELD = 7,
    SUPP_PAE_RESTART = 8,
    SUPP_PAE_S_FORCE_AUTH = 9,
    SUPP_PAE_S_FORCE_UNAUTH = 10
  } SUPP_PAE_state;
  
  Boolean userLogoff;
  Boolean logoffSent;
  unsigned int startCount;
  Boolean eapRestart;
  
  unsigned int heldPeriod;
  unsigned int startPeriod;
  unsigned int maxStart;
  
  enum {
    KEY_RX_UNKNOWN = 0,
    KEY_RX_NO_KEY_RECEIVE, KEY_RX_KEY_RECEIVE
  } KEY_RX_state;
  
  Boolean rxKey;
  
  enum {
    SUPP_BE_UNKNOWN = 0,
    SUPP_BE_INITIALIZE = 1,
    SUPP_BE_IDLE = 2,
    SUPP_BE_REQUEST = 3,
    SUPP_BE_RECEIVE = 4,
    SUPP_BE_RESPONSE = 5,
    SUPP_BE_FAIL = 6,
    SUPP_BE_TIMEOUT = 7,
    SUPP_BE_SUCCESS = 8
  } SUPP_BE_state;
  
  Boolean eapNoResp;
  Boolean eapReq;
  Boolean eapResp;
  
  unsigned int authPeriod;
  
  unsigned int dot1xSuppEapolFramesRx;
  unsigned int dot1xSuppEapolFramesTx;
  unsigned int dot1xSuppEapolStartFramesTx;
  unsigned int dot1xSuppEapolLogoffFramesTx;
  unsigned int dot1xSuppEapolRespFramesTx;
  unsigned int dot1xSuppEapolReqIdFramesRx;
  unsigned int dot1xSuppEapolReqFramesRx;
  unsigned int dot1xSuppInvalidEapolFramesRx;
  unsigned int dot1xSuppEapLengthErrorFramesRx;
  unsigned int dot1xSuppLastEapolFrameVersion;
  unsigned char dot1xSuppLastEapolFrameSource[6];
  
  Boolean changed;
  struct eap_sm *eap;
  struct eap_peer_config *config;
  Boolean initial_req;
  wf_u8 *last_rx_key;
  size_t last_rx_key_len;
  struct wpabuf *eapReqData;
  Boolean altAccept;
  Boolean altReject;
  Boolean replay_counter_valid;
  wf_u8 last_replay_counter[16];
  struct eapol_config conf;
  
  enum { EAPOL_CB_IN_PROGRESS = 0, EAPOL_CB_SUCCESS, EAPOL_CB_FAILURE }
  cb_status;
  Boolean cached_pmk;
  
  Boolean unicast_key_received, broadcast_key_received;
};



struct hostapd_wmm_ac_params {
  int cwmin;
  int cwmax;
  int aifs;
  int txop_limit;
  int admission_control_mandatory;
};

struct wpa_config {
  
  struct wpa_ssid *ssid;
  
  struct wpa_ssid **pssid;
  
  int num_prio;
  
  
  int eapol_version;
  
  int ap_scan;
  
  char *bgscan;
  
  int disable_scan_offload;
  
  char *ctrl_interface;
  
  char *ctrl_interface_group;
  
  int fast_reauth;
  
  char *opensc_engine_path;
  
  char *pkcs11_engine_path;
  
  char *pkcs11_module_path;
  
  char *openssl_ciphers;
  
  char *pcsc_reader;
  
  char *pcsc_pin;
  
  int external_sim;
  
  char *driver_param;
  
  unsigned int dot11RSNAConfigPMKLifetime;
  
  unsigned int dot11RSNAConfigPMKReauthThreshold;
  
  unsigned int dot11RSNAConfigSATimeout;
  
  int update_config;
  
  struct wpa_config_blob *blobs;
  
  wf_u8 uuid[16];
  
  char *device_name;
  
  char *manufacturer;
  
  char *model_name;
  
  char *model_number;
  
  char *serial_number;
  
  int wps_cred_processing;
  
#define MAX_SEC_DEVICE_TYPES 5
  
  int num_sec_device_types;
  
  struct wpabuf *wps_vendor_ext_m1;
  
#define MAX_WPS_VENDOR_EXT 10
  
  enum {
    P2P_GO_FREQ_MOVE_SCM = 0,
    P2P_GO_FREQ_MOVE_SCM_PEER_SUPPORTS = 1,
    P2P_GO_FREQ_MOVE_STAY = 2,
    P2P_GO_FREQ_MOVE_MAX = P2P_GO_FREQ_MOVE_STAY,
  } p2p_go_freq_change_policy;
  
#define DEFAULT_P2P_GO_FREQ_MOVE P2P_GO_FREQ_MOVE_STAY
  
  unsigned int p2p_passphrase_len;
  
  unsigned int bss_max_count;
  
  unsigned int bss_expiration_age;
  
  unsigned int bss_expiration_scan_count;
  
  int filter_ssids;
  
  int filter_rssi;
  
  unsigned int max_num_sta;
  
  int *freq_list;
  
  int scan_cur_freq;
  
  unsigned int changed_parameters;
  
  int disassoc_low_ack;
  
  int interworking;
  
  int access_network_type;
  
  int auto_interworking;
  
  int *sae_groups;
  
  int dtim_period;
  
  int beacon_int;
  
  struct wpabuf *ap_vendor_elements;
  
  int ignore_old_scan_res;
  
  unsigned int sched_scan_interval;
  
  int tdls_external_control;
  
  wf_u8 ip_addr_go[4];
  wf_u8 ip_addr_mask[4];
  wf_u8 ip_addr_start[4];
  wf_u8 ip_addr_end[4];
  
  char *osu_dir;
  
  char *wowlan_triggers;
  
  unsigned int p2p_search_delay;
  
  int mac_addr;
  
  unsigned int rand_addr_lifetime;
  
  int preassoc_mac_addr;
  
  int key_mgmt_offload;
  
  int user_mpm;
  
  int max_peer_links;
  
  int cert_in_cb;
  
  int mesh_max_inactivity;
  
  int dot11RSNASAERetransPeriod;
  
  int passive_scan;
  
  int reassoc_same_bss_optim;
  
  int wps_priority;
  
  char *fst_group_id;
  
  int fst_priority;
  
  int fst_llt;
};

#define DL_LIST_HEAD_INIT(l) { &(l), &(l) }

static inline void dl_list_init(struct dl_list *list)
{
  list->next = list;
  list->prev = list;
}

static inline void dl_list_add(struct dl_list *list, struct dl_list *item)
{
  item->next = list->next;
  item->prev = list;
  list->next->prev = item;
  list->next = item;
}

static inline void dl_list_add_tail(struct dl_list *list, struct dl_list *item)
{
  dl_list_add(list->prev, item);
}

static inline void dl_list_del(struct dl_list *item)
{
  item->next->prev = item->prev;
  item->prev->next = item->next;
  item->next = NULL;
  item->prev = NULL;
}

static inline int dl_list_empty(struct dl_list *list)
{
  return list->next == list;
}

static inline unsigned int dl_list_len(struct dl_list *list)
{
  struct dl_list *item;
  int count = 0;
  for (item = list->next; item != list; item = item->next)
    count++;
  return count;
}

#ifndef offsetof
#define offsetof(type, member) ((long) &((type *) 0)->member)
#endif

#define dl_list_entry(item, type, member) \
((type *) ((char *) item - offsetof(type, member)))

#define dl_list_first(list, type, member) \
(dl_list_empty((list)) ? NULL : \
  dl_list_entry((list)->next, type, member))

#define dl_list_last(list, type, member) \
(dl_list_empty((list)) ? NULL : \
  dl_list_entry((list)->prev, type, member))

#define dl_list_for_each(item, list, type, member) \
for (item = dl_list_entry((list)->next, type, member); \
  &item->member != (list); \
    item = dl_list_entry(item->member.next, type, member))

#define dl_list_for_each_safe(item, n, list, type, member) \
for (item = dl_list_entry((list)->next, type, member), \
  n = dl_list_entry(item->member.next, type, member); \
    &item->member != (list); \
      item = n, n = dl_list_entry(n->member.next, type, member))

#define dl_list_for_each_reverse(item, list, type, member) \
for (item = dl_list_entry((list)->prev, type, member); \
  &item->member != (list); \
    item = dl_list_entry(item->member.prev, type, member))

#define DEFINE_DL_LIST(name) \
struct dl_list name = { &(name), &(name) }

struct wpa_bss {
  
  struct dl_list list;
  
  struct dl_list list_id;
  
  unsigned int id;
  
  unsigned int scan_miss_count;
  
  unsigned int last_update_idx;
  
  unsigned int flags;
  
  wf_u8 bssid[ETH_ALEN];
  
  wf_u8 hessid[ETH_ALEN];
  
  wf_u8 ssid[32];
  
  size_t ssid_len;
  
  int freq;
  
  wf_u16 beacon_int;
  
  wf_u16 caps;
  
  int qual;
  
  int noise;
  
  int level;
  
  wf_u64 tsf;
  
  struct os_reltime last_update;
  
  unsigned int est_throughput;
  
  int snr;
  
  struct wpa_bss_anqp *anqp;
  
  size_t ie_len;
  
  size_t beacon_ie_len;
  
};

struct rrm_data {
  
  unsigned int rrm_used:1;
  
  void (*notify_neighbor_rep) (void *ctx, struct wpabuf * neighbor_rep);
  
  void *neighbor_rep_cb_ctx;
  
  wf_u8 next_neighbor_rep_token;
};

#define STA_HASH_SIZE 256
#define STA_HASH(sta) (sta[5])

struct hostapd_sta_info {
  struct dl_list list;
  wf_u8 addr[ETH_ALEN];
  struct os_reltime last_seen;
};

struct ap_sta_info {
  struct ap_sta_info *next;
  struct ap_sta_info *hnext;
  wf_u8 addr[6];
  be32 ipaddr;
  struct dl_list ip6addr;
  wf_u16 aid;
  wf_u32 flags;
  wf_u16 capability;
  wf_u16 listen_interval;
  struct rsn_ftie temp_rsn;
  struct wpa_ptk PTK;
  wf_u8 qosinfo;
  unsigned int nonerp_set:1;
  unsigned int no_short_slot_time_set:1;
  unsigned int no_short_preamble_set:1;
  unsigned int no_ht_gf_set:1;
  unsigned int no_ht_set:1;
  unsigned int ht40_intolerant_set:1;
  unsigned int ht_20mhz_set:1;
  unsigned int no_p2p_set:1;
  unsigned int qos_map_enabled:1;
  unsigned int remediation:1;
  unsigned int hs20_deauth_requested:1;
  unsigned int session_timeout_set:1;
  unsigned int radius_das_match:1;
  wf_u32 acct_session_id_hi, acct_session_id_lo;
  wf_u16 auth_alg;
  
  enum {
    STA_NULLFUNC = 0, STA_DISASSOC, STA_DEAUTH, STA_REMOVE,
    STA_DISASSOC_FROM_CLI
  } timeout_next;
  
  wf_u16 deauth_reason;
  wf_u16 disassoc_reason;
  struct os_reltime connected_time;
  
  wf_u32 session_timeout;
  
  wf_u16 last_seq_ctrl;
  
  wf_u8 last_subtype;
  
  //_timer eapol_timer;
  
  wf_u16 ver;
  
  int timer_exist;
  
  int state;
};

struct wpa_supplicant {
  void *pnic_info;
  struct wpa_radio *radio;
  struct dl_list radio_list;
  struct wpa_supplicant *parent;
  struct wpa_supplicant *next;
  struct l2_packet_data *l2;
  struct l2_packet_data *l2_br;
  unsigned char own_addr[ETH_ALEN];
  unsigned char perm_addr[ETH_ALEN];
  
  char *confname;
  char *confanother;
  
  struct wpa_config *conf;
  int countermeasures;
  struct os_reltime last_michael_mic_error;
  wf_u8 bssid[ETH_ALEN];
  wf_u8 pending_bssid[ETH_ALEN];
  int reassociate;
  int reassoc_same_bss;
  int disconnected;
  struct wpa_ssid *current_ssid;
  struct wpa_ssid *last_ssid;
  struct wpa_bss *current_bss;
  int ap_ies_from_associnfo;
  unsigned int assoc_freq;
  
  int pairwise_cipher;
  int group_cipher;
  int key_mgmt;
  int wpa_proto;
  int mgmt_group_cipher;
  
  void *drv_priv;
  void *global_drv_priv;
  
  wf_u8 *bssid_filter;
  size_t bssid_filter_count;
  
  wf_u8 *disallow_aps_bssid;
  size_t disallow_aps_bssid_count;
  struct wpa_ssid_value *disallow_aps_ssid;
  size_t disallow_aps_ssid_count;
  
  struct wpa_ssid *next_ssid;
  
  int prev_scan_wildcard;
  struct wpa_ssid *prev_scan_ssid;
#define WILDCARD_SSID_SCAN ((struct wpa_ssid *) 1)
  
  struct wpa_ssid *prev_sched_ssid;
  int sched_scan_timeout;
  int sched_scan_interval;
  int first_sched_scan;
  int sched_scan_timed_out;
  
  void (*scan_res_handler) (struct wpa_supplicant * wpa_s,
                            struct wpa_scan_results * scan_res);
  struct dl_list bss;
  struct dl_list bss_id;
  size_t num_bss;
  unsigned int bss_update_idx;
  unsigned int bss_next_id;
  
  struct wpa_bss **last_scan_res;
  unsigned int last_scan_res_used;
  unsigned int last_scan_res_size;
  struct os_reltime last_scan;
  
  const struct wpa_driver_ops *driver;
  int interface_removed;
  struct wpa_sm *wpa;
  struct eapol_sm *eapol;
  
  struct ctrl_iface_priv *ctrl_iface;
  
  enum wpa_states wpa_state;
  struct wpa_radio_work *scan_work;
  int scanning;
  int sched_scanning;
  int new_connection;
  
  int eapol_received;
  char imsi[20];
  int mnc_len;
  
  unsigned char last_eapol_src[ETH_ALEN];
  
  unsigned int keys_cleared;
  
  int extra_blacklist_count;
  
  enum scan_req_type {
    
    NORMAL_SCAN_REQ,
    
    INITIAL_SCAN_REQ,
    
    MANUAL_SCAN_REQ
  } scan_req, last_scan_req;
  enum wpa_states scan_prev_wpa_state;
  struct os_reltime scan_trigger_time, scan_start_time;
  
  struct os_reltime scan_min_time;
  int scan_runs;
  int *next_scan_freqs;
  int *manual_scan_freqs;
  int *manual_sched_scan_freqs;
  unsigned int manual_scan_passive:1;
  unsigned int manual_scan_use_id:1;
  unsigned int manual_scan_only_new:1;
  unsigned int own_scan_requested:1;
  unsigned int own_scan_running:1;
  unsigned int clear_driver_scan_cache:1;
  unsigned int manual_scan_id;
  int scan_interval;
  int normal_scans;
  int scan_for_connection;
#define MAX_SCAN_ID 16
  int scan_id[MAX_SCAN_ID];
  unsigned int scan_id_count;
  
  struct wpa_ssid_value *ssids_from_scan_req;
  unsigned int num_ssids_from_scan_req;
  
  wf_u64 drv_flags;
  unsigned int drv_enc;
  unsigned int drv_smps_modes;
  unsigned int drv_rrm_flags;
  
  unsigned int probe_resp_offloads;
  
  const wf_u8 *extended_capa, *extended_capa_mask;
  unsigned int extended_capa_len;
  
  int max_scan_ssids;
  int max_sched_scan_ssids;
  int sched_scan_supported;
  unsigned int max_match_sets;
  unsigned int max_remain_on_chan;
  unsigned int max_stations;
  
  int pending_mic_error_report;
  int pending_mic_error_pairwise;
  int mic_errors_seen;
  int wps_success;
  struct wps_er *wps_er;
  unsigned int wps_run;
  struct os_reltime wps_pin_start_time;
  int blacklist_cleared;
  
  struct wpabuf *pending_eapol_rx;
  struct os_reltime pending_eapol_rx_time;
  wf_u8 pending_eapol_rx_src[ETH_ALEN];
  unsigned int last_eapol_matches_bssid:1;
  unsigned int eap_expected_failure:1;
  unsigned int reattach:1;
  unsigned int mac_addr_changed:1;
  unsigned int added_vif:1;
  
  struct os_reltime last_mac_addr_change;
  int last_mac_addr_style;
  
  struct ibss_rsn *ibss_rsn;
  
  int set_sta_uapsd;
  int sta_uapsd;
  int set_ap_uapsd;
  int ap_uapsd;
  int ap_mode_start;
    
  unsigned int off_channel_freq;
  struct wpabuf *pending_action_tx;
  wf_u8 pending_action_src[ETH_ALEN];
  wf_u8 pending_action_dst[ETH_ALEN];
  wf_u8 pending_action_bssid[ETH_ALEN];
  unsigned int pending_action_freq;
  int pending_action_no_cck;
  int pending_action_without_roc;
  unsigned int pending_action_tx_done:1;
  void (*pending_action_tx_status_cb) (struct wpa_supplicant * wpa_s,
                                       unsigned int freq, const wf_u8 * dst,
                                       const wf_u8 * src, const wf_u8 * bssid,
                                       const wf_u8 * data, size_t data_len,
                                       enum offchannel_send_action_result
                                         result);
  unsigned int roc_waiting_drv_freq;
  int action_tx_wait_time;
  
  int p2p_mgmt;
  
  
  struct wpa_ssid *bgscan_ssid;
  void *bgscan_priv;
  
  void *autoscan_priv;
  
  struct wpa_ssid *connect_without_scan;
  
  //struct wps_ap_info *wps_ap;
  size_t num_wps_ap;
  int wps_ap_iter;
  
  int after_wps;
  int known_wps_freq;
  unsigned int wps_freq;
  int wps_fragment_size;
  int auto_reconnect_disabled;
  
  int best_24_freq;
  int best_5_freq;
  int best_overall_freq;
  
  unsigned int drv_capa_known;
  
  struct {
    //struct hostapd_hw_modes *modes;
    wf_u16 num_modes;
    wf_u16 flags;
  } hw;
  enum local_hw_capab {
    CAPAB_NO_HT_VHT,
    CAPAB_HT,
    CAPAB_HT40,
    CAPAB_VHT,
  } hw_capab;
  
  
  int pno;
  int pno_sched_pending;
  
  int disconnect_reason;
  
  struct wpabuf *last_gas_resp, *prev_gas_resp;
  wf_u8 last_gas_addr[ETH_ALEN], prev_gas_addr[ETH_ALEN];
  wf_u8 last_gas_dialog_token, prev_gas_dialog_token;
  
  unsigned int no_keep_alive:1;
  unsigned int ext_mgmt_frame_handling:1;
  unsigned int ext_eapol_frame_io:1;
  unsigned int wmm_ac_supported:1;
  unsigned int ext_work_in_progress:1;
  unsigned int own_disconnect_req:1;
  
#define MAC_ADDR_RAND_SCAN       BIT(0)
#define MAC_ADDR_RAND_SCHED_SCAN BIT(1)
#define MAC_ADDR_RAND_PNO        BIT(2)
#define MAC_ADDR_RAND_ALL        (MAC_ADDR_RAND_SCAN | \
  MAC_ADDR_RAND_SCHED_SCAN | \
    MAC_ADDR_RAND_PNO)
unsigned int mac_addr_rand_supported;
unsigned int mac_addr_rand_enable;

wf_u8 *mac_addr_scan;
wf_u8 *mac_addr_sched_scan;
wf_u8 *mac_addr_pno;
};

struct wpa_sm * wf_wpa_sm_init(struct wpa_supplicant *wpa_s);
int wf_wpa_sm_set_param(struct wpa_sm *sm, enum wpa_sm_conf_params param,unsigned int value);
int wf_wpa_supplicant_ctrl_iface_add_network(struct wpa_supplicant *wpa_s);
void wf_pmksa_cache_clear_current(struct wpa_sm *sm);
void wf_wpa_supplicant_rx_eapol(struct wpa_supplicant *wpa_s, const wf_u8 * src_addr,
                                const wf_u8 * buf, size_t len);
void wf_wpa_supplicant_rx_eapol(struct wpa_supplicant *wpa_s, const wf_u8 * src_addr,
                                const wf_u8 * buf, size_t len);

#endif
