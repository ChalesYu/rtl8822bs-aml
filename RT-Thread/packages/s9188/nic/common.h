/*
 * common.h
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
#ifndef __COMMON_H__
#define __COMMON_H__


// os porting define
#include "wf_os_api.h"

// configure
#include "wf_config.h"

// public define
#include "wf_mix.h"
#include "wf_pt.h"
#include "wf_list.h"
#include "wf_timer.h"
#include "wf_que.h"
#include "wf_msg.h"
#include "queue.h"

// nic info
#include "nic.h"
#include "nic_io.h"

// func module
#include "wf_80211.h"
#include "efuse.h"
#include "hw_info.h"
#include "hw_ctrl.h"
#include "local_config.h"
#ifdef CFG_ENABLE_AP_MODE
#include "ap.h"
#endif
#include "wlan_mgmt.h"
#include "sec.h"
#include "wdn.h"
#include "tx.h"
#include "rx.h"
#include "fw_download.h"
#include "mcu_cmd.h"

#if  defined (CONFIG_ARS_FIRMWARE_SUPPORT)
//#include "odm.h"
#endif

#if  defined (CONFIG_ARS_DRIVER_SUPPORT)
#include "ars_entry.h"
#endif

#include "scan.h"
#include "auth.h"
#include "assoc.h"
#include "mlme.h"
#include "ie.h"
#include "action.h"
#include "lps.h"
#include "adhoc.h"
#include "p2p.h"

#endif

