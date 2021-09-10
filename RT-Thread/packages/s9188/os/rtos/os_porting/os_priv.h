
#ifndef __OS_PRIV_H__
#define __OS_PRIV_H__

#if !defined(CONFIG_LITTLE_ENDIAN) && !defined(CONFIG_BIG_ENDIAN)
#define CONFIG_LITTLE_ENDIAN
#endif

typedef size_t      SIZE_T;
typedef signed long SSIZE_T;
typedef long long   loff_t;

typedef UBaseType_t wf_irq;

#define WF_OFFSETOF(type, field)            OS_OFFSETOF(type, field)
#define WF_FIELD_SIZEOF(t, f)               OS_FIELD_SIZEOF(t, f)
#define WF_CONTAINER_OF(ptr, type, field)   OS_CONTAINER_OF(ptr, type, field)

#define do_div(n, base) ({wf_u64 _##n = n; (n) /= (base); _##n % (base);})

#define WF_HZ                   OS_HZ
#define wf_os_api_do_div        do_div
#define wf_yield                osThreadYield
#define wf_inline               OS_INLINE
#define wf_memcpy               memcpy
#define wf_memcmp               memcmp
#define wf_memset               memset
#define wf_mdelay(ms)           osDelay(pdMS_TO_TICKS(ms))
#define wf_msleep(ms)           osDelay(pdMS_TO_TICKS(ms))
#define wf_usleep(us)

#define wf_kzalloc(sz)          osZMalloc(sz)
#define wf_kfree                osFree
#define wf_vmalloc              osZMalloc
#define wf_vfree                osFree
#define wf_alloc_skb(sz)        osZMalloc(sz)
#define wf_free_skb             osFree

#define wf_packed               OS_PACKED

/* workqueue */
typedef osWork_t wf_work_struct;
typedef osWorkqueue_t wf_workqueue_struct;
typedef struct wf_workqueue_mgnt_st_ wf_workqueue_mgnt_st;
typedef struct
{
    char *workqueue_name;
    osWorkFunc_t func;
} wf_workqueue_func_param_st;

typedef void *wf_lock_mutex;
typedef void *spinlock_t;
typedef void *wf_os_api_sema_t;
typedef void *wf_os_api_timer_t;
typedef void *wf_file;

#define le16_to_cpu      OS_LE16_TO_CPU
#define cpu_to_le16      OS_CPU_TO_LE16
#define be16_to_cpu      OS_BE16_TO_CPU
#define cpu_to_be16      OS_CPU_TO_BE16

#define le32_to_cpu      OS_LE32_TO_CPU
#define cpu_to_le32      OS_CPU_TO_LE32
#define be32_to_cpu      OS_BE32_TO_CPU
#define cpu_to_be32      OS_CPU_TO_BE32

#define le64_to_cpu      OS_LE64_TO_CPU
#define cpu_to_le64      OS_CPU_TO_LE64
#define be64_to_cpu      OS_BE64_TO_CPU
#define cpu_to_be64      OS_CPU_TO_BE64

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

#endif

