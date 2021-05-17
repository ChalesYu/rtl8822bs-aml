#include "common.h"
#include "wf_debug.h"

#define ODM_DEBUG 0

#define  PHYSTATUS_SEND_CNT (15)

#define RND4(x) (((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)

static wf_s32 odm_queue_insert(odm_mgnt_st *odm, odm_mcu_cmd_st *cmd_node)
{
    wf_enque_tail(&cmd_node->node, &odm->odm_queue);
    wf_os_api_sema_post(&odm->odm_sema);
    return WF_RETURN_OK;
}

static wf_s32 odm_queue_remove(odm_mgnt_st *odm, odm_mcu_cmd_st **cmd_node)
{
    wf_que_list_t *node           = NULL;
    odm_mcu_cmd_st *tmp_node    = NULL;

    if (wf_os_api_sema_wait(&odm->odm_sema))
    {
        return -2;
    }
    node = wf_deque_head(&odm->odm_queue);
    if(NULL == node)
    {
        return -1;
    }

    tmp_node = WF_CONTAINER_OF(node, odm_mcu_cmd_st, node);
    *cmd_node = tmp_node;
    return 0;
};

wf_s32 wf_sync_odm_phystatus(nic_info_st * nic_info, odm_h2mcu_phystatus_st *sync_msg)
{
    wf_s32 ret = 0;
    wf_s32 len = RND4(sizeof(odm_h2mcu_phystatus_st));
    prx_info_t prx_info = nic_info->rx_info;
    if (nic_info->is_driver_stopped || nic_info->is_surprise_removed)
    {
        return WF_RETURN_FAIL;
    }

#if ODM_DEBUG
    LOG_D("DataRate:%x",sync_msg->DataRate);
    LOG_D("StationID:%x",sync_msg->StationID);
    LOG_D("bPacketMatchBSSID:%x",sync_msg->bPacketMatchBSSID);
    LOG_D("bPacketToSelf:%x",sync_msg->bPacketToSelf);
    LOG_D("bPacketBeacon:%x",sync_msg->bPacketBeacon);
    LOG_D("bToSelf:%x",sync_msg->bToSelf);
    LOG_D("cck_agc_rpt_ofdm_cfosho_a:%x",sync_msg->cck_agc_rpt_ofdm_cfosho_a);
    LOG_D("cck_sig_qual_ofdm_pwdb_all:%x",sync_msg->cck_sig_qual_ofdm_pwdb_all);
    LOG_D("gain:%x",sync_msg->gain);
    LOG_D("path_rxsnr:%x",sync_msg->path_rxsnr );
    LOG_D("stream_rxevm:%x",sync_msg->stream_rxevm);
    LOG_D("path_cfotail:%x",sync_msg->path_cfotail);
    LOG_D("bcn_cnt:%x",sync_msg->bcn_cnt);
#endif

    ret = mcu_cmd_communicate_try(nic_info,UMSG_OPS_MSG_RHY_STATUS,(wf_u32 *)sync_msg,len/4,prx_info->m0_rxbuf,2);
#if ODM_DEBUG
    LOG_D("signal_strength:%d",prx_info->m0_rxbuf[0]);
    LOG_D("signal_quality:%d",prx_info->m0_rxbuf[1]);
#endif

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return ret;
    }
    else if(WF_RETURN_CMD_BUSY == ret)
    {
        LOG_W("[%s] cmd busy,try again if need!",__func__);
        return ret;
    }

    return WF_RETURN_OK;
}

static wf_s32 cmd_func_phystatus(nic_info_st *nic_info, odm_mcu_cmd_param_u *param)
{
    return wf_sync_odm_phystatus(nic_info,&param->odm_phystatus);
}


void wf_display_odm_msg(mcu_msg_body_st *msg)
{
    LOG_I("[%s] cur_wireless_mode       =0x%x,\n"
        "CurrentBandType         =0x%x,\n"
        "ForcedDataRate          =0x%x,\n"
        "Cur40MhzPrimeSC         =0x%x,\n"
        "dot11PrivacyAlgrthm     =0x%x,\n"
        "CurrentChannelBW        =0x%x,\n"
        "CurrentChannel          =0x%x,\n"
        "net_closed              =0x%x,\n"
        "u1ForcedIgiLb           =0x%x,\n"
        "bScanInProcess          =0x%x,\n"
        "bpower_saving           =0x%x,\n"
        "traffic_stat_cur_tx_tp  =0x%x,\n"
        "traffic_stat_cur_rx_tp  =0x%x,\n"
        "msgWdgStateVal          =0x%x,\n"
        "ability                 =0x%x,\n"
        "Rssi_Min                =%d,\n"
        "dig_CurIGValue          =0x%x,\n"
        "wifi_direct             =0x%x,\n"
        "wifi_display            =0x%x,\n"
        "dbg_level               =0x%x,\n"
        "PhyRegPgVersion         =0x%x,\n"
        "PhyRegPgValueType       =0x%x,\n"
        "bDisablePowerTraining   =0x%x,\n"
        "fw_state                =0x%x,\n"
        "sta_count               =0x%x,\n"
        "tx_bytes                =%llu,\n"
        "rx_bytes                =%llu,\n"
        "dbg_cmp                 =%llu.\n"
        ,__func__
        ,msg->cur_wireless_mode
        ,msg->CurrentBandType
        ,msg->ForcedDataRate
        ,msg->nCur40MhzPrimeSC
        ,msg->dot11PrivacyAlgrthm
        ,msg->CurrentChannelBW
        ,msg->CurrentChannel
        ,msg->net_closed
        ,msg->u1ForcedIgiLb
        ,msg->bScanInProcess
        ,msg->bpower_saving
        ,msg->traffic_stat_cur_tx_tp
        ,msg->traffic_stat_cur_rx_tp
        ,msg->msgWdgStateVal
        ,msg->ability
        ,msg->Rssi_Min
        ,msg->dig_CurIGValue
        ,msg->wifi_direct
        ,msg->wifi_display
        ,msg->dbg_level
        ,msg->PhyRegPgVersion
        ,msg->PhyRegPgValueType
        ,msg->bDisablePowerTraining
        ,msg->fw_state
        ,msg->sta_count
        ,msg->tx_bytes
        ,msg->rx_bytes
        ,msg->dbg_cmp);
}


static void odm_queue_handle(nic_info_st *nic_info)
{
    odm_mcu_cmd_st *cmd_node    = NULL;
    odm_mgnt_st *odm            = nic_info->odm;
    wf_s32 ret = 0;
    mlme_state_e status;
    wf_u8 exit_flag = 0;
    //wf_os_api_thread_affinity(DEFAULT_CPU_ID);

    while(1)
    {
        ret = odm_queue_remove(odm,&cmd_node);
        if(ret)
        {
            LOG_D("exit_flag:%d",exit_flag);
            if((nic_info->is_driver_stopped == wf_true) || (nic_info->is_surprise_removed == wf_true))
            {
                break;
            }
            //LOG_I("[%s,%d] exe",__func__,__LINE__);
            continue;
        }
        if((nic_info->is_driver_stopped == wf_true) || (nic_info->is_surprise_removed == wf_true))
        {
            wf_kfree(cmd_node);
            cmd_node = NULL;
            exit_flag = 1;
            continue;
        }
        wf_mlme_get_state(nic_info, &status);
        if (status == MLME_STATE_IDLE)
        {
            //LOG_I("odm watch dog running...");
            if(cmd_node->cmd_func)
            {
                cmd_node->cmd_func(nic_info,&cmd_node->param);
            }
        }


        wf_kfree(cmd_node);
        cmd_node = NULL;
    }

    while(wf_os_api_thread_wait_stop(odm->odm_tid) == wf_false)
    {
        wf_msleep(10);
    }

    wf_os_api_thread_exit(odm->odm_tid);
}

static void wf_wdn_update_traffic_stat(nic_info_st *nic_info,wf_u8 update_odm_flag)
{
    tx_info_st *tx_info         = NULL;
    wdn_net_info_st *wdn_info   = NULL;
    rx_info_t * rx_info         = NULL;
    odm_mgnt_st *odm            = NULL;
    wdn_info = wf_wdn_find_info(nic_info,wf_wlan_get_cur_bssid(nic_info));
    if (wdn_info == NULL)
    {
        return;
    }

    /*tx statistics*/
    tx_info = (tx_info_st *)nic_info->tx_info;
    wdn_info->wdn_stats.tx_bytes        = tx_info->tx_bytes;
    wdn_info->wdn_stats.tx_pkts         = tx_info->tx_pkts;
    wdn_info->wdn_stats.tx_drops        = tx_info->tx_drop;
    wdn_info->wdn_stats.cur_tx_bytes    = wdn_info->wdn_stats.tx_bytes - wdn_info->wdn_stats.last_tx_bytes;
    wdn_info->wdn_stats.last_tx_bytes   = wdn_info->wdn_stats.tx_bytes;
    wdn_info->wdn_stats.cur_tx_tp       = (wf_u32)(wdn_info->wdn_stats.cur_tx_bytes * 8 / 2 / 1024 / 1024);

    /*rx statistics*/
    rx_info = (rx_info_t *)nic_info->rx_info;
    wdn_info->wdn_stats.rx_bytes        = rx_info->rx_bytes;
    wdn_info->wdn_stats.rx_pkts         = rx_info->rx_pkts;
    wdn_info->wdn_stats.rx_drops        = rx_info->rx_drop;
    wdn_info->wdn_stats.cur_rx_bytes    = wdn_info->wdn_stats.rx_bytes - wdn_info->wdn_stats.last_rx_bytes;
    wdn_info->wdn_stats.last_rx_bytes   = wdn_info->wdn_stats.rx_bytes;
    wdn_info->wdn_stats.cur_rx_tp       = (wf_u32)(wdn_info->wdn_stats.cur_rx_bytes * 8 / 2 / 1024 / 1024);

#if 0
    WDN_INFO("tx_bytes:%lld,rx_bytes:%lld,cur_tx_bytes:%lld,cur_rx_bytes:%lld",
             wdn_info->wdn_stats.tx_bytes, wdn_info->wdn_stats.rx_pkts,
             wdn_info->wdn_stats.cur_tx_bytes, wdn_info->wdn_stats.cur_rx_bytes);
#endif
    /*update odm message*/
    if(update_odm_flag)
    {
        odm     = (odm_mgnt_st *)nic_info->odm;
        odm->odm_msg.traffic_stat_cur_tx_tp = wdn_info->wdn_stats.cur_tx_tp;
        odm->odm_msg.traffic_stat_cur_rx_tp = wdn_info->wdn_stats.cur_rx_tp;
    }
}

static wf_s32 wf_odm_watchdog_cmds_handle(nic_info_st *nic_info, odm_mcu_cmd_param_u *param)
{
    int ret = 0;
    odm_mgnt_st *odm                = (odm_mgnt_st *)nic_info->odm;
    mcu_msg_body_st *odm_msg             = &odm->odm_msg;
    wf_bool blinked                 = wf_false;
    wf_bool bsta_state              = wf_false ;
    wf_bool bbt_disabled            = wf_true;
    //LOG_I("[%s,%d]",__func__,__LINE__);
    if (nic_info->is_driver_stopped || nic_info->is_surprise_removed)
    {
        return WF_RETURN_FAIL;
    }

    odm->wdg_exec_cnt++;

    wf_wdn_update_traffic_stat(nic_info,1);

    wf_mlme_get_connect(nic_info,&blinked);

    if(NIC_USB == nic_info->nic_type)
    {
        bsta_state = (nic_info->nic_state & WIFI_STATION_STATE)? wf_true:wf_false;
        ret = wf_mcu_msg_body_get(nic_info,odm_msg);
        odm_msg->msgWdgStateVal = blinked + (bsta_state<<1) + (bbt_disabled<<2);
        ret = wf_mcu_msg_body_set(nic_info,odm_msg);
    }
    else //sdio
    {
        wf_mcu_check_rx_fifo(nic_info);
    }
    ret = wf_mcu_watchdog(nic_info);

    ret = wf_odm_update_wdn_info(nic_info);


    return ret;
}

static void odm_wdg_handle(wf_os_api_timer_t * timer)
{
    odm_mgnt_st * ba_order = WF_CONTAINER_OF((wf_os_api_timer_t *)timer, odm_mgnt_st, odm_wdg_timer);
    odm_mcu_cmd_st *cmd_node                    = NULL;
    nic_info_st *ni                             = ba_order->nic_info;
    odm_mgnt_st *odm                            = (odm_mgnt_st *)ni->odm;
    hw_info_st  *hw_info                        = ni->hw_info;

    if ((ni->is_driver_stopped == wf_true) || (ni->is_surprise_removed == wf_true))
    {
        return;
    }
#ifdef CONFIG_MP_MODE
    wf_os_api_timer_set(&odm->odm_wdg_timer, 2000);
    return;
#endif

    odm->last_bcn_cnt = odm->bcn_cnt;
    odm->bcn_cnt      = 0;
    if(wf_true == hw_info->use_drv_odm)
    {
        cmd_node = wf_kzalloc(sizeof(odm_mcu_cmd_st));
        if(NULL == cmd_node)
        {
            LOG_E("[%s] wf_kzalloc cmd_node is null",__func__);
            return;
        }

        cmd_node->cmd_func = wf_odm_watchdog_cmds_handle;

        odm_queue_insert(odm,cmd_node);
    }


    wf_os_api_timer_set(&odm->odm_wdg_timer, 2000);
}
wf_s32 wf_odm_mgnt_init(nic_info_st *nic_info)
{
    odm_mgnt_st *odm = NULL;

    if(NULL != nic_info->odm)
    {
        LOG_D("nic_info->odm is not null");
        return 0;
    }
    odm = wf_os_api_get_odm(nic_info);
    if(NULL != odm)
    {
        LOG_D("wf_os_api_get_odm get odm");
        nic_info->odm        = odm;
    }
    else
    {
        LOG_D(" wf_kzalloc for odm");
        odm = wf_kzalloc(sizeof(odm_mgnt_st));
        if ( NULL == odm )
        {
           LOG_E("[%s] malloc odm failed", __func__);
           return WF_RETURN_FAIL;
        }
        odm->nic_info = nic_info;
        nic_info->odm = odm;
        odm->phystatus_enable = wf_true;
        odm->send_flag_val = 0;
        wf_os_api_timer_reg(&odm->odm_wdg_timer, (void *)odm_wdg_handle, &odm->odm_wdg_timer);
        wf_que_init(&odm->odm_queue,WF_LOCK_TYPE_SPIN);
        wf_os_api_sema_init(&odm->odm_sema, 0);

        sprintf(odm->odm_name,
                nic_info->virNic ? "odm:vir%d_%d" : "odm:wlan%d_%d",
                nic_info->hif_node_id, nic_info->ndev_id);
        if (NULL ==(odm->odm_tid=wf_os_api_thread_create(odm->odm_tid, odm->odm_name, odm_queue_handle, nic_info)))
        {
            LOG_E("[wf_mlme_init] create mlme thread failed");
            return -1;
        }
        else
        {
            wf_os_api_thread_wakeup(odm->odm_tid);
        }
        
        wf_os_api_set_odm(nic_info, odm);
    }

    return WF_RETURN_OK;
}

wf_s32 wf_odm_mgnt_term(nic_info_st *nic_info)
{
    odm_mgnt_st *odm = NULL;

     LOG_I("[%s,%d] start",__func__,__LINE__);
    if(NULL == nic_info->odm)
    {
        LOG_I("[%s,%d]",__func__,__LINE__);
        return 0;
    }

    odm = wf_os_api_get_odm(nic_info);
    if(NULL == odm)
    {
        LOG_I("[%s,%d]",__func__,__LINE__);
        return 0;
    }
    LOG_I("[%s,%d]",__func__,__LINE__);
    wf_os_api_timer_unreg(&odm->odm_wdg_timer);
    if (odm && odm->odm_tid)
    {
        wf_os_api_sema_post(&odm->odm_sema);
        wf_os_api_thread_destory(odm->odm_tid);
        odm->odm_tid = NULL;
         wf_os_api_set_odm(nic_info,NULL);
         nic_info->odm=NULL;
         wf_kfree(odm);
         odm  = NULL;
    }

    LOG_D("[wf_odm_mgnt_term] end");

    return WF_RETURN_OK;
}
















void wf_odm_handle_phystatus(nic_info_st *nic_info, recv_phy_status_st *rps, wf_u8 *pbuf, prx_pkt_t ppt)
{
    odm_mgnt_st *odm                            = (odm_mgnt_st *)nic_info->odm;
    hw_info_st *hw_info                        = nic_info->hw_info;
    wdn_net_info_st *wdn_net_info               = NULL;
    odm_phy_info_st  *odm_phy                   = NULL;
    odm_mcu_cmd_st *cmd_node                    = NULL;
    odm_h2mcu_phystatus_st *sync_phystatus_msg  = NULL ;
    wf_u8 bPacketMatchBSSID                     = 0;
    wf_u8 bPacketBeacon                         = 0;

    wdn_net_info  = wf_wdn_find_info(nic_info,get_hdr_bssid(pbuf));
    if (wdn_net_info == NULL)
    {
        return;
    }

    odm->send_flag_val++;
    if(PHYSTATUS_SEND_CNT <= odm->send_flag_val)
    {
        cmd_node = wf_kzalloc(sizeof(odm_mcu_cmd_st));
        if(NULL == cmd_node)
        {
            LOG_E("[%s] wf_kzalloc cmd_node is null",__func__);
            return;
        }
    }
    else
    {
        return;
    }

    odm_phy = &odm->phy[wdn_net_info->wdn_id];

    wf_memcpy(&odm_phy->raw_phystatus,rps,28);
    wf_memcpy(&odm_phy->phy_status,&ppt->phy_status,sizeof(ppt->phy_status));
    odm_phy->rx_rate = ppt->pkt_info.rx_rate;
    odm_phy->is_cck_rate = (odm_phy->rx_rate <= DESC_RATE11M) ? wf_true : wf_false;
    bPacketMatchBSSID  = (!IsFrameTypeCtrl(pbuf)) && (!ppt->pkt_info.icv_err) && (!ppt->pkt_info.crc_err) && (0 == wf_memcmp(get_hdr_bssid(pbuf), wdn_net_info->mac,WF_ETH_ALEN));

    if(bPacketMatchBSSID && (GetFrameSubType(pbuf) == WIFI_BEACON))
    {
        bPacketBeacon = wf_true;
        odm->bcn_cnt++;
    }
    else
    {
        bPacketBeacon = wf_false;
    }


    #if ODM_DEBUG
    LOG_I("rx_rate:%d, signal_strength:%d,signal_qual:%d",ppt->pkt_info.rx_rate,ppt->phy_status.signal_strength,ppt->phy_status.signal_qual);
    #endif

    sync_phystatus_msg  = &cmd_node->param.odm_phystatus ;
    wf_memset(sync_phystatus_msg,0,sizeof(odm_h2mcu_phystatus_st));

    /* DataRate */
    sync_phystatus_msg->DataRate  = ppt->pkt_info.rx_rate;

    /* StationID */
    sync_phystatus_msg->StationID = wdn_net_info->wdn_id;

    /* bPacketMatchBSSID */
    sync_phystatus_msg->bPacketMatchBSSID   = bPacketMatchBSSID;

    /* bPacketToSelf */
    if(bPacketMatchBSSID && (0 == wf_memcmp(hw_info->macAddr,get_ra(pbuf),WF_ETH_ALEN)))
    {
        sync_phystatus_msg->bPacketToSelf = wf_true;
    }
    else
    {
        sync_phystatus_msg->bPacketToSelf = wf_false;
    }

    /* bPacketBeacon */
    sync_phystatus_msg->bPacketBeacon       = bPacketBeacon;

    /* bToSelf */
    if(0 == wf_memcmp(hw_info->macAddr,get_ra(pbuf),WF_ETH_ALEN))
    {
        sync_phystatus_msg->bToSelf  = wf_true;
    }
    else
    {
        sync_phystatus_msg->bToSelf  = wf_false;
    }

    /* cck_agc_rpt_ofdm_cfosho_a */
    sync_phystatus_msg->cck_agc_rpt_ofdm_cfosho_a   = rps->cck_agc_rpt_ofdm_cfosho_a;

    /* cck_sig_qual_ofdm_pwdb_all */
    sync_phystatus_msg->cck_sig_qual_ofdm_pwdb_all  = rps->cck_sig_qual_ofdm_pwdb_all;

    /* gain */
    sync_phystatus_msg->gain                        = rps->path_agc[0].gain;

    /* path_rxsnr */
    sync_phystatus_msg->path_rxsnr                  = rps->path_rxsnr[0];

    /* stream_rxevm */
    sync_phystatus_msg->stream_rxevm                = rps->stream_rxevm[0];

    /* path_cfotail */
    sync_phystatus_msg->path_cfotail                = rps->path_cfotail[0];

    /* bcn_cnt */
    sync_phystatus_msg->bcn_cnt = odm->bcn_cnt;

    //wf_memcpy(sync_phystatus_msg->Scaninfo_bssid,get_hdr_bssid(pbuf),WF_ETH_ALEN);

    if (odm->phystatus_enable == wf_true)
    {
        cmd_node->cmd_func = cmd_func_phystatus;
    }

    odm_queue_insert(odm,cmd_node);
    odm->send_flag_val = 0;

}






wf_s32 wf_odm_report_rssi(nic_info_st *nic_info)
{
    wf_s32 ret = WF_RETURN_OK;

    return ret;
}

int wf_odm_update_wdn_info(nic_info_st *nic_info)
{
    odm_mgnt_st *odm = (odm_mgnt_st *)nic_info->odm;
    mcu_msg_sta_info_st *odm_wdn = NULL;
    wdn_net_info_st * wdn_net_info = NULL;
    wf_u32 rate_bitmap = 0;
    int ret  = 0;
    wf_u8 i = 0;

    #if 1
    for(i=0;i<ODM_WDN_INFO_SIZE;i++)
    {
        odm_wdn = &odm->wdn[i];
        if(wf_true == odm_wdn->bUsed)
        {
            odm_wdn->ra_change = wf_false;
            ret = wf_mcu_msg_sta_info_get(nic_info,i,odm_wdn);
            if(ret)
            {
                LOG_W("wf_mcu_msg_sta_info_get failed");
                return ret;
            }
            wdn_net_info = wf_wdn_find_info_by_id(nic_info,i);
            if(wdn_net_info && wf_true == odm_wdn->ra_change)
            {
                mcu_msg_sta_info_pars(wdn_net_info,odm_wdn);
                ret = wf_mcu_get_rate_bitmap(nic_info, wdn_net_info,odm_wdn, &rate_bitmap);
                ret = wf_mcu_rfconfig_set(nic_info,wdn_net_info->wdn_id, wdn_net_info->raid, wdn_net_info->bw_mode, odm->wdn[i].htpriv.sgi_20m, rate_bitmap);
            }
        }
    }
    #else
    for(i=0;i<ODM_WDN_INFO_SIZE;i++)
    {
        odm_wdn = &odm->wdn[i];
        if(wf_true == odm_wdn->bUsed  && wf_true == odm_wdn->ra_change)
        {
            odm_wdn->ra_change = wf_false;
            ret = wf_mcu_msg_sta_info_get(nic_info,i,odm_wdn);
            wdn_net_info = wf_wdn_find_info_by_id(nic_info,i);
            if(wdn_net_info)
            {
                 mcu_msg_sta_info_pars(wdn_net_info,odm_wdn);
                ret = wf_mcu_get_rate_bitmap(nic_info, wdn_net_info, odm_wdn,&rate_bitmap);
                ret = wf_mcu_rfconfig_set(nic_info,wdn_net_info->wdn_id, wdn_net_info->raid, wdn_net_info->bw_mode, odm->wdn[i].htpriv.sgi_20m, rate_bitmap);
            }
        }
    }
    #endif

    return WF_RETURN_OK;
}



