#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_DRIVER_SUPPORT

#if 0
#define ARS_BBPS_DBG(fmt, ...)      LOG_D("ARS_BBPS[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_BBPS_PRT(fmt, ...)      LOG_D("ARS_BBPS-"fmt,##__VA_ARGS__)

#else
#define ARS_BBPS_DBG(fmt, ...)
#define ARS_BBPS_PRT(fmt, ...) 
#endif

#define ARS_BBPS_INFO(fmt, ...)      LOG_I("ARS_BBPS-"fmt,##__VA_ARGS__)
#define ARS_BBPS_ERR(fmt, ...)      LOG_E("ARS_BBPS-"fmt,##__VA_ARGS__)

wf_s32 odm_DynamicBBPowerSaving(void *ars)
{   
    ars_st*     pars = NULL;

    #if 0
    if (pars->SupportICType != ODM_RTL8723A)
        return;
    if(!(pars->SupportAbility & ODM_BB_PWR_SAVE))
        return;
    if(!(pars->SupportPlatform & (ODM_WIN|ODM_CE)))
        return;
    
    //1 2.Power Saving for 92C
    if((pars->SupportICType == ODM_RTL8192C) &&(pars->RFType == ODM_2T2R))
    {
        odm_1R_CCA(pars);
    }
    
    // 20100628 Joseph: Turn off BB power save for 88CE because it makesthroughput unstable.
    // 20100831 Joseph: Turn ON BB power save again after modifying AGC delay from 900ns ot 600ns.
    //1 3.Power Saving for 88C
    else
    {
        ODM_RF_Saving(pars, FALSE);
    }
    #else
    //to do
    #endif
    pars = ars;
    
    return 0;
}

wf_s32 odm_DynamicBBPowerSavingInit(void *ars)
{
    ars_st *pars = NULL;
    ars_bbps_info_st *bbps = NULL;
    if(NULL == ars)
    {
        ARS_BBPS_ERR("input param is null");
        return WF_RETURN_FAIL;
    }

    ARS_BBPS_INFO("start");
    pars = ars;
    bbps = &pars->bbps;
    
    bbps->PreCCAState = CCA_MAX;
    bbps->CurCCAState = CCA_MAX;
    bbps->PreRFState = RF_MAX;
    bbps->CurRFState = RF_MAX;
    bbps->Rssi_val_min = 0;
    bbps->initialize = 0;

    return WF_RETURN_OK;
}

#endif

