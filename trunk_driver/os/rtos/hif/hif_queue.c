
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"
#include "hif/hif.h"
#include "trx/rx_rtos.h"


#define TX_AGG_BUFF_SIZE (32*1024)

static int wf_data_queue_full(wf_que_t *queue, int queue_node_num)
{
    return (queue_node_num == wf_que_count(queue));
}

int wf_data_queue_insert(wf_que_t *queue, data_queue_node_st *qnode)
{
    //LOG_I("[%s] num:%d", __func__, queue->num);
    wf_enque_tail(&qnode->node, queue);
    return queue->cnt;
}

data_queue_node_st *wf_data_queue_remove(wf_que_t *queue)
{
    wf_list_t *head = NULL;

    head = wf_deque_head(queue);
    if (NULL != head)
    {
        return wf_list_entry(head, data_queue_node_st, node);
    }

    return NULL;
}

data_queue_node_st *wf_queue_node_malloc(int cnt)
{
    data_queue_node_st *node = NULL;

    node = wf_kzalloc(cnt * sizeof(data_queue_node_st));
    if (NULL == node)
    {
        LOG_E("[%s] wf_kzalloc failed, check!!!!", __func__);
        return NULL;
    }

    return node;
}

static hif_netbuf_st *wf_netbuf_node_malloc(int cnt)
{
    hif_netbuf_st *node = NULL;

    node = wf_kzalloc(cnt * sizeof(hif_netbuf_st));
    if (NULL == node)
    {
        LOG_E("[%s] wf_kzalloc failed , check!!!!", __func__);
        return NULL;
    }

    return node;
}

hif_netbuf_st *wf_netbuf_queue_remove(wf_que_t *queue)
{
    wf_list_t *head = NULL;

    head = wf_deque_head(queue);
    if (NULL != head)
    {
        return wf_list_entry(head, hif_netbuf_st, node);
    }

    return NULL;
}

int wf_tx_queue_empty(void *hif_info)
{
#ifdef CONFIG_SOFT_TX_AGGREGATION
    hif_node_st *hif_node = (hif_node_st *)hif_info;
    data_queue_mngt_st *trxq     = (data_queue_mngt_st *)&hif_node->trx_pipe;
    wf_que_t      *free_tx_queue = &trxq->free_tx_queue;
    //wf_que_t      *tx_queue = &trxq->tx_queue;

    //LOG_I("[free_tx_queue] num:%d", free_tx_queue->num);

    if (wf_data_queue_full(free_tx_queue, WF_TX_MAX_DATA_QUEUE_NODE_NUM) == 1)
        return 1;
    //LOG_I("[free_tx_queue] cnt:%d, tx_queue cnt:%d", free_tx_queue->cnt, tx_queue->cnt);
    return 0;
#else
    return 1;
#endif
}

int wf_tx_queue_insert(void *hif_info, wf_u8 agg_num, char *buff, wf_u32 buff_len, wf_u32 addr, int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param)
{
#ifdef CONFIG_SOFT_TX_AGGREGATION
    int ret = 0;
    hif_node_st *hif_node = (hif_node_st *)hif_info;
    data_queue_node_st *qnode = NULL;
    data_queue_mngt_st *trxq = (data_queue_mngt_st *)&hif_node->trx_pipe;
    wf_que_t *free_tx_queue = &trxq->free_tx_queue;
    wf_que_t *tx_queue = &trxq->tx_queue;
    struct xmit_buf *pxmitbuf = NULL;
    int tmp_cnt = 0;

    if (HIF_SDIO == hif_node->hif_type)
    {
#   ifdef CONFIG_SDIO_FLAG
        wf_lock_lock(&trxq->queu_txop_lock);
        /*if tx_queue is full, drop and return*/
        if (wf_data_queue_full(tx_queue, WF_TX_MAX_DATA_QUEUE_NODE_NUM))
        {
            LOG_I("[%s] tx_queue is full", __func__);
            wf_lock_unlock(&trxq->queu_txop_lock);
            return WF_RETURN_FAIL;
        }

        /*get node from free_tx_queue*/
        do
        {
            qnode = wf_data_queue_remove(free_tx_queue);
            if (NULL != qnode)
            {
                break;
            }
            wf_msleep(20);
        }
        while (1);

        qnode->buff             = (wf_u8 *)buff;
        qnode->real_size        = buff_len;
        qnode->addr             = addr;
        qnode->tx_info          = tx_info;
        qnode->param            = param;
        qnode->tx_callback_func = tx_callback_func;
        qnode->agg_num          = agg_num;
        qnode->state            = TX_STATE_IDL;
        qnode->pg_num           = (buff_len + 20 + 127) / 128;
        if (NULL != param)
        {
            pxmitbuf = param;
            qnode->encrypt_algo     = pxmitbuf->encrypt_algo;
            qnode->qsel             = pxmitbuf->qsel;
            qnode->ether_type       = pxmitbuf->ether_type;
            qnode->icmp_pkt         = pxmitbuf->icmp_pkt;
        }

//        if (0 < qnode->agg_num)
//        {
//            pxmitbuf = param;
//            LOG_I("[%s, %d] node_id:%d, buffer_id:%d, agg_num:%d, pg_num:%d", __func__, __LINE__, qnode->node_id, pxmitbuf->buffer_id, qnode->agg_num, pxmitbuf->pg_num);
//        }
//
//        LOG_I("[%s] addr:%x", __func__, addr);

        wf_data_queue_insert(tx_queue, qnode);
        qnode->state            = TX_STATE_INSERT;
        wf_lock_unlock(&trxq->queu_txop_lock);

        trxq->hif_tx_wq.ops->workqueue_work(&trxq->hif_tx_wq);
#   endif
    }
    else     // hif NIC_USB
    {
#   ifdef CONFIG_USB_FLAG
        data_queue_node_st qnode_tx = {0};

        qnode = &qnode_tx;
        qnode->buff             = (wf_u8 *)buff;
        qnode->real_size        = buff_len;
        qnode->addr             = addr;
        qnode->tx_info          = tx_info;
        qnode->param            = param;
        qnode->tx_callback_func = tx_callback_func;
        qnode->agg_num          = agg_num;
        qnode->state            = TX_STATE_IDL;
        if (NULL != param)
        {
            pxmitbuf = param;
            qnode->pg_num = pxmitbuf->pg_num;
            qnode->encrypt_algo     = pxmitbuf->encrypt_algo;
            qnode->qsel             = pxmitbuf->qsel;
            qnode->ether_type       = pxmitbuf->ether_type;
            qnode->icmp_pkt         = pxmitbuf->icmp_pkt;
        }
        qnode->state            = TX_STATE_INSERT;

        hif_node->ops->hif_write(hif_node, WF_USB_NET_PIP, addr, (char*)qnode, qnode->real_size);
#   endif
    }

    return ret;
#else
    wf_s32 ret = 0;
    data_queue_node_st qnode_tx = {0};
    hif_node_st *hif_node       = hif_info;
    data_queue_node_st *qnode = &qnode_tx;
    struct xmit_buf *pxmitbuf   = NULL;

    qnode->buff             = (wf_u8 *)buff;
    qnode->real_size        = buff_len;
    qnode->addr             = addr;
    qnode->tx_info          = tx_info;
    qnode->param            = param;
    qnode->tx_callback_func = tx_callback_func;
    qnode->agg_num          = agg_num;
    qnode->state            = TX_STATE_IDL;
    if (NULL != param)
    {
        pxmitbuf = param;
        qnode->pg_num = pxmitbuf->pg_num;
    }
    qnode->state            = TX_STATE_INSERT;


#if !defined(CONFIG_SDIO_FLAG)
    if (HIF_USB == hif_node->hif_type)
    {
        hif_node->ops->hif_write(hif_node, WF_USB_NET_PIP, addr, (char*)qnode, qnode->real_size);
    }
    else
#endif
#if !defined(CONFIG_USB_FLAG)
    {
        hif_node->ops->hif_write(hif_node, WF_SDIO_TRX_QUEUE_FLAG, addr, (char*)qnode, qnode->real_size);
    }
#endif

    return ret;
#endif
}

void wf_tx_hif_queue_work(void *hif_info)
{
    hif_node_st *hif_node = (hif_node_st *)hif_info;
    data_queue_mngt_st *trxq  = (data_queue_mngt_st *)&hif_node->trx_pipe;

    trxq->hif_tx_wq.ops->workqueue_work(&trxq->hif_tx_wq);
}

data_queue_node_st* wf_tx_queue_remove(data_queue_mngt_st *trxq)
{
    data_queue_node_st *qnode   = NULL;
    wf_que_t      *tx_queue     = &trxq->tx_queue;

    /*get node from free_tx_queue*/
    wf_lock_lock(&trxq->queu_txop_lock);
    qnode = wf_data_queue_remove(tx_queue);
    if (NULL != qnode)
    {
        if (TX_STATE_INSERT != qnode->state)
        {
            LOG_W("[%s], state[%d] != TX_STATE_INSERT", __func__, qnode->state);
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
//    LOG_I("[%s, %d] ether_type:ARP", __func__, __LINE__);
        ret = wf_true;
    }
    else if (0x86dd == qnode->ether_type)
    {
//    LOG_I("[%s, %d] ether_type:IPv6", __func__, __LINE__);
        ret = wf_true;
    }
    else if (0x888e == qnode->ether_type)
    {
//    LOG_I("[%s, %d] ether_type:802.1x", __func__, __LINE__);
        ret = wf_true;
    }
    else if (0x8864 == qnode->ether_type)
    {
//    LOG_I("[%s, %d] ether_type:PPPoE", __func__, __LINE__);
        ret = wf_true;
    }
    else if (0x0800 == qnode->ether_type && qnode->icmp_pkt)
    {
//    LOG_I("[%s, %d] ether_type:IP--ICMP", __func__, __LINE__);
        ret = wf_true;
    }

    return ret;
}

static void agg_update_send_core(hif_node_st *hif_info,
                                 data_queue_node_st *agg_qnode,
                                 wf_u8 agg_num,
                                 wf_u32 pkt_len)
{
    hif_sdio_st *sd = &hif_info->u.sdio;
    wf_u8 send_pg_num = WF_RND4(agg_qnode->pg_num);

    wf_tx_agg_num_fill(agg_num, sd->tx_agg_buffer);
    agg_qnode->buff         = sd->tx_agg_buffer;
    agg_qnode->real_size = pkt_len;
    agg_qnode->agg_num  = agg_num;
    sd->free_tx_page -= send_pg_num;
    sd->tx_state = TX_STATE_SENDING;
    hif_info->ops->hif_write(hif_info,
                             WF_SDIO_TRX_QUEUE_FLAG,
                             agg_qnode->addr,
                             (char*)agg_qnode,
                             agg_qnode->real_size);
    sd->tx_state = TX_STATE_COMPETE;
    wf_sdio_tx_flow_free_pg_ctl(hif_info, agg_qnode->hw_queue, send_pg_num);
    wf_sdio_tx_flow_agg_num_ctl(hif_info, agg_qnode->agg_num);
}

static void wf_hif_tx_work_agg(wf_work_t *work)
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
    int bret                        = 0;
    wf_bool agg_break               = wf_false;
    wf_u32 align_size               = 0;
    wf_workqueue_mgnt_t *wq_mgt     = NULL;
    wf_timer_t timer                = {0};

    wq_mgt = WF_CONTAINER_OF(work, wf_workqueue_mgnt_t, work);
    dqm = WF_CONTAINER_OF(wq_mgt, data_queue_mngt_st, hif_tx_wq);
    if (NULL == dqm)
    {
        return;
    }
    hif_info = WF_CONTAINER_OF(dqm, hif_node_st, trx_pipe);
    if (NULL == hif_info)
    {
        return;
    }
    data_queue = &dqm->tx_queue;
    if (NULL == data_queue)
    {
        return;
    }
    sd = &hif_info->u.sdio;
    if (sd->count_start)
    {
        wf_timer_set(&timer, 0);
        sd->count++;
    }
    while (NULL != (qnode = wf_tx_queue_remove(dqm)))
    {
        if (sd->count_start)
        {
            sd->tx_pkt_num++;
        }
        sd->tx_state = qnode->state = TX_STATE_FLOW_CTL;
#ifdef CONFIG_RICHV200
        if (WF_PKT_TYPE_FRAME != (qnode->buff[0] & 0x03)) //not need flow control
        {

            sd->tx_state = qnode->state = TX_STATE_SENDING;
            hif_info->ops->hif_write(hif_info, WF_SDIO_TRX_QUEUE_FLAG, qnode->addr, (char*)qnode, qnode->real_size);
        }
        else
#endif
        {

            if (wf_true == pkt_is_agg_disable(qnode))
            {
                agg_break = wf_true;
            }
            else
            {
                agg_break = wf_false;
            }
            //update max_page_num, max_agg_num
            if (0 == agg_num || agg_qnode.qsel != qnode->qsel)
            {
                wf_sdio_update_txbuf_size(hif_info, qnode, &max_page_num, &max_agg_num);
            }

            align_size = TXDESC_SIZE + WF_RND_MAX((qnode->real_size-TXDESC_SIZE), 8);
            if ((0 != agg_num && WF_RND4(agg_qnode.pg_num + qnode->pg_num) > max_page_num) ||
                (0 != agg_num &&  agg_qnode.qsel != qnode->qsel)                ||
                (0 != agg_num && agg_num > max_agg_num)                        ||
                (0 != agg_num && wf_true == agg_break)
               )
            {
                if (sd->count_start)
                {
                    sd->tx_agg_num++;
                }
//                if (1)
//                {
//                  if (agg_num > 5)
//                  LOG_I("agg_num:%d, pg_num:%d, qsel:%d, hw_queu:%d, max_page_num:%d, max_agg_num:%d, free_node_num:%d",
//                        agg_num, agg_qnode.pg_num, agg_qnode.qsel, agg_qnode.hw_queue, max_page_num, max_agg_num, wf_que_count(&dqm->free_tx_queue));
//                }
                agg_update_send_core(hif_info, &agg_qnode, agg_num, pkt_len);
                agg_num = 0;
                pkt_len = 0;

                //for current qnode
                wf_sdio_update_txbuf_size(hif_info, qnode, &max_page_num, &max_agg_num);

                wf_memcpy(&agg_qnode, qnode, sizeof(agg_qnode));

                agg_num++;
                wf_memcpy(sd->tx_agg_buffer + pkt_len, qnode->buff, qnode->real_size);
                pkt_len += align_size ;
            }
            else
            {
                agg_num++;
                if (1 == agg_num)
                {
                    wf_memcpy(&agg_qnode, qnode, sizeof(agg_qnode));
                }
                else
                {
                    agg_qnode.pg_num    += qnode->pg_num;
                }
//                agg_qnode.pg_num = size_to_pg(agg_qnode.real_size);
                wf_memcpy(sd->tx_agg_buffer + pkt_len, qnode->buff, qnode->real_size);
                pkt_len += align_size ;
            }
        }


        if (qnode->tx_callback_func)
        {
            bret = qnode->tx_callback_func(qnode->tx_info, qnode->param);
            if (wf_true == bret)
            {

            }
            else
            {
                LOG_W("[%s, %d] tx_callback_func failed", __func__, __LINE__);
            }
        }

        qnode->state = TX_STATE_COMPETE;
        wf_data_queue_insert(&hif_info->trx_pipe.free_tx_queue, qnode);
    }

    if (pkt_len > 0 && agg_num > 0)
    {
        if (sd->count_start)
        {
            sd->tx_agg_num++;
        }
//        if (wf_true == agg_break)
//        {
//          if (agg_num > 5)
//          LOG_I("%s agg_num:%d, pg_num:%d, qsel:%d, hw_queu:%d, max_page_num:%d, max_agg_num:%d, free_tx_node_num:%d",
//                (wf_true == agg_break) ?"agg_break":"agg_enable",
//                agg_num, agg_qnode.pg_num, agg_qnode.qsel, agg_qnode.hw_queue, max_page_num, max_agg_num, wf_que_count(&dqm->free_tx_queue));
//        }
        agg_update_send_core(hif_info, &agg_qnode, agg_num, pkt_len);
    }

    if (sd->count_start)
    {
        sd->tx_all_time += wf_timer_elapsed(&timer);
    }
}
#endif

int hif_frame_dispath(hif_node_st *hif_info, hif_netbuf_st *netbuf_node)
{
    int num = 0;
    hw_info_st *hw_info;
    wf_u8 *pdata = NULL;
#ifdef CONFIG_RICHV200
    struct rxd_detail_new *prxd = NULL;
#else
    struct rxd_detail_org *prxd = NULL;
#endif
    wf_u8 bc_addr[WF_ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#ifdef CONFIG_RICHV200
    prxd = (struct rxd_detail_new *)(netbuf_node->payload);
#else
    prxd = (struct rxd_detail_org *)(netbuf_node->payload);
#endif

    pdata = (netbuf_node->payload) + RXDESC_SIZE + 32;

//    LOG_D("[hif_frame_dispath]:mac1:"WF_MAC_FMT, WF_MAC_ARG(get_ra(pdata)));
//    LOG_D("[hif_frame_dispath]:ta:"WF_MAC_FMT, WF_MAC_ARG(get_ta(pdata)));
    while (num < hif_info->nic_number)
    {
        if (NULL == hif_info->nic_info[num])
        {
            LOG_E("%s  nic_info[%d] NULL", __func__, num);
            return WF_RETURN_FAIL;
        }
        hw_info = hif_info->nic_info[num]->hw_info;
        if (hw_info == NULL)
        {
            LOG_E("%s  hw_info[%d] NULL", __func__, num);
            return WF_RETURN_FAIL;
        }

        if (wf_memcmp(hw_info->macAddr, get_ra(pdata), WF_ETH_ALEN) == 0)
        {
            wf_rx_work(hif_info->nic_info[num], netbuf_node);
            return 0;
        }
        else if (wf_memcmp(bc_addr, get_ra(pdata), WF_ETH_ALEN) == 0)
        {
            wf_rx_work(hif_info->nic_info[num], netbuf_node);
        }
        num++;
    }

    return 0;
}

static void wf_hif_rx_workqueue(wf_work_t *work)
{
    data_queue_mngt_st *dqm         = NULL;
    hif_netbuf_st *netbuf_node      = NULL;
    data_queue_mngt_st *trxq        = NULL;
    wf_que_t *data_queue            = NULL;
    data_queue_node_st *qnode       = NULL;
    hif_node_st *hif_info           = NULL;
    wf_workqueue_mgnt_t *wq_mgt     = NULL;

    wq_mgt = WF_CONTAINER_OF(work, wf_workqueue_mgnt_t, work);
    dqm = WF_CONTAINER_OF(wq_mgt, data_queue_mngt_st, hif_rx_wq);
    if (NULL == dqm)
    {
        return;
    }

    hif_info = WF_CONTAINER_OF(dqm, hif_node_st, trx_pipe);
    if (NULL == hif_info)
    {
        return;
    }
    trxq = &hif_info->trx_pipe;

    if (NULL == trxq)
    {
        LOG_E("[%s] rx data queue is null", __func__);
        return;
    }

    while (1)
    {
        netbuf_node = wf_netbuf_queue_remove(&trxq->netbuf_queue);
        if (NULL == netbuf_node)
        {
            break;
        }

        if (hm_get_mod_removed() == wf_true || hif_info->dev_removed == wf_true)
        {
            netbuf_node->len = 0;
            wf_enque_tail(&netbuf_node->node, &trxq->free_netbuf_queue);
            continue;
        }

        if (0 == netbuf_node->len)
        {
            wf_enque_tail(&netbuf_node->node, &trxq->free_netbuf_queue);
            continue;
        }

        hif_frame_dispath(hif_info, netbuf_node);
        netbuf_node->len = 0;
        wf_enque_tail(&netbuf_node->node, &trxq->free_netbuf_queue);
    }

#if 0
    if (HIF_USB == hif_info->hif_type)
    {
        /* check urb node, if have free, used it */
        if (NULL !=(qnode = wf_data_queue_remove(&hif_info->trx_pipe.free_rx_queue)))
        {
            hif_info->ops->hif_read(hif_info, WF_USB_NET_PIP, READ_QUEUE_INX, (wf_u8*)qnode, WF_MAX_RECV_BUFF_LEN_USB);
        }
    }
#endif

    return;
}

int wf_hif_queue_enable(hif_node_st *hif_node)
{
    data_queue_mngt_st *hqueue   = NULL;
    data_queue_node_st *qnode    = NULL;

    LOG_D("[%s] begin", __func__);

    hqueue = &hif_node->trx_pipe;

    /*rx queue*/
    if (HIF_USB == hif_node->hif_type)
    {
//        while (NULL !=(qnode = wf_data_queue_remove(&hif_node->trx_pipe.free_rx_queue)))
//        {
//            hif_node->ops->hif_read(hif_node, WF_USB_NET_PIP, READ_QUEUE_INX, (wf_u8*)qnode, WF_MAX_RECV_BUFF_LEN_USB);
//        }

        hif_node->hif_tr_ctrl = wf_true;
    }
    LOG_D("[%s] end", __func__);
    return WF_RETURN_OK;
}


wf_s32 wf_hif_queue_disable(hif_node_st *hif_node)
{
    return WF_RETURN_OK;
}


int wf_data_queue_mngt_init(void *hif_node)
{
    int i                               = 0;
    hif_netbuf_st *netbuf_node          = NULL;
    data_queue_node_st *recv_node       = NULL;
    data_queue_node_st *send_node       = NULL;
    hif_node_st *hif_info               = (hif_node_st *)hif_node;
    data_queue_mngt_st *data_queue_mngt = &hif_info->trx_pipe;

#ifdef CONFIG_SOFT_TX_AGGREGATION
    wf_que_init(&data_queue_mngt->tx_queue, WF_LOCK_TYPE_IRQ);
    wf_lock_init(&data_queue_mngt->queu_txop_lock, WF_LOCK_TYPE_NONE);
    wf_que_init(&data_queue_mngt->free_tx_queue, WF_LOCK_TYPE_IRQ);
#endif
    wf_que_init(&data_queue_mngt->netbuf_queue, WF_LOCK_TYPE_IRQ);
    wf_que_init(&data_queue_mngt->free_netbuf_queue, WF_LOCK_TYPE_IRQ);
    wf_que_init(&data_queue_mngt->free_rx_queue, WF_LOCK_TYPE_IRQ);

    /**
     * trx workqueue register
     */
#ifdef CONFIG_SOFT_TX_AGGREGATION
    {
        wf_workqueue_func_param_t wq_hif_tx_param =
        {"hif_tx", wf_hif_tx_work_agg};
        wf_os_api_workqueue_register(&data_queue_mngt->hif_tx_wq,
                                     &wq_hif_tx_param);
    }
#endif
    {
        wf_workqueue_func_param_t wq_hif_rx_param =
        {"hif_rx", wf_hif_rx_workqueue};
        wf_os_api_workqueue_register(&data_queue_mngt->hif_rx_wq,
                                     &wq_hif_rx_param);
    }

    /**
     * trx create buf
     */
    /* tx agg buf */
#ifdef CONFIG_SOFT_TX_AGGREGATION
    if (HIF_SDIO == hif_info->hif_type)
    {
        hif_info->u.sdio.tx_agg_buffer = wf_kzalloc(TX_AGG_BUFF_SIZE);
        if (NULL == hif_info->u.sdio.tx_agg_buffer)
        {
            LOG_E("[%s] tx_agg_buffer failed", __func__);
            return 0;
        }
        hif_info->u.sdio.free_tx_page = 0;
    }
#endif
    /* rx netbuf */
    hif_info->trx_pipe.all_netbuf_queue
        = wf_netbuf_node_malloc(HIF_QUEUE_ALLOC_MEM_NUM);
    for (i = 0; i < HIF_QUEUE_ALLOC_MEM_NUM; i++)
    {
        netbuf_node = hif_info->trx_pipe.all_netbuf_queue + i;
#if 0
        if (HIF_USB == hif_info->hif_type)
        {
            netbuf_node->mem = wf_kzalloc(WF_MAX_RECV_BUFF_LEN_USB);
        }
        else
#endif
        {
            netbuf_node->mem = wf_kzalloc(WF_MAX_RECV_BUFF_LEN_SDIO);
        }
        if (NULL == netbuf_node->mem)
        {
            LOG_E("no memory for netbuf");
            return -WF_RETURN_FAIL;
        }
        netbuf_node->payload =
            (wf_u8 *)WF_N_BYTE_ALIGMENT((SIZE_PTR)(netbuf_node->mem),
                                        HIF_QUEUE_ALLOC_MEM_ALIGN_SZ); /* payload point to align address */
        netbuf_node->len = 0;

        wf_enque_tail(&netbuf_node->node, &data_queue_mngt->free_netbuf_queue);
    }

    /**
     * trx queue init
     */
    /* for rx */
    hif_info->trx_pipe.all_rx_queue =
        wf_queue_node_malloc(WF_RX_MAX_DATA_QUEUE_NODE_NUM);
    for (i = 0; i < WF_RX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {
        recv_node = hif_info->trx_pipe.all_rx_queue + i;
        recv_node->hif_node = hif_node;
        recv_node->real_size = 0;
        recv_node->node_id = i;
#       if 0
        if (HIF_USB == hif_info->hif_type)
        {
            recv_node->u.purb       = usb_alloc_urb(0, GFP_KERNEL);
            if (NULL == recv_node->u.purb)
            {
                LOG_E("[%s] usb_alloc_urb failed", __func__);
                wf_free_skb((struct sk_buff*)recv_node->buff);
                wf_kfree(recv_node);
                continue;
            }
        }
        else if (HIF_SDIO == hif_info->hif_type)
#       endif
        {
            recv_node->u.sd_func = hif_info->u.sdio.func;
        }

        wf_enque_tail(&recv_node->node, &data_queue_mngt->free_rx_queue);
    }
    /* for tx */
#ifdef CONFIG_SOFT_TX_AGGREGATION
    hif_info->trx_pipe.all_tx_queue =
        wf_queue_node_malloc(WF_TX_MAX_DATA_QUEUE_NODE_NUM);
    for(i = 0; i < WF_TX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {
        send_node = hif_info->trx_pipe.all_tx_queue + i;
        send_node->hif_node = hif_info;
        send_node->real_size    = 0;
        send_node->node_id      = i;
#       if 0
        if (HIF_USB == hif_info->hif_type)
        {
            send_node->u.purb = usb_alloc_urb(0, GFP_KERNEL);
            if (NULL == send_node->u.purb)
            {
                LOG_E("[%s] usb_alloc_urb failed", __func__);
                wf_free_skb((struct sk_buff*)send_node->buff);
                wf_kfree(send_node);
                continue;
            }
        }
        else if (HIF_SDIO == hif_info->hif_type)
#       endif
        {
            send_node->u.sd_func = hif_info->u.sdio.func;
        }

        wf_enque_tail(&send_node->node, &data_queue_mngt->free_tx_queue);
    }
#endif

    data_queue_mngt->is_init = wf_true;

    return WF_RETURN_OK;
}


int wf_data_queue_mngt_term(void *hif_node)
{
    wf_list_t *pos = NULL;
    wf_list_t *next = NULL;
    hif_netbuf_st *netbuf_node = NULL;
    data_queue_node_st *data_node = NULL;
    data_queue_node_st *tx_node = NULL;
    hif_node_st  *hif_info = (hif_node_st  *)hif_node;
    data_queue_mngt_st *trxq = &hif_info->trx_pipe;

#ifdef CONFIG_SOFT_TX_AGGREGATION
    trxq->hif_tx_wq.ops->workqueue_term(&trxq->hif_tx_wq);
#endif
    trxq->hif_rx_wq.ops->workqueue_term(&trxq->hif_rx_wq);

    if (HIF_SDIO == hif_info->hif_type)
    {
        if (hif_info->u.sdio.tx_agg_buffer)
        {
            wf_kfree(hif_info->u.sdio.tx_agg_buffer);
            hif_info->u.sdio.tx_agg_buffer = NULL;
        }
    }

    /*rx queue free */
    wf_list_for_each_safe(pos, next, &trxq->free_rx_queue.head)
    {
        data_node = wf_list_entry(pos, data_queue_node_st, node);
        if (data_node)
        {
            hif_info  = (hif_node_st  *)data_node->hif_node;
//            if (HIF_USB == hif_info->hif_type)
//            {
//                usb_kill_urb(data_node->u.purb);
//                usb_free_urb(data_node->u.purb);
//            }
            wf_list_delete(&data_node->node);
            data_node = NULL;
            trxq->free_rx_queue.cnt--;
        }
    }
    wf_que_deinit(&trxq->free_rx_queue);

    if (trxq->all_rx_queue)
    {
        wf_kfree(trxq->all_rx_queue);
        trxq->all_rx_queue = NULL;
    }

    /*netbuf free */
    wf_list_for_each_safe(pos, next, &trxq->free_netbuf_queue.head)
    {
        netbuf_node = wf_list_entry(pos, hif_netbuf_st, node);
        if (netbuf_node)
        {
            wf_list_delete(&netbuf_node->node);
            wf_kfree(netbuf_node->mem);
            netbuf_node = NULL;
            trxq->free_rx_queue.cnt--;
        }
    }
    wf_list_for_each_safe(pos, next, &trxq->netbuf_queue.head)
    {
        netbuf_node = wf_list_entry(pos, hif_netbuf_st, node);
        if (netbuf_node)
        {
            wf_list_delete(&netbuf_node->node);
            wf_kfree(netbuf_node->mem);
            netbuf_node = NULL;
            trxq->free_rx_queue.cnt--;
        }
    }

    if (trxq->all_netbuf_queue)
    {
        wf_kfree(trxq->all_netbuf_queue);
        trxq->all_netbuf_queue = NULL;
    }

    wf_que_deinit(&trxq->netbuf_queue);
    wf_que_deinit(&trxq->free_netbuf_queue);
    wf_que_deinit(&trxq->free_rx_queue);

#ifdef CONFIG_SOFT_TX_AGGREGATION
    wf_list_for_each_safe(pos, next, &trxq->free_tx_queue.head)
    {
        tx_node = wf_list_entry(pos, data_queue_node_st, node);
        if (tx_node)
        {
            hif_info  = (hif_node_st *)tx_node->hif_node;
            if (HIF_USB == hif_info->hif_type)
            {
//                usb_kill_urb(tx_node->u.purb);
//                usb_free_urb(tx_node->u.purb);
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
#endif

    return WF_RETURN_OK;
}

