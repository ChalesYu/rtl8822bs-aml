#ifndef __ARS_RAINFO_H__
#define __ARS_RAINFO_H__

typedef struct ra_info_st_
{
    wf_u8               Type;               // DM_Type_ByFW/DM_Type_ByDriver
    wf_u8               HighRSSIThresh;     // if RSSI > HighRSSIThresh => RATRState is DM_RATR_STA_HIGH
    wf_u8               LowRSSIThresh;      // if RSSI <= LowRSSIThresh => RATRState is DM_RATR_STA_LOW
    wf_u8               RATRState;          // Current RSSI level, DM_RATR_STA_HIGH/DM_RATR_STA_MIDDLE/DM_RATR_STA_LOW

    wf_u8               LdpcThres;          // if RSSI > LdpcThres => switch from LPDC to BCC
    wf_bool             bLowerRtsRate;

    wf_bool             bUseLdpc;
    wf_u8               UltraLowRSSIThresh;
    wf_u32              LastRATR;           // RATR Register Content

    wf_u8               bUseRAMask;
}ars_ra_info_st;

void phydm_ra_dynamic_retry_limit(void *pars);
void phydm_ra_dynamic_retry_count(void *ars);
void odm_RefreshRateAdaptiveMask(void *ars);
void odm_RSSIMonitorCheck(void *ars);

#endif

