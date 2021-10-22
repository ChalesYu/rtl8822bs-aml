/*
 * wf_timer.h
 *
 * This file contains all the prototypes for the wf_timer.c file
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __WF_TIMER_H__
#define __WF_TIMER_H__

typedef struct
{
    wf_u32 start;
    wf_u32 expire;
    wf_u32 interval;
} wf_timer_t;

wf_inline static void wf_timer_set (wf_timer_t *ptimer, wf_u32 intv_ms)
{
    ptimer->start = (wf_u32)wf_os_api_timestamp();
    ptimer->interval = wf_os_api_msecs_to_timestamp(intv_ms);
    ptimer->expire = ptimer->start + ptimer->interval;
}

wf_inline static void wf_timer_reset (wf_timer_t *ptimer)
{
    ptimer->start = ptimer->expire;
    ptimer->expire = ptimer->start + ptimer->interval;
}

wf_inline static void wf_timer_restart (wf_timer_t *ptimer)
{
    ptimer->start = (wf_u32)wf_os_api_timestamp();
    ptimer->expire = ptimer->start + ptimer->interval;
}

wf_inline static void wf_timer_mod (wf_timer_t *ptimer, wf_u32 intv_ms)
{
    ptimer->interval = wf_os_api_msecs_to_timestamp(intv_ms);
    ptimer->expire = ptimer->start + ptimer->interval;
}

wf_inline static wf_bool wf_timer_expired (wf_timer_t *ptimer)
{
    return (wf_bool)((wf_s32)(ptimer->expire - wf_os_api_timestamp()) < 0);
}

wf_inline static wf_s32 wf_timer_remaining (wf_timer_t *ptimer)
{
    return (wf_s32)(ptimer->expire - wf_os_api_timestamp());
}

wf_inline static wf_u32 wf_timer_elapsed (wf_timer_t *ptimer)
{
    return wf_os_api_timestamp_to_msecs((wf_u32)wf_os_api_timestamp() - ptimer->start);
}

int wf_timer_init (void);
int wf_timer_term (void);

#endif

