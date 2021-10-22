/*
 * wf_config.h
 *
 * used for .....
 *
 * Author: luozhi
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
#ifndef __WF_CONFIG_H__
#define __WF_CONFIG_H__

/*
all gl configuration should be placed there
*/

#define SURVEY_TO       (100)
#define AUTH_TIMEOUT    (500)
#define ASSOC_TIMEOUT   (300)
#define ADDBA_TO        (2000)
#define LINKED_TO       (1)

#define REAUTH_LIMIT    (2)
#define REASSOC_LIMIT   (4)
#define READDBA_LIMIT   (2)


#define CFG_SUPPORT_WAPI 1
#define CFG_SUPPORT_PASSPOINT   0

/*------------------------------------------------------------------------------
 * Flags of Wi-Fi Direct support
 *------------------------------------------------------------------------------
 */
#ifdef LINUX
#   ifdef CONFIG_X86
#define CFG_ENABLE_WIFI_DIRECT          1
#define CFG_SUPPORT_802_11W             1
#define CONFIG_SUPPORT_GTK_REKEY        1
#   else
#define CFG_ENABLE_WIFI_DIRECT          1
#define CFG_SUPPORT_802_11W             1   /*!< 0(default): Disable 802.11W */
#define CONFIG_SUPPORT_GTK_REKEY        1
#   endif
#else /* !LINUX */
#define CFG_ENABLE_WIFI_DIRECT           0
#define CFG_SUPPORT_802_11W              0  /* Not support at WinXP */
#endif /* LINUX */

#ifdef __RTOS__

#define WF_VERSION          "1.0.0"
#define CONFIG_RICHV200
#   if !defined(CONFIG_RICHV200) && !defined(CONFIG_RICHV100)
#   error: should select a IC version
#   endif

#define CONFIG_SDIO_FLAG
#   if !defined(CONFIG_USB_FLAG) && !defined(CONFIG_SDIO_FLAG)
#   error: should define a hif interface
#   endif

/* work mode, only can enable one */
//#define CFG_ENABLE_AP_MODE
//#define CFG_ENABLE_ADHOC_MODE
//#define CFG_ENABLE_MONITOR_MODE
//#define CONFIG_STA_AND_AP_MODE

#define MCU_CMD_MAILBOX

//#define CONFIG_ARS_FIRMWARE_SUPPORT

#define MAX_XMITBUF_SZ          2048
#define XMIT_DATA_BUFFER_CNT    8
// tx frame  number
#define NR_XMITFRAME            32

/* tx soft agg */
#define CONFIG_SOFT_TX_AGGREGATION
/* rx soft agg */
#define CONFIG_SOFT_RX_AGGREGATION

#endif


#define CFG_SUPPORT_802_11D              1

#define CONFIG_NATIVEAP_MLME
#ifndef CONFIG_NATIVEAP_MLME
#define CONFIG_HOSTAPD_MLME
#endif



#define MACID_NUM_SW_LIMIT 32
#define SEC_CAM_ENT_NUM_SW_LIMIT 32

#ifdef CONFIG_MP_INCLUDED
#define MP_DRIVER   1
#define CONFIG_MP_IWPRIV_SUPPORT
#else /* !CONFIG_MP_INCLUDED */
#define MP_DRIVER   0
#endif /* !CONFIG_MP_INCLUDED */

#define WF_CONFIG_P2P
#ifdef WF_CONFIG_P2P
#define CONFIG_WFD
#define CONFIG_P2P_OP_CHK_SOCIAL_CH
#define CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT /* replace CONFIG_P2P_CHK_INVITE_CH_LIST flag */
#define CONFIG_P2P_INVITE_IOT
#endif

#if (!defined(MCU_CMD_MAILBOX) && !defined(MCU_CMD_TXD))
//#define MCU_CMD_MAILBOX
#endif

#ifdef __RTTHREAD__
#include "rtconfig.h"

#define WF_VERSION            "1.0.0"

#ifdef PKG_S9188_FILE_FIRMWARE
#define WIFI_FW_DIR           PKG_S9188_FILE_FIRMWARE
#else
#define WIFI_FW_DIR           "/lib/firmware/fw_9188_r1751.bin"
#endif

#ifdef PKG_S9188_FILE_CFG
#define WIFI_CFG_DIR          PKG_S9188_FILE_CFG
#else
#define WIFI_CFG_DIR          "/etc/wifi.cfg"
#endif

#ifdef PKG_S9188_USING_SDIO
#define CONFIG_SDIO_FLAG
#endif
#ifdef PKG_S9188_USING_USB
#define CONFIG_USB_FLAG
#endif
#ifdef PKG_S9188_USING_BOTH
#define CONFIG_BOTH_FLAG
#endif
#if !defined(CONFIG_SDIO_FLAG) && !defined(CONFIG_USB_FLAG) && !defined(CONFIG_BOTH_FLAG)
#define CONFIG_SDIO_FLAG
#endif

/* work mode, only can enable one */
//#define CONFIG_STA_AND_AP_MODE
#ifdef PKG_S9188_MODE_STA
#define CFG_ENABLE_STA_MODE
#endif
#ifdef PKG_S9188_MODE_AP
#define CFG_ENABLE_AP_MODE
#endif
#if !defined(CFG_ENABLE_STA_MODE) && !defined(CFG_ENABLE_AP_MODE)
#define CFG_ENABLE_STA_MODE
#endif

#define MCU_CMD_MAILBOX
//#define MCU_CMD_TXD

#define CONFIG_ARS_FIRMWARE_SUPPORT
//#define CONFIG_ARS_DRIVER_SUPPORT

#define MAX_XMITBUF_SZ        (2048)

#ifdef PKG_S9188_XMIT_DATA_NUM
#define XMIT_DATA_BUFFER_CNT  PKG_S9188_XMIT_DATA_NUM
#else
#define XMIT_DATA_BUFFER_CNT  (8)
#endif

// tx frame  number
#ifdef PKG_S9188_XMIT_FRAME_NUM
#define NR_XMITFRAME           PKG_S9188_XMIT_FRAME_NUM
#else
#define NR_XMITFRAME           32
#endif

//open tx soft agg
#define CONFIG_SOFT_TX_AGGREGATION

//open rx soft agg
#define CONFIG_SOFT_RX_AGGREGATION

#if ((defined(CONFIG_STA_AND_AP_MODE) && defined(CFG_ENABLE_AP_MODE)) || \
     (defined(CONFIG_STA_AND_AP_MODE)&& defined(CFG_ENABLE_STA_MODE)) || \
     (defined(CFG_ENABLE_AP_MODE)&& defined(CFG_ENABLE_STA_MODE)))
error "wifi only one work mode can define"
#endif

#endif

#endif      /* END OF __WF_CONFIG_H__ */
