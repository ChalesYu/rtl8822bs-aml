#ifndef __WF_HW_SEC_H__
#define __WF_HW_SEC_H__

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
#endif