#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT

static void ars_gpio_init(nic_info_st *pnic_info)
{
    wf_u8 tmp1byte = 0;
    wf_u32 err = 0;

    tmp1byte = wf_io_read8(pnic_info, REG_GPIO_MUXCFG,&err);
    if(err)
    {
        LOG_E("REG_GPIO_MUXCFG read error,%d",err);
        return;
    }
    tmp1byte &= (GPIOSEL_GPIO | ~GPIOSEL_ENBT);

    wf_io_write8(pnic_info, REG_GPIO_MUXCFG, tmp1byte);
}
void ars_mp_init( ars_st *pars)
{

    nic_info_st *nic_info = pars->nic_info;
    hw_info_st *hw = nic_info->hw_info;
    // Update information every period
    pars->mp_mode = hw->mp_mode;

}

void ODM_CmnInfoUpdate(ars_st *pars,wf_u32 CmnInfo,wf_u64 Value )
{
    //
    // This init variable may be changed in run time.
    //
    switch  (CmnInfo)
    {
        case ODM_CMNINFO_LINK_IN_PROGRESS:
            pars->bLinkInProcess = (wf_bool)Value;
            break;
        
        case    ODM_CMNINFO_ABILITY:
            pars->SupportAbility = (wf_u32)Value;
            break;

        case    ODM_CMNINFO_RF_TYPE:
            pars->RFType = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_WIFI_DIRECT:
            pars->bWIFI_Direct = (wf_bool)Value;
            break;

        case    ODM_CMNINFO_WIFI_DISPLAY:
            pars->bWIFI_Display = (wf_bool)Value;
            break;

        case    ODM_CMNINFO_LINK:
            pars->bLinked = (wf_bool)Value;
            break;

        case    ODM_CMNINFO_STATION_STATE:
            pars->bsta_state = (wf_bool)Value;
            break;
            
        case    ODM_CMNINFO_RSSI_MIN:
            pars->RSSI_Min= (wf_u8)Value;
            break;

        case    ODM_CMNINFO_DBG_COMP:
            pars->dbg.DebugComponents = Value;
            break;

        case    ODM_CMNINFO_DBG_LEVEL:
            pars->dbg.DebugLevel = (wf_u32)Value;
            break;
        case    ODM_CMNINFO_RA_THRESHOLD_HIGH:
            pars->ra.HighRSSIThresh = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_RA_THRESHOLD_LOW:
            pars->ra.LowRSSIThresh = (wf_u8)Value;
            break;


        case    ODM_CMNINFO_AP_TOTAL_NUM:
            pars->APTotalNum = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_POWER_TRAINING:
            pars->bDisablePowerTraining = (wf_bool)Value;
            break;

/*
        case    ODM_CMNINFO_OP_MODE:
            pars->OPMode = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_WM_MODE:
            pars->WirelessMode = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_BAND:
            pars->BandType = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_SEC_CHNL_OFFSET:
            pars->SecChOffset = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_SEC_MODE:
            pars->Security = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_BW:
            pars->BandWidth = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_CHNL:
            pars->Channel = (wf_u8)Value;
            break;          
*/  
                default:
            //do nothing
            break;
    }
}

void ars_init_all_timers(ars_st *pars)
{
    nic_info_st *nic_info = pars->nic_info;
    local_info_st *local    = nic_info->local_info;

    if(1 == local->antenna_diversity)
    {
        //ODM_AntDivTimers(pDM_Odm,INIT_ANTDIV_TIMMER);
    }

    if(1 == local->beamforming_support)
    {
    //ODM_InitializeTimer(pDM_Odm, &pDM_Odm->BeamformingInfo.BeamformingTimer,
    //  (RT_TIMER_CALL_BACK)Beamforming_SWTimerCallback, NULL, "BeamformingTimer");
    }

    wf_os_api_timer_reg(&pars->ars_timer, (void *)ars_timer_handle, &pars->ars_timer);
}


wf_u8 DummyHubUsbMode = 1;/* USB 2.0 */
void phydm_hook_dummy_member(ars_st *pars)
{
    if (pars->HubUsbMode == NULL)
    {   
        pars->HubUsbMode = &DummyHubUsbMode;
    }
}

static void odm_CommonInfoSelfInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

    //cck setting
    pars->bCckHighPower = (wf_bool) hw_read_bb_reg(pars->nic_info, ODM_REG_CCK_RPT_FORMAT_11N, ODM_BIT_CCK_RPT_FORMAT_11N);
    
    pars->RFPathRxEnable = (wf_u8) hw_read_bb_reg(pars->nic_info, ODM_REG_BB_RX_PATH_11N, ODM_BIT_BB_RX_PATH_11N);

    ars_debug_init(pars);
    ars_mp_init(pars);

    pars->TxRate = 0xFF;

    pars->number_linked_client = 0;
    pars->pre_number_linked_client = 0;
    pars->number_active_client = 0;
    pars->pre_number_active_client = 0;
    phydm_hook_dummy_member(pars);
}

void Phydm_AdaptivityInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}

void phydm_ra_info_init(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}

void odm_RateAdaptiveMaskInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}

void odm_RA_ParaAdjust_init(void*ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}

void ODM_CfoTrackingInit(void* ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}

void ODM_EdcaTurboInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}

void odm_RSSIMonitorInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}

void phydm_rf_init(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}
void odm_TXPowerTrackingInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}
void odm_AntennaDiversityInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}
void odm_AutoChannelSelectInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}
void odm_PathDiversityInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}
void phydm_Beamforming_Init(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}
void odm_DynamicBBPowerSavingInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;

}
void odm_DynamicTxPowerInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return;
    }

    pars = ars;
}


static void ars_init_core(ars_st *pars)
{
    odm_CommonInfoSelfInit(pars);
    ars_dig_init(pars);
    Phydm_NHMCounterStatisticsInit(pars);
    Phydm_AdaptivityInit(pars);
    phydm_ra_info_init(pars);
    odm_RateAdaptiveMaskInit(pars);
    odm_RA_ParaAdjust_init(pars);
    ODM_CfoTrackingInit(pars);
    ODM_EdcaTurboInit(pars);
    odm_RSSIMonitorInit(pars);
    phydm_rf_init(pars);
    odm_TXPowerTrackingInit(pars);
    odm_AntennaDiversityInit(pars);
    odm_AutoChannelSelectInit(pars);
    odm_PathDiversityInit(pars);
    phydm_Beamforming_Init(pars);

    odm_DynamicBBPowerSavingInit(pars);
    odm_DynamicTxPowerInit(pars);
}



static void ars_init_for_netopen(ars_st *pars)
{
    wf_u32 SupportAbility = 0;
    nic_info_st *nic_info = NULL;
    local_info_st *local = NULL;
    hw_info_st    *hw    = NULL;

    if(NULL == pars)
    {
        ARS_DBG("input param is null");
        return;
    }
    
    nic_info = pars->nic_info;
    local   = nic_info->local_info;
    hw      = nic_info->hw_info;

    if(NIC_USB == nic_info->nic_type)
    {
        ars_gpio_init(nic_info);
    }

    
    SupportAbility = 0
                     | ODM_BB_DIG
                     | ODM_BB_RA_MASK
                     | ODM_BB_DYNAMIC_TXPWR
                     | ODM_BB_FA_CNT
                     | ODM_BB_RSSI_MONITOR
                     /*| ODM_BB_CCK_PD
                     | ODM_BB_PWR_SAVE */
                     | ODM_BB_CFO_TRACKING
                     | ODM_MAC_EDCA_TURBO
                     | ODM_RF_TX_PWR_TRACK
                     | ODM_RF_CALIBRATION
                     | ODM_BB_NHM_CNT
                     /*| ODM_BB_PWR_TRAIN */
                     ;

    if(1 == local->adaptivity_en)
    {
        SupportAbility |= ODM_BB_ADAPTIVITY;
    }

    if(1 == local->antenna_diversity)
    {
        SupportAbility |= ODM_BB_ANT_DIV;
    }

    if(1 == hw->mp_mode)
    {
        SupportAbility = 0 | ODM_RF_CALIBRATION | ODM_RF_TX_PWR_TRACK;
    }


    ODM_CmnInfoUpdate(pars, ODM_CMNINFO_ABILITY, SupportAbility);

}


 void ODM_CmnInfoInit(ars_st *pars, ODM_CMNINFO_E   CmnInfo,wf_u32 Value )
{
    //
    // This section is used for init value
    //
    switch  (CmnInfo)
    {
        //
        // Fixed ODM value.
        //
        case    ODM_CMNINFO_ABILITY:
            pars->SupportAbility = (wf_u32)Value;
            break;

        case    ODM_CMNINFO_RF_TYPE:
            pars->RFType = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_MP_TEST_CHIP:
            pars->bIsMPChip= (wf_u8)Value;
            break;
            
        case    ODM_CMNINFO_CUT_VER:
            pars->CutVersion = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_FAB_VER:
            pars->FabVersion = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_RFE_TYPE:
            pars->RFEType = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_RF_ANTENNA_TYPE:
            pars->AntDivType= (wf_u8)Value;
            break;

        case    ODM_CMNINFO_BOARD_TYPE:
            pars->BoardType = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_PACKAGE_TYPE:
            pars->PackageType = (wf_u8)Value;
            break;

        case    ODM_CMNINFO_EXT_LNA:
            pars->ExtLNA = (wf_u8)Value;
            break;

//        case    ODM_CMNINFO_5G_EXT_LNA:
//            pars->ExtLNA5G = (wf_u8)Value;
//            break;

        case    ODM_CMNINFO_EXT_PA:
            pars->ExtPA = (wf_u8)Value;
            break;

//        case    ODM_CMNINFO_5G_EXT_PA:
//            pars->ExtPA5G = (wf_u8)Value;
//            break;

        case    ODM_CMNINFO_GPA:
            pars->TypeGPA = (wf_u32)Value;
            break;
        case    ODM_CMNINFO_APA:
            pars->TypeAPA = (wf_u32)Value;
            break;
        case    ODM_CMNINFO_GLNA:
            pars->TypeGLNA = (wf_u32)Value;
            break;
        case    ODM_CMNINFO_ALNA:
            pars->TypeALNA = (wf_u32)Value;
            break;

        case    ODM_CMNINFO_EXT_TRSW:
            pars->ExtTRSW = (wf_u8)Value;
            break;
        case    ODM_CMNINFO_EXT_LNA_GAIN:
            pars->ExtLNAGain = (wf_u8)Value;
            break;
        case    ODM_CMNINFO_PATCH_ID:
            pars->PatchID = (wf_u8)Value;
            break;
        case    ODM_CMNINFO_BINHCT_TEST:
            pars->bInHctTest = (wf_bool)Value;
            break;
        case    ODM_CMNINFO_BWIFI_TEST:
            pars->bWIFITest = (wf_bool)Value;
            break;  
        case    ODM_CMNINFO_SMART_CONCURRENT:
            pars->bDualMacSmartConcurrent = (wf_u32)Value;
            break;
        case    ODM_CMNINFO_DOMAIN_CODE_2G:
            pars->odm_Regulation2_4G = (wf_u8)Value;
            break;
        case    ODM_CMNINFO_CONFIG_BB_RF:
            pars->ConfigBBRF = (wf_bool)Value;
            break;
        case    ODM_CMNINFO_IQKFWOFFLOAD:
            pars->IQKFWOffload = (wf_u8)Value;
            break;
        //To remove the compiler warning, must add an empty default statement to handle the other values.   
        default:
            //do nothing
            break;  
        
    }

}


void ODM_CmnInfoHook(ars_st *pars,ODM_CMNINFO_E CmnInfo,void *pValue)
{
    //
    // Hook call by reference pointer.
    //
    switch  (CmnInfo)
    {
        //
        // Dynamic call by reference pointer.
        //
        case    ODM_CMNINFO_MAC_PHY_MODE:
            pars->pMacPhyMode = (wf_u8 *)pValue;
            break;
        
        case    ODM_CMNINFO_TX_UNI:
            pars->pNumTxBytesUnicast = (wf_u64 *)pValue;
            break;

        case    ODM_CMNINFO_RX_UNI:
            pars->pNumRxBytesUnicast = (wf_u64 *)pValue;
            break;

        case    ODM_CMNINFO_WM_MODE:
            pars->pWirelessMode = (wf_u8 *)pValue;
            break;

        case    ODM_CMNINFO_BAND:
            pars->pBandType = (wf_u8 *)pValue;
            break;

        case    ODM_CMNINFO_SEC_CHNL_OFFSET:
            pars->pSecChOffset = (wf_u8 *)pValue;
            break;

        case    ODM_CMNINFO_SEC_MODE:
            pars->pSecurity = (wf_u8 *)pValue;
            break;

        case    ODM_CMNINFO_BW:
            pars->pBandWidth = (wf_u8 *)pValue;
            break;

        case    ODM_CMNINFO_CHNL:
            pars->pChannel = (wf_u8 *)pValue;
            break;
        
        case    ODM_CMNINFO_DMSP_GET_VALUE:
            pars->pbGetValueFromOtherMac = (wf_bool *)pValue;
            break;

        case    ODM_CMNINFO_BUDDY_ADAPTOR:
            pars->pBuddyAdapter = (void* *)pValue;
            break;

        case    ODM_CMNINFO_DMSP_IS_MASTER:
            pars->pbMasterOfDMSP = (wf_bool *)pValue;
            break;

        case    ODM_CMNINFO_SCAN:
            pars->pbScanInProcess = (wf_bool *)pValue;
            break;

        case    ODM_CMNINFO_POWER_SAVING:
            pars->pbPowerSaving = (wf_bool *)pValue;
            break;

        case    ODM_CMNINFO_ONE_PATH_CCA:
            pars->pOnePathCCA = (wf_u8 *)pValue;
            break;

        case    ODM_CMNINFO_DRV_STOP:
            pars->pbDriverStopped =  (wf_bool *)pValue;
            break;

        case    ODM_CMNINFO_PNP_IN:
            pars->pbDriverIsGoingToPnpSetPowerSleep =  (wf_bool *)pValue;
            break;

        case    ODM_CMNINFO_INIT_ON:
            pars->pinit_adpt_in_progress =  (wf_bool *)pValue;
            break;

        case    ODM_CMNINFO_ANT_TEST:
            pars->pAntennaTest =  (wf_u8 *)pValue;
            break;

        case    ODM_CMNINFO_NET_CLOSED:
            pars->pbNet_closed = (wf_bool *)pValue;
            break;

        case    ODM_CMNINFO_FORCED_RATE:
            pars->pForcedDataRate = (wf_u16 *)pValue;
            break;

        case  ODM_CMNINFO_FORCED_IGI_LB:
            pars->pu1ForcedIgiLb = (wf_u8 *)pValue;
            break;

        case    ODM_CMNINFO_P2P_LINK:
            pars->dig.bP2PInProcess = (wf_u8 *)pValue;
            break;

        case    ODM_CMNINFO_IS1ANTENNA:
            pars->pIs1Antenna = (wf_bool *)pValue;
            break;
            
        case    ODM_CMNINFO_RFDEFAULTPATH:
            pars->pRFDefaultPath= (wf_u8 *)pValue;
            break;

        case    ODM_CMNINFO_FCS_MODE:
            pars->pIsFcsModeEnable = (wf_bool *)pValue;
            break;
        /*add by YuChen for beamforming PhyDM*/
        case    ODM_CMNINFO_HUBUSBMODE:
            pars->HubUsbMode = (wf_u8 *)pValue;
            break;
        case    ODM_CMNINFO_FWDWRSVDPAGEINPROGRESS:
            pars->pbFwDwRsvdPageInProgress = (wf_bool *)pValue;
            break;
        case    ODM_CMNINFO_TX_TP:
            pars->pCurrentTxTP = (wf_u32 *)pValue;
            break;
        case    ODM_CMNINFO_RX_TP:
            pars->pCurrentRxTP = (wf_u32 *)pValue;
            break;
        case    ODM_CMNINFO_SOUNDING_SEQ:
            pars->pSoundingSeq = (wf_u8 *)pValue;
            break;
        //case  ODM_CMNINFO_RTSTA_AID:
        //  pars->pAidMap =  (u1Byte *)pValue;
        //  break;

        //case  ODM_CMNINFO_BT_COEXIST:
        //  pars->BTCoexist = (wf_bool *)pValue;        

        //case  ODM_CMNINFO_STA_STATUS:
            //pars->pODM_StaInfo[] = (PSTA_INFO_T)pValue;
            //break;

        //case  ODM_CMNINFO_PHY_STATUS:
        //  pars->pPhyInfo = (ODM_PHY_INFO *)pValue;
        //  break;

        //case  ODM_CMNINFO_MAC_STATUS:
        //  pars->pMacInfo = (ODM_MAC_INFO *)pValue;
        //  break;
        //To remove the compiler warning, must add an empty default statement to handle the other values.               
        default:
            //do nothing
            break;

    }

}

 void ODM_CmnInfoPtrArrayHook(ars_st *pars,ODM_CMNINFO_E    CmnInfo,wf_u16 Index,void *pValue)
{
    //
    // Hook call by reference pointer.
    //
    switch  (CmnInfo)
    {
        #if 0
        //
        // Dynamic call by reference pointer.
        //      
        case    ODM_CMNINFO_STA_STATUS:
            pDM_Odm->pODM_StaInfo[Index] = (PSTA_INFO_T)pValue;
            
            if (IS_STA_VALID(pDM_Odm->pODM_StaInfo[Index]))
            #if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
                pDM_Odm->platform2phydm_macid_table[((PSTA_INFO_T)pValue)->AssociatedMacId] = Index; /*AssociatedMacId are unique bttween different Adapter*/
            #elif (DM_ODM_SUPPORT_TYPE == ODM_AP)
                pDM_Odm->platform2phydm_macid_table[((PSTA_INFO_T)pValue)->aid] = Index;
            #elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
                pDM_Odm->platform2phydm_macid_table[((PSTA_INFO_T)pValue)->mac_id] = Index;
            #endif
            
            break;      
        //To remove the compiler warning, must add an empty default statement to handle the other values.               

        #endif
        default:
            //do nothing
            break;

    }
    
}


void Init_ODM_ComInfo(ars_st *pars)
{
    nic_info_st *nic_info = pars->nic_info;
    hw_info_st  *hw       = nic_info->hw_info;
    wdn_net_info_st *wdn  = NULL;
    wf_s32 i = 0;

    ODM_CmnInfoInit(pars, ODM_CMNINFO_INTERFACE, nic_info->nic_type);

    ODM_CmnInfoInit(pars, ODM_CMNINFO_MP_TEST_CHIP, hw->mp_mode);

    ODM_CmnInfoInit(pars, ODM_CMNINFO_PATCH_ID, pars->tmp_save.CustomerID);

    //ODM_CmnInfoInit(pars, ODM_CMNINFO_BWIFI_TEST, adapter->registrypriv.wifi_spec);


    ODM_CmnInfoInit(pars, ODM_CMNINFO_RF_TYPE, ODM_1T1R);

    {
        //1 ======= BoardType: ODM_CMNINFO_BOARD_TYPE =======
        wf_u8 odm_board_type = ODM_BOARD_DEFAULT;

        if (pars->tmp_save.ExternalLNA_2G != 0) 
        {
            odm_board_type |= ODM_BOARD_EXT_LNA;
            ODM_CmnInfoInit(pars, ODM_CMNINFO_EXT_LNA, 1);
        }
        if (pars->tmp_save.ExternalPA_2G != 0) 
        {
            odm_board_type |= ODM_BOARD_EXT_PA;
            ODM_CmnInfoInit(pars, ODM_CMNINFO_EXT_PA, 1);
        }

        ODM_CmnInfoInit(pars, ODM_CMNINFO_BOARD_TYPE, odm_board_type);
        //1 ============== End of BoardType ==============
    }

    ODM_CmnInfoInit(pars, ODM_CMNINFO_DOMAIN_CODE_2G, hw->Regulation2_4G);

    ODM_CmnInfoInit(pars, ODM_CMNINFO_GPA, pars->tmp_save.TypeGPA);
    ODM_CmnInfoInit(pars, ODM_CMNINFO_APA, pars->tmp_save.TypeAPA);
    ODM_CmnInfoInit(pars, ODM_CMNINFO_GLNA, pars->tmp_save.TypeGLNA);
    ODM_CmnInfoInit(pars, ODM_CMNINFO_ALNA, pars->tmp_save.TypeALNA);
    ODM_CmnInfoInit(pars, ODM_CMNINFO_RFE_TYPE, pars->tmp_save.RFEType);
    ODM_CmnInfoInit(pars, ODM_CMNINFO_EXT_TRSW, 0);

    /* Pointer reference */
    #if 0
    wdn = wf_wdn_find_info(nic_info, wf_wlan_get_cur_bssid(nic_info));
    if(wdn)
    {
        wf_s32 i = 0;
        mlme_info_t *mlme = nic_info->mlme_info;
        ODM_CmnInfoHook(pars, ODM_CMNINFO_TX_UNI, &(wdn->wdn_stats.tx_bytes));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_RX_UNI, &(wdn->wdn_stats.rx_bytes));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_TX_TP, &(wdn->wdn_stats.cur_tx_tp));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_RX_TP, &(wdn->wdn_stats.cur_rx_tp));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_WM_MODE, &wdn->network_type);
        //ODM_CmnInfoHook(pars, ODM_CMNINFO_BAND, &(wdn->bw_mode)); //ODM_BAND_2_4G
        ODM_CmnInfoHook(pars, ODM_CMNINFO_FORCED_RATE, &(wdn->datarate));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_SEC_CHNL_OFFSET, &(wdn->channle_offset));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_BW, &(wdn->bw_mode));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_SEC_MODE, &(wdn->auth_algo));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_CHNL, &( wdn->channel));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_NET_CLOSED, &(nic_info->is_up));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_FORCED_IGI_LB, &(pars->tmp_save.u1ForcedIgiLb));

        ODM_CmnInfoHook(pars, ODM_CMNINFO_SCAN, &(pars->tmp_save.bScanInProcess));
        ODM_CmnInfoHook(pars, ODM_CMNINFO_POWER_SAVING, &(pars->tmp_save.bpower_saving));
        
        if(NIC_USB == nic_info->nic_type)
        {
            ODM_CmnInfoHook(pars, ODM_CMNINFO_HUBUSBMODE, &(pars->tmp_save.usb_speed));
        }

        for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
        {
            ODM_CmnInfoPtrArrayHook(pars, ODM_CMNINFO_STA_STATUS, i, NULL);
        }
    }
    #endif


}

static void Init_ODM_ComInfo_8188f(ars_st *pars)
{
    nic_info_st *nic_info   = NULL;
    hw_info_st  *hw         = NULL;
    local_info_st *local    = NULL;
    wf_u32 SupportAbility = 0;
    wf_u8  cut_ver, fab_ver;

    nic_info = pars->nic_info;
    if(NULL == nic_info)
    {
        LOG_E("[%s] nic_info is null",__func__);
        return;
    }
    
    hw       = nic_info->hw_info;
    if(NULL == hw)
    {
        LOG_E("[%s] hw is null",__func__);
        return;
    }
    
    local    = nic_info->local_info;
    if(NULL == local)
    {
        LOG_E("[%s] local is null",__func__);
        return;
    }
    
    Init_ODM_ComInfo(pars);

    //ODM_CmnInfoInit(pars, ODM_CMNINFO_PACKAGE_TYPE, pHalData->PackageType);
    //ODM_CmnInfoInit(pars, ODM_CMNINFO_IC_TYPE, ODM_RTL8188F);

    //fab_ver = ODM_TSMC;
    //cut_ver = GET_CVID_CUT_VERSION(pHalData->VersionID);

    //ODM_CmnInfoInit(pars, ODM_CMNINFO_FAB_VER, fab_ver);
    //ODM_CmnInfoInit(pars, ODM_CMNINFO_CUT_VER, cut_ver);

    if( 0 == hw->use_drv_odm)
    {
        SupportAbility = 0;
    }
    else
    {
        SupportAbility = ODM_RF_CALIBRATION | ODM_RF_TX_PWR_TRACK;
    }

    ODM_CmnInfoUpdate(pars, ODM_CMNINFO_ABILITY, SupportAbility);
}

wf_s32 ars_init(nic_info_st *pnic_info)
{
    ars_st *pars = NULL;
    LOG_I("ars_init start");

    if(NULL != pnic_info->ars)
    {
        LOG_D("nic_info->ars is not null");
        return 0;
    }
    
    pars = wf_kzalloc(sizeof(ars_st));
    if (pars == NULL)
    {
        LOG_D("malloc p2p_info_st failed");
        return WF_RETURN_FAIL;
    }

    pnic_info->ars = pars;
    pars->nic_info = pnic_info;
    phy_InitBBRFRegisterDefinition(pars);
    Init_ODM_ComInfo_8188f(pars);
    ars_init_all_timers(pars);
    return 0;
}
wf_s32 ars_term(nic_info_st *pnic_info)
{
    ars_st *pars = NULL;
    
    LOG_I("ars_term start");
    pars = pnic_info->ars;
    if(NULL == pars)
    {
        return 0;
    }
    
    wf_os_api_timer_unreg(&pars->ars_timer);
    wf_kfree(pars);
    pars = NULL;

    return 0;
}

#endif
