#include "common.h"
#include "wf_debug.h"

#ifdef CONFIG_P2P
typedef wf_s32 (*proto_handle)(nic_info_st *nic_info);
typedef struct p2p_proto_ops_st_
{
    P2P_PROTO_WK_ID id;
    proto_handle proto_func;
}p2p_proto_ops_st;


wf_bool p2p_check_buddy_linkstate(nic_info_st *nic_info)
{
    wf_bool  bconnect = wf_false;
#ifdef CONFIG_CONCURRENT_MODE
    nic_info_st *pbuddy_nic = nic_info->vir_nic;
    wf_mlme_get_connect(pbuddy_nic,&bconnect);
#endif
    return bconnect;
}


#ifdef CONFIG_CONCURRENT_MODE
static wf_s32 handler_p2p_concurrent_func(nic_info_st *nic_info)
{
    #if 0
    struct wifidirect_info *pwdinfo = &pwadptdata->wdinfo;
    struct mlme_ext_priv *pmlmeext = &(pwadptdata->mlmeextpriv);
    struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
    u8 val8;
    _func_enter_;

    
    if (do_chk_partner_fwstate(pwadptdata, _FW_LINKED)) {
        PwADPTDATA pbuddy_wadptdata = pwadptdata->pbuddy_wadptdata;
        struct mlme_ext_priv *pbuddy_mlmeext = &pbuddy_wadptdata->mlmeextpriv;

        pwdinfo->operating_channel = pbuddy_mlmeext->cur_channel;

        if (pwdinfo->driver_interface == DRIVER_CFG80211) {
            WL_INFO("%s, switch ch back to buddy's cur_channel=%d\n", __func__,
                    pbuddy_mlmeext->cur_channel);

            if (flag) {
                do_set_chabw(pwadptdata, pbuddy_mlmeext->cur_channel,
                                   pbuddy_mlmeext->cur_ch_offset,
                                   pbuddy_mlmeext->cur_bwmode);
            }
            if (do_chk_partner_fwstate(pwadptdata, WIFI_FW_STATION_STATE))
                nulldata_to_pre_issue_func(pbuddy_wadptdata, NULL, 0, 3, 500);
        } else if (pwdinfo->driver_interface == DRIVER_WEXT) {
            if (wl_p2p_chk_state(pwdinfo, P2P_STATE_IDLE)) {
                if (pwdinfo->ext_listen_period > 0) {
                    WL_INFO("[%s] P2P_STATE_IDLE, ext_listen_period = %d\n",
                            __FUNCTION__, pwdinfo->ext_listen_period);

                    if (pbuddy_mlmeext->cur_channel != pwdinfo->listen_channel) {
                        nulldata_to_pre_issue_func(pbuddy_wadptdata, NULL, 1, 3, 500);
                        do_set_chabw(pwadptdata, pwdinfo->listen_channel,
                                           HAL_PRIME_CHNL_OFFSET_DONT_CARE,
                                           CHANNEL_WIDTH_20);
                    }

                    wl_p2p_set_state(pwdinfo, P2P_STATE_LISTEN);
                    if (!buddy_mlmeinfo_state_to_check_func
                        (pwadptdata, WIFI_FW_AP_STATE, 1)
                        && ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)) {
                        val8 = 1;
                        Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_MLME_SITESURVEY,
                                          (u8 *) (&val8));
                    }
                    _set_timer(&pwdinfo->ap_p2p_switch_timer,
                               pwdinfo->ext_listen_period);
                }
            } else if (wl_p2p_chk_state(pwdinfo, P2P_STATE_LISTEN) ||
                       wl_p2p_chk_state(pwdinfo, P2P_STATE_GONEGO_FAIL) ||
                       (wl_p2p_chk_state(pwdinfo, P2P_STATE_GONEGO_ING)
                        && pwdinfo->nego_req_info.benable == _FALSE)
                       || wl_p2p_chk_state(pwdinfo,
                                            P2P_STATE_RX_PROVISION_DIS_REQ)) {
                WL_INFO("[%s] P2P_STATE_IDLE, ext_listen_interval = %d\n",
                        __FUNCTION__, pwdinfo->ext_listen_interval);

                if (pbuddy_mlmeext->cur_channel != pwdinfo->listen_channel) {
                    do_set_chabw(pwadptdata, pbuddy_mlmeext->cur_channel,
                                       pbuddy_mlmeext->cur_ch_offset,
                                       pbuddy_mlmeext->cur_bwmode);
                    if (!buddy_mlmeinfo_state_to_check_func
                        (pwadptdata, WIFI_FW_AP_STATE, 1)
                        && ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)) {
                        val8 = 0;
                        Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_MLME_SITESURVEY,
                                          (u8 *) (&val8));
                    }
                    wl_p2p_set_state(pwdinfo, P2P_STATE_IDLE);
                    nulldata_to_pre_issue_func(pbuddy_wadptdata, NULL, 0, 3, 500);
                }

                _set_timer(&pwdinfo->ap_p2p_switch_timer,
                           pwdinfo->ext_listen_interval);
            } else if (wl_p2p_chk_state(pwdinfo, P2P_STATE_GONEGO_OK)) {
                val8 = 0;
                Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_MLME_SITESURVEY,
                                  (u8 *) (&val8));
                do_set_chabw(pwadptdata, pbuddy_mlmeext->cur_channel,
                                   pbuddy_mlmeext->cur_ch_offset,
                                   pbuddy_mlmeext->cur_bwmode);
                nulldata_to_pre_issue_func(pbuddy_wadptdata, NULL, 0, 3, 500);
            } else
                if (wl_p2p_chk_state(pwdinfo, P2P_STATE_TX_PROVISION_DIS_REQ))
            {
                val8 = 1;
                do_set_chabw(pwadptdata,
                                   pwdinfo->tx_prov_disc_info.
                                   peer_channel_num[0],
                                   HAL_PRIME_CHNL_OFFSET_DONT_CARE,
                                   CHANNEL_WIDTH_20);
                Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_MLME_SITESURVEY,
                                  (u8 *) (&val8));
                probereq_p2p_to_pre_issue_func(pwadptdata, NULL);
                _set_timer(&pwdinfo->pre_tx_scan_timer, P2P_TX_PRESCAN_TIMEOUT);
            } else if (wl_p2p_chk_state(pwdinfo, P2P_STATE_GONEGO_ING)
                       && pwdinfo->nego_req_info.benable == _TRUE) {
                val8 = 1;
                do_set_chabw(pwadptdata,
                                   pwdinfo->nego_req_info.peer_channel_num[0],
                                   HAL_PRIME_CHNL_OFFSET_DONT_CARE,
                                   CHANNEL_WIDTH_20);
                Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_MLME_SITESURVEY,
                                  (u8 *) (&val8));
                probereq_p2p_to_pre_issue_func(pwadptdata, NULL);
                _set_timer(&pwdinfo->pre_tx_scan_timer, P2P_TX_PRESCAN_TIMEOUT);
            } else if (wl_p2p_chk_state(pwdinfo, P2P_STATE_TX_INVITE_REQ)
                       && pwdinfo->invitereq_info.benable == _TRUE) {
            }
        }
    } else {
        if (!wl_p2p_chk_state(pwdinfo, P2P_STATE_GONEGO_OK)
            || pwadptdata->registrypriv.full_ch_in_p2p_handshake == 0)
            do_set_chabw(pwadptdata, pwdinfo->listen_channel,
                               HAL_PRIME_CHNL_OFFSET_DONT_CARE,
                               CHANNEL_WIDTH_20);
        else
            WL_INFO
                ("%s, buddy not linked, go nego ok, not back to listen channel\n",
                 __func__);
    }

    _func_exit_;
    #else
    p2p_info_st *p2p_info = nic_info->p2p;
    struct wifidirect_info *pwdinfo = &p2p_info->wdinfo;
    if(p2p_check_buddy_linkstate(nic_info))
    {
        pwdinfo->operating_channel = wf_wlan_get_cur_channel(nic_info->vir_nic);
        if (pwdinfo->driver_interface == DRIVER_CFG80211) 
        {
            wf_u8 current_bwmode = wf_wlan_get_cur_bw(nic_info->vir_nic);

             wf_hw_info_set_channnel_bw(nic_info,pwdinfo->operating_channel,current_bwmode,HAL_PRIME_CHNL_OFFSET_DONT_CARE);
            
            //if (do_chk_partner_fwstate(pwadptdata, WIFI_FW_STATION_STATE))
            //    nulldata_to_pre_issue_func(pbuddy_wadptdata, NULL, 0, 3, 500);
        }
        else
        {
            LOG_I("WEXT for p2p to be doing.");
        }
    }
    else
    {
        if(P2P_STATE_GONEGO_OK != pwdinfo->p2p_state)
        {
            wf_hw_info_set_channnel_bw(nic_info,pwdinfo->listen_channel,CHANNEL_WIDTH_20,HAL_PRIME_CHNL_OFFSET_DONT_CARE);
        }
    }
    #endif
    return 0;
}
#endif

static wf_s32 handler_find_phase_func(nic_info_st * nic_info)
{
    #if 0
    struct wifidirect_info *pwdinfo = &pwadptdata->wdinfo;
    struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;
    NDIS_802_11_SSID ssid;
    _irqL irqL;
    u8 _status = 0;

    _func_enter_;

    if (flag) {
        memset((unsigned char *)&ssid, 0, sizeof(NDIS_802_11_SSID));
        Func_Of_Proc_Pre_Memcpy(ssid.Ssid, pwdinfo->p2p_wildcard_ssid,
                    P2P_WILDCARD_SSID_LEN);
        ssid.SsidLength = P2P_WILDCARD_SSID_LEN;
    }
    wl_p2p_set_state(pwdinfo, P2P_STATE_FIND_PHASE_SEARCH);

    spin_lock_bh(&pmlmepriv->lock);
    _status = proc_sitesurvey_cmd_func(pwadptdata, &ssid, 1, NULL, 0);
    spin_unlock_bh(&pmlmepriv->lock);

    _func_exit_;
    #else
    p2p_info_st *p2p_info = nic_info->p2p;
    struct wifidirect_info *pwdinfo = &p2p_info->wdinfo;
    
    //wf_scan_start(nic_info,SCAN_TYPE_ACTIVE,NULL,pwdinfo->p2p_wildcard_ssid,1,);
    #endif
    return 0;
}

static wf_s32 p2p_state_handler_restore_func(nic_info_st *nic_info)
{
    #if 0
    struct wifidirect_info *pwdinfo = &pwadptdata->wdinfo;
    struct mlme_priv *pmlmepriv = &pwadptdata->mlmepriv;

    _func_enter_;

    if (wl_p2p_chk_state(pwdinfo, P2P_STATE_GONEGO_ING)
        || wl_p2p_chk_state(pwdinfo, P2P_STATE_GONEGO_FAIL)) {
        wl_p2p_set_role(pwdinfo, P2P_ROLE_DEVICE);
    }
#ifdef CONFIG_CONCURRENT_MODE
    if (do_chk_partner_fwstate(pwadptdata, _FW_LINKED)) {
        _wadptdata *pbuddy_wadptdata = pwadptdata->pbuddy_wadptdata;
        struct mlme_priv *pbuddy_mlmepriv = &pbuddy_wadptdata->mlmepriv;
        struct mlme_ext_priv *pbuddy_mlmeext = &pbuddy_wadptdata->mlmeextpriv;

        if (wl_p2p_chk_state(pwdinfo, P2P_STATE_TX_PROVISION_DIS_REQ)
            || wl_p2p_chk_state(pwdinfo, P2P_STATE_RX_PROVISION_DIS_RSP)) {
            do_set_chabw(pwadptdata, pbuddy_mlmeext->cur_channel,
                               pbuddy_mlmeext->cur_ch_offset,
                               pbuddy_mlmeext->cur_bwmode);

            nulldata_to_pre_issue_func(pbuddy_wadptdata, NULL, 0, 3, 500);
        }
    }
#endif

    if (flag) {
        wl_p2p_set_state(pwdinfo, wl_p2p_pre_state(pwdinfo));
    }
    if (wl_p2p_chk_role(pwdinfo, P2P_ROLE_DEVICE)) {
#ifdef CONFIG_CONCURRENT_MODE
        handler_p2p_concurrent_func(pwadptdata, 1);
#else
        do_set_chabw(pwadptdata, pwdinfo->listen_channel,
                           HAL_PRIME_CHNL_OFFSET_DONT_CARE, CHANNEL_WIDTH_20);
#endif
    }
    _func_exit_;
    #else
    p2p_info_st *p2p_info = nic_info->p2p;
    struct wifidirect_info *pwdinfo = &p2p_info->wdinfo;
    
    if(P2P_STATE_GONEGO_ING == pwdinfo->p2p_state || P2P_STATE_GONEGO_FAIL == pwdinfo->p2p_state)
    {
        p2p_set_role(pwdinfo, P2P_ROLE_DEVICE);
    }
#ifdef CONFIG_CONCURRENT_MODE
    if(p2p_check_buddy_linkstate(nic_info))
    {
        if(P2P_STATE_TX_PROVISION_DIS_REQ == pwdinfo->p2p_state  || P2P_STATE_RX_PROVISION_DIS_RSP == pwdinfo->p2p_state)
        {
            wf_u8 cur_channel = wf_wlan_get_cur_channel(nic_info->vir_nic);
            wf_u8 cur_bwmode  = wf_wlan_get_cur_bw(nic_info->vir_nic);
            wf_hw_info_set_channnel_bw(nic_info->vir_nic,cur_channel,cur_bwmode,HAL_PRIME_CHNL_OFFSET_DONT_CARE);
            //nulldata_to_pre_issue_func(pbuddy_wadptdata, NULL, 0, 3, 500);
        }
    }
#endif
    p2p_set_state(pwdinfo, pwdinfo->pre_p2p_state);

    if (P2P_ROLE_DEVICE == pwdinfo->role ) 
    {
#ifdef CONFIG_CONCURRENT_MODE
        handler_p2p_concurrent_func(nic_info);
#else
        wf_hw_info_set_channnel_bw(nic_info,pwdinfo->listen_channel,CHANNEL_WIDTH_20,HAL_PRIME_CHNL_OFFSET_DONT_CARE);
#endif
    }
    #endif
    
    return 0;
}


static wf_s32 handler_pre_tx_provdisc_func(nic_info_st *nic_info)
{
    #if 0
    struct wifidirect_info *pwdinfo = &pwadptdata->wdinfo;
    u8 val8 = 1;
    _func_enter_;

    #ifdef CONFIG_CONCURRENT_MODE
     if (do_chk_partner_fwstate(pwadptdata, _FW_LINKED))
     {
        handler_p2p_concurrent_func(pwadptdata, 1);
     }
    #endif
        do_set_chabw(pwadptdata,  pwdinfo->tx_prov_disc_info.peer_channel_num[0],
                           HAL_PRIME_CHNL_OFFSET_DONT_CARE, CHANNEL_WIDTH_20);
        Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_MLME_SITESURVEY, (u8 *) (&val8));
        probereq_p2p_to_pre_issue_func(pwadptdata, NULL);
        _set_timer(&pwdinfo->pre_tx_scan_timer, P2P_TX_PRESCAN_TIMEOUT);
    #else
    p2p_info_st *p2p_info = nic_info->p2p;
    struct wifidirect_info *pwdinfo = &p2p_info->wdinfo;
    #ifdef CONFIG_CONCURRENT_MODE
    if(p2p_check_buddy_linkstate(nic_info))
    {
        handler_p2p_concurrent_func(nic_info);
    }
    #endif
    wf_hw_info_set_channnel_bw(nic_info,pwdinfo->tx_prov_disc_info.peer_channel_num[0],CHANNEL_WIDTH_20,HAL_PRIME_CHNL_OFFSET_DONT_CARE);
    wf_mcu_set_mlme_scan(nic_info,wf_true);
    wf_os_api_timer_set(&p2p_info->pre_tx_scan_timer, P2P_TX_PRESCAN_TIMEOUT);
    #endif

    return 0;
}

static wf_s32 handler_pre_tx_invitereq_func(nic_info_st *nic_info)
{
    #if 0
    struct wifidirect_info *pwdinfo = &pwadptdata->wdinfo;
    u8 val8 = 1;
    _func_enter_;

    #ifdef CONFIG_CONCURRENT_MODE
     if (do_chk_partner_fwstate(pwadptdata, _FW_LINKED))
     {
        handler_p2p_concurrent_func(pwadptdata, 1);
     }
    #endif
    
    if (flag) {
        do_set_chabw(pwadptdata, pwdinfo->invitereq_info.peer_ch,
                           HAL_PRIME_CHNL_OFFSET_DONT_CARE, CHANNEL_WIDTH_20);
        Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_MLME_SITESURVEY, (u8 *) (&val8));
        probereq_p2p_to_pre_issue_func(pwadptdata, NULL);
        _set_timer(&pwdinfo->pre_tx_scan_timer, P2P_TX_PRESCAN_TIMEOUT);
    }
    _func_exit_;
    #else
    p2p_info_st *p2p_info = nic_info->p2p;
    struct wifidirect_info *pwdinfo = &p2p_info->wdinfo;
    #ifdef CONFIG_CONCURRENT_MODE
    if(p2p_check_buddy_linkstate(nic_info))
    {
        handler_p2p_concurrent_func(nic_info);
    }
    #endif
    wf_hw_info_set_channnel_bw(nic_info,pwdinfo->invitereq_info.peer_ch,CHANNEL_WIDTH_20,HAL_PRIME_CHNL_OFFSET_DONT_CARE);
    wf_mcu_set_mlme_scan(nic_info,wf_true);
    wf_os_api_timer_set(&p2p_info->pre_tx_scan_timer, P2P_TX_PRESCAN_TIMEOUT);
    #endif

    return 0;
}

static wf_s32 handler_pre_tx_negoreq_func(nic_info_st *nic_info)
{
    #if 0
    struct wifidirect_info *pwdinfo = &pwadptdata->wdinfo;
    u8 val8 = 1;
    _func_enter_;
    #ifdef CONFIG_CONCURRENT_MODE
     if (do_chk_partner_fwstate(pwadptdata, _FW_LINKED))
     {
        handler_p2p_concurrent_func(pwadptdata, 1);
     }
    #endif
    if (flag) {
        do_set_chabw(pwadptdata, pwdinfo->nego_req_info.peer_channel_num[0],
                           HAL_PRIME_CHNL_OFFSET_DONT_CARE, CHANNEL_WIDTH_20);
        Func_Of_Proc_Chip_Hw_Set_Hwreg(pwadptdata, HW_VAR_MLME_SITESURVEY, (u8 *) (&val8));
        probereq_p2p_to_pre_issue_func(pwadptdata, NULL);
        probereq_p2p_to_pre_issue_func(pwadptdata, pwdinfo->nego_req_info.peerDevAddr);
        _set_timer(&pwdinfo->pre_tx_scan_timer, P2P_TX_PRESCAN_TIMEOUT);
    }
    _func_exit_;
    #else
    p2p_info_st *p2p_info = nic_info->p2p;
    struct wifidirect_info *pwdinfo = &p2p_info->wdinfo;
    #ifdef CONFIG_CONCURRENT_MODE
    if(p2p_check_buddy_linkstate(nic_info))
    {
        handler_p2p_concurrent_func(nic_info);
    }
    #endif
    wf_hw_info_set_channnel_bw(nic_info,pwdinfo->nego_req_info.peer_channel_num[0],CHANNEL_WIDTH_20,HAL_PRIME_CHNL_OFFSET_DONT_CARE);
    wf_mcu_set_mlme_scan(nic_info,wf_true);
    //probereq_p2p_to_pre_issue_func(pwadptdata, NULL);
    //probereq_p2p_to_pre_issue_func(pwadptdata, pwdinfo->nego_req_info.peerDevAddr);
    wf_os_api_timer_set(&p2p_info->pre_tx_scan_timer, P2P_TX_PRESCAN_TIMEOUT);
    #endif
    return 0;
}

static wf_s32 handler_ro_ch_func(nic_info_st *nic_info)
{
    #if 0
    struct cfg80211_wifidirect_info *pcfg80211_wdinfo =
        &pwadptdata->cfg80211_wdinfo;
    struct wifidirect_info *pwdinfo = &pwadptdata->wdinfo;
    struct mlme_ext_priv *pmlmeext = &pwadptdata->mlmeextpriv;
    u8 ch, bw, offset;
    _func_enter_;

    if (proc_get_ch_setting_union_func(pwadptdata, &ch, &bw, &offset, 1) != 0) {
        if (0)
            WL_INFO(FUNC_ADPT_FMT
                    " back to linked/linking union - ch:%u, bw:%u, offset:%u\n",
                    FUNC_ADPT_ARG(pwadptdata), ch, bw, offset);
    } else if (wadptdata_wdev_data(pwadptdata)->p2p_enabled
               && pwdinfo->listen_channel) {
        ch = pwdinfo->listen_channel;
        bw = CHANNEL_WIDTH_20;
        offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
        if (0)
            WL_INFO(FUNC_ADPT_FMT
                    " back to listen ch - ch:%u, bw:%u, offset:%u\n",
                    FUNC_ADPT_ARG(pwadptdata), ch, bw, offset);
    } else {
        ch = pcfg80211_wdinfo->restore_channel;
        bw = CHANNEL_WIDTH_20;
        offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
        if (0)
            WL_INFO(FUNC_ADPT_FMT
                    " back to restore ch - ch:%u, bw:%u, offset:%u\n",
                    FUNC_ADPT_ARG(pwadptdata), ch, bw, offset);
    }

    if (flag) {
        do_set_chabw(pwadptdata, ch, offset, bw);
    }
    wl_p2p_set_state(pwdinfo, wl_p2p_pre_state(pwdinfo));
#ifdef CONFIG_DEBUG_CFG80211
    WL_INFO("%s, role=%d, p2p_state=%d\n", __func__, wl_p2p_role(pwdinfo),
            wl_p2p_state(pwdinfo));
#endif

    pcfg80211_wdinfo->is_ro_ch = _FALSE;
    pcfg80211_wdinfo->last_ro_ch_time = Func_Of_Proc_Get_Current_Time();

    WL_INFO
        ("cfg80211_remain_on_channel_expired cookie:0x%llx, ch=%d, bw=%d, offset=%d\n",
         pcfg80211_wdinfo->remain_on_ch_cookie, do_query_var(pwadptdata, CHANNEL),
         do_query_var(pwadptdata, BW), do_query_var(pwadptdata, CH_OFFSET));

    wl_cfg80211_remain_on_channel_expired(pwadptdata,
                                           pcfg80211_wdinfo->
                                           remain_on_ch_cookie,
                                           &pcfg80211_wdinfo->
                                           remain_on_ch_channel,
                                           pcfg80211_wdinfo->remain_on_ch_type,
                                           GFP_KERNEL);

    _func_exit_;
    #else
    #endif

    return 0;
}

static p2p_proto_ops_st gl_p2p_proto_funs[]=
{
    {P2P_FIND_PHASE_WK,handler_find_phase_func},
    {P2P_RESTORE_STATE_WK,p2p_state_handler_restore_func},
    {P2P_PRE_TX_PROVDISC_PROCESS_WK,handler_pre_tx_provdisc_func},
    {P2P_PRE_TX_NEGOREQ_PROCESS_WK,handler_pre_tx_negoreq_func},
    {P2P_PRE_TX_INVITEREQ_PROCESS_WK,handler_pre_tx_invitereq_func},
#ifdef CONFIG_CONCURRENT_MODE
    {P2P_AP_P2P_CH_SWITCH_PROCESS_WK,handler_p2p_concurrent_func},
#endif
    {P2P_RO_CH_WK,handler_ro_ch_func}
};

char *proto_id_to_str(P2P_PROTO_WK_ID proto_id)
{
    switch(proto_id)
    {
        case P2P_FIND_PHASE_WK:
            return to_str(P2P_FIND_PHASE_WK);
        case P2P_RESTORE_STATE_WK:
            return to_str(P2P_RESTORE_STATE_WK);
        case P2P_PRE_TX_PROVDISC_PROCESS_WK:
            return to_str(P2P_PRE_TX_PROVDISC_PROCESS_WK);
        case P2P_PRE_TX_NEGOREQ_PROCESS_WK:
            return to_str(P2P_PRE_TX_NEGOREQ_PROCESS_WK);
        case P2P_PRE_TX_INVITEREQ_PROCESS_WK:
            return to_str(P2P_PRE_TX_INVITEREQ_PROCESS_WK);
        case P2P_AP_P2P_CH_SWITCH_PROCESS_WK:
            return to_str(P2P_AP_P2P_CH_SWITCH_PROCESS_WK);
        case P2P_RO_CH_WK:
            return to_str(P2P_RO_CH_WK);
        default:
            return "unknown proto_id";
    }
}
void p2p_protocol_dispatch_entry(nic_info_st *nic_info, P2P_PROTO_WK_ID proto_id)
{
    wf_s32 ret = 0;
    if(P2P_FIND_PHASE_WK > proto_id || proto_id > P2P_RO_CH_WK)
    {
        LOG_W("%s() proto_id:%d",__func__,proto_id);
    }
    
    if(gl_p2p_proto_funs[proto_id].proto_func)
    {
        ret = gl_p2p_proto_funs[proto_id].proto_func(nic_info);
        if(WF_RETURN_OK != ret)
        {
            LOG_E("%s() %s failed",__func__,proto_id_to_str(proto_id));
        }
    }
}

#endif
