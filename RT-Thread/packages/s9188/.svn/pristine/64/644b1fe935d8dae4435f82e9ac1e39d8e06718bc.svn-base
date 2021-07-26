#ifndef __WF_OIDS_SEC_H__
#define __WF_OIDS_SEC_H__

/* Cihpher suite flags */
#define CIPHER_FLAG_NONE                        0x00000000
#define CIPHER_FLAG_WEP40                       0x00000001 /* BIT 1 */
#define CIPHER_FLAG_TKIP                        0x00000002 /* BIT 2 */
#define CIPHER_FLAG_CCMP                        0x00000008 /* BIT 4 */
#define CIPHER_FLAG_WEP104                      0x00000010 /* BIT 5 */
#define CIPHER_FLAG_WEP128                      0x00000020 /* BIT 6 */

#define MAX_KEY_NUM                             4
#define WEP_40_LEN                              5
#define WEP_104_LEN                             13
#define LEGACY_KEY_MAX_LEN                      16
#define CCMP_KEY_LEN                            16
#define TKIP_KEY_LEN                            32
#define MAX_KEY_LEN                             32

#define TKIP_MIC_KEY_LEN						8
#define TKIP_ENC_KEY_LEN						16

#define WF_DOT11_MAX_NUM_DEFAULT_KEY            4


typedef wf_u8          PARAM_MAC_ADDRESS[6];
typedef wf_u64         PARAM_KEY_RSC;

#define IS_TRANSMIT_KEY       BIT(31)
#define IS_UNICAST_KEY        BIT(30)
#define IS_AUTHENTICATOR      BIT(28)


typedef enum WF_DOT11_CIPHER_ALGORITHM {
  WF_DOT11_CIPHER_ALGO_NONE,
  WF_DOT11_CIPHER_ALGO_WEP40,
  WF_DOT11_CIPHER_ALGO_TKIP,
  WF_DOT11_CIPHER_ALGO_CCMP,
  WF_DOT11_CIPHER_ALGO_WEP104,
  WF_DOT11_CIPHER_ALGO_WEP,
  WF_DOT11_CIPHER_ALGO_IHV_START,
  WF_DOT11_CIPHER_ALGO_IHV_END
} WF_DOT11_CIPHER_ALGORITHM, *WF_PDOT11_CIPHER_ALGORITHM;


typedef enum _ENUM_NLO_CIPHER_ALGORITHM {
    NLO_CIPHER_ALGO_NONE = 0x00,
    NLO_CIPHER_ALGO_WEP40 = 0x01,
    NLO_CIPHER_ALGO_TKIP = 0x02,
    NLO_CIPHER_ALGO_CCMP = 0x04,
    NLO_CIPHER_ALGO_WEP104 = 0x05,
    NLO_CIPHER_ALGO_WPA_USE_GROUP = 0x100,
    NLO_CIPHER_ALGO_RSN_USE_GROUP = 0x100,
    NLO_CIPHER_ALGO_WEP = 0x101
} ENUM_NLO_CIPHER_ALGORITHM, * P_ENUM_NLO_CIPHER_ALGORITHM;


/* brief Key mapping of struct NDIS_802_11_KEY  */
typedef struct _WF_NDIS_802_11_KEY_T
{
    wf_u32             u4Length;             /* Length of structure */
    wf_u32             u4KeyIndex;           /* KeyID */
    wf_u32             u4KeyLength;          /* Key length in bytes */
    PARAM_MAC_ADDRESS   arBSSID;              /* MAC address */
    PARAM_KEY_RSC       rKeyRSC;
    wf_u8              aucKeyMaterial[32];    /* Key content by above setting */
    wf_u8              ucBssIdx;
} WF_NDIS_802_11_KEY_T, *P_WF_NDIS_802_11_KEY_T;

typedef struct _ZY_TEST_T
{
    wf_u32             u4Length;             /* Length of structure */
    wf_u32             u4KeyIndex;           /* KeyID */
    wf_u32             u4KeyLength;          /* Key length in bytes */
    wf_u8 			   mac_addr[6];
    PARAM_KEY_RSC       rKeyRSC;
    wf_u8              aucKeyMaterial[32];    /* Key content by above setting */
    wf_u8              ucBssIdx;
} ZY_TEST_T, *P_ZY_TEST_T;


typedef struct _WF_NICKEY
{
    BOOLEAN                     ToBeWritten;
	BOOLEAN						bePairwise;
    BOOLEAN                     Valid;
    PARAM_MAC_ADDRESS           MacAddr;
	wf_u8						KeyIndex;
    wf_u32      				AlgoId;
    wf_u8                       KeyLength;              // length of KeyValue in bytes
    wf_u8                       KeyValue[16];           // 128 bits
    wf_u8                       TxMICKey[8];            // for TKIP only
    wf_u8                       RxMICKey[8];            // for TKIP only
    union {
        struct {
            wf_u64           PN:48;                  // for CCMP
            wf_u64           PN_unused:16;
        };
        struct {
            wf_u64           TSC:48;                 // for TKIP
            wf_u64           TSC_unused:16;
        };
        struct {
            wf_u32               IV:24;                  // for WEP 
            wf_u32               IV_unused:8;
        };
    };
    struct {
        wf_u64               ReplayCounter:48;       // for CCMP or TKIP
        wf_u64               ReplayCounter_unused:16;
    };
} WF_NICKEY, *P_WF_NICKEY;


VOID wf_reset_cipher_abilities(PADAPTER pAdapter);

VOID wf_set_encryption(PADAPTER pAdapter, BOOLEAN isUnicast, ENUM_NLO_CIPHER_ALGORITHM algoId);

VOID wf_set_hw_default_cipher(PADAPTER pAdapter);

// Parse the key information.
NDIS_STATUS
wf_set_nic_key(PADAPTER prAdapter, P_WF_NDIS_802_11_KEY_T pNdisKey, BOOLEAN bPairwise);

VOID wf_sta_hw_set_unicast_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);

VOID wf_sta_hw_set_group_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);

#endif 

