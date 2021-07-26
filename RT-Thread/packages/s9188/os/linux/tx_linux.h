#ifndef __TX_LINUX__
#define __TX_LINUX__

void tx_work_init(struct net_device *ndev);
void tx_work_term(struct net_device *ndev);
void tx_work_wake(struct net_device *ndev);

#endif
