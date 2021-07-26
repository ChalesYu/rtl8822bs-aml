#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_DRIVER_SUPPORT

#if 0
#define ARS_AD_DBG(fmt, ...)      LOG_D("ARS_ANTDIV[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_AD_PRT(fmt, ...)      LOG_D("ARS_ANTDIV-"fmt,##__VA_ARGS__)

#else
#define ARS_AD_DBG(fmt, ...)
#define ARS_AD_PRT(fmt, ...) 
#endif

#define ARS_AD_INFO(fmt, ...)      LOG_I("ARS_ANTDIV-"fmt,##__VA_ARGS__)
#define ARS_AD_ERR(fmt, ...)      LOG_E("ARS_ANTDIV-"fmt,##__VA_ARGS__)


wf_s32 odm_AntennaDiversity(void *ars)
{
#if 0
    PDM_ODM_T       pDM_Odm = (PDM_ODM_T)pDM_VOID;
    if(pDM_Odm->mp_mode == TRUE)
        return;

#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
        ODM_AntDiv(pDM_Odm);
#endif
#endif

    return WF_RETURN_OK;
}

wf_s32 odm_AntennaDiversityInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_AD_DBG("input param is null");
        return WF_RETURN_FAIL;
    }

    ARS_AD_INFO("start");
    pars = ars;

    if(pars->mp_mode == wf_true)
    {
        return WF_RETURN_OK;
    }
    
    #if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
        ODM_AntDiv_Config(pDM_Odm);
        ODM_AntDivInit(pDM_Odm);
    #endif

    return WF_RETURN_OK;
}


#endif

