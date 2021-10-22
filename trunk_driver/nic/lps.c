/*
 * lpc.c
 *
 * used for low power saving
 *
 * Author: houchuang
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
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

#define LPS_DBG(fmt, ...)      LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LPS_ARRAY(data, len)   log_array(data, len)
#define LPS_INFO(fmt, ...)     LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LPS_WARN(fmt, ...)     LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LPS_ERROR(fmt, ...)    LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)

#ifdef CONFIG_LPS

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
static int lps_check_lps_ok(nic_info_st *pnic_info)
{
    int ret = WF_RETURN_OK;
    pwr_info_st *ppwr_info = pnic_info->pwr_info;
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u64 current_time;
    wf_u64 delta_time;

    LPS_DBG();
    current_time = wf_os_api_timestamp();
    delta_time = current_time - ppwr_info->delay_lps_last_timestamp;
    if (delta_time < LPS_DELAY_TIME)
    {
        LPS_WARN(" return: delta_time < LPS_DELAY_TIME");
        return WF_RETURN_FAIL;
    }
    if (wf_local_cfg_get_work_mode(pnic_info) != WF_INFRA_MODE)
    {
        LPS_WARN(" return: %d", pnic_info->nic_state);
        return WF_RETURN_FAIL;
    }
    if (psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X &&
        psec_info->binstallGrpkey == wf_false)
    {
        LPS_WARN(" Group handshake still in progress!");
        return WF_RETURN_FAIL;
    }

    return ret;
}

#if USE_M0_LPS_INTERFACES
#else
static void lps_set_fw_power_mode(nic_info_st *pnic_info, wf_u8 lps_mode)
{
    wf_u8 smart_ps = 0;
    wf_u8 u1wMBOX1PwrModeParm[wMBOX1_PWRMODE_LEN] = { 0 };
    wf_u8 power_state = 0;
    wf_u8 awake_intvl = 1;
    wf_u8 byte5 = 0;
    wf_u8 rlbm = 0;
    pwr_info_st *pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;

    LPS_DBG();
    pwr_info_ptr->b_mailbox_sync = wf_true;

    if (lps_mode == PWR_MODE_MIN)
    {
        rlbm = 0;
        awake_intvl = 2;
        smart_ps = pwr_info_ptr->smart_lps;
    }
    else if (lps_mode == PWR_MODE_MAX)
    {
        rlbm = 1;
        awake_intvl = 2;
        smart_ps = pwr_info_ptr->smart_lps;
    }
    else if (lps_mode == PWR_MODE_DTIM)
    {
        rlbm = 2;
        if (pwr_info_ptr->dtim > 0 && pwr_info_ptr->dtim < 16)
        {
            awake_intvl = pwr_info_ptr->dtim + 1;
        }
        else
        {
            awake_intvl = 4;
        }
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

    wf_mcu_fill_mbox1_fw(pnic_info, wMBOX1_9086X_SET_PWR_MODE, u1wMBOX1PwrModeParm, wMBOX1_PWRMODE_LEN);

    pwr_info_ptr->b_mailbox_sync = wf_false;
}

static void lps_set_rpwm_hw(nic_info_st *pnic_info, wf_u8 lps_state)
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
#endif

wf_inline static void _lps_init_lps_lock(wf_os_api_sema_t *sema)
{
    wf_os_api_sema_init(sema, 1);
}

wf_inline static void _lps_enter_lps_lock(wf_os_api_sema_t *sema)
{
    wf_os_api_sema_wait(sema);
}

wf_inline static void _lps_exit_lps_lock(wf_os_api_sema_t *sema)
{
    wf_os_api_sema_post(sema);
}

/***************************************************************
    Global Function
***************************************************************/
void wf_lps_ctrl_state_hdl(nic_info_st *pnic_info, wf_u8 lps_ctrl_type)
{
    static wf_bool con_flag = wf_true;

    LPS_DBG();

    LPS_INFO("con_flag = %d", con_flag);
    if (con_flag == wf_true)
    {
        wf_lps_ctrl_wk_hdl(pnic_info, LPS_CTRL_CONNECT);
        con_flag = wf_false;
    }
    wf_lps_ctrl_wk_hdl(pnic_info, lps_ctrl_type);
}

static void lps_set_enter_register(nic_info_st *pnic_info, wf_u8 lps_mode, wf_u8 smart_lps)
{
    pwr_info_st *ppwr_info = pnic_info->pwr_info;
    wf_bool is_connected;

    LPS_DBG();

    if(ppwr_info->pwr_current_mode == lps_mode)
    {
        if (ppwr_info->pwr_mgnt == PWR_MODE_ACTIVE)
        {
            LPS_DBG(" Skip: now in PWR_MODE_ACTIVE");
            return;
        }
    }
    if (lps_mode == PWR_MODE_ACTIVE)
    {
        ppwr_info->lps_exit_cnts++;
        ppwr_info->pwr_current_mode = lps_mode;
        LPS_DBG("ppwr_info->pwr_current_mode = %d", ppwr_info->pwr_current_mode);
#if USE_M0_LPS_INTERFACES
        wf_mcu_set_lps_opt(pnic_info, 0);
#else
        lps_set_rpwm_hw(pnic_info, PS_STATE_S4);
        lps_set_fw_power_mode(pnic_info, PWR_MODE_ACTIVE);
#endif
        ppwr_info->b_fw_current_in_ps_mode = wf_false;
        LPS_DBG(" FW exit low power saving mode\r\n");
    }
    else
    {
        wf_mlme_get_connect(pnic_info, &is_connected);
        if (lps_check_lps_ok(pnic_info) == WF_RETURN_OK && is_connected)
        {
            ppwr_info->lps_enter_cnts++;
        }
        ppwr_info->pwr_current_mode = ppwr_info->pwr_mgnt;
        ppwr_info->smart_lps = smart_lps;
        ppwr_info->b_fw_current_in_ps_mode = wf_true;
#if USE_M0_LPS_INTERFACES
        wf_mcu_set_lps_opt(pnic_info, 1);
#else
        lps_set_fw_power_mode(pnic_info, PWR_MODE_ACTIVE);
        lps_set_rpwm_hw(pnic_info, PS_STATE_S4);
#endif
        LPS_DBG(" FW enter low power saving mode\r\n");
    }
}

static void lps_enter(nic_info_st *pnic_info, char *msg)
{
    pwr_info_st *ppwr_info = (pwr_info_st *)pnic_info->pwr_info;
    wf_bool bconnect;
    wf_u8 n_assoc_iface = 0;
    char buff[32] = {0};

    LPS_DBG(" reason: %s", msg);

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

    if (lps_check_lps_ok(pnic_info) == WF_RETURN_FAIL)
    {
        LPS_DBG("Check lps fail");
        return;
    }

    LOG_D("ppwr_info->pwr_mgnt = %d, ppwr_info->pwr_current_mode = %d", ppwr_info->pwr_mgnt, ppwr_info->pwr_current_mode);
    if(ppwr_info->pwr_mgnt != PWR_MODE_ACTIVE)
    {
        if(ppwr_info->pwr_current_mode == PWR_MODE_ACTIVE)
        {
            sprintf(buff, "WIFI-%s", msg);
            ppwr_info->b_power_saving = wf_true;

            lps_set_enter_register(pnic_info, ppwr_info->pwr_mgnt, ppwr_info->smart_lps);
        }
    }
}

static void lps_exit(nic_info_st *pnic_info, char *msg)
{
    char buff[32] = {0};
    pwr_info_st *ppwr_info= pnic_info->pwr_info;

    LPS_DBG(" reason: %s", msg);

    if (ppwr_info->pwr_mgnt != PWR_MODE_ACTIVE)
    {
        if (ppwr_info->pwr_current_mode != PWR_MODE_ACTIVE)
        {
            sprintf(buff, "WIFI-%s", msg);
            lps_set_enter_register(pnic_info, PWR_MODE_ACTIVE, 0);
            LPS_DBG(" Exit lps from sleep success");
        }
        else
        {
            LPS_DBG(" Now is awake");
        }
    }
    else
    {
        printk("Enter lps fail: pwr_info->pwr_mgnt == PWR_MODE_ACTIVE\r\n");
    }
}

static wf_u8* lps_query_data_from_ie(wf_u8 * ie, wf_u8 type)
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

static void lps_rsvd_page_chip_hw_construct_beacon(nic_info_st *pnic_info, wf_u8 *frame_index_ptr, wf_u32 *length_out)
{
    struct wl_ieee80211_hdr *wlan_hdr_ptr;
    wdn_net_info_st *pwdn =  wf_wdn_find_info(pnic_info,wf_wlan_get_cur_bssid(pnic_info));
    wf_u16 *frame_control; // Mac header (1)
    wf_u8 bc_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    hw_info_st *hw_info_ptr =(hw_info_st *) pnic_info->hw_info;
    wf_u32 pkt_len = 0;
    wf_wlan_mgmt_info_t *wlan_info_ptr = (wf_wlan_mgmt_info_t *)pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *cur_network_ptr = & (wlan_info_ptr->cur_network);

    LPS_DBG();
    if (pwdn == NULL)
    {
        LPS_WARN("Not find wdn");
        return;
    }

    wlan_hdr_ptr = (struct wl_ieee80211_hdr *)frame_index_ptr;
    frame_control = &(wlan_hdr_ptr->frame_ctl);
    *frame_control = 0; // Clear 0
    wf_memcpy(wlan_hdr_ptr->addr1, bc_addr, WF_ETH_ALEN);
    wf_memcpy(wlan_hdr_ptr->addr2, hw_info_ptr->macAddr, WF_ETH_ALEN);
    wf_memcpy(wlan_hdr_ptr->addr3, wf_wlan_get_cur_bssid(pnic_info), WF_ETH_ALEN);
    SetSeqNum(wlan_hdr_ptr, 0); // Set Sequence Control field
    SetFrameType(frame_index_ptr, WIFI_BEACON); // Set Frame Type field

    frame_index_ptr = frame_index_ptr + sizeof(struct wl_ieee80211_hdr_3addr);
    pkt_len = pkt_len + sizeof(struct wl_ieee80211_hdr_3addr);

    frame_index_ptr = frame_index_ptr + 8;
    pkt_len = pkt_len + 8;

    wf_memcpy(frame_index_ptr, (wf_u8 *)lps_query_data_from_ie(cur_network_ptr->ies, BCN_INTERVAL), 2);
    frame_index_ptr = frame_index_ptr + 2;
    pkt_len = pkt_len + 2;

    wf_memcpy(frame_index_ptr, (wf_u8 *)lps_query_data_from_ie(cur_network_ptr->ies, CAPABILITY), 2);
    frame_index_ptr = frame_index_ptr + 2;
    pkt_len = pkt_len + 2;

    frame_index_ptr = set_ie(frame_index_ptr, WF_80211_MGMT_EID_SSID, cur_network_ptr->ssid.length,
                             cur_network_ptr->ssid.data, &pkt_len);
    frame_index_ptr= set_ie(frame_index_ptr, WF_80211_MGMT_EID_SUPP_RATES,
                            ((pwdn->datarate_len > 8) ? 8 : pwdn->datarate_len),
                            pwdn->datarate, &pkt_len); // cur_network->SupportedRates
    frame_index_ptr= set_ie(frame_index_ptr, WF_80211_MGMT_EID_DS_PARAMS, 1,
                            (wf_u8 *)&cur_network_ptr->channel, &pkt_len); // Configuration.DSConfig
    if (pwdn->ext_datarate_len > 0)
    {
        frame_index_ptr= set_ie(frame_index_ptr, WF_80211_MGMT_EID_EXT_SUPP_RATES,
                                pwdn->ext_datarate_len,
                                pwdn->ext_datarate, &pkt_len);
    }

    if (pkt_len + TXDESC_SIZE > 512)
    {
        LPS_DBG("Beacon frame too large: %d", pkt_len);
        return;
    }

    *length_out = pkt_len; // Output packet length
}

static void lps_rsvd_page_chip_hw_construct_pspoll(nic_info_st *pnic_info, wf_u8 *frame_index_ptr, wf_u32 *length_out)
{
    struct wl_ieee80211_hdr *wlan_hdr_ptr;
    wf_u16 *frame_control; // Mac header (1)
    wf_wlan_mgmt_info_t *wlan_info_ptr = (wf_wlan_mgmt_info_t *)pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *cur_network_ptr = &(wlan_info_ptr->cur_network);
    hw_info_st *hw_info_ptr =(hw_info_st *) pnic_info->hw_info;
    LPS_DBG();

    wlan_hdr_ptr = (struct wl_ieee80211_hdr *)frame_index_ptr;
    frame_control = &wlan_hdr_ptr->frame_ctl;
    *frame_control = 0; // Clear 0

    SetPwrMgt(frame_control); // Set Power Management bit
    SetFrameSubType(frame_index_ptr, WIFI_PSPOLL); // Set SubType in Frame Control field
    SetDuration(frame_index_ptr, cur_network_ptr->aid | 0xC000); // Set Duration/ID field

    wf_memcpy(wlan_hdr_ptr->addr1, wf_wlan_get_cur_bssid(pnic_info), WF_ETH_ALEN);
    wf_memcpy(wlan_hdr_ptr->addr2, hw_info_ptr->macAddr, WF_ETH_ALEN);

    *length_out = 16; // Output packet length
}

static void lps_rsvd_page_chip_hw_construct_nullfunctiondata(nic_info_st *pnic_info, wf_u8 *frame_index_ptr, wf_u32 *length_out,
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
    switch (get_sys_work_mode(pnic_info)) // In nic local_info
    {
        case WF_INFRA_MODE:
            SetToDs(frame_control);
            wf_memcpy(wlan_hdr_ptr->addr1, wf_wlan_get_cur_bssid(pnic_info), WF_ETH_ALEN);
            wf_memcpy(wlan_hdr_ptr->addr2, hw_info_ptr->macAddr, WF_ETH_ALEN);
            wf_memcpy(wlan_hdr_ptr->addr3, addr_start_ptr, WF_ETH_ALEN);
            break;
        case WF_MASTER_MODE:
            SetFrDs(frame_control);
            wf_memcpy(wlan_hdr_ptr->addr1, addr_start_ptr, WF_ETH_ALEN);
            wf_memcpy(wlan_hdr_ptr->addr2, wf_wlan_get_cur_bssid(pnic_info), WF_ETH_ALEN);
            wf_memcpy(wlan_hdr_ptr->addr3, hw_info_ptr->macAddr, WF_ETH_ALEN);
            break;
        case WF_ADHOC_MODE:
        default:
            wf_memcpy(wlan_hdr_ptr->addr1, addr_start_ptr, WF_ETH_ALEN);
            wf_memcpy(wlan_hdr_ptr->addr2, hw_info_ptr->macAddr, WF_ETH_ALEN);
            wf_memcpy(wlan_hdr_ptr->addr3, wf_wlan_get_cur_bssid(pnic_info), WF_ETH_ALEN);
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

static void lps_fill_fake_txdesc(nic_info_st *pnic_info, wf_u8 *tx_des_start_addr, wf_u32 pkt_len,
                                 wf_bool is_ps_poll, wf_bool is_bt_qos_null, wf_bool is_dataframe)
{
    LPS_DBG();
    wf_memset(tx_des_start_addr, 0, TXDESC_SIZE);
    wf_set_bits_to_le_u32(tx_des_start_addr, 27, 1, 1); // bFirstSeg
    wf_set_bits_to_le_u32(tx_des_start_addr, 26, 1, 1);  // bLastSeg
    wf_set_bits_to_le_u32(tx_des_start_addr, 16, 8, 0X28); // Offset = 32
    wf_set_bits_to_le_u32(tx_des_start_addr, 0, 16, pkt_len);  // Buffer size + command header
    wf_set_bits_to_le_u32(tx_des_start_addr+4, 8, 5, QSLT_MGNT); // Fixed queue of Mgnt queue

    /* Set NAVUSEHDR to prevent Ps-poll AId filed to be changed to error vlaue by Hw. */
    if (is_ps_poll == wf_true)
    {
        wf_set_bits_to_le_u32(tx_des_start_addr+12, 15, 1, 1);
    }
    else
    {
        wf_set_bits_to_le_u32(tx_des_start_addr+32, 15, 1, 1); // Hw set sequence number
        wf_set_bits_to_le_u32(tx_des_start_addr+12, 6, 2, 0);
    }

    if (is_bt_qos_null == wf_true)
    {
        wf_set_bits_to_le_u32(tx_des_start_addr+8, 23, 1, 1);
    }
    wf_set_bits_to_le_u32(tx_des_start_addr+12, 8, 1, 1); /* use data rate which is set by Sw */
    wf_set_bits_to_le_u32(tx_des_start_addr, 31, 1, 1);

    wf_set_bits_to_le_u32(tx_des_start_addr+16, 0, 7, DESC_RATE1M);

    /* Encrypt the data frame if under security mode excepct null data. Suggested by CCW. */
    if (is_dataframe == wf_true)
    {
        wf_u32 EncAlg;
        sec_info_st *sec_info = pnic_info->sec_info;

        EncAlg = sec_info->dot11PrivacyAlgrthm;
        switch (EncAlg)
        {
            case _NO_PRIVACY_:
                wf_set_bits_to_le_u32(tx_des_start_addr+4, 22, 2,  0x0);
                break;
            case _WEP40_:
            case _WEP104_:
            case _TKIP_:
                wf_set_bits_to_le_u32(tx_des_start_addr+4, 22, 2,  0x1);
                break;
            case _SMS4_:
                wf_set_bits_to_le_u32(tx_des_start_addr+4, 22, 2,  0x2);
                break;
            case _AES_:
                wf_set_bits_to_le_u32(tx_des_start_addr+4, 22, 2,  0x3);
                break;
            default:
                wf_set_bits_to_le_u32(tx_des_start_addr+4, 22, 2,  0x0);
                break;
        }
    }

#ifdef CONFIG_RICHV200
    wf_txdesc_chksum(tx_des_start_addr);
#else
    wf_txdesc_chksum((struct tx_desc *)tx_des_start_addr);
#endif
}

static void lps_rsvd_page_mgntframe_attrib_update(nic_info_st * pnic_info, struct xmit_frame *pattrib)
{
    wdn_net_info_st *wdn_net_info_ptr;

    LPS_DBG();

    pattrib->hdrlen = WLAN_HDR_A3_LEN;
    pattrib->nr_frags = 1;
    pattrib->priority = 7;

    wdn_net_info_ptr =  wf_wdn_find_info(pnic_info,wf_wlan_get_cur_bssid(pnic_info));
    pattrib->qsel = QSLT_MGNT;

    pattrib->encrypt_algo = _NO_PRIVACY_;

    pattrib->ht_en = wf_false;
    pattrib->seqnum = wdn_net_info_ptr->wdn_xmitpriv.txseq_tid[QSLT_MGNT];
}

static wf_bool lps_mpdu_send_complete_cb(nic_info_st *nic_info, struct xmit_buf *pxmitbuf)
{
    tx_info_st *tx_info = nic_info->tx_info;

    wf_xmit_buf_delete(tx_info, pxmitbuf);

    wf_io_tx_xmit_wake(nic_info);

    return wf_true;
}

static wf_bool lps_mpdu_insert_sending_queue(nic_info_st *nic_info, struct xmit_frame *pxmitframe)
{
    wf_u8 *mem_addr;
    wf_u32 ff_hwaddr;
    wf_bool bRet = wf_true;
    int ret;
    wf_bool inner_ret = wf_true;
    wf_bool blast = wf_false;
    int t, sz, w_sz, pull = 0;
    struct xmit_buf *pxmitbuf = pxmitframe->pxmitbuf;
    hw_info_st *hw_info = nic_info->hw_info;
    wf_u32  txlen = 0;

    LPS_DBG();
    mem_addr = pxmitframe->buf_addr;

    for (t = 0; t < pxmitframe->nr_frags; t++)
    {
        if (inner_ret != wf_true && ret == wf_true)
            ret = wf_false;

        if (t != (pxmitframe->nr_frags - 1))
        {
            LPS_DBG("pattrib->nr_frags=%d\n", pxmitframe->nr_frags);
            sz = hw_info->frag_thresh;
            sz = sz - 4 - 0; /* 4: wlan head filed????????? */
        }
        else
        {
            /* no frag */
            blast = wf_true;
            sz = pxmitframe->last_txcmdsz;
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
        ff_hwaddr = wf_quary_addr(pxmitframe->qsel);

        txlen = TXDESC_SIZE + pxmitframe->last_txcmdsz;
        wf_timer_set(&pxmitbuf->time, 0);

        if(blast)
        {
            ret = wf_io_write_data(nic_info, 1, mem_addr, w_sz,
                                   ff_hwaddr,(void *)lps_mpdu_send_complete_cb, nic_info, pxmitbuf);
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

static wf_s32 lps_resv_page_xmit(nic_info_st * pnic_info, struct xmit_frame *mgnt_frame_ptr)
{
    wf_s32 ret = wf_false;
    LPS_DBG();

    if (WF_CANNOT_RUN(pnic_info))
    {
        wf_xmit_buf_delete(pnic_info->tx_info, mgnt_frame_ptr->pxmitbuf);
        wf_xmit_frame_delete(pnic_info->tx_info, mgnt_frame_ptr);
        LPS_DBG(" fail: pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped");
        return ret;
    }

    ret = lps_mpdu_insert_sending_queue(pnic_info, mgnt_frame_ptr);
    return ret;
}

static void lps_set_fw_rsvd_page(nic_info_st *pnic_info)
{
    wf_u8 rsvd_page_num = 0;
    wf_u32 max_rsvd_page_buff_size = 0;
    wf_u32 page_size = 128;// Unit byte
    struct xmit_frame *cmd_frame_ptr;
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
    lps_rsvd_page_chip_hw_construct_beacon(pnic_info, &reserved_page_packet[buff_index], &beacon_length);
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
    rsvd_page_loc.LocPsPoll = total_page_number;
    lps_rsvd_page_chip_hw_construct_pspoll(pnic_info, &reserved_page_packet[buff_index], &ps_poll_length);
    lps_fill_fake_txdesc(pnic_info, &reserved_page_packet[buff_index - TXDESC_SIZE],
                         ps_poll_length, wf_true, wf_false, wf_false); // ???????
    current_packet_page_num = (wf_u8)PageNum(TXDESC_SIZE + ps_poll_length, page_size);
    total_page_number = total_page_number + current_packet_page_num;
    buff_index = buff_index + current_packet_page_num * page_size;

    // null data * 1 page
    rsvd_page_loc.LocNullData = total_page_number;
    lps_rsvd_page_chip_hw_construct_nullfunctiondata(pnic_info, &reserved_page_packet[buff_index],
            &null_data_length, wf_wlan_get_cur_bssid(pnic_info),
            wf_false, 0, 0, wf_false);
    lps_fill_fake_txdesc(pnic_info, &reserved_page_packet[buff_index - TXDESC_SIZE],
                         null_data_length, wf_false, wf_false, wf_false);
    current_packet_page_num = (wf_u8)PageNum(null_data_length + TXDESC_SIZE, page_size);
    total_page_number = total_page_number + current_packet_page_num;
    buff_index = buff_index + current_packet_page_num * page_size;

    // Qos null data * 1 page
    rsvd_page_loc.LocQosNull = total_page_number;
    lps_rsvd_page_chip_hw_construct_nullfunctiondata(pnic_info, &reserved_page_packet[buff_index],
            &qos_null_length, wf_wlan_get_cur_bssid(pnic_info),
            wf_true, 0, 0, wf_false);
    lps_fill_fake_txdesc(pnic_info, &reserved_page_packet[buff_index - TXDESC_SIZE],
                         qos_null_length, wf_false, wf_false, wf_false);
    current_packet_page_num = (wf_u8)PageNum(qos_null_length + TXDESC_SIZE, page_size);
    total_page_number = total_page_number + current_packet_page_num;
    total_packets_len = buff_index + qos_null_length; // Do not contain TXDESC_SIZE of next packet
    buff_index = buff_index + current_packet_page_num * page_size;

    if (total_packets_len > max_rsvd_page_buff_size)
    {
        LPS_DBG(" Rsvd page size is not enough! total_packets_len: %d, max_rsvd_page_buff_size: %d", total_packets_len, max_rsvd_page_buff_size);
    }
    else
    {
        // update attribute
        lps_rsvd_page_mgntframe_attrib_update(pnic_info, cmd_frame_ptr);
        cmd_frame_ptr->qsel = QSLT_BEACON;
        cmd_frame_ptr->pktlen= total_packets_len - TXDESC_OFFSET;       // ???????

        ret = lps_resv_page_xmit(pnic_info, cmd_frame_ptr);
        if (ret == wf_false)
        {
            LPS_DBG(" fail: lps_resv_page_xmit: %d", ret);
        }
    }

    printk("Set RSVD page location to FW, total packet len: %d, total page num: %d\r\n",
           total_packets_len, total_page_number);
    LPS_DBG(" ProbeRsp: %d, PsPoll: %d, NullData: %d, QosNull: %d, BTNull: %d\r\n",
            rsvd_page_loc.LocProbeRsp, rsvd_page_loc.LocPsPoll, rsvd_page_loc.LocNullData, rsvd_page_loc.LocQosNull, rsvd_page_loc.LocBTQosNull);

    wf_mlme_get_connect(pnic_info, &b_connect);

    if (b_connect == wf_true)
    {
        if (wf_mcu_set_rsvd_page_h2c_loc(pnic_info, &rsvd_page_loc) == WF_RETURN_FAIL)
        {
            LPS_WARN(" fail: lps_set_rsvd_page_h2c_loc");
        }
    }
}

static int wf_lps_start (nic_info_st *pnic_info, wf_u8 lps_ctrl_type)
{
    mlme_info_t *pmlme_info;
    wf_msg_que_t *pmsg_que;
    wf_msg_t *pmsg;
    mlme_lps_t *param;
    int rst;

    LPS_DBG();

    if (pnic_info == NULL)
    {
        return -1;
    }

    if (!pnic_info->is_up)
    {
        LPS_WARN("ndev down");
        return -2;
    }

    pmlme_info = pnic_info->mlme_info;
    pmsg_que = &pmlme_info->msg_que;

    rst = wf_msg_new(pmsg_que, WF_MLME_TAG_LPS, &pmsg);
    if (rst)
    {
        LPS_WARN("msg new fail error code: %d", rst);
        return -3;
    }
    param = (mlme_lps_t *)pmsg->value;

    param->lps_ctrl_type = lps_ctrl_type;

    rst = wf_msg_push(pmsg_que, pmsg);
    if (rst)
    {
        wf_msg_del(pmsg_que, pmsg);
        LPS_WARN("msg push fail error code: %d", rst);
        return -4;
    }

    return 0;
}

void wf_lps_ctrl_wk_hdl(nic_info_st *pnic_info, wf_u8 lps_ctrl_type)
{
    pwr_info_st *ppwr_info = (pwr_info_st *)pnic_info->pwr_info;
    wf_bool lps_flag = wf_false, bconnect;

    LPS_DBG();

    if (wf_local_cfg_get_work_mode(pnic_info) == WF_ADHOC_MODE)
    {
        LPS_DBG(" return: nic mode is ADHOC, break!!!");
        return;
    }

    LPS_DBG("lps_ctrl_type == %d", lps_ctrl_type);
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
            ppwr_info->lps_idle_cnts = 0;
#if USE_M0_LPS_INTERFACES
            wf_mcu_set_lps_config(pnic_info);
#endif
            wf_mcu_set_fw_lps_config(pnic_info);
            lps_set_fw_rsvd_page(pnic_info);
            wf_mcu_set_fw_lps_get(pnic_info);

            lps_flag = wf_false;
            break;
        case LPS_CTRL_SPECIAL_PACKET:
            ppwr_info->delay_lps_last_timestamp = wf_os_api_timestamp();
            atomic_set(&ppwr_info->lps_spc_flag, 0);
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
        lps_enter(pnic_info, (char *)(LPS_CTRL_TYPE_STR[lps_ctrl_type]));
    }
    else
    {
        lps_exit(pnic_info, (char *)(LPS_CTRL_TYPE_STR[lps_ctrl_type]));
    }
}

wf_u32 wf_lps_wakeup(nic_info_st *pnic_info, wf_u8 lps_ctrl_type, wf_bool enqueue) // Enqueue for interrupt
{
    wf_bool bconnect, wakeup_flag = wf_false;
    wf_u32 ret = WF_RETURN_OK;

    LPS_DBG();

    if(WF_CANNOT_RUN(pnic_info))
    {
        LPS_WARN("WF_CANNOT_RUN = true Skip!");
        return -1;
    }

    if (lps_ctrl_type == LPS_CTRL_ENTER)
    {
        LPS_WARN("Error: lps ctrl type not support!");
        return -2;
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
            wakeup_flag = wf_true;
        }

        if(!bconnect)
        {
            LPS_INFO("driver not connected!!!");
            return ret;
        }
    }

    if (wakeup_flag == wf_true)
    {
        if (enqueue == wf_true)
        {
            wf_lps_start(pnic_info, lps_ctrl_type);
        }
        else
        {
            wf_lps_ctrl_wk_hdl(pnic_info, lps_ctrl_type);
        }
    }

    return ret;
}

wf_u32 wf_lps_sleep(nic_info_st *pnic_info, wf_u8 lps_ctrl_type, wf_bool enqueue) // Enqueue for interrupt
{
    pwr_info_st *pwr_info_ptr;
    wf_u32 lps_deny = 0;
    wf_u32 ret = WF_RETURN_OK;
    wf_bool bconnect;

    LPS_DBG();

    pwr_info_ptr = (pwr_info_st *)pnic_info->pwr_info;

    if (wf_local_cfg_get_work_mode(pnic_info) != WF_INFRA_MODE)
    {
        LPS_INFO("lps only supports the STA mode");
        return WF_RETURN_FAIL;
    }

    wf_mlme_get_connect(pnic_info, &bconnect);
    if(!bconnect)
    {
        LPS_INFO("driver not connected!!!");
        return ret;
    }

    _lps_enter_lps_lock(&pwr_info_ptr->lock);
    lps_deny = pwr_info_ptr->lps_deny;
    _lps_exit_lps_lock(&pwr_info_ptr->lock);
    if (lps_deny != 0)
    {
        LPS_INFO("Skip: can not sleep! Reason: %d", lps_deny);
        return WF_RETURN_FAIL;
    }

    if (lps_ctrl_type != LPS_CTRL_ENTER)
    {
        LPS_WARN("Error: lps ctrl type not support!");
        return WF_RETURN_FAIL;
    }

    if (enqueue == wf_true)
    {
        wf_lps_start(pnic_info, lps_ctrl_type);
    }
    else
    {
        wf_lps_ctrl_wk_hdl(pnic_info, lps_ctrl_type);
    }

    return ret;
}

wf_pt_ret_t wf_lps_sleep_mlme_monitor(wf_pt_t *pt, nic_info_st *pnic_info)
{
    mlme_info_t *pmlme_info  = (mlme_info_t*)pnic_info->mlme_info;
    pwr_info_st *ppwr_info = (pwr_info_st *)pnic_info->pwr_info;
    wf_u8 n_assoc_iface = 0;
    wf_bool bconnect;
    mlme_state_e state;

    PT_BEGIN(pt);

    LPS_DBG();

    for(;;)
    {
        wf_timer_set(&ppwr_info->lps_timer, 2000);
        PT_WAIT_UNTIL(pt, wf_timer_expired(&ppwr_info->lps_timer));

        wf_mlme_get_state(pnic_info, &state);
        if (state != MLME_STATE_IDLE)
        {
            wf_timer_set(&ppwr_info->lps_timer, 2000);
            PT_WAIT_UNTIL(pt, wf_timer_expired(&ppwr_info->lps_timer));
            continue;
        }
        pmlme_info->link_info.num_tx_ok_in_period = 0;
        pmlme_info->link_info.num_rx_unicast_ok_in_period = 0;

        PT_WAIT_WHILE(pt, pmlme_info->link_info.busy_traffic);

        wf_mlme_get_connect(pnic_info, &bconnect);
        if (bconnect)
        {
            n_assoc_iface++;
        }
        if (n_assoc_iface == 0)
        {
            if (ppwr_info->pwr_current_mode != PWR_MODE_ACTIVE)
            {
                wf_lps_wakeup(pnic_info, LPS_CTRL_NO_LINKED, wf_true);
                continue;
            }
        }
        else
        {
            if(bconnect)
            {
                LPS_DBG("pkt_num == %d", pmlme_info->link_info.num_rx_unicast_ok_in_period +
                        pmlme_info->link_info.num_tx_ok_in_period);
                LPS_DBG("ppwr_info->pwr_current_mode = %d", ppwr_info->pwr_current_mode);
                if (pmlme_info->link_info.num_rx_unicast_ok_in_period +
                    pmlme_info->link_info.num_tx_ok_in_period > 8)
                {
                    if (ppwr_info->pwr_current_mode != PWR_MODE_ACTIVE)
                    {
                        wf_lps_wakeup(pnic_info, LPS_CTRL_TRAFFIC_BUSY, wf_true);
                        continue;
                    }
                    else
                    {
                        LPS_DBG(" now is awake");
                        continue;
                    }
                }
                else
                {
                    if (ppwr_info->pwr_current_mode == PWR_MODE_ACTIVE)
                    {
                        wf_lps_sleep(pnic_info, LPS_CTRL_ENTER, wf_true);
                        continue;
                    }
                    else
                    {
                        LPS_DBG(" now is sleeping");
                        continue;
                    }
                }
            }
            else
            {
                wf_lps_wakeup(pnic_info, LPS_CTRL_TRAFFIC_BUSY, wf_true);
                continue;
            }
        }
    }
    PT_END(pt);
}
#endif

wf_s32 wf_lps_init(nic_info_st *pnic_info)
{
    pwr_info_st *pwr_info;

    LPS_DBG();

    pwr_info = wf_kzalloc(sizeof(pwr_info_st));
    if (pwr_info == NULL)
    {
        LPS_WARN("[LPS] malloc lps_param_st failed");
        return -1;
    }
    else
    {
        pnic_info->pwr_info = (void *)pwr_info;
    }
#ifdef CONFIG_LPS
    _lps_init_lps_lock(&pwr_info->lock); // Init lock
    _lps_init_lps_lock(&pwr_info->check_32k_lock);

    pwr_info->rf_pwr_state = rf_on;
    pwr_info->lps_enter_cnts = 0;
    pwr_info->lps_idle_cnts = 0;
    pwr_info->rpwm = 0;
    pwr_info->b_fw_current_in_ps_mode = wf_false;
    pwr_info->pwr_current_mode = PWR_MODE_ACTIVE;
    pwr_info->smart_lps = 2; // According to 9083

    // Set pwr_info->pwr_mgmt according to registry_par->power_mgnt and registrypriv.mp_mode in 9083
    pwr_info->pwr_mgnt = PWR_MODE_MAX;
#endif
    pwr_info->bInSuspend = wf_false;
    return 0;
}

wf_s32 wf_lps_term(nic_info_st *pnic_info)
{
    pwr_info_st *pwr_info;

    LPS_DBG();

    if (pnic_info == NULL)
    {
        return 0;
    }
    pwr_info = pnic_info->pwr_info;

    if (pwr_info)
    {
        wf_kfree(pwr_info);
        pnic_info->pwr_info = NULL;
    }
    return 0;
}



