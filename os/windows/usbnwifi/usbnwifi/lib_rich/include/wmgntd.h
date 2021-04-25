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

#ifndef __WL_MLME_H_
#define __WL_MLME_H_
#ifdef CONFIG_LOWMEM
#define	MAX_BSS_CNT	64
#else
#define	MAX_BSS_CNT	128
#endif
#define   MAX_JOIN_TIMEOUT	6500

#define SCANNING_TIMEOUT 8000
#ifdef CONFIG_SCAN_BACKOP
#define CONC_SCANNING_TIMEOUT_SINGLE_BAND 10000
#endif

#define	SCANQUEUE_LIFETIME 20000

#define WIFI_NULL_STATE					0x00000000
#define WIFI_ASOC_STATE					0x00000001
#define WIFI_SLEEP_STATE				0x00000004
#define WIFI_STATION_STATE				0x00000008
#define WIFI_AP_STATE					0x00000010
#define WIFI_ADHOC_STATE				0x00000020
#define WIFI_ADHOC_MASTER_STATE			0x00000040
#define WIFI_UNDER_LINKING				0x00000080
#define WIFI_UNDER_WPS					0x00000100
#define WIFI_STA_ALIVE_CHK_STATE		0x00000400
#define WIFI_SITE_MONITOR				0x00000800
#define WIFI_MP_STATE					0x00010000
#define WIFI_OP_CH_SWITCHING			0x00800000
#define WIFI_FW_NO_EXIST                0x01000000

#define WIFI_MONITOR_STATE				0x80000000

#define MIRACAST_DISABLED	0
#define MIRACAST_SOURCE		BIT0
#define MIRACAST_SINK		BIT1

#define MIRACAST_MODE_REVERSE(mode) \
	((((mode) & MIRACAST_SOURCE) ? MIRACAST_SINK : 0) | (((mode) & MIRACAST_SINK) ? MIRACAST_SOURCE : 0))

bool chk_miracast_en(PNIC Nic, u8 tag);
bool do_chk_miracast_mode(PNIC Nic, u8 mode);
const char *do_query_miracast_str(int mode);

#define MLME_STATE(wadptdata) get_fwstate(&((wadptdata)->mlmepriv))

#define MLME_IS_STA(wadptdata) (MLME_STATE((wadptdata)) & WIFI_STATION_STATE)
#define MLME_IS_AP(wadptdata) (MLME_STATE((wadptdata)) & WIFI_AP_STATE)
#define MLME_IS_ADHOC(wadptdata) (MLME_STATE((wadptdata)) & WIFI_ADHOC_STATE)
#define MLME_IS_ADHOC_MASTER(wadptdata) (MLME_STATE((wadptdata)) & WIFI_ADHOC_MASTER_STATE)
#define MLME_IS_MONITOR(wadptdata) (MLME_STATE((wadptdata)) & WIFI_MONITOR_STATE)
#define MLME_IS_MP(wadptdata) (MLME_STATE((wadptdata)) & WIFI_MP_STATE)
#ifdef CONFIG_P2P
#define MLME_IS_GC(wadptdata) wl_p2p_chk_role(&(wadptdata)->wdinfo, P2P_ROLE_CLIENT)
#define MLME_IS_GO(wadptdata) wl_p2p_chk_role(&(wadptdata)->wdinfo, P2P_ROLE_GO)
#else
#define MLME_IS_GC(wadptdata) 0
#define MLME_IS_GO(wadptdata) 0
#endif
#define MLME_IS_MSRC(wadptdata) do_chk_miracast_mode((wadptdata), MIRACAST_SOURCE)
#define MLME_IS_MSINK(wadptdata) do_chk_miracast_mode((wadptdata), MIRACAST_SINK)

#define MLME_STATE_FMT "%s%s%s%s%s%s%s%s%s%s%s%s%s%s"
#define MLME_STATE_ARG(wadptdata) \
	MLME_IS_STA((wadptdata)) ? (MLME_IS_GC((wadptdata)) ? " GC" : " STA") : "", \
	MLME_IS_AP((wadptdata)) ? (MLME_IS_GO((wadptdata)) ? " GO" : " AP") : "", \
	MLME_IS_ADHOC((wadptdata)) ? " ADHOC" : "", \
	MLME_IS_ADHOC_MASTER((wadptdata)) ? " ADHOC_M" : "", \
	MLME_IS_MONITOR((wadptdata)) ? " MONITOR" : "", \
	MLME_IS_MP((wadptdata)) ? " MP" : "", \
	MLME_IS_MSRC((wadptdata)) ? " MSRC" : "", \
	MLME_IS_MSINK((wadptdata)) ? " MSINK" : "", \
	(MLME_STATE((wadptdata)) & WIFI_SITE_MONITOR) ? " SCAN" : "", \
	(MLME_STATE((wadptdata)) & WIFI_UNDER_LINKING) ? " LINKING" : "", \
	(MLME_STATE((wadptdata)) & WIFI_ASOC_STATE) ? " ASOC" : "", \
	(MLME_STATE((wadptdata)) & WIFI_OP_CH_SWITCHING) ? " OP_CH_SW" : "", \
	(MLME_STATE((wadptdata)) & WIFI_UNDER_WPS) ? " WPS" : "", \
	(MLME_STATE((wadptdata)) & WIFI_SLEEP_STATE) ? " SLEEP" : ""

#define _FW_UNDER_LINKING	WIFI_UNDER_LINKING
#define _FW_LINKED			WIFI_ASOC_STATE
#define _FW_UNDER_SURVEY	WIFI_SITE_MONITOR

#define WAIT_DONE	0
#define	ABORT		1
#define ABORT_NO_WAIT	2

enum dot11AuthAlgrthmNum {
	dot11AuthAlgrthm_Open = 0,
	dot11AuthAlgrthm_Shared,
	dot11AuthAlgrthm_8021X,
	dot11AuthAlgrthm_Auto,
	dot11AuthAlgrthm_WAPI,
	dot11AuthAlgrthm_MaxNum
};

typedef enum _WP_SCAN_TYPE {
	SCAN_PASSIVE,
	SCAN_ACTIVE,
	SCAN_MIX,
} WP_SCAN_TYPE, *WTL_SCAN_TYPE;

#define WIFI_FREQUENCY_BAND_AUTO 0
#define WIFI_FREQUENCY_BAND_2GHZ 2

#define wl_band_valid(band) ((band) <= WIFI_FREQUENCY_BAND_2GHZ)

enum DriverInterface {
	DRIVER_WEXT = 1,
	DRIVER_CFG80211 = 2
};

enum SCAN_RESULT_TYPE {
	SCAN_RESULT_P2P_ONLY = 0,
	SCAN_RESULT_ALL = 1,
	SCAN_RESULT_WFD_TYPE = 2
};

typedef struct _WP_LINK_DETECT_T {
	u32 NumTxOkInPeriod;
	u32 NumRxOkInPeriod;
	u32 NumRxUnicastOkInPeriod;
	BOOLEAN bBusyTraffic;
	BOOLEAN bTxBusyTraffic;
	BOOLEAN bRxBusyTraffic;
	BOOLEAN bHigherBusyTraffic;
	BOOLEAN bHigherBusyRxTraffic;
	BOOLEAN bHigherBusyTxTraffic;
	u8 TrafficTransitionCount;
	u32 LowPowerTransitionCount;
} WP_LINK_DETECT_T, *WTL_LINK_DETECT_T;

struct profile_info {
	u8 ssidlen;
	u8 ssid[WLAN_SSID_MAXLEN];
	u8 peermac[ETH_ALEN];
};

struct tx_invite_req_info {
	u8 token;
	u8 benable;
	u8 go_ssid[WLAN_SSID_MAXLEN];
	u8 ssidlen;
	u8 go_bssid[ETH_ALEN];
	u8 peer_macaddr[ETH_ALEN];
	u8 operating_ch;
	u8 peer_ch;

};

struct tx_invite_resp_info {
	u8 token;
};


struct tx_provdisc_req_info {
	u16 wps_config_method_request;
	u16 peer_channel_num[2];
	NDIS_802_11_SSID ssid;
	u8 peerDevAddr[ETH_ALEN];
	u8 peerIFAddr[ETH_ALEN];
	u8 benable;
};

struct rx_provdisc_req_info {
	u8 peerDevAddr[ETH_ALEN];
	u8 strconfig_method_desc_of_prov_disc_req[4];
};

struct tx_nego_req_info {
	u16 peer_channel_num[2];
	u8 peerDevAddr[ETH_ALEN];
	u8 benable;
	u8 peer_ch;
};

struct group_id_info {
	u8 go_device_addr[ETH_ALEN];
	u8 ssid[WLAN_SSID_MAXLEN];
};

struct scan_limit_info {
	u8 scan_op_ch_only;
#ifndef CONFIG_P2P_OP_CHK_SOCIAL_CH
	u8 operation_ch[2];
#else
	u8 operation_ch[5];
#endif
};

#ifdef CONFIG_P2P_WOWLAN

enum P2P_WOWLAN_RECV_FRAME_TYPE {
	P2P_WOWLAN_RECV_NEGO_REQ = 0,
	P2P_WOWLAN_RECV_INVITE_REQ = 1,
	P2P_WOWLAN_RECV_PROVISION_REQ = 2,
};

struct p2p_wowlan_info {

	u8 is_trigger;
	enum P2P_WOWLAN_RECV_FRAME_TYPE wowlan_recv_frame_type;
	u8 wowlan_peer_addr[ETH_ALEN];
	u16 wowlan_peer_wpsconfig;
	u8 wowlan_peer_is_persistent;
	u8 wowlan_peer_invitation_type;
};

#endif

#ifdef CONFIG_P2P
struct wifidirect_info {
	_wadptdata *pwadptdata;
	_timer find_phase_timer;
	_timer restore_p2p_state_timer;

	_timer pre_tx_scan_timer;
	_timer reset_ch_sitesurvey;
	_timer reset_ch_sitesurvey2;
#ifdef CONFIG_CONCURRENT_MODE
	_timer ap_p2p_switch_timer;
#endif
	struct tx_provdisc_req_info tx_prov_disc_info;
	struct rx_provdisc_req_info rx_prov_disc_info;
	struct tx_invite_req_info invitereq_info;
	struct profile_info profileinfo[P2P_MAX_PERSISTENT_GROUP_NUM];
	struct tx_invite_resp_info inviteresp_info;
	struct tx_nego_req_info nego_req_info;
	struct group_id_info groupid_info;
	struct scan_limit_info rx_invitereq_info;
	struct scan_limit_info p2p_info;

#ifdef CONFIG_P2P_WOWLAN
	struct p2p_wowlan_info p2p_wow_info;
#endif

	enum P2P_ROLE role;
	enum P2P_STATE pre_p2p_state;
	enum P2P_STATE p2p_state;
	u8 device_addr[ETH_ALEN];
	u8 interface_addr[ETH_ALEN];
	u8 social_chan[4];
	u8 listen_channel;
	u8 operating_channel;
	u8 listen_dwell;
	u8 support_rate[8];
	u8 p2p_wildcard_ssid[P2P_WILDCARD_SSID_LEN];
	u8 intent;
	u8 p2p_peer_interface_addr[ETH_ALEN];
	u8 p2p_peer_device_addr[ETH_ALEN];
	u8 peer_intent;
	u8 device_name[WPS_MAX_DEVICE_NAME_LEN];
	u8 device_name_len;
	u8 profileindex;
	u8 peer_operating_ch;
	u8 find_phase_state_exchange_cnt;
	u16 device_password_id_for_nego;
	u8 negotiation_dialog_token;
	u8 nego_ssid[WLAN_SSID_MAXLEN];
	u8 nego_ssidlen;
	u8 p2p_group_ssid[WLAN_SSID_MAXLEN];
	u8 p2p_group_ssid_len;
	u8 persistent_supported;
	u8 session_available;

	enum P2P_WPSINFO ui_got_wps_info;
	u16 supported_wps_cm;
	u8 external_uuid;
	u8 uuid[16];
	uint channel_list_attr_len;
	u8 channel_list_attr[100];
	u8 driver_interface;

#ifdef CONFIG_CONCURRENT_MODE
	u16 ext_listen_interval;
	u16 ext_listen_period;
#endif

};
#endif

#ifdef CONFIG_TDLS
struct tdls_ss_record {
	u8 macaddr[ETH_ALEN];
	u8 RxPWDBAll;
	u8 is_tdls_sta;
};

struct tdls_temp_mgmt {
	u8 initiator;
	u8 peer_addr[ETH_ALEN];
};

struct tdls_info {
	u8 ap_prohibited;
	u8 ch_switch_prohibited;
	u8 link_established;
	u8 sta_cnt;
	u8 sta_maximum;
	struct tdls_ss_record ss_record;

	u8 ch_sensing;
	u8 cur_channel;
	u8 collect_pkt_num[MAX_CHANNEL_NUM];
	_lock cmd_lock;
	_lock hdl_lock;
	u8 watchdog_count;
	u8 dev_discovered;
	u8 tdls_enable;

	u8 driver_setup;
};

struct tdls_txmgmt {
	u8 peer[ETH_ALEN];
	u8 action_code;
	u8 dialog_token;
	u16 status_code;
	u8 *buf;
	size_t len;
};
#endif

enum {
	WL_ROAM_ON_EXPIRED = BIT0,
	WL_ROAM_ON_RESUME = BIT1,
	WL_ROAM_ACTIVE = BIT2,
};

struct beacon_keys {
	u8 ssid[IW_ESSID_MAX_SIZE];
	u32 ssid_len;
	u8 bcn_channel;
	u16 ht_cap_info;
	u8 ht_info_infos_0_sco;
	int encryp_protocol;
	int pairwise_cipher;
	int group_cipher;
	int is_8021x;
};

struct qos_priv {
	unsigned int qos_option;
};

struct mlme_priv {

	KSPIN_LOCK lock;
	sint fw_state;
	u8 bScanInProcess;
	u8 to_join;
#ifdef CONFIG_LAYER2_ROAMING
	u8 to_roam;
	struct wlan_network *roam_network;
	u8 roam_flags;
	u8 roam_rssi_diff_th;
	u32 roam_scan_int_ms;
	u32 roam_scanr_exp_ms;
	u8 roam_tgt_addr[ETH_ALEN];
#endif

	u8 *nic_hdl;

	LIST_ENTRY *pscanned;
	_queue free_bss_pool;
	_queue scanned_queue;
	u8 *free_bss_buf;
	u32 num_of_scanned;

	NDIS_802_11_SSID assoc_ssid;
	u8 assoc_bssid[6];

	struct wlan_network cur_network;
	struct wlan_network *cur_network_scanned;

	struct beacon_keys cur_beacon_keys;
	struct beacon_keys new_beacon_keys;
	u8 new_beacon_cnts;


	u32 auto_scan_int_ms;

	WDFTIMER assoc_timer;

	uint assoc_by_bssid;
	uint assoc_by_rssi;

	WDFTIMER scan_to_timer;
	u32 scan_start_time;

#ifdef CONFIG_SET_SCAN_DENY_TIMER
	_timer set_scan_deny_timer;
	ATOMIC_T set_scan_deny;
#endif

	struct qos_priv qospriv;

#ifdef CONFIG_80211N_HT

	int num_sta_no_ht;

	int num_FortyMHzIntolerant;

	struct ht_priv htpriv;

#endif

	WP_LINK_DETECT_T LinkDetectInfo;
	WDFTIMER dynamic_chk_timer;

	u8 acm_mask;
	const struct country_chplan *country_ent;
	u8 ChannelPlan;
	WP_SCAN_TYPE scan_mode;

	u8 *wps_probe_req_ie;
	u32 wps_probe_req_ie_len;

	u8 ext_capab_ie_data[8];
	u8 ext_capab_ie_len;

#if defined (CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME)
	int num_sta_non_erp;

	int num_sta_no_short_slot_time;

	int num_sta_no_short_preamble;

	int olbc;

	int num_sta_ht_no_gf;

	int num_sta_ht_20mhz;

	int num_sta_40mhz_intolerant;

	int olbc_ht;

#ifdef CONFIG_80211N_HT
	int ht_20mhz_width_req;
	int ht_intolerant_ch_reported;
	u16 ht_op_mode;
	u8 sw_to_20mhz;
#endif

	u8 *assoc_req;
	u32 assoc_req_len;
	u8 *assoc_rsp;
	u32 assoc_rsp_len;

	u8 *wps_beacon_ie;
	u8 *wps_probe_resp_ie;
	u8 *wps_assoc_resp_ie;

	u32 wps_beacon_ie_len;
	u32 wps_probe_resp_ie_len;
	u32 wps_assoc_resp_ie_len;

	u8 *p2p_beacon_ie;
	u8 *p2p_probe_req_ie;
	u8 *p2p_probe_resp_ie;
	u8 *p2p_go_probe_resp_ie;
	u8 *p2p_assoc_req_ie;
	u8 *p2p_assoc_resp_ie;

	u32 p2p_beacon_ie_len;
	u32 p2p_probe_req_ie_len;
	u32 p2p_probe_resp_ie_len;
	u32 p2p_go_probe_resp_ie_len;
	u32 p2p_assoc_req_ie_len;
	u32 p2p_assoc_resp_ie_len;

	_lock bcn_update_lock;
	u8 update_bcn;

	u8 ori_ch;
	u8 ori_bw;
	u8 ori_offset;
#endif


#ifdef CONFIG_CONCURRENT_MODE
	u8 scanning_via_buddy_intf;
#endif

};

#define mlme_set_scan_to_timer(mlme, ms) \
	do { \
 \
		_set_timer(&(mlme)->scan_to_timer, (ms)); \
	} while(0)

#define wl_mlme_set_auto_scan_int(wadptdata, ms) \
	do { \
		wadptdata->mlmepriv.auto_scan_int_ms = ms; \
	} while (0)

void wl_os_scan_finish_mark(PNIC Nic, bool aborted);

void do_reset_securitypriv(PNIC Nic, u8 tag);
extern int init_hostapd_mode(PNIC Nic, u8 tag);
extern void unload_hostapd_mode(PNIC Nic, u8 tag);

extern void do_joinbss_event_prehandle(PNIC Nic, u8 * pbuf, u8 tag);
extern void do_scan_event_callback(PNIC Nic, u8 * pbuf);
extern void do_scandone_event_callback(PNIC Nic, u8 * pbuf);
extern void do_joinbss_event_callback(PNIC Nic, u8 * pbuf);
extern void wl_stassoc_event_callback(PNIC Nic, u8 * pbuf);
extern void wl_stadel_event_callback(PNIC Nic, u8 * pbuf);
void rpt_sta_mstatus(PNIC Nic, u8 tag);
extern void wl_do_atimdone_event_cb(PNIC Nic, u8 * pbuf);
extern void do_wfprs_event_callback(PNIC Nic, u8 * pbuf);
extern void wl_wmm_event_callback(PNIC Nic, u8 * pbuf);
#ifdef CONFIG_IEEE80211W
void do_sta_timeout_event_callback(PNIC Nic, u8 * pbuf);
#endif

#ifdef CONFIG_EVENT_THREAD_MODE
thread_return event_thread(thread_context context);
#endif

extern void do_network_queue_unnew(PNIC Nic, u8 isfreeall, u8 tag);
extern int do_admin_priv_init(PNIC Nic, u8 tag);

extern void do_admin_priv_unnew(struct mlme_priv *pmlmepriv, u8 tag);

extern sint do_chose_and_join_from_scanlist(struct mlme_priv *pmlmepriv,
												   u8 tag);


extern sint wl_set_key(PNIC Nic, struct security_priv *psecuritypriv,
						sint keyid, u8 set_tx, bool enqueue, u8 tag);
extern sint do_set_auth_work(struct security_priv *psecuritypriv,
						 PNIC Nic);


__inline static u8 *get_bssid(struct mlme_priv *pmlmepriv)
{
	return pmlmepriv->cur_network.network.MacAddress;
}

__inline static sint check_fwstate(struct mlme_priv *pmlmepriv, sint state)
{
	if (pmlmepriv->fw_state & state)
		return _TRUE;

	return _FALSE;
}

__inline static sint get_fwstate(struct mlme_priv *pmlmepriv)
{
	return pmlmepriv->fw_state;
}

__inline static void set_fwstate(struct mlme_priv *pmlmepriv, sint state)
{
	pmlmepriv->fw_state |= state;
	if (_FW_UNDER_SURVEY == state) {
		pmlmepriv->bScanInProcess = _TRUE;
	}
}

__inline static void _clr_fwstate_(struct mlme_priv *pmlmepriv, sint state)
{
	pmlmepriv->fw_state &= ~state;
	if (_FW_UNDER_SURVEY == state) {
		pmlmepriv->bScanInProcess = _FALSE;
	}
}

__inline static void clr_fwstate(struct mlme_priv *pmlmepriv, sint state)
{
#if 0
	_irqL irqL;

	spin_lock_bh(&pmlmepriv->lock);
	_clr_fwstate_(pmlmepriv, state);
	spin_unlock_bh(&pmlmepriv->lock);
#endif
}

__inline static void up_scanned_network(struct mlme_priv *pmlmepriv)
{
#if 0
	_irqL irqL;

	spin_lock_bh(&pmlmepriv->lock);
	pmlmepriv->num_of_scanned++;
	spin_unlock_bh(&pmlmepriv->lock);
#endif
}

#ifdef CONFIG_CONCURRENT_MODE
sint wl_partner_wadptdata_up(PNIC Nic);
sint do_chk_partner_fwstate(PNIC Nic, sint state);
u8 do_query_partner_bBusyTraffic(PNIC Nic, u8 tag);
#endif

__inline static void down_scanned_network(struct mlme_priv *pmlmepriv)
{
#if 0
	_irqL irqL;

	spin_lock_bh(&pmlmepriv->lock);
	pmlmepriv->num_of_scanned--;
	spin_unlock_bh(&pmlmepriv->lock);
#endif
}

__inline static void fill_paraval(struct mlme_priv *pmlmepriv, sint val)
{
#if 0
	_irqL irqL;

	spin_lock_bh(&pmlmepriv->lock);
	pmlmepriv->num_of_scanned = val;
	spin_unlock_bh(&pmlmepriv->lock);
#endif
}

extern u16 do_query_capability(WLAN_BSSID_EX * bss);


extern void wl_disconnect_hdl_under_linked(PNIC Nic,
											struct sta_info *psta,
											u8 free_assoc);


extern void wl_make_random_ibss(u8 * pibss, u8 tag);

extern void do_assoc_resources_unnew(PNIC Nic, int lock_scanned_queue,
									 u8 tag);
extern void wl_disconnect_flag(PNIC Nic, u16 reason,
									u8 locally_generated, u8 tag);
extern void wl_connect_flag(PNIC Nic, u8 tag);

void wl_drv_scan_self(PNIC Nic, u8 tag);
void do_scan_finish(PNIC Nic, u32 type);

extern int do_rebuild_sec_ie(PNIC Nic, u8 * in_ie, u8 * out_ie,
							   uint in_len, u8 tag);

extern void do_rgp_dev_network_renew(PNIC Nic, u8 tag);

#ifdef CONFIG_SET_SCAN_DENY_TIMER
bool chk_scan_deny(PNIC Nic);
void do_clear_scan_deny(PNIC Nic);
void wl_scan_deny_timer_set_hdl(PNIC Nic);
void wl_scan_deny_set(PNIC Nic, u32 ms);
#else
#define chk_scan_deny(wadptdata) _FALSE
#define do_clear_scan_deny(wadptdata) do {} while (0)
#define wl_scan_deny_timer_set_hdl(wadptdata) do {} while (0)
#define wl_scan_deny_set(wadptdata, ms) do {} while (0)
#endif

void do_admin_priv_ie_data_unnew(struct mlme_priv *pmlmepriv, u8 tag);

#define MLME_BEACON_IE			0
#define MLME_PROBE_REQ_IE		1
#define MLME_PROBE_RESP_IE		2
#define MLME_GO_PROBE_RESP_IE	3
#define MLME_ASSOC_REQ_IE		4
#define MLME_ASSOC_RESP_IE		5

#define TIMESTAMPE 	0
#define BCN_INTERVAL 	1
#define CAPABILITY 	2


u8 *do_query_data_from_ie(u8 * ie, u8 type);

extern struct wlan_network *do_new_network(struct mlme_priv *pmlmepriv,
											  u8 tag);

sint do_chk_linked(PNIC Nic, u8 tag);

#ifdef CONFIG_80211N_HT
void wl_ht_use_default_setting(PNIC Nic, u8 tag);
void do_make_wmm_ie_ht(PNIC Nic, u8 * out_ie, uint * pout_len,
						 u8 tag);
unsigned int do_rebuild_ht_ie(PNIC Nic, u8 * in_ie, u8 * out_ie,
								   uint in_len, uint * pout_len, u8 channel,
								   u8 tag);

void wl_issue_addbareq_cmd(PNIC Nic, struct xmit_frame *pxmitframe,
							u8 tag);

void wl_do_append_exented_cap(PNIC Nic, u8 * out_ie, uint * pout_len,
							u8 tag);
#endif

int chk_same_network(WLAN_BSSID_EX * src, WLAN_BSSID_EX * dst, u8 feature,
					u8 tag);

#ifdef CONFIG_LAYER2_ROAMING
#define wl_roam_flags(wadptdata) ((wadptdata)->mlmepriv.roam_flags)
#define wl_chk_roam_flags(wadptdata, flags) ((wadptdata)->mlmepriv.roam_flags & flags)
#define wl_clr_roam_flags(wadptdata, flags) \
	do { \
		((wadptdata)->mlmepriv.roam_flags &= ~flags); \
	} while (0)

#define wl_set_roam_flags(wadptdata, flags) \
	do { \
		((wadptdata)->mlmepriv.roam_flags |= flags); \
	} while (0)

#define wl_assign_roam_flags(wadptdata, flags) \
	do { \
		((wadptdata)->mlmepriv.roam_flags = flags); \
	} while (0)

void wl_roaming_unlock(PNIC Nic, struct wlan_network *tgt_network);
void wl_roaming(PNIC Nic, struct wlan_network *tgt_network);
void do_set_to_roam(PNIC Nic, u8 to_roam);
u8 wl_dec_to_roam(PNIC Nic);
u8 wl_to_roam(PNIC Nic);
int do_chose_roaming_candidate(struct mlme_priv *pmlmepriv);
#else
#define wl_roam_flags(wadptdata) 0
#define wl_chk_roam_flags(wadptdata, flags) 0
#define wl_clr_roam_flags(wadptdata, flags) do {} while (0)
#define wl_set_roam_flags(wadptdata, flags) do {} while (0)
#define wl_assign_roam_flags(wadptdata, flags) do {} while (0)
#define wl_roaming_unlock(wadptdata, tgt_network) do {} while(0)
#define wl_roaming(wadptdata, tgt_network) do {} while(0)
#define do_set_to_roam(wadptdata, to_roam) do {} while(0)
#define wl_dec_to_roam(wadptdata) 0
#define wl_to_roam(wadptdata) 0
#define do_chose_roaming_candidate(mlme) _FAIL
#endif

bool do_adjust_chbw(PNIC Nic, u8 req_ch, u8 * req_bw,
					 u8 * req_offset, u8 tag);

struct sta_media_status_rpt_cmd_parm {
	struct sta_info *sta;
	bool connected;
};

void rpt_sta_media_status(PNIC Nic, struct sta_info *sta,
							  bool connected, u8 tag);
u8 rpt_sta_media_status_cmd(PNIC Nic, struct sta_info *sta,
								bool connected, u8 tag);


void rpt_sta_media_status_cmd_hdl(PNIC Nic,
									  struct sta_media_status_rpt_cmd_parm
									  *parm);

#define IPV4_SRC(_iphdr)			(((u8 *)(_iphdr)) + 12)
#define IPV4_DST(_iphdr)			(((u8 *)(_iphdr)) + 16)
#define GET_IPV4_IHL(_iphdr)		BE_BITS_TO_1BYTE(((u8 *)(_iphdr)) + 0, 0, 4)
#define GET_IPV4_PROTOCOL(_iphdr)	BE_BITS_TO_1BYTE(((u8 *)(_iphdr)) + 9, 0, 8)

#define GET_UDP_SRC(_udphdr)			BE_BITS_TO_2BYTE(((u8 *)(_udphdr)) + 0, 0, 16)
#define GET_UDP_DST(_udphdr)			BE_BITS_TO_2BYTE(((u8 *)(_udphdr)) + 2, 0, 16)

#define TCP_SRC(_tcphdr)				(((u8 *)(_tcphdr)) + 0)
#define TCP_DST(_tcphdr)				(((u8 *)(_tcphdr)) + 2)
#define GET_TCP_FIN(_tcphdr)			BE_BITS_TO_1BYTE(((u8 *)(_tcphdr)) + 13, 0, 1)
#define GET_TCP_SYN(_tcphdr)			BE_BITS_TO_1BYTE(((u8 *)(_tcphdr)) + 13, 1, 1)
#define GET_TCP_ACK(_tcphdr)			BE_BITS_TO_1BYTE(((u8 *)(_tcphdr)) + 13, 4, 1)

#endif
