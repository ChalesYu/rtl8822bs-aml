#ifndef __WF_HW_MGNT_H__
#define __WF_HW_MGNT_H__

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

VOID HwSetEncryption();
#endif