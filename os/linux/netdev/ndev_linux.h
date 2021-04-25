#ifndef __NEDV_LINUX_H__
#define __NEDV_LINUX_H__

#include "wf_config.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/rtnetlink.h>
#include <linux/skbuff.h>
#include <linux/wireless.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/ieee80211.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 0, 8)
#include <linux/export.h>
#else
#include <linux/nfsd/export.h>
#endif

#include <linux/types.h>
#include <linux/uaccess.h>
#include <net/iw_handler.h>
#include <net/rtnetlink.h>
#include <net/net_namespace.h>


#include "iw.h"

#include "common.h"

#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
extern rx_pkt_t pkt_temp2;


#if (LINUX_VERSION_CODE>=KERNEL_VERSION(3,0,0))
#include <linux/u64_stats_sync.h>
#endif

typedef struct sk_buff   wf_pkt;

typedef struct
{
    nic_info_st *nic;

    struct iw_statistics  iw_stats;
    struct tasklet_struct recv_task;
    struct tasklet_struct send_task;
    struct tasklet_struct get_tx_data_task;
    struct net_device_stats stats;

    struct
    {
        wf_wlan_ssid_t ssid;
        wf_80211_bssid_t bssid;
    } iw_conn_setting;

} ndev_priv_st;

int ndev_notifier_register(void);
void ndev_notifier_unregister(void);
int ndev_unregister(nic_info_st *nic_info);
int ndev_register(nic_info_st *nic_info);
#ifdef CONFIG_CONCURRENT_MODE
int ndev_vir_open(struct net_device * ndev);
#endif
int ndev_shutdown(nic_info_st *nic_info);

int ndev_rx(nic_info_st *nic_info, struct sk_buff *skb);
ndev_priv_st *ndev_get_priv(nic_info_st *nic_info);
int ndev_open(struct net_device *ndev);

void ndev_tx_resource_enable (struct net_device *ndev, wf_pkt *pkt);
void ndev_tx_resource_disable (struct net_device *ndev, wf_pkt *pkt);



#endif
