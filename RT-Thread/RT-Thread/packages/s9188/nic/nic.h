/*
 * nic.h
 *
 * used for Initialization logic
 *
 * Author: songqiang
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __NIC_H__
#define __NIC_H__


typedef enum
{
    NIC_USB     = 1,
    NIC_SDIO    = 2,
} nic_type_e;

typedef struct
{
    wf_u8  fw_rom_type;
    wf_u32 fw0_size;
    wf_u32 fw1_size;
    char  *fw0;
    char  *fw1;
} fwdl_info_t;


typedef struct nic_info
{
    void *hif_node;
    int   hif_node_id;

    void *dev;

    void *ndev;
    wf_u8 ndev_id;
    wf_u8 ndev_num;

    void *pwiphy;
    void *pwidev;
    void *widev_priv;

    nic_type_e nic_type;
    wf_bool virNic;
    wf_bool is_surprise_removed;
    wf_bool is_driver_stopped;
    wf_bool is_up;
    wf_bool is_init_commplete;
    wf_os_api_sema_t cmd_sema;
    wf_u32  nic_state;
    fwdl_info_t fwdl_info;
    wf_u32 setband;

    void *wdn;
    void *hw_info;
    void *local_info;
    void *odm;
    void *mlme_info;
    void *scan_info;
    void *auth_info;
    void *sec_info;
    void *pwr_info;
    void *assoc_info;
    void *wlan_mgmt_info;
    void *sta_info;
    void *tx_info;
    void *rx_info;
    void *iwstats;
    void *mp_info;
    void *p2p;
    void *ars;
    void *adhoc_info;
    int func_check_flag;
    int (*nic_write)(void *node, unsigned char flag, unsigned int addr, char *data, int datalen);
    int (*nic_read)(void *node, unsigned char flag, unsigned int addr, char *data, int datalen);

    int (*nic_tx_queue_insert)(void *node,wf_u8 agg_num, char *buff, wf_u32 buff_len, wf_u32 addr,
                            int (*tx_callback_func)(void *tx_info, void *param), void *tx_info, void *param);
    int (*nic_tx_queue_empty)(void *node);

    int (*nic_write_fw)(void *node, wf_u8 which,  wf_u8 *firmware, wf_u32 len);

    int (*nic_write_cmd)(void *node, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len);

    int (*nic_cfg_file_read)(void *pnic_info);

    int (*nic_tx_wake)(struct nic_info *pnic_info);

    wf_u32 nic_num;
    void *buddy_nic;

    wf_u32 *wdn_id_bitmap;
    wf_lock_t *mlme_hw_access_lock;
    wf_lock_t *mcu_hw_access_lock;
    wf_u8 *hw_ch;
    wf_u8 *hw_bw;
    wf_u8 *hw_offset;

	wf_u32 agg_time;
	wf_u32 agg_size;
} nic_info_st;

int nic_init(nic_info_st *nic_info);
int nic_term(nic_info_st *nic_info);
int nic_enable(nic_info_st *nic_info);
int nic_disable(nic_info_st *nic_info);
int nic_suspend(nic_info_st *nic_info);
int nic_resume(nic_info_st *nic_info);
int nic_shutdown(nic_info_st *nic_info);
wf_u8 *nic_to_local_addr(nic_info_st *nic_info);
wf_inline static void nic_mlme_hw_access_lock (nic_info_st *pnic_info)
{
    if (pnic_info->mlme_hw_access_lock)
    {
        wf_lock_lock(pnic_info->mlme_hw_access_lock);
    }
}
wf_inline static int nic_mlme_hw_access_trylock (nic_info_st *pnic_info)
{
    if (pnic_info->mlme_hw_access_lock)
    {
        return wf_lock_trylock(pnic_info->mlme_hw_access_lock);
    }

    return 0;
}
wf_inline static void nic_mlme_hw_access_unlock (nic_info_st *pnic_info)
{
    if (pnic_info->mlme_hw_access_lock)
    {
        wf_lock_unlock(pnic_info->mlme_hw_access_lock);
    }
}

wf_inline static void nic_mcu_hw_access_lock (nic_info_st *pnic_info)
{
    if (pnic_info->mcu_hw_access_lock)
    {
        wf_lock_lock(pnic_info->mcu_hw_access_lock);
    }
}
wf_inline static int nic_mcu_hw_access_trylock (nic_info_st *pnic_info)
{
    if (pnic_info->mcu_hw_access_lock)
    {
        return wf_lock_trylock(pnic_info->mcu_hw_access_lock);
    }

    return 0;
}
wf_inline static void nic_mcu_hw_access_unlock (nic_info_st *pnic_info)
{
    if (pnic_info->mcu_hw_access_lock)
    {
        wf_lock_unlock(pnic_info->mcu_hw_access_lock);
    }
}


#define wf_is_surprise_removed(nic_info)    ((nic_info->is_surprise_removed) == wf_true)
#define wf_is_drv_stopped(nic_info)         ((nic_info->is_driver_stopped) == wf_true)

#define WF_CANNOT_RUN(nic_info)     (wf_is_surprise_removed(nic_info) || wf_is_drv_stopped(nic_info))


#endif

