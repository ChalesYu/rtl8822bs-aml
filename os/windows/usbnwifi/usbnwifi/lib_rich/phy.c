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


#define MAX_PRECMD_CNT 16
#define MAX_RFDEPENDCMD_CNT 16
#define MAX_POSTCMD_CNT 16

#define MAX_DOZE_WAITING_TIMES_9x 64

///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////
VOID
Func_Chip_Bb_Settxpower_Index_Process(IN PNIC Nic,
						  IN u32 PowerIndex, IN u8 RFPath, IN u8 Rate)
{
	u32 inbuff[3] = { 0 };
	u32 outbuff = 0;

	inbuff[0] = RFPath;
	inbuff[1] = Rate;
	inbuff[2] = PowerIndex;
	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_MP_SET_TXPOWERINDEX, inbuff, &outbuff, 3, 1)) {
		
		MpTrace(COMP_EVENTS, DBG_NORMAL, ("Error :===>%s,fail,code :UMSG_OPS_MP_SET_TXPOWERINDEX!!!\n",
				__func__));

		return;
	}

	if (outbuff == 1) {
		MpTrace(COMP_EVENTS, DBG_NORMAL, ("Invalid Rate!!\n"));
	} else if (outbuff == 2) {
		MpTrace(COMP_EVENTS, DBG_NORMAL, ("Invalid RFPath!!\n"));
	}
}

///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////
u8
Func_Chip_Bb_Gettxpowerindex_Process(IN PNIC Nic,
						  IN u8 RFPath,
						  IN u8 Rate, IN CHANNEL_WIDTH BandWidth, IN u8 Channel)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	s8 txPower = 0, powerDiffByRate = 0, limit = 0;
	BOOLEAN bIn24G = _FALSE;

	txPower =
		(s8) Func_Chip_Bb_Gettxpowerindexbase(Nic, RFPath, Rate, BandWidth, Channel,
									 &bIn24G);
	powerDiffByRate =
		Func_Chip_Bb_Pre_Gettxpowerbyrate(Nic, BAND_ON_2_4G, MSG_RF_PATH_A, RF_1TX,
							 Rate);

	limit =
		Func_Chip_Bb_Gettxpowerlimit(Nic, Nic->registrypriv.RegPwrTblSel,
							(u8) (!bIn24G), pHalData->CurrentChannelBW, RFPath,
							Rate, pHalData->CurrentChannel);

	powerDiffByRate = powerDiffByRate > limit ? limit : powerDiffByRate;
	txPower += powerDiffByRate;

	txPower += Func_Chip_Bb_Gettxpowertrackingoffset(Nic, RFPath, Rate);

	if (txPower > MAX_POWER_INDEX)
		txPower = MAX_POWER_INDEX;

	return (u8) txPower;
}
///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////
VOID Func_Chip_Bb_Settxpower_Level_Process(IN PNIC Nic, IN u8 Channel)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	u8 cur_antenna;
	u8 RFPath = MSG_RF_PATH_A;

	RFPath = pHalData->ant_path;
	
	MpTrace(COMP_EVENTS, DBG_NORMAL, ("==>Func_Chip_Bb_Settxpower_Level_Process()\n"));

	Func_Chip_Bb_Settxpowerlevelbypath(Nic, Channel, RFPath);
	
	MpTrace(COMP_EVENTS, DBG_NORMAL, ("<==Func_Chip_Bb_Settxpower_Level_Process()\n"));
}
///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////
VOID
Func_Chip_Bb_Handle_Sw_Chnl_And_Set_Bw_Process(IN PNIC Nic,
							  IN BOOLEAN bSwitchChannel,
							  IN BOOLEAN bSetBandWidth,
							  IN u8 ChannelNum,
							  IN CHANNEL_WIDTH ChnlWidth,
							  IN EXTCHNL_OFFSET ExtChnlOffsetOf40MHz,
							  IN u8 CenterFrequencyIndex1)
{
	int ret = FALSE;
	u4Byte arg[7] = { 0 };
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	u8 tmpChannel = pHalData->CurrentChannel;
	CHANNEL_WIDTH tmpBW = pHalData->CurrentChannelBW;
	u8 tmpnCur40MhzPrimeSC = pHalData->nCur40MhzPrimeSC;
	u8 tmpCenterFrequencyIndex1 = pHalData->CurrentCenterFrequencyIndex1;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;

	if (!bSwitchChannel && !bSetBandWidth) {
		MpTrace(COMP_EVENTS, DBG_NORMAL, ("Func_Chip_Bb_Handle_Sw_Chnl_And_Set_Bw_Process:  not switch channel and not set bandwidth\n"));
		return;
	}


	if (bSwitchChannel) {
		{
			if (Func_Chip_Hw_Islegalchannel(Nic, ChannelNum))
				pHalData->bSwChnl = _TRUE;
		}
	}

	if (bSetBandWidth) {
		pHalData->bSetChnlBW = _TRUE;
	}

	if (!pHalData->bSetChnlBW && !pHalData->bSwChnl) {
		return;
	}

	if (pHalData->bSwChnl) {
		pHalData->CurrentChannel = ChannelNum;
		pHalData->CurrentCenterFrequencyIndex1 = ChannelNum;
	}

	if (pHalData->bSetChnlBW) {
		pHalData->CurrentChannelBW = ChnlWidth;
		pHalData->nCur40MhzPrimeSC = ExtChnlOffsetOf40MHz;
		pHalData->CurrentCenterFrequencyIndex1 = CenterFrequencyIndex1;

	}

	if (!WL_CANNOT_RUN(Nic)) {
		arg[0] = pHalData->RfRegChnlVal[0];
		arg[1] = pHalData->CurrentChannel;
		arg[2] = pHalData->CurrentChannelBW;
		arg[3] = pHalData->nCur40MhzPrimeSC;
		arg[4] = pHalData->bSwChnl;
		arg[5] = pHalData->bSetChnlBW;
		arg[6] = pHalData->bSetPowLevel;

		ret =
			Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_CHNLBW_MODE, arg,
										NULL, 7, 0);
		if (!ret) {
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("Error :===>%s,fail,code :UMSG_OPS_HAL_CHNLBW_MODE!!!\n",
					__func__));
			return;
		}
	} else {
		if (pHalData->bSwChnl) {
			pHalData->CurrentChannel = tmpChannel;
			pHalData->CurrentCenterFrequencyIndex1 = tmpChannel;
		}
		if (pHalData->bSetChnlBW) {
			pHalData->CurrentChannelBW = tmpBW;
			pHalData->nCur40MhzPrimeSC = tmpnCur40MhzPrimeSC;
			pHalData->CurrentCenterFrequencyIndex1 = tmpCenterFrequencyIndex1;
		}
	}

}
