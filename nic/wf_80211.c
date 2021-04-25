
#include "common.h"
#include "wf_debug.h"

#if 0
#define _80211_DBG(fmt, ...)      LOG_D("[%s]"fmt, __func__, ##__VA_ARGS__)
#define _80211_WARN(fmt, ...)     LOG_E("[%s]"fmt, __func__, ##__VA_ARGS__)
#define _80211_ARRAY(data, len)   log_array(data, len)
#else
#define _80211_DBG(fmt, ...)
#define _80211_WARN(fmt, ...)
#define _80211_ARRAY(data, len)
#endif

int wf_80211_mgmt_ies_search (void *pvar, wf_u16 var_len,
                              wf_u8 cmp_id,
                              wf_80211_mgmt_ie_t **pout)
{
    wf_80211_mgmt_ie_t *pie = NULL;
    wf_u16 ele_len;
    wf_u8 *pstart = pvar;
    wf_u16 offset = 0;

    if (pvar == NULL || pout == NULL)
    {
        return WF_RETURN_FAIL;
    }

    while (1)
    {
        pie = (wf_80211_mgmt_ie_t *)(pstart + offset);
        if (pie->element_id == cmp_id)
        {
            *pout = pie;
            return WF_RETURN_OK;
        }

        offset += 2 + pie->len;
        if (offset >= var_len)
        {
            return WF_RETURN_FAIL;
        }
    }
}

int wf_80211_mgmt_ies_search_with_oui (void *pies, wf_u16 ies_len,
                                       wf_u8 cmp_id, wf_u8 *oui,
                                       wf_80211_mgmt_ie_t **ppie)
{
    wf_80211_mgmt_ie_t *pie;
    wf_u16 ele_len;
    wf_u8 *pstart = pies;
    wf_u16 offset = 0;

    if (pies == NULL || ppie == NULL)
    {
        return WF_RETURN_FAIL;
    }

    while (1)
    {
        pie = (wf_80211_mgmt_ie_t *)(pstart + offset);
        if (pie->element_id == cmp_id)
        {
            if (wf_memcmp(pie->data, oui, 4) == 0)
            {
                *ppie = pie;
                return WF_RETURN_OK;
            }
        }

        offset += 2 + pie->len;
        if (offset >= ies_len)
        {
            return WF_RETURN_FAIL;
        }
    }
}

wf_u8 *wf_80211_set_fixed_ie(wf_u8 *pbuf, wf_u32 len, wf_u8 *source, wf_u32 *frlen)
{

	wf_memcpy((void *)pbuf, (void *)source, len);

	*frlen = *frlen + len;
	return (pbuf + len);
}


wf_bool is_snap_hdr(wf_u8 *phdr)
{
    wf_u8 *psnap = phdr;
    wf_u8 *psnap_type;
    psnap_type = phdr + SNAP_HDR_SIZE;

    if ((array_equal(psnap, wl_rfc1042_header, SNAP_HDR_SIZE) &&
         (array_equal(psnap_type, SNAP_ETH_TYPE_IPX, 2) == wf_false) &&
         (array_equal(psnap_type, SNAP_ETH_TYPE_APPLETALK_AARP, 2) == wf_false))
        || array_equal(psnap, wl_bridge_tunnel_header, SNAP_HDR_SIZE))
    {
        return wf_true;
    }
    else
    {
        return wf_false;
    }
}

wf_u8 *get_bssid(wf_u8 *pbuf)
{
    switch ((To_From_Ds_t)(GET_HDR_To_From_DS(pbuf) >> 8))
    {
        case STA_TO_STA :
            return GET_ADDR3(pbuf);
        case STA_TO_DS :
            return GET_ADDR1(pbuf);
        case DS_TO_STA :
            return GET_ADDR2(pbuf);
        case MESH_TO_MESH :
        default :
            return NULL;
    }
}

static wf_inline wf_u32 rsne_cipher_suite_parse (wf_u32 cipher_suite)
{
    if (cipher_suite == WF_80211_RSN_CIPHER_SUITE_USE_GROUP)
        return CIPHER_SUITE_NONE;
    if (cipher_suite == WF_80211_RSN_CIPHER_SUITE_WEP40)
        return CIPHER_SUITE_WEP40;
    if (cipher_suite == WF_80211_RSN_CIPHER_SUITE_WEP104)
        return CIPHER_SUITE_WEP104;
    if (cipher_suite == WF_80211_RSN_CIPHER_SUITE_TKIP)
        return CIPHER_SUITE_TKIP;
    if (cipher_suite == WF_80211_RSN_CIPHER_SUITE_CCMP)
        return CIPHER_SUITE_CCMP;

    return 0;
}

int wf_80211_mgmt_rsn_parse (void *prsn, wf_u16 len,
                             wf_u32 *pgroup_cipher, wf_u32 *pairwise_cipher)
{
    wf_80211_mgmt_ie_t *pie;
    wf_u8 left, *pos;
    wf_u16 version;
    wf_u16 cipher_suite_cnt;
    int ret = 0;

    if (prsn == NULL || len == 0)
    {
        _80211_WARN("NULL point");
        ret = -1;
        goto exit;
    }

    pie = prsn;
    if (!(pie->element_id == WF_80211_MGMT_EID_RSN &&
          len >= WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len))
    {
        _80211_DBG("no rsn element");
        ret = -2;
        goto exit;
    }
    pos = pie->data;
    left = len - WF_OFFSETOF(wf_80211_mgmt_ie_t, data);

#define RSNE_VERSION_SIZE                       2
#define RSNE_GROUP_DATA_CIPHER_SUITE_SIZE       4
#define RSNE_PARIWISE_CIPHER_SUITE_COUNT_SIZE   2
#define RSNE_PARIWISE_CIPHER_SUITE_LIST_SIZE    4

    /* check version field */
    if (left < RSNE_VERSION_SIZE)
    {
        _80211_WARN("no version field");
        ret = -3;
        goto exit;
    }
    version = wf_le16_to_cpu(*(wf_u16 *)pos);
    if (version != 1)
    {
        _80211_WARN("no support version");
        ret = -4;
        goto exit;
    }
    left -= RSNE_VERSION_SIZE;
    pos += RSNE_VERSION_SIZE;
    if (left == 0)
    {
        _80211_DBG("no any option field");
        goto exit;
    }

    /* get group data cipher suite */
    if (left < RSNE_GROUP_DATA_CIPHER_SUITE_SIZE)
    {
        _80211_DBG("no group data cipher suite");
        ret = -5;
        goto exit;
    }
    if (pgroup_cipher)
    {
        *pgroup_cipher = rsne_cipher_suite_parse(wf_be32_to_cpu(*(wf_u32 *)pos));
    }
    left -= RSNE_GROUP_DATA_CIPHER_SUITE_SIZE;
    pos  += RSNE_GROUP_DATA_CIPHER_SUITE_SIZE;
    if (left == 0)
    {
        _80211_DBG("only group data cipher suite field");
        goto exit;
    }

    /* get pairwise cipher suite count */
    if (left < RSNE_PARIWISE_CIPHER_SUITE_COUNT_SIZE)
    {
        _80211_DBG("no cipher suite count filed");
        ret = -6;
        goto exit;
    }
    cipher_suite_cnt = wf_le16_to_cpu(*(wf_u16 *)pos);
    left -= RSNE_PARIWISE_CIPHER_SUITE_COUNT_SIZE;
    pos  += RSNE_PARIWISE_CIPHER_SUITE_COUNT_SIZE;
    /* get pairwise cipher suite */
    if (cipher_suite_cnt == 0 ||
        left < cipher_suite_cnt * RSNE_PARIWISE_CIPHER_SUITE_LIST_SIZE)
    {
        _80211_WARN("cipher suite count(%d) error", cipher_suite_cnt);
        ret = -7;
        goto exit;
    }
    if (pairwise_cipher)
    {
        *pairwise_cipher = 0x0;
        do
        {
            *pairwise_cipher |= rsne_cipher_suite_parse(wf_be32_to_cpu(*(wf_u32 *)pos));
            pos += RSNE_PARIWISE_CIPHER_SUITE_LIST_SIZE;
        }
        while (--cipher_suite_cnt);
    }

exit :
    return ret;
}

static wf_inline wf_u32 get_wpa_cipher_suite (wf_u32 cipher_suite)
{
    wf_u32 oui;
    wf_u8 type;

    oui = cipher_suite >> 8;
    if (oui != WF_80211_OUI_MICROSOFT)
        return 0;

    type = (wf_u8)cipher_suite;
    switch (type)
    {
        case 0 :
            return CIPHER_SUITE_NONE;
        case 1 :
            return CIPHER_SUITE_WEP40;
        case 2 :
            return CIPHER_SUITE_TKIP;
        case 4 :
            return CIPHER_SUITE_CCMP;
        case 5 :
            return CIPHER_SUITE_WEP104;
    }

    return 0;
}


int wf_80211_mgmt_wpa_parse (void *pwpa, wf_u16 len,
                             wf_u32 *pmulticast_cipher, wf_u32 *punicast_cipher)
{
    wf_u8 left, *pos;
    wf_80211_mgmt_ie_t *pie;
    wf_u32 oui_type;
    wf_u16 version;
    wf_u16 cipher_suite_cnt;
    int ret = 0;

    if (pwpa == NULL || len == 0)
    {
        _80211_WARN("invalid parameter");
        ret = -1;
        goto exit;
    }

    pie = pwpa;
    if (!(pie->element_id == WF_80211_MGMT_EID_VENDOR_SPECIFIC &&
          len >= WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + pie->len))
    {
        _80211_DBG("data corrupt");
        ret = -2;
        goto exit;
    }
    pos = pie->data;
    left = len - WF_OFFSETOF(wf_80211_mgmt_ie_t, data);

#define WPA_OUI_TYPE_SIZE                       4
#define WPA_VERSION_SIZE                        2
#define WPA_MULTICAST_CIPHER_SUITE_SIZE         4
#define WPA_UNICAST_CIPHER_SUITE_COUNT_SIZE     2
#define WPA_UNICAST_CIPHER_SUITE_LIST_SIZE      4

    /* check OUT-type field */
    if (left < WPA_OUI_TYPE_SIZE)
    {
        _80211_WARN("no oui-type field");
        ret = -3;
        goto exit;
    }
    oui_type = wf_be32_to_cpu(*(wf_u32 *)pos);
    if (oui_type !=
        ((WF_80211_OUI_MICROSOFT << 8) | WF_80211_OUI_TYPE_MICROSOFT_WPA))
    {
        _80211_DBG("no wpa element");
        ret = -4;
        goto exit;
    }
    left -= WPA_OUI_TYPE_SIZE;
    pos += WPA_OUI_TYPE_SIZE;

    /* check version field */
    if (left < WPA_VERSION_SIZE)
    {
        _80211_WARN("no version field");
        ret = -3;
        goto exit;
    }
    version = wf_le16_to_cpu(*(wf_u16 *)pos);
    if (version != 1)
    {
        _80211_WARN("no support version");
        ret = -4;
        goto exit;
    }
    left -= WPA_VERSION_SIZE;
    pos += WPA_VERSION_SIZE;
    if (left == 0)
    {
        _80211_DBG("no any option field");
        goto exit;
    }

    /* get mulitcast cipher suite */
    if (left < WPA_MULTICAST_CIPHER_SUITE_SIZE)
    {
        _80211_WARN("no mulitcast cipher suite");
        ret = -5;
        goto exit;
    }
    if (pmulticast_cipher)
        *pmulticast_cipher = get_wpa_cipher_suite(wf_be32_to_cpu(*(wf_u32 *)pos));
    left -= WPA_MULTICAST_CIPHER_SUITE_SIZE;
    pos += WPA_MULTICAST_CIPHER_SUITE_SIZE;
    if (left == 0)
    {
        _80211_DBG("only mulitcast cipher suite field");
        goto exit;
    }

    /* get unicast cipher suite count */
    if (left < WPA_UNICAST_CIPHER_SUITE_COUNT_SIZE)
    {
        _80211_WARN("no unicast cipher suite count filed");
        ret = -6;
        goto exit;
    }
    cipher_suite_cnt = wf_le16_to_cpu(*(wf_u16 *)pos);

    left -= WPA_UNICAST_CIPHER_SUITE_COUNT_SIZE;
    pos += WPA_UNICAST_CIPHER_SUITE_COUNT_SIZE;
    /* get unicast cipher suite */
    if (cipher_suite_cnt == 0 ||
        left < cipher_suite_cnt * WPA_UNICAST_CIPHER_SUITE_LIST_SIZE)
    {
        _80211_WARN("cipher suite count(%d) error", cipher_suite_cnt);
        ret = -7;
        goto exit;
    }
    if (punicast_cipher)
    {
        *punicast_cipher = 0x0;
        do
        {
            *punicast_cipher |= get_wpa_cipher_suite(wf_be32_to_cpu(*(wf_u32 *)pos));
            pos += WPA_UNICAST_CIPHER_SUITE_LIST_SIZE;
        }
        while (--cipher_suite_cnt);
    }

exit :
    return ret;
}


int wf_80211_mgmt_wmm_parse (void *pwmm, wf_u16 len)
{
    wf_u8 left, *pos;
    wf_80211_wmm_param_ie_t *pie;
    wf_u64 oui_type;
    wf_u16 version;
    int ret = 0;

    if (pwmm == NULL || len == 0)
    {
        _80211_WARN("NUll point");
        ret = -1;
        goto exit;
    }

    pie = pwmm;
    if (!(pie->element_id == WF_80211_MGMT_EID_VENDOR_SPECIFIC &&
          len >= WF_OFFSETOF(wf_80211_wmm_param_ie_t, oui) + pie->len))
    {
        _80211_WARN("data corrupt");
        ret = -2;
        goto exit;
    }

    oui_type = (pie->oui[0] << 16) | (pie->oui[1] << 8) | (pie->oui[2] << 0);
    if (!(oui_type == WF_80211_OUI_MICROSOFT &&
          pie->oui_type == WF_80211_OUI_TYPE_MICROSOFT_WMM))
    {
        _80211_DBG("no wmm element");
        ret = -3;
        goto exit;
    }
    if (!(pie->oui_subtype == 0 && pie->version == 1))
    {
        _80211_WARN("unknow subtype(%d) and version(%d)",
                    pie->oui_subtype, pie->version);
        ret = -4;
        goto exit;
    }

exit :
    return ret;
}

int wf_wlan_get_sec_ie(wf_u8 *in_ie, wf_u32 in_len,
                       wf_u8 *rsn_ie, wf_u16 *rsn_len,
                       wf_u8 *wpa_ie, wf_u16 *wpa_len,
                       wf_u8 flag)
{
    wf_u8 authmode, sec_idx, i;
    wf_u8 wpa_oui[4] = { 0x0, 0x50, 0xf2, 0x01 };
    wf_u32 cnt;

    cnt = (_TIMESTAMP_ + _BEACON_ITERVAL_ + _CAPABILITY_);

    sec_idx = 0;
    if (flag)
    {
        while (cnt < in_len)
        {
            authmode = in_ie[cnt];

            if ((authmode == _WPA_IE_ID_)
                && (!wf_memcpy(&in_ie[cnt + 2], &wpa_oui[0], 4)))
            {
#if 0
                LOG_E("\n wpa_ie_to_get_func: sec_idx=%d in_ie[cnt+1]+2=%d\n",
                      sec_idx, in_ie[cnt + 1] + 2);
#endif

                if (wpa_ie)
                {
                    wf_memcpy(wpa_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

                    for (i = 0; i < (in_ie[cnt + 1] + 2); i = i + 8)
                    {

                    }
                }

                *wpa_len = in_ie[cnt + 1] + 2;
                cnt += in_ie[cnt + 1] + 2;
            }
            else
            {
                if (authmode == _WPA2_IE_ID_)
                {
#if 0
                    LOG_E("\n get_rsn_ie: sec_idx=%d in_ie[cnt+1]+2=%d\n",
                          sec_idx, in_ie[cnt + 1] + 2);
#endif

                    if (rsn_ie)
                    {
                        wf_memcpy(rsn_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

                        for (i = 0; i < (in_ie[cnt + 1] + 2); i = i + 8)
                        {

                        }
                    }

                    *rsn_len = in_ie[cnt + 1] + 2;
                    cnt += in_ie[cnt + 1] + 2;
                }
                else
                {
                    cnt += in_ie[cnt + 1] + 2;
                }
            }

        }
    }

    return (*rsn_len + *wpa_len);
}

wf_u8 wf_wlan_check_is_wps_ie(wf_u8 *ie_ptr, wf_u32 *wps_ielen)
{
    wf_u8 match = wf_false;
    wf_u8 eid, wps_oui[4] = {0x0, 0x50, 0xf2, 0x04};

    if (ie_ptr == NULL)
        return match;

    eid = ie_ptr[0];

    if ((eid == _WPA_IE_ID_) && (!wf_memcmp(&ie_ptr[2], wps_oui, 4)))
    {
        *wps_ielen = ie_ptr[1] + 2;
        match = wf_true;
    }
    return match;
}

wf_u8 *wf_wlan_get_wps_ie(wf_u8 *temp_ie, wf_u32 temp_len, wf_u8 *wps_ie, wf_u32 *ie_len)
{
	wf_u32 count = 0;
	wf_u8 *temp_wps_ie = NULL;
	wf_u8 eid, wps_oui[4] = {0x00, 0x50, 0xf2, 0x04};

	if(ie_len)
	{
		*ie_len = 0;
	}

	if(!temp_ie)
	{
		LOG_E("[%s]temp_ie isn't null, check", __func__);
        *ie_len = 0;
		return temp_wps_ie;
	}

	if(temp_len <= 0)
	{
		LOG_E("[%s]ie_len is 0, check", __func__);
        *ie_len = 0;
		return temp_wps_ie;
	}

	while(count + 1 + 4 < temp_len)
	{
		eid = temp_ie[count];
		if(eid == WF_80211_MGMT_EID_VENDOR_SPECIFIC && wf_memcmp(&temp_ie[count + 2], wps_oui, 4) == wf_false)
		{
			 	temp_wps_ie = temp_ie + count;
                if(wps_ie)
                {
			        wf_memcpy(wps_ie, &temp_ie[count], temp_ie[count + 1 + 2]);
                }

				if(ie_len)
				{
					*ie_len = temp_ie[count + 1] + 2;
				}
				break;
			}
			else
			{
				count += temp_ie[count + 1] + 2;
			}
	}
	return temp_wps_ie;
}

int wf_ch_2_freq(int ch)
{
    if (ch >= 1 && ch <= 13)
    {
        return 2407 + ch * 5;
    }
    else if (ch == 14)
    {
        return 2484;
    }
    else if (ch >= 36 && ch <= 177)
    {
        return 5000 + ch * 5;
    }

    return 0; /* not supported */
}

int freq_2_ch(int freq)
{
    /* see 802.11 17.3.8.3.2 and Annex J */
    if (freq < 2484)
        return (freq - 2407) / 5;
    else if (freq == 2484)
        return 14;
    else if (freq >= 4910 && freq <= 4980)
        return (freq - 4000) / 5;
    else if (freq <= 45000) /* DMG band lower limit */
        return (freq - 5000) / 5;
    else if (freq >= 58320 && freq <= 64800)
        return (freq - 56160) / 2160;
    else
        return 0; /* err param */
}


