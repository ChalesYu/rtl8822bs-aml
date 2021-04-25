#include "pcomp.h"
#include "common.h"

NDIS_STATUS
StaGetAlgorithmPair(
	__in  PADAPTER                    pAdapter,
	__in  STA_QUERY_ALGO_PAIR_FUNC    QueryFunction,
	__deref_out_opt PDOT11_AUTH_CIPHER_PAIR    *AlgoPairs,
	__out PULONG                      NumAlgoPairs
)
{
	DOT11_AUTH_CIPHER_PAIR_LIST     CipherPairList;
	PDOT11_AUTH_CIPHER_PAIR_LIST    FullPairList;
	NDIS_STATUS                     ndisStatus;
	ULONG                           size;

	*AlgoPairs = NULL;
	*NumAlgoPairs = 0;

	//
	// First get the total size of the algorithm pair list.
	//
	ndisStatus = (*QueryFunction)(pAdapter, &CipherPairList, sizeof(CipherPairList));
	if (ndisStatus != NDIS_STATUS_SUCCESS && ndisStatus != NDIS_STATUS_BUFFER_OVERFLOW)
	{
		return ndisStatus;
	}

	// Integer overflow
	if (FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) >
		FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) +
		CipherPairList.uTotalNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR))
	{
		return NDIS_STATUS_FAILURE;
	}

	size = FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) +
		CipherPairList.uTotalNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR);

	MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle,
		&FullPairList,
		size,
		HW11_MEMORY_TAG);
	if (FullPairList == NULL)
	{
		return NDIS_STATUS_RESOURCES;
	}

	//
	// Get the size of the list and copy the algorithm pair list data. Note that we over-allocated a little
	// bit for convenience.
	//

	ndisStatus = (*QueryFunction)(pAdapter, FullPairList, size);
	ASSERT(ndisStatus == NDIS_STATUS_SUCCESS && FullPairList->uTotalNumOfEntries == FullPairList->uNumOfEntries);

	*AlgoPairs = (PDOT11_AUTH_CIPHER_PAIR)FullPairList;
	*NumAlgoPairs = FullPairList->uNumOfEntries;

	//
	// Copy the algorithm pair to the beginning of the allocated buffer. Note that we cannot
	// use NdisMoveMemory as the source and destination overlap.
	//
	RtlMoveMemory(FullPairList,
		FullPairList->AuthCipherPairs,
		FullPairList->uNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR));

	return NDIS_STATUS_SUCCESS;
}

VOID StaResetCipherAbilities(PADAPTER pAdapter)
{
	// We set DOT11_AUTH_ALGO_RSNA as default.   2021/04/09
	pAdapter->MibInfo.AuthAlgorithm = DOT11_AUTH_ALGO_RSNA;
	
	StaSetDefaultCipher(pAdapter);
	return;
}

VOID HwSetEncryption()
{
	// TODO
	return;
}