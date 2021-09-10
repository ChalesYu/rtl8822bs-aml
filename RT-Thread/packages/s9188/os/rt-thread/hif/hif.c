/*
 * hif.c
 *
 * hif, the driver init.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "common.h"
#include "hif.h"
#include "cfg_parse.h"
#include "pwr/power.h"
#include "wlan_dev/wf_wlan_dev.h"

#include "trx/tx_rtthread.h"
#include "trx/rx_rtthread.h"

#include "wf_debug.h"

#ifndef WIFI_CFG_DIR
#define WIFI_CFG_DIR  "/etc/wifi.cfg"
#endif

#ifndef WIFI_FW_DIR
#define WIFI_FW_DIR   "/lib/firmware/fw_s9083.bin"
#endif

#define CMD_PARAM_LENGTH_TX    12
#define CMD_PARAM_LENGTH_RX    8
#define TXDESC_OFFSET_NEW      20
#define TXDESC_PACK_LEN        4
#define RXDESC_OFFSET_NEW      16
#define RXDESC_PACK_LEN        4
#define TX_RX_REG_MAX_SIZE     28
#define FIRMWARE_BLOCK_SIZE    (512 -  TXDESC_OFFSET_NEW - TXDESC_PACK_LEN)

static hif_mngent_st *gl_hif = NULL;
static wf_bool gl_mode_removed = wf_true;

wf_bool hm_get_mod_removed(void)
{
  return gl_mode_removed;
}

wf_list_t *hm_get_list_head()
{
  if(NULL != gl_hif )
  {
    return &(gl_hif->hif_usb_sdio);
  }
  
  LOG_E("gl_hif is null");
  return 0;
}

static int hif_create_id (wf_u64 *id_map, wf_u8 *id)
{
  wf_u8 i = 0;
  int bit_mask = 0;
  for (i = 0; i < 64; i++)
  {
    bit_mask = BIT(i);
    if (!(*id_map & bit_mask))
    {
      *id_map |= bit_mask;
      *id = i;
      return WF_RETURN_OK;
    }
  }
  
  return WF_RETURN_FAIL;
}

static int hif_destory_id (wf_u64 *id_map, wf_u8  id)
{
  if (id >= 64)
  {
    return WF_RETURN_FAIL;
  }
  
  *id_map &= ~ BIT(id);
  return WF_RETURN_OK;
}

wf_u8 hm_new_id(int *err)
{
  int ret = 0;
  wf_u8  id  = 0;
  if(NULL != gl_hif )
  {
    ret = hif_create_id(&gl_hif->id_bitmap,&id);
    if(err)
    {
      *err = ret;
    }
    return id;
  }
  
  return 0xff;
}

wf_u8 hm_del_id(wf_u8 id)
{
  return hif_destory_id(&gl_hif->id_bitmap,id);
}


wf_u8 hm_new_usb_id(int *err)
{
  int ret = 0;
  wf_u8  id  = 0;
  if(NULL != gl_hif )
  {
    ret = hif_create_id(&gl_hif->usb_id_bitmap,&id);
    if(err)
    {
      *err = ret;
    }
    return id;
  }
  return 0xff;
}
wf_u8 hm_del_usb_id(wf_u8 id)
{
  return hif_destory_id(&gl_hif->usb_id_bitmap,id);
}

wf_u8 hm_new_sdio_id(int *err)
{
  int ret = 0;
  wf_u8  id  = 0;
  if(NULL != gl_hif )
  {
    ret = hif_create_id(&gl_hif->sdio_id_bitmap,&id);
    if(err)
    {
      *err = ret;
    }
    
    return id;
  }
  
  return 0xff;
}

wf_u8 hm_del_sdio_id(wf_u8 id)
{
  return hif_destory_id(&gl_hif->sdio_id_bitmap,id);
}


wf_lock_t *hm_get_lock(void)
{
  if(NULL != gl_hif )
  {
    return &gl_hif->lock_mutex;
  }
  
  LOG_E("gl_hif is null");
  return 0;
}


wf_u8 hm_set_num(HM_OPERATION op)
{
  if(NULL != gl_hif )
  {
    if( HM_ADD == op )
    {
      return gl_hif->hif_num++;
    }
    
    else if( HM_SUB == op )
    {
      return gl_hif->hif_num--;
    }
  }
  
  LOG_E("gl_hif is null");
  return 0;
}

wf_u8 hm_set_usb_num(HM_OPERATION op)
{
  if(NULL != gl_hif )
  {
    if( HM_ADD == op )
    {
      return gl_hif->usb_num++;
    }
    else if( HM_SUB == op )
    {
      return gl_hif->usb_num--;
    }
  }
  
  LOG_E("gl_hif is null");
  return 0;
}
wf_u8 hm_set_sdio_num(HM_OPERATION op)

{
  if(NULL != gl_hif )
  {
    if( HM_ADD == op )
    {
      return gl_hif->sdio_num++;
    }
    else if( HM_SUB == op )
    {
      return gl_hif->sdio_num--;
    }
  }
  
  LOG_E("gl_hif is null");
  return 0;
}

int hif_nic_tx_wake(nic_info_st *nic_info)
{
  tx_work_wake(nic_info->ndev);
  return 0;
}

hif_mngent_st *hif_mngent_get(void)
{
  return gl_hif;
}

int hif_init(void)
{
  int i;
  loff_t pos;
  wf_file *file = NULL;
  fw_file_header_t fw_file_head;
  fw_header_t fw_head;
  
  LOG_I("Driver Ver:%s", WF_VERSION);
  
  if(wf_false == gl_mode_removed) {
    LOG_W("SCI WIFI Driver has already init\r\n");
    return -1;
  }
  
  LOG_D("\n\n   <SCI WIFI DRV INIT>   \n\n");
  LOG_D("************HIF INIT*************");

  gl_hif = wf_kzalloc(sizeof(hif_mngent_st));
  if( NULL == gl_hif )
  {
    LOG_E("wf_kzalloc failed");
    return -1;
  }
  
  gl_hif->cfg_dir = WIFI_CFG_DIR;

#ifdef CONFIG_RICHV200
    LOG_D("prase richv200 firmware!");
    file = wf_os_api_file_open(WIFI_FW_DIR);
    if(file == NULL)
    {
        LOG_E("firmware open failed");
        wf_kfree(gl_hif);
        return -1;
    }
    pos = 0;
    wf_os_api_file_read(file, pos, (unsigned char *)&fw_file_head, sizeof(fw_file_head));
    if((fw_file_head.magic_number != 0xaffa) || (fw_file_head.interface_type != 0x9188))
    {
        LOG_E("firmware format error, magic:0x%x, type:0x%x",
              fw_file_head.magic_number, fw_file_head.interface_type);
        wf_os_api_file_close(file);
        wf_kfree(gl_hif);
        return -1;
    }

    gl_hif->fw_rom_type = fw_file_head.rom_type;
    pos += sizeof(fw_file_head);
    for(i=0; i<fw_file_head.firmware_num; i++)
    {
        wf_memset(&fw_head, 0, sizeof(fw_head));
        wf_os_api_file_read(file, pos, (unsigned char *)&fw_head, sizeof(fw_head));
        if(fw_head.type == 0)
        {
            LOG_D("FW0 Ver: %d.%d.%d.%d, size:%dBytes",
                  fw_head.version & 0xFF, (fw_head.version >> 8) & 0xFF,
                  (fw_head.version >> 16) & 0xFF,(fw_head.version >> 24) & 0xFF,
                  fw_head.length);
            gl_hif->fw0_size = fw_head.length;
            gl_hif->fw0 = wf_kzalloc(fw_head.length);
            if(NULL == gl_hif->fw0)
            {
                LOG_E("firmware0 wf_kzalloc failed");
                wf_os_api_file_close(file);
                wf_kfree(gl_hif);
                return -1;
            }
            wf_os_api_file_read(file, fw_head.offset, (unsigned char *)gl_hif->fw0, fw_head.length);
        }
        else
        {
            LOG_D("FW1 Ver: %d.%d.%d.%d, size:%dBytes",
                  fw_head.version & 0xFF, (fw_head.version >> 8) & 0xFF,
                  (fw_head.version >> 16) & 0xFF,(fw_head.version >> 24) & 0xFF,
                  fw_head.length);
            fw_head.length -= 32;
            gl_hif->fw1_size = fw_head.length;
            gl_hif->fw1 = wf_kzalloc(fw_head.length);
            if(NULL == gl_hif->fw1)
            {
                LOG_E("firmware1 wf_kzalloc failed");
                wf_os_api_file_close(file);
                wf_kfree(gl_hif);
                return -1;
            }
            wf_os_api_file_read(file, fw_head.offset + 32, (unsigned char *)gl_hif->fw1, fw_head.length);
        }
        pos += sizeof(fw_head);
    }
    wf_os_api_file_close(file);

#else
    LOG_D("prase richv100 firmware!");
    file = wf_os_api_file_open(WIFI_FW_DIR);
    if(file == NULL)
    {
        LOG_E("firmware open failed");
        wf_kfree(gl_hif);
        return -1;
    }
    pos = 0;
    wf_os_api_file_read(file, pos, (unsigned char *)&fw_file_head, sizeof(fw_file_head));
    if((fw_file_head.magic_number != 0xaffa) || (fw_file_head.interface_type != 0x9083))
    {
        LOG_E("firmware format error, magic:0x%x, type:0x%x",
              fw_file_head.magic_number, fw_file_head.interface_type);
        wf_os_api_file_close(file);
        wf_kfree(gl_hif);
        return -1;
    }

    gl_hif->fw_rom_type = fw_file_head.rom_type;
    pos += sizeof(fw_file_head);
    for(i=0; i<fw_file_head.firmware_num; i++)
    {
        wf_memset(&fw_head, 0, sizeof(fw_head));
        wf_os_api_file_read(file, pos, (unsigned char *)&fw_head, sizeof(fw_head));
        if(fw_head.type == 0)
        {
            LOG_D("FW0 Ver: %d.%d.%d.%d, size:%dBytes",
                  fw_head.version & 0xFF, (fw_head.version >> 8) & 0xFF,
                  (fw_head.version >> 16) & 0xFF,(fw_head.version >> 24) & 0xFF,
                  fw_head.length);
            gl_hif->fw0_size = fw_head.length;
            gl_hif->fw0 = wf_kzalloc(fw_head.length);
            if(NULL == gl_hif->fw0)
            {
                LOG_E("firmware0 wf_kzalloc failed");
                wf_os_api_file_close(file);
                wf_kfree(gl_hif);
                return -1;
            }
            wf_os_api_file_read(file, fw_head.offset, (unsigned char *)gl_hif->fw0, fw_head.length);
        }
        pos += sizeof(fw_head);
    }
    wf_os_api_file_close(file);
#endif
  
  wf_list_init(&gl_hif->hif_usb_sdio);
  gl_hif->usb_num     = 0;
  gl_hif->sdio_num    = 0;
  gl_hif->hif_num     = 0;
  
  wf_lock_init(hm_get_lock(),WF_LOCK_TYPE_MUTEX);
  
  gl_mode_removed = wf_false;
  
  return 0;
}

void hif_exit(void)
{
  LOG_I("[%s] start ",__func__);
  gl_mode_removed = wf_true;
  
  gl_hif->cfg_dir = NULL;
  
  gl_hif->fw0_size = 0;
  gl_hif->fw1_size = 0;
  if(gl_hif->fw0)
  {
    wf_kfree(gl_hif->fw0);
  }
  if(gl_hif->fw1)
  {
    wf_kfree(gl_hif->fw1);
  }
  
  wf_lock_term(hm_get_lock());
  wf_kfree(gl_hif);
  gl_hif = NULL;
}

static void hif_hw_access_init (hif_node_st *hif_info)
{
    /* mlme hardware access hw lock */
    wf_lock_init(&hif_info->mlme_hw_access_lock, WF_LOCK_TYPE_MUTEX);

    /* mcu hardware access hw lock */
    wf_lock_init(&hif_info->mcu_hw_access_lock, WF_LOCK_TYPE_MUTEX);
}

static wf_s32 hif_add_nic(hif_node_st *hif_info,int num)
{
    hif_mngent_st *hif_mngent = hif_mngent_get();
    struct rt_sdio_function *psdio_func_tmp = hif_info->u.sdio.func;
//    struct usb_interface *pusb_intf_tmp = hif_info->u.usb.pusb_intf;


    hif_info->nic_info[num] = wf_kzalloc(sizeof(nic_info_st));
    if (hif_info->nic_info[num] == NULL)
    {
        LOG_E("[hif_dev_insert] malloc nic_info failed");
        return -1;
    }

    hif_info->nic_info[num]->hif_node = hif_info;
    hif_info->nic_info[num]->hif_node_id = hif_info->node_id;
    hif_info->nic_info[num]->ndev_id = num;
    hif_info->nic_info[num]->is_up   = wf_false;
    hif_info->nic_info[num]->virNic = num?wf_true:wf_false;

    if(hif_info->hif_type == HIF_USB)
    {
        hif_info->nic_info[num]->nic_type = NIC_USB;
//        hif_info->nic_info[num]->dev = &pusb_intf_tmp->dev;
    }
    else
    {
        hif_info->nic_info[num]->nic_type = NIC_SDIO;
        hif_info->nic_info[num]->dev = &psdio_func_tmp->card;
    }
    hif_info->nic_info[num]->fwdl_info.fw_rom_type = hif_mngent->fw_rom_type;
    hif_info->nic_info[num]->fwdl_info.fw0 = hif_mngent->fw0;
    hif_info->nic_info[num]->fwdl_info.fw0_size = hif_mngent->fw0_size;
    hif_info->nic_info[num]->fwdl_info.fw1 = hif_mngent->fw1;
    hif_info->nic_info[num]->fwdl_info.fw1_size = hif_mngent->fw1_size;

    hif_info->nic_info[num]->func_check_flag = 0xAA55BB66;
    hif_info->nic_info[num]->nic_read = hif_io_read;
    hif_info->nic_info[num]->nic_write = hif_io_write;
    hif_info->nic_info[num]->nic_cfg_file_read = wf_cfg_file_parse;
    hif_info->nic_info[num]->nic_tx_queue_insert = hif_info->ops->hif_tx_queue_insert;
    hif_info->nic_info[num]->nic_tx_queue_empty =  hif_info->ops->hif_tx_queue_empty;
    hif_info->nic_info[num]->nic_tx_wake        =  hif_nic_tx_wake;

#ifdef CONFIG_RICHV200
    hif_info->nic_info[num]->nic_write_fw = hif_write_firmware;
    hif_info->nic_info[num]->nic_write_cmd = hif_write_cmd;
#endif

    hif_info->nic_info[num]->mlme_hw_access_lock = &hif_info->mlme_hw_access_lock;
    hif_info->nic_info[num]->mcu_hw_access_lock = &hif_info->mcu_hw_access_lock;

    hif_info->nic_number++;
    hif_info->nic_info[num]->nic_num = num;
    
    hif_info->nic_info[num]->buddy_nic = NULL;
    if (hif_info->nic_number == 2)
    {
      /*for buddy*/
      hif_info->nic_info[0]->buddy_nic = hif_info->nic_info[1];
      hif_info->nic_info[1]->buddy_nic = hif_info->nic_info[0];
    }

    return 0;
}

int hif_dev_insert(hif_node_st *hif_info)
{
  int ret;
  int i;
  int nic_num = 0;
  
  hif_info->dev_removed = wf_false;
  
  /* power on */
  LOG_D("************HIF DEV INSERT*************");
  LOG_D("<< Power on >>");
  if (power_on(hif_info) < 0)
  {
    LOG_E("===>power_on error, exit!!");
    return WF_RETURN_FAIL;
  }

  LOG_D("wf_power_on success");
  
#ifdef CONFIG_RICHV200
  side_road_cfg(hif_info);
#endif
  
  if(HIF_SDIO ==hif_info->hif_type)
  {
    // cmd53 is ok, next for side-road configue
    wf_sdioh_config(hif_info);
    wf_sdioh_interrupt_enable(hif_info);
  }
  
  /*create hif trx func*/
  LOG_D("<< create hif tx/rx queue >>");
  wf_data_queue_mngt_init(hif_info);
  
#ifdef CONFIG_RICHV200
  ret = wf_hif_bulk_enable(hif_info);
  if(WF_RETURN_FAIL == ret)
  {
    LOG_E("[%s] wf_hif_bulk_enable failed",__func__);
    return -1;
  }
#endif
  
  ret = wf_hif_queue_enable(hif_info);
  if(WF_RETURN_FAIL == ret)
  {
    LOG_E("[%s] wf_hif_queue_enable failed",__func__);
    return -1;
  }
  
  /*ndev reg*/
  LOG_D("<< add nic to hif_node >>");
  LOG_D("   node_id    :%d",hif_info->node_id);
  LOG_D("   hif_type   :%d  [1:usb  2:sdio]",hif_info->hif_type);
  
#ifdef CONFIG_STA_AND_AP_MODE
  nic_num = 2;
#else 
  nic_num = 1;
#endif
  hif_hw_access_init(hif_info);
  for(i=0;i<nic_num;i++)
  {
    ret = hif_add_nic(hif_info,i);
    if(ret != 0)
    {
      LOG_E("[%s] wlan_register nic[1] failed",__func__);
      return -1;
    }
    
    LOG_D("<< wlan register %d>>",i);
#ifdef CFG_ENABLE_AP_MODE
    rt_wlan_register(hif_info->nic_info[i], RT_WLAN_DEVICE_STA_NAME);
    rt_wlan_set_mode(RT_WLAN_DEVICE_STA_NAME,RT_WLAN_AP);
#endif
    
#ifdef CFG_ENABLE_STA_MODE 
    rt_wlan_register(hif_info->nic_info[i], RT_WLAN_DEVICE_STA_NAME);
    rt_wlan_set_mode(RT_WLAN_DEVICE_STA_NAME,RT_WLAN_STATION);
#endif
    
#ifdef CONFIG_STA_AND_AP_MODE
    rt_wlan_register(hif_info->nic_info[i], (i==0)?RT_WLAN_DEVICE_STA_NAME:RT_WLAN_DEVICE_AP_NAME);
    rt_wlan_set_mode((i==0)?RT_WLAN_DEVICE_STA_NAME:RT_WLAN_DEVICE_AP_NAME,(i==0)?RT_WLAN_STATION:RT_WLAN_AP);
#endif
  }

  return 0;
}

void  hif_dev_removed(hif_node_st *hif_info)
{
  nic_info_st *nic_info   = NULL;
  int nic_cnt             = 0;
  
  hif_info->dev_removed = wf_true;
  
  LOG_D("************HIF DEV REMOVE [NODE:%d TYPE:%d]*************",
        hif_info->node_id,hif_info->hif_type);
#ifdef CONFIG_RICHV200
#if 0
  wf_mcu_reset_chip(hif_info->nic_info[0]);
  if(hif_info->cmd_completion_flag)
  {
    //wf_mdelay(HIF_BULK_MSG_TIMEOUT+10);
    LOG_D("<< wf_hif_bulk_cmd_post_exit >>");
    wf_hif_bulk_cmd_post_exit(hif_info);
  }
#else
  wf_hif_bulk_cmd_post_exit(hif_info);
#endif
#endif
  
  /* ndev unreg */
  LOG_D("<< ndev unregister >>");
  for (nic_cnt=0; nic_cnt<MAX_NIC; nic_cnt++)
  {
    nic_info = hif_info->nic_info[nic_cnt];
    if (nic_info)
    {
      LOG_D("[hif_dev_removed] nic_id    :%d",nic_info->ndev_id);
      nic_info->is_surprise_removed = hif_info->dev_removed;
      rt_wlan_shutdown(nic_info);
      if(wf_false == nic_info->is_init_commplete)
      {
        nic_term(nic_info);
      }
      rt_wlan_unregister(nic_info);
      if (0 == nic_cnt) //for concurrent mode
      {
        int i=0;
        for (i=1; i<MAX_NIC; i++)
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
  
  if(HIF_SDIO == hif_info->hif_type)
  {
    wf_sdioh_interrupt_disable(hif_info);
  }
  wf_hif_queue_disable(hif_info);
  
#ifdef CONFIG_RICHV200
  wf_hif_bulk_disable(hif_info);
#endif
  
  /* term hif trx mode */
  LOG_D("<< term hif tx/rx queue >>");
  wf_data_queue_mngt_term(hif_info);
  
  /* power off */
  LOG_D("<< Power off >>");
  
  if(power_off(hif_info)<0)
  {
    LOG_E("power_off failed");
  }
}

void hif_node_register(hif_node_st **node, hif_enum type, struct hif_node_ops *ops)
{
  hif_node_st  *hif_node = NULL;
  
  hif_node = wf_kzalloc(sizeof(hif_node_st));
  if(NULL == hif_node )
  {
    LOG_E("wf_kzalloc for hif_node failed");
    return;
  }
  
  hif_node->hif_type      = type;
  hif_node->node_id       = hm_new_id(NULL);
  hif_node->ops           = ops;
  
  wf_lock_lock(hm_get_lock());
  wf_list_insert_tail(&hif_node->next, hm_get_list_head());
  
  if(HIF_SDIO == hif_node->hif_type)
  {
    hm_set_sdio_num(HM_ADD);
  }
  else if( HIF_USB == hif_node->hif_type )
  {
    hm_set_usb_num(HM_ADD);
  }
  
  hm_set_num(HM_ADD);
  
  wf_lock_unlock(hm_get_lock());
  
  *node = hif_node;
  
};

void hif_node_unregister(hif_node_st *pnode)
{
  wf_list_t *tmp   = NULL;
  wf_list_t *next  = NULL;
  hif_node_st *node       = NULL;
  int ret                 = 0;
  
  LOG_I("[%s] start",__func__);
  wf_lock_lock(hm_get_lock());
  wf_list_for_each_safe(tmp,next,hm_get_list_head())
  {
    node = wf_list_entry(tmp,hif_node_st, next);
    if( NULL != node  && pnode == node )
    {
      wf_list_delete(&node->next);
      hm_set_num(HM_SUB);
      if( HIF_USB == node->hif_type )
      {
        hm_set_usb_num(HM_SUB);
      }
      else if(HIF_SDIO == node->hif_type)
      {
        hm_set_sdio_num(HM_SUB);
      }
      break;
    }
  }
  
  if(NULL != node)
  {
    ret = hm_del_id(node->node_id);
    if(ret)
    {
      LOG_E("hm_del_id [%d] failed",node->node_id);
    }
    wf_kfree(node);
    node = NULL;
  }
  
  wf_lock_unlock(hm_get_lock());
  LOG_I("[%s] end",__func__);
}

#ifdef CONFIG_RICHV200


static void io_txdesc_chksum(wf_u8 *ptx_desc)
{
  wf_u16 *usPtr = (wf_u16 *) ptx_desc;
  wf_u32 index;
  wf_u16 checksum = 0;
  
  for (index = 0; index < 9; index++)
    checksum ^= le16_to_cpu(*(usPtr + index));
  
  wf_set_bits_to_le_u32(ptx_desc + 16, 16, 16, checksum);
}

static wf_u16 io_firmware_chksum(wf_u8 *firmware, wf_u32 len)
{
  wf_u32 loop;
  wf_u16 *u16Ptr = (wf_u16 *) firmware;
  wf_u32 index;
  wf_u16 checksum = 0;
  
  loop = len / 2;
  for (index = 0; index < loop; index++)
    checksum ^= le16_to_cpu(*(u16Ptr + index));
  
  return checksum;
}

int wf_hif_bulk_enable(hif_node_st *hif_node)
{
  hif_node->reg_buffer =  wf_kzalloc(512);
  if(NULL == hif_node->reg_buffer)
  {
    LOG_E("no memmory for hif reg buffer");
    return WF_RETURN_FAIL;
  }
  hif_node->fw_buffer =  wf_kzalloc(512);
  if(NULL == hif_node->fw_buffer)
  {
    LOG_E("no memmory for hif fw buffer");
    return WF_RETURN_FAIL;
  }
  hif_node->cmd_snd_buffer =  wf_kzalloc(512);
  if(NULL == hif_node->cmd_snd_buffer)
  {
    LOG_E("no memmory for hif cmd buffer");
    return WF_RETURN_FAIL;
  }
  
  hif_node->cmd_rcv_buffer =  wf_kzalloc(512);
  if(NULL == hif_node->cmd_rcv_buffer)
  {
    LOG_E("no memmory for hif cmd buffer");
    return WF_RETURN_FAIL;
  }
  
  rt_mutex_init(&hif_node->reg_mutex, "hif", RT_IPC_FLAG_FIFO);
  rt_completion_init(&hif_node->reg_completion);
  rt_completion_init(&hif_node->fw_completion);
  rt_completion_init(&hif_node->cmd_completion);
  
  hif_node->reg_size = 0;
  hif_node->fw_size = 0;
  hif_node->cmd_size = 0;
  hif_node->bulk_enable = wf_true;
  return WF_RETURN_OK;
}

int wf_hif_bulk_disable(hif_node_st *hif_node)
{
  if(NULL != hif_node->reg_buffer)
  {
    wf_kfree(hif_node->reg_buffer);
  }
  if(NULL != hif_node->fw_buffer)
  {
    wf_kfree(hif_node->fw_buffer);
  }
  if(NULL != hif_node->cmd_snd_buffer)
  {
    wf_kfree(hif_node->cmd_snd_buffer);
  }
  if(NULL != hif_node->cmd_rcv_buffer)
  {
    wf_kfree(hif_node->cmd_rcv_buffer);
  }
  hif_node->reg_size = 0;
  hif_node->fw_size = 0;
  hif_node->cmd_size = 0;
  
  return WF_RETURN_OK;
}

void wf_hif_bulk_reg_init(hif_node_st *hif_node)
{
  rt_completion_init(&hif_node->reg_completion);
  hif_node->reg_size = 0;
}

int wf_hif_bulk_reg_wait(hif_node_st *hif_node, wf_u32 timeout)
{
  return !rt_completion_wait(&hif_node->reg_completion, (timeout * WF_HZ) / 1000);
}

void wf_hif_bulk_reg_post(hif_node_st *hif_node, wf_u8 *buff, wf_u16 len)
{
  if(len <= 512)
  {
    wf_memcpy(hif_node->reg_buffer, buff, len);
    hif_node->reg_size = len;
    rt_completion_done(&hif_node->reg_completion);
  }
}

void wf_hif_bulk_fw_init(hif_node_st *hif_node)
{
  rt_completion_init(&hif_node->fw_completion);
  hif_node->fw_size = 0;
}

int wf_hif_bulk_fw_wait(hif_node_st *hif_node, wf_u32 timeout)
{
  return !rt_completion_wait(&hif_node->fw_completion, (timeout * WF_HZ) / 1000);
}

void wf_hif_bulk_fw_post(hif_node_st *hif_node, wf_u8 *buff, wf_u16 len)
{
  if(len <= 512)
  {
    wf_memcpy(hif_node->fw_buffer, buff, len);
    hif_node->fw_size = len;
    rt_completion_done(&hif_node->fw_completion);
  }
}

void wf_hif_bulk_cmd_init(hif_node_st *hif_node)
{
  rt_completion_init(&hif_node->cmd_completion);
  hif_node->cmd_size = 0;
}

int wf_hif_bulk_cmd_wait(hif_node_st *hif_node, wf_u32 timeout)
{
  hif_node->cmd_completion_flag = 1;
  hif_node->cmd_remove = 0;
  return !rt_completion_wait(&hif_node->cmd_completion, (timeout * WF_HZ) / 1000);
}

void wf_hif_bulk_cmd_post(hif_node_st *hif_node, wf_u8 *buff, wf_u16 len)
{
  if(len <= 512)
  {
    wf_memcpy(hif_node->cmd_rcv_buffer, buff, len);
    hif_node->cmd_size = len;
    hif_node->cmd_completion_flag = 0;
    hif_node->cmd_remove = 0;
    rt_completion_done(&hif_node->cmd_completion);
  }
}

void wf_hif_bulk_cmd_post_exit(hif_node_st *hif_node)
{
  hif_node->cmd_completion_flag = 0;
  hif_node->cmd_remove = 1;
  if(wf_true == hif_node->bulk_enable)
  {
    rt_completion_done(&hif_node->cmd_completion);
  }
}

int wf_hif_bulk_rxd_type(wf_u8 *prx_desc)
{
  wf_u8 u8Value;
  
  u8Value = wf_le_u8_read(prx_desc);
  
  return u8Value & 0x03;
}

int hif_write_firmware(void *node, wf_u8 which,  wf_u8 *firmware, wf_u32 len)
{
  wf_u8  u8Value;
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
  wf_u8 *alloc_buffer;
  wf_u8 *use_buffer;
  wf_u8 *ptx_desc;
  wf_u8 *prx_desc;
  hif_node_st *hif_node = (hif_node_st *)node;
  hif_mngent_st *hif = hif_mngent_get();
  
  if (hif_node->dev_removed == wf_true)
  {
    return -1;
  }
  
  if(which == FIRMWARE_M0)
  {
    if(hif->fw0_size == 0) {
      LOG_I("m0 firmware has already been download");
      return -1;
    }
  }
  else if(which == FIRMWARE_DSP)
  {
    if(hif->fw1_size == 0) {
      LOG_I("51 firmware has already been download");
      return -1;
    }
  }
  
  align_len = ((len + 3) / 4) * 4;
  
  /* alloc mem for xmit */
  buffer_len = TXDESC_OFFSET_NEW + TXDESC_PACK_LEN + FIRMWARE_BLOCK_SIZE;
  LOG_D("firmware download length is %d", len);
  LOG_D("firmware download buffer size is %d", buffer_len);
  alloc_buffer = wf_kzalloc(buffer_len + 4);
  if(alloc_buffer == NULL)
  {
    LOG_E("can't wf_kzalloc memmory for download firmware");
    return -1;
  }
  use_buffer = (wf_u8 *) WF_N_BYTE_ALIGMENT((SIZE_PTR) (alloc_buffer), 4);
  
  block_num = align_len / FIRMWARE_BLOCK_SIZE;
  if(align_len % FIRMWARE_BLOCK_SIZE)
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
  
  LOG_I("fwdownload block number is %d", block_num);
  wf_hif_bulk_fw_init(hif_node);
  
  for(i=0; i<block_num; i++)
  {
    wf_memset(use_buffer, 0, buffer_len);
    ptx_desc = use_buffer;
    /* set for fw xmit */
    wf_set_bits_to_le_u32(ptx_desc, 0, 2, TYPE_FW);
    /* set for first packet */
    if(i == 0)
    {
      wf_set_bits_to_le_u32(ptx_desc, 11, 1, 1);
    }
    /* set for last packet */
    if(i == (block_num - 1))
    {
      wf_set_bits_to_le_u32(ptx_desc, 10, 1, 1);
    }
    /* set for which firmware */
    wf_set_bits_to_le_u32(ptx_desc, 12, 1, which);
    /* set for reg HWSEQ_EN */
    wf_set_bits_to_le_u32(ptx_desc, 18, 1, 1);
    /* set for pkt_len */
    if(remain_size > FIRMWARE_BLOCK_SIZE)
    {
      send_once = FIRMWARE_BLOCK_SIZE;
    }
    else
    {
      send_once = remain_size;
    }
    
    wf_memcpy(ptx_desc + TXDESC_OFFSET_NEW, firmware + send_size, send_once);
    
    send_len = TXDESC_OFFSET_NEW + send_once;
    /* set for  firmware checksum */
    if(i == (block_num - 1))
    {
      checksum = io_firmware_chksum(firmware, align_len);
      LOG_I("cal checksum=0x%04x",checksum);
      wf_set_bits_to_le_u32(ptx_desc + send_len, 0, 32, checksum);
      LOG_D("my checksum is 0x%04x,fw_len=%d", checksum,align_len);
      send_len += TXDESC_PACK_LEN;
      send_once += TXDESC_PACK_LEN;
    }
    wf_set_bits_to_le_u32(ptx_desc + 8, 0, 16, send_once);
    
    /* set for checksum */
    io_txdesc_chksum(ptx_desc);
    
    if(hif_io_write(hif_node, 2, CMD_QUEUE_INX, (char *)ptx_desc, send_len) < 0)
    {
      LOG_E("bulk download firmware error");
      wf_kfree(alloc_buffer);
      return -1;
    }
    
    send_size += send_once;
    remain_size -= send_once;
    // delay for giving hardware handle time
    rt_hw_us_delay(100);
  }
  
  if(wf_hif_bulk_fw_wait(hif_node, HIF_BULK_MSG_TIMEOUT) == 0)
  {
    LOG_E("bulk access fw read timeout");
    wf_kfree(alloc_buffer);
    return -1;
  }
  
  prx_desc = use_buffer;
  back_len = RXDESC_OFFSET_NEW + RXDESC_PACK_LEN;
  if(hif_node->fw_size != back_len)
  {
    LOG_E("bulk access fw read length error");
    wf_kfree(alloc_buffer);
    return -1;
  }
  
  wf_memcpy(prx_desc, hif_node->fw_buffer, hif_node->fw_size);
  
  u8Value = wf_le_u8_read(prx_desc);
  if((u8Value & 0x03) != TYPE_FW)
  {
    LOG_E("bulk download firmware type error by read back");
    wf_kfree(alloc_buffer);
    return -1;
  }
  u16Value = wf_le_u16_read(prx_desc + 4);
  u16Value &= 0x3FFF;
  if(u16Value != RXDESC_PACK_LEN)
  {
    LOG_E("bulk download firmware length error, value: %d", u16Value);
    wf_kfree(alloc_buffer);
    return -1;
  }
  
  u8Value = wf_le_u8_read(prx_desc + 16);
  if(u8Value != 0x00)
  {
    LOG_E("bulk download firmware status error");
    u16Value = wf_le_u16_read(prx_desc + 18);
    LOG_D("Read checksum is 0x%04x", u16Value);
    if(u8Value == 0x01)
    {
      LOG_E("bulk download firmware txd checksum error");
    }
    else if(u8Value ==0x02)
    {
      LOG_E("bulk download firmware fw checksum error");
    }
    else if(u8Value ==0x03)
    {
      LOG_E("bulk download firmware fw & txd checksum error");
    }
    wf_kfree(alloc_buffer);
    return -1;
  }
  wf_kfree(alloc_buffer);
  
  if(which == FIRMWARE_M0)
  {
    LOG_I("bulk download m0 firmware ok");
    hif->fw0_size = 0;
    if(hif->fw0) {
      wf_kfree(hif->fw0);
      hif->fw0 = NULL;
    }
  }
  else if(which == FIRMWARE_DSP)
  {
    LOG_I("bulk download dsp firmware ok");
    hif->fw1_size = 0;
    if(hif->fw1) {
      wf_kfree(hif->fw0);
      hif->fw1 = NULL;
    }
  }
  
  return 0;
}


int hif_write_cmd(void *node, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len)
{
  int ret = 0;
  wf_u8  u8Value;
  wf_u16 u16Value;
  wf_u8 *ptx_desc;
  wf_u8 *prx_desc;
  wf_u16 snd_pktLen = 0;
  wf_u16 rcv_pktLen = 0;
  hif_node_st *hif_node = (hif_node_st *)node;
  
  if (hif_node->dev_removed == wf_true)
  {
    return -1;
  }
  
  ptx_desc = hif_node->cmd_snd_buffer;
  wf_memset(ptx_desc, 0, TXDESC_OFFSET_NEW + CMD_PARAM_LENGTH_TX);
  
  /* set for reg xmit */
  wf_set_bits_to_le_u32(ptx_desc, 0, 2, TYPE_CMD);
  /* set for cmd index */
  wf_set_bits_to_le_u32(ptx_desc, 2, 8, 0);
  /* set for reg HWSEQ_EN */
  wf_set_bits_to_le_u32(ptx_desc, 18, 1, 1);
  /* set SEQ  for test*/
  //wf_set_bits_to_le_u32(ptx_desc, 24, 8, __gmcu_cmd_count & 0xFF);
  /* set for pkt_len */
  wf_set_bits_to_le_u32(ptx_desc + 8, 0, 16, CMD_PARAM_LENGTH_TX + send_len * 4);
  /* set for checksum */
  io_txdesc_chksum(ptx_desc);
  
  /* set for  func_code */
  wf_set_bits_to_le_u32(ptx_desc + TXDESC_OFFSET_NEW, 0, 32, cmd);
  /* set for  len */
  wf_set_bits_to_le_u32(ptx_desc + TXDESC_OFFSET_NEW + 4, 0, 32, send_len);
  /* set for  offs */
  wf_set_bits_to_le_u32(ptx_desc + TXDESC_OFFSET_NEW + 8, 0, 32, recv_len);
  
  /* set for send content */
  if(send_len != 0)
  {
    wf_memcpy(ptx_desc + TXDESC_OFFSET_NEW + CMD_PARAM_LENGTH_TX, send_buf, send_len * 4);
  }
  
  snd_pktLen = TXDESC_OFFSET_NEW + CMD_PARAM_LENGTH_TX + send_len * 4;
  
  wf_hif_bulk_cmd_init(hif_node);
  ret = wf_tx_queue_insert(hif_node, 1, (char *)ptx_desc, snd_pktLen, wf_quary_addr(CMD_QUEUE_INX), NULL, NULL, NULL);
  if(ret != 0)
  {
    LOG_E("bulk access cmd error by send");
    ret = -1;
    goto mcu_cmd_communicate_exit;
  }
  
  if(wf_hif_bulk_cmd_wait(hif_node, HIF_BULK_MSG_TIMEOUT) == 0)
  {
    LOG_E("bulk access cmd read timeout");
    ret = -1;
    goto mcu_cmd_communicate_exit;
  }
  
  prx_desc = hif_node->cmd_rcv_buffer;
  rcv_pktLen = RXDESC_OFFSET_NEW + recv_len * 4 + CMD_PARAM_LENGTH_RX;
  if(hif_node->cmd_size != rcv_pktLen)
  {
    LOG_E("mcu cmd: 0x%08X", cmd);
    LOG_E("bulk access cmd read length error, recv cmd size is %d, but need pkt_len is %d", hif_node->cmd_size, rcv_pktLen);
    ret = -1;
    goto mcu_cmd_communicate_exit;
  }
  
  prx_desc = hif_node->cmd_rcv_buffer;
  u8Value = wf_le_u8_read(prx_desc);
  if((u8Value & 0x03) != TYPE_CMD)
  {
    LOG_E("bulk access cmd read error");
    ret = -1;
    goto mcu_cmd_communicate_exit;
  }
  u16Value = wf_le_u16_read(prx_desc + 4);
  u16Value &= 0x3FFF;
  if(u16Value != (recv_len * 4 + CMD_PARAM_LENGTH_RX))
  {
    LOG_E("bulk access cmd read length error, value is %d, send cmd is 0x%x, cmd is 0x%x",
          u16Value, cmd, *((wf_u32 *)prx_desc + RXDESC_OFFSET_NEW));
    
    ret = -1;
    goto mcu_cmd_communicate_exit;
  }
  
  if(recv_len != 0)
  {
    wf_memcpy(recv_buf, prx_desc + RXDESC_OFFSET_NEW + CMD_PARAM_LENGTH_RX, recv_len * 4);
  }
  
mcu_cmd_communicate_exit:
  
  return ret;
}

#endif




