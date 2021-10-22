#ifndef __WF_OIDS_ADAPT_H__
#define __WF_OIDS_ADAPT_H__


EVT_WDF_WORKITEM wf_set_Wpa_Workitem;
EVT_WDF_WORKITEM wf_write_key_Workitem;
EVT_WDF_WORKITEM wf_asych_OID_Workitem;

typedef struct wf_oids_timer_ctx_s {
    PADAPTER padapter;
}wf_oids_timer_ctx_t;

typedef struct wf_ap_info_s{
	wf_u8 valid;
	wf_wlan_mgmt_scan_que_node_t scan_info;
	// TODO: We don't need these data for NDIS5. Delete them and modify
	// the related rx dispatch and NDIS indication.   2021/06/11
	wf_u8 beacon[1536];
	wf_u8 assoc_req[1536];
	wf_u8 assoc_resp[1536];

	KSPIN_LOCK lock;
	wf_u32 bcn_len;
	wf_u32 assoc_req_len;
	wf_u32 assoc_resp_len;
}wf_ap_info_t;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(wf_oids_timer_ctx_t, wf_get_timer_context);

typedef struct _MEDIA_STREAMING_INDICATIONS_T {
    NDIS_802_11_STATUS_TYPE         StatusType;
    NDIS_802_11_MEDIA_STREAM_MODE   MediaStreamMode;
} MEDIA_STREAMING_INDICATIONS_T, *P_MEDIA_STREAMING_INDICATIONS_T;


/**
* This routine will enable/disable the cipher suite.
*/
VOID
wf_sec_set_cipher_suite (
    IN PADAPTER prAdapter,
    IN wf_u32     u4CipherSuitesFlags
);

/** 
* This routine is to get indicated media state.
*/
ENUM_PARAM_MEDIA_STATE_T
wf_get_media_state_indicated(
	IN P_GLUE_INFO_T	prGlueInfo
);

void wf_submit_link_qual(PADAPTER padapter);
void wf_submit_link_speed(PADAPTER padapter, ULONG64  linkSpeed);    
/** 
* This routine is to start a scan operation.
*/
NDIS_STATUS
wf_set_scan(PADAPTER prAdapter);

/** 
* Call NIC to start the scan operation.
*/
NDIS_STATUS wf_set_start_scan(void *nic_info);

NDIS_STATUS wf_submit_scan_complete(PADAPTER pAdapter);

//NDIS_STATUS wf_oids_adapt_init(void *adapter);

VOID wf_submit_disassoc_complete(PADAPTER pAdapter, ULONG Reason);

NDIS_STATUS wf_set_start_deassoc(void *adapter, wf_bool en_ind);

NDIS_STATUS wf_set_start_assoc (PADAPTER adapter, P_PARAM_SSID_T pDot11SSID);

static wf_wlan_mgmt_scan_que_node_t *wf_find_scan_info_by_ssid(PADAPTER adapter, P_PARAM_SSID_T pDot11SSID);

NDIS_STATUS wf_submit_assoc_complete(PADAPTER padapter, ULONG status);

NDIS_STATUS wf_submit_connect_complete(PADAPTER prAdapter, ULONG status);

VOID wf_set_wpa_ie(PADAPTER pAdapter);

VOID wf_set_auth(PADAPTER pAdapter);

VOID wf_set_auth_cipher(PADAPTER pAdapter);

VOID
wf_indicate_StatusAndComplete(
    IN P_GLUE_INFO_T prGlueInfo,
    IN NDIS_STATUS eStatus,
    IN PVOID       pvBuf,
    IN wf_u32     u4BufLen
);

BOOLEAN
wf_reset_media_stream_mode(IN PADAPTER prAdapter);

/**
* Judge whether a MAC address is a broadcast address.
*/
BOOLEAN
wf_is_broadcast(wf_u8* inputAddr);

VOID
wf_attr_reset(PADAPTER pAdapter);

NDIS_STATUS wf_wpa_workitem_init(PADAPTER pAdapter);

NDIS_STATUS wf_writeKey_workitem_init(PADAPTER pAdapter);

NDIS_STATUS wf_asychOID_workitem_init(PADAPTER pAdapter);

VOID
wf_assoc_resp_info_update(PADAPTER padapter, rx_pkt_t *nic_pkt);

VOID
wf_assoc_req_info_update(PADAPTER padapter);

VOID
wf_reset_assoc_info(PADAPTER pAdapter);

OS_SYSTIME
wf_get_time_tick(VOID);

wf_bool
wf_check_ssid_valid(PADAPTER prAdapter, P_PARAM_SSID_T pDot11SSID);

VOID
wf_reset_pkt_statistics_info(PADAPTER prAdapter);

// Create a timer to deal with unexpected situation for OIDs such as
// connection failed.
NDIS_STATUS wf_oids_exception_timer_create(void *adapter);

void wf_oids_exception_handle(WDFTIMER WdfTimer);

// Save association infomation to local file for next auto connection.
void wf_save_assoc_ssid(PADAPTER padapter, wf_bool type);
// Get association information from local file.
int wf_get_assoc_ssid(PADAPTER padapter, wf_wlan_ssid_t *ssid);

NDIS_STATUS wf_set_scan_hidden_network(PADAPTER padapter, P_PARAM_SSID_T pDot11SSID);

#endif 
