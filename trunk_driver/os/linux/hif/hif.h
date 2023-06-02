/*
 * hif.h
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
#ifndef __HIF_H__
#define __HIF_H__
#include <linux/mutex.h>
#include <linux/completion.h>

#include "wf_list.h"
#include "sdio.h"
#include "usb.h"
#include "ndev_linux.h"
#include "hif_queue.h"

typedef enum hif_enum_
{
    HIF_USB     = 1,
    HIF_SDIO    = 2,
} hif_enum;

#define MAX_NIC 5

struct hif_node_;
typedef struct hif_node_ hif_node_st;

/*node operation*/
struct hif_node_ops
{
    int (*hif_write)(struct hif_node_ *node, unsigned char flag, unsigned int addr, char *data, int datalen);
    int (*hif_read)(struct hif_node_ *node, unsigned char flag, unsigned int addr, char *data, int datalen);
    int (*hif_show)(struct hif_node_ *node);
    int (*hif_init)(struct hif_node_ *node);
    int (*hif_exit)(struct hif_node_ *node);
    int (*hif_insert_netdev)(struct hif_node_ *node);
    int (*hif_remove_netdev)(struct hif_node_ *node);
    int (*hif_tx_queue_insert)(void *hif_info,wf_u8 agg_num, char *buff, wf_u32 buff_len, wf_u32 addr,
                               int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param);
    int (*hif_tx_queue_empty)(void *hif_info);
};


struct hif_node_
{
    wf_u8 node_id;
    wf_list_t next;
    hif_enum hif_type;//can be HIF_USB, HIF_SDIO
    union
    {
        hif_sdio_st   sdio;
        hif_usb_mngt  usb;
    } u;

    /*common part */
    struct hif_node_ops *ops;
    nic_info_st *nic_info[MAX_NIC];
    data_queue_mngt_st trx_pipe;
    wf_bool hif_tr_ctrl;
    wf_bool dev_removed;
    wf_u32 nic_number;

    void *odm;
    void *ars;
    /*proc debug system*/
    void *proc_info;

#ifdef CONFIG_RICHV200
    /* usb or sdio rx handle info */
#define HIF_BULK_MSG_TIMEOUT    5000
    struct mutex      reg_mutex;
    struct completion reg_completion;
    struct completion fw_completion;
    struct completion cmd_completion;
    wf_u8 *reg_buffer;
    wf_u8 *fw_buffer;
    wf_u8 *cmd_snd_buffer;
    wf_u8 *cmd_rcv_buffer;
    wf_u16 reg_size;
    wf_u16 fw_size;
    wf_u16 cmd_size;
    wf_u8 cmd_completion_flag;
    wf_u8 cmd_remove;
    wf_bool bulk_enable;
#endif

    wf_u32 wdn_id_bitmap;
    wf_lock_t mlme_hw_access_lock;
    wf_lock_t mcu_hw_access_lock;
    wf_u8 hw_ch;
    wf_u8 hw_bw;
    wf_u8 hw_offset;
};


typedef struct hif_mm_st_
{
    char *mem;
    unsigned int mem_sz; //mem size
    char is_free;
    wf_list_t mm_next;
} hif_mm_st;

typedef struct hif_management_
{
    wf_u8 usb_num; //usb number in hif_usb_sdio
    wf_u8 sdio_num; //sdio number in hif_usb_sdio
    wf_u8 hif_num; //all usb and sido number node in hif_usb_sdio, so hif_num = usb_num+sdio_num

    wf_u64 id_bitmap;
    wf_u64 usb_id_bitmap;
    wf_u64 sdio_id_bitmap;

    wf_u32  cfg_size;
    char   *cfg_content;

    wf_u8  fw_rom_type;
    wf_u32 fw0_size;
    wf_u32 fw1_size;
    char  *fw0;
    char  *fw1;

    wf_list_t hif_usb_sdio;
    wf_lock_t lock_mutex;

    /*common part*/
    wf_list_t mm_list;

} hif_mngent_st;


/*hm: hif management*/

typedef enum
{
    HM_ADD = 1,//usb_num,sdio_num and hif_num do add operation
    HM_SUB = 2 //usb_num,sdio_num and hif_num do subtraction operation
} HM_OPERATION;

wf_bool hm_get_mod_removed(void);

wf_list_t *hm_get_list_head(void);
wf_u8 hm_new_id(int *err);
wf_u8 hm_new_usb_id(int *err);
wf_u8 hm_new_sdio_id(int *err);
wf_u8 hm_del_id(wf_u8 id);
wf_u8 hm_del_usb_id(wf_u8 id);
wf_u8 hm_del_sdio_id(wf_u8 id);
wf_lock_t *hm_get_lock(void);

wf_u8 hm_set_num(HM_OPERATION op);
wf_u8 hm_set_usb_num(HM_OPERATION op);
wf_u8 hm_set_sdio_num(HM_OPERATION op);
hif_mngent_st * hif_mngent_get(void);


void hif_node_register(hif_node_st **node, hif_enum type, struct hif_node_ops *ops);
void hif_node_unregister(hif_node_st *node);


int  hif_dev_insert(hif_node_st *hif_info);
void hif_dev_removed(hif_node_st *hif_info);


int hif_io_write(void *node,  unsigned char flag, unsigned int addr, char *data, int datalen);
int hif_io_read(void *node,  unsigned char flag, unsigned int addr, char *data, int datalen);
int hif_io_enable(void *node);
int hif_io_disable(void *node);
unsigned char hif_io_read8(void *node, unsigned int addr,int *err);
unsigned long hif_io_read16(void *node, unsigned int addr,int *err);
unsigned int hif_io_read32(void *node, unsigned int addr,int *err);
int hif_io_write8(void *node, unsigned int addr, unsigned char value);
int hif_io_write16(void *node, unsigned int addr, unsigned long value);
int hif_io_write32(void *node, unsigned int addr, unsigned int value);

#ifdef MCU_CMD_TXD
wf_u8 wf_io_bulk_read8(void *hif, wf_u16 addr);
wf_u16 wf_io_bulk_read16(void *hif, wf_u16 addr);
wf_u32 wf_io_bulk_read32(void *hif, wf_u16 addr);
int wf_io_bulk_write8(void *hif, wf_u16 addr, wf_u8 value);
int wf_io_bulk_write16(void *hif, wf_u16 addr, wf_u16 value);
int wf_io_bulk_write32(void *hif, wf_u16 addr, wf_u32 value);
#endif

int wf_hif_queue_enable(hif_node_st *hif_node);
int wf_hif_queue_disable(hif_node_st *hif_node);

#ifdef CONFIG_RICHV200
int wf_hif_bulk_enable(hif_node_st *hif_node);
int wf_hif_bulk_disable(hif_node_st *hif_node);

void wf_hif_bulk_reg_init(hif_node_st *hif_node);
int wf_hif_bulk_reg_wait(hif_node_st *hif_node, wf_u32 timeout);
void wf_hif_bulk_reg_post(hif_node_st *hif_node, wf_u8 *buff, wf_u16 len);
void wf_hif_bulk_fw_init(hif_node_st *hif_node);
int wf_hif_bulk_fw_wait(hif_node_st *hif_node, wf_u32 timeout);
void wf_hif_bulk_fw_post(hif_node_st *hif_node, wf_u8 *buff, wf_u16 len);
void wf_hif_bulk_cmd_init(hif_node_st *hif_node);
int wf_hif_bulk_cmd_wait(hif_node_st *hif_node, wf_u32 timeout);
void wf_hif_bulk_cmd_post(hif_node_st *hif_node, wf_u8 *buff, wf_u16 len);
void wf_hif_bulk_cmd_post_exit(hif_node_st *hif_node);

int wf_hif_bulk_rxd_type(wf_u8 *prx_desc);

int hif_write_firmware(void *node, wf_u8 which,  wf_u8 *firmware, wf_u32 len);
int hif_write_cmd(void *node, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len);

#endif

void hif_tasklet_rx_handle(unsigned long data);
void hif_tasklet_tx_handle(unsigned long data);

#endif
