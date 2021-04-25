/******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef HAL_OPS_DM_H
#define HAL_OPS_DM_H

#define u1Byte u8
#define s1Byte s8
#define u4Byte u32
#define s4Byte s32
#define u8Byte u64
#define u2Byte u16



typedef struct msg_var_req_ {
	int var;
	SIZE_T msg;
	SIZE_T set;
} msg_var_req_t;

typedef struct msg_config_ {
	u32 customer_id;
	u32 cut_version;
	u32 Regulation2_4G;
	u32 TypeGPA;
	u32 TypeAPA;
	u32 TypeGLNA;
	u32 TypeALNA;
	u32 RFEType;
	u32 PackageType;
	u32 boardConfig;
} msg_config_t;

typedef struct phy_cali_ {
	u8 TxPowerTrackControl;
	s8 Remnant_CCKSwingIdx;
	s8 Remnant_OFDMSwingIdx;
	u8 rsvd;
} phy_cali_t;

typedef struct msg_rw_val_ {
	u64 tx_bytes;
	u64 rx_bytes;
	u32 cur_wireless_mode;
	u32 CurrentBandType;
	u32 ForcedDataRate;
	u32 nCur40MhzPrimeSC;
	u32 dot11PrivacyAlgrthm;
	u32 CurrentChannelBW;
	u32 CurrentChannel;
	u32 net_closed;
	u32 u1ForcedIgiLb;
	u32 bScanInProcess;
	u32 bpower_saving;
	u32 traffic_stat_cur_tx_tp;
	u32 traffic_stat_cur_rx_tp;
	u32 msgWdgStateVal;
	u32 ability;
	u32 Rssi_Min;
	u32 dig_CurIGValue;
	u32 wifi_direct;
	u32 wifi_display;
	u64 dbg_cmp;
	u32 dbg_level;
	u32 PhyRegPgVersion;
	u32 PhyRegPgValueType;
	phy_cali_t phy_cali;
	u32 bDisablePowerTraining;
	u32 fw_state;
	u32 sta_count;
} msg_rw_val_t;
extern msg_rw_val_t msg_rw_val, r_msg_rw_val;

typedef enum _BASEBAND_CONFIG_PHY_REG_PG_VALUE_TYPE {
	PHY_REG_PG_RELATIVE_VALUE = 0,
	PHY_REG_PG_EXACT_VALUE = 1
} PHY_REG_PG_TYPE;

typedef struct msg_rssi_ {
	u8 sg;
	u8 sq;
} msg_rssi_t;

typedef struct msg_scan_bssid_ {
	u8 bssid[6];

} msg_scan_bssid_t;

typedef struct _OPS_DM_Per_Pkt_Info_ {
	u1Byte DataRate;
	u1Byte StationID;
	BOOLEAN bPacketMatchBSSID;
	BOOLEAN bPacketToSelf;
	BOOLEAN bPacketBeacon;
	BOOLEAN bToSelf;
	u1Byte cck_agc_rpt_ofdm_cfosho_a;
	u1Byte cck_sig_qual_ofdm_pwdb_all;
	u1Byte gain;
	u1Byte path_rxsnr;
	s1Byte stream_rxevm;
	s1Byte path_cfotail;
	u32 bcn_cnt;
	u4Byte rsvd;
	msg_scan_bssid_t Scaninfo;
} OPS_DM_PACKET_INFO_T, *POPS_DM_PACKET_INFO_T;

typedef struct RX_AGC_T_ {
	u1Byte gain:7, trsw:1;

} RX_AGC_T, *pRX_AGC_T;

typedef struct _Phy_Status_Rpt_9082_ {
	RX_AGC_T path_agc[2];
	u1Byte ch_corr[2];
	u1Byte cck_sig_qual_ofdm_pwdb_all;
	u1Byte cck_agc_rpt_ofdm_cfosho_a;
	u1Byte cck_rpt_b_ofdm_cfosho_b;
	u1Byte rsvd_1;
	u1Byte noise_power_db_msb;
	s1Byte path_cfotail[2];
	u1Byte pcts_mask[2];
	s1Byte stream_rxevm[2];
	u1Byte path_rxsnr[2];
	u1Byte noise_power_db_lsb;
	u1Byte rsvd_2[3];
	u1Byte stream_csi[2];
	u1Byte stream_target_csi[2];
	s1Byte sig_evm;
	u1Byte rsvd_3;
	u1Byte antsel_rx_keep_2:1;
	u1Byte sgi_en:1;
	u1Byte rxsc:2;
	u1Byte idle_long:1;
	u1Byte r_ant_train_en:1;
	u1Byte ant_sel_b:1;
	u1Byte ant_sel:1;

} PHY_STATUS_RPT_9082_T, *PPHY_STATUS_RPT_9082_T;

#define		DM_RATR_STA_INIT			0
#define		DM_RATR_STA_HIGH			1
#define 		DM_RATR_STA_MIDDLE		2
#define 		DM_RATR_STA_LOW			3

#define MSG_ASSOCIATE_ENTRY_NUM 32

#define MSG_DBG_OFF					1

#define MSG_DBG_SERIOUS				2

#define MSG_DBG_WARNING				3

#define MSG_DBG_LOUD					4

#define MSG_DBG_TRACE					5

#define	CCK_TABLE_SIZE			33

#define MSG_RF_PATH_A  0

typedef enum _HAL_STATUS {
	HAL_STATUS_SUCCESS,
	HAL_STATUS_FAILURE,

} HAL_STATUS, *PHAL_STATUS;

void msg_sync_var(PNIC Nic);
void msg_get_var(PNIC Nic);

void Func_Of_Init_Dm_Priv(IN PNIC Nic);
void Func_Of_Deinit_Dm_Priv(IN PNIC Nic);
VOID Func_Of_Haldmwatchdog(IN PNIC Nic);

u4Byte Func_Hw_Op_Get_Rate_Bitmap(IN PNIC Nic,
						   IN u4Byte macid,
						   IN u4Byte ra_mask, IN u1Byte rssi_level);
VOID Func_Chip_Bb_Iqcalibrate_Process(IN PNIC Nic,
						   IN BOOLEAN bReCovery, IN BOOLEAN bRestore);

VOID Func_Chip_Bb_Lccalibrate_Process(PNIC Nic);

VOID Func_Hw_Op_Cleartxpowertrackingstate(PNIC Nic);

HAL_STATUS Func_Hw_Op_Configrfwithheaderfile(IN PNIC Nic,
									  IN int ConfigType, IN int eRFPath);

HAL_STATUS Func_Hw_Op_Configrfwithtxpwrtrackheaderfile(IN void *pAdapter);

HAL_STATUS Func_Hw_Op_Configbbwithheaderfile(PNIC Nic, IN int ConfigType);

VOID Func_Hw_Op_Cmninfoptrarrayhook(IN PNIC Nic,
							 IN u2Byte Index, IN PVOID pValue);



s32 Func_Chip_Hw_Trx_Test_Set_Powertracking(PNIC Nic, u8 enable);
void Func_Chip_Hw_Trx_Test_Get_Powertracking(PNIC Nic, u8 * enable);
s8 Func_Chip_Bb_Gettxpowertrackingoffset(PNIC Nic, u8 Rate, u8 RFPath);

typedef enum _MSG_BB_Config_Type {
	CONFIG_BB_PHY_REG,
	CONFIG_BB_AGC_TAB,
	CONFIG_BB_AGC_TAB_2G,
	CONFIG_BB_PHY_REG_PG = 4,
	CONFIG_BB_PHY_REG_MP,
	CONFIG_BB_AGC_TAB_DIFF,
} MSG_BB_Config_Type, *PMSG_BB_Config_Type;

typedef enum _MSG_Support_Ability_Definition {
	MSG_BB_DIG = BIT0,
	MSG_BB_RA_MASK = BIT1,
	MSG_BB_DYNAMIC_TXPWR = BIT2,
	MSG_BB_FA_CNT = BIT3,
	MSG_BB_RSSI_MONITOR = BIT4,
	MSG_BB_CCK_PD = BIT5,
	MSG_BB_ANT_DIV = BIT6,
	MSG_BB_PWR_SAVE = BIT7,
	MSG_BB_PWR_TRAIN = BIT8,
	MSG_BB_RATE_ADAPTIVE = BIT9,
	MSG_BB_PATH_DIV = BIT10,
	MSG_BB_PSD = BIT11,
	MSG_BB_RXHP = BIT12,
	MSG_BB_ADAPTIVITY = BIT13,
	MSG_BB_CFO_TRACKING = BIT14,
	MSG_BB_NHM_CNT = BIT15,
	MSG_BB_PRIMARY_CCA = BIT16,
	MSG_BB_TXBF = BIT17,

	MSG_MAC_EDCA_TURBO = BIT20,
	MSG_MAC_EARLY_MODE = BIT21,

	MSG_RF_TX_PWR_TRACK = BIT24,
	MSG_RF_RX_GAIN_TRACK = BIT25,
	MSG_RF_CALIBRATION = BIT26,

} MSG_ABILITY_E;

#define DYNAMIC_FUNC_DISABLE		(0x0)

void sta_info_init(void);

#if 0 //need change
void Func_Enter_Var_Bh_Lock(_irqL irqL);
void Func_Enter_Var_Bh_Unlock(_irqL irqL);

int Func_Chip_Bb_Status_Query_Send(union recv_frame *precvframe, u8 * pbuf, int pkt_len);
#endif

void Func_Hw_Op_Sync_Var(PNIC Nic, const char *f, int l);
void Func_Hw_Op_Get_Var(PNIC Nic, const char *f, int l);

#define msg_get_var(a)  Func_Hw_Op_Get_Var(a, __func__, __LINE__)
#define msg_sync_var(a) Func_Hw_Op_Sync_Var(a, __func__, __LINE__)

#endif
