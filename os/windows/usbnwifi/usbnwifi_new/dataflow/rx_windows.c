/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    usb_recv.c

Abstract:
    Bus dependant layer USB specific function
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    01-01-2006    Created
Notes:

--*/

#include "wf_os_api.h"
#include "wf_debug.h"
//#include "pcomp.h"
#include "pcomp.h"
#include "common.h"
#include "rx_windows.h"
#include "wf_oids_adapt.h"

//extern wf_u8 calc_rx_rate(wf_u8 rx_rate);
extern void print_buffer(PUCHAR title, LONG idx, PUCHAR src, LONG length);

//KEVENT evt_cmd_completion;
//KEVENT evt_reg_completion;
//KEVENT evt_fw_completion;

NDIS_STATUS wf_recv_parser_rx_desc(wf_recv_pkt_t *pkt, ULONG *data_len)
{
	struct rx_pkt_info *pinfo = &pkt->nic_pkt.pkt_info;
#ifdef CONFIG_RICHV200_FPGA
    struct rxd_detail_new *prxd = (struct rxd_detail_new *)pkt->buffer;
	pinfo->pkt_type = prxd->data_type;
    pinfo->hif_hdr_len = RXD_SIZE + prxd->drvinfo_size*8;
	pkt->rpt_sel = prxd->notice;
	*data_len = RXDESC_SIZE + prxd->drvinfo_size * 8 + prxd->pkt_len;
	if(pinfo->pkt_type != WF_PKT_TYPE_FRAME) {
       *data_len -= 8;
    }
#else
    struct rxd_detail_org *prxd = (struct rxd_detail_org *)pkt->buffer;
	pinfo->pkt_type = WF_PKT_TYPE_FRAME;
    pinfo->hif_hdr_len = RXD_SIZE + 32 + 0;
	pkt->rpt_sel = prxd->rpt_sel;
	*data_len = RXDESC_SIZE + DRVINFO_SZ*8 + prxd->shift + prxd->pkt_len;
#endif

	if(prxd->crc32) {
		return NDIS_STATUS_FAILURE;
	}

	wf_memcpy(pkt->nic_pkt.rxd_raw_buf, pkt->buffer, RXDESC_SIZE);
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
	pinfo->qos_pri = prxd->tid;
    pinfo->rx_rate = calc_rx_rate(prxd->rx_rate);
	//pkt->rpt_sel = prxd->rpt_sel;

	//*data_len = RXDESC_SIZE + DRVINFO_SZ*8 + prxd->shift + prxd->pkt_len;

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_recv_parser_phy_status(wf_recv_pkt_t *pkt, recv_phy_status_st *physts)
{
	NdisMoveMemory(physts, pkt->nic_pkt.pdata - 32, 32);
	
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_recv_parser_mac_header(wf_recv_pkt_t *pkt)
{
	data_frame_header_t *mac_header;
	rx_pkt_info_t *pkt_info = &pkt->nic_pkt.pkt_info;
	UCHAR mac_header_len;

	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS wf_recv_frame_dispatch(wf_recv_info_t *recv_info, wf_recv_pkt_t *pkt)
{
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	PADAPTER padapter = recv_info->padapter;
	rx_pkt_info_t *pkt_info = &pkt->nic_pkt.pkt_info;
	data_frame_header_t *mac_header;
	wf_u8 sub_type, *buffer;
	wf_80211_mgmt_t *pmgmt;
	wf_ap_info_t *ap_info = padapter->ap_info;
	wf_u32 frame_type, data_len;

	buffer = pkt->nic_pkt.pdata;
	data_len = pkt->nic_pkt.len;

	pmgmt = buffer;
	frame_type = GET_HDR_Type(buffer);
	pkt_info->frame_type = frame_type;

	if(ap_info == NULL || ap_info->scan_info == NULL) {
		return NDIS_STATUS_SUCCESS;
	}

	if(memcmp(ap_info->scan_info->bssid, pmgmt->bssid, sizeof(wf_80211_bssid_t)) != 0) {
		return NDIS_STATUS_SUCCESS;
	}	
	
	switch(frame_type){
	case MAC_FRAME_TYPE_MGT:
		sub_type = wf_80211_get_frame_type(pmgmt->frame_control);
		switch(sub_type) {
		case WF_80211_FRM_BEACON:
		case WF_80211_FRM_PROBE_RESP:
			if(ap_info->bcn_len == 0) {
				memcpy(ap_info->beacon, buffer, data_len);
				ap_info->bcn_len = data_len;
			}
			break;
		case WF_80211_FRM_ASSOC_RESP:
			if(ap_info->assoc_resp_len == 0) {
				memcpy(ap_info->assoc_resp, buffer, data_len);
				ap_info->assoc_resp_len = data_len;
			}
			break;
		default:
			break;
		}
		//LOG_D("recv a mgmt frame! len:%d\n", pkt_info->pkt_len);
		break;
	case MAC_FRAME_TYPE_CTRL:
		//LOG_D("recv a ctrl frame! len:%d\n", pkt_info->pkt_len);
		break;
	case MAC_FRAME_TYPE_DATA:
		//LOG_D("recv a data frame! len:%d\n", pkt_info->pkt_len);
		break;
	default:
		LOG_E("frame type = %x", pkt_info->frame_type);
		ret = NDIS_STATUS_NOT_RECOGNIZED;
		break;
	}
	
	return ret;
}

NDIS_STATUS wf_recv_data_dispatch(wf_recv_info_t *recv_info, wf_recv_pkt_t *pkt)
{
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	rx_pkt_info_t *pkt_info = &pkt->nic_pkt.pkt_info;
	nic_info_st *nic_info = pkt->nic_pkt.p_nic_info;
	recv_phy_status_st physts;

	switch(pkt_info->pkt_type) {
	case WF_PKT_TYPE_CMD:
		//LOG_D("recv a cmd frame! len:%d\n", pkt_info->pkt_len);
		//wf_memcpy(cmd_buffer, pkt->nic_pkt.pdata, pkt->nic_pkt.len);
		//cmd_len = pkt->nic_pkt.len
		//KeSetEvent(&evt_cmd_completion, 0, FALSE);
		break;
	case WF_PKT_TYPE_FW:
		//LOG_D("recv a firmware frame! len:%d\n", pkt_info->pkt_len);
		//wf_memcpy(fw_buffer, pkt->nic_pkt.pdata, pkt->nic_pkt.len);
		//fw_len = pkt->nic_pkt.len
		//KeSetEvent(&evt_fw_completion, 0, FALSE);
		break;
	case WF_PKT_TYPE_REG:
		//LOG_D("recv a register frame! len:%d\n", pkt_info->pkt_len);
		//wf_memcpy(reg_buffer, pkt->nic_pkt.pdata, pkt->nic_pkt.len);
		//reg_len = pkt->nic_pkt.len
		//KeSetEvent(&evt_reg_completion, 0, FALSE);
		break;
	case WF_PKT_TYPE_FRAME:

		pkt->nic_pkt.pdata += pkt->nic_pkt.pkt_info.hif_hdr_len;
		pkt->nic_pkt.len -= pkt->nic_pkt.pkt_info.hif_hdr_len;
		
		if(pkt_info->phy_status) {
			hw_info_st *hw_info = nic_info->hw_info;
		
			ret = wf_recv_parser_phy_status(pkt, &physts);
			if(ret != NDIS_STATUS_SUCCESS) {
				LOG_E("recv packet parser rx phy status failed!\n");
				break;
			}

			//LOG_D("process phy status msg!\n");

	        wf_odm_calc_str_and_qual(nic_info, (wf_u8 *)&physts, pkt->nic_pkt.pdata, &pkt->nic_pkt);

	        if(hw_info && hw_info->use_drv_odm) {
	            wf_odm_handle_phystatus(nic_info, &physts, pkt->nic_pkt.pdata, &pkt->nic_pkt);
	        }
		}

		ret = wf_rx_common_process(&pkt->nic_pkt);
	    //if(ret) {
	    //    ret = NDIS_STATUS_NOT_RECOGNIZED;
		//	break;
	    //} else {
		//	ret = NDIS_STATUS_SUCCESS;
		//}

		ret = wf_recv_frame_dispatch(recv_info, pkt);
		if(ret != NDIS_STATUS_SUCCESS) {
			LOG_E("recv pkt dispatch failed!\n");
			break;
		}
		
		break;
	default:
		LOG_E("PKT TYPE = %x", pkt_info->pkt_type);
		ret = NDIS_STATUS_NOT_RECOGNIZED;
		break;
	}

	return ret;
}


void wf_recv_fill_context(PDOT11_EXTSTA_RECV_CONTEXT rx_ctx, ULONG PhyId, wf_u32 current_freq, wf_u16 rx_rate) 
{
    // Fill in the send context fields that are always needed
    MP_ASSIGN_NDIS_OBJECT_HEADER(rx_ctx->Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_EXTSTA_RECV_CONTEXT_REVISION_1,
                                 sizeof(DOT11_EXTSTA_RECV_CONTEXT));
    rx_ctx->uPhyId = PhyId;
    rx_ctx->lRSSI = -100;//Hw11GetRSSI(Nic, pNicFragment);// Temporarily set a fixed value.
    rx_ctx->ucDataRate = rx_rate;
    rx_ctx->uChCenterFrequency = current_freq; //1-2412  
    rx_ctx->uReceiveFlags = 0;

    // TODO: what's the equivalent of CRCstatus in _RX_STATUS_DESC_8187?
/*
    if (HW_IS_IN_NETWORK_MONITOR_MODE(Nic))
    {
        //
        // If this packet has a FCS error, set the RAW packet FCS error flag
        //
        if (pNicFragment->RxDesc.CRCstatus & RXS_CRC32) 
            Dot11RecvContext->uReceiveFlags = DOT11_RECV_FLAG_RAW_PACKET_FCS_FAILURE;
    }
*/
}


NDIS_STATUS wf_recv_release_source(PADAPTER          padapter, wf_recv_pkt_t *pkt)
{
	wf_recv_info_t *recv_info ;
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	rx_pkt_info_t *pkt_info;
	rx_pkt_t *nic_pkt;
	ULONG RecvFlags;

	pkt_info = &pkt->nic_pkt.pkt_info;
	nic_pkt = &pkt->nic_pkt;
	recv_info = padapter->recv_info;
	
	if(pkt->buf_hdl != NULL) {
		WdfObjectDereference(pkt->buf_hdl);
		pkt->buf_hdl = NULL;
	}

	if(pkt->src_buffer != NULL) {
		pkt->src_buffer = NULL;
		pkt->buffer = NULL;
		ExInterlockedInsertHeadList(&recv_info->free_head, &pkt->list, &recv_info->rxque_lock);
	}

	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS wf_recv_release_source_ex(void **ppkt)
{
	wf_recv_pkt_t *pkt = *ppkt;
	wf_recv_info_t *recv_info ;
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	rx_pkt_info_t *pkt_info;
	rx_pkt_t *nic_pkt;
	ULONG RecvFlags;
	PADAPTER padapter = pkt->net_if;

	pkt_info = &pkt->nic_pkt.pkt_info;
	nic_pkt = &pkt->nic_pkt;
	recv_info = padapter->recv_info;
	
	if(pkt->buf_hdl != NULL) {
		WdfObjectDereference(pkt->buf_hdl);
		pkt->buf_hdl = NULL;
	}

	if(pkt->src_buffer != NULL) {
		pkt->src_buffer = NULL;
		pkt->buffer = NULL;
		ExInterlockedInsertHeadList(&recv_info->free_head, &pkt->list, &recv_info->rxque_lock);
	}

	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS wf_recv_data_submit(PADAPTER         padapter, wf_recv_pkt_t *pkt)
{
	wf_recv_info_t *recv_info ;
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	rx_pkt_info_t *pkt_info;
	rx_pkt_t *nic_pkt;
	ULONG RecvFlags;

	nic_info_st *pnic_info = padapter->nic_info;
	wf_wlan_info_t *wlan_info = pnic_info->wlan_info;
	wf_wlan_network_t *pcur_network = &wlan_info->cur_network;
	wf_u32 current_freq = wf_ch_2_freq((int)pcur_network->channel);
	struct rx_pkt_info *pinfo = &pkt->nic_pkt.pkt_info;

	pkt_info = &pkt->nic_pkt.pkt_info;
	nic_pkt = &pkt->nic_pkt;
	recv_info = padapter->recv_info;

	if(pkt == NULL) {
		LOG_E("pkt is NULL");
		return NDIS_STATUS_FAILURE;
	}

	pkt->mdl = pkt->src_buffer;
	if(pkt->mdl == NULL) {
		LOG_E("mdl is NULL");
		return NDIS_STATUS_FAILURE;
	}

	
	// Set protected bit in frame control flags;
	wf_u8 *pframe_ctrl = nic_pkt->pdata + 1;
	if(((*pframe_ctrl) & 0x40) == 0x40)
	{
		*pframe_ctrl -= 0x40;
		nic_pkt->len -=12; // TODO: Figure out WHY???
	}

	//TEST_RX_PRINT(nic_pkt->pdata, nic_pkt->len);

	//LOG_D("len=%d", nic_pkt->len);
	MmInitializeMdl(pkt->mdl, nic_pkt->pdata, nic_pkt->len);
    MmBuildMdlForNonPagedPool(pkt->mdl);
	KeFlushIoBuffers(pkt->mdl, TRUE, TRUE);
	NDIS_MDL_LINKAGE(pkt->mdl) = NULL;

	pkt->mdl->ByteCount = nic_pkt->len;
	pkt->mdl->MappedSystemVa = (UCHAR *)pkt->mdl->StartVa+pkt->mdl->ByteOffset;

	//LOG_D("byte_cnt=%d, byte_offset=%d, sys_va=%x, start_va=%x",
	//	pkt->mdl->ByteCount, pkt->mdl->ByteOffset, pkt->mdl->StartVa, pkt->mdl->MappedSystemVa);

	pkt->nb = NdisAllocateNetBuffer(recv_info->nb_pool, pkt->mdl, 0, 0);
	if(pkt->nb == NULL) {
		LOG_E("malloc packet net buffer failed!\n");
		return NDIS_STATUS_FAILURE;
	}

	NET_BUFFER_DATA_LENGTH(pkt->nb) = nic_pkt->len;
	NET_BUFFER_NEXT_NB(pkt->nb) = NULL;
	
	NET_BUFFER_LIST_FIRST_NB(pkt->nbl) = pkt->nb;
	NET_BUFFER_LIST_STATUS(pkt->nbl) = NDIS_STATUS_SUCCESS;
	//NET_BUFFER_LIST_INFO(pkt->nbl, MediaSpecificInformation) = &pkt->rx_ctx;

	wf_recv_fill_context(&pkt->rx_ctx, padapter->MibInfo.SelectedPhyId, current_freq, pinfo);

	RecvFlags = (NDIS_CURRENT_IRQL() == DISPATCH_LEVEL) ? NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL:0;

	NdisMIndicateReceiveNetBufferLists(
            padapter->MiniportAdapterHandle,
            pkt->nbl,
            0,
            1,
            RecvFlags|NDIS_RECEIVE_FLAGS_RESOURCES
            );
		
	if (pkt->nb != NULL) {
        NdisFreeNetBuffer(pkt->nb);
        pkt->nb = NULL;
    }
	
	if(pkt->mdl != NULL) {
		MmPrepareMdlForReuse(pkt->mdl);
		pkt->mdl = NULL;
	}

	wf_recv_release_source(padapter, pkt);
	return NDIS_STATUS_SUCCESS;
}


void wf_recv_data_reorder(rx_pkt_t *pkt, PADAPTER padapter)
{
    wdn_net_info_st *pwdn_info = pkt->wdn_info;
    hw_info_st      *phw_info  = NULL;
    wf_s32 hdr_tansfer_ret = 0;
    wf_s32 ret     = 0;
    wf_s32 prio    = 0;
    wf_s32 seq_num = 0;
    wf_s32 timeout = 0;
	wf_recv_pkt_t *recv_pkt;
    
    prio = pkt->pkt_info.qos_pri;
    seq_num = pkt->pkt_info.seq_num;
	recv_pkt = CONTAINING_RECORD(pkt, wf_recv_pkt_t, nic_pkt);

    if (NULL == pwdn_info || (prio > 15)) {
        if (pkt->pskb) {
        	wf_recv_release_source(padapter, recv_pkt);
            //wf_free_skb(pkt->pskb);
        }

        if (prio > 15)
            LOG_E("pri error:%d",prio);
        return;
    }

//    hdr_tansfer_ret = wlan_to_eth(pkt);
//    if (hdr_tansfer_ret < 0)
//    {
//        if (pkt->pskb)
//        {
//            wf_free_skb(pkt->pskb);
//        }
//        LOG_E("hdr_tansfer error");
//        return;
//    }

    if(0 == pkt->pkt_info.qos_flag) {
    	wf_recv_data_submit(padapter, recv_pkt);
        //upload_skb(ndev, pkt->pskb);
        return;
    }

    if(wf_false == pwdn_info->ba_ctl[prio].enable) {
    	wf_recv_data_submit(padapter, recv_pkt);
        //upload_skb(ndev, pkt->pskb);
        //rx_do_update_expect_seq(seq_num,&pwdn_info->ba_ctl[prio]);
        return;
    }

    if(0 == pwdn_info->ba_ctl[prio].wait_timeout) {
        pwdn_info->ba_ctl[prio].wait_timeout = 50;
    }

    if(NULL == pwdn_info->ba_ctl[prio].upload_func || NULL == pwdn_info->ba_ctl[prio].free_skb) {
        pwdn_info->ba_ctl[prio].upload_func = wf_recv_data_submit;
        pwdn_info->ba_ctl[prio].free_skb    = wf_recv_release_source_ex;
    }

	recv_pkt->net_if = padapter;
	pkt->pskb = recv_pkt;
#if RX_REORDER_THREAD_EN
    if(wf_rx_reorder_queue_insert(pkt) < 0) {
        if (pkt->pskb) {
        	wf_recv_release_source(padapter, recv_pkt);
            //wf_free_skb(pkt->pskb);
            pkt->pskb = NULL;
        }
    }
#else
	
    wf_rx_data_reorder_core(pkt);
#endif

}

void wf_recv_complete_callback(void *adapter, WDFMEMORY BufferHdl, ULONG data_len, UCHAR offset)
{
	PADAPTER padapter = adapter;
	wf_recv_info_t *recv_info = padapter->recv_info;
	wf_recv_pkt_t *pkt = NULL;
	PLIST_ENTRY plist = NULL;
	ULONG curr_pkt_len, pkt_len;
	LONG remain_len;
	UCHAR *curr_buffer, *temp_buffer;
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	static ULONG i = 0;
	wf_u8 print_flag = 0;

	temp_buffer = WdfMemoryGetBuffer(BufferHdl, NULL);
	curr_buffer = temp_buffer + offset;

	if(data_len>MAX_RECEIVE_BUFFER_SIZE || data_len<RXD_SIZE || temp_buffer == NULL) {
        LOG_E("Recv invalid USB bulk in length:%d buffer 0x%p\n",
        	data_len, curr_buffer);
        goto ERROR_DEAL;
    }

	remain_len = data_len;

	while(1){
		curr_pkt_len = 0;
		pkt = NULL;
		if(IsListEmpty(&recv_info->free_head)) {
			//LOG_E("drop rx pkt len:%d ", data_len);
			goto ERROR_DEAL;
		}

		plist = ExInterlockedRemoveHeadList(&recv_info->free_head, &recv_info->rxque_lock);
		pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
		wf_memset(&pkt->nic_pkt, 0, sizeof(rx_pkt_t));

		pkt->src_buffer = temp_buffer;
		pkt->buffer = curr_buffer;

		ret = wf_recv_parser_rx_desc(pkt, &curr_pkt_len);
		if(ret != NDIS_STATUS_SUCCESS || curr_pkt_len <= pkt->nic_pkt.pkt_info.hif_hdr_len) {
			LOG_E("pkt crc error! len=%d", curr_pkt_len);
			goto ERROR_DEAL;
		}

		if(curr_pkt_len < RXDESC_SIZE) {
			LOG_E("pke len error! len=%d", curr_pkt_len);
			goto ERROR_DEAL;
		}
		
		pkt->nic_pkt.pdata = curr_buffer;
		pkt->nic_pkt.len = curr_pkt_len;
		pkt->nic_pkt.p_nic_info = padapter->nic_info;

		if(pkt->rpt_sel) {
			//LOG_D("recv a c2h frame! len:%d", pkt->nic_pkt.len-32);
			wf_rx_notice_process(pkt->nic_pkt.pdata, pkt->nic_pkt.len-32);
		}

		pkt->buf_hdl = BufferHdl;
		WdfObjectReference(pkt->buf_hdl);
		ExInterlockedInsertTailList(&recv_info->pend_head, &pkt->list, &recv_info->rxque_lock);

		//wf_recv_data_intf(padapter);
		KeSetEvent(&recv_info->rx_evt, 0, FALSE);

		pkt_len = WF_RND8(curr_pkt_len);
		curr_buffer += pkt_len;
		remain_len -= pkt_len;

		if(remain_len <= 0) {
			break;
		}
	}

	return;
	
ERROR_DEAL:
	if(pkt != NULL) {
		ExInterlockedInsertTailList(&recv_info->free_head, &pkt->list, &recv_info->rxque_lock);
	}
}



VOID wf_recv_data_thread(PADAPTER padapter)
{
	//PADAPTER                    padapter;
	PADAPTER_WORKITEM_CONTEXT   WorkItemContext;
	wf_recv_info_t *recv_info ;
	struct xmit_buf *pxmitbuf   = NULL;
	PLIST_ENTRY plist = NULL;
	wf_recv_pkt_t *pkt = NULL;
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	rx_pkt_info_t *pkt_info;
	rx_pkt_t *nic_pkt;
	nic_info_st *nic_info = padapter->nic_info;
	
	recv_info = padapter->recv_info;
	while(1) {
		KeWaitForSingleObject(&recv_info->rx_evt, Executive, KernelMode, TRUE, NULL);

		if(recv_info->rx_thread->stop) {
			LOG_D("stop recv proc");
			break;
		}

		while(!IsListEmpty(&recv_info->pend_head)) {

			if(WF_CANNOT_RUN(nic_info)) {
				//LOG_D("exit pkt proc");
		        break;
		    }
		
			plist = ExInterlockedRemoveHeadList(&recv_info->pend_head, &recv_info->rxque_lock);
			pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);

			ret = wf_recv_data_dispatch(recv_info, pkt);
			if(ret != NDIS_STATUS_SUCCESS) {
				LOG_E("recv data dispatch failed!\n");
				goto ERROR_DEAL;
			}

			if(pkt->nic_pkt.pkt_info.pkt_type == WF_PKT_TYPE_FRAME &&
				pkt->nic_pkt.pkt_info.frame_type == MAC_FRAME_TYPE_DATA) {
			#if RX_REORDER_ENABLE
				wf_recv_data_reorder(&pkt->nic_pkt, padapter);
			#else
				ret = wf_recv_data_submit(padapter, pkt);
				if(ret != NDIS_STATUS_SUCCESS) {
					LOG_E("submit data to ndis failed!");
				}
			#endif
			} else {
				wf_recv_release_source(padapter, pkt);
			}

			pkt = NULL;
			ret = NDIS_STATUS_SUCCESS;
		}

	ERROR_DEAL:
		if(ret != NDIS_STATUS_SUCCESS) {
			if(pkt->buf_hdl != NULL) {
				WdfObjectDereference(pkt->buf_hdl);
			}
			ExInterlockedInsertHeadList(&recv_info->free_head, &pkt->list, &recv_info->rxque_lock);
		}
	}

	wf_os_api_thread_exit(recv_info->rx_thread);
}

void wf_recv_thread_create(PADAPTER pAdapter)
{
	wf_recv_info_t *recv_info = pAdapter->recv_info;

	KeInitializeEvent(&recv_info->rx_evt, SynchronizationEvent, FALSE);

	recv_info->rx_thread = wf_os_api_thread_create(NULL, "recv_thread", wf_recv_data_thread, pAdapter);
	if (NULL == recv_info->rx_thread)
    {
        LOG_E("[wf_mlme_init] create mlme thread failed");
        return -1;
    }
}


void wf_recv_pkt_init(void *param)
{
	PADAPTER padapter = param;
	wf_recv_info_t *recv_info = padapter->recv_info;
	NET_BUFFER_LIST_POOL_PARAMETERS     nbl_pool_param;
    NET_BUFFER_POOL_PARAMETERS          nb_pool_param;
	wf_recv_pkt_t *pkt;
	int i;
	
	NdisZeroMemory(&nbl_pool_param, sizeof(NET_BUFFER_LIST_POOL_PARAMETERS));
    nbl_pool_param.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    nbl_pool_param.Header.Revision = NET_BUFFER_LIST_POOL_PARAMETERS_REVISION_1;
    nbl_pool_param.Header.Size = sizeof(NET_BUFFER_LIST_POOL_PARAMETERS);
    nbl_pool_param.fAllocateNetBuffer = FALSE;
    nbl_pool_param.ContextSize = 0;
    nbl_pool_param.PoolTag = WF_RX_NBL_POOL_TAG;
    nbl_pool_param.DataSize = 0;
    recv_info->nbl_pool = NdisAllocateNetBufferListPool(padapter->MiniportAdapterHandle, &nbl_pool_param);
    if (recv_info->nbl_pool == NULL){
		LOG_E("malloc rx net buffer list pool failed!\n");
        return;
    }

	NdisZeroMemory(&nb_pool_param, sizeof(NET_BUFFER_POOL_PARAMETERS));
    nb_pool_param.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    nb_pool_param.Header.Revision = NET_BUFFER_POOL_PARAMETERS_REVISION_1;
    nb_pool_param.Header.Size = sizeof(NET_BUFFER_POOL_PARAMETERS);
    nb_pool_param.PoolTag = WF_RX_NB_POOL_TAG;
    nb_pool_param.DataSize = 0;
    recv_info->nb_pool = NdisAllocateNetBufferPool(padapter->MiniportAdapterHandle, &nb_pool_param);
    if (recv_info->nb_pool == NULL){
		LOG_E("malloc rx net buffer pool failed!\n");
        return;
    }

	for(i=0; i<RECV_QUEUE_DEPTH; i++) {
		pkt = &recv_info->packet[i];
		NdisZeroMemory(pkt, sizeof(wf_recv_pkt_t));
		
		pkt->nbl = NdisAllocateNetBufferList(recv_info->nbl_pool, MP_RECEIVE_NBL_CONTEXT_SIZE, 0);
		if(pkt->nbl == NULL) {
			LOG_E("malloc packet net buffer list failed!\n");
			return;
		}
		NET_BUFFER_LIST_INFO(pkt->nbl, MediaSpecificInformation) = &pkt->rx_ctx;
		
		ExInterlockedInsertTailList(&recv_info->free_head, &pkt->list, &recv_info->rxque_lock);
	}
}

void wf_recv_pkt_deinit(void *param)
{
	PADAPTER padapter = param;
	wf_recv_info_t *recv_info = padapter->recv_info;
	PLIST_ENTRY plist;
	wf_recv_pkt_t *pkt;

	while(!IsListEmpty(&recv_info->pend_head)) {
		plist = ExInterlockedRemoveHeadList(&recv_info->pend_head, &recv_info->rxque_lock);
		pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
	}
	
	while(!IsListEmpty(&recv_info->free_head)) {
		plist = ExInterlockedRemoveHeadList(&recv_info->free_head, &recv_info->rxque_lock);
		pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
	}

	if (recv_info->nb_pool) {
        NdisFreeNetBufferPool(recv_info->nb_pool);
        recv_info->nb_pool = NULL;
    }
	
	if (recv_info->nbl_pool) {
        NdisFreeNetBufferListPool(recv_info->nbl_pool);
        recv_info->nbl_pool = NULL;
    }
}

void wf_recv_init(void *param)
{
	PADAPTER padapter = param;
	wf_recv_info_t *recv_info = NULL;
	wf_recv_pkt_t *pkt;
	int i;

	LOG_D("start init recv!\n");

	recv_info = wf_malloc(sizeof(wf_recv_info_t));
	if(recv_info == NULL) {
		LOG_E("malloc recv info failed!\n");
		return;
	}

	padapter->recv_info = recv_info;
	recv_info->padapter = padapter;

	KeInitializeSpinLock(&recv_info->rxque_lock);

	InitializeListHead(&recv_info->pend_head);
	InitializeListHead(&recv_info->free_head);
	
	wf_recv_pkt_init(param);
	
	wf_recv_thread_create(padapter);
}

void wf_recv_deinit(void *param)
{
	PADAPTER padapter = param;
	wf_recv_info_t *recv_info = padapter->recv_info;
	PLIST_ENTRY plist;
	wf_recv_pkt_t *pkt;

	LOG_D("start deinit recv!\n");

	if(recv_info == NULL) {
		return;
	}

	if(recv_info->rx_thread != NULL) {
		recv_info->rx_thread->stop = wf_true;
		KeSetEvent(&recv_info->rx_evt, 0, FALSE);
		wf_os_api_thread_destory(recv_info->rx_thread);
		recv_info->rx_thread = NULL;
	}

	wf_recv_pkt_deinit(param);

	wf_free(recv_info);
}

























