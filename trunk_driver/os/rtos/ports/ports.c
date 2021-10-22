
/* include */
#include "ports.h"

/* macro */

/* function declaration */

/* thread */
int wf_thread_new (wf_thread_t *rthrd_id,
                   const char *name, wf_thread_fn_t fn, void *arg)
{
    osThreadAttr_t thrd_attr =
    {
        .name       = name,
        .stack_size = OS_ALIGN(2000, sizeof(StackType_t)),
        .priority   = osPriorityLow,
    };
    osThreadId_t thrd_id;

    thrd_id = osThreadNew((osThreadFunc_t)fn, arg, &thrd_attr);
    if (OS_WARN_ON(!thrd_id))
    {
        return -1;
    }
    *rthrd_id = thrd_id;

    return 0;
}

OS_INLINE
int wf_thread_sleep (wf_u32 ms)
{
    return osDelay(pdMS_TO_TICKS(ms));
}

OS_INLINE
int wf_thread_yield (void)
{
    return osThreadYield();
}

OS_INLINE
int wf_thread_suspend (wf_thread_t thrd_id)
{
    return osThreadSuspend(thrd_id) == osOK ? 0 : -1;
}

OS_INLINE
int wf_thread_resume (wf_thread_t thrd_id)
{
    return osThreadResume(thrd_id) == osOK ? 0 : -1;
}

OS_INLINE
void wf_thread_exit (void)
{
    osThreadExit();
}


/* mutex */
OS_INLINE
int wf_mutex_new (wf_mutex_t *rmutex_id)
{
    osMutexAttr_t mutex_attr =
    {
        .attr_bits = (osMutexRecursive | osMutexPrioInherit),
    };
    wf_mutex_t mutex_id;

    mutex_id = (wf_mutex_t)osMutexNew(&mutex_attr);
    if (OS_WARN_ON(!mutex_id))
    {
        return -1;
    }
    *rmutex_id = mutex_id;

    return 0;
}

OS_INLINE
int wf_mutex_lock (wf_mutex_t mutex_id)
{
    return osMutexAcquire(mutex_id, osWaitForever) == osOK ? 0 : -1;
}

OS_INLINE
int wf_mutex_try_lock (wf_mutex_t mutex_id)
{
    return osMutexAcquire(mutex_id, 0) == osOK ? 0 : -1;
}

OS_INLINE
int wf_mutex_unlock (wf_mutex_t mutex_id)
{
    return osMutexRelease(mutex_id) == osOK ? 0 : -1;
}

OS_INLINE
int wf_mutex_free (wf_mutex_t mutex_id)
{
    return osMutexDelete(mutex_id) == osOK ? 0 : -1;
}


/* semaphone */
OS_INLINE
int wf_sema_new (wf_sema_t *rsema_id, wf_u32 count)
{
    wf_sema_t sema_id;

    sema_id = (wf_sema_t)osSemaphoreNew(~0ul, count, NULL);
    if (OS_WARN_ON(!sema_id))
    {
        return -1;
    }
    *rsema_id = sema_id;

    return 0;
}

OS_INLINE
int wf_sema_pend (wf_sema_t sema_id, wf_u32 ms)
{
    return osSemaphoreAcquire(sema_id, pdMS_TO_TICKS(ms)) == osOK ? 0 : -1;
}

OS_INLINE
int wf_sema_try (wf_sema_t sema_id)
{
    return osSemaphoreAcquire(sema_id, 0) == osOK ? 0 : -1;
}

OS_INLINE
int wf_sema_post (wf_sema_t sema_id)
{
    return osSemaphoreRelease(sema_id) == osOK ? 0 : -1;
}

OS_INLINE
int wf_sema_free (wf_sema_t sema_id)
{
    return osSemaphoreDelete(sema_id) == osOK ? 0 : -1;
}


/* timer */
OS_INLINE
int wf_timer_once_reg (wf_timer_once_t *rtimer, wf_timer_once_fn_t fn, void *arg)
{
    osTimerId_t timer;

    timer = osTimerNew((osTimerFunc_t)fn, osTimerOnce, arg, NULL);
    if (OS_WARN_ON(!timer))
    {
        return -1;
    }
    *rtimer = timer;

    return 0;
}

OS_INLINE
int wf_timer_once_mod (wf_timer_once_t timer, wf_u32 ms)
{
    osStatus_t status;

    status = osTimerStart(timer, pdMS_TO_TICKS(ms));
    if (OS_WARN_ON(status != osOK))
    {
        return -1;
    }

    return 0;
}

OS_INLINE
int wf_timer_once_unreg (wf_timer_once_t timer)
{
    osStatus_t status;

    status = osTimerDelete(timer);
    if (OS_WARN_ON(status != osOK))
    {
        return -1;
    }

    return 0;
}


OS_INLINE
wf_u32 wf_tick (void)
{
    return osKernelGetTickCount();
}

OS_INLINE
wf_u32 wf_msecs_to_ticks (wf_u32 msecs)
{
    return pdMS_TO_TICKS(msecs);
}

OS_INLINE
wf_u32 wf_ticks_to_msecs (wf_u32 ticks)
{
    return ticks * 1000 / osKernelGetTickFreq();
}


/* memory */
void *wf_zmalloc (wf_u32 size)
{
    if (size)
    {
        void *ptr = pvPortMalloc(size);

        if (ptr)
        {
            memset(ptr, 0x0, size);
        }

        return ptr;
    }

    return NULL;
}

OS_INLINE
void wf_free (void *ptr)
{
    vPortFree(ptr);
}


/* critical */
OS_INLINE
wf_irq_sta_t wf_enter_critical (void)
{
    return portSET_INTERRUPT_MASK_FROM_ISR();
}

OS_INLINE
void wf_exit_critical (wf_irq_sta_t irq_sta)
{
    portCLEAR_INTERRUPT_MASK_FROM_ISR(irq_sta);
}


/* sdio */
OS_INLINE
int wf_sdio_driver_reg (wf_sdio_driver_t *driver)
{
    return sdio_dev_driver_register((void *)driver) != OS_ROK ? -1 : 0;
}

OS_INLINE
int wf_sdio_driver_unreg (wf_sdio_driver_t *driver)
{
    return sdio_dev_driver_unregister((void *)driver) != OS_ROK ? -1 : 0;
}


OS_INLINE
void wf_sdio_set_drvdata (wf_sdio_func_t func, void *drvdata)
{
    bsp_sdio_func_priv_set(func, drvdata);
}

OS_INLINE
void *wf_sdio_get_drvdata (wf_sdio_func_t func)
{
    return bsp_sdio_func_priv(func);
}


void wf_sdio_claim_host (wf_sdio_func_t func)
{
    sdio_obj_t *sdio_obj;

    if (OS_WARN_ON(!func))
    {
        return;
    }

    sdio_obj = bsp_sdmmc1_sdio_priv(((sdio_func_t *)func)->hsdio);
    osMutexAcquire(sdio_obj->io_mutex, osWaitForever);
}

void wf_sdio_release_host (wf_sdio_func_t func)
{
    sdio_obj_t *sdio_obj;

    if (OS_WARN_ON(!func))
    {
        return;
    }

    sdio_obj = bsp_sdmmc1_sdio_priv(((sdio_func_t *)func)->hsdio);
    osMutexRelease(sdio_obj->io_mutex);
}


OS_INLINE
int wf_sdio_claim_irq (wf_sdio_func_t func, wf_sdio_irq_fn_t irq)
{
    if (OS_WARN_ON((!func) || (!irq)))
    {
        return -1;
    }

    if (HAL_SDIO_ClaimIRQ(func, (sdio_irq_handler_t *)irq) != HAL_OK)
    {
        return -2;
    }

    return 0;
}

OS_INLINE
int wf_sdio_release_irq (wf_sdio_func_t func)
{
    if (OS_WARN_ON(!func))
    {
        return -1;
    }

    if (HAL_SDIO_ReleaseIrq(func) != HAL_OK)
    {
        return -2;
    }

    return 0;
}


OS_INLINE
int wf_sdio_enable_func (wf_sdio_func_t func)
{
    if (OS_WARN_ON(!func))
    {
        return -1;
    }

    if (HAL_SDIO_EnableFunc(func) != HAL_OK)
    {
        return -2;
    }

    return 0;
}

OS_INLINE
int wf_sdio_disable_func (wf_sdio_func_t func)
{
    if (OS_WARN_ON(!func))
    {
        return -1;
    }

    if (HAL_SDIO_DisableFunc(func) != HAL_OK)
    {
        return -2;
    }

    return 0;
}


OS_INLINE
int wf_sdio_set_block_size (wf_sdio_func_t func, wf_u32 blksz)
{
    if (OS_WARN_ON(!func))
    {
        return -1;
    }

    if (HAL_SDIO_SetBlockSize(func, blksz) != HAL_OK)
    {
        return -2;
    }

    return 0;
}


OS_INLINE
int wf_sdio_rw_direct (wf_sdio_func_t func, wf_u8 write,
                       wf_u32 addr, wf_u8 in, wf_u8 *out)
{
    return HAL_SDIO_RwDirect(func, write, addr, in, out) != HAL_OK ? -1 : 0;
}

OS_INLINE
int wf_sdio_rw_extended (wf_sdio_func_t func, wf_u8 write,
                         wf_u32 addr, wf_u8 incr_addr,
                         wf_u8 *buf, wf_u32 size)
{
    return HAL_SDIO_RwExtended(func, write, addr, incr_addr, buf, size) !=
           HAL_OK ? -1 : 0;
}

