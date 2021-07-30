/*
 * wf_os_api_workqueue.h
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
#ifndef __WF_OS_API_WORKQUEUE_H__


typedef struct wf_workqueue_ops_st_
{
    int (*workqueue_init)(wf_workqueue_mgnt_st *wq, void *param);
    int (*workqueue_term)(wf_workqueue_mgnt_st *wq);
    int (*workqueue_work)(wf_workqueue_mgnt_st *wq);
}wf_workqueue_ops_st;

struct wf_workqueue_mgnt_st_
{
    wf_work_struct work;
    wf_workqueue_struct *workqueue;
    wf_workqueue_func_param_st *param;
    wf_workqueue_ops_st *ops;
};


void wf_os_api_workqueue_register(wf_workqueue_mgnt_st *wq, void *param);

#endif // !__WF_OS_API_WORKQUEUE_H__

