/******************************************************************************
 *
 * Copyright(c) 2007 - 2014 Wf_wlan Corporation. All rights reserved.
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
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
/*
 * Public General Configure
 */
#define AUTOCONF_INCLUDED

#define WLT908621X_MODULE_NAME "9083H"
#define DRV_NAME "s9083h"


#define QSLT_BEACON						0x8
#define QSLT_HIGH						0x9
#define QSLT_MGNT						0xA

/*
 * Wi-Fi Functions Configure
 */
#define CONFIG_80211N_HT
#define CONFIG_RECV_REORDERING_CTRL

//#define CONFIG_AP_MODE
#ifdef CONFIG_AP_MODE
	#define CONFIG_NATIVEAP_MLME
	#ifndef CONFIG_NATIVEAP_MLME
		#define CONFIG_HOSTAPD_MLME
	#endif

	/* #define CONFIG_FIND_BEST_CHANNEL */
	#define CONFIG_TX_MCAST2UNI		/* Support IP multicast to unicast */
	/* #define CONFIG_AUTO_AP_MODE */
#endif

//#define CONFIG_P2P
#ifdef CONFIG_P2P
	#define CONFIG_WFD
	#define CONFIG_P2P_REMOVE_GROUP_INFO

	/* #define CONFIG_DBG_P2P */
	//#define CONFIG_P2P_PS
	#define CONFIG_P2P_OP_CHK_SOCIAL_CH
	#define CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT	/* replace CONFIG_P2P_CHK_INVITE_CH_LIST flag */
	#define CONFIG_P2P_INVITE_IOT
#endif

/* #define CONFIG_CONCURRENT_MODE *//* Set from Makefile */
#ifdef CONFIG_CONCURRENT_MODE
	#define CONFIG_TSF_RESET_OFFLOAD	/* For 2 PORT TSF SYNC. */
	/* #define CONFIG_HWPORT_SWAP   */
	#define CONFIG_RUNTIME_PORT_SWITCH
	/* #define DBG_RUNTIME_PORT_SWITCH */
	#define CONFIG_SCAN_BACKOP
#endif /* CONFIG_CONCURRENT_MODE */

//#define CONFIG_LAYER2_ROAMING
//#define CONFIG_LAYER2_ROAMING_RESUME

#define CONFIG_80211D

#define CONFIG_XMIT_ACK
#ifdef CONFIG_XMIT_ACK
	#define CONFIG_ACTIVE_KEEP_ALIVE_CHECK
#endif

#define CONFIG_wMBOX0_PACKET_EN
#define CONFIG_RF_POWER_TRIM
#define WL_NOTCH_FILTER 0		/* 0:Disable, 1:Enable, */

/*
 * Interface Related Configure
 */
//#define FPGA_TEST  //36
//#define CONFIG_USB_TX_AGGREGATION
#define CONFIG_USB_RX_AGGREGATION
#define USB_INTERFERENCE_ISSUE
#define CONFIG_GLOBAL_UI_PID
#define CONFIG_OUT_EP_WIFI_MODE
#define ENABLE_USB_DROP_INCORRECT_OUT
/* #define CONFIG_REDUCE_USB_TX_INT	*/

/*
 * CONFIG_USE_USB_BUFFER_ALLOC_XX uses Linux USB Buffer alloc API and is for Linux platform only now!
 */
/* #define CONFIG_USE_USB_BUFFER_ALLOC_TX	*//* Trade-off: For TX path, improve stability on some platforms, but may cause performance degrade on other platforms. */
/* #define CONFIG_USE_USB_BUFFER_ALLOC_RX	*//* For RX path */
#define CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC
#define CONFIG_USB_VENDOR_REQ_MUTEX
#define CONFIG_VENDOR_REQ_RETRY

/*
 * Others
 */
#define CONFIG_SKB_COPY			/* for amsdu */
#define CONFIG_NEW_SIGNAL_STAT_PROCESS
/* #define CONFIG_SIGNAL_DISPLAY_DBM *//* display RX signal with dbm */
#ifdef CONFIG_SIGNAL_DISPLAY_DBM
#endif

#define CONFIG_EMBEDDED_FWIMG
#define CONFIG_LONG_DELAY_ISSUE
#define CONFIG_ATTEMPT_TO_FIX_AP_BEACON_ERROR

/*
 * Auto Configure Section
 */
#ifdef CONFIG_MINIMAL_MEMORY_USAGE
	#undef CONFIG_USB_TX_AGGREGATION
	#undef CONFIG_USB_RX_AGGREGATION
#endif

#ifdef CONFIG_POWER_SAVING
	#define CONFIG_LPS
#endif /* CONFIG_POWER_SAVING */

#ifdef CONFIG_WOWLAN
	#define CONFIG_ARP_KEEP_ALIVE
#endif /* CONFIG_WOWLAN */


#ifdef CONFIG_AP_MODE
	/* #define CONFIG_INTERRUPT_BASED_TXBCN *//* Tx Beacon when driver BCN_OK ,BCN_ERR interrupt occurs */
	#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_INTERRUPT_BASED_TXBCN)
		#undef CONFIG_INTERRUPT_BASED_TXBCN
	#endif
	#ifdef CONFIG_INTERRUPT_BASED_TXBCN
		/* #define CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT */
		#define CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
	#endif
#endif /* CONFIG_AP_MODE */

#ifdef CONFIG_USE_USB_BUFFER_ALLOC_RX

#else
	#define CONFIG_PREALLOC_RECV_SKB
	#ifdef CONFIG_PREALLOC_RECV_SKB
		/* #define CONFIG_FIX_NR_BULKIN_BUFFER *//* only use PREALLOC_RECV_SKB buffer, don't alloc skb at runtime */
	#endif
#endif

#ifdef CONFIG_USB_TX_AGGREGATION
/* #define CONFIG_TX_EARLY_MODE */
#endif

/*#ifdef CONFIG_TX_EARLY_MODE
#define wlan9086X_EARLY_MODE_PKT_NUM_10	0
#endif*/

/*
 * Debug Related Configure
 */
//#define CONFIG_DEBUG			/* WL_INFO, etc... */
//#define CONFIG_DEBUG_WLT908621X
//#define CONFIG_PROC_DEBUG
//#define DBG_MEMORY_LEAK 1
//#define DBG_MEM_ALLOC

#define SETNEW
//#define SimpleTest
#define NEW_EEPROM_TXPWR_DEFAULT
#undef RENEW_EEPROM_THERMAL
//#define CHECK_ONLY11B  //only for jrx

//#define CONFIG_LOWMEM     //for test for jw
#define CONFIG_FORCE_241FFF   //for 0x24 at pwr_down 

// you can choose below define if you need.
//#define CONFIG_DIRECT_PWRCTL  //update efuse value(pwrcal+freq(ct,xt)+thermal) drv used, but not write efuse
//#define CONFIG_BW_20M  //force to turn to BW 20MHz, close 40MHz.
//#define CONFIG_RTSCTS      //force to open rts/cts , may cause tx throughput lower 
