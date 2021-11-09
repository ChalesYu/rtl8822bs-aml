/*
 * proc.h
 *
 * used for print logs
 *
 * Author: pansiwei
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
#ifndef __PROC_H__
#define __PROC_H__

/* includes */

/* function declaration */
int wf_get_wlan_mgmt_info (nic_info_st *pnic_info);
int wf_get_mlme_info (nic_info_st *pnic_info);
int wf_get_rx_info (nic_info_st *pnic_info);
int wf_get_tx_info (nic_info_st *pnic_info);
int wf_get_ap_info (nic_info_st *pnic_info);
int wf_get_sta_info (nic_info_st *pnic_info);
int wf_get_hif_info (nic_info_st *pnic_info);
int wf_get_phy_info (nic_info_st *pnic_info);
int wf_get_mac_info (nic_info_st *pnic_info);
int wf_get_phy_cnt_reset (nic_info_st *pnic_info);

#endif /* __PROC_H__ */

