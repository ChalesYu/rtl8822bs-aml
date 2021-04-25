#include "common.h"
#include "wf_debug.h"

#ifdef CONFIG_P2P



static wf_u8 wl_basic_rate_cck[4] = 
{
    IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK
};

static wf_u8 wl_basic_rate_ofdm[3] = 
{
    IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK
};

static wf_u8 wl_basic_rate_mix[7] = 
{
    IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
    IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK
};

char* p2p_role_to_str(P2P_ROLE role)
{
    switch(role)
    {
        case P2P_ROLE_DISABLE: {return to_str(P2P_ROLE_DISABLE);}
        case P2P_ROLE_DEVICE: {return to_str(P2P_ROLE_DEVICE);}
        case P2P_ROLE_CLIENT: {return to_str(P2P_ROLE_CLIENT);}
        case P2P_ROLE_GO : {return to_str(P2P_ROLE_GO);}
        default:return "Unknown_p2p_role";
    }
    return NULL;
}
char *p2p_state_to_str(P2P_STATE state)
{
    switch(state)
    {
        case P2P_STATE_NONE                        : {return to_str(P2P_STATE_NONE);}
        case P2P_STATE_IDLE                         : {return to_str(P2P_STATE_IDLE);}
        case P2P_STATE_LISTEN                             : {return to_str(P2P_STATE_LISTEN);}
        case P2P_STATE_SCAN                        : {return to_str(P2P_STATE_SCAN);}
        case P2P_STATE_FIND_PHASE_LISTEN             : {return to_str(P2P_STATE_FIND_PHASE_LISTEN);}
        case P2P_STATE_FIND_PHASE_SEARCH            : {return to_str(P2P_STATE_FIND_PHASE_SEARCH);}
        case P2P_STATE_TX_PROVISION_DIS_REQ          : {return to_str(P2P_STATE_TX_PROVISION_DIS_REQ);}
        case P2P_STATE_RX_PROVISION_DIS_RSP         : {return to_str(P2P_STATE_RX_PROVISION_DIS_RSP);}
        case P2P_STATE_RX_PROVISION_DIS_REQ         : {return to_str(P2P_STATE_RX_PROVISION_DIS_REQ);}
        case P2P_STATE_GONEGO_ING                   : {return to_str(P2P_STATE_GONEGO_ING);}
        case P2P_STATE_GONEGO_OK                  : {return to_str(P2P_STATE_GONEGO_OK);}
        case P2P_STATE_GONEGO_FAIL                  : {return to_str(P2P_STATE_GONEGO_FAIL);}
        case P2P_STATE_RECV_INVITE_REQ_MATCH      : {return to_str(P2P_STATE_RECV_INVITE_REQ_MATCH);}
        case P2P_STATE_PROVISIONING_ING          : {return to_str(P2P_STATE_PROVISIONING_ING);}
        case P2P_STATE_PROVISIONING_DONE           : {return to_str(P2P_STATE_PROVISIONING_DONE);}
        case P2P_STATE_TX_INVITE_REQ               : {return to_str(P2P_STATE_TX_INVITE_REQ);}
        case P2P_STATE_RX_INVITE_RESP_OK            : {return to_str(P2P_STATE_RX_INVITE_RESP_OK);}
        case P2P_STATE_RECV_INVITE_REQ_DISMATCH     : {return to_str(P2P_STATE_RECV_INVITE_REQ_DISMATCH);}
        case P2P_STATE_RECV_INVITE_REQ_GO          : {return to_str(P2P_STATE_RECV_INVITE_REQ_GO);}
        case P2P_STATE_RECV_INVITE_REQ_JOIN       : {return to_str(P2P_STATE_RECV_INVITE_REQ_JOIN);}
        case P2P_STATE_RX_INVITE_RESP_FAIL         : {return to_str(P2P_STATE_RX_INVITE_RESP_FAIL);}
        case P2P_STATE_RX_INFOR_NOREADY            : {return to_str(P2P_STATE_RX_INFOR_NOREADY);}
        case P2P_STATE_TX_INFOR_NOREADY            : {return to_str(P2P_STATE_TX_INFOR_NOREADY);}
        default:return "Unknown_p2p_state";
    }
    return NULL;
}

static void p2p_do_renew_tx_rate(nic_info_st *nic_info, wf_u8 wirelessmode)
{
    p2p_info_st *p2p_info           = nic_info->p2p;
    struct wifidirect_info *pwdinfo = &p2p_info->wdinfo;
    wf_u8 *supported_rates = NULL;
    wf_u8 support_rate_cnt = 0;
    wf_u16 basic_dr_cfg;
    wf_s32 ret = 0;
    if (P2P_STATE_NONE !=pwdinfo->p2p_state)
    {
        return;
    }
    
    if ((wirelessmode & WIRELESS_11B) && (wirelessmode == WIRELESS_11B)) 
    {
        supported_rates =  wl_basic_rate_cck;
        support_rate_cnt = sizeof(wl_basic_rate_cck)/sizeof(wl_basic_rate_cck[0]);
    } 
    else if (wirelessmode & WIRELESS_11B) 
    {
        supported_rates =  wl_basic_rate_mix;
        support_rate_cnt = sizeof(wl_basic_rate_mix)/sizeof(wl_basic_rate_mix[0]);
    } 
    else 
    {
        supported_rates =  wl_basic_rate_ofdm;
        support_rate_cnt = sizeof(wl_basic_rate_ofdm)/sizeof(wl_basic_rate_ofdm[0]);
    }

    if (wirelessmode & WIRELESS_11B)
    {
        p2p_info->mgnt_tx_rate = IEEE80211_CCK_RATE_1MB;
    }
    else
    {
        p2p_info->mgnt_tx_rate = IEEE80211_OFDM_RATE_6MB;
    }

    get_bratecfg_by_support_dates(supported_rates, support_rate_cnt, &basic_dr_cfg);
    ret = wf_mcu_set_basic_rate(nic_info,  basic_dr_cfg);
    if(WF_RETURN_OK != ret)
    {
        LOG_E("%s()  wf_mcu_set_basic_rate failed",__func__);
    }
}


static void wifi_direct_info_init(struct wifidirect_info *pwdinfo)
{
#ifdef CONFIG_IOCTL_CFG80211
    pwdinfo->driver_interface = DRIVER_CFG80211;
#else
    pwdinfo->driver_interface = DRIVER_WEXT;
#endif
    
    /*  1, 6, 11 are the social channel defined in the WiFi Direct specification. */
    pwdinfo->social_chan[0] = 1;
    pwdinfo->social_chan[1] = 6;
    pwdinfo->social_chan[2] = 11;
    pwdinfo->social_chan[3] = 0;    /*  channel 0 for scanning ending in site survey function. */

    /*	Use the OFDM rate in the P2P probe response frame. ( 6(B), 9(B), 12, 18, 24, 36, 48, 54 )	 */
    pwdinfo->p2p_support_rate[0] = 0x8c;	/*	6(B) */
    pwdinfo->p2p_support_rate[1] = 0x92;	/*	9(B) */
    pwdinfo->p2p_support_rate[2] = 0x18;	/*	12 */
    pwdinfo->p2p_support_rate[3] = 0x24;	/*	18 */
    pwdinfo->p2p_support_rate[4] = 0x30;	/*	24 */
    pwdinfo->p2p_support_rate[5] = 0x48;	/*	36 */
    pwdinfo->p2p_support_rate[6] = 0x60;	/*	48 */
    pwdinfo->p2p_support_rate[7] = 0x6c;	/*	54 */

    pwdinfo->p2p_state = P2P_STATE_NONE;
    wf_memcpy((void *)pwdinfo->p2p_wildcard_ssid, "DIRECT-", 7);
    wf_memset(pwdinfo->device_name, 0x00, WPS_MAX_DEVICE_NAME_LEN);
    pwdinfo->device_name_len = 0;


    wf_memset(&pwdinfo->invitereq_info, 0x00, sizeof(struct tx_invite_req_info));
    pwdinfo->invitereq_info.token = 3;

    wf_memset(&pwdinfo->inviteresp_info, 0x00, sizeof(struct tx_invite_resp_info));
    pwdinfo->inviteresp_info.token = 0;


    pwdinfo->profileindex = 0;
    wf_memset(&pwdinfo->profileinfo[0], 0x00,sizeof(struct profile_info) * P2P_MAX_PERSISTENT_GROUP_NUM);

    pwdinfo->find_phase_state_exchange_cnt = P2P_FINDPHASE_EX_NONE;



    //kernel4.4 happen to : WARNING: "__umoddi3"
    //pwdinfo->listen_dwell = (wf_u8)((wf_os_api_timestamp() % 3) + 1);

    wf_memset(&pwdinfo->tx_prov_disc_info, 0x00,sizeof(struct tx_provdisc_req_info));
    pwdinfo->tx_prov_disc_info.wps_config_method_request = WPS_CM_NONE;

    wf_memset(&pwdinfo->nego_req_info, 0x00, sizeof(struct tx_nego_req_info));

    pwdinfo->device_password_id_for_nego = WPS_DPID_PBC;
    pwdinfo->negotiation_dialog_token = 1;

    wf_memset(pwdinfo->nego_ssid, 0x00, WLAN_SSID_MAXLEN);
    pwdinfo->nego_ssidlen = 0;

    pwdinfo->ui_got_wps_info = P2P_NO_WPSINFO;
#ifdef CONFIG_WFD
    pwdinfo->supported_wps_cm = WPS_CONFIG_METHOD_DISPLAY | WPS_CONFIG_METHOD_PBC;
    wf_memset(&pwdinfo->wfd_info,0,sizeof(struct wifi_display_info));
#else
    pwdinfo->supported_wps_cm = WPS_CONFIG_METHOD_DISPLAY | WPS_CONFIG_METHOD_PBC | WPS_CONFIG_METHOD_KEYPAD;
#endif
    pwdinfo->channel_list_attr_len = 0;
    wf_memset(pwdinfo->channel_list_attr, 0x00, 100);

    wf_memset(pwdinfo->rx_prov_disc_info.strconfig_method_desc_of_prov_disc_req, 0x00, 4);
    wf_memset(pwdinfo->rx_prov_disc_info.strconfig_method_desc_of_prov_disc_req, '0', 3);
    wf_memset(&pwdinfo->groupid_info, 0x00, sizeof(struct group_id_info));
#ifdef CONFIG_CONCURRENT_MODE
#ifdef CONFIG_IOCTL_CFG80211
    pwdinfo->ext_listen_interval = 1000;
    pwdinfo->ext_listen_period = 3000;
#else
    pwdinfo->ext_listen_interval = 1000;
    pwdinfo->ext_listen_period = 1000;
#endif
#endif


    wf_memset(pwdinfo->p2p_peer_interface_addr, 0x00, WF_ETH_ALEN);
    wf_memset(pwdinfo->p2p_peer_device_addr, 0x00, WF_ETH_ALEN);

    pwdinfo->rx_invitereq_info.operation_ch[0] = 0;
    pwdinfo->rx_invitereq_info.operation_ch[1] = 0;
#ifdef CONFIG_P2P_OP_CHK_SOCIAL_CH
    pwdinfo->rx_invitereq_info.operation_ch[2] = 0;
    pwdinfo->rx_invitereq_info.operation_ch[3] = 0;
    pwdinfo->rx_invitereq_info.operation_ch[4] = 0;
#endif
    pwdinfo->rx_invitereq_info.scan_op_ch_only = 0;
    pwdinfo->p2p_info.operation_ch[0] = 0;
    pwdinfo->p2p_info.operation_ch[1] = 0;
#ifdef CONFIG_P2P_OP_CHK_SOCIAL_CH
    pwdinfo->p2p_info.operation_ch[2] = 0;
    pwdinfo->p2p_info.operation_ch[3] = 0;
    pwdinfo->p2p_info.operation_ch[4] = 0;
#endif
    pwdinfo->p2p_info.scan_op_ch_only = 0;

}

wf_s32 p2p_info_init(nic_info_st *nic_info)
{

    p2p_info_st *p2p_info = NULL;

    LOG_I("p2p_info_init start");
    if(NULL != nic_info->p2p)
    {
        LOG_D("nic_info->p2p is not null");
        return 0;
    }
    
    p2p_info = wf_kzalloc(sizeof(p2p_info_st));
    if (p2p_info == NULL)
    {
        LOG_D("malloc p2p_info_st failed");
        return WF_RETURN_FAIL;
    }

    nic_info->p2p = p2p_info;
    p2p_info->nic_info = nic_info;
    wifi_direct_info_init(&p2p_info->wdinfo);
    p2p_timers_init(nic_info);
    
    
    return 0;

}



wf_s32 p2p_info_term(nic_info_st *nic_info)
{
    p2p_info_st *p2p_info = NULL;
    struct wifidirect_info *pwdinfo = NULL;

    LOG_I("p2p_info_term start");
    p2p_info = nic_info->p2p;
    if(NULL == p2p_info)
    {
        return 0;
    }
    
    pwdinfo = &p2p_info->wdinfo;
    /*todo*/

    wf_kfree(p2p_info);
    p2p_info = NULL;
    return 0;
}



inline void p2p_set_role(struct wifidirect_info *wdinfo, enum P2P_ROLE role)
{
    if (wdinfo->role != role) 
    {
        wdinfo->role = role;
    }
}
inline P2P_ROLE p2p_get_role(struct wifidirect_info *wdinfo)
{
    return wdinfo->role;
}

inline void p2p_set_state(struct wifidirect_info *wdinfo, enum P2P_STATE state)
{
    if (wdinfo->p2p_state != state) 
    {
        wdinfo->p2p_state = state;
    }
}


inline void p2p_set_pre_state(struct wifidirect_info *wdinfo, enum P2P_STATE state)
{
    if (wdinfo->pre_p2p_state != state) 
    {
        wdinfo->pre_p2p_state = state;
    }
}

wf_s32 wf_p2p_enable(nic_info_st *nic_info,P2P_ROLE role)
{
    
    wf_s32 ret = WF_RETURN_OK;
    nic_info_st *pother_nic                 = NULL;
    struct wifidirect_info *pwdinfo         = NULL;
    struct wifidirect_info *pbuddy_wdinfo   = NULL;
    wf_wlan_info_t *pother_wlan_info        = NULL;
    wf_wlan_network_t *pother_cur_network   = NULL;
    wf_u8 channel = 0;
    wf_u8 ch_offset = 0;
    wf_u16 bwmode = 0;
    wf_bool other_bconnect= wf_false;
    p2p_info_st *p2p_info = nic_info->p2p;
    p2p_info_st *other_p2p_info = NULL;

    LOG_I("[%s,%d] role:%s",__func__,__LINE__,p2p_role_to_str(role));
    pwdinfo = &p2p_info->wdinfo;
    if (role == P2P_ROLE_DEVICE || role == P2P_ROLE_CLIENT || role == P2P_ROLE_GO) 
    {

        p2p_info->p2p_enabled = wf_true;
        p2p_do_renew_tx_rate(nic_info, WIRELESS_11G_24N);
        
#ifdef CONFIG_CONCURRENT_MODE
        pother_nic          = nic_info->vir_nic;
        other_p2p_info      = pother_nic->p2p;
        pbuddy_wdinfo       = &other_p2p_info->wdinfo;
        pother_wlan_info    = pother_nic->wlan_info;
        pother_cur_network = &pother_wlan_info->cur_network;
        if (pbuddy_wdinfo->p2p_state != P2P_STATE_NONE)
        {
            return ret;
        }
        wf_mlme_get_connect(pother_nic,&other_bconnect);
        if (other_bconnect == wf_true && 
            (1 == pother_cur_network->channel || 6 == pother_cur_network->channel|| 11 == pother_cur_network->channel))
        {
            pwdinfo->listen_channel = pother_cur_network->channel;
        } 
        else
#endif
        {
            pwdinfo->listen_channel = 11;
        }

        if (role == P2P_ROLE_DEVICE) 
        {
            p2p_set_role(pwdinfo, P2P_ROLE_DEVICE);
#ifdef CONFIG_CONCURRENT_MODE
            if (other_bconnect == wf_true) 
            {
                p2p_set_state(pwdinfo, P2P_STATE_IDLE);
            } 
            else
#endif
            {
                p2p_set_state(pwdinfo, P2P_STATE_LISTEN);
            }
            pwdinfo->intent = 1;
            p2p_set_pre_state(pwdinfo, P2P_STATE_LISTEN);
        } 
        else if (role == P2P_ROLE_CLIENT) 
        {
            p2p_set_role(pwdinfo, P2P_ROLE_CLIENT);
            p2p_set_state(pwdinfo, P2P_STATE_GONEGO_OK);
            pwdinfo->intent = 1;
            p2p_set_pre_state(pwdinfo, P2P_STATE_GONEGO_OK);
        } 
        else if (role == P2P_ROLE_GO) 
        {
            p2p_set_role(pwdinfo, P2P_ROLE_GO);
            p2p_set_state(pwdinfo, P2P_STATE_GONEGO_OK);
            pwdinfo->intent = 15;
            p2p_set_pre_state(pwdinfo, P2P_STATE_GONEGO_OK);
        }

        wf_odm_sync_msg(nic_info,HAL_MSG_P2P_STATE,wf_true);
#ifdef CONFIG_WFD
        //if (Func_Chip_Hw_Chk_Wl_Func(pwadptdata, WL_FUNC_MIRACAST))
        if(0)
        {
            wf_odm_sync_msg(nic_info,HAL_MSG_WIFI_DISPLAY_STATE,wf_true);
        }
#endif
    }

    return 0;

}

wf_s32 wf_p2p_disable(nic_info_st *nic_info)
{
    p2p_info_st *p2p_info = nic_info->p2p;
    struct wifidirect_info *pwdinfo         = &p2p_info->wdinfo;
#ifdef CONFIG_IOCTL_CFG80211
    if (pwdinfo->driver_interface == DRIVER_CFG80211)
        p2p_info->p2p_enabled = wf_false;
#endif

    if(pwdinfo->p2p_state != P2P_STATE_NONE)
    {
        wf_os_api_timer_unreg(&p2p_info->find_phase_timer);
        wf_os_api_timer_unreg(&p2p_info->restore_p2p_state_timer);
        wf_os_api_timer_unreg(&p2p_info->pre_tx_scan_timer);
        wf_os_api_timer_unreg(&p2p_info->reset_ch_sitesurvey);
        wf_os_api_timer_unreg(&p2p_info->reset_ch_sitesurvey2);
        p2p_reset_invitereq_operation_ch(pwdinfo);
        p2p_reset_operation_ch(pwdinfo);
#ifdef CONFIG_CONCURRENT_MODE
        wf_os_api_timer_unreg(&p2p_info->ap_p2p_switch_timer);
#endif
        p2p_set_state(pwdinfo, P2P_STATE_NONE);
        p2p_set_pre_state(pwdinfo, P2P_STATE_NONE);
        p2p_set_role(pwdinfo, P2P_ROLE_DISABLE);
        wf_memset(&pwdinfo->rx_prov_disc_info, 0x00, sizeof(struct rx_provdisc_req_info));
        wf_memset(&pwdinfo->profileinfo[0], 0x00, sizeof(struct profile_info) * P2P_MAX_PERSISTENT_GROUP_NUM);
        pwdinfo->profileindex = 0;
    }

    wf_odm_sync_msg(nic_info,HAL_MSG_P2P_STATE,wf_false);
#ifdef CONFIG_WFD
    //if (Func_Chip_Hw_Chk_Wl_Func(pwadptdata, WL_FUNC_MIRACAST))
    if(0)
    { 
        wf_odm_sync_msg(nic_info,HAL_MSG_WIFI_DISPLAY_STATE,wf_false);
    }
#endif
#if 0
    if (_FAIL == wl_pwr_wakeup(pwadptdata)) {
    ret = _FAIL;
    goto exit;
    }
#endif
    p2p_do_renew_tx_rate(nic_info, WIRELESS_11BG_24N);

#ifdef CONFIG_IOCTL_CFG80211
    pwdinfo->driver_interface = DRIVER_CFG80211;
#else
    pwdinfo->driver_interface = DRIVER_WEXT;
#endif
    return 0;
}
#endif
