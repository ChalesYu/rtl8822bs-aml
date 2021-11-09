/*
 * rx_rtos.c
 *
 * used for rx frame handle.
 *
 * Author: hichard
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
#include "hif/hif.h"
#include "trx/rx_rtos.h"
#include "trx/tx_rtos.h"
#include "wifi/wf_wifi.h"

#if (PBUF_POOL_BUFSIZE < 1580)
#error "It must config PBUF_POOL_BUFSIZE >= 1580 in lwipopts.h"
#endif

/* A-MSDU  struct
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

static void rx_data_reorder_process(rx_pkt_t *pkt, nic_info_st *pnic_info);
extern int wf_wifi_eapol_handle(wf_wifi_hd_t wifi_hd, void *buffer, wf_u16 len);

#if 0
wf_bool is_snap_pkt(struct pbuf *p)
{
    return is_snap_hdr(p->payload);
}

struct pbuf *alloc_msdu(const wf_u8 *da, const wf_u8 *sa, wf_u8 *msdu, wf_u16 msdu_len)
{
    wf_u16 length;
    wf_u8  *data_ptr;
    struct pbuf *sub_pbuf;

    length = msdu_len + 14;
#if ETH_PAD_SIZE
    length += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif
//    sub_pbuf = pbuf_alloc(PBUF_RAW, length, PBUF_POOL);
    if (NULL == sub_pbuf)
    {
        LOG_E("[%s]: alloc sub_pbuf error", __FUNCTION__);
        return NULL;
    }

//#if ETH_PAD_SIZE
//    pbuf_header(sub_pbuf, -ETH_PAD_SIZE); /* drop the padding word */
//#endif
//
//    pbuf_header(sub_pbuf, -14);
//    pbuf_take(sub_pbuf, msdu, msdu_len);

    if (is_snap_pkt(sub_pbuf))
    {
        /* remove RFC1042 or Bridge-Tunnel encapsulation and replace EtherType */
//        pbuf_header(sub_pbuf, -((int)SNAP_HDR_SIZE));
//        pbuf_header(sub_pbuf, 12);
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
        wf_memcpy((wf_u8 *)sub_pbuf->payload + 2 + WF_ETH_ALEN, sa, WF_ETH_ALEN);
    }

    return sub_pbuf;
}

int process_amsdu(nic_info_st *pnic_info, prx_pkt_t ppkt)
{
    int pktlen;
    int ret = 0;
    wf_u16 subframeSize, padding_len;
    wf_u8 *pdata;
    wf_u8 *sa;
    wf_u8 *da;
    wf_u8 nr_subframes = 0;
    struct pbuf *p;
    wf_u32 frame_type;
    LOG_W("%s, ", __func__);

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

    LOG_I("da addr="WF_MAC_FMT, WF_MAC_ARG(pdata));
    LOG_I("sa addr="WF_MAC_FMT, WF_MAC_ARG(pdata + WF_ETH_ALEN));

    do
    {
        /* subframe head(14B)
        DA(B0~5) + SA(B6~11) + LEN(B12~13) + MSDU */
        subframeSize = WF_PCHAR_2_BE16(pdata + AMSDU_HDR_Len_OFFSET);
        if (subframeSize + AMSDU_HDR_LEN > pktlen)
        {
            LOG_E("amsdu process error  (msdu_size:%d  pktLen:%d)", subframeSize, pktlen);
            break;
        }
        da = pdata;
        sa = pdata + WF_ETH_ALEN;

        p = alloc_msdu(da, sa, pdata + WF_ETH_HLEN, subframeSize);
        if (!p)
        {
            LOG_E("alloc pbuf error");
            break;
        }

        nr_subframes++;

        upload_skb(pnic_info, p);
        LOG_I("upload_skb(): len = %d", p->tot_len);

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
#endif

static wf_inline wf_u8 *pkt_pull (prx_pkt_t prx_pkt, int size)
{
    if (size > prx_pkt->len)
    {
        LOG_E("%s, size = %d, prx_pkt->len=%d", __func__, size, prx_pkt->len);
        return NULL;
    }

    prx_pkt->pdata  += size;
    prx_pkt->len    -= size;

    return prx_pkt->pdata;
}

static wf_inline wf_u8 *pkt_trim (prx_pkt_t prx_pkt, int size)
{
    if (prx_pkt->len < size)
    {
        LOG_D("prx_pkt->len < size");
        return NULL;
    }

    if (size)
    {
        prx_pkt->len   -= size;
        prx_pkt->ptail  = prx_pkt->pdata + prx_pkt->len;
    }

    return prx_pkt->ptail;
}

/* mpdu header conver to msdu header
802.11 MAC header(32B) + IV(8B) + SNAP(6+2B) + MSDU + ICV(4B)
IV&ICV only used for tkip encrypt */
static int wlan_to_eth (rx_pkt_t *pkt)
{
    prx_pkt_info_t prx_info = &pkt->pkt_info;
    wf_u8 *pbuf = pkt->pdata;
    wf_u8 rmv_len;
    wf_u8 snap_hdr_offset;
    wf_u16 data_len;
    wf_u16 eth_type;
    wf_bool bsnap;

    /* drop icv filed only for tkip encrypt packet */
    if (prx_info->bdecrypted)
    {
        pkt_trim(pkt, prx_info->icv_len);
    }

    /* if this is a null packet, maybe lps should handle it */
    if (pkt->len == prx_info->wlan_hdr_len)
    {
        LOG_I("recv a null packet!");
        return -1;
    }

    /* rmv_len = mac header(32B) + iv(8B) + snap header(6B if exist) */
    snap_hdr_offset = prx_info->wlan_hdr_len + prx_info->iv_len;
    bsnap = is_snap_hdr(&pbuf[snap_hdr_offset]);
    rmv_len = snap_hdr_offset + (bsnap ? SNAP_HDR_SIZE : 0);

    /* retrive msdu payload length(contain snap type field 2B) */
    if (rmv_len > pkt->len)
    {
        LOG_E("[wlan_to_eth] data_len error (pktlen:%d  rmv_len:%d)",
              pkt->len, rmv_len);
        return -2;
    }
    data_len = pkt->len - rmv_len;

    /* retrive snap type field */
    wf_memcpy(&eth_type, &pbuf[rmv_len], 2);
    eth_type = ntohs((unsigned short)eth_type);
    prx_info->eth_type = eth_type;

    /* point to ethnet mac header */
    pbuf = pkt_pull(pkt, (rmv_len - sizeof(struct wf_ethhdr) + (bsnap ? 2 : 0))); /* 2 for snap type filed */
    if (!pbuf)
    {
        LOG_D("pbuf == NULL");
        return -3;
    }
    /* fill ethnet mac header da&sa filds */
    wf_memcpy(pbuf, prx_info->dst_addr, MAC_ADDR_LEN);
    wf_memcpy(pbuf + MAC_ADDR_LEN, prx_info->src_addr, MAC_ADDR_LEN);
    /* fill len fild if no snap field in mpdu packet */
    if (!bsnap)
    {
        data_len = htons(data_len);
        wf_memcpy(pbuf + 12, (wf_u8 *)&data_len, sizeof(data_len));
    }

    return 0;
}

static void relay_msdu (nic_info_st *nic_info, rx_pkt_t *pkt)
{
    /* new xmit frame */
    wf_u8 *frame = wf_kzalloc(pkt->len);
    if (!frame)
    {
        LOG_E("F:%s L:%d malloc out_buf fail\n", __FUNCTION__, __LINE__);
        return;
    }
    wf_memcpy(frame, pkt->pdata, pkt->len);

    /* send frame */
    if (wf_tx_msdu(nic_info, frame, pkt->len, frame))
    {
        wf_free(frame);
    }
    else
    {
        tx_work_wake(nic_info);
    }
}

static int msdu_parse (nic_info_st *nic_info, rx_pkt_t *pkt)
{
    struct wf_ethhdr *pethhdr = (struct wf_ethhdr *)pkt->pdata;
    hw_info_st *hw_info = nic_info->hw_info;

    /* eapol packet handler */
    if (htons(pethhdr->type) == 0x888e &&
        wf_80211_is_same_addr(hw_info->macAddr, pethhdr->dest))
    {
        wf_wifi_eapol_handle(nic_info, pkt->pdata, pkt->len);
        return 0;
    }

    /* other type packet handler */
    switch (get_sys_work_mode(nic_info))
    {
        case WF_MASTER_MODE :
            if (!wf_80211_is_same_addr(hw_info->macAddr, pethhdr->dest))
            {
                /* the address of packet no point to me, so the packet should
                 relay to this bss */
                relay_msdu(nic_info, pkt);
                /* here only broadcast frame should upload stack */
                if (!wf_80211_is_bcast_addr(pethhdr->dest))
                {
                    return 0;
                }
            }
        case WF_INFRA_MODE :
        case WF_ADHOC_MODE :
            /* upload skb to tcpip stack */
            return 1;

        default :
            LOG_I("mlme mode is not allowed to recv frame");
            return -1;
    }
}

static int create_skb (nic_info_st *pnic_info, rx_pkt_t *pkt)
{
    wf_u8 *skb_data;
    wf_u32 skb_len;
    wf_wifi_skb_hd_t skb;

    /* create new skb object */
    skb = nic_priv(pnic_info)->ops->skb_create(pnic_info,
            pkt->pdata, pkt->len,
            (void *)&skb_data, &skb_len);
    if (!skb)
    {
        LOG_E("[%s]: skb create fail", __func__);
        return -1;
    }

    /* retrive skb information */
    pkt->pdata  = skb_data;
    pkt->ptail  = skb_data + pkt->len; /* ptail point to real data boundary */
    pkt->len    = skb_len;
    pkt->pskb   = skb;

    return 0;
}

static void data_frame_handle (nic_info_st *nic_info, rx_pkt_t *pkt)
{
    hw_info_st *hw_info = nic_info->hw_info;
    wdn_net_info_st *pwdn_info = pkt->wdn_info;
    wf_s32 prio = pkt->pkt_info.qos_pri;
    int ret;

    if (!pwdn_info || prio > 15)
    {
        LOG_E("[%s]: invalid packet", __func__);
        return;
    }

    /* mpdu convert to msdu */
    ret = wlan_to_eth(pkt);
    if (ret)
    {
        LOG_E("[%s]: wlan header conver fail", __func__);
        return;
    }

    /* parse msdu */
    ret = msdu_parse(nic_info, pkt);
    if (ret <= 0)
    {
        LOG_D("[%s]: parse msdu fail", __func__);
        return;
    }

    /* new skb prepare upload to stack */
    ret = create_skb(nic_info, pkt);
    if (ret)
    {
        return;
    }

    /* if sbk should reorder befor upload */
    if (hw_info->ba_enable && pkt->pkt_info.qos_flag &&
        pwdn_info->ba_ctl && pwdn_info->ba_ctl[prio].enable)
    {
        if (pwdn_info->ba_ctl[prio].wait_timeout == 0)
        {
            pwdn_info->ba_ctl[prio].wait_timeout = 50;
        }

        if (!pwdn_info->ba_ctl[prio].upload_func ||
            !pwdn_info->ba_ctl[prio].free_skb)
        {
            pwdn_info->ba_ctl[prio].upload_func =
                (upload_to_kernel)nic_priv(nic_info)->ops->skb_upload;
            pwdn_info->ba_ctl[prio].free_skb =
                (free_skb_cb)nic_priv(nic_info)->ops->skb_free;
        }

#if RX_REORDER_THREAD_EN
        if (wf_rx_reorder_queue_insert(pkt) < 0)
        {
            if (pkt->pskb)
            {
//                pbuf_free(pkt->pskb);
                pkt->pskb = NULL;
            }
        }
#else
        wf_rx_data_reorder_core(pkt);
#endif
    }
    else
    {
        nic_priv(nic_info)->ops->skb_upload(nic_info, pkt->pskb);
    }
}

void mpdu_process(nic_info_st *nic_info, wf_u32 tot_len,
                  wf_u32 remain_len, wf_u8 *buf, wf_u16 buf_size)
{
    rx_pkt_t pkt;
    recv_phy_status_st phyStatus;
    int ret;

    if (wf_rx_data_type(buf) != WF_PKT_TYPE_FRAME)
    {
        LOG_E("[%s]: error mpdu type, tot_len is %d, remain_len is %d, pkt_len is %d",
              __func__, tot_len, remain_len, buf_size);
        return;
    }

    /* update rx info */
    {
        rx_info_t *rx_info = nic_info->rx_info;
        rx_info->rx_total_pkts++;
        rx_info->rx_bytes += buf_size;
    }

    /* initilize pkt */
    {
        wf_memset(&pkt, 0, sizeof(rx_pkt_t));
        pkt.pdata       = buf;
        pkt.ptail       = &buf[buf_size];
        pkt.len         = buf_size;
        pkt.p_nic_info  = nic_info;
        pkt.prx_info    = nic_info->rx_info;
    }

    /* parse rxd */
    wf_rx_rxd_prase(pkt.pdata, &pkt);
    if (pkt.pkt_info.hif_hdr_len > pkt.len)
    {
        LOG_E("error mpdu (hif_hdr_len:%d  pkt_len:%d)",
              pkt.pkt_info.hif_hdr_len, pkt.len);
        return;
    }

    /* copy phystatus*/
    if (pkt.pkt_info.phy_status)
    {
        wf_memcpy((wf_u8 *)&phyStatus, (wf_u8 *)pkt.pdata + RXD_SIZE,
                  sizeof(recv_phy_status_st));
    }

    pkt_pull(&pkt, pkt.pkt_info.hif_hdr_len);
    /* process phystatus */
    if (pkt.pkt_info.phy_status)
    {
        wf_rx_calc_str_and_qual(nic_info, (wf_u8 *)&phyStatus, pkt.pdata, &pkt);
#if defined CONFIG_ARS_DRIVER_SUPPORT
        if (hw_info && hw_info->use_drv_odm)
        {
            //ars to do
            wf_ars_query_phystatus(nic_info, &phyStatus, pkt.pdata, &pkt);
        }
#endif
    }

    /* process mpdu body */
    ret = wf_rx_common_process(&pkt);
    if (ret)
    {
        return;
    }

#ifdef CFG_ENABLE_MONITOR_MODE
    if (wf_local_cfg_get_work_mode(nic_info) == WF_MONITOR_MODE)
    {
        LOG_E("The Driver not support Monitor mode");
//        pbuf_free(pkt.pskb);
        return;
    }
#endif

    if (GET_HDR_Type(pkt.pdata) == MAC_FRAME_TYPE_DATA)
    {
        data_frame_handle(nic_info, &pkt);
    }
}


int wf_rx_work(nic_info_st *pnic_info, struct hif_netbuf *netbuf)
{
    wf_u8 *pbuf;
    wf_s32 remain_len;
    wf_u16 once_len;
    wf_u16 pkt_len;
    wf_u16 usb_agg_index = 0;
    wf_bool valid, notice;

    if (NULL == pnic_info)
    {
        return WF_RETURN_FAIL;
    }

    if (pnic_info->is_up == wf_false)
    {
        return WF_RETURN_FAIL;
    }

    if (pnic_info->rx_info == NULL)
    {
        return WF_RETURN_FAIL;
    }

    if (pnic_info->mlme_info == NULL)
    {
        return WF_RETURN_FAIL;
    }

    if (pnic_info->hw_info == NULL)
    {
        return WF_RETURN_FAIL;
    }

    pbuf = netbuf->payload;
    remain_len = netbuf->len;

    do
    {
        /* get the first packet length. the total data maybe consist ofmutil
        mpdu packets, it's made by host io bus(usb/sdio) */
        pkt_len = wf_rx_get_pkt_len_and_check_valid(pbuf, remain_len, &valid, &notice);
        if (valid != wf_true)
        {
            LOG_E("wf_rx_get_pkt_len_and_check_valid error! agg index:%d, tot_len:%d, remain_len:%d, pkt_len: %d",
                  usb_agg_index, netbuf->len, remain_len, pkt_len);
#if 0
            {
                int i;
                for(i = 0; i < netbuf->len;)
                {
                    printk("0x%02X, ", netbuf->payload[i]);
                    i++;
                    if ((i % 16) == 0)
                    {
                        printk("\r\n");
                    }
                }
            }
#endif
            return 0;
        }

#ifdef CONFIG_MP_MODE
        wf_mp_rx_common_process(pnic_info, pbuf, pkt_len);
#else
        mpdu_process(pnic_info, netbuf->len, remain_len, pbuf, pkt_len);
#endif

        /* prepare handle next mpdu */
        usb_agg_index++;
        once_len = WF_RND8(pkt_len);
        pbuf += once_len;
        remain_len -= once_len;
    }
    while (remain_len > 0);

    return 0;
}




