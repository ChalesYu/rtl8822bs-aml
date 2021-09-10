
/* include */
#include "common.h"

/* macro */

/* type */

/* function declaration */

OS_INLINE wf_u64 wf_os_api_timestamp (void)
{
    return (wf_u64)osKernelGetTickCount();
}

OS_INLINE wf_u32 wf_os_api_msecs_to_timestamp (wf_u32 msecs)
{
    return pdMS_TO_TICKS(msecs);
}

OS_INLINE wf_u32 wf_os_api_timestamp_to_msecs (wf_u32 timestamp)
{
    return timestamp * 1000 / osKernelGetTickFreq();
}


int wf_os_api_timer_reg (wf_os_api_timer_t *ptimer, void (*fn) (wf_os_api_timer_t *), void *pdata)
{
    osTimerId_t timer_id = osTimerNew((osTimerFunc_t)fn, osTimerOnce, pdata, NULL);
    if (OS_WARN_ON(!timer_id))
    {
        return -1;
    }

    *ptimer = (wf_os_api_timer_t)timer_id;

    return 0;
}

OS_INLINE int wf_os_api_timer_set (wf_os_api_timer_t *ptimer, wf_u32 intv_ms)
{
    osStatus_t status = osTimerStart(*ptimer, pdMS_TO_TICKS(intv_ms));
    if (OS_WARN_ON(status != osOK))
    {
        return -1;
    }

    return 0;
}

OS_INLINE int wf_os_api_timer_unreg (wf_os_api_timer_t *ptimer)
{
    osStatus_t status = osTimerDelete(*ptimer);
    if (OS_WARN_ON(status != osOK))
    {
        return -1;
    }

    return 0;
}

int wf_os_api_timer_init (void)
{
    return 0;
}

int wf_os_api_timer_term (void)
{
    return 0;
}

