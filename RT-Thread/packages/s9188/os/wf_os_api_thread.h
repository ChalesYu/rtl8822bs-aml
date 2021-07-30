/*
 * wf_os_api_thread.h
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
#ifndef __WF_OS_API_THREAD_H__
#define __WF_OS_API_THREAD_H__

#define DEFAULT_CPU_ID  (0)

void* wf_os_api_thread_create(void *ptid, char *name, void *func, void *param);
int wf_os_api_thread_wakeup(void *ptid);
int wf_os_api_thread_destory(void *ptid);
wf_bool wf_os_api_thread_wait_stop(void *ptid);
void wf_os_api_thread_exit(void *ptid);
void wf_os_api_thread_affinity(wf_u8 cpu_id);


#endif

