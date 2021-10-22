/*
 * tx_linux.c
 *
 * used for frame rx handle for linux
 *
 * Author: renhaibo
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
#include <net/ieee80211_radiotap.h>
#include "ndev_linux.h"
#include "hif.h"
#include "tx.h"
#include "common.h"

static wf_bool tx_work_need_stop(nic_info_st *nic_info)
{
    wf_u16 stop_flag;
    tx_info_st *ptx_info = nic_info->tx_info;

    wf_lock_lock(&ptx_info->lock);
    stop_flag = ptx_info->xmit_stop_flag;
    wf_lock_unlock(&ptx_info->lock);

    return stop_flag;
}

static wf_bool mpdu_send_complete_cb(nic_info_st *nic_info, struct xmit_buf *pxmitbuf)
{
    tx_info_st *tx_info = nic_info->tx_info;

    wf_xmit_buf_delete(tx_info, pxmitbuf);

    tx_work_wake(nic_info->ndev);

    return wf_true;
}

static wf_bool mpdu_insert_sending_queue(nic_info_st *nic_info, struct xmit_frame *pxmitframe, wf_bool ack)
{
    wf_u8 *mem_addr;
    wf_u32 ff_hwaddr;
    wf_bool bRet = wf_true;
    int ret;
    wf_bool inner_ret = wf_true;
    wf_bool blast = wf_false;
    int t, sz, w_sz, pull = 0;
    struct xmit_buf *pxmitbuf = pxmitframe->pxmitbuf;
    hw_info_st *hw_info = nic_info->hw_info;
    wf_u32  txlen = 0;

    mem_addr = pxmitframe->buf_addr;

    for (t = 0; t < pxmitframe->nr_frags; t++)
    {
        if (inner_ret != wf_true && ret == wf_true)
            ret = wf_false;

        if (t != (pxmitframe->nr_frags - 1))
        {
            LOG_D("pxmitframe->nr_frags=%d\n", pxmitframe->nr_frags);
            sz = hw_info->frag_thresh;
            sz = sz - 4 - 0; /* 4: wlan head filed????????? */
        }
        else
        {
            /* no frag */
            blast = wf_true;
            sz = pxmitframe->last_txcmdsz;
        }

        pull = wf_tx_txdesc_init(pxmitframe, mem_addr, sz, wf_false, 1);
        if (pull)
        {
            mem_addr += PACKET_OFFSET_SZ; /* pull txdesc head */
            pxmitframe->buf_addr = mem_addr;
            w_sz = sz + TXDESC_SIZE;
        }
        else
        {
            w_sz = sz + TXDESC_SIZE + PACKET_OFFSET_SZ;
        }

        if (wf_sec_encrypt(pxmitframe, mem_addr, w_sz))
        {
            ret = wf_false;
            LOG_E("encrypt fail!!!!!!!!!!!");
        }
        ff_hwaddr = wf_quary_addr(pxmitframe->qsel);

        txlen = TXDESC_SIZE + pxmitframe->last_txcmdsz;
        pxmitbuf->ether_type = pxmitframe->ether_type;
        pxmitbuf->icmp_pkt   = pxmitframe->icmp_pkt;
        pxmitbuf->qsel       = pxmitframe->qsel;
        wf_timer_set(&pxmitbuf->time, 0);

        if(blast)
        {
            ret = wf_io_write_data(nic_info, 1, mem_addr, w_sz,
                                   ff_hwaddr,(void *)mpdu_send_complete_cb, nic_info, pxmitbuf);
        }
        else
        {
            ret = wf_io_write_data(nic_info, 1, mem_addr, w_sz,
                                   ff_hwaddr, NULL, nic_info, pxmitbuf);
        }

        if (WF_RETURN_FAIL == ret)
        {
            bRet = wf_false;
            break;
        }

        wf_tx_stats_cnt(nic_info, pxmitframe, sz);

        mem_addr += w_sz;
        mem_addr = (wf_u8 *) WF_RND4(((SIZE_PTR) (mem_addr)));
    }

    return bRet;
}



static int tx_work_mpdu_xmit(nic_info_st *nic_info)
{
    struct xmit_frame *pxframe = NULL;
    tx_info_st *tx_info = nic_info->tx_info;
    struct xmit_buf *pxmitbuf = NULL;
    wf_s32 res = wf_false;
    wf_bool bTxQueue_empty;
    int addbaRet = -1;
    wf_bool bRet = wf_false;
    mlme_info_t *mlme_info = nic_info->mlme_info;
    hw_info_st *hw_info = nic_info->hw_info;
    struct sk_buff *skb;

    while (1)
    {
        if(WF_CANNOT_RUN(nic_info))
        {
            return -1;
        }

        if(tx_work_need_stop(nic_info))
        {
            LOG_D("[%d]wf_tx_send_need_stop, just return it", nic_info->ndev_id);
            return -1;
        }

        bTxQueue_empty = wf_que_is_empty(&tx_info->pending_frame_queue);
        if (bTxQueue_empty == wf_true)
        {
            //LOG_D("tx_work_mpdu_xmit break, tx queue empty");
            break;
        }

        pxmitbuf = wf_xmit_buf_new(tx_info);
        if (pxmitbuf == NULL)
        {
            //LOG_D("tx_work_mpdu_xmit break, no xmitbuf");
            break;
        }

        pxframe = wf_tx_data_getqueue(tx_info);
        if (pxframe)
        {
            pxframe->pxmitbuf = pxmitbuf;
            pxframe->buf_addr = pxmitbuf->pbuf;
            pxmitbuf->priv_data = pxframe;

            /* error msdu */
            if (pxframe->priority > 15)
            {
                wf_xmit_buf_delete(tx_info, pxmitbuf);
                wf_xmit_frame_delete(tx_info, pxframe);
                dev_kfree_skb_any(pxframe->pkt);
                pxframe->pkt = NULL;
                LOG_E("[%s]:error msdu", __func__);
                break;
            }

            /* BA start check */
            if (mlme_info->link_info.num_tx_ok_in_period_with_tid[pxframe->qsel] > 100 && (hw_info->ba_enable == wf_true))
            {
                addbaRet = wf_action_frame_add_ba_request(nic_info, pxframe);
                if (addbaRet == 0)
                {
                    LOG_I("Send Msg to MLME for starting BA!!");
                    wf_xmit_buf_delete(tx_info, pxmitbuf);
                    break;
                }
            }

            /* msdu2mpdu */
            if (pxframe->pkt != NULL)
            {
                skb = (struct sk_buff *)pxframe->pkt;

                res = wf_tx_msdu_to_mpdu(nic_info, pxframe, skb->data, skb->len);


                dev_kfree_skb_any(pxframe->pkt);
                pxframe->pkt = NULL;
            }

            /* send to hif tx queue */
            if (res == wf_true)
            {
                bRet = mpdu_insert_sending_queue(nic_info, pxframe, wf_false);
                if (bRet == wf_false)
                {
                    wf_xmit_buf_delete(tx_info, pxmitbuf);
                }
                else
                {
                    wf_xmit_frame_delete(tx_info, pxframe);
                }

                /* check tx resource */
                bRet = wf_need_wake_queue(nic_info);
                if (bRet == wf_true)
                {
                    tx_info_st *tx_info = nic_info->tx_info;
                    LOG_W("<<<<ndev tx start queue,free:%d,pending:%d",tx_info->free_xmitframe_cnt,tx_info->pending_frame_cnt);
                    ndev_tx_resource_enable(nic_info->ndev, pxframe->pkt);
                }
            }
            else
            {
                LOG_E("wf_tx_msdu_to_mpdu error!!");

                wf_xmit_buf_delete(tx_info, pxmitbuf);
                wf_xmit_frame_delete(tx_info, pxframe);
            }
        }
        else
        {
            wf_xmit_buf_delete(tx_info, pxmitbuf);
            break;
        }
    }

    return 0;
}

void tx_work_init(struct net_device *ndev)
{
    ndev_priv_st *ndev_priv;
    nic_info_st *nic_info;

    ndev_priv = netdev_priv(ndev);
    nic_info = ndev_priv->nic;

    tasklet_init(&ndev_priv->get_tx_data_task, (void (*)(unsigned long))tx_work_mpdu_xmit,(unsigned long)nic_info);
}

void tx_work_term(struct net_device *ndev)
{
    ndev_priv_st *ndev_priv;
    nic_info_st *nic_info;
    tx_info_st *tx_info;
    struct xmit_frame *pxmitframe;

    ndev_priv = netdev_priv(ndev);
    nic_info = ndev_priv->nic;
    tx_info = nic_info->tx_info;

    while (wf_que_is_empty(&tx_info->pending_frame_queue) == wf_false)
    {
        pxmitframe = wf_tx_data_getqueue(tx_info);
        wf_xmit_frame_delete(tx_info, pxmitframe);
        if (pxmitframe->pkt)
        {
            dev_kfree_skb_any(pxmitframe->pkt);
            pxmitframe->pkt = NULL;
        }
    }

    tasklet_kill(&ndev_priv->get_tx_data_task);
}

void tx_work_wake(struct net_device *ndev)
{
    ndev_priv_st *ndev_priv;

    ndev_priv = netdev_priv(ndev);

    tasklet_schedule(&ndev_priv->get_tx_data_task);
}

