#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_DRIVER_SUPPORT

#if 1
#define ARS_TP_DBG(fmt, ...)      LOG_D("ARS_TP[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_TP_PRT(fmt, ...)      LOG_D("ARS_TP-"fmt, ##__VA_ARGS__)

#else
#define ARS_THD_DBG(fmt, ...)
#define ARS_TP_PRT(fmt, ...)

#endif
#define ARS_TP_INFO(fmt, ...)      LOG_I("ARS_TP-"fmt,##__VA_ARGS__)
#define ARS_TP_ERR(fmt, ...)      LOG_E("ARS_TP-"fmt,##__VA_ARGS__)


wf_s32 odm_DynamicTxPowerNIC(void *ars)
{
    #if 0
    ars_st *pars = (ars_st*)ars;
    
    if (!(pars->SupportAbility & ODM_BB_DYNAMIC_TXPWR))
        return;
    
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))

    if(pars->SupportICType == ODM_RTL8192C) 
    {
        odm_DynamicTxPower_92C(pars);
    }
    else if(pars->SupportICType == ODM_RTL8192D)
    {
        odm_DynamicTxPower_92D(pars);
    }
    else if (pars->SupportICType == ODM_RTL8821)
    {
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN))
        PADAPTER        Adapter  =  pars->Adapter;
        PMGNT_INFO      pMgntInfo = GetDefaultMgntInfo(Adapter);

        if (pMgntInfo->RegRspPwr == 1)
        {
            if(pars->RSSI_Min > 60)
            {
                ODM_SetMACReg(pars, ODM_REG_RESP_TX_11AC, BIT20|BIT19|BIT18, 1); // Resp TXAGC offset = -3dB

            }
            else if(pars->RSSI_Min < 55)
            {
                ODM_SetMACReg(pars, ODM_REG_RESP_TX_11AC, BIT20|BIT19|BIT18, 0); // Resp TXAGC offset = 0dB
            }
        }
#endif
    }
#endif  
#else
//do nothing
#endif
    return 0;
}

wf_s32 odm_DynamicTxPower(void *ars)
{
    ars_st *pars = (ars_st*)ars;
    if (!(pars->SupportAbility & ODM_BB_DYNAMIC_TXPWR))
    {
        return WF_RETURN_OK;
    }

    odm_DynamicTxPowerNIC(pars);

    return WF_RETURN_OK;
}

#if 0
void ODM_TXPowerTrackingCallback_ThermalMeter(ars_st *ars)
{
    PODM_RF_CAL_T   pRFCalibrateInfo = &(ars->RFCalibrateInfo);


    HAL_DATA_TYPE   *pHalData = GET_HAL_DATA(Adapter);
    //PMGNT_INFO            pMgntInfo = &Adapter->MgntInfo;
    
    wf_u8           ThermalValue = 0, delta, delta_LCK, delta_IQK, offset;
    wf_u8           ThermalValue_AVG_count = 0;
    wf_u32          ThermalValue_AVG = 0;   
//  wf_s32          ele_A=0, ele_D, TempCCk, X, value32;
//  wf_s32          Y, ele_C=0;
//  wf_s8           OFDM_index[2], CCK_index=0, OFDM_index_old[2]={0,0}, CCK_index_old=0, index;
//  wf_s8           deltaPowerIndex = 0;
    wf_u32          i = 0;//, j = 0;
    wf_bool         is2T = FALSE;
//  wf_bool         bInteralPA = FALSE;

    wf_u8           OFDM_max_index = 34, rf = (is2T) ? 2 : 1; //OFDM BB Swing should be less than +3.0dB, which is required by Arthur
    wf_u8           Indexforchannel = 0;/*GetRightChnlPlaceforIQK(pHalData->CurrentChannel)*/
    enum            _POWER_DEC_INC { POWER_DEC, POWER_INC };
    
    PDM_ODM_T       pDM_Odm = &pHalData->odmpriv;


    TXPWRTRACK_CFG  c;


    //4 1. The following TWO tables decide the final index of OFDM/CCK swing table.
    wf_s8           deltaSwingTableIdx[2][index_mapping_NUM_88E] = { 
                        // {{Power decreasing(lower temperature)}, {Power increasing(higher temperature)}}
                        {0,0,2,3,4,4,5,6,7,7,8,9,10,10,11}, {0,0,1,2,3,4,4,4,4,5,7,8,9,9,10}
                    };  
    wf_u8           thermalThreshold[2][index_mapping_NUM_88E]={
                        // {{Power decreasing(lower temperature)}, {Power increasing(higher temperature)}}
                        {0,2,4,6,8,10,12,14,16,18,20,22,24,26,27}, {0,2,4,6,8,10,12,14,16,18,20,22,25,25,25}
                    };      


    //4 2. Initilization ( 7 steps in total )

    ConfigureTxpowerTrack(pDM_Odm, &c);
    
    pDM_Odm->RFCalibrateInfo.TXPowerTrackingCallbackCnt++; //cosa add for debug
    pDM_Odm->RFCalibrateInfo.bTXPowerTrackingInit = TRUE;
    
#if (MP_DRIVER == 1) 
    pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = pHalData->TxPowerTrackControl; // <Kordan> We should keep updating the control variable according to HalData.
    // <Kordan> RFCalibrateInfo.RegA24 will be initialized when ODM HW configuring, but MP configures with para files.
    pDM_Odm->RFCalibrateInfo.RegA24 = 0x090e1317; 
#endif

    ARS_TP_INFO("===>odm_TXPowerTrackingCallback_ThermalMeter_8188E, pDM_Odm->BbSwingIdxCckBase: %d, pDM_Odm->BbSwingIdxOfdmBase: %d \n", pRFCalibrateInfo->BbSwingIdxCckBase, pRFCalibrateInfo->BbSwingIdxOfdmBase);
/*
    if (!pDM_Odm->RFCalibrateInfo.TM_Trigger) {
        ODM_SetRFReg(pDM_Odm, RF_PATH_A, c.ThermalRegAddr, BIT17 | BIT16, 0x3);
        pDM_Odm->RFCalibrateInfo.TM_Trigger = 1;
        return;
    }
*/  
    ThermalValue = (wf_u8)ODM_GetRFReg(pDM_Odm, RF_PATH_A, c.ThermalRegAddr, 0xfc00);   //0x42: RF Reg[15:10] 88E

    if( ! ThermalValue || ! pDM_Odm->RFCalibrateInfo.TxPowerTrackControl)
    {   
        return;
    }

    //4 3. Initialize ThermalValues of RFCalibrateInfo
    
    if( ! pDM_Odm->RFCalibrateInfo.ThermalValue)
    {
        pDM_Odm->RFCalibrateInfo.ThermalValue_LCK = ThermalValue;               
        pDM_Odm->RFCalibrateInfo.ThermalValue_IQK = ThermalValue;                                       
    }           

    if(pDM_Odm->RFCalibrateInfo.bReloadtxpowerindex)
    {
        ARS_TP_INFO("reload ofdm index for band switch\n"); 
    }

    //4 4. Calculate average thermal meter
    
    pDM_Odm->RFCalibrateInfo.ThermalValue_AVG[pDM_Odm->RFCalibrateInfo.ThermalValue_AVG_index] = ThermalValue;
    pDM_Odm->RFCalibrateInfo.ThermalValue_AVG_index++;
    if(pDM_Odm->RFCalibrateInfo.ThermalValue_AVG_index == c.AverageThermalNum)
        pDM_Odm->RFCalibrateInfo.ThermalValue_AVG_index = 0;

    for(i = 0; i < c.AverageThermalNum; i++)
    {
        if(pDM_Odm->RFCalibrateInfo.ThermalValue_AVG[i])
        {
            ThermalValue_AVG += pDM_Odm->RFCalibrateInfo.ThermalValue_AVG[i];
            ThermalValue_AVG_count++;
        }
    }

    if(ThermalValue_AVG_count)
    {
        // Give the new thermo value a weighting
        ThermalValue_AVG += (ThermalValue*4);
        
        ThermalValue = (wf_u8)(ThermalValue_AVG / (ThermalValue_AVG_count+4));
        ARS_TP_INFO("AVG Thermal Meter = 0x%x \n", ThermalValue);                   
    }
            
    //4 5. Calculate delta, delta_LCK, delta_IQK.
    
    delta     = (ThermalValue > pDM_Odm->RFCalibrateInfo.ThermalValue)?(ThermalValue - pDM_Odm->RFCalibrateInfo.ThermalValue):(pDM_Odm->RFCalibrateInfo.ThermalValue - ThermalValue);
    delta_LCK = (ThermalValue > pDM_Odm->RFCalibrateInfo.ThermalValue_LCK)?(ThermalValue - pDM_Odm->RFCalibrateInfo.ThermalValue_LCK):(pDM_Odm->RFCalibrateInfo.ThermalValue_LCK - ThermalValue);
    delta_IQK = (ThermalValue > pDM_Odm->RFCalibrateInfo.ThermalValue_IQK)?(ThermalValue - pDM_Odm->RFCalibrateInfo.ThermalValue_IQK):(pDM_Odm->RFCalibrateInfo.ThermalValue_IQK - ThermalValue);
        
    //4 6. If necessary, do LCK.    

    /*if((delta_LCK > pHalData->Delta_LCK) && (pHalData->Delta_LCK != 0))*/
        if (delta_LCK >= c.Threshold_IQK) { 
            /*Delta temperature is equal to or larger than 20 centigrade.*/
            pDM_Odm->RFCalibrateInfo.ThermalValue_LCK = ThermalValue;
            (*c.PHY_LCCalibrate)(pDM_Odm);
        }
    

    //3 7. If necessary, move the index of swing table to adjust Tx power.  
    
    if (delta > 0 && pDM_Odm->RFCalibrateInfo.TxPowerTrackControl)
    {
    
        delta = ThermalValue > pHalData->EEPROMThermalMeter?(ThermalValue - pHalData->EEPROMThermalMeter):(pHalData->EEPROMThermalMeter - ThermalValue);


        //4 7.1 The Final Power Index = BaseIndex + PowerIndexOffset
        
        
        if(ThermalValue > pHalData->EEPROMThermalMeter) {
            CALCULATE_SWINGTALBE_OFFSET(offset, POWER_INC, index_mapping_NUM_88E, delta);
            pDM_Odm->RFCalibrateInfo.DeltaPowerIndexLast = pDM_Odm->RFCalibrateInfo.DeltaPowerIndex;
            pDM_Odm->RFCalibrateInfo.DeltaPowerIndex =  deltaSwingTableIdx[POWER_INC][offset];

        } else {
        
            CALCULATE_SWINGTALBE_OFFSET(offset, POWER_DEC, index_mapping_NUM_88E, delta);
            pDM_Odm->RFCalibrateInfo.DeltaPowerIndexLast = pDM_Odm->RFCalibrateInfo.DeltaPowerIndex;
            pDM_Odm->RFCalibrateInfo.DeltaPowerIndex = (-1)*deltaSwingTableIdx[POWER_DEC][offset];
        }
        
        if (pDM_Odm->RFCalibrateInfo.DeltaPowerIndex == pDM_Odm->RFCalibrateInfo.DeltaPowerIndexLast)
            pDM_Odm->RFCalibrateInfo.PowerIndexOffset = 0;
        else
            pDM_Odm->RFCalibrateInfo.PowerIndexOffset = pDM_Odm->RFCalibrateInfo.DeltaPowerIndex - pDM_Odm->RFCalibrateInfo.DeltaPowerIndexLast;
        
        for(i = 0; i < rf; i++)         
            pDM_Odm->RFCalibrateInfo.OFDM_index[i] = pRFCalibrateInfo->BbSwingIdxOfdmBase + pDM_Odm->RFCalibrateInfo.PowerIndexOffset;
        pDM_Odm->RFCalibrateInfo.CCK_index = pRFCalibrateInfo->BbSwingIdxCckBase + pDM_Odm->RFCalibrateInfo.PowerIndexOffset;

        pRFCalibrateInfo->BbSwingIdxCck = pDM_Odm->RFCalibrateInfo.CCK_index;   
        pRFCalibrateInfo->BbSwingIdxOfdm[RF_PATH_A] = pDM_Odm->RFCalibrateInfo.OFDM_index[RF_PATH_A];   

        ARS_TP_INFO("The 'CCK' final index(%d) = BaseIndex(%d) + PowerIndexOffset(%d)\n", pRFCalibrateInfo->BbSwingIdxCck, pRFCalibrateInfo->BbSwingIdxCckBase, pDM_Odm->RFCalibrateInfo.PowerIndexOffset);
        ARS_TP_INFO("The 'OFDM' final index(%d) = BaseIndex(%d) + PowerIndexOffset(%d)\n", pRFCalibrateInfo->BbSwingIdxOfdm[RF_PATH_A], pRFCalibrateInfo->BbSwingIdxOfdmBase, pDM_Odm->RFCalibrateInfo.PowerIndexOffset);

        //4 7.1 Handle boundary conditions of index.
        
        
        for(i = 0; i < rf; i++)
        {
            if(pDM_Odm->RFCalibrateInfo.OFDM_index[i] > OFDM_max_index)
            {
                pDM_Odm->RFCalibrateInfo.OFDM_index[i] = OFDM_max_index;
            }
            else if (pDM_Odm->RFCalibrateInfo.OFDM_index[i] < 0)
            {
                pDM_Odm->RFCalibrateInfo.OFDM_index[i] = 0;
            }
        }

        if(pDM_Odm->RFCalibrateInfo.CCK_index > c.SwingTableSize_CCK-1)
            pDM_Odm->RFCalibrateInfo.CCK_index = c.SwingTableSize_CCK-1;
        else if (pDM_Odm->RFCalibrateInfo.CCK_index < 0)
            pDM_Odm->RFCalibrateInfo.CCK_index = 0;
    }
    else
    {
        ARS_TP_INFO
            ("The thermal meter is unchanged or TxPowerTracking OFF: ThermalValue: %d , pDM_Odm->RFCalibrateInfo.ThermalValue: %d)\n", ThermalValue, pDM_Odm->RFCalibrateInfo.ThermalValue);
        pDM_Odm->RFCalibrateInfo.PowerIndexOffset = 0;
    }
    ARS_TP_INFO
        ("TxPowerTracking: [CCK] Swing Current Index: %d, Swing Base Index: %d\n", pDM_Odm->RFCalibrateInfo.CCK_index, pRFCalibrateInfo->BbSwingIdxCckBase);
                
    ARS_TP_INFO
        ("TxPowerTracking: [OFDM] Swing Current Index: %d, Swing Base Index: %d\n", pDM_Odm->RFCalibrateInfo.OFDM_index[RF_PATH_A], pRFCalibrateInfo->BbSwingIdxOfdmBase);
    
    if (pDM_Odm->RFCalibrateInfo.PowerIndexOffset != 0 && pDM_Odm->RFCalibrateInfo.TxPowerTrackControl)
    {
        //4 7.2 Configure the Swing Table to adjust Tx Power.
        
            pDM_Odm->RFCalibrateInfo.bTxPowerChanged = TRUE; // Always TRUE after Tx Power is adjusted by power tracking.           
            //
            // 2012/04/23 MH According to Luke's suggestion, we can not write BB digital
            // to increase TX power. Otherwise, EVM will be bad.
            //
            // 2012/04/25 MH Add for tx power tracking to set tx power in tx agc for 88E.
            if (ThermalValue > pDM_Odm->RFCalibrateInfo.ThermalValue)
            {
                //ARS_TP_INFO
                //  ("Temperature Increasing: delta_pi: %d , delta_t: %d, Now_t: %d, EFUSE_t: %d, Last_t: %d\n", 
                //  pDM_Odm->RFCalibrateInfo.PowerIndexOffset, delta, ThermalValue, pHalData->EEPROMThermalMeter, pDM_Odm->RFCalibrateInfo.ThermalValue);   
            }
            else if (ThermalValue < pDM_Odm->RFCalibrateInfo.ThermalValue)// Low temperature
            {
                //ARS_TP_INFO
                //  ("Temperature Decreasing: delta_pi: %d , delta_t: %d, Now_t: %d, EFUSE_t: %d, Last_t: %d\n",
                //      pDM_Odm->RFCalibrateInfo.PowerIndexOffset, delta, ThermalValue, pHalData->EEPROMThermalMeter, pDM_Odm->RFCalibrateInfo.ThermalValue);               
            }
            if (ThermalValue > pHalData->EEPROMThermalMeter)
            {
//              ARS_TP_INFO("Temperature(%d) hugher than PG value(%d), increases the power by TxAGC\n", ThermalValue, pHalData->EEPROMThermalMeter);
                (*c.ODM_TxPwrTrackSetPwr)(pDM_Odm, TXAGC, 0, 0);                            
            }
            else
            {
    //          ARS_TP_INFO("Temperature(%d) lower than PG value(%d), increases the power by TxAGC\n", ThermalValue, pHalData->EEPROMThermalMeter);
                (*c.ODM_TxPwrTrackSetPwr)(pDM_Odm, BBSWING, RF_PATH_A, Indexforchannel);    
                if(is2T)
                    (*c.ODM_TxPwrTrackSetPwr)(pDM_Odm, BBSWING, RF_PATH_B, Indexforchannel);                
            }
            
            pRFCalibrateInfo->BbSwingIdxCckBase = pRFCalibrateInfo->BbSwingIdxCck;
            pRFCalibrateInfo->BbSwingIdxOfdmBase = pRFCalibrateInfo->BbSwingIdxOfdm[RF_PATH_A];
            pDM_Odm->RFCalibrateInfo.ThermalValue = ThermalValue;

    }
        
    // if((delta_IQK > pHalData->Delta_IQK) && (pHalData->Delta_IQK != 0))
    if ((delta_IQK >= 8)) // Delta temperature is equal to or larger than 20 centigrade.
        (*c.DoIQK)(pDM_Odm, delta_IQK, ThermalValue, 8);
            
    ARS_TP_INFO("<===dm_TXPowerTrackingCallback_ThermalMeter_8188E\n");
    
    pDM_Odm->RFCalibrateInfo.TXPowercount = 0;
}
#else
void ODM_TXPowerTrackingCallback_ThermalMeter(ars_st *ars)
{
}
#endif

wf_s32 odm_TXPowerTrackingCheckCE(void *ars)
{
    ars_st *pars = ars;
    
    if (!(pars->SupportAbility & ODM_RF_TX_PWR_TRACK))
    {
        return WF_RETURN_OK;
    }

    ARS_TP_DBG();
    if(!pars->RFCalibrateInfo.TM_Trigger)        //at least delay 1 sec
    {
      
        hw_write_rf_reg(pars->nic_info, ODM_RF_PATH_A, RF_T_METER_NEW, (BIT(17) | BIT(16)), 0x03);
        ARS_TP_PRT("Trigger Thermal Meter!!\n");
        pars->RFCalibrateInfo.TM_Trigger = 1;
        
    }
    else
    {
        ARS_TP_PRT("Schedule TxPowerTracking direct call!!\n");
        ODM_TXPowerTrackingCallback_ThermalMeter(pars);
        pars->RFCalibrateInfo.TM_Trigger = 0;
    }

    return WF_RETURN_OK;
}


wf_s32 odm_DynamicTxPowerInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_TP_ERR("input param is null");
        return WF_RETURN_FAIL;
    }

    ARS_TP_INFO("start");
    pars = ars;

    return WF_RETURN_OK;
}


#endif

