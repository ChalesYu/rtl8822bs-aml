/*
 * wlan_mgmt.h
 *
 * This file contains all the prototypes for the wlan_mgmt.c file
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#ifndef __WLAN_MGMT_H__
#define __WLAN_MGMT_H__

/* macro */
#define WLAN_HDR_A3_QOS_LEN             26
#define WLAN_HDR_A3_LEN                 24
#define WLAN_BSS_IES_SIZE_MAX   WF_80211_IES_SIZE_MAX

/*
 * pscan_que_node   point to type of wf_wlan_mgmt_scan_que_node_t
 * pnic_info        point to type of nic_info_st
 * rst              point to type of wf_wlan_mgmt_scan_que_for_rst_e,
 *                  indicate result.
 */
#define wf_wlan_mgmt_param_chk(para, type)\
do\
{\
    type tmp;\
    (void)(&tmp == &para);\
}\
while (0)
#define _pscan_que(pnic_info)\
    (&((wf_wlan_mgmt_info_t *)(pnic_info)->wlan_mgmt_info)->scan_que)
#define wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)\
do\
{\
    wf_que_list_t *_pos = NULL, *_phead = NULL; pscan_que_node = NULL;\
    wf_wlan_mgmt_param_chk(pnic_info, nic_info_st *);\
    wf_wlan_mgmt_param_chk(pscan_que_node, wf_wlan_mgmt_scan_que_node_t *);\
    if ((pnic_info) && !WF_CANNOT_RUN(pnic_info) &&\
        !wf_wlan_mgmt_scan_que_read_try(_pscan_que(pnic_info)))\
    {\
        nic_info_st *_pnic_info = pnic_info;\
        _phead = wf_que_list_head(&_pscan_que(pnic_info)->ready);\
        wf_list_for_each(_pos, _phead)\
        {\
            /* get scan queue node point */\
            (pscan_que_node) =\
                (void *)wf_list_entry(_pos, wf_wlan_mgmt_scan_que_node_t, list);
/* { here include user code ... } */

#define wf_wlan_mgmt_scan_que_for_end(rst)\
            wf_wlan_mgmt_param_chk(rst, wf_wlan_mgmt_scan_que_for_rst_e);\
        }\
        wf_wlan_mgmt_scan_que_read_post(_pscan_que(_pnic_info));\
    }\
    rst = (_phead == NULL) ? WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL :\
          (_pos == _phead) ? WF_WLAN_MGMT_SCAN_QUE_FOR_RST_END :\
                             WF_WLAN_MGMT_SCAN_QUE_FOR_RST_BREAK;\
}\
while (0)

/* type define */
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
    WF_WLAN_BSS_NAME_IEEE80211_A,
    WF_WLAN_BSS_NAME_IEEE80211_B,
    WF_WLAN_BSS_NAME_IEEE80211_G,
    WF_WLAN_BSS_NAME_IEEE80211_BG,
    WF_WLAN_BSS_NAME_IEEE80211_AN,
    WF_WLAN_BSS_NAME_IEEE80211_BN,
    WF_WLAN_BSS_NAME_IEEE80211_GN,
    WF_WLAN_BSS_NAME_IEEE80211_BGN,
} wf_wlan_bss_name_e;
typedef enum
{
    WF_WLAN_OPR_MODE_ADHOC  = 1,
    WF_WLAN_OPR_MODE_MASTER = 3,
    WF_WLAN_OPR_MODE_MESH   = 7,
} wf_wlan_operation_mode_e;
typedef struct
{
    wf_que_list_t list;
    wf_que_t *parent;
    wf_u8 ttl; /* use to indcate this node time to live */
    wf_bool updated;

    unsigned long timestamp;
    wf_wlan_signal_strength_t signal_strength, signal_strength_scale;
    wf_wlan_signal_qual_t signal_qual;
    wf_80211_bssid_t bssid;
    wf_wlan_ssid_t ssid;
    wf_80211_hidden_ssid_e ssid_type;
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
    wf_u8 ies[WF_80211_IES_SIZE_MAX];
} wf_wlan_mgmt_scan_que_node_t;

typedef struct
{
    wf_que_t free, ready;
    wf_lock_t lock;
    wf_u8 read_cnt;
    wf_os_api_sema_t sema;
} wf_wlan_mgmt_scan_que_t;

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
    CHANNEL_WIDTH           bw;
    wf_u64                  timestamp;
    wf_u16                  bcn_interval;
    wf_u16                  cap_info;
    int                     aid;
    wf_u32                  ies_length;
    wf_u8                   rate_len;
    wf_u8                   rate[16];
    wf_u8                   short_slot;
    wf_u8                   ies[WF_80211_IES_SIZE_MAX];
    wf_u32                  join_res;
    wf_bool                 ht_enable;
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
    wf_80211_hidden_ssid_e  hidden_ssid_mode;
    wf_wlan_ssid_t          hidden_ssid;
#endif
} wf_wlan_network_t, wf_wlan_mgmt_cur_network_t;


#define WF_WLAN_NAME_SIZE_MAX   16
typedef struct
{
    void *tid;
    char name[WF_WLAN_NAME_SIZE_MAX];
    wf_wlan_mgmt_cur_network_t cur_network;
    wf_wlan_mgmt_scan_que_t scan_que;
    wf_msg_que_t msg_que;
} wf_wlan_mgmt_info_t;

enum
{
    /* priority level 0 */
    WF_WLAN_MGMT_TAG_UNINSTALL          = WF_MSG_TAG_SET(0, 0, 0),

    /* priority level 1 */
    WF_WLAN_MGMT_TAG_SCAN_QUE_FLUSH     = WF_MSG_TAG_SET(0, 1, 0),

    /* priority level 2 */
    WF_WLAN_MGMT_TAG_BEACON_FRAME       = WF_MSG_TAG_SET(0, 2, 0),
    WF_WLAN_MGMT_TAG_PROBERSP_FRAME,

    /* priority level 3 */
    WF_WLAN_MGMT_TAG_SCAN_QUE_REFRESH   = WF_MSG_TAG_SET(0, 3, 0),
};

typedef enum
{
    WF_WLAN_MGMT_SCAN_QUE_FOR_RST_END,
    WF_WLAN_MGMT_SCAN_QUE_FOR_RST_BREAK,
    WF_WLAN_MGMT_SCAN_QUE_FOR_RST_FAIL = -1,
} wf_wlan_mgmt_scan_que_for_rst_e;

/* function declaration */

int wf_wlan_mgmt_rx_frame (void *ptr);
int wf_wlan_mgmt_send_msg (nic_info_st *pnic_info, wf_msg_tag_t tag);
wf_inline static int wf_wlan_mgmt_scan_que_flush (nic_info_st *pnic_info)
{
    return wf_wlan_mgmt_send_msg(pnic_info, WF_WLAN_MGMT_TAG_SCAN_QUE_FLUSH);
}
int wf_wlan_mgmt_scan_que_refresh (nic_info_st *pnic_info,
                                   wf_u8 *pch, wf_u8 ch_num);
int wf_wlan_mgmt_init(nic_info_st *nic_info);
int wf_wlan_mgmt_term(nic_info_st *nic_info);
int wf_wlan_mgmt_scan_que_read_try (wf_wlan_mgmt_scan_que_t *pscan_que);
int wf_wlan_mgmt_scan_que_read_post (wf_wlan_mgmt_scan_que_t *pscan_que);
wf_inline static
wf_bool wf_wlan_is_same_ssid (wf_wlan_ssid_t *pssid1, wf_wlan_ssid_t *pssid2)
{
    return (wf_bool)(pssid1->length == pssid2->length &&
                     !wf_memcmp(pssid1->data, pssid2->data, pssid1->length));
}
void wf_wlan_set_cur_ssid(nic_info_st *pnic_info, wf_wlan_ssid_t *pssid);
wf_wlan_ssid_t *wf_wlan_get_cur_ssid(nic_info_st *pnic_info);
void wf_wlan_set_cur_bssid(nic_info_st *pnic_info,  wf_u8 *bssid);
wf_u8 *wf_wlan_get_cur_bssid(nic_info_st *pnic_info);
void wf_wlan_set_cur_channel(nic_info_st *pnic_info, wf_u8 channel);
wf_u8 wf_wlan_get_cur_channel(nic_info_st *pnic_info);
void wf_wlan_set_cur_bw(nic_info_st *pnic_info, CHANNEL_WIDTH bw);
CHANNEL_WIDTH wf_wlan_get_cur_bw(nic_info_st *pnic_info);
int wf_wlan_get_max_rate(nic_info_st *pnic_info,wf_u8 *mac,wf_u16 *max_rate);
int wf_wlan_get_signal_and_qual(nic_info_st *pnic_info,wf_u8 *qual, wf_u8 *level);


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
        *(wf_u16 *)(pbuf) &= cpu_to_le16(~(WF_BIT(3) | WF_BIT(2))); \
        *(wf_u16 *)(pbuf) |= cpu_to_le16(type); \
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

__inline static wf_bool IS_MCAST(unsigned char *da)
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

typedef enum
{
    wf_ndis802_11AuthModeOpen,
    wf_ndis802_11AuthModeShared,
    wf_ndis802_11AuthModeAutoSwitch,
    wf_ndis802_11AuthModeWPA,
    wf_ndis802_11AuthModeWPAPSK,
    wf_ndis802_11AuthModeWPANone,
    wf_ndis802_11AuthModeWAPI,
    wf_ndis802_11AuthModeMax   /*  Not a real mode, upper bound */
} wf_ndis_802_11_auth_mode_e;

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







#endif

