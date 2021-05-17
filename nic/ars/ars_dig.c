#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT
#if 0
#define ARS_DIG_DBG(fmt, ...)      LOG_D("ARS_DIG[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_DIG_PRT(fmt, ...)      LOG_D("ARS_DIG-"fmt,##__VA_ARGS__)

#else
#define ARS_DIG_DBG(fmt, ...)
#define ARS_DIG_PRT(fmt, ...) 
#endif

#define ARS_DIG_INFO(fmt, ...)      LOG_I("ARS_DIG-"fmt,##__VA_ARGS__)
#define ARS_DIG_ERR(fmt, ...)      LOG_E("ARS_DIG-"fmt,##__VA_ARGS__)


wf_s32 ars_dig_init(void *ars)
{
    ars_st *pars = NULL;
    ars_dig_info_st *dig = NULL;
    if(NULL == ars)
    {
        ARS_DIG_DBG("input param is null");
        return WF_RETURN_FAIL;
    }

    pars = ars;
    dig = &pars->dig;
    if(NULL == dig)
    {
        ARS_DIG_DBG("dig is null");
        return WF_RETURN_FAIL;
    }

    ars_io_lock_try(pars);
    dig->CurIGValue             = (wf_u8) hw_read_bb_reg(pars->nic_info, ODM_REG_IGI_A_11N, ODM_BIT_IGI_11N);
    ars_io_unlock_try(pars);
    
    dig->bStopDIG               = wf_false;
    dig->bIgnoreDIG             = wf_false;
    dig->bPSDInProgress         = wf_false;
    dig->FALowThresh            = DM_FALSEALARM_THRESH_LOW;
    dig->FAHighThresh           = DM_FALSEALARM_THRESH_HIGH;
    dig->BackoffVal             = DM_DIG_BACKOFF_DEFAULT;
    dig->BackoffVal_range_max   = DM_DIG_BACKOFF_MAX;
    dig->BackoffVal_range_min   = DM_DIG_BACKOFF_MIN;
    dig->PreCCK_CCAThres        = 0xFF;
    dig->CurCCK_CCAThres        = 0x83;
    dig->ForbiddenIGI           = DM_DIG_MIN_NIC;
    dig->LargeFAHit             = 0;
    dig->Recover_cnt            = 0;
    dig->bMediaConnect_0        = wf_false;
    dig->bMediaConnect_1        = wf_false;

    //To Initialize pars->bDMInitialGainEnable == wf_false to avoid DIG error
    dig->bDMInitialGainEnable = wf_true;


    dig->DIG_Dynamic_MIN_0 = DM_DIG_MIN_NIC;
    dig->DIG_Dynamic_MIN_1 = DM_DIG_MIN_NIC;

    //To Initi BT30 IGI
    dig->BT30_CurIGI=0x32;

    wf_memset( dig->pause_dig_value, 0, (DM_DIG_MAX_PAUSE_TYPE + 1));
    dig->pause_dig_level = 0;
    wf_memset(dig->pause_cckpd_value, 0, (DM_DIG_MAX_PAUSE_TYPE + 1));
    dig->pause_cckpd_level = 0;


    if(pars->BoardType & (ODM_BOARD_EXT_PA|ODM_BOARD_EXT_LNA))
    {
        dig->rx_gain_range_max = DM_DIG_MAX_NIC;
        dig->rx_gain_range_min = DM_DIG_MIN_NIC;
    }
    else
    {
        dig->rx_gain_range_max = DM_DIG_MAX_NIC;
        dig->rx_gain_range_min = DM_DIG_MIN_NIC;
    }

    return WF_RETURN_OK;
}

wf_s32 odm_FalseAlarmCounterStatistics(void *ars)
{
    ars_st *pars = ars;
    FALSE_ALARM_STATISTICS *FalseAlmCnt = &pars->FalseAlmCnt;
    wf_u32 ret_value                    = 0;
    nic_info_st *nic_info               = NULL;
    
    if(!(pars->SupportAbility & ODM_BB_FA_CNT))
    {
        return WF_RETURN_OK;
    }

    nic_info = pars->nic_info;
    
    //hold ofdm counter
    hw_write_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_HOLDC_11N, BIT(31), 1); //hold page C counter
    hw_write_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_RSTD_11N, BIT(31), 1); //hold page D counter

    ret_value = hw_read_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_TYPE1_11N, bMaskDWord);
    FalseAlmCnt->Cnt_Fast_Fsync = (ret_value&0xffff);
    FalseAlmCnt->Cnt_SB_Search_fail = ((ret_value&0xffff0000)>>16);

    ret_value = hw_read_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_TYPE2_11N, bMaskDWord);
    FalseAlmCnt->Cnt_OFDM_CCA = (ret_value&0xffff); 
    FalseAlmCnt->Cnt_Parity_Fail = ((ret_value&0xffff0000)>>16);

    ret_value = hw_read_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_TYPE3_11N, bMaskDWord);
    FalseAlmCnt->Cnt_Rate_Illegal = (ret_value&0xffff);
    FalseAlmCnt->Cnt_Crc8_fail = ((ret_value&0xffff0000)>>16);

    ret_value = hw_read_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_TYPE4_11N, bMaskDWord);
    FalseAlmCnt->Cnt_Mcs_fail = (ret_value&0xffff);

    FalseAlmCnt->Cnt_Ofdm_fail =    FalseAlmCnt->Cnt_Parity_Fail + FalseAlmCnt->Cnt_Rate_Illegal +
                            FalseAlmCnt->Cnt_Crc8_fail + FalseAlmCnt->Cnt_Mcs_fail +
                            FalseAlmCnt->Cnt_Fast_Fsync + FalseAlmCnt->Cnt_SB_Search_fail;


    //hold cck counter
    hw_write_bb_reg(pars->nic_info, ODM_REG_CCK_FA_RST_11N, BIT(12), 1); 
    hw_write_bb_reg(pars->nic_info, ODM_REG_CCK_FA_RST_11N, BIT(14), 1); 

    ret_value = hw_read_bb_reg(pars->nic_info, ODM_REG_CCK_FA_LSB_11N, bMaskByte0);
    FalseAlmCnt->Cnt_Cck_fail = ret_value;

    ret_value = hw_read_bb_reg(pars->nic_info, ODM_REG_CCK_FA_MSB_11N, bMaskByte3);
    FalseAlmCnt->Cnt_Cck_fail +=  (ret_value& 0xff)<<8;

    ret_value = hw_read_bb_reg(pars->nic_info, ODM_REG_CCK_CCA_CNT_11N, bMaskDWord);
    FalseAlmCnt->Cnt_CCK_CCA = ((ret_value&0xFF)<<8) |((ret_value&0xFF00)>>8);

    
    FalseAlmCnt->Cnt_all = (FalseAlmCnt->Cnt_Fast_Fsync + 
                        FalseAlmCnt->Cnt_SB_Search_fail +
                        FalseAlmCnt->Cnt_Parity_Fail +
                        FalseAlmCnt->Cnt_Rate_Illegal +
                        FalseAlmCnt->Cnt_Crc8_fail +
                        FalseAlmCnt->Cnt_Mcs_fail +
                        FalseAlmCnt->Cnt_Cck_fail); 

    FalseAlmCnt->Cnt_CCA_all = FalseAlmCnt->Cnt_OFDM_CCA + FalseAlmCnt->Cnt_CCK_CCA;

    
    //reset false alarm counter registers
    hw_write_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_RSTC_11N, BIT(31), 1);
    hw_write_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_RSTC_11N, BIT(31), 0);
    hw_write_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_RSTD_11N, BIT(27), 1);
    hw_write_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_RSTD_11N, BIT(27), 0);

    //update ofdm counter
    hw_write_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_HOLDC_11N, BIT(31), 0); //update page C counter
    hw_write_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_RSTD_11N, BIT(31), 0); //update page D counter

    //reset CCK CCA counter
    hw_write_bb_reg(pars->nic_info, ODM_REG_CCK_FA_RST_11N, BIT(13)|BIT(12), 0); 
    hw_write_bb_reg(pars->nic_info, ODM_REG_CCK_FA_RST_11N, BIT(13)|BIT(12), 2); 
    //reset CCK FA counter
    hw_write_bb_reg(pars->nic_info, ODM_REG_CCK_FA_RST_11N, BIT(15)|BIT(14), 0); 
    hw_write_bb_reg(pars->nic_info, ODM_REG_CCK_FA_RST_11N, BIT(15)|BIT(14), 2); 

    return WF_RETURN_OK;

}


wf_s32 ODM_Write_DIG(void *ars, wf_u8 CurrentIGI)
{
    ars_st* pars = ars;
    ars_dig_info_st* pDM_DigTable = &pars->dig;

    if (pDM_DigTable->bStopDIG) 
    {
        ARS_DIG_DBG("ODM_Write_DIG(): Stop Writing IGI\n");
        return WF_RETURN_OK;
    }

    ARS_DIG_DBG("%s(): ODM_REG(IGI_A,pars)=0x%x, ODM_BIT(IGI,pars)=0x%x\n",
        __func__,ODM_REG_IGI_A_11N,ODM_BIT_IGI_11N);

    //1 Check initial gain by upper bound       
    if ((!pDM_DigTable->bPSDInProgress) && pars->bLinked)
    {
        if (CurrentIGI > pDM_DigTable->rx_gain_range_max) 
        {
            ARS_DIG_DBG("ODM_Write_DIG(): CurrentIGI(0x%02x) is larger than upper bound !!\n", CurrentIGI);
            CurrentIGI = pDM_DigTable->rx_gain_range_max;
        }
        if (pars->SupportAbility & ODM_BB_ADAPTIVITY && pars->adaptivity_flag == wf_true)
        {
            if(CurrentIGI > pars->Adaptivity_IGI_upper)
            {
                CurrentIGI = pars->Adaptivity_IGI_upper;
            }
    
            ARS_DIG_DBG("ODM_Write_DIG(): Adaptivity case: Force upper bound to 0x%x !!!!!!\n", CurrentIGI);
        }
    }

    if(pDM_DigTable->CurIGValue != CurrentIGI)
    {

        ARS_DIG_DBG("set bb reg");
        /*Add by YuChen for USB IO too slow issue*/
        if ((pars->SupportAbility & ODM_BB_ADAPTIVITY) && (CurrentIGI > pDM_DigTable->CurIGValue))
        {
            Phydm_Adaptivity(pars, CurrentIGI);
        }

        //1 Set IGI value
        hw_write_bb_reg(pars->nic_info, ODM_REG_IGI_A_11N, ODM_BIT_IGI_11N, CurrentIGI);

        if(pars->RFType > ODM_1T1R)
        {
            hw_write_bb_reg(pars->nic_info, ODM_REG_IGI_B_11N, ODM_BIT_IGI_11N, CurrentIGI);
        }

        if(pars->RFType > ODM_2T2R)
        {
            hw_write_bb_reg(pars->nic_info, ODM_REG_IGI_C_11N, ODM_BIT_IGI_11N, CurrentIGI);
            hw_write_bb_reg(pars->nic_info, ODM_REG_IGI_D_11N, ODM_BIT_IGI_11N, CurrentIGI);
        }

        
        pDM_DigTable->CurIGValue = CurrentIGI;
    }

    ARS_DIG_DBG("%s(): CurrentIGI(0x%02x).\n", __func__,CurrentIGI);

    return WF_RETURN_OK;
}

wf_bool odm_DigAbort(void *ars)
{
    ars_st*     pars = ars;
    ars_dig_info_st *dig = &pars->dig;


    //SupportAbility
    if(!(pars->SupportAbility & ODM_BB_FA_CNT))
    {
        ARS_DIG_DBG("%s(): Return: SupportAbility ODM_BB_FA_CNT is disabled\n",__func__);
        return  wf_true;
    }

    //SupportAbility
    if(!(pars->SupportAbility & ODM_BB_DIG))
    {   
        ARS_DIG_DBG("%s(): Return: SupportAbility ODM_BB_DIG is disabled\n",__func__);
        return  wf_true;
    }

    //ScanInProcess
    if(*(pars->pbScanInProcess))
    {
        ARS_DIG_DBG("%s(): Return: In Scan Progress \n",__func__);
            return  wf_true;
    }

    if(dig->bIgnoreDIG)
    {
        dig->bIgnoreDIG = wf_false;
        ARS_DIG_DBG("%s(): Return: Ignore DIG \n",__func__);
            return  wf_true;
    }

    //add by Neil Chen to avoid PSD is processing
    if(dig->bDMInitialGainEnable == wf_false)
    {
        ARS_DIG_DBG("%s(): Return: PSD is Processing \n",__func__);
        return  wf_true;
    }


    #ifdef CONFIG_SPECIAL_SETTING_FOR_FUNAI_TV  
    if(pars->bLinked)
    {   
        nic_info_st *nic_info = pars->nic_info;
        local_info_st *local = nic_info->local_info;
        if(0 != local->force_igi)
        {
            ARS_DIG_DBG("pars->RSSI_Min=%d \n",pars->RSSI_Min);
            ODM_Write_DIG(pars,local->force_igi);
            return  wf_true;
        }
    }
    #endif

    return  wf_false;
}

wf_s32 odm_DIGbyRSSI_LPS(void *ars)
{
    ars_st* pars = ars;
    FALSE_ALARM_STATISTICS  *pFalseAlmCnt = &pars->FalseAlmCnt;

    wf_u8   RSSI_Lower=DM_DIG_MIN_NIC;   //0x1E or 0x1C
    wf_u8   CurrentIGI=pars->RSSI_Min;

    if(odm_DigAbort(pars) == wf_true)
    {
        return WF_RETURN_OK;
    }

    CurrentIGI=CurrentIGI+RSSI_OFFSET_DIG;

    ARS_DIG_DBG("%s()==>\n",__func__);

    // Using FW PS mode to make IGI
    //Adjust by  FA in LPS MODE
    if(pFalseAlmCnt->Cnt_all> DM_DIG_FA_TH2_LPS)
    {   
        CurrentIGI = CurrentIGI+4;
    }
    else if (pFalseAlmCnt->Cnt_all > DM_DIG_FA_TH1_LPS)
    {   
        CurrentIGI = CurrentIGI+2;
    }
    else if(pFalseAlmCnt->Cnt_all < DM_DIG_FA_TH0_LPS)
    {   
        CurrentIGI = CurrentIGI-2;
    }


    //Lower bound checking

    //RSSI Lower bound check
    if((pars->RSSI_Min-10) > DM_DIG_MIN_NIC)
    {   
        RSSI_Lower =(pars->RSSI_Min-10);
    }
    else
    {   
        RSSI_Lower =DM_DIG_MIN_NIC;
    }

    //Upper and Lower Bound checking
     if(CurrentIGI > DM_DIG_MAX_NIC)
    {   
        CurrentIGI=DM_DIG_MAX_NIC;
       }
     else if(CurrentIGI < RSSI_Lower)
    {   
        CurrentIGI =RSSI_Lower;
    }

    ARS_DIG_DBG("odm_DIGbyRSSI_LPS(): pFalseAlmCnt->Cnt_all = %d\n",pFalseAlmCnt->Cnt_all);
    ARS_DIG_DBG("odm_DIGbyRSSI_LPS(): pars->RSSI_Min = %d\n",pars->RSSI_Min);
    ARS_DIG_DBG("odm_DIGbyRSSI_LPS(): CurrentIGI = 0x%x\n",CurrentIGI);

    ODM_Write_DIG(pars, CurrentIGI);//ODM_Write_DIG(pars, dig->CurIGValue);

    return WF_RETURN_OK;
}

wf_s32 odm_FAThresholdCheck(void *ars,wf_bool bDFSBand,wf_bool bPerformance,wf_u32 RxTp,wf_u32 TxTp,wf_u32 *dm_FA_thres)
{
    ars_st *pars = ars;
    
    if(pars->bLinked && (bPerformance||bDFSBand))
    {
            // For NIC
        dm_FA_thres[0] = DM_DIG_FA_TH0;
        dm_FA_thres[1] = DM_DIG_FA_TH1;
        dm_FA_thres[2] = DM_DIG_FA_TH2;
        
    }
    else
    {
        
        dm_FA_thres[0] = 2000;
        dm_FA_thres[1] = 4000;
        dm_FA_thres[2] = 5000;
        
    }
    
    return WF_RETURN_OK;
}

wf_u8 odm_ForbiddenIGICheck(void *ars,wf_u8 DIG_Dynamic_MIN,wf_u8 CurrentIGI)
{
    ars_st*pars                         = ars;
    ars_dig_info_st *pDM_DigTable        = &pars->dig;
    PFALSE_ALARM_STATISTICS pFalseAlmCnt = &pars->FalseAlmCnt;
    wf_u8 rx_gain_range_min              = pDM_DigTable->rx_gain_range_min;

    if(pFalseAlmCnt->Cnt_all > 10000)
    {
        ARS_DIG_DBG("odm_DIG(): Abnormally false alarm case. \n");

        if(pDM_DigTable->LargeFAHit != 3)
        {      
            pDM_DigTable->LargeFAHit++;
        }
        
        if(pDM_DigTable->ForbiddenIGI < CurrentIGI)//if(pDM_DigTable->ForbiddenIGI < pDM_DigTable->CurIGValue)
        {
            pDM_DigTable->ForbiddenIGI = CurrentIGI;//pDM_DigTable->ForbiddenIGI = pDM_DigTable->CurIGValue;
            pDM_DigTable->LargeFAHit = 1;
        }

        if(pDM_DigTable->LargeFAHit >= 3)
        {
            if((pDM_DigTable->ForbiddenIGI + 2) > pDM_DigTable->rx_gain_range_max)
            {
                rx_gain_range_min = pDM_DigTable->rx_gain_range_max;
            }
            else
            {
                rx_gain_range_min = (pDM_DigTable->ForbiddenIGI + 2);
            }
            pDM_DigTable->Recover_cnt = 1800;
            ARS_DIG_DBG("odm_DIG(): Abnormally false alarm case: Recover_cnt = %d \n", pDM_DigTable->Recover_cnt);
        }
    }
    else
    {
        if(pDM_DigTable->Recover_cnt != 0)
        {
            pDM_DigTable->Recover_cnt --;
            ARS_DIG_DBG("odm_DIG(): Normal Case: Recover_cnt = %d \n", pDM_DigTable->Recover_cnt);
        }
        else
        {
            if(pDM_DigTable->LargeFAHit < 3)
            {
                if((pDM_DigTable->ForbiddenIGI - 2) < DIG_Dynamic_MIN) //DM_DIG_MIN)
                {
                    pDM_DigTable->ForbiddenIGI = DIG_Dynamic_MIN; //DM_DIG_MIN;
                    rx_gain_range_min = DIG_Dynamic_MIN; //DM_DIG_MIN;
                    //ARS_DIG_DBG ("odm_DIG(): Normal Case: At Lower Bound\n");
                }
                else
                {
                    pDM_DigTable->ForbiddenIGI -= 2;
                    rx_gain_range_min = (pDM_DigTable->ForbiddenIGI + 2);
                    //ARS_DIG_DBG ("odm_DIG(): Normal Case: Approach Lower Bound\n");
                }
            }
            else
            {
                pDM_DigTable->LargeFAHit = 0;
            }
        }
    }
    
    return rx_gain_range_min;

}


wf_s32 odm_DIG(void *ars)
{
    ars_st*                   pars = ars;
    ars_dig_info_st*          dig  = &pars->dig;
    FALSE_ALARM_STATISTICS*     pFalseAlmCnt = &pars->FalseAlmCnt;
    wf_bool                     FirstConnect = wf_false;
    wf_bool                     FirstDisConnect = wf_false;
    wf_u8                      DIG_MaxOfMin= 0;
    wf_u8                      DIG_Dynamic_MIN = 0;
    wf_u8                      dm_dig_max = 0;
    wf_u8                      dm_dig_min = 0;
    wf_u8                      CurrentIGI = dig->CurIGValue;
    wf_u8                      offset = 0;
    wf_u32                      dm_FA_thres[3];
    wf_u32                      TxTp = 0, RxTp = 0;
    wf_bool                     bDFSBand = wf_false;
    wf_bool                     bPerformance = wf_true;
    wf_bool bFirstTpTarget = wf_false;
    wf_bool bFirstCoverage = wf_false;

    if(odm_DigAbort(pars) == wf_true)
    {
        return WF_RETURN_FAIL;
    }
    
    //1 Update status
    DIG_Dynamic_MIN = dig->DIG_Dynamic_MIN_0;
    FirstConnect = (pars->bLinked) && (dig->bMediaConnect_0 == wf_false);
    FirstDisConnect = (!pars->bLinked) && (dig->bMediaConnect_0 == wf_true);
    

    //1 Boundary Decision
    dm_dig_max      = 0x5A;
    dm_dig_min      = DM_DIG_MIN_NIC;
    DIG_MaxOfMin    = DM_DIG_MAX_AP;


    ARS_DIG_PRT("odm_DIG():rssi:%d Absolutly dig: 0x%x~0x%x\n",pars->RSSI_Min,dm_dig_min,dm_dig_max);

    //1 Adjust boundary by RSSI
    if(pars->bLinked && bPerformance)
    {
        wf_u8 tmp_dig = 0;
        //2 Modify DIG upper bound
        
        offset  = 15;
        tmp_dig = pars->RSSI_Min + offset;
        ARS_DIG_PRT("tmp_dig:0x%x",tmp_dig);
        if(tmp_dig > dm_dig_max )
        {
            dig->rx_gain_range_max = dm_dig_max;
        }
        else if(tmp_dig < dm_dig_min )
        {
            dig->rx_gain_range_max = dm_dig_min;
        }
        else
        {
            dig->rx_gain_range_max = tmp_dig;
        }

        //2 Modify DIG lower bound
        if(pars->bOneEntryOnly)
        {
            if(pars->RSSI_Min < dm_dig_min)
            {
                DIG_Dynamic_MIN = dm_dig_min;
            }
            else if (pars->RSSI_Min > DIG_MaxOfMin)
            {
                DIG_Dynamic_MIN = DIG_MaxOfMin;
            }
            else
            {
                DIG_Dynamic_MIN = pars->RSSI_Min;
            }
        }

    }
    else
    {
        dig->rx_gain_range_max = DM_DIG_MAX_OF_MIN;
        DIG_Dynamic_MIN = dm_dig_min;
    }
    
    //1 Force Lower Bound for AntDiv
    if(pars->bLinked && !pars->bOneEntryOnly)
    {
        if(pars->SupportAbility & ODM_BB_ANT_DIV)
        {
            if (pars->AntDivType == CG_TRX_HW_ANTDIV || pars->AntDivType == CG_TRX_SMART_ANTDIV) 
            {
                if (dig->AntDiv_RSSI_max > DIG_MaxOfMin)
                {
                    DIG_Dynamic_MIN = DIG_MaxOfMin;
                }
                else
                {
                    DIG_Dynamic_MIN = (wf_u8) dig->AntDiv_RSSI_max;
                }
                ARS_DIG_PRT("odm_DIG(): Antenna diversity case: Force lower bound to 0x%x !!!!!!\n", DIG_Dynamic_MIN);
                ARS_DIG_PRT("odm_DIG(): Antenna diversity case: RSSI_max = 0x%x !!!!!!\n", dig->AntDiv_RSSI_max);
            }
        }
    }
    
    ARS_DIG_PRT("odm_DIG():Dig Adjust boundary by RSSI [0x%x~0x%x]\n", DIG_Dynamic_MIN,dig->rx_gain_range_max);
//  ARS_DIG_PRT("odm_DIG(): Link status: bLinked = %d, RSSI = %d, bFirstConnect = %d, bFirsrDisConnect = %d\n\n",
//        pars->bLinked, pars->RSSI_Min, FirstConnect, FirstDisConnect);

    //1 Modify DIG lower bound, deal with abnormal case
    //2 Abnormal false alarm case
    if(!pars->bLinked)
    {
        dig->rx_gain_range_min = DIG_Dynamic_MIN;

        if (FirstDisConnect)
        {
            dig->ForbiddenIGI = DIG_Dynamic_MIN;
        }
    }
    else
    {
        dig->rx_gain_range_min = odm_ForbiddenIGICheck(pars, DIG_Dynamic_MIN, CurrentIGI);
    }
    

    //2 Abnormal # beacon case
    if(pars->bLinked && !FirstConnect)
    {
        ARS_DIG_PRT("Beacon Num (%d)\n", pars->dbg.dbg_info.NumQryBeaconPkt);
        if((pars->dbg.dbg_info.NumQryBeaconPkt < 5) && (pars->bsta_state))
        {
            dig->rx_gain_range_min = dm_dig_min;
            ARS_DIG_DBG("odm_DIG(): Abnrormal #beacon (%d) case in STA mode: Force lower bound to 0x%x !!!!!!\n\n",
                pars->dbg.dbg_info.NumQryBeaconPkt, dig->rx_gain_range_min);
        }
    }


    //2 Abnormal lower bound case
    if(dig->rx_gain_range_min > dig->rx_gain_range_max)
    {
        dig->rx_gain_range_min = dig->rx_gain_range_max;
        ARS_DIG_PRT("odm_DIG(): Abnrormal lower bound case: Force lower bound to 0x%x !!!!!!\n\n",dig->rx_gain_range_min);
    }

    
    //1 False alarm threshold decision
    odm_FAThresholdCheck(pars, bDFSBand, bPerformance, RxTp, TxTp, dm_FA_thres);
    ARS_DIG_PRT("odm_DIG(): False alarm threshold = %d, %d, %d \n\n", dm_FA_thres[0], dm_FA_thres[1], dm_FA_thres[2]);

    //1 Adjust initial gain by false alarm
    if(pars->bLinked && bPerformance)
    {
        //2 After link
        //ARS_DIG_DBG("odm_DIG(): Adjust IGI after link\n");

        if(bFirstTpTarget || (FirstConnect && bPerformance))
        {   
            dig->LargeFAHit = 0;
            
            if(pars->RSSI_Min < DIG_MaxOfMin)
            {
                if(CurrentIGI < pars->RSSI_Min)
                {
                    CurrentIGI = pars->RSSI_Min;
                }
            }
            else
            {
                if(CurrentIGI < DIG_MaxOfMin)
                {
                    CurrentIGI = DIG_MaxOfMin;
                }
            }


            ARS_DIG_PRT("odm_DIG(): First connect case: IGI does on-shot to 0x%x\n", CurrentIGI);

        }
        else
        {
            if(pFalseAlmCnt->Cnt_all > dm_FA_thres[2])
            {
                CurrentIGI = CurrentIGI + 4;
            }
            else if (pFalseAlmCnt->Cnt_all > dm_FA_thres[1])
            {
                CurrentIGI = CurrentIGI + 2;
            }
            else if(pFalseAlmCnt->Cnt_all < dm_FA_thres[0])
            {
                CurrentIGI = CurrentIGI - 2;
            }

            //4 Abnormal # beacon case
            if((pars->dbg.dbg_info.NumQryBeaconPkt < 5) && (pFalseAlmCnt->Cnt_all < DM_DIG_FA_TH1) && (pars->bsta_state))
            {                       
                CurrentIGI = dig->rx_gain_range_min;
                ARS_DIG_PRT("odm_DIG(): Abnormal #beacon (%d) case: IGI does one-shot to 0x%x\n", 
                    pars->dbg.dbg_info.NumQryBeaconPkt, CurrentIGI);
            }
            ARS_DIG_PRT("odm_DIG(): FalseAlm Cnt All:%d, CurrentIGI:%d\n",pFalseAlmCnt->Cnt_all,CurrentIGI);
        }
    }   
    else
    {
        //2 Before link
        //ARS_DIG_PRT("odm_DIG(): Adjust IGI before link\n");
        
        if(FirstDisConnect || bFirstCoverage)
        {
            CurrentIGI = dm_dig_min;
            ARS_DIG_PRT("odm_DIG(): First disconnect case: IGI does on-shot to lower bound\n");
        }
        else
        {
            if(pFalseAlmCnt->Cnt_all > dm_FA_thres[2])
            {
                CurrentIGI = CurrentIGI + 4;
            }
            else if (pFalseAlmCnt->Cnt_all > dm_FA_thres[1])
            {
                CurrentIGI = CurrentIGI + 2;
            }
            else if(pFalseAlmCnt->Cnt_all < dm_FA_thres[0])
            {
                CurrentIGI = CurrentIGI - 2;
            }
        }
    }

    //1 Check initial gain by upper/lower bound
    if(CurrentIGI < dig->rx_gain_range_min)
    {
        CurrentIGI = dig->rx_gain_range_min;
    }
    
    if(CurrentIGI > dig->rx_gain_range_max)
    {
        CurrentIGI = dig->rx_gain_range_max;
    }

    ARS_DIG_PRT("odm_DIG(): CurIGValue=0x%x, TotalFA = %d\n\n", CurrentIGI, pFalseAlmCnt->Cnt_all); 

    //1 High power RSSI threshold
    ODM_Write_DIG(pars, CurrentIGI);//ODM_Write_DIG(pars, dig->CurIGValue);
    dig->bMediaConnect_0 = pars->bLinked;
    dig->DIG_Dynamic_MIN_0 = DIG_Dynamic_MIN;
        
    return WF_RETURN_OK;
}




wf_s32 ODM_Write_CCK_CCA_Thres(void *ars,wf_u8 CurCCK_CCAThres)
{
    ars_st*                  pars = ars;
    ars_dig_info_st* pDM_DigTable = &pars->dig;

    if(pDM_DigTable->CurCCK_CCAThres!=CurCCK_CCAThres)      //modify by Guo.Mingzhi 2012-01-03
    {
        wf_io_write8(pars->nic_info, ODM_REG_CCK_CCA_11N, CurCCK_CCAThres);
    }
    pDM_DigTable->PreCCK_CCAThres = pDM_DigTable->CurCCK_CCAThres;
    pDM_DigTable->CurCCK_CCAThres = CurCCK_CCAThres;

    return WF_RETURN_OK;
}


wf_s32 odm_CCKPacketDetectionThresh(void *ars    )
{
    ars_st*                   pars = ars;
    FALSE_ALARM_STATISTICS     *FalseAlmCnt = &pars->FalseAlmCnt;
    wf_u8                      CurCCK_CCAThres, RSSI_thd = 55;


    if((!(pars->SupportAbility & ODM_BB_CCK_PD)) || (!(pars->SupportAbility & ODM_BB_FA_CNT)))
    {
        ARS_DIG_DBG("odm_CCKPacketDetectionThresh()  return==========\n");
#ifdef MCR_WIRELESS_EXTEND
        ODM_Write_CCK_CCA_Thres(pars, 0x43);
#endif
        return WF_RETURN_OK;
    }

    if(pars->ExtLNA)
    {
        return WF_RETURN_OK;
    }

    ARS_DIG_DBG("odm_CCKPacketDetectionThresh()  ==========>\n");

    if (pars->bLinked)
    {
        if (pars->RSSI_Min > RSSI_thd)
            CurCCK_CCAThres = 0xcd;
        else if ((pars->RSSI_Min <= RSSI_thd) && (pars->RSSI_Min > 10))
            CurCCK_CCAThres = 0x83;
        else
        {
            if(FalseAlmCnt->Cnt_Cck_fail > 1000)
                CurCCK_CCAThres = 0x83;
            else
                CurCCK_CCAThres = 0x40;
        }
    } else 
    {
        if(FalseAlmCnt->Cnt_Cck_fail > 1000)
        {
            CurCCK_CCAThres = 0x83;
        }
        else
        {
            CurCCK_CCAThres = 0x40;
        }
    }
    
    ODM_Write_CCK_CCA_Thres(pars, CurCCK_CCAThres);

    ARS_DIG_DBG("odm_CCKPacketDetectionThresh()  CurCCK_CCAThres = 0x%x\n",CurCCK_CCAThres);

    return WF_RETURN_OK;
}

#endif

