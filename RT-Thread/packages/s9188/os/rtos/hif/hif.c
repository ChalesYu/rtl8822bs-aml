/*
 * hif.c
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

#include "wf_debug.h"
#include "common.h"
#include "hif.h"
#include "power.h"
#include "cfg_parse.h"
#include "tx_rtos.h"
#include "wlan_rtos.h"


#if 1
#define HIF_DBG(fmt, ...)       OS_LOG_D("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define HIF_DBG(fmt, ...)
#endif
#define HIF_INFO(fmt, ...)      OS_LOG_I("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HIF_WARN(fmt, ...)      OS_LOG_W("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HIF_ERROR(fmt, ...)     OS_LOG_E("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define RX_CMD_PARAM_LENGTH     8
#define TX_CMD_PARAM_LENGTH     12
#define TXDESC_OFFSET_NEW       20
#define TXDESC_PACK_LEN         4
#define RXDESC_OFFSET_NEW       16
#define RXDESC_PACK_LEN         4
#define TX_RX_REG_MAX_SIZE      28
#define FIRMWARE_BLOCK_SIZE     (512 -  TXDESC_OFFSET_NEW - TXDESC_PACK_LEN)

static char *wf_cfg = "./wifi.cfg";
static hif_mngent_t *ghif = NULL;
static OS_BOOL gmode_removed = wf_true;

#ifdef CONFIG_RICHV200
char *ifname = "";
#else
char *ifname = "";
#endif
char *if2name = "";


hif_mngent_t *hif_mngent_get(void)
{
    return ghif;
}

OS_BOOL hm_get_mod_removed(void)
{
    return gmode_removed;
}

os_list_t *hm_get_list_head(void)
{
    if (NULL != ghif)
    {
        return &(ghif->hif_usb_sdio);
    }

    HIF_ERROR("ghif is null");
    return 0;
}

static OS_RET hif_create_id(OS_U64 *id_map, OS_U8 *id)
{
    OS_U8 i = 0;
    int bit_mask = 0;

    for (i = 0; i < 64; i++)
    {
        bit_mask = OS_BIT(i);
        if (!(*id_map & bit_mask))
        {
            *id_map |= bit_mask;
            *id = i;
            return OS_ROK;
        }
    }

    return -OS_RERROR;
}

static OS_RET hif_destory_id(OS_U64 *id_map, OS_U8 id)
{
    if (id >= 64)
    {
        return -OS_RERROR;
    }

    *id_map &= ~ OS_BIT(id);
    return OS_ROK;
}


OS_U8 hm_new_id(OS_RET *err)
{
    OS_RET ret = 0;
    OS_U8  id = 0;

    if (NULL != ghif)
    {
        ret = hif_create_id(&ghif->id_bitmap, &id);
        if (err)
        {
            *err = ret;
        }
        return id;
    }

    return 0xff;
}

OS_U8 hm_del_id(OS_U8 id)
{
    return hif_destory_id(&ghif->id_bitmap, id);
}


OS_U8 hm_new_usb_id(OS_RET *err)
{
    OS_RET ret = 0;
    OS_U8  id  = 0;
    if (NULL != ghif)
    {
        ret = hif_create_id(&ghif->usb_id_bitmap, &id);
        if (err)
        {
            *err = ret;
        }
        return id;
    }
    return 0xff;
}

OS_U8 hm_del_usb_id(OS_U8 id)
{
    return hif_destory_id(&ghif->usb_id_bitmap, id);
}


OS_U8 hm_new_sdio_id(OS_RET *err)
{
    OS_RET ret = 0;
    OS_U8  id  = 0;
    if (NULL != ghif)
    {
        ret = hif_create_id(&ghif->sdio_id_bitmap, &id);
        if (err)
        {
            *err = ret;
        }

        return id;
    }

    return 0xff;
}

OS_U8 hm_del_sdio_id(OS_U8 id)
{
    return hif_destory_id(&ghif->sdio_id_bitmap, id);
}


wf_lock_t *hm_get_lock(void)
{
    if (!OS_WARN_ON(!ghif))
    {
        return &ghif->lock_mutex;
    }

    return NULL;
}


OS_U8 hm_set_num(HM_OPERATION op)
{
    if (!OS_WARN_ON(!ghif))
    {
        switch (op)
        {
            case HM_ADD :
                return ghif->hif_num++;

            case HM_SUB :
                return ghif->hif_num--;

            default :
                break;
        }
    }

    return 0;
}

OS_U8 hm_set_usb_num(HM_OPERATION op)
{
    if (!OS_WARN_ON(!ghif))
    {
        switch (op)
        {
            case HM_ADD :
                return ghif->usb_num++;

            case HM_SUB :
                return ghif->usb_num--;

            default :
                break;
        }
    }

    return 0;
}

OS_U8 hm_set_sdio_num(HM_OPERATION op)
{
    if (!OS_WARN_ON(!ghif))
    {
        switch (op)
        {
            case HM_ADD :
                return ghif->sdio_num++;

            case HM_SUB :
                return ghif->sdio_num--;

            default :
                break;
        }
    }

    return 0;
}


OS_STATIC_INLINE int hif_nic_tx_wake(nic_info_st *nic_info)
{
    tx_work_wake(nic_info->ndev);

    return 0;
}

OS_STATIC_INLINE void *get_fw_pos(OS_U32 pos)
{
    extern unsigned char fw_bin[];

    return (void *)&fw_bin[pos];
}

OS_RET hif_init(void)
{
    OS_U8 i;
    OS_U32 pos;
    fw_file_header_t *fw_file_head;
    fw_header_t *fw_head;
    OS_RET ret;

    HIF_DBG("\n\n    <WIFI DRV INSMOD>    \n\n");
    HIF_DBG("************HIF INIT*************");

    ghif = wf_kzalloc(sizeof(hif_mngent_t));
    if (OS_WARN_ON(!ghif))
    {
        return -OS_RNOMEM;
    }

#ifdef CONFIG_RICHV200
    HIF_DBG("prase richv200 firmware!");

    /* parase file head */
    pos = 0;
    fw_file_head = get_fw_pos(pos);
    if (fw_file_head->magic_number != 0xaffa ||
        fw_file_head->interface_type != 0x9188)
    {
        HIF_ERROR("firmware format error, magic:0x%x, type:0x%x",
                  fw_file_head->magic_number, fw_file_head->interface_type);
        wf_kfree(ghif);
        return -1;
    }
    ghif->fw_rom_type = fw_file_head->rom_type;
    pos += sizeof(*fw_file_head);

    /* parase fireware head */
    for (i = 0; i < fw_file_head->firmware_num; i++)
    {
        fw_head = get_fw_pos(pos);
        if (fw_head->type == 0)
        {
            HIF_DBG("FW0 Ver: %d.%d.%d.%d, size:%dBytes",
                    (fw_head->version >>  0) & 0xFF,
                    (fw_head->version >>  8) & 0xFF,
                    (fw_head->version >> 16) & 0xFF,
                    (fw_head->version >> 24) & 0xFF,
                    fw_head->length);
            ghif->fw0_size = fw_head->length;
            ghif->fw0 = get_fw_pos(fw_head->offset);
        }
        else
        {
            HIF_DBG("FW1 Ver: %d.%d.%d.%d, size:%dBytes",
                    (fw_head->version >>  0) & 0xFF,
                    (fw_head->version >>  8) & 0xFF,
                    (fw_head->version >> 16) & 0xFF,
                    (fw_head->version >> 24) & 0xFF,
                    fw_head->length);
            ghif->fw1_size = fw_head->length - 32;
            ghif->fw1 = get_fw_pos(fw_head->offset + 32);
        }
        pos += sizeof(*fw_head);
    }
#else
    HIF_DBG("prase richv100 firmware!");
    pos = 0;
    fw_file_head = get_fw_pos(pos);
    if ((fw_file_head.magic_number != 0xaffa) || (fw_file_head.interface_type != 0x9083))
    {
        HIF_ERROR("firmware format error, magic:0x%x, type:0x%x",
                  fw_file_head.magic_number, fw_file_head.interface_type);
        wf_kfree(ghif);
        return -1;
    }

    ghif->fw_rom_type = fw_file_head.rom_type;
    pos += sizeof(*fw_file_head);
    for (i = 0; i < fw_file_head.firmware_num; i++)
    {
        fw_head = get_fw_pos(pos);
        if (fw_head.type == 0)
        {
            HIF_DBG("FW0 Ver: %d.%d.%d.%d, size:%dBytes",
                    fw_head.version & 0xFF, (fw_head.version >> 8) & 0xFF,
                    (fw_head.version >> 16) & 0xFF, (fw_head.version >> 24) & 0xFF,
                    fw_head.length);
            ghif->fw0_size = fw_head->length;
            ghif->fw0 = get_fw_pos(fw_head->offset);
        }
        pos += sizeof(*fw_head);
    }
#endif

    os_list_init(&ghif->hif_usb_sdio);
    ghif->usb_num   = 0;
    ghif->sdio_num  = 0;
    ghif->hif_num   = 0;

    wf_lock_init(hm_get_lock(), WF_LOCK_TYPE_MUTEX);

    gmode_removed = wf_false;

#if defined(CONFIG_USB_FLAG)
    ret = usb_init();
    if (ret)
    {
        ndev_notifier_unregister();
        return ret;
    }
#elif defined(CONFIG_SDIO_FLAG)
    ret = sdio_init();
    if (ret)
    {
        return ret;
    }
#else
    ret = usb_init();
    if (ret)
    {
        ndev_notifier_unregister();
        return ret;
    }

    ret = sdio_init();
    if (ret)
    {
        ndev_notifier_unregister();
        return ret;
    }
#endif

    return OS_ROK;
}

void hif_exit(void)
{
    HIF_INFO("start");
    gmode_removed = wf_true;

    /* exit */
#if defined(CONFIG_USB_FLAG)
    usb_exit();
#elif defined(CONFIG_SDIO_FLAG)
    sdio_exit();
#else
    usb_exit();
    sdio_exit();
#endif

    HIF_INFO("ndev_notifier_unregister");

    if (ghif->cfg_content != NULL)
    {
        wf_kfree(ghif->cfg_content);
        ghif->cfg_content = NULL;
    }
    ghif->cfg_size = 0;

    ghif->fw0_size = 0;
    ghif->fw1_size = 0;
    if (ghif->fw0)
    {
        wf_kfree(ghif->fw0);
    }
    if (ghif->fw1)
    {
        wf_kfree(ghif->fw1);
    }

    wf_lock_term(hm_get_lock());
    wf_kfree(ghif);
    ghif = NULL;

    HIF_INFO("end");
}

static OS_RET hif_add_nic(hif_node_t *hif_info, int num)
{
    hif_mngent_t *phif_mngent = hif_mngent_get();
    nic_info_st *pnic_info;

    pnic_info = wf_kzalloc(sizeof(nic_info_st));
    if (OS_WARN_ON(!pnic_info))
    {
        return -OS_RNOMEM;
    }

    pnic_info->hif_node     = hif_info;
    pnic_info->hif_node_id  = hif_info->node_id;
    pnic_info->ndev_id      = num;
    pnic_info->is_up        = wf_false;
    pnic_info->virNic       = (wf_bool)!!num;

    pnic_info->nic_type = (hif_info->hif_type == HIF_USB) ? NIC_USB : NIC_SDIO;

    pnic_info->fwdl_info.fw_rom_type    = phif_mngent->fw_rom_type;
    pnic_info->fwdl_info.fw0            = phif_mngent->fw0;
    pnic_info->fwdl_info.fw0_size       = phif_mngent->fw0_size;
    pnic_info->fwdl_info.fw1            = phif_mngent->fw1;
    pnic_info->fwdl_info.fw1_size       = phif_mngent->fw1_size;

    pnic_info->func_check_flag      = 0xAA55BB66;
    pnic_info->nic_read             = hif_io_read;
    pnic_info->nic_write            = hif_io_write;
    pnic_info->nic_tx_queue_insert  = hif_info->ops->hif_tx_queue_insert;
    pnic_info->nic_tx_queue_empty   = hif_info->ops->hif_tx_queue_empty;
#ifdef CONFIG_RICHV200
    pnic_info->nic_write_fw         = hif_write_firmware;
    pnic_info->nic_write_cmd        = hif_write_cmd;
#endif
    pnic_info->nic_cfg_file_read    = NULL,//wf_cfg_file_parse;
               pnic_info->nic_tx_wake          = hif_nic_tx_wake;

    pnic_info->mlme_hw_access_lock  = &hif_info->mlme_hw_access_lock;
    pnic_info->mcu_hw_access_lock   = &hif_info->mcu_hw_access_lock;

    hif_info->nic_number++;
    pnic_info->nic_num = num;

    pnic_info->buddy_nic = NULL;
    if (hif_info->nic_number == 2)
    {
        /*for buddy*/
        hif_info->nic_info[0]->buddy_nic = hif_info->nic_info[1];
        hif_info->nic_info[1]->buddy_nic = hif_info->nic_info[0];
    }

    hif_info->nic_info[num] = pnic_info;

    return 0;
}

OS_STATIC_INLINE void hif_hw_access_init (hif_node_t *hif_info)
{
    /* mlme hardware access hw lock */
    wf_lock_init(&hif_info->mlme_hw_access_lock, WF_LOCK_TYPE_MUTEX);

    /* mcu hardware access hw lock */
    wf_lock_init(&hif_info->mcu_hw_access_lock, WF_LOCK_TYPE_MUTEX);
}


OS_RET hif_dev_insert(hif_node_t *hif_info)
{
    int i       = 0;
    int nic_num = 0;
    OS_RET ret  = OS_ROK;

    hif_info->dev_removed = wf_false;

    /* power on */
    HIF_DBG("************HIF DEV INSERT*************");
    HIF_DBG("<< Power on >>");
    if (power_on(hif_info) < 0)
    {
        HIF_ERROR("===>power_on error, exit!!");
        return -OS_RERROR;
    }
    else
    {
        HIF_DBG("power_on success");
#ifdef CONFIG_RICHV200
        side_road_cfg(hif_info);
#endif
        if (HIF_SDIO == hif_info->hif_type)
        {
            // cmd53 is ok, next for side-road configue
#ifdef CONFIG_SDIO_FLAG
            wf_sdioh_config(hif_info);
            wf_sdioh_interrupt_enable(hif_info);
#endif
        }
    }

    /*create hif trx func*/
    HIF_DBG("<< create hif tx/rx queue >>");
    wf_data_queue_mngt_init(hif_info);

#ifdef CONFIG_RICHV200
    ret = wf_hif_bulk_enable(hif_info);
    if (ret)
    {
        HIF_ERROR("wf_hif_bulk_enable failed");
        return -OS_RERROR;
    }
#endif

    ret = wf_hif_queue_enable(hif_info);
    if (ret)
    {
        HIF_ERROR("wf_hif_queue_enable failed");
        return -OS_RERROR;
    }

    /* ndev reg */
    HIF_DBG("<< add nic to hif_node >>");
    HIF_DBG("   node_id    :%d", hif_info->node_id);
    HIF_DBG("   hif_type   :%d  [1:usb  2:sdio]", hif_info->hif_type);

#ifdef CONFIG_STA_AND_AP_MODE
    nic_num = 2;
#else
    nic_num = 1;
#endif

    hif_hw_access_init(hif_info);

    for (i = 0; i < nic_num; i++)
    {
        ret = hif_add_nic(hif_info, i);
        if (ret != 0)
        {
            HIF_ERROR("ndev_register nic[%d] failed", i);
            return -OS_RERROR;
        }

        HIF_DBG("<< ndev register %d>>", i);
        {
            char name[] = "wlan0_s0";
            os_sprintf(name, "wlan%d_%c%d",
                       hif_info->node_id,
                       hif_info->hif_type == HIF_SDIO ? 's' : 'u',
                       i);
            ret = wlan_register(hif_info->nic_info[i], (const char *)name);
            if (ret)
            {
                HIF_ERROR("nic[%d] failed", i);
                return -OS_RERROR;
            }
        }

        {
            wlan_dev_t *wdev = hif_info->nic_info[i]->ndev;
            OS_RET ret;

            ret = wlan_dev_init(wdev);
            if (ret)
            {
                return -OS_RERROR;
            }
            ret = wlan_dev_open(wdev);
            if (ret)
            {
                return -OS_RERROR;
            }
            {
                wlan_priv_t *wdev_priv = wlan_dev_priv(wdev);
                nic_info_st *pnic_info = wdev_priv->nic;
                wf_mlme_scan_start(pnic_info, SCAN_TYPE_PASSIVE,
                                   NULL, 0, NULL, 0, 0);
            }
        }
    }

    return OS_ROK;
}


void hif_dev_removed(hif_node_t *hif_info)
{
    nic_info_st *nic_info   = NULL;
    int nic_cnt             = 0;

    hif_info->dev_removed = wf_true;

    HIF_DBG("************HIF DEV REMOVE [NODE:%d TYPE:%d]*************",
            hif_info->node_id, hif_info->hif_type);
#ifdef CONFIG_RICHV200
#if 0
    wf_mcu_reset_chip(hif_info->nic_info[0]);
    if (hif_info->cmd_completion_flag)
    {
        //wf_mdelay(HIF_BULK_MSG_TIMEOUT+10);
        HIF_DBG("<< wf_hif_bulk_cmd_post_exit >>");
        wf_hif_bulk_cmd_post_exit(hif_info);
    }
#else
    wf_hif_bulk_cmd_post_exit(hif_info);
#endif
#endif

    /* ndev unreg */
    HIF_DBG("<< ndev unregister >>");
    for (nic_cnt = 0; nic_cnt < MAX_NIC; nic_cnt++)
    {
        nic_info = hif_info->nic_info[nic_cnt];
        if (nic_info)
        {
            HIF_DBG("nic_id    :%d", nic_info->ndev_id);
            nic_info->is_surprise_removed = (wf_bool)hif_info->dev_removed;
            if (wf_false == nic_info->is_init_commplete)
            {
                nic_term(nic_info);
            }
            wlan_unregister(nic_info);
            if (0 == nic_cnt) //for concurrent mode
            {
                int i = 0;
                for (i = 1; i < MAX_NIC; i++)
                {
                    if (hif_info->nic_info[i])
                    {
                        hif_info->nic_info[i]->buddy_nic = NULL;
                    }
                }
            }

            wf_kfree(nic_info);
            hif_info->nic_info[nic_cnt] = NULL;
        }
        nic_info = NULL;
    }

    if (HIF_SDIO == hif_info->hif_type)
    {
#ifndef CONFIG_USB_FLAG
        wf_sdioh_interrupt_disable(hif_info);
#endif
    }
    wf_hif_queue_disable(hif_info);

#ifdef CONFIG_RICHV200
    wf_hif_bulk_disable(hif_info);
#endif

    /* term hif trx mode */
    HIF_DBG("<< term hif tx/rx queue >>");
    wf_data_queue_mngt_term(hif_info);

    /* power off */
    HIF_DBG("<< Power off >>");

    if (power_off(hif_info) < 0)
    {
        HIF_ERROR("power_off failed");
    }
}


void hif_node_register(hif_node_t **node, hif_enum type, hif_node_ops_t *ops)
{
    hif_node_t  *hif_node = NULL;

    hif_node = wf_kzalloc(sizeof(hif_node_t));
    if (NULL == hif_node)
    {
        HIF_ERROR("wf_kzalloc for hif_node failed");
        return;
    }

    hif_node->hif_type      = type;
    hif_node->node_id       = hm_new_id(NULL);
    hif_node->ops           = ops;

    wf_lock_lock(hm_get_lock());
    os_list_insert_tail(&hif_node->next, hm_get_list_head());

    if (HIF_SDIO == hif_node->hif_type)
    {
        hm_set_sdio_num(HM_ADD);
    }
    else if (HIF_USB == hif_node->hif_type)
    {
        hm_set_usb_num(HM_ADD);
    }

    hm_set_num(HM_ADD);

    wf_lock_unlock(hm_get_lock());

    *node = hif_node;
}

void hif_node_unregister(hif_node_t *pnode)
{
    os_list_t *tmp      = NULL;
    os_list_t *next     = NULL;
    hif_node_t *node    = NULL;
    OS_RET ret          = 0;

    HIF_INFO("start");
    wf_lock_lock(hm_get_lock());
    wf_list_for_each_safe(tmp, next, hm_get_list_head())
    {
        node = wf_list_entry(tmp, hif_node_t, next);
        if (NULL != node  && pnode == node)
        {
            os_list_delete(&node->next);
            hm_set_num(HM_SUB);
            if (HIF_USB == node->hif_type)
            {
                hm_set_usb_num(HM_SUB);
            }
            else if (HIF_SDIO == node->hif_type)
            {
                hm_set_sdio_num(HM_SUB);
            }
            break;
        }
    }

    if (NULL != node)
    {
        ret = hm_del_id(node->node_id);
        if (ret)
        {
            HIF_ERROR("hm_del_id [%d] failed", node->node_id);
        }
        wf_kfree(node);
        node = NULL;
    }

    wf_lock_unlock(hm_get_lock());
    HIF_INFO("end");
}

#ifdef CONFIG_RICHV200
static void io_txdesc_chksum(OS_U8 *ptx_desc)
{
    wf_u16 *usPtr = (wf_u16 *) ptx_desc;
    wf_u32 index;
    wf_u16 checksum = 0;

    for (index = 0; index < 9; index++)
    {
        checksum ^= le16_to_cpu(*(usPtr + index));
    }

    wf_set_bits_to_le_u32(ptx_desc + 16, 16, 16, checksum);
}

static wf_u16 io_firmware_chksum(OS_U8 *firmware, wf_u32 len)
{
    wf_u32 loop;
    wf_u16 *u16Ptr = (wf_u16 *) firmware;
    wf_u32 index;
    wf_u16 checksum = 0;

    loop = len / 2;
    for (index = 0; index < loop; index++)
    {
        checksum ^= le16_to_cpu(*(u16Ptr + index));
    }

    return checksum;
}

OS_RET wf_hif_bulk_enable(hif_node_t *hif_node)
{
    hif_node->reg_buffer = wf_kzalloc(512);
    if (NULL == hif_node->reg_buffer)
    {
        HIF_ERROR("no memmory for hif reg buffer");
        return -OS_RERROR;
    }
    hif_node->fw_buffer = wf_kzalloc(512);
    if (NULL == hif_node->fw_buffer)
    {
        HIF_ERROR("no memmory for hif fw buffer");
        return -OS_RERROR;
    }
    hif_node->cmd_snd_buffer = wf_kzalloc(512);
    if (NULL == hif_node->cmd_snd_buffer)
    {
        HIF_ERROR("no memmory for hif cmd buffer");
        return -OS_RERROR;
    }

    hif_node->cmd_rcv_buffer = wf_kzalloc(512);
    if (NULL == hif_node->cmd_rcv_buffer)
    {
        HIF_ERROR("no memmory for hif cmd buffer");
        return -OS_RERROR;
    }

    wf_lock_init(&hif_node->reg_mutex, WF_LOCK_TYPE_MUTEX);
    osCompletionInit(&hif_node->reg_completion);
    osCompletionInit(&hif_node->fw_completion);
    osCompletionInit(&hif_node->cmd_completion);

    hif_node->reg_size = 0;
    hif_node->fw_size = 0;
    hif_node->cmd_size = 0;
    hif_node->bulk_enable = wf_true;

    return OS_ROK;
}

OS_RET wf_hif_bulk_disable(hif_node_t *hif_node)
{
    if (NULL != hif_node->reg_buffer)
    {
        wf_kfree(hif_node->reg_buffer);
    }
    if (NULL != hif_node->fw_buffer)
    {
        wf_kfree(hif_node->fw_buffer);
    }
    if (NULL != hif_node->cmd_snd_buffer)
    {
        wf_kfree(hif_node->cmd_snd_buffer);
    }
    if (NULL != hif_node->cmd_rcv_buffer)
    {
        wf_kfree(hif_node->cmd_rcv_buffer);
    }
    hif_node->reg_size = 0;
    hif_node->fw_size = 0;
    hif_node->cmd_size = 0;

    return OS_ROK;
}

void wf_hif_bulk_reg_init(hif_node_t *hif_node)
{
    osCompletionInit(&hif_node->reg_completion);
    hif_node->reg_size = 0;
}

OS_RET wf_hif_bulk_reg_wait(hif_node_t *hif_node, wf_u32 timeout)
{
    return osCompletionWaitForTimeout(&hif_node->reg_completion, (timeout * WF_HZ) / 1000);
}

void wf_hif_bulk_reg_post(hif_node_t *hif_node, OS_U8 *buff, wf_u16 len)
{
    if (len <= 512)
    {
        wf_memcpy(hif_node->reg_buffer, buff, len);
        hif_node->reg_size = len;
        osComplete(&hif_node->reg_completion);
    }
}

void wf_hif_bulk_fw_init(hif_node_t *hif_node)
{
    osCompletionInit(&hif_node->fw_completion);
    hif_node->fw_size = 0;
}

OS_RET wf_hif_bulk_fw_wait(hif_node_t *hif_node, wf_u32 timeout)
{
    return osCompletionWaitForTimeout(&hif_node->fw_completion, (timeout * WF_HZ) / 1000);
}

void wf_hif_bulk_fw_post(hif_node_t *hif_node, OS_U8 *buff, wf_u16 len)
{
    if (len <= 512)
    {
        wf_memcpy(hif_node->fw_buffer, buff, len);
        hif_node->fw_size = len;
        osComplete(&hif_node->fw_completion);
    }
}

void wf_hif_bulk_cmd_init(hif_node_t *hif_node)
{
    osCompletionInit(&hif_node->cmd_completion);
    hif_node->cmd_size = 0;
}

OS_RET wf_hif_bulk_cmd_wait(hif_node_t *hif_node, wf_u32 timeout)
{
    hif_node->cmd_completion_flag = 1;
    hif_node->cmd_remove = 0;
    return osCompletionWaitForTimeout(&hif_node->cmd_completion, (timeout * WF_HZ) / 1000);
}

void wf_hif_bulk_cmd_post(hif_node_t *hif_node, OS_U8 *buff, wf_u16 len)
{
    if (len <= 512)
    {
        wf_memcpy(hif_node->cmd_rcv_buffer, buff, len);
        hif_node->cmd_size = len;
        hif_node->cmd_completion_flag = 0;
        hif_node->cmd_remove = 0;
        osComplete(&hif_node->cmd_completion);
    }
}

void wf_hif_bulk_cmd_post_exit(hif_node_t *hif_node)
{
    hif_node->cmd_completion_flag = 0;
    hif_node->cmd_remove = 1;
    if (wf_true == hif_node->bulk_enable)
    {
        osComplete(&hif_node->cmd_completion);
    }
}

int wf_hif_bulk_rxd_type(OS_U8 *prx_desc)
{
    OS_U8 u8Value;

    u8Value = wf_le_u8_read(prx_desc);

    return u8Value & 0x03;
}


OS_RET hif_write_firmware(void *node, OS_U8 which,  OS_U8 *firmware, wf_u32 len)
{
    OS_U8  u8Value;
    wf_u16 i;
    wf_u16 checksum;
    wf_u16 u16Value;
    wf_u32 align_len;
    wf_u32 buffer_len;
    wf_u32 back_len;
    wf_u32 send_once;
    wf_u32 send_len;
    wf_u32 send_size;
    wf_u32 remain_size;
    wf_u32 block_num;
    OS_U8 *alloc_buffer;
    OS_U8 *use_buffer;
    OS_U8 *ptx_desc;
    OS_U8 *prx_desc;
    hif_node_t *hif_node = (hif_node_t *)node;

    if (hif_node->dev_removed == wf_true)
    {
        return -1;
    }

    align_len = ((len + 3) / 4) * 4;

    /* alloc mem for xmit */
    buffer_len = TXDESC_OFFSET_NEW + TXDESC_PACK_LEN + FIRMWARE_BLOCK_SIZE;
    HIF_DBG("firmware download length is %d", len);
    HIF_DBG("firmware download buffer size is %d", buffer_len);
    alloc_buffer = wf_kzalloc(buffer_len + 4);
    if (alloc_buffer == NULL)
    {
        HIF_ERROR("can't wf_kzalloc memmory for download firmware");
        return -1;
    }
    use_buffer = (OS_U8 *)WF_N_BYTE_ALIGMENT((SIZE_PTR) (alloc_buffer), 4);

    block_num = align_len / FIRMWARE_BLOCK_SIZE;
    if (align_len % FIRMWARE_BLOCK_SIZE)
    {
        block_num += 1;
    }
    else
    {
        align_len += 4;
        block_num += 1;
    }
    remain_size = align_len;
    send_size = 0;

    HIF_INFO("fwdownload block number is %d", block_num);
    wf_hif_bulk_fw_init(hif_node);

    for (i = 0; i < block_num; i++)
    {
        wf_memset(use_buffer, 0, buffer_len);
        ptx_desc = use_buffer;
        /* set for fw xmit */
        wf_set_bits_to_le_u32(ptx_desc, 0, 2, TYPE_FW);
        /* set for first packet */
        if (i == 0)
        {
            wf_set_bits_to_le_u32(ptx_desc, 11, 1, 1);
        }
        /* set for last packet */
        if (i == (block_num - 1))
        {
            wf_set_bits_to_le_u32(ptx_desc, 10, 1, 1);
        }
        /* set for which firmware */
        wf_set_bits_to_le_u32(ptx_desc, 12, 1, which);
        /* set for reg HWSEQ_EN */
        wf_set_bits_to_le_u32(ptx_desc, 18, 1, 1);
        /* set for pkt_len */
        if (remain_size > FIRMWARE_BLOCK_SIZE)
        {
            send_once = FIRMWARE_BLOCK_SIZE;
        }
        else
        {
            send_once = remain_size;
        }

        wf_memcpy(ptx_desc + TXDESC_OFFSET_NEW, firmware + send_size, send_once);

        send_len = TXDESC_OFFSET_NEW + send_once;
        /* set for firmware checksum */
        if (i == (block_num - 1))
        {
            checksum = io_firmware_chksum(firmware, align_len);
            HIF_INFO("cal checksum = %d", checksum);
            wf_set_bits_to_le_u32(ptx_desc + send_len, 0, 32, checksum);
            HIF_DBG("my checksum is 0x%04x, fw_len = %d", checksum, align_len);
            send_len += TXDESC_PACK_LEN;
            send_once += TXDESC_PACK_LEN;
        }
        wf_set_bits_to_le_u32(ptx_desc + 8, 0, 16, send_once);

        /* set for checksum */
        io_txdesc_chksum(ptx_desc);

        if (hif_io_write(hif_node, 2, CMD_QUEUE_INX, (char *)ptx_desc, send_len) < 0)
        {
            HIF_ERROR("bulk download firmware error");
            wf_kfree(alloc_buffer);
            return -1;
        }

        send_size += send_once;
        remain_size -= send_once;

        // delay for giving hardware handle time
        wf_mdelay(1);
    }

    if (wf_hif_bulk_fw_wait(hif_node, HIF_BULK_MSG_TIMEOUT) == 0)
    {
        HIF_ERROR("bulk access fw read timeout");
        wf_kfree(alloc_buffer);
        return -1;
    }

    prx_desc = use_buffer;
    back_len = RXDESC_OFFSET_NEW + RXDESC_PACK_LEN;
    if (hif_node->fw_size != back_len)
    {
        HIF_ERROR("bulk access fw read length error");
        wf_kfree(alloc_buffer);
        return -1;
    }

    wf_memcpy(prx_desc, hif_node->fw_buffer, hif_node->fw_size);

    u8Value = wf_le_u8_read(prx_desc);
    if ((u8Value & 0x03) != TYPE_FW)
    {
        HIF_ERROR("bulk download firmware type error by read back");
        wf_kfree(alloc_buffer);
        return -1;
    }
    u16Value = wf_le_u16_read(prx_desc + 4);
    u16Value &= 0x3FFF;
    if (u16Value != RXDESC_PACK_LEN)
    {
        HIF_ERROR("bulk download firmware length error, value: %d", u16Value);
        wf_kfree(alloc_buffer);
        return -1;
    }

    u8Value = wf_le_u8_read(prx_desc + 16);
    if (u8Value != 0x00)
    {
        HIF_ERROR("bulk download firmware status error");
        u16Value = wf_le_u16_read(prx_desc + 18);
        HIF_DBG("Read checksum is 0x%04x", u16Value);
        if (u8Value == 0x01)
        {
            HIF_ERROR("bulk download firmware txd checksum error");
        }
        else if (u8Value == 0x02)
        {
            HIF_ERROR("bulk download firmware fw checksum error");
        }
        else if (u8Value == 0x03)
        {
            HIF_ERROR("bulk download firmware fw & txd checksum error");
        }
        wf_kfree(alloc_buffer);
        return -1;
    }
    wf_kfree(alloc_buffer);

    if (which == FIRMWARE_M0)
    {
        HIF_INFO("bulk download m0 firmware ok");
    }
    else if (which == FIRMWARE_DSP)
    {
        HIF_INFO("bulk download dsp firmware ok");
    }

    return 0;
}


OS_RET hif_write_cmd(void *node, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len)
{
    OS_RET ret = 0;
    OS_U8  u8Value;
    wf_u16 u16Value;
    OS_U8 *ptx_desc;
    OS_U8 *prx_desc;
    wf_u16 snd_pktLen = 0;
    wf_u16 rcv_pktLen = 0;
    hif_node_t *hif_node = (hif_node_t *)node;

    if (hif_node->dev_removed == wf_true)
    {
        return -1;
    }

    ptx_desc = hif_node->cmd_snd_buffer;
    wf_memset(ptx_desc, 0, TXDESC_OFFSET_NEW + TX_CMD_PARAM_LENGTH);

    /* set for reg xmit */
    wf_set_bits_to_le_u32(ptx_desc, 0, 2, TYPE_CMD);
    /* set for cmd index */
    wf_set_bits_to_le_u32(ptx_desc, 2, 8, 0);
    /* set for reg HWSEQ_EN */
    wf_set_bits_to_le_u32(ptx_desc, 18, 1, 1);
    /* set SEQ  for test*/
    //wf_set_bits_to_le_u32(ptx_desc, 24, 8, __gmcu_cmd_count & 0xFF);
    /* set for pkt_len */
    wf_set_bits_to_le_u32(ptx_desc + 8, 0, 16, TX_CMD_PARAM_LENGTH + send_len * 4);
    /* set for checksum */
    io_txdesc_chksum(ptx_desc);

    /* set for  func_code */
    wf_set_bits_to_le_u32(ptx_desc + TXDESC_OFFSET_NEW, 0, 32, cmd);
    /* set for  len */
    wf_set_bits_to_le_u32(ptx_desc + TXDESC_OFFSET_NEW + 4, 0, 32, send_len);
    /* set for  offs */
    wf_set_bits_to_le_u32(ptx_desc + TXDESC_OFFSET_NEW + 8, 0, 32, recv_len);

    /* set for send content */
    if (send_len != 0)
    {
        wf_memcpy(ptx_desc + TXDESC_OFFSET_NEW + TX_CMD_PARAM_LENGTH, send_buf, send_len * 4);
    }

    snd_pktLen = TXDESC_OFFSET_NEW + TX_CMD_PARAM_LENGTH + send_len * 4;

    wf_hif_bulk_cmd_init(hif_node);
    ret = wf_tx_queue_insert(hif_node, 1, (char *)ptx_desc, snd_pktLen, wf_quary_addr(CMD_QUEUE_INX), NULL, NULL, NULL);
    if (ret != 0)
    {
        HIF_ERROR("bulk access cmd error by send");
        ret = -1;
        goto mcu_cmd_communicate_exit;
    }

    if (wf_hif_bulk_cmd_wait(hif_node, HIF_BULK_MSG_TIMEOUT) == 0)
    {
        HIF_ERROR("bulk access cmd read timeout");
        ret = -1;
        goto mcu_cmd_communicate_exit;
    }

    prx_desc = hif_node->cmd_rcv_buffer;
    rcv_pktLen = RXDESC_OFFSET_NEW + recv_len * 4 + TX_CMD_PARAM_LENGTH;
    if (hif_node->cmd_size != rcv_pktLen)
    {
        HIF_ERROR("mcu cmd: 0x%08X", cmd);
        HIF_ERROR("bulk access cmd read length error, recv cmd size is %d, but need pkt_len is %d", hif_node->cmd_size, rcv_pktLen);
        ret = -1;
        goto mcu_cmd_communicate_exit;
    }

    prx_desc = hif_node->cmd_rcv_buffer;
    u8Value = wf_le_u8_read(prx_desc);
    if ((u8Value & 0x03) != TYPE_CMD)
    {
        HIF_ERROR("bulk access cmd read error");
        ret = -1;
        goto mcu_cmd_communicate_exit;
    }
    u16Value = wf_le_u16_read(prx_desc + 4);
    u16Value &= 0x3FFF;
    if (u16Value != (recv_len * 4 + TX_CMD_PARAM_LENGTH))
    {
        HIF_ERROR("bulk access cmd read length error, value is %d, send cmd is 0x%x, cmd is 0x%x",
                  u16Value, cmd, *((wf_u32 *)prx_desc + RXDESC_OFFSET_NEW));

        ret = -1;
        goto mcu_cmd_communicate_exit;
    }

    if (recv_len != 0)
    {
        wf_memcpy(recv_buf, prx_desc + RXDESC_OFFSET_NEW + TX_CMD_PARAM_LENGTH, recv_len * 4);
    }

mcu_cmd_communicate_exit:

    return ret;
}

#endif

