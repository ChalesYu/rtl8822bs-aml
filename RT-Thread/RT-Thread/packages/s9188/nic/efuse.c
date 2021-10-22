/*
 * efuse.c
 *
 * used for read efuse value
 *
 * Author: songqiang
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "common.h"
#include "wf_debug.h"

#define BOOT_FROM_EEPROM        WF_BIT(4)
#define EEPROMSEL               WF_BIT(4)
#define EEPROM_EN               WF_BIT(5)

int wf_mcu_efuse_get(nic_info_st *nic_info, EUSE_CODE efuse_code, wf_u32 *outdata, wf_u32 outdata_len)
{
    int ret = 0;
    wf_u32 efuse_num = efuse_code;
    wf_u8 *outbuff = NULL;
    outbuff = wf_kzalloc(MAILBOX_MAX_TXLEN * 4);
    if (outbuff == NULL) {
      LOG_E("alloc recv buff fail");
      return  WF_RETURN_FAIL;
    }
    ret = mcu_cmd_communicate(nic_info,UMSG_OPS_MP_EFUSE_GET,&efuse_num,1,(wf_u32*)outbuff,MAILBOX_MAX_TXLEN);

	wf_memcpy(outdata,&outbuff[1],outdata_len);

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] UMSG_OPS_MP_EFUSE_GET failed", __func__);
		if (outbuff) 
		{
			wf_kfree(outbuff);
		}
        return ret;
    }
    else if(WF_RETURN_CMD_BUSY == ret)
    {
        LOG_W("[%s] cmd busy,try again if need!",__func__);
		if (outbuff) 
		{
			wf_kfree(outbuff);
		}
        return ret;
    }
	if (outbuff) 
	{
		wf_kfree(outbuff);
	}

    return WF_RETURN_OK;
}

