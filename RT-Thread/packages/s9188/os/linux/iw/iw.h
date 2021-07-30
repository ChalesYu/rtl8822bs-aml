/*
 * iw.h
 *
 * used for wext 
 *
 * Author: houchuang
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
#ifndef __IW_H__
#define __IW_H__

#include "ndev_linux.h"


#define IW_PRV_IOCTL_START       (SIOCIWFIRSTPRIV)

#ifdef CONFIG_MP_MODE
#define IW_PRV_MP_GET            (SIOCIWFIRSTPRIV + 6)
#endif
#define IW_PRV_FW_DEBUG          (SIOCIWFIRSTPRIV + 0)
#define IW_PRV_READ_REG_TEST     (SIOCIWFIRSTPRIV + 1)
#define IW_PRV_WRITE_REG_TEST    (SIOCIWFIRSTPRIV + 2)
#define IW_PRV_ARS            	 (SIOCIWFIRSTPRIV + 3)
#define IW_PRV_TEST            	 (SIOCIWFIRSTPRIV + 4)
#define IW_PRV_TXAGG             (SIOCIWFIRSTPRIV + 5)


#define IW_IOC_HOSTAPD           (SIOCIWFIRSTPRIV + 28)
#define IW_IOC_WPA_SUPPLICANT    (SIOCIWFIRSTPRIV + 31)


enum {
	IW_PRV_WRITE_REG = 1,
	IW_PRV_READ_REG,
	IW_PRV_WRITE_RF,
	IW_PRV_READ_RF,
	IW_PRV_SET_MP,
	IW_PRV_MP_START,
	IW_PRV_MP_STOP,
	IW_PRV_MP_RATE,
	IW_PRV_MP_CHANNEL,
	IW_PRV_MP_BANDWIDTH,
	IW_PRV_MP_TXPOWER,
	IW_PRV_MP_ANT_TX,
	IW_PRV_MP_ANT_RX,
	IW_PRV_MP_CTX,
	IW_PRV_MP_QUERY,
	IW_PRV_MP_ARX,
	IW_PRV_MP_PSD,
	IW_PRV_MP_PWRTRK,
	IW_PRV_MP_THER,
	IW_PRV_MP_IOCTL,
	IW_PRV_EFUSE_GET_PHY,
	IW_PRV_EFUSE_SET_PHY,
	IW_PRV_GetFreqRegVal,
	IW_PRV_SetFreqRegVal,
	IW_PRV_SetTempCalVal,
	IW_PRV_GetTempCalVal,
	IW_PRV_MP_RESET_STATS,
	IW_PRV_MP_DUMP,
	IW_PRV_MP_PHYPARA,
	IW_PRV_MP_SetRFPathSwh,
	IW_PRV_MP_QueryDrvStats,
	IW_PRV_MP_SetBT,
	IW_PRV_CTA_TEST,
	IW_PRV_MP_DISABLE_BT_COEXIST,
	IW_PRV_MP_PwrCtlDM,
	IW_PRV_MP_GETVER,
	IW_PRV_MP_MON,
	IW_PRV_EFUSE_MASK,
	IW_PRV_EFUSE_FILE,
	IW_PRV_MP_TX,
	IW_PRV_MP_RX,
	IW_PRV_MP_HW_TX_MODE,
#ifdef CONFIG_WOWLAN
	IW_PRV_MP_WOW_ENABLE,
	IW_PRV_MP_WOW_SET_PATTERN,
#endif
#ifdef CONFIG_AP_WOWLAN
	IW_PRV_MP_AP_WOW_ENABLE,
#endif
	IW_PRV_MP_NULL,
	IW_PRV_MP_GET_TXPOWER_INX,

	IW_PRV_MP_SD_IREAD,
	IW_PRV_MP_SD_IWRITE,
	IW_PRV_MP_HW_SET_GI,
	IW_PRV_GET,
	IW_PRV_SET,
	IW_PRV_READ_BB,
	IW_PRV_WRITE_BB,
	IW_PRV_EFUSE_GET_LOGIC,
	IW_PRV_EFUSE_SET_LOGIC,
	IW_PRV_WRITE_PHY_EFUSE,
	IW_PRV_FW_INIT,
};

#endif
