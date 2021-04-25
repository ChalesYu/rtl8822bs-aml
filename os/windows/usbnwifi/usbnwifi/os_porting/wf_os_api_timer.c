/* include */
#include <wdm.h>
#include "wf_os_api.h"
#include "wf_mix.h"

/* macro */
#define DELAY_ONE_MICROSECOND   (-10)
#define DELAY_ONE_MILLISECOND   (DELAY_ONE_MICROSECOND*1000)
/* type */

/* function declaration */

void KdeferredRoutine(KDPC *Dpc, PVOID DeferredContext,
                      PVOID SystemArgument1, PVOID SystemArgument2)
{
    wf_os_api_timer_t *ptimer = DeferredContext;

    if (ptimer && ptimer->fn)
    {
        ptimer->fn(ptimer->pdata);
    }
}

int wf_os_api_timer_reg (wf_os_api_timer_t *ptimer,
                         void (* fn) (void *), void *pdata)
{
    ptimer->fn = fn;
    ptimer->pdata = pdata;
    KeInitializeDpc(&ptimer->kdpc, KdeferredRoutine, ptimer);
    KeInitializeTimer(&ptimer->ktimer);

    return 0;
}

int wf_os_api_timer_set (wf_os_api_timer_t *ptimer, wf_u32 intv_ms)
{
#if 0
    return !(KeSetTimer(&ptimer->ktimer, intv_ms * (-1000000 * 10),&ptimer->kdpc) == TRUE);
#else
	return 0;
#endif
}

int wf_os_api_timer_unreg (wf_os_api_timer_t *ptimer)
{
    KeCancelTimer(&ptimer->ktimer);

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


wf_u32 wf_os_api_timestamp (void)
{
#if 0
    PLARGE_INTEGER CurrentCount;
    KeQueryTickCount(CurrentCount);

    return CurrentCount;
#else
	return 0;
#endif
}

wf_u32 wf_os_api_msecs_to_timestamp (wf_u32 msecs)
{
    wf_u32 num = WF_DIV_ROUND_CLOSEST(msecs * 10000, KeQueryTimeIncrement());

    return num ? num : 1;
}

wf_u32 wf_os_api_timestamp_to_msecs (wf_u32 timestamp)
{
    return timestamp * (1000 * 10) * KeQueryTimeIncrement();
}
