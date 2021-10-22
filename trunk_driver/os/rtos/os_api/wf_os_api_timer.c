
/* include */
#include "common.h"

/* macro */

/* type */

/* function declaration */

OS_INLINE wf_u64 wf_os_api_timestamp (void)
{
    return (wf_u64)wf_tick();
}

OS_INLINE wf_u32 wf_os_api_msecs_to_timestamp (wf_u32 msecs)
{
    return wf_msecs_to_ticks(msecs);
}

OS_INLINE wf_u32 wf_os_api_timestamp_to_msecs (wf_u32 timestamp)
{
    return wf_ticks_to_msecs(timestamp);
}


OS_INLINE int wf_os_api_timer_reg (wf_os_api_timer_t *timer,
                                   void (*fn) (wf_os_api_timer_t *),
                                   void *arg)
{
    return wf_timer_once_reg(timer, (wf_timer_once_fn_t)fn, arg);
}

OS_INLINE int wf_os_api_timer_set (wf_os_api_timer_t *timer, wf_u32 ms)
{
    return wf_timer_once_mod(*timer, ms);
}

OS_INLINE int wf_os_api_timer_unreg (wf_os_api_timer_t *timer)
{
    return wf_timer_once_unreg(*timer);
}

OS_INLINE int wf_os_api_timer_init (void)
{
    return 0;
}

OS_INLINE int wf_os_api_timer_term (void)
{
    return 0;
}

