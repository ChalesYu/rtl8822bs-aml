#include "common.h"
#include "wf_debug.h"

#ifdef CONFIG_LPS

#if 0
#define LPS_DBG(fmt, ...)      LOG_D("[%s]"fmt, __func__, ##__VA_ARGS__)
#define LPS_ARRAY(data, len)   log_array(data, len)
#define LPS_WARN(fmt, ...)     LOG_E("[%s]"fmt, __func__, ##__VA_ARGS__)
#else
#define LPS_DBG(fmt, ...)
#define LPS_ARRAY(data, len)
#define LPS_WARN(fmt, ...)
#endif
#define LPS_INFO(fmt, ...)     LOG_I("[%s]"fmt, __func__, ##__VA_ARGS__)


/***************************************************************
    Const Define
***************************************************************/
const char LPS_CTRL_TYPE_STR[13][40] =
{
    "LPS_CTRL_SCAN",
    "LPS_CTRL_JOINBSS",
    "LPS_CTRL_CONNECT",
    "LPS_CTRL_DISCONNECT",
    "LPS_CTRL_SPECIAL_PACKET",
    "LPS_CTRL_LEAVE",
    "LPS_CTRL_TRAFFIC_BUSY",
    "LPS_CTRL_TX_TRAFFIC_LEAVE",
    "LPS_CTRL_RX_TRAFFIC_LEAVE",
    "LPS_CTRL_ENTER",
    "LPS_CTRL_LEAVE_CFG80211_PWRMGMT",
    "LPS_CTRL_NO_LINKED",
    "LPS_CTRL_MAX"
};


/***************************************************************
    Static Function
***************************************************************/
static wf_u8* st_query_data_from_ie(wf_u8 * ie, wf_u8 type)
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

static wf_u8 * st_ie_to_set_func(wf_u8 * pbuf, int index, wf_u32 len, wf_u8 * source, wf_u32 * frlen)
{
    *pbuf = (wf_u8) index;

    *(pbuf + 1) = (wf_u8) len;

    if (len > 0)
        wf_memcpy((void *)(pbuf + 2), (void *)source, len);

    *frlen = *frlen + (len + 2);

    return (pbuf + len + 2);
}

static void st_rsvd_page_chip_hw_construct_beacon(nic_info_st *pnic_info, wf_u8 *frame_index_ptr, wf_u32 *length_out)
{
    struct wl_ieee80211_hdr *wlan_hdr_ptr;
    wdn_net_info_st *pwdn =  wf_wdn_find_info(pnic_info,wf_wlan_get_cur_bssid(pnic_info));
    wf_u16 *frame_control; // Mac header (1)
    wf_u8 bc_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    hw_info_st *hw_info_ptr =(hw_info_st *) pnic_info->hw_info;
    wf_u32 pkt_len = 0;
    wf_wlan_info_t *wlan_info_ptr = (wf_wlan_info_t *)pnic_info->wlan_info;
    wf_wlan_network_t *cur_network_ptr = & (wlan_info_ptr->cur_network);
    mlme_info_t *mlme_info_ptr = pnic_info->mlme_info;

    LPS_DBG();
    if (pwdn == NULL)
    {
        LPS_WARN("Not find wdn");
        return;
    }

    wlan_hdr_ptr = (struct wl_ieee80211_hdr *)frame_index_ptr;
    frame_control = &(wlan_hdr_ptr->frame_ctl);
    *frame_control = 0; // Clear 0
    wf_memcpy(wlan_hdr_ptr->addr1, bc_addr, ETH_ADDRESS_LEN);
    wf_memcpy(wlan_hdr_ptr->addr2, hw_info_ptr->macAddr, ETH_ADDRESS_LEN);
    wf_memcpy(wlan_hdr_ptr->addr3, wf_wlan_get_cur_bssid(pnic_info), ETH_ADDRESS_LEN);
    SetSeqNum(wlan_hdr_ptr, 0); // Set Sequence Control field
    SetFrameType(frame_index_ptr, WIFI_BEACON); // Set Frame Type field

    frame_index_ptr = frame_index_ptr + sizeof(struct wl_ieee80211_hdr_3addr);
    pkt_len = pkt_len + sizeof(struct wl_ieee80211_hdr_3addr);

    frame_index_ptr = frame_index_ptr + 8;
    pkt_len = pkt_len + 8;

    wf_memcpy(frame_index_ptr, (wf_u8 *)st_query_data_from_ie(cur_network_ptr->ies, BCN_INTERVAL), 2);
    frame_index_ptr = frame_index_ptr + 2;
    pkt_len = pkt_len + 2;

    wf_memcpy(frame_index_ptr, (wf_u8 *)st_query_data_from_ie(cur_network_ptr->ies, CAPABILITY), 2);
    frame_index_ptr = frame_index_ptr + 2;
    pkt_len = pkt_len + 2;
    // LPS_DBG(" pkt_len: %d", pkt_len);
    // if ((mlme_info_ptr->state & 0x03) == WIFI_FW_AP_STATE) // ???????
    // {
    //     pkt_len = pkt_len + cur_network_ptr->ies_length - sizeof(NDIS_802_11_FIXED_IEs);
    //     wf_memcpy(frame_index_ptr, cur_network_ptr->ies + sizeof(NDIS_802_11_FIXED_IEs), pkt_len);

    //     goto _ConstructBeacon;
    // }
    // LPS_DBG(" pkt_len: %d", pkt_len);
    frame_index_ptr = st_ie_to_set_func(frame_index_ptr, _SSID_IE_, cur_network_ptr->ssid.length,
                                        cur_network_ptr->ssid.data, &pkt_len);
    frame_index_ptr= st_ie_to_set_func(frame_index_ptr, _SUPPORTEDRATES_IE_,
                                       ((pwdn->datarate_len > 8) ? 8 : pwdn->datarate_len),
                                        pwdn->datarate, &pkt_len); // cur_network->SupportedRates
    frame_index_ptr= st_ie_to_set_func(frame_index_ptr, _DSSET_IE_, 1,
                                       (wf_u8 *)&cur_network_ptr->channel, &pkt_len); // Configuration.DSConfig
    if (pwdn->ext_datarate_len > 0)
    {
        frame_index_ptr= st_ie_to_set_func(frame_index_ptr, _EXT_SUPPORTEDRATES_IE_,
                                           pwdn->ext_datarate_len,
                                           pwdn->ext_datarate, &pkt_len);
    }

_ConstructBeacon:
    if (pkt_len + TXDESC_SIZE > 512)
    {
        LPS_DBG("Beacon frame too large: %d", pkt_len);
        return;
    }

    *length_out = pkt_len; // Output packet length
}

static void st_rsvd_page_chip_hw_construct_pspoll(nic_info_st *pnic_info, wf_u8 *frame_index_ptr, wf_u32 *length_out)
{
    struct wl_ieee80211_hdr *wlan_hdr_ptr;
    wf_u32 pkt_len = 0;
    wf_u16 *frame_control; // Mac header (1)
    wf_wlan_info_t *wlan_info_ptr = (wf_wlan_info_t *)pnic_info->wlan_info;
    wf_wlan_network_t *cur_network_ptr = &(wlan_info_ptr->cur_network);
    hw_info_st *hw_info_ptr =(hw_info_st *) pnic_info->hw_info;
    LPS_DBG();

    wlan_hdr_ptr = (struct wl_ieee80211_hdr *)frame_index_ptr;
    frame_control = &wlan_hdr_ptr->frame_ctl;
    *frame_control = 0; // Clear 0

    SetPwrMgt(frame_control); // Set Power Management bit
    SetFrameSubType(frame_index_ptr, WIFI_PSPOLL); // Set SubType in Frame Control field
    SetDuration(frame_index_ptr, cur_network_ptr->aid | 0xC000); // Set Duration/ID field

    wf_memcpy(wlan_hdr_ptr->addr1, wf_wlan_get_cur_bssid(pnic_info), ETH_ADDRESS_LEN);
    wf_memcpy(wlan_hdr_ptr->addr2, hw_info_ptr->macAddr, ETH_ADDRESS_LEN);

    *length_out = 16; // Output packet length
}

static void st_rsvd_page_chip_hw_construct_nullfunctiondata(nic_info_st *pnic_info, wf_u8 *frame_index_ptr, wf_u32 *length_out,
                                                            wf_u8 *addr_start_ptr, wf_bool b_qos, wf_u8 ac, wf_u8 eosp, wf_bool b_force_power_save)
{
    struct wl_ieee80211_hdr *wlan_hdr_ptr;
    wf_u16 *frame_control; // Mac header (1)
    wf_u32 pkt_len = 0;
    hw_info_st *hw_info_ptr =(hw_info_st *) pnic_info->hw_info;

    wlan_hdr_ptr = (struct wl_ieee80211_hdr *)frame_index_ptr;
    frame_control = &wlan_hdr_ptr->frame_ctl;
    *frame_control = 0; // Clear 0

    LPS_DBG();
    if (b_force_power_save == wf_true)
    {
        SetPwrMgt(frame_control);
    }
    switch (wf_local_cfg_get_work_mode(pnic_info)) // In nic local_info
    {
        case WF_INFRA_MODE:
            SetToDs(frame_control);
            wf_memcpy(wlan_hdr_ptr->addr1, wf_wlan_get_cur_bssid(pnic_info), ETH_ADDRESS_LEN);
            wf_memcpy(wlan_hdr_ptr->addr2, hw_info_ptr->macAddr, ETH_ADDRESS_LEN);
            wf_memcpy(wlan_hdr_ptr->addr3, addr_start_ptr, ETH_ADDRESS_LEN);
            break;
        case WF_MASTER_MODE:
            SetFrDs(frame_control);
            wf_memcpy(wlan_hdr_ptr->addr1, addr_start_ptr, ETH_ADDRESS_LEN);
            wf_memcpy(wlan_hdr_ptr->addr2, wf_wlan_get_cur_bssid(pnic_info), ETH_ADDRESS_LEN);
            wf_memcpy(wlan_hdr_ptr->addr3, hw_info_ptr->macAddr, ETH_ADDRESS_LEN);
            break;
        case WF_ADHOC_MODE:
        default:
            wf_memcpy(wlan_hdr_ptr->addr1, addr_start_ptr, ETH_ADDRESS_LEN);
            wf_memcpy(wlan_hdr_ptr->addr2, hw_info_ptr->macAddr, ETH_ADDRESS_LEN);
            wf_memcpy(wlan_hdr_ptr->addr3, wf_wlan_get_cur_bssid(pnic_info), ETH_ADDRESS_LEN);
            break;
    }
    SetSeqNum(wlan_hdr_ptr, 0);

    if (b_qos == wf_true)
    {
        wf_80211_qos_hdr_t *wlan_qos_hdr_ptr;

        SetFrameSubType(frame_index_ptr, WIFI_QOS_DATA_NULL);

        wlan_qos_hdr_ptr = (wf_80211_qos_hdr_t *)frame_index_ptr;
        SetPriority(&wlan_qos_hdr_ptr->qos_ctrl, ac);
        SetEOSP(&wlan_qos_hdr_ptr->qos_ctrl, eosp);

        pkt_len = sizeof(wf_80211_qos_hdr_t);
    }
    else
    {
        SetFrameSubType(frame_index_ptr, WIFI_DATA_NULL);

        pkt_len = sizeof(wf_80211_hdr_3addr_t);
    }

    *length_out = pkt_len;
}

static void st_fill_fake_txdesc(nic_info_st *pnic_info, wf_u8 *tx_des_start_addr, wf_u32 pkt_len,
                                      wf_bool is_ps_poll, wf_bool is_bt_qos_null, wf_bool is_dataframe)
{
    LPS_DBG();
    wf_memset(tx_des_start_addr, 0, TXDESC_SIZE);
    SET_TX_DESC_FIRST_SEG_9086X(tx_des_start_addr, 1); // bFirstSeg
    SET_TX_DESC_LAST_SEG_9086X(tx_des_start_addr, 1);  // bLastSeg
    SET_TX_DESC_OFFSET_9086X(tx_des_start_addr, 0X28); // Offset = 32
    SET_TX_DESC_PKT_SIZE_9086X(tx_des_start_addr, pkt_len);  // Buffer size + command header
    SET_TX_DESC_QUEUE_SEL_9086X(tx_des_start_addr, QSLT_MGNT); // Fixed queue of Mgnt queue

    /* Set NAVUSEHDR to prevent Ps-poll AId filed to be changed to error vlaue by Hw. */
    if (is_ps_poll == wf_true)
    {
        SET_TX_DESC_NAV_USE_HDR_9086X(tx_des_start_addr, 1);
    }
    else
    {
        SET_TX_DESC_HWSEQ_EN_9086X(tx_des_start_addr, 1); // Hw set sequence number
        SET_TX_DESC_HWSEQ_SEL_9086X(tx_des_start_addr, 0);
    }

    if (is_bt_qos_null == wf_true)
    {
        SET_TX_DESC_BT_INT_9086X(tx_des_start_addr, 1);
    }
    SET_TX_DESC_USE_RATE_9086X(tx_des_start_addr, 1); /* use data rate which is set by Sw */
    SET_TX_DESC_OWN_9086X(tx_des_start_addr, 1);

    SET_TX_DESC_TX_RATE_9086X(tx_des_start_addr, DESC_RATE1M);

    /* Encrypt the data frame if under security mode excepct null data. Suggested by CCW. */
    if (is_dataframe == wf_true)
    {
        wf_u32 EncAlg;
        sec_info_st *sec_info = pnic_info->sec_info;

        EncAlg = sec_info->dot11PrivacyAlgrthm;
        switch (EncAlg) {
        case _NO_PRIVACY_:
            SET_TX_DESC_SEC_TYPE_9086X(tx_des_start_addr, 0x0);
            break;
        case _WEP40_:
        case _WEP104_:
        case _TKIP_:
            SET_TX_DESC_SEC_TYPE_9086X(tx_des_start_addr, 0x1);
            break;
        case _SMS4_:
            SET_TX_DESC_SEC_TYPE_9086X(tx_des_start_addr, 0x2);
            break;
        case _AES_:
            SET_TX_DESC_SEC_TYPE_9086X(tx_des_start_addr, 0x3);
            break;
        default:
            SET_TX_DESC_SEC_TYPE_9086X(tx_des_start_addr, 0x0);
            break;
        }
    }

    wf_txdesc_chksum((struct tx_desc *)tx_des_start_addr);
}

static wf_bool st_mpdu_send_complete_cb(nic_info_st *nic_info, struct xmit_buf *pxmitbuf)
{
    tx_info_st *tx_info = nic_info->tx_info;

    wf_xmit_buf_delete(tx_info, pxmitbuf);

    tx_work_wake(nic_info->ndev);

    return wf_true;
}

static void st_rsvd_page_mgntframe_attrib_update(nic_info_st * pnic_info, struct pkt_attrib *pattrib)
{
    wdn_net_info_st *wdn_net_info_ptr;
    tx_info_st *tx_info = pnic_info->tx_info;
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    LPS_DBG();

    pattrib->hdrlen = WLAN_HDR_A3_LEN;
    pattrib->nr_frags = 1;
    pattrib->priority = 7;

    if (is_bcast_addr(pcur_network->mac_addr))
    {
        pattrib->mac_id = wdn_net_info_ptr->wdn_id;
    }
    else
    {
        pattrib->mac_id = 0;
        LPS_DBG(" mgmt use mac_id 0 will affect RA");
    }

    wdn_net_info_ptr =  wf_wdn_find_info(pnic_info,wf_wlan_get_cur_bssid(pnic_info));
    pattrib->qsel = QSLT_MGNT;

    pattrib->raid = (wdn_net_info_ptr->tx_rate == MGN_1M) ? RATEID_IDX_B : RATEID_IDX_G;
    pattrib->rate = wdn_net_info_ptr->tx_rate;
    pattrib->encrypt_algo = _NO_PRIVACY_;

    pattrib->qos_en = wf_false;
    pattrib->ht_en = wf_false;
    pattrib->bwmode = CHANNEL_WIDTH_20;
    pattrib->ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
    pattrib->sgi = wf_false;
    pattrib->seqnum = wdn_net_info_ptr->wdn_xmitpriv.txseq_tid[QSLT_MGNT];

    pattrib->retry_ctrl = wf_true;

    pattrib->mbssid = 0;
    pattrib->hw_ssn_sel = tx_info->hw_ssn_seq_no;
}
static wf_bool st_mpdu_insert_sending_queue(nic_info_st *nic_info, struct xmit_frame *pxmitframe, wf_bool ack)
{
    wf_u8 val;
    wf_u32 curTime,endTime,timeout;
    wf_u8 *mem_addr;
    wf_u32 ff_hwaddr;
    wf_bool bRet = wf_true;
    int ret;
    wf_bool inner_ret = wf_true;
    wf_bool blast = wf_false;
    int t, sz, w_sz, pull = 0;
    struct xmit_buf *pxmitbuf = pxmitframe->pxmitbuf;
    struct pkt_attrib *pattrib = &pxmitframe->attrib;
    hw_info_st *hw_info = nic_info->hw_info;
    tx_info_st *tx_info = nic_info->tx_info;
    sec_info_st *sec_info = nic_info->sec_info;
    mlme_state_e state;
    wf_u32  txlen = 0;

    LPS_DBG();
    mem_addr = pxmitframe->buf_addr;

    for (t = 0; t < pattrib->nr_frags; t++)
    {
        if (inner_ret != wf_true && ret == wf_true)
            ret = wf_false;

        if (t != (pattrib->nr_frags - 1))
        {
            LPS_DBG("pattrib->nr_frags=%d\n", pattrib->nr_frags);
            sz = hw_info->frag_thresh;
            sz = sz - 4 - 0; /* 4: wlan head filed????????? */
        }
        else
        {
            /* no frag */
            blast = wf_true;
            sz = pattrib->last_txcmdsz;
        }

        pull = wf_tx_txdesc_init(pxmitframe, mem_addr, sz, wf_false, 1);
        if (pull)
        {
            mem_addr += PACKET_OFFSET_SZ; /* pull txdesc head */
            pxmitframe->buf_addr = mem_addr;
            w_sz = sz + TXDESC_SIZE;
        }
        else
        {
            w_sz = sz + TXDESC_SIZE + PACKET_OFFSET_SZ;
        }

        if (wf_sec_encrypt(pxmitframe, mem_addr, w_sz))
        {
            ret = wf_false;
            LPS_WARN("encrypt fail!!!!!!!!!!!");
        }
        ff_hwaddr = wf_quary_addr(pattrib->qsel);

        txlen = TXDESC_SIZE + pxmitframe->attrib.last_txcmdsz;
        pxmitbuf->pg_num   += (txlen+127)/128;
        wf_timer_set(&pxmitbuf->time, 0);

        if(blast)
        {
            ret = wf_io_write_data(nic_info, 1, mem_addr, w_sz,
                                          ff_hwaddr,(void *)st_mpdu_send_complete_cb, nic_info, pxmitbuf);
        }
        else
        {
            ret = wf_io_write_data(nic_info, 1, mem_addr, w_sz,
                                          ff_hwaddr, NULL, nic_info, pxmitbuf);
        }

        if (WF_RETURN_FAIL == ret)
        {
            bRet = wf_false;
            break;
        }

        wf_tx_stats_cnt(nic_info, pxmitframe, sz);

        mem_addr += w_sz;
        mem_addr = (wf_u8 *) WF_RND4(((SIZE_PTR) (mem_addr)));
    }

    return bRet;
}
static wf_s32 st_chip_hw_mgnt_xmit(nic_info_st * pnic_info, struct xmit_frame * mgnt_frame_ptr)
{
    wf_s32 ret = wf_false;
    wf_u8 *pframe = (wf_u8 *) (mgnt_frame_ptr->buf_addr) + TXDESC_OFFSET;
    struct pkt_attrib *pattrib = &mgnt_frame_ptr->attrib;
    LPS_DBG();

    wf_memcpy(pattrib->ra, GetAddr1Ptr(pframe), ETH_ADDRESS_LEN);
    wf_memcpy(pattrib->ta, GetAddr2Ptr(pframe), ETH_ADDRESS_LEN);

    ret = st_mpdu_insert_sending_queue(pnic_info, mgnt_frame_ptr, wf_true);

    return ret;
}

static wf_s32 st_mgntframe_xmit(nic_info_st * pnic_info, struct xmit_frame *mgnt_frame_ptr)
{
    wf_s32 ret = wf_false;
    LPS_DBG();

    if ((pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped))
    {
        wf_xmit_buf_delete(pnic_info->tx_info, mgnt_frame_ptr->pxmitbuf);
        wf_xmit_frame_delete(pnic_info->tx_info, mgnt_frame_ptr);
        LPS_DBG(" fail: pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped");
        return ret;
    }

    ret = st_chip_hw_mgnt_xmit(pnic_info, mgnt_frame_ptr); // s_tx_mgnt
    return ret;
}

static wf_s32 st_rsvd_page_h2c_loc_set(nic_info_st * nic_info, PRSVDPAGE_LOC rsvdpageloc)
{
    wf_u8 u1wMBOX1RsvdPageParm[wMBOX1_RSVDPAGE_LOC_LEN] = { 0 };
    int ret = 0;
    LPS_DBG();

    SET_wMBOX1CMD_RSVDPAGE_LOC_PROBE_RSP(u1wMBOX1RsvdPageParm, rsvdpageloc->LocProbeRsp);
    SET_wMBOX1CMD_RSVDPAGE_LOC_PSPOLL(u1wMBOX1RsvdPageParm, rsvdpageloc->LocPsPoll);
    SET_wMBOX1CMD_RSVDPAGE_LOC_NULL_DATA(u1wMBOX1RsvdPageParm, rsvdpageloc->LocNullData);
    SET_wMBOX1CMD_RSVDPAGE_LOC_QOS_NULL_DATA(u1wMBOX1RsvdPageParm, rsvdpageloc->LocQosNull);
    SET_wMBOX1CMD_RSVDPAGE_LOC_BT_QOS_NULL_DATA(u1wMBOX1RsvdPageParm, rsvdpageloc->LocBTQosNull);

#if 0 //CHIP51_DIRECT_ACCESS
    ret = FillH2CCmd(nic_info,wMBOX1_9086X_RSVD_PAGE,wMBOX1_RSVDPAGE_LOC_LEN,u1wMBOX1RsvdPageParm);
#else
    ret = wf_mcu_fill_mbox1_fw(nic_info, wMBOX1_9086X_RSVD_PAGE, u1wMBOX1RsvdPageParm ,wMBOX1_RSVDPAGE_LOC_LEN);
#endif

    return ret;
}

static void st_hal_set_fw_rsvd_page(nic_info_st *pnic_info)
{
    wf_u8 rsvd_page_num = 0;
    wf_u32 max_rsvd_page_buff_size = 0;
    wf_u32 page_size = 128;// Unit byte
    struct xmit_frame *cmd_frame_ptr;
    struct pkt_attrib * attrib_ptr;
    wf_u8 *reserved_page_packet; // Unit byte
    RSVDPAGE_LOC rsvd_page_loc;
    wf_u16 buff_index = 0; // Unit byte
    wf_u32 beacon_length = 0;
    wf_u8 current_packet_page_num = 0;
    wf_u8 total_page_number = 0;
    wf_u32 ps_poll_length = 0;
    wf_u32 null_data_length = 0;
    wf_u32 qos_null_length = 0;
    wf_u32 total_packets_len = 0;
    wf_bool b_connect = wf_false;
    int ret = 0;
    LPS_DBG();

    rsvd_page_num = 255 - TX_PAGE_BOUNDARY_9086X + 1;
    LPS_DBG(" Page size: %d, rsvd page num: %d", page_size, rsvd_page_num);
    max_rsvd_page_buff_size = rsvd_page_num * page_size;
    LPS_DBG(" max_rsvd_page_buff_size: %d", max_rsvd_page_buff_size);
    if (max_rsvd_page_buff_size > MAX_CMDBUF_SZ)
    {
        LPS_DBG("max_rsvd_page_buff_size(%d) is larger than MAX_CMDBUF_SZ(%d)\r\n",
                max_rsvd_page_buff_size, MAX_CMDBUF_SZ);
    }
    // alloc memory for cmd frame
    cmd_frame_ptr = wf_xmit_cmdframe_new(pnic_info->tx_info, CMDBUF_RSVD, 1);
    if (cmd_frame_ptr == NULL)
    {
        LPS_DBG("Alloc reserved page packet fail!");
        return;
    }
    reserved_page_packet = cmd_frame_ptr->buf_addr;
    wf_memset(&rsvd_page_loc, 0, sizeof(RSVDPAGE_LOC));

    // beacon * 2 pages
    buff_index = TXDESC_OFFSET;
    st_rsvd_page_chip_hw_construct_beacon(pnic_info, &reserved_page_packet[buff_index], &beacon_length);
    /*
    * When we count the first page size, we need to reserve description size for the RSVD
    * packet, it will be filled in front of the packet in TXPKTBUF.
    */
    current_packet_page_num = (wf_u8)PageNum(TXDESC_SIZE + beacon_length, page_size);
    // If we don't add 1 more page, ARP offload function will fail at 8723bs
    if (current_packet_page_num == 1)
    {
        current_packet_page_num++;
    }
    total_page_number = total_page_number + current_packet_page_num;
    buff_index = buff_index + current_packet_page_num * page_size;

    // ps-poll * 1 page
    rsvd_page_loc.LocPsPoll = total_page_number;// ?????????
    st_rsvd_page_chip_hw_construct_pspoll(pnic_info, &reserved_page_packet[buff_index], &ps_poll_length);
    st_fill_fake_txdesc(pnic_info, &reserved_page_packet[buff_index - TXDESC_SIZE],
                        ps_poll_length, wf_true, wf_false, wf_false); // ???????
    current_packet_page_num = (wf_u8)PageNum(TXDESC_SIZE + ps_poll_length, page_size);
    total_page_number = total_page_number + current_packet_page_num;
    buff_index = buff_index + current_packet_page_num * page_size;

    // null data * 1 page
    rsvd_page_loc.LocNullData = total_page_number;// ????????
    st_rsvd_page_chip_hw_construct_nullfunctiondata(pnic_info, &reserved_page_packet[buff_index],
                                                    &null_data_length, wf_wlan_get_cur_bssid(pnic_info),
                                                    wf_false, 0, 0, wf_false);
    st_fill_fake_txdesc(pnic_info, &reserved_page_packet[buff_index - TXDESC_SIZE],
                        null_data_length, wf_false, wf_false, wf_false);
    current_packet_page_num = (wf_u8)PageNum(null_data_length + TXDESC_SIZE, page_size);
    total_page_number = total_page_number + current_packet_page_num;
    buff_index = buff_index + current_packet_page_num * page_size;

    // Qos null data * 1 page
    rsvd_page_loc.LocQosNull = total_page_number;// ???????
    st_rsvd_page_chip_hw_construct_nullfunctiondata(pnic_info, &reserved_page_packet[buff_index],
                                                    &qos_null_length, wf_wlan_get_cur_bssid(pnic_info),
                                                    wf_true, 0, 0, wf_false);
    st_fill_fake_txdesc(pnic_info, &reserved_page_packet[buff_index - TXDESC_SIZE],
                        qos_null_length, wf_false, wf_false, wf_false);
    current_packet_page_num = (wf_u8)PageNum(qos_null_length + TXDESC_SIZE, page_size);
    total_page_number = total_page_number + current_packet_page_num;
    total_packets_len = buff_index + qos_null_length; // Do not contain TXDESC_SIZE of next packet
    buff_index = buff_index + current_packet_page_num * page_size;

download_page:
    if (total_packets_len > max_rsvd_page_buff_size)
    {
        LPS_DBG(" Rsvd page size is not enough! total_packets_len: %d, max_rsvd_page_buff_size: %d", total_packets_len, max_rsvd_page_buff_size);
    }
    else
    {
        // update attribute
        attrib_ptr = &cmd_frame_ptr->attrib;
        st_rsvd_page_mgntframe_attrib_update(pnic_info, attrib_ptr);
        attrib_ptr->qsel = QSLT_BEACON;
        attrib_ptr->pktlen = total_packets_len - TXDESC_OFFSET;       // ???????
        attrib_ptr->last_txcmdsz = total_packets_len - TXDESC_OFFSET; // ???????


        ret = st_mgntframe_xmit(pnic_info, cmd_frame_ptr);
        if (ret == wf_false)
        {
            LPS_DBG(" fail: st_mgntframe_xmit: %d", ret);
        }
    }

    printk("Set RSVD page location to FW, total packet len: %d, total page num: %d\r\n",
           total_packets_len, total_page_number);
    LPS_DBG(" ProbeRsp: %d, PsPoll: %d, NullData: %d, QosNull: %d, BTNull: %d\r\n",
           rsvd_page_loc.LocProbeRsp, rsvd_page_loc.LocPsPoll, rsvd_page_loc.LocNullData, rsvd_page_loc.LocQosNull, rsvd_page_loc.LocBTQosNull);

    wf_mlme_get_connect(pnic_info, &b_connect);

    if (b_connect == wf_true)
    {
        if (st_rsvd_page_h2c_loc_set(pnic_info, &rsvd_page_loc) == WF_RETURN_FAIL)
        {
            LPS_WARN(" fail: st_rsvd_page_h2c_loc_set");
        }
    }
}

static void st_set_fw_join_bss_rpt_cmd(nic_info_st *pnic_info, wf_u8 in_value)
{
    pwr_info_st *pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;
    wf_u8 mstatus = in_value;
    wf_u8 dlbcn_count = 0;
    wf_u8 poll = 0;
    wf_bool b_cn_valid = wf_false;
    wf_s32 ret = WF_RETURN_OK;
    LPS_DBG();

    if (mstatus == 1)
    {
        wf_mcu_set_fw_lps_config(pnic_info);
        do
        {
            st_hal_set_fw_rsvd_page(pnic_info);
            dlbcn_count++;
            do
            {
                wf_yield();
                st_get_lps_hw_reg(pnic_info, HW_VAR_BCN_VALID, (wf_u8 *)&b_cn_valid);
                LPS_DBG(" b_cn_valid: %d", b_cn_valid);
                poll++;
            }while(b_cn_valid == wf_false && (poll % 10) != 0 &&
                   !((pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped)));

        }while(b_cn_valid == wf_false && dlbcn_count <= 100 &&
               !((pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped)));

        if ((pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped))
        {
            printk("Error: surprise removed or driver stopped!\r\n");
        }
        else if (b_cn_valid == wf_false)
        {
            printk("Error: RSVD page failed! dlbcn_count: %u, poll: %u\r\n", dlbcn_count, poll);
        }
        else
        {
            printk("RSVD page success! dlbcn_count: %u, poll: %u\r\n", dlbcn_count, poll);
        }
        // 9083 source code
//        if(pHalData->RegFwHwTxQCtrl & BIT(6))
//            poll = wf_true;
//        else
//            poll = wf_false;

        poll = wf_false;
        if (NIC_USB == pnic_info->nic_type)
        {
            ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_LPS_GET, (wf_u32 *)&poll, 1, NULL, 0);
        }
        else
        {
            ret = mcu_cmd_communicate(pnic_info, WLAN_OPS_DXX0_HAL_LPS_GET, (wf_u32 *)&poll, 1, NULL, 0);
        }

        if (WF_RETURN_FAIL == ret)
        {
            LPS_WARN("[%s] UMSG_OPS_HAL_LPS_GET[%d] failed, check!!!!", __func__, poll);
        }
    }
}

static void st_set_fw_power_mode(nic_info_st *pnic_info, wf_u8 lps_mode)
{
    int i;
    wf_u8 smart_ps = 0;
    wf_u8 u1wMBOX1PwrModeParm[wMBOX1_PWRMODE_LEN] = { 0 };
    wf_u8 power_state = 0;
    wf_u8 awake_intvl = 1;
    wf_u8 byte5 = 0;
    wf_u8 rlbm = 0;
    pwr_info_st *pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;

    LPS_DBG();
    pwr_info_ptr->b_mailbox_sync = wf_true;

    if (lps_mode == PS_MODE_MIN)
    {
        rlbm = 0;
        awake_intvl = 2;
        smart_ps = pwr_info_ptr->smart_lps;
    }
    else if (lps_mode == PS_MODE_MAX)
    {
        rlbm = 1;
        awake_intvl = 2;
        smart_ps = pwr_info_ptr->smart_lps;
    }
    else if (lps_mode == PS_MODE_DTIM)
    {
        rlbm = 2;
        awake_intvl = 4;
        smart_ps = pwr_info_ptr->smart_lps;
    }
    else
    {
        rlbm = 2;
        awake_intvl = 4;
        smart_ps = pwr_info_ptr->smart_lps;
    }
    if (lps_mode > 0)
    {
        power_state = 0x00;
        byte5 = 0x40;
    }
    else
    {
        power_state = 0x0C;
        byte5 = 0x40;
    }

    SET_9086X_wMBOX1CMD_PWRMODE_PARM_MODE(u1wMBOX1PwrModeParm, (lps_mode > 0) ? 1 : 0);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_SMART_PS(u1wMBOX1PwrModeParm, smart_ps);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_RLBM(u1wMBOX1PwrModeParm, rlbm);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_BCN_PASS_TIME(u1wMBOX1PwrModeParm, awake_intvl);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_ALL_QUEUE_UAPSD(u1wMBOX1PwrModeParm, 0); //pnic_info->registrypriv.uapsd_enable
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_PWR_STATE(u1wMBOX1PwrModeParm, power_state);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_BYTE5(u1wMBOX1PwrModeParm, byte5);

#if 0 //CHIP51_DIRECT_ACCESS
    FillH2CCmd(pnic_info,wMBOX1_9086X_SET_PWR_MODE, wMBOX1_PWRMODE_LEN, u1wMBOX1PwrModeParm);
#else
	wf_mcu_fill_mbox1_fw(pnic_info, wMBOX1_9086X_SET_PWR_MODE, u1wMBOX1PwrModeParm, wMBOX1_PWRMODE_LEN);
#endif
	pwr_info_ptr->b_mailbox_sync = wf_false;
}

static void st_set_lps_hw_reg(nic_info_st *pnic_info, wf_u8 type, wf_u8 in_value)
{
    switch(type)
    {
        case HW_VAR_wMBOX1_FW_JOINBSSRPT:
            st_set_fw_join_bss_rpt_cmd(pnic_info, in_value);
            break;
        case HW_VAR_wMBOX1_FW_PWRMODE:
            st_set_fw_power_mode(pnic_info, in_value);
            break;
        default:
            break;
    }
}

static wf_s32 st_get_fw_bcn_valid(nic_info_st *pnic_info, wf_u8 type, wf_u8 *out_value, wf_u32 len)
{
    wf_s32 ret = WF_RETURN_OK;
    wf_u32 in_value;
    wf_u32 out_value_tmp = 0;
    in_value = (wf_u32)type;

    LPS_DBG();
    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_GET_HWREG, &in_value, 1, &out_value_tmp, 1);
    LPS_DBG(" in_value: %d, out_value_tmp: %x", in_value, out_value_tmp);
    if (WF_RETURN_FAIL == ret)
    {
        LPS_WARN("[%s] UMSG_OPS_HAL_GET_HWREG[%d] failed, check!", __func__, out_value_tmp);
        ret = WF_RETURN_FAIL;
    }
    *out_value = (wf_u8)(out_value_tmp & 0x000000FF);

    return ret;
}
static wf_s32 st_get_fw_lps_rf_on(nic_info_st *pnic_info, wf_u8 type, wf_u8 *out_value, wf_u32 len)
{
    wf_u32 val_crc;
    pwr_info_st *pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;
    wf_s32 err;
    wf_s32 ret = WF_RETURN_OK;

    LPS_DBG();
    if (pnic_info->is_surprise_removed || pwr_info_ptr->rf_pwr_state == rf_off)
    {
        *out_value = wf_true;
    }
    else
    {
        val_crc = wf_io_read32(pnic_info, REG_RCR, &err);
        if(err)
        {
            LPS_DBG("read failed,err:%d", err);
            ret = WF_RETURN_FAIL;
        }
        val_crc &= 0x00070000;

        if (val_crc)
        {
            *out_value = wf_false;
        }
        else
        {
            *out_value = wf_true;
        }
    }

    return ret;
}
static wf_s32 st_get_lps_hw_reg(nic_info_st *pnic_info, wf_u8 type, wf_u8 *out_value)
{
    wf_s32 ret = WF_RETURN_OK;

    switch(type)
    {
        case HW_VAR_BCN_VALID:
            ret = st_get_fw_bcn_valid(pnic_info, (wf_u8)HW_VAR_BCN_VALID, out_value, 1);
            break;
        case HW_VAR_FWLPS_RF_ON:
            ret = st_get_fw_lps_rf_on(pnic_info, (wf_u8)HW_VAR_FWLPS_RF_ON, out_value, 1);
            break;
        default:
            break;
    }

    return ret;
}

static bool st_check_nic_state(nic_info_st *pnic_info, wf_u32 check_state)
{
    bool ret = wf_false;
    if (pnic_info->nic_state & check_state)
    {
        ret = wf_true;
    }
    return ret;
}
static int st_check_lps_ok(nic_info_st *pnic_info)
{
    int ret = WF_RETURN_OK;
    pwr_info_st *pwr_info_ptr;
    sec_info_st *sec_info_ptr;
    wf_u32 current_time;
    wf_u32 delta_time;
    pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;
    sec_info_ptr = (sec_info_st *)pnic_info->sec_info;

    LPS_DBG();
    current_time = st_get_current_time();
    delta_time = current_time - pwr_info_ptr->delay_lps_last_timestamp;
    if (delta_time < LPS_DELAY_TIME)
    {
        LPS_WARN(" return: delta_time < LPS_DELAY_TIME");
        return WF_RETURN_FAIL;
    }
    if (//st_check_nic_state(pnic_info, WIFI_SITE_MONITOR) ||
        st_check_nic_state(pnic_info, WIFI_UNDER_LINKING | WIFI_UNDER_WPS) ||
        st_check_nic_state(pnic_info, WIFI_AP_STATE) ||
        st_check_nic_state(pnic_info, WIFI_ADHOC_MASTER_STATE | WIFI_ADHOC_STATE))
    {
        LPS_WARN(" return: %d", pnic_info->nic_state);
        return WF_RETURN_FAIL;
    }
    if (sec_info_ptr->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X &&
        sec_info_ptr->binstallGrpkey == wf_false)
    {
        LPS_WARN(" Group handshake still in progress!");
        return WF_RETURN_FAIL;
    }

    return ret;
}
static void st_set_rpwm_hw(nic_info_st *pnic_info, wf_u8 lps_state)
{
    int error;
    wf_u8 temp;
    pwr_info_st *pwr_info_ptr;
    pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;

    LPS_DBG();
    lps_state = PS_STATE(lps_state);
    if (lps_state == pwr_info_ptr->rpwm)
    {
        LPS_DBG(" Already set rpwm 0x%02x", pwr_info_ptr->rpwm);
        return;
    }
    if (pnic_info->is_driver_stopped == wf_true)
    {
        LPS_DBG(" Driver stopped");
        if (lps_state < PS_STATE_S2)
        {
            LPS_DBG(" Reject to enter PS_STATE(0x%02X) lower than S2 when DriverStopped!", lps_state);
            return;
        }
    }
    // This code block should discuss with fw team
    temp = wf_io_read8(pnic_info, 0xFE78, &error); //0xFE78
    LPS_DBG(" error = %d temp = %d", error, temp);
    temp = temp & 0xFC;
    if (lps_state <= PS_STATE_S2)
    {
        temp = temp | 0x01;
        wf_io_write8(pnic_info, 0xFE78, temp);
        LPS_DBG(" Set rpwm lps state: 0x%02x", lps_state);
    }
    else
    {
        temp = temp | 0x02;
        wf_io_write8(pnic_info, 0xFE78, temp);
        LPS_DBG(" Set rpwm lps state: 0x%02x", lps_state);
    }
}
static void st_set_lps_mode_hw(nic_info_st *pnic_info, wf_u8 bcn_ant_mode, wf_u8 lps_mode, wf_u8 smart_lps, const char* msg)
{
    pwr_info_st *pwr_info_ptr;
#if USE_M0_LPS_INTERFACES
    wf_u32 arg[1] = {0};
    wf_u32 val32;
#endif
    pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;

    LPS_DBG();
    // if (lps_mode >= PS_MODE_NUM)
    // {
    //     printk("Error: lps mode not support\r\n");
    //     return;
    // }
    if (pwr_info_ptr->pwr_current_mode == lps_mode)
    {
        if (lps_mode == PS_MODE_ACTIVE)
        {
            LPS_DBG(" Skip: now in PS_MODE_ACTIVE");
            return;
        }
        if (pwr_info_ptr->smart_lps == smart_lps && pwr_info_ptr->bcn_ant_mode == bcn_ant_mode)
        {
            LPS_DBG(" Skip: lps mode is the same as current lps mode");
            return;
        }
    }
    if (lps_mode == PS_MODE_ACTIVE)
    {
        pwr_info_ptr->lps_exit_cnts++;
        pwr_info_ptr->pwr_current_mode = lps_mode;
#if USE_M0_LPS_INTERFACES
        arg[0] = 0;
        mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_LPS_OPT, arg, 1, &val32, 1);
        if(val32 != 0)
            LOG_W(" Leave LPS fail\n");
#else
        st_set_rpwm_hw(pnic_info, PS_STATE_S4);
        st_set_lps_hw_reg(pnic_info, HW_VAR_wMBOX1_FW_PWRMODE, lps_mode);
#endif
        pwr_info_ptr->b_fw_current_in_ps_mode = wf_false;
        LPS_DBG(" FW exit low power saving mode\r\n");
    }
    else
    {
        if (st_check_lps_ok(pnic_info) == WF_RETURN_OK && st_check_nic_state(pnic_info, WIFI_ASOC_STATE))
        {
            pwr_info_ptr->lps_enter_cnts;
        }
        pwr_info_ptr->pwr_current_mode = lps_mode;
        pwr_info_ptr->smart_lps = smart_lps;
        pwr_info_ptr->bcn_ant_mode = bcn_ant_mode;
        pwr_info_ptr->b_fw_current_in_ps_mode = wf_true;
#if USE_M0_LPS_INTERFACES
        arg[0] = 1;
            mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_LPS_OPT, arg, 1, NULL, 0);
#else
        st_set_lps_hw_reg(pnic_info, HW_VAR_wMBOX1_FW_PWRMODE, lps_mode);
        st_set_rpwm_hw(pnic_info, PS_STATE_S2);
#endif
        LPS_DBG(" FW enter low power saving mode\r\n");
    }
}

static int st_wait_rf_resume(nic_info_st *pnic_info, wf_u32 delay_ms)
{
    int ret = WF_RETURN_OK;
    wf_u32 start_time;
    wf_u8 status;

    LPS_DBG();
    start_time = st_get_current_time();
    while(1)
    {
        if (pnic_info->is_surprise_removed)
        {
            ret = WF_RETURN_FAIL;
            LPS_DBG(" Error: device surprise removed!");
            break;
        }
        if (st_get_current_time() - start_time > delay_ms)
        {
            ret = WF_RETURN_FAIL;
            LPS_DBG(" Wait time out!");
            break;
        }
        if (st_get_lps_hw_reg(pnic_info, HW_VAR_FWLPS_RF_ON, &status) == WF_RETURN_OK)
        {
            if (status == wf_true)
            {
                break;
            }
        }
        wf_msleep(1);
    }
    return ret;
}
static void st_enter_lps(nic_info_st *pnic_info, char *msg)
{
    wf_u8 n_assoc_iface = 0;
    char buff[32] = {0};
    int i;
    pwr_info_st *pwr_info_ptr;
    wf_bool bconnect;
    pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;
    // hif_node_st *hif_node_ptr = (hif_node_st *)pnic_info->hif_node;
    LPS_DBG(" reason: %s", msg);

    // for (i = 0; i < hif_node_ptr->nic_number; i++)
    // {
    //     if (st_check_nic_state(hif_node_ptr->nic_info[i], WIFI_ASOC_STATE))
    //     {
    //         n_assoc_iface++;
    //     }
    // }
    // if (n_assoc_iface != 1)
    // {
    //     if (n_assoc_iface == 0)
    //     {
    //         LPS_DBG(" Can not enter lps: NO LINKED");
    //         return;
    //     }
    //     LPS_DBG(" Can not enter lps: more than one nic linked");
    // }

    // for (i = 0; i < hif_node_ptr->nic_number; i++)
    // {
    //     if (st_check_lps_ok(hif_node_ptr->nic_info[i]) == WF_RETURN_FAIL)
    //     {
    //         LPS_DBG("Check lps fail");
    //         return;
    //     }
    // }
    wf_mlme_get_connect(pnic_info, &bconnect);
    if (bconnect && pnic_info->nic_num == 0)
    {
        n_assoc_iface++;
    }
    if (n_assoc_iface == 0)
    {
        LPS_DBG(" Can not enter lps: NO LINKED || virtual nic");
        return;
    }
    if (st_check_lps_ok(pnic_info) == WF_RETURN_FAIL)
    {
        LPS_DBG("Check lps fail");
        return;
    }


    // I think pwr_info_ptr->b_leisure_lps can be replaced by pwr_info_ptr->pwr_mode
    if (pwr_info_ptr->pwr_mgnt != PS_MODE_ACTIVE)
    {
        // I think lps_idle_cnts is no use.
        if (pwr_info_ptr->lps_idle_cnts >= 2)
        {
            if (pwr_info_ptr->pwr_current_mode == PS_MODE_ACTIVE)
            {
                sprintf(buff, "WIFI-%s", msg);
                pwr_info_ptr->b_power_saving = wf_true;
                st_set_lps_mode_hw(pnic_info, pwr_info_ptr->bcn_ant_mode, pwr_info_ptr->pwr_mgnt,
                                   pwr_info_ptr->smart_lps, buff);
                LPS_DBG(" Enter lps success....");
            }
            else
            {
                LPS_DBG(" Now is sleeping");
            }
        }
        else
        {
            pwr_info_ptr->lps_idle_cnts++;
        }
    }
    else
    {
        LPS_DBG(" Enter lps fail: pwr_info_ptr->pwr_mgnt == PS_MODE_ACTIVE");
    }
}

static void st_exit_lps(nic_info_st *pnic_info, char *msg)
{
    char buff[32] = {0};
    pwr_info_st *pwr_info_ptr;
    pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;

    LPS_DBG(" reason: %s", msg);

    if (pwr_info_ptr->pwr_mgnt != PS_MODE_ACTIVE)
    {
        if (pwr_info_ptr->pwr_current_mode != PS_MODE_ACTIVE)
        {
            sprintf(buff, "WIFI-%s", msg);
            st_set_lps_mode_hw(pnic_info, 0, PS_MODE_ACTIVE, 0, buff);
            if (pwr_info_ptr->pwr_current_mode == PS_MODE_ACTIVE)
            {
                st_wait_rf_resume(pnic_info, 300);
            }
            LPS_DBG(" Exit lps from sleep success");
        }
        else
        {
            LPS_DBG(" Now is awake");
        }
    }
    else
    {
        printk("Enter lps fail: pwr_info->pwr_mgnt == PS_MODE_ACTIVE\r\n");
    }
}

inline static wf_u32 st_get_current_time(void)
{
    return jiffies;
}
inline static wf_u32 st_ms_to_systime(wf_u32 ms)
{
    return ms / 1000 * HZ;
}
inline static wf_u32 st_systime_to_ms(wf_u32 systime)
{
    return systime / HZ * 1000;
}
inline static void _st_init_lps_lock(wf_os_api_sema_t *sema)
{
    wf_os_api_sema_init(sema, 1);
}

inline static void _st_enter_lps_lock(wf_os_api_sema_t *sema)
{
    wf_os_api_sema_wait(sema);
}

inline static void _st_exit_lps_lock(wf_os_api_sema_t *sema)
{
    wf_os_api_sema_post(sema);
}

/***************************************************************
    Global Function
***************************************************************/
void wf_lps_ctrl_wk_hdl(nic_info_st *pnic_info, wf_u8 lps_ctrl_type)
{
    wf_u8 m_status;
    pwr_info_st *pwr_info_ptr;
    wf_bool lps_flag = wf_false, bconnect;
    pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;

    LPS_DBG();
    if (st_check_nic_state(pnic_info, WIFI_ADHOC_MASTER_STATE) ||
        st_check_nic_state(pnic_info, WIFI_ADHOC_STATE))
    {
        LPS_DBG(" return: nic state WIFI_ADHOC_MASTER_STATE || WIFI_ADHOC_STATE");
        return;
    }

    switch(lps_ctrl_type)
    {
        case LPS_CTRL_SCAN:
            wf_mlme_get_connect(pnic_info, &bconnect);
            if (bconnect)
            {
                lps_flag = wf_false;
            }
            break;
        case LPS_CTRL_CONNECT:
            m_status = 1;
            pwr_info_ptr->lps_idle_cnts = 0;
#if USE_M0_LPS_INTERFACES
            wf_mcu_lps_config(pnic_info);
#endif
            st_set_lps_hw_reg(pnic_info, HW_VAR_wMBOX1_FW_JOINBSSRPT, m_status);
            lps_flag = wf_false;
            break;
        case LPS_CTRL_SPECIAL_PACKET:
            pwr_info_ptr->delay_lps_last_timestamp = st_get_current_time();
            atomic_set(&pwr_info_ptr->lps_spc_flag, 0);
        case LPS_CTRL_LEAVE:
        case LPS_CTRL_JOINBSS:
        case LPS_CTRL_LEAVE_CFG80211_PWRMGMT:
        case LPS_CTRL_TRAFFIC_BUSY:
        case LPS_CTRL_TX_TRAFFIC_LEAVE:
        case LPS_CTRL_RX_TRAFFIC_LEAVE:
        case LPS_CTRL_NO_LINKED:
        case LPS_CTRL_DISCONNECT:
            lps_flag = wf_false;
            break;
        case LPS_CTRL_ENTER:
            lps_flag = wf_true;
            break;
        default:
            break;
    }
    if (lps_flag == wf_true)
    {
        st_enter_lps(pnic_info, (char *)(LPS_CTRL_TYPE_STR[lps_ctrl_type]));
    }
    else
    {
        st_exit_lps(pnic_info, (char *)(LPS_CTRL_TYPE_STR[lps_ctrl_type]));
    }
}
wf_s32 wf_lps_init(nic_info_st *pnic_info)
{
    wf_s32 ret = WF_RETURN_OK;
    pwr_info_st *pwr_info;
    LPS_DBG();

    pwr_info = wf_kzalloc(sizeof(pwr_info_st));
    if (pwr_info == NULL)
    {
        LPS_WARN("[LPS] malloc lps_param_st failed");
        return WF_RETURN_FAIL;
    }
    else
    {
        pnic_info->pwr_info = (void *)pwr_info;
    }
    _st_init_lps_lock(&pwr_info->lock); // Init lock
    _st_init_lps_lock(&pwr_info->check_32k_lock);

    pwr_info->rf_pwr_state = rf_on;
    pwr_info->lps_enter_cnts = 0;
    pwr_info->pwr_state_check_cnts = 0;
    pwr_info->lps_idle_cnts = 0;
    pwr_info->rpwm = 0;
    pwr_info->wfprs = PS_STATE_S4;
    pwr_info->b_fw_current_in_ps_mode = wf_false;
    pwr_info->pwr_current_mode = PS_MODE_ACTIVE;
    pwr_info->smart_lps = 2; // According to 9083
    pwr_info->bcn_ant_mode = 0;

    // Set pwr_info->pwr_mgmt according to registry_par->power_mgnt and registrypriv.mp_mode in 9083
    pwr_info->pwr_mgnt = PS_MODE_MAX;
    //pwr_info->b_leisure_lps = (pwr_info->pwr_mgnt != PS_MODE_ACTIVE)? wf_true : wf_false;

    return ret;
}

void wf_lps_deny(nic_info_st *pnic_info, PS_DENY_REASON reason)
{
    pwr_info_st *pwr_info_ptr;
    pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;
    LPS_DBG();

    _st_enter_lps_lock(&pwr_info_ptr->lock);
    if (pwr_info_ptr->lps_deny & BIT(reason))
    {
        printk("[WARNING] Reason %d had been set before!!\r\n", reason);
    }
    pwr_info_ptr->lps_deny | BIT(reason);
    _st_exit_lps_lock(&pwr_info_ptr->lock);
}

void wf_lps_deny_cancel(nic_info_st *pnic_info, PS_DENY_REASON reason)
{
    pwr_info_st *pwr_info_ptr;
    pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;
    _st_enter_lps_lock(&pwr_info_ptr->lock);
    LPS_DBG();

    if (((pwr_info_ptr->lps_deny) & BIT(reason)) == 0)
    {
        printk("[WARNING] Reason %d had been canceled before!!\r\n", reason);
    }
    _st_exit_lps_lock(&pwr_info_ptr->lock);

    pwr_info_ptr->lps_deny & (~BIT(reason));
}

wf_s32 wf_lps_wakeup(nic_info_st *pnic_info, wf_u8 lps_ctrl_type, wf_bool enqueue) // Enqueue for interrupt
{
    wf_u32 n_assoc_iface = 0;
    wf_u32 ret = WF_RETURN_OK;
    int i;
    wf_bool wakeup_flag = wf_false, bconnect;
    // hif_node_st *hif_node_ptr = (hif_node_st *)pnic_info->hif_node;

    LPS_DBG();

    if (pnic_info->is_driver_stopped == wf_true)
    {
        LPS_WARN("is_driver_stopped = true Skip!");
        return WF_RETURN_FAIL;
    }
    if (pnic_info->is_surprise_removed == wf_true)
    {
        LPS_WARN("nic is surprise removed Skip!");
        return WF_RETURN_FAIL;
    }
    if (lps_ctrl_type == LPS_CTRL_ENTER || lps_ctrl_type >= LPS_CTRL_MAX)
    {
        LPS_WARN("Error: lps ctrl type not support!");
        return WF_RETURN_FAIL;
    }
    if (lps_ctrl_type == LPS_CTRL_NO_LINKED)
    {
        wakeup_flag = wf_true;
    }
    else
    {
        wf_mlme_get_connect(pnic_info, &bconnect);
        if (bconnect && pnic_info->nic_num == 0)
        {
            n_assoc_iface++;
        }
        if (n_assoc_iface)
        {

            wakeup_flag = wf_true;
        }
    }

    if (wakeup_flag == wf_true)
    {
        if (enqueue == wf_true)
        {
            mlme_msg_t msg;
            msg.module = MLME_MSG_LPS;
            msg.position = 0;
            msg.lps_type = lps_ctrl_type;
            wf_mlme_msg_send(pnic_info, &msg);
        }
        else
        {
            wf_lps_ctrl_wk_hdl(pnic_info, lps_ctrl_type);
        }

    }
    return ret;
}

wf_s32 wf_lps_sleep(nic_info_st *pnic_info, wf_u8 lps_ctrl_type, wf_bool enqueue) // Enqueue for interrupt
{
    wf_u32 ret = WF_RETURN_OK;
    pwr_info_st *pwr_info_ptr;
    wf_u32 lps_deny = 0;
    pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;

    LOG_D("wf_lps_sleep");
    if (wf_local_cfg_get_work_mode(pnic_info) != WF_INFRA_MODE)
    {
        LPS_INFO("lps only supports the STA mode");
        return WF_RETURN_FAIL;
    }
    _st_enter_lps_lock(&pwr_info_ptr->lock);
    lps_deny = pwr_info_ptr->lps_deny;
    _st_exit_lps_lock(&pwr_info_ptr->lock);
    if (lps_deny != 0)
    {
        LPS_INFO("Skip: can not sleep! Reason: %d", lps_deny);
        return WF_RETURN_FAIL;
    }

    if (lps_ctrl_type != LPS_CTRL_ENTER || lps_ctrl_type >= LPS_CTRL_MAX)
    {
        LPS_WARN("Error: lps ctrl type not support!");
        return WF_RETURN_FAIL;
    }

    if (enqueue == wf_true)
    {
        mlme_msg_t msg;
        msg.module = MLME_MSG_LPS;
        msg.position = 0;
        msg.lps_type = lps_ctrl_type;
        wf_mlme_msg_send(pnic_info, &msg);
    }
    else
    {
        wf_lps_ctrl_wk_hdl(pnic_info, lps_ctrl_type);
    }

    return ret;
}

void wf_lps_sleep_mlme_monitor(nic_info_st *pnic_info)
{
    wf_u8 n_assoc_iface = 0;
    wf_u32 ret = WF_RETURN_OK;
    int i = 0;
    // hif_node_st *hif_node_ptr = (hif_node_st *)pnic_info->hif_node;
    mlme_info_t *mlme_info  = (mlme_info_t*)pnic_info->mlme_info;
    pwr_info_st *pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;
    wf_bool bconnect;
    LPS_DBG();

    wf_mlme_get_connect(pnic_info, &bconnect);
    if (bconnect)
    {
        n_assoc_iface++;
    }
    if (n_assoc_iface == 0)
    {
        if (pwr_info_ptr->pwr_current_mode != PS_MODE_ACTIVE)
        {
            wf_lps_wakeup(pnic_info, LPS_CTRL_NO_LINKED, wf_true);
        }
    }
    else
    {
        if(bconnect)
        {
            if (mlme_info->link_info.num_rx_unicast_ok_in_period + mlme_info->link_info.num_tx_ok_in_period > 8)
            {
                if (pwr_info_ptr->pwr_current_mode != PS_MODE_ACTIVE)
                {
                    wf_lps_wakeup(pnic_info, LPS_CTRL_TRAFFIC_BUSY, wf_true);
                }
                else
                {
                    LPS_DBG(" now is awake");
                }
            }
            else
            {
                if (pwr_info_ptr->pwr_current_mode == PS_MODE_ACTIVE)
                {
                    wf_lps_sleep(pnic_info, LPS_CTRL_ENTER, wf_true);
                }
                else
                {
                    LPS_DBG(" now is sleeping");
                }
            }
        }
    }
}
#endif

