#ifndef __ARS_TIMER_H__
#define __ARS_TIMER_H__

#define ARS_TIMESTATP (2000) //ms
#define ODM_NAME_LEN    (32)

typedef struct ars_dbg_info_s{
    FALSE_ALARM_STATISTICS false_alarm_cnt;
    wf_u8 noisy_decision;
    wf_u8 rssi_min;
    wf_u8 pwdb;
    wf_u8 change_state;
    wf_u32 pt_score;
    wf_u8 curr_igi;
    wf_u8 igi_dynamic_min;
    wf_u8 igi_target;
    wf_u8 rssi_level;
    wf_u32 crystal_cap;
    wf_s32 cfo_avg_pre;
    wf_u8 thermal_val;
    wf_u8 thermal_lck;
    wf_u8 thermal_iqk;
    wf_u8 dpk_thermal;

    wf_u32 sq;
}ars_dbg_info_t;


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

wf_s32 odm_FalseAlarmCounterStatistics(void *ars);
void ars_dump_info(nic_info_st *nic_info);

wf_s32 ars_thread_sema_post(void *ars);

wf_s32 ars_thread_init(void *ars);
wf_s32 ars_thread_end(void *ars);

#endif
