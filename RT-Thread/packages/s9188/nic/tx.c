#include "common.h"
#include "wf_debug.h"

#define XMIT_DATA_BUFFER_CNT (4)
#define XMIT_MGMT_BUFFER_CNT (8)
#define XMIT_CMD_BUFFER_CNT  (4)

#define XMIT_ACK_REG         0x0298

#define ETHTYPE_ARP          0x0806
#define ETHTYPE_EAPOL        0x888e

static wf_bool xmit_buf_resource_new(struct xmit_buf *pxmit_buf,
                                     nic_info_st *nic_info, wf_u32 alloc_sz)
{
    if (alloc_sz > 0)
    {
        pxmit_buf->pallocated_buf = wf_kzalloc(alloc_sz + XMITBUF_ALIGN_SZ);
        if (pxmit_buf->pallocated_buf == NULL)
        {
            return wf_false;
        }

        pxmit_buf->ptail = pxmit_buf->pbuf =
                               (wf_u8 *) WF_N_BYTE_ALIGMENT((SIZE_PTR) (pxmit_buf->pallocated_buf),
                                       XMITBUF_ALIGN_SZ);
    }

    return wf_true;
}

typedef struct
{
    unsigned char version:4;
    unsigned char header_len:4;
    unsigned char tos;
    unsigned short total_len;
    unsigned short ident;
    unsigned short flags:3;
    unsigned short seg_offset:13;
    unsigned char  ttl;
    unsigned char  proto;
    unsigned short checksum;
    unsigned char  src_ip[4];
    unsigned char  dest_ip[4];
} wf_packed ip_header;

static void do_set_qos(struct xmit_frame *pxmitframe, ip_header *ip_hdr)
{
    wf_s32 user_priority = 0;

    if (pxmitframe->ether_type == 0x0800)
    {
        user_priority = ip_hdr->tos >> 5;
    }

#if 0
    if (user_priority)
    {
        LOG_W("user_priority=%d  [proto=%d]",user_priority,ip_hdr.proto);
        LOG_W("src_ip:  %d.%d.%d.%d",ip_hdr.src_ip[0],ip_hdr.src_ip[1],ip_hdr.src_ip[2],ip_hdr.src_ip[3]);
        LOG_W("dest_ip:  %d.%d.%d.%d",ip_hdr.dest_ip[0],ip_hdr.dest_ip[1],ip_hdr.dest_ip[2],ip_hdr.dest_ip[3]);
    }
#endif

    pxmitframe->priority = user_priority;
    pxmitframe->hdrlen = WLAN_HDR_A3_QOS_LEN;
}

static wf_bool xmit_frame_sec_init(nic_info_st *nic_info, wdn_net_info_st *pwdn,
                                   struct xmit_frame *pxmitframe)
{
    sec_info_st *sec_info = nic_info->sec_info;

    memset(pxmitframe->dot11tkiptxmickey.skey, 0, 16);
    memset(pxmitframe->dot118021x_UncstKey.skey, 0, 16);

    /* if network is 8021X type, befor EAPOL 4 handshark complete, only EAPOL
    packet can get througth */
    if (sec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X &&
        pwdn->ieee8021x_blocked == wf_true)
    {
        LOG_I("pwdn->ieee8021x_blocked == wf_true");

        pxmitframe->encrypt_algo = _NO_PRIVACY_;

        if (pxmitframe->ether_type != 0x888e)
        {
            LOG_I("pxmitframe->ether_type(%.4x) != 0x888e", pxmitframe->ether_type);
            return wf_false;
        }
    }
    else
    {
        GET_ENCRY_ALGO(sec_info, pwdn, pxmitframe->encrypt_algo, pxmitframe->bmcast);

        switch(sec_info->dot11AuthAlgrthm)
        {
            case dot11AuthAlgrthm_Open:
            case dot11AuthAlgrthm_Shared:
            case dot11AuthAlgrthm_Auto:
                pxmitframe->key_idx = (wf_u8) sec_info->dot11PrivacyKeyIndex;
                break;
            case dot11AuthAlgrthm_8021X:
                if (pxmitframe->bmcast)
                    pxmitframe->key_idx = (wf_u8) sec_info->dot118021XGrpKeyid;
                else
                    pxmitframe->key_idx = 0;
                break;
            default:
                pxmitframe->key_idx = 0;
                break;
        }

        if (((pxmitframe->encrypt_algo == _WEP40_) || (pxmitframe->encrypt_algo == _WEP104_))
            && (pxmitframe->ether_type == 0x888e))
            pxmitframe->encrypt_algo = _NO_PRIVACY_;
    }

    /* iv & icv & txmickey */
    switch(pxmitframe->encrypt_algo)
    {
        case _WEP40_:
        case _WEP104_:
            pxmitframe->iv_len = 4;
            pxmitframe->icv_len = 4;
            WEP_IV(pxmitframe->iv, pwdn->dot11txpn, pxmitframe->key_idx);
            break;
        case _TKIP_:
            pxmitframe->iv_len = 8;
            pxmitframe->icv_len = 4;

            if (sec_info->busetkipkey == wf_false)
            {
                return wf_false;
            }

            if (pxmitframe->bmcast)
                TKIP_IV(pxmitframe->iv, pwdn->dot11txpn, pxmitframe->key_idx);
            else
                TKIP_IV(pxmitframe->iv, pwdn->dot11txpn, 0);

            wf_memcpy(pxmitframe->dot11tkiptxmickey.skey, pwdn->dot11tkiptxmickey.skey, 16);
            break;
        case _AES_:
            pxmitframe->iv_len = 8;
            pxmitframe->icv_len = 8;

            if (pxmitframe->bmcast)
                AES_IV(pxmitframe->iv, pwdn->dot11txpn, pxmitframe->key_idx);
            else
                AES_IV(pxmitframe->iv, pwdn->dot11txpn, 0);
            break;
        default:
            pxmitframe->iv_len = 0;
            pxmitframe->icv_len = 0;
            break;
    }

    if (pxmitframe->encrypt_algo != _NO_PRIVACY_ &&
        pxmitframe->encrypt_algo != _WEP40_ &&
        pxmitframe->encrypt_algo != _WEP104_)
    {
        wf_memcpy(pxmitframe->dot118021x_UncstKey.skey,
                  pwdn->dot118021x_UncstKey.skey, 16);
    }

    pxmitframe->bswenc = pxmitframe->encrypt_algo == _AES_ ? wf_false : wf_true;

    return wf_true;
}

static void xmit_frame_vcs_init(nic_info_st *nic_info, struct xmit_frame *pxmitframe)
{
    wf_u32 sz;
    hw_info_st *hw_info = nic_info->hw_info;
    wdn_net_info_st *pwdn = pxmitframe->pwdn;

    if (pxmitframe->nr_frags != 1)
    {
        sz = hw_info->frag_thresh;
    }
    else
    {
        sz = pxmitframe->last_txcmdsz;
    }

    if (pwdn->network_type < WIRELESS_11_24N)
    {
        if (sz > hw_info->rts_thresh)
        {
            pxmitframe->vcs_mode = RTS_CTS;
        }
        else
        {
            if (pwdn->rtsen)
                pxmitframe->vcs_mode = RTS_CTS;
            else if (pwdn->cts2self)
                pxmitframe->vcs_mode = CTS_TO_SELF;
            else
                pxmitframe->vcs_mode = NONE_VCS;
        }
    }
    else
    {
        while (wf_true)
        {
            if (pwdn->rtsen || pwdn->cts2self)
            {
                if (pwdn->rtsen)
                    pxmitframe->vcs_mode = RTS_CTS;
                else if (pwdn->cts2self)
                    pxmitframe->vcs_mode = CTS_TO_SELF;

                break;
            }

            if (pxmitframe->ht_en)
            {
                wf_u8 HTOpMode = pwdn->HT_protection;
                if ((pwdn->bw_mode && (HTOpMode == 2 || HTOpMode == 3))
                    || (!pwdn->bw_mode && HTOpMode == 3))
                {
                    pxmitframe->vcs_mode = RTS_CTS;
                    break;
                }
            }

            if (sz > hw_info->rts_thresh)
            {
                pxmitframe->vcs_mode = RTS_CTS;
                break;
            }

            if (pxmitframe->ampdu_en == wf_true)
            {
                pxmitframe->vcs_mode = RTS_CTS;
                break;
            }

            pxmitframe->vcs_mode = NONE_VCS;
            break;
        }
    }

    if (hw_info->vcs_en == 1)
    {
        pxmitframe->vcs_mode = hw_info->vcs_type;
    }
}

wf_bool wf_xmit_frame_init(nic_info_st *nic_info, struct xmit_frame *pxmitframe, wf_u8 *msdu_buf, int msdu_len)
{
    wf_u8 *ra_addr;
    struct wf_ethhdr *pethhdr;
    ip_header iphdr;
    wdn_net_info_st *pwdn = NULL;
    wf_u8 bc_addr[WF_ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    hw_info_st *hw_info = nic_info->hw_info;
    int pkt_offset = 0;

    pethhdr = (struct wf_ethhdr *)msdu_buf;
    if (wf_mlme_check_mode(nic_info, WF_ADHOC_MODE))
    {
        ra_addr = pethhdr->dest;
    }
    else if (wf_mlme_check_mode(nic_info, WF_INFRA_MODE))
    {
        ra_addr = wf_wlan_get_cur_bssid(nic_info);
    }
    else if (wf_mlme_check_mode(nic_info, WF_MASTER_MODE))
    {
        ra_addr = pethhdr->dest;
    }
    else
    {
        LOG_E("[%s]: mlme mode error, can't xmit data", __func__);
        return wf_false;
    }

    pxmitframe->bmcast = IS_MCAST(ra_addr);
    pxmitframe->ether_type = wf_be_u16_to_host_u16(&pethhdr->type);
    pkt_offset += WF_ETH_HLEN;

    if(pxmitframe->bmcast)
    {
        pwdn = wf_wdn_find_info(nic_info, bc_addr);
        if(pwdn == NULL)
        {
            LOG_I("get wdn_info fail");
            return wf_false;
        }
    }
    else
    {
        pwdn = wf_wdn_find_info(nic_info, ra_addr);
        if (pwdn == NULL)
        {
            LOG_I("[frame_attrib_init] => get sta_unit fail, ra:" WF_MAC_FMT, WF_MAC_ARG(ra_addr));
            return wf_false;
        }
    }

    pxmitframe->pktlen = msdu_len - WF_ETH_HLEN;
    pxmitframe->dhcp_pkt = 0;

    iphdr.tos = 0;

    switch (pxmitframe->ether_type)
    {
        case WF_ETH_P_IP:
            iphdr.header_len = WF_GET_IPV4_IHL(msdu_buf+pkt_offset);
            iphdr.proto = WF_GET_IPV4_PROTOCOL(msdu_buf+pkt_offset);
            iphdr.tos = WF_GET_IPV4_TOS(msdu_buf+pkt_offset);

#ifdef TX_DEBUG
            LOG_D("[%s]: header_len:%d", __func__, iphdr.header_len);
            LOG_D("[%s]: proto:0x%x", __func__, iphdr.proto);
            LOG_D("[%s]: tos:0x%x", __func__, iphdr.tos);
#endif

            pkt_offset += iphdr.header_len * 4;
            switch(iphdr.proto)
            {
                case 0x01:
                {
                    /* ICMP */
#ifdef TX_DEBUG
                    LOG_D("= ICMP Packet =");
#endif
                }
                break;
                case 0x02:
                {
                    /* IGMP */
#ifdef TX_DEBUG
                    LOG_D("= IGMP Packet =");
#endif
                }
                break;
                case 0x11:
                    /* UDP */
                {
                    wf_u8 udp[8];

                    wf_memcpy(udp,msdu_buf+pkt_offset, 8);
                    pkt_offset += 8;

#ifdef TX_DEBUG
                    LOG_D("= UDP Packet =");
#endif
                    if ((WF_GET_UDP_SRC(udp) == 68 && WF_GET_UDP_DST(udp) == 67)
                        || (WF_GET_UDP_SRC(udp) == 67 && WF_GET_UDP_DST(udp) == 68))
                    {
                        if (pxmitframe->pktlen > 282)
                        {
                            pxmitframe->dhcp_pkt = 1;

//#ifdef TX_DEBUG
                            LOG_D("<DHCP> Send");
//#endif
                        }
                    }
                }
                break;
                case 0x06:
                {
                    /* TCP */

                    //wf_memcpy(tcp,msdu_buf+pkt_offset, 20);
                    pkt_offset += 20;

#ifdef TX_DEBUG
                    LOG_D("= TCP Packet =");
#endif
                }
                break;
                default:
                    break;
            }
            break;
        // case 0x888e:
        //     #ifdef TX_DEBUG
        //     LOG_D("= EAPOL packet =");
        //     #endif
        //     break;
        // case WF_ETH_P_ARP:
        //     #ifdef TX_DEBUG
        //     LOG_D("= ARP Packet =");
        //     #endif
        //     break;
        // case WF_ETH_P_IPV6:
        //     #ifdef TX_DEBUG
        //     LOG_D("= IPv6 Packet =");
        //     #endif
        //     break;
        default:
            break;
    }

#ifdef CONFIG_LPS
    // if (pxmitframe->icmp_pkt == 1)
    // {
    //     wf_lps_wakeup(nic_info, LPS_CTRL_SPECIAL_PACKET, wf_true);
    // }
    // else if (pxmitframe->dhcp_pkt == 1)
    // {
    //     //DBG_COUNTER(nic_info->tx_logs.core_tx_upd_attrib_active);
    //     wf_lps_wakeup(nic_info, LPS_CTRL_SPECIAL_PACKET, wf_true);
    // }
    // if(atomic_read(&pwr_info->lps_spc_flag) == 0 && pwr_info->b_fw_current_in_ps_mode == wf_true)
    // {
    //     wf_lps_wakeup(nic_info, LPS_CTRL_SPECIAL_PACKET, wf_true);
    //     atomic_set(&pwr_info->lps_spc_flag, 1);
    // }
#endif

    if (xmit_frame_sec_init(nic_info, pwdn, pxmitframe) == wf_false)
    {
        return wf_false;
    }

    pxmitframe->pwdn = pwdn;
    pxmitframe->priority = 0;
    pxmitframe->pkt_hdrlen = WF_ETH_HLEN;
    pxmitframe->hdrlen = WLAN_HDR_A3_LEN;

    if(pwdn->qos_option)
    {
        do_set_qos(pxmitframe, &iphdr);

        if (pwdn->acm_mask != 0)
        {
            pxmitframe->priority = wf_chk_qos(pwdn->acm_mask, pxmitframe->priority, 1);
        }
    }

#ifdef TX_DEBUG
    LOG_D("priority:%d",pxmitframe->priority);
#endif

    pxmitframe->qsel = pxmitframe->priority;

    if (hw_info->dot80211n_support)
    {
        pxmitframe->ampdu_en = wf_false;
        pxmitframe->ht_en = pwdn->ht_enable;
    }

    return wf_true;
}


wf_bool frame_txp_addmic(nic_info_st *nic_info, struct xmit_frame *pxmitframe)
{
    wf_u8 hw_hdr_offset = 0;
    wf_u8 priority[4] = { 0x0, 0x0, 0x0, 0x0 };
    wf_u8 null_key[16];
    wf_u8 *pframe, *payload, mic[8];
    wf_s32 curfragnum, length;
    struct mic_data micdata;
    hw_info_st *hw_info = nic_info->hw_info;
    sec_info_st *sec_info = nic_info->sec_info;

    /* make none(all zone) key */
    wf_memset(null_key, 0x0, sizeof(null_key));

#ifdef CONFIG_SOFT_TX_AGGREGATION
    hw_hdr_offset = TXDESC_SIZE + (pxmitframe->pkt_offset * PACKET_OFFSET_SZ);
#else
    hw_hdr_offset = TXDESC_OFFSET;
#endif

    if (pxmitframe->encrypt_algo == _TKIP_)
    {
        pframe = pxmitframe->buf_addr + hw_hdr_offset; /* point to msdu filed */
        /* calculate use tx mic key */
        if (pxmitframe->bmcast)
        {
            if (wf_memcmp(sec_info->dot118021XGrptxmickey[sec_info->dot118021XGrpKeyid].skey,
                          null_key, 16) == 0)
            {
                return wf_false;
            }
            wf_sec_mic_set_key(&micdata,
                               sec_info->dot118021XGrptxmickey[sec_info->dot118021XGrpKeyid].skey);
        }
        else
        {
            if (!wf_memcmp(&pxmitframe->dot11tkiptxmickey.skey[0], null_key,
                           sizeof(null_key)))
            {
                return wf_false;
            }
            wf_sec_mic_set_key(&micdata, &pxmitframe->dot11tkiptxmickey.skey[0]);
        }
        /* calculate use DA & SA */
        if (pframe[1] & 1) /* ToDS == 1 */
        {
            wf_sec_mic_append(&micdata, &pframe[16], 6); /* addr3 for DA */
            if (pframe[1] & 2) /* From Ds == 1 */
                wf_sec_mic_append(&micdata, &pframe[24], 6); /* addr4 for SA */
            else
                wf_sec_mic_append(&micdata, &pframe[10], 6); /* addr2 for SA */
        }
        else /* ToDS == 0 */
        {
            wf_sec_mic_append(&micdata, &pframe[4], 6); /* addr1 for DA */
            if (pframe[1] & 2) /* From Ds == 1 */
                wf_sec_mic_append(&micdata, &pframe[16], 6); /* addr3 for SA */
            else
                wf_sec_mic_append(&micdata, &pframe[10], 6); /* addr2 for SA */
        }
        /* calculate use priority value */
        if(pxmitframe->pwdn->qos_option)
            priority[0] = (wf_u8)pxmitframe->priority;
        wf_sec_mic_append(&micdata, &priority[0], 4);
        /* calculate use msdu(all fragments) */
        payload = pframe;
        for (curfragnum = 0; curfragnum < pxmitframe->nr_frags; curfragnum++)
        {
            payload = (wf_u8 *)WF_RND4((SIZE_PTR)payload);
            payload = &payload[pxmitframe->hdrlen + pxmitframe->iv_len];
            if ((curfragnum + 1) == pxmitframe->nr_frags)
            {
                length = pxmitframe->last_txcmdsz - pxmitframe->hdrlen -
                         pxmitframe->iv_len - pxmitframe->icv_len;
                wf_sec_mic_append(&micdata, payload, length);
                payload += length;
            }
            else
            {
                length = hw_info->frag_thresh - pxmitframe->hdrlen -
                         pxmitframe->iv_len - pxmitframe->icv_len;
                wf_sec_mic_append(&micdata, payload, length);
                payload += length + pxmitframe->icv_len;
#if 1
                /* auther: luozhi
                   date: 2020-9-10
                   todo: fix bug
                   point to next fragment(payload) should skip tx descript head */
                payload += hw_hdr_offset;
#endif
            }
        }

        /* fill mic field */
        wf_sec_get_mic(&micdata, &(mic[0]));
//        LOG_I("txp_addmic: before add mic code!!!");
//        LOG_I("txp_addmic: pattrib->last_txcmdsz=%d!!!",
//              pattrib->last_txcmdsz);
        wf_memcpy(payload, &(mic[0]), sizeof(mic));
        pxmitframe->last_txcmdsz += sizeof(mic);

//        LOG_I("\n ========last pkt========");
//        payload = payload - pattrib->last_txcmdsz + 8;
//        for (curfragnum = 0; curfragnum < pattrib->last_txcmdsz;
//             curfragnum = curfragnum + 8)
//            LOG_I(" %.2x,  %.2x,  %.2x,  %.2x,  %.2x,  %.2x,  %.2x,  %.2x ",
//                  *(payload + curfragnum), *(payload + curfragnum + 1),
//                  *(payload + curfragnum + 2), *(payload + curfragnum + 3),
//                  *(payload + curfragnum + 4), *(payload + curfragnum + 5),
//                  *(payload + curfragnum + 6), *(payload + curfragnum + 7));
    }

    return wf_true;
}

static wf_bool tx_mac_hdr_build(nic_info_st *nic_info, struct xmit_frame *pxmitframe, wf_u8 *msdu_buf, wf_u8 * hdr)
{
    wf_bool qos_option = wf_false;
    wf_u16 *qc;
    struct wf_ethhdr *pethhdr;
    wf_80211_data_t *pwlanhdr = (wf_80211_data_t *)hdr;
    wf_u16 *fctrl = &pwlanhdr->frame_control;
    hw_info_st *hw_info = nic_info->hw_info;
    wdn_net_info_st *pwdn;
    wf_u8 bc_addr[WF_ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    WF_ASSERT(pxmitframe != NULL);

    pethhdr = (struct wf_ethhdr *)msdu_buf;
    wf_memset(hdr, 0, WLANHDR_OFFSET);
    SetFrameSubType(fctrl, WIFI_DATA_TYPE);
    if ((wf_mlme_check_mode(nic_info, WF_INFRA_MODE) == wf_true))
    {
        SetToDs(fctrl);
        wf_memcpy(pwlanhdr->addr3, pethhdr->dest, WF_ETH_ALEN);
        wf_memcpy(pwlanhdr->addr2, pethhdr->src, WF_ETH_ALEN);
        wf_memcpy(pwlanhdr->addr1, wf_wlan_get_cur_bssid(nic_info), WF_ETH_ALEN);
    }
    else if ((wf_mlme_check_mode(nic_info, WF_MASTER_MODE) == wf_true))
    {
        SetFrDs(fctrl);
        wf_memcpy(pwlanhdr->addr3, pethhdr->src, WF_ETH_ALEN);
        wf_memcpy(pwlanhdr->addr2, wf_wlan_get_cur_bssid(nic_info), WF_ETH_ALEN);
        wf_memcpy(pwlanhdr->addr1, pethhdr->dest, WF_ETH_ALEN);
    }
    else if (wf_mlme_check_mode(nic_info, WF_ADHOC_MODE) == wf_true)
    {
        wf_memcpy(pwlanhdr->addr3, wf_wlan_get_cur_bssid(nic_info), WF_ETH_ALEN);
        wf_memcpy(pwlanhdr->addr2, pethhdr->src, WF_ETH_ALEN);
        wf_memcpy(pwlanhdr->addr1, pethhdr->dest, WF_ETH_ALEN);
    }
    else
    {
        LOG_I("mlme mode is not allowed to xmit frame");
        return wf_false;
    }

    if(IS_MCAST(pwlanhdr->addr1))
    {
        pwdn = wf_wdn_find_info(nic_info, bc_addr);
    }
    else
    {
        pwdn = wf_wdn_find_info(nic_info, pwlanhdr->addr1);
    }
    if (pwdn == NULL)
    {
        LOG_I("%s, pwdn==NULL\n", __func__);
        return wf_false;
    }
    if (pxmitframe->pwdn != pwdn)
    {
        LOG_I("[%s]:pxmitframe->pwdn(%p) != pwdn(%p)\n", __func__,
              pxmitframe->pwdn, pwdn);
        return wf_false;
    }

    if(pwdn->qos_option)
    {
        SetFrameSubType(fctrl, WIFI_QOS_DATA_TYPE);
        qos_option = wf_true;
    }

    if (pxmitframe->encrypt_algo)
        SetPrivacy(fctrl);

    if (qos_option)
    {
        qc = (unsigned short *)(hdr + pxmitframe->hdrlen - 2);

        if (pxmitframe->priority)
            SetPriority(qc, pxmitframe->priority);

        SetEOSP(qc, 0);

        SetAckpolicy(qc, 0);
    }

    pwdn->wdn_xmitpriv.txseq_tid[pxmitframe->priority]++;
    pwdn->wdn_xmitpriv.txseq_tid[pxmitframe->priority] &= 0xFFF;
    pxmitframe->seqnum = pwdn->wdn_xmitpriv.txseq_tid[pxmitframe->priority];

    SetSeqNum(hdr, pxmitframe->seqnum);

    if (hw_info->dot80211n_support)
    {
        if (pwdn->ht_enable && pwdn->htpriv.ampdu_enable)
        {
            if (pwdn->htpriv.agg_enable_bitmap & WF_BIT(pxmitframe->priority))
                pxmitframe->ampdu_en = wf_true;
        }

        if ( pxmitframe->ampdu_en == wf_true)
        {
            wf_u16 tx_seq;

            tx_seq = pwdn->ba_starting_seqctrl[pxmitframe->priority & 0x0f];

            if (SN_LESS(pxmitframe->seqnum, tx_seq))
            {
                pxmitframe->ampdu_en = wf_false;
            }
            else if (SN_EQUAL(pxmitframe->seqnum, tx_seq))
            {
                pwdn->ba_starting_seqctrl[pxmitframe->priority & 0x0f] = (tx_seq + 1) & 0xfff;
                pxmitframe->ampdu_en = wf_true;
            }
            else
            {
                pwdn->ba_starting_seqctrl[pxmitframe->priority & 0x0f] = (pxmitframe->seqnum + 1) & 0xfff;
                pxmitframe->ampdu_en = wf_true;
            }

        }
    }

    return wf_true;
}

static wf_s32 tx_set_snap(wf_u16 h_proto, wf_u8 * data)
{
    struct wf_80211_snap_header *snap = (struct wf_80211_snap_header *)data;

    snap->dsap = 0xaa;
    snap->ssap = 0xaa;
    snap->ctrl = 0x03;

    snap->oui[0] = 0x00;
    snap->oui[1] = 0x00;
    if (h_proto == 0x8137 || h_proto == 0x80f3)
    {
        snap->oui[2] = 0xf8;
    }
    else
    {
        snap->oui[2] = 0x00;
    }

    *(wf_u16 *) (data + sizeof(wf_80211_snap_header_st)) = htons(h_proto);

    return sizeof(wf_80211_snap_header_st) + sizeof(wf_u16);
}

void wf_tx_stats_cnt(nic_info_st *nic_info, struct xmit_frame *pxmitframe, int sz)
{
    wdn_net_info_st *pwdn = NULL;
    struct wdninfo_stats *pwdns = NULL;
    mlme_info_t *mlme_info = nic_info->mlme_info;
    tx_info_st *tx_info = nic_info->tx_info;

    if ((pxmitframe->frame_tag & 0x0f) == DATA_FRAMETAG)
    {
        mlme_info->link_info.num_tx_ok_in_period += 1;
        mlme_info->link_info.num_tx_ok_in_period_with_tid[pxmitframe->qsel] += 1;

        tx_info->tx_pkts += 1;
        tx_info->tx_bytes += sz;

        pwdn = pxmitframe->pwdn;
        if (pwdn)
        {
            pwdns = &pwdn->wdn_stats;

            pwdns->tx_pkts += 1;

            pwdns->tx_bytes += sz;
        }

    }
}

static wf_u8 sectype_to_hwdesc_get(struct xmit_frame *pxmitframe)
{
    wf_u8 sectype = 0;

    if ((pxmitframe->encrypt_algo > 0) && !pxmitframe->bswenc)
    {
        switch (pxmitframe->encrypt_algo)
        {
            case _WEP40_:
            case _WEP104_:
                sectype = 1;
                break;
            case _TKIP_:
            case _TKIP_WTMIC_:
                sectype = 2;
                break;
            case _AES_:
                sectype = 3;
                break;

            case _NO_PRIVACY_:
                sectype = 0;
                break;
            default:
                sectype = 4;
                break;
        }
    }

    return sectype;
}

static wf_u8 txdesc_scmapping_get(nic_info_st *nic_info, struct xmit_frame *pxmitframe)
{
    wf_u8 txdesc_scseting = 0;
    wdn_net_info_st *pwdn = pxmitframe->pwdn;

    if (wf_wlan_get_cur_bw(nic_info) == CHANNEL_WIDTH_40)
    {
        if (pwdn->bw_mode == CHANNEL_WIDTH_40)
        {
            txdesc_scseting = HT_DATA_SC_DONOT_CARE;
        }
        else if (pwdn->bw_mode == CHANNEL_WIDTH_20)
        {
            if (pwdn->channle_offset == HAL_PRIME_CHNL_OFFSET_UPPER)
            {
                txdesc_scseting = HT_DATA_SC_20_UPPER_OF_40MHZ;
            }
            else if (pwdn->channle_offset == HAL_PRIME_CHNL_OFFSET_LOWER)
            {
                txdesc_scseting = HT_DATA_SC_20_LOWER_OF_40MHZ;
            }
            else
            {
                txdesc_scseting = HT_DATA_SC_DONOT_CARE;
            }
        }
    }
    else
    {
        txdesc_scseting = HT_DATA_SC_DONOT_CARE;
    }

    return txdesc_scseting;
}

static wf_u8 txdesc_bwmapping_get(nic_info_st *nic_info, struct xmit_frame *pxmitframe)
{
    wf_u8 desc_bw_setting = 0;

    if (wf_wlan_get_cur_bw(nic_info)== CHANNEL_WIDTH_40)
    {
        if (pxmitframe->pwdn->bw_mode == CHANNEL_WIDTH_40)
            desc_bw_setting = 1;
        else
            desc_bw_setting = 0;
    }
    else
    {
        desc_bw_setting = 0;
    }

    return desc_bw_setting;
}

static void txdesc_vcs_fill(nic_info_st *nic_info, struct xmit_frame *pxmitframe, wf_u8 * ptxdesc)
{
    wdn_net_info_st *pwdn = pxmitframe->pwdn;

#ifdef CONFIG_RICHV200
    /* set VCS MODE */
    wf_set_bits_to_le_u32(ptxdesc + 8, 27, 2, 0);

    switch (pxmitframe->vcs_mode)
    {
        case RTS_CTS:
            wf_set_bits_to_le_u32(ptxdesc + 8, 27, 2, 1);
            break;
        case CTS_TO_SELF:
            wf_set_bits_to_le_u32(ptxdesc + 8, 27, 2, 2);
            break;
        case NONE_VCS:
        default:
            break;
    }

    if (pxmitframe->vcs_mode)
    {

        if (pwdn->short_preamble == wf_true)
            wf_set_bits_to_le_u32(ptxdesc + 8, 29, 1, 1);

        if (pxmitframe->ht_en)
        {
            wf_set_bits_to_le_u32(ptxdesc + 8, 30, 2, txdesc_scmapping_get(nic_info, pxmitframe));
        }
    }
    else
    {

    }

#else
    WF_TX_DESC_HW_RTS_ENABLE_9086X(ptxdesc, 0);

    switch (pxmitframe->vcs_mode)
    {
        case RTS_CTS:
            WF_TX_DESC_RTS_ENABLE_9086X(ptxdesc, 1);
            break;
        case CTS_TO_SELF:
            WF_TX_DESC_CTS2SELF_9086X(ptxdesc, 1);
            break;
        case NONE_VCS:
        default:
            break;
    }

    if (pxmitframe->vcs_mode)
    {
        WF_TX_DESC_RTSRATE_FB_CFG_9086X(ptxdesc, 1);

        WF_TX_DESC_RTSRATE_CFG_9086X(ptxdesc, 1);

        if (pwdn->short_preamble == wf_true)
            WF_TX_DESC_RTS_SHORT_9086X(ptxdesc, 1);

        if (pxmitframe->ht_en)
        {
            WF_TX_DESC_RTS_SC_9086X(ptxdesc, txdesc_scmapping_get(nic_info, pxmitframe));
        }
    }
    else
    {

    }
#endif
}

static void txdesc_phy_fill(nic_info_st *nic_info, struct xmit_frame *pxmitframe,
                            wf_u8 * ptxdesc)
{

    if (pxmitframe->ht_en)
    {
#ifdef CONFIG_RICHV200
        /* set DBW */
        wf_set_bits_to_le_u32(ptxdesc + 16, 12, 1, txdesc_bwmapping_get(nic_info, pxmitframe));
        /* set DSC */
        wf_set_bits_to_le_u32(ptxdesc + 16, 13, 2, txdesc_scmapping_get(nic_info, pxmitframe));
#else
        WF_TX_DESC_DATA_BW_9086X(ptxdesc, txdesc_bwmapping_get(nic_info, pxmitframe));
        WF_TX_DESC_DATA_SC_9086X(ptxdesc, txdesc_scmapping_get(nic_info, pxmitframe));
#endif
    }
}

static void txdesc_fill(struct xmit_frame *pxmitframe, wf_u8 * pbuf, wf_bool bSendAck)
{
    nic_info_st *nic_info = pxmitframe->nic_info;
    wdn_net_info_st *pwdn = pxmitframe->pwdn;
    hw_info_st *hw_info = nic_info->hw_info;

    if(pxmitframe->frame_tag != DATA_FRAMETAG)
    {
        return;
    }

#ifdef CONFIG_RICHV200
    /* set for data type */
    wf_set_bits_to_le_u32(pbuf, 0, 2, TYPE_DATA);
    /* set mac id or sta index */
    wf_set_bits_to_le_u32(pbuf + 16, 0, 5, pwdn->wdn_id);
    if (pwdn->raid <= 8)
    {
        /* set rate mode, mgmt frame use fix mode */
        wf_set_bits_to_le_u32(pbuf + 16, 5, 1, 0);
        /* set RATE ID, mgmt frame use 802.11 B, the number is raid */
        wf_set_bits_to_le_u32(pbuf + 16, 6, 3, pwdn->raid);
    }
    else
    {
        /* set rate mode, mgmt frame use adp mode */
        wf_set_bits_to_le_u32(pbuf + 16, 5, 1, 1);
        /* set RATE ID, the number is raid - 9 */
        wf_set_bits_to_le_u32(pbuf + 16, 9, 3, pwdn->raid - 9);
    }
    /* set QOS QUEUE  */
    wf_set_bits_to_le_u32(pbuf + 12, 6, 5, pxmitframe->qsel);
    /* set SEQ */
    wf_set_bits_to_le_u32(pbuf, 19, 12, pxmitframe->seqnum);
    /* set secture type */
    wf_set_bits_to_le_u32(pbuf + 12, 3, 3, sectype_to_hwdesc_get(pxmitframe));

    txdesc_vcs_fill(nic_info, pxmitframe, pbuf);

    if ((pxmitframe->ether_type != 0x888e) && (pxmitframe->ether_type != 0x0806) &&
        (pxmitframe->ether_type != 0x88B4) && (pxmitframe->dhcp_pkt != 1) &&
        (hw_info->use_fixRate != wf_true))
    {

        if (pxmitframe->ampdu_en == wf_true)
        {
            /* set AGG Enable */
            wf_set_bits_to_le_u32(pbuf + 12, 0, 1, 1);
        }
        else
        {
            /* set AGG Break */
            wf_set_bits_to_le_u32(pbuf + 12, 19, 1, 1);
        }

        txdesc_phy_fill(nic_info, pxmitframe, pbuf);

        /* set USE_RATE auto */
        wf_set_bits_to_le_u32(pbuf + 8, 16, 1, 0);
    }
    else
    {
        /* set AGG Break */
        wf_set_bits_to_le_u32(pbuf + 12, 19, 1, 1);
        /* set USE_RATE */
        wf_set_bits_to_le_u32(pbuf + 8, 16, 1, 1);

        //txdesc_phy_fill(nic_info, pattrib, pbuf);

        if (pwdn->short_preamble== wf_true)
            wf_set_bits_to_le_u32(pbuf + 8, 17, 1, 1);

        /* set USE_RATE */
        wf_set_bits_to_le_u32(pbuf + 8, 16, 1, 1);

        /* set TX RATE */
        if (hw_info->use_fixRate == wf_true)
        {
            wf_set_bits_to_le_u32(pbuf + 8, 18, 7, wf_mrate_to_hwrate(pwdn->tx_rate));
        }
        else
        {
            wf_set_bits_to_le_u32(pbuf + 8, 18, 7, DESC_RATE1M);
        }
    }
    
#ifdef CONFIG_SOFT_TX_AGGREGATION
    wf_set_bits_to_le_u32(pbuf + 12, 24, 8, pxmitframe->agg_num);
#endif

    if (bSendAck == wf_true)
    {
        /* set SPE_RPT */
        wf_set_bits_to_le_u32(pbuf + 12, 21, 1, 1);
        /* set SW_DEFINE */
        wf_set_bits_to_le_u32(pbuf + 4, 16, 12, pwdn->wdn_xmitpriv.txseq_tid[pxmitframe->qsel]);
    }

    /* set PKT_LEN */
    wf_set_bits_to_le_u32(pbuf + 8, 0, 16, pxmitframe->last_txcmdsz);

    /* set BMC */
    if (pxmitframe->bmcast)
        wf_set_bits_to_le_u32(pbuf + 12, 14, 1, 1);

    /* set HWSEQ_EN */
    if(!pwdn->qos_option)
        wf_set_bits_to_le_u32(pbuf, 18, 1, 1);

#else

    WF_TX_DESC_MACID_9086X(pbuf, pwdn->wdn_id);
    WF_TX_DESC_QUEUE_SEL_9086X(pbuf, pxmitframe->qsel);
    WF_TX_DESC_SEQ_9086X(pbuf, pxmitframe->seqnum);

    WF_TX_DESC_SEC_TYPE_9086X(pbuf, sectype_to_hwdesc_get(pxmitframe));
    txdesc_vcs_fill(nic_info, pxmitframe, pbuf);

    WF_TX_DESC_RATE_ID_9086X(pbuf, pwdn->raid);

    if ((pxmitframe->ether_type == ETHTYPE_EAPOL) || (pxmitframe->ether_type == ETHTYPE_ARP) || (pxmitframe->ether_type == 0x88B4)
        || (pxmitframe->dhcp_pkt == 1) || (hw_info->use_fixRate == wf_true))
    {
        WF_TX_DESC_AGG_BREAK_9086X(pbuf, 1);

        txdesc_phy_fill(nic_info, pxmitframe, pbuf);

        if (pwdn->short_preamble== wf_true)
        {
            WF_TX_DESC_DATA_SHORT_9086X(pbuf, 1);
        }


        WF_TX_DESC_USE_RATE_9086X(pbuf, 1);

        if (hw_info->use_fixRate == wf_true)
        {  
            WF_TX_DESC_TX_RATE_9086X(pbuf, wf_mrate_to_hwrate(pwdn->tx_rate));
        }
        else
        {        
            WF_TX_DESC_TX_RATE_9086X(pbuf, DESC_RATE1M);
        }
    }
    else
    {
        if (pxmitframe->ampdu_en == wf_true)
        {
            WF_TX_DESC_AGG_ENABLE_9086X(pbuf, 1);
            WF_TX_DESC_AGGNUM_CFG_9086X(pbuf, 1);
            WF_TX_DESC_MINISPACE_CFG_9086X(pbuf, 1);
        }
        else
        {
            WF_TX_DESC_AGG_BREAK_9086X(pbuf, 1);
        }

        txdesc_phy_fill(nic_info, pxmitframe, pbuf);

        WF_TX_DESC_DATARATE_FB_CFG_9086X(pbuf, 1);
        WF_TX_DESC_USE_RATE_9086X(pbuf, 0);
    }

#ifdef CONFIG_SOFT_TX_AGGREGATION
    WF_TX_DESC_USB_TXAGG_NUM_9086X(pbuf,pxmitframe->agg_num);
#endif

    if (bSendAck == wf_true)
    {
        WF_TX_DESC_SPE_RPT_9086X(pbuf, 1);
        WF_TX_DESC_SW_DEFINE_9086X(pbuf, pwdn->wdn_xmitpriv.txseq_tid[pxmitframe->qsel]);
    }

    WF_TX_DESC_PKT_SIZE_9086X(pbuf, pxmitframe->last_txcmdsz);

    if (pxmitframe->bmcast)
        WF_TX_DESC_BMC_9086X(pbuf, 1);

    if(!pwdn->qos_option)
        WF_TX_DESC_HWSEQ_EN_9086X(pbuf, 1);
#endif
}


#ifdef CONFIG_RICHV200
void wf_txdesc_chksum(wf_u8 *ptx_desc)
{
    wf_u16 *usPtr = (wf_u16 *) ptx_desc;
    wf_u32 index;
    wf_u16 checksum = 0;

    for (index = 0; index < 9; index++)
        checksum ^= le16_to_cpu(*(usPtr + index));

    wf_set_bits_to_le_u32(ptx_desc + 16, 16, 16, checksum);
}
#else
void wf_txdesc_chksum(struct tx_desc *ptxdesc)
{
    wf_u16 *usPtr = (wf_u16 *) ptxdesc;
    wf_u32 count;
    wf_u32 index;
    wf_u16 checksum = 0;

    ptxdesc->txdw7 &= cpu_to_le32(0xffff0000);

    count = 16;

    for (index = 0; index < count; index++)
        checksum ^= le16_to_cpu(*(usPtr + index));

    ptxdesc->txdw7 |= cpu_to_le32(checksum & 0x0000ffff);
}
#endif

static void txdesc_update(struct xmit_frame *pxmitframe, wf_u8 * pbuf)
{
    nic_info_st *nic_info;
    hw_info_st *hw_info;
	nic_info = pxmitframe->nic_info;
	hw_info  = nic_info->hw_info;
    txdesc_fill(pxmitframe, pbuf, (wf_bool)(hw_info->tx_data_rpt));

#ifdef CONFIG_RICHV200
    wf_txdesc_chksum(pbuf);
#else
    wf_txdesc_chksum((struct tx_desc *)pbuf);
#endif
}

wf_bool wf_tx_txdesc_init(struct xmit_frame *pxmitframe, wf_u8 * pmem, wf_s32 sz,
                          wf_bool bagg_pkt, wf_u8 dum)
{
    wf_bool ret = wf_false;

    struct tx_desc *ptxdesc = (struct tx_desc *)pmem;

    if ((PACKET_OFFSET_SZ != 0)
        && (wf_false == bagg_pkt)
        /*&& (chk_urb_zero_pkt(nic_info, sz) == 0)*/)
    {
        ptxdesc = (struct tx_desc *)(pmem + PACKET_OFFSET_SZ);
        ret = wf_true;
        pxmitframe->pkt_offset--;
    }
    if (dum)
    {
        wf_memset(ptxdesc, 0, TXDESC_OFFSET);
        //wf_memset(ptxdesc, 0, sizeof(struct tx_desc));
        txdesc_update(pxmitframe, (wf_u8 *)ptxdesc);
    }
    return ret;

}

wf_u32 wf_quary_addr(wf_u8 qsel)
{
    wf_u32 addr = 0;

    switch (qsel)
    {
        case 0:
        case 3:
            addr = BE_QUEUE_INX;
            break;
        case 1:
        case 2:
            addr = BK_QUEUE_INX;
            break;
        case 4:
        case 5:
            addr = VI_QUEUE_INX;
            break;
        case 6:
        case 7:
            addr = VO_QUEUE_INX;
            break;
        case QSLT_BEACON:
            addr = BCN_QUEUE_INX;
            break;
        case QSLT_HIGH:
            addr = HIGH_QUEUE_INX;
            break;
        case QSLT_MGNT:
        default:
            addr = MGT_QUEUE_INX;
            break;

    }
    return addr;
}


wf_u8 wf_ra_sGI_get(wdn_net_info_st *pwdn, wf_u8 pad)
{
    wf_u8 sgi = wf_false, sgi_20m = wf_false, sgi_40m = wf_false;

    if (pad)
    {
        sgi_20m = pwdn->htpriv.sgi_20m;
        sgi_40m = pwdn->htpriv.sgi_40m;
    }

    if (pwdn->bw_mode == CHANNEL_WIDTH_40)
    {
        sgi = sgi_40m;
    }
    else
    {
        sgi = sgi_20m;
    }

    return sgi;
}

wf_u8 wf_chk_qos(wf_u8 acm_mask, wf_u8 priority, wf_u8 pad)
{
    wf_u8 change_priority = priority;

    if (pad)
    {
        switch (priority)
        {
            case 0:
            case 3:
                if (acm_mask & WF_BIT(1))
                    change_priority = 1;
                break;
            case 1:
            case 2:
                break;
            case 4:
            case 5:
                if (acm_mask & WF_BIT(2))
                    change_priority = 0;
                break;
            case 6:
            case 7:
                if (acm_mask & WF_BIT(3))
                    change_priority = 5;
                break;
            default:
                LOG_E("[%s]: invalid pattrib->priority: %d!!!",__func__, priority);
                break;
        }
    }

    return change_priority;
}

wf_inline wf_bool wf_need_stop_queue(nic_info_st *nic_info)
{
    tx_info_st *tx_info = nic_info->tx_info;

    if (tx_info->free_xmitframe_cnt <= 1)
    {
        tx_info->xmitFrameCtl = 1;
        return wf_true;
    }

    return wf_false;
}


wf_inline wf_bool wf_need_wake_queue(nic_info_st *nic_info)
{
    tx_info_st *tx_info = nic_info->tx_info;

    if (tx_info->xmitFrameCtl == 1)
    {
        if (tx_info->free_xmitframe_cnt > (NR_XMITFRAME-1))
        {
            tx_info->xmitFrameCtl = 0;
            return wf_true;
        }
    }

    return wf_false;
}

struct xmit_buf *wf_xmit_buf_new(tx_info_st *tx_info)
{
    wf_list_t *plist, *phead;
    struct xmit_buf *pxmitbuf = NULL;
    wf_que_t *pfree_xmitbuf_queue = &tx_info->xmit_buf_queue;

    wf_lock_lock(&pfree_xmitbuf_queue->lock);

    if (wf_list_is_empty(wf_que_list_head(pfree_xmitbuf_queue)) == wf_true)
    {
        pxmitbuf = NULL;
    }
    else
    {
        phead = wf_que_list_head(pfree_xmitbuf_queue);
        plist = wf_list_next(phead);
        pxmitbuf = WF_CONTAINER_OF(plist, struct xmit_buf, list);
        wf_list_delete(&(pxmitbuf->list));
    }

    if (pxmitbuf != NULL)
    {
        tx_info->free_xmitbuf_cnt--;
        pxmitbuf->priv_data = NULL;
        pxmitbuf->pkt_len = 0;
        pxmitbuf->pg_num = 0;
        pxmitbuf->agg_num = 0;
        pxmitbuf->send_flag = 0;
        pxmitbuf->ptail = pxmitbuf->pbuf = (wf_u8 *) WF_N_BYTE_ALIGMENT((SIZE_PTR) (pxmitbuf->pallocated_buf),XMITBUF_ALIGN_SZ);
    }

    wf_lock_unlock(&pfree_xmitbuf_queue->lock);
#if 0
    if(NULL != pxmitbuf)
        LOG_I("[%s] new buffer_id:%d",__func__,pxmitbuf->buffer_id);
#endif
    return pxmitbuf;
}
#ifdef CONFIG_LPS
static struct xmit_buf *__wnew_cmd_txbuf(tx_info_st *tx_info,
        enum cmdbuf_type buf_type)
{
    struct xmit_buf *pxmitbuf = NULL;

    pxmitbuf = &tx_info->pcmd_xmitbuf[buf_type];
    if (pxmitbuf != NULL)
    {
        pxmitbuf->priv_data = NULL;
    }
    else
    {
        LOG_I("%s fail, no xmitbuf available", __func__);
    }

    return pxmitbuf;
}

struct xmit_frame* wf_xmit_cmdframe_new(tx_info_st *tx_info,
                                        enum cmdbuf_type buf_type,
                                        u8 tag)
{
    struct xmit_frame *pcmdframe;
    struct xmit_buf *pxmitbuf;

    LOG_I(" func: %s", __func__);
    if ((pcmdframe = wf_xmit_frame_new(tx_info)) == NULL)
    {
        LOG_I("%s, alloc xmitframe fail", __FUNCTION__);
        return NULL;
    }

    if ((pxmitbuf = __wnew_cmd_txbuf(tx_info, buf_type)) == NULL)
    {
        LOG_I("%s, alloc xmitbuf fail", __FUNCTION__);
        wf_xmit_frame_delete(tx_info, pcmdframe);
        return NULL;
    }

    if (tag)
    {
        pcmdframe->frame_tag = MGNT_FRAMETAG;

        pcmdframe->pxmitbuf = pxmitbuf;

        if (pxmitbuf->pbuf == NULL)
        {
            LOG_I(" pxmitbuf->pbuf == NULL");
        }

        pcmdframe->buf_addr = pxmitbuf->pbuf;

        pxmitbuf->priv_data = pcmdframe;
    }
    return pcmdframe;

}
#endif
wf_bool wf_xmit_buf_delete(tx_info_st *tx_info, struct xmit_buf *pxmitbuf)
{
    wf_que_t *pfree_xmitbuf_queue = &tx_info->xmit_buf_queue;

    if ((pxmitbuf == NULL) || (tx_info == NULL))
    {
        return wf_false;
    }

    wf_lock_lock(&pfree_xmitbuf_queue->lock);
    wf_list_delete(&pxmitbuf->list);
    wf_list_insert_tail(&(pxmitbuf->list),
                        wf_que_list_head(pfree_xmitbuf_queue));
    tx_info->free_xmitbuf_cnt++;
    wf_lock_unlock(&pfree_xmitbuf_queue->lock);
    //LOG_I("[%s] free buffer_id:%d",__func__,pxmitbuf->buffer_id);
    return wf_true;
}

struct xmit_buf *wf_xmit_extbuf_new(tx_info_st *tx_info)
{
    wf_list_t *plist, *phead;
    struct xmit_buf *pxmitbuf = NULL;
    wf_que_t *pfree_xmitbuf_queue = &tx_info->xmit_extbuf_queue;

    wf_lock_lock(&pfree_xmitbuf_queue->lock);

    if (wf_list_is_empty(wf_que_list_head(pfree_xmitbuf_queue)) == wf_true)
    {
        pxmitbuf = NULL;
    }
    else
    {
        phead = wf_que_list_head(pfree_xmitbuf_queue);
        plist = wf_list_next(phead);
        pxmitbuf = WF_CONTAINER_OF(plist, struct xmit_buf, list);
        wf_list_delete(&(pxmitbuf->list));
    }

    if (pxmitbuf != NULL)
    {
        tx_info->free_xmit_extbuf_cnt--;
        pxmitbuf->priv_data = NULL;
        pxmitbuf->pkt_len = 0;
        pxmitbuf->pg_num = 0;
        pxmitbuf->agg_num = 0;
        pxmitbuf->send_flag = 0;
    }

    wf_lock_unlock(&pfree_xmitbuf_queue->lock);
    return pxmitbuf;
}

wf_bool wf_xmit_extbuf_delete(tx_info_st *tx_info, struct xmit_buf *pxmitbuf)
{
    wf_que_t *pfree_xmitbuf_queue = &tx_info->xmit_extbuf_queue;

    if ((pxmitbuf == NULL) || (tx_info == NULL))
    {
        LOG_I("[%s]: tx_info or xmit_buf is NULL", __func__);
        return wf_false;
    }

    wf_lock_lock(&pfree_xmitbuf_queue->lock);
    wf_list_delete(&pxmitbuf->list);
    wf_list_insert_tail(&(pxmitbuf->list),
                        wf_que_list_head(pfree_xmitbuf_queue));
    tx_info->free_xmit_extbuf_cnt++;
    wf_lock_unlock(&pfree_xmitbuf_queue->lock);

    return wf_true;
}

struct xmit_frame *wf_xmit_frame_new(tx_info_st *tx_info)
{

    wf_list_t *plist, *phead;
    struct xmit_frame *pxframe = NULL;
    wf_que_t *pfree_xmit_queue = &tx_info->xmit_frame_queue;

    wf_lock_lock(&pfree_xmit_queue->lock);

    if (wf_list_is_empty(wf_que_list_head(pfree_xmit_queue)) == wf_true)
    {
#if 0
        LOG_E("%s failed: free_xmitframe_cnt=%d", __func__,
              tx_info->free_xmitframe_cnt);
#endif
        pxframe = NULL;
    }
    else
    {
        phead = wf_que_list_head(pfree_xmit_queue);

        plist = wf_list_next(phead);

        pxframe = WF_CONTAINER_OF(plist, struct xmit_frame, list);

        wf_list_delete(&(pxframe->list));
        tx_info->free_xmitframe_cnt--;

#if 0
        LOG_D("%s ok:free_xmitframe_cnt=%d", __func__,
              tx_info->free_xmitframe_cnt);
#endif
    }

    if (pxframe != NULL)
    {
        pxframe->buf_addr = NULL;
        pxframe->pxmitbuf = NULL;

        pxframe->frame_tag = DATA_FRAMETAG;

        pxframe->pkt = NULL;
        pxframe->pkt_offset = (PACKET_OFFSET_SZ / 8);

#ifdef CONFIG_SOFT_TX_AGGREGATION
        pxframe->agg_num = 1;
#endif
#ifdef CONFIG_XMIT_ACK
        pxframe->ack_report = 0;
#endif
    }

    wf_lock_unlock(&pfree_xmit_queue->lock);

    return pxframe;
}

wf_bool wf_xmit_frame_delete(tx_info_st *tx_info, struct xmit_frame * pxmitframe)
{
    wf_que_t *free_queue = NULL;
    wf_que_t *queue = NULL;

    if (pxmitframe == NULL)
    {
        LOG_E("[%s]:pxmitframe==NULL!!!", __func__);
        return wf_false;
    }

    queue = &tx_info->xmit_frame_queue;
    wf_lock_lock(&tx_info->pending_lock);
    wf_list_delete(&pxmitframe->list);
    tx_info->pending_frame_cnt--;
    wf_lock_unlock(&tx_info->pending_lock);
    free_queue = &tx_info->xmit_frame_queue;
    wf_lock_lock(&free_queue->lock);
    wf_list_insert_tail(&pxmitframe->list, wf_que_list_head(free_queue));
    tx_info->free_xmitframe_cnt++;
    wf_lock_unlock(&free_queue->lock);

    return wf_true;
}

wf_bool wf_xmit_frame_enqueue(tx_info_st *tx_info, struct xmit_frame * pxmitframe)
{
    wf_que_t *queue = NULL;

    if (pxmitframe == NULL)
    {
        LOG_E("[%s]:pxmitframe==NULL!!!", __func__);
        return wf_false;
    }

    queue = &tx_info->xmit_frame_queue;

    wf_lock_lock(&queue->lock);
    wf_list_insert_tail(&pxmitframe->list, wf_que_list_head(queue));
    tx_info->free_xmitframe_cnt++;
    wf_lock_unlock(&queue->lock);

    return wf_true;
}



void wf_tx_data_enqueue_tail(tx_info_st *tx_info, struct xmit_frame *pxmitframe)
{
    wf_list_t *phead;

    wf_lock_lock(&tx_info->pending_lock);
    phead = wf_que_list_head(&tx_info->pending_frame_queue);
    wf_list_insert_tail(&pxmitframe->list, phead);
    tx_info->pending_frame_cnt++;
    wf_lock_unlock(&tx_info->pending_lock);
}

void wf_tx_data_enqueue_head(tx_info_st *tx_info, struct xmit_frame *pxmitframe)
{
    wf_list_t *phead;

    wf_lock_lock(&tx_info->pending_lock);
    phead = wf_que_list_head(&tx_info->pending_frame_queue);
    wf_list_insert_head(&pxmitframe->list, phead);
    tx_info->pending_frame_cnt++;
    wf_lock_unlock(&tx_info->pending_lock);
}


struct xmit_frame *wf_tx_data_getqueue(tx_info_st *tx_info)
{
    wf_list_t *plist, *phead;
    struct xmit_frame *pxframe;

    if (wf_que_is_empty(&tx_info->pending_frame_queue) == wf_true)
        return NULL;

    wf_lock_lock(&tx_info->pending_lock);
    phead = wf_que_list_head(&tx_info->pending_frame_queue);
    plist = wf_list_next(phead);
    pxframe = WF_CONTAINER_OF(plist, struct xmit_frame, list);
    wf_lock_unlock(&tx_info->pending_lock);

    return pxframe;
}

struct xmit_frame *wf_tx_data_dequeue(tx_info_st *tx_info)
{
    wf_list_t *plist, *phead;
    struct xmit_frame *pxframe;

    if (wf_que_is_empty(&tx_info->pending_frame_queue) == wf_true)
        return NULL;

    wf_lock_lock(&tx_info->pending_lock);
    phead = wf_que_list_head(&tx_info->pending_frame_queue);
    plist = wf_list_next(phead);
    pxframe = WF_CONTAINER_OF(plist, struct xmit_frame, list);
    tx_info->pending_frame_cnt--;
    wf_list_delete(&pxframe->list);
    wf_lock_unlock(&tx_info->pending_lock);

    return pxframe;
}


void wf_tx_agg_enqueue_head(tx_info_st *tx_info, struct xmit_frame *pxmitframe)
{
    wf_list_t  *phead;

    wf_lock_lock(&tx_info->agg_frame_queue.lock);
    phead = wf_que_list_head(&tx_info->agg_frame_queue);
    wf_list_insert_head(&pxmitframe->list, phead);
    tx_info->agg_frame_queue.cnt++;
    wf_lock_unlock(&tx_info->agg_frame_queue.lock);
}

struct xmit_frame *wf_tx_agg_dequeue(tx_info_st *tx_info)
{
    wf_list_t *plist, *phead;
    struct xmit_frame *pxframe;

    if (wf_que_is_empty(&tx_info->agg_frame_queue) == wf_true)
        return NULL;

    wf_lock_lock(&tx_info->agg_frame_queue.lock);
    phead = wf_que_list_head(&tx_info->agg_frame_queue);
    plist = wf_list_next(phead);
    pxframe = WF_CONTAINER_OF(plist, struct xmit_frame, list);
    tx_info->agg_frame_queue.cnt--;
    wf_list_delete(&pxframe->list);
    wf_lock_unlock(&tx_info->agg_frame_queue.lock);

    return pxframe;
}



int wf_nic_beacon_xmit(nic_info_st *nic_info, struct xmit_buf *pxmitbuf, wf_u16 len)
{
    wf_u8 *pbuf;
    wf_u8 *pwlanhdr;

    #ifndef CONFIG_RICHV200
    struct tx_desc *ptxdesc = NULL;
    #endif
    
    tx_info_st *tx_info = nic_info->tx_info;

    if(pxmitbuf == NULL)
    {
        LOG_E("[%s]: xmitbuf is NULL", __func__);
        return -1;
    }

    if(WF_CANNOT_RUN(nic_info))
    {
        wf_xmit_extbuf_delete(tx_info, pxmitbuf);
        return -1;
    }

#ifdef CONFIG_RICHV200
    // add txd
    pbuf = pxmitbuf->pbuf;
    pwlanhdr = pbuf + TXDESC_OFFSET_NEW;
    wf_memset(pbuf, 0, TXDESC_OFFSET_NEW);

    /* set for data type */
    wf_set_bits_to_le_u32(pbuf, 0, 2, TYPE_DATA);
    /* set HWSEQ_EN */
    wf_set_bits_to_le_u32(pbuf, 18, 1, 1);
    /* set PKT_LEN */
    wf_set_bits_to_le_u32(pbuf + 8, 0, 16, len);
    /* set USE_RATE */
    wf_set_bits_to_le_u32(pbuf + 8, 16, 1, 1);
    /* set TX RATE */
    wf_set_bits_to_le_u32(pbuf + 8, 18, 7, DESC_RATE1M);
    /* set QOS QUEUE, must bcn queue */
    wf_set_bits_to_le_u32(pbuf + 12, 11, 3, 1);
    wf_set_bits_to_le_u32(pbuf + 12, 6, 5, 0); //QSLT_MGNT);

    if(nic_info->nic_num == 1)
    {
        /* set MBSSID */
        wf_set_bits_to_le_u32(pbuf + 12, 18, 1, 1);
        /* set mac id or sta index */
        wf_set_bits_to_le_u32(pbuf + 16, 0, 5, 1);
        /* set SEQ */
        wf_set_bits_to_le_u32(pbuf, 19, 12, 1);
    }
    else
    {
        /* set MBSSID */
        wf_set_bits_to_le_u32(pbuf + 12, 18, 1, 0);
        /* set mac id or sta index */
        wf_set_bits_to_le_u32(pbuf + 16, 0, 5, 0);
        /* set SEQ */
        wf_set_bits_to_le_u32(pbuf, 19, 12, 0);
    }

    /* set RETRY_LIMIT_EN */
    wf_set_bits_to_le_u32(pbuf + 12, 15, 1, 1);
    /* set DATA_RETRY_LIMIT */
    wf_set_bits_to_le_u32(pbuf + 12, 16, 2, 0);
    /* set rate mode, mgmt frame use fix mode */
    wf_set_bits_to_le_u32(pbuf + 16, 5, 1, 0);
    /* set RATE ID, mgmt frame use 802.11 B, the number is 0 */
    wf_set_bits_to_le_u32(pbuf + 16, 6, 3, 0);
    
    /* set DBW */
    wf_set_bits_to_le_u32(pbuf + 16, 12, 1, CHANNEL_WIDTH_20);
    /* set DSC */
    wf_set_bits_to_le_u32(pbuf + 16, 13, 2, HT_DATA_SC_DONOT_CARE);

    /* set BMC */
    if (IS_MCAST(GetAddr1Ptr(pwlanhdr)))
        wf_set_bits_to_le_u32(pbuf + 12, 14, 1, 1);

    // add txd checksum
    wf_txdesc_chksum(pbuf);

    len += TXDESC_OFFSET_NEW;

#else

    // add txd
    pbuf = pxmitbuf->pbuf;
    pwlanhdr = pbuf + sizeof(struct tx_desc);
    ptxdesc = (struct tx_desc *)pbuf;
    wf_memset(ptxdesc, 0, sizeof(struct tx_desc));

    WF_TX_DESC_QUEUE_SEL_9086X(pbuf, QSLT_BEACON);

    WF_TX_DESC_RSVD_9086X(pbuf, 0);

    if(nic_info->nic_num == 1)
    {
        WF_TX_DESC_MBSSID_9086X(pbuf, 1);
    }
    else
    {
        WF_TX_DESC_MBSSID_9086X(pbuf, 0);
    }


    WF_TX_DESC_RETRY_LIMIT_ENABLE_9086X(pbuf, 1);
    WF_TX_DESC_DATA_RETRY_LIMIT_9086X(pbuf, 0);

    if(nic_info->nic_num == 1)
    {
        WF_TX_DESC_MACID_9086X(pbuf, 1);
        WF_TX_DESC_SEQ_9086X(pbuf, 1);
    }
    else
    {
        WF_TX_DESC_MACID_9086X(pbuf, 0);
        WF_TX_DESC_SEQ_9086X(pbuf, 0);
    }

    WF_TX_DESC_DATA_BW_9086X(pbuf, CHANNEL_WIDTH_20);
    WF_TX_DESC_DATA_SC_9086X(pbuf, HT_DATA_SC_DONOT_CARE);

    WF_TX_DESC_USE_RATE_9086X(pbuf, 1);
    WF_TX_DESC_RATE_ID_9086X(pbuf, RATEID_IDX_B);
    WF_TX_DESC_TX_RATE_9086X(pbuf, DESC_RATE1M);

    WF_TX_DESC_PKT_SIZE_9086X(pbuf, len);

    if (IS_MCAST(GetAddr1Ptr(pwlanhdr)))
        WF_TX_DESC_BMC_9086X(pbuf, 1);

    WF_TX_DESC_HWSEQ_EN_9086X(pbuf, 1);


    wf_txdesc_chksum(ptxdesc);
#endif

    len += sizeof(struct tx_desc);
#if 0
    LOG_I("[%s] txd data:",__func__);
    {
        int i = 0;
        for(i=0; i<10; i++)
        {
            printk("0x%x ",((int*)pbuf)[i]);
        }
        printk("\n");
    }
#endif
    pxmitbuf->pg_num   += (len+127)/128;
    LOG_I("[%s,%d] buffer_id:%d, pg_num:%d",__func__,__LINE__,(int)pxmitbuf->buffer_id,pxmitbuf->pg_num);
    // xmit the frame

    wf_io_write_data(nic_info,1, (char *)pbuf, len, wf_quary_addr(QSLT_BEACON), (int (*)(void *, void *))wf_xmit_extbuf_delete, tx_info, pxmitbuf);

    return 0;
}

int wf_nic_mgmt_frame_xmit (nic_info_st *nic_info, wdn_net_info_st *wdn,
                            struct xmit_buf *pxmitbuf, wf_u16 len)
{
    wf_u8 *pbuf;
    wf_u8 *pwlanhdr;
#ifndef CONFIG_RICHV200
    struct tx_desc *ptxdesc;
#endif
    tx_info_st *tx_info = nic_info->tx_info;
    
    if(pxmitbuf == NULL)
    {
        LOG_E("[%s]: xmitbuf is NULL", __func__);
        return -1;
    }

    if(WF_CANNOT_RUN(nic_info))
    {
        wf_xmit_extbuf_delete(tx_info, pxmitbuf);
        return -1;
    }

#ifdef CONFIG_RICHV200
    // add txd
    pbuf = pxmitbuf->pbuf;
    pwlanhdr = pbuf + TXDESC_OFFSET_NEW;
    wf_memset(pbuf, 0, TXDESC_OFFSET_NEW);

    /* set for data type */
    wf_set_bits_to_le_u32(pbuf, 0, 2, TYPE_DATA);
    /* set HWSEQ_EN */
    //wf_set_bits_to_le_u32(pbuf, 18, 1, 1);
    /* set SEQ */
    wf_set_bits_to_le_u32(pbuf, 19, 12, GetSequence(pwlanhdr));
    /* set PKT_LEN */
    wf_set_bits_to_le_u32(pbuf + 8, 0, 16, len);
    /* set USE_RATE */
    wf_set_bits_to_le_u32(pbuf + 8, 16, 1, 1);
    /* set DATA LONG or SHORT*/

    /* set TX RATE */
    wf_set_bits_to_le_u32(pbuf + 8, 18, 7, DESC_RATE1M);
    /* set QOS QUEUE, must mgmt queue */
    wf_set_bits_to_le_u32(pbuf + 12, 12, 1, 1);
    wf_set_bits_to_le_u32(pbuf + 12, 6, 5, 0); //QSLT_MGNT);
    /* set MBSSID */
    wf_set_bits_to_le_u32(pbuf + 12, 18, 1, 0);
    /* set RETRY_LIMIT_EN */
    wf_set_bits_to_le_u32(pbuf + 12, 15, 1, 1);
    /* set DATA_RETRY_LIMIT */
    wf_set_bits_to_le_u32(pbuf + 12, 16, 2, 0);
    /* set rate mode, mgmt frame use fix mode */
    wf_set_bits_to_le_u32(pbuf + 16, 5, 1, 0);
    /* set RATE ID, mgmt frame use 802.11 B, the number is 0 */
    wf_set_bits_to_le_u32(pbuf + 16, 6, 3, 0);
    /* set mac id or sta index */
    wf_set_bits_to_le_u32(pbuf + 16, 0, 5, 0x01);

    if (wf_wlan_get_cur_bw(nic_info) == CHANNEL_WIDTH_40)
    {
        /* set DBW */
        wf_set_bits_to_le_u32(pbuf + 16, 12, 1, CHANNEL_WIDTH_20);

        if (wdn == NULL)
        {
            LOG_E("No wdn only can use 20MHz BW!!!");
            wf_xmit_extbuf_delete(tx_info, pxmitbuf);
            return -1;
        }
        else
        {
            if (wdn->channle_offset == HAL_PRIME_CHNL_OFFSET_UPPER)
            {
                /* set DSC */
                wf_set_bits_to_le_u32(pbuf + 16, 13, 2, HT_DATA_SC_20_UPPER_OF_40MHZ);
            }
            else if (wdn->channle_offset == HAL_PRIME_CHNL_OFFSET_LOWER)
            {
                /* set DSC */
                wf_set_bits_to_le_u32(pbuf + 16, 13, 2, HT_DATA_SC_20_LOWER_OF_40MHZ);
            }
            else
            {
                /* set DSC */
                wf_set_bits_to_le_u32(pbuf + 16, 13, 2, HT_DATA_SC_DONOT_CARE);
            }
        }
    }
    else
    {
        /* set DBW */
        wf_set_bits_to_le_u32(pbuf + 16, 12, 1, CHANNEL_WIDTH_20);
        /* set DSC */
        wf_set_bits_to_le_u32(pbuf + 16, 13, 2, HT_DATA_SC_DONOT_CARE);
    }

    /* set BMC */
    if (IS_MCAST(GetAddr1Ptr(pwlanhdr)))
        wf_set_bits_to_le_u32(pbuf + 12, 14, 1, 1);

    // add txd checksum
    wf_txdesc_chksum(pbuf);

    len += TXDESC_OFFSET_NEW;

// #if 1
//     LOG_D("Mgmt frame length is %d,  txd:", len);
//     for(raid=0; raid<len; raid++)
//     {
//         if((raid !=0 ) && (raid % 4) == 0)
//         {
//             printk("\n");
//         }
//         printk("%02x  ", pbuf[raid]);
//     }
//     printk("\n");
// #endif

#else
    // add txd
    pbuf = pxmitbuf->pbuf;
    pwlanhdr = pbuf + sizeof(struct tx_desc);
    ptxdesc = (struct tx_desc *)pbuf;
    wf_memset(ptxdesc, 0, sizeof(struct tx_desc));


    WF_TX_DESC_QUEUE_SEL_9086X(pbuf, QSLT_MGNT);

    WF_TX_DESC_RSVD_9086X(pbuf, 0);

    if(nic_info->nic_num == 1)
    {
        WF_TX_DESC_MBSSID_9086X(pbuf, 1);
    }
    else
    {
        WF_TX_DESC_MBSSID_9086X(pbuf, 0);
    }

    WF_TX_DESC_RETRY_LIMIT_ENABLE_9086X(pbuf, 1);
    WF_TX_DESC_DATA_RETRY_LIMIT_9086X(pbuf, 0);


    if (wdn)
    {
        wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT]++;
        wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT] &= 0xFFF;
        WF_TX_DESC_SEQ_9086X(pbuf, wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT]);

        WF_TX_DESC_MACID_9086X(pbuf, wdn->wdn_id);
        WF_TX_DESC_DATA_BW_9086X(pbuf, wdn->bw_mode);

        if (wdn->channle_offset == HAL_PRIME_CHNL_OFFSET_UPPER)
        {
            WF_TX_DESC_DATA_SC_9086X(pbuf, HT_DATA_SC_20_UPPER_OF_40MHZ);
        }
        else if (wdn->channle_offset == HAL_PRIME_CHNL_OFFSET_LOWER)
        {
            WF_TX_DESC_DATA_SC_9086X(pbuf, HT_DATA_SC_20_LOWER_OF_40MHZ);
        }
        else
        {
            WF_TX_DESC_DATA_SC_9086X(pbuf, HT_DATA_SC_DONOT_CARE);
        }

        WF_TX_DESC_USE_RATE_9086X(pbuf, 1);
        WF_TX_DESC_RATE_ID_9086X(pbuf, wdn->raid);
        WF_TX_DESC_TX_RATE_9086X(pbuf, wf_mrate_to_hwrate(wdn->tx_rate));

#if 0
        LOG_D("TXD[MGMT]: mac_id:%d raid:%d qsel:%d seqnum:%d tx_rate:%d\n",
              wdn->wdn_id, wdn->raid, QSLT_MGNT, wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT], wdn->tx_rate);
#endif
    }
    else
    {
        WF_TX_DESC_SEQ_9086X(pbuf, 0);
        WF_TX_DESC_MACID_9086X(pbuf, 0);
        WF_TX_DESC_DATA_BW_9086X(pbuf, CHANNEL_WIDTH_20);
        WF_TX_DESC_DATA_SC_9086X(pbuf, HT_DATA_SC_DONOT_CARE);

        WF_TX_DESC_USE_RATE_9086X(pbuf, 1);
        if(wf_p2p_is_valid(nic_info))
        {
            WF_TX_DESC_RATE_ID_9086X(pbuf, RATEID_IDX_G);
            WF_TX_DESC_TX_RATE_9086X(pbuf, DESC_RATE6M);
        }
        else
        {
            WF_TX_DESC_RATE_ID_9086X(pbuf, RATEID_IDX_B);
            WF_TX_DESC_TX_RATE_9086X(pbuf, DESC_RATE1M);
        }
    }

    WF_TX_DESC_PKT_SIZE_9086X(pbuf, len);

    if (IS_MCAST(GetAddr1Ptr(pwlanhdr)))
        WF_TX_DESC_BMC_9086X(pbuf, 1);


    WF_TX_DESC_HWSEQ_EN_9086X(pbuf, 1);

    // add txd checksum
    if(NIC_USB == nic_info->nic_type)
    {
        wf_txdesc_chksum(ptxdesc);
    }

    len += sizeof(struct tx_desc);
#if 0
    LOG_I("[%s] txd data:",__func__);
    {
        int i = 0;
        for(i=0; i<10; i++)
        {
            printk("0x%x ",((int*)pbuf)[i]);
        }
        printk("\n");
    }
#endif
#endif
    pxmitbuf->pg_num   += (len+127)/128;
    pxmitbuf->qsel      = QSLT_MGNT;

    //LOG_I("[%s,%d] buffer_id:%d, pg_num:%d",__func__,__LINE__,(int)pxmitbuf->buffer_id,pxmitbuf->pg_num);

    // xmit the frame

    wf_io_write_data(nic_info, 1, (char *)pbuf, len, wf_quary_addr(QSLT_MGNT), (int (*)(void *, void *))wf_xmit_extbuf_delete, tx_info, pxmitbuf);
    tx_info->tx_mgnt_pkts += 1;
    return 0;
}



int wf_nic_mgmt_frame_xmit_with_ack(nic_info_st *nic_info, wdn_net_info_st *wdn, struct xmit_buf *pxmitbuf, wf_u16 len)
{
    int ret;
    wf_u8 val;
    wf_u8 *pbuf;
    wf_u8 *pwlanhdr;
#ifndef CONFIG_RICHV200
    struct tx_desc *ptxdesc;
#endif
    wf_u32 timeout = 0;

    tx_info_st *tx_info = nic_info->tx_info;

    if(pxmitbuf == NULL)
    {
        LOG_E("[%s]: xmitbuf is NULL", __func__);
        return -1;
    }

    if(WF_CANNOT_RUN(nic_info))
    {
        wf_xmit_extbuf_delete(tx_info, pxmitbuf);
        return -1;
    }

#ifdef CONFIG_RICHV200
    // add txd
    pbuf = pxmitbuf->pbuf;
    pwlanhdr = pbuf + TXDESC_OFFSET_NEW;
    wf_memset(pbuf, 0, TXDESC_OFFSET_NEW);

    /* set for data type */
    wf_set_bits_to_le_u32(pbuf, 0, 2, TYPE_DATA);
    /* set HWSEQ_EN */
    wf_set_bits_to_le_u32(pbuf, 18, 1, 1);

    /* set QOS QUEUE, must mgmt queue */
    wf_set_bits_to_le_u32(pbuf + 12, 12, 1, 1);
    wf_set_bits_to_le_u32(pbuf + 12, 6, 5, 0); //QSLT_MGNT);

    /* set MBSSID */
    wf_set_bits_to_le_u32(pbuf + 12, 18, 1, 0);
    /* set RETRY_LIMIT_EN */
    wf_set_bits_to_le_u32(pbuf + 12, 15, 1, 1);
    /* set DATA_RETRY_LIMIT */
    wf_set_bits_to_le_u32(pbuf + 12, 16, 2, 0);

    if (wdn)
    {
        wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT]++;
        wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT] &= 0xFFF;
        /* set SEQ */
        wf_set_bits_to_le_u32(pbuf, 19, 12, wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT]);

        /* set mac id or sta index */
        wf_set_bits_to_le_u32(pbuf + 16, 0, 5, wdn->wdn_id);
        /* set DBW */
        wf_set_bits_to_le_u32(pbuf + 16, 12, 1, wdn->bw_mode);

        if (wdn->channle_offset == HAL_PRIME_CHNL_OFFSET_UPPER)
        {
            /* set DSC */
            wf_set_bits_to_le_u32(pbuf + 16, 13, 2, HT_DATA_SC_20_UPPER_OF_40MHZ);
        }
        else if (wdn->channle_offset == HAL_PRIME_CHNL_OFFSET_LOWER)
        {
            /* set DSC */
            wf_set_bits_to_le_u32(pbuf + 16, 13, 2, HT_DATA_SC_20_LOWER_OF_40MHZ);
        }
        else
        {
            /* set DSC */
            wf_set_bits_to_le_u32(pbuf + 16, 13, 2, HT_DATA_SC_DONOT_CARE);
        }
        /* set USE_RATE */
        wf_set_bits_to_le_u32(pbuf + 8, 16, 1, 1);
        /* set rate mode, mgmt frame use fix mode */
        wf_set_bits_to_le_u32(pbuf + 16, 5, 1, 0);
        /* set RATE ID */
        wf_set_bits_to_le_u32(pbuf + 16, 6, 3,  wdn->raid);
        /* set TX RATE */
        wf_set_bits_to_le_u32(pbuf + 8, 18, 7, wf_mrate_to_hwrate(wdn->tx_rate));

        /* set SPE_RPT */
        wf_set_bits_to_le_u32(pbuf + 12, 21, 1, 1);
        /* set SW_DEFINE */
        wf_set_bits_to_le_u32(pbuf + 4, 16, 12, wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT]);

#if 0
        LOG_D("TXD[MGMT]: mac_id:%d raid:%d qsel:%d seqnum:%d tx_rate:%d\n",
              wdn->wdn_id, wdn->raid, QSLT_MGNT, wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT], wdn->tx_rate);
#endif
    }
    else
    {
        /* set SEQ */
        wf_set_bits_to_le_u32(pbuf, 19, 12, 0);
        /* set mac id or sta index */
        wf_set_bits_to_le_u32(pbuf + 16, 0, 5, 0);
        /* set DBW */
        wf_set_bits_to_le_u32(pbuf + 16, 12, 1, CHANNEL_WIDTH_20);
        /* set DSC */
        wf_set_bits_to_le_u32(pbuf + 16, 13, 2, HT_DATA_SC_DONOT_CARE);
        /* set USE_RATE */
        wf_set_bits_to_le_u32(pbuf + 8, 16, 1, 1);
        /* set rate mode, mgmt frame use fix mode */
        wf_set_bits_to_le_u32(pbuf + 16, 5, 1, 0);
        /* set RATE ID, mgmt frame use 802.11 B, the number is 0 */
        wf_set_bits_to_le_u32(pbuf + 16, 6, 3, 0);
        /* set TX RATE */
        wf_set_bits_to_le_u32(pbuf + 8, 18, 7, DESC_RATE1M);
    }

    /* set PKT_LEN */
    wf_set_bits_to_le_u32(pbuf + 8, 0, 16, len);

    /* set BMC */
    if (IS_MCAST(GetAddr1Ptr(pwlanhdr)))
        wf_set_bits_to_le_u32(pbuf + 12, 14, 1, 1);

    // add txd checksum
    wf_txdesc_chksum(pbuf);

    len += TXDESC_OFFSET_NEW;
#else
    // add txd
    pbuf = pxmitbuf->pbuf;
    pwlanhdr = pbuf + sizeof(struct tx_desc);
    ptxdesc = (struct tx_desc *)pbuf;
    wf_memset(ptxdesc, 0, sizeof(struct tx_desc));


    WF_TX_DESC_QUEUE_SEL_9086X(pbuf, QSLT_MGNT);

    WF_TX_DESC_RSVD_9086X(pbuf, 0);

    if(nic_info->nic_num == 1)
    {
        WF_TX_DESC_MBSSID_9086X(pbuf, 1);
    }
    else
    {
        WF_TX_DESC_MBSSID_9086X(pbuf, 0);
    }

    WF_TX_DESC_RETRY_LIMIT_ENABLE_9086X(pbuf, 1);
    WF_TX_DESC_DATA_RETRY_LIMIT_9086X(pbuf, 0);


    if (wdn)
    {
        wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT]++;
        wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT] &= 0xFFF;
        WF_TX_DESC_SEQ_9086X(pbuf, wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT]);

        WF_TX_DESC_MACID_9086X(pbuf, wdn->wdn_id);
        WF_TX_DESC_DATA_BW_9086X(pbuf, wdn->bw_mode);

        if (wdn->channle_offset == HAL_PRIME_CHNL_OFFSET_UPPER)
        {
            WF_TX_DESC_DATA_SC_9086X(pbuf, HT_DATA_SC_20_UPPER_OF_40MHZ);
        }
        else if (wdn->channle_offset == HAL_PRIME_CHNL_OFFSET_LOWER)
        {
            WF_TX_DESC_DATA_SC_9086X(pbuf, HT_DATA_SC_20_LOWER_OF_40MHZ);
        }
        else
        {
            WF_TX_DESC_DATA_SC_9086X(pbuf, HT_DATA_SC_DONOT_CARE);
        }

        WF_TX_DESC_USE_RATE_9086X(pbuf, 1);
        WF_TX_DESC_RATE_ID_9086X(pbuf, RATEID_IDX_B);
        WF_TX_DESC_TX_RATE_9086X(pbuf, DESC_RATE1M);

        WF_TX_DESC_SPE_RPT_9086X(pbuf, 1);
        WF_TX_DESC_SW_DEFINE_9086X(pbuf, wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT]);

        WF_TX_DESC_HWSEQ_EN_9086X(pbuf, 0);

#if 1
        LOG_D("TXD[MGMT]: mac_id:%d raid:%d qsel:%d seqnum:%d tx_rate:%d\n",
              wdn->wdn_id, wdn->raid, QSLT_MGNT, wdn->wdn_xmitpriv.txseq_tid[QSLT_MGNT], wdn->tx_rate);
#endif
    }
    else
    {
        WF_TX_DESC_SEQ_9086X(pbuf, 0);
        WF_TX_DESC_MACID_9086X(pbuf, 0);
        WF_TX_DESC_DATA_BW_9086X(pbuf, CHANNEL_WIDTH_20);
        WF_TX_DESC_DATA_SC_9086X(pbuf, HT_DATA_SC_DONOT_CARE);

        WF_TX_DESC_USE_RATE_9086X(pbuf, 1);
        WF_TX_DESC_RATE_ID_9086X(pbuf, RATEID_IDX_B);
        WF_TX_DESC_TX_RATE_9086X(pbuf, DESC_RATE1M);

        WF_TX_DESC_SPE_RPT_9086X(pbuf, 1);
        WF_TX_DESC_SW_DEFINE_9086X(pbuf, 0);


        WF_TX_DESC_HWSEQ_EN_9086X(pbuf, 1);
    }

    WF_TX_DESC_PKT_SIZE_9086X(pbuf, len);

    if (IS_MCAST(GetAddr1Ptr(pwlanhdr)))
        WF_TX_DESC_BMC_9086X(pbuf, 1);

    // add txd checksum
    wf_txdesc_chksum(ptxdesc);

    len += sizeof(struct tx_desc);
#endif

    pxmitbuf->pg_num   = (len+127)/128;
    //LOG_I("[%s,%d] buffer_id:%d, pg_num:%d",__func__,__LINE__,(int)pxmitbuf->buffer_id,pxmitbuf->pg_num);
    // xmit the frame
    wf_io_write8(nic_info, XMIT_ACK_REG, 0);
    ret = wf_io_write_data(nic_info, 1, (char *)pbuf, len, wf_quary_addr(QSLT_MGNT), (int (*)(void *, void *))wf_xmit_extbuf_delete, tx_info, pxmitbuf);
    if (ret == 0)
    {
        timeout = 0;

        /*wait ack*/
        while(timeout < 2000)
        {
            if(WF_CANNOT_RUN(nic_info))
                return -1;

            val = wf_io_read8(nic_info, XMIT_ACK_REG,NULL);
            if (val)
            {
                ret = 0;
                //LOG_D("Get ACK success, time_cnt:%d",timeout);
                break;
            }
            else
            {
                ret = -1;
            }

            timeout++;
        }
    }

    tx_info->tx_mgnt_pkts += 1;
    return ret;
}


wf_u32 wf_get_wlan_pkt_size(struct xmit_frame *pxmit_frame)
{
    wf_u32 len = 0;

    len = pxmit_frame->hdrlen + pxmit_frame->iv_len;
    len += SNAP_HDR_SIZE + sizeof(wf_u16);
    len += pxmit_frame->pktlen;
    if (pxmit_frame->encrypt_algo == _TKIP_)
    {
        len += 8;
        len += ((wf_false) ? pxmit_frame->icv_len : 0);
    }

    return len;
}



int wf_tx_info_init(nic_info_st *nic_info)
{
    wf_bool res;
    int i;
    tx_info_st *tx_info;
    struct xmit_frame *pxmit_frame;
    struct xmit_buf *pxmit_buf;


    LOG_I("tx_info init");
    tx_info = wf_kzalloc(sizeof(tx_info_st));
    if (tx_info == NULL)
    {
        LOG_E("[%s] malloc tx_info failed", __func__);
        nic_info->tx_info = NULL;
        return -1;
    }

    wf_lock_spin_init(&tx_info->lock);
    /* xmit_frame buffer init */
    wf_que_init(&tx_info->xmit_frame_queue, WF_LOCK_TYPE_IRQ);
#if TX_AGG_QUEUE_ENABLE
    wf_que_init(&tx_info->agg_frame_queue, WF_LOCK_TYPE_IRQ);
#endif
    tx_info->pallocated_frame_buf =
        wf_vmalloc(NR_XMITFRAME * sizeof(struct xmit_frame) + 4);

    if (tx_info->pallocated_frame_buf == NULL)
    {
        tx_info->pxmit_frame_buf = NULL;
        LOG_E("[wf_tx_info_init] alloc xmit_frame buf fail!");
        return -2;
    }
    memset(tx_info->pallocated_frame_buf, 0, NR_XMITFRAME * sizeof(struct xmit_frame) + 4);

    tx_info->pxmit_frame_buf =
        (wf_u8 *) WF_N_BYTE_ALIGMENT((SIZE_PTR) (tx_info->pallocated_frame_buf), 4);

    pxmit_frame = (struct xmit_frame *)tx_info->pxmit_frame_buf;
    for (i = 0; i < NR_XMITFRAME; i++)
    {
        wf_list_init(&(pxmit_frame->list));

        pxmit_frame->frame_tag = NULL_FRAMETAG;
        pxmit_frame->nic_info = nic_info;

        pxmit_frame->pxmitbuf = NULL;
        pxmit_frame->buf_addr = NULL;
        pxmit_frame->pkt = NULL;
        pxmit_frame->frame_id = (wf_u16)(i+1);
        wf_list_insert_tail(&(pxmit_frame->list), &(tx_info->xmit_frame_queue.head));
        //LOG_I("[%d] addr:%p",i,pxmit_frame);
        pxmit_frame++;
    }
    tx_info->free_xmitframe_cnt = NR_XMITFRAME;

    /* xmit_buf buffer init */
    wf_que_init(&tx_info->xmit_buf_queue, WF_LOCK_TYPE_IRQ);
    tx_info->pallocated_xmitbuf =
        wf_vmalloc(XMIT_DATA_BUFFER_CNT * sizeof(struct xmit_buf) + 4);

    if (tx_info->pallocated_xmitbuf == NULL)
    {
        LOG_E("[%s] alloc xmit_buf buf fail!", __func__);
        return -3;
    }
    memset(tx_info->pallocated_xmitbuf, 0, XMIT_DATA_BUFFER_CNT * sizeof(struct xmit_buf) + 4);

    tx_info->pxmitbuf =
        (wf_u8 *) WF_N_BYTE_ALIGMENT((SIZE_PTR) (tx_info->pallocated_xmitbuf), 4);

    pxmit_buf = (struct xmit_buf *)tx_info->pxmitbuf;

    for (i = 0; i < XMIT_DATA_BUFFER_CNT; i++)
    {
        wf_list_init(&pxmit_buf->list);

        pxmit_buf->priv_data = NULL;
        pxmit_buf->nic_info = nic_info;
        pxmit_buf->buffer_id      = (wf_u8)i;
        //LOG_I("[%d] buffer_addr:%p",i,pxmit_buf);
        if ((res = xmit_buf_resource_new(pxmit_buf, nic_info, MAX_XMITBUF_SZ)) == wf_false)
        {
            wf_msleep(10);
            res = xmit_buf_resource_new(pxmit_buf, nic_info, MAX_XMITBUF_SZ);
            if (res == wf_false)
            {
                LOG_E("[%s] no memory for xmit_buf frame buf!", __func__);
                return -4;
            }
        }

        pxmit_buf->flags = XMIT_VO_QUEUE;

        wf_list_insert_tail(&pxmit_buf->list, &(tx_info->xmit_buf_queue.head));

        pxmit_buf++;
    }

    tx_info->free_xmitbuf_cnt = XMIT_DATA_BUFFER_CNT;

    /* mgmt frame xmit_buf buffer init */
    wf_que_init(&tx_info->xmit_extbuf_queue, WF_LOCK_TYPE_IRQ);
    tx_info->pallocated_xmit_extbuf =
        wf_vmalloc(XMIT_MGMT_BUFFER_CNT * sizeof(struct xmit_buf) + 4);

    if (tx_info->pallocated_xmit_extbuf == NULL)
    {
        LOG_E("[%s] alloc xmit_buf buf fail!", __func__);
        return -3;
    }
    memset(tx_info->pallocated_xmit_extbuf, 0, XMIT_MGMT_BUFFER_CNT * sizeof(struct xmit_buf) + 4);

    tx_info->pxmit_extbuf =
        (wf_u8 *) WF_N_BYTE_ALIGMENT((SIZE_PTR) (tx_info->pallocated_xmit_extbuf), 4);

    pxmit_buf = (struct xmit_buf *)tx_info->pxmit_extbuf;

    for (i = 0; i < XMIT_MGMT_BUFFER_CNT; i++)
    {
        wf_list_init(&pxmit_buf->list);

        pxmit_buf->priv_data = NULL;
        pxmit_buf->nic_info = nic_info;

        if ((res = xmit_buf_resource_new(pxmit_buf, nic_info, MAX_XMIT_EXTBUF_SZ)) == wf_false)
        {
            LOG_E("[%s] no memory for xmit_extbuf frame buf!", __func__);
            return -4;
        }

        wf_list_insert_tail(&pxmit_buf->list, &(tx_info->xmit_extbuf_queue.head));

        pxmit_buf++;
    }

    tx_info->free_xmit_extbuf_cnt = XMIT_MGMT_BUFFER_CNT;

#ifdef CONFIG_LPS
    for (i = 0; i < CMDBUF_MAX; i++)
    {
        pxmit_buf = &tx_info->pcmd_xmitbuf[i];
        if (pxmit_buf)
        {
            wf_list_init(&pxmit_buf->list);

            pxmit_buf->priv_data = NULL;
            pxmit_buf->nic_info = nic_info;

            if ((res = xmit_buf_resource_new(pxmit_buf, nic_info, MAX_CMDBUF_SZ)) == wf_false)
            {
                return -5;
            }
            pxmit_buf->alloc_sz = MAX_CMDBUF_SZ + XMITBUF_ALIGN_SZ;
        }
    }
#endif
    /* pending frame queue init */
    wf_que_init(&tx_info->pending_frame_queue, WF_LOCK_TYPE_IRQ);
    wf_lock_init(&tx_info->pending_lock,WF_LOCK_TYPE_IRQ);
    tx_info->pending_frame_cnt= 0;

    tx_info->nic_info = nic_info;
    nic_info->tx_info = tx_info;

    return 0;
}

int wf_tx_info_term(nic_info_st *nic_info)
{
    tx_info_st *tx_info = nic_info->tx_info;
    struct xmit_buf *pxmitbuf;
    struct xmit_frame *pxmitframe;

    LOG_D("[%s] start", __func__);

    if (tx_info)
    {
        int i;
        pxmitbuf = (struct xmit_buf *)tx_info->pxmitbuf;
        pxmitframe = (struct xmit_frame *)tx_info->pxmit_frame_buf;

        pxmitbuf = (struct xmit_buf *)tx_info->pxmitbuf;
        for (i = 0; i < XMIT_DATA_BUFFER_CNT; i++)
        {
            if (pxmitbuf->pallocated_buf)
                wf_kfree(pxmitbuf->pallocated_buf);
            pxmitbuf++;
        }

        pxmitbuf = (struct xmit_buf *)tx_info->pxmit_extbuf;
        for (i = 0; i < XMIT_MGMT_BUFFER_CNT; i++)
        {
            if (pxmitbuf->pallocated_buf)
                wf_kfree(pxmitbuf->pallocated_buf);
            pxmitbuf++;
        }

#ifdef CONFIG_LPS
        pxmitbuf = (struct xmit_buf *)tx_info->pcmd_xmitbuf;
        for (i = 0; i < CMDBUF_MAX; i++)
        {
            if (pxmitbuf->pallocated_buf)
                wf_kfree(pxmitbuf->pallocated_buf);
            pxmitbuf++;
        }
#endif

        if (tx_info->pallocated_frame_buf)
        {
            wf_vfree(tx_info->pallocated_frame_buf);
        }

        if (tx_info->pallocated_xmitbuf)
        {
            wf_vfree(tx_info->pallocated_xmitbuf);
        }

        if (tx_info->pallocated_xmit_extbuf)
        {
            wf_vfree(tx_info->pallocated_xmit_extbuf);
        }

        wf_lock_term(&tx_info->pending_lock);
        wf_lock_spin_free(&tx_info->lock);
        wf_kfree(tx_info);
        nic_info->tx_info = NULL;
    }

    LOG_D("[%s] end", __func__);


    return 0;
}


wf_u8 wf_mrate_to_hwrate(wf_u8 rate)
{
    wf_u8 ret = DESC_RATE1M;

    switch (rate)
    {
        case MGN_1M:
            ret = DESC_RATE1M;
            break;
        case MGN_2M:
            ret = DESC_RATE2M;
            break;
        case MGN_5_5M:
            ret = DESC_RATE5_5M;
            break;
        case MGN_11M:
            ret = DESC_RATE11M;
            break;
        case MGN_6M:
            ret = DESC_RATE6M;
            break;
        case MGN_9M:
            ret = DESC_RATE9M;
            break;
        case MGN_12M:
            ret = DESC_RATE12M;
            break;
        case MGN_18M:
            ret = DESC_RATE18M;
            break;
        case MGN_24M:
            ret = DESC_RATE24M;
            break;
        case MGN_36M:
            ret = DESC_RATE36M;
            break;
        case MGN_48M:
            ret = DESC_RATE48M;
            break;
        case MGN_54M:
            ret = DESC_RATE54M;
            break;

        case MGN_MCS0:
            ret = DESC_RATEMCS0;
            break;
        case MGN_MCS1:
            ret = DESC_RATEMCS1;
            break;
        case MGN_MCS2:
            ret = DESC_RATEMCS2;
            break;
        case MGN_MCS3:
            ret = DESC_RATEMCS3;
            break;
        case MGN_MCS4:
            ret = DESC_RATEMCS4;
            break;
        case MGN_MCS5:
            ret = DESC_RATEMCS5;
            break;
        case MGN_MCS6:
            ret = DESC_RATEMCS6;
            break;
        case MGN_MCS7:
            ret = DESC_RATEMCS7;
            break;
        default:
            break;
    }

    return ret;
}

wf_u8 wf_hwrate_to_mrate(wf_u8 rate)
{
    wf_u8 ret_rate = MGN_1M;

    switch (rate)
    {

        case DESC_RATE1M:
            ret_rate = MGN_1M;
            break;
        case DESC_RATE2M:
            ret_rate = MGN_2M;
            break;
        case DESC_RATE5_5M:
            ret_rate = MGN_5_5M;
            break;
        case DESC_RATE11M:
            ret_rate = MGN_11M;
            break;
        case DESC_RATE6M:
            ret_rate = MGN_6M;
            break;
        case DESC_RATE9M:
            ret_rate = MGN_9M;
            break;
        case DESC_RATE12M:
            ret_rate = MGN_12M;
            break;
        case DESC_RATE18M:
            ret_rate = MGN_18M;
            break;
        case DESC_RATE24M:
            ret_rate = MGN_24M;
            break;
        case DESC_RATE36M:
            ret_rate = MGN_36M;
            break;
        case DESC_RATE48M:
            ret_rate = MGN_48M;
            break;
        case DESC_RATE54M:
            ret_rate = MGN_54M;
            break;
        case DESC_RATEMCS0:
            ret_rate = MGN_MCS0;
            break;
        case DESC_RATEMCS1:
            ret_rate = MGN_MCS1;
            break;
        case DESC_RATEMCS2:
            ret_rate = MGN_MCS2;
            break;
        case DESC_RATEMCS3:
            ret_rate = MGN_MCS3;
            break;
        case DESC_RATEMCS4:
            ret_rate = MGN_MCS4;
            break;
        case DESC_RATEMCS5:
            ret_rate = MGN_MCS5;
            break;
        case DESC_RATEMCS6:
            ret_rate = MGN_MCS6;
            break;
        case DESC_RATEMCS7:
            ret_rate = MGN_MCS7;
            break;

        default:
            LOG_E("[%s]: Non supported Rate [%x]!!!", __func__, rate);
            break;
    }

    return ret_rate;
}


wf_bool wf_tx_msdu_to_mpdu(nic_info_st *nic_info, struct xmit_frame *pxmitframe, wf_u8 *msdu_buf, int msdu_len)
{
    SIZE_PTR addr;
    wf_u8 hw_hdr_offset;
    wf_u8 *pbuf_start;
    wf_u8 *pframe, *mem_start;
    wf_s32 frg_inx, frg_len, mpdu_len, llc_sz, mem_sz;
    hw_info_st *hw_info = nic_info->hw_info;
    int msduOffset = 0;
    int msduRemainLen = 0;

    WF_ASSERT(pxmitframe != NULL);

    if (pxmitframe->buf_addr == NULL)
    {
        LOG_E("[%s]: xmit_buf->buf_addr==NULL", __func__);
        return wf_false;
    }

    pbuf_start = pxmitframe->buf_addr;

#ifdef CONFIG_SOFT_TX_AGGREGATION
    hw_hdr_offset = TXDESC_SIZE + (pxmitframe->pkt_offset * PACKET_OFFSET_SZ);
#else
    hw_hdr_offset = TXDESC_OFFSET;
#endif

    mem_start = pbuf_start + hw_hdr_offset; /* point to wlan head(skip TXD filed) */

    /* fill wlan head filed */
    if (tx_mac_hdr_build(nic_info, pxmitframe, msdu_buf, mem_start) == wf_false)
    {
        LOG_I("[%s]: do_wlanhdr_build fail; drop pkt", __func__);
        return wf_false;
    }

    pxmitframe->pwlanhdr = (wf_80211_data_t *)mem_start;
    msduOffset += WF_ETH_HLEN;

    frg_inx = 0;
    frg_len = hw_info->frag_thresh;

    while (1)
    {
        llc_sz = 0;

        mpdu_len = frg_len;

        pframe = mem_start;

        SetMFrag(mem_start);

        pframe += pxmitframe->hdrlen; /* point to (iv+llc+msdu) filed */
        mpdu_len -= pxmitframe->hdrlen;
        //LOG_I("[%s]: hdrlen:%d mpdu_len:%d",__func__, pxmitframe->hdrlen, mpdu_len);

        /* fill iv filed */
        if (pxmitframe->iv_len)
        {
            wf_memcpy(pframe, pxmitframe->iv, pxmitframe->iv_len);
            pframe += pxmitframe->iv_len;
            mpdu_len -= pxmitframe->iv_len;
        }
        //LOG_I("[%s]: iv_len:%d mpdu_len:%d",__func__, pattrib->iv_len, mpdu_len);

        /* fill llc head filed if first fragment */
        if (frg_inx == 0)
        {
            llc_sz = tx_set_snap(pxmitframe->ether_type, pframe);
            pframe += llc_sz;
            mpdu_len -= llc_sz;
        }
        //LOG_I("[%s]: llc_sz:%d mpdu_len:%d",__func__, llc_sz, mpdu_len);

        /* fill fragment msdu filed */
        if (pxmitframe->icv_len && pxmitframe->bswenc)
        {
            mpdu_len -= pxmitframe->icv_len; /* get fragment length(without icv) */
        }
        //LOG_I("[%s]: icv_len:%d mpdu_len:%d",__func__, pxmitframe->icv_len, mpdu_len);


        if (pxmitframe->bmcast)
        {
            /* don't do fragment to broadcat/multicast packets */
            wf_memcpy(pframe, msdu_buf + msduOffset, msdu_len - msduOffset);
            mem_sz = msdu_len - msduOffset;

            //LOG_I("[%s]: msduOffset:%d mem_sz:%d", __func__, msduOffset, mem_sz);
        }
        else
        {
            msduRemainLen = msdu_len - msduOffset;
            if (msduRemainLen > mpdu_len)
            {
                wf_memcpy(pframe, msdu_buf + pxmitframe->pkt_hdrlen, mpdu_len);
                msduOffset += mpdu_len;
                mem_sz = mpdu_len;
            }
            else
            {
                wf_memcpy(pframe, msdu_buf + pxmitframe->pkt_hdrlen, msduRemainLen);
                msduOffset += msduRemainLen;
                mem_sz = msduRemainLen;
            }

            //LOG_I("[%s]: msduOffset:%d mem_sz:%d", __func__, msduOffset, mem_sz);
        }
        pframe += mem_sz;

        /* skip icv field */
        if (pxmitframe->icv_len && pxmitframe->bswenc)
        {
            pframe += pxmitframe->icv_len;
        }

        frg_inx++;

        if (pxmitframe->bmcast || (msduOffset == msdu_len))
        {
            /* retrive frags number */
            pxmitframe->nr_frags = (wf_u8)frg_inx;
            /* calculate last fragment size */
            pxmitframe->last_txcmdsz = pxmitframe->hdrlen + pxmitframe->iv_len + llc_sz +
                                       mem_sz + (pxmitframe->bswenc ? pxmitframe->icv_len : 0);
            /* last fragment, clear the more data flag */
            ClearMFrag(mem_start);
            break;
        }
        else
        {
            LOG_D("[%s]: There're still something in packet!", __func__);
        }

        addr = (SIZE_PTR)(pframe);
        mem_start = (unsigned char *)WF_RND4(addr) + hw_hdr_offset; /* adjust next point,
                                                                       should jump tx descrption head */
        /* copy wlan head, use the same as first fragment head */
        wf_memcpy(mem_start, pbuf_start + hw_hdr_offset, pxmitframe->hdrlen);
    }

    if (frame_txp_addmic(nic_info, pxmitframe) == wf_false)
    {
        LOG_I("[%s]: frame_txp_addmic return false", __func__);
        return wf_false;
    }

    if (pxmitframe->bmcast == wf_false)
        xmit_frame_vcs_init(nic_info, pxmitframe);
    else
        pxmitframe->vcs_mode = NONE_VCS;

    return wf_true;
}



int wf_tx_msdu(nic_info_st *nic_info, wf_u8 *msdu_buf, int msdu_len, void *pkt)
{
    wf_s32 res;
    tx_info_st *ptx_info = nic_info->tx_info;
    struct xmit_frame *pxmitframe = NULL;
    pxmitframe = wf_xmit_frame_new(ptx_info);
    if (pxmitframe == NULL)
    {
        LOG_E("[%s]: no more pxmitframe", __func__);
        return -1;
    }
    res = wf_xmit_frame_init(nic_info, pxmitframe, msdu_buf, msdu_len);
    if (res == wf_false)
    {
        LOG_W("[%s]: xmit frame info init fail", __func__);
        wf_xmit_frame_enqueue(ptx_info, pxmitframe);
        return -1;
    }

    pxmitframe->pkt = pkt;

    wf_tx_data_enqueue_tail(ptx_info, pxmitframe);

    return 0;
}


wf_bool wf_tx_data_check(nic_info_st *nic_info)
{
    tx_info_st *ptx_info = nic_info->tx_info;
    hw_info_st *phw_info = nic_info->hw_info;
    mlme_info_t *mlme_info = nic_info->mlme_info;
    local_info_st * plocal = (local_info_st *)nic_info->local_info;

    if(nic_info->is_up == 0)
    {
        goto tx_drop;
    }

    if (phw_info->mp_mode)
    {
        LOG_I("mp mode will drop the tx frame");
        goto tx_drop;
    }

    if(plocal->work_mode == WF_INFRA_MODE)
    {
        if(mlme_info->connect == wf_false)
        {
            goto tx_drop;
        }
    }
#ifdef CFG_ENABLE_AP_MODE
    else if(plocal->work_mode == WF_MASTER_MODE)
    {
        if(wf_ap_status_get(nic_info) == WF_AP_STATUS_UNINITILIZED)
        {
            goto tx_drop;
        }
    }
#endif
#if 0
    if (mlme_info->state != MLME_STATE_IDLE)
        goto tx_drop;
#endif

    return wf_true;

tx_drop:
    ptx_info->tx_drop++;
    // LOG_W("[%s,%d] tx_drop",__func__,__LINE__);

    return wf_false;
}


#ifdef CONFIG_SOFT_TX_AGGREGATION

void wf_tx_agg_num_fill(wf_u16 agg_num, wf_u8 * pbuf)
{
#ifdef CONFIG_RICHV200
    wf_set_bits_to_le_u32(pbuf + 12, 24, 8, agg_num);
    // recalc txd checksum
    wf_txdesc_chksum(pbuf);
#else
    WF_TX_DESC_USB_TXAGG_NUM_9086X(pbuf, agg_num);
    // recalc txd checksum
    wf_txdesc_chksum((struct tx_desc *)pbuf);
#endif
}

wf_u32 wf_nic_get_tx_max_len(nic_info_st *nic_info,struct xmit_frame *pxmitframe)
{
    return MAX_XMITBUF_SZ;
}

int wf_nic_tx_qsel_check(wf_u8 pre_qsel, wf_u8 next_qsel)
{
    int chk_rst = WF_RETURN_OK;
    if (((pre_qsel == QSLT_HIGH) || ((next_qsel == QSLT_HIGH)))
        && (pre_qsel != next_qsel))
    {
        chk_rst = WF_RETURN_FAIL;
    }
    return chk_rst;
}

int check_agg_condition(nic_info_st *nic_info,struct xmit_buf *pxmitbuf)
{
    return 0;
}
#endif

void wf_tx_xmit_stop(nic_info_st *nic_info)
{
    tx_info_st *ptx_info = nic_info->tx_info;

    wf_lock_bh_lock(&ptx_info->lock);
    ptx_info->xmit_stop_flag++;
    wf_lock_bh_unlock(&ptx_info->lock);
}

void wf_tx_xmit_start(nic_info_st *nic_info)
{
    tx_info_st *ptx_info = nic_info->tx_info;

    wf_lock_bh_lock(&ptx_info->lock);
    if(ptx_info->xmit_stop_flag > 0)
       ptx_info->xmit_stop_flag--;
    wf_lock_bh_unlock(&ptx_info->lock);
    wf_io_tx_xmit_wake(nic_info);
}

void wf_tx_xmit_pending_queue_clear(nic_info_st *nic_info)
{
    tx_info_st *tx_info = nic_info->tx_info;
    struct xmit_frame *pxmitframe;

    while (wf_que_is_empty(&tx_info->pending_frame_queue) == wf_false)
    {
        pxmitframe = wf_tx_data_getqueue(tx_info);
        wf_xmit_frame_delete(tx_info, pxmitframe);
        if (pxmitframe->pkt)
        {
            wf_free_skb(pxmitframe->pkt);
            pxmitframe->pkt = NULL;
        }
    }
}

// judge hif queue is empty
int wf_tx_xmit_hif_queue_empty(nic_info_st *nic_info)
{
    return ((wf_io_write_data_queue_check(nic_info) == wf_true) &&
           (wf_mcu_check_tx_buff(nic_info) == WF_RETURN_OK));
}

