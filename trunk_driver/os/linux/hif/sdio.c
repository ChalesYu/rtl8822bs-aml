/*
 * sdio.c
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
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>

#include "wf_os_api.h"
#include "wf_list.h"
#include "hif.h"
#include "sdio.h"
#include "hif_queue.h"

#define SDIO_HW_QUEUE_HIGH  (1)
#define SDIO_HW_QUEUE_MID   (2)
#define SDIO_HW_QUEUE_LOW   (3)

#define SDIO_RETRUN_OK      (0)
#define SDIO_RETRUN_FAIL    (1)

#define SDIO_VENDOR_ID_WL   0x02e7
#define WL_DEVICE_ID_SDIO   0x9086
#define SDIO_ID_DARK_MODE   0x0DDF
#define SDIO_BLK_SIZE       (512)

#define ALIGN_SZ_RXBUFF     8


#define DBG_USE_AGG_NUM 1

#define RND4(x) (((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)

#define WORD_LEN (4)

#ifndef BIT
#define BIT(x)  (1 << (x))
#endif

#define ADDR_CHANGE(addr) ((8 << 13) | ((addr) & 0xFFFF))

static wf_s32 sdhz = 0;
module_param(sdhz,  int, 0);

static void sdio_interrupt_deregister(struct sdio_func *func);

wf_bool sdio_operation_is_ok(struct sdio_func *func)
{
#if 0
    hif_node_st *node = NULL;

    node        = sdio_get_drvdata(func);
    if(node->u.sdio.current_irq || node->u.sdio.current_irq == current)
    {
        return wf_false;
    }

    return wf_true;
#else
    return wf_true;
#endif
}
static wf_s32 sdio_func_print(struct sdio_func *func)
{
    LOG_I("func_num:%d, vender:0x%x, device:0x%x, max_blksize:%d, cur_blksize:%d, state:%d",
          (wf_s32)func->num,
          (wf_s32)func->vendor,
          (wf_s32)func->device,
          (wf_s32)func->max_blksize,
          (wf_s32)func->cur_blksize,
          func->state
         );

    return 0;
}

/* to set func->max_blksize, func->cur_blksize*/
static wf_s32 sdio_func_set_blk(struct sdio_func *func, unsigned blksize)
{
    wf_s32 ret = 0;

    sdio_claim_host(func);
    ret = sdio_set_block_size(func, blksize);
    if( ret )
    {
        LOG_E("[%s] sdio_set_block_size failed",__func__);
        sdio_release_host(func);
        return SDIO_RETRUN_FAIL;
    }

    ret = sdio_enable_func(func);
    if( 0 != ret )
    {
        LOG_E("[%s] sdio_enable_func failed",__func__);
        sdio_release_host(func);
        return SDIO_RETRUN_FAIL;
    }
    sdio_release_host(func);

    return ret;
}


static wf_u8 sdio_get_devid(wf_u32 addr)
{
    wf_u8 dev_id    = 0;
    wf_u16 pdev_id  = 0;

    pdev_id = (wf_u16)(addr >> 16);
    switch (pdev_id)
    {
        case 0x1025:
            dev_id = 0;
            break;

        case 0x1026:
            dev_id = 8;
            break;
        case 0x1031:
        case 0x1032:
        case 0x1033:
            dev_id = 4 + (pdev_id - 0x1031);
            break;
        default:
            dev_id = 8;
            break;
    }

    return dev_id;
}



static wf_u32 sdio_get_destaddr(const wf_u32 src_addr, wf_u8 *p_id, wf_u16 *p_set)
{
    wf_u8 dev_id    = 0;
    wf_u16 val_set  = 0;
    wf_u32 des_addr = 0;
    wf_s32 is_sdio_id  = 0;

    dev_id = sdio_get_devid(src_addr);
    val_set = 0;

    switch (dev_id)
    {
        case 0:
            is_sdio_id = 1;
            dev_id   = 8;
            val_set = src_addr & 0xFFFF;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            val_set = src_addr & 0x1FFF;
            break;
        case 7:
            val_set = src_addr & 0x0003;
            break;

        case 8:
        default:
            dev_id = 8;
#ifdef CONFIG_RICHV200
            val_set = src_addr & 0xFFFF;
#else
            val_set = src_addr & 0x7FFF;
#endif
            break;
    }
    des_addr = (dev_id << 13) | val_set;


    if (p_id)
    {
        if(!is_sdio_id)
        {
            *p_id = dev_id;
        }
        else
        {
            *p_id = 0;
        }
    }
    if (p_set) *p_set = val_set;

    return des_addr;
}

static wf_s32 sdio_data_disable(struct sdio_func *func)
{
    wf_s32 err      = 0;
    wf_u8 recv      = 0;
    wf_u8 send      = 0;

    sdio_claim_host(func);
    recv = sdio_readb(func, ADDR_CHANGE(WL_REG_HCTL), &err);
    if (err)
    {
        LOG_E("%s: ERR (%d) addr=0x%x\n", __func__, err, ADDR_CHANGE(WL_REG_HCTL));
        sdio_release_host(func);
        return SDIO_RETRUN_FAIL;
    }

    if (!(recv & BIT(1)))
    {
        LOG_E("recv bit(1) failed");
        sdio_release_host(func);
        return SDIO_RETRUN_FAIL;
    }

    send = recv | BIT(0);
    sdio_writeb(func, send, ADDR_CHANGE(WL_REG_HCTL), &err);
    if (err)
    {
        LOG_E("%s: ERR (%d) addr=0x%x\n", __func__, err, ADDR_CHANGE(WL_REG_HCTL));
        sdio_release_host(func);
        return SDIO_RETRUN_FAIL;
    }

    sdio_release_host(func);
    return SDIO_RETRUN_OK;

}


static wf_u8 sdio_data_enable(struct sdio_func *func)
{
    wf_s32 err          = 0;
    wf_u8 recv          = 0;
    wf_u8 send          = 0;
    wf_timer_t timer    = {0,0,0};

    sdio_claim_host(func);
    recv = sdio_readb(func, ADDR_CHANGE(WL_REG_HCTL), &err);
    if (err)
    {
        LOG_E("%s: ERR (%d) addr=0x%x\n", __func__, err, ADDR_CHANGE(WL_REG_HCTL));
        sdio_release_host(func);
        return 0;
    }
    if(recv & BIT(1) ) //resume
    {
        sdio_release_host(func);
        return 0;
    }

    send = recv & ( ~ BIT(0));
    sdio_writeb(func, send, ADDR_CHANGE(WL_REG_HCTL), &err);
    if (err)
    {
        LOG_E("%s: ERR (%d) addr=0x%x\n", __func__, err, ADDR_CHANGE(WL_REG_HCTL));
        sdio_release_host(func);
        return 0;
    }

    /* polling for BIT1 */
    wf_timer_set(&timer, 200);
    while (1)
    {
        recv = sdio_readb(func, ADDR_CHANGE(WL_REG_HCTL), &err);
        if (err)
        {
            LOG_E("%s: ERR (%d) addr=0x%x\n", __func__, err, ADDR_CHANGE(WL_REG_HCTL));
            sdio_release_host(func);
            return 0;
        }

        if (!err && (recv & BIT(1)))
            break;

        if (wf_timer_expired(&timer))
        {
            LOG_E("timeout(err:%d) sdh_val:0x%02x\n",err, recv);
            sdio_release_host(func);
            return 0;
        }
    }

    sdio_release_host(func);

    return 1;

}



wf_u8 sdio_get_hwQueue_by_fifoID(wf_u8 fifo_id)
{
    wf_u8 ret = 0;

    switch(fifo_id)
    {
        case 1:
        case 2:
        case 4:
            ret = SDIO_HW_QUEUE_HIGH;
            break;

        case 5:
            ret = SDIO_HW_QUEUE_MID;
            break;

        case 6:
        default:
            ret = SDIO_HW_QUEUE_LOW;
            break;
    }

    return ret;
}


wf_u8 sdio_get_fifoaddr_by_que_Index(wf_u8 queIndex, wf_u32 len, wf_u32 *fifo_addr, wf_u8 is_read)
{
    wf_u8 fifo_id = 0;

    switch(queIndex)
    {
#ifdef CONFIG_RICHV200
        case CMD_QUEUE_INX:
#ifdef CONFIG_RICHV300
            fifo_id = 7;
#else
            fifo_id = 3;
#endif
            break;
#endif
        case BE_QUEUE_INX:
        case BK_QUEUE_INX:
            fifo_id = 6;
            break;

        case VI_QUEUE_INX:
            fifo_id = 5;
            break;

        case MGT_QUEUE_INX:
        case BCN_QUEUE_INX:
        case VO_QUEUE_INX:
        case HIGH_QUEUE_INX:
            fifo_id = 1;
            break;

        case READ_QUEUE_INX:
            fifo_id = 7;
            break;
        default:
            break;
    }

    if (is_read > 0)
    {
        *fifo_addr = (fifo_id << 13) | ((len/4) & 0x0003);
    }
    else
    {
        *fifo_addr = (fifo_id << 13) | ((len/4) & 0x1FFF);
    }

    //LOG_I("sdio_get_fifoaddr_by_que_Index -> fifo_id[%d] fifo_addr[0x%x]",fifo_id, *fifo_addr);
    return fifo_id;

}


static wf_s32 sdio_write_data(struct sdio_func *func, wf_u32 addr, wf_u8 * data, wf_u32 len)
{

    wf_u32 des_addr     = 0;
    wf_u8 device_id     = 0;
    wf_u16 val_set      = 0;
    wf_u8 sus_leave     = 0;
    wf_s32 err_ret         = 0;
    wf_s32 i               = 0;
    hif_node_st *node   = NULL;
    wf_u32 len4rnd      = 0;

    node        = sdio_get_drvdata(func);

    len4rnd = WF_RND4(len);

    if(len <= 4)
    {
        des_addr = sdio_get_destaddr(addr, &device_id, &val_set);
        if (device_id == 8 && val_set < 0x100)
        {
            sus_leave = sdio_data_enable(func);
        }
    }
    else
    {
        //LOG_D("%s,addr=0x%04x,len=%d",__FUNCTION__,addr,len);

        sdio_get_fifoaddr_by_que_Index(addr,len4rnd,&des_addr, 0);
    }

    // print addr and value
#if 0
    if(len == 1)
    {
        LOG_I("write 0x%02x -> addr[0x%08x],des_addr=[0x%08x]",data[0],addr,des_addr);
    }
    else if (len == 2)
    {
        LOG_I("write 0x%04x -> addr[0x%08x],des_addr=[0x%08x]",*((wf_u16 *)data),addr,des_addr);
    }
    else if (len == 4)
    {
        LOG_I("write 0x%08x -> addr[0x%08x],des_addr=[0x%08x]",*((wf_u32 *)data),addr,des_addr);
    }
    else
    {
        LOG_I("write %d len bytes-> addr[0x%08x],des_addr=[0x%08x]",len,addr,des_addr);
    }
#endif

    // end of print
    sdio_claim_host(func);
    if(WORD_LEN == len)
    {
        wf_u32 data_value = * (wf_u32*)data;
        if(hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
        {
            sdio_release_host(func);
            return -1;
        }
        sdio_writel(func, data_value, des_addr, &err_ret);
        if (err_ret < 0)
        {
            if(-ENOMEDIUM == err_ret)
            {
                err_ret = WF_RETURN_REMOVED_FAIL;
                LOG_W("[%s,%d] device removed warning.",__func__,__LINE__);
            }
            else
            {
                LOG_E("sderr: Failed to write word, Err: 0x%08x,%s, ret:%d\n", addr, __func__,err_ret);
            }
        }
    }
    else if(WORD_LEN < len )
    {
#if 0
        printk("int sdio_write_data,printk data:len=%d\n",len);
        for(i=0; i<len; i++)
        {
            printk("0x%02x,",data[i]);
            if((i+1)%16 == 0)
                printk("\n");
        }
        printk("\nend\n");
#endif

        err_ret=sdio_memcpy_toio(func,des_addr,data,len);
        if (err_ret < 0)
        {
            if(-ENOMEDIUM == err_ret)
            {
                err_ret = WF_RETURN_REMOVED_FAIL;
                LOG_W("[%s,%d] device removed warning.",__func__,__LINE__);
            }
            else
            {
                LOG_E("sderr: sdio_memcpy_toio, Err: 0x%08x,%s, ret:%d\n", addr, __func__,err_ret);
            }
        }
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            sdio_writeb(func, *(data + i), des_addr + i, &err_ret);
            if (err_ret)
            {
                if(-ENOMEDIUM == err_ret)
                {
                    err_ret = WF_RETURN_REMOVED_FAIL;
                    LOG_W("[%s,%d] device removed warning.",__func__,__LINE__);
                }
                else
                {
                    LOG_E("sderr: Failed to write byte, Err: 0x%08x,%s\n", addr, __func__);
                }
                break;
            }
        }
    }
    sdio_release_host(func);

    if(1 == sus_leave )
    {
        sdio_data_disable(func);
        //LOG_I("[%s,%d]",__func__,__LINE__);
    }
    return err_ret;


}

wf_s32 sdio_read_data(struct sdio_func *func, wf_u32 addr, wf_u8 * data, wf_u32 len)
{
    wf_u32 des_addr     = 0;
    wf_u8 device_id     = 0;
    wf_u16 val_set      = 0;
    wf_u8 sus_leave     = 0;

    wf_u32 value32      = 0;
    wf_s32 err_ret      = 0;
    wf_s32 i            = 0;
    wf_u32 len4rnd      = 0;


    if(len <= 4)
    {
        des_addr = sdio_get_destaddr(addr, &device_id, &val_set);
        if (device_id == 8 && val_set < 0x100)
        {
            sus_leave =sdio_data_enable(func);
        }
    }
    else
    {
        len4rnd = WF_RND4(len);
        sdio_get_fifoaddr_by_que_Index(READ_QUEUE_INX,len4rnd,&des_addr, 1);
    }

    sdio_claim_host(func);
    if(WORD_LEN == len)
    {
        value32 = sdio_readl(func,des_addr,&err_ret);
        if (err_ret)
        {
            if(-ENOMEDIUM == err_ret)
            {
                err_ret = WF_RETURN_REMOVED_FAIL;
                LOG_W("[%s,%d] device removed warning.",__func__,__LINE__);
            }
            else
            {
                LOG_E("sderr: Failed to read word, Err: 0x%08x,%s, ret=%d\n", addr, __func__, err_ret);
            }
        }
        else
        {
            for (i = 0; i < len; i++)
            {
                data[i] = ((wf_u8*)&value32)[i];
            }
        }
    }
    else if(WORD_LEN < len )
    {
        //LOG_D("in sdio_read_data,dest_addr=0x%04x,len=%d",des_addr,len);
        err_ret = sdio_memcpy_fromio(func,data,des_addr,len);
        if (err_ret < 0)
        {

            if(-ENOMEDIUM == err_ret)
            {
                err_ret = WF_RETURN_REMOVED_FAIL;
                LOG_W("[%s,%d] device removed warning.",__func__,__LINE__);
            }
            else
            {
                LOG_E("sderr: sdio_memcpy_fromio, Err: 0x%08x,%s, ret:%d\n", addr, __func__,err_ret);
            }
        }
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            data[i] = sdio_readb(func, des_addr + i, &err_ret);
            //LOG_I("read addr[0x%08x]=0x%02x,des_addr=0x%08x",addr+i,data[i],des_addr+i);
            if (err_ret)
            {
                if(-ENOMEDIUM == err_ret)
                {
                    err_ret = WF_RETURN_REMOVED_FAIL;
                    LOG_W("[%s,%d] device removed warning.",__func__,__LINE__);

                }
                else
                {
                    LOG_E("sderr: Failed to read byte, Err: 0x%08x,%s\n", addr, __func__);
                }
                break;
            }

        }
    }

    sdio_release_host(func);
    if(1 == sus_leave )
    {
        sdio_data_disable(func);
    }

    return err_ret;

}


wf_s32 wf_sdio_tx_flow_agg_num_check(void *phif_info, wf_u8 agg_num,wf_u8 data_type)
{
#if DBG_USE_AGG_NUM
    hif_node_st *hif_info  = phif_info;
    hif_sdio_st *sd         = NULL;
    wf_s32 txAggNum_remain  = 0;

    if(NULL == hif_info)
    {
        return WF_RETURN_FAIL;
    }

    sd         = &hif_info->u.sdio;
    if( data_type != WF_PKT_TYPE_FRAME )
    {
        return WF_RETURN_OK;
    }

    txAggNum_remain = sd->SdioTxOQTFreeSpace - agg_num;
    if (txAggNum_remain < 0)
    {
        return  WF_RETURN_FAIL;
    }
#endif
    return WF_RETURN_OK;
}

wf_s32 wf_sdio_tx_flow_free_pg_check(void *phif_info, wf_u32 hw_queue, wf_u8 pg_num,wf_u8 data_type)
{
    hif_node_st *hif_info      = phif_info;
    hif_sdio_st *sd            = NULL;
    nic_info_st *nic_info      = NULL;
    wf_s32 lpg_remain_num      = 0;
    wf_s32 mpg_remain_num      = 0;
    wf_s32 hpg_remain_num      = 0;

    if(NULL == hif_info)
    {
        return -1;
    }

    sd       = &hif_info->u.sdio;
    nic_info = hif_info->nic_info[0];
    if(data_type != WF_PKT_TYPE_FRAME  )
    {
        return WF_RETURN_OK;
    }

    if(hw_queue == SDIO_HW_QUEUE_LOW)   //LOW
    {
        lpg_remain_num = sd->tx_fifo_lpg_num - pg_num;

        if (lpg_remain_num < 0)
        {
            if ((sd->tx_fifo_ppg_num  + lpg_remain_num) > TX_RESERVED_PG_NUM)
            {
                return WF_RETURN_OK;
            }
            else
            {
                return WF_RETURN_FAIL;
            }
        }
        else
        {
            return WF_RETURN_OK;
        }
    }
    else if (hw_queue == SDIO_HW_QUEUE_MID)    //MID
    {
        mpg_remain_num = sd->tx_fifo_mpg_num - pg_num;

        if (mpg_remain_num < 0)
        {
            if ((sd->tx_fifo_ppg_num  + mpg_remain_num) > TX_RESERVED_PG_NUM)
            {
                return WF_RETURN_OK;
            }
            else
            {
                return WF_RETURN_FAIL;
            }
        }
        else
        {
            return WF_RETURN_OK;
        }
    }
    else                                       // HIGH
    {
        hpg_remain_num = sd->tx_fifo_hpg_num - pg_num;

        if (hpg_remain_num < 0)
        {
            if ((sd->tx_fifo_ppg_num  + hpg_remain_num) > TX_RESERVED_PG_NUM)
            {
                return WF_RETURN_OK;
            }
            else
            {
                return WF_RETURN_FAIL;
            }
        }
        else
        {
            return WF_RETURN_OK;
        }
    }

    return WF_RETURN_FAIL;
}

static wf_s32 wf_sdio_tx_wait_freeAGG(hif_node_st *hif_info,wf_u8 need_agg_num)
{
    hif_sdio_st *sd         = &hif_info->u.sdio;
    nic_info_st *nic_info   = NULL;
    wf_s32 n                = 0;
    wf_s32 ret              = 0;

    nic_info = hif_info->nic_info[0];
    while(sd->SdioTxOQTFreeSpace < need_agg_num)
    {
        if (hm_get_mod_removed() || hif_info->dev_removed)
        {
            return WF_RETURN_FAIL;
        }

        ret = sdio_read_data(sd->func, SDIO_BASE | WL_REG_AC_OQT_FREEPG,&sd->SdioTxOQTFreeSpace,1);
        if(0 != ret)
        {
            if(WF_RETURN_REMOVED_FAIL == ret)
            {
                hif_info->dev_removed = wf_true;
            }
        }
        if ((++n % 60) == 0)
        {
            if ((n % 300) == 0)
            {
                LOG_W("%s(%d): QOT free space(%d), agg_num: %d\n", __func__, n, sd->SdioTxOQTFreeSpace, need_agg_num);

            }
        }
    }

    return 0;
}


static wf_s32 wf_sdio_tx_wait_freePG(hif_node_st *hif_info, wf_u8 hw_queue, wf_u8 need_pg_num)
{

    nic_info_st *nic_info   = NULL;
    wf_u32 value32          = 0;
    wf_s32 ret              = 0;

    hif_sdio_st *sd         = &hif_info->u.sdio;

    nic_info = hif_info->nic_info[0];
    while (1)
    {
        if (hm_get_mod_removed() || hif_info->dev_removed)
        {
            return WF_RETURN_FAIL;
        }

        ret = sdio_read_data(sd->func, SDIO_BASE | WL_REG_PUB_FREEPG,(wf_u8*)&value32,4);
        if(0 != ret )
        {
            if(WF_RETURN_REMOVED_FAIL == ret)
            {
                hif_info->dev_removed = wf_true;
            }
        }
        sd->tx_fifo_ppg_num = value32;

        if (hw_queue == SDIO_HW_QUEUE_HIGH)
        {
            ret = sdio_read_data(sd->func, SDIO_BASE | WL_REG_HIG_FREEPG,(wf_u8*)&value32,4);
            if(0 != ret )
            {
                if(WF_RETURN_REMOVED_FAIL == ret)
                {
                    hif_info->dev_removed = wf_true;
                }
            }
            sd->tx_fifo_hpg_num = value32;

            if (sd->tx_fifo_hpg_num + sd->tx_fifo_ppg_num  > TX_RESERVED_PG_NUM + need_pg_num)
            {
                return WF_RETURN_OK;
            }
        }

        else if (hw_queue == SDIO_HW_QUEUE_MID)
        {
            ret = sdio_read_data(sd->func, SDIO_BASE | WL_REG_MID_FREEPG,(wf_u8*)&value32,4);
            if(0 != ret )
            {
                if(WF_RETURN_REMOVED_FAIL == ret)
                {
                    hif_info->dev_removed = wf_true;
                }
            }
            sd->tx_fifo_mpg_num = value32;
            if (sd->tx_fifo_mpg_num + sd->tx_fifo_ppg_num  > TX_RESERVED_PG_NUM + need_pg_num)
            {
                return WF_RETURN_OK;
            }
        }

        else if(SDIO_HW_QUEUE_LOW == hw_queue)
        {
            ret = sdio_read_data(sd->func, SDIO_BASE | WL_REG_LOW_FREEPG,(wf_u8*)&value32,4);
            if(0 != ret )
            {
                if(WF_RETURN_REMOVED_FAIL == ret)
                {
                    hif_info->dev_removed = wf_true;
                }
            }
            sd->tx_fifo_lpg_num = value32;
            if (sd->tx_fifo_lpg_num + sd->tx_fifo_ppg_num  > TX_RESERVED_PG_NUM + need_pg_num)
            {
                return WF_RETURN_OK;
            }
        }
        else
        {
            LOG_E("[%s,%d] unknown hw_queue:%d",__func__,__LINE__,hw_queue);
        }

    }

}



wf_s32 wf_sdio_tx_flow_agg_num_ctl(void *phif_info, wf_u8 agg_num)
{
#if DBG_USE_AGG_NUM
    hif_node_st *hif_info   = phif_info;
    hif_sdio_st *sd         = NULL;

    if(NULL == hif_info)
    {
        return WF_RETURN_FAIL;
    }

    sd         = &hif_info->u.sdio;
    sd->SdioTxOQTFreeSpace -= agg_num;
#endif
    return WF_RETURN_OK;
}

wf_s32 wf_sdio_tx_flow_free_pg_ctl(void *phif_info, wf_u32 hw_queue, wf_u8 pg_num)
{
    hif_node_st *hif_info   = phif_info;
    hif_sdio_st *sd         = NULL;

    if(NULL == hif_info)
    {
        return -1;
    }

    sd         = &hif_info->u.sdio;
    if(hw_queue == SDIO_HW_QUEUE_LOW)   //LOW
    {
        if (sd->tx_fifo_lpg_num > pg_num)
        {
            sd->tx_fifo_lpg_num = sd->tx_fifo_lpg_num - pg_num;
        }
        else
        {
            sd->tx_fifo_ppg_num = sd->tx_fifo_ppg_num - (pg_num - sd->tx_fifo_lpg_num);
            sd->tx_fifo_lpg_num = 0;
        }
    }
    else if (hw_queue == SDIO_HW_QUEUE_MID)    //MID
    {
        if (sd->tx_fifo_mpg_num > pg_num)
        {
            sd->tx_fifo_mpg_num = sd->tx_fifo_mpg_num - pg_num;
        }
        else
        {
            sd->tx_fifo_ppg_num = sd->tx_fifo_ppg_num - (pg_num - sd->tx_fifo_mpg_num);
            sd->tx_fifo_mpg_num = 0;
        }
    }
    else                                       // HIGH
    {
        if (sd->tx_fifo_hpg_num > pg_num)
        {
            sd->tx_fifo_hpg_num = sd->tx_fifo_hpg_num - pg_num;
        }
        else
        {
            sd->tx_fifo_ppg_num = sd->tx_fifo_ppg_num - (pg_num - sd->tx_fifo_hpg_num);
            sd->tx_fifo_hpg_num = 0;
        }
    }

    return WF_RETURN_OK;
}

wf_s32 wf_sdio_update_txbuf_size(void*phif_info,void *pqnode,wf_s32 *max_page_num,wf_s32 *max_agg_num)
{
    hif_node_st *hif_info       = phif_info;
    hif_sdio_st *sd             = NULL;
    data_queue_mngt_st *dqm     = NULL;
    wf_s32 ret                  = 0;
    wf_u8 fifo_id               = 0;
    data_queue_node_st *qnode   = pqnode;
    wf_u8 need_pg               = 0;
    wf_timer_t timer            = {0};
    if(NULL == hif_info || NULL == qnode)
    {
        return -1;
    }
    if(hif_info->u.sdio.count_start)
    {
        wf_timer_set(&timer, 0);
    }
    dqm = &hif_info->trx_pipe;
    sd = &hif_info->u.sdio;
    //need_pg               = qnode->pg_num>64?qnode->pg_num:64;
    need_pg             = qnode->pg_num;
    fifo_id=sdio_get_fifoaddr_by_que_Index(qnode->addr, 0, &qnode->fifo_addr, 0);
    qnode->hw_queue = sdio_get_hwQueue_by_fifoID(fifo_id);
    ret = wf_sdio_tx_flow_free_pg_check(hif_info,qnode->hw_queue,need_pg,WF_PKT_TYPE_FRAME);
    if (ret == WF_RETURN_FAIL)
    {
        ret = wf_sdio_tx_wait_freePG(hif_info, qnode->hw_queue, need_pg);
    }

    ret = wf_sdio_tx_flow_agg_num_check(hif_info,qnode->agg_num,WF_PKT_TYPE_FRAME);
    if (ret == WF_RETURN_FAIL)
    {
        wf_sdio_tx_wait_freeAGG(hif_info,qnode->agg_num);
    }

    if(SDIO_HW_QUEUE_HIGH == qnode->hw_queue)
    {
        sd->free_tx_page = sd->tx_fifo_ppg_num+sd->tx_fifo_hpg_num;
        if(sd->free_tx_page > 243)
        {
            LOG_W("[%s,%d] free_tx_page:%d,ppg:%d,hpg:%d, need:%d",__func__,__LINE__,sd->free_tx_page,sd->tx_fifo_ppg_num,sd->tx_fifo_hpg_num,need_pg);
            sd->free_tx_page = 243;
        }
    }
    else if(SDIO_HW_QUEUE_MID == qnode->hw_queue)
    {
        sd->free_tx_page = sd->tx_fifo_ppg_num+sd->tx_fifo_mpg_num;
        if(sd->free_tx_page > 233)
        {
            LOG_W("[%s,%d] free_tx_page:%d,ppg:%d,hpg:%d, need:%d",__func__,__LINE__,sd->free_tx_page,sd->tx_fifo_ppg_num,sd->tx_fifo_mpg_num,need_pg);
            sd->free_tx_page = 233;
        }
    }
    else if(SDIO_HW_QUEUE_LOW == qnode->hw_queue)
    {
        sd->free_tx_page = sd->tx_fifo_ppg_num + sd->tx_fifo_lpg_num;
        if(sd->free_tx_page > 233)
        {
            LOG_W("[%s,%d] free_tx_page:%d, ppg:%d,lpg:%d,need:%d",__func__,__LINE__,sd->free_tx_page,sd->tx_fifo_ppg_num,sd->tx_fifo_lpg_num,need_pg);
            sd->free_tx_page = 233;
        }
    }
    else
    {
        if(sd->free_tx_page > 231)
        {
            LOG_W("[%s,%d] free_tx_page:%d,ppg:%d, need:%d",__func__,__LINE__,sd->free_tx_page,sd->tx_fifo_ppg_num,need_pg);
            sd->free_tx_page = 231;
        }
        sd->free_tx_page = sd->tx_fifo_ppg_num;
    }



    *max_page_num = sd->free_tx_page - TX_RESERVED_PG_NUM;
    *max_agg_num  = sd->SdioTxOQTFreeSpace - qnode->agg_num;

    if(hif_info->u.sdio.count_start)
    {
        sd->tx_flow_ctl_time+=wf_timer_elapsed(&timer);
    }

    return 0;
}

static wf_s32 wf_sdio_req_packet(hif_sdio_st * sd, wf_u8 rw, wf_u32 addr, wf_u32 pkt_len, void *pkt)
{
    wf_s32 err_ret = 0;
    wf_bool ready = sdio_operation_is_ok(sd->func);

    if(ready)
    {
        sdio_claim_host(sd->func);
    }

    if (rw)
    {
        err_ret = sdio_memcpy_fromio(sd->func, pkt, addr, pkt_len);
    }
    else
    {
        err_ret = sdio_memcpy_toio(sd->func, addr, pkt, pkt_len);
    }


    if(ready)
    {
        sdio_release_host(sd->func);
    }

    return err_ret;
}

#ifdef CONFIG_SOFT_TX_AGGREGATION
static wf_s32 wf_sdio_write_net_data_agg(hif_node_st *hif_node, wf_u32 addr, wf_u8 * sdata, wf_u32 slen)
{
    data_queue_node_st * qnode    = (data_queue_node_st *)sdata;
    wf_s32 ret                              = 0;
    wf_u32 fifo_addr                        = 0;
    wf_u8 fifo_id                           = 0;
    wf_u32 len4rnd                          = 0;
    wf_timer_t timer                        = {0};

    if(hif_node->u.sdio.count_start)
    {
        wf_timer_set(&timer,0);
    }
    len4rnd = WF_RND4(slen);

    hif_node->trx_pipe.tx_queue_cnt++;

    //if(2!= addr && 5!=addr && 8!=addr)
    //{
    //    LOG_I("[%s] sec:%d,addr:%d",__func__,qnode->encrypt_algo,addr);
    //}
    fifo_id=sdio_get_fifoaddr_by_que_Index(addr, len4rnd, &fifo_addr, 0);
    ret = wf_sdio_req_packet(&hif_node->u.sdio, SDIO_WD, fifo_addr, WF_RND512(len4rnd), qnode->buff);
    if(ret < 0)
    {
        LOG_E("[%s] wf_sdio_req_packet failed,ret=%d, q_sel:%d, fifo_addr:0x%x, data_addr:%p, data_len:%d",
              __func__,ret, addr, fifo_addr, qnode->buff, len4rnd);
    }

    if(hif_node->u.sdio.count_start)
    {
        hif_node->u.sdio.tx_agg_send_time += wf_timer_elapsed(&timer);
    }
    return WF_RETURN_OK;
}
#else
static wf_s32 wf_sdio_write_net_data(hif_node_st *hif_node, wf_u32 addr, wf_u8 * sdata, wf_u32 slen)
{
    data_queue_node_st * data_queue_node    = (data_queue_node_st *)sdata;
    wf_s32 ret                              = 0;
    struct xmit_buf *pxmitbuf               = (struct xmit_buf *)data_queue_node->param;
    wf_s32 pg_num                           = 0;
    wf_u32 fifo_addr                        = 0;
    wf_u8 fifo_id                           = 0;
    wf_u32 len4rnd                          = 0;
    wf_u8 hw_queue                          = 0;
    wf_u8 data_type                         = 0;

    data_queue_node->state = TX_STATE_FLOW_CTL;
    len4rnd = WF_RND4(slen);

    hif_node->trx_pipe.tx_queue_cnt++;

    pg_num = data_queue_node->pg_num;


    // if(2!= addr && 5!=addr && 8!=addr)
    // {
    //     LOG_I("[%s] sec:%d,addr:%d",__func__,pxmitbuf->encrypt_algo,addr);
    // }
    fifo_id=sdio_get_fifoaddr_by_que_Index(addr, len4rnd, &fifo_addr, 0);
    hw_queue = sdio_get_hwQueue_by_fifoID(fifo_id);
#ifdef CONFIG_RICHV200
    data_type = data_queue_node->buff[0]& 0x03;

#else
    data_type = WF_PKT_TYPE_FRAME;
#endif

    ret = wf_sdio_tx_flow_free_pg_check(hif_node,hw_queue,pg_num,data_type);
    if (ret == WF_RETURN_FAIL)
    {
        ret = wf_sdio_tx_wait_freePG(hif_node, hw_queue, pg_num);
        data_queue_node->state = TX_STATE_FLOW_CTL_SECOND;
    }

    ret = wf_sdio_tx_flow_agg_num_check(hif_node,pxmitbuf->agg_num,data_type);
    if (ret == WF_RETURN_FAIL)
    {
        wf_sdio_tx_wait_freeAGG(hif_node,pxmitbuf->agg_num);
        data_queue_node->state = TX_STATE_FLOW_CTL_SECOND;
    }

    data_queue_node->state = TX_STATE_SENDING;
    ret = wf_sdio_req_packet(&hif_node->u.sdio, SDIO_WD, fifo_addr, WF_RND512(len4rnd), data_queue_node->buff);
    if(ret < 0)
    {
        LOG_E("[%s] wf_sdio_req_packet failed,ret=%d, q_sel:%d, fifo_addr:0x%x, data_addr:%p, data_len:%d",
              __func__,ret, addr, fifo_addr, data_queue_node->buff, len4rnd);

        wf_sdio_tx_wait_freePG(hif_node, hw_queue, pg_num);
        wf_sdio_tx_wait_freeAGG(hif_node,pxmitbuf->agg_num);
    }
    else
    {
        wf_sdio_tx_flow_free_pg_ctl(hif_node, hw_queue, pg_num);
        wf_sdio_tx_flow_agg_num_ctl(hif_node, pxmitbuf->agg_num);
    }

    data_queue_node->state = TX_STATE_COMPETE;

    if(data_queue_node->tx_callback_func)
    {
        ret = data_queue_node->tx_callback_func(data_queue_node->tx_info, data_queue_node->param);
        if(wf_true == ret)
        {
            ret = WF_RETURN_OK;
        }
    }
    wf_data_queue_insert(&hif_node->trx_pipe.free_tx_queue, data_queue_node);
    return WF_RETURN_OK;
}


#endif
static wf_s32 wf_sdio_read_net_data(hif_node_st *hif_node, wf_u32 addr, wf_u8 * rdata, wf_u32 rlen)
{
    hif_sdio_st        *sd      = &hif_node->u.sdio;
    struct sk_buff *pskb        = NULL;
    wf_s32 rx_queue_len            = 0;
    wf_u32 read_size               = 0;
    wf_s32 ret                     = -1;
    wf_u32 fifo_addr;

    if((rlen < 16) || rlen > MAX_RXBUF_SZ)
    {
        LOG_E("[%s] rlen error rlen:%d",__func__,rlen);
        return -1;
    }
    hif_node->trx_pipe.rx_queue_cnt++;

    if(rlen>512)
    {
        read_size = WF_RND_MAX(rlen, 512);
    }
    else
    {
        read_size = rlen;
    }

    if(read_size > WF_MAX_RECV_BUFF_LEN_SDIO + HIF_QUEUE_ALLOC_SKB_ALIGN_SZ)
    {
        LOG_E("[%s] read_size(%d) should be less than (%d)",__func__,read_size,WF_MAX_RECV_BUFF_LEN_SDIO + HIF_QUEUE_ALLOC_SKB_ALIGN_SZ);
        while(1);
    }

    pskb = skb_dequeue(&hif_node->trx_pipe.free_rx_queue_skb);
    if(NULL == pskb)
    {
#if 1

        if(hif_node->trx_pipe.alloc_cnt<HIF_MAX_ALLOC_CNT)
        {
            LOG_W("[%s] alloc_skb again",__func__);
            hif_node->trx_pipe.alloc_cnt++;
            wf_hif_queue_alloc_skb(&hif_node->trx_pipe.free_rx_queue_skb,hif_node->hif_type);
        }
        else
        {
            LOG_W("[%s] wf_alloc_skb skip", __func__);
        }
        return -1;
#else
        LOG_E("[%s] skb_dequeue failed",__func__);
        return -1;
#endif
    }
    else
    {
        if (skb_tailroom(pskb) < read_size)
        {
            skb_queue_tail(&hif_node->trx_pipe.free_rx_queue_skb, pskb);
            return -1;
        }
    }

    sdio_get_fifoaddr_by_que_Index(addr, read_size, &fifo_addr, 1);
    ret = wf_sdio_req_packet(sd, SDIO_RD, fifo_addr, read_size, pskb->data);
    if(ret < 0)
    {
        LOG_E("sdio_req_packet error:0x%x",ret);
        if (pskb)
        {
            skb_reset_tail_pointer(pskb);
            pskb->len = 0;
            skb_queue_tail(&hif_node->trx_pipe.free_rx_queue_skb, pskb);
        }
        return -1;
    }

    skb_put(pskb, rlen);

    if (hif_node->nic_info[0] != NULL)
    {
        ret = wf_rx_data_len_check(hif_node->nic_info[0],pskb->data, pskb->len);
    }
    else
    {
        ret = -1;
    }

    if (ret == -1)
    {
        if (pskb)
        {
            skb_reset_tail_pointer(pskb);
            pskb->len = 0;
            skb_queue_tail(&hif_node->trx_pipe.free_rx_queue_skb, pskb);
        }
    }
    else
    {

        if (wf_rx_data_type(pskb->data) == WF_PKT_TYPE_FRAME)
        {
            skb_queue_tail(&hif_node->trx_pipe.rx_queue, pskb);

            rx_queue_len = skb_queue_len(&hif_node->trx_pipe.rx_queue);
            if ( rx_queue_len <= 1)
            {
                wf_tasklet_hi_sched(&hif_node->trx_pipe.recv_task);
            }

            ret = WF_PKT_TYPE_FRAME;
        }
        else
        {
#ifdef CONFIG_RICHV200
            if (wf_rx_cmd_check(pskb->data, pskb->len) == 0)
            {
                switch(wf_rx_data_type(pskb->data))
                {
                    case WF_PKT_TYPE_CMD:
                        wf_hif_bulk_cmd_post(hif_node, pskb->data, pskb->len);
                        ret = WF_PKT_TYPE_CMD;
                        break;

                    case WF_PKT_TYPE_FW:
                        wf_hif_bulk_fw_post(hif_node, pskb->data, pskb->len);
                        ret = WF_PKT_TYPE_FW;
                        break;

                    case WF_PKT_TYPE_REG:
                        wf_hif_bulk_reg_post(hif_node, pskb->data, pskb->len);
                        ret = WF_PKT_TYPE_REG;
                        break;

                    default:
                        LOG_E("recv rxd type error");
                        ret = -1;
                        break;
                }

            }

            if (pskb)
            {
                skb_reset_tail_pointer(pskb);
                pskb->len = 0;
                skb_queue_tail(&hif_node->trx_pipe.free_rx_queue_skb, pskb);
            }

#endif
        }
    }

    return ret;
}


static wf_s32 wf_sdio_read_net_data_irq(hif_node_st *hif_node, wf_u32 addr, wf_u8 * rdata, wf_u32 rlen)
{
    hif_sdio_st        *sd      = &hif_node->u.sdio;
    struct sk_buff *pskb        = NULL;
    wf_s32 rx_queue_len            = 0;
    wf_u32 read_size               = 0;
    wf_s32 ret                     = -1;
    wf_u32 fifo_addr;

    if((rlen < 16) || rlen > MAX_RXBUF_SZ)
    {
        LOG_E("[%s] rlen error rlen:%d",__func__,rlen);
        return -1;
    }
    hif_node->trx_pipe.rx_queue_cnt++;
    if(sd->count_start)
    {
        sd->rx_pkt_num++;
    }

    if(rlen>512)
    {
        read_size = WF_RND_MAX(rlen, 512);
    }
    else
    {
        read_size = rlen;
    }

    if(read_size > WF_MAX_RECV_BUFF_LEN_SDIO + HIF_QUEUE_ALLOC_SKB_ALIGN_SZ)
    {
        LOG_E("[%s] read_size(%d) should be less than (%d)",__func__,read_size,WF_MAX_RECV_BUFF_LEN_SDIO + HIF_QUEUE_ALLOC_SKB_ALIGN_SZ);
        while(1);
    }

    pskb = skb_dequeue(&hif_node->trx_pipe.free_rx_queue_skb);
    if(NULL == pskb)
    {
        if(hif_node->trx_pipe.alloc_cnt<HIF_MAX_ALLOC_CNT)
        {
            LOG_W("[%s] alloc_skb again",__func__);
            hif_node->trx_pipe.alloc_cnt++;
            wf_hif_queue_alloc_skb(&hif_node->trx_pipe.free_rx_queue_skb,hif_node->hif_type);
        }
        else
        {
            LOG_W("[%s] wf_alloc_skb skip", __func__);
        }
        return -1;

    }
    else
    {
        if (skb_tailroom(pskb) < read_size)
        {
            skb_queue_tail(&hif_node->trx_pipe.free_rx_queue_skb, pskb);
            return -1;
        }
    }

    sdio_get_fifoaddr_by_que_Index(addr, read_size, &fifo_addr, 1);
    ret = sdio_memcpy_fromio(sd->func, pskb->data, fifo_addr, read_size);
    if(ret < 0)
    {
        LOG_E("sdio_req_packet error:0x%x",ret);
        if (pskb)
        {
            skb_reset_tail_pointer(pskb);
            pskb->len = 0;
            skb_queue_tail(&hif_node->trx_pipe.free_rx_queue_skb, pskb);
        }
        return -1;
    }

    skb_put(pskb, rlen);

    if (hif_node->nic_info[0] != NULL)
    {
        ret = wf_rx_data_len_check(hif_node->nic_info[0],pskb->data, pskb->len);
    }
    else
    {
        ret = -1;
    }

    if (ret == -1)
    {
        if (pskb)
        {
            skb_reset_tail_pointer(pskb);
            pskb->len = 0;
            skb_queue_tail(&hif_node->trx_pipe.free_rx_queue_skb, pskb);
        }
    }
    else
    {

        if (wf_rx_data_type(pskb->data) == WF_PKT_TYPE_FRAME)
        {
            skb_queue_tail(&hif_node->trx_pipe.rx_queue, pskb);

            rx_queue_len = skb_queue_len(&hif_node->trx_pipe.rx_queue);
            if ( rx_queue_len <= 1)
            {
                wf_tasklet_hi_sched(&hif_node->trx_pipe.recv_task);
            }

            ret = WF_PKT_TYPE_FRAME;
        }
        else
        {
#ifdef CONFIG_RICHV200
            if (wf_rx_cmd_check(pskb->data, pskb->len) == 0)
            {
                switch(wf_rx_data_type(pskb->data))
                {
                    case WF_PKT_TYPE_CMD:
                        wf_hif_bulk_cmd_post(hif_node, pskb->data, pskb->len);
                        ret = WF_PKT_TYPE_CMD;
                        break;

                    case WF_PKT_TYPE_FW:
                        wf_hif_bulk_fw_post(hif_node, pskb->data, pskb->len);
                        ret = WF_PKT_TYPE_FW;
                        break;

                    case WF_PKT_TYPE_REG:
                        wf_hif_bulk_reg_post(hif_node, pskb->data, pskb->len);
                        ret = WF_PKT_TYPE_REG;
                        break;

                    default:
                        LOG_E("recv rxd type error");
                        ret = -1;
                        break;
                }

            }

            if (pskb)
            {
                skb_reset_tail_pointer(pskb);
                pskb->len = 0;
                skb_queue_tail(&hif_node->trx_pipe.free_rx_queue_skb, pskb);
            }

#endif
        }
    }

    return ret;
}


static wf_s32 wf_sdio_write(struct hif_node_ *node, unsigned char flag, wf_u32 addr, char *data, wf_s32 datalen)
{
    wf_s32 ret = 0;
    if(NULL ==node|| 0 == datalen)
    {
        LOG_I("[%s,%d] node:%p,datalen:%d",__func__,__LINE__,node,datalen);
        return -1;
    }
    if(hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
    {
        return -1;
    }
    else
    {
        if(WF_SDIO_TRX_QUEUE_FLAG == flag)
        {
#ifdef CONFIG_SOFT_TX_AGGREGATION
            ret = wf_sdio_write_net_data_agg(node, addr,data,datalen);
#else
            ret = wf_sdio_write_net_data(node, addr,data,datalen);
#endif
        }
        else
        {
            ret = sdio_write_data(node->u.sdio.func,addr,data,datalen);
        }
    }
    return ret;
}

static wf_s32 wf_sdio_read(struct hif_node_ *node, unsigned char flag, unsigned int addr, char *data, wf_s32 datalen)
{
    wf_s32 ret = 0;

    //LOG_I("wf_sdio_read");
    if(hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
    {
        return -1;
    }
    else
    {
        if(WF_SDIO_TRX_QUEUE_FLAG == flag)
        {
            ret = wf_sdio_read_net_data(node,addr,data,datalen);
        }
        else
        {
            ret = sdio_read_data(node->u.sdio.func,addr,data,datalen);
        }
    }
    return ret;
}


static wf_s32 wf_sdio_show(struct hif_node_ *node)
{
    return 0;
}

static wf_s32 wf_sdio_init(struct hif_node_ *node)
{
    LOG_I("wf_sdio_init start");
    LOG_I("sdio_id=%d\n",node->u.sdio.sdio_id);

    node->u.sdio.sdio_id = hm_new_sdio_id(NULL);
    node->u.sdio.block_transfer_len = SDIO_BLK_SIZE;
    node->u.sdio.SdioRxFIFOCnt      = 0;

#ifdef CONFIG_RICHV200
    node->u.sdio.sdio_himr          = 0x10D;
    node->u.sdio.sdio_hisr          = -1;
    node->u.sdio.SysIntrMask        = -1;
#else
    node->u.sdio.sdio_himr          = 1;
    node->u.sdio.sdio_hisr          = 1;
    node->u.sdio.SysIntrMask        = 1;
#endif
    sdio_func_print(node->u.sdio.func);

    LOG_I("wf_sdio_init end");
    return 0;
}

static wf_s32 wf_sdio_deinit(struct hif_node_ *node)
{
    wf_s32 ret = 0;
    LOG_I("wf_sdio_deinit start");
    sdio_func_print(node->u.sdio.func);
    LOG_I("remove sdio_id:%d",node->u.sdio.sdio_id);
    ret = hm_del_sdio_id(node->u.sdio.sdio_id);
    if(ret)
    {
        LOG_E("hm_del_sdio_id(%d) failed",node->u.sdio.sdio_id);
    }
    sdio_claim_host(node->u.sdio.func);
    sdio_disable_func(node->u.sdio.func);
    sdio_release_host(node->u.sdio.func);

    LOG_I("wf_sdio_deinit end");

    return 0;
}


static struct hif_node_ops  sdio_node_ops=
{
    .hif_read           = wf_sdio_read,
    .hif_write          = wf_sdio_write,
    .hif_show           = wf_sdio_show,
    .hif_init           = wf_sdio_init,
    .hif_exit           = wf_sdio_deinit,
    .hif_tx_queue_insert = wf_tx_queue_insert,
    .hif_tx_queue_empty  = wf_tx_queue_empty,
};


#define SDIO_INT_READ_BUG 0

#if 0
static wf_s32 sdio_get_rx_len(struct sdio_func *func)
{
    wf_s32 ret = 0;
    wf_u32 rx_req_len = 0;

    ret = sdio_read_data(func, SDIO_BASE | WL_REG_SZ_RX_REQ, (wf_u8*)&rx_req_len, 4);
    if(ret)
    {
        LOG_E("read rx_req_len error,ret=0x%08x",ret);
        rx_req_len = 0;
    }

    return rx_req_len;
}
#endif

static void sdio_irq_handle(struct sdio_func *func)
{
    hif_node_st  *hif_node = NULL;
    wf_u32 isr = 0;
    wf_u32 isr_clean = 0;
    wf_u32 rx_req_len = 0;
    wf_s32 ret_type;

#if 0
    wf_s32 ret      = 0;

    hif_node = sdio_get_drvdata(func);
    hif_node->u.sdio.irq_cnt++;
    hif_node->u.sdio.int_flag++;

    hif_node->u.sdio.sdio_hisr = 0;

    ret = sdio_read_data(func, SDIO_BASE | WL_REG_HISR, (wf_u8 *)&isr, 4);
    if(0 != ret )
    {
        LOG_E("[%s] read hisr error, ret=0x%08x",__func__,ret);
        return;
    }

    if (isr == 0)
    {
        LOG_E("[%s] irq:0x%x error, check irq",__func__,isr);
        hif_node->u.sdio.int_flag--;
        return;
    }

    hif_node->u.sdio.sdio_hisr = isr;

    if (hif_node->u.sdio.sdio_hisr & WF_BIT(2))
    {
        wf_u32 value = 0;
        LOG_E("[%s] tx dma error!!",__func__);

        sdio_read_data(func, 0x288, (wf_u8 *)&value, 4);
        LOG_I("0x288----0x%x",value);
        sdio_read_data(func, 0x288, (wf_u8 *)&value, 4);
        LOG_I("0x288----0x%x",value);
        sdio_read_data(func, 0x210, (wf_u8 *)&value, 4);
        LOG_I("0x210----0x%x",value);
        sdio_read_data(func, 0x438, (wf_u8 *)&value, 4);
        LOG_I("0x438----0x%x",value);

        sdio_read_data(func, 0x210, (wf_u8 *)&value, 4);
        sdio_write_data(func,0x210, (wf_u8*)&value, 4);

        isr_clean = WF_BIT(2);
        sdio_write_data(func,SDIO_BASE | WL_REG_HISR, (wf_u8*)&isr_clean, 4);
        hif_node->u.sdio.sdio_hisr ^= WF_BIT(2);
    }

    if (hif_node->u.sdio.sdio_hisr & WF_BIT(3))
    {
        wf_u32 value;
        LOG_E("[%s] rx dma error!!",__func__);
        sdio_read_data(func, 0x210, (wf_u8 *)&value, 4);
        LOG_I("0x210----0x%x",value);
        sdio_read_data(func, 0x210, (wf_u8 *)&value, 4);
        LOG_I("0x210----0x%x",value);
        sdio_read_data(func, 0x288, (wf_u8 *)&value, 4);
        LOG_I("0x288----0x%x",value);
        isr_clean = WF_BIT(3);
        sdio_write_data(func,SDIO_BASE | WL_REG_HISR, (wf_u8*)&isr_clean, 4);
        hif_node->u.sdio.sdio_hisr ^= WF_BIT(3);
    }

    if (hif_node->u.sdio.sdio_hisr & (WF_BIT(8)|WF_BIT(0)))
    {
        while(1)
        {
            rx_req_len = sdio_get_rx_len(func);

            if (rx_req_len == 0)
            {
                break;
            }

            if ((rx_req_len < MIN_RXD_SIZE) || (rx_req_len > MAX_RXBUF_SZ) )
            {
                LOG_E("wf_sdio_recv error,rx_req_len:0x%x",rx_req_len);
                break;
            }

            ret_type = wf_sdio_read_net_data(hif_node, READ_QUEUE_INX, NULL, rx_req_len);
            if (ret_type < 0)
            {
                //LOG_E("ret_type:%d",ret_type);
                break;
            }

            if (ret_type != TYPE_DATA)
            {
                isr_clean = WF_BIT(8);  /* clean CMD irq bit*/
                //LOG_I("[1]clean CMD irq bit,rx_req_len:%d,ret_type:0x%x",rx_req_len,ret_type);
                sdio_write_data(func, SDIO_BASE | WL_REG_HISR, (wf_u8*)&isr_clean, 4);
                //LOG_I("[2]clean CMD irq bit,rx_req_len:%d,ret_type:0x%x",rx_req_len,ret_type);
                hif_node->u.sdio.sdio_hisr ^= WF_BIT(8);
            }
        }

        hif_node->u.sdio.sdio_hisr ^= WF_BIT(0);

    }
    hif_node->u.sdio.int_flag--;
#else
    wf_u32 isr_addr     = 0;
    wf_u32 rcv_len_addr     = 0;

    wf_u8  device_id    = 0;
    wf_u16 val_set      = 0;
    wf_s32 err_ret      = 0;
    wf_timer_t timer    = {0};

    sdio_claim_host(func);

    hif_node = sdio_get_drvdata(func);
    hif_node->u.sdio.irq_cnt++;
    hif_node->u.sdio.int_flag++;
    hif_node->u.sdio.current_irq = current;
    hif_node->u.sdio.sdio_hisr = 0;

    isr_addr = sdio_get_destaddr(SDIO_BASE | WL_REG_HISR, &device_id, &val_set);

    isr = sdio_readl(func,isr_addr,&err_ret);
    if (err_ret)
    {
        if(-ENOMEDIUM == err_ret)
        {
            err_ret = WF_RETURN_REMOVED_FAIL;
            LOG_W("[%s,%d] device removed warning.",__func__,__LINE__);
        }
        else
        {
            LOG_E("sderr: Failed to read word, Err: 0x%08x,%s, ret=%d\n", isr_addr, __func__, err_ret);
        }
        sdio_release_host(func);
        return;
    }

    if (isr == 0)
    {
        LOG_E("[%s] irq:0x%x error, check irq",__func__,isr);
        hif_node->u.sdio.int_flag--;
        sdio_release_host(func);
        return;
    }

    hif_node->u.sdio.sdio_hisr = isr;

    /* tx dma err process */
    if (hif_node->u.sdio.sdio_hisr & WF_BIT(2))
    {
        wf_u32 value = 0;
        LOG_E("[%s] tx dma error!!",__func__);
        sdio_read_data(func, 0x210, (wf_u8 *)&value, 4);
        LOG_I("0x210----0x%x",value);

        isr_clean = WF_BIT(2);
        sdio_writel(func, isr_clean, isr_addr, &err_ret);
        if (err_ret < 0)
        {
            if(-ENOMEDIUM == err_ret)
            {
                err_ret = WF_RETURN_REMOVED_FAIL;
                LOG_W("[%s,%d] device removed warning.",__func__,__LINE__);
            }
            else
            {
                LOG_E("sderr: Failed to write word, Err: 0x%08x,%s, ret:%d\n", isr_addr, __func__,err_ret);
            }
            sdio_release_host(func);
            return;
        }
        hif_node->u.sdio.sdio_hisr ^= WF_BIT(2);
    }

    /* rx dma err process */
    if (hif_node->u.sdio.sdio_hisr & WF_BIT(3))
    {
        LOG_E("[%s] rx dma error!!",__func__);

        isr_clean = WF_BIT(3);
        sdio_writel(func, isr_clean, isr_addr, &err_ret);
        if (err_ret < 0)
        {
            if(-ENOMEDIUM == err_ret)
            {
                err_ret = WF_RETURN_REMOVED_FAIL;
                LOG_W("[%s,%d] device removed warning.",__func__,__LINE__);
            }
            else
            {
                LOG_E("sderr: Failed to write word, Err: 0x%08x,%s, ret:%d\n", isr_addr, __func__,err_ret);
            }
            sdio_release_host(func);
            return;
        }
        hif_node->u.sdio.sdio_hisr ^= WF_BIT(3);
    }

    /* rx data process */
    if(hif_node->u.sdio.count_start)
    {
        wf_timer_set(&timer,0);
    }

    if(wf_false == hif_node->trx_pipe.is_init)
    {
        LOG_I("resources is not ready,give up! isr:0x%x",hif_node->u.sdio.sdio_hisr);
        sdio_release_host(func);
        return ;
    }

    if (hif_node->u.sdio.sdio_hisr & (WF_BIT(8)|WF_BIT(0)))
    {
        if(hif_node->u.sdio.count_start)
        {
            hif_node->u.sdio.rx_count++;
        }
        while(1)
        {
            rcv_len_addr = sdio_get_destaddr(SDIO_BASE | WL_REG_SZ_RX_REQ, &device_id, &val_set);
            rx_req_len = sdio_readl(func,rcv_len_addr,&err_ret);
            if (err_ret)
            {
                if(-ENOMEDIUM == err_ret)
                {
                    err_ret = WF_RETURN_REMOVED_FAIL;
                    LOG_W("[%s,%d] device removed warning.",__func__,__LINE__);
                }
                else
                {
                    LOG_E("sderr: Failed to read word, Err: 0x%08x,%s, ret=%d\n", rcv_len_addr, __func__, err_ret);
                }
                sdio_release_host(func);
                return;
            }

            if (rx_req_len == 0)
            {
                break;
            }

            if ((rx_req_len < MIN_RXD_SIZE) || (rx_req_len > MAX_RXBUF_SZ) )
            {
                LOG_E("wf_sdio_recv error,rx_req_len:0x%x",rx_req_len);
                break;
            }

            ret_type = wf_sdio_read_net_data_irq(hif_node, READ_QUEUE_INX, NULL, rx_req_len);
            if (ret_type < 0)
            {
                break;
            }

            if (ret_type != TYPE_DATA)
            {
                isr_clean = WF_BIT(8);  /* clean CMD irq bit*/
                sdio_writel(func, isr_clean, isr_addr, &err_ret);
                hif_node->u.sdio.sdio_hisr ^= WF_BIT(8);
            }
        }

        hif_node->u.sdio.sdio_hisr ^= WF_BIT(0);

    }
    if(hif_node->u.sdio.count_start)
    {
        hif_node->u.sdio.rx_time += wf_timer_elapsed(&timer);
    }
    hif_node->u.sdio.int_flag--;

    sdio_release_host(func);

#endif
}


static wf_s32 sdio_interrupt_register(struct sdio_func *func)
{
    wf_s32 err;

    sdio_claim_host(func);
    err = sdio_claim_irq(func, &sdio_irq_handle);
    if (err < 0)
    {
        LOG_E("[%s] sdio_interrupt_register error ",__func__);
        sdio_release_host(func);
        return err;
    }
    sdio_release_host(func);
    return 0;
}

static void sdio_interrupt_deregister(struct sdio_func *func)
{
    wf_s32 err;
    hif_node_st *node;

    node        = sdio_get_drvdata(func);

    sdio_claim_host(func);
    err = sdio_release_irq(func);
    if(hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
    {
        sdio_release_host(func);
        return ;
    }
    if (err < 0)
    {
        LOG_E("[%s] sdio_interrupt_deregister error ",__func__);
    }

    sdio_release_host(func);
}


static wf_s32 sdio_ctl_init(struct sdio_func *func)
{
    wf_u8  value8       = 0;
    wf_s32 count        = 0;
    wf_s32 initSuccess  = 0;
    wf_s32 ret          = 0;

    LOG_I("[%s] ",__func__);

    sdio_read_data(func, SDIO_BASE|WL_REG_HCTL, &value8, 1);
    value8 &= 0xFE;
    ret = sdio_write_data(func, SDIO_BASE|WL_REG_HCTL, &value8, 1);
    if( ret < 0)
    {
        LOG_E("[%s] 0x903a failed, check!!!",__func__);
        return ret;
    }

    while(1)
    {
        ret = sdio_read_data(func, SDIO_BASE | WL_REG_HCTL, &value8, 1);
        if(0 != ret )
        {
            break;
        }
        if(value8 & WF_BIT(1))
        {
            initSuccess = wf_true;
            break;
        }

        count++;
        if(count > 1000)
        {
            break;
        }
    }

    if(initSuccess == wf_false)
    {
        LOG_E("[%s] failed!!!",__func__);
        return -1;
    }

#if 0

#if 0

    //set bus mode
    value8 = 0x10;
    ret = sdio_write_data(func, 0x07,&value8, 1);
    ret = sdio_read_data(func,0x07,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x07,value8);

    //enable IOE1
    value8 = 0x02;
    ret = sdio_write_data(func, 0x02,&value8, 1);
    ret = sdio_read_data(func,0x02,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x02,value8);


    //wait io_ready
    while(1)
    {
        ret = sdio_read_data(func,0x03,&value8, 1);
        if(0x02 == value8)
        {
            break;
        }
    }

    //set bus mode
    {
        wf_u16 value16      = 0;
        value16 = 0x2810;
        ret = sdio_write_data(func, SDIO_BASE|0x07,(wf_u8*)&value16, 2);
        ret = sdio_read_data(func,SDIO_BASE|0x07,(wf_u8*)&value16, 2);
        LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x07,value16);
    }

    //enable IEN1,IEN0
    value8 = 0x03;
    ret = sdio_write_data(func,SDIO_BASE| 0x04,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x04,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x04,value8);

    //set card capability
    value8 = 0xff;
    ret = sdio_write_data(func, SDIO_BASE|0x08,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x08,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x08,value8);

    //set bus suspend
    value8 = 0x00;
    ret = sdio_write_data(func, SDIO_BASE|0x0c,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x0c,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x0c,value8);

    //set function select
    value8 = 0x00;
    ret = sdio_write_data(func, SDIO_BASE|0x0d,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x0d,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x0d,value8);


    //set function 0 block size
    value8 = 0x01;
    ret = sdio_write_data(func, SDIO_BASE|0x10,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x10,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x10,value8);
    value8 = 0x00;
    ret = sdio_write_data(func,SDIO_BASE| 0x11,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x11,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x11,value8);

    //set powet control
    value8 = 0x00;
    ret = sdio_write_data(func,SDIO_BASE| 0x12,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x12,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x12,value8);

    //set high speed
    value8 = 0x00;
    ret = sdio_write_data(func, SDIO_BASE|0x13,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x13,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x13,value8);

    //set sdio standard function
    value8 = 0x00;
    ret = sdio_write_data(func, SDIO_BASE|0x100,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x100,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x13,value8);

    //set function 1 block size,[8:0]=9'h100
    value8 = 0x00;
    ret = sdio_write_data(func, SDIO_BASE|0x110,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x110,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x110,value8);

    value8 = 0x01;
    ret = sdio_write_data(func, SDIO_BASE|0x111,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x111,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x111,value8);


#endif
    //set int mask timeout value low
    value8 = 0x04;
    ret = sdio_write_data(func, SDIO_BASE|0x9002,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x9002,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x9002,value8);

    //set int mask timeout value high
    value8 = 0x00;
    ret = sdio_write_data(func, SDIO_BASE|0x9003,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x9003,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x9003,value8);

    //enable host int
    value8 = 0xff;
    ret = sdio_write_data(func, SDIO_BASE|0x9004,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x9004,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x9004,value8);

    value8 = 0xcf;
    ret = sdio_write_data(func, SDIO_BASE|0x9034,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x9034,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x9034,value8);

    value8 = 0x1;
    ret = sdio_write_data(func, SDIO_BASE|0x9058,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x9058,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x9058,value8);

    value8 = 0x3;
    ret = sdio_write_data(func, SDIO_BASE|0x9060,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x9060,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x9060,value8);

    value8 = 0x8;
    ret = sdio_write_data(func, SDIO_BASE|0x9048,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x9048,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x9048,value8);

    value8 = 0x3;
    ret = sdio_write_data(func, SDIO_BASE|0x9060,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x9060,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x9060,value8);


#if 0
    value8 = 0xff;
    ret = sdio_write_data(func, SDIO_BASE|0x904c,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x904c,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x904c,value8);

    value8 = 0xff;
    ret = sdio_write_data(func, SDIO_BASE|0x904d,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x904d,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x904d,value8);

    value8 = 0xff;
    ret = sdio_write_data(func, SDIO_BASE|0x904e,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x904e,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x904e,value8);

    value8 = 0xff;
    ret = sdio_write_data(func, SDIO_BASE|0x904f,&value8, 1);
    ret = sdio_read_data(func,SDIO_BASE|0x904f,&value8, 1);
    LOG_I("[%s] reg[0x%x]---0x%x",__func__,0x904f,value8);
#endif
#endif

    return 0;
}


static wf_s32 sdio_speed_set(struct sdio_func *func,wf_u32 hz)
{
    struct mmc_host *host = func->card->host;
    struct mmc_ios *ios = &host->ios;

    LOG_I("sdio speed max=%d,min=%d,set=%d",host->f_max,host->f_min,hz);
    sdio_claim_host(func);
    host->ios.clock = hz;
    host->ops->set_ios(host, ios);
    sdio_release_host(func);

    return 0;

}
static wf_s32 sdio_speed_get(struct sdio_func *func)
{
    struct mmc_host *host = func->card->host;

    LOG_I("sdio speed %d",host->ios.clock);

    return 0;

}

static wf_s32 sdio_func_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
    hif_node_st  *hif_node  = NULL;
    wf_s32 ret              = 0;

    LOG_I("Class=%x",func->class);
    LOG_I("Vendor ID:%x",func->vendor);
    LOG_I("Device ID:%x",func->device);
    LOG_I("Function#:%d",func->num);

    if(sdhz)
    {
        sdio_speed_set(func,sdhz);
    }

    /*set sdio blksize*/
    sdio_func_print(func);
    ret = sdio_func_set_blk(func, SDIO_BLK_SIZE);
    if(ret)
    {
        LOG_E("[%s] sdio_func_set_blk failed",__func__);
        return SDIO_RETRUN_FAIL;
    }

    if (sdio_ctl_init(func) < 0)
    {
        LOG_E("sdio_ctl_init error");
        return -ENODEV;
    }

    hif_node_register(&hif_node,HIF_SDIO,&sdio_node_ops);
    if(NULL == hif_node )
    {
        LOG_E("hif_node_register for HIF_SDIO failed");
        return -ENODEV;
    }

    LOG_I("%p",hif_node);
    hif_node->u.sdio.sdio_hisr_en = 0;
    hif_node->u.sdio.func = func;
    hif_node->u.sdio.irq_cnt = 0;
    sdio_set_drvdata(func,hif_node);

    if(NULL != hif_node->ops->hif_init)
    {
        hif_node->ops->hif_init(hif_node);
    }
    ret = sdio_interrupt_register(hif_node->u.sdio.func);

    if(ret < 0)
    {
        LOG_E("interrupt_register failed");
        return -ENODEV;

    }

    /*insert netdev*/
    if( NULL != hif_node->ops->hif_insert_netdev )
    {
        if(hif_node->ops->hif_insert_netdev(hif_node) < 0)
        {
            LOG_E("hif_insert_netdev error");
            return 0;
        }
    }
    else
    {
        if (hif_dev_insert(hif_node) < 0)
        {
            LOG_E("hif_dev_insert error");
            return 0;
        }
    }
    {
        hif_sdio_st *sd = &hif_node->u.sdio;
        wf_u32 value32;
        wf_u8 value8;

        sdio_read_data(sd->func, SDIO_BASE | WL_REG_PUB_FREEPG,(wf_u8*)&value32,4);
        sd->tx_fifo_ppg_num = value32;
        sdio_read_data(sd->func, SDIO_BASE | WL_REG_HIG_FREEPG,(wf_u8*)&value32,4);
        sd->tx_fifo_hpg_num = value32;
        sdio_read_data(sd->func, SDIO_BASE | WL_REG_MID_FREEPG,(wf_u8*)&value32,4);
        sd->tx_fifo_mpg_num = value32;
        sdio_read_data(sd->func, SDIO_BASE | WL_REG_LOW_FREEPG,(wf_u8*)&value32,4);
        sd->tx_fifo_lpg_num = value32;
        sdio_read_data(sd->func, SDIO_BASE | WL_REG_EXT_FREEPG,(wf_u8*)&value32,4);
        sd->tx_fifo_epg_num = value32;

        LOG_I("ppg_num:%d,hpg_num:%d,mgp_num:%d,lpg_num:%d,epg_num:%d",sd->tx_fifo_ppg_num,sd->tx_fifo_hpg_num,
              sd->tx_fifo_mpg_num,sd->tx_fifo_lpg_num,sd->tx_fifo_epg_num);

        sdio_read_data(sd->func, SDIO_BASE | WL_REG_QUE_PRI_SEL,(wf_u8*)&value8,1);
        if (value8 & BIT(0))
        {
            sd->tx_no_low_queue = wf_false;
            LOG_I("HIGH(fifo_1,fifo_2,fifo_4) MID(fifi_5) LOW(fifo_6)");
        }
        else
        {
            sd->tx_no_low_queue = wf_true;
            LOG_I("HIGH(fifo_1,fifo_2,fifo_4) MID(fifi_5, fifo_6)");
        }

        sdio_read_data(sd->func, SDIO_BASE | WL_REG_AC_OQT_FREEPG, &sd->SdioTxOQTFreeSpace,1);
        LOG_I("SdioTxOQTFreeSpace:%d",sd->SdioTxOQTFreeSpace);

        LOG_I("[%s] end",__func__);
    }

    sdio_speed_get(func);
    return 0;
}

static void sdio_func_remove(struct sdio_func *func)
{
    hif_node_st *node   = sdio_get_drvdata(func);

    LOG_I("*****SDIO REMOVED**********");
    LOG_I("[%s] start",__func__);
    if( NULL != node )
    {
        hif_dev_removed(node);
        sdio_interrupt_deregister(node->u.sdio.func);
        wf_sdio_deinit(node);
        hif_node_unregister(node);
    }
    else
    {
        LOG_E("wf_sdio_func_remove failed");
        return ;
    }


    LOG_I("[%s] end",__func__);
}

static struct sdio_device_id sdio_ids[] =
{
    {SDIO_DEVICE(SDIO_VENDOR_ID_WL, WL_DEVICE_ID_SDIO)}, /* Smart Ass */
    {SDIO_DEVICE(SDIO_VENDOR_ID_WL, SDIO_ID_DARK_MODE)}, /* Fantasy */
    {},
};

MODULE_DEVICE_TABLE(sdio, sdio_ids);

static struct sdio_driver wf_sdio_driver =
{
    .name       = "s9083h",
    .id_table   = sdio_ids,
    .probe      = sdio_func_probe,
    .remove     = sdio_func_remove,
};


wf_s32 sdio_init(void)
{
    wf_s32 ret = 0;
    LOG_I("sdio_init !!!");
    ret = sdio_register_driver(&wf_sdio_driver);
    if(ret != 0)
    {
        LOG_E("sdio_register_driver failed");
    }
    return ret;
}

wf_s32 sdio_exit(void)
{
    LOG_I("sdio_exit !!!");
    sdio_unregister_driver(&wf_sdio_driver);
    return 0;
}



wf_s32 wf_sdioh_interrupt_disable(void *hif_info)
{
    wf_u32 himr = 0;
    hif_sdio_st *sd = &((hif_node_st*)hif_info)->u.sdio;
    if(sd->sdio_hisr_en)
    {
        sdio_write_data(sd->func, SDIO_BASE | WL_REG_HIMR,(wf_u8*)&himr, WORD_LEN);
        sd->sdio_hisr = himr;
        sd->sdio_hisr_en = 0;
    }

    return WF_RETURN_OK;
}

wf_s32 wf_sdioh_interrupt_enable(void *hif_info)
{
    wf_u32 himr;
    hif_sdio_st *sd = &((hif_node_st*)hif_info)->u.sdio;

    himr = cpu_to_le32(sd->sdio_himr);
    sdio_write_data(sd->func, SDIO_BASE | WL_REG_HIMR, (wf_u8*)&himr, WORD_LEN);
    sd->sdio_hisr = himr;
    sd->sdio_hisr_en = 1;
    return WF_RETURN_OK;
}

wf_s32 wf_sdioh_config(void *hif_info)
{
    hif_sdio_st *sd         = &((hif_node_st*)hif_info)->u.sdio;
    unsigned int value32    = 0;
    unsigned char value8    = 0;
    /* need open bulk transport */
    //enable host int
    value32 = 0xFFFFFFFF;
    sdio_write_data(sd->func, SDIO_BASE | 0x9048,(wf_u8 *)&value32, 4);

    value32 = 0xFFFFFFFF;
    sdio_write_data(sd->func, SDIO_BASE | 0x904C,(wf_u8 *)&value32, 4);

#if 0
    sdio_read_data(sd->func,  SDIO_BASE | WL_REG_TXCTL,(wf_u8 *)&value32, 4);
    value32 &= 0xFFF8;
    sdio_write_data(sd->func, SDIO_BASE | WL_REG_TXCTL, (wf_u8 *)&value32, 4);
#endif

    value8 = 0xFF;
    sdio_write_data(sd->func, SDIO_BASE | 0x9068, (wf_u8 *)&value8, 1);

    return 0;
}


