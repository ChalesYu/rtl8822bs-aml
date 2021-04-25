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
//#include <pdata.h>
#include "globals.h"
#include "ndis.h"
#include "hw_pcomp.h"
#include "hw_main.h"
#include "hw_Nic.h"
#include "hw_func.h"
#include "usb_main.h"
#include "bus_intf.h"
#include "8187_gen.h"



static s8
Func_Chip_Bb_Gettxpowerbyrate(IN PNIC Nic,
					  IN u8 Band, IN u8 RFPath, IN u8 TxNum, IN u8 Rate);

static s8 Func_Of_Proc_Regsty_Get_Target_Tx_Power(IN PNIC Nic,
								  IN u8 Band,
								  IN u8 RfPath, IN RATE_SECTION RateSection)
{
	struct registry_priv *regsty = &Nic->registrypriv;
	s8 value = 0;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s RateSection:%d in %sG, RfPath:%d\n", __func__,
			   RateSection, (Band == BAND_ON_2_4G) ? "2.4" : "5", RfPath));

	if (RfPath > 0) {
		return -1;
	}

	if (Band != BAND_ON_2_4G) {
		return -1;
	}

	if (RateSection >= RATE_SECTION_NUM) {
		return -1;
	}

	if (Band == BAND_ON_2_4G)
		value = regsty->target_tx_pwr_2g[RfPath][RateSection];

	return value;
}

bool Func_Of_Proc_Regsty_Chk_Target_Tx_Power_Valid(PNIC Nic)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(Nic);
//	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
	int tx_num, rs;
	s8 target;

	for (rs = 0; rs < RATE_SECTION_NUM; rs++) {
		tx_num = rate_section_to_tx_num(rs);
		if (tx_num >= hal_spec->nss_num)
			continue;

		target = Func_Of_Proc_Regsty_Get_Target_Tx_Power(Nic, BAND_ON_2_4G, 0, rs);
		if (target == -1)
			return _FALSE;
	}

	return _TRUE;
}

static u8
Func_Chip_Bb_Gettxpowerbyratebase(IN PNIC Nic,
						 IN u8 Band,
						 IN u8 RfPath, IN u8 TxNum, IN RATE_SECTION RateSection)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u8 value = 0;

	if (Band != BAND_ON_2_4G) {
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s invalid Band:%d\n", __func__, Band));
		return 0;
	}

	if (RateSection >= RATE_SECTION_NUM) {
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s invalid RateSection:%d in %sG, RfPath:%d, TxNum:%d\n",
				   __func__, RateSection, (Band == BAND_ON_2_4G) ? "2.4" : "5",
				   RfPath, TxNum));
		return 0;
	}

	if (Band == BAND_ON_2_4G)
		value = pHalData->TxPwrByRateBase2_4G[RfPath][TxNum][RateSection];

	return value;
}

VOID
Func_Chip_Bb_Settxpowerbyratebase(IN PNIC Nic,
						 IN u8 Band,
						 IN u8 RfPath,
						 IN RATE_SECTION RateSection, IN u8 TxNum, IN u8 Value)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	if (Band != BAND_ON_2_4G) {
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s invalid Band:%d\n", __func__, Band));
		return;
	}

	if (RateSection >= RATE_SECTION_NUM) {
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s invalid RateSection:%d in %sG, RfPath:%d, TxNum:%d\n",
				   __func__, RateSection, (Band == BAND_ON_2_4G) ? "2.4" : "5",
				   RfPath, TxNum));
		return;
	}

	if (Band == BAND_ON_2_4G)
		pHalData->TxPwrByRateBase2_4G[RfPath][TxNum][RateSection] = Value;
}

u8 Func_Chip_Bb_Get_Target_Tx_Power(IN PNIC Nic,
						   IN u8 Band,
						   IN u8 RfPath, IN RATE_SECTION RateSection)
{
	struct registry_priv *regsty = &Nic->registrypriv;
	s16 target_power;

	if (Func_Chip_Bb_Is_Tx_Power_By_Rate_Needed(Nic) == _FALSE
		&& regsty->target_tx_pwr_valid == _TRUE)
		target_power =
			2 * Func_Of_Proc_Regsty_Get_Target_Tx_Power(Nic, Band, RfPath,
											   RateSection);
	else
		target_power =
			Func_Chip_Bb_Gettxpowerbyratebase(Nic, Band, RfPath,
									 rate_section_to_tx_num(RateSection),
									 RateSection);

	return target_power;
}

static VOID Func_Chip_Bb_Storetxpowerbyratebase(IN PNIC Nic)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(Nic);

	u8 rate_sec_base[RATE_SECTION_NUM] = {
		MGN_11M,
		MGN_54M,
		MGN_MCS7,
	};

	u8 rs, tx_num, base, index;

	for (rs = 0; rs < RATE_SECTION_NUM; rs++) {
		tx_num = rate_section_to_tx_num(rs);
		if (tx_num >= hal_spec->nss_num)
			continue;

		base =
			Func_Chip_Bb_Gettxpowerbyrate(Nic, BAND_ON_2_4G, 0, tx_num,
								  rate_sec_base[rs]);
		Func_Chip_Bb_Settxpowerbyratebase(Nic, BAND_ON_2_4G, 0, rs, tx_num, base);
	}
}

VOID Func_Chip_Bb_Inittxpowerbyrate(IN PNIC Nic)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u8 TxNum = 0, rate = 0, i = 0, j = 0;

	for (rate = 0; rate < TX_PWR_BY_RATE_NUM_RATE; ++rate)
		pHalData->TxPwrByRateOffset[BAND_ON_2_4G][0][0][rate] = 0;
}

static VOID Func_Chip_Bb_Converttxpowerbyrateindbmtorelativevalues(IN PNIC Nic)
{
//	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u8 base = 0, i = 0, value = 0,
		band = 0, path = 0, txNum = 0, index = 0, startIndex = 0, endIndex = 0;
	u8 cckRates[4] = { MGN_1M, MGN_2M, MGN_5_5M, MGN_11M }
	, ofdmRates[8] = {
	MGN_6M, MGN_9M, MGN_12M, MGN_18M, MGN_24M, MGN_36M, MGN_48M, MGN_54M}
	, mcs0_7Rates[8] = {
	MGN_MCS0, MGN_MCS1, MGN_MCS2, MGN_MCS3, MGN_MCS4, MGN_MCS5, MGN_MCS6,
			MGN_MCS7};

	base =
		Func_Chip_Bb_Pre_Gettxpowerbyrate(Nic, BAND_ON_2_4G, MSG_RF_PATH_A, RF_1TX,
							 MGN_11M);
	for (i = 0; i < sizeof(cckRates); ++i) {
		value =
			Func_Chip_Bb_Pre_Gettxpowerbyrate(Nic, BAND_ON_2_4G, MSG_RF_PATH_A, RF_1TX,
								 cckRates[i]);
		Func_Chip_Bb_Settxpowerbyrate(Nic, BAND_ON_2_4G, MSG_RF_PATH_A, RF_1TX,
							 cckRates[i], value - base);
	}

	base =
		Func_Chip_Bb_Pre_Gettxpowerbyrate(Nic, BAND_ON_2_4G, MSG_RF_PATH_A, RF_1TX,
							 MGN_54M);
	for (i = 0; i < sizeof(ofdmRates); ++i) {
		value =
			Func_Chip_Bb_Pre_Gettxpowerbyrate(Nic, BAND_ON_2_4G, MSG_RF_PATH_A, RF_1TX,
								 ofdmRates[i]);
		Func_Chip_Bb_Settxpowerbyrate(Nic, BAND_ON_2_4G, MSG_RF_PATH_A, RF_1TX,
							 ofdmRates[i], value - base);
	}

	base =
		Func_Chip_Bb_Pre_Gettxpowerbyrate(Nic, BAND_ON_2_4G, MSG_RF_PATH_A, RF_1TX,
							 MGN_MCS7);
	for (i = 0; i < sizeof(mcs0_7Rates); ++i) {
		value =
			Func_Chip_Bb_Pre_Gettxpowerbyrate(Nic, BAND_ON_2_4G, MSG_RF_PATH_A, RF_1TX,
								 mcs0_7Rates[i]);
		Func_Chip_Bb_Settxpowerbyrate(Nic, BAND_ON_2_4G, MSG_RF_PATH_A, RF_1TX,
							 mcs0_7Rates[i], value - base);
	}

}

static VOID Func_Chip_Bb_Txpowerbyrateconfiguration(IN PNIC Nic)
{
//	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	Func_Chip_Bb_Storetxpowerbyratebase(Nic);
	Func_Chip_Bb_Converttxpowerbyrateindbmtorelativevalues(Nic);
}

VOID
Func_Chip_Bb_Settxpowerindexbyratesection(IN PNIC Nic,
								 IN u8 RFPath, IN u8 Channel, IN u8 RateSection)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

	if (RateSection == CCK) {
		u8 cckRates[] = { MGN_1M, MGN_2M, MGN_5_5M, MGN_11M };
		if (pHalData->CurrentBandType == BAND_ON_2_4G)
			Func_Chip_Bb_Settxpowerindexbyratearray(Nic, RFPath,
										   pHalData->CurrentChannelBW, Channel,
										   cckRates,
										   sizeof(cckRates) / sizeof(u8));

	} else if (RateSection == OFDM) {
		u8 ofdmRates[] =
			{ MGN_6M, MGN_9M, MGN_12M, MGN_18M, MGN_24M, MGN_36M, MGN_48M,
   MGN_54M };
		Func_Chip_Bb_Settxpowerindexbyratearray(Nic, RFPath,
									   pHalData->CurrentChannelBW, Channel,
									   ofdmRates,
									   sizeof(ofdmRates) / sizeof(u8));

	} else if (RateSection == HT_MCS0_MCS7) {
		u8 htRates1T[] =
			{ MGN_MCS0, MGN_MCS1, MGN_MCS2, MGN_MCS3, MGN_MCS4, MGN_MCS5,
   MGN_MCS6, MGN_MCS7 };
		Func_Chip_Bb_Settxpowerindexbyratearray(Nic, RFPath,
									   pHalData->CurrentChannelBW, Channel,
									   htRates1T,
									   sizeof(htRates1T) / sizeof(u8));

	} else {
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Invalid RateSection %d in %s", RateSection, __FUNCTION__));
	}
}

static BOOLEAN Func_Chip_Bb_Getchnlindex(IN u8 Channel, OUT u8 * ChannelIdx)
{
	u8 i = 0;
	BOOLEAN bIn24G = _TRUE;

	if (Channel <= 14) {
		bIn24G = _TRUE;
		*ChannelIdx = Channel - 1;
	}

	return bIn24G;
}

u8
Func_Chip_Bb_Gettxpowerindexbase(IN PNIC Nic,
						IN u8 RFPath,
						IN u8 Rate,
						IN CHANNEL_WIDTH BandWidth,
						IN u8 Channel, OUT PBOOLEAN bIn24G)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	u8 i = 0;
	u8 txPower = 0;
	u8 chnlIdx = (Channel - 1);

	if (Func_Chip_Hw_Islegalchannel(Nic, Channel) == _FALSE) {
		chnlIdx = 0;
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Illegal channel!!\n"));
	}

	*bIn24G = Func_Chip_Bb_Getchnlindex(Channel, &chnlIdx);

	if (*bIn24G) {
		if (IS_CCK_RATE(Rate)) {
			txPower = pHalData->Index24G_CCK_Base[RFPath][chnlIdx];
		} else if (MGN_6M <= Rate) {
			txPower = pHalData->Index24G_BW40_Base[RFPath][chnlIdx];
		} else {
			MpTrace(COMP_RICHD, DBG_NORMAL, 
				("Func_Chip_Bb_Gettxpowerindexbase: INVALID Rate.\n"));
		}

		if ((MGN_6M <= Rate && Rate <= MGN_54M) && !IS_CCK_RATE(Rate)) {
			txPower += pHalData->OFDM_24G_Diff[RFPath][0];
		}
		if (BandWidth == CHANNEL_WIDTH_20) {
			if (MGN_MCS0 <= Rate && Rate <= MGN_MCS7)
				txPower += pHalData->BW20_24G_Diff[RFPath][0];

		} else if (BandWidth == CHANNEL_WIDTH_40) {
			if (MGN_MCS0 <= Rate && Rate <= MGN_MCS7)
				txPower += pHalData->BW40_24G_Diff[RFPath][0];

		}
	}

	return txPower;
}

static u8 Func_Chip_Bb_Getrateindexoftxpowerbyrate(IN u8 Rate)
{
	u8 index = 0;
	switch (Rate) {
	case MGN_1M:
		index = 0;
		break;
	case MGN_2M:
		index = 1;
		break;
	case MGN_5_5M:
		index = 2;
		break;
	case MGN_11M:
		index = 3;
		break;
	case MGN_6M:
		index = 4;
		break;
	case MGN_9M:
		index = 5;
		break;
	case MGN_12M:
		index = 6;
		break;
	case MGN_18M:
		index = 7;
		break;
	case MGN_24M:
		index = 8;
		break;
	case MGN_36M:
		index = 9;
		break;
	case MGN_48M:
		index = 10;
		break;
	case MGN_54M:
		index = 11;
		break;
	case MGN_MCS0:
		index = 12;
		break;
	case MGN_MCS1:
		index = 13;
		break;
	case MGN_MCS2:
		index = 14;
		break;
	case MGN_MCS3:
		index = 15;
		break;
	case MGN_MCS4:
		index = 16;
		break;
	case MGN_MCS5:
		index = 17;
		break;
	case MGN_MCS6:
		index = 18;
		break;
	case MGN_MCS7:
		index = 19;
		break;
	default:
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Invalid rate 0x%x in %s\n", Rate, __FUNCTION__));
		break;
	};

	return index;
}

static s8
Func_Chip_Bb_Gettxpowerbyrate(IN PNIC Nic,
					  IN u8 Band, IN u8 RFPath, IN u8 TxNum, IN u8 Rate)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	s8 value = 0;
	u8 rateIndex = Func_Chip_Bb_Getrateindexoftxpowerbyrate(Rate);

	if (Band != BAND_ON_2_4G) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Invalid band %d in %s\n", Band, __func__));
		goto exit;
	}

	if (TxNum >= RF_MAX_TX_NUM) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Invalid TxNum %d in %s\n", TxNum, __func__));
		goto exit;
	}
	if (rateIndex >= TX_PWR_BY_RATE_NUM_RATE) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Invalid RateIndex %d in %s\n", rateIndex, __func__));
		goto exit;
	}

	value = pHalData->TxPwrByRateOffset[Band][RFPath][TxNum][rateIndex];

exit:
	return value;
}

s8
Func_Chip_Bb_Pre_Gettxpowerbyrate(IN PNIC Nic,
					 IN u8 Band, IN u8 RFPath, IN u8 TxNum, IN u8 Rate)
{
	if (!Func_Chip_Bb_Is_Tx_Power_By_Rate_Needed(Nic))
		return 0;

	return Func_Chip_Bb_Gettxpowerbyrate(Nic, Band, RFPath, TxNum, Rate);
}

VOID
Func_Chip_Bb_Settxpowerbyrate(IN PNIC Nic,
					 IN u8 Band,
					 IN u8 RFPath, IN u8 TxNum, IN u8 Rate, IN s8 Value)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u8 rateIndex = Func_Chip_Bb_Getrateindexoftxpowerbyrate(Rate);

	if (Band != BAND_ON_2_4G) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Invalid band %d in %s\n", Band, __FUNCTION__));
		return;
	}

	if (TxNum >= RF_MAX_TX_NUM) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Invalid TxNum %d in %s\n", TxNum, __FUNCTION__));
		return;
	}
	if (rateIndex >= TX_PWR_BY_RATE_NUM_RATE) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Invalid RateIndex %d in %s\n", rateIndex, __FUNCTION__));
		return;
	}

	pHalData->TxPwrByRateOffset[Band][RFPath][TxNum][rateIndex] = Value;
}

VOID Func_Chip_Bb_Settxpowerlevelbypath(IN PNIC Nic, IN u8 channel, IN u8 path)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	BOOLEAN bIsIn24G = (pHalData->CurrentBandType == BAND_ON_2_4G);

	{
		if (bIsIn24G)
			Func_Chip_Bb_Settxpowerindexbyratesection(Nic, path, channel, CCK);

		Func_Chip_Bb_Settxpowerindexbyratesection(Nic, path, channel, OFDM);
		Func_Chip_Bb_Settxpowerindexbyratesection(Nic, path, channel, HT_MCS0_MCS7);
	}
}

VOID
Func_Chip_Bb_Settxpowerindexbyratearray(IN PNIC Nic,
							   IN u8 RFPath,
							   IN CHANNEL_WIDTH BandWidth,
							   IN u8 Channel,
							   IN u8 * Rates, IN u8 RateArraySize)
{
	u32 powerIndex = 0;
	int i = 0;

	for (i = 0; i < RateArraySize; ++i) {
		powerIndex =
			Func_Chip_Bb_Gettxpowerindex(Nic, RFPath, Rates[i], BandWidth, Channel);
		Func_Chip_Bb_Settxpowerindex(Nic, powerIndex, RFPath, Rates[i]);
	}
}

s8 Func_Chip_Bb_Getworldwidelimit(s8 * LimitTable)
{
	s8 min = LimitTable[0];
	u8 i = 0;

	for (i = 0; i < MAX_REGULATION_NUM; ++i) {
		if (LimitTable[i] < min)
			min = LimitTable[i];
	}

	return min;
}

static s8 Func_Chip_Bb_Getchannelindexoftxpowerlimit(IN u8 Band, IN u8 Channel)
{
	s8 channelIndex = -1;
	u8 i = 0;

	if (Band == BAND_ON_2_4G) {
		channelIndex = Channel - 1;
	} else {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Invalid Band %d in %s\n", Band, __func__));
	}

	if (channelIndex == -1)
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Invalid Channel %d of Band %d in %s\n", Channel, Band,
				   __func__));
	
	return channelIndex;
}

s8
Func_Chip_Bb_Gettxpowerlimit(IN PNIC Nic,
					IN u32 RegPwrTblSel,
					IN BAND_TYPE Band,
					IN CHANNEL_WIDTH Bandwidth,
					IN u8 RfPath, IN u8 DataRate, IN u8 Channel)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	s16 band = -1, regulation = -1, bandwidth = -1,
		rateSection = -1, channel = -1;
	s8 powerLimit = MAX_POWER_INDEX;

	if ((Nic->registrypriv.RegEnableTxPowerLimit == 2
		 && pHalData->EEPROMRegulatory != 1)
		|| Nic->registrypriv.RegEnableTxPowerLimit == 0)
		return MAX_POWER_INDEX;

	switch (RegPwrTblSel) {
	case 1:
		regulation = TXPWR_LMT_ETSI;
		break;
	case 2:
		regulation = TXPWR_LMT_MKK;
		break;
	case 3:
		regulation = TXPWR_LMT_FCC;
		break;
	case 4:
		regulation = TXPWR_LMT_WW;
		break;
	default:
		regulation = pHalData->Regulation2_4G;
		break;
	}

	if (Band == BAND_ON_2_4G)
		band = 0;

	if (Bandwidth == CHANNEL_WIDTH_20)
		bandwidth = 0;
	else if (Bandwidth == CHANNEL_WIDTH_40)
		bandwidth = 1;

	switch (DataRate) {
	case MGN_1M:
	case MGN_2M:
	case MGN_5_5M:
	case MGN_11M:
		rateSection = 0;
		break;

	case MGN_6M:
	case MGN_9M:
	case MGN_12M:
	case MGN_18M:
	case MGN_24M:
	case MGN_36M:
	case MGN_48M:
	case MGN_54M:
		rateSection = 1;
		break;

	case MGN_MCS0:
	case MGN_MCS1:
	case MGN_MCS2:
	case MGN_MCS3:
	case MGN_MCS4:
	case MGN_MCS5:
	case MGN_MCS6:
	case MGN_MCS7:
		rateSection = 2;
		break;

	default:
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Wrong rate 0x%x\n", DataRate));
		break;
	}

	if (rateSection == 1)
		bandwidth = 0;

	if (rateSection == 0)
		bandwidth = 0;

	if (Band == BAND_ON_2_4G)
		channel = Func_Chip_Bb_Getchannelindexoftxpowerlimit(BAND_ON_2_4G, Channel);

	if (band == -1 || regulation == -1 || bandwidth == -1 ||
		rateSection == -1 || channel == -1) {

		return MAX_POWER_INDEX;
	}

	if (Band == BAND_ON_2_4G) {
		s8 limits[10] = { 0 };
		u8 i = 0;
		if (bandwidth >= MAX_2_4G_BANDWIDTH_NUM)
			bandwidth = MAX_2_4G_BANDWIDTH_NUM - 1;
		for (i = 0; i < MAX_REGULATION_NUM; ++i)
			limits[i] =
				pHalData->
				TxPwrLimit_2_4G[i][bandwidth][rateSection][channel][RfPath];

		powerLimit =
			(regulation ==
			 TXPWR_LMT_WW) ? Func_Chip_Bb_Getworldwidelimit(limits) : pHalData->
			TxPwrLimit_2_4G[regulation][bandwidth][rateSection][channel]
			[RfPath];

	} else
		MpTrace(COMP_RICHD, DBG_NORMAL, ("No power limit table of the specified band\n"));

	return powerLimit;
}

static VOID Func_Chip_Bb_Converttxpowerlimittopowerindex(IN PNIC Nic)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u8 base;
	u8 regulation, bw, channel, rateSection;
	s8 tempValue = 0, tempPwrLmt = 0;
	u8 rfPath = 0;

	msg_get_var(Nic);
	if (msg_rw_val.PhyRegPgValueType != PHY_REG_PG_EXACT_VALUE) {
		wl_warn_on(1);
		return;
	}

	for (regulation = 0; regulation < MAX_REGULATION_NUM; ++regulation) {

		for (bw = 0; bw < MAX_2_4G_BANDWIDTH_NUM; ++bw) {

			for (channel = 0; channel < CENTER_CH_2G_NUM; ++channel) {

				for (rateSection = CCK; rateSection <= HT_1SS; ++rateSection) {
					tempPwrLmt =
						pHalData->
						TxPwrLimit_2_4G[regulation][bw][rateSection][channel]
						[0];

					if (tempPwrLmt != MAX_POWER_INDEX) {

						base =
							Func_Chip_Bb_Get_Target_Tx_Power(Nic, BAND_ON_2_4G, 0,
													rateSection);
						tempValue = tempPwrLmt - base;
						pHalData->
							TxPwrLimit_2_4G[regulation][bw][rateSection]
							[channel][0] = tempValue;
					}
				}
			}
		}
	}
}

VOID Func_Chip_Bb_Inittxpowerlimit(IN PNIC Nic)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u8 i, j, k, l, m;

	for (i = 0; i < MAX_REGULATION_NUM; ++i)
		for (j = 0; j < MAX_2_4G_BANDWIDTH_NUM; ++j)
			for (k = 0; k < MAX_RATE_SECTION_NUM; ++k)
				for (m = 0; m < CENTER_CH_2G_NUM; ++m)
					for (l = 0; l < 1; ++l)
						pHalData->TxPwrLimit_2_4G[i][j][k][m][l] =
							MAX_POWER_INDEX;

}

u8
Func_Chip_Bb_Gettxpowerindex(IN PNIC Nic,
					IN u8 RFPath,
					IN u8 Rate, IN CHANNEL_WIDTH BandWidth, IN u8 Channel)
{
	u8 txPower = 0x3E;

	txPower =
		Func_Chip_Bb_Gettxpowerindex_Process(Nic, RFPath, Rate, BandWidth, Channel);

	return txPower;
}

VOID
Func_Chip_Bb_Settxpowerindex(IN PNIC Nic,
					IN u32 PowerIndex, IN u8 RFPath, IN u8 Rate)
{
	Func_Chip_Bb_Settxpower_Index_Process(Nic, PowerIndex, RFPath, Rate);
}

bool Func_Chip_Bb_Is_Tx_Power_Limit_Needed(PNIC Nic)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
	struct registry_priv *regsty = &Nic->registrypriv;

	if (regsty->RegEnableTxPowerLimit == 1
		|| (regsty->RegEnableTxPowerLimit == 2
			&& hal_data->EEPROMRegulatory == 1))
		return _TRUE;
	return _FALSE;
}

bool Func_Chip_Bb_Is_Tx_Power_By_Rate_Needed(PNIC Nic)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
	struct registry_priv *regsty = &Nic->registrypriv;

	if (regsty->RegEnableTxPowerByRate == 1
		|| (regsty->RegEnableTxPowerByRate == 2
			&& hal_data->EEPROMRegulatory != 2))
		return _TRUE;
	return _FALSE;
}

int Func_Chip_Bb_Load_Tx_Power_By_Rate(PNIC Nic, u8 chk_file)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
//	struct registry_priv *regsty = &Nic->registrypriv;

	int ret = FALSE;

	hal_data->txpwr_by_rate_loaded = 0;
	Func_Chip_Bb_Inittxpowerbyrate(Nic);

	hal_data->txpwr_limit_loaded = 0;

#ifdef CONFIG_EMBEDDED_FWIMG
	if (HAL_STATUS_SUCCESS ==
		Func_Hw_Op_Configbbwithheaderfile(Nic, CONFIG_BB_PHY_REG_PG)) {
		MpTrace(COMP_RICHD, DBG_NORMAL, ("default power by rate loaded\n"));
		hal_data->txpwr_by_rate_from_file = 0;
		goto post_hdl;
	}
#endif
	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s():Read Tx power by rate fail\n", __func__));
	goto exit;

post_hdl:
	msg_get_var(Nic);
	if (msg_rw_val.PhyRegPgValueType != PHY_REG_PG_EXACT_VALUE) {
		wl_warn_on(1);
		goto exit;
	}

	Func_Chip_Bb_Txpowerbyrateconfiguration(Nic);
	hal_data->txpwr_by_rate_loaded = 1;

	ret = TRUE;

exit:
	return ret;
}

int Func_Chip_Bb_Load_Tx_Power_Limit(PNIC Nic, u8 chk_file)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
	struct registry_priv *regsty = &Nic->registrypriv;


	int ret = FALSE;

	hal_data->txpwr_limit_loaded = 0;
	Func_Chip_Bb_Inittxpowerlimit(Nic);

	if (!hal_data->txpwr_by_rate_loaded && regsty->target_tx_pwr_valid != _TRUE) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s():Read Tx power limit before target tx power is specify\n",
			 __func__));

		goto exit;
	}
#ifdef CONFIG_EMBEDDED_FWIMG
	if (HAL_STATUS_SUCCESS == Func_Hw_Op_Configrfwithheaderfile(Nic, 1, 0)) {
		MpTrace(COMP_RICHD, DBG_NORMAL,("default power limit loaded\n"));
		hal_data->txpwr_limit_from_file = 0;
		goto post_hdl;
	}
#endif
	MpTrace(COMP_RICHD, DBG_NORMAL,("%s():Read Tx power limit fail\n", __func__));
	goto exit;

post_hdl:
	Func_Chip_Bb_Converttxpowerlimittopowerindex(Nic);
	hal_data->txpwr_limit_loaded = 1;
	ret = TRUE;

exit:
	return ret;
}

void Func_Chip_Bb_Load_Tx_Power_Ext_Info(PNIC Nic, u8 chk_file)
{
	struct registry_priv *regsty = &Nic->registrypriv;
	
	regsty->target_tx_pwr_valid = Func_Of_Proc_Regsty_Chk_Target_Tx_Power_Valid(Nic);

	if (Func_Chip_Bb_Is_Tx_Power_By_Rate_Needed(Nic)
		|| (Func_Chip_Bb_Is_Tx_Power_Limit_Needed(Nic)
			&& regsty->target_tx_pwr_valid != _TRUE))
		Func_Chip_Bb_Load_Tx_Power_By_Rate(Nic, chk_file);

	if (Func_Chip_Bb_Is_Tx_Power_Limit_Needed(Nic))
		Func_Chip_Bb_Load_Tx_Power_Limit(Nic, chk_file);
}
