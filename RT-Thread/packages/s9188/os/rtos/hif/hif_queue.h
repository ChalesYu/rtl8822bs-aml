
#ifndef __HIF_QUEUE_H__
#define __HIF_QUEUE_H__

#define WF_RX_MAX_DATA_QUEUE_NODE_NUM   (8)

#ifdef CONFIG_SOFT_RX_AGGREGATION
#define WF_MAX_RECV_BUFF_LEN_USB        (1024 * 9)
#define WF_MAX_RECV_BUFF_LEN_SDIO       (1024 * 16)
#else
#define WF_MAX_RECV_BUFF_LEN_USB        (1024 * 4)
#define WF_MAX_RECV_BUFF_LEN_SDIO       (1024 * 4)
#endif

#define HIF_QUEUE_ALLOC_MEM_ALIGN_SZ    (4)
#define HIF_QUEUE_ALLOC_MEM_NUM         (6)
//#define HIF_MAX_ALLOC_MEM_CNT           (8)

typedef enum DATA_QUEUE_NODE_STATUS_
{
    TX_STATE_IDL   = 0,
    TX_STATE_INSERT = 1,
    TX_STATE_IN_PIP  = 2,
    TX_STATE_FLOW_CTL = 3,
    TX_STATE_FLOW_CTL_SECOND = 4,
    TX_STATE_SENDING = 5,
    TX_STATE_COMPETE = 6,
} hif_queue_node_state_enum;

typedef struct hif_netbuf
{
    wf_list_t node;
    wf_u8 *mem;
    wf_u8 *payload;
    wf_u16 len;
} hif_netbuf_t;

typedef struct
{
    wf_list_t node;
    void *hif_node;//point to hif_node_t
    wf_u8 *buff;   //point to sk_buff for rx
    wf_u32 buff_size;
    wf_u32 real_size;
    wf_u32 addr;
    wf_u8  agg_num;
    wf_u8  pg_num;
    union
    {
//        struct urb * purb;
        sdio_func_t *sd_func;
    } u;

    void *tx_info;
    void *param;
    int (*tx_callback_func)(void *tx_info, void *param);
    int node_id;
    hif_queue_node_state_enum state;
} data_queue_node_t;


typedef struct
{
    wf_que_t netbuf_queue;
    wf_que_t free_netbuf_queue;
    hif_netbuf_t *all_netbuf_queue;

    wf_que_t free_rx_queue;
    data_queue_node_t *all_rx_queue;

    wf_u64 rx_queue_cnt;

    /*hif rx handle*/
    wf_workqueue_mgnt_st rx_wq;
} data_queue_mngt_t;

int wf_data_queue_mngt_init(void *hif_node);
int wf_data_queue_mngt_term(void *hif_node);

int wf_tx_queue_insert(void *hif_info,wf_u8 agg_num, char *buff, wf_u32 buff_len, wf_u32 addr,
                       int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param);

int wf_tx_queue_empty(void *hif_info);


int wf_data_queue_insert(wf_que_t *queue, data_queue_node_t *qnode);

hif_netbuf_t *wf_netbuf_queue_remove(wf_que_t *queue);

#endif

