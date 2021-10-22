
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

#ifdef CONFIG_ARS_DRIVER_SUPPORT

#define ARS_PD_DBG(fmt, ...)        LOG_D("ARS_PD[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_PD_PRT(fmt, ...)        LOG_D("ARS_PD-"fmt,##__VA_ARGS__)
#define ARS_PD_INFO(fmt, ...)       LOG_I("ARS_PD-"fmt,##__VA_ARGS__)
#define ARS_PD_ERR(fmt, ...)        LOG_E("ARS_PD-"fmt,##__VA_ARGS__)


wf_s32 odm_PathDiversity(void *ars)
{
    return WF_RETURN_OK;
}

wf_s32 odm_PathDiversityInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_PD_ERR("input param is null");
        return WF_RETURN_FAIL;
    }
    ARS_PD_INFO("start");
    pars = ars;

    return WF_RETURN_OK;
}


#endif

