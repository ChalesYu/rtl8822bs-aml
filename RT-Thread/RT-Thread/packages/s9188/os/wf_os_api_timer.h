/*
 * wf_os_api_timer.h
 *
 * used for .....
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __WF_OS_API_TIMER_H__
#define __WF_OS_API_TIMER_H__

wf_u64 wf_os_api_timestamp (void);
wf_u32 wf_os_api_msecs_to_timestamp (wf_u32 msecs);
wf_u32 wf_os_api_timestamp_to_msecs (wf_u32 timestamp);
int wf_os_api_timer_reg (wf_os_api_timer_t *ptimer, void (* fn) (wf_os_api_timer_t *), void *pdata);
int wf_os_api_timer_set (wf_os_api_timer_t *ptimer, wf_u32 intv_ms);
int wf_os_api_timer_unreg (wf_os_api_timer_t *ptimer);
int wf_os_api_timer_init (void);
int wf_os_api_timer_term (void);


#endif

