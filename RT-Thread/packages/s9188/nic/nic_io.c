/*
 * nic_io.c
 *
 * used for nic io read or write
 *
 * Author: renhaibo
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
#include "common.h"
#include "wf_debug.h"

wf_u8 wf_io_read8(const nic_info_st *nic_info, wf_u32 addr,int *err)
{
    wf_u8 value;
    int ret = 0;
    WF_ASSERT(nic_info != NULL);

    ret = nic_info->nic_read(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
    if(err)
    {
        *err = ret;
    }

    return value;
}

wf_u16 wf_io_read16(const nic_info_st *nic_info, wf_u32 addr, int *err)
{
    wf_u16 value;
    int ret = 0;
    WF_ASSERT(nic_info != NULL);

    ret = nic_info->nic_read(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
    if(err)
    {
        *err = ret;
    }

    return value;
}

wf_u32 wf_io_read32(const nic_info_st *nic_info, wf_u32 addr, int *err)
{
    wf_u32 value;
    int ret = 0;
    WF_ASSERT(nic_info != NULL);

    ret = nic_info->nic_read(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
    if(err)
    {
        *err = ret;
    }

    return value;
}

int wf_io_write8(const nic_info_st *nic_info, wf_u32 addr, wf_u8 value)
{
    WF_ASSERT(nic_info != NULL);

    return nic_info->nic_write(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
}

int wf_io_write16(const nic_info_st *nic_info, wf_u32 addr, wf_u16 value)
{
    WF_ASSERT(nic_info != NULL);

    return nic_info->nic_write(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
}

int wf_io_write32(const nic_info_st *nic_info, wf_u32 addr, wf_u32 value)
{
    WF_ASSERT(nic_info != NULL);

    return nic_info->nic_write(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
}


int wf_io_write_data(const nic_info_st *nic_info, wf_u8 agg_num, char *pbuf, wf_u32 len, wf_u32 addr,
                     int (*callback_func)(void*tx_info, void *param), void *tx_info, void *param)
{
    int ret = 0;

    WF_ASSERT(nic_info != NULL);

    if (nic_info->nic_tx_queue_insert == NULL)
    {
        LOG_E("nic_tx_queue_insert is not register, please check!!");
        return -1;
    }

    ret = nic_info->nic_tx_queue_insert(nic_info->hif_node, agg_num, pbuf, len, addr,
                                        callback_func, tx_info, param);

    return ret;
}


int wf_io_write_data_queue_check(const nic_info_st *nic_info)
{
    WF_ASSERT(nic_info != NULL);
    if (nic_info->nic_tx_queue_empty == NULL)
    {
        LOG_E("nic_tx_queue_empty is not register, please check!!");
        return -1;
    }

    return nic_info->nic_tx_queue_empty(nic_info->hif_node);
}

int wf_io_tx_xmit_wake(const nic_info_st *nic_info)
{
    WF_ASSERT(nic_info != NULL);
    if (nic_info->nic_tx_wake == NULL)
    {
        LOG_E("nic_tx_wake is not register, please check!!");
        return -1;
    }

    return nic_info->nic_tx_wake((nic_info_st *)nic_info);
}


int wf_io_write_firmware(const nic_info_st *nic_info, wf_u8 which,  wf_u8 *firmware, wf_u32 len)
{
    WF_ASSERT(nic_info != NULL);
    WF_ASSERT(firmware != NULL);

    if (nic_info->nic_write_fw == NULL)
    {
        LOG_E("nic_write_fw is not register, please check!!");
        return -1;
    }

    return nic_info->nic_write_fw(nic_info->hif_node, which, firmware, len);
}


static char* cmd_to_str(int cmd)
{
#define c2s(x) #x
    switch(cmd)
    {
        case FUNC_REPLY                                     :
            return c2s(FUNC_REPLY);
        case UMSG_OPS_READ_VERSION                          :
            return c2s(UMSG_OPS_READ_VERSION);
        case UMSG_OPS_HAL_EFUSEMAP                          :
            return c2s(UMSG_OPS_HAL_EFUSEMAP);
        case UMSG_OPS_HAL_EFUSEMAP_LEN                      :
            return c2s(UMSG_OPS_HAL_EFUSEMAP_LEN);
        case UMSG_OPS_HAL_EFUSETYPE                         :
            return c2s(UMSG_OPS_HAL_EFUSETYPE);
        case UMSG_OPS_HAL_SET_HWREG                         :
            return c2s(UMSG_OPS_HAL_SET_HWREG);
        case UMSG_OPS_HAL_GET_HWREG                         :
            return c2s(UMSG_OPS_HAL_GET_HWREG);
        case UMSG_OPS_HAL_MSG_WDG                           :
            return c2s(UMSG_OPS_HAL_MSG_WDG);
        case UMSG_OPS_HAL_WRITEVAR_MSG                      :
            return c2s(UMSG_OPS_HAL_WRITEVAR_MSG);
        case UMSG_OPS_HAL_READVAR_MSG                       :
            return c2s(UMSG_OPS_HAL_READVAR_MSG);
        case UMSG_0PS_MSG_GET_RATE_BITMAP                   :
            return c2s(UMSG_0PS_MSG_GET_RATE_BITMAP);
        case UMSG_OPS_MSG_RHY_STATUS                        :
            return c2s(UMSG_OPS_MSG_RHY_STATUS);
        case UMSG_OPS_HAL_GET_MSG_STA_INFO                  :
            return c2s(UMSG_OPS_HAL_GET_MSG_STA_INFO);
        case UMSG_OPS_HAL_SYNC_MSG_STA_INFO                 :
            return c2s(UMSG_OPS_HAL_SYNC_MSG_STA_INFO);
        case UMSG_OPS_HAL_CALI_LLC                          :
            return c2s(UMSG_OPS_HAL_CALI_LLC);
        case UMSG_OPS_HAL_PHY_IQ_CALIBRATE                  :
            return c2s(UMSG_OPS_HAL_PHY_IQ_CALIBRATE);
        case UMSG_OPS_HAL_CHNLBW_MODE                       :
            return c2s(UMSG_OPS_HAL_CHNLBW_MODE);
        case UMSG_OPS_HAL_DW_FW                             :
            return c2s(UMSG_OPS_HAL_DW_FW);
        case UMSG_OPS_HAL_FW_INIT                           :
            return c2s(UMSG_OPS_HAL_FW_INIT);
        case UMSG_OPS_HAL_UPDATE_THERMAL                    :
            return c2s(UMSG_OPS_HAL_UPDATE_THERMAL);
        case UMSG_OPS_HAL_UPDATE_TX_FIFO                    :
            return c2s(UMSG_OPS_HAL_UPDATE_TX_FIFO);
        case UMSG_OPS_HAL_SET_BCN_REG                       :
            return c2s(UMSG_OPS_HAL_SET_BCN_REG);
        case UMSG_OPS_HAL_SET_MAC                           :
            return c2s(UMSG_OPS_HAL_SET_MAC);
        case UMSG_OPS_HAL_SET_BSSID                         :
            return c2s(UMSG_OPS_HAL_SET_BSSID);
        case UMSG_OPS_HAL_SET_BCN                           :
            return c2s(UMSG_OPS_HAL_SET_BCN);
        case UMSG_OPS_HW_SET_BASIC_RATE                     :
            return c2s(UMSG_OPS_HW_SET_BASIC_RATE);
        case UMSG_OPS_HW_SET_OP_MODE                        :
            return c2s(UMSG_OPS_HW_SET_OP_MODE);
        case UMSG_OPS_HW_SET_CORRECT_TSF                    :
            return c2s(UMSG_OPS_HW_SET_CORRECT_TSF);
        case UMSG_OPS_HW_SET_MLME_DISCONNECT                :
            return c2s(UMSG_OPS_HW_SET_MLME_DISCONNECT);
        case UMSG_OPS_HW_SET_MLME_SITE                      :
            return c2s(UMSG_OPS_HW_SET_MLME_SITE);
        case UMSG_OPS_HW_SET_MLME_JOIN                      :
            return c2s(UMSG_OPS_HW_SET_MLME_JOIN);
        case UMSG_OPS_HW_SET_DK_CFG                         :
            return c2s(UMSG_OPS_HW_SET_DK_CFG);
        case UMSG_OPS_HAL_SEC_WRITE_CAM                     :
            return c2s(UMSG_OPS_HAL_SEC_WRITE_CAM);
        case UMSG_OPS_HAL_H2C_CMD                           :
            return c2s(UMSG_OPS_HAL_H2C_CMD);
        case UMSG_OPS_HAL_CHECK_RXFIFO_FULL                  :
            return c2s(UMSG_OPS_HAL_CHECK_RXFIFO_FULL);
        case UMSG_OPS_HAL_LPS_OPT                           :
            return c2s(UMSG_OPS_HAL_LPS_OPT);
        case UMSG_OPS_HAL_LPS_CONFIG                        :
            return c2s(UMSG_OPS_HAL_LPS_CONFIG);
        case UMSG_OPS_HAL_LPS_SET                           :
            return c2s(UMSG_OPS_HAL_LPS_SET);
        case UMSG_OPS_HAL_LPS_GET                           :
            return c2s(UMSG_OPS_HAL_LPS_GET);
        case UMSG_OPS_HAL_SET_USB_AGG_NORMAL                :
            return c2s(UMSG_OPS_HAL_SET_USB_AGG_NORMAL);
        case UMSG_OPS_EFUSE_1BYTE                           :
            return c2s(UMSG_OPS_EFUSE_1BYTE);
        case UMSG_OPS_HAL_DEINIT                            :
            return c2s(UMSG_OPS_HAL_DEINIT);
        case UMSG_OPS_MP_SET_ANT_TX                         :
            return c2s(UMSG_OPS_MP_SET_ANT_TX);
        case UMSG_OPS_MP_SET_ANT_RX                         :
            return c2s(UMSG_OPS_MP_SET_ANT_RX);
        case UMSG_OPS_MP_PROSET_TXPWR_1                     :
            return c2s(UMSG_OPS_MP_PROSET_TXPWR_1);
        case UMSG_OPS_MP_INIT                               :
            return c2s(UMSG_OPS_MP_INIT);
        case UMSG_OPS_MP_SET_PRX                            :
            return c2s(UMSG_OPS_MP_SET_PRX);
        case UMSG_OPS_MP_DIS_DM                             :
            return c2s(UMSG_OPS_MP_DIS_DM);
        case UMSG_OPS_MP_SET_CCKCTX                         :
            return c2s(UMSG_OPS_MP_SET_CCKCTX);
        case UMSG_OPS_MP_SET_OFDMCTX                        :
            return c2s(UMSG_OPS_MP_SET_OFDMCTX);
        case UMSG_OPS_MP_SET_SINGLECARRTX                   :
            return c2s(UMSG_OPS_MP_SET_SINGLECARRTX);
        case UMSG_OPS_MP_SET_SINGLETONETX                   :
            return c2s(UMSG_OPS_MP_SET_SINGLETONETX);
        case UMSG_OPS_MP_SET_CARRSUPPTX                     :
            return c2s(UMSG_OPS_MP_SET_CARRSUPPTX);
        case UMSG_OPS_MSG_WRITE_DIG                         :
            return c2s(UMSG_OPS_MSG_WRITE_DIG);
        case UMSG_OPS_MP_EFUSE_READ                         :
            return c2s(UMSG_OPS_MP_EFUSE_READ);
        case UMSG_OPS_MP_EFUSE_WRITE                        :
            return c2s(UMSG_OPS_MP_EFUSE_WRITE);
        case UMSG_OPS_MP_EFUSE_ACCESS                       :
            return c2s(UMSG_OPS_MP_EFUSE_ACCESS);
        case UMSG_OPS_MP_EFUSE_GSize                        :
            return c2s(UMSG_OPS_MP_EFUSE_GSize);
        case UMSG_OPS_MP_EFUSE_GET                          :
            return c2s(UMSG_OPS_MP_EFUSE_GET);
        case UMSG_OPS_MP_EFUSE_SET                          :
            return c2s(UMSG_OPS_MP_EFUSE_SET);
        case UMSG_OPS_MP_MACRXCOUNT                         :
            return c2s(UMSG_OPS_MP_MACRXCOUNT);
        case UMSG_OPS_MP_PHYRXCOUNT                         :
            return c2s(UMSG_OPS_MP_PHYRXCOUNT);
        case UMSG_OPS_MP_RESETCOUNT                         :
            return c2s(UMSG_OPS_MP_RESETCOUNT);
        case UMSG_OPS_MP_PHYTXOK                            :
            return c2s(UMSG_OPS_MP_PHYTXOK);
        case UMSG_OPS_MP_CTXRATE                            :
            return c2s(UMSG_OPS_MP_CTXRATE);
        case UMSG_OPS_MP_SET_POWER                          :
            return c2s(UMSG_OPS_MP_SET_POWER);
        case UMSG_OPS_MP_RESET_MAC_RX_COUNTERS              :
            return c2s(UMSG_OPS_MP_RESET_MAC_RX_COUNTERS);
        case UMSG_OPS_MP_RESET_PHY_RX_COUNTERS              :
            return c2s(UMSG_OPS_MP_RESET_PHY_RX_COUNTERS);
        case UMSG_OPS_MP_SET_RF_PATH_SWITCH                 :
            return c2s(UMSG_OPS_MP_SET_RF_PATH_SWITCH);
        case UMSG_OPS_MP_BB_RF_GAIN_OFFSET                  :
            return c2s(UMSG_OPS_MP_BB_RF_GAIN_OFFSET);
        case UMSG_OPS_HAL_FREQ_GET                          :
            return c2s(UMSG_OPS_HAL_FREQ_GET);
        case UMSG_OPS_HAL_FREQ_SET                          :
            return c2s(UMSG_OPS_HAL_FREQ_SET);
        case UMSG_OPS_HAL_TEMP_GET                          :
            return c2s(UMSG_OPS_HAL_TEMP_GET);
        case UMSG_OPS_HAL_TEMP_SET                          :
            return c2s(UMSG_OPS_HAL_TEMP_SET);
        case UMSG_OPS_HAL_SET_XTAL                          :
            return c2s(UMSG_OPS_HAL_SET_XTAL);
        case UMSG_OPS_HAL_SET_XTAL_DEFAULT                  :
            return c2s(UMSG_OPS_HAL_SET_XTAL_DEFAULT);
        case UMSG_OPS_MP_BB_REG_GET                         :
            return c2s(UMSG_OPS_MP_BB_REG_GET);
        case UMSG_OPS_MP_BB_REG_SET                         :
            return c2s(UMSG_OPS_MP_BB_REG_SET);
        case UMSG_OPS_MP_RF_REG_GET                         :
            return c2s(UMSG_OPS_MP_RF_REG_GET);
        case UMSG_OPS_MP_RF_REG_SET                         :
            return c2s(UMSG_OPS_MP_RF_REG_SET);
        case UMSG_OPS_MP_USER_INFO                          :
            return c2s(UMSG_OPS_MP_USER_INFO);
        case UMSG_OPS_CMD_TEST                              :
            return c2s(UMSG_OPS_CMD_TEST);
        case UMSG_OPS_RESET_CHIP                            :
            return c2s(UMSG_OPS_RESET_CHIP);
        case UMSG_OPS_HAL_DBGLOG_CONFIG                     :
            return c2s(UMSG_OPS_HAL_DBGLOG_CONFIG);
        default:
            LOG_E("Unknown cmd:0x%x",cmd);
            return "Unknown cmd";
    }
}

int wf_io_write_cmd_by_mailbox(nic_info_st *nic_info, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len)
{
    int ret  = 0;
    wf_u32 mailbox_reg_addr = MAILBOX_ARG_START;
    wf_u8 tryCnt = 0;

    nic_mcu_hw_access_lock(nic_info);
//    LOG_I(">>>>> func_code:0x%08x,%s sema ret:%d",cmd,cmd_to_str(cmd), ret);

    for (tryCnt = 0; tryCnt < 2; tryCnt++)
    {
        ret = wf_io_write32(nic_info,MAILBOX_REG_START,cmd);
        if(WF_RETURN_FAIL == ret)
        {
            LOG_E("[%s,%d] wf_io_write32 failed cmd:0x%x",__func__,__LINE__,cmd);
            goto exit;

        }
        ret = wf_io_write32(nic_info, mailbox_reg_addr, send_len);
        if(WF_RETURN_FAIL == ret)
        {
            LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
            goto exit;
        }

        mailbox_reg_addr += MAILBOX_WORD_LEN;
        ret = wf_io_write32(nic_info, mailbox_reg_addr, recv_len);
        if(WF_RETURN_FAIL == ret)
        {
            LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
            goto exit;
        }

        mailbox_reg_addr += MAILBOX_WORD_LEN;

        while ((send_len--) && send_buf)
        {
            ret = wf_io_write32(nic_info, mailbox_reg_addr, *send_buf++);
            if(WF_RETURN_FAIL == ret)
            {
                LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
                goto exit;
            }
            mailbox_reg_addr += MAILBOX_WORD_LEN;
        }

        ret = wf_mcu_cmd_get_status(nic_info,cmd);
        if (WF_RETURN_FAIL == ret)
        {
            if (tryCnt == 0)
            {
                LOG_W("mcu_cmd_get_status failed, try again. cmd:%s",cmd_to_str(cmd));
            }
            else
            {
                LOG_E("mcu_cmd_get_status failed, please check the hardware io. cmd:%s",cmd_to_str(cmd));
                goto exit;
            }
        }
        else if(WF_RETURN_REMOVED_FAIL == ret)
        {
            LOG_W("[%s,%d] driver or device is removed. cmd:0x%08x,%s",__func__,__LINE__,cmd,cmd_to_str(cmd));
            goto exit;
        }
        else
        {
            // m0 ack
            break;
        }
    }

    mailbox_reg_addr = MAILBOX_ARG_START;
    if (ret == 0)
    {
        while ((recv_len--) && recv_buf)
        {
            *recv_buf++ = wf_io_read32(nic_info, mailbox_reg_addr,NULL);
            mailbox_reg_addr += MAILBOX_WORD_LEN;
        }
    }
    
    ret = WF_RETURN_OK;

exit:
    nic_mcu_hw_access_unlock(nic_info);
//    LOG_I("<<<<<< [%d] func_code:0x%08x,%s",nic_info->ndev_id,cmd,cmd_to_str(cmd));

    return ret;
}


int wf_io_write_cmd_by_mailbox_try(nic_info_st *nic_info, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len)
{
    int ret = 0;
    wf_u32 mailbox_reg_addr = MAILBOX_ARG_START;

    nic_mcu_hw_access_trylock(nic_info);

//    LOG_I(">>>>> [%d] func_code:0x%08x,%s sema ret:%d",nic_info->ndev_id,cmd,cmd_to_str(cmd), ret);
    ret = wf_io_write32(nic_info,MAILBOX_REG_START,cmd);
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_io_write32 failed cmd:0x%x",__func__,__LINE__,cmd);
        goto exit;

    }
    ret = wf_io_write32(nic_info, mailbox_reg_addr, send_len);
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
        goto exit;
    }

    mailbox_reg_addr += MAILBOX_WORD_LEN;
    ret = wf_io_write32(nic_info, mailbox_reg_addr, recv_len);
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
        goto exit;
    }

    mailbox_reg_addr += MAILBOX_WORD_LEN;

    while ((send_len--) && send_buf)
    {
        ret = wf_io_write32(nic_info, mailbox_reg_addr, *send_buf++);
        if(WF_RETURN_FAIL == ret)
        {
            LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
            goto exit;
        }
        mailbox_reg_addr += MAILBOX_WORD_LEN;
    }

    ret = wf_mcu_cmd_get_status(nic_info,cmd);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("mcu_cmd_get_status failed, check mcu feedback");
        goto exit;
    }
    else if(WF_RETURN_REMOVED_FAIL == ret)
    {
        LOG_W("[%s,%d] driver or device is removed. cmd:0x%08x,%s",__func__,__LINE__,cmd,cmd_to_str(cmd));
        goto exit;
    }

    mailbox_reg_addr = MAILBOX_ARG_START;
    if (ret == 0)
    {
        while ((recv_len--) && recv_buf)
        {
            *recv_buf++ = wf_io_read32(nic_info, mailbox_reg_addr,NULL);
            mailbox_reg_addr += MAILBOX_WORD_LEN;
        }
    }

    ret = WF_RETURN_OK;

exit:

     nic_mcu_hw_access_unlock(nic_info);
//     LOG_I("<<<<<< [%d] func_code:0x%08x,%s sema ret:%d",nic_info->ndev_id,cmd,cmd_to_str(cmd), ret);

    return ret;
}


int wf_io_write_cmd_by_txd(nic_info_st *nic_info, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len)
{
    WF_ASSERT(nic_info != NULL);

    if (nic_info->nic_write_cmd == NULL)
    {
        LOG_E("nic_write_cmd is not register, please check!!");
        return -1;
    }

    return nic_info->nic_write_cmd(nic_info->hif_node, cmd, send_buf, send_len, recv_buf, recv_len);
}


