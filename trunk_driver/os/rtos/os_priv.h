
#ifndef __OS_PRIV_H__
#define __OS_PRIV_H__

/* includes */
#include "ports/ports.h"
#include "wf_list.h"

/* macro */
#define CONFIG_LITTLE_ENDIAN

#define wf_align_up(x, a)       (((x) + ((a) - 1)) & (~((a) - 1)))

#define wf_os_api_do_div(n, base) \
    ({wf_u64 _##n = n; (n) /= (base); _##n % (base);})

#define WF_OFFSETOF(type, field)    ((size_t)&((type *)0)->field)
#define WF_FIELD_SIZEOF(t, f)       sizeof(((t *)0)->f)
#define WF_CONTAINER_OF(ptr, type, field) \
    (type *)((char *)(ptr) - WF_OFFSETOF(type, field))


#define wf_yield()      wf_thread_sleep(1)
#define wf_mdelay(ms)   wf_thread_sleep(ms)
#define wf_msleep(ms)   wf_thread_sleep(ms)


#define wf_kzalloc(sz)      wf_zmalloc(sz)
#define wf_kfree(x)         wf_free(x)
#define wf_vmalloc(sz)      wf_zmalloc(sz)
#define wf_vfree(x)         wf_free(x)
#define wf_alloc_skb(sz)    wf_zmalloc(sz)
#define wf_free_skb(x)      wf_free(x)


#ifndef htonl
#define htonl(x)    cpu_to_be32(x)
#endif
#ifndef ntohl
#define ntohl(x)    be32_to_cpu(x)
#endif
#ifndef htons
#define htons(x)    cpu_to_be16(x)
#endif
#ifndef ntohs
#define ntohs(x)    be16_to_cpu(x)
#endif


#define WF_TYPE_CHECK(type, x) \
    ({ type __dummy; \
       (void)(&__dummy == &x); \
       1; \
     })

#define WF_WARN_ON(condition) \
    ({  if (condition) \
        { \
            LOG_W("\n" __FILE__ ":%d: WARN " #condition "!\n", __LINE__); \
        } \
        !!(condition);\
     })

#define WF_ASSERT(expr) do { \
    if (!(expr)) \
    { \
        LOG_E("\n" __FILE__ ":%d: Assertion " #expr " failed!\n", __LINE__); \
        WF_BUG();\
    } \
} while (0)


#define WF_BUG() do { \
    portDISABLE_INTERRUPTS(); \
    *((volatile int *)0x07FFFFF0) = 0x0; /* trig hard fault */ \
} while (0)

#define WF_BUG_ON(x) do { \
    if (x) WF_BUG(); \
} while (0)


#define WF_UNUSED(x)    ((void)(x))


/* typedef */
typedef size_t          SIZE_T;
typedef signed long     SSIZE_T;
typedef long long       loff_t;

typedef wf_irq_sta_t    wf_irq;


struct wf_work_t_;
typedef void (*wf_work_fn_t) (struct wf_work_t_ *);
typedef struct wf_work_t_
{
    wf_list_t entry;
    wf_u32 data;
    wf_work_fn_t func;
} wf_work_t, wf_work_struct;
typedef struct
{
    wf_thread_t work_thrd_id;
    wf_list_t head;
    const char *name;
    wf_u32 event;
    wf_sema_t sema;
} wf_work_queue_t, wf_workqueue_struct;

typedef struct wf_workqueue_mgnt_st_ wf_workqueue_mgnt_st, wf_workqueue_mgnt_t;
typedef struct
{
    char *workqueue_name;
    wf_work_fn_t func;
} wf_workqueue_func_param_st, wf_workqueue_func_param_t;


typedef void *spinlock_t;
typedef wf_mutex_t wf_lock_mutex;
typedef wf_sema_t wf_os_api_sema_t;
typedef wf_sema_t wf_os_api_completion_t;
typedef wf_timer_once_t wf_os_api_timer_t;
typedef void *wf_file;

#include "os_api/wf_os_api_sdio.h"
#include "os_api/wf_os_api_completion.h"

#endif

