#ifndef __WF_OIDS_SEC_H__
#define __WF_OIDS_SEC_H__

typedef NDIS_STATUS(*STA_QUERY_ALGO_PAIR_FUNC)(
	IN  PADAPTER                        pAdapter,
	__out PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList,
	IN  ULONG                           TotalLength
	);

NDIS_STATUS
StaGetAlgorithmPair(
	__in  PADAPTER                    pAdapter,
	__in  STA_QUERY_ALGO_PAIR_FUNC    QueryFunction,
	__deref_out_opt PDOT11_AUTH_CIPHER_PAIR    *AlgoPairs,
	__out PULONG                      NumAlgoPairs
);


VOID StaResetCipherAbilities(PADAPTER pAdapter);

VOID HwSetDefaultCipher(PADAPTER pAdapter);

NDIS_STATUS
Hw11SetKeyMappingKey(
	__in  PADAPTER pAdapter,
	__in  DOT11_MAC_ADDRESS MacAddr,
	__in  DOT11_DIRECTION direction,
	__in  BOOLEAN persistent,
	__in  DOT11_CIPHER_ALGORITHM algoId,
	__in  ULONG keyLength,
	__in  PUCHAR keyValue
);


typedef struct _NICKEY
{
	BOOLEAN                     Persistent;
	BOOLEAN                     Valid;
	DOT11_MAC_ADDRESS           MacAddr;
	DOT11_CIPHER_ALGORITHM      AlgoId;
	UCHAR                       KeyLength;              // length of KeyValue in bytes
	UCHAR                       KeyValue[16];           // 128 bits
	UCHAR                       TxMICKey[8];            // for TKIP only
	UCHAR                       RxMICKey[8];            // for TKIP only
	union {
		struct {
			ULONGLONG           PN : 48;                  // for CCMP
			ULONGLONG           PN_unused : 16;
		};
		struct {
			ULONGLONG           TSC : 48;                 // for TKIP
			ULONGLONG           TSC_unused : 16;
		};
		struct {
			ULONG               IV : 24;                  // for WEP 
			ULONG               IV_unused : 8;
		};
	};
	struct {
		ULONGLONG               ReplayCounter : 48;       // for CCMP or TKIP
		ULONGLONG               ReplayCounter_unused : 16;
	};
} NICKEY, * PNICKEY;

typedef struct _NIC_PER_STA_KEY
{
	DOT11_MAC_ADDRESS           StaMacAddr;
	BOOLEAN                     Valid;
	NICKEY                      NicKey[DOT11_MAX_NUM_DEFAULT_KEY];
} NIC_PER_STA_KEY, * PNIC_PER_STA_KEY;

NDIS_STATUS
HwSetKey(
	__in  PADAPTER pAdapter,
	__in  PNICKEY pNicKey,
	__in  UCHAR keyIndex,
	__in  BOOLEAN perStaKey,
	__in  BOOLEAN persistent,
	__in  DOT11_CIPHER_ALGORITHM algoId,
	__in  ULONG keyLength,
	__in  PUCHAR keyValue
);

VOID HwAddKeyEntry(PADAPTER pAdapter, PNICKEY pNicKey, UCHAR keyIndex);

NDIS_STATUS
Sta11SetCipherDefaultKey(
	__in   PADAPTER pAdapter,
	__in   PDOT11_CIPHER_DEFAULT_KEY_VALUE defaultKey
);

NDIS_STATUS
Sta11SetCipherKeyMappingKey(
	__in  PADAPTER pAdapter,
	__in  PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE pKeyMappingKeys,
	__in  ULONG TotalLength
);

NDIS_STATUS
Hw11SetDefaultKey(
	__in  PADAPTER pAdapter,
	__in  DOT11_MAC_ADDRESS MacAddr,
	__in  ULONG keyID,
	__in  BOOLEAN persistent,
	__in  DOT11_CIPHER_ALGORITHM algoId,
	__in  ULONG keyLength,
	__in  PUCHAR keyValue
);

static int sta_hw_set_group_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);

static int sta_hw_set_unicast_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);

VOID HwRemoveKeyEntry(PADAPTER pAdapter, wf_u8 keyIndex);

NDIS_STATUS
Sta11SetEnabledMulticastCipherAlgorithm(
	__in  PADAPTER       pAdapter,
	__in  PDOT11_CIPHER_ALGORITHM_LIST  pCipherAlgoList,
	__out PULONG          pBytesRead
);
#endif