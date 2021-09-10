
/* include */
#include "common.h"

/* macro */

/* type */

/* function declaration */

OS_INLINE void wf_os_api_thread_affinity (wf_u8 cpu_id)
{
    OS_UNUSED(cpu_id);

    osThreadWaitStartup();
}

void* wf_os_api_thread_create (void *tid, char *name, void *func, void *param)
{
    OS_UNUSED(tid);

    osThreadId_t thrd_id;
    osThreadAttr_t thrd_attr =
    {
        .name       = (const char *)name,
        .stack_size = OS_ALIGN(2000, sizeof(StackType_t)),
        .priority   = osPriorityLow,
    };

    thrd_id = osThreadNew((osThreadFunc_t)func, param, &thrd_attr);
    if (OS_WARN_ON(!thrd_id))
    {
        return NULL;
    }

    return thrd_id;
}

OS_INLINE int wf_os_api_thread_wakeup (void *tid)
{
    if (OS_WARN_ON(!tid))
    {
        return -1;
    }

    osThreadStartup(tid);

    return 0;
}

OS_INLINE int wf_os_api_thread_destory (void *tid)
{
    if (OS_WARN_ON(!tid))
    {
        return -1;
    }

    osThreadStop(tid);

    return 0;
}

OS_INLINE wf_bool wf_os_api_thread_wait_stop (void *tid)
{
    OS_UNUSED(tid);

    return (wf_bool)osThreadShouldStop();
}

OS_INLINE void wf_os_api_thread_exit (void *comp)
{
    OS_UNUSED(comp);

    osThreadExit();
}

