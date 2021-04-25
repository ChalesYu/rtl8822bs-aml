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
    wf_que_list *node           = NULL;
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
    wf_u32 rx_buf[2];
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

    if(NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info,UMSG_OPS_MSG_RHY_STATUS,(wf_u32 *)sync_msg,len/4,prx_info->m0_rxbuf,2);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info,WLAN_OPS_DXX0_MSG_RHY_STATUS,(wf_u32 *)sync_msg,len/4,prx_info->m0_rxbuf,2);
    }


#if ODM_DEBUG
    LOG_D("signal_strength:%d",prx_info->m0_rxbuf[0]);
    LOG_D("signal_quality:%d",prx_info->m0_rxbuf[1]);
#endif

    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("wf_sync_odm_phystatus failed");
        return ret;
    }

    return WF_RETURN_OK;
}

static wf_s32 cmd_func_phystatus(nic_info_st *nic_info, odm_mcu_cmd_param_u *param)
{
    return wf_sync_odm_phystatus(nic_info,&param->odm_phystatus);
}


void wf_display_odm_msg(odm_msg_st *msg)
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
    wf_workqueue_mgnt_st *wq_mgt = NULL;
    odm_mgnt_st *odm            = nic_info->odm;
    wf_s32 ret = 0;
    mlme_state_e status;

	//wf_os_api_thread_affinity(DEFAULT_CPU_ID);

    while((nic_info->is_driver_stopped == wf_false) && (nic_info->is_surprise_removed == wf_false))
    {
        ret = odm_queue_remove(odm,&cmd_node);
        if(ret)
        {
            //LOG_I("[%s,%d] exe",__func__,__LINE__);
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
        else
        {
            wf_yield();
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

static wf_s32 wf_odm_watchdog_cmds_handle(nic_info_st *nic_info, odm_mcu_cmd_param_u *param)
{
    int ret = 0;
    odm_mgnt_st *odm                = (odm_mgnt_st *)nic_info->odm;
    odm_msg_st *odm_msg             = &odm->odm_msg;
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
        ret = wf_get_odm_msg(nic_info);
        odm_msg->msgWdgStateVal = blinked + (bsta_state<<1) + (bbt_disabled<<2);
        ret = wf_set_odm_msg(nic_info);
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
	hw_info_st *phw_info = (hw_info_st *)nic_info->hw_info;
	wf_s32 ret=0;
	
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
        
#ifdef CONFIG_ARS_SUPPORT
    //ars to do
#else
        wf_os_api_set_odm(nic_info, odm);
#endif
    }

    return WF_RETURN_OK;
}

wf_s32 wf_odm_mgnt_term(nic_info_st *nic_info)
{
    odm_mgnt_st *odm = NULL;

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
#ifdef CONFIG_ARS_SUPPORT
            //ars to do
#else
         wf_os_api_set_odm(nic_info,NULL);
#endif
         nic_info->odm=NULL;
         wf_kfree(odm);
         odm  = NULL;
    }

    LOG_D("[wf_odm_mgnt_term] end");

    return WF_RETURN_OK;
}

wf_s32 wf_get_odm_msg(nic_info_st *nic_info)
{
    int ret = 0;
    odm_mgnt_st *odm = nic_info->odm;
    if(NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_READVAR_MSG, NULL, 0,(wf_u32 *) &odm->odm_msg, sizeof(odm_msg_st) / 4);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_READVAR_MSG, NULL,    0,(wf_u32 *) &odm->odm_msg, sizeof(odm_msg_st) / 4);
    }

    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] UMSG_OPS_HAL_READVAR_MSG failed",__func__);
        return ret;
    }

    //wf_display_odm_msg(&odm->odm_msg);

    return WF_RETURN_OK;
}

wf_s32 wf_set_odm_msg(nic_info_st *nic_info)
{
    wf_s32 ret = 0;
    odm_mgnt_st *odm = nic_info->odm;
    //wf_display_odm_msg(odm->odm_msg);
    if(NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_WRITEVAR_MSG, (wf_u32 *) &odm->odm_msg,sizeof(odm_msg_st) / 4,NULL,0 );
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_WRITEVAR_MSG, (wf_u32 *) &odm->odm_msg,sizeof(odm_msg_st) / 4,NULL,0 );
    }

    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] UMSG_OPS_HAL_READVAR_MSG failed",__func__);
        return ret;
    }

    return WF_RETURN_OK;
}


wf_s32 wf_set_odm_dig(nic_info_st *nic_info, wf_bool init_gain, wf_u32 rx_gain)
{
    wf_s32 ret;
    wf_u32 buf[3];

    if (init_gain == wf_true)
    {
        buf[1] = 0;
        buf[2] = rx_gain;

        if (rx_gain == 0xff)
        {
            buf[0] = WF_BIT(1);
        }
        else
        {
            buf[0] = WF_BIT(0);
        }
        //LOG_I("[%s] %d,%d,%d",__func__,buf[0],buf[1],buf[2]);
        if(NIC_USB == nic_info->nic_type)
        {
            ret = mcu_cmd_communicate(nic_info, UMSG_OPS_MSG_PAUSEIG, buf, 3, NULL, 0);
        }
        else
        {
            ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_MSG_PAUSEIG, buf, 3, NULL, 0);
        }
        if(WF_RETURN_FAIL == ret)
        {
            LOG_E("[%s] UMSG_OPS_MSG_PAUSEIG failed",__func__);
            return ret;
        }
    }
    else
    {
        //msg_var_req_t msg_var_req;

        //msg_var_req.var = eVariable;
        //msg_var_req.msg = (SIZE_T) pValue1;
        //msg_var_req.set = (SIZE_T) bSet;
        //Func_Set_Msg_Var_Req(Adapter, &msg_var_req);
    }

    return WF_RETURN_OK;
}


wf_s32 wf_set_odm_default(nic_info_st * nic_info)
{
    wf_s32 ret = 0;

    if(NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_MSG_INIT_DEFAULT_VALUE,NULL,0,NULL,0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info,WLAN_OPS_DXX0_HAL_MSG_INIT_DEFAULT_VALUE,NULL,0,NULL,0);
    }

    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("set odm default value failed");
        return ret;
    }

    return WF_RETURN_OK;
}


wf_s32 wf_set_odm_init(nic_info_st * nic_info)
{
    wf_s32 ret = 0;

    if(NIC_USB == nic_info->nic_type)
    {
        ret =   mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_INIT_MSG, NULL, 0, NULL, 0);
    }
    else
    {
        ret =   mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_INIT_MSG, NULL, 0, NULL,    0);
    }
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] 0x1f failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}


wf_s32 signal_scale_mapping(wf_s32 current_sig)
{
    wf_s32 result_sig = 0;

    if (current_sig >= 51 && current_sig <= 100)
    {
        result_sig = 100;
    }
    else if (current_sig >= 41 && current_sig <= 50)
    {
        result_sig = 80 + ((current_sig - 40) * 2);
    }
    else if (current_sig >= 31 && current_sig <= 40)
    {
        result_sig = 66 + (current_sig - 30);
    }
    else if (current_sig >= 21 && current_sig <= 30)
    {
        result_sig = 54 + (current_sig - 20);
    }
    else if (current_sig >= 10 && current_sig <= 20)
    {
        result_sig = 42 + (((current_sig - 10) * 2) / 3);
    }
    else if (current_sig >= 5 && current_sig <= 9)
    {
        result_sig = 22 + (((current_sig - 5) * 3) / 2);
    }
    else if (current_sig >= 1 && current_sig <= 4)
    {
        result_sig = 6 + (((current_sig - 1) * 3) / 2);
    }
    else
    {
        result_sig = current_sig;
    }

    return result_sig;

}

/*天线在CCK速率下pwr*/
static wf_s8 cal_ant_cck_rssi_pwr(wf_u8 lna_idx, wf_u8 vga_idx)
{
    wf_s8 rx_pwr_all = 0x00;

    switch (lna_idx)
    {
        case 7:
            if (vga_idx <= 27)
                rx_pwr_all = -100 + 2 * (27 - vga_idx);
            else
                rx_pwr_all = -100;
            break;
        case 5:
            rx_pwr_all = -74 + 2 * (21 - vga_idx);
            break;
        case 3:
            rx_pwr_all = -60 + 2 * (20 - vga_idx);
            break;
        case 1:
            rx_pwr_all = -44 + 2 * (19 - vga_idx);
            break;
        default:
            //LOG_W("[%s] lna_idx:%d, vga_index:%d",__func__,lna_idx,vga_idx);
            break;
    }

    return rx_pwr_all;
}

wf_u8 query_rxpwr_percentage(wf_s8 AntPower)
{
    wf_u8 percent = 0;

    if ((AntPower <= -100) || (AntPower >= 20))
    {
        percent =  0;
    }
    else if (AntPower >= 0)
    {
        percent =  100;
    }
    else
    {
        percent =  100 + AntPower;
    }

    return percent;
}


static wf_u8 cal_evm2percentage(char Value)
{
    char ret_val;

    ret_val = Value;
    ret_val /= 2;

#ifdef MSG_EVM_ENHANCE_ANTDIV
    if (ret_val >= 0)
        ret_val = 0;

    if (ret_val <= -40)
        ret_val = -40;

    ret_val = 0 - ret_val;
    ret_val *= 3;
#else
    if (ret_val >= 0)
        ret_val = 0;

    if (ret_val <= -33)
        ret_val = -33;

    ret_val = 0 - ret_val;
    ret_val *= 3;

    if (ret_val == 99)
        ret_val = 100;
#endif

    return ret_val;
}

wf_s32 translate_percentage_to_dbm(wf_u32 SignalStrengthIndex)
{
    wf_s32 SignalPower;

#ifdef CONFIG_SIGNAL_SCALE_MAPPING
    SignalPower = (char) ((SignalStrengthIndex + 1) >> 1);
    SignalPower -= 95;
#else
    SignalPower = SignalStrengthIndex - 100;
#endif

    return SignalPower; /* in dBM.raw data */
}




void wf_odm_handle_phystatus(nic_info_st *nic_info, recv_phy_status_st *rps, wf_u8 *pbuf, prx_pkt_t ppt)
{
    odm_mgnt_st *odm                            = (odm_mgnt_st *)nic_info->odm;
    hw_info_st *hw_info                        = nic_info->hw_info;
    wdn_net_info_st *wdn_net_info               = NULL;
    odm_phy_info_st  *odm_phy                   = NULL;
    odm_mcu_cmd_st *cmd_node                    = NULL;
    odm_h2mcu_phystatus_st *sync_phystatus_msg  = NULL ;
    wf_u8 from_to_ds = 0;
    wf_u8 is_data_frame = 0;
    wf_u8 bPacketMatchBSSID = 0;
    wf_u8 bPacketBeacon = 0;

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


wf_s32 wf_odm_calc_str_and_qual(nic_info_st *nic_info, wf_u8 *rx_phystatus, wf_u8 *mac_frame, void *prx_pkt)
{
    prx_pkt_t ppt = (prx_pkt_t)prx_pkt;
    wf_bool is_cck_rate = wf_false;
    wf_u8 rate_cacl = 0;
    wf_u8 lna_index = 0;
    wf_u8 vga_index = 0;
    wf_s8 rx_pwr     = 0;
    wf_u8 pwdb_all      = 0;
    recv_phy_status_st *rps = (recv_phy_status_st *)rx_phystatus;

    rate_cacl = ppt->pkt_info.rx_rate;

    is_cck_rate = (rate_cacl <= DESC_RATE11M) ? wf_true : wf_false;
    if(is_cck_rate)
    {
        lna_index   = ((rps->cck_agc_rpt_ofdm_cfosho_a & 0xE0)>>5);
        vga_index   = rps->cck_agc_rpt_ofdm_cfosho_a & 0x1F;
        rx_pwr      = cal_ant_cck_rssi_pwr(lna_index, vga_index);
        pwdb_all    = query_rxpwr_percentage(rx_pwr);

        if(pwdb_all >40)
        {
            ppt->phy_status.signal_qual = 100;
        }
        else
        {
            if( rps->cck_sig_qual_ofdm_pwdb_all > 64)
            {
                ppt->phy_status.signal_qual = 0;
            }
            else if( rps->cck_sig_qual_ofdm_pwdb_all < 20)
            {
                ppt->phy_status.signal_qual = 100;
            }
            else
            {
                ppt->phy_status.signal_qual = (64-rps->cck_sig_qual_ofdm_pwdb_all)*100/44;
            }
        }

#ifdef CONFIG_SIGNAL_SCALE_MAPPING
        ppt->phy_status.signal_strength = signal_scale_mapping(pwdb_all);
#else
        ppt->phy_status.signal_strength = pwdb_all;
#endif
    }
    else
    {
        wf_u8 evm          = 0;
        wf_u8 rssi         = 0;
        char tmp_rx_pwr   = (rps->path_agc[0].gain & 0x3F)*2  - 110;
        rssi            = query_rxpwr_percentage(tmp_rx_pwr);
        rx_pwr          = ((rps->cck_sig_qual_ofdm_pwdb_all >> 1) & 0x7F) - 110;
        pwdb_all        = query_rxpwr_percentage(rx_pwr);
        evm             = cal_evm2percentage(rps->stream_rxevm[0]);

        ppt->phy_status.signal_qual = evm & 0xFF;

#ifdef CONFIG_SIGNAL_SCALE_MAPPING
        ppt->phy_status.signal_strength = signal_scale_mapping(rssi);
#else
        ppt->phy_status.signal_strength = rssi;
#endif

    }

    #if 0
    ppt->phy_status.signal_strength = signal_scale_mapping(ppt->phy_status.signal_strength);
    ppt->phy_status.rssi            = translate_percentage_to_dbm(ppt->phy_status.signal_strength);
    #endif

    return WF_RETURN_OK;
}

wf_s32 wf_odm_sync_msg(nic_info_st *nic_info,ODM_MSG_VARIABLE ops,  wf_u32 val)
{
    odm_mgnt_st *odm        = (odm_mgnt_st *)nic_info->odm;
    odm_msg_st *odm_msg     = &odm->odm_msg;

    wf_get_odm_msg(nic_info);
    switch (ops)
    {
        case HAL_MSG_STA_INFO:
        {
            break;
        }
        case HAL_MSG_P2P_STATE:
        {
            odm_msg->wifi_direct = val;
            break;
        }
        case HAL_MSG_WIFI_DISPLAY_STATE:
        {
            odm_msg->wifi_display = val;
            break;
        }
        default:
        {
            break;
        }
    }

    wf_set_odm_msg(nic_info);

    return WF_RETURN_OK;
}


wf_s32 wf_odm_set_ability(nic_info_st *nic_info,ODM_ABILITY_OPS ops,  wf_u32 ability)
{
    odm_mgnt_st *odm        = (odm_mgnt_st *)nic_info->odm;
    odm_msg_st *odm_msg     = &odm->odm_msg;

    wf_get_odm_msg(nic_info);

    //lock

    switch (ops)
    {
        case ODM_DIS_ALL_FUNC:
            odm_msg->ability = ability;
            break;
        case ODM_FUNC_SET:
            odm_msg->ability |= ability;
            break;
        case ODM_FUNC_CLR:
            odm_msg->ability &= ~(ability);
            break;
        case ODM_FUNC_BACKUP:
            //LOG_I("ability backup %x\r\n", odm->backup_ability);
            odm->backup_ability = odm_msg->ability;
            break;
        case ODM_FUNC_RESTORE:
            //LOG_I("ability restory %X\r\n", odm->backup_ability);
            odm_msg->ability = odm->backup_ability;
            break;
    }

    //unlock

    wf_set_odm_msg(nic_info);

    return WF_RETURN_OK;
}


wf_s32 wf_odm_set_rfconfig(nic_info_st *nic_info, wf_u8 mac_id, wf_u8 raid, wf_u8 bw, wf_u8 sgi, wf_u32 mask)
{
    wf_u8 u1wMBOX1MacIdConfigParm[wMBOX1_MACID_CFG_LEN] = { 0 };
    odm_mgnt_st *odm = (odm_mgnt_st *)nic_info->odm;
    int ret = 0;

    LOG_I("[%s] mac_id:%d, raid:%d, bw:%d, sgi:%d,mask:0x%x",__func__, mac_id, raid, bw, sgi,mask);
    SET_9086X_wMBOX1CMD_MACID_CFG_MACID(u1wMBOX1MacIdConfigParm, mac_id);
    SET_9086X_wMBOX1CMD_MACID_CFG_RAID(u1wMBOX1MacIdConfigParm, raid);
    SET_9086X_wMBOX1CMD_MACID_CFG_SGI_EN(u1wMBOX1MacIdConfigParm, (sgi) ? 1 : 0);
    SET_9086X_wMBOX1CMD_MACID_CFG_BW(u1wMBOX1MacIdConfigParm, bw);

    if (odm->odm_msg.bDisablePowerTraining)
    {
        SET_9086X_wMBOX1CMD_MACID_CFG_DISPT(u1wMBOX1MacIdConfigParm, 1);
    }
    else
    {
        ret = wf_get_odm_msg(nic_info);

        if (odm->odm_msg.bDisablePowerTraining)
        {
            SET_9086X_wMBOX1CMD_MACID_CFG_DISPT(u1wMBOX1MacIdConfigParm, 1);

        }

    }

    SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK0(u1wMBOX1MacIdConfigParm,
                                          (wf_u8) (mask & 0x000000ff));
    SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK1(u1wMBOX1MacIdConfigParm,
                                          (wf_u8) ((mask & 0x0000ff00) >> 8));
    SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK2(u1wMBOX1MacIdConfigParm,
                                          (wf_u8) ((mask & 0x00ff0000) >> 16));
    SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK3(u1wMBOX1MacIdConfigParm,
                                          (wf_u8) ((mask & 0xff000000) >> 24));
    #if CHIP51_DIRECT_ACCESS
    ret = FillH2CCmd(nic_info,wMBOX1_9086X_MACID_CFG,wMBOX1_MACID_CFG_LEN,u1wMBOX1MacIdConfigParm);
    #else
    ret = wf_mcu_fill_mbox1_fw(nic_info, wMBOX1_9086X_MACID_CFG, u1wMBOX1MacIdConfigParm, wMBOX1_MACID_CFG_LEN);
    #endif
    return ret;
}

wf_s32 wf_odm_report_rssi(nic_info_st *nic_info)
{
    wf_s32 ret = WF_RETURN_OK;

    return ret;
}

int wf_odm_update_wdn_info(nic_info_st *nic_info)
{
    odm_mgnt_st *odm = (odm_mgnt_st *)nic_info->odm;
    odm_wdn_info_st *odm_wdn = NULL;
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
            ret = wf_mcu_get_odm_wdn_info(nic_info,i);
            wdn_net_info = wf_wdn_find_info_by_id(nic_info,i);
            if(wdn_net_info && wf_true == odm_wdn->ra_change)
            {
                ret = wf_mcu_get_rate_bitmap(nic_info, wdn_net_info, &rate_bitmap);
                ret = wf_odm_set_rfconfig(nic_info,wdn_net_info->wdn_id, wdn_net_info->raid, wdn_net_info->bw_mode, odm->wdn[i].htpriv.sgi_20m, rate_bitmap);
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
            ret = wf_mcu_get_odm_wdn_info(nic_info,i);
            wdn_net_info = wf_wdn_find_info_by_id(nic_info,i);
            if(wdn_net_info)
            {
                ret = wf_mcu_get_rate_bitmap(nic_info, wdn_net_info, &rate_bitmap);
                ret = wf_odm_set_rfconfig(nic_info,wdn_net_info->wdn_id, wdn_net_info->raid, wdn_net_info->bw_mode, odm->wdn[i].htpriv.sgi_20m, rate_bitmap);
            }
        }
    }
    #endif

    return WF_RETURN_OK;
}


wf_s32 wf_odm_set_media_status(nic_info_st *nic_info, wf_bool opmode,
                                     wf_bool miracast, wf_bool miracast_sink, wf_u8 role,
                                     wf_u8 macid,wf_bool macid_ind,wf_u8 macid_end)
{
    wf_u8 parm[wMBOX1_MEDIA_STATUS_RPT_LEN] = { 0 };
    int ret = 0;

    SET_wMBOX1CMD_MSRRPT_PARM_OPMODE(parm, opmode);
    SET_wMBOX1CMD_MSRRPT_PARM_MACID_IND(parm, macid_ind);
    SET_wMBOX1CMD_MSRRPT_PARM_MIRACAST(parm, miracast);
    SET_wMBOX1CMD_MSRRPT_PARM_MIRACAST_SINK(parm, miracast_sink);
    SET_wMBOX1CMD_MSRRPT_PARM_ROLE(parm, role);
    SET_wMBOX1CMD_MSRRPT_PARM_MACID(parm, macid);
    SET_wMBOX1CMD_MSRRPT_PARM_MACID_END(parm, macid_end);

    #if CHIP51_DIRECT_ACCESS
    ret = FillH2CCmd(nic_info,wMBOX1_9086X_MEDIA_STATUS_RPT,wMBOX1_MEDIA_STATUS_RPT_LEN,parm);
    #else
    ret = wf_mcu_fill_mbox1_fw(nic_info, wMBOX1_9086X_MEDIA_STATUS_RPT, parm ,wMBOX1_MEDIA_STATUS_RPT_LEN);
    #endif

    return ret;
}

wf_s32 wf_odm_connect_media_status(nic_info_st *nic_info,wdn_net_info_st *wdn_net_info)
{
    wf_s32 ret = 0;

    ret = wf_odm_set_media_status(nic_info,wf_true,wf_false,wf_false,wMBOX1_MSR_ROLE_AP,wdn_net_info->wdn_id,0,0);
    return ret;
}

wf_s32 wf_odm_disconnect_media_status(nic_info_st *nic_info,wdn_net_info_st *wdn_net_info)
{
    wf_s32 ret = 0;

    ret = wf_odm_set_media_status(nic_info,wf_false,wf_false,wf_false,wMBOX1_MSR_ROLE_RSVD,wdn_net_info->wdn_id,0,0);
    return ret;
}

#ifdef CFG_ENABLE_AP_MODE
wf_s32 wf_ap_odm_connect_media_status(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_s32 ret = 0;

    ret = wf_odm_set_media_status(pnic_info,wf_true, wf_false, wf_false, wMBOX1_MSR_ROLE_STA, pwdn_info->wdn_id, 0, 0);
    return ret;
}

wf_s32 wf_ap_odm_disconnect_media_status(nic_info_st *pnic_info,wdn_net_info_st *pwdn_info)
{
    wf_s32 ret = 0;

    ret = wf_odm_set_media_status(pnic_info,wf_false, wf_false, wf_false, wMBOX1_MSR_ROLE_STA, pwdn_info->wdn_id, 0, 0);
    return ret;
}
#endif

static wf_u8 _is_fw_read_cmd_down(nic_info_st *nic_info, wf_u8 msgbox_num)
{
    wf_u8  read_down = wf_false;
    int retry_cnts = 100;
    wf_u8 valid;

    do
    {
        valid = wf_io_read8(nic_info, REG_HMETFR,NULL) & BIT(msgbox_num);
        if (0 == valid)
        {
            read_down = wf_true;
        }
        else
        {
            wf_mdelay(1);
         }
    } while ((!read_down) && (retry_cnts--));

    return read_down;

}


/*****************************************
* H2C Msg format :
*| 31 - 8       |7-5    | 4 - 0 |
*| h2c_msg  |Class  |CMD_ID |
*| 31-0                     |
*| Ext msg                  |
*
******************************************/
wf_s32 FillH2CCmd(nic_info_st *nic_info, wf_u8 ElementID, wf_u32 CmdLen, wf_u8 *pCmdBuffer)
{
    wf_u8  h2c_box_num;
    wf_u32 msgbox_addr;
    wf_u32 msgbox_ex_addr = 0;
    wf_u32 h2c_cmd = 0;
    wf_u32 h2c_cmd_ex = 0;
    wf_s32 ret = WF_RETURN_FAIL;
    odm_mgnt_st *odm = nic_info->odm;
    if (!pCmdBuffer)
    {
        LOG_E("[%s] pCmdBuffer is null",__func__);
        return ret;
    }
    if (CmdLen > RTL8188F_MAX_CMD_LEN)
    {
        LOG_E("[%s] CmdLen[%d] > RTL8188F_MAX_CMD_LEN[%d]",__func__,CmdLen,RTL8188F_MAX_CMD_LEN);
        return ret;
    }
    if (nic_info->is_driver_stopped || nic_info->is_surprise_removed)
    {
        return WF_RETURN_FAIL;
    }

    wf_lock_mutex_lock(&odm->chip_op_mutex);
    /*pay attention to if  race condition happened in  H2C cmd setting. */
    h2c_box_num = odm->LastHMEBoxNum;

    if (!_is_fw_read_cmd_down(nic_info, h2c_box_num))
    {
        LOG_I(" fw read cmd failed...,h2c_box_num:%d\n",h2c_box_num);
        LOG_I("[%s] MAC_1C0=%08x, MAC_1C4=%08x, MAC_1C8=%08x, MAC_1CC=%08x\n",
            __func__,
            wf_io_read32(nic_info, 0x1c0,NULL),
            wf_io_read32(nic_info, 0x1c4,NULL),
            wf_io_read32(nic_info, 0x1c8,NULL),
            wf_io_read32(nic_info, 0x1cc,NULL));
        LOG_I("[%s] 0x1c0: 0x%8x,0x1c4: 0x%8x\n",
            __func__,
            wf_io_read32(nic_info, 0x1c0,NULL),
            wf_io_read32(nic_info, 0x1c4,NULL));
        return ret;
    }

    if (CmdLen <= 3)
    {
        wf_memcpy((wf_u8 *)(&h2c_cmd) + 1, pCmdBuffer, CmdLen);
    }
    else
    {
        wf_memcpy((wf_u8 *)(&h2c_cmd) + 1, pCmdBuffer, 3);
        wf_memcpy((wf_u8 *)(&h2c_cmd_ex), pCmdBuffer + 3, CmdLen - 3);
        /**(wf_u8*)(&h2c_cmd) |= BIT(7); */
    }

    *(wf_u8 *)(&h2c_cmd) |= ElementID;

    if (CmdLen > 3)
    {
        msgbox_ex_addr = REG_HMEBOX_EXT0_8188F + (h2c_box_num * RTL8188F_EX_MESSAGE_BOX_SIZE);
        h2c_cmd_ex = wf_le32_to_cpu(h2c_cmd_ex);
        wf_io_write32(nic_info, msgbox_ex_addr, h2c_cmd_ex);
    }

    msgbox_addr = REG_HMEBOX_0 + (h2c_box_num * MESSAGE_BOX_SIZE);
    h2c_cmd = wf_le32_to_cpu(h2c_cmd);
    wf_io_write32(nic_info, msgbox_addr, h2c_cmd);

    LOG_I("[%s] MSG_BOX:%d, CmdLen(%d), CmdID(0x%x), reg:0x%x =>h2c_cmd:0x%.8x, reg:0x%x =>h2c_cmd_ex:0x%.8x\n"
        ,__func__,odm->LastHMEBoxNum , CmdLen, ElementID, msgbox_addr, h2c_cmd, msgbox_ex_addr, h2c_cmd_ex);

    odm->LastHMEBoxNum = (h2c_box_num + 1) % MAX_H2C_BOX_NUMS;
    wf_lock_mutex_unlock(&odm->chip_op_mutex);

    return WF_RETURN_OK;
}

wf_s32 wf_mcu_odm_init_msg(nic_info_st *nic_info, odm_msg_st *msg)
{
    wf_s32 ret = 0;

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_INIT_MSG_VAR, (wf_u32 *) msg, sizeof(odm_msg_st) / 4, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_INIT_MSG_VAR, (wf_u32 *) msg, sizeof(odm_msg_st) / 4, NULL, 0);
    }
    
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("UMSG_OPS_HAL_INIT_MSG_VAR failed");
        return -1;
    }

    return 0;

}

wf_s32 wf_mcu_handle_bb_lccalibrate(nic_info_st *nic_info)
{
    wf_u32 timeout      = 2000;
    wf_u32 timecount    = 0;
    wf_s32 ret = 0;

	#if 1	
    odm_mgnt_st *odm = NULL;
    odm = (odm_mgnt_st *)nic_info->odm;

    if (NULL == odm)
    {
        return WF_RETURN_FAIL;
    }

    wf_get_odm_msg(nic_info);
    if (!(odm->odm_msg.ability & WF_BIT(26)))
        return WF_RETURN_OK;

    while (odm->odm_msg.bScanInProcess && timecount < timeout)
    {
        wf_mdelay(50);
        timecount += 50;
        wf_get_odm_msg(nic_info);
    }
	#endif

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_CALI_LLC, NULL, 0, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_CALI_LLC, NULL, 0, NULL, 0);
    }

    if (WF_RETURN_FAIL == ret )
    {
        LOG_E("[%s] UMSG_OPS_HAL_CALI_LLC failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}



wf_s32  wf_mcu_handle_bb_iq_calibrate(nic_info_st *nic_info, wf_u8 channel)
{
    wf_s32 ret = 0;
    wf_u32 buff[2] = { 0 };
    wf_s32 len = 2;
	
    #if 1		
    odm_mgnt_st *odm = NULL;
	odm = (odm_mgnt_st *)nic_info->odm;

    if (NULL == odm)
    {
        return WF_RETURN_FAIL;
    }

    wf_get_odm_msg(nic_info);
    if (!(odm->odm_msg.ability & WF_BIT(26)))
        return WF_RETURN_OK;
	#endif

#if WF_NEED_CHANGE_DEBUG
    buff[0] = pMptCtx->MptRfPath;
    buff[1] = (wf_u32) nic_info->hw_info.current_channel;
#else
    buff[0] = 0;
    buff[1] = channel;
#endif
    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_PHY_IQ_CALIBRATE, buff, len, NULL,  0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_PHY_IQ_CALIBRATE, buff, len, NULL,  0);
    }
    if (WF_RETURN_FAIL == ret )
    {
        LOG_E("[%s] UMSG_OPS_HAL_PHY_IQ_CALIBRATE failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}


wf_s32 wf_mcu_get_odm_wdn_info(nic_info_st *nic_info, wf_u32 wdn_id)
{
    wf_s32 ret = 0;
    odm_mgnt_st *odm = (odm_mgnt_st *)nic_info->odm;
    int len = WF_RND4(sizeof(odm->wdn[0]));

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_GET_MSG_STA_INFO, &wdn_id, 1, (wf_u32 *)&odm->wdn[wdn_id], len / 4);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_SYNC_MSG_STA_INFO, &wdn_id, 1, (wf_u32 *)&odm->wdn[wdn_id], len / 4);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
    }

    return ret;
}


wf_s32 wf_mcu_set_odm_wdn_info(nic_info_st *nic_info, wf_u16 wdn_id)
{
    wf_s32 ret = 0;
    odm_mgnt_st *odm = (odm_mgnt_st *)nic_info->odm;
    wf_u32 *pbuf  = NULL;
    wf_s32 len = 0;
    wf_s32 i = 0;

    if ( wdn_id >= ODM_WDN_INFO_SIZE)
    {
        LOG_E("[%s] wdn_id[%d] is bigger than ODM_WDN_INFO_SIZE", __func__, wdn_id);
        return WF_RETURN_FAIL;
    }

    len = WF_RND4(sizeof(odm->wdn[wdn_id]));

    pbuf = (wf_u32 *) wf_kzalloc(len);
    if (!pbuf)
    {
        LOG_E("[%s] failed", __func__);
        return WF_RETURN_FAIL;
    }

    wf_memcpy(pbuf, &odm->wdn[wdn_id], len);
#if 0
    for (i = 0; i < len / 4; i++)
    {
        LOG_I("[%s] 0x%x", __func__, pbuf[i]);
    }
#endif

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_SYNC_MSG_STA_INFO, pbuf, len / 4, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_SYNC_MSG_STA_INFO, pbuf, len / 4, NULL, 0);
    }

    wf_kfree(pbuf);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

static wf_s32 bit_value_from_ieee_value_to_get_func(wf_u8 val, wf_u8 flag)
{
    wf_u8 dot11_rate_table[] ={ 2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108, 0 };

    wf_s32 i = 0;
    if (flag)
    {
        while (dot11_rate_table[i] != 0)
        {
            if (dot11_rate_table[i] == val)
                return BIT(i);
            i++;
        }
    }
    return 0;
}


wf_s32 wf_mcu_get_rate_bitmap(nic_info_st *nic_info, wdn_net_info_st *wdn_net_info, wf_u32 *rate_bitmap)
{
    wf_s32 ret = 0;
    wf_u32 buf[3] = {0};
    odm_mgnt_st *odm = (odm_mgnt_st *)nic_info->odm;
    odm_wdn_info_st *odm_wdn  = NULL;
    wf_u32 ra_mask = 0;
    wf_s32 i = 0;

    if (NULL == wdn_net_info)
    {
        LOG_E("[%s] param is null, check!!!", __func__);
        return WF_RETURN_OK;
    }

    odm_wdn = &odm->wdn[wdn_net_info->wdn_id];

    /*calc ra_mask*/
    for (i = 0; i < wdn_net_info->datarate_len; i++)
    {
        if (wdn_net_info->datarate[i])
            ra_mask |= bit_value_from_ieee_value_to_get_func(wdn_net_info->datarate[i] & 0x7f, 1);
    }

    for (i = 0; i < wdn_net_info->ext_datarate_len; i++)
    {
        if (wdn_net_info->ext_datarate[i])
            ra_mask |= bit_value_from_ieee_value_to_get_func(wdn_net_info->ext_datarate[i] & 0x7f, 1);
    }
#define HT_MCS_SUPPORT_MCS_LIMIT (8)
    for (i = 0; i < HT_MCS_SUPPORT_MCS_LIMIT; i++)
    {
        if (odm_wdn->htpriv.ht_cap.supp_mcs_set[i / 8] & WF_BIT(i % 8))
        {
            ra_mask |= WF_BIT(i + 12);
        }
    }

    LOG_D("[%s] 0x%x", __func__, ra_mask);

    odm_wdn = &odm->wdn[wdn_net_info->wdn_id];
    buf[0] = odm_wdn->mac_id;
    buf[1] = ra_mask;
    buf[2] = odm_wdn->rssi_level;
    LOG_I("[%s] 0x%x,0x%x,0x%x", __func__, buf[0], buf[1], buf[2]);
    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_0PS_MSG_GET_RATE_BITMAP, buf, 3, rate_bitmap, 1);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_DXX0_0PS_MSG_GET_RATE_BITMAP, buf, 3, rate_bitmap, 1);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return WF_RETURN_FAIL;
    }

    return ra_mask;
}


wf_s32 wf_odm_update(nic_info_st *nic_info, wdn_net_info_st *wdn_net_info)
{
    odm_mgnt_st *odm = (odm_mgnt_st *)nic_info->odm;
    wf_u32 rate_bitmap = 0;
    wf_u32 macid = wdn_net_info->wdn_id;
    wf_u8 sgi;
    wf_s32 ret;

    odm->wdn[macid].bUsed = wf_true;//???
    odm->wdn[macid].mac_id = wdn_net_info->wdn_id;

    wf_memcpy(odm->wdn[macid].hwaddr,wdn_net_info->mac,WF_ETH_ALEN);
    odm->wdn[macid].ra_rpt_linked   = wf_false;
    odm->wdn[macid].wireless_mode   = wdn_net_info->network_type;
    odm->wdn[macid].rssi_level      = 0;
    odm->wdn[macid].ra_change       = wf_false;
    odm->wdn[macid].htpriv.ht_option           = wdn_net_info->htpriv.ht_option;
    odm->wdn[macid].htpriv.ampdu_enable        = wdn_net_info->htpriv.ampdu_enable;
    odm->wdn[macid].htpriv.tx_amsdu_enable     = wdn_net_info->htpriv.tx_amsdu_enable;
    odm->wdn[macid].htpriv.bss_coexist         = wdn_net_info->htpriv.bss_coexist;
    odm->wdn[macid].htpriv.tx_amsdu_maxlen     = wdn_net_info->htpriv.tx_amsdu_maxlen;
    odm->wdn[macid].htpriv.rx_ampdu_maxlen     = wdn_net_info->htpriv.rx_ampdu_maxlen;
    odm->wdn[macid].htpriv.rx_ampdu_min_spacing= wdn_net_info->htpriv.rx_ampdu_min_spacing;
    odm->wdn[macid].htpriv.ch_offset           = wdn_net_info->htpriv.ch_offset;
    odm->wdn[macid].htpriv.sgi_20m             = wdn_net_info->htpriv.sgi_20m;
    odm->wdn[macid].htpriv.sgi_40m             = wdn_net_info->htpriv.sgi_40m;
    odm->wdn[macid].htpriv.agg_enable_bitmap   = wdn_net_info->htpriv.agg_enable_bitmap;
    odm->wdn[macid].htpriv.candidate_tid_bitmap= wdn_net_info->htpriv.candidate_tid_bitmap;
    odm->wdn[macid].htpriv.ldpc_cap            = wdn_net_info->htpriv.ldpc;
    odm->wdn[macid].htpriv.stbc_cap            = wdn_net_info->htpriv.tx_stbc;//??rx_stbc
    odm->wdn[macid].htpriv.smps_cap            = wdn_net_info->htpriv.smps;
    wf_memcpy(&odm->wdn[macid].htpriv.ht_cap,&wdn_net_info->ht_cap,sizeof(wdn_net_info->ht_cap));

    ret = wf_mcu_set_odm_wdn_info(nic_info, wdn_net_info->wdn_id);
    if (ret == WF_RETURN_FAIL)
        return ret;

    ret = wf_mcu_get_rate_bitmap(nic_info, wdn_net_info, &rate_bitmap);
    if (ret == WF_RETURN_FAIL)
        return ret;

    sgi = wf_ra_sGI_get(wdn_net_info, 1);
    LOG_D("macid=%d raid=%d bw=%d sgi=%d rate_bitmap=0x%08x", macid, wdn_net_info->raid, wdn_net_info->bw_mode, sgi, rate_bitmap);

    ret = wf_odm_set_rfconfig(nic_info, macid, wdn_net_info->raid, wdn_net_info->bw_mode, sgi, rate_bitmap);
    if (ret == WF_RETURN_FAIL)
        return ret;

    ret = wf_odm_connect_media_status(nic_info, wdn_net_info);
    if (ret == WF_RETURN_FAIL)
        return ret;

    return WF_RETURN_OK;
}

