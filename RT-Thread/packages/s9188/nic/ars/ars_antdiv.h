#ifndef __ARS_ANTDIV_H__
#define __ARS_ANTDIV_H__

typedef enum _ANT_DIV_TYPE
{
    CG_TRX_HW_ANTDIV        = 0x01,
    CGCS_RX_HW_ANTDIV       = 0x02,
    CG_TRX_SMART_ANTDIV     = 0x03,
    S0S1_SW_ANTDIV          = 0x04, /*8723B intrnal switch S0 S1*/
    HL_SW_SMART_ANT_TYPE1   = 0x10 /*Hong-Lin Smart antenna use for 8821AE which is a 2 Ant. entitys, and each Ant. is equipped with 4 antenna patterns*/
}ANT_DIV_TYPE_E, *PANT_DIV_TYPE_E;

typedef struct _SW_Antenna_Switch_
{
    wf_u8       Double_chk_flag;    /*If current antenna RSSI > "RSSI_CHECK_THRESHOLD", than check this antenna again*/
    wf_u8       try_flag;
    wf_s32      PreRSSI;
    wf_u8       CurAntenna;
    wf_u8       PreAntenna;
    wf_u8       RSSI_Trying;
    wf_u8       reset_idx;
    wf_u8       Train_time;
    wf_u8       Train_time_flag; /*base on RSSI difference between two antennas*/
    //RT_TIMER  phydm_SwAntennaSwitchTimer;
    wf_u32      PktCnt_SWAntDivByCtrlFrame;
    wf_bool     bSWAntDivByCtrlFrame;

    /* AntDect (Before link Antenna Switch check) need to be moved*/
    wf_u16      Single_Ant_Counter;
    wf_u16      Dual_Ant_Counter;
    wf_u16      Aux_FailDetec_Counter;
    wf_u16      Retry_Counter;  
    wf_u8       SWAS_NoLink_State;
    wf_u32      SWAS_NoLink_BK_Reg948;
    wf_bool     ANTA_ON;    /*To indicate Ant A is or not*/
    wf_bool     ANTB_ON;    /*To indicate Ant B is on or not*/
    wf_bool     Pre_Aux_FailDetec;
    wf_bool     RSSI_AntDect_bResult;   
    wf_u8       Ant5G;
    wf_u8       Ant2G;

    
}SWAT_T, *pSWAT_T;


typedef struct antdiv_info_st_
{
	wf_u8 reserved;
}ars_antdiv_info_st;

wf_s32 odm_AntennaDiversity(void *ars);
wf_s32 odm_AntennaDiversityInit(void *ars);
#endif