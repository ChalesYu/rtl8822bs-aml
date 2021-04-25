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

#ifndef __COMMON_wMBOX1_H__
#define __COMMON_wMBOX1_H__

enum wmbox1_cmd {
	wMBOX1_RSVD_PAGE = 0x00,
	wMBOX1_MEDIA_STATUS_RPT = 0x01,
	wMBOX1_SCAN_ENABLE = 0x02,
	wMBOX1_KEEP_ALIVE = 0x03,
	wMBOX1_DISCON_DECISION = 0x04,
	wMBOX1_PSD_OFFLOAD = 0x05,
	wMBOX1_AP_OFFLOAD = 0x08,
	wMBOX1_BCN_RSVDPAGE = 0x09,
	wMBOX1_PROBERSP_RSVDPAGE = 0x0A,
	wMBOX1_FCS_RSVDPAGE = 0x10,
	wMBOX1_FCS_INFO = 0x11,
	wMBOX1_AP_WOW_GPIO_CTRL = 0x13,
	wMBOX1_CHNL_SWITCH_OPER_OFFLOAD = 0x1C,

	wMBOX1_SET_PWR_MODE = 0x20,
	wMBOX1_PS_TUNING_PARA = 0x21,
	wMBOX1_PS_TUNING_PARA2 = 0x22,
	wMBOX1_P2P_LPS_PARAM = 0x23,
	wMBOX1_P2P_PS_OFFLOAD = 0x24,
	wMBOX1_PS_SCAN_ENABLE = 0x25,
	wMBOX1_SAP_PS_ = 0x26,
	wMBOX1_INACTIVE_PS_ = 0x27,
	wMBOX1_FWLPS_IN_IPS_ = 0x28,

	wMBOX1_MACID_CFG = 0x40,
	wMBOX1_TXBF = 0x41,
	wMBOX1_RSSI_SETTING = 0x42,
	wMBOX1_AP_REQ_TXRPT = 0x43,
	wMBOX1_INIT_RATE_COLLECT = 0x44,
	wMBOX1_IQ_CALIBRATION = 0x45,

	wMBOX1_RA_PARA_ADJUST = 0x47,
	wMBOX1_DYNAMIC_TX_PATH = 0x48,

	wMBOX1_FW_TRACE_EN = 0x49,

	wMBOX1_B_TYPE_TDMA = 0x60,
	wMBOX1_BT_INFO = 0x61,
	wMBOX1_FORCE_BT_TXPWR = 0x62,
	wMBOX1_BT_IGNORE_WLANACT = 0x63,
	wMBOX1_DAC_SWING_VALUE = 0x64,
	wMBOX1_ANT_SEL_RSV = 0x65,
	wMBOX1_WL_OPMODE = 0x66,
	wMBOX1_BT_MP_OPER = 0x67,
	wMBOX1_BT_CONTROL = 0x68,
	wMBOX1_BT_WIFI_CTRL = 0x69,
	wMBOX1_BT_FW_PATCH = 0x6A,

	wMBOX1_WOWLAN = 0x80,
	wMBOX1_REMOTE_WAKE_CTRL = 0x81,
	wMBOX1_AOAC_GLOBAL_INFO = 0x82,
	wMBOX1_AOAC_RSVD_PAGE = 0x83,
	wMBOX1_AOAC_RSVD_PAGE2 = 0x84,
	wMBOX1_D0_SCAN_OFFLOAD_CTRL = 0x85,
	wMBOX1_D0_SCAN_OFFLOAD_INFO = 0x86,
	wMBOX1_CHNL_SWITCH_OFFLOAD = 0x87,
	wMBOX1_AOAC_RSVDPAGE3 = 0x88,
	wMBOX1_P2P_OFFLOAD_RSVD_PAGE = 0x8A,
	wMBOX1_P2P_OFFLOAD = 0x8B,

	wMBOX1_RESET_TSF = 0xC0,
	wMBOX1_BCNHWSEQ = 0xC2,
	wMBOX1_MAXID,
};

#define wMBOX1_INACTIVE_PS_LEN		3
#define wMBOX1_RSVDPAGE_LOC_LEN		5
#define wMBOX1_MEDIA_STATUS_RPT_LEN		3
#define wMBOX1_KEEP_ALIVE_CTRL_LEN	2
#define wMBOX1_DISCON_DECISION_LEN	 	3
#define wMBOX1_AP_OFFLOAD_LEN		3
#define wMBOX1_AP_WOW_GPIO_CTRL_LEN	4
#define wMBOX1_AP_PS_LEN			2
#define wMBOX1_PWRMODE_LEN			7
#define wMBOX1_PSTUNEPARAM_LEN			4
#define wMBOX1_MACID_CFG_LEN	 	7
#define wMBOX1_BTMP_OPER_LEN			5
#define wMBOX1_WOWLAN_LEN			5
#define wMBOX1_REMOTE_WAKE_CTRL_LEN	3
#define wMBOX1_AOAC_GLOBAL_INFO_LEN	2
#define wMBOX1_AOAC_RSVDPAGE_LOC_LEN	7
#define wMBOX1_SCAN_OFFLOAD_CTRL_LEN	4
#define wMBOX1_BT_FW_PATCH_LEN			6
#define wMBOX1_RSSI_SETTING_LEN		4
#define wMBOX1_AP_REQ_TXRPT_LEN		2
#define wMBOX1_FORCE_BT_TXPWR_LEN		3
#define wMBOX1_BCN_RSVDPAGE_LEN		5
#define wMBOX1_PROBERSP_RSVDPAGE_LEN	5
#define wMBOX1_P2PRSVDPAGE_LOC_LEN	5
#define wMBOX1_P2P_OFFLOAD_LEN	3

//#define eqMacAddr(a,b)						( ((a)[0]==(b)[0] && (a)[1]==(b)[1] && (a)[2]==(b)[2] && (a)[3]==(b)[3] && (a)[4]==(b)[4] && (a)[5]==(b)[5]) ? 1:0 )
#define cpMacAddr(des,src)					((des)[0]=(src)[0],(des)[1]=(src)[1],(des)[2]=(src)[2],(des)[3]=(src)[3],(des)[4]=(src)[4],(des)[5]=(src)[5])
#define cpIpAddr(des,src)					((des)[0]=(src)[0],(des)[1]=(src)[1],(des)[2]=(src)[2],(des)[3]=(src)[3])

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
#define GET_ARP_PKT_LLC_TYPE(__pHeader)					ReadLE2Byte(((u8 *)(__pHeader)) + 6)

#define GET_ARP_PKT_OPERATION(__pHeader)				ReadLE2Byte(((u8 *)(__pHeader)) + 6)
#define GET_ARP_PKT_SENDER_MAC_ADDR(__pHeader, _val) 	cpMacAddr((u8*)(_val), ((u8*)(__pHeader))+8)
#define GET_ARP_PKT_SENDER_IP_ADDR(__pHeader, _val) 		cpIpAddr((u8*)(_val), ((u8*)(__pHeader))+14)
#define GET_ARP_PKT_TARGET_MAC_ADDR(__pHeader, _val) 	cpMacAddr((u8*)(_val), ((u8*)(__pHeader))+18)
#define GET_ARP_PKT_TARGET_IP_ADDR(__pHeader, _val) 	cpIpAddr((u8*)(_val), ((u8*)(__pHeader))+24)

#define SET_ARP_PKT_HW(__pHeader, __Value)					WriteLE2Byte(((u8 *)(__pHeader)) + 0, __Value)
#define SET_ARP_PKT_PROTOCOL(__pHeader, __Value)			WriteLE2Byte(((u8 *)(__pHeader)) + 2, __Value)
#define SET_ARP_PKT_HW_ADDR_LEN(__pHeader, __Value)			WriteLE1Byte(((u8 *)(__pHeader)) + 4, __Value)
#define SET_ARP_PKT_PROTOCOL_ADDR_LEN(__pHeader, __Value)	WriteLE1Byte(((u8 *)(__pHeader)) + 5, __Value)
#define SET_ARP_PKT_OPERATION(__pHeader, __Value)			WriteLE2Byte(((u8 *)(__pHeader)) + 6, __Value)
#define SET_ARP_PKT_SENDER_MAC_ADDR(__pHeader, _val) 	cpMacAddr(((u8*)(__pHeader))+8, (u8*)(_val))
#define SET_ARP_PKT_SENDER_IP_ADDR(__pHeader, _val) 		cpIpAddr(((u8*)(__pHeader))+14, (u8*)(_val))
#define SET_ARP_PKT_TARGET_MAC_ADDR(__pHeader, _val) 	cpMacAddr(((u8*)(__pHeader))+18, (u8*)(_val))
#define SET_ARP_PKT_TARGET_IP_ADDR(__pHeader, _val) 		cpIpAddr(((u8*)(__pHeader))+24, (u8*)(_val))

#define FW_WOWLAN_FUN_EN				BIT(0)
#define FW_WOWLAN_PATTERN_MATCH			BIT(1)
#define FW_WOWLAN_MAGIC_PKT				BIT(2)
#define FW_WOWLAN_UNICAST				BIT(3)
#define FW_WOWLAN_ALL_PKT_DROP			BIT(4)
#define FW_WOWLAN_GPIO_ACTIVE			BIT(5)
#define FW_WOWLAN_REKEY_WAKEUP			BIT(6)
#define FW_WOWLAN_DEAUTH_WAKEUP			BIT(7)

#define FW_WOWLAN_GPIO_WAKEUP_EN		BIT(0)
#define FW_FW_PARSE_MAGIC_PKT			BIT(1)

#define FW_REMOTE_WAKE_CTRL_EN			BIT(0)
#define FW_REALWOWLAN_EN				BIT(5)

#define FW_WOWLAN_KEEP_ALIVE_EN			BIT(0)
#define FW_ADOPT_USER					BIT(1)
#define FW_WOWLAN_KEEP_ALIVE_PKT_TYPE	BIT(2)

#define FW_REMOTE_WAKE_CTRL_EN			BIT(0)
#define FW_ARP_EN						BIT(1)
#define FW_REALWOWLAN_EN				BIT(5)
#define FW_WOW_FW_UNICAST_EN			BIT(7)

#endif

#define SET_wMBOX1CMD_RSVDPAGE_LOC_PROBE_RSP(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_PSPOLL(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_NULL_DATA(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_QOS_NULL_DATA(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_BT_QOS_NULL_DATA(__pwMBOX1Cmd, __Value)SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+4, 0, 8, __Value)

#define SET_wMBOX1CMD_MSRRPT_PARM_OPMODE(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(((u8 *)(__pwMBOX1Cmd)), 0, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MACID_IND(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(((u8 *)(__pwMBOX1Cmd)), 1, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MIRACAST(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(((u8 *)(__pwMBOX1Cmd)), 2, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MIRACAST_SINK(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(((u8 *)(__pwMBOX1Cmd)), 3, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_ROLE(__pwMBOX1Cmd, __Value)				SET_BITS_TO_LE_1BYTE(((u8 *)(__pwMBOX1Cmd)), 4, 4, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MACID(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(((u8 *)(__pwMBOX1Cmd)) + 1, 0, 8, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MACID_END(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(((u8 *)(__pwMBOX1Cmd)) + 2, 0, 8, (__Value))

#define GET_wMBOX1CMD_MSRRPT_PARM_OPMODE(__pwMBOX1Cmd)		LE_BITS_TO_1BYTE(((u8 *)(__pwMBOX1Cmd)), 0, 1)
#define GET_wMBOX1CMD_MSRRPT_PARM_MIRACAST(__pwMBOX1Cmd)		LE_BITS_TO_1BYTE(((u8 *)(__pwMBOX1Cmd)), 2, 1)
#define GET_wMBOX1CMD_MSRRPT_PARM_MIRACAST_SINK(__pwMBOX1Cmd)	LE_BITS_TO_1BYTE(((u8 *)(__pwMBOX1Cmd)), 3, 1)
#define GET_wMBOX1CMD_MSRRPT_PARM_ROLE(__pwMBOX1Cmd)			LE_BITS_TO_1BYTE(((u8 *)(__pwMBOX1Cmd)), 4, 4)

#define wMBOX1_MSR_ROLE_RSVD	0
#define wMBOX1_MSR_ROLE_STA	1
#define wMBOX1_MSR_ROLE_AP		2
#define wMBOX1_MSR_ROLE_GC		3
#define wMBOX1_MSR_ROLE_GO		4
#define wMBOX1_MSR_ROLE_ADHOC	6
#define wMBOX1_MSR_ROLE_MAX	7

extern const char *const _wmbox1_msr_role_str[];
#define wmbox1_msr_role_str(role) (((role) >= wMBOX1_MSR_ROLE_MAX) ? _wmbox1_msr_role_str[wMBOX1_MSR_ROLE_MAX] : _wmbox1_msr_role_str[(role)])

#define wMBOX1_MSR_FMT "%s %s%s"
#define wMBOX1_MSR_ARG(wmbox1_msr) \
	GET_wMBOX1CMD_MSRRPT_PARM_OPMODE((wmbox1_msr)) ? " C" : "", \
	wmbox1_msr_role_str(GET_wMBOX1CMD_MSRRPT_PARM_ROLE((wmbox1_msr))), \
	GET_wMBOX1CMD_MSRRPT_PARM_MIRACAST((wmbox1_msr)) ? (GET_wMBOX1CMD_MSRRPT_PARM_MIRACAST_SINK((wmbox1_msr)) ? " MSINK" : " MSRC") : ""

s32 Func_Of_Proc_Chip_Hw_Set_Fwmediastatusrpt_Cmd(PNIC Nic, bool opmode,
									 bool miracast, bool miracast_sink, u8 role,
									 u8 macid, bool macid_ind, u8 macid_end);
s32 Func_Of_Proc_Chip_Hw_Set_Fwmediastatusrpt_Single_Cmd(PNIC Nic, bool opmode,
											bool miracast, bool miracast_sink,
											u8 role, u8 macid);

#define SET_wMBOX1CMD_KEEPALIVE_PARM_ENABLE(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 1, __Value)
#define SET_wMBOX1CMD_KEEPALIVE_PARM_ADOPT(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 1, 1, __Value)
#define SET_wMBOX1CMD_KEEPALIVE_PARM_PKT_TYPE(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 2, 1, __Value)
#define SET_wMBOX1CMD_KEEPALIVE_PARM_CHECK_PERIOD(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 0, 8, __Value)

#define SET_wMBOX1CMD_DISCONDECISION_PARM_ENABLE(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 1, __Value)
#define SET_wMBOX1CMD_DISCONDECISION_PARM_ADOPT(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 1, 1, __Value)
#define SET_wMBOX1CMD_DISCONDECISION_PARM_CHECK_PERIOD(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 0, 8, __Value)
#define SET_wMBOX1CMD_DISCONDECISION_PARM_TRY_PKT_NUM(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 0, 8, __Value)

#define SET_wMBOX1CMD_AP_WOWLAN_EN(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_wMBOX1CMD_AP_WOWLAN_RSVDPAGE_LOC_BCN(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_wMBOX1CMD_AP_WOWLAN_RSVDPAGE_LOC_ProbeRsp(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_INDEX(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 4, __Value)
#define SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_wMBOX0_EN(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 4, 1, __Value)
#define SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_PLUS(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 5, 1, __Value)
#define SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_HIGH_ACTIVE(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 6, 1, __Value)
#define SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_EN(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 7, 1, __Value)
#define SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_DURATION(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 8, __Value)
#define SET_wMBOX1CMD_AP_WOW_GPIO_CTRL_wMBOX0_DURATION(__pwMBOX1Cmd, __Value)SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_wMBOX1CMD_AP_WOW_PS_EN(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 1, __Value)
#define SET_wMBOX1CMD_AP_WOW_PS_32K_EN(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 1, 1, __Value)
#define SET_wMBOX1CMD_AP_WOW_PS_RF(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 2, 1, __Value)
#define SET_wMBOX1CMD_AP_WOW_PS_DURATION(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 8, __Value)

#define SET_wMBOX1CMD_WOWLAN_FUNC_ENABLE(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_PATTERN_MATCH_ENABLE(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 1, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_MAGIC_PKT_ENABLE(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 2, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_UNICAST_PKT_ENABLE(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 3, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_ALL_PKT_DROP(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 4, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_GPIO_ACTIVE(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 5, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_REKEY_WAKE_UP(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 6, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_DISCONNECT_WAKE_UP(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 7, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_GPIONUM(__pwMBOX1Cmd, __Value)				SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 7, __Value)
#define SET_wMBOX1CMD_WOWLAN_DATAPIN_WAKE_UP(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 7, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_GPIO_DURATION(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_wMBOX1CMD_WOWLAN_GPIO_PULSE_EN(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 0, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_GPIO_PULSE_COUNT(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 1, 7, __Value)
#define SET_wMBOX1CMD_WOWLAN_LOWPR_RX(__pwMBOX1Cmd, __Value)				SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+4, 0, 1, __Value)
#define SET_wMBOX1CMD_WOWLAN_CHANGE_UNIT(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+4, 2, 1, __Value)
#define SET_wMBOX1CMD_REMOTE_WAKECTRL_ENABLE(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 1, __Value)
#define SET_wMBOX1CMD_REMOTE_WAKE_CTRL_ARP_OFFLOAD_EN(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 1, 1, __Value)
#define SET_wMBOX1CMD_REMOTE_WAKE_CTRL_NDP_OFFLOAD_EN(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 2, 1, __Value)
#define SET_wMBOX1CMD_REMOTE_WAKE_CTRL_GTK_OFFLOAD_EN(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 3, 1, __Value)
#define SET_wMBOX1CMD_REMOTE_WAKE_CTRL_NLO_OFFLOAD_EN(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 4, 1, __Value)
#define SET_wMBOX1CMD_REMOTE_WAKE_CTRL_FW_UNICAST_EN(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 7, 1, __Value)
#define SET_wMBOX1CMD_REMOTE_WAKE_CTRL_P2P_OFFLAD_EN(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 1, __Value)
#define SET_wMBOX1CMD_REMOTE_WAKE_CTRL_NBNS_FILTER_EN(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 2, 1, __Value)
#define SET_wMBOX1CMD_REMOTE_WAKE_CTRL_ARP_ACTION(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 1, __Value)
#define SET_wMBOX1CMD_REMOTE_WAKE_CTRL_FW_PARSING_UNTIL_WAKEUP(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 4, 1, __Value)

#define SET_wMBOX1CMD_AOAC_GLOBAL_INFO_PAIRWISE_ENC_ALG(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_wMBOX1CMD_AOAC_GLOBAL_INFO_GROUP_ENC_ALG(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 8, __Value)

#define SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_REMOTE_WAKE_CTRL_INFO(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd), 0, 8, __Value)
#define SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_ARP_RSP(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 8, __Value)
#define SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_NEIGHBOR_ADV(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_GTK_RSP(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 0, 8, __Value)
#define SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_GTK_INFO(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+4, 0, 8, __Value)

#ifdef CONFIG_P2P_WOWLAN
#define SET_wMBOX1CMD_RSVDPAGE_LOC_P2P_BCN(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_P2P_PROBE_RSP(__pwMBOX1Cmd, __Value)				SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_P2P_NEGO_RSP(__pwMBOX1Cmd, __Value)			SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_P2P_INVITE_RSP(__pwMBOX1Cmd, __Value)		SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_P2P_PD_RSP(__pwMBOX1Cmd, __Value)	SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+4, 0, 8, __Value)
#endif

typedef struct _RSVDPAGE_LOC {
	u8 LocProbeRsp;
	u8 LocPsPoll;
	u8 LocNullData;
	u8 LocQosNull;
	u8 LocBTQosNull;
#ifdef CONFIG_WOWLAN
	u8 LocRemoteCtrlInfo;
	u8 LocArpRsp;
	u8 LocNbrAdv;
	u8 LocGTKRsp;
	u8 LocGTKInfo;
	u8 LocProbeReq;
	u8 LocNetList;
#endif
	u8 LocApOffloadBCN;
#ifdef CONFIG_P2P_WOWLAN
	u8 LocP2PBeacon;
	u8 LocP2PProbeRsp;
	u8 LocNegoRsp;
	u8 LocInviteRsp;
	u8 LocPDRsp;
#endif
} RSVDPAGE_LOC, *PRSVDPAGE_LOC;

#endif
u8 wl_hal_set_fw_media_status_cmd(PNIC Nic, u8 mstatus, u8 macid);
#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
void Func_Of_Proc_Chip_Hw_Set_Fw_Wow_Related_Cmd(PNIC Nic, u8 enable);
#ifdef CONFIG_P2P_WOWLAN
u8 Func_Of_Proc_Chip_Hw_Set_Fwp2Prsvdpage_Cmd(PNIC Nic, PRSVDPAGE_LOC rsvdpageloc);
u8 Func_Of_Proc_Chip_Hw_Set_P2P_Wowlan_Offload_Cmd(PNIC Nic);
#endif
#endif
