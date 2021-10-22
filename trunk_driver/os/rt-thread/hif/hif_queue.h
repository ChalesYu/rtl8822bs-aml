/*
 * hif_queue.h
 *
 * hif queue functiuon declare.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __HIF_QUEUE_H__
#define __HIF_QUEUE_H__

#include "wf_os_api.h"
#include "wf_list.h"
#include "wf_que.h"

#define WF_TX_MAX_DATA_QUEUE_NODE_NUM   (XMIT_DATA_BUFFER_CNT + XMIT_MGMT_BUFFER_CNT)
#define WF_RX_MAX_DATA_QUEUE_NODE_NUM   (8)

#ifdef CONFIG_SOFT_RX_AGGREGATION
#define WF_MAX_RECV_BUFF_LEN_USB        (1024 * 9)
#define WF_MAX_RECV_BUFF_LEN_SDIO       ((1024 * AGG_LEN)+512)
#else
#define WF_MAX_RECV_BUFF_LEN_USB        (1024 * 4)
#define WF_MAX_RECV_BUFF_LEN_SDIO       (1024 * 4)
#endif

#define HIF_QUEUE_ALLOC_MEM_ALIGN_SZ    (4)
#define HIF_QUEUE_ALLOC_MEM_NUM         (6)

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

typedef struct hif_netbuf {
  wf_list_t node;
  wf_u8 *mem;
  wf_u8 *payload;
  wf_u16 len;
} hif_netbuf_st;

typedef struct
{
    wf_list_t node;
    void *hif_node;//point to hif_node_st
    wf_u8 *buff;   //point to sk_buff for rx
    wf_u32 buff_size;
    wf_u32 real_size;
    wf_u32 addr;
    wf_u8  agg_num;
    wf_u8  pg_num;
    wf_u8 encrypt_algo;
    wf_u8 qsel;
    wf_u16 ether_type;
    wf_u8 icmp_pkt;
    wf_u8 hw_queue;
    wf_u32 fifo_addr;
    union
    {
//        struct urb * purb;
        struct rt_sdio_function *sd_func;
    }u;

    void *tx_info;
    void *param;
    int (*tx_callback_func)(void *tx_info, void *param);
    int node_id;
    hif_queue_node_state_enum state;
}data_queue_node_st;


typedef struct trx_queue_st_
{
    wf_que_t netbuf_queue;
    wf_que_t free_netbuf_queue;
    hif_netbuf_st *all_netbuf_queue;
    
    wf_que_t free_rx_queue;
    data_queue_node_st *all_rx_queue;
    
    wf_que_t free_tx_queue;
    wf_que_t tx_queue;
    data_queue_node_st *all_tx_queue;
    wf_lock_t queu_txop_lock; //for tx queue operation lock.
    
    wf_u64 rx_queue_cnt;
    wf_u64 tx_queue_cnt;
    
    /* hif layer workqueue */
    wf_workqueue_mgnt_st hif_tx_wq;
    wf_workqueue_mgnt_st hif_rx_wq;
    
    wf_bool is_init;
}data_queue_mngt_st;

int wf_data_queue_mngt_init(void *hif_node);
int wf_data_queue_mngt_term(void *hif_node);

int wf_tx_queue_insert(void *hif_info,wf_u8 agg_num, char *buff, wf_u32 buff_len, wf_u32 addr,
                            int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param);
void wf_tx_hif_queue_work(void *hif_info);

int wf_tx_queue_empty(void *hif_info);


int wf_data_queue_insert(wf_que_t *queue, data_queue_node_st *qnode);

hif_netbuf_st *wf_netbuf_queue_remove(wf_que_t *queue);

#endif
