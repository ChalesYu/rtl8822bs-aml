/*
 * hif_queue.c
 *
 * used for .....
 *
 * Author: luozhi
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

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/usb.h>

#include "common.h"
#include "wf_que.h"
#include "hif_queue.h"
#include "hif.h"
#include "rx_linux.h"

#define TX_AGG_BUFF_SIZE (32*1024)

static int wf_data_queue_full(wf_que_t *queue,int queue_node_num)
{
    return (queue_node_num == wf_que_count(queue));
}

int wf_data_queue_insert(wf_que_t *queue, data_queue_node_st *qnode)
{
    //LOG_I("[%s] num:%d",__func__,queue->num);
    wf_enque_tail(&qnode->node,queue);
    return queue->cnt;
}

data_queue_node_st * wf_data_queue_remove(wf_que_t *queue)
{
    data_queue_node_st *rb_node     = NULL;
    wf_list_t    *head              = NULL;

    head    = wf_deque_head(queue);
    if(NULL != head)
    {
        rb_node = wf_list_entry(head, data_queue_node_st, node);
    }

    return rb_node;
}


data_queue_node_st * wf_queue_node_malloc(int cnt)
{
    data_queue_node_st *node = NULL;

    node = wf_kzalloc(cnt * sizeof(data_queue_node_st));
    if(NULL == node)
    {
        LOG_E("[%s] wf_kzalloc failed ,check!!!!", __func__);
        return NULL;
    }

    return node;
}

int wf_hif_queue_alloc_skb_one(struct sk_buff_head *skb_head,wf_u8 hif_type)
{
    struct sk_buff *pskb    = NULL;
    SIZE_PTR tmpaddr        = 0;
    SIZE_PTR alignment      = 0;
    if(hif_type == HIF_USB)
    {
        pskb = wf_alloc_skb(WF_MAX_RECV_BUFF_LEN_USB + HIF_QUEUE_ALLOC_SKB_ALIGN_SZ);
    }
    else
    {
        pskb = wf_alloc_skb(WF_MAX_RECV_BUFF_LEN_SDIO + HIF_QUEUE_ALLOC_SKB_ALIGN_SZ);
    }
    if (pskb)
    {
        // if(skb_is_nonlinear(pskb)) {
        //     LOG_E("[%s]: alloc skb is no linear", __func__);
        // }
        tmpaddr = (SIZE_PTR) pskb->data;
        alignment = tmpaddr & (HIF_QUEUE_ALLOC_SKB_ALIGN_SZ - 1);
        skb_reserve(pskb, (HIF_QUEUE_ALLOC_SKB_ALIGN_SZ - alignment));
        skb_queue_tail(skb_head, pskb);
    }
    else
    {
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

void wf_hif_queue_alloc_skb(struct sk_buff_head *skb_head,wf_u8 hif_type)
{
    int i                   = 0;
    int ret                 = 0;
    for (i = 0; i < HIF_QUEUE_ALLOC_SKB_NUM; i++)
    {
        ret = wf_hif_queue_alloc_skb_one(skb_head,hif_type);
        if(WF_RETURN_FAIL == ret)
        {
            LOG_E("[%s] wf_hif_queue_alloc_skb failed.[%d]",__func__,i);
            break;
        }
    }
}

static void wf_hif_queue_free_skb(struct sk_buff_head *skb_head)
{
    struct sk_buff *skb = NULL;

    while ((skb = skb_dequeue(skb_head)) != NULL)
    {
        wf_free_skb(skb);
    }
}



int wf_tx_queue_empty(void *hif_info)
{
    hif_node_st *hif_node = (hif_node_st *)hif_info;
    data_queue_mngt_st *trxq     = (data_queue_mngt_st *)&hif_node->trx_pipe;
    wf_que_t      *free_tx_queue = &trxq->free_tx_queue;
    //wf_que_t      *tx_queue = &trxq->tx_queue;

    //LOG_I("[free_tx_queue] num:%d",free_tx_queue->num);

    if (wf_data_queue_full(free_tx_queue,WF_TX_MAX_DATA_QUEUE_NODE_NUM) == 1)
        return 1;
    //LOG_I("[free_tx_queue] cnt:%d, tx_queue cnt:%d",free_tx_queue->cnt,tx_queue->cnt);
    return 0;
}

int wf_tx_queue_insert(void *hif_info,wf_u8 agg_num,char *buff, wf_u32 buff_len, wf_u32 addr, int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param)
{
    int ret = 0;
    hif_node_st *hif_node = (hif_node_st *)hif_info;
    data_queue_node_st *qnode = NULL;
    data_queue_mngt_st *trxq     = (data_queue_mngt_st *)&hif_node->trx_pipe;
    wf_que_t      *free_tx_queue = &trxq->free_tx_queue;
    wf_que_t      *tx_queue = &trxq->tx_queue;
    struct xmit_buf *pxmitbuf   = NULL;
    int tmp_cnt   = 0;
    wf_timer_t timer;

    wf_lock_lock(&trxq->queu_txop_lock);
    /*if tx_queue is full, drop and return*/
    if(wf_data_queue_full(tx_queue, WF_TX_MAX_DATA_QUEUE_NODE_NUM))
    {
        LOG_I("[%s] tx_queue is full",__func__);
        wf_lock_unlock(&trxq->queu_txop_lock);
        return WF_RETURN_FAIL;
    }

    /*get node from free_tx_queue*/
    wf_timer_set(&timer, 100);
    do
    {
        tmp_cnt++;
        if(wf_timer_expired(&timer))
        {
            wf_timer_reset(&timer);
            LOG_W("[%s] timeout for get data_queue[%d].",__func__, tmp_cnt);
        }
        qnode = wf_data_queue_remove(free_tx_queue);
    }
    while(NULL == qnode);

    qnode->buff             = buff;
    qnode->real_size        = buff_len;
    qnode->addr             = addr;
    qnode->tx_info          = tx_info;
    qnode->param            = param;
    qnode->tx_callback_func = tx_callback_func;
    qnode->agg_num          = agg_num;
    qnode->state            = TX_STATE_IDL;
    qnode->pg_num           = (buff_len + 20 + 127) / 128;
    if(NULL != param)
    {
        pxmitbuf = param;
        qnode->encrypt_algo     = pxmitbuf->encrypt_algo;
        qnode->qsel             = pxmitbuf->qsel;
        qnode->ether_type       = pxmitbuf->ether_type;
        qnode->icmp_pkt         = pxmitbuf->icmp_pkt;
    }
#if 0
    if(0 < qnode->agg_num)
    {
        pxmitbuf = param;
        LOG_I("[%s,%d] node_id:%d, buffer_id:%d,agg_num:%d, pg_num:%d",__func__,__LINE__,qnode->node_id,pxmitbuf->buffer_id,qnode->agg_num, qnode->pg_num);
    }
#endif
    //LOG_I("[%s] addr:%x",__func__, addr);

    wf_data_queue_insert(tx_queue,qnode);
    qnode->state            = TX_STATE_INSERT;
    wf_lock_unlock(&trxq->queu_txop_lock);
    if(HIF_SDIO == hif_node->hif_type)
    {

#if HIF_QUEUE_TX_WORKQUEUE_USE
        trxq->tx_wq.ops->workqueue_work(&trxq->tx_wq);
#else
        wf_tasklet_hi_sched(&hif_node->trx_pipe.send_task);
#endif
    }
    else
    {
        //if(wf_que_count(tx_queue) <=1)
        {
            wf_tasklet_hi_sched(&hif_node->trx_pipe.send_task);
        }
    }
    return ret;
}

data_queue_node_st* wf_tx_queue_remove(data_queue_mngt_st *trxq)
{
    data_queue_node_st *qnode   = NULL;
    wf_que_t      *tx_queue     = &trxq->tx_queue;

    /*get node from free_tx_queue*/
    wf_lock_lock(&trxq->queu_txop_lock);
    qnode = wf_data_queue_remove(tx_queue);
    if(NULL != qnode)
    {
        if(TX_STATE_INSERT != qnode->state)
        {
            LOG_W("[%s], state[%d] != TX_STATE_INSERT",__func__,qnode->state);
        }
        qnode->state = TX_STATE_IN_PIP;
    }
    wf_lock_unlock(&trxq->queu_txop_lock);
    return qnode;
}
#ifdef CONFIG_SOFT_TX_AGGREGATION
/*check the packet that can't be agg*/
wf_bool pkt_is_agg_disable(data_queue_node_st *qnode)
{
    wf_bool ret = wf_false;
    if (0x0806 == qnode->ether_type)
    {
        LOG_I("[%s,%d] ether_type:ARP",__func__,__LINE__);
        ret = wf_true;
    }
    else if (0x86dd == qnode->ether_type)
    {
        LOG_I("[%s,%d] ether_type:IPv6",__func__,__LINE__);
        ret = wf_true;
    }
    else if (0x888e == qnode->ether_type)
    {
        LOG_I("[%s,%d] ether_type:802.1x",__func__,__LINE__);
        ret = wf_true;
    }
    else if (0x8864 == qnode->ether_type)
    {
        LOG_I("[%s,%d] ether_type:PPPoE",__func__,__LINE__);
        ret = wf_true;
    }
    else if(0x0800 == qnode->ether_type && qnode->icmp_pkt)
    {
        LOG_I("[%s,%d] ether_type:IP--ICMP",__func__,__LINE__);
        ret = wf_true;
    }

    return ret;
}

static void agg_update_send_core(hif_node_st *hif_info,data_queue_node_st *agg_qnode,wf_u8  agg_num,wf_u32 pkt_len)
{
    hif_sdio_st *sd = &hif_info->u.sdio;
    wf_u8 send_pg_num = WF_RND4(agg_qnode->pg_num);

    wf_tx_agg_num_fill(agg_num, sd->tx_agg_buffer);
    agg_qnode->buff         = sd->tx_agg_buffer;
    agg_qnode->real_size = pkt_len;
    agg_qnode->agg_num  = agg_num;
    sd->free_tx_page -= send_pg_num;
    sd->tx_state = TX_STATE_SENDING;
    hif_info->ops->hif_write(hif_info,WF_SDIO_TRX_QUEUE_FLAG,agg_qnode->addr,(char*)agg_qnode,agg_qnode->real_size);
    sd->tx_state = TX_STATE_COMPETE;
    wf_sdio_tx_flow_free_pg_ctl(hif_info,agg_qnode->hw_queue,send_pg_num);
    wf_sdio_tx_flow_agg_num_ctl(hif_info, agg_qnode->agg_num);
}
static void wf_tx_work_agg(struct work_struct *work)
{
    data_queue_mngt_st *dqm         = NULL;
    wf_que_t *data_queue            = NULL;
    data_queue_node_st *qnode       = NULL;
    hif_sdio_st *sd                 = NULL;
    data_queue_node_st agg_qnode;
    hif_node_st *hif_info           = NULL;
    wf_s32 pkt_len                  = 0;
    wf_s32 max_page_num             = 0;
    wf_s32 max_agg_num              = 0;
    wf_s32 agg_num                  = 0;
    wf_bool bret                    = 0;
    wf_bool agg_break               = wf_false;
    wf_u32 align_size               = 0;
    wf_workqueue_mgnt_st *wq_mgt    = NULL;
    wf_timer_t timer                = {0};
    wq_mgt = container_of(work, wf_workqueue_mgnt_st, work);
    dqm = container_of(wq_mgt, data_queue_mngt_st, tx_wq);
    if(NULL == dqm)
    {
        return;
    }

    hif_info = container_of(dqm, hif_node_st, trx_pipe);
    if(NULL == hif_info)
    {
        return;
    }

    data_queue = &dqm->tx_queue;
    if(NULL == data_queue)
    {
        return;
    }
    sd = &hif_info->u.sdio;
    if(sd->count_start)
    {
        wf_timer_set(&timer,0);
        sd->count++;
    }
    wf_memset(&agg_qnode, 0, sizeof(agg_qnode));
    while(NULL != (qnode = wf_tx_queue_remove(dqm)))
    {
        if(sd->count_start)
        {
            sd->tx_pkt_num++;
        }
        sd->tx_state=qnode->state = TX_STATE_FLOW_CTL;
#ifdef CONFIG_RICHV200
        if(WF_PKT_TYPE_FRAME != (qnode->buff[0] & 0x03) ) //not need flow control
        {

            sd->tx_state=qnode->state = TX_STATE_SENDING;
            hif_info->ops->hif_write(hif_info,WF_SDIO_TRX_QUEUE_FLAG,qnode->addr,(char*)qnode,qnode->real_size);

        }
        else
#endif
        {

            if(wf_true == pkt_is_agg_disable(qnode))
            {
                agg_break = wf_true;
            }
            else
            {
                agg_break = wf_false;
            }
            //update max_page_num, max_agg_num
            if(0 == agg_num || agg_qnode.qsel != qnode->qsel)
            {
                wf_sdio_update_txbuf_size(hif_info,qnode,&max_page_num,&max_agg_num);
            }

            align_size = TXDESC_SIZE + WF_RND_MAX((qnode->real_size-TXDESC_SIZE), 8);
            if( (0 != agg_num && WF_RND4(agg_qnode.pg_num+qnode->pg_num) > max_page_num) ||
                (0 != agg_num &&  agg_qnode.qsel != qnode->qsel)                ||
                (0 != agg_num && agg_num > max_agg_num )                        ||
                (0 != agg_num && wf_true == agg_break)
              )
            {
                if(sd->count_start)
                {
                    sd->tx_agg_num++;
                }
                //if(0)
                //{
                //    LOG_I("agg_num:%d,pg_num:%d,qsel:%d,hw_queu:%d,max_page_num:%d,max_agg_num:%d,free_node_num:%d",
                //        agg_num,agg_qnode.pg_num,agg_qnode.qsel,agg_qnode.hw_queue,max_page_num,max_agg_num,wf_que_count(&dqm->free_tx_queue));
                //}
                agg_update_send_core(hif_info,&agg_qnode,agg_num,pkt_len);
                agg_num = 0;
                pkt_len = 0;

                //for current qnode
                wf_sdio_update_txbuf_size(hif_info,qnode,&max_page_num,&max_agg_num);

                wf_memcpy(&agg_qnode,qnode,sizeof(agg_qnode));

                agg_num++;
                wf_memcpy(sd->tx_agg_buffer+pkt_len,qnode->buff,qnode->real_size);
                pkt_len += align_size ;

            }
            else
            {

                agg_num++;
                if(1 == agg_num)
                {
                    wf_memcpy(&agg_qnode,qnode,sizeof(agg_qnode));
                }
                else
                {
                    agg_qnode.pg_num    += qnode->pg_num;
                }
                //agg_qnode.pg_num = size_to_pg(agg_qnode.real_size);
                wf_memcpy(sd->tx_agg_buffer+pkt_len,qnode->buff,qnode->real_size);
                pkt_len += align_size ;

            }


        }


        if(qnode->tx_callback_func)
        {
            bret = qnode->tx_callback_func(qnode->tx_info, qnode->param);
            if(wf_true == bret)
            {

            }
            else
            {
                LOG_W("[%s,%d] tx_callback_func failed",__func__,__LINE__);
            }
        }

        qnode->state = TX_STATE_COMPETE;
        wf_data_queue_insert(&hif_info->trx_pipe.free_tx_queue, qnode);
    }

    if(pkt_len>0 && agg_num>0)
    {
        if(sd->count_start)
        {
            sd->tx_agg_num++;
        }
        //if( wf_true == agg_break)
        //{
        //    LOG_I("%s agg_num:%d,pg_num:%d,qsel:%d,hw_queu:%d, max_page_num:%d,max_agg_num:%d,free_tx_node_num:%d",
        //            (wf_true == agg_break) ?"agg_break":"agg_enable",
        //            agg_num,agg_qnode.pg_num,agg_qnode.qsel,agg_qnode.hw_queue,max_page_num,max_agg_num,wf_que_count(&dqm->free_tx_queue));
        //}
        agg_update_send_core(hif_info,&agg_qnode,agg_num,pkt_len);
    }

    if(sd->count_start)
    {
        sd->tx_all_time += wf_timer_elapsed(&timer);
    }
}
#else
static void wf_tx_work(struct work_struct *work)
{
    data_queue_mngt_st *dqm     = NULL;
    wf_que_t *data_queue        = NULL;
    data_queue_node_st *qnode   = NULL;
    hif_node_st *hif_info       = NULL;

    wf_workqueue_mgnt_st *wq_mgt = NULL;
    wq_mgt = container_of(work, wf_workqueue_mgnt_st, work);
    dqm = container_of(wq_mgt, data_queue_mngt_st, tx_wq);

    if(NULL == dqm)
    {
        return;
    }
    data_queue = &dqm->tx_queue;
    if(NULL == data_queue)
    {
        return;
    }

    while(NULL != (qnode = wf_tx_queue_remove(dqm)))
    {

        hif_info = qnode->hif_node;
#if 0
        if(0 < qnode->agg_num)
        {
            pxmitbuf = qnode->param;
            LOG_I("[%s,%d] node_id:%d, buffer_id:%d,agg_num:%d, pg_num:%d",__func__,__LINE__,qnode->node_id,pxmitbuf->buffer_id,qnode->agg_num, pxmitbuf->pg_num);
        }
#endif

        if(HIF_USB == hif_info->hif_type)
        {
            hif_info->ops->hif_write(hif_info,WF_USB_NET_PIP,qnode->addr,(char*)qnode,qnode->real_size);
        }
        else
        {
            //LOG_I("[%s] addr:%x",__func__, qnode->addr);
            hif_info->ops->hif_write(hif_info,WF_SDIO_TRX_QUEUE_FLAG,qnode->addr,(char*)qnode,qnode->real_size);
        }
    }
}

#endif

int hif_frame_dispath(hif_node_st *hif_info,struct sk_buff *pskb )
{
    int num = 0;
    struct net_device *ndev = NULL;
    wf_u8 *pdata = NULL;
#ifdef CONFIG_RICHV200
    struct rxd_detail_new *prxd = NULL;
#else
    struct rxd_detail_org *prxd = NULL;
#endif
    wf_u8 bc_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#ifdef CONFIG_RICHV200
    prxd = (struct rxd_detail_new *)(pskb->data);
#else
    prxd = (struct rxd_detail_org *)(pskb->data);
#endif


    pdata = (pskb->data) + RXDESC_SIZE + 32;

//  LOG_D("[hif_frame_dispath]:mac1:"WF_MAC_FMT,WF_MAC_ARG(get_ra(pdata)));
//  LOG_D("[hif_frame_dispath]:ta:"WF_MAC_FMT,WF_MAC_ARG(get_ta(pdata)));
    while(num < hif_info->nic_number)
    {
        if(NULL == hif_info->nic_info[num])
        {
            LOG_E("%s  nic_info[%d] NULL",__func__,num);
            return WF_RETURN_FAIL;
        }

        ndev = hif_info->nic_info[num]->ndev;

        if(ndev == NULL)
        {
            LOG_E("%s  ndev[%d] NULL",__func__,num);
            return WF_RETURN_FAIL;
        }

        if(wf_memcmp(ndev->dev_addr,get_ra(pdata),ETH_ALEN) == 0)
        {
            //LOG_D("nic[%d]: "WF_MAC_FMT,num,WF_MAC_ARG(nic_to_local_addr(hif_info->nic_info[num]) ));
            rx_work(hif_info->nic_info[num]->ndev, pskb);
            return 0;

        }
        else if(wf_memcmp(bc_addr,get_ra(pdata),ETH_ALEN) == 0)
        {
            rx_work(hif_info->nic_info[num]->ndev, pskb);
        }
        num++;
    }

    return 0;
}

int hif_tasklet_rx_handle(hif_node_st *hif_info)
{
    struct sk_buff *pskb  = NULL;
    data_queue_node_st *qnode       = NULL;
    int ret = 0;

    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return 0;
    }

    while(1)
    {
        pskb = skb_dequeue(&hif_info->trx_pipe.rx_queue);
        if (NULL == pskb)
        {
            break;
        }

        if (hm_get_mod_removed() == wf_true || hif_info->dev_removed == wf_true)
        {
            skb_queue_tail(&hif_info->trx_pipe.free_rx_queue_skb, pskb);
            continue;
        }

        ret = hif_frame_dispath(hif_info,pskb);
        if(ret)
        {
            //LOG_W("[%s] failed ret:%d",__func__,ret);
        }

        skb_reset_tail_pointer(pskb);
        pskb->len = 0;
        skb_queue_tail(&hif_info->trx_pipe.free_rx_queue_skb, pskb);

        if(HIF_USB == hif_info->hif_type)
        {
            /* check urb node, if have free, used it */
            if(NULL !=(qnode = wf_data_queue_remove(&hif_info->trx_pipe.free_rx_queue)))
            {
                hif_info->ops->hif_read(hif_info, WF_USB_NET_PIP, READ_QUEUE_INX, (wf_u8*)qnode,WF_MAX_RECV_BUFF_LEN_USB);
            }
        }
    }

    return 0;
}



int hif_tasklet_tx_handle(hif_node_st *hif_info)
{
    data_queue_mngt_st *dqm     = &hif_info->trx_pipe;
    data_queue_node_st *qnode   = NULL;

    while(NULL != (qnode = wf_tx_queue_remove(dqm)))
    {
        if (hm_get_mod_removed() == wf_true || hif_info->dev_removed == wf_true)
            return 0;

        hif_info = qnode->hif_node;
        if(HIF_USB == hif_info->hif_type)
        {
            hif_info->ops->hif_write(hif_info,WF_USB_NET_PIP,qnode->addr,(char*)qnode,qnode->real_size);
        }
        else
        {
            hif_info->ops->hif_write(hif_info,WF_SDIO_TRX_QUEUE_FLAG,qnode->addr,(char*)qnode,qnode->real_size);
        }
    }
    return 0;
}

int wf_hif_queue_enable(hif_node_st *hif_node)
{
    data_queue_mngt_st *hqueue  = NULL;
    data_queue_node_st  *qnode  = NULL;

    LOG_D("[%s] begin",__func__);

    hqueue  = &hif_node->trx_pipe;

    /*rx queue*/
    if(HIF_USB == hif_node->hif_type )
    {
        while(NULL !=(qnode = wf_data_queue_remove(&hif_node->trx_pipe.free_rx_queue)))
        {
            hif_node->ops->hif_read(hif_node, WF_USB_NET_PIP, READ_QUEUE_INX, (wf_u8*)qnode, WF_MAX_RECV_BUFF_LEN_USB);
        }

        hif_node->hif_tr_ctrl = wf_true;
    }
    LOG_D("[%s] end",__func__);
    return WF_RETURN_OK;
}


wf_s32 wf_hif_queue_disable(hif_node_st *hif_node)
{
    struct sk_buff *pskb    = NULL;

    hif_node->hif_tr_ctrl = wf_false;

    /*clean rx queue*/
    while(NULL != (pskb = skb_dequeue(&hif_node->trx_pipe.rx_queue)))
    {
        wf_free_skb(pskb);
        pskb = NULL;
    }

    return WF_RETURN_OK;
}

int wf_data_queue_mngt_init(void *hif_node)
{
    int i                                           = 0;
    data_queue_node_st *recv_node                   = NULL;
    data_queue_node_st *send_node                   = NULL;
    hif_node_st *hif_info                           = (hif_node_st *)hif_node;
    data_queue_mngt_st *data_queue_mngt             = &hif_info->trx_pipe;
#ifdef CONFIG_SOFT_TX_AGGREGATION
    static wf_workqueue_func_param_st wq_tx_param   = {"wf_tx_workqueue",wf_tx_work_agg};
#else
    static wf_workqueue_func_param_st wq_tx_param   = {"wf_tx_workqueue",wf_tx_work};
#endif

    skb_queue_head_init(&data_queue_mngt->rx_queue);
    skb_queue_head_init(&data_queue_mngt->free_rx_queue_skb);
    wf_que_init(&data_queue_mngt->free_rx_queue,WF_LOCK_TYPE_IRQ);


    wf_que_init(&data_queue_mngt->free_tx_queue, WF_LOCK_TYPE_IRQ);
    wf_que_init(&data_queue_mngt->tx_queue, WF_LOCK_TYPE_IRQ);
    wf_lock_init(&data_queue_mngt->queu_txop_lock,WF_LOCK_TYPE_NONE);

    data_queue_mngt->alloc_cnt=0;

#ifdef CONFIG_SOFT_TX_AGGREGATION
    if(HIF_SDIO == hif_info->hif_type)
    {
        hif_info->u.sdio.tx_agg_buffer = wf_kzalloc(TX_AGG_BUFF_SIZE);
        if(NULL == hif_info->u.sdio.tx_agg_buffer)
        {
            LOG_E("[%s] tx_agg_buffer failed",__func__);
            return 0;
        }
        hif_info->u.sdio.free_tx_page = 0;
    }
#endif

    wf_tasklet_init(&data_queue_mngt->recv_task, (void (*)(unsigned long))hif_tasklet_rx_handle,(unsigned long)hif_node);



    /*tx queue init*/
    wf_os_api_workqueue_register(&data_queue_mngt->tx_wq,&wq_tx_param);
    wf_tasklet_init(&data_queue_mngt->send_task, (void (*)(unsigned long))hif_tasklet_tx_handle,(unsigned long)hif_node);


    data_queue_mngt->alloc_cnt++;
    wf_hif_queue_alloc_skb(&data_queue_mngt->free_rx_queue_skb,hif_info->hif_type);

    /*rx queue init */
    hif_info->trx_pipe.all_rx_queue = wf_queue_node_malloc(WF_RX_MAX_DATA_QUEUE_NODE_NUM);
    for(i=0; i<WF_RX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {
        recv_node = hif_info->trx_pipe.all_rx_queue + i;

        //LOG_I("[%s] [%d] recv_node:%p",__func__,i,recv_node);
        recv_node->hif_node = hif_node;
        recv_node->real_size = 0;
        recv_node->node_id = i;
        if(HIF_USB == hif_info->hif_type)
        {
            recv_node->u.purb       = usb_alloc_urb(0,GFP_KERNEL);
            if(NULL == recv_node->u.purb)
            {
                LOG_E("[%s] usb_alloc_urb failed",__func__);
                wf_free_skb( (struct sk_buff*)recv_node->buff);
                wf_kfree(recv_node);
                continue;
            }
        }
        else if(HIF_SDIO == hif_info->hif_type)
        {
            recv_node->u.sd_func = hif_info->u.sdio.func;
        }

        wf_enque_tail(&recv_node->node, &data_queue_mngt->free_rx_queue);
        recv_node = NULL;
    }

    hif_info->trx_pipe.all_tx_queue = wf_queue_node_malloc(WF_TX_MAX_DATA_QUEUE_NODE_NUM);
    for(i=0; i<WF_TX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {

        send_node = hif_info->trx_pipe.all_tx_queue + i;
        //LOG_I("[%s] [%d] send_node:%p",__func__,i,send_node);
        send_node->hif_node = hif_info;
        send_node->real_size    = 0;
        send_node->node_id      = i;
        if(HIF_USB == hif_info->hif_type)
        {
            send_node->u.purb       = usb_alloc_urb(0,GFP_KERNEL);
            if(NULL == send_node->u.purb)
            {
                LOG_E("[%s] usb_alloc_urb failed",__func__);
                wf_free_skb( (struct sk_buff*)send_node->buff);
                wf_kfree(send_node);
                continue;
            }
        }
        else if(HIF_SDIO == hif_info->hif_type)
        {
            send_node->u.sd_func = hif_info->u.sdio.func;
        }

        wf_enque_tail(&send_node->node, &data_queue_mngt->free_tx_queue);
        send_node = NULL;
    }


    data_queue_mngt->is_init = wf_true;

    return WF_RETURN_OK;
}


int wf_data_queue_mngt_term(void *hif_node)
{
    wf_list_t *pos = NULL;
    wf_list_t *next = NULL;
    data_queue_node_st *data_node = NULL;
    data_queue_node_st *tx_node = NULL;
    hif_node_st  *hif_info      = (hif_node_st  *)hif_node;
    data_queue_mngt_st *trxq    = &hif_info->trx_pipe;

    if (trxq)
    {
        tasklet_kill(&trxq->recv_task);
    }
#if HIF_QUEUE_TX_WORKQUEUE_USE
    if(trxq->tx_wq.ops)
    {
        if(trxq->tx_wq.ops->workqueue_term)
        {
            trxq->tx_wq.ops->workqueue_term(&trxq->tx_wq);
        }
    }
    else
    {
        return WF_RETURN_OK;
    }
#else
    if (trxq)
    {
        tasklet_kill(&trxq->send_task);
    }
#endif

    if(HIF_SDIO == hif_info->hif_type)
    {
        if(hif_info->u.sdio.tx_agg_buffer)
        {
            wf_kfree(hif_info->u.sdio.tx_agg_buffer);
            hif_info->u.sdio.tx_agg_buffer = NULL;
        }

    }
    //LOG_I("[%s,%d]",__func__,__LINE__);
    /*rx queue freee */
    // wf_lock_lock(&trxq->free_rx_queue.lock); // no need lock, lock will cause usb_kill_urb run bug, by renhaibo
    wf_list_for_each_safe(pos,next,&trxq->free_rx_queue.head)
    {
        data_node = wf_list_entry(pos, data_queue_node_st, node);
        if(data_node)
        {
            hif_info  = (hif_node_st  *)data_node->hif_node;
            if(HIF_USB == hif_info->hif_type)
            {
                usb_kill_urb(data_node->u.purb);
                usb_free_urb(data_node->u.purb);
            }

            wf_list_delete(&data_node->node);
            data_node = NULL;
            trxq->free_rx_queue.cnt--;

        }

    }
    //wf_lock_unlock(&trxq->free_rx_queue.lock);
    wf_que_deinit(&trxq->free_rx_queue);

    if (trxq->all_rx_queue)
    {
        wf_kfree(trxq->all_rx_queue);
        trxq->all_rx_queue = NULL;
    }

    //LOG_I("[%s,%d]",__func__,__LINE__);
    /*tx queue free*/
    //wf_lock_lock(&trxq->free_tx_queue.lock);   // no need lock, lock will cause usb_kill_urb run bug, by renhaibo
    wf_list_for_each_safe(pos,next,&trxq->free_tx_queue.head)
    {
        tx_node = wf_list_entry(pos, data_queue_node_st, node);
        if(tx_node)
        {
            hif_info  = (hif_node_st  *)tx_node->hif_node;
            if(HIF_USB == hif_info->hif_type)
            {
                usb_kill_urb(tx_node->u.purb);
                usb_free_urb(tx_node->u.purb);
            }
            wf_list_delete(&tx_node->node);
            tx_node = NULL;
            trxq->free_tx_queue.cnt--;
        }
    }
    //wf_lock_unlock(&trxq->free_tx_queue.lock);
    wf_que_deinit(&trxq->free_tx_queue);
    wf_que_deinit(&trxq->tx_queue);
    wf_lock_term(&trxq->queu_txop_lock);
    if (trxq->all_tx_queue)
    {
        wf_kfree(trxq->all_tx_queue);
        trxq->all_tx_queue = NULL;
    }

    wf_hif_queue_free_skb(&hif_info->trx_pipe.free_rx_queue_skb);

    return WF_RETURN_OK;
}

