#ifndef __P2P_TIMER_H__
#define __P2P_TIMER_H__
typedef struct
{
    wf_timer_t remain_on_ch_timer;
}p2p_timer_st;

typedef struct
{
    wf_u32 duration;
}p2p_timer_param_st;
void p2p_reset_operation_ch(void *pwifidirect_info);
void p2p_reset_invitereq_operation_ch(void *pwifidirect_info);
void p2p_timers_init(nic_info_st * pnic_info);

#endif
