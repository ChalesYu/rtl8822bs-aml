
#ifndef __WF_OIDS_H__
#define __WF_OIDS_H__

#define CFG_SLT_SUPPORT                             0

#define OID_CUSTOM_OID_INTERFACE_VERSION                0xFFA0C000

#if CFG_SLT_SUPPORT
#define OID_CUSTOM_SLT                               0xFFA0C587
#endif


/* Precedent OIDs */
#define PARAM_MAX_LEN_SSID                      32

#define OID_CUSTOM_WMM_PS_TEST                          0xFFA0C589
#define OID_CUSTOM_MCR_RW                               0xFFA0C801
#define OID_CUSTOM_EEPROM_RW                            0xFFA0C803
#define OID_CUSTOM_SW_CTRL                              0xFFA0C805
#define OID_CUSTOM_SEC_CHECK                            0xFFA0C806


/* RF Test specific OIDs */
#define OID_CUSTOM_TEST_MODE                            0xFFA0C901
#define OID_CUSTOM_TEST_RX_STATUS                       0xFFA0C903
#define OID_CUSTOM_TEST_TX_STATUS                       0xFFA0C905
#define OID_CUSTOM_ABORT_TEST_MODE                      0xFFA0C906
#define OID_CUSTOM_MTK_WIFI_TEST                        0xFFA0C911
#define OID_CUSTOM_TEST_ICAP_MODE                       0xFFA0C913

/* NVRAM */
#define OID_CUSTOM_MTK_NVRAM_RW                         0xFFA0C941
#define OID_CUSTOM_CFG_SRC_TYPE                         0xFFA0C942

#if CFG_SUPPORT_WAPI
#define OID_802_11_WAPI_MODE                            0xFFA0CA00
#define OID_802_11_WAPI_ASSOC_INFO                      0xFFA0CA01
#define OID_802_11_SET_WAPI_KEY                         0xFFA0CA02
#endif

#define MAX_ARRAY_SEND_PACKETS						64

#define PARAM_MAX_LEN_RATES                     8
#define PARAM_MAX_LEN_RATES_EX                  16

#define MAX_NUM_GROUP_ADDR                      	32


#define PARAM_DEVICE_WAKE_UP_ENABLE                     0x00000001

/* Packet filter bit definitioin (wf_u32 bit-wise definition) */
#define PARAM_PACKET_FILTER_DIRECTED            0x00000001
#define PARAM_PACKET_FILTER_MULTICAST           0x00000002
#define PARAM_PACKET_FILTER_ALL_MULTICAST       0x00000004
#define PARAM_PACKET_FILTER_BROADCAST           0x00000008

#define PARAM_PACKET_FILTER_SUPPORTED   (PARAM_PACKET_FILTER_DIRECTED | \
                                         PARAM_PACKET_FILTER_MULTICAST | \
                                         PARAM_PACKET_FILTER_BROADCAST | \
                                         PARAM_PACKET_FILTER_ALL_MULTICAST)


/*------------------------------------------------------------------------------
 * 802.11i RSN Pre-authentication PMKID cahce maximun number
 *------------------------------------------------------------------------------
 */
#define CFG_MAX_PMKID_CACHE                     16 

/* 7.3.2.1 SSID element */
#define ELEM_MAX_LEN_SSID                           32

#define PARAM_MAX_LEN_RATES_EX                  16

typedef NDIS_STATUS (*PFN_OID_HANDLER_FUNC_REQ) (
	void *prAdapter, void *pvBuf, wf_u32 u4BufLen, wf_u32 *pu4OutInfoLen);


#define DISP_STRING(_str)       _str

typedef wf_u8          PARAM_RATES_EX[PARAM_MAX_LEN_RATES_EX];

#define CFG_ENABLE_WAKEUP_ON_LAN 1

typedef enum _ENUM_PARAM_NETWORK_TYPE_T {
	PARAM_NETWORK_TYPE_FH,
	PARAM_NETWORK_TYPE_DS,
	PARAM_NETWORK_TYPE_OFDM5,
	PARAM_NETWORK_TYPE_OFDM24,
	PARAM_NETWORK_TYPE_AUTOMODE,
	PARAM_NETWORK_TYPE_NUM                    /*!< Upper bound, not real case */
} ENUM_PARAM_NETWORK_TYPE_T, * P_ENUM_PARAM_NETWORK_TYPE_T;

typedef enum _ENUM_OID_METHOD_T {
    ENUM_OID_GLUE_ONLY,
    ENUM_OID_GLUE_EXTENSION,
    ENUM_OID_DRIVER_CORE
} ENUM_OID_METHOD_T, *P_ENUM_OID_METHOD_T;

typedef enum _PARAM_DEVICE_POWER_STATE
{
    ParamDeviceStateUnspecified = 0,
    ParamDeviceStateD0,
    ParamDeviceStateD1,
    ParamDeviceStateD2,
    ParamDeviceStateD3,
    ParamDeviceStateMaximum
} PARAM_DEVICE_POWER_STATE, *PPARAM_DEVICE_POWER_STATE;

/* NDIS_802_11_AUTHENTICATION_MODE */
typedef enum _ENUM_PARAM_AUTH_MODE_T
{
    AUTH_MODE_OPEN,                     /*!< Open system */
    AUTH_MODE_SHARED,                   /*!< Shared key */
    AUTH_MODE_AUTO_SWITCH,              /*!< Either open system or shared key */
    AUTH_MODE_WPA,
    AUTH_MODE_WPA_PSK,
    AUTH_MODE_WPA_NONE,                 /*!< For Ad hoc */
    AUTH_MODE_WPA2,
    AUTH_MODE_WPA2_PSK,
    AUTH_MODE_NUM                       /*!< Upper bound, not real case */
} ENUM_PARAM_AUTH_MODE_T, *P_ENUM_PARAM_AUTH_MODE_T;

typedef struct _PARAM_WEP_T
{
    wf_u32             u4Length;             /*!< Length of structure */
    wf_u32             u4KeyIndex;           /*!< 0: pairwise key, others group keys */
    wf_u32             u4KeyLength;          /*!< Key length in bytes */
    wf_u8              aucKeyMaterial[32];    /*!< Key content by above setting */
} PARAM_WEP_T, *P_PARAM_WEP_T;


/* NDIS_802_11_ENCRYPTION_STATUS *//* Encryption types */
typedef enum _ENUM_WEP_STATUS_T
{
    ENUM_WEP_ENABLED,
    ENUM_ENCRYPTION1_ENABLED = ENUM_WEP_ENABLED,
    ENUM_WEP_DISABLED,
    ENUM_ENCRYPTION_DISABLED = ENUM_WEP_DISABLED,
    ENUM_WEP_KEY_ABSENT,
    ENUM_ENCRYPTION1_KEY_ABSENT = ENUM_WEP_KEY_ABSENT,
    ENUM_WEP_NOT_SUPPORTED,
    ENUM_ENCRYPTION_NOT_SUPPORTED = ENUM_WEP_NOT_SUPPORTED,
    ENUM_ENCRYPTION2_ENABLED,
    ENUM_ENCRYPTION2_KEY_ABSENT,
    ENUM_ENCRYPTION3_ENABLED,
    ENUM_ENCRYPTION3_KEY_ABSENT
} ENUM_PARAM_ENCRYPTION_STATUS_T, *P_ENUM_PARAM_ENCRYPTION_STATUS_T;

typedef enum _ENUM_RELOAD_DEFAULTS
{
    ENUM_RELOAD_WEP_KEYS
} PARAM_RELOAD_DEFAULTS, *P_PARAM_RELOAD_DEFAULTS;

typedef struct _PARAM_CUSTOM_MCR_RW_STRUC_T {
    wf_u32             u4McrOffset;
    wf_u32             u4McrData;
} PARAM_CUSTOM_MCR_RW_STRUC_T, *P_PARAM_CUSTOM_MCR_RW_STRUC_T;

typedef struct _PARAM_CUSTOM_MEM_DUMP_STRUC_T {
    wf_u32     u4Address;
    wf_u32     u4Length;
    wf_u32     u4RemainLength;
    wf_u8      ucFragNum;
} PARAM_CUSTOM_MEM_DUMP_STRUC_T, *P_PARAM_CUSTOM_MEM_DUMP_STRUC_T;


typedef struct _PARAM_CUSTOM_SW_CTRL_STRUC_T {
    wf_u32             u4Id;
    wf_u32             u4Data;
} PARAM_CUSTOM_SW_CTRL_STRUC_T, *P_PARAM_CUSTOM_SW_CTRL_STRUC_T;

typedef struct _PARAM_MTK_WIFI_TEST_STRUC_T {
    wf_u32                 u4FuncIndex;
    wf_u32                 u4FuncData;
} PARAM_MTK_WIFI_TEST_STRUC_T, *P_PARAM_MTK_WIFI_TEST_STRUC_T;

typedef struct _PARAM_CUSTOM_EEPROM_RW_STRUC_T {
    wf_u8              ucEepromMethod; /* For read only read: 1, query size: 0*/
    wf_u8              ucEepromIndex;
    wf_u8              reserved;
    wf_u16            u2EepromData;
} PARAM_CUSTOM_EEPROM_RW_STRUC_T, *P_PARAM_CUSTOM_EEPROM_RW_STRUC_T,
    PARAM_CUSTOM_NVRAM_RW_STRUCT_T, *P_PARAM_CUSTOM_NVRAM_RW_STRUCT_T;



/* OID set/query processing entry */
typedef struct _WLAN_REQ_ENTRY {
    NDIS_OID            rOid;            /* OID */
    wf_u8             *pucOidName;      /* OID name text */
    wf_u8             fgQryBufLenChecking;
    wf_u8             fgSetBufLenChecking;
    ENUM_OID_METHOD_T   eOidMethod;
    wf_u32             u4InfoBufLen;
    PFN_OID_HANDLER_FUNC_REQ    pfOidQueryHandler;
    PFN_OID_HANDLER_FUNC_REQ    pfOidSetHandler;
} WLAN_REQ_ENTRY, *P_WLAN_REQ_ENTRY;


// NIC mangement related macros.
#define MAX_NUM_PHY_TYPES                    4
#define MAX_NUM_DOT11_REG_DOMAINS_VALUE      10
#define MAX_NUM_DIVERSITY_SELECTION_RX_LIST  256

#define NATIVE_802_11_MAX_SCAN_SSID				8
#define NATIVE_802_11_MAX_DESIRED_BSSID			8
#define NATIVE_802_11_MAX_DESIRED_SSID			1
#define NATIVE_802_11_MAX_EXCLUDED_MACADDR		4
#define NATIVE_802_11_MAX_PRIVACY_EXEMPTION		32

#define WF_WIN_80211_IES_SIZE_MAX               768

/* The macro to check whether two SSIDs are equal */
#define EQUAL_SSID(pucSsid1, ucSsidLen1, pucSsid2, ucSsidLen2) \
    ((ucSsidLen1 <= ELEM_MAX_LEN_SSID) && \
        (ucSsidLen2 <= ELEM_MAX_LEN_SSID) && \
        ((ucSsidLen1) == (ucSsidLen2)) && \
        !wf_memcmp(pucSsid1, pucSsid2, ucSsidLen1))


typedef enum wf_phy_id_e{
	WF_PHY_ID_B,
	WF_PHY_ID_G,
	WF_PHY_ID_N,
	WF_PHY_ID_MAX
}wf_phy_id_e;

#define WF_PHY_ID_DEFAULT WF_PHY_ID_B

//
// PHY specific MIB. The MIB could be different for different vendors.
//
typedef struct _NICPHYMIB
{
	DOT11_PHY_TYPE                      PhyType;
	ULONG                               PhyID;
	DOT11_RATE_SET                      OperationalRateSet;
	DOT11_RATE_SET                      ActiveRateSet;
	UCHAR                               Channel;
	DOT11_SUPPORTED_DATA_RATES_VALUE_V2 SupportedDataRatesValue;
} NICPHYMIB, *PNICPHYMIB;

typedef enum _SCAN_STATE
{
	ScanStateSwChnl = 0,
	ScanStatePerformScan_Pass1,
	ScanStateMaxState
} SCAN_STATE;

typedef struct _PARAM_SSID_T {
    wf_u32  u4SsidLen;      /*!< SSID length in bytes. Zero length is broadcast(any) SSID */
    wf_u8   aucSsid[PARAM_MAX_LEN_SSID];
} PARAM_SSID_T, *P_PARAM_SSID_T;

typedef struct _OctetString
{
    PUCHAR  Octet;
    USHORT  Length;
}OctetString,*pOctetString;


typedef struct _TIM{
    UCHAR   DtimCount;
    UCHAR   DtimPeriod;
    UCHAR   BitmapCtrl;
    UCHAR   PartialVirtualBitmap[251];
}TIM,*pTIM;

/*--------------------------------------------------------------*/
/*! \brief Set/Query authentication and encryption capability.  */
/*--------------------------------------------------------------*/
typedef struct _PARAM_AUTH_ENCRYPTION_T
{
    ENUM_PARAM_AUTH_MODE_T              eAuthModeSupported;
    ENUM_PARAM_ENCRYPTION_STATUS_T      eEncryptStatusSupported;
} PARAM_AUTH_ENCRYPTION_T, *P_PARAM_AUTH_ENCRYPTION_T;

typedef struct _PARAM_CAPABILITY_T
{
     wf_u32                  u4Length;
     wf_u32                  u4Version;
     wf_u32                  u4NoOfPMKIDs;
     wf_u32                  u4NoOfAuthEncryptPairsSupported;
     PARAM_AUTH_ENCRYPTION_T  arAuthenticationEncryptionSupported[1];
} PARAM_CAPABILITY_T, *P_PARAM_CAPABILITY_T;

typedef struct _PARAM_802_11_CONFIG_FH_T {
	wf_u32                  u4Length;         /*!< Length of structure */
	wf_u32                  u4HopPattern;     /*!< Defined as 802.11 */
	wf_u32                  u4HopSet;         /*!< to one if non-802.11 */
	wf_u32                  u4DwellTime;      /*!< In unit of Kusec */
} PARAM_802_11_CONFIG_FH_T, * P_PARAM_802_11_CONFIG_FH_T;

typedef struct _PARAM_802_11_CONFIG_T {
	wf_u32                  u4Length;         /*!< Length of structure */
	wf_u32                  u4BeaconPeriod;   /*!< In unit of Kusec */
	wf_u32                  u4ATIMWindow;     /*!< In unit of Kusec */
	wf_u32                  u4DSConfig;       /*!< Channel frequency in unit of kHz */
	PARAM_802_11_CONFIG_FH_T rFHConfig;
} PARAM_802_11_CONFIG_T, * P_PARAM_802_11_CONFIG_T;

/*! \brief Capabilities, privacy, rssi and IEs of each BSSID */
typedef struct _PARAM_BSSID_EX_T {
    wf_u32                         u4Length;             /*!< Length of structure */
	wf_80211_addr_t               arMacAddress;         /*!< BSSID */
    wf_u8                          Reserved[2];
    PARAM_SSID_T                    rSsid;               /*!< SSID */
    wf_u32                         u4Privacy;            /*!< Need WEP encryption */
	INT32                      rRssi;               /*!< in dBm */
    ENUM_PARAM_NETWORK_TYPE_T       eNetworkTypeInUse;
    PARAM_802_11_CONFIG_T           rConfiguration;
    ENUM_PARAM_OP_MODE_T            eOpMode;
    PARAM_RATES_EX                  rSupportedRates;
    wf_u32                         u4IELength;
    wf_u8                          aucIEs[1];
} PARAM_BSSID_EX_T, *P_PARAM_BSSID_EX_T;

typedef struct _PARAM_BSSID_LIST_EX {
    wf_u32                         u4NumberOfItems;      /*!< at least 1 */
    PARAM_BSSID_EX_T                arBssid[1];
} PARAM_BSSID_LIST_EX_T, *P_PARAM_BSSID_LIST_EX_T;

typedef struct _CMD_LINK_ATTRIB {
    wf_u8       cRssiTrigger;
    wf_u8      ucDesiredRateLen;
    wf_u16     u2DesiredRate[32];
    wf_u8      ucMediaStreamMode;
    wf_u8      aucReserved[1];
} CMD_LINK_ATTRIB, *P_CMD_LINK_ATTRIB;

typedef struct wf_cache_info_s{
	wf_u8  dev_mac[6];
	wf_bool  ap_valid;
	PARAM_SSID_T ap_ssid;
	wf_u8  ap_mac[6];
}wf_cache_info_t;



typedef struct wf_mib_info_s
{
	// To store xmit_ok, xmit_error, recv_ok, recv_error frames.
	LARGE_INTEGER num_xmit_ok;
	LARGE_INTEGER num_xmit_error;
	LARGE_INTEGER num_recv_ok;
	LARGE_INTEGER num_recv_error;

	// Association information.
	P_PARAM_BSSID_EX_T 			curApInfo;
	CMD_LINK_ATTRIB             eLinkAttr;
	wf_u8						rRssi;
	
	BOOLEAN 					connect_state;

	// Authentication, association and encryption.
	ENUM_PARAM_AUTH_MODE_T auth_mode;
	BOOLEAN                 fgTxBcKeyExist;             /* Bc Transmit key exist or not */
	wf_u8                  ucTxDefaultKeyID;           /* Bc Transmit key ID */
	ENUM_PARAM_ENCRYPTION_STATUS_T  eEncStatus;			/* Current Encryption status */
	wf_u32	HwUnicastCipher;		/* UnicastCipher */
	wf_u32	HwMulticastCipher;		/* MulticastCipher */

	wf_lock_t   writeKeyLock;		// Protect keyTable.
	WF_NICKEY	KeyTable[MAX_KEY_NUM];

	wf_u32  u4CipherSuitesFlags;
	// BSS list.
	WDFSPINLOCK 						bss_lock;
	ULONG								bss_cnt;
	wf_wlan_mgmt_scan_que_node_t 		bss_node[64];

	// To handle the unexpected situation.
	WDFTIMER                			exception_timer;

	// Event to notify that some work is finished.
	KEVENT 								halt_deauth_finish;
	
	KEVENT 								scan_hidden_finish;
} wf_mib_info_t, * pwf_mib_info_t;

#define PARAM_WHQL_RSSI_MAX_DBM                 (-10)
#define PARAM_WHQL_RSSI_MIN_DBM                 (-200)



#define OID_RTL_SAMPLE_OID       0xDEEF1201
#define OID_RTL_SAMPLE2_OID      0xDEEF1202


#define MP_PRIVATE_OID_LIST         \
    OID_RTL_SAMPLE_OID,               \
    OID_RTL_SAMPLE2_OID,

VOID
Mp11CompletePendedRequest(
	__in  PADAPTER		  pAdapter,
	__in  NDIS_STATUS	  ndisStatus
	);

NDIS_STATUS wf_mp_oids_init(PADAPTER	   pAdapter);

VOID wf_mp_oids_deinit(PADAPTER	  	  pAdapter);

static wf_wlan_mgmt_scan_que_node_t *wf_find_scan_info_by_bssid(PADAPTER adapter, wf_u8* destBssid);

BOOLEAN
wf_req_search_supported_Oid_entry (
    IN  NDIS_OID            rOid,
    OUT P_WLAN_REQ_ENTRY    *ppWlanReqEntry
    );


/* Query */

/**
* Specifies the vendor-assigned version number of the miniport driver. 
*/
NDIS_STATUS
wf_req_query_vendor_driver_version (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQryBuf,
	IN wf_u32 u4QryBufLen,
	OUT wf_u32* pu4QryInfoLen
);

/**
* Specifies the maximum number of bytes that the NIC can provide as lookahead data.
* This specification does not include a header. 
*/
NDIS_STATUS
wf_req_query_max_frame_size (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID	 pvQueryBuf,
	IN	wf_u32  u4QueryBufLen,
	OUT wf_u32* pu4QueryInfoLen
);


/**
* Specifies a bitmask that defines optional properties of the underlying driver or a NIC. 
*/
NDIS_STATUS
wf_req_query_mac_options (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQryBuf,
	IN wf_u32 u4QryBufLen,
	OUT wf_u32* pu4QryInfoLen
);

/**
* Specifies the maximum number of send packet descriptors that a miniport driver's MiniportSendPackets function can accept. 
*/
NDIS_STATUS
wf_req_query_max_send_packets(
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQueryBuf,
	IN wf_u32 u4QueryBufLen,
	OUT wf_u32* pu4QueryInfoLen
);

/**
* The maximum number of multicast addresses the NIC driver can manage. 
*/
NDIS_STATUS
wf_req_query_max_list_size (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQryBuf,
	IN wf_u32 u4QryBufLen,
	OUT wf_u32* pu4QryInfoLen
);

/**
* The MAC address of the current NIC. 
*/
NDIS_STATUS
wf_wlan_oid_query_cur_addr (
	IN  PADAPTER	pAdapter,
	IN	PVOID	 pvQueryBuffer,
	IN	wf_u32  u4QueryBufferLen,
	OUT wf_u32* pu4QueryInfoLen
);

/**
* Specifies the types of physical media that the NIC supports. 
*/
NDIS_STATUS
wf_req_query_physical_medium(
    IN P_GLUE_INFO_T prGlueInfo,
    OUT PVOID pvQryBuf,
    IN wf_u32 u4QryBufLen,
    OUT wf_u32* pu4QryInfoLen
);

/**
* Return the wake-up capabilities of its NIC or requests an intermediate driver to return the intermediate
* driver's wake-up capabilities. 
*/
NDIS_STATUS
wf_req_query_PnP_capabilities(
	IN P_GLUE_INFO_T prGlueInfo,
	IN PVOID		pvQueryBuf,
	IN wf_u32		u4QueryBufLen,
	OUT wf_u32* pu4QueryInfoLen
);

/**
* Specifies an array of OIDs for objects that the underlying driver or its NIC supports.
* Objects include general, media-specific, and implementation-specific objects.
*/
NDIS_STATUS
wf_req_query_supported_list (
	IN P_GLUE_INFO_T	prGlueInfo,
	OUT PVOID			pvQryBuf,
	IN wf_u32			u4QryBufLen,
	OUT wf_u32*		pu4QryInfoLen
);

/**
* Specifies the maximum total packet length, in bytes, the NIC supports. This specification includes the header. 
*/
NDIS_STATUS
wf_req_query_max_total_size (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQueryBuf,
	IN wf_u32 u4QueryBufLen,
	OUT wf_u32* pu4QueryInfoLen
);

/**
* Specifies the maximum speed of the NIC in kbps. 
*/
NDIS_STATUS
wf_wlan_oid_query_link_speed(
	IN PADAPTER	prAdapter,
	IN	PVOID	 pvQueryBuffer,
	IN	wf_u32  u4QueryBufferLen,
	OUT wf_u32* pu4QueryInfoLen
);

/**
* Specifies the connection status of the NIC on the network.
*/
NDIS_STATUS
wf_req_query_media_connect_status (
	IN P_GLUE_INFO_T	 prGlueInfo,
	OUT PVOID			 pvQryBuf,
	IN wf_u32			 u4QryBufLen,
	OUT wf_u32*		 pu4QryInfoLen
);

/**
* Specifies the number of frames that are transmitted without errors.
*/
NDIS_STATUS
wf_wlan_oid_query_xmit_ok (
	IN	PADAPTER prAdapter,
	IN	PVOID		pvQueryBuffer,
	IN	wf_u32 	u4QueryBufferLen,
	OUT wf_u32*	pu4QueryInfoLen
);

/**
* Specifies the number of frames that a NIC fails to transmit.
*/
NDIS_STATUS
wf_wlan_oid_query_xmit_error (
	IN	PADAPTER prAdapter,
	IN	PVOID		pvQueryBuffer,
	IN	wf_u32 	u4QueryBufferLen,
	OUT wf_u32*	pu4QueryInfoLen
);

/**
* Specifies the number of frames that the NIC receives without errors and indicates to bound protocols.
*/
NDIS_STATUS
wf_wlan_oid_query_rcv_ok (
	IN	PADAPTER 	prAdapter,
	IN	PVOID			pvQueryBuffer,
	IN	wf_u32 		u4QueryBufferLen,
	OUT wf_u32*		pu4QueryInfoLen
);

/**
* Specifies the number of frames that a NIC receives but does not indicate to the protocols due to errors.
*/
NDIS_STATUS
wf_wlan_oid_query_rcv_error (
	IN	PADAPTER 	prAdapter,
	IN	PVOID			pvQueryBuffer,
	IN	wf_u32 		u4QueryBufferLen,
	OUT wf_u32*		pu4QueryInfoLen
);

/**
* Query the current authentication mode.
*/
NDIS_STATUS
wf_wlan_oid_query_auth_mode (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
);

/**
* Query the NIC running mode.
*/
NDIS_STATUS
wf_wlan_oid_query_infra_mode (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
);

/**
* Query the Encryption status.
*/
NDIS_STATUS
wf_wlan_oid_query_enc_status (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
);

/**
* Query the SSID with which the device is associated.
*/
NDIS_STATUS
wf_wlan_oid_query_ssid (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
);

/**
* Query the miniport driver for its supported wireless network authentication and encryption capabilities.
*/
NDIS_STATUS
wf_wlan_oid_query_capability (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
);

/**
* Returns a list containing all of the detected BSSIDs and their attributes to NDIS.
*/
NDIS_STATUS
wf_wlan_oid_query_bssid_list (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
);

/**
* If the media state is connected, return the BSSID.
*/
NDIS_STATUS
wf_wlan_oid_query_bssid (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
);

/**
* Return the current value of the received signal strength indication (RSSI).
*/
NDIS_STATUS
wf_wlan_oid_query_rssi (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
);

/**
* Return the IEs that were used in the last association or reassociation request to an access point. 
*/
NDIS_STATUS
wf_req_query_assoc_info (
	IN	P_GLUE_INFO_T	  prGlueInfo,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
);

/**
* Query the media type.
*/
NDIS_STATUS
wf_req_query_media(
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQryBuf,
	IN wf_u32 u4QryBufLen,
	OUT wf_u32* pu4QryInfoLen
);

/**
* Reports the types of net packets that are in receive indications from a miniport driver.
*/
NDIS_STATUS
wf_wlan_oid_query_cur_pkt_filter (
	IN PADAPTER	prAdapter,
	OUT PVOID		pvQueryBuffer,
	IN	wf_u32 	u4QueryBufferLen,
	OUT wf_u32*	pu4QueryInfoLen
);

/**
* Query the mac address of the physical device.
*/
NDIS_STATUS
wf_wlan_oid_query_permanent_addr(
    IN  PADAPTER prAdapter,
    IN  PVOID    pvQueryBuffer,
    IN  wf_u32   u4QueryBufferLen,
    OUT wf_u32*  pu4QueryInfoLen
);

/**
* Query the vendor id of the device.
*/
NDIS_STATUS
wf_wlan_oid_query_vendor_id(
    IN  PADAPTER prAdapter,
    IN  PVOID    pvQueryBuffer,
    IN  wf_u32   u4QueryBufferLen,
    OUT wf_u32*  pu4QueryInfoLen
);


/* Set */

/**
* Set network address. (Empty function )
*/
NDIS_STATUS
wf_wlan_oid_set_network_address(
    IN  PADAPTER    prAdapter,
    IN  PVOID       pvSetBuffer,
    IN  wf_u32      u4SetBufferLen,
    OUT wf_u32*     pu4SetInfoLen
);

/**
* Set multicast list. (Empty function )
*/
NDIS_STATUS
wf_wlan_oid_set_multicast_list(
     IN  PADAPTER    prAdapter,
     IN  PVOID       pvSetBuffer,
     IN  wf_u32     u4SetBufferLen,
     OUT wf_u32*    pu4SetInfoLen
);

/**
* Set authentication mode.
*/
NDIS_STATUS
wf_wlan_oid_set_auth_mode (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
);

/**
* Set the NIC running mode.
*/
NDIS_STATUS
wf_wlan_oid_set_infra_mode (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
);

/**
* Set the Encryption status.
*/
NDIS_STATUS
wf_wlan_oid_set_enc_status (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
);

/**
* Return NDIS_STATUS_PENDING to OID_802_11_SSID;
*/
NDIS_STATUS
wf_pseudo_wlan_oid_set_ssid(
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
);

/**
* Set the service set identifier (SSID) of the BSS with which the device can associate.
*/
NDIS_STATUS
wf_wlan_oid_set_ssid (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
);

/**
* Set diasssociation.
*/
NDIS_STATUS
wf_wlan_oid_set_disassoc (
	IN  PADAPTER		  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 			  u4SetBufferLen,
	OUT wf_u32*			  pu4SetInfoLen
);

/**
* Set an active scan.
*/
NDIS_STATUS
wf_wlan_oid_set_bssid_list_scan (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
);

/**
* Set a key to a specified value. Keys are used for authentication or encryption or both.
*/
NDIS_STATUS
wf_wlan_oid_set_add_key (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID	 pvSetBuffer,
	IN	wf_u32  u4SetBufferLen,
	OUT wf_u32* pu4SetInfoLen
);

/**
* Set a WEP key to a specified value.
*/
NDIS_STATUS
wf_wlan_oid_set_add_wep (
    IN  PADAPTER       prAdapter,
    IN  PVOID    pvSetBuffer,
    IN  wf_u32  u4SetBufferLen,
    OUT wf_u32* pu4SetInfoLen
);

/**
* Return NDIS_STATUS_PENDING to OID_802_11_BSSID;
*/
NDIS_STATUS
wf_pseudo_wlan_oid_set_bssid(
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
);

/**
* Set an association with the specified BSSID.
*/
NDIS_STATUS
wf_wlan_oid_set_bssid (
	IN	PADAPTER	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32		  u4SetBufferLen,
	OUT wf_u32* 	  pu4SetInfoLen
);

/**
* Reload the defaults of the specified type.
*/
NDIS_STATUS
wf_wlan_oid_set_reload_defaults (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
);

/**
* Specifies the types of net packets for which a protocol receives indications from a miniport driver.
*/
NDIS_STATUS
wf_wlan_oid_set_cur_pkt_filter (
	IN PADAPTER	prAdapter,
	IN	PVOID	 pvSetBuffer,
	IN	wf_u32  u4SetBufferLen,
	OUT wf_u32* pu4SetInfoLen
);

/**
* Specifies the number of bytes of received packet data that the miniport driver should indicate to the
* protocol driver. This specification does not include the header.
*/
NDIS_STATUS
wf_req_set_current_look_ahead(
	IN	P_GLUE_INFO_T	prGlueInfo,
	IN	PVOID			prSetBuffer,
	IN	wf_u32 		u4SetBufferLen,
	OUT wf_u32*		pu4SetInfoLen
);

#endif  // _MP_OIDS_H_

