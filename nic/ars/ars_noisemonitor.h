#ifndef __ARS_NOISE_MONITOR_H__
#define __ARS_NOISE_MONITOR_H__

#define MAX_RF_PATH 4
typedef struct _ODM_NOISE_MONITOR_
{
    wf_s8           noise[MAX_RF_PATH];
    wf_s16          noise_all;  
}ODM_NOISE_MONITOR;

typedef struct nm_info_st_
{
}ars_nm_info_st;
#endif
