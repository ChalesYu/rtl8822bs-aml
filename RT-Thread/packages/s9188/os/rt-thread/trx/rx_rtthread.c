/*
 * rx_rtthread.c
 *
 * used for rx frame handle.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "common.h"
#include "wf_os_api.h"
#include "hif/hif.h"
#include "hif/hif_queue.h"
#include "wlan_dev/wf_wlan_dev.h"
#include "tx_rtthread.h"
#include "rx_rtthread.h"

#include "wf_debug.h"

#if (PBUF_POOL_BUFSIZE < 1580)
#error "It must config PBUF_POOL_BUFSIZE >= 1580 in lwipopts.h"
#endif

/* A-MSDU  struc
| A-MSDU Frame 1 | A-MSDU Frame 2 | A-MSDU Frame 3 |...| A-MSDU Frame n|
Every A-MSDU frame structrue:
| DA | SA | Len |    MSDU    | Padding |
|  6 |  6 |  2  |   0~2304   |   0~3   |
*/
#define AMSDU_HDR_LEN           14   /* DA(6)  SA(6)  Length(2)  */
#define AMSDU_HDR_DA_OFFSET     0
#define AMSDU_HDR_SA_OFFSET     6
#define AMSDU_HDR_Len_OFFSET    12
#define AMSDU_MSDU_OFFSET       14

static void rx_data_reorder_process(rx_pkt_t *pkt,struct rt_wlan_device *wlan);

wf_bool is_snap_pkt(struct pbuf *p)
{
    return is_snap_hdr(p->payload);
}

rt_inline int create_pbuf(prx_pkt_t prx_pkt, wf_u8 * pbuf,wf_u32 pkt_len)
{
  wf_u16 length = pkt_len;
  struct pbuf *p;
  
  /* We allocate a pbuf chain of pbufs from the pool. */
#if ETH_PAD_SIZE
  length += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif
  p = pbuf_alloc(PBUF_RAW, length, PBUF_POOL);
  if(NULL == p) {
    LOG_E("[%s]: alloc pbuf error",__FUNCTION__);
    return -1;
  }
  
#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
  pbuf_take(p, pbuf,pkt_len);
  
  prx_pkt->pskb = p;
  prx_pkt->pdata = p->payload;
  prx_pkt->ptail = &(((wf_u8 *)p->payload)[pkt_len]);
  prx_pkt->len = p->tot_len;
  
  return 0;
}


rt_inline wf_u8 * pkt_pull(prx_pkt_t prx_pkt,int size)
{
    struct pbuf *p = (struct pbuf *)prx_pkt->pskb;
    if( size > prx_pkt->len )
    {
        wf_u8 * p = NULL;
        LOG_E("%s,size = %d,prx_pkt->len=%d",__func__,size,prx_pkt->len);
        *p= 9;
        return NULL;
    }
    if(p)
    {
      pbuf_header(p, -size);
      prx_pkt->pdata = p->payload;
      prx_pkt->len  = p->tot_len;
    }
    else
    {
        prx_pkt->pdata += size;
        prx_pkt->len -= size;
    }

    return prx_pkt->pdata;
}

rt_inline wf_u8 * pkt_trim(prx_pkt_t prx_pkt,int size)
{
    struct pbuf *p = (struct pbuf *)prx_pkt->pskb;

    if(p->tot_len < size)
    {
        LOG_D("pbuf->tot_len < size");
        return NULL;
    }

    if(size == 0)
    {
        return (wf_u8 *)p->payload;
    }

    p->len -= size;
    prx_pkt->ptail = &(((wf_u8 *)p->payload)[p->len]);
    prx_pkt->len = p->tot_len;

    return  (wf_u8 *)prx_pkt->ptail;
}

rt_inline void free_pbuf_func(void **p)
{
    pbuf_free(*p);
    *p = NULL;
}

rt_inline void wf_rx_eapol_handle(struct rt_wlan_device *wlan, void * pskbv)
{
    rt_uint8_t *frame;
    struct pbuf *p = pskbv;
    
    if (wlan == RT_NULL)
    {
      pbuf_free(p);
      return;
    }
    
    /* handle eapol data directly */
    if (p->len == p->tot_len)
    {
      frame = (rt_uint8_t *)p->payload;
      wf_wlan_dev_report_eapol(wlan, frame, p->tot_len);
      LOG_D("F:%s L:%d run len:%d", __FUNCTION__, __LINE__, p->tot_len);
      pbuf_free(p);
      return;
    }
    
    frame = rt_malloc(p->tot_len);
    if (frame == RT_NULL)
    {
      LOG_E("F:%s L:%d malloc out_buf fail\n", __FUNCTION__, __LINE__);
      pbuf_free(p);
      return;
    }
    /*copy pbuf -> data dat*/
    pbuf_copy_partial(p, frame, p->tot_len, 0);
    pbuf_free(p);
    /* handle eapol data */
    wf_wlan_dev_report_eapol(wlan, frame, p->tot_len);
    LOG_D("F:%s L:%d run len:%d", __FUNCTION__, __LINE__, p->tot_len);
    rt_free(frame);
}

rt_inline void upload_skb(void *pndev, void * pskbv)
{
    int ret;
    wf_u16 eth_type;
    nic_info_st *nic_info;
    struct rt_wlan_device *wlan = pndev;
    struct rt_wlan_priv   *wlan_priv;
    struct pbuf *p = pskbv;
    struct wf_ethhdr *pethhdr;

    if(p == NULL )
    {
      LOG_E("[%s]:NULL Pointer", __func__);
      return ;
    }
    
#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE);
#endif

    wlan_priv = wlan->user_data;
    if (wlan_priv == NULL) {
        pbuf_free(p);
        return;
    }

    nic_info = wlan_priv->nic;
    pethhdr = (struct wf_ethhdr *)p->payload;
    eth_type = htons(pethhdr->type);

#ifdef CFG_ENABLE_MONITOR_MODE
    if(wf_local_cfg_get_work_mode(nic_info) == WF_MONITOR_MODE)
    {
        LOG_E("The Driver not support Monitor mode");
        rt_wlan_dev_promisc_handler(wlan, p, p->tot_len);
        pbuf_free(p);
        return;
    }
#endif

    if ((wf_mlme_check_mode(nic_info, WF_INFRA_MODE) == wf_true) ||
        (wf_mlme_check_mode(nic_info, WF_ADHOC_MODE) == wf_true))
    {
        if((wf_memcmp(wlan_priv->hw_addr,pethhdr->dest,WF_ETH_ALEN) == 0) ||
          (WF_IS_MCAST(pethhdr->dest))) {
            if(0x888e == eth_type) {
              wf_rx_eapol_handle(wlan, p);
            } else {
              rt_wlan_dev_report_data(wlan, p, p->tot_len);
            }
        } else {
            pbuf_free(p);
        }
    }
    else if ((wf_mlme_check_mode(nic_info, WF_MASTER_MODE) == wf_true))
    {
          if(wf_memcmp(wlan_priv->hw_addr,pethhdr->dest,WF_ETH_ALEN) == 0) {
            if(0x888e == eth_type) {
              wf_rx_eapol_handle(wlan, p);
            } else {
              rt_wlan_dev_report_data(wlan, p, p->tot_len);
            }
            return;
          }
          
          if(!WF_IS_MCAST(pethhdr->dest)) {
            // forward it
            wf_u8 *frame = rt_malloc(p->tot_len);
            if (frame == RT_NULL)
            {
              LOG_E("F:%s L:%d malloc out_buf fail\n", __FUNCTION__, __LINE__);
              pbuf_free(p);
              return;
            }
            /*copy pbuf -> data dat*/
            pbuf_copy_partial(p, frame, p->tot_len, 0);
            if(0 != wf_tx_msdu(wlan_priv->nic, frame, p->tot_len, frame))
            {
              rt_free(frame);
            }
            else
            {
              tx_work_wake(wlan);
            }
            pbuf_free(p);
         } else {
            wf_u8 *frame = rt_malloc(p->tot_len);
            if (frame == RT_NULL)
            {
              LOG_E("F:%s L:%d malloc out_buf fail\n", __FUNCTION__, __LINE__);
              pbuf_free(p);
              return;
            }
            /*copy pbuf -> data dat*/
            pbuf_copy_partial(p, frame, p->tot_len, 0);
            if(0 != wf_tx_msdu(wlan_priv->nic, frame, p->tot_len, frame))
            {
              rt_free(frame);
            }
            else
            {
              tx_work_wake(wlan);
            }
            
            rt_wlan_dev_transfer_prot(wlan, p, p->tot_len);
        }
    }
    else
    {
        LOG_I("mlme mode is not allowed to recv frame");
        pbuf_free(p);
    }
}


struct pbuf *alloc_msdu(const wf_u8 *da, const wf_u8 *sa, wf_u8 *msdu,wf_u16 msdu_len)
{
    wf_u16 length;
    wf_u8  *data_ptr;
    struct pbuf *sub_pbuf;
    
    length = msdu_len + 14;
#if ETH_PAD_SIZE
    length += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif
    sub_pbuf = pbuf_alloc(PBUF_RAW, length, PBUF_POOL);
    if(NULL == sub_pbuf) {
      LOG_E("[%s]: alloc sub_pbuf error",__FUNCTION__);
      return NULL;
    }
    
#if ETH_PAD_SIZE
    pbuf_header(sub_pbuf, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    
    pbuf_header(sub_pbuf, -14);
    pbuf_take(sub_pbuf, msdu, msdu_len);

    if (is_snap_pkt(sub_pbuf))
    {
        /* remove RFC1042 or Bridge-Tunnel encapsulation and replace EtherType */
        pbuf_header(sub_pbuf, -((int)SNAP_HDR_SIZE));
        pbuf_header(sub_pbuf, 12);
        wf_memcpy(sub_pbuf->payload, sa, WF_ETH_ALEN);
        wf_memcpy((wf_u8 *)sub_pbuf->payload + WF_ETH_ALEN, da, WF_ETH_ALEN);
    }
    else
    {
        /* Leave Ethernet header part of hdr and full payload */
        wf_u16 len;

        len = htons(sub_pbuf->tot_len);
        wf_memcpy(sub_pbuf->payload, &len, 2);
        wf_memcpy((wf_u8 *)sub_pbuf->payload + 2, da, WF_ETH_ALEN);
        wf_memcpy((wf_u8 *)sub_pbuf->payload + 2 + WF_ETH_ALEN, da, WF_ETH_ALEN);
    }

    return sub_pbuf;
}


int process_amsdu(struct rt_wlan_device *wlan, prx_pkt_t ppkt)
{
    int pktlen;
    int ret = 0;
    wf_u16 subframeSize,padding_len;
    wf_u8 *pdata;
    wf_u8 *sa;
    wf_u8 *da;
    wf_u8 nr_subframes = 0;
    struct pbuf *p;
    wf_u32 frame_type;
    LOG_W("%s,",__func__);

    frame_type = GET_HDR_Type(ppkt->pdata);
    if (frame_type != MAC_FRAME_TYPE_DATA)
    {
        LOG_E("amsdu should be data frame ! check...");
        return -1;
    }

    pktlen = ppkt->len - ppkt->pkt_info.wlan_hdr_len - ppkt->pkt_info.iv_len;
    pdata = ppkt->pdata + ppkt->pkt_info.wlan_hdr_len + ppkt->pkt_info.iv_len;

    LOG_I("amsdu process TID:%d  wlan_hdr_len:%d  pkt_len:%d  iv_len:%d",
          ppkt->pkt_info.qos_pri, ppkt->pkt_info.wlan_hdr_len, ppkt->len, ppkt->pkt_info.iv_len);

    LOG_I("da addr="WF_MAC_FMT,WF_MAC_ARG(pdata));
    LOG_I("sa addr="WF_MAC_FMT,WF_MAC_ARG(pdata + WF_ETH_ALEN));


    do
    {
        subframeSize = WF_PCHAR_2_BE16(pdata + AMSDU_HDR_Len_OFFSET);
        if(subframeSize + AMSDU_HDR_LEN > pktlen)
        {
            LOG_E("amsdu process error  (msdu_size:%d  pktLen:%d)", subframeSize, pktlen);
            break;
        }

        da = pdata;
        sa = pdata + WF_ETH_ALEN;

        p = alloc_msdu(da, sa, pdata + WF_ETH_HLEN, subframeSize);
        if(!p)
        {
            LOG_E("alloc pbuf error");
            break;
        }

        nr_subframes++;
        
        upload_skb(wlan, p);
        LOG_I("upload_skb(): len = %d",p->tot_len);

        pdata += WF_ETH_HLEN;
        pktlen -= WF_ETH_HLEN;

        if (nr_subframes > MAX_SUBFRAME_COUNT)
        {
            LOG_E("too many subframes!!");
            break;
        }

        pdata += subframeSize;
        pktlen -= subframeSize;
        if (pktlen != 0)
        {
            padding_len = 4 - ((subframeSize + WF_ETH_HLEN) & (4 - 1));
            if (padding_len == 4)
                padding_len = 0;

            if (pktlen < padding_len)
            {
                LOG_E("ParseSubframe(): pktlen < padding_len !\n");
                break;
            }
            pdata += padding_len;
            pktlen -= padding_len;
        }
    }
    while(pktlen > AMSDU_HDR_LEN);

    return ret;
}

int wlan_to_eth (prx_pkt_t ppkt)
{
    prx_pkt_info_t prx_info = &ppkt->pkt_info;
    wf_u8 *pbuf = ppkt->pdata;
    wf_u8 rmv_len;
    wf_u8 snap_hdr_offset;
    wf_u16 data_len;
    wf_u16 eth_type;
    wf_bool bsnap;

    if (prx_info->bdecrypted)
    {
        pkt_trim(ppkt, prx_info->icv_len);
    }

    /* if this is a null packet, maybe lps should handle it */
    if(ppkt->len == prx_info->wlan_hdr_len) {
        LOG_I("recv a null packet!");
        return -1;
    }

    snap_hdr_offset = prx_info->wlan_hdr_len + prx_info->iv_len;
    bsnap = is_snap_hdr(pbuf + snap_hdr_offset);
    rmv_len = snap_hdr_offset + (bsnap ? SNAP_HDR_SIZE : 0);

    if (rmv_len > ppkt->len)
    {
        LOG_E("[wlan_to_eth] data_len error (pktlen:%d  rmv_len:%d)",
                ppkt->len,rmv_len);
        return -1;
    }

    data_len = ppkt->len - rmv_len;
    //LOG_W("rmv_len=%d,prx_info->wlan_hdr_len = %d",rmv_len,prx_info->wlan_hdr_len);
    wf_memcpy(&eth_type, pbuf + rmv_len, 2);
    eth_type = ntohs((unsigned short)eth_type);
    prx_info->eth_type = eth_type;

    pbuf = pkt_pull(ppkt, (rmv_len - sizeof(struct wf_ethhdr) + (bsnap ? 2 : 0)));
    if (!pbuf)
    {
        LOG_D("pbuf == NULL");
        return -1;
    }

    wf_memcpy(pbuf, prx_info->dst_addr, MAC_ADDR_LEN);
    wf_memcpy(pbuf + MAC_ADDR_LEN, prx_info->src_addr, MAC_ADDR_LEN);

    if (!bsnap)
    {
        data_len = htons(data_len);
        wf_memcpy(pbuf + 12, (wf_u8 *)&data_len, sizeof(data_len));
    } 

    return 0;
}



static void rx_data_upload_process(rx_pkt_t *pkt,struct rt_wlan_device *wlan)
{
    int ret;
    if (pkt->pkt_info.amsdu)
    {
        /* amsdu process */
        ret = process_amsdu(wlan, pkt);
        if (pkt->pskb)
        {
            pbuf_free(pkt->pskb);
        }
    }
    else
    {
        /* msdu process */
        ret = wlan_to_eth(pkt);
        if (ret == 0)
        {
            upload_skb(wlan, pkt->pskb);
        }
        else
        {
            if (pkt->pskb)
            {
                pbuf_free(pkt->pskb);
            }
        }
    }
}


static void rx_data_reorder_process(rx_pkt_t *pkt,struct rt_wlan_device *wlan)
{
    wdn_net_info_st *pwdn_info = pkt->wdn_info;
    wf_s32 hdr_tansfer_ret = 0;
    wf_s32 prio    = 0;
    wf_s32 seq_num = 0;
    

    prio = pkt->pkt_info.qos_pri;
    seq_num = pkt->pkt_info.seq_num;

    if (NULL == pwdn_info || (prio > 15))
    {
        if (pkt->pskb)
        {
            pbuf_free(pkt->pskb);
        }

        LOG_E("[%s]:pri error:%d , drop packet",__func__, prio);
        return;
    }

    hdr_tansfer_ret = wlan_to_eth(pkt);
    if (hdr_tansfer_ret < 0)
    {
        if (pkt->pskb)
        {
            pbuf_free(pkt->pskb);
        }
        return;
    }
    
    if(0 == pkt->pkt_info.qos_flag)
    {
        upload_skb(wlan, pkt->pskb);
        return;
    }

    if(NULL == pwdn_info->ba_ctl ||  wf_false == pwdn_info->ba_ctl[prio].enable)
    {
        //LOG_W("[%s]：pwdn_info->ba_ctl->%p, pwdn_info->ba_ctl[%d].enable->%d", 
        //      __func__, pwdn_info->ba_ctl, prio, pwdn_info->ba_ctl[prio].enable);
        upload_skb(wlan, pkt->pskb);
        //rx_do_update_expect_seq(seq_num,&pwdn_info->ba_ctl[prio]);
        return;
    }

    if(0 == pwdn_info->ba_ctl[prio].wait_timeout)
    {
        pwdn_info->ba_ctl[prio].wait_timeout = 50;
    }

    if(NULL == pwdn_info->ba_ctl[prio].upload_func || NULL == pwdn_info->ba_ctl[prio].free_skb)
    {
        pwdn_info->ba_ctl[prio].upload_func = upload_skb;
        pwdn_info->ba_ctl[prio].free_skb    = free_pbuf_func;
    }

#if RX_REORDER_THREAD_EN
    if(wf_rx_reorder_queue_insert(pkt) < 0)
    {
        if (pkt->pskb)
        {
            pbuf_free(pkt->pskb);
            pkt->pskb = NULL;
        }
    }
#else
    wf_rx_data_reorder_core(pkt);
#endif

}

void mpdu_process(struct rt_wlan_device *wlan, wf_u32 tot_len, wf_u32 remain_len, wf_u8 *buf, wf_u16 buf_size)
{
    int ret;
    rx_pkt_t pkt;
    rt_wlan_priv_st *pwlan_priv = wlan->user_data;
    nic_info_st *nic_info = pwlan_priv->nic;
    hw_info_st *hw_info = NULL;
    rx_info_t *rx_info = NULL;
    recv_phy_status_st phyStatus;

    if ( NULL == nic_info )
    {
        LOG_E("%s,NULL== nic_info",__func__);
        goto exit;
    }

    hw_info = nic_info->hw_info;

    if (wf_rx_data_type(buf) != WF_PKT_TYPE_FRAME)
    {
        LOG_E("[%s]: error mpdu type, tot_len is %d, remain_len is %d, pkt_len is %d",
              __func__, tot_len, remain_len, buf_size);
        goto exit;
    }

    rx_info = nic_info->rx_info;

    rx_info->rx_total_pkts++;
    rx_info->rx_bytes += buf_size;

    wf_memset(&pkt, 0, sizeof(rx_pkt_t));

    /* agg pkt, new create new skb*/
    ret = create_pbuf(&pkt, buf,buf_size);
    if(ret)
    {
        goto exit ;
    }

    pkt.p_nic_info = nic_info;
    pkt.prx_info = nic_info->rx_info;

    wf_rx_rxd_prase(pkt.pdata, &pkt);

    if (pkt.pkt_info.hif_hdr_len > pkt.len)
    {
        LOG_E("error mpdu (hif_hdr_len:%d  pkt_len:%d)",pkt.pkt_info.hif_hdr_len,pkt.len);
        goto exit;
    }

    /* copy phystatus*/
    if (pkt.pkt_info.phy_status)
    {
        wf_memcpy((wf_u8 *)&phyStatus, (wf_u8 *)pkt.pdata + RXD_SIZE, sizeof(recv_phy_status_st));
    }

    pkt_pull(&pkt, pkt.pkt_info.hif_hdr_len);
    /* process phystatus */
    if (pkt.pkt_info.phy_status)
    {
        hw_info_st *hw_info = nic_info->hw_info;

        wf_rx_calc_str_and_qual(nic_info, (wf_u8 *)&phyStatus, pkt.pdata, &pkt);

        if(hw_info && hw_info->use_drv_odm)
        {
#if defined CONFIG_ARS_DRIVER_SUPPORT
            wf_ars_query_phystatus(nic_info, &phyStatus, pkt.pdata, &pkt);
#endif
        }
    }

    /* process frame*/
    ret=wf_rx_common_process(&pkt);
    if(ret)
    {
        goto exit;
    }

#ifdef CFG_ENABLE_MONITOR_MODE
    if(wf_local_cfg_get_work_mode(nic_info) == WF_MONITOR_MODE)
    {
        LOG_E("The Driver not support Monitor mode");
        pbuf_free(pkt.pskb);
        return;
    }
#endif
  
    if(GET_HDR_Type(pkt.pdata) == MAC_FRAME_TYPE_DATA)
    {
    	if (hw_info->ba_enable == wf_true)
        {
            rx_data_reorder_process(&pkt,wlan);
        }
        else
        {
            rx_data_upload_process(&pkt,wlan);
        }
        
        return;
    }

exit:
    if (pkt.pskb)
    {
        pbuf_free(pkt.pskb);
    }
}


int wf_rx_work(struct rt_wlan_device *wlan, struct hif_netbuf *netbuf)
{
    wf_u8 *pbuf;
    wf_s32 remain_len;
    wf_u16 once_len;
    wf_u16 pkt_len;
    wf_u16 usb_agg_index = 0;
    wf_bool  valid,notice;
    rt_wlan_priv_st *pwlan_priv = wlan->user_data;
    nic_info_st *nic_info = pwlan_priv->nic;
    if(NULL == nic_info)
    {
        return WF_RETURN_FAIL;
    }

    if(nic_info->is_up == 0)
    {
        return WF_RETURN_FAIL;
    }

    if (nic_info->rx_info == NULL)
    {
        return WF_RETURN_FAIL;
    }

    if (nic_info->mlme_info == NULL)
    {
        return WF_RETURN_FAIL;
    }

    if (nic_info->hw_info == NULL)
    {
        return WF_RETURN_FAIL;
    }

    pbuf = netbuf->payload;
    remain_len = netbuf->len;


    do
    {
        pkt_len = wf_rx_get_pkt_len_and_check_valid(pbuf, remain_len, &valid, &notice);
        if (valid != wf_true)
        {
            LOG_E("wf_rx_get_pkt_len_and_check_valid error! agg index:%d, tot_len:%d, remain_len:%d, pkt_len: %d",
                    usb_agg_index, netbuf->len, remain_len, pkt_len);
            // {
            //     int i;
            //     for(i=0; i<netbuf->len;)
            //     {
            //         printk("0x%02X, ", netbuf->payload[i]);
            //         i++;
            //         if((i % 16) == 0) {
            //            printk("\r\n");
            //         }
            //     }
            // }
            return 0;
        }
        
        #ifdef CONFIG_MP_MODE
        wf_mp_rx_common_process(nic_info, pbuf, pkt_len);    
        #else
        mpdu_process(wlan,  netbuf->len, remain_len, pbuf, pkt_len);
        #endif

        usb_agg_index++;
        once_len = WF_RND8(pkt_len);
        pbuf += once_len;
        remain_len -= once_len;
    }
    while (remain_len > 0);
    
    return 0;
}




