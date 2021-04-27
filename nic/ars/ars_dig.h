#ifndef __ARS_DIG_H__
#define __ARS_DIG_H__

/* Pause DIG & CCKPD */
#define     DM_DIG_MAX_PAUSE_TYPE       0x7
typedef struct _FALSE_ALARM_STATISTICS
{
    wf_u32  Cnt_Parity_Fail;
    wf_u32  Cnt_Rate_Illegal;
    wf_u32  Cnt_Crc8_fail;
    wf_u32  Cnt_Mcs_fail;
    wf_u32  Cnt_Ofdm_fail;
    wf_u32  Cnt_Ofdm_fail_pre;  //For RTL8881A
    wf_u32  Cnt_Cck_fail;
    wf_u32  Cnt_all;
    wf_u32  Cnt_Fast_Fsync;
    wf_u32  Cnt_SB_Search_fail;
    wf_u32  Cnt_OFDM_CCA;
    wf_u32  Cnt_CCK_CCA;
    wf_u32  Cnt_CCA_all;
    wf_u32  Cnt_BW_USC; //Gary
    wf_u32  Cnt_BW_LSC; //Gary
}FALSE_ALARM_STATISTICS, *PFALSE_ALARM_STATISTICS;

typedef enum tag_Dynamic_Init_Gain_Operation_Type_Definition
{
    DIG_TYPE_THRESH_HIGH    = 0,
    DIG_TYPE_THRESH_LOW = 1,
    DIG_TYPE_BACKOFF        = 2,
    DIG_TYPE_RX_GAIN_MIN    = 3,
    DIG_TYPE_RX_GAIN_MAX    = 4,
    DIG_TYPE_ENABLE         = 5,
    DIG_TYPE_DISABLE        = 6,    
    DIG_OP_TYPE_MAX
}DM_DIG_OP_E;


typedef enum tag_PHYDM_Pause_Type 
{
    PHYDM_PAUSE = BIT(0),
    PHYDM_RESUME = BIT(1)
} PHYDM_PAUSE_TYPE;

typedef enum tag_PHYDM_Pause_Level 
{
/* number of pause level can't exceed DM_DIG_MAX_PAUSE_TYPE */
    PHYDM_PAUSE_LEVEL_0 = 0,
    PHYDM_PAUSE_LEVEL_1 = 1,
    PHYDM_PAUSE_LEVEL_2 = 2,
    PHYDM_PAUSE_LEVEL_3 = 3,
    PHYDM_PAUSE_LEVEL_4 = 4,
    PHYDM_PAUSE_LEVEL_5 = 5,
    PHYDM_PAUSE_LEVEL_6 = 6,
    PHYDM_PAUSE_LEVEL_7 = DM_DIG_MAX_PAUSE_TYPE     /* maximum level */
} PHYDM_PAUSE_LEVEL;


#define     DM_DIG_THRESH_HIGH          40
#define     DM_DIG_THRESH_LOW           35

#define     DM_FALSEALARM_THRESH_LOW    400
#define     DM_FALSEALARM_THRESH_HIGH   1000

#define     DM_DIG_MAX_NIC              0x3e
#define     DM_DIG_MIN_NIC              0x1e //0x22//0x1c
#define     DM_DIG_MAX_OF_MIN_NIC       0x3e

#define     DM_DIG_MAX_AP                   0x3e
#define     DM_DIG_MIN_AP                   0x1c
#define     DM_DIG_MAX_OF_MIN           0x2A    //0x32
#define     DM_DIG_MIN_AP_DFS               0x20

#define     DM_DIG_MAX_NIC_HP           0x46
#define     DM_DIG_MIN_NIC_HP               0x2e

#define     DM_DIG_MAX_AP_HP                0x42
#define     DM_DIG_MIN_AP_HP                0x30


//vivi 92c&92d has different definition, 20110504
//this is for 92c
#ifdef CONFIG_SPECIAL_SETTING_FOR_FUNAI_TV
#define     DM_DIG_FA_TH0               0x80//0x20
#else
#define     DM_DIG_FA_TH0               0x200//0x20
#endif

#define     DM_DIG_FA_TH1                   0x300
#define     DM_DIG_FA_TH2                   0x400
//this is for 92d
#define     DM_DIG_FA_TH0_92D               0x100
#define     DM_DIG_FA_TH1_92D               0x400
#define     DM_DIG_FA_TH2_92D               0x600

#define     DM_DIG_BACKOFF_MAX          12
#define     DM_DIG_BACKOFF_MIN          -4
#define     DM_DIG_BACKOFF_DEFAULT      10

#define         DM_DIG_FA_TH0_LPS               4 //-> 4 in lps
#define         DM_DIG_FA_TH1_LPS               15 //-> 15 lps
#define         DM_DIG_FA_TH2_LPS               30 //-> 30 lps
#define         RSSI_OFFSET_DIG             0x05


typedef struct dig_info_st_
{
    wf_bool    bDMInitialGainEnable;
    wf_bool     bStopDIG;       // for debug
    wf_bool     bIgnoreDIG;
    wf_bool     bPSDInProgress;

    wf_u8       Dig_Enable_Flag;
    wf_u8       Dig_Ext_Port_Stage;
    
    int         RssiLowThresh;
    int             RssiHighThresh;

    wf_u32      FALowThresh;
    wf_u32      FAHighThresh;

    wf_u8       CurSTAConnectState;
    wf_u8       PreSTAConnectState;
    wf_u8       CurMultiSTAConnectState;

    wf_u8       PreIGValue;
    wf_u8       CurIGValue;
    wf_u8       BackupIGValue;      //MP DIG
    wf_u8       BT30_CurIGI;
    wf_u8       IGIBackup;

    wf_s8       BackoffVal;
    wf_s8       BackoffVal_range_max;
    wf_s8       BackoffVal_range_min;
    wf_u8       rx_gain_range_max;
    wf_u8       rx_gain_range_min;
    wf_u8       Rssi_val_min;

    wf_u8       PreCCK_CCAThres;
    wf_u8       CurCCK_CCAThres;
    wf_u8       PreCCKPDState;
    wf_u8       CurCCKPDState;
    wf_u8       CCKPDBackup;
    wf_u8       pause_cckpd_level;
    wf_u8       pause_cckpd_value[DM_DIG_MAX_PAUSE_TYPE + 1];

    wf_u8       LargeFAHit;
    wf_u8       ForbiddenIGI;
    wf_u32      Recover_cnt;

    wf_u8       DIG_Dynamic_MIN_0;
    wf_u8       DIG_Dynamic_MIN_1;
    wf_bool     bMediaConnect_0;
    wf_bool     bMediaConnect_1;

    wf_u32      AntDiv_RSSI_max;
    wf_u32      RSSI_max;

    wf_u8       *bP2PInProcess;

    wf_u8       pause_dig_level;
    wf_u8       pause_dig_value[DM_DIG_MAX_PAUSE_TYPE + 1];
}ars_dig_info_st;

void ars_dig_init(void *ars);

void odm_CCKPacketDetectionThresh(void *ars    );
void odm_FalseAlarmCounterStatistics(void *pars);
void odm_DIGbyRSSI_LPS(void *ars);
void odm_DIG(void *ars);


#endif
