#ifndef __ARS_ADAPTIVITY_H__
#define __ARS_ADAPTIVITY_H__

#define PwdBUpperBound	7
#define DFIRloss	5

typedef enum ODM_SUPPORT_ABILITY_ENUM_
{
    ODM_BB_DIG = WF_BIT(0),
    ODM_BB_RA_MASK = WF_BIT(1),
    ODM_BB_DYNAMIC_TXPWR = WF_BIT(2), //use
    ODM_BB_FA_CNT = WF_BIT(3),
    ODM_BB_RSSI_MONITOR = WF_BIT(4),
    ODM_BB_CCK_PD = WF_BIT(5),
    ODM_BB_ANT_DIV = WF_BIT(6),
    ODM_BB_PWR_SAVE = WF_BIT(7),
    ODM_BB_PWR_TRAIN = WF_BIT(8),
    ODM_BB_RATE_ADAPTIVE = WF_BIT(9),
    ODM_BB_PATH_DIV = WF_BIT(10),
    ODM_BB_PSD = WF_BIT(11),
    ODM_BB_RXHP = WF_BIT(12),
    ODM_BB_ADAPTIVITY = WF_BIT(13),
    ODM_BB_CFO_TRACKING = WF_BIT(14),
    ODM_BB_NHM_CNT = WF_BIT(15),   //use
    ODM_BB_PRIMARY_CCA = WF_BIT(16),
    ODM_BB_TXBF = WF_BIT(17),

    ODM_MAC_EDCA_TURBO = WF_BIT(20),
    ODM_MAC_EARLY_MODE = WF_BIT(21),

    ODM_RF_TX_PWR_TRACK = WF_BIT(24),
    ODM_RF_RX_GAIN_TRACK = WF_BIT(25),
    ODM_RF_CALIBRATION = WF_BIT(26),

} ODM_SUPPORT_ABILITY_ENUM;
    

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
