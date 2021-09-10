/*
 * wf_os_api_workqueue.c
 *
 * os workqueue realization.
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

int wf_os_api_workqueue_init(wf_workqueue_mgnt_st *arg, void *param)
{
    static wf_u8 wf_workqueue_prio = 0;
    wf_workqueue_func_param_st *tparam = param;
    
    if(NULL == arg || NULL == tparam->workqueue_name )
    {
        LOG_E("[%s] arg or workqueue_name is null",__func__);
        return -1;
    }
    
    if(wf_workqueue_prio > (WF_WORKQUEUE_PRIORITY_NUM - 1)) {
      LOG_E("create workqueue error, please reconfig WF_WORKQUEUE_PRIORITY_NUM");
      return -1;
    }
    
    rt_work_init(&arg->work, tparam->func, tparam->param);
    arg->workqueue = rt_workqueue_create(tparam->workqueue_name, WF_WORKQUEUE_STACK_SIZE,
                                         WF_WORKQUEUE_PRIORITY_START + WF_WORKQUEUE_PRIORITY_NUM - 1 - wf_workqueue_prio);
    if (arg->workqueue == RT_NULL)
    {
      LOG_E("wf wifi work queue create failed", __FUNCTION__, __LINE__);
      return -1;
    }
    
    wf_workqueue_prio++;
    return 0;
}

int wf_os_api_workqueue_term(wf_workqueue_mgnt_st *arg)
{
    return rt_workqueue_destroy(arg->workqueue);
}

int wf_os_api_workqueue_work(wf_workqueue_mgnt_st *arg)
{
    return rt_workqueue_dowork(arg->workqueue, &arg->work);
}

static wf_workqueue_ops_st wf_gl_workqueue_ops =
{
    .workqueue_init = wf_os_api_workqueue_init,
    .workqueue_term = wf_os_api_workqueue_term,
    .workqueue_work = wf_os_api_workqueue_work,
};

void wf_os_api_workqueue_register(wf_workqueue_mgnt_st *wq,void *param)
{
    wq->ops = &wf_gl_workqueue_ops;
    wq->param = param;
    wq->ops->workqueue_init(wq,wq->param);
}

