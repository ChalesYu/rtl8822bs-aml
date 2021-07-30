/*
 * wdn.c
 *
 * impliment WDN(wireless device node) management
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include "common.h"
#include "wf_debug.h"

#define WDN_INFO_DUMP

#if 0
#define WDN_DBG(fmt, ...)       LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define WDN_ARRAY(data, len)    log_array(data, len)
#else
#define WDN_DBG(fmt, ...)
#define WDN_ARRAY(data, len)
#endif
#define WDN_INFO(fmt, ...)      LOG_I("[%s:%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WDN_WARN(fmt, ...)      LOG_W("[%s:%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WDN_ERROR(fmt, ...)     LOG_E("[%s:%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

wf_u8 WPA_OUI[4] = { 0x00, 0x50, 0xf2, 0x01 };
wf_u8 WMM_OUI[4] = { 0x00, 0x50, 0xf2, 0x02 };
wf_u8 WPS_OUI[4] = { 0x00, 0x50, 0xf2, 0x04 };
wf_u8 P2P_OUI[4] = { 0x50, 0x6F, 0x9A, 0x09 };
wf_u8 WFD_OUI[4] = { 0x50, 0x6F, 0x9A, 0x0A };


static int new_wdn_id (wdn_list *pwdn, wf_u8 *pwdn_id)
{
    wf_u8 i;
    int bit_mask;

    for (i = 0; i < 32; i++)
    {
        bit_mask = BIT(i);
        if (!(pwdn->id_bitmap & bit_mask))
        {
            pwdn->id_bitmap |= bit_mask;
            *pwdn_id = i;
            return 0;
        }
    }

    return -1;
}

static int free_wdn_id (wdn_list *pwdn, wf_u16 id)
{
    if (id >= 32)
    {
        return -1;
    }

    pwdn->id_bitmap &= ~ BIT(id);
    return 0;
}

void get_bratecfg_by_support_dates(wf_u8 *pdataRate, wf_u8 dataRate_len, wf_u16 *pBrateCfg)
{
    wf_u8 i, is_brate, brate;

    for (i = 0; i < dataRate_len; i++)
    {
        is_brate = pdataRate[i] & IEEE80211_BASIC_RATE_MASK;
        brate = pdataRate[i] & 0x7f;

        if (is_brate)
        {
            switch (brate)
            {
                case IEEE80211_CCK_RATE_1MB:
                    *pBrateCfg |= WF_RATE_1M;
                    break;
                case IEEE80211_CCK_RATE_2MB:
                    *pBrateCfg |= WF_RATE_2M;
                    break;
                case IEEE80211_CCK_RATE_5MB:
                    *pBrateCfg |= WF_RATE_5_5M;
                    break;
                case IEEE80211_CCK_RATE_11MB:
                    *pBrateCfg |= WF_RATE_11M;
                    break;
                case IEEE80211_OFDM_RATE_6MB:
                    *pBrateCfg |= WF_RATE_6M;
                    break;
                case IEEE80211_OFDM_RATE_9MB:
                    *pBrateCfg |= WF_RATE_9M;
                    break;
                case IEEE80211_OFDM_RATE_12MB:
                    *pBrateCfg |= WF_RATE_12M;
                    break;
                case IEEE80211_OFDM_RATE_18MB:
                    *pBrateCfg |= WF_RATE_18M;
                    break;
                case IEEE80211_OFDM_RATE_24MB:
                    *pBrateCfg |= WF_RATE_24M;
                    break;
                case IEEE80211_OFDM_RATE_36MB:
                    *pBrateCfg |= WF_RATE_36M;
                    break;
                case IEEE80211_OFDM_RATE_48MB:
                    *pBrateCfg |= WF_RATE_48M;
                    break;
                case IEEE80211_OFDM_RATE_54MB:
                    *pBrateCfg |= WF_RATE_54M;
                    break;
            }
        }
    }
}

wf_u8 wf_wdn_get_raid_by_network_type (wdn_net_info_st *pwdn_info)
{
    wf_u8 raid = RATEID_IDX_BGN_40M_1SS;

    switch (pwdn_info->network_type)
    {
        case WIRELESS_11B:
            raid = RATEID_IDX_B;
            pwdn_info->tx_rate = MGN_1M;
            break;

        case WIRELESS_11G:
            raid = RATEID_IDX_G;
            pwdn_info->tx_rate = MGN_1M;
            break;

        case WIRELESS_11BG:
            raid = RATEID_IDX_BG;
            pwdn_info->tx_rate = MGN_1M;
            break;

        case WIRELESS_11_24N:
            raid = RATEID_IDX_GN_N1SS;
            pwdn_info->tx_rate = MGN_MCS0;
            break;

        case WIRELESS_11G_24N:
            raid = RATEID_IDX_GN_N1SS;
            pwdn_info->tx_rate = MGN_1M;
            break;

        case WIRELESS_11B_24N:
            raid = RATEID_IDX_GN_N1SS;
            pwdn_info->tx_rate = MGN_1M;
            break;

        case WIRELESS_11BG_24N:
            if (pwdn_info->bw_mode == CHANNEL_WIDTH_20)
            {
                raid = RATEID_IDX_BGN_20M_1SS_BN;
            }
            else
            {
                raid = RATEID_IDX_BGN_40M_1SS;
            }
            pwdn_info->tx_rate = MGN_1M;
            break;

        default:
            WDN_WARN("error network type(0x%x)\n", pwdn_info->network_type);
            break;

    }

    return raid;
}

wdn_net_info_st *wf_wdn_find_info (nic_info_st *pnic_info, wf_u8 *pmac)
{
    wf_list_t *pos, *pos_next;
    wdn_node_st *pwdn_node;
    wdn_list *pwdn = pnic_info->wdn;

    if(NULL == pmac)
    {
        return NULL;
    }

    wf_list_for_each_safe(pos, pos_next, &pwdn->head)
    {
        pwdn_node = wf_list_entry(pos, wdn_node_st, list);
        if (pwdn_node && !wf_memcmp(pwdn_node->info.mac, pmac, WF_ETH_ALEN))
            return &pwdn_node->info;
    }

    return NULL;
}

wdn_net_info_st *wf_wdn_find_info_by_id(nic_info_st *nic_info, wf_u8 wdn_id)
{
    wf_list_t *pos = NULL;
    wf_list_t *next = NULL;
    wdn_node_st *tmp_node = NULL;
    wdn_list *wdn = (wdn_list *)nic_info->wdn;
    wdn_net_info_st *tmp_node_info = NULL;

    wf_list_for_each_safe(pos, next, &wdn->head)
    {
        tmp_node = wf_list_entry(pos, wdn_node_st, list);
        if (tmp_node && (tmp_node->info.wdn_id == wdn_id))
        {
            tmp_node_info = &tmp_node->info;
            break;

        }
        tmp_node = NULL;
    }

    return tmp_node_info;
}

wdn_net_info_st *wf_wdn_add (nic_info_st *pnic_info, wf_u8 *pmac)
{
    wdn_list *pwdn = pnic_info->wdn;
    wdn_node_st *pwdn_node;
    wdn_net_info_st *pwdn_info;
    wf_u8 wdn_info_id;

    /* return the wdn if already existed */
    pwdn_info = wf_wdn_find_info(pnic_info, pmac);
    if (pwdn_info != NULL)
        return pwdn_info;

    if (wf_list_is_empty(&pwdn->free))
    {
        WDN_WARN("no more wdn resource");
        return NULL;
    }

    if (new_wdn_id(pwdn, &wdn_info_id))
    {
        WDN_WARN("alloc wdn id fail");
        return NULL;
    }

    /* node remove from free list */
    pwdn_node = wf_list_entry(pwdn->free.pnext, wdn_node_st, list);
    wf_list_delete(&pwdn_node->list);

    /* update wdn_info */
    pwdn_info = &pwdn_node->info;
    wf_memset(pwdn_info, 0, sizeof(wdn_net_info_st));
    wf_memcpy(pwdn_info->mac, pmac, WF_ETH_ALEN);
    pwdn_info->wdn_id = wdn_info_id;

    /* link the node to head list */
    wf_list_insert_tail(&pwdn_node->list, &pwdn->head);

    /* update wdn count */
    pwdn->cnt++;

    return pwdn_info;
}

int wf_wdn_remove (nic_info_st *pnic_info, wf_u8 *pmac)
{
    wdn_list *pwdn = pnic_info->wdn;
    wdn_node_st *pwdn_node;
    wdn_net_info_st *pwdn_info;

    pwdn_info = wf_wdn_find_info(pnic_info, pmac);
    if (!pwdn_info)
    {
        WDN_WARN("wdn no find");
        return -1;
    }

    /* node remove from head list */
    pwdn_node = wf_list_entry(pwdn_info, wdn_node_st, info);
    wf_list_delete(&pwdn_node->list);
    /* link the node to free list */
    wf_list_insert_tail(&pwdn_node->list, &pwdn->free);

    /* update wdn */
    free_wdn_id(pwdn, pwdn_info->wdn_id);
    pwdn->cnt--;

    return 0;
}

int wf_wdn_init(nic_info_st *pnic_info)
{
    wdn_list *pwdn;
    wf_u8 i;
    wdn_node_st *pwdn_node;

    pwdn = wf_kzalloc(sizeof(wdn_list));
    if (NULL == pwdn)
    {
        WDN_WARN("malloc pwd failed");
        return -1;
    }

    wf_list_init(&pwdn->head);
    wf_list_init(&pwdn->free);
    pwdn->cnt = 0;
    pwdn->id_bitmap = 0x0;
    pnic_info->wdn = pwdn;

    /* add list node */
    for (i = 0; i < WDN_NUM_MAX; i++)
    {
        pwdn_node = wf_kzalloc(sizeof(wdn_node_st));
        if (NULL == pwdn_node)
        {
            WDN_WARN("wf_kzalloc pwdn_node failed, check!!!");
            return -2;
        }

        wf_list_insert_tail(&pwdn_node->list, &pwdn->free);
    }

    return 0;
}

int wf_wdn_term (nic_info_st *pnic_info)
{
    wdn_list *pwdn = pnic_info->wdn;
    wf_list_t *pos, *pos_next;
    wdn_node_st *pwdn_node;

    LOG_I("[%s] start",__func__);
    if (pwdn == NULL)
        return -1;

    wf_list_for_each_safe(pos, pos_next, &pwdn->head)
    {
        pwdn_node = wf_list_entry(pos, wdn_node_st, list);
        wf_list_delete(&pwdn_node->list);
        wf_kfree(pwdn_node);
    }

    wf_list_for_each_safe(pos, pos_next, &pwdn->free)
    {
        pwdn_node = wf_list_entry(pos, wdn_node_st, list);
        wf_list_delete(&pwdn_node->list);
        wf_kfree(pwdn_node);
    }

    wf_kfree(pwdn);

    return 0;
}

int wf_wdn_data_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info)
{
    wf_u8 i;
    wf_u8 *pele_start;
    wf_80211_mgmt_ie_t *pie;
    wf_wlan_mgmt_info_t *wlan_mgmt_info = nic_info->wlan_mgmt_info;
    wf_wlan_network_t *cur_network = &wlan_mgmt_info->cur_network;
    hw_info_st *hw_info = nic_info->hw_info;

    /* init defrag que */
    queue_initialize(&wdn_info->defrag_q);

    /* set mode */
    wdn_info->mode = wf_local_cfg_get_work_mode(nic_info);

    /* set ie info */
    pele_start = &cur_network->ies[0];

    /* set bssid */
    wf_memcpy(wdn_info->bssid, cur_network->bssid, MAC_ADDR_LEN);

    /* set ssid */
    wdn_info->ssid_len = (wf_u8)cur_network->ssid.length;

    /* set channel */
    wdn_info->channel = cur_network->channel;
    wdn_info->bw_mode = cur_network->bw;

    /* set bcn interval */
    wdn_info->bcn_interval = cur_network->bcn_interval;
    {
        wf_u64 tmp = cur_network->timestamp;
        wdn_info->tsf = cur_network->timestamp - wf_os_api_do_div(tmp, cur_network->bcn_interval * 1024) - 1024;
    }

    /* set listen interval */
    wdn_info->listen_interval = 3;

    /* set capability info */
    wf_ie_cap_info_update(nic_info, wdn_info, cur_network->cap_info);

    if (wf_80211_mgmt_ies_search(pele_start, (wf_u16)cur_network->ies_length, WF_80211_MGMT_EID_SSID, &pie) == WF_RETURN_OK)
    {
        wf_ie_ssid_update(nic_info, wdn_info, pie->data, pie->len);
    }
    else
    {
        return WF_RETURN_FAIL;
    }

    if (wf_80211_mgmt_ies_search(pele_start, (wf_u16)cur_network->ies_length, WF_80211_MGMT_EID_SUPP_RATES, &pie) == WF_RETURN_OK)
    {
        wf_ie_supported_rates_update(nic_info, wdn_info, pie->data, pie->len);
    }
    else
    {
        return WF_RETURN_FAIL;
    }

    if (wf_80211_mgmt_ies_search(pele_start, (wf_u16)cur_network->ies_length, WF_80211_MGMT_EID_EXT_SUPP_RATES, &pie) == WF_RETURN_OK)
    {
        wf_ie_extend_supported_rates_update(nic_info, wdn_info, pie->data, pie->len);
    }

    if ((wf_80211_mgmt_ies_search(pele_start, (wf_u16)cur_network->ies_length, WF_80211_MGMT_EID_HT_OPERATION, &pie) == WF_RETURN_OK)
        && hw_info->dot80211n_support)
    {
        WDN_DBG("HT Operation Info Parse");
        wf_ie_ht_operation_info_update(nic_info, wdn_info, pie->data, pie->len);
    }

    if ((wf_80211_mgmt_ies_search(pele_start, (wf_u16)cur_network->ies_length, WF_80211_MGMT_EID_HT_CAPABILITY, &pie) == WF_RETURN_OK)
        && hw_info->dot80211n_support)
    {
        WDN_DBG("HT Capability Info Parse");
        wf_ie_ht_capability_update(nic_info, wdn_info, pie->data, pie->len);
    }

    if (wf_80211_mgmt_ies_search(pele_start, (wf_u16)cur_network->ies_length, WF_80211_MGMT_EID_ERP_INFO, &pie) == WF_RETURN_OK)
    {
        WDN_DBG("ERP Info Parse");
        wf_ie_erp_update(nic_info, wdn_info, pie->data, pie->len);
    }

    if (wf_80211_mgmt_ies_search_with_oui(pele_start, (wf_u16)cur_network->ies_length, WF_80211_MGMT_EID_VENDOR_SPECIFIC, WMM_OUI, &pie) == WF_RETURN_OK)
    {
        WDN_DBG("WMM in IE [oui:%x-%x-%x-%x  len:%d]", pie->data[0], pie->data[1], pie->data[2], pie->data[3], pie->len);
        wf_ie_wmm_update(nic_info, wdn_info, pie->data, pie->len);
    }

    if (wf_80211_mgmt_ies_search_with_oui(pele_start, (wf_u16)cur_network->ies_length, WF_80211_MGMT_EID_VENDOR_SPECIFIC, WPA_OUI, &pie) == WF_RETURN_OK)
    {
        WDN_DBG("WPA in IE [oui:%x-%x-%x-%x  len:%d]", pie->data[0], pie->data[1], pie->data[2], pie->data[3], pie->len);
        wf_ie_wpa_update(nic_info, wdn_info, pie->data, pie->len);
    }

    if (wf_80211_mgmt_ies_search(pele_start, (wf_u16)cur_network->ies_length, WF_80211_MGMT_EID_RSN, &pie) == WF_RETURN_OK)
    {
        wf_ie_rsn_update(nic_info, wdn_info, pie->data, pie->len);
    }

    wdn_info->raid = wf_wdn_get_raid_by_network_type(wdn_info);

#if 0 // no change the rx agg param. commment by joy.huang
#ifdef CONFIG_SOFT_RX_AGGREGATION
    wf_mcu_set_usb_agg_normal(nic_info, wdn_info->network_type);
#endif
#endif

    wdn_info->ieee8021x_blocked = wf_true;

    /* add debug info */
#ifdef WDN_INFO_DUMP
#define _DUMP   WDN_INFO
    _DUMP("== WDN INFO DUMP ==");

    _DUMP("ID: %d", wdn_info->wdn_id);
    _DUMP("SSID: %s", wdn_info->ssid);
    _DUMP("short_slot: %d", wdn_info->short_slot);
    _DUMP("short_preamble: %d", wdn_info->short_preamble);
    _DUMP("bw_mode: %d", wdn_info->bw_mode);
    _DUMP("tsf: %lld", wdn_info->tsf);

    _DUMP("Supported Rates:");
    for (i = 0; i < wdn_info->datarate_len; i++)
    {
        _DUMP("rate_%d: 0x%x", i, wdn_info->datarate[i]);
    }
    _DUMP("Extend Supported Rates:");
    for (i = 0; i < wdn_info->ext_datarate_len; i++)
    {
        _DUMP("rate_%d: 0x%x", i, wdn_info->ext_datarate[i]);
    }


    if (wdn_info->network_type == WIRELESS_11B)
    {
        _DUMP("network_type: 802.11 B");
    }
    else if (wdn_info->network_type == WIRELESS_11G)
    {
        _DUMP("network_type: 802.11 G");
    }
    else if (wdn_info->network_type == WIRELESS_11BG)
    {
        _DUMP("network_type: 802.11 BG");
    }
    else if (wdn_info->network_type == WIRELESS_11G_24N)
    {
        _DUMP("network_type: 802.11 GN");
    }
    else if (wdn_info->network_type == WIRELESS_11B_24N)
    {
        _DUMP("network_type: 802.11 BN");
    }
    else if (wdn_info->network_type == WIRELESS_11BG_24N)
    {
        _DUMP("network_type: 802.11 BGN");
    }

    _DUMP("User Rate ID: %d", wdn_info->raid);

    if (wdn_info->auth_algo == 0)
    {
        _DUMP("OPEN SYSTEM");
    }
//    else
    {
        wf_mcu_set_preamble(nic_info, PREAMBLE_LONG);
        if (wdn_info->wep_enable)
        {
            _DUMP("WEP");
        }

        if (wdn_info->wpa_enable)
        {
            _DUMP("WPA");
        }

        if (wdn_info->rsn_enable)
        {
            _DUMP("WPA2");
        }
    }

    _DUMP("ampdu_max_len: %d", wdn_info->htpriv.rx_ampdu_maxlen);
    _DUMP("ampdu_min_spacing: %d", wdn_info->htpriv.rx_ampdu_min_spacing);
#undef _DUMP
#endif

    return WF_RETURN_OK;
}


#define wdn_update_last_rx_pkts(wdn_stats) \
do { \
    wdn_stats.last_rx_mgnt_pkts = wdn_stats.rx_mgnt_pkts; \
    wdn_stats.last_rx_beacon_pkts = wdn_stats.rx_beacon_pkts; \
    wdn_stats.last_rx_probereq_pkts = wdn_stats.rx_probereq_pkts; \
    wdn_stats.last_rx_probersp_pkts = wdn_stats.rx_probersp_pkts; \
    wdn_stats.last_rx_probersp_bm_pkts = wdn_stats.rx_probersp_bm_pkts; \
    wdn_stats.last_rx_probersp_uo_pkts = wdn_stats.rx_probersp_uo_pkts; \
    wdn_stats.last_rx_ctrl_pkts = wdn_stats.rx_ctrl_pkts; \
    wdn_stats.last_rx_data_pkts = wdn_stats.rx_data_pkts; \
} while(0)

wf_u8 wf_wdn_is_alive(wdn_net_info_st *wdn_net_info, wf_u8 update_tag)
{
    if ( (wdn_net_info->wdn_stats.last_rx_data_pkts + wdn_net_info->wdn_stats.last_rx_ctrl_pkts) ==
         (wdn_net_info->wdn_stats.rx_data_pkts + wdn_net_info->wdn_stats.rx_ctrl_pkts))
    {
        return wf_false;
    }

    if (update_tag)
    {
        wdn_update_last_rx_pkts(wdn_net_info->wdn_stats);
    }

    return wf_true;
}



wf_u8 wf_wdn_get_cnt(nic_info_st *pnic_info)
{
    wdn_list *pwdn = NULL;

    if(NULL == pnic_info)
    {
        return 0;
    }

    pwdn = pnic_info->wdn;
    if(NULL == pwdn)
    {
        return 0;
    }

    return pwdn->cnt;
}
