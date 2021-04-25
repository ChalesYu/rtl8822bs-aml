#include "wf_os_api.h"
#include "wf_debug.h"

int wf_os_api_workqueue_init(wf_workqueue_mgnt_st *arg, void *param)
{
    wf_workqueue_func_param_st *tparam = param;
    if(NULL == arg || NULL == tparam->workqueue_name )
    {
        LOG_E("[%s] arg or workqueue_name is null",__func__);
        return -1;
    }
    
    INIT_WORK(&arg->work, tparam->func);
    arg->workqueue  = create_singlethread_workqueue(tparam->workqueue_name);
    
    return 0;
}

int wf_os_api_workqueue_term(wf_workqueue_mgnt_st *arg)
{
    flush_workqueue(arg->workqueue);
    destroy_workqueue(arg->workqueue);
    return 0;
}

int wf_os_api_workqueue_work(wf_workqueue_mgnt_st *arg)
{
    queue_work(arg->workqueue,&arg->work);
    return 0;
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

