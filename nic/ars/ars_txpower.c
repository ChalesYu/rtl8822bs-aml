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

