#ifndef __ARS_ADAPTIVITY_H__
#define __ARS_ADAPTIVITY_H__

typedef struct _ADAPTIVITY_STATISTICS 
{
    wf_s8           TH_L2H_ini_backup;
    wf_s8           TH_EDCCA_HL_diff_backup;
    wf_s8           IGI_Base;
    wf_u8           IGI_target;
    wf_u8           NHMWait;
    wf_s8           H2L_lb;
    wf_s8           L2H_lb;
    wf_bool         bFirstLink;
    wf_bool         bCheck;
    wf_bool         DynamicLinkAdaptivity;
    wf_u8           APNumTH;
    wf_u8           AdajustIGILevel;
} ADAPTIVITY_STATISTICS, *PADAPTIVITY_STATISTICS;

typedef struct adaptivity_statistics_info_st_
{
    
}adaptivity_statistics_info_st;

void Phydm_Adaptivity(void *ars,wf_u8 IGI);
void Phydm_CheckAdaptivity(void *ars);
void Phydm_NHMCounterStatisticsInit(void *ars);


#endif
