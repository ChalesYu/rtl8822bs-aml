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
    
    rt_kprintf("WIFItx_info, free_xmitbuf_cnt:%d, free_xmitframe_cnt:%d,pending_frame_cnt:%d\r\n",
               tx_info->free_xmitbuf_cnt, tx_info->free_xmitframe_cnt,
               tx_info->pending_frame_cnt);
  }
  else
  {
    rt_kprintf("famat error, eg: wifi_txinfo wlan0 \r\n");
  }
  
  return 0;
}
#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(wifi_txinfo,  display wifi tx_info);
#endif /* FINSH_USING_MSH */
