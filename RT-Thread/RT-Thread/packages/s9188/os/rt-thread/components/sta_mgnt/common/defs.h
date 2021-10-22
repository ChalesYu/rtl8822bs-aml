/*
* WPA Supplicant - Common definitions
* Copyright (c) 2004-2015, Jouni Malinen <j@w1.fi>
*
* This software may be distributed under the terms of the BSD license.
* See README for more details.
*/

#ifndef DEFS_H
#define DEFS_H

#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
typedef enum { FALSE = 0, TRUE = 1 } Boolean;

#ifndef BIT
#define BIT(x) (1U << (x))
#endif



#define WPA_CIPHER_NONE BIT(0)
#define WPA_CIPHER_WEP40 BIT(1)
#define WPA_CIPHER_WEP104 BIT(2)
#define WPA_CIPHER_TKIP BIT(3)
#define WPA_CIPHER_CCMP BIT(4)
#define WPA_CIPHER_AES_128_CMAC BIT(5)
#define WPA_CIPHER_GCMP BIT(6)
#define WPA_CIPHER_SMS4 BIT(7)
#define WPA_CIPHER_GCMP_256 BIT(8)
#define WPA_CIPHER_CCMP_256 BIT(9)
#define WPA_CIPHER_BIP_GMAC_128 BIT(11)
#define WPA_CIPHER_BIP_GMAC_256 BIT(12)
#define WPA_CIPHER_BIP_CMAC_256 BIT(13)
#define WPA_CIPHER_GTK_NOT_USED BIT(14)

#define WPA_KEY_MGMT_IEEE8021X BIT(0)
#define WPA_KEY_MGMT_PSK BIT(1)
#define WPA_KEY_MGMT_NONE BIT(2)
#define WPA_KEY_MGMT_IEEE8021X_NO_WPA BIT(3)
#define WPA_KEY_MGMT_WPA_NONE BIT(4)
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

static inline int wpa_key_mgmt_wpa_ieee8021x(int akm)
{
  return !!(akm & (WPA_KEY_MGMT_IEEE8021X |
                   WPA_KEY_MGMT_FT_IEEE8021X |
                     WPA_KEY_MGMT_CCKM |
                       WPA_KEY_MGMT_OSEN |
			 WPA_KEY_MGMT_IEEE8021X_SHA256 |
                           WPA_KEY_MGMT_IEEE8021X_SUITE_B |
                             WPA_KEY_MGMT_IEEE8021X_SUITE_B_192));
}

static inline int wpa_key_mgmt_wpa_psk(int akm)
{
  return !!(akm & (WPA_KEY_MGMT_PSK |
                   WPA_KEY_MGMT_FT_PSK |
                     WPA_KEY_MGMT_PSK_SHA256 |
                       WPA_KEY_MGMT_SAE |
			 WPA_KEY_MGMT_FT_SAE));
}

static inline int wpa_key_mgmt_ft(int akm)
{
  return !!(akm & (WPA_KEY_MGMT_FT_PSK |
                   WPA_KEY_MGMT_FT_IEEE8021X |
                     WPA_KEY_MGMT_FT_SAE));
}

static inline int wpa_key_mgmt_sae(int akm)
{
  return !!(akm & (WPA_KEY_MGMT_SAE |
                   WPA_KEY_MGMT_FT_SAE));
}

static inline int wpa_key_mgmt_sha256(int akm)
{
  return !!(akm & (WPA_KEY_MGMT_PSK_SHA256 |
                   WPA_KEY_MGMT_IEEE8021X_SHA256 |
                     WPA_KEY_MGMT_OSEN |
                       WPA_KEY_MGMT_IEEE8021X_SUITE_B));
}

static inline int wpa_key_mgmt_sha384(int akm)
{
  return !!(akm & WPA_KEY_MGMT_IEEE8021X_SUITE_B_192);
}

static inline int wpa_key_mgmt_suite_b(int akm)
{
  return !!(akm & (WPA_KEY_MGMT_IEEE8021X_SUITE_B |
                   WPA_KEY_MGMT_IEEE8021X_SUITE_B_192));
}

static inline int wpa_key_mgmt_wpa(int akm)
{
  return wpa_key_mgmt_wpa_ieee8021x(akm) ||
    wpa_key_mgmt_wpa_psk(akm) ||
      wpa_key_mgmt_sae(akm);
}

static inline int wpa_key_mgmt_wpa_any(int akm)
{
  return wpa_key_mgmt_wpa(akm) || (akm & WPA_KEY_MGMT_WPA_NONE);
}

static inline int wpa_key_mgmt_cckm(int akm)
{
  return akm == WPA_KEY_MGMT_CCKM;
}


#define WPA_PROTO_WPA BIT(0)
#define WPA_PROTO_RSN BIT(1)
#define WPA_PROTO_WAPI BIT(2)
#define WPA_PROTO_OSEN BIT(3)

#define WPA_AUTH_ALG_OPEN BIT(0)
#define WPA_AUTH_ALG_SHARED BIT(1)
#define WPA_AUTH_ALG_LEAP BIT(2)
#define WPA_AUTH_ALG_FT BIT(3)
#define WPA_AUTH_ALG_SAE BIT(4)


enum wpa_alg {
  WPA_ALG_NONE,
  WPA_ALG_WEP,
  WPA_ALG_TKIP,
  WPA_ALG_CCMP,
  WPA_ALG_IGTK,
  WPA_ALG_PMK,
  WPA_ALG_GCMP,
  WPA_ALG_SMS4,
  WPA_ALG_KRK,
  WPA_ALG_GCMP_256,
  WPA_ALG_CCMP_256,
  WPA_ALG_BIP_GMAC_128,
  WPA_ALG_BIP_GMAC_256,
  WPA_ALG_BIP_CMAC_256
};

enum wpa_states {
  WPA_DISCONNECTED,
  WPA_INTERFACE_DISABLED,
  WPA_INACTIVE,
  WPA_SCANNING,
  WPA_AUTHENTICATING,
  WPA_ASSOCIATING,
  WPA_ASSOCIATED,
  WPA_4WAY_HANDSHAKE,
  WPA_GROUP_HANDSHAKE,
  WPA_COMPLETED
};

#define MLME_SETPROTECTION_PROTECT_TYPE_NONE 0
#define MLME_SETPROTECTION_PROTECT_TYPE_RX 1
#define MLME_SETPROTECTION_PROTECT_TYPE_TX 2
#define MLME_SETPROTECTION_PROTECT_TYPE_RX_TX 3

#define MLME_SETPROTECTION_KEY_TYPE_GROUP 0
#define MLME_SETPROTECTION_KEY_TYPE_PAIRWISE 1

enum mfp_options {
  NO_MGMT_FRAME_PROTECTION = 0,
  MGMT_FRAME_PROTECTION_OPTIONAL = 1,
  MGMT_FRAME_PROTECTION_REQUIRED = 2,
};
#define MGMT_FRAME_PROTECTION_DEFAULT 3

enum hostapd_hw_mode {
  HOSTAPD_MODE_IEEE80211B,
  HOSTAPD_MODE_IEEE80211G,
  HOSTAPD_MODE_IEEE80211A,
  HOSTAPD_MODE_IEEE80211AD,
  HOSTAPD_MODE_IEEE80211ANY,
  NUM_HOSTAPD_MODES
};

enum wpa_ctrl_req_type {
  WPA_CTRL_REQ_UNKNOWN,
  WPA_CTRL_REQ_EAP_IDENTITY,
  WPA_CTRL_REQ_EAP_PASSWORD,
  WPA_CTRL_REQ_EAP_NEW_PASSWORD,
  WPA_CTRL_REQ_EAP_PIN,
  WPA_CTRL_REQ_EAP_OTP,
  WPA_CTRL_REQ_EAP_PASSPHRASE,
  WPA_CTRL_REQ_SIM,
  WPA_CTRL_REQ_PSK_PASSPHRASE,
  WPA_CTRL_REQ_EXT_CERT_CHECK,
  NUM_WPA_CTRL_REQS
};

#define EAP_MAX_METHODS 8

enum mesh_plink_state {
  PLINK_IDLE = 1,
  PLINK_OPN_SNT,
  PLINK_OPN_RCVD,
  PLINK_CNF_RCVD,
  PLINK_ESTAB,
  PLINK_HOLDING,
  PLINK_BLOCKED,
};

enum set_band {
  WPA_SETBAND_AUTO,
  WPA_SETBAND_5G,
  WPA_SETBAND_2G
};

enum wpa_radio_work_band {
  BAND_2_4_GHZ = BIT(0),
  BAND_5_GHZ = BIT(1),
  BAND_60_GHZ = BIT(2),
};

#endif /* DEFS_H */
