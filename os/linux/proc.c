#include "wf_debug.h"
#include "ndev_linux.h"
#include "proc.h"
#include "common.h"
#include "hif.h"

char WF_PROC_NAME[20]={ 's','c','i','c','s'};
char demo_var[20] = {'s','s'};

#define __user



static int wf_get_demo(struct seq_file *m, void *v)
{
    wf_print_seq(m, "%s %s\n",WF_PROC_NAME, demo_var);
    return 0;
}

static ssize_t wf_set_demo(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
    char tmp[64];

    if (count < 1)
    {
        return -EINVAL;
    }

    if (count > sizeof(tmp))
    {
        printk("[%s] warning: do not have enough space",__func__);
        return -EFAULT;
    }

    if (buffer && !copy_from_user(tmp, buffer, count))
    {
        sscanf(tmp,"%s", demo_var);
        LOG_D("[%s] set info: %s\n", __func__, demo_var);
    }
    else
    {
        return -EFAULT;
    }

    return count;
}


static int wf_get_version_info(struct seq_file *m, void *v)
{
#ifdef COMPILE_TIME
    wf_print_seq(m, "Driver Ver:%s, Compile time:%s\n", WF_VERSION, COMPILE_TIME);
#else
    wf_print_seq(m,"Driver Ver:%s\n", WF_VERSION);
#endif
	return 0;
}


static int wf_get_rx_info(struct seq_file *m, void *v)
{
    hif_node_st *hif_info           = m->private;
    nic_info_st *pnic_info          = NULL;
    wdn_net_info_st *wdn_net_info   = NULL;
    rx_info_t *rx_info              = NULL;
	data_queue_node_st *data_node   = NULL;
    int i                           = 0;
    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }


    /*hif debug info*/
    wf_print_seq(m, "node_id:%d\n", hif_info->node_id);
    wf_print_seq(m, "hif_type:%d\n", hif_info->hif_type);
    wf_print_seq(m, "rx_queue_cnt:%lld\n", hif_info->trx_pipe.rx_queue_cnt);
    wf_print_seq(m, "free rx data queue node num:%d\n", hif_info->trx_pipe.free_rx_queue.cnt);

    for(i=0;i<WF_RX_MAX_DATA_QUEUE_NODE_NUM;i++)
    {
        data_node = hif_info->trx_pipe.all_rx_queue + i;
        if( 0 != data_node->state)
        {
            wf_print_seq(m, "[%d] state:%d, pg_num:%d,agg_num:%d\n", data_node->node_id,data_node->state,data_node->pg_num,data_node->agg_num);
        }
    }

    /*odm debug info*/
    do{
        odm_mgnt_st *odm                = NULL;
        odm_phy_info_st *odm_phy        = NULL;
        int     i = 0;
        pnic_info = hif_info->nic_info[0];
        if(NULL == pnic_info)
        {
            break;
        }
        odm = (odm_mgnt_st *)pnic_info->odm;
        wf_print_seq(m,"wdg_exec_cnt:%d,bcn_cnt:%d,last_bcn_cnt:%d\n",odm->wdg_exec_cnt,odm->bcn_cnt,odm->last_bcn_cnt);
        wdn_net_info  = wf_wdn_find_info(pnic_info,wf_wlan_get_cur_bssid(pnic_info));
        if(NULL != wdn_net_info)
        {
             odm_phy = &odm->phy[wdn_net_info->wdn_id];
             wf_print_seq(m,"signal_qual:%d,signal_strength:%d\n",odm_phy->phy_status.signal_qual,odm_phy->phy_status.signal_strength);
             wf_print_seq(m,"is_cck_rate:%d, rx_rate:%d\n",odm_phy->is_cck_rate,odm_phy->rx_rate);
             for(i=0;i<28;i++)
            {
                wf_print_seq(m,"0x%x ",odm_phy->raw_phystatus[i]);
                if((i+1)%16 == 0)
                {
                    wf_print_seq(m,"\n");
                }
            }
            wf_print_seq(m,"\n");
        }

        rx_info = pnic_info->rx_info;
        wf_print_seq(m, "m0_buf[0]:%d, m0_buf[1]:%d,data_rate:%d\n", rx_info->m0_rxbuf[0],rx_info->m0_rxbuf[1],rx_info->m0_rxbuf[2]);
    }while(0);

    if(HIF_SDIO == hif_info->hif_type)
    {
        hif_sdio_st *sd = &hif_info->u.sdio;

        wf_print_seq(m,"irq_cnt:%lld, int_flag:%d\n",sd->irq_cnt,sd->int_flag);
        wf_print_seq(m,"0x9008:0x%08x\n",hif_io_read32(hif_info, SDIO_BASE | WL_REG_HISR,NULL));
        wf_print_seq(m,"0x284:0x%08x\n",hif_io_read32(hif_info, 0x284,NULL));
        wf_print_seq(m,"0x288:0x%08x\n",hif_io_read32(hif_info, 0x288,NULL));
        wf_print_seq(m,"0x114:0x%08x\n",hif_io_read32(hif_info, 0x114,NULL));
        wf_print_seq(m,"0x200:0x%08x\n",hif_io_read32(hif_info, 0x200,NULL));
        wf_print_seq(m,"0x204:0x%08x\n",hif_io_read32(hif_info, 0x204,NULL));
        wf_print_seq(m,"0x208:0x%08x\n",hif_io_read32(hif_info, 0x208,NULL));
        wf_print_seq(m,"0x20C:0x%08x\n",hif_io_read32(hif_info, 0x20C,NULL));
        wf_print_seq(m,"0x210:0x%08x\n",hif_io_read32(hif_info, 0x210,NULL));
        wf_print_seq(m,"0x214:0x%08x\n",hif_io_read32(hif_info, 0x214,NULL));
        wf_print_seq(m,"0x218:0x%08x\n",hif_io_read32(hif_info, 0x218,NULL));
        wf_print_seq(m,"0x21c:0x%08x\n",hif_io_read32(hif_info, 0x21c,NULL));
    }

    wf_print_seq(m,"rx skb queue_len:%d\n",skb_queue_len(&hif_info->trx_pipe.rx_queue));
    wf_print_seq(m,"free rx skb queue_len:%d\n",skb_queue_len(&hif_info->trx_pipe.free_rx_queue_skb));

    if( NULL != wdn_net_info)
    {
        int tid = 0;
        for(tid=0;tid<TID_NUM;tid++)
        {
            recv_ba_ctrl_st *ba_ctl = &wdn_net_info->ba_ctl[tid];
            if(NULL != ba_ctl && wf_true == ba_ctl->enable)
            {
                wf_print_seq(m,"[%d] rx reorder drop:%lld\n",tid,ba_ctl->drop_pkts);
                wf_print_seq(m,"[%d] timeout_cnt:%u\n",tid,ba_ctl->timeout_cnt);
            }
        }
    }

    return 0;
}

static ssize_t wf_set_rx_info(struct file * file, const char __user * buffer, size_t count, loff_t * pos, void * data)
{
    hif_node_st *hif_info   = data;

    if(1 > count)
    {
        return -EFAULT;
    }

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }

    LOG_I("[%s] node_id:%d\n",__func__, hif_info->node_id);
    LOG_I("[%s] hif_type:%d\n",__func__, hif_info->hif_type);
    return count;
}

static int wf_get_tx_info(struct seq_file *m, void *v)
{
    hif_node_st *hif_info           = m->private;
    nic_info_st *pnic_info           = NULL;
    tx_info_st *tx_info             = NULL;
	data_queue_node_st *data_node   = NULL;
    int i                           = 0;
    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }


    /*hif debug info*/
    wf_print_seq(m, "node_id:%d\n", hif_info->node_id);
    wf_print_seq(m, "hif_type:%d\n", hif_info->hif_type);
    wf_print_seq(m, "tx_queue_cnt:%lld\n", hif_info->trx_pipe.tx_queue_cnt);
    wf_print_seq(m, "free tx data queue node num:%d\n", hif_info->trx_pipe.free_tx_queue.cnt);
    wf_print_seq(m, "tx data queue node num:%d\n", hif_info->trx_pipe.tx_queue.cnt);

    for(i=0;i<WF_TX_MAX_DATA_QUEUE_NODE_NUM;i++)
    {
        data_node = hif_info->trx_pipe.all_tx_queue + i;
        if((TX_STATE_COMPETE != data_node->state) && (TX_STATE_IDL != data_node->state))
        {
            wf_print_seq(m, "[%d] state:%d, pg_num:%d,agg_num:%d, addr:0x%x\n", data_node->node_id,data_node->state,data_node->pg_num,data_node->agg_num,data_node->addr);
        }
    }

    if(HIF_SDIO == hif_info->hif_type)
    {
        hif_sdio_st *sd = &hif_info->u.sdio;

        wf_print_seq(m,"tx_fifo_ppg_num    :%d\n",sd->tx_fifo_ppg_num);
        wf_print_seq(m,"tx_fifo_hpg_num    :%d\n",sd->tx_fifo_hpg_num);
        wf_print_seq(m,"tx_fifo_lpg_num    :%d\n",sd->tx_fifo_lpg_num);
    }

    pnic_info = hif_info->nic_info[0];
    if(NULL == pnic_info)
    {

        return 0;
    }

    tx_info = pnic_info->tx_info;
    wf_print_seq(m,"free tx frame num:%d,free_xmitbuf_cnt:%d\n",tx_info->free_xmitframe_cnt,tx_info->free_xmitbuf_cnt);

    return 0;
}

static ssize_t wf_set_tx_info(struct file * file, const char __user * buffer, size_t count, loff_t * pos, void * data)
{
    hif_node_st *hif_info   = data;

    if(1 > count)
    {
        return -EFAULT;
    }

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }

    LOG_I("[%s] node_id:%d\n",__func__, hif_info->node_id);
    LOG_I("[%s] hif_type:%d\n",__func__, hif_info->hif_type);
    return count;
}


#ifdef CFG_ENABLE_AP_MODE
static int wf_get_ap_info(struct seq_file *m, void *v)
{
    hif_node_st *hif_info  = m->private;
    nic_info_st *pnic_info = NULL;
    wf_wlan_info_t *pwlan_info;
    wf_wlan_network_t *pcur_network;
    wdn_list *pwdn;
    wdn_net_info_st *pwdn_info;
    wf_list_t *pos, *pos_next;
    sec_info_st *psec_info = NULL;

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }
    pnic_info = hif_info->nic_info[0];
    if(NULL == pnic_info)
    {
        return -1;
    }


    /* ap message free queue */
    pwlan_info = pnic_info->wlan_info;
    if (pwlan_info)
    {
        pcur_network = &pwlan_info->cur_network;
        wf_print_seq(m, "ap_msg_free[WF_AP_MSG_TAG_AUTH_FRAME].count=%d\n",
                     pcur_network->ap_msg_free[WF_AP_MSG_TAG_AUTH_FRAME].cnt);
        wf_print_seq(m, "ap_msg_free[WF_AP_MSG_TAG_ASSOC_REQ_FRAME].count=%d\n",
                     pcur_network->ap_msg_free[WF_AP_MSG_TAG_ASSOC_REQ_FRAME].cnt);
        wf_print_seq(m, "ap_msg_free[WF_AP_MSG_TAG_BEACON_REQ_FRAME].count=%d\n",
                     pcur_network->ap_msg_free[WF_AP_MSG_TAG_BEACON_FRAME].cnt);
        wf_print_seq(m, "pcur_network->ap_tid=%08lx\n", (long)pcur_network->ap_tid);
    }

    psec_info = pnic_info->sec_info;
    if (psec_info)
    {
        wf_print_seq(m, "psec_info->dot11AuthAlgrthm=%d\n", psec_info->dot11AuthAlgrthm);
    }

    /* wdn message queue */
    pwdn = pnic_info->wdn;
    pwdn_info = pnic_info->wdn;
    if (pwdn)
    {
        wf_print_seq(m, "\npwdn->cnt=%d", pwdn->cnt);
        wf_print_seq(m, "\npwdn->id_bitmap=0x%x\n", pwdn->id_bitmap);
        wf_list_for_each_safe(pos, pos_next, &pwdn->head)
        {
            pwdn_info = &wf_list_entry(pos, wdn_node_st, list)->info;
            wf_print_seq(m, "pwdn_info->wdn_id=%d\n", pwdn_info->wdn_id);
            wf_print_seq(m, "         ->mac="WF_MAC_FMT"\n", WF_MAC_ARG(pwdn_info->mac));
            wf_print_seq(m, "         ->ieee8021x_blocked=%d\n", pwdn_info->ieee8021x_blocked);
            wf_print_seq(m, "         ->dot118021XPrivacy=%d\n", pwdn_info->dot118021XPrivacy);
            wf_print_seq(m, "         ->ap_msg.count=%d\n", pwdn_info->ap_msg.cnt);
            wf_print_seq(m, "         ->ap_msg.rx_pkt_stat=%d\n", pwdn_info->rx_pkt_stat);
        }
    }

    return 0;
}

static ssize_t wf_set_ap_info(struct file * file, const char __user * buffer, size_t count, loff_t * pos, void * data)
{
    hif_node_st *hif_info   = data;

    if(1 > count)
    {
        return -EFAULT;
    }

    if(NULL == hif_info)
    {
        LOG_E("[%s] pnic_info is null",__func__);
        return -1;
    }

    LOG_I("[%s] node_id:%d\n",__func__, hif_info->node_id);
    LOG_I("[%s] hif_type:%d\n",__func__, hif_info->hif_type);
    return count;
}
#else
static int wf_get_sta_info(struct seq_file *m, void *v)
{
    nic_info_st *pnic_info = NULL;
    hif_node_st *hif_info  = m->private;
    wdn_list *pwdn;
    wdn_net_info_st *pwdn_info;
    wf_list_t *pos, *pos_next;
    sec_info_st *psec_info = NULL;

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }

    pnic_info = hif_info->nic_info[0];
    if(NULL == pnic_info)
    {
        LOG_E("[%s] pnic_info is null", __func__);
        return -1;
    }

    psec_info = pnic_info->sec_info;
    /* ap message free queue */
    if (psec_info)
    {
        wf_print_seq(m, "psec_info->dot11AuthAlgrthm=%d\n", psec_info->dot11AuthAlgrthm);
    }

    /* wdn message queue */
    pwdn = pnic_info->wdn;
    pwdn_info = pnic_info->wdn;
    if (pwdn)
    {
        wf_print_seq(m, "\npwdn->cnt=%d", pwdn->cnt);
        wf_print_seq(m, "\npwdn->id_bitmap=0x%x\n", pwdn->id_bitmap);
        wf_list_for_each_safe(pos, pos_next, &pwdn->head)
        {
            pwdn_info = &wf_list_entry(pos, wdn_node_st, list)->info;
            wf_print_seq(m, "pwdn_info->wdn_id=%d\n", pwdn_info->wdn_id);
            wf_print_seq(m, "         ->mac="WF_MAC_FMT"\n", WF_MAC_ARG(pwdn_info->mac));
            wf_print_seq(m, "         ->ieee8021x_blocked=%d\n", pwdn_info->ieee8021x_blocked);
            wf_print_seq(m, "         ->dot118021XPrivacy=%d\n", pwdn_info->dot118021XPrivacy);
        }
    }

    return 0;
}

static ssize_t wf_set_sta_info(struct file * file, const char __user * buffer, size_t count, loff_t * pos, void * data)
{
    hif_node_st *hif_info   = data;

    if(1 > count)
    {
        return -EFAULT;
    }

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }

    LOG_I("[%s] node_id:%d\n",__func__, hif_info->node_id);
    LOG_I("[%s] hif_type:%d\n",__func__, hif_info->hif_type);
    return count;
}

#endif

static int wf_get_rtx_info(struct seq_file *m, void *v)
{
    nic_info_st *pnic_info          = NULL;
    hif_node_st *hif_info           = m->private;
    rx_info_t *rx_info              = NULL;
    tx_info_st *tx_info             = NULL;
    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }

    pnic_info = hif_info->nic_info[0];
    if(NULL == pnic_info)
    {
        LOG_E("[%s] pnic_info is null",__func__);
        return -1;
    }
    wf_print_seq(m,"------------------------------------------------------\n");

    /*rx_info*/
    rx_info = pnic_info->rx_info;
    wf_print_seq(m,"rx_total:%lld, rx_drop:%lld, rx_pkts:%lld\n",
                rx_info->rx_total_pkts, rx_info->rx_drop,rx_info->rx_pkts);
    wf_print_seq(m,"rx mgnt pkts:%lld\n",rx_info->rx_mgnt_pkt);
    wf_print_seq(m,"rx data pkts:%lld\n",rx_info->rx_data_pkt);

    /*tx_info*/
    tx_info = pnic_info->tx_info;
    wf_print_seq(m,"tx data dropped:%lld, tx data pkts:%lld\n",tx_info->tx_drop,tx_info->tx_pkts);

    return 0;
}

static ssize_t wf_set_rtx_info(struct file * file, const char __user * buffer, size_t count, loff_t * pos, void * data)
{
    hif_node_st *hif_info   = data;

    if(1 > count)
    {
        return -EFAULT;
    }

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }


    return count;
}



const struct wf_proc_handle proc_hdls[] =
{
    wf_register_proc_interface("demo",wf_get_demo, wf_set_demo),
    wf_register_proc_interface("tx", wf_get_tx_info, wf_set_tx_info),
    wf_register_proc_interface("rx", wf_get_rx_info, wf_set_rx_info),
#ifdef CFG_ENABLE_AP_MODE
    wf_register_proc_interface("ap_info", wf_get_ap_info, wf_set_ap_info),
#else
    wf_register_proc_interface("ap_info", wf_get_sta_info, wf_set_sta_info),
#endif
    wf_register_proc_interface("rtx_info", wf_get_rtx_info, wf_set_rtx_info),
    wf_register_proc_interface("version", wf_get_version_info, NULL),
};
const int wf_proc_hdls_num = sizeof(proc_hdls) / sizeof(struct wf_proc_handle);




inline struct proc_dir_entry * wf_proc_create_dir (const char *name, struct proc_dir_entry *parents, void *data)
{
    struct proc_dir_entry *entry;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
#if 1
    entry = proc_mkdir_data(name, S_IRUGO | S_IXUGO, parents, data);
#else
    entry = proc_mkdir(name, parents);
    if(!entry)
    {
        LOG_E("[proc_mkdir]1 error!\n");
    }
#endif
#else
    /* entry = proc_mkdir_mode(name, S_IRUGO|S_IXUGO, parent); */
    entry = proc_mkdir(name, parents);
    if(!entry)
    {
        LOG_E("[proc_mkdir]2 error!\n");
    }
    if (entry)
    {
        entry->data = data;
    }
#endif

    return entry;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 5, 0)
inline struct proc_dir_entry *wf_proc_create_entry(const char *name, struct proc_dir_entry *parents,
    const struct proc_ops *fops, void * data)
#else
inline struct proc_dir_entry *wf_proc_create_entry(const char *name, struct proc_dir_entry *parents,
    const struct file_operations *fops, void * data)
#endif
{
    struct proc_dir_entry *entry;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26))
    entry = proc_create_data(name,  S_IFREG | S_IRUGO | S_IWUGO, parents, fops, data);
#else
    entry = create_proc_entry(name, S_IFREG | S_IRUGO | S_IWUGO, parents);
    if (entry)
    {
        entry->data = data;
        entry->proc_fops = fops;
    }
#endif

    return entry;
}

static SSIZE_T wf_proc_write(struct file *file, const char __user *buffer, SIZE_T count, loff_t *pos)
{
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 9, 1))
return 0;
#else

    ssize_t index = (ssize_t)PDE_DATA(file_inode(file));
    const struct wf_proc_handle *hdl = proc_hdls + index;
    ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *, void *) = hdl->write;

    if (write)
    {
        return write(file, buffer, count, pos, ((struct seq_file *)file->private_data)->private);
    }

    return -EROFS;
#endif
}

static int wf_proc_open(struct inode *inode, struct file *file)
{
    ssize_t index = (ssize_t)PDE_DATA(inode);
    const struct wf_proc_handle *hdl = proc_hdls + index;
    void *private = proc_get_parent_data(inode);

    int (*show)(struct seq_file *, void *) = hdl->show ? hdl->show : 0;

    return single_open(file, show, private);

}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 5, 0)
static const struct proc_ops wf_proc_fops =
{
    //.owner = THIS_MODULE,
    .proc_open = wf_proc_open,
    .proc_read = seq_read,
    .proc_write = wf_proc_write,
    .proc_lseek = default_llseek,
    .proc_release = single_release,
};
#else
static const struct file_operations wf_proc_fops =
{
    .owner = THIS_MODULE,
    .open = wf_proc_open,
    .read = seq_read,
    .write = wf_proc_write,
    .llseek = seq_lseek,
    .release = single_release,
};
#endif
int wf_proc_init(void *hif_info)
{
    int ret = wf_false;
    SSIZE_T p;
    hif_node_st *hif_node = (hif_node_st *)hif_info;
    wf_proc_st   *proc_info      = NULL;
    struct proc_dir_entry *entry = NULL;

    proc_info   = wf_kzalloc(sizeof(wf_proc_st));
    if ( NULL == proc_info )
    {
       LOG_E("[%s] malloc proc_info failed", __func__);
       return WF_RETURN_FAIL;
    }


    LOG_D("[%s] start\n", __func__);

    if (hif_node->hif_type == HIF_USB)
    {

        sprintf(proc_info->proc_name,"wlan%d_u%d",hif_node->node_id,hif_node->u.usb.usb_id);
    }
    else
    {
        sprintf(proc_info->proc_name,"wlan%d_s%d",hif_node->node_id,hif_node->u.sdio.sdio_id);
    }

    proc_info->proc_root = wf_proc_create_dir(proc_info->proc_name, wf_proc_net, hif_node);
    if (NULL == proc_info->proc_root)
    {
        LOG_E("[%s]proc dir create error", __func__);
    }

    for (p = 0; p < wf_proc_hdls_num; p++)
    {

        entry = wf_proc_create_entry(proc_hdls[p].name, proc_info->proc_root, &wf_proc_fops, (void *)p);
        if (!entry)
        {
            LOG_E("[%s]proc entry create error", __func__);
        }
    }

    proc_info->hif_info = hif_info;
    hif_node->proc_info = proc_info;


    return ret;

}
void wf_proc_term(void *hif_info)
{
    int i;
    hif_node_st *hif_node        = hif_info;
    wf_proc_st   *proc_info      = hif_node->proc_info;

    if (proc_info == NULL)
    {
        return;
    }

    if (proc_info->proc_root == NULL)
    {
        return;
    }

    for (i = 0; i < wf_proc_hdls_num; i++)
    {
        remove_proc_entry(proc_hdls[i].name, proc_info->proc_root);
    }

    remove_proc_entry(proc_info->proc_name, wf_proc_net);
    proc_info->proc_root = NULL;

    wf_kfree(proc_info);
    proc_info = NULL;

}


