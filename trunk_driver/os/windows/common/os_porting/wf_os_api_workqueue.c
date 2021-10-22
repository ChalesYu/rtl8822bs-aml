#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"

static void wf_workque_init(wf_work_struct *wk_que, void *dev, void *func, void *param)
{
    wk_que->driver_obj = dev;
    wk_que->function = func;
    wk_que->param = param;
    wk_que->que_type = DelayedWorkQueue;
    wk_que->work_item = IoAllocateWorkItem(dev);
	if (NULL == wk_que->work_item)
	{
		LOG_E("[%s] IoAllocateWorkItem failed\n", __func__);
		return;
	}
    //IoInitializeWorkItem(dev, wk_que->work_item);
}

void *wf_workque_alloc(wf_work_struct *wk_que)
{
	UNREFERENCED_PARAMETER(wk_que);

    return NULL;
}

static void wf_workque_enque(wf_work_struct *wk_que)
{
    IoQueueWorkItem(
        wk_que->work_item,
        wk_que->function,
        wk_que->que_type,
        wk_que->param);
}

static void wf_workque_deque(wf_work_struct *wk_que)
{
    //IoUninitializeWorkItem(wk_que->work_item);
}

static void wf_workque_destroy(wf_work_struct *wk_que)
{
    IoFreeWorkItem(wk_que->work_item);
}

int wf_os_api_workqueue_init(wf_workqueue_mgnt_st *arg, void *param)
{
    wf_workqueue_func_param_st *tparam = param;
    wf_workque_init(&arg->work, tparam->DriverObject, tparam->func, &arg->work);
    return 0;
}
int wf_os_api_workqueue_term(wf_workqueue_mgnt_st *arg)
{
    wf_workque_deque(&arg->work);
    wf_workque_destroy(&arg->work);
    return 0;
}
int wf_os_api_workqueue_work(wf_workqueue_mgnt_st *arg)
{
    wf_workque_enque(&arg->work);
    return 0;
}

static wf_workqueue_ops_st wf_gl_workqueue_ops =
{
    wf_os_api_workqueue_init,
    wf_os_api_workqueue_term,
    wf_os_api_workqueue_work,
};

void wf_os_api_workqueue_register(wf_workqueue_mgnt_st *wq,void *param)
{
    wq->ops = &wf_gl_workqueue_ops;
    wq->param = param;
    wq->ops->workqueue_init(wq,wq->param);
}

