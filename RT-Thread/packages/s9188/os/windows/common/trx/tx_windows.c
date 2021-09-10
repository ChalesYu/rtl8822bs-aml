/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    usb_xmit.c

Abstract:
    Bus dependant layer USB specific function
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    01-01-2006    Created

Notes:

--*/
#include "wf_debug.h"
#include "pcomp.h"

//#define DATA_FRAME_HDR_SHORT    24  /* ALL data frame present */
#define ADDRESS4_LEN            6   /* if To/From_DS=1,address4 present */
#define QOS_CONTRL_LEN          2   /* if qos field present in subtype field */
#define HT_CONTRL_LEN           4

void wf_data_que_init(wf_data_que_t *que)
{
	KeInitializeSpinLock(&que->lock);
	InitializeListHead(&que->head);
	que->cnt = 0;
}

NDIS_STATUS wf_pkt_data_enque(wf_data_que_t *que, PLIST_ENTRY list, wf_u8 pos)
{
	KIRQL irq = 0;

	if(pos >= QUE_POS_MAX) {
		return NDIS_STATUS_FAILURE;
	}
	
	KeAcquireSpinLock(&que->lock, &irq);
	//InterlockedIncrement(&que->cnt);
	que->cnt++;
	if(pos == QUE_POS_HEAD) {
		InsertHeadList(&que->head, list);
		//ExInterlockedInsertHeadList(&que->head, &pkt->list, &que->lock);
	} else if(pos == QUE_POS_TAIL) {
		InsertTailList(&que->head, list);
		//ExInterlockedInsertTailList(&que->head, &pkt->list, &que->lock);
	}
	KeReleaseSpinLock(&que->lock, irq);

	return NDIS_STATUS_SUCCESS;
}


PLIST_ENTRY wf_pkt_data_deque(wf_data_que_t *que, wf_u8 pos)
{
	KIRQL irq = 0;
	PLIST_ENTRY plist = NULL;

	if(pos >= QUE_POS_MAX) {
		return plist;
	}

	KeAcquireSpinLock(&que->lock, &irq);
	que->cnt--;
	if(pos == QUE_POS_HEAD) {
		plist = RemoveHeadList(&que->head);
		//plist = ExInterlockedRemoveHeadList(&que->head, &que->lock);
	} else if(pos == QUE_POS_TAIL) {
		plist = RemoveTailList(&que->head);
		//plist = ExInterlockedRemoveTailList(&que->head, &que->lock);
	}
	KeReleaseSpinLock(&que->lock, irq);

	return plist;
}


int wf_pkt_get_hdr_len(wf_u8 *pbuf, wf_u16 pkt_len, wf_u16 *peth_type, wf_u16 *phdr_len)
{
	wf_u16 wlan_hdr_len;
    wf_u8 rmv_len;
    wf_u16 snap_hdr_offset;
    wf_u16 eth_type;
    wf_bool bsnap=0;

	wlan_hdr_len = MAC_HDR_LEN_SHORT;

	if((GET_HDR_To_From_DS(pbuf) >> 8) == 0x03) {
		wlan_hdr_len += ADDRESS4_LEN;
	}

	if ((GET_HDR_SubTpye(pbuf) & MAC_FRAME_TYPE_QOS_DATA) == MAC_FRAME_TYPE_QOS_DATA)
    {
        wlan_hdr_len += QOS_CONTRL_LEN;
    }

	if (GET_HDR_Order(pbuf))
    {
        wlan_hdr_len += HT_CONTRL_LEN ;
    }

    snap_hdr_offset = wlan_hdr_len;//prx_info->iv_len
    bsnap = is_snap_hdr(pbuf + snap_hdr_offset);
    rmv_len = (wf_u8)(snap_hdr_offset + (bsnap ? SNAP_HDR_SIZE : 0));

    if (rmv_len > pkt_len)
    {
        LOG_E("[wlan_to_eth] data_len error (pktlen:%d  rmv_len:%d)", pkt_len,rmv_len);
        return -1;
    }

    //data_len = ppkt->data_len - rmv_len;
    //LOG_W("rmv_len=%d,prx_info->wlan_hdr_len = %d",rmv_len,prx_info->wlan_hdr_len);
    wf_memcpy(&eth_type, pbuf + rmv_len, 2);
    //eth_type = ntohs((unsigned short)eth_type);

	*peth_type = eth_type;
	//all hdr len we need remove
	*phdr_len = wlan_hdr_len + SNAP_HDR_SIZE + sizeof(eth_type);

    return 0;
}


void print_buffer(PUCHAR title, LONG idx, PUCHAR src, LONG length)
{
	LONG i, local_len;

	if(idx%16) {
		return;
	}

	if(length > 1000) {
		local_len = 1000;
	} else {
		local_len = length;
	}
	
	DbgPrint("########:%s[%d]\n", title, length);
	for(i=0; i<(length); i++) {
		if(i && i%16==0) {
			DbgPrint("\n");
		}
		DbgPrint("%02x, ", src[i]);
	}
	DbgPrint("\n");
}

#if 0

typedef struct data_frame_header_s{
	struct {
		wf_u16 protocol :2;
		wf_u16 type		:2;
		wf_u16 subtype	:4;
		wf_u16 to_ds	:1;
		wf_u16 from_ds	:1;
		wf_u16 more_frag:1;
		wf_u16 retry	:1;
		wf_u16 pwr_mgmt	:1;
		wf_u16 more_data:1;
		wf_u16 protected:1;
		wf_u16 htc		:1;
	}frame_ctrl;
	
	wf_u16 duration;
	wf_u8 addr1[MAC_ADDR_LEN];
	wf_u8 addr2[MAC_ADDR_LEN];
	wf_u8 addr3[MAC_ADDR_LEN];
	wf_u16 seq_ctrl;
	wf_u8 addr4[MAC_ADDR_LEN];
	
	struct {
		wf_u16 tid			:4;
		wf_u16 eosp			:1;
		wf_u16 ack_policy	:2;
		wf_u16 amsdu_present:1;
		wf_u16 amsdu_type	:1;
		wf_u16 rdg			:1;
		wf_u16 buffered_ac	:4;
		wf_u16 reserved		:1;
		wf_u16 ac_constraint:1;
	}qos_ctrl;

	wf_u32 ht_ctrl;
}data_frame_header_t;


void print_80211_hdr(void *buf)
{
	data_frame_header_t *hdr_80211 = buf;
	DbgPrint("##################\n");
	DbgPrint("%-8s:%d\n", "protocol", hdr_80211->frame_ctrl.protocol);
	DbgPrint("%-8s:%d\n", "type", hdr_80211->frame_ctrl.type);
	DbgPrint("%-8s:%d\n", "subtype", hdr_80211->frame_ctrl.subtype);
	DbgPrint("%-8s:%d\n", "to_ds", hdr_80211->frame_ctrl.to_ds);
	DbgPrint("%-8s:%d\n", "from_ds", hdr_80211->frame_ctrl.from_ds);
	DbgPrint("%-8s:%d\n", "more_frag", hdr_80211->frame_ctrl.more_frag);
	DbgPrint("%-8s:%d\n", "retry", hdr_80211->frame_ctrl.retry);
	DbgPrint("%-8s:%d\n", "more_data", hdr_80211->frame_ctrl.more_data);
	DbgPrint("%-8s:%d\n", "protected", hdr_80211->frame_ctrl.protected);
	DbgPrint("%-8s:%d\n", "htc", hdr_80211->frame_ctrl.htc);
}
#endif

#if 0
int wf_xmit_get_info(wf_xmit_pkt_t *ppkt, wf_u16 *peth_type, wf_u16 *phdr_len)
{
    wf_u8 *pbuf = ppkt->ptempbuf;
	wf_u16 wlan_hdr_len;
    wf_u8 rmv_len;
    wf_u16 snap_hdr_offset;
    wf_u16 eth_type;
    wf_bool bsnap=0;

	wlan_hdr_len = DATA_FRAME_HDR_SHORT;

	if((GET_HDR_To_From_DS(pbuf) >> 8) == 0x03) {
		wlan_hdr_len += ADDRESS4_LEN;
	}

	if ((GET_HDR_SubTpye(pbuf) & MAC_FRAME_TYPE_QOS_DATA) == MAC_FRAME_TYPE_QOS_DATA)
    {
        wlan_hdr_len += QOS_CONTRL_LEN;
    }

	if (GET_HDR_Order(pbuf))
    {
        wlan_hdr_len += HT_CONTRL_LEN ;
    }

    snap_hdr_offset = wlan_hdr_len;//prx_info->iv_len
    bsnap = is_snap_hdr(pbuf + snap_hdr_offset);
    rmv_len = (wf_u8)(snap_hdr_offset + (bsnap ? SNAP_HDR_SIZE : 0));

    if (rmv_len > ppkt->data_len)
    {
        LOG_E("[wlan_to_eth] data_len error (pktlen:%d  rmv_len:%d)",ppkt->data_len,rmv_len);
        return -1;
    }

    //data_len = ppkt->data_len - rmv_len;
    //LOG_W("rmv_len=%d,prx_info->wlan_hdr_len = %d",rmv_len,prx_info->wlan_hdr_len);
    wf_memcpy(&eth_type, pbuf + rmv_len, 2);
    //eth_type = ntohs((unsigned short)eth_type);

	*peth_type = eth_type;
	//all hdr len we need remove
	*phdr_len = wlan_hdr_len + SNAP_HDR_SIZE + sizeof(eth_type);

    return 0;
}
#endif



char wf_xmit_cannot_send(void *adapter)
{
	PADAPTER padapter = adapter;
	nic_info_st *nic_info = padapter->nic_info;
	wf_xmit_info_t *xmit_info = padapter->xmit_info;

	if(WF_CANNOT_RUN(nic_info)) {
        return -1;
    }

	if (wf_mlme_check_mode(nic_info, WF_MONITOR_MODE) == wf_true) {
        //work_monitor_tx_entry(pnetdev, (struct sk_buff *)pkt);
        return -2;
    }

	if(wf_tx_data_check(nic_info) == wf_false){
    	return -3;
	}

	if(IsListEmpty(&xmit_info->data_free.head)) {
		return -4;
	}

	if(padapter->dev_state != WF_DEV_STATE_RUN) {
		return -5;
	}

	return 0;
}


static int wf_xmit_add_ba(nic_info_st *nic_info, struct xmit_frame *pxmitframe)
{
	wf_u8 issued;
	mlme_info_t *mlme_info;
	wdn_net_info_st *pwdn_info;

    if (pxmitframe->bmcast)
	{
        return -1;
	}

	mlme_info = (mlme_info_t *)nic_info->mlme_info;

	pwdn_info = pxmitframe->pwdn;
    if (pwdn_info == NULL)
	{
        return -1;
	}

    if (pwdn_info->ba_enable_flag[pxmitframe->priority] == wf_true)
	{
        return -1;
	}

	if((pwdn_info->htpriv.ht_option == wf_true) && (pwdn_info->htpriv.ampdu_enable == wf_true))
	{
		issued = (pwdn_info->htpriv.agg_enable_bitmap >> pxmitframe->priority) & 0x1;
		issued |= (pwdn_info->htpriv.candidate_tid_bitmap >> pxmitframe->priority) & 0x1;
		if(issued == 0)
		{
			if ((pxmitframe->frame_tag == DATA_FRAMETAG) && (pxmitframe->ether_type != 0x0806) &&
				(pxmitframe->ether_type != 0x888e) && (pxmitframe->dhcp_pkt != 1))
            {
                pwdn_info->htpriv.candidate_tid_bitmap |= WF_BIT(pxmitframe->priority);
                mlme_info->bareq_parm.tid = pxmitframe->priority;
                pwdn_info->ba_enable_flag[pxmitframe->priority] = wf_true;

				wf_mlme_add_ba_req(nic_info);
                return 0;
            }
        }
     }

    return -1;
}

//need modify with linux driver
static wf_bool wf_xmit_send_frag(nic_info_st *nic_info, wf_xmit_pkt_t *pkt, wf_bool ack)
{
    wf_u8 *mem_addr;
    wf_u32 ff_hwaddr;
    wf_bool bRet = wf_true;
    int ret = 0;
    wf_bool inner_ret = wf_true;
    wf_bool blast = wf_false;
    int t, sz, w_sz, pull = 0;
    struct xmit_frame *pxmitframe = &pkt->xframe;
    hw_info_st *hw_info = nic_info->hw_info;
    wf_u32  txlen = 0;

	UNREFERENCED_PARAMETER(ack);

    mem_addr = pxmitframe->buf_addr;

    for (t = 0; t < pxmitframe->nr_frags; t++)
    {
        if (inner_ret != wf_true && ret == wf_true)
            ret = wf_false;

        if (t != (pxmitframe->nr_frags - 1))
        {
            LOG_D("pattrib->nr_frags=%d\n", pxmitframe->nr_frags);
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
        //pxmitbuf->pg_num   += (txlen+127)/128;
        //wf_timer_set(&pxmitbuf->time, 0);

		//print_buffer("full data", 0, mem_addr, w_sz);

        if(blast)
        {
            ret = wf_io_write_data(nic_info, 1, (char *)mem_addr, w_sz,
                                          ff_hwaddr,wf_xmit_send_complete, nic_info, pkt);
        }
        else
        {
            ret = wf_io_write_data(nic_info, 1, (char *)mem_addr, w_sz,
                                          ff_hwaddr, NULL, nic_info, NULL);
			
        }

        if (ret)
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

#if defined(MP_USE_NET_BUFFER_LIST)

#define NDIS_PKT_STATUE_SET(ndis_pkt, status) NET_BUFFER_LIST_STATUS(ndis_pkt) = status;
#define NDIS_PKT_STATUE_GET(ndis_pkt) 		  NET_BUFFER_LIST_STATUS(ndis_pkt)


NDIS_STATUS wf_xmit_pkt_wlan2eth(PADAPTER padapter, void *ppkt)
{
	wf_xmit_pkt_t *pkt = ppkt;
	struct wf_ethhdr *etherhdr;
	//data_frame_header_t hdr_80211;
	wf_u16 eth_type, hdr_len, msdu_len;
	wf_u16 eth_type_temp;
	wf_u8 mac_hdr[MAC_HDR_LEN_SHORT] = {0};

	//save 802.11 mac header
	NdisMoveMemory(mac_hdr, pkt->ptempbuf, MAC_HDR_LEN_SHORT);

	wf_pkt_get_hdr_len(pkt->ptempbuf, pkt->data_len, &eth_type, &hdr_len);
	//wf_xmit_get_info(pkt, &eth_type, &hdr_len);
	eth_type_temp = ntohs((unsigned short)eth_type);
	//LOG_D("eth_type=[%x][%x], hdr_len=%d", eth_type, eth_type_temp, hdr_len);

	etherhdr = (struct wf_ethhdr *)(pkt->ptempbuf + hdr_len - sizeof(struct wf_ethhdr));
	msdu_len = (wf_u16)(pkt->data_len - hdr_len + sizeof(struct wf_ethhdr));
	pkt->data_len = msdu_len;
	pkt->ptempbuf = (UCHAR *)etherhdr;
	//pkt->xframe.nic_info = padapter->nic_info;

	NdisMoveMemory(&etherhdr->type, &eth_type, 2);
	NdisMoveMemory(etherhdr->dest, GetAddr3Ptr(mac_hdr), MAC_ADDR_LEN);
	NdisMoveMemory(etherhdr->src, GetAddr2Ptr(mac_hdr), MAC_ADDR_LEN);
	
	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS wf_xmit_copy_ndispkt2buffer(void *ndis_pkt, void *pkt_buff)
{
	PNET_BUFFER_LIST nbl = ndis_pkt;
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
	wf_xmit_pkt_t *pkt = pkt_buff;
    PNET_BUFFER     curr_nb;
    PMDL            curr_mdl;
    long            curr_len;
    wf_u8          	*pSrc;
    wf_u8          	*pDest;
	wf_u8 nb_cnt = 0;

    //may we should add txdesc len to pkt->data_len
	//pkt->data_offset = RTL8187_HWDESC_HEADER_LEN + 20;

	if(nbl->FirstNetBuffer == NULL) {
		LOG_E("nbl tx buffer is NULL");
		return NDIS_STATUS_RESOURCES;
	}
	
	pDest = pkt->ptempbuf;
	pkt->data_len = 0;
    for (curr_nb = nbl->FirstNetBuffer; curr_nb != NULL; curr_nb = curr_nb->Next) {
		nb_cnt++;
		curr_mdl = curr_nb->CurrentMdl;
        pSrc = MmGetSystemAddressForMdlSafe(curr_mdl, NormalPagePriority);
        if (!pSrc) {
            return NDIS_STATUS_RESOURCES;
        }

        //
        // For the first MDL with data, we need to skip the free space
        //
        pSrc += curr_nb->CurrentMdlOffset;
        curr_len = MmGetMdlByteCount(curr_mdl) - curr_nb->CurrentMdlOffset;
        if (curr_len > 0) {
            if (curr_len + pkt->data_len > MP_802_11_MAX_FRAME_SIZE) {
                LOG_E("%s: The total MSDU size (%d) is greater than max allowed (%d), nb_cnt=%d\n",
					__FUNCTION__, curr_len + pkt->data_len, MP_802_11_MAX_FRAME_SIZE, nb_cnt);
                return NDIS_STATUS_INVALID_LENGTH;
            }
            
            //
            // Copy the data.
            // 
            NdisMoveMemory(pDest, pSrc, curr_len);
            pkt->data_len += curr_len;
            pDest += curr_len;
        }

        curr_mdl = curr_mdl->Next;
        while (curr_mdl) {
            pSrc = MmGetSystemAddressForMdlSafe(curr_mdl, NormalPagePriority);
            if (!pSrc) {
                return NDIS_STATUS_RESOURCES;
            }

            curr_len = MmGetMdlByteCount(curr_mdl);
            if (curr_len > 0) {
                if (curr_len + pkt->data_len > MP_802_11_MAX_FRAME_SIZE) {
                    LOG_E("%s: The total MSDU size (%d) is greater than max allowed (%d), nb_cnt=%d\n",
					__FUNCTION__, curr_len + pkt->data_len, MP_802_11_MAX_FRAME_SIZE, nb_cnt);
                    return NDIS_STATUS_INVALID_LENGTH;
                }

                // Copy the data. 
                NdisMoveMemory((PUCHAR)pDest, pSrc, curr_len);
                pkt->data_len += curr_len;
                pDest += curr_len;                   
            }

            curr_mdl = curr_mdl->Next;
        }
    }

	if(nb_cnt > 1) {
		LOG_E("the net buffer has more than 1 packet! cnt=%d", nb_cnt);
	}

    return ndisStatus;
}


NDIS_STATUS wf_xmit_copy_nb2buffer(PNET_BUFFER curr_nb, void *pkt_buff)
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
	wf_xmit_pkt_t *pkt = pkt_buff;
    //PNET_BUFFER     curr_nb;
    PMDL            curr_mdl;
    long            curr_len;
    wf_u8          	*pSrc;
    wf_u8          	*pDest;

    //may we should add txdesc len to pkt->data_len
	//pkt->data_offset = RTL8187_HWDESC_HEADER_LEN + 20;
	pDest = pkt->ptempbuf;
	pkt->data_len = 0;

	curr_mdl = NET_BUFFER_CURRENT_MDL(curr_nb);
    pSrc = MmGetSystemAddressForMdlSafe(curr_mdl, NormalPagePriority);
    if (pSrc == NULL) {
        return NDIS_STATUS_RESOURCES;
    }

    //
    // For the first MDL with data, we need to skip the free space
    //
    pSrc += NET_BUFFER_CURRENT_MDL_OFFSET(curr_nb);
    curr_len = MmGetMdlByteCount(curr_mdl) - NET_BUFFER_CURRENT_MDL_OFFSET(curr_nb);
    if (curr_len > 0) {
        if (curr_len + pkt->data_len > MP_802_11_MAX_FRAME_SIZE) {
            LOG_E("Total MSDU greater than max allowed (%d), curr=%d, data_len=%d",
				MP_802_11_MAX_FRAME_SIZE, curr_len, pkt->data_len);
			print_buffer("buffer", 0, pSrc, 200);
            return NDIS_STATUS_INVALID_LENGTH;
        }
        
        // Copy the data.
        NdisMoveMemory(pDest, pSrc, curr_len);
        pkt->data_len += curr_len;
        pDest += curr_len;
    }

    curr_mdl = NDIS_MDL_LINKAGE(curr_mdl);
    while (curr_mdl) {
        pSrc = MmGetSystemAddressForMdlSafe(curr_mdl, NormalPagePriority);
        if (!pSrc) {
            return NDIS_STATUS_RESOURCES;
        }

        curr_len = MmGetMdlByteCount(curr_mdl);
        if (curr_len > 0) {
            if (curr_len + pkt->data_len > MP_802_11_MAX_FRAME_SIZE) {
                LOG_E("Total MSDU greater than max allowed (%d), curr=%d, data_len=%d",
				MP_802_11_MAX_FRAME_SIZE, curr_len, pkt->data_len);
                return NDIS_STATUS_INVALID_LENGTH;
            }

            // Copy the data. 
            NdisMoveMemory((PUCHAR)pDest, pSrc, curr_len);
            pkt->data_len += curr_len;
            pDest += curr_len;                   
        }

        curr_mdl = NDIS_MDL_LINKAGE(curr_mdl);
    }


    return ndisStatus;
}


/**
 * This function is called by NDIS when the OS submits a NBL for transmission to
 * this miniport. The NBL will be transmitted immediately if possible, or otherwise
 * is queued up for later trannmission by the miniport.
 * 
 * \param miniport_ctx        The context for this miniport
 * \param nbl                 The chain of NBL to be transmitted
 * \param flag                Flags such as dispatch level, etc for send
 *
 * \sa MPHandleSendCompleteInterrupt. MpTransmitTxMSDU
 */
void wf_xmit_ndis_pkt(NDIS_HANDLE mp_ctx, PNET_BUFFER_LIST nbl, NDIS_PORT_NUMBER port_number, ULONG flag)
{
    PADAPTER            pAdapter = (PADAPTER)mp_ctx;
	wf_xmit_info_t 		*xmit_info = pAdapter->xmit_info;
	nic_info_st 		*nic_info = pAdapter->nic_info;
	mlme_info_t 		*mlme_info = NULL;
	hw_info_st 			*hw_info = NULL;
    PNET_BUFFER_LIST    curr_nbl = NULL, next_nbl = NULL;
	//PNET_BUFFER     	curr_nb;
	LIST_ENTRY 			*plist;
	wf_xmit_pkt_t 		*pkt = NULL;
	wf_data_que_t 		*ppend, *pfree;
	int 				ret = -1;
	NDIS_STATUS     	failStatus = NDIS_STATUS_SUCCESS;
	//wf_dbg_info_t 		*dbg_info = pAdapter->dbg_info;
	BOOLEAN             DispatchLevel = (flag & NDIS_SEND_FLAGS_DISPATCH_LEVEL) ? TRUE : FALSE;
	KIRQL irq = 0;

	UNREFERENCED_PARAMETER(port_number);

	if(nbl == NULL || xmit_info == NULL || nic_info == NULL) {
		LOG_E("param is NULL\n");
		return;
	}
	
	mlme_info = nic_info->mlme_info;
	hw_info = nic_info->hw_info;
	if(WF_CANNOT_RUN(nic_info) || mlme_info == NULL || hw_info == NULL) {
		LOG_E("xmit need be stoped\n");
        goto EXIT_ERROR;
    }

	KeAcquireSpinLock(&xmit_info->send_lock, &irq);

	ppend = &xmit_info->data_pend;
	pfree = &xmit_info->data_free;

	//we think a net buffer list is a packet
	for(curr_nbl = nbl; curr_nbl != NULL; curr_nbl = next_nbl) {
		next_nbl = NET_BUFFER_LIST_NEXT_NBL(curr_nbl);
		NET_BUFFER_LIST_NEXT_NBL(curr_nbl) = NULL;
		failStatus = NDIS_STATUS_SUCCESS;

		ret = wf_xmit_cannot_send(pAdapter);
		if(ret) {
			//LOG_E("can't send data, reason=%d", ret);
			NDIS_PKT_STATUE_SET(curr_nbl, NDIS_STATUS_RESOURCES);
			NdisMSendNetBufferListsComplete(pAdapter->MiniportAdapterHandle, curr_nbl, DispatchLevel);
			continue;
		}

		plist = wf_pkt_data_deque(pfree, QUE_POS_HEAD);
		if(plist == NULL) {
			LOG_E("get tx buffer failed!");
			NDIS_PKT_STATUE_SET(curr_nbl, NDIS_STATUS_RESOURCES);
			NdisMSendNetBufferListsComplete(pAdapter->MiniportAdapterHandle, curr_nbl, DispatchLevel);
			continue;
		}

		pkt = CONTAINING_RECORD(plist, wf_xmit_pkt_t, list);
		

		pkt->ptempbuf = pkt->tempbuffer;
		pkt->xmit_flag = DispatchLevel;
		pkt->ndis_pkt = curr_nbl;
		NDIS_PKT_STATUE_SET(curr_nbl, NDIS_STATUS_PENDING);

		InterlockedIncrement(&xmit_info->proc_cnt);
		failStatus = wf_xmit_copy_ndispkt2buffer(curr_nbl, pkt);//wf_xmit_copy_nb2buffer(curr_nb, pkt);
		if(failStatus != NDIS_STATUS_SUCCESS) {
			LOG_E("copy data from nb failed");
			NDIS_PKT_STATUE_SET(curr_nbl, NDIS_STATUS_FAILURE);
			wf_xmit_send_complete(nic_info, pkt);
			continue;
		}
		

#if 0
		if(!pkt->is_preproc) {
			wf_xmit_pkt_wlan2eth(pAdapter, pkt);
			wf_xmit_frame_init(pAdapter->nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len);

			pkt->xframe.buf_addr = pkt->buffer;

			if(pkt->xframe.priority > 15) {
	        	LOG_E("the tx priority error!\n");
				NET_BUFFER_LIST_STATUS(curr_nbl) = NDIS_STATUS_FAILURE;
				wf_xmit_send_complete(nic_info, pkt);
				continue;
	        }
			
			pkt->is_preproc = wf_true;
		} else {
			LOG_D("the pkt has preproced! skip init proc");
		}		

		if (mlme_info->link_info.num_tx_ok_in_period_with_tid[pkt->xframe.qsel] > 100 && (hw_info->ba_enable == wf_true)) {
            ret = wf_xmit_add_ba(nic_info, &pkt->xframe);
            if (ret == 0) {
	        	LOG_I("Send Msg to MLME for starting BA!!");
            }
        }

		ret = wf_tx_msdu_to_mpdu(nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len);
		if (ret != wf_true) {
        	LOG_E("wf_tx_msdu_to_mpdu error!!");
			NET_BUFFER_LIST_STATUS(curr_nbl) = NDIS_STATUS_FAILURE;
			wf_xmit_send_complete(nic_info, pkt);
			continue;
		}

		ret = wf_xmit_send_frag(nic_info, pkt, wf_false);
		if(ret != wf_true) {
        	LOG_E("xmit packet error! ret=%d", ret);
			NET_BUFFER_LIST_STATUS(curr_nbl) = NDIS_STATUS_FAILURE;
			wf_xmit_send_complete(nic_info, pkt);
			continue;
		}
#else
			wf_pkt_data_enque(ppend, &pkt->list, QUE_POS_TAIL);
			KeSetEvent(&xmit_info->tx_evt, 0, FALSE);
#endif
	}

	KeReleaseSpinLock(&xmit_info->send_lock, irq);

	return;

EXIT_ERROR:
	for(next_nbl = nbl; next_nbl != NULL; next_nbl = next_nbl->Next) {
		NDIS_PKT_STATUE_SET(curr_nbl, NDIS_STATUS_FAILURE);
    }

	NdisMSendNetBufferListsComplete(pAdapter->MiniportAdapterHandle, nbl, DispatchLevel);
}

int wf_xmit_send_complete(nic_info_st *nic_info, wf_xmit_pkt_t *pkt)
{
	PADAPTER padapter = nic_info->hif_node;
	wf_xmit_info_t *xmit_info = padapter->xmit_info;
	//wf_dbg_info_t *dbg_info = padapter->dbg_info;

	if(pkt == NULL) {
		LOG_E("pkt is NULL");
		return wf_false;
	}

	if(pkt->ndis_pkt == NULL) {
		LOG_E("nbl is NULL");
		return wf_false;
	}

	//all packet is NDIS_STATUS_PENDING or error status, if the packet status
	//has be setted NDIS_STATUS_SUCCESS, means the packet is process twice
	//if(NDIS_PKT_STATUE_GET(pkt->ndis_pkt) == NDIS_STATUS_SUCCESS) {
	//	LOG_E("the pkt has processed");
	//	return wf_false;
	//}

	if(NDIS_PKT_STATUE_GET(pkt->ndis_pkt) == NDIS_STATUS_PENDING) {
		NDIS_PKT_STATUE_SET(pkt->ndis_pkt, NDIS_STATUS_SUCCESS);
		pkt->xmit_flag = FALSE;
	}
	
	NdisMSendNetBufferListsComplete(pkt->mp_handle, pkt->ndis_pkt, pkt->xmit_flag);

	pkt->ptempbuf = NULL;
	pkt->is_preproc = wf_false;
	pkt->ndis_pkt = NULL;
	
	wf_pkt_data_enque(&xmit_info->data_free, &pkt->list, QUE_POS_HEAD);
	InterlockedDecrement(&xmit_info->proc_cnt);
	
    return wf_true;
}


#else

#define NDIS_PKT_STATUE_SET(ndis_pkt, status) NDIS_SET_PACKET_STATUS(ndis_pkt, status);
#define NDIS_PKT_STATUE_GET(ndis_pkt) 		  NDIS_GET_PACKET_STATUS(ndis_pkt)


NDIS_STATUS wf_xmit_pkt_wlan2eth(PADAPTER padapter, void *ppkt)
{
	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS wf_xmit_copy_ndispkt2buffer(void *ndis_pkt, void *pkt_buff)
{
	PNDIS_PACKET 		prNdisPacket = ndis_pkt;
	wf_xmit_pkt_t *pkt = pkt_buff;
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;
	UINT		tempBuffSize, totalBuffSize;
	PNDIS_BUFFER NdisBuffer ;
	PUCHAR tempBuffer = NULL;

	NdisBuffer = prNdisPacket->Private.Head ;
	totalBuffSize = 0;

	NdisQueryBufferSafe(
		NdisBuffer,
		&tempBuffer,
		&tempBuffSize,
		NormalPagePriority
		);
	
	NdisMoveMemory(pkt->ptempbuf, tempBuffer, tempBuffSize);
	totalBuffSize += tempBuffSize;

	while(1)
	{
		NdisGetNextBuffer(NdisBuffer , &NdisBuffer ) ;
		if( NdisBuffer == NULL) 
			break;

		NdisQueryBufferSafe(
			NdisBuffer,
			&tempBuffer,
			&tempBuffSize,
			NormalPagePriority
			);
		
		NdisMoveMemory(pkt->ptempbuf + totalBuffSize, tempBuffer, tempBuffSize);
		totalBuffSize += tempBuffSize;
	}
	
	pkt->data_len = totalBuffSize;

    return ndisStatus;
}

void wf_xmit_ndis_pkt(NDIS_HANDLE       mp_ctx, PPNDIS_PACKET pkt_array, UINT pkt_num)
{
	P_GLUE_INFO_T		prGlueInfo = (P_GLUE_INFO_T)mp_ctx;
	PADAPTER            pAdapter = (PADAPTER)prGlueInfo->prAdapter;
	PNDIS_PACKET 		prNdisPacket;
	wf_xmit_info_t 		*xmit_info = pAdapter->xmit_info;
	nic_info_st 		*nic_info = pAdapter->nic_info;
	mlme_info_t 		*mlme_info = nic_info->mlme_info;
	hw_info_st 			*hw_info = nic_info->hw_info;
	wf_mib_info_t 		*mib_info = pAdapter->mib_info;
    LIST_ENTRY 			*plist;
	wf_xmit_pkt_t 		*pkt = NULL;
	wf_data_que_t 		*ppend, *pfree;
	int 				ret = -1;
	int					i;
	static wf_bool		resourceFlag = wf_true;
	NDIS_STATUS     	failStatus = NDIS_STATUS_SUCCESS;
	KIRQL irq = 0;

	PNDIS_BUFFER NdisBuffer ;
	PUCHAR tempBuffer = NULL;
	UINT		tempBuffSize, totalBuffSize;
	
	if(WF_CANNOT_RUN(nic_info) || mlme_info == NULL || hw_info == NULL) {
		LOG_E("xmit need be stoped\n");
    }

	if(resourceFlag == wf_true){
		NdisMSendResourcesAvailable(pAdapter->MiniportAdapterHandle);
		resourceFlag = wf_false;
	}
	
	KeAcquireSpinLock(&xmit_info->send_lock, &irq);

	ppend = &xmit_info->data_pend;
	pfree = &xmit_info->data_free;
	
	for(i=0; i < pkt_num; i++)
	{
		prNdisPacket = pkt_array[i];
		ret = wf_xmit_cannot_send(pAdapter);
		if(ret) {
			//failStatus = NDIS_STATUS_NOT_ACCEPTED;
			NDIS_PKT_STATUE_SET(prNdisPacket, NDIS_STATUS_FAILURE);
			LOG_E("can't send data, reason=%d", ret);
			NdisMSendComplete(prGlueInfo->rMiniportAdapterHandle,
                      prNdisPacket,
                      NDIS_STATUS_FAILURE);
			continue;
		}
		//totalBuffSize = 0;
		
		NdisBuffer = prNdisPacket->Private.Head ;

		plist = wf_pkt_data_deque(pfree, QUE_POS_HEAD);
		if(plist == NULL) {
			//failStatus = NDIS_STATUS_RESOURCES;
			NDIS_PKT_STATUE_SET(prNdisPacket, NDIS_STATUS_RESOURCES);
			NdisMSendComplete(prGlueInfo->rMiniportAdapterHandle,
                      prNdisPacket,
                      NDIS_STATUS_FAILURE);
            resourceFlag = wf_true;
			LOG_E("get tx buffer failed!");
			continue;
		}
	
		pkt = CONTAINING_RECORD(plist, wf_xmit_pkt_t, list);
		InterlockedIncrement(&xmit_info->proc_cnt);
		pkt->ptempbuf = pkt->tempbuffer;
		pkt->ndis_pkt = prNdisPacket;

#if 0
		NdisQueryBufferSafe(
			NdisBuffer,
			&tempBuffer,
			&tempBuffSize,
			NormalPagePriority
			);
		
		NdisMoveMemory(pkt->ptempbuf, tempBuffer, tempBuffSize);
    	totalBuffSize += tempBuffSize;

		while(1)
		{
			NdisGetNextBuffer(NdisBuffer , &NdisBuffer ) ;
			if( NdisBuffer == NULL) break;

			NdisQueryBufferSafe(
				NdisBuffer,
				&tempBuffer,
				&tempBuffSize,
				NormalPagePriority
				);
			
			NdisMoveMemory(pkt->ptempbuf + totalBuffSize, tempBuffer, tempBuffSize);
			totalBuffSize += tempBuffSize;
		}
#endif
		wf_xmit_copy_ndispkt2buffer(prNdisPacket, pkt);
		NDIS_PKT_STATUE_SET(prNdisPacket, NDIS_STATUS_PENDING);
		//pkt->data_len = totalBuffSize;

#if 0		
		//pkt->xmit_flag = (KeGetCurrentIrql() == DISPATCH_LEVEL) ? 1 : 0;
		if(!pkt->is_preproc) {
			//wf_xmit_pkt_wlan2eth(pAdapter, pkt);
			//pkt->xframe.nic_info = pAdapter->nic_info;
			wf_xmit_frame_init(pAdapter->nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len);

			pkt->xframe.buf_addr = pkt->buffer;

			if(pkt->xframe.priority > 15) {
				LOG_E("the tx priority error!\n");
				failStatus = NDIS_STATUS_FAILURE;
				goto ERROR_DEAL;
			}

			pkt->is_preproc = wf_true;
		}			

		if (mlme_info->link_info.num_tx_ok_in_period_with_tid[pkt->xframe.qsel] > 100 && (hw_info->ba_enable == wf_true)) {
            ret = wf_xmit_add_ba(nic_info, &pkt->xframe);
            if (ret == 0) {
	        	LOG_I("Send Msg to MLME for starting BA!!");
            }
        }
		ret = wf_tx_msdu_to_mpdu(nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len);
		if (ret != wf_true) {
        	LOG_E("wf_tx_msdu_to_mpdu error!!");
			failStatus = NDIS_STATUS_FAILURE;
			goto ERROR_DEAL;
		}

		ret = wf_xmit_send_frag(nic_info, pkt, wf_false);
		if(ret != wf_true) {
        	LOG_E("xmit packet error!\n");
			failStatus = NDIS_STATUS_FAILURE;
			goto ERROR_DEAL;
		}
	
ERROR_DEAL:

		if(failStatus != NDIS_STATUS_SUCCESS) {
	
			NdisMSendComplete(prGlueInfo->rMiniportAdapterHandle,
								  prNdisPacket,
								  NDIS_STATUS_NOT_ACCEPTED);
			if(pkt != NULL) {
				pkt->ptempbuf = NULL;
				pkt->is_preproc = wf_false;
				pkt->tx_pkt_desc = NULL;
				wf_pkt_data_enque(pfree, &pkt->list, QUE_POS_TAIL);
				InterlockedDecrement(&xmit_info->proc_cnt);
				InterlockedIncrement(&mib_info->num_xmit_error.LowPart);
			}
		}
#else
		wf_pkt_data_enque(ppend, &pkt->list, QUE_POS_TAIL);
		//InterlockedDecrement(&xmit_info->proc_cnt);
		KeSetEvent(&xmit_info->tx_evt, 0, FALSE);
#endif
	}
	KeReleaseSpinLock(&xmit_info->send_lock, irq);
	return;
}


int wf_xmit_send_complete(nic_info_st *nic_info, wf_xmit_pkt_t *pkt)
{
	PADAPTER prAdapter = (PADAPTER)nic_info->hif_node;
	wf_mib_info_t *mib_info = prAdapter->mib_info;
	wf_xmit_info_t 	*xmit_info = prAdapter->xmit_info;
	NDIS_STATUS     	failStatus = NDIS_STATUS_SUCCESS;
	
	
	
	if(pkt == NULL) {
			LOG_E("pkt is NULL");
			return wf_false;
	}

	//if(NDIS_PKT_STATUE_GET(pkt->ndis_pkt) == NDIS_STATUS_SUCCESS) {
	//	LOG_E("the pkt has processed");
	//	return wf_false;
	//}

	if(NDIS_PKT_STATUE_GET(pkt->ndis_pkt) == NDIS_STATUS_PENDING) {
		NDIS_PKT_STATUE_SET(pkt->ndis_pkt, NDIS_STATUS_SUCCESS);
		pkt->xmit_flag = FALSE;
		InterlockedIncrement(&mib_info->num_xmit_ok.LowPart);
	} else {
		InterlockedIncrement(&mib_info->num_xmit_error.LowPart);
		failStatus = NDIS_STATUS_FAILURE;
	}
	
	xmit_info->tx_byte += pkt->data_len;
	NdisMSendComplete(prAdapter->MiniportAdapterHandle,
                      (PNDIS_PACKET) pkt->ndis_pkt,
                      failStatus);
	
	pkt->ptempbuf = NULL;
	pkt->is_preproc = wf_false;
	
	wf_pkt_data_enque(&xmit_info->data_free, &pkt->list, QUE_POS_HEAD);
	
	InterlockedDecrement(&xmit_info->proc_cnt);
	return 0;
}


#endif

void wf_xmit_data_thread(PADAPTER         padapter)
{
	PLIST_ENTRY plist = NULL;
	NDIS_STATUS ndis_ret = NDIS_STATUS_SUCCESS;
	wf_bool ret = wf_true;
	wf_xmit_pkt_t *pkt = NULL;
	mlme_state_e state;
	nic_info_st *nic_info = padapter->nic_info;
	wf_mib_info_t *mib_info;
	mlme_info_t *mlme_info = nic_info->mlme_info;
    hw_info_st *hw_info = nic_info->hw_info;
	wf_xmit_info_t *xmit_info = padapter->xmit_info;
	wf_data_que_t *ppend, *pfree;
	int addbaRet = -1;
	PRKTHREAD pthread;
	KPRIORITY prio;
#ifdef NDIS51_MINIPORT
	P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T)padapter->parent;
	PNDIS_PACKET ndis_pkt; 
#else
	PNET_BUFFER_LIST ndis_pkt;
#endif

	pthread = KeGetCurrentThread();
	if(pthread != NULL) {
		prio = KeSetPriorityThread(pthread, LOW_REALTIME_PRIORITY);
		LOG_D("old_prio=%d, new_prio=%d", prio, KeQueryPriorityThread(pthread));
	} else {
		LOG_W("pthread is NULL");
	}

	ppend = &xmit_info->data_pend;
	pfree = &xmit_info->data_free;
	mib_info = padapter->mib_info;
			
	while(1) {
		
		KeWaitForSingleObject(&xmit_info->tx_evt, Executive, KernelMode, TRUE, NULL);

		if(xmit_info->tx_thread->stop) {
			break;
		}
	
		while(!IsListEmpty(&ppend->head)) {

			if(WF_CANNOT_RUN(nic_info)) {
		        break;
		    }
			
			wf_mlme_get_state(nic_info, &state);
	        if (state == MLME_STATE_SCAN){
	            break;
	        }
			
			plist = wf_pkt_data_deque(ppend, QUE_POS_HEAD);
			if(plist == NULL) {
				LOG_E("get xmit list failed");
				break;
			}
			pkt = CONTAINING_RECORD(plist, wf_xmit_pkt_t, list);
			ndis_pkt = pkt->ndis_pkt;
			//InterlockedIncrement(&xmit_info->proc_cnt);
#if 0
			//if the packet send is break in BA request process, we no need exec preproc
			if(!pkt->is_preproc) {
#ifdef MP_USE_NET_BUFFER_LIST
				wf_xmit_pkt_wlan2eth(padapter, pkt);
#endif

				if(wf_xmit_frame_init(padapter->nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len) == wf_false) {
					LOG_E("xmit frame init error!");
					ndis_ret = NDIS_STATUS_FAILURE;
					break;
				}
				
				pkt->xframe.buf_addr = pkt->buffer;

				if(pkt->xframe.priority > 15) {
		        	LOG_E("the tx priority error!");
					ndis_ret = NDIS_STATUS_FAILURE;
					break;
		        }

				pkt->is_preproc = wf_true;
			}			
			
			if (mlme_info->link_info.num_tx_ok_in_period_with_tid[pkt->xframe.qsel] > 100 && (hw_info->ba_enable == wf_true)) {
	            addbaRet = wf_xmit_add_ba(nic_info, &pkt->xframe);
	            if (addbaRet == 0) {
		        	LOG_I("Send Msg to MLME for starting BA!!");
					//wf_pkt_data_enque(ppend, &pkt->list, QUE_POS_HEAD);
					//InterlockedDecrement(&xmit_info->proc_cnt);
					//break;
	            }
	        }

			ret = wf_tx_msdu_to_mpdu(nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len);
			if (ret != wf_true) {
	        	LOG_E("wf_tx_msdu_to_mpdu error!!");
				ndis_ret = NDIS_STATUS_FAILURE;
				break;
			}

			ret = wf_xmit_send_frag(nic_info, pkt, wf_false);
			if(ret != wf_true) {
	        	LOG_E("xmit packet error!");
				ndis_ret = NDIS_STATUS_FAILURE;
				break;
			}
#endif
			if(!pkt->is_preproc) {
				wf_xmit_pkt_wlan2eth(padapter, pkt);
				wf_xmit_frame_init(padapter->nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len);

				pkt->xframe.buf_addr = pkt->buffer;

				if(pkt->xframe.priority > 15) {
		        	LOG_E("the tx priority error!\n");
					NDIS_PKT_STATUE_SET(ndis_pkt, NDIS_STATUS_FAILURE);
					wf_xmit_send_complete(nic_info, pkt);
					continue;
		        }
				
				pkt->is_preproc = wf_true;
			} else {
				LOG_D("the pkt has preproced! skip init proc");
			}		

			if (mlme_info->link_info.num_tx_ok_in_period_with_tid[pkt->xframe.qsel] > 100 && (hw_info->ba_enable == wf_true)) {
	            ret = wf_xmit_add_ba(nic_info, &pkt->xframe);
	            if (ret == 0) {
		        	LOG_I("Send Msg to MLME for starting BA!!");
	            }
	        }

			ret = wf_tx_msdu_to_mpdu(nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len);
			if (ret != wf_true) {
	        	LOG_E("wf_tx_msdu_to_mpdu error!!");
				NDIS_PKT_STATUE_SET(ndis_pkt, NDIS_STATUS_FAILURE);
				wf_xmit_send_complete(nic_info, pkt);
				continue;
			}

			ret = wf_xmit_send_frag(nic_info, pkt, wf_false);
			if(ret != wf_true) {
	        	LOG_E("xmit packet error! ret=%d", ret);
				NDIS_PKT_STATUE_SET(ndis_pkt, NDIS_STATUS_FAILURE);
				wf_xmit_send_complete(nic_info, pkt);
				continue;
			}
			//wf_dbg_counter_add(padapter->dbg_info, pkt->data_len-WF_ETH_HLEN, DBG_DIR_TX);

			//pkt = NULL;
			//ndis_ret = NDIS_STATUS_SUCCESS;
		}
#if 0
		if(ndis_ret != NDIS_STATUS_SUCCESS && pkt != NULL) {
#ifdef NDIS51_MINIPORT
			LOG_D("---ZY_TEST--exit this send 1");
			if(pkt->tx_pkt_desc != NULL)
			{
				NdisMSendComplete(prGlueInfo->rMiniportAdapterHandle,
							  (PNDIS_PACKET)pkt->tx_pkt_desc,
							  NDIS_STATUS_NOT_ACCEPTED);
				pkt->tx_pkt_desc = NULL;
			}
			InterlockedIncrement(&mib_info->num_xmit_error.LowPart);
#endif
			
			LOG_D("---ZY_TEST--exit this send 2");
			pkt->ptempbuf = NULL;
			pkt->is_preproc = wf_false;
			wf_pkt_data_enque(pfree, &pkt->list, QUE_POS_TAIL);
			InterlockedDecrement(&xmit_info->proc_cnt);
			pkt = NULL;
		}
#endif
	}
	wf_os_api_thread_exit(xmit_info->tx_thread);
}

void wf_xmit_cancel_send(NDIS_HANDLE mp_ctx, PVOID CancelId)
{
	UNREFERENCED_PARAMETER(mp_ctx);
	UNREFERENCED_PARAMETER(CancelId);
}

NDIS_STATUS wf_xmit_init(void *param)
{
	PADAPTER padapter = param;
	wf_xmit_info_t *xmit_info = NULL;
	wf_u8 i;
	wf_xmit_pkt_t *pkt = NULL;// in 9188 driver, the wf_pkt_t need replaced by xmit_frame
	wf_data_que_t *pfree;

	LOG_D("start init xmit!\n");

	xmit_info = wf_malloc(sizeof(wf_xmit_info_t));
	if(xmit_info == NULL) {
		LOG_E("malloc xmit info failed!\n");
		return NDIS_STATUS_FAILURE;
	}

	padapter->xmit_info = xmit_info;
	xmit_info->padapter = padapter;

	KeInitializeSpinLock(&xmit_info->send_lock);

	wf_data_que_init(&xmit_info->data_pend);
	wf_data_que_init(&xmit_info->data_free);

	pfree = &xmit_info->data_free;
	
	for(i=0; i<XMIT_QUEUE_DEPTH; i++) {
		pkt = &xmit_info->packet[i];
		NdisZeroMemory(pkt, sizeof(wf_xmit_pkt_t));
		pkt->xframe.frame_tag = DATA_FRAMETAG;
		pkt->xframe.pkt_offset = (PACKET_OFFSET_SZ / 8);
		pkt->mp_handle = padapter->MiniportAdapterHandle;
		pkt->xframe.nic_info = padapter->nic_info;
		pkt->net_intf = padapter;
		wf_pkt_data_enque(pfree, &pkt->list, QUE_POS_TAIL);
	}

	KeInitializeEvent(&xmit_info->tx_evt, SynchronizationEvent, FALSE);
	xmit_info->tx_thread = wf_os_api_thread_create(NULL, "xmit_thread", wf_xmit_data_thread, padapter);
	if (NULL == xmit_info->tx_thread) {
        LOG_E("[wf_mlme_init] create mlme thread failed");
        return NDIS_STATUS_FAILURE;
    }

	return NDIS_STATUS_SUCCESS;
}

void wf_xmit_deinit(void *param)
{
	PADAPTER padapter = param;
	wf_xmit_info_t *xmit_info = padapter->xmit_info;
	//PLIST_ENTRY plist;
	//wf_xmit_pkt_t *pkt;
	wf_data_que_t *pque[2];
	//int i;

	LOG_D("start deinit xmit!\n");

	if(xmit_info == NULL) {
		return;
	}

	pque[0] = &xmit_info->data_free;
	pque[1] = &xmit_info->data_pend;

	if(xmit_info->tx_thread != NULL) {
		xmit_info->tx_thread->stop = wf_true;
		KeSetEvent(&xmit_info->tx_evt, 0, FALSE);
		wf_os_api_thread_destory(xmit_info->tx_thread);
		xmit_info->tx_thread = NULL;
	}

//	for(i=0; i<2; i++) {
//		while(!IsListEmpty(&pque[i]->head)) {
//			plist = wf_pkt_data_deque(pque[i], QUE_POS_HEAD);
//			pkt = CONTAINING_RECORD(plist, wf_xmit_pkt_t, list);
//			//need do deinit xmit packet
//		}
//	}

	wf_free(xmit_info);
	
}


