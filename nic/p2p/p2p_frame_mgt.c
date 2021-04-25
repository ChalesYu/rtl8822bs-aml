#include "common.h"
#include "wf_debug.h"

#ifdef CONFIG_P2P


#define wf_memmove memmove
#define WF_GET_LE16(a) ((wf_u16) (((a)[1] << 8) | (a)[0]))
#define WF_PUT_LE16(a, val) \
    do {    \
        (a)[1] = ((wf_u16) (val)) >> 8;\
        (a)[0] = ((wf_u16) (val)) & 0xff;\
    } while (0)

#define WF_GET_BE16(a) ((wf_u16) (((a)[0] << 8) | (a)[1]))
#define WF_PUT_BE16(a, val)         \
        do {                    \
            (a)[0] = ((wf_u16) (val)) >> 8;    \
            (a)[1] = ((wf_u16) (val)) & 0xff;  \
        } while (0)

#define WF_GET_BE32(a) ((((wf_u32) (a)[0]) << 24) | (((wf_u32) (a)[1]) << 16) | \
                         (((wf_u32) (a)[2]) << 8) | ((wf_u32) (a)[3]))
#define WF_PUT_BE32(a, val)                 \
                do {                            \
                    (a)[0] = (wf_u8) ((((wf_u32) (val)) >> 24) & 0xff);   \
                    (a)[1] = (wf_u8) ((((wf_u32) (val)) >> 16) & 0xff);   \
                    (a)[2] = (wf_u8) ((((wf_u32) (val)) >> 8) & 0xff);    \
                    (a)[3] = (wf_u8) (((wf_u32) (val)) & 0xff);       \
                } while (0)

#define WLAN_EID_VENDOR_SPECIFIC 221

wf_u8 *p2p_ie_to_get_func(wf_u8 * in_ie, wf_s32 in_len, wf_u8 * p2p_ie, wf_u32 * p2p_ielen)
{
    wf_u32 cnt;
    wf_u8 *p2p_ie_ptr = NULL;
    wf_u8 eid, p2p_oui[4] = { 0x50, 0x6F, 0x9A, 0x09 };

    if (p2p_ielen)
    {
        *p2p_ielen = 0;
    }

    if (!in_ie || in_len < 0)
    {
        LOG_W("[%s,%d] in_len is not right",__func__,__LINE__);
        return p2p_ie_ptr;
    }

    if (in_len <= 0)
    {
        return p2p_ie_ptr;
    }

    cnt = 0;

    while (cnt + 1 + 4 < in_len)
    {
        eid = in_ie[cnt];

        if (cnt + 1 + 4 >= WF_80211_IES_SIZE_MAX)
        {
            LOG_W("[%s,%d] cnt is not right",__func__,__LINE__);
            return NULL;
        }

        if (eid == WLAN_EID_VENDOR_SPECIFIC && wf_memcmp(&in_ie[cnt + 2], p2p_oui, 4) == 0)
        {
            p2p_ie_ptr = in_ie + cnt;

            if (p2p_ie)
            {
                wf_memcpy(p2p_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);
            }

            if (p2p_ielen)
            {
                *p2p_ielen = in_ie[cnt + 1] + 2;
            }

            break;
        }
        else
        {
            cnt += in_ie[cnt + 1] + 2;
        }

    }

    return p2p_ie_ptr;
}

void p2p_ie_dump_func(void *sel, wf_u8 * ie, wf_u32 ie_len, wf_u8 flag)
{
    wf_u8 *pos = (wf_u8 *) ie;
    wf_u8 id;
    wf_u16 len;

    wf_u8 *p2p_ie;
    wf_u32 p2p_ielen;

    p2p_ie = p2p_ie_to_get_func(ie, ie_len, NULL, &p2p_ielen);
    if (p2p_ie != ie || p2p_ielen == 0)
    {
        return;
    }

    if (flag)
    {
        pos += 6;
    }
    while (pos - ie + 3 <= ie_len)
    {
        id = *pos;
        len = WF_GET_LE16(pos + 1);

        LOG_I("%s ID:%u, LEN:%u%s\n", __func__, id, len,((pos - ie + 3 + len) <= ie_len) ? "" : "(exceed ie_len)");

        pos += (3 + len);
    }
}

static wf_u8 *p2p_attr_to_get_func(wf_u8 * p2p_ie, wf_u32 p2p_ielen, wf_u8 target_attr_id,wf_u8 * buf_attr, wf_u32 * len_attr)
{
    wf_u8 *attr_ptr = NULL;
    wf_u8 *target_attr_ptr = NULL;
    wf_u8 p2p_oui[4] = { 0x50, 0x6F, 0x9A, 0x09 };

    if (len_attr)
        *len_attr = 0;

    if (!p2p_ie || p2p_ielen <= 6 || (p2p_ie[0] != WLAN_EID_VENDOR_SPECIFIC) || (wf_memcmp(p2p_ie + 2, p2p_oui, 4) != 0))
    {
        return attr_ptr;
    }

    attr_ptr = p2p_ie + 6;

    while ((attr_ptr - p2p_ie + 3) <= p2p_ielen)
    {
        wf_u8 attr_id = *attr_ptr;
        wf_u16 attr_data_len = WF_GET_LE16(attr_ptr + 1);
        wf_u16 attr_len = attr_data_len + 3;

        if (0)
        {
            LOG_I("%s attr_ptr:%p, id:%u, length:%u\n", __func__, attr_ptr, attr_id, attr_data_len);
        }

        if ((attr_ptr - p2p_ie + attr_len) > p2p_ielen)
        {
            break;
        }

        if (attr_id == target_attr_id)
        {
            target_attr_ptr = attr_ptr;

            if (buf_attr)
            {
                wf_memcpy(buf_attr, attr_ptr, attr_len);
            }

            if (len_attr)
            {
                *len_attr = attr_len;
            }

            break;
        } else {
            attr_ptr += attr_len;
        }
    }

    return target_attr_ptr;
}

wf_u8 *p2p_attr_content_to_get_func(wf_u8 * p2p_ie, wf_u32 p2p_ielen, wf_u8 target_attr_id, wf_u8 * buf_content, wf_u32 * len_content)
{
    wf_u8 *attr_ptr;
    wf_u32 attr_len;

    if (len_content)
    {
        *len_content = 0;
    }

    attr_ptr = p2p_attr_to_get_func(p2p_ie, p2p_ielen, target_attr_id, NULL, &attr_len);

    if (attr_ptr && attr_len)
    {
        if (buf_content)
        {
            wf_memcpy(buf_content, attr_ptr + 3, attr_len - 3);
        }

        if (len_content)
        {
            *len_content = attr_len - 3;
        }

        return attr_ptr + 3;
    }

    return NULL;
}

wf_u32 p2p_set_ie(wf_u8 * pbuf, wf_u8 index, wf_u16 attr_len, wf_u8 * pdata_attr)
{
    wf_u32 a_len = 0;
    *pbuf = index;

    WF_PUT_LE16(pbuf + 1, attr_len);

    if (pdata_attr)
    {
        wf_memcpy(pbuf + 3, pdata_attr, attr_len);
    }

    a_len = attr_len + 3;

    return a_len;
}

static wf_u32 p2p_ie_to_del_func(wf_u8 * ies, wf_u32 ies_len_ori, const wf_u8 *msg, wf_u8 flag)
{
    wf_u8 *target_ie;
    wf_u32 target_ie_len;
    wf_u32 ies_len = ies_len_ori;
    int index = 0;

    if (flag) {
        while (1) {
            target_ie = p2p_ie_to_get_func(ies, ies_len, NULL, &target_ie_len);
            if (target_ie && target_ie_len) {
                wf_u8 *next_ie = target_ie + target_ie_len;
                wf_u32 remain_len = ies_len - (next_ie - ies);
                wf_memmove(target_ie, next_ie, remain_len);
                wf_memset(target_ie + remain_len, 0, target_ie_len);
                ies_len -= target_ie_len;

                index++;
            } else {
                break;
            }
        }
    }
    return ies_len;
}

static wf_u32 p2p_attr_to_del_func(wf_u8 * ie, wf_u32 ielen_ori, wf_u8 attr_id, wf_u8 flag)
{
    wf_u8 *target_attr;
    wf_u32 target_attr_len;
    wf_u32 ielen = ielen_ori;
    int index = 0;

    if (flag) {
        while (1) {
            target_attr =
                p2p_attr_to_get_func(ie, ielen, attr_id, NULL, &target_attr_len);
            if (target_attr && target_attr_len) {
                wf_u8 *next_attr = target_attr + target_attr_len;
                wf_u32 remain_len = ielen - (next_attr - ie);
                wf_memmove(target_attr, next_attr, remain_len);
                wf_memset(target_attr + remain_len, 0, target_attr_len);
                *(ie + 1) -= target_attr_len;
                ielen -= target_attr_len;

                index++;
            } else {
                break;
            }
        }
    }
    return ielen;
}

wf_u32 beacon_p2p_ie_build_func(struct wifidirect_info *pwdinfo, wf_u8 * pbuf, wf_u8 vendor_flag)
{
    wf_u8 p2pie[WF_MAX_P2P_IE_LEN]  = { 0x00 };
    wf_u16 capability               = 0;
    wf_u32 len                      = 0;
    wf_u32 p2pielen                 = 0;

    p2pie[p2pielen++]   = 0x50;
    p2pie[p2pielen++]   = 0x6F;
    p2pie[p2pielen++]   = 0x9A;
    p2pie[p2pielen++]   = 0x09;
    capability          = P2P_DEVCAP_INVITATION_PROC | P2P_DEVCAP_CLIENT_DISCOVERABILITY;
    capability          |= ((P2P_GRPCAP_GO | P2P_GRPCAP_INTRABSS) << 8);

    if (pwdinfo->p2p_state == P2P_STATE_PROVISIONING_ING)
    {
        capability |= (P2P_GRPCAP_GROUP_FORMATION << 8);
    }

    capability          = wf_cpu_to_le16(capability);

    p2pielen            += p2p_set_ie(&p2pie[p2pielen], P2P_ATTR_CAPABILITY, 2, (wf_u8 *) & capability);

    p2pielen            += p2p_set_ie(&p2pie[p2pielen], P2P_ATTR_DEVICE_ID, WF_ETH_ALEN, pwdinfo->device_addr);

    if (vendor_flag)
    {
        pbuf            = set_ie(pbuf, _VENDOR_SPECIFIC_IE_, p2pielen, (unsigned char *)p2pie, &len);
    }

    return len;

}

wf_u32 probe_resp_p2p_ie_build_func(struct wifidirect_info * pwdinfo, wf_u8 * pbuf,
                            wf_u8 flag)
{
    wf_u8 p2pie[WF_MAX_P2P_IE_LEN] = { 0x00 };
    wf_u32 len = 0, p2pielen = 0;

    p2pielen = 0;
    p2pie[p2pielen++] = 0x50;
    p2pie[p2pielen++] = 0x6F;
    p2pie[p2pielen++] = 0x9A;
    p2pie[p2pielen++] = 0x09;

    p2pie[p2pielen++] = P2P_ATTR_CAPABILITY;

    WF_PUT_LE16(p2pie + p2pielen, 0x0002);
    p2pielen += 2;

    p2pie[p2pielen++] = DMP_P2P_DEVCAP_SUPPORT;

    if (pwdinfo->role == P2P_ROLE_GO) {
        p2pie[p2pielen] = (P2P_GRPCAP_GO | P2P_GRPCAP_INTRABSS);

        if (pwdinfo->p2p_state == P2P_STATE_PROVISIONING_ING)
            p2pie[p2pielen] |= P2P_GRPCAP_GROUP_FORMATION;

        p2pielen++;
    } else if (pwdinfo->role ==P2P_ROLE_DEVICE) {
        if (pwdinfo->persistent_supported)
            p2pie[p2pielen++] =
                P2P_GRPCAP_PERSISTENT_GROUP | DMP_P2P_GRPCAP_SUPPORT;
        else
            p2pie[p2pielen++] = DMP_P2P_GRPCAP_SUPPORT;
    }

    p2pie[p2pielen++] = P2P_ATTR_EX_LISTEN_TIMING;

    WF_PUT_LE16(p2pie + p2pielen, 0x0004);
    p2pielen += 2;

    WF_PUT_LE16(p2pie + p2pielen, 0xFFFF);
    p2pielen += 2;

    WF_PUT_LE16(p2pie + p2pielen, 0xFFFF);
    p2pielen += 2;

    p2pie[p2pielen++] = P2P_ATTR_DEVICE_INFO;

    WF_PUT_LE16(p2pie + p2pielen, 21 + pwdinfo->device_name_len);
    p2pielen += 2;

    wf_memcpy(p2pie + p2pielen, pwdinfo->device_addr, WF_ETH_ALEN);
    p2pielen += WF_ETH_ALEN;

    WF_PUT_BE16(p2pie + p2pielen, pwdinfo->supported_wps_cm);
    p2pielen += 2;

    {
        WF_PUT_BE16(p2pie + p2pielen, WPS_PDT_CID_MULIT_MEDIA);
        p2pielen += 2;

        WF_PUT_BE32(p2pie + p2pielen, WPSOUI);
        p2pielen += 4;

        WF_PUT_BE16(p2pie + p2pielen, WPS_PDT_SCID_MEDIA_SERVER);
        p2pielen += 2;
    }

    p2pie[p2pielen++] = 0x00;

    WF_PUT_BE16(p2pie + p2pielen, WPS_ATTR_DEVICE_NAME);
    p2pielen += 2;

    WF_PUT_BE16(p2pie + p2pielen, pwdinfo->device_name_len);
    p2pielen += 2;

    if (flag)
    {
        wf_memcpy(p2pie + p2pielen, pwdinfo->device_name,
                    pwdinfo->device_name_len);
        p2pielen += pwdinfo->device_name_len;
    }
    if (pwdinfo->role == P2P_ROLE_GO)
    {
        //p2pielen += add_group_info_attr_of_go_func(pwdinfo, p2pie + p2pielen, 1);
    }

    pbuf = set_ie(pbuf, _VENDOR_SPECIFIC_IE_, p2pielen, (unsigned char *)p2pie, &len);

    return len;

}

void p2p_provision_request_to_issue_func(nic_info_st *nic_info, wf_u8 * pssid, wf_u8 ussidlen,wf_u8 * pdev_raddr, wf_u8 flag)
{

}

void p2p_GO_request_to_issue_func(nic_info_st *nic_info, wf_u8 *raddr, wf_u8 flag)
{
}

void p2p_invitation_request_to_issue_func(nic_info_st *nic_info, wf_u8 * raddr, wf_u8 flag)
{
}

static wf_s32 p2p_mgmt_frame_xmit (nic_info_st *nic_info, wdn_net_info_st *wdn, struct xmit_buf *pxmitbuf, wf_u16 len)
{
    wf_u8 bc_addr[WF_ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    wf_u16 raid;
    wf_u8 *pbuf;
    wf_u8 *pwlanhdr;
    struct tx_desc *ptxdesc;
    struct sta_unit *psta;
    tx_info_st *tx_info = nic_info->tx_info;
    mlme_info_t *mlme_info = nic_info->mlme_info;

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

#ifdef CONFIG_RICHV200_FPGA
    // add txd
    pbuf = pxmitbuf->pbuf;
    pwlanhdr = pbuf + TXDESC_OFFSET_NEW;
    wf_memset(pbuf, 0, TXDESC_OFFSET_NEW);

    /* set for data type */
    SET_BITS_TO_LE_4BYTE(pbuf, 0, 2, TYPE_DATA);
    /* set HWSEQ_EN */
    //SET_BITS_TO_LE_4BYTE(pbuf, 18, 1, 1);
    /* set SEQ */
    SET_BITS_TO_LE_4BYTE(pbuf, 19, 12, GetSequence(pwlanhdr));
    /* set PKT_LEN */
    SET_BITS_TO_LE_4BYTE(pbuf + 8, 0, 16, len);
    /* set USE_RATE */
    SET_BITS_TO_LE_4BYTE(pbuf + 8, 16, 1, 1);
    /* set DATA LONG or SHORT*/

    /* set TX RATE */
    SET_BITS_TO_LE_4BYTE(pbuf + 8, 18, 7, DESC_RATE1M);
    /* set QOS QUEUE, must mgmt queue */
    SET_BITS_TO_LE_4BYTE(pbuf + 12, 12, 1, 1);
    SET_BITS_TO_LE_4BYTE(pbuf + 12, 6, 5, 0); //QSLT_MGNT);
    /* set MBSSID */
    SET_BITS_TO_LE_4BYTE(pbuf + 12, 18, 1, 0);
    /* set RETRY_LIMIT_EN */
    SET_BITS_TO_LE_4BYTE(pbuf + 12, 15, 1, 1);
    /* set DATA_RETRY_LIMIT */
    SET_BITS_TO_LE_4BYTE(pbuf + 12, 16, 2, 0);
    /* set rate mode, mgmt frame use fix mode */
    SET_BITS_TO_LE_4BYTE(pbuf + 16, 5, 1, 0);
    /* set RATE ID, mgmt frame use 802.11 B, the number is 0 */
    SET_BITS_TO_LE_4BYTE(pbuf + 16, 6, 3, 0);
    /* set mac id or sta index */
    SET_BITS_TO_LE_4BYTE(pbuf + 16, 0, 5, 0x01);

    if (wf_wlan_get_cur_bw(nic_info) == CHANNEL_WIDTH_40)
    {
        /* set DBW */
        SET_BITS_TO_LE_4BYTE(pbuf + 16, 12, 1, CHANNEL_WIDTH_20);

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
                SET_BITS_TO_LE_4BYTE(pbuf + 16, 13, 2, HT_DATA_SC_20_UPPER_OF_40MHZ);
            }
            else if (wdn->channle_offset == HAL_PRIME_CHNL_OFFSET_LOWER)
            {
                /* set DSC */
                SET_BITS_TO_LE_4BYTE(pbuf + 16, 13, 2, HT_DATA_SC_20_LOWER_OF_40MHZ);
            }
            else
            {
                /* set DSC */
                SET_BITS_TO_LE_4BYTE(pbuf + 16, 13, 2, HT_DATA_SC_DONOT_CARE);
            }
        }
    }
    else
    {
        /* set DBW */
        SET_BITS_TO_LE_4BYTE(pbuf + 16, 12, 1, CHANNEL_WIDTH_20);
        /* set DSC */
        SET_BITS_TO_LE_4BYTE(pbuf + 16, 13, 2, HT_DATA_SC_DONOT_CARE);
    }

    /* set BMC */
    if (IS_MCAST(GetAddr1Ptr(pwlanhdr)))
        SET_BITS_TO_LE_4BYTE(pbuf + 12, 14, 1, 1);

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
#ifdef CONFIG_CONCURRENT_MODE
	if(nic_info->nic_num == 1)
	{
    	WF_TX_DESC_MBSSID_9086X(pbuf, 1);
	}
	else
	{
    	WF_TX_DESC_MBSSID_9086X(pbuf, 0);
	}
#else
    WF_TX_DESC_MBSSID_9086X(pbuf, 0);
#endif


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
    }
    else
    {
        WF_TX_DESC_SEQ_9086X(pbuf, 0);
        WF_TX_DESC_MACID_9086X(pbuf, 0);
        WF_TX_DESC_DATA_BW_9086X(pbuf, CHANNEL_WIDTH_20);
        WF_TX_DESC_DATA_SC_9086X(pbuf, HT_DATA_SC_DONOT_CARE);

        WF_TX_DESC_USE_RATE_9086X(pbuf, 1);

        WF_TX_DESC_RATE_ID_9086X(pbuf, RATEID_IDX_G);
        WF_TX_DESC_TX_RATE_9086X(pbuf, DESC_RATE6M);

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

    wf_io_write_data(nic_info, 1, pbuf, len, wf_quary_addr(QSLT_MGNT), (void *)wf_xmit_extbuf_delete, tx_info, pxmitbuf);

    return 0;
}

int probereq_p2p_to_issue_func(nic_info_st *pnic_info, wf_u8 * da, wf_s32 wait_ack, wf_u8 flag)
{
    tx_info_st *ptx_info        = (tx_info_st *)pnic_info->tx_info;
    struct xmit_buf *pxmit_buf  = NULL;
    wf_80211_mgmt_t *pframe     = NULL;
    wf_scan_info_t *pscan_info     = pnic_info->scan_info;
    hw_info_st *hw_info         = pnic_info->hw_info;
    wf_u32 var_len              = 0;
    wf_u8 *pvar                 = NULL;
    p2p_info_st *p2p_info       = pnic_info->p2p;
    struct wifidirect_info *pwd = &p2p_info->wdinfo;

    /* alloc xmit_buf */
    pxmit_buf = wf_xmit_extbuf_new(ptx_info);
    if (pxmit_buf == NULL)
    {
        LOG_W("pxmit_buf is NULL");
        return -1;
    }

    /* set frame head */
    wf_memset(pxmit_buf->pbuf, 0,TXDESC_OFFSET + WF_OFFSETOF(wf_80211_mgmt_t, probe_req));
    pframe = (void *)&pxmit_buf->pbuf[TXDESC_OFFSET];

    /* set control field */
    wf_80211_set_frame_type(&pframe->frame_control, WF_80211_STYPE_PROBE_REQ);

    /* set address field */
    if(da)
    {
        wf_memcpy((void*)pframe->da, (void*)da, sizeof(pframe->da));
        wf_memcpy(pframe->sa, nic_to_local_addr(pnic_info), sizeof(pframe->sa));
        wf_memcpy((void*)pframe->bssid, (void*)da, sizeof(pframe->bssid));
    }
    else
    {
        wf_memset(pframe->da, 0xff, sizeof(pframe->da));
        wf_memcpy(pframe->sa, nic_to_local_addr(pnic_info), sizeof(pframe->sa));
        wf_memset(pframe->bssid, 0xff, sizeof(pframe->bssid));
    }
//    SCAN_DBG("SA="WF_MAC_FMT, WF_MAC_ARG(pframe->da));
//    SCAN_DBG("DA="WF_MAC_FMT, WF_MAC_ARG(pframe->sa));
//    SCAN_DBG("BSSID="WF_MAC_FMT, WF_MAC_ARG(pframe->bssid));

    /* set variable field */
    var_len = 0;
    pvar = &pframe->probe_req.variable[0];
    /*1.SSID*/
    if(pwd->p2p_state == P2P_STATE_TX_PROVISION_DIS_REQ)
    {
        pvar = set_ie(pvar, PROBE_REQUEST_IE_SSID,
                      pwd->tx_prov_disc_info.ssidlen, pwd->tx_prov_disc_info.ssid,
                      &var_len);
    }
    else
    {
        P2P_DBG("p2p_wildcard_ssid:%s",pwd->p2p_wildcard_ssid);
        pvar = set_ie(pvar, PROBE_REQUEST_IE_SSID, P2P_WILDCARD_SSID_LEN, pwd->p2p_wildcard_ssid,&var_len);
    }
    /*2.Supported Rates and BSS Membership Selectors*/
    pvar = set_ie(pvar, PROBE_REQUEST_IE_RATE, 8, &pwd->p2p_support_rate[0], &var_len);

    /*4. wps ie and p2p ie*/
    {
        if(pwd->driver_interface == DRIVER_CFG80211)
        {
            mlme_info_t *mlme_info = pnic_info->mlme_info;
#ifdef CONFIG_IOCTL_CFG80211 // for compatibility with windows compile
            if(mlme_info && mlme_info->probereq_wps_ie && mlme_info->wps_ie_len)
            {
                P2P_DBG("wps_ie_len:%d",mlme_info->wps_ie_len);
                wf_memcpy(pvar,mlme_info->probereq_wps_ie,mlme_info->wps_ie_len);
                var_len += mlme_info->wps_ie_len;
                pvar+=mlme_info->wps_ie_len;
            }
#endif

            if(p2p_info->p2p_probe_req_ie && p2p_info->p2p_probe_req_ie_len)
            {
                P2P_DBG(" p2p_probe_req_ie_len:%d",p2p_info->p2p_probe_req_ie_len);
                wf_memcpy(pvar,p2p_info->p2p_probe_req_ie,p2p_info->p2p_probe_req_ie_len);
                var_len += p2p_info->p2p_probe_req_ie_len;
                pvar+=p2p_info->p2p_probe_req_ie_len;
            }
        }
        else
        {

        }
    }

    /* frame send */
    pxmit_buf->pkt_len = WF_OFFSETOF(wf_80211_mgmt_t, probe_req.variable) + var_len;
    if (p2p_mgmt_frame_xmit(pnic_info, NULL, pxmit_buf, pxmit_buf->pkt_len))
    {
        LOG_W("probe frame send fail");
        return -1;
    }
    return 0;
}

int probereq_p2p_to_pre_issue_func(nic_info_st *nic_info, wf_u8 * da)
{
    P2P_DBG("probereq_p2p_to_pre_issue_func");
    return probereq_p2p_to_issue_func(nic_info, da, wf_false, 1);
}

#endif

