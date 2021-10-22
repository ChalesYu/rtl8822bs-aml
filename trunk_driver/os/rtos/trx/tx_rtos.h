
#ifndef __TX_RTOS__
#define __TX_RTOS__

void tx_work_init(nic_info_st *pnic_info);
void tx_work_term(nic_info_st *pnic_info);
void tx_work_wake(nic_info_st *pnic_info);

#endif

