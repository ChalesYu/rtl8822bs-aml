/*
 * wf_os_api_thread.c
 *
 * os thread realization.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "wf_os_api.h"
#include "wf_debug.h"


typedef struct wf_thread {
  rt_thread_t thread_id;
  wf_u8        state;
}wf_thread_t;


void wf_os_api_thread_affinity(wf_u8 cpu_id)
{

}

void* wf_os_api_thread_create (void *tid, char *name, void *func, void *param)
{
  static wf_u8 wf_thread_prio = 0;
  wf_thread_t *thread_id;
  
  if(wf_thread_prio > (WF_THREAD_PRIORITY_NUM - 1)) {
    LOG_E("create thread error, please reconfig WF_THREAD_PRIORITY_NUM");
    return NULL;
  }
  
  thread_id = wf_kzalloc(sizeof(struct wf_thread));
  if(thread_id == NULL) {
    LOG_E("There is no memory for thread\r\n");
    return NULL;
  }
  
  thread_id->state = 0;
  thread_id->thread_id = rt_thread_create(name, (void (*)(void *))func, param, 
                WF_THREAD_STACK_SIZE, WF_THREAD_PRIORITY_START + wf_thread_prio, 20);
  if(thread_id->thread_id != NULL) {
    wf_thread_prio++;
    return thread_id;
  }
  
  wf_kfree(thread_id);
  
  LOG_E("Create thread failed");
  
  return NULL;
}

wf_inline int wf_os_api_thread_wakeup (void *tid)
{
  wf_thread_t *thread_id = (wf_thread_t *)tid;
  
  return rt_thread_startup(thread_id->thread_id);
}

int wf_os_api_thread_destory (void *tid)
{
  wf_thread_t *thread_id = (wf_thread_t *)tid;
  
  thread_id->state = 0x01;
  
  while(!(thread_id->state & 0x02)) {
    wf_mdelay(10);
  }
  wf_kfree(thread_id);
  return 0;
}

wf_inline wf_bool wf_os_api_thread_wait_stop (void *tid)
{
  wf_thread_t *thread_id = (wf_thread_t *)tid;
  
  return (thread_id->state & 0x01) ? wf_true : wf_false;
}

wf_inline void wf_os_api_thread_exit (void *tid)
{
  wf_thread_t *thread_id = (wf_thread_t *)tid;
  
  thread_id->state |= 0x02;
}

