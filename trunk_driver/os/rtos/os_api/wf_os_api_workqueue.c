
/* include */
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL  (0xf)
#include "common.h"

/* macro */
#define INIT_WORK(_work, _func) do { \
    (_work)->data = 0; \
    wf_list_init(&(_work)->entry); \
    (_work)->func = (_func); \
} while (0)

#define WORK_QUEUE_EVENT_DESTORY    WF_BIT(0)

/* type define */
enum
{
    WORK_PENDING_BIT = 0, /* work item is pending execution */
    WORK_DELAYED_BIT = 1, /* work item is delayed */

    WORK_PENDING = 1 << WORK_PENDING_BIT,
    WORK_DELAYED = 1 << WORK_DELAYED_BIT,
};

/* funcation declaration */
void wf_os_api_thrd_cfg_get (const char *name,
                             wf_u32 *rpriority, wf_u32 *rtask_size);

static void worker_thrd (void *arg)
{
    wf_work_queue_t *wq = arg;

    for (;;)
    {
        /* put current work queue into suspend state, wait new work */
        wf_sema_pend(wq->sema, ~0ul);

        /* work queue destory */
        if (wq->event & WORK_QUEUE_EVENT_DESTORY)
        {
            /* notify queue work has stop */
            wq->event &= ~WORK_QUEUE_EVENT_DESTORY;
            break;
        }

        /* handle all pending works */
        {
            wf_irq_sta_t irq_sta = wf_enter_critical();
            while (!wf_list_is_empty(&wq->head))
            {
                wf_work_t *work =
                    wf_list_entry(wf_list_next(&wq->head), wf_work_t, entry);
                wf_work_fn_t f = work->func;

                wf_list_delete(&work->entry);
                wf_exit_critical(irq_sta);

                f(work);
                work->data &= ~WORK_PENDING;

                irq_sta = wf_enter_critical();
            }
            wf_exit_critical(irq_sta);
        }
    }

    /* thread exit, then resource will be release by idle task */
    wf_thread_exit();
}

static wf_work_queue_t *work_queue_create (const char *name)
{
    wf_work_queue_t *wq;
    int ret;

    wq = wf_zmalloc(sizeof(*wq));
    if (WF_WARN_ON(!wq))
    {
        return NULL;
    }

    ret = wf_sema_new(&wq->sema, 0);
    if (ret)
    {
        wf_free(wq);
        return NULL;
    }

    {
        wf_u32 pri, task_size;
        wf_os_api_thrd_cfg_get(name, &pri, &task_size);
        ret = wf_thread_new(&wq->work_thrd_id,
                            name, pri, task_size,
                            worker_thrd, wq);
    }
    if (ret)
    {
        wf_sema_free(wq->sema);
        wf_free(wq);
        return NULL;
    }

    wq->name = name;
    wq->event = 0;
    wf_list_init(&wq->head);

    return wq;
}

static int queue_work (wf_work_queue_t *wq, wf_work_t *work)
{
    if (wq->work_thrd_id && !(work->data & WORK_PENDING))
    {
        wf_irq_sta_t irq_sta = wf_enter_critical();
        if (!wf_list_is_empty(&work->entry))
        {
            wf_exit_critical(irq_sta);
            /* work is already pending */
            LOG_D("%s %d: \"%s\" is already working", __FILE__, __LINE__, wq->name);
            return 0;
        }

        /* add work */
        wf_list_insert_tail(&work->entry, &wq->head);
        wf_exit_critical(irq_sta);

        /* wakeup work queue */
        work->data |= WORK_PENDING;
        wf_sema_post(wq->sema);
        return 1;
    }
    return 0;
}

static void work_queue_flush (wf_work_queue_t *wq)
{
    WF_UNUSED(wq);
}

static void work_queue_destory (wf_work_queue_t *wq)
{
    if (WF_WARN_ON(!wq->work_thrd_id))
    {
        return;
    }

    /* notify and wait queue work thread stop */
    wq->event |= WORK_QUEUE_EVENT_DESTORY;
    wf_sema_post(wq->sema);
    while (wq->event & WORK_QUEUE_EVENT_DESTORY) wf_thread_sleep(10);

    /* free queue work thread resource */
    wf_sema_free(wq->sema);
    wq->work_thrd_id = NULL;
    wf_free(wq);
}


int wf_os_api_workqueue_init (wf_workqueue_mgnt_t *wqm, void *arg)
{
    wf_workqueue_func_param_t *param = arg;

    if (WF_WARN_ON(!wqm || !param->workqueue_name))
    {
        return -1;
    }

    INIT_WORK(&wqm->work, param->func);
    wqm->workqueue = work_queue_create(param->workqueue_name);

    return 0;
}

int wf_os_api_workqueue_term (wf_workqueue_mgnt_t *wqm)
{
    work_queue_flush(wqm->workqueue);
    work_queue_destory(wqm->workqueue);

    return 0;
}

int wf_os_api_workqueue_work (wf_workqueue_mgnt_t *wqm)
{
    return queue_work(wqm->workqueue, &wqm->work);
}

static wf_workqueue_ops_st wf_gl_workqueue_ops =
{
    .workqueue_init = wf_os_api_workqueue_init,
    .workqueue_term = wf_os_api_workqueue_term,
    .workqueue_work = wf_os_api_workqueue_work,
};

void wf_os_api_workqueue_register (wf_workqueue_mgnt_t *wq, void *param)
{
    wq->ops = &wf_gl_workqueue_ops;
    wq->param = param;
    wq->ops->workqueue_init(wq, wq->param);
}


