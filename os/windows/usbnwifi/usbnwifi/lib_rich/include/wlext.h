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

#ifndef __WL_MLME_EXT_H_
#define __WL_MLME_EXT_H_

#define SURVEY_TO		(100)
#define REAUTH_TO		(300)
#define REASSOC_TO		(300)
#define ADDBA_TO			(2000)

#define LINKED_TO (1)

#define REAUTH_LIMIT	(4)
#define REASSOC_LIMIT	(4)
#define READDBA_LIMIT	(2)

#define ROAMING_LIMIT	8
#if 1
#define CHANNEL    0
#define BW         1
#define CH_OFFSET  2
#endif

#define _HW_STATE_NOLINK_		0x00
#define _HW_STATE_ADHOC_		0x01
#define _HW_STATE_STATION_ 	0x02
#define _HW_STATE_AP_			0x03
#define _HW_STATE_MONITOR_ 0x04

#define _HW_STATE_NO_EXIST_     		0xAA

#define		_1M_RATE_	0
#define		_2M_RATE_	1
#define		_5M_RATE_	2
#define		_11M_RATE_	3
#define		_6M_RATE_	4
#define		_9M_RATE_	5
#define		_12M_RATE_	6
#define		_18M_RATE_	7
#define		_24M_RATE_	8
#define		_36M_RATE_	9
#define		_48M_RATE_	10
#define		_54M_RATE_	11

#define MCS_RATE_1R	(0x000000ff)

extern unsigned char WL_WPA_OUI[];
extern unsigned char WMM_OUI[];
extern unsigned char WPS_OUI[];
extern unsigned char WFD_OUI[];
extern unsigned char P2P_OUI[];

extern unsigned char WMM_INFO_OUI[];
extern unsigned char WMM_PARA_OUI[];

typedef enum _WP_CHANNEL_DOMAIN {
	WL_CHPLAN_FCC = 0x00,
	WL_CHPLAN_IC = 0x01,
	WL_CHPLAN_ETSI = 0x02,
	WL_CHPLAN_SPAIN = 0x03,
	WL_CHPLAN_FRANCE = 0x04,
	WL_CHPLAN_MKK = 0x05,
	WL_CHPLAN_MKK1 = 0x06,
	WL_CHPLAN_ISRAEL = 0x07,
	WL_CHPLAN_TELEC = 0x08,
	WL_CHPLAN_GLOBAL_DOAMIN = 0x09,
	WL_CHPLAN_WORLD_WIDE_13 = 0x0A,
	WL_CHPLAN_TAIWAN = 0x0B,
	WL_CHPLAN_CHINA = 0x0C,
	WL_CHPLAN_SINGAPORE_INDIA_MEXICO = 0x0D,
	WL_CHPLAN_KOREA = 0x0E,
	WL_CHPLAN_TURKEY = 0x0F,
	WL_CHPLAN_JAPAN = 0x10,
	WL_CHPLAN_FCC_NO_DFS = 0x11,
	WL_CHPLAN_JAPAN_NO_DFS = 0x12,
	WL_CHPLAN_WORLD_WIDE_5G = 0x13,
	WL_CHPLAN_TAIWAN_NO_DFS = 0x14,

	WL_CHPLAN_WORLD_NULL = 0x20,
	WL_CHPLAN_ETSI1_NULL = 0x21,
	WL_CHPLAN_FCC1_NULL = 0x22,
	WL_CHPLAN_MKK1_NULL = 0x23,
	WL_CHPLAN_ETSI2_NULL = 0x24,
	WL_CHPLAN_FCC1_FCC1 = 0x25,
	WL_CHPLAN_WORLD_ETSI1 = 0x26,
	WL_CHPLAN_MKK1_MKK1 = 0x27,
	WL_CHPLAN_WORLD_KCC1 = 0x28,
	WL_CHPLAN_WORLD_FCC2 = 0x29,
	WL_CHPLAN_FCC2_NULL = 0x2A,
	WL_CHPLAN_WORLD_FCC3 = 0x30,
	WL_CHPLAN_WORLD_FCC4 = 0x31,
	WL_CHPLAN_WORLD_FCC5 = 0x32,
	WL_CHPLAN_WORLD_FCC6 = 0x33,
	WL_CHPLAN_FCC1_FCC7 = 0x34,
	WL_CHPLAN_WORLD_ETSI2 = 0x35,
	WL_CHPLAN_WORLD_ETSI3 = 0x36,
	WL_CHPLAN_MKK1_MKK2 = 0x37,
	WL_CHPLAN_MKK1_MKK3 = 0x38,
	WL_CHPLAN_FCC1_NCC1 = 0x39,
	WL_CHPLAN_FCC1_NCC2 = 0x40,
	WL_CHPLAN_GLOBAL_NULL = 0x41,
	WL_CHPLAN_ETSI1_ETSI4 = 0x42,
	WL_CHPLAN_FCC1_FCC2 = 0x43,
	WL_CHPLAN_FCC1_NCC3 = 0x44,
	WL_CHPLAN_WORLD_ETSI5 = 0x45,
	WL_CHPLAN_FCC1_FCC8 = 0x46,
	WL_CHPLAN_WORLD_ETSI6 = 0x47,
	WL_CHPLAN_WORLD_ETSI7 = 0x48,
	WL_CHPLAN_WORLD_ETSI8 = 0x49,
	WL_CHPLAN_WORLD_ETSI9 = 0x50,
	WL_CHPLAN_WORLD_ETSI10 = 0x51,
	WL_CHPLAN_WORLD_ETSI11 = 0x52,
	WL_CHPLAN_FCC1_NCC4 = 0x53,
	WL_CHPLAN_WORLD_ETSI12 = 0x54,
	WL_CHPLAN_FCC1_FCC9 = 0x55,
	WL_CHPLAN_WORLD_ETSI13 = 0x56,
	WL_CHPLAN_FCC1_FCC10 = 0x57,
	WL_CHPLAN_MKK2_MKK4 = 0x58,
	WL_CHPLAN_WORLD_ETSI14 = 0x59,
	WL_CHPLAN_FCC1_FCC5 = 0x60,

	WL_CHPLAN_MAX,
	WL_CHPLAN_WK_WLAN_DEFINE = 0x7F,
} WP_CHANNEL_DOMAIN, *WTL_CHANNEL_DOMAIN;

typedef enum _WP_CHANNEL_DOMAIN_2G {
	WL_RD_2G_NULL = 0,
	WL_RD_2G_WORLD = 1,
	WL_RD_2G_ETSI1 = 2,
	WL_RD_2G_FCC1 = 3,
	WL_RD_2G_MKK1 = 4,
	WL_RD_2G_ETSI2 = 5,
	WL_RD_2G_GLOBAL = 6,
	WL_RD_2G_MKK2 = 7,
	WL_RD_2G_FCC2 = 8,

	WL_RD_2G_MAX,
} WP_CHANNEL_DOMAIN_2G, *WTL_CHANNEL_DOMAIN_2G;


bool check_chplan_is_empty_func(u8 id, u8 flag);
#define wl_is_channel_plan_valid(chplan) (((chplan) < WL_CHPLAN_MAX || (chplan) == WL_CHPLAN_WK_WLAN_DEFINE) && !check_chplan_is_empty_func(chplan, 1))
#define wl_is_legacy_channel_plan(chplan) ((chplan) < 0x20)

typedef struct _WP_CHANNEL_PLAN {
	unsigned char Channel[MAX_CHANNEL_NUM];
	unsigned char Len;
} WP_CHANNEL_PLAN, *WTL_CHANNEL_PLAN;

typedef struct _WP_CHANNEL_PLAN_2G {
	unsigned char Channel[14];
	unsigned char Len;
} WP_CHANNEL_PLAN_2G, *WTL_CHANNEL_PLAN_2G;

typedef struct _WP_CHANNEL_PLAN_MAP {
	u8 Index2G;
	u8 regd;
} WP_CHANNEL_PLAN_MAP, *WTL_CHANNEL_PLAN_MAP;

enum Associated_AP {
	atherosAP = 0,
	broadcomAP = 1,
	ciscoAP = 2,
	marvellAP = 3,
	ralinkAP = 4,
	wf_wlanAP = 5,
	airgocapAP = 6,
	unknownAP = 7,
	maxAP,
};

typedef enum _HT_IOT_PEER {
	HT_IOT_PEER_UNKNOWN = 0,
	HT_IOT_PEER_WK_WLAN = 1,
	HT_IOT_PEER_BROADCOM = 3,
	HT_IOT_PEER_RALINK = 4,
	HT_IOT_PEER_ATHEROS = 5,
	HT_IOT_PEER_CISCO = 6,
	HT_IOT_PEER_MARVELL = 8,
	HT_IOT_PEER_MAX = 18
} HT_IOT_PEER_E, *PHTIOT_PEER_E;


enum SCAN_STATE {
	SCAN_DISABLE = 0,
	SCAN_START = 1,
	SCAN_PS_ANNC_WAIT = 2,
	SCAN_ENTER = 3,
	SCAN_PROCESS = 4,

	SCAN_BACKING_OP = 5,
	SCAN_BACK_OP = 6,
	SCAN_LEAVING_OP = 7,
	SCAN_LEAVE_OP = 8,

	SCAN_SW_ANTDIV_BL = 9,

	SCAN_TO_P2P_LISTEN = 10,
	SCAN_P2P_LISTEN = 11,

	SCAN_COMPLETE = 12,
	SCAN_STATE_MAX,
};

const char *proc_scan_state_str_func(u8 state);

enum ss_backop_flag {
	SS_BACKOP_EN = BIT0,
	SS_BACKOP_EN_NL = BIT1,

	SS_BACKOP_PS_ANNC = BIT4,
	SS_BACKOP_TX_RESUME = BIT5,
};

struct ss_res {
	u8 state;
	u8 next_state;
	int bss_cnt;
	int channel_idx;
	int scan_mode;
	u16 scan_ch_ms;
	u8 rx_ampdu_accept;
	u8 rx_ampdu_size;
	u8 igi_scan;
	u8 igi_before_scan;
#ifdef CONFIG_SCAN_BACKOP
	u8 backop_flags_sta;
	u8 backop_flags_ap;
	u8 backop_flags;
	u8 scan_cnt;
	u8 scan_cnt_max;
	u32 backop_time;
	u16 backop_ms;
#endif
#if defined(DBG_SCAN_SW_ANTDIV_BL)
	u8 is_sw_antdiv_bl_scan;
#endif
	u8 ssid_num;
	u8 ch_num;
	NDIS_802_11_SSID ssid[WL_SSID_SCAN_AMOUNT];
	struct wl_ieee80211_channel ch[WL_CHANNEL_SCAN_AMOUNT];
};

#define 	WIFI_FW_NULL_STATE			_HW_STATE_NOLINK_
#define	WIFI_FW_STATION_STATE		_HW_STATE_STATION_
#define	WIFI_FW_AP_STATE				_HW_STATE_AP_
#define	WIFI_FW_ADHOC_STATE			_HW_STATE_ADHOC_

#define	WIFI_FW_AUTH_NULL			0x00000100
#define	WIFI_FW_AUTH_STATE			0x00000200
#define	WIFI_FW_AUTH_SUCCESS			0x00000400

#define	WIFI_FW_ASSOC_STATE			0x00002000
#define	WIFI_FW_ASSOC_SUCCESS		0x00004000

#define	WIFI_FW_LINKING_STATE		(WIFI_FW_AUTH_NULL | WIFI_FW_AUTH_STATE | WIFI_FW_AUTH_SUCCESS |WIFI_FW_ASSOC_STATE)

#define WL_SCAN_NUM_OF_CH 3
#define WL_BACK_OP_CH_MS 400

struct mlme_ext_info {
	u32 state;
	u32 reauth_count;
	u32 reassoc_count;
	u32 link_count;
	u32 auth_seq;
	u32 auth_algo;
	u32 authModeToggle;
	u32 enc_algo;
	u32 key_index;
	u32 iv;
	u8 chg_txt[128];
	u16 aid;
	u16 bcn_interval;
	u16 capability;
	u8 assoc_AP_vendor;
	u8 slotTime;
	u8 preamble_mode;
	u8 WMM_enable;
	u8 ERP_enable;
	u8 ERP_IE;
	u8 HT_enable;
	u8 HT_caps_enable;
	u8 HT_info_enable;
	u8 HT_protection;
	u8 turboMode_cts2self;
	u8 turboMode_rtsen;
	u8 SM_PS;
	u8 agg_enable_bitmap;
	u8 ADDBA_retry_count;
	u8 candidate_tid_bitmap;
	u8 dialogToken;
	BOOLEAN bAcceptAddbaReq;
	u8 bwmode_updated;
	u8 hidden_ssid_mode;

	struct ADDBA_request ADDBA_req;
	struct WMM_para_element WMM_param;
	struct HT_caps_element HT_caps;
	struct HT_info_element HT_info;
	WLAN_BSSID_EX network;
};

typedef struct _WP_CHANNEL_INFO {
	u8 ChannelNum;
	WP_SCAN_TYPE ScanType;
#ifdef CONFIG_FIND_BEST_CHANNEL
	u32 rx_count;
#endif
} WP_CHANNEL_INFO, *WTL_CHANNEL_INFO;

#define wl_rfctl_is_tx_blocked_by_cac(rfctl) _FALSE

int proc_ch_set_search_ch_func(u8 flag, WP_CHANNEL_INFO * ch_set, const u32 ch);
bool check_mlme_band_func(u8 flag, PNIC Nic, const u32 ch);

#define P2P_MAX_REG_CLASSES 10

#define P2P_MAX_REG_CLASS_CHANNELS 20

struct p2p_channels {
	struct p2p_reg_class {
		u8 reg_class;

		u8 channel[P2P_MAX_REG_CLASS_CHANNELS];

		size_t channels;
	} reg_class[P2P_MAX_REG_CLASSES];

	size_t reg_classes;
};

struct p2p_oper_class_map {
	enum hw_mode { IEEE80211G, IEEE80211A } mode;
	u8 op_class;
	u8 min_chan;
	u8 max_chan;
	u8 inc;
	enum { BW20, BW40PLUS, BW40MINUS } bw;
};
typedef struct wlan_bssid_rssi_ {
	u32 rssi;
	u32 linkQ;
	u8 bssid[6];
} wlan_bssid_rssi_t;

#define MAX_BSSID_RSSI_TBL 64

struct mlme_ext_priv {
//	_wadptdata *pwadptdata;
	u8 mlmeext_init;

//	ATOMIC_T event_seq;
    u32 event_seq;
	u16 mgnt_seq;
#ifdef CONFIG_IEEE80211W
	u16 sa_query_seq;
	u64 mgnt_80211w_IPN;
	u64 mgnt_80211w_IPN_rx;
#endif

	unsigned char cur_channel;
	unsigned char cur_bwmode;
	unsigned char cur_ch_offset;
	unsigned char cur_wireless_mode;

	unsigned char max_chan_nums;
	WP_CHANNEL_INFO channel_set[MAX_CHANNEL_NUM];
	struct p2p_channels channel_list;
	unsigned char basicrate[NumRates];
	unsigned char datarate[NumRates];
#ifdef CONFIG_80211N_HT
	unsigned char default_supported_mcs_set[16];
#endif

	struct ss_res sitesurvey_res;
	struct mlme_ext_info mlmext_info;
	WDFTIMER survey_timer;
	WDFTIMER link_timer;

	u32 last_scan_time;
	u8 scan_abort;
	u8 tx_rate;

	u32 retry;

	u64 TSFValue;

	u8 adaptive_tsf_done;
	u32 bcn_delay_cnt[9];
	u32 bcn_delay_ratio[9];
	u32 bcn_cnt;
	u8 DrvBcnEarly;
	u8 DrvBcnTimeOut;

#ifdef CONFIG_AP_MODE
	unsigned char bstart_bss;
#endif

#ifdef CONFIG_80211D
	u8 update_channel_plan_by_ap_done;
#endif
	u8 action_public_dialog_token;
	u16 action_public_rxseq;

	u8 active_keep_alive_check;
#ifdef DBG_FIXED_CHAN
	u8 fixed_chan;
#endif
	u8 en_hw_update_tsf;
	wlan_bssid_rssi_t bssid_rssi[MAX_BSSID_RSSI_TBL];
};


#define mlmeext_msr(mlmeext) ((mlmeext)->mlmext_info.state & 0x03)
#define mlmeext_scan_state(mlmeext) ((mlmeext)->sitesurvey_res.state)
#define mlmeext_scan_state_str(mlmeext) proc_scan_state_str_func((mlmeext)->sitesurvey_res.state)
#define mlmeext_chk_scan_state(mlmeext, _state) ((mlmeext)->sitesurvey_res.state == (_state))
#define mlmeext_set_scan_state(mlmeext, _state) \
	do { \
		((mlmeext)->sitesurvey_res.state = (_state)); \
		((mlmeext)->sitesurvey_res.next_state = (_state)); \
 \
	} while (0)

#define mlmeext_scan_next_state(mlmeext) ((mlmeext)->sitesurvey_res.next_state)
#define mlmeext_set_scan_next_state(mlmeext, _state) \
	do { \
		((mlmeext)->sitesurvey_res.next_state = (_state)); \
 \
	} while (0)

#ifdef CONFIG_SCAN_BACKOP
#define mlmeext_scan_backop_flags(mlmeext) ((mlmeext)->sitesurvey_res.backop_flags)
#define mlmeext_chk_scan_backop_flags(mlmeext, flags) ((mlmeext)->sitesurvey_res.backop_flags & (flags))
#define mlmeext_assign_scan_backop_flags(mlmeext, flags) \
		do { \
			((mlmeext)->sitesurvey_res.backop_flags = (flags)); \
			WL_INFO("assign_scan_backop_flags:0x%02x\n", (mlmeext)->sitesurvey_res.backop_flags); \
		} while (0)

#define mlmeext_scan_backop_flags_sta(mlmeext) ((mlmeext)->sitesurvey_res.backop_flags_sta)
#define mlmeext_chk_scan_backop_flags_sta(mlmeext, flags) ((mlmeext)->sitesurvey_res.backop_flags_sta & (flags))
#define mlmeext_assign_scan_backop_flags_sta(mlmeext, flags) \
	do { \
		((mlmeext)->sitesurvey_res.backop_flags_sta = (flags)); \
	} while (0)

#define mlmeext_scan_backop_flags_ap(mlmeext) ((mlmeext)->sitesurvey_res.backop_flags_ap)
#define mlmeext_chk_scan_backop_flags_ap(mlmeext, flags) ((mlmeext)->sitesurvey_res.backop_flags_ap & (flags))
#define mlmeext_assign_scan_backop_flags_ap(mlmeext, flags) \
	do { \
		((mlmeext)->sitesurvey_res.backop_flags_ap = (flags)); \
	} while (0)
#else
#define mlmeext_scan_backop_flags(mlmeext) (0)
#define mlmeext_chk_scan_backop_flags(mlmeext, flags) (0)
#define mlmeext_assign_scan_backop_flags(mlmeext, flags) do {} while (0)

#define mlmeext_scan_backop_flags_sta(mlmeext) (0)
#define mlmeext_chk_scan_backop_flags_sta(mlmeext, flags) (0)
#define mlmeext_assign_scan_backop_flags_sta(mlmeext, flags) do {} while (0)

#define mlmeext_scan_backop_flags_ap(mlmeext) (0)
#define mlmeext_chk_scan_backop_flags_ap(mlmeext, flags) (0)
#define mlmeext_assign_scan_backop_flags_ap(mlmeext, flags) do {} while (0)
#endif

void init_mlme_default_rate_set(PNIC Nic);
int mlme_ext_priv_to_init_func(PNIC Nic, u8 flag);
int hw_mlme_ext_to_init_func(u8 flag, PNIC Nic);
void proc_mlme_ext_deinit_func(struct mlme_ext_priv *pmlmeext, u8 flag);
extern void do_init_admin_ext_timer(PNIC Nic, u8 tag);
extern void do_init_addba_retry_timer(PNIC Nic, struct sta_info *psta);
extern struct xmit_frame *proc_alloc_mgtxmitframe_func(struct xmit_priv *pxmitpriv);
struct xmit_frame *mgtxmitframe_to_alloc_once_func(struct xmit_priv *pxmitpriv);

unsigned char get_netype_to_raid_ex(PNIC Nic, struct sta_info *psta,
									 u8 tag);

void query_rate_set(PNIC Nic, unsigned char *pbssrate,
				  int *bssrate_len, u8 tag);
void do_set_mcs_by_flag(u8 * mcs_set, u32 mask, u8 tag);
void do_renew_Bratelist(PNIC Nic, u8 * mBratesOS);

void do_set_MSR(PNIC Nic, u8 type);

u8 do_query_var(PNIC Nic, u8 type);
void do_set_var(PNIC Nic, u8 type, u8 var);

void do_set_chabw(PNIC Nic, unsigned char channel,
						unsigned char channel_offset, unsigned short bwmode);
void do_chose_chan(PNIC Nic, unsigned char channel, u8 tag);

void _set_cam(PNIC Nic, u8 * mac, u8 * key, u8 id, u16 ctrl);
void cam_entry_clean(PNIC Nic, u8 id);

void invalidate_cam_all(PNIC Nic, u8 tag);

void do_flush_all_cam(PNIC Nic, u8 tag);

void proc_site_survey_func(PNIC Nic, u8 survey_channel, WP_SCAN_TYPE ScanType,
				 u8 flag);
u8 bss_info_to_collect_func(PNIC Nic, union recv_frame *precv_frame,
					WLAN_BSSID_EX * bssid, u8 flag);

u8 *query_bssid(WLAN_BSSID_EX * pnetwork);

int chk_sta_associated_to_ap(PNIC Nic, u8 tag);

unsigned char assoc_AP_chk(u8 * pframe, uint len, u8 tag);

int WMM_param_handler(PNIC Nic, PNDIS_802_11_VARIABLE_IEs pIE);
void do_WMMAssocRsp_work(PNIC Nic, u8 tag);

void HT_caps_handler(PNIC Nic, PNDIS_802_11_VARIABLE_IEs pIE,
					 u8 tag);
void HT_info_handler(PNIC Nic, PNDIS_802_11_VARIABLE_IEs pIE,
					 u8 tag);
void do_Htassocrsp_work(PNIC Nic);

void ERP_IE_handler(PNIC Nic, PNDIS_802_11_VARIABLE_IEs pIE, u8 tag);
void do_renew_VCS(PNIC Nic, struct sta_info *psta);
void do_renew_ldpc_stbc_cap(struct sta_info *psta, u8 tag);

int do_query_bcn_keys(PNIC Nic, struct beacon_keys *recv_beacon,
					 u8 * pframe, u32 packet_len);
int do_chk_bcn_info(PNIC Nic, u8 * pframe, u32 packet_len, u8 tag);
void do_renew_bcn_info(PNIC Nic, u8 * pframe, uint len,
						struct sta_info *psta);
void do_renew_capinfo(PNIC Nic, u16 updateCap, u8 tag);
void do_renew_wireless_mode(PNIC Nic, u8 tag);
void do_renew_tx_Brate(PNIC Nic, u8 modulation, u8 tag);
int do_ies_query_supported_rate(u8 * rate_set, u8 * rate_num, u8 * ies,
							   uint ies_len);

void proc_sta_info_update_func(PNIC Nic, struct sta_info *psta, u8 flag);

unsigned int proc_receive_disconnect_func(PNIC Nic, unsigned char *MacAddr,
								unsigned short reason, u8 locally_generated,
								u8 flag);

int chk_short_GI(PNIC Nic, u8 bwmode,
					 struct HT_caps_element *pHT_caps);
unsigned int chk_ap_in_tkip(PNIC Nic, u8 tag);

bool _wl_chk_camctl_cap(PNIC Nic, u8 cap);
void wl_set_camctl_mask(PNIC Nic, u32 flags);
bool _wl_chk_camctl_mask(PNIC Nic, u32 flags);

struct sec_cam_bmp;
void wl_sec_cam_map_clr_all(struct sec_cam_bmp *map);

bool chk_camid_gk(PNIC Nic, u8 cam_id, u8 tag);
s16 do_camid_search(s16 kid, s8 gk, PNIC Nic, u8 * addr);
s16 wl_new_camid(PNIC Nic, struct sta_info *sta, u8 kid,
					bool * used, u8 tag);
void wl_unnew_camid(PNIC Nic, u8 cam_id);

//struct macid_bmp;
//struct macid_ctl_t;
void dump_macid_map(void *sel, struct macid_bmp *map, u8 max_num);
bool wl_wmid_is_used(struct macid_ctl_t *macid_ctl, u8 id);
bool wl_wmid_is_bmc(struct macid_ctl_t *macid_ctl, u8 id);
s8 wl_wmid_query_if_g(struct macid_ctl_t *macid_ctl, u8 id);
void wl_new_macid(PNIC Nic, struct sta_info *psta, u8 tag);
void do_release_wmid(PNIC Nic, struct sta_info *psta, u8 tag);
void wl_wmid_ctl_set_msr(struct macid_ctl_t *macid_ctl, u8 id, u8 wmbox1_msr,
							   u8 tag);

u32 join_res_to_report_func(PNIC Nic, int res);
void survey_event_to_report_func(PNIC Nic, union recv_frame *precv_frame,
						 u8 flag);
u32 del_sta_event_to_report_func(PNIC Nic, unsigned char *MacAddr,
						 unsigned short reason, bool enqueue,
						 u8 locally_generated, u8 flag);
void add_sta_event_to_report_func(PNIC Nic, unsigned char *MacAddr, u8 flag);
bool check_port_switch_func(PNIC Nic, u8 flag);
void wmm_edca_update_to_report_func(PNIC Nic, u8 flag);

void cntl_bcn_timing(PNIC Nic, u8 tag);
u8 bmc_sleepq_cmd_to_chk_func(PNIC Nic, u8 flag);
extern u8 proc_set_tx_beacon_cmd_func(PNIC Nic, u8 flag);
unsigned int setup_beacon_frame(PNIC Nic,
								unsigned char *beacon_frame);
void proc_mgnt_tx_rate_update_func(PNIC Nic, u8 rate);
void proc_monitor_frame_attrib_update_func(PNIC Nic,
								 struct pkt_attrib *pattrib, u8 flag);
void proc_mgntframe_attrib_update_func(PNIC Nic, struct pkt_attrib *pattrib);
void proc_mgntframe_attrib_addr_update_func(PNIC Nic,
								  struct xmit_frame *pmgntframe, u8 flag);
void mgntframe_to_dump_func(PNIC Nic, struct xmit_frame *pmgntframe);
s32 mgntframe_and_wait_to_dump_func(PNIC Nic, struct xmit_frame *pmgntframe,
							int timeout_ms);
s32 mgntframe_and_wait_ack_to_dump_func(PNIC Nic,
								struct xmit_frame *pmgntframe);
s32 mgntframe_and_wait_ack_timeout_to_dump_func(PNIC Nic,
										struct xmit_frame *pmgntframe,
										int timeout_ms);

#ifdef CONFIG_P2P
void probersp_p2p_to_issue_func(PNIC Nic, unsigned char *da, u8 flag);
void p2p_provision_request_to_issue_func(PNIC Nic, u8 * pssid, u8 ussidlen,
								 u8 * pdev_raddr, u8 flag);
void p2p_GO_request_to_issue_func(PNIC Nic, u8 * raddr, u8 flag);
void probereq_p2p_to_pre_issue_func(PNIC Nic, u8 * da);
int probereq_p2p_ex_to_issue_func(PNIC Nic, u8 * da, int try_cnt,
						  int wait_ms);
void p2p_invitation_request_to_issue_func(PNIC Nic, u8 * raddr, u8 flag);
#endif
void beacon_to_issue_func(PNIC Nic, int timeout_ms, u8 flag);
void probersp_to_issue_func(PNIC Nic, unsigned char *da,
					u8 is_valid_p2p_probereq, u8 flag);
void assocreq_to_issue_func(PNIC Nic, u8 flag);
void asocrsp_to_issue_func(PNIC Nic, unsigned short status,
				   struct sta_info *pstat, int pkt_type, u8 flag);
void auth_to_issue_func(PNIC Nic, struct sta_info *psta,
				unsigned short status, u8 flag);
void probereq_to_pre_issue_func(PNIC Nic, NDIS_802_11_SSID * pssid, u8 * da);
int nulldata_to_pre_issue_func(PNIC Nic, unsigned char *da,
				   unsigned int power_mode, int try_cnt, int wait_ms);
s32 nulldata_in_interrupt_to_issue_func(PNIC Nic, u8 * da,
								unsigned int power_mode, u8 flag);
int qos_nulldata_to_pre_issue_func(PNIC Nic, unsigned char *da, u16 tid,
					   int try_cnt, int wait_ms);
int deauth_to_pre_issue_func(PNIC Nic, unsigned char *da, unsigned short reason);
int deauth_ex_to_issue_func(PNIC Nic, u8 * da, unsigned short reason,
					int try_cnt, int wait_ms, u8 flag);
void action_spct_ch_switch_to_issue_func(PNIC Nic, u8 * ra, u8 new_ch,
								 u8 ch_offset, u8 flag);
void addba_req_to_issue_func(PNIC Nic, unsigned char *ra, u8 tid, u8 flag);
void addba_rsp_to_issue_func(PNIC Nic, unsigned char *ra, u8 tid, u16 status,
					 u8 size);
u8 addba_rsp_wait_ack_to_issue_func(PNIC Nic, unsigned char *ra, u8 tid,
							u16 status, u8 size, int try_cnt, int wait_ms,
							u8 flag);
void del_ba_to_issue_func(PNIC Nic, unsigned char *ra, u8 tid, u16 reason,
				  u8 initiator);
int del_ba_ex_to_issue_func(PNIC Nic, unsigned char *ra, u8 tid, u16 reason,
					u8 initiator, int try_cnt, int wait_ms, u8 flag);

#ifdef CONFIG_IEEE80211W
void action_SA_Query_to_issue_func(PNIC Nic, unsigned char *raddr,
						   unsigned char action, unsigned short tid,
						   u8 key_type, u8 flag);
int deauth_11w_to_issue_func(PNIC Nic, unsigned char *da,
					 unsigned short reason, u8 key_type);
extern void do_init_dot11w_expire_timer(PNIC Nic,
									 struct sta_info *psta);
#endif
int action_SM_PS_to_pre_issue_func(PNIC Nic, unsigned char *raddr,
					   u8 NewMimoPsMode);
int action_SM_PS_wait_ack_to_issue_func(PNIC Nic, unsigned char *raddr,
								u8 NewMimoPsMode, int try_cnt, int wait_ms);

unsigned int proc_send_delba_sta_tid_func(PNIC Nic, u8 initiator,
								struct sta_info *sta, u8 tid, u8 force);
unsigned int proc_send_delba_sta_tid_wait_ack_func(PNIC Nic, u8 initiator,
										 struct sta_info *sta, u8 tid,
										 u8 force);

unsigned int proc_send_delba_func(PNIC Nic, u8 initiator, u8 * addr);
unsigned int proc_send_beacon_func(PNIC Nic);

void proc_start_clnt_assoc_func(PNIC Nic, u8 flag);
void proc_start_clnt_auth_func(PNIC Nic, u8 flag);

unsigned int proc_doAssocReq_func(PNIC Nic, union recv_frame *precv_frame);
unsigned int proc_doAssocRsp_func(PNIC Nic, union recv_frame *precv_frame);
unsigned int proc_doProbeReq_func(PNIC Nic, union recv_frame *precv_frame);

unsigned int proc_doBeacon_func(PNIC Nic, union recv_frame *precv_frame);
unsigned int proc_doAtim_func(PNIC Nic, union recv_frame *precv_frame);
unsigned int proc_doDisassoc_func(PNIC Nic, union recv_frame *precv_frame);
unsigned int proc_doAuth_func(PNIC Nic, union recv_frame *precv_frame);
unsigned int proc_doAuthClient_func(PNIC Nic, union recv_frame *precv_frame);
unsigned int proc_doDeAuth_func(PNIC Nic, union recv_frame *precv_frame);
unsigned int proc_doAction_func(PNIC Nic, union recv_frame *precv_frame);

unsigned int proc_on_action_spct_func(PNIC Nic, union recv_frame *precv_frame);

#define RX_AMPDU_ACCEPT_INVALID 0xFF
#define RX_AMPDU_SIZE_INVALID 0xFF

enum rx_ampdu_reason {
	RX_AMPDU_DRV_FIXED = 1,
	RX_AMPDU_BTCOEX = 2,
	RX_AMPDU_DRV_SCAN = 3,
};
u8 proc_rx_ampdu_size_func(PNIC Nic, u8 flag);
bool check_rx_ampdu_is_accept_func(PNIC Nic, u8 flag);
bool proc_rx_ampdu_set_size_func(PNIC Nic, u8 size, u8 reason);
bool accept_rx_ampdu_set_func(PNIC Nic, u8 accept, u8 reason);
u8 apply_rx_ampdu_sta_tid_func(PNIC Nic, struct sta_info *sta, u8 tid,
						  u8 accept, u8 size, u8 flag);
u8 apply_rx_ampdu_sta_func(PNIC Nic, struct sta_info *sta, u8 accept,
					  u8 size, u8 flag);

#ifdef CONFIG_IEEE80211W
unsigned int proc_doAction_sa_query_func(PNIC Nic,
							   union recv_frame *precv_frame);
#endif

void callback_of_mlmeext_joinbss_event_func(PNIC Nic, int join_res, u8 flag);
void callback_of_mlmeext_sta_del_event_func(PNIC Nic, u8 flag);
void callback_of_mlmeext_sta_add_event_func(PNIC Nic, struct sta_info *psta,
									u8 flag);

void check_linked_status_func(PNIC Nic, u8 from_timer, u8 flag);

void dump_linked_info_func(PNIC Nic);

void proc_survey_timer_func(PNIC Nic);
void proc_link_timer_func(PNIC Nic);
void proc_addba_timer_func(struct sta_info *psta);
#ifdef CONFIG_IEEE80211W
void query_sa_timer_hdl_func(struct sta_info *psta);
#endif

#define set_survey_timer(mlmeext, ms) \
	do { \
 \
		_set_timer(&(mlmeext)->survey_timer, (ms)); \
	} while(0)

#define set_link_timer(mlmeext, ms) \
	do { \
 \
		_set_timer(&(mlmeext)->link_timer, (ms)); \
	} while(0)

extern void work_addba_req(PNIC Nic, u8 * paddba_req, u8 * addr,
							  u8 tag);

extern void do_renew_TSF(struct mlme_ext_priv *pmlmeext, u8 * pframe, uint len,
					   u8 tag);
extern void devp_early(struct mlme_ext_priv *pmlmeext, u8 * pframe,
							   uint len, u8 tag);
extern u8 watchdog_traffic_status_func(PNIC Nic, u8 from_timer, u8 flag);

#ifdef CONFIG_CONCURRENT_MODE
sint buddy_mlmeinfo_state_to_check_func(PNIC Nic, u32 state, u8 flag);
#endif

void chk_join_done_of_ch_func(PNIC Nic, int join_res, u8 flag);

int check_start_clnt_join_func(PNIC Nic, u8 * ch, u8 * bw, u8 * offset,
							u8 flag);
int proc_get_ch_setting_union_func(PNIC Nic, u8 * ch, u8 * bw, u8 * offset,
							 bool include_self);

void preproc_dev_iface_status_func(PNIC Nic, u8 * sta_num, u8 * ld_sta_num,
						  u8 * lg_sta_num, u8 * ap_num, u8 * ld_ap_num);
void proc_dev_iface_status_no_self_func(PNIC Nic, u8 * sta_num,
								  u8 * ld_sta_num, u8 * lg_sta_num, u8 * ap_num,
								  u8 * ld_ap_num);

struct cmd_hdl {
	uint parmsize;
	 u8(*wmbox1funs) (struct PNIC Nic, u8 * pbuf);
};

u8 read_macreg_hdl(PNIC Nic, u8 * pbuf);
u8 write_macreg_hdl(PNIC Nic, u8 * pbuf);
u8 read_bbreg_hdl(PNIC Nic, u8 * pbuf);
u8 write_bbreg_hdl(PNIC Nic, u8 * pbuf);
u8 read_rfreg_hdl(PNIC Nic, u8 * pbuf);
u8 write_rfreg_hdl(PNIC Nic, u8 * pbuf);

u8 proc_join_cmd_func(PNIC Nic, u8 * pbuf);
u8 proc_disconnect_func(PNIC Nic, u8 * pbuf);
u8 proc_createbss_func(PNIC Nic, u8 * pbuf);
u8 proc_setopmode_func(PNIC Nic, u8 * pbuf);
u8 proc_sitesurvey_cmd_hdl_func(PNIC Nic, u8 * pbuf);
u8 proc_setauth_func(PNIC Nic, u8 * pbuf);
u8 proc_setkey_func(PNIC Nic, u8 * pbuf);
u8 proc_set_stakey_func(PNIC Nic, u8 * pbuf);
u8 set_assocsta_hdl(PNIC Nic, u8 * pbuf);
u8 del_assocsta_hdl(PNIC Nic, u8 * pbuf);
u8 proc_add_ba_func(PNIC Nic, unsigned char *pbuf);
u8 proc_add_ba_rsp_func(PNIC Nic, unsigned char *pbuf);

u8 proc_mlme_evt_func(PNIC Nic, unsigned char *pbuf);
u8 proc_wmbox1_msg_func(PNIC Nic, unsigned char *pbuf);
u8 bmc_sleepq_to_chk_func(PNIC Nic, unsigned char *pbuf);
u8 proc_tx_beacon_func(PNIC Nic, unsigned char *pbuf);
u8 proc_set_ch_func(PNIC Nic, u8 * pbuf);
u8 proc_set_chplan_func(PNIC Nic, unsigned char *pbuf);
u8 proc_led_blink_func(PNIC Nic, unsigned char *pbuf);
u8 proc_set_csa_func(PNIC Nic, unsigned char *pbuf);
u8 proc_tdls_func(PNIC Nic, unsigned char *pbuf);
u8 proc_run_in_thread_func(PNIC Nic, u8 * pbuf);
u8 proc_getmacreg_func(PNIC Nic, u8 * pbuf);
u8 Func_Chip_Bb_Status_Query_Hdl(PNIC Nic, unsigned char *pbuf);
u8 Func_Set_Odm_Var_Hdl(PNIC Nic, unsigned char *pbuf);

#define GEN_DRV_CMD_HANDLER(size, cmd)	{size, cmd},
#define GEN_MLME_EXT_HANDLER(size, cmd)	{size, cmd},

#ifdef _WL_CMD_C_

struct cmd_hdl wlancmds[] = {
	GEN_DRV_CMD_HANDLER(sizeof(struct readMAC_parm), proc_getmacreg_func)
		GEN_DRV_CMD_HANDLER(0, NULL)
		GEN_DRV_CMD_HANDLER(0, NULL)
		GEN_DRV_CMD_HANDLER(0, NULL)
		GEN_DRV_CMD_HANDLER(0, NULL)
		GEN_DRV_CMD_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct joinbss_parm), proc_join_cmd_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct disconnect_parm), proc_disconnect_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct createbss_parm), proc_createbss_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct setopmode_parm), proc_setopmode_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct sitesurvey_parm), proc_sitesurvey_cmd_hdl_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct setauth_parm), proc_setauth_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct setkey_parm), proc_setkey_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct set_stakey_parm), proc_set_stakey_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct set_assocsta_parm), NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct del_assocsta_parm), NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct setstapwrstate_parm), NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct setbasicrate_parm), NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct getbasicrate_parm), NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct setdatarate_parm), NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct getdatarate_parm), NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct setphyinfo_parm), NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct getphyinfo_parm), NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct setphy_parm), NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct getphy_parm), NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct addBaReq_parm), proc_add_ba_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct set_ch_parm), proc_set_ch_func)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(0, NULL)
		GEN_MLME_EXT_HANDLER(sizeof(struct Tx_Beacon_param), proc_tx_beacon_func)
		GEN_MLME_EXT_HANDLER(0, proc_mlme_evt_func)
		GEN_MLME_EXT_HANDLER(0, proc_drvextra_cmd_func)
		GEN_MLME_EXT_HANDLER(0, proc_wmbox1_msg_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct SetChannelPlan_param),
							 proc_set_chplan_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct LedBlink_param), proc_led_blink_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct SetChannelSwitch_param), proc_set_csa_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct TDLSoption_param), proc_tdls_func)
		GEN_MLME_EXT_HANDLER(0, bmc_sleepq_to_chk_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct RunInThread_param),
							 proc_run_in_thread_func)
		GEN_MLME_EXT_HANDLER(sizeof(struct addBaRsp_parm), proc_add_ba_rsp_func)
		GEN_MLME_EXT_HANDLER(0, Func_Chip_Bb_Status_Query_Hdl)
		GEN_MLME_EXT_HANDLER(0, Func_Set_Odm_Var_Hdl)
};

#endif

struct wMBOX0Event_Header {
//CONFIG_LITTLE_ENDIAN
	unsigned int len:16;
	unsigned int ID:8;
	unsigned int seq:8;

	unsigned int rsvd;

};

void do_dummy_event_callback(PNIC Nic, u8 * pbuf);
void wl_fwdbg_event_callback(PNIC Nic, u8 * pbuf);


enum wl_wmbox0_event {
	GEN_EVT_CODE(_Read_MACREG) = 0,
	GEN_EVT_CODE(_Read_BBREG),
	GEN_EVT_CODE(_Read_RFREG),
	GEN_EVT_CODE(_Read_EEPROM),
	GEN_EVT_CODE(_Read_EFUSE),
	GEN_EVT_CODE(_Read_CAM),
	GEN_EVT_CODE(_Get_BasicRate),
	GEN_EVT_CODE(_Get_DataRate),
	GEN_EVT_CODE(_Survey),
	GEN_EVT_CODE(_SurveyDone),

	GEN_EVT_CODE(_JoinBss),
	GEN_EVT_CODE(_AddSTA),
	GEN_EVT_CODE(_DelSTA),
	GEN_EVT_CODE(_AtimDone),
	GEN_EVT_CODE(_TX_Report),
	GEN_EVT_CODE(_CCX_Report),
	GEN_EVT_CODE(_DTM_Report),
	GEN_EVT_CODE(_TX_Rate_Statistics),
	GEN_EVT_CODE(_wMBOX0LBK),
	GEN_EVT_CODE(_FWDBG),
	GEN_EVT_CODE(_wMBOX0FEEDBACK),
	GEN_EVT_CODE(_ADDBA),
	GEN_EVT_CODE(_wMBOX0BCN),
	GEN_EVT_CODE(_ReportPwrState),
	GEN_EVT_CODE(_CloseRF),
	GEN_EVT_CODE(_WMM),
#ifdef CONFIG_IEEE80211W
	GEN_EVT_CODE(_TimeoutSTA),
#endif
	MAX_wMBOX0EVT
};



#if 0
static struct fwevent wlanevents[] = {
	{0, do_dummy_event_callback},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, &do_scan_event_callback},
	{sizeof(struct surveydone_event), &do_scandone_event_callback},

	{0, &do_joinbss_event_callback},
	{sizeof(struct stassoc_event), &wl_stassoc_event_callback},
	{sizeof(struct stadel_event), &wl_stadel_event_callback},
	{0, &wl_do_atimdone_event_cb},
	{0, do_dummy_event_callback},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, wl_fwdbg_event_callback},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, &do_wfprs_event_callback},
	{0, NULL},
	{0, &wl_wmm_event_callback},
#ifdef CONFIG_IEEE80211W
	{sizeof(struct stadel_event), &do_sta_timeout_event_callback},
#endif

};
#else
static struct fwevent wlanevents[] = {
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{sizeof(struct surveydone_event), NULL},

	{0, NULL},
	{sizeof(struct stassoc_event), NULL},
	{sizeof(struct stadel_event), NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
#ifdef CONFIG_IEEE80211W
	{sizeof(struct stadel_event), NULL},
#endif
};

#endif
#endif


