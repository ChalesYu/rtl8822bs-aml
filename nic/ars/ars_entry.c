#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT
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
   
    wf_kfree(pars);
    pars = NULL;

    return 0;
}

#endif