#ifndef __WF_OS_API_TIMER_H__

typedef struct
{
	KTIMER ktimer;
	KDPC kdpc;
	void(*fn) (void *);
	void *pdata;
} wf_os_api_timer_t;

int wf_os_api_timer_reg(wf_os_api_timer_t *ptimer, void(*fn) (void *), void *pdata);
int wf_os_api_timer_set(wf_os_api_timer_t *ptimer, wf_u32 intv_ms);
int wf_os_api_timer_unreg(wf_os_api_timer_t *ptimer);
int wf_os_api_timer_init(void);
int wf_os_api_timer_term(void);

/**
* system tick
*/
wf_u32 wf_os_api_timestamp(void);
wf_u32 wf_os_api_msecs_to_timestamp(wf_u32 msecs);
wf_u32 wf_os_api_timestamp_to_msecs(wf_u32 timestamp);


#endif // !__WF_OS_API_TIMER_H__
