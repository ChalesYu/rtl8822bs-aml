
#ifndef __PORTS_H__
#define __PORTS_H__

/* includes */
#include "wf_typedef.h"
#include "global.h"

/* macro */
#define wf_inline   OS_INLINE
#define wf_weak     OS_WEAK
#define wf_packed   OS_PACKED

#define WF_HZ   OS_HZ


#define wf_memcpy   os_memcpy
#define wf_memcmp   os_memcmp
#define wf_memset   os_memset

#define WF_LOG_PRINT(...)   os_log_print(__VA_ARGS__)
#define log_array(p, l)     os_log_array(p, l)


#define le16_to_cpu     OS_LE16_TO_CPU
#define cpu_to_le16     OS_CPU_TO_LE16
#define be16_to_cpu     OS_BE16_TO_CPU
#define cpu_to_be16     OS_CPU_TO_BE16

#define le32_to_cpu     OS_LE32_TO_CPU
#define cpu_to_le32     OS_CPU_TO_LE32
#define be32_to_cpu     OS_BE32_TO_CPU
#define cpu_to_be32     OS_CPU_TO_BE32

#define le64_to_cpu     OS_LE64_TO_CPU
#define cpu_to_le64     OS_CPU_TO_LE64
#define be64_to_cpu     OS_BE64_TO_CPU
#define cpu_to_be64     OS_CPU_TO_BE64


/* type */
typedef void *wf_thread_t;
typedef void (*wf_thread_fn_t) (void *);

typedef void *wf_mutex_t;

typedef void *wf_sema_t;

typedef void *wf_timer_once_t;
typedef void (*wf_timer_once_fn_t) (void *);

typedef wf_u32 wf_irq_sta_t;

typedef void *wf_sdio_func_t;
typedef struct
{
    wf_u8 class;
    wf_u16 vendor;
    wf_u16 device;
} wf_sdio_device_id_t;
typedef struct
{
    wf_sdio_device_id_t *id;
    int (*probe)(wf_sdio_func_t, wf_sdio_device_id_t *);
    void (*remove)(wf_sdio_func_t);
} wf_sdio_driver_t;
typedef void (*wf_sdio_irq_fn_t) (wf_sdio_func_t);


/* function declaration */

int wf_thread_new (wf_thread_t *rthrd_id,
                   const char *name, wf_thread_fn_t fn, void *arg);
int wf_thread_sleep (wf_u32 ms);
int wf_thread_yield (void);
int wf_thread_suspend (wf_thread_t thrd_id);
int wf_thread_resume (wf_thread_t thrd_id);
void wf_thread_exit (void);

int wf_mutex_new (wf_mutex_t *rmutex_id);
int wf_mutex_lock (wf_mutex_t mutex_id);
int wf_mutex_try_lock (wf_mutex_t mutex_id);
int wf_mutex_unlock (wf_mutex_t mutex_id);
int wf_mutex_free (wf_mutex_t mutex_id);

int wf_sema_new (wf_sema_t *sema_id, wf_u32 count);
int wf_sema_pend (wf_sema_t sema_id, wf_u32 ms);
int wf_sema_try (wf_sema_t sema_id);
int wf_sema_post (wf_sema_t sema_id);
int wf_sema_free (wf_sema_t sema_id);

int wf_timer_once_reg (wf_timer_once_t *rtimer, wf_timer_once_fn_t fn, void *arg);
int wf_timer_once_mod (wf_timer_once_t timer, wf_u32 ms);
int wf_timer_once_unreg (wf_timer_once_t timer);

wf_u32 wf_tick (void);
wf_u32 wf_msecs_to_ticks (wf_u32 msecs);
wf_u32 wf_ticks_to_msecs (wf_u32 ticks);

wf_irq_sta_t wf_enter_critical (void);
void wf_exit_critical (wf_irq_sta_t irq_sta);

void *wf_zmalloc (wf_u32 size);
void wf_free (void *ptr);

int wf_sdio_driver_reg (wf_sdio_driver_t *driver);
int wf_sdio_driver_unreg (wf_sdio_driver_t *driver);
void wf_sdio_set_drvdata (wf_sdio_func_t func, void *drvdata);
void *wf_sdio_get_drvdata (wf_sdio_func_t func);
void wf_sdio_claim_host (wf_sdio_func_t func);
void wf_sdio_release_host (wf_sdio_func_t func);
int wf_sdio_claim_irq (wf_sdio_func_t func, wf_sdio_irq_fn_t irq);
int wf_sdio_release_irq (wf_sdio_func_t func);
int wf_sdio_enable_func (wf_sdio_func_t func);
int wf_sdio_disable_func (wf_sdio_func_t func);
int wf_sdio_set_block_size (wf_sdio_func_t func, wf_u32 blksz);
int wf_sdio_rw_direct (wf_sdio_func_t func, wf_u8 write,
                       wf_u32 addr, wf_u8 in, wf_u8 *out);
int wf_sdio_rw_extended (wf_sdio_func_t func, wf_u8 write,
                         wf_u32 addr, wf_u8 incr_addr,
                         wf_u8 *buf, wf_u32 size);

#endif

