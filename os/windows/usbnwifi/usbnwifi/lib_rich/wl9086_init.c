/******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/
#include "globals.h"
#include "ndis.h"
#include "hw_pcomp.h"
#include "hw_main.h"
#include "hw_Nic.h"
#include "hw_func.h"
#include "usb_main.h"
#include "bus_intf.h"
#include "8187_gen.h"

void Func_Init_Hal_Spec_Process(PNIC Nic)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(Nic);

	hal_spec->ic_name = "wll9086x";
	hal_spec->macid_num = 16;
	hal_spec->sec_cam_ent_num = 16;
	hal_spec->sec_cap = 0;
	hal_spec->nss_num = 1;
	hal_spec->band_cap = BAND_CAP_2G;
	hal_spec->bw_cap = BW_CAP_20M | BW_CAP_40M;
	hal_spec->proto_cap = PROTO_CAP_11B | PROTO_CAP_11G | PROTO_CAP_11N;

	hal_spec->wl_func = 0 | WL_FUNC_P2P | WL_FUNC_MIRACAST | WL_FUNC_TDLS;
}

u8 Func_To_Get_Eeprom_Size_Process(PNIC Nic)
{
	u8 size = 0;
	int ret = FALSE;
	u32 cr;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_EFUSETYPE, NULL, &cr,
									0, 1);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Error :===>%s,fail,code :UMSG_OPS_HAL_EFUSETYPE!!!\n", __func__));
	}
	size = (cr & BOOT_FROM_EEPROM) ? 6 : 4;
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("EEPROM type is %s\n", size == 4 ? "E-FUSE" : "93C46"));
	
	return size;;
}


static void Func_Of_Cal_Txdesc_Chksum(struct tx_desc *ptxdesc)
{
	u16 *usPtr = (u16 *) ptxdesc;
	u32 count;
	u32 index;
	u16 checksum = 0;

	ptxdesc->txdw7 &= cpu_to_le32(0xffff0000);

	count = 16;

	for (index = 0; index < count; index++)
		checksum ^= le16_to_cpu(*(usPtr + index));

	ptxdesc->txdw7 |= cpu_to_le32(checksum & 0x0000ffff);

}


static u8 Func_Bwmapping_Process(IN PNIC Nic, IN struct pkt_attrib * pattrib)
{
	u8 BWSettingOfDesc = 0;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

	if (pHalData->CurrentChannelBW == CHANNEL_WIDTH_40) {
		if (pattrib->bwmode == CHANNEL_WIDTH_40)
			BWSettingOfDesc = 1;
		else
			BWSettingOfDesc = 0;
	} else
		BWSettingOfDesc = 0;

	return BWSettingOfDesc;
}

static u8 Func_Scmapping_Process(PNIC Nic, struct pkt_attrib * pattrib)
{
	u8 SCSettingOfDesc = 0;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

	if (pHalData->CurrentChannelBW == CHANNEL_WIDTH_40) {

		if (pattrib->bwmode == CHANNEL_WIDTH_40)
			SCSettingOfDesc = HT_DATA_SC_DONOT_CARE;
		else if (pattrib->bwmode == CHANNEL_WIDTH_20) {
			if (pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER)
				SCSettingOfDesc = HT_DATA_SC_20_UPPER_OF_40MHZ;
			else if (pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER)
				SCSettingOfDesc = HT_DATA_SC_20_LOWER_OF_40MHZ;
			else
				SCSettingOfDesc = HT_DATA_SC_DONOT_CARE;
		}
	} else
		SCSettingOfDesc = HT_DATA_SC_DONOT_CARE;

	return SCSettingOfDesc;
}

static u8 Func_Fill_Txdesc_Sectype(struct pkt_attrib *pattrib)
{
	u8 sectype = 0;

	if ((pattrib->encrypt > 0) && !pattrib->bswenc) {
		switch (pattrib->encrypt) {
		case _WEP40_:
		case _WEP104_:
			sectype = 1;
			break;
		case _TKIP_:
		case _TKIP_WTMIC_:
			sectype = 2;
			break;
		case _AES_:
			sectype = 3;
			break;

		case _NO_PRIVACY_:
			sectype = 0;
			break;
		default:
			sectype = 4;
			break;
		}
	}

	return sectype;
}

void Func_Wf_Config_Xmit(PNIC Nic, int event, u32 val)
{
	u32 temp;

	temp = HwPlatformIORead4Byte(Nic, WF_XMIT_CTL);
	

	if (event & WF_XMIT_AGG_MAXNUMS) {
		temp = temp & 0x07FFFFFF;
		val = val & 0x1F;
		temp = temp | (val << 27);
	}

	if (event & WF_XMIT_AMPDU_DENSITY) {
		temp = temp & 0xFFFF1FFF;
		val = val & 0x07;
		temp = temp | (val << 13);

	}

	if (event & WF_XMIT_OFFSET) {
		temp = temp & 0xFFFFFF00;
		val = val & 0xFF;
		temp = temp | (val << 0);
	}

	if (event & WF_XMIT_PKT_OFFSET) {
		temp = temp & 0xFFFFE0FF;
		val = val & 0x1F;
		temp = temp | (val << 8);
	}

	HwPlatformIOWrite4Byte(Nic, WF_XMIT_CTL, temp);
	MpTrace(COMP_RICHD, DBG_NORMAL,("%s \n", __func__));
}

static void Func_Fill_Txdesc_Vcs_Process(PNIC Nic, struct pkt_attrib *pattrib,
								  u8 * ptxdesc)
{

	WF_TX_DESC_HW_RTS_ENABLE_9086X(ptxdesc, 0);


	switch (pattrib->vcs_mode) {
	case RTS_CTS:
		WF_TX_DESC_RTS_ENABLE_9086X(ptxdesc, 1);
		break;
	case CTS_TO_SELF:
		WF_TX_DESC_CTS2SELF_9086X(ptxdesc, 1);
		break;
	case NONE_VCS:
	default:
		break;
	}

	if (pattrib->vcs_mode) {
		WF_TX_DESC_RTSRATE_FB_CFG_9086X(ptxdesc, 1);

		WF_TX_DESC_RTSRATE_CFG_9086X(ptxdesc, 1);

		if (Nic->mlmeextpriv.mlmext_info.preamble_mode == PREAMBLE_SHORT)
			WF_TX_DESC_RTS_SHORT_9086X(ptxdesc, 1);

		if (pattrib->ht_en)
			WF_TX_DESC_RTS_SC_9086X(ptxdesc,
									Func_Scmapping_Process(Nic, pattrib));
	} else {

	}
}

static void Func_Fill_Txdesc_Phy_Process(PNIC Nic, struct pkt_attrib *pattrib,
								  u8 * ptxdesc)
{

	if (pattrib->ht_en) {
		WF_TX_DESC_DATA_BW_9086X(ptxdesc, Func_Bwmapping_Process(Nic, pattrib));
		WF_TX_DESC_DATA_SC_9086X(ptxdesc, Func_Scmapping_Process(Nic, pattrib));
	}
}
#if 1

static inline u32 wifi_mac_hash(const u8 * mac)
{
	u32 x;

	x = mac[0];
	x = (x << 2) ^ mac[1];
	x = (x << 2) ^ mac[2];
	x = (x << 2) ^ mac[3];
	x = (x << 2) ^ mac[4];
	x = (x << 2) ^ mac[5];

	x ^= x >> 8;
	x = x & (32 - 1);

	return x;
}

__inline static SINGLE_LIST_ENTRY *get_next(SINGLE_LIST_ENTRY * list)
{
	return list->Next;
}

#define LIST_CONTAINOR(ptr, type, member) \
        ((type *)((char *)(ptr)-(SIZE_T)(&((type *)0)->member)))
        
struct sta_info *do_query_stainfo(struct sta_priv *pstapriv, const u8 * hwaddr,
								 u8 tag)
{
	u32 index;
	const u8 *addr;
	SINGLE_LIST_ENTRY *plist, *phead;
	struct sta_info *psta = NULL;
	const u8 bc_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	_func_enter_;

	if (hwaddr == NULL)
		return NULL;

	if (IS_MCAST((unsigned char *)hwaddr))
		addr = bc_addr;
	else
		addr = hwaddr;

	index = wifi_mac_hash(addr);

	if (tag) {
//		spin_lock_bh(&pstapriv->sta_hash_lock);

		phead = &(pstapriv->sta_hash[index]);
		plist = get_next(phead);

		while ((Func_Of_Proc_End_Of_Queue_Search(phead, plist)) == _FALSE) {

			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);

			if ((Func_Of_Proc_Pre_Memcmp(psta->hwaddr, addr, ETH_ALEN)) == _TRUE) {
				break;
			}
			psta = NULL;
			plist = get_next(plist);
		}

//		spin_unlock_bh(&pstapriv->sta_hash_lock);
	}
	_func_exit_;
	return psta;

}


struct sta_info *do_query_bcmc_stainfo(PNIC Nic, u8 tag)
{
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &Nic->stapriv;
	u8 bc_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	_func_enter_;
	psta = Nic->stapriv.sta_aid[0];// ddliu  32 buff
	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====psta=%x\n", __func__,psta));
	_func_exit_;
	return psta;

}

static void Func_Of_Fill_Default_Txdesc(PNIC Nic, struct xmit_frame *pxmitframe,
										 u8 * pbuf)
{
//	PNIC Nic;
	HAL_DATA_TYPE *pHalData;
	struct mlme_ext_priv *pmlmeext;
	struct mlme_ext_info *pmlmeinfo;
	struct pkt_attrib *pattrib;
	s32 bmcst;
	u64 tx_ra_bitmap = 0;
	struct sta_info *psta = do_query_bcmc_stainfo(Nic, 1);

	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====psta=%x,Nic:%x\n", __func__,psta,Nic));

	pattrib = &pxmitframe->attrib;
	pHalData = GET_HAL_DATA(Nic);
	
	Func_Of_Proc_Chip_Hw_Update_Sta_Rate_Mask(Nic,psta);
	tx_ra_bitmap = psta->ra_mask;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====wirlessmode=%x\n", __func__,psta->wireless_mode));
	
	pattrib->raid = get_netype_to_raid_ex(Nic,psta, 1);

	Func_Of_Proc_Pre_Memset(pbuf, 0, TXDESC_SIZE);
#if 0
	Nic = pxmitframe->Nic;
#endif

	
	pmlmeext = &Nic->mlmeextpriv;
	pmlmeinfo = &(pmlmeext->mlmext_info);

	pattrib = &pxmitframe->attrib;
	bmcst = IS_MCAST(pattrib->ra);

	if (pxmitframe->frame_tag == DATA_FRAMETAG) {
		u8 drv_userate = 0;

		WF_TX_DESC_MACID_9086X(pbuf, pattrib->mac_id);
#if XMIT_OLD_TEST
		WF_TX_DESC_RATE_ID_9086X(pbuf, pattrib->raid);
#else
		if (pattrib->raid <= 8) {
			WF_TX_DESC_RATE_ID_9086X(pbuf, pattrib->raid);
		} else {
			WF_TX_DESC_RATE_ADP_9086X(pbuf, pattrib->raid - 9);
		}
#endif
		WF_TX_DESC_QUEUE_SEL_9086X(pbuf, pattrib->qsel);
		WF_TX_DESC_SEQ_9086X(pbuf, pattrib->seqnum);

		WF_TX_DESC_SEC_TYPE_9086X(pbuf, Func_Fill_Txdesc_Sectype(pattrib));
		Func_Fill_Txdesc_Vcs_Process(Nic, pattrib, pbuf);

		if ((pattrib->ether_type != 0x888e) &&
			(pattrib->ether_type != 0x0806) &&
			(pattrib->ether_type != 0x88B4) &&
			(pattrib->dhcp_pkt != 1) && (drv_userate != 1)
			) {

			if (pattrib->ampdu_en == TRUE) {
				WF_TX_DESC_AGG_ENABLE_9086X(pbuf, 1);
				WF_TX_DESC_AGGNUM_CFG_9086X(pbuf, 1);
				WF_TX_DESC_MINISPACE_CFG_9086X(pbuf, 1);
			} else {
				WF_TX_DESC_AGG_BREAK_9086X(pbuf, 1);
			}

			Func_Fill_Txdesc_Phy_Process(Nic, pattrib, pbuf);

#if XMIT_OLD_TEST
			WF_TX_DESC_DATARATE_FB_CFG_9086X(pbuf, 1);
#else
			WF_TX_DESC_DATARATE_FB_CFG_9086X(pbuf, 1);
#endif

#ifdef CONFIG_CMCC_TEST
			SET_TX_DESC_DATA_SHORT_9086X(pbuf, 1);
#endif
		} else {

			WF_TX_DESC_AGG_BREAK_9086X(pbuf, 1);
			WF_TX_DESC_USE_RATE_9086X(pbuf, 1);
			if (pmlmeinfo->preamble_mode == PREAMBLE_SHORT)
				WF_TX_DESC_DATA_SHORT_9086X(pbuf, 1);
			WF_TX_DESC_TX_RATE_9086X(pbuf, Func_Mratetohwrate(pmlmeext->tx_rate));
#ifdef CONFIG_USB_TX_AGGREGATION
#endif
		}

#if defined(CONFIG_USB_TX_AGGREGATION)
		WF_TX_DESC_USB_TXAGG_NUM_9086X(pbuf, pxmitframe->agg_num);
#endif

	} else if (pxmitframe->frame_tag == MGNT_FRAMETAG) {

		WF_TX_DESC_MACID_9086X(pbuf, pattrib->mac_id);
		WF_TX_DESC_QUEUE_SEL_9086X(pbuf, pattrib->qsel);

#if XMIT_OLD_TEST
		WF_TX_DESC_RATE_ID_9086X(pbuf, pattrib->raid);

#else
		if (pattrib->raid <= 8) {
			WF_TX_DESC_RATE_ID_9086X(pbuf, pattrib->raid);
		} else {
			WF_TX_DESC_RATE_ADP_9086X(pbuf, pattrib->raid - 9);
		}

#endif

		WF_TX_DESC_SEQ_9086X(pbuf, pattrib->seqnum);
		WF_TX_DESC_USE_RATE_9086X(pbuf, 1);
		WF_TX_DESC_RSVD_9086X(pbuf, pattrib->rsvd_packet);
		WF_TX_DESC_MBSSID_9086X(pbuf, pattrib->mbssid & 0xF);
		if (pattrib->rsvd_packet) {
		}
#if XMIT_OLD_TEST
		WF_TX_DESC_RETRY_LIMIT_ENABLE_9086X(pbuf, 1);

		if (pattrib->retry_ctrl == TRUE) {
			WF_TX_DESC_DATA_RETRY_LIMIT_9086X(pbuf, 6);
		} else {
			WF_TX_DESC_DATA_RETRY_LIMIT_9086X(pbuf, 12);
		}

#else
		WF_TX_DESC_RETRY_LIMIT_ENABLE_9086X(pbuf, 1);

		if (pattrib->retry_ctrl == TRUE) {
			WF_TX_DESC_DATA_RETRY_LIMIT_9086X(pbuf, 0);

		} else {
			WF_TX_DESC_DATA_RETRY_LIMIT_9086X(pbuf, 1);
		}

#endif
		{
			WF_TX_DESC_TX_RATE_9086X(pbuf, Func_Mratetohwrate(pmlmeext->tx_rate));
		}

#ifdef CONFIG_XMIT_ACK
		if (pxmitframe->ack_report) {

			WF_TX_DESC_SPE_RPT_9086X(pbuf, 1);
			WF_TX_DESC_SW_DEFINE_9086X(pbuf,
									   (u8) (Nic->xmitpriv.seq_no));
		}
#endif
	} else if (pxmitframe->frame_tag == TXAGG_FRAMETAG)
    {
	    MpTrace(COMP_RICHD, DBG_NORMAL,("%s: TXAGG_FRAMETAG\n", __func__));
    }
	else {
	    MpTrace(COMP_RICHD, DBG_NORMAL,("%s: frame_tag=0x%x\n", __func__, pxmitframe->frame_tag));
		WF_TX_DESC_MACID_9086X(pbuf, pattrib->mac_id);
		
#if XMIT_OLD_TEST
		WF_TX_DESC_RATE_ID_9086X(pbuf, pattrib->raid);
#else
		if (pattrib->raid <= 8) {
			WF_TX_DESC_RATE_ID_9086X(pbuf, pattrib->raid);
		} else {
			WF_TX_DESC_RATE_ADP_9086X(pbuf, pattrib->raid - 9);
		}

#endif

		WF_TX_DESC_QUEUE_SEL_9086X(pbuf, pattrib->qsel);
		WF_TX_DESC_SEQ_9086X(pbuf, pattrib->seqnum);
		WF_TX_DESC_USE_RATE_9086X(pbuf, 1);
		WF_TX_DESC_TX_RATE_9086X(pbuf, Func_Mratetohwrate(pmlmeext->tx_rate));
	}

	WF_TX_DESC_PKT_SIZE_9086X(pbuf, pattrib->last_txcmdsz);

	if (bmcst)
		WF_TX_DESC_BMC_9086X(pbuf, 1);

	if (!pattrib->qos_en)
		WF_TX_DESC_HWSEQ_EN_9086X(pbuf, 1);
}

void Func_Of_Update_Txdesc(PNIC Nic,struct xmit_frame *pxmitframe, u8 * pbuf)
{


	Func_Of_Fill_Default_Txdesc(Nic, pxmitframe, pbuf);

	Func_Of_Cal_Txdesc_Chksum((struct tx_desc *)pbuf);
}
#endif

static void Func_Ccx_Fwc2Htxrpt_Process(PNIC Nic, u8 * pdata, u8 len)
{
	u8 seq_no;

#define	GET_9086X_wMBOX0_TX_RPT_LIFE_TIME_OVER(_Header)	LE_BITS_TO_1BYTE((_Header + 0), 6, 1)
#define	GET_9086X_wMBOX0_TX_RPT_RETRY_OVER(_Header)	LE_BITS_TO_1BYTE((_Header + 0), 7, 1)

	seq_no = *(pdata + 6);

#if 0//def CONFIG_XMIT_ACK
	if (GET_9086X_wMBOX0_TX_RPT_RETRY_OVER(pdata) |
		GET_9086X_wMBOX0_TX_RPT_LIFE_TIME_OVER(pdata))
		wl_finish_ack_tx(WL_SCTX_DONE_CCX_PKT_FAIL, &Nic->xmitpriv, 1);
	else
		wl_finish_ack_tx(WL_SCTX_DONE_SUCCESS, &Nic->xmitpriv, 1);
#endif
}

#ifdef CONFIG_FW_wMBOX0_DEBUG
static void Func_Debug_Fwc2H_Process(PNIC Nic, u8 * pdata, u8 len)
{
	int i = 0;
	int cnt = 0, total_length = 0;
	u8 buf[128] = { 0 };
	u8 more_data = FALSE;
	u8 *nextdata = NULL;
	u8 test = 0;

	u8 data_len;
	u8 seq_no;

	nextdata = pdata;
	do {
		data_len = *(nextdata + 1);
		seq_no = *(nextdata + 2);

		for (i = 0; i < data_len - 2; i++) {
			cnt += sprintf((buf + cnt), "%c", nextdata[3 + i]);

			if (nextdata[3 + i] == 0x0a && nextdata[4 + i] == 0xff)
				more_data = TRUE;
			else if (nextdata[3 + i] == 0x0a && nextdata[4 + i] != 0xff)
				more_data = FALSE;
		}
	    MpTrace(COMP_RICHD, DBG_NORMAL,("[RTKFW, SEQ=%d]: %s", seq_no, buf));
		data_len += 3;
		total_length += data_len;

		if (more_data == TRUE) {
			Func_Of_Proc_Pre_Memset(buf, '\0', 128);
			cnt = 0;
			nextdata = (pdata + total_length);
		}
	} while (more_data == TRUE);
}
#endif

static void Func_Process_wMBOX0_Event(PNIC Nic, PwMBOX0_EVT_HDR pC2hEvent,
							  u8 * wmbox0Buf)
{
	u8 index = 0;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	int len = pC2hEvent->CmdLen;
	int w_len = len / 4;
	int b_len = len % 4;
	int i;
	int id = 0;
	int buf_id = 0;
	int buf_len = 0;
	int _integer;
	int _remainder;
	u32 *buf;
	u8 *phead;
	u8 *pwrite;
	u32 offset = 0;
	int send_len = MAILBOX_MAX_TXLEN - 2;

	if (wmbox0Buf == NULL) {
	    MpTrace(COMP_RICHD, DBG_NORMAL,("%s wmbox0buff is NULL\n", __func__));
		return;
	}

	switch (pC2hEvent->CmdID) {
	case wMBOX0_CCX_TX_RPT:
		Func_Ccx_Fwc2Htxrpt_Process(Nic, wmbox0Buf, pC2hEvent->CmdLen);
		break;

#ifdef CONFIG_FW_wMBOX0_DEBUG
	case wMBOX0_EXTEND:
		Func_Debug_Fwc2H_Process(Nic, wmbox0Buf, pC2hEvent->CmdLen);
		break;
#endif

	default:

		return;
		break;

	}
exit:
	HwPlatformIOWrite1Byte(Nic, REG_wMBOX0EVT_CLEAR, wMBOX0_EVT_HOST_CLOSE);

	i = i + 1;
}


void Func_Mcu_Get_Hwreg_Process(PNIC Nic, u8 variable, u8 * val)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	u8 val8;
	u16 val16;
	u32 val32;

	switch (variable) {
	case HW_VAR_TXPAUSE:
		Func_Mcu_Hal_Get_Hwreg(Nic, HW_VAR_TXPAUSE, val, 1);
		break;

	case HW_VAR_BCN_VALID:
#ifdef CONFIG_CONCURRENT_MODE
		if (Nic->iface_type == IFACE_PORT1) {
			Func_Mcu_Hal_Get_Hwreg(Nic, HW_VAR_BCN_VALID1, val, 1);
		} else {
			Func_Mcu_Hal_Get_Hwreg(Nic, HW_VAR_BCN_VALID, val, 1);
		}
#else
		Func_Mcu_Hal_Get_Hwreg(Nic, HW_VAR_BCN_VALID, val, 1);
#endif
		break;

	case HW_VAR_FWLPS_RF_ON:{
			u32 valRCR;

			//if ((wl_is_surprise_removed(Nic)) ||
			//need think about it
			    if(
				(Nic->pwrctl_priv.rf_pwrstate == rf_off)) {
				*val = TRUE;
			} else {
				valRCR = HwPlatformIORead4Byte(Nic, REG_RCR);
				valRCR &= 0x00070000;
				if (valRCR)
					*val = FALSE;
				else
					*val = TRUE;
			}
		}
		break;

	case HW_VAR_EFUSE_USAGE:
		*val = pHalData->EfuseUsedPercentage;
		break;

	case HW_VAR_EFUSE_BYTES:
		*((u16 *) val) = pHalData->EfuseUsedBytes;
		break;
	case HW_VAR_APFM_ON_MAC:
		*val = pHalData->bMacPwrCtrlOn;
		break;
	case HW_VAR_HCI_SUS_STATE:
		*val = pHalData->hci_sus_state;
		break;
	case HW_VAR_CHK_HI_QUEUE_EMPTY:
		Func_Mcu_Hal_Get_Hwreg(Nic, HW_VAR_CHK_HI_QUEUE_EMPTY, val, 1);

		break;
#ifdef CONFIG_WOWLAN
	case HW_VAR_RPWM_TOG:
		break;
	case HW_VAR_WAKEUP_REASON:
		Func_Mcu_Hal_Get_Hwreg(Nic, HW_VAR_WAKEUP_REASON, val, 1);
		break;
	case HW_VAR_SYS_CLKR:
		Func_Mcu_Hal_Get_Hwreg(Nic, HW_VAR_SYS_CLKR, val, 1);
		break;
#endif
	case HW_VAR_DUMP_MAC_QUEUE_INFO:

		break;
	default:
		Func_To_Get_Hwreg(Nic, variable, val);
		break;
	}
}

void Func_Mcu_Set_Hwreg_Process(PNIC Nic, u8 variable, u8 * val)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	u8 val8;
	u16 val16;
	u32 val32;
	u8 psmode;
	_func_enter_;

	switch (variable) {
	case HW_VAR_MEDIA_STATUS:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_MEDIA_STATUS, val, 1);
		break;

	case HW_VAR_MEDIA_STATUS1:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_MEDIA_STATUS1, val, 1);
		break;

	case HW_VAR_SET_OPMODE:
		Func_Mcu_Hw_Var_Set_Opmode(Nic, variable, val);
		break;

	case HW_VAR_MAC_ADDR:
		Func_Mcu_Hw_Var_Set_Macaddr(Nic, variable, val);
		break;

	case HW_VAR_BSSID:
		Func_Mcu_Hw_Var_Set_Bssid(Nic, variable, val);
		break;

	case HW_VAR_BASIC_RATE:{
			struct mlme_ext_info *mlmext_info =
				&Nic->mlmeextpriv.mlmext_info;
			u16 input_b = 0, masked = 0, ioted = 0, BrateCfg = 0;
			u32 temp = 0;
			u16 rrsr_2g_force_mask = RRSR_CCK_RATES;
			u16 rrsr_2g_allow_mask =
				(RRSR_24M | RRSR_12M | RRSR_6M | RRSR_CCK_RATES);
			int ret;

			Func_Chip_Hw_Setbratecfg(Nic, val, &BrateCfg);
			input_b = BrateCfg;

			BrateCfg |= rrsr_2g_force_mask;
			BrateCfg &= rrsr_2g_allow_mask;
			masked = BrateCfg;

#ifdef CONFIG_CMCC_TEST
			BrateCfg |= (RRSR_11M | RRSR_5_5M | RRSR_1M);
			BrateCfg |= (RRSR_24M | RRSR_18M | RRSR_12M);
#endif

			if (mlmext_info->assoc_AP_vendor == HT_IOT_PEER_CISCO) {
				if ((BrateCfg & (RRSR_24M | RRSR_12M | RRSR_6M)) == 0)
					BrateCfg |= RRSR_6M;
			}
			ioted = BrateCfg;

			pHalData->BasicRateSet = BrateCfg;

			MpTrace(COMP_RICHD, DBG_NORMAL,
				("HW_VAR_BASIC_RATE: %#x -> %#x -> %#x\n", input_b, masked,
					ioted));


			temp = BrateCfg;

			ret =
				Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HW_SET_BASIC_RATE,
											&temp, NULL, 1, 0);
			if (!ret) {
				MpTrace(COMP_RICHD, DBG_NORMAL,
					("===>%s,fail,code :UMSG_OPS_HW_SET_BASIC_RATE!\n",
						__func__));
			}

		}
		break;

	case HW_VAR_TXPAUSE:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_TXPAUSE, val, 1);
		break;

	case HW_VAR_BCN_FUNC:
		Func_Mcu_Hw_Var_Set_Bcn_Func(Nic, variable, val);
		break;

	case HW_VAR_CORRECT_TSF:
		Func_Mcu_Hw_Var_Set_Correct_Tsf(Nic, variable, val);
		break;

	case HW_VAR_CHECK_BSSID:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_CHECK_BSSID, val, 1);
		break;

	case HW_VAR_MLME_DISCONNECT:
		Func_Mcu_Hw_Var_Set_Mlme_Disconnect(Nic, variable, val);
		break;

	case HW_VAR_MLME_SITESURVEY:
		Func_Mcu_Hw_Var_Set_Mlme_Sitesurvey(Nic, variable, val);
		break;

	case HW_VAR_MLME_JOIN:
		Func_Mcu_Hw_Var_Set_Mlme_Join(Nic, variable, val);
		break;

	case HW_VAR_ON_RCR_AM:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_ON_RCR_AM, val, 1);
		break;

	case HW_VAR_OFF_RCR_AM:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_OFF_RCR_AM, val, 1);
		break;

	case HW_VAR_BEACON_INTERVAL:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_BEACON_INTERVAL, val, 2);
		break;

	case HW_VAR_SLOT_TIME:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_SLOT_TIME, val, 1);
		break;

	case HW_VAR_RESP_SIFS:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_RESP_SIFS, val, 4);
		break;

	case HW_VAR_ACK_PREAMBLE:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_ACK_PREAMBLE, val, 1);
		break;

	case HW_VAR_CAM_EMPTY_ENTRY:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_CAM_EMPTY_ENTRY, val, 1);
		break;

	case HW_VAR_CAM_INVALID_ALL:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_CAM_INVALID_ALL, val, 1);
		break;

	case HW_VAR_AC_PARAM_VO:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_AC_PARAM_VO, val, 4);
		break;

	case HW_VAR_AC_PARAM_VI:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_AC_PARAM_VI, val, 4);
		break;

	case HW_VAR_AC_PARAM_BE:
		pHalData->AcParam_BE = ((u32 *) (val))[0];
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_AC_PARAM_BE, val, 4);
		break;

	case HW_VAR_AC_PARAM_BK:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_AC_PARAM_BK, val, 4);
		break;

	case HW_VAR_ACM_CTRL:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_ACM_CTRL, val, 1);
		break;

	case HW_VAR_AMPDU_FACTOR:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_AMPDU_FACTOR, val, 1);
		break;

	case HW_VAR_wMBOX1_FW_PWRMODE:
		psmode = *val;

		Func_Of_Set_Fwpwrmode_Cmd(Nic, psmode);

		break;

	case HW_VAR_wMBOX1_PS_TUNE_PARAM:
		Func_Of_Set_Fwpstuneparam_Cmd(Nic);
		break;

	case HW_VAR_wMBOX1_FW_JOINBSSRPT:
		Func_Of_Set_Fwjoinbssrpt_Cmd(Nic, *val);

#ifdef CONFIG_P2P
	case HW_VAR_wMBOX1_FW_P2P_PS_OFFLOAD:
		Func_Of_Set_P2P_Ps_Offload_Cmd(Nic, *val);
		break;
#endif

	case HW_VAR_EFUSE_USAGE:
		pHalData->EfuseUsedPercentage = *val;
		break;

	case HW_VAR_EFUSE_BYTES:
		pHalData->EfuseUsedBytes = *((u16 *) val);
		break;
	case HW_VAR_FIFO_CLEARN_UP:
		{
			int ret;
			u32 res;
			struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;
			val32 = pwrpriv->bkeepfwalive;

			ret =
				Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HW_FIFO_CLEARN_UP,
											&val32, &res, 1, 1);
			if (!ret) {
				MpTrace(COMP_RICHD, DBG_NORMAL,("===>%s,fail\n", __func__));
				return;
			}
			Nic->xmitpriv.nqos_ssn = (u16) res;
		}
		break;

	case HW_VAR_RESTORE_HW_SEQ:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_RESTORE_HW_SEQ, NULL, 0);
		break;

#ifdef CONFIG_CONCURRENT_MODE
	case HW_VAR_CHECK_TXBUF:{
			Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_CHECK_TXBUF, NULL, 0);
		}
		break;
#endif

	case HW_VAR_APFM_ON_MAC:
		pHalData->bMacPwrCtrlOn = *val;
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("%s: bMacPwrCtrlOn=%d\n", __func__, pHalData->bMacPwrCtrlOn));
		break;

	case HW_VAR_HCI_SUS_STATE:
		pHalData->hci_sus_state = *val;
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("%s: hci_sus_state=%u\n", __func__, pHalData->hci_sus_state));
		break;

	case HW_VAR_NAV_UPPER:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_NAV_UPPER, val, 4);
		break;

	case HW_VAR_BCN_VALID:
#ifdef CONFIG_CONCURRENT_MODE
		val8 = Nic->iface_type;
#else
		val8 = 0;
#endif
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_BCN_VALID, &val8, 1);
		break;

	case HW_VAR_DL_BCN_SEL:
#ifdef CONFIG_CONCURRENT_MODE
		val8 = Nic->iface_type;
#else
		val8 = 0;
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_DL_BCN_SEL, &val8, 1);
#endif
		break;

	case HW_VAR_DO_IQK:
		if (*val)
			pHalData->bNeedIQK = TRUE;
		else
			pHalData->bNeedIQK = FALSE;
		break;

	case HW_VAR_DL_RSVD_PAGE:
		{
			Func_Of_Download_Rsvd_Page(Nic, WP_MEDIA_CONNECT);
		}
		break;

	case HW_VAR_MACID_SLEEP:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_MACID_SLEEP, val, 1);
		break;

	case HW_VAR_MACID_WAKEUP:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_MACID_WAKEUP, val, 1);
		break;

	case HW_VAR_EN_HW_UPDATE_TSF:
		Func_Mcu_Hw_Var_Set_Hw_Update_Tsf(Nic);
		break;
	default:
		Func_Mcu_Sethwreg(Nic, variable, val);
		break;
	}

	_func_exit_;
}

static void Func_Chip_Hw_Ra_Info_Dump(PNIC * Nic, void *sel)
{
}

u8 Func_To_Get_Hal_Def_Var_Process(PNIC Nic, HAL_DEF_VARIABLE variable, void *pval)
{
	PHAL_DATA_TYPE pHalData;
	u8 bResult;

	pHalData = GET_HAL_DATA(Nic);
	bResult = TRUE;

	switch (variable) {
	case HAL_DEF_MAX_RECVBUF_SZ:
		*((u32 *) pval) = MAX_RECVBUF_SZ;
		break;

	case HAL_DEF_RX_PACKET_OFFSET:
		*((u32 *) pval) = RXDESC_SIZE + DRVINFO_SZ * 8;
		break;

	case HW_VAR_MAX_RX_AMPDU_FACTOR:
		*(HT_CAP_AMPDU_FACTOR *) pval = MAX_AMPDU_FACTOR_16K;
		break;
	case HW_VAR_BEST_AMPDU_DENSITY:
		*((u32 *) pval) = AMPDU_DENSITY_VALUE_7;
		break;
	case HAL_DEF_TX_LDPC:
	case HAL_DEF_RX_LDPC:
		*((u8 *) pval) = FALSE;
		break;
	case HAL_DEF_TX_STBC:
		*((u8 *) pval) = 0;
		break;
	case HAL_DEF_RX_STBC:
		*((u8 *) pval) = 1;
		break;
	case HW_DEF_RA_INFO_DUMP:
		Func_Chip_Hw_Ra_Info_Dump(Nic, pval);
		break;

	case HAL_DEF_TX_PAGE_BOUNDARY:
		*(u8 *) pval = TX_PAGE_BOUNDARY_9086X;
		break;

	case HAL_DEF_MACID_SLEEP:
		*(u8 *) pval = TRUE;
		break;
	case HAL_DEF_TX_PAGE_SIZE:
		*((u32 *) pval) = PAGE_SIZE_128;
		break;
	case HAL_DEF_RX_DMA_SZ_WOW:
		*(u32 *) pval = RX_DMA_SIZE_9086X - RESV_FMWF;
		break;
	case HAL_DEF_RX_DMA_SZ:
		*(u32 *) pval = RX_DMA_BOUNDARY_9086X + 1;
		break;
	case HAL_DEF_RX_PAGE_SIZE:
		*((u32 *) pval) = 8;
		break;
	default:
		bResult = Func_To_Get_Hal_Def_Var(Nic, variable, pval);
		break;
	}

	return bResult;
}

