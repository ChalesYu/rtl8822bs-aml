
#ifndef __WF_WIFI_H__
#define __WF_WIFI_H__

/* includes */
#include "common.h"

/* macro */
#define NIC_PIRV_ALIGN_SIZE     wf_align_up(sizeof(nic_info_st), sizeof(wf_u64))

/* type define */

#define WF_WIFI_DEV_SSID_MAX_LENGTH     32 /* SSID MAX LEN */
typedef struct
{
    wf_u8 len;
    wf_u8 data[WF_WIFI_DEV_SSID_MAX_LENGTH + 1];
} wf_wifi_ssid_t;

#define SHARED_ENABLED  0x00008000
#define WPA_SECURITY    0x00200000
#define WPA2_SECURITY   0x00400000
#define WPS_ENABLED     0x10000000
#define WEP_ENABLED     0x0001
#define TKIP_ENABLED    0x0002
#define AES_ENABLED     0x0004
#define WSEC_SWFLAG     0x0008
typedef enum
{
    WF_WIFI_SEC_OPEN            = 0,                                            /* Open security                           */
    WF_WIFI_SEC_WEP_PSK         = WEP_ENABLED,                                  /* WEP Security with open authentication   */
    WF_WIFI_SEC_WEP_SHARED      = WEP_ENABLED   | SHARED_ENABLED,               /* WEP Security with shared authentication */
    WF_WIFI_SEC_WPA_TKIP_PSK    = WPA_SECURITY  | TKIP_ENABLED,                 /* WPA Security with TKIP                  */
    WF_WIFI_SEC_WPA_AES_PSK     = WPA_SECURITY  | AES_ENABLED,                  /* WPA Security with AES                   */
    WF_WIFI_SEC_WPA_MIXED_PSK   = WPA_SECURITY  | AES_ENABLED   | TKIP_ENABLED, /* WPA Security with AES & TKIP           */
    WF_WIFI_SEC_WPA2_AES_PSK    = WPA2_SECURITY | AES_ENABLED,                  /* WPA2 Security with AES                  */
    WF_WIFI_SEC_WPA2_TKIP_PSK   = WPA2_SECURITY | TKIP_ENABLED,                 /* WPA2 Security with TKIP                 */
    WF_WIFI_SEC_WPA2_MIXED_PSK  = WPA2_SECURITY | AES_ENABLED   | TKIP_ENABLED, /* WPA2 Security with AES & TKIP           */
    WF_WIFI_SEC_WPS_OPEN        = WPS_ENABLED,                                  /* WPS with open security                  */
    WF_WIFI_SEC_WPS_SECURE      = WPS_ENABLED   | AES_ENABLED,                  /* WPS with AES security                   */
    WF_WIFI_SEC_UNKNOWN         = -1,                                           /* May be returned by scan function if security is unknown.
                                                                                   Do not pass this to the join function! */
} wf_wifi_sec_t;

typedef enum
{
    WF_WLAN_BAND_5GHZ       = 0,            /* Denotes 5GHz radio band   */
    WF_WLAN_BAND_2_4GHZ     = 1,            /* Denotes 2.4GHz radio band */
    WF_WLAN_BAND_UNKNOWN    = 0x7fffffff,   /* unknown */
} wf_wifi_band_t;

typedef enum
{
    WF_WIFI_SCAN_TYPE_PASSIVE,
    WF_WIFI_SCAN_TYPE_ACTIVE,

    WF_WIFI_SCAN_TYPE_MAX,
} wf_wifi_scan_type_e;

#define WF_WIFI_SCAN_SSID_NUM_MAX       1
#define WF_WIFI_SCAN_CHANNEL_NUM_MAX    13
typedef struct
{
    wf_wifi_scan_type_e type;
    wf_wifi_ssid_t ssids[WF_WIFI_SCAN_SSID_NUM_MAX];
    wf_u8 ssid_num;
    wf_u8 chs[WF_WIFI_SCAN_CHANNEL_NUM_MAX];
    wf_u8 chs_num;
} wf_wifi_scan_info_t;

typedef struct
{
    /* ssid */
    wf_wifi_ssid_t ssid;
    /* ssid hidden */
    wf_bool hidden;
    /* hwaddr */
    wf_u8 bssid[6];
    /* security type */
    wf_wifi_sec_t sec;
    /* radio channel */
    wf_u8 channel;
    /* maximal data rate */
    wf_u32 datarate;
    /* 2.4G/5G */
    wf_wifi_band_t band;
    /* signal strength */
    wf_s8 rssi;
} wf_wifi_scan_report_t;

#define WF_WIFI_DEV_PASSWORD_MAX_LENGTH     32 /* PASSWORD MAX LEN*/
typedef struct
{
    wf_u8 len;
    wf_u8 data[WF_WIFI_DEV_PASSWORD_MAX_LENGTH + 1];
} wf_wifi_key_t;

typedef struct
{
    wf_wifi_ssid_t ssid;
    wf_wifi_key_t key;
    wf_u8 bssid[6];
    wf_u8 channel;
    wf_wifi_sec_t sec;
} wf_wifi_conn_info_t;

typedef struct
{
    wf_wifi_ssid_t ssid;
    wf_wifi_key_t key;
    wf_u8 channel;
    wf_wifi_sec_t sec;
    wf_bool hidden;
} wf_wifi_ap_info_t;

typedef enum
{
    WF_WIFI_MODE_AUTO       = 0, /* Let the driver decides */
    WF_WIFI_MODE_ADHOC      = 1, /* Single cell network */
    WF_WIFI_MODE_INFRA      = 2, /* Multi cell network, roaming, ... */
    WF_WIFI_MODE_MASTER     = 3, /* Synchronisation master or Access Point */
    WF_WIFI_MODE_MONITOR    = 6, /* Passive monitor (listen only) */
} wf_wifi_mode_t;

typedef enum
{
    WF_WIFI_COUNTRY_AFGHANISTAN,
    WF_WIFI_COUNTRY_ALBANIA,
    WF_WIFI_COUNTRY_ALGERIA,
    WF_WIFI_COUNTRY_AMERICAN_SAMOA,
    WF_WIFI_COUNTRY_ANGOLA,
    WF_WIFI_COUNTRY_ANGUILLA,
    WF_WIFI_COUNTRY_ANTIGUA_AND_BARBUDA,
    WF_WIFI_COUNTRY_ARGENTINA,
    WF_WIFI_COUNTRY_ARMENIA,
    WF_WIFI_COUNTRY_ARUBA,
    WF_WIFI_COUNTRY_AUSTRALIA,
    WF_WIFI_COUNTRY_AUSTRIA,
    WF_WIFI_COUNTRY_AZERBAIJAN,
    WF_WIFI_COUNTRY_BAHAMAS,
    WF_WIFI_COUNTRY_BAHRAIN,
    WF_WIFI_COUNTRY_BAKER_ISLAND,
    WF_WIFI_COUNTRY_BANGLADESH,
    WF_WIFI_COUNTRY_BARBADOS,
    WF_WIFI_COUNTRY_BELARUS,
    WF_WIFI_COUNTRY_BELGIUM,
    WF_WIFI_COUNTRY_BELIZE,
    WF_WIFI_COUNTRY_BENIN,
    WF_WIFI_COUNTRY_BERMUDA,
    WF_WIFI_COUNTRY_BHUTAN,
    WF_WIFI_COUNTRY_BOLIVIA,
    WF_WIFI_COUNTRY_BOSNIA_AND_HERZEGOVINA,
    WF_WIFI_COUNTRY_BOTSWANA,
    WF_WIFI_COUNTRY_BRAZIL,
    WF_WIFI_COUNTRY_BRITISH_INDIAN_OCEAN_TERRITORY,
    WF_WIFI_COUNTRY_BRUNEI_DARUSSALAM,
    WF_WIFI_COUNTRY_BULGARIA,
    WF_WIFI_COUNTRY_BURKINA_FASO,
    WF_WIFI_COUNTRY_BURUNDI,
    WF_WIFI_COUNTRY_CAMBODIA,
    WF_WIFI_COUNTRY_CAMEROON,
    WF_WIFI_COUNTRY_CANADA,
    WF_WIFI_COUNTRY_CAPE_VERDE,
    WF_WIFI_COUNTRY_CAYMAN_ISLANDS,
    WF_WIFI_COUNTRY_CENTRAL_AFRICAN_REPUBLIC,
    WF_WIFI_COUNTRY_CHAD,
    WF_WIFI_COUNTRY_CHILE,
    WF_WIFI_COUNTRY_CHINA,
    WF_WIFI_COUNTRY_CHRISTMAS_ISLAND,
    WF_WIFI_COUNTRY_COLOMBIA,
    WF_WIFI_COUNTRY_COMOROS,
    WF_WIFI_COUNTRY_CONGO,
    WF_WIFI_COUNTRY_CONGO_THE_DEMOCRATIC_REPUBLIC_OF_THE,
    WF_WIFI_COUNTRY_COSTA_RICA,
    WF_WIFI_COUNTRY_COTE_DIVOIRE,
    WF_WIFI_COUNTRY_CROATIA,
    WF_WIFI_COUNTRY_CUBA,
    WF_WIFI_COUNTRY_CYPRUS,
    WF_WIFI_COUNTRY_CZECH_REPUBLIC,
    WF_WIFI_COUNTRY_DENMARK,
    WF_WIFI_COUNTRY_DJIBOUTI,
    WF_WIFI_COUNTRY_DOMINICA,
    WF_WIFI_COUNTRY_DOMINICAN_REPUBLIC,
    WF_WIFI_COUNTRY_DOWN_UNDER,
    WF_WIFI_COUNTRY_ECUADOR,
    WF_WIFI_COUNTRY_EGYPT,
    WF_WIFI_COUNTRY_EL_SALVADOR,
    WF_WIFI_COUNTRY_EQUATORIAL_GUINEA,
    WF_WIFI_COUNTRY_ERITREA,
    WF_WIFI_COUNTRY_ESTONIA,
    WF_WIFI_COUNTRY_ETHIOPIA,
    WF_WIFI_COUNTRY_FALKLAND_ISLANDS_MALVINAS,
    WF_WIFI_COUNTRY_FAROE_ISLANDS,
    WF_WIFI_COUNTRY_FIJI,
    WF_WIFI_COUNTRY_FINLAND,
    WF_WIFI_COUNTRY_FRANCE,
    WF_WIFI_COUNTRY_FRENCH_GUINA,
    WF_WIFI_COUNTRY_FRENCH_POLYNESIA,
    WF_WIFI_COUNTRY_FRENCH_SOUTHERN_TERRITORIES,
    WF_WIFI_COUNTRY_GABON,
    WF_WIFI_COUNTRY_GAMBIA,
    WF_WIFI_COUNTRY_GEORGIA,
    WF_WIFI_COUNTRY_GERMANY,
    WF_WIFI_COUNTRY_GHANA,
    WF_WIFI_COUNTRY_GIBRALTAR,
    WF_WIFI_COUNTRY_GREECE,
    WF_WIFI_COUNTRY_GRENADA,
    WF_WIFI_COUNTRY_GUADELOUPE,
    WF_WIFI_COUNTRY_GUAM,
    WF_WIFI_COUNTRY_GUATEMALA,
    WF_WIFI_COUNTRY_GUERNSEY,
    WF_WIFI_COUNTRY_GUINEA,
    WF_WIFI_COUNTRY_GUINEA_BISSAU,
    WF_WIFI_COUNTRY_GUYANA,
    WF_WIFI_COUNTRY_HAITI,
    WF_WIFI_COUNTRY_HOLY_SEE_VATICAN_CITY_STATE,
    WF_WIFI_COUNTRY_HONDURAS,
    WF_WIFI_COUNTRY_HONG_KONG,
    WF_WIFI_COUNTRY_HUNGARY,
    WF_WIFI_COUNTRY_ICELAND,
    WF_WIFI_COUNTRY_INDIA,
    WF_WIFI_COUNTRY_INDONESIA,
    WF_WIFI_COUNTRY_IRAN_ISLAMIC_REPUBLIC_OF,
    WF_WIFI_COUNTRY_IRAQ,
    WF_WIFI_COUNTRY_IRELAND,
    WF_WIFI_COUNTRY_ISRAEL,
    WF_WIFI_COUNTRY_ITALY,
    WF_WIFI_COUNTRY_JAMAICA,
    WF_WIFI_COUNTRY_JAPAN,
    WF_WIFI_COUNTRY_JERSEY,
    WF_WIFI_COUNTRY_JORDAN,
    WF_WIFI_COUNTRY_KAZAKHSTAN,
    WF_WIFI_COUNTRY_KENYA,
    WF_WIFI_COUNTRY_KIRIBATI,
    WF_WIFI_COUNTRY_KOREA_REPUBLIC_OF,
    WF_WIFI_COUNTRY_KOSOVO,
    WF_WIFI_COUNTRY_KUWAIT,
    WF_WIFI_COUNTRY_KYRGYZSTAN,
    WF_WIFI_COUNTRY_LAO_PEOPLES_DEMOCRATIC_REPUBIC,
    WF_WIFI_COUNTRY_LATVIA,
    WF_WIFI_COUNTRY_LEBANON,
    WF_WIFI_COUNTRY_LESOTHO,
    WF_WIFI_COUNTRY_LIBERIA,
    WF_WIFI_COUNTRY_LIBYAN_ARAB_JAMAHIRIYA,
    WF_WIFI_COUNTRY_LIECHTENSTEIN,
    WF_WIFI_COUNTRY_LITHUANIA,
    WF_WIFI_COUNTRY_LUXEMBOURG,
    WF_WIFI_COUNTRY_MACAO,
    WF_WIFI_COUNTRY_MACEDONIA_FORMER_YUGOSLAV_REPUBLIC_OF,
    WF_WIFI_COUNTRY_MADAGASCAR,
    WF_WIFI_COUNTRY_MALAWI,
    WF_WIFI_COUNTRY_MALAYSIA,
    WF_WIFI_COUNTRY_MALDIVES,
    WF_WIFI_COUNTRY_MALI,
    WF_WIFI_COUNTRY_MALTA,
    WF_WIFI_COUNTRY_MAN_ISLE_OF,
    WF_WIFI_COUNTRY_MARTINIQUE,
    WF_WIFI_COUNTRY_MAURITANIA,
    WF_WIFI_COUNTRY_MAURITIUS,
    WF_WIFI_COUNTRY_MAYOTTE,
    WF_WIFI_COUNTRY_MEXICO,
    WF_WIFI_COUNTRY_MICRONESIA_FEDERATED_STATES_OF,
    WF_WIFI_COUNTRY_MOLDOVA_REPUBLIC_OF,
    WF_WIFI_COUNTRY_MONACO,
    WF_WIFI_COUNTRY_MONGOLIA,
    WF_WIFI_COUNTRY_MONTENEGRO,
    WF_WIFI_COUNTRY_MONTSERRAT,
    WF_WIFI_COUNTRY_MOROCCO,
    WF_WIFI_COUNTRY_MOZAMBIQUE,
    WF_WIFI_COUNTRY_MYANMAR,
    WF_WIFI_COUNTRY_NAMIBIA,
    WF_WIFI_COUNTRY_NAURU,
    WF_WIFI_COUNTRY_NEPAL,
    WF_WIFI_COUNTRY_NETHERLANDS,
    WF_WIFI_COUNTRY_NETHERLANDS_ANTILLES,
    WF_WIFI_COUNTRY_NEW_CALEDONIA,
    WF_WIFI_COUNTRY_NEW_ZEALAND,
    WF_WIFI_COUNTRY_NICARAGUA,
    WF_WIFI_COUNTRY_NIGER,
    WF_WIFI_COUNTRY_NIGERIA,
    WF_WIFI_COUNTRY_NORFOLK_ISLAND,
    WF_WIFI_COUNTRY_NORTHERN_MARIANA_ISLANDS,
    WF_WIFI_COUNTRY_NORWAY,
    WF_WIFI_COUNTRY_OMAN,
    WF_WIFI_COUNTRY_PAKISTAN,
    WF_WIFI_COUNTRY_PALAU,
    WF_WIFI_COUNTRY_PANAMA,
    WF_WIFI_COUNTRY_PAPUA_NEW_GUINEA,
    WF_WIFI_COUNTRY_PARAGUAY,
    WF_WIFI_COUNTRY_PERU,
    WF_WIFI_COUNTRY_PHILIPPINES,
    WF_WIFI_COUNTRY_POLAND,
    WF_WIFI_COUNTRY_PORTUGAL,
    WF_WIFI_COUNTRY_PUETO_RICO,
    WF_WIFI_COUNTRY_QATAR,
    WF_WIFI_COUNTRY_REUNION,
    WF_WIFI_COUNTRY_ROMANIA,
    WF_WIFI_COUNTRY_RUSSIAN_FEDERATION,
    WF_WIFI_COUNTRY_RWANDA,
    WF_WIFI_COUNTRY_SAINT_KITTS_AND_NEVIS,
    WF_WIFI_COUNTRY_SAINT_LUCIA,
    WF_WIFI_COUNTRY_SAINT_PIERRE_AND_MIQUELON,
    WF_WIFI_COUNTRY_SAINT_VINCENT_AND_THE_GRENADINES,
    WF_WIFI_COUNTRY_SAMOA,
    WF_WIFI_COUNTRY_SANIT_MARTIN_SINT_MARTEEN,
    WF_WIFI_COUNTRY_SAO_TOME_AND_PRINCIPE,
    WF_WIFI_COUNTRY_SAUDI_ARABIA,
    WF_WIFI_COUNTRY_SENEGAL,
    WF_WIFI_COUNTRY_SERBIA,
    WF_WIFI_COUNTRY_SEYCHELLES,
    WF_WIFI_COUNTRY_SIERRA_LEONE,
    WF_WIFI_COUNTRY_SINGAPORE,
    WF_WIFI_COUNTRY_SLOVAKIA,
    WF_WIFI_COUNTRY_SLOVENIA,
    WF_WIFI_COUNTRY_SOLOMON_ISLANDS,
    WF_WIFI_COUNTRY_SOMALIA,
    WF_WIFI_COUNTRY_SOUTH_AFRICA,
    WF_WIFI_COUNTRY_SPAIN,
    WF_WIFI_COUNTRY_SRI_LANKA,
    WF_WIFI_COUNTRY_SURINAME,
    WF_WIFI_COUNTRY_SWAZILAND,
    WF_WIFI_COUNTRY_SWEDEN,
    WF_WIFI_COUNTRY_SWITZERLAND,
    WF_WIFI_COUNTRY_SYRIAN_ARAB_REPUBLIC,
    WF_WIFI_COUNTRY_TAIWAN_PROVINCE_OF_CHINA,
    WF_WIFI_COUNTRY_TAJIKISTAN,
    WF_WIFI_COUNTRY_TANZANIA_UNITED_REPUBLIC_OF,
    WF_WIFI_COUNTRY_THAILAND,
    WF_WIFI_COUNTRY_TOGO,
    WF_WIFI_COUNTRY_TONGA,
    WF_WIFI_COUNTRY_TRINIDAD_AND_TOBAGO,
    WF_WIFI_COUNTRY_TUNISIA,
    WF_WIFI_COUNTRY_TURKEY,
    WF_WIFI_COUNTRY_TURKMENISTAN,
    WF_WIFI_COUNTRY_TURKS_AND_CAICOS_ISLANDS,
    WF_WIFI_COUNTRY_TUVALU,
    WF_WIFI_COUNTRY_UGANDA,
    WF_WIFI_COUNTRY_UKRAINE,
    WF_WIFI_COUNTRY_UNITED_ARAB_EMIRATES,
    WF_WIFI_COUNTRY_UNITED_KINGDOM,
    WF_WIFI_COUNTRY_UNITED_STATES,
    WF_WIFI_COUNTRY_UNITED_STATES_REV4,
    WF_WIFI_COUNTRY_UNITED_STATES_NO_DFS,
    WF_WIFI_COUNTRY_UNITED_STATES_MINOR_OUTLYING_ISLANDS,
    WF_WIFI_COUNTRY_URUGUAY,
    WF_WIFI_COUNTRY_UZBEKISTAN,
    WF_WIFI_COUNTRY_VANUATU,
    WF_WIFI_COUNTRY_VENEZUELA,
    WF_WIFI_COUNTRY_VIET_NAM,
    WF_WIFI_COUNTRY_VIRGIN_ISLANDS_BRITISH,
    WF_WIFI_COUNTRY_VIRGIN_ISLANDS_US,
    WF_WIFI_COUNTRY_WALLIS_AND_FUTUNA,
    WF_WIFI_COUNTRY_WEST_BANK,
    WF_WIFI_COUNTRY_WESTERN_SAHARA,
    WF_WIFI_COUNTRY_WORLD_WIDE_XX,
    WF_WIFI_COUNTRY_YEMEN,
    WF_WIFI_COUNTRY_ZAMBIA,
    WF_WIFI_COUNTRY_ZIMBABWE,
    WF_WIFI_COUNTRY_UNKNOWN
} wf_wifi_country_code_t;

typedef void *wf_wifi_hd_t;
typedef void *wf_wifi_skb_hd_t;
typedef struct
{
    int (*netif_register) (wf_wifi_hd_t wifi_hd);
    int (*netif_unregister) (wf_wifi_hd_t wifi_hd);

//    netif_carr_on
//    netif_carr_off

    /* for station */
    int (*scan_report) (wf_wifi_hd_t wifi_hd, wf_wifi_scan_report_t *scan_rpo);
    int (*scan_done) (wf_wifi_hd_t wifi_hd);

    int (*conn_done) (wf_wifi_hd_t wifi_hd, wf_wifi_conn_info_t *conn_info);
//    disconn_evt
    int (*disconn_evt) (wf_wifi_hd_t wifi_hd);

    /* for ap */
    int (*assoc_evt) (wdn_net_info_st *pwdn_info);
    int (*disassoc_evt) (wdn_net_info_st *pwdn_info);

    /* skb */
    wf_wifi_skb_hd_t (*skb_create) (wf_wifi_hd_t wifi_hd, void *pdata, wf_u32 plen,
                                    void **skb_data, wf_u32 *skb_len);
    int (*skb_upload) (wf_wifi_hd_t wifi_hd, wf_wifi_skb_hd_t skb);
    int (*skb_free) (wf_wifi_hd_t wifi_hd, wf_wifi_skb_hd_t skb);
} wf_wifi_ops_t;
typedef struct
{
    void *parent; /* point to nic */
    const wf_wifi_ops_t *ops;
    wf_workqueue_mgnt_t tx_wq;
} nic_priv_t;

/* function declaration */

static wf_inline nic_info_st *nic_alloc (void)
{
    return wf_kzalloc(NIC_PIRV_ALIGN_SIZE + sizeof(nic_priv_t));
}
int nic_priv_init (nic_info_st *pnic_info);
static wf_inline nic_priv_t *nic_priv (nic_info_st *pnic_info)
{
    return (nic_priv_t *)((char *)pnic_info + NIC_PIRV_ALIGN_SIZE);
}

static wf_inline void *wf_wifi_priv (wf_wifi_hd_t wifi_hd)
{
    nic_info_st *pnic_info = wifi_hd;

    return pnic_info->ndev;
}
static wf_inline void wf_wifi_priv_set (wf_wifi_hd_t wifi_hd, void *priv)
{
    nic_info_st *pnic_info = wifi_hd;

    pnic_info->ndev = priv;
}

int wf_wifi_init (const wf_wifi_ops_t *ops);
void wf_wifi_uninit (void);

int wf_wifi_enable (wf_wifi_hd_t wifi_hd);
int wf_wifi_disable (wf_wifi_hd_t wifi_hd);

int wf_wifi_set_mode (wf_wifi_hd_t wifi_hd, wf_wifi_mode_t mode);

int wf_wifi_scan (wf_wifi_hd_t wifi_hd, wf_wifi_scan_info_t *scan_info);
int wf_wifi_scan_stop (wf_wifi_hd_t wifi_hd);

int wf_wifi_conn (wf_wifi_hd_t wifi_hd, wf_wifi_conn_info_t *conn_info);
int wf_wifi_disconn (wf_wifi_hd_t wifi_hd);

int wf_wifi_ap_start (wf_wifi_hd_t wifi_hd, wf_wifi_ap_info_t *ap_info);
int wf_wifi_ap_stop (wf_wifi_hd_t wifi_hd);
int wf_wifi_ap_deauth (wf_wifi_hd_t wifi_hd);

int wf_wifi_set_powersave (wf_wifi_hd_t wifi_hd, int level);
int wf_wifi_get_powersave (wf_wifi_hd_t wifi_hd);

int wf_wifi_set_channel (wf_wifi_hd_t wifi_hd, int channel);
int wf_wifi_get_channel (wf_wifi_hd_t wifi_hd);

int wf_wifi_set_country (wf_wifi_hd_t wifi_hd,
                         wf_wifi_country_code_t country_code);
wf_wifi_country_code_t wf_wifi_get_country (wf_wifi_hd_t wifi_hd);

int wf_wifi_set_mac (wf_wifi_hd_t wifi_hd, wf_u8 mac[6]);
int wf_wifi_get_mac (wf_wifi_hd_t wifi_hd, wf_u8 *mac[6]);

int wf_wifi_get_rssi (wf_wifi_hd_t wifi_hd);

int wf_wifi_data_xmit (wf_wifi_hd_t wifi_hd, void *data, wf_u32 len);

#endif /* __WF_WIFI_H__ */

