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

#ifndef __HAL_INTF_H__
#define __HAL_INTF_H__

#define WL_USB BIT1

typedef enum _HAL_HW_TIMER_TYPE {
	HAL_TIMER_NONE = 0,
	HAL_TIMER_TXBF = 1,
	HAL_TIMER_EARLYMODE = 2,
} HAL_HW_TIMER_TYPE, *PHAL_HW_TIMER_TYPE;

typedef enum _HW_VARIABLES {
	HW_VAR_MEDIA_STATUS,
	HW_VAR_MEDIA_STATUS1,
	HW_VAR_SET_OPMODE,
	HW_VAR_MAC_ADDR,
	HW_VAR_BSSID,
	HW_VAR_INIT_RTS_RATE,
	HW_VAR_BASIC_RATE,
	HW_VAR_TXPAUSE,
	HW_VAR_BCN_FUNC,
	HW_VAR_CORRECT_TSF,
	HW_VAR_CHECK_BSSID,
	HW_VAR_MLME_DISCONNECT,
	HW_VAR_MLME_SITESURVEY,
	HW_VAR_MLME_JOIN,
	HW_VAR_ON_RCR_AM,
	HW_VAR_OFF_RCR_AM,
	HW_VAR_BEACON_INTERVAL,
	HW_VAR_SLOT_TIME,
	HW_VAR_RESP_SIFS,
	HW_VAR_ACK_PREAMBLE,
	HW_VAR_SEC_CFG,
	HW_VAR_SEC_DK_CFG,
	HW_VAR_BCN_VALID,
	HW_VAR_RF_TYPE,
	HW_VAR_CAM_EMPTY_ENTRY,
	HW_VAR_CAM_INVALID_ALL,
	HW_VAR_AC_PARAM_VO,
	HW_VAR_AC_PARAM_VI,
	HW_VAR_AC_PARAM_BE,
	HW_VAR_AC_PARAM_BK,
	HW_VAR_ACM_CTRL,
	HW_VAR_AMPDU_MIN_SPACE,
	HW_VAR_AMPDU_FACTOR,
	HW_VAR_RXDMA_AGG_PG_TH,
	HW_VAR_SET_RPWM,
	HW_VAR_wFPRS,
	HW_VAR_wMBOX1_FW_PWRMODE,
	HW_VAR_wMBOX1_PS_TUNE_PARAM,
	HW_VAR_wMBOX1_FW_JOINBSSRPT,
	HW_VAR_FWLPS_RF_ON,
	HW_VAR_wMBOX1_FW_P2P_PS_OFFLOAD,
	HW_VAR_TRIGGER_GPIO_0,
	HW_VAR_PRIV_0,
	HW_VAR_PRIV_1,	
	HW_VAR_SWITCH_EPHY_WoWLAN,
	HW_VAR_EFUSE_USAGE,
	HW_VAR_EFUSE_BYTES,
	HW_VAR_PRIV_2,
	HW_VAR_PRIV_3,  //30
	HW_VAR_FIFO_CLEARN_UP,
	HW_VAR_RESTORE_HW_SEQ,
	HW_VAR_CHECK_TXBUF,
	HW_VAR_PRIV_4,
	HW_VAR_APFM_ON_MAC,
	HW_VAR_HCI_SUS_STATE,
	HW_VAR_SYS_CLKR,
	HW_VAR_NAV_UPPER,
	HW_VAR_wMBOX0_HANDLE,
	HW_VAR_RPT_TIMER_SETTING,
	HW_VAR_TX_RPT_MAX_MACID,
	HW_VAR_CHK_HI_QUEUE_EMPTY,
	HW_VAR_DL_BCN_SEL,
	HW_VAR_AMPDU_MAX_TIME,
	HW_VAR_WIRELESS_MODE,
	HW_VAR_USB_MODE,
	HW_VAR_PORT_SWITCH,
	HW_VAR_DO_IQK,
	HW_VAR_DM_IN_LPS,
	HW_VAR_SET_REQ_FW_PS,
	HW_VAR_FW_PS_STATE,
	HW_VAR_SOUNDING_ENTER,
	HW_VAR_SOUNDING_LEAVE,
	HW_VAR_SOUNDING_RATE,
	HW_VAR_SOUNDING_STATUS,
	HW_VAR_SOUNDING_FW_NDPA,
	HW_VAR_SOUNDING_CLK,
	HW_VAR_HW_REG_TIMER_INIT,
	HW_VAR_HW_REG_TIMER_RESTART,
	HW_VAR_HW_REG_TIMER_START,
	HW_VAR_HW_REG_TIMER_STOP,
	HW_VAR_DL_RSVD_PAGE,
	HW_VAR_MACID_LINK,
	HW_VAR_MACID_NOLINK,
	HW_VAR_MACID_SLEEP,
	HW_VAR_MACID_WAKEUP,
	HW_VAR_DUMP_MAC_QUEUE_INFO,
	HW_VAR_ASIX_IOT,
	HW_VAR_EN_HW_UPDATE_TSF,
	HW_VAR_CH_SW_NEED_TO_TAKE_CARE_IQK_INFO,
	HW_VAR_CH_SW_IQK_INFO_BACKUP,
	HW_VAR_CH_SW_IQK_INFO_RESTORE,
	HW_VAR_BCN_VALID1,
	HW_VAR_WOWLAN,
	HW_VAR_WAKEUP_REASON,
	HW_VAR_RPWM_TOG,
	HW_SET_GPIO_WL_CTRL,
} HW_VARIABLES;

typedef enum _HAL_DEF_VARIABLE {
	HAL_DEF_UNDERCORATEDSMOOTHEDPWDB,
	HAL_DEF_IS_SUPPORT_ANT_DIV,
	HAL_DEF_DRVINFO_SZ,
	HAL_DEF_MAX_RECVBUF_SZ,
	HAL_DEF_RX_PACKET_OFFSET,
	HAL_DEF_RX_DMA_SZ_WOW,
	HAL_DEF_RX_DMA_SZ,
	HAL_DEF_RX_PAGE_SIZE,
	HAL_DEF_DBG_DUMP_RXPKT,
	HAL_DEF_RA_DECISION_RATE,
	HAL_DEF_RA_SGI,
	HAL_DEF_PT_PWR_STATUS,
	HAL_DEF_TX_LDPC,
	HAL_DEF_RX_LDPC,
	HAL_DEF_TX_STBC,
	HAL_DEF_RX_STBC,
	HW_VAR_MAX_RX_AMPDU_FACTOR,
	HW_DEF_RA_INFO_DUMP,
	HAL_DEF_DBG_DUMP_TXPKT,

	HAL_DEF_TX_PAGE_SIZE,
	HAL_DEF_TX_PAGE_BOUNDARY,
	HAL_DEF_TX_PAGE_BOUNDARY_WOWLAN,
	HAL_DEF_ANT_DETECT,
	HAL_DEF_MACID_SLEEP,
	HAL_DEF_DBG_DIS_PWT,
	HAL_DEF_EFUSE_USAGE,
	HAL_DEF_EFUSE_BYTES,
	HW_VAR_BEST_AMPDU_DENSITY,
} HAL_DEF_VARIABLE;

typedef enum _HAL_MSG_VARIABLE {
	HAL_MSG_STA_INFO,
	HAL_MSG_P2P_STATE,
	HAL_MSG_WIFI_DISPLAY_STATE,
	HAL_MSG_NOISE_MONITOR,
	HAL_MSG_REGULATION,
	HAL_MSG_INITIAL_GAIN,
	HAL_MSG_FA_CNT_DUMP,
	HAL_MSG_DBG_FLAG,
	HAL_MSG_DBG_LEVEL,
	HAL_MSG_RX_INFO_DUMP,
#ifdef CONFIG_AUTO_CHNL_SEL_NHM
	HAL_MSG_AUTO_CHNL_SEL,
#endif
} HAL_MSG_VARIABLE;

typedef enum _HAL_INTF_PS_FUNC {
	HAL_USB_SELECT_SUSPEND,
	HAL_MAX_ID,
} HAL_INTF_PS_FUNC;

typedef s32(*wmbox0_id_filter) (u8 * wmbox0_evt);

#if 0
struct hal_ops {
	u32(*hal_init) (PNIC Nic);
	u32(*hal_deinit) (PNIC Nic);

	void (*enable_interrupt) (PNIC Nic);
	void (*disable_interrupt) (PNIC Nic);

	void (*hal_dm_watchdog) (PNIC Nic);
	wmbox0_id_filter wmbox0_id_filter_ccx;

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
	void (*clear_interrupt) (PNIC Nic);
#endif

	void (*fw_correct_bcn) (PNIC Nic);
};

#define RF_CHANGE_BY_INIT	0
#define RF_CHANGE_BY_IPS 	BIT28
#define RF_CHANGE_BY_PS 	BIT29
#define RF_CHANGE_BY_HW 	BIT30
#define RF_CHANGE_BY_SW 	BIT31

typedef enum _wowlan_subcode {
	WOWLAN_ENABLE = 0,
	WOWLAN_DISABLE = 1,
	WOWLAN_AP_ENABLE = 2,
	WOWLAN_AP_DISABLE = 3,
	WOWLAN_PATTERN_CLEAN = 4
} wowlan_subcode;
#endif

struct wowlan_ioctl_param {
	unsigned int subcode;
	unsigned int subcode_value;
	unsigned int wakeup_reason;
};

#define Rx_Pairwisekey			0x01
#define Rx_GTK					0x02
#define Rx_DisAssoc				0x04
#define Rx_DeAuth				0x08
#define Rx_ARPReq				0x09
#define FWDecisionDisconnect	0x10
#define Rx_MagicPkt				0x21
#define Rx_UnicastPkt			0x22
#define Rx_PatternPkt			0x23
#define	RX_PNOWakeUp			0x55
#define	AP_WakeUp			0x66

u8 Func_Of_Proc_Chip_Hw_Data_Init(PNIC Nic);
void Func_Of_Proc_Chip_Hw_Data_Deinit(PNIC Nic);

void Func_Of_Proc_Chip_Hw_Def_Value_Init(PNIC Nic);

void Func_Of_Proc_Chip_Hw_Free_Data(PNIC Nic);

void Func_Of_Proc_Chip_Hw_Dm_Init(PNIC Nic);
void Func_Of_Proc_Chip_Hw_Dm_Deinit(PNIC Nic);

u32 Func_Of_Proc_Chip_Hw_Power_On(PNIC Nic);
void Func_Of_Proc_Chip_Hw_Power_Off(PNIC Nic);

uint Func_Of_Proc_Chip_Hw_Init(PNIC Nic);
uint Func_Of_Proc_Chip_Hw_Deinit(PNIC Nic);
void wl_hal_stop(PNIC Nic);
void Func_Of_Proc_Chip_Hw_Set_Hwreg(PNIC Nic, u8 variable, u8 * val);
void Func_Of_Proc_Chip_Hw_Get_Hwreg(PNIC Nic, u8 variable, u8 * val);

void Func_Of_Proc_Chip_Hw_Set_Hwreg_With_Buf(PNIC Nic, u8 * pbuffer, int length);

void Func_Of_Proc_Chip_Hw_Chip_Configure(PNIC Nic);
void Func_Of_Proc_Chip_Hw_Read_Chip_Info(PNIC Nic);
void Func_Of_Proc_Chip_Hw_Read_Chip_Version(PNIC Nic);

u8 Func_Of_Proc_Chip_Hw_Get_Def_Var(PNIC Nic, HAL_DEF_VARIABLE eVariable,
					   PVOID pValue);

void Func_Of_Proc_Chip_Hw_Set_Odm_Var(PNIC Nic, HAL_MSG_VARIABLE eVariable,
						 PVOID pValue1, BOOLEAN bSet);
void Func_Of_Proc_Chip_Hw_Get_Odm_Var(PNIC Nic, HAL_MSG_VARIABLE eVariable,
						 PVOID pValue1);

u32 Func_Of_Proc_Chip_Hw_Inirp_Init(PNIC Nic);
u32 Func_Of_Proc_Chip_Hw_Inirp_Deinit(PNIC Nic);

#if 0 //tangjian
s32 do_hal_txp_enqueue(PNIC Nic,
							  struct xmit_frame *pxmitframe, u8 tag);
s32 Func_Of_Proc_Chip_Hw_Mgnt_Xmit(PNIC Nic, struct xmit_frame *pmgntframe);

#endif
void Func_Of_Proc_Chip_Hw_Update_Ra_Mask(struct sta_info *psta, u8 rssi_level);


#ifdef CONFIG_AP_MODE
void Func_Of_Proc_Chip_Hw_Add_Ra_Tid(PNIC Nic, u64 bitmap, u8 * arg,
						u8 rssi_level);
#endif

void Func_Of_Proc_Chip_Hw_Start_Thread(PNIC Nic);
void Func_Of_Proc_Chip_Hw_Stop_Thread(PNIC Nic);

u32 Func_Of_Proc_Chip_Hw_Read_Bbreg(PNIC Nic, u32 RegAddr, u32 BitMask);
void Func_Of_Proc_Chip_Hw_Write_Bbreg(PNIC Nic, u32 RegAddr, u32 BitMask,
						 u32 Data);
u32 Func_Of_Proc_Chip_Hw_Read_Rfreg(PNIC Nic, u32 eRFPath, u32 RegAddr,
					   u32 BitMask);
void Func_Of_Proc_Chip_Hw_Write_Rfreg(PNIC Nic, u32 eRFPath, u32 RegAddr,
						 u32 BitMask, u32 Data);

#define PHY_QueryBBReg(Adapter, RegAddr, BitMask) Func_Of_Proc_Chip_Hw_Read_Bbreg((Adapter), (RegAddr), (BitMask))
#define PHY_SetBBReg(Adapter, RegAddr, BitMask, Data) Func_Of_Proc_Chip_Hw_Write_Bbreg((Adapter), (RegAddr), (BitMask), (Data))
#define PHY_QueryRFReg(Adapter, eRFPath, RegAddr, BitMask) Func_Of_Proc_Chip_Hw_Read_Rfreg((Adapter), (eRFPath), (RegAddr), (BitMask))
#define PHY_SetRFReg(Adapter, eRFPath, RegAddr, BitMask, Data) Func_Of_Proc_Chip_Hw_Write_Rfreg((Adapter), (eRFPath), (RegAddr), (BitMask), (Data))

#define PHY_SetMacReg	PHY_SetBBReg
#define PHY_QueryMacReg PHY_QueryBBReg

void Func_Of_Proc_Chip_Hw_Set_Chnl_Bw(PNIC Nic, u8 channel,
						 CHANNEL_WIDTH Bandwidth, u8 Offset40);
void Func_Of_Proc_Chip_Hw_Dm_Watchdog(PNIC Nic);
void wl_hal_dm_watchdog_in_lps(PNIC Nic);

#ifdef CONFIG_HOSTAPD_MLME
s32 Func_Of_Proc_Chip_Hw_Hostap_Mgnt_Xmit_Entry(PNIC Nic, _pkt * pkt);
#endif

bool Func_Of_Proc_Chip_Hw_wMBOX0_Valid(PNIC Nic, u8 * buf);
s32 Func_Of_Proc_Chip_Hw_wMBOX0_Evt_Read(PNIC Nic, u8 * buf);
s32 Func_Of_Proc_Chip_Hw_wMBOX0_Handler(PNIC Nic, u8 * wmbox0_evt);
bool Func_Of_Proc_Chip_Hw_wMBOX0_Id_Filter_Ccx(u8 * buf);

s32 Func_Of_Proc_Chip_Hw_Is_Disable_Sw_Channel_Plan(PNIC Nic);

s32 Func_Of_Proc_Chip_Hw_Macid_Sleep(PNIC Nic, u8 macid);
s32 Func_Of_Proc_Chip_Hw_Macid_Wakeup(PNIC Nic, u8 macid);

s32 Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(PNIC Nic, u8 ElementID, u32 CmdLen,
						 u8 * pCmdBuffer);
void wl_hal_fill_fake_txdesc(PNIC Nic, u8 * pDesc, u32 BufferLen,
							  u8 IsPsPoll, u8 IsBTQosNull, u8 bDataFrame);
u8 wl_hal_get_txbuff_rsvd_page_num(PNIC Nic, bool wowlan);

void Func_Of_Proc_Chip_Hw_Fw_Correct_Bcn(PNIC Nic);
s32 Func_Of_Proc_Chip_Hw_Fw_Dl(PNIC Nic, u8 wowlan);

#endif
