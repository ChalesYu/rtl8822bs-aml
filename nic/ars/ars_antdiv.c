#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT

void odm_AntennaDiversity(void *ars)
{
#if 0
    PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
    if(pDM_Odm->mp_mode == TRUE)
    	return;

#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
    	ODM_AntDiv(pDM_Odm);
#endif
#endif
}

#endif

