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

//Copy from wl_ext.c
unsigned char WL_WPA_OUI[] = { 0x00, 0x50, 0xf2, 0x01 };
unsigned char WMM_OUI[] = { 0x00, 0x50, 0xf2, 0x02 };
unsigned char WPS_OUI[] = { 0x00, 0x50, 0xf2, 0x04 };
unsigned char P2P_OUI[] = { 0x50, 0x6F, 0x9A, 0x09 };
unsigned char WFD_OUI[] = { 0x50, 0x6F, 0x9A, 0x0A };

unsigned char WMM_INFO_OUI[] = { 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01 };
unsigned char WMM_PARA_OUI[] = { 0x00, 0x50, 0xf2, 0x02, 0x01, 0x01 };

unsigned char WPA_TKIP_CIPHER[4] = { 0x00, 0x50, 0xf2, 0x02 };
unsigned char RSN_TKIP_CIPHER[4] = { 0x00, 0x0f, 0xac, 0x02 };



static void _set_cam_cache(PNIC Nic, u8 * mac, u8 * key, u8 id,
							u16 ctrl);
static void cam_cache_clean(PNIC Nic, u8 id);

static unsigned char ARTHEROS_OUI1[] = { 0x00, 0x03, 0x7f };
static unsigned char ARTHEROS_OUI2[] = { 0x00, 0x13, 0x74 };

unsigned char BROADCOM_OUI1[] = { 0x00, 0x10, 0x18 };
unsigned char BROADCOM_OUI2[] = { 0x00, 0x0a, 0xf7 };
unsigned char BROADCOM_OUI3[] = { 0x00, 0x05, 0xb5 };

unsigned char CISCO_OUI[] = { 0x00, 0x40, 0x96 };
unsigned char MARVELL_OUI[] = { 0x00, 0x50, 0x43 };
unsigned char RALINK_OUI[] = { 0x00, 0x0c, 0x43 };
unsigned char WK_WLAN_OUI[] = { 0x00, 0xe0, 0x4c };

unsigned char WK_WLAN_96B_IE[] = { 0x00, 0xe0, 0x4c, 0x02, 0x01, 0x20 };

extern unsigned char WL_WPA_OUI[];
extern unsigned char WPA_TKIP_CIPHER[4];
extern unsigned char RSN_TKIP_CIPHER[4];

#define WAIT_FOR_BCN_TO_MIN	(6000)
#define WAIT_FOR_BCN_TO_MAX	(20000)

static u8 wl_basic_rate_cck[4] = {
	IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
		IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
		IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK
};

static u8 wl_basic_rate_ofdm[3] = {
	IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK,
		IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK
};

static u8 wl_basic_rate_mix[7] = {
	IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
		IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
		IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK,
		IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK
};

static int have_cckrates(unsigned char *rate, int ratelen)
{
	int i;

	for (i = 0; i < ratelen; i++) {
		if ((((rate[i]) & 0x7f) == 2) || (((rate[i]) & 0x7f) == 4) ||
			(((rate[i]) & 0x7f) == 11) || (((rate[i]) & 0x7f) == 22))
			return TRUE;
	}

	return FALSE;

}

static int only_cckrate(unsigned char *rate, int ratelen)
{
	int i;

	for (i = 0; i < ratelen; i++) {
		if ((((rate[i]) & 0x7f) != 2) && (((rate[i]) & 0x7f) != 4) &&
			(((rate[i]) & 0x7f) != 11) && (((rate[i]) & 0x7f) != 22))
			return FALSE;
	}

	return TRUE;
}

static s8 do_query_tx_nss(struct sta_info *psta, PNIC Nic)
{
	u8 rf_type = 3, custom_rf_type;
	s8 nss = 1;

	custom_rf_type = Nic->registrypriv.rf_config;
	Func_Of_Proc_Chip_Hw_Get_Hwreg(Nic, HW_VAR_RF_TYPE, (u8 *) (&rf_type));

	if (!psta)
		return nss;

	if (custom_rf_type != 0xF)
		rf_type = custom_rf_type;

	if (psta->htpriv.ht_option) {
		nss = 1;
	}

	MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s: %d SS, rf_type=%d\n", __FUNCTION__, nss, rf_type));
	return nss;
}

u8 get_netype_to_raid_ex(PNIC Nic, struct sta_info * psta, u8 tag)
{
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	u8 raid = RATEID_IDX_BGN_40M_1SS, cur_rf_type, rf_type, custom_rf_type;
	s8 tx_nss;
	if (tag) {
		tx_nss = do_query_tx_nss(psta, Nic);

		switch (psta->wireless_mode) {
		case WIRELESS_11B:
			raid = RATEID_IDX_B;
			break;

		case WIRELESS_11G:
			raid = RATEID_IDX_G;
			break;
		case WIRELESS_11BG:
			raid = RATEID_IDX_BG;
			break;
		case WIRELESS_11_24N:
		case WIRELESS_11G_24N:
			if (tx_nss == 1)
				raid = RATEID_IDX_GN_N1SS;
			else
				MpTrace(COMP_EVENTS, DBG_NORMAL, ("tx_nss error!(tx_nss=%d)\n", tx_nss));
			break;
		case WIRELESS_11B_24N:
		case WIRELESS_11BG_24N:
			if (psta->bw_mode == CHANNEL_WIDTH_20) {
				if (tx_nss == 1)
					raid = RATEID_IDX_BGN_20M_1SS_BN;
				else
					MpTrace(COMP_EVENTS, DBG_NORMAL, ("tx_nss error!(tx_nss=%d)\n", tx_nss));
			} else {
				if (tx_nss == 1)
					raid = RATEID_IDX_BGN_40M_1SS;
				else
					MpTrace(COMP_EVENTS, DBG_NORMAL, ("tx_nss error!(tx_nss=%d)\n", tx_nss));
			}
			break;
		default:
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("unexpected wireless mode!(psta->wireless_mode=%x)\n", 
				psta->wireless_mode));
			break;

		}

	}
	return raid;

}

static unsigned char rmap2wifirate(unsigned char rate)
{
	unsigned char val = 0;

	switch (rate & 0x7f) {
	case 0:
		val = IEEE80211_CCK_RATE_1MB;
		break;

	case 1:
		val = IEEE80211_CCK_RATE_2MB;
		break;

	case 2:
		val = IEEE80211_CCK_RATE_5MB;
		break;

	case 3:
		val = IEEE80211_CCK_RATE_11MB;
		break;

	case 4:
		val = IEEE80211_OFDM_RATE_6MB;
		break;

	case 5:
		val = IEEE80211_OFDM_RATE_9MB;
		break;

	case 6:
		val = IEEE80211_OFDM_RATE_12MB;
		break;

	case 7:
		val = IEEE80211_OFDM_RATE_18MB;
		break;

	case 8:
		val = IEEE80211_OFDM_RATE_24MB;
		break;

	case 9:
		val = IEEE80211_OFDM_RATE_36MB;
		break;

	case 10:
		val = IEEE80211_OFDM_RATE_48MB;
		break;

	case 11:
		val = IEEE80211_OFDM_RATE_54MB;
		break;

	}

	return val;

}

static int chk_basicrate(PNIC Nic, unsigned char rate)
{
	int i;
	unsigned char val;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;

	for (i = 0; i < NumRates; i++) {
		val = pmlmeext->basicrate[i];

		if ((val != 0xff) && (val != 0xfe)) {
			if (rate == rmap2wifirate(val)) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

static unsigned int rmap2rateset(PNIC Nic, unsigned char *rateset)
{
	int i;
	unsigned int len = 0;
	unsigned char rate;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;

	for (i = 0; i < NumRates; i++) {
		rate = pmlmeext->datarate[i];

		switch (rate) {
		case 0xff:
			return len;

		case 0xfe:
			continue;

		default:
			rate = rmap2wifirate(rate);

			if (chk_basicrate(Nic, rate) == TRUE) {
				rate |= IEEE80211_BASIC_RATE_MASK;
			}

			rateset[len] = rate;
			len++;
			break;
		}
	}
	return len;
}

void query_rate_set(PNIC Nic, unsigned char *pbssrate,
				  int *bssrate_len, u8 tag)
{
	unsigned char supportedrates[NumRates];
	if (tag) {
		memset(supportedrates, 0, NumRates);
		*bssrate_len = rmap2rateset(Nic, supportedrates);
		Func_Of_Proc_Pre_Memcpy(pbssrate, supportedrates, *bssrate_len);
	}
}

void do_set_mcs_by_flag(u8 * mcs_set, u32 mask, u8 tag)
{
	if (tag) {
		u8 mcs_rate_1r = (u8) (mask & 0xff);
		u8 mcs_rate_2r = (u8) ((mask >> 8) & 0xff);
		u8 mcs_rate_3r = (u8) ((mask >> 16) & 0xff);
		u8 mcs_rate_4r = (u8) ((mask >> 24) & 0xff);

		mcs_set[0] &= mcs_rate_1r;
		mcs_set[1] &= mcs_rate_2r;
		mcs_set[2] &= mcs_rate_3r;
		mcs_set[3] &= mcs_rate_4r;
	}
}

void do_renew_Bratelist(IN PNIC Nic, IN u8 * mBratesOS)
{
	u8 i;
	u8 rate;

	for (i = 0; i < NDIS_802_11_LENGTH_RATES_EX; i++) {
		rate = mBratesOS[i] & 0x7f;
		switch (rate) {
		case IEEE80211_CCK_RATE_1MB:
		case IEEE80211_CCK_RATE_2MB:
		case IEEE80211_CCK_RATE_5MB:
		case IEEE80211_CCK_RATE_11MB:
		case IEEE80211_OFDM_RATE_6MB:
		case IEEE80211_OFDM_RATE_12MB:
		case IEEE80211_OFDM_RATE_24MB:
			mBratesOS[i] |= IEEE80211_BASIC_RATE_MASK;
			break;
		}
	}

}

void do_set_MSR(PNIC Nic, u8 type)
{
#ifdef CONFIG_CONCURRENT_MODE
	if (Nic->iface_type == IFACE_PORT1) {
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_MEDIA_STATUS1, (u8 *) (&type));
	} else
#endif
	{
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_MEDIA_STATUS, (u8 *) (&type));
	}
}

inline u8 do_query_var(PNIC Nic, u8 type)
{
	u8 out = 0;

	switch (type) {
	case CHANNEL:
		out = Nic->dvobjpriv.oper_channel;
		break;
	case BW:
		out = Nic->dvobjpriv.oper_bwmode;
		break;
	case CH_OFFSET:
		out = Nic->dvobjpriv.oper_ch_offset;
		break;
	default:
		MpTrace(COMP_EVENTS, DBG_NORMAL, ("[%s] Invalid type\n", __FUNCTION__));
		break;
	}
	return out;
}

inline void do_set_var(PNIC Nic, u8 type, u8 var)
{

	u8 ch;
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;

	if (type == CHANNEL) {
		ch = var;

		if (dvobj->oper_channel != ch) {
			dvobj->on_oper_ch_time = Func_Of_Proc_Get_Current_Time();

		}

		dvobj->oper_channel = ch;

	} else if (type == BW) {
		Nic->dvobjpriv.oper_bwmode = var;
	} else if (type == CH_OFFSET) {
		Nic->dvobjpriv.oper_ch_offset = var;
	} else {
		MpTrace(COMP_EVENTS, DBG_NORMAL, ("[%s] Invalid type\n", __FUNCTION__));
	}

	return;
}

static u8 do_query_center_ch(u8 chnl_bw, u8 channel, u8 chnl_offset)
{
	u8 center_ch = channel;

	if (chnl_bw == CHANNEL_WIDTH_40) {
		if (chnl_offset == HAL_PRIME_CHNL_OFFSET_LOWER)
			center_ch = channel + 2;
		else
			center_ch = channel - 2;
	}

	return center_ch;
}

void do_chose_chan(PNIC Nic, unsigned char channel, u8 tag)
{
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	if (tag) {
		
		NDIS_WAIT_FOR_MUTEX(&Nic->dvobjpriv.setch_mutex);

		do_set_var(Nic, CHANNEL, channel);

		Func_Chip_Bb_Handle_Sw_Chnl_And_Set_Bw_Process(Nic, TRUE, FALSE, channel, 0, 0,
									  channel);
		
		NDIS_RELEASE_MUTEX(&Nic->dvobjpriv.setch_mutex);
	}
}

void do_set_chabw(PNIC Nic, unsigned char channel,
						unsigned char channel_offset, unsigned short bwmode)
{
	u8 center_ch;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;

	if (Nic->bNotifyChannelChange) {
		MpTrace(COMP_EVENTS, DBG_NORMAL, ("[%s] ch = %d, offset = %d, bwmode = %d\n", __FUNCTION__,
			channel, channel_offset, bwmode));
	}

	center_ch = do_query_center_ch(bwmode, channel, channel_offset);

	//NDIS_WAIT_FOR_MUTEX(&Nic->dvobjpriv.setch_mutex);


	do_set_var(Nic, CHANNEL, channel);
	do_set_var(Nic, BW, bwmode);
	do_set_var(Nic, CH_OFFSET, channel_offset);

	Func_Of_Proc_Chip_Hw_Set_Chnl_Bw(Nic, center_ch, bwmode, channel_offset);

	//NDIS_RELEASE_MUTEX(&Nic->dvobjpriv.setch_mutex);

}

__inline u8 *query_bssid(WLAN_BSSID_EX * pnetwork)
{
	return (pnetwork->MacAddress);
}

int chk_sta_associated_to_ap(PNIC Nic, u8 tag)
{
	struct mlme_ext_priv *pmlmeext;
	struct mlme_ext_info *pmlmeinfo;

	if (!Nic)
		return FALSE;
	if (tag) {
		pmlmeext = &Nic->mlmeextpriv;
		pmlmeinfo = &(pmlmeext->mlmext_info);

		if ((pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS)
			&& ((pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE)) {
			return TRUE;
		} else {
			return FALSE;
		}
	} else {
		return FALSE;
	}
}

void invalidate_cam_all(PNIC Nic, u8 tag)
{
	//_irqL irqL;
	u8 val8 = 0;
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	struct sec_cam_bmp *map = &cam_ctl->used;
	if (tag) {
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_CAM_INVALID_ALL, &val8);

//		spin_lock_bh(&cam_ctl->lock);
		map->m0 = 0;
		memset(dvobj->cam_cache, 0,
			   sizeof(struct sec_cam_ent) * SEC_CAM_ENT_NUM_SW_LIMIT);
//		spin_unlock_bh(&cam_ctl->lock);
	}
}

static void _do_clear_cam(PNIC Nic, u8 entry)
{
	unsigned char null_sta[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char null_key[] =
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00 };

	Func_Of_Proc_Sec_Write_Cam_Ent(Nic, entry, 0, null_sta, null_key);
}

 void _set_cam(PNIC Nic, u8 * mac, u8 * key, u8 id, u16 ctrl)
{
#ifdef CONFIG_WRITE_CACHE_ONLY
	_set_cam_cache(Nic, mac, key, id, ctrl);
#else
	Func_Of_Proc_Sec_Write_Cam_Ent(Nic, id, ctrl, mac, key);
	_set_cam_cache(Nic, mac, key, id, ctrl);
#endif
}

void cam_entry_clean(PNIC Nic, u8 id)
{
	_do_clear_cam(Nic, id);
	cam_cache_clean(Nic, id);
}

static void _set_cam_cache(PNIC Nic, u8 * mac, u8 * key, u8 id,
							u16 ctrl)
{
	//_irqL irqL;
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

//	spin_lock_bh(&cam_ctl->lock);

	dvobj->cam_cache[id].ctrl = ctrl;
	Func_Of_Proc_Pre_Memcpy(dvobj->cam_cache[id].mac, mac, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(dvobj->cam_cache[id].key, key, 16);

//	spin_unlock_bh(&cam_ctl->lock);
}

static void cam_cache_clean(PNIC Nic, u8 id)
{
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

//	spin_lock_bh(&cam_ctl->lock);

	memset(&(dvobj->cam_cache[id]), 0, sizeof(struct sec_cam_ent));

//	spin_unlock_bh(&cam_ctl->lock);
}
inline bool _wl_chk_camctl_cap(PNIC Nic, u8 cap)
{
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	if (cam_ctl->sec_cap & cap)
		return TRUE;
	return FALSE;
}

static inline void _wl_camctl_set_mask(PNIC Nic, u32 flags)
{
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	cam_ctl->flags |= flags;
}

inline void wl_set_camctl_mask(PNIC Nic, u32 flags)
{
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

//	spin_lock_bh(&cam_ctl->lock);
	_wl_camctl_set_mask(Nic, flags);
//	spin_unlock_bh(&cam_ctl->lock);
}
bool _wl_chk_camctl_mask(PNIC Nic, u32 flags)
//inline bool _wl_chk_camctl_mask(PNIC Nic, u32 flags)
{
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	if (cam_ctl->flags & flags)
		return TRUE;
	return FALSE;
}

static inline bool chk_sec_camid_set(struct sec_cam_bmp *map, u8 id)
{
	if (id < 32)
		return (map->m0 & BIT(id));
	else
		wl_warn_on(1);

	return 0;
}

static inline void doset_sec_cam_list(struct sec_cam_bmp *map, u8 id)
{
	if (id < 32)
		map->m0 |= BIT(id);
	else
		wl_warn_on(1);
}

static inline void doclr_sec_cam_list(struct sec_cam_bmp *map, u8 id)
{
	if (id < 32)
		map->m0 &= ~BIT(id);
	else
		wl_warn_on(1);
}

static bool _chk_sec_camid_used(struct cam_ctl_t *cam_ctl, u8 id)
{
	bool ret = FALSE;

	if (id >= cam_ctl->num) {
		wl_warn_on(1);
		goto exit;
	}

	ret = chk_sec_camid_set(&cam_ctl->used, id);

exit:
	return ret;
}

static inline bool _chk_camid_gk(PNIC Nic, u8 cam_id)
{
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	bool ret = FALSE;

	if (cam_id >= cam_ctl->num) {
		wl_warn_on(1);
		goto exit;
	}

	if (_chk_sec_camid_used(cam_ctl, cam_id) == FALSE)
		goto exit;

	ret = (dvobj->cam_cache[cam_id].ctrl & BIT6) ? TRUE : FALSE;

exit:
	return ret;
}

inline bool chk_camid_gk(PNIC Nic, u8 cam_id, u8 tag)
{
	bool ret = 0;
	//_irqL irqL;
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	if (tag) {
//		spin_lock_bh(&cam_ctl->lock);
		ret = _chk_camid_gk(Nic, cam_id);
//		spin_unlock_bh(&cam_ctl->lock);
	}
	return ret;
}

static bool chk_cam_cache(PNIC Nic, s16 kid, s8 gk, u8 id, u8 * addr)
{
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	bool ret = FALSE;

	if (addr && Func_Of_Proc_Pre_Memcmp(dvobj->cam_cache[id].mac, addr, ETH_ALEN) == FALSE)
		goto exit;
	if (kid >= 0 && kid != (dvobj->cam_cache[id].ctrl & 0x03))
		goto exit;
	if (gk != -1 && (gk ? TRUE : FALSE) != _chk_camid_gk(Nic, id))
		goto exit;

	ret = TRUE;

exit:
	return ret;
}

static s16 _do_camid_search(s16 kid, s8 gk, PNIC Nic, u8 * addr)
{
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	int i;
	s16 cam_id = -1;

	for (i = 0; i < cam_ctl->num; i++) {
		if (chk_cam_cache(Nic, kid, gk, i, addr)) {
			cam_id = i;
			break;
		}
	}

	return cam_id;
}

s16 do_camid_search(s16 kid, s8 gk, PNIC Nic, u8 * addr)
{
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	//_irqL irqL;
	s16 cam_id = -1;

//	spin_lock_bh(&cam_ctl->lock);
	cam_id = _do_camid_search(kid, gk, Nic, addr);
//	spin_unlock_bh(&cam_ctl->lock);

	return cam_id;
}

s16 wl_new_camid(PNIC Nic, struct sta_info * sta, u8 kid,
					bool * used, u8 tag)
{
	s16 cam_id = -1;
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	struct mlme_ext_info *mlmeinfo = &Nic->mlmeextpriv.mlmext_info;

	*used = FALSE;

//	spin_lock_bh(&cam_ctl->lock);
	{

		if ((((mlmeinfo->state & 0x03) == WIFI_FW_AP_STATE)
			 || ((mlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE))
			&& !sta) {
			if (kid > 3) {
				MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s group key with invalid key id:%u\n"), __FUNCTION__,kid);
				wl_warn_on(1);
				goto bitmap_handle;
			}
			cam_id = kid;
		} else {
			int i;
			u8 *addr = sta ? sta->hwaddr : NULL;

			u8 start_id = 0;

			if (!sta) {
				if (!(mlmeinfo->state & WIFI_FW_ASSOC_SUCCESS)) {
					goto bitmap_handle;
				}

				addr = get_bssid(&Nic->mlmepriv);
			}

			if (_wl_chk_camctl_cap(Nic, SEC_CAP_CHK_BMC) == TRUE)
				i = _do_camid_search(kid, sta ? FALSE : TRUE, Nic, addr);
			else
				i = _do_camid_search(kid, -1, Nic, addr);

			if (i >= 0) {
				cam_id = i;
				goto bitmap_handle;
			}

			for (i = 0; i < cam_ctl->num; i++) {
				if (((i + start_id) % cam_ctl->num) < 4)
					continue;

				if (_chk_sec_camid_used
					(cam_ctl, ((i + start_id) % cam_ctl->num)) == FALSE)
					break;
			}

			if (i == cam_ctl->num) {
				if (sta)
					MpTrace(COMP_EVENTS, DBG_NORMAL, ("pairwise key with "), MAC_FMT" id:%u no room\n", 
						MAC_ARG(addr), kid)
				else
					MpTrace(COMP_EVENTS, DBG_NORMAL, ("group key with "), MAC_FMT" id:%u no room\n",
						MAC_ARG(addr), kid)
				wl_warn_on(1);
				goto bitmap_handle;
			}

			cam_id = ((i + start_id) % cam_ctl->num);
			start_id = ((i + start_id + 1) % cam_ctl->num);
		}
	}

bitmap_handle:
	if (cam_id >= 0) {
		*used = _chk_sec_camid_used(cam_ctl, cam_id);
		doset_sec_cam_list(&cam_ctl->used, cam_id);
	}

//	spin_unlock_bh(&cam_ctl->lock);

	return cam_id;
}

void wl_unnew_camid(PNIC Nic, u8 cam_id)
{
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

//	spin_lock_bh(&cam_ctl->lock);

	if (cam_id < cam_ctl->num)
		doclr_sec_cam_list(&cam_ctl->used, cam_id);

//	spin_unlock_bh(&cam_ctl->lock);
}

void do_flush_all_cam(PNIC Nic, u8 tag)
{
	u8 temp;
	struct mlme_priv *pmlmepriv = &(Nic->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	if (tag) {
#ifdef CONFIG_CONCURRENT_MODE
		if (do_chk_partner_fwstate(Nic, _FW_LINKED) == TRUE) {
			if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
				struct sta_priv *pstapriv = &Nic->stapriv;
				struct sta_info *psta;

				psta =
					do_query_stainfo(pstapriv, pmlmeinfo->network.MacAddress, 1);
				if (psta) {
					if (psta->state & WIFI_AP_STATE) {
					} else {
#if 0
						proc_clearstakey_cmd_func(Nic, psta, FALSE, 1);
#endif

					}
				}
			} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == TRUE) {
				int i, cam_id;
				u8 null_addr[ETH_ALEN] = { 0, 0, 0, 0, 0, 0 };

				for (i = 0; i < 4; i++) {
					cam_id = do_camid_search(i, -1, Nic, null_addr);
					if (cam_id >= 0) {
						cam_entry_clean(Nic, cam_id);
						wl_unnew_camid(Nic, cam_id);
					}
				}

				temp = FALSE;
				Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_SEC_DK_CFG, &temp);

			}
		} else
#endif
		{
			invalidate_cam_all(Nic, 1);
			temp = FALSE;
			Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_SEC_DK_CFG, &temp);
		}
	}
}


int WMM_param_handler(PNIC Nic, PNDIS_802_11_VARIABLE_IEs pIE)
{
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct mlme_priv *pmlmepriv = &(Nic->mlmepriv);

	if (pmlmepriv->qospriv.qos_option == 0) {
		pmlmeinfo->WMM_enable = 0;
		return FALSE;
	}

	if (Func_Of_Proc_Pre_Memcmp
		(&(pmlmeinfo->WMM_param), (pIE->data + 6),
		 sizeof(struct WMM_para_element))) {
		return FALSE;
	} else {
		Func_Of_Proc_Pre_Memcpy(&(pmlmeinfo->WMM_param), (pIE->data + 6),
					sizeof(struct WMM_para_element));
	}
	pmlmeinfo->WMM_enable = 1;
	return TRUE;

}

void do_WMMAssocRsp_work(PNIC Nic, u8 tag)
{
	u16 TXOP;
	u32 acParm, i;
	u8 acm_mask;
	u32 edca[4], inx[4];
	u8 ACI, ACM, AIFS, ECWMin, ECWMax, a_SifsTime;
	                       //aSifsTime  defined in  hw_80211.h
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct xmit_priv *pxmitpriv = &Nic->xmitpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct registry_priv *pregpriv = &Nic->registrypriv;

	acm_mask = 0;

	if (pmlmeext->cur_wireless_mode & WIRELESS_11_24N)
		a_SifsTime = 16;
	else
		a_SifsTime = 10;

	if (pmlmeinfo->WMM_enable == 0) {
		Nic->mlmepriv.acm_mask = 0;

		AIFS = aSifsTime + (2 * pmlmeinfo->slotTime);

		if (pmlmeext->cur_wireless_mode & (WIRELESS_11G)) {
			ECWMax = 10;
			ECWMin = 4;
		} else if (pmlmeext->cur_wireless_mode & WIRELESS_11B) {
			ECWMax = 10;
			ECWMin = 5;
		} else {
			ECWMax = 10;
			ECWMin = 4;
		}

		TXOP = 0;
		acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_AC_PARAM_BE, (u8 *) (&acParm));
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_AC_PARAM_BK, (u8 *) (&acParm));
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_AC_PARAM_VI, (u8 *) (&acParm));

		TXOP = 0x2f;
		ECWMax = 3;
		ECWMin = 2;
		acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_AC_PARAM_VO, (u8 *) (&acParm));
	} else {
		edca[0] = edca[1] = edca[2] = edca[3] = 0;

		for (i = 0; i < 4; i++) {
			ACI = (pmlmeinfo->WMM_param.ac_param[i].ACI_AIFSN >> 5) & 0x03;
			ACM = (pmlmeinfo->WMM_param.ac_param[i].ACI_AIFSN >> 4) & 0x01;

			AIFS =
				(pmlmeinfo->WMM_param.ac_param[i].ACI_AIFSN & 0x0f) *
				pmlmeinfo->slotTime + aSifsTime;

			ECWMin = (pmlmeinfo->WMM_param.ac_param[i].CW & 0x0f);
			ECWMax = (pmlmeinfo->WMM_param.ac_param[i].CW & 0xf0) >> 4;
			TXOP = le16_to_cpu(pmlmeinfo->WMM_param.ac_param[i].TXOP_limit);

			acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);

			switch (ACI) {
			case 0x0:
				Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_AC_PARAM_BE,
								  (u8 *) (&acParm));
				acm_mask |= (ACM ? BIT(1) : 0);
				edca[XMIT_BE_QUEUE] = acParm;
				break;

			case 0x1:
				Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_AC_PARAM_BK,
								  (u8 *) (&acParm));
				edca[XMIT_BK_QUEUE] = acParm;
				break;

			case 0x2:
				Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_AC_PARAM_VI,
								  (u8 *) (&acParm));
				acm_mask |= (ACM ? BIT(2) : 0);
				edca[XMIT_VI_QUEUE] = acParm;
				break;

			case 0x3:
				Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_AC_PARAM_VO,
								  (u8 *) (&acParm));
				acm_mask |= (ACM ? BIT(3) : 0);
				edca[XMIT_VO_QUEUE] = acParm;
				break;
			}

			MpTrace(COMP_EVENTS, DBG_NORMAL, ("WMM(%x): %x, %x\n"), ACI, ACM, acParm);
		}

		if (Nic->registrypriv.acm_method == 1)
			Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_ACM_CTRL, (u8 *) (&acm_mask));
		else
			Nic->mlmepriv.acm_mask = acm_mask;

		inx[0] = 0;
		inx[1] = 1;
		inx[2] = 2;
		inx[3] = 3;

		for (i = 0; i < 4; i++) {
			pxmitpriv->wmm_para_seq[i] = inx[i];
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("wmm_para_seq(%d): %d\n", i, pxmitpriv->wmm_para_seq[i]));
		}
	}
}

static void chk_bwmode_renew(PNDIS_802_11_VARIABLE_IEs pIE,
								PNIC Nic)
{
#ifdef CONFIG_80211N_HT
	u8 cbw40_enable = 0;
	unsigned char new_ch_offset;
	unsigned char new_bwmode;
	struct HT_info_element *pHT_info;
	struct mlme_priv *pmlmepriv = &(Nic->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;
	struct registry_priv *pregistrypriv = &Nic->registrypriv;

	if (!pIE)
		return;

	if (phtpriv->ht_option == FALSE)
		return;

	if (pIE->Length > sizeof(struct HT_info_element))
		return;

	if (pmlmeext->cur_channel > 14)
		return;

	pHT_info = (struct HT_info_element *)pIE->data;

	if (Func_Chip_Hw_Chk_Bw_Cap(Nic, BW_CAP_40M)) {
		if((pregistrypriv->bw_mode & 0x0F)>=CHANNEL_WIDTH_40)
			//REGSTY_IS_BW_2G_SUPPORT
			cbw40_enable = 1;
	}

	if ((pHT_info->infos[0] & BIT(2)) && cbw40_enable) {
		new_bwmode = CHANNEL_WIDTH_40;

		switch (pHT_info->infos[0] & 0x3) {
		case 1:
			new_ch_offset = HAL_PRIME_CHNL_OFFSET_LOWER;
			break;

		case 3:
			new_ch_offset = HAL_PRIME_CHNL_OFFSET_UPPER;
			break;

		default:
			new_bwmode = CHANNEL_WIDTH_20;
			new_ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
			break;
		}
	} else {
		new_bwmode = CHANNEL_WIDTH_20;
		new_ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	}

	if ((new_bwmode != pmlmeext->cur_bwmode
		 || new_ch_offset != pmlmeext->cur_ch_offset)
		&& new_bwmode < pmlmeext->cur_bwmode) {
		pmlmeinfo->bwmode_updated = TRUE;

		pmlmeext->cur_bwmode = new_bwmode;
		pmlmeext->cur_ch_offset = new_ch_offset;

		HT_info_handler(Nic, pIE, 1);
	} else {
		pmlmeinfo->bwmode_updated = FALSE;
	}

	if (TRUE == pmlmeinfo->bwmode_updated) {
		struct sta_info *psta;
		WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
		struct sta_priv *pstapriv = &Nic->stapriv;
#if 0
		psta = do_query_stainfo(pstapriv, cur_network->MacAddress, 1);
#else
		psta = pstapriv;
#endif
		if (psta) {
			struct ht_priv *phtpriv_sta = &psta->htpriv;

			if (phtpriv_sta->ht_option) {
				psta->bw_mode = pmlmeext->cur_bwmode;
				phtpriv_sta->ch_offset = pmlmeext->cur_ch_offset;
			} else {
				psta->bw_mode = CHANNEL_WIDTH_20;
				phtpriv_sta->ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
			}
#if 0
			proc_dm_ra_mask_wk_cmd_func(Nic, (u8 *) psta, 1);
#endif
		}

	}
#endif
}

void HT_caps_handler(PNIC Nic, PNDIS_802_11_VARIABLE_IEs pIE, u8 tag)
{
#ifdef CONFIG_80211N_HT
	unsigned int i;
	u8 rf_type = 3;
	u8 max_AMPDU_len, min_MPDU_spacing;
	u8 cur_ldpc_cap = 0, cur_stbc_cap = 0, cur_beamform_cap = 0;
	struct registry_priv *pregistrypriv = &Nic->registrypriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;

	if (pIE == NULL)
		return;

	if (phtpriv->ht_option == FALSE)
		return;
	if (tag) {
		pmlmeinfo->HT_caps_enable = 1;

		for (i = 0; i < (pIE->Length); i++) {
			if (i != 2) {
				pmlmeinfo->HT_caps.u.HT_cap[i] &= (pIE->data[i]);
			} else {

				if ((pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x3) >
					(pIE->data[i] & 0x3)) {
					max_AMPDU_len = (pIE->data[i] & 0x3);
				} else {
					max_AMPDU_len =
						(pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x3);
				}

				if ((pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x1c) >
					(pIE->data[i] & 0x1c)) {
					min_MPDU_spacing =
						(pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x1c);
				} else {
					min_MPDU_spacing = (pIE->data[i] & 0x1c);
				}

				pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para =
					max_AMPDU_len | min_MPDU_spacing;
			}
		}

		pmlmeinfo->HT_caps.u.HT_cap_element.HT_caps_info =
			le16_to_cpu(pmlmeinfo->HT_caps.u.HT_cap_element.HT_caps_info);
		pmlmeinfo->HT_caps.u.HT_cap_element.HT_ext_caps =
			le16_to_cpu(pmlmeinfo->HT_caps.u.HT_cap_element.HT_ext_caps);

		Func_Of_Proc_Chip_Hw_Get_Hwreg(Nic, HW_VAR_RF_TYPE, (u8 *) (&rf_type));

		for (i = 0; i < 16; i++)
			pmlmeinfo->HT_caps.u.HT_cap_element.MCS_rate[i] &=
				pmlmeext->default_supported_mcs_set[i];

		if (rf_type == 3) {
			do_set_mcs_by_flag(pmlmeinfo->HT_caps.u.HT_cap_element.MCS_rate,
								 MCS_RATE_1R, 1);
		}

		if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
			if (TEST_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_TX)
				&& GET_HT_CAP_ELE_TX_STBC(pIE->data)) {
				SET_FLAG(cur_stbc_cap, STBC_HT_ENABLE_TX);
				MpTrace(COMP_EVENTS, DBG_NORMAL, ("Enable HT Tx STBC !\n"));
			}
			phtpriv->stbc_cap = cur_stbc_cap;

		} else {
			if (TEST_FLAG(phtpriv->ldpc_cap, LDPC_HT_ENABLE_TX)
				&& GET_HT_CAP_ELE_LDPC_CAP(pIE->data)) {
				SET_FLAG(cur_ldpc_cap, (LDPC_HT_ENABLE_TX | LDPC_HT_CAP_TX));
				MpTrace(COMP_EVENTS, DBG_NORMAL, ("Enable HT Tx LDPC!\n"));
			}
			phtpriv->ldpc_cap = cur_ldpc_cap;

			if (TEST_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_TX)
				&& GET_HT_CAP_ELE_RX_STBC(pIE->data)) {
				SET_FLAG(cur_stbc_cap, (STBC_HT_ENABLE_TX | STBC_HT_CAP_TX));
				MpTrace(COMP_EVENTS, DBG_NORMAL, ("Enable HT Tx STBC!\n"));
			}
			phtpriv->stbc_cap = cur_stbc_cap;
		}
	}
#endif
}

void HT_info_handler(PNIC Nic, PNDIS_802_11_VARIABLE_IEs pIE, u8 tag)
{
#ifdef CONFIG_80211N_HT
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;

	if (pIE == NULL)
		return;

	if (phtpriv->ht_option == FALSE)
		return;

	if (pIE->Length > sizeof(struct HT_info_element))
		return;
	if (tag) {
		pmlmeinfo->HT_info_enable = 1;
		Func_Of_Proc_Pre_Memcpy(&(pmlmeinfo->HT_info), pIE->data, pIE->Length);
	}
#endif
	return;
}

void do_Htassocrsp_work(PNIC Nic)
{
	unsigned char min_MPDU_spacing;
	unsigned char max_AMPDU_len;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s\n", __FUNCTION__));

	if ((pmlmeinfo->HT_info_enable) && (pmlmeinfo->HT_caps_enable)) {
		pmlmeinfo->HT_enable = 1;
	} else {
		pmlmeinfo->HT_enable = 0;
		return;
	}

	max_AMPDU_len = pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x03;

	min_MPDU_spacing =
		(pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x1c) >> 2;

	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_AMPDU_MIN_SPACE,
					  (u8 *) (&min_MPDU_spacing));

	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_AMPDU_FACTOR, (u8 *) (&max_AMPDU_len));

}

void ERP_IE_handler(PNIC Nic, PNDIS_802_11_VARIABLE_IEs pIE, u8 tag)
{
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	if (pIE->Length > 1)
		return;
	if (tag) {
		pmlmeinfo->ERP_enable = 1;
		Func_Of_Proc_Pre_Memcpy(&(pmlmeinfo->ERP_IE), pIE->data, pIE->Length);
	}
}

void do_renew_VCS(PNIC Nic, struct sta_info *psta)
{
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct registry_priv *pregpriv = &Nic->registrypriv;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	switch (pregpriv->vwltcarrier_sense) {
	case 0:
		psta->cts2self = 0;
		psta->rtsen = 0;
		break;

	case 1:
		if (pregpriv->vcs_type == 1) {
			psta->cts2self = 0;
			psta->rtsen = 1;
		} else {
			psta->cts2self = 1;
			psta->rtsen = 0;
		}
		break;

	case 2:
	default:
		if (((pmlmeinfo->ERP_enable) && (pmlmeinfo->ERP_IE & BIT(1)))
			) {
			if (pregpriv->vcs_type == 1) {
				psta->cts2self = 0;
				psta->rtsen = 1;
			} else {
				psta->cts2self = 1;
				psta->rtsen = 0;
			}
		} else {
			psta->cts2self = 0;
			psta->rtsen = 0;
		}
		break;
	}
}

void do_renew_ldpc_stbc_cap(struct sta_info *psta, u8 tag)
{
#ifdef CONFIG_80211N_HT
	if (tag) {
		if (psta->htpriv.ht_option) {
			if (TEST_FLAG(psta->htpriv.ldpc_cap, LDPC_HT_ENABLE_TX))
				psta->ldpc = 1;

			if (TEST_FLAG(psta->htpriv.stbc_cap, STBC_HT_ENABLE_TX))
				psta->stbc = 1;
		} else {
			psta->stbc = 0;
			psta->ldpc = 0;
		}
	}
#endif
}

int do_query_bcn_keys(PNIC Nic,
					 struct beacon_keys *recv_beacon, u8 * pframe,
					 u32 packet_len)
{
	int left;
	u16 capability;
	unsigned char *pos;
	struct wl_ieee802_11_elems elems;
	struct HT_info_element *pht_info = NULL;
	struct wl_ieee80211_ht_cap *pht_cap = NULL;

	memset(recv_beacon, 0, sizeof(*recv_beacon));

	capability =
		le16_to_cpu(*(unsigned short *)(pframe + WLAN_HDR_A3_LEN + 10));

	left =
		packet_len - sizeof(struct wl_ieee80211_hdr_3addr) -
		_BEACON_IE_OFFSET_;
	pos = pframe + sizeof(struct wl_ieee80211_hdr_3addr) + _BEACON_IE_OFFSET_;
	if (proc_ieee802_11_parse_elems_func(pos, left, &elems, 1, 1) == ParseFailed)
		return FALSE;

	if (elems.ht_capabilities) {
		if (elems.ht_capabilities_len != sizeof(*pht_cap))
			return FALSE;

		pht_cap = (struct wl_ieee80211_ht_cap *)elems.ht_capabilities;
		recv_beacon->ht_cap_info = pht_cap->cap_info;
	}

	if (elems.ht_operation) {
		if (elems.ht_operation_len != sizeof(*pht_info))
			return FALSE;

		pht_info = (struct HT_info_element *)elems.ht_operation;
		recv_beacon->ht_info_infos_0_sco = pht_info->infos[0] & 0x03;
	}

	if (elems.ds_params
		&& elems.ds_params_len == sizeof(recv_beacon->bcn_channel))
		Func_Of_Proc_Pre_Memcpy(&recv_beacon->bcn_channel, elems.ds_params,
					sizeof(recv_beacon->bcn_channel));
	else if (pht_info)
		recv_beacon->bcn_channel = pht_info->primary_channel;
	else {
		recv_beacon->bcn_channel = Nic->mlmeextpriv.cur_channel;
	}

	if (elems.ssid) {
		if (elems.ssid_len > sizeof(recv_beacon->ssid))
			return FALSE;

		Func_Of_Proc_Pre_Memcpy(recv_beacon->ssid, elems.ssid, elems.ssid_len);
		recv_beacon->ssid_len = elems.ssid_len;
	} else;

	if (elems.rsn_ie && elems.rsn_ie_len) {
		recv_beacon->encryp_protocol = ENCRYP_PROTOCOL_WPA2;
		proc_parse_wpa2_ie_func(elems.rsn_ie - 2, elems.rsn_ie_len + 2,
						  &recv_beacon->group_cipher,
						  &recv_beacon->pairwise_cipher, &recv_beacon->is_8021x,
						  1);
	} else if (elems.wpa_ie && elems.wpa_ie_len) {
		recv_beacon->encryp_protocol = ENCRYP_PROTOCOL_WPA;
		proc_parse_wpa_ie_func(elems.wpa_ie - 2, elems.wpa_ie_len + 2,
						 &recv_beacon->group_cipher,
						 &recv_beacon->pairwise_cipher, &recv_beacon->is_8021x,
						 1);
	} else if (capability & BIT(4)) {
		recv_beacon->encryp_protocol = ENCRYP_PROTOCOL_WEP;
	}

	return TRUE;
}

static void bcn_keys_prt(struct beacon_keys *recv_beacon)
{
	int i;
	char *p;
	u8 ssid[IW_ESSID_MAX_SIZE + 1];

	Func_Of_Proc_Pre_Memcpy(ssid, recv_beacon->ssid, recv_beacon->ssid_len);
	ssid[recv_beacon->ssid_len] = '\0';

	MpTrace(COMP_EVENTS, DBG_NORMAL,("%s: ssid = %s\n"), __FUNCTION__, ssid);
	MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s: channel = %x\n"), __FUNCTION__, recv_beacon->bcn_channel);
	MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s: ht_cap = %x\n"), __FUNCTION__, recv_beacon->ht_cap_info);
	MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s: ht_info_infos_0_sco = %x\n"), __FUNCTION__,
		recv_beacon->ht_info_infos_0_sco);
	MpTrace(COMP_EVENTS, DBG_NORMAL,("%s: sec=%d, group = %x, pair = %x, 8021X = %x\n"), __FUNCTION__,
			recv_beacon->encryp_protocol, recv_beacon->group_cipher,
			recv_beacon->pairwise_cipher, recv_beacon->is_8021x);
}

int do_chk_bcn_info(PNIC Nic, u8 * pframe, u32 packet_len, u8 tag)
{
	unsigned int len;
	struct beacon_keys recv_beacon;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct wlan_network *cur_network = &(Nic->mlmepriv.cur_network);
	u8 *pbssid = GetAddr3Ptr(pframe);
	if (tag) {
		if (chk_sta_associated_to_ap(Nic, 1) == FALSE)
			return TRUE;

		len = packet_len - sizeof(struct wl_ieee80211_hdr_3addr);

		if (len > MAX_IE_SZ) {
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s IE too long for survey event\n"), __FUNCTION__);
			return FALSE;
		}

		if (Func_Of_Proc_Pre_Memcmp(cur_network->network.MacAddress, pbssid, 6) == FALSE) {
			MpTrace(COMP_EVENTS, DBG_NORMAL,
				("Oops: wl_check_network_encrypt linked but recv other bssid bcn\n"),
				 MAC_FMT MAC_FMT, MAC_ARG(pbssid),
				 MAC_ARG(cur_network->network.MacAddress));
			return TRUE;
		}

		if (do_query_bcn_keys(Nic, &recv_beacon, pframe, packet_len) ==
			FALSE)
			return TRUE;

		if (recv_beacon.ssid_len == 0) {
			Func_Of_Proc_Pre_Memcpy(recv_beacon.ssid, pmlmepriv->cur_beacon_keys.ssid,
						pmlmepriv->cur_beacon_keys.ssid_len);
			recv_beacon.ssid_len = pmlmepriv->cur_beacon_keys.ssid_len;
		}

		if (Func_Of_Proc_Pre_Memcmp
			(&recv_beacon, &pmlmepriv->cur_beacon_keys,
			 sizeof(recv_beacon)) == TRUE) {
			pmlmepriv->new_beacon_cnts = 0;
		} else if ((pmlmepriv->new_beacon_cnts == 0) ||
				   Func_Of_Proc_Pre_Memcmp(&recv_beacon, &pmlmepriv->new_beacon_keys,
							   sizeof(recv_beacon)) == FALSE) {
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s: start new beacon (seq=%d)\n"), __FUNCTION__,GetSequence(pframe));

			if (pmlmepriv->new_beacon_cnts == 0) {
				MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s: cur beacon key\n"), __FUNCTION__);
			}
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s: new beacon key\n"), __FUNCTION__);
			memcpy(&pmlmepriv->new_beacon_keys, &recv_beacon,
				   sizeof(recv_beacon));
			pmlmepriv->new_beacon_cnts = 1;
		} else {
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s: new beacon again (seq=%d)\n"), __FUNCTION__, GetSequence(pframe));
			pmlmepriv->new_beacon_cnts++;
		}

		if (pmlmepriv->new_beacon_cnts >= 3) {
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s: new beacon occur!!\n"), __FUNCTION__);
			pmlmepriv->cur_beacon_keys.ht_cap_info = recv_beacon.ht_cap_info;
			pmlmepriv->cur_beacon_keys.ht_info_infos_0_sco =
				recv_beacon.ht_info_infos_0_sco;

			if (Func_Of_Proc_Pre_Memcmp(&recv_beacon, &pmlmepriv->cur_beacon_keys,
							sizeof(recv_beacon)) == FALSE) {
				return FALSE;
			}

			MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s bw mode change\n", __FUNCTION__));
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s bcn now: ht_cap_info:%x ht_info_infos_0:%x\n"), __FUNCTION__,
				cur_network->BcnInfo.ht_cap_info,
				cur_network->BcnInfo.ht_info_infos_0);
			cur_network->BcnInfo.ht_cap_info = recv_beacon.ht_cap_info;
			cur_network->BcnInfo.ht_info_infos_0 =
				(cur_network->BcnInfo.ht_info_infos_0 & (~0x03)) |
				recv_beacon.ht_info_infos_0_sco;

			MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s bcn link: ht_cap_info:%x ht_info_infos_0:%x\n"),
					__FUNCTION__, cur_network->BcnInfo.ht_cap_info,
					cur_network->BcnInfo.ht_info_infos_0);

			memcpy(&pmlmepriv->cur_beacon_keys, &recv_beacon,
				   sizeof(recv_beacon));
			pmlmepriv->new_beacon_cnts = 0;
		}
	}
	return TRUE;

}

void do_renew_bcn_info(PNIC Nic, u8 * pframe, uint pkt_len,
						struct sta_info *psta)
{
	unsigned int i;
	unsigned int len;
	PNDIS_802_11_VARIABLE_IEs pIE;

	len = pkt_len - (_BEACON_IE_OFFSET_ + WLAN_HDR_A3_LEN);

	for (i = 0; i < len;) {
		pIE =
			(PNDIS_802_11_VARIABLE_IEs) (pframe +
										 (_BEACON_IE_OFFSET_ +
										  WLAN_HDR_A3_LEN) + i);

		switch (pIE->ElementID) {
		case _VENDOR_SPECIFIC_IE_:
			if (Func_Of_Proc_Pre_Memcmp(pIE->data, WMM_PARA_OUI, 6)
				&& pIE->Length == WLAN_WMM_LEN) {
#if 0
				(WMM_param_handler(Nic, pIE)) ?
					//in wl_ext.c
					wmm_edca_update_to_report_func(Nic, 1) : 0;
#endif

			}

			break;

		case _HT_EXTRA_INFO_IE_:
			chk_bwmode_renew(pIE, Nic);
			break;
		case _ERPINFO_IE_:
			ERP_IE_handler(Nic, pIE, 1);
			do_renew_VCS(Nic, psta);
			break;
		default:
			break;
		}

		i += (pIE->Length + 2);
	}
}

unsigned int chk_ap_in_tkip(PNIC Nic, u8 tag)
{
	u32 i;
	PNDIS_802_11_VARIABLE_IEs pIE;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
	if (tag) {
		if (do_query_capability((WLAN_BSSID_EX *) cur_network) &
			WLAN_CAPABILITY_PRIVACY) {
			for (i = sizeof(NDIS_802_11_FIXED_IEs);
				 i < pmlmeinfo->network.IELength;) {
				pIE = (PNDIS_802_11_VARIABLE_IEs) (pmlmeinfo->network.IEs + i);

				switch (pIE->ElementID) {
				case _VENDOR_SPECIFIC_IE_:
					if ((Func_Of_Proc_Pre_Memcmp(pIE->data, WL_WPA_OUI, 4))
						&& (Func_Of_Proc_Pre_Memcmp((pIE->data + 12), WPA_TKIP_CIPHER, 4)))
					{
						return TRUE;
					}
					break;

				case _RSN_IE_2_:
					if (Func_Of_Proc_Pre_Memcmp((pIE->data + 8), RSN_TKIP_CIPHER, 4)) {
						return TRUE;
					}

				default:
					break;
				}

				i += (pIE->Length + 2);
			}

			return FALSE;
		} else {
			return FALSE;
		}
	} else {
		return FALSE;
	}

}

int wifirate2_ratemap_inx(unsigned char rate)
{
	int inx = 0;
	rate = rate & 0x7f;

	switch (rate) {
	case 54 * 2:
		inx = 11;
		break;

	case 48 * 2:
		inx = 10;
		break;

	case 36 * 2:
		inx = 9;
		break;

	case 24 * 2:
		inx = 8;
		break;

	case 18 * 2:
		inx = 7;
		break;

	case 12 * 2:
		inx = 6;
		break;

	case 9 * 2:
		inx = 5;
		break;

	case 6 * 2:
		inx = 4;
		break;

	case 11 * 2:
		inx = 3;
		break;
	case 11:
		inx = 2;
		break;

	case 2 * 2:
		inx = 1;
		break;

	case 1 * 2:
		inx = 0;
		break;

	}
	return inx;
}

int chk_short_GI(PNIC Nic, u8 bwmode,
					 struct HT_caps_element *pHT_caps)
{
	unsigned char bit_offset;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	if (!(pmlmeinfo->HT_enable))
		return FALSE;

	bit_offset = (bwmode & CHANNEL_WIDTH_40) ? 6 : 5;

	if (pHT_caps->u.HT_cap_element.HT_caps_info & (0x1 << bit_offset)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void do_renew_tx_Brate(PNIC Nic, u8 wirelessmode, u8 tag)
{
	NDIS_802_11_RATES_EX supported_rates;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &Nic->wdinfo;

	if (!wl_p2p_chk_state(pwdinfo, P2P_STATE_NONE))
		return;
#endif
	if (tag) {
		memset(supported_rates, 0, NDIS_802_11_LENGTH_RATES_EX);

		if ((wirelessmode & WIRELESS_11B) && (wirelessmode == WIRELESS_11B)) {
			Func_Of_Proc_Pre_Memcpy(supported_rates, wl_basic_rate_cck, 4);
		} else if (wirelessmode & WIRELESS_11B) {
			Func_Of_Proc_Pre_Memcpy(supported_rates, wl_basic_rate_mix, 7);
		} else {
			Func_Of_Proc_Pre_Memcpy(supported_rates, wl_basic_rate_ofdm, 3);
		}

		if (wirelessmode & WIRELESS_11B)
			proc_mgnt_tx_rate_update_func(Nic, IEEE80211_CCK_RATE_1MB);
		else
			proc_mgnt_tx_rate_update_func(Nic, IEEE80211_OFDM_RATE_6MB);

		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_BASIC_RATE, supported_rates);
	}
}

unsigned char assoc_AP_chk(u8 * pframe, uint len, u8 tag)
{
	unsigned int i;
	PNDIS_802_11_VARIABLE_IEs pIE;
	if (tag) {
		for (i = sizeof(NDIS_802_11_FIXED_IEs); i < len;) {
			pIE = (PNDIS_802_11_VARIABLE_IEs) (pframe + i);

			switch (pIE->ElementID) {
			case _VENDOR_SPECIFIC_IE_:
				if ((Func_Of_Proc_Pre_Memcmp(pIE->data, ARTHEROS_OUI1, 3))
					|| (Func_Of_Proc_Pre_Memcmp(pIE->data, ARTHEROS_OUI2, 3))) {
					MpTrace(COMP_EVENTS, DBG_NORMAL, ("link to Artheros AP\n"));
					return HT_IOT_PEER_ATHEROS;
				} else if ((Func_Of_Proc_Pre_Memcmp(pIE->data, BROADCOM_OUI1, 3))
						   || (Func_Of_Proc_Pre_Memcmp(pIE->data, BROADCOM_OUI2, 3))
						   || (Func_Of_Proc_Pre_Memcmp(pIE->data, BROADCOM_OUI3, 3))) {
					MpTrace(COMP_EVENTS, DBG_NORMAL, ("link to Broadcom AP\n"));
					return HT_IOT_PEER_BROADCOM;
				} else if (Func_Of_Proc_Pre_Memcmp(pIE->data, MARVELL_OUI, 3)) {
					MpTrace(COMP_EVENTS, DBG_NORMAL, ("link to Marvell AP\n"));
					return HT_IOT_PEER_MARVELL;
				} else if (Func_Of_Proc_Pre_Memcmp(pIE->data, RALINK_OUI, 3)) {
					MpTrace(COMP_EVENTS, DBG_NORMAL, ("link to Ralink AP\n"));
					return HT_IOT_PEER_RALINK;
				} else if (Func_Of_Proc_Pre_Memcmp(pIE->data, CISCO_OUI, 3)) {
					MpTrace(COMP_EVENTS, DBG_NORMAL, ("link to Cisco AP\n"));
					return HT_IOT_PEER_CISCO;
				} else if (Func_Of_Proc_Pre_Memcmp(pIE->data, WK_WLAN_OUI, 3)) {
					u32 Vender = HT_IOT_PEER_WK_WLAN;

					MpTrace(COMP_EVENTS, DBG_NORMAL, ("link to Wf_wlan AP\n"));
					return Vender;
				}

				else {
					break;
				}

			default:
				break;
			}

			i += (pIE->Length + 2);
		}
	}
	MpTrace(COMP_EVENTS, DBG_NORMAL, ("link to new AP\n"));
	return HT_IOT_PEER_UNKNOWN;
}

void do_renew_capinfo(PNIC Nic, u16 updateCap, u8 tag)
{
	BOOLEAN ShortPreamble;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	if (tag) {
		{

			if (updateCap & cShortPreamble) {
				if (pmlmeinfo->preamble_mode != PREAMBLE_SHORT) {
					ShortPreamble = TRUE;
					pmlmeinfo->preamble_mode = PREAMBLE_SHORT;
					Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_ACK_PREAMBLE,
									  (u8 *) & ShortPreamble);
				}
			} else {
				if (pmlmeinfo->preamble_mode != PREAMBLE_LONG) {
					ShortPreamble = FALSE;
					pmlmeinfo->preamble_mode = PREAMBLE_LONG;
					Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_ACK_PREAMBLE,
									  (u8 *) & ShortPreamble);
				}
			}
		}

		if (updateCap & cIBSS) {
			pmlmeinfo->slotTime = NON_SHORT_SLOT_TIME;
		} else {
			if (pmlmeext->cur_wireless_mode & (WIRELESS_11_24N)) {
				pmlmeinfo->slotTime = SHORT_SLOT_TIME;
			} else if (pmlmeext->cur_wireless_mode & (WIRELESS_11G)) {
				if ((updateCap & cShortSlotTime)) {
					pmlmeinfo->slotTime = SHORT_SLOT_TIME;
				} else {
					pmlmeinfo->slotTime = NON_SHORT_SLOT_TIME;
				}
			} else {
				pmlmeinfo->slotTime = NON_SHORT_SLOT_TIME;
			}
		}

		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_SLOT_TIME, &pmlmeinfo->slotTime);
	}
}

void do_renew_wireless_mode(PNIC Nic, u8 tag)
{
	u32 SIFS_Timer;
	int ratelen, network_type = 0;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
	unsigned char *rate = cur_network->SupportedRates;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &(Nic->wdinfo);
#endif
	if (tag) {
		ratelen = rateset_len_to_get_func(cur_network->SupportedRates, 1);

		if ((pmlmeinfo->HT_info_enable) && (pmlmeinfo->HT_caps_enable)) {
			pmlmeinfo->HT_enable = 1;
		}

		if (pmlmeext->cur_channel > 14) {
			network_type = WIRELESS_INVALID;
		} else {
			if (pmlmeinfo->HT_enable)
				network_type = WIRELESS_11_24N;

			if ((only_cckrate(rate, ratelen)) == TRUE) {
				network_type |= WIRELESS_11B;
			} else if ((have_cckrates(rate, ratelen)) == TRUE) {
				network_type |= WIRELESS_11BG;
			} else {
				network_type |= WIRELESS_11G;
			}
		}

		pmlmeext->cur_wireless_mode = network_type & WIRELESS_11BG_24N;

		SIFS_Timer = 0x0a0a0808;

		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_RESP_SIFS, (u8 *) & SIFS_Timer);

		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_WIRELESS_MODE,
						  (u8 *) & (pmlmeext->cur_wireless_mode));

		if ((pmlmeext->cur_wireless_mode & WIRELESS_11B)
#ifdef CONFIG_P2P
			&& wl_p2p_chk_state(pwdinfo, P2P_STATE_NONE)
#endif
			)
			proc_mgnt_tx_rate_update_func(Nic, IEEE80211_CCK_RATE_1MB);
		else
			proc_mgnt_tx_rate_update_func(Nic, IEEE80211_OFDM_RATE_6MB);
	}
}

int do_ies_query_supported_rate(u8 * rate_set, u8 * rate_num, u8 * ies,
							   uint ies_len)
{
	u8 *ie;
	unsigned int ie_len;

	if (!rate_set || !rate_num)
		return FALSE;

	*rate_num = 0;

	ie = ie_to_get_func(ies, _SUPPORTEDRATES_IE_, &ie_len, ies_len);
	if (ie == NULL)
		goto ext_rate;

	Func_Of_Proc_Pre_Memcpy(rate_set, ie + 2, ie_len);
	*rate_num = ie_len;

ext_rate:
	ie = ie_to_get_func(ies, _EXT_SUPPORTEDRATES_IE_, &ie_len, ies_len);
	if (ie) {
		Func_Of_Proc_Pre_Memcpy(rate_set + *rate_num, ie + 2, ie_len);
		*rate_num += ie_len;
	}

	if (*rate_num == 0)
		return FALSE;

	if (0) {
		int i;

		for (i = 0; i < *rate_num; i++)
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("rate:0x%02x\n"), *(rate_set + i));
	}

	return TRUE;
}

void work_addba_req(PNIC Nic, u8 * paddba_req, u8 * addr, u8 tag)
{
	u16 tid, start_seq, param;
	u8 size, accept = FALSE;
	struct sta_info *psta;
	struct sta_priv *pstapriv = &Nic->stapriv;
	struct ADDBA_request *preq = (struct ADDBA_request *)paddba_req;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

#if 0
	psta = do_query_stainfo(pstapriv, addr, 1);
#else
	psta = pstapriv;
#endif
	if (!psta)
		goto exit;

	
	if (tag) {
		start_seq = le16_to_cpu(preq->BA_starting_seqctrl) >> 4;

		param = le16_to_cpu(preq->BA_para_set);
		tid = (param >> 2) & 0x0f;

		accept = check_rx_ampdu_is_accept_func(Nic, 1);
		size = proc_rx_ampdu_size_func(Nic, 1);
#if 0
		if (accept == TRUE)
			proc_addbarsp_cmd_func(Nic, addr, tid, 0, size, start_seq, 1);
		else
			proc_addbarsp_cmd_func(Nic, addr, tid, 37, size, start_seq, 1);
#endif
	}
exit:
	return;
}

void do_renew_TSF(struct mlme_ext_priv *pmlmeext, u8 * pframe, uint len, u8 tag)
{
	u8 *pIE;
	u32 *pbuf;
	if (tag) {
		pIE = pframe + sizeof(struct wl_ieee80211_hdr_3addr);
		pbuf = (u32 *) pIE;

		pmlmeext->TSFValue = le32_to_cpu(*(pbuf + 1));

		pmlmeext->TSFValue = pmlmeext->TSFValue << 32;

		pmlmeext->TSFValue |= le32_to_cpu(*pbuf);
	}
}

void devp_early(struct mlme_ext_priv *pmlmeext, u8 * pframe, uint len,
						u8 tag)
{
	int i;
	u8 *pIE;
	u32 *pbuf;
	u64 tsf = 0;
	u32 delay_ms;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	if (tag) {
		pmlmeext->bcn_cnt++;

		pIE = pframe + sizeof(struct wl_ieee80211_hdr_3addr);
		pbuf = (u32 *) pIE;

		tsf = le32_to_cpu(*(pbuf + 1));
		tsf = tsf << 32;
		tsf |= le32_to_cpu(*pbuf);

		delay_ms = Func_Of_Proc_Modular64(tsf, (pmlmeinfo->bcn_interval * 1024));
		delay_ms = delay_ms / 1000;

		if (delay_ms >= 8) {
			pmlmeext->bcn_delay_cnt[8]++;
		} else {
			pmlmeext->bcn_delay_cnt[delay_ms]++;
		}

		if (pmlmeext->bcn_cnt > 100 && (pmlmeext->adaptive_tsf_done == TRUE)) {
			u8 ratio_20_delay, ratio_80_delay;
			u8 DrvBcnEarly, DrvBcnTimeOut;

			ratio_20_delay = 0;
			ratio_80_delay = 0;
			DrvBcnEarly = 0xff;
			DrvBcnTimeOut = 0xff;

			MpTrace(COMP_EVENTS, DBG_NORMAL, ("%s(): bcn_cnt = %d\n"), __FUNCTION__, pmlmeext->bcn_cnt);
			for (i = 0; i < 9; i++) {
				pmlmeext->bcn_delay_ratio[i] =
					(pmlmeext->bcn_delay_cnt[i] * 100) / pmlmeext->bcn_cnt;

				ratio_20_delay += pmlmeext->bcn_delay_ratio[i];
				ratio_80_delay += pmlmeext->bcn_delay_ratio[i];

				if (ratio_20_delay > 20 && DrvBcnEarly == 0xff) {
					DrvBcnEarly = i;
				}

				if (ratio_80_delay > 80 && DrvBcnTimeOut == 0xff) {
					DrvBcnTimeOut = i;
				}

				pmlmeext->bcn_delay_cnt[i] = 0;
				pmlmeext->bcn_delay_ratio[i] = 0;
			}

			pmlmeext->DrvBcnEarly = DrvBcnEarly;
			pmlmeext->DrvBcnTimeOut = DrvBcnTimeOut;

			pmlmeext->bcn_cnt = 0;
		}
	}

}

void cntl_bcn_timing(PNIC Nic, u8 tag)
{

	int ret = FALSE;
	u32 bcn_ctrl_reg;
	u32 par[6] = { 0 };
	u32 res[5] = { 0 };
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;

	bcn_ctrl_reg = REG_BCN_CTRL;
#ifdef CONFIG_CONCURRENT_MODE
	if (Nic->iface_type == IFACE_PORT1)
		bcn_ctrl_reg = REG_BCN_CTRL_1;
#endif
	if (tag) {
		par[0] = Nic->mlmepriv.fw_state;
		par[1] = pmlmeinfo->bcn_interval;
		par[2] =
			check_fwstate(&Nic->mlmepriv,
						  WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE |
						  WIFI_AP_STATE);
		par[3] = pHalData->RegFwHwTxQCtrl;
		par[4] = pHalData->RegReg542;
		par[5] = bcn_ctrl_reg;

		ret =
			Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_SET_BCN_REG, par,
										res, 6, 5);
		if (!ret) {
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("===>%s,UMSG_OPS_HAL_SET_BCN_REG fail!\n"), __FUNCTION__);
			return;
		}
		pHalData->RegBcnCtrlVal = (u8) res[0];
		pHalData->RegTxPause = (u8) res[1];
		pHalData->RegFwHwTxQCtrl = (u8) res[2];
		pHalData->RegReg542 = (u8) res[3];
		pHalData->RegCR_1 = (u8) res[4];
	}
}

#define CONFIG_SHARED_BMC_MACID

static inline bool wl_wmid_is_set(struct macid_bmp *map, u8 id)
{
	if (id < 32)
		return (map->m0 & BIT(id));
	else
		wl_warn_on(1);

	return 0;
}

static inline void wl_wmid_map_set(struct macid_bmp *map, u8 id)
{
	if (id < 32)
		map->m0 |= BIT(id);
	else
		wl_warn_on(1);
}

static inline void wl_wmid_map_clr(struct macid_bmp *map, u8 id)
{
	if (id < 32)
		map->m0 &= ~BIT(id);
	else
		wl_warn_on(1);
}

inline bool wl_wmid_is_used(struct macid_ctl_t *macid_ctl, u8 id)
{
	return wl_wmid_is_set(&macid_ctl->used, id);
}

inline bool wl_wmid_is_bmc(struct macid_ctl_t * macid_ctl, u8 id)
{
	return wl_wmid_is_set(&macid_ctl->bmc, id);
}

inline s8 wl_wmid_query_if_g(struct macid_ctl_t * macid_ctl, u8 id)
{
	int i;

#ifdef CONFIG_SHARED_BMC_MACID
	if (wl_wmid_is_bmc(macid_ctl, id))
		return -1;
#endif

	for (i = 0; i < IFACE_ID_MAX; i++) {
		if (wl_wmid_is_set(&macid_ctl->if_g[i], id))
			return i;
	}
	return -1;
}

void wl_new_macid(PNIC Nic, struct sta_info *psta, u8 tag)
{
	int i;
	//_irqL irqL;
	u8 last_id = 0;
	u8 bc_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct macid_ctl_t *macid_ctl = &dvobj->macid_ctl;
	struct macid_bmp *used_map = &macid_ctl->used;

	if (Func_Of_Proc_Pre_Memcmp(psta->hwaddr, wadptdata_mac_addr(Nic), ETH_ALEN)) {
		psta->mac_id = macid_ctl->num;
		return;
	}
	if (tag) {
#ifdef CONFIG_SHARED_BMC_MACID
		if (Func_Of_Proc_Pre_Memcmp(psta->hwaddr, bc_addr, ETH_ALEN)) {
//			spin_lock_bh(&macid_ctl->lock);
			wl_wmid_map_set(used_map, 1);
			wl_wmid_map_set(&macid_ctl->bmc, 1);
			for (i = 0; i < IFACE_ID_MAX; i++)
				wl_wmid_map_set(&macid_ctl->if_g[Nic->iface_id], 1);
			macid_ctl->sta[1] = psta;
//			spin_unlock_bh(&macid_ctl->lock);
			i = 1;
			goto assigned;
		}
#endif

//		spin_lock_bh(&macid_ctl->lock);

		for (i = last_id; i < macid_ctl->num; i++) {
#ifdef CONFIG_SHARED_BMC_MACID
			if (i == 1)
				continue;
#endif
			if (!wl_wmid_is_used(macid_ctl, i))
				break;
		}

		if (i < macid_ctl->num) {

			wl_wmid_map_set(used_map, i);

			if (Func_Of_Proc_Pre_Memcmp(psta->hwaddr, bc_addr, ETH_ALEN))
				wl_wmid_map_set(&macid_ctl->bmc, i);

			wl_wmid_map_set(&macid_ctl->if_g[Nic->iface_id], i);
			macid_ctl->sta[i] = psta;

			last_id++;
			last_id %= macid_ctl->num;
		}

//		spin_unlock_bh(&macid_ctl->lock);

		if (i >= macid_ctl->num) {
			psta->mac_id = macid_ctl->num;
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("if%u, hwaddr: "), MAC_FMT " no available macid\n",
				Nic->iface_id + 1, MAC_ARG(psta->hwaddr));
			wl_warn_on(1);
			goto exit;
		} else {
			goto assigned;
		}

assigned:
		psta->mac_id = i;
		MpTrace(COMP_EVENTS, DBG_NORMAL, ("if%u, hwaddr: "), MAC_FMT " macid:%u\n",
			Nic->iface_id + 1, MAC_ARG(psta->hwaddr), psta->mac_id);
	}
exit:
	return;
}

void do_release_wmid(PNIC Nic, struct sta_info *psta, u8 tag)
{
	//_irqL irqL;
	u8 bc_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	struct dvobj_priv *dvobj = &Nic->dvobjpriv;
	struct macid_ctl_t *macid_ctl = &dvobj->macid_ctl;

	if (Func_Of_Proc_Pre_Memcmp(psta->hwaddr, wadptdata_mac_addr(Nic), ETH_ALEN))
		return;
	if (tag) {
#ifdef CONFIG_SHARED_BMC_MACID
		if (Func_Of_Proc_Pre_Memcmp(psta->hwaddr, bc_addr, ETH_ALEN))
			return;

		if (psta->mac_id == 1) {
			MpTrace(COMP_EVENTS, DBG_NORMAL, ("if%u, hwaddr: "), MAC_FMT "with macid:%u\n",
				Nic->iface_id + 1, MAC_ARG(psta->hwaddr), psta->mac_id);
			wl_warn_on(1);
			return;
		}
#endif

//		spin_lock_bh(&macid_ctl->lock);

		if (psta->mac_id < macid_ctl->num) {
			int i;

			if (!wl_wmid_is_used(macid_ctl, psta->mac_id)) {
				MpTrace(COMP_EVENTS, DBG_NORMAL, ("if%u, hwaddr: "), MAC_FMT "macid:%u not used\n",
					Nic->iface_id + 1, MAC_ARG(psta->hwaddr), psta->mac_id);
				wl_warn_on(1);
			}

			wl_wmid_map_clr(&macid_ctl->used, psta->mac_id);
			wl_wmid_map_clr(&macid_ctl->bmc, psta->mac_id);
			for (i = 0; i < IFACE_ID_MAX; i++)
				wl_wmid_map_clr(&macid_ctl->if_g[i], psta->mac_id);
			for (i = 0; i < 2; i++)
				wl_wmid_map_clr(&macid_ctl->ch_g[i], psta->mac_id);
			macid_ctl->sta[psta->mac_id] = NULL;
		}

//		spin_unlock_bh(&macid_ctl->lock);

		psta->mac_id = macid_ctl->num;
	}
}

void wl_wmid_ctl_set_msr(struct macid_ctl_t *macid_ctl, u8 id,
									  u8 wmbox1_msr, u8 tag)

//inline void wl_wmid_ctl_set_msr(struct macid_ctl_t *macid_ctl, u8 id,
//									  u8 wmbox1_msr, u8 tag)
{
	if (tag) {
		if (id >= macid_ctl->num) {
			wl_warn_on(1);
			return;
		}

		macid_ctl->wmbox1_msr[id] = wmbox1_msr;
	}
}

NIC *query_port0_dever(struct dvobj_priv * dvobj, u8 tag)
{
	NIC *port0_iface = NULL;
	int i;
	if (tag) {
		for (i = 0; i < dvobj->iface_nums; i++) {
			if (dvobj->Nic[i] == IFACE_PORT0)				
				break;
		}

		if (i < 0 || i >= dvobj->iface_nums)
			wl_warn_on(1);
		else
			port0_iface = dvobj->Nic[i];
	}
	return port0_iface;
}

#ifdef CONFIG_WOWLAN
bool do_chk_pattern_valid(u8 * input, u8 len, u8 tag)
{
	int i = 0;
	bool res = FALSE;
	if (tag) {
		if (len != 2)
			goto exit;

		for (i = 0; i < len; i++)
			if (Func_Ishexdigit(input[i]) == FALSE)
				goto exit;

		res = TRUE;
	}
exit:
	return res;
}

bool wl_read_from_pkt_list(PNIC Nic, u8 idx, u8 tag)
{
	u32 m_res;
	int i = 0;
	bool res = TRUE;
	u32 mbox[2];
	int ret = FALSE;
	u8 count = 0, tmp = 0;
	u16 offset, rx_buf_ptr = 0;
	u16 cam_start_offset = 0;
	u16 ctrl_l = 0, ctrl_h = 0;
	u32 data_l = 0, data_h = 0, rx_dma_buff_sz = 0, page_sz = 0;
	if (tag) {
		if (idx > MAX_WKFM_NUM) {
			WL_INFO("[Error]: %s, pattern index is out of range\n", __FUNCTION__);
			return FALSE;
		}

		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_RX_DMA_SZ_WOW,
							(u8 *) & rx_dma_buff_sz);

		if (rx_dma_buff_sz == 0) {
			WL_INFO("[Error]: %s, rx_dma_buff_sz is 0!!\n", __FUNCTION__);
			return FALSE;
		}

		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_RX_PAGE_SIZE, (u8 *) & page_sz);

		if (page_sz == 0) {
			WL_INFO("[Error]: %s, page_sz is 0!!\n", __FUNCTION__);
			return FALSE;
		}

		offset = (u16) PageNum(rx_dma_buff_sz, page_sz);
		cam_start_offset = offset * page_sz;

		mbox[0] = cam_start_offset;
		mbox[1] = idx;
		ret =
			Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_READ_WKFM_CAM, mbox,
										&m_res, 2, 1);
		if (!ret) {
			m_res = FALSE;
			WL_INFO("===>%s,UMSG_OPS_HAL_READ_WKFM_CAM fail!\n", __FUNCTION__);
			return m_res;
		}

		res = (u8) m_res;
	}
	return res;
}

bool wl_write_to_pkt_list(PNIC Nic, u8 idx,
							 struct wltwow_pattern * context, u8 tag)
{
	u32 m_res;
	u32 mbox[8] = { 0 };
	int ret = FALSE;
	int res = 0, i = 0;
	u8 count = 0, tmp = 0;
	u16 cam_start_offset = 0;
	u16 ctrl_l = 0, ctrl_h = 0;
	u16 offset, rx_buf_ptr = 0;
	u32 data = 0, rx_dma_buff_sz = 0, page_sz = 0;

	if (idx > MAX_WKFM_NUM) {
		WL_INFO("[Error]: %s, pattern index is out of range\n", __FUNCTION__);
		return FALSE;
	}
	if (tag) {
		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_RX_DMA_SZ_WOW,
							(u8 *) & rx_dma_buff_sz);

		if (rx_dma_buff_sz == 0) {
			WL_INFO("[Error]: %s, rx_dma_buff_sz is 0!!\n", __FUNCTION__);
			return FALSE;
		}

		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_RX_PAGE_SIZE, (u8 *) & page_sz);

		if (page_sz == 0) {
			WL_INFO("[Error]: %s, page_sz is 0!!\n", __FUNCTION__);
			return FALSE;
		}

		offset = (u16) PageNum(rx_dma_buff_sz, page_sz);

		cam_start_offset = offset * page_sz;

		mbox[0] = cam_start_offset;
		mbox[1] = idx;
		mbox[2] = context->type;
		mbox[3] = context->crc;
		for (i = 0; i < 4; i++)
			mbox[i + 4] = context->mask[i];
		ret =
			Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_WRITE_WKFM_CAM,
										mbox, &m_res, 8, 1);
		if (!ret) {
			m_res = FALSE;
			WL_INFO("===>%s,UMSG_OPS_HAL_READ_WKFM_CAM fail!\n", __FUNCTION__);
			return m_res;
		}
		res = m_res;
	}
	return res;
}

void priv_pattern_prt(PNIC Nic, u8 idx, u8 tag)
{
	u8 val8 = 0;
	char str_1[128];
	char *p_str;
	int i = 0, j = 0, len = 0, max_len = 0;
	struct pwrctrl_priv *pwrctl = wadptdata_to_pwrctl(Nic);

	WL_INFO("=========[%d]========\n", idx);
	if (tag) {
		WL_INFO(">>>priv_pattern_content:\n");
		p_str = str_1;
		max_len = sizeof(str_1);
		for (i = 0; i < MAX_WKFM_PATTERN_SIZE / 8; i++) {
			memset(p_str, 0, max_len);
			len = 0;
			for (j = 0; j < 8; j++) {
				val8 = pwrctl->patterns[idx].content[i * 8 + j];
				len += snprintf(p_str + len, max_len - len, "%02x ", val8);
			}
			WL_INFO("%s\n", p_str);
		}

		WL_INFO(">>>priv_pattern_mask:\n");
		for (i = 0; i < MAX_WKFM_SIZE / 8; i++) {
			memset(p_str, 0, max_len);
			len = 0;
			for (j = 0; j < 8; j++) {
				val8 = pwrctl->patterns[idx].mask[i * 8 + j];
				len += snprintf(p_str + len, max_len - len, "%02x ", val8);
			}
			WL_INFO("%s\n", p_str);
		}
	}
}

void clean_pattern_work(PNIC Nic, u8 tag)
{
	struct pwrctrl_priv *pwrctl = wadptdata_to_pwrctl(Nic);
	struct wltwow_pattern zero_pattern;
	int i = 0;
	if (tag) {
		memset(&zero_pattern, 0, sizeof(struct wltwow_pattern));

		zero_pattern.type = PATTERN_INVALID;

		for (i = 0; i < MAX_WKFM_NUM; i++)
			wl_write_to_pkt_list(Nic, i, &zero_pattern, 1);

		pwrctl->wowlan_pattern_idx = 0;
		wf_io_write8(Nic, REG_WKFMCAM_NUM, pwrctl->wowlan_pattern_idx);
	}
}

void do_query_sec_iv(Nic Nic, u8 * pcur_dot11txpn, u8 * StaAddr,
					u8 tag)
{
	struct sta_info *psta;
	struct security_priv *psecpriv = &Nic->securitypriv;
	if (tag) {
		memset(pcur_dot11txpn, 0, 8);
		if (NULL == StaAddr)
			return;
		psta = do_query_stainfo(&Nic->stapriv, StaAddr, 1);
		WL_INFO("%s(): StaAddr: %02x %02x %02x %02x %02x %02x\n",
				__FUNCTION__, StaAddr[0], StaAddr[1], StaAddr[2],
				StaAddr[3], StaAddr[4], StaAddr[5]);

		if (psta) {
			if (psecpriv->dot11PrivacyAlgrthm != _NO_PRIVACY_
				&& psta->dot11txpn.val > 0)
				psta->dot11txpn.val--;
			AES_IV(pcur_dot11txpn, psta->dot11txpn, 0);

			WL_INFO
				("%s(): CurrentIV: %02x %02x %02x %02x %02x %02x %02x %02x \n",
				 __FUNCTION__, pcur_dot11txpn[0], pcur_dot11txpn[1],
				 pcur_dot11txpn[2], pcur_dot11txpn[3], pcur_dot11txpn[4],
				 pcur_dot11txpn[5], pcur_dot11txpn[6], pcur_dot11txpn[7]);
		}
	}
}

void do_set_sec_pn(Nic Nic, u8 tag)
{
	struct sta_info *psta;
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct pwrctrl_priv *pwrpriv = wadptdata_to_pwrctl(Nic);
	struct security_priv *psecpriv = &Nic->securitypriv;
	if (tag) {
		psta = do_query_stainfo(&Nic->stapriv,
							   query_bssid(&pmlmeinfo->network), 1);

		if (psta) {
			if (pwrpriv->wowlan_fw_iv > psta->dot11txpn.val) {
				if (psecpriv->dot11PrivacyAlgrthm != _NO_PRIVACY_)
					psta->dot11txpn.val = pwrpriv->wowlan_fw_iv + 2;
			} else {
				WL_INFO("%s(): FW IV is smaller than driver\n", __FUNCTION__);
				psta->dot11txpn.val += 2;
			}
			WL_INFO("%s: dot11txpn: 0x%016llx\n", __FUNCTION__,
					psta->dot11txpn.val);
		}
	}
}
#endif

