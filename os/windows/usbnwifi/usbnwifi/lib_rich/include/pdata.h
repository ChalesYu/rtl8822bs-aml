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

#ifndef __HAL_DATA_H__
#define __HAL_DATA_H__

typedef enum _WP_MULTI_FUNC {
	WP_MULTI_FUNC_NONE = 0x00,
	WP_MULTI_FUNC_WIFI = 0x01,
	WP_MULTI_FUNC_BT = 0x02,
	WP_MULTI_FUNC_GPS = 0x04,
} WP_MULTI_FUNC, *WTL_MULTI_FUNC;
typedef enum _WP_POLARITY_CTL {
	WP_POLARITY_LOW_ACT = 0,
	WP_POLARITY_HIGH_ACT = 1,
} WP_POLARITY_CTL, *WTL_POLARITY_CTL;

typedef enum _WP_REGULATOR_MODE {
	WP_SWITCHING_REGULATOR = 0,
	WP_LDO_REGULATOR = 1,
} WP_REGULATOR_MODE, *WTL_REGULATOR_MODE;

typedef enum _INTERFACE_SELECT_PCIE {
	INTF_SEL0_SOLO_MINICARD = 0,
	INTF_SEL1_BT_COMBO_MINICARD = 1,
	INTF_SEL2_PCIe = 2,
} INTERFACE_SELECT_PCIE, *PINTERFACE_SELECT_PCIE;



typedef enum _WP_AMPDU_BRUST_MODE {
	WP_AMPDU_BRUST_NONE = 0,
	WP_AMPDU_BRUST_LPD = 1,
	WP_AMPDU_BRUST_SWD = 2,
	WP_AMPDU_BRUST_PDO = 3,
	WP_AMPDU_BRUST_MPO = 4,
	WP_AMPDU_BRUST_TLI = 5,
	WP_AMPDU_BRUST_WLI = 6,
	WP_AMPDU_BRUST_SLK = 7,
} WP_AMPDU_BRUST, *WTL_AMPDU_BRUST_MODE;

#define CHANNEL_GROUP_MAX		(3 + 9)
#define MAX_PG_GROUP			13

#define MAX_REGULATION_NUM						4
#define MAX_RF_PATH_NUM_IN_POWER_LIMIT_TABLE	4
#define MAX_2_4G_BANDWIDTH_NUM					2
#define MAX_RATE_SECTION_NUM						10

#define MAX_BASE_NUM_IN_PHY_REG_PG_2_4G			10


#ifdef CONFIG_USB_RX_AGGREGATION
typedef enum _USB_RX_AGG_MODE {
	USB_RX_AGG_DISABLE,
	USB_RX_AGG_DMA,
	USB_RX_AGG_USB,
	USB_RX_AGG_MIX
} USB_RX_AGG_MODE;

#endif

typedef struct _BB_INIT_REGISTER {
	u16 offset;
	u32 value;

} BB_INIT_REGISTER, *PBB_INIT_REGISTER;

#define PAGE_SIZE_128	128
#define PAGE_SIZE_256	256
#define PAGE_SIZE_512	512

#define HCI_SUS_ENTER		0
#define HCI_SUS_LEAVING		1
#define HCI_SUS_LEAVE		2
#define HCI_SUS_ENTERING	3
#define HCI_SUS_ERR			4

#ifdef CONFIG_AUTO_CHNL_SEL_NHM
typedef enum _ACS_OP {
	ACS_INIT,
	ACS_RESET,
	ACS_SELECT,
} ACS_OP;

typedef enum _ACS_STATE {
	ACS_DISABLE,
	ACS_ENABLE,
} ACS_STATE;

struct auto_chan_sel {
	ATOMIC_T state;
	u8 ch;
};
#endif

#define EFUSE_FILE_UNUSED 0
#define EFUSE_FILE_FAILED 1
#define EFUSE_FILE_LOADED 2

#define MACADDR_FILE_UNUSED 0
#define MACADDR_FILE_FAILED 1
#define MACADDR_FILE_LOADED 2

#define KFREE_FLAG_ON				BIT0
#define KFREE_FLAG_THERMAL_K_ON		BIT1

#define MAX_IQK_INFO_BACKUP_CHNL_NUM 	5
#define MAX_IQK_INFO_BACKUP_REG_NUM		10

struct kfree_data_t {
	u8 flag;
	s8 bb_gain[1][1];
	s8 thermal;
};

bool Func_Kfree_Data_Is_Bb_Gain_Empty(struct kfree_data_t *data);

struct hal_spec_t {
	char *ic_name;
	u8 macid_num;

	u8 sec_cam_ent_num;
	u8 sec_cap;

	u8 nss_num;
	u8 band_cap;
	u8 bw_cap;
	u8 proto_cap;

	u8 wl_func;
};

struct hal_iqk_reg_backup {
	u8 central_chnl;
	u8 bw_mode;
	u32 reg_backup[1][MAX_IQK_INFO_BACKUP_REG_NUM];
};

typedef struct hal_com_data {
	WP_MULTI_FUNC MultiFunc;
	WP_POLARITY_CTL PolarityCtl;
	WP_REGULATOR_MODE RegulatorMode;
	u8 hw_init_completed;
	u16 FirmwareVersion;
	u16 FirmwareVersionRev;
	u16 FirmwareSubVersion;
	u16 FirmwareSignature;
	u8 RegFWOffload;
	u8 fw_ractrl;
	u8 FwRsvdPageStartOffset;
	u8 LastHMEBoxNum;

	CHANNEL_WIDTH CurrentChannelBW;
	BAND_TYPE CurrentBandType;
	BAND_TYPE BandSet;
	u8 CurrentChannel;
	u8 CurrentCenterFrequencyIndex1;
	u8 nCur40MhzPrimeSC;
	BOOLEAN bSwChnlAndSetBWInProgress;
	u8 bDisableSWChannelPlan;
	u16 BasicRateSet;
	u32 ReceiveConfig;
	BOOLEAN bSwChnl;
	BOOLEAN bSetChnlBW;
	BOOLEAN bSetPowLevel;
	BOOLEAN bChnlBWInitialized;
#ifdef CONFIG_AUTO_CHNL_SEL_NHM
	struct auto_chan_sel acs;
#endif
	u8 rf_chip;
	u8 rf_type;
	u8 PackageType;
	u8 NumTotalRFPath;

	u16 ForcedDataRate;
	u8 u1ForcedIgiLb;
	u8 bDumpRxPkt;
	u8 bDumpTxPkt;
	u8 bDisableTXPowerTraining;

	u8 bautoload_fail_flag;
	u8 efuse_file_status;
	u8 macaddr_file_status;
	u8 EepromOrEfuse;
	u8 efuse_eeprom_data[EEPROM_MAX_SIZE];
	u8 InterfaceSel;
	u16 CustomerID;

	u16 EEPROMVID;
	u16 EEPROMSVID;
	u16 EEPROMPID;
	u16 EEPROMSDID;
	u8 EEPROMCustomerID;
	u8 EEPROMSubCustomerID;
	u8 EEPROMVersion;
	u8 EEPROMRegulatory;
	u8 EEPROMThermalMeter;
	u8 EEPROMBluetoothCoexist;
	u8 EEPROMBluetoothType;
	u8 EEPROMBluetoothAntNum;
	u8 EEPROMBluetoothAntIsolation;
	u8 EEPROMBluetoothRadioShared;
	u8 bTXPowerDataReadFromEEPORM;
	u8 EEPROMMACAddr[ETH_ALEN];

#ifdef CONFIG_RF_POWER_TRIM
	u8 EEPROMRFGainOffset;
	u8 EEPROMRFGainVal;
	struct kfree_data_t kfree_data;
#endif

	u8 EfuseUsedPercentage;
	u16 EfuseUsedBytes;
	EFUSE_HAL EfuseHal;

	u8 Index24G_CCK_Base[1][CENTER_CH_2G_NUM];
	u8 Index24G_BW40_Base[1][CENTER_CH_2G_NUM];
	s8 CCK_24G_Diff[1][MAX_TX_COUNT];
	s8 OFDM_24G_Diff[1][MAX_TX_COUNT];
	s8 BW20_24G_Diff[1][MAX_TX_COUNT];
	s8 BW40_24G_Diff[1][MAX_TX_COUNT];

	u8 Regulation2_4G;

	u8 TxPwrInPercentage;

	u8 TxPwrByRateTable;
	u8 TxPwrByRateBand;
	s8 TxPwrByRateOffset[TX_PWR_BY_RATE_NUM_BAND]
		[TX_PWR_BY_RATE_NUM_RF]
		[TX_PWR_BY_RATE_NUM_RF]
		[TX_PWR_BY_RATE_NUM_RATE];

	u8 tx_pwr_lmt_5g_20_40_ref;

	s8 TxPwrLimit_2_4G[MAX_REGULATION_NUM]
		[MAX_2_4G_BANDWIDTH_NUM]
		[MAX_RATE_SECTION_NUM]
		[CENTER_CH_2G_NUM]
		[1];

	u8 TxPwrByRateBase2_4G[TX_PWR_BY_RATE_NUM_RF]
		[TX_PWR_BY_RATE_NUM_RF]
		[MAX_BASE_NUM_IN_PHY_REG_PG_2_4G];

	u8 txpwr_by_rate_loaded:1;
	u8 txpwr_by_rate_from_file:1;
	u8 txpwr_limit_loaded:1;
	u8 txpwr_limit_from_file:1;

	u8 PGMaxGroup;

	u8 CurrentCckTxPwrIdx;
	u8 CurrentOfdm24GTxPwrIdx;
	u8 CurrentBW2024GTxPwrIdx;
	u8 CurrentBW4024GTxPwrIdx;

	u8 pwrGroupCnt;
	u32 MCSTxPowerLevelOriginalOffset[MAX_PG_GROUP][16];
	u32 CCKTxPowerLevelOriginalOffset;

	u8 CrystalCap;

	u8 PAType_2G;
	u8 LNAType_2G;
	u8 ExternalPA_2G;
	u8 ExternalLNA_2G;
	u16 TypeGLNA;
	u16 TypeGPA;
	u16 TypeALNA;
	u16 TypeAPA;
	u16 RFEType;

	u8 bLedOpenDrain;
	u32 AcParam_BE;

	BB_REGISTER_DEFINITION_T PHYRegDef;

	u32 RfRegChnlVal[1];

	BOOLEAN bRDGEnable;

	u8 RegTxPause;
	u8 RegBcnCtrlVal;
	u8 RegFwHwTxQCtrl;
	u8 RegReg542;
	u8 RegCR_1;
	u8 Reg837;
	u16 RegRRSR;

	u8 AntDivCfg;
	u8 AntDetection;
	u8 TRxAntDivType;
	u8 ant_path;
	u32 AntennaTxPath;
	u32 AntennaRxPath;
	u8 sw_antdiv_bl_state;

	u8 DM_Type;
	KSPIN_LOCK  IQKSpinLock;
	u8 INIDATA_RATE[MACID_NUM_SW_LIMIT];

	int EntryMinUndecoratedSmoothedPWDB;
	int EntryMaxUndecoratedSmoothedPWDB;
	int MinUndecoratedPWDBForDM;

	u8 bIQKInitialized;
	u8 bNeedIQK;

	BOOLEAN pwrdown;

	u32 interfaceIndex;

#ifdef CONFIG_P2P
	u8 p2p_ps_offload;
#endif
	u8 bMacPwrCtrlOn;
	u8 hci_sus_state;

	u8 RegIQKFWOffload;
	
//  no used
//	struct submit_ctx iqk_sctx;

	WP_AMPDU_BRUST AMPDUBurstMode;

	u8 OutEpQueueSel;
	u8 OutEpNumber;

	BOOLEAN UsbRxHighSpeedMode;
	BOOLEAN UsbTxVeryHighSpeedMode;
	u32 UsbBulkOutSize;
	BOOLEAN bSupportUSB3;

	u32 IntArray[3];
	u32 IntrMask[3];
	u8 C2hArray[16];
#ifdef CONFIG_USB_TX_AGGREGATION
	u8 UsbTxAggMode;
	u8 UsbTxAggDescNum;
#endif

#ifdef CONFIG_USB_RX_AGGREGATION
	u16 HwRxPageSize;
	u32 MaxUsbRxAggBlock;

	USB_RX_AGG_MODE UsbRxAggMode;
	u8 UsbRxAggBlockCount;
	u8 UsbRxAggBlockTimeout;
	u8 UsbRxAggPageCount;
	u8 UsbRxAggPageTimeout;

	u8 RegAcUsbDmaSize;
	u8 RegAcUsbDmaTime;
#endif

	struct hal_spec_t hal_spec;

	u8 RfKFreeEnable;
	u8 RfKFree_ch_group;
	BOOLEAN bCCKinCH14;
	BB_INIT_REGISTER RegForRecover[5];

	struct hal_iqk_reg_backup iqk_reg_backup[MAX_IQK_INFO_BACKUP_CHNL_NUM];
} HAL_DATA_COMMON, *PHAL_DATA_COMMON;

typedef struct hal_com_data HAL_DATA_TYPE, *PHAL_DATA_TYPE;
#define GET_HAL_DATA(__pAdapter)			((HAL_DATA_TYPE *)(&(__pAdapter)->HalData))
#define GET_HAL_SPEC(__pAdapter)			(&(GET_HAL_DATA((__pAdapter))->hal_spec))
#define GET_MSG(__pAdapter)				(&(GET_HAL_DATA((__pAdapter))->msgpriv))

#define GET_HAL_RFPATH_NUM(__pAdapter)		(((HAL_DATA_TYPE *)((__pAdapter)->HalData))->NumTotalRFPath )
#define WP_GetInterfaceSelection(_Adapter) 		(GET_HAL_DATA(_Adapter)->InterfaceSel)
#define GET_RF_TYPE(__pAdapter)				(GET_HAL_DATA(__pAdapter)->rf_type)
#define GET_KFREE_DATA(_wadptdata) (&(GET_HAL_DATA((_wadptdata))->kfree_data))

#define	SUPPORT_HW_RADIO_DETECT(Adapter)	(	WP_GetInterfaceSelection(Adapter) == INTF_SEL2_MINICARD ||\
												WP_GetInterfaceSelection(Adapter) == INTF_SEL3_USB_Solo ||\
												WP_GetInterfaceSelection(Adapter) == INTF_SEL4_USB_Combo)

#define get_hal_mac_addr(wadptdata) 				(GET_HAL_DATA(wadptdata)->EEPROMMACAddr)
#define is_boot_from_eeprom(wadptdata) 			(GET_HAL_DATA(wadptdata)->EepromOrEfuse)
#define wl_get_hw_init_completed(wadptdata)		(GET_HAL_DATA(wadptdata)->hw_init_completed)
#define wl_is_hw_init_completed(wadptdata)		(GET_HAL_DATA(wadptdata)->hw_init_completed == _TRUE)


#ifdef CONFIG_AUTO_CHNL_SEL_NHM
#define GET_ACS_STATE(pwadptdata)					(Func_Atomic_Of_Read(&GET_HAL_DATA(pwadptdata)->acs.state))
#define SET_ACS_STATE(pwadptdata, set_state)			(Func_Atomic_Of_Set(&GET_HAL_DATA(pwadptdata)->acs.state, set_state))
#define wl_get_acs_channel(pwadptdata)				(GET_HAL_DATA(pwadptdata)->acs.ch)
#define wl_set_acs_channel(pwadptdata, survey_ch)	(GET_HAL_DATA(pwadptdata)->acs.ch = survey_ch)
#endif


uint Func_Loadparam(PNIC Nic);
u8 Func_Of_Proc_Init_Drv_Sw(PNIC Nic);



#endif
