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

#ifndef __wlan9086X_CMD_H__
#define __wlan9086X_CMD_H__

enum wmbox1_cmd_9086X {
	wMBOX1_9086X_RSVD_PAGE = 0x00,
	wMBOX1_9086X_MEDIA_STATUS_RPT = 0x01,
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

	wMBOX1_9086X_MACID_CFG = 0x40,
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

#define SET_9086X_wMBOX1CMD_RSVDPAGE_LOC_PROBE_RSP(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_RSVDPAGE_LOC_PSPOLL(__pwMBOX1Cmd, __Value)				SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_RSVDPAGE_LOC_NULL_DATA(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_RSVDPAGE_LOC_QOS_NULL_DATA(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_RSVDPAGE_LOC_BT_QOS_NULL_DATA(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+4, 0, 8, __Value)

#define SET_9086X_wMBOX1CMD_KEEPALIVE_PARM_ENABLE(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 1, __Value)
#define SET_9086X_wMBOX1CMD_KEEPALIVE_PARM_ADOPT(__pwMBOX1Cmd, __Value)				SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 1, 1, __Value)
#define SET_9086X_wMBOX1CMD_KEEPALIVE_PARM_PKT_TYPE(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 2, 1, __Value)
#define SET_9086X_wMBOX1CMD_KEEPALIVE_PARM_CHECK_PERIOD(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 0, 8, __Value)

#define SET_9086X_wMBOX1CMD_DISCONDECISION_PARM_ENABLE(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 1, __Value)
#define SET_9086X_wMBOX1CMD_DISCONDECISION_PARM_ADOPT(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 1, 1, __Value)
#define SET_9086X_wMBOX1CMD_DISCONDECISION_PARM_CHECK_PERIOD(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_DISCONDECISION_PARM_TRY_PKT_NUM(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 0, 8, __Value)

#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_MODE(__pwMBOX1Cmd, __Value)				SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_RLBM(__pwMBOX1Cmd, __Value)				SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 4, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_SMART_PS(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 4, 4, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_BCN_PASS_TIME(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_ALL_QUEUE_UAPSD(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_BCN_EARLY_wMBOX0_RPT(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 2, 1, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_PWR_STATE(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+4, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_BYTE5(__pwMBOX1Cmd, __Value)				SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+5, 0, 8, __Value)

#define GET_9086X_wMBOX1CMD_PWRMODE_PARM_MODE(__pwMBOX1Cmd)					LE_BITS_TO_1BYTE(__pwMBOX1Cmd, 0, 8)

#define SET_9086X_wMBOX1CMD_PSTUNE_PARM_BCN_TO_LIMIT(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PSTUNE_PARM_DTIM_TIMEOUT(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PSTUNE_PARM_ADOPT(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 0, 1, __Value)
#define SET_9086X_wMBOX1CMD_PSTUNE_PARM_PS_TIMEOUT(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 1, 7, __Value)
#define SET_9086X_wMBOX1CMD_PSTUNE_PARM_DTIM_PERIOD(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+3, 0, 8, __Value)

#define SET_9086X_wMBOX1CMD_MACID_CFG_MACID(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RAID(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 0, 5, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_SGI_EN(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 7, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_BW(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 0, 2, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_NO_UPDATE(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 3, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_DISPT(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 6, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_DISRA(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 7, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK0(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+3, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK1(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+4, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK2(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+5, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK3(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+6, 0, 8, __Value)

#define SET_9086X_wMBOX1CMD_RSSI_SETTING_MACID(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_RSSI_SETTING_RSSI(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 0, 7, __Value)
#define SET_9086X_wMBOX1CMD_RSSI_SETTING_ULDL_STATE(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+3, 0, 8, __Value)

#define SET_9086X_wMBOX1CMD_APREQRPT_PARM_MACID1(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_APREQRPT_PARM_MACID2(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 0, 8, __Value)

#define SET_9086X_wMBOX1CMD_BT_PWR_IDX(__pwMBOX1Cmd, __Value)							SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)

#define SET_9086X_wMBOX1CMD_BT_MPOPER_VER(__pwMBOX1Cmd, __Value)							SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 4, __Value)
#define SET_9086X_wMBOX1CMD_BT_MPOPER_REQNUM(__pwMBOX1Cmd, __Value)							SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 4, 4, __Value)
#define SET_9086X_wMBOX1CMD_BT_MPOPER_IDX(__pwMBOX1Cmd, __Value)							SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_BT_MPOPER_PARAM1(__pwMBOX1Cmd, __Value)							SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_BT_MPOPER_PARAM2(__pwMBOX1Cmd, __Value)							SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+3, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_BT_MPOPER_PARAM3(__pwMBOX1Cmd, __Value)							SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+4, 0, 8, __Value)

#define SET_9086X_wMBOX1CMD_BT_FW_PATCH_SIZE(__pwMBOX1Cmd, __Value)					SET_BITS_TO_LE_2BYTE((pu1Byte)(__pwMBOX1Cmd), 0, 16, __Value)
#define SET_9086X_wMBOX1CMD_BT_FW_PATCH_ADDR0(__pwMBOX1Cmd, __Value) 					SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_BT_FW_PATCH_ADDR1(__pwMBOX1Cmd, __Value) 					SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_BT_FW_PATCH_ADDR2(__pwMBOX1Cmd, __Value) 					SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+4, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_BT_FW_PATCH_ADDR3(__pwMBOX1Cmd, __Value) 					SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+5, 0, 8, __Value)

void Func_Of_Set_Fwpwrmode_Cmd(PNIC Nic, u8 Mode);
void Func_Of_Set_Fwjoinbssrpt_Cmd(PNIC Nic, u8 mstatus);
void Func_Of_Set_Rssi_Cmd(PNIC Nic, u8 * param);
void Func_Of_Add_Rateatid(PNIC Nic, u64 bitmap, u8 * arg,
						   u8 rssi_level);
void wll9086x_fw_try_ap_cmd(PNIC Nic, u32 need_ack);
void Func_Of_Set_Fwpstuneparam_Cmd(PNIC Nic);
void Func_Of_Set_Fwmacidconfig_Cmd(PNIC Nic, u8 mac_id, u8 raid,
									u8 bw, u8 sgi, u32 mask);
void Func_Of_Set_Fwbtmpoper_Cmd(PNIC Nic, u8 idx, u8 ver, u8 reqnum,
								 u8 * param);
void Func_Of_Download_Rsvd_Page(PNIC Nic, u8 mstatus);
#ifdef CONFIG_P2P
void Func_Of_Set_P2P_Ps_Offload_Cmd(PNIC Nic, u8 p2p_ps_state);
#endif

void Func_Check_Fw_Rsvdpage_Content(PNIC Nic);

#ifdef CONFIG_P2P_WOWLAN
void wll9086x_set_p2p_wowlan_offload_cmd(PNIC Nic);
#endif

void Func_Of_Set_Fwpwrmodeinips_Cmd(PNIC Nic, u8 cmd_param);

#ifdef CONFIG_TSF_RESET_OFFLOAD
u8 Func_Of_Reset_Tsf(PNIC Nic, u8 reset_port);
#endif

#endif
