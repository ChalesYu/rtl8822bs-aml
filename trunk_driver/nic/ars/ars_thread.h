#ifndef __ARS_TIMER_H__
#define __ARS_TIMER_H__

#define ARS_TIMESTATP (2000) //ms
#define ODM_NAME_LEN    (32)


typedef struct ars_thread_info_st_
{
    wf_u64 run_times;
    wf_u32 bcn_cnt;
    wf_u32 last_bcn_cnt;


    wf_s8 ars_thread_name[ODM_NAME_LEN];
    void *ars_thread_tid;
    wf_os_api_sema_t   ars_sema;

    wf_os_api_timer_t thread_timer;
}ars_thread_info_st;

wf_s32 ars_thread_sema_post(void *ars);

wf_s32 ars_thread_init(void *ars);
wf_s32 ars_thread_end(void *ars);

#endif
