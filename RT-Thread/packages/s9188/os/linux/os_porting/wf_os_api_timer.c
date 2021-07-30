/*
 * wf_os_api_timer.c
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
/* include */
#include "wf_os_api.h"

/* macro */

/* type */

/* function declaration */


wf_inline wf_u64 wf_os_api_timestamp(void)
{
    return jiffies;
}

wf_inline wf_u32 wf_os_api_msecs_to_timestamp(wf_u32 msecs)
{
    return msecs_to_jiffies(msecs);
}

wf_inline wf_u32 wf_os_api_timestamp_to_msecs(wf_u32 timestamp)
{
    return jiffies_to_msecs(timestamp);
}

int wf_os_api_timer_reg(wf_os_api_timer_t *ptimer, void (* fn) (wf_os_api_timer_t *), void *pdata)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
    timer_setup(ptimer, fn, 0);
#else
    init_timer(ptimer);
    ptimer->function    = (void *)fn;
    ptimer->data        = (unsigned long)pdata;
#endif
    return 0;
}

wf_inline int wf_os_api_timer_set (wf_os_api_timer_t *ptimer, wf_u32 intv_ms)
{
    mod_timer(ptimer, jiffies + msecs_to_jiffies(intv_ms));
    return 0;
}

wf_inline int wf_os_api_timer_unreg (wf_os_api_timer_t *ptimer)
{
    del_timer(ptimer);
    return 0;
}

int wf_os_api_timer_init (void)
{
    return 0;
}

int wf_os_api_timer_term (void)
{
    return 0;
}

