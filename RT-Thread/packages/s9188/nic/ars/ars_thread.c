#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_DRIVER_SUPPORT
#if 0
#define ARS_THD_DBG(fmt, ...)      LOG_D("ARS_THD[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_THD_PRT(fmt, ...)      LOG_D("ARS_THD-"fmt,##__VA_ARGS__)

#else
#define ARS_THD_DBG(fmt, ...)
#define ARS_THD_PRT(fmt, ...)
#endif

#define ARS_THD_INFO(fmt, ...)      LOG_I("ARS_THD-"fmt,##__VA_ARGS__)
#define ARS_THD_ERR(fmt, ...)      LOG_E("ARS_THD-"fmt,##__VA_ARGS__)


wf_s32 odm_CommonInfoSelfUpdate(ars_st *pars)
{
    wf_u8 EntryCnt = 0;
    wf_u8 num_active_client = 0;
//    wf_u32  i= 0;
    wf_u32 OneEntry_MACID = 0;
//    wf_u32 ma_rx_tp = 0;

    if(pars->wdn_net)
    {
        if(pars->wdn_net->bw_mode == CHANNEL_WIDTH_20)
        {
            pars->ControlChannel = pars->wdn_net->channel;
        }
        else
        {
            pars->ControlChannel = pars->wdn_net->channel;
        }
    }

    EntryCnt = wf_wdn_get_cnt(pars->nic_info);
    
    if(EntryCnt == 1)
    {
        pars->bOneEntryOnly = wf_true;
        pars->OneEntry_MACID=OneEntry_MACID;
    }
    else
    {
        pars->bOneEntryOnly = wf_false;
    }

    pars->pre_number_linked_client = pars->number_linked_client;
    pars->pre_number_active_client = pars->number_active_client;
    
    pars->number_linked_client = EntryCnt;
    pars->number_active_client = num_active_client;

    return WF_RETURN_OK;
}

wf_s32 phydm_BasicDbgMessage(ars_st *pars)
{
    PFALSE_ALARM_STATISTICS FalseAlmCnt = &pars->FalseAlmCnt;
    ars_dig_info_st *dig = &pars->dig;
    wf_u8   legacy_table[12] = {1, 2, 5, 11, 6, 9, 12, 18, 24, 36, 48, 54};
    wf_u8   vht_en = ((pars->RxRate) >= ODM_RATEVHTSS1MCS0) ? 1 : 0;
    if (pars->RxRate <= ODM_RATE11M) 
    {
        ARS_THD_INFO("[CCK AGC Report] LNA_idx = %d, VGA_idx = %d",pars->cck_lna_idx, pars->cck_vga_idx);        
    } 
    else 
    {
        ARS_THD_INFO("[OFDM AGC Report] { 0x%x, 0x%x, 0x%x, 0x%x }",
            pars->ofdm_agc_idx[0], pars->ofdm_agc_idx[1], pars->ofdm_agc_idx[2], pars->ofdm_agc_idx[3]);    
    }

    ARS_THD_INFO("RSSI: { %d,  %d,  %d,  %d },    RxRate: { %s%s%s%s%d%s}",
        (pars->RSSI_A == 0xff) ? 0 : pars->RSSI_A , 
        (pars->RSSI_B == 0xff) ? 0 : pars->RSSI_B , 
        (pars->RSSI_C == 0xff) ? 0 : pars->RSSI_C, 
        (pars->RSSI_D == 0xff) ? 0 : pars->RSSI_D,
        ((pars->RxRate >= ODM_RATEVHTSS1MCS0) && (pars->RxRate <= ODM_RATEVHTSS1MCS9)) ? "VHT 1ss  " : "",
        ((pars->RxRate >= ODM_RATEVHTSS2MCS0) && (pars->RxRate <= ODM_RATEVHTSS2MCS9)) ? "VHT 2ss " : "",
        ((pars->RxRate >= ODM_RATEVHTSS3MCS0) && (pars->RxRate <= ODM_RATEVHTSS3MCS9)) ? "VHT 3ss " : "",
        (pars->RxRate >= ODM_RATEMCS0) ? "MCS " : "",
        (vht_en) ? ((pars->RxRate - ODM_RATEVHTSS1MCS0)%10) : ((pars->RxRate >= ODM_RATEMCS0) ? (pars->RxRate - ODM_RATEMCS0) : ((pars->RxRate <= ODM_RATE54M)?legacy_table[pars->RxRate]:0)),
        (pars->RxRate >= ODM_RATEMCS0) ? "" : "M");

    ARS_THD_INFO("[CCA Cnt] {CCK, OFDM, Total} = {%d, %d, %d}",  
        FalseAlmCnt->Cnt_CCK_CCA, FalseAlmCnt->Cnt_OFDM_CCA, FalseAlmCnt->Cnt_CCA_all);

    ARS_THD_INFO("[FA Cnt] {CCK, OFDM, Total} = {%d, %d, %d}",   
        FalseAlmCnt->Cnt_Cck_fail, FalseAlmCnt->Cnt_Ofdm_fail, FalseAlmCnt->Cnt_all);

    ARS_THD_INFO("[OFDM FA Detail] Parity_Fail = ( %d ), Rate_Illegal = ( %d ), CRC8_fail = ( %d ), Mcs_fail = ( %d ), Fast_Fsync = ( %d ), SB_Search_fail = ( %d )",    
        FalseAlmCnt->Cnt_Parity_Fail, FalseAlmCnt->Cnt_Rate_Illegal, FalseAlmCnt->Cnt_Crc8_fail, FalseAlmCnt->Cnt_Mcs_fail, FalseAlmCnt->Cnt_Fast_Fsync, FalseAlmCnt->Cnt_SB_Search_fail);
    
    ARS_THD_PRT("bLinked = %d, RSSI_Min = %d, CurrentIGI = 0x%x, bNoisy=%d",
        pars->bLinked, pars->RSSI_Min, dig->CurIGValue, pars->NoisyDecision);

    return WF_RETURN_OK;
}


wf_s32 phydm_NoisyDetection(ars_st *pars)
{
    wf_u32  Total_FA_Cnt, Total_CCA_Cnt;
    wf_u32  Score = 0, i, Score_Smooth;
    
    Total_CCA_Cnt = pars->FalseAlmCnt.Cnt_CCA_all;
    Total_FA_Cnt  = pars->FalseAlmCnt.Cnt_all;

    for(i=0;i<=16;i++)
    {
        if( Total_FA_Cnt*16>=Total_CCA_Cnt*(16-i) )
        {
            Score = 16-i;
            break;
        }
    }
    // NoisyDecision_Smooth = NoisyDecision_Smooth>>1 + (Score<<3)>>1;
    pars->NoisyDecision_Smooth = (pars->NoisyDecision_Smooth>>1) + (Score<<2);

    // Round the NoisyDecision_Smooth: +"3" comes from (2^3)/2-1
    Score_Smooth = (Total_CCA_Cnt>=300)?((pars->NoisyDecision_Smooth+3)>>3):0;

    pars->NoisyDecision = (Score_Smooth>=3)?1:0;
    
    switch(Score_Smooth)
    {
        case 0:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=0%%\n");
            break;
        case 1:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=6.25%%\n");
            break;
        case 2:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=12.5%%\n");
            break;
        case 3:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=18.75%%\n");
            break;
        case 4:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=25%%\n");
            break;
        case 5:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=31.25%%\n");
            break;
        case 6:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=37.5%%\n");
            break;
        case 7:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=43.75%%\n");
            break;
        case 8:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=50%%\n");
            break;
        case 9:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=56.25%%\n");
            break;
        case 10:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=62.5%%\n");
            break;
        case 11:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=68.75%%\n");
            break;
        case 12:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=75%%\n");
            break;
        case 13:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=81.25%%\n");
            break;
        case 14:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=87.5%%\n");
            break;
        case 15:
            ARS_THD_INFO ("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=93.75%%\n");            
            break;
        case 16:
            ARS_THD_INFO("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=100%%\n");
            break;
        default:
            ARS_THD_INFO("[NoisyDetection] Unknown Value!! Need Check!!\n");            
    }

    ARS_THD_INFO("[NoisyDetection] Total_CCA_Cnt=%d, Total_FA_Cnt=%d, NoisyDecision_Smooth=%d, Score=%d, Score_Smooth=%d, pars->NoisyDecision=%d\n",
    Total_CCA_Cnt, Total_FA_Cnt, pars->NoisyDecision_Smooth, Score, Score_Smooth, pars->NoisyDecision);

    return WF_RETURN_OK;
}

wf_s32 odm_UpdatePowerTrainingState(void *ars    )
{
    ars_st *pars  = ars;
    PFALSE_ALARM_STATISTICS     FalseAlmCnt     = &pars->FalseAlmCnt;
    ars_dig_info_st*            pDM_DigTable    = &pars->dig;
    wf_u32                      score = 0;

    if(!(pars->SupportAbility & ODM_BB_PWR_TRAIN))
    {
        return WF_RETURN_OK;
    }

    pars->bChangeState = wf_false;

    // Debug command
    if(pars->ForcePowerTrainingState)
    {
        if(pars->ForcePowerTrainingState == 1 && !pars->bDisablePowerTraining)
        {
            pars->bChangeState = wf_true;
            pars->bDisablePowerTraining = wf_true;
        }
        else if(pars->ForcePowerTrainingState == 2 && pars->bDisablePowerTraining)
        {
            pars->bChangeState = wf_true;
            pars->bDisablePowerTraining = wf_false;
        }

        pars->PT_score = 0;
        pars->dbg.dbg_info.NumQryPhyStatusOFDM  = 0;
        pars->dbg.dbg_info.NumQryPhyStatusCCK   = 0;
        ARS_THD_DBG("ForcePowerTrainingState = %d\n", pars->ForcePowerTrainingState);
        return WF_RETURN_OK;
    }
    
    if(!pars->bLinked)
    {
        return WF_RETURN_OK;
    }
    
    // First connect
    if((pars->bLinked) && (pDM_DigTable->bMediaConnect_0 == wf_false))
    {
        pars->PT_score = 0;
        pars->bChangeState = wf_true;
        pars->dbg.dbg_info.NumQryPhyStatusOFDM = 0;
        pars->dbg.dbg_info.NumQryPhyStatusCCK = 0;
        ARS_THD_DBG("odm_UpdatePowerTrainingState(): First Connect\n");
        return WF_RETURN_OK;
    }

    // Compute score
    if(pars->NHM_cnt_0 >= 215)
        score = 2;
    else if(pars->NHM_cnt_0 >= 190) 
        score = 1;                          // unknow state
    else
    {
        wf_u32  RX_Pkt_Cnt = 0;
        
        RX_Pkt_Cnt = (wf_u32)(pars->dbg.dbg_info.NumQryPhyStatusOFDM) + (wf_u32)(pars->dbg.dbg_info.NumQryPhyStatusCCK);
        
        if((FalseAlmCnt->Cnt_CCA_all > 31 && RX_Pkt_Cnt > 31) && (FalseAlmCnt->Cnt_CCA_all >= RX_Pkt_Cnt))
        {
            if((RX_Pkt_Cnt + (RX_Pkt_Cnt >> 1)) <= FalseAlmCnt->Cnt_CCA_all)
            {
                score = 0;
            }
            else if((RX_Pkt_Cnt + (RX_Pkt_Cnt >> 2)) <= FalseAlmCnt->Cnt_CCA_all)
            {
                score = 1;
            }
            else
            {
                score = 2;
            }
        }
        ARS_THD_PRT("odm_UpdatePowerTrainingState(): RX_Pkt_Cnt = %d, Cnt_CCA_all = %d\n", 
            RX_Pkt_Cnt, FalseAlmCnt->Cnt_CCA_all);
    }
    ARS_THD_PRT("odm_UpdatePowerTrainingState(): NumQryPhyStatusOFDM = %d, NumQryPhyStatusCCK = %d\n",
            (wf_u32)(pars->dbg.dbg_info.NumQryPhyStatusOFDM), (wf_u32)(pars->dbg.dbg_info.NumQryPhyStatusCCK));
    ARS_THD_PRT("odm_UpdatePowerTrainingState(): NHM_cnt_0 = %d, score = %d\n", 
        pars->NHM_cnt_0, score);

    // smoothing
    pars->PT_score = (score << 4) + (pars->PT_score>>1) + (pars->PT_score>>2);
    score = (pars->PT_score + 32) >> 6;
    ARS_THD_DBG("odm_UpdatePowerTrainingState(): PT_score = %d, score after smoothing = %d\n", 
        pars->PT_score, score);

    // Mode decision
    if(score == 2)
    {
        if(pars->bDisablePowerTraining)
        {
            pars->bChangeState = wf_true;
            pars->bDisablePowerTraining = wf_false;
            ARS_THD_DBG("odm_UpdatePowerTrainingState(): Change state\n");
        }
        ARS_THD_DBG("odm_UpdatePowerTrainingState(): Enable Power Training\n");
    }
    else if(score == 0)
    {
        if(!pars->bDisablePowerTraining)
        {
            pars->bChangeState = wf_true;
            pars->bDisablePowerTraining = wf_true;
            ARS_THD_DBG("odm_UpdatePowerTrainingState(): Change state\n");
        }
        ARS_THD_DBG("odm_UpdatePowerTrainingState(): Disable Power Training\n");
    }

    pars->dbg.dbg_info.NumQryPhyStatusOFDM = 0;
    pars->dbg.dbg_info.NumQryPhyStatusCCK = 0;

    return WF_RETURN_OK;
}

wf_s32 odm_IQCalibrate(ars_st *pars)
{
    return WF_RETURN_OK;
}

wf_s32 phydm_rf_watchdog(ars_st *pars)
{
    odm_TXPowerTrackingCheckCE(pars);
    odm_IQCalibrate(pars);

    return WF_RETURN_OK;
}

/* Justin: According to the current RRSI to adjust Response Frame TX power, 2012/11/05 */
wf_s32 odm_dtc(ars_st *pars)
{
#ifdef CONFIG_DM_RESP_TXAGC
    #define DTC_BASE            35  /* RSSI higher than this value, start to decade TX power */
    #define DTC_DWN_BASE       (DTC_BASE-5) /* RSSI lower than this value, start to increase TX power */

    /* RSSI vs TX power step mapping: decade TX power */
    static const u8 dtc_table_down[]={
        DTC_BASE,
        (DTC_BASE+5),
        (DTC_BASE+10),
        (DTC_BASE+15),
        (DTC_BASE+20),
        (DTC_BASE+25)
    };

    /* RSSI vs TX power step mapping: increase TX power */
    static const u8 dtc_table_up[]={
        DTC_DWN_BASE,
        (DTC_DWN_BASE-5),
        (DTC_DWN_BASE-10),
        (DTC_DWN_BASE-15),
        (DTC_DWN_BASE-15),
        (DTC_DWN_BASE-20),
        (DTC_DWN_BASE-20),
        (DTC_DWN_BASE-25),
        (DTC_DWN_BASE-25),
        (DTC_DWN_BASE-30),
        (DTC_DWN_BASE-35)
    };

    wf_u8 i;
    wf_u8 dtc_steps=0;
    wf_u8 sign;
    wf_u8 resp_txagc=0;

    #if 0
    /* As DIG is disabled, DTC is also disable */
    if(!(pars->SupportAbility & ODM_XXXXXX))
        return;
    #endif

    if (DTC_BASE < pars->RSSI_Min) {
        /* need to decade the CTS TX power */
        sign = 1;
        for (i=0;i<WF_ARRAY_SIZE(dtc_table_down);i++)
        {
            if ((dtc_table_down[i] >= pars->RSSI_Min) || (dtc_steps >= 6))
                break;
            else
                dtc_steps++;
        }
    }
#if 0
    else if (DTC_DWN_BASE > pars->RSSI_Min)
    {
        /* needs to increase the CTS TX power */
        sign = 0;
        dtc_steps = 1;
        for (i=0;i<WF_ARRAY_SIZE(dtc_table_up);i++)
        {
            if ((dtc_table_up[i] <= pars->RSSI_Min) || (dtc_steps>=10))
                break;
            else
                dtc_steps++;
        }
    }
#endif
    else
    {
        sign = 0;
        dtc_steps = 0;
    }

    resp_txagc = dtc_steps | (sign << 4);
    resp_txagc = resp_txagc | (resp_txagc << 5);
    wf_io_write8(pars->nic_info, 0x06d9, resp_txagc);

    ARS_THD_DBG("%s RSSI_Min:%u, set RESP_TXAGC to %s %u\n", 
        __func__, pars->RSSI_Min, sign?"minus":"plus", dtc_steps);
    
#endif /* CONFIG_RESP_TXAGC_ADJUST */

    return WF_RETURN_OK;

}

wf_s32 odm_CommonInfoSelfReset(ars_st *pars)
{
    pars->dbg.dbg_info.NumQryBeaconPkt = 0;
    return WF_RETURN_OK;
}

wf_s32 ars_thread_sema_post(void *ars)
{
    ars_st *pars = ars;
    ars_thread_info_st *thread = &pars->ars_thread;
    
    wf_os_api_sema_post(&thread->ars_sema);
    return WF_RETURN_OK;
}

static wf_s32 ars_thread_sema_wait(void *ars)
{
    ars_st *pars = ars;
    ars_thread_info_st *thread = &pars->ars_thread;

    return wf_os_api_sema_wait(&thread->ars_sema);
};

static wf_s32 ars_thread_core(nic_info_st *nic_info)
{
    ars_st * pars            = NULL;
    hw_info_st  *hw          = NULL;
    wdn_net_info_st *wdn_net = NULL;
    wf_s32 err               = 0;
    wf_u8  value_u8          = 0;
    wf_s32 ret               = 0;
    wf_bool bconnect         = wf_false;
    ars_dig_info_st*  pDM_DigTable = NULL;
    
    pars = nic_info->ars;

    pDM_DigTable = &pars->dig;
        
    //mp mode
    hw = nic_info->hw_info;
    if(1 == hw->mp_mode)
    {
        return -1;
    }

    #if 1 // need move to connect process
    wf_mlme_get_connect(nic_info, &bconnect);
    if(wf_false == bconnect)
    {
        return -1;
    }
    
    wdn_net = wf_wdn_find_info(nic_info, wf_wlan_get_cur_bssid(nic_info));
    if(NULL == wdn_net)
    {
        pars->is_hook_pointer = wf_false;
        return -2;
    }
    else if(wf_false == pars->is_hook_pointer)
    {
        wf_s32 i = 0;
        pars->is_hook_pointer = wf_true;
        ODM_CmnInfoHook(pars, ODM_CMNINFO_TX_UNI, &(wdn_net->wdn_stats.tx_bytes));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_RX_UNI, &(wdn_net->wdn_stats.rx_bytes));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_TX_TP, &(wdn_net->wdn_stats.cur_tx_tp));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_RX_TP, &(wdn_net->wdn_stats.cur_rx_tp));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_WM_MODE, &wdn_net->network_type);
        ODM_CmnInfoHook(pars, ODM_CMNINFO_BAND, &(wdn_net->bw_mode));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_FORCED_RATE, &(wdn_net->datarate));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_SEC_CHNL_OFFSET, &(wdn_net->channle_offset));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_BW, &(wdn_net->bw_mode));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_SEC_MODE, &(wdn_net->auth_algo));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_CHNL, &( wdn_net->channel));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_NET_CLOSED, &(nic_info->is_up));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_FORCED_IGI_LB, &(pars->com_save.u1ForcedIgiLb));

        ODM_CmnInfoHook(pars, ODM_CMNINFO_SCAN, &(pars->com_save.bScanInProcess));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_POWER_SAVING, &(pars->com_save.bpower_saving));

        if(NIC_USB == nic_info->nic_type)
        {
            ODM_CmnInfoHook(pars, ODM_CMNINFO_HUBUSBMODE, &(pars->com_save.usb_speed));
        }

        for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
        {
            ODM_CmnInfoPtrArrayHook(pars, ODM_CMNINFO_STA_STATUS, i, NULL);
        }
    }

    pars->wdn_net = wdn_net;
    #endif

    //switch counter to RX fifo
    value_u8 = wf_io_read8(nic_info, REG_RXERR_RPT+3,&err);
    if(err)
    {
        LOG_E("wf_io_read8 REG_RXERR_RPT+3 failed");
        return -3;
    }
    ARS_THD_DBG("value_u8:0x%x",value_u8);
   
    ret = wf_io_write8(nic_info, REG_RXERR_RPT+3, value_u8|0xa0);
    pars->dbg.dbg_priv.dbg_rx_fifo_last_overflow = pars->dbg.dbg_priv.dbg_rx_fifo_curr_overflow;
    pars->dbg.dbg_priv.dbg_rx_fifo_curr_overflow = wf_io_read8(nic_info, REG_RXERR_RPT,&err);
    if(err)
    {
        LOG_E("wf_io_read8 REG_RXERR_RPT failed");
        return -3;
    }

    pars->dbg.dbg_priv.dbg_rx_fifo_diff_overflow = pars->dbg.dbg_priv.dbg_rx_fifo_curr_overflow-pars->dbg.dbg_priv.dbg_rx_fifo_last_overflow;

    ARS_THD_PRT("===================================%lld===============================================>>>>",pars->ars_thread.run_times);
    ARS_THD_PRT("dbg_rx_fifo_diff_overflow: %lld", pars->dbg.dbg_priv.dbg_rx_fifo_diff_overflow);

    pars->bLinked = wf_true;
    pars->bsta_state = wf_true;
    pars->SupportAbility =  ODM_BB_DIG
                            |ODM_BB_RA_MASK
                            |ODM_BB_DYNAMIC_TXPWR
                            |ODM_BB_FA_CNT
                            |ODM_BB_RSSI_MONITOR
                            |ODM_BB_CFO_TRACKING
                            |ODM_BB_NHM_CNT
                            |ODM_MAC_EDCA_TURBO
                            |ODM_RF_TX_PWR_TRACK
                            |ODM_RF_CALIBRATION;

#if 1
{
    mcu_msg_body_st mcu_msg;
    ret = wf_mcu_msg_body_get(nic_info,&mcu_msg);
    {
        ARS_THD_INFO("odm SupportAbility:0x%x",mcu_msg.ability);
    }
}
#endif
    ARS_THD_INFO("SupportAbility:0x%x",pars->SupportAbility);
    ret = odm_CommonInfoSelfUpdate(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_CommonInfoSelfUpdate failed",__func__);
        return ret;
    }
    // ODM_BB_FA_CNT
    ret = odm_FalseAlarmCounterStatistics(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_FalseAlarmCounterStatistics failed",__func__);
        return ret;
    }
    
    ret = phydm_BasicDbgMessage(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] phydm_BasicDbgMessage failed",__func__);
        return ret;
    }
    
    ret = phydm_NoisyDetection(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] phydm_NoisyDetection failed",__func__);
        return ret;
    }

    // ODM_BB_RSSI_MONITOR
    ret = odm_RSSIMonitorCheck(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_RSSIMonitorCheck failed",__func__);
        return ret;
    }

    // ODM_BB_PWR_TRAIN
    ret = odm_UpdatePowerTrainingState(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_UpdatePowerTrainingState failed",__func__);
        return ret;
    }

    // ODM_BB_DIG
    ret = odm_DIG(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_DIG failed",__func__);
        return ret;
    }
    

    // ODM_BB_ADAPTIVITY
    ret = Phydm_CheckAdaptivity(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] Phydm_CheckAdaptivity failed",__func__);
        return ret;
    }

    // ODM_BB_ADAPTIVITY
    ret = Phydm_Adaptivity(pars, pDM_DigTable->CurIGValue);
    if(ret)
    {
        ARS_THD_ERR("[%s] Phydm_Adaptivity failed",__func__);
        return ret;
    }

    // ODM_BB_CCK_PD
    ret = odm_CCKPacketDetectionThresh(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_CCKPacketDetectionThresh failed",__func__);
        return ret;
    }

    ret = phydm_ra_dynamic_retry_limit(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] phydm_ra_dynamic_retry_limit failed",__func__);
        return ret;
    }
    ret = phydm_ra_dynamic_retry_count(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] phydm_ra_dynamic_retry_count failed",__func__);
        return ret;
    }

    // ODM_BB_RA_MASK
    ret = odm_RefreshRateAdaptiveMask(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_RefreshRateAdaptiveMask failed",__func__);
        return ret;
    }
    
    //odm_RefreshBasicRateMask(pars);//windows has

    ret = odm_DynamicBBPowerSaving(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_DynamicBBPowerSaving failed",__func__);
        return ret;
    }
    
    ret = odm_EdcaTurboCheck(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_EdcaTurboCheck failed",__func__);
        return ret;
    }

    ret = odm_PathDiversity(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_PathDiversity failed",__func__);
        return ret;
    }

    // ODM_BB_CFO_TRACKING
    ret = ODM_CfoTracking(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] ODM_CfoTracking failed",__func__);
        return ret;
    }
    
    ret = odm_DynamicTxPower(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_DynamicTxPower failed",__func__);
        return ret;
    }
    
    ret = odm_AntennaDiversity(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_AntennaDiversity failed",__func__);
        return ret;
    }
    
    ret = phydm_Beamforming_Watchdog(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] phydm_Beamforming_Watchdog failed",__func__);
        return ret;
    }

    // ODM_RF_TX_PWR_TRACK
    ret = phydm_rf_watchdog(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] phydm_rf_watchdog failed",__func__);
        return ret;
    }
    
    ret = odm_dtc(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_dtc failed",__func__);
        return ret;
    }
    
    ret = odm_CommonInfoSelfReset(pars);
    if(ret)
    {
        ARS_THD_ERR("[%s] odm_CommonInfoSelfReset failed",__func__);
        return ret;
    }

    ARS_THD_PRT("<<<<=====================================%lld=============================================",pars->ars_thread.run_times++);
    return 0;
}
void ars_thread_handle(nic_info_st *nic_info)
{
    ars_st * pars   = NULL;
    wf_s32 ret      = 0;
    mlme_state_e state;

    ARS_THD_INFO("start");
    
    if(NULL == nic_info)
    {
        ARS_THD_DBG("input param is null");
        return;
    }
   
    pars = nic_info->ars;
    if(NULL == pars )
    {
        ARS_THD_DBG("nic_info is null");
        return;
    }

    while(1)
    {
        ret = ars_thread_sema_wait(pars);
        if(ret)
        {
            ARS_THD_DBG("odm_queue_remove failed");
            continue;
        }

        
        if((nic_info->is_driver_stopped == wf_true) || (nic_info->is_surprise_removed == wf_true))
        {
            break;
        }

        wf_mlme_get_state(nic_info, &state);
        if (state == MLME_STATE_IDLE)
        {
            ars_thread_core(nic_info);
        }
        
    }

    while(wf_os_api_thread_wait_stop(pars->ars_thread.ars_thread_tid) == wf_false)
    {
        wf_msleep(10);
    }

	wf_os_api_thread_exit(pars->ars_thread.ars_thread_tid);
   
}


wf_s32 wf_ars_process_once(nic_info_st *nic_info)
{
    if (nic_info->ars)
    {
        ars_thread_sema_post(nic_info->ars);
    }

    return WF_RETURN_OK;
}

static void thread_timer_handler(wf_os_api_timer_t * timer)
{
    ars_thread_info_st * thread = NULL;
    ars_st *pars                = NULL;

    if(NULL == timer)
    {
        LOG_E("[%s] input param is null",__func__);
        return ;
    }
    
    thread =  WF_CONTAINER_OF((wf_os_api_timer_t *)timer, ars_thread_info_st, thread_timer);

    pars = WF_CONTAINER_OF(thread, ars_st, ars_thread);

    if(NULL != pars)
    {
        ars_thread_sema_post(pars);
    }

    wf_os_api_timer_set(&thread->thread_timer, 2000);
}
wf_s32 ars_thread_init(void *ars)
{
    ars_st *pars                = ars;
    nic_info_st *nic_info       = NULL;
    ars_thread_info_st *thread  = NULL;

    if(NULL == pars)
    {
        return WF_RETURN_FAIL;
    }

    nic_info       = pars->nic_info;
    thread  = &pars->ars_thread;
    
    wf_os_api_sema_init(&thread->ars_sema, 0);
    sprintf(thread->ars_thread_name,
                nic_info->virNic ? "ars:vir%d_%d" : "ars:wlan%d_%d",
                nic_info->hif_node_id, nic_info->ndev_id);
    if (NULL ==(thread->ars_thread_tid=wf_os_api_thread_create(thread->ars_thread_tid, thread->ars_thread_name, ars_thread_handle, nic_info)))
    {
        LOG_E("[ars_thread_init] create thread failed");
    }
    else
    {
        wf_os_api_thread_wakeup(thread->ars_thread_tid);
    }

    wf_os_api_timer_reg(&thread->thread_timer, (void *)thread_timer_handler, &thread->thread_timer);
    
    return WF_RETURN_OK;
}
wf_s32 ars_thread_end(void *ars)
{
    ars_st *pars                = ars;
    ars_thread_info_st *thread  = NULL;
    
    if(NULL == pars)
    {
        LOG_E("input parsm is err.");
        return WF_RETURN_FAIL;
    }
    
    thread = &pars->ars_thread;
    wf_os_api_timer_unreg(&thread->thread_timer);
    if(thread->ars_thread_tid)
    {
        ars_thread_sema_post(pars);
        wf_os_api_thread_destory(thread->ars_thread_tid);
        thread->ars_thread_tid = NULL;
    }
    
    return WF_RETURN_OK;
}

#endif

