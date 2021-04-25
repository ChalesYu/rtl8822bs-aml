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







///////////////////////////////copy from mlme
u8 *do_query_data_from_ie(u8 * ie, u8 type)
{
	if (type == CAPABILITY)
		return (ie + 8 + 2);
	else if (type == TIMESTAMPE)
		return (ie + 0);
	else if (type == BCN_INTERVAL)
		return (ie + 8);
	else
		return NULL;
}

/////////////////////////ssvc.h
#define WL_GET_BE16(a) ((u16) (((a)[0] << 8) | (a)[1]))
#define WL_PUT_BE16(a, val)			\
	do {					\
		(a)[0] = ((u16) (val)) >> 8;	\
		(a)[1] = ((u16) (val)) & 0xff;	\
	} while (0)

#define WL_GET_LE16(a) ((u16) (((a)[1] << 8) | (a)[0]))
#define WL_PUT_LE16(a, val)			\
	do {					\
		(a)[1] = ((u16) (val)) >> 8;	\
		(a)[0] = ((u16) (val)) & 0xff;	\
	} while (0)

#define WL_GET_BE24(a) ((((u32) (a)[0]) << 16) | (((u32) (a)[1]) << 8) | \
			 ((u32) (a)[2]))
#define WL_PUT_BE24(a, val)					\
	do {							\
		(a)[0] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[2] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define WL_GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
			 (((u32) (a)[2]) << 8) | ((u32) (a)[3]))
#define WL_PUT_BE32(a, val)					\
	do {							\
		(a)[0] = (u8) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[2] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[3] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define WL_GET_LE32(a) ((((u32) (a)[3]) << 24) | (((u32) (a)[2]) << 16) | \
			 (((u32) (a)[1]) << 8) | ((u32) (a)[0]))
#define WL_PUT_LE32(a, val)					\
	do {							\
		(a)[3] = (u8) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[2] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[0] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define WL_GET_BE64(a) ((((u64) (a)[0]) << 56) | (((u64) (a)[1]) << 48) | \
			 (((u64) (a)[2]) << 40) | (((u64) (a)[3]) << 32) | \
			 (((u64) (a)[4]) << 24) | (((u64) (a)[5]) << 16) | \
			 (((u64) (a)[6]) << 8) | ((u64) (a)[7]))
#define WL_PUT_BE64(a, val)				\
	do {						\
		(a)[0] = (u8) (((u64) (val)) >> 56);	\
		(a)[1] = (u8) (((u64) (val)) >> 48);	\
		(a)[2] = (u8) (((u64) (val)) >> 40);	\
		(a)[3] = (u8) (((u64) (val)) >> 32);	\
		(a)[4] = (u8) (((u64) (val)) >> 24);	\
		(a)[5] = (u8) (((u64) (val)) >> 16);	\
		(a)[6] = (u8) (((u64) (val)) >> 8);	\
		(a)[7] = (u8) (((u64) (val)) & 0xff);	\
	} while (0)

#define WL_GET_LE64(a) ((((u64) (a)[7]) << 56) | (((u64) (a)[6]) << 48) | \
			 (((u64) (a)[5]) << 40) | (((u64) (a)[4]) << 32) | \
			 (((u64) (a)[3]) << 24) | (((u64) (a)[2]) << 16) | \
			 (((u64) (a)[1]) << 8) | ((u64) (a)[0]))




u8 WL_WPA_OUI_TYPE[] = { 0x00, 0x50, 0xf2, 1 };

u16 WL_WPA_VERSION = 1;
u8 WPA_AUTH_KEY_MGMT_NONE[] = { 0x00, 0x50, 0xf2, 0 };
u8 WPA_AUTH_KEY_MGMT_UNSPEC_802_1X[] = { 0x00, 0x50, 0xf2, 1 };
u8 WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X[] = { 0x00, 0x50, 0xf2, 2 };
u8 WPA_CIPHER_SUITE_NONE[] = { 0x00, 0x50, 0xf2, 0 };
u8 WPA_CIPHER_SUITE_WEP40[] = { 0x00, 0x50, 0xf2, 1 };
u8 WPA_CIPHER_SUITE_TKIP[] = { 0x00, 0x50, 0xf2, 2 };
u8 WPA_CIPHER_SUITE_WRAP[] = { 0x00, 0x50, 0xf2, 3 };
u8 WPA_CIPHER_SUITE_CCMP[] = { 0x00, 0x50, 0xf2, 4 };
u8 WPA_CIPHER_SUITE_WEP104[] = { 0x00, 0x50, 0xf2, 5 };

u16 RSN_VERSION_BSD = 1;
u8 RSN_AUTH_KEY_MGMT_UNSPEC_802_1X[] = { 0x00, 0x0f, 0xac, 1 };
u8 RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X[] = { 0x00, 0x0f, 0xac, 2 };
u8 RSN_CIPHER_SUITE_NONE[] = { 0x00, 0x0f, 0xac, 0 };
u8 RSN_CIPHER_SUITE_WEP40[] = { 0x00, 0x0f, 0xac, 1 };
u8 RSN_CIPHER_SUITE_TKIP[] = { 0x00, 0x0f, 0xac, 2 };
u8 RSN_CIPHER_SUITE_WRAP[] = { 0x00, 0x0f, 0xac, 3 };
u8 RSN_CIPHER_SUITE_CCMP[] = { 0x00, 0x0f, 0xac, 4 };
u8 RSN_CIPHER_SUITE_WEP104[] = { 0x00, 0x0f, 0xac, 5 };

static u8 WIFI_CCKRATES[] =
	{ (IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK),
	(IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK),
	(IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK),
	(IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK)
};

static u8 WIFI_OFDMRATES[] = { (IEEE80211_OFDM_RATE_6MB),
	(IEEE80211_OFDM_RATE_9MB),
	(IEEE80211_OFDM_RATE_12MB),
	(IEEE80211_OFDM_RATE_18MB),
	(IEEE80211_OFDM_RATE_24MB),
	IEEE80211_OFDM_RATE_36MB,
	IEEE80211_OFDM_RATE_48MB,
	IEEE80211_OFDM_RATE_54MB
};

u8 mgn_rates_cck[4] = { MGN_1M, MGN_2M, MGN_5_5M, MGN_11M };
u8 mgn_rates_ofdm[8] =
	{ MGN_6M, MGN_9M, MGN_12M, MGN_18M, MGN_24M, MGN_36M, MGN_48M, MGN_54M };
u8 mgn_rates_mcs0_7[8] =
	{ MGN_MCS0, MGN_MCS1, MGN_MCS2, MGN_MCS3, MGN_MCS4, MGN_MCS5, MGN_MCS6,
MGN_MCS7 };

static const char *const _rate_section_str[] = {
	"CCK",
	"OFDM",
	"HT_1SS",
	"RATE_SECTION_UNKNOWN",
};

const char *section_str_of_rate_func(u8 section, u8 flag)
{
	if (flag) {
		section = (section >= RATE_SECTION_NUM) ? RATE_SECTION_NUM : section;
	}
	return _rate_section_str[section];
}

struct rate_section_ent rates_by_sections[RATE_SECTION_NUM] = {
	{0, 4, mgn_rates_cck},
	{0, 8, mgn_rates_ofdm},
	{0, 8, mgn_rates_mcs0_7},

};

int bit_value_from_ieee_value_to_get_func(u8 val, u8 flag)
{
	unsigned char dot11_rate_table[] =
		{ 2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108, 0 };

	int i = 0;
	if (flag) {
		while (dot11_rate_table[i] != 0) {
			if (dot11_rate_table[i] == val)
				return BIT(i);
			i++;
		}
	}
	return 0;
}

uint check_is_cckrates_included_func(u8 * rate, u8 flag)
{
	u32 i = 0;

	if (flag) {
		while (rate[i] != 0) {
			if ((((rate[i]) & 0x7f) == 2) || (((rate[i]) & 0x7f) == 4) ||
				(((rate[i]) & 0x7f) == 11) || (((rate[i]) & 0x7f) == 22))
				return TRUE;
			i++;
		}
	}
	return FALSE;
}

uint check_is_cckratesonly_included_func(u8 * rate, u8 flag)
{
	u32 i = 0;

	if (flag) {
		while (rate[i] != 0) {
			if ((((rate[i]) & 0x7f) != 2) && (((rate[i]) & 0x7f) != 4) &&
				(((rate[i]) & 0x7f) != 11) && (((rate[i]) & 0x7f) != 22))
				return FALSE;

			i++;
		}
	}
	return TRUE;

}

int network_type_to_check_func(unsigned char *rate, int ratelen, int channel,
						   u8 flag)
{
	UNREFERENCED_PARAMETER(flag);
	UNREFERENCED_PARAMETER(ratelen);
	if (channel <= 14) {
		if ((check_is_cckratesonly_included_func(rate, 1)) == TRUE)
			return WIRELESS_11B;
		else if ((check_is_cckrates_included_func(rate, 1)) == TRUE)
			return WIRELESS_11BG;
		else
			return WIRELESS_11G;
	} else
		return WIRELESS_INVALID;

}

u8 *fixed_ie_to_set_func(u8 flag, unsigned char *pbuf, unsigned int len,
					 unsigned char *source, unsigned int *frlen)
{
	if (flag) {
		Func_Of_Proc_Pre_Memcpy((void *)pbuf, (void *)source, len);
	}
	*frlen = *frlen + len;
	return (pbuf + len);
}

u8 *ie_to_set_func(u8 * pbuf, sint index, uint len, u8 * source, uint * frlen) {
	*pbuf = (u8) index;

	*(pbuf + 1) = (u8) len;

	if (len > 0)
		Func_Of_Proc_Pre_Memcpy((void *)(pbuf + 2), (void *)source, len);

	*frlen = *frlen + (len + 2);

	return (pbuf + len + 2);
}

inline u8 *ie_ch_switch_to_set_func(u8 * buf, u32 * buf_len, u8 ch_switch_mode,
								u8 new_ch, u8 ch_switch_cnt, u8 flag)
{
	u8 ie_data[3];

	if (flag) {
		ie_data[0] = ch_switch_mode;
	}
	ie_data[1] = new_ch;
	ie_data[2] = ch_switch_cnt;
	return ie_to_set_func(buf, WLAN_EID_CHANNEL_SWITCH, 3, ie_data, buf_len);
}

inline u8 convert_sec_ch_offset_to_hal_ch_offset_func(u8 ch_offset, u8 flag)
{
	if (flag) {
		if (ch_offset == SCN)
			return HAL_PRIME_CHNL_OFFSET_DONT_CARE;
		else if (ch_offset == SCA)
			return HAL_PRIME_CHNL_OFFSET_UPPER;
		else if (ch_offset == SCB)
			return HAL_PRIME_CHNL_OFFSET_LOWER;
	}
	return HAL_PRIME_CHNL_OFFSET_DONT_CARE;
}

inline u8 ch_offset_to_secondary_ch_offset_of_hal_func(u8 ch_offset, u8 flag)
{
	if (flag) {
		if (ch_offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE)
			return SCN;
		else if (ch_offset == HAL_PRIME_CHNL_OFFSET_LOWER)
			return SCB;
		else if (ch_offset == HAL_PRIME_CHNL_OFFSET_UPPER)
			return SCA;
	}
	return SCN;
}

inline u8 *ie_secondary_ch_offset_to_set_func(u8 * buf, u32 * buf_len,
										  u8 secondary_ch_offset, u8 flag)
{
	if (flag) {
		return ie_to_set_func(buf, WLAN_EID_SECONDARY_CHANNEL_OFFSET, 1,
						  &secondary_ch_offset, buf_len);
	} else {
		return 0;
	}
}

inline u8 *ie_mesh_ch_switch_parm_to_set_func(u8 * buf, u32 * buf_len, u8 ttl,
										  u8 flags, u16 reason, u16 precedence,
										  u8 flag)
{
	u8 ie_data[6];

	ie_data[0] = ttl;
	ie_data[1] = flags;
	if (flag) {
		WL_PUT_LE16((u8 *) & ie_data[2], reason);
		WL_PUT_LE16((u8 *) & ie_data[4], precedence);
	}
	return ie_to_set_func(buf, 0x118, 6, ie_data, buf_len);
}

u8 *ie_to_get_func(u8 * pbuf, sint index, sint * len, sint limit)
{
	sint tmp, i;
	u8 *p;
	_func_enter_;
	if (limit < 1) {
		_func_exit_;
		return NULL;
	}

	p = pbuf;
	i = 0;
	*len = 0;
	while (1) {
		if (*p == index) {
			*len = *(p + 1);
			return (p);
		} else {
			tmp = *(p + 1);
			p += (tmp + 2);
			i += (tmp + 2);
		}
		if (i >= limit)
			break;
	}
	_func_exit_;
	return NULL;
}

static u8 *ie_ex_to_get_func(u8 * in_ie, uint in_len, u8 eid, u8 * oui, u8 oui_len,
				  u8 * ie, uint * ielen, u8 flag)
{
	uint cnt;
	u8 *target_ie = NULL;

	if (flag) {
		if (ielen)
			*ielen = 0;
	}
	if (!in_ie || in_len <= 0)
		return target_ie;

	cnt = 0;

	while (cnt < in_len) {
		if (eid == in_ie[cnt]
			&& (!oui || Func_Of_Proc_Pre_Memcmp(&in_ie[cnt + 2], oui, oui_len) == TRUE)) {
			target_ie = &in_ie[cnt];

			if (ie)
				Func_Of_Proc_Pre_Memcpy(ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

			if (ielen)
				*ielen = in_ie[cnt + 1] + 2;

			break;
		} else {
			cnt += in_ie[cnt + 1] + 2;
		}

	}

	return target_ie;
}

int ies_ie_to_remove_func(u8 * ies, uint * ies_len, uint offset, u8 eid, u8 * oui,
					  u8 oui_len, u8 flag)
{
	int ret = FALSE;
	u8 *target_ie;
	u32 target_ielen;
	u8 *start;
	uint search_len;

	if (flag) {
		if (!ies || !ies_len || *ies_len <= offset)
			goto exit;
	}
	start = ies + offset;
	search_len = *ies_len - offset;

	while (1) {
		target_ie =
			ie_ex_to_get_func(start, search_len, eid, oui, oui_len, NULL,
						  &target_ielen, 1);
		if (target_ie && target_ielen) {
			u8 *remain_ies = target_ie + target_ielen;
			uint remain_len = search_len - (remain_ies - start);

			Func_Of_Proc_Pre_Memmove(target_ie, remain_ies, remain_len);
			*ies_len = *ies_len - target_ielen;
			ret = TRUE;

			start = target_ie;
			search_len = remain_len;
		} else {
			break;
		}
	}
exit:
	return ret;
}

void supported_rate_to_set_func(u8 * SupportedRates, uint mode, u8 flag)
{
	_func_enter_;
	if (flag) {
		memset(SupportedRates, 0, NDIS_802_11_LENGTH_RATES_EX);
	}
	switch (mode) {
	case WIRELESS_11B:
		Func_Of_Proc_Pre_Memcpy(SupportedRates, WIFI_CCKRATES, IEEE80211_CCK_RATE_LEN);
		break;

	case WIRELESS_11G:
		Func_Of_Proc_Pre_Memcpy(SupportedRates, WIFI_OFDMRATES,
					IEEE80211_NUM_OFDM_RATESLEN);
		break;

	case WIRELESS_11BG:
	case WIRELESS_11G_24N:
	case WIRELESS_11_24N:
	case WIRELESS_11BG_24N:
		Func_Of_Proc_Pre_Memcpy(SupportedRates, WIFI_CCKRATES, IEEE80211_CCK_RATE_LEN);
		Func_Of_Proc_Pre_Memcpy(SupportedRates + IEEE80211_CCK_RATE_LEN, WIFI_OFDMRATES,
					IEEE80211_NUM_OFDM_RATESLEN);
		break;

	}
	_func_exit_;
}

uint rateset_len_to_get_func(u8 * rateset, u8 flag)
{
	uint i = 0;
	_func_enter_;
	if (flag) {
		while (1) {
			if ((rateset[i]) == 0)
				break;

			if (i > 12)
				break;

			i++;
		}
	}
	_func_exit_;
	return i;
}

int proc_generate_ie_func(struct registry_priv *pregistrypriv, u8 flag)
{
	u8 wireless_mode = 0;
	int sz = 0, rateLen;
	WLAN_BSSID_EX *pdev_network = &pregistrypriv->dev_network;
	u8 *ie = pdev_network->IEs;

	_func_enter_;

	sz += 8;
	ie += sz;

	*(u16 *) ie = cpu_to_le16((u16) pdev_network->Configuration.BeaconPeriod);
	sz += 2;
	ie += 2;

	*(u16 *) ie = 0;
	if (flag) {
		*(u16 *) ie |= cpu_to_le16(cap_IBSS);
	}
	if (pregistrypriv->preamble == PREAMBLE_SHORT)
		*(u16 *) ie |= cpu_to_le16(cap_ShortPremble);

	if (pdev_network->Privacy)
		*(u16 *) ie |= cpu_to_le16(cap_Privacy);

	sz += 2;
	ie += 2;

	ie = ie_to_set_func(ie, _SSID_IE_, pdev_network->Ssid.SsidLength,
					pdev_network->Ssid.Ssid, &sz);

	if (pregistrypriv->wireless_mode == WIRELESS_11BG_24N) {
		if (pdev_network->Configuration.DSConfig <= 14)
			wireless_mode = WIRELESS_11BG_24N;
	} else if (pregistrypriv->wireless_mode == WIRELESS_MODE_MAX) {
		if (pdev_network->Configuration.DSConfig <= 14)
			wireless_mode = WIRELESS_11BG_24N;
	} else
		wireless_mode = pregistrypriv->wireless_mode;

	supported_rate_to_set_func(pdev_network->SupportedRates, wireless_mode, 1);

	rateLen = rateset_len_to_get_func(pdev_network->SupportedRates, 1);

	if (rateLen > 8) {
		ie = ie_to_set_func(ie, _SUPPORTEDRATES_IE_, 8,
						pdev_network->SupportedRates, &sz);
	} else {
		ie = ie_to_set_func(ie, _SUPPORTEDRATES_IE_, rateLen,
						pdev_network->SupportedRates, &sz);
	}

	ie = ie_to_set_func(ie, _DSSET_IE_, 1,
					(u8 *) & (pdev_network->Configuration.DSConfig), &sz);

	ie = ie_to_set_func(ie, _IBSS_PARA_IE_, 2,
					(u8 *) & (pdev_network->Configuration.ATIMWindow), &sz);

	if (rateLen > 8) {
		ie = ie_to_set_func(ie, _EXT_SUPPORTEDRATES_IE_, (rateLen - 8),
						(pdev_network->SupportedRates + 8), &sz);
	}
#ifdef CONFIG_80211N_HT
	if (((pregistrypriv->wireless_mode & WIRELESS_11_24N))
		&& (pregistrypriv->ht_enable == TRUE)) {
	}
#endif

	_func_exit_;

	return sz;

}

unsigned char *wpa_ie_to_get_func(unsigned char *pie, int *wpa_ie_len, int limit,
							  u8 flag)
{
	int len;
	u16 val16;
	unsigned char wpa_oui_type[] = { 0x00, 0x50, 0xf2, 0x01 };
	u8 *pbuf = pie;
	int limit_new = limit;

	while (1) {
		pbuf = ie_to_get_func(pbuf, _WPA_IE_ID_, &len, limit_new);

		if (pbuf) {

			if (Func_Of_Proc_Pre_Memcmp((pbuf + 2), wpa_oui_type, sizeof(wpa_oui_type)) ==
				FALSE) {

				goto check_next_ie;
			}

			Func_Of_Proc_Pre_Memcpy((u8 *) & val16, (pbuf + 6), sizeof(val16));

			val16 = le16_to_cpu(val16);
			if (val16 != 0x0001)
				goto check_next_ie;

			*wpa_ie_len = *(pbuf + 1);

			return pbuf;

		} else {

			*wpa_ie_len = 0;
			return NULL;
		}

check_next_ie:

		limit_new = limit - (pbuf - pie) - 2 - len;

		if (limit_new <= 0)
			break;

		pbuf += (2 + len);

	}

	if (flag) {
		*wpa_ie_len = 0;
	}
	return NULL;

}

unsigned char *wpa2_ie_to_get_func(unsigned char *pie, int *rsn_ie_len, int limit)
{

	return ie_to_get_func(pie, _WPA2_IE_ID_, rsn_ie_len, limit);

}

int wpa_cipher_suite_to_get_func(u8 * s, u8 flag)
{
	if (flag) {
		if (Func_Of_Proc_Pre_Memcmp(s, WPA_CIPHER_SUITE_NONE, WPA_SELECTOR_LEN) == TRUE)
			return WPA_CIPHER_NONE;
		if (Func_Of_Proc_Pre_Memcmp(s, WPA_CIPHER_SUITE_WEP40, WPA_SELECTOR_LEN) == TRUE)
			return WPA_CIPHER_WEP40;
		if (Func_Of_Proc_Pre_Memcmp(s, WPA_CIPHER_SUITE_TKIP, WPA_SELECTOR_LEN) == TRUE)
			return WPA_CIPHER_TKIP;
		if (Func_Of_Proc_Pre_Memcmp(s, WPA_CIPHER_SUITE_CCMP, WPA_SELECTOR_LEN) == TRUE)
			return WPA_CIPHER_CCMP;
		if (Func_Of_Proc_Pre_Memcmp(s, WPA_CIPHER_SUITE_WEP104, WPA_SELECTOR_LEN) == TRUE)
			return WPA_CIPHER_WEP104;
	}
	return 0;
}

static int wpa2_cipher_suite_to_get_func(u8 * s, u8 flag)
{
	if (flag) {
		if (Func_Of_Proc_Pre_Memcmp(s, RSN_CIPHER_SUITE_NONE, RSN_SELECTOR_LEN) == TRUE)
			return WPA_CIPHER_NONE;
		if (Func_Of_Proc_Pre_Memcmp(s, RSN_CIPHER_SUITE_WEP40, RSN_SELECTOR_LEN) == TRUE)
			return WPA_CIPHER_WEP40;
		if (Func_Of_Proc_Pre_Memcmp(s, RSN_CIPHER_SUITE_TKIP, RSN_SELECTOR_LEN) == TRUE)
			return WPA_CIPHER_TKIP;
		if (Func_Of_Proc_Pre_Memcmp(s, RSN_CIPHER_SUITE_CCMP, RSN_SELECTOR_LEN) == TRUE)
			return WPA_CIPHER_CCMP;
		if (Func_Of_Proc_Pre_Memcmp(s, RSN_CIPHER_SUITE_WEP104, RSN_SELECTOR_LEN) == TRUE)
			return WPA_CIPHER_WEP104;
	}
	return 0;
}

int proc_parse_wpa_ie_func(u8 * wpa_ie, int wpa_ie_len, int *group_cipher,
					 int *pairwise_cipher, int *is_8021x, u8 flag)
{
	int i, ret = TRUE;
	int left, count;
	u8 *pos;
	u8 SUITE_1X[4] = { 0x00, 0x50, 0xf2, 1 };

	if (flag) {
		if (wpa_ie_len <= 0) {
			return FALSE;
		}
	}

	if ((*wpa_ie != _WPA_IE_ID_) || (*(wpa_ie + 1) != (u8) (wpa_ie_len - 2)) ||
		(Func_Of_Proc_Pre_Memcmp(wpa_ie + 2, WL_WPA_OUI_TYPE, WPA_SELECTOR_LEN) != TRUE))
	{
		return FALSE;
	}

	pos = wpa_ie;

	pos += 8;
	left = wpa_ie_len - 8;

	if (left >= WPA_SELECTOR_LEN) {

		*group_cipher = wpa_cipher_suite_to_get_func(pos, 1);

		pos += WPA_SELECTOR_LEN;
		left -= WPA_SELECTOR_LEN;

	} else if (left > 0) {
		MpTrace(COMP_EVENTS, DBG_NORMAL,
			("%s: ie length mismatch, %u too much", __FUNCTION__, left));

		return FALSE;
	}

	if (left >= 2) {
		count = WL_GET_LE16(pos);
		pos += 2;
		left -= 2;

		if (count == 0 || left < count * WPA_SELECTOR_LEN) {

			return FALSE;
		}

		for (i = 0; i < count; i++) {
			*pairwise_cipher |= wpa_cipher_suite_to_get_func(pos, 1);

			pos += WPA_SELECTOR_LEN;
			left -= WPA_SELECTOR_LEN;
		}

	} else if (left == 1) {

		return FALSE;
	}

	if (is_8021x) {
		if (left >= 6) {
			pos += 2;
			if (Func_Of_Proc_Pre_Memcmp(pos, SUITE_1X, 4) == 1) {
				*is_8021x = 1;
			}
		}
	}

	return ret;

}

int proc_parse_wpa2_ie_func(u8 * rsn_ie, int rsn_ie_len, int *group_cipher,
					  int *pairwise_cipher, int *is_8021x, u8 flag)
{
	int i, ret = TRUE;
	int left, count;
	u8 *pos;
	u8 SUITE_1X[4] = { 0x00, 0x0f, 0xac, 0x01 };

	if (flag) {
		if (rsn_ie_len <= 0) {
			return FALSE;
		}
	}

	if ((*rsn_ie != _WPA2_IE_ID_) || (*(rsn_ie + 1) != (u8) (rsn_ie_len - 2))) {
		return FALSE;
	}

	pos = rsn_ie;
	pos += 4;
	left = rsn_ie_len - 4;

	if (left >= RSN_SELECTOR_LEN) {

		*group_cipher = wpa2_cipher_suite_to_get_func(pos, 1);

		pos += RSN_SELECTOR_LEN;
		left -= RSN_SELECTOR_LEN;

	} else if (left > 0) {
		MpTrace(COMP_EVENTS, DBG_NORMAL,
			("%s: ie length mismatch, %u too much", __FUNCTION__, left));
		return FALSE;
	}

	if (left >= 2) {
		count = WL_GET_LE16(pos);
		pos += 2;
		left -= 2;

		if (count == 0 || left < count * RSN_SELECTOR_LEN) {
			return FALSE;
		}

		for (i = 0; i < count; i++) {
			*pairwise_cipher |= wpa2_cipher_suite_to_get_func(pos, 1);

			pos += RSN_SELECTOR_LEN;
			left -= RSN_SELECTOR_LEN;
		}

	} else if (left == 1) {
		MpTrace(COMP_EVENTS, DBG_NORMAL,
			("%s: ie too short (for key mgmt)", __FUNCTION__));
		return FALSE;
	}

	if (is_8021x) {
		if (left >= 6) {
			pos += 2;
			if (Func_Of_Proc_Pre_Memcmp(pos, SUITE_1X, 4) == 1) {
				MpTrace(COMP_EVENTS, DBG_NORMAL,
					("%s (): there has 802.1x auth\n", __FUNCTION__));
				*is_8021x = 1;
			}
		}
	}

	return ret;

}

int wapi_ie_to_get_func(u8 * in_ie, uint in_len, u8 * wapi_ie, u16 * wapi_len,
					u8 flag)
{
	int len = 0;
	u8 authmode, i;
	uint cnt;
	u8 wapi_oui1[4] = { 0x0, 0x14, 0x72, 0x01 };
	u8 wapi_oui2[4] = { 0x0, 0x14, 0x72, 0x02 };

	_func_enter_;

	if (flag) {
		if (wapi_len)
			*wapi_len = 0;
	}
	if (!in_ie || in_len <= 0)
		return len;

	cnt = (_TIMESTAMP_ + _BEACON_ITERVAL_ + _CAPABILITY_);

	while (cnt < in_len) {
		authmode = in_ie[cnt];

		if (authmode == _WAPI_IE_
			&& (Func_Of_Proc_Pre_Memcmp(&in_ie[cnt + 6], wapi_oui1, 4) == TRUE
				|| Func_Of_Proc_Pre_Memcmp(&in_ie[cnt + 6], wapi_oui2, 4) == TRUE)) {
			if (wapi_ie) {
				Func_Of_Proc_Pre_Memcpy(wapi_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

				for (i = 0; i < (in_ie[cnt + 1] + 2); i = i + 8) {
     				MpTrace(COMP_EVENTS, DBG_NORMAL,
     					("\n %2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x\n",
							   wapi_ie[i], wapi_ie[i + 1], wapi_ie[i + 2],
							   wapi_ie[i + 3], wapi_ie[i + 4], wapi_ie[i + 5],
							   wapi_ie[i + 6], wapi_ie[i + 7]));	
				}
			}

			if (wapi_len)
				*wapi_len = in_ie[cnt + 1] + 2;

			cnt += in_ie[cnt + 1] + 2;
		} else {
			cnt += in_ie[cnt + 1] + 2;
		}
	}

	if (wapi_len)
		len = *wapi_len;

	_func_exit_;

	return len;

}

int sec_ie_to_get_func(u8 * in_ie, uint in_len, u8 * rsn_ie, u16 * rsn_len,
				   u8 * wpa_ie, u16 * wpa_len, u8 flag)
{
	u8 authmode, sec_idx, i;
	u8 wpa_oui[4] = { 0x0, 0x50, 0xf2, 0x01 };
	uint cnt;

	_func_enter_;

	cnt = (_TIMESTAMP_ + _BEACON_ITERVAL_ + _CAPABILITY_);

	sec_idx = 0;
	if (flag) {
		while (cnt < in_len) {
			authmode = in_ie[cnt];

			if ((authmode == _WPA_IE_ID_)
				&& (Func_Of_Proc_Pre_Memcmp(&in_ie[cnt + 2], &wpa_oui[0], 4) == TRUE)) {
				MpTrace(COMP_EVENTS, DBG_NORMAL,
					("\n wpa_ie_to_get_func: sec_idx=%d in_ie[cnt+1]+2=%d\n",
							sec_idx, in_ie[cnt + 1] + 2));	

				if (wpa_ie) {
					Func_Of_Proc_Pre_Memcpy(wpa_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

					for (i = 0; i < (in_ie[cnt + 1] + 2); i = i + 8) {

					}
				}

				*wpa_len = in_ie[cnt + 1] + 2;
				cnt += in_ie[cnt + 1] + 2;
			} else {
				if (authmode == _WPA2_IE_ID_) {
					MpTrace(COMP_EVENTS, DBG_NORMAL,
						("\n get_rsn_ie: sec_idx=%d in_ie[cnt+1]+2=%d\n",
								sec_idx, in_ie[cnt + 1] + 2));

					if (rsn_ie) {
						Func_Of_Proc_Pre_Memcpy(rsn_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

						for (i = 0; i < (in_ie[cnt + 1] + 2); i = i + 8) {

						}
					}

					*rsn_len = in_ie[cnt + 1] + 2;
					cnt += in_ie[cnt + 1] + 2;
				} else {
					cnt += in_ie[cnt + 1] + 2;
				}
			}

		}
	}
	_func_exit_;

	return (*rsn_len + *wpa_len);

}

u8 check_is_wps_ie_func(u8 * ie_ptr, uint * wps_ielen, u8 flag)
{
	u8 match = FALSE;
	u8 eid, wps_oui[4] = { 0x0, 0x50, 0xf2, 0x04 };
	if (flag) {
		if (ie_ptr == NULL)
			return match;
	}
	eid = ie_ptr[0];

	if ((eid == _WPA_IE_ID_) && (Func_Of_Proc_Pre_Memcmp(&ie_ptr[2], wps_oui, 4) == TRUE)) {
		*wps_ielen = ie_ptr[1] + 2;
		match = TRUE;
	}
	return match;
}

u8 *wps_ie_from_scan_queue_to_get_func(u8 * in_ie, uint in_len, u8 * wps_ie,
								   uint * wps_ielen, u8 frame_type, u8 flag)
{
	u8 *wps = NULL;

	if (flag) {
		MpTrace(COMP_EVENTS, DBG_NORMAL,
			("[%s] frame_type = %d\n", __FUNCTION__, frame_type));
		switch (frame_type) {
		case 1:
		case 3:
			{
				wps =
					wps_ie_to_get_func(in_ie + _PROBERSP_IE_OFFSET_,
								   in_len - _PROBERSP_IE_OFFSET_, wps_ie,
								   wps_ielen, 1);
				break;
			}
		case 2:
			{
				wps =
					wps_ie_to_get_func(in_ie + _PROBEREQ_IE_OFFSET_,
								   in_len - _PROBEREQ_IE_OFFSET_, wps_ie,
								   wps_ielen, 1);
				break;
			}
		}
	}
	return wps;
}

u8 *wps_ie_to_get_func(u8 * in_ie, uint in_len, u8 * wps_ie, uint * wps_ielen,
				   u8 flag)
{
	uint cnt;
	u8 *wpsie_ptr = NULL;
	u8 eid, wps_oui[4] = { 0x00, 0x50, 0xf2, 0x04 };

	if (flag) {
		if (wps_ielen)
			*wps_ielen = 0;
	}
	if (!in_ie) {
		wl_warn_on(1);
		return wpsie_ptr;
	}

	if (in_len <= 0)
		return wpsie_ptr;

	cnt = 0;

	while (cnt + 1 + 4 < in_len) {
		eid = in_ie[cnt];

		if (cnt + 1 + 4 >= MAX_IE_SZ) {
			wl_warn_on(1);
			return NULL;
		}

		if (eid == WLAN_EID_VENDOR_SPECIFIC
			&& Func_Of_Proc_Pre_Memcmp(&in_ie[cnt + 2], wps_oui, 4) == TRUE) {
			wpsie_ptr = in_ie + cnt;

			if (wps_ie)
				Func_Of_Proc_Pre_Memcpy(wps_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

			if (wps_ielen)
				*wps_ielen = in_ie[cnt + 1] + 2;

			break;
		} else {
			cnt += in_ie[cnt + 1] + 2;
		}

	}

	return wpsie_ptr;
}

u8 *wps_attr_to_get_func(u8 * wps_ie, uint wps_ielen, u16 target_attr_id,
					 u8 * buf_attr, u32 * len_attr, u8 flag)
{
	u8 *attr_ptr = NULL;
	u8 *target_attr_ptr = NULL;
	u8 wps_oui[4] = { 0x00, 0x50, 0xF2, 0x04 };

	if (flag) {
		if (len_attr)
			*len_attr = 0;
	}
	if ((wps_ie[0] != _VENDOR_SPECIFIC_IE_) ||
		(Func_Of_Proc_Pre_Memcmp(wps_ie + 2, wps_oui, 4) != TRUE)) {
		return attr_ptr;
	}

	attr_ptr = wps_ie + 6;

	while (attr_ptr - wps_ie < wps_ielen) {
		u16 attr_id = WL_GET_BE16(attr_ptr);
		u16 attr_data_len = WL_GET_BE16(attr_ptr + 2);
		u16 attr_len = attr_data_len + 4;

		if (attr_id == target_attr_id) {
			target_attr_ptr = attr_ptr;

			if (buf_attr)
				Func_Of_Proc_Pre_Memcpy(buf_attr, attr_ptr, attr_len);

			if (len_attr)
				*len_attr = attr_len;

			break;
		} else {
			attr_ptr += attr_len;
		}

	}

	return target_attr_ptr;
}

u8 *wps_attr_content_to_get_func(u8 flag, u8 * wps_ie, uint wps_ielen,
							 u16 target_attr_id, u8 * buf_content,
							 uint * len_content)
{
	u8 *attr_ptr;
	u32 attr_len;

	if (flag) {
		if (len_content)
			*len_content = 0;
	}
	attr_ptr =
		wps_attr_to_get_func(wps_ie, wps_ielen, target_attr_id, NULL, &attr_len, 1);

	if (attr_ptr && attr_len) {
		if (buf_content)
			Func_Of_Proc_Pre_Memcpy(buf_content, attr_ptr + 4, attr_len - 4);

		if (len_content)
			*len_content = attr_len - 4;

		return attr_ptr + 4;
	}

	return NULL;
}

static int proc_ieee802_11_parse_vendor_specific_func(u8 * pos, uint elen,
												struct wl_ieee802_11_elems
												*elems, int show_errors,
												u8 flag)
{
	unsigned int oui;

	if (elen < 4) {
		if (show_errors) {
			MpTrace(COMP_EVENTS, DBG_NORMAL,
				("short vendor specific "
					"information element ignored (len=%lu)\n",
					(unsigned long)elen));
		}
		return -1;
	}

	oui = WL_GET_BE24(pos);
	if (flag) {
		switch (oui) {
		case OUI_MICROSOFT:
			switch (pos[3]) {
			case 1:
				elems->wpa_ie = pos;
				elems->wpa_ie_len = elen;
				break;
			case WME_OUI_TYPE:
				if (elen < 5) {
					MpTrace(COMP_EVENTS, DBG_NORMAL,
						("short WME "
							"information element ignored "
							"(len=%lu)\n", (unsigned long)elen));

					return -1;
				}
				switch (pos[4]) {
				case WME_OUI_SUBTYPE_INFORMATION_ELEMENT:
				case WME_OUI_SUBTYPE_PARAMETER_ELEMENT:
					elems->wme = pos;
					elems->wme_len = elen;
					break;
				case WME_OUI_SUBTYPE_TSPEC_ELEMENT:
					elems->wme_tspec = pos;
					elems->wme_tspec_len = elen;
					break;
				default:
					MpTrace(COMP_EVENTS, DBG_NORMAL,
						("unknown WME "
							   "information element ignored "
							   "(subtype=%d len=%lu)\n",
							   pos[4], (unsigned long)elen));
					return -1;
				}
				break;
			case 4:
				elems->wps_ie = pos;
				elems->wps_ie_len = elen;
				break;
			default:
				MpTrace(COMP_EVENTS, DBG_NORMAL,
					("Unknown Microsoft "
						   "information element ignored "
						   "(type=%d len=%lu)\n", pos[3], (unsigned long)elen));

				return -1;
			}
			break;

		case OUI_BROADCOM:
			switch (pos[3]) {
			case VENDOR_HT_CAPAB_OUI_TYPE:
				elems->vendor_ht_cap = pos;
				elems->vendor_ht_cap_len = elen;
				break;
			default:
				MpTrace(COMP_EVENTS, DBG_NORMAL,
					("Unknown Broadcom "
						   "information element ignored "
						   "(type=%d len=%lu)\n", pos[3], (unsigned long)elen));
				return -1;
			}
			break;

		default:
			MpTrace(COMP_EVENTS, DBG_NORMAL,
				( "unknown vendor specific information "
					   "element ignored (vendor OUI %02x:%02x:%02x "
					   "len=%lu)\n",
					   pos[0], pos[1], pos[2], (unsigned long)elen));
			return -1;
		}
	}
	return 0;

}

ParseRes proc_ieee802_11_parse_elems_func(u8 * start, uint len,
									struct wl_ieee802_11_elems * elems,
									int show_errors, u8 flag)
{
	uint left = len;
	u8 *pos = start;
	int unknown = 0;

	if (flag) {
		memset(elems, 0, sizeof(*elems));
	}
	while (left >= 2) {
		u8 id, elen;

		id = *pos++;
		elen = *pos++;
		left -= 2;

		if (elen > left) {
			if (show_errors) {
				MpTrace(COMP_EVENTS, DBG_NORMAL,
					("IEEE 802.11 element "
						"parse failed (id=%d elen=%d "
						"left=%lu)\n", id, elen, (unsigned long)left));
			}
			return ParseFailed;
		}

		switch (id) {
		case WLAN_EID_SSID:
			elems->ssid = pos;
			elems->ssid_len = elen;
			break;
		case WLAN_EID_SUPP_RATES:
			elems->supp_rates = pos;
			elems->supp_rates_len = elen;
			break;
		case WLAN_EID_FH_PARAMS:
			elems->fh_params = pos;
			elems->fh_params_len = elen;
			break;
		case WLAN_EID_DS_PARAMS:
			elems->ds_params = pos;
			elems->ds_params_len = elen;
			break;
		case WLAN_EID_CF_PARAMS:
			elems->cf_params = pos;
			elems->cf_params_len = elen;
			break;
		case WLAN_EID_TIM:
			elems->tim = pos;
			elems->tim_len = elen;
			break;
		case WLAN_EID_IBSS_PARAMS:
			elems->ibss_params = pos;
			elems->ibss_params_len = elen;
			break;
		case WLAN_EID_CHALLENGE:
			elems->challenge = pos;
			elems->challenge_len = elen;
			break;
		case WLAN_EID_ERP_INFO:
			elems->erp_info = pos;
			elems->erp_info_len = elen;
			break;
		case WLAN_EID_EXT_SUPP_RATES:
			elems->ext_supp_rates = pos;
			elems->ext_supp_rates_len = elen;
			break;
		case WLAN_EID_VENDOR_SPECIFIC:
			if (proc_ieee802_11_parse_vendor_specific_func(pos, elen,
													 elems, show_errors, 1))
				unknown++;
			break;
		case WLAN_EID_RSN:
			elems->rsn_ie = pos;
			elems->rsn_ie_len = elen;
			break;
		case WLAN_EID_PWR_CAPABILITY:
			elems->power_cap = pos;
			elems->power_cap_len = elen;
			break;
		case WLAN_EID_SUPPORTED_CHANNELS:
			elems->supp_channels = pos;
			elems->supp_channels_len = elen;
			break;
		case WLAN_EID_MOBILITY_DOMAIN:
			elems->mdie = pos;
			elems->mdie_len = elen;
			break;
		case WLAN_EID_FAST_BSS_TRANSITION:
			elems->ftie = pos;
			elems->ftie_len = elen;
			break;
		case WLAN_EID_TIMEOUT_INTERVAL:
			elems->timeout_int = pos;
			elems->timeout_int_len = elen;
			break;
		case WLAN_EID_HT_CAP:
			elems->ht_capabilities = pos;
			elems->ht_capabilities_len = elen;
			break;
		case WLAN_EID_HT_OPERATION:
			elems->ht_operation = pos;
			elems->ht_operation_len = elen;
			break;
		default:
			unknown++;
			if (!show_errors)
				break;
			MpTrace(COMP_EVENTS, DBG_NORMAL,
				("IEEE 802.11 element parse "
					   "ignored unknown element (id=%d elen=%d)\n", id, elen));
			break;
		}

		left -= elen;
		pos += elen;
	}

	if (left)
		return ParseFailed;

	return unknown ? ParseUnknown : ParseOK;

}

static u8 key_of_char2num_func(u8 ch);
static u8 key_of_char2num_func(u8 ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	else if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else
		return 0xff;
}

u8 str_2char_2num_func(u8 hch, u8 lch);
u8 str_2char_2num_func(u8 hch, u8 lch)
{
	return ((key_of_char2num_func(hch) * 10) + key_of_char2num_func(lch));
}

u8 key_of_2char2num_func(u8 hch, u8 lch);
u8 key_of_2char2num_func(u8 hch, u8 lch)
{
	return ((key_of_char2num_func(hch) << 4) | key_of_char2num_func(lch));
}

void macstr2_num_func(u8 * dst, u8 * src);
void macstr2_num_func(u8 * dst, u8 * src)
{
	int jj, kk;
	for (jj = 0, kk = 0; jj < ETH_ALEN; jj++, kk += 3) {
		dst[jj] = key_of_2char2num_func(src[kk], src[kk + 1]);
	}
}

u8 ip_addr_convert_func(u8 hch, u8 mch, u8 lch)
{
	return ((key_of_char2num_func(hch) * 100) + (key_of_char2num_func(mch) * 10) +
			key_of_char2num_func(lch));
}

u8 invalid_mac_address_to_check_func(u8 * mac_addr, u8 check_local_bit, u8 flag)
{
	u8 null_mac_addr[ETH_ALEN] = { 0, 0, 0, 0, 0, 0 };
	u8 multi_mac_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	u8 res = FALSE;

	if (flag) {
		if (Func_Of_Proc_Pre_Memcmp(mac_addr, null_mac_addr, ETH_ALEN)) {
			res = TRUE;
			goto func_exit;
		}
	}
	if (Func_Of_Proc_Pre_Memcmp(mac_addr, multi_mac_addr, ETH_ALEN)) {
		res = TRUE;
		goto func_exit;
	}

	if (mac_addr[0] & BIT0) {
		res = TRUE;
		goto func_exit;
	}

	if (check_local_bit == TRUE) {
		if (mac_addr[0] & BIT1) {
			res = TRUE;
			goto func_exit;
		}
	}

func_exit:
	return res;
}

extern char *wl_initmac;
void proc_macaddr_cfg_func(u8 * out, const u8 * hw_mac_addr, u8 flag)
{
#define DEFAULT_RANDOM_MACADDR 1
	u8 mac[ETH_ALEN];

	if (out == NULL) {
		wl_warn_on(1);
		return;
	}

	if (wl_initmac) {
		int jj, kk;

		for (jj = 0, kk = 0; jj < ETH_ALEN; jj++, kk += 3)
			mac[jj] = key_of_2char2num_func(wl_initmac[kk], wl_initmac[kk + 1]);

		goto err_chk;
	}

	if (hw_mac_addr) {
		Func_Of_Proc_Pre_Memcpy(mac, hw_mac_addr, ETH_ALEN);
		goto err_chk;
	}

err_chk:
	if (invalid_mac_address_to_check_func(mac, TRUE, 1) == TRUE) {
		MpTrace(COMP_EVENTS, DBG_NORMAL,
			("invalid mac addr:" MAC_FMT ", assign default one\n",
				   MAC_ARG(mac)));
		mac[0] = 0xb4;
		mac[1] = 0x04;
		mac[2] = 0x18;
		mac[3] = 0x00;
		mac[4] = 0x00;
		mac[5] = 0x12; 
	}

	if (flag) {
		Func_Of_Proc_Pre_Memcpy(out, mac, ETH_ALEN);
	}
	MpTrace(COMP_EVENTS, DBG_NORMAL,
		("%s mac addr:" MAC_FMT "\n", __func__, MAC_ARG(out)));
}

void ies_chbw_to_get_func(u8 * ies, int ies_len, u8 * ch, u8 * bw, u8 * offset,
					  u8 flag)
{
	u8 *p;
	int ie_len;

	*ch = 0;
	*bw = CHANNEL_WIDTH_20;
	*offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;

	p = ie_to_get_func(ies, _DSSET_IE_, &ie_len, ies_len);
	if (flag) {
		if (p && ie_len > 0)
			*ch = *(p + 2);
	}
#ifdef CONFIG_80211N_HT
	{
		u8 *ht_cap_ie, *ht_op_ie;
		int ht_cap_ielen, ht_op_ielen;

		ht_cap_ie = ie_to_get_func(ies, EID_HTCapability, &ht_cap_ielen, ies_len);
		if (ht_cap_ie && ht_cap_ielen) {
			if (GET_HT_CAP_ELE_CHL_WIDTH(ht_cap_ie + 2))
				*bw = CHANNEL_WIDTH_40;
		}

		ht_op_ie = ie_to_get_func(ies, EID_HTInfo, &ht_op_ielen, ies_len);
		if (ht_op_ie && ht_op_ielen) {
			if (*ch == 0) {
				*ch = GET_HT_OP_ELE_PRI_CHL(ht_op_ie + 2);
			} else if (*ch != 0 && *ch != GET_HT_OP_ELE_PRI_CHL(ht_op_ie + 2)) {
				MpTrace(COMP_EVENTS, DBG_NORMAL,
					("%s ch inconsistent, DSSS:%u, HT primary:%u\n",
						__func__, *ch, GET_HT_OP_ELE_PRI_CHL(ht_op_ie + 2)));
			}

			if (!GET_HT_OP_ELE_STA_CHL_WIDTH(ht_op_ie + 2))
				*bw = CHANNEL_WIDTH_20;

			if (*bw == CHANNEL_WIDTH_40) {
				switch (GET_HT_OP_ELE_2ND_CHL_OFFSET(ht_op_ie + 2)) {
				case SCA:
					*offset = HAL_PRIME_CHNL_OFFSET_LOWER;
					break;
				case SCB:
					*offset = HAL_PRIME_CHNL_OFFSET_UPPER;
					break;
				}
			}
		}
	}
#endif
}

void get_chbw_of_bss_func(WLAN_BSSID_EX * bss, u8 * ch, u8 * bw, u8 * offset,
					  u8 flag)
{
	if (flag) {
		ies_chbw_to_get_func(bss->IEs + sizeof(NDIS_802_11_FIXED_IEs)
						 , bss->IELength - sizeof(NDIS_802_11_FIXED_IEs)
						 , ch, bw, offset, 1);
	}
	if (*ch == 0) {
		*ch = bss->Configuration.DSConfig;
	} else if (*ch != bss->Configuration.DSConfig) {
		MpTrace(COMP_EVENTS, DBG_NORMAL,
			("inconsistent ch - ies:%u bss->Configuration.DSConfig:%u\n",
				*ch, bss->Configuration.DSConfig));
		*ch = bss->Configuration.DSConfig;
		wl_warn_on(1);
	}
}

bool check_is_chbw_grouped_func(u8 ch_a, u8 bw_a, u8 offset_a, u8 ch_b, u8 bw_b,
						 u8 offset_b, u8 flag)
{
	bool is_grouped = FALSE;

	if (ch_a != ch_b) {
		goto exit;
	} else if ((bw_a == CHANNEL_WIDTH_40)
			   && (bw_b == CHANNEL_WIDTH_40)
		) {
		if (offset_a != offset_b)
			goto exit;
	}

	if (flag) {
		is_grouped = TRUE;
	}
exit:
	return is_grouped;
}

static u8 wl_get_offset_by_ch(u8 channel, u8 tag)
{
	u8 offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	if (tag) {
		if (channel >= 1 && channel <= 4) {
			offset = HAL_PRIME_CHNL_OFFSET_LOWER;
		} else if (channel >= 5 && channel <= 14) {
			offset = HAL_PRIME_CHNL_OFFSET_UPPER;
		} else {
			offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
		}
	}
	return offset;

}

void chbw_to_sync_func(u8 * req_ch, u8 * req_bw, u8 * req_offset, u8 * g_ch,
				   u8 * g_bw, u8 * g_offset, u8 flag)
{

	*req_ch = *g_ch;

	switch (*req_bw) {
	case CHANNEL_WIDTH_40:
		if (*g_bw == CHANNEL_WIDTH_40)
			*req_offset = *g_offset;
		else if (*g_bw == CHANNEL_WIDTH_20)
			*req_offset = wl_get_offset_by_ch(*req_ch, 1);

		if (*req_offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE) {
			MpTrace(COMP_EVENTS, DBG_NORMAL,
			("%s req 40MHz BW without offset, down to 20MHz\n",
					   __func__));
			wl_warn_on(1);
			*req_bw = CHANNEL_WIDTH_20;
		}
		break;
	case CHANNEL_WIDTH_20:
		*req_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
		break;
	default:
		MpTrace(COMP_EVENTS, DBG_NORMAL,
		("%s req unsupported BW:%u\n", __func__, *req_bw));
		wl_warn_on(1);
	}

	if (flag) {
		if (*req_bw > *g_bw) {
			*g_bw = *req_bw;
			*g_offset = *req_offset;
		}
	}
}

u32 p2p_merged_ies_len_tp_get_func(u8 * in_ie, u32 in_len, u8 flag)
{
	PNDIS_802_11_VARIABLE_IEs pIE;
	u8 OUI[4] = { 0x50, 0x6f, 0x9a, 0x09 };
	int i = 0;
	int j = 0, len = 0;

	if (flag) {
		while (i < in_len) {
			pIE = (PNDIS_802_11_VARIABLE_IEs) (in_ie + i);

			if (pIE->ElementID == _VENDOR_SPECIFIC_IE_
				&& Func_Of_Proc_Pre_Memcmp(pIE->data, OUI, 4)) {
				len += pIE->Length - 4;
			}

			i += (pIE->Length + 2);
		}
	}
	return len + 4;
}

int proc_p2p_merge_ies_func(u8 * in_ie, u32 in_len, u8 * merge_ie, u8 flag)
{
	PNDIS_802_11_VARIABLE_IEs pIE;
	u8 len = 0;
	u8 OUI[4] = { 0x50, 0x6f, 0x9a, 0x09 };
	u8 ELOUI[6] = { 0xDD, 0x00, 0x50, 0x6f, 0x9a, 0x09 };
	int i = 0;

	if (merge_ie != NULL) {
		if (flag) {
			Func_Of_Proc_Pre_Memcpy(merge_ie, ELOUI, 6);
			merge_ie += 6;
		}
		while (i < in_len) {
			pIE = (PNDIS_802_11_VARIABLE_IEs) (in_ie + i);

			if (pIE->ElementID == _VENDOR_SPECIFIC_IE_
				&& Func_Of_Proc_Pre_Memcmp(pIE->data, OUI, 4)) {
				Func_Of_Proc_Pre_Memcpy(merge_ie, pIE->data + 4, pIE->Length - 4);
				len += pIE->Length - 4;
				merge_ie += pIE->Length - 4;
			}

			i += (pIE->Length + 2);
		}

		return len + 4;

	}

	return 0;
}

void p2p_ie_dump_func(void *sel, u8 * ie, u32 ie_len, u8 flag)
{
	u8 *pos = (u8 *) ie;
	u8 id;
	u16 len;

	u8 *p2p_ie;
	uint p2p_ielen;

	p2p_ie = p2p_ie_to_get_func(ie, ie_len, NULL, &p2p_ielen);
	if (p2p_ie != ie || p2p_ielen == 0)
		return;

	if (flag) {
		pos += 6;
	}
	while (pos - ie + 3 <= ie_len) {
		id = *pos;
		len = WL_GET_LE16(pos + 1);
		MpTrace(COMP_EVENTS, DBG_NORMAL,
		("%s ID:%u, LEN:%u%s\n", __func__, id, len,
				   ((pos - ie + 3 + len) <= ie_len) ? "" : "(exceed ie_len)"));

		pos += (3 + len);
	}
}

u8 *p2p_ie_to_get_func(u8 * in_ie, int in_len, u8 * p2p_ie, uint * p2p_ielen)
{
	uint cnt;
	u8 *p2p_ie_ptr = NULL;
	u8 eid, p2p_oui[4] = { 0x50, 0x6F, 0x9A, 0x09 };

	if (p2p_ielen)
		*p2p_ielen = 0;

	if (!in_ie || in_len < 0) {
		wl_warn_on(1);
		return p2p_ie_ptr;
	}

	if (in_len <= 0)
		return p2p_ie_ptr;

	cnt = 0;

	while (cnt + 1 + 4 < in_len) {
		eid = in_ie[cnt];

		if (cnt + 1 + 4 >= MAX_IE_SZ) {
			wl_warn_on(1);
			return NULL;
		}

		if (eid == WLAN_EID_VENDOR_SPECIFIC
			&& Func_Of_Proc_Pre_Memcmp(&in_ie[cnt + 2], p2p_oui, 4) == TRUE) {
			p2p_ie_ptr = in_ie + cnt;

			if (p2p_ie)
				Func_Of_Proc_Pre_Memcpy(p2p_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

			if (p2p_ielen)
				*p2p_ielen = in_ie[cnt + 1] + 2;

			break;
		} else {
			cnt += in_ie[cnt + 1] + 2;
		}

	}

	return p2p_ie_ptr;
}

static u8 *p2p_attr_to_get_func(u8 * p2p_ie, uint p2p_ielen, u8 target_attr_id,
					 u8 * buf_attr, u32 * len_attr)
{
	u8 *attr_ptr = NULL;
	u8 *target_attr_ptr = NULL;
	u8 p2p_oui[4] = { 0x50, 0x6F, 0x9A, 0x09 };

	if (len_attr)
		*len_attr = 0;

	if (!p2p_ie || p2p_ielen <= 6 || (p2p_ie[0] != WLAN_EID_VENDOR_SPECIFIC)
		|| (Func_Of_Proc_Pre_Memcmp(p2p_ie + 2, p2p_oui, 4) != TRUE))
		return attr_ptr;

	attr_ptr = p2p_ie + 6;

	while ((attr_ptr - p2p_ie + 3) <= p2p_ielen) {
		u8 attr_id = *attr_ptr;
		u16 attr_data_len = WL_GET_LE16(attr_ptr + 1);
		u16 attr_len = attr_data_len + 3;

		if (0)
			MpTrace(COMP_EVENTS, DBG_NORMAL,
			("%s attr_ptr:%p, id:%u, length:%u\n", __func__, attr_ptr,
					attr_id, attr_data_len));

		if ((attr_ptr - p2p_ie + attr_len) > p2p_ielen)
			break;

		if (attr_id == target_attr_id) {
			target_attr_ptr = attr_ptr;

			if (buf_attr)
				Func_Of_Proc_Pre_Memcpy(buf_attr, attr_ptr, attr_len);

			if (len_attr)
				*len_attr = attr_len;

			break;
		} else {
			attr_ptr += attr_len;
		}
	}

	return target_attr_ptr;
}

u8 *p2p_attr_content_to_get_func(u8 * p2p_ie, uint p2p_ielen, u8 target_attr_id,
							 u8 * buf_content, uint * len_content)
{
	u8 *attr_ptr;
	u32 attr_len;

	if (len_content)
		*len_content = 0;

	attr_ptr =
		p2p_attr_to_get_func(p2p_ie, p2p_ielen, target_attr_id, NULL, &attr_len);

	if (attr_ptr && attr_len) {
		if (buf_content)
			Func_Of_Proc_Pre_Memcpy(buf_content, attr_ptr + 3, attr_len - 3);

		if (len_content)
			*len_content = attr_len - 3;

		return attr_ptr + 3;
	}

	return NULL;
}

u32 p2p_attr_content_to_set_func(u8 * pbuf, u8 attr_id, u16 attr_len,
							 u8 * pdata_attr, u8 flag)
{
	u32 a_len;

	*pbuf = attr_id;

	WL_PUT_LE16(pbuf + 1, attr_len);

	if (flag) {
		if (pdata_attr)
			Func_Of_Proc_Pre_Memcpy(pbuf + 3, pdata_attr, attr_len);
	}
	a_len = attr_len + 3;

	return a_len;
}

static uint p2p_ie_to_del_func(u8 * ies, uint ies_len_ori, const char *msg, u8 flag)
{
	u8 *target_ie;
	u32 target_ie_len;
	uint ies_len = ies_len_ori;
	int index = 0;

	if (flag) {
		while (1) {
			target_ie = p2p_ie_to_get_func(ies, ies_len, NULL, &target_ie_len);
			if (target_ie && target_ie_len) {
				u8 *next_ie = target_ie + target_ie_len;
				uint remain_len = ies_len - (next_ie - ies);
				Func_Of_Proc_Pre_Memmove(target_ie, next_ie, remain_len);
				memset(target_ie + remain_len, 0, target_ie_len);
				ies_len -= target_ie_len;

				index++;
			} else {
				break;
			}
		}
	}
	return ies_len;
}

static uint p2p_attr_to_del_func(u8 * ie, uint ielen_ori, u8 attr_id, u8 flag)
{
	u8 *target_attr;
	u32 target_attr_len;
	uint ielen = ielen_ori;
	int index = 0;

	if (flag) {
		while (1) {
			target_attr =
				p2p_attr_to_get_func(ie, ielen, attr_id, NULL, &target_attr_len);
			if (target_attr && target_attr_len) {
				u8 *next_attr = target_attr + target_attr_len;
				uint remain_len = ielen - (next_attr - ie);
				Func_Of_Proc_Pre_Memmove(target_attr, next_attr, remain_len);
				memset(target_attr + remain_len, 0, target_attr_len);
				*(ie + 1) -= target_attr_len;
				ielen -= target_attr_len;

				index++;
			} else {
				break;
			}
		}
	}
	return ielen;
}

inline u8 *get_p2p_ie_of_bss_ex_func(WLAN_BSSID_EX * bss_ex, u8 * p2p_ie,
								 uint * p2p_ielen)
{
	return p2p_ie_to_get_func(BSS_EX_TLV_IES(bss_ex), BSS_EX_TLV_IES_LEN(bss_ex),
						  p2p_ie, p2p_ielen);
}

void del_p2p_ie_of_bss_ex_func(WLAN_BSSID_EX * bss_ex, u8 flag)
{
#define DBG_BSS_EX_DEL_P2P_IE 0

	u8 *ies = BSS_EX_TLV_IES(bss_ex);
	uint ies_len_ori = BSS_EX_TLV_IES_LEN(bss_ex);
	uint ies_len;

	ies_len =
		p2p_ie_to_del_func(ies, ies_len_ori,
					   DBG_BSS_EX_DEL_P2P_IE ? __func__ : NULL, 1);
	if (flag) {
		bss_ex->IELength -= ies_len_ori - ies_len;
	}
}

void del_p2p_attr_of_bss_ex_func(WLAN_BSSID_EX * bss_ex, u8 attr_id, u8 flag)
{
	u8 *ies = BSS_EX_TLV_IES(bss_ex);
	uint ies_len = BSS_EX_TLV_IES_LEN(bss_ex);

	u8 *ie;
	uint ie_len, ie_len_ori;

	int index = 0;

	if (flag) {
		while (1) {
			ie = p2p_ie_to_get_func(ies, ies_len, NULL, &ie_len_ori);
			if (ie) {
				u8 *next_ie_ori = ie + ie_len_ori;
				uint remain_len =
					bss_ex->IELength - (next_ie_ori - bss_ex->IEs);
				u8 has_target_attr = 0;
				ie_len = p2p_attr_to_del_func(ie, ie_len_ori, attr_id, 1);
				if (ie_len != ie_len_ori) {
					u8 *next_ie = ie + ie_len;

					Func_Of_Proc_Pre_Memmove(next_ie, next_ie_ori, remain_len);
					memset(next_ie + remain_len, 0, ie_len_ori - ie_len);
					bss_ex->IELength -= ie_len_ori - ie_len;

					ies = next_ie;
				} else {
					ies = next_ie_ori;
				}

				ies_len = remain_len;

				index++;
			} else {
				break;
			}
		}
	}
}

void wfd_ie_dump_func(void *sel, u8 * ie, u32 ie_len)
{
	u8 *pos = (u8 *) ie;
	u8 id;
	u16 len;

	u8 *wfd_ie;
	uint wfd_ielen;

	wfd_ie = wfd_ie_to_get_func(1, ie, ie_len, NULL, &wfd_ielen);
	if (wfd_ie != ie || wfd_ielen == 0)
		return;

	pos += 6;
	while (pos - ie + 3 <= ie_len) {
		id = *pos;
		len = WL_GET_BE16(pos + 1);
		MpTrace(COMP_EVENTS, DBG_NORMAL,
		("%s ID:%u, LEN:%u%s\n", __func__, id, len,
				   ((pos - ie + 3 + len) <= ie_len) ? "" : "(exceed ie_len)"));

		pos += (3 + len);
	}
}

u8 *wfd_ie_to_get_func(u8 flag, u8 * in_ie, int in_len, u8 * wfd_ie,
				   uint * wfd_ielen)
{
	uint cnt;
	u8 *wfd_ie_ptr = NULL;
	u8 eid, wfd_oui[4] = { 0x50, 0x6F, 0x9A, 0x0A };

	if (flag) {
		if (wfd_ielen)
			*wfd_ielen = 0;
	}
	if (!in_ie || in_len < 0) {
		wl_warn_on(1);
		return wfd_ie_ptr;
	}

	if (in_len <= 0)
		return wfd_ie_ptr;

	cnt = 0;

	while (cnt + 1 + 4 < in_len) {
		eid = in_ie[cnt];

		if (cnt + 1 + 4 >= MAX_IE_SZ) {
			wl_warn_on(1);
			return NULL;
		}

		if (eid == WLAN_EID_VENDOR_SPECIFIC
			&& Func_Of_Proc_Pre_Memcmp(&in_ie[cnt + 2], wfd_oui, 4) == TRUE) {
			wfd_ie_ptr = in_ie + cnt;

			if (wfd_ie)
				Func_Of_Proc_Pre_Memcpy(wfd_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

			if (wfd_ielen)
				*wfd_ielen = in_ie[cnt + 1] + 2;

			break;
		} else {
			cnt += in_ie[cnt + 1] + 2;
		}

	}

	return wfd_ie_ptr;
}

static u8 *wfd_attr_to_get_func(u8 * wfd_ie, uint wfd_ielen, u8 target_attr_id,
					 u8 * buf_attr, u32 * len_attr, u8 flag)
{
	u8 *attr_ptr = NULL;
	u8 *target_attr_ptr = NULL;
	u8 wfd_oui[4] = { 0x50, 0x6F, 0x9A, 0x0A };

	if (flag) {
		if (len_attr)
			*len_attr = 0;
	}
	if (!wfd_ie || wfd_ielen <= 6 || (wfd_ie[0] != WLAN_EID_VENDOR_SPECIFIC)
		|| (Func_Of_Proc_Pre_Memcmp(wfd_ie + 2, wfd_oui, 4) != TRUE))
		return attr_ptr;

	attr_ptr = wfd_ie + 6;

	while ((attr_ptr - wfd_ie + 3) <= wfd_ielen) {
		u8 attr_id = *attr_ptr;
		u16 attr_data_len = WL_GET_BE16(attr_ptr + 1);
		u16 attr_len = attr_data_len + 3;

		if (0)
			MpTrace(COMP_EVENTS, DBG_NORMAL,
			("%s attr_ptr:%p, id:%u, length:%u\n", __func__, attr_ptr,
					attr_id, attr_data_len));

		if ((attr_ptr - wfd_ie + attr_len) > wfd_ielen)
			break;

		if (attr_id == target_attr_id) {
			target_attr_ptr = attr_ptr;

			if (buf_attr)
				Func_Of_Proc_Pre_Memcpy(buf_attr, attr_ptr, attr_len);

			if (len_attr)
				*len_attr = attr_len;

			break;
		} else {
			attr_ptr += attr_len;
		}
	}

	return target_attr_ptr;
}

u8 *wfd_attr_content_to_get_func(u8 * wfd_ie, uint wfd_ielen, u8 target_attr_id,
							 u8 * buf_content, uint * len_content, u8 flag)
{
	u8 *attr_ptr;
	u32 attr_len;

	if (flag) {
		if (len_content)
			*len_content = 0;
	}
	attr_ptr =
		wfd_attr_to_get_func(wfd_ie, wfd_ielen, target_attr_id, NULL, &attr_len, 1);

	if (attr_ptr && attr_len) {
		if (buf_content)
			Func_Of_Proc_Pre_Memcpy(buf_content, attr_ptr + 3, attr_len - 3);

		if (len_content)
			*len_content = attr_len - 3;

		return attr_ptr + 3;
	}

	return NULL;
}

uint wfd_ie_to_del_func(u8 * ies, uint ies_len_ori, const char *msg, u8 flag)
{
	u8 *target_ie;
	u32 target_ie_len;
	uint ies_len = ies_len_ori;
	int index = 0;

	if (flag) {
		while (1) {
			target_ie = wfd_ie_to_get_func(1, ies, ies_len, NULL, &target_ie_len);
			if (target_ie && target_ie_len) {
				u8 *next_ie = target_ie + target_ie_len;
				uint remain_len = ies_len - (next_ie - ies);

				Func_Of_Proc_Pre_Memmove(target_ie, next_ie, remain_len);
				memset(target_ie + remain_len, 0, target_ie_len);
				ies_len -= target_ie_len;

				index++;
			} else {
				break;
			}
		}
	}
	return ies_len;
}

static uint wfd_attr_to_del_func(u8 * ie, uint ielen_ori, u8 attr_id, u8 flag)
{
	u8 *target_attr;
	u32 target_attr_len;
	uint ielen = ielen_ori;
	int index = 0;

	if (flag) {
		while (1) {
			target_attr =
				wfd_attr_to_get_func(ie, ielen, attr_id, NULL, &target_attr_len, 1);
			if (target_attr && target_attr_len) {
				u8 *next_attr = target_attr + target_attr_len;
				uint remain_len = ielen - (next_attr - ie);

				Func_Of_Proc_Pre_Memmove(target_attr, next_attr, remain_len);
				memset(target_attr + remain_len, 0, target_attr_len);
				*(ie + 1) -= target_attr_len;
				ielen -= target_attr_len;

				index++;
			} else {
				break;
			}
		}
	}
	return ielen;
}

inline u8 *get_wfd_ie_of_bss_ex_func(WLAN_BSSID_EX * bss_ex, u8 * wfd_ie,
								 uint * wfd_ielen)
{
	return wfd_ie_to_get_func(1, BSS_EX_TLV_IES(bss_ex), BSS_EX_TLV_IES_LEN(bss_ex),
						  wfd_ie, wfd_ielen);
}

void del_wfd_ie_of_bss_ex_func(WLAN_BSSID_EX * bss_ex, u8 flag)
{
#define DBG_BSS_EX_DEL_WFD_IE 0
	u8 *ies = BSS_EX_TLV_IES(bss_ex);
	uint ies_len_ori = BSS_EX_TLV_IES_LEN(bss_ex);
	uint ies_len;

	ies_len =
		wfd_ie_to_del_func(ies, ies_len_ori,
					   DBG_BSS_EX_DEL_WFD_IE ? __func__ : NULL, 1);
	if (flag) {
		bss_ex->IELength -= ies_len_ori - ies_len;
	}
}

void del_wfd_attr_of_bss_ex_func(WLAN_BSSID_EX * bss_ex, u8 attr_id)
{
	u8 *ies = BSS_EX_TLV_IES(bss_ex);
	uint ies_len = BSS_EX_TLV_IES_LEN(bss_ex);

	u8 *ie;
	uint ie_len, ie_len_ori;

	int index = 0;

	while (1) {
		ie = wfd_ie_to_get_func(1, ies, ies_len, NULL, &ie_len_ori);
		if (ie) {
			u8 *next_ie_ori = ie + ie_len_ori;
			uint remain_len = bss_ex->IELength - (next_ie_ori - bss_ex->IEs);
			u8 has_target_attr = 0;

			ie_len = wfd_attr_to_del_func(ie, ie_len_ori, attr_id, 1);
			if (ie_len != ie_len_ori) {
				u8 *next_ie = ie + ie_len;

				Func_Of_Proc_Pre_Memmove(next_ie, next_ie_ori, remain_len);
				memset(next_ie + remain_len, 0, ie_len_ori - ie_len);
				bss_ex->IELength -= ie_len_ori - ie_len;

				ies = next_ie;
			} else {
				ies = next_ie_ori;
			}

			ies_len = remain_len;

			index++;
		} else {
			break;
		}
	}
}

int is_empty_essid_of_ieee80211_func(const char *essid, int essid_len)
{
	if (essid_len == 1 && essid[0] == ' ')
		return 1;

	while (essid_len) {
		essid_len--;
		if (essid[essid_len] != '\0')
			return 0;
	}

	return 1;
}

int get_hdrlen_of_ieee80211_func(u16 fc)
{
	int hdrlen = 24;

	switch (WLAN_FC_GET_TYPE(fc)) {
	case WL_IEEE80211_FTYPE_DATA:
		if (fc & WL_IEEE80211_STYPE_QOS_DATA)
			hdrlen += 2;
		if ((fc & WL_IEEE80211_FCTL_FROMDS) && (fc & WL_IEEE80211_FCTL_TODS))
			hdrlen += 6;
		break;
	case WL_IEEE80211_FTYPE_CTL:
		switch (WLAN_FC_GET_STYPE(fc)) {
		case WL_IEEE80211_STYPE_CTS:
		case WL_IEEE80211_STYPE_ACK:
			hdrlen = 10;
			break;
		default:
			hdrlen = 16;
			break;
		}
		break;
	}

	return hdrlen;
}

int cipher_info_to_get_func(struct wlan_network *pnetwork, u8 flag)
{
	u32 wpa_ielen;
	unsigned char *pbuf;
	int group_cipher = 0, pairwise_cipher = 0, is8021x = 0;
	int ret = FALSE;
	pbuf =
		wpa_ie_to_get_func(&pnetwork->network.IEs[12], &wpa_ielen,
					   pnetwork->network.IELength - 12, 1);

	if (pbuf && (wpa_ielen > 0)) {
		MpTrace(COMP_EVENTS, DBG_NORMAL,
		          ("cipher_info_to_get_func: wpa_ielen: %d", wpa_ielen));
		if (TRUE ==
			proc_parse_wpa_ie_func(pbuf, wpa_ielen + 2, &group_cipher,
							 &pairwise_cipher, &is8021x, 1)) {

			pnetwork->BcnInfo.pairwise_cipher = pairwise_cipher;
			if (flag) {
				pnetwork->BcnInfo.group_cipher = group_cipher;
			}
			pnetwork->BcnInfo.is_8021x = is8021x;

			ret = TRUE;
		}
	} else {

		pbuf =
			wpa2_ie_to_get_func(&pnetwork->network.IEs[12], &wpa_ielen,
							pnetwork->network.IELength - 12);

		if (pbuf && (wpa_ielen > 0)) {

			if (TRUE ==
				proc_parse_wpa2_ie_func(pbuf, wpa_ielen + 2, &group_cipher,
								  &pairwise_cipher, &is8021x, 1)) {
				MpTrace(COMP_EVENTS, DBG_NORMAL,
						  ("get RSN IE  OK!!!\n"));
				pnetwork->BcnInfo.pairwise_cipher = pairwise_cipher;
				pnetwork->BcnInfo.group_cipher = group_cipher;
				pnetwork->BcnInfo.is_8021x = is8021x;

				ret = TRUE;
			}
		}
	}

	return ret;
}

void bcn_info_to_get_func(struct wlan_network *pnetwork, u8 flag)
{
	unsigned short cap = 0;
	u8 bencrypt = 0;
	u16 wpa_len = 0, rsn_len = 0;
	struct HT_info_element *pht_info = NULL;
	struct wl_ieee80211_ht_cap *pht_cap = NULL;
	unsigned int len;
	unsigned char *p;

	if (flag) {
		Func_Of_Proc_Pre_Memcpy((u8 *) & cap,
					do_query_data_from_ie(pnetwork->network.IEs, CAPABILITY), 2);
	}
	cap = le16_to_cpu(cap);
	if (cap & WLAN_CAPABILITY_PRIVACY) {
		bencrypt = 1;
		pnetwork->network.Privacy = 1;
	} else {
		pnetwork->BcnInfo.encryp_protocol = ENCRYP_PROTOCOL_OPENSYS;
	}
	sec_ie_to_get_func(pnetwork->network.IEs, pnetwork->network.IELength, NULL,
				   &rsn_len, NULL, &wpa_len, 1);
	MpTrace(COMP_EVENTS, DBG_NORMAL,
			  ("bcn_info_to_get_func: ssid=%s\n", pnetwork->network.Ssid.Ssid));
	MpTrace(COMP_EVENTS, DBG_NORMAL,
			  ("bcn_info_to_get_func: wpa_len=%d rsn_len=%d\n", wpa_len, rsn_len));
	MpTrace(COMP_EVENTS, DBG_NORMAL,
			  ("bcn_info_to_get_func: ssid=%s\n", pnetwork->network.Ssid.Ssid));
	MpTrace(COMP_EVENTS, DBG_NORMAL,
			  ("bcn_info_to_get_func: wpa_len=%d rsn_len=%d\n", wpa_len, rsn_len));

	if (rsn_len > 0) {
		pnetwork->BcnInfo.encryp_protocol = ENCRYP_PROTOCOL_WPA2;
	} else if (wpa_len > 0) {
		pnetwork->BcnInfo.encryp_protocol = ENCRYP_PROTOCOL_WPA;
	} else {
		if (bencrypt)
			pnetwork->BcnInfo.encryp_protocol = ENCRYP_PROTOCOL_WEP;
	}

	cipher_info_to_get_func(pnetwork, 1);

	p = ie_to_get_func(pnetwork->network.IEs + _FIXED_IE_LENGTH_,
				   _HT_CAPABILITY_IE_, &len,
				   pnetwork->network.IELength - _FIXED_IE_LENGTH_);
	if (p && len > 0) {
		pht_cap = (struct wl_ieee80211_ht_cap *)(p + 2);
		pnetwork->BcnInfo.ht_cap_info = pht_cap->cap_info;
	} else {
		pnetwork->BcnInfo.ht_cap_info = 0;
	}
	p = ie_to_get_func(pnetwork->network.IEs + _FIXED_IE_LENGTH_, _HT_ADD_INFO_IE_,
				   &len, pnetwork->network.IELength - _FIXED_IE_LENGTH_);
	if (p && len > 0) {
		pht_info = (struct HT_info_element *)(p + 2);
		pnetwork->BcnInfo.ht_info_infos_0 = pht_info->infos[0];
	} else {
		pnetwork->BcnInfo.ht_info_infos_0 = 0;
	}
}

u16 proc_mcs_rate_func(u8 rf_type, u8 bw_40MHz, u8 short_GI, unsigned char *MCS_rate,
				 u8 flag)
{
	u16 max_rate = 0;

	if (flag) {
		if (MCS_rate[0] & BIT(7))
			max_rate =
				(bw_40MHz) ? ((short_GI) ? 1500 : 1350) : ((short_GI) ? 722 :
														   650);
		else if (MCS_rate[0] & BIT(6))
			max_rate =
				(bw_40MHz) ? ((short_GI) ? 1350 : 1215) : ((short_GI) ? 650 :
														   585);
		else if (MCS_rate[0] & BIT(5))
			max_rate =
				(bw_40MHz) ? ((short_GI) ? 1200 : 1080) : ((short_GI) ? 578 :
														   520);
		else if (MCS_rate[0] & BIT(4))
			max_rate =
				(bw_40MHz) ? ((short_GI) ? 900 : 810) : ((short_GI) ? 433 :
														 390);
		else if (MCS_rate[0] & BIT(3))
			max_rate =
				(bw_40MHz) ? ((short_GI) ? 600 : 540) : ((short_GI) ? 289 :
														 260);
		else if (MCS_rate[0] & BIT(2))
			max_rate =
				(bw_40MHz) ? ((short_GI) ? 450 : 405) : ((short_GI) ? 217 :
														 195);
		else if (MCS_rate[0] & BIT(1))
			max_rate =
				(bw_40MHz) ? ((short_GI) ? 300 : 270) : ((short_GI) ? 144 :
														 130);
		else if (MCS_rate[0] & BIT(0))
			max_rate =
				(bw_40MHz) ? ((short_GI) ? 150 : 135) : ((short_GI) ? 72 : 65);
	}
	return max_rate;
}

int frame_parse_of_action_func(const u8 * frame, u32 frame_len, u8 * category,
						   u8 * action, u8 flag)
{
	const u8 *frame_body = frame + sizeof(struct wl_ieee80211_hdr_3addr);
	u16 fc;
	u8 c;
	u8 a = ACT_PUBLIC_MAX;

	fc = le16_to_cpu(((struct wl_ieee80211_hdr_3addr *)frame)->frame_ctl);

	if ((fc & (WL_IEEE80211_FCTL_FTYPE | WL_IEEE80211_FCTL_STYPE))
		!= (WL_IEEE80211_FTYPE_MGMT | WL_IEEE80211_STYPE_ACTION)
		) {
		return FALSE;
	}

	c = frame_body[0];

	switch (c) {
	case WL_WLAN_CATEGORY_P2P:
		break;
	default:
		a = frame_body[1];
	}

	if (flag) {
		if (category)
			*category = c;
		if (action)
			*action = a;
	}
	return TRUE;
}

static const char *_action_public_str[] = {
	"ACT_PUB_BSSCOEXIST",
	"ACT_PUB_DSE_ENABLE",
	"ACT_PUB_DSE_DEENABLE",
	"ACT_PUB_DSE_REG_LOCATION",
	"ACT_PUB_EXT_CHL_SWITCH",
	"ACT_PUB_DSE_MSR_REQ",
	"ACT_PUB_DSE_MSR_RPRT",
	"ACT_PUB_MP",
	"ACT_PUB_DSE_PWR_CONSTRAINT",
	"ACT_PUB_VENDOR",
	"ACT_PUB_GAS_INITIAL_REQ",
	"ACT_PUB_GAS_INITIAL_RSP",
	"ACT_PUB_GAS_COMEBACK_REQ",
	"ACT_PUB_GAS_COMEBACK_RSP",
	"ACT_PUB_TDLS_DISCOVERY_RSP",
	"ACT_PUB_LOCATION_TRACK",
	"ACT_PUB_RSVD",
};

const char *public_str_action_func(u8 action)
{
	action = (action >= ACT_PUBLIC_MAX) ? ACT_PUBLIC_MAX : action;
	return _action_public_str[action];
}
