
#ifndef __TX_RTOS__
#define __TX_RTOS__

void tx_work_init(wlan_dev_t *wlan);
void tx_work_term(wlan_dev_t *wlan);
void tx_work_wake(wlan_dev_t *wlan);

#endif

