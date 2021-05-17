#ifndef __ARS_RAINFO_H__
#define __ARS_RAINFO_H__

#define     DM_RATR_STA_INIT            0
#define     DM_RATR_STA_HIGH            1
#define     DM_RATR_STA_MIDDLE          2
#define     DM_RATR_STA_LOW             3

#define HIGH_RSSI_THRESH	50
#define LOW_RSSI_THRESH	20

#define	ACTIVE_TP_THRESHOLD	150
#define	RA_RETRY_DESCEND_NUM	2
#define	RA_RETRY_LIMIT_LOW	4
#define	RA_RETRY_LIMIT_HIGH	32

#define PHYDM_IC_8051_SERIES		(ODM_RTL8881A|ODM_RTL8812|ODM_RTL8821|ODM_RTL8192S|ODM_RTL8192C|ODM_RTL8192D|ODM_RTL8723A|ODM_RTL8188E|ODM_RTL8192E|ODM_RTL8723B|ODM_RTL8703B|ODM_RTL8188F)
#define PHYDM_IC_3081_SERIES		(ODM_RTL8814A|ODM_RTL8821B|ODM_RTL8822B)

#define RAINFO_BE_RX_STATE			BIT(0)  // 1:RX    //ULDL
#define RAINFO_STBC_STATE			BIT(1)
//#define RAINFO_LDPC_STATE 			BIT2
#define RAINFO_NOISY_STATE 			BIT(2)    // set by Noisy_Detection
#define RAINFO_SHURTCUT_STATE 		BIT(3)
#define RAINFO_SHURTCUT_FLAG 		BIT(4)
#define RAINFO_INIT_RSSI_RATE_STATE  BIT(5)
#define RAINFO_BF_STATE 				BIT(6)
#define RAINFO_BE_TX_STATE 			BIT(7) // 1:TX

#define	RA_MASK_CCK		0xf
#define	RA_MASK_OFDM		0xff0
#define	RA_MASK_HT1SS		0xff000
#define	RA_MASK_HT2SS		0xff00000
/*#define	RA_MASK_MCS3SS	*/
#define	RA_MASK_HT4SS		0xff0
#define	RA_MASK_VHT1SS	0x3ff000
#define	RA_MASK_VHT2SS	0xffc00000
#define RA_FIRST_MACID 	0



typedef struct _Rate_Adaptive_Table_ 
{
    wf_u8       firstconnect;

#if (defined(CONFIG_RA_DBG_CMD))
    wf_bool     is_ra_dbg_init;

    wf_u8   RTY_P[ODM_NUM_RATE_IDX];
    wf_u8   RTY_P_default[ODM_NUM_RATE_IDX];
    wf_bool RTY_P_modify_note[ODM_NUM_RATE_IDX];

    wf_u8   RATE_UP_RTY_RATIO[ODM_NUM_RATE_IDX];
    wf_u8   RATE_UP_RTY_RATIO_default[ODM_NUM_RATE_IDX];
    wf_bool RATE_UP_RTY_RATIO_modify_note[ODM_NUM_RATE_IDX];

    wf_u8   RATE_DOWN_RTY_RATIO[ODM_NUM_RATE_IDX];
    wf_u8   RATE_DOWN_RTY_RATIO_default[ODM_NUM_RATE_IDX];
    wf_bool RATE_DOWN_RTY_RATIO_modify_note[ODM_NUM_RATE_IDX];

    wf_bool RA_Para_feedback_req;

    wf_u8   para_idx;
    wf_u8   rate_idx;
    wf_u8   value;
    wf_u16  value_16;
    wf_u8   rate_length;
#endif
    wf_u8   link_tx_rate[ODM_ASSOCIATE_ENTRY_NUM];

    #if (defined(CONFIG_RA_DYNAMIC_RTY_LIMIT))
    wf_u8 per_rate_retrylimit_20M[ODM_NUM_RATE_IDX];
    wf_u8 per_rate_retrylimit_40M[ODM_NUM_RATE_IDX];    
    wf_u8           retry_descend_num;
    wf_u8           retrylimit_low;
    wf_u8           retrylimit_high;
    #endif


} RA_T, *pRA_T;
    

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

    RA_T ra_table;

    wf_bool ra_rpt_linked;
}ars_ra_info_st;

wf_s32 phydm_ra_dynamic_retry_limit(void *pars);
wf_s32 phydm_ra_dynamic_retry_count(void *ars);
wf_s32 odm_RefreshRateAdaptiveMask(void *ars);
wf_s32 odm_RSSIMonitorCheck(void *ars);
wf_s32 phydm_ra_info_init(void *ars);
wf_s32 odm_RateAdaptiveMaskInit(void *ars);
wf_s32 odm_RA_ParaAdjust_init(void*ars);
wf_s32 odm_RSSIMonitorInit(void *ars);
wf_s8 phydm_rssi_report(void *ars, wdn_net_info_st *pwdn);

#endif

