#ifndef __ARS_TIMER_H__
#define __ARS_TIMER_H__

typedef struct timer_ctl_info_st_
{
    wf_u32 bcn_cnt;
    wf_u32 last_bcn_cnt;
}ars_timer_ctl_info_st;

void ars_timer_handle(wf_os_api_timer_t * timer);

#endif
