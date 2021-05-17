#include "common.h"
#include "wf_debug.h"

#ifdef CONFIG_ARS_SUPPORT
#if 0
#define ARS_ENTRY_DBG(fmt, ...)      LOG_D("ARS_ENTRY[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_ENTRY_PRT(fmt, ...)      LOG_D("ARS_ENTRY-"fmt,##__VA_ARGS__)

#else
#define ARS_ENTRY_DBG(fmt, ...)
#define ARS_ENTRY_PRT(fmt, ...) 
#endif

#define ARS_ENTRY_INFO(fmt, ...)      LOG_I("ARS_ENTRY-"fmt,##__VA_ARGS__)
#define ARS_ENTRY_ERR(fmt, ...)      LOG_E("ARS_ENTRY-"fmt,##__VA_ARGS__)

static wf_s32 ars_gpio_init(nic_info_st *pnic_info)
{
    wf_u8 tmp1byte  = 0;
    wf_u32 err      = 0;
    wf_s32 ret      = 0;

    tmp1byte = wf_io_read8(pnic_info, REG_GPIO_MUXCFG,&err);
    if(err)
    {
        ARS_ENTRY_ERR("REG_GPIO_MUXCFG read error,%d",err);
        return WF_RETURN_FAIL;
    }
    tmp1byte &= (GPIOSEL_GPIO | ~GPIOSEL_ENBT);

    ret = wf_io_write8(pnic_info, REG_GPIO_MUXCFG, tmp1byte);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] failed",__func__);
        return ret;
    }
    
    return WF_RETURN_OK;
}
wf_s32 ars_mp_init( ars_st *pars)
{
    nic_info_st *nic_info   = NULL;
    hw_info_st *hw          = NULL;
    
    if(NULL == pars)
    {
        ARS_ENTRY_ERR("param is null");
        return WF_RETURN_FAIL;
    }

    nic_info   = pars->nic_info;
    hw          = nic_info->hw_info;
    // Update information every period
    pars->mp_mode = hw->mp_mode;
    
    return WF_RETURN_OK;
}

wf_s32 ars_init_all_timers(ars_st *pars)
{
    nic_info_st *nic_info   = NULL;
    local_info_st *local    = NULL;

    if(NULL == pars)
    {
        ARS_ENTRY_ERR("param is null");
        return WF_RETURN_FAIL;
    }
    
    nic_info = pars->nic_info;
    local    = nic_info->local_info;
    if(1 == local->antenna_diversity)
    {
        //ODM_AntDivTimers(pDM_Odm,INIT_ANTDIV_TIMMER);
    }

    if(1 == local->beamforming_support)
    {
    //ODM_InitializeTimer(pDM_Odm, &pDM_Odm->BeamformingInfo.BeamformingTimer,
    //  (RT_TIMER_CALL_BACK)Beamforming_SWTimerCallback, NULL, "BeamformingTimer");
    }

    //wf_os_api_timer_reg(&pars->ars_timer, (void *)ars_thread_handle, &pars->ars_timer);

    return WF_RETURN_OK;
}


wf_u8 DummyHubUsbMode = 1;/* USB 2.0 */
wf_s32 phydm_hook_dummy_member(ars_st *pars)
{
    if (pars->HubUsbMode == NULL)
    {   
        pars->HubUsbMode = &DummyHubUsbMode;
    }

    return WF_RETURN_OK;
}

static wf_s32 odm_CommonInfoSelfInit(void *ars)
{
    ars_st *pars    = NULL;
    wf_s32 ret      = 0;
    
    if(NULL == ars)
    {
        ARS_ENTRY_ERR("input param is null");
        return WF_RETURN_FAIL;
    }

    pars = ars;

    ars_io_lock_try(pars);
    //cck setting
    pars->bCckHighPower = (wf_bool) hw_read_bb_reg(pars->nic_info, ODM_REG_CCK_RPT_FORMAT_11N, ODM_BIT_CCK_RPT_FORMAT_11N);
    
    pars->RFPathRxEnable = (wf_u8) hw_read_bb_reg(pars->nic_info, ODM_REG_BB_RX_PATH_11N, ODM_BIT_BB_RX_PATH_11N);
    ars_io_unlock_try(pars);
    
    ret = ars_debug_init(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] ars_debug_init failed",__func__);
        return ret;
    }
    ret = ars_mp_init(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] ars_mp_init failed",__func__);
        return ret;
    }

    pars->TxRate = 0xFF;

    pars->number_linked_client      = 0;
    pars->pre_number_linked_client  = 0;
    pars->number_active_client      = 0;
    pars->pre_number_active_client  = 0;
    ret = phydm_hook_dummy_member(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] ars_mp_init failed",__func__);
        return ret;
    }

    return WF_RETURN_OK;
}


static wf_s32 ars_init_core(ars_st *pars)
{
    wf_s32 ret = 0;
    
    ret = odm_CommonInfoSelfInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] odm_CommonInfoSelfInit failed",__func__);
        return ret;
    }
    
    ret = ars_dig_init(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] ars_dig_init failed",__func__);
        return ret;
    }
    
    ret = Phydm_NHMCounterStatisticsInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] Phydm_NHMCounterStatisticsInit failed",__func__);
        return ret;
    }
    
    ret = Phydm_AdaptivityInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] Phydm_AdaptivityInit failed",__func__);
        return ret;
    }
    
    ret = phydm_ra_info_init(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] ars_mp_init failed",__func__);
        return ret;
    }
    
    ret = odm_RateAdaptiveMaskInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] odm_RateAdaptiveMaskInit failed",__func__);
        return ret;
    }
    
    ret = odm_RA_ParaAdjust_init(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] odm_RA_ParaAdjust_init failed",__func__);
        return ret;
    }
    
    ret = ODM_CfoTrackingInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] ODM_CfoTrackingInit failed",__func__);
        return ret;
    }
    
    ret = ODM_EdcaTurboInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] ODM_EdcaTurboInit failed",__func__);
        return ret;
    }
    
    ret = odm_RSSIMonitorInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] odm_RSSIMonitorInit failed",__func__);
        return ret;
    }
    
    ret = phydm_rf_init(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] phydm_rf_init failed",__func__);
        return ret;
    }
    
    ret = odm_AntennaDiversityInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] odm_AntennaDiversityInit failed",__func__);
        return ret;
    }
    ret = odm_AutoChannelSelectInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] odm_AutoChannelSelectInit failed",__func__);
        return ret;
    }
    ret = odm_PathDiversityInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] odm_PathDiversityInit failed",__func__);
        return ret;
    }
    ret = phydm_Beamforming_Init(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] phydm_Beamforming_Init failed",__func__);
        return ret;
    }
    
    ret = odm_DynamicBBPowerSavingInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] odm_DynamicBBPowerSavingInit failed",__func__);
        return ret;
    }
    
    ret = odm_DynamicTxPowerInit(pars);
    if(ret)
    {
        ARS_ENTRY_ERR("[%s] odm_DynamicTxPowerInit failed",__func__);
        return ret;
    }

    return WF_RETURN_OK;
}



static wf_s32 ars_init_for_netopen(ars_st *pars)
{
    wf_u32 SupportAbility = 0;
    nic_info_st *nic_info = NULL;
    local_info_st *local = NULL;
    hw_info_st    *hw    = NULL;

    if(NULL == pars)
    {
        ARS_ENTRY_ERR("input param is null");
        return WF_RETURN_FAIL;
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

    pars->SupportAbility = SupportAbility;

    return WF_RETURN_OK;
}


wf_s32 ODM_CmnInfoInit(ars_st *pars, ODM_CMNINFO_E   CmnInfo,wf_u32 Value )
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

    return WF_RETURN_OK;
}


wf_s32 ODM_CmnInfoHook(ars_st *pars,ODM_CMNINFO_E CmnInfo,void *pValue)
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

    return WF_RETURN_OK;
}

 wf_s32 ODM_CmnInfoPtrArrayHook(ars_st *pars,ODM_CMNINFO_E    CmnInfo,wf_u16 Index,void *pValue)
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

    return WF_RETURN_OK;
    
}


wf_s32 Init_ODM_ComInfo(ars_st *pars)
{
    nic_info_st *nic_info = pars->nic_info;
    hw_info_st  *hw       = nic_info->hw_info;

    ODM_CmnInfoInit(pars, ODM_CMNINFO_INTERFACE, nic_info->nic_type);

    ODM_CmnInfoInit(pars, ODM_CMNINFO_MP_TEST_CHIP, hw->mp_mode);

    ODM_CmnInfoInit(pars, ODM_CMNINFO_PATCH_ID, pars->com_save.CustomerID);

    //ODM_CmnInfoInit(pars, ODM_CMNINFO_BWIFI_TEST, adapter->registrypriv.wifi_spec);


    ODM_CmnInfoInit(pars, ODM_CMNINFO_RF_TYPE, ODM_1T1R);

    {
        //1 ======= BoardType: ODM_CMNINFO_BOARD_TYPE =======
        wf_u8 odm_board_type = ODM_BOARD_DEFAULT;

        if (pars->com_save.ExternalLNA_2G != 0) 
        {
            odm_board_type |= ODM_BOARD_EXT_LNA;
            ODM_CmnInfoInit(pars, ODM_CMNINFO_EXT_LNA, 1);
        }
        if (pars->com_save.ExternalPA_2G != 0) 
        {
            odm_board_type |= ODM_BOARD_EXT_PA;
            ODM_CmnInfoInit(pars, ODM_CMNINFO_EXT_PA, 1);
        }

        ODM_CmnInfoInit(pars, ODM_CMNINFO_BOARD_TYPE, odm_board_type);
        //1 ============== End of BoardType ==============
    }

    ODM_CmnInfoInit(pars, ODM_CMNINFO_DOMAIN_CODE_2G, hw->Regulation2_4G);

    ODM_CmnInfoInit(pars, ODM_CMNINFO_GPA, pars->com_save.TypeGPA);
    ODM_CmnInfoInit(pars, ODM_CMNINFO_APA, pars->com_save.TypeAPA);
    ODM_CmnInfoInit(pars, ODM_CMNINFO_GLNA, pars->com_save.TypeGLNA);
    ODM_CmnInfoInit(pars, ODM_CMNINFO_ALNA, pars->com_save.TypeALNA);
    ODM_CmnInfoInit(pars, ODM_CMNINFO_RFE_TYPE, pars->com_save.RFEType);
    ODM_CmnInfoInit(pars, ODM_CMNINFO_EXT_TRSW, 0);

    return WF_RETURN_OK;
}

static wf_s32 Init_ODM_ComInfo_8188f(ars_st *pars)
{
    nic_info_st *nic_info   = NULL;
    hw_info_st  *hw         = NULL;
    local_info_st *local    = NULL;
    wf_u32 SupportAbility = 0;

    nic_info = pars->nic_info;
    if(NULL == nic_info)
    {
        LOG_E("[%s] nic_info is null",__func__);
        return WF_RETURN_FAIL;
    }
    
    hw       = nic_info->hw_info;
    if(NULL == hw)
    {
        LOG_E("[%s] hw is null",__func__);
        return WF_RETURN_FAIL;
    }
    
    local    = nic_info->local_info;
    if(NULL == local)
    {
        LOG_E("[%s] local is null",__func__);
        return WF_RETURN_FAIL;
    }
    
    Init_ODM_ComInfo(pars);

    if( 0 == hw->use_drv_odm)
    {
        SupportAbility = 0;
    }
    else
    {
        SupportAbility = ODM_RF_CALIBRATION | ODM_RF_TX_PWR_TRACK;
    }

    pars->SupportAbility = SupportAbility;

    return WF_RETURN_OK;
}

wf_s32 wf_ars_info_update(nic_info_st *pnic_info)
{
    ars_st *pars = pnic_info->ars;
    pars->bLinked = wf_true;
    
    return WF_RETURN_OK;
}
wf_s32 ars_init(nic_info_st *pnic_info)
{
    wf_s32 ret      = 0;
    ars_st *pars    = NULL;
    
    ARS_ENTRY_INFO("start");

    if(NULL != pnic_info->odm)
    {
        LOG_D("nic_info->odm is not null");
        return 0;
    }
    pars = wf_os_api_get_ars(pnic_info);
    if(NULL != pars)
    {
        LOG_D("wf_os_api_get_odm get odm");
        pnic_info->ars        = pars;
    }
    else
    {
        
        if(NULL != pnic_info->ars)
        {
            LOG_D("nic_info->ars is not null");
            return 0;
        }
        
        pars = wf_kzalloc(sizeof(ars_st));
        if (pars == NULL)
        {
            LOG_D("malloc pars failed");
            return WF_RETURN_FAIL;
        }

        pnic_info->ars = pars;
        pars->nic_info = pnic_info;
        ret = phy_InitBBRFRegisterDefinition(pars);
        if(ret < 0)
        {
            ARS_ENTRY_ERR("[%s] phy_InitBBRFRegisterDefinition failed",__func__);
            return ret;
        }
        
        ret = Init_ODM_ComInfo_8188f(pars);
        if(ret < 0)
        {
            ARS_ENTRY_ERR("[%s] Init_ODM_ComInfo_8188f failed",__func__);
            return ret;
        }
        ret = ars_init_core(pars);
        if(ret < 0)
        {
            ARS_ENTRY_ERR("[%s] ars_init_core failed",__func__);
            return ret;
        }
        ret = ars_thread_init(pars);
        if(ret < 0)
        {
            ARS_ENTRY_ERR("[%s] ars_thread_init failed",__func__);
            return ret;
        }
        ret = ars_init_all_timers(pars);
        if(ret < 0)
        {
            ARS_ENTRY_ERR("[%s] ars_init_all_timers failed",__func__);
            return ret;
        }

        wf_os_api_set_ars(pnic_info, pars);
    }
    
    ARS_ENTRY_INFO("end");
    return 0;
}
wf_s32 ars_term(nic_info_st *pnic_info)
{
    ars_st *pars = NULL;
    
    LOG_I("ars_term start");

    if(NULL == pnic_info->ars)
    {
        return 0;
    }

    pars = wf_os_api_get_ars(pnic_info);
    if(NULL == pars)
    {
        LOG_I("[%s,%d]",__func__,__LINE__);
        return 0;
    }
    
    ars_thread_end(pars); 
    
    wf_kfree(pars);
    pars = NULL;

    return 0;
}

wf_s32 ars_io_lock_try(ars_st *ars)
{
    wf_s32 ret              = 0;
    nic_info_st *nic_info   = NULL;
   
    if(NULL == ars)
    {
        return WF_RETURN_FAIL;
    }
    nic_info = ars->nic_info;
    
    ret = wf_os_api_sema_try(&nic_info->cmd_sema);
    if (ret < 0)
    {
        return WF_RETURN_CMD_BUSY; // busy
    }
        
        //LOG_I(">>>>> func_code:0x%08x,%s sema ret:%d",cmd,cmd_to_str(cmd), ret);
#ifdef CONFIG_CONCURRENT_MODE
{
    nic_info_st *pbuddy_nic_info = NULL;
    pbuddy_nic_info = nic_info->vir_nic;
    {
        ret = wf_os_api_sema_try(&pbuddy_nic_info->cmd_sema);
        if (ret < 0)
        {
            wf_os_api_sema_post(&nic_info->cmd_sema);
            return WF_RETURN_CMD_BUSY; // busy
        }
    }
}
#endif


    return WF_RETURN_OK;
}

wf_s32 ars_io_unlock_try(ars_st *ars)
{
    nic_info_st *nic_info   = NULL;
    if(NULL == ars)
    {
        return WF_RETURN_FAIL;
    }

    nic_info = ars->nic_info;
#ifdef CONFIG_CONCURRENT_MODE
    {
        nic_info_st *pbuddy_nic_info = NULL;
        pbuddy_nic_info = nic_info->vir_nic;
        {
            wf_os_api_sema_post(&pbuddy_nic_info->cmd_sema);
        }
    }

#endif
    wf_os_api_sema_post(&nic_info->cmd_sema);

    return WF_RETURN_OK;
}

#endif
