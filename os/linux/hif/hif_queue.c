#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/usb.h>

#include "wf_os_api.h"
#include "common.h"
#include "wf_que.h"
#include "hif_queue.h"
#include "hif.h"
#include "rx_linux.h"
#include "wf_debug.h"

static void wf_tx_work(struct work_struct *work);
static void wf_hif_queue_rx_work(struct work_struct *work);

wf_u32 wf_data_queue_len(wf_que_t *queue)
{
    wf_u32 len = 0;

    wf_lock_lock(&queue->lock);
    len = queue->cnt;
    wf_lock_unlock(&queue->lock);;

    return len;
}

static int wf_data_queue_full(wf_que_t *queue,int queue_node_num)
{
    return queue_node_num == queue->cnt;
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
    wf_list_t    *head       = NULL;

    if(wf_que_is_empty(queue))
    {
        //LOG_I("[%s] queue [%d] is empty",__func__,queue->qtype);
        return NULL;
    }

    head    = wf_deque_head(queue);
    rb_node = wf_list_entry(head, data_queue_node_st, node);

    return rb_node;
}


data_queue_node_st * wf_queue_node_malloc(void *hif_node,int cnt )
{
    data_queue_node_st *node = NULL;

    node = wf_kzalloc(cnt * sizeof(data_queue_node_st));
    if(NULL == node)
    {
        LOG_E("[%s] kzalloc failed ,check!!!!", __func__);
        return NULL;
    }

    return node;
}

#if HIF_QUEUE_PRE_ALLOC_DEBUG

int wf_hif_queue_alloc_skb_one(struct sk_buff_head *skb_head,wf_u8 hif_type)
{
    int i                   = 0;
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
    struct sk_buff *pskb    = NULL;
    SIZE_PTR tmpaddr        = 0;
    SIZE_PTR alignment      = 0;
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

#endif

int wf_data_queue_mngt_init(void *hif_node)
{
    int i = 0;
    data_queue_node_st *recv_node = NULL;
    data_queue_node_st *send_node = NULL;
    hif_node_st *hif_info = (hif_node_st *)hif_node;
    data_queue_mngt_st *data_queue_mngt = &hif_info->trx_pipe;
    static wf_workqueue_func_param_st wq_rx_param={"wf_rx_workqueue",wf_hif_queue_rx_work};
    static wf_workqueue_func_param_st wq_tx_param={"wf_tx_workqueue",wf_tx_work};
    skb_queue_head_init(&data_queue_mngt->rx_queue);
    skb_queue_head_init(&data_queue_mngt->free_rx_queue_skb);
    wf_que_init(&data_queue_mngt->free_rx_queue,WF_LOCK_TYPE_IRQ);
    wf_que_init(&data_queue_mngt->free_tx_queue, WF_LOCK_TYPE_IRQ);
    wf_que_init(&data_queue_mngt->tx_queue, WF_LOCK_TYPE_IRQ);
    data_queue_mngt->alloc_cnt=0;
#if HIF_QUEUE_RX_WORKQUEUE_USE
    /*rx queue init*/
    wf_os_api_workqueue_register(&data_queue_mngt->rx_wq,&wq_rx_param);
#endif

    wf_tasklet_init(&data_queue_mngt->recv_task, (void (*)(unsigned long))hif_tasklet_rx_handle,(unsigned long)hif_node);



    /*tx queue init*/
    wf_os_api_workqueue_register(&data_queue_mngt->tx_wq,&wq_tx_param);
    wf_tasklet_init(&data_queue_mngt->send_task, (void (*)(unsigned long))hif_tasklet_tx_handle,(unsigned long)hif_node);


#if HIF_QUEUE_PRE_ALLOC_DEBUG
    data_queue_mngt->alloc_cnt++;
    wf_hif_queue_alloc_skb(&data_queue_mngt->free_rx_queue_skb,hif_info->hif_type);
#endif

    /*rx queue init */
    hif_info->trx_pipe.all_rx_queue = wf_queue_node_malloc(hif_node,WF_RX_MAX_DATA_QUEUE_NODE_NUM);
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



    hif_info->trx_pipe.all_tx_queue = wf_queue_node_malloc(hif_node,WF_TX_MAX_DATA_QUEUE_NODE_NUM);
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

#if HIF_QUEUE_RX_WORKQUEUE_USE
    if (trxq->rx_wq.ops->workqueue_term)
    {
        trxq->rx_wq.ops->workqueue_term(&trxq->rx_wq);
    }
#else

    if (trxq)
    {
        tasklet_kill(&trxq->recv_task);
    }
#endif
#if HIF_QUEUE_TX_WORKQUEUE_USE
    if(trxq->tx_wq.ops->workqueue_term)
    {
        trxq->tx_wq.ops->workqueue_term(&trxq->tx_wq);
    }
#else
    if (trxq)
    {
        tasklet_kill(&trxq->send_task);
    }
#endif

    //LOG_I("[%s,%d]",__func__,__LINE__);
    /*rx queue freee */
    wf_lock_lock(&trxq->free_rx_queue.lock);
    wf_list_for_each_safe(pos,next,&trxq->free_rx_queue.head)
    {
        data_node = wf_list_entry(pos, data_queue_node_st, node);
        if(data_node)
        {
            hif_info  = (hif_node_st  *)data_node->hif_node;
            if(HIF_USB == hif_info->hif_type)
            {
                usb_free_urb(data_node->u.purb);
            }

            wf_list_delete(&data_node->node);
            data_node = NULL;
            trxq->free_rx_queue.cnt--;

        }

    }
    wf_lock_unlock(&trxq->free_rx_queue.lock);

    if (trxq->all_rx_queue)
    {
        kfree(trxq->all_rx_queue);
        trxq->all_rx_queue = NULL;
    }

    //LOG_I("[%s,%d]",__func__,__LINE__);
    /*tx queue free*/
    wf_lock_lock(&trxq->free_tx_queue.lock);
    wf_list_for_each_safe(pos,next,&trxq->free_tx_queue.head)
    {
        tx_node = wf_list_entry(pos, data_queue_node_st, node);
        if(tx_node)
        {
            hif_info  = (hif_node_st  *)tx_node->hif_node;
            if(HIF_USB == hif_info->hif_type)
            {
                usb_free_urb(tx_node->u.purb);   
            }
            wf_list_delete(&tx_node->node);
            tx_node = NULL;
            trxq->free_tx_queue.cnt--;
        }
    }
    wf_lock_unlock(&trxq->free_tx_queue.lock);

    if (trxq->all_tx_queue)
    {
        kfree(trxq->all_tx_queue);
        trxq->all_tx_queue = NULL;
    }

#if HIF_QUEUE_PRE_ALLOC_DEBUG
    wf_hif_queue_free_skb(&hif_info->trx_pipe.free_rx_queue_skb);
#endif

    return WF_RETURN_OK;
}

int wf_tx_queue_empty(void *hif_info)
{
    hif_node_st *hif_node = (hif_node_st *)hif_info;
    data_queue_node_st *qnode = NULL;
    data_queue_mngt_st *trxq     = (data_queue_mngt_st *)&hif_node->trx_pipe;
    wf_que_t      *free_tx_queue = &trxq->free_tx_queue;
    wf_que_t      *tx_queue = &trxq->tx_queue;

    //LOG_I("[free_tx_queue] num:%d",free_tx_queue->num);

    if (wf_data_queue_full(free_tx_queue,WF_TX_MAX_DATA_QUEUE_NODE_NUM) == 1)
        return 1;

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

#if 0
    /*if tx_queue is full, wait*/
    while(wf_data_queue_full(tx_queue, WF_MAX_DATA_QUEUE_NODE_NUM))
    {
        LOG_I("[%s] tx_queue is full");
    }
#else
    /*if tx_queue is full, drop and return*/
    if(wf_data_queue_full(tx_queue, WF_TX_MAX_DATA_QUEUE_NODE_NUM))
    {
        LOG_I("[%s] tx_queue is full",__func__);
        return WF_RETURN_FAIL;
    }
#endif

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
    if(NULL != param)
    {
        pxmitbuf = param;
#if 0
        qnode->pg_num = (pxmitbuf->pkt_len+127)/128;
#else
        qnode->pg_num = pxmitbuf->pg_num;
#endif
    }
#if 0
    if(0 < qnode->agg_num)
    {
        pxmitbuf = param;
        LOG_I("[%s,%d] node_id:%d, buffer_id:%d,agg_num:%d, pg_num:%d",__func__,__LINE__,qnode->node_id,pxmitbuf->buffer_id,qnode->agg_num, pxmitbuf->pg_num);
    }
#endif
    //LOG_I("[%s] addr:%x",__func__, addr);

    wf_data_queue_insert(tx_queue,qnode);
    qnode->state            = TX_STATE_INSERT;
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
        if(wf_data_queue_len(tx_queue) <=1)
        {
            wf_tasklet_hi_sched(&hif_node->trx_pipe.send_task);
        }
    }
    return ret;
}

int wf_tx_queue_remove(data_queue_mngt_st *trxq)
{
    data_queue_node_st *qnode = NULL;
    wf_que_t      *free_tx_queue = &trxq->free_tx_queue;
    wf_que_t      *tx_queue = &trxq->tx_queue;
    /*get node from free_tx_queue*/
    while(NULL != (qnode = wf_data_queue_remove(tx_queue)))
    {
        wf_data_queue_insert(free_tx_queue,qnode);
    }

    return WF_RETURN_OK;
}

static void wf_tx_work(struct work_struct *work)
{
    data_queue_mngt_st *dqm     = NULL;
    wf_que_t *data_queue   = NULL;
    struct xmit_buf *pxmitbuf   = NULL;
    data_queue_node_st *qnode   = NULL;
    hif_node_st *hif_info       = NULL;
    int ret                     = 0;

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

    while(NULL != (qnode = wf_data_queue_remove(data_queue)))
    {

        hif_info = qnode->hif_node;
#if 0
        if(0 < qnode->agg_num)
        {
            pxmitbuf = qnode->param;
            LOG_I("[%s,%d] node_id:%d, buffer_id:%d,agg_num:%d, pg_num:%d",__func__,__LINE__,qnode->node_id,pxmitbuf->buffer_id,qnode->agg_num, pxmitbuf->pg_num);
        }
#endif
        if(TX_STATE_INSERT != qnode->state)
        {
            LOG_E("[%s], state[%d] != TX_STATE_INSERT",__func__,qnode->state);
        }
        qnode->state = TX_STATE_IN_PIP;
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


#if 1
static void wf_hif_queue_rx_work(struct work_struct *work)
{
    data_queue_mngt_st *dqm     = NULL;
    data_queue_node_st *qnode   = NULL;
    hif_node_st *hif_info       = NULL;
    struct sk_buff *pskb        = NULL;
    wf_workqueue_mgnt_st *wq_mgt = NULL;

    wq_mgt = container_of(work, wf_workqueue_mgnt_st, work);
    dqm = container_of(wq_mgt, data_queue_mngt_st, rx_wq);

    hif_info = container_of(dqm,hif_node_st,trx_pipe);
    if(NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null",__func__);
        return;
    }

    while(1)
    {
        pskb = skb_dequeue(&hif_info->trx_pipe.rx_queue);
        if (NULL == pskb)
        {
            break;
        }

#ifdef CONFIG_CONCURRENT_MODE
        hif_frame_nic_check(hif_info,pskb);
#else
        if (hif_info->nic_info[0])
        {
            if (hif_info->nic_info[0]->ndev)
            {
                rx_work(hif_info->nic_info[0]->ndev, pskb);
            }
        }
#endif

#if HIF_QUEUE_PRE_ALLOC_DEBUG
        skb_reset_tail_pointer(pskb);
        pskb->len = 0;
        skb_queue_tail(&hif_info->trx_pipe.free_rx_queue_skb, pskb);
#else
        wf_free_skb(pskb);
        pskb = NULL;
#endif

        if(HIF_USB == hif_info->hif_type)
        {
            /* check urb node, if have free, used it */
            if(NULL !=(qnode = wf_data_queue_remove(&hif_info->trx_pipe.free_rx_queue)))
            {
                hif_info->ops->hif_read(hif_info,WF_USB_NET_PIP,READ_QUEUE_INX, (wf_u8*)qnode,WF_MAX_RECV_BUFF_LEN_USB);
            }
        }
    }
}
#endif


