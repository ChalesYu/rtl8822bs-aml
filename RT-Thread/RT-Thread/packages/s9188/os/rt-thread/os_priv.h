/*
 * os_priv.h
 *
 * used for os priv define.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __OS_PRIV_H__
#define __OS_PRIV_H__

#if (!defined(CONFIG_LITTLE_ENDIAN) && !defined(CONFIG_BIG_ENDIAN))
#define CONFIG_LITTLE_ENDIAN
#endif

typedef size_t         SIZE_T;
typedef ssize_t        SSIZE_T;
typedef rt_uint32_t    loff_t;
typedef rt_base_t      wf_irq;

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

#ifdef CONFIG_LITTLE_ENDIAN

#define __cpu_to_le64(x)        ((  wf_u64)(wf_u64)(x))
#define __le64_to_cpu(x)        ((  wf_u64)(wf_u64)(x))
#define __cpu_to_le32(x)        ((  wf_u32)(wf_u32)(x))
#define __le32_to_cpu(x)        ((  wf_u32)(wf_u32)(x))
#define __cpu_to_le16(x)        ((  wf_u16)(wf_u16)(x))
#define __le16_to_cpu(x)        ((  wf_u16)(wf_u16)(x))
#define __cpu_to_be64(x)        ((  wf_u64)___constant_swab64((x)))
#define __be64_to_cpu(x)        ___constant_swab64((  wf_u64)(wf_u64)(x))
#define __cpu_to_be32(x)        ((  wf_u32)___constant_swab32((x)))
#define __be32_to_cpu(x)        ___constant_swab32((  wf_u32)(wf_u32)(x))
#define __cpu_to_be16(x)        ((  wf_u16)___constant_swab16((x)))
#define __be16_to_cpu(x)        ___constant_swab16((  wf_u16)(wf_u16)(x))

#ifndef htonl
#define htonl(x)        __cpu_to_be32(x)
#endif
#ifndef ntohl
#define ntohl(x)        __be32_to_cpu(x)
#endif  
#ifndef htons
#define htons(x)        __cpu_to_be16(x)
#endif
#ifndef ntohs
#define ntohs(x)        __be16_to_cpu(x)
#endif

#else   // CONFIG_BIG_ENDIAN

#define __cpu_to_le64(x)        ((  wf_u64)___constant_swab64((x)))
#define __le64_to_cpu(x)        ___constant_swab64((  wf_u64)(wf_u64)(x))
#define __cpu_to_le32(x)        ((  wf_u32)___constant_swab32((x)))
#define __le32_to_cpu(x)        ___constant_swab32((  wf_u32)(wf_u32)(x))
#define __cpu_to_le16(x)        ((  wf_u16)___constant_swab16((x)))
#define __le16_to_cpu(x)        ___constant_swab16((  wf_u16)(wf_u16)(x))
#define __cpu_to_be64(x)        ((  wf_u64)(wf_u64)(x))
#define __be64_to_cpu(x)        ((  wf_u64)(wf_u64)(x))
#define __cpu_to_be32(x)        ((  wf_u32)(wf_u32)(x))
#define __be32_to_cpu(x)        ((  wf_u32)(wf_u32)(x))
#define __cpu_to_be16(x)        ((  wf_u16)(wf_u16)(x))
#define __be16_to_cpu(x)        ((  wf_u16)(wf_u16)(x))

#ifndef htonl
#define htonl(x)        x
#endif
#ifndef ntohl
#define ntohl(x)        x
#endif  
#ifndef htons
#define htons(x)        x
#endif
#ifndef ntohs
#define ntohs(x)        x
#endif

#endif


#define le64_to_cpu     __le64_to_cpu
#define cpu_to_le32     __cpu_to_le32
#define le32_to_cpu     __le32_to_cpu
#define cpu_to_le16     __cpu_to_le16
#define le16_to_cpu     __le16_to_cpu
#define cpu_to_be64     __cpu_to_be64
#define be64_to_cpu     __be64_to_cpu
#define cpu_to_be32     __cpu_to_be32
#define be32_to_cpu     __be32_to_cpu
#define cpu_to_be16     __cpu_to_be16
#define be16_to_cpu     __be16_to_cpu

#define WF_OFFSETOF(type, field)            ((size_t) &((type *)0)->field)
#define WF_FIELD_SIZEOF(t, f)               (sizeof(((t*)0)->f))
#define WF_CONTAINER_OF(ptr, type, field)   rt_container_of(ptr, type, field)

#define WF_HZ                      RT_TICK_PER_SECOND
#define wf_os_api_do_div(n,base)   (n%base)
#define wf_yield                   rt_thread_yield
#define wf_inline                  __inline //rt_inline

#define wf_memcpy                  memcpy
#define wf_memcmp                  memcmp
#define wf_memset                  memset
#define wf_mdelay                  rt_thread_mdelay
#define wf_udelay                  rt_hw_us_delay
#define wf_msleep                  rt_thread_mdelay

#define wf_kzalloc(sz)             rt_calloc(1,sz)
#define wf_kfree                   rt_free
#define wf_vmalloc                 rt_malloc
#define wf_vfree                   rt_free
#define wf_alloc_skb(sz)           rt_malloc
#define wf_free_skb                rt_free
#define wf_packed                 __attribute__((__packed__))

#define WF_WORKQUEUE_PRIORITY_START  5
#define WF_WORKQUEUE_PRIORITY_NUM    3
#define WF_WORKQUEUE_STACK_SIZE      2048

#define WF_THREAD_PRIORITY_START     8
#define WF_THREAD_PRIORITY_NUM       4
#define WF_THREAD_STACK_SIZE         2048

typedef struct rt_work            wf_work_struct;
typedef struct rt_workqueue       wf_workqueue_struct;
typedef void (*work_func)(wf_work_struct *work, void *param);
typedef struct wf_workqueue_mgnt_st_  wf_workqueue_mgnt_st;

typedef struct
{
    char *workqueue_name;
    work_func func;
    void *param;
}wf_workqueue_func_param_st;

typedef struct rt_mutex            wf_lock_mutex;
typedef struct rt_semaphore        wf_os_api_sema_t;
#ifdef RT_USING_SMP
typedef rt_hw_spinlock_t           spinlock_t;
#else
typedef rt_ubase_t                 spinlock_t;
#endif
typedef struct rt_timer            wf_os_api_timer_t;
typedef FILE                       wf_file;

#endif
