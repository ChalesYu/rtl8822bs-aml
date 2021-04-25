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

#define _WL_RF_C_

#include "globals.h"
#include "ndis.h"
#include "hw_pcomp.h"
#include "hw_main.h"
#include "hw_Nic.h"
#include "hw_func.h"
#include "usb_main.h"
#include "bus_intf.h"
#include "8187_gen.h"



int wl_c2f_chan(int chan)
{

	if (chan >= 1 && chan <= 14) {
		if (chan == 14)
			return 2484;
		else if (chan < 14)
			return 2407 + chan * 5;
	}

	return 0;
}

int wl_f2c_chan(int freq)
{
	if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else
		return 0;
}

const char *const _ch_width_str[] = {
	"20MHz",
	"40MHz",
	"CHANNEL_WIDTH_MAX",
};

const u8 _ch_width_to_bw_cap[] = {
	BW_CAP_20M,
	BW_CAP_40M,
	0,
};

const char *const _band_str[] = {
	"2.4G",
	"BAND_MAX",
};

const u8 _band_to_band_cap[] = {
	BAND_CAP_2G,
	0,
};

#define COUNTRY_CHPLAN_ASSIGN_EN_11AC(_val)

#define COUNTRY_CHPLAN_ASSIGN_DEF_MODULE_FLAGS(_val)

#define CHPLAN_ARY(_alpha2, _chplan, _en_11ac, _def_module_flags) \
	{.alpha2 = (_alpha2), .chplan = (_chplan) \
		COUNTRY_CHPLAN_ASSIGN_EN_11AC(_en_11ac) \
		COUNTRY_CHPLAN_ASSIGN_DEF_MODULE_FLAGS(_def_module_flags) \
	}

static const struct country_chplan chplan_arry[] = {
	CHPLAN_ARY("AD", 0x26, 1, 0x00),
	CHPLAN_ARY("AE", 0x26, 1, 0xFB),
	CHPLAN_ARY("AF", 0x42, 1, 0x00),
	CHPLAN_ARY("AG", 0x30, 1, 0x00),
	CHPLAN_ARY("AI", 0x26, 1, 0x00),
	CHPLAN_ARY("AL", 0x26, 1, 0xF1),
	CHPLAN_ARY("AM", 0x26, 1, 0xB0),
	CHPLAN_ARY("AO", 0x26, 1, 0xE0),
	CHPLAN_ARY("AQ", 0x26, 1, 0x00),
	CHPLAN_ARY("AR", 0x57, 1, 0xF3),
	CHPLAN_ARY("AS", 0x34, 1, 0x00),
	CHPLAN_ARY("AT", 0x26, 1, 0xFB),
	CHPLAN_ARY("AU", 0x45, 1, 0xFB),
	CHPLAN_ARY("AW", 0x34, 1, 0xB0),
	CHPLAN_ARY("AZ", 0x26, 1, 0xF1),
	CHPLAN_ARY("BA", 0x26, 1, 0xF1),
	CHPLAN_ARY("BB", 0x34, 1, 0x50),
	CHPLAN_ARY("BD", 0x26, 1, 0xF1),
	CHPLAN_ARY("BE", 0x26, 1, 0xFB),
	CHPLAN_ARY("BF", 0x26, 1, 0xB0),
	CHPLAN_ARY("BG", 0x26, 1, 0xF1),
	CHPLAN_ARY("BH", 0x47, 1, 0xF1),
	CHPLAN_ARY("BI", 0x26, 1, 0xB0),
	CHPLAN_ARY("BJ", 0x26, 1, 0xB0),
	CHPLAN_ARY("BN", 0x47, 1, 0x10),
	CHPLAN_ARY("BO", 0x30, 1, 0xF1),
	CHPLAN_ARY("BR", 0x34, 1, 0xF1),
	CHPLAN_ARY("BS", 0x34, 1, 0x20),
	CHPLAN_ARY("BW", 0x26, 1, 0xF1),
	CHPLAN_ARY("BY", 0x26, 1, 0xF1),
	CHPLAN_ARY("BZ", 0x34, 1, 0x00),
	CHPLAN_ARY("CA", 0x34, 1, 0xFB),
	CHPLAN_ARY("CC", 0x26, 1, 0x00),
	CHPLAN_ARY("CD", 0x26, 1, 0xB0),
	CHPLAN_ARY("CF", 0x26, 1, 0xB0),
	CHPLAN_ARY("CG", 0x26, 1, 0xB0),
	CHPLAN_ARY("CH", 0x26, 1, 0xFB),
	CHPLAN_ARY("CI", 0x26, 1, 0xF1),
	CHPLAN_ARY("CK", 0x26, 1, 0x00),
	CHPLAN_ARY("CL", 0x30, 1, 0xF1),
	CHPLAN_ARY("CM", 0x26, 1, 0xB0),
	CHPLAN_ARY("CN", 0x48, 1, 0xFB),
	CHPLAN_ARY("CO", 0x34, 1, 0xF1),
	CHPLAN_ARY("CR", 0x34, 1, 0xF1),
	CHPLAN_ARY("CV", 0x26, 1, 0xB0),
	CHPLAN_ARY("CX", 0x45, 1, 0x00),
	CHPLAN_ARY("CY", 0x26, 1, 0xFB),
	CHPLAN_ARY("CZ", 0x26, 1, 0xFB),
	CHPLAN_ARY("DE", 0x26, 1, 0xFB),
	CHPLAN_ARY("DJ", 0x26, 1, 0x80),
	CHPLAN_ARY("DK", 0x26, 1, 0xFB),
	CHPLAN_ARY("DM", 0x34, 1, 0x00),
	CHPLAN_ARY("DO", 0x34, 1, 0xF1),
	CHPLAN_ARY("DZ", 0x26, 1, 0xF1),
	CHPLAN_ARY("EC", 0x34, 1, 0xF1),
	CHPLAN_ARY("EE", 0x26, 1, 0xFB),
	CHPLAN_ARY("EG", 0x47, 0, 0xF1),
	CHPLAN_ARY("EH", 0x47, 1, 0x80),
	CHPLAN_ARY("ER", 0x26, 1, 0x00),
	CHPLAN_ARY("ES", 0x26, 1, 0xFB),
	CHPLAN_ARY("ET", 0x26, 1, 0xB0),
	CHPLAN_ARY("FI", 0x26, 1, 0xFB),
	CHPLAN_ARY("FJ", 0x34, 1, 0x00),
	CHPLAN_ARY("FK", 0x26, 1, 0x00),
	CHPLAN_ARY("FM", 0x34, 1, 0x00),
	CHPLAN_ARY("FO", 0x26, 1, 0x00),
	CHPLAN_ARY("FR", 0x26, 1, 0xFB),
	CHPLAN_ARY("GA", 0x26, 1, 0xB0),
	CHPLAN_ARY("GB", 0x26, 1, 0xFB),
	CHPLAN_ARY("GD", 0x34, 1, 0xB0),
	CHPLAN_ARY("GE", 0x26, 1, 0x00),
	CHPLAN_ARY("GF", 0x26, 1, 0x80),
	CHPLAN_ARY("GG", 0x26, 1, 0x00),
	CHPLAN_ARY("GH", 0x26, 1, 0xF1),
	CHPLAN_ARY("GI", 0x26, 1, 0x00),
	CHPLAN_ARY("GL", 0x26, 1, 0x00),
	CHPLAN_ARY("GM", 0x26, 1, 0xB0),
	CHPLAN_ARY("GN", 0x26, 1, 0x10),
	CHPLAN_ARY("GP", 0x26, 1, 0x00),
	CHPLAN_ARY("GQ", 0x26, 1, 0xB0),
	CHPLAN_ARY("GR", 0x26, 1, 0xFB),
	CHPLAN_ARY("GS", 0x26, 1, 0x00),
	CHPLAN_ARY("GT", 0x34, 1, 0xF1),
	CHPLAN_ARY("GU", 0x34, 1, 0x00),
	CHPLAN_ARY("GW", 0x26, 1, 0xB0),
	CHPLAN_ARY("GY", 0x44, 1, 0x00),
	CHPLAN_ARY("HK", 0x26, 1, 0xFB),
	CHPLAN_ARY("HM", 0x45, 1, 0x00),
	CHPLAN_ARY("HN", 0x32, 1, 0xF1),
	CHPLAN_ARY("HR", 0x26, 1, 0xF9),
	CHPLAN_ARY("HT", 0x34, 1, 0x50),
	CHPLAN_ARY("HU", 0x26, 1, 0xFB),
	CHPLAN_ARY("ID", 0x54, 0, 0xF3),
	CHPLAN_ARY("IE", 0x26, 1, 0xFB),
	CHPLAN_ARY("IL", 0x47, 1, 0xF1),
	CHPLAN_ARY("IM", 0x26, 1, 0x00),
	CHPLAN_ARY("IN", 0x47, 1, 0xF1),
	CHPLAN_ARY("IQ", 0x26, 1, 0x00),
	CHPLAN_ARY("IR", 0x26, 0, 0x00),
	CHPLAN_ARY("IS", 0x26, 1, 0xFB),
	CHPLAN_ARY("IT", 0x26, 1, 0xFB),
	CHPLAN_ARY("JE", 0x26, 1, 0x00),
	CHPLAN_ARY("JM", 0x51, 1, 0xF1),
	CHPLAN_ARY("JO", 0x49, 1, 0xFB),
	CHPLAN_ARY("JP", 0x27, 1, 0xFF),
	CHPLAN_ARY("KE", 0x47, 1, 0xF9),
	CHPLAN_ARY("KG", 0x26, 1, 0xF1),
	CHPLAN_ARY("KH", 0x26, 1, 0xF1),
	CHPLAN_ARY("KI", 0x26, 1, 0x00),
	CHPLAN_ARY("KN", 0x34, 1, 0x00),
	CHPLAN_ARY("KR", 0x28, 1, 0xFB),
	CHPLAN_ARY("KW", 0x47, 1, 0xFB),
	CHPLAN_ARY("KY", 0x34, 1, 0x00),
	CHPLAN_ARY("KZ", 0x26, 1, 0x00),
	CHPLAN_ARY("LA", 0x26, 1, 0x00),
	CHPLAN_ARY("LB", 0x26, 1, 0xF1),
	CHPLAN_ARY("LC", 0x34, 1, 0x00),
	CHPLAN_ARY("LI", 0x26, 1, 0xFB),
	CHPLAN_ARY("LK", 0x26, 1, 0xF1),
	CHPLAN_ARY("LR", 0x26, 1, 0xB0),
	CHPLAN_ARY("LS", 0x26, 1, 0xF1),
	CHPLAN_ARY("LT", 0x26, 1, 0xFB),
	CHPLAN_ARY("LU", 0x26, 1, 0xFB),
	CHPLAN_ARY("LV", 0x26, 1, 0xFB),
	CHPLAN_ARY("LY", 0x26, 1, 0x00),
	CHPLAN_ARY("MA", 0x47, 1, 0xF1),
	CHPLAN_ARY("MC", 0x26, 1, 0xFB),
	CHPLAN_ARY("MD", 0x26, 1, 0xF1),
	CHPLAN_ARY("ME", 0x26, 1, 0xF1),
	CHPLAN_ARY("MF", 0x34, 1, 0x00),
	CHPLAN_ARY("MG", 0x26, 1, 0x20),
	CHPLAN_ARY("MH", 0x34, 1, 0x00),
	CHPLAN_ARY("MK", 0x26, 1, 0xF1),
	CHPLAN_ARY("ML", 0x26, 1, 0xB0),
	CHPLAN_ARY("MM", 0x26, 1, 0x00),
	CHPLAN_ARY("MN", 0x26, 1, 0x00),
	CHPLAN_ARY("MO", 0x26, 1, 0x00),
	CHPLAN_ARY("MP", 0x34, 1, 0x00),
	CHPLAN_ARY("MQ", 0x26, 1, 0x40),
	CHPLAN_ARY("MR", 0x26, 1, 0xA0),
	CHPLAN_ARY("MS", 0x26, 1, 0x00),
	CHPLAN_ARY("MT", 0x26, 1, 0xFB),
	CHPLAN_ARY("MU", 0x26, 1, 0xB0),
	CHPLAN_ARY("MV", 0x26, 1, 0x00),
	CHPLAN_ARY("MW", 0x26, 1, 0xB0),
	CHPLAN_ARY("MX", 0x34, 1, 0xF1),
	CHPLAN_ARY("MY", 0x47, 1, 0xF1),
	CHPLAN_ARY("MZ", 0x26, 1, 0xF1),
	CHPLAN_ARY("NA", 0x26, 0, 0x00),
	CHPLAN_ARY("NC", 0x26, 1, 0x00),
	CHPLAN_ARY("NE", 0x26, 1, 0xB0),
	CHPLAN_ARY("NF", 0x45, 1, 0x00),
	CHPLAN_ARY("NG", 0x50, 1, 0xF9),
	CHPLAN_ARY("NI", 0x34, 1, 0xF1),
	CHPLAN_ARY("NL", 0x26, 1, 0xFB),
	CHPLAN_ARY("NO", 0x26, 1, 0xFB),
	CHPLAN_ARY("NP", 0x47, 1, 0xF0),
	CHPLAN_ARY("NR", 0x26, 1, 0x00),
	CHPLAN_ARY("NU", 0x45, 1, 0x00),
	CHPLAN_ARY("NZ", 0x45, 1, 0xFB),
	CHPLAN_ARY("OM", 0x26, 1, 0xF9),
	CHPLAN_ARY("PA", 0x34, 1, 0xF1),
	CHPLAN_ARY("PE", 0x34, 1, 0xF1),
	CHPLAN_ARY("PF", 0x26, 1, 0x00),
	CHPLAN_ARY("PG", 0x26, 1, 0xF1),
	CHPLAN_ARY("PH", 0x26, 1, 0xF1),
	CHPLAN_ARY("PK", 0x51, 1, 0xF1),
	CHPLAN_ARY("PL", 0x26, 1, 0xFB),
	CHPLAN_ARY("PM", 0x26, 1, 0x00),
	CHPLAN_ARY("PR", 0x34, 1, 0xF1),
	CHPLAN_ARY("PT", 0x26, 1, 0xFB),
	CHPLAN_ARY("PW", 0x34, 1, 0x00),
	CHPLAN_ARY("PY", 0x34, 1, 0xF1),
	CHPLAN_ARY("QA", 0x51, 1, 0xF9),
	CHPLAN_ARY("RE", 0x26, 1, 0x00),
	CHPLAN_ARY("RO", 0x26, 1, 0xF1),
	CHPLAN_ARY("RS", 0x26, 1, 0xF1),
	CHPLAN_ARY("RU", 0x59, 1, 0xFB),
	CHPLAN_ARY("RW", 0x26, 1, 0xB0),
	CHPLAN_ARY("SA", 0x26, 1, 0xFB),
	CHPLAN_ARY("SB", 0x26, 1, 0x00),
	CHPLAN_ARY("SC", 0x34, 1, 0x90),
	CHPLAN_ARY("SE", 0x26, 1, 0xFB),
	CHPLAN_ARY("SG", 0x47, 1, 0xFB),
	CHPLAN_ARY("SH", 0x26, 1, 0x00),
	CHPLAN_ARY("SI", 0x26, 1, 0xFB),
	CHPLAN_ARY("SJ", 0x26, 1, 0x00),
	CHPLAN_ARY("SK", 0x26, 1, 0xFB),
	CHPLAN_ARY("SL", 0x26, 1, 0xB0),
	CHPLAN_ARY("SM", 0x26, 1, 0x00),
	CHPLAN_ARY("SN", 0x26, 1, 0xF1),
	CHPLAN_ARY("SO", 0x26, 1, 0x00),
	CHPLAN_ARY("SR", 0x34, 1, 0x00),
	CHPLAN_ARY("ST", 0x34, 1, 0x80),
	CHPLAN_ARY("SV", 0x30, 1, 0xF1),
	CHPLAN_ARY("SX", 0x34, 1, 0x00),
	CHPLAN_ARY("SZ", 0x26, 1, 0x20),
	CHPLAN_ARY("TC", 0x26, 1, 0x00),
	CHPLAN_ARY("TD", 0x26, 1, 0xB0),
	CHPLAN_ARY("TF", 0x26, 1, 0x80),
	CHPLAN_ARY("TG", 0x26, 1, 0xB0),
	CHPLAN_ARY("TH", 0x26, 1, 0xF1),
	CHPLAN_ARY("TJ", 0x26, 1, 0x40),
	CHPLAN_ARY("TK", 0x45, 1, 0x00),
	CHPLAN_ARY("TM", 0x26, 1, 0x00),
	CHPLAN_ARY("TN", 0x47, 1, 0xF1),
	CHPLAN_ARY("TO", 0x26, 1, 0x00),
	CHPLAN_ARY("TR", 0x26, 1, 0xF1),
	CHPLAN_ARY("TT", 0x42, 1, 0xF1),
	CHPLAN_ARY("TW", 0x39, 1, 0xFF),
	CHPLAN_ARY("TZ", 0x26, 1, 0xF0),
	CHPLAN_ARY("UA", 0x26, 1, 0xFB),
	CHPLAN_ARY("UG", 0x26, 1, 0xF1),
	CHPLAN_ARY("US", 0x34, 1, 0xFF),
	CHPLAN_ARY("UY", 0x34, 1, 0xF1),
	CHPLAN_ARY("UZ", 0x47, 1, 0xF0),
	CHPLAN_ARY("VA", 0x26, 1, 0x00),
	CHPLAN_ARY("VC", 0x34, 1, 0x10),
	CHPLAN_ARY("VE", 0x30, 1, 0xF1),
	CHPLAN_ARY("VI", 0x34, 1, 0x00),
	CHPLAN_ARY("VN", 0x26, 1, 0xF1),
	CHPLAN_ARY("VU", 0x26, 1, 0x00),
	CHPLAN_ARY("WF", 0x26, 1, 0x00),
	CHPLAN_ARY("WS", 0x34, 1, 0x00),
	CHPLAN_ARY("YE", 0x26, 1, 0x40),
	CHPLAN_ARY("YT", 0x26, 1, 0x80),
	CHPLAN_ARY("ZA", 0x26, 1, 0xF1),
	CHPLAN_ARY("ZM", 0x26, 1, 0xB0),
	CHPLAN_ARY("ZW", 0x26, 1, 0xF1),
};

static const u16 chplan_arry_sz = sizeof(chplan_arry) / sizeof(struct country_chplan);

const struct country_chplan *wl_query_chplan_from_list(const char
														 *country_code, u8 tag)
{
	int i;
	u16 map_sz = 0;
	char code[2];
	const struct country_chplan *ent = NULL;
	const struct country_chplan *map = NULL;
	if (tag) {
		code[0] = Func_Alpha_To_Upper(country_code[0]);
		code[1] = Func_Alpha_To_Upper(country_code[1]);

		map = chplan_arry;
		map_sz = chplan_arry_sz;

		for (i = 0; i < map_sz; i++) {
			if (strncmp(code, map[i].alpha2, 2) == 0) {
				ent = &map[i];
				break;
			}
		}
	}
	return ent;
}

static int wl_ch2bb_gain_sel(int ch)
{
	int sel = -1;

	if (ch >= 1 && ch <= 14)
		sel = 0;

	return sel;
}

static s8 wl_query_mk_tx_gain_offset(PNIC Nic, u8 path, u8 ch,
										  u8 tag)
{
	s8 kfree_offset = 0;

#ifdef CONFIG_RF_POWER_TRIM
	s8 bb_gain_sel = wl_ch2bb_gain_sel(ch);
//	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
	struct kfree_data_t *kfree_data = GET_KFREE_DATA(Nic);
	if (tag) {
		if (bb_gain_sel < 0 || bb_gain_sel >= 1) {
			wl_warn_on(1);
			goto exit;
		}

		if (kfree_data->flag & KFREE_FLAG_ON) {
			kfree_offset = kfree_data->bb_gain[bb_gain_sel][path];
			if (1)
				MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s path:%u, ch:%u, bb_gain_sel:%d, kfree_offset:%d\n",
						__func__, path, ch, bb_gain_sel, kfree_offset));
		}
	}
exit:
#endif
	return kfree_offset;
}

#define RF_TX_GAIN_OFFSET_9086X(_val) (abs((_val)) | (((_val) > 0) ? BIT5 : 0))

static void wl_set_tx_gain_offset(u8 path, s8 offset, PNIC Nic)
{
	u8 write_value;

	MpTrace(COMP_EVENTS, DBG_NORMAL, ("kfree gain_offset 0x55:0x%x ",
			Func_Of_Proc_Chip_Hw_Read_Rfreg(Nic, path, 0x55, 0xffffffff)));

	write_value = RF_TX_GAIN_OFFSET_9086X(offset);
	Func_Of_Proc_Chip_Hw_Write_Rfreg(Nic, path, 0x55, 0x0fc000, write_value);
	MpTrace(COMP_EVENTS, DBG_NORMAL, (" after :0x%x\n",
			Func_Of_Proc_Chip_Hw_Read_Rfreg(Nic, path, 0x55, 0xffffffff)));
}

void wl_us_tx_gain_offset(PNIC Nic, u8 ch, u8 tag)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
	s8 kfree_offset = 0;
	s8 tx_pwr_track_offset = 0;
	s8 total_offset;
	int i;
	if (tag) {
		for (i = 0; i < hal_data->NumTotalRFPath; i++) {
			kfree_offset = wl_query_mk_tx_gain_offset(Nic, i, ch, 1);
			total_offset = kfree_offset + tx_pwr_track_offset;
			wl_set_tx_gain_offset(i, total_offset, Nic);
		}
	}
}
