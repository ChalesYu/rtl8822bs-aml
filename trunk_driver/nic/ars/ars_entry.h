#ifndef __ARS_ENTRY_H__
#define __ARS_ENTRY_H__

#ifdef CONFIG_ARS_DRIVER_SUPPORT


#define  ARS_INIT_WRITE_REG_EN  0

#define     DM_Type_ByFW            0
#define     DM_Type_ByDriver        1

#define ODM_ASSOCIATE_ENTRY_NUM (32)

#include "ars_define.h"
#include "ars_reg.h"
#include "ars_acs.h"
#include "ars_noisemonitor.h"
#include "ars_pathdiv.h"
#include "ars_rainfo.h"
#include "ars_antdect.h"
#include "ars_antdiv.h"
#include "ars_bbps.h"
#include "ars_beamforming.h"
#include "ars_cfo.h"
#include "ars_edcaturbocheck.h"
#include "ars_txpower.h"
#include "ars_dig.h"
#include "ars_hwconfig.h"
#include "ars_debug.h"
#include "ars_adaptivity.h"
#include "ars_powertracking.h"
#include "ars_thread.h"
#include "ars_phy_iq.h"

typedef enum _PHYDM_H2C_CMD 
{
    ODM_H2C_RSSI_REPORT = 0,
    ODM_H2C_PSD_RESULT = 1,
    ODM_H2C_PathDiv = 2,
    ODM_H2C_WIFI_CALIBRATION = 3,
    ODM_H2C_IQ_CALIBRATION = 4,
    ODM_H2C_RA_PARA_ADJUST = 5,
    PHYDM_H2C_DYNAMIC_TX_PATH = 6,
    PHYDM_H2C_FW_TRACE_EN = 7,
    PHYDM_H2C_TXBF = 8,
    ODM_MAX_H2CCMD
} PHYDM_H2C_CMD;

typedef enum _RT_RF_TYPE_DEFINITION
{
    RF_1T2R = 0,
    RF_2T4R = 1,
    RF_2T2R = 2,
    RF_1T1R = 3,
    RF_2T2R_GREEN = 4,
    RF_2T3R = 5,    
    RF_3T3R = 6,
    RF_3T4R = 7,
    RF_4T4R = 8,

    RF_MAX_TYPE = 0xF, /* wf_u8 */
}RT_RF_TYPE_DEF_E;

typedef enum tag_ODM_RF_Type_Definition 
{
    ODM_1T1R,
    ODM_1T2R,
    ODM_2T2R,
    ODM_2T2R_GREEN,
    ODM_2T3R,
    ODM_2T4R,
    ODM_3T3R,
    ODM_3T4R,
    ODM_4T4R,
    ODM_XTXR
}ODM_RF_TYPE_E;
    

typedef enum _BASEBAND_CONFIG_PHY_REG_PG_VALUE_TYPE
{
    PHY_REG_PG_RELATIVE_VALUE = 0,
    PHY_REG_PG_EXACT_VALUE = 1
} PHY_REG_PG_TYPE;

typedef enum _ODM_Common_Info_Definition
{
//-------------REMOVED CASE-----------//
    //ODM_CMNINFO_CCK_HP,
    //ODM_CMNINFO_RFPATH_ENABLE,        // Define as ODM write???   
    //ODM_CMNINFO_BT_COEXIST,               // ODM_BT_COEXIST_E
    //ODM_CMNINFO_OP_MODE,              // ODM_OPERATION_MODE_E
//-------------REMOVED CASE-----------//

    //
    // Fixed value:
    //

    //-----------HOOK BEFORE REG INIT-----------//
    ODM_CMNINFO_PLATFORM = 0,
    ODM_CMNINFO_ABILITY,                    // ODM_ABILITY_E
    ODM_CMNINFO_INTERFACE,              // ODM_INTERFACE_E
    ODM_CMNINFO_MP_TEST_CHIP,
    ODM_CMNINFO_IC_TYPE,                    // ODM_IC_TYPE_E
    ODM_CMNINFO_CUT_VER,                    // ODM_CUT_VERSION_E
    ODM_CMNINFO_FAB_VER,                    // ODM_FAB_E
    ODM_CMNINFO_RF_TYPE,                    // ODM_RF_PATH_E or ODM_RF_TYPE_E?
    ODM_CMNINFO_RFE_TYPE, 
    ODM_CMNINFO_BOARD_TYPE,             // ODM_BOARD_TYPE_E
    ODM_CMNINFO_PACKAGE_TYPE,
    ODM_CMNINFO_EXT_LNA,                    // TRUE
    ODM_CMNINFO_5G_EXT_LNA, 
    ODM_CMNINFO_EXT_PA,
    ODM_CMNINFO_5G_EXT_PA,
    ODM_CMNINFO_GPA,
    ODM_CMNINFO_APA,
    ODM_CMNINFO_GLNA,
    ODM_CMNINFO_ALNA,
    ODM_CMNINFO_EXT_TRSW,
    ODM_CMNINFO_EXT_LNA_GAIN,
    ODM_CMNINFO_PATCH_ID,               //CUSTOMER ID
    ODM_CMNINFO_BINHCT_TEST,
    ODM_CMNINFO_BWIFI_TEST,
    ODM_CMNINFO_SMART_CONCURRENT,
    ODM_CMNINFO_CONFIG_BB_RF,
    ODM_CMNINFO_DOMAIN_CODE_2G,
    ODM_CMNINFO_DOMAIN_CODE_5G,
    ODM_CMNINFO_IQKFWOFFLOAD,
    ODM_CMNINFO_HUBUSBMODE,
    ODM_CMNINFO_FWDWRSVDPAGEINPROGRESS,
    ODM_CMNINFO_TX_TP,
    ODM_CMNINFO_RX_TP,
    ODM_CMNINFO_SOUNDING_SEQ,
    //-----------HOOK BEFORE REG INIT-----------//  


    //
    // Dynamic value:
    //
//--------- POINTER REFERENCE-----------//
    ODM_CMNINFO_MAC_PHY_MODE,           // ODM_MAC_PHY_MODE_E
    ODM_CMNINFO_TX_UNI,
    ODM_CMNINFO_RX_UNI,
    ODM_CMNINFO_WM_MODE,                // ODM_WIRELESS_MODE_E
    ODM_CMNINFO_BAND,                   // ODM_BAND_TYPE_E
    ODM_CMNINFO_SEC_CHNL_OFFSET,        // ODM_SEC_CHNL_OFFSET_E
    ODM_CMNINFO_SEC_MODE,               // ODM_SECURITY_E
    ODM_CMNINFO_BW,                     // ODM_BW_E
    ODM_CMNINFO_CHNL,
    ODM_CMNINFO_FORCED_RATE,
    
    ODM_CMNINFO_DMSP_GET_VALUE,
    ODM_CMNINFO_BUDDY_ADAPTOR,
    ODM_CMNINFO_DMSP_IS_MASTER,
    ODM_CMNINFO_SCAN,
    ODM_CMNINFO_POWER_SAVING,
    ODM_CMNINFO_ONE_PATH_CCA,           // ODM_CCA_PATH_E
    ODM_CMNINFO_DRV_STOP,
    ODM_CMNINFO_PNP_IN,
    ODM_CMNINFO_INIT_ON,
    ODM_CMNINFO_ANT_TEST,
    ODM_CMNINFO_NET_CLOSED,
    //ODM_CMNINFO_RTSTA_AID,                // For win driver only?
    ODM_CMNINFO_FORCED_IGI_LB,
    ODM_CMNINFO_P2P_LINK,
    ODM_CMNINFO_FCS_MODE,
    ODM_CMNINFO_IS1ANTENNA,
    ODM_CMNINFO_RFDEFAULTPATH,
//--------- POINTER REFERENCE-----------//

//------------CALL BY VALUE-------------//
    ODM_CMNINFO_WIFI_DIRECT,
    ODM_CMNINFO_WIFI_DISPLAY,
    ODM_CMNINFO_LINK_IN_PROGRESS,           
    ODM_CMNINFO_LINK,
    ODM_CMNINFO_STATION_STATE,
    ODM_CMNINFO_RSSI_MIN,
    ODM_CMNINFO_DBG_COMP,               // wf_u64
    ODM_CMNINFO_DBG_LEVEL,              // wf_u32
    ODM_CMNINFO_RA_THRESHOLD_HIGH,      // wf_u8
    ODM_CMNINFO_RA_THRESHOLD_LOW,       // wf_u8
    ODM_CMNINFO_RF_ANTENNA_TYPE,        // wf_u8
    ODM_CMNINFO_BT_ENABLED,
    ODM_CMNINFO_BT_HS_CONNECT_PROCESS,
    ODM_CMNINFO_BT_HS_RSSI,
    ODM_CMNINFO_BT_OPERATION,
    ODM_CMNINFO_BT_LIMITED_DIG,                 //Need to Limited Dig or not
    ODM_CMNINFO_BT_DIG,
    ODM_CMNINFO_BT_BUSY,                    //Check Bt is using or not//neil    
    ODM_CMNINFO_BT_DISABLE_EDCA,
    ODM_CMNINFO_AP_TOTAL_NUM,
    ODM_CMNINFO_POWER_TRAINING,
//------------CALL BY VALUE-------------//

    //
    // Dynamic ptr array hook itms.
    //
    ODM_CMNINFO_STA_STATUS,
    ODM_CMNINFO_PHY_STATUS,
    ODM_CMNINFO_MAC_STATUS,
    
    ODM_CMNINFO_MAX,


}ODM_CMNINFO_E;

typedef enum tag_Bandwidth_Definition
{
    ODM_BW20M       = 0,
    ODM_BW40M       = 1,
    ODM_BW80M       = 2,
    ODM_BW160M      = 3,
    ODM_BW5M        = 4,
    ODM_BW10M       = 5,
    ODM_BW_MAX      = 6
}ODM_BW_E;


typedef enum tag_Board_Definition
{
    ODM_BOARD_DEFAULT   = 0,      // The DEFAULT case.
    ODM_BOARD_MINICARD  = BIT(0), // 0 = non-mini card, 1= mini card.
    ODM_BOARD_SLIM      = BIT(1), // 0 = non-slim card, 1 = slim card
    ODM_BOARD_BT        = BIT(2), // 0 = without BT card, 1 = with BT
    ODM_BOARD_EXT_PA    = BIT(3), // 0 = no 2G ext-PA, 1 = existing 2G ext-PA
    ODM_BOARD_EXT_LNA   = BIT(4), // 0 = no 2G ext-LNA, 1 = existing 2G ext-LNA
    ODM_BOARD_EXT_TRSW  = BIT(5), // 0 = no ext-TRSW, 1 = existing ext-TRSW
    ODM_BOARD_EXT_PA_5G = BIT(6), // 0 = no 5G ext-PA, 1 = existing 5G ext-PA
    ODM_BOARD_EXT_LNA_5G= BIT(7), // 0 = no 5G ext-LNA, 1 = existing 5G ext-LNA
}ODM_BOARD_TYPE_E;



typedef struct temporarily_save_info_st_
{
    wf_u8 u1ForcedIgiLb;
    wf_u8 usb_speed; // 1.1, 2.0 or 3.0
    wf_u32 bScanInProcess;

    wf_u8   bpower_saving; //for LPS/IPS


    wf_u16  TypeGLNA;
    wf_u16  TypeGPA;
    wf_u16  TypeALNA;
    wf_u16  TypeAPA;
    wf_u16  RFEType;

    wf_u8   PAType_2G;
    wf_u8   LNAType_2G;
    wf_u8   ExternalPA_2G;
    wf_u8   ExternalLNA_2G;

    wf_u16  CustomerID;

    /* Upper and Lower Signal threshold for Rate Adaptive*/	
    wf_s32 EntryMinUndecoratedSmoothedPWDB;
    wf_s32 EntryMaxUndecoratedSmoothedPWDB;
    wf_s32 MinUndecoratedPWDBForDM;
}com_save_info_st;

typedef struct
{
    wf_u8 EEPROMThermalMeter;
}EERPOM_DATA_ST;

typedef struct _RSSI_STA
{
    wf_s32  UndecoratedSmoothedPWDB;
    wf_s32  UndecoratedSmoothedCCK;
    wf_s32  UndecoratedSmoothedOFDM;
    wf_u32  OFDM_pkt;
    wf_u64  PacketMap;
    wf_u8   ValidBit;

    wf_u8 rssi_level;
}RSSI_STA, *PRSSI_STA;

typedef  struct adaptive_rate_system_st
{
    void *nic_info;
    wdn_net_info_st *wdn_net;
    ars_acs_info_st acs;
    com_save_info_st com_save; //
    ars_nm_info_st nm;
    ars_pathdiv_info_st pathdiv;
    ars_ra_info_st ra;
    ars_txpower_info_st txpower;
    ars_antdect_info_st antdect;
    ars_antdiv_info_st antdiv;
    ars_bbps_info_st bbps;
    ars_beamform_info_st beamform;
    ars_cfo_info_st cfo;
    ars_dig_info_st dig;
    ars_edcaturbo_info_st edcaturbo;
    ars_hwconfig_info_st  hwconfig;
    RSSI_STA  rssi_sta[ODM_ASSOCIATE_ENTRY_NUM];
    ars_dbg_info_st dbg;
   
    FALSE_ALARM_STATISTICS  FalseAlmCnt;
    // from here copy from onkey version
    //will rewrite after function is ok
    PHY_REG_PG_TYPE     PhyRegPgValueType;
    wf_u8               PhyRegPgVersion;

    wf_u32          NumQryPhyStatusAll;     //CCK + OFDM
    wf_u32          LastNumQryPhyStatusAll; 
    wf_u32          RxPWDBAve;
    wf_bool         MPDIG_2G;       //off MPDIG
    wf_u8           Times_2G;

    //------ ODM HANDLE, DRIVER NEEDS NOT TO HOOK------//
    wf_bool         bCckHighPower; 
    wf_u8           RFPathRxEnable;     // ODM_CMNINFO_RFPATH_ENABLE
    wf_u8           ControlChannel;

    wf_u8           cck_lna_idx;
    wf_u8           cck_vga_idx;
    wf_u8           ofdm_agc_idx[4];

    wf_u32          SupportAbility;
    //For Adaptivtiy
    wf_u16          NHM_cnt_0;
    wf_u16          NHM_cnt_1;
    wf_s8           TH_L2H_default;
    wf_s8           TH_EDCCA_HL_diff_default;
    wf_s8           TH_L2H_ini;
    wf_s8           TH_EDCCA_HL_diff;
    wf_s8           TH_L2H_ini_mode2;
    wf_s8           TH_EDCCA_HL_diff_mode2;
    wf_bool         Carrier_Sense_enable;
    wf_u8           Adaptivity_IGI_upper;
    wf_bool         adaptivity_flag;
    wf_u8           DCbackoff;
    wf_bool         Adaptivity_enable;
    wf_u8           APTotalNum;
    wf_bool         EDCCA_enable;
    ADAPTIVITY_STATISTICS   Adaptivity;
    //For Adaptivtiy
    wf_u8           LastUSBHub;
    wf_u8           TxBfDataRate;
    
    wf_u8           c2h_cmd_start;
    wf_u8           fw_debug_trace[60]; 
    wf_u8           pre_c2h_seq;
    wf_bool         fw_buff_is_enpty;
    wf_u32          data_frame_num;
    
    SWAT_T          DM_SWAT_Table;

    wf_u8           RSSI_A;
    wf_u8           RSSI_B;
    wf_u8           RSSI_C;
    wf_u8           RSSI_D;
    wf_u64          RSSI_TRSW;  
    wf_u64          RSSI_TRSW_H;
    wf_u64          RSSI_TRSW_L;    
    wf_u64          RSSI_TRSW_iso;
    wf_u8           TXAntStatus;
    wf_u8           RXAntStatus;
    wf_u8           RxRate;
    wf_bool         bNoisyState;
    
    wf_u8           TxRate;
    wf_u8           pre_number_linked_client;   
    wf_u8           number_linked_client;
    wf_u8           pre_number_active_client;   
    wf_u8           number_active_client;

    wf_u8           *HubUsbMode;
    wf_u8           mp_mode;
    wf_bool         bLinkInProcess;

    // RF Type 4T4R/3T3R/2T2R/1T2R/1T1R/...
    wf_u8           RFType;
    wf_u8           RFEType;
    // Board Type Normal/HighPower/MiniCard/SLIM/Combo/... = 0/1/2/3/4/...
    wf_u8           BoardType;
    wf_u8           PackageType;
    wf_u16          TypeGLNA;
    wf_u16          TypeGPA;
    wf_u16          TypeALNA;
    wf_u16          TypeAPA;
    // with external LNA  NO/Yes = 0/1
    wf_u8           ExtLNA; // 2G
    // with external PA  NO/Yes = 0/1
    wf_u8           ExtPA; // 2G
    // with external TRSW  NO/Yes = 0/1
    wf_u8           ExtTRSW;
    wf_u8           ExtLNAGain; // 2G
    wf_u8           PatchID; //Customer ID
    wf_bool         bInHctTest;
    wf_bool         bWIFITest;

    wf_bool         bDualMacSmartConcurrent;
    wf_u32          BK_SupportAbility;
    wf_u8           AntDivType;
    wf_bool         ConfigBBRF;
    wf_u8           odm_Regulation2_4G;
    wf_u8           IQKFWOffload;
    
    wf_bool         bWIFI_Direct;
    wf_bool         bWIFI_Display;
    wf_bool         bLinked;
    wf_bool         bsta_state;
    
    wf_u8           RSSI_Min;

    wf_bool         bIsMPChip;
    wf_u8           CutVersion;
    wf_u8           FabVersion;


    // MAC PHY Mode SMSP/DMSP/DMDP = 0/1/2
    wf_u8           *pMacPhyMode;
    //TX Unicast byte count
    wf_u64          *pNumTxBytesUnicast;
    //RX Unicast byte count
    wf_u64          *pNumRxBytesUnicast;
    // Wireless mode B/G/A/N = BIT0/BIT1/BIT2/BIT3
    wf_u8           *pWirelessMode; //ODM_WIRELESS_MODE_E
    // Frequence band 2.4G/5G = 0/1
    wf_u8           *pBandType;
    // Secondary channel offset don't_care/below/above = 0/1/2
    wf_u8           *pSecChOffset;
    // Security mode Open/WEP/AES/TKIP = 0/1/2/3
    wf_u8           *pSecurity;
    // BW info 20M/40M/80M = 0/1/2
    wf_u8           *pBandWidth;
    // Central channel location Ch1/Ch2/....
    wf_u8           *pChannel;  //central channel number
    wf_bool         DPK_Done;
    // Common info for 92D DMSP
    
    wf_bool         *pbGetValueFromOtherMac;
    void            *pBuddyAdapter;
    wf_bool         *pbMasterOfDMSP; //MAC0: master, MAC1: slave
    // Common info for Status
    wf_bool         *pbScanInProcess;
    wf_bool         *pbPowerSaving;
    // CCA Path 2-path/path-A/path-B = 0/1/2; using ODM_CCA_PATH_E.
    wf_u8           *pOnePathCCA;
    //pMgntInfo->AntennaTest
    wf_u8           *pAntennaTest;
    wf_bool         *pbNet_closed;
    //wf_u8            *pAidMap;
    wf_u8           *pu1ForcedIgiLb;
    wf_bool         *pIsFcsModeEnable;
/*--------- For 8723B IQK-----------*/
    wf_bool         *pIs1Antenna;
    wf_u8           *pRFDefaultPath;
    // 0:S1, 1:S0
    
//--------- POINTER REFERENCE-----------//
    wf_bool         is_hook_pointer;
    wf_u16*         pForcedDataRate;
    wf_bool         *pbFwDwRsvdPageInProgress;
    wf_u32          *pCurrentTxTP;
    wf_u32          *pCurrentRxTP;
    wf_u8           *pSoundingSeq;

    wf_bool         *pbDriverStopped;
    wf_bool         *pbDriverIsGoingToPnpSetPowerSleep;
    wf_bool         *pinit_adpt_in_progress;

    wf_bool         bOneEntryOnly;
    wf_u32          OneEntry_MACID;


    /*for noise detection*/
    wf_bool         NoisyDecision; /*b_noisy*/
    wf_bool         pre_b_noisy;    
    wf_u32          NoisyDecision_Smooth;
    ODM_NOISE_MONITOR noise_level;//[ODM_MAX_CHANNEL_NUM];

    ODM_RF_CAL_T    RFCalibrateInfo;
    RT_BEAMFORMING_INFO BeamformingInfo;


    //
    // Power Training
    //
    wf_u8           ForcePowerTrainingState;
    wf_bool         bChangeState;
    wf_u32          PT_score;
    wf_u64          OFDM_RX_Cnt;
    wf_u64          CCK_RX_Cnt;
    wf_bool         bDisablePowerTraining;

    // eeprom data
    EERPOM_DATA_ST eeprom_data;

    ars_thread_info_st ars_thread;

    wf_bool fw_ractrl;

    wf_u8 LastHMEBoxNum;
}ars_st;

wf_s32 ars_init(nic_info_st *pnic_info);
wf_s32 ars_term(nic_info_st *pnic_info);

wf_s32 ODM_CmnInfoUpdate(ars_st *pars,wf_u32 CmnInfo,wf_u64 Value );
wf_s32 ODM_CmnInfoHook(ars_st *pars,ODM_CMNINFO_E CmnInfo,void *pValue);
wf_s32 ODM_CmnInfoPtrArrayHook(ars_st *pars,ODM_CMNINFO_E    CmnInfo,wf_u16 Index,void *pValue);
wf_s32 wf_ars_info_update(nic_info_st *pnic_info);
wf_s32 wf_ars_process_once(nic_info_st *nic_info);

wf_s32 ars_io_lock_try(ars_st *ars);
wf_s32 ars_io_unlock_try(ars_st *ars);

#endif
#endif
