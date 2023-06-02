/*
 * proc.c
 *
 * used for print logs
 *
 * Author: pansiwei
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include "ndev_linux.h"
#include "proc.h"
#include "common.h"
#include "hif.h"

#define PDE_DATA pde_data

char WF_PROC_NAME[20]= { 's','c','i','c','s'};
char demo_var[20] = {'s','s'};

#ifndef __user
#define __user
#endif

static int wf_get_version_info(struct seq_file *m, void *v)
{
#ifdef COMPILE_TIME
    wf_print_seq(m, "Driver Ver:%s, Compile time:%s\n", WF_VERSION, COMPILE_TIME);
#else
    wf_print_seq(m,"Driver Ver:%s\n", WF_VERSION);
#endif
    return 0;
}

static int wf_get_wlan_mgmt_info(struct seq_file *m, void *v)
{
    nic_info_st *pnic_info;
    hif_node_st *hif_info  = m->private;
    wf_wlan_mgmt_info_t *pwlan_mgmt_info;

    if(NULL == hif_info)
    {
        LOG_W("[%s] hif_info is null", __func__);
        return -1;
    }

    pnic_info = hif_info->nic_info[0];
    if(NULL == pnic_info)
    {
        LOG_W("[%s] pnic_info is null", __func__);
        return -1;
    }

    pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    /* ap message free queue */
    if (NULL == pwlan_mgmt_info)
    {
        LOG_W("[%s] pwlan_mgmt_info is null", __func__);
        return -1;
    }

    {
        wf_wlan_mgmt_scan_que_t *pscan_que = &pwlan_mgmt_info->scan_que;
        wf_print_seq(m, "pscan_que->free.count=%d\n", wf_que_count(&pscan_que->free));
        wf_print_seq(m, "pscan_que->ready.count=%d\n", wf_que_count(&pscan_que->ready));
        wf_print_seq(m, "pscan_que->read_cnt=%d\n", pscan_que->read_cnt);
        if (0)
        {
            wf_wlan_mgmt_scan_que_node_t *pscan_que_node;
            wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
            wf_print_seq(m, "-------------------------\n");
            wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
            {
                wf_print_seq(m, "sig_str: %d, ssid: %s\n", pscan_que_node->signal_strength, pscan_que_node->ssid.data);
            }
            wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);
        }
    }

    return 0;
}


static int wf_get_mlme_info (struct seq_file *m, void *v)
{
    nic_info_st *pnic_info;
    hif_node_st *hif_info = m->private;
    mlme_info_t *pmlme_info;
    wf_u8 i = 0;

    if(NULL == hif_info)
    {
        LOG_W("[%s] hif_info is null", __func__);
        return -1;
    }

    for(i=0; i<hif_info->nic_number; i++)
    {
        pnic_info = hif_info->nic_info[i];
        if(NULL == pnic_info)
        {
            LOG_W("[%s] pnic_info is null", __func__);
            continue;
        }

        pmlme_info = pnic_info->mlme_info;
        /* ap message free queue */
        if (NULL == pmlme_info)
        {
            LOG_W("[%s] pnic_info->mlme_info is null", __func__);
            continue;
        }

        {
            wf_print_seq(m, "[%d] pmlme_info->link_info.busy_traffic=%d\n", pnic_info->ndev_id,pmlme_info->link_info.busy_traffic);
        }
    }
    return 0;
}


static int wf_get_rx_info(struct seq_file *m, void *v)
{
    hif_node_st *hif_info           = m->private;
    wdn_net_info_st *wdn_net_info   = NULL;
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

    for(i=0; i<WF_RX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {
        data_node = hif_info->trx_pipe.all_rx_queue + i;
        if( 0 != data_node->state)
        {
            wf_print_seq(m, "[%d] state:%d, pg_num:%d,agg_num:%d\n", data_node->node_id,data_node->state,data_node->pg_num,data_node->agg_num);
        }
    }

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
        if(wdn_net_info->ba_ctl != NULL)
        {
            for(tid=0; tid<TID_NUM; tid++)
            {
                recv_ba_ctrl_st *ba_ctl = &wdn_net_info->ba_ctl[tid];
                if(NULL != ba_ctl && wf_true == ba_ctl->enable)
                {
                    wf_print_seq(m,"[%d] rx reorder drop:%lld\n",tid,ba_ctl->drop_pkts);
                    wf_print_seq(m,"[%d] timeout_cnt:%u\n",tid,ba_ctl->timeout_cnt);
                }
            }
        }
    }

    return 0;
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

    for(i=0; i<WF_TX_MAX_DATA_QUEUE_NODE_NUM; i++)
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
    wf_print_seq(m,"data_queue_check:%d \n",wf_io_write_data_queue_check(pnic_info));
    wf_print_seq(m,"check_tx_buff:%d\n",wf_mcu_check_tx_buff(pnic_info));
    return 0;
}



#ifdef CFG_ENABLE_AP_MODE
static int wf_get_ap_info(struct seq_file *m, void *v)
{
    hif_node_st *hif_info  = m->private;
    nic_info_st *pnic_info = NULL;
    wf_wlan_mgmt_info_t *pwlan_info;
    wf_wlan_network_t *pcur_network;
    wdn_list *pwdn;
    wdn_net_info_st *pwdn_info;
    wf_list_t *pos, *pos_next;
    sec_info_st *psec_info = NULL;
    int i = 0;

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }

    for(i=0; i<hif_info->nic_number; i++)
    {
        pnic_info = hif_info->nic_info[i];
        if(NULL == pnic_info)
        {
            continue;
        }

        wf_print_seq(m, "--------------nic[%d] ----------\n ",i);

        /* ap message free queue */
        pwlan_info = pnic_info->wlan_mgmt_info;
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
            wf_print_seq(m, "pcur_network->channel=%d\n", pcur_network->channel);
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
                wf_print_seq(m, "         ->channel=%d\n", pwdn_info->channel);
                wf_print_seq(m, "         ->mac="WF_MAC_FMT"\n", WF_MAC_ARG(pwdn_info->mac));
                wf_print_seq(m, "         ->ieee8021x_blocked=%d\n", pwdn_info->ieee8021x_blocked);
                wf_print_seq(m, "         ->dot118021XPrivacy=%d\n", pwdn_info->dot118021XPrivacy);
                wf_print_seq(m, "         ->ap_msg.count=%d\n", pwdn_info->ap_msg.cnt);
                wf_print_seq(m, "         ->ap_msg.rx_pkt_stat=%d\n", pwdn_info->rx_pkt_stat);
            }
        }
    }
    return 0;
}


#endif
static int wf_get_sta_info(struct seq_file *m, void *v)
{
    nic_info_st *pnic_info = NULL;
    hif_node_st *hif_info  = m->private;
    wdn_list *pwdn;
    wdn_net_info_st *pwdn_info;
    wf_list_t *pos, *pos_next;
    sec_info_st *psec_info = NULL;
    wf_u8 i = 0;
    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }

    for(i=0; i<hif_info->nic_number; i++)
    {
        pnic_info = hif_info->nic_info[i];
        if(NULL == pnic_info)
        {
            LOG_E("[%s] pnic_info is null", __func__);
            return -1;
        }

        psec_info = pnic_info->sec_info;
        /* ap message free queue */
        if (psec_info)
        {
            wf_print_seq(m, "[%d] psec_info->dot11AuthAlgrthm=%d\n", pnic_info->ndev_id,psec_info->dot11AuthAlgrthm);
        }

        /* wdn message queue */
        pwdn = pnic_info->wdn;
        pwdn_info = pnic_info->wdn;
        if (pwdn)
        {
            wf_print_seq(m, "\n[%d]pwdn->cnt=%d",pnic_info->ndev_id, pwdn->cnt);
            wf_print_seq(m, "\n[%d]pwdn->id_bitmap=0x%x\n", pnic_info->ndev_id,pwdn->id_bitmap);
            wf_list_for_each_safe(pos, pos_next, &pwdn->head)
            {
                pwdn_info = &wf_list_entry(pos, wdn_node_st, list)->info;
                wf_print_seq(m, "[%d]pwdn_info->wdn_id=%d\n", pnic_info->ndev_id,pwdn_info->wdn_id);
                wf_print_seq(m, "[%d]         ->mac="WF_MAC_FMT"\n", pnic_info->ndev_id,WF_MAC_ARG(pwdn_info->mac));
                wf_print_seq(m, "[%d]         ->ieee8021x_blocked=%d\n",pnic_info->ndev_id, pwdn_info->ieee8021x_blocked);
                wf_print_seq(m, "[%d]         ->dot118021XPrivacy=%d\n",pnic_info->ndev_id, pwdn_info->dot118021XPrivacy);
                wf_print_seq(m, "[%d]         ->rx_pkt_stat=%d\n",pnic_info->ndev_id, pwdn_info->rx_pkt_stat);
                wf_print_seq(m, "[%d]         ->rx_pkt_stat_last=%d\n",pnic_info->ndev_id, pwdn_info->rx_pkt_stat_last);
            }
        }
    }
    return 0;
}

static int wf_get_hif_info(struct seq_file *m, void *v)
{
    hif_node_st *hif_info           = m->private;
    int i = 0;
    data_queue_node_st *data_node   = NULL;

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }

    /*hif debug info*/
    wf_print_seq(m, "node_id : %d, nic_num:%d\n", hif_info->node_id,hif_info->nic_number);
    wf_print_seq(m, "hif_type: %s\n", hif_info->hif_type==1?"HIF_USB":"HIF_SDIO");

    /*hif--rx info*/
    wf_print_seq(m, "[rx] all  queue cnt:%lld\n", hif_info->trx_pipe.rx_queue_cnt);
    wf_print_seq(m, "[rx] free queue node num:%d\n", hif_info->trx_pipe.free_rx_queue.cnt);

    for(i=0; i<WF_RX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {
        data_node = hif_info->trx_pipe.all_rx_queue + i;
        if( 0 != data_node->state)
        {
            wf_print_seq(m, "[rx] qnode(%d) state:%d, pg_num:%d,agg_num:%d\n", data_node->node_id,data_node->state,data_node->pg_num,data_node->agg_num);
        }
    }
    if(HIF_SDIO == hif_info->hif_type)
    {
        hif_sdio_st *sd = &hif_info->u.sdio;
        wf_print_seq(m,"[rx] rx_time:%lld ms\n",sd->rx_time );
        wf_print_seq(m,"[rx] rx_count:%lld\n",sd->rx_count );
        wf_print_seq(m,"[rx] rx_pkt_num:%lld\n",sd->rx_pkt_num );
    }

    /*hif--tx info*/
    wf_print_seq(m, "[tx] all queue cnt:%lld\n", hif_info->trx_pipe.tx_queue_cnt);
    wf_print_seq(m, "[tx] free  tx data queue node num:%d\n", hif_info->trx_pipe.free_tx_queue.cnt);
    wf_print_seq(m, "[tx] using tx data queue node num:%d\n", hif_info->trx_pipe.tx_queue.cnt);
    for(i=0; i<WF_TX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {
        data_node = hif_info->trx_pipe.all_tx_queue + i;
        if((TX_STATE_COMPETE != data_node->state) && (TX_STATE_IDL != data_node->state))
        {
            wf_print_seq(m, "[tx] qnode(%d) state:%d, pg_num:%d,agg_num:%d, addr:0x%x\n", data_node->node_id,data_node->state,data_node->pg_num,data_node->agg_num,data_node->addr);
        }
    }
    if(HIF_SDIO == hif_info->hif_type)
    {
        hif_sdio_st *sd = &hif_info->u.sdio;

        wf_print_seq(m,"[tx] fifo_ppg_num    :%d\n",sd->tx_fifo_ppg_num);
        wf_print_seq(m,"[tx] fifo_hpg_num    :%d\n",sd->tx_fifo_hpg_num);
        wf_print_seq(m,"[tx] fifo_lpg_num    :%d\n",sd->tx_fifo_lpg_num);
        wf_print_seq(m,"[tx] tx_state:%d\n",sd->tx_state);
        wf_print_seq(m,"[tx] tx_all_time:%lld ms\n",sd->tx_all_time);
        wf_print_seq(m,"[tx] tx_flow_ctl_time:%lld ms\n",sd->tx_flow_ctl_time);
        wf_print_seq(m,"[tx] tx_agg_send_time:%lld ms\n",sd->tx_agg_send_time);
        wf_print_seq(m,"[tx] tx_agg_num:%lld \n",sd->tx_agg_num);
        wf_print_seq(m,"[tx] tx_pkt_num:%lld \n",sd->tx_pkt_num);
        wf_print_seq(m,"[tx] count:%lld \n",sd->count);
    }

    /*register info*/
    {
        nic_info_st *pnic_info = hif_info->nic_info[0];
        if(NULL == pnic_info)
        {
            LOG_E("[%s] pnic_info is null",__func__);
            return -1;
        }

        /*operate register*/
        {
            int ret;
            wf_u32 cnt;
            wf_io_write32(pnic_info, 0x664, (3 << 28)); /* [26], [31:28] = 3 */
            cnt = wf_io_read32(pnic_info, 0x664, &ret);
            if (ret)
            {
                wf_print_seq(m,"[reg] cck ok cnt read fail\n");
            }
            else
            {
                wf_print_seq(m,"[reg] cck ok cnt:%d\n", cnt & ((1 << 23) - 1)); /* [22:0] */
            }
        }
        {
            int ret;
            wf_u32 cnt;
            wf_io_write32(pnic_info, 0x664, (0 << 28)); /* 26, 31:28 = 0 */
            cnt = wf_io_read32(pnic_info, 0x664, &ret);
            if (ret)
            {
                wf_print_seq(m,"[reg] OFDM ok cnt read fail\n");
            }
            else
            {
                wf_print_seq(m,"[reg] OFDM ok cnt:%d\n", cnt & ((1 << 23) - 1)); /* [22:0] */
            }
        }
        {
            int ret;
            wf_u32 cnt;
            wf_io_write32(pnic_info, 0x664, (6 << 28)); /* 26, 31:28 = 0 */
            cnt = wf_io_read32(pnic_info, 0x664, &ret);
            if (ret)
            {
                wf_print_seq(m,"[reg] HT ok cnt read fail\n");
            }
            else
            {
                wf_print_seq(m,"[reg] HT ok cnt:%d\n", cnt & ((1 << 23) - 1)); /* [22:0] */
            }
        }
#if 0
        {
            int ret;
            wf_u32 cnt;
            wf_io_write32(pnic_info, 0x664, (16 << 28)); /* 26, 31:28 = 0 */
            cnt = wf_io_read32(pnic_info, 0x664, &ret);
            if (ret)
            {
                wf_print_seq(m,"[reg] VHT ok cnt read fail\n");
            }
            else
            {
                wf_print_seq(m,"[reg] VHT ok cnt:%d\n", cnt & ((1 << 23) - 1)); /* [22:0] */
            }
        }
#endif
    }
    return 0;
}

static int wf_get_phy_info(struct seq_file *m, void *v)
{
    hif_node_st *hif_info           = m->private;
    int i                          = 0;

    wf_u32 read_tx_reg[]= {0x9cc,0x9d0,0xf14,0xf18};
    wf_u32 read_rx_ofdm_reg[]= {0xda0,0xda4,0xf90,0xf94,0xf98,0xf9c};
    wf_u32 read_rx_cck_reg[]= {0xfa0,0xf84,0xf88};

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }

    wf_print_seq(m,"tx_reg:\n");
    for(i=0 ; i < (sizeof(read_tx_reg)/sizeof(read_tx_reg[0])) ; i++)
    {
        wf_print_seq(m,"0x%04x:0x%08x\n",read_tx_reg[i],hif_io_read32(hif_info, read_tx_reg[i],NULL));
    }

    wf_print_seq(m,"rx_ofdm_reg:\n");
    for(i=0; i < (sizeof(read_rx_ofdm_reg)/sizeof(read_rx_ofdm_reg[0])) ; i++)
    {
        wf_print_seq(m,"0x%04x:0x%08x\n",read_rx_ofdm_reg[i],hif_io_read32(hif_info, read_rx_ofdm_reg[i],NULL));
    }

    wf_print_seq(m,"read_rx_cck_reg:\n");
    for(i=0; i < (sizeof(read_rx_cck_reg)/sizeof(read_rx_cck_reg[0])); i++)
    {
        wf_print_seq(m,"0x%04x:0x%08x\n",read_rx_cck_reg[i],hif_io_read32(hif_info, read_rx_cck_reg[i],NULL));
    }

    return 0;
}

static int wf_get_mac_info(struct seq_file *m, void *v)
{
    hif_node_st *hif_info           = m->private;
    int i                           = 0;
    wf_u32 mac_reg[]= {0x284,0x288,0x114,0x200,0x204,0x208,0x20c,0x210,0x214,0x218,0x21c};

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }

    wf_print_seq(m,"mac_reg:\n");
    for(i=0; i < sizeof(mac_reg)/sizeof(mac_reg[0]); i++)
    {
        wf_print_seq(m,"0x%04x:0x%08x\n",mac_reg[i],hif_io_read32(hif_info, mac_reg[i],NULL));
    }

    return 0;
}

static int wf_get_phy_cnt_reset(struct seq_file *m, void *v)
{
    wf_u32 reg;
    hif_node_st *hif_info           = m->private;
    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return -1;
    }

    reg = hif_io_read32(hif_info, 0xf14,NULL);
    if(reg & (1<<16))
    {
        wf_print_seq(m,"1\n");
    }
    else
    {
        wf_print_seq(m,"0\n");
    }

    return 0;
}
SSIZE_T wf_set_phy_cnt_reset(struct file *file, const char __user *buffer, SIZE_T count, loff_t *pos, void *data)
{
    //hif_node_st *hif_info           = m->private;
    //wf_u32 reg;
    LOG_W("wf_set_phy_cnt_reset");

#if 0

    reg = hif_io_read32(hif_info, 0xf14,NULL);
    reg |= 1<<16;
    hif_io_write32(hif_info,0xf14,reg);

    reg = hif_io_read32(hif_info, 0xf14,NULL);
    reg &= ~(1<<16);
    hif_io_write32(hif_info,0xf14,reg);
    wf_print_seq(m,"reset phy cnt\n");
#endif
    return 0;
}

const struct wf_proc_handle proc_hdls[] =
{
    wf_register_proc_interface("version",   wf_get_version_info,   NULL),
    wf_register_proc_interface("tx",        wf_get_tx_info,        NULL),
    wf_register_proc_interface("rx",        wf_get_rx_info,        NULL),
#ifdef CFG_ENABLE_AP_MODE
    wf_register_proc_interface("ap",        wf_get_ap_info,        NULL),
#endif
    wf_register_proc_interface("sta",       wf_get_sta_info,       NULL),
    wf_register_proc_interface("hif",       wf_get_hif_info,       NULL),
    wf_register_proc_interface("wlan_mgmt", wf_get_wlan_mgmt_info, NULL),
    wf_register_proc_interface("mlme",      wf_get_mlme_info,      NULL),
    wf_register_proc_interface("mac",       wf_get_mac_info,       NULL),
    wf_register_proc_interface("phy",       wf_get_phy_info,       NULL),
    wf_register_proc_interface("phy_cnt_reset",   wf_get_phy_cnt_reset,  NULL),

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


