
#include "pcomp.h"
//#include "wf_debug.h"

#define LOG_D(fmt,...) //DbgPrint("[%s,%d] "fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);
#define LOG_N(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);
#define LOG_I(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);
#define LOG_W(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);
#define LOG_E(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);
#define NUM_SUPPORTED_OIDS      (sizeof(arWlanOidReqTable) / sizeof(WLAN_REQ_ENTRY))

NDIS_STATUS wf_oids_not_implemented_func(void *prAdapter, void *pvBuf, wf_u32 u4BufLen, wf_u32 *pu4OutInfoLen) 
{
	LOG_D("---OID not implemented!------------------------------");
	return NDIS_STATUS_SUCCESS;
}


/*******************************************************************************
*                        P U B L I C   D A T A
********************************************************************************
*/


/*******************************************************************************
*                       P R I V A T E   D A T A
********************************************************************************
*/
/* OID processing table */
/* Order is important here because the OIDs should be in order of
   increasing value for binary searching. */
// TODO: Check
WLAN_REQ_ENTRY arWlanOidReqTable[] = {
    /* General Operational Characteristics */
    /* (NDIS_OID)rOid,                          (wf_u8*)pucOidName,                 fgQryBufLenChecking, fgSetBufLenChecking, eOidMethod, u4InfoBufLen,                                pfOidQueryHandler,                  pfOidSetHandler} */
    { OID_GEN_SUPPORTED_LIST,                   DISP_STRING("OID_GEN_SUPPORTED_LIST"),              FALSE,  FALSE,  ENUM_OID_GLUE_ONLY,   0,                                          wf_req_query_supported_list,              NULL },
    { OID_GEN_HARDWARE_STATUS,                  DISP_STRING("OID_GEN_HARDWARE_STATUS"),             TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   sizeof(NDIS_HARDWARE_STATUS),               wf_oids_not_implemented_func,/*reqQueryHardwareStatus,*/             NULL },
    { OID_GEN_MEDIA_SUPPORTED,                  DISP_STRING("OID_GEN_MEDIA_SUPPORTED"),             TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   sizeof(NDIS_MEDIUM),                        wf_req_query_media,                      NULL },
    { OID_GEN_MEDIA_IN_USE,                     DISP_STRING("OID_GEN_MEDIA_IN_USE"),                TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   sizeof(NDIS_MEDIUM),                        wf_req_query_media,                      NULL },
    { OID_GEN_MAXIMUM_LOOKAHEAD,                DISP_STRING("OID_GEN_MAXIMUM_LOOKAHEAD"),           TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                          wf_req_query_max_frame_size,               NULL },
    { OID_GEN_MAXIMUM_FRAME_SIZE,               DISP_STRING("OID_GEN_MAXIMUM_FRAME_SIZE"),          TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                          wf_req_query_max_frame_size,               NULL },
    { OID_GEN_LINK_SPEED,                       DISP_STRING("OID_GEN_LINK_SPEED"),                  TRUE,   FALSE,  ENUM_OID_DRIVER_CORE, 4,                                          wf_wlan_oid_query_link_speed,              NULL },
    { OID_GEN_TRANSMIT_BUFFER_SPACE,            DISP_STRING("OID_GEN_TRANSMIT_BUFFER_SPACE"),       TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                          wf_oids_not_implemented_func,/*reqQueryTxBufferSpace,*/              NULL },
    { OID_GEN_RECEIVE_BUFFER_SPACE,             DISP_STRING("OID_GEN_RECEIVE_BUFFER_SPACE"),        TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                          wf_oids_not_implemented_func,/*reqQueryRxBufferSpace,*/              NULL },
    { OID_GEN_TRANSMIT_BLOCK_SIZE,              DISP_STRING("OID_GEN_TRANSMIT_BLOCK_SIZE"),         TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                          wf_oids_not_implemented_func,/*wf_req_query_max_total_size,*/               NULL },
    { OID_GEN_RECEIVE_BLOCK_SIZE,               DISP_STRING("OID_GEN_RECEIVE_BLOCK_SIZE"),          TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                          wf_oids_not_implemented_func,/*wf_req_query_max_total_size,*/               NULL },
    { OID_GEN_VENDOR_ID,                        DISP_STRING("OID_GEN_VENDOR_ID"),                   TRUE,   FALSE,  ENUM_OID_GLUE_EXTENSION,4,                                        wf_oids_not_implemented_func,/*wlanoidQueryVendorId,*/               NULL },
    { OID_GEN_VENDOR_DESCRIPTION,               DISP_STRING("OID_GEN_VENDOR_DESCRIPTION"),          FALSE,  FALSE,  ENUM_OID_GLUE_ONLY,   0,                                          wf_oids_not_implemented_func,/*reqQueryVendorDescription,*/          NULL },
    { OID_GEN_CURRENT_PACKET_FILTER,            DISP_STRING("OID_GEN_CURRENT_PACKET_FILTER"),       TRUE,   TRUE,   ENUM_OID_DRIVER_CORE, 4,                                          wf_wlan_oid_query_cur_pkt_filter,    wf_wlan_oid_set_cur_pkt_filter },
    { OID_GEN_CURRENT_LOOKAHEAD,                DISP_STRING("OID_GEN_CURRENT_LOOKAHEAD"),           TRUE,   TRUE,   ENUM_OID_GLUE_ONLY,   4,                                          wf_req_query_max_frame_size,               wf_req_set_current_look_ahead },
    { OID_GEN_DRIVER_VERSION,                   DISP_STRING("OID_GEN_DRIVER_VERSION"),              TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   2,                                          wf_oids_not_implemented_func,/*reqQueryDriverVersion,*/              NULL },
    { OID_GEN_MAXIMUM_TOTAL_SIZE,               DISP_STRING("OID_GEN_MAXIMUM_TOTAL_SIZE"),          TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                          wf_req_query_max_total_size,               NULL },
    { OID_GEN_MAC_OPTIONS,                      DISP_STRING("OID_GEN_MAC_OPTIONS"),                 TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                          wf_req_query_mac_options,                 NULL },
    { OID_GEN_MEDIA_CONNECT_STATUS,             DISP_STRING("OID_GEN_MEDIA_CONNECT_STATUS"),        TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   sizeof(NDIS_MEDIA_STATE),                   wf_req_query_media_connect_status,         NULL },
    { OID_GEN_MAXIMUM_SEND_PACKETS,             DISP_STRING("OID_GEN_MAXIMUM_SEND_PACKETS"),        TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                          wf_req_query_max_send_packets,             NULL },
    { OID_GEN_VENDOR_DRIVER_VERSION,            DISP_STRING("OID_GEN_VENDOR_DRIVER_VERSION"),       TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                          wf_req_query_vendor_driver_version,        NULL },
    { OID_GEN_NETWORK_LAYER_ADDRESSES,          DISP_STRING("OID_GEN_NETWORK_LAYER_ADDRESSES"),     FALSE,  FALSE,  ENUM_OID_DRIVER_CORE, 0,                                          NULL,                               wf_oids_not_implemented_func,/*wlanoidSetNetworkAddress*/},
    { OID_GEN_PHYSICAL_MEDIUM,                  DISP_STRING("OID_GEN_PHYSICAL_MEDIUM"),             TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   sizeof(NDIS_PHYSICAL_MEDIUM),               wf_req_query_physical_medium,             NULL },
    /* General Statistics */
    { OID_GEN_XMIT_OK,                          DISP_STRING("OID_GEN_XMIT_OK"),                     FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          wf_wlan_oid_query_xmit_ok,                 NULL },
    { OID_GEN_RCV_OK,                           DISP_STRING("OID_GEN_RCV_OK"),                      FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          wf_wlan_oid_query_rcv_ok,                  NULL },
    { OID_GEN_XMIT_ERROR,                       DISP_STRING("OID_GEN_XMIT_ERROR"),                  FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          wf_wlan_oid_query_xmit_error,              NULL },
    { OID_GEN_RCV_ERROR,                        DISP_STRING("OID_GEN_RCV_ERROR"),                   FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          wf_wlan_oid_query_rcv_error,               NULL },
    { OID_GEN_RCV_NO_BUFFER,                    DISP_STRING("OID_GEN_RCV_NO_BUFFER"),               FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          wf_oids_not_implemented_func,/*wlanoidQueryRcvNoBuffer,*/            NULL },
    { OID_GEN_RCV_CRC_ERROR,                    DISP_STRING("OID_GEN_RCV_CRC_ERROR"),               FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          wf_oids_not_implemented_func,/*wlanoidQueryRcvCrcError,*/            NULL },

    /* Ethernet Operational Characteristics */
    { OID_802_3_PERMANENT_ADDRESS,              DISP_STRING("OID_802_3_PERMANENT_ADDRESS"),         TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  6,                                          wf_oids_not_implemented_func,/*wlanoidQueryPermanentAddr,*/          NULL },
    { OID_802_3_CURRENT_ADDRESS,                DISP_STRING("OID_802_3_CURRENT_ADDRESS"),           TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  6,                                          wf_wlan_oid_query_cur_addr,            NULL },
    { OID_802_3_MULTICAST_LIST,                 DISP_STRING("OID_802_3_MULTICAST_LIST"),            FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          wf_oids_not_implemented_func,/*wlanoidQueryMulticastList, */         wf_oids_not_implemented_func,/*wlanoidSetMulticastList*/},
    { OID_802_3_MAXIMUM_LIST_SIZE,              DISP_STRING("OID_802_3_MAXIMUM_LIST_SIZE"),         FALSE,  FALSE,  ENUM_OID_GLUE_ONLY,   4,                                           wf_req_query_max_list_size,                NULL },

    /* Ethernet Statistics */
    { OID_802_3_RCV_ERROR_ALIGNMENT,            DISP_STRING("OID_802_3_RCV_ERROR_ALIGNMENT"),       TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   4,                                           wf_oids_not_implemented_func,/*reqQueryRcvErrorAlignment,*/          NULL },
    { OID_802_3_XMIT_ONE_COLLISION,             DISP_STRING("OID_802_3_XMIT_ONE_COLLISION"),        TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  4,                                          wf_oids_not_implemented_func,/*wlanoidQueryXmitOneCollision,*/       NULL },
    { OID_802_3_XMIT_MORE_COLLISIONS,           DISP_STRING("OID_802_3_XMIT_MORE_COLLISIONS"),      TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  4,                                          wf_oids_not_implemented_func,/*wlanoidQueryXmitMoreCollisions,*/     NULL },
    { OID_802_3_XMIT_MAX_COLLISIONS,            DISP_STRING("OID_802_3_XMIT_MAX_COLLISIONS"),       TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  4,                                          wf_oids_not_implemented_func,/*wlanoidQueryXmitMaxCollisions,*/      NULL },

    /* NDIS 802.11 Wireless LAN OIDs */
    { OID_802_11_BSSID,                         DISP_STRING("OID_802_11_BSSID"),                    TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_MAC_ADDRESS),            wf_wlan_oid_query_bssid,                  wf_wlan_oid_set_bssid},
    { OID_802_11_SSID,                          DISP_STRING("OID_802_11_SSID"),                     TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_SSID),                   wf_wlan_oid_query_ssid,                   wf_wlan_oid_set_ssid },
    { OID_802_11_INFRASTRUCTURE_MODE,           DISP_STRING("OID_802_11_INFRASTRUCTURE_MODE"),      TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_NETWORK_INFRASTRUCTURE), wf_wlan_oid_query_infra_mode,     wf_wlan_oid_set_infra_mode },
    { OID_802_11_ADD_WEP,                       DISP_STRING("OID_802_11_ADD_WEP"),                  FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          NULL,                               wf_oids_not_implemented_func,/*wlanoidSetAddWep*/ },
    { OID_802_11_REMOVE_WEP,                    DISP_STRING("OID_802_11_REMOVE_WEP"),               FALSE,  TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_KEY_INDEX),              NULL,                               wf_oids_not_implemented_func,/*wlanoidSetRemoveWep*/ },
    { OID_802_11_DISASSOCIATE,                  DISP_STRING("OID_802_11_DISASSOCIATE"),             FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          NULL,                               wf_oids_not_implemented_func,/*wlanoidSetDisassociate*/},
    { OID_802_11_AUTHENTICATION_MODE,           DISP_STRING("OID_802_11_AUTHENTICATION_MODE"),      TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_AUTHENTICATION_MODE),    wf_wlan_oid_query_auth_mode,               wf_wlan_oid_set_auth_mode },
//    { OID_802_11_PRIVACY_FILTER,              DISP_STRING("OID_802_11_PRIVACY_FILTER"),           TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_PRIVACY_FILTER),         wf_oids_not_implemented_func,/*wlanoidQueryPrivacyFilter,*/          wlanoidSetPirvacyFilter },
    { OID_802_11_BSSID_LIST_SCAN,               DISP_STRING("OID_802_11_BSSID_LIST_SCAN"),          FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          NULL,                               wf_wlan_oid_set_bssid_list_scan },
    { OID_802_11_ENCRYPTION_STATUS,             DISP_STRING("OID_802_11_ENCRYPTION_STATUS"),        TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_ENCRYPTION_STATUS),      wf_wlan_oid_query_enc_status,       wf_wlan_oid_set_enc_status },
    { OID_802_11_RELOAD_DEFAULTS,               DISP_STRING("OID_802_11_RELOAD_DEFAULTS"),          FALSE,  TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_RELOAD_DEFAULTS),        NULL,                               wf_wlan_oid_set_reload_defaults },
    { OID_802_11_ADD_KEY,                       DISP_STRING("OID_802_11_ADD_KEY"),                  FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          NULL,                               wf_wlan_oid_set_add_key },
    { OID_802_11_REMOVE_KEY,                    DISP_STRING("OID_802_11_REMOVE_KEY"),               FALSE,  TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_REMOVE_KEY),             NULL,                               wf_oids_not_implemented_func,/*wlanoidSetRemoveKey*/ },
    { OID_802_11_ASSOCIATION_INFORMATION,       DISP_STRING("OID_802_11_ASSOCIATION_INFORMATION"),  FALSE,  FALSE,  ENUM_OID_GLUE_ONLY,   0,                                           wf_req_query_assoc_info,                  NULL },
    { OID_802_11_TEST,                          DISP_STRING("OID_802_11_TEST"),                     FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          NULL,                               wf_oids_not_implemented_func,/*wlanoidSetTest*/},
    { OID_802_11_MEDIA_STREAM_MODE,             DISP_STRING("OID_802_11_MEDIA_STREAM_MODE"),        TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_MEDIA_STREAM_MODE),      wf_oids_not_implemented_func,/*wlanoidQueryMediaStreamMode,*/        wf_oids_not_implemented_func,/*wlanoidSetMediaStreamMode*/ },
    { OID_802_11_CAPABILITY,                    DISP_STRING("OID_802_11_CAPABILITY"),               TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  4*sizeof(ULONG)+14*sizeof(NDIS_802_11_AUTHENTICATION_ENCRYPTION),       wf_wlan_oid_query_capability,     NULL },
    { OID_802_11_PMKID,                         DISP_STRING("OID_802_11_PMKID"),                    FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          wf_oids_not_implemented_func,/*wlanoidQueryPmkid,*/                  wf_oids_not_implemented_func,/*wlanoidSetPmkid*/ },
    { OID_802_11_NETWORK_TYPES_SUPPORTED,       DISP_STRING("OID_802_11_NETWORK_TYPES_SUPPORTED"),  FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          wf_oids_not_implemented_func,/*wlanoidQueryNetworkTypesSupported,*/  NULL },
    { OID_802_11_NETWORK_TYPE_IN_USE,           DISP_STRING("OID_802_11_NETWORK_TYPE_IN_USE"),      TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_NETWORK_TYPE),           wf_oids_not_implemented_func,/*wlanoidQueryNetworkTypeInUse,*/       wf_oids_not_implemented_func,/*wlanoidSetNetworkTypeInUse*/ },
    { OID_802_11_RSSI,                          DISP_STRING("OID_802_11_RSSI"),                     TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_RSSI),                   wf_wlan_oid_query_rssi,                   NULL },
    { OID_802_11_RSSI_TRIGGER,                  DISP_STRING("OID_802_11_RSSI_TRIGGER"),             TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_RSSI),                   wf_oids_not_implemented_func,/*wlanoidQueryRssiTrigger,*/            wf_oids_not_implemented_func,/*wlanoidSetRssiTrigger*/ },
    { OID_802_11_FRAGMENTATION_THRESHOLD,       DISP_STRING("OID_802_11_FRAGMENTATION_THRESHOLD"),  TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_FRAGMENTATION_THRESHOLD),wf_oids_not_implemented_func,/*wlanoidQueryFragThreshold,*/          wf_oids_not_implemented_func,/*wlanoidSetFragThreshold*/ },
    { OID_802_11_RTS_THRESHOLD,                 DISP_STRING("OID_802_11_RTS_THRESHOLD"),            TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_RTS_THRESHOLD),          wf_oids_not_implemented_func,/*wlanoidQueryRtsThreshold,*/           wf_oids_not_implemented_func,/*wlanoidSetRtsThreshold*/ },
    { OID_802_11_SUPPORTED_RATES,               DISP_STRING("OID_802_11_SUPPORTED_RATES"),          TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_RATES),                  wf_oids_not_implemented_func,/*wlanoidQuerySupportedRates,*/         NULL },
    { OID_802_11_DESIRED_RATES,                 DISP_STRING("OID_802_11_DESIRED_RATES"),            FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_RATES),                  wf_oids_not_implemented_func,/*wlanoidQueryDesiredRates,*/           wf_oids_not_implemented_func,/*wlanoidSetDesiredRates*/ },
    { OID_802_11_CONFIGURATION,                 DISP_STRING("OID_802_11_CONFIGURATION"),            TRUE,   TRUE,   ENUM_OID_GLUE_EXTENSION,  sizeof(NDIS_802_11_CONFIGURATION),          wf_oids_not_implemented_func,/*reqExtQueryConfiguration,*/          wf_oids_not_implemented_func,/*reqExtSetConfiguration*/ },
    { OID_802_11_POWER_MODE,                    DISP_STRING("OID_802_11_POWER_MODE"),               TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_POWER_MODE),             wf_oids_not_implemented_func,/*wlanoidQuery802dot11PowerSaveProfile,*/              wf_oids_not_implemented_func,/*wlanoidSet802dot11PowerSaveProfile*/ },
    { OID_802_11_BSSID_LIST,                    DISP_STRING("OID_802_11_BSSID_LIST"),               FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          wf_wlan_oid_query_bssid_list,              NULL },
    { OID_802_11_STATISTICS,                    DISP_STRING("OID_802_11_STATISTICS"),               TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  sizeof(NDIS_802_11_STATISTICS),             wf_oids_not_implemented_func,/*wlanoidQueryStatistics,*/             NULL },
#if CFG_TCP_IP_CHKSUM_OFFLOAD
    { OID_TCP_TASK_OFFLOAD,                     DISP_STRING("OID_TCP_TASK_OFFLOAD"),                FALSE,  FALSE,  ENUM_OID_GLUE_EXTENSION,  sizeof(NDIS_TASK_OFFLOAD),                  wf_oids_not_implemented_func,/*reqQueryTaskOffload,*/               wf_oids_not_implemented_func,/*reqExtSetTaskOffload*/ },
#endif
    { OID_PNP_CAPABILITIES,                     DISP_STRING("OID_PNP_CAPABILITIES"),                TRUE,   FALSE,  ENUM_OID_GLUE_ONLY,   sizeof(NDIS_PNP_CAPABILITIES),               wf_req_query_PnP_capabilities,            NULL },
    { OID_PNP_SET_POWER,                        DISP_STRING("OID_PNP_SET_POWER"),                   TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  sizeof(NDIS_DEVICE_POWER_STATE),            NULL,                               wf_oids_not_implemented_func,/*wlanoidSetAcpiDevicePowerState*/},
    { OID_PNP_QUERY_POWER,                      DISP_STRING("OID_PNP_QUERY_POWER"),                 TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(NDIS_DEVICE_POWER_STATE),            wf_oids_not_implemented_func,/*wlanoidQueryAcpiDevicePowerState,*/                  NULL },
#if CFG_ENABLE_WAKEUP_ON_LAN
    { OID_PNP_ADD_WAKE_UP_PATTERN,              DISP_STRING("OID_PNP_ADD_WAKE_UP_PATTERN"),         FALSE,  FALSE,  ENUM_OID_GLUE_EXTENSION,0,                                      NULL,                               wf_oids_not_implemented_func,/*wlanoidSetAddWakeupPattern*/ },
    { OID_PNP_REMOVE_WAKE_UP_PATTERN,           DISP_STRING("OID_PNP_REMOVE_WAKE_UP_PATTERN"),      FALSE,  FALSE,  ENUM_OID_GLUE_EXTENSION 0,                                      NULL,                               wf_oids_not_implemented_func,/*wlanoidSetRemoveWakeupPattern*/ },
    { OID_PNP_ENABLE_WAKE_UP,                   DISP_STRING("OID_PNP_ENABLE_WAKE_UP"),              TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(wf_u32),                         wf_oids_not_implemented_func,/*wlanoidQueryEnableWakeup,*/           wf_oids_not_implemented_func,/*wlanoidSetEnableWakeup*/ },
#endif
    { OID_CUSTOM_OID_INTERFACE_VERSION,         DISP_STRING("OID_CUSTOM_OID_INTERFACE_VERSION"),    TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  4,                                          wf_oids_not_implemented_func,/*wlanoidQueryOidInterfaceVersion,*/    NULL },

#if CFG_SLT_SUPPORT
    { OID_CUSTOM_SLT,                               DISP_STRING("OID_CUSTOM_SLT"),    FALSE,   FALSE,  ENUM_OID_DRIVER_CORE,  sizeof(PARAM_MTK_SLT_TEST_STRUC_T),                                          wf_oids_not_implemented_func,/*wlanoidQuerySLTStatus,*/    wf_oids_not_implemented_func,/*wlanoidUpdateSLTMode*/ },
#endif
    { OID_CUSTOM_WMM_PS_TEST,                   DISP_STRING("OID_CUSTOM_WMM_PS_TEST"),              TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  4,                                          NULL,                               wf_oids_not_implemented_func,/*wlanoidSetWiFiWmmPsTest*/ },
    { OID_CUSTOM_MCR_RW,                        DISP_STRING("OID_CUSTOM_MCR_RW"),                   TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(PARAM_CUSTOM_MCR_RW_STRUC_T),        wf_oids_not_implemented_func,/*wlanoidQueryMcrRead,*/                wf_oids_not_implemented_func,/*wlanoidSetMcrWrite*/ },
    { OID_CUSTOM_EEPROM_RW,                     DISP_STRING("OID_CUSTOM_EEPROM_RW"),                TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(PARAM_CUSTOM_EEPROM_RW_STRUC_T),     wf_oids_not_implemented_func,/*wlanoidQueryEepromRead,*/             wf_oids_not_implemented_func,/*wlanoidSetEepromWrite*/ },
    { OID_CUSTOM_SW_CTRL,                        DISP_STRING("OID_CUSTOM_SW_CTRL"),                   TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(PARAM_CUSTOM_SW_CTRL_STRUC_T),        wf_oids_not_implemented_func,/*wlanoidQuerySwCtrlRead,*/                wf_oids_not_implemented_func,/*wlanoidSetSwCtrlWrite*/ },
    { OID_CUSTOM_TEST_MODE,                     DISP_STRING("OID_CUSTOM_TEST_MODE"),                FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          NULL,                               wf_oids_not_implemented_func,/*wlanoidRftestSetTestMode*/ },
    { OID_CUSTOM_ABORT_TEST_MODE,               DISP_STRING("OID_CUSTOM_ABORT_TEST_MODE"),          FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,  0,                                          NULL,                               wf_oids_not_implemented_func,/*wlanoidRftestSetAbortTestMode*/ },
    { OID_CUSTOM_MTK_WIFI_TEST,                 DISP_STRING("OID_CUSTOM_MTK_WIFI_TEST"),            TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,  sizeof(PARAM_MTK_WIFI_TEST_STRUC_T),        wf_oids_not_implemented_func,/*wlanoidRftestQueryAutoTest,*/         wf_oids_not_implemented_func,/*wlanoidRftestSetAutoTest*/ },
    { OID_CUSTOM_TEST_ICAP_MODE,                DISP_STRING("OID_CUSTOM_TEST_ICAP_MODE"),           FALSE,   FALSE,   ENUM_OID_DRIVER_CORE, 0,                                          NULL,                              wf_oids_not_implemented_func,/*wlanoidRftestSetTestIcapMode*/},
//    { OID_CUSTOM_TEST_RX_STATUS,                DISP_STRING("OID_CUSTOM_TEST_RX_STATUS"),           TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  sizeof(PARAM_CUSTOM_RFTEST_RX_STATUS_STRUC_T),   wlanoidQueryRfTestRxStatus,    NULL },
//    { OID_CUSTOM_TEST_TX_STATUS,                DISP_STRING("OID_CUSTOM_TEST_TX_STATUS"),           TRUE,   FALSE,  ENUM_OID_DRIVER_CORE,  sizeof(PARAM_CUSTOM_RFTEST_TX_STATUS_STRUC_T),   wlanoidQueryRfTestTxStatus,    NULL },
    /* (NDIS_OID)rOid,                          (wf_u8*)pucOidName,                 fgQryBufLenChecking, fgSetBufLenChecking, fgIsHandleInGlueLayerOnly, u4InfoBufLen, pfOidQueryHandler,                  pfOidSetHandler} */
#if 0
    { OID_CUSTOM_MTK_NVRAM_RW,                  DISP_STRING("OID_CUSTOM_MTK_NVRAM_RW"),             TRUE,   TRUE,   ENUM_OID_DRIVER_CORE,   sizeof(PARAM_CUSTOM_NVRAM_RW_STRUCT_T),    wlanoidQueryNvramRead,              wlanoidSetNvramWrite },
    { OID_CUSTOM_CFG_SRC_TYPE,                  DISP_STRING("OID_CUSTOM_CFG_SRC_TYPE"),             FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,   sizeof(ENUM_CFG_SRC_TYPE_T),               wlanoidQueryCfgSrcType,             NULL },
#endif
#if CFG_SUPPORT_WAPI
    { OID_802_11_WAPI_MODE,                     DISP_STRING("OID_802_11_WAPI_MODE"),                FALSE,  TRUE,   ENUM_OID_DRIVER_CORE,   4,                                          NULL,                               wf_oids_not_implemented_func,/*wlanoidSetWapiMode*/ },
    { OID_802_11_WAPI_ASSOC_INFO,               DISP_STRING("OID_802_11_WAPI_ASSOC_INFO"),          FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,   0,                                          NULL,                               wf_oids_not_implemented_func,/*wlanoidSetWapiAssocInfo*/ },
    { OID_802_11_SET_WAPI_KEY,                  DISP_STRING("OID_802_11_SET_WAPI_KEY"),             FALSE,  FALSE,  ENUM_OID_DRIVER_CORE,   4,/*sizeof(PARAM_WPI_KEY_T),*/                    NULL,                               wf_oids_not_implemented_func,/*wlanoidSetWapiKey*/ },
#endif
};

ULONG DSSS_Freq_Channel[] = {
	0,
	2412,
	2417,
	2422,
	2427,
	2432,
	2437,
	2442,
	2447,
	2452,
	2457,
	2462,
	2467,
	2472,
	2484
};


BOOLEAN
wf_req_search_supported_Oid_entry (
    IN  NDIS_OID            rOid,
    OUT P_WLAN_REQ_ENTRY    *ppWlanReqEntry
    )
{
    INT i, j, k;

    i = 0;
    j = NUM_SUPPORTED_OIDS - 1;

    while (i <= j) {
        k = (i + j) / 2;

        if (rOid == arWlanOidReqTable[k].rOid) {
            *ppWlanReqEntry = &arWlanOidReqTable[k];
            return TRUE;
        } else if (rOid < arWlanOidReqTable[k].rOid) {
            j = k - 1;
        } else {
            i = k + 1;
        }
    }

    return FALSE;
}   /* wf_req_search_supported_Oid_entry */

static wf_wlan_mgmt_scan_que_node_t *wf_find_scan_info_by_bssid(PADAPTER adapter, wf_u8* destBssid)
{
	nic_info_st *pnic_info = adapter->nic_info;
	wf_wlan_mgmt_scan_que_node_t *pscanned_info = NULL;
	wf_wlan_mgmt_scan_que_node_t *srch_end = NULL;
	wf_wlan_mgmt_scan_que_for_rst_e scanned_ret;
	/* Check if there is space for one more entry */
	wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscanned_info)
	{
		if(wf_memcmp(pscanned_info->bssid, destBssid, MAC_ADDR_LEN) == 0) {
			srch_end = pscanned_info;
			break;
		}
	}
	wf_wlan_mgmt_scan_que_for_end(scanned_ret);

	return srch_end;
}


/* Query */

NDIS_STATUS
wf_req_query_vendor_driver_version (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQryBuf,
	IN wf_u32 u4QryBufLen,
	OUT wf_u32* pu4QryInfoLen
	)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	LOG_D("Driver version: %d . %d", DRIVER_VERSION_MAJOR, DRIVER_VERSION_MINOR);

	*(wf_u32 *)pvQryBuf = ((wf_u16) DRIVER_VERSION_MAJOR << 16) +
                           (wf_u16) DRIVER_VERSION_MINOR;

	return ndisStatus;
}

NDIS_STATUS
wf_req_query_max_frame_size (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID	 pvQueryBuf,
	IN	wf_u32  u4QueryBufLen,
	OUT wf_u32* pu4QueryInfoLen
	)
{
	*pu4QueryInfoLen = sizeof(wf_u32);

	if (u4QueryBufLen < sizeof(wf_u32)) {
		return NDIS_STATUS_INVALID_LENGTH;
	}

	*(wf_u32*) pvQueryBuf = MP_802_11_MAX_FRAME_SIZE;

	return NDIS_STATUS_SUCCESS;

}

NDIS_STATUS
wf_req_query_mac_options (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQryBuf,
	IN wf_u32 u4QryBufLen,
	OUT wf_u32* pu4QryInfoLen
	)
{
	*pu4QryInfoLen = sizeof(wf_u32);

	if (u4QryBufLen < sizeof(wf_u32)) {
		return NDIS_STATUS_INVALID_LENGTH;
	}


	/* NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA is set to indicate to the
	   protocol that it can access the lookahead data by any means that
	   it wishes.  On some systems there are fast copy routines that
	   may have trouble accessing shared memory.  Netcard drivers that
	   indicate data out of shared memory, should not have this flag
	   set on these troublesome systems  For the time being this driver
	   will set this flag.	This should be safe because the data area
	   of the RFDs is contained in uncached memory. */

	/* NOTE: Don't set NDIS_MAC_OPTION_RECEIVE_SERIALIZED if we are
			 doing multipacket (ndis4) style receives. */


	*(wf_u32*) pvQryBuf = (wf_u32) (NDIS_MAC_OPTION_TRANSFERS_NOT_PEND  |
									  NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
									  NDIS_MAC_OPTION_NO_LOOPBACK);
	// TODO: Decide whether to add some optional properties.    2021/05/31
	
	return NDIS_STATUS_SUCCESS;

} /* end of wf_req_query_mac_options() */

NDIS_STATUS
wf_req_query_max_send_packets(
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQueryBuf,
	IN wf_u32 u4QueryBufLen,
	OUT wf_u32* pu4QueryInfoLen
	)
{
	*pu4QueryInfoLen = sizeof(wf_u32);

	if (u4QueryBufLen < sizeof(wf_u32)) {
		return NDIS_STATUS_INVALID_LENGTH;
	}
	
	*(wf_u32*) pvQueryBuf = MAX_ARRAY_SEND_PACKETS;

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
wf_req_query_max_list_size (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQryBuf,
	IN wf_u32 u4QryBufLen,
	OUT wf_u32* pu4QryInfoLen
	)
{
	*pu4QryInfoLen = sizeof(wf_u32);

	if (u4QryBufLen < sizeof(wf_u32)) {
		return NDIS_STATUS_INVALID_LENGTH;
	}


	*(wf_u32*) pvQryBuf = MAX_NUM_GROUP_ADDR;

	return NDIS_STATUS_SUCCESS;

}

NDIS_STATUS
wf_wlan_oid_query_cur_addr (
	IN  PADAPTER	pAdapter,
	IN	PVOID	 pvQueryBuffer,
	IN	wf_u32  u4QueryBufferLen,
	OUT wf_u32* pu4QueryInfoLen
	)
{

	if (u4QueryBufferLen < MAC_ADDR_LEN) {
		return NDIS_STATUS_INVALID_LENGTH;
	}
	NdisMoveMemory(pvQueryBuffer, pAdapter->PermanentAddress, MAC_ADDR_LEN);
	*pu4QueryInfoLen = MAC_ADDR_LEN;

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
wf_req_query_physical_medium (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQryBuf,
	IN wf_u32 u4QryBufLen,
	OUT wf_u32* pu4QryInfoLen
	)
{
    ASSERT(prGlueInfo);
    ASSERT(pu4QryInfoLen);

    *pu4QryInfoLen = sizeof(NDIS_PHYSICAL_MEDIUM);

    if (u4QryBufLen < sizeof(NDIS_PHYSICAL_MEDIUM)) {
        return NDIS_STATUS_BUFFER_TOO_SHORT;
    }

    ASSERT(pvQryBuf);

    *(PNDIS_PHYSICAL_MEDIUM) pvQryBuf = NdisPhysicalMediumWirelessLan;

    return NDIS_STATUS_SUCCESS;

}

NDIS_STATUS
wf_req_query_PnP_capabilities(
	IN P_GLUE_INFO_T prGlueInfo,
	IN PVOID		pvQueryBuf,
	IN wf_u32		u4QueryBufLen,
	OUT wf_u32*	pu4QueryInfoLen
	)
{
	PNDIS_PNP_CAPABILITIES prPwrMgtCap = (PNDIS_PNP_CAPABILITIES) pvQueryBuf;

	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(NDIS_PNP_CAPABILITIES);

	if (u4QueryBufLen < sizeof(NDIS_PNP_CAPABILITIES)) {
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQueryBuf);

#if CFG_ENABLE_WAKEUP_ON_LAN
	prPwrMgtCap->Flags = PARAM_DEVICE_WAKE_UP_ENABLE;

	prPwrMgtCap->WakeUpCapabilities.MinMagicPacketWakeUp =
		NdisDeviceStateD2;

	prPwrMgtCap->WakeUpCapabilities.MinPatternWakeUp =
		NdisDeviceStateD2;
#else
	prPwrMgtCap->Flags = 0;

	prPwrMgtCap->WakeUpCapabilities.MinMagicPacketWakeUp =
		ParamDeviceStateUnspecified;

	prPwrMgtCap->WakeUpCapabilities.MinPatternWakeUp =
		ParamDeviceStateUnspecified;
#endif

	prPwrMgtCap->WakeUpCapabilities.MinLinkChangeWakeUp =
		ParamDeviceStateUnspecified;

	return NDIS_STATUS_SUCCESS;

}

NDIS_STATUS
wf_req_query_supported_list (
	IN P_GLUE_INFO_T	prGlueInfo,
	OUT PVOID			pvQryBuf,
	IN wf_u32			u4QryBufLen,
	OUT wf_u32*		pu4QryInfoLen
	)
{
	UINT	i;

	*pu4QryInfoLen = NUM_SUPPORTED_OIDS * sizeof(NDIS_OID);

	LOG_D("buf_len=%d, need=%d", u4QryBufLen, *pu4QryInfoLen);

	/* Check if the query buffer is large enough to hold all the query
	   information. */
	if (u4QryBufLen < *pu4QryInfoLen) {
		/* Not enough room for the query information. */
		LOG_D("qry buffer length error! len=%d", u4QryBufLen);
		return NDIS_STATUS_INVALID_LENGTH;
	}

	for (i = 0; i < NUM_SUPPORTED_OIDS; i++) {
		*((PNDIS_OID) pvQryBuf + i) = arWlanOidReqTable[i].rOid;
	}

	return NDIS_STATUS_SUCCESS;
}	/* wf_req_query_supported_list */

NDIS_STATUS
wf_req_query_max_total_size (
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQueryBuf,
	IN wf_u32 u4QueryBufLen,
	OUT wf_u32* pu4QueryInfoLen
	)
{

	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(wf_u32);

	if (u4QueryBufLen < sizeof(wf_u32)) {
		return NDIS_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQueryBuf);

	*(wf_u32*) pvQueryBuf = DOT11_MAX_PDU_SIZE;

	return NDIS_STATUS_SUCCESS;

} 

NDIS_STATUS
wf_wlan_oid_query_link_speed(
	IN  PADAPTER	prAdapter,
	IN	PVOID	 pvQueryBuffer,
	IN	wf_u32  u4QueryBufferLen,
	OUT wf_u32* pu4QueryInfoLen
	)
{
	nic_info_st *pnic_info;
	mlme_info_t *pmlme_info;
	wdn_net_info_st *pwdn_info;
	enum NETWORK_TYPE cur_network_type;
	P_GLUE_INFO_T prGlueInfo;
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

	*pu4QueryInfoLen = sizeof(wf_u32);		
	*(wf_u32*)pvQueryBuffer = 0;
	prGlueInfo = (P_GLUE_INFO_T)prAdapter->parent;
	pnic_info = prAdapter->nic_info;
	pmlme_info = pnic_info->mlme_info;

	pwdn_info = pmlme_info->pwdn_info;
	if(prGlueInfo->eParamMediaStateIndicated != PARAM_MEDIA_STATE_CONNECTED)
	{
		*(wf_u32*)pvQueryBuffer = 10000;
		return ndisStatus;
	}

	if(pwdn_info == NULL) return ndisStatus;
	
	cur_network_type = pwdn_info->network_type;
	if(cur_network_type == WIRELESS_11B)
	{
		*(wf_u32*)pvQueryBuffer = 11 * 10000;
	}
	else if(cur_network_type >= WIRELESS_11G && cur_network_type < WIRELESS_11_24N)
	{
		*(wf_u32*)pvQueryBuffer = 54 * 10000;
	}
	else if(cur_network_type >= WIRELESS_11_24N)
	{
		*(wf_u32*)pvQueryBuffer = 150 * 10000;
	}
	return ndisStatus;
} 

NDIS_STATUS
wf_req_query_media_connect_status (
	IN P_GLUE_INFO_T	 prGlueInfo,
	OUT PVOID			 pvQryBuf,
	IN wf_u32			 u4QryBufLen,
	OUT wf_u32*		 pu4QryInfoLen
	)
{
	PADAPTER prAdapter;
	nic_info_st *pnic_info;
	
	*pu4QryInfoLen = sizeof(ENUM_PARAM_MEDIA_STATE_T);
	if (u4QryBufLen < sizeof(ENUM_PARAM_MEDIA_STATE_T)) {
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}

	/* Now we simply return our status (NdisMediaState[Dis]Connected) */
#if NDIS5_DBG
	if(prGlueInfo->eParamMediaStateIndicated == PARAM_MEDIA_STATE_CONNECTED){
		LOG_D("---CONNECTED");
		
	}else{
		LOG_D("---DISCONNECTED");		
	}
#endif
	*(P_ENUM_PARAM_MEDIA_STATE_T)pvQryBuf =
		prGlueInfo->eParamMediaStateIndicated;

	return NDIS_STATUS_SUCCESS;

}

NDIS_STATUS
wf_wlan_oid_query_xmit_ok (
	IN	PADAPTER prAdapter,
	IN	PVOID		pvQueryBuffer,
	IN	wf_u32 	u4QueryBufferLen,
	OUT wf_u32*	pu4QueryInfoLen
	)
{
	wf_mib_info_t *mib_info = prAdapter->mib_info;
	ASSERT(prAdapter);
	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}
	ASSERT(pu4QueryInfoLen);

	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in query receive error! (Adapter not ready).");
		*pu4QueryInfoLen = sizeof(wf_u32);
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}
	else if (u4QueryBufferLen < sizeof(wf_u32)
			|| (u4QueryBufferLen > sizeof(wf_u32) && u4QueryBufferLen < sizeof(wf_u64))) {
		LOG_D("Fail in query receive error! (Invalid length).");
		*pu4QueryInfoLen = sizeof(wf_u64);
		return NDIS_STATUS_INVALID_LENGTH;
	}

	if(u4QueryBufferLen == sizeof(wf_u32)){
		*pu4QueryInfoLen = sizeof(wf_u32);
        *(wf_u32*) pvQueryBuffer = (wf_u32)mib_info->num_xmit_ok.LowPart;
	}else{
		*pu4QueryInfoLen = sizeof(wf_u64);
		*(wf_u64*) pvQueryBuffer = (wf_u64) mib_info->num_xmit_ok.QuadPart;
	}
	return NDIS_STATUS_SUCCESS;
} 

NDIS_STATUS
wf_wlan_oid_query_xmit_error (
	IN	PADAPTER 	prAdapter,
	IN	PVOID			pvQueryBuffer,
	IN	wf_u32 		u4QueryBufferLen,
	OUT wf_u32*		pu4QueryInfoLen
	)
{
		
	wf_mib_info_t *mib_info;

	ASSERT(prAdapter);
	
	mib_info = prAdapter->mib_info;
	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}
	ASSERT(pu4QueryInfoLen);

	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in query receive error! (Adapter not ready).");
		*pu4QueryInfoLen = sizeof(wf_u32);
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}
	else if (u4QueryBufferLen < sizeof(wf_u32)
			|| (u4QueryBufferLen > sizeof(wf_u32) && u4QueryBufferLen < sizeof(wf_u64))) {
		LOG_D("Fail in query receive error! (Invalid length).");
		*pu4QueryInfoLen = sizeof(wf_u64);
		return NDIS_STATUS_INVALID_LENGTH;
	}

	if(u4QueryBufferLen == sizeof(wf_u32)){
		*pu4QueryInfoLen = sizeof(wf_u32);
		*(wf_u32*) pvQueryBuffer = (wf_u32) mib_info->num_xmit_error.LowPart;
	}else{
		*pu4QueryInfoLen = sizeof(wf_u64);
		*(wf_u64*) pvQueryBuffer = (wf_u64) mib_info->num_xmit_error.QuadPart;
	}
	return NDIS_STATUS_SUCCESS;
} 


NDIS_STATUS
wf_wlan_oid_query_rcv_ok (
	IN	PADAPTER 	prAdapter,
	IN	PVOID			pvQueryBuffer,
	IN	wf_u32 		u4QueryBufferLen,
	OUT wf_u32*		pu4QueryInfoLen
	)
{
	wf_mib_info_t *mib_info = prAdapter->mib_info;

	ASSERT(prAdapter);
	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}
	ASSERT(pu4QueryInfoLen);

	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in query receive error! (Adapter not ready).");
		*pu4QueryInfoLen = sizeof(wf_u32);
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}
	else if (u4QueryBufferLen < sizeof(wf_u32)
			|| (u4QueryBufferLen > sizeof(wf_u32) && u4QueryBufferLen < sizeof(wf_u64))) {
		LOG_D("Fail in query receive error! (Invalid length).");
		*pu4QueryInfoLen = sizeof(wf_u64);
		return NDIS_STATUS_INVALID_LENGTH;
	}
			
	if(u4QueryBufferLen == sizeof(wf_u32)){
		*pu4QueryInfoLen = sizeof(wf_u32);
		*(wf_u32*) pvQueryBuffer = (wf_u32)mib_info->num_recv_ok.LowPart;
	}else{
		*pu4QueryInfoLen = sizeof(wf_u64);
		*(wf_u64*) pvQueryBuffer = (wf_u64) mib_info->num_recv_ok.QuadPart;
	}
	return NDIS_STATUS_SUCCESS;
} 


NDIS_STATUS
wf_wlan_oid_query_rcv_error (
	IN	PADAPTER 	prAdapter,
	IN	PVOID			pvQueryBuffer,
	IN	wf_u32 		u4QueryBufferLen,
	OUT wf_u32*		pu4QueryInfoLen
	)
{
	wf_mib_info_t *mib_info;

	ASSERT(prAdapter);
	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}
	ASSERT(pu4QueryInfoLen);

	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in query receive error! (Adapter not ready).");
		*pu4QueryInfoLen = sizeof(wf_u32);
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}
	else if (u4QueryBufferLen < sizeof(wf_u32)
			|| (u4QueryBufferLen > sizeof(wf_u32) && u4QueryBufferLen < sizeof(wf_u64))) {
		LOG_D("Fail in query receive error! (Invalid length).");
		*pu4QueryInfoLen = sizeof(wf_u64);
		return NDIS_STATUS_INVALID_LENGTH;
	}

	mib_info = prAdapter->mib_info;
	if(u4QueryBufferLen == sizeof(wf_u32)){
		*pu4QueryInfoLen = sizeof(wf_u32);
		*(wf_u32*) pvQueryBuffer = (wf_u32) mib_info->num_recv_error.LowPart;
	}else{
		*pu4QueryInfoLen = sizeof(wf_u64);
		*(wf_u64*) pvQueryBuffer = (wf_u64) mib_info->num_recv_error.QuadPart;
	}
	return NDIS_STATUS_SUCCESS;
} 

NDIS_STATUS
wf_wlan_oid_query_auth_mode (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
	)
{
	wf_mib_info_t* mib_info = prAdapter->mib_info;
	ASSERT(prAdapter);
	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(ENUM_PARAM_AUTH_MODE_T);

	if (u4QueryBufferLen < sizeof(ENUM_PARAM_AUTH_MODE_T)) {
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}

	*(P_ENUM_PARAM_AUTH_MODE_T)pvQueryBuffer = mib_info->auth_mode;

#if NDIS5_DBG
	switch (*(P_ENUM_PARAM_AUTH_MODE_T)pvQueryBuffer) {
	case AUTH_MODE_OPEN:
		LOG_D("Current auth mode: Open");
		break;

	case AUTH_MODE_SHARED:
		LOG_D("Current auth mode: Shared");
		break;

	case AUTH_MODE_AUTO_SWITCH:
		LOG_D("Current auth mode: Auto-switch");
		break;

	case AUTH_MODE_WPA:
		LOG_D("Current auth mode: WPA");
		break;

	case AUTH_MODE_WPA_PSK:
		LOG_D("Current auth mode: WPA PSK");
		break;

	case AUTH_MODE_WPA_NONE:
		LOG_D("Current auth mode: WPA None");
		break;

	case AUTH_MODE_WPA2:
		LOG_D("Current auth mode: WPA2");
		break;

	case AUTH_MODE_WPA2_PSK:
		LOG_D("Current auth mode: WPA2 PSK");
		break;

	default:
		LOG_D("Current auth mode: %d",
			*(P_ENUM_PARAM_AUTH_MODE_T)pvQueryBuffer);
	}
#endif
	return NDIS_STATUS_SUCCESS;
} /* wf_wlan_oid_query_auth_mode */

NDIS_STATUS
wf_wlan_oid_query_infra_mode (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
	)
{
	ASSERT(prAdapter);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(ENUM_PARAM_OP_MODE_T);

	if (u4QueryBufferLen < sizeof(ENUM_PARAM_OP_MODE_T)) {
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}

	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}

	*(P_ENUM_PARAM_OP_MODE_T)pvQueryBuffer = prAdapter->Dot11RunningMode;
	if(prAdapter->Dot11RunningMode == NET_TYPE_INFRA)
	LOG_D("Running infrastructure.");
	if(prAdapter->Dot11RunningMode == NET_TYPE_IBSS)
	LOG_D("Running AdHoc.");
	return NDIS_STATUS_SUCCESS;
}	/* wf_wlan_oid_query_infra_mode */

NDIS_STATUS
wf_wlan_oid_query_enc_status (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
	)
{
	BOOLEAN 			  fgTransmitKeyAvailable = TRUE;
	ENUM_PARAM_ENCRYPTION_STATUS_T eEncStatus = 0;
	wf_mib_info_t* mib_info = prAdapter->mib_info;

	ASSERT(prAdapter);
	ASSERT(pu4QueryInfoLen);
	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}

	*pu4QueryInfoLen = sizeof(ENUM_PARAM_ENCRYPTION_STATUS_T);
	

	fgTransmitKeyAvailable = mib_info->fgTxBcKeyExist;

	switch (mib_info->eEncStatus) {
	case ENUM_ENCRYPTION3_ENABLED:
		if (fgTransmitKeyAvailable) {
			eEncStatus = ENUM_ENCRYPTION3_ENABLED;
		}
		else {
			eEncStatus = ENUM_ENCRYPTION3_KEY_ABSENT;
		}
		break;

	case ENUM_ENCRYPTION2_ENABLED:
		if (fgTransmitKeyAvailable) {
			eEncStatus = ENUM_ENCRYPTION2_ENABLED;
			break;
		}
		else {
			eEncStatus = ENUM_ENCRYPTION2_KEY_ABSENT;
		}
		break;

	case ENUM_ENCRYPTION1_ENABLED:
		if (fgTransmitKeyAvailable) {
			eEncStatus = ENUM_ENCRYPTION1_ENABLED;
		}
		else {
			eEncStatus = ENUM_ENCRYPTION1_KEY_ABSENT;
		}
		break;

	case ENUM_ENCRYPTION_DISABLED:
		eEncStatus = ENUM_ENCRYPTION_DISABLED;
		break;

	default:
		LOG_D("Unknown Encryption Status Setting:%d",
			mib_info->eEncStatus);
	}

#if NDIS5_DBG
	LOG_D("Encryption status: %d Return:%d",
		mib_info->eEncStatus, eEncStatus);
#endif

	*(P_ENUM_PARAM_ENCRYPTION_STATUS_T)pvQueryBuffer = eEncStatus;

	return NDIS_STATUS_SUCCESS;
} /* wf_wlan_oid_query_enc_status */

NDIS_STATUS
wf_wlan_oid_query_ssid (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
	)
{
	P_PARAM_SSID_T prAssociatedSsid;
	wf_mib_info_t* mib_info = prAdapter->mib_info;
	P_GLUE_INFO_T prGlueInfo = prAdapter->parent;

	ASSERT(prAdapter);
	ASSERT(pu4QueryInfoLen);

	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}

	*pu4QueryInfoLen = sizeof(PARAM_SSID_T);

	/* Check for query buffer length */
	if (u4QueryBufferLen < *pu4QueryInfoLen) {
		LOG_D("Invalid length %d", u4QueryBufferLen);
		return NDIS_STATUS_INVALID_LENGTH;
	}

	prAssociatedSsid = (P_PARAM_SSID_T)pvQueryBuffer;

	NdisZeroMemory(prAssociatedSsid->aucSsid, sizeof(prAssociatedSsid->aucSsid));
	

	// TODO: Initialize NIC module.
	
	if (wf_get_media_state_indicated(prGlueInfo) == PARAM_MEDIA_STATE_CONNECTED) {
		prAssociatedSsid->u4SsidLen = mib_info->curApInfo->rSsid.u4SsidLen;

		if (prAssociatedSsid->u4SsidLen) {
			NdisMoveMemory(prAssociatedSsid->aucSsid,
				mib_info->curApInfo->rSsid.aucSsid,
				prAssociatedSsid->u4SsidLen);
		}
	}
	else {
		prAssociatedSsid->u4SsidLen = 0;

		LOG_D("Null SSID");
	}

	return NDIS_STATUS_SUCCESS;
} /* wf_wlan_oid_query_ssid */

NDIS_STATUS
wf_wlan_oid_query_capability (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
	)
{
	P_PARAM_CAPABILITY_T  prCap;
	P_PARAM_AUTH_ENCRYPTION_T prAuthenticationEncryptionSupported;

	ASSERT(prAdapter);
	ASSERT(pu4QueryInfoLen);
	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}

	*pu4QueryInfoLen = 4 * sizeof(wf_u32) + 14 * sizeof(PARAM_AUTH_ENCRYPTION_T);

	if (u4QueryBufferLen < *pu4QueryInfoLen) {
		return NDIS_STATUS_INVALID_LENGTH;
	}

	prCap = (P_PARAM_CAPABILITY_T)pvQueryBuffer;

	prCap->u4Length = *pu4QueryInfoLen;
	prCap->u4Version = 2; /* WPA2 */
	prCap->u4NoOfPMKIDs = CFG_MAX_PMKID_CACHE;
	prCap->u4NoOfAuthEncryptPairsSupported = 14;

	prAuthenticationEncryptionSupported =
		&prCap->arAuthenticationEncryptionSupported[0];

	// fill 14 entries of supported settings
	prAuthenticationEncryptionSupported[0].eAuthModeSupported =
		AUTH_MODE_OPEN;

	prAuthenticationEncryptionSupported[0].eEncryptStatusSupported =
		ENUM_ENCRYPTION_DISABLED;

	prAuthenticationEncryptionSupported[1].eAuthModeSupported =
		AUTH_MODE_OPEN;
	prAuthenticationEncryptionSupported[1].eEncryptStatusSupported =
		ENUM_ENCRYPTION1_ENABLED;

	prAuthenticationEncryptionSupported[2].eAuthModeSupported =
		AUTH_MODE_SHARED;
	prAuthenticationEncryptionSupported[2].eEncryptStatusSupported =
		ENUM_ENCRYPTION_DISABLED;

	prAuthenticationEncryptionSupported[3].eAuthModeSupported =
		AUTH_MODE_SHARED;
	prAuthenticationEncryptionSupported[3].eEncryptStatusSupported =
		ENUM_ENCRYPTION1_ENABLED;

	prAuthenticationEncryptionSupported[4].eAuthModeSupported =
		AUTH_MODE_WPA;
	prAuthenticationEncryptionSupported[4].eEncryptStatusSupported =
		ENUM_ENCRYPTION2_ENABLED;

	prAuthenticationEncryptionSupported[5].eAuthModeSupported =
		AUTH_MODE_WPA;
	prAuthenticationEncryptionSupported[5].eEncryptStatusSupported =
		ENUM_ENCRYPTION3_ENABLED;

	prAuthenticationEncryptionSupported[6].eAuthModeSupported =
		AUTH_MODE_WPA_PSK;
	prAuthenticationEncryptionSupported[6].eEncryptStatusSupported =
		ENUM_ENCRYPTION2_ENABLED;

	prAuthenticationEncryptionSupported[7].eAuthModeSupported =
		AUTH_MODE_WPA_PSK;
	prAuthenticationEncryptionSupported[7].eEncryptStatusSupported =
		ENUM_ENCRYPTION3_ENABLED;

	prAuthenticationEncryptionSupported[8].eAuthModeSupported =
		AUTH_MODE_WPA_NONE;
	prAuthenticationEncryptionSupported[8].eEncryptStatusSupported =
		ENUM_ENCRYPTION2_ENABLED;

	prAuthenticationEncryptionSupported[9].eAuthModeSupported =
		AUTH_MODE_WPA_NONE;
	prAuthenticationEncryptionSupported[9].eEncryptStatusSupported =
		ENUM_ENCRYPTION3_ENABLED;

	prAuthenticationEncryptionSupported[10].eAuthModeSupported =
		AUTH_MODE_WPA2;
	prAuthenticationEncryptionSupported[10].eEncryptStatusSupported =
		ENUM_ENCRYPTION2_ENABLED;

	prAuthenticationEncryptionSupported[11].eAuthModeSupported =
		AUTH_MODE_WPA2;
	prAuthenticationEncryptionSupported[11].eEncryptStatusSupported =
		ENUM_ENCRYPTION3_ENABLED;

	prAuthenticationEncryptionSupported[12].eAuthModeSupported =
		AUTH_MODE_WPA2_PSK;
	prAuthenticationEncryptionSupported[12].eEncryptStatusSupported =
		ENUM_ENCRYPTION2_ENABLED;

	prAuthenticationEncryptionSupported[13].eAuthModeSupported =
		AUTH_MODE_WPA2_PSK;
	prAuthenticationEncryptionSupported[13].eEncryptStatusSupported =
		ENUM_ENCRYPTION3_ENABLED;

	return NDIS_STATUS_SUCCESS;

} /* wf_wlan_oid_query_capability */

NDIS_STATUS
wf_wlan_oid_query_bssid_list (
	IN	PADAPTER	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32		  u4QueryBufferLen,
	OUT wf_u32* 	  pu4QueryInfoLen
	)
{
	P_GLUE_INFO_T prGlueInfo;
	wf_u32 u4BssidListExLen;
	P_PARAM_BSSID_LIST_EX_T prList;
	P_PARAM_BSSID_EX_T prBssidEx;
	wf_u8* cp;
	wf_u32 idx;
	wf_wlan_mgmt_scan_que_node_t *pscanned_info;
	wf_mib_info_t* mib_info = prAdapter->mib_info;
	
	ASSERT(prAdapter);
	ASSERT(pu4QueryInfoLen);

	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);

		if(!pvQueryBuffer) {
			return NDIS_STATUS_INVALID_DATA;
		}
	}
	prGlueInfo = (P_GLUE_INFO_T)prAdapter->parent;

	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in set Authentication mode! (Adapter not ready). ");
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}

	WdfSpinLockAcquire(mib_info->bss_lock);

	u4BssidListExLen = 0;
	for(idx=0; idx < mib_info->bss_cnt; idx++)
	{
		pscanned_info = (wf_wlan_mgmt_scan_que_node_t*) mib_info->bss_node[idx];
		u4BssidListExLen += (sizeof(PARAM_BSSID_EX_T) + pscanned_info->ie_len - 13);// Minus (12 + 1)
	}	
	if(u4BssidListExLen) {
		u4BssidListExLen += 4; // u4NumberOfItems.
	}
	else {
		u4BssidListExLen = sizeof(PARAM_BSSID_LIST_EX_T);
	}
	*pu4QueryInfoLen = u4BssidListExLen;
	if (u4QueryBufferLen < *pu4QueryInfoLen) {
        return NDIS_STATUS_INVALID_LENGTH;
    }

	prList = (P_PARAM_BSSID_LIST_EX_T) pvQueryBuffer;
	cp = (wf_u8*)&prList->arBssid[0];
	for(idx=0; idx < mib_info->bss_cnt; idx++) {
		pscanned_info = (wf_wlan_mgmt_scan_que_node_t*) mib_info->bss_node[idx];
		prBssidEx = (P_PARAM_BSSID_EX_T)cp;	

		wf_memcpy(prBssidEx->arMacAddress, pscanned_info->bssid, MAC_ADDR_LEN);
		prBssidEx->rSsid.u4SsidLen = pscanned_info->ssid.length;
		wf_memcpy(prBssidEx->rSsid.aucSsid, pscanned_info->ssid.data , pscanned_info->ssid.length);
		prBssidEx->u4Privacy = pscanned_info->privacy;
		prBssidEx->rRssi = pscanned_info->signal_strength - 100;
		prBssidEx->eNetworkTypeInUse = PARAM_NETWORK_TYPE_AUTOMODE;
		prBssidEx->rConfiguration.u4Length = sizeof(PARAM_802_11_CONFIG_T);
		prBssidEx->rConfiguration.u4DSConfig = DSSS_Freq_Channel[pscanned_info->channel];
		prBssidEx->eOpMode = pscanned_info->opr_mode == WF_WLAN_OPR_MODE_ADHOC ? 
			NET_TYPE_IBSS : NET_TYPE_INFRA;
		wf_memcpy(prBssidEx->rSupportedRates, prBssidEx->rSupportedRates, PARAM_MAX_LEN_RATES_EX);
		prBssidEx->u4IELength = pscanned_info->ie_len;
		wf_memcpy(prBssidEx->aucIEs, pscanned_info->ies, prBssidEx->u4IELength);
		prBssidEx->u4Length = sizeof(PARAM_BSSID_EX_T) - 1 + prBssidEx->u4IELength;
		
		if(prBssidEx->rRssi > PARAM_WHQL_RSSI_MAX_DBM) {
			prBssidEx->rRssi = PARAM_WHQL_RSSI_MAX_DBM;
		}
		cp += prBssidEx->u4Length;
		prList->u4NumberOfItems++;
	}
	WdfSpinLockRelease(mib_info->bss_lock);

	return NDIS_STATUS_SUCCESS;
} /* wf_wlan_oid_query_bssid_list */

NDIS_STATUS
wf_wlan_oid_query_bssid (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
	)
{
	P_GLUE_INFO_T prGlueInfo;
	wf_mib_info_t* mib_info;
	P_PARAM_BSSID_EX_T 			curApInfo;

	NDIS_STATUS rStatus = NDIS_STATUS_SUCCESS;

	prGlueInfo = (P_GLUE_INFO_T)prAdapter->parent;
	mib_info = (wf_mib_info_t*) prAdapter->mib_info;
	curApInfo = (P_PARAM_BSSID_EX_T)mib_info->curApInfo;

	if (u4QueryBufferLen < MAC_ADDR_LEN) {
		ASSERT(pu4QueryInfoLen);
		*pu4QueryInfoLen = MAC_ADDR_LEN;
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(u4QueryBufferLen >= MAC_ADDR_LEN);
	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}
	ASSERT(pu4QueryInfoLen);

	if(wf_get_media_state_indicated(prGlueInfo) == PARAM_MEDIA_STATE_CONNECTED) {
		wf_memcpy(pvQueryBuffer, curApInfo->arMacAddress, MAC_ADDR_LEN);
	}
	else {
		rStatus = NDIS_STATUS_ADAPTER_NOT_READY;
	}

	*pu4QueryInfoLen = MAC_ADDR_LEN;
	return rStatus;
} /* wf_wlan_oid_query_bssid */

NDIS_STATUS
wf_wlan_oid_query_rssi (
	IN	PADAPTER 	  prAdapter,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
	)
{
	P_GLUE_INFO_T prGlueInfo;
	wf_mib_info_t *mib_info;
	wf_s32	rRssi;
	ASSERT(prAdapter);
	ASSERT(pu4QueryInfoLen);

	prGlueInfo = (P_GLUE_INFO_T)prAdapter->parent;
	mib_info = (wf_mib_info_t *)prAdapter->mib_info;
	
	if (u4QueryBufferLen) {
		ASSERT(pvQueryBuffer);
	}

	*pu4QueryInfoLen = sizeof(INT32);

	/* Check for query buffer length */
	if (u4QueryBufferLen < *pu4QueryInfoLen) {
		LOG_D("Too short length %ld\n", u4QueryBufferLen);
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}

	if (wf_get_media_state_indicated(prGlueInfo) != PARAM_MEDIA_STATE_CONNECTED) {
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}
	else {
		rRssi = mib_info->rRssi - 100; // ranged from (-128 ~ 30) in unit of dBm

		if(rRssi > PARAM_WHQL_RSSI_MAX_DBM)
			rRssi = PARAM_WHQL_RSSI_MAX_DBM;
		else if(rRssi < PARAM_WHQL_RSSI_MIN_DBM)
			rRssi = PARAM_WHQL_RSSI_MIN_DBM;

		wf_memcpy(pvQueryBuffer, &rRssi, sizeof(wf_s32));
		return NDIS_STATUS_SUCCESS;
	}
	return NDIS_STATUS_SUCCESS;
} /* end of wf_wlan_oid_query_rssi() */

NDIS_STATUS
wf_req_query_assoc_info (
	IN	P_GLUE_INFO_T	  prGlueInfo,
	OUT PVOID			  pvQueryBuffer,
	IN	wf_u32 		  u4QueryBufferLen,
	OUT wf_u32*		  pu4QueryInfoLen
	)
{
	wf_u8* pQueryBuff = (wf_u8*)pvQueryBuffer;
#if NDIS5_DBG
	PNDIS_802_11_ASSOCIATION_INFORMATION prAssocInfo =
		(PNDIS_802_11_ASSOCIATION_INFORMATION) pvQueryBuffer;
	PUINT8 cp;
#endif

	*pu4QueryInfoLen = sizeof(NDIS_802_11_ASSOCIATION_INFORMATION) +
					   prGlueInfo->rNdisAssocInfo.RequestIELength +
					   prGlueInfo->rNdisAssocInfo.ResponseIELength;

	if (u4QueryBufferLen < *pu4QueryInfoLen) {
		LOG_D("Query assoc info failed for invalid length.");
		return NDIS_STATUS_INVALID_LENGTH;
	}

	prGlueInfo->rNdisAssocInfo.OffsetRequestIEs = sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);
	prGlueInfo->rNdisAssocInfo.OffsetResponseIEs = sizeof(NDIS_802_11_ASSOCIATION_INFORMATION)
						+ prGlueInfo->aucAssocReqIE_Len;
	wf_memcpy(pQueryBuff,
			   (PVOID)&prGlueInfo->rNdisAssocInfo,
			   sizeof(NDIS_802_11_ASSOCIATION_INFORMATION));
	wf_memcpy(pQueryBuff + (prGlueInfo->rNdisAssocInfo.OffsetRequestIEs),
				prGlueInfo->aucAssocReqIE,
				prGlueInfo->aucAssocReqIE_Len);
	wf_memcpy(pQueryBuff + (prGlueInfo->rNdisAssocInfo.OffsetResponseIEs),
				prGlueInfo->aucAssocRespIE,
				prGlueInfo->aucAssocRespIE_Len);

#if NDIS5_DBG
	/* Dump the PARAM_ASSOCIATION_INFORMATION content. */
	LOG_D("QUERY: Assoc Info - Length: %d\n", prAssocInfo->Length);

	LOG_D("AvailableRequestFixedIEs: 0x%04x\n",
		prAssocInfo->AvailableRequestFixedIEs);
	LOG_D("Request Capabilities: 0x%04x\n",
		prAssocInfo->RequestFixedIEs.Capabilities);
	LOG_D("Request Listen Interval: 0x%04x\n",
		prAssocInfo->RequestFixedIEs.ListenInterval);
	cp = (wf_u8*) &prAssocInfo->RequestFixedIEs.CurrentAPAddress;
	LOG_D("CurrentAPAddress: %02x-%02x-%02x-%02x-%02x-%02x\n",
		cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]);
	LOG_D("Request IEs: length=%d, offset=%d\n",
		prAssocInfo->RequestIELength, prAssocInfo->OffsetRequestIEs);

	cp = (wf_u8*) pvQueryBuffer + sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);
	LOG_D("RequestIELength:%d", prAssocInfo->RequestIELength);
	cp += prAssocInfo->RequestIELength;

	LOG_D("AvailableResponseFixedIEs: 0x%04x\n",
		prAssocInfo->AvailableResponseFixedIEs);
	LOG_D("Response Capabilities: 0x%04x\n",
		prAssocInfo->ResponseFixedIEs.Capabilities);
	LOG_D("StatusCode: 0x%04x\n",
		prAssocInfo->ResponseFixedIEs.StatusCode);
	LOG_D("AssociationId: 0x%04x\n",
		prAssocInfo->ResponseFixedIEs.AssociationId);
	LOG_D("Response IEs: length=%d, offset=%d\n",
		prAssocInfo->ResponseIELength, prAssocInfo->OffsetResponseIEs);
#endif

	return NDIS_STATUS_SUCCESS;

} /* end of wf_req_query_assoc_info() */

NDIS_STATUS
wf_req_query_media(
	IN P_GLUE_INFO_T prGlueInfo,
	OUT PVOID pvQryBuf,
	IN wf_u32 u4QryBufLen,
	OUT wf_u32* pu4QryInfoLen
	)
{
	*pu4QryInfoLen = sizeof(PNDIS_MEDIUM);

	if (u4QryBufLen < sizeof(PNDIS_MEDIUM)) {
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}

	*(PNDIS_MEDIUM) pvQryBuf = NdisMedium802_3;

	return NDIS_STATUS_SUCCESS;

} /* end of reqQueryMedia() */

NDIS_STATUS
wf_wlan_oid_query_cur_pkt_filter (
	IN PADAPTER	prAdapter,
	OUT PVOID		pvQueryBuffer,
	IN	wf_u32 	u4QueryBufferLen,
	OUT wf_u32*	pu4QueryInfoLen
	)
{
	*pu4QueryInfoLen = sizeof(wf_u32);

	if (u4QueryBufferLen >= sizeof(wf_u32)) {
		*(wf_u32*) pvQueryBuffer = prAdapter->PacketFilter;
	}

	return NDIS_STATUS_SUCCESS;
}	/* wf_wlan_oid_query_cur_pkt_filter */


/* Set */
	
NDIS_STATUS
wf_wlan_oid_set_auth_mode (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
	)
{
	P_GLUE_INFO_T prGlueInfo;

	wf_mib_info_t* mib_info = prAdapter->mib_info;

	prGlueInfo = (P_GLUE_INFO_T)prAdapter->parent;

	*pu4SetInfoLen = sizeof(ENUM_PARAM_AUTH_MODE_T);

	if (u4SetBufferLen < sizeof(ENUM_PARAM_AUTH_MODE_T)) {
		return NDIS_STATUS_INVALID_LENGTH;
	}

	/* RF Test */
	//if (IS_ARB_IN_RFTEST_STATE(prAdapter)) {
	//	return NDIS_STATUS_SUCCESS;
	//}

	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in set Authentication mode! (Adapter not ready). ");
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}

	/* Check if the new authentication mode is valid. */
	if (*(P_ENUM_PARAM_AUTH_MODE_T)pvSetBuffer >= AUTH_MODE_NUM) {
		LOG_D("Invalid auth mode %d", *(P_ENUM_PARAM_AUTH_MODE_T)pvSetBuffer);
		return NDIS_STATUS_INVALID_DATA;
	}

	switch (*(P_ENUM_PARAM_AUTH_MODE_T)pvSetBuffer) {
	case AUTH_MODE_WPA:
	case AUTH_MODE_WPA_PSK:
	case AUTH_MODE_WPA2:
	case AUTH_MODE_WPA2_PSK:
		/* infrastructure mode only */
		if (prAdapter->Dot11RunningMode != NET_TYPE_INFRA) {
			return NDIS_STATUS_NOT_ACCEPTED;
		}
		break;

	case AUTH_MODE_WPA_NONE:
		/* ad hoc mode only */
		if (prAdapter->Dot11RunningMode != NET_TYPE_IBSS) {
			return NDIS_STATUS_NOT_ACCEPTED;
		}
		break;

	default:
		;
	}

	/* Save the new authentication mode. */
	mib_info->auth_mode = *(P_ENUM_PARAM_AUTH_MODE_T)pvSetBuffer;

#if NDIS5_DBG
	switch (mib_info->auth_mode) {
	case AUTH_MODE_OPEN:
		LOG_D("New auth mode: open");
		break;

	case AUTH_MODE_SHARED:
		LOG_D("New auth mode: shared");
		break;

	case AUTH_MODE_AUTO_SWITCH:
		LOG_D("New auth mode: auto-switch");
		break;

	case AUTH_MODE_WPA:
		LOG_D("New auth mode: WPA");
		break;

	case AUTH_MODE_WPA_PSK:
		LOG_D("New auth mode: WPA PSK");
		break;

	case AUTH_MODE_WPA_NONE:
		LOG_D("New auth mode: WPA None");
		break;

	case AUTH_MODE_WPA2:
		LOG_D("New auth mode: WPA2");
		break;

	case AUTH_MODE_WPA2_PSK:
		LOG_D("New auth mode: WPA2 PSK");
		break;

	default:
		LOG_D("New auth mode: unknown (%d)",
			mib_info->auth_mode);
	}
#endif


	return NDIS_STATUS_SUCCESS;

} /* wf_wlan_oid_set_auth_mode */

NDIS_STATUS
wf_wlan_oid_set_infra_mode (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
	)
{
	ASSERT(prAdapter);
	ASSERT(pvSetBuffer);
	ASSERT(pu4SetInfoLen);

	if (u4SetBufferLen < sizeof(ENUM_PARAM_OP_MODE_T))
		return NDIS_STATUS_BUFFER_TOO_SHORT;

	*pu4SetInfoLen = sizeof(ENUM_PARAM_OP_MODE_T);


	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in set Infrastructure mode! (Adapter not ready). ");
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}
	
	prAdapter->Dot11RunningMode = *(P_ENUM_PARAM_OP_MODE_T)pvSetBuffer;;

	// TODO: Reset work. Don't reset cipher info.   2021/06/03
	return NDIS_STATUS_SUCCESS;
}  

NDIS_STATUS
wf_wlan_oid_set_enc_status (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
	)
{
	P_GLUE_INFO_T		  prGlueInfo;
	NDIS_STATUS 		  rStatus = NDIS_STATUS_SUCCESS;
	ENUM_PARAM_ENCRYPTION_STATUS_T eEewEncrypt;

	ASSERT(prAdapter);
	ASSERT(pvSetBuffer);
	ASSERT(pu4SetInfoLen);

	prGlueInfo = (P_GLUE_INFO_T)prAdapter->parent;

	*pu4SetInfoLen = sizeof(ENUM_PARAM_ENCRYPTION_STATUS_T);

	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in set Authentication mode! (Adapter not ready). ");
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}

	eEewEncrypt = *(P_ENUM_PARAM_ENCRYPTION_STATUS_T)pvSetBuffer;
	LOG_D("ENCRYPTION_STATUS %d", eEewEncrypt);

	switch (eEewEncrypt) {
	case ENUM_ENCRYPTION_DISABLED: /* Disable WEP, TKIP, AES */
		LOG_D("Disable Encryption");
		wf_sec_set_cipher_suite(prAdapter,
			CIPHER_FLAG_WEP40  |
			CIPHER_FLAG_WEP104 |
			CIPHER_FLAG_WEP128);
		break;

	case ENUM_ENCRYPTION1_ENABLED: /* Enable WEP. Disable TKIP, AES */
		LOG_D("Enable Encryption1");
		wf_sec_set_cipher_suite(prAdapter,
			CIPHER_FLAG_WEP40  |
			CIPHER_FLAG_WEP104 |
			CIPHER_FLAG_WEP128);
		break;

	case ENUM_ENCRYPTION2_ENABLED: /* Enable WEP, TKIP. Disable AES */
		wf_sec_set_cipher_suite(prAdapter,
			CIPHER_FLAG_WEP40  |
			CIPHER_FLAG_WEP104 |
			CIPHER_FLAG_WEP128 |
			CIPHER_FLAG_TKIP);
		LOG_D("Enable Encryption2");
		break;

	case ENUM_ENCRYPTION3_ENABLED: /* Enable WEP, TKIP, AES */
		wf_sec_set_cipher_suite(prAdapter,
			CIPHER_FLAG_WEP40  |
			CIPHER_FLAG_WEP104 |
			CIPHER_FLAG_WEP128 |
			CIPHER_FLAG_TKIP |
			CIPHER_FLAG_CCMP);
		LOG_D("Enable Encryption3");
		break;

	default:
		LOG_D("Unacceptible encryption status: %d",
			*(P_ENUM_PARAM_ENCRYPTION_STATUS_T)pvSetBuffer);

		rStatus = NDIS_STATUS_NOT_SUPPORTED;
	}

	if (rStatus == NDIS_STATUS_SUCCESS) {
		/* Save the new encryption status. */
		wf_mib_info_t* mib_info = prAdapter->mib_info;
		mib_info->eEncStatus =
			*(P_ENUM_PARAM_ENCRYPTION_STATUS_T)pvSetBuffer;
	}

	return rStatus;
} 

NDIS_STATUS
wf_wlan_oid_set_ssid (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
	)
{
	P_GLUE_INFO_T prGlueInfo;
	P_PARAM_SSID_T pParamSsid;
	wf_mib_info_t *mib_info;
	P_PARAM_BSSID_EX_T curApInfo;
	wf_u32 i;
	NDIS_STATUS  ndisStatus = NDIS_STATUS_SUCCESS;	
	prGlueInfo = (P_GLUE_INFO_T)prAdapter->parent;
	mib_info = prAdapter->mib_info;
	curApInfo = mib_info->curApInfo;
	
	if(u4SetBufferLen < sizeof(PARAM_SSID_T) || u4SetBufferLen > sizeof(PARAM_SSID_T)) {
        return NDIS_STATUS_INVALID_LENGTH;
    }
	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in set SSID! (Adapter not ready). ");
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}
	pParamSsid = (P_PARAM_SSID_T) pvSetBuffer;
    if (pParamSsid->u4SsidLen > PARAM_MAX_LEN_SSID + 1) {
        return NDIS_STATUS_INVALID_LENGTH;
    }

	/**
	* NDIS automatically sends incorrect SSID sometimes.
	*/
	if(pParamSsid->u4SsidLen == 32)
	{
		if(wf_check_ssid_valid(prAdapter, pParamSsid) == wf_false)
		{
			LOG_D("Setting invalid SSID.");
			return NDIS_STATUS_INVALID_DATA;
		}
	}
	
	LOG_D("Set SSID.");
#if NDIS5_DBG
	DbgPrint("SSID Length: %d, SSID:", pParamSsid->u4SsidLen);
	for(i = 0; i < pParamSsid->u4SsidLen; i++) DbgPrint("%c", pParamSsid->aucSsid[i]);
	DbgPrint("\n");
#endif
	if(wf_get_media_state_indicated(prGlueInfo) == PARAM_MEDIA_STATE_CONNECTED) {
		if(!(EQUAL_SSID(curApInfo->rSsid.aucSsid,
                    curApInfo->rSsid.u4SsidLen,
                    pParamSsid->aucSsid,
                    pParamSsid->u4SsidLen)))
        {
        	LOG_D("Deauth to connect to another AP.");
			wf_set_start_deassoc(prAdapter);
		 	ndisStatus = wf_set_start_assoc(prAdapter, pParamSsid);
			if ((ndisStatus == NDIS_STATUS_SUCCESS))
		    {
				curApInfo->rSsid.u4SsidLen = pParamSsid->u4SsidLen;
				wf_memcpy(curApInfo->rSsid.aucSsid, pParamSsid->aucSsid, pParamSsid->u4SsidLen);
		    }
		}
	}
	else{
		ndisStatus = wf_set_start_assoc(prAdapter, pParamSsid);
		if ((ndisStatus == NDIS_STATUS_SUCCESS))
	    {
			curApInfo->rSsid.u4SsidLen = pParamSsid->u4SsidLen;
			wf_memcpy(curApInfo->rSsid.aucSsid, pParamSsid->aucSsid, pParamSsid->u4SsidLen);
	    }
	}
	return NDIS_STATUS_SUCCESS;
} /* end of wf_wlan_oid_set_ssid() */

NDIS_STATUS
wf_wlan_oid_set_bssid_list_scan (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
	)
{
	nic_info_st *nic_info = prAdapter->nic_info;
	mlme_state_e state;
	NDIS_STATUS  ndisStatus = NDIS_STATUS_SUCCESS;

	wf_mlme_get_state(nic_info, &state);

	if(state == MLME_STATE_SCAN){
		LOG_D("Another scan request is processing.");
		return ndisStatus;
	}

	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in set Authentication mode! (Adapter not ready). ");
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}

	ASSERT(pu4SetInfoLen);
	*pu4SetInfoLen = 0;

	LOG_D("********* SCANNING FOR BSS **********");
    if(prAdapter->bRequestedScan == TRUE) {
		LOG_D("Another scan request is processing.");
		return ndisStatus;
	}
	prAdapter->bRequestedScan = TRUE;
	//prAdapter->CurrentRequestID_Scan = prAdapter->PendedRequest;
    
	ndisStatus = wf_set_scan(prAdapter);

	//return ndisStatus;
	return NDIS_STATUS_SUCCESS;
} /* wf_wlan_oid_set_bssid_list_scan */

NDIS_STATUS
wf_wlan_oid_set_bssid (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
	)
{
	P_GLUE_INFO_T prGlueInfo;
	wf_u8* pAddr;
	wf_mib_info_t* mib_info;
	P_PARAM_BSSID_EX_T curApInfo;
	wf_wlan_mgmt_scan_que_node_t *scan_info;
	P_PARAM_SSID_T pParamSsid;
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	*pu4SetInfoLen = MAC_ADDR_LEN;;
	if (u4SetBufferLen != MAC_ADDR_LEN){
		*pu4SetInfoLen = MAC_ADDR_LEN;
		return NDIS_STATUS_INVALID_LENGTH;
	}
	else if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in setting BSSID! (Adapter not ready).");
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}
	
	prGlueInfo = prAdapter->parent;
	mib_info = prAdapter->mib_info;
	curApInfo = mib_info->curApInfo;
	pAddr = (wf_u8*)pvSetBuffer;

	if(wf_is_broadcast(pAddr) == TRUE){
		prAdapter->bBssidLocked = FALSE;
		return NDIS_STATUS_SUCCESS;
	}
	else
	{
		prAdapter->bBssidLocked = TRUE;
		wf_memcpy(prAdapter->SpecifiedMacAddr, pAddr, MAC_ADDR_LEN);
	}

	scan_info = wf_find_scan_info_by_bssid(prAdapter, pAddr);
	if(scan_info == NULL) {
		LOG_E("wf_find_scan_info_by_ssid fail\n");
 		return NDIS_STATUS_FAILURE; 
	}
	pParamSsid = &scan_info->ssid;
	
	if(wf_get_media_state_indicated(prGlueInfo) == PARAM_MEDIA_STATE_CONNECTED) {
		if(!(EQUAL_SSID(curApInfo->rSsid.aucSsid,
                    curApInfo->rSsid.u4SsidLen,
                    pParamSsid->aucSsid,
                    pParamSsid->u4SsidLen)))
        {
			wf_set_start_deassoc(prAdapter);

		 	ndisStatus = wf_set_start_assoc(prAdapter, pParamSsid);
			if ((ndisStatus == NDIS_STATUS_SUCCESS))
		    {
				curApInfo->rSsid.u4SsidLen = pParamSsid->u4SsidLen;
				wf_memcmp(curApInfo->rSsid.aucSsid, pParamSsid->aucSsid, pParamSsid->u4SsidLen);
		    }
		}
	}
	else{
		ndisStatus = wf_set_start_assoc(prAdapter, pParamSsid);
		if ((ndisStatus == NDIS_STATUS_SUCCESS))
	    {
			curApInfo->rSsid.u4SsidLen = pParamSsid->u4SsidLen;
			wf_memcmp(curApInfo->rSsid.aucSsid, pParamSsid->aucSsid, pParamSsid->u4SsidLen);
	    }
	}

	return ndisStatus;
} /* end of wf_wlan_oid_set_bssid() */


NDIS_STATUS
wf_wlan_oid_set_add_key(
	IN	PADAPTER 	  prAdapter,
	IN	PVOID	 pvSetBuffer,
	IN	wf_u32  u4SetBufferLen,
	OUT wf_u32* pu4SetInfoLen
)
{
	P_GLUE_INFO_T prGlueInfo;
	P_WF_NDIS_802_11_KEY_T prSettingKey;
	BOOLEAN  bPairwiseKey;
	if (prAdapter->dev_state != WF_DEV_STATE_RUN)
	{
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}

	prGlueInfo = (P_GLUE_INFO_T)prAdapter->parent;

	prSettingKey = (P_WF_NDIS_802_11_KEY_T)pvSetBuffer;

	if(((prSettingKey->arBSSID[0] == 0xff) && (prSettingKey->arBSSID[1] == 0xff) && (prSettingKey->arBSSID[2] == 0xff) &&
				(prSettingKey->arBSSID[3] == 0xff) && (prSettingKey->arBSSID[4] == 0xff) && (prSettingKey->arBSSID[5] == 0xff)))
	{
		if(prGlueInfo->bWaitGroupKeyState == FALSE)
		{
			return NDIS_STATUS_SUCCESS;
		}
	}
	/* Verify the key structure length. */
	if (prSettingKey->u4Length > u4SetBufferLen) {
		LOG_D("Invalid key structure length (%d) greater than total buffer length (%d)",
			(wf_u8)prSettingKey->u4Length,
			(wf_u8)u4SetBufferLen);
		*pu4SetInfoLen = u4SetBufferLen;
		return NDIS_STATUS_INVALID_LENGTH;
	}

	/* Verify the key material length for key material buffer */
	if (prSettingKey->u4KeyLength > prSettingKey->u4Length - WF_OFFSETOF(WF_NDIS_802_11_KEY_T, aucKeyMaterial)) {
		LOG_D("Invalid key material length:%d", (wf_u8)prSettingKey->u4KeyLength);
		*pu4SetInfoLen = u4SetBufferLen;
		return NDIS_STATUS_INVALID_DATA;
	}

	/* Exception check */
	if (prSettingKey->u4KeyIndex & 0x0fffff00) {
		return NDIS_STATUS_INVALID_DATA;
	}

	if (prSettingKey->u4KeyIndex & BIT(28)) {
		LOG_E("Error! Key from authenticator.");
	}

	/* Exception check, pairwise key must with transmit bit enabled */
	if ((prSettingKey->u4KeyIndex & BITS(30, 31)) == IS_UNICAST_KEY) {
		return NDIS_STATUS_INVALID_DATA;
	}

	if (!(prSettingKey->u4KeyLength == WEP_40_LEN || prSettingKey->u4KeyLength == WEP_104_LEN ||
		prSettingKey->u4KeyLength == CCMP_KEY_LEN || prSettingKey->u4KeyLength == TKIP_KEY_LEN))
	{
		return NDIS_STATUS_INVALID_DATA;
	}

	/* Exception check, pairwise key must with transmit bit enabled */
	if ((prSettingKey->u4KeyIndex & BITS(30, 31)) == BITS(30, 31)) {
		if (((prSettingKey->u4KeyIndex & 0xff) != 0) ||
			((prSettingKey->arBSSID[0] == 0xff) && (prSettingKey->arBSSID[1] == 0xff) && (prSettingKey->arBSSID[2] == 0xff) &&
				(prSettingKey->arBSSID[3] == 0xff) && (prSettingKey->arBSSID[4] == 0xff) && (prSettingKey->arBSSID[5] == 0xff))) {
			return NDIS_STATUS_INVALID_DATA;
		}
	}

#if NDIS5_DBG
	LOG_D("Set: Dump PARAM_KEY content");
	LOG_D("Length    : %d", prSettingKey->u4Length);
	LOG_D("Key Index : 0x%08lx", prSettingKey->u4KeyIndex);
	LOG_D("Key Length: %d", prSettingKey->u4KeyLength);
	LOG_D("BSSID: %02x-%02x-%02x-%02x-%02x-%02x", prSettingKey->arBSSID[0], prSettingKey->arBSSID[1],
		prSettingKey->arBSSID[2], prSettingKey->arBSSID[3], prSettingKey->arBSSID[4], prSettingKey->arBSSID[5]);
#endif
	bPairwiseKey = (prSettingKey->u4KeyIndex & BIT(30)) ? TRUE : FALSE;
	wf_set_nic_key(prAdapter, prSettingKey, bPairwiseKey);
	WdfWorkItemEnqueue(prAdapter->writeKeyWorkitem);

	return NDIS_STATUS_SUCCESS;
} /* wf_wlan_oid_set_add_key */

NDIS_STATUS
wf_wlan_oid_set_reload_defaults (
	IN	PADAPTER 	  prAdapter,
	IN	PVOID			  pvSetBuffer,
	IN	wf_u32 		  u4SetBufferLen,
	OUT wf_u32*		  pu4SetInfoLen
	)
{
	NDIS_STATUS rStatus = NDIS_STATUS_SUCCESS;
	ENUM_PARAM_NETWORK_TYPE_T eNetworkType;
	wf_u32 u4Len;
	wf_u8 ucCmdSeqNum;

	ASSERT(prAdapter);
	ASSERT(pu4SetInfoLen);
	*pu4SetInfoLen = sizeof(PARAM_RELOAD_DEFAULTS);

	//if (IS_ARB_IN_RFTEST_STATE(prAdapter)) {
	//	return WLAN_STATUS_SUCCESS;
	//}

	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in set ReloafDefault! (Adapter not ready). ");
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}

	ASSERT(pvSetBuffer);
	/* Verify the available reload options and reload the settings. */
	switch (*(P_PARAM_RELOAD_DEFAULTS)pvSetBuffer) {
	case ENUM_RELOAD_WEP_KEYS:
		// TODO: Determine whether to reload default keys.  2021/07/08
		LOG_D("Reload WEP keys");
	default:
		LOG_D("Invalid reload option %d", *(wf_u32*)pvSetBuffer);
		rStatus = NDIS_STATUS_INVALID_DATA;
	}

	return rStatus;
} /* wf_wlan_oid_set_reload_defaults */

NDIS_STATUS
wf_wlan_oid_set_cur_pkt_filter (
	IN PADAPTER	prAdapter,
	IN	PVOID	 pvSetBuffer,
	IN	wf_u32  u4SetBufferLen,
	OUT wf_u32* pu4SetInfoLen
	)
{
	wf_u32 u4NewPacketFilter;
	NDIS_STATUS rStatus = NDIS_STATUS_SUCCESS;

	if (u4SetBufferLen < sizeof(wf_u32)) {
		*pu4SetInfoLen = sizeof(wf_u32);
		return NDIS_STATUS_INVALID_LENGTH;
	}
	/* Set the new packet filter. */
	u4NewPacketFilter = *(wf_u32*) pvSetBuffer;

	LOG_D("New packet filter: %#08lx", u4NewPacketFilter);

	if (prAdapter->dev_state != WF_DEV_STATE_RUN) {
		LOG_D("Fail in set current packet filter! (Adapter not ready).");
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}

		/* Verify the bits of the new packet filter. If any bits are set that
		   we don't support, leave. */
	if (u4NewPacketFilter & ~(PARAM_PACKET_FILTER_SUPPORTED)) {
			rStatus = NDIS_STATUS_NOT_SUPPORTED;
	}
	else{
		prAdapter->PacketFilter = u4NewPacketFilter;
	}


	return rStatus;
}	/* wf_wlan_oid_set_cur_pkt_filter */

NDIS_STATUS
wf_req_set_current_look_ahead(
	IN	P_GLUE_INFO_T	prGlueInfo,
	IN	PVOID			prSetBuffer,
	IN	wf_u32 		u4SetBufferLen,
	OUT wf_u32*		pu4SetInfoLen
	)
{
	/* We don't need to do anything for this OID. */
	*pu4SetInfoLen = sizeof(wf_u32);
	LOG_D("Indication length is %d (in byte) ", *(wf_u32*)prSetBuffer);
	return NDIS_STATUS_SUCCESS;

} /* end of wf_req_set_current_look_ahead() */

