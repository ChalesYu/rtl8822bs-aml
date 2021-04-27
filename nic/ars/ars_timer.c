#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT

#define ARS_TIMESTATP (2000) //ms

void odm_CommonInfoSelfUpdate(ars_st *pars)
{
    wf_u8 EntryCnt = 0;
    wf_u8 num_active_client = 0;
    wf_u32  i= 0;
    wf_u32 OneEntry_MACID = 0;
    wf_u32 ma_rx_tp = 0;


    if (*(pars->pBandWidth) == ODM_BW40M)
    {
        if (*(pars->pSecChOffset) == 1)
        {      
            pars->ControlChannel = *(pars->pChannel) - 2;
        }
        else if (*(pars->pSecChOffset) == 2)
        {      
            pars->ControlChannel = *(pars->pChannel) + 2;
        }
    }
    else
    {
        pars->ControlChannel = *(pars->pChannel);
    }

    EntryCnt = wf_wdn_get_cnt(pars->nic_info);
    
    if(EntryCnt == 1)
    {
        pars->bOneEntryOnly = wf_true;
        pars->OneEntry_MACID=OneEntry_MACID;
    }
    else
        pars->bOneEntryOnly = wf_false;

    pars->pre_number_linked_client = pars->number_linked_client;
    pars->pre_number_active_client = pars->number_active_client;
    
    pars->number_linked_client = EntryCnt;
    pars->number_active_client = num_active_client;
}

void phydm_BasicDbgMessage(ars_st *pars)
{
    PFALSE_ALARM_STATISTICS FalseAlmCnt = &pars->FalseAlmCnt;
    ars_dig_info_st *dig = &pars->dig;
    wf_u8   legacy_table[12] = {1, 2, 5, 11, 6, 9, 12, 18, 24, 36, 48, 54};
    wf_u8   vht_en = ((pars->RxRate) >= ODM_RATEVHTSS1MCS0) ? 1 : 0;

    if (pars->RxRate <= ODM_RATE11M) 
    {
        LOG_I("[CCK AGC Report] LNA_idx = 0x%x, VGA_idx = 0x%x\n",pars->cck_lna_idx, pars->cck_vga_idx);        
    } 
    else 
    {
        LOG_I("[OFDM AGC Report] { 0x%x, 0x%x, 0x%x, 0x%x }\n",
            pars->ofdm_agc_idx[0], pars->ofdm_agc_idx[1], pars->ofdm_agc_idx[2], pars->ofdm_agc_idx[3]);    
    }

    LOG_I("RSSI: { %d,  %d,  %d,  %d },    RxRate: { %s%s%s%s%d%s}\n",
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

    LOG_I("[CCA Cnt] {CCK, OFDM, Total} = {%d, %d, %d}\n",  
        FalseAlmCnt->Cnt_CCK_CCA, FalseAlmCnt->Cnt_OFDM_CCA, FalseAlmCnt->Cnt_CCA_all);

    LOG_I("[FA Cnt] {CCK, OFDM, Total} = {%d, %d, %d}\n",   
        FalseAlmCnt->Cnt_Cck_fail, FalseAlmCnt->Cnt_Ofdm_fail, FalseAlmCnt->Cnt_all);
    
    LOG_I("[OFDM FA Detail] Parity_Fail = (( %d )), Rate_Illegal = (( %d )), CRC8_fail = (( %d )), Mcs_fail = (( %d )), Fast_Fsync = (( %d )), SB_Search_fail = (( %d ))\n",    
        FalseAlmCnt->Cnt_Parity_Fail, FalseAlmCnt->Cnt_Rate_Illegal, FalseAlmCnt->Cnt_Crc8_fail, FalseAlmCnt->Cnt_Mcs_fail, FalseAlmCnt->Cnt_Fast_Fsync, FalseAlmCnt->Cnt_SB_Search_fail);
    
    LOG_I("bLinked = %d, RSSI_Min = %d, CurrentIGI = 0x%x, bNoisy=%d\n\n",
        pars->bLinked, pars->RSSI_Min, dig->CurIGValue, pars->NoisyDecision);    




}


void phydm_NoisyDetection(ars_st *pars)
{
    wf_u32  Total_FA_Cnt, Total_CCA_Cnt;
    wf_u32  Score = 0, i, Score_Smooth;
    
    Total_CCA_Cnt = pars->FalseAlmCnt.Cnt_CCA_all;
    Total_FA_Cnt  = pars->FalseAlmCnt.Cnt_all;    

#if 0
    if( Total_FA_Cnt*16>=Total_CCA_Cnt*14 )         // 87.5
    
    else if( Total_FA_Cnt*16>=Total_CCA_Cnt*12 )    // 75
    
    else if( Total_FA_Cnt*16>=Total_CCA_Cnt*10 )    // 56.25
    
    else if( Total_FA_Cnt*16>=Total_CCA_Cnt*8 )     // 50

    else if( Total_FA_Cnt*16>=Total_CCA_Cnt*7 )     // 43.75

    else if( Total_FA_Cnt*16>=Total_CCA_Cnt*6 )     // 37.5

    else if( Total_FA_Cnt*16>=Total_CCA_Cnt*5 )     // 31.25%
        
    else if( Total_FA_Cnt*16>=Total_CCA_Cnt*4 )     // 25%

    else if( Total_FA_Cnt*16>=Total_CCA_Cnt*3 )     // 18.75%

    else if( Total_FA_Cnt*16>=Total_CCA_Cnt*2 )     // 12.5%

    else if( Total_FA_Cnt*16>=Total_CCA_Cnt*1 )     // 6.25%
#else
    for(i=0;i<=16;i++)
    {
        if( Total_FA_Cnt*16>=Total_CCA_Cnt*(16-i) )
        {
            Score = 16-i;
            break;
        }
    }
#endif
    // NoisyDecision_Smooth = NoisyDecision_Smooth>>1 + (Score<<3)>>1;
    pars->NoisyDecision_Smooth = (pars->NoisyDecision_Smooth>>1) + (Score<<2);

    // Round the NoisyDecision_Smooth: +"3" comes from (2^3)/2-1
    Score_Smooth = (Total_CCA_Cnt>=300)?((pars->NoisyDecision_Smooth+3)>>3):0;

    pars->NoisyDecision = (Score_Smooth>=3)?1:0;
    
#if 1
    switch(Score_Smooth)
    {
        case 0:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=0%%\n");
            break;
        case 1:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=6.25%%\n");
            break;
        case 2:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=12.5%%\n");
            break;
        case 3:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=18.75%%\n");
            break;
        case 4:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=25%%\n");
            break;
        case 5:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=31.25%%\n");
            break;
        case 6:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=37.5%%\n");
            break;
        case 7:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=43.75%%\n");
            break;
        case 8:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=50%%\n");
            break;
        case 9:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=56.25%%\n");
            break;
        case 10:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=62.5%%\n");
            break;
        case 11:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=68.75%%\n");
            break;
        case 12:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=75%%\n");
            break;
        case 13:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=81.25%%\n");
            break;
        case 14:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=87.5%%\n");
            break;
        case 15:
            LOG_I ("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=93.75%%\n");            
            break;
        case 16:
            LOG_I("[NoisyDetection] Total_FA_Cnt/Total_CCA_Cnt=100%%\n");
            break;
        default:
            LOG_I("[NoisyDetection] Unknown Value!! Need Check!!\n");            
    }
#endif
    LOG_I("[NoisyDetection] Total_CCA_Cnt=%d, Total_FA_Cnt=%d, NoisyDecision_Smooth=%d, Score=%d, Score_Smooth=%d, pars->NoisyDecision=%d\n",
    Total_CCA_Cnt, Total_FA_Cnt, pars->NoisyDecision_Smooth, Score, Score_Smooth, pars->NoisyDecision);
    
}

void odm_UpdatePowerTrainingState(void *ars    )
{
    ars_st *pars  = ars;
    PFALSE_ALARM_STATISTICS     FalseAlmCnt     = &pars->FalseAlmCnt;
    ars_dig_info_st*            pDM_DigTable    = &pars->dig;
    wf_u32                      score = 0;

    if(!(pars->SupportAbility & ODM_BB_PWR_TRAIN))
        return;

    LOG_I("odm_UpdatePowerTrainingState()============>\n");
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
        pars->dbg.dbg_info.NumQryPhyStatusOFDM = 0;
        pars->dbg.dbg_info.NumQryPhyStatusCCK = 0;
        LOG_I("odm_UpdatePowerTrainingState(): ForcePowerTrainingState = %d\n", 
            pars->ForcePowerTrainingState);
        return;
    }
    
    if(!pars->bLinked)
        return;
    
    // First connect
    if((pars->bLinked) && (pDM_DigTable->bMediaConnect_0 == wf_false))
    {
        pars->PT_score = 0;
        pars->bChangeState = wf_true;
        pars->dbg.dbg_info.NumQryPhyStatusOFDM = 0;
        pars->dbg.dbg_info.NumQryPhyStatusCCK = 0;
        LOG_I("odm_UpdatePowerTrainingState(): First Connect\n");
        return;
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
        LOG_I("odm_UpdatePowerTrainingState(): RX_Pkt_Cnt = %d, Cnt_CCA_all = %d\n", 
            RX_Pkt_Cnt, FalseAlmCnt->Cnt_CCA_all);
    }
    LOG_I("odm_UpdatePowerTrainingState(): NumQryPhyStatusOFDM = %d, NumQryPhyStatusCCK = %d\n",
            (wf_u32)(pars->dbg.dbg_info.NumQryPhyStatusOFDM), (wf_u32)(pars->dbg.dbg_info.NumQryPhyStatusCCK));
    LOG_I("odm_UpdatePowerTrainingState(): NHM_cnt_0 = %d, score = %d\n", 
        pars->NHM_cnt_0, score);

    // smoothing
    pars->PT_score = (score << 4) + (pars->PT_score>>1) + (pars->PT_score>>2);
    score = (pars->PT_score + 32) >> 6;
    LOG_I("odm_UpdatePowerTrainingState(): PT_score = %d, score after smoothing = %d\n", 
        pars->PT_score, score);

    // Mode decision
    if(score == 2)
    {
        if(pars->bDisablePowerTraining)
        {
            pars->bChangeState = wf_true;
            pars->bDisablePowerTraining = wf_false;
            LOG_I("odm_UpdatePowerTrainingState(): Change state\n");
        }
        LOG_I("odm_UpdatePowerTrainingState(): Enable Power Training\n");
    }
    else if(score == 0)
    {
        if(!pars->bDisablePowerTraining)
        {
            pars->bChangeState = wf_true;
            pars->bDisablePowerTraining = wf_true;
            LOG_I("odm_UpdatePowerTrainingState(): Change state\n");
        }
        LOG_I("odm_UpdatePowerTrainingState(): Disable Power Training\n");
    }

    pars->dbg.dbg_info.NumQryPhyStatusOFDM = 0;
    pars->dbg.dbg_info.NumQryPhyStatusCCK = 0;
}

void odm_IQCalibrate(ars_st *pars)
{

}

void phydm_rf_watchdog(ars_st *pars)
{
    odm_TXPowerTrackingCheckCE(pars);
    odm_IQCalibrate(pars);
}

/* Justin: According to the current RRSI to adjust Response Frame TX power, 2012/11/05 */
void odm_dtc(ars_st *pars)
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

    u8 i;
    u8 dtc_steps=0;
    u8 sign;
    u8 resp_txagc=0;

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

    LOG_I("%s RSSI_Min:%u, set RESP_TXAGC to %s %u\n", 
        __func__, pars->RSSI_Min, sign?"minus":"plus", dtc_steps);
#endif /* CONFIG_RESP_TXAGC_ADJUST */
}

void odm_CommonInfoSelfReset(ars_st *pars)
{
    pars->dbg.dbg_info.NumQryBeaconPkt = 0;
}

void ars_timer_handle(wf_os_api_timer_t * timer)
{
    ars_st * pars           = NULL;
    nic_info_st *nic_info   = NULL;
    hw_info_st  *hw         = NULL;
    wdn_net_info_st *wdn_net = NULL;
    wf_s32 err = 0;
    wf_u8  value_u8 = 0;
    if(NULL == timer)
    {
        ARS_DBG("input param is null");
        return;
    }
    
    pars= WF_CONTAINER_OF((wf_os_api_timer_t *)timer, ars_st, ars_timer);
    if(NULL == pars)
    {
        ARS_DBG("pars is null");
        return;
    }

    nic_info = pars->nic_info;
    if(NULL == nic_info )
    {
        ARS_DBG("nic_info is null");
        return;
    }

    // concurrent mode---- to add
    
    //mp mode
    hw = nic_info->hw_info;
    if(1 == hw->mp_mode)
    {
        wf_os_api_timer_set(&pars->ars_timer, ARS_TIMESTATP);
        return;
    }

    wdn_net = wf_wdn_find_info(nic_info, wf_wlan_get_cur_bssid(nic_info));
    if(NULL == wdn_net)
    {
        wf_os_api_timer_set(&pars->ars_timer, ARS_TIMESTATP);
        return;
    }

    pars->wdn_net = wdn_net;
    
    //switch counter to RX fifo
    value_u8 = wf_io_read8(nic_info, REG_RXERR_RPT+3,&err);
    if(err)
    {
        LOG_E("wf_io_read8 REG_RXERR_RPT+3 failed");
        wf_os_api_timer_set(&pars->ars_timer, ARS_TIMESTATP);
        return;
    }
    
    wf_io_write8(nic_info, REG_RXERR_RPT+3, value_u8|0xa0);
    pars->dbg.dbg_priv.dbg_rx_fifo_last_overflow = pars->dbg.dbg_priv.dbg_rx_fifo_curr_overflow;
    pars->dbg.dbg_priv.dbg_rx_fifo_curr_overflow = wf_io_read8(nic_info, REG_RXERR_RPT,&err);
    if(err)
    {
        LOG_E("wf_io_read8 REG_RXERR_RPT failed");
        wf_os_api_timer_set(&pars->ars_timer, ARS_TIMESTATP);
        return;
    }
    
    pars->dbg.dbg_priv.dbg_rx_fifo_diff_overflow = pars->dbg.dbg_priv.dbg_rx_fifo_curr_overflow-pars->dbg.dbg_priv.dbg_rx_fifo_last_overflow;
    

    ODM_CmnInfoUpdate(pars , ODM_CMNINFO_LINK, wf_true);
    ODM_CmnInfoUpdate(pars , ODM_CMNINFO_STATION_STATE, wf_true);
    ODM_CmnInfoUpdate(pars, ODM_CMNINFO_BT_ENABLED, wf_false);

    odm_CommonInfoSelfUpdate(pars);
    phydm_BasicDbgMessage(pars);

    odm_FalseAlarmCounterStatistics(pars);
    
    phydm_NoisyDetection(pars);

    odm_RSSIMonitorCheck(pars);

    if(*(pars->pbPowerSaving) == wf_true)
    {
        odm_DIGbyRSSI_LPS(pars);
        {
            ars_dig_info_st*  pDM_DigTable = &pars->dig;
            Phydm_Adaptivity(pars, pDM_DigTable->CurIGValue);
        }
        
        LOG_I ("DMWatchdog in power saving mode\n");
        return;
    }

    Phydm_CheckAdaptivity(pars);
    odm_UpdatePowerTrainingState(pars);
    odm_DIG(pars);
    {
        ars_dig_info_st*  pDM_DigTable = &pars->dig;
        Phydm_Adaptivity(pars, pDM_DigTable->CurIGValue);
    }
    
    odm_CCKPacketDetectionThresh(pars);
    phydm_ra_dynamic_retry_limit(pars);
    phydm_ra_dynamic_retry_count(pars);
    odm_RefreshRateAdaptiveMask(pars);
    //odm_RefreshBasicRateMask(pars);//windows has
    odm_DynamicBBPowerSaving(pars);
    odm_EdcaTurboCheck(pars);
    odm_PathDiversity(pars);
    ODM_CfoTracking(pars);
    odm_DynamicTxPower(pars);
    odm_AntennaDiversity(pars);
    phydm_Beamforming_Watchdog(pars);

    phydm_rf_watchdog(pars);

    odm_dtc(pars);

    odm_CommonInfoSelfReset(pars);

    wf_os_api_timer_set(&pars->ars_timer, ARS_TIMESTATP);
   
}
#endif

