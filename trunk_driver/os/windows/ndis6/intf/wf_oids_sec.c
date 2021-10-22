#include "pcomp.h"
#include "wf_debug.h"



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

//	MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle,
//		&FullPairList,
//		size,
//		HW11_MEMORY_TAG);

	FullPairList = wf_malloc(size);
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
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	
	mib_info->AuthAlgorithm = DOT11_AUTH_ALGO_RSNA;
	wf_set_default_cipher(pAdapter);
	HwSetDefaultCipher(pAdapter);
	return;
}

VOID HwSetDefaultCipher(PADAPTER pAdapter)
{
	nic_info_st *pnic_info = pAdapter->nic_info;
	sec_info_st *sec_info = pnic_info->sec_info;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

	sec_info->ndisencryptstatus = wf_ndis802_11EncryptionDisabled;
	sec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
	sec_info->dot118021XGrpPrivacy = _NO_PRIVACY_;
	sec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
	sec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;
	for (int index = 0; index < NATIVE_802_11_MAX_KEY_MAPPING_ENTRY; index++)
	{
		mib_info->KeyTable[index].Valid = FALSE;
	}
	return;
}

NDIS_STATUS
Hw11SetKeyMappingKey(
	__in  PADAPTER pAdapter,
	__in  DOT11_MAC_ADDRESS MacAddr,
	__in  DOT11_DIRECTION direction,
	__in  BOOLEAN persistent,
	__in  DOT11_CIPHER_ALGORITHM algoId,
	__in  ULONG keyLength,
	__in  PUCHAR keyValue
)
{
	PNICKEY pNicKey = NULL;
	ULONG   index;
	ULONG   emptyIndex;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

	//
	// We don't support uni-direction key mapping keys
	//
	if (direction != DOT11_DIR_BOTH) {
		return NDIS_STATUS_NOT_SUPPORTED;
	}
	//
	// Search the key mapping table to find either a matching MacAddr or an empty key entry.
	//
	emptyIndex = 0;
	for (index = DOT11_MAX_NUM_DEFAULT_KEY; index < KEY_TABLE_SIZE; index++) {
		pNicKey = mib_info->KeyTable + index;
		if (pNicKey->Valid && NdisEqualMemory(pNicKey->MacAddr, MacAddr, sizeof(DOT11_MAC_ADDRESS)) == 1) {
			break;
		}

		if (!pNicKey->Valid && emptyIndex == 0) {
			emptyIndex = index;
		}
	}

	if (index == KEY_TABLE_SIZE) {
		//
		// We did not find a key with matching MacAddr. 
		//

		if (keyLength == 0) {
			//
			// If we are asked to delete a key, fail the request.
			//
			return NDIS_STATUS_INVALID_DATA;
		}
		else if (emptyIndex == 0) {
			//
			// If we are asked to add a key but the table is full, fail the request.
			//
			return NDIS_STATUS_RESOURCES;
		}

		pNicKey = mib_info->KeyTable + emptyIndex;
		ETH_COPY_NETWORK_ADDRESS(pNicKey->MacAddr, MacAddr);
	}

	return HwSetKey(pAdapter,
		pNicKey,
		(UCHAR)(pNicKey - mib_info->KeyTable),
		FALSE,
		persistent,
		algoId,
		keyLength,
		keyValue);
}


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
)
{
	ULONG                       i;
	PDOT11_KEY_ALGO_CCMP        CCMPKey = NULL;
	PDOT11_KEY_ALGO_TKIP_MIC    TKIPKey = NULL;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

	//
	// If keyLength is non-zero, the key is to be added, otherwise, it is to be deleted.
	// Note: MacAddr of the key is already set.
	//

	if (keyLength != 0) {
		//
		// If the algoId is generic WEP, change it to WEP40 or WEP104 depending on the key length.
		//
		switch (algoId) {
		case DOT11_CIPHER_ALGO_WEP:
			switch (keyLength) {
			case 40 / 8:
				algoId = DOT11_CIPHER_ALGO_WEP40;
				break;
			case 104 / 8:
				algoId = DOT11_CIPHER_ALGO_WEP104;
				break;
			default:
				return NDIS_STATUS_INVALID_DATA;
			}

			//
			// fall through
			//
		case DOT11_CIPHER_ALGO_WEP104:
		case DOT11_CIPHER_ALGO_WEP40:
			break;
		case DOT11_CIPHER_ALGO_CCMP:
			//
			// Validate the key length
			//
			if (keyLength < (ULONG)FIELD_OFFSET(DOT11_KEY_ALGO_CCMP, ucCCMPKey))
			{
				return NDIS_STATUS_INVALID_DATA;
			}
			CCMPKey = (PDOT11_KEY_ALGO_CCMP)keyValue;
			if (keyLength < FIELD_OFFSET(DOT11_KEY_ALGO_CCMP, ucCCMPKey) +
				CCMPKey->ulCCMPKeyLength)
			{
				return NDIS_STATUS_INVALID_DATA;
			}
			//
			// Only support 16-byte CCMP key 
			//
			if (CCMPKey->ulCCMPKeyLength != 16)
			{
				return NDIS_STATUS_INVALID_DATA;
			}
			keyLength = CCMPKey->ulCCMPKeyLength;
			keyValue = CCMPKey->ucCCMPKey;
			break;

		case DOT11_CIPHER_ALGO_TKIP:
			//
			// Validate the key length
			//
			if (keyLength < (ULONG)FIELD_OFFSET(DOT11_KEY_ALGO_TKIP_MIC, ucTKIPMICKeys))
			{
				return NDIS_STATUS_INVALID_DATA;
			}
			TKIPKey = (PDOT11_KEY_ALGO_TKIP_MIC)keyValue;
			if (keyLength < FIELD_OFFSET(DOT11_KEY_ALGO_TKIP_MIC, ucTKIPMICKeys) +
				TKIPKey->ulTKIPKeyLength +
				TKIPKey->ulMICKeyLength)
			{
				return NDIS_STATUS_INVALID_DATA;
			}
			//
			// Only support 16-byte TKIP key and 8-byte Tx/Rx MIC key
			//
			if (TKIPKey->ulTKIPKeyLength != 16 || TKIPKey->ulMICKeyLength != 16)
			{
				return NDIS_STATUS_INVALID_DATA;
			}
			keyLength = TKIPKey->ulTKIPKeyLength;
			keyValue = TKIPKey->ucTKIPMICKeys;
			break;
		}
		//
		// The real key length cannot be larger than our register length for a key.
		// Also, for TKIP, we only support 8-byte Tx MIC key and 8-byte Rx MIC key 
		//
		if (keyLength > sizeof(pNicKey->KeyValue) ||
			algoId == DOT11_CIPHER_ALGO_TKIP && TKIPKey->ulMICKeyLength != 16) {
			return NDIS_STATUS_INVALID_DATA;
		}

		//
		// If the current encryption algorithm is WEP, change it to more specific WEP40 or WEP104.
		//

		if (mib_info->UnicastCipherAlgorithm == DOT11_CIPHER_ALGO_WEP &&
			(algoId == DOT11_CIPHER_ALGO_WEP40 || algoId == DOT11_CIPHER_ALGO_WEP104)) {
			mib_info->UnicastCipherAlgorithm = algoId;

			//
			// For RTL8180, also program the hardware
			//

		}

		if (mib_info->MulticastCipherAlgorithm == DOT11_CIPHER_ALGO_WEP &&
			(algoId == DOT11_CIPHER_ALGO_WEP40 || algoId == DOT11_CIPHER_ALGO_WEP104)) {
			mib_info->MulticastCipherAlgorithm = algoId;
		}

		//
		// We should never have unicast and multicast cipher with different length of WEP.
		// See comment in function Hw11SetEncryption().
		//
		ASSERT(!(mib_info->UnicastCipherAlgorithm == DOT11_CIPHER_ALGO_WEP40 &&
			mib_info->MulticastCipherAlgorithm == DOT11_CIPHER_ALGO_WEP104));

		ASSERT(!(mib_info->UnicastCipherAlgorithm == DOT11_CIPHER_ALGO_WEP104 &&
			mib_info->MulticastCipherAlgorithm == DOT11_CIPHER_ALGO_WEP40));

		//
		// For key mapping key, its algorithm must match current unicast cipher (unless 
		// the key is for multicast/broadcast data frames).
		//
		// For per-STA key, or key mapping key for multicast/broadcast data frames, 
		// its algorithm must match the current unicast cipher.
		//
		// For default key, its algorithm must match either the current unicast cipher
		// or the current multicast cipher. 
		//
		if (mib_info->UnicastCipherAlgorithm != algoId &&
			keyIndex >= DOT11_MAX_NUM_DEFAULT_KEY &&
			ETH_IS_UNICAST(pNicKey->MacAddr)) {
			return NDIS_STATUS_INVALID_DATA;
		}

		if (mib_info->UnicastCipherAlgorithm != algoId &&
			mib_info->MulticastCipherAlgorithm != algoId) {
			return NDIS_STATUS_INVALID_DATA;
		}

		//
		// Update the key entry for valid key. We cannot fail after this point. Otherwise, 
		// the existing key will be modified when we fail to set the new key.
		//

		pNicKey->Valid = TRUE;
		pNicKey->Persistent = persistent;
		pNicKey->KeyLength = (UCHAR)keyLength;
		pNicKey->AlgoId = algoId;

		if (keyIndex < DOT11_MAX_NUM_DEFAULT_KEY) {
			LOG_D("Update %s key %d (algo %d), (index %d): ",
				perStaKey ? "per-STA" : "default", keyIndex, algoId, keyIndex);
		}
		else {
			LOG_D("Update key mapping key for %02X-%02X-%02X-%02X-%02X-%02X (algo %d)(index %d): ",
				pNicKey->MacAddr[0], pNicKey->MacAddr[1], pNicKey->MacAddr[2],
				pNicKey->MacAddr[3], pNicKey->MacAddr[4], pNicKey->MacAddr[5], algoId, keyIndex);
		}
		for (i = 0; i < keyLength; i++) {
			pNicKey->KeyValue[i] = keyValue[i];
			DbgPrint("%02X ", keyValue[i]);
		}
		DbgPrint("\n");
		for (i = keyLength; i < sizeof(pNicKey->KeyValue); i++)
			pNicKey->KeyValue[i] = 0;

		switch (algoId) {
		case DOT11_CIPHER_ALGO_WEP:
		case DOT11_CIPHER_ALGO_WEP104:
		case DOT11_CIPHER_ALGO_WEP40:
			pNicKey->IV = 1;
			break;

		case DOT11_CIPHER_ALGO_CCMP:
			pNicKey->PN = 1;
			pNicKey->ReplayCounter = ((ULONGLONG)CCMPKey->ucIV48Counter[0]) |
				(((ULONGLONG)CCMPKey->ucIV48Counter[1]) << 8) |
				(((ULONGLONG)CCMPKey->ucIV48Counter[2]) << 16) |
				(((ULONGLONG)CCMPKey->ucIV48Counter[3]) << 24) |
				(((ULONGLONG)CCMPKey->ucIV48Counter[4]) << 32) |
				(((ULONGLONG)CCMPKey->ucIV48Counter[5]) << 40);
			break;

		case DOT11_CIPHER_ALGO_TKIP:
			pNicKey->TSC = 1;

			pNicKey->ReplayCounter = ((ULONGLONG)TKIPKey->ucIV48Counter[0]) |
				(((ULONGLONG)TKIPKey->ucIV48Counter[1]) << 8) |
				(((ULONGLONG)TKIPKey->ucIV48Counter[2]) << 16) |
				(((ULONGLONG)TKIPKey->ucIV48Counter[3]) << 24) |
				(((ULONGLONG)TKIPKey->ucIV48Counter[4]) << 32) |
				(((ULONGLONG)TKIPKey->ucIV48Counter[5]) << 40);
			NdisMoveMemory(pNicKey->RxMICKey, Add2Ptr(keyValue, keyLength), 8);
			NdisMoveMemory(pNicKey->TxMICKey, Add2Ptr(keyValue, keyLength + 8), 8);
			break;
		}
		

		//
		// Program the hardware.
		//
		HwAddKeyEntry(pAdapter, pNicKey, keyIndex);
	}
	else {
		//
		// Remove the key from hardware.
		//
		NdisZeroMemory(pNicKey, sizeof(NICKEY));
		if (!perStaKey) {
			HwRemoveKeyEntry(pAdapter, keyIndex);
		}

	}

	return NDIS_STATUS_SUCCESS;
}

VOID HwAddKeyEntry(PADAPTER pAdapter, PNICKEY pNicKey, UCHAR keyIndex)
{
	wf_u32 wep_key_idx, wep_key_len;
	
	nic_info_st *pnic_info = pAdapter->nic_info;
	sec_info_st *psec_info = pnic_info->sec_info;
	wdn_net_info_st *pwdn_info;
	wf_mib_info_t *mib_info = pAdapter->mib_info;

	if (pNicKey->AlgoId == DOT11_CIPHER_ALGO_WEP ||
		pNicKey->AlgoId == DOT11_CIPHER_ALGO_WEP40 ||
		pNicKey->AlgoId == DOT11_CIPHER_ALGO_WEP104)
	{
		if (keyIndex >= DOT11_MAX_NUM_DEFAULT_KEY ||
			pNicKey->KeyLength == 0)
		{
			LOG_E("Set WEP key error!");
			return;
		}
		wep_key_idx = keyIndex;
		wep_key_len = pNicKey->KeyLength;
		psec_info->ndisencryptstatus = wf_ndis802_11Encryption1Enabled;

		wep_key_len = wep_key_len <= 5 ? 5 : 13; // 5 bytes for WEP40, 13 bytes for WEP104.
		if (wep_key_len == 13)
		{
			psec_info->dot11PrivacyAlgrthm = _WEP104_;
		}
		else
		{
			psec_info->dot11PrivacyAlgrthm = _WEP40_;
		}
		psec_info->dot11PrivacyKeyIndex = wep_key_idx;
		memcpy(psec_info->dot11DefKey[wep_key_idx].skey,
			pNicKey->KeyValue, wep_key_len);
		psec_info->dot11DefKeylen[wep_key_idx] = wep_key_len;
		psec_info->key_mask |= BIT(wep_key_idx);
	}
	else if (psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) /* 802_1x */
	{
		local_info_st *plocal_info = pnic_info->local_info;
		if (plocal_info->work_mode == WF_INFRA_MODE) /* sta mode */
		{
			pwdn_info = wf_wdn_find_info(pnic_info,
				wf_wlan_get_cur_bssid(pnic_info));
			if (pwdn_info == NULL)
			{
				LOG_E("pwdn_info NULL! ");
				return;
			}

			if (psec_info->ndisencryptstatus == wf_ndis802_11Encryption2Enabled ||
				psec_info->ndisencryptstatus == wf_ndis802_11Encryption3Enabled)
			{
				pwdn_info->dot118021XPrivacy = (wf_u16)psec_info->dot11PrivacyAlgrthm;
			}
			/* PTK */
			if (mib_info->UnicastCipherAlgorithm == pNicKey->AlgoId
				&& keyIndex >= DOT11_MAX_NUM_DEFAULT_KEY) /* pairwise key */
			{
				LOG_D("pwdn_info->dot118021XPrivacy = %d", pwdn_info->dot118021XPrivacy);
				/* KCK PTK0~127 */
				memcpy(pwdn_info->dot118021x_UncstKey.skey, pNicKey->KeyValue,
					pNicKey->KeyLength < 16 ? pNicKey->KeyLength : 16);

				if (pNicKey->AlgoId == DOT11_CIPHER_ALGO_TKIP) /* set mic key */
				{
					/* KEK PTK128~255 */
					// The first 8 bytes will be the MIC key used for received packets and the 
					// last 8 bytes will be the MIC key used for transmitted packets. 
					memcpy(pwdn_info->dot11tkiprxmickey.skey,
						pNicKey->RxMICKey, 8); /* PTK128~191 */
					memcpy(pwdn_info->dot11tkiptxmickey.skey,
						pNicKey->TxMICKey, 8); /* PTK192~255 */
					psec_info->busetkipkey = wf_true;
				}
				if (pwdn_info->dot118021XPrivacy == _AES_)
				{
					LOG_D("sta_hw_set_unicast_key");
					sta_hw_set_unicast_key(pnic_info, pwdn_info);
				}
			}
			if (mib_info->MulticastCipherAlgorithm == pNicKey->AlgoId
				&& keyIndex < DOT11_MAX_NUM_DEFAULT_KEY) /* group key */
			{
				LOG_D("psec_info->dot118021XGrpPrivacy = %d", psec_info->dot118021XGrpPrivacy);
				memcpy(psec_info->dot118021XGrpKey[keyIndex].skey,
					pNicKey->KeyValue,
					pNicKey->KeyLength < 16 ? pNicKey->KeyLength : 16);
				memcpy(psec_info->dot118021XGrprxmickey[keyIndex].skey,
					pNicKey->RxMICKey, 8);
				memcpy(psec_info->dot118021XGrptxmickey[keyIndex].skey,
					pNicKey->TxMICKey, 8);
				psec_info->binstallGrpkey = wf_true;
				psec_info->dot118021XGrpKeyid = keyIndex;
				if (psec_info->dot118021XGrpPrivacy == _AES_)
				{
					LOG_D("sta_hw_set_group_key");
					sta_hw_set_group_key(pnic_info, pwdn_info);
				}
			}
		}
	}
	return;
}

NDIS_STATUS
Sta11SetCipherKeyMappingKey(
	__in  PADAPTER pAdapter,
	__in  PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE pKeyMappingKeys,
	__in  ULONG TotalLength
)
{
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	ULONG       size;

	if (TotalLength == 0)
		return NDIS_STATUS_INVALID_DATA;

	while (TotalLength >= (ULONG)FIELD_OFFSET(DOT11_CIPHER_KEY_MAPPING_KEY_VALUE, ucKey))
	{
		size = FIELD_OFFSET(DOT11_CIPHER_KEY_MAPPING_KEY_VALUE, ucKey) + pKeyMappingKeys->usKeyLength;
		if (TotalLength < size)
		{
			return NDIS_STATUS_INVALID_DATA;
		}

		//
		// Check cipher algorithm
		//
		switch (pKeyMappingKeys->AlgorithmId)
		{
		case DOT11_CIPHER_ALGO_CCMP:
		case DOT11_CIPHER_ALGO_TKIP:
		case DOT11_CIPHER_ALGO_WEP:
			break;
		case DOT11_CIPHER_ALGO_WEP40:
			if (pKeyMappingKeys->usKeyLength != 40 / 8)
				return NDIS_STATUS_INVALID_DATA;
			break;
		case DOT11_CIPHER_ALGO_WEP104:
			if (pKeyMappingKeys->usKeyLength != 104 / 8)
				return NDIS_STATUS_INVALID_DATA;
			break;

		default:
			return NDIS_STATUS_INVALID_DATA;
		}
		ndisStatus = Hw11SetKeyMappingKey(pAdapter,
			pKeyMappingKeys->PeerMacAddr,
			pKeyMappingKeys->Direction,
			pKeyMappingKeys->bStatic,
			pKeyMappingKeys->AlgorithmId,
			pKeyMappingKeys->bDelete ? 0 : pKeyMappingKeys->usKeyLength,
			pKeyMappingKeys->ucKey);
		TotalLength -= size;
		pKeyMappingKeys = Add2Ptr(pKeyMappingKeys, size);
	}

	//
	// Return success only if all data are consumed.
	//
	return (TotalLength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
}


NDIS_STATUS
Sta11SetCipherDefaultKey(
	__in   PADAPTER pAdapter,
	__in   PDOT11_CIPHER_DEFAULT_KEY_VALUE defaultKey
)
{

	//
	// Check the validity of the defaultKey
	//

	if (defaultKey->uKeyIndex >= DOT11_MAX_NUM_DEFAULT_KEY)
		{
		return NDIS_STATUS_INVALID_DATA;
		}
	//
	// Check cipher algorithm
	//

	switch (defaultKey->AlgorithmId)
	{
	case DOT11_CIPHER_ALGO_CCMP:
	case DOT11_CIPHER_ALGO_TKIP:
	case DOT11_CIPHER_ALGO_WEP:
		break;

	case DOT11_CIPHER_ALGO_WEP40:
		if (defaultKey->usKeyLength != 40 / 8)
			return NDIS_STATUS_INVALID_DATA;
		break;

	case DOT11_CIPHER_ALGO_WEP104:
		if (defaultKey->usKeyLength != 104 / 8)
			return NDIS_STATUS_INVALID_DATA;
		break;
	default:
		return NDIS_STATUS_INVALID_DATA;
	}

	//
	// Set HW default key
	//


	return Hw11SetDefaultKey(pAdapter,
		defaultKey->MacAddr,
		defaultKey->uKeyIndex,
		defaultKey->bStatic,
		defaultKey->AlgorithmId,
		defaultKey->bDelete ? 0 : defaultKey->usKeyLength,
		defaultKey->ucKey);
}

NDIS_STATUS
Hw11SetDefaultKey(
	__in  PADAPTER pAdapter,
	__in  DOT11_MAC_ADDRESS MacAddr,
	__in  ULONG keyID,
	__in  BOOLEAN persistent,
	__in  DOT11_CIPHER_ALGORITHM algoId,
	__in  ULONG keyLength,
	__in  PUCHAR keyValue
)
{
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	//
	// Note that both this function and other functions accessing key table are called
	// as a result of OID set. As such, no locking is needed since NDIS serializes
	// the OID set operation. If this premise is no longer valid, locking will
	// be needed.
	//
	UNREFERENCED_PARAMETER(MacAddr);

	ASSERT(keyID < DOT11_MAX_NUM_DEFAULT_KEY);
	if (keyID >= DOT11_MAX_NUM_DEFAULT_KEY)
		return NDIS_STATUS_INVALID_DATA;

	if (mib_info->CurrentBSSType == dot11_BSS_type_independent) {
		LOG_E("Unsupported BSS type.");
		return NDIS_STATUS_FAILURE;
	}
	else {
		//
		// Save the new default key in the key table. 
		//
	return HwSetKey(pAdapter,
			mib_info->KeyTable + keyID,
			(UCHAR)keyID,
			FALSE,
			persistent,
			algoId,
			keyLength,
			keyValue);
	}
}

static int sta_hw_set_unicast_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
	wf_u8 cam_id;
	wf_u16 ctrl;
	int ret;

	cam_id = 4;
	ctrl = BIT(15) | (pwdn_info->dot118021XPrivacy << 2);
	ret = wf_mcu_set_sec_cam(pnic_info,
		cam_id, ctrl,
		pwdn_info->mac, pwdn_info->dot118021x_UncstKey.skey);

	return ret;
}

static int sta_hw_set_group_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
	sec_info_st *psec_info = pnic_info->sec_info;
	wf_u8 cam_id;
	wf_u16 ctrl;
	//wf_u32 buf[2];
	int ret;

	cam_id = psec_info->dot118021XGrpKeyid & 0x03; /* cam_id0~3 8021x group key */
	ctrl = (wf_u16)(BIT(15)|BIT(6)|(psec_info->dot118021XGrpPrivacy << 2)|psec_info->dot118021XGrpKeyid);
	ret = wf_mcu_set_sec_cam(pnic_info, cam_id, ctrl,
		pwdn_info->bssid,
		psec_info->dot118021XGrpKey[psec_info->dot118021XGrpKeyid].skey);
	wf_mcu_set_on_rcr_am(pnic_info, wf_true);

	return ret;
}

VOID HwRemoveKeyEntry(PADAPTER pAdapter, wf_u8 keyIndex)
{
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	mib_info->KeyTable[keyIndex].Valid = wf_false;
	return;
}

#if 0
NDIS_STATUS
Sta11SetEnabledMulticastCipherAlgorithm(
	__in  PADAPTER       pAdapter,
	__in  PDOT11_CIPHER_ALGORITHM_LIST  pCipherAlgoList,
	__out PULONG          pBytesRead
)
{
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
	ULONG       index;

	__try
	{

		// Only support no more than STA_MULTICAST_CIPHER_MAX_COUNT cipher algorithms
		if (pCipherAlgoList->uNumOfEntries > STA_MULTICAST_CIPHER_MAX_COUNT)
		{
			*pBytesRead = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds);
			ndisStatus = NDIS_STATUS_INVALID_LENGTH;
			__leave;
		}

		*pBytesRead = FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds) +
			pCipherAlgoList->uNumOfEntries * sizeof(DOT11_CIPHER_ALGORITHM);


		//
		// If there is only one enabled multicast cipher, we known exactly what the 
		// multicast cipher will be. Program the hardware. Otherwise, we have to wait
		// until we know which multicast cipher will be used among those enabled.
		//
		if (pCipherAlgoList->uNumOfEntries == 1)
		{
			mib_info->MulticastCipherAlgorithm = pCipherAlgoList->AlgorithmIds[0];
			Hw11SetEncryption(pAdapter, FALSE, pCipherAlgoList->AlgorithmIds[0]);
		}
		else
		{
			mib_info->MulticastCipherAlgorithm = DOT11_CIPHER_ALGO_NONE;
			Hw11SetEncryption(pAdapter, FALSE, DOT11_CIPHER_ALGO_NONE);
		}

		// Copy the data locally
		mib_info->MulticastCipherAlgorithmCount = pCipherAlgoList->uNumOfEntries;
		for (index = 0; index < pCipherAlgoList->uNumOfEntries; index++)
		{
			mib_info->MulticastCipherAlgorithmList[index] = pCipherAlgoList->AlgorithmIds[index];
		}

		ndisStatus = NDIS_STATUS_SUCCESS;
	}
	__finally
	{
	}

	return ndisStatus;
}
#endif


