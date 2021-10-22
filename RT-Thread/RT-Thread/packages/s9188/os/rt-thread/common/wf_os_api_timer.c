/*
 * wf_os_api_timer.c
 *
 * os timer realization.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "wf_os_api.h"

wf_inline wf_u64 wf_os_api_timestamp(void)
{
    return rt_tick_get();
}

wf_inline wf_u32 wf_os_api_msecs_to_timestamp(wf_u32 msecs)
{
    return rt_tick_from_millisecond(msecs);
}

wf_inline wf_u32 wf_os_api_timestamp_to_msecs(wf_u32 timestamp)
{
    return (timestamp * 1000) / WF_HZ;
}

int wf_os_api_timer_reg(wf_os_api_timer_t *ptimer, void (* fn) (wf_os_api_timer_t *), void *pdata)
{
  static int seq = 0;
  char name[RT_NAME_MAX] = {0};
  sprintf(name, "wft%d", seq++);
  rt_timer_init(ptimer, name,
                (void(*)(void *))fn,
                ptimer, RT_TICK_PER_SECOND, RT_TIMER_FLAG_ONE_SHOT /* | RT_TIMER_FLAG_PERIODIC */);
  return 0;
}

wf_inline int wf_os_api_timer_set (wf_os_api_timer_t *ptimer, wf_u32 intv_ms)
{
  rt_tick_t timeout = rt_tick_from_millisecond(intv_ms);
  
  rt_timer_control(ptimer, RT_TIMER_CTRL_SET_TIME, &timeout);
  rt_timer_start(ptimer);
  return 0;
}

wf_inline int wf_os_api_timer_unreg (wf_os_api_timer_t *ptimer)
{
    rt_timer_detach(ptimer);
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

