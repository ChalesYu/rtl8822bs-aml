#ifndef __ARS_BBPS_H__
#define __ARS_BBPS_H__

typedef enum tag_1R_CCA_Type_Definition
{
    CCA_1R =0,
    CCA_2R = 1,
    CCA_MAX = 2,
}DM_1R_CCA_E;

typedef enum tag_RF_Type_Definition
{
    RF_Save =0,
    RF_Normal = 1,
    RF_MAX = 2,
}DM_RF_E;
    
typedef struct bb_powersave_info_st_
{
    wf_u8      PreCCAState;
    wf_u8      CurCCAState;

    wf_u8      PreRFState;
    wf_u8      CurRFState;

    wf_s32         Rssi_val_min;

    wf_u8      initialize;
    wf_u32      Reg874,RegC70,Reg85C,RegA74;
}ars_bbps_info_st;

wf_s32 odm_DynamicBBPowerSaving(void *ars);
wf_s32 odm_DynamicBBPowerSavingInit(void *ars);
#endif