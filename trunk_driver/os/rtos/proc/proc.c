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
/* includes */
#include "common.h"
#include "hif/hif.h"

/* macro */
#define container_of(ptr, type, member)     ({(type *)((char *)ptr - ((size_t)&((type *)0)->member));})

/* function declaration */
int wf_get_wlan_mgmt_info (nic_info_st *pnic_info)
{
    wf_wlan_mgmt_info_t *pwlan_mgmt_info;
    hif_node_st *hif_info;

    hif_info  = pnic_info->hif_node;
    if (NULL == hif_info)
    {
        LOG_W("[%s] hif_info is null", __func__);
        return -1;
    }

    pnic_info = hif_info->nic_info[0];
    if (NULL == pnic_info)
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
        printf("pscan_que->free.count=%d\r\n", wf_que_count(&pscan_que->free));
        printf("pscan_que->ready.count=%d\r\n", wf_que_count(&pscan_que->ready));
        printf("pscan_que->read_cnt=%d\r\n", pscan_que->read_cnt);
        if (0)
        {
            wf_wlan_mgmt_scan_que_node_t *pscan_que_node;
            wf_wlan_mgmt_scan_que_for_rst_e scan_que_for_rst;
            printf("-------------------------\r\n");
            wf_wlan_mgmt_scan_que_for_begin(pnic_info, pscan_que_node)
            {
                printf("sig_str: %d, ssid: %s\r\n",
                       pscan_que_node->signal_strength, pscan_que_node->ssid.data);
            }
            wf_wlan_mgmt_scan_que_for_end(scan_que_for_rst);
        }
    }

    return 0;
}

int wf_get_mlme_info (nic_info_st *pnic_info)
{
    hif_node_st *hif_info;
    mlme_info_t *pmlme_info;
    wf_u8 i = 0;

    hif_info  = pnic_info->hif_node;
    if (NULL == hif_info)
    {
        LOG_W("[%s] hif_info is null", __func__);
        return -1;
    }

    for (i=0; i<hif_info->nic_number; i++)
    {
        pnic_info = hif_info->nic_info[i];
        if (NULL == pnic_info)
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
            printf("[%d] pmlme_info->link_info.busy_traffic=%d\r\n",
                   pnic_info->ndev_id, pmlme_info->link_info.busy_traffic);
        }
    }

    return 0;
}

int wf_get_rx_info (nic_info_st *pnic_info)
{
    hif_node_st *hif_info;
    wdn_net_info_st *wdn_net_info = NULL;
    data_queue_node_st *data_node = NULL;
    int i = 0;

    hif_info  = pnic_info->hif_node;
    if (NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }
    /*hif debug info*/
    printf("node_id:%d\r\n", hif_info->node_id);
    printf("hif_type:%d\r\n", hif_info->hif_type);
    printf("rx_queue_cnt:%lld\r\n", hif_info->trx_pipe.rx_queue_cnt);
    printf("free rx data queue node num:%d\r\n", hif_info->trx_pipe.free_rx_queue.cnt);

    for (i=0; i<WF_RX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {
        data_node = hif_info->trx_pipe.all_rx_queue + i;
        if (0 != data_node->state)
        {
            printf("[%d] state:%d, pg_num:%d,agg_num:%d\r\n",
                   data_node->node_id, data_node->state, data_node->pg_num,
                   data_node->agg_num);
        }
    }

    if (HIF_SDIO == hif_info->hif_type)
    {
        hif_sdio_st *sd = &hif_info->u.sdio;

        printf("irq_cnt:%lld, int_flag:%d\r\n", sd->irq_cnt, sd->int_flag);
        printf("0x9008:0x%08x\r\n", hif_io_read32(hif_info, SDIO_BASE | WL_REG_HISR, NULL));
        printf("0x284:0x%08x\r\n", hif_io_read32(hif_info, 0x284, NULL));
        printf("0x288:0x%08x\r\n", hif_io_read32(hif_info, 0x288, NULL));
        printf("0x114:0x%08x\r\n", hif_io_read32(hif_info, 0x114, NULL));
        printf("0x200:0x%08x\r\n", hif_io_read32(hif_info, 0x200, NULL));
        printf("0x204:0x%08x\r\n", hif_io_read32(hif_info, 0x204, NULL));
        printf("0x208:0x%08x\r\n", hif_io_read32(hif_info, 0x208, NULL));
        printf("0x20C:0x%08x\r\n", hif_io_read32(hif_info, 0x20C, NULL));
        printf("0x210:0x%08x\r\n", hif_io_read32(hif_info, 0x210, NULL));
        printf("0x214:0x%08x\r\n", hif_io_read32(hif_info, 0x214, NULL));
        printf("0x218:0x%08x\r\n", hif_io_read32(hif_info, 0x218, NULL));
        printf("0x21c:0x%08x\r\n", hif_io_read32(hif_info, 0x21c, NULL));
    }

    if (NULL != wdn_net_info)
    {
        int tid = 0;

        if (wdn_net_info->ba_ctl != NULL)
        {
            for (tid=0; tid<TID_NUM; tid++)
            {
                recv_ba_ctrl_st *ba_ctl = &wdn_net_info->ba_ctl[tid];
                if (NULL != ba_ctl && wf_true == ba_ctl->enable)
                {
                    printf("[%d] rx reorder drop:%lld\r\n", tid, ba_ctl->drop_pkts);
                    printf("[%d] timeout_cnt:%u\r\n", tid, ba_ctl->timeout_cnt);
                }
            }
        }
    }

    return 0;
}

int wf_get_tx_info (nic_info_st *pnic_info)
{
    hif_node_st *hif_info;
    tx_info_st *tx_info = NULL;
    data_queue_node_st *data_node = NULL;
    int i = 0;

    hif_info  = pnic_info->hif_node;
    if (NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }
    /*hif debug info*/
    printf("node_id:%d\r\n", hif_info->node_id);
    printf("hif_type:%d\r\n", hif_info->hif_type);
    printf("tx_queue_cnt:%lld\r\n", hif_info->trx_pipe.tx_queue_cnt);
    printf("free tx data queue node num:%d\r\n", wf_que_count(&hif_info->trx_pipe.free_tx_queue));
    printf("tx data queue node num:%d\r\n", wf_que_count(&hif_info->trx_pipe.tx_queue));
    for (i=0; i<WF_TX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {
        data_node = hif_info->trx_pipe.all_tx_queue + i;
        if ((TX_STATE_COMPETE != data_node->state) && (TX_STATE_IDL != data_node->state))
        {
            printf("[%d] state:%d, pg_num:%d,agg_num:%d, addr:0x%x\r\n",
                   data_node->node_id, data_node->state, data_node->pg_num,
                   data_node->agg_num, data_node->addr);
        }
    }

    if (HIF_SDIO == hif_info->hif_type)
    {
        hif_sdio_st *sd = &hif_info->u.sdio;

        printf("tx_fifo_ppg_num    :%d\r\n", sd->tx_fifo_ppg_num);
        printf("tx_fifo_hpg_num    :%d\r\n", sd->tx_fifo_hpg_num);
        printf("tx_fifo_lpg_num    :%d\r\n", sd->tx_fifo_lpg_num);
    }

    pnic_info = hif_info->nic_info[0];
    if (NULL == pnic_info)
    {
        return 0;
    }

    tx_info = pnic_info->tx_info;
    printf("free tx frame num:%d, free_xmitbuf_cnt:%d\r\n",
           tx_info->free_xmitframe_cnt, tx_info->free_xmitbuf_cnt);
    printf("data_queue_check:%d \r\n", wf_io_write_data_queue_check(pnic_info));
    printf("check_tx_buff:%d\r\n", wf_mcu_check_tx_buff(pnic_info));

    return 0;
}

int wf_get_ap_info (nic_info_st *pnic_info)
{
    static hif_node_st *hif_info;
    wf_wlan_mgmt_info_t *pwlan_info;
    wf_wlan_network_t *pcur_network;
    wdn_list *pwdn;
    wdn_net_info_st *pwdn_info;
    wf_list_t *pos, *pos_next;
    sec_info_st *psec_info = NULL;
    int i = 0;

    hif_info  = pnic_info->hif_node;
    if (NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }

    for (i=0; i < hif_info->nic_number; i++)
    {
        pnic_info = hif_info->nic_info[i];
        if (NULL == pnic_info)
        {
            continue;
        }

        printf("--------------nic[%d] ----------\r\n ", i);

        /* ap message free queue */
        pwlan_info = pnic_info->wlan_mgmt_info;
        if (pwlan_info)
        {
            pcur_network = &pwlan_info->cur_network;
            printf("ap_msg_free[WF_AP_MSG_TAG_AUTH_FRAME].count=%d\r\n",
                   pcur_network->ap_msg_free[WF_AP_MSG_TAG_AUTH_FRAME].cnt);
            printf("ap_msg_free[WF_AP_MSG_TAG_ASSOC_REQ_FRAME].count=%d\r\n",
                   pcur_network->ap_msg_free[WF_AP_MSG_TAG_ASSOC_REQ_FRAME].cnt);
            printf("ap_msg_free[WF_AP_MSG_TAG_BEACON_REQ_FRAME].count=%d\r\n",
                   pcur_network->ap_msg_free[WF_AP_MSG_TAG_BEACON_FRAME].cnt);
            printf("pcur_network->ap_tid=%08lx\r\n", (long)pcur_network->ap_tid);
            printf("pcur_network->channel=%d\r\n", pcur_network->channel);
        }

        psec_info = pnic_info->sec_info;
        if (psec_info)
        {
            printf("psec_info->dot11AuthAlgrthm=%d\r\n", psec_info->dot11AuthAlgrthm);
        }
        /* wdn message queue */
        pwdn = pnic_info->wdn;
        pwdn_info = pnic_info->wdn;
        if (pwdn)
        {
            printf("\r\npwdn->cnt=%d", pwdn->cnt);
            printf("\r\npwdn->id_bitmap=0x%x\r\n", pwdn->id_bitmap);
            wf_list_for_each_safe(pos, pos_next, &pwdn->head)
            {
                pwdn_info = &container_of(pos, wdn_node_st, list)->info;
                printf("pwdn_info->wdn_id=%d\r\n", pwdn_info->wdn_id);
                printf("         ->channel=%d\r\n", pwdn_info->channel);
                printf("         ->mac="WF_MAC_FMT"\r\n", WF_MAC_ARG(pwdn_info->mac));
                printf("         ->ieee8021x_blocked=%d\r\n",
                       pwdn_info->ieee8021x_blocked);
                printf("         ->dot118021XPrivacy=%d\r\n",
                       pwdn_info->dot118021XPrivacy);
                printf("         ->ap_msg.count=%d\r\n", pwdn_info->ap_msg.cnt);
                printf("         ->ap_msg.rx_pkt_stat=%d\r\n", pwdn_info->rx_pkt_stat);
            }
        }
    }

    return 0;
}

int wf_get_sta_info (nic_info_st *pnic_info)
{
    hif_node_st *hif_info;
    wdn_list *pwdn;
    wdn_net_info_st *pwdn_info;
    wf_list_t *pos, *pos_next;
    sec_info_st *psec_info = NULL;
    wf_u8 i = 0;
    hif_info  = pnic_info->hif_node;

    if (NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }

    for (i=0; i<hif_info->nic_number; i++)
    {
        pnic_info = hif_info->nic_info[i];
        if (NULL == pnic_info)
        {
            LOG_E("[%s] pnic_info is null", __func__);
            return -1;
        }

        psec_info = pnic_info->sec_info;
        /* ap message free queue */
        if (psec_info)
        {
            printf("[%d] psec_info->dot11AuthAlgrthm=%d\r\n", pnic_info->ndev_id,
                   psec_info->dot11AuthAlgrthm);
        }

        /* wdn message queue */
        pwdn = pnic_info->wdn;
        pwdn_info = pnic_info->wdn;
        if (pwdn)
        {
            printf("\r\n[%d]pwdn->cnt=%d", pnic_info->ndev_id, pwdn->cnt);
            printf("\r\n[%d]pwdn->id_bitmap=0x%x\r\n", pnic_info->ndev_id,
                   pwdn->id_bitmap);
            wf_list_for_each_safe(pos, pos_next, &pwdn->head)
            {
                pwdn_info = &container_of(pos, wdn_node_st, list)->info;
                printf("[%d]pwdn_info->wdn_id=%d\r\n", pnic_info->ndev_id,
                       pwdn_info->wdn_id);
                printf("[%d]         ->mac="WF_MAC_FMT"\r\n", pnic_info->ndev_id,
                       WF_MAC_ARG(pwdn_info->mac));
                printf("[%d]         ->ieee8021x_blocked=%d\r\n", pnic_info->ndev_id,
                       pwdn_info->ieee8021x_blocked);
                printf("[%d]         ->dot118021XPrivacy=%d\r\n", pnic_info->ndev_id,
                       pwdn_info->dot118021XPrivacy);
                printf("[%d]         ->rx_pkt_stat=%d\r\n", pnic_info->ndev_id,
                       pwdn_info->rx_pkt_stat);
                printf("[%d]         ->rx_pkt_stat_last=%d\r\n", pnic_info->ndev_id,
                       pwdn_info->rx_pkt_stat_last);
            }
        }
    }

    return 0;
}

int wf_get_hif_info (nic_info_st *pnic_info)
{
    static hif_node_st *hif_info;
    int i = 0;
    data_queue_node_st *data_node   = NULL;

    hif_info  = pnic_info->hif_node;
    if (NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }

    /*hif debug info*/
    printf("node_id : %d, nic_num:%d\r\n", hif_info->node_id, hif_info->nic_number);
    printf("hif_type: %s\r\n", hif_info->hif_type == 1?"HIF_USB":"HIF_SDIO");

    /*hif--rx info*/
    printf("[rx] all  queue cnt:%lld\r\n", hif_info->trx_pipe.rx_queue_cnt);
    printf("[rx] free queue node num:%d\r\n", hif_info->trx_pipe.free_rx_queue.cnt);

    for (i=0; i<WF_RX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {
        data_node = hif_info->trx_pipe.all_rx_queue + i;
        if ( 0 != data_node->state)
        {
            printf("[rx] qnode(%d) state:%d, pg_num:%d,agg_num:%d\r\n",
                   data_node->node_id, data_node->state, data_node->pg_num,
                   data_node->agg_num);
        }
    }
    if (HIF_SDIO == hif_info->hif_type)
    {
        hif_sdio_st *sd = &hif_info->u.sdio;
        printf("[rx] rx_time:%lld ms\r\n", sd->rx_time );
        printf("[rx] rx_count:%lld\r\n", sd->rx_count );
        printf("[rx] rx_pkt_num:%lld\r\n", sd->rx_pkt_num );
    }

    /*hif--tx info*/
    printf("[tx] all queue cnt:%lld\r\n", hif_info->trx_pipe.tx_queue_cnt);
    printf("[tx] free  tx data queue node num:%d\r\n", hif_info->trx_pipe.free_tx_queue.cnt);
    printf("[tx] using tx data queue node num:%d\r\n", hif_info->trx_pipe.tx_queue.cnt);
    for (i=0; i<WF_TX_MAX_DATA_QUEUE_NODE_NUM; i++)
    {
        data_node = hif_info->trx_pipe.all_tx_queue + i;
        if((TX_STATE_COMPETE != data_node->state) && (TX_STATE_IDL != data_node->state))
        {
            printf("[tx] qnode(%d) state:%d, pg_num:%d, agg_num:%d, addr:0x%x\r\n",
                   data_node->node_id, data_node->state, data_node->pg_num,
                   data_node->agg_num, data_node->addr);
        }
    }
    if (HIF_SDIO == hif_info->hif_type)
    {
        hif_sdio_st *sd = &hif_info->u.sdio;

        printf("[tx] fifo_ppg_num    :%d\r\n", sd->tx_fifo_ppg_num);
        printf("[tx] fifo_hpg_num    :%d\r\n", sd->tx_fifo_hpg_num);
        printf("[tx] fifo_lpg_num    :%d\r\n", sd->tx_fifo_lpg_num);
        printf("[tx] tx_state:%d\r\n", sd->tx_state);
        printf("[tx] tx_all_time:%lld ms\r\n", sd->tx_all_time);
        printf("[tx] tx_flow_ctl_time:%lld ms\r\n", sd->tx_flow_ctl_time);
        printf("[tx] tx_agg_send_time:%lld ms\r\n", sd->tx_agg_send_time);
        printf("[tx] tx_agg_num:%lld \r\n", sd->tx_agg_num);
        printf("[tx] tx_pkt_num:%lld \r\n", sd->tx_pkt_num);
        printf("[tx] count:%lld \r\n", sd->count);
    }

    /*register info*/
    {
        nic_info_st *pnic_info = hif_info->nic_info[0];
        if (NULL == pnic_info)
        {
            LOG_E("[%s] pnic_info is null", __func__);
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
                printf("[reg] cck ok cnt read fail\r\n");
            }
            else
            {
                printf("[reg] cck ok cnt:%d\r\n", cnt & ((1 << 23) - 1)); /* [22:0] */
            }
        }
        {
            int ret;
            wf_u32 cnt;

            wf_io_write32(pnic_info, 0x664, (0 << 28)); /* 26, 31:28 = 0 */
            cnt = wf_io_read32(pnic_info, 0x664, &ret);
            if (ret)
            {
                printf("[reg] OFDM ok cnt read fail\r\n");
            }
            else
            {
                printf("[reg] OFDM ok cnt:%d\r\n", cnt & ((1 << 23) - 1)); /* [22:0] */
            }
        }
        {
            int ret;
            wf_u32 cnt;

            wf_io_write32(pnic_info, 0x664, (6 << 28)); /* 26, 31:28 = 0 */
            cnt = wf_io_read32(pnic_info, 0x664, &ret);
            if (ret)
            {
                printf("[reg] HT ok cnt read fail\r\n");
            }
            else
            {
                printf("[reg] HT ok cnt:%d\r\n", cnt & ((1 << 23) - 1)); /* [22:0] */
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
                printf("[reg] VHT ok cnt read fail\n");
            }
            else
            {
                printf("[reg] VHT ok cnt:%d\n", cnt & ((1 << 23) - 1)); /* [22:0] */
            }
        }
#endif
    }

    return 0;
}

int wf_get_phy_info (nic_info_st *pnic_info)
{
    int i = 0;
    hif_node_st *hif_info  = pnic_info->hif_node;
    wf_u32 read_tx_reg[]= {0x9cc, 0x9d0, 0xf14, 0xf18};
    wf_u32 read_rx_ofdm_reg[]= {0xda0, 0xda4, 0xf90, 0xf94, 0xf98, 0xf9c};
    wf_u32 read_rx_cck_reg[]= {0xfa0, 0xf84, 0xf88};

    if (NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }

    printf("tx_reg:\r\n");
    for (i=0; i < (sizeof(read_tx_reg)/sizeof(read_tx_reg[0])); i++)
    {
        printf("0x%04x:0x%08x\r\n", read_tx_reg[i], hif_io_read32(hif_info, read_tx_reg[i], NULL));
    }

    printf("rx_ofdm_reg:\r\n");
    for (i=0; i < (sizeof(read_rx_ofdm_reg)/sizeof(read_rx_ofdm_reg[0])); i++)
    {
        printf("0x%04x:0x%08x\r\n", read_rx_ofdm_reg[i], hif_io_read32(hif_info,
                read_rx_ofdm_reg[i], NULL));
    }

    printf("read_rx_cck_reg:\r\n");
    for (i=0; i < (sizeof(read_rx_cck_reg)/sizeof(read_rx_cck_reg[0])); i++)
    {
        printf("0x%04x:0x%08x\r\n", read_rx_cck_reg[i], hif_io_read32(hif_info,
                read_rx_cck_reg[i], NULL));
    }

    return 0;
}

int wf_get_mac_info (nic_info_st *pnic_info)
{
    hif_node_st *hif_info  = pnic_info->hif_node;
    int i = 0;
    wf_u32 mac_reg[]= {0x284, 0x288, 0x114, 0x200, 0x204, 0x208, 0x20c, 0x210, 0x214, 0x218, 0x21c};

    if (NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }

    printf("mac_reg:\r\n");
    for (i=0; i < sizeof(mac_reg)/sizeof(mac_reg[0]); i++)
    {
        printf("0x%04x:0x%08x\r\n", mac_reg[i], hif_io_read32(hif_info, mac_reg[i], NULL));
    }

    return 0;
}

int wf_get_phy_cnt_reset (nic_info_st *pnic_info)
{
    wf_u32 reg;
    hif_node_st *hif_info;

    hif_info  = pnic_info->hif_node;
    if (NULL == hif_info)
    {
        LOG_E("[%s] hif_info is null", __func__);
        return -1;
    }

    reg = hif_io_read32(hif_info, 0xf14,NULL);
    if (reg & (1<<16))
    {
        printf("1\r\n");
    }
    else
    {
        printf("0\r\n");
    }

    return 0;
}

