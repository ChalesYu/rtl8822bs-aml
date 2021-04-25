#include "globals.h"
#include "ndis.h"
#include "hw_pcomp.h"
#include "hw_main.h"
#include "st_main.h"
#include "hw_Nic.h"
#include "hw_func.h"
#include "usb_main.h"
#include "bus_intf.h"
#include "8187_gen.h"

static void _wl_do_stainfo_init(struct sta_info *psta, u8 tag);
extern u8 proc_joinbss_cmd_func(PNIC Nic, struct wlan_network * pnetwork);

#if 1
static void proc_rate_set_init_func(PNIC Nic, u8 flag)
{
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;

	unsigned char supported_mcs_set[16] =
		{ 0xff, 0xff, 0xff, 0x00, 0x00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
0x0, 0x0, 0x0 };

	Func_Of_Proc_Pre_Memcpy(pmlmeext->default_supported_mcs_set, supported_mcs_set,
				sizeof(pmlmeext->default_supported_mcs_set));
}
#endif

u32 _wl_do_sta_priv_init(PNIC Nic, u8 tag)
{
	struct sta_info *psta = NULL;
	s32 i;

	struct sta_priv *pstapriv =  &Nic->stapriv;

	_func_enter_;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====psta malloc size=%x\n", __func__,(sizeof(struct sta_info))));

#if 1
	pstapriv->pallocated_stainfo_buf =
		wl_zvmalloc(sizeof(struct sta_info) * 32 + 32);

#else
	MP_ALLOCATE_MEMORY(Nic->MiniportAdapterHandle, 
                           pstapriv->pallocated_stainfo_buf ,
                           (sizeof(struct sta_info) * 32 + 4),
                           STA11_MEMORY_TAG);
#endif	
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====psta malloc buf=%x\n", __func__,pstapriv->pallocated_stainfo_buf));


	if (!pstapriv->pallocated_stainfo_buf)
	{
		MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
		return FALSE;
	}
	if (tag) {
		pstapriv->pstainfo_buf = pstapriv->pallocated_stainfo_buf + 4 -
			((SIZE_PTR) (pstapriv->pallocated_stainfo_buf) & 3);

		//Func_Of_Proc_Pre_Init_Queue(&pstapriv->free_sta_queue);

		pstapriv->asoc_sta_count = 0;
		//Func_Of_Proc_Pre_Init_Queue(&pstapriv->sleep_q);
		//Func_Of_Proc_Pre_Init_Queue(&pstapriv->wakeup_q);

		psta = (struct sta_info *)(pstapriv->pstainfo_buf);
	}
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====psta=%x\n", __func__,psta));

	for (i = 0; i < MACID_NUM_SW_LIMIT; i++) {
		_wl_do_stainfo_init(psta, 1);
		Nic->stapriv.sta_aid[i] = psta;
		psta->Nic = Nic;
		MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====psta[%d]=%x,Nic=%x\n", __func__,i,Nic->stapriv.sta_aid[i],psta->Nic));
		psta++;
	}


//for test
	psta = Nic->stapriv.sta_aid[0];
	psta->rssi_level = DM_RATR_STA_INIT;
	psta->bw_mode = 0;//CHANNEL_WIDTH_20
	psta->wireless_mode = WIRELESS_11BG_24N;
	psta->raid = get_netype_to_raid_ex(Nic, psta, 1);
	psta->htpriv.ht_cap.supp_mcs_set[0] = 0xff;
	psta->htpriv.ht_option = 1;

	proc_rate_set_init_func(Nic, 1);
	
	Func_Of_Proc_Chip_Hw_Update_Sta_Rate_Mask(Nic,psta);
	psta->ra_mask = 0xfff;
	
	Func_Of_Proc_Chip_Hw_Set_Odm_Var(Nic, HAL_MSG_STA_INFO, psta, _TRUE);
	
	Func_Of_Proc_Chip_Hw_Update_Ra_Mask(psta, psta->rssi_level);//add by ddliu

//	pstapriv->max_num_sta = MACID_NUM_SW_LIMIT;
//	pstapriv->adhoc_expire_to = 4;
//	pstapriv->tim_bitmap = 0;
//	pstapriv->sta_dz_bitmap = 0;
//	pstapriv->auth_to = 3;
//	pstapriv->assoc_to = 3;
//	pstapriv->asoc_list_cnt = 0;
//	pstapriv->auth_list_cnt = 0;

	_func_exit_;

	return _TRUE;

}

static void _wl_do_stainfo_init(struct sta_info *psta, u8 tag)
{

	_func_enter_;

	memset((u8 *) psta, 0, sizeof(struct sta_info));
	if (tag) {
		
		//_wl_do_sta_tx_priv_init(&psta->sta_xmitpriv, 1);
		//_do_init_sta_rx_priv(&psta->sta_recvpriv, 1);

		psta->flags = 0;
		psta->sleepq_len = 0;
		psta->capability = 0;
		psta->expire_to = 0;
		psta->keep_alive_trycnt = 0;

		psta->bpairwise_key_installed = _FALSE;

//		psta->no_ht_set = 0;
//		psta->nonerp_set = 0;
//		psta->ht_20mhz_set = 0;
//		psta->no_ht_gf_set = 0;
//		psta->ht_40mhz_intolerant = 0;
//		psta->no_short_slot_time_set = 0;
//		psta->no_short_preamble_set = 0;

	}
	//wl_do_init_stc(&psta->st_ctl, 1);

	//wl_new_macid(psta->Nic, psta, 1);
	
	_func_exit_;

}

void 
Hw11RestoreEncryption(
    __in  PNIC pNic
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
}


NDIS_STATUS
ReadNicInfo8187(
    IN PNIC         Nic
    )
{
	 MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
     return NDIS_STATUS_SUCCESS;
}
	
NDIS_STATUS    
InitializeNic8187(
	IN PNIC 		Nic,
	IN UCHAR		   Channel
	)
{

	BOOL status = FALSE;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));

	MpTrace(COMP_RICHD, DBG_NORMAL, ("Chip_Hw_Init start\n"));
	status = Func_Of_Proc_Chip_Hw_Init(Nic);
	
	if (status == FALSE) {
		MpTrace(COMP_RICHD, DBG_NORMAL, ("===>%s,fail!\n", __func__));
		return NDIS_STATUS_FAILURE;
	 }else
	 	{
		   MpTrace(COMP_RICHD, DBG_NORMAL, ("===>%s,success!\n", __func__));	 	
	 	}
	 
	if (_wl_do_sta_priv_init(Nic, 1) == FALSE) {
		return NDIS_STATUS_FAILURE;
	}
		 
	return NDIS_STATUS_SUCCESS;
}


BOOLEAN
SetRFPowerState8187(
    PNIC            Nic, 
    RT_RF_POWER_STATE   eRFPowerState
)
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
	return TRUE;
}

BOOLEAN
SetRFPowerState(
    PNIC                Nic, 
    RT_RF_POWER_STATE   eRFPowerState
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>RFPowerState=%d\n", __func__,eRFPowerState));
	return TRUE;
}

VOID
HaltNic8187(
    IN PNIC         Nic,
    IN BOOLEAN          bReset
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s=====>\n", __func__));

	NDIS_STATUS status = FALSE;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

    if(pHalData->hw_init_completed ==TRUE)
    {
    	MpTrace(COMP_RICHD, DBG_NORMAL, ("Func_Of_Proc_Chip_Hw_Deinit\n"));
		
    	status = Func_Of_Proc_Chip_Hw_Deinit(Nic);
    	
    	if (status == FALSE) {
    		MpTrace(COMP_RICHD, DBG_NORMAL, ("===>%s,fail!\n", __func__));
    	 }
    }
}

void
HwRemoveKeyEntry(
    __in PNIC Nic,
    __in UCHAR Index,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>Index=%d AlgoId=%d\n",
		                             __func__,Index,AlgoId));
	cam_entry_clean(Nic, Index);
}

#define _NO_PRIVACY_		0x0
#define _WEP40_				0x1
#define _TKIP_				0x2
#define _TKIP_WTMIC_		0x3
#define _AES_				0x4
#define _WEP104_			0x5
#define _WEP_WPA_MIXED_		0x07
#define _SMS4_				0x06

void _set_cam(PNIC Nic, u8 * mac, u8 * key, u8 id, u16 ctrl);
void
HwAddKeyEntry(
	__in PNIC Nic,
	__in PNICKEY pNicKey,
	__in UCHAR Index)
{
	u8 algorithm;
	u16 ctrl = 0;
	s16 cam_id = 0;
	u8 temp = 0xcc;
	u8 join_type = 0;
	unsigned char null_addr[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>index=%d\n", __func__, Index));

	switch (pNicKey->AlgoId) {
	case DOT11_CIPHER_ALGO_WEP40:
		algorithm = _WEP40_;
		break;
	case DOT11_CIPHER_ALGO_WEP104:
		algorithm = _WEP104_;
		break;
	case DOT11_CIPHER_ALGO_CCMP:
		algorithm = _AES_;
		break;
	case DOT11_CIPHER_ALGO_TKIP:
		algorithm = _TKIP_;
		break;
	default:
		algorithm = _NO_PRIVACY_;
	}

	switch (pNicKey->AlgoId) {
	case DOT11_CIPHER_ALGO_WEP40:
	case DOT11_CIPHER_ALGO_WEP104:
		cam_id = Index;
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_SEC_CFG, &temp);
		ctrl = BIT(15) | ((algorithm) << 2) | (Index);
		_set_cam(Nic, pNicKey->MacAddr, pNicKey->KeyValue, cam_id, ctrl);
		temp = _TRUE;
	    Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_SEC_DK_CFG, &temp);
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_ON_RCR_AM, null_addr);
		break;
	case DOT11_CIPHER_ALGO_CCMP:
	case DOT11_CIPHER_ALGO_TKIP:
		if (Nic->ManagementInfo.KeyProcKeyID == Index) {
			cam_id = 4;
			Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_SEC_CFG, &temp);
			ctrl = BIT(15) | ((algorithm) << 2) | (Index - 4);
			_set_cam(Nic, pNicKey->MacAddr, pNicKey->KeyValue, cam_id, ctrl);
			Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_ON_RCR_AM, null_addr);
		}

		if (Nic->ManagementInfo.DefKeyProcKeyID == Index) {
			cam_id = 5;
			ctrl = BIT(15) | BIT(6) | ((algorithm) << 2) | (Index);
			_set_cam(Nic, pNicKey->MacAddr, pNicKey->KeyValue, cam_id, ctrl);
		}
		break;
	default:;
	}
	

}

void SwChnlPhy(
    PNIC    pNic,
    UCHAR   channel
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>channel=%d\n", __func__,channel));
	do_set_chabw(pNic, channel, 0, 0);
}

void
HwSetParamBeforeScan(
	__in PNIC pNic
)
{
	u8 val8;
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
	val8 = 1;
	Func_Of_Proc_Chip_Hw_Set_Hwreg(pNic, HW_VAR_MLME_SITESURVEY, (u8 *)(&val8));
}

void
HwRestoreToBeforeScan(
	__in PNIC pNic
)
{
	u8 val8;
	u8 join_type = 0;
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
	val8 = 0;
	Func_Of_Proc_Chip_Hw_Set_Hwreg(pNic, HW_VAR_MLME_SITESURVEY, (u8 *)(&val8));
	Func_Of_Proc_Chip_Hw_Set_Hwreg(pNic, HW_VAR_BSSID, pNic->ManagementInfo.BssId);
	Func_Of_Proc_Chip_Hw_Set_Hwreg(pNic, HW_VAR_MLME_JOIN, (u8 *)(&join_type));

	//proc_joinbss_cmd_func(pNic,NULL);
}

void
HwPeriodTimerHandler(
	__in PNIC pNic
	)
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
}
#if 1
VOID TxFillDescriptorRich(    IN  PNIC        Nic,
                         struct xmit_frame * xmitframe,
                                    u8 * pbuf)
{
#if 0
	struct sta_info *psta;

	union Keytype dot11tkiptxmickey;
	union Keytype dot118021x_UncstKey;
	  
    xmitframe->attrib.type
    xmitframe->attrib.subtype
    xmitframe->attrib.bswenc
    xmitframe->attrib.dhcp_pkt
    xmitframe->attrib.seqnum
    xmitframe->attrib.hw_ssn_sel
    xmitframe->attrib.pkt_hdrlen
    xmitframe->attrib.hdrlen
    xmitframe->attrib.pktlen
    xmitframe->attrib.last_txcmdsz
    xmitframe->attrib.nr_frags
    xmitframe->attrib.encrypt          =bEncrypt;
    xmitframe->attrib.iv_len
    xmitframe->attrib.icv_len
    xmitframe->attrib.iv[18]
    xmitframe->attrib.icv[16]
    xmitframe->attrib.priority
    xmitframe->attrib.ack_policy
    xmitframe->attrib.mac_id
    xmitframe->attrib.vcs_mode
    xmitframe->attrib.dst[ETH_ALEN]
    xmitframe->attrib.src[ETH_ALEN]
    xmitframe->attrib.ta[ETH_ALEN]
    xmitframe->attrib.ra[ETH_ALEN]
    xmitframe->attrib.key_idx
    xmitframe->attrib.qos_en
    xmitframe->attrib.ht_en
    xmitframe->attrib.raid
    xmitframe->attrib.bwmode
    xmitframe->attrib.ch_offset
    xmitframe->attrib.sgi              =bUseShortPreamble;
    xmitframe->attrib.ampdu_en
    xmitframe->attrib.ampdu_spacing
    xmitframe->attrib.mdata
    xmitframe->attrib.pctrl
    xmitframe->attrib.triggered
    xmitframe->attrib.qsel
    xmitframe->attrib.order
    xmitframe->attrib.eosp
    xmitframe->attrib.rate
    xmitframe->attrib.intel_proxim
    xmitframe->attrib.retry_ctrl
    xmitframe->attrib.mbssid
    xmitframe->attrib.ldpc
    xmitframe->attrib.stbc
    xmitframe->attrib.rtsen
    xmitframe->attrib.cts2self
    xmitframe->attrib.key_type

    xmitframe->attrib.icmp_pkt
	xmitframe.attrib.rsvd_packet
#endif

	Func_Of_Update_Txdesc(Nic,xmitframe,pbuf);
}
#endif

unsigned int GetQosOption(PNIC Nic)
{
	return Nic->Station->QosInfo.qos_option;
}

VOID
TxFillDescriptor8187(
    IN  PNIC        Nic,
    IN  PTX_DESC_8187   TxDesc,
    IN  UCHAR           QueueIndex,
    IN  PUCHAR          pHeader,
    IN  BOOLEAN         bFirstSeg,
    IN  BOOLEAN         bLastSeg,
    IN  PUCHAR          VirtualAddress,
    IN  ULONG_PTR           BufferLen,
    IN  ULONG           PktLen,
    IN  USHORT          Rate,
    IN  BOOLEAN         bMulticast,
    IN  BOOLEAN         bUseShortPreamble,
    IN  BOOLEAN         bMoreFrag,
    IN  BOOLEAN         bCTSEnable,
    IN  BOOLEAN         bRTSEnable,
    IN  USHORT          RTSDuration,
    IN  USHORT          RTSRate,
    IN  BOOLEAN         bEncrypt,         //
    IN  UCHAR           RetryLimit,
    IN  PVOID           Reserved   
    )
{
	struct xmit_frame tempxmitframe, *xmitframe;
	PNICKEY pNicKey = &(Nic->ManagementInfo.KeyProcTable);
	PSTATION        pStation = Nic->Station;
    //MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
	MP_DOT11_MGMT_DATA_MAC_HEADER *pFrameHead = pHeader;
	unsigned char *pbuf = pHeader;
	xmitframe = &tempxmitframe;
	PQOS_INFO PQosInfo = &(pStation->QosInfo);

	NdisZeroMemory(TxDesc, TXDESC_SIZE);
	NdisZeroMemory(xmitframe, sizeof(struct xmit_frame));
	if (pFrameHead->FrameControl.Type == DOT11_FRAME_TYPE_DATA) {
		xmitframe->frame_tag = DATA_FRAMETAG;
	}
	else if (pFrameHead->FrameControl.Type == DOT11_FRAME_TYPE_MANAGEMENT) {
		xmitframe->frame_tag = MGNT_FRAMETAG;
	}

	xmitframe->attrib.qos_en = 0;

	if ((PQosInfo->qos_option) && (pFrameHead->FrameControl.Type == DOT11_FRAME_TYPE_DATA))
	{

		DbgPrint("===========LEN1:%x,pk:%x\n", BufferLen,PktLen);
		BufferLen = BufferLen + 2;
		PktLen = PktLen +2 ;
		xmitframe->attrib.qos_en = 1;
		
		RtlMoveMemory(pbuf+26,pbuf+24, BufferLen-26);
		
		NdisZeroMemory(pbuf+24,2);

		if (PQosInfo->priority)
			(*(unsigned short *)(pbuf+24)) |= (PQosInfo->priority);
			//*(unsigned short *)(pbuf) |= cpu_to_le16((PQosInfo->priority) & 0xf);

		(*(unsigned short *)(pbuf+24))  |= ((PQosInfo->eosp & 1) << 4);
		(*(unsigned short *)(pbuf+24)) |= ((PQosInfo->ack_policy & 3) << 5);

		//*(unsigned short *)(pbuf) |= cpu_to_le16( (PQosInfo->eosp & 1) << 4); 
		//*(unsigned short *)(pbuf) |= cpu_to_le16( (PQosInfo->ack_policy & 3) << 5);

		pFrameHead->FrameControl.Subtype |= DOT11_DATA_SUBTYPE_QOS_DATA;
	}

	xmitframe->attrib.bswenc = 0;
	xmitframe->attrib.dhcp_pkt = 0;
	xmitframe->attrib.seqnum = 0;
	xmitframe->attrib.hw_ssn_sel = 0;
	xmitframe->attrib.pkt_hdrlen = 0;
	xmitframe->attrib.hdrlen = 0;
	xmitframe->attrib.pktlen = PktLen;
	xmitframe->attrib.last_txcmdsz = PktLen;
	xmitframe->attrib.nr_frags = 0;	
	if (pStation->Config.UnicastCipherAlgorithm != DOT11_CIPHER_ALGO_NONE &&
		pFrameHead->FrameControl.WEP) {
		xmitframe->attrib.encrypt = pNicKey->AlgoId;
	}
	else {
		xmitframe->attrib.encrypt = 0;
	}
	xmitframe->attrib.iv_len = 0;
	xmitframe->attrib.icv_len = 0;
	//	xmitframe->attrib.iv[18]
	//	xmitframe->attrib.icv[16]
	xmitframe->attrib.priority = 0;
	xmitframe->attrib.ack_policy = 0;
	xmitframe->attrib.mac_id = 0;
	xmitframe->attrib.vcs_mode = 0;
		//xmitframe->attrib.dst[ETH_ALEN]
		//xmitframe->attrib.src[ETH_ALEN]
		//xmitframe->attrib.ta[ETH_ALEN]
		//xmitframe->attrib.ra[ETH_ALEN]
	xmitframe->attrib.key_idx = 0;
	xmitframe->attrib.ht_en = 0;
	xmitframe->attrib.raid = 0;
	xmitframe->attrib.bwmode = 0;
	xmitframe->attrib.ch_offset = 0;
	xmitframe->attrib.sgi = 0;// bUseShortPreamble;
	xmitframe->attrib.ampdu_en = 0;
	xmitframe->attrib.ampdu_spacing = 0;
	xmitframe->attrib.mdata = 0;
	xmitframe->attrib.pctrl = 0;
	/*xmitframe->attrib.triggered
		xmitframe->attrib.qsel
		xmitframe->attrib.order
		xmitframe->attrib.eosp*/
	xmitframe->attrib.rate = Rate;
		//xmitframe->attrib.intel_proxim
		//xmitframe->attrib.retry_ctrl
		//xmitframe->attrib.mbssid
		//xmitframe->attrib.ldpc
		//xmitframe->attrib.stbc
		//xmitframe->attrib.rtsen
		//xmitframe->attrib.cts2self
		//xmitframe->attrib.key_type

		//xmitframe->attrib.icmp_pkt
		//xmitframe.attrib.rsvd_packet
	TxFillDescriptorRich(Nic, xmitframe, TxDesc);


}
///////////////////////////////////////////////////////////////////////////
// called in HwInitializeVariable
// Updates hardware specific variables and some of the Native Wi-Fi capability
// to match the hardware capabilities
///////////////////////////////////////////////////////////////////////////
VOID
Init8185(
    IN  PNIC        pNic
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));

    Func_Of_Proc_Pre_Mutex_Init(&pNic->MsgMutex);
    
    if (Func_Of_Proc_Chip_Hw_Power_On(pNic) == FALSE) {
        MpTrace(COMP_RICHD, DBG_NORMAL, ("Power_On Failed\n"));
    }else
    	{
        MpTrace(COMP_RICHD, DBG_NORMAL, ("Power_On OK\n"));	
    	}
    //
    
    MpTrace(COMP_RICHD, DBG_NORMAL, ("[write bin start]\n"));
    Func_Mcu_Go(pNic);
    MpTrace(COMP_RICHD, DBG_NORMAL, ("[write bin end]\n"));
    
    if (Func_Mcu_Message_Up(pNic) == FALSE) {
        MpTrace(COMP_RICHD, DBG_NORMAL, ("Mcu_Message_Up failed\n"));
    }else
    	{
	    MpTrace(COMP_RICHD, DBG_NORMAL, ("Mcu_Message_Up OK\n"));
    	}
	
    Func_Set_Concurrent_Func(pNic);
    
    Func_Of_Proc_Chip_Hw_Read_Chip_Version(pNic);
    
    Func_Of_Proc_Chip_Hw_Chip_Configure(pNic);
    
    Func_Of_Proc_Chip_Hw_Read_Chip_Info(pNic);
		

	if (Func_Loadparam(pNic) == FALSE)
	{
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Func_Loadparam Error!\n"));
	}
	else
	{
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Func_Loadparam success!\n"));
	}
	
	if (Func_Of_Proc_Init_Drv_Sw(pNic) == FALSE)
	{
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Initialize software resourceFailed!\n"));
	}
	else
	{
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Initialize software resource success!\n"));
	}
	
}

void LedControlCallback8187(
  WDFTIMER Timer
)
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
}


VOID
HwSetNicPSMode(
    PNIC    Nic,
    RT_PS_MODE  PowerSaveMode
)
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
}

BOOLEAN
SetupBeaconQueue8187(
    PNIC    Nic
)
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
	return TRUE;
}

void
SetEncryption8187(
    IN PNIC  pNic,
    IN DOT11_CIPHER_ALGORITHM algoId,
    IN BOOLEAN useDefaultKey
    )
{
   MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>algoId=%d DefaultKe=%d\n",
   	                  __func__,algoId,useDefaultKey));
}

void
HwSetBSSIDAndBcnIntv(
    __in  PNIC pNic
    )
{
	u8 join_type = 0;
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
	pNic->ManagementInfo.KeyProcKeyID = 0xFF;
	pNic->ManagementInfo.DefKeyProcKeyID = 0xFF;
	Func_Of_Proc_Chip_Hw_Set_Hwreg(pNic, HW_VAR_BSSID, pNic->ManagementInfo.BssId);
	Func_Of_Proc_Chip_Hw_Set_Hwreg(pNic, HW_VAR_MLME_JOIN, (u8 *)(&join_type));
}



NDIS_STATUS 
HwSetNICAddress(
    __in  PNIC Nic
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
    return NDIS_STATUS_SUCCESS;

}


VOID
HwInternalStartReqComplete(
    __in  PNIC        pNic
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
}

void
HwSaveAdjustBeforeScan(
    IN PNIC pNic
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
}

ULONG
HwRSSI(
    PNIC        pNic,
    RX_STATUS_DESC_RICHD     RxDescRichD
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
	PNIC_RX_FRAGMENT pNicFragment= CONTAINING_RECORD (&RxDescRichD, NIC_RX_FRAGMENT, RxDescRichD);
	return ((ULONG) pNicFragment->attrib.phy_info.SignalStrength);
}

void
HwPostSendHandler(
    __in PNIC     pNic,
    __in ULONG    sentBufferLength
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
}


VOID
QueryRxDescStatus8187(
    IN  PNIC        Nic,
    IN  PVOID    pDesc,
    OUT PRX_STATUS_DESC_RICHD  pRtRfdStatus
    )
{
	//MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
	
	PNIC_RX_FRAGMENT pNicFragment= CONTAINING_RECORD (pRtRfdStatus, NIC_RX_FRAGMENT, RxDescRichD);

    u8 *pdesc;
	pdesc = (u8*)pDesc;
	u8 rate;

	
	struct rx_pkt_attrib *pattrib = &pNicFragment->attrib;
	

	memset(pattrib, 0, sizeof(struct rx_pkt_attrib));

	pattrib->pkt_len = (u16) GET_RX_STATUS_DESC_PKT_LEN_9086X(pdesc);
	

	//
	//pRtRfdStatus->Length = pattrib->pkt_len;
	pRtRfdStatus->Length =GET_RX_STATUS_DESC_PKT_LEN_9086X(pdesc);

	pRtRfdStatus->Rx_moreFrag = 0;
	pRtRfdStatus->Rx_moreData = 0;
	pRtRfdStatus->Decrypted = 1;
	pRtRfdStatus->ICV = 0;
	pRtRfdStatus->FOVF = 0;
	pRtRfdStatus->CRCError = 0;
	
	rate = GET_RX_STATUS_DESC_RXRATE_9086X(pdesc);
	if (rate >= 0x14 && rate <= 0x1B) {
			pRtRfdStatus->RxRate = rate - 0x08;
	} else if (rate >= 0x1C && rate <= 0x23) {
			pRtRfdStatus->RxRate = rate - 0x10;
	} else if (rate >= 0x24 && rate <= 0x2B) {
			pRtRfdStatus->RxRate = rate - 0x18;
	} else if (rate < 0x14) {
			pRtRfdStatus->RxRate = rate;
	}
    

	pattrib->pkt_rpt_type = GET_RX_STATUS_DESC_RPT_SEL_9086X(pdesc) ? wMBOX0_PACKET : NORMAL_RX;
	if (pattrib->pkt_rpt_type == NORMAL_RX) {
		pattrib->crc_err = 0;
		pattrib->icv_err = 0;
		pattrib->drvinfo_sz = 32;
		pattrib->encrypt = 0;
		pattrib->qos = 0;
		pattrib->shift_sz = 0;
		pattrib->physt = 0;
		pattrib->bdecrypted = 1;
		pattrib->physt = (u8) GET_RX_STATUS_DESC_PHY_STATUS_9086X(pdesc);

		pattrib->priority = (u8) GET_RX_STATUS_DESC_TID_9086X(pdesc);
		pattrib->amsdu = 0;
		pattrib->mdata = 0;
		pattrib->mfrag = 0;

		pattrib->seq_num = (u16) GET_RX_STATUS_DESC_SEQ_9086X(pdesc);
		pattrib->frag_num = 0;

		rate = GET_RX_STATUS_DESC_RXRATE_9086X(pdesc);

		if ((rate >= 0x14 && rate <= 0x1B) || (rate >= 0x24 && rate <= 0x2B))
			pattrib->sgi = 1;
		else
			pattrib->sgi = 0;

		if (rate <= 0x1b)
			pattrib->bw = 0;
		else
			pattrib->bw = 1;

		if (rate >= 0x14 && rate <= 0x1B) {
			pattrib->data_rate = rate - 0x08;
		} else if (rate >= 0x1C && rate <= 0x23) {
			pattrib->data_rate = rate - 0x10;
		} else if (rate >= 0x24 && rate <= 0x2B) {
			pattrib->data_rate = rate - 0x18;
		} else if (rate < 0x14) {
			pattrib->data_rate = rate;
		}
/*
	MpTrace(COMP_RICHD, DBG_NORMAL, 
					 ("    NORMAL_RX:pkt_len=%d encrypt=%d qos=%d phystatus=%d amsdu=%d rate=%d seq_num=%d\n",
					  pattrib->pkt_len,pattrib->encrypt,pattrib->qos,pattrib->physt,pattrib->amsdu,rate,pattrib->seq_num));
*/
	}else
	{
		MpTrace(COMP_RICHD, DBG_NORMAL, ("    wMBOX0_PACKET\n"));
	}
	
}


ULONG
RF_ReadReg(
    __in  PNIC    Nic,
    __in  UCHAR      offset
    )
{
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====>\n", __func__));
    return 0;
}


