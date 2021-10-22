/*
 * wf_os_api_workqueue.h
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
#ifndef __WF_OS_API_WORKQUEUE_H__
#define __WF_OS_API_WORKQUEUE_H__

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

