
#include "wf_debug.h"
#include "common.h"
#include "hif.h"

#if 1
#define SDIO_DBG(fmt, ...)      OS_LOG_D("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define SDIO_DBG(fmt, ...)
#endif
#define SDIO_INFO(fmt, ...)     OS_LOG_I("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define SDIO_WARN(fmt, ...)     OS_LOG_W("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define SDIO_ERROR(fmt, ...)    OS_LOG_E("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define SDIO_HW_QUEUE_HIGH  (1)
#define SDIO_HW_QUEUE_MID   (2)
#define SDIO_HW_QUEUE_LOW   (3)

#define SDIO_VENDOR_ID_WL   0x02e7
#define WL_DEVICE_ID_SDIO   0x9086
#define SDIO_BLK_SIZE       512

#define ALIGN_SZ_RXBUFF     8

#define RND4(x)     OS_ALIGN(x, 4)

#define WORD_LEN    4

#define ADDR_CHANGE(addr) ((8 << 13) | ((addr) & 0xFFFF))


static void sdio_interrupt_deregister(sdio_func_t *func);

static OS_RET sdio_func_print(sdio_func_t *func)
{
    SDIO_INFO("func_num:%d, vender:0x%x, device:0x%x, max_blksize:%d, cur_blksize:%d, state:%d",
              (OS_S32)func->num,
              (OS_S32)func->vendor,
              (OS_S32)func->device,
              (OS_S32)func->max_blksize,
              (OS_S32)func->cur_blksize,
              func->state
             );

    return 0;
}

/* to set func->max_blksize, func->cur_blksize*/
static OS_RET sdio_func_set_blk(sdio_func_t *func, unsigned blk_size)
{
    OS_RET ret = OS_ROK;

    sdio_claim_host(func);
    ret = sdio_set_block_size(func, blk_size);
    if (ret)
    {
        SDIO_ERROR("sdio_set_block_size failed");
        sdio_release_host(func);
        return -OS_RERROR;
    }

    ret = sdio_enable_func(func);
    if (ret)
    {
        SDIO_ERROR("sdio_enable_func failed");
        sdio_release_host(func);
        return -OS_RERROR;
    }
    sdio_release_host(func);

    return ret;
}


static OS_U8 sdio_get_dev_id(OS_U32 addr)
{
    OS_U8 dev_id    = 0;
    OS_U16 pdev_id  = 0;

    pdev_id = (OS_U16)(addr >> 16);
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


static OS_U32 sdio_get_destaddr(const OS_U32 src_addr, OS_U8 *p_id, OS_U16 *p_set)
{
    OS_U8 dev_id        = 0;
    OS_U16 val_set      = 0;
    OS_U32 des_addr     = 0;
    OS_S32 is_sdio_id   = 0;

    dev_id = sdio_get_dev_id(src_addr);
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
        if (!is_sdio_id)
        {
            *p_id = dev_id;
        }
        else
        {
            *p_id = 0;
        }
    }
    if (p_set)
    {
        *p_set = val_set;
    }

    return des_addr;
}

static OS_RET sdio_data_disable(sdio_func_t *func)
{
    OS_RET err  = OS_ROK;
    OS_U8 recv  = 0;
    OS_U8 send  = 0;

    sdio_claim_host(func);
    recv = sdio_readb(func, ADDR_CHANGE(WL_REG_HCTL), &err);
    if (err)
    {
        SDIO_ERROR("ERR (%d) addr = 0x%x\n", err, ADDR_CHANGE(WL_REG_HCTL));
        sdio_release_host(func);
        return -OS_RERROR;
    }

    if (!(recv & OS_BIT(1)))
    {
        SDIO_ERROR("recv bit(1) failed");
        sdio_release_host(func);
        return -OS_RERROR;
    }

    send = recv | OS_BIT(0);
    sdio_writeb(func, send, ADDR_CHANGE(WL_REG_HCTL), &err);
    if (err)
    {
        SDIO_ERROR("ERR (%d) addr = 0x%x\n", err, ADDR_CHANGE(WL_REG_HCTL));
        sdio_release_host(func);
        return -OS_RERROR;
    }

    sdio_release_host(func);
    return OS_ROK;
}


static OS_U8 sdio_data_enable(sdio_func_t *func)
{
    OS_RET err          = OS_ROK;
    OS_U8 recv          = 0;
    OS_U8 send          = 0;
    wf_timer_t timer    = {0, 0, 0};

    sdio_claim_host(func);
    recv = sdio_readb(func, ADDR_CHANGE(WL_REG_HCTL), &err);
    if (err)
    {
        SDIO_ERROR("ERR (%d) addr = 0x%x\n", err, ADDR_CHANGE(WL_REG_HCTL));
        sdio_release_host(func);
        return 0;
    }
    if (recv & OS_BIT(1)) //resume
    {
        sdio_release_host(func);
        return OS_ROK;
    }

    send = recv & (~ OS_BIT(0));
    sdio_writeb(func, send, ADDR_CHANGE(WL_REG_HCTL), &err);
    if (err)
    {
        SDIO_ERROR("ERR (%d) addr = 0x%x\n", err, ADDR_CHANGE(WL_REG_HCTL));
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
            SDIO_ERROR("ERR (%d) addr = 0x%x\n", err, ADDR_CHANGE(WL_REG_HCTL));
            sdio_release_host(func);
            return 0;
        }

        if (!err && (recv & OS_BIT(1)))
        {
            break;
        }

        if (wf_timer_expired(&timer))
        {
            SDIO_ERROR("timeout(err:%d) sdh_val:0x%02x\n", err, recv);
            sdio_release_host(func);
            return 0;
        }
    }

    sdio_release_host(func);

    return 1;
}


OS_U8 sdio_get_hwQueue_by_fifoID(OS_U8 fifo_id)
{
    OS_U8 ret = 0;

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


OS_U8 sdio_get_fifoaddr_by_que_Index(OS_U8 queIndex, OS_U32 len,
                                     OS_U32 *fifo_addr, OS_U8 sec)
{
    OS_U8 fifo_id = 0;

    switch(queIndex)
    {
#ifdef CONFIG_RICHV200
        case CMD_QUEUE_INX:
            fifo_id = 3;
            break;
#endif
        case BE_QUEUE_INX:
        case BK_QUEUE_INX:
            if (sec == 1)
            {
                fifo_id = 6;
            }
            else
            {
                fifo_id = 6;
            }
            break;

        case VI_QUEUE_INX:
            fifo_id = 5;
            break;

        case MGT_QUEUE_INX:
        case BCN_QUEUE_INX:
        case VO_QUEUE_INX:
        case HIGH_QUEUE_INX:
            if (sec == 1)
            {
                fifo_id = 1;
            }
            else
            {
                fifo_id = 1;
            }
            break;

        case READ_QUEUE_INX:
            fifo_id = 7;
            break;
        default:
            break;
    }

    if (fifo_id == 7)
    {
        *fifo_addr = (fifo_id << 13) | ((len / 4) & 0x0003);
    }
    else
    {
        *fifo_addr = (fifo_id << 13) | ((len / 4) & 0x1FFF);
    }

    //SDIO_INFO("sdio_get_fifoaddr_by_que_Index -> fifo_id[%d] fifo_addr[0x%x]", fifo_id, *fifo_addr);
    return fifo_id;

}


static OS_RET sdio_write_data(sdio_func_t *func, OS_U32 addr,
                              OS_U8 * data, OS_U32 len)
{
    OS_U32 des_addr     = 0;
    OS_U8 device_id     = 0;
    OS_U16 val_set      = 0;
    OS_U8 sus_leave     = 0;
    OS_RET err_ret      = 0;
    OS_S32 i            = 0;
    hif_node_t *node   = NULL;
    OS_U32 len4rnd      = 0;

    node = sdio_func_priv(func);

    len4rnd = WF_RND4(len);

    if (len <= 4)
    {
        des_addr = sdio_get_destaddr(addr, &device_id, &val_set);
        if (device_id == 8 && val_set < 0x100)
        {
            sus_leave = sdio_data_enable(func);
        }
    }
    else
    {
        //SDIO_DBG("%s, addr = 0x%04x, len=%d", __FUNCTION__, addr, len);
        sdio_get_fifoaddr_by_que_Index(addr, len4rnd, &des_addr, 0);
    }

    // print addr and value
#if 0
    if (len == 1)
    {
        LOG_I("write 0x%02x -> addr[0x%08x], des_addr=[0x%08x]", data[0], addr, des_addr);
    }
    else if (len == 2)
    {
        LOG_I("write 0x%04x -> addr[0x%08x], des_addr=[0x%08x]", *((wf_u16 *)data), addr, des_addr);
    }
    else if (len == 4)
    {
        LOG_I("write 0x%08x -> addr[0x%08x], des_addr=[0x%08x]", *((wf_u32 *)data), addr, des_addr);
    }
    else
    {
        LOG_I("write %d len bytes-> addr[0x%08x], des_addr=[0x%08x]", len, addr, des_addr);
    }
#endif
    // end of print
    sdio_claim_host(func);
    if (WORD_LEN == len)
    {
        OS_U32 data_value = *(OS_U32*)data;
        if (hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
        {
            sdio_release_host(func);
            return -1;
        }
        sdio_writel(func, data_value, des_addr, &err_ret);
        if (err_ret < 0)
        {
            if (HAL_ERROR == err_ret)
            {
                err_ret = WF_RETURN_REMOVED_FAIL;
                SDIO_WARN("device removed warning");
            }
            else
            {
                SDIO_ERROR("sderr: Failed to write word, Err: 0x%08x, ret:%d\n", addr, err_ret);
            }
        }
    }
    else if (WORD_LEN < len)
    {
#if 0
        printk("int sdio_write_data, printk data:len=%d\n", len);
        for(i = 0; i < len; i++)
        {
            printk("0x%02x, ", data[i]);
            if ((i+1)%16 == 0)
                printk("\n");
        }
        printk("\nend\n");
#endif
        err_ret = sdio_memcpy_toio(func, des_addr, data, len);
        if (err_ret < 0)
        {
            if (HAL_ERROR == err_ret)
            {
                err_ret = WF_RETURN_REMOVED_FAIL;
                SDIO_WARN("device removed warning.");
            }
            else
            {
                SDIO_ERROR("sderr: sdio_memcpy_toio, Err: 0x%08x, ret:%d\n", addr, err_ret);
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
                if (HAL_ERROR == err_ret)
                {
                    err_ret = WF_RETURN_REMOVED_FAIL;
                    SDIO_WARN("device removed warning.");
                }
                else
                {
                    SDIO_ERROR("sderr: Failed to write byte, Err: 0x%08x", addr);
                }
                break;
            }
        }
    }
    sdio_release_host(func);

    if (1 == sus_leave)
    {
        sdio_data_disable(func);
        //LOG_I();
    }

    return err_ret;
}

OS_RET sdio_read_data(sdio_func_t *func, OS_U32 addr, OS_U8 * data, OS_U32 len)
{
    OS_U32 des_addr     = 0;
    OS_U8 device_id     = 0;
    OS_U16 val_set      = 0;
    OS_U8 sus_leave     = 0;

    OS_U32 value32      = 0;
    OS_RET err_ret      = 0;
    OS_S32 i            = 0;
    OS_U32 len4rnd      = 0;

    if (len <= 4)
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
        sdio_get_fifoaddr_by_que_Index(READ_QUEUE_INX, len4rnd, &des_addr, 0);
    }

    sdio_claim_host(func);
    if (WORD_LEN == len)
    {
        value32 = sdio_readl(func, des_addr, &err_ret);
        if (err_ret)
        {
            if (HAL_ERROR == err_ret)
            {
                err_ret = WF_RETURN_REMOVED_FAIL;
                SDIO_WARN("device removed warning.");
            }
            else
            {
                SDIO_ERROR("sderr: Failed to read word, Err: 0x%08x, ret=%d\n", addr, err_ret);
            }
        }
        else
        {
            for (i = 0; i < len; i++)
            {
                data[i] = ((OS_U8*)&value32)[i];
            }
        }
    }
    else if (WORD_LEN < len)
    {
        //SDIO_DBG("in sdio_read_data, dest_addr = 0x%04x, len=%d", des_addr, len);
        err_ret = sdio_memcpy_fromio(func, data, des_addr, len);
        if (err_ret < 0)
        {

            if (HAL_ERROR == err_ret)
            {
                err_ret = WF_RETURN_REMOVED_FAIL;
                SDIO_WARN("device removed warning.");
            }
            else
            {
                SDIO_ERROR("sderr: sdio_memcpy_fromio, Err: 0x%08x, ret:%d\n", addr, err_ret);
            }
        }
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            data[i] = sdio_readb(func, des_addr + i, &err_ret);
            //SDIO_INFO("read addr[0x%08x]=0x%02x, des_addr = 0x%08x", addr+i, data[i], des_addr+i);
            if (err_ret)
            {
                if (HAL_ERROR == err_ret)
                {
                    err_ret = WF_RETURN_REMOVED_FAIL;
                    SDIO_WARN("device removed warning.");

                }
                else
                {
                    SDIO_ERROR("sderr: Failed to read byte, Err: 0x%08x, ", addr);
                }
                break;
            }

        }
    }
    sdio_release_host(func);
    if (1 == sus_leave)
    {
        sdio_data_disable(func);
    }

    return err_ret;

}


#ifdef CONFIG_RICHV200
static OS_RET wf_sdio_tx_flow_agg_num_check(hif_node_t *hif_info, OS_U8 agg_num, OS_U8 data_type)
{
    hif_sdio_t *sd         = &hif_info->u.sdio;
    OS_S32 txAggNum_remain  = 0;

    if (data_type != WF_PKT_TYPE_FRAME )
    {
        return WF_RETURN_OK;
    }

    txAggNum_remain = sd->SdioTxOQTFreeSpace - agg_num;
    if (txAggNum_remain < 0)
    {
        return  WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

static OS_RET wf_sdio_tx_flow_free_pg_check(hif_node_t *hif_info, OS_U32 hw_queue, OS_U8 pg_num, OS_U8 data_type)
{
    hif_sdio_t *sd            = &hif_info->u.sdio;
    nic_info_st *nic_info      = NULL;
    OS_S32 lpg_remain_num      = 0;
    OS_S32 mpg_remain_num      = 0;
    OS_S32 hpg_remain_num      = 0;



    nic_info = hif_info->nic_info[0];
    if (data_type != WF_PKT_TYPE_FRAME )
    {
        return WF_RETURN_OK;
    }

    if (hw_queue == SDIO_HW_QUEUE_LOW)   //LOW
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

#else
static OS_U32 sdio_get_ac_index_by_qsel(OS_U8 qsel, OS_U8 sec)
{
    OS_U32 addr = 0;
    if (sec)
    {
        SDIO_INFO("sec:%d", sec);
        switch (qsel)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                addr = AC5_IDX;
                break;
            case 4:
            case 5:
                addr = AC4_IDX;
                break;
            case 6:
            case 7:
                addr = AC3_IDX;
                break;
            case QSLT_BEACON:
                addr = AC3_IDX;
                break;
            case QSLT_HIGH:
                addr = AC3_IDX;
                break;
            case QSLT_MGNT:
            default:
                addr = AC3_IDX;
                break;
        }
    }
    else if (0 == sec)
    {
        switch (qsel)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                addr = AC3_IDX;
                break;
            case 4:
            case 5:
                addr = AC4_IDX;
                break;
            case 6:
            case 7:
                addr = AC5_IDX;
                break;
            case QSLT_BEACON:
                addr = AC5_IDX;
                break;
            case QSLT_HIGH:
                addr = AC5_IDX;
                break;
            case QSLT_MGNT:
            default:
                addr = AC5_IDX;
                break;
        }

    }

    return addr;

}

static OS_RET wf_sdio_wait_enough_txoqt_space(hif_node_t *hif_info, OS_U32 PageIdx, OS_U8 pg_num)
{
    hif_sdio_t *sd                 = &hif_info->u.sdio;
    nic_info_st *nic_info           = NULL;
    OS_U8 DedicatedPgNum            = 0;
    OS_U8 RequiredPublicFreePgNum   = 0;
    OS_S32 i                        = 0;
    OS_RET ret                      = 0;

    nic_info = hif_info->nic_info[0];
    //SDIO_INFO("PageIdx:%d", PageIdx);
    while (sd->SdioTxFIFOFreePage[PageIdx] +  sd->SdioTxFIFOFreePage[ACX_IDX] <= TX_RESERVED_PG_NUM + pg_num)
    {
        if (nic_info->is_driver_stopped || nic_info->is_surprise_removed)
        {
            return WF_RETURN_FAIL;
        }

        for (i = 0; i < 6; i++)
        {
            ret = sdio_read_data(sd->func, SDIO_BASE | (WL_REG_AC0_FREEPG + i * 4), &sd->SdioTxFIFOFreePage[i], 1);
            if (0 != ret)
            {
                if (HAL_ERROR == ret)
                {
                    nic_info->is_surprise_removed = wf_true;
                }
            }
        }
    }

    DedicatedPgNum = sd->SdioTxFIFOFreePage[PageIdx];
    if (pg_num <= DedicatedPgNum)
    {
        sd->SdioTxFIFOFreePage[PageIdx] -= pg_num;
    }
    else
    {
        sd->SdioTxFIFOFreePage[PageIdx] = 0;
        RequiredPublicFreePgNum = pg_num - DedicatedPgNum;
        sd->SdioTxFIFOFreePage[ACX_IDX] -= RequiredPublicFreePgNum;
    }

    return WF_RETURN_OK;
}

#endif

static OS_RET wf_sdio_tx_wait_freeAGG(hif_node_t *hif_info, OS_U8 need_agg_num)
{
    hif_sdio_t *sd         = &hif_info->u.sdio;
    nic_info_st *nic_info   = NULL;
    OS_S32 n                = 0;
    OS_RET ret              = 0;

    nic_info = hif_info->nic_info[0];
    while(sd->SdioTxOQTFreeSpace < need_agg_num)
    {
        if (nic_info->is_driver_stopped || nic_info->is_surprise_removed)
        {
            return WF_RETURN_FAIL;
        }

        ret = sdio_read_data(sd->func, SDIO_BASE | WL_REG_AC_OQT_FREEPG, &sd->SdioTxOQTFreeSpace, 1);
        if (0 != ret)
        {
            if (HAL_ERROR == ret)
            {
                nic_info->is_surprise_removed = wf_true;
            }
        }
        if ((++n % 60) == 0)
        {
            if ((n % 300) == 0)
            {
                SDIO_WARN("(%d): QOT free space(%d), agg_num: %d\n", n, sd->SdioTxOQTFreeSpace, need_agg_num);
            }
        }
    }

    return 0;
}
static OS_RET wf_sdio_tx_wait_freePG(hif_node_t *hif_info, OS_U8 hw_queue, OS_U8 need_pg_num)
{
    nic_info_st *nic_info   = NULL;
    OS_U32 value32          = 0;
    OS_RET ret              = 0;

    hif_sdio_t *sd         = &hif_info->u.sdio;

    nic_info = hif_info->nic_info[0];
    while (1)
    {
        if (nic_info->is_driver_stopped || nic_info->is_surprise_removed)
        {
            return WF_RETURN_FAIL;
        }

        ret = sdio_read_data(sd->func, SDIO_BASE | WL_REG_PUB_FREEPG, (OS_U8*)&value32, 4);
        if (0 != ret)
        {
            if (HAL_ERROR == ret)
            {
                nic_info->is_surprise_removed = wf_true;
            }
        }
        sd->tx_fifo_ppg_num = value32;

        ret = sdio_read_data(sd->func, SDIO_BASE | WL_REG_HIG_FREEPG, (OS_U8*)&value32, 4);
        if (0 != ret)
        {
            if (HAL_ERROR == ret)
            {
                nic_info->is_surprise_removed = wf_true;
            }
        }
        sd->tx_fifo_hpg_num = value32;

        ret = sdio_read_data(sd->func, SDIO_BASE | WL_REG_MID_FREEPG, (OS_U8*)&value32, 4);
        if (0 != ret)
        {
            if (HAL_ERROR == ret)
            {
                nic_info->is_surprise_removed = wf_true;
            }
        }
        sd->tx_fifo_mpg_num = value32;

        ret = sdio_read_data(sd->func, SDIO_BASE | WL_REG_LOW_FREEPG, (OS_U8*)&value32, 4);
        if (0 != ret)
        {
            if (HAL_ERROR == ret)
            {
                nic_info->is_surprise_removed = wf_true;
            }
        }
        sd->tx_fifo_lpg_num = value32;

        if (hw_queue == SDIO_HW_QUEUE_HIGH) // HIGH
        {
            if (sd->tx_fifo_hpg_num + sd->tx_fifo_ppg_num  > TX_RESERVED_PG_NUM + need_pg_num)
            {
                return WF_RETURN_OK;
            }
        }
        else if (hw_queue == SDIO_HW_QUEUE_MID) // MID
        {
            if (sd->tx_fifo_mpg_num + sd->tx_fifo_ppg_num  > TX_RESERVED_PG_NUM + need_pg_num)
            {
                return WF_RETURN_OK;
            }
        }
        else   // LOW
        {
            if (sd->tx_fifo_lpg_num + sd->tx_fifo_ppg_num  > TX_RESERVED_PG_NUM + need_pg_num)
            {
                return WF_RETURN_OK;
            }
        }

    };

    return  WF_RETURN_FAIL;

}



static OS_RET wf_sdio_tx_flow_agg_num_ctl(hif_node_t *hif_info, OS_U8 agg_num)
{
    hif_sdio_t *sd         = &hif_info->u.sdio;

    sd->SdioTxOQTFreeSpace -= agg_num;

    return WF_RETURN_OK;
}

static OS_RET wf_sdio_tx_flow_free_pg_ctl(hif_node_t *hif_info, OS_U32 hw_queue, OS_U8 pg_num)
{
    hif_sdio_t *sd         = &hif_info->u.sdio;

    if (hw_queue == SDIO_HW_QUEUE_LOW)   //LOW
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

static OS_RET wf_sdio_req_packet(hif_sdio_t * sd, OS_U8 rw, OS_U32 addr, OS_U32 pkt_len, void *pkt)
{
    OS_RET err_ret = 0;
    sdio_claim_host(sd->func);
    if (rw)
    {
        err_ret = sdio_memcpy_fromio(sd->func, pkt, addr, pkt_len);
    }
    else
    {
        err_ret = sdio_memcpy_toio(sd->func, addr, pkt, pkt_len);
    }
    sdio_release_host(sd->func);
    return err_ret;
}


static OS_RET wf_sdio_write_net_data(hif_node_t *hif_node, OS_U32 addr,
                                     OS_U8 * sdata, OS_U32 slen)
{
    data_queue_node_t *data_queue_node      = (data_queue_node_t *)sdata;
    OS_RET ret                              = 0;
    struct xmit_buf *pxmitbuf               = (struct xmit_buf *)data_queue_node->param;
    OS_RET pg_num                           = 0;
    OS_U32 fifo_addr                        = 0;
    OS_U8 fifo_id                           = 0;
    OS_U32 len4rnd                          = 0;
    OS_U8 hw_queue                          = 0;
#ifdef CONFIG_RICHV200
    OS_U8 data_type                         = 0;
#else
    OS_U32 page_idx                         = 0;
#endif
    data_queue_node->state = TX_STATE_FLOW_CTL;
    len4rnd = WF_RND4(slen);

    pg_num = pxmitbuf->pg_num;

    if (2!= addr && 5 != addr && 8 != addr)
    {
        SDIO_INFO("sec:%d, addr:%d", pxmitbuf->encrypt_algo, addr);
    }
    fifo_id = sdio_get_fifoaddr_by_que_Index(addr, len4rnd, &fifo_addr, pxmitbuf->encrypt_algo);
    hw_queue = sdio_get_hwQueue_by_fifoID(fifo_id);

#ifdef CONFIG_RICHV200
    data_type = data_queue_node->buff[0]& 0x03;
    ret = wf_sdio_tx_flow_free_pg_check(hif_node, hw_queue, pg_num, data_type);
    if (ret == WF_RETURN_FAIL)
    {
        ret = wf_sdio_tx_wait_freePG(hif_node, hw_queue, pg_num);
        data_queue_node->state = TX_STATE_FLOW_CTL_SECOND;
    }

    ret = wf_sdio_tx_flow_agg_num_check(hif_node, pxmitbuf->agg_num, data_type);
    if (ret == WF_RETURN_FAIL)
    {
        wf_sdio_tx_wait_freeAGG(hif_node, pxmitbuf->agg_num);
        data_queue_node->state = TX_STATE_FLOW_CTL_SECOND;
    }
#else
    //page_idx =sdio_get_ac_page_by_que_index(addr, pxmitbuf->encrypt_algo);
    page_idx = sdio_get_ac_index_by_qsel(pxmitbuf->qsel, pxmitbuf->encrypt_algo);
    ret = wf_sdio_wait_enough_txoqt_space(hif_node, page_idx, pg_num);
#endif
    data_queue_node->state = TX_STATE_SENDING;
    ret = wf_sdio_req_packet(&hif_node->u.sdio, SDIO_WD, fifo_addr, len4rnd, data_queue_node->buff);
    if (ret < 0)
    {
        SDIO_ERROR("wf_sdio_req_packet failed, ret=%d, q_sel:%d, fifo_addr:0x%x, data_addr:%p, data_len:%d",
                   ret, addr, fifo_addr, data_queue_node->buff, len4rnd);

        wf_sdio_tx_wait_freePG(hif_node, hw_queue, pg_num);
        wf_sdio_tx_wait_freeAGG(hif_node, pxmitbuf->agg_num);
    }
    else
    {
        wf_sdio_tx_flow_free_pg_ctl(hif_node, hw_queue, pg_num);
        wf_sdio_tx_flow_agg_num_ctl(hif_node, pxmitbuf->agg_num);
    }

    data_queue_node->state = TX_STATE_COMPETE;

    if (data_queue_node->tx_callback_func)
    {
        ret = data_queue_node->tx_callback_func(data_queue_node->tx_info, data_queue_node->param);
        if (wf_true == ret)
        {
            ret = WF_RETURN_OK;
        }
    }

    return WF_RETURN_OK;
}

static OS_RET wf_sdio_read_net_data(hif_node_t *hif_node, OS_U32 addr,
                                    OS_U8 * rdata, OS_U32 rlen)
{
    hif_sdio_t *sd              = &hif_node->u.sdio;
    hif_netbuf_t *netbuf_node   = NULL;
    OS_S32 rx_queue_len         = 0;
    OS_U32 read_size            = 0;
    OS_RET ret                  = -1;
    OS_U32 fifo_addr;

    if ((rlen < 16) || rlen > MAX_RXBUF_SZ)
    {
        SDIO_ERROR("rlen error len:%d", rlen);
        return -1;
    }
    hif_node->trx_pipe.rx_queue_cnt++;

    if (rlen > 512)
    {
        read_size = WF_RND_MAX(rlen, 512);
    }
    else
    {
        read_size = rlen;
    }

    if (read_size > WF_MAX_RECV_BUFF_LEN_SDIO)
    {
        SDIO_ERROR("read_size(%d) should be less than (%d)",
                   read_size, WF_MAX_RECV_BUFF_LEN_SDIO + HIF_QUEUE_ALLOC_MEM_ALIGN_SZ);
        OS_BUG();
    }

    while (1)
    {
        netbuf_node = wf_netbuf_queue_remove(&hif_node->trx_pipe.free_netbuf_queue);
        if (NULL == netbuf_node)
        {
            SDIO_DBG("node num:%d", hif_node->trx_pipe.free_netbuf_queue.cnt);
            SDIO_DBG("netbuf_queue, node num:%d", wf_que_count(&hif_node->trx_pipe.netbuf_queue));
            /* must clear the buffer */
            static wf_u32 sdio_skip_data[WF_MAX_RECV_BUFF_LEN_SDIO / 4];
            sdio_get_fifoaddr_by_que_Index(addr, read_size, &fifo_addr, 0);
            wf_sdio_req_packet(sd, SDIO_RD, fifo_addr, read_size, sdio_skip_data);
            hif_node->trx_pipe.rx_wq.ops->workqueue_work(&hif_node->trx_pipe.rx_wq);

            SDIO_WARN("There is no netbuf for recv data");
            return -1;
        }

        if (netbuf_node->len != 0)
        {
            SDIO_ERROR("netbuf len is not zero, no vaild");
            netbuf_node->len = 0;
            wf_enque_tail(&netbuf_node->node, &hif_node->trx_pipe.free_netbuf_queue);
            return -1;
        }
        break;
    }

    sdio_get_fifoaddr_by_que_Index(addr, read_size, &fifo_addr, 0);
    ret = wf_sdio_req_packet(sd, SDIO_RD, fifo_addr, read_size, netbuf_node->payload);
    if (ret < 0)
    {
        SDIO_ERROR("sdio_req_packet error:0x%x", ret);
        if (NULL != netbuf_node)
        {
            netbuf_node->len = 0;
            wf_enque_tail(&netbuf_node->node, &hif_node->trx_pipe.free_netbuf_queue);
            return -1;
        }
        return -1;
    }

    netbuf_node->len = rlen;
    if (hif_node->nic_info[0] != NULL)
    {
        ret = wf_rx_data_len_check(hif_node->nic_info[0], netbuf_node->payload, netbuf_node->len);
    }
    else
    {
        ret = -1;
    }

    if (ret == -1)
    {
        if (NULL != netbuf_node)
        {
            netbuf_node->len = 0;
            wf_enque_tail(&netbuf_node->node, &hif_node->trx_pipe.free_netbuf_queue);
            return -1;
        }
    }
    else
    {

        if (wf_rx_data_type(netbuf_node->payload) == WF_PKT_TYPE_FRAME)
        {
            wf_enque_tail(&netbuf_node->node, &hif_node->trx_pipe.netbuf_queue);

            if (wf_que_count(&hif_node->trx_pipe.netbuf_queue) <= 1)
            {
                hif_node->trx_pipe.rx_wq.ops->workqueue_work(&hif_node->trx_pipe.rx_wq);
            }

            ret = WF_PKT_TYPE_FRAME;
        }
        else
        {
#ifdef CONFIG_RICHV200
            if (wf_rx_cmd_check(netbuf_node->payload, netbuf_node->len) == 0)
            {
                switch(wf_rx_data_type(netbuf_node->payload))
                {
                    case WF_PKT_TYPE_CMD:
                        wf_hif_bulk_cmd_post(hif_node, netbuf_node->payload, netbuf_node->len);
                        ret = WF_PKT_TYPE_CMD;
                        break;

                    case WF_PKT_TYPE_FW:
                        wf_hif_bulk_fw_post(hif_node, netbuf_node->payload, netbuf_node->len);
                        ret = WF_PKT_TYPE_FW;
                        break;

                    case WF_PKT_TYPE_REG:
                        wf_hif_bulk_reg_post(hif_node, netbuf_node->payload, netbuf_node->len);
                        ret = WF_PKT_TYPE_REG;
                        break;

                    default:
                        SDIO_ERROR("recv rxd type error");
                        ret = -1;
                        break;
                }

            }
            if (NULL != netbuf_node)
            {
                netbuf_node->len = 0;
                wf_enque_tail(&netbuf_node->node, &hif_node->trx_pipe.free_netbuf_queue);
            }
#endif
        }
    }

    return ret;
}

static OS_RET wf_sdio_write(hif_node_t *node, unsigned char flag, OS_U32 addr, char *data, OS_S32 datalen)
{
    OS_RET ret = 0;
    if (NULL ==node|| 0 == datalen)
    {
        SDIO_INFO("node null, datalen:%d", datalen);
        return -1;
    }
    if (hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
    {
        return -1;
    }
    else
    {
        if (WF_SDIO_TRX_QUEUE_FLAG == flag)
        {
            ret = wf_sdio_write_net_data(node, addr, (wf_u8 *)data, datalen);
        }
        else
        {
            ret = sdio_write_data(node->u.sdio.func, addr, (OS_U8 *)data, datalen);
        }
    }
    return ret;
}

static OS_RET wf_sdio_read(hif_node_t *node, unsigned char flag, unsigned int addr, char *data, OS_S32 datalen)
{
    OS_RET ret = 0;

    //SDIO_INFO("wf_sdio_read");
    if (hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
    {
        return -1;
    }
    else
    {
        if (WF_SDIO_TRX_QUEUE_FLAG == flag)
        {
            ret = wf_sdio_read_net_data(node, addr, (wf_u8 *)data, datalen);
        }
        else
        {
            ret = sdio_read_data(node->u.sdio.func, addr, (OS_U8 *)data, datalen);
        }
    }
    return ret;
}


static OS_RET wf_sdio_show(hif_node_t *node)
{
    return 0;
}

static OS_RET wf_sdio_init(hif_node_t *node)
{
    SDIO_INFO("wf_sdio_init start");
    SDIO_INFO("sdio_id=%d\n", node->u.sdio.sdio_id);

    node->u.sdio.sdio_id            = hm_new_sdio_id(NULL);
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

    SDIO_INFO("wf_sdio_init end");
    return 0;
}

static OS_RET wf_sdio_deinit(hif_node_t *node)
{
    OS_RET ret = 0;
    SDIO_INFO("wf_sdio_deinit start");
    sdio_func_print(node->u.sdio.func);
    SDIO_INFO("remove sdio_id:%d", node->u.sdio.sdio_id);
    ret = hm_del_sdio_id(node->u.sdio.sdio_id);
    if (ret)
    {
        SDIO_ERROR("hm_del_sdio_id(%d) failed", node->u.sdio.sdio_id);
    }
    sdio_claim_host(node->u.sdio.func);
    sdio_disable_func(node->u.sdio.func);
    sdio_release_host(node->u.sdio.func);

    SDIO_INFO("wf_sdio_deinit end");

    return 0;
}


static hif_node_ops_t  sdio_node_ops=
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

static OS_RET sdio_get_rx_len(sdio_func_t *func)
{
    OS_RET ret = 0;
    OS_U32 rx_req_len = 0;

    ret = sdio_read_data(func, SDIO_BASE | WL_REG_SZ_RX_REQ, (OS_U8*)&rx_req_len, 4);
    if (ret)
    {
        SDIO_ERROR("read rx_req_len error, ret = 0x%08x", ret);
        rx_req_len = 0;
    }

    return rx_req_len;
}

static void sdio_irq_handle(sdio_func_t *func)
{
    hif_node_t *hif_node = NULL;
    OS_U32 isr;
    OS_U32 isr_clean;
    OS_RET ret         = 0;
    OS_U32 rx_req_len = 0;
    OS_S32 ret_type;

    hif_node = sdio_func_priv(func);
    hif_node->u.sdio.irq_cnt++;
    hif_node->u.sdio.int_flag++;

    hif_node->u.sdio.sdio_hisr = 0;

    ret = sdio_read_data(func, SDIO_BASE | WL_REG_HISR, (OS_U8 *)&isr, 4);
    if (ret)
    {
        SDIO_ERROR("read hisr error, ret = 0x%08x", ret);
        return;
    }

    if (isr == 0)
    {
        SDIO_ERROR("irq:0x%x error, check irq", isr);
        hif_node->u.sdio.int_flag--;
        return;
    }

    hif_node->u.sdio.sdio_hisr = isr;

    if (hif_node->u.sdio.sdio_hisr & WF_BIT(2))
    {
        OS_U32 value = 0;
        SDIO_ERROR("tx dma error!!");

        sdio_read_data(func, 0x288, (OS_U8 *)&value, 4);
        SDIO_INFO("0x288----0x%x", value);
        sdio_read_data(func, 0x288, (OS_U8 *)&value, 4);
        SDIO_INFO("0x288----0x%x", value);
        sdio_read_data(func, 0x210, (OS_U8 *)&value, 4);
        SDIO_INFO("0x210----0x%x", value);
        sdio_read_data(func, 0x438, (OS_U8 *)&value, 4);
        SDIO_INFO("0x438----0x%x", value);

        sdio_read_data(func, 0x210, (OS_U8 *)&value, 4);
        sdio_write_data(func, 0x210, (OS_U8*)&value, 4);

        isr_clean = WF_BIT(2);
        sdio_write_data(func, SDIO_BASE | WL_REG_HISR, (OS_U8*)&isr_clean, 4);
        hif_node->u.sdio.sdio_hisr ^= WF_BIT(2);
    }

    if (hif_node->u.sdio.sdio_hisr & WF_BIT(3))
    {
        OS_U32 value;
        SDIO_ERROR("rx dma error!!");
        sdio_read_data(func, 0x210, (OS_U8 *)&value, 4);
        SDIO_INFO("0x210----0x%x", value);
        sdio_read_data(func, 0x210, (OS_U8 *)&value, 4);
        SDIO_INFO("0x210----0x%x", value);
        sdio_read_data(func, 0x288, (OS_U8 *)&value, 4);
        SDIO_INFO("0x288----0x%x", value);
        sdio_read_data(func, 0x290, (OS_U8 *)&value, 4);
        SDIO_INFO("0x290----0x%x", value);
        sdio_read_data(func, 0x10c, (OS_U8 *)&value, 4);
        SDIO_INFO("0x10c----0x%x", value);
        isr_clean = WF_BIT(3);
        sdio_write_data(func, SDIO_BASE | WL_REG_HISR, (OS_U8*)&isr_clean, 4);
        hif_node->u.sdio.sdio_hisr ^= WF_BIT(3);
    }

    if (hif_node->u.sdio.sdio_hisr & (WF_BIT(8) | WF_BIT(0)))
    {
        while (1)
        {
            rx_req_len = sdio_get_rx_len(func);

            if (rx_req_len == 0)
            {
                break;
            }

            if ((rx_req_len < MIN_RXD_SIZE) || (rx_req_len > MAX_RXBUF_SZ))
            {
                SDIO_ERROR("wf_sdio_recv error, rx_req_len:0x%x", rx_req_len);
                break;
            }

            ret_type = wf_sdio_read_net_data(hif_node, READ_QUEUE_INX, NULL, rx_req_len);
            if (ret_type < 0)
            {
                //SDIO_ERROR("ret_type:%d", ret_type);
                break;
            }

            if (ret_type != TYPE_DATA)
            {
                isr_clean = WF_BIT(8);  /* clean CMD irq bit*/
                //SDIO_INFO("[1]clean CMD irq bit, rx_req_len:%d, ret_type:0x%x", rx_req_len, ret_type);
                sdio_write_data(func, SDIO_BASE | WL_REG_HISR, (OS_U8*)&isr_clean, 4);
                //SDIO_INFO("[2]clean CMD irq bit, rx_req_len:%d, ret_type:0x%x", rx_req_len, ret_type);
                hif_node->u.sdio.sdio_hisr ^= WF_BIT(8);
            }
        }

        hif_node->u.sdio.sdio_hisr ^= WF_BIT(0);

    }
    hif_node->u.sdio.int_flag--;

}


static OS_RET sdio_interrupt_register(sdio_func_t *func)
{
    OS_RET err;

    sdio_claim_host(func);
    err = sdio_claim_irq(func, &sdio_irq_handle);
    if (err < 0)
    {
        SDIO_ERROR("sdio_interrupt_register error ");
        sdio_release_host(func);
        return err;
    }
    sdio_release_host(func);
    return 0;
}

static void sdio_interrupt_deregister(sdio_func_t *func)
{
    OS_RET err;
    hif_node_t *node = sdio_func_priv(func);

    sdio_claim_host(func);
    err = sdio_release_irq(func);
    if (hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
    {
        sdio_release_host(func);
        return ;
    }
    if (err < 0)
    {
        SDIO_ERROR("sdio_interrupt_deregister error ");
    }

    sdio_release_host(func);
}


static OS_RET sdio_ctl_init(sdio_func_t *func)
{
    OS_U8  value8       = 0;
    OS_S32 count        = 0;
    OS_S32 initSuccess  = 0;
    OS_RET ret          = 0;

    SDIO_INFO();

    sdio_read_data(func, SDIO_BASE | WL_REG_HCTL, &value8, 1);
    value8 &= 0xFE;
    ret = sdio_write_data(func, SDIO_BASE | WL_REG_HCTL, &value8, 1);
    if (ret < 0)
    {
        SDIO_ERROR("0x903a failed, check!!!");
        return ret;
    }

    while (1)
    {
        ret = sdio_read_data(func, SDIO_BASE | WL_REG_HCTL, &value8, 1);
        if (ret)
        {
            break;
        }
        if (value8 & WF_BIT(1))
        {
            initSuccess = wf_true;
            break;
        }

        count++;
        if (count > 1000)
        {
            break;
        }
    }

    if (initSuccess == wf_false)
    {
        SDIO_ERROR("failed!!!");
        return -1;
    }

    return 0;
}

static OS_RET sdio_func_probe(sdio_func_t *func, sdio_device_id_t *id)
{
    hif_node_t *hif_node   = NULL;
    OS_RET ret             = 0;

    SDIO_INFO("Class=%x", func->class);
    SDIO_INFO("Vendor ID:%x", func->vendor);
    SDIO_INFO("Device ID:%x", func->device);
    SDIO_INFO("Function#:%d", func->num);

    /*set sdio blk_size*/
    sdio_func_print(func);
    ret = sdio_func_set_blk(func, SDIO_BLK_SIZE);
    if (ret)
    {
        SDIO_ERROR("sdio_func_set_blk failed");
        return -OS_RERROR;
    }

    if (sdio_ctl_init(func) < 0)
    {
        SDIO_ERROR("sdio_ctl_init error");
        return -1;
    }

    hif_node_register(&hif_node, HIF_SDIO, &sdio_node_ops);
    if (NULL == hif_node)
    {
        SDIO_ERROR("hif_node_register for HIF_SDIO failed");
        return -2;
    }

    SDIO_INFO("%p", hif_node);
    hif_node->u.sdio.sdio_hisr_en   = 0;
    hif_node->u.sdio.func           = func;
    hif_node->u.sdio.irq_cnt        = 0;
    sdio_func_priv_set(func, hif_node);

    if (NULL != hif_node->ops->hif_init)
    {
        hif_node->ops->hif_init(hif_node);
    }
    ret = sdio_interrupt_register(hif_node->u.sdio.func);

    if (ret < 0)
    {
        SDIO_ERROR("interrupt_register failed");
        return -3;
    }

    /* insert netdev */
    if (NULL != hif_node->ops->hif_insert_netdev)
    {
        if (hif_node->ops->hif_insert_netdev(hif_node) < 0)
        {
            SDIO_ERROR("hif_insert_netdev error");
            return 0;
        }
    }
    else
    {
        if (hif_dev_insert(hif_node) < 0)
        {
            SDIO_ERROR("hif_dev_insert error");
            return 0;
        }
    }
    {
        hif_sdio_t *sd = &hif_node->u.sdio;
        OS_U32 value32;
        OS_U8 value8;

        sdio_read_data(sd->func, SDIO_BASE | WL_REG_PUB_FREEPG, (OS_U8*)&value32, 4);
        sd->tx_fifo_ppg_num = value32;
        sdio_read_data(sd->func, SDIO_BASE | WL_REG_HIG_FREEPG, (OS_U8*)&value32, 4);
        sd->tx_fifo_hpg_num = value32;
        sdio_read_data(sd->func, SDIO_BASE | WL_REG_MID_FREEPG, (OS_U8*)&value32, 4);
        sd->tx_fifo_mpg_num = value32;
        sdio_read_data(sd->func, SDIO_BASE | WL_REG_LOW_FREEPG, (OS_U8*)&value32, 4);
        sd->tx_fifo_lpg_num = value32;
        sdio_read_data(sd->func, SDIO_BASE | WL_REG_EXT_FREEPG, (OS_U8*)&value32, 4);
        sd->tx_fifo_epg_num = value32;

        SDIO_INFO("ppg_num:%d, hpg_num:%d, mgp_num:%d, lpg_num:%d, epg_num:%d",
                  sd->tx_fifo_ppg_num, sd->tx_fifo_hpg_num,
                  sd->tx_fifo_mpg_num, sd->tx_fifo_lpg_num, sd->tx_fifo_epg_num);

        sdio_read_data(sd->func, SDIO_BASE | WL_REG_QUE_PRI_SEL,
                       (OS_U8*)&value8, 1);
        if (value8 & OS_BIT(0))
        {
            sd->tx_no_low_queue = wf_false;
            SDIO_INFO("HIGH(fifo_1, fifo_2, fifo_4) MID(fifi_5) LOW(fifo_6)");
        }
        else
        {
            sd->tx_no_low_queue = wf_true;
            SDIO_INFO("HIGH(fifo_1, fifo_2, fifo_4) MID(fifi_5, fifo_6)");
        }

        sdio_read_data(sd->func, SDIO_BASE | WL_REG_AC_OQT_FREEPG,
                       &sd->SdioTxOQTFreeSpace, 1);
        SDIO_INFO("SdioTxOQTFreeSpace:%d", sd->SdioTxOQTFreeSpace);

        SDIO_INFO("end");
    }

    return 0;
}

static void sdio_func_remove(sdio_func_t *func)
{
    hif_node_t *node = sdio_func_priv(func);

    SDIO_INFO("*****SDIO REMOVED**********");

    SDIO_INFO("start");
    if (!node)
    {
        hif_dev_removed(node);
        sdio_interrupt_deregister(node->u.sdio.func);
        wf_sdio_deinit(node);
        hif_node_unregister(node);
    }
    else
    {
        SDIO_ERROR("wf_sdio_func_remove failed");
        return;
    }
    SDIO_INFO("end");
}

sdio_device_id_t sdio_ids[] =
{
    {
        .class = ~0,
        .vendor = SDIO_VENDOR_ID_WL,
        .device = WL_DEVICE_ID_SDIO,
    },
};

static sdio_dev_driver_t wf_sdio_driver =
{
    .name       = "s9083h",
    .id_table   = sdio_ids,
    .probe      = sdio_func_probe,
    .remove     = sdio_func_remove,
};


OS_RET sdio_init(void)
{
    OS_RET ret;

    SDIO_INFO("sdio_init !!!");
    ret = sdio_dev_driver_register(&wf_sdio_driver);
    if (ret)
    {
        SDIO_ERROR("sdio_dev_driver_register failed");
    }

    return ret;
}

OS_RET sdio_exit(void)
{
    SDIO_INFO("sdio_exit !!!");
    sdio_dev_driver_unregister(&wf_sdio_driver);

    return 0;
}


OS_RET wf_sdioh_interrupt_disable(void *hif_info)
{
    OS_U32 himr = 0;
    hif_sdio_t *sd = &((hif_node_t*)hif_info)->u.sdio;
    if (sd->sdio_hisr_en)
    {
        sdio_write_data(sd->func, SDIO_BASE | WL_REG_HIMR, (OS_U8*)&himr, WORD_LEN);
        sd->sdio_hisr = himr;
        sd->sdio_hisr_en = 0;
    }

    return WF_RETURN_OK;
}

OS_RET wf_sdioh_interrupt_enable(void *hif_info)
{
    OS_U32 himr;
    hif_sdio_t *sd = &((hif_node_t*)hif_info)->u.sdio;

    himr = cpu_to_le32(sd->sdio_himr);
    sdio_write_data(sd->func, SDIO_BASE | WL_REG_HIMR, (OS_U8*)&himr, WORD_LEN);
    sd->sdio_hisr = himr;
    sd->sdio_hisr_en = 1;
    return WF_RETURN_OK;
}

OS_RET wf_sdioh_config(void *hif_info)
{
    hif_sdio_t *sd          = &((hif_node_t*)hif_info)->u.sdio;
    unsigned int value32    = 0;
    unsigned char value8    = 0;
    /* need open bulk transport */
    //enable host int
    value32 = 0xFFFFFFFF;
    sdio_write_data(sd->func, SDIO_BASE | 0x9048, (OS_U8 *)&value32, 4);

    value32 = 0xFFFFFFFF;
    sdio_write_data(sd->func, SDIO_BASE | 0x904C, (OS_U8 *)&value32, 4);

#if 0
    sdio_read_data(sd->func,  SDIO_BASE | WL_REG_TXCTL, (OS_U8 *)&value32, 4);
    value32 &= 0xFFF8;
    sdio_write_data(sd->func, SDIO_BASE | WL_REG_TXCTL, (OS_U8 *)&value32, 4);
#endif

    value8 = 0xFF;
    sdio_write_data(sd->func, SDIO_BASE | 0x9068, (OS_U8 *)&value8, 1);

    return 0;
}

