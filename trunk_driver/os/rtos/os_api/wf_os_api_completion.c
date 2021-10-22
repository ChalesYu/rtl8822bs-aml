
/* include */
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

/* funcation declaration */

void wf_os_api_completion_wait_for (wf_os_api_completion_t *x)
{
    WF_WARN_ON(wf_sema_pend(*x, ~0ul));
}

wf_u8 wf_os_api_completion_try_wait_for (wf_os_api_completion_t *x)
{
    return wf_sema_try(*x) ? 0 : 1;
}

wf_u32 wf_os_api_completion_wait_for_timeout (wf_os_api_completion_t *x, wf_u32 timeout)
{
    return wf_sema_pend(*x, timeout) ? 0 : 1;
}


void wf_os_api_complete (wf_os_api_completion_t *x)
{
    WF_WARN_ON(wf_sema_post(*x));
}


void wf_os_api_completion_init (wf_os_api_completion_t *x)
{
    if (*x)
    {
        while (!wf_sema_try(*x));
    }
    else
    {
        wf_sema_new(x, 0);
    }
}

void wf_os_api_completion_reinit (wf_os_api_completion_t *x)
{
    while (!wf_sema_try(*x));
}


