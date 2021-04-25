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

#ifndef	__WL_RF_H_
#define __WL_RF_H_

#define NumRates	(13)

#define SHORT_SLOT_TIME					9
#define NON_SHORT_SLOT_TIME				20

#define	MAX_CHANNEL_NUM					14

#define CENTER_CH_2G_NUM		14

struct regulatory_class {
	u32 starting_freq;
	u8 channel_set[MAX_CHANNEL_NUM];
	u8 channel_cck_power[MAX_CHANNEL_NUM];
	u8 channel_ofdm_power[MAX_CHANNEL_NUM];
	u8 txpower_limit;
	u8 channel_spacing;
	u8 modem;
};

#if 0  //define in hw_main.h
       //define in hw_80211.h
typedef enum _CAPABILITY {
	cIBSS = 0x0002,
	cShortPreamble = 0x0020,
	cShortSlotTime = 0x0400,
} CAPABILITY, *PCAPABILITY;


enum _REG_PREAMBLE_MODE {
	PREAMBLE_LONG = 1,
	PREAMBLE_AUTO = 2,
	PREAMBLE_SHORT = 3,
};
#endif

#define rf_path_char(path) (((path) >= 1) ? 'X' : 'A' + (path))

#define HAL_PRIME_CHNL_OFFSET_DONT_CARE	0
#define HAL_PRIME_CHNL_OFFSET_LOWER	1
#define HAL_PRIME_CHNL_OFFSET_UPPER	2

typedef enum _BAND_TYPE {
	BAND_ON_2_4G = 0,
	BAND_MAX = 1,
} BAND_TYPE, *PBAND_TYPE;

extern const char *const _band_str[];
#define band_str(band) (((band) >= BAND_MAX) ? _band_str[BAND_MAX] : _band_str[(band)])

extern const u8 _band_to_band_cap[];
#define band_to_band_cap(band) (((band) >= BAND_MAX) ? _band_to_band_cap[BAND_MAX] : _band_to_band_cap[(band)])

#if 0   //define other place
typedef enum _CHANNEL_WIDTH {
	CHANNEL_WIDTH_20 = 0,
	CHANNEL_WIDTH_40 = 1,
	CHANNEL_WIDTH_MAX = 2,
} CHANNEL_WIDTH, *PCHANNEL_WIDTH;
#endif

extern const char *const _ch_width_str[];
#define ch_width_str(bw) (((bw) >= CHANNEL_WIDTH_MAX) ? _ch_width_str[CHANNEL_WIDTH_MAX] : _ch_width_str[(bw)])

extern const u8 _ch_width_to_bw_cap[];
#define ch_width_to_bw_cap(bw) (((bw) >= CHANNEL_WIDTH_MAX) ? _ch_width_to_bw_cap[CHANNEL_WIDTH_MAX] : _ch_width_to_bw_cap[(bw)])

typedef enum _EXTCHNL_OFFSET {
	EXTCHNL_OFFSET_NO_EXT = 0,
	EXTCHNL_OFFSET_UPPER = 1,
	EXTCHNL_OFFSET_NO_DEF = 2,
	EXTCHNL_OFFSET_LOWER = 3,
} EXTCHNL_OFFSET, *PEXTCHNL_OFFSET;

typedef enum _HT_DATA_SC {
	HT_DATA_SC_DONOT_CARE = 0,
	HT_DATA_SC_20_UPPER_OF_40MHZ = 1,
	HT_DATA_SC_20_LOWER_OF_40MHZ = 2,
} HT_DATA_SC, *PHT_DATA_SC_E;

#define IS_ALPHA2_NO_SPECIFIED(_alpha2) ((*((u16 *)(_alpha2))) == 0xFFFF)

struct country_chplan {
	char alpha2[2];
	u8 chplan;
};

int wl_c2f_chan(int chan);
int wl_f2c_chan(int freq);
const struct country_chplan *wl_query_chplan_from_list(const char
														 *country_code, u8 tag);
void wl_us_tx_gain_offset(PNIC Nic, u8 ch, u8 tag);

#endif
