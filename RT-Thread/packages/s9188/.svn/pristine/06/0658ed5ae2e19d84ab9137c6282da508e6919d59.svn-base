
/* include */
#include "wf_os_api.h"

/* macro */

/* type */

/* function declaration */

void wf_os_api_thread_affinity(wf_u8 cpu_id)
{

}


void* wf_os_api_thread_create (void *tid, char *name, void *func, void *param)
{
    struct task_struct *htask = NULL;

    htask = kthread_run(func, param, name);
    if (IS_ERR(htask))
    {
        return NULL;
    }

    return htask;
}

wf_inline int wf_os_api_thread_wakeup (void *tid)
{
    struct task_struct *htask = (struct task_struct *)(tid);

    if (htask)
        wake_up_process(htask);

    return 0;
}

int wf_os_api_thread_destory (void *tid)
{
    struct task_struct *htask = (struct task_struct *)(tid);
    if (htask)
    {
        //printk("wf_os_api_thread_destory - htask=%p",htask);
        kthread_stop(htask);
        htask = NULL;
    }
    else
    {
        return -1;
    }

    return 0;
}


wf_inline wf_bool wf_os_api_thread_wait_stop (void *tid)
{
    return kthread_should_stop() ? wf_true : wf_false;
}


wf_inline void wf_os_api_thread_exit (void *comp)
{
}

