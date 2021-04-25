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

#ifndef __wlan9086X_HAL_H__
#define __wlan9086X_HAL_H__




#define REG_HMEBOX_DBG_2_9086X	0x008C
#define REG_HISR0_9086X					0x00B4
#define REG_HISR1_9086X					0x00BC
#define REG_WOWLAN_WAKE_REASON 0x01C7
#define XMIT_ACK_REG      				0x0298

#define		rFPGA0_XA_HSSIParameter1		0x820
#define		rFPGA0_XA_HSSIParameter2		0x824
#define		rFPGA0_XA_LSSIParameter		0x840
#define		rFPGA0_XA_LSSIReadBack		0x8a0
#define		TransceiverA_HSPI_Readback	0x8b8

#define		bLSSIReadAddress          		0x7f800000
#define		bLSSIReadEdge             		0x80000000
#define		bLSSIReadBackData         		0xfffff

#define		bMaskDWord		0xffffffff

VOID
Func_Chip_Bb_Settxpower_Index_Process(IN PNIC Nic,
						  IN u32 PowerIndex, IN u8 RFPath, IN u8 Rate);

u8
Func_Chip_Bb_Gettxpowerindex_Process(IN PNIC Nic,
						  IN u8 RFPath,
						  IN u8 Rate,
						  IN CHANNEL_WIDTH BandWidth, IN u8 Channel);

VOID Func_Chip_Bb_Settxpower_Level_Process(IN PNIC Nic, IN u8 channel);

VOID Func_Chip_Bb_Setrfpath_Switch_Process(IN PNIC Nic, IN BOOLEAN bMain);


#define FW_9086X_SIZE			0x8000
#define FW_9086X_START_ADDRESS	0x1000
#define FW_9086X_END_ADDRESS		0x1FFF

#define IS_FW_HEADER_EXIST_9086X(_pFwHdr)	((le16_to_cpu(_pFwHdr->Signature)&0xFFF0) == 0x88F0)

#define MAILBOX_REG_START  0x00000300
#define MAILBOX_REG_END	   0x000003DF

#define MAILBOX_REG_INT    0x000003F0
#define MAILBOX_INT_FINISH 0x000003E8

#define MAX_MAILBOX_LEN       	56
#define MAILBOX_WORD_LEN       	4

#define MAILBOX_REG_FUNC       	(MAILBOX_REG_START)
#define MAILBOX_ARG_START  		(MAILBOX_REG_FUNC + MAILBOX_WORD_LEN)

#define MAILBOX_MAX_RDLEN       (56 - 1)
#define MAILBOX_MAX_TXLEN       (56 - 3)

#define MAILBOX_ADD_ASSERT(addr) ((addr) >= MAILBOX_REG_START && (addr) <= MAILBOX_REG_END) ? _TRUE : _FALSE)

#define MAILBOX_TGIRGER_INT(pwadptdata) 	    (HwPlatformIOWrite1Byte(pwadptdata, MAILBOX_REG_INT, _TRUE))

#define MAILBOX_SET_INTFINISH(pwadptdata)     (HwPlatformIOWrite4Byte(pwadptdata, MAILBOX_INT_FINISH, 0x12345678))
#define MAILBOX_GET_INTFINISH(pwadptdata)     ((HwPlatformIORead4Byte(pwadptdata, MAILBOX_INT_FINISH) == 0x55 )? _TRUE : _FALSE)

#define UMSG_NORMAL_INT_MAXTIME    60000 // 1000

#define WF_XMIT_CTL   		    0x22C
#define WF_XMIT_AGG_MAXNUMS 	0x01
#define WF_XMIT_AMPDU_DENSITY   0x02
#define WF_XMIT_OFFSET          0x04
#define WF_XMIT_PKT_OFFSET      0x08
#define WF_XMIT_ALL             0xFF

void Func_Wf_Config_Xmit(PNIC Nic, int event, u32 val);

#define RX_DMA_SIZE_9086X			0x4000
#ifdef CONFIG_FW_wMBOX0_DEBUG
#define RX_DMA_RESERVED_SIZE_9086X	0x100
#else
#define RX_DMA_RESERVED_SIZE_9086X	0x80
#endif

#ifdef CONFIG_WOWLAN
#define RESV_FMWF	WKFMCAM_SIZE*MAX_WKFM_NUM
#else
#define RESV_FMWF	0
#endif

#define RX_DMA_BOUNDARY_9086X		(RX_DMA_SIZE_9086X - RX_DMA_RESERVED_SIZE_9086X - 1)

#define BCNQ_PAGE_NUM_9086X		0x08
#ifdef CONFIG_CONCURRENT_MODE
#define BCNQ1_PAGE_NUM_9086X		0x08
#else
#define BCNQ1_PAGE_NUM_9086X		0x00
#endif


#ifdef CONFIG_WOWLAN
#define WOWLAN_PAGE_NUM_9086X	0x07
#else
#define WOWLAN_PAGE_NUM_9086X	0x00
#endif


#ifdef CONFIG_AP_WOWLAN
#define AP_WOWLAN_PAGE_NUM_9086X	0x02
#endif

#define TX_TOTAL_PAGE_NUMBER_9086X	(0xFF - BCNQ_PAGE_NUM_9086X - BCNQ1_PAGE_NUM_9086X - WOWLAN_PAGE_NUM_9086X)
#define TX_PAGE_BOUNDARY_9086X		(TX_TOTAL_PAGE_NUMBER_9086X + 1)

#define WMM_NORMAL_TX_TOTAL_PAGE_NUMBER_9086X	TX_TOTAL_PAGE_NUMBER_9086X
#define WMM_NORMAL_TX_PAGE_BOUNDARY_9086X		(WMM_NORMAL_TX_TOTAL_PAGE_NUMBER_9086X + 1)


#include "comm.h"

#define EFUSE_OOB_PROTECT_BYTES (18 + 1)

#define HAL_EFUSE_MEMORY

#define HWSET_MAX_SIZE_9086X			512
#define EFUSE_REAL_CONTENT_LEN_9086X	256
#define EFUSE_MAP_LEN_9086X				512
#define EFUSE_MAX_SECTION_9086X			(EFUSE_MAP_LEN_9086X / 8)

#define EFUSE_IC_ID_OFFSET			506
#define AVAILABLE_EFUSE_ADDR(addr) 	(addr < EFUSE_REAL_CONTENT_LEN_9086X)

#define EFUSE_ACCESS_ON			0x69
#define EFUSE_ACCESS_OFF			0x00

#define EFUSE_BT_REAL_BANK_CONTENT_LEN	512
#define EFUSE_BT_REAL_CONTENT_LEN		1536
#define EFUSE_BT_MAP_LEN				1024
#define EFUSE_BT_MAX_SECTION			128

#define EFUSE_PROTECT_BYTES_BANK		16

typedef struct _wMBOX0_EVT_HDR {
	u8 CmdID;
	u8 CmdLen;
	u8 CmdSeq;
} __attribute__ ((__packed__)) wMBOX0_EVT_HDR, *PwMBOX0_EVT_HDR;


u8 Func_To_Get_Eeprom_Size_Process(PNIC Nic);

void Func_Init_Hal_Spec_Process(PNIC Nic);

void Func_Mcu_Set_Hwreg_Process(PNIC Nic, u8 variable, u8 * val);
void Func_Mcu_Get_Hwreg_Process(PNIC Nic, u8 variable, u8 * val);

u8 Func_To_Get_Hal_Def_Var_Process(PNIC Nic, HAL_DEF_VARIABLE variable, void *pval);


#ifdef CONFIG_FW_wMBOX0_DEBUG
void Func_Debug_Fwc2H_Process(PNIC Nic, u8 * pdata, u8 len);
#endif

enum _UMSG_OPS_CODE {
	FUNC_REPLY = 0x0,
	UMSG_OPS_READ_VERSION = 0x01,
	UMSG_OPS_HAL_GETESIZE = 0x06,
	UMSG_OPS_HAL_EFSUSESEL = 0x07,
	UMSG_OPS_HAL_EFUSEMAP = 0x08,
	UMSG_OPS_HAL_EFUSEMAP_LEN = 0x09,
	UMSG_OPS_HAL_EFUSETYPE = 0x0A,
	UMSG_OPS_HAL_EFUSE_BAUTOLOAD_FLAG = 0x0B,
	UMSG_OPS_HAL_PWRDWMODE = 0x0C,
	UMSG_OPS_HAL_RFCHNLVAL = 0x0D,
	UMSG_OPS_HAL_INIT_STEP0 = 0x11,
	UMSG_OPS_HAL_INIT_STEP1 = 0x12,
	UMSG_OPS_HAL_INIT_ANT_SEL = 0x13,
	UMSG_OPS_HAL_INIT_DM = 0x14,
	UMSG_OPS_HAL_RF_CHNLBW = 0x15,
	UMSG_OPS_HAL_SET_BW_MODE = 0x16,
	UMSG_OPS_SET_TX_PW_LEVEL = 0x17,
	UMSG_OPS_HAL_MAC_TXEN_RXEN = 0x18,
	UMSG_OPS_HAL_FWHW_TXQ_CTRL = 0x19,
	UMSG_OPS_HAL_INIT_STEP2 = 0x1A,
	UMSG_PHY_LCC_RATE = 0x1B,
	UMSG_OPS_HAL_SET_HWREG = 0x1C,
	UMSG_OPS_HAL_GET_HWREG = 0x1D,

	UMSG_OPS_HAL_CONFIG_MSG = 0x1E,
	UMSG_OPS_HAL_INIT_MSG = 0x1F,
	UMSG_OPS_HAL_MSG_WDG = 0x20,
	UMSG_OPS_HAL_WRITEVAR_MSG = 0x21,
	UMSG_OPS_HAL_READVAR_MSG = 0x22,
	UMSG_OPS_MSG_UPDATEIG = 0x23,
	UMSG_OPS_MSG_PAUSEIG = 0x24,
	UMSG_OPS_PHYDM_wMBOX0_CONTENT_PARS = 0x25,
	UMSG_0PS_MSG_GET_RATE_BITMAP = 0x26,
	UMSG_OPS_MSG_RF_SAVE = 0x27,
	UMSG_OPS_MSG_RHY_STATUS = 0x28,
	UMSG_OPS_MSG_EMB_MAC_IMG = 0x29,
	UMSG_OPS_MSG_EMB_RF_IMG = 0x2A,
	UMSG_OPS_INIT_BB_PHY_REG = 0x2B,
	UMSG_OPS_MSG_EMB_TXPWRTRACK_IMG = 0x2C,
	UMSG_OPS_HAL_GET_MSG_STA_INFO = 0x2D,
	UMSG_OPS_HAL_SYNC_MSG_STA_INFO = 0x2E,
	UMSG_OPS_MSG_TXPWR_TRACKING_CHECK = 0x2F,

	UMSG_OPS_HAL_MSG_ADAPTIVITY_PARM_SET = 0x30,
	UMSG_OPS_HAL_MSG_SET_PWR_TRACK_CTR = 0x31,
	UMSG_OPS_HAL_MSG_GET_PWR_TRACK_CTR = 0x32,
	UMSG_OPS_HAL_MSG_GET_PHY_REG_PG_VER = 0x33,
	UMSG_OPS_HAL_MSG_ABILITY_OPS = 0x34,
	UMSG_OPS_HAL_MSG_GET_TX_PWR_TRACKING_OFFSET = 0x35,
	UMSG_OPS_HAL_MSG_GET_PHY_REG_PG_VAL_TYPE = 0x36,
	UMSG_OPS_HAL_SET_REG_CR_9086X = 0x37,
	UMSG_OPS_HAL_MSG_GET_DISABLE_PWR_TRAINING = 0x38,
	UMSG_OPS_HAL_MSG_SET_REGA24 = 0x39,
	UMSG_OPS_HAL_MSG_SET_PHY_REG_PG_VERISON = 0x3A,
	UMSG_OPS_HAL_MSG_SET_PHY_REG_PG_VAL_TYPE = 0x3B,
	UMSG_OPS_HAL_MSG_INIT_DEFAULT_VALUE = 0x3C,
	UMSG_OPS_HAL_MSG_SET_APK_THERMAL_METER_IGNORE = 0x3D,
	UMSG_OPS_HAL_SET_REG_CCK_CHECK_9086X = 0x3E,
	UMSG_OPS_HAL_SET_PLL_REF_CLK_SEL = 0x3F,
	UMSG_OPS_HAL_SET_REG_AMPDU_MAX_LENGTH_9086X = 0x40,
	UMSG_OPS_HAL_SET_REG_DWBCN1_CTRL_9086X = 0x41,
	UMSG_OPS_HAL_INIT_MSG_VAR = 0x44,

	UMSG_OPS_HAL_CALI_LLC = 0x45,
	UMSG_OPS_HAL_PHY_IQ_CALIBRATE = 0x46,
	UMSG_OPS_HAL_CONFIG_CONCURRENT_MODE = 0x47,
	UMSG_OPS_HAL_CHNLBW_MODE = 0x48,
	UMSG_OPS_HAL_DW_FW = 0x49,
	UMSG_OPS_HAL_INIT_MAC_PHY_RF = 0x4A,
	UMSG_OPS_HAL_FW_INIT = 0x4B,
#ifdef RENEW_EEPROM_THERMAL 
	UMSG_OPS_HAL_UPDATE_THERMAL = 0x4C,
#endif
	UMSG_OPS_HAL_UPDATE_TX_FIFO = 0x4D,
	UMSG_OPS_HAL_RESUME_TXBCN = 0x4E,
	UMSG_OPS_HAL_STOP_TXBCN = 0x4F,
	UMSG_OPS_HAL_BCN_FUNC_ENABLE = 0x50,
	UMSG_OPS_HAL_SET_BCN_REG = 0x51,
	UMSG_OPS_HAL_NOTCH_FILTER = 0x52,
	UMSG_OPS_HW_VAR_SET_MONITOR = 0x53,
	UMSG_OPS_HAL_SET_MAC = 0x54,
	UMSG_OPS_HAL_SET_BSSID = 0x55,
	UMSG_OPS_HAL_SET_BCN = 0x56,
	UMSG_OPS_HW_SET_BASIC_RATE = 0x57,
	UMSG_OPS_HW_SET_OP_MODE = 0x58,
	UMSG_OPS_HW_SET_CORRECT_TSF = 0x59,
	UMSG_OPS_HW_SET_MLME_DISCONNECT = 0x5a,
	UMSG_OPS_HW_SET_MLME_SITE = 0x5b,
	UMSG_OPS_HW_SET_MLME_JOIN = 0x5c,
	UMSG_OPS_HW_FIFO_CLEARN_UP = 0x5d,
	UMSG_OPS_HW_UPDATE_TSF = 0x5e,
	UMSG_OPS_HW_SET_DK_CFG = 0x5f,
	UMSG_OPS_HW_FWLPS_RF_ON = 0x60,

	UMSG_OPS_HAL_STATES = 0x66,
	UMSG_OPS_HAL_SEC_READ_CAM = 0x67,
	UMSG_OPS_HAL_SEC_WRITE_CAM = 0x68,
	UMSG_OPS_HAL_wMBOX1_CMD = 0x69,
	UMSG_Ops_HAL_ISMONITOR_RST = 0x6a,
	UMSG_OPS_HAL_CHECK_RXFIFO_FULL = 0x6b,
	UMSG_OPS_HAL_TXDMA_STATUS = 0x6c,
	UMSG_OPS_HAL_MSG_IO = 0x6d,
	UMSG_OPS_HAL_CKIPSSTUTAS = 0x6e,
	UMSG_OPS_HAL_TEST_LDO = 0x6f,

	UMSG_OPS_HAL_LPS_SET = 0x7D,
	UMSG_OPS_HAL_LPS_GET = 0x7E,
	UMSG_OPS_HAL_LPS_CONFIG = 0x7F,
	UMSG_OPS_HAL_READ_WKFM_CAM = 0x80,
	UMSG_OPS_HAL_WRITE_WKFM_CAM = 0x81,
	UMSG_OPS_HAL_SWITCH_GPIO_WL = 0x82,
	UMSG_OPS_HAL_SET_OUTPUT_GPIO = 0x83,
	UMSG_OPS_HAL_ENABLE_RXDMA = 0x84,
	UMSG_OPS_HAL_DISABLE_TX_REPORT = 0x85,
	UMSG_OPS_HAL_ENABLE_TX_REPORT = 0x86,
	UMSG_OPS_HAL_RELEASE_RX_DMA = 0x87,
	UMSG_OPS_HAL_CHECK_WOW_CTL = 0x88,
	UMSG_OPS_HAL_UPDATE_TX_IV = 0x89,
	UMSG_OPS_HAL_GATE_BB = 0x8A,
	UMSG_OPS_HAL_SET_RXFF_BOUNDARY = 0x8B,
	UMSG_OPS_HAL_REG_SWITCH = 0x8c,
	UMSG_OPS_HAL_SET_USB_AGG_NORMAL = 0X8D,
	UMSG_OPS_HAL_SET_USB_AGG_CUSTOMER = 0X8E,
	UMSG_OPS_HAL_DM_DYNAMIC_TX_AGG = 0X8F,
	UMSG_OPS_HAL_SET_RPWM = 0X90,
	UMSG_OPS_REPROBE_USB3 = 0x91,
	UMSG_OPS_EFUSE_1BYTE = 0x92,
	UMSG_OPS_HAL_DEINIT = 0x93,
	UMSG_OPS_MAC_HIDDEN = 0x94,
	UMSG_OPS_HAL_PAUSE_RXDMA = 0x95,
	MO_OPS_HAL_UMSG_CLOSE,

	UMSG_OPS_MP_TEST,
	UMSG_OPS_MP_CMD_HDL,
	UMSG_OPS_MP_SET_ANT_TX,
	UMSG_OPS_MP_SET_ANT_RX,
	UMSG_OPS_MP_PROSET_TXPWR_1,
	UMSG_OPS_MP_PROSET_TXPWR_2,
	UMSG_OPS_MP_INIT,
	UMSG_OPS_MP_ARX,
	UMSG_OPS_MP_GET_PSDATA,
	UMSG_OPS_MP_SET_PRX,
	UMSG_OPS_MP_JOIN,
	UMSG_OPS_MP_DIS_DM,
	UMSG_OPS_MP_SET_TXPOWERINDEX,
	UMSG_OPS_MP_SET_CCKCTX,
	UMSG_OPS_MP_SET_OFDMCTX,
	UMSG_OPS_MP_SET_SINGLECARRTX,
	UMSG_OPS_MP_SET_SINGLETONETX,
	UMSG_OPS_MP_SET_CARRSUPPTX,
	UMSG_OPS_MP_SET_MACTXEDCA,
	UMSG_OPS_MSG_WRITE_DIG,
	UMSG_OPS_MP_EFUSE_READ,
	UMSG_OPS_MP_EFUSE_WRITE,
	UMSG_OPS_MP_EFUSE_ACCESS,
	UMSG_OPS_MP_EFUSE_GSize,
	UMSG_OPS_MP_EFUSE_GET,
	UMSG_OPS_MP_EFUSE_SET,
	UMSG_OPS_MP_MACRXCOUNT,
	UMSG_OPS_MP_PHYRXCOUNT,
	UMSG_OPS_MP_RESETCOUNT,
	UMSG_OPS_MP_PHYTXOK,
	UMSG_OPS_MP_CTXRATE,
	FUNC_CMD_MAX
};

#define MP_ADDR_START  FUNC_CMD_MAX + 0x100

enum _UMSG_MP_OPS_CODE {
	MP_FUNC_REPLY = MP_ADDR_START,
	MP_FUNC_START,
	MP_FUNC_SETRFPATH,
	MP_FUNC_CHANNEL,
	MP_FUNC_BANDWIDTH,
	MP_FUNC_ANT_TX,
	MP_FUNC_TX_POWER,
	MP_FUNC_SETRATE,
	MP_FUNC_CTX,
	MP_FUNC_STOP,
	MP_FUNC_JOIN,

	MP_FUNC_CMD_MAX
};

typedef enum _wMBOX0_EVT {
	wMBOX0_DBG = 0x00,
	wMBOX0_LB = 0x01,
	wMBOX0_TXBF = 0x02,
	wMBOX0_CCX_TX_RPT = 0x03,
	wMBOX0_BT_INFO = 0x09,
	wMBOX0_BT_MP_INFO = 0x0B,
	wMBOX0_RA_RPT = 0x0C,
	wMBOX0_RA_PARA_RPT = 0x0E,
	wMBOX0_FW_CHNL_SWITCH_COMPLETE = 0x10,
	wMBOX0_IQK_FINISH = 0x11,
	wMBOX0_MAILBOX_STATUS = 0x15,
	wMBOX0_P2P_RPORT = 0x16,
	wMBOX0_MAC_HIDDEN_RPT = 0x19,
	wMBOX0_BCN_EARLY_RPT = 0x1E,
	wMBOX0_BT_SCOREBOARD_STATUS = 0x20,
	wMBOX0_EXTEND = 0xff,
} wMBOX0_EVT;

u32 Func_Mcu_Go(PNIC Nic);
int Func_Mcu_Message_Up(PNIC Nic);
int Func_Mcu_Universal_Func_Interface(PNIC Nic, u32 func_code, u32 * send,
								u32 * recv, int send_len, int recv_len);
int Func_Mcu_Special_Func_Interface(PNIC Nic, u32 func_code, u32 * rsp,
							  int len, int offs);
int Func_Mcu_Send_Buffer_Interface(PNIC Nic, u32 func_code, u32 * send,
							 int send_len, int offs, int total_len);

int UMSG_Hal_InitRF(PNIC Nic);
u32 umsg_hal_set_chnl_bw(PNIC Nic, u8 channel, CHANNEL_WIDTH Bandwidth,
					   u8 Offset40, u8 Offset80);

void Func_Hw_Var_Set_Fwlps_Rfon(PNIC Nic, u8 variable, u8 * val);

void Func_Init_Hal_Dm(IN PNIC Nic);

u8 hw_var_set_fwstate_cmd(PNIC Nic);
void umsg_hw_var_set_fwlps_rfon(PNIC Nic, u8 variable, u8 * val);

void hw_var_set_fwstate(PNIC Nic);
void Func_Mcu_Hw_Var_Set_Opmode(PNIC Nic, u8 variable, u8 * val);
void Func_Mcu_Hw_Var_Set_Bssid(PNIC Nic, u8 variable, u8 * val);
void Func_Mcu_Hw_Var_Set_Macaddr(PNIC Nic, u8 variable, u8 * val);
void umsg_hw_var_set_basic_rate(PNIC Nic, u8 variable, u8 * val);
void Func_Mcu_Hw_Var_Set_Bcn_Func(PNIC Nic, u8 variable, u8 * val);
void Func_Mcu_Hw_Var_Set_Correct_Tsf(PNIC Nic, u8 variable, u8 * val);
void Func_Mcu_Hw_Var_Set_Mlme_Sitesurvey(PNIC Nic, u8 variable, u8 * val);
void Func_Mcu_Hw_Var_Set_Mlme_Join(PNIC Nic, u8 variable, u8 * val);
void Func_Mcu_Hw_Var_Set_Mlme_Disconnect(PNIC Nic, u8 variable, u8 * val);
void Func_Mcu_Hw_Var_Set_wMBOX1_Fw_Pwrmode(PNIC Nic, u8 variable, u8 * val);
void umsg_hw_var_set_fifo_clearn_up(PNIC Nic, u8 variable, u8 * val);

void Func_Mcu_Hw_Var_Set_Hw_Update_Tsf(PNIC Nic);
u8 hw_var_set_dk_cmd(PNIC Nic, u8 variable, u8 val);

void hw_var_set_dk_cfg(PNIC Nic, u8 variable, u8 val);
u32 Func_Mcu_Hal_Get_Hwreg(PNIC Nic, u8 variable, u8 * val, int len);
u32 Func_Mcu_Hal_Set_Hwreg(PNIC Nic, u32 variable, u8 * val, int len);

s32 Func_Mcu_Fill_wMBOX1_Fw(PNIC Nic, u8 ElementID, u32 CmdLen,
				   u8 * pCmdBuffer);

VOID
Func_Chip_Bb_Handle_Sw_Chnl_And_Set_Bw_Process(IN PNIC Nic,
							  IN BOOLEAN bSwitchChannel,
							  IN BOOLEAN bSetBandWidth,
							  IN u8 ChannelNum,
							  IN CHANNEL_WIDTH ChnlWidth,
							  IN EXTCHNL_OFFSET ExtChnlOffsetOf40MHz,
							  IN u8 CenterFrequencyIndex1);

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
void umsg_hw_var_set_wowlan(PNIC Nic, u8 variable, u8 * val);
#endif

void Func_Set_Concurrent_Func(PNIC Nic);


#endif
