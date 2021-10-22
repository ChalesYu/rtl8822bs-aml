/*
 * fw_download.c
 *
 * used for firmware download
 *
 * Author: hejiuyi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "common.h"
#include "wf_debug.h"

#define WF_START_ADDR        0x00e8
#define REG_MCUFWDL          0x0080
#define MCUFWDL_EN           WF_BIT(0)
#define MCUFWDL_RDY          WF_BIT(1)
#define FWDL_ChkSum_rpt      WF_BIT(2)
#define MACINI_RDY           WF_BIT(3)
#define BBINI_RDY            WF_BIT(4)
#define RFINI_RDY            WF_BIT(5)
#define WINTINI_RDY          WF_BIT(6)
#define RAM_DL_SEL           WF_BIT(7)
#define CPU_DL_READY         WF_BIT(15) /* add flag  by gw for fw download ready 20130826 */
#define ROM_DLEN             WF_BIT(19)
#define CPRST                WF_BIT(23)

#define DSP_CTRL_READ8       wf_io_read8
#define DSP_CTRL_WRITE8      wf_io_write8
#define DSP_CTRL_READ32      wf_io_read32
#define DSP_CTRL_WRITE32     wf_io_write32

static int fwdl_wait_fw_startup(nic_info_st *nic_info)
{
  wf_u32 value_to_check = 0;
  wf_u32 value32;
  wf_u16 count;
  wf_u32 value_expected = (MCUFWDL_RDY | FWDL_ChkSum_rpt | WINTINI_RDY | RAM_DL_SEL);

  count = 0;
  do {
    count++;
    value32 = DSP_CTRL_READ32(nic_info, REG_MCUFWDL,NULL);
    value_to_check = value32 & value_expected;
    if (value_to_check == value_expected){
      break;
    }
    LOG_I("Polling FW Ready......, True value:0x%08X,  Need value:0x%08X, but value:0x%08X", value32, value_expected, value_to_check);
    wf_mdelay(100);
  } while (count < 20);
  
  LOG_I("count:%d",count);
  if (value_to_check != value_expected) {
    LOG_I("%s: fail\n", __func__);
    return WF_RETURN_FAIL;
  }
  
  LOG_I("%s: Finish\n", __func__);
  return WF_RETURN_OK;
}

int wf_fw_download(nic_info_st *nic_info)
{
  wf_timer_t timer;
  fwdl_info_t *fwdl_info = &nic_info->fwdl_info;
  
  LOG_I("start");
  wf_hw_mcu_disable(nic_info);
  wf_hw_mcu_enable(nic_info);
  
  wf_timer_set(&timer, 0);
  LOG_I("fw downloading.....");
  
  int ret;
  wf_u8  value8;
  
  if(fwdl_info->fw_rom_type)
  {
    LOG_D("new rom select");
    
    value8 = wf_io_read8(nic_info, 0xf4,NULL);
    value8 &= 0xFE;
    ret = wf_io_write8(nic_info, 0xf4, value8);
    value8 = wf_io_read8(nic_info, 0xf4,NULL);
  }
  else
  {
    LOG_D("old rom select");
    
    value8 = wf_io_read8(nic_info, 0xf4,NULL);
    value8 |= 0x01;
    ret = wf_io_write8(nic_info, 0xf4, value8);
    value8 = wf_io_read8(nic_info, 0xf4,NULL);
  }
  
  if(0 != wf_io_write_firmware(nic_info, 0, (wf_u8 *)fwdl_info->fw0, fwdl_info->fw0_size))
  {
    return -1;
  }
  
  if(0 != wf_io_write_firmware(nic_info, 1, (wf_u8 *)fwdl_info->fw1, fwdl_info->fw1_size))
  {
    return -1;
  }
  
  
  LOG_D("===>fw download elapsed:  %d ms", wf_timer_elapsed(&timer));
  
  if (wf_hw_mcu_startup(nic_info) < 0)
  {
    LOG_E("===>wf_hw_mcu_startup error, exit!!");
    return WF_RETURN_FAIL;
  }
  
  if(fwdl_wait_fw_startup(nic_info) < 0)
  {
    LOG_E("===>dsp_run_startup error, exit!!");
    return WF_RETURN_FAIL;
  }
  
  wf_mcu_disable_fw_dbginfo(nic_info);
  
  LOG_I("end");
  
  return 0;
}


