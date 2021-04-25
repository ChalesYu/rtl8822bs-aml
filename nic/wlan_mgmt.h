#ifndef __WLAN_MGMT_H__
#define __WLAN_MGMT_H__

#define WLAN_ETHHDR_LEN                 14
#define WLAN_ETHADDR_LEN                6
#define WLAN_IEEE_OUI_LEN               3
#define WLAN_ADDR_LEN                   6
#define WLAN_CRC_LEN                    4
#define WLAN_BSSID_LEN                  6
#define WLAN_BSS_TS_LEN                 8
#define WLAN_HDR_A3_LEN                 24
#define WLAN_HDR_A4_LEN                 30
#define WLAN_HDR_A3_QOS_LEN             26
#define WLAN_HDR_A4_QOS_LEN             32
#define WLAN_SSID_MAXLEN                32
#define WLAN_DATA_MAXLEN                2312

#define WLAN_CSCAN_HEADER               "CSCAN S\x01\x00\x00S\x00"
#define WLAN_CSCAN_HEADER_SIZE          12

#define BEACON_IE_OFFSET 				12
#define _VENDOR_SPECIFIC_IE_            221

#define WLAN_A3_PN_OFFSET               24
#define WLAN_A4_PN_OFFSET               30

#define WLAN_MIN_ETHFRM_LEN             60
#define WLAN_MAX_ETHFRM_LEN             1514
#define WLAN_ETHHDR_LEN                 14
#define WLAN_WMM_LEN                    24

#define WF_NUM_PRE_AUTH_KEY             16
#define WF_NUM_PMKID_CACHE              WF_NUM_PRE_AUTH_KEY

#define MLME_BEACON_IE          0
#define MLME_PROBE_REQ_IE       1
#define MLME_PROBE_RESP_IE      2
#define MLME_GO_PROBE_RESP_IE   3

#define TIMESTAMPE              0
#define BCN_INTERVAL            1
#define CAPABILITY              2

typedef enum
{
    WF_WLAN_BSSID_TYPE_BEACON   = WF_80211_FRM_BEACON,
    WF_WLAN_BSSID_TYPE_PROBERSP = WF_80211_FRM_PROBE_RESP,
    WF_WLAN_BSSID_TYPE_PROBEREQ = WF_80211_FRM_PROBE_REQ,
} wf_wlan_bss_frame_type_t;
typedef struct
{
    wf_u32 length;
    wf_80211_mgmt_ssid_t data;
} wf_wlan_ssid_t;
typedef wf_s8 wf_wlan_rssi_t;
typedef wf_u8 wf_wlan_signal_strength_t;
typedef wf_u8 wf_wlan_signal_qual_t;
typedef enum
{
    WF_WLAN_NETWORK_TYPE_11FH,
    WF_WLAN_NETWORK_TYPE_11DS,
    WF_WLAN_NETWORK_TYPE_11OFDM5,
    WF_WLAN_NETWORK_TYPE_11OFDM24,
    WF_WLAN_NETWORK_TYPE_MAX, /*  dummy upper bound */
} wf_wlan_bss_network_type_e;
typedef struct
{
    wf_u32 length;     /*  Length of structure */
    wf_u32 hop_pattern;/*  As defined by 802.11, MSB set */
    wf_u32 hop_set;    /*  to one if non-802.11 */
    wf_u32 dwell_time; /*  units are Kusec */
} wf_wlan_config_fh_t;
typedef struct
{
    wf_u32 length;        /*  Length of structure */
    wf_u32 beacon_period; /*  units are Kusec */
    wf_u32 atim_window;   /*  units are Kusec */
    wf_u32 ds;     /*  Frequency, units are kHz */
    wf_wlan_config_fh_t fh;
} wf_wlan_bss_config_t;
typedef enum
{
    WF_WLAN_OPR_MODE_ADHOC = 1,
    WF_WLAN_OPR_MODE_MASTER = 3,
    WF_WLAN_OPR_MODE_MESH = 7,
} wf_wlan_operation_mode_e;
#define WLAN_BSS_IES_SIZE_MAX   WF_80211_IES_SIZE_MAX
#define WLAN_CUSTOM_LEN_MAX     64
typedef struct
{
    wf_u32 length; /* The length, in bytes, consist of the wf_wlan_bss_info_t */
    wf_80211_addr_t mac_addr;
    wf_80211_bssid_t bssid;
    wf_wlan_bss_frame_type_t frame_type;
    wf_wlan_ssid_t ssid;
    wf_u32 privacy;
    wf_wlan_rssi_t rssi;
    wf_wlan_bss_network_type_e network_type;
    wf_wlan_bss_config_t config;
    wf_wlan_operation_mode_e mode;
    wf_u8 spot_rate[16];
    wf_u32 ies_length;
    wf_u8 ies[WLAN_BSS_IES_SIZE_MAX];
} wf_wlan_bss_info_t;


typedef struct
{
    wf_wlan_signal_strength_t signal_strength;
    wf_wlan_signal_qual_t signal_quality;
    wf_u8 optimum_antenna;
    wf_u8 reserved_0;
} wf_wlan_phy_info_t;

typedef enum
{
    WF_WLAN_SEC_AUTHMODEOPEN,
    WF_WLAN_SEC_AUTHMODESHARED,
    WF_WLAN_SEC_AUTHMODEAUTOSWITCH,
    WF_WLAN_SEC_AUTHMODEWPA,
    WF_WLAN_SEC_AUTHMODEWPAPSK,
    WF_WLAN_SEC_AUTHMODEWPANONE,
    WF_WLAN_SEC_AUTHMODEWAPI,
    WF_WLAN_SEC_AUTHMODEMAX /*  Not a real mode, upper bound */
} wf_wlan_security_auth_mode_e;
typedef struct
{
    wf_wlan_security_auth_mode_e auth_mode;
    wf_u32 ndisauthtype;
    wf_u8 *wps_probe_req_ie;
    wf_u32 wps_probe_req_ie_len;
} wf_wlan_security_t;

typedef struct
{
    /*
     * below for rmote is AP
     */
    int network_type;
    int fixed;
    unsigned long timestamp;
    int aid;
    wf_wlan_security_t security;
    wf_wlan_phy_info_t phy;
    wf_u16 bw_mode;
    wf_wlan_bss_info_t bss;
    wf_u8 timeout_count;

    /*
     * below for rmote is STA
     */
    wf_list_t sta_list;
} wf_wlan_remote_t;


typedef enum
{
    ENC_PROT_OPEN,
    ENC_PROT_WEP,
    ENC_PROT_WPA,
    ENC_PROT_WAP2,
    ENC_PROT_WAPI,
} wf_wlan_encryp_protocol_e;
typedef enum
{
    WF_WLAN_SCAN_PASSIVE,
    WF_WLAN_SCAN_ACTIVE,
} wf_wlan_scan_type_t;


typedef struct
{
    wf_list_t list;
    wf_u8 data[0];
} wf_wlan_queue_node_t;
typedef struct
{
    wf_list_t head;
    wf_lock_spin lock;
    wf_u8 count;
} wf_wlan_queue_t;


typedef enum
{
    WF_WLAN_BSS_NAME_IEEE80211_A,
    WF_WLAN_BSS_NAME_IEEE80211_B,
    WF_WLAN_BSS_NAME_IEEE80211_G,
    WF_WLAN_BSS_NAME_IEEE80211_BG,
    WF_WLAN_BSS_NAME_IEEE80211_AN,
    WF_WLAN_BSS_NAME_IEEE80211_BN,
    WF_WLAN_BSS_NAME_IEEE80211_GN,
    WF_WLAN_BSS_NAME_IEEE80211_BGN,
} wf_wlan_bss_name_e;
typedef struct
{
    unsigned long timestamp;
    wf_wlan_signal_strength_t signal_strength, signal_strength_scale;
    wf_wlan_signal_qual_t signal_qual;
    wf_80211_bssid_t bssid;
    wf_wlan_ssid_t ssid;
    wf_u8 channel;
    wf_u8 spot_rate[16];
    wf_wlan_operation_mode_e opr_mode;
    wf_bool privacy;
    wf_wlan_bss_name_e name;
    wf_bool ht_spot;
    wf_bool bw_40mhz;
    wf_bool short_gi;
    wf_u16 mcs;
    wf_u8 wpa_ie[64];
    wf_u8 wps_ie[64];
    wf_u8 rsn_ie[64];
    wf_u64 ie_len;
    wf_80211_frame_e frame_type;
    wf_u8 ies[WLAN_BSS_IES_SIZE_MAX];
} wf_wlan_scanned_info_t;

typedef struct
{
    wf_wlan_queue_t free;
    wf_wlan_queue_t que;
    wf_lock_spin    lock;
    wf_u8           count; /* the count of user read scanned_info */
    wf_os_api_sema_t         sema; /* used for Mutex */
} wf_wlan_scanned_t;


typedef struct
{
    wf_wlan_queue_t beacon;
    wf_wlan_queue_t probersp;
} wf_wlan_rx_frame_t;

typedef enum
{
    WF_WLAN_MSG_TAG_BEACON,
    WF_WLAN_MSG_TAG_PROBEREQ,
    WF_WLAN_MSG_TAG_PROBERSP,
    WF_WLAN_MSG_TAG_NTFY_SCANNED_FLUSH,
    WF_WLAN_MSG_TAG_NTFY_UNINSTALL,
} wf_wlan_msg_tag_e;
typedef struct
{
    wf_wlan_msg_tag_e tag;
    wf_u16 len;
    wf_u8 value[0];
} wf_wlan_msg_t;
typedef struct
{
    wf_wlan_queue_t que;
    wf_os_api_sema_t sema;
} wf_wlan_msg_que_t;

typedef struct
{
    wf_u8 encryp_protocol;
    int group_cipher;
    int pairwise_cipher;
    int is_8021x;

    wf_u16 ht_cap_info;
    wf_u8 ht_info_infos_0;
} wf_wlan_bcn_info_t;

typedef struct
{
    wf_u8 ACI;
    wf_u8 ECW;
    wf_u16 TXOP_limit;
} wf_wmm_ac_st;

typedef struct
{
    wf_u16 OUI;
    wf_u8  OUI_pandding;
    wf_u8  type_sub;
    wf_u8  version;
    wf_u8  qos_info;
    wf_u8 reserved;
    wf_wmm_ac_st ac[4];
} wf_wmm_para_st;

typedef struct
{
    wf_u16 cap_info;
    wf_u8  ampdu_params_info;
    wf_u8  supp_mcs_set[WF_MCS_NUM];
    wf_u16 extended_ht_cap_info;
    wf_u32 tx_BF_cap_info;
    wf_u8  antenna_selection_info;
} wf_wlan_ht_cap_info_st;

typedef struct
{
    wf_u8 primary_channel;
    wf_u8 infos[5];
    wf_u8 MCS_rate[16];
} wf_wlan_ht_op_info_st;

typedef struct
{
    wf_wlan_rssi_t          rssi;
    wf_80211_addr_t         mac_addr;
    wf_wlan_ssid_t          ssid;
    wf_80211_bssid_t        bssid;
    wf_u8                   channel;
    wf_u8                   bw;
    wf_u64                  timestamp;
    wf_u16                  bcn_interval;
    wf_u16                  cap_info;
    int                     aid;
    wf_u32                  ies_length;
    wf_u8                   rate_len;
    wf_u8                   rate[16];
    wf_u8                   short_slot;
    wf_u8                   ies[WLAN_BSS_IES_SIZE_MAX];
    wf_u32                  join_res;
	wf_bool 				ht_enable;
//	#ifdef CFG_ENABLE_ADHOC_MODE	
//    wf_bool                 is_ibss,
//	wf_bool 				join_to;
//	#endif
	
	struct
	{
	    wf_u8 ie[WF_OFFSETOF(wf_80211_mgmt_t, assoc_req.listen_interval) +
	             WF_80211_IES_SIZE_MAX];
	    wf_u32 ie_len;
	} assoc_req;
	struct
	{
	    wf_u8 ie[WF_80211_IES_SIZE_MAX];
	    wf_u32 ie_len;
	} assoc_resp;

#ifdef CFG_ENABLE_AP_MODE
    void                   *ap_tid;
    char                    ap_name[30];
    wf_80211_wmm_param_ie_t pwmm;
    wf_80211_mgmt_ht_cap_t  pht_cap;
    wf_80211_mgmt_ht_operation_t pht_oper;
    wf_u8                   cur_wireless_mode;
    wf_u8                   channle_offset;
    wf_que_t                ap_msg_free[WF_AP_MSG_TAG_MAX];
    wf_ap_status            ap_state;
	wf_u8 					hidden_ssid_mode;
    wf_wlan_ssid_t          hidden_ssid;
#endif
} wf_wlan_network_t;

typedef struct
{
    struct
    {
        wf_u8 signal_strength;
        wf_u8 signal_qual;
        wf_s8 rssi;
        wf_bool valid_flag;
    } phy_sta;
    union
    {
        wf_u8 frame[0];
        wf_80211_mgmt_t mgmt;
    };
} msg_frame_t;


#define WF_WLAN_NAME_SIZE_MAX           16
#define WF_DIRCT_CMD        BIT0
typedef struct
{
    void *tid;
    char name[WF_WLAN_NAME_SIZE_MAX];

    wf_wlan_remote_t remote; /* remote contain AP or STA information */
    wf_wlan_network_t cur_network;

    wf_wlan_scanned_t scanned;
    wf_wlan_rx_frame_t rx_frame;
    wf_wlan_queue_t notify;
    wf_wlan_msg_que_t msg_que;
} wf_wlan_info_t;

int wf_wlan_mgmt_rx_frame (void *);
int wf_wlan_init(nic_info_st *nic_info);
int wf_notify_send (nic_info_st *pnic_info, wf_wlan_msg_tag_e tag);
int wf_wlan_term(nic_info_st *nic_info);
int wf_wlan_scanned_acce_try (wf_wlan_scanned_t *pscanned);
int wf_wlan_scanned_acce_post (wf_wlan_scanned_t *pscanned);

int msg_que_send (wf_wlan_info_t *pwlan_info, wf_wlan_queue_node_t *pnode);




static wf_inline wf_bool wf_wlan_is_uninstalling (nic_info_st *pnic_info)
{
    return (pnic_info->is_driver_stopped == wf_true ||
            pnic_info->is_surprise_removed == wf_true);
}

/*
 * premote      point to type of wf_wlan_remote_t
 * pnode        point to type of wf_wlan_queue_node_t
 * pnic_info    point to type of nic_info_st
 * pret         point to type of int, reture the scanned result
 *              -1 fail
 *               1 break
 *               0 success
 */
#define WF_WLAN_SCANNED_EACH_RET_END        0
#define WF_WLAN_SCANNED_EACH_RET_BREAK      1
#define WF_WLAN_SCANNED_EACH_RET_FAIL      -1
#define pscanned_(pnic_info)\
    (&((wf_wlan_info_t *)(pnic_info)->wlan_info)->scanned)
#define pnode_ _pnode
#define wf_wlan_scanned_each_begin(pscanned_info, pnic_info)\
{\
    wf_list_t *_pcur = NULL, *_phead = NULL;\
    if ((pnic_info) && !wf_wlan_is_uninstalling(pnic_info) &&\
                       !wf_wlan_scanned_acce_try(pscanned_(pnic_info)))\
    {\
        _phead = &pscanned_(pnic_info)->que.head;\
        wf_list_for_each(_pcur, _phead)\
        {\
             wf_wlan_queue_node_t *_pnode;\
             _pnode = (void *)wf_list_entry(_pcur, wf_wlan_queue_node_t, list);\
             (pscanned_info) = (wf_wlan_scanned_info_t *)_pnode->data;
             /* { here contain user code } */
#define wf_wlan_scanned_each_end(pnic_info, pret)\
        }\
        wf_wlan_scanned_acce_post(pscanned_(pnic_info));\
    }\
    *pret = (_phead == NULL) ? WF_WLAN_SCANNED_EACH_RET_FAIL :\
            (_pcur == _phead) ? WF_WLAN_SCANNED_EACH_RET_END :\
            WF_WLAN_SCANNED_EACH_RET_BREAK;\
}
















enum WIFI_FRAME_TYPE
{
    WIFI_MGT_TYPE = (0),
    WIFI_CTRL_TYPE = (WF_BIT(2)),
    WIFI_DATA_TYPE = (WF_BIT(3)),
    WIFI_QOS_DATA_TYPE = (WF_BIT(7) | WF_BIT(3)),
};

enum WIFI_FRAME_SUBTYPE
{

    WIFI_ASSOCREQ = (0 | WIFI_MGT_TYPE),
    WIFI_ASSOCRSP = (WF_BIT(4) | WIFI_MGT_TYPE),
    WIFI_REASSOCREQ = (WF_BIT(5) | WIFI_MGT_TYPE),
    WIFI_REASSOCRSP = (WF_BIT(5) | WF_BIT(4) | WIFI_MGT_TYPE),
    WIFI_PROBEREQ = (WF_BIT(6) | WIFI_MGT_TYPE),
    WIFI_PROBERSP = (WF_BIT(6) | WF_BIT(4) | WIFI_MGT_TYPE),
    WIFI_BEACON = (WF_BIT(7) | WIFI_MGT_TYPE),
    WIFI_ATIM = (WF_BIT(7) | WF_BIT(4) | WIFI_MGT_TYPE),
    WIFI_DISASSOC = (WF_BIT(7) | WF_BIT(5) | WIFI_MGT_TYPE),
    WIFI_AUTH = (WF_BIT(7) | WF_BIT(5) | WF_BIT(4) | WIFI_MGT_TYPE),
    WIFI_DEAUTH = (WF_BIT(7) | WF_BIT(6) | WIFI_MGT_TYPE),
    WIFI_ACTION = (WF_BIT(7) | WF_BIT(6) | WF_BIT(4) | WIFI_MGT_TYPE),
    WIFI_ACTION_NOACK = (WF_BIT(7) | WF_BIT(6) | WF_BIT(5) | WIFI_MGT_TYPE),

    WIFI_NDPA = (WF_BIT(6) | WF_BIT(4) | WIFI_CTRL_TYPE),
    WIFI_PSPOLL = (WF_BIT(7) | WF_BIT(5) | WIFI_CTRL_TYPE),
    WIFI_RTS = (WF_BIT(7) | WF_BIT(5) | WF_BIT(4) | WIFI_CTRL_TYPE),
    WIFI_CTS = (WF_BIT(7) | WF_BIT(6) | WIFI_CTRL_TYPE),
    WIFI_ACK = (WF_BIT(7) | WF_BIT(6) | WF_BIT(4) | WIFI_CTRL_TYPE),
    WIFI_CFEND = (WF_BIT(7) | WF_BIT(6) | WF_BIT(5) | WIFI_CTRL_TYPE),
    WIFI_CFEND_CFACK = (WF_BIT(7) | WF_BIT(6) | WF_BIT(5) | WF_BIT(4) | WIFI_CTRL_TYPE),

    WIFI_DATA = (0 | WIFI_DATA_TYPE),
    WIFI_DATA_CFACK = (WF_BIT(4) | WIFI_DATA_TYPE),
    WIFI_DATA_CFPOLL = (WF_BIT(5) | WIFI_DATA_TYPE),
    WIFI_DATA_CFACKPOLL = (WF_BIT(5) | WF_BIT(4) | WIFI_DATA_TYPE),
    WIFI_DATA_NULL = (WF_BIT(6) | WIFI_DATA_TYPE),
    WIFI_CF_ACK = (WF_BIT(6) | WF_BIT(4) | WIFI_DATA_TYPE),
    WIFI_CF_POLL = (WF_BIT(6) | WF_BIT(5) | WIFI_DATA_TYPE),
    WIFI_CF_ACKPOLL = (WF_BIT(6) | WF_BIT(5) | WF_BIT(4) | WIFI_DATA_TYPE),
    WIFI_QOS_DATA_NULL = (WF_BIT(6) | WIFI_QOS_DATA_TYPE),
};

#define WF_NUM_PRE_AUTH_KEY         16
#define WF_NUM_PMKID_CACHE          WF_NUM_PRE_AUTH_KEY

typedef enum
{
    dot11AuthAlgrthm_Open = 0,
    dot11AuthAlgrthm_Shared,
    dot11AuthAlgrthm_8021X,
    dot11AuthAlgrthm_Auto,
    dot11AuthAlgrthm_WAPI,
    dot11AuthAlgrthm_MaxNum
} auth_algo_e;

#define _TO_DS_     WF_BIT(8)
#define _FROM_DS_   WF_BIT(9)
#define _MORE_FRAG_ WF_BIT(10)
#define _RETRY_     WF_BIT(11)
#define _PWRMGT_    WF_BIT(12)
#define _MORE_DATA_ WF_BIT(13)
#define _PRIVACY_   WF_BIT(14)
#define _ORDER_         WF_BIT(15)


#define SetToDs(pbuf)   \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16(_TO_DS_); \
    } while(0)

#define GetToDs(pbuf)   (((*(wf_u16 *)(pbuf)) & le16_to_cpu(_TO_DS_)) != 0)

#define ClearToDs(pbuf) \
    do  {   \
        *(wf_u16 *)(pbuf) &= (~cpu_to_le16(_TO_DS_)); \
    } while(0)

#define SetFrDs(pbuf)   \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16(_FROM_DS_); \
    } while(0)

#define GetFrDs(pbuf)   (((*(wf_u16 *)(pbuf)) & le16_to_cpu(_FROM_DS_)) != 0)

#define ClearFrDs(pbuf) \
    do  {   \
        *(wf_u16 *)(pbuf) &= (~cpu_to_le16(_FROM_DS_)); \
    } while(0)

#define get_tofr_ds(pframe) ((GetToDs(pframe) << 1) | GetFrDs(pframe))

#define SetMFrag(pbuf)  \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16(_MORE_FRAG_); \
    } while(0)

#define GetMFrag(pbuf)  (((*(wf_u16 *)(pbuf)) & le16_to_cpu(_MORE_FRAG_)) != 0)

#define ClearMFrag(pbuf)    \
    do  {   \
        *(wf_u16 *)(pbuf) &= (~cpu_to_le16(_MORE_FRAG_)); \
    } while(0)

#define SetRetry(pbuf)  \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16(_RETRY_); \
    } while(0)

#define GetRetry(pbuf)  (((*(wf_u16 *)(pbuf)) & le16_to_cpu(_RETRY_)) != 0)

#define ClearRetry(pbuf)    \
    do  {   \
        *(wf_u16 *)(pbuf) &= (~cpu_to_le16(_RETRY_)); \
    } while(0)

#define SetPwrMgt(pbuf) \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16(_PWRMGT_); \
    } while(0)

#define GetPwrMgt(pbuf) (((*(wf_u16 *)(pbuf)) & le16_to_cpu(_PWRMGT_)) != 0)

#define ClearPwrMgt(pbuf)   \
    do  {   \
        *(wf_u16 *)(pbuf) &= (~cpu_to_le16(_PWRMGT_)); \
    } while(0)

#define SetMData(pbuf)  \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16(_MORE_DATA_); \
    } while(0)

#define GetMData(pbuf)  (((*(wf_u16 *)(pbuf)) & le16_to_cpu(_MORE_DATA_)) != 0)

#define ClearMData(pbuf)    \
    do  {   \
        *(wf_u16 *)(pbuf) &= (~cpu_to_le16(_MORE_DATA_)); \
    } while(0)

#define SetPrivacy(pbuf)    \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16(_PRIVACY_); \
    } while(0)

#define GetPrivacy(pbuf)    (((*(wf_u16 *)(pbuf)) & le16_to_cpu(_PRIVACY_)) != 0)

#define ClearPrivacy(pbuf)  \
    do  {   \
        *(wf_u16 *)(pbuf) &= (~cpu_to_le16(_PRIVACY_)); \
    } while(0)

#define GetOrder(pbuf)  (((*(wf_u16 *)(pbuf)) & le16_to_cpu(_ORDER_)) != 0)

#define GetFrameType(pbuf)  (le16_to_cpu(*(wf_u16 *)(pbuf)) & (WF_BIT(3) | WF_BIT(2)))

#define SetFrameType(pbuf,type) \
    do {    \
        *(wf_u16 *)(pbuf) &= __constant_cpu_to_le16(~(WF_BIT(3) | WF_BIT(2))); \
        *(wf_u16 *)(pbuf) |= __constant_cpu_to_le16(type); \
    } while(0)

#define GetFrameSubType(pbuf)   (cpu_to_le16(*(wf_u16 *)(pbuf)) & (WF_BIT(7) | WF_BIT(6) | WF_BIT(5) | WF_BIT(4) | WF_BIT(3) | WF_BIT(2)))

#define SetFrameSubType(pbuf,type) \
    do {    \
        *(wf_u16 *)(pbuf) &= cpu_to_le16(~(WF_BIT(7) | WF_BIT(6) | WF_BIT(5) | WF_BIT(4) | WF_BIT(3) | WF_BIT(2))); \
        *(wf_u16 *)(pbuf) |= cpu_to_le16(type); \
    } while(0)

#define GetSequence(pbuf)   (cpu_to_le16(*(wf_u16 *)((SIZE_PTR)(pbuf) + 22)) >> 4)

#define GetFragNum(pbuf)    (cpu_to_le16(*(wf_u16 *)((SIZE_PTR)(pbuf) + 22)) & 0x0f)

#define GetTupleCache(pbuf) (cpu_to_le16(*(wf_u16 *)((SIZE_PTR)(pbuf) + 22)))

#define SetFragNum(pbuf, num) \
    do {    \
        *(wf_u16 *)((SIZE_PTR)(pbuf) + 22) = \
            ((*(wf_u16 *)((SIZE_PTR)(pbuf) + 22)) & le16_to_cpu(~(0x000f))) | \
            cpu_to_le16(0x0f & (num));     \
    } while(0)

#define SetSeqNum(pbuf, num) \
    do {    \
        *(wf_u16 *)((SIZE_PTR)(pbuf) + 22) = \
            ((*(wf_u16 *)((SIZE_PTR)(pbuf) + 22)) & le16_to_cpu((wf_u16)~0xfff0)) | \
            le16_to_cpu((wf_u16)(0xfff0 & (num << 4))); \
    } while(0)

#define SetDuration(pbuf, dur) \
    do {    \
        *(wf_u16 *)((SIZE_PTR)(pbuf) + 2) = cpu_to_le16(0xffff & (dur)); \
    } while(0)

#define SetPriority(pbuf, tid)  \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16(tid & 0xf); \
    } while(0)

#define GetPriority(pbuf)   ((le16_to_cpu(*(wf_u16 *)(pbuf))) & 0xf)

#define SetEOSP(pbuf, eosp) \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16( (eosp & 1) << 4); \
    } while(0)

#define SetAckpolicy(pbuf, ack) \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16( (ack & 3) << 5); \
    } while(0)

#define GetAckpolicy(pbuf) (((le16_to_cpu(*(wf_u16 *)pbuf)) >> 5) & 0x3)

#define GetAMsdu(pbuf) (((le16_to_cpu(*(wf_u16 *)pbuf)) >> 7) & 0x1)

#define SetAMsdu(pbuf, amsdu)   \
    do  {   \
        *(wf_u16 *)(pbuf) |= cpu_to_le16( (amsdu & 1) << 7); \
    } while(0)

#define GetAid(pbuf)    (cpu_to_le16(*(wf_u16 *)((SIZE_PTR)(pbuf) + 2)) & 0x3fff)

#define GetTid(pbuf)    (cpu_to_le16(*(wf_u16 *)((SIZE_PTR)(pbuf) + (((GetToDs(pbuf)<<1)|GetFrDs(pbuf))==3?30:24))) & 0x000f)

#define GetAddr1Ptr(pbuf)   ((unsigned char *)((SIZE_PTR)(pbuf) + 4))

#define GetAddr2Ptr(pbuf)   ((unsigned char *)((SIZE_PTR)(pbuf) + 10))

#define GetAddr3Ptr(pbuf)   ((unsigned char *)((SIZE_PTR)(pbuf) + 16))

#define GetAddr4Ptr(pbuf)   ((unsigned char *)((SIZE_PTR)(pbuf) + 24))

#define MacAddr_isBcst(addr) \
( \
    ( (addr[0] == 0xff) && (addr[1] == 0xff) && \
        (addr[2] == 0xff) && (addr[3] == 0xff) && \
        (addr[4] == 0xff) && (addr[5] == 0xff) )  ? wf_true : wf_false \
)

__inline static int IS_MCAST(unsigned char *da)
{
    if ((*da) & 0x01)
        return wf_true;
    else
        return wf_false;
}

__inline static unsigned char *get_ra(unsigned char *pframe)
{
    unsigned char *ra;
    ra = GetAddr1Ptr(pframe);
    return ra;
}

__inline static unsigned char *get_ta(unsigned char *pframe)
{
    unsigned char *ta;
    ta = GetAddr2Ptr(pframe);
    return ta;
}

__inline static unsigned char *get_da(unsigned char *pframe)
{
    unsigned char *da;
    unsigned int to_fr_ds = (GetToDs(pframe) << 1) | GetFrDs(pframe);

    switch (to_fr_ds)
    {
        case 0x00:
            da = GetAddr1Ptr(pframe);
            break;
        case 0x01:
            da = GetAddr1Ptr(pframe);
            break;
        case 0x02:
            da = GetAddr3Ptr(pframe);
            break;
        default:
            da = GetAddr3Ptr(pframe);
            break;
    }

    return da;
}

__inline static unsigned char *get_sa(unsigned char *pframe)
{
    unsigned char *sa;
    unsigned int to_fr_ds = (GetToDs(pframe) << 1) | GetFrDs(pframe);

    switch (to_fr_ds)
    {
        case 0x00:
            sa = GetAddr2Ptr(pframe);
            break;
        case 0x01:
            sa = GetAddr3Ptr(pframe);
            break;
        case 0x02:
            sa = GetAddr2Ptr(pframe);
            break;
        default:
            sa = GetAddr4Ptr(pframe);
            break;
    }

    return sa;
}

__inline static unsigned char *get_hdr_bssid(unsigned char *pframe)
{
    unsigned char *sa = NULL;
    unsigned int to_fr_ds = (GetToDs(pframe) << 1) | GetFrDs(pframe);

    switch (to_fr_ds)
    {
        case 0x00:
            sa = GetAddr3Ptr(pframe);
            break;
        case 0x01:
            sa = GetAddr2Ptr(pframe);
            break;
        case 0x02:
            sa = GetAddr1Ptr(pframe);
            break;
        case 0x03:
            sa = GetAddr1Ptr(pframe);
            break;
    }

    return sa;
}

__inline static int IsFrameTypeCtrl(unsigned char *pframe)
{
    if (WIFI_CTRL_TYPE == GetFrameType(pframe))
        return wf_true;
    else
        return wf_false;
}


typedef wf_u8 wlan_ieee80211_mac_address_st[6];

typedef struct wl_ndis_802_11_wep
{
    wf_u32 Length;
    wf_u32 KeyIndex;
    wf_u32 KeyLength;
    wf_u8  KeyMaterial[16];
} wl_ndis_802_11_wep_st;

typedef struct wl_ndis_auth_mode
{
    wf_u32 Length;
    wf_80211_bssid_t Bssid;
    wf_u32 Flags;
} wl_ndis_auth_mode_st;
/*
 * Length is the 4 bytes multiples of the sume of
 *  [ETH_ALEN] + 2 + sizeof (struct ndis_802_11_ssid) + sizeof (wf_u32)
 *  + sizeof (NDIS_802_11_RSSI) + sizeof (enum NDIS_802_11_NETWORK_TYPE)
 *  + sizeof (struct ndis_802_11_config)
 *  + NDIS_802_11_LENGTH_RATES_EX + IELength
 *
 * Except the IELength, all other fields are fixed length.
 * Therefore, we can define a macro to represent the partial sum. */

enum wf_ndis_802_11_auth_mode
{
    wf_ndis802_11AuthModeOpen,
    wf_ndis802_11AuthModeShared,
    wf_ndis802_11AuthModeAutoSwitch,
    wf_ndis802_11AuthModeWPA,
    wf_ndis802_11AuthModeWPAPSK,
    wf_ndis802_11AuthModeWPANone,
    wf_ndis802_11AuthModeWAPI,
    wf_ndis802_11AuthModeMax   /*  Not a real mode, upper bound */
};

typedef enum
{
    wf_ndis802_11WEPEnabled,
    wf_ndis802_11Encryption1Enabled = wf_ndis802_11WEPEnabled,
    wf_ndis802_11WEPDisabled,
    wf_ndis802_11EncryptionDisabled = wf_ndis802_11WEPDisabled,
    wf_ndis802_11WEPKeyAbsent,
    wf_ndis802_11Encryption1KeyAbsent = wf_ndis802_11WEPKeyAbsent,
    wf_ndis802_11WEPNotSupported,
    wf_ndis802_11EncryptionNotSupported = wf_ndis802_11WEPNotSupported,
    wf_ndis802_11Encryption2Enabled,
    wf_ndis802_11Encryption2KeyAbsent,
    wf_ndis802_11Encryption3Enabled,
    wf_ndis802_11Encryption3KeyAbsent,
    wf_ndis802_11_EncrypteionWAPI
} ndis_802_11_wep_status_e;

typedef struct wlan_ieee80211_ssid_st_
{
    wf_u32 length;
    wf_u8 ssid[32];
} wlan_ieee80211_ssid_st;






int wf_wlanMgmt_init(nic_info_st *nic_info);
int wf_wlanMgmt_frame_parse(nic_info_st *nic_info, char *data);
int wf_wlanMgmt_term(nic_info_st *nic_info);

void wf_wlan_set_cur_ssid(nic_info_st *nic_info, wf_wlan_ssid_t *ssid);
wf_wlan_ssid_t *wf_wlan_get_cur_ssid(nic_info_st *nic_info);

void wf_wlan_set_cur_bssid(nic_info_st *nic_info,  wf_u8 *bssid);
wf_u8 *wf_wlan_get_cur_bssid(nic_info_st *nic_info);

void wf_wlan_set_cur_channel(nic_info_st *nic_info, wf_u8 channel);
wf_u8 wf_wlan_get_cur_channel(nic_info_st *nic_info);

void wf_wlan_set_cur_bw(nic_info_st *nic_info, CHANNEL_WIDTH bw);
CHANNEL_WIDTH wf_wlan_get_cur_bw(nic_info_st *nic_info);
void wf_wlan_ap_term(nic_info_st *pnic_info);

int wf_wlan_get_max_rate(nic_info_st *pnic_info,wf_u8 *mac,wf_u16 *max_rate);

int wf_wlan_get_signal_and_qual(nic_info_st *pnic_info,wf_u8 *qual, wf_u8 *level);

#endif

