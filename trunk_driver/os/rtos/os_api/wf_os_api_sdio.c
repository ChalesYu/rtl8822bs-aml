
/* include */
#include "common.h"

/* macro */

/* type */

wf_inline
int wf_os_api_sdio_register_driver (wf_os_api_sdio_driver_t *drv)
{
    return wf_sdio_driver_reg((void *)drv);
}

wf_inline
int wf_os_api_sdio_unregister_driver (wf_os_api_sdio_driver_t *drv)
{
    return wf_sdio_driver_unreg((void *)drv);
}


wf_inline
void wf_os_api_sdio_set_drvdata (wf_os_api_sdio_func_t func, void *priv_data)
{
    wf_sdio_set_drvdata(func, priv_data);
}

wf_inline
void *wf_os_api_sdio_get_drvdata (wf_os_api_sdio_func_t func)
{
    return wf_sdio_get_drvdata(func);
}


wf_inline
void wf_os_api_sdio_claim_host (wf_os_api_sdio_func_t func)
{
    wf_sdio_claim_host(func);
}

wf_inline
void wf_os_api_sdio_release_host (wf_os_api_sdio_func_t func)
{
    wf_sdio_release_host(func);
}


wf_inline
int wf_os_api_sdio_claim_irq (wf_os_api_sdio_func_t func,
                              wf_os_api_sdio_irq_fn_t handler)
{
    return wf_sdio_claim_irq(func, handler);
}

wf_inline
int wf_os_api_sdio_release_irq (wf_os_api_sdio_func_t func)
{
    return wf_sdio_release_irq(func);
}


wf_inline
int wf_os_api_sdio_enable_func (wf_os_api_sdio_func_t func)
{
    return wf_sdio_enable_func(func);
}

wf_inline
int wf_os_api_sdio_disable_func (wf_os_api_sdio_func_t func)
{
    return wf_sdio_disable_func(func);
}


wf_inline
int wf_os_api_sdio_set_block_size (wf_os_api_sdio_func_t func, wf_u32 blksz)
{
    return wf_sdio_set_block_size(func, blksz);
}


wf_u8 wf_os_api_sdio_readb (wf_os_api_sdio_func_t func,
                            wf_u32 addr, int *err_ret)
{
    wf_u8 val;
    int ret;

    if (WF_WARN_ON(!func))
    {
        if (err_ret)
        {
            *err_ret = -22;
        }
        return 0xFF;
    }

    ret = wf_sdio_rw_direct(func, 0, addr, 0, &val);
    if (ret)
    {
        *err_ret = ret;
    }
    if (ret)
    {
        return 0xFF;
    }

    return val;
}

void wf_os_api_sdio_writeb (wf_os_api_sdio_func_t func,
                            wf_u8 b, wf_u32 addr, int *err_ret)
{
    int ret;

    if (WF_WARN_ON(!func))
    {
        if (err_ret)
        {
            *err_ret = -22;
        }
        return;
    }

    ret = wf_sdio_rw_direct(func, 1, addr, b, NULL);
    if (err_ret)
    {
        *err_ret = ret;
    }
}


wf_inline
int wf_os_api_sdio_memcpy_fromio (wf_os_api_sdio_func_t func, void *dst,
                                  wf_u32 addr, wf_u32 count)
{
    return wf_sdio_rw_extended(func, 0, addr, 1, dst, count);
}

wf_inline
int wf_os_api_sdio_memcpy_toio (wf_os_api_sdio_func_t func, wf_u32 addr,
                                void *src, wf_u32 count)
{
    return wf_sdio_rw_extended(func, 1, addr, 1, src, count);
}


wf_u16 wf_os_api_sdio_readw (wf_os_api_sdio_func_t func,
                             wf_u32 addr, int *err_ret)
{
    wf_u16 tmp;
    int ret;

    ret = wf_os_api_sdio_memcpy_fromio(func, &tmp, addr, 2);
    if (err_ret)
    {
        *err_ret = ret;
    }
    if (ret)
    {
        return 0xFFFF;
    }

    return wf_le16_to_cpu(tmp);
}

void wf_os_api_sdio_writew (wf_os_api_sdio_func_t func,
                            wf_u16 b, wf_u32 addr, int *err_ret)
{
    wf_u16 tmp;
    int ret;

    tmp = wf_cpu_to_le16(b);

    ret = wf_os_api_sdio_memcpy_toio(func, addr, &tmp, 2);
    if (err_ret)
    {
        *err_ret = ret;
    }
}


wf_u32 wf_os_api_sdio_readl (wf_os_api_sdio_func_t func,
                             wf_u32 addr, int *err_ret)
{
    wf_u32 tmp;
    int ret;

    ret = wf_os_api_sdio_memcpy_fromio(func, &tmp, addr, 4);
    if (err_ret)
    {
        *err_ret = ret;
    }
    if (ret)
    {
        return 0xFFFFFFFF;
    }

    return wf_le32_to_cpu(tmp);
}

void wf_os_api_sdio_writel (wf_os_api_sdio_func_t func,
                            wf_u32 b, wf_u32 addr, int *err_ret)
{
    wf_u32 tmp;
    int ret;

    tmp = wf_cpu_to_le32(b);

    ret = wf_os_api_sdio_memcpy_toio(func, addr, &tmp, 4);
    if (err_ret)
    {
        *err_ret = ret;
    }
}

