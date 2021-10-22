/*
 * wf_config.h
 *
 * used for .....
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
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

// rx mem number, every mem size is 9K, because rx agg is enable.
#ifdef PKG_S9188_RX_MEM_NUM
#define HIF_QUEUE_ALLOC_MEM_NUM   PKG_S9188_RX_MEM_NUM
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
