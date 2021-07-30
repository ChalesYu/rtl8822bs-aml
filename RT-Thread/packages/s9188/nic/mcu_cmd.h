/*
 * mcu_cmd.h
 *
 * used for cmd Interactive command
 *
 * Author: songqiang
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#ifndef __MCU_CMD_H__
#define __MCU_CMD_H__

#ifdef MCU_CMD_MAILBOX
#define mcu_cmd_communicate        wf_io_write_cmd_by_mailbox
#define mcu_cmd_communicate_try    wf_io_write_cmd_by_mailbox_try
#endif

#ifdef MCU_CMD_TXD
#define mcu_cmd_communicate         wf_io_write_cmd_by_txd
#endif


#define MAILBOX_REG_START  0x00000300
#define MAILBOX_REG_END	   0x000003DF

#define MAX_MAILBOX_LEN       	56
#define MAILBOX_WORD_LEN       	4

#define MAILBOX_REG_FUNC       	(MAILBOX_REG_START)
#define MAILBOX_ARG_START  		(MAILBOX_REG_FUNC + MAILBOX_WORD_LEN)

#define MAILBOX_MAX_RDLEN       (56 - 1)
#define MAILBOX_MAX_TXLEN       (56 - 3)


#define WF_MAILBOX_INT_FINISH 0x03E8
#define WF_MAILBOX_REG_INT    0x03F0


#define UMSG_NORMAL_INT_MAXTIME    60000 // 1000

#define REG_wMBOX0EVT_MSG_NORMAL		0x01A0

#define SHORT_SLOT_TIME					9
#define BCN_INTERVAL_TIME				100
#define NON_SHORT_SLOT_TIME				20

#define WF_FREQ_ADDR					0X24   /* frequency */

#define WF_RATE_1M				BIT(0)
#define WF_RATE_2M				BIT(1)
#define WF_RATE_5_5M			BIT(2)
#define WF_RATE_11M				BIT(3)
#define WF_RATE_6M				BIT(4)
#define WF_RATE_9M				BIT(5)
#define WF_RATE_12M				BIT(6)
#define WF_RATE_18M				BIT(7)
#define WF_RATE_24M				BIT(8)
#define WF_RATE_36M				BIT(9)
#define WF_RATE_48M				BIT(10)
#define WF_RATE_54M				BIT(11)

#define WF_SECURITY_CAM_SIZE (24)
#define WF_SECURITY_KEY_SIZE (16)


#define wMBOX1_MACID_CFG_LEN            7
#define wMBOX1_MEDIA_STATUS_RPT_LEN     3


enum _REG_PREAMBLE_MODE {
	PREAMBLE_LONG = 1,
	PREAMBLE_AUTO = 2,
	PREAMBLE_SHORT = 3,
};

/*copy from origin 9082*/
enum WLAN_WL_H2M_ENUM_TYPE {
	WLAN_WL_H2M_PWR_TPPES = 0x20,
	WLAN_WL_H2M_SYS_CALIBRATION = 0x6D,
};

typedef enum _UMSG_OPS_CODE {
	FUNC_REPLY                                      = 0x0,
    UMSG_OPS_READ_VERSION                           = 0x01,
    UMSG_OPS_HAL_EFUSEMAP                           = 0x08,
    UMSG_OPS_HAL_EFUSEMAP_LEN                       = 0x09,
    UMSG_OPS_HAL_EFUSETYPE                          = 0x0A,

    UMSG_OPS_HAL_INIT_STEP0                         = 0x11,    // usb
    UMSG_OPS_HAL_GET_BCN_PAR                        = 0x111,   // sdio
    UMSG_OPS_HAL_INIT_PKT_LEN                       = 0x12,    // sdio    
    UMSG_OPS_HAL_INIT_ANT_SEL                       = 0x13,    // sdio
    UMSG_OPS_HAL_CCA_CONFIG                         = 0x1A,    // sdio
    UMSG_OPS_HAL_SET_HWREG                          = 0x1C,    // sdio
    UMSG_OPS_HAL_GET_HWREG                          = 0x1D,    // sdio

	UMSG_OPS_HAL_HW_INIT							= 0X10,
	UMSG_OPS_HAL_CONFIG_MSG                         = 0x1E,
    UMSG_OPS_HAL_INIT_MSG                           = 0x1F,
    UMSG_OPS_HAL_MSG_WDG                            = 0x20,
    UMSG_OPS_HAL_WRITEVAR_MSG                       = 0x21,
    UMSG_OPS_HAL_READVAR_MSG                        = 0x22,   // check mp mode use
    UMSG_OPS_MSG_PAUSEIG                            = 0x24,
    UMSG_0PS_MSG_GET_RATE_BITMAP                    = 0x26,
    UMSG_OPS_MSG_RHY_STATUS                         = 0x28,
    UMSG_0PS_MSG_SET_RATE_BITMAP                    = 0x29,
    UMSG_OPS_HAL_GET_MSG_STA_INFO                   = 0x2D,
	UMSG_OPS_HAL_SYNC_MSG_STA_INFO                  = 0x2E,
    UMSG_OPS_HAL_ARS_INIT							= 0x2F,
    UMSG_OPS_HAL_MSG_INIT_DEFAULT_VALUE             = 0x3C,
    UMSG_OPS_HAL_INIT_MSG_VAR                       = 0x44,
	UMSG_OPS_HAL_CALI_LLC                           = 0x45,
	UMSG_OPS_HAL_PHY_IQ_CALIBRATE                   = 0x46,
    UMSG_OPS_HAL_CONFIG_CONCURRENT_MODE             = 0x47,
    UMSG_OPS_HAL_CHNLBW_MODE                        = 0x48,
    UMSG_OPS_HAL_DW_FW                              = 0x49,
    UMSG_OPS_HAL_FW_INIT                            = 0x4B,
    UMSG_OPS_HAL_UPDATE_THERMAL                     = 0x4C,
    UMSG_OPS_HAL_UPDATE_TX_FIFO                     = 0x4D,
    UMSG_OPS_HAL_SET_BCN_REG                        = 0x51,
    UMSG_OPS_HAL_SET_MAC                            = 0x54,
    UMSG_OPS_HAL_SET_BSSID                          = 0x55,
    
    UMSG_OPS_HAL_SET_BCN                            = 0x56,
    UMSG_OPS_HW_SET_BASIC_RATE                      = 0x57,
    UMSG_OPS_HW_SET_OP_MODE                         = 0x58,
    UMSG_OPS_HW_SET_CORRECT_TSF                     = 0x59,
    UMSG_OPS_HW_SET_MLME_DISCONNECT                 = 0x5a,
    UMSG_OPS_HW_SET_MLME_SITE                       = 0x5b,
    UMSG_OPS_HW_SET_MLME_JOIN                       = 0x5c,
    UMSG_OPS_HW_SET_DK_CFG                          = 0x5f,

    UMSG_OPS_HAL_SEC_WRITE_CAM                      = 0x68,
    UMSG_OPS_HAL_H2C_CMD                            = 0x69,
    UMSG_OPS_HAL_CHECK_RXFIFO_FULL                  = 0x6b,

    UMSG_OPS_HAL_LPS_OPT                            = 0x7B,
    UMSG_OPS_HAL_LPS_CONFIG                         = 0x7C,
    UMSG_OPS_HAL_LPS_SET                            = 0x7D,
    UMSG_OPS_HAL_LPS_GET                            = 0x7E,

    UMSG_OPS_HAL_SET_USB_AGG_NORMAL                 = 0X8D,
    UMSG_OPS_HAL_SET_USB_AGG_CUSTOMER               = 0X8E,
    
    UMSG_OPS_EFUSE_1BYTE                            = 0x92,
    UMSG_OPS_HAL_DEINIT                             = 0x93,
    UMSG_OPS_MP_SET_ANT_TX                          = 0X99,
    UMSG_OPS_MP_SET_ANT_RX                          = 0X9A,
    UMSG_OPS_MP_PROSET_TXPWR_1                      = 0X9B,
    UMSG_OPS_MP_INIT                                = 0X9D,
    UMSG_OPS_MP_SET_PRX                             = 0XA0,
    UMSG_OPS_MP_DIS_DM                              = 0XA2,
    UMSG_OPS_MP_SET_CCKCTX                          = 0XA4,
    UMSG_OPS_MP_SET_OFDMCTX                         = 0XA5,
    UMSG_OPS_MP_SET_SINGLECARRTX                    = 0XA6,
    UMSG_OPS_MP_SET_SINGLETONETX                    = 0XA7,
    UMSG_OPS_MP_SET_CARRSUPPTX                      = 0XA8,
    UMSG_OPS_MSG_WRITE_DIG                          = 0XAA,
    UMSG_OPS_MP_EFUSE_READ                          = 0XAB,//logic
    UMSG_OPS_MP_EFUSE_WRITE                         = 0XAC,
    UMSG_OPS_MP_EFUSE_ACCESS                        = 0XAD,
    UMSG_OPS_MP_EFUSE_GSize                         = 0XAE,
    UMSG_OPS_MP_EFUSE_GET                           = 0XAF,//phy
    UMSG_OPS_MP_EFUSE_SET                           = 0XB0,
    UMSG_OPS_MP_MACRXCOUNT                          = 0XB1,
    UMSG_OPS_MP_PHYRXCOUNT                          = 0XB2,
    UMSG_OPS_MP_RESETCOUNT                          = 0XB3,
    UMSG_OPS_MP_PHYTXOK                             = 0XB4,
    UMSG_OPS_MP_CTXRATE                             = 0XB5,

    UMSG_OPS_MP_SET_POWER                           = 0x10,
    UMSG_OPS_MP_RESET_MAC_RX_COUNTERS               = 0x19B,
    UMSG_OPS_MP_RESET_PHY_RX_COUNTERS               = 0x19C,
    UMSG_OPS_MP_SET_RF_PATH_SWITCH                  = 0x193,
    UMSG_OPS_MP_BB_RF_GAIN_OFFSET                   = 0x198,
    UMSG_OPS_HAL_FREQ_GET                           = 0x101,
    UMSG_OPS_HAL_FREQ_SET                           = 0x102,
    UMSG_OPS_HAL_TEMP_GET                           = 0x103,
    UMSG_OPS_HAL_TEMP_SET                           = 0x104,
    UMSG_OPS_HAL_SET_XTAL                           = 0x105,
    UMSG_OPS_HAL_SET_XTAL_DEFAULT                   = 0x106,
    UMSG_OPS_MP_BB_REG_GET                          = 0x1A0,
    UMSG_OPS_MP_BB_REG_SET                          = 0x1A1,
    UMSG_OPS_MP_RF_REG_GET                          = 0x1A2,
    UMSG_OPS_MP_RF_REG_SET                          = 0x1A3,
    UMSG_OPS_MP_USER_INFO                           = 0XF0,
    UMSG_OPS_CMD_TEST                               = 0XF1,
    UMSG_OPS_RESET_CHIP                             = 0XF2,
    UMSG_OPS_HAL_DBGLOG_CONFIG                      = 0xF6,
} MCU_UMSG_CMD;

typedef enum WLAN__HAL_VALUEIABLES
{
    WLAN_HAL_VALUE_MEDIA_STATUS,
    WLAN_HAL_VALUE_MEDIA_STATUS1,
    WLAN_HAL_VALUE_SET_OPMODE,
    WLAN_HAL_VALUE_MAC_ADDR,
    WLAN_HAL_VALUE_BSSID,
    WLAN_HAL_VALUE_INIT_RTS_RATE,
    WLAN_HAL_VALUE_BASIC_RATE,
    WLAN_HAL_VALUE_TXPAUSE,
    WLAN_HAL_VALUE_BCN_FUNC,
    WLAN_HAL_VALUE_CORRECT_TSF,
    WLAN_HAL_VALUE_CHECK_BSSID,
    WLAN_HAL_VALUE_MLME_DISCONNECT,
    WLAN_HAL_VALUE_MLME_SITESURVEY,
    WLAN_HAL_VALUE_MLME_JOIN,
    WLAN_HAL_VALUE_ON_RCR_AM,
    WLAN_HAL_VALUE_OFF_RCR_AM,
    WLAN_HAL_VALUE_BEACON_INTERVAL,
    WLAN_HAL_VALUE_SLOT_TIME,
    WLAN_HAL_VALUE_RESP_SIFS,
    WLAN_HAL_VALUE_ACK_PREAMBLE,
    WLAN_HAL_VALUE_SEC_CFG,
    WLAN_HAL_VALUE_SEC_DK_CFG,
    WLAN_HAL_VALUE_BCN_VALID,
    WLAN_HAL_VALUE_RF_TYPE,
    WLAN_HAL_VALUE_CAM_EMPTY_ENTRY,
    WLAN_HAL_VALUE_CAM_INVALID_ALL,
    WLAN_HAL_VALUE_AC_PARAM_VO,
    WLAN_HAL_VALUE_AC_PARAM_VI,
    WLAN_HAL_VALUE_AC_PARAM_BE,
    WLAN_HAL_VALUE_AC_PARAM_BK,
    WLAN_HAL_VALUE_ACM_CTRL,
    WLAN_HAL_VALUE_AMPDU_MIN_SPACE,
    WLAN_HAL_VALUE_AMPDU_FACTIONOR,
    WLAN_HAL_VALUE_RXDMA_AGG_PG_TH,
    WLAN_HAL_VALUE_SET_RPWM,
    WLAN_HAL_VALUE_wFPRS,
    WLAN_HAL_VALUE_wMBOX1_FW_PWRMODE,
    WLAN_HAL_VALUE_wMBOX1_POWER_SAVE_TUNE_PARAM,
    WLAN_HAL_VALUE_wMBOX1_FW_JOINBSSRPT,
    WLAN_HAL_VALUE_FWLPS_RF_ON,
    WLAN_HAL_VALUE_wMBOX1_FW_P2P_POWER_SAVE_OFFLOAD,
    WLAN_HAL_VALUE_TDLS_WRCR,
    WLAN_HAL_VALUE_TDLS_RS_RCR,
    WLAN_HAL_VALUE_TRIGGER_GPIO_0,
    WLAN_HAL_VALUE_BT_SET_COEXIST,
    WLAN_HAL_VALUE_BT_ISSUE_DELBA,
    WLAN_HAL_VALUE_CURRENT_ANTENNA,
    WLAN_HAL_VALUE_ANTENNA_DIVERSITY_SELECT,
    WLAN_HAL_VALUE_SWITCH_EPHY_WOWLAN,
    WLAN_HAL_VALUE_EEPORM_USAGE,
    WLAN_HAL_VALUE_EEPORM_BYTES,
    WLAN_HAL_VALUE_EEPORM_BT_USAGE,
    WLAN_HAL_VALUE_EEPORM_BT_BYTES,
    WLAN_HAL_VALUE_FIFO_CLEARN_UP,
    WLAN_HAL_VALUE_RESTORE_HW_SEQ,
    WLAN_HAL_VALUE_CHECK_TXBUF,
    WLAN_HAL_VALUE_PCIE_STOP_TX_DMA,
    WLAN_HAL_VALUE_APFM_ON_MAC,
    WLAN_HAL_VALUE_HCI_SUS_STATE,
    WLAN_HAL_VALUE_SYS_CLKR,
    WLAN_HAL_VALUE_NAV_UPPER,
    WLAN_HAL_VALUE_wMBOX0_HANDLE,
    WLAN_HAL_VALUE_RPT_TIMER_SETTING,
    WLAN_HAL_VALUE_TX_RPT_MAX_MACID,
    WLAN_HAL_VALUE_CHK_HI_QUEUE_EMPTY,
    WLAN_HAL_VALUE_DL_BCN_SEL,
    WLAN_HAL_VALUE_AMPDU_MAX_TIME,
    WLAN_HAL_VALUE_WIRELESS_MODE,
    WLAN_HAL_VALUE_USB_MODE,
    WLAN_HAL_VALUE_PORT_SWITCH,
    WLAN_HAL_VALUE_DO_IQK,
    WLAN_HAL_VALUE_DM_IN_LPS,
    WLAN_HAL_VALUE_SET_REQ_FW_PS,
    WLAN_HAL_VALUE_FW_POWER_SAVE_STATE,
    WLAN_HAL_VALUE_SOUNDING_ENTER,
    WLAN_HAL_VALUE_SOUNDING_LEAVE,
    WLAN_HAL_VALUE_SOUNDING_RATE,
    WLAN_HAL_VALUE_SOUNDING_STATUS,
    WLAN_HAL_VALUE_SOUNDING_FW_NDPA,
    WLAN_HAL_VALUE_SOUNDING_CLK,
    WLAN_HAL_VALUE_HW_REG_TIMER_INIT,
    WLAN_HAL_VALUE_HW_REG_TIMER_RESTART,
    WLAN_HAL_VALUE_HW_REG_TIMER_START,
    WLAN_HAL_VALUE_HW_REG_TIMER_STOP,
    WLAN_HAL_VALUE_DL_RSVD_PAGE,
    WLAN_HAL_VALUE_MACID_LINK,
    WLAN_HAL_VALUE_MACID_NOLINK,
    WLAN_HAL_VALUE_MACID_SLEEP,
    WLAN_HAL_VALUE_MACID_WAKEUP,
    WLAN_HAL_VALUE_DUMP_MAC_QUEUE_INFO,
    WLAN_HAL_VALUE_ASIX_IOT,
    WLAN_HAL_VALUE_EN_HW_GENXIN_TSF,
    WLAN_HAL_VALUE_BCN_VALID1,
    WLAN_HAL_VALUE_WOWLAN,
    WLAN_HAL_VALUE_WAKEUP_REASON,
    WLAN_HAL_VALUE_RPWM_TOG,
    WLAN_HW_SET_GPIO_WL_CTRL,
} SDIO_HW_VARIABLES;


typedef enum _HAL_MSG_VARIABLE 
{
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
} MSG_BODY_VARIABLE;

#define WIFI_NULL_STATE					0x00000000
#define WIFI_ASOC_STATE					0x00000001
#define WIFI_SLEEP_STATE				0x00000004
#define WIFI_STATION_STATE				0x00000008
#define WIFI_AP_STATE					0x00000010
#define WIFI_ADHOC_STATE				0x00000020
#define WIFI_ADHOC_MASTER_STATE			0x00000040
#define WIFI_UNDER_LINKING				0x00000080
#define WIFI_UNDER_WPS					0x00000100
#define WIFI_STA_ALIVE_CHK_STATE		0x00000400
#define WIFI_SITE_MONITOR				0x00000800
#define WIFI_MP_STATE					0x00010000
#define WIFI_OP_CH_SWITCHING			0x00800000
#define WIFI_FW_NO_EXIST                0x01000000
#define WIFI_MONITOR_STATE				0x80000000



/*macid config*/
#define SET_9086X_wMBOX1CMD_MACID_CFG_MACID(__pwMBOX1Cmd, __Value)          wf_set_bits_to_le_u8(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RAID(__pwMBOX1Cmd, __Value)           wf_set_bits_to_le_u8(__pwMBOX1Cmd+1, 0, 5, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_SGI_EN(__pwMBOX1Cmd, __Value)         wf_set_bits_to_le_u8(__pwMBOX1Cmd+1, 7, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_BW(__pwMBOX1Cmd, __Value)             wf_set_bits_to_le_u8(__pwMBOX1Cmd+2, 0, 2, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_NO_UPDATE(__pwMBOX1Cmd, __Value)      wf_set_bits_to_le_u8(__pwMBOX1Cmd+2, 3, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_DISPT(__pwMBOX1Cmd, __Value)          wf_set_bits_to_le_u8(__pwMBOX1Cmd+2, 6, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_DISRA(__pwMBOX1Cmd, __Value)          wf_set_bits_to_le_u8(__pwMBOX1Cmd+2, 7, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK0(__pwMBOX1Cmd, __Value)     wf_set_bits_to_le_u8(__pwMBOX1Cmd+3, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK1(__pwMBOX1Cmd, __Value)     wf_set_bits_to_le_u8(__pwMBOX1Cmd+4, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK2(__pwMBOX1Cmd, __Value)     wf_set_bits_to_le_u8(__pwMBOX1Cmd+5, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK3(__pwMBOX1Cmd, __Value)     wf_set_bits_to_le_u8(__pwMBOX1Cmd+6, 0, 8, __Value)


/*MEDIA STATUS*/
#define SET_wMBOX1CMD_MSRRPT_PARM_OPMODE(__pwMBOX1Cmd, __Value)             wf_set_bits_to_le_u8(((wf_u8 *)(__pwMBOX1Cmd)), 0, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MACID_IND(__pwMBOX1Cmd, __Value)          wf_set_bits_to_le_u8(((wf_u8 *)(__pwMBOX1Cmd)), 1, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MIRACAST(__pwMBOX1Cmd, __Value)           wf_set_bits_to_le_u8(((wf_u8 *)(__pwMBOX1Cmd)), 2, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MIRACAST_SINK(__pwMBOX1Cmd, __Value)      wf_set_bits_to_le_u8(((wf_u8 *)(__pwMBOX1Cmd)), 3, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_ROLE(__pwMBOX1Cmd, __Value)               wf_set_bits_to_le_u8(((wf_u8 *)(__pwMBOX1Cmd)), 4, 4, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MACID(__pwMBOX1Cmd, __Value)              wf_set_bits_to_le_u8(((wf_u8 *)(__pwMBOX1Cmd)) + 1, 0, 8, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MACID_END(__pwMBOX1Cmd, __Value)          wf_set_bits_to_le_u8(((wf_u8 *)(__pwMBOX1Cmd)) + 2, 0, 8, (__Value))

/*role*/
#define wMBOX1_MSR_ROLE_RSVD    0
#define wMBOX1_MSR_ROLE_STA     1
#define wMBOX1_MSR_ROLE_AP      2
#define wMBOX1_MSR_ROLE_GC      3
#define wMBOX1_MSR_ROLE_GO      4
#define wMBOX1_MSR_ROLE_ADHOC   6
#define wMBOX1_MSR_ROLE_MAX     7



enum wmbox1_cmd_9086X
{
    wMBOX1_9086X_RSVD_PAGE = 0x00,
    wMBOX1_9086X_MEDIA_STATUS_RPT = 0x01, //use
    wMBOX1_9086X_SCAN_ENABLE = 0x02,
    wMBOX1_9086X_KEEP_ALIVE = 0x03,
    wMBOX1_9086X_DISCON_DECISION = 0x04,
    wMBOX1_9086X_PSD_OFFLOAD = 0x05,
    wMBOX1_9086X_AP_OFFLOAD = 0x08,
    wMBOX1_9086X_BCN_RSVDPAGE = 0x09,
    wMBOX1_9086X_PROBERSP_RSVDPAGE = 0x0A,
    wMBOX1_9086X_FCS_RSVDPAGE = 0x10,
    wMBOX1_9086X_FCS_INFO = 0x11,
    wMBOX1_9086X_AP_WOW_GPIO_CTRL = 0x13,

    wMBOX1_9086X_SET_PWR_MODE = 0x20,
    wMBOX1_9086X_PS_TUNING_PARA = 0x21,
    wMBOX1_9086X_PS_TUNING_PARA2 = 0x22,
    wMBOX1_9086X_P2P_LPS_PARAM = 0x23,
    wMBOX1_9086X_P2P_PS_OFFLOAD = 0x24,
    wMBOX1_9086X_PS_SCAN_ENABLE = 0x25,
    wMBOX1_9086X_SAP_PS_ = 0x26,
    wMBOX1_9086X_INACTIVE_PS_ = 0x27,
    wMBOX1_9086X_FWLPS_IN_IPS_ = 0x28,

    wMBOX1_9086X_MACID_CFG = 0x40, //use
    wMBOX1_9086X_TXBF = 0x41,
    wMBOX1_9086X_RSSI_SETTING = 0x42,
    wMBOX1_9086X_AP_REQ_TXRPT = 0x43,
    wMBOX1_9086X_INIT_RATE_COLLECT = 0x44,
    wMBOX1_9086X_RA_PARA_ADJUST = 0x46,

    wMBOX1_9086X_B_TYPE_TDMA = 0x60,
    wMBOX1_9086X_BT_INFO = 0x61,
    wMBOX1_9086X_FORCE_BT_TXPWR = 0x62,
    wMBOX1_9086X_BT_IGNORE_WLANACT = 0x63,
    wMBOX1_9086X_DAC_SWING_VALUE = 0x64,
    wMBOX1_9086X_ANT_SEL_RSV = 0x65,
    wMBOX1_9086X_WL_OPMODE = 0x66,
    wMBOX1_9086X_BT_MP_OPER = 0x67,
    wMBOX1_9086X_BT_CONTROL = 0x68,
    wMBOX1_9086X_BT_WIFI_CTRL = 0x69,
    wMBOX1_9086X_BT_FW_PATCH = 0x6A,
    wMBOX1_9086X_BT_WLAN_CALIBRATION = 0x6D,

    wMBOX1_9086X_WOWLAN = 0x80,
    wMBOX1_9086X_REMOTE_WAKE_CTRL = 0x81,
    wMBOX1_9086X_AOAC_GLOBAL_INFO = 0x82,
    wMBOX1_9086X_AOAC_RSVD_PAGE = 0x83,
    wMBOX1_9086X_AOAC_RSVD_PAGE2 = 0x84,
    wMBOX1_9086X_D0_SCAN_OFFLOAD_CTRL = 0x85,
    wMBOX1_9086X_D0_SCAN_OFFLOAD_INFO = 0x86,
    wMBOX1_9086X_CHNL_SWITCH_OFFLOAD = 0x87,
    wMBOX1_9086X_P2P_OFFLOAD_RSVD_PAGE = 0x8A,
    wMBOX1_9086X_P2P_OFFLOAD = 0x8B,

    wMBOX1_9086X_RESET_TSF = 0xC0,
    wMBOX1_9086X_MAXID,
};



/*phy status*/
typedef struct
{
    wf_u8 gain:7, trsw:1;

} recv_agc_st;

typedef struct
{
    recv_agc_st path_agc[2];
    wf_u8 ch_corr[2];
    wf_u8 cck_sig_qual_ofdm_pwdb_all;
    wf_u8 cck_agc_rpt_ofdm_cfosho_a;
    wf_u8 cck_rpt_b_ofdm_cfosho_b;
    wf_u8 rsvd_1;
    wf_u8 noise_power_db_msb;
    wf_s8 path_cfotail[2];
    wf_u8 pcts_mask[2];
    wf_s8 stream_rxevm[2];
    wf_u8 path_rxsnr[2];
    wf_u8 noise_power_db_lsb;
    wf_u8 rsvd_2[3];
    wf_u8 stream_csi[2];
    wf_u8 stream_target_csi[2];
    wf_s8 sig_evm;
    wf_u8 rsvd_3;
    wf_u8 antsel_rx_keep_2:1;
    wf_u8 sgi_en:1;
    wf_u8 rxsc:2;
    wf_u8 idle_long:1;
    wf_u8 r_ant_train_en:1;
    wf_u8 ant_sel_b:1;
    wf_u8 ant_sel:1;

} recv_phy_status_st;


typedef struct
{
	wf_u32 is_normal_chip;
	wf_u32 customer_id;
	wf_u32 wifi_spec;
	wf_u32 cut_version;
	wf_u32 Regulation2_4G;
	wf_u32 TypeGPA;
	wf_u32 TypeAPA;
	wf_u32 TypeGLNA;
	wf_u32 TypeALNA;
	wf_u32 RFEType;
	wf_u32 PackageType;
	wf_u32 boardConfig;
} phy_config_t;


#ifdef CONFIG_RICHV100
typedef struct fw_init_param_
{
	wf_u32 send_msg[7];//input param, usb uses the first DW, but SDIO needs mac addr to send.
	wf_u32 recv_msg[9];
}hw_param_st;


#else

typedef struct fw_init_param_
{
	wf_u32 work_mode;
	wf_u32 mac_addr[WF_ETH_ALEN];
	wf_u32 concurrent_mode;
	wf_u32 rx_agg_enable;
}hw_param_st;
#endif
typedef struct phy_cali_ 
{
    wf_u8 TxPowerTrackControl;
    wf_s8 Remnant_CCKSwingIdx;
    wf_s8 Remnant_OFDMSwingIdx;
    wf_u8 rsvd;
} phy_cali_t;

/*sync to mcu, odm msg*/
typedef struct odm_msg_st_ {
    wf_u64 tx_bytes;
    wf_u64 rx_bytes;
    wf_u32 cur_wireless_mode;
    wf_u32 CurrentBandType;
    wf_u32 ForcedDataRate;
    wf_u32 nCur40MhzPrimeSC;
    wf_u32 dot11PrivacyAlgrthm;
    wf_u32 CurrentChannelBW;
    wf_u32 CurrentChannel;
    wf_u32 net_closed;
    wf_u32 u1ForcedIgiLb;
    wf_u32 bScanInProcess;
    wf_u32 bpower_saving;
    wf_u32 traffic_stat_cur_tx_tp;
    wf_u32 traffic_stat_cur_rx_tp;
    wf_u32 msgWdgStateVal;
    wf_u32 ability;
    wf_u32 Rssi_Min;
    wf_u32 dig_CurIGValue;
    wf_u32 wifi_direct;
    wf_u32 wifi_display;
    wf_u64 dbg_cmp;
    wf_u32 dbg_level;
    wf_u32 PhyRegPgVersion;
    wf_u32 PhyRegPgValueType;
    phy_cali_t phy_cali;
    wf_u32 bDisablePowerTraining;
    wf_u32 fw_state;
    wf_u32 sta_count;
} mcu_msg_body_st;

typedef enum ODM_ABILITY_OPS_
{
    ODM_DIS_ALL_FUNC,
    ODM_FUNC_SET,
    ODM_FUNC_CLR,
    ODM_FUNC_BACKUP,
    ODM_FUNC_RESTORE,
} MCU_MSG_BODY_ABILITY_OPS;

struct mcu_ht_priv {
    wf_u8 ht_option;
    wf_u8 ampdu_enable;
    wf_u8 tx_amsdu_enable;
    wf_u8 bss_coexist;

    wf_u32 tx_amsdu_maxlen;
    wf_u32 rx_ampdu_maxlen;

    wf_u8 rx_ampdu_min_spacing;

    wf_u8 ch_offset;
    wf_u8 sgi_20m;
    wf_u8 sgi_40m;

    wf_u8 agg_enable_bitmap;
    wf_u8 candidate_tid_bitmap;

    wf_u8 ldpc_cap;
    wf_u8 stbc_cap;
    wf_u8 smps_cap;
    wf_80211_mgmt_ht_cap_t ht_cap;

};

typedef struct odm_wdn_info_st_
{
    wf_u8  bUsed;
    wf_u32 mac_id;
    wf_u8  hwaddr[MAC_ADDR_LEN];
    wf_u8  ra_rpt_linked;
    wf_u8  wireless_mode;
    wf_u8  rssi_level;
    wf_u8  ra_change;
    struct mcu_ht_priv htpriv;
} mcu_msg_sta_info_st;




wf_s32 translate_percentage_to_dbm(wf_u32 SignalStrengthIndex);
wf_s32 signal_scale_mapping(wf_s32 current_sig);





wf_s32 wf_mcu_cmd_get_status(nic_info_st *nic_info,wf_u32 cmd);
wf_s32 wf_mcu_enable_xmit(nic_info_st *nic_info);
wf_s32 wf_mcu_disable_xmit(nic_info_st *nic_info);
wf_s32 wf_mcu_check_tx_buff(nic_info_st *nic_info);
wf_s32 wf_mcu_disable_fw_dbginfo(nic_info_st *pnic_info);

wf_s32 wf_mcu_set_macaddr(nic_info_st *nic_info, wf_u8 * val);
wf_s32 wf_mcu_get_chip_version(nic_info_st *nic_info, wf_u32 *version);
wf_s32 wf_mcu_set_concurrent_mode(nic_info_st *nic_info, wf_bool concur_mode);
wf_s32 wf_mcu_set_op_mode(nic_info_st *nic_info, wf_u32 mode);
wf_s32 wf_mcu_set_hw_invalid_all(nic_info_st *nic_info);
wf_s32 wf_mcu_set_ch_bw(nic_info_st *nic_info, wf_u32 *args, wf_u32 arg_len);
wf_s32 wf_mcu_set_hw_reg(nic_info_st *nic_info, wf_u32 * value, wf_u32 len);
wf_s32 wf_mcu_set_config_xmit(nic_info_st *nic_info, wf_s32 event, wf_u32 val);
wf_s32 wf_mcu_set_user_info(nic_info_st *nic_info, wf_bool state);
wf_s32 wf_mcu_set_mlme_scan(nic_info_st *nic_info, wf_bool enable);
wf_s32 wf_mcu_set_mlme_join(nic_info_st *nic_info, wf_u8 type);
wf_s32 wf_mcu_set_bssid(nic_info_st *nic_info, wf_u8 *bssid);
wf_s32 wf_mcu_set_sifs(nic_info_st *nic_info, wf_u32 value);
wf_s32 wf_mcu_set_macid_wakeup(nic_info_st *nic_info, wf_u32 wdn_id);
wf_s32 wf_mcu_set_basic_rate(nic_info_st *nic_info, wf_u16 br_cfg);
wf_s32 wf_mcu_set_preamble(nic_info_st *nic_info, wf_u8 short_val);
wf_s32 wf_mcu_set_slot_time(nic_info_st *nic_info, wf_u32 slotTime);
wf_s32 wf_mcu_set_media_status(nic_info_st *nic_info, wf_u32 status);
wf_s32 wf_mcu_set_phy_config(nic_info_st *nic_info, phy_config_t *cfg);
wf_s32 wf_mcu_set_bcn_intv(nic_info_st *nic_info, wf_u16 val);
wf_s32 wf_mcu_set_wmm_para_enable(nic_info_st *nic_info,  wdn_net_info_st *wdn_info);
wf_s32 wf_mcu_set_wmm_para_disable(nic_info_st *nic_info, wdn_net_info_st *wdn_info);
wf_s32 wf_mcu_set_correct_tsf(nic_info_st *nic_info, wf_u64 tsf);
wf_s32 wf_mcu_set_on_rcr_am (nic_info_st *nic_info, wf_bool var_on);
wf_s32 wf_mcu_set_dk_cfg (nic_info_st *nic_info, wf_u32 auth_algrthm, wf_bool dk_en);
wf_s32 wf_mcu_set_sec_cfg (nic_info_st *nic_info, wf_u8 val);
wf_s32 wf_mcu_set_sec_cam (nic_info_st *nic_info, wf_u8 cam_id, wf_u16 ctrl, wf_u8 *mac, wf_u8 *key);
wf_s32 wf_mcu_set_min_ampdu_space(nic_info_st *pnic_info, wf_u8 min_space);
wf_s32 wf_mcu_set_max_ampdu_len(nic_info_st *pnic_info, wf_u8 max_len);
wf_s32 wf_mcu_set_ac_vo(nic_info_st *pnic_info);
wf_s32 wf_mcu_set_ac_vi(nic_info_st *pnic_info);
wf_s32 wf_mcu_set_ac_be(nic_info_st *pnic_info);
wf_s32 wf_mcu_set_ac_bk(nic_info_st *pnic_info);
wf_s32 wf_mcu_get_bcn_valid(nic_info_st *pnic_info,wf_u32 *val32);
wf_s32 wf_mcu_set_bcn_valid(nic_info_st *pnic_info);
wf_s32 wf_mcu_set_bcn_sel(nic_info_st *pnic_info);
wf_s32 wf_mcu_update_thermal(nic_info_st *nic_info);

wf_s32 wf_mcu_handle_rf_lck_calibrate(nic_info_st *nic_info);
wf_s32 wf_mcu_handle_rf_iq_calibrate(nic_info_st *nic_info, wf_u8 channel);

wf_s32 wf_mcu_msg_body_get(nic_info_st *nic_info,mcu_msg_body_st *mcu_msg);
wf_s32 wf_mcu_msg_body_set(nic_info_st *nic_info,mcu_msg_body_st *mcu_msg);
wf_s32 wf_mcu_msg_body_set_ability(nic_info_st *nic_info,MCU_MSG_BODY_ABILITY_OPS ops,  wf_u32 ability);
wf_s32 wf_mcu_msg_body_sync(nic_info_st *nic_info,MSG_BODY_VARIABLE ops,  wf_u32 val);
wf_s32 wf_mcu_msg_sta_info_get(nic_info_st *nic_info, wf_u32 wdn_id,mcu_msg_sta_info_st *msg_sta);
wf_s32 wf_mcu_msg_sta_info_set(nic_info_st *nic_info, mcu_msg_sta_info_st *msg_sta);

wf_s32 wf_mcu_rate_table_update(nic_info_st *nic_info, wdn_net_info_st *wdn_net_info);



#ifdef CONFIG_RICHV100
wf_s32 wf_mcu_init_hardware1(nic_info_st *nic_info);
wf_s32 wf_mcu_init_hardware2(nic_info_st *nic_info, hw_param_st *param);
wf_s32 wf_mcu_burst_pktlen_init(nic_info_st *nic_info);
wf_s32 wf_mcu_ant_sel_init(nic_info_st *nic_info);
wf_s32 wf_mcu_update_tx_fifo(nic_info_st *nic_info);
wf_s32 wf_mcu_cca_config(nic_info_st *nic_info);
wf_s32 wf_mcu_msg_init(nic_info_st * nic_info);
wf_s32 wf_mcu_msg_body_init(nic_info_st *nic_info, mcu_msg_body_st *msg);
wf_s32 wf_mcu_msg_init_default(nic_info_st *nic_info);
#endif


#ifdef CONFIG_RICHV200
wf_s32 wf_mcu_hw_init(nic_info_st *nic_info, hw_param_st *param);
wf_s32 wf_mcu_ars_init(nic_info_st *nic_info);
wf_s32 wf_mcu_reset_chip(nic_info_st *nic_info);
wf_s32 wf_mcu_set_agg_param(nic_info_st *nic_info, wf_u32 agg_size, wf_u32 agg_timeout, wf_u32 agg_dma_enable);
#endif

#ifdef CONFIG_LPS
wf_s32 wf_mcu_set_lps_opt(nic_info_st *pnic_info, wf_u32 data);
wf_s32 wf_mcu_set_lps_config(nic_info_st *nic_info);
wf_s32 wf_mcu_set_fw_lps_config(nic_info_st *pnic_info);
wf_s32 wf_mcu_set_fw_lps_get(nic_info_st *pnic_info);
wf_s32 wf_mcu_set_rsvd_page_h2c_loc(nic_info_st * nic_info, void *rsvdpage);
#endif

#ifdef CFG_ENABLE_AP_MODE
wf_s32 wf_mcu_set_ap_mode(nic_info_st *pnic_info);
wf_s32 wf_mcu_set_sec(nic_info_st *pnic_info);
wf_s32 wf_ap_odm_connect_media_status(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);
wf_s32 wf_ap_odm_disconnect_media_status(nic_info_st *pnic_info,wdn_net_info_st *pwdn_info);
#endif

#ifdef CFG_ENABLE_ADHOC_MODE
wf_s32 wf_mcu_set_bcn_reg(nic_info_st *pnic_info);
wf_s32 wf_adhoc_odm_connect_media_status(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);
wf_s32 wf_adhoc_odm_disconnect_media_status(nic_info_st *pnic_info,wdn_net_info_st *pwdn_info);
#endif

#ifdef CONFIG_MP_MODE
wf_s32 wf_mcu_mp_bb_rf_gain_offset(nic_info_st *nic_info);
wf_s32 wf_mcu_set_usb_agg_normal(nic_info_st *nic_info, wf_u8 cur_wireless_mode);

#endif

#endif
