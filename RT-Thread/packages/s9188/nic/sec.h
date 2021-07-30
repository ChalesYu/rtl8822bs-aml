/*
 * sec.h
 *
 * This file contains all the prototypes for the sec.c file
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
#ifndef __WF_SECURITY_H_
#define __WF_SECURITY_H_

#define _NO_PRIVACY_        0x0
#define _WEP40_             0x1
#define _TKIP_              0x2
#define _TKIP_WTMIC_        0x3
#define _AES_               0x4
#define _WEP104_            0x5
#define _WEP_WPA_MIXED_     0x07
#define _SMS4_              0x06
#define IEEE80211W_RIGHT_KEY    0x0
#define IEEE80211W_WRONG_KEY    0x1
#define IEEE80211W_NO_KEY       0x2

#define is_wep_enc(alg) (((alg) == _WEP40_) || ((alg) == _WEP104_))

const char *list_sec_type_str(wf_u8 value);

#define _WPA_IE_ID_ 0xdd
#define _WPA2_IE_ID_    0x30

typedef enum
{
    ENCRYP_PROTOCOL_OPENSYS,
    ENCRYP_PROTOCOL_WEP,
    ENCRYP_PROTOCOL_WPA,
    ENCRYP_PROTOCOL_WPA2,
    ENCRYP_PROTOCOL_WAPI,
    ENCRYP_PROTOCOL_MAX
} ENCRYP_PROTOCOL_E;

#ifndef wf_ndis802_11AuthModeWPA2
#define wf_ndis802_11AuthModeWPA2 (wf_ndis802_11AuthModeWPANone + 1)
#endif

#ifndef wf_ndis802_11AuthModeWPA2PSK
#define wf_ndis802_11AuthModeWPA2PSK (wf_ndis802_11AuthModeWPANone + 2)
#endif

union pn48
{

    wf_u64 val;

#ifdef CONFIG_LITTLE_ENDIAN

    struct
    {
        wf_u8 TSC0;
        wf_u8 TSC1;
        wf_u8 TSC2;
        wf_u8 TSC3;
        wf_u8 TSC4;
        wf_u8 TSC5;
        wf_u8 TSC6;
        wf_u8 TSC7;
    } _byte_;

#elif defined(CONFIG_BIG_ENDIAN)

    struct
    {
        wf_u8 TSC7;
        wf_u8 TSC6;
        wf_u8 TSC5;
        wf_u8 TSC4;
        wf_u8 TSC3;
        wf_u8 TSC2;
        wf_u8 TSC1;
        wf_u8 TSC0;
    } _byte_;

#endif

};

struct arc4context
{
    wf_u32 x;
    wf_u32 y;
    wf_u8 state[256];
};

union Keytype
{
    wf_u8 skey[16];
    wf_u32 lkey[4];
};

typedef struct _SEC_PMKID_LIST
{
    wf_u8 bUsed;
    wf_u8 Bssid[6];
    wf_u8 PMKID[16];
    wf_u8 SsidBuf[33];
    wf_u8 *ssid_octet;
    wf_u16 ssid_length;
} SEC_PMKID_LIST, *pSEC_PMKID_LIST;

typedef struct sec_info
{
    wf_u32 dot11AuthAlgrthm;
    wf_u32 dot11PrivacyAlgrthm; /* for wep or wpa/wpa2 unicast privacy algrthm */

    wf_u32 dot11PrivacyKeyIndex;
    union Keytype dot11DefKey[4];
    wf_u32 dot11DefKeylen[4];
    wf_u8 key_mask;

    wf_u32 dot118021XGrpPrivacy;
    wf_u32 dot118021XGrpKeyid;
    union Keytype dot118021XGrpKey[4];
    union Keytype dot118021XGrptxmickey[4];
    union Keytype dot118021XGrprxmickey[4];
    union pn48 dot11Grptxpn;
    union pn48 dot11Grprxpn;
#ifdef CFG_ENABLE_AP_MODE
    wf_u32 wpa_psk;
    wf_u32 wpa_multicast_cipher;
    wf_u32 wpa_unicast_cipher;
    wf_u32 rsn_group_cipher;
    wf_u32 rsn_pairwise_cipher;
#endif

    wf_u8 wps_ie[WF_MAX_WPS_IE_LEN];
    int wps_ie_len;

    wf_u8 binstallGrpkey;
    wf_u8 busetkipkey;
    wf_u8 bcheck_grpkey;
    wf_u8 bgrpkey_handshake;

    wf_u32 ndisauthtype;
    wf_u32 ndisencryptstatus;

    wl_ndis_802_11_wep_st ndiswep;

    wf_bool wpa_enable;
    wf_bool rsn_enable;

    wf_u8 assoc_info[600];
    wf_u8 szofcapability[256];
    wf_u8 oidassociation[512];
    wf_u8 authenticator_ie[256];
    wf_u8 supplicant_ie[256];

    wf_u32 last_mic_err_time;
    wf_u8 btkip_countermeasure;
    wf_u8 btkip_wait_report;
    wf_u32 btkip_countermeasure_time;

    SEC_PMKID_LIST PMKIDList[WF_NUM_PMKID_CACHE];
    wf_u8 PMKIDIndex;

    wf_u8 bWepDefaultKeyIdxSet;

} sec_info_st, * sec_info_pt;

#define GET_ENCRY_ALGO(psec_info, pwdn, encry_algo, bmcst)\
do{\
    switch(psec_info->dot11AuthAlgrthm)\
    {\
        case dot11AuthAlgrthm_Open:\
        case dot11AuthAlgrthm_Shared:\
        case dot11AuthAlgrthm_Auto:\
            encry_algo = (wf_u8)psec_info->dot11PrivacyAlgrthm;\
            break;\
        case dot11AuthAlgrthm_8021X:\
            if(bmcst)\
                encry_algo = (wf_u8)psec_info->dot118021XGrpPrivacy;\
            else\
                encry_algo =(wf_u8) pwdn->dot118021XPrivacy;\
            break;\
         case dot11AuthAlgrthm_WAPI:\
             encry_algo = (wf_u8)psec_info->dot11PrivacyAlgrthm;\
             break;\
    }\
}while(0)

#define _AES_IV_LEN_ 8

#define SET_ICE_IV_LEN( iv_len, icv_len, encrypt_algo)\
do{\
    switch(encrypt_algo)\
    {\
        case _WEP40_:\
        case _WEP104_:\
            iv_len = 4;\
            icv_len = 4;\
            break;\
        case _TKIP_:\
            iv_len = 8;\
            icv_len = 4;\
            break;\
        case _AES_:\
            iv_len = 8;\
            icv_len = 8;\
            break;\
        case _SMS4_:\
            iv_len = 18;\
            icv_len = 16;\
            break;\
        default:\
            iv_len = 0;\
            icv_len = 0;\
            break;\
    }\
}while(0)

#define GET_TKIP_PN(iv, dot11txpn)                  \
do {                                    \
    dot11txpn._byte_.TSC0 = iv[2];                  \
    dot11txpn._byte_.TSC1 = iv[0];                  \
    dot11txpn._byte_.TSC2 = iv[4];                  \
    dot11txpn._byte_.TSC3 = iv[5];                  \
    dot11txpn._byte_.TSC4 = iv[6];                  \
    dot11txpn._byte_.TSC5 = iv[7];                  \
} while (0)

#define ROL32( A, n )   ( ((A) << (n)) | ( ((A)>>(32-(n)))  & ( (1UL << (n)) - 1 ) ) )
#define ROR32( A, n )   ROL32( (A), 32-(n) )

struct mic_data
{
    wf_u32 K0, K1;
    wf_u32 L, R;
    wf_u32 M;
    wf_u32 nBytesInM;
};

#define RORc(x, y) \
    (((((unsigned long) (x) & 0xFFFFFFFFUL) >> \
       (unsigned long) ((y) & 31)) | \
      ((unsigned long) (x) << (unsigned long) (32 - ((y) & 31)))) & \
     0xFFFFFFFFUL)

#define S(x, n)         RORc((x), (n))
#define R(x, n)         (((x)&0xFFFFFFFFUL)>>(n))

void wf_sec_mic_set_key(struct mic_data *pmicdata, wf_u8 * key);
void wf_sec_mic_append(struct mic_data *pmicdata, wf_u8 * src, wf_u32 nBytes);
void wf_sec_get_mic(struct mic_data *pmicdata, wf_u8 * dst);
int wf_wep_encrypt_auth (nic_info_st *pnic_info,
                         wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
int wf_wep_decrypt_auth (nic_info_st *pnic_info,
                         wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len);
int wf_sec_encrypt (void *pxmitframe, wf_u8 *pdata, wf_u32 len);
int wf_sec_decryptor (void *);
int wf_sec_info_init(nic_info_st *nic_info);
int wf_sec_info_term(nic_info_st *nic_info);

#endif
