#ifndef __ARS_CFO_H__
#define __ARS_CFO_H__

typedef struct _CFO_TRACKING_
{
    wf_bool         bATCStatus;
    wf_bool         largeCFOHit;
    wf_bool         bAdjust;
    wf_u8           CrystalCap;
    wf_u8           DefXCap;
    wf_s32              CFO_tail[2];
    wf_s32              CFO_ave_pre;
    wf_u32          packetCount;
    wf_u32          packetCount_pre;

    wf_bool         bForceXtalCap;
    wf_bool         bReset;
}CFO_TRACKING;
    

typedef struct cfo_info_st_
{
    CFO_TRACKING cfo_tr;
}cfo_info_st;


void ODM_ParsingCFO(void *pars, void *pkt_info,wf_s8 *pcfotail );

#endif
