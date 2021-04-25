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

#ifndef __HAL_COMMON_H__
#define __HAL_COMMON_H__


#include "hint.h"
#include "comm_cfg.h"
#include "comm_wmbox1.h"


#define PPG_BB_GAIN_2G_TX_OFFSET_MASK	0x0F
#define PPG_BB_GAIN_2G_TXB_OFFSET_MASK	0xF0

#define PPG_THERMAL_OFFSET_MASK			0x1F
#define KFREE_BB_GAIN_2G_TX_OFFSET(_ppg_v) (((_ppg_v) == PPG_BB_GAIN_2G_TX_OFFSET_MASK) ? 0 : (((_ppg_v) & 0x01) ? ((_ppg_v) >> 1) : (-((_ppg_v) >> 1))))
#define KFREE_THERMAL_OFFSET(_ppg_v) (((_ppg_v) == PPG_THERMAL_OFFSET_MASK) ? 0 : (((_ppg_v) & 0x01) ? ((_ppg_v) >> 1) : (-((_ppg_v) >> 1))))

#define GET_PG_KFREE_THERMAL_K_ON_9086X(_pg_m)	LE_BITS_TO_1BYTE(((u8 *)(_pg_m)) + 0xC8, 5, 1)

#define PPG_BB_GAIN_2G_TXA_OFFSET_9086X	0xEE
#define PPG_THERMAL_OFFSET_9086X		0xEF

#define	EEPROM_TX_PWR_INX_9086X				0x10

#define	EEPROM_ChannelPlan_9086X			 0xBA 
#define	EEPROM_XTAL_9086X					0xB9
#define	EEPROM_THERMAL_METER_9086X			0x22

#define	EEPROM_RF_BOARD_OPTION_9086X		0xC1
#define	EEPROM_FEATURE_OPTION_9086X			0xC2
#define	EEPROM_VERSION_9086X				0xC4
#define	EEPROM_CustomID_9086X				0xC5
#define EEPROM_COUNTRY_CODE_9086X			0xCB

#define EEPROM_MAC_ADDR_9086XU				0xD7
#define EEPROM_VID_9086XU					0xD0
#define EEPROM_PID_9086XU					0xD2
#define EEPROM_USB_OPTIONAL_FUNCTION0_9086XU 0xD4

#define EEPROM_CHANNEL_PLAN_BY_HW_MASK		0x80

#define WLT_EEPROM_ID							0x9082
#define EEPROM_Default_ThermalMeter			0x12
#define	EEPROM_Default_ThermalMeter_9086X		0x18

#define EEPROM_Default_CrystalCap_9086X			0x20

#define EEPROM_DEFAULT_24G_INDEX			0x2D
#define EEPROM_DEFAULT_24G_HT20_DIFF		0X02

#ifdef NEW_EEPROM_TXPWR_DEFAULT
#define EEPROM_DEFAULT_24G_CCK_INDEX                    0x25
#define EEPROM_DEFAULT_24G_OFDM_INDEX                   0x2A
#define EEPROM_DEFAULT_24G_OFDM_DIFF    0X02
#else
#define EEPROM_DEFAULT_24G_OFDM_DIFF	0X04
#endif

#define EEPROM_DEFAULT_DIFF				0XFE
#define EEPROM_DEFAULT_BOARD_OPTION		0x00

#define TX_PWR_BY_RATE_NUM_BAND			1
#define TX_PWR_BY_RATE_NUM_RF			1
#define TX_PWR_BY_RATE_NUM_RATE			20

#define	MAX_CHNL_GROUP_24G		6

#define 	MAX_TX_COUNT				4

typedef struct _TxPowerInfo24G {
	u8 IndexCCK_Base[1][MAX_CHNL_GROUP_24G];
	u8 IndexBW40_Base[1][MAX_CHNL_GROUP_24G];
	s8 CCK_Diff[1][MAX_TX_COUNT];
	s8 OFDM_Diff[1][MAX_TX_COUNT];
	s8 BW20_Diff[1][MAX_TX_COUNT];
	s8 BW40_Diff[1][MAX_TX_COUNT];
} TxPowerInfo24G, *PTxPowerInfo24G;


typedef enum _ANTENNA_PATH {
	ANTENNA_NONE = 0,
	ANTENNA_A = 8,
} ANTENNA_PATH;
typedef struct _R_ANTENNA_SELECT_OFDM {
	u32 r_tx_antenna:4;
	u32 r_ant_l:4;
	u32 r_ant_non_ht:4;
	u32 r_ant_ht1:4;
	u32 r_ant_ht2:4;
	u32 r_ant_ht_s1:4;
	u32 r_ant_non_ht_s1:4;
	u32 OFDM_TXSC:2;
	u32 Reserved:2;
} R_ANTENNA_SELECT_OFDM;

typedef struct _R_ANTENNA_SELECT_CCK {
	u8 r_cckrx_enable_2:2;
	u8 r_cckrx_enable:2;
	u8 r_ccktx_enable:4;
} R_ANTENNA_SELECT_CCK;


#define REG_APS_FSMCO					0x0004
#define REG_RSV_CTRL					0x001C
#define REG_AFE_XTAL_CTRL				0x0024
#define REG_GPIO_PIN_CTRL				0x0044
#define REG_GPIO_INTM					0x0048
#define REG_HSIMR						0x0058
#define REG_GPIO_PIN_CTRL_2			0x0060

#define REG_CR							0x0100

#define REG_wMBOX0EVT_MSG_NORMAL		0x01A0
#define REG_wMBOX0EVT_CLEAR				0x01AF
#define REG_MCUTST_WOWLAN			0x01C7

#define REG_FWHW_TXQ_CTRL				0x0420
#define REG_HWSEQ_CTRL					0x0423
#define REG_BCNQ_BDNY					0x0424

#define REG_BAR_MODE_CTRL				0x04CC

#define REG_BCN_CTRL					0x0550
#define REG_BCN_CTRL_1					0x0551

#define REG_DUAL_TSF_RST				0x0553

#define REG_P2P_CTWIN					0x0572

#define REG_NOA_DESC_SEL				0x05CF
#define REG_NOA_DESC_DURATION		0x05E0
#define REG_NOA_DESC_INTERVAL			0x05E4
#define REG_NOA_DESC_START			0x05E8
#define REG_NOA_DESC_COUNT			0x05EC

#define REG_RCR							0x0608

#define REG_BSSID						0x0618

#define REG_PNO_STATUS					0x0631

#define RXERR_TYPE_OFDM_MPDU_OK 		2
#define RXERR_TYPE_OFDM_MPDU_FAIL	3

#define RXERR_TYPE_CCK_MPDU_OK		6
#define RXERR_TYPE_CCK_MPDU_FAIL		7

#define RXERR_TYPE_HT_MPDU_OK			11
#define RXERR_TYPE_HT_MPDU_FAIL 		12

#define RXERR_COUNTER_MASK			0xFFFFF
#define RXERR_RPT_RST					BIT(27)
#define _RXERR_RPT_SEL(type)			((type) << 28)

#define REG_RXERR_RPT					0x0664

#define REG_SECCFG						0x0680

#define REG_WKFMCAM_CMD				0x0698
#define REG_WKFMCAM_NUM				REG_WKFMCAM_CMD

#define REG_BCN_PSR_RPT				0x06A8
#define REG_BSSID1						0x0708

#define REG_USB_HRPWM					0xFE58

#define RRSR_1M					BIT0
#define RRSR_2M					BIT1
#define RRSR_5_5M				BIT2
#define RRSR_11M				BIT3
#define RRSR_6M					BIT4
#define RRSR_9M					BIT5
#define RRSR_12M				BIT6
#define RRSR_18M				BIT7
#define RRSR_24M				BIT8
#define RRSR_36M				BIT9
#define RRSR_48M				BIT10
#define RRSR_54M				BIT11

#define RRSR_CCK_RATES (RRSR_11M|RRSR_5_5M|RRSR_2M|RRSR_1M)

#define RATE_1M					BIT(0)
#define RATE_2M					BIT(1)
#define RATE_5_5M				BIT(2)
#define RATE_11M				BIT(3)
#define RATE_6M					BIT(4)
#define RATE_9M					BIT(5)
#define RATE_12M				BIT(6)
#define RATE_18M				BIT(7)
#define RATE_24M				BIT(8)
#define RATE_36M				BIT(9)
#define RATE_48M				BIT(10)
#define RATE_54M				BIT(11)

#define RCR_APPFCS				BIT31
#define RCR_APP_MIC				BIT30
#define RCR_APP_ICV				BIT29
#define RCR_APP_PHYST_RXFF		BIT28
#define RCR_HTC_LOC_CTRL		BIT14

//////////////////tangjian
#if 0
#define RCR_AMF					BIT13
#define RCR_ADF					BIT11
#define RCR_ACF                                 BIT12 
#define RCR_ACRC32				BIT8
#endif
#define RCR_CBSSID_BCN			BIT7
#define RCR_CBSSID_DATA		BIT6
//#define RCR_APWRMGT                     BIT5
#define RCR_AB					BIT3
#define RCR_AM					BIT2
#define RCR_APM					BIT1
#define RCR_AAP					BIT0

#define AFSM_HSUS				BIT(11)
#define BOOT_FROM_EEPROM		BIT(4)
#define EEPROMSEL				BIT(4)
#define EEPROM_EN				BIT(5)

#define SCR_TxEncEnable			BIT(2)
#define SCR_RxDecEnable			BIT(3)
#define SCR_NoSKMC				BIT(5)

#define MAX_TX_AGG_PACKET_NUMBER 	0xFF

#define wMBOX0_EVT_HOST_CLOSE		0x00

#define LAST_ENTRY_OF_TX_PKT_BUFFER_9086X		255





#define 		bRFRegOffsetMask	0xfffff

#define DESC_RATE1M					0x00
#define DESC_RATE2M					0x01
#define DESC_RATE5_5M				0x02
#define DESC_RATE11M				0x03

#define DESC_RATE6M					0x04
#define DESC_RATE9M					0x05
#define DESC_RATE12M				0x06
#define DESC_RATE18M				0x07
#define DESC_RATE24M				0x08
#define DESC_RATE36M				0x09
#define DESC_RATE48M				0x0a
#define DESC_RATE54M				0x0b

#define DESC_RATEMCS0				0x0c
#define DESC_RATEMCS1				0x0d
#define DESC_RATEMCS2				0x0e
#define DESC_RATEMCS3				0x0f
#define DESC_RATEMCS4				0x10
#define DESC_RATEMCS5				0x11
#define DESC_RATEMCS6				0x12
#define DESC_RATEMCS7				0x13

#define HDATA_RATE(rate)\
(rate == DESC_RATE1M)?"CCK_1M" :\
(rate == DESC_RATE2M)?"CCK_2M" :\
(rate == DESC_RATE5_5M)?"CCK5_5M" :\
(rate == DESC_RATE11M)?"CCK_11M" :\
(rate == DESC_RATE6M)?"OFDM_6M" :\
(rate == DESC_RATE9M)?"OFDM_9M" :\
(rate == DESC_RATE12M)?"OFDM_12M" :\
(rate == DESC_RATE18M)?"OFDM_18M" :\
(rate == DESC_RATE24M)?"OFDM_24M" :\
(rate == DESC_RATE36M)?"OFDM_36M" :\
(rate == DESC_RATE48M)?"OFDM_48M" :\
(rate == DESC_RATE54M)?"OFDM_54M" :\
(rate == DESC_RATEMCS0)?"MCS0" :\
(rate == DESC_RATEMCS1)?"MCS1" :\
(rate == DESC_RATEMCS2)?"MCS2" :\
(rate == DESC_RATEMCS3)?"MCS3" :\
(rate == DESC_RATEMCS4)?"MCS4" :\
(rate == DESC_RATEMCS5)?"MCS5" :\
(rate == DESC_RATEMCS6)?"MCS6" :\
(rate == DESC_RATEMCS7)?"MCS7" : "UNKNOWN"

enum {
	UP_LINK,
	DOWN_LINK,
};
typedef enum _WP_MEDIA_STATUS {
	WP_MEDIA_DISCONNECT = 0,
	WP_MEDIA_CONNECT = 1
} WP_MEDIA_STATUS;

#define MAX_DLFW_PAGE_SIZE			4096
typedef enum _FIRMWARE_SOURCE {
	FW_SOURCE_IMG_FILE = 0,
	FW_SOURCE_HEADER_FILE = 1,
} FIRMWARE_SOURCE, *PFIRMWARE_SOURCE;

typedef enum _CH_SW_USE_CASE {
	CH_SW_USE_CASE_TDLS = 0,
	CH_SW_USE_CASE_MCC = 1
} CH_SW_USE_CASE;

#define QSLT_BK							0x2
#define QSLT_BE							0x0
#define QSLT_VI							0x5
#define QSLT_VO							0x7
#define QSLT_CMD						0x13

#define TX_SELE_HQ			BIT(0)
#define TX_SELE_LQ			BIT(1)
#define TX_SELE_NQ			BIT(2)
#define TX_SELE_EQ			BIT(3)
#define XMIT_VO_QUEUE (0)
#define XMIT_VI_QUEUE (1)
#define XMIT_BE_QUEUE (2)
#define XMIT_BK_QUEUE (3)
#define	LDPC_HT_ENABLE_RX			BIT0
#define	LDPC_HT_ENABLE_TX			BIT1
#define	LDPC_HT_CAP_TX				BIT3
#define SEC_CAP_CHK_BMC	BIT0
#define SEC_STATUS_STA_PK_GK_CONFLICT_DIS_BMC_SEARCH	BIT0

#define	STBC_HT_ENABLE_RX			BIT0
#define	STBC_HT_ENABLE_TX			BIT1
#define	STBC_HT_CAP_TX				BIT3
#define PageNum_128(_Len)		(u32)(((_Len)>>7) + ((_Len)&0x7F ? 1:0))
#define PageNum_256(_Len)		(u32)(((_Len)>>8) + ((_Len)&0xFF ? 1:0))
#define PageNum_512(_Len)		(u32)(((_Len)>>9) + ((_Len)&0x1FF ? 1:0))
#define PageNum(_Len, _Size)		(u32)(((_Len)/(_Size)) + ((_Len)&((_Size) - 1) ? 1:0))

struct dbg_rx_counter {
	u32 rx_pkt_ok;
	u32 rx_pkt_crc_error;
	u32 rx_pkt_drop;
	u32 rx_ofdm_fa;
	u32 rx_cck_fa;
	u32 rx_ht_fa;
};
void Func_Of_Proc_Dump_Mac_Rx_Counters(PNIC Nic,
							  struct dbg_rx_counter *rx_counter);
void Func_Of_Proc_Dump_Phy_Rx_Counters(PNIC Nic,
							  struct dbg_rx_counter *rx_counter);

#ifdef DBG_RX_COUNTER_DUMP
#define DUMP_DRV_RX_COUNTER	BIT0
#define DUMP_MAC_RX_COUNTER	BIT1
#define DUMP_PHY_RX_COUNTER	BIT2
#define DUMP_DRV_TRX_COUNTER_DATA	BIT3

void Func_Of_Proc_Dump_Rx_Counters(PNIC Nic);
#endif

void Func_Of_Proc_Chip_Hw_Config_Rftype(PNIC Nic);

#define BAND_CAP_2G			BIT0
#define BAND_CAP_BIT_NUM	1

#define BW_CAP_5M		BIT0
#define BW_CAP_10M		BIT1
#define BW_CAP_20M		BIT2
#define BW_CAP_40M		BIT3
#define BW_CAP_BIT_NUM	4

#define PROTO_CAP_11B		BIT0
#define PROTO_CAP_11G		BIT1
#define PROTO_CAP_11N		BIT2
#define PROTO_CAP_11AC		BIT3
#define PROTO_CAP_BIT_NUM	4

#define WL_FUNC_P2P			BIT0
#define WL_FUNC_MIRACAST	BIT1
#define WL_FUNC_TDLS		BIT2
#define WL_FUNC_FTM			BIT3
#define WL_FUNC_BIT_NUM		4


#define RECV_TYEP_AAP			BIT(0)				// accept all 
#define RECV_TYEP_APM			BIT(1)				// accept physical match
#define RECV_TYEP_AM			BIT(2)				// accept multicast
#define RECV_TYEP_AB			BIT(3)				// accept broadcast
#define RECV_TYEP_ADF			BIT(18)				// accept Data(frame type) frame
#define RECV_TYEP_ACF			BIT(19)				// accept control frame
#define RECV_TYEP_AMF			BIT(20)				// accept management frame
#define RECV_TYEP_APWRMGT		BIT(22)				// accept power management packet
#define RECV_TYEP_APP_PHYST_RXFF		BIT(28)		// enable mac auto reset phy


void Func_Dump_Hal_Spec(void *sel, PNIC Nic);

bool Func_Chip_Hw_Chk_Band_Cap(PNIC Nic, u8 cap);
bool Func_Chip_Hw_Chk_Bw_Cap(PNIC Nic, u8 cap);
bool Func_Chip_Hw_Chk_Proto_Cap(PNIC Nic, u8 cap);
bool Func_Chip_Hw_Is_Band_Support(PNIC Nic, u8 band);
bool Func_Chip_Hw_Is_Bw_Support(PNIC Nic, u8 bw);
bool Func_Chip_Hw_Is_Wireless_Mode_Support(PNIC Nic, u8 mode);
u8 Func_Chip_Hw_Largest_Bw(PNIC Nic, u8 in_bw);

bool Func_Chip_Hw_Chk_Wl_Func(PNIC Nic, u8 func);

u8 Func_Chip_Hw_Com_Config_Channel_Plan(IN PNIC Nic,
							   IN char *hw_alpha2,
							   IN u8 hw_chplan,
							   IN char *sw_alpha2,
							   IN u8 sw_chplan,
							   IN u8 def_chplan, IN BOOLEAN AutoLoadFail);

int Func_Chip_Hw_Config_Macaddr(PNIC Nic, bool autoload_fail);

BOOLEAN Func_Chip_Hw_Islegalchannel(IN PNIC Nic, IN u32 Channel);

u8 Func_Mratetohwrate(u8 rate);

u8 Func_Hwratetomrate(u8 rate);

void Func_Chip_Hw_Setbratecfg(IN PNIC Nic,
					IN u8 * mBratesOS, OUT u16 * pBrateCfg);

BOOLEAN Func_Chip_Hw_Mappingoutpipe(IN PNIC Nic, IN u8 NumOutPipe);

void Func_Chip_Hw_Init_Macaddr(PNIC Nic);

void Func_Of_Proc_Init_Hal_Com_Default_Value(PNIC Nic);

void Func_Dev_To_Host_Evt_Clear(PNIC Nic);
s32 Func_Dev_To_Host_Evt_Read(PNIC Nic, u8 * buf);
s32 Func_Dev_To_Host_Evt_Read_Process(PNIC Nic, u8 * buf);

//////need change 
void Func_Of_Proc_Chip_Hw_Update_Sta_Rate_Mask(PNIC Nic, struct sta_info *psta);

void Func_Of_Proc_Sec_Write_Cam_Ent(PNIC Nic, u8 id, u16 ctrl, u8 * mac,
						   u8 * key);
bool Func_Of_Proc_Sec_Read_Cam_Is_Gk(PNIC Nic, u8 id);

void SetHwReg(PNIC Nic, u8 variable, u8 * val);
void Func_To_Get_Hwreg(PNIC Nic, u8 variable, u8 * val);

void Func_Mcu_Sethwreg(PNIC Nic, u8 variable, u8 * val);
void umsg_GetHwReg(PNIC Nic, u8 variable, u8 * val);

void Func_Of_Proc_Chip_Hw_Check_Rxfifo_Full(PNIC Nic);

u8 Func_To_Get_Hal_Def_Var(PNIC Nic, HAL_DEF_VARIABLE variable, void *value);

BOOLEAN Func_Eqnbyte(u8 * str1, u8 * str2, u32 num);


BOOLEAN
Func_To_Get_Hex_Value_From_String(IN char *szStr,
					  IN OUT u32 * pu4bVal, IN OUT u32 * pu4bMove);

BOOLEAN
Func_To_Get_Fraction_Value_From_String(IN char *szStr,
						   IN OUT u8 * pInteger,
						   IN OUT u8 * pFraction, IN OUT u32 * pu4bMove);

BOOLEAN Func_Iscommentstring(IN char *szStr);

BOOLEAN
Func_Parsequalifiedstring(IN char *In,
					 IN OUT u32 * Start,
					 OUT char *Out,
					 IN char LeftQualifier, IN char RightQualifier);

BOOLEAN Func_To_Get_U1Byteinteger_From_String_In_Decimal(IN char *Str, IN OUT u8 * pInt);

BOOLEAN Func_Isallspaceortab(u8 * data, u8 size);

#if 0
void Func_Linked_Info_Dump(PNIC Nic, u8 benable);
#endif

#ifdef DBG_RX_SIGNAL_DISPLAY_RAW_DATA
void Func_Of_Proc_Get_Raw_Rssi_Info(void *sel, PNIC Nic);
void Func_Of_Proc_Store_Phy_Info(PNIC Nic, union recv_frame *prframe);
void Func_Of_Proc_Dump_Raw_Rssi_Info(PNIC Nic, void *sel);
#endif

#define		HWSET_MAX_SIZE			512

int Func_Check_Phy_Efuse_Tx_Power_Info_Valid(PNIC Nic);
int Func_Chip_Hw_Efuse_Macaddr_Offset(PNIC Nic);
int Func_Chip_Hw_Getphyefusemacaddr(PNIC Nic, u8 * mac_addr);

#ifdef CONFIG_RF_POWER_TRIM
void Func_Of_Proc_Bb_Rf_Gain_Offset(PNIC Nic);
#endif

void Func_Dm_Dynamicusbtxagg(PNIC Nic, u8 from_timer);
u8 Func_Of_Proc_Chip_Hw_Busagg_Qsel_Check(PNIC Nic, u8 pre_qsel, u8 next_qsel);
void GetHalMSGVar(PNIC Nic,
				  HAL_MSG_VARIABLE eVariable, PVOID pValue1, PVOID pValue2);
void SetHalMSGVar(PNIC Nic,
				  HAL_MSG_VARIABLE eVariable, PVOID pValue1, BOOLEAN bSet);

#ifdef CONFIG_BACKGROUND_NOISE_MONITOR
struct noise_info {
	u8 bPauseDIG;
	u8 IGIValue;
	u32 max_time;
	u8 chan;
};
#endif

void Func_Of_Proc_Get_Noise(PNIC Nic);
u8 Func_Of_Proc_Get_Current_Tx_Rate(PNIC Nic, u8 macid);
void Func_Of_Proc_Chip_Hw_Set_Fw_Rsvd_Page(PNIC Nic, bool finished);
void Func_Of_Proc_Chip_Hw_Set_Fwrsvdpage_Cmd(PNIC Nic, PRSVDPAGE_LOC prsvdpageloc);

s8 Func_Of_Proc_Chip_Hw_Ch_Sw_Iqk_Info_Search(PNIC Nic, u8 central_chnl,
								 u8 bw_mode);
void Func_Of_Proc_Chip_Hw_Ch_Sw_Iqk_Info_Backup(PNIC Nic);
void Func_Of_Proc_Chip_Hw_Ch_Sw_Iqk_Info_Restore(PNIC Nic, u8 ch_sw_use_case);


typedef enum _HAL_PHYDM_OPS {
	HAL_PHYDM_DIS_ALL_FUNC,
	HAL_PHYDM_FUNC_SET,
	HAL_PHYDM_FUNC_CLR,
	HAL_PHYDM_ABILITY_BK,
	HAL_PHYDM_ABILITY_RESTORE,
	HAL_PHYDM_ABILITY_SET,
	HAL_PHYDM_ABILITY_GET,
} HAL_PHYDM_OPS;

#define DYNAMIC_FUNC_DISABLE		(0x0)
u32 Func_Of_Proc_Phydm_Ability_Ops(PNIC Nic, HAL_PHYDM_OPS ops, u32 ability);

#define wl_phydm_func_disable_all(wadptdata)	\
	Func_Of_Proc_Phydm_Ability_Ops(wadptdata, HAL_PHYDM_DIS_ALL_FUNC, 0)

#define wl_phydm_func_for_offchannel(wadptdata) \
	do { \
		Func_Of_Proc_Phydm_Ability_Ops(wadptdata, HAL_PHYDM_DIS_ALL_FUNC, 0); \
	} while (0)

#define wl_phydm_func_set(wadptdata, ability)	\
	Func_Of_Proc_Phydm_Ability_Ops(wadptdata, HAL_PHYDM_FUNC_SET, ability)

#define wl_phydm_func_clr(wadptdata, ability)	\
	Func_Of_Proc_Phydm_Ability_Ops(wadptdata, HAL_PHYDM_FUNC_CLR, ability)

#define wl_phydm_ability_backup(wadptdata)	\
	Func_Of_Proc_Phydm_Ability_Ops(wadptdata, HAL_PHYDM_ABILITY_BK, 0)

#define wl_phydm_ability_restore(wadptdata)	\
	Func_Of_Proc_Phydm_Ability_Ops(wadptdata, HAL_PHYDM_ABILITY_RESTORE, 0)

#define wl_phydm_ability_set(wadptdata, ability)	\
	Func_Of_Proc_Phydm_Ability_Ops(wadptdata, HAL_PHYDM_ABILITY_SET, ability)

static inline u32 wl_phydm_ability_get(PNIC Nic)
{
	return Func_Of_Proc_Phydm_Ability_Ops(Nic, HAL_PHYDM_ABILITY_GET, 0);
}

#ifdef CONFIG_FW_wMBOX0_DEBUG
void Func_Debug_Fwc2H(PNIC Nic, u8 * pdata, u8 len);
#endif

void Func_Update_Iot_Info(PNIC Nic);

#ifdef CONFIG_AUTO_CHNL_SEL_NHM
void Func_Of_Proc_Acs_Start(PNIC Nic, bool bStart);
#endif

void Func_Chip_Hw_Set_Crystal_Cap(PNIC Nic, u8 crystal_cap);
void Func_Wf_Set_Fw_Lps_Config(PNIC Nic);

#endif
