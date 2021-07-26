#ifndef __ARS_POWERTRACKING_H__
#define __ARS_POWERTRACKING_H__

#define DPK_DELTA_MAPPING_NUM   13
#define index_mapping_HP_NUM    15  
#define OFDM_TABLE_SIZE     43
#define CCK_TABLE_SIZE          33
#define CCK_TABLE_SIZE_88F  21
#define TXSCALE_TABLE_SIZE      37
#define TXPWR_TRACK_TABLE_SIZE  30
#define DELTA_SWINGIDX_SIZE     30
#define BAND_NUM                4

#define AVG_THERMAL_NUM     8
#define HP_THERMAL_NUM      8
#define IQK_MAC_REG_NUM     4
#define IQK_ADDA_REG_NUM        16
#define IQK_BB_REG_NUM_MAX  10

#define IQK_BB_REG_NUM      9

#define IQK_Matrix_REG_NUM  8
#define IQK_Matrix_Settings_NUM	14+24+21 // Channels_2_4G_NUM + Channels_5G_20M_NUM + Channels_5G


typedef struct _IQK_MATRIX_REGS_SETTING
{
    wf_bool     bIQKDone;
    wf_s32      Value[3][IQK_Matrix_REG_NUM];
    wf_bool     bBWIqkResultSaved[3];   
}IQK_MATRIX_REGS_SETTING,*PIQK_MATRIX_REGS_SETTING;

typedef struct ODM_RF_Calibration_Structure
{
    //for tx power tracking
    
    wf_u32  RegA24; // for TempCCK
    wf_s32  RegE94;
    wf_s32  RegE9C;
    wf_s32  RegEB4;
    wf_s32  RegEBC; 

    wf_u8      TXPowercount;
    wf_bool bTXPowerTrackingInit; 
    wf_bool bTXPowerTracking;
    wf_u8      TxPowerTrackControl; //for mp mode, turn off txpwrtracking as default
    wf_u8      TM_Trigger;
    wf_u8      InternalPA5G[2];    //pathA / pathB
    
    wf_u8      ThermalMeter[2];    // ThermalMeter, index 0 for RFIC0, and 1 for RFIC1
    wf_u8      ThermalValue;
    wf_u8      ThermalValue_LCK;
    wf_u8      ThermalValue_IQK;
    wf_u8  ThermalValue_DPK;       
    wf_u8  ThermalValue_AVG[AVG_THERMAL_NUM];
    wf_u8  ThermalValue_AVG_index;     
    wf_u8  ThermalValue_RxGain;
    wf_u8  ThermalValue_Crystal;
    wf_u8  ThermalValue_DPKstore;
    wf_u8  ThermalValue_DPKtrack;
    wf_bool TxPowerTrackingInProgress;
    
    wf_bool bReloadtxpowerindex;    
    wf_u8  bRfPiEnable;
    wf_u32  TXPowerTrackingCallbackCnt; //cosa add for debug


    //------------------------- Tx power Tracking -------------------------//
    wf_u8  bCCKinCH14;
    wf_u8  CCK_index;
    wf_u8  OFDM_index[MAX_RF_PATH];
    wf_s8  PowerIndexOffset[MAX_RF_PATH];
    wf_s8  DeltaPowerIndex[MAX_RF_PATH];
    wf_s8  DeltaPowerIndexLast[MAX_RF_PATH];   
    wf_bool bTxPowerChanged;
        
    wf_u8  ThermalValue_HP[HP_THERMAL_NUM];
    wf_u8  ThermalValue_HP_index;
    IQK_MATRIX_REGS_SETTING IQKMatrixRegSetting[IQK_Matrix_Settings_NUM];
    wf_u8  Delta_LCK;
    wf_s8  BBSwingDiff2G, BBSwingDiff5G; // Unit: dB
    wf_u8  DeltaSwingTableIdx_2GCCKA_P[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GCCKA_N[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GCCKB_P[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GCCKB_N[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GCCKC_P[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GCCKC_N[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GCCKD_P[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GCCKD_N[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GA_P[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GA_N[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GB_P[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GB_N[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GC_P[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GC_N[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GD_P[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GD_N[DELTA_SWINGIDX_SIZE];
   
    wf_u8  DeltaSwingTableIdx_2GA_P_8188E[DELTA_SWINGIDX_SIZE];
    wf_u8  DeltaSwingTableIdx_2GA_N_8188E[DELTA_SWINGIDX_SIZE];
    
    wf_u8          BbSwingIdxOfdm[MAX_RF_PATH];
    wf_u8          BbSwingIdxOfdmCurrent;
    wf_u8          BbSwingIdxOfdmBase[MAX_RF_PATH];

    wf_bool         BbSwingFlagOfdm;
    wf_u8          BbSwingIdxCck;
    wf_u8          BbSwingIdxCckCurrent;
    wf_u8          BbSwingIdxCckBase;
    wf_u8          DefaultOfdmIndex;
    wf_u8          DefaultCckIndex;    
    wf_bool         BbSwingFlagCck;
    
    wf_s8          Absolute_OFDMSwingIdx[MAX_RF_PATH];   
    wf_s8          Remnant_OFDMSwingIdx[MAX_RF_PATH];   
    wf_s8          Remnant_CCKSwingIdx;
    wf_s8          Modify_TxAGC_Value;       /*Remnat compensate value at TxAGC */
    wf_bool         Modify_TxAGC_Flag_PathA;
    wf_bool         Modify_TxAGC_Flag_PathB;
    wf_bool         Modify_TxAGC_Flag_PathC;
    wf_bool         Modify_TxAGC_Flag_PathD;
    wf_bool         Modify_TxAGC_Flag_PathA_CCK;
    
    wf_s8          KfreeOffset[MAX_RF_PATH];
    
    //--------------------------------------------------------------------//    
    
    //for IQK   
    wf_u32  RegC04;
    wf_u32  Reg874;
    wf_u32  RegC08;
    wf_u32  RegB68;
    wf_u32  RegB6C;
    wf_u32  Reg870;
    wf_u32  Reg860;
    wf_u32  Reg864;
    
    wf_bool bIQKInitialized;
    wf_bool bLCKInProgress;
    wf_bool bAntennaDetected;
    wf_bool bNeedIQK;
    wf_bool bIQKInProgress; 
    wf_u8  Delta_IQK;
    wf_u32  ADDA_backup[IQK_ADDA_REG_NUM];
    wf_u32  IQK_MAC_backup[IQK_MAC_REG_NUM];
    wf_u32  IQK_BB_backup_recover[9];
    wf_u32  IQK_BB_backup[IQK_BB_REG_NUM];  
    wf_u32  TxIQC_8723B[2][3][2]; // { {S1: 0xc94, 0xc80, 0xc4c} , {S0: 0xc9c, 0xc88, 0xc4c}}
    wf_u32  RxIQC_8723B[2][2][2]; // { {S1: 0xc14, 0xca0} ,           {S0: 0xc14, 0xca0}}
    wf_u32  TxIQC_8703B[3][2];  /* { {S1: 0xc94, 0xc80, 0xc4c} , {S0: 0xc9c, 0xc88, 0xc4c}}*/
    wf_u32  RxIQC_8703B[2][2];  /* { {S1: 0xc14, 0xca0} ,           {S0: 0xc14, 0xca0}}*/

    

    // <James> IQK time measurement 
    wf_u64  IQK_StartTime;
    wf_u64  IQK_ProgressingTime;
    wf_u32  LOK_Result;

    //for APK
    wf_u32  APKoutput[2][2]; //path A/B; output1_1a/output1_2a
    wf_u8  bAPKdone;
    wf_u8  bAPKThermalMeterIgnore;
    
    // DPK
    wf_bool bDPKFail;   
    wf_u8  bDPdone;
    wf_u8  bDPPathAOK;
    wf_u8  bDPPathBOK;

    wf_u32  TxLOK[2];
    wf_u32  DpkTxAGC;
    wf_s32  DpkGain;
    wf_u32  DpkThermal[4];
    wf_s8 Modify_TxAGC_Value_OFDM;
    wf_s8 Modify_TxAGC_Value_CCK;
}ODM_RF_CAL_T,*PODM_RF_CAL_T;

wf_s32 phydm_rf_init(void *ars);
wf_s32 odm_TXPowerTrackingInit(void *ars);

#endif
