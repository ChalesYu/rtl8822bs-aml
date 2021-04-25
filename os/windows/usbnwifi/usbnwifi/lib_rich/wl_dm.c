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

//#include <drv_types.h>
//#include "ssvc.h"
#include "globals.h"
#include "ndis.h"
#include "hw_pcomp.h"
#include "hw_main.h"
#include "hw_Nic.h"
#include "hw_func.h"
#include "usb_main.h"
#include "bus_intf.h"
#include "8187_gen.h"

#include "comm_wmbox1.h"
#include "pdata.h"

msg_rw_val_t msg_rw_val, r_msg_rw_val;
static NDIS_SPIN_LOCK  sta_bh_lock;
static NDIS_SPIN_LOCK  var_bh_lock;
static int blastPackBcnCnt;
static int bPacketBeacon_cnt;


extern void rx_process_phy_info(PNIC Nic, u32 * rx_buf);

static void Func_Hw_Op_Sta_Hdl(PNIC Nic);

void Func_Hw_Op_Sync_Var(PNIC Nic, const char *f, int l)
{
	int ret = FALSE;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_WRITEVAR_MSG,
									(u32 *) & msg_rw_val, NULL,
									sizeof(msg_rw_val) / 4, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_HAL_WRITEVAR_MSG!!!\n",
				__func__));
		return;
	}
}

void Func_Hw_Op_Get_Var(PNIC Nic, const char *f, int l)
{
	int ret = FALSE;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_READVAR_MSG, NULL,
									(u32 *) & msg_rw_val, 0,
									sizeof(msg_rw_val) / 4);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_HAL_READVAR_MSG!!!\n",
				__func__));
		return;
	}

}

static void Func_Hw_Op_Sync_Config(PNIC Nic)
{
	int ret = FALSE;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct pwrctrl_priv *pwrctl = &Nic->pwrctl_priv;
	msg_config_t msg_config;

	get_fwstate(pmlmepriv);

	msg_config.customer_id = (u32) pHalData->CustomerID;
	msg_config.cut_version = 0;
	msg_config.Regulation2_4G = (u32) pHalData->Regulation2_4G;
	msg_config.TypeGPA = (u32) pHalData->TypeGPA;
	msg_config.TypeAPA = (u32) pHalData->TypeAPA;
	msg_config.TypeGLNA = (u32) pHalData->TypeGLNA;

	msg_config.TypeALNA = (u32) pHalData->TypeALNA;
	msg_config.RFEType = (u32) pHalData->RFEType;
	msg_config.PackageType = (u32) pHalData->PackageType;
	msg_config.boardConfig =
		(u32) (pHalData->ExternalLNA_2G + pHalData->ExternalPA_2G * 256 +
			   pHalData->EEPROMBluetoothCoexist * 65536);

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_CONFIG_MSG,
									(u32 *) & msg_config, NULL,
									sizeof(msg_config) / 4, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_HAL_CONFIG_MSG!!!\n",
				__func__));
	}

}

static void Func_Hw_Op_Init_Var(PNIC Nic)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct pwrctrl_priv *pwrctl = &Nic->pwrctl_priv;

	int ret = FALSE;
	
	NdisAcquireSpinLock(&var_bh_lock);

	memset(&msg_rw_val, 0, sizeof(msg_rw_val));
	msg_rw_val.tx_bytes = Nic->dvobjpriv.traffic_stat.tx_bytes;
	msg_rw_val.rx_bytes = Nic->dvobjpriv.traffic_stat.rx_bytes;
	msg_rw_val.cur_wireless_mode = (u32) pmlmeext->cur_wireless_mode;
	msg_rw_val.CurrentBandType = (u32) pHalData->CurrentBandType;
	msg_rw_val.ForcedDataRate = (u32) pHalData->ForcedDataRate;
	msg_rw_val.nCur40MhzPrimeSC = (u32) pHalData->nCur40MhzPrimeSC;
	msg_rw_val.dot11PrivacyAlgrthm = Nic->securitypriv.dot11PrivacyAlgrthm;
	msg_rw_val.CurrentChannelBW = (u32) pHalData->CurrentChannelBW;

	msg_rw_val.CurrentChannel = (u32) pHalData->CurrentChannel;
//	msg_rw_val.net_closed = (u32) Nic->net_closed;
	msg_rw_val.u1ForcedIgiLb = (u32) pHalData->u1ForcedIgiLb;
	msg_rw_val.bScanInProcess = (u32) pmlmepriv->bScanInProcess;
	msg_rw_val.bpower_saving = (u32) pwrctl->bpower_saving;
	msg_rw_val.traffic_stat_cur_tx_tp = Nic->dvobjpriv.traffic_stat.cur_tx_tp;
	msg_rw_val.traffic_stat_cur_rx_tp = Nic->dvobjpriv.traffic_stat.cur_rx_tp;

	msg_rw_val.wifi_display = FALSE;
	msg_rw_val.wifi_direct = FALSE;
	msg_rw_val.dbg_cmp = 0;
	msg_rw_val.dbg_level = MSG_DBG_TRACE;

	NdisReleaseSpinLock(&var_bh_lock);

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_INIT_MSG_VAR,
									(u32 *) & msg_rw_val, NULL,
									sizeof(msg_rw_val) / 4, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_HAL_INIT_MSG_VAR!!!\n",
				__func__));
		return;
	}
    ret =
        Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_WRITEVAR_MSG,
                                        (u32 *) & msg_rw_val, NULL,
                                        sizeof(msg_rw_val) / 4, 0);
    if (!ret) {
        MpTrace(COMP_RICHD, DBG_NORMAL, ("Error :===>%s,fail,code :UMSG_OPS_HAL_WRITEVAR_MSG!!!\n",
                __func__));
        return;
    }

}

VOID Chip_Bb_Setrfpath_Switch_Process(IN PNIC Nic,
							   IN BOOLEAN bMain, IN BOOLEAN is2T)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	if (bMain)
		PHY_SetBBReg(Nic, 0x92C, bMaskDWord, 0x1);
	else
		PHY_SetBBReg(Nic, 0x92C, bMaskDWord, 0x2);
}

VOID Func_Chip_Bb_Setrfpath_Switch_Process(IN PNIC Nic, IN BOOLEAN bMain)
{
	Chip_Bb_Setrfpath_Switch_Process(Nic, bMain, TRUE);
}

s32 Func_Chip_Hw_Trx_Test_Set_Powertracking(PNIC Nic, u8 enable)
{

	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	if (check_fwstate(&Nic->mlmepriv, WIFI_MP_STATE) == FALSE) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("SetPowerTracking! Fail: not in MP mode!\n"));
		return FALSE;
	}
	msg_get_var(Nic);

	NdisAcquireSpinLock(&var_bh_lock);
	
	if (enable)
		msg_rw_val.phy_cali.TxPowerTrackControl = _TRUE;
	else
		msg_rw_val.phy_cali.TxPowerTrackControl = FALSE;


	NdisReleaseSpinLock(&var_bh_lock);
	msg_sync_var(Nic);

	return TRUE;

}

void Func_Chip_Hw_Trx_Test_Get_Powertracking(PNIC Nic, u8 * enable)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	msg_get_var(Nic);

	*enable = msg_rw_val.phy_cali.TxPowerTrackControl;
}

s8 Func_Chip_Bb_Gettxpowertrackingoffset(PNIC Nic, u8 RFPath, u8 Rate)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

	s8 offset = 0;

	if (RFPath != 0) {
		return offset;
	}

	msg_get_var(Nic);

	if (msg_rw_val.phy_cali.TxPowerTrackControl == FALSE)
		return offset;

	if ((Rate == MGN_1M) || (Rate == MGN_2M) || (Rate == MGN_5_5M)
		|| (Rate == MGN_11M)) {
		offset = (s8) msg_rw_val.phy_cali.Remnant_CCKSwingIdx;

	} else {
		offset = msg_rw_val.phy_cali.Remnant_OFDMSwingIdx;

	}

	return offset;
}

u32 Func_Of_Proc_Phydm_Ability_Ops(PNIC Nic, HAL_PHYDM_OPS ops, u32 ability)
{

	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u32 result = 0;

	static u32 bk_ability = 0;

	msg_get_var(Nic);

	MpTrace(COMP_RICHD, DBG_NORMAL,
		("%s %X \r\n", __func__, msg_rw_val.ability));

	NdisAcquireSpinLock(&var_bh_lock);

	switch (ops) {
	case HAL_PHYDM_DIS_ALL_FUNC:
		msg_rw_val.ability = DYNAMIC_FUNC_DISABLE;
		break;
	case HAL_PHYDM_FUNC_SET:
		msg_rw_val.ability |= ability;
		break;
	case HAL_PHYDM_FUNC_CLR:
		msg_rw_val.ability &= ~(ability);
		break;
	case HAL_PHYDM_ABILITY_BK:
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("ability bk %x\r\n", bk_ability));
		bk_ability = msg_rw_val.ability;
		break;
	case HAL_PHYDM_ABILITY_RESTORE:
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("ability restory %X\r\n", bk_ability));

		msg_rw_val.ability = bk_ability;
		break;
	case HAL_PHYDM_ABILITY_SET:
		msg_rw_val.ability = ability;
		break;
	case HAL_PHYDM_ABILITY_GET:
		result = msg_rw_val.ability;
		break;
	}

	NdisReleaseSpinLock(&var_bh_lock);
	msg_sync_var(Nic);

	return result;

}

static void Func__Init_Dm_Priv(PNIC Nic)
{

	NdisAllocateSpinLock(&sta_bh_lock);
	NdisAllocateSpinLock(&var_bh_lock);

	Func_Hw_Op_Sync_Config(Nic);
	Func_Hw_Op_Init_Var(Nic);
}



void Func_Of_Init_Dm_Priv(IN PNIC Nic)
{
	Func__Init_Dm_Priv(Nic);
}

void Func_Of_Deinit_Dm_Priv(IN PNIC Nic)
{
}

void Func_Init_Hal_Dm(IN PNIC Nic)
{
	int ret = FALSE;

	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

	pHalData->DM_Type = 1;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_INIT_MSG, NULL, NULL, 0,
									0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_HAL_INIT_MSG!!!\n", __func__));

		return;
	}
	MpTrace(COMP_RICHD, DBG_NORMAL,
		("===>%s,success,code :UMSG_OPS_HAL_INIT_MSG!!!\n", __func__));

}



VOID Func_Of_Haldmwatchdog(IN PNIC Nic)
{

	BOOLEAN bFwCurrentInPSMode = FALSE;
	BOOLEAN bFwPSAwake = _TRUE;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	u8 bLinked = FALSE;
	u8 bsta_state = FALSE;
	u8 bBtDisabled = _TRUE;
	int ret = FALSE;


#ifdef CONFIG_CONCURRENT_MODE
	PwADPTDATA pbuddy_wadptdata = Nic->pbuddy_wadptdata;
#endif

//	if (Nic->registrypriv.mp_mode == 1 && Nic->mppriv.mp_dm == 0)
//		return;

	if (!wl_is_hw_init_completed(Nic))
		goto skip_dm;

#ifdef CONFIG_LPS
	bFwCurrentInPSMode = Nic->pwrctl_priv.bFwCurrentInPSMode;
	Func_Hw_Var_Set_Fwlps_Rfon(Nic, HW_VAR_FWLPS_RF_ON, (u8 *) (&bFwPSAwake));
#endif

	if ((wl_is_hw_init_completed(Nic))
		&& ((!bFwCurrentInPSMode) && bFwPSAwake)) {
		Func_Of_Proc_Chip_Hw_Check_Rxfifo_Full(Nic);
	}

	if (bFwCurrentInPSMode) {
		return;
	}

	if (wl_is_hw_init_completed(Nic)) {
		if (do_chk_linked(Nic, 1)) {
			bLinked = _TRUE;
			if (check_fwstate(&Nic->mlmepriv, WIFI_STATION_STATE))
				bsta_state = _TRUE;
		}
#ifdef CONFIG_CONCURRENT_MODE
		if (pbuddy_wadptdata && do_chk_linked(pbuddy_wadptdata, 1)) {
			bLinked = _TRUE;
			if (pbuddy_wadptdata
				&& check_fwstate(&pbuddy_wadptdata->mlmepriv, WIFI_STATION_STATE))
				bsta_state = _TRUE;
		}
#endif

		msg_get_var(Nic);
		msg_rw_val.msgWdgStateVal = U32_MOVE_BITS(bLinked, 0) +
			U32_MOVE_BITS(bsta_state, 1) + U32_MOVE_BITS(bBtDisabled, 2);
		msg_sync_var(Nic);

		ret = Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_MSG_WDG, NULL, NULL,
										0, 0);
		if (!ret) {
			MpTrace(COMP_RICHD, DBG_NORMAL,
				("Error :===>%s,fail,code :UMSG_OPS_HAL_MSG_WDG!!!\n",
					__func__));
			return;
		}

		Func_Hw_Op_Sta_Hdl(Nic);

		blastPackBcnCnt = bPacketBeacon_cnt;

		bPacketBeacon_cnt = 0;
	}

skip_dm:

	return;
}

#if 0
void Func_Of_Query_Rx_Desc_Status(union recv_frame *precvframe, u8 * pdesc)
{
	struct rx_pkt_attrib *pattrib;
	u8 rate;

	pattrib = &precvframe->u.hdr.attrib;
	memset(pattrib, 0, sizeof(struct rx_pkt_attrib));

	pattrib->pkt_len = (u16) GET_RX_STATUS_DESC_PKT_LEN_9086X(pdesc);
	pattrib->pkt_rpt_type =
		GET_RX_STATUS_DESC_RPT_SEL_9086X(pdesc) ? wMBOX0_PACKET : NORMAL_RX;

	if (pattrib->pkt_rpt_type == NORMAL_RX) {

		pattrib->crc_err = (u8) GET_RX_STATUS_DESC_CRC32_9086X(pdesc);
		pattrib->icv_err = 0;
		pattrib->drvinfo_sz = 32;
		pattrib->encrypt = (u8) GET_RX_STATUS_DESC_SECURITY_9086X(pdesc);
		pattrib->qos = (u8) GET_RX_STATUS_DESC_QOS_9086X(pdesc);
		pattrib->shift_sz = 0;
		pattrib->physt = 0;
		pattrib->bdecrypted = 1;
		pattrib->physt = (u8) GET_RX_STATUS_DESC_PHY_STATUS_9086X(pdesc);

		pattrib->priority = (u8) GET_RX_STATUS_DESC_TID_9086X(pdesc);
		pattrib->amsdu = (u8) GET_RX_STATUS_DESC_AMSDU_9086X(pdesc);
		pattrib->mdata = (u8) GET_RX_STATUS_DESC_MORE_DATA_9086X(pdesc);
		pattrib->mfrag = (u8) GET_RX_STATUS_DESC_MORE_FRAG_9086X(pdesc);

		pattrib->seq_num = (u16) GET_RX_STATUS_DESC_SEQ_9086X(pdesc);
		pattrib->frag_num = (u8) GET_RX_STATUS_DESC_FRAG_9086X(pdesc);

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

	}

}
#endif

VOID Func_Chip_Bb_Lccalibrate_Process(PNIC Nic)
{
	int ret = FALSE;
	BOOLEAN bSingleTone = FALSE, bCarrierSuppression = FALSE;
	u4Byte timeout = 2000, timecount = 0;

	MpTrace(COMP_RICHD, DBG_NORMAL,	("%s==>start!!!\n", __func__));

	msg_get_var(Nic);

	if (!(msg_rw_val.ability & BIT26))
		return;
	MpTrace(COMP_RICHD, DBG_NORMAL,	("1"));

	if (bSingleTone || bCarrierSuppression)
		return;
	MpTrace(COMP_RICHD, DBG_NORMAL,	("2"));

	while (msg_rw_val.bScanInProcess && timecount < timeout) {
		Func_Of_Proc_Mdelay_Os(50);
		timecount += 50;
		msg_get_var(Nic);
	}
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_CALI_LLC, NULL, NULL,
									0, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_HAL_CALI_LLC!!!\n", __func__));
		return;
	}
	MpTrace(COMP_RICHD, DBG_NORMAL,	("%s==>OK!!!\n", __func__));
	
}

VOID Func_Chip_Bb_Iqcalibrate_Process(IN PNIC Nic,
						   IN BOOLEAN bReCovery, IN BOOLEAN bRestore)
{
	u4Byte buff[2] = { 0 };
	int len = 2;
	int ret = FALSE;

	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	BOOLEAN bSingleTone = FALSE, bCarrierSuppression = FALSE;

	msg_get_var(Nic);

	if (!(msg_rw_val.ability & BIT26))
		return;

	if (bSingleTone || bCarrierSuppression)
		return;

	buff[0] = MSG_RF_PATH_A;
	buff[1] = (u32) pHalData->CurrentChannel;
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_PHY_IQ_CALIBRATE, buff,
									NULL, len, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_HAL_PHY_IQ_CALIBRATE!!!\n",
				__func__));
		return;
	}
}

////////////////////Func_Of_Proc_Chip_Hw_Set_Odm_Var
u8 Func_Set_Odm_Var_Hdl(PNIC Nic, unsigned char *pbuf)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	int ret = FALSE;
	msg_rw_val_t old;

	HAL_MSG_VARIABLE eVariable;
	PVOID pValue1;
	BOOLEAN bSet;
	u2Byte tempindex;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)pbuf;

	msg_var_req_t *p_msg_var_req = (msg_var_req_t *) pdrvextra_cmd_parm->pbuf;

	if (!p_msg_var_req)
		return -1;

	eVariable = p_msg_var_req->var;

	pValue1 = (PVOID) p_msg_var_req->msg;

	bSet = p_msg_var_req->set;

	msg_get_var(Nic);

	memcpy(&old, &msg_rw_val, sizeof(msg_rw_val_t));

	switch (eVariable) {
	case HAL_MSG_STA_INFO:
		{
			struct sta_info *psta = (struct sta_info *)pValue1;
			if (bSet) {
				MpTrace(COMP_RICHD, DBG_NORMAL,
					("### Set STA_(%d) info ###\n", psta->mac_id));
				
				Func_Hw_Op_Cmninfoptrarrayhook(Nic, psta->mac_id, psta);
			} else {
				MpTrace(COMP_RICHD, DBG_NORMAL,
					("### Clean STA_(%d) info ###\n", psta->mac_id));

				Func_Hw_Op_Cmninfoptrarrayhook(Nic, psta->mac_id, NULL);
			}
		}
		break;
	case HAL_MSG_P2P_STATE:
		old.wifi_direct = bSet;
		break;
	case HAL_MSG_WIFI_DISPLAY_STATE:
		old.wifi_display = bSet;
		break;
	case HAL_MSG_REGULATION:
		Func_Hw_Op_Sync_Config(Nic);
		break;
	case HAL_MSG_INITIAL_GAIN:
		{
			u8 rx_gain = *((u8 *) (pValue1));
			u32 buf[3];

			buf[1] = 0;
			buf[2] = rx_gain;
			if (rx_gain == 0xff) {
				buf[0] = BIT1;
				ret =
					Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_MSG_PAUSEIG,
												buf, NULL, 3, 0);
				if (!ret) {
					MpTrace(COMP_RICHD, DBG_NORMAL,
						("Error :===>%s,fail,code :UMSG_OPS_MSG_PAUSEIG!!!\n",
							__func__));

					return FALSE;
				}
			} else {
				buf[0] = BIT0;
				ret =
					Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_MSG_PAUSEIG,
												buf, NULL, 3, 0);
				if (!ret) {
					MpTrace(COMP_RICHD, DBG_NORMAL,
						("Error :===>%s,fail,code :UMSG_OPS_MSG_PAUSEIG!!!\n",
							__func__));
					return FALSE;
				}
			}
		}
		break;
	case HAL_MSG_FA_CNT_DUMP:

		if (*((u8 *) pValue1)) {
			old.dbg_cmp |= (BIT0 | BIT3);
		} else {
			old.dbg_cmp &= ~(BIT0 | BIT3);
		}
		break;
	case HAL_MSG_DBG_FLAG:
		old.dbg_cmp = *((u8Byte *) pValue1);

		break;
	case HAL_MSG_DBG_LEVEL:
		old.dbg_level = *((u8Byte *) pValue1);

		break;
	case HAL_MSG_RX_INFO_DUMP:
		{
#ifdef NOT_FIXED
			msg_get_var(Nic);

			if (msg_rw_val.FalseAlmCnt)
				WL_INFO
					("Cnt_Cck_fail = %d, Cnt_Ofdm_fail = %d, Total False Alarm = %d\n",
					 FalseAlmCnt->Cnt_Cck_fail, FalseAlmCnt->Cnt_Ofdm_fail,
					 FalseAlmCnt->Cnt_all);

			WL_INFO("============ Rx Info dump ===================\n");

			WL_INFO("bLinked = %d, RSSI_Min = %d(%%), CurrentIGI = 0x%x\n",
					U32_GET_BITS(msg_rw_val.msgWdgStateVal, 0),
					msg_rw_val.Rssi_Min, msg_rw_val.dig_CurIGValue);

			WL_INFO("RxRate not fixed !\n");

			if (U32_GET_BITS(msg_rw_val.msgWdgStateVal, 0)) {
				WL_INFO("RxRate = %s, RSSI_A = %d(%%), RSSI_B = %d(%%)\n",
						HDATA_RATE(pmsgpriv->RxRate), pmsgpriv->RSSI_A,
						pmsgpriv->RSSI_B);

#ifdef DBG_RX_SIGNAL_DISPLAY_RAW_DATA
				Func_Of_Proc_Dump_Raw_Rssi_Info(Nic);
#endif
			}
#endif
		}
		break;

#ifdef CONFIG_AUTO_CHNL_SEL_NHM
	case HAL_MSG_AUTO_CHNL_SEL:
		{
			ACS_OP acs_op = *(ACS_OP *) pValue1;

			wl_phydm_func_set(Nic, MSG_BB_NHM_CNT);

			if (ACS_INIT == acs_op) {
#ifdef DBG_AUTO_CHNL_SEL_NHM
				WL_INFO("[ACS-" ADPT_FMT "] HAL_MSG_AUTO_CHNL_SEL: ACS_INIT\n",
						ADPT_ARG(Nic));
#endif
				msg_AutoChannelSelectInit(pmsgpriv);
			} else if (ACS_RESET == acs_op) {

#ifdef DBG_AUTO_CHNL_SEL_NHM
				WL_INFO("[ACS-" ADPT_FMT "] HAL_MSG_AUTO_CHNL_SEL: ACS_RESET\n",
						ADPT_ARG(Nic));
#endif
				msg_AutoChannelSelectReset(pmsgpriv);

			} else if (ACS_SELECT == acs_op) {

#ifdef DBG_AUTO_CHNL_SEL_NHM
				WL_INFO("[ACS-" ADPT_FMT
						"] HAL_MSG_AUTO_CHNL_SEL: ACS_SELECT, CH(%d)\n",
						ADPT_ARG(Nic), wl_get_acs_channel(Nic));
#endif
				msg_AutoChannelSelect(pmsgpriv, wl_get_acs_channel(Nic));
			} else
				WL_INFO("[ACS-" ADPT_FMT
						"] HAL_MSG_AUTO_CHNL_SEL: Unexpected OP\n",
						ADPT_ARG(Nic));

		}
		break;
#endif

	default:
		break;
	}

	if (memcmp(&old, &msg_rw_val, sizeof(msg_rw_val_t))) {

		msg_get_var(Nic);

		NdisAcquireSpinLock(&var_bh_lock);
		memcpy(&msg_rw_val, &old, sizeof(msg_rw_val_t));
		NdisReleaseSpinLock(&var_bh_lock);

		msg_sync_var(Nic);
	}
	wl_mfree((u8 *) p_msg_var_req, sizeof(msg_var_req_t));

	return _TRUE;
}

void wl_bssid_rssi_init(PNIC Nic)
{
	int i;
	MpTrace(COMP_RICHD, DBG_NORMAL,("rssi clear!!!\r\n"));

	for (i = 0; i < MAX_BSSID_RSSI_TBL; i++) {
		memset(&Nic->mlmeextpriv.bssid_rssi[i], 0,
			   sizeof(wlan_bssid_rssi_t));
	}
}

void wlan_bssid_rssi_insert(PNIC Nic, u8 * target, u32 * rx_buf)
{
	int i;
	wlan_bssid_rssi_t *pBssidTbl = Nic->mlmeextpriv.bssid_rssi;
	wlan_bssid_rssi_t *ptarget;
	u8 zero_mac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	int find = 0;
	if (!pBssidTbl)
		return;

	for (i = 0; i < MAX_BSSID_RSSI_TBL; i++) {
		if (memcmp(pBssidTbl->bssid, target, 6)) {
			find = _TRUE;
			break;
		}
		pBssidTbl++;
	}

	if (find == FALSE) {
		pBssidTbl = Nic->mlmeextpriv.bssid_rssi;
		for (i = 0; i < MAX_BSSID_RSSI_TBL; i++) {

			if (Func_Of_Proc_Pre_Memcmp(pBssidTbl->bssid, zero_mac, 6)) {
				find = _TRUE;
				break;
			}
			pBssidTbl++;

		}

	}
	if (find) {
		ptarget = (pBssidTbl);
		memcpy(ptarget->bssid, target, 6);
		ptarget->rssi = rx_buf[0];
		ptarget->linkQ = rx_buf[1];
	}

}

void search_bssid_rssi_tbl(PNIC Nic, u8 * target,
						   WLAN_PHY_INFO * phy)
{
	int i;
	int find = FALSE;
	wlan_bssid_rssi_t *pBssidTbl = Nic->mlmeextpriv.bssid_rssi;

	for (i = 0; i < MAX_BSSID_RSSI_TBL; i++) {
		if (!memcmp(target, pBssidTbl->bssid, 6)) {
			find = _TRUE;
			break;
		}
		pBssidTbl++;
	}
	if (find == _TRUE) {
		phy->SignalStrength = pBssidTbl->rssi;
		phy->SignalQuality = pBssidTbl->linkQ;
	}
}


u8 Func_Chip_Bb_Status_Query_Hdl(PNIC Nic, unsigned char *pbuf)
{

	int ret;
	int len;
	u8 *ptr;
	u8 mac_id;
	POPS_DM_PACKET_INFO_T ppkt_info;
	OPS_DM_PACKET_INFO_T *pkt;
	u32 rx_buf[2];

	if (!pbuf) {
		return FALSE;
	}

	ptr = (u8 *) pbuf;
	ppkt_info = (POPS_DM_PACKET_INFO_T) ptr;
	len = RND4(sizeof(OPS_DM_PACKET_INFO_T));
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_MSG_RHY_STATUS,
									(u32 *) ptr, rx_buf, (len >> 2), 2);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :rx_query_phy_status!!!\n", __func__));

	}
	pkt = (POPS_DM_PACKET_INFO_T) ptr;
	mac_id = pkt->StationID;

	return ret;
}

void msg_RxPhyStatus_work(OUT struct phy_info * pPhyInfo,
				 IN u8 *pPhyStatus,
	 			 IN OPS_DM_PACKET_INFO_T pPktinfo);

int Func_Chip_Bb_Status_Query_Send(PNIC pNic, PNIC_RX_FRAGMENT pNicFragment, u8 * pPhyStatus,
						  int pkt_len)
{

	u32 rx_buf[2];
	int res = _TRUE;
	static int phystatus2 = 0;
	struct cmd_obj *phy_status_cmd;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	//PwADPTDATA pwadptdata = precvframe->u.hdr.wadptdata;	
	//struct cmd_priv *pcmdpriv = &(pwadptdata->cmdpriv);
	struct cmd_priv *pcmdpriv = &(pNic->cmdpriv);
	
	//struct rx_pkt_attrib *pattrib = &precvframe->u.hdr.attrib;
	struct rx_pkt_attrib *pattrib = &pNicFragment->attrib;

	//HAL_DATA_TYPE *pHalData = GET_HAL_DATA(pwadptdata);
	HAL_DATA_TYPE *pHalData = &pNic->HalData;

	struct phy_info *pPHYInfo = (struct phy_info *)(&pattrib->phy_info);

	struct mlme_ext_priv *pmlmeext = &pNic->mlmeextpriv;
	u8 *wlanhdr;
	PPHY_STATUS_RPT_9082_T pPhyStaRpt = (PPHY_STATUS_RPT_9082_T) pPhyStatus;
	OPS_DM_PACKET_INFO_T pkt_info;
	u8 *sa;
	int ret = FALSE;
	struct sta_priv *pstapriv;
	struct sta_info *psta = NULL;
	struct dvobj_priv *psdpriv = &pNic->dvobjpriv;
	//struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;

	
	int pkt_info_len;
	POPS_DM_PACKET_INFO_T pbuf;
	BOOLEAN bPacketBeacon;
	pkt_info.bPacketMatchBSSID = _FALSE;
	pkt_info.bPacketToSelf = _FALSE;
	pkt_info.bPacketBeacon = _FALSE;

	wlanhdr = pNicFragment->RxBuffer;
	sa = get_ta(wlanhdr);

	pkt_info.StationID = 0xFF;

	if (Func_Of_Proc_Pre_Memcmp(wadptdata_mac_addr(pNic), sa, ETH_ALEN) == _TRUE) {
		static u32 start_time = 0;

		if ((start_time == 0) || (Func_Of_Proc_Get_Passing_Time_Ms(start_time) > 5000)) {
			start_time = Func_Of_Proc_Get_Current_Time();
		}
		//pdbgpriv->dbg_rx_conflic_mac_addr_cnt++;
	} else {
		pstapriv = &pNic->stapriv;
		//psta = do_query_stainfo(pstapriv, sa, 1);
		//if (psta)
		//	pkt_info.StationID = psta->mac_id;
	}
	pkt_info.bPacketMatchBSSID = (!IsFrameTypeCtrl(wlanhdr))
		&& (!pattrib->icv_err) && (!pattrib->crc_err)
		&& Func_Of_Proc_Pre_Memcmp(get_hdr_bssid(wlanhdr), get_bssid(&pNic->mlmepriv),
					   ETH_ALEN);

	pkt_info.bPacketBeacon = pkt_info.bPacketMatchBSSID
		&& (GetFrameSubType(wlanhdr) == WIFI_BEACON);

	if (pkt_info.bPacketBeacon && pkt_info.StationID < 32) {
		bPacketBeacon_cnt++;
	}
	
	if (mlmeext_chk_scan_state(pmlmeext, SCAN_PROCESS)) {
		memcpy(pkt_info.Scaninfo.bssid, get_hdr_bssid(wlanhdr), ETH_ALEN);
	}
	pkt_info.bToSelf = (!pattrib->icv_err) && (!pattrib->crc_err)
		&& Func_Of_Proc_Pre_Memcmp(get_ra(wlanhdr), wadptdata_mac_addr(pNic), ETH_ALEN);

	pkt_info.bPacketToSelf = pkt_info.bPacketMatchBSSID
		&& Func_Of_Proc_Pre_Memcmp(get_ra(wlanhdr), wadptdata_mac_addr(pNic), ETH_ALEN);
	pkt_info.bcn_cnt = blastPackBcnCnt;
	pkt_info.DataRate = pattrib->data_rate;
	pkt_info.cck_agc_rpt_ofdm_cfosho_a = pPhyStaRpt->cck_agc_rpt_ofdm_cfosho_a;
	pkt_info.cck_sig_qual_ofdm_pwdb_all =
		pPhyStaRpt->cck_sig_qual_ofdm_pwdb_all;
	pkt_info.gain = pPhyStaRpt->path_agc[0].gain;
	pkt_info.path_rxsnr = pPhyStaRpt->path_rxsnr[0];
	pkt_info.stream_rxevm = pPhyStaRpt->stream_rxevm[0];
	pkt_info.path_cfotail = pPhyStaRpt->path_cfotail[0];

	msg_RxPhyStatus_work(pPHYInfo,pPhyStatus, pkt_info);
	rx_buf[0] = pPHYInfo->SignalStrength;
	rx_buf[1] = pPHYInfo->SignalQuality;
	{
		if (pkt_info.bPacketMatchBSSID
			&& (check_fwstate(&pNic->mlmepriv, WIFI_AP_STATE) == _TRUE)
			) {
			rx_process_phy_info(pNic, rx_buf);
		} else if (pkt_info.bPacketToSelf || pkt_info.bPacketBeacon) {
			rx_process_phy_info(pNic, rx_buf);
		}
	}

	phystatus2++;
	if (phystatus2 % 15) {
		return ret;
	}

	pbuf = (POPS_DM_PACKET_INFO_T)wl_zmalloc(RND4(sizeof(pkt_info)));
	if (pbuf == NULL) {
		res = _FALSE;
		goto exit;
	}

	phy_status_cmd = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (phy_status_cmd == NULL) {
		res = _FALSE;
		wl_mfree((u8 *)pbuf, RND4(sizeof(pkt_info)));
		goto exit;
	}

	Func_Of_Proc_Pre_Memcpy(pbuf, &pkt_info, sizeof(pkt_info));

	init_h2fwcmd_w_parm_no_rsp(phy_status_cmd, pbuf,
							   GEN_CMD_CODE(_PHYSTATUS));

	//res = preproc_enqueue_cmd_func(pcmdpriv, phy_status_cmd);
exit:

	return res;

}



u1Byte msg_QueryRxPwrPercentage(IN s1Byte AntPower)
{
        if ((AntPower <= -100) || (AntPower >= 20)) {
                return 0;
        } else if (AntPower >= 0) {
                return 100;
        } else {
                return (100 + AntPower);
        }

}

s1Byte msg_CCKRSSI_9086X(IN u1Byte LNA_idx, IN u1Byte VGA_idx)                                                  
{                                                                                                               
        s1Byte rx_pwr_all = 0x00;                                                                               
        switch (LNA_idx) {                                                                                      
        case 7:                                                                                                 
                if (VGA_idx <= 27)                                                                              
                        rx_pwr_all = -100 + 2 * (27 - VGA_idx);                                                 
                else                                                                                            
                        rx_pwr_all = -100;                                                                      
                break;                                                                                          
                                                                                                                
        case 5:                                                                                                 
                rx_pwr_all = -74 + 2 * (21 - VGA_idx);                                                          
                break;                                                                                          
                                                                                                                
        case 3:                                                                                                 
                rx_pwr_all = -60 + 2 * (20 - VGA_idx);                                                          
                break;                                                                                          
                                                                                                                
        case 1:                                                                                                 
                rx_pwr_all = -44 + 2 * (19 - VGA_idx);                                                          
                break;                                                                                          
                                                                                                                
        default:                                                                                                
                break;                                                                                          
        }                                                                                                       
        return rx_pwr_all;                                                                                      
}                                                                                                               
         
static u1Byte msg_EVMdbToPercentage(IN s1Byte Value)
{
        s1Byte ret_val;

        ret_val = Value;
        ret_val /= 2;

#ifdef MSG_EVM_ENHANCE_ANTDIV
        if (ret_val >= 0)
                ret_val = 0;

        if (ret_val <= -40)
                ret_val = -40;

        ret_val = 0 - ret_val;
        ret_val *= 3;
#else
        if (ret_val >= 0)
                ret_val = 0;

        if (ret_val <= -33)
                ret_val = -33;

        ret_val = 0 - ret_val;
        ret_val *= 3;

        if (ret_val == 99)
                ret_val = 100;
#endif

        return (u1Byte) ret_val;
}


void msg_RxPhyStatus_work(OUT struct phy_info * pPhyInfo,
				 IN u8 *pPhyStatus,
	 			 IN OPS_DM_PACKET_INFO_T pPktinfo)
{
	u1Byte i, Max_spatial_stream;
	s1Byte rx_pwr[4], rx_pwr_all = 0;
	u1Byte EVM, PWDB_ALL = 0, PWDB_ALL_BT;
	u1Byte RSSI, total_rssi = 0;
	BOOLEAN isCCKrate = FALSE;
	u1Byte rf_rx_num = 0;
	u1Byte LNA_idx = 0;
	u1Byte VGA_idx = 0;
	PPHY_STATUS_RPT_9082_T pPhyStaRpt = (PPHY_STATUS_RPT_9082_T) pPhyStatus;

	isCCKrate = (pPktinfo.DataRate <= DESC_RATE11M) ? TRUE : FALSE;
	pPhyInfo->RxMIMOSignalQuality[0] = -1;

	if (isCCKrate) {
		u1Byte report;
		u1Byte cck_agc_rpt;

		cck_agc_rpt = pPhyStaRpt->cck_agc_rpt_ofdm_cfosho_a;
		{
			LNA_idx = ((cck_agc_rpt & 0xE0) >> 5);
			VGA_idx = (cck_agc_rpt & 0x1F);
			rx_pwr_all = msg_CCKRSSI_9086X(LNA_idx, VGA_idx);
			PWDB_ALL = msg_QueryRxPwrPercentage(rx_pwr_all);
			if (PWDB_ALL > 100)
				PWDB_ALL = 100;
		}

		pPhyInfo->RxPWDBAll = PWDB_ALL;
		pPhyInfo->BTRxRSSIPercentage = PWDB_ALL;
		pPhyInfo->RecvSignalPower = rx_pwr_all;
		{
			u1Byte SQ, SQ_rpt;

			if (pPhyInfo->RxPWDBAll > 40) {
				SQ = 100;
			} else {
				SQ_rpt = pPhyStaRpt->cck_sig_qual_ofdm_pwdb_all;

				if (SQ_rpt > 64)
					SQ = 0;
				else if (SQ_rpt < 20)
					SQ = 100;
				else
					SQ = ((64 - SQ_rpt) * 100) / 44;

			}

			pPhyInfo->SignalQuality = SQ;
			pPhyInfo->RxMIMOSignalQuality[MSG_RF_PATH_A] = SQ;
		}

		pPhyInfo->RxMIMOSignalStrength[0] = PWDB_ALL;
	} else {
		for (i = 0; i < 1; i++) {
			rf_rx_num = 1;
			rx_pwr[i] = ((pPhyStaRpt->path_agc[i].gain & 0x3F) * 2) - 110;

			pPhyInfo->RxPwr[i] = rx_pwr[i];

			RSSI = msg_QueryRxPwrPercentage(rx_pwr[i]);
			total_rssi += RSSI;

			pPhyInfo->RxMIMOSignalStrength[i] = (u1Byte) RSSI;

			pPhyInfo->RxSNR[i] =
				(s4Byte) (pPhyStaRpt->path_rxsnr[i] / 2);

		}

		rx_pwr_all =
			(((pPhyStaRpt->cck_sig_qual_ofdm_pwdb_all) >> 1) & 0x7f) - 110;

		PWDB_ALL_BT = PWDB_ALL = msg_QueryRxPwrPercentage(rx_pwr_all);

		pPhyInfo->RxPWDBAll = PWDB_ALL;
		pPhyInfo->BTRxRSSIPercentage = PWDB_ALL_BT;
		pPhyInfo->RxPower = rx_pwr_all;
		pPhyInfo->RecvSignalPower = rx_pwr_all;

		EVM = msg_EVMdbToPercentage((pPhyStaRpt->stream_rxevm[0]));

		pPhyInfo->SignalQuality = (u1Byte) (EVM & 0xff);
		pPhyInfo->RxMIMOSignalQuality[i] = (u1Byte) (EVM & 0xff);

//		MSG_ParsingCFO(pPktinfo, pPhyStaRpt->path_cfotail);

	}
	if (isCCKrate) {
#ifdef CONFIG_SIGNAL_SCALE_MAPPING
		pPhyInfo->SignalStrength =
			(u1Byte) (Func_Hw_Op_Signalscalemapping(pDM_Odm, PWDB_ALL));
#else
		pPhyInfo->SignalStrength = (u1Byte) PWDB_ALL;

#endif
	} else {
		if (rf_rx_num != 0) {
#ifdef CONFIG_SIGNAL_SCALE_MAPPING
			pPhyInfo->SignalStrength =
				(u1Byte) (Func_Hw_Op_Signalscalemapping
						  (pDM_Odm, total_rssi /= rf_rx_num));
#else
			total_rssi /= rf_rx_num;
			pPhyInfo->SignalStrength = (u1Byte) total_rssi;
#endif
		}
	}

}


HAL_STATUS Func_Hw_Op_Configbbwithheaderfile(PNIC Nic, IN int ConfigType)
{
	int ret = FALSE;
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_INIT_BB_PHY_REG,
									&ConfigType, NULL, 1, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_INIT_BB_PHY_REG!!!\n",
				__func__));

		return HAL_STATUS_FAILURE;
	}
	return HAL_STATUS_SUCCESS;
}

HAL_STATUS Func_Hw_Op_Configrfwithheaderfile(IN PNIC Nic,
									  IN int ConfigType, IN int eRFPath)
{
	u32 buf[2] = { 0 };
	int len = 2;
	int ret;

	buf[0] = ConfigType;
	buf[1] = eRFPath;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_MSG_EMB_RF_IMG, buf, NULL,
									len, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_MSG_EMB_RF_IMG!!!\n",
				__func__));
		return HAL_STATUS_FAILURE;
	}
	return HAL_STATUS_SUCCESS;

}

HAL_STATUS Func_Hw_Op_Configrfwithtxpwrtrackheaderfile(IN PNIC Nic)
{

	int ret = FALSE;
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_MSG_EMB_TXPWRTRACK_IMG,
									NULL, NULL, 0, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_MSG_EMB_TXPWRTRACK_IMG!!!\n",
				__func__));

		return HAL_STATUS_FAILURE;
	}

	return HAL_STATUS_SUCCESS;
}

u4Byte
Func_Hw_Op_Get_Rate_Bitmap(IN PNIC Nic,
					IN u4Byte macid, IN u4Byte ra_mask, IN u1Byte rssi_level)
{
	u32 buf[3] = { 0 };
	u32 rate_bitmap = 0;
	int len = 3;
	int ret = FALSE;
	buf[0] = macid;
	buf[1] = ra_mask;
	buf[2] = rssi_level;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_0PS_MSG_GET_RATE_BITMAP, buf,
									&rate_bitmap, len, 1);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_0PS_MSG_GET_RATE_BITMAP!!!\n",
				__func__));
	}
	return rate_bitmap;

}

s4Byte Func_Chip_Bb_Dm_Cmd(IN PNIC Nic,
				 IN char *input,
				 IN u4Byte in_len,
				 IN u1Byte flag, OUT char *output, IN u4Byte out_len)
{

	return 1;
}


typedef struct MSG_STA_INFO {
	u8 bUsed;
	uint mac_id;
	u8 hwaddr[ETH_ALEN];
	u8 ra_rpt_linked;
	u8 wireless_mode;
	u8 rssi_level;
	u8 ra_change;
#ifdef CONFIG_80211N_HT
	struct ht_priv htpriv;
#endif
} MSG_STA_INFO_T;

static MSG_STA_INFO_T msg_sta_info[MSG_ASSOCIATE_ENTRY_NUM];

static void Func_Sync_Odm_Sta_Infor(IN PNIC Nic, int id)
{
	struct sta_info *psta = NULL;

	int len;
	u32 *pbuf;

	if (sizeof(msg_sta_info[0]) % 4 != 0) {
		MpTrace(COMP_RICHD, DBG_NORMAL,("error case !\n"));
		return;
	}
	len = RND4(sizeof(msg_sta_info[0]));

	pbuf = (u32 *) wl_zmalloc(len);
	if (!pbuf) {
		return;
	}

	memcpy(pbuf, &msg_sta_info[id], len);

	Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_SYNC_MSG_STA_INFO, pbuf,
								NULL, len / 4, 0);

	if (pbuf)
		wl_mfree((u8 *) pbuf, len);

}

static void Func_To_Get_Odm_Sta_Infor(PNIC Nic, int id)
{

	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &Nic->stapriv;
	int sta_len;
	u32 *pbuf;
	MSG_STA_INFO_T *p_msg_sta;
	u32 buf[1];

	sta_len = RND4(sizeof(MSG_STA_INFO_T));

#if 0
	psta = do_query_stainfo(pstapriv, msg_sta_info[id].hwaddr, 1);

	if (!psta) {
		return;
	}
#endif

	pbuf = (u32 *) wl_zmalloc(sta_len);
	if(pbuf == NULL)
		return;

	memcpy(pbuf, &msg_sta_info[id], sizeof(msg_sta_info[0]));

	buf[0] = id;

	Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_GET_MSG_STA_INFO, buf,
								pbuf, 1, sta_len / 4);

	p_msg_sta = (MSG_STA_INFO_T *) pbuf;

	if (p_msg_sta->bUsed && p_msg_sta->mac_id < MSG_ASSOCIATE_ENTRY_NUM) {

		NdisAcquireSpinLock(&sta_bh_lock);
		
		memcpy((u8 *) & msg_sta_info[p_msg_sta->mac_id], (u8 *) p_msg_sta,
			   sizeof(MSG_STA_INFO_T));
		(&Nic->RxInfo.PendingListLock);
		
		NdisReleaseSpinLock(&sta_bh_lock);
	}

	if(pbuf)
		wl_mfree((u8 *) pbuf, sta_len);
}

static void Func_Hw_Op_Sta_Hdl(PNIC Nic)
{
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &Nic->stapriv;
	int i;

	for (i = 0; i < MSG_ASSOCIATE_ENTRY_NUM; i++) {

		if (msg_sta_info[i].bUsed && msg_sta_info[i].ra_change) {
			msg_sta_info[i].ra_change = FALSE;
			Func_To_Get_Odm_Sta_Infor(Nic, i);
#if 0			
//			psta = do_query_stainfo(pstapriv, msg_sta_info[i].hwaddr, 1);
//			if (psta)
#endif
				Func_Of_Proc_Chip_Hw_Update_Ra_Mask(psta, psta->rssi_level);
		}
	}
}

VOID Func_Hw_Op_Cmninfoptrarrayhook(IN PNIC Nic,
							 IN u2Byte Index, IN PVOID pValue)
{
	MSG_STA_INFO_T *pmsg_sta;

	struct sta_info *psta = (struct sta_info *)pValue;

	if (Index > MSG_ASSOCIATE_ENTRY_NUM)
		return;

	if (psta) {
		NdisAcquireSpinLock(&sta_bh_lock);
		msg_sta_info[Index].bUsed = _TRUE;

		pmsg_sta = &msg_sta_info[Index];
		pmsg_sta->mac_id = psta->mac_id;
		memcpy(pmsg_sta->hwaddr, psta->hwaddr, 6);
		pmsg_sta->ra_rpt_linked = psta->ra_rpt_linked;
		pmsg_sta->wireless_mode = psta->wireless_mode;
		pmsg_sta->rssi_level = psta->rssi_level;
		pmsg_sta->ra_change = FALSE;
#ifdef CONFIG_80211N_HT
		memcpy(&pmsg_sta->htpriv, &psta->htpriv, sizeof(struct ht_priv));
#endif
		NdisReleaseSpinLock(&sta_bh_lock);
	} else {

		NdisAcquireSpinLock(&sta_bh_lock);
		msg_sta_info[Index].bUsed = FALSE;
		NdisReleaseSpinLock(&sta_bh_lock);
	}

	Func_Sync_Odm_Sta_Infor(Nic, Index);
}
