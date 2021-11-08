/*
 * wf_wlan_cmd.c
 *
 * used for wlan debug.
 *
 * Author: renhaibo
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "common.h"
#include "hif/hif.h"
#include "wf_wlan_dev.h"

int wifi_txinfo(int argc, char **argv)
{
  if (argc == 1)
  {
    rt_device_t device;
    struct rt_wlan_device *wlan;
    struct rt_wlan_priv *wlan_priv;
    nic_info_st *nic_info;
    tx_info_st *tx_info;
    
    device = rt_device_find("wlan0");
    if(RT_NULL == device) {
      rt_kprintf("can't the wifi device\r\n");
      return -1;
    }
    
    wlan = rt_container_of(device, struct rt_wlan_device, device);
    wlan_priv = wlan->user_data;
    nic_info = wlan_priv->nic;
    tx_info = nic_info->tx_info;
    
    rt_kprintf("wifi tx_info, free_xmitbuf_cnt:%d, free_xmitframe_cnt:%d,pending_frame_cnt:%d\r\n",
               tx_info->free_xmitbuf_cnt, tx_info->free_xmitframe_cnt,
               tx_info->pending_frame_cnt);
  }
  else
  {
    rt_kprintf("format error, eg: wifi_txinfo \r\n");
  }
  
  return 0;
}
#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(wifi_txinfo,  display wifi tx_info);
#endif /* FINSH_USING_MSH */

int wifi_rxinfo(int argc, char **argv)
{
  if (argc == 1)
  {
    rt_device_t device;
    struct rt_wlan_device *wlan;
    struct rt_wlan_priv *wlan_priv;
    nic_info_st *nic_info;
    hif_node_st *hif_info;
    
    device = rt_device_find("wlan0");
    if(RT_NULL == device) {
      rt_kprintf("can't the wifi device\r\n");
      return -1;
    }
    
    wlan = rt_container_of(device, struct rt_wlan_device, device);
    wlan_priv = wlan->user_data;
    nic_info = wlan_priv->nic;
    hif_info = nic_info->hif_node;
    
    rt_kprintf("wifi rx_info, free_netbuf_queue:%d, use_netbuf_queue:%d\r\n",
               wf_que_count(&hif_info->trx_pipe.free_netbuf_queue), 
               wf_que_count(&hif_info->trx_pipe.netbuf_queue));
    if(HIF_SDIO == hif_info->hif_type)
    {
        hif_sdio_st *sd = &hif_info->u.sdio;

        printf("irq_cnt:%lld, int_flag:%d\n",sd->irq_cnt,sd->int_flag);
//        rt_kprintf("0x9008:0x%08x\n",hif_io_read32(hif_info, SDIO_BASE | WL_REG_HISR,NULL));
        rt_kprintf("0x284:0x%08x\n",hif_io_read32(hif_info, 0x284,NULL));
        rt_kprintf("0x288:0x%08x\n",hif_io_read32(hif_info, 0x288,NULL));
        rt_kprintf("0x114:0x%08x\n",hif_io_read32(hif_info, 0x114,NULL));
        rt_kprintf("0x200:0x%08x\n",hif_io_read32(hif_info, 0x200,NULL));
        rt_kprintf("0x204:0x%08x\n",hif_io_read32(hif_info, 0x204,NULL));
        rt_kprintf("0x208:0x%08x\n",hif_io_read32(hif_info, 0x208,NULL));
        rt_kprintf("0x20C:0x%08x\n",hif_io_read32(hif_info, 0x20C,NULL));
        rt_kprintf("0x210:0x%08x\n",hif_io_read32(hif_info, 0x210,NULL));
        rt_kprintf("0x214:0x%08x\n",hif_io_read32(hif_info, 0x214,NULL));
        rt_kprintf("0x218:0x%08x\n",hif_io_read32(hif_info, 0x218,NULL));
        rt_kprintf("0x21c:0x%08x\n",hif_io_read32(hif_info, 0x21c,NULL));
    }
  }
  else
  {
    rt_kprintf("format error, eg: wifi_rxinfo \r\n");
  }
  
  return 0;
}
#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(wifi_rxinfo,  display wifi rx_info);
#endif /* FINSH_USING_MSH */
