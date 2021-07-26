#ifndef __ARS_ADAPTIVITY_H__
#define __ARS_ADAPTIVITY_H__

#define PwdBUpperBound	7
#define DFIRloss	5

typedef enum tag_PhyDM_MACEDCCA_Type
{
    PhyDM_IGNORE_EDCCA      = 0,
    PhyDM_DONT_IGNORE_EDCCA = 1
}PhyDM_MACEDCCA_Type;


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

wf_s32 Phydm_Adaptivity(void *ars,wf_u8 IGI);
wf_s32 Phydm_CheckAdaptivity(void *ars);
wf_s32 Phydm_NHMCounterStatisticsInit(void *ars);
wf_s32 Phydm_AdaptivityInit(void *ars);

#endif
