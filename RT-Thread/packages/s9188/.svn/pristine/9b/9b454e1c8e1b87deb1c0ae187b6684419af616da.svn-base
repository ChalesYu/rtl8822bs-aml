#ifndef __ARS_BEAMFORMING_H__
#define __ARS_BEAMFORMING_H__

#define MAX_BEAMFORMEE_SU   2
#define MAX_BEAMFORMER_SU   2

#define MAX_BEAMFORMEE_MU   0
#define MAX_BEAMFORMER_MU   0

#define BEAMFORMEE_ENTRY_NUM        (MAX_BEAMFORMEE_SU + MAX_BEAMFORMEE_MU)
#define BEAMFORMER_ENTRY_NUM        (MAX_BEAMFORMER_SU + MAX_BEAMFORMER_MU)

#define BEACON_QUEUE    BCN_QUEUE_INX
#define NORMAL_QUEUE    MGT_QUEUE_INX

typedef enum _TXBF_SET_TYPE{
    TXBF_SET_SOUNDING_ENTER,
    TXBF_SET_SOUNDING_LEAVE,
    TXBF_SET_SOUNDING_RATE,
    TXBF_SET_SOUNDING_STATUS,
    TXBF_SET_SOUNDING_FW_NDPA,
    TXBF_SET_SOUNDING_CLK,
    TXBF_SET_TX_PATH_RESET,
    TXBF_SET_GET_TX_RATE
}TXBF_SET_TYPE,*PTXBF_SET_TYPE;


typedef enum _TXBF_GET_TYPE{
    TXBF_GET_EXPLICIT_BEAMFORMEE,
    TXBF_GET_EXPLICIT_BEAMFORMER,
    TXBF_GET_MU_MIMO_STA,
    TXBF_GET_MU_MIMO_AP
}TXBF_GET_TYPE,*PTXBF_GET_TYPE;



typedef enum _BEAMFORMING_CAP 
{
    BEAMFORMING_CAP_NONE = 0x0,
    BEAMFORMER_CAP_HT_EXPLICIT = BIT(1), 
    BEAMFORMEE_CAP_HT_EXPLICIT = BIT(2), 
    BEAMFORMER_CAP_VHT_SU = BIT(5),           /* Self has er Cap, because Reg er  & peer ee */
    BEAMFORMEE_CAP_VHT_SU = BIT(6),           /* Self has ee Cap, because Reg ee & peer er */
    BEAMFORMER_CAP_VHT_MU = BIT(7),           /* Self has er Cap, because Reg er  & peer ee */
    BEAMFORMEE_CAP_VHT_MU = BIT(8),           /* Self has ee Cap, because Reg ee & peer er */
    BEAMFORMER_CAP = BIT(9),
    BEAMFORMEE_CAP = BIT(10),
}BEAMFORMING_CAP, *PBEAMFORMING_CAP;

typedef enum _BEAMFORMING_ENTRY_STATE 
{
    BEAMFORMING_ENTRY_STATE_UNINITIALIZE, 
    BEAMFORMING_ENTRY_STATE_INITIALIZEING, 
    BEAMFORMING_ENTRY_STATE_INITIALIZED, 
    BEAMFORMING_ENTRY_STATE_PROGRESSING, 
    BEAMFORMING_ENTRY_STATE_PROGRESSED 
} BEAMFORMING_ENTRY_STATE, *PBEAMFORMING_ENTRY_STATE;

typedef enum _WIRELESS_MODE 
{
    WIRELESS_MODE_UNKNOWN = 0x00,
    WIRELESS_MODE_A = 0x01,
    WIRELESS_MODE_B = 0x02,
    WIRELESS_MODE_G = 0x04,
    WIRELESS_MODE_AUTO = 0x08,
    WIRELESS_MODE_N_24G = 0x10,
    WIRELESS_MODE_N_5G = 0x20,
    WIRELESS_MODE_AC_5G = 0x40,
    WIRELESS_MODE_AC_24G  = 0x80,
    WIRELESS_MODE_AC_ONLY  = 0x100,
} WIRELESS_MODE;

typedef enum _SOUNDING_MODE 
{
    SOUNDING_SW_VHT_TIMER = 0x0,
    SOUNDING_SW_HT_TIMER = 0x1, 
    SOUNDING_STOP_All_TIMER = 0x2, 
    SOUNDING_HW_VHT_TIMER = 0x3,            
    SOUNDING_HW_HT_TIMER = 0x4,
    SOUNDING_STOP_OID_TIMER = 0x5, 
    SOUNDING_AUTO_VHT_TIMER = 0x6,
    SOUNDING_AUTO_HT_TIMER = 0x7,
    SOUNDING_FW_VHT_TIMER = 0x8,
    SOUNDING_FW_HT_TIMER = 0x9,
}SOUNDING_MODE, *PSOUNDING_MODE;


typedef struct _RT_BEAMFORMEE_ENTRY 
{
    wf_bool bUsed;
    wf_bool bTxBF;
    wf_bool bSound;
    wf_u16  AID;                /*Used to construct AID field of NDPA packet.*/
    wf_u16  MacId;              /*Used to Set Reg42C in IBSS mode. */
    wf_u16  P_AID;              /*Used to fill Reg42C & Reg714 to compare with P_AID of Tx DESC. */
    wf_u16  G_ID;               /*Used to fill Tx DESC*/
    wf_u8   MyMacAddr[6];
    wf_u8   MacAddr[6];         /*Used to fill Reg6E4 to fill Mac address of CSI report frame.*/
    CHANNEL_WIDTH           SoundBW;        /*Sounding BandWidth*/
    wf_u16                  SoundPeriod;
    BEAMFORMING_CAP         BeamformEntryCap;
    BEAMFORMING_ENTRY_STATE BeamformEntryState; 
    wf_bool                     bBeamformingInProgress;
    /*wf_u8 LogSeq;                                 // Move to _RT_BEAMFORMER_ENTRY*/
    /*wf_u16    LogRetryCnt:3;      // 0~4              // Move to _RT_BEAMFORMER_ENTRY*/
    /*wf_u16    LogSuccessCnt:2;        // 0~2              // Move to _RT_BEAMFORMER_ENTRY*/
    wf_u16  LogStatusFailCnt:5; // 0~21
    wf_u16  DefaultCSICnt:5;        // 0~21
    wf_u8   CSIMatrix[327];
    wf_u16  CSIMatrixLen;
    wf_u8   NumofSoundingDim;
    wf_u8   CompSteeringNumofBFer;
    wf_u8   su_reg_index;
    /*For MU-MIMO*/
    wf_bool is_mu_sta;
    wf_u8   mu_reg_index;
    wf_u8   gid_valid[8];
    wf_u8   user_position[16];
} RT_BEAMFORMEE_ENTRY, *PRT_BEAMFORMEE_ENTRY;



typedef struct _RT_BEAMFORMER_ENTRY 
{
    wf_bool             bUsed;
    /*P_AID of BFer entry is probably not used*/
    wf_u16              P_AID;                  /*Used to fill Reg42C & Reg714 to compare with P_AID of Tx DESC. */
    wf_u16              G_ID;
    wf_u8               MyMacAddr[6];
    wf_u8               MacAddr[6];
    BEAMFORMING_CAP BeamformEntryCap;
    wf_u8               NumofSoundingDim;
    wf_u8               ClockResetTimes;        /*Modified by Jeffery @2015-04-10*/
    wf_u8               PreLogSeq;              /*Modified by Jeffery @2015-03-30*/
    wf_u8               LogSeq;                 /*Modified by Jeffery @2014-10-29*/
    wf_u16              LogRetryCnt:3;          /*Modified by Jeffery @2014-10-29*/
    wf_u16              LogSuccess:2;           /*Modified by Jeffery @2014-10-29*/
    wf_u8               su_reg_index;
     /*For MU-MIMO*/
    wf_bool             is_mu_ap;
    wf_u8               gid_valid[8];
    wf_u8               user_position[16];
    wf_u16              AID;
} RT_BEAMFORMER_ENTRY, *PRT_BEAMFORMER_ENTRY;

typedef struct _RT_SOUNDING_INFO 
{
    wf_u8           SoundIdx;
    CHANNEL_WIDTH   SoundBW;
    SOUNDING_MODE   SoundMode; 
    wf_u16          SoundPeriod;
} RT_SOUNDING_INFO, *PRT_SOUNDING_INFO;

typedef struct _RT_BEAMFORMING_OID_INFO 
{
    wf_u8           SoundOidIdx;
    CHANNEL_WIDTH   SoundOidBW; 
    SOUNDING_MODE   SoundOidMode;
    wf_u16          SoundOidPeriod;
} RT_BEAMFORMING_OID_INFO, *PRT_BEAMFORMING_OID_INFO;


//2 HAL TXBF related
typedef struct _HAL_TXBF_INFO 
{
    wf_u8               TXBFIdx;
    wf_u8               NdpaIdx;
    wf_u8               BW;
    wf_u8               Rate;

    //RT_TIMER          Txbf_FwNdpaTimer;


} HAL_TXBF_INFO, *PHAL_TXBF_INFO;


typedef struct _RT_BEAMFORM_STAINFO 
{
    wf_u8*                      RA; 
    wf_u16                      AID; 
    wf_u16                      MacID;
    wf_u8                       MyMacAddr[6];
    WIRELESS_MODE               WirelessMode;
    CHANNEL_WIDTH               BW;
    BEAMFORMING_CAP         BeamformCap;
    wf_u8                       HtBeamformCap;
    wf_u16                      VhtBeamformCap;
    wf_u8                       CurBeamform; 
    wf_u16                      CurBeamformVHT;
} RT_BEAMFORM_STAINFO, *PRT_BEAMFORM_STAINFO;


typedef struct _RT_BEAMFORMING_INFO 
{
    BEAMFORMING_CAP         BeamformCap;
    RT_BEAMFORMEE_ENTRY     BeamformeeEntry[BEAMFORMEE_ENTRY_NUM];
    RT_BEAMFORMER_ENTRY     BeamformerEntry[BEAMFORMER_ENTRY_NUM];
    RT_BEAMFORM_STAINFO     BeamformSTAinfo;
    wf_u8                   BeamformeeCurIdx;
    //RT_TIMER                  BeamformingTimer;
    //RT_TIMER                  mu_timer;
    RT_SOUNDING_INFO            SoundingInfo;
    RT_BEAMFORMING_OID_INFO BeamformingOidInfo;
    HAL_TXBF_INFO           TxbfInfo;
    wf_u8                   SoundingSequence;
    wf_u8                   beamformee_su_cnt;
    wf_u8                   beamformer_su_cnt;
    wf_u32                  beamformee_su_reg_maping;
    wf_u32                  beamformer_su_reg_maping;
    /*For MU-MINO*/
    wf_u8                   beamformee_mu_cnt;
    wf_u8                   beamformer_mu_cnt;
    wf_u32                  beamformee_mu_reg_maping;
    wf_u8                   mu_ap_index;
    wf_bool                 is_mu_sounding;
    wf_u8                   FirstMUBFeeIndex;

    /* Control register */
    wf_u32                  RegMUTxCtrl;        /* For USB/SDIO interfaces aync I/O  */
} RT_BEAMFORMING_INFO, *PRT_BEAMFORMING_INFO;

typedef struct beamform_info_st_
{
}ars_beamform_info_st;

wf_s32 phydm_Beamforming_Watchdog(void *ars);
wf_s32 phydm_Beamforming_Init(void *ars);

#endif
