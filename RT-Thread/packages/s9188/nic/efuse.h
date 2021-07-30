/*
 * efuse.h
 *
 * used for read efuse value
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
#ifndef __EFUSE_H__
#define __EFUSE_H__

typedef enum WLAN__EEPORM_CODE 
{
	EFUSE_PHYSPACE = 0,
	WLAN_EEPORM_MAC = 1,
	EFUSE_VID = 2,
	EFUSE_PID = 3,
	EFUSE_MANU = 4,
	EFUSE_PRODUCT = 5,
	EFUSE_FREQCAL = 6,
	EFUSE_TEMPCAL = 7,
	EFUSE_CHANNELPLAN = 8,
	EFUSE_POWERCAL = 9,
	EFUSE_HEADERCHECK = 10,
	EFUSE_FIXDVALUE = 11,
	WLAN_EEPORM_BASEVALUE2 = 12,
	EFUSE_PHYCFGCHECK = 13,
	EFUSE_GROUP_0=14,
	EFUSE_GROUP_1=15,
	EFUSE_GROUP_2=16,
	EFUSE_GROUP_3=17,
	EFUSE_GROUP_4=18,
#ifdef CONFIG_FIX_FREQ
	EFUSE_CUSTOMER1=19,
#endif
	WLAN_EEPORM_CODE_MAX
} EUSE_CODE;


int wf_mcu_efuse_get(nic_info_st *nic_info, EUSE_CODE efuse_code, wf_u32 *outdata, wf_u32 outdata_len);
#endif