
#include "wf_debug.h"
#include "common.h"
#include "hif.h"

#if 0
#define PWR_DBG(fmt, ...)      LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define PWR_ARRAY(data, len)   os_log_array(data, len)
#else
#define PWR_DBG(fmt, ...)
#define PWR_ARRAY(data, len)
#endif
#define PWR_INFO(fmt, ...)     LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define PWR_WARN(fmt, ...)     LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define PWR_ERROR(fmt, ...)    LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)


int side_road_cfg(hif_node_t *node)
{
    int ret         = 0;
    wf_u8  value8   = 0;

    /************************func configure*****************************/

    /* enable reg r/w */
    LOG_I("old:0xac---0x%x", hif_io_read8(node, 0xac, NULL));
    value8 = hif_io_read8(node, 0xac, NULL);
    value8 |= 0x02;
    ret = hif_io_write8(node, 0xac, value8);
    LOG_I("new:0xac---0x%x", hif_io_read8(node, 0xac, NULL));

    /* M0 Uart/Fw_type select */
    LOG_I("old:0xf8---0x%x", hif_io_read8(node, 0xf8, NULL));
    value8 = hif_io_read8(node, 0xf8, NULL);
    value8 |= 0x10;      // special write for all
    value8 |= 0x40;      // 开M0的串口

#ifdef CONFIG_FW_ENCRYPT
    value8 |= 0x80;      // 加密固件
#else
    value8 &= 0x7F;      // 非加密固件
#endif
    ret = hif_io_write8(node, 0xf8, value8);
    LOG_I("new:0xf8---0x%x", hif_io_read8(node, 0xf8, NULL));

    LOG_I("old:0x98---0x%x", hif_io_read8(node, 0x98, NULL));
    hif_io_write8(node, 0x98, 0xff);
    LOG_I("new:0x98---0x%x", hif_io_read8(node, 0x98, NULL));


    LOG_I("new:0x1c---0x%x", hif_io_read8(node, 0x1c, NULL));

    /*  For Bluk transport */
#if 1
    ret = hif_io_write32(node, 0x200, 0x00100000);
    LOG_I("0x200---0x%x", hif_io_read32(node, 0x200, NULL));
    ret = hif_io_write32(node, 0x200, 0x80100000);
    LOG_I("0x200---0x%x", hif_io_read32(node, 0x200, NULL));
#endif

    LOG_I("[%s] cfg sucess", __func__);

    return 0;
}



int power_off(hif_node_t *node)
{
    int ret = 0;
    wf_u8  value8 = 0;
    wf_u16 value16 = 0;
    wf_u32 value32 = 0;

    if (hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
    {
        return WF_RETURN_OK;
    }

    switch(node->hif_type)
    {
        case HIF_USB:
        {
            // 0x00ac  bit 22 write 1, reset dsp
            value32 = hif_io_read32(node, 0xac, NULL);
            value32 |= WF_BIT(22);
            ret = hif_io_write32(node, 0xac, value32);
            if (WF_RETURN_FAIL == ret)
            {
                LOG_E("[%s] 0xac bit 22 set 1 failed, check!!!", __func__);
                return ret;
            }

            // clear the power off bit
            value32 &= ~((wf_u32)WF_BIT(11));
            ret = hif_io_write32(node, 0xac, value32);
            if (WF_RETURN_FAIL == ret)
            {
                LOG_E("[%s] 0xac failed, check!!!", __func__);
                return ret;
            }

            // 配置下电起始条件为：0x00AC[10] 先配0再配1，上升沿使能硬件下电状态机
            value32 &= ~((wf_u32)WF_BIT(10));
            ret = hif_io_write32(node, 0xac, value32);
            if (WF_RETURN_FAIL == ret)
            {
                LOG_E("[%s] 0xac failed, check!!!", __func__);
                return ret;
            }
            value32 |= WF_BIT(10);
            ret = hif_io_write32(node, 0xac, value32);
            if (WF_RETURN_FAIL == ret)
            {
                LOG_E("[%s] 0xac failed, check!!!", __func__);
                return ret;
            }

            wf_msleep(10);
            // waiting for power off
            value16 = 0;
            while (1)
            {
                value32 = hif_io_read32(node, 0xac, NULL);
                if (value32 & WF_BIT(11))
                {
                    break;
                }
                wf_msleep(1);
                value16++;
                if (value16 > 10)
                {
                    break;
                }
            }

            if (value16 > 10)
            {
                LOG_E("[%s] failed!!!", __func__);
                return WF_RETURN_FAIL;
            }
        }
        break;

        case HIF_SDIO:
        {
            // 0x00ac  bit 22 write 1, reset dsp
            value8 = hif_io_read8(node, 0xac+2, NULL);
            value8 |= WF_BIT(6);
            ret = hif_io_write8(node, 0xac+2, value8);
            if (WF_RETURN_FAIL == ret)
            {
                LOG_E("[%s] 0xac failed, check!!!", __func__);
                return ret;
            }

            // clear the power off bit
            value8 = hif_io_read8(node, 0x9094, NULL);
            value8 &= 0xFE;
            ret = hif_io_write8(node, 0x9094, value8);
            if (WF_RETURN_FAIL == ret)
            {
                LOG_E("[%s] 0x9094 failed, check!!!", __func__);
                return ret;
            }

            // 配置下电起始条件为：0x00AC[10] 先配0再配1，上升沿使能硬件下电状态机
            value8 = hif_io_read8(node, 0xac+1, NULL);
            value8 &= ~(WF_BIT(2));
            ret = hif_io_write8(node, 0xac+1, value8);
            if (WF_RETURN_FAIL == ret)
            {
                LOG_E("[%s] 0xac failed, check!!!", __func__);
                return ret;
            }
            value8 |= WF_BIT(2);
            ret = hif_io_write8(node, 0xac+1, value8);
            if (WF_RETURN_FAIL == ret)
            {
                LOG_E("[%s] 0xac failed, check!!!", __func__);
                return ret;
            }
            wf_msleep(10);
            // waiting for power off
            value16 = 0;
            while (1)
            {
                value8 = hif_io_read8(node, 0x9094, NULL);
                if (value8 & WF_BIT(0))
                {
                    break;
                }
                wf_msleep(1);
                value16++;
                if (value16 > 100)
                {
                    break;
                }
            }

            if (value16 > 100)
            {
                LOG_E("[%s] failed!!!", __func__);
                return WF_RETURN_FAIL;
            }
        }
        break;

        default:
        {
            LOG_E("Error Nic type");
            return WF_RETURN_FAIL;
        }
    }

    //set 0x_00AC  0
    ret = hif_io_write8(node, 0xac, 0);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] 0xac failed, check!!!", __func__);
        return ret;
    }

    return WF_RETURN_OK;
}

int power_on(hif_node_t *node)
{
    int ret = 0;
    wf_bool initSuccess = wf_false;
    wf_u8  value8 = 0;
    wf_u16 value16 = 0;

    LOG_I("[%s] start", __func__);

    // check chip status first
    value8 = hif_io_read8(node, 0xac, NULL);
    if (value8 & 0x10)
    {
        value16 = hif_io_read16(node, 0xec, NULL);
        LOG_D("[%s] power on status 0xec:0x%x", __func__, value16);

        power_off(node);
    }
    else
    {
        LOG_D("[%s] power off status", __func__);
    }

    //set 0x_00AC  bit 4 写0
    value8 = hif_io_read8(node, 0xac, NULL);
    value8 &= 0xEF;
    ret = hif_io_write8(node, 0xac, value8);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] 0xac failed, check!!!", __func__);
        return ret;
    }
    //set 0x_00AC  bit 0 写0
    value8 &= 0xFE;
    ret = hif_io_write8(node, 0xac, value8);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] 0xac failed, check!!!", __func__);
        return ret;
    }
    //set 0x_00AC  bit 0 写1
    value8 |= 0x01;
    ret = hif_io_write8(node, 0xac, value8);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] 0xac failed, check!!!", __func__);
        return ret;
    }
    wf_msleep(10);
    // waiting for power on
    value16 = 0;

    while (1)
    {
        value8 = hif_io_read8(node, 0xac, NULL);
        if (value8 & 0x10)
        {
            initSuccess = wf_true;
            break;
        }
        value16++;
        if (value16 > 1000)
        {
            break;
        }
    }

    if (initSuccess == wf_false)
    {
        LOG_E("[%s] failed!!!", __func__);
        return WF_RETURN_FAIL;
    }

    LOG_I("[%s] success", __func__);

    return WF_RETURN_OK;
}


int power_suspend(hif_node_t *node)
{
    hif_io_write32(node, 0x4, 0x20030a02);
    PWR_DBG("reg 0x4(%x)", hif_io_read32(node, 0x4, NULL));

    return 0;
}


int power_resume (hif_node_t *node)
{
    int ret = 0;
    wf_bool initSuccess = wf_false;
    wf_u8  value8 = 0;
    wf_u16 value16 = 0;

    PWR_WARN("resume start\n");

    node->dev_removed = wf_false;

    //set 0x_00AC  bit 4 鍐?
    value8 = hif_io_read8(node, 0xac, NULL);
    value8 &= 0xEF;
    ret = hif_io_write8(node, 0xac, value8);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] 0xac failed, check!!!", __func__);
        return ret;
    }
    //set 0x_00AC  bit 0 鍐?
    value8 &= 0xFE;
    ret = hif_io_write8(node, 0xac, value8);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] 0xac failed, check!!!", __func__);
        return ret;
    }
    //set 0x_00AC  bit 0 鍐?
    value8 |= 0x01;
    ret = hif_io_write8(node, 0xac, value8);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] 0xac failed, check!!!", __func__);
        return ret;
    }
    wf_msleep(10);
    // waiting for power on
    value16 = 0;

    while (1)
    {
        value8 = hif_io_read8(node, 0xac, NULL);
        if (value8 & 0x10)
        {
            initSuccess = wf_true;
            break;
        }
        value16++;
        if (value16 > 1000)
        {
            break;
        }
    }

    if (initSuccess == wf_false)
    {
        LOG_E("[%s] failed!!!", __func__);
        return WF_RETURN_FAIL;
    }

    PWR_DBG("wf_power_on success");

    if (HIF_SDIO ==node->hif_type)
    {
        // cmd53 is ok, next for side-road configue
#ifndef CONFIG_USB_FLAG
        wf_sdioh_config(node);
        wf_sdioh_interrupt_enable(node);
#endif
    }

    /*create hif trx func*/
    PWR_DBG("<< create hif tx/rx queue >>");
    wf_data_queue_mngt_init(node);

#ifdef CONFIG_RICHV200
    ret = wf_hif_bulk_enable(node);
    if (WF_RETURN_FAIL == ret)
    {
        PWR_ERROR("[%s] wf_hif_bulk_enable failed", __func__);
        return -1;
    }
#endif

//    ret = wf_hif_queue_enable(node);
    if (WF_RETURN_FAIL == ret)
    {
        PWR_ERROR("[%s] wf_hif_queue_enable failed", __func__);
        return -1;
    }

    /*ndev reg*/
    PWR_DBG("<< add nic to hif_node >>");
    PWR_DBG("   node_id    :%d", node->node_id);
    PWR_DBG("   hif_type   :%d  [1:usb  2:sdio]", node->hif_type);

    return 0;
}


