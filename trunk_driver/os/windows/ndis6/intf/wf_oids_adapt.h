#ifndef __WF_OIDS_ADAPT_H__
#define __WF_OIDS_ADAPT_H__


typedef enum wf_oids_pend_req_e{
	OIDS_PEND_REQ_MIN,
	OIDS_PEND_REQ_ASSOC,
	OIDS_PEND_REQ_IP,
	OIDS_PEND_REQ_MAX,
}wf_oids_pend_req_e;

typedef struct wf_oids_timer_ctx_s {
    PADAPTER padapter;
	wf_u8 msg_type;
}wf_oids_timer_ctx_t;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(wf_oids_timer_ctx_t, wf_get_timer_context)



typedef struct wf_ap_info_s{
	LONG valid;

	wf_wlan_mgmt_scan_que_node_t scan_info;
	wf_u8 beacon[MP_802_11_MAX_FRAME_SIZE];
	wf_u8 assoc_req[MP_802_11_MAX_FRAME_SIZE];
	wf_u8 assoc_resp[MP_802_11_MAX_FRAME_SIZE];

	KSPIN_LOCK lock;
	wf_u32 bcn_len;
	wf_u32 assoc_req_len;
	wf_u32 assoc_resp_len;
}wf_ap_info_t;

typedef struct wf_no_ie_info_s {
    wf_80211_mgmt_timestamp_t timestamp;
    wf_80211_mgmt_beacon_interval_t intv;
    wf_80211_mgmt_capab_t capab;
    /* followed by some of SSID, Supported rates,
     * FH Params, DS Params, CF Params, IBSS Params, TIM */
    wf_u8 variable[0];
} wf_packed wf_no_ie_info_t;



typedef struct _BSS_FILTER_INFO
{
	wf_80211_bssid_t bssid[DEFAULT_MAX_BSS_NUM];
	wf_wlan_ssid_t ssid[DEFAULT_MAX_BSS_NUM];
	int cnt;
} BSS_FILTER_INFO, *PBSS_FILTER_INFO;

void wf_save_assoc_ssid(PADAPTER padapter, wf_u8 type);
wf_u64 wf_get_speed_by_raid(wf_u8 raid);


NDIS_STATUS wf_submit_assoc_complete(PADAPTER padapter, ULONG status);
NDIS_STATUS wf_submit_connect_complete(PADAPTER pAdapter, ULONG status);
NDIS_STATUS wf_submit_link_qual(PADAPTER pAdapter);
NDIS_STATUS wf_submit_scan_complete(PADAPTER pAdapter);
VOID wf_submit_disassoc_complete(PADAPTER pAdapter, ULONG Reason);
VOID wf_submit_link_speed(PADAPTER padapter, ULONG64  linkSpeed);


NDIS_STATUS wf_set_start_scan(void *nic_info);
NDIS_STATUS wf_set_start_assoc (PADAPTER adapter, PDOT11_SSID_LIST pDot11SSIDList);
NDIS_STATUS wf_set_start_deassoc(void *adapter);
NDIS_STATUS wf_set_auth_alg(PADAPTER pAdapter, PDOT11_AUTH_ALGORITHM_LIST pAuthAlgoList, PULONG pBytesRead);
NDIS_STATUS wf_set_unicast_cipher_alg(PADAPTER pAdapter, PDOT11_CIPHER_ALGORITHM_LIST pCipherAlgoList, PULONG pBytesRead);
NDIS_STATUS wf_set_multicast_cipher_alg(PADAPTER pAdapter, PDOT11_CIPHER_ALGORITHM_LIST pCipherAlgoList, PULONG pBytesRead);
VOID wf_set_default_cipher(PADAPTER pAdapter);
VOID wf_set_encryption(PADAPTER pAdapter, BOOLEAN isUnicast, DOT11_CIPHER_ALGORITHM  algoId);
VOID wf_set_auth(PADAPTER pAdapter);
VOID wf_set_auth_cipher(PADAPTER pAdapter);
VOID wf_set_wpa_ie(PADAPTER pAdapter);
VOID wf_check_PMKID_candidate(PADAPTER pAdapter);

NDIS_STATUS wf_get_bss_list(PADAPTER       pAdapter, PDOT11_BYTE_ARRAY pDot11ByteArray, ULONG TotalLength);
NDIS_STATUS wf_get_unicast_alg_pair(PADAPTER pAdapter, PDOT11_AUTH_CIPHER_PAIR_LIST pAuthCipherList, ULONG TotalLength);
NDIS_STATUS wf_get_multicast_alg_pair(PADAPTER pAdapter, PDOT11_AUTH_CIPHER_PAIR_LIST pAuthCipherList, ULONG TotalLength);
NDIS_STATUS wf_get_data_rate_map(PADAPTER padapter, PDOT11_DATA_RATE_MAPPING_TABLE pDataRateMappingTable, ULONG TotalLength);
NDIS_STATUS wf_get_statistics(PADAPTER pAdapter, NDIS_OID Oid, PVOID StatBuffer, PULONG BytesWritten);


void wf_oids_adapt_init(void *adapter);
void wf_oids_adapt_deinit(void *adapter);


#endif 

