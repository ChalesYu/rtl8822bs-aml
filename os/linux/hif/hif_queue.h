#ifndef __HIF_QUEUE_H__
#define __HIF_QUEUE_H__

#include <linux/list.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>

#define WF_TX_MAX_DATA_QUEUE_NODE_NUM   (32)
#define WF_RX_MAX_DATA_QUEUE_NODE_NUM   (14)

#ifdef CONFIG_SOFT_RX_AGGREGATION
#define WF_MAX_RECV_BUFF_LEN_USB        (1024 * 9)
#define WF_MAX_RECV_BUFF_LEN_SDIO       (1024 * 16)
#else
#define WF_MAX_RECV_BUFF_LEN_USB        (1024 * 4)
#define WF_MAX_RECV_BUFF_LEN_SDIO       (1024 * 4)
#endif


#define HIF_QUEUE_TX_WORKQUEUE_USE      (1)

#define HIF_QUEUE_ALLOC_SKB_ALIGN_SZ    (8)
#define HIF_QUEUE_ALLOC_SKB_NUM         (16)
#define HIF_MAX_ALLOC_CNT               (4)

typedef enum DATA_QUEUE_NODE_STATUS_
{
    TX_STATE_IDL   = 0,
    TX_STATE_INSERT = 1,
    TX_STATE_IN_PIP  = 2,
    TX_STATE_FLOW_CTL = 3,
    TX_STATE_FLOW_CTL_SECOND = 4,
    TX_STATE_SENDING = 5,
    TX_STATE_COMPETE = 6,
}hif_queue_node_state_enum;
typedef struct
{
    wf_list_t node;
    void *hif_node;//point to hif_node_st
    wf_u8 *buff; //point to sk_buff for rx
    wf_u32 buff_size;
    wf_u32 real_size;
    wf_u32 addr;
    wf_u8  agg_num;
    wf_u8  pg_num;
    union
    {
        struct urb * purb;
        struct sdio_func *sd_func;
    }u;

    void *tx_info;
    void *param;
    int (*tx_callback_func)(void *tx_info, void *param);
    int node_id;
    hif_queue_node_state_enum state;
}data_queue_node_st;


typedef struct trx_queue_st_
{
    struct sk_buff_head rx_queue;
    struct sk_buff_head free_rx_queue_skb;
    wf_u8  alloc_cnt;
    wf_que_t free_rx_queue;
    data_queue_node_st *all_rx_queue;

    wf_que_t free_tx_queue;
    wf_que_t tx_queue;//
    data_queue_node_st *all_tx_queue;

    wf_u64 rx_queue_cnt;
    wf_u64 tx_queue_cnt;

    /*hif tx handle*/
    wf_workqueue_mgnt_st tx_wq;
    struct tasklet_struct send_task;

    /*hif rx handle*/
    wf_workqueue_mgnt_st rx_wq;
    struct tasklet_struct recv_task;

}data_queue_mngt_st;

int wf_data_queue_mngt_init(void *hif_node);
int wf_data_queue_mngt_term(void *hif_node);

int wf_tx_queue_remove(data_queue_mngt_st *trxq);
int wf_tx_queue_insert(void *hif_info,wf_u8 agg_num, char *buff, wf_u32 buff_len, wf_u32 addr,
                            int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param);
int wf_tx_queue_full(data_queue_mngt_st *trxq);
int wf_tx_queue_empty(void *hif_info);


int wf_data_queue_insert(wf_que_t *queue, data_queue_node_st *qnode);

data_queue_node_st * wf_data_queue_remove(wf_que_t *queue);

void wf_data_queue_unlock(wf_que_t *queue);
wf_u32 wf_data_queue_len(wf_que_t *queue);

int wf_hif_queue_alloc_skb_one(struct sk_buff_head *skb_head,wf_u8 hif_type);
void wf_hif_queue_alloc_skb(struct sk_buff_head *skb_head,wf_u8 hif_type);

#endif
