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

void Func_Of_Proc_Chip_Hw_Config_Rftype(PNIC Nic)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	pHalData->rf_type = 3;
	pHalData->NumTotalRFPath = 1;

	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s RF_Type is %d TotalTxPath is %d \n", __FUNCTION__,
			pHalData->rf_type, pHalData->NumTotalRFPath));
}

#define	EEPROM_CHANNEL_PLAN_BY_HW_MASK	0x80

u8 Func_Chip_Hw_Com_Config_Channel_Plan(IN PNIC Nic,
							   IN char *hw_alpha2,
							   IN u8 hw_chplan,
							   IN char *sw_alpha2,
							   IN u8 sw_chplan,
							   IN u8 def_chplan, IN BOOLEAN AutoLoadFail)
{
	PHAL_DATA_TYPE pHalData;
	u8 force_hw_chplan = FALSE;
	int chplan = -1;
	const struct country_chplan *country_ent = NULL, *ent;

	pHalData = GET_HAL_DATA(Nic);

	if (hw_chplan == 0xFF)
		goto chk_hw_country_code;

	if (AutoLoadFail == TRUE)
		goto chk_sw_config;

#ifndef CONFIG_FORCE_SW_CHANNEL_PLAN
	if (hw_chplan & EEPROM_CHANNEL_PLAN_BY_HW_MASK)
		force_hw_chplan = TRUE;
#endif

	hw_chplan &= (~EEPROM_CHANNEL_PLAN_BY_HW_MASK);

chk_hw_country_code:
	if (hw_alpha2 && !IS_ALPHA2_NO_SPECIFIED(hw_alpha2)) {
		ent = wl_query_chplan_from_list(hw_alpha2, 1);
		if (ent) {
			country_ent = ent;
			chplan = ent->chplan;
			goto chk_sw_config;
		} else
			MpTrace(COMP_RICHD, DBG_NORMAL, 
				("%s unsupported hw_alpha2:\"%c%c\"\n", __func__,
					   hw_alpha2[0], hw_alpha2[1]));
	}

	if (wl_is_channel_plan_valid(hw_chplan))
		chplan = hw_chplan;
	else if (force_hw_chplan == TRUE) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s unsupported hw_chplan:0x%02X\n", __func__, hw_chplan));
		force_hw_chplan = FALSE;
	}

chk_sw_config:
	if (force_hw_chplan == TRUE)
		goto done;

	if (sw_alpha2 && !IS_ALPHA2_NO_SPECIFIED(sw_alpha2)) {
		ent = wl_query_chplan_from_list(sw_alpha2, 1);
		if (ent) {
			country_ent = ent;
			chplan = ent->chplan;
			goto done;
		} else
			MpTrace(COMP_RICHD, DBG_NORMAL, 
				("%s unsupported sw_alpha2:\"%c%c\"\n", __func__,
					   sw_alpha2[0], sw_alpha2[1]));
	}

	if (wl_is_channel_plan_valid(sw_chplan)) {
		country_ent = NULL;
		chplan = sw_chplan;
	} else if (sw_chplan != WL_CHPLAN_MAX)
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s unsupported sw_chplan:0x%02X\n", __func__, sw_chplan));
done:
	if (chplan == -1) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s use def_chplan:0x%02X\n", __func__, def_chplan));
		chplan = def_chplan;
	} else if (country_ent) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s country code:\"%c%c\" with chplan:0x%02X\n", __func__,
				   country_ent->alpha2[0], country_ent->alpha2[1],
				   country_ent->chplan));
	} else
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s chplan:0x%02X\n", __func__, chplan));
	
	Nic->mlmepriv.country_ent = country_ent;
	pHalData->bDisableSWChannelPlan = force_hw_chplan;

	return chplan;
}

BOOLEAN Func_Chip_Hw_Islegalchannel(IN PNIC Nic, IN u32 Channel)
{
	BOOLEAN bLegalChannel = TRUE;

	if ((Channel <= 14) && (Channel >= 1)) {
		if (IsSupported24G(Nic->registrypriv.wireless_mode) == FALSE) {
			bLegalChannel = FALSE;
			MpTrace(COMP_RICHD, DBG_NORMAL, 
				("(Channel <= 14) && (Channel >=1) but wireless_mode do not support 2.4G\n"));
		}
	} else {
		bLegalChannel = FALSE;
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Channel is Invalid !!!\n"));
	}

	return bLegalChannel;
}

u8 Func_Mratetohwrate(u8 rate)
{
	u8 ret = DESC_RATE1M;

	switch (rate) {
	case MGN_1M:
		ret = DESC_RATE1M;
		break;
	case MGN_2M:
		ret = DESC_RATE2M;
		break;
	case MGN_5_5M:
		ret = DESC_RATE5_5M;
		break;
	case MGN_11M:
		ret = DESC_RATE11M;
		break;
	case MGN_6M:
		ret = DESC_RATE6M;
		break;
	case MGN_9M:
		ret = DESC_RATE9M;
		break;
	case MGN_12M:
		ret = DESC_RATE12M;
		break;
	case MGN_18M:
		ret = DESC_RATE18M;
		break;
	case MGN_24M:
		ret = DESC_RATE24M;
		break;
	case MGN_36M:
		ret = DESC_RATE36M;
		break;
	case MGN_48M:
		ret = DESC_RATE48M;
		break;
	case MGN_54M:
		ret = DESC_RATE54M;
		break;

	case MGN_MCS0:
		ret = DESC_RATEMCS0;
		break;
	case MGN_MCS1:
		ret = DESC_RATEMCS1;
		break;
	case MGN_MCS2:
		ret = DESC_RATEMCS2;
		break;
	case MGN_MCS3:
		ret = DESC_RATEMCS3;
		break;
	case MGN_MCS4:
		ret = DESC_RATEMCS4;
		break;
	case MGN_MCS5:
		ret = DESC_RATEMCS5;
		break;
	case MGN_MCS6:
		ret = DESC_RATEMCS6;
		break;
	case MGN_MCS7:
		ret = DESC_RATEMCS7;
		break;
	default:
		break;
	}

	return ret;
}

u8 Func_Hwratetomrate(u8 rate)
{
	u8 ret_rate = MGN_1M;

	switch (rate) {

	case DESC_RATE1M:
		ret_rate = MGN_1M;
		break;
	case DESC_RATE2M:
		ret_rate = MGN_2M;
		break;
	case DESC_RATE5_5M:
		ret_rate = MGN_5_5M;
		break;
	case DESC_RATE11M:
		ret_rate = MGN_11M;
		break;
	case DESC_RATE6M:
		ret_rate = MGN_6M;
		break;
	case DESC_RATE9M:
		ret_rate = MGN_9M;
		break;
	case DESC_RATE12M:
		ret_rate = MGN_12M;
		break;
	case DESC_RATE18M:
		ret_rate = MGN_18M;
		break;
	case DESC_RATE24M:
		ret_rate = MGN_24M;
		break;
	case DESC_RATE36M:
		ret_rate = MGN_36M;
		break;
	case DESC_RATE48M:
		ret_rate = MGN_48M;
		break;
	case DESC_RATE54M:
		ret_rate = MGN_54M;
		break;
	case DESC_RATEMCS0:
		ret_rate = MGN_MCS0;
		break;
	case DESC_RATEMCS1:
		ret_rate = MGN_MCS1;
		break;
	case DESC_RATEMCS2:
		ret_rate = MGN_MCS2;
		break;
	case DESC_RATEMCS3:
		ret_rate = MGN_MCS3;
		break;
	case DESC_RATEMCS4:
		ret_rate = MGN_MCS4;
		break;
	case DESC_RATEMCS5:
		ret_rate = MGN_MCS5;
		break;
	case DESC_RATEMCS6:
		ret_rate = MGN_MCS6;
		break;
	case DESC_RATEMCS7:
		ret_rate = MGN_MCS7;
		break;

	default:
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Func_Hwratetomrate(): Non supported Rate [%x]!!!\n", rate));
		break;
	}

	return ret_rate;
}

void Func_Chip_Hw_Setbratecfg(IN PNIC Nic, IN u8 * mBratesOS, OUT u16 * pBrateCfg)
{
	u8 i, is_brate, brate;

	for (i = 0; i < NDIS_802_11_LENGTH_RATES_EX; i++) {
		is_brate = mBratesOS[i] & IEEE80211_BASIC_RATE_MASK;
		brate = mBratesOS[i] & 0x7f;

		if (is_brate) {
			switch (brate) {
			case IEEE80211_CCK_RATE_1MB:
				*pBrateCfg |= RATE_1M;
				break;
			case IEEE80211_CCK_RATE_2MB:
				*pBrateCfg |= RATE_2M;
				break;
			case IEEE80211_CCK_RATE_5MB:
				*pBrateCfg |= RATE_5_5M;
				break;
			case IEEE80211_CCK_RATE_11MB:
				*pBrateCfg |= RATE_11M;
				break;
			case IEEE80211_OFDM_RATE_6MB:
				*pBrateCfg |= RATE_6M;
				break;
			case IEEE80211_OFDM_RATE_9MB:
				*pBrateCfg |= RATE_9M;
				break;
			case IEEE80211_OFDM_RATE_12MB:
				*pBrateCfg |= RATE_12M;
				break;
			case IEEE80211_OFDM_RATE_18MB:
				*pBrateCfg |= RATE_18M;
				break;
			case IEEE80211_OFDM_RATE_24MB:
				*pBrateCfg |= RATE_24M;
				break;
			case IEEE80211_OFDM_RATE_36MB:
				*pBrateCfg |= RATE_36M;
				break;
			case IEEE80211_OFDM_RATE_48MB:
				*pBrateCfg |= RATE_48M;
				break;
			case IEEE80211_OFDM_RATE_54MB:
				*pBrateCfg |= RATE_54M;
				break;
			}
		}
	}
}
#if 0
static VOID Func_Oneoutpipemapping(IN PNIC Nic)
{
	struct dvobj_priv *pdvobjpriv = wadptdata_to_dvobj(pAdapter);

	pdvobjpriv->Queue2Pipe[0] = pdvobjpriv->RtOutPipe[0];
	pdvobjpriv->Queue2Pipe[1] = pdvobjpriv->RtOutPipe[0];
	pdvobjpriv->Queue2Pipe[2] = pdvobjpriv->RtOutPipe[0];
	pdvobjpriv->Queue2Pipe[3] = pdvobjpriv->RtOutPipe[0];

	pdvobjpriv->Queue2Pipe[4] = pdvobjpriv->RtOutPipe[0];
	pdvobjpriv->Queue2Pipe[5] = pdvobjpriv->RtOutPipe[0];
	pdvobjpriv->Queue2Pipe[6] = pdvobjpriv->RtOutPipe[0];
	pdvobjpriv->Queue2Pipe[7] = pdvobjpriv->RtOutPipe[0];
}

static VOID Func_Two_Outpipe_Mapping(IN PNIC Nic, IN BOOLEAN bWIFICfg)
{
	struct dvobj_priv *pdvobjpriv = wadptdata_to_dvobj(pAdapter);

	if (bWIFICfg) {

		pdvobjpriv->Queue2Pipe[0] = pdvobjpriv->RtOutPipe[1];
		pdvobjpriv->Queue2Pipe[1] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[2] = pdvobjpriv->RtOutPipe[1];
		pdvobjpriv->Queue2Pipe[3] = pdvobjpriv->RtOutPipe[0];

		pdvobjpriv->Queue2Pipe[4] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[5] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[6] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[7] = pdvobjpriv->RtOutPipe[0];

	} else {

		pdvobjpriv->Queue2Pipe[0] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[1] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[2] = pdvobjpriv->RtOutPipe[1];
		pdvobjpriv->Queue2Pipe[3] = pdvobjpriv->RtOutPipe[1];

		pdvobjpriv->Queue2Pipe[4] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[5] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[6] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[7] = pdvobjpriv->RtOutPipe[0];

	}

}

static VOID Func_Three_Outpipe_Mapping(IN PNIC Nic, IN BOOLEAN bWIFICfg)
{
	struct dvobj_priv *pdvobjpriv = wadptdata_to_dvobj(pAdapter);

	if (bWIFICfg) {

		pdvobjpriv->Queue2Pipe[0] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[1] = pdvobjpriv->RtOutPipe[1];
		pdvobjpriv->Queue2Pipe[2] = pdvobjpriv->RtOutPipe[2];
		pdvobjpriv->Queue2Pipe[3] = pdvobjpriv->RtOutPipe[1];

		pdvobjpriv->Queue2Pipe[4] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[5] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[6] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[7] = pdvobjpriv->RtOutPipe[0];

	} else {

		pdvobjpriv->Queue2Pipe[0] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[1] = pdvobjpriv->RtOutPipe[1];
		pdvobjpriv->Queue2Pipe[2] = pdvobjpriv->RtOutPipe[2];
		pdvobjpriv->Queue2Pipe[3] = pdvobjpriv->RtOutPipe[2];

		pdvobjpriv->Queue2Pipe[4] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[5] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[6] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[7] = pdvobjpriv->RtOutPipe[0];
	}

}

static VOID Func_Four_Out_Pipe_Mapping(IN PNIC Nic, IN BOOLEAN bWIFICfg)
{
	struct dvobj_priv *pdvobjpriv = wadptdata_to_dvobj(pAdapter);

	if (bWIFICfg) {

		pdvobjpriv->Queue2Pipe[0] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[1] = pdvobjpriv->RtOutPipe[1];
		pdvobjpriv->Queue2Pipe[2] = pdvobjpriv->RtOutPipe[2];
		pdvobjpriv->Queue2Pipe[3] = pdvobjpriv->RtOutPipe[1];

		pdvobjpriv->Queue2Pipe[4] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[5] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[6] = pdvobjpriv->RtOutPipe[3];
		pdvobjpriv->Queue2Pipe[7] = pdvobjpriv->RtOutPipe[0];

	} else {

		pdvobjpriv->Queue2Pipe[0] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[1] = pdvobjpriv->RtOutPipe[1];
		pdvobjpriv->Queue2Pipe[2] = pdvobjpriv->RtOutPipe[2];
		pdvobjpriv->Queue2Pipe[3] = pdvobjpriv->RtOutPipe[2];

		pdvobjpriv->Queue2Pipe[4] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[5] = pdvobjpriv->RtOutPipe[0];
		pdvobjpriv->Queue2Pipe[6] = pdvobjpriv->RtOutPipe[3];
		pdvobjpriv->Queue2Pipe[7] = pdvobjpriv->RtOutPipe[0];
	}

}


BOOLEAN Func_Chip_Hw_Mappingoutpipe(IN PNIC Nic, IN u8 NumOutPipe)
{
	struct registry_priv *pregistrypriv = &pAdapter->registrypriv;

	BOOLEAN bWIFICfg = FALSE;

	BOOLEAN result = TRUE;

	switch (NumOutPipe) {
	case 2:
		Func_Two_Outpipe_Mapping(pAdapter, bWIFICfg);
		break;
	case 3:
	case 4:
		Func_Three_Outpipe_Mapping(pAdapter, bWIFICfg);
		break;
	case 1:
		Func_Oneoutpipemapping(pAdapter);
		break;
	default:
		result = FALSE;
		break;
	}

	return result;

}
#endif

void Func_Chip_Hw_Init_Macaddr(PNIC Nic)
{
	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_MAC_ADDR, wadptdata_mac_addr(Nic));
#ifdef  CONFIG_CONCURRENT_MODE
	if (Nic->pbuddy_wadptdata)
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic->pbuddy_wadptdata, HW_VAR_MAC_ADDR,
						  wadptdata_mac_addr(Nic->pbuddy_wadptdata));
#endif
}

void Func_Of_Proc_Init_Hal_Com_Default_Value(PNIC Nic)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

	pHalData->AntDetection = 1;
}

void Func_Dev_To_Host_Evt_Clear(PNIC Nic)
{
#if 1
#else
	wl_write8(wadptdata, REG_wMBOX0EVT_CLEAR, wMBOX0_EVT_HOST_CLOSE);
#endif
}

s32 Func_Dev_To_Host_Evt_Read(PNIC Nic, u8 * buf)
{
	s32 ret = FALSE;
	struct wmbox0_evt_hdr *wmbox0_evt;
	int i;
	u8 trigger;

	if (buf == NULL)
		goto exit;

exit:
	return ret;
}

s32 Func_Dev_To_Host_Evt_Read_Process(PNIC Nic, u8 * buf)
{
	s32 ret = FALSE;
	struct wmbox0_evt_hdr_88xx *wmbox0_evt;
	int i;
	u8 trigger;

	if (buf == NULL)
		goto exit;
exit:
	return ret;
}

static unsigned char Func_To_Get_Highest_Rate_Idx(u32 mask)
{
	int i;
	unsigned char rate_idx = 0;

	for (i = 31; i >= 0; i--) {
		if (mask & BIT(i)) {
			rate_idx = i;
			break;
		}
	}

	return rate_idx;
}

void Func_Of_Proc_Chip_Hw_Update_Sta_Rate_Mask(PNIC Nic, struct sta_info *psta)
{
	u8 i, rf_type, limit;
	u64 tx_ra_bitmap;

	if (psta == NULL) {
		return;
	}

	tx_ra_bitmap = 0;

	for (i = 0; i < sizeof(psta->bssrateset); i++) {
		if (psta->bssrateset[i])
			tx_ra_bitmap |=
				bit_value_from_ieee_value_to_get_func(psta->bssrateset[i] & 0x7f,
												  1);
	}

#ifdef CONFIG_80211N_HT
	{
		if (psta->htpriv.ht_option) {
			rf_type = 3;
			Func_Of_Proc_Chip_Hw_Get_Hwreg(Nic, HW_VAR_RF_TYPE, (u8 *) (&rf_type));
			limit = 8;

			if (check_fwstate(&Nic->mlmepriv, WIFI_AP_STATE) == TRUE) {
				if (psta->htpriv.smps_cap == 0 || psta->htpriv.smps_cap == 1) {
					limit = 8;
				}
			}

			for (i = 0; i < limit; i++) {
				if (psta->htpriv.ht_cap.supp_mcs_set[i / 8] & BIT(i % 8))
					tx_ra_bitmap |= BIT(i + 12);
			}
		}
	}
#endif
	psta->ra_mask = tx_ra_bitmap;
	psta->init_rate = Func_To_Get_Highest_Rate_Idx(tx_ra_bitmap) & 0x3f;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("supp_mcs_set = %02x, %02x, %02x, rf_type=%d, tx_ra_bitmap=%016llx,initrate=%x\n",
		 psta->htpriv.ht_cap.supp_mcs_set[0],
		 psta->htpriv.ht_cap.supp_mcs_set[1],
		 psta->htpriv.ht_cap.supp_mcs_set[2], rf_type, tx_ra_bitmap,psta->init_rate));

	
}

#ifndef SEC_CAM_ACCESS_TIMEOUT_MS
#define SEC_CAM_ACCESS_TIMEOUT_MS 200
#endif

#ifndef DBG_SEC_CAM_ACCESS
#define DBG_SEC_CAM_ACCESS 0
#endif

static void Func_Of_Proc_Sec_Read_Cam_Ent(PNIC Nic, u8 id, u8 * ctrl, u8 * mac,
						  u8 * key)
{
	u8 i;
	u32 *mbox;
	int ret = 0;
	u32 idno = id;

	mbox = (u32 *) wl_malloc((23) * 4);
	if (!mbox) {

		return;
	}
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_SEC_READ_CAM, &idno,
									mbox, 1, 23);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Error: ===>%s fail: code :UMSG_OPS_HAL_SEC_WRITE_CAM\n",
				__func__));
		wl_mfree((u8 *) mbox, (23) * 4);
		return;
	}

	ctrl = (u8 *) (&mbox[0]);

	for (i = 0; i < 6; i++)
		mac[i] = (u8) mbox[i + 1];

	for (i = 0; i < 16; i++)
		key[i] = (u8) mbox[i + 7];

	if (mbox)
		wl_mfree((u8 *) mbox, (23) * 4);

}

void Func_Of_Proc_Sec_Write_Cam_Ent(PNIC Nic, u8 id, u16 ctrl, u8 * mac,
						   u8 * key)
{
	unsigned int i;
	int j, ret = FALSE;
	u8 addr;
	u32 wdata;
	u32 mbox[100];
	/*u32 *mbox = (u32 *) wl_malloc((24) * 4);
	if (!mbox) {

		return;
	}*/

	mbox[0] = id;
	mbox[1] = ctrl;

	for (i = 0; i < 6; i++)
		mbox[i + 2] = mac[i];

	for (i = 0; i < 16; i++)
		mbox[i + 8] = key[i];

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_SEC_WRITE_CAM, mbox,
									NULL, 24, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Error: ===>%s fail: code :UMSG_OPS_HAL_SEC_WRITE_CAM\n",
				__func__));
		/*wl_mfree((u8 *) mbox, (24) * 4);*/
		return;
	}
	/*if (mbox)
		wl_mfree((u8 *) mbox, (24) * 4);*/
}

bool Func_Of_Proc_Sec_Read_Cam_Is_Gk(PNIC Nic, u8 id)
{
	bool res;
	u16 ctrl;

	Func_Of_Proc_Sec_Read_Cam_Ent(Nic, id, (u8 *) & ctrl, NULL, NULL);

	res = (ctrl & BIT6) ? TRUE : FALSE;
	return res;
}

const char *const _wmbox1_msr_role_str[] = {
	"RSVD",
	"STA",
	"AP",
	"GC",
	"GO",
	"TDLS",
	"ADHOC",
	"INVALID",
};

s32 Func_Of_Proc_Chip_Hw_Set_Fwmediastatusrpt_Cmd(PNIC Nic, bool opmode,
									 bool miracast, bool miracast_sink, u8 role,
									 u8 macid, bool macid_ind, u8 macid_end)
{
	struct macid_ctl_t *macid_ctl = &Nic->dvobjpriv.macid_ctl;
	u8 parm[wMBOX1_MEDIA_STATUS_RPT_LEN] = { 0 };
	int i;
	s32 ret;

	SET_wMBOX1CMD_MSRRPT_PARM_OPMODE(parm, opmode);
	SET_wMBOX1CMD_MSRRPT_PARM_MACID_IND(parm, macid_ind);
	SET_wMBOX1CMD_MSRRPT_PARM_MIRACAST(parm, miracast);
	SET_wMBOX1CMD_MSRRPT_PARM_MIRACAST_SINK(parm, miracast_sink);
	SET_wMBOX1CMD_MSRRPT_PARM_ROLE(parm, role);
	SET_wMBOX1CMD_MSRRPT_PARM_MACID(parm, macid);
	SET_wMBOX1CMD_MSRRPT_PARM_MACID_END(parm, macid_end);

	ret =
		Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic, wMBOX1_MEDIA_STATUS_RPT,
							 wMBOX1_MEDIA_STATUS_RPT_LEN, parm);
	if (ret != TRUE)
		goto exit;

	SET_wMBOX1CMD_MSRRPT_PARM_MACID_IND(parm, 0);
	if (macid_ind == 0)
		macid_end = macid;

	for (i = macid; macid <= macid_end; macid++)
		wl_wmid_ctl_set_msr(macid_ctl, macid, parm[0], 1);

exit:
	return ret;
}

inline s32 Func_Of_Proc_Chip_Hw_Set_Fwmediastatusrpt_Single_Cmd(PNIC Nic,
												   bool opmode, bool miracast,
												   bool miracast_sink, u8 role,
												   u8 macid)
{
	return Func_Of_Proc_Chip_Hw_Set_Fwmediastatusrpt_Cmd(Nic, opmode, miracast,
											miracast_sink, role, macid, 0, 0);
}

void Func_Of_Proc_Chip_Hw_Set_Fwrsvdpage_Cmd(PNIC Nic, PRSVDPAGE_LOC rsvdpageloc)
{
	u8 u1wMBOX1RsvdPageParm[wMBOX1_RSVDPAGE_LOC_LEN] = { 0 };
	u8 ret = 0;
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("RsvdPageLoc: ProbeRsp=%d PsPoll=%d Null=%d QoSNull=%d BTNull=%d\n",
			rsvdpageloc->LocProbeRsp, rsvdpageloc->LocPsPoll,
			rsvdpageloc->LocNullData, rsvdpageloc->LocQosNull,
			rsvdpageloc->LocBTQosNull));

	SET_wMBOX1CMD_RSVDPAGE_LOC_PROBE_RSP(u1wMBOX1RsvdPageParm,
									  rsvdpageloc->LocProbeRsp);
	SET_wMBOX1CMD_RSVDPAGE_LOC_PSPOLL(u1wMBOX1RsvdPageParm, rsvdpageloc->LocPsPoll);
	SET_wMBOX1CMD_RSVDPAGE_LOC_NULL_DATA(u1wMBOX1RsvdPageParm,
									  rsvdpageloc->LocNullData);
	SET_wMBOX1CMD_RSVDPAGE_LOC_QOS_NULL_DATA(u1wMBOX1RsvdPageParm,
										  rsvdpageloc->LocQosNull);
	SET_wMBOX1CMD_RSVDPAGE_LOC_BT_QOS_NULL_DATA(u1wMBOX1RsvdPageParm,
											 rsvdpageloc->LocBTQosNull);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic,
							   wMBOX1_RSVD_PAGE,
							   wMBOX1_RSVDPAGE_LOC_LEN, u1wMBOX1RsvdPageParm);

}


void Func_Of_Proc_Chip_Hw_Set_Fwaoacrsvdpage_Cmd(PNIC Nic,
									PRSVDPAGE_LOC rsvdpageloc)
{
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	u8 res = 0, count = 0, ret = 0;
#ifdef CONFIG_WOWLAN
	u8 u1wMBOX1AoacRsvdPageParm[wMBOX1_AOAC_RSVDPAGE_LOC_LEN] = { 0 };
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("AOACRsvdPageLoc: RWC=%d ArpRsp=%d NbrAdv=%d GtkRsp=%d GtkInfo=%d ProbeReq=%d NetworkList=%d\n",
		 rsvdpageloc->LocRemoteCtrlInfo, rsvdpageloc->LocArpRsp,
		 rsvdpageloc->LocNbrAdv, rsvdpageloc->LocGTKRsp,
		 rsvdpageloc->LocGTKInfo, rsvdpageloc->LocProbeReq,
		 rsvdpageloc->LocNetList));

	if (check_fwstate(pmlmepriv, _FW_LINKED)) {
		SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_REMOTE_WAKE_CTRL_INFO
			(u1wMBOX1AoacRsvdPageParm, rsvdpageloc->LocRemoteCtrlInfo);
		SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_ARP_RSP(u1wMBOX1AoacRsvdPageParm,
											 rsvdpageloc->LocArpRsp);
		SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_GTK_RSP(u1wMBOX1AoacRsvdPageParm,
											 rsvdpageloc->LocGTKRsp);
		SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_GTK_INFO(u1wMBOX1AoacRsvdPageParm,
											  rsvdpageloc->LocGTKInfo);
		ret =
			Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic, wMBOX1_AOAC_RSVD_PAGE,
								 wMBOX1_AOAC_RSVDPAGE_LOC_LEN,
								 u1wMBOX1AoacRsvdPageParm);
	}
#endif
}

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
static void Func_Of_Proc_Chip_Hw_Force_Enable_Rxdma(PNIC Nic)
{
	int ret = FALSE;
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_ENABLE_RXDMA, NULL,
									NULL, 0, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("==================> %s,fail!\n", __func__));
		return;
	}
}

static void Func_Of_Proc_Chip_Hw_Release_Rx_Dma(PNIC Nic)
{
	int ret = FALSE;
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_RELEASE_RX_DMA, NULL,
									NULL, 0, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("==================> %s,fail!\n", __func__));
		return;
	}
}

static u8 Func_Of_Proc_Chip_Hw_Pause_Rx_Dma(PNIC Nic)
{
	u8 ret = 0;
	s8 trycnt = 100;
	u16 len = 0;
	u32 tmp = 0;
	int res = 0;
	u32 outbox = FALSE;
	int m0ret = FALSE;

	if (Nic->intf_start)
		Nic->intf_start(Nic);

	m0ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_PAUSE_RXDMA, NULL,
									&outbox, 0, 1);
	if (!m0ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("=================> %s,fail!\n", __func__));
		return FALSE;
	}

	if (outbox) {
		if (Nic->intf_stop)
			Nic->intf_stop(Nic);
		WL_ENABLE_FUNC(Nic, DF_RX_BIT);
		WL_ENABLE_FUNC(Nic, DF_TX_BIT);

		ret = TRUE;
	} else {
		ret = FALSE;
	}

	return ret;
}

#endif

#ifdef CONFIG_WOWLAN
static u8 Func_Of_Proc_Chip_Hw_Check_Wow_Ctrl(PNIC Nic, u8 chk_type)
{
	int ret = FALSE;
	u32 m_chk = chk_type;
	u32 m_res = FALSE;
	u8 res = FALSE;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_CHECK_WOW_CTL, &m_chk,
									&m_res, 1, 1);
	if (!ret) {
		return res;
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("====>%s,fail\n", __func__));
	}
	res = (u8) m_res;
	return res;
}


static void Func_Of_Proc_Chip_Hw_Update_Tx_Iv(PNIC Nic)
{
	struct pwrctrl_priv *pwrctl = &Nic->pwrctl_priv;
	u64 iv_low = 0, iv_high = 0;

	int ret = FALSE;

	u32 iv[2] = { 0 };
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_UPDATE_TX_IV, NULL, iv,
									0, 2);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("===========>%s,fail!\n", __func__));
		return;
	}
	iv_low = iv[0];
	iv_low &= 0xffff;
	iv_high = iv[1];
	pwrctl->wowlan_fw_iv = iv_high << 16 | iv_low;

	do_set_sec_pn(Nic);
}

static u8 Func_Of_Proc_Chip_Hw_Set_Keep_Alive_Cmd(PNIC Nic, u8 enable, u8 pkt_type)
{

	u8 u1wMBOX1KeepAliveParm[wMBOX1_KEEP_ALIVE_CTRL_LEN] = { 0 };
	u8 adopt = 1, check_period = 5;
	u8 ret = FALSE;
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s(): enable = %d\n", __func__, enable));

	SET_wMBOX1CMD_KEEPALIVE_PARM_ENABLE(u1wMBOX1KeepAliveParm, enable);
	SET_wMBOX1CMD_KEEPALIVE_PARM_ADOPT(u1wMBOX1KeepAliveParm, adopt);
	SET_wMBOX1CMD_KEEPALIVE_PARM_PKT_TYPE(u1wMBOX1KeepAliveParm, pkt_type);
	SET_wMBOX1CMD_KEEPALIVE_PARM_CHECK_PERIOD(u1wMBOX1KeepAliveParm, check_period);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic,
							   wMBOX1_KEEP_ALIVE,
							   wMBOX1_KEEP_ALIVE_CTRL_LEN, u1wMBOX1KeepAliveParm);

	return ret;
}

static u8 Func_Of_Proc_Chip_Hw_Set_Disconnect_Decision_Cmd(PNIC Nic, u8 enable)
{
	u8 u1wMBOX1DisconDecisionParm[wMBOX1_DISCON_DECISION_LEN] = { 0 };
	u8 adopt = 1, check_period = 10, trypkt_num = 0;
	u8 ret = FALSE;
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s(): enable = %d\n", __func__, enable));
	SET_wMBOX1CMD_DISCONDECISION_PARM_ENABLE(u1wMBOX1DisconDecisionParm, enable);
	SET_wMBOX1CMD_DISCONDECISION_PARM_ADOPT(u1wMBOX1DisconDecisionParm, adopt);
	SET_wMBOX1CMD_DISCONDECISION_PARM_CHECK_PERIOD(u1wMBOX1DisconDecisionParm,
												check_period);
	SET_wMBOX1CMD_DISCONDECISION_PARM_TRY_PKT_NUM(u1wMBOX1DisconDecisionParm,
											   trypkt_num);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic,
							   wMBOX1_DISCON_DECISION,
							   wMBOX1_DISCON_DECISION_LEN,
							   u1wMBOX1DisconDecisionParm);
	return ret;
}

static u8 Func_Of_Proc_Chip_Hw_Set_Wowlan_Ctrl_Cmd(PNIC Nic, u8 enable,
									  u8 change_unit)
{
	struct security_priv *psecpriv = &Nic->securitypriv;
	struct pwrctrl_priv *ppwrpriv = &Nic->pwrctl_priv;

	u8 u1wMBOX1WoWlanCtrlParm[wMBOX1_WOWLAN_LEN] = { 0 };
	u8 discont_wake = 1, gpionum = 0, gpio_dur = 0;
	u8 hw_unicast = 0, gpio_pulse_cnt = 0, gpio_pulse_en = 0;
	u8 sdio_wakeup_enable = 1;
	u8 gpio_high_active = 0;
	u8 pattern_en = 0;
	u8 magic_pkt = 0;
	u8 gpio_unit = 0;
	u8 ret = FALSE;

	if (!ppwrpriv->wowlan_pno_enable)
		magic_pkt = enable;

	if (psecpriv->dot11PrivacyAlgrthm == _WEP40_
		|| psecpriv->dot11PrivacyAlgrthm == _WEP104_)
		hw_unicast = 1;
	else
		hw_unicast = 0;

	if (ppwrpriv->wowlan_pattern) {
		if (enable)
			pattern_en = 1;
		else
			pattern_en = 0;
	}
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s(): enable=%d change_unit=%d\n", __func__, enable, change_unit));

	if (enable && change_unit) {
		gpio_dur = 0x40;
		gpio_unit = 1;
		gpio_pulse_en = 1;
	}

	SET_wMBOX1CMD_WOWLAN_FUNC_ENABLE(u1wMBOX1WoWlanCtrlParm, enable);
	SET_wMBOX1CMD_WOWLAN_PATTERN_MATCH_ENABLE(u1wMBOX1WoWlanCtrlParm, pattern_en);
	SET_wMBOX1CMD_WOWLAN_MAGIC_PKT_ENABLE(u1wMBOX1WoWlanCtrlParm, magic_pkt);
	SET_wMBOX1CMD_WOWLAN_UNICAST_PKT_ENABLE(u1wMBOX1WoWlanCtrlParm, hw_unicast);
	SET_wMBOX1CMD_WOWLAN_ALL_PKT_DROP(u1wMBOX1WoWlanCtrlParm, 0);
	SET_wMBOX1CMD_WOWLAN_GPIO_ACTIVE(u1wMBOX1WoWlanCtrlParm, gpio_high_active);

	SET_wMBOX1CMD_WOWLAN_REKEY_WAKE_UP(u1wMBOX1WoWlanCtrlParm, enable);

	SET_wMBOX1CMD_WOWLAN_DISCONNECT_WAKE_UP(u1wMBOX1WoWlanCtrlParm, discont_wake);
	SET_wMBOX1CMD_WOWLAN_GPIONUM(u1wMBOX1WoWlanCtrlParm, gpionum);
	SET_wMBOX1CMD_WOWLAN_DATAPIN_WAKE_UP(u1wMBOX1WoWlanCtrlParm, sdio_wakeup_enable);

	SET_wMBOX1CMD_WOWLAN_GPIO_DURATION(u1wMBOX1WoWlanCtrlParm, gpio_dur);
	SET_wMBOX1CMD_WOWLAN_CHANGE_UNIT(u1wMBOX1WoWlanCtrlParm, gpio_unit);

	SET_wMBOX1CMD_WOWLAN_GPIO_PULSE_EN(u1wMBOX1WoWlanCtrlParm, gpio_pulse_en);
	SET_wMBOX1CMD_WOWLAN_GPIO_PULSE_COUNT(u1wMBOX1WoWlanCtrlParm, gpio_pulse_cnt);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic,
							   wMBOX1_WOWLAN, wMBOX1_WOWLAN_LEN, u1wMBOX1WoWlanCtrlParm);
	return ret;
}

static u8 Func_Of_Proc_Chip_Hw_Set_Remote_Wake_Ctrl_Cmd(PNIC Nic, u8 enable)
{
	struct security_priv *psecuritypriv = &(Nic->securitypriv);
	struct pwrctrl_priv *ppwrpriv = &Nic->pwrctl_priv;
	u8 u1wMBOX1RemoteWakeCtrlParm[wMBOX1_REMOTE_WAKE_CTRL_LEN] = { 0 };
	u8 ret = FALSE, count = 0;

	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s(): enable=%d\n", __func__, enable));

	if (!ppwrpriv->wowlan_pno_enable) {
		SET_wMBOX1CMD_REMOTE_WAKECTRL_ENABLE(u1wMBOX1RemoteWakeCtrlParm, enable);
		SET_wMBOX1CMD_REMOTE_WAKE_CTRL_ARP_OFFLOAD_EN(u1wMBOX1RemoteWakeCtrlParm, 1);

		SET_wMBOX1CMD_REMOTE_WAKE_CTRL_FW_UNICAST_EN(u1wMBOX1RemoteWakeCtrlParm,
												  !ppwrpriv->wowlan_pattern);

		SET_wMBOX1CMD_REMOTE_WAKE_CTRL_NBNS_FILTER_EN(u1wMBOX1RemoteWakeCtrlParm,
												   !ppwrpriv->wowlan_pattern);

		if ((psecuritypriv->dot11PrivacyAlgrthm == _AES_) ||
			(psecuritypriv->dot11PrivacyAlgrthm == _NO_PRIVACY_)) {
			SET_wMBOX1CMD_REMOTE_WAKE_CTRL_ARP_ACTION(u1wMBOX1RemoteWakeCtrlParm, 0);
		} else {
			SET_wMBOX1CMD_REMOTE_WAKE_CTRL_ARP_ACTION(u1wMBOX1RemoteWakeCtrlParm, 1);
		}

		SET_wMBOX1CMD_REMOTE_WAKE_CTRL_FW_PARSING_UNTIL_WAKEUP
			(u1wMBOX1RemoteWakeCtrlParm, 1);
	}

#ifdef CONFIG_P2P_WOWLAN
	if (TRUE == ppwrpriv->wowlan_p2p_mode) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("P2P OFFLOAD ENABLE\n"));
		SET_wMBOX1CMD_REMOTE_WAKE_CTRL_P2P_OFFLAD_EN(u1wMBOX1RemoteWakeCtrlParm, 1);
	} else {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("P2P OFFLOAD DISABLE\n"));
		SET_wMBOX1CMD_REMOTE_WAKE_CTRL_P2P_OFFLAD_EN(u1wMBOX1RemoteWakeCtrlParm, 0);
	}
#endif

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic,
							   wMBOX1_REMOTE_WAKE_CTRL,
							   wMBOX1_REMOTE_WAKE_CTRL_LEN,
							   u1wMBOX1RemoteWakeCtrlParm);
	return ret;
}

static u8 Func_Of_Proc_Chip_Hw_Set_Global_Info_Cmd(PNIC Nic, u8 group_alg,
									  u8 pairwise_alg)
{
	u8 ret = FALSE;
	u8 u1wMBOX1AOACGlobalInfoParm[wMBOX1_AOAC_GLOBAL_INFO_LEN] = { 0 };

	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s(): group_alg=%d pairwise_alg=%d\n",
			__func__, group_alg, pairwise_alg));

	SET_wMBOX1CMD_AOAC_GLOBAL_INFO_PAIRWISE_ENC_ALG(u1wMBOX1AOACGlobalInfoParm,
												 pairwise_alg);
	SET_wMBOX1CMD_AOAC_GLOBAL_INFO_GROUP_ENC_ALG(u1wMBOX1AOACGlobalInfoParm,
											  group_alg);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic,
							   wMBOX1_AOAC_GLOBAL_INFO,
							   wMBOX1_AOAC_GLOBAL_INFO_LEN,
							   u1wMBOX1AOACGlobalInfoParm);

	return ret;
}


void Func_Of_Proc_Chip_Hw_Set_Fw_Wow_Related_Cmd(PNIC Nic, u8 enable)
{
	struct security_priv *psecpriv = &Nic->securitypriv;
	struct pwrctrl_priv *ppwrpriv = &Nic->pwrctl_priv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;

	u16 media_status_rpt;
	u8 pkt_type = 0;
	u8 ret = TRUE;

	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("+%s()+: enable=%d\n", __func__, enable));

	_func_enter_;

	Func_Of_Proc_Chip_Hw_Set_Wowlan_Ctrl_Cmd(Nic, enable, FALSE);

	if (enable) {
		Func_Of_Proc_Chip_Hw_Set_Global_Info_Cmd(Nic,
									psecpriv->dot118021XGrpPrivacy,
									psecpriv->dot11PrivacyAlgrthm);

		if (!(ppwrpriv->wowlan_pno_enable)) {
			Func_Of_Proc_Chip_Hw_Set_Disconnect_Decision_Cmd(Nic, enable);
#ifdef CONFIG_ARP_KEEP_ALIVE
			if ((psecpriv->dot11PrivacyAlgrthm == _WEP40_) ||
				(psecpriv->dot11PrivacyAlgrthm == _WEP104_))
				pkt_type = 0;
			else
				pkt_type = 1;
#else
			pkt_type = 0;
#endif
			Func_Of_Proc_Chip_Hw_Set_Keep_Alive_Cmd(Nic, enable, pkt_type);
		}
		Func_Of_Proc_Chip_Hw_Set_Remote_Wake_Ctrl_Cmd(Nic, enable);
	} else {
		Func_Of_Proc_Chip_Hw_Set_Remote_Wake_Ctrl_Cmd(Nic, enable);
	}
	_func_exit_;
	MpTrace(COMP_RICHD, DBG_NORMAL, ("-%s()-\n", __func__));
}
#endif

#ifdef CONFIG_AP_WOWLAN
static u8 Func_Of_Proc_Chip_Hw_Set_Ap_Wowlan_Ctrl_Cmd(PNIC Nic, u8 enable)
{
	struct security_priv *psecpriv = &Nic->securitypriv;
	struct pwrctrl_priv *ppwrpriv = &Nic->pwrctl_priv;

	u8 u1wMBOX1APWoWlanCtrlParm[wMBOX1_AP_WOW_GPIO_CTRL_LEN] = { 0 };
	u8 gpionum = 0, gpio_dur = 0;
	u8 gpio_pulse = enable;
	u8 sdio_wakeup_enable = 1;
	u8 gpio_high_active = 0;
	u8 ret = FALSE;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s(): enable=%d\n", __func__, enable));

	SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_INDEX(u1wMBOX1APWoWlanCtrlParm, gpionum);
	SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_PLUS(u1wMBOX1APWoWlanCtrlParm, gpio_pulse);
	SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_HIGH_ACTIVE(u1wMBOX1APWoWlanCtrlParm,
											gpio_high_active);
	SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_EN(u1wMBOX1APWoWlanCtrlParm, enable);
	SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_DURATION(u1wMBOX1APWoWlanCtrlParm, gpio_dur);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic,
							   wMBOX1_AP_WOW_GPIO_CTRL,
							   wMBOX1_AP_WOW_GPIO_CTRL_LEN, u1wMBOX1APWoWlanCtrlParm);

	return ret;
}

static u8 Func_Of_Proc_Chip_Hw_Set_Ap_Offload_Ctrl_Cmd(PNIC Nic, u8 enable)
{
	u8 u1wMBOX1APOffloadCtrlParm[wMBOX1_WOWLAN_LEN] = { 0 };
	u8 ret = FALSE;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s(): bFuncEn=%d\n", __func__, enable));

	SET_wMBOX1CMD_AP_WOWLAN_EN(u1wMBOX1APOffloadCtrlParm, enable);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic,
							   wMBOX1_AP_OFFLOAD,
							   wMBOX1_AP_OFFLOAD_LEN, u1wMBOX1APOffloadCtrlParm);

	return ret;
}

static u8 Func_Of_Proc_Chip_Hw_Set_Ap_Ps_Cmd(PNIC Nic, u8 enable)
{
	u8 ap_ps_parm[wMBOX1_AP_PS_LEN] = { 0 };
	u8 ret = FALSE;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s(): enable=%d\n", __func__, enable));
	SET_wMBOX1CMD_AP_WOW_PS_EN(ap_ps_parm, enable);
	SET_wMBOX1CMD_AP_WOW_PS_RF(ap_ps_parm, enable);

	if (enable)
		SET_wMBOX1CMD_AP_WOW_PS_DURATION(ap_ps_parm, 0x32);
	else
		SET_wMBOX1CMD_AP_WOW_PS_DURATION(ap_ps_parm, 0x0);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic, wMBOX1_SAP_PS_, wMBOX1_AP_PS_LEN, ap_ps_parm);

	return ret;
}

static void Func_Of_Proc_Chip_Hw_Set_Ap_Rsvdpage_Loc_Cmd(PNIC Nic,
											PRSVDPAGE_LOC rsvdpageloc)
{
	u8 rsvdparm[wMBOX1_AOAC_RSVDPAGE_LOC_LEN] = { 0 };
	u8 ret = FALSE, header = 0;

	header = wl_read8(Nic, REG_BCNQ_BDNY);

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s: beacon: %d, probeRsp: %d, header:0x%02x\n", __func__,
			rsvdpageloc->LocApOffloadBCN, rsvdpageloc->LocProbeRsp, header));

	SET_wMBOX1CMD_AP_WOWLAN_RSVDPAGE_LOC_BCN(rsvdparm,
										  rsvdpageloc->LocApOffloadBCN +
										  header);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic, wMBOX1_BCN_RSVDPAGE,
							   wMBOX1_BCN_RSVDPAGE_LEN, rsvdparm);

	if (ret == FALSE)
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s: wMBOX1_BCN_RSVDPAGE cmd fail\n", __func__));

	Func_Of_Proc_Msleep_Os(10);

	memset(&rsvdparm, 0, sizeof(rsvdparm));

	SET_wMBOX1CMD_AP_WOWLAN_RSVDPAGE_LOC_ProbeRsp(rsvdparm,
											   rsvdpageloc->LocProbeRsp +
											   header);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic, wMBOX1_PROBERSP_RSVDPAGE,
							   wMBOX1_PROBERSP_RSVDPAGE_LEN, rsvdparm);

	if (ret == FALSE)
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s: wMBOX1_PROBERSP_RSVDPAGE cmd fail\n", __func__));

	Func_Of_Proc_Msleep_Os(10);
}

static void Func_Of_Proc_Chip_Hw_Set_Fw_Ap_Wow_Related_Cmd(PNIC Nic, u8 enable)
{
	Func_Of_Proc_Chip_Hw_Set_Ap_Offload_Ctrl_Cmd(Nic, enable);
	Func_Of_Proc_Chip_Hw_Set_Ap_Wowlan_Ctrl_Cmd(Nic, enable);
	Func_Of_Proc_Chip_Hw_Set_Ap_Ps_Cmd(Nic, enable);
}

static void Func_Of_Proc_Chip_Hw_Ap_Wow_Enable(PNIC Nic)
{
	struct pwrctrl_priv *pwrctl = &Nic->pwrctl_priv;
	struct security_priv *psecuritypriv = &Nic->securitypriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;

	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

	int res;
	u16 media_status_rpt;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s, WOWLAN_AP_ENABLE\n", __func__));

	Func_Of_Proc_Chip_Hw_Fw_Dl(Nic, TRUE);

	media_status_rpt = WP_MEDIA_CONNECT;
	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_wMBOX1_FW_JOINBSSRPT,
					  (u8 *) & media_status_rpt);

	beacon_to_issue_func(Nic, 0, 1);

	Func_Of_Proc_Msleep_Os(2);

	res = Func_Of_Proc_Chip_Hw_Pause_Rx_Dma(Nic);
	if (res == FALSE)
		MpTrace(COMP_RICHD, DBG_NORMAL, ("[WARNING] pause RX DMA fail\n"));

	MpTrace(COMP_RICHD, DBG_NORMAL, ("Set Enable AP WOWLan cmd\n"));

	Func_Of_Proc_Chip_Hw_Set_Fw_Ap_Wow_Related_Cmd(Nic, 1);

	wl_write8(Nic, REG_MCUTST_WOWLAN, 0);
	if (Nic->intf_stop)
		Nic->intf_stop(Nic);
#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic))
		Nic->pbuddy_wadptdata->intf_stop(Nic->pbuddy_wadptdata);
#endif
	wl_write8(Nic, REG_RSV_CTRL, 0x20);
}

static void Func_Of_Proc_Chip_Hw_Ap_Wow_Disable(PNIC Nic)
{
	struct pwrctrl_priv *pwrctl = wadptdata_to_pwrctl(Nic);

	u16 media_status_rpt;
	u8 val8;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s, WOWLAN_AP_DISABLE\n", __func__));
	
	pwrctl->wowlan_wake_reason = wl_read8(Nic, REG_MCUTST_WOWLAN);

	MpTrace(COMP_RICHD, DBG_NORMAL, ("wakeup_reason: 0x%02x\n", pwrctl->wowlan_wake_reason));

	Func_Of_Proc_Chip_Hw_Set_Fw_Ap_Wow_Related_Cmd(Nic, 0);

	Func_Of_Proc_Msleep_Os(2);

	Func_Of_Proc_Chip_Hw_Force_Enable_Rxdma(Nic);

	Func_Of_Proc_Chip_Hw_Fw_Dl(Nic, FALSE);

	media_status_rpt = WP_MEDIA_CONNECT;

	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_wMBOX1_FW_JOINBSSRPT,
					  (u8 *) & media_status_rpt);

	beacon_to_issue_func(Nic, 0);
}
#endif

#ifdef CONFIG_P2P_WOWLAN
static int proc_hidden_ssid_update_func(u8 * ies, u32 ies_len, u8 hidden_ssid_mode)
{
	u8 *ssid_ie;
	sint ssid_len_ori;
	int len_diff = 0;

	ssid_ie = ie_to_get_func(ies, WLAN_EID_SSID, &ssid_len_ori, ies_len);

	if (ssid_ie && ssid_len_ori > 0) {
		switch (hidden_ssid_mode) {
		case 1:
			{
				u8 *next_ie = ssid_ie + 2 + ssid_len_ori;
				u32 remain_len = 0;

				remain_len = ies_len - (next_ie - ies);

				ssid_ie[1] = 0;
				Func_Of_Proc_Pre_Memcpy(ssid_ie + 2, next_ie, remain_len);
				len_diff -= ssid_len_ori;

				break;
			}
		case 2:
			memset(&ssid_ie[2], 0, ssid_len_ori);
			break;
		default:
			break;
		}
	}

	return len_diff;
}

static void Func_Of_Proc_Chip_Hw_Construct_P2Pbeacon(PNIC Nic, u8 * pframe,
										u32 * pLength)
{
	struct wl_ieee80211_hdr *pwlanhdr;
	unsigned short *fctrl;
	unsigned int rate_len;

	u32 pktlen;
	struct mlme_priv *pmlmepriv = &(Nic->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
	u8 bc_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &(Nic->wdinfo);
#endif

	u8 *dbgbuf = pframe;
	u8 dbgbufLen = 0, index = 0;


	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s\n", __FUNCTION__));

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, bc_addr, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, query_bssid(cur_network), ETH_ALEN);

	SetSeqNum(pwlanhdr, 0);
	SetFrameSubType(pframe, WIFI_BEACON);

	pframe += sizeof(struct wl_ieee80211_hdr_3addr);
	pktlen = sizeof(struct wl_ieee80211_hdr_3addr);

	if ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) {
#ifdef CONFIG_P2P
		u32 wpsielen = 0, insert_len = 0;
		u8 *wpsie = NULL;
		wpsie =
			wps_ie_to_get_func(cur_network->IEs + _FIXED_IE_LENGTH_,
						   cur_network->IELength - _FIXED_IE_LENGTH_, NULL,
						   &wpsielen);

		if (wl_p2p_chk_role(pwdinfo, P2P_ROLE_GO) && wpsie && wpsielen > 0) {
			uint wps_offset, remainder_ielen;
			u8 *premainder_ie, *pframe_wscie;

			wps_offset = (uint) (wpsie - cur_network->IEs);

			premainder_ie = wpsie + wpsielen;

			remainder_ielen = cur_network->IELength - wps_offset - wpsielen;

			{
				pframe_wscie = pframe + wps_offset;
				Func_Of_Proc_Pre_Memcpy(pframe, cur_network->IEs, wps_offset + wpsielen);
				pframe += (wps_offset + wpsielen);
				pktlen += (wps_offset + wpsielen);

				*(u16 *) (pframe + insert_len) =
					cpu_to_be16(WPS_ATTR_PRIMARY_DEV_TYPE);
				insert_len += 2;

				*(u16 *) (pframe + insert_len) = cpu_to_be16(0x0008);
				insert_len += 2;

				*(u16 *) (pframe + insert_len) =
					cpu_to_be16(WPS_PDT_CID_MULIT_MEDIA);
				insert_len += 2;

				*(u32 *) (pframe + insert_len) = cpu_to_be32(WPSOUI);
				insert_len += 4;

				*(u16 *) (pframe + insert_len) =
					cpu_to_be16(WPS_PDT_SCID_MEDIA_SERVER);
				insert_len += 2;

				*(u16 *) (pframe + insert_len) =
					cpu_to_be16(WPS_ATTR_DEVICE_NAME);
				insert_len += 2;

				*(u16 *) (pframe + insert_len) =
					cpu_to_be16(pwdinfo->device_name_len);
				insert_len += 2;

				Func_Of_Proc_Pre_Memcpy(pframe + insert_len, pwdinfo->device_name,
							pwdinfo->device_name_len);
				insert_len += pwdinfo->device_name_len;

				*(pframe_wscie + 1) = (wpsielen - 2) + insert_len;

				pframe += insert_len;
				pktlen += insert_len;

				Func_Of_Proc_Pre_Memcpy(pframe, premainder_ie, remainder_ielen);
				pframe += remainder_ielen;
				pktlen += remainder_ielen;
			}
		} else
#endif
		{
			int len_diff;
			Func_Of_Proc_Pre_Memcpy(pframe, cur_network->IEs, cur_network->IELength);
			len_diff =
				proc_hidden_ssid_update_func(pframe + _BEACON_IE_OFFSET_,
								   cur_network->IELength - _BEACON_IE_OFFSET_,
								   pmlmeinfo->hidden_ssid_mode);
			pframe += (cur_network->IELength + len_diff);
			pktlen += (cur_network->IELength + len_diff);
		}
#ifdef CONFIG_P2P
		if (wl_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
			u32 len;

			{
				len = beacon_p2p_ie_build_func(pwdinfo, pframe);
			}

			pframe += len;
			pktlen += len;

		}
#endif

		goto _issue_bcn;

	}

	pframe += 8;
	pktlen += 8;

	Func_Of_Proc_Pre_Memcpy(pframe,
				(unsigned char
				 *)(do_query_data_from_ie(cur_network->IEs, BCN_INTERVAL)), 2);

	pframe += 2;
	pktlen += 2;

	Func_Of_Proc_Pre_Memcpy(pframe,
				(unsigned char
				 *)(do_query_data_from_ie(cur_network->IEs, CAPABILITY)), 2);

	pframe += 2;
	pktlen += 2;

	pframe =
		ie_to_set_func(pframe, _SSID_IE_, cur_network->Ssid.SsidLength,
				   cur_network->Ssid.Ssid, &pktlen);

	rate_len = rateset_len_to_get_func(cur_network->SupportedRates, 1);
	pframe =
		ie_to_set_func(pframe, _SUPPORTEDRATES_IE_, ((rate_len > 8) ? 8 : rate_len),
				   cur_network->SupportedRates, &pktlen);

	pframe =
		ie_to_set_func(pframe, _DSSET_IE_, 1,
				   (unsigned char *)&(cur_network->Configuration.DSConfig),
				   &pktlen);

	{
		u8 erpinfo = 0;
		u32 ATIMWindow;
		ATIMWindow = 0;
		pframe =
			ie_to_set_func(pframe, _IBSS_PARA_IE_, 2,
					   (unsigned char *)(&ATIMWindow), &pktlen);

		pframe = ie_to_set_func(pframe, _ERPINFO_IE_, 1, &erpinfo, &pktlen);
	}

	if (rate_len > 8) {
		pframe =
			ie_to_set_func(pframe, _EXT_SUPPORTEDRATES_IE_, (rate_len - 8),
					   (cur_network->SupportedRates + 8), &pktlen);
	}

_issue_bcn:

	*pLength = pktlen;
}

static int reg_classes_full_count_to_get_func(struct p2p_channels channel_list)
{
	int cnt = 0;
	int i;

	for (i = 0; i < channel_list.reg_classes; i++) {
		cnt += channel_list.reg_class[i].channels;
	}

	return cnt;
}

static void Func_Of_Proc_Chip_Hw_Construct_P2Pprobersp(PNIC Nic, u8 * pframe,
										  u32 * pLength)
{
	struct wl_ieee80211_hdr *pwlanhdr;
	unsigned short *fctrl;
	unsigned char *mac;

	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct mlme_priv *pmlmepriv = &(Nic->mlmepriv);
	u16 beacon_interval = 100;
	u16 capInfo = 0;
	struct wifidirect_info *pwdinfo = &(Nic->wdinfo);
	u8 wpsie[255] = { 0x00 };
	u32 wpsielen = 0, p2pielen = 0;
	u32 pktlen;

	u8 *dbgbuf = pframe;
	u8 dbgbufLen = 0, index = 0;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s\n", __FUNCTION__));
	
	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	mac = wadptdata_mac_addr(Nic);

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	memset(pwlanhdr->addr1, 0, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, mac, ETH_ALEN);

	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, mac, ETH_ALEN);

	SetSeqNum(pwlanhdr, 0);
	SetFrameSubType(fctrl, WIFI_PROBERSP);

	pktlen = sizeof(struct wl_ieee80211_hdr_3addr);
	pframe += pktlen;

	pframe += 8;
	pktlen += 8;

	Func_Of_Proc_Pre_Memcpy(pframe, (unsigned char *)&beacon_interval, 2);
	pframe += 2;
	pktlen += 2;

	capInfo |= cap_ShortPremble;
	capInfo |= cap_ShortSlot;

	Func_Of_Proc_Pre_Memcpy(pframe, (unsigned char *)&capInfo, 2);
	pframe += 2;
	pktlen += 2;

	pframe =
		ie_to_set_func(pframe, _SSID_IE_, 7, pwdinfo->p2p_wildcard_ssid, &pktlen);

	pframe =
		ie_to_set_func(pframe, _SUPPORTEDRATES_IE_, 8, pwdinfo->support_rate,
				   &pktlen);

	pframe =
		ie_to_set_func(pframe, _DSSET_IE_, 1,
				   (unsigned char *)&pwdinfo->listen_channel, &pktlen);

	{

		wpsielen = 0;
		*(u32 *) (wpsie) = cpu_to_be32(WPSOUI);
		wpsielen += 4;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_VER1);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0001);
		wpsielen += 2;

		wpsie[wpsielen++] = WPS_VERSION_1;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_SIMPLE_CONF_STATE);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0001);
		wpsielen += 2;

		wpsie[wpsielen++] = WPS_WSC_STATE_NOT_CONFIG;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_RESP_TYPE);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0001);
		wpsielen += 2;

		wpsie[wpsielen++] = WPS_RESPONSE_TYPE_8021X;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_UUID_E);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0010);
		wpsielen += 2;

		if (pwdinfo->external_uuid == 0) {
			memset(wpsie + wpsielen, 0x0, 16);
			Func_Of_Proc_Pre_Memcpy(wpsie + wpsielen, mac, ETH_ALEN);
		} else {
			Func_Of_Proc_Pre_Memcpy(wpsie + wpsielen, pwdinfo->uuid, 0x10);
		}
		wpsielen += 0x10;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_MANUFACTURER);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0007);
		wpsielen += 2;

		Func_Of_Proc_Pre_Memcpy(wpsie + wpsielen, "Wf_wlan", 7);
		wpsielen += 7;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_MODEL_NAME);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0006);
		wpsielen += 2;

		Func_Of_Proc_Pre_Memcpy(wpsie + wpsielen, "9192CU", 6);
		wpsielen += 6;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_MODEL_NUMBER);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0001);
		wpsielen += 2;

		wpsie[wpsielen++] = 0x31;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_SERIAL_NUMBER);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(ETH_ALEN);
		wpsielen += 2;

		Func_Of_Proc_Pre_Memcpy(wpsie + wpsielen, "123456", ETH_ALEN);
		wpsielen += ETH_ALEN;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_PRIMARY_DEV_TYPE);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0008);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_PDT_CID_MULIT_MEDIA);
		wpsielen += 2;

		*(u32 *) (wpsie + wpsielen) = cpu_to_be32(WPSOUI);
		wpsielen += 4;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_PDT_SCID_MEDIA_SERVER);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_DEVICE_NAME);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(pwdinfo->device_name_len);
		wpsielen += 2;

		Func_Of_Proc_Pre_Memcpy(wpsie + wpsielen, pwdinfo->device_name,
					pwdinfo->device_name_len);
		wpsielen += pwdinfo->device_name_len;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_CONF_METHOD);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0002);
		wpsielen += 2;

		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(pwdinfo->supported_wps_cm);
		wpsielen += 2;

		pframe =
			ie_to_set_func(pframe, _VENDOR_SPECIFIC_IE_, wpsielen,
					   (unsigned char *)wpsie, &pktlen);

		p2pielen = probe_resp_p2p_ie_build_func(pwdinfo, pframe);
		pframe += p2pielen;
		pktlen += p2pielen;
	}

	*pLength = pktlen;

}

static void Func_Of_Proc_Chip_Hw_Construct_P2Pnegorsp(PNIC Nic, u8 * pframe,
										 u32 * pLength)
{
	unsigned char category = WL_WLAN_CATEGORY_PUBLIC;
	u8 action = P2P_PUB_ACTION_ACTION;
	u32 p2poui = cpu_to_be32(P2POUI);
	u8 oui_subtype = P2P_GO_NEGO_RESP;
	u8 wpsie[255] = { 0x00 }, p2pie[255] = {
	0x00};
	u8 p2pielen = 0, i;
	uint wpsielen = 0;
	u16 wps_devicepassword_id = 0x0000;
	uint wps_devicepassword_id_len = 0;
	u8 channel_cnt_24g = 0, channel_cnt_5gl = 0, channel_cnt_5gh;
	u16 len_channellist_attr = 0;
	u32 pktlen;
	u8 dialogToken = 0;

	struct wl_ieee80211_hdr *pwlanhdr;
	unsigned short *fctrl;

	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct wifidirect_info *pwdinfo = &(Nic->wdinfo);


	u8 *dbgbuf = pframe;
	u8 dbgbufLen = 0, index = 0;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s\n", __FUNCTION__));

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	memset(pwlanhdr->addr1, 0, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, wadptdata_mac_addr(Nic), ETH_ALEN);

	SetSeqNum(pwlanhdr, 0);
	SetFrameSubType(pframe, WIFI_ACTION);

	pktlen = sizeof(struct wl_ieee80211_hdr_3addr);
	pframe += pktlen;

	pframe = fixed_ie_to_set_func(pframe, 1, &(category), &(pktlen));
	pframe = fixed_ie_to_set_func(pframe, 1, &(action), &(pktlen));
	pframe = fixed_ie_to_set_func(pframe, 4, (unsigned char *)&(p2poui), &(pktlen));
	pframe = fixed_ie_to_set_func(pframe, 1, &(oui_subtype), &(pktlen));

	pframe = fixed_ie_to_set_func(pframe, 1, &(dialogToken), &(pktlen));

	memset(wpsie, 0x00, 255);
	wpsielen = 0;

	wpsielen = 0;
	*(u32 *) (wpsie) = cpu_to_be32(WPSOUI);
	wpsielen += 4;

	*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_VER1);
	wpsielen += 2;

	*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0001);
	wpsielen += 2;

	wpsie[wpsielen++] = WPS_VERSION_1;

	*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_ATTR_DEVICE_PWID);
	wpsielen += 2;

	*(u16 *) (wpsie + wpsielen) = cpu_to_be16(0x0002);
	wpsielen += 2;

	if (wps_devicepassword_id == WPS_DPID_USER_SPEC) {
		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_DPID_REGISTRAR_SPEC);
	} else if (wps_devicepassword_id == WPS_DPID_REGISTRAR_SPEC) {
		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_DPID_USER_SPEC);
	} else {
		*(u16 *) (wpsie + wpsielen) = cpu_to_be16(WPS_DPID_PBC);
	}
	wpsielen += 2;

	pframe =
		ie_to_set_func(pframe, _VENDOR_SPECIFIC_IE_, wpsielen,
				   (unsigned char *)wpsie, &pktlen);

	p2pielen = 0;
	p2pie[p2pielen++] = 0x50;
	p2pie[p2pielen++] = 0x6F;
	p2pie[p2pielen++] = 0x9A;
	p2pie[p2pielen++] = 0x09;

	p2pie[p2pielen++] = P2P_ATTR_STATUS;

	*(u16 *) (p2pie + p2pielen) = cpu_to_le16(0x0001);
	p2pielen += 2;

	p2pie[p2pielen++] = 1;

	p2pie[p2pielen++] = P2P_ATTR_CAPABILITY;

	*(u16 *) (p2pie + p2pielen) = cpu_to_le16(0x0002);
	p2pielen += 2;

	if (wl_p2p_chk_role(pwdinfo, P2P_ROLE_CLIENT)) {
		p2pie[p2pielen++] = 0;
	} else {
		p2pie[p2pielen++] = DMP_P2P_DEVCAP_SUPPORT;
	}

	if (pwdinfo->persistent_supported) {
		p2pie[p2pielen++] = P2P_GRPCAP_CROSS_CONN | P2P_GRPCAP_PERSISTENT_GROUP;
	} else {
		p2pie[p2pielen++] = P2P_GRPCAP_CROSS_CONN;
	}

	p2pie[p2pielen++] = P2P_ATTR_GO_INTENT;

	*(u16 *) (p2pie + p2pielen) = cpu_to_le16(0x0001);
	p2pielen += 2;

	if (pwdinfo->peer_intent & 0x01) {
		p2pie[p2pielen++] = (pwdinfo->intent << 1);
	} else {
		p2pie[p2pielen++] = ((pwdinfo->intent << 1) | BIT(0));
	}

	p2pie[p2pielen++] = P2P_ATTR_CONF_TIMEOUT;

	*(u16 *) (p2pie + p2pielen) = cpu_to_le16(0x0002);
	p2pielen += 2;

	p2pie[p2pielen++] = 200;
	p2pie[p2pielen++] = 200;

	p2pie[p2pielen++] = P2P_ATTR_OPERATING_CH;

	*(u16 *) (p2pie + p2pielen) = cpu_to_le16(0x0005);
	p2pielen += 2;

	p2pie[p2pielen++] = 'X';
	p2pie[p2pielen++] = 'X';

	p2pie[p2pielen++] = 0x04;

	if (pwdinfo->operating_channel <= 14) {
		p2pie[p2pielen++] = 0x51;
	}

	p2pie[p2pielen++] = pwdinfo->operating_channel;

	p2pie[p2pielen++] = P2P_ATTR_INTENTED_IF_ADDR;

	*(u16 *) (p2pie + p2pielen) = cpu_to_le16(ETH_ALEN);
	p2pielen += 2;

	Func_Of_Proc_Pre_Memcpy(p2pie + p2pielen, wadptdata_mac_addr(Nic), ETH_ALEN);
	p2pielen += ETH_ALEN;

	p2pie[p2pielen++] = P2P_ATTR_CH_LIST;

	len_channellist_attr = 3
		+ (1 + 1) * (u16) pmlmeext->channel_list.reg_classes
		+ reg_classes_full_count_to_get_func(pmlmeext->channel_list);

#ifdef CONFIG_CONCURRENT_MODE
	if (do_chk_partner_fwstate(Nic, _FW_LINKED)) {
		*(u16 *) (p2pie + p2pielen) = cpu_to_le16(5 + 1);
	} else {
		*(u16 *) (p2pie + p2pielen) = cpu_to_le16(len_channellist_attr);
	}
#else

	*(u16 *) (p2pie + p2pielen) = cpu_to_le16(len_channellist_attr);

#endif
	p2pielen += 2;

	p2pie[p2pielen++] = 'X';
	p2pie[p2pielen++] = 'X';

	p2pie[p2pielen++] = 0x04;

#ifdef CONFIG_CONCURRENT_MODE
	if (do_chk_partner_fwstate(Nic, _FW_LINKED)) {
		_wadptdata *pbuddy_wadptdata = Nic->pbuddy_wadptdata;
		struct mlme_ext_priv *pbuddy_mlmeext = &pbuddy_wadptdata->mlmeextpriv;

		if (pbuddy_mlmeext->cur_channel > 14) {

		} else {
			p2pie[p2pielen++] = 0x51;
		}

		p2pie[p2pielen++] = 1;

		p2pie[p2pielen++] = pbuddy_mlmeext->cur_channel;
	} else {
		int i, j;
		for (j = 0; j < pmlmeext->channel_list.reg_classes; j++) {
			p2pie[p2pielen++] = pmlmeext->channel_list.reg_class[j].reg_class;

			p2pie[p2pielen++] = pmlmeext->channel_list.reg_class[j].channels;

			for (i = 0; i < pmlmeext->channel_list.reg_class[j].channels; i++) {
				p2pie[p2pielen++] =
					pmlmeext->channel_list.reg_class[j].channel[i];
			}
		}
	}
#else
	{
		int i, j;
		for (j = 0; j < pmlmeext->channel_list.reg_classes; j++) {
			p2pie[p2pielen++] = pmlmeext->channel_list.reg_class[j].reg_class;

			p2pie[p2pielen++] = pmlmeext->channel_list.reg_class[j].channels;

			for (i = 0; i < pmlmeext->channel_list.reg_class[j].channels; i++) {
				p2pie[p2pielen++] =
					pmlmeext->channel_list.reg_class[j].channel[i];
			}
		}
	}
#endif

	p2pie[p2pielen++] = P2P_ATTR_DEVICE_INFO;

	*(u16 *) (p2pie + p2pielen) = cpu_to_le16(21 + pwdinfo->device_name_len);
	p2pielen += 2;

	Func_Of_Proc_Pre_Memcpy(p2pie + p2pielen, wadptdata_mac_addr(Nic), ETH_ALEN);
	p2pielen += ETH_ALEN;

	*(u16 *) (p2pie + p2pielen) = cpu_to_be16(pwdinfo->supported_wps_cm);

	p2pielen += 2;

	*(u16 *) (p2pie + p2pielen) = cpu_to_be16(WPS_PDT_CID_MULIT_MEDIA);
	p2pielen += 2;

	*(u32 *) (p2pie + p2pielen) = cpu_to_be32(WPSOUI);
	p2pielen += 4;

	*(u16 *) (p2pie + p2pielen) = cpu_to_be16(WPS_PDT_SCID_MEDIA_SERVER);
	p2pielen += 2;

	p2pie[p2pielen++] = 0x00;

	*(u16 *) (p2pie + p2pielen) = cpu_to_be16(WPS_ATTR_DEVICE_NAME);
	p2pielen += 2;

	*(u16 *) (p2pie + p2pielen) = cpu_to_be16(pwdinfo->device_name_len);
	p2pielen += 2;

	Func_Of_Proc_Pre_Memcpy(p2pie + p2pielen, pwdinfo->device_name,
				pwdinfo->device_name_len);
	p2pielen += pwdinfo->device_name_len;

	if (wl_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
		p2pie[p2pielen++] = P2P_ATTR_GROUP_ID;

		*(u16 *) (p2pie + p2pielen) =
			cpu_to_le16(ETH_ALEN + pwdinfo->nego_ssidlen);
		p2pielen += 2;

		Func_Of_Proc_Pre_Memcpy(p2pie + p2pielen, pwdinfo->device_addr, ETH_ALEN);
		p2pielen += ETH_ALEN;

		Func_Of_Proc_Pre_Memcpy(p2pie + p2pielen, pwdinfo->nego_ssid,
					pwdinfo->nego_ssidlen);
		p2pielen += pwdinfo->nego_ssidlen;

	}

	pframe =
		ie_to_set_func(pframe, _VENDOR_SPECIFIC_IE_, p2pielen,
				   (unsigned char *)p2pie, &pktlen);


	*pLength = pktlen;
}

static void Func_Of_Proc_Chip_Hw_Construct_P2Pinvitersp(PNIC Nic, u8 * pframe,
										   u32 * pLength)
{
	unsigned char category = WL_WLAN_CATEGORY_PUBLIC;
	u8 action = P2P_PUB_ACTION_ACTION;
	u32 p2poui = cpu_to_be32(P2POUI);
	u8 oui_subtype = P2P_INVIT_RESP;
	u8 p2pie[255] = { 0x00 };
	u8 p2pielen = 0, i;
	u8 channel_cnt_24g = 0, channel_cnt_5gl = 0, channel_cnt_5gh = 0;
	u16 len_channellist_attr = 0;
	u32 pktlen;
	u8 dialogToken = 0;
#ifdef CONFIG_CONCURRENT_MODE
	_wadptdata *pbuddy_wadptdata = Nic->pbuddy_wadptdata;
	struct wifidirect_info *pbuddy_wdinfo = &pbuddy_wadptdata->wdinfo;
	struct mlme_priv *pbuddy_mlmepriv = &pbuddy_wadptdata->mlmepriv;
	struct mlme_ext_priv *pbuddy_mlmeext = &pbuddy_wadptdata->mlmeextpriv;
#endif

	struct wl_ieee80211_hdr *pwlanhdr;
	unsigned short *fctrl;

	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct wifidirect_info *pwdinfo = &(Nic->wdinfo);

	u8 *dbgbuf = pframe;
	u8 dbgbufLen = 0, index = 0;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s\n", __FUNCTION__));

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	memset(pwlanhdr->addr1, 0, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);

	memset(pwlanhdr->addr3, 0, ETH_ALEN);

	SetSeqNum(pwlanhdr, 0);
	SetFrameSubType(pframe, WIFI_ACTION);

	pframe += sizeof(struct wl_ieee80211_hdr_3addr);
	pktlen = sizeof(struct wl_ieee80211_hdr_3addr);

	pframe = fixed_ie_to_set_func(pframe, 1, &(category), &(pktlen));
	pframe = fixed_ie_to_set_func(pframe, 1, &(action), &(pktlen));
	pframe = fixed_ie_to_set_func(pframe, 4, (unsigned char *)&(p2poui), &(pktlen));
	pframe = fixed_ie_to_set_func(pframe, 1, &(oui_subtype), &(pktlen));

	pframe = fixed_ie_to_set_func(pframe, 1, &(dialogToken), &(pktlen));

	p2pielen = 0;
	p2pie[p2pielen++] = 0x50;
	p2pie[p2pielen++] = 0x6F;
	p2pie[p2pielen++] = 0x9A;
	p2pie[p2pielen++] = 0x09;

	p2pie[p2pielen++] = P2P_ATTR_STATUS;

	*(u16 *) (p2pie + p2pielen) = cpu_to_le16(0x0001);
	p2pielen += 2;

	p2pie[p2pielen++] = P2P_STATUS_FAIL_INFO_UNAVAILABLE;

	p2pie[p2pielen++] = P2P_ATTR_CONF_TIMEOUT;

	*(u16 *) (p2pie + p2pielen) = cpu_to_le16(0x0002);
	p2pielen += 2;

	p2pie[p2pielen++] = 200;
	p2pie[p2pielen++] = 200;

	pframe =
		ie_to_set_func(pframe, _VENDOR_SPECIFIC_IE_, p2pielen,
				   (unsigned char *)p2pie, &pktlen);


	*pLength = pktlen;

}

static void Func_Of_Proc_Chip_Hw_Construct_P2Pprovisiondisrsp(PNIC Nic,
												 u8 * pframe, u32 * pLength)
{
	unsigned char category = WL_WLAN_CATEGORY_PUBLIC;
	u8 action = P2P_PUB_ACTION_ACTION;
	u8 dialogToken = 0;
	u32 p2poui = cpu_to_be32(P2POUI);
	u8 oui_subtype = P2P_PROVISION_DISC_RESP;
	u8 wpsie[100] = { 0x00 };
	u8 wpsielen = 0;
	u32 pktlen;

	struct wl_ieee80211_hdr *pwlanhdr;
	unsigned short *fctrl;

	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct wifidirect_info *pwdinfo = &(Nic->wdinfo);

	u8 *dbgbuf = pframe;
	u8 dbgbufLen = 0, index = 0;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s\n", __FUNCTION__));

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	memset(pwlanhdr->addr1, 0, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, wadptdata_mac_addr(Nic), ETH_ALEN);

	SetSeqNum(pwlanhdr, 0);
	SetFrameSubType(pframe, WIFI_ACTION);

	pframe += sizeof(struct wl_ieee80211_hdr_3addr);
	pktlen = sizeof(struct wl_ieee80211_hdr_3addr);

	pframe = fixed_ie_to_set_func(pframe, 1, &(category), &(pktlen));
	pframe = fixed_ie_to_set_func(pframe, 1, &(action), &(pktlen));
	pframe = fixed_ie_to_set_func(pframe, 4, (unsigned char *)&(p2poui), &(pktlen));
	pframe = fixed_ie_to_set_func(pframe, 1, &(oui_subtype), &(pktlen));
	pframe = fixed_ie_to_set_func(pframe, 1, &(dialogToken), &(pktlen));

	wpsielen = 0;
	WL_PUT_BE32(wpsie, WPSOUI);
	wpsielen += 4;
	WL_PUT_BE16(wpsie + wpsielen, WPS_ATTR_CONF_METHOD);
	wpsielen += 2;

	WL_PUT_BE16(wpsie + wpsielen, 0x0002);
	wpsielen += 2;

	WL_PUT_BE16(wpsie + wpsielen, WPS_CM_PUSH_BUTTON);
	wpsielen += 2;

	pframe =
		ie_to_set_func(pframe, _VENDOR_SPECIFIC_IE_, wpsielen,
				   (unsigned char *)wpsie, &pktlen);


	*pLength = pktlen;

}

u8 Func_Of_Proc_Chip_Hw_Set_Fwp2Prsvdpage_Cmd(PNIC Nic, PRSVDPAGE_LOC rsvdpageloc)
{
	u8 u1wMBOX1P2PRsvdPageParm[wMBOX1_P2PRSVDPAGE_LOC_LEN] = { 0 };
	u8 ret = FALSE;

	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("P2PRsvdPageLoc: P2PBeacon=%d P2PProbeRsp=%d NegoRsp=%d InviteRsp=%d PDRsp=%d\n",
		 rsvdpageloc->LocP2PBeacon, rsvdpageloc->LocP2PProbeRsp,
		 rsvdpageloc->LocNegoRsp, rsvdpageloc->LocInviteRsp,
		 rsvdpageloc->LocPDRsp));
	
	SET_wMBOX1CMD_RSVDPAGE_LOC_P2P_BCN(u1wMBOX1P2PRsvdPageParm,
									rsvdpageloc->LocProbeRsp);
	SET_wMBOX1CMD_RSVDPAGE_LOC_P2P_PROBE_RSP(u1wMBOX1P2PRsvdPageParm,
										  rsvdpageloc->LocPsPoll);
	SET_wMBOX1CMD_RSVDPAGE_LOC_P2P_NEGO_RSP(u1wMBOX1P2PRsvdPageParm,
										 rsvdpageloc->LocNullData);
	SET_wMBOX1CMD_RSVDPAGE_LOC_P2P_INVITE_RSP(u1wMBOX1P2PRsvdPageParm,
										   rsvdpageloc->LocQosNull);
	SET_wMBOX1CMD_RSVDPAGE_LOC_P2P_PD_RSP(u1wMBOX1P2PRsvdPageParm,
									   rsvdpageloc->LocBTQosNull);

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic,
							   wMBOX1_P2P_OFFLOAD_RSVD_PAGE,
							   wMBOX1_P2PRSVDPAGE_LOC_LEN, u1wMBOX1P2PRsvdPageParm);

	return ret;
}

u8 Func_Of_Proc_Chip_Hw_Set_P2P_Wowlan_Offload_Cmd(PNIC Nic)
{

	u8 offload_cmd[wMBOX1_P2P_OFFLOAD_LEN] = { 0 };
	struct wifidirect_info *pwdinfo = &(Nic->wdinfo);
	struct P2P_WoWlan_Offload_t *p2p_wowlan_offload =
		(struct P2P_WoWlan_Offload_t *)offload_cmd;
	u8 ret = FALSE;

	memset(p2p_wowlan_offload, 0, sizeof(struct P2P_WoWlan_Offload_t));
	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s\n", __func__));
	switch (pwdinfo->role) {
	case P2P_ROLE_DEVICE:
		MpTrace(COMP_RICHD, DBG_NORMAL, ("P2P_ROLE_DEVICE\n"));
		p2p_wowlan_offload->role = 0;
		break;
	case P2P_ROLE_CLIENT:
		MpTrace(COMP_RICHD, DBG_NORMAL, ("P2P_ROLE_CLIENT\n"));
		p2p_wowlan_offload->role = 1;
		break;
	case P2P_ROLE_GO:
		MpTrace(COMP_RICHD, DBG_NORMAL, ("P2P_ROLE_GO\n"));
		p2p_wowlan_offload->role = 2;
		break;
	default:
		MpTrace(COMP_RICHD, DBG_NORMAL, ("P2P_ROLE_DISABLE\n"));
		break;
	}
	p2p_wowlan_offload->Wps_Config[0] = pwdinfo->supported_wps_cm >> 8;
	p2p_wowlan_offload->Wps_Config[1] = pwdinfo->supported_wps_cm;
	offload_cmd = (u8 *) p2p_wowlan_offload;
	MpTrace(COMP_RICHD, DBG_NORMAL, ("p2p_wowlan_offload: %x:%x:%x\n", offload_cmd[0], offload_cmd[1],
			offload_cmd[2]));

	ret = Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(Nic,
							   wMBOX1_P2P_OFFLOAD,
							   wMBOX1_P2P_OFFLOAD_LEN, offload_cmd);
	return ret;

}
#endif

static void Func_Of_Proc_Chip_Hw_Construct_Beacon(PNIC Nic,
									 u8 * pframe, u32 * pLength)
{
	struct wl_ieee80211_hdr *pwlanhdr;
	u16 *fctrl;
	u32 rate_len, pktlen;
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
	u8 bc_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, bc_addr, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, query_bssid(cur_network), ETH_ALEN);

	SetSeqNum(pwlanhdr, 0);
	SetFrameSubType(pframe, WIFI_BEACON);

	pframe += sizeof(struct wl_ieee80211_hdr_3addr);
	pktlen = sizeof(struct wl_ieee80211_hdr_3addr);

	pframe += 8;
	pktlen += 8;

	Func_Of_Proc_Pre_Memcpy(pframe,
				(unsigned char
				 *)(do_query_data_from_ie(cur_network->IEs, BCN_INTERVAL)), 2);

	pframe += 2;
	pktlen += 2;

	Func_Of_Proc_Pre_Memcpy(pframe,
				(unsigned char
				 *)(do_query_data_from_ie(cur_network->IEs, CAPABILITY)), 2);

	pframe += 2;
	pktlen += 2;

	if ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) {
		pktlen += cur_network->IELength - sizeof(NDIS_802_11_FIXED_IEs);
		Func_Of_Proc_Pre_Memcpy(pframe, cur_network->IEs + sizeof(NDIS_802_11_FIXED_IEs),
					pktlen);

		goto _ConstructBeacon;
	}

	pframe =
		ie_to_set_func(pframe, _SSID_IE_, cur_network->Ssid.SsidLength,
				   cur_network->Ssid.Ssid, &pktlen);

	rate_len = rateset_len_to_get_func(cur_network->SupportedRates, 1);
	pframe =
		ie_to_set_func(pframe, _SUPPORTEDRATES_IE_, ((rate_len > 8) ? 8 : rate_len),
				   cur_network->SupportedRates, &pktlen);

	pframe =
		ie_to_set_func(pframe, _DSSET_IE_, 1,
				   (unsigned char *)&(cur_network->Configuration.DSConfig),
				   &pktlen);

	if ((pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) {
		u32 ATIMWindow;
		ATIMWindow = 0;
		pframe =
			ie_to_set_func(pframe, _IBSS_PARA_IE_, 2,
					   (unsigned char *)(&ATIMWindow), &pktlen);
	}

	if (rate_len > 8) {
		pframe =
			ie_to_set_func(pframe, _EXT_SUPPORTEDRATES_IE_, (rate_len - 8),
					   (cur_network->SupportedRates + 8), &pktlen);
	}

_ConstructBeacon:

	if ((pktlen + TXDESC_SIZE) > 512) {
		MpTrace(COMP_RICHD, DBG_NORMAL, ("beacon frame too large\n"));
		return;
	}

	*pLength = pktlen;

}

static void Func_Of_Proc_Chip_Hw_Construct_Pspoll(PNIC Nic,
									 u8 * pframe, u32 * pLength)
{
	struct wl_ieee80211_hdr *pwlanhdr;
	u16 *fctrl;
	u32 pktlen;
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;
	SetPwrMgt(fctrl);
	SetFrameSubType(pframe, WIFI_PSPOLL);

	SetDuration(pframe, (pmlmeinfo->aid | 0xc000));

	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, query_bssid(&(pmlmeinfo->network)), ETH_ALEN);

	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);

	*pLength = 16;
}

static void Func_Of_Proc_Chip_Hw_Construct_Nullfunctiondata(PNIC Nic,
											   u8 * pframe,
											   u32 * pLength,
											   u8 * StaAddr,
											   u8 bQoS,
											   u8 AC,
											   u8 bEosp, u8 bForcePowerSave)
{
	struct wl_ieee80211_hdr *pwlanhdr;
	u16 *fctrl;
	u32 pktlen;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct wlan_network *cur_network = &pmlmepriv->cur_network;
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &pwlanhdr->frame_ctl;
	*(fctrl) = 0;
	if (bForcePowerSave) {
		SetPwrMgt(fctrl);
	}

	switch (cur_network->network.InfrastructureMode) {
	case Ndis802_11Infrastructure:
		SetToDs(fctrl);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, query_bssid(&(pmlmeinfo->network)),
					ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, StaAddr, ETH_ALEN);
		break;
	case Ndis802_11APMode:
		SetFrDs(fctrl);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, StaAddr, ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, query_bssid(&(pmlmeinfo->network)),
					ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, wadptdata_mac_addr(Nic), ETH_ALEN);
		break;
	case Ndis802_11IBSS:
	default:
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, StaAddr, ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, query_bssid(&(pmlmeinfo->network)),
					ETH_ALEN);
		break;
	}

	SetSeqNum(pwlanhdr, 0);

	if (bQoS == TRUE) {
		struct wl_ieee80211_hdr_3addr_qos *pwlanqoshdr;

		SetFrameSubType(pframe, WIFI_QOS_DATA_NULL);

		pwlanqoshdr = (struct wl_ieee80211_hdr_3addr_qos *)pframe;
		SetPriority(&pwlanqoshdr->qc, AC);
		SetEOSP(&pwlanqoshdr->qc, bEosp);

		pktlen = sizeof(struct wl_ieee80211_hdr_3addr_qos);
	} else {
		SetFrameSubType(pframe, WIFI_DATA_NULL);

		pktlen = sizeof(struct wl_ieee80211_hdr_3addr);
	}

	*pLength = pktlen;
}

void Func_Of_Proc_Chip_Hw_Construct_Probersp(PNIC Nic, u8 * pframe, u32 * pLength,
								u8 * StaAddr, BOOLEAN bHideSSID)
{
	struct wl_ieee80211_hdr *pwlanhdr;
	u16 *fctrl;
	u8 *mac, *bssid;
	u32 pktlen;
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	mac = wadptdata_mac_addr(Nic);
	bssid = cur_network->MacAddress;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, StaAddr, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, mac, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, bssid, ETH_ALEN);

	SetSeqNum(pwlanhdr, 0);
	SetFrameSubType(fctrl, WIFI_PROBERSP);

	pktlen = sizeof(struct wl_ieee80211_hdr_3addr);
	pframe += pktlen;

	if (cur_network->IELength > MAX_IE_SZ)
		return;

	Func_Of_Proc_Pre_Memcpy(pframe, cur_network->IEs, cur_network->IELength);
	pframe += cur_network->IELength;
	pktlen += cur_network->IELength;

	*pLength = pktlen;
}

#ifdef CONFIG_WOWLAN
static void Func_Of_Proc_Chip_Hw_Construct_Arprsp(PNIC Nic,
									 u8 * pframe,
									 u32 * pLength, u8 * pIPAddress)
{
	struct wl_ieee80211_hdr *pwlanhdr;
	u16 *fctrl;
	u32 pktlen;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct wlan_network *cur_network = &pmlmepriv->cur_network;
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct security_priv *psecuritypriv = &Nic->securitypriv;
	static u8 ARPLLCHeader[8] =
		{ 0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x08, 0x06 };
	u8 *pARPRspPkt = pframe;
	u8 *payload = pframe;
	u8 EncryptionHeadOverhead = 0;

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &pwlanhdr->frame_ctl;
	*(fctrl) = 0;

	SetFrameType(fctrl, WIFI_DATA);
	SetToDs(fctrl);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, query_bssid(&(pmlmeinfo->network)), ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, query_bssid(&(pmlmeinfo->network)), ETH_ALEN);

	SetSeqNum(pwlanhdr, 0);
	SetDuration(pwlanhdr, 0);

	*pLength = 24;

	switch (psecuritypriv->dot11PrivacyAlgrthm) {
	case _WEP40_:
	case _WEP104_:
		EncryptionHeadOverhead = 4;
		break;
	case _TKIP_:
		EncryptionHeadOverhead = 8;
		break;
	case _AES_:
		EncryptionHeadOverhead = 8;
		break;
	default:
		EncryptionHeadOverhead = 0;
	}

	if (EncryptionHeadOverhead > 0) {
		memset(&(pframe[*pLength]), 0, EncryptionHeadOverhead);
		*pLength += EncryptionHeadOverhead;
		SetPrivacy(fctrl);
	}

	pARPRspPkt = (u8 *) (pframe + *pLength);
	payload = pARPRspPkt;
	Func_Of_Proc_Pre_Memcpy(pARPRspPkt, ARPLLCHeader, 8);
	*pLength += 8;

	pARPRspPkt += 8;
	SET_ARP_PKT_HW(pARPRspPkt, 0x0100);
	SET_ARP_PKT_PROTOCOL(pARPRspPkt, 0x0008);
	SET_ARP_PKT_HW_ADDR_LEN(pARPRspPkt, 6);
	SET_ARP_PKT_PROTOCOL_ADDR_LEN(pARPRspPkt, 4);
	SET_ARP_PKT_OPERATION(pARPRspPkt, 0x0200);
	SET_ARP_PKT_SENDER_MAC_ADDR(pARPRspPkt, wadptdata_mac_addr(Nic));
	SET_ARP_PKT_SENDER_IP_ADDR(pARPRspPkt, pIPAddress);
#ifdef CONFIG_ARP_KEEP_ALIVE
	if (!is_zero_mac_addr(pmlmepriv->gw_mac_addr)) {
		SET_ARP_PKT_TARGET_MAC_ADDR(pARPRspPkt, pmlmepriv->gw_mac_addr);
		SET_ARP_PKT_TARGET_IP_ADDR(pARPRspPkt, pmlmepriv->gw_ip);
	} else
#endif
	{
		SET_ARP_PKT_TARGET_MAC_ADDR(pARPRspPkt,
									query_bssid(&(pmlmeinfo->network)));
		SET_ARP_PKT_TARGET_IP_ADDR(pARPRspPkt, pIPAddress);
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s Target Mac Addr:" MAC_FMT "\n", __FUNCTION__,
				MAC_ARG(query_bssid(&(pmlmeinfo->network)))));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s Target IP Addr" IP_FMT "\n", __FUNCTION__,
				IP_ARG(pIPAddress)));
	}

	*pLength += 28;

	if (psecuritypriv->dot11PrivacyAlgrthm == _TKIP_) {
		u8 mic[8];
		struct mic_data micdata;
		struct sta_info *psta = NULL;
		u8 priority[4] = { 0x0, 0x0, 0x0, 0x0 };
		u8 null_key[16] =
			{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0 };

		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s(): Add MIC\n", __FUNCTION__));

		psta = do_query_stainfo(&Nic->stapriv,
							   query_bssid(&(pmlmeinfo->network)), 1);
		if (psta != NULL) {
			if (Func_Of_Proc_Pre_Memcmp(&psta->dot11tkiptxmickey.skey[0],
							null_key, 16) == TRUE) {
				MpTrace(COMP_RICHD, DBG_NORMAL, 
					("%s(): STA dot11tkiptxmickey==0\n", __func__));
			}
			sl_secmicsetkey(&micdata, &psta->dot11tkiptxmickey.skey[0]);
		}

		sl_secmicappend(&micdata, pwlanhdr->addr3, 6);

		sl_secmicappend(&micdata, pwlanhdr->addr2, 6);

		priority[0] = 0;

		sl_secmicappend(&micdata, &priority[0], 4);

		sl_secmicappend(&micdata, payload, 36);

		sl_secgetmic(&micdata, &(mic[0]), 1);

		pARPRspPkt += 28;
		Func_Of_Proc_Pre_Memcpy(pARPRspPkt, &(mic[0]), 8);

		*pLength += 8;
	}
}


#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
static void Func_Of_Proc_Get_Current_Ip_Address(PNIC Nic, u8 * pcurrentip)
{
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct in_device *my_ip_ptr = Nic->pnetdev->ip_ptr;
	u8 ipaddress[4];

	if ((pmlmeinfo->state & WIFI_FW_LINKING_STATE) ||
		pmlmeinfo->state & WIFI_FW_AP_STATE) {
		if (my_ip_ptr != NULL) {
			struct in_ifaddr *my_ifa_list = my_ip_ptr->ifa_list;
			if (my_ifa_list != NULL) {
				ipaddress[0] = my_ifa_list->ifa_address & 0xFF;
				ipaddress[1] = (my_ifa_list->ifa_address >> 8) & 0xFF;
				ipaddress[2] = (my_ifa_list->ifa_address >> 16) & 0xFF;
				ipaddress[3] = my_ifa_list->ifa_address >> 24;
				MpTrace(COMP_RICHD, DBG_NORMAL, 
					("%s: %d.%d.%d.%d ==========\n", __func__,
						ipaddress[0], ipaddress[1], ipaddress[2], ipaddress[3]));
				Func_Of_Proc_Pre_Memcpy(pcurrentip, ipaddress, 4);
			}
		}
	}
}
#endif

static void Func_Of_Proc_Chip_Hw_Gate_Bb(PNIC Nic, bool stop)
{
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;
	u8 val8 = 0;
	u16 val16 = 0;
	int ret = FALSE;
	u32 m_stop = stop;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_GATE_BB, &m_stop, NULL,
									1, 0);
	if (!ret) {
		return;
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("====>%s,fail\n", __func__));
	}
}

static void Func_Of_Proc_Chip_Hw_Set_Wow_Rxff_Boundary(PNIC Nic, bool wow_mode)
{
	u8 val8 = 0;
	u16 rxff_bndy = 0;
	u32 rx_dma_buff_sz_wow = 0;
	u32 rx_dma_buff_sz = 0;
	u32 mbox[2] = { 0 };
	int ret = FALSE;
	if (wow_mode) {
		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_RX_DMA_SZ_WOW,
							(u8 *) & rx_dma_buff_sz);
	} else {
		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_RX_DMA_SZ,
							(u8 *) & rx_dma_buff_sz);
	}
	mbox[0] = wow_mode;
	mbox[1] = rx_dma_buff_sz;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_SET_RXFF_BOUNDARY, mbox,
									NULL, 2, 0);
	if (!ret) {
		WL_INFO("====>%s,fail\n", __func__);
	}

}

static int Func_Of_Proc_Chip_Hw_Set_Pattern(PNIC Nic, u8 * pattern,
							   u8 len, u8 * mask)
{
	struct pwrctrl_priv *pwrctl = &Nic->pwrctl_priv;
	struct mlme_ext_priv *pmlmeext = NULL;
	struct mlme_ext_info *pmlmeinfo = NULL;
	struct wltwow_pattern wow_pattern;
	u8 mask_hw[MAX_WKFM_SIZE] = { 0 };
	u8 content[MAX_WKFM_PATTERN_SIZE] = { 0 };
	u8 broadcast_addr[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	u8 multicast_addr1[2] = { 0x33, 0x33 };
	u8 multicast_addr2[3] = { 0x01, 0x00, 0x5e };
	u8 res = FALSE, index = 0, mask_len = 0;
	u8 mac_addr[ETH_ALEN] = { 0 };
	u16 count = 0;
	int i, j;

	if (pwrctl->wowlan_pattern_idx > MAX_WKFM_NUM) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s pattern_idx is more than MAX_FMC_NUM: %d\n",
				__func__, MAX_WKFM_NUM));
		return FALSE;
	}

	pmlmeext = &Nic->mlmeextpriv;
	pmlmeinfo = &pmlmeext->mlmext_info;
	Func_Of_Proc_Pre_Memcpy(mac_addr, wadptdata_mac_addr(Nic), ETH_ALEN);
	memset(&wow_pattern, 0, sizeof(struct wltwow_pattern));

	mask_len = DIV_ROUND_UP(len, 8);

	if (memcmp(pattern, broadcast_addr, ETH_ALEN) == 0)
		wow_pattern.type = PATTERN_BROADCAST;
	else if (memcmp(pattern, multicast_addr1, 2) == 0)
		wow_pattern.type = PATTERN_MULTICAST;
	else if (memcmp(pattern, multicast_addr2, 3) == 0)
		wow_pattern.type = PATTERN_MULTICAST;
	else if (memcmp(pattern, mac_addr, ETH_ALEN) == 0)
		wow_pattern.type = PATTERN_UNICAST;
	else
		wow_pattern.type = PATTERN_INVALID;

	for (i = 0; i < mask_len - 1; i++) {
		mask_hw[i] = mask[i] >> 6;
		mask_hw[i] |= (mask[i + 1] & 0x3F) << 2;
	}

	mask_hw[i] = (mask[i] >> 6) & 0x3F;
	mask_hw[0] &= 0xC0;

	for (i = 0; i < (MAX_WKFM_SIZE / 4); i++) {
		wow_pattern.mask[i] = mask_hw[i * 4];
		wow_pattern.mask[i] |= (mask_hw[i * 4 + 1] << 8);
		wow_pattern.mask[i] |= (mask_hw[i * 4 + 2] << 16);
		wow_pattern.mask[i] |= (mask_hw[i * 4 + 3] << 24);
	}

	count = 0;
	for (i = 12; i < len; i++) {
		if ((mask[i / 8] >> (i % 8)) & 0x01) {
			content[count] = pattern[i];
			count++;
		}
	}

	wow_pattern.crc = sl_calc_crc(content, count);

	if (wow_pattern.crc != 0) {
		if (wow_pattern.type == PATTERN_INVALID)
			wow_pattern.type = PATTERN_VALID;
	}

	index = HwPlatformIORead1Byte(Nic, REG_WKFMCAM_NUM);

	if (!pwrctl->bInSuspend)
		index += 2;

	res = wl_write_to_pkt_list(Nic, index, &wow_pattern, 1);

	if (res == TRUE) {
		pwrctl->wowlan_pattern_idx++;
		HwPlatformIOWrite1Byte(Nic, REG_WKFMCAM_NUM, pwrctl->wowlan_pattern_idx);
	} else {
		WL_INFO("%s: ERROR write_to_frame_mask_cam fail\n", __func__);
	}

	return res;
}

static void Func_Of_Proc_Chip_Hw_Dl_Pattern(PNIC Nic, u8 clean_all)
{
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;
	int i = 0, total = 0;

	total = pwrpriv->wowlan_pattern_idx;

	clean_pattern_work(Nic, 1);

	if (!clean_all) {
		for (i = 0; i < total; i++) {
			Func_Of_Proc_Chip_Hw_Set_Pattern(Nic,
								pwrpriv->patterns[i].content,
								pwrpriv->patterns[i].len,
								pwrpriv->patterns[i].mask);
		}
		MpTrace(COMP_RICHD, DBG_NORMAL, ("pattern downloaded\n"));
	} else {
		for (i = 0; i < MAX_WKFM_NUM; i++) {
			memset(pwrpriv->patterns[i].content, '\0',
				   sizeof(pwrpriv->patterns[i].content));
			memset(pwrpriv->patterns[i].mask, '\0',
				   sizeof(pwrpriv->patterns[i].mask));
			pwrpriv->patterns[i].len = 0;
		}
		MpTrace(COMP_RICHD, DBG_NORMAL, ("clean all pattern\n"));
	}
}

static void Func_Of_Proc_Chip_Hw_Wow_Enable(PNIC Nic)
{
	struct pwrctrl_priv *pwrctl = &Nic->pwrctl_priv;
	struct security_priv *psecuritypriv = &Nic->securitypriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct sta_info *psta = NULL;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	int res;
	u16 media_status_rpt;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s, WOWLAN_ENABLE\n", __func__));

	Func_Of_Proc_Chip_Hw_Gate_Bb(Nic, TRUE);

	res = Func_Of_Proc_Chip_Hw_Pause_Rx_Dma(Nic);
	if (res == FALSE)
		MpTrace(COMP_RICHD, DBG_NORMAL, ("[WARNING] pause RX DMA fail\n"));

	Func_Of_Proc_Chip_Hw_Set_Wow_Rxff_Boundary(Nic, TRUE);

	if (pwrctl->wowlan_pattern)
		Func_Of_Proc_Chip_Hw_Dl_Pattern(Nic, FALSE);

	Func_Of_Proc_Chip_Hw_Fw_Dl(Nic, TRUE);
	media_status_rpt = WP_MEDIA_CONNECT;
	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_wMBOX1_FW_JOINBSSRPT,
					  (u8 *) & media_status_rpt);

	if (!pwrctl->wowlan_pno_enable) {
		psta = do_query_stainfo(&Nic->stapriv, get_bssid(pmlmepriv), 1);
		if (psta != NULL)
			rpt_sta_media_status(Nic, psta, 1, 1);
	}
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Set WOWLan cmd\n"));	

	Func_Of_Proc_Chip_Hw_Set_Fw_Wow_Related_Cmd(Nic, 1);

	res = Func_Of_Proc_Chip_Hw_Check_Wow_Ctrl(Nic, TRUE);

	if (res == FALSE)
		MpTrace(COMP_RICHD, DBG_NORMAL, ("[Error]%s: set wowlan CMD fail!!\n", __func__));

	pwrctl->wowlan_wake_reason = HwPlatformIORead1Byte(Nic, REG_WOWLAN_WAKE_REASON);
	MpTrace(COMP_RICHD, DBG_NORMAL,
		("wowlan_wake_reason: 0x%02x\n", pwrctl->wowlan_wake_reason));

#ifdef CONFIG_GTK_OL_DBG
	dump_sec_cam(WL_DBGDUMP, Nic);
#endif
	if (Nic->intf_stop)
		Nic->intf_stop(Nic);

#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic))
		Nic->pbuddy_wadptdata->intf_stop(Nic->pbuddy_wadptdata);
#endif
	HwPlatformIOWrite1Byte(Nic, REG_RSV_CTRL, 0x20);

	Func_Of_Proc_Chip_Hw_Gate_Bb(Nic, FALSE);
}

static void Func_Of_Proc_Chip_Hw_Wow_Disable(PNIC Nic)
{
	struct pwrctrl_priv *pwrctl = &Nic->pwrctl_priv;
	struct security_priv *psecuritypriv = &Nic->securitypriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct sta_info *psta = NULL;
	int res;
	u16 media_status_rpt;
	u8 val8;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s, WOWLAN_DISABLE\n", __func__));

	if (!pwrctl->wowlan_pno_enable) {
		psta = do_query_stainfo(&Nic->stapriv, get_bssid(pmlmepriv), 1);
		if (psta != NULL)
			rpt_sta_media_status(Nic, psta, 0, 1);
		else
			MpTrace(COMP_RICHD, DBG_NORMAL, ("%s: psta is null\n", __func__));

	}

	pwrctl->wowlan_wake_reason = HwPlatformIORead1Byte(Nic, REG_WOWLAN_WAKE_REASON);
	MpTrace(COMP_RICHD, DBG_NORMAL,
		("wakeup_reason: 0x%02x\n", pwrctl->wowlan_wake_reason));

	Func_Of_Proc_Chip_Hw_Set_Fw_Wow_Related_Cmd(Nic, 0);

	res = Func_Of_Proc_Chip_Hw_Check_Wow_Ctrl(Nic, FALSE);

	if (res == FALSE) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("[Error]%s: disable WOW cmd fail\n!!", __func__));
		Func_Of_Proc_Chip_Hw_Force_Enable_Rxdma(Nic);
	}

	Func_Of_Proc_Chip_Hw_Gate_Bb(Nic, TRUE);

	res = Func_Of_Proc_Chip_Hw_Pause_Rx_Dma(Nic);
	if (res == FALSE)
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("[WARNING] pause RX DMA fail\n"));

	if (pwrctl->wowlan_pattern)
		Func_Of_Proc_Chip_Hw_Dl_Pattern(Nic, TRUE);

	Func_Of_Proc_Chip_Hw_Set_Wow_Rxff_Boundary(Nic, FALSE);

	Func_Of_Proc_Chip_Hw_Release_Rx_Dma(Nic);

	Func_Of_Proc_Chip_Hw_Update_Tx_Iv(Nic);

	Func_Of_Proc_Chip_Hw_Fw_Dl(Nic, FALSE);

	if ((pwrctl->wowlan_wake_reason != FWDecisionDisconnect) &&
		(pwrctl->wowlan_wake_reason != Rx_Pairwisekey) &&
		(pwrctl->wowlan_wake_reason != Rx_DisAssoc) &&
		(pwrctl->wowlan_wake_reason != Rx_DeAuth)) {

		media_status_rpt = WP_MEDIA_CONNECT;
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_wMBOX1_FW_JOINBSSRPT,
						  (u8 *) & media_status_rpt);

		if (psta != NULL)
			rpt_sta_media_status(Nic, psta, 1, 1);
	}
	Func_Of_Proc_Chip_Hw_Gate_Bb(Nic, FALSE);
}
#endif

void Func_Wf_Set_Fw_Lps_Config(PNIC Nic)
{

	struct mlme_priv *pmlmepriv;
	struct mlme_ext_priv *pmlmeext;
	struct mlme_ext_info *pmlmeinfo;
	int ret;
	u8 *pbuf;
	u8 *phead;
	int mail_box_len = 0;
	u32 send[MAILBOX_MAX_TXLEN];
	u32 recv[MAILBOX_MAX_RDLEN];
	WLAN_BSSID_EX *cur_network;
	u8 ip[4];
	u8 iv[8];
	struct pwrctrl_priv *pwrctl = &Nic->pwrctl_priv;
	struct security_priv *psecuritypriv = &Nic->securitypriv;
	struct sta_info *psta = NULL;
	int i, j = 0;
	u32 enable = 1;

	pmlmepriv = &(Nic->mlmepriv);

	pmlmeext = &Nic->mlmeextpriv;
	pmlmeinfo = &pmlmeext->mlmext_info;

	cur_network = &(pmlmeinfo->network);

	pbuf = phead = (u8 *) send;
	memset(pbuf, 0, sizeof(send));
	memcpy(pbuf, (u8 *) cur_network->MacAddress, 6);
	pbuf += 6;
	mail_box_len += 6;

	memcpy(pbuf, (u8 *) & pmlmeinfo->aid, 2);
	pbuf += 2;
	mail_box_len += 2;

	if (mail_box_len + sizeof(NDIS_802_11_FIXED_IEs) > MAILBOX_MAX_TXLEN * 4) {
		goto exit;
	}
	memcpy(pbuf, (u8 *) cur_network->IEs, sizeof(NDIS_802_11_FIXED_IEs));
	pbuf += sizeof(NDIS_802_11_FIXED_IEs);
	mail_box_len += sizeof(NDIS_802_11_FIXED_IEs);

	if (mail_box_len + sizeof(NDIS_802_11_SSID) > MAILBOX_MAX_TXLEN * 4) {
		goto exit;
	}
	memcpy(pbuf, (u8 *) & cur_network->Ssid, sizeof(NDIS_802_11_SSID));
	pbuf += sizeof(NDIS_802_11_SSID);
	mail_box_len += sizeof(NDIS_802_11_SSID);

	if (mail_box_len + sizeof(NDIS_802_11_RATES_EX) > MAILBOX_MAX_TXLEN * 4) {
		goto exit;
	}
	memcpy(pbuf, (u8 *) & cur_network->SupportedRates,
		   sizeof(NDIS_802_11_RATES_EX));
	pbuf += sizeof(NDIS_802_11_RATES_EX);
	mail_box_len += sizeof(NDIS_802_11_RATES_EX);

	if (mail_box_len + 4 > MAILBOX_MAX_TXLEN * 4) {
		goto exit;
	}
	memcpy(pbuf, (u8 *) & cur_network->Configuration.DSConfig, 4);
	pbuf += 4;
	mail_box_len += 4;
#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
	MpTrace(COMP_RICHD, DBG_NORMAL,
		("%s %d mail_box_len = %d\r\n", __func__, __LINE__, mail_box_len));

	if (pwrctl->wowlan_mode) {

		if (mail_box_len + 4 > MAILBOX_MAX_TXLEN * 4)
			goto exit;
		memcpy(pbuf, (u8 *) & enable, 4);
		mail_box_len += 4;
		pbuf += 4;

		Func_Of_Proc_Get_Current_Ip_Address(Nic, ip);
		if (mail_box_len + 4 > MAILBOX_MAX_TXLEN * 4)
			goto exit;
		memcpy(pbuf, (u8 *) ip, 4);
		mail_box_len += 4;
		pbuf += 4;

		if (mail_box_len + 4 > MAILBOX_MAX_TXLEN * 4)
			goto exit;
		memcpy(pbuf, (u8 *) pmlmepriv->gw_ip, 4);
		mail_box_len += 4;
		pbuf += 4;

		if (mail_box_len + 6 > MAILBOX_MAX_TXLEN * 4)
			goto exit;
		memcpy(pbuf, (u8 *) pmlmepriv->gw_mac_addr, 6);
		mail_box_len += 6;
		pbuf += 6;

		do_query_sec_iv(Nic, iv, query_bssid(&pmlmeinfo->network), 1);
		if (mail_box_len + 8 > MAILBOX_MAX_TXLEN * 4)
			goto exit;
		memcpy(pbuf, (u8 *) iv, 8);
		mail_box_len += 8;
		pbuf += 8;

		if (mail_box_len + 4 > MAILBOX_MAX_TXLEN * 4)
			goto exit;
		memcpy(pbuf, (u8 *) & psecuritypriv->dot11PrivacyAlgrthm, 4);
		mail_box_len += 4;
		pbuf += 4;

		if (mail_box_len + 16 > MAILBOX_MAX_TXLEN * 4)
			goto exit;
		psta = do_query_stainfo(&Nic->stapriv, cur_network->MacAddress, 1);
		if (psta && psecuritypriv->dot11PrivacyAlgrthm == _TKIP_) {

			memcpy(pbuf, (u8 *) & psta->dot11tkiptxmickey.skey[0], 16);
			pbuf += 16;
			mail_box_len += 16;
		}

	}
	MpTrace(COMP_RICHD, DBG_NORMAL,
		("%s %d mail_box_len = %d\r\n", __func__, __LINE__, mail_box_len));

#endif

	mail_box_len = _RND(mail_box_len, 4);
	for (i = 0; i < mail_box_len; i++) {

		MpTrace(COMP_RICHD, DBG_NORMAL,("%02X ", phead[i]));

		if (++j >= 32) {
			j = 0;
			MpTrace(COMP_RICHD, DBG_NORMAL,("\r\n"));
		}
	}

	mail_box_len = mail_box_len / 4;
	MpTrace(COMP_RICHD, DBG_NORMAL,
		("%s %d mail_box_len = %d\r\n", __func__, __LINE__, mail_box_len));

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_LPS_CONFIG, send, NULL,
									mail_box_len, 0);
	if (!ret) {
		goto exit;
	}
exit:
	return;
}

void Func_Of_Proc_Chip_Hw_Set_Fw_Rsvd_Page(PNIC Nic, bool finished)
{
	PHAL_DATA_TYPE pHalData;
	struct xmit_frame *pcmdframe;
	struct pkt_attrib *pattrib;
	struct xmit_priv *pxmitpriv;
	struct mlme_ext_priv *pmlmeext;
	struct mlme_ext_info *pmlmeinfo;
	struct pwrctrl_priv *pwrctl;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	u32 BeaconLength = 0, ProbeRspLength = 0, PSPollLength = 0;
	u32 NullDataLength = 0, QosNullLength = 0, BTQosNullLength = 0;
	u32 ProbeReqLength = 0, NullFunctionDataLength = 0;
	u8 TxDescLen = TXDESC_SIZE, TxDescOffset = TXDESC_OFFSET;
	u8 TotalPageNum = 0, CurtPktPageNum = 0, RsvdPageNum = 0;
	u8 *ReservedPagePacket;
	u16 BufIndex = 0;
	u32 TotalPacketLen = 0, MaxRsvdPageBufSize = 0, PageSize = 0;
	RSVDPAGE_LOC RsvdPageLoc;
	u8 temp;
	int i = 0, j = 0;
	int xmit_success = 0;
	int ret;
	u32 *pread = NULL;
	u8 *head = NULL;
	u32 recv[MAILBOX_MAX_RDLEN];
	int word_len;
	int intger, remainder;
	int offset = 0;

	pHalData = GET_HAL_DATA(Nic);

	pxmitpriv = &Nic->xmitpriv;
	pmlmeext = &Nic->mlmeextpriv;
	pmlmeinfo = &pmlmeext->mlmext_info;
	pwrctl = &Nic->pwrctl_priv;
#if 0
    //need to add
	pcmdframe = wl_alloc_cmdxmitframe(pxmitpriv);
#else
	pcmdframe = &Nic->pwrctl_priv;
#endif

	if (pcmdframe == NULL) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s: alloc ReservedPagePacket fail!\n", __FUNCTION__));

		return;

	}

	RsvdPageLoc.LocProbeRsp = 0;
	RsvdPageLoc.LocPsPoll = 2;
	RsvdPageLoc.LocNullData = 3;
	RsvdPageLoc.LocQosNull = 4;
	RsvdPageLoc.LocBTQosNull = 0;

	head = (u8 *) wl_zmalloc(1026);
	pread = (u32 *) head;
	if (!pread) {

		goto error;
	}
	word_len = 1026 / 4;
	intger = word_len / MAILBOX_MAX_RDLEN;
	remainder = word_len % MAILBOX_MAX_RDLEN;

	for (i = 0; i < intger; i++) {
		ret =
			Func_Mcu_Special_Func_Interface(Nic, UMSG_OPS_HAL_LPS_GET, pread,
									  MAILBOX_MAX_RDLEN, offset);
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("chuck    MAILBOX_MAX_RDLEN  = 0%x,offset = 0x%x\n",
				MAILBOX_MAX_RDLEN, offset));
		if (!ret) {

			goto error;
		} else {
			offset = offset + MAILBOX_MAX_RDLEN;
		}
		pread = pread + MAILBOX_MAX_RDLEN;
	}

	if (remainder > 0) {
		ret =
			Func_Mcu_Special_Func_Interface(Nic, UMSG_OPS_HAL_LPS_GET, pread,
									  remainder, offset);
		if (!ret) {

			goto error;
		}
		offset = offset + remainder;
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("chuck    MAILBOX_MAX_RDLEN  = 0%x,offset = 0x%x\n",
				MAILBOX_MAX_RDLEN, offset));
	}

	TotalPacketLen = *(u16 *) head;

	memcpy(pcmdframe->buf_addr, &head[2], TotalPacketLen);

download_page:
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s PageNum(%d), pktlen(%d)\n",
			__func__, TotalPageNum, TotalPacketLen));

	{
		pattrib = &pcmdframe->attrib;
		proc_mgntframe_attrib_update_func(Nic, pattrib);
		pattrib->qsel = QSLT_BEACON;
		pattrib->pktlen = TotalPacketLen - TxDescOffset;
		pattrib->last_txcmdsz = TotalPacketLen - TxDescOffset;
		pattrib->rsvd_packet = 1;
#if 0		
		mgntframe_and_wait_to_dump_func(Nic, pcmdframe, 100);
#endif
		xmit_success = 1;
	}
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s: Set RSVD page location to Fw ,TotalPacketLen(%d), TotalPageNum(%d)\n",
		 __func__, TotalPacketLen, TotalPageNum));

error:
	if (head){
		wl_mfree(head, 1026);
	}
#if 0
	//need to change
	if (!xmit_success)
		unnew_txp_func(pxmitpriv, pcmdframe, 1);
#endif
}

void Func_Mcu_Sethwreg(PNIC Nic, u8 variable, u8 * val)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
	u8 temp;
	int ret = FALSE;
	u32 v[2] = { 0 };
	struct security_priv *sec = &Nic->securitypriv;

	_func_enter_;

	switch (variable) {
	case HW_VAR_PORT_SWITCH:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_PORT_SWITCH, NULL, 0);
#if defined(CONFIG_CONCURRENT_MODE)
		if (Nic->iface_type == IFACE_PORT0) {
			Nic->iface_type = IFACE_PORT1;
			Nic->pbuddy_wadptdata->iface_type = IFACE_PORT0;
        	MpTrace(COMP_RICHD, DBG_NORMAL, 
        		("port switch - port0(" ADPT_FMT "), port1(" ADPT_FMT
					   ")\n", ADPT_ARG(Nic->pbuddy_wadptdata),
					   ADPT_ARG(Nic)));			
		} else {
			Nic->iface_type = IFACE_PORT0;
			Nic->pbuddy_wadptdata->iface_type = IFACE_PORT1;
        	MpTrace(COMP_RICHD, DBG_NORMAL, 
        		("port switch - port0(" ADPT_FMT "), port1(" ADPT_FMT
					   ")\n", ADPT_ARG(Nic),
					   ADPT_ARG(wadptdata->pbuddy_wadptdata)));
		}
#endif
		break;

	case HW_VAR_INIT_RTS_RATE:
		break;

	case HW_VAR_SEC_CFG:
		{
			u8 mbox[4] = { 0 };
			mbox[0] = 0;//ghy change _wl_chk_camctl_cap(Nic, SEC_CAP_CHK_BMC);
			mbox[1] = 0;//ghy change
				/*_wl_chk_camctl_mask(Nic,
									  SEC_STATUS_STA_PK_GK_CONFLICT_DIS_BMC_SEARCH);*/
			mbox[2] = *((u8 *)val);
			mbox[3] = 1;

			Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_SEC_CFG, mbox, 4);

		}
		break;

	case HW_VAR_SEC_DK_CFG:
		if (val) {
			v[0] = 1;
		} else {
			v[0] = 0;
		}

		v[1] = sec->dot11AuthAlgrthm;

		ret =
			Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HW_SET_DK_CFG, v, NULL,
										2, 0);
		if (!ret) {

		}
		break;

	case HW_VAR_ASIX_IOT:
		ret = Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_ASIX_IOT, val, 1);
		if (!ret) {

		}
		break;

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
	case HW_VAR_WOWLAN:
		{
			struct wowlan_ioctl_param *poidparam;

			poidparam = (struct wowlan_ioctl_param *)val;
			switch (poidparam->subcode) {
#ifdef CONFIG_WOWLAN
			case WOWLAN_PATTERN_CLEAN:
				Func_Of_Proc_Chip_Hw_Dl_Pattern(Nic, TRUE);
				break;
			case WOWLAN_ENABLE:
				Func_Of_Proc_Chip_Hw_Wow_Enable(Nic);
				break;
			case WOWLAN_DISABLE:
				Func_Of_Proc_Chip_Hw_Wow_Disable(Nic);
				break;
#endif
#ifdef CONFIG_AP_WOWLAN
			case WOWLAN_AP_ENABLE:
				Func_Of_Proc_Chip_Hw_Ap_Wow_Enable(Nic);
				break;
			case WOWLAN_AP_DISABLE:
				Func_Of_Proc_Chip_Hw_Ap_Wow_Disable(Nic);
				break;
#endif
			default:
				break;
			}
		}
		break;
#endif
	default:
		if (0)
        	MpTrace(COMP_RICHD, DBG_NORMAL, 
        		("variable(%d) not defined!\n", variable));
		break;
	}

	_func_exit_;
}

void Func_To_Get_Hwreg(PNIC Nic, u8 variable, u8 * val)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);

	_func_enter_;

	switch (variable) {
	case HW_VAR_BASIC_RATE:
		*((u16 *) val) = hal_data->BasicRateSet;
		break;
	case HW_VAR_RF_TYPE:
		*((u8 *) val) = hal_data->rf_type;
		break;
	case HW_VAR_DO_IQK:
		*val = hal_data->bNeedIQK;
		break;
	case HW_VAR_CH_SW_NEED_TO_TAKE_CARE_IQK_INFO:
		*val = FALSE;

		break;
	default:
		if (0)
        	MpTrace(COMP_RICHD, DBG_NORMAL, 
        		("variable(%d) not defined!\n", variable));
		break;
	}

	_func_exit_;
}

u8 Func_To_Get_Hal_Def_Var(PNIC Nic, HAL_DEF_VARIABLE variable, void *value)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
	u8 bResult = TRUE;

	switch (variable) {
	case HAL_DEF_UNDERCORATEDSMOOTHEDPWDB:
		{
#if 0	

			struct mlme_priv *pmlmepriv;
			struct sta_priv *pstapriv;
			struct sta_info *psta;

			pmlmepriv = &Nic->mlmepriv;
			pstapriv = &Nic->stapriv;
		
			//need to add
			psta =
				do_query_stainfo(pstapriv,
								pmlmepriv->cur_network.network.MacAddress, 1);

			psta = &Nic->stapriv;
			
			if (psta) {
				*((int *)value) = 0;
			}
#else
            *((int *)value) = 0;
#endif
		}
		break;
	case HAL_DEF_DBG_DUMP_RXPKT:
		*((u8 *) value) = hal_data->bDumpRxPkt;
		break;
	case HAL_DEF_DBG_DUMP_TXPKT:
		*((u8 *) value) = hal_data->bDumpTxPkt;
		break;
	case HAL_DEF_ANT_DETECT:
		*((u8 *) value) = hal_data->AntDetection;
		break;
	case HAL_DEF_MACID_SLEEP:
		*(u8 *) value = FALSE;
		break;
	case HAL_DEF_TX_PAGE_SIZE:
		*((u32 *) value) = PAGE_SIZE_128;
		break;
	case HAL_DEF_DBG_DIS_PWT:
		*(u8 *) value = hal_data->bDisableTXPowerTraining;
		break;
	default:
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s: [WARNING] HAL_DEF_VARIABLE(%d) not defined!\n",
				   __FUNCTION__, variable));
		bResult = FALSE;
		break;
	}

	return bResult;
}

BOOLEAN Func_Eqnbyte(u8 * str1, u8 * str2, u32 num)
{
	if (num == 0)
		return FALSE;
	while (num > 0) {
		num--;
		if (str1[num] != str2[num])
			return FALSE;
	}
	return TRUE;
}

static u32 Func_Mapchartohexdigit(IN char chTmp)
{
	if (chTmp >= '0' && chTmp <= '9')
		return (chTmp - '0');
	else if (chTmp >= 'a' && chTmp <= 'f')
		return (10 + (chTmp - 'a'));
	else if (chTmp >= 'A' && chTmp <= 'F')
		return (10 + (chTmp - 'A'));
	else
		return 0;
}

BOOLEAN
Func_To_Get_Hex_Value_From_String(IN char *szStr,
					  IN OUT u32 * pu4bVal, IN OUT u32 * pu4bMove)
{
	char *szScan = szStr;

	if (szStr == NULL || pu4bVal == NULL || pu4bMove == NULL) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Func_To_Get_Hex_Value_From_String(): Invalid inpur argumetns! szStr: %p, pu4bVal: %p, pu4bMove: %p\n",
			 szStr, pu4bVal, pu4bMove));

		return FALSE;
	}

	*pu4bMove = 0;
	*pu4bVal = 0;

	while (*szScan != '\0' && (*szScan == ' ' || *szScan == '\t')) {
		szScan++;
		(*pu4bMove)++;
	}

	if (*szScan == '0' && (*(szScan + 1) == 'x' || *(szScan + 1) == 'X')) {
		szScan += 2;
		(*pu4bMove) += 2;
	}

	if (!Func_Ishexdigit(*szScan)) {
		return FALSE;
	}

	do {
		(*pu4bVal) <<= 4;
		*pu4bVal += Func_Mapchartohexdigit(*szScan);

		szScan++;
		(*pu4bMove)++;
	} while (Func_Ishexdigit(*szScan));

	return TRUE;
}

BOOLEAN
Func_To_Get_Fraction_Value_From_String(IN char *szStr,
						   IN OUT u8 * pInteger,
						   IN OUT u8 * pFraction, IN OUT u32 * pu4bMove)
{
	char *szScan = szStr;

	*pu4bMove = 0;
	*pInteger = 0;
	*pFraction = 0;

	while (*szScan != '\0' && (*szScan == ' ' || *szScan == '\t')) {
		++szScan;
		++(*pu4bMove);
	}

	do {
		(*pInteger) *= 10;
		*pInteger += (*szScan - '0');

		++szScan;
		++(*pu4bMove);

		if (*szScan == '.') {
			++szScan;
			++(*pu4bMove);

			if (*szScan < '0' || *szScan > '9')
				return FALSE;
			else {
				*pFraction = *szScan - '0';
				++szScan;
				++(*pu4bMove);
				return TRUE;
			}
		}
	} while (*szScan >= '0' && *szScan <= '9');

	return TRUE;
}

BOOLEAN Func_Iscommentstring(IN char *szStr)
{
	if (*szStr == '/' && *(szStr + 1) == '/') {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOLEAN Func_To_Get_U1Byteinteger_From_String_In_Decimal(IN char *Str, IN OUT u8 * pInt)
{
	u16 i = 0;
	*pInt = 0;

	while (Str[i] != '\0') {
		if (Str[i] >= '0' && Str[i] <= '9') {
			*pInt *= 10;
			*pInt += (Str[i] - '0');
		} else {
			return FALSE;
		}
		++i;
	}

	return TRUE;
}

BOOLEAN
Func_Parsequalifiedstring(IN char *In,
					 IN OUT u32 * Start,
					 OUT char *Out,
					 IN char LeftQualifier, IN char RightQualifier)
{
	u32 i = 0, j = 0;
	char c = In[(*Start)++];

	if (c != LeftQualifier)
		return FALSE;

	i = (*Start);
	while ((c = In[(*Start)++]) != RightQualifier) ;
	j = (*Start) - 2;
	strncpy((char *)Out, (const char *)(In + i), j - i + 1);

	return TRUE;
}

BOOLEAN Func_Isallspaceortab(u8 * data, u8 size)
{
	u8 cnt = 0, NumOfSpaceAndTab = 0;

	while (size > cnt) {
		if (data[cnt] == ' ' || data[cnt] == '\t' || data[cnt] == '\0')
			++NumOfSpaceAndTab;

		++cnt;
	}

	return size == NumOfSpaceAndTab;
}

void Func_Of_Proc_Chip_Hw_Check_Rxfifo_Full(PNIC Nic)
{
	struct debug_priv *pdbgpriv = &Nic->drv_dbg;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	int ret = FALSE;
	u32 mbox[2] = { 0 };
	int save_cnt = FALSE;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_CHECK_RXFIFO_FULL, NULL,
									mbox, 0, 2);
	if (!ret) {
		return;
	}
	save_cnt = (int)mbox[0];

	if (save_cnt) {
		pdbgpriv->dbg_rx_fifo_last_overflow =
			pdbgpriv->dbg_rx_fifo_curr_overflow;
		pdbgpriv->dbg_rx_fifo_curr_overflow = mbox[1];
		pdbgpriv->dbg_rx_fifo_diff_overflow =
			pdbgpriv->dbg_rx_fifo_curr_overflow -
			pdbgpriv->dbg_rx_fifo_last_overflow;
	} else {
		pdbgpriv->dbg_rx_fifo_last_overflow = 1;
		pdbgpriv->dbg_rx_fifo_curr_overflow = 1;
		pdbgpriv->dbg_rx_fifo_diff_overflow = 1;
	}
}
#if 0
void Func_Linked_Info_Dump(PNIC Nic, u8 benable)
{
	struct pwrctrl_priv *pwrctrlpriv = &Nic->pwrctl_priv;

	if (Nic->bLinkInfoDump == benable)
		return;

	WL_INFO("%s %s \n", __FUNCTION__, (benable) ? "enable" : "disable");

	if (benable) {
#ifdef CONFIG_LPS
		pwrctrlpriv->org_power_mgnt = pwrctrlpriv->power_mgnt;
		do_set_pm_lps(Nic, PS_MODE_ACTIVE, 1);
#endif

	} else {

#ifdef CONFIG_LPS
		do_set_pm_lps(Nic, pwrctrlpriv->org_power_mgnt, 1);
#endif
	}
	Nic->bLinkInfoDump = benable;
}
#endif

#ifdef DBG_RX_SIGNAL_DISPLAY_RAW_DATA
void Func_Of_Proc_Get_Raw_Rssi_Info(void *sel, PNIC Nic)
{
	u8 isCCKrate, rf_path;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	struct rx_raw_rssi *psample_pkt_rssi = &Nic->recvpriv.raw_rssi_info;
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("RxRate = %s, PWDBALL = %d(%%), rx_pwr_all = %d(dBm)\n",
			   HDATA_RATE(psample_pkt_rssi->data_rate),
			   psample_pkt_rssi->pwdball, psample_pkt_rssi->pwr_all));
	isCCKrate = (psample_pkt_rssi->data_rate <= DESC_RATE11M) ? TRUE : FALSE;

	if (isCCKrate)
		psample_pkt_rssi->mimo_signal_strength[0] = psample_pkt_rssi->pwdball;

	for (rf_path = 0; rf_path < pHalData->NumTotalRFPath; rf_path++) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("RF_PATH_%d=>signal_strength:%d(%%),signal_quality:%d(%%)\n",
				   rf_path, psample_pkt_rssi->mimo_signal_strength[rf_path],
				   psample_pkt_rssi->mimo_signal_quality[rf_path]));

		if (!isCCKrate) {
			MpTrace(COMP_RICHD, DBG_NORMAL, 
				("\trx_ofdm_pwr:%d(dBm),rx_ofdm_snr:%d(dB)\n",
					   psample_pkt_rssi->ofdm_pwr[rf_path],
					   psample_pkt_rssi->ofdm_snr[rf_path]));
		}
	}
}

void Func_Of_Proc_Dump_Raw_Rssi_Info(PNIC Nic, void *sel)
{
	u8 isCCKrate, rf_path;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	struct rx_raw_rssi *psample_pkt_rssi = &Nic->recvpriv.raw_rssi_info;
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("============ RAW Rx Info dump ===================\n"));
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("RxRate = %s, PWDBALL = %d(%%), rx_pwr_all = %d(dBm)\n",
			   HDATA_RATE(psample_pkt_rssi->data_rate),
			   psample_pkt_rssi->pwdball, psample_pkt_rssi->pwr_all));

	isCCKrate = (psample_pkt_rssi->data_rate <= DESC_RATE11M) ? TRUE : FALSE;

	if (isCCKrate)
		psample_pkt_rssi->mimo_signal_strength[0] = psample_pkt_rssi->pwdball;

	for (rf_path = 0; rf_path < pHalData->NumTotalRFPath; rf_path++) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("RF_PATH_%d=>signal_strength:%d(%%),signal_quality:%d(%%)",
				   rf_path, psample_pkt_rssi->mimo_signal_strength[rf_path],
				   psample_pkt_rssi->mimo_signal_quality[rf_path]));

		if (!isCCKrate)
			MpTrace(COMP_RICHD, DBG_NORMAL, 
				(",rx_ofdm_pwr:%d(dBm),rx_ofdm_snr:%d(dB)\n",
					   psample_pkt_rssi->ofdm_pwr[rf_path],
					   psample_pkt_rssi->ofdm_snr[rf_path]));

		else
			MpTrace(COMP_RICHD, DBG_NORMAL, ("\n"));

	}
}

void Func_Of_Proc_Store_Phy_Info(PNIC Nic, union recv_frame *prframe)
{
	u8 isCCKrate, rf_path;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;

	PMSG_PHY_INFO_T pPhyInfo = (PMSG_PHY_INFO_T) (&pattrib->phy_info);
	struct rx_raw_rssi *psample_pkt_rssi = &Nic->recvpriv.raw_rssi_info;

	psample_pkt_rssi->data_rate = pattrib->data_rate;
	isCCKrate = (pattrib->data_rate <= DESC_RATE11M) ? TRUE : FALSE;

	psample_pkt_rssi->pwdball = pPhyInfo->RxPWDBAll;
	psample_pkt_rssi->pwr_all = pPhyInfo->RecvSignalPower;

	for (rf_path = 0; rf_path < pHalData->NumTotalRFPath; rf_path++) {
		psample_pkt_rssi->mimo_signal_strength[rf_path] =
			pPhyInfo->RxMIMOSignalStrength[rf_path];
		psample_pkt_rssi->mimo_signal_quality[rf_path] =
			pPhyInfo->RxMIMOSignalQuality[rf_path];
		if (!isCCKrate) {
			psample_pkt_rssi->ofdm_pwr[rf_path] = pPhyInfo->RxPwr[rf_path];
			psample_pkt_rssi->ofdm_snr[rf_path] = pPhyInfo->RxSNR[rf_path];
		}
	}
}
#endif

int Func_Check_Phy_Efuse_Tx_Power_Info_Valid(PNIC Nic)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	u8 *pContent = pHalData->efuse_eeprom_data;
	int index = 0;
	u16 tx_index_offset = 0x0000;

	tx_index_offset = EEPROM_TX_PWR_INX_9086X;

	for (index = 0; index < 11; index++) {
		if (pContent[tx_index_offset + index] == 0xFF)
			return FALSE;
	}
	return TRUE;
}

int Func_Chip_Hw_Efuse_Macaddr_Offset(PNIC Nic)
{
	u8 interface_type = 0;
	int addr_offset = -1;

	addr_offset = EEPROM_MAC_ADDR_9086XU;

	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s: - Macaddr_Offset:%u\n",__func__, addr_offset));


	return addr_offset;
}

int Func_Chip_Hw_Getphyefusemacaddr(PNIC Nic, u8 * mac_addr)
{
	int ret = FALSE;
	int addr_offset;
	int efuse_code = 1, i = 0;
	u8 *outbuff = NULL;

	outbuff = wl_zmalloc(MAILBOX_MAX_TXLEN * 4);
	if (outbuff == NULL) {
		goto exit;
	}

	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_MP_EFUSE_GET, &efuse_code, (u32 *) outbuff, 1,
		 MAILBOX_MAX_TXLEN)) {

		goto exit;
	}

	for (i = 0; i < outbuff[0]; ++i) {
		mac_addr[i] = outbuff[i + 1];
	}

exit:
	if (outbuff)
		wl_mfree(outbuff, MAILBOX_MAX_TXLEN * 4);

	return ret;
}

#ifdef CONFIG_RF_POWER_TRIM
static u32 Array_kfreemap[] = {
	0x08, 0xe,
	0x06, 0xc,
	0x04, 0xa,
	0x02, 0x8,
	0x00, 0x6,
	0x03, 0x4,
	0x05, 0x2,
	0x07, 0x0,
	0x09, 0x0,
	0x0c, 0x0,
};

void Func_Of_Proc_Bb_Rf_Gain_Offset(PNIC Nic)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	struct registry_priv *registry_par = &Nic->registrypriv;
	struct kfree_data_t *kfree_data = &pHalData->kfree_data;
	u8 value = pHalData->EEPROMRFGainOffset;
	u8 tmp = 0x3e;
	u32 res, i = 0;
	u4Byte ArrayLen = sizeof(Array_kfreemap) / sizeof(u32);
	u32* Array = Array_kfreemap;
	u4Byte v1 = 0, v2 = 0, GainValue = 0, target = 0;

	if (registry_par->RegPwrTrimEnable == 2) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Registry kfree default force disable.\n"));
		return;
	}

	if (kfree_data->flag & KFREE_FLAG_ON)
		wl_us_tx_gain_offset(Nic, 6, 1);

}
#endif

bool Func_Kfree_Data_Is_Bb_Gain_Empty(struct kfree_data_t *data)
{
#ifdef CONFIG_RF_POWER_TRIM
	int i, j;

	if (data->bb_gain[0][0] != 0)
		return 0;
#endif
	return 1;
}

#ifdef CONFIG_USB_RX_AGGREGATION
static void Func_Of_Proc_Set_Usb_Agg_By_Mode_Normal(PNIC Nic, u8 cur_wireless_mode)
{
	int ret = FALSE;
	u32 mbox1[1] = { 0 };
	u32 mbox2[1] = { 0 };

	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	if (cur_wireless_mode < WIRELESS_11_24N && cur_wireless_mode > 0) {
		if (0x6 != pHalData->RegAcUsbDmaSize
			|| 0x10 != pHalData->RegAcUsbDmaTime) {
			pHalData->RegAcUsbDmaSize = 0x6;
			pHalData->RegAcUsbDmaTime = 0x10;

			mbox1[0] = cur_wireless_mode;
			ret =
				Func_Mcu_Universal_Func_Interface(Nic,
											UMSG_OPS_HAL_SET_USB_AGG_NORMAL,
											mbox1, NULL, 1, 0);
			if (!ret) {
				MpTrace(COMP_RICHD, DBG_NORMAL, 
					("===> %s fail \n", __func__));
				return;
			}
		}

	} else if (cur_wireless_mode >= WIRELESS_11_24N
			   && cur_wireless_mode <= WIRELESS_MODE_MAX) {
		if (0x5 != pHalData->RegAcUsbDmaSize
			|| 0x20 != pHalData->RegAcUsbDmaTime) {
			pHalData->RegAcUsbDmaSize = 0x5;
			pHalData->RegAcUsbDmaTime = 0x20;

			mbox2[0] = cur_wireless_mode;

			ret =
				Func_Mcu_Universal_Func_Interface(Nic,
											UMSG_OPS_HAL_SET_USB_AGG_NORMAL,
											mbox2, NULL, 1, 0);
			if (!ret) {
				MpTrace(COMP_RICHD, DBG_NORMAL, 
					("===> %s fail \n", __func__));
				return;
			}

		}

	} else {
	}
}

void Func_Of_Proc_Set_Usb_Agg_By_Mode_Customer(PNIC Nic, u8 cur_wireless_mode,
									  u8 UsbDmaSize, u8 Legacy_UsbDmaSize)
{

}

static void Func_Of_Proc_Set_Usb_Agg_By_Mode(PNIC Nic, u8 cur_wireless_mode)
{
	Func_Of_Proc_Set_Usb_Agg_By_Mode_Normal(Nic, cur_wireless_mode);
}
#endif

void Func_Dm_Dynamicusbtxagg(PNIC Nic, u8 from_timer)
{
//	struct dvobj_priv *pdvobjpriv = wadptdata_to_dvobj(Nic);
	struct mlme_priv *pmlmepriv = &(Nic->mlmepriv);
	struct mlme_ext_priv *pmlmeextpriv = &(Nic->mlmeextpriv);
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u8 cur_wireless_mode = pmlmeextpriv->cur_wireless_mode;
#ifdef CONFIG_CONCURRENT_MODE
	struct mlme_ext_priv *pbuddymlmeextpriv =
		&(Nic->pbuddy_wadptdata->mlmeextpriv);
#endif

#ifdef CONFIG_USB_RX_AGGREGATION
	Func_Of_Proc_Set_Usb_Agg_By_Mode(Nic, cur_wireless_mode);
#endif
}

inline u8 Func_Of_Proc_Chip_Hw_Busagg_Qsel_Check(PNIC Nic, u8 pre_qsel,
									u8 next_qsel)
{
	struct mlme_priv *pmlmepriv = &(Nic->mlmepriv);
	u8 chk_rst = TRUE;

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) != TRUE)
		return chk_rst;

	if (((pre_qsel == QSLT_HIGH) || ((next_qsel == QSLT_HIGH)))
		&& (pre_qsel != next_qsel)) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("### bus-agg break cause of qsel misatch, pre_qsel=0x%02x,next_qsel=0x%02x ###\n",
			 pre_qsel, next_qsel));
		chk_rst = FALSE;
	}
	return chk_rst;
}

#ifdef CONFIG_GPIO_API
u8 Func_Of_Proc_Chip_Hw_Get_Gpio(PNIC Nic, u8 gpio_num)
{
	u8 value;
	u8 direction;
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;

	do_deny_ps(Nic, PS_DENY_IOCTL, 1);

	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("rf_pwrstate=0x%02x\n", pwrpriv->rf_pwrstate));

	left_aps_direct(Nic, 1);

	direction =
		(wl_read8(Nic, REG_GPIO_PIN_CTRL + 2) & BIT(gpio_num)) >> gpio_num;

	if (direction)
		value =
			(wl_read8(Nic, REG_GPIO_PIN_CTRL + 1) & BIT(gpio_num)) >>
			gpio_num;
	else
		value =
			(wl_read8(Nic, REG_GPIO_PIN_CTRL) & BIT(gpio_num)) >> gpio_num;

	do_cancel_deny_ps(Nic, PS_DENY_IOCTL, 1);
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s direction=%d value=%d\n", __FUNCTION__, direction, value));

	return value;
}

int Func_Of_Proc_Chip_Hw_Set_Gpio_Output_Value(PNIC Nic, u8 gpio_num, bool isHigh)
{
	u8 direction = 0;
	u8 res = -1;

	do_deny_ps(Nic, PS_DENY_IOCTL, 1);

	left_aps_direct(Nic, 1);

	direction =
		(wl_read8(Nic, REG_GPIO_PIN_CTRL + 2) & BIT(gpio_num)) >> gpio_num;

	if (direction) {
		if (isHigh)
			wl_write8(Nic, REG_GPIO_PIN_CTRL + 1,
					   wl_read8(Nic,
								 REG_GPIO_PIN_CTRL + 1) | BIT(gpio_num));
		else
			wl_write8(Nic, REG_GPIO_PIN_CTRL + 1,
					   wl_read8(Nic,
								 REG_GPIO_PIN_CTRL + 1) & ~BIT(gpio_num));

		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s Set gpio %x[%d]=%d\n", __FUNCTION__, REG_GPIO_PIN_CTRL + 1,
				gpio_num, isHigh));

		res = 0;
	} else {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s The gpio is input,not be set!\n", __FUNCTION__));

		res = -1;
	}

	do_cancel_deny_ps(Nic, PS_DENY_IOCTL, 1);
	return res;
}

int Func_Of_Proc_Chip_Hw_Config_Gpio(PNIC Nic, u8 gpio_num, bool isOutput)
{
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s gpio_num =%d direction=%d\n", __FUNCTION__, gpio_num, isOutput));

	do_deny_ps(Nic, PS_DENY_IOCTL, 1);

	left_aps_direct(Nic, 1);

	if (isOutput) {
		wl_write8(Nic, REG_GPIO_PIN_CTRL + 2,
				   wl_read8(Nic, REG_GPIO_PIN_CTRL + 2) | BIT(gpio_num));
	} else {
		wl_write8(Nic, REG_GPIO_PIN_CTRL + 2,
				   wl_read8(Nic, REG_GPIO_PIN_CTRL + 2) & ~BIT(gpio_num));
	}

	do_cancel_deny_ps(Nic, PS_DENY_IOCTL, 1);

	return 0;
}

int Func_Of_Proc_Chip_Hw_Register_Gpio_Interrupt(PNIC Nic, int gpio_num,
									void (*callback) (u8 level))
{
	u8 value;
	u8 direction;
	PHAL_DATA_TYPE phal = GET_HAL_DATA(Nic);

	do_deny_ps(Nic, PS_DENY_IOCTL, 1);

	left_aps_direct(Nic, 1);

	direction =
		(wl_read8(Nic, REG_GPIO_PIN_CTRL + 2) & BIT(gpio_num)) >> gpio_num;
	if (direction) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s Can't register output gpio as interrupt.\n",
				   __FUNCTION__));
		return -1;
	}

	wl_write8(Nic, REG_GPIO_PIN_CTRL + 3,
			   wl_read8(Nic, REG_GPIO_PIN_CTRL + 3) | BIT(gpio_num));

	Nic->gpiointpriv.callback[gpio_num] = callback;

	value = wl_read8(Nic, REG_GPIO_PIN_CTRL) & BIT(gpio_num);
	Nic->gpiointpriv.interrupt_mode =
		wl_read8(Nic, REG_HSIMR + 2) ^ value;
	wl_write8(Nic, REG_GPIO_INTM, Nic->gpiointpriv.interrupt_mode);

	Nic->gpiointpriv.interrupt_enable_mask =
		wl_read8(Nic, REG_HSIMR + 2) | BIT(gpio_num);
	wl_write8(Nic, REG_HSIMR + 2,
			   Nic->gpiointpriv.interrupt_enable_mask);

	Func_Of_Proc_Chip_Hw_Update_Hisr_Hsisr_Ind(Nic, 1);

	do_cancel_deny_ps(Nic, PS_DENY_IOCTL, 1);

	return 0;
}

int Func_Of_Proc_Chip_Hw_Disable_Gpio_Interrupt(PNIC Nic, int gpio_num)
{
	u8 value;
	u8 direction;
	PHAL_DATA_TYPE phal = GET_HAL_DATA(Nic);

	do_deny_ps(Nic, PS_DENY_IOCTL, 1);

	left_aps_direct(Nic, 1);

	wl_write8(Nic, REG_GPIO_PIN_CTRL + 3,
			   wl_read8(Nic, REG_GPIO_PIN_CTRL + 3) & ~BIT(gpio_num));

	Nic->gpiointpriv.callback[gpio_num] = NULL;

	Nic->gpiointpriv.interrupt_mode =
		wl_read8(Nic, REG_GPIO_INTM) & ~BIT(gpio_num);
	wl_write8(Nic, REG_GPIO_INTM, 0x00);

	Nic->gpiointpriv.interrupt_enable_mask =
		wl_read8(Nic, REG_HSIMR + 2) & ~BIT(gpio_num);
	wl_write8(Nic, REG_HSIMR + 2,
			   Nic->gpiointpriv.interrupt_enable_mask);

	if (!Nic->gpiointpriv.interrupt_enable_mask)
		Func_Of_Proc_Chip_Hw_Update_Hisr_Hsisr_Ind(Nic, 0);

	do_cancel_deny_ps(Nic, PS_DENY_IOCTL, 1);

	return 0;
}
#endif

s8 Func_Of_Proc_Chip_Hw_Ch_Sw_Iqk_Info_Search(PNIC Nic, u8 central_chnl,
								 u8 bw_mode)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u8 i;

	for (i = 0; i < MAX_IQK_INFO_BACKUP_CHNL_NUM; i++) {
		if ((pHalData->iqk_reg_backup[i].central_chnl != 0)) {
			if ((pHalData->iqk_reg_backup[i].central_chnl == central_chnl)
				&& (pHalData->iqk_reg_backup[i].bw_mode == bw_mode)) {
				return i;
			}
		}
	}

	return -1;
}

void Func_Of_Proc_Chip_Hw_Ch_Sw_Iqk_Info_Backup(PNIC Nic)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	s8 res;
	u8 i;

	res =
		Func_Of_Proc_Chip_Hw_Ch_Sw_Iqk_Info_Search(Nic, pHalData->CurrentChannel,
									  pHalData->CurrentChannelBW);
	if ((res >= 0) && (res < MAX_IQK_INFO_BACKUP_CHNL_NUM)) {
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_CH_SW_IQK_INFO_BACKUP,
						  (u8 *) & (pHalData->iqk_reg_backup[res]));
		return;
	}

	for (i = 0; i < MAX_IQK_INFO_BACKUP_CHNL_NUM; i++) {
		if (pHalData->iqk_reg_backup[i].central_chnl == 0) {
			Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_CH_SW_IQK_INFO_BACKUP,
							  (u8 *) & (pHalData->iqk_reg_backup[i]));
			return;
		}
	}

	for (i = 1; i < MAX_IQK_INFO_BACKUP_CHNL_NUM; i++) {
		Func_Of_Proc_Pre_Memcpy(&(pHalData->iqk_reg_backup[i - 1]),
					&(pHalData->iqk_reg_backup[i]),
					sizeof(struct hal_iqk_reg_backup));
	}
	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_CH_SW_IQK_INFO_BACKUP,
					  (u8 *) & (pHalData->
								iqk_reg_backup[MAX_IQK_INFO_BACKUP_CHNL_NUM -
											   1]));
}

void Func_Of_Proc_Chip_Hw_Ch_Sw_Iqk_Info_Restore(PNIC Nic, u8 ch_sw_use_case)
{
	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_CH_SW_IQK_INFO_RESTORE, &ch_sw_use_case);
}

void Func_Of_Proc_Dump_Mac_Rx_Counters(PNIC Nic,
							  struct dbg_rx_counter *rx_counter)
{
	u32 outbuff[6];

	if (!rx_counter) {
		wl_warn_on(1);
		return;
	}

	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_MP_MACRXCOUNT, NULL, outbuff, 0, 6)) {
		return;
	}

	rx_counter->rx_pkt_ok = outbuff[0];
	rx_counter->rx_pkt_crc_error = outbuff[1];
	rx_counter->rx_cck_fa = outbuff[2];
	rx_counter->rx_ofdm_fa = outbuff[3];
	rx_counter->rx_ht_fa = outbuff[4];
	rx_counter->rx_pkt_drop = outbuff[5];
}

void Func_Of_Proc_Dump_Phy_Rx_Counters(PNIC Nic,
							  struct dbg_rx_counter *rx_counter)
{
	u32 outbuff[4];

	if (!rx_counter) {
		wl_warn_on(1);
		return;
	}

	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_MP_PHYRXCOUNT, NULL, outbuff, 0, 4)) {
		return;
	}

	rx_counter->rx_pkt_ok = outbuff[0];
	rx_counter->rx_pkt_crc_error = outbuff[1];
	rx_counter->rx_ofdm_fa = outbuff[2];
	rx_counter->rx_cck_fa = outbuff[3];
}

#ifdef DBG_RX_COUNTER_DUMP
void Func_Of_Proc_Dump_Drv_Rx_Counters(PNIC Nic,
							  struct dbg_rx_counter *rx_counter)
{
	struct recv_priv *precvpriv = &Nic->recvpriv;
	if (!rx_counter) {
		wl_warn_on(1);
		return;
	}
	rx_counter->rx_pkt_ok = Nic->drv_rx_cnt_ok;
	rx_counter->rx_pkt_crc_error = Nic->drv_rx_cnt_crcerror;
	rx_counter->rx_pkt_drop = precvpriv->rx_drop - Nic->drv_rx_cnt_drop;
}

static void Func_Of_Proc_Reset_Drv_Rx_Counters(PNIC Nic)
{
	struct recv_priv *precvpriv = &Nic->recvpriv;
	Nic->drv_rx_cnt_ok = 0;
	Nic->drv_rx_cnt_crcerror = 0;
	Nic->drv_rx_cnt_drop = precvpriv->rx_drop;
}

void Func_Of_Proc_Dump_Rx_Counters(PNIC Nic)
{
	struct dbg_rx_counter rx_counter;
	u32 flag;

	if (Nic->dump_rx_cnt_mode & DUMP_DRV_RX_COUNTER) {
		memset(&rx_counter, 0, sizeof(struct dbg_rx_counter));
		Func_Of_Proc_Dump_Drv_Rx_Counters(Nic, &rx_counter);
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Drv Received packet OK:%d CRC error:%d Drop Packets: %d\n",
				rx_counter.rx_pkt_ok, rx_counter.rx_pkt_crc_error,
				rx_counter.rx_pkt_drop));

		Func_Of_Proc_Reset_Drv_Rx_Counters(Nic);
	}

	if (Nic->dump_rx_cnt_mode & DUMP_MAC_RX_COUNTER) {
		memset(&rx_counter, 0, sizeof(struct dbg_rx_counter));
		Func_Of_Proc_Dump_Mac_Rx_Counters(Nic, &rx_counter);
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Mac Received packet OK:%d CRC error:%d FA Counter: %d Drop Packets: %d\n",
			 rx_counter.rx_pkt_ok, rx_counter.rx_pkt_crc_error,
			 rx_counter.rx_cck_fa + rx_counter.rx_ofdm_fa + rx_counter.rx_ht_fa,
			 rx_counter.rx_pkt_drop));

		flag = 0x01;
		if (!Func_Mcu_Universal_Func_Interface
			(Nic, UMSG_OPS_MP_RESETCOUNT, &flag, NULL, 1, 0)) {
			return;
		}
	}

	if (Nic->dump_rx_cnt_mode & DUMP_PHY_RX_COUNTER) {
		memset(&rx_counter, 0, sizeof(struct dbg_rx_counter));
		Func_Of_Proc_Dump_Phy_Rx_Counters(Nic, &rx_counter);
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Phy Received packet OK:%d CRC error:%d FA Counter: %d\n",
				rx_counter.rx_pkt_ok, rx_counter.rx_pkt_crc_error,
				rx_counter.rx_ofdm_fa + rx_counter.rx_cck_fa));

		flag = 0x02;
		if (!Func_Mcu_Universal_Func_Interface
			(Nic, UMSG_OPS_MP_RESETCOUNT, &flag, NULL, 1, 0)) {
			return;
		}
	}
}
#endif
void Func_Of_Proc_Get_Noise(PNIC Nic)
{

}

u8 Func_Of_Proc_Get_Current_Tx_Rate(PNIC Nic, u8 macid)
{
	u8 rate_id = 0;

	return rate_id;
}

#ifdef CONFIG_FW_wMBOX0_DEBUG

void Func_Debug_Fwc2H(PNIC Nic, u8 * pdata, u8 len)
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

		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("[WPKFW, SEQ=%d]: %s", seq_no, buf));
		
		data_len += 3;
		total_length += data_len;

		if (more_data == TRUE) {
			memset(buf, '\0', 128);
			cnt = 0;
			nextdata = (pdata + total_length);
		}
	} while (more_data == TRUE);
}
#endif
void Func_Update_Iot_Info(PNIC Nic)
{
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	switch (pmlmeinfo->assoc_AP_vendor) {
	case HT_IOT_PEER_MARVELL:
		pmlmeinfo->turboMode_cts2self = 1;
		pmlmeinfo->turboMode_rtsen = 0;
		break;

	case HT_IOT_PEER_RALINK:
		pmlmeinfo->turboMode_cts2self = 0;
		pmlmeinfo->turboMode_rtsen = 1;
		wl_phydm_func_clr(Nic, MSG_BB_DYNAMIC_TXPWR);
		break;
	case HT_IOT_PEER_WK_WLAN:
		wl_phydm_func_clr(Nic, MSG_BB_DYNAMIC_TXPWR);
		break;
	default:
		pmlmeinfo->turboMode_cts2self = 0;
		pmlmeinfo->turboMode_rtsen = 1;
		break;
	}

}

#ifdef CONFIG_AUTO_CHNL_SEL_NHM
void Func_Of_Proc_Acs_Start(PNIC Nic, bool bStart)
{
	if (TRUE == bStart) {
		ACS_OP acs_op = ACS_INIT;

		Func_Of_Proc_Chip_Hw_Set_Odm_Var(Nic, HAL_MSG_AUTO_CHNL_SEL, &acs_op, TRUE);
		wl_set_acs_channel(Nic, 0);
		SET_ACS_STATE(Nic, ACS_ENABLE);
	} else {
		SET_ACS_STATE(Nic, ACS_DISABLE);
#ifdef DBG_AUTO_CHNL_SEL_NHM
		if (1) {
			u8 best_24g_ch = 0;

			Func_Of_Proc_Chip_Hw_Get_Odm_Var(Nic, HAL_MSG_AUTO_CHNL_SEL,
								&(best_24g_ch));
		    MpTrace(COMP_RICHD, DBG_NORMAL, 
			        ("[ACS-" ADPT_FMT "] Best 2.4G CH:%u\n", ADPT_ARG(Nic),
					best_24g_ch));
		}
#endif
	}
}
#endif

void Func_Chip_Hw_Set_Crystal_Cap(PNIC Nic, u8 crystal_cap)
{
	crystal_cap = crystal_cap & 0x3F;

}

#if 0
static const char *const _band_cap_str[] = {
	"2G",
};

static const char *const _bw_cap_str[] = {
	"5M",
	"10M",
	"20M",
	"40M",
};

static const char *const _proto_cap_str[] = {
	"b",
	"g",
	"n",
};

static const char *const _wl_func_str[] = {
	"P2P",
	"MIRACAST",
	"TDLS",
	"FTM",
};


void Func_Dump_Hal_Spec(void *sel, PNIC Nic)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(wadptdata);
	int i;

	WL_INFO_L1("macid_num:%u\n", hal_spec->macid_num);
	WL_INFO_L1("sec_cap:0x%02x\n", hal_spec->sec_cap);
	WL_INFO_L1("sec_cam_ent_num:%u\n", hal_spec->sec_cam_ent_num);
	WL_INFO_L1("nss_num:%u\n", hal_spec->nss_num);

	WL_INFO_L1("band_cap:");
	for (i = 0; i < BAND_CAP_BIT_NUM; i++) {
		if (((hal_spec->band_cap) >> i) & BIT0 && _band_cap_str[i])
			WL_INFO_L2("%s ", _band_cap_str[i]);
	}
	WL_INFO_L2("\n");

	WL_INFO_L1("bw_cap:");
	for (i = 0; i < BW_CAP_BIT_NUM; i++) {
		if (((hal_spec->bw_cap) >> i) & BIT0 && _bw_cap_str[i])
			WL_INFO_L2("%s ", _bw_cap_str[i]);
	}
	WL_INFO_L2("\n");

	WL_INFO_L1("proto_cap:");
	for (i = 0; i < PROTO_CAP_BIT_NUM; i++) {
		if (((hal_spec->proto_cap) >> i) & BIT0 && _proto_cap_str[i])
			WL_INFO_L2("%s ", _proto_cap_str[i]);
	}
	WL_INFO_L2("\n");

	WL_INFO_L1("wl_func:");
	for (i = 0; i < WL_FUNC_BIT_NUM; i++) {
		if (((hal_spec->wl_func) >> i) & BIT0 && _wl_func_str[i])
			WL_INFO_L2("%s ", _wl_func_str[i]);
	}
	WL_INFO_L2("\n");
}
#endif

//delete inline in win
bool Func_Chip_Hw_Chk_Band_Cap(PNIC Nic, u8 cap)
{
	return (GET_HAL_SPEC(Nic)->band_cap & cap);
}

bool Func_Chip_Hw_Chk_Bw_Cap(PNIC Nic, u8 cap)
{
	return (GET_HAL_SPEC(Nic)->bw_cap & cap);
}

bool Func_Chip_Hw_Chk_Proto_Cap(PNIC Nic, u8 cap)
{
	return (GET_HAL_SPEC(Nic)->proto_cap & cap);
}

bool Func_Chip_Hw_Chk_Wl_Func(PNIC Nic, u8 func)
{
	return (GET_HAL_SPEC(Nic)->wl_func & func);
}

bool Func_Chip_Hw_Is_Band_Support(PNIC Nic, u8 band)
{
	return (GET_HAL_SPEC(Nic)->band_cap & band_to_band_cap(band));
}

bool Func_Chip_Hw_Is_Bw_Support(PNIC Nic, u8 bw)
{
	return (GET_HAL_SPEC(Nic)->bw_cap & ch_width_to_bw_cap(bw));
}

bool Func_Chip_Hw_Is_Wireless_Mode_Support(PNIC Nic, u8 mode)
{
	u8 proto_cap = GET_HAL_SPEC(Nic)->proto_cap;

	if (mode == WIRELESS_11B)
		if ((proto_cap & PROTO_CAP_11B)
			&& Func_Chip_Hw_Chk_Band_Cap(Nic, BAND_CAP_2G))
			return 1;

	if (mode == WIRELESS_11G)
		if ((proto_cap & PROTO_CAP_11G)
			&& Func_Chip_Hw_Chk_Band_Cap(Nic, BAND_CAP_2G))
			return 1;

	if (mode == WIRELESS_11_24N)
		if ((proto_cap & PROTO_CAP_11N)
			&& Func_Chip_Hw_Chk_Band_Cap(Nic, BAND_CAP_2G))
			return 1;

	return 0;
}

u8 Func_Chip_Hw_Largest_Bw(PNIC Nic, u8 in_bw)
{
	for (; in_bw > CHANNEL_WIDTH_20; in_bw--) {
		if (Func_Chip_Hw_Is_Bw_Support(Nic, in_bw))
			break;
	}

	if (!Func_Chip_Hw_Is_Bw_Support(Nic, in_bw))
		wl_warn_on(1);

	return in_bw;
}
