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

//#define _WL_CMD_C_

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
#include "wmgntd.h"

#if 1
struct iw_point {
	void  *pointer;
	__u16 length;
	__u16 flags;
};
union iwreq_data {
	char name[16];

	struct iw_point data;
};
#define IW_CUSTOM_MAX	256
/*
typedef enum _NDIS_802_11_AUTHENTICATION_MODE {
	Ndis802_11AuthModeOpen,
	Ndis802_11AuthModeShared,
	Ndis802_11AuthModeAutoSwitch,
	Ndis802_11AuthModeWPA,
	Ndis802_11AuthModeWPAPSK,
	Ndis802_11AuthModeWPANone,
	Ndis802_11AuthModeWAPI,
	Ndis802_11AuthModeMax
} NDIS_802_11_AUTHENTICATION_MODE, *PNDIS_802_11_AUTHENTICATION_MODE;
*/

#define REGSTY_BW_2G(regsty) ((regsty)->bw_mode & 0x0F)
#define REGSTY_IS_BW_2G_SUPPORT(regsty, bw) (REGSTY_BW_2G((regsty)) >= (bw))

unsigned int do_rebuild_ht_ie(PNIC  Nic, u8 * in_ie, u8 * out_ie,
								   uint in_len, uint * pout_len, u8 channel,
								   u8 tag)
{
	u32 ielen, out_len;
	u32 rx_packet_offset, max_recvbuf_sz;
	HT_CAP_AMPDU_DENSITY best_ampdu_density;
	HT_CAP_AMPDU_FACTOR max_rx_ampdu_factor;
	u8 cbw40_enable = 0, rf_type = 0, operation_bw = 0, rf_num =
		0, rx_stbc_nss = 0;
	unsigned char *p, *pframe;
	struct wl_ieee80211_ht_cap ht_capie;
	struct registry_priv *pregistrypriv = &Nic->registrypriv;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;

	phtpriv->ht_option = _FALSE;

	out_len = *pout_len;
	if (tag) {
		memset(&ht_capie, 0, sizeof(struct wl_ieee80211_ht_cap));

		ht_capie.cap_info = IEEE80211_HT_CAP_DSSSCCK40;

		if (phtpriv->sgi_20m)
			ht_capie.cap_info |= IEEE80211_HT_CAP_SGI_20;

		if (in_ie == NULL) {
			if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
				operation_bw = Nic->mlmeextpriv.cur_bwmode;
				if (operation_bw > CHANNEL_WIDTH_40)
					operation_bw = CHANNEL_WIDTH_40;
			} else
				operation_bw = CHANNEL_WIDTH_40;
		} else {
			p = ie_to_get_func(in_ie, _HT_ADD_INFO_IE_, &ielen, in_len);
			if (p && (ielen == sizeof(struct ieee80211_ht_addt_info))) {
				struct HT_info_element *pht_info =
					(struct HT_info_element *)(p + 2);
				if (pht_info->infos[0] & BIT(2)) {
					switch (pht_info->infos[0] & 0x3) {
					case 1:
					case 3:
						operation_bw = CHANNEL_WIDTH_40;
						break;
					default:
						operation_bw = CHANNEL_WIDTH_20;
						break;
					}
				} else {
					operation_bw = CHANNEL_WIDTH_20;
				}
			}
		}

		if (Func_Chip_Hw_Chk_Bw_Cap(Nic, BW_CAP_40M)) {
			if (channel <= 14) {
				if (REGSTY_IS_BW_2G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_40))
					cbw40_enable = 1;
			}
		}

		if ((cbw40_enable == 1) && (operation_bw == CHANNEL_WIDTH_40)) {
			ht_capie.cap_info |= IEEE80211_HT_CAP_SUP_WIDTH;
			if (phtpriv->sgi_40m)
				ht_capie.cap_info |= IEEE80211_HT_CAP_SGI_40;
		}

		ht_capie.cap_info |= IEEE80211_HT_CAP_SM_PS;

		if (TEST_FLAG(phtpriv->ldpc_cap, LDPC_HT_ENABLE_RX)) {
			ht_capie.cap_info |= IEEE80211_HT_CAP_LDPC_CODING;
			//WL_INFO("[HT] Declare supporting RX LDPC\n");
		}

		if (TEST_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_TX)) {
			ht_capie.cap_info |= IEEE80211_HT_CAP_TX_STBC;
			//WL_INFO("[HT] Declare supporting TX STBC\n");
		}

		if (TEST_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_RX)) {
			if ((pregistrypriv->rx_stbc == 0x3) ||
				((channel <= 14) && (pregistrypriv->rx_stbc == 0x1))
				) {
				Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_RX_STBC,
									(u8 *) (&rx_stbc_nss));
				SET_HT_CAP_ELE_RX_STBC(&ht_capie, rx_stbc_nss);
				//WL_INFO("[HT] Declare supporting RX STBC = %d\n", rx_stbc_nss);
			}
		}

		Func_Of_Proc_Pre_Memcpy(ht_capie.supp_mcs_set, pmlmeext->default_supported_mcs_set,
					16);

		Func_Of_Proc_Chip_Hw_Get_Hwreg(Nic, HW_VAR_RF_TYPE, (u8 *) (&rf_type));

		do_set_mcs_by_flag(ht_capie.supp_mcs_set, MCS_RATE_1R, 1);

		{
			u32 rx_packet_offset, max_recvbuf_sz;
			Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_RX_PACKET_OFFSET,
								&rx_packet_offset);
			Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_MAX_RECVBUF_SZ,
								&max_recvbuf_sz);
			if (max_recvbuf_sz - rx_packet_offset >= (8191 - 256)) {
				//WL_INFO("%s IEEE80211_HT_CAP_MAX_AMSDU is set\n", __FUNCTION__);
				ht_capie.cap_info =
					ht_capie.cap_info | IEEE80211_HT_CAP_MAX_AMSDU;
			}
		}

		if (Nic->driver_rx_ampdu_factor != 0xFF)
			max_rx_ampdu_factor =
				(HT_CAP_AMPDU_FACTOR) Nic->driver_rx_ampdu_factor;
		else
			Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HW_VAR_MAX_RX_AMPDU_FACTOR,
								&max_rx_ampdu_factor);

		ht_capie.ampdu_params_info = (max_rx_ampdu_factor & 0x03);

		if (Nic->driver_rx_ampdu_spacing != 0xFF) {
			ht_capie.ampdu_params_info |=
				((Nic->driver_rx_ampdu_spacing & 0x07) << 2);
		} else {
			if (Nic->securitypriv.dot11PrivacyAlgrthm == _AES_) {
				Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HW_VAR_BEST_AMPDU_DENSITY,
									&best_ampdu_density);

				ht_capie.ampdu_params_info |=
					(IEEE80211_HT_CAP_AMPDU_DENSITY &
					 (best_ampdu_density << 2));

			} else
				ht_capie.ampdu_params_info |=
					(IEEE80211_HT_CAP_AMPDU_DENSITY & 0x00);
		}

		pframe = ie_to_set_func(out_ie + out_len, _HT_CAPABILITY_IE_,
							sizeof(struct wl_ieee80211_ht_cap),
							(unsigned char *)&ht_capie, pout_len);

		phtpriv->ht_option = _TRUE;

		if (in_ie != NULL) {
			p = ie_to_get_func(in_ie, _HT_ADD_INFO_IE_, &ielen, in_len);
			if (p && (ielen == sizeof(struct ieee80211_ht_addt_info))) {
				out_len = *pout_len;
				pframe =
					ie_to_set_func(out_ie + out_len, _HT_ADD_INFO_IE_, ielen, p + 2,
							   pout_len);
			}
		}

		return (phtpriv->ht_option);
	} else {
		return 0;
	}
}

static int chk_sec_in_queue(u8 * bssid, PNIC Nic)
{
	int i = 0;
	struct security_priv *psecuritypriv = &Nic->securitypriv;

	do {
		if ((psecuritypriv->PMKIDList[i].bUsed) &&
			(Func_Of_Proc_Pre_Memcmp(psecuritypriv->PMKIDList[i].Bssid, bssid, ETH_ALEN) ==
			 _TRUE)) {
			break;
		} else {
			i++;
		}

	} while (i < NUM_PMKID_CACHE);

	if (i == NUM_PMKID_CACHE) {
		i = -1;
	}

	return (i);

}

static int do_remove_pmkid(PNIC Nic, u8 * ie, uint ie_len, u8 tag)
{
	int i;
	u16 pmkid_cnt = WL_GET_LE16(ie + 14 + 20);
	struct security_priv *sec = &Nic->securitypriv;
	if (tag) {
		if (ie[13] <= 20)
			goto exit;

		for (i = 0; i < pmkid_cnt; i++)
			//WL_INFO("    " KEY_FMT "\n", KEY_ARG(ie + 14 + 20 + 2 + i * 16));

		ie_len -= 2 + pmkid_cnt * 16;
		ie[13] = 20;
	}
exit:
	return (ie_len);
}

static int wl_do_append_pmkid(PNIC Nic, int iEntry, u8 * ie,
							uint ie_len, u8 tag)
{
	struct security_priv *sec = &Nic->securitypriv;
	if (tag) {
		if (ie[13] > 20) {
			int i;
			u16 pmkid_cnt = WL_GET_LE16(ie + 14 + 20);
			if (pmkid_cnt == 1
				&& Func_Of_Proc_Pre_Memcmp(ie + 14 + 20 + 2, &sec->PMKIDList[iEntry].PMKID,
							   16)) {
				goto exit;
			}

			ie_len -= 2 + pmkid_cnt * 16;
			ie[13] = 20;
		}

		if (ie[13] <= 20) {

			WL_PUT_LE16(&ie[ie_len], 1);
			ie_len += 2;

			Func_Of_Proc_Pre_Memcpy(&ie[ie_len], &sec->PMKIDList[iEntry].PMKID, 16);
			ie_len += 16;

			ie[13] += 18;
		}
exit:
		return (ie_len);
	} else {
		return 0;
	}
}


static void do_report_sec_ie(PNIC Nic, u8 * sec_ie, u8 authmode)
{
	uint len;
	u8 *buff, *p, i;
	union iwreq_data wrqu;

	_func_enter_;

	buff = NULL;
	if (authmode == _WPA_IE_ID_) {
		//WL_INFO_L2("do_report_sec_ie, authmode=%d\n", authmode);

		buff = wl_zmalloc(IW_CUSTOM_MAX);
		if (NULL == buff) {
			return;
		}
		p = buff;

		p += sprintf(p, "ASSOCINFO(ReqIEs=");

		len = sec_ie[1] + 2;
		len = (len < IW_CUSTOM_MAX) ? len : IW_CUSTOM_MAX;

		for (i = 0; i < len; i++) {
			p += sprintf(p, "%02x", sec_ie[i]);
		}

		p += sprintf(p, ")");

		Func_Of_Proc_Pre_Memset(&wrqu, 0, sizeof(wrqu));

		wrqu.data.length = p - buff;

		wrqu.data.length =
			(wrqu.data.length <
			 IW_CUSTOM_MAX) ? wrqu.data.length : IW_CUSTOM_MAX;

		wl_mfree(buff, IW_CUSTOM_MAX);
	}

exit:

	_func_exit_;

}

sint do_rebuild_sec_ie(PNIC  Nic, u8 * in_ie, u8 * out_ie,
						 uint in_len, u8 tag)
{
	int iEntry;
	uint ielength = 0, cnt, remove_cnt;
	u8 authmode = 0x0, securitytype, match;
	u8 wpa_oui[4] = { 0x0, 0x50, 0xf2, 0x01 };
	u8 sec_ie[255], uncst_oui[4], bkup_ie[255];

	struct security_priv *psecuritypriv = &Nic->securitypriv;
	uint ndisauthmode = psecuritypriv->ndisauthtype;
	uint ndissecuritytype = psecuritypriv->ndisencryptstatus;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;

	_func_enter_;

	if (tag) {
		Func_Of_Proc_Pre_Memcpy(out_ie, in_ie, 12);
		ielength = 12;
		if ((ndisauthmode == Ndis802_11AuthModeWPA)
			|| (ndisauthmode == Ndis802_11AuthModeWPAPSK))
			authmode = _WPA_IE_ID_;
		if ((ndisauthmode == Ndis802_11AuthModeWPA2)
			|| (ndisauthmode == Ndis802_11AuthModeWPA2PSK))
			authmode = _WPA2_IE_ID_;

		if (check_fwstate(pmlmepriv, WIFI_UNDER_WPS)) {
			Func_Of_Proc_Pre_Memcpy(out_ie + ielength, psecuritypriv->wps_ie,
						psecuritypriv->wps_ie_len);

			ielength += psecuritypriv->wps_ie_len;
		} else if ((authmode == _WPA_IE_ID_) || (authmode == _WPA2_IE_ID_)) {
			Func_Of_Proc_Pre_Memcpy(&out_ie[ielength], &psecuritypriv->supplicant_ie[0],
						psecuritypriv->supplicant_ie[1] + 2);
			ielength += psecuritypriv->supplicant_ie[1] + 2;
			do_report_sec_ie(Nic, psecuritypriv->supplicant_ie, authmode);

		}

		iEntry = chk_sec_in_queue(pmlmepriv->assoc_bssid, Nic);
		if (iEntry < 0) {
			if (authmode == _WPA2_IE_ID_)
				ielength = do_remove_pmkid(Nic, out_ie, ielength, 1);
		} else {
			if (authmode == _WPA2_IE_ID_)
				ielength =
					wl_do_append_pmkid(Nic, iEntry, out_ie, ielength, 1);
		}

		_func_exit_;
	}
	return ielength;
}

void wl_ht_use_default_setting(PNIC Nic, u8 tag)
{
	BOOLEAN bHwLDPCSupport = _FALSE, bHwSTBCSupport = _FALSE;
	struct registry_priv *pregistrypriv = &Nic->registrypriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;
	if (tag) {
		phtpriv->bss_coexist = 0;

		phtpriv->sgi_40m =
			TEST_FLAG(pregistrypriv->short_gi, BIT1) ? _TRUE : _FALSE;
		phtpriv->sgi_20m =
			TEST_FLAG(pregistrypriv->short_gi, BIT0) ? _TRUE : _FALSE;

		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_RX_LDPC, (u8 *) & bHwLDPCSupport);
		CLEAR_FLAGS(phtpriv->ldpc_cap);
		if (bHwLDPCSupport) {
			if (TEST_FLAG(pregistrypriv->ldpc_cap, BIT4))
				SET_FLAG(phtpriv->ldpc_cap, LDPC_HT_ENABLE_RX);
		}
		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_TX_LDPC, (u8 *) & bHwLDPCSupport);
		if (bHwLDPCSupport) {
			if (TEST_FLAG(pregistrypriv->ldpc_cap, BIT5))
				SET_FLAG(phtpriv->ldpc_cap, LDPC_HT_ENABLE_TX);
		}
		//if (phtpriv->ldpc_cap)
		//	WL_INFO("[HT] HAL Support LDPC = 0x%02X\n", phtpriv->ldpc_cap);

		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_TX_STBC, (u8 *) & bHwSTBCSupport);
		CLEAR_FLAGS(phtpriv->stbc_cap);
		if (bHwSTBCSupport) {
			if (TEST_FLAG(pregistrypriv->stbc_cap, BIT5))
				SET_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_TX);
		}
		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_RX_STBC, (u8 *) & bHwSTBCSupport);
		if (bHwSTBCSupport) {
			if (TEST_FLAG(pregistrypriv->stbc_cap, BIT4))
				SET_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_RX);
		}
		//if (phtpriv->stbc_cap)
		//	WL_INFO("[HT] HAL Support STBC = 0x%02X\n", phtpriv->stbc_cap);
	}
}

void do_make_wmm_ie_ht(PNIC Nic, u8 * out_ie, uint * pout_len,
						 u8 tag)
{
	int out_len;
	u8 *pframe;
	unsigned char WMM_IE[] = { 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x00 };
	if (tag) {
		if (Nic->mlmepriv.qospriv.qos_option == 0) {
			out_len = *pout_len;
			pframe = ie_to_set_func(out_ie + out_len, _VENDOR_SPECIFIC_IE_,
								_WMM_IE_Length_, WMM_IE, pout_len);

			Nic->mlmepriv.qospriv.qos_option = 1;
		}
	}
}

void wl_do_append_exented_cap(PNIC Nic, u8 * out_ie, uint * pout_len,
							u8 tag)
{
	u8 cap_content[8] = { 0 };
	u8 *pframe;
	u8 null_content[8] = { 0 };
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;
	if (tag) {
		if (phtpriv->bss_coexist) {
			SET_EXT_CAPABILITY_ELE_BSS_COEXIST(cap_content, 1);
		}

		if (_FALSE == Func_Of_Proc_Pre_Memcmp(cap_content, null_content, 8))
			pframe =
				ie_to_set_func(out_ie + *pout_len, EID_EXTCapability, 8,
						   cap_content, pout_len);
	}
}
#endif 



#if 0
#include <drv_types.h>
#include <pdata.h>

#ifndef DBG_CMD_EXECUTE
#define DBG_CMD_EXECUTE 0
#endif

#ifdef CONFIG_PROC_DEBUG

static struct proc_dir_entry *wl_proc = NULL;

inline struct proc_dir_entry *get_wl_drv_proc(void)
{
	return wl_proc;
}

#define WL_PROC_NAME DRV_NAME

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0))
#define file_inode(file) ((file)->f_dentry->d_inode)
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0))
#define PDE_DATA(inode) PDE((inode))->data
#define proc_get_parent_data(inode) PDE((inode))->parent->data
#endif

#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24))
#define get_proc_net proc_net
#else
#define get_proc_net init_net.proc_net
#endif

inline struct proc_dir_entry *wl_proc_create_dir(const char *name, struct proc_dir_entry *parent, void *data)
{
	struct proc_dir_entry *entry;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	entry = proc_mkdir_data(name, S_IRUGO|S_IXUGO, parent, data);
#else
	entry = proc_mkdir(name, parent);
	if (entry)
		entry->data = data;
#endif

	return entry;
}

inline struct proc_dir_entry *wl_proc_create_entry(const char *name, struct proc_dir_entry *parent, 
	const struct file_operations *fops, void * data)
{
	struct proc_dir_entry *entry;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	entry = proc_create_data(name,  S_IFREG|S_IRUGO|S_IWUGO, parent, fops, data);
#else
	entry = create_proc_entry(name, S_IFREG|S_IRUGO|S_IWUGO, parent);
	if (entry) {
		entry->data = data;
		entry->proc_fops = fops;
	}
#endif

	return entry;
}


/*
* wl_drv_proc:
* init/deinit when register/unregister driver
*/
const struct wl_proc_hdl drv_proc_hdls [] = {
#if 0
	{"ver_info", proc_get_drv_version, NULL},
	{"log_level", proc_get_log_level, proc_set_log_level},
	{"drv_cfg", proc_get_drv_cfg, NULL},
#ifdef DBG_MEM_ALLOC
	{"mstat", proc_get_mstat, NULL},
#endif /* DBG_MEM_ALLOC */
	{"country_chplan_map", proc_get_country_chplan_map, NULL},
	{"chplan_id_list", proc_get_chplan_id_list, NULL},
	{"chplan_test", proc_get_chplan_test, NULL},
#endif
};

const int drv_proc_hdls_num = sizeof(drv_proc_hdls) / sizeof(struct wl_proc_hdl);

static int wl_drv_proc_open(struct inode *inode, struct file *file)
{
	ssize_t index = (ssize_t)PDE_DATA(inode);
	const struct wl_proc_hdl *hdl = drv_proc_hdls+index;
       return single_open(file, hdl->show, NULL);
}

static ssize_t wl_drv_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
	ssize_t index = (ssize_t)PDE_DATA(file_inode(file));
	const struct wl_proc_hdl *hdl = drv_proc_hdls+index;
	ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *, void *) = hdl->write;
	
	if (write)
		return write(file, buffer, count, pos, NULL);

	return -EROFS;
}

static const struct file_operations wl_drv_proc_fops = {
	.owner = THIS_MODULE,
	.open = wl_drv_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.write = wl_drv_proc_write,
};

int wl_drv_proc_init(void)
{
	int ret = _FAIL;
	ssize_t i;
	struct proc_dir_entry *entry = NULL;

	if (wl_proc != NULL) {
		wl_warn_on(1);
		goto exit;
	}

	wl_proc = wl_proc_create_dir(WL_PROC_NAME, get_proc_net, NULL);

	if (wl_proc == NULL) {
		wl_warn_on(1);
		goto exit;
	}

	for (i=0;i<drv_proc_hdls_num;i++) {
		entry = wl_proc_create_entry(drv_proc_hdls[i].name, wl_proc, &wl_drv_proc_fops, (void *)i);
		if (!entry) {
			wl_warn_on(1);
			goto exit;
		}
	}

	ret = _SUCCESS;

exit:
	return ret;
}

void wl_drv_proc_deinit(void)
{
	int i;

	if (wl_proc == NULL)
		return;

	for (i=0;i<drv_proc_hdls_num;i++)
		remove_proc_entry(drv_proc_hdls[i].name, wl_proc);

	remove_proc_entry(WL_PROC_NAME, get_proc_net);
	wl_proc = NULL;
}


/*
* wl_wadptdata_proc:
* init/deinit when register/unregister net_device
*/
const struct wl_proc_hdl adapter_proc_hdls [] = {
#if 0
	{"write_reg", proc_get_dummy, proc_set_write_reg},
	{"read_reg", proc_get_read_reg, proc_set_read_reg},
	{"adapters_status", proc_get_dump_wadptdatas_status, NULL},
	{"fwstate", proc_get_fwstate, NULL},
	{"sec_info", proc_get_sec_info, NULL},
	{"mlmext_state", proc_get_mlmext_state, NULL},
	{"qos_option", proc_get_qos_option, NULL},
	{"ht_option", proc_get_ht_option, NULL},
	{"rf_info", proc_get_rf_info, NULL},
	{"scan_param", proc_get_scan_param, proc_set_scan_param},
	{"scan_abort", proc_get_scan_abort, NULL},	
#ifdef CONFIG_SCAN_BACKOP
	{"backop_flags_sta", proc_get_backop_flags_sta, proc_set_backop_flags_sta},
	{"backop_flags_ap", proc_get_backop_flags_ap, proc_set_backop_flags_ap},
#endif
	{"survey_info", proc_get_survey_info, proc_set_survey_info},
	{"ap_info", proc_get_ap_info, NULL},
	{"trx_info", proc_get_trx_info, proc_reset_trx_info},
	{"rate_ctl", proc_get_rate_ctl, proc_set_rate_ctl},
	{"dis_pwt_ctl", proc_get_dis_pwt, proc_set_dis_pwt},
	{"mac_qinfo", proc_get_mac_qinfo, NULL},
	{"macid_info", proc_get_macid_info, NULL},
	{"sec_cam", proc_get_sec_cam, proc_set_sec_cam},
	{"sec_cam_cache", proc_get_sec_cam_cache, NULL},
	{"suspend_info", proc_get_suspend_resume_info, NULL},
	{"wifi_spec",proc_get_wifi_spec,NULL},
#ifdef CONFIG_LAYER2_ROAMING
	{"roam_flags", proc_get_roam_flags, proc_set_roam_flags},
	{"roam_param", proc_get_roam_param, proc_set_roam_param},
	{"roam_tgt_addr", proc_get_dummy, proc_set_roam_tgt_addr},
#endif /* CONFIG_LAYER2_ROAMING */

	{"fwdl_test_case", proc_get_dummy, proc_set_fwdl_test_case},
	{"del_rx_ampdu_test_case", proc_get_dummy, proc_set_del_rx_ampdu_test_case},
	{"wait_hiq_empty", proc_get_dummy, proc_set_wait_hiq_empty},

	{"mac_reg_dump", proc_get_mac_reg_dump, NULL},
	{"bb_reg_dump", proc_get_bb_reg_dump, NULL},
	{"rf_reg_dump", proc_get_rf_reg_dump, NULL},
#ifdef CONFIG_AP_MODE
	{"all_sta_info", proc_get_all_sta_info, NULL},
#endif /* CONFIG_AP_MODE */

#endif	
#ifdef DBG_MEMORY_LEAK
	{"_malloc_cnt", proc_get_malloc_cnt, NULL},
#endif /* DBG_MEMORY_LEAK */

#if 0
#ifdef CONFIG_FIND_BEST_CHANNEL
	{"best_channel", proc_get_best_channel, proc_set_best_channel},
#endif

	{"rx_signal", proc_get_rx_signal, proc_set_rx_signal},
	{"hw_info", proc_get_hw_status, NULL},
#ifdef CONFIG_80211N_HT
	{"ht_enable", proc_get_ht_enable, proc_set_ht_enable},
	{"bw_mode", proc_get_bw_mode, proc_set_bw_mode},
	{"ampdu_enable", proc_get_ampdu_enable, proc_set_ampdu_enable},
	{"rx_stbc", proc_get_rx_stbc, proc_set_rx_stbc},
	{"rx_ampdu", proc_get_rx_ampdu, proc_set_rx_ampdu},
	{"rx_ampdu_factor",proc_get_rx_ampdu_factor,proc_set_rx_ampdu_factor},
	{"rx_ampdu_density",proc_get_rx_ampdu_density,proc_set_rx_ampdu_density},
	{"tx_ampdu_density",proc_get_tx_ampdu_density,proc_set_tx_ampdu_density},	 
#endif /* CONFIG_80211N_HT */

	{"en_fwps", proc_get_en_fwps, proc_set_en_fwps},
	{"mac_rptbuf", proc_get_mac_rptbuf, NULL},

#if defined(DBG_CONFIG_ERROR_DETECT)
	{"sreset", proc_get_sreset, proc_set_sreset},
#endif /* DBG_CONFIG_ERROR_DETECT */
	{"trx_info_debug", proc_get_trx_info_debug, NULL},
	{"linked_info_dump",proc_get_linked_info_dump,proc_set_linked_info_dump},

	{"current_tx_rate", proc_get_current_tx_rate, NULL},
#ifdef CONFIG_GPIO_API
	{"gpio_info",proc_get_gpio,proc_set_gpio},
	{"gpio_set_output_value",proc_get_dummy,proc_set_gpio_output_value},
	{"gpio_set_direction",proc_get_dummy,proc_set_config_gpio},
#endif

#ifdef CONFIG_DBG_COUNTER
	{"rx_logs", proc_get_rx_logs, NULL},
	{"tx_logs", proc_get_tx_logs, NULL},
	{"int_logs", proc_get_int_logs, NULL},
#endif

#ifdef CONFIG_GPIO_WAKEUP
	{"wowlan_gpio_info", proc_get_wowlan_gpio_info,
		proc_set_wowlan_gpio_info},
#endif
#ifdef CONFIG_P2P_WOWLAN
	{"p2p_wowlan_info", proc_get_p2p_wowlan_info, NULL},
#endif
	{"country_code", proc_get_country_code, proc_set_country_code},
	{"chan_plan", proc_get_chan_plan, proc_set_chan_plan},
#ifdef CONFIG_DFS_MASTER
	{"dfs_master_test_case", proc_get_dfs_master_test_case, proc_set_dfs_master_test_case},
	{"update_non_ocp", proc_get_dummy, proc_set_update_non_ocp},
	{"radar_detect", proc_get_dummy, proc_set_radar_detect},
#endif
	{"new_bcn_max", proc_get_new_bcn_max, proc_set_new_bcn_max},
	{"sink_udpport",proc_get_udpport,proc_set_udpport},
#ifdef DBG_RX_COUNTER_DUMP
	{"dump_rx_cnt_mode",proc_get_rx_cnt_dump,proc_set_rx_cnt_dump},
#endif	
	{"change_bss_chbw", NULL, proc_set_change_bss_chbw},
	{"target_tx_power", proc_get_target_tx_power, NULL},
	{"tx_power_by_rate", proc_get_tx_power_by_rate, NULL},
	{"tx_power_limit", proc_get_tx_power_limit, NULL},
	{"tx_power_ext_info", proc_get_tx_power_ext_info, proc_set_tx_power_ext_info},
#ifdef CONFIG_RF_POWER_TRIM
	{"tx_gain_offset", proc_get_dummy, proc_set_tx_gain_offset},
	{"kfree_flag", proc_get_kfree_flag, proc_set_kfree_flag},
	{"kfree_bb_gain", proc_get_kfree_bb_gain, proc_set_kfree_bb_gain},
	{"kfree_thermal", proc_get_kfree_thermal, proc_set_kfree_thermal},
#endif
#ifdef CONFIG_POWER_SAVING
	{"ps_info",proc_get_ps_info, NULL},
#endif
#ifdef CONFIG_TDLS
	{"tdls_info", proc_get_tdls_info, NULL},
#endif
	{"monitor", proc_get_monitor, proc_set_monitor},

#ifdef CONFIG_AUTO_CHNL_SEL_NHM
	{"acs", proc_get_best_chan, proc_set_acs},
#endif
#ifdef CONFIG_PREALLOC_RX_SKB_BUFFER
	{"rtkm_info", proc_get_rtkm_info, NULL},
#endif
	{"efuse_map", proc_get_efuse_map, NULL},
#ifdef CONFIG_IEEE80211W
	{"11w_tx_sa_query", proc_get_tx_sa_query, proc_set_tx_sa_query},
	{"11w_tx_deauth", proc_get_tx_deauth, proc_set_tx_deauth},
	{"11w_tx_auth", proc_get_tx_auth, proc_set_tx_auth},
#endif /* CONFIG_IEEE80211W */
	{"hal_spec", proc_get_hal_spec, NULL},
#endif
};

const int adapter_proc_hdls_num = sizeof(adapter_proc_hdls) / sizeof(struct wl_proc_hdl);

static int wl_wadptdata_proc_open(struct inode *inode, struct file *file)
{
	ssize_t index = (ssize_t)PDE_DATA(inode);
	const struct wl_proc_hdl *hdl = adapter_proc_hdls+index;

	return single_open(file, hdl->show, proc_get_parent_data(inode));
}

static ssize_t wl_wadptdata_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
	ssize_t index = (ssize_t)PDE_DATA(file_inode(file));
	const struct wl_proc_hdl *hdl = adapter_proc_hdls+index;
	ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *, void *) = hdl->write;

	if (write)
		return write(file, buffer, count, pos, ((struct seq_file *)file->private_data)->private);

	return -EROFS;
}

static const struct file_operations wl_wadptdata_proc_fops = {
	.owner = THIS_MODULE,
	.open = wl_wadptdata_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.write = wl_wadptdata_proc_write,
};

struct proc_dir_entry *wl_wadptdata_proc_init(struct net_device *dev)
{
	struct proc_dir_entry *drv_proc = get_wl_drv_proc();
	struct proc_dir_entry *dir_dev = NULL;
	struct proc_dir_entry *entry = NULL;
	_wadptdata *adapter = wl_netdev_priv(dev);
	u8 rf_type;
	ssize_t i;

	if (drv_proc == NULL) {
		wl_warn_on(1);
		goto exit;
	}

	if (adapter->dir_dev != NULL) {
		wl_warn_on(1);
		goto exit;
	}

	dir_dev = wl_proc_create_dir(dev->name, drv_proc, dev);
	if (dir_dev == NULL) {
		wl_warn_on(1);
		goto exit;
	}

	adapter->dir_dev = dir_dev;

	for (i=0;i<adapter_proc_hdls_num;i++) {
		entry = wl_proc_create_entry(adapter_proc_hdls[i].name, dir_dev, &wl_wadptdata_proc_fops, (void *)i);
		if (!entry) {
			wl_warn_on(1);
			goto exit;
		}
	}


exit:
	return dir_dev;
}

void wl_wadptdata_proc_deinit(struct net_device *dev)
{
	struct proc_dir_entry *drv_proc = get_wl_drv_proc();
	struct proc_dir_entry *dir_dev = NULL;
	_wadptdata *adapter = wl_netdev_priv(dev);
	int i;

	dir_dev = adapter->dir_dev;

	if (dir_dev == NULL) {
		wl_warn_on(1);
		return;
	}

	for (i=0;i<adapter_proc_hdls_num;i++)
		remove_proc_entry(adapter_proc_hdls[i].name, dir_dev);


	remove_proc_entry(dev->name, drv_proc);

	adapter->dir_dev = NULL;
}

void wl_wadptdata_proc_replace(struct net_device *dev)
{
	struct proc_dir_entry *drv_proc = get_wl_drv_proc();
	struct proc_dir_entry *dir_dev = NULL;
	_wadptdata *adapter = wl_netdev_priv(dev);
	int i;

	dir_dev = adapter->dir_dev;

	if (dir_dev == NULL) {
		wl_warn_on(1);
		return;
	}

	for (i=0;i<adapter_proc_hdls_num;i++)
		remove_proc_entry(adapter_proc_hdls[i].name, dir_dev);


	remove_proc_entry(adapter->old_ifname, drv_proc);

	adapter->dir_dev = NULL;

	wl_wadptdata_proc_init(dev);

}

#endif /* CONFIG_PROC_DEBUG */


static u32 g_wait_hiq_empty_ms = 0;

static u32 wl_get_wait_hiq_empty_ms(void)
{
	return g_wait_hiq_empty_ms;
}

sint proc_init_cmd_priv_func(struct cmd_priv * pcmdpriv, u8 flag)
{
	sint res = _SUCCESS;

	_func_enter_;

	Func_Of_Proc_Pre_Init_Sema(&(pcmdpriv->cmd_queue_sema), 0);
	Func_Of_Proc_Pre_Init_Sema(&(pcmdpriv->terminate_cmdthread_sema), 0);

	if (flag) {
		Func_Of_Proc_Pre_Init_Queue(&(pcmdpriv->cmd_queue));
	}

	pcmdpriv->cmd_seq = 1;

	pcmdpriv->cmd_allocated_buf = wl_zmalloc(MAX_CMDSZ + CMDBUFF_ALIGN_SZ);

	if (pcmdpriv->cmd_allocated_buf == NULL) {
		res = _FAIL;
		goto exit;
	}

	pcmdpriv->cmd_buf =
		pcmdpriv->cmd_allocated_buf + CMDBUFF_ALIGN_SZ -
		((SIZE_PTR) (pcmdpriv->cmd_allocated_buf) & (CMDBUFF_ALIGN_SZ - 1));

	pcmdpriv->rsp_allocated_buf = wl_zmalloc(MAX_RSPSZ + 4);

	if (pcmdpriv->rsp_allocated_buf == NULL) {
		res = _FAIL;
		goto exit;
	}

	pcmdpriv->rsp_buf =
		pcmdpriv->rsp_allocated_buf + 4 -
		((SIZE_PTR) (pcmdpriv->rsp_allocated_buf) & 3);

	pcmdpriv->cmd_issued_cnt = pcmdpriv->cmd_done_cnt = pcmdpriv->rsp_cnt = 0;

	Func_Of_Proc_Pre_Mutex_Init(&pcmdpriv->sctx_mutex);
exit:

	_func_exit_;

	return res;

}

#ifdef CONFIG_wMBOX0_WK
static void callback_wmbox0_wk_func(_workitem * work);
#endif
sint proc_init_evt_priv_func(struct evt_priv *pevtpriv, u8 flag)
{
	sint res = _SUCCESS;

	_func_enter_;

	Func_Atomic_Of_Set(&pevtpriv->event_seq, 0);
	pevtpriv->evt_done_cnt = 0;

#ifdef CONFIG_EVENT_THREAD_MODE

	if (flag) {
		Func_Of_Proc_Pre_Init_Sema(&(pevtpriv->evt_notify), 0);
		Func_Of_Proc_Pre_Init_Sema(&(pevtpriv->terminate_evtthread_sema), 0);
	}
	pevtpriv->evt_allocated_buf = wl_zmalloc(MAX_EVTSZ + 4);
	if (pevtpriv->evt_allocated_buf == NULL) {
		res = _FAIL;
		goto exit;
	}
	pevtpriv->evt_buf =
		pevtpriv->evt_allocated_buf + 4 -
		((unsigned int)(pevtpriv->evt_allocated_buf) & 3);

	Func_Of_Proc_Pre_Init_Queue(&(pevtpriv->evt_queue));

exit:

#endif

#ifdef CONFIG_wMBOX0_WK
	_init_workitem(&pevtpriv->wmbox0_wk, callback_wmbox0_wk_func, NULL);
	pevtpriv->wmbox0_wk_alive = _FALSE;
	pevtpriv->wmbox0_queue = Func_Of_Proc_Cbuf_Alloc(wMBOX0_QUEUE_MAX_LEN + 1);
#endif

	_func_exit_;

	return res;
}

void proc_free_evt_priv_func(struct evt_priv *pevtpriv, u8 flag)
{
	_func_enter_;

	WL_INFO_L2("+proc_free_evt_priv_func \n");

#ifdef CONFIG_EVENT_THREAD_MODE
	Func_Of_Procw_Free_Sema(&(pevtpriv->evt_notify));
	Func_Of_Procw_Free_Sema(&(pevtpriv->terminate_evtthread_sema));

	if (pevtpriv->evt_allocated_buf)
		wl_mfree(pevtpriv->evt_allocated_buf, MAX_EVTSZ + 4);
#endif

#ifdef CONFIG_wMBOX0_WK
	_cancel_workitem_sync(&pevtpriv->wmbox0_wk);
	while (pevtpriv->wmbox0_wk_alive)
		Func_Of_Proc_Msleep_Os(10);

	if (flag) {
		while (!Func_Of_Proc_Cbuf_Empty(pevtpriv->wmbox0_queue)) {
			void *wmbox0;
			if ((wmbox0 = Func_Of_Proc_Cbuf_Pop(pevtpriv->wmbox0_queue)) != NULL
				&& wmbox0 != (void *)pevtpriv) {
				wl_mfree(wmbox0, 16);
			}
		}
	}
	Func_Of_Proc_Cbuf_Free(pevtpriv->wmbox0_queue);
#endif

	WL_INFO_L2("-proc_free_evt_priv_func \n");

	_func_exit_;

}

void proc_free_cmd_priv_func(struct cmd_priv *pcmdpriv, u8 flag)
{
	_func_enter_;

	if (pcmdpriv) {
		Func_Of_Proc_Pre_Spinlock_Free(&(pcmdpriv->cmd_queue.lock));
		Func_Of_Procw_Free_Sema(&(pcmdpriv->cmd_queue_sema));
		Func_Of_Procw_Free_Sema(&(pcmdpriv->terminate_cmdthread_sema));

		if (pcmdpriv->cmd_allocated_buf)
			wl_mfree(pcmdpriv->cmd_allocated_buf,
					  MAX_CMDSZ + CMDBUFF_ALIGN_SZ);

		if (pcmdpriv->rsp_allocated_buf)
			wl_mfree(pcmdpriv->rsp_allocated_buf, MAX_RSPSZ + 4);

		if (flag) {
			Func_Of_Proc_Pre_Mutex_Free(&pcmdpriv->sctx_mutex);
		}
	}
	_func_exit_;
}

#ifdef DBG_CMD_QUEUE
extern u8 dump_cmd_id;
#endif

sint proc_enqueue_cmd_func(_queue * queue, struct cmd_obj *obj, bool to_head,
					  u8 flag)
{
	_irqL irqL;

	_func_enter_;

	if (flag) {
		if (obj == NULL)
			goto exit;
	}
	_enter_critical(&queue->lock, &irqL);

	if (to_head)
		Func_Of_Proc_List_Insert_Head(&obj->list, &queue->queue);
	else
		Func_Of_Proc_List_Insert_Tail(&obj->list, &queue->queue);

#ifdef DBG_CMD_QUEUE
	if (dump_cmd_id) {
		WL_INFO_L1("%s===> cmdcode:0x%02x\n", __FUNCTION__, obj->cmdcode);
		if (obj->cmdcode == GEN_CMD_CODE(_Set_MLME_EVT)) {
			if (obj->parmbuf) {
				struct wMBOX0Event_Header *pwmbox0_evt_hdr =
					(struct wMBOX0Event_Header *)(obj->parmbuf);
				WL_INFO_L1("pwmbox0_evt_hdr->ID:0x%02x(%d)\n", pwmbox0_evt_hdr->ID,
					   pwmbox0_evt_hdr->ID);
			}
		}
		if (obj->cmdcode == GEN_CMD_CODE(_Set_Drv_Extra)) {
			if (obj->parmbuf) {
				struct drvextra_cmd_parm *pdrvextra_cmd_parm =
					(struct drvextra_cmd_parm *)(obj->parmbuf);
				WL_INFO_L1("pdrvextra_cmd_parm->ec_id:0x%02x\n",
					   pdrvextra_cmd_parm->ec_id);
			}
		}
	}

	if (queue->queue.prev->next != &queue->queue) {
		WL_INFO
			("[%d] head %p, tail %p, tail->prev->next %p[tail], tail->next %p[head]\n",
			 __LINE__, &queue->queue, queue->queue.prev,
			 queue->queue.prev->prev->next, queue->queue.prev->next);

		WL_INFO("==========%s============\n", __FUNCTION__);
		WL_INFO("head:%p,obj_addr:%p\n", &queue->queue, obj);
		WL_INFO("pwadptdata: %p\n", obj->pwadptdata);
		WL_INFO("cmdcode: 0x%02x\n", obj->cmdcode);
		WL_INFO("res: %d\n", obj->res);
		WL_INFO("parmbuf: %p\n", obj->parmbuf);
		WL_INFO("cmdsz: %d\n", obj->cmdsz);
		WL_INFO("rsp: %p\n", obj->rsp);
		WL_INFO("rspsz: %d\n", obj->rspsz);
		WL_INFO("sctx: %p\n", obj->sctx);
		WL_INFO("list->next: %p\n", obj->list.next);
		WL_INFO("list->prev: %p\n", obj->list.prev);
	}
#endif

	_exit_critical(&queue->lock, &irqL);

exit:

	_func_exit_;

	return _SUCCESS;
}

#if 1
struct cmd_obj *proc_dequeue_cmd_func(_queue * queue)
{
	_irqL irqL;
	struct cmd_obj *obj;

	_func_enter_;

	_enter_critical(&queue->lock, &irqL);

#ifdef DBG_CMD_QUEUE
	if (queue->queue.prev->next != &queue->queue) {
		WL_INFO
			("[%d] head %p, tail %p, tail->prev->next %p[tail], tail->next %p[head]\n",
			 __LINE__, &queue->queue, queue->queue.prev,
			 queue->queue.prev->prev->next, queue->queue.prev->next);
	}
#endif

	if (Func_Of_Proc_Is_List_Empty(&(queue->queue))) {
		obj = NULL;
	} else {
		obj = LIST_CONTAINOR(get_next(&(queue->queue)), struct cmd_obj, list);

#ifdef DBG_CMD_QUEUE
		if (queue->queue.prev->next != &queue->queue) {
			WL_INFO("==========%s============\n", __FUNCTION__);
			WL_INFO("head:%p,obj_addr:%p\n", &queue->queue, obj);
			WL_INFO("pwadptdata: %p\n", obj->pwadptdata);
			WL_INFO("cmdcode: 0x%02x\n", obj->cmdcode);
			WL_INFO("res: %d\n", obj->res);
			WL_INFO("parmbuf: %p\n", obj->parmbuf);
			WL_INFO("cmdsz: %d\n", obj->cmdsz);
			WL_INFO("rsp: %p\n", obj->rsp);
			WL_INFO("rspsz: %d\n", obj->rspsz);
			WL_INFO("sctx: %p\n", obj->sctx);
			WL_INFO("list->next: %p\n", obj->list.next);
			WL_INFO("list->prev: %p\n", obj->list.prev);
		}

		if (dump_cmd_id) {
			WL_INFO("%s===> cmdcode:0x%02x\n", __FUNCTION__, obj->cmdcode);
			if (obj->cmdcode == GEN_CMD_CODE(_Set_Drv_Extra)) {
				if (obj->parmbuf) {
					struct drvextra_cmd_parm *pdrvextra_cmd_parm =
						(struct drvextra_cmd_parm *)(obj->parmbuf);
					WL_INFO_L1("pdrvextra_cmd_parm->ec_id:0x%02x\n",
						   pdrvextra_cmd_parm->ec_id);
				}
			}

		}
#endif

		wl_list_delete(&obj->list);
	}

	_exit_critical(&queue->lock, &irqL);

	_func_exit_;

	return obj;
}
#else
struct cmd_obj *proc_dequeue_cmd_func(_queue * queue)
{
	_irqL irqL;
	struct cmd_obj *obj;
	_list *phead, *plist;
	int cnt = 0;
	static int report_traffic_cnt = 0;
	_func_enter_;

	_enter_critical(&queue->lock, &irqL);

#ifdef DBG_CMD_QUEUE
	if (queue->queue.prev->next != &queue->queue) {
		WL_INFO
			("[%d] head %p, tail %p, tail->prev->next %p[tail], tail->next %p[head]\n",
			 __LINE__, &queue->queue, queue->queue.prev,
			 queue->queue.prev->prev->next, queue->queue.prev->next);
	}
#endif

#if 1

	if (++report_traffic_cnt >= 200) {
		report_traffic_cnt = 0;
		cnt = 0;
		phead = get_list_head(queue);
		plist = get_next(phead);

		while ((Func_Of_Proc_End_Of_Queue_Search(phead, plist)) == _FALSE) {
			plist = get_next(plist);
			cnt++;
		}

		WL_INFO_L1("cmd thread traffic pendig = %d\r\n", cnt);
	}
#endif

	if (Func_Of_Proc_Is_List_Empty(&(queue->queue))) {
		obj = NULL;
	} else {
		obj = LIST_CONTAINOR(get_next(&(queue->queue)), struct cmd_obj, list);

#ifdef DBG_CMD_QUEUE
		if (queue->queue.prev->next != &queue->queue) {
			WL_INFO("==========%s============\n", __FUNCTION__);
			WL_INFO("head:%p,obj_addr:%p\n", &queue->queue, obj);
			WL_INFO("pwadptdata: %p\n", obj->pwadptdata);
			WL_INFO("cmdcode: 0x%02x\n", obj->cmdcode);
			WL_INFO("res: %d\n", obj->res);
			WL_INFO("parmbuf: %p\n", obj->parmbuf);
			WL_INFO("cmdsz: %d\n", obj->cmdsz);
			WL_INFO("rsp: %p\n", obj->rsp);
			WL_INFO("rspsz: %d\n", obj->rspsz);
			WL_INFO("sctx: %p\n", obj->sctx);
			WL_INFO("list->next: %p\n", obj->list.next);
			WL_INFO("list->prev: %p\n", obj->list.prev);
		}

		if (dump_cmd_id) {
			WL_INFO("%s===> cmdcode:0x%02x\n", __FUNCTION__, obj->cmdcode);
			if (obj->cmdcode == GEN_CMD_CODE(_Set_Drv_Extra)) {
				if (obj->parmbuf) {
					struct drvextra_cmd_parm *pdrvextra_cmd_parm =
						(struct drvextra_cmd_parm *)(obj->parmbuf);
					WL_INFO_L1("pdrvextra_cmd_parm->ec_id:0x%02x\n",
						   pdrvextra_cmd_parm->ec_id);
				}
			}

		}
#endif

		wl_list_delete(&obj->list);
	}

	_exit_critical(&queue->lock, &irqL);

	_func_exit_;

	return obj;
}

#endif
u32 cmd_priv_to_init_func(struct cmd_priv * pcmdpriv)
{
	u32 res;
	_func_enter_;
	res = proc_init_cmd_priv_func(pcmdpriv, 1);
	_func_exit_;
	return res;
}

u32 evt_priv_to_init_func(struct evt_priv * pevtpriv)
{
	int res;
	_func_enter_;
	res = proc_init_evt_priv_func(pevtpriv, 1);
	_func_exit_;
	return res;
}

void preproc_free_evt_priv_func(struct evt_priv *pevtpriv)
{
	_func_enter_;
	WL_INFO_L2("preproc_free_evt_priv_func\n");
	proc_free_evt_priv_func(pevtpriv, 1);
	_func_exit_;
}

void preproc_free_cmd_priv_func(struct cmd_priv *pcmdpriv)
{
	_func_enter_;
	WL_INFO_L2("preproc_free_cmd_priv_func\n");
	proc_free_cmd_priv_func(pcmdpriv, 1);
	_func_exit_;
}


static int proc_cmd_filter_func(struct cmd_priv *pcmdpriv, struct cmd_obj *cmd_obj, u8 flag)
{
	u8 bAllow = _FALSE;

	if (flag) {
		if (cmd_obj->cmdcode == GEN_CMD_CODE(_SetChannelPlan))
			bAllow = _TRUE;
	}
	if (cmd_obj->no_io)
		bAllow = _TRUE;

	if ((!wl_is_hw_init_completed(pcmdpriv->pwadptdata) && (bAllow == _FALSE))
		|| Func_Atomic_Of_Read(&(pcmdpriv->cmdthd_running)) == _FALSE) {
		if (DBG_CMD_EXECUTE)
			WL_INFO(ADPT_FMT " drop " CMD_FMT
					" hw_init_completed:%u, cmdthd_running:%u\n",
					ADPT_ARG(cmd_obj->pwadptdata)
					, CMD_ARG(cmd_obj),
					wl_get_hw_init_completed(cmd_obj->pwadptdata),
					Func_Atomic_Of_Read(&pcmdpriv->cmdthd_running));
		if (0)
			wl_warn_on(1);

		return _FAIL;
	}
	return _SUCCESS;
}

u32 preproc_enqueue_cmd_func(struct cmd_priv * pcmdpriv, struct cmd_obj * cmd_obj)
{
	int res = _FAIL;
	PwADPTDATA pwadptdata = pcmdpriv->pwadptdata;

	_func_enter_;

	if (cmd_obj == NULL) {
		goto exit;
	}

	cmd_obj->pwadptdata = pwadptdata;

#ifdef CONFIG_CONCURRENT_MODE
	if (pwadptdata->wadptdata_type != PRIMARY_wADPTDATA && pwadptdata->pbuddy_wadptdata)
		pcmdpriv = &(pwadptdata->pbuddy_wadptdata->cmdpriv);
#endif

	res = proc_cmd_filter_func(pcmdpriv, cmd_obj, 1);
	if ((_FAIL == res) || (cmd_obj->cmdsz > MAX_CMDSZ)) {
		if (cmd_obj->cmdsz > MAX_CMDSZ) {
			WL_INFO("%s failed due to obj->cmdsz(%d) > MAX_CMDSZ(%d)\n",
					__func__, cmd_obj->cmdsz, MAX_CMDSZ);
			wl_warn_on(1);
		}

		if (cmd_obj->cmdcode == GEN_CMD_CODE(_Set_Drv_Extra)) {
			struct drvextra_cmd_parm *extra_parm =
				(struct drvextra_cmd_parm *)cmd_obj->parmbuf;

			if (extra_parm->pbuf && extra_parm->size > 0)
				wl_mfree(extra_parm->pbuf, extra_parm->size);
		}
		proc_free_cmd_obj_func(cmd_obj);
		goto exit;
	}

	res = proc_enqueue_cmd_func(&pcmdpriv->cmd_queue, cmd_obj, 0, 1);

	if (res == _SUCCESS)
		Func_Of_Proc_Pre_Up_Sema(&pcmdpriv->cmd_queue_sema);

exit:

	_func_exit_;

	return res;
}

struct cmd_obj *preproc_dequeue_cmd_func(struct cmd_priv *pcmdpriv)
{
	struct cmd_obj *cmd_obj;

	_func_enter_;

	cmd_obj = proc_dequeue_cmd_func(&pcmdpriv->cmd_queue);

	_func_exit_;
	return cmd_obj;
}

void proc_cmd_clr_isr_func(struct cmd_priv *pcmdpriv)
{
	_func_enter_;
	pcmdpriv->cmd_done_cnt++;
	_func_exit_;
}

void proc_free_cmd_obj_func(struct cmd_obj *pcmd)
{
	struct drvextra_cmd_parm *extra_parm = NULL;
	_func_enter_;

	if (pcmd->parmbuf != NULL) {
		wl_mfree((unsigned char *)pcmd->parmbuf, pcmd->cmdsz);
	}
	if (pcmd->rsp != NULL) {
		if (pcmd->rspsz != 0) {
			wl_mfree((unsigned char *)pcmd->rsp, pcmd->rspsz);
		}
	}

	wl_mfree((unsigned char *)pcmd, sizeof(struct cmd_obj));

	_func_exit_;
}

void proc_stop_cmd_thread_func(_wadptdata * wadptdata, u8 flag)
{
	if (flag) {
		if (wadptdata->cmdThread &&
			Func_Atomic_Of_Read(&(wadptdata->cmdpriv.cmdthd_running)) == _TRUE &&
			wadptdata->cmdpriv.stop_req == 0) {
			wadptdata->cmdpriv.stop_req = 1;
			Func_Of_Proc_Pre_Up_Sema(&wadptdata->cmdpriv.cmd_queue_sema);
			Func_Of_Proc_Down_Sema(&wadptdata->cmdpriv.terminate_cmdthread_sema);
		}
	}
}

thread_return proc_cmd_thread_func(thread_context context)
{
	u8 ret;
	struct cmd_obj *pcmd;
	u8 *pcmdbuf, *prspbuf;
	u32 cmd_start_time;
	u32 cmd_process_time;
	u8(*cmd_hdl) (_wadptdata * pwadptdata, u8 * pbuf);
	void (*pcmd_callback) (_wadptdata * dev, struct cmd_obj * pcmd);
	PwADPTDATA pwadptdata = (PwADPTDATA) context;
	struct cmd_priv *pcmdpriv = &(pwadptdata->cmdpriv);
	struct drvextra_cmd_parm *extra_parm = NULL;
	_irqL irqL;
	_func_enter_;

	thread_enter("WL_CMD_THREAD");

	pcmdbuf = pcmdpriv->cmd_buf;
	prspbuf = pcmdpriv->rsp_buf;

	pcmdpriv->stop_req = 0;
	Func_Atomic_Of_Set(&(pcmdpriv->cmdthd_running), _TRUE);
	Func_Of_Proc_Pre_Up_Sema(&pcmdpriv->terminate_cmdthread_sema);

	WL_INFO_L2("start r908621x proc_cmd_thread_func !!!!\n");

	while (1) {
		if (Func_Of_Proc_Down_Sema(&pcmdpriv->cmd_queue_sema) == _FAIL) {
			WL_INFO_L1(FUNC_ADPT_FMT
					   " Func_Of_Proc_Down_Sema(&pcmdpriv->cmd_queue_sema) return _FAIL, break\n",
					   FUNC_ADPT_ARG(pwadptdata));
			break;
		}

		if (WL_CANNOT_RUN(pwadptdata)) {
			WL_INFO_L1
				("%s: DriverStopped(%s) SurpriseRemoved(%s) break at line %d\n",
				 __func__, wl_is_drv_stopped(pwadptdata) ? "True" : "False",
				 wl_is_surprise_removed(pwadptdata) ? "True" : "False",
				 __LINE__);
			break;
		}

		if (pcmdpriv->stop_req) {
			WL_INFO_L1(FUNC_ADPT_FMT " stop_req:%u, break\n",
					   FUNC_ADPT_ARG(pwadptdata), pcmdpriv->stop_req);
			break;
		}

		_enter_critical(&pcmdpriv->cmd_queue.lock, &irqL);
		if (Func_Of_Proc_Is_List_Empty(&(pcmdpriv->cmd_queue.queue))) {
			_exit_critical(&pcmdpriv->cmd_queue.lock, &irqL);
			continue;
		}
		_exit_critical(&pcmdpriv->cmd_queue.lock, &irqL);

_next:
		if (WL_CANNOT_RUN(pwadptdata)) {
			WL_INFO_L1
				("%s: DriverStopped(%s) SurpriseRemoved(%s) break at line %d\n",
				 __func__, wl_is_drv_stopped(pwadptdata) ? "True" : "False",
				 wl_is_surprise_removed(pwadptdata) ? "True" : "False",
				 __LINE__);
			break;
		}

		pcmd = preproc_dequeue_cmd_func(pcmdpriv);
		if (!pcmd) {
			continue;
		}

		cmd_start_time = Func_Of_Proc_Get_Current_Time();
		pcmdpriv->cmd_issued_cnt++;

		if (pcmd->cmdsz > MAX_CMDSZ) {
			WL_INFO_L1("%s cmdsz:%d > MAX_CMDSZ:%d\n", __func__, pcmd->cmdsz,
					   MAX_CMDSZ);
			pcmd->res = wMBOX1_PARAMETERS_ERROR;
			goto post_process;
		}

		if (pcmd->cmdcode >= (sizeof(wlancmds) / sizeof(struct cmd_hdl))) {
			WL_INFO_L1("%s undefined cmdcode:%d\n", __func__, pcmd->cmdcode);
			pcmd->res = wMBOX1_PARAMETERS_ERROR;
			goto post_process;
		}

		cmd_hdl = wlancmds[pcmd->cmdcode].wmbox1funs;
		if (!cmd_hdl) {
			WL_INFO_L1("%s no cmd_hdl for cmdcode:%d\n", __func__,
					   pcmd->cmdcode);
			pcmd->res = wMBOX1_PARAMETERS_ERROR;
			goto post_process;
		}

		if (_FAIL == proc_cmd_filter_func(pcmdpriv, pcmd, 1)) {
			pcmd->res = wMBOX1_DROPPED;
			if (pcmd->cmdcode == GEN_CMD_CODE(_Set_Drv_Extra)) {
				extra_parm = (struct drvextra_cmd_parm *)pcmd->parmbuf;
				if (extra_parm && extra_parm->pbuf && extra_parm->size > 0)
					wl_mfree(extra_parm->pbuf, extra_parm->size);
			}
			goto post_process;
		}

		if (DBG_CMD_EXECUTE)
			WL_INFO(ADPT_FMT " " CMD_FMT " %sexecute\n",
					ADPT_ARG(pcmd->pwadptdata), CMD_ARG(pcmd)
					,
					pcmd->res == wMBOX1_ENQ_HEAD ? "ENQ_HEAD " : (pcmd->res ==
															   wMBOX1_ENQ_HEAD_FAIL
															   ?
															   "ENQ_HEAD_FAIL "
															   : ""));

		Func_Of_Proc_Pre_Memcpy(pcmdbuf, pcmd->parmbuf, pcmd->cmdsz);
		ret = cmd_hdl(pcmd->pwadptdata, pcmdbuf);
		pcmd->res = ret;

		pcmdpriv->cmd_seq++;

post_process:

		_enter_critical_mutex(&(pcmd->pwadptdata->cmdpriv.sctx_mutex), NULL);
		if (pcmd->sctx) {
			if (0)
				WL_INFO_L1(FUNC_ADPT_FMT " pcmd->sctx\n",
						   FUNC_ADPT_ARG(pcmd->pwadptdata));
			if (pcmd->res == wMBOX1_SUCCESS)
				do_stx_done_err(&pcmd->sctx, 0, 1);
			else
				do_stx_done_err(&pcmd->sctx, WL_SCTX_DONE_CMD_ERROR, 1);
		}
		_exit_critical_mutex(&(pcmd->pwadptdata->cmdpriv.sctx_mutex), NULL);

		cmd_process_time = Func_Of_Proc_Get_Passing_Time_Ms(cmd_start_time);
		if (cmd_process_time > 1000) {
			WL_INFO(ADPT_FMT " " CMD_FMT " process_time=%d\n",
					ADPT_ARG(pcmd->pwadptdata), CMD_ARG(pcmd), cmd_process_time);
			if (0)
				wl_warn_on(1);
		}

		if (pcmd->cmdcode <
			(sizeof(wl_cmd_callback) / sizeof(struct _cmd_callback))) {
			pcmd_callback = wl_cmd_callback[pcmd->cmdcode].callback;
			if (pcmd_callback == NULL) {
				WL_INFO_L2("mlme_cmd_hdl(): pcmd_callback=0x%p, cmdcode=0x%x\n", pcmd_callback, pcmd->cmdcode);
				proc_free_cmd_obj_func(pcmd);
			} else {
				pcmd_callback(pcmd->pwadptdata, pcmd);
			}
		} else {
			WL_INFO_L2("%s: cmdcode=0x%x callback not defined!\n",
						__FUNCTION__, pcmd->cmdcode);
			proc_free_cmd_obj_func(pcmd);
		}

		flush_signals_thread();

		goto _next;

	}

	Func_Atomic_Of_Set(&(pcmdpriv->cmdthd_running), _FALSE);

	do {
		pcmd = preproc_dequeue_cmd_func(pcmdpriv);
		if (pcmd == NULL)
			break;

		if (0)
			WL_INFO("%s: leaving... drop " CMD_FMT "\n", __func__,
					CMD_ARG(pcmd));

		if (pcmd->cmdcode == GEN_CMD_CODE(_Set_Drv_Extra)) {
			extra_parm = (struct drvextra_cmd_parm *)pcmd->parmbuf;
			if (extra_parm->pbuf && extra_parm->size > 0) {
				wl_mfree(extra_parm->pbuf, extra_parm->size);
			}
		}

		proc_free_cmd_obj_func(pcmd);
	} while (1);

	Func_Of_Proc_Pre_Up_Sema(&pcmdpriv->terminate_cmdthread_sema);

	_func_exit_;

	thread_exit();

}

#ifdef CONFIG_EVENT_THREAD_MODE
u32 proc_enqueue_evt_func(struct evt_priv *pevtpriv, struct evt_obj *obj)
{
	_irqL irqL;
	int res;
	_queue *queue = &pevtpriv->evt_queue;

	_func_enter_;

	res = _SUCCESS;

	if (obj == NULL) {
		res = _FAIL;
		goto exit;
	}

	_enter_critical_bh(&queue->lock, &irqL);

	Func_Of_Proc_List_Insert_Tail(&obj->list, &queue->queue);

	_exit_critical_bh(&queue->lock, &irqL);

exit:

	_func_exit_;

	return res;
}

struct evt_obj *proc_dequeue_evt_func(_queue * queue)
{
	_irqL irqL;
	struct evt_obj *pevtobj;

	_func_enter_;

	_enter_critical_bh(&queue->lock, &irqL);

	if (Func_Of_Proc_Is_List_Empty(&(queue->queue)))
		pevtobj = NULL;
	else {
		pevtobj =
			LIST_CONTAINOR(get_next(&(queue->queue)), struct evt_obj, list);
		wl_list_delete(&pevtobj->list);
	}

	_exit_critical_bh(&queue->lock, &irqL);

	_func_exit_;

	return pevtobj;
}

void proc_free_evt_obj_func(struct evt_obj *pevtobj)
{
	_func_enter_;

	if (pevtobj->parmbuf)
		wl_mfree((unsigned char *)pevtobj->parmbuf, pevtobj->evtsz);

	wl_mfree((unsigned char *)pevtobj, sizeof(struct evt_obj));

	_func_exit_;
}

void proc_evt_notify_isr_func(struct evt_priv *pevtpriv)
{
	_func_enter_;
	pevtpriv->evt_done_cnt++;
	Func_Of_Proc_Pre_Up_Sema(&(pevtpriv->evt_notify));
	_func_exit_;
}
#endif

u8 proc_setstandby_cmd_func(_wadptdata * pwadptdata, uint action)
{
	struct cmd_obj *pwmbox1;
	struct usb_suspend_parm *psetusbsuspend;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;

	u8 ret = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		ret = _FAIL;
		goto exit;
	}

	psetusbsuspend =
		(struct usb_suspend_parm *)wl_zmalloc(sizeof(struct usb_suspend_parm));
	if (psetusbsuspend == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		ret = _FAIL;
		goto exit;
	}

	psetusbsuspend->action = action;

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, psetusbsuspend,
							   GEN_CMD_CODE(Func_Set_Usb_Suspend));

	ret = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	_func_exit_;

	return ret;
}

u8 proc_sitesurvey_cmd_func(_wadptdata * pwadptdata, NDIS_802_11_SSID * ssid,
					  int ssid_num, struct wl_ieee80211_channel * ch,
					  int ch_num)
{
	u8 res = _FAIL;
	struct cmd_obj *pwmbox1;
	struct sitesurvey_parm *psurveyPara;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &(pwadptdata->wdinfo);
#endif

	_func_enter_;

#ifdef CONFIG_LPS
	if (check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE) {
		proc_lps_ctrl_wk_cmd_func(pwadptdata, LPS_CTRL_SCAN, 1);
	}
#endif

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL)
		return _FAIL;

	psurveyPara =
		(struct sitesurvey_parm *)wl_zmalloc(sizeof(struct sitesurvey_parm));
	if (psurveyPara == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		return _FAIL;
	}

	do_network_queue_unnew(pwadptdata, _FALSE, 1);

	WL_INFO_L2("%s: flush network queue\n", __FUNCTION__);

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, psurveyPara, GEN_CMD_CODE(_SiteSurvey));

	psurveyPara->scan_mode = pmlmepriv->scan_mode;

	if (ssid) {
		int i;
		for (i = 0; i < ssid_num && i < WL_SSID_SCAN_AMOUNT; i++) {
			if (ssid[i].SsidLength) {
				Func_Of_Proc_Pre_Memcpy(&psurveyPara->ssid[i], &ssid[i],
							sizeof(NDIS_802_11_SSID));
				psurveyPara->ssid_num++;
				if (0)
					WL_INFO(FUNC_ADPT_FMT " ssid:(%s, %d)\n",
							FUNC_ADPT_ARG(pwadptdata), psurveyPara->ssid[i].Ssid,
							psurveyPara->ssid[i].SsidLength);
			}
		}
	}

	if (ch) {
		int i;
		for (i = 0; i < ch_num && i < WL_CHANNEL_SCAN_AMOUNT; i++) {
			if (ch[i].hw_value && !(ch[i].flags & WL_IEEE80211_CHAN_DISABLED)) {
				Func_Of_Proc_Pre_Memcpy(&psurveyPara->ch[i], &ch[i],
							sizeof(struct wl_ieee80211_channel));
				psurveyPara->ch_num++;
				if (0)
					WL_INFO(FUNC_ADPT_FMT " ch:%u\n", FUNC_ADPT_ARG(pwadptdata),
							psurveyPara->ch[i].hw_value);
			}
		}
	}

	set_fwstate(pmlmepriv, _FW_UNDER_SURVEY);

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

	if (res == _SUCCESS) {

		pmlmepriv->scan_start_time = Func_Of_Proc_Get_Current_Time();

#ifdef CONFIG_SCAN_BACKOP
		if ((pwadptdata->pbuddy_wadptdata->mlmeextpriv.mlmext_info.state & 0x03) ==
			WIFI_FW_AP_STATE) {

			mlme_set_scan_to_timer(pmlmepriv,
								   CONC_SCANNING_TIMEOUT_SINGLE_BAND);
		} else
#endif
			mlme_set_scan_to_timer(pmlmepriv, SCANNING_TIMEOUT);

	} else {
		_clr_fwstate_(pmlmepriv, _FW_UNDER_SURVEY);
	}

	_func_exit_;

	return res;
}

u8 proc_setdatarate_cmd_func(_wadptdata * pwadptdata, u8 * rateset, u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct setdatarate_parm *pbsetdataratepara;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pbsetdataratepara =
		(struct setdatarate_parm *)wl_zmalloc(sizeof(struct setdatarate_parm));
	if (pbsetdataratepara == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	if (flag) {
		init_h2fwcmd_w_parm_no_rsp(pwmbox1, pbsetdataratepara,
								   GEN_CMD_CODE(_SetDataRate));
	}
#ifdef MP_FIRMWARE_OFFLOAD
	pbsetdataratepara->curr_rateidx = *(u32 *) rateset;
#else
	pbsetdataratepara->mac_id = 5;
	Func_Of_Proc_Pre_Memcpy(pbsetdataratepara->datarates, rateset, NumRates);
#endif
	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
exit:

	_func_exit_;

	return res;
}

u8 proc_setbasicrate_cmd_func(_wadptdata * pwadptdata, u8 * rateset)
{
	struct cmd_obj *pwmbox1;
	struct setbasicrate_parm *pssetbasicratepara;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}
	pssetbasicratepara =
		(struct setbasicrate_parm *)
		wl_zmalloc(sizeof(struct setbasicrate_parm));

	if (pssetbasicratepara == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, pssetbasicratepara, _SetBasicRate_CMD_);

	Func_Of_Proc_Pre_Memcpy(pssetbasicratepara->basicrates, rateset, NumRates);

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
exit:

	_func_exit_;

	return res;
}

u8 proc_setphy_cmd_func(_wadptdata * pwadptdata, u8 modem, u8 ch)
{
	struct cmd_obj *pwmbox1;
	struct setphy_parm *psetphypara;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}
	psetphypara = (struct setphy_parm *)wl_zmalloc(sizeof(struct setphy_parm));

	if (psetphypara == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, psetphypara, _SetPhy_CMD_);

	WL_INFO_L2("CH=%d, modem=%d", ch, modem);

	psetphypara->modem = modem;
	psetphypara->rfchannel = ch;

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
exit:
	_func_exit_;
	return res;
}

static u8 proc_getmacreg_cmd_func(_wadptdata * pwadptdata, u8 len, u32 addr)
{
	struct cmd_obj *pwmbox1;
	struct readMAC_parm *preadmacparm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;
	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}
	preadmacparm =
		(struct readMAC_parm *)wl_zmalloc(sizeof(struct readMAC_parm));

	if (preadmacparm == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, preadmacparm, GEN_CMD_CODE(_GetMACReg));

	preadmacparm->len = len;
	preadmacparm->addr = addr;

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:
	_func_exit_;
	return res;
}

void proc_usb_catc_trigger_cmd_func(_wadptdata * pwadptdata, const char *caller)
{
	WL_INFO("%s caller:%s\n", __func__, caller);
	proc_getmacreg_cmd_func(pwadptdata, 1, 0x1c4);
}

u8 proc_setbbreg_cmd_func(_wadptdata * pwadptdata, u8 offset, u8 val)
{
	struct cmd_obj *pwmbox1;
	struct writeBB_parm *pwritebbparm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;
	_func_enter_;
	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}
	pwritebbparm =
		(struct writeBB_parm *)wl_zmalloc(sizeof(struct writeBB_parm));

	if (pwritebbparm == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, pwritebbparm, GEN_CMD_CODE(_SetBBReg));

	pwritebbparm->offset = offset;
	pwritebbparm->value = val;

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
exit:
	_func_exit_;
	return res;
}

u8 proc_getbbreg_cmd_func(_wadptdata * pwadptdata, u8 offset, u8 * pval)
{
	struct cmd_obj *pwmbox1;
	struct readBB_parm *prdbbparm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;
	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}
	prdbbparm = (struct readBB_parm *)wl_zmalloc(sizeof(struct readBB_parm));

	if (prdbbparm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		return _FAIL;
	}

	Func_Of_Proc_Init_Listhead(&pwmbox1->list);
	pwmbox1->cmdcode = GEN_CMD_CODE(_GetBBReg);
	pwmbox1->parmbuf = (unsigned char *)prdbbparm;
	pwmbox1->cmdsz = sizeof(struct readBB_parm);
	pwmbox1->rsp = pval;
	pwmbox1->rspsz = sizeof(struct readBB_rsp);

	prdbbparm->offset = offset;

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
exit:
	_func_exit_;
	return res;
}

u8 proc_setrfreg_cmd_func(_wadptdata * pwadptdata, u8 offset, u32 val)
{
	struct cmd_obj *pwmbox1;
	struct writeRF_parm *pwriterfparm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;
	_func_enter_;
	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}
	pwriterfparm =
		(struct writeRF_parm *)wl_zmalloc(sizeof(struct writeRF_parm));

	if (pwriterfparm == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, pwriterfparm, GEN_CMD_CODE(_SetRFReg));

	pwriterfparm->offset = offset;
	pwriterfparm->value = val;

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
exit:
	_func_exit_;
	return res;
}

u8 proc_getrfreg_cmd_func(_wadptdata * pwadptdata, u8 offset, u8 * pval)
{
	struct cmd_obj *pwmbox1;
	struct readRF_parm *prdrfparm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	prdrfparm = (struct readRF_parm *)wl_zmalloc(sizeof(struct readRF_parm));
	if (prdrfparm == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	Func_Of_Proc_Init_Listhead(&pwmbox1->list);
	pwmbox1->cmdcode = GEN_CMD_CODE(_GetRFReg);
	pwmbox1->parmbuf = (unsigned char *)prdrfparm;
	pwmbox1->cmdsz = sizeof(struct readRF_parm);
	pwmbox1->rsp = pval;
	pwmbox1->rspsz = sizeof(struct readRF_rsp);

	prdrfparm->offset = offset;

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	_func_exit_;

	return res;
}

void callback_getbbrfreg_cmdrsp_callback_func(_wadptdata * pwadptdata, struct cmd_obj *pcmd)
{
	_func_enter_;

	wl_mfree((unsigned char *)pcmd->parmbuf, pcmd->cmdsz);
	wl_mfree((unsigned char *)pcmd, sizeof(struct cmd_obj));

#ifdef CONFIG_MP_INCLUDED
	if (pwadptdata->registrypriv.mp_mode == 1)
		pwadptdata->mppriv.workparam.bcompleted = _TRUE;
#endif
	_func_exit_;
}

void callback_readtssi_cmdrsp_func(_wadptdata * pwadptdata, struct cmd_obj *pcmd)
{
	_func_enter_;

	wl_mfree((unsigned char *)pcmd->parmbuf, pcmd->cmdsz);
	wl_mfree((unsigned char *)pcmd, sizeof(struct cmd_obj));

#ifdef CONFIG_MP_INCLUDED
	if (pwadptdata->registrypriv.mp_mode == 1)
		pwadptdata->mppriv.workparam.bcompleted = _TRUE;
#endif

	_func_exit_;
}

static u8 proc_createbss_cmd_func(_wadptdata * wadptdata, int flags, bool adhoc,
							s16 req_ch, u8 req_bw, u8 req_offset)
{
	struct cmd_obj *cmdobj;
	struct createbss_parm *parm;
	struct cmd_priv *pcmdpriv = &wadptdata->cmdpriv;
	struct mlme_priv *pmlmepriv = &wadptdata->mlmepriv;
	struct submit_ctx sctx;
	u8 res = _SUCCESS;

	parm = (struct createbss_parm *)wl_zmalloc(sizeof(*parm));
	if (parm == NULL) {
		res = _FAIL;
		goto exit;
	}

	if (adhoc) {
		parm->adhoc = 1;
	} else {
		parm->adhoc = 0;
		parm->req_ch = req_ch;
		parm->req_bw = req_bw;
		parm->req_offset = req_offset;
	}

	if (flags & WL_CMDF_DIRECTLY) {
		if (wMBOX1_SUCCESS != proc_createbss_func(wadptdata, (u8 *) parm))
			res = _FAIL;
		wl_mfree((u8 *) parm, sizeof(*parm));
	} else {
		cmdobj = (struct cmd_obj *)wl_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			wl_mfree((u8 *) parm, sizeof(*parm));
			goto exit;
		}

		init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, GEN_CMD_CODE(_CreateBss));

		if (flags & WL_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			do_init_sctx(&sctx, 2000, 1);
		}

		res = preproc_enqueue_cmd_func(pcmdpriv, cmdobj);

		if (res == _SUCCESS && (flags & WL_CMDF_WAIT_ACK)) {
			wl_sctx_wait(&sctx, __func__, 1);
			_enter_critical_mutex(&pcmdpriv->sctx_mutex, NULL);
			if (sctx.status == WL_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_exit_critical_mutex(&pcmdpriv->sctx_mutex, NULL);
		}
	}

exit:
	return res;
}

inline u8 proc_create_ibss_cmd_func(_wadptdata * wadptdata, int flags)
{
	return proc_createbss_cmd_func(wadptdata, flags, 1, -1, 0, 0);
}

inline u8 proc_startbss_cmd_func(_wadptdata * wadptdata, int flags)
{
	return proc_createbss_cmd_func(wadptdata, flags, 0, -1, 0, 0);
}

#endif 
static int proc_restruct_wmm_ie_func(PNIC Nic, u8 * in_ie, uint in_len,
							   u8 * out_ie, uint initial_out_len)
{
	unsigned int i, j;
	unsigned int ielength = 0;

	i = 12;
	while (i < in_len) {
		ielength = initial_out_len;

		if (in_ie[i] == 0xDD && in_ie[i + 2] == 0x00 && in_ie[i + 3] == 0x50
			&& in_ie[i + 4] == 0xF2 && in_ie[i + 5] == 0x02 && i + 5 < in_len) {

			for (j = i; j < i + 9; j++) {
				out_ie[ielength] = in_ie[j];
				ielength++;
			}
			out_ie[initial_out_len + 1] = 0x07;
			out_ie[initial_out_len + 6] = 0x00;
			out_ie[initial_out_len + 8] = 0x00;

			break;
		}

		i += (in_ie[i + 1] + 2);
	}

	return ielength;

}
#if 0
inline u8 proc_change_bss_chbw_cmd_func(_wadptdata * wadptdata, int flags, u8 req_ch,
								  u8 req_bw, u8 req_offset)
{
	return proc_createbss_cmd_func(wadptdata, flags, 0, req_ch, req_bw, req_offset);
}

#endif

u8 proc_joinbss_cmd_func(PNIC Nic, struct wlan_network * pnetwork)
{
	u8 *auth, res = _TRUE;
	uint t_len = 0;
	WLAN_BSSID_EX *psecnetwork;
	struct cmd_obj *pcmd;
	struct cmd_priv *pcmdpriv = &Nic->cmdpriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct qos_priv *pqospriv = &pmlmepriv->qospriv;
	struct security_priv *psecuritypriv = &Nic->securitypriv;
	struct registry_priv *pregistrypriv = &Nic->registrypriv;
#ifdef CONFIG_80211N_HT
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;
#endif
	NDIS_802_11_NETWORK_INFRASTRUCTURE ndis_network_mode =
		pnetwork->network.InfrastructureMode;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	u32 tmp_len;
	u8 *ptmp = NULL;
	_func_enter_;


	if (pmlmepriv->assoc_ssid.SsidLength == 0) {
		//WL_INFO_L2("+Join cmd: Any SSid\n");
	} else {
		//WL_INFO_L2("+Join cmd: SSid=[%s]\n", pmlmepriv->assoc_ssid.Ssid);
	}

	pcmd = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pcmd == NULL) {
		res = _FALSE;
		//WL_INFO_L2("proc_joinbss_cmd_func: memory allocate for cmd_obj fail!!!\n");
		goto exit;
	}
	t_len = sizeof(WLAN_BSSID_EX);

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE | WIFI_ADHOC_STATE) !=
		_TRUE) {
		switch (ndis_network_mode) {
		case Ndis802_11IBSS:
			set_fwstate(pmlmepriv, WIFI_ADHOC_STATE);
			break;

		case Ndis802_11Infrastructure:
			set_fwstate(pmlmepriv, WIFI_STATION_STATE);
			break;

		case Ndis802_11APMode:
		case Ndis802_11AutoUnknown:
		case Ndis802_11InfrastructureMax:
//		case Ndis802_11Monitor:
			break;

		}
	}

	pmlmeinfo->assoc_AP_vendor =
		assoc_AP_chk(pnetwork->network.IEs, pnetwork->network.IELength, 1);

	psecnetwork = (WLAN_BSSID_EX *) wl_zmalloc(sizeof(WLAN_BSSID_EX));
	if (psecnetwork == NULL) {
		if (pcmd != NULL)
			wl_mfree((unsigned char *)pcmd, sizeof(struct cmd_obj));

		res = _FALSE;

		//WL_INFO_L2("proc_joinbss_cmd_func :psecnetwork==NULL!!!\n");

		goto exit;
	}

	Func_Of_Proc_Pre_Memset(psecnetwork, 0, t_len);

	Func_Of_Proc_Pre_Memcpy(psecnetwork, &pnetwork->network,
				get_WLAN_BSSID_EX_sz(&pnetwork->network));

	auth = &psecuritypriv->authenticator_ie[0];
	psecuritypriv->authenticator_ie[0] = (unsigned char)psecnetwork->IELength;

	if ((psecnetwork->IELength - 12) < (256 - 1)) {
		Func_Of_Proc_Pre_Memcpy(&psecuritypriv->authenticator_ie[1], &psecnetwork->IEs[12],
					psecnetwork->IELength - 12);
	} else {
		Func_Of_Proc_Pre_Memcpy(&psecuritypriv->authenticator_ie[1], &psecnetwork->IEs[12],
					(256 - 1));
	}

	psecnetwork->IELength = 0;

	if (pmlmepriv->assoc_by_bssid == _FALSE) {
		Func_Of_Proc_Pre_Memcpy(&pmlmepriv->assoc_bssid[0],
					&pnetwork->network.MacAddress[0], ETH_ALEN);
	}

	psecnetwork->IELength =
		do_rebuild_sec_ie(Nic, &pnetwork->network.IEs[0],
							&psecnetwork->IEs[0], pnetwork->network.IELength,
							1);
	
	pqospriv->qos_option = 0;

	if (pregistrypriv->wmm_enable) {
		tmp_len =
			proc_restruct_wmm_ie_func(Nic, &pnetwork->network.IEs[0],
								pnetwork->network.IELength,
								&psecnetwork->IEs[0], psecnetwork->IELength);

		if (psecnetwork->IELength != tmp_len) {
			psecnetwork->IELength = tmp_len;
			pqospriv->qos_option = 1;
		} else {
			pqospriv->qos_option = 0;
		}
	}
#ifdef CONFIG_80211N_HT
	phtpriv->ht_option = _FALSE;
	ptmp =
		ie_to_get_func(&pnetwork->network.IEs[12], _HT_CAPABILITY_IE_, &tmp_len,
				   pnetwork->network.IELength - 12);
	if (pregistrypriv->ht_enable && ptmp && tmp_len > 0) {
		if ((Nic->securitypriv.dot11PrivacyAlgrthm != _WEP40_) &&
			(Nic->securitypriv.dot11PrivacyAlgrthm != _WEP104_) &&
			(Nic->securitypriv.dot11PrivacyAlgrthm != _TKIP_)) {
			wl_ht_use_default_setting(Nic, 1);

			do_make_wmm_ie_ht(Nic, &psecnetwork->IEs[0],
								&psecnetwork->IELength, 1);

			do_rebuild_ht_ie(Nic, &pnetwork->network.IEs[12],
								  &psecnetwork->IEs[0],
								  pnetwork->network.IELength - 12,
								  &psecnetwork->IELength,
								  pnetwork->network.Configuration.DSConfig, 1);
		}
	}

	wl_do_append_exented_cap(Nic, &psecnetwork->IEs[0],
						   &psecnetwork->IELength, 1);

#endif

	pcmd->cmdsz = sizeof(WLAN_BSSID_EX);

#ifdef CONFIG_WLT9012
	psecnetwork->Length = cpu_to_le32(psecnetwork->Length);
	psecnetwork->Ssid.SsidLength = cpu_to_le32(psecnetwork->Ssid.SsidLength);
	psecnetwork->Privacy = cpu_to_le32(psecnetwork->Privacy);
	psecnetwork->Rssi = cpu_to_le32(psecnetwork->Rssi);
	psecnetwork->NetworkTypeInUse = cpu_to_le32(psecnetwork->NetworkTypeInUse);
	psecnetwork->Configuration.ATIMWindow =
		cpu_to_le32(psecnetwork->Configuration.ATIMWindow);
	psecnetwork->Configuration.BeaconPeriod =
		cpu_to_le32(psecnetwork->Configuration.BeaconPeriod);
	psecnetwork->Configuration.DSConfig =
		cpu_to_le32(psecnetwork->Configuration.DSConfig);
	psecnetwork->Configuration.FHConfig.DwellTime =
		cpu_to_le32(psecnetwork->Configuration.FHConfig.DwellTime);
	psecnetwork->Configuration.FHConfig.HopPattern =
		cpu_to_le32(psecnetwork->Configuration.FHConfig.HopPattern);
	psecnetwork->Configuration.FHConfig.HopSet =
		cpu_to_le32(psecnetwork->Configuration.FHConfig.HopSet);
	psecnetwork->Configuration.FHConfig.Length =
		cpu_to_le32(psecnetwork->Configuration.FHConfig.Length);
	psecnetwork->Configuration.Length =
		cpu_to_le32(psecnetwork->Configuration.Length);
	psecnetwork->InfrastructureMode =
		cpu_to_le32(psecnetwork->InfrastructureMode);
	psecnetwork->IELength = cpu_to_le32(psecnetwork->IELength);
#endif

	Func_Of_Proc_Init_Listhead(&pcmd->list);
	pcmd->cmdcode = _JoinBss_CMD_;
	pcmd->parmbuf = (unsigned char *)psecnetwork;
	pcmd->rsp = NULL;
	pcmd->rspsz = 0;

//	res = preproc_enqueue_cmd_func(pcmdpriv, pcmd);

exit:

	_func_exit_;

	return res;
}

#if 0

u8 proc_disassoc_cmd_func(_wadptdata * pwadptdata, u32 deauth_timeout_ms, bool enqueue)
{
	struct cmd_obj *cmdobj = NULL;
	struct disconnect_parm *param = NULL;
	struct cmd_priv *cmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;

	WL_INFO_L2("+proc_disassoc_cmd_func\n");

	param = (struct disconnect_parm *)wl_zmalloc(sizeof(*param));
	if (param == NULL) {
		res = _FAIL;
		goto exit;
	}
	param->deauth_timeout_ms = deauth_timeout_ms;

	if (enqueue) {
		cmdobj = (struct cmd_obj *)wl_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			wl_mfree((u8 *) param, sizeof(*param));
			goto exit;
		}
		init_h2fwcmd_w_parm_no_rsp(cmdobj, param, _DisConnect_CMD_);
		res = preproc_enqueue_cmd_func(cmdpriv, cmdobj);
	} else {

		if (wMBOX1_SUCCESS != proc_disconnect_func(pwadptdata, (u8 *) param))
			res = _FAIL;
		wl_mfree((u8 *) param, sizeof(*param));
	}

exit:

	_func_exit_;

	return res;
}

u8 proc_setopmode_cmd_func(_wadptdata * pwadptdata,
					 NDIS_802_11_NETWORK_INFRASTRUCTURE networktype,
					 bool enqueue)
{
	struct cmd_obj *pwmbox1;
	struct setopmode_parm *psetop;

	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;
	psetop =
		(struct setopmode_parm *)wl_zmalloc(sizeof(struct setopmode_parm));

	if (psetop == NULL) {
		res = _FAIL;
		goto exit;
	}
	psetop->mode = (u8) networktype;

	if (enqueue) {
		pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
		if (pwmbox1 == NULL) {
			wl_mfree((u8 *) psetop, sizeof(*psetop));
			res = _FAIL;
			goto exit;
		}

		init_h2fwcmd_w_parm_no_rsp(pwmbox1, psetop, _SetOpMode_CMD_);
		res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
	} else {
		proc_setopmode_func(pwadptdata, (u8 *) psetop);
		wl_mfree((u8 *) psetop, sizeof(*psetop));
	}
exit:

	_func_exit_;

	return res;
}

u8 proc_setstakey_cmd_func(_wadptdata * pwadptdata, struct sta_info * sta, u8 key_type,
					 bool enqueue)
{
	struct cmd_obj *pwmbox1;
	struct set_stakey_parm *psetstakey_para;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	struct set_stakey_rsp *psetstakey_rsp = NULL;

	struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;
	struct security_priv *psecuritypriv = &pwadptdata->securitypriv;
	u8 res = _SUCCESS;

	_func_enter_;

	psetstakey_para =
		(struct set_stakey_parm *)wl_zmalloc(sizeof(struct set_stakey_parm));
	if (psetstakey_para == NULL) {
		res = _FAIL;
		goto exit;
	}

	Func_Of_Proc_Pre_Memcpy(psetstakey_para->addr, sta->hwaddr, ETH_ALEN);

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
		psetstakey_para->algorithm =
			(unsigned char)psecuritypriv->dot11PrivacyAlgrthm;
	} else {
		GET_ENCRY_ALGO(psecuritypriv, sta, psetstakey_para->algorithm, _FALSE);
	}

	if (key_type == GROUP_KEY) {
		Func_Of_Proc_Pre_Memcpy(&psetstakey_para->key,
					&psecuritypriv->dot118021XGrpKey[psecuritypriv->
													 dot118021XGrpKeyid].skey,
					16);
	} else if (key_type == UNICAST_KEY) {
		Func_Of_Proc_Pre_Memcpy(&psetstakey_para->key, &sta->dot118021x_UncstKey, 16);
	}

	pwadptdata->securitypriv.busetkipkey = _TRUE;

	if (enqueue) {
		pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
		if (pwmbox1 == NULL) {
			wl_mfree((u8 *) psetstakey_para, sizeof(struct set_stakey_parm));
			res = _FAIL;
			goto exit;
		}

		psetstakey_rsp =
			(struct set_stakey_rsp *)wl_zmalloc(sizeof(struct set_stakey_rsp));
		if (psetstakey_rsp == NULL) {
			wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
			wl_mfree((u8 *) psetstakey_para, sizeof(struct set_stakey_parm));
			res = _FAIL;
			goto exit;
		}

		init_h2fwcmd_w_parm_no_rsp(pwmbox1, psetstakey_para, _SetStaKey_CMD_);
		pwmbox1->rsp = (u8 *) psetstakey_rsp;
		pwmbox1->rspsz = sizeof(struct set_stakey_rsp);
		res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
	} else {
		proc_set_stakey_func(pwadptdata, (u8 *) psetstakey_para);
		wl_mfree((u8 *) psetstakey_para, sizeof(struct set_stakey_parm));
	}
exit:

	_func_exit_;

	return res;
}

u8 proc_clearstakey_cmd_func(_wadptdata * pwadptdata, struct sta_info * sta, u8 enqueue,
					   u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct set_stakey_parm *psetstakey_para;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	struct set_stakey_rsp *psetstakey_rsp = NULL;
	struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;
	struct security_priv *psecuritypriv = &pwadptdata->securitypriv;
	s16 cam_id = 0;
	u8 res = _SUCCESS;

	_func_enter_;

	if (!enqueue) {
		while ((cam_id = do_camid_search(-1, -1, pwadptdata, sta->hwaddr)) >= 0) {
			WL_INFO_L1("clear key for addr:" MAC_FMT ", camid:%d\n",
					   MAC_ARG(sta->hwaddr), cam_id);
			cam_entry_clean(pwadptdata, cam_id);
			wl_unnew_camid(pwadptdata, cam_id);
		}
	} else {
		pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
		if (pwmbox1 == NULL) {
			res = _FAIL;
			goto exit;
		}

		psetstakey_para =
			(struct set_stakey_parm *)
			wl_zmalloc(sizeof(struct set_stakey_parm));
		if (psetstakey_para == NULL) {
			wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
			res = _FAIL;
			goto exit;
		}

		psetstakey_rsp =
			(struct set_stakey_rsp *)wl_zmalloc(sizeof(struct set_stakey_rsp));
		if (psetstakey_rsp == NULL) {
			wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
			wl_mfree((u8 *) psetstakey_para, sizeof(struct set_stakey_parm));
			res = _FAIL;
			goto exit;
		}

		init_h2fwcmd_w_parm_no_rsp(pwmbox1, psetstakey_para, _SetStaKey_CMD_);
		pwmbox1->rsp = (u8 *) psetstakey_rsp;
		pwmbox1->rspsz = sizeof(struct set_stakey_rsp);

		if (flag) {
			Func_Of_Proc_Pre_Memcpy(psetstakey_para->addr, sta->hwaddr, ETH_ALEN);
		}
		psetstakey_para->algorithm = _NO_PRIVACY_;

		res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

	}

exit:

	_func_exit_;

	return res;
}

u8 proc_setrttbl_cmd_func(_wadptdata * pwadptdata, struct setratable_parm * prate_table)
{
	struct cmd_obj *pwmbox1;
	struct setratable_parm *psetrttblparm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;
	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}
	psetrttblparm =
		(struct setratable_parm *)wl_zmalloc(sizeof(struct setratable_parm));

	if (psetrttblparm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, psetrttblparm, GEN_CMD_CODE(_SetRaTable));

	Func_Of_Proc_Pre_Memcpy(psetrttblparm, prate_table, sizeof(struct setratable_parm));

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
exit:
	_func_exit_;
	return res;

}

u8 proc_getrttbl_cmd_func(_wadptdata * pwadptdata, struct getratable_rsp * pval)
{
	struct cmd_obj *pwmbox1;
	struct getratable_parm *pgetrttblparm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;
	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}
	pgetrttblparm =
		(struct getratable_parm *)wl_zmalloc(sizeof(struct getratable_parm));

	if (pgetrttblparm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	Func_Of_Proc_Init_Listhead(&pwmbox1->list);
	pwmbox1->cmdcode = GEN_CMD_CODE(_GetRaTable);
	pwmbox1->parmbuf = (unsigned char *)pgetrttblparm;
	pwmbox1->cmdsz = sizeof(struct getratable_parm);
	pwmbox1->rsp = (u8 *) pval;
	pwmbox1->rspsz = sizeof(struct getratable_rsp);

	pgetrttblparm->rsvd = 0x0;

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
exit:
	_func_exit_;
	return res;

}

u8 proc_setassocsta_cmd_func(_wadptdata * pwadptdata, u8 * mac_addr)
{
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	struct cmd_obj *pwmbox1;
	struct set_assocsta_parm *psetassocsta_para;
	struct set_stakey_rsp *psetassocsta_rsp = NULL;

	u8 res = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	psetassocsta_para =
		(struct set_assocsta_parm *)
		wl_zmalloc(sizeof(struct set_assocsta_parm));
	if (psetassocsta_para == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	psetassocsta_rsp =
		(struct set_stakey_rsp *)wl_zmalloc(sizeof(struct set_assocsta_rsp));
	if (psetassocsta_rsp == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		wl_mfree((u8 *) psetassocsta_para, sizeof(struct set_assocsta_parm));
		return _FAIL;
	}

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, psetassocsta_para, _SetAssocSta_CMD_);
	pwmbox1->rsp = (u8 *) psetassocsta_rsp;
	pwmbox1->rspsz = sizeof(struct set_assocsta_rsp);

	Func_Of_Proc_Pre_Memcpy(psetassocsta_para->addr, mac_addr, ETH_ALEN);

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	_func_exit_;

	return res;
}

u8 proc_addbareq_cmd_func(_wadptdata * pwadptdata, u8 tid, u8 * addr, u8 flag)
{
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	struct cmd_obj *pwmbox1;
	struct addBaReq_parm *paddbareq_parm;

	u8 res = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	paddbareq_parm =
		(struct addBaReq_parm *)wl_zmalloc(sizeof(struct addBaReq_parm));
	if (paddbareq_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	if (flag) {
		paddbareq_parm->tid = tid;
		Func_Of_Proc_Pre_Memcpy(paddbareq_parm->addr, addr, ETH_ALEN);
	}
	init_h2fwcmd_w_parm_no_rsp(pwmbox1, paddbareq_parm, GEN_CMD_CODE(_AddBAReq));

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	_func_exit_;

	return res;
}

u8 proc_addbarsp_cmd_func(_wadptdata * pwadptdata, u8 * addr, u16 tid, u8 status, u8 size,
					u16 start_seq, u8 flag)
{
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	struct cmd_obj *pwmbox1;
	struct addBaRsp_parm *paddBaRsp_parm;
	u8 res = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	paddBaRsp_parm =
		(struct addBaRsp_parm *)wl_zmalloc(sizeof(struct addBaRsp_parm));

	if (paddBaRsp_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	if (flag) {
		Func_Of_Proc_Pre_Memcpy(paddBaRsp_parm->addr, addr, ETH_ALEN);
	}
	paddBaRsp_parm->tid = tid;
	paddBaRsp_parm->status = status;
	paddBaRsp_parm->size = size;
	paddBaRsp_parm->start_seq = start_seq;

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, paddBaRsp_parm, GEN_CMD_CODE(_AddBARsp));

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	_func_exit_;

	return res;
}

u8 proc_reset_securitypriv_cmd_func(_wadptdata * pwadptdata, u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = RESET_SECURITYPRIV;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;

	if (flag) {
		init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
								   GEN_CMD_CODE(_Set_Drv_Extra));
	}

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	_func_exit_;

	return res;

}

u8 proc_free_assoc_resources_cmd_func(_wadptdata * pwadptdata, u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = FREE_ASSOC_RESOURCES;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;

	if (flag) {
		init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
								   GEN_CMD_CODE(_Set_Drv_Extra));
	}

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	_func_exit_;

	return res;

}

u8 proc_dynamic_chk_wk_cmd_func(_wadptdata * pwadptdata, u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	if (flag) {
		pdrvextra_cmd_parm->ec_id = DYNAMIC_CHK_WK_CID;
		pdrvextra_cmd_parm->type = 0;
		pdrvextra_cmd_parm->size = 0;
		pdrvextra_cmd_parm->pbuf = NULL;
		init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
								   GEN_CMD_CODE(_Set_Drv_Extra));
	}

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	_func_exit_;

	return res;

}

u8 ch_cmd_to_set_func(_wadptdata * pwadptdata, u8 ch, u8 bw, u8 ch_offset, u8 enqueue,
				  u8 flag)
{
	struct cmd_obj *pcmdobj;
	struct set_ch_parm *set_ch_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;

	u8 res = _SUCCESS;

	_func_enter_;

	WL_INFO(FUNC_NDEV_FMT " ch:%u, bw:%u, ch_offset:%u\n",
			FUNC_NDEV_ARG(pwadptdata->pnetdev), ch, bw, ch_offset);

	set_ch_parm = (struct set_ch_parm *)wl_zmalloc(sizeof(*set_ch_parm));
	if (set_ch_parm == NULL) {
		res = _FAIL;
		goto exit;
	}
	set_ch_parm->ch = ch;
	set_ch_parm->bw = bw;
	set_ch_parm->ch_offset = ch_offset;

	if (enqueue) {
		pcmdobj = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
		if (pcmdobj == NULL) {
			wl_mfree((u8 *) set_ch_parm, sizeof(*set_ch_parm));
			res = _FAIL;
			goto exit;
		}

		if (flag) {
			init_h2fwcmd_w_parm_no_rsp(pcmdobj, set_ch_parm,
									   GEN_CMD_CODE(_SetChannel));
		}
		res = preproc_enqueue_cmd_func(pcmdpriv, pcmdobj);
	} else {
		if (wMBOX1_SUCCESS != proc_set_ch_func(pwadptdata, (u8 *) set_ch_parm))
			res = _FAIL;

		wl_mfree((u8 *) set_ch_parm, sizeof(*set_ch_parm));
	}

exit:

	WL_INFO(FUNC_NDEV_FMT " res:%u\n", FUNC_NDEV_ARG(pwadptdata->pnetdev), res);

	_func_exit_;

	return res;
}

u8 chplan_cmd_to_set_func(_wadptdata * wadptdata, int flags, u8 chplan,
					   const struct country_chplan * country_ent, u8 swconfig,
					   u8 flag)
{
	struct cmd_obj *cmdobj;
	struct SetChannelPlan_param *parm;
	struct cmd_priv *pcmdpriv = &wadptdata->cmdpriv;
	struct mlme_priv *pmlmepriv = &wadptdata->mlmepriv;
	struct submit_ctx sctx;
	u8 res = _SUCCESS;

	_func_enter_;

	if (swconfig && Func_Of_Proc_Chip_Hw_Is_Disable_Sw_Channel_Plan(wadptdata) == _TRUE) {
		res = _FAIL;
		goto exit;
	}

	if (country_ent)
		chplan = country_ent->chplan;

	if (!wl_is_channel_plan_valid(chplan)) {
		res = _FAIL;
		goto exit;
	}

	parm = (struct SetChannelPlan_param *)wl_zmalloc(sizeof(*parm));
	if (parm == NULL) {
		res = _FAIL;
		goto exit;
	}
	if (flag) {
		parm->country_ent = country_ent;
		parm->channel_plan = chplan;
	}
	if (flags & WL_CMDF_DIRECTLY) {
		if (wMBOX1_SUCCESS != proc_set_chplan_func(wadptdata, (u8 *) parm))
			res = _FAIL;
		wl_mfree((u8 *) parm, sizeof(*parm));
	} else {
		cmdobj = (struct cmd_obj *)wl_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			wl_mfree((u8 *) parm, sizeof(*parm));
			goto exit;
		}

		init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, GEN_CMD_CODE(_SetChannelPlan));

		if (flags & WL_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			do_init_sctx(&sctx, 2000, 1);
		}

		res = preproc_enqueue_cmd_func(pcmdpriv, cmdobj);

		if (res == _SUCCESS && (flags & WL_CMDF_WAIT_ACK)) {
			wl_sctx_wait(&sctx, __func__, 1);
			_enter_critical_mutex(&pcmdpriv->sctx_mutex, NULL);
			if (sctx.status == WL_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_exit_critical_mutex(&pcmdpriv->sctx_mutex, NULL);
		}
	}

exit:

	_func_exit_;

	return res;
}

inline u8 proc_set_chplan_cmd_func(_wadptdata * wadptdata, int flags, u8 chplan,
							 u8 swconfig)
{
	return chplan_cmd_to_set_func(wadptdata, flags, chplan, NULL, swconfig, 1);
}

inline u8 country_cmd_to_set_func(_wadptdata * wadptdata, int flags,
							  const char *country_code, u8 swconfig, u8 flag)
{
	const struct country_chplan *ent;

	if (flag) {
		if (Func_Is_Alpha(country_code[0]) == _FALSE
			|| Func_Is_Alpha(country_code[1]) == _FALSE) {
			WL_INFO_L1("%s input country_code is not alpha2\n", __func__);
			return _FAIL;
		}
	}
	ent = wl_query_chplan_from_list(country_code, 1);

	if (ent == NULL) {
		WL_INFO_L1("%s unsupported country_code:\"%c%c\"\n", __func__,
				   country_code[0], country_code[1]);
		return _FAIL;
	}

	WL_INFO_L1("%s country_code:\"%c%c\" mapping to chplan:0x%02x\n", __func__,
			   country_code[0], country_code[1], ent->chplan);

	return chplan_cmd_to_set_func(wadptdata, flags, WL_CHPLAN_MAX, ent, swconfig,
							   1);
}

u8 proc_led_blink_cmd_func(_wadptdata * pwadptdata, PVOID pLed)
{
	struct cmd_obj *pcmdobj;
	struct LedBlink_param *ledBlink_param;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;

	u8 res = _SUCCESS;

	_func_enter_;

	WL_INFO_L2("+proc_led_blink_cmd_func\n");

	pcmdobj = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pcmdobj == NULL) {
		res = _FAIL;
		goto exit;
	}

	ledBlink_param =
		(struct LedBlink_param *)wl_zmalloc(sizeof(struct LedBlink_param));
	if (ledBlink_param == NULL) {
		wl_mfree((u8 *) pcmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	ledBlink_param->pLed = pLed;

	init_h2fwcmd_w_parm_no_rsp(pcmdobj, ledBlink_param,
							   GEN_CMD_CODE(_LedBlink));
	res = preproc_enqueue_cmd_func(pcmdpriv, pcmdobj);

exit:

	_func_exit_;

	return res;
}

u8 csa_cmd_to_set_func(_wadptdata * pwadptdata, u8 new_ch_no)
{
	struct cmd_obj *pcmdobj;
	struct SetChannelSwitch_param *setChannelSwitch_param;
	struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;

	u8 res = _SUCCESS;

	_func_enter_;

	WL_INFO_L2("+csa_cmd_to_set_func\n");

	pcmdobj = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pcmdobj == NULL) {
		res = _FAIL;
		goto exit;
	}

	setChannelSwitch_param =
		(struct SetChannelSwitch_param *)
		wl_zmalloc(sizeof(struct SetChannelSwitch_param));
	if (setChannelSwitch_param == NULL) {
		wl_mfree((u8 *) pcmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	setChannelSwitch_param->new_ch_no = new_ch_no;

	init_h2fwcmd_w_parm_no_rsp(pcmdobj, setChannelSwitch_param,
							   GEN_CMD_CODE(_SetChannelSwitch));
	res = preproc_enqueue_cmd_func(pcmdpriv, pcmdobj);

exit:

	_func_exit_;

	return res;
}

u8 proc_tdls_cmd_func(_wadptdata * pwadptdata, const u8 * addr, u8 option)
{
	struct cmd_obj *pcmdobj;
	struct TDLSoption_param *TDLSoption;
	struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;

	u8 res = _SUCCESS;

	_func_enter_;

exit:

	_func_exit_;

	return res;
}

u8 update_enable_hw_tsf_cmd_func(_wadptdata * pwadptdata, u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	if (flag) {
		pdrvextra_cmd_parm->ec_id = EN_HW_UPDATE_TSF_WK_CID;
		pdrvextra_cmd_parm->type = 0;
		pdrvextra_cmd_parm->size = 0;
		pdrvextra_cmd_parm->pbuf = NULL;
	}
	init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
							   GEN_CMD_CODE(_Set_Drv_Extra));

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:
	return res;
}

static void traffic_statistics_collect_func(_wadptdata * pwadptdata, u8 flag)
{
	struct dvobj_priv *pdvobjpriv = wadptdata_to_dvobj(pwadptdata);
	_irqL irqL;

#ifdef CONFIG_CONCURRENT_MODE
	if (pwadptdata->wadptdata_type != PRIMARY_wADPTDATA)
		return;
#endif

	if (flag) {
		pdvobjpriv->traffic_stat.tx_bytes = pwadptdata->xmitpriv.tx_bytes;
		pdvobjpriv->traffic_stat.tx_pkts = pwadptdata->xmitpriv.tx_pkts;
		pdvobjpriv->traffic_stat.tx_drop = pwadptdata->xmitpriv.tx_drop;
	}
	pdvobjpriv->traffic_stat.rx_bytes = pwadptdata->recvpriv.rx_bytes;
	pdvobjpriv->traffic_stat.rx_pkts = pwadptdata->recvpriv.rx_pkts;
	pdvobjpriv->traffic_stat.rx_drop = pwadptdata->recvpriv.rx_drop;

#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(pwadptdata)) {
		pdvobjpriv->traffic_stat.tx_bytes +=
			pwadptdata->pbuddy_wadptdata->xmitpriv.tx_bytes;
		pdvobjpriv->traffic_stat.tx_pkts +=
			pwadptdata->pbuddy_wadptdata->xmitpriv.tx_pkts;
		pdvobjpriv->traffic_stat.tx_drop +=
			pwadptdata->pbuddy_wadptdata->xmitpriv.tx_drop;

		pdvobjpriv->traffic_stat.rx_bytes +=
			pwadptdata->pbuddy_wadptdata->recvpriv.rx_bytes;
		pdvobjpriv->traffic_stat.rx_pkts +=
			pwadptdata->pbuddy_wadptdata->recvpriv.rx_pkts;
		pdvobjpriv->traffic_stat.rx_drop +=
			pwadptdata->pbuddy_wadptdata->recvpriv.rx_drop;
	}
#endif

	pdvobjpriv->traffic_stat.cur_tx_bytes =
		pdvobjpriv->traffic_stat.tx_bytes -
		pdvobjpriv->traffic_stat.last_tx_bytes;
	pdvobjpriv->traffic_stat.cur_rx_bytes =
		pdvobjpriv->traffic_stat.rx_bytes -
		pdvobjpriv->traffic_stat.last_rx_bytes;
	pdvobjpriv->traffic_stat.last_tx_bytes = pdvobjpriv->traffic_stat.tx_bytes;
	pdvobjpriv->traffic_stat.last_rx_bytes = pdvobjpriv->traffic_stat.rx_bytes;

	pdvobjpriv->traffic_stat.cur_tx_tp =
		(u32) (pdvobjpriv->traffic_stat.cur_tx_bytes * 8 / 2 / 1024 / 1024);
	pdvobjpriv->traffic_stat.cur_rx_tp =
		(u32) (pdvobjpriv->traffic_stat.cur_rx_bytes * 8 / 2 / 1024 / 1024);

	msg_rw_val.traffic_stat_cur_tx_tp = pdvobjpriv->traffic_stat.cur_tx_tp;
	msg_rw_val.traffic_stat_cur_rx_tp = pdvobjpriv->traffic_stat.cur_rx_tp;

}

u8 watchdog_traffic_status_func(_wadptdata * pwadptdata, u8 from_timer, u8 flag)
{
	u8 bEnterPS = _FALSE;
	u16 BusyThresholdHigh;
	u16 BusyThresholdLow;
	u16 BusyThreshold;
	u8 bBusyTraffic = _FALSE, bTxBusyTraffic = _FALSE, bRxBusyTraffic = _FALSE;
	u8 bHigherBusyTraffic = _FALSE, bHigherBusyRxTraffic =
		_FALSE, bHigherBusyTxTraffic = _FALSE;

	struct mlme_priv *pmlmepriv = &(pwadptdata->mlmepriv);

	WP_LINK_DETECT_T *link_detect = &pmlmepriv->LinkDetectInfo;
	{
		BusyThresholdHigh = 100;
		BusyThresholdLow = 75;
	}
	BusyThreshold = BusyThresholdHigh;

	if (flag) {
		traffic_statistics_collect_func(pwadptdata, 1);
	}
	if ((check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE)
		) {
		if (pmlmepriv->LinkDetectInfo.bBusyTraffic)
			BusyThreshold = BusyThresholdLow;

		if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod > BusyThreshold ||
			pmlmepriv->LinkDetectInfo.NumTxOkInPeriod > BusyThreshold) {
			bBusyTraffic = _TRUE;

			if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod >
				pmlmepriv->LinkDetectInfo.NumTxOkInPeriod)
				bRxBusyTraffic = _TRUE;
			else
				bTxBusyTraffic = _TRUE;
		}

		if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod > 4000 ||
			pmlmepriv->LinkDetectInfo.NumTxOkInPeriod > 4000) {
			bHigherBusyTraffic = _TRUE;

			if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod >
				pmlmepriv->LinkDetectInfo.NumTxOkInPeriod)
				bHigherBusyRxTraffic = _TRUE;
			else
				bHigherBusyTxTraffic = _TRUE;
		}
#ifdef CONFIG_TRAFFIC_PROTECT
#define TX_ACTIVE_TH 10
#define RX_ACTIVE_TH 20
#define TRAFFIC_PROTECT_PERIOD_MS 4500

		if (link_detect->NumTxOkInPeriod > TX_ACTIVE_TH
			|| link_detect->NumRxUnicastOkInPeriod > RX_ACTIVE_TH) {

			WL_INFO_L1(FUNC_ADPT_FMT
					   " acqiure wake_lock for %u ms(tx:%d,rx_unicast:%d)\n",
					   FUNC_ADPT_ARG(pwadptdata), TRAFFIC_PROTECT_PERIOD_MS,
					   link_detect->NumTxOkInPeriod,
					   link_detect->NumRxUnicastOkInPeriod);

			Func_Of_Proc_Lock_Traffic_Suspend_Timeout(TRAFFIC_PROTECT_PERIOD_MS);
		}
#endif

#ifdef CONFIG_LPS
		if (((pmlmepriv->LinkDetectInfo.NumRxUnicastOkInPeriod +
			  pmlmepriv->LinkDetectInfo.NumTxOkInPeriod) > 8) ||
#ifdef CONFIG_LPS_SLOW_TRANSITION
			(pmlmepriv->LinkDetectInfo.NumRxUnicastOkInPeriod > 2)
#else
			(pmlmepriv->LinkDetectInfo.NumRxUnicastOkInPeriod > 4)
#endif
			) {
#ifdef DBG_RX_COUNTER_DUMP
			if (pwadptdata->dump_rx_cnt_mode & DUMP_DRV_TRX_COUNTER_DATA)
				WL_INFO("(-)Tx = %d, Rx = %d \n",
						pmlmepriv->LinkDetectInfo.NumTxOkInPeriod,
						pmlmepriv->LinkDetectInfo.NumRxUnicastOkInPeriod);
#endif
			bEnterPS = _FALSE;
#ifdef CONFIG_LPS_SLOW_TRANSITION
			if (bBusyTraffic == _TRUE) {
				if (pmlmepriv->LinkDetectInfo.TrafficTransitionCount <= 4)
					pmlmepriv->LinkDetectInfo.TrafficTransitionCount = 4;

				pmlmepriv->LinkDetectInfo.TrafficTransitionCount++;

				if (pmlmepriv->LinkDetectInfo.TrafficTransitionCount > 30) {
					pmlmepriv->LinkDetectInfo.TrafficTransitionCount = 30;
				}
			}
#endif

		} else {
#ifdef DBG_RX_COUNTER_DUMP
			if (pwadptdata->dump_rx_cnt_mode & DUMP_DRV_TRX_COUNTER_DATA)
				WL_INFO("(+)Tx = %d, Rx = %d \n",
						pmlmepriv->LinkDetectInfo.NumTxOkInPeriod,
						pmlmepriv->LinkDetectInfo.NumRxUnicastOkInPeriod);
#endif
#ifdef CONFIG_LPS_SLOW_TRANSITION
			if (pmlmepriv->LinkDetectInfo.TrafficTransitionCount >= 2)
				pmlmepriv->LinkDetectInfo.TrafficTransitionCount -= 2;
			else
				pmlmepriv->LinkDetectInfo.TrafficTransitionCount = 0;

			if (pmlmepriv->LinkDetectInfo.TrafficTransitionCount == 0)
				bEnterPS = _TRUE;
#else
			bEnterPS = _TRUE;
#endif
		}

#ifdef CONFIG_DYNAMIC_DTIM
		if (pmlmepriv->LinkDetectInfo.LowPowerTransitionCount == 8)
			bEnterPS = _FALSE;

		WL_INFO("LowPowerTransitionCount=%d\n",
				pmlmepriv->LinkDetectInfo.LowPowerTransitionCount);
#endif

		if (bEnterPS) {
			if (!from_timer) {
#ifdef CONFIG_DYNAMIC_DTIM
				if (pmlmepriv->LinkDetectInfo.LowPowerTransitionCount < 8) {
					wadptdata_to_pwrctl(pwadptdata)->dtim = 1;
				} else {
					wadptdata_to_pwrctl(pwadptdata)->dtim = 3;
				}
#endif
				do_in_LPS(pwadptdata, "TRAFFIC_IDLE", 1);
			} else {
			}
#ifdef CONFIG_DYNAMIC_DTIM
			if (wadptdata_to_pwrctl(pwadptdata)->bFwCurrentInPSMode == _TRUE)
				pmlmepriv->LinkDetectInfo.LowPowerTransitionCount++;
#endif
		} else {
#ifdef CONFIG_DYNAMIC_DTIM
			if (pmlmepriv->LinkDetectInfo.LowPowerTransitionCount != 8)
				pmlmepriv->LinkDetectInfo.LowPowerTransitionCount = 0;
			else
				pmlmepriv->LinkDetectInfo.LowPowerTransitionCount++;
#endif
			if (!from_timer) {
				do_out_LPS(pwadptdata, "TRAFFIC_BUSY", 1);
			} else {
#ifdef CONFIG_CONCURRENT_MODE
				if (pwadptdata->iface_type == IFACE_PORT0)
#endif
					proc_lps_ctrl_wk_cmd_func(pwadptdata, LPS_CTRL_TRAFFIC_BUSY, 1);
			}
		}

#endif
	} else {
#ifdef CONFIG_LPS
		struct dvobj_priv *dvobj = wadptdata_to_dvobj(pwadptdata);
		int n_assoc_iface = 0;
		int i;

		for (i = 0; i < dvobj->iface_nums; i++) {
			if (check_fwstate
				(&(dvobj->pwadptdatas[i]->mlmepriv), WIFI_ASOC_STATE))
				n_assoc_iface++;
		}

		if (!from_timer && n_assoc_iface == 0)
			do_out_LPS(pwadptdata, "NON_LINKED", 1);
#endif
	}

	chk_session_tracker_cmd_func(pwadptdata, NULL);

	pmlmepriv->LinkDetectInfo.NumRxOkInPeriod = 0;
	pmlmepriv->LinkDetectInfo.NumTxOkInPeriod = 0;
	pmlmepriv->LinkDetectInfo.NumRxUnicastOkInPeriod = 0;
	pmlmepriv->LinkDetectInfo.bBusyTraffic = bBusyTraffic;
	pmlmepriv->LinkDetectInfo.bTxBusyTraffic = bTxBusyTraffic;
	pmlmepriv->LinkDetectInfo.bRxBusyTraffic = bRxBusyTraffic;
	pmlmepriv->LinkDetectInfo.bHigherBusyTraffic = bHigherBusyTraffic;
	pmlmepriv->LinkDetectInfo.bHigherBusyRxTraffic = bHigherBusyRxTraffic;
	pmlmepriv->LinkDetectInfo.bHigherBusyTxTraffic = bHigherBusyTxTraffic;

	return bEnterPS;

}

static void proc_dynamic_chk_wk_func(_wadptdata * pwadptdata, u8 flag)
{
	struct mlme_priv *pmlmepriv;
	pmlmepriv = &(pwadptdata->mlmepriv);

#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
#ifdef CONFIG_AP_MODE
	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == _TRUE) {
		chk_expire_timeout_func(pwadptdata, 1);
	}
#endif
#endif

	if (flag) {
		dump_linked_info_func(pwadptdata);
	}

	{
		check_linked_status_func(pwadptdata, 0, 1);
		watchdog_traffic_status_func(pwadptdata, 0, 1);
#ifdef DBG_RX_COUNTER_DUMP
		Func_Of_Proc_Dump_Rx_Counters(pwadptdata);
#endif
		Func_Dm_Dynamicusbtxagg(pwadptdata, 0);
	}

	Func_Of_Proc_Chip_Hw_Dm_Watchdog(pwadptdata);

}

#ifdef CONFIG_LPS

void proc_lps_ctrl_wk_func(_wadptdata * pwadptdata, u8 lps_ctrl_type);
void proc_lps_ctrl_wk_func(_wadptdata * pwadptdata, u8 lps_ctrl_type)
{
	struct pwrctrl_priv *pwrpriv = wadptdata_to_pwrctl(pwadptdata);
	struct mlme_priv *pmlmepriv = &(pwadptdata->mlmepriv);
	u8 mstatus;

	_func_enter_;

	if ((check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE)
		|| (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE)) {
		return;
	}

	switch (lps_ctrl_type) {
	case LPS_CTRL_SCAN:
		if (check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE) {
			do_out_LPS(pwadptdata, "LPS_CTRL_SCAN", 1);
		}
		break;
	case LPS_CTRL_JOINBSS:
		do_out_LPS(pwadptdata, "LPS_CTRL_JOINBSS", 1);
		break;
	case LPS_CTRL_CONNECT:
		mstatus = 1;
		pwrpriv->LpsIdleCount = 0;
		Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_wMBOX1_FW_JOINBSSRPT,
						  (u8 *) (&mstatus));
		break;
	case LPS_CTRL_DISCONNECT:
		mstatus = 0;
		do_out_LPS(pwadptdata, "LPS_CTRL_DISCONNECT", 1);
		Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_wMBOX1_FW_JOINBSSRPT,
						  (u8 *) (&mstatus));
		break;
	case LPS_CTRL_SPECIAL_PACKET:
		pwrpriv->DelayLPSLastTimeStamp = Func_Of_Proc_Get_Current_Time();
		do_out_LPS(pwadptdata, "LPS_CTRL_SPECIAL_PACKET", 1);
		break;
	case LPS_CTRL_LEAVE:
		do_out_LPS(pwadptdata, "LPS_CTRL_LEAVE", 1);
		break;
	case LPS_CTRL_LEAVE_CFG80211_PWRMGMT:
		do_out_LPS(pwadptdata, "CFG80211_PWRMGMT", 1);
		break;
	case LPS_CTRL_TRAFFIC_BUSY:
		do_out_LPS(pwadptdata, "LPS_CTRL_TRAFFIC_BUSY", 1);
		break;
	case LPS_CTRL_TX_TRAFFIC_LEAVE:
		do_out_LPS(pwadptdata, "LPS_CTRL_TX_TRAFFIC_LEAVE", 1);
		break;
	case LPS_CTRL_RX_TRAFFIC_LEAVE:
		do_out_LPS(pwadptdata, "LPS_CTRL_RX_TRAFFIC_LEAVE", 1);
		break;
	case LPS_CTRL_ENTER:
		do_in_LPS(pwadptdata, "TRAFFIC_IDLE_1", 1);
		break;
	default:
		break;
	}

	_func_exit_;
}

u8 proc_lps_ctrl_wk_cmd_func(_wadptdata * pwadptdata, u8 lps_ctrl_type, u8 enqueue)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;

	if (enqueue) {
		pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
		if (pwmbox1 == NULL) {
			res = _FAIL;
			goto exit;
		}

		pdrvextra_cmd_parm =
			(struct drvextra_cmd_parm *)
			wl_zmalloc(sizeof(struct drvextra_cmd_parm));
		if (pdrvextra_cmd_parm == NULL) {
			wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
			res = _FAIL;
			goto exit;
		}

		pdrvextra_cmd_parm->ec_id = LPS_CTRL_WK_CID;
		pdrvextra_cmd_parm->type = lps_ctrl_type;
		pdrvextra_cmd_parm->size = 0;
		pdrvextra_cmd_parm->pbuf = NULL;

		init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
								   GEN_CMD_CODE(_Set_Drv_Extra));

		res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
	} else {
		proc_lps_ctrl_wk_func(pwadptdata, lps_ctrl_type);
	}

exit:

	_func_exit_;

	return res;

}

static void proc_dm_in_lps_func(_wadptdata * pwadptdata)
{
	Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_DM_IN_LPS, NULL);
}

u8 proc_dm_in_lps_wk_cmd_func(_wadptdata * pwadptdata)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = DM_IN_LPS_WK_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
							   GEN_CMD_CODE(_Set_Drv_Extra));

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	return res;

}

static void proc_lps_change_dtim_func(_wadptdata * pwadptdata, u8 dtim, u8 flag)
{
	struct pwrctrl_priv *pwrpriv = wadptdata_to_pwrctl(pwadptdata);

	if (flag) {
		if (dtim <= 0 || dtim > 16)
			return;
	}
	if (pwrpriv->dtim != dtim) {
		WL_INFO
			("change DTIM from %d to %d, bFwCurrentInPSMode=%d, ps_mode=%d\n",
			 pwrpriv->dtim, dtim, pwrpriv->bFwCurrentInPSMode,
			 pwrpriv->pwr_mode);

		pwrpriv->dtim = dtim;
	}

	if ((pwrpriv->bFwCurrentInPSMode == _TRUE)
		&& (pwrpriv->pwr_mode > PS_MODE_ACTIVE)) {
		u8 ps_mode = pwrpriv->pwr_mode;

		Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_wMBOX1_FW_PWRMODE, (u8 *) (&ps_mode));
	}
}

#endif

u8 proc_lps_change_dtim_cmd_func(_wadptdata * pwadptdata, u8 dtim, u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;
	{
		pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
		if (pwmbox1 == NULL) {
			res = _FAIL;
			goto exit;
		}

		pdrvextra_cmd_parm =
			(struct drvextra_cmd_parm *)
			wl_zmalloc(sizeof(struct drvextra_cmd_parm));
		if (pdrvextra_cmd_parm == NULL) {
			wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
			res = _FAIL;
			goto exit;
		}

		pdrvextra_cmd_parm->ec_id = LPS_CHANGE_DTIM_CID;
		pdrvextra_cmd_parm->type = dtim;
		pdrvextra_cmd_parm->size = 0;
		pdrvextra_cmd_parm->pbuf = NULL;

		if (flag) {
			init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
									   GEN_CMD_CODE(_Set_Drv_Extra));
		}
		res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
	}

exit:

	return res;

}

#if (RATE_ADAPTIVE_SUPPORT==1)
static void proc_report_timer_setting_wk_func(_wadptdata * pwadptdata, u16 minRptTime)
{
	Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_RPT_TIMER_SETTING, (u8 *) (&minRptTime));
}

u8 proc_rpt_timer_cfg_cmd_func(_wadptdata * pwadptdata, u16 minRptTime)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;

	u8 res = _SUCCESS;

	_func_enter_;
	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = RTP_TIMER_CFG_WK_CID;
	pdrvextra_cmd_parm->type = minRptTime;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;
	init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
							   GEN_CMD_CODE(_Set_Drv_Extra));
	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
exit:

	_func_exit_;

	return res;

}

#endif

static void proc_dm_ra_mask_func(_wadptdata * pwadptdata, struct sta_info *psta, u8 flag)
{
	if (flag) {
		if (psta) {
			Func_Of_Proc_Chip_Hw_Update_Ra_Mask(psta, psta->rssi_level);
		}
	}
}

u8 proc_dm_ra_mask_wk_cmd_func(_wadptdata * pwadptdata, u8 * psta, u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = DM_RA_MSK_WK_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = psta;

	if (flag) {
		init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
								   GEN_CMD_CODE(_Set_Drv_Extra));
	}
	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	return res;

}

static void proc_power_saving_wk_func(_wadptdata * pwadptdata)
{
	do_ps_work(pwadptdata, 1);
}

static void proc_reset_securitypriv_func(_wadptdata * pwadptdata)
{
	do_reset_securitypriv(pwadptdata, 1);
}

static void proc_free_assoc_resources_func(_wadptdata * pwadptdata)
{
	do_assoc_resources_unnew(pwadptdata, 1, 1);
}

#ifdef CONFIG_P2P
u8 protocol_p2p_wk_cmd_func(_wadptdata * pwadptdata, int intCmdType)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct wifidirect_info *pwdinfo = &(pwadptdata->wdinfo);
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	_func_enter_;

	if (wl_p2p_chk_state(pwdinfo, P2P_STATE_NONE)) {
		return res;
	}

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = P2P_PROTO_WK_CID;
	pdrvextra_cmd_parm->type = intCmdType;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;

	init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
							   GEN_CMD_CODE(_Set_Drv_Extra));

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	_func_exit_;

	return res;

}
#endif

u8 proc_ps_cmd_func(_wadptdata * pwadptdata, u8 flag)
{
	struct cmd_obj *ppscmd;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;

	u8 res = _SUCCESS;
	_func_enter_;

#ifdef CONFIG_CONCURRENT_MODE
	if (pwadptdata->wadptdata_type != PRIMARY_wADPTDATA)
		goto exit;
#endif

	ppscmd = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (ppscmd == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((unsigned char *)ppscmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	if (flag) {
		pdrvextra_cmd_parm->ec_id = POWER_SAVING_CTRL_WK_CID;
		pdrvextra_cmd_parm->type = 0;
		pdrvextra_cmd_parm->size = 0;
		pdrvextra_cmd_parm->pbuf = NULL;
		init_h2fwcmd_w_parm_no_rsp(ppscmd, pdrvextra_cmd_parm,
								   GEN_CMD_CODE(_Set_Drv_Extra));
	}
	res = preproc_enqueue_cmd_func(pcmdpriv, ppscmd);

exit:

	_func_exit_;

	return res;

}

#ifdef CONFIG_AP_MODE

static void proc_chk_hi_queue_func(_wadptdata * pwadptdata, u8 flag)
{
	struct sta_info *psta_bmc;
	struct sta_priv *pstapriv = &pwadptdata->stapriv;
	u32 start = Func_Of_Proc_Get_Current_Time();
	u8 empty = _FALSE;

	psta_bmc = do_query_bcmc_stainfo(pwadptdata, 1);
	if (!psta_bmc)
		return;

	if (flag) {
		Func_Of_Proc_Chip_Hw_Get_Hwreg(pwadptdata, HW_VAR_CHK_HI_QUEUE_EMPTY, &empty);
	}
	while (_FALSE == empty
		   && Func_Of_Proc_Get_Passing_Time_Ms(start) < wl_get_wait_hiq_empty_ms()) {
		Func_Of_Proc_Msleep_Os(100);
		Func_Of_Proc_Chip_Hw_Get_Hwreg(pwadptdata, HW_VAR_CHK_HI_QUEUE_EMPTY, &empty);
	}

	if (psta_bmc->sleepq_len == 0) {
		if (empty == _SUCCESS) {
			bool update_tim = _FALSE;

			if (pstapriv->tim_bitmap & BIT(0))
				update_tim = _TRUE;

			pstapriv->tim_bitmap &= ~BIT(0);
			pstapriv->sta_dz_bitmap &= ~BIT(0);

			if (update_tim == _TRUE)
				beacon_update_of_ap_func(pwadptdata, _TIM_IE_, NULL, _TRUE,
							   "bmc sleepq and HIQ empty", 1);
		} else {
			proc_chk_hi_queue_cmd_func(pwadptdata, 1);
		}

	}

}

u8 proc_chk_hi_queue_cmd_func(_wadptdata * pwadptdata, u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((unsigned char *)pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = CHECK_HIQ_WK_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;

	if (flag) {
		init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
								   GEN_CMD_CODE(_Set_Drv_Extra));
	}
	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	return res;

}

#endif

u8 proc_wmbox0_packet_wk_cmd_func(PwADPTDATA pwadptdata, u8 * pbuf, u16 length, u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 *extra_cmd_buf;
	u8 res = _SUCCESS;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	extra_cmd_buf = wl_zmalloc(length);
	if (extra_cmd_buf == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		wl_mfree((u8 *) pdrvextra_cmd_parm, sizeof(struct drvextra_cmd_parm));
		res = _FAIL;
		goto exit;
	}

	Func_Of_Proc_Pre_Memcpy(extra_cmd_buf, pbuf, length);
	pdrvextra_cmd_parm->ec_id = wMBOX0_WK_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = length;
	pdrvextra_cmd_parm->pbuf = extra_cmd_buf;

	if (flag) {
		init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
								   GEN_CMD_CODE(_Set_Drv_Extra));
	}
	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:
	return res;
}

static s32 proc_mp_cmd_hdl_func(_wadptdata * pwadptdata, u8 mp_cmd_id)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(pwadptdata);
	int ret = wMBOX1_SUCCESS;
	u8 rfreg0;
	u32 inbuff = 0;

	if (mp_cmd_id == MP_START) {
		#ifdef SimpleTest
			Func_Of_Proc_Chip_Hw_Deinit(pwadptdata);
		#endif
		if (pwadptdata->registrypriv.mp_mode == 0) {
			pwadptdata->registrypriv.mp_mode = 1;
			Func_Of_Proc_Chip_Hw_Deinit(pwadptdata);
			WL_INFO_L1("mp_mode = %d\n", pwadptdata->registrypriv.mp_mode);
		#ifdef CONFIG_RF_POWER_TRIM
			pwadptdata->registrypriv.RegPwrTrimEnable = 1;
			Func_Of_Proc_Chip_Hw_Read_Chip_Info(pwadptdata);
		#endif
			Func_Of_Proc_Chip_Hw_Init(pwadptdata);
		}

		if (pwadptdata->registrypriv.mp_mode == 0) {
			ret = wMBOX1_REJECTED;
			goto exit;
		}

		if (pwadptdata->mppriv.mode == MP_OFF) {
			if (mp_start_test(pwadptdata) == _FAIL) {
				ret = wMBOX1_REJECTED;
				goto exit;
			}
			pwadptdata->mppriv.mode = MP_ON;
			MPT_PwrCtlDM(pwadptdata, 0);
		}
		pwadptdata->mppriv.bmac_filter = _FALSE;
#ifdef SETNEW
		inbuff = 0x26;
#else
		inbuff = 0x20;
#endif
		if (!Func_Mcu_Universal_Func_Interface
			(pwadptdata, UMSG_OPS_MSG_WRITE_DIG, &inbuff, NULL, 1, 0)) {
			WL_INFO("Error :===>%s,fail,code :UMSG_OPS_MSG_WRITE_DIG!!!\n",
					__func__);
			ret = wMBOX1_REJECTED;
			goto exit;
		}

	} else if (mp_cmd_id == MP_STOP) {
		if (pwadptdata->registrypriv.mp_mode == 1) {
			MPT_DeInitAdapter(pwadptdata);
			Func_Of_Proc_Chip_Hw_Deinit(pwadptdata);
			pwadptdata->registrypriv.mp_mode = 0;
			Func_Of_Proc_Chip_Hw_Init(pwadptdata);
		}

		if (pwadptdata->mppriv.mode != MP_OFF) {
			mp_stop_test(pwadptdata);
			pwadptdata->mppriv.mode = MP_OFF;
		}

	} else {
		WL_INFO(FUNC_ADPT_FMT "invalid id:%d\n", FUNC_ADPT_ARG(pwadptdata),
				mp_cmd_id);
		ret = wMBOX1_PARAMETERS_ERROR;
		wl_warn_on(1);
	}

exit:
	return ret;
}

u8 proc_mp_cmd_func(_wadptdata * wadptdata, u8 mp_cmd_id, u8 flags)
{
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *parm;
	struct cmd_priv *pcmdpriv = &wadptdata->cmdpriv;
	struct submit_ctx sctx;
	u8 res = _SUCCESS;

	parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (parm == NULL) {
		res = _FAIL;
		goto exit;
	}

	parm->ec_id = MP_CMD_WK_CID;
	parm->type = mp_cmd_id;
	parm->size = 0;
	parm->pbuf = NULL;

	if (flags & WL_CMDF_DIRECTLY) {
		if (wMBOX1_SUCCESS != proc_mp_cmd_hdl_func(wadptdata, mp_cmd_id))
			res = _FAIL;
		wl_mfree((u8 *) parm, sizeof(*parm));
	} else {
		cmdobj = (struct cmd_obj *)wl_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			wl_mfree((u8 *) parm, sizeof(*parm));
			goto exit;
		}

		init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, GEN_CMD_CODE(_Set_Drv_Extra));

		if (flags & WL_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			do_init_sctx(&sctx, 10 * 1000, 1);
		}

		res = preproc_enqueue_cmd_func(pcmdpriv, cmdobj);

		if (res == _SUCCESS && (flags & WL_CMDF_WAIT_ACK)) {
			wl_sctx_wait(&sctx, __func__, 1);
			_enter_critical_mutex(&pcmdpriv->sctx_mutex, NULL);
			if (sctx.status == WL_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_exit_critical_mutex(&pcmdpriv->sctx_mutex, NULL);
			if (sctx.status != WL_SCTX_DONE_SUCCESS)
				res = _FAIL;
		}
	}

exit:
	return res;
}

u8 proc_wmbox0_wk_cmd_func(PwADPTDATA pwadptdata, u8 * wmbox0_evt, u8 flag)
{
	struct cmd_obj *pwmbox1;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &pwadptdata->cmdpriv;
	u8 res = _SUCCESS;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (pwmbox1 == NULL) {
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = wMBOX0_WK_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = wmbox0_evt ? 16 : 0;
	pdrvextra_cmd_parm->pbuf = wmbox0_evt;

	if (flag) {
		init_h2fwcmd_w_parm_no_rsp(pwmbox1, pdrvextra_cmd_parm,
								   GEN_CMD_CODE(_Set_Drv_Extra));
	}
	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);

exit:

	return res;
}

u8 proc_run_in_thread_cmd_func(PwADPTDATA pwadptdata, void (*func) (void *),
						 void *context)
{
	struct cmd_priv *pcmdpriv;
	struct cmd_obj *pwmbox1;
	struct RunInThread_param *parm;
	s32 res = _SUCCESS;

	_func_enter_;

	pcmdpriv = &pwadptdata->cmdpriv;

	pwmbox1 = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (NULL == pwmbox1) {
		res = _FAIL;
		goto exit;
	}

	parm =
		(struct RunInThread_param *)
		wl_zmalloc(sizeof(struct RunInThread_param));
	if (NULL == parm) {
		wl_mfree((u8 *) pwmbox1, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	parm->func = func;
	parm->context = context;
	init_h2fwcmd_w_parm_no_rsp(pwmbox1, parm, GEN_CMD_CODE(_RunInThreadCMD));

	res = preproc_enqueue_cmd_func(pcmdpriv, pwmbox1);
exit:

	_func_exit_;

	return res;
}

s32 proc_wmbox0_evt_func(_wadptdata * wadptdata, u8 * wmbox0_evt)
{
	s32 ret = _FAIL;
	u8 buf[16];

	if (!wmbox0_evt) {
		if (Func_Of_Proc_Chip_Hw_wMBOX0_Evt_Read(wadptdata, buf) == _SUCCESS) {
			wmbox0_evt = buf;

			if (Func_Of_Proc_Chip_Hw_wMBOX0_Id_Filter_Ccx(wmbox0_evt) == _FALSE)
				goto exit;

			ret = Func_Of_Proc_Chip_Hw_wMBOX0_Handler(wadptdata, wmbox0_evt);
		}
	} else {

		if (Func_Of_Proc_Chip_Hw_wMBOX0_Id_Filter_Ccx(wmbox0_evt) == _FALSE)
			goto exit;

		ret = Func_Of_Proc_Chip_Hw_wMBOX0_Handler(wadptdata, wmbox0_evt);
	}
exit:
	return ret;
}

#ifdef CONFIG_wMBOX0_WK
static void callback_wmbox0_wk_func(_workitem * work)
{
	struct evt_priv *evtpriv = container_of(work, struct evt_priv, wmbox0_wk);
	_wadptdata *wadptdata = container_of(evtpriv, _wadptdata, evtpriv);
	u8 *wmbox0_evt;

	evtpriv->wmbox0_wk_alive = _TRUE;

	while (!Func_Of_Proc_Cbuf_Empty(evtpriv->wmbox0_queue)) {
		if ((wmbox0_evt = (u8 *) Func_Of_Proc_Cbuf_Pop(evtpriv->wmbox0_queue)) != NULL) {
			Func_Dev_To_Host_Evt_Clear(wadptdata);
		} else if ((wmbox0_evt = (u8 *) wl_malloc(16)) != NULL) {
			if (Func_Of_Proc_Chip_Hw_wMBOX0_Evt_Read(wadptdata, wmbox0_evt) != _SUCCESS) {
				wl_mfree(wmbox0_evt, 16);
				continue;
			}
		} else {
			wl_warn_on(1);
			continue;
		}

		if ((void *)wmbox0_evt == (void *)evtpriv)
			continue;

		if (!Func_Of_Proc_Chip_Hw_wMBOX0_Valid(wadptdata, wmbox0_evt)) {
			wl_mfree(wmbox0_evt, 16);
			continue;
		}

		if (Func_Of_Proc_Chip_Hw_wMBOX0_Id_Filter_Ccx(wmbox0_evt) == _TRUE) {
			Func_Of_Proc_Chip_Hw_wMBOX0_Handler(wadptdata, wmbox0_evt);
			wl_mfree(wmbox0_evt, 16);
		} else {
			proc_wmbox0_wk_cmd_func(wadptdata, wmbox0_evt, 1);
		}
	}

	evtpriv->wmbox0_wk_alive = _FALSE;
}
#endif

static u8 proc_session_tracker_cmd_func(u8 flag, _wadptdata * wadptdata, u8 cmd,
					   struct sta_info *sta, u8 * local_naddr, u8 * local_port,
					   u8 * remote_naddr, u8 * remote_port)
{
	struct cmd_priv *cmdpriv = &wadptdata->cmdpriv;
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *cmd_parm;
	struct st_cmd_parm *st_parm;
	u8 res = _SUCCESS;

	cmdobj = (struct cmd_obj *)wl_zmalloc(sizeof(struct cmd_obj));
	if (cmdobj == NULL) {
		res = _FAIL;
		goto exit;
	}

	cmd_parm =
		(struct drvextra_cmd_parm *)
		wl_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (cmd_parm == NULL) {
		wl_mfree((u8 *) cmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	st_parm = (struct st_cmd_parm *)wl_zmalloc(sizeof(struct st_cmd_parm));
	if (st_parm == NULL) {
		wl_mfree((u8 *) cmdobj, sizeof(struct cmd_obj));
		wl_mfree((u8 *) cmd_parm, sizeof(struct drvextra_cmd_parm));
		res = _FAIL;
		goto exit;
	}

	if (flag) {
		st_parm->cmd = cmd;
		st_parm->sta = sta;
		if (cmd != ST_CMD_CHK) {
			Func_Of_Proc_Pre_Memcpy(&st_parm->local_naddr, local_naddr, 4);
			Func_Of_Proc_Pre_Memcpy(&st_parm->local_port, local_port, 2);
			Func_Of_Proc_Pre_Memcpy(&st_parm->remote_naddr, remote_naddr, 4);
			Func_Of_Proc_Pre_Memcpy(&st_parm->remote_port, remote_port, 2);
		}
	}
	cmd_parm->ec_id = SESSION_TRACKER_WK_CID;
	cmd_parm->type = 0;
	cmd_parm->size = sizeof(struct st_cmd_parm);
	cmd_parm->pbuf = (u8 *) st_parm;
	init_h2fwcmd_w_parm_no_rsp(cmdobj, cmd_parm, GEN_CMD_CODE(_Set_Drv_Extra));
	cmdobj->no_io = 1;

	res = preproc_enqueue_cmd_func(cmdpriv, cmdobj);

exit:
	return res;
}

inline u8 chk_session_tracker_cmd_func(_wadptdata * wadptdata, struct sta_info * sta)
{
	return proc_session_tracker_cmd_func(1, wadptdata, ST_CMD_CHK, sta, NULL, NULL, NULL,
							   NULL);
}

inline u8 add_session_tracker_cmd_func(_wadptdata * wadptdata, struct sta_info * sta,
								  u8 * local_naddr, u8 * local_port,
								  u8 * remote_naddr, u8 * remote_port)
{
	return proc_session_tracker_cmd_func(1, wadptdata, ST_CMD_ADD, sta, local_naddr,
							   local_port, remote_naddr, remote_port);
}

inline u8 del_session_tracker_cmd_func(_wadptdata * wadptdata, struct sta_info * sta,
								  u8 * local_naddr, u8 * local_port,
								  u8 * remote_naddr, u8 * remote_port)
{
	return proc_session_tracker_cmd_func(1, wadptdata, ST_CMD_DEL, sta, local_naddr,
							   local_port, remote_naddr, remote_port);
}

void chk_for_session_tracker_sta_func(_wadptdata * wadptdata, struct sta_info *sta,
								 u8 flag)
{
	struct st_ctl_t *st_ctl = &sta->st_ctl;
	int i;
	_irqL irqL;
	_list *plist, *phead, *pnext;
	_list dlist;
	struct session_tracker *st = NULL;
	u8 op_wfd_mode = MIRACAST_DISABLED;

	if (!(sta->state & _FW_LINKED))
		goto exit;

	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++) {
		if (st_ctl->reg[i].s_proto != 0)
			break;
	}
	if (i >= SESSION_TRACKER_REG_ID_NUM)
		goto chk_sta;

	if (flag) {
		Func_Of_Proc_Init_Listhead(&dlist);
	}
	_enter_critical_bh(&st_ctl->tracker_q.lock, &irqL);

	phead = &st_ctl->tracker_q.queue;
	plist = get_next(phead);
	pnext = get_next(plist);
	while (Func_Of_Proc_End_Of_Queue_Search(phead, plist) == _FALSE) {
		st = LIST_CONTAINOR(plist, struct session_tracker, list);
		plist = pnext;
		pnext = get_next(pnext);

		if (st->status != ST_STATUS_ESTABLISH
			&& Func_Of_Proc_Get_Passing_Time_Ms(st->set_time) > ST_EXPIRE_MS) {
			wl_list_delete(&st->list);
			Func_Of_Proc_List_Insert_Tail(&st->list, &dlist);
		}

		if (st->status == ST_STATUS_CHECK)
			st->status = ST_STATUS_ESTABLISH;

		if (st->status != ST_STATUS_ESTABLISH)
			continue;

#ifdef CONFIG_WFD
		if (ntohs(st->local_port) == wadptdata->wfd_info.rtsp_ctrlport)
			op_wfd_mode |= MIRACAST_SINK;
		if (ntohs(st->remote_port) == wadptdata->wfd_info.peer_rtsp_ctrlport)
			op_wfd_mode |= MIRACAST_SOURCE;
#endif
	}

	_exit_critical_bh(&st_ctl->tracker_q.lock, &irqL);

	plist = get_next(&dlist);
	while (Func_Of_Proc_End_Of_Queue_Search(&dlist, plist) == _FALSE) {
		st = LIST_CONTAINOR(plist, struct session_tracker, list);
		plist = get_next(plist);
		wl_mfree((u8 *) st, sizeof(struct session_tracker));
	}

chk_sta:
	if (STA_OP_WFD_MODE(sta) != op_wfd_mode) {
		STA_SET_OP_WFD_MODE(sta, op_wfd_mode);
		rpt_sta_media_status_cmd(wadptdata, sta, 1, 1);
	}

exit:
	return;
}

void chk_for_session_tracker_wadptdata_func(_wadptdata * wadptdata)
{
	struct sta_priv *stapriv = &wadptdata->stapriv;
	struct sta_info *sta;
	int i;
	_irqL irqL;
	_list *plist, *phead;
	u8 op_wfd_mode = MIRACAST_DISABLED;

	_enter_critical_bh(&stapriv->sta_hash_lock, &irqL);

	for (i = 0; i < NUM_STA; i++) {
		phead = &(stapriv->sta_hash[i]);
		plist = get_next(phead);

		while ((Func_Of_Proc_End_Of_Queue_Search(phead, plist)) == _FALSE) {
			sta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);

			chk_for_session_tracker_sta_func(wadptdata, sta, 1);

			op_wfd_mode |= STA_OP_WFD_MODE(sta);
		}
	}

	_exit_critical_bh(&stapriv->sta_hash_lock, &irqL);

#ifdef CONFIG_WFD
	wadptdata->wfd_info.op_wfd_mode = MIRACAST_MODE_REVERSE(op_wfd_mode);
#endif
}

static void proc_session_tracker_cmd_hdl_func(_wadptdata * wadptdata, struct st_cmd_parm *parm,
							 u8 flag)
{
	u8 cmd = parm->cmd;
	struct sta_info *sta = parm->sta;

	if (cmd == ST_CMD_CHK) {
		if (sta)
			chk_for_session_tracker_sta_func(wadptdata, sta, 1);
		else
			chk_for_session_tracker_wadptdata_func(wadptdata);

		goto exit;

	} else if (cmd == ST_CMD_ADD || cmd == ST_CMD_DEL) {
		struct st_ctl_t *st_ctl;
		u32 local_naddr = parm->local_naddr;
		u16 local_port = parm->local_port;
		u32 remote_naddr = parm->remote_naddr;
		u16 remote_port = parm->remote_port;
		struct session_tracker *st = NULL;
		_irqL irqL;
		_list *plist, *phead;
		u8 free_st = 0;
		u8 alloc_st = 0;

		if (!(sta->state & _FW_LINKED))
			goto exit;

		st_ctl = &sta->st_ctl;

		_enter_critical_bh(&st_ctl->tracker_q.lock, &irqL);

		phead = &st_ctl->tracker_q.queue;
		plist = get_next(phead);
		while (Func_Of_Proc_End_Of_Queue_Search(phead, plist) == _FALSE) {
			st = LIST_CONTAINOR(plist, struct session_tracker, list);

			if (st->local_naddr == local_naddr
				&& st->local_port == local_port
				&& st->remote_naddr == remote_naddr
				&& st->remote_port == remote_port)
				break;

			plist = get_next(plist);
		}

		if (Func_Of_Proc_End_Of_Queue_Search(phead, plist) == _TRUE)
			st = NULL;

		switch (cmd) {
		case ST_CMD_DEL:
			if (st) {
				wl_list_delete(plist);
				free_st = 1;
			}
			goto unlock;
		case ST_CMD_ADD:
			if (!st)
				alloc_st = 1;
		}

unlock:
		_exit_critical_bh(&st_ctl->tracker_q.lock, &irqL);

		if (free_st) {
			wl_mfree((u8 *) st, sizeof(struct session_tracker));
			goto exit;
		}

		if (alloc_st) {
			st = (struct session_tracker *)
				wl_zmalloc(sizeof(struct session_tracker));
			if (!st)
				goto exit;

			if (flag) {
				st->local_naddr = local_naddr;
				st->local_port = local_port;
				st->remote_naddr = remote_naddr;
				st->remote_port = remote_port;
				st->set_time = Func_Of_Proc_Get_Current_Time();
				st->status = ST_STATUS_CHECK;
			}
			_enter_critical_bh(&st_ctl->tracker_q.lock, &irqL);
			Func_Of_Proc_List_Insert_Tail(&st->list, phead);
			_exit_critical_bh(&st_ctl->tracker_q.lock, &irqL);
		}
	}

exit:
	return;
}

u8 proc_drvextra_cmd_func(_wadptdata * pwadptdata, unsigned char *pbuf)
{
	int ret = wMBOX1_SUCCESS;
	struct drvextra_cmd_parm *pdrvextra_cmd;

	if (!pbuf)
		return wMBOX1_PARAMETERS_ERROR;

	pdrvextra_cmd = (struct drvextra_cmd_parm *)pbuf;

	switch (pdrvextra_cmd->ec_id) {
	case STA_MSTATUS_RPT_WK_CID:
		rpt_sta_media_status_cmd_hdl(pwadptdata,
										 (struct sta_media_status_rpt_cmd_parm
										  *)pdrvextra_cmd->pbuf);
		break;

	case DYNAMIC_CHK_WK_CID:
#ifdef CONFIG_CONCURRENT_MODE
		if (pwadptdata->pbuddy_wadptdata)
			proc_dynamic_chk_wk_func(pwadptdata->pbuddy_wadptdata, 1);
#endif
		proc_dynamic_chk_wk_func(pwadptdata, 1);
		break;
	case POWER_SAVING_CTRL_WK_CID:
		proc_power_saving_wk_func(pwadptdata);
		break;
#ifdef CONFIG_LPS
	case LPS_CTRL_WK_CID:
		proc_lps_ctrl_wk_func(pwadptdata, (u8) pdrvextra_cmd->type);
		break;
	case DM_IN_LPS_WK_CID:
		proc_dm_in_lps_func(pwadptdata);
		break;
	case LPS_CHANGE_DTIM_CID:
		proc_lps_change_dtim_func(pwadptdata, (u8) pdrvextra_cmd->type, 1);
		break;
#endif
#if (RATE_ADAPTIVE_SUPPORT==1)
	case RTP_TIMER_CFG_WK_CID:
		proc_report_timer_setting_wk_func(pwadptdata, pdrvextra_cmd->type);
		break;
#endif

#ifdef CONFIG_P2P
	case P2P_PROTO_WK_CID:
		handler_p2p_protocol_wk_func(pwadptdata, pdrvextra_cmd->type, 1);
		break;
#endif
#ifdef CONFIG_AP_MODE
	case CHECK_HIQ_WK_CID:
		proc_chk_hi_queue_func(pwadptdata, 1);
		break;
#endif
	case RESET_SECURITYPRIV:
		proc_reset_securitypriv_func(pwadptdata);
		break;
	case FREE_ASSOC_RESOURCES:
		proc_free_assoc_resources_func(pwadptdata);
		break;
	case wMBOX0_WK_CID:
		Func_Of_Proc_Chip_Hw_Set_Hwreg_With_Buf(pwadptdata, pdrvextra_cmd->pbuf,
								   pdrvextra_cmd->size);
		break;
	case DM_RA_MSK_WK_CID:
		proc_dm_ra_mask_func(pwadptdata, (struct sta_info *)pdrvextra_cmd->pbuf, 1);
		break;
	case SESSION_TRACKER_WK_CID:
		proc_session_tracker_cmd_hdl_func(pwadptdata,
								(struct st_cmd_parm *)pdrvextra_cmd->pbuf, 1);
		break;

	case EN_HW_UPDATE_TSF_WK_CID:
		Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_EN_HW_UPDATE_TSF, NULL);
		break;

	case MP_CMD_WK_CID:
		ret = proc_mp_cmd_hdl_func(pwadptdata, pdrvextra_cmd->type);
		break;

	default:
		break;
	}

	if (pdrvextra_cmd->pbuf && pdrvextra_cmd->size > 0) {
		wl_mfree(pdrvextra_cmd->pbuf, pdrvextra_cmd->size);
	}

	return ret;
}

void proc_survey_cmd_callback_func(_wadptdata * pwadptdata, struct cmd_obj *pcmd)
{
	struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;

	_func_enter_;

	if (pcmd->res == wMBOX1_DROPPED) {
		mlme_set_scan_to_timer(pmlmepriv, 1);
	} else if (pcmd->res != wMBOX1_SUCCESS) {
		mlme_set_scan_to_timer(pmlmepriv, 1);
		WL_INFO_L2("\n ********Error: MgntActwl_set_802_11_bssid_LIST_SCAN Fail ************\n\n.");
	}

	proc_free_cmd_obj_func(pcmd);

	_func_exit_;
}

void callback_disassoc_cmd_func(_wadptdata * pwadptdata, struct cmd_obj *pcmd)
{
	_irqL irqL;
	struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;

	_func_enter_;

	if (pcmd->res != wMBOX1_SUCCESS) {
		_enter_critical_bh(&pmlmepriv->lock, &irqL);
		set_fwstate(pmlmepriv, _FW_LINKED);
		_exit_critical_bh(&pmlmepriv->lock, &irqL);

		WL_INFO_L2("\n ***Error: disconnect_cmd_callback Fail ***\n.");

		goto exit;
	}
#ifdef CONFIG_BR_EXT
	else
		db_cleanup_of_nat25_func(pwadptdata, 1);
#endif

	proc_free_cmd_obj_func(pcmd);

exit:

	_func_exit_;
}

void callback_getmacreg_cmdrsp_func(_wadptdata * pwadptdata, struct cmd_obj *pcmd)
{

	_func_enter_;

	proc_free_cmd_obj_func(pcmd);

	_func_exit_;
}

static struct wlan_network *oldest_wlan_network_to_get_func(_queue * scanned_queue,
														u8 tag)
{
	_list *plist, *phead;
	struct wlan_network *pwlan = NULL;
	struct wlan_network *oldest = NULL;
	_func_enter_;
	phead = get_list_head(scanned_queue);

	plist = get_next(phead);
	if (tag) {
		while (1) {

			if (Func_Of_Proc_End_Of_Queue_Search(phead, plist) == _TRUE)
				break;

			pwlan = LIST_CONTAINOR(plist, struct wlan_network, list);

			if (pwlan->fixed != _TRUE) {
				if (oldest == NULL
					|| time_after(oldest->last_scanned, pwlan->last_scanned))
					oldest = pwlan;
			}

			plist = get_next(plist);
		}
	}
	_func_exit_;
	return oldest;

}

void callback_joinbss_cmd_func(_wadptdata * pwadptdata, struct cmd_obj *pcmd)
{
	struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;

	_func_enter_;

	if (pcmd->res == wMBOX1_DROPPED) {
		_set_timer(&pmlmepriv->assoc_timer, 1);
	} else if (pcmd->res != wMBOX1_SUCCESS) {
		_set_timer(&pmlmepriv->assoc_timer, 1);
	}

	proc_free_cmd_obj_func(pcmd);

	_func_exit_;
}

void proc_create_ibss_post_func(_wadptdata * pwadptdata, int status, u8 flag)
{
	_irqL irqL;
	u8 timer_cancelled;
	struct sta_info *psta = NULL;
	struct wlan_network *pwlan = NULL;
	struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;
	WLAN_BSSID_EX *pdev_network = &pwadptdata->registrypriv.dev_network;
	struct wlan_network *mlme_cur_network = &(pmlmepriv->cur_network);

	if (status != wMBOX1_SUCCESS)
		_set_timer(&pmlmepriv->assoc_timer, 1);

	if (flag) {
		_cancel_timer(&pmlmepriv->assoc_timer, &timer_cancelled);
	}
	_enter_critical_bh(&pmlmepriv->lock, &irqL);

	{
		_irqL irqL;

		pwlan = do_new_network(pmlmepriv, 1);
		_enter_critical_bh(&(pmlmepriv->scanned_queue.lock), &irqL);
		if (pwlan == NULL) {
			pwlan = oldest_wlan_network_to_get_func(&pmlmepriv->scanned_queue, 1);
			if (pwlan == NULL) {
				WL_INFO_L2("Error:  can't get pwlan in do_joinbss_event_callback\n");
				_exit_critical_bh(&(pmlmepriv->scanned_queue.lock), &irqL);
				goto createbss_cmd_fail;
			}
			pwlan->last_scanned = Func_Of_Proc_Get_Current_Time();
		} else {
			Func_Of_Proc_List_Insert_Tail(&(pwlan->list),
								 &pmlmepriv->scanned_queue.queue);
		}

		pdev_network->Length = get_WLAN_BSSID_EX_sz(pdev_network);
		Func_Of_Proc_Pre_Memcpy(&(pwlan->network), pdev_network, pdev_network->Length);

		Func_Of_Proc_Pre_Memcpy(&mlme_cur_network->network, pdev_network,
					(get_WLAN_BSSID_EX_sz(pdev_network)));

		_clr_fwstate_(pmlmepriv, _FW_UNDER_LINKING);

		_exit_critical_bh(&(pmlmepriv->scanned_queue.lock), &irqL);
	}

createbss_cmd_fail:
	_exit_critical_bh(&pmlmepriv->lock, &irqL);
exit:
	return;
}

void callback_setstaKey_cmdrsp_func(_wadptdata * pwadptdata, struct cmd_obj *pcmd)
{

	struct sta_priv *pstapriv = &pwadptdata->stapriv;
	struct set_stakey_rsp *psetstakey_rsp =
		(struct set_stakey_rsp *)(pcmd->rsp);
	struct sta_info *psta = do_query_stainfo(pstapriv, psetstakey_rsp->addr, 1);

	_func_enter_;

	if (psta == NULL) {
		WL_INFO_L2("\nERROR: callback_setstaKey_cmdrsp_func => can't get sta_info \n\n");
		goto exit;
	}

exit:

	proc_free_cmd_obj_func(pcmd);

	_func_exit_;

}

void callback_setassocsta_cmdrsp_func(_wadptdata * pwadptdata, struct cmd_obj *pcmd)
{
	_irqL irqL;
	struct sta_priv *pstapriv = &pwadptdata->stapriv;
	struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;
	struct set_assocsta_parm *passocsta_parm =
		(struct set_assocsta_parm *)(pcmd->parmbuf);
	struct set_assocsta_rsp *passocsta_rsp =
		(struct set_assocsta_rsp *)(pcmd->rsp);
	struct sta_info *psta = do_query_stainfo(pstapriv, passocsta_parm->addr, 1);

	_func_enter_;

	if (psta == NULL) {
		WL_INFO_L2("\nERROR: setassocsta_cmdrsp_callbac => can't get sta_info \n\n");
		goto exit;
	}

	psta->aid = psta->mac_id = passocsta_rsp->cam_id;

	_enter_critical_bh(&pmlmepriv->lock, &irqL);

	if ((check_fwstate(pmlmepriv, WIFI_MP_STATE) == _TRUE)
		&& (check_fwstate(pmlmepriv, _FW_UNDER_LINKING) == _TRUE))
		_clr_fwstate_(pmlmepriv, _FW_UNDER_LINKING);

	set_fwstate(pmlmepriv, _FW_LINKED);
	_exit_critical_bh(&pmlmepriv->lock, &irqL);

exit:
	proc_free_cmd_obj_func(pcmd);

	_func_exit_;
}

void callback_getrttbl_cmd_cmdrsp_func(_wadptdata * pwadptdata,
									  struct cmd_obj *pcmd);
void callback_getrttbl_cmd_cmdrsp_func(_wadptdata * pwadptdata, struct cmd_obj *pcmd)
{
	_func_enter_;

	proc_free_cmd_obj_func(pcmd);
#ifdef CONFIG_MP_INCLUDED
	if (pwadptdata->registrypriv.mp_mode == 1)
		pwadptdata->mppriv.workparam.bcompleted = _TRUE;
#endif

	_func_exit_;

}
#endif


