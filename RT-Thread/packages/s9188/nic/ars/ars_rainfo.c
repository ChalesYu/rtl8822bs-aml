#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_DRIVER_SUPPORT

#define HT_MCS_SUPPORT_MCS_LIMIT (8)

#if 0
#define ARS_RA_DBG(fmt, ...)      LOG_D("ARS_RA[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_RA_PRT(fmt, ...)      LOG_D("ARS_RA-"fmt,##__VA_ARGS__)

#else
#define ARS_RA_DBG(fmt, ...)
#define ARS_RA_PRT(fmt, ...)
#endif
#define ARS_RA_INFO(fmt, ...)      LOG_I("ARS_RA-"fmt,##__VA_ARGS__)
#define ARS_RA_ERR(fmt, ...)      LOG_E("ARS_RA-"fmt,##__VA_ARGS__)


#define TEST_FLAG(__Flag,__testFlag)        (((__Flag) & (__testFlag)) != 0)
#define STBC_HT_ENABLE_RX           BIT(0)
#define STBC_HT_ENABLE_TX           BIT(1)
#define STBC_HT_TEST_TX_ENABLE      BIT(2)
#define STBC_HT_CAP_TX              BIT(3)

static wf_s32 wf_wdn_update_traffic_stat(nic_info_st *nic_info,wdn_net_info_st *wdn_info)
{
    tx_info_st *tx_info         = NULL;
    rx_info_t * rx_info         = NULL;

    
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

#if 1
    ARS_RA_INFO("tx_bytes:%lld,rx_bytes:%lld,cur_tx_bytes:%lld,cur_rx_bytes:%lld",
             wdn_info->wdn_stats.tx_bytes, wdn_info->wdn_stats.rx_pkts,
             wdn_info->wdn_stats.cur_tx_bytes, wdn_info->wdn_stats.cur_rx_bytes);
#endif

    return 0;
}



/*H2C_RSSI_REPORT*/
wf_s8 phydm_rssi_report(void *ars, wdn_net_info_st *pwdn)
{
    ars_st *pars = ars;
    
    wf_u8 H2C_Parameter[4] = {0};
    wf_u8 UL_DL_STATE = 0;
    wf_u8 STBC_TX = 0;
    wf_u8 TxBF_EN = 0;
    wf_u8 cmdlen = 4;
    wf_u8 first_connect = 0;
    wf_u64  curTxOkCnt = 0;
    wf_u64 curRxOkCnt = 0;
    RSSI_STA *rssi_sta = NULL;

    if(NULL == pars || NULL == pwdn)
    {
        LOG_E("[%s] input param is null",__func__);
        return WF_RETURN_FAIL;
    }

    wf_wdn_update_traffic_stat(pars->nic_info,pwdn);
    rssi_sta = &pars->rssi_sta[pwdn->wdn_id];
    
    if (rssi_sta->UndecoratedSmoothedPWDB == (-1)) 
    {
        LOG_I("%s mac_id:%u, mac:"WF_MAC_FMT", rssi == -1\n", __func__,pwdn->wdn_id, WF_MAC_ARG(pwdn->mac));
        return WF_RETURN_FAIL;
    }

    curTxOkCnt = pwdn->wdn_stats.cur_tx_bytes;
    curRxOkCnt = pwdn->wdn_stats.cur_rx_bytes;
    if (curRxOkCnt > (curTxOkCnt * 6))
    {   
        UL_DL_STATE = 1;
    }
    else
    {   
        UL_DL_STATE = 0;
    }
    
    #ifdef CONFIG_BEAMFORMING
    {
        #if (BEAMFORMING_SUPPORT == 1)
        BEAMFORMING_CAP Beamform_cap = phydm_Beamforming_GetEntryBeamCapByMacId(pDM_Odm, pEntry->mac_id);
        #else/*for drv beamforming*/
        BEAMFORMING_CAP Beamform_cap = beamforming_get_entry_beam_cap_by_mac_id(&Adapter->mlmepriv, pEntry->mac_id);
        #endif

        if (Beamform_cap & (BEAMFORMER_CAP_HT_EXPLICIT | BEAMFORMER_CAP_VHT_SU))
            TxBF_EN = 1;
        else
            TxBF_EN = 0;
    }
    #endif /*#ifdef CONFIG_BEAMFORMING*/
        
    if (TxBF_EN)
    {   
        STBC_TX = 0;
    }
    else 
    {
        
        //STBC_TX = TEST_FLAG(pwdn->htpriv.stbc_cap, STBC_HT_ENABLE_TX);

    }
        
    H2C_Parameter[0] = (wf_u8)(pwdn->wdn_id & 0xFF);
    H2C_Parameter[2] = rssi_sta->UndecoratedSmoothedPWDB & 0x7F;
        
    if (UL_DL_STATE)
    {
        H2C_Parameter[3] |= RAINFO_BE_RX_STATE;
    }
        
    if (TxBF_EN)
    {
        H2C_Parameter[3] |= RAINFO_BF_STATE;
    }
    if (STBC_TX)
    {
        H2C_Parameter[3] |= RAINFO_STBC_STATE;
    }
    if (pars->NoisyDecision)
    {
        H2C_Parameter[3] |= RAINFO_NOISY_STATE;
    }
        
    if (pars->ra.ra_rpt_linked == wf_false) 
    {
        H2C_Parameter[3] |= RAINFO_INIT_RSSI_RATE_STATE;
        pars->ra.ra_rpt_linked = wf_true;
        first_connect = wf_true;
    }
        
    #if 0
    if (first_connect) 
    {
        DBG_871X("%s mac_id:%u, mac:"MAC_FMT", rssi:%d\n", __func__,
            pEntry->mac_id, MAC_ARG(pEntry->hwaddr), pEntry->rssi_stat.UndecoratedSmoothedPWDB);
            
        DBG_871X("%s RAINFO - TP:%s, TxBF:%s, STBC:%s, Noisy:%s, Firstcont:%s\n", __func__,
            (UL_DL_STATE) ? "DL" : "UL", (TxBF_EN) ? "EN" : "DIS", (STBC_TX) ? "EN" : "DIS",
            (pDM_Odm->NoisyDecision) ? "True" : "False", (first_connect) ? "True" : "False");
    }
    #endif
        
    if (pars->fw_ractrl == wf_true) 
    {
        FillH2CCmd(pars->nic_info, ODM_H2C_RSSI_REPORT, cmdlen, H2C_Parameter);
    } 
    
    return WF_RETURN_OK;
}

wf_s32 FindMinimumRSSI(void *ars)
{
    ars_st *pars = ars;
    /*Determine the minimum RSSI*/

    if ((pars->bLinked != wf_true) &&(pars->com_save.EntryMinUndecoratedSmoothedPWDB == 0)) 
    {
        pars->com_save.MinUndecoratedPWDBForDM = 0;
        /*ODM_RT_TRACE(pDM_Odm,COMP_BB_POWERSAVING, DBG_LOUD, ("Not connected to any\n"));*/
    } 
    else
    {
        pars->com_save.MinUndecoratedPWDBForDM = pars->com_save.EntryMinUndecoratedSmoothedPWDB;
    }

    return WF_RETURN_OK;
}


wf_s32 odm_RSSIMonitorCheckCE(void *ars)
{
    ars_st*     pars        = ars;
    RSSI_STA *rssi_sta      = NULL;
    wf_s32 i                = 0;
    wf_s32 tmpEntryMaxPWDB  = 0;
    wf_s32 tmpEntryMinPWDB  = 0xff;
    wf_u8  sta_cnt          = 0;
    wdn_net_info_st *pwdn   = NULL;

    if(NULL == pars)
    {
        LOG_I("[%s] input param is null",__func__);
        return WF_RETURN_FAIL;
    }
    
    if (pars->bLinked != wf_true)
    {
        return WF_RETURN_OK; 
    }

    for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) 
    {
        rssi_sta = &pars->rssi_sta[i];
        pwdn     =  wf_wdn_find_info_by_id(pars->nic_info, i);
        if(NULL != pwdn)
        {
            if (rssi_sta->UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
            {
                tmpEntryMinPWDB = rssi_sta->UndecoratedSmoothedPWDB;
            }

            if (rssi_sta->UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
            {
                tmpEntryMaxPWDB = rssi_sta->UndecoratedSmoothedPWDB;
            }

            if ( WF_RETURN_OK == phydm_rssi_report(pars, pwdn))
            {
                sta_cnt++;
            }
        }
    }
    ARS_RA_PRT("%s==> sta_cnt(%d)\n", __func__, sta_cnt);

    if (tmpEntryMaxPWDB != 0)   // If associated entry is found
    {
        pars->com_save.EntryMaxUndecoratedSmoothedPWDB = tmpEntryMaxPWDB;
    }
    else
    {
        pars->com_save.EntryMaxUndecoratedSmoothedPWDB = 0;
    }

    if (tmpEntryMinPWDB != 0xff) // If associated entry is found
    {
        pars->com_save.EntryMinUndecoratedSmoothedPWDB = tmpEntryMinPWDB;
    }
    else
    {
        pars->com_save.EntryMinUndecoratedSmoothedPWDB = 0;
    }

    //LOG_D("[%s,%d] EntryMinUndecoratedSmoothedPWDB:%d",__func__,__LINE__,pars->com_save.EntryMinUndecoratedSmoothedPWDB);
    FindMinimumRSSI(pars);

    pars->RSSI_Min = pars->com_save.MinUndecoratedPWDBForDM;
    //LOG_I("[%s] RSSI_Min:%d",__func__,pars->RSSI_Min);
    return WF_RETURN_OK;
}


wf_s32 odm_RSSIMonitorCheck(void *ars)
{
    ars_st*     pars = ars;
    if (!(pars->SupportAbility & ODM_BB_RSSI_MONITOR))
    {
        return WF_RETURN_OK;
    }
    
    odm_RSSIMonitorCheckCE(pars);

    return WF_RETURN_OK;
}

wf_s32 phydm_ra_dynamic_retry_count(void *ars)
{
    ars_st* pars = ars;
    ARS_RA_DBG("pars->pre_b_noisy = %d\n", pars->pre_b_noisy );
    if (pars->pre_b_noisy != pars->NoisyDecision) 
    {

        ars_io_lock_try(pars);
        if (pars->NoisyDecision) 
        {
            ARS_RA_DBG("->Noisy Env. RA fallback value\n");
            hw_write_bb_reg(pars->nic_info, 0x430, bMaskDWord, 0x0);
            hw_write_bb_reg(pars->nic_info, 0x434, bMaskDWord, 0x04030201);       
        } 
        else 
        {
            ARS_RA_DBG("->Clean Env. RA fallback value\n");
            hw_write_bb_reg(pars->nic_info, 0x430, bMaskDWord, 0x02010000);
            hw_write_bb_reg(pars->nic_info, 0x434, bMaskDWord, 0x06050403);
        }
        pars->pre_b_noisy = pars->NoisyDecision;
        ars_io_unlock_try(pars);
    }

    return WF_RETURN_OK;
}

wf_bool ODM_RAStateCheck(void *ars,wf_s32 RSSI, wf_bool bForceUpdate, wf_u8* pRATRState)
{
    ars_st *pars                = ars;
    ars_ra_info_st *pRA         = &pars->ra;
    const wf_u8 GoUpGap         = 5;
    wf_u8 HighRSSIThreshForRA   = pRA->HighRSSIThresh;
    wf_u8 LowRSSIThreshForRA    = pRA->LowRSSIThresh;
    wf_u8 RATRState;
    
    ARS_RA_DBG("RSSI= (( %d )), Current_RSSI_level = (( %d ))\n", RSSI, *pRATRState);
    ARS_RA_DBG("[Ori RA RSSI Thresh]  High= (( %d )), Low = (( %d ))\n", HighRSSIThreshForRA, LowRSSIThreshForRA);
    // Threshold Adjustment:
    // when RSSI state trends to go up one or two levels, make sure RSSI is high enough.
    // Here GoUpGap is added to solve the boundary's level alternation issue.

    switch (*pRATRState) 
    {
        case DM_RATR_STA_INIT:
        case DM_RATR_STA_HIGH:
            break;

        case DM_RATR_STA_MIDDLE:
            HighRSSIThreshForRA += GoUpGap;
            break;

        case DM_RATR_STA_LOW:
            HighRSSIThreshForRA += GoUpGap;
            LowRSSIThreshForRA += GoUpGap;
            break;

        default:
            ARS_RA_DBG("wrong rssi level setting %d !", *pRATRState);
            break;
    }

    // Decide RATRState by RSSI.
    if (RSSI > HighRSSIThreshForRA)
    {   
        RATRState = DM_RATR_STA_HIGH;
    }
    else if (RSSI > LowRSSIThreshForRA)
    {   
        RATRState = DM_RATR_STA_MIDDLE;
    }
    else
    {   
        RATRState = DM_RATR_STA_LOW;
    }
    ARS_RA_DBG("[Mod RA RSSI Thresh]  High= (( %d )), Low = (( %d ))\n", HighRSSIThreshForRA, LowRSSIThreshForRA);
    
    if (*pRATRState != RATRState || bForceUpdate) 
    {
        ARS_RA_DBG("[RSSI Level Update] %d -> %d\n", *pRATRState, RATRState);
        *pRATRState = RATRState;
        return wf_true;
    }

    return wf_false;
}

wf_u8 query_ra_short_GI(wdn_net_info_st *pwdn)
{
    wf_u8   sgi     = wf_false;
    wf_u8   sgi_20m = wf_false;
    wf_u8   sgi_40m = wf_false;


    sgi_20m = pwdn->htpriv.sgi_20m;
    sgi_40m = pwdn->htpriv.sgi_40m;

    switch(pwdn->bw_mode)
    {
        case CHANNEL_WIDTH_40:
            sgi = sgi_40m;
            break;
        case CHANNEL_WIDTH_20:
        default:
            sgi = sgi_20m;
            break;
    }

    return sgi;
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

static wf_u32 get_ra_mask(wdn_net_info_st *pwdn)
{
    wf_u32 ra_mask = 0;
    int i = 0;
    
    /*calc ra_mask*/
    for (i = 0; i < pwdn->datarate_len; i++)
    {
        if (pwdn->datarate[i])
            ra_mask |= bit_value_from_ieee_value_to_get_func(pwdn->datarate[i] & 0x7f, 1);
    }

    for (i = 0; i < pwdn->ext_datarate_len; i++)
    {
        if (pwdn->ext_datarate[i])
            ra_mask |= bit_value_from_ieee_value_to_get_func(pwdn->ext_datarate[i] & 0x7f, 1);
    }

    for (i = 0; i < HT_MCS_SUPPORT_MCS_LIMIT; i++)
    {
        if (pwdn->htpriv.ht_cap.supp_mcs_set[i / 8] & WF_BIT(i % 8))
        {
            ra_mask |= WF_BIT(i + 12);
        }
    }

    return ra_mask;
}

wf_u32 ODM_Get_Rate_Bitmap( wdn_net_info_st *pwdn,        wf_u32 ra_mask,wf_u8 rssi_level)
{
    wf_u32  rate_bitmap = 0;
    wf_u32  WirelessMode;
    //u1Byte    WirelessMode =*(pDM_Odm->pWirelessMode);
    WirelessMode = pwdn->network_type;

    switch (WirelessMode) 
    {
    case ODM_WM_B:
        if (ra_mask & 0x0000000c)       //11M or 5.5M enable
            rate_bitmap = 0x0000000d;
        else
            rate_bitmap = 0x0000000f;
        break;

    case (ODM_WM_G):
    case (ODM_WM_A):
        if (rssi_level == DM_RATR_STA_HIGH)
            rate_bitmap = 0x00000f00;
        else
            rate_bitmap = 0x00000ff0;
        break;

    case (ODM_WM_B|ODM_WM_G):
        if (rssi_level == DM_RATR_STA_HIGH)
            rate_bitmap = 0x00000f00;
        else if (rssi_level == DM_RATR_STA_MIDDLE)
            rate_bitmap = 0x00000ff0;
        else
            rate_bitmap = 0x00000ff5;
        break;

    case (ODM_WM_B|ODM_WM_G|ODM_WM_N24G)    :
    case (ODM_WM_B|ODM_WM_N24G) :
    case (ODM_WM_G|ODM_WM_N24G) :
    {
            if (rssi_level == DM_RATR_STA_HIGH)
                rate_bitmap = 0x000f0000;
            else if (rssi_level == DM_RATR_STA_MIDDLE)
                rate_bitmap = 0x000ff000;
            else 
            {
                if (pwdn->bw_mode == ODM_BW40M)
                    rate_bitmap = 0x000ff015;
                else
                    rate_bitmap = 0x000ff005;
            }
        
    }
    break;

    case (ODM_WM_AC|ODM_WM_G):
        if (rssi_level == 1)
            rate_bitmap = 0xfc3f0000;
        else if (rssi_level == 2)
            rate_bitmap = 0xfffff000;
        else
            rate_bitmap = 0xffffffff;
        break;

    default:
            rate_bitmap = 0x0fffffff;
        break;

    }

    ARS_RA_DBG("%s ==> rssi_level:0x%02x, WirelessMode:0x%02x, rate_bitmap:0x%08x\n", __func__, rssi_level, WirelessMode, rate_bitmap);
    ARS_RA_DBG(" ==> rssi_level:0x%02x, WirelessMode:0x%02x, rate_bitmap:0x%08x\n", rssi_level, WirelessMode, rate_bitmap);

    return (ra_mask & rate_bitmap);

}

wf_s32 MacIdConfig(nic_info_st *nic_info, wf_u8 mac_id, wf_u8 raid, wf_u8 bw, wf_u8 sgi, wf_u32 mask)
{
    wf_u8 u1wMBOX1MacIdConfigParm[wMBOX1_MACID_CFG_LEN] = { 0 };
    int ret = 0;
    mcu_msg_body_st mcu_msg;
    
    LOG_I("[%s] mac_id:%d, raid:%d, bw:%d, sgi:%d,mask:0x%x",__func__, mac_id, raid, bw, sgi,mask);
    SET_9086X_wMBOX1CMD_MACID_CFG_MACID(u1wMBOX1MacIdConfigParm, mac_id);
    SET_9086X_wMBOX1CMD_MACID_CFG_RAID(u1wMBOX1MacIdConfigParm, raid);
    SET_9086X_wMBOX1CMD_MACID_CFG_SGI_EN(u1wMBOX1MacIdConfigParm, (sgi) ? 1 : 0);
    SET_9086X_wMBOX1CMD_MACID_CFG_BW(u1wMBOX1MacIdConfigParm, bw);

    ret = wf_mcu_msg_body_get(nic_info,&mcu_msg);
    if (mcu_msg.bDisablePowerTraining)
    {
        SET_9086X_wMBOX1CMD_MACID_CFG_DISPT(u1wMBOX1MacIdConfigParm, 1);
    }

    SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK0(u1wMBOX1MacIdConfigParm,
                                          (wf_u8) (mask & 0x000000ff));
    SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK1(u1wMBOX1MacIdConfigParm,
                                          (wf_u8) ((mask & 0x0000ff00) >> 8));
    SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK2(u1wMBOX1MacIdConfigParm,
                                          (wf_u8) ((mask & 0x00ff0000) >> 16));
    SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK3(u1wMBOX1MacIdConfigParm,
                                          (wf_u8) ((mask & 0xff000000) >> 24));
    
    ret = FillH2CCmd(nic_info,wMBOX1_9086X_MACID_CFG,wMBOX1_MACID_CFG_LEN,u1wMBOX1MacIdConfigParm);
    
    return ret;
}

wf_s32 UpdateHalRAMask8188F(nic_info_st *nic_info, wdn_net_info_st *pwdn, u8 rssi_level)
{
    wf_u32  mask            = 0;
    wf_u32 rate_bitmap      = 0;
    wf_u8   shortGIrate     = wf_false;
    ars_st *pars            = nic_info->ars;
    
    shortGIrate = query_ra_short_GI(pwdn);
    mask = get_ra_mask(pwdn);
    rate_bitmap = 0xffffffff;
    rate_bitmap = ODM_Get_Rate_Bitmap(pwdn, mask, rssi_level);
    ARS_RA_DBG("%s => mac_id:%d, networkType:0x%02x, mask:0x%08x\n\t ==> rssi_level:%d, rate_bitmap:0x%08x\n",
             __func__, pwdn->wdn_id, pwdn->network_type, mask, rssi_level, rate_bitmap);

    mask &= rate_bitmap;

    if (pars->fw_ractrl == wf_true)
    {   
        MacIdConfig(nic_info, pwdn->wdn_id, pwdn->raid, pwdn->bw_mode, shortGIrate, mask);
    }
    
    /*set correct initial date rate for each mac_id */
    //pHalData->INIDATA_RATE[mac_id] = psta->init_rate;
    //ARS_RA_DBG("%s(): mac_id=%d raid=0x%x bw=%d mask=0x%x init_rate=0x%x\n", __func__, pwdn->wdn_id, pwdn->raid, pwdn->bw_mode, mask, pwdn->init_rate);
    return WF_RETURN_OK;
}


wf_s32 rtw_hal_update_ra_mask(wdn_net_info_st *pwdn, ars_st * pars)
{
    RSSI_STA *rssi_sta = NULL;
    nic_info_st *nic_info = NULL;
    
    if(NULL == pwdn || NULL == pars)
    {
        return -1;
    }
    
    rssi_sta = &pars->rssi_sta[pwdn->wdn_id];
    if(NULL == rssi_sta)
    {   
        return -2;
    }

    nic_info = pars->nic_info;
    if(NULL == nic_info)
    {
        return -3;
    }
    if(nic_info->nic_state & WIFI_AP_STATE)
    {
        #if 0
        add_RATid(padapter, psta, rssi_level);
        #else
        //need to do
        #endif
    }
    else
    {
        UpdateHalRAMask8188F(nic_info, pwdn, rssi_sta->rssi_level);
    }

    return 0;
}

wf_s32 odm_RefreshRateAdaptiveMaskCE(void *ars)
{
    ars_st * pars = ars;
    RSSI_STA *rssi_sta      = NULL;
    wdn_net_info_st *pwdn   = NULL;
    wf_u8  i;

    if (!pars->ra.bUseRAMask) 
    {
        ARS_RA_DBG("<---- odm_RefreshRateAdaptiveMask(): driver does not control rate adaptive mask\n");
        return WF_RETURN_FAIL;
    }

    for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) 
    {
        rssi_sta = &pars->rssi_sta[i];
        pwdn     =  wf_wdn_find_info_by_id(pars->nic_info, i);
        if(NULL != pwdn)
        {
            if (wf_true == ODM_RAStateCheck(pars, rssi_sta->UndecoratedSmoothedPWDB, wf_false , &rssi_sta->rssi_level)) 
            {
                ARS_RA_DBG("RSSI:%d, RSSI_LEVEL:%d\n", rssi_sta->UndecoratedSmoothedPWDB, rssi_sta->rssi_level);
                rtw_hal_update_ra_mask(pwdn, pars);
            } 
            else if (pars->bChangeState) 
            {
                ARS_RA_DBG("Change Power Training State, bDisablePowerTraining = %d\n", pars->bDisablePowerTraining);
                rtw_hal_update_ra_mask(pwdn, pars);
            }
        }
    }

    return WF_RETURN_OK;
}

wf_s32 odm_RefreshRateAdaptiveMask(void *ars)
{
    ars_st*      pars = ars;
    ARS_RA_DBG("odm_RefreshRateAdaptiveMask()---------->\n");
    if (!(pars->SupportAbility & ODM_BB_RA_MASK)) 
    {
        ARS_RA_DBG("odm_RefreshRateAdaptiveMask(): Return cos not supported\n");
        return WF_RETURN_OK;
    }
    //
    // 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
    // at the same time. In the stage2/3, we need to prive universal interface and merge all
    // HW dynamic mechanism.
    //
    
    odm_RefreshRateAdaptiveMaskCE(pars);

    return WF_RETURN_OK;
}



#if (defined(CONFIG_RA_DYNAMIC_RTY_LIMIT))

wf_s32 phydm_ra_dynamic_retry_limit(void *ars)
{
    ars_st *pars = ars;
    ars_ra_info_st*         pRA_Table = &pars->ra;
    PSTA_INFO_T     pEntry;
    wf_u8   i, retry_offset;
    wf_u32  ma_rx_tp;


    if (pars->pre_number_active_client == pars->number_active_client) 
    {
        
        ARS_RA_DBG" pre_number_active_client ==  number_active_client\n");
        return;
        
    } 
    else 
    {
        if (pars->number_active_client == 1) 
        {
            phydm_reset_retry_limit_table(pars);
            ARS_RA_DBG"one client only->reset to default value\n");
        } 
        else 
        {

            retry_offset = pars->number_active_client * pRA_Table->retry_descend_num;
            
            for (i = 0; i < ODM_NUM_RATE_IDX; i++) 
            {

                phydm_retry_limit_table_bound(pars, &(pRA_Table->per_rate_retrylimit_20M[i]), retry_offset);
                phydm_retry_limit_table_bound(pars, &(pRA_Table->per_rate_retrylimit_40M[i]), retry_offset);    
            }               
        }
    }

    return WF_RETURN_OK;
}
#else
wf_s32 phydm_ra_dynamic_retry_limit(void *pars)
{
    return WF_RETURN_OK;
}

#endif


wf_s32 phydm_ra_info_init(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_RA_DBG("input param is null");
        return WF_RETURN_FAIL;
    }

    pars = ars;

    #if (defined(CONFIG_RA_DYNAMIC_RTY_LIMIT))
    phydm_ra_dynamic_retry_limit_init(pars);
    #endif

    return WF_RETURN_OK;
}

wf_s32 odm_RateAdaptiveMaskInit(void *ars)
{
    ars_st *pars = NULL;
    ars_ra_info_st *ra = NULL;
    
    if(NULL == ars)
    {
        ARS_RA_DBG("input param is null");
        return WF_RETURN_FAIL;
    }

    pars = ars;
    ra = &pars->ra;
    ra->Type = DM_Type_ByDriver;
    if (ra->Type == DM_Type_ByDriver)
    {   
        ra->bUseRAMask = wf_true;
    }
    else
    {
        ra->bUseRAMask = wf_false;
    }

    ra->RATRState = DM_RATR_STA_INIT;


    ra->LdpcThres = 35;
    ra->bUseLdpc = wf_false;


    ra->HighRSSIThresh = 50;

    ra->LowRSSIThresh = 20;

    return WF_RETURN_OK;
}

wf_s32 odm_RA_ParaAdjust_init(void*ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_RA_DBG("input param is null");
        return WF_RETURN_FAIL;
    }

    pars = ars;

    return WF_RETURN_OK;
}

wf_s32 odm_RSSIMonitorInit(void *ars)
{
    ars_st *pars = NULL;
    ars_ra_info_st *ra = NULL;
    if(NULL == ars)
    {
        ARS_RA_DBG("input param is null");
        return WF_RETURN_FAIL;
    }

    ARS_RA_INFO("start");
    pars = ars;

    ra          = &pars->ra;
    ra->ra_table.firstconnect   = wf_false;

    return WF_RETURN_OK;
}


#endif

