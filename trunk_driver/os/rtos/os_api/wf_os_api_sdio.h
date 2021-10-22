/**
  ******************************************************************************
  * @file    wf_sdio_io.h
  * @brief   This file contains all the function prototypes for
  *          the sdmmc_sdio.c file
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef __WF_OS_API_SDIO_H__
#define __WF_OS_API_SDIO_H__

/* include */

/* macro */

/* type define */
typedef void *wf_os_api_sdio_func_t;
typedef struct
{
    wf_u8 class; /* Standard interface or SDIO_ANY_ID */
    wf_u16 vendor; /* Vendor or SDIO_ANY_ID */
    wf_u16 device; /* Device ID or SDIO_ANY_ID */
} wf_os_api_sdio_device_id_t;
typedef struct
{
    wf_os_api_sdio_device_id_t *id;
    int (*probe)(wf_os_api_sdio_func_t, wf_os_api_sdio_device_id_t *);
    void (*remove)(wf_os_api_sdio_func_t);
} wf_os_api_sdio_driver_t;
typedef void (*wf_os_api_sdio_irq_fn_t) (wf_os_api_sdio_func_t);

/* function declaration */
int wf_os_api_sdio_register_driver (wf_os_api_sdio_driver_t *drv);
int wf_os_api_sdio_unregister_driver (wf_os_api_sdio_driver_t *drv);
void wf_os_api_sdio_set_drvdata (wf_os_api_sdio_func_t func, void *priv_data);
void *wf_os_api_sdio_get_drvdata (wf_os_api_sdio_func_t func);
void wf_os_api_sdio_claim_host (wf_os_api_sdio_func_t func);
void wf_os_api_sdio_release_host (wf_os_api_sdio_func_t func);
int wf_os_api_sdio_claim_irq (wf_os_api_sdio_func_t func,
                              wf_os_api_sdio_irq_fn_t handler);
int wf_os_api_sdio_release_irq (wf_os_api_sdio_func_t func);
int wf_os_api_sdio_enable_func (wf_os_api_sdio_func_t func);
int wf_os_api_sdio_disable_func (wf_os_api_sdio_func_t func);
int wf_os_api_sdio_set_block_size (wf_os_api_sdio_func_t func, wf_u32 blksz);
wf_u8 wf_os_api_sdio_readb (wf_os_api_sdio_func_t func,
                            wf_u32 addr, int *err_ret);
void wf_os_api_sdio_writeb (wf_os_api_sdio_func_t func,
                            wf_u8 b, wf_u32 addr, int *err_ret);
int wf_os_api_sdio_memcpy_fromio (wf_os_api_sdio_func_t func, void *dst,
                                  wf_u32 addr, wf_u32 count);
int wf_os_api_sdio_memcpy_toio (wf_os_api_sdio_func_t func, wf_u32 addr,
                                void *src, wf_u32 count);
wf_u16 wf_os_api_sdio_readw (wf_os_api_sdio_func_t func,
                             wf_u32 addr, int *err_ret);
void wf_os_api_sdio_writew (wf_os_api_sdio_func_t func,
                            wf_u16 b, wf_u32 addr, int *err_ret);
wf_u32 wf_os_api_sdio_readl (wf_os_api_sdio_func_t func,
                             wf_u32 addr, int *err_ret);
void wf_os_api_sdio_writel (wf_os_api_sdio_func_t func,
                            wf_u32 b, wf_u32 addr, int *err_ret);

#endif /* __WF_OS_API_SDIO_H__ */

