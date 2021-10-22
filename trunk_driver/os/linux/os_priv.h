/*
 * os_priv.h
 *
 * used for .....
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#ifndef __OS_PRIV_H__
#define __OS_PRIV_H__

typedef __kernel_size_t         SIZE_T;
typedef __kernel_ssize_t        SSIZE_T;

typedef unsigned long           wf_irq;
typedef struct tasklet_struct   wf_tasklet;
#define wf_tasklet_hi_sched        tasklet_hi_schedule
#define wf_tasklet_sched           tasklet_schedule
#define wf_tasklet_init            tasklet_init

#define WF_OFFSETOF(type, field)            offsetof(type, field)
//#define WF_FIELD_SIZEOF(type, field)        FIELD_SIZEOF(type, field)
#define WF_FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
#define WF_CONTAINER_OF(ptr, type, field)   container_of(ptr, type, field)

#define WF_HZ                       HZ
#define wf_os_api_do_div           do_div
#define wf_yield                   yield
#define wf_inline                  __always_inline

#define wf_memcpy                  memcpy
#define wf_memcmp                  memcmp
#define wf_memset                  memset
#define wf_mdelay                  mdelay
#define wf_udelay                  udelay
#define wf_msleep                  msleep

#ifdef MEMDBG_ENABLE
#include "memdbg.h"
#define wf_kzalloc(sz) memdbg_kmalloc(sz,__func__,__LINE__)
#define wf_kfree(ptr)    memdbg_kfree(ptr,__func__,__LINE__)
#else
#define wf_kzalloc(sz)             kzalloc(sz, in_interrupt()? GFP_ATOMIC : GFP_KERNEL)
#define wf_kfree                   kfree
#endif
#define wf_vmalloc                 vmalloc
#define wf_vfree                   vfree
#define wf_alloc_skb(sz)           __dev_alloc_skb(sz, in_interrupt()? GFP_ATOMIC : GFP_KERNEL)
#define wf_free_skb                dev_kfree_skb_any

#define wf_packed                 __attribute__((__packed__))

typedef struct work_struct wf_work_struct;
typedef struct workqueue_struct  wf_workqueue_struct;
typedef void (*work_func)(wf_work_struct *work);
typedef struct wf_workqueue_mgnt_st_ wf_workqueue_mgnt_st;

typedef struct
{
    char *workqueue_name;
    work_func func;
}wf_workqueue_func_param_st;

typedef struct mutex            wf_lock_mutex;
typedef struct semaphore        wf_os_api_sema_t;
typedef struct timer_list       wf_os_api_timer_t;
typedef struct file             wf_file;

#endif
