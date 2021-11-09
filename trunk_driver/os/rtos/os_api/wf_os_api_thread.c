
/* include */
#include "common.h"

/* macro */
enum
{
    WF_OS_API_THRD_EVT_SHOULD_STOP_BIT  = 0,

    WF_OS_API_THRD_EVT_SHOULD_STOP      = WF_BIT(WF_OS_API_THRD_EVT_SHOULD_STOP_BIT),
};

/* type */
typedef struct
{
    wf_thread_t id;
    wf_u32 event;
} wf_os_api_thread_t;

/* variable declaration */

/* function declaration */
void wf_os_api_thrd_cfg_get (const char *name,
                             wf_u32 *rpriority, wf_u32 *rtask_size);

void *wf_os_api_thread_create (void *tid, char *name, void *func, void *param)
{
    wf_os_api_thread_t *thread;
    WF_UNUSED(tid);

    thread = wf_zmalloc(sizeof(*thread));
    if (!thread)
    {
        return NULL;
    }

    {
        wf_irq_sta_t irq_sta;
        int ret;

        /* prevent scheduler work, before new thread suspend done */
        irq_sta = wf_enter_critical();
        /* create thread */
        {
            wf_u32 pri, task_size;
            wf_os_api_thrd_cfg_get(name, &pri, &task_size);
            ret = wf_thread_new(&thread->id,
                                name, pri, task_size,
                                (wf_thread_fn_t)func, param);
        }
        if (ret)
        {
            wf_exit_critical(irq_sta);
            wf_free(thread);
            return NULL;
        }
        /* put new thread into suspend state */
        wf_thread_suspend(thread->id);
        wf_exit_critical(irq_sta);
    }

    thread->event = 0;

    return thread;
}

wf_inline void wf_os_api_thread_affinity (wf_u8 cpu_id)
{
    WF_UNUSED(cpu_id);
}

wf_inline int wf_os_api_thread_wakeup (void *tid)
{
    wf_os_api_thread_t *thread = tid;

    return wf_thread_resume(thread->id);
}

wf_bool wf_os_api_thread_wait_stop (void *tid)
{
    wf_os_api_thread_t *thread = tid;

    if (thread->event & WF_OS_API_THRD_EVT_SHOULD_STOP)
    {
        thread->event &= ~WF_OS_API_THRD_EVT_SHOULD_STOP;
        return wf_true;
    }

    return wf_false;
}

int wf_os_api_thread_destory (void *tid)
{
    wf_os_api_thread_t *thread = tid;

    thread->event |= WF_OS_API_THRD_EVT_SHOULD_STOP;
    while (thread->event & WF_OS_API_THRD_EVT_SHOULD_STOP)
    {
        wf_thread_sleep(10);
    }
    wf_free(thread);

    return 0;
}

wf_inline void wf_os_api_thread_exit (void *tid)
{
    wf_thread_exit();
}


