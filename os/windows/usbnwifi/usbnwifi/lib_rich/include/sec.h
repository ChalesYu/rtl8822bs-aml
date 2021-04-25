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

#ifndef __WL_SECURITY_H_
#define __WL_SECURITY_H_

#define _NO_PRIVACY_		0x0
#define _WEP40_				0x1
#define _TKIP_				0x2
#define _TKIP_WTMIC_		0x3
#define _AES_				0x4
#define _WEP104_			0x5
#define _WEP_WPA_MIXED_	0x07
#define _SMS4_				0x06
#ifdef CONFIG_IEEE80211W
#define _BIP_				0x8
#endif
#define IEEE80211W_RIGHT_KEY	0x0
#define IEEE80211W_WRONG_KEY	0x1
#define IEEE80211W_NO_KEY		0x2

#define is_wep_enc(alg) (((alg) == _WEP40_) || ((alg) == _WEP104_))

const char *list_sec_type_str(u8 value);

#define _WPA_IE_ID_	0xdd
#define _WPA2_IE_ID_	0x30

typedef enum {
	ENCRYP_PROTOCOL_OPENSYS,
	ENCRYP_PROTOCOL_WEP,
	ENCRYP_PROTOCOL_WPA,
	ENCRYP_PROTOCOL_WPA2,
	ENCRYP_PROTOCOL_WAPI,
	ENCRYP_PROTOCOL_MAX
} ENCRYP_PROTOCOL_E;

#ifndef Ndis802_11AuthModeWPA2
#define Ndis802_11AuthModeWPA2 (Ndis802_11AuthModeWPANone + 1)
#endif

#ifndef Ndis802_11AuthModeWPA2PSK
#define Ndis802_11AuthModeWPA2PSK (Ndis802_11AuthModeWPANone + 2)
#endif

union pn48 {

	u64 val;
	struct {
		u8 TSC0;
		u8 TSC1;
		u8 TSC2;
		u8 TSC3;
		u8 TSC4;
		u8 TSC5;
		u8 TSC6;
		u8 TSC7;
	} _byte_;
};

union Keytype {
	u8 skey[16];
	u32 lkey[4];
};

typedef struct _WP_PMKID_LIST {
	u8 bUsed;
	u8 Bssid[6];
	u8 PMKID[16];
	u8 SsidBuf[33];
	u8 *ssid_octet;
	u16 ssid_length;
} WP_PMKID_LIST, *pWP_PMKID_LIST;

struct security_priv {
	u32 dot11AuthAlgrthm;                       /* 802.11 auth, could be open, shared, 8021x and authswitch */
	u32 dot11PrivacyAlgrthm;                    /* This specify the privacy for shared auth. algorithm. */

	u32 dot11PrivacyKeyIndex;                   /* this is only valid for legendary wep, 0~3 for key id. (tx key index) */
	union Keytype dot11DefKey[4];               /* this is only valid for def. key	 */
	u32 dot11DefKeylen[4];
	u8 key_mask;                                /* use to restore wep key after hal_init */

	u32 dot118021XGrpPrivacy;                   /* This specify the privacy algthm. used for Grp key */
	u32 dot118021XGrpKeyid;                     /* key id used for Grp Key ( tx key index) */
	union Keytype dot118021XGrpKey[4];
	union Keytype dot118021XGrptxmickey[4];
	union Keytype dot118021XGrprxmickey[4];
	union pn48 dot11Grptxpn;                    /* PN48 used for Grp Key xmit. */
	union pn48 dot11Grprxpn;                    /* PN48 used for Grp Key recv. */
#ifdef CONFIG_IEEE80211W
	u32 dot11wBIPKeyid;                         /* key id used for BIP Key ( tx key index) */
	union Keytype dot11wBIPKey[6];              /* BIP Key, for index4 and index5 */
	union pn48 dot11wBIPtxpn;                   /* PN48 used for BIP xmit. */
	union pn48 dot11wBIPrxpn;                   /* PN48 used for BIP recv. */
#endif
#ifdef CONFIG_AP_MODE
	unsigned int dot8021xalg;
	unsigned int wpa_psk;
	unsigned int wpa_group_cipher;
	unsigned int wpa2_group_cipher;
	unsigned int wpa_pairwise_cipher;
	unsigned int wpa2_pairwise_cipher;
#endif

	u8 wps_ie[MAX_WPS_IE_LEN];
	int wps_ie_len;

	u8 binstallGrpkey;
#ifdef CONFIG_IEEE80211W
	u8 binstallBIPkey;
#endif
	u8 busetkipkey;
	u8 bcheck_grpkey;
	u8 bgrpkey_handshake;

	s32 sw_encrypt;                             /* from registry_priv */
	s32 sw_decrypt;                             /* from registry_priv */

	s32 hw_decrypted;

	u32 ndisauthtype;
	u32 ndisencryptstatus;

	NDIS_802_11_WEP ndiswep;

	u8 assoc_info[600];
	u8 szofcapability[256];                     /* for wpa2 usage */
	u8 oidassociation[512];                     /* for wpa/wpa2 usage */
	u8 authenticator_ie[256];                   /* store ap security information element */
	u8 supplicant_ie[256];                      /* store sta security information element */

	u32 last_mic_err_time;
	u8 btkip_countermeasure;
	u8 btkip_wait_report;
	u32 btkip_countermeasure_time;

	WP_PMKID_LIST PMKIDList[NUM_PMKID_CACHE];
	u8 PMKIDIndex;

	u8 bWepDefaultKeyIdxSet;

};

#define GET_ENCRY_ALGO(psecuritypriv, psta, encry_algo, bmcst)\
do{\
	switch(psecuritypriv->dot11AuthAlgrthm)\
	{\
		case dot11AuthAlgrthm_Open:\
		case dot11AuthAlgrthm_Shared:\
		case dot11AuthAlgrthm_Auto:\
			encry_algo = (u8)psecuritypriv->dot11PrivacyAlgrthm;\
			break;\
		case dot11AuthAlgrthm_8021X:\
			if(bmcst)\
				encry_algo = (u8)psecuritypriv->dot118021XGrpPrivacy;\
			else\
				encry_algo =(u8) psta->dot118021XPrivacy;\
			break;\
	     case dot11AuthAlgrthm_WAPI:\
		     encry_algo = (u8)psecuritypriv->dot11PrivacyAlgrthm;\
		     break;\
	}\
}while(0)

#define _AES_IV_LEN_ 8

#define SET_ICE_IV_LEN( iv_len, icv_len, encrypt)\
do{\
	switch(encrypt)\
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

#define ROL32( A, n )	( ((A) << (n)) | ( ((A)>>(32-(n)))  & ( (1UL << (n)) - 1 ) ) )
#define ROR32( A, n ) 	ROL32( (A), 32-(n) )

struct mic_data {
	u32 K0, K1;
	u32 L, R;
	u32 M;
	u32 nBytesInM;
};

#define RORc(x, y) \
( ((((unsigned long) (x) & 0xFFFFFFFFUL) >> (unsigned long) ((y) & 31)) | \
   ((unsigned long) (x) << (unsigned long) (32 - ((y) & 31)))) & 0xFFFFFFFFUL)
#define S(x, n)         RORc((x), (n))
#define R(x, n)         (((x)&0xFFFFFFFFUL)>>(n))
#ifdef CONFIG_IEEE80211W
int omac1_aes_128(u8 * key, u8 * data, size_t data_len, u8 * mac);
#endif
void sl_secmicsetkey(struct mic_data *pmicdata, u8 * key);
void sl_secmicappend(struct mic_data *pmicdata, u8 * src, u32 nBytes);
void sl_secgetmic(struct mic_data *pmicdata, u8 * dst, u8 tag);

void sl_seccalctkipmic(u8 * data,
						u32 data_len,
						u8 * key, u8 * header, u8 * Miccode, u8 priority);

void sl_wep_encrypt(PNIC Nic, u8 * pxmitframe, u8 tag);

void sl_wep_decrypt(PNIC Nic, u8 * precvframe, u8 tag);
#ifdef CONFIG_IEEE80211W
u32 wl_BIP_verify(PNIC Nic, u8 * precvframe);
#endif

void sl_sec_restore_wep_key(PNIC Nic, u8 tag);
u8 sl_handle_tkip_countermeasure(PNIC Nic, const char *caller,
								  u8 tag);

#ifdef CONFIG_WOWLAN
u16 sl_calc_crc(u8 * pdata, int length);
#endif

#endif
