/*++

Copyright (c) SCICS Co., Ltd. All rights reserved.

Module Name:
    wf_oids_adapt.c

Abstract:
    OIDs security related.
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    03-06-2021    Created
Notes:

--*/

#include "wf_debug.h"
#include "pcomp.h"

VOID wf_reset_cipher_abilities(PADAPTER pAdapter)
{
	// We set DOT11_AUTH_ALGO_RSNA as default.   2021/04/09
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	ENUM_NLO_CIPHER_ALGORITHM defaultCipher = NLO_CIPHER_ALGO_NONE;

	mib_info->fgTxBcKeyExist = FALSE;
	mib_info->eEncStatus = ENUM_ENCRYPTION3_ENABLED;
	mib_info->auth_mode = AUTH_MODE_OPEN;
	
	wf_set_encryption(pAdapter, TRUE, defaultCipher);
	wf_set_encryption(pAdapter, FALSE, defaultCipher);
	
	wf_set_hw_default_cipher(pAdapter);

	return;
}

VOID wf_set_encryption(PADAPTER pAdapter, BOOLEAN isUnicast, ENUM_NLO_CIPHER_ALGORITHM algoId)
{
	nic_info_st *pnic_info = pAdapter->nic_info;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	sec_info_st *psec_info = NULL;
	if(pnic_info == NULL || mib_info == NULL) {
		LOG_E("param is NULL");
		return;
	}

	psec_info = pnic_info->sec_info;
	if(psec_info == NULL) {
		LOG_E("param is NULL");
		return;
	}
	
	if (isUnicast) {
		mib_info->HwUnicastCipher = algoId;
		psec_info->dot11PrivacyAlgrthm = algoId;
	}
	else {
		mib_info->HwMulticastCipher = algoId;
		psec_info->dot118021XGrpPrivacy = algoId;
	}
	
	return;
}

VOID wf_set_hw_default_cipher(PADAPTER pAdapter)
{
	nic_info_st *pnic_info = pAdapter->nic_info;
	sec_info_st *sec_info = pnic_info->sec_info;
	wf_mib_info_t *mib_info = pAdapter->mib_info;
	int index;

	sec_info->ndisencryptstatus = wf_ndis802_11EncryptionDisabled;
	sec_info->dot11PrivacyAlgrthm = _NO_PRIVACY_;
	sec_info->dot118021XGrpPrivacy = _NO_PRIVACY_;
	sec_info->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
	sec_info->ndisauthtype = wf_ndis802_11AuthModeOpen;

	for (index = 0; index < MAX_KEY_NUM; index++)
	{
		mib_info->KeyTable[index].Valid = FALSE;
		mib_info->KeyTable[index].ToBeWritten = FALSE;
	}
	return;
}

NDIS_STATUS
wf_set_nic_key(PADAPTER prAdapter, P_WF_NDIS_802_11_KEY_T pNdisKey, BOOLEAN bPairwise)
{
	wf_mib_info_t *mib_info = prAdapter->mib_info;
	P_WF_NICKEY pNicKey = NULL;
	wf_u32	keyIndex = 0;
	wf_u32  cipherType;
	ASSERT(mib_info);

	cipherType = bPairwise ? mib_info->HwUnicastCipher : mib_info->HwMulticastCipher;

	for(keyIndex = 0; keyIndex < MAX_KEY_NUM; keyIndex++)
	{
		if(mib_info->KeyTable[keyIndex].Valid == FALSE) break;
	}
	if(keyIndex == MAX_KEY_NUM){
		LOG_E("RESOURCE ERROR! NIC key table is full.");
		return NDIS_STATUS_ADAPTER_NOT_READY;
	}
	pNicKey = mib_info->KeyTable + keyIndex;
	pNicKey->Valid = TRUE;
	pNicKey->ToBeWritten = TRUE;
	pNicKey->KeyIndex = pNdisKey->u4KeyIndex & 0xff;
	pNicKey->KeyLength = pNdisKey->u4KeyLength;
	pNicKey->bePairwise = bPairwise;
	wf_memcpy(pNicKey->MacAddr, pNdisKey->arBSSID, MAC_ADDR_LEN);

	//if(cipherType == WF_DOT11_CIPHER_ALGO_WEP)
	
	switch (cipherType)
	{
	case WF_DOT11_CIPHER_ALGO_TKIP:
		pNicKey->AlgoId = WF_DOT11_CIPHER_ALGO_TKIP;
		wf_memcpy(pNicKey->KeyValue, pNdisKey->aucKeyMaterial, TKIP_ENC_KEY_LEN);
		wf_memcpy(pNicKey->RxMICKey, pNdisKey->aucKeyMaterial + 16, TKIP_MIC_KEY_LEN);
		wf_memcpy(pNicKey->TxMICKey, pNdisKey->aucKeyMaterial + 24, TKIP_MIC_KEY_LEN);
		break;
	case WF_DOT11_CIPHER_ALGO_CCMP:
		pNicKey->AlgoId = WF_DOT11_CIPHER_ALGO_CCMP;
		wf_memcpy(pNicKey->KeyValue, pNdisKey->aucKeyMaterial, CCMP_KEY_LEN);
		break;
	case WF_DOT11_CIPHER_ALGO_WEP40:
		if(pNicKey->KeyLength > 5){
			LOG_E("Invalid key length for WEP40");
			return NDIS_STATUS_FAILURE;
		}
		wf_memcpy(pNicKey->KeyValue, pNdisKey->aucKeyMaterial, 5);
		break;
	case WF_DOT11_CIPHER_ALGO_WEP104:
		if(pNicKey->KeyLength > 13){
			LOG_E("Invalid key length for WEP104");
			return NDIS_STATUS_FAILURE;
		}
		wf_memcpy(pNicKey->KeyValue, pNdisKey->aucKeyMaterial, 13);
		break;
	default:
		LOG_E("Unsupported cipher type.");
		break;
	}
	
	return NDIS_STATUS_SUCCESS;
}

VOID wf_sta_hw_set_unicast_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
	wf_u8 cam_id;
	wf_u16 ctrl;
	int ret;

	cam_id = 4;
	ctrl = BIT(15) | (pwdn_info->dot118021XPrivacy << 2);
	ret = wf_mcu_set_sec_cam(pnic_info,
		cam_id, ctrl,
		pwdn_info->mac, pwdn_info->dot118021x_UncstKey.skey);
	if(ret != 0)
	{
		LOG_E("Write key to mailbox failed! (Unicast)");
	}
}

VOID wf_sta_hw_set_group_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
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
	if(ret != 0)
	{
		LOG_E("Write key to mailbox failed! (Multicast)");
	}
}

