#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT



void odm_DynamicBBPowerSaving(void *ars)
{   
    ars_st*     pars = ars;

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
}


#endif

