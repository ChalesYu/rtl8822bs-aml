/*
 * rx.c
 *
 * used for rx frame handle
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
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"
#include "queue.h"
#ifdef CONFIG_MP_MODE
#include "mp.h"
#endif

#define BA_REORDER_QUEUE_NUM (64)
//#define C2H_ADPT_SPEED_DEBUG
//#define C2H_ADPT_TXACK_DEBUG

#define MMPDU_SIZE          2304
#define MAX_MGNT_HDR_LEN    28
#define MIN_MGNT_HDR_LEN    24
#define MAC_HEADER_OFFSET   56
#define DATA_FRAME_HDR_SHORT    24  /* ALL data frame present */
#define ADDRESS4_LEN            6   /* if To/From_DS = 1, address4 present */
#define QOS_CONTRL_LEN          2   /* if qos field present in subtype field */
#define HT_CONTRL_LEN           4


#define GET_C2H_TX_RPT_LIFE_TIME_OVER(_Header)  wf_le_bits_to_u8((_Header + 0), 6, 1)
#define GET_C2H_TX_RPT_RETRY_OVER(_Header)      wf_le_bits_to_u8((_Header + 0), 7, 1)
#define C2H_ID(_c2h)        wf_le_bits_to_u8(((wf_u8*)(_c2h)), 0, 8)
#define C2H_SEQ(_c2h)       wf_le_bits_to_u8(((wf_u8*)(_c2h)) + 1, 0, 8)
#define C2H_PAYLOAD(_c2h)   (((wf_u8*)(_c2h)) + 2)


wf_inline wf_u8 *pkt_push(prx_pkt_t prx_pkt, int size)
{
    if (prx_pkt->pdata - size < prx_pkt->phead)
        return NULL;

    prx_pkt->pdata -= size;
    prx_pkt->len += size;

    return prx_pkt->pdata;
}

wf_inline static wf_u8 *pkt_put(prx_pkt_t prx_pkt, int size)
{
    wf_u8 *ptmp;

    if (prx_pkt->ptail + size > prx_pkt->pend)
        return NULL;
    ptmp = prx_pkt->ptail;

    prx_pkt->ptail += size;
    prx_pkt->len += size;

    return ptmp;
}
wf_inline static void pkt_reserve(prx_pkt_t prx_pkt, int size)
{
    prx_pkt->pdata += size;
    prx_pkt->ptail += size;
}

wf_inline static void free_rx_pkt(prx_pkt_t prx_pkt, wf_bool bfreeSkb)
{
    prx_info_t prx_info = ((nic_info_st *)((prx_pkt_t)(prx_pkt))->p_nic_info)->rx_info;
    queue_insert_tail(&prx_info->free_rx_pkt_list, &prx_pkt->entry);

#if 0
    if (bfreeSkb)
        wf_destroy_skb(prx_pkt);
#endif
}

static void free_rx_queue(p_que_t prx_q, wf_bool bfreeSkb)
{
    p_que_entry_t p_entry;
    prx_pkt_t prx_pkt;

    while (queue_is_not_empty(prx_q))
    {
        queue_remove_head(prx_q, p_entry, p_que_entry_t);
        if (p_entry == NULL)
            break;
        prx_pkt = (prx_pkt_t)p_entry;
        free_rx_pkt(prx_pkt, bfreeSkb);
    }
}

void set_encrypt_algo_num(prx_pkt_t ppkt, wdn_net_info_st *wdn_net_info)
{
    nic_info_st *pnic_info;
    sec_info_st *psec_info;
    wf_bool bmcast = IS_MCAST(ppkt->pkt_info.rx_addr);

    pnic_info = (nic_info_st *)ppkt->p_nic_info;
    psec_info = (sec_info_st *)pnic_info->sec_info;

    if (!GetPrivacy(ppkt->pdata))
    {
        ppkt->pkt_info.encrypt_algo = _NO_PRIVACY_;
        return ;
    }

    if (psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X)
    {
        ppkt->pkt_info.encrypt_algo =
            bmcast ? psec_info->dot118021XGrpPrivacy : wdn_net_info->dot118021XPrivacy;
    }
    else
    {
        ppkt->pkt_info.encrypt_algo = psec_info->dot11PrivacyAlgrthm;
    }
}

void set_iv_icv_len(prx_pkt_t ppkt)
{
    prx_pkt_info_t prx_info = &ppkt->pkt_info;

    switch (prx_info->encrypt_algo)
    {
        case _NO_PRIVACY_:
            prx_info->iv_len    = 0;
            prx_info->icv_len   = 0;
            break;
        case _WEP40_:
        case _WEP104_:
            prx_info->iv_len = 4;
            prx_info->icv_len = 4;
            break;
        case _TKIP_:
            prx_info->iv_len = 8;
            prx_info->icv_len = 4;
            break;
        case _AES_:
            prx_info->iv_len = 8;
            prx_info->icv_len = 0;
            break;
        case _SMS4_:
            prx_info->iv_len = 18;
            prx_info->icv_len = 16;
            break;
        default:
            prx_info->iv_len = 0;
            prx_info->icv_len = 0;
            break;
    }
}

wf_u8 calc_rx_rate(wf_u8 rx_rate)
{
    wf_u8 tmp_rate = 0;

    if (rx_rate < DESC_RATEMCS8)
    {
        tmp_rate = rx_rate;
    }
    else if (rx_rate >= DESC_RATEMCS8 && rx_rate <= DESC_RATEMCS15)
    {
        tmp_rate = rx_rate - 0x08;
    }
    else if (rx_rate >= DESC_RATEMCS16 && rx_rate <= DESC_RATEMCS23)
    {
        tmp_rate = rx_rate - 0x10;
    }
    else if (rx_rate >= DESC_RATEMCS24 && rx_rate <= DESC_RATEMCS31)
    {
        tmp_rate = rx_rate - 0x18;
    }

    return tmp_rate;
}


#ifdef C2H_ADPT_TXACK_DEBUG
static int rx_txDataRpt_parse(wf_u8 *pdata, wf_u8 len, wf_u16 *seqNum)
{
    int ret;

    *seqNum = *(pdata + 6);

    if (GET_C2H_TX_RPT_RETRY_OVER(pdata) | GET_C2H_TX_RPT_LIFE_TIME_OVER(pdata))
    {
        ret = -1;

    }
    else
    {
        ret = 0;
    }

    return ret;
}
#endif

static int rx_c2h_pkt_hdr_parse(wf_u8 *buf, wf_u16 len, wf_u8 *id, wf_u8 *seq, wf_u8 *plen, wf_u8 **payload)
{
    if (!buf || len > 256 || len < 3)
        return -1;

    *id = C2H_ID(buf);
    *seq = C2H_SEQ(buf);
    *plen = len - 2;
    *payload = C2H_PAYLOAD(buf);

    return 0;
}



static prx_pkt_t rx_recombination(p_que_t p_defrag_que)
{
    wf_u8 order_num = 0;
    wf_u8 *pbuff;
    wf_u8 wlan_hdr_offset;
    p_que_entry_t p_entry;
    p_que_t pdefrag_que = p_defrag_que;
    prx_pkt_t ppkt_first;
    prx_pkt_t ppkt;

    WF_ASSERT(p_defrag_que);

    /* get first defrag pkt */
    queue_remove_head(pdefrag_que, p_entry, p_que_entry_t);
    if (p_entry == NULL)
    {
        return NULL;
    }

    ppkt_first = (prx_pkt_t)p_entry;
    if (order_num != ppkt_first->pkt_info.frag_num)
    {
        free_rx_pkt(ppkt_first, wf_true);
        return NULL;
    }

    while (queue_is_not_empty(pdefrag_que))
    {
        order_num++;  /* must be 1 */

        queue_remove_head(pdefrag_que, p_entry, p_que_entry_t);
        ppkt = (prx_pkt_t)p_entry;

        if (ppkt->pkt_info.frag_num != order_num)
        {
            free_rx_pkt(ppkt, wf_true);
            return NULL;
        }

        wlan_hdr_offset = ppkt->pkt_info.wlan_hdr_len + ppkt->pkt_info.iv_len;

#if 0
        LOG_E("order_num:%d frag_num:%d wlanhdrLen=%d iv_len:%d",
              order_num, ppkt->pkt_info.frag_num, ppkt->pkt_info.wlan_hdr_len, ppkt->pkt_info.iv_len);
#endif

        //pkt_pull(ppkt, wlan_hdr_offset);

        //pkt_trim(ppkt_first, ppkt_first->pkt_info.iv_len);

        pbuff = pkt_put(ppkt_first, ppkt->len);
        if (pbuff == NULL)
        {
            free_rx_pkt(ppkt, wf_true);
            return NULL;
        }

        wf_memcpy(ppkt_first->ptail, ppkt->pdata, ppkt->len);
        ppkt_first->pkt_info.icv_len = ppkt->pkt_info.icv_len;

        free_rx_pkt(ppkt, wf_true);
    }

    free_rx_queue(pdefrag_que, wf_false);
    LOG_I("[rx_recombination]  wlanhdrLen=%d iv_len:%d", ppkt_first->pkt_info.wlan_hdr_len, ppkt_first->pkt_info.iv_len);

    return  ppkt_first;  /* normal return */

}


static prx_pkt_t rx_defrag(prx_pkt_t ppkt)
{
    nic_info_st *pnic_info  = (nic_info_st *)ppkt->p_nic_info;
    prx_pkt_info_t prx_pkt_info = &ppkt->pkt_info;
    p_que_t pdefrag_q = NULL;
    wf_u8 mfrag;
    wf_u8 frag_num;
    prx_pkt_t p_return_pkt = NULL;
    wdn_net_info_st *pwdn;
    p_que_entry_t p_entry;
    rx_info_t *rx_info = pnic_info->rx_info;
    prx_pkt_t ppkt_defrag;

    mfrag = prx_pkt_info->more_frag;
    frag_num = prx_pkt_info->frag_num;
    pwdn = ppkt->wdn_info;

    pdefrag_q = &pwdn->defrag_q;

    queue_remove_head(&rx_info->free_rx_pkt_list, p_entry, p_que_entry_t);
    if (p_entry == NULL)
    {
        free_rx_queue(pdefrag_q, wf_true);
        return NULL;
    }

    ppkt_defrag = (prx_pkt_t)p_entry;
    wf_memcpy(ppkt_defrag, ppkt, sizeof(rx_pkt_t));

    /* not frag pkt , return it */
    if (mfrag == 0)
    {
        if (frag_num == 0)
        {
            /* not frag pkt , return it */
            p_return_pkt = ppkt;
        }
        else if(pwdn->defrag_flag == 1)
        {
            /* the last frag pkt, insert to defrag list, and then defrag to a new pkt */
            LOG_E("[defrag] insert frag:%d", frag_num);
            queue_insert_tail(pdefrag_q, &ppkt_defrag->entry);

            p_return_pkt = rx_recombination(pdefrag_q);

            pwdn->defrag_flag = 0;
        }

    }
    else if (mfrag == 1)
    {
        /* first frag pkt */
        if (frag_num == 0)
        {
            pwdn->defrag_flag = 1;

            if (queue_is_not_empty(pdefrag_q))
            {
                free_rx_queue(pdefrag_q, wf_true);
            }

            LOG_E("[defrag] insert frag:%d", frag_num);
            queue_insert_tail(pdefrag_q, &ppkt_defrag->entry);

            p_return_pkt = NULL;
        }
        else if (pwdn->defrag_flag == 1)
        {
            /* mfrag pkt but not the last one */
            LOG_E("[defrag] insert frag:%d", frag_num);
            queue_insert_tail(pdefrag_q, &ppkt_defrag->entry);

            p_return_pkt = NULL;
        }
    }

    return p_return_pkt;
}

static void rx_parse_qos_ctrl_field(prx_pkt_t prx_pkt)
{
    wf_u8 *pbuf;
    wf_u8 qos_ctrl_offset;
    prx_pkt_info_t ppkt_info = &prx_pkt->pkt_info;
    wf_u16 to_from_ds_n8 = (wf_u8)(GET_HDR_To_From_DS(prx_pkt->pdata) >> 8);;

    if ((GET_HDR_SubTpye(prx_pkt->pdata) & MAC_FRAME_TYPE_QOS_DATA) == MAC_FRAME_TYPE_QOS_DATA)
    {
        ppkt_info->qos_flag = 1;
    }

    ppkt_info->wlan_hdr_len = DATA_FRAME_HDR_SHORT; /* 24 bytes */
    qos_ctrl_offset = DATA_FRAME_HDR_SHORT;

    if (to_from_ds_n8 == 0x03)
    {
        ppkt_info->wlan_hdr_len += ADDRESS4_LEN; /* 24 + 6 = 30 bytes */
        qos_ctrl_offset += ADDRESS4_LEN;
    }

    if (ppkt_info->qos_flag)
    {
        pbuf = prx_pkt->pdata + qos_ctrl_offset;
        ppkt_info->qos_pri = GET_TID(pbuf);
        ppkt_info->ack_policy = GET_Ack_Policy(pbuf);
        ppkt_info->amsdu = GET_Amsdu(pbuf);
        ppkt_info->wlan_hdr_len += QOS_CONTRL_LEN ;  /* 26/32 bytes */
    }
    else
    {
        ppkt_info->qos_pri = 0;
        ppkt_info->ack_policy = 0;
        ppkt_info->amsdu = 0;
    }
}

static int rx_parse_ht_ctrl_field(prx_pkt_t ppkt)
{
    if (GET_HDR_Order(ppkt->pdata))
    {
        ppkt->pkt_info.wlan_hdr_len += HT_CONTRL_LEN ;
    }

    return 0;
}

static int rx_data_pkt_sta2sta(prx_pkt_t prx_pkt)
{
#ifdef CFG_ENABLE_ADHOC_MODE
    wf_bool bmcast;
    nic_info_st *nic_info = prx_pkt->p_nic_info;
    wf_u8 *da =  GetAddr1Ptr(prx_pkt->pdata);
    wf_u8 *sa =  GetAddr2Ptr(prx_pkt->pdata);
    wf_u8 *ra =  GetAddr1Ptr(prx_pkt->pdata);
    wf_u8 *ta =  GetAddr2Ptr(prx_pkt->pdata);
    wf_u8 *bssid = GetAddr3Ptr(prx_pkt->pdata);

#if 0
    LOG_D("da:"WF_MAC_FMT, WF_MAC_ARG(da));
    LOG_D("sa:"WF_MAC_FMT, WF_MAC_ARG(sa));
    LOG_D("ta :"WF_MAC_FMT, WF_MAC_ARG(ta));
    LOG_D("ra :"WF_MAC_FMT, WF_MAC_ARG(ra));
    LOG_D("bssid :"WF_MAC_FMT, WF_MAC_ARG(bssid));
#endif

    if (NIC_INFO_2_WORK_MODE(nic_info) != WF_ADHOC_MODE)
    {
        LOG_E("[%s] not ADHOC mode", __func__);
        return -1;
    }

    bmcast = is_bcast_addr(da);
    if (bmcast == wf_false)
    {
        if(mac_addr_equal(bssid, prx_pkt->wdn_info->bssid) == wf_false)
        {
            //LOG_E("bssid is not match, dropped!! ");
            return -1;
        }


        if(mac_addr_equal(da, nic_to_local_addr(nic_info)) == wf_false)
        {
            //LOG_E("da is not match, dropped!! ");
            //LOG_I("[da]:"WF_MAC_FMT", [ta]:"WF_MAC_FMT, WF_MAC_ARG(da), WF_MAC_ARG(ta));
            return -1;
        }
    }

    /* copy ra ta */
    wf_memcpy(prx_pkt->pkt_info.dst_addr, da, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.src_addr, sa, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.bssid, bssid, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.rx_addr, ra, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.tx_addr, ta, MAC_ADDR_LEN);
#endif
    return 0;
}

static int rx_data_pkt_ap2sta(prx_pkt_t prx_pkt)
{
    wf_bool bmcast;
    nic_info_st *nic_info = prx_pkt->p_nic_info;
    wf_u8 *da =  GetAddr1Ptr(prx_pkt->pdata);
    wf_u8 *sa =  GetAddr3Ptr(prx_pkt->pdata);
    wf_u8 *ra =  GetAddr1Ptr(prx_pkt->pdata);
    wf_u8 *ta =  GetAddr2Ptr(prx_pkt->pdata);
    wf_u8 *bssid = GetAddr2Ptr(prx_pkt->pdata);

    if (NIC_INFO_2_WORK_MODE(nic_info) != WF_INFRA_MODE)
    {
        LOG_E("[%s] not STA mode", __func__);
        return -1;
    }

    bmcast = is_bcast_addr(da);
    if (bmcast == wf_false)
    {
        if(mac_addr_equal(bssid, prx_pkt->wdn_info->bssid) == wf_false)
        {
            //LOG_E("bssid is not match, dropped!! ");
            return -1;
        }


        if(mac_addr_equal(da, nic_to_local_addr(nic_info)) == wf_false)
        {
            //LOG_E("da is not match, dropped!! ");
            //LOG_I("[da]:"WF_MAC_FMT", [ta]:"WF_MAC_FMT, WF_MAC_ARG(da), WF_MAC_ARG(ta));
            return -1;
        }
    }

    /* copy ra ta */
    wf_memcpy(prx_pkt->pkt_info.dst_addr, da, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.src_addr, sa, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.bssid, bssid, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.rx_addr, ra, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.tx_addr, ta, MAC_ADDR_LEN);

    return 0;
}

static int rx_data_pkt_sta2ap(prx_pkt_t prx_pkt)
{
#ifdef CFG_ENABLE_AP_MODE
    nic_info_st *nic_info = prx_pkt->p_nic_info;
    wf_u8 *da = GetAddr3Ptr(prx_pkt->pdata);
    wf_u8 *sa = GetAddr2Ptr(prx_pkt->pdata);
    wf_u8 *ra = GetAddr1Ptr(prx_pkt->pdata);
    wf_u8 *ta = GetAddr2Ptr(prx_pkt->pdata);
    wf_u8 *bssid = GetAddr1Ptr(prx_pkt->pdata);

    /* check mode */
    if (NIC_INFO_2_WORK_MODE(nic_info) != WF_MASTER_MODE)
    {
        //LOG_E("[%s, %d]", __func__, __LINE__);
        return -1;
    }

    /* check bssid */
    if (mac_addr_equal(bssid, nic_to_local_addr(prx_pkt->p_nic_info)) == wf_false)
    {
        LOG_E("in rx_data_pkt_sta2ap, bssid error");
        return -1;
    }

    /* copy address */
    wf_memcpy(prx_pkt->pkt_info.dst_addr, da, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.src_addr, sa, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.rx_addr, ra, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.tx_addr, ta, MAC_ADDR_LEN);
    wf_memcpy(prx_pkt->pkt_info.bssid, bssid, MAC_ADDR_LEN);
#endif
    return 0;
}



static int rx_check_seq_ctrl(prx_pkt_t prx_pkt)
{
    wf_u8 tid;
    prx_pkt_info_t ppkt_info = &prx_pkt->pkt_info;
    wf_u16 seq_ctrl = (ppkt_info->frag_num & 0xf) | ((ppkt_info->seq_num & 0xffff) << 4);


    tid = ppkt_info->qos_pri;
    if (tid > 15)
    {
        LOG_E("in seq_ctrl func, tid error:tid=%d", tid);
        return -1;
    }

    /* todo: use to filter duplicate packet. when receive the first data packet,
    the seq_ctrl_recorder[tid] value is random, and the retry value is effect */
    if (prx_pkt->wdn_info->seq_ctrl_recorder[tid] == seq_ctrl &&
        GET_HDR_Retry(prx_pkt->pdata))
    {
        //LOG_E("retry frame , drop it");
        return -1;
    }

    prx_pkt->wdn_info->seq_ctrl_recorder[tid] = seq_ctrl;
    //LOG_I("data frame  TID[%d], SeqNum[%d]", tid, ppkt_info->seq_num);

    return 0;
}


int rx_check_data_frame_valid(prx_pkt_t prx_pkt)
{
    int ret = 0;
    nic_info_st *nic_info = (nic_info_st *)prx_pkt->p_nic_info;

    if(NULL == nic_info)
    {
        LOG_E("%s, check NULL == nic_info", __func__);
        return -1;
    }
    if (NULL == prx_pkt->wdn_info)
    {
//      LOG_E("for data frame , wdn should not be null");
        return -1;
    }

    switch(NIC_INFO_2_WORK_MODE(nic_info))
    {
        case WF_INFRA_MODE :  //sta
            if(GET_HDR_To_From_DS(prx_pkt->pdata) != FROM_DS)
            {
                LOG_E("to_from_ds field error");
                ret = -1;
                break;
            }

            ret = rx_data_pkt_ap2sta(prx_pkt);
            break;

        case WF_MASTER_MODE : // ap
            if(GET_HDR_To_From_DS(prx_pkt->pdata) != TO_DS)
            {
                LOG_E("to_from_ds field error");
                ret = -1;
                break;
            }

            ret = rx_data_pkt_sta2ap(prx_pkt);
            break;
        case WF_ADHOC_MODE :
            ret = rx_data_pkt_sta2sta(prx_pkt);
            break;
        case WF_AUTO_MODE :
        case WF_REPEAT_MODE :
        case WF_SECOND_MODES :
        case WF_MONITOR_MODE :
        case WF_MESH_MODE :
        default:
            LOG_W("finish the rest check");
            break;
    }

    if (ret != 0)
    {
        //LOG_E("%s, check ra, ta error", __func__);
        return -1;
    }


    ret = rx_check_seq_ctrl(prx_pkt);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}



int rx_check_mngt_frame_valid(prx_pkt_t prx_pkt)
{
    nic_info_st *nic_info = (nic_info_st *)prx_pkt->p_nic_info;
    prx_pkt_info_t ppkt_info = &prx_pkt->pkt_info;
    wf_bool bmcast;
    wf_u8 *pbuf = prx_pkt->pdata;
    wf_u8 *pra = GET_ADDR1(pbuf);

    if(NULL == nic_info)
    {
        LOG_E("check prx_pkt->p_nic_info == NULL ");
        return -1;
    }

    switch(NIC_INFO_2_WORK_MODE(nic_info))
    {
        case WF_INFRA_MODE :  //sta
        case WF_ADHOC_MODE :

            if (ppkt_info->pkt_len > MMPDU_SIZE + MAX_MGNT_HDR_LEN)
            {
                LOG_E("[rx_check_mngt_frame_valid] pkt_len error:%d", ppkt_info->pkt_len);
                return -1;
            }

            bmcast = is_bcast_addr(GET_ADDR1(pbuf));
            if(!is_bcast_addr(pra))
            {
                if (!mac_addr_equal(pra, nic_to_local_addr(nic_info)))
                {
                    return -1;
                }
            }

            wf_memcpy(ppkt_info->src_addr, GET_ADDR2(pbuf), MAC_ADDR_LEN);
            break;

        case WF_MASTER_MODE : // ap

            break;
        case WF_AUTO_MODE :
        case WF_REPEAT_MODE :
        case WF_SECOND_MODES :
        case WF_MONITOR_MODE :
        case WF_MESH_MODE :
        default:
            LOG_W("finish the rest check");
            break;
    }

    return 0;
}


static int rx_process_data_frame(prx_pkt_t ppkt)
{
    int ret = 0;
    nic_info_st *pnic_info = NULL;
    prx_pkt_info_t  prx_pkt_info = &ppkt->pkt_info;

    if(NULL == ppkt)
    {
        return -1;
    }

    pnic_info = (nic_info_st *)ppkt->p_nic_info;
    if(NULL == pnic_info)
    {
        return -1;
    }

    rx_parse_qos_ctrl_field(ppkt);
    rx_parse_ht_ctrl_field(ppkt);
    set_encrypt_algo_num(ppkt, ppkt->wdn_info);
    set_iv_icv_len(ppkt);

    /* decrypt */
    if (GET_HDR_Protected(ppkt->pdata))
    {
        ret = wf_sec_decryptor(ppkt);
        if (ret < 0)
        {
            return -1;
        }
    }

    /* defrag  */
    if ((prx_pkt_info->more_frag == 0) && (prx_pkt_info->frag_num == 0))
    {
        ; /* no defrag frame */
    }
    else
    {
        LOG_I("rx_defrag process [frag_num:%d]", prx_pkt_info->frag_num);
        ppkt = rx_defrag(ppkt);
        ret = -1; // no process , so dropped
    }

    return ret;
}

#ifdef CFG_ENABLE_MONITOR_MODE
static int rx_set_radiotap_hdr(prx_pkt_t ppkt)
{
    struct rx_radiotap_header *prtap_hdr = NULL;
    nic_info_st *pnic_info = ppkt->p_nic_info;
    wf_wlan_mgmt_info_t *pwlan_info = pnic_info->wlan_mgmt_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_u16 rt_len = 8;
    wf_u16 tmp_16bit = 0;
    wf_u8 hdr_buf[64] = { 0 };
    struct rx_pkt_info *pinfo = &ppkt->pkt_info;
    struct sk_buff * pskb = ppkt->pskb;
    wf_u8 * ptr = NULL;
    wf_u8 data_rate[] =
    {
        2, 4, 11, 22,
        12, 18, 24, 36, 48, 72, 93, 108,
        0, 1, 2, 3, 4, 5, 6, 7,
    };

    prtap_hdr = (struct rx_radiotap_header *)&hdr_buf[0];
    prtap_hdr->it_version = PKTHDR_RADIOTAP_VERSION;
    if(pinfo->tsfl)
    {
        wf_u64 tmp_64bit;

        prtap_hdr->it_present |= (1 << WF_RADIOTAP_TSFT);
        tmp_64bit = wf_cpu_to_le64(pinfo->tsfl);
        memcpy(&hdr_buf[rt_len], &tmp_64bit, 8);
        rt_len += 8;
    }

    prtap_hdr->it_present |= (1 << WF_RADIOTAP_FLAGS);

    if ((pinfo->encrypt_algo == 1) || (pinfo->encrypt_algo == 5))
    {
        hdr_buf[rt_len] |= WF_RADIOTAP_F_WEP;
    }

    if (pinfo->frag_num)
    {
        hdr_buf[rt_len] |= WF_RADIOTAP_F_FRAG;
    }

    hdr_buf[rt_len] |= WF_RADIOTAP_F_FCS;

    if (pinfo->crc_err)
    {
        hdr_buf[rt_len] |= WF_RADIOTAP_F_BADFCS;
    }

    if (pinfo->sgi)
    {
        hdr_buf[rt_len] |= 0x80;
    }
    rt_len += 1;

    if (pinfo->rx_rate < 12)
    {
        prtap_hdr->it_present |= (1 << WF_RADIOTAP_RATE);
        hdr_buf[rt_len] = data_rate[pinfo->rx_rate];
    }
    rt_len += 1;

    tmp_16bit = 0;
    prtap_hdr->it_present |= (1 << WF_RADIOTAP_CHANNEL);
    tmp_16bit = wf_ch_2_freq(pcur_network->channel);
    wf_memcpy(&hdr_buf[rt_len], &tmp_16bit, 2);
    rt_len += 2;

    tmp_16bit = 0;
//  if (pHalData->CurrentBandType == 0)
    tmp_16bit |= wf_cpu_to_le16(WF_CHAN_2GHZ);

    if (pinfo->rx_rate < 12)
    {
        if (pinfo->rx_rate < 4)
        {
            tmp_16bit |= wf_cpu_to_le16(WF_CHAN_CCK);
        }
        else
        {
            tmp_16bit |= wf_cpu_to_le16(WF_CHAN_OFDM);
        }
    }
    else
    {
        tmp_16bit |= wf_cpu_to_le16(WF_CHAN_DYN);
    }
    wf_memcpy(&hdr_buf[rt_len], &tmp_16bit, 2);
    rt_len += 2;

    prtap_hdr->it_present |= (1 << WF_RADIOTAP_DBM_ANTSIGNAL);
//  hdr_buf[rt_len] = pinfo->phy_info.RecvSignalPower;
    hdr_buf[rt_len] = 0;
    rt_len += 1;

    prtap_hdr->it_present |= (1 << WF_RADIOTAP_ANTENNA);
    hdr_buf[rt_len] = 0;
    rt_len += 1;

    prtap_hdr->it_present |= (1 << WF_RADIOTAP_RX_FLAGS);
    rt_len += 2;

    if (pinfo->rx_rate >= 12 && pinfo->rx_rate < 20)
    {
        prtap_hdr->it_present |= (1 << WF_RADIOTAP_MCS);
        hdr_buf[rt_len] |= BIT(1);

        hdr_buf[rt_len] |= BIT(0);
        hdr_buf[rt_len + 1] |= (pinfo->bw & 0x03);

        hdr_buf[rt_len] |= BIT(2);
        hdr_buf[rt_len + 1] |= (pinfo->sgi & 0x01) << 2;

        hdr_buf[rt_len] |= BIT(5);
        hdr_buf[rt_len + 1] |= (pinfo->stbc & 0x03) << 5;

        rt_len += 2;

        hdr_buf[rt_len] = data_rate[pinfo->rx_rate];
        rt_len += 1;
    }

    if (skb_headroom(pskb) < rt_len)
    {
        LOG_E("headroom is too small.");
        return -1;
    }

    ptr = skb_push(pskb, rt_len);
    if (ptr)
    {
        prtap_hdr->it_len = wf_cpu_to_le16(rt_len);
        wf_memcpy(ptr, prtap_hdr, rt_len);
    }
    else
    {
        return -1;
    }

    return 0;
}

int rx_process_monitor_frame(prx_pkt_t ppkt)
{
    int ret = 0;
    nic_info_st *pnic_info = NULL;

    if(NULL == ppkt)
    {
        return -1;
    }

    pnic_info = (nic_info_st *)ppkt->p_nic_info;
    if(NULL == pnic_info)
    {
        return -1;
    }

    if (rx_set_radiotap_hdr(ppkt))
    {
        return -1;
    }

    return ret;
}
#endif

#if RX_REORDER_THREAD_EN
wf_s32 wf_rx_reorder_queue_insert(rx_pkt_t *pkt)
{
    rx_info_t *rx_info = NULL;
    rx_reorder_node_t *node = NULL;

    if(NULL == pkt)
    {
        LOG_E("pkt is null");
        return WF_RETURN_FAIL;
    }

    rx_info = pkt->prx_info;
    if(NULL == rx_info)
    {
        LOG_E("rx_info is null");
        return WF_RETURN_FAIL;
    }

    node = wf_kzalloc(sizeof(rx_reorder_node_t));
    if(NULL == node)
    {
        LOG_E("wf_kzalloc for reorder nod failed");
        return WF_RETURN_FAIL;
    }

    wf_memcpy(&node->pkt, pkt, sizeof(rx_pkt_t));
    wf_enque_tail(&node->list, &rx_info->rx_reorder_queue);

    wf_os_api_sema_post(&rx_info->rx_reorder_sema);
    return WF_RETURN_OK;
}

wf_s32 wf_rx_reorder_queue_remove(rx_info_t *rx_info, rx_reorder_node_t **rx_reorder_node)
{
    wf_que_list_t *node            = NULL;
    rx_reorder_node_t *tmp_node    = NULL;

    if (wf_os_api_sema_wait(&rx_info->rx_reorder_sema))
    {
        return -2;
    }
    node = wf_deque_head(&rx_info->rx_reorder_queue);
    if(NULL == node)
    {
        return -1;
    }

    tmp_node = WF_CONTAINER_OF(node, rx_reorder_node_t, list);
    *rx_reorder_node = tmp_node;
    return 0;
};

static void rx_reorder_thread_handle(nic_info_st *nic_info)
{
    rx_reorder_node_t *rx_reorder_node  = NULL;
    rx_info_t *rx_info                  = NULL;
    wf_s32 ret = 0;

    //wf_os_api_thread_affinity(DEFAULT_CPU_ID);

    while(1)
    {
        rx_info = nic_info->rx_info;
        if(NULL == rx_info)
        {
            continue;
        }
        wf_lock_lock(&rx_info->op_lock);
        ret = wf_rx_reorder_queue_remove(rx_info, &rx_reorder_node);
        if(ret)
        {
            if(nic_info->is_driver_stopped == wf_true) || (nic_info->is_surprise_removed == wf_true)
            {
                break;
            }
            //LOG_I("[%s, %d] exe", __func__, __LINE__);
            continue;
        }

        if(rx_reorder_node)
        {
            wf_rx_data_reorder_core(&rx_reorder_node->pkt);
            wf_kfree(rx_reorder_node);
            rx_reorder_node = NULL;
        }
        wf_lock_unlock(&rx_info->op_lock);
    }

    while(wf_os_api_thread_wait_stop(rx_info->rx_reorder_tid) == wf_false)
    {
        wf_msleep(10);
    }

    wf_os_api_thread_exit(rx_info->rx_reorder_tid);
}

#endif

/**************************************** external func *******************************************/


int wf_rx_init(nic_info_st *nic_info)
{
    int i;
    prx_pkt_t ppkt;
    prx_info_t prx_info;
    p_que_t pfree_que;
    prx_info = wf_kzalloc(sizeof(rx_info_t));

    LOG_I("rx_init init");

    if (prx_info == NULL)
    {
        LOG_E("in rx_init, alloc rx_info buffer error");
        return -1;
    }
    nic_info->rx_info = prx_info;

    pfree_que = &prx_info->free_rx_pkt_list;

    prx_info->p_nic_info = nic_info;

    queue_initialize(pfree_que);
    queue_initialize(&prx_info->recv_rx_pkt_list);
    queue_initialize(&prx_info->rx_mgmt_frame_defrag_list);

    prx_info->prx_pkt_buf_alloc = wf_kzalloc(sizeof(rx_pkt_t) * MAX_PKT_NUM);
    if (prx_info->prx_pkt_buf_alloc == NULL)
    {
        LOG_E("prx_pkt_buf_alloc ==null");
        wf_kfree(prx_info->p_nic_info);
        return -1;
    }
    ppkt = (prx_pkt_t)prx_info->prx_pkt_buf_alloc;
    for ( i = 0 ; i < MAX_PKT_NUM ; i++ )
    {
        queue_insert_tail(pfree_que, &ppkt[i].entry);
    }
    wf_rx_action_ba_ctl_init(nic_info);
#if RX_REORDER_THREAD_EN
    wf_que_init(&prx_info->rx_reorder_queue, WF_LOCK_TYPE_IRQ);
    wf_lock_init(&prx_info->op_lock, WF_LOCK_TYPE_NONE);
    wf_os_api_sema_init(&prx_info->rx_reorder_sema, 0);
    sprintf(prx_info->rx_reorder_name,
            nic_info->virNic ? "rx_reorder:vir%d_%d" : "rx_reorder:wlan%d_%d",
            nic_info->hif_node_id, nic_info->ndev_id);
    if (NULL ==(prx_info->rx_reorder_tid = wf_os_api_thread_create(prx_info->rx_reorder_tid, prx_info->rx_reorder_name, rx_reorder_thread_handle, nic_info)))
    {
        LOG_E("[wf_mlme_init] create mlme thread failed");
        return -1;
    }
    else
    {
        wf_os_api_thread_wakeup(prx_info->rx_reorder_tid);
    }
#endif
    return 0;
}

int wf_rx_term(nic_info_st *nic_info)
{
    rx_info_t *rx_info = nic_info->rx_info;

    LOG_D("[wf_rx_term] start");

    if (rx_info)
    {
#if RX_REORDER_THREAD_EN
        if (rx_info->rx_reorder_tid)
        {
            wf_lock_term(&rx_info->op_lock);
            wf_os_api_sema_post(&rx_info->rx_reorder_sema);

            wf_os_api_thread_destory(rx_info->rx_reorder_tid);
            rx_info->rx_reorder_tid = NULL;
        }
#endif
        wf_rx_action_ba_ctl_deinit(nic_info);
        if (rx_info->prx_pkt_buf_alloc)
            wf_kfree(rx_info->prx_pkt_buf_alloc);

        wf_kfree(rx_info);
        nic_info->rx_info = NULL;
    }

    LOG_D("[wf_rx_term] end");

    return 0;
}

/*
common process
include check valid, header process
*/
int wf_rx_common_process(prx_pkt_t ppkt)
{
    int ret = 0;
    wf_u32 frame_type;
    nic_info_st *pnic_info = ppkt->p_nic_info;
    rx_info_t *rx_info = pnic_info->rx_info;
    mlme_info_t *mlme_info = pnic_info->mlme_info;
    wdn_net_info_st *pwdn_info;

    /* retrive wdn_info */
    pwdn_info = wf_wdn_find_info(ppkt->p_nic_info, get_ta(ppkt->pdata));
    ppkt->wdn_info = pwdn_info;

    if(pwdn_info != NULL)
    {
        /* rx packet statistics used for connection alive check */
        pwdn_info->rx_pkt_stat++;
    }

#ifdef CONFIG_LPS
    if(!MacAddr_isBcst(get_da(ppkt->pdata)) && (!IS_MCAST(get_da(ppkt->pdata))))
    {
        mlme_info->link_info.num_rx_unicast_ok_in_period++;
    }
#endif

#ifdef CFG_ENABLE_MONITOR_MODE
    if(wf_local_cfg_get_work_mode(pnic_info) == WF_MONITOR_MODE)
    {
        ret = rx_process_monitor_frame(ppkt);
        return ret;
    }
#endif

    frame_type = GET_HDR_Type(ppkt->pdata);
    switch(frame_type)
    {
        case MAC_FRAME_TYPE_DATA :
            ret = rx_check_data_frame_valid(ppkt);
            if (ret < 0)
            {
                break;
            }

            rx_info->rx_data_pkt++;

            ret = rx_process_data_frame(ppkt);
            if (ret < 0)
            {
                rx_info->rx_drop++;
                break;
            }

            mlme_info->link_info.num_rx_ok_in_period++;
            rx_info->rx_pkts++;
            break;

        case MAC_FRAME_TYPE_MGT:

            ret = rx_check_mngt_frame_valid(ppkt);
            if(ret < 0)
            {
                break;
            }

            ret = wf_wlan_mgmt_rx_frame(ppkt);
            if (ret > 0)
            {
                rx_info->rx_mgnt_pkt++;
            }

            break;

        case MAC_FRAME_TYPE_CTRL:
            LOG_I("ctrl frame <len:%d>, dropped!!", ppkt->len);
            break;

        default :
            LOG_E("Error Frame Type!! <%d> <len:%d>", frame_type, ppkt->len);
            break;

    }
    return ret;
}

void phydm_print_rate(wf_u8 rate)
{
    wf_u8       legacy_table[12] = {1, 2, 5, 11, 6, 9, 12, 18, 24, 36, 48, 54};
    wf_u8       rate_idx = rate & 0x7f; /*remove bit7 SGI*/

    if (rate_idx >= ODM_RATEMCS8)
    {
        LOG_I("unsupport rate");
    }
    else
    {
        if ((rate_idx >= ODM_RATE1M)&&(rate_idx < ODM_RATE6M))
        {
            LOG_I("[CCK] %dM", legacy_table[rate_idx]);
        }
        else if ((rate_idx >= ODM_RATE6M)&&(rate_idx < ODM_RATEMCS0))
        {
            LOG_I("[OFDM] %dM", legacy_table[rate_idx]);
        }
        else
        {
            LOG_I("[MCS] index:%d", rate_idx - ODM_RATEMCS0);
        }
    }
}


void rx_c2h_ra_report_handler(wf_u8 *cmd_buf, wf_u8 cmd_len)
{
    wf_u8   macid = cmd_buf[1];
    wf_u8   rate = cmd_buf[0];
    wf_u8   curr_ra_ratio = 0xff;
    wf_u8   curr_bw = 0xff;

    if (cmd_len >= 7)
    {
        curr_ra_ratio = cmd_buf[5];
        curr_bw = cmd_buf[6];
        LOG_I("RA retry ratio: [%d]:", curr_ra_ratio);
    }

    if (cmd_buf[3] != 0)
    {
        if (cmd_buf[3] == 0xff)
        {
            LOG_I("FW Level: Fix rate[%d]:", macid);
        }
        else if (cmd_buf[3] == 1)
        {
            LOG_I("Try Success[%d]:", macid);
        }
        else if (cmd_buf[3] == 2)
        {
            LOG_I("Try Fail & Try Again[%d]:", macid);
        }
        else if (cmd_buf[3] == 3)
        {
            LOG_I("rate Back[%d]:", macid);
        }
        else if (cmd_buf[3] == 4)
        {
            LOG_I("start rate by RSSI[%d]:", macid);
        }
        else if (cmd_buf[3] == 5)
        {
            LOG_I("Try rate[%d]:", macid);
        }
    }

    LOG_I("Tx rate Update[%d]:", macid);
    phydm_print_rate(rate);

}



int wf_rx_notice_process(wf_u8 *pbuf, wf_u16 skb_len)
{
    wf_u8 a[8] = {0xe6, 0xb4, 0x04, 0x0b, 0x13, 0x08, 0xdf, 0x3e};
    int ret, i, c2h_len;
#ifdef C2H_ADPT_TXACK_DEBUG
    wf_u16 seqNum;
    int tx_ret;
#endif
    wf_u8 id;
    wf_u8 seq;
    wf_u8 plen;
    wf_u8 *payload;
    wf_u8 changeBuf[256];

    c2h_len = skb_len - RXDESC_SIZE;

    if ( c2h_len > 256 || c2h_len < 3)
    {
        return -1;
    }

    for (i = 0; i < c2h_len; i++)
    {
        changeBuf[i] = (pbuf+RXDESC_SIZE)[i]^a[i%8];
    }

    ret = rx_c2h_pkt_hdr_parse(changeBuf, c2h_len, &id, &seq, &plen, &payload);

    if (ret == 0)
    {
        if (id== 0x3)
        {
#ifdef C2H_ADPT_TXACK_DEBUG
            tx_ret = rx_txDataRpt_parse(payload, plen, &seqNum);
            if (tx_ret == 0)
            {
                LOG_D("[c2h] tx data[%d] ack report success !!", seqNum);
            }
            else
            {
                LOG_E("[c2h] tx data[%d] ack report failed !!", seqNum);
            }
#endif
        }
        else if (id == 0x10)
        {
            LOG_I("[c2h] chbw switch complete !!");
        }
        else if (id == 0x0c)
        {
#ifdef C2H_ADPT_SPEED_DEBUG
            LOG_D("*************************");
            rx_c2h_ra_report_handler(payload, plen);
            LOG_D("*************************");
#endif
        }
        else
        {
            LOG_D("[c2h] c2h id = %d!!", id);
        }
    }

    return 0;
}

#ifdef CONFIG_RICHV200
wf_inline int wf_rx_cmd_check(wf_u8 *pbuf, wf_u16 skb_len)
{
    wf_u16 pkt_len;
    struct rxd_detail_new *prxd = NULL;
    prxd = (struct rxd_detail_new *)pbuf;

    pkt_len = RXDESC_SIZE + prxd->drvinfo_size * 8 + prxd->pkt_len - 8;

    if (pkt_len != skb_len)
    {
        return -1;
    }

    return 0;
}
#endif

wf_inline int wf_rx_data_len_check(nic_info_st *pnic_info, wf_u8 *pbuf, wf_u16 skb_len)
{
    int ret;
    wf_u16 pkt_len;
    rx_info_t *rx_info = NULL;
    //int i = 0;
#ifdef CONFIG_MP_MODE
    wf_mp_info_st *mp_info = pnic_info->mp_info;
#endif
#ifdef CONFIG_RICHV200
    struct rxd_detail_new *prxd = NULL;
    prxd = (struct rxd_detail_new *)pbuf;
#else
    struct rxd_detail_org *prxd = NULL;
    prxd = (struct rxd_detail_org *)pbuf;
#endif
//    LOG_D("pkt_len:%x", prxd->pkt_len);


    if ((pnic_info == NULL) || (pbuf == NULL))
    {
        return -1;
    }
    else
    {
        rx_info = pnic_info->rx_info;
    }

    if (rx_info == NULL)
    {
        return -1;
    }
    //for(;i < 200;i++)
    //{
    //LOG_D("%x", *(pbuf + i));
    //}
    //LOG_W("end!!!!!!!!!!!!!!!!!!!!!!!!");
    // test = (void *)prxd;
    //for(i = 0;i < sizeof(struct rxd_detail_org);i++)
    //{
    //      LOG_D("0x%x", *(test+i));
    //}

    if (prxd->crc32 == 1)
    {
        rx_info->rx_crcerr_pkt++;

#ifdef  CONFIG_MP_MODE
        if(mp_info == NULL)
        {
            return -1;
        }

        mp_info->rx_crcerrpktcount++;
#else
        LOG_E("crc check error !!");
#endif

        return -1;
    }

#ifdef CONFIG_RICHV200
    if(prxd->notice == 1)
#else
    if(prxd->rpt_sel == 1)
#endif
    {
        pkt_len = RXDESC_SIZE + prxd->pkt_len;
        if (pkt_len == skb_len)
        {
            ret = wf_rx_notice_process(pbuf, skb_len);
        }
        else
        {
            LOG_E("c2h frame length error!!");
        }

        return -1;
    }
    else
    {
#ifdef CONFIG_RICHV200
        pkt_len = RXDESC_SIZE + prxd->drvinfo_size * 8 + prxd->pkt_len;
        if (wf_rx_data_type(pbuf) != WF_PKT_TYPE_FRAME)
        {
            pkt_len -= 8;
        }
#else
        pkt_len = RXDESC_SIZE + DRVINFO_SZ * 8 + prxd->shift + prxd->pkt_len;
#endif
    }

    if (pkt_len > skb_len)
    {
        LOG_E("rxd[0]:0x%08x   rxd[1]:0x%08x   prxd->pkt_len:%d", ((int*)prxd)[0], ((int*)prxd)[1], prxd->pkt_len);
        LOG_E("wf_rx_data_len_check error !! pkt_len:%d, skb_len:%d", pkt_len, skb_len);
        return -1;
    }

    return pkt_len;
}

#ifdef CONFIG_RICHV300
wf_inline static wf_bool wf_rxdesc_chksum(wf_u8 *prx_desc)
{
    wf_u16 *usPtr = (wf_u16 *) prx_desc;
    wf_u32 index;
    wf_u16 checksum;
    wf_u16 checksum_calc = 0;

    for (index = 0; index < 11; index++)
        checksum_calc ^= wf_le16_to_cpu(*(usPtr + index));

    checksum = wf_le_bits_to_u32(prx_desc + 20, 16, 16);
    if(checksum == checksum_calc)
    {
        return wf_true;
    }
    else
    {
        LOG_E("rxd checksum error, not a valid rxd, my checksum: 0x%04x, recv checksum:0x%04x", checksum_calc, checksum);
        return wf_false;
    }
}
#endif

wf_inline wf_u16 wf_rx_get_pkt_len_and_check_valid(wf_u8 *buf, wf_u16 remain, wf_bool *valid, wf_bool *notice)
{
    wf_u16 pkt_len;
#ifdef CONFIG_RICHV200
    struct rxd_detail_new *prxd = (struct rxd_detail_new *)buf;
#else
    struct rxd_detail_org *prxd = (struct rxd_detail_org *)buf;
#endif

#ifdef CONFIG_RICHV300
    if(wf_false == wf_rxdesc_chksum(buf))
    {
        *valid = wf_false;
        return 0;
    }
#endif

#ifdef CONFIG_RICHV200
    pkt_len = RXDESC_SIZE + prxd->drvinfo_size * 8 + prxd->pkt_len;
    if ((prxd->pkt_len == 0) || (prxd->drvinfo_size != 4) ||
        (prxd->cmd_index != 0) || (prxd->crc32 == 1) || (pkt_len > remain))
#else
    pkt_len = RXDESC_SIZE+DRVINFO_SZ * 8+prxd->pkt_len;
    if ((prxd->pkt_len == 0) || (prxd->crc32 == 1) || (pkt_len > remain))
#endif
    {
        *valid = wf_false;
    }
    else
    {
#ifdef CONFIG_RICHV200
        if(prxd->notice == 1)
#else
        if(prxd->rpt_sel == 1)
#endif
        {
            *notice = wf_true;
            *valid = wf_false;

            LOG_E(" c2h is filter, [rpt_sel]:%d, [notice]:%d, need check error!!", prxd->rpt_sel, prxd->notice == 1);
        }
        else
        {
            *valid = wf_true;
            *notice = wf_true;
        }
    }

    return pkt_len;
}



wf_inline PKT_TYPE_T wf_rx_data_type(wf_u8 *pbuf)
{
#ifdef CONFIG_RICHV200
    wf_u8 u8Value;

    u8Value = wf_le_u8_read(pbuf);

    return (PKT_TYPE_T)(u8Value & 0x03);
#else
    return WF_PKT_TYPE_FRAME;
#endif
}

void wf_rx_rxd_prase(wf_u8 *pbuf, struct rx_pkt *prx_pkt)
{
#ifdef CONFIG_RICHV200
    {
        struct rx_pkt_info *pinfo = &prx_pkt->pkt_info;
        struct rxd_detail_new *prxd = (struct rxd_detail_new *)pbuf;
        wf_memcpy(prx_pkt->rxd_raw_buf, pbuf, RXDESC_SIZE);

        pinfo->seq_num = prxd->seq;
        pinfo->pkt_len = prxd->pkt_len;
        pinfo->amsdu = prxd->amsdu;
        pinfo->qos_flag = prxd->qos;
        pinfo->more_data = prxd->more_data;
        pinfo->frag_num = prxd->frag;
        pinfo->more_frag = prxd->more_frag;
        pinfo->encrypt_algo = prxd->encrypt_algo;
        pinfo->usb_agg_pktnum = prxd->usb_agg_pktnum;
        pinfo->phy_status = prxd->phy_status;
        pinfo->hif_hdr_len = RXD_SIZE + prxd->drvinfo_size * 8;
        pinfo->qos_pri = prxd->tid;
        pinfo->rx_rate = calc_rx_rate(prxd->rx_rate);
        //LOG_I("seq_num:%d", pinfo->seq_num);
    }
#else
    nic_info_st *nic_info = prx_pkt->p_nic_info;

    if(NIC_USB == nic_info->nic_type)
    {
        struct rx_pkt_info *pinfo = &prx_pkt->pkt_info;
        struct rxd_detail_org *prxd = (struct rxd_detail_org *)pbuf;
        wf_memcpy(prx_pkt->rxd_raw_buf, pbuf, RXDESC_SIZE);

        pinfo->crc_err = prxd->crc32;
        pinfo->icv_err = 0;
        pinfo->seq_num = prxd->seq;
        pinfo->pkt_len = prxd->pkt_len;
        pinfo->amsdu = prxd->amsdu;
        pinfo->qos_flag = prxd->qos;
        pinfo->more_data = prxd->more_data;
        pinfo->frag_num = prxd->frag;
        pinfo->more_frag = prxd->more_frag;
        pinfo->encrypt_algo = prxd->encrypt_algo;
        pinfo->usb_agg_pktnum = prxd->usb_agg_pktnum;
        pinfo->phy_status = prxd->phy_status;
        pinfo->hif_hdr_len = RXD_SIZE + 32 + 0;
        pinfo->qos_pri = prxd->tid;
        pinfo->rx_rate = calc_rx_rate(prxd->rx_rate);
        if( pinfo->more_frag || pinfo->frag_num)
        {
            LOG_W("defragment frame");
        }

    }
    else if(NIC_SDIO == nic_info->nic_type)
    {
        struct rx_pkt_info *pinfo = &prx_pkt->pkt_info;
        SDIO_RXD_STRUCT *prxd = (SDIO_RXD_STRUCT *)pbuf;

        pinfo->pkt_type = prxd->RptSel ? 4 : 0;
        pinfo->seq_num = prxd->Seq;
        pinfo->pkt_len = prxd->PktLen;
        pinfo->amsdu = prxd->Amsdu;
        pinfo->qos_flag = prxd->Qos;
        pinfo->more_data = prxd->MoreData;
        pinfo->frag_num = prxd->Frag;
        pinfo->more_frag = prxd->MoreFrag;
        pinfo->phy_status = prxd->PhyStatus;
        pinfo->hif_hdr_len = RXD_SIZE + 32 + 0;
        pinfo->qos_pri = prxd->Tid;
        pinfo->rx_rate = calc_rx_rate(prxd->RateBit);
    }
#endif
}

int wf_rx_action_ba_ctl_init(nic_info_st *nic_info)
{
    wf_u8 tid = 0;
    recv_ba_ctrl_st *ba_ctl = NULL;
    rx_reorder_queue_st *order_node = NULL;
    rx_info_t *rx_info = NULL;
    int i = 0;

    if(NULL == nic_info)
    {
        return WF_RETURN_FAIL;
    }

    rx_info = nic_info->rx_info;
    for (tid  = 0; tid  < TID_NUM; tid++)
    {
        ba_ctl                  = &rx_info->ba_ctl[tid];
        ba_ctl->enable          = wf_false;
        ba_ctl->indicate_seq    = 0xffff;
        ba_ctl->wend_b          = 0xffff;
        ba_ctl->wsize_b         = 64;/* max_ampdu_sz; */ /* ex. 32(kbytes) -> wsize_b = 32 */
        ba_ctl->ampdu_size      = 0xff;
        ba_ctl->nic_node        = nic_info;
        ba_ctl->timer_start     = wf_false;
        ba_ctl->drop_pkts       = 0;
        ba_ctl->upload_func     = NULL;
        wf_que_init(&ba_ctl->pending_reorder_queue, WF_LOCK_TYPE_NONE);
        wf_que_init(&ba_ctl->free_order_queue, WF_LOCK_TYPE_NONE);
        wf_lock_init(&ba_ctl->pending_get_de_queue_lock, WF_LOCK_TYPE_BH);

        //wf_os_api_timer_reg(&ba_ctl->reordering_ctrl_timer, (void *)rx_reorder_timeout_handle, ba_ctl);
        wf_os_api_timer_reg(&ba_ctl->reordering_ctrl_timer, rx_reorder_timeout_handle, &ba_ctl->reordering_ctrl_timer);

        for(i = 0; i < BA_REORDER_QUEUE_NUM; i++)
        {
            order_node = wf_kzalloc(sizeof(rx_reorder_queue_st));
            if(NULL == order_node)
            {
                LOG_E("[%s] wf_kzalloc failed", __func__);
                break;
            }
            order_node->pskb = NULL;
            wf_enque_tail(&order_node->list, &ba_ctl->free_order_queue);

        }

    }

    return WF_RETURN_OK;
}



wf_s32 rx_free_reorder_empty(recv_ba_ctrl_st *ba_ctl)
{
    wf_que_t *queue_head     = NULL;
    wf_s32 ret = 0;
    if(NULL == ba_ctl)
    {
        return 0;
    }

    queue_head = &ba_ctl->free_order_queue;

    ret =  wf_que_is_empty(queue_head);
    return ret;
}

rx_reorder_queue_st *rx_free_reorder_dequeue(recv_ba_ctrl_st *ba_ctl)
{
    wf_que_t *queue_head = NULL;
    wf_list_t *phead       = NULL;
    wf_list_t *plist       = NULL;
    rx_reorder_queue_st *tmp = NULL;

    if(NULL == ba_ctl)
    {
        return NULL;
    }


    if(rx_free_reorder_empty(ba_ctl))
    {
        return NULL;
    }

    queue_head = &ba_ctl->free_order_queue;
    wf_lock_lock(&queue_head->lock);
    phead = wf_que_list_head(queue_head);
    plist = wf_list_next(phead);

    tmp = WF_CONTAINER_OF(plist, rx_reorder_queue_st, list);
    if(tmp)
    {
        queue_head->cnt--;
        wf_list_delete(plist);
    }
    wf_lock_unlock(&queue_head->lock);
    return tmp;
}

int rx_free_reorder_enqueue(recv_ba_ctrl_st *ba_ctl, rx_reorder_queue_st *node)
{
    wf_que_t *queue_head = NULL;
    wf_list_t *phead       = NULL;
    wf_list_t *plist       = NULL;

    if(NULL == ba_ctl || NULL == node)
    {
        return -1;
    }

    queue_head = &ba_ctl->free_order_queue;
    wf_lock_lock(&queue_head->lock);
    phead = wf_que_list_head(queue_head);
    plist = wf_list_next(phead);

    queue_head->cnt--;
    wf_list_insert_tail(&node->list, plist);
    wf_lock_unlock(&queue_head->lock);

    return 0;
}

void rx_do_update_expect_seq(wf_u16 seq_num, recv_ba_ctrl_st   *ba_order)
{
    if(NULL == ba_order)
    {
        return ;
    }
    ba_order->indicate_seq = (seq_num + 1) & 0xFFF;
    //ba_order->wend_b = (ba_order->indicate_seq + ba_order->wsize_b - 1) & 0xFFF;
    wf_timer_set(&ba_order->expect_time_start, 0);
}

int rx_pending_reorder_is_empty(recv_ba_ctrl_st   *ba_order)
{
    wf_list_t  *phead = NULL;
    wf_que_t *queue_head  = NULL;
    int ret = 0;
    if(NULL == ba_order)
    {
        return wf_true;
    }
    queue_head  = &ba_order->pending_reorder_queue;

    wf_lock_lock(&queue_head->lock);
    phead = wf_que_list_head(queue_head);
    ret =  wf_list_is_empty(phead);
    wf_lock_unlock(&queue_head->lock);

    return ret;
}
wf_s32 rx_pending_reorder_enqueue(wf_u16 current_seq, void *pskb, recv_ba_ctrl_st   *ba_order)
{
    wf_list_t  *phead = NULL;
    wf_list_t  *pos = NULL;
    rx_reorder_queue_st *pprev_pkt = NULL;
    rx_reorder_queue_st *new_pkt   = NULL;

    wf_u16 seq_num = 0;
    wf_s32 find_flag = 0;
    wf_que_t *queue_head  = NULL;

    if(NULL == ba_order || NULL == pskb )
    {
        LOG_E("[%s] ba_order or pskb is null", __func__);
        return REORDER_DROP;
    }

    queue_head  = &ba_order->pending_reorder_queue;
    wf_lock_lock(&queue_head->lock);
    phead = wf_que_list_head(queue_head);
    wf_list_for_each_prev(pos, phead)
    {
        pprev_pkt = WF_CONTAINER_OF(pos, rx_reorder_queue_st, list);
        if(pprev_pkt)
        {
            seq_num = pprev_pkt->seq_num;
            //LOG_I("[%s] cur seq:%d, seq:%d", __func__, prx_pkt_info->seq_num , seq_num);

            if (SN_EQUAL(seq_num, current_seq))//dup
            {
                ba_order->drop_pkts++;
                LOG_W("[%s]: dup the packet, seq:%d", __func__, current_seq);
                wf_lock_unlock(&queue_head->lock);
                return -1;
            }
            else if (SN_LESS(current_seq, seq_num))//continue
            {

            }
            else //revert
            {
                find_flag = 1;
                break;

            }
        }
        pprev_pkt = NULL;
    }
    wf_lock_unlock(&queue_head->lock);

    new_pkt = rx_free_reorder_dequeue(ba_order);
    while(NULL == new_pkt)
    {
        LOG_W("waite for rx_free_reorder_dequeue");
        //wf_msleep(ba_order->wait_timeout);
        new_pkt = rx_free_reorder_dequeue(ba_order);
    }

    new_pkt->seq_num = current_seq;
    new_pkt->pskb = pskb;

    wf_lock_lock(&queue_head->lock);
    queue_head->cnt++;
    if(0 == find_flag )
    {
        //LOG_I("insert <p:%d> seq_num:%d [%d, %d]", new_pkt->qos_pri, new_pkt->seq_num, ba_order->indicate_seq, ba_order->wend_b);
        wf_list_insert_head(&new_pkt->list, phead);
    }
    else
    {
        //LOG_I("insert <p:%d> seq_num:%d prev <p:%d> seq_num:%d [%d, %d]", new_pkt->qos_pri, new_pkt->seq_num, pprev_pkt->qos_pri, pprev_pkt->seq_num, ba_order->indicate_seq, ba_order->wend_b);
        wf_list_insert_head(&new_pkt->list, &pprev_pkt->list);
    }

    wf_lock_unlock(&queue_head->lock);

    return 0;

}

rx_reorder_queue_st * rx_pending_reorder_dequeue(recv_ba_ctrl_st   *ba_order)
{
    wf_list_t *phead = NULL;
    wf_list_t *plist = NULL;
    wf_que_t *queue_head =NULL;
    rx_reorder_queue_st *tmp = NULL;

    if(NULL == ba_order)
    {
        return NULL;
    }

    if(rx_pending_reorder_is_empty(ba_order))
    {
        return NULL;
    }

    queue_head = &ba_order->pending_reorder_queue;

    wf_lock_lock(&queue_head->lock);
    phead = wf_que_list_head(queue_head);
    plist = wf_list_next(phead);

    tmp = WF_CONTAINER_OF(plist, rx_reorder_queue_st, list);

    if(tmp)
    {
        queue_head->cnt--;
        wf_list_delete(plist);
    }
    wf_lock_unlock(&queue_head->lock);
    return tmp;
}
rx_reorder_queue_st *rx_pending_reorder_getqueue(recv_ba_ctrl_st   *ba_order)
{
    wf_list_t *phead = NULL;
    wf_list_t *plist = NULL;
    wf_que_t *queue_head = NULL;

    if(NULL == ba_order)
    {
        return NULL;
    }


    if(rx_pending_reorder_is_empty(ba_order))
    {
        return NULL;
    }

    queue_head = &ba_order->pending_reorder_queue;
    wf_lock_lock(&queue_head->lock);
    phead = wf_que_list_head(queue_head);
    plist = wf_list_next(phead);
    wf_lock_unlock(&queue_head->lock);

    return WF_CONTAINER_OF(plist, rx_reorder_queue_st, list);
}

int rx_pending_reorder_get_cnt(recv_ba_ctrl_st   *ba_order)
{
    wf_que_t *queue_head = NULL;
    int ret = 0;
    if(NULL == ba_order)
    {
        return -1;
    }

    queue_head = &ba_order->pending_reorder_queue;
    wf_lock_lock(&queue_head->lock);

    ret = queue_head->cnt;
    wf_lock_unlock(&queue_head->lock);

    return ret;
}


int rx_do_chk_expect_seq(wf_u16 seq_num, recv_ba_ctrl_st   *ba_order)
{
    if(NULL == ba_order)
    {
        return REORDER_DROP;
    }

    if (ba_order->indicate_seq == 0xFFFF)
    {
        ba_order->indicate_seq = seq_num;
        wf_timer_set(&ba_order->expect_time_start, 0);
    }

    ba_order->wend_b = (ba_order->indicate_seq + ba_order->wsize_b - 1) & 0xFFF;

    //LOG_I("[%s]: current seq is %d", __func__, seq_num);
    if (SN_EQUAL(seq_num, ba_order->indicate_seq))
    {
        ba_order->indicate_seq = (ba_order->indicate_seq + 1) & 0xFFF;
        wf_timer_set(&ba_order->expect_time_start, 0);
    }
    else if (SN_LESS(seq_num, ba_order->indicate_seq))//drop
    {
        //LOG_I("seq:%d indicate_seq:%d", seq_num, ba_order->indicate_seq);
        ba_order->drop_pkts++;
        return REORDER_DROP;
    }
    else if (SN_LESS(ba_order->wend_b, seq_num))
    {
        if (seq_num >= (ba_order->wsize_b - 1))
        {
            ba_order->indicate_seq = seq_num + 1 - ba_order->wsize_b;
            //LOG_I("[1] recv_seq:%d  update indicate_seq :%d", seq_num, ba_order->indicate_seq);
        }
        else
        {
            ba_order->indicate_seq = 0xFFF - (ba_order->wsize_b - (seq_num + 1)) + 1;
            //LOG_I("[2] recv_seq:%d update indicate_seq :%d", seq_num, ba_order->indicate_seq);
        }
        wf_timer_set(&ba_order->expect_time_start, 0);
    }

    return REORDER_ENQUE;
}

int wf_rx_action_ba_ctl_deinit(nic_info_st *nic_info)
{
    wf_u8 tid                       = 0;
    wf_list_t *pos                  = NULL;
    wf_list_t *next                 = NULL;
    wf_list_t *phead                = NULL;
    recv_ba_ctrl_st *ba_ctl         = NULL;
    rx_info_t       *rx_info        = NULL;
    rx_reorder_queue_st *order_node = NULL;
    wf_que_t   *queue_head          = NULL;

    if(NULL == nic_info)
    {
        return -1;
    }

    rx_info = nic_info->rx_info;

    LOG_I("[%s] handle", __func__);
    for (tid  = 0; tid  < TID_NUM; tid++)
    {
        ba_ctl                  = &rx_info->ba_ctl[tid];
        if(NULL == ba_ctl)
        {
            continue;
        }
        wf_lock_lock(&ba_ctl->pending_get_de_queue_lock);
        if(wf_false == rx_pending_reorder_is_empty(ba_ctl))
        {
            LOG_I("start free pending_reorder_queue");
            while(1)
            {
                order_node = rx_pending_reorder_dequeue(ba_ctl);
                if(NULL == order_node)
                {
                    break;
                }
                if(NULL == order_node->pskb)
                {
                    ba_ctl->free_skb(nic_info, order_node->pskb);
                    order_node->pskb = NULL;
                }
                wf_kfree(order_node);
                order_node = NULL;
            }
        }

        if(rx_free_reorder_empty(ba_ctl))
        {
            wf_lock_unlock(&ba_ctl->pending_get_de_queue_lock);
            continue;
        }

        LOG_I("[%s, %d]", __func__, __LINE__);

        queue_head = &ba_ctl->free_order_queue;
        phead = wf_que_list_head(queue_head);
        wf_lock_lock(&queue_head->lock);
        wf_list_for_each_safe(pos, next, phead)
        {
            order_node = wf_list_entry(pos, rx_reorder_queue_st, list);
            if(order_node)
            {
                wf_kfree(order_node);
                order_node = NULL;
            }
            pos = NULL;
        }
        wf_lock_unlock(&queue_head->lock);
        wf_lock_unlock(&ba_ctl->pending_get_de_queue_lock);

    }

    return WF_RETURN_OK;
}

wf_s32 wf_rx_ba_reinit(nic_info_st *nic_info, wf_u8 tid)
{
    recv_ba_ctrl_st *ba_ctl         = NULL;
    rx_info_t       *rx_info        = NULL;
    rx_reorder_queue_st *order_node = NULL;

    if(NULL == nic_info)
    {
        return -1;
    }

    if(tid>=TID_NUM)
    {
        return -2;
    }

    rx_info = nic_info->rx_info;
    if(NULL == rx_info)
    {
        return -3;
    }

    ba_ctl  = &rx_info->ba_ctl[tid];
    if(NULL == ba_ctl)
    {
        return -4;
    }

    wf_lock_lock(&ba_ctl->pending_get_de_queue_lock);
    // LOG_I("[%s]: reinit ba_ctl tid:%d", __func__, tid);
    ba_ctl->enable          = wf_false;
    ba_ctl->indicate_seq    = 0xffff;
    ba_ctl->wend_b          = 0xffff;
    if(wf_false == rx_pending_reorder_is_empty(ba_ctl))
    {
        LOG_I("start free pending_reorder_queue");
        while(1)
        {
            order_node = rx_pending_reorder_dequeue(ba_ctl);
            if(NULL == order_node)
            {
                break;
            }
            if(NULL == order_node->pskb)
            {
                ba_ctl->free_skb(nic_info, order_node->pskb);
                order_node->pskb = NULL;
            }
            wf_kfree(order_node);
            order_node = NULL;
        }
    }

    wf_lock_unlock(&ba_ctl->pending_get_de_queue_lock);

    return 0;
}

void wf_rx_ba_all_reinit(nic_info_st *nic_info)
{
    wf_u8 tid;

    for (tid  = 0; tid  < TID_NUM; tid++)
    {
        wf_rx_ba_reinit(nic_info, tid);
    }
}

int rx_reorder_upload(recv_ba_ctrl_st   *ba_order)
{
    int tmp_ret = 0;
    nic_info_st *nic_info = ba_order->nic_node;

    //LOG_I("[%s, %d]", __func__, __LINE__);
    if(NULL == nic_info)
    {
        return -1;
    }

    while(1)
    {
        rx_reorder_queue_st *get_reorder = NULL;

        get_reorder = rx_pending_reorder_getqueue(ba_order);
        if(NULL == get_reorder)
        {
            break;
        }

        if (!SN_LESS(ba_order->indicate_seq, get_reorder->seq_num))
        {
            rx_reorder_queue_st *de_reorder = NULL;
            de_reorder = rx_pending_reorder_dequeue(ba_order);
            if(de_reorder != get_reorder)
            {
                LOG_I("[%s, %d]", __func__, __LINE__);
                rx_pending_reorder_enqueue(de_reorder->seq_num, de_reorder->pskb, ba_order);
                break;
            }


            if (SN_EQUAL(ba_order->indicate_seq, de_reorder->seq_num))
            {
                ba_order->indicate_seq = (ba_order->indicate_seq + 1) & 0xFFF;
                wf_timer_set(&ba_order->expect_time_start, 0);
                //LOG_I("update indicate_seq <p:%d> %d", de_reorder->qos_pri, ba_order->indicate_seq);
            }

            if(de_reorder->pskb)
            {
                ba_order->upload_func(nic_info, de_reorder->pskb);
            }
            //LOG_I("out order <p:%d> seq:%d", de_reorder->qos_pri, de_reorder->seq_num);
            rx_free_reorder_enqueue(ba_order, de_reorder);

            tmp_ret = 0;
        }
        else
        {
            tmp_ret = rx_pending_reorder_get_cnt( ba_order);
            break;
        }

    }

    return tmp_ret;
}

void rx_reorder_timeout_handle(wf_os_api_timer_t * timer)
{
    recv_ba_ctrl_st   *ba_order = WF_CONTAINER_OF((wf_os_api_timer_t *)timer, recv_ba_ctrl_st, reordering_ctrl_timer);
    wf_u8 pktCnt_inQueue = 0;
    rx_reorder_queue_st *get_reorder = NULL;
    if(NULL == ba_order)
    {
        return;
    }

    wf_lock_lock(&ba_order->pending_get_de_queue_lock);
    get_reorder = rx_pending_reorder_getqueue(ba_order);
    if(NULL == get_reorder)
    {
        wf_lock_unlock(&ba_order->pending_get_de_queue_lock);
        return;
    }
    //LOG_I("[%s] indicate_seq:%d, seq_num:%d", __func__, ba_order->indicate_seq, get_reorder->seq_num);
    ba_order->indicate_seq = get_reorder->seq_num;

    pktCnt_inQueue = rx_reorder_upload(ba_order);
    if (pktCnt_inQueue != 0 )
    {
        //LOG_W("%d pkts in order queue(%s)", pktCnt_inQueue, __func__);
        wf_os_api_timer_set(&ba_order->reordering_ctrl_timer, ba_order->wait_timeout);
    }
    wf_lock_unlock(&ba_order->pending_get_de_queue_lock);
}

void wf_rx_data_reorder_core(rx_pkt_t *pkt)
{
    wf_s32 ret = 0;
    wdn_net_info_st *pwdn_info = NULL;
    wf_s32 seq_num = 0;
    wf_u8 pktCnt_inQueue = 0;
    wf_s32 prio    = 0;
    recv_ba_ctrl_st *ba = NULL;

    if(NULL == pkt || NULL == pkt->pskb)
    {
        LOG_E("[%s]: skb is null, drop it", __func__);
        return;
    }


    pwdn_info = pkt->wdn_info;
    if( NULL == pwdn_info)
    {
        LOG_E("[%s] pwdn_info is null", __func__);
        return;
    }

    prio   = pkt->pkt_info.qos_pri;
    ba = &pwdn_info->ba_ctl[prio];
    if(NULL == ba )
    {
        LOG_E("[%s] ba ", __func__);
        return;
    }

    wf_lock_lock(&ba->pending_get_de_queue_lock);
    seq_num = pkt->pkt_info.seq_num;
    ret = rx_do_chk_expect_seq(seq_num, ba);
    if(REORDER_ENQUE == ret)
    {
        //LOG_I("pri:%d enqueue seq:%d  indicate:%d", prio, seq_num, ba->indicate_seq);
        ret = rx_pending_reorder_enqueue(seq_num, pkt->pskb, ba);
        // if(ret < 0) {
        //     LOG_E("[%s]: pending packet error", __func__);
        // }
    }
    else
    {
        ;//LOG_E("drop packet");
    }

    if (ret < 0)
    {
        if (pkt->pskb)
        {
            ba->free_skb(pkt->p_nic_info, pkt->pskb);
            pkt->pskb = NULL;
        }
        wf_lock_unlock(&ba->pending_get_de_queue_lock);
        return;
    }

    pktCnt_inQueue = rx_reorder_upload(ba);
    if (pktCnt_inQueue != 0 )
    {
        //LOG_W("%d pkts in order queue(%s)", pktCnt_inQueue, __func__);
        wf_os_api_timer_set(&ba->reordering_ctrl_timer, ba->wait_timeout);
    }
    wf_lock_unlock(&ba->pending_get_de_queue_lock);
}



static wf_s8 cal_ant_cck_rssi_pwr(wf_u8 lna_idx, wf_u8 vga_idx)
{
    wf_s8 rx_pwr_all = 0x00;

    switch (lna_idx)
    {
        case 7:
            if (vga_idx <= 27)
                rx_pwr_all = -100 + 2 * (27 - vga_idx);
            else
                rx_pwr_all = -100;
            break;
        case 5:
            rx_pwr_all = -74 + 2 * (21 - vga_idx);
            break;
        case 3:
            rx_pwr_all = -60 + 2 * (20 - vga_idx);
            break;
        case 1:
            rx_pwr_all = -44 + 2 * (19 - vga_idx);
            break;
        default:
            //LOG_W("[%s] lna_idx:%d, vga_index:%d", __func__, lna_idx, vga_idx);
            break;
    }

    return rx_pwr_all;
}
static wf_u8 query_rxpwr_percentage(wf_s8 AntPower)
{
    wf_u8 percent = 0;

    if ((AntPower <= -100) || (AntPower >= 20))
    {
        percent =  0;
    }
    else if (AntPower >= 0)
    {
        percent =  100;
    }
    else
    {
        percent =  100 + AntPower;
    }

    return percent;
}
static wf_u8 cal_evm2percentage(wf_s8 Value)
{
    wf_s8 ret_val;

    ret_val = Value;
    ret_val /= 2;

#ifdef MSG_EVM_ENHANCE_ANTDIV
    if (ret_val >= 0)
        ret_val = 0;

    if (ret_val <= -40)
        ret_val = -40;

    ret_val = 0 - ret_val;
    ret_val *= 3;
#else
    if (ret_val >= 0)
        ret_val = 0;

    if (ret_val <= -33)
        ret_val = -33;

    ret_val = 0 - ret_val;
    ret_val *= 3;

    if (ret_val == 99)
        ret_val = 100;
#endif

    return ret_val;
}

wf_s32 wf_rx_calc_str_and_qual(nic_info_st *nic_info, wf_u8 *rx_phystatus, wf_u8 *mac_frame, void *prx_pkt)
{
    prx_pkt_t ppt       = (prx_pkt_t)prx_pkt;
    wf_bool is_cck_rate = wf_false;
    wf_u8 rate_cacl     = 0;
    wf_u8 lna_index     = 0;
    wf_u8 vga_index     = 0;
    wf_s8 rx_pwr        = 0;
    wf_u8 pwdb_all      = 0;
    recv_phy_status_st *rps = (recv_phy_status_st *)rx_phystatus;

    rate_cacl = ppt->pkt_info.rx_rate;

    is_cck_rate = (rate_cacl <= DESC_RATE11M) ? wf_true : wf_false;
    if(is_cck_rate)
    {
        lna_index   = ((rps->cck_agc_rpt_ofdm_cfosho_a & 0xE0) >> 5);
        vga_index   = rps->cck_agc_rpt_ofdm_cfosho_a & 0x1F;
        rx_pwr      = cal_ant_cck_rssi_pwr(lna_index, vga_index);
        pwdb_all    = query_rxpwr_percentage(rx_pwr);

        if(pwdb_all > 40)
        {
            ppt->phy_status.signal_qual = 100;
        }
        else
        {
            if( rps->cck_sig_qual_ofdm_pwdb_all > 64)
            {
                ppt->phy_status.signal_qual = 0;
            }
            else if( rps->cck_sig_qual_ofdm_pwdb_all < 20)
            {
                ppt->phy_status.signal_qual = 100;
            }
            else
            {
                ppt->phy_status.signal_qual = (64 - rps->cck_sig_qual_ofdm_pwdb_all) * 100 / 44;
            }
        }

#ifdef CONFIG_SIGNAL_SCALE_MAPPING
        ppt->phy_status.signal_strength = signal_scale_mapping(pwdb_all);
#else
        ppt->phy_status.signal_strength = pwdb_all;
#endif
    }
    else
    {
        wf_u8 evm       = 0;
        wf_u8 rssi      = 0;
        char tmp_rx_pwr = (rps->path_agc[0].gain & 0x3F) * 2  - 110;
        rssi            = query_rxpwr_percentage(tmp_rx_pwr);
        rx_pwr          = ((rps->cck_sig_qual_ofdm_pwdb_all >> 1) & 0x7F) - 110;
        pwdb_all        = query_rxpwr_percentage(rx_pwr);
        evm             = cal_evm2percentage(rps->stream_rxevm[0]);

        ppt->phy_status.signal_qual = evm & 0xFF;

#ifdef CONFIG_SIGNAL_SCALE_MAPPING
        ppt->phy_status.signal_strength = signal_scale_mapping(rssi);
#else
        ppt->phy_status.signal_strength = rssi;
#endif

    }

#if 0
    ppt->phy_status.signal_strength = signal_scale_mapping(ppt->phy_status.signal_strength);
    ppt->phy_status.rssi            = translate_percentage_to_dbm(ppt->phy_status.signal_strength);
#endif

    return WF_RETURN_OK;
}




