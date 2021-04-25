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

#ifndef __DRV_CONF_H__
#define __DRV_CONF_H__
#include "cfg.h"
//#include "hal_ic_cfg.h"

#define RATE_ADAPTIVE_SUPPORT			0


#if defined(CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC) && !defined(CONFIG_USB_VENDOR_REQ_MUTEX)
#warning "define CONFIG_USB_VENDOR_REQ_MUTEX for CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC automatically"
#define CONFIG_USB_VENDOR_REQ_MUTEX
#endif
#if defined(CONFIG_VENDOR_REQ_RETRY) &&  !defined(CONFIG_USB_VENDOR_REQ_MUTEX)
#warning "define CONFIG_USB_VENDOR_REQ_MUTEX for CONFIG_VENDOR_REQ_RETRY automatically"
#define CONFIG_USB_VENDOR_REQ_MUTEX
#endif

#define DYNAMIC_CAMID_ALLOC

#define WL_SCAN_SPARSE_MIRACAST 1
#define WL_SCAN_SPARSE_BG 0

#ifndef CONFIG_WL_HIQ_FILTER
#define CONFIG_WL_HIQ_FILTER 1
#endif

#ifndef CONFIG_WL_ADAPTIVITY_MODE
#define CONFIG_WL_ADAPTIVITY_MODE 0
#endif

#ifndef CONFIG_WL_ADAPTIVITY_DML
#define CONFIG_WL_ADAPTIVITY_DML 0
#endif

#ifndef CONFIG_WL_ADAPTIVITY_DC_BACKOFF
#define CONFIG_WL_ADAPTIVITY_DC_BACKOFF 2
#endif

#ifndef CONFIG_WL_ADAPTIVITY_TH_L2H_INI
#define CONFIG_WL_ADAPTIVITY_TH_L2H_INI 0
#endif

#ifndef CONFIG_WL_ADAPTIVITY_TH_EDCCA_HL_DIFF
#define CONFIG_WL_ADAPTIVITY_TH_EDCCA_HL_DIFF 0
#endif

#ifndef CONFIG_WL_TARGET_TX_PWR_2G_A
#define CONFIG_WL_TARGET_TX_PWR_2G_A {-1, -1, -1}
#endif

#ifndef CONFIG_WL_AMPLIFIER_TYPE_2G
#define CONFIG_WL_AMPLIFIER_TYPE_2G 0
#endif

#ifndef CONFIG_WL_RFE_TYPE
#define CONFIG_WL_RFE_TYPE 64
#endif

#ifndef CONFIG_WL_GLNA_TYPE
#define CONFIG_WL_GLNA_TYPE 0
#endif

#ifndef CONFIG_WL_PLL_REF_CLK_SEL
#define CONFIG_WL_PLL_REF_CLK_SEL 0x0F
#endif

#define MACID_NUM_SW_LIMIT 32
#define SEC_CAM_ENT_NUM_SW_LIMIT 32

#endif
