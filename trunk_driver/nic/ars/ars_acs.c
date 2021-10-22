
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

#ifdef CONFIG_ARS_DRIVER_SUPPORT
#define ARS_ACS_DBG(fmt, ...)       LOG_D("ARS_ACS[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_ACS_PRT(fmt, ...)       LOG_D("ARS_ACS-"fmt,##__VA_ARGS__)
#define ARS_ACS_INFO(fmt, ...)      LOG_I("ARS_ACS-"fmt,##__VA_ARGS__)
#define ARS_ACS_ERR(fmt, ...)       LOG_E("ARS_ACS-"fmt,##__VA_ARGS__)

wf_s32 odm_AutoChannelSelectInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_ACS_ERR("input param is null");
        return WF_RETURN_FAIL;
    }
    ARS_ACS_INFO("start");

    pars = ars;

    return WF_RETURN_OK;
}


#endif

