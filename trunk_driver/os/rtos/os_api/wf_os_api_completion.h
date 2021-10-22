/*
 * wf_os_api_completion.h
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
#ifndef __WF_OS_API_COMPLETION_H__
#define __WF_OS_API_COMPLETION_H__

void wf_os_api_completion_wait_for (wf_os_api_completion_t *x);
wf_u8 wf_os_api_completion_try_wait_for (wf_os_api_completion_t *x);
wf_u32 wf_os_api_completion_wait_for_timeout (wf_os_api_completion_t *x, wf_u32 timeout);
void wf_os_api_complete (wf_os_api_completion_t *x);
void wf_os_api_completion_init (wf_os_api_completion_t *x);
void wf_os_api_completion_reinit (wf_os_api_completion_t *x);

#endif

