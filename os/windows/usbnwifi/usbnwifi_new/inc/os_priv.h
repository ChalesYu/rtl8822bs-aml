#ifndef __OS_PRIV_H__
#define __OS_PRIV_H__


typedef int wf_tasklet;
typedef int loff_t;


void *wf_malloc(wf_u32 size);
void wf_free(void *ptr);
void wf_Msleep(wf_u32 MilliSecond);
void wf_Usleep(wf_u32 MilliSecond);

#define WF_TIMESTAMP               wf_win_tick_count()


#define ___constant_swab16(x) ((wf_u16)(				\
	(((wf_u16)(x) & (wf_u16)0x00ffU) << 8) |			\
	(((wf_u16)(x) & (wf_u16)0xff00U) >> 8)))

#define ___constant_swab32(x) ((wf_u32)(				\
	(((wf_u32)(x) & (wf_u32)0x000000ffUL) << 24) |		\
	(((wf_u32)(x) & (wf_u32)0x0000ff00UL) <<  8) |		\
	(((wf_u32)(x) & (wf_u32)0x00ff0000UL) >>  8) |		\
	(((wf_u32)(x) & (wf_u32)0xff000000UL) >> 24)))

#define ___constant_swab64(x) ((wf_u64)(				\
	(((wf_u64)(x) & (wf_u64)0x00000000000000ffULL) << 56) |	\
	(((wf_u64)(x) & (wf_u64)0x000000000000ff00ULL) << 40) |	\
	(((wf_u64)(x) & (wf_u64)0x0000000000ff0000ULL) << 24) |	\
	(((wf_u64)(x) & (wf_u64)0x00000000ff000000ULL) <<  8) |	\
	(((wf_u64)(x) & (wf_u64)0x000000ff00000000ULL) >>  8) |	\
	(((wf_u64)(x) & (wf_u64)0x0000ff0000000000ULL) >> 24) |	\
	(((wf_u64)(x) & (wf_u64)0x00ff000000000000ULL) >> 40) |	\
	(((wf_u64)(x) & (wf_u64)0xff00000000000000ULL) >> 56)))

#define ___constant_swahw32(x) ((wf_u32)(			\
	(((wf_u32)(x) & (wf_u32)0x0000ffffUL) << 16) |		\
	(((wf_u32)(x) & (wf_u32)0xffff0000UL) >> 16)))

#define ___constant_swahb32(x) ((wf_u32)(			\
	(((wf_u32)(x) & (wf_u32)0x00ff00ffUL) << 8) |		\
	(((wf_u32)(x) & (wf_u32)0xff00ff00UL) >> 8)))

#define __constant_cpu_to_le64(x) ((  wf_u64)___constant_swab64((x)))
#define __constant_le64_to_cpu(x) ___constant_swab64((  wf_u64)(wf_u64)(x))
#define __constant_cpu_to_le32(x) ((  wf_u32)___constant_swab32((x)))
#define __constant_le32_to_cpu(x) ___constant_swab32((  wf_u32)(wf_u32)(x))
#define __constant_cpu_to_le16(x) ((  wf_u16)___constant_swab16((x)))
#define __constant_le16_to_cpu(x) ___constant_swab16((  wf_u16)(wf_u16)(x))

#define __cpu_to_le64(x) ((  wf_u64)(wf_u64)(x))
#define __le64_to_cpu(x) ((  wf_u64)(wf_u64)(x))
#define __cpu_to_le32(x) ((  wf_u32)(wf_u32)(x))
#define __le32_to_cpu(x) ((  wf_u32)(wf_u32)(x))
#define __cpu_to_le16(x) ((  wf_u16)(wf_u16)(x))
#define __le16_to_cpu(x) ((  wf_u16)(wf_u16)(x))
#define __cpu_to_be64(x)  ((  wf_u64)___constant_swab64((x)))
#define __be64_to_cpu(x)  ___constant_swab64((  wf_u64)(wf_u64)(x))
#define __cpu_to_be32(x)  ((  wf_u32)___constant_swab32((x)))
#define __be32_to_cpu(x)  ___constant_swab32((  wf_u32)(wf_u32)(x))
#define __cpu_to_be16(x)  ((  wf_u16)___constant_swab16((x)))
#define __be16_to_cpu(x)  ___constant_swab16((  wf_u16)(wf_u16)(x))

#define htonl(x) __cpu_to_be32(x)
#define ntohl(x) __be32_to_cpu(x)
#define htons(x) __cpu_to_be16(x)
#define ntohs(x) __be16_to_cpu(x)

#define le64_to_cpu __le64_to_cpu
#define cpu_to_le32 __cpu_to_le32
#define le32_to_cpu __le32_to_cpu
#define cpu_to_le16 __cpu_to_le16
#define le16_to_cpu __le16_to_cpu
#define cpu_to_be64 __cpu_to_be64
#define be64_to_cpu __be64_to_cpu
#define cpu_to_be32 __cpu_to_be32
#define be32_to_cpu __be32_to_cpu
#define cpu_to_be16 __cpu_to_be16
#define be16_to_cpu __be16_to_cpu



#ifndef WF_OFFSETOF
#define WF_OFFSETOF(type, field)        FIELD_OFFSET(type, field)
#endif

#ifndef WF_FIELD_SIZEOF
#define WF_FIELD_SIZEOF(type, field)    RTL_FIELD_SIZE(type, field)
#endif

#ifndef WF_CONTAINER_OF
#define WF_CONTAINER_OF(ptr, type, field) \
    ((type *)((wf_u8 *)ptr - WF_OFFSETOF(type, field)))
#endif


#define WF_HZ                      wf_win_hz()

#define wf_os_api_do_div(n,base)             (n%base)
#define wf_yield()                   wf_Msleep(1)
#define wf_inline                  __inline

#define wf_memcpy                  memcpy
#define wf_memcmp                  memcmp
#define wf_memset                  memset
#define wf_mdelay                  wf_Msleep
#define wf_udelay                  wf_Usleep
#define wf_msleep                  wf_Msleep

#define wf_kzalloc(sz)             wf_malloc(sz)
#define wf_kfree                   wf_free
#define wf_vmalloc                 wf_malloc
#define wf_vfree                   wf_free
#define wf_alloc_skb(sz)
#define wf_free_skb

#define wf_packed

typedef struct wf_thread_s
{
	void *obj;
	void *tid;
	wf_u8 stop;
	char name[64];
}wf_thread_t;


typedef struct wf_workqueue_s {
    PIO_WORKITEM *work_item;
    void *driver_obj;
    void(*function)(void *);
    WORK_QUEUE_TYPE que_type;
    void *param;
}wf_workqueue_t;


typedef wf_workqueue_t wf_work_struct;
typedef wf_workqueue_t wf_workqueue_struct;

typedef void(*work_func)(wf_work_struct *work);
typedef struct wf_workqueue_mgnt_st_ wf_workqueue_mgnt_st;

typedef struct
{
    char *workqueue_name;
    work_func func;
    PDRIVER_OBJECT  DriverObject;
}wf_workqueue_func_param_st;

typedef unsigned long           wf_irq;

typedef NDIS_SPIN_LOCK spinlock_t;

typedef wf_u32 wf_lock_mutex;

#define WF_SEMA_LIMIT_MAX   (1<<30)

typedef struct wf_event_s {
	KEVENT evt;
	LARGE_INTEGER timeout;
}wf_event_t;
typedef KSEMAPHORE wf_os_api_sema_t;

typedef struct
{
	KTIMER ktimer;
	KDPC kdpc;
	void(*fn) (void *);
	void *pdata;
} wf_os_api_timer_t;

typedef struct wf_file_s {
	HANDLE handle;
	IO_STATUS_BLOCK io_status;
}wf_file_t;
typedef wf_file_t  wf_file;

#endif