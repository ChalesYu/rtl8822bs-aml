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

#include "wf_debug.h"
#include "pcomp.h"


#define RXD_SIZE_COMMON (20)
#define RXD_SIZE_DATA   (24)

#define ENC_MIC_LEN 8
//#define DATA_FRAME_HDR_SHORT    24  /* ALL data frame present */


//extern wf_u8 calc_rx_rate(wf_u8 rx_rate);
extern void print_buffer(PUCHAR title, LONG idx, PUCHAR src, LONG length);

//KEVENT evt_cmd_completion;
//KEVENT evt_reg_completion;
//KEVENT evt_fw_completion;




NDIS_STATUS wf_recv_get_pre_info(wf_u8 *pkt_buf, wf_recv_preproc_t *pkt_pre)
{
	//struct rx_pkt_info *pinfo = &pkt->nic_pkt->pkt_info;
#ifdef CONFIG_RICHV200_FPGA
    struct rxd_detail_new *prxd = (struct rxd_detail_new *)pkt_buf;

	pkt_pre->rpt_sel = (wf_u8)prxd->notice;
	pkt_pre->hdr_len = (wf_u8)(RXD_SIZE + prxd->drvinfo_size*8);
	pkt_pre->pkt_type = (wf_u8)prxd->data_type;
	pkt_pre->pkt_len = (wf_u16)(RXDESC_SIZE + prxd->drvinfo_size * 8 + prxd->pkt_len);
	if(pkt_pre->pkt_type != WF_PKT_TYPE_FRAME) {
		pkt_pre->pkt_len -= 8;
	}
#else
    struct rxd_detail_org *prxd = (struct rxd_detail_org *)pkt_buf;

	pkt_pre->rpt_sel = prxd->rpt_sel;
	pkt_pre->hdr_len = RXD_SIZE + 32 + 0;
	pkt_pre->pkt_type = WF_PKT_TYPE_FRAME;
	pkt_pre->pkt_len = RXDESC_SIZE + DRVINFO_SZ*8 + prxd->shift + prxd->pkt_len;
#endif

	if(prxd->crc32) {
		LOG_E("crc error! len=%d", pkt_pre->pkt_len);
		return NDIS_STATUS_FAILURE;
	}

	if(pkt_pre->pkt_len < RXD_SIZE_COMMON || pkt_pre->pkt_len > MP_802_11_MAX_FRAME_SIZE) {
		LOG_E("pke len error! len=%d", pkt_pre->pkt_len);
		return NDIS_STATUS_FAILURE;
	}

	return NDIS_STATUS_SUCCESS;
}



NDIS_STATUS wf_recv_parser_rx_desc(wf_recv_pkt_t *pkt)
{
	struct rx_pkt_info *pinfo = &pkt->nic_pkt.pkt_info;
#if 0
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
	pkt->rpt_sel = (UCHAR)prxd->rpt_sel;
	*data_len = RXDESC_SIZE + DRVINFO_SZ*8 + prxd->shift + prxd->pkt_len;
#endif
#else

#ifdef CONFIG_RICHV200_FPGA
    struct rxd_detail_new *prxd = (struct rxd_detail_new *)pkt->buffer;
#else
    struct rxd_detail_org *prxd = (struct rxd_detail_org *)pkt->buffer;
#endif

#endif

	wf_memcpy(pkt->nic_pkt.rxd_raw_buf, pkt->buffer, RXDESC_SIZE);
	pinfo->seq_num = (wf_u16)prxd->seq;
    pinfo->pkt_len = (wf_u16)prxd->pkt_len;//use pre_info.pkt_len
    pinfo->amsdu = (wf_u8)prxd->amsdu;
    pinfo->qos_flag = (wf_u8)prxd->qos;
    pinfo->more_data = (wf_u8)prxd->more_data;
    pinfo->frag_num = (wf_u8)prxd->frag;
    pinfo->more_frag = (wf_u8)prxd->more_frag;
    pinfo->encrypt_algo = (wf_u8)prxd->encrypt_algo;
    pinfo->usb_agg_pktnum = (wf_u8)prxd->usb_agg_pktnum;
    pinfo->phy_status = (wf_u8)prxd->phy_status;
	pinfo->qos_pri = (wf_u8)prxd->tid;
    pinfo->rx_rate = calc_rx_rate((wf_u8)prxd->rx_rate);
	//skip the rxd info
	pkt->nic_pkt.pdata += RXD_SIZE;
	pkt->nic_pkt.len -= RXD_SIZE;
	//pkt->rpt_sel = prxd->rpt_sel;

	//*data_len = RXDESC_SIZE + DRVINFO_SZ*8 + prxd->shift + prxd->pkt_len;

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_recv_parser_phy_status(nic_info_st *nic_info, wf_recv_pkt_t *pkt)
{
	hw_info_st *hw_info = nic_info->hw_info;
	recv_phy_status_st *physts = NULL;

	if(pkt->nic_pkt.pkt_info.phy_status) {
		physts = wf_malloc(sizeof(recv_phy_status_st));
		if(physts == NULL) {
			LOG_E("malloc phy status buffer failed!");
			return NDIS_STATUS_FAILURE;
		}

		NdisMoveMemory(physts, pkt->nic_pkt.pdata, 32);

		//skip phy status info
		pkt->nic_pkt.pdata += 32;
		pkt->nic_pkt.len -= 32;
		//we need use frame data
		wf_rx_calc_str_and_qual(nic_info, (wf_u8*)physts, pkt->nic_pkt.pdata, &pkt->nic_pkt);

	    //if(hw_info && hw_info->use_drv_odm) {
	    //    wf_odm_handle_phystatus(nic_info, physts, pkt->nic_pkt->pdata, &pkt->nic_pkt);
	    //}

		wf_free(physts);
	}else {
		pkt->nic_pkt.pdata += 32;
		pkt->nic_pkt.len -= 32;
	}
	
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_recv_parser_mac_header(wf_recv_pkt_t *pkt)
{
	UNREFERENCED_PARAMETER(pkt);

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_recv_release_source(PADAPTER          padapter, wf_recv_pkt_t *pkt)
{
	wf_recv_info_t *recv_info;
	recv_info = padapter->recv_info;

	if(pkt->buf_hdl != NULL) {
		WdfObjectDereference(pkt->buf_hdl);	
		pkt->buf_hdl = NULL;
	}

	if(pkt->src_buffer != NULL) {
		pkt->src_buffer = NULL;
		pkt->buffer = NULL;
		wf_pkt_data_enque(&recv_info->comm_free, &pkt->list, QUE_POS_HEAD);
		InterlockedDecrement(&recv_info->proc_cnt);
	}

	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS wf_recv_release_source_ex(void **ppkt)
{
	wf_recv_pkt_t *pkt = *ppkt;
	wf_recv_info_t *recv_info ;
	PADAPTER padapter = pkt->net_if;

	recv_info = padapter->recv_info;
	
	if(pkt->buf_hdl != NULL) {
		WdfObjectDereference(pkt->buf_hdl);
		pkt->buf_hdl = NULL;
	}

	if(pkt->src_buffer != NULL) {
		pkt->src_buffer = NULL;
		pkt->buffer = NULL;
		wf_pkt_data_enque(&recv_info->comm_free, &pkt->list, QUE_POS_HEAD);
		InterlockedDecrement(&recv_info->proc_cnt);
	}

	return NDIS_STATUS_SUCCESS;
}


#if defined(MP_USE_NET_BUFFER_LIST)
void wf_recv_fill_context(PDOT11_EXTSTA_RECV_CONTEXT rx_ctx, ULONG PhyId, wf_u32 current_freq, wf_u16 rx_rate) 
{
    // Fill in the send context fields that are always needed
    MP_ASSIGN_NDIS_OBJECT_HEADER(rx_ctx->Header, 
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 DOT11_EXTSTA_RECV_CONTEXT_REVISION_1,
                                 sizeof(DOT11_EXTSTA_RECV_CONTEXT));
    rx_ctx->uPhyId = PhyId;
    rx_ctx->lRSSI = -100;//Hw11GetRSSI(Nic, pNicFragment);// Temporarily set a fixed value.
    rx_ctx->ucDataRate = (UCHAR)rx_rate;
    rx_ctx->uChCenterFrequency = current_freq; //1-2412  
    rx_ctx->uReceiveFlags = 0;
}


NDIS_STATUS wf_recv_data_submit(nic_info_st *pnic_info, wf_recv_pkt_t *pkt)
{
	wf_recv_info_t *recv_info ;
	rx_pkt_info_t *pkt_info;
	rx_pkt_t *nic_pkt;
	PADAPTER padapter;
	ULONG RecvFlags;

	padapter = (PADAPTER) pnic_info->hif_node;
	wf_mib_info_t *mib_info = padapter->mib_info;
	wf_wlan_mgmt_info_t *wlan_info = NULL;
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;

	if(pkt == NULL) {
		LOG_E("pkt is NULL");
		return NDIS_STATUS_FAILURE;
	}

	if(mib_info == NULL || pnic_info == NULL) {
		LOG_E("param is NULL");
		ret = NDIS_STATUS_FAILURE;
		goto ERROR_DEAL;
	}

	if(WF_CANNOT_RUN(pnic_info)) {
		LOG_E("param is NULL");
		ret = NDIS_STATUS_FAILURE;
		goto ERROR_DEAL;
	}

	wlan_info = pnic_info->wlan_mgmt_info;
	if(wlan_info == NULL) {
		LOG_E("param is NULL");
		ret = NDIS_STATUS_FAILURE;
		goto ERROR_DEAL;
	}
	
	pkt_info = &pkt->nic_pkt.pkt_info;
	nic_pkt = &pkt->nic_pkt;
	recv_info = padapter->recv_info;

	

	pkt->mdl = (PMDL)pkt->src_buffer;
	if(pkt->mdl == NULL) {
		LOG_E("mdl is NULL");
		ret = NDIS_STATUS_FAILURE;
		goto ERROR_DEAL;
	}

	//LOG_D("len=%d", nic_pkt->len);
	MmInitializeMdl(pkt->mdl, pkt->tmp_data, nic_pkt->len);
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
		ret = NDIS_STATUS_FAILURE;
		goto ERROR_DEAL;
	}

	NET_BUFFER_DATA_LENGTH(pkt->nb) = nic_pkt->len;
	NET_BUFFER_NEXT_NB(pkt->nb) = NULL;
	
	NET_BUFFER_LIST_FIRST_NB(pkt->nbl) = pkt->nb;
	NET_BUFFER_LIST_STATUS(pkt->nbl) = NDIS_STATUS_SUCCESS;
	//NET_BUFFER_LIST_INFO(pkt->nbl, MediaSpecificInformation) = &pkt->rx_ctx;

	wf_recv_fill_context(&pkt->rx_ctx, mib_info->OperatingPhyId, 
		wf_ch_2_freq((int)wlan_info->cur_network.channel), pkt_info->rx_rate);

	RecvFlags = (NDIS_CURRENT_IRQL() == DISPATCH_LEVEL) ? NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL:0;

	NdisMIndicateReceiveNetBufferLists(
            padapter->MiniportAdapterHandle,
            pkt->nbl,
            0,
            1,
            RecvFlags|NDIS_RECEIVE_FLAGS_RESOURCES
            );
	
	//wf_dbg_counter_add(padapter->dbg_info, nic_pkt->len-WLAN_HDR_A3_QOS_LEN, DBG_DIR_RX);	
	
	if (pkt->nb != NULL) {
        NdisFreeNetBuffer(pkt->nb);
        pkt->nb = NULL;
    }
	
	if(pkt->mdl != NULL) {
		MmPrepareMdlForReuse(pkt->mdl);
		pkt->mdl = NULL;
	}
	
ERROR_DEAL:
	wf_recv_release_source(padapter, pkt);
	return ret;
}


//hdr_len+iv_len+payload_len+mic_len
void wf_recv_proc_header(PADAPTER          padapter, wf_recv_pkt_t *pkt)
{
	prx_pkt_info_t prx_info = &pkt->nic_pkt.pkt_info;
	wf_u8 protected_flag = GetPrivacy(pkt->nic_pkt.pdata);
	wf_u8 *dst_ptr, *src_ptr;
	wf_u16 *fctrl;
	wf_u32 length = 0;
	wf_u8 mic_len;

	UNREFERENCED_PARAMETER(padapter);
	
	src_ptr = pkt->nic_pkt.pdata;
	dst_ptr = pkt->tmp_data;

	//LOG_D("prt=%d, algo=%d, hdr_len=%d, len=%d, iv=%d, icv=%d",
	//	protected_flag, prx_info->encrypt_algo, prx_info->wlan_hdr_len,
	//	pkt->nic_pkt->len, prx_info->iv_len, prx_info->icv_len);

	if(protected_flag) {
		switch (prx_info->encrypt_algo) {
        case _AES_:
		case _TKIP_:
		case _TKIP_WTMIC_:
			mic_len = ENC_MIC_LEN;
            break;
		
		case _WEP40_:
        case _WEP104_:
		case _WEP_WPA_MIXED_:
        default:
        	mic_len = 0;
            break;
	    }
		
		length = prx_info->wlan_hdr_len;
		wf_memcpy(dst_ptr, src_ptr, length);

		dst_ptr += length;
		src_ptr += length;
		src_ptr += prx_info->iv_len;//skip iv info

		pkt->nic_pkt.len -= (prx_info->iv_len+prx_info->icv_len+mic_len);

		length = pkt->nic_pkt.len - prx_info->wlan_hdr_len;
		wf_memcpy(dst_ptr, src_ptr, length);
	} else {
		length = pkt->nic_pkt.len;
		wf_memcpy(dst_ptr, src_ptr, length);
	}
	
	pkt->nic_pkt.pdata = pkt->tmp_data;
	fctrl = (wf_u16 *)pkt->nic_pkt.pdata;
	ClearPrivacy(fctrl);

#if 0
	wdn_net_info_st *wdn_info;
	wdn_info = wf_wdn_find_info(padapter->nic_info, get_ta(pkt->nic_pkt->pdata));
	if(GET_HDR_Type(pkt->nic_pkt->pdata) == MAC_FRAME_TYPE_DATA && 
		wdn_info->ieee8021x_blocked == wf_false) {
		LOG_D("hif_hdr_len=%d wlan_hdr=%d iv=%d", 
			pkt->nic_pkt->pkt_info.hif_hdr_len, 
			pkt->nic_pkt->pkt_info.wlan_hdr_len,
			pkt->nic_pkt->pkt_info.iv_len);
		print_buffer("rx", 0, pkt->nic_pkt->pdata, pkt->nic_pkt->len);
	}
#endif
}

void wf_recv_release_nbl(NDIS_HANDLE        mp_ctx, PNET_BUFFER_LIST nbl, ULONG flag)
{
	UNREFERENCED_PARAMETER(mp_ctx);
	UNREFERENCED_PARAMETER(nbl);
	UNREFERENCED_PARAMETER(flag);
}


#else

VOID
kalUpdateRxCSUMOffloadParam (
    IN PVOID pvPacket
    )
{
    PNDIS_PACKET                        prPacket = (PNDIS_PACKET)pvPacket;
	// TODO: 
	return;
}

wf_u32 wf_wlan_to_eth (prx_pkt_t ppkt)
{
	prx_pkt_info_t prx_info = &ppkt->pkt_info;
	wf_u8 *pbuf = ppkt->pdata;
	wf_u8 rmv_len;
	wf_u8 snap_hdr_offset;
	wf_u16 data_len;
	wf_u16 eth_type;
	wf_bool bsnap;
	struct wf_ethhdr *peth_hdr;

	if (prx_info->bdecrypted)
	{
		ppkt->len = ppkt->len - prx_info->icv_len;
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
		LOG_E("[wlan_to_eth] data_len error (pktlen:%d	rmv_len:%d)",
				ppkt->len,rmv_len);
		return -1;
	}

	data_len = ppkt->len - rmv_len;
	wf_memcpy(&eth_type, pbuf + rmv_len, 2);
	prx_info->eth_type = eth_type;

	pbuf += (rmv_len - sizeof(struct wf_ethhdr) + (bsnap ? 2 : 0));
	ppkt->pdata = pbuf;
	ppkt->len -= (rmv_len - sizeof(struct wf_ethhdr) + (bsnap ? 2 : 0));
		
	if (!pbuf)
	{
		LOG_D("pbuf == NULL");
		return -1;
	}

	peth_hdr = pbuf;
	wf_memcpy(peth_hdr->dest, prx_info->dst_addr, MAC_ADDR_LEN);
	wf_memcpy(peth_hdr->src, prx_info->src_addr, MAC_ADDR_LEN);
	peth_hdr->type = eth_type;
	
	if (!bsnap)
	{
		data_len = htons(data_len);
		wf_memcpy(pbuf + 12, (wf_u8 *)&data_len, sizeof(data_len));
	}

	return 0;
}



NDIS_STATUS wf_recv_data_submit(nic_info_st *pnic_info, wf_recv_pkt_t *pkt)
{
	PNDIS_PACKET prNdisPacket;
	PPNDIS_PACKET apvPkts;
	NDIS_STATUS 	rStatus;
	rx_pkt_t *nic_pkt;
	PNDIS_BUFFER	prNdisBuf;
	PADAPTER padapter = (PADAPTER)pnic_info->hif_node;
	wf_mib_info_t *mib_info = padapter->mib_info;
	wf_u32 u4SysTime;
	struct wf_ethhdr *cur_etherhdr;
	KIRQL oldIrql;

	nic_pkt = &pkt->nic_pkt;
	prNdisPacket = pkt->prPktDescriptor;
	apvPkts = &prNdisPacket;

	wf_wlan_to_eth(nic_pkt);
	kalUpdateRxCSUMOffloadParam (prNdisPacket);
	cur_etherhdr = (struct wf_ethhdr *) nic_pkt->pdata;
	if(ntohs(cur_etherhdr->type) == 0x888e)
	{
#if SET_ORG_PKT 
		GLUE_SET_PKT_FLAG_1X(prNdisPacket);
#endif
		LOG_D("EAPol frame received.");
	}
	
#if SET_ORG_PKT
	u4SysTime = (OS_SYSTIME)wf_get_time_tick();
	GLUE_SET_PKT_ARRIVAL_TIME(prNdisPacket, u4SysTime);

	/* Ether Hdr Len */
	GLUE_SET_PKT_HEADER_LEN(prNdisPacket, 14);
	
	/* Set the value of Frame Length */
	GLUE_SET_PKT_FRAME_LEN(prNdisPacket, (wf_u16)nic_pkt->len);
#endif
	NdisAllocateBuffer(&rStatus,
                       &prNdisBuf,
                       padapter->hBufPool,
                       (PVOID)nic_pkt->pdata,
                       nic_pkt->len);

	if (rStatus != NDIS_STATUS_SUCCESS) {
		LOG_E("NDIS allocate buffer failed");
        return NDIS_STATUS_FAILURE;
    }
	
	NdisChainBufferAtBack(prNdisPacket, prNdisBuf);
	NDIS_SET_PACKET_STATUS(prNdisPacket, NDIS_STATUS_RESOURCES);

	//wf_memcpy(prNdisPacket->MiniportReserved, pkt, sizeof(PVOID));
	//NDIS_SET_PACKET_STATUS(prNdisPacket, NDIS_STATUS_SUCCESS);

	KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);
	NdisMIndicateReceivePacket(padapter->MiniportAdapterHandle,
        apvPkts,
        1);
	KeLowerIrql(oldIrql);
	InterlockedIncrement(&mib_info->num_recv_ok.LowPart);
#if 0
	rStatus = NDIS_GET_PACKET_STATUS(prNdisPacket);
	if(rStatus == NDIS_STATUS_SUCCESS){
		LOG_D("---ZY_TEST---recv indication success");
	}
	else if(rStatus == NDIS_STATUS_PENDING)
	{
		LOG_D("---ZY_TEST---recv indication pending");
	}
#endif
	NdisUnchainBufferAtBack(prNdisPacket, &prNdisBuf);

	if (prNdisBuf) {
		NdisFreeBuffer(prNdisBuf);
	}
	
	NdisReinitializePacket(prNdisPacket);
	wf_recv_release_source(padapter, pkt);
	
	return NDIS_STATUS_SUCCESS;
}

//we always need translate header from 802.11 to 802.3, so we no longer need proc iv length
void wf_recv_proc_header(PADAPTER          padapter, wf_recv_pkt_t *pkt)
{
	return;
}

#endif

void wf_recv_mgmt_monitor(PADAPTER padapter, wf_recv_pkt_t *pkt)
{
	rx_pkt_info_t *pkt_info = &pkt->nic_pkt.pkt_info;
	wf_u8 *buffer;
	wf_u32 sub_type;
	wf_80211_mgmt_t *pmgmt;
	wf_ap_info_t *ap_info = padapter->ap_info;
	wf_u32 data_len;
	KIRQL irq = 0;
	
	buffer = pkt->nic_pkt.pdata;
	data_len = pkt->nic_pkt.len;

	pmgmt = (wf_80211_mgmt_t *)buffer;

	sub_type = wf_80211_get_frame_type(pmgmt->frame_control);

#ifndef MP_USE_NET_BUFFER_LIST
	if(sub_type == WF_80211_FRM_ASSOC_RESP )
	{
		wf_assoc_resp_info_update(padapter, &pkt->nic_pkt);
	}
#endif

	if(ap_info == NULL) {
		return;
	}

	if(!ap_info->valid) {
		//LOG_E("ap info is invalid!\n");
		return;
	}

	if(wf_memcmp(ap_info->scan_info.bssid, pmgmt->bssid, sizeof(wf_80211_bssid_t)) != 0) {
		return;
	}	

	KeAcquireSpinLock(&ap_info->lock, &irq);
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
	KeReleaseSpinLock(&ap_info->lock, irq);
}


void wf_recv_data_reorder(rx_pkt_t *pkt, PADAPTER padapter)
{
    wdn_net_info_st *pwdn_info = pkt->wdn_info;
    wf_s32 prio    = 0;
    wf_s32 seq_num = 0;
	wf_recv_pkt_t *recv_pkt;
    
    prio = pkt->pkt_info.qos_pri;
    seq_num = pkt->pkt_info.seq_num;
	recv_pkt = CONTAINING_RECORD(pkt, wf_recv_pkt_t, nic_pkt);

    if (NULL == pwdn_info || (prio > 15)) {
    	wf_recv_release_source(padapter, recv_pkt);

        if (prio > 15)
            LOG_E("pri error:%d",prio);
        return;
    }

    if(0 == pkt->pkt_info.qos_flag) {
    	wf_recv_data_submit(padapter, recv_pkt);
        //upload_skb(ndev, pkt->pskb);
        return;
    }

	if(pwdn_info->ba_ctl == NULL) {
		wf_recv_data_submit(padapter, recv_pkt);
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


//#pragma LOCKEDCODE
NDIS_STATUS wf_recv_frame_dispatch(PADAPTER padapter, wf_recv_pkt_t *pkt)
{
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	rx_pkt_info_t *pkt_info = &pkt->nic_pkt.pkt_info;
	switch(pkt_info->frame_type){
	case MAC_FRAME_TYPE_MGT:
		wf_recv_mgmt_monitor(padapter, pkt);
	case MAC_FRAME_TYPE_CTRL:
		wf_recv_release_source(padapter, pkt);
		break;
	
	case MAC_FRAME_TYPE_DATA:
		wf_recv_proc_header(padapter, pkt);
		#if RX_REORDER_ENABLE
			wf_recv_data_reorder(&pkt->nic_pkt, padapter);
		#else
			ret = wf_recv_data_submit(padapter, pkt);
			if(ret != NDIS_STATUS_SUCCESS) {
				LOG_E("submit data to ndis failed!");
			}
		#endif
		break;
		
	default:
		LOG_E("frame type = %x", pkt_info->frame_type);
		ret = NDIS_STATUS_NOT_RECOGNIZED;
		break;
	}
	
	return ret;
}

NDIS_STATUS wf_recv_data_dispatch(PADAPTER padapter, wf_recv_preproc_t *pre_info)
{
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	wf_usb_info_t *usb_info = padapter->usb_info;
	wf_recv_info_t *recv_info = padapter->recv_info;
	nic_info_st *nic_info = padapter->nic_info;
	wf_recv_pkt_t *pkt = NULL;
	PLIST_ENTRY plist = NULL;
	struct rx_pkt_info *pinfo;
	wf_dbg_info_t *dbg_info = padapter->dbg_info;
	
	switch(pre_info->pkt_type) {
	case WF_PKT_TYPE_CMD:
		LOG_D("recv a cmd frame! len:%d", pre_info->pkt_len);
		if(pre_info->pkt_len <= SIDE_ROAD_BUF_SIZE) {
			wf_memcpy(usb_info->cmd_buffer, pre_info->pkt_buf, pre_info->pkt_len);
			usb_info->cmd_len = pre_info->pkt_len;
			KeSetEvent(&usb_info->cmd_finish, 0, FALSE);
		}
		break;
	case WF_PKT_TYPE_FW:
		LOG_D("recv a firmware frame! len:%d", pre_info->pkt_len);
		if(pre_info->pkt_len <= SIDE_ROAD_BUF_SIZE) {
			wf_memcpy(usb_info->fw_buffer, pre_info->pkt_buf, pre_info->pkt_len);
			usb_info->fw_len = pre_info->pkt_len;
			KeSetEvent(&usb_info->fw_finish, 0, FALSE);
		}
		break;
	case WF_PKT_TYPE_REG:
		LOG_D("recv a register frame! len:%d", pre_info->pkt_len);
		if(pre_info->pkt_len <= SIDE_ROAD_BUF_SIZE) {
			wf_memcpy(usb_info->reg_buffer, pre_info->pkt_buf, pre_info->pkt_len);
			usb_info->reg_len = pre_info->pkt_len;
			KeSetEvent(&usb_info->reg_finish, 0, FALSE);
		}
		break;
	case WF_PKT_TYPE_FRAME:	
		if(pre_info->rpt_sel) {
			//LOG_D("c2h frame");
			//wf_rx_notice_process(pkt->nic_pkt->pdata, (wf_u16)(pkt->nic_pkt->len-32));
		} else {
			if(padapter->dev_state != WF_DEV_STATE_RUN) {
				LOG_D("can not recv data");
				ret = NDIS_STATUS_FAILURE;
				break;
			}
			
			if(IsListEmpty(&recv_info->comm_free.head) || WF_CANNOT_RUN(nic_info)) {
				LOG_E("free=%d, mgmt_pend=%d, data_pend=%d", 
					recv_info->comm_free.cnt, recv_info->mgmt_pend.cnt, recv_info->data_pend.cnt);
				dbg_info->drop_by_que++;
				ret = NDIS_STATUS_FAILURE;
				break;
			}

			plist = wf_pkt_data_deque(&recv_info->comm_free, QUE_POS_HEAD);
			if(plist == NULL) {
				LOG_E("can't get data list");
				ret = NDIS_STATUS_FAILURE;
				break;
			}
			
			pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
			wf_memset(&pkt->nic_pkt, 0, sizeof(rx_pkt_t));
			InterlockedIncrement(&recv_info->proc_cnt);

			pinfo = &pkt->nic_pkt.pkt_info;

			pkt->nic_pkt.p_nic_info = nic_info;
			pkt->src_buffer 	= pre_info->src_buf;
			pkt->buffer 		= pre_info->pkt_buf;
			pkt->buf_hdl 		= pre_info->buf_hdl;
			
			pinfo->pkt_type 	= pre_info->pkt_type;
			pinfo->pkt_len 		= pre_info->pkt_len;
			pinfo->hif_hdr_len 	= pre_info->hdr_len;
			pinfo->frame_type 	= GET_HDR_Type(pre_info->pkt_buf + pre_info->hdr_len);

			pkt->nic_pkt.pdata 	= pkt->buffer;
			pkt->nic_pkt.len 	= pre_info->pkt_len;
			pkt->net_if			= padapter;
			
			WdfObjectReference(pkt->buf_hdl);
			wf_pkt_data_enque((pinfo->frame_type == MAC_FRAME_TYPE_DATA) ? 
				&recv_info->data_pend : &recv_info->mgmt_pend, &pkt->list, QUE_POS_TAIL);
			KeSetEvent(&recv_info->rx_evt, 0, FALSE);
		}
		break;
	default:
		LOG_E("PKT TYPE = %x", pre_info->pkt_type);
		dbg_info->drop_by_type++;
		ret = NDIS_STATUS_NOT_RECOGNIZED;
		break;
	}
	
	return ret;
}


void wf_recv_complete_callback(void *adapter, WDFMEMORY BufferHdl, ULONG data_len, ULONG offset)
{
	PADAPTER padapter = adapter;
	LONG remain_len;
	ULONG pkt_len;
	UCHAR *curr_buffer, *temp_buffer;
	wf_recv_preproc_t pre_info;
	wf_dbg_info_t *dbg_info = padapter->dbg_info;
	nic_info_st *nic_info = padapter->nic_info;

	temp_buffer = WdfMemoryGetBuffer(BufferHdl, NULL);
	curr_buffer = temp_buffer + offset;

	if(data_len > MAX_RECEIVE_BUFFER_SIZE || 
		data_len < RXD_SIZE_COMMON || 
		temp_buffer == NULL) {
        LOG_E("Recv invalid USB bulk in length=%d, offset=%d buffer 0x%p\n",
        	data_len, offset, curr_buffer);
		print_buffer("invalid data", 0, curr_buffer, data_len);
        return;
    }

	remain_len = data_len;

	while(1){
		if(WF_CANNOT_RUN(nic_info)) {
			LOG_D("exit pkt proc");
	        break;
	    }

		pre_info.buf_hdl = BufferHdl;
		pre_info.src_buf = temp_buffer;
		pre_info.pkt_buf = curr_buffer;

		if(wf_recv_get_pre_info(curr_buffer, &pre_info) != NDIS_STATUS_SUCCESS) {
			//LOG_E("get pre info failed!");
			return;
		}

		if(wf_recv_data_dispatch(padapter, &pre_info) != NDIS_STATUS_SUCCESS) {
			LOG_E("data dispatch failed!");
			return;
		}

		pkt_len = WF_RND8(pre_info.pkt_len);
		curr_buffer += pkt_len;
		remain_len -= pkt_len;

		if(remain_len <= 0) {
			break;
		}
	}
}

VOID wf_recv_data_release_thread(PADAPTER padapter)
{
	wf_recv_info_t *recv_info ;
	PLIST_ENTRY plist = NULL;
	wf_recv_pkt_t *pkt = NULL;
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	nic_info_st *nic_info = padapter->nic_info;
	wf_data_que_t *to_be_released;
	//struct rx_pkt_info *pinfo;
	wf_dbg_info_t *dbg_info;
	PRKTHREAD pthread;
	KPRIORITY prio;
	
	recv_info = padapter->recv_info;
	to_be_released = &recv_info->to_be_released;

	pthread = KeGetCurrentThread();
	if(pthread != NULL) {
		prio = KeSetPriorityThread(pthread, LOW_REALTIME_PRIORITY);
		LOG_D("old_prio=%d, new_prio=%d", prio, KeQueryPriorityThread(pthread));
	} else {
		LOG_W("pthread is NULL");
	}
	
	while(1) {
		KeWaitForSingleObject(&recv_info->rx_release_evt, Executive, KernelMode, TRUE, NULL);

		if(recv_info->rx_release_thread->stop) {
			LOG_D("stop recv resource release proc");
			break;
		}

		dbg_info = padapter->dbg_info;

		while(!IsListEmpty(&to_be_released->head)) {
			plist = wf_pkt_data_deque(to_be_released, QUE_POS_HEAD);
			
			if(plist == NULL) {
				LOG_E("get recv list failed");
				continue;
			}
			pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
			//InterlockedIncrement(&recv_info->proc_cnt);
			// TODO: Delete this dbg function to improve the performance.  2021/09/03
			// wf_dbg_counter_add(prAdapter->dbg_info, nic_pkt->len-WLAN_HDR_A3_QOS_LEN, DBG_DIR_RX);
			wf_recv_release_source(padapter, pkt);
			
		}
	}

	wf_os_api_thread_exit(recv_info->rx_release_thread);
}


VOID wf_recv_data_thread(PADAPTER padapter)
{
	wf_recv_info_t *recv_info ;
	PLIST_ENTRY plist = NULL;
	wf_recv_pkt_t *pkt = NULL;
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	nic_info_st *nic_info = padapter->nic_info;
	wf_data_que_t *pdata, *pmgmt, *pfree;
	//struct rx_pkt_info *pinfo;
	wf_dbg_info_t *dbg_info;
	PRKTHREAD pthread;
	KPRIORITY prio;
	
	recv_info = padapter->recv_info;
	pdata = &recv_info->data_pend;
	pmgmt = &recv_info->mgmt_pend;
	pfree = &recv_info->comm_free;

	pthread = KeGetCurrentThread();
	if(pthread != NULL) {
		prio = KeSetPriorityThread(pthread, LOW_REALTIME_PRIORITY);
		LOG_D("old_prio=%d, new_prio=%d", prio, KeQueryPriorityThread(pthread));
	} else {
		LOG_W("pthread is NULL");
	}
	
	while(1) {
		KeWaitForSingleObject(&recv_info->rx_evt, Executive, KernelMode, TRUE, NULL);

		if(recv_info->rx_thread->stop) {
			LOG_D("stop recv proc");
			break;
		}

		dbg_info = padapter->dbg_info;

		while(!IsListEmpty(&pmgmt->head) || !IsListEmpty(&pdata->head)) {
			if(WF_CANNOT_RUN(nic_info)) {
				LOG_D("exit pkt proc");
		        break;
		    }

			if(!IsListEmpty(&pmgmt->head)) {
				plist = wf_pkt_data_deque(pmgmt, QUE_POS_HEAD);
			} else if(!IsListEmpty(&pdata->head)) {
				plist = wf_pkt_data_deque(pdata, QUE_POS_HEAD);
			}
			
			if(plist == NULL) {
				LOG_E("get recv list failed");
				continue;
			}
			pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
			//InterlockedIncrement(&recv_info->proc_cnt);

			ret = wf_recv_parser_rx_desc(pkt);
			if(ret != NDIS_STATUS_SUCCESS) {
				LOG_E("pkt parser rxd error!");
				wf_recv_release_source(padapter, pkt);
				continue;
			}

			ret = wf_recv_parser_phy_status(padapter->nic_info, pkt);
			if(ret != NDIS_STATUS_SUCCESS) {
				LOG_E("recv packet parser rx phy status failed!\n");
				wf_recv_release_source(padapter, pkt);
				continue;
			}

			//send the frame to nic
			ret = wf_rx_common_process(&pkt->nic_pkt);
			if(ret) {
				wf_recv_release_source(padapter, pkt);
				continue;
			}
			
			ret = wf_recv_frame_dispatch(padapter, pkt);
			if(ret != NDIS_STATUS_SUCCESS) {
				LOG_E("recv pkt dispatch failed!\n");
				wf_recv_release_source(padapter, pkt);
				dbg_info->drop_by_type++;
				continue;
			}
		}
	}

	wf_os_api_thread_exit(recv_info->rx_thread);
}



NDIS_STATUS wf_recv_pkt_init(void *param)
{
	PADAPTER padapter = param;
	wf_recv_info_t *recv_info = padapter->recv_info;
	//wf_data_que_t *pfree = &recv_info->comm_free;
	wf_recv_pkt_t *pkt;
	int i;
#ifdef MP_USE_NDIS5
	NDIS_STATUS           status;
#endif // MP_USE_NDIS5
#if defined(MP_USE_NET_BUFFER_LIST)
	NET_BUFFER_LIST_POOL_PARAMETERS     nbl_pool_param;
    NET_BUFFER_POOL_PARAMETERS          nb_pool_param;
	
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
        return NDIS_STATUS_FAILURE;
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
        return NDIS_STATUS_FAILURE;
    }
	
	for(i=0; i<RECV_QUEUE_DEPTH; i++) {
		pkt = &recv_info->packet[i];
		NdisZeroMemory(pkt, sizeof(wf_recv_pkt_t));
		pkt->nbl = NdisAllocateNetBufferList(recv_info->nbl_pool, MP_RECEIVE_NBL_CONTEXT_SIZE, 0);
		pkt->tmp_data = wf_malloc(MAX_RECV_PKT_SIZE);
		if(pkt->tmp_data == NULL)
		{
			LOG_E("malloc temp data failed!\n");
			return NDIS_STATUS_FAILURE;
		}
		if(pkt->nbl == NULL) {
			LOG_E("malloc packet net buffer list failed!\n");
			return NDIS_STATUS_FAILURE;
		}
		NET_BUFFER_LIST_INFO(pkt->nbl, MediaSpecificInformation) = &pkt->rx_ctx;
		wf_pkt_data_enque(&recv_info->comm_free, &pkt->list, QUE_POS_TAIL);
	}
#else
	/* Allocate packet descriptor pool:
		   Set up a pool of data for us to build our packet array out of
		   for indicating groups of packets to NDIS.
		   This could be quite the memory hog, but makes management
		   of the pointers associated with Asynchronous memory allocation
		   easier. */
	NdisAllocatePacketPoolEx(&status,
							 &padapter->hPktPool,
							 (UINT) RECV_QUEUE_DEPTH,
							 (UINT) RECV_QUEUE_DEPTH,
							 4 * sizeof(PVOID));
	if(status != NDIS_STATUS_SUCCESS){
		LOG_E("Allocate packet descriptor pool failed!");
		return NDIS_STATUS_FAILURE;
	}
	for(i=0; i<RECV_QUEUE_DEPTH; i++) {
		pkt = &recv_info->packet[i];
		NdisZeroMemory(pkt, sizeof(wf_recv_pkt_t));
	
		NdisAllocatePacket(&status,
						   &pkt->prPktDescriptor,
						   padapter->hPktPool);
		if(status != NDIS_STATUS_SUCCESS){
			LOG_E("Allocate packet descriptor failed!");
			return NDIS_STATUS_FAILURE;
		}
		
		wf_pkt_data_enque(&recv_info->comm_free, &pkt->list, QUE_POS_TAIL);
	}
    NdisAllocateBufferPool(&status,
                   &padapter->hBufPool,
                   (UINT)RECV_QUEUE_DEPTH);
	if(status != NDIS_STATUS_SUCCESS){
			LOG_E("Allocate buffer pool failed!");
			return NDIS_STATUS_FAILURE;
	}
#endif

	return NDIS_STATUS_SUCCESS;
}

void wf_recv_pkt_deinit(void *param)
{
	PADAPTER padapter = param;
	wf_recv_info_t *recv_info = padapter->recv_info;
	wf_data_que_t *pdata, *pmgmt, *pfree;
	PLIST_ENTRY plist;
	wf_recv_pkt_t *pkt;
	int i;

	pdata = &recv_info->data_pend;
	pmgmt = &recv_info->mgmt_pend;
	pfree = &recv_info->comm_free;

	while(!IsListEmpty(&pdata->head)) {
		
		plist = wf_pkt_data_deque(pdata, QUE_POS_HEAD);
		pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
	}
	
	while(!IsListEmpty(&pmgmt->head)) {
		plist = wf_pkt_data_deque(pmgmt, QUE_POS_HEAD);
		pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
	}

	while(!IsListEmpty(&pfree->head)) {
		plist = wf_pkt_data_deque(pfree, QUE_POS_HEAD);
		pkt = CONTAINING_RECORD(plist, wf_recv_pkt_t, list);
	}
	
#if defined(MP_USE_NET_BUFFER_LIST)
	for(i=0; i<RECV_QUEUE_DEPTH; i++) {
		pkt = &recv_info->packet[i];
		wf_free(pkt->tmp_data);
	}

	if (recv_info->nb_pool) {
        NdisFreeNetBufferPool(recv_info->nb_pool);
        recv_info->nb_pool = NULL;
    }
	
	if (recv_info->nbl_pool) {
        NdisFreeNetBufferListPool(recv_info->nbl_pool);
        recv_info->nbl_pool = NULL;
    }
#else
	for(i=0; i<RECV_QUEUE_DEPTH; i++) {
		pkt = &recv_info->packet[i];
		wf_free(pkt->tmp_data);
		NdisFreePacket(pkt->prPktDescriptor);
	}
	if(padapter->hBufPool){		
		NdisFreeBufferPool(padapter->hBufPool);
		padapter->hBufPool = NULL;
	}
	if(padapter->hPktPool){
		NdisFreePacketPool(padapter->hPktPool);
		padapter->hPktPool = NULL;
	}
#endif
}

NDIS_STATUS wf_recv_init(void *param)
{
	PADAPTER padapter = param;
	wf_recv_info_t *recv_info = NULL;

	LOG_D("start init recv!\n");
#if NDIS51_MINIPORT
	//recv_info = ExAllocatePoolWithTagPriority(NonPagedPool, sizeof(wf_recv_info_t), MP_RX_NET_BUFFER_POOL_TAG, NormalPoolPriority);
	recv_info = wf_malloc(sizeof(wf_recv_info_t));
#else
	recv_info = wf_malloc(sizeof(wf_recv_info_t));
#endif
	if(recv_info == NULL) {
		LOG_E("malloc recv info failed!\n");
		return NDIS_STATUS_FAILURE;
	}else{
		NdisZeroMemory(recv_info, sizeof(wf_recv_info_t));
	}

	padapter->recv_info = recv_info;
	recv_info->padapter = padapter;

	wf_data_que_init(&recv_info->mgmt_pend);
	wf_data_que_init(&recv_info->data_pend);
	wf_data_que_init(&recv_info->comm_free);
	
	if(wf_recv_pkt_init(param) != NDIS_STATUS_SUCCESS) {
		LOG_E("recv pkt init failed");
		return NDIS_STATUS_FAILURE;
	}
	
	KeInitializeEvent(&recv_info->rx_evt, SynchronizationEvent, FALSE);

	recv_info->rx_thread = wf_os_api_thread_create(NULL, "recv_thread", wf_recv_data_thread, padapter);
	if (NULL == recv_info->rx_thread) {
			LOG_E("[wf_recv_init] create recv data thread failed");
			return NDIS_STATUS_FAILURE;
	}

#if NDIS51_MINIPORT
	KeInitializeEvent(&recv_info->rx_release_evt, SynchronizationEvent, FALSE);

	recv_info->rx_release_thread = wf_os_api_thread_create(NULL, "recv_resource_release_thread", wf_recv_data_release_thread, padapter);
	
	if (recv_info->rx_release_thread == NULL) {
				LOG_E("[wf_recv_init] create recv release thread failed");
				return NDIS_STATUS_FAILURE;
	}
#endif	
	return NDIS_STATUS_SUCCESS;
}

void wf_recv_deinit(void *param)
{
	PADAPTER padapter = param;
	wf_recv_info_t *recv_info = padapter->recv_info;

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
#if NDIS51_MINIPORT
	if(recv_info->rx_release_thread != NULL){
		recv_info->rx_release_thread->stop =wf_true;
		KeSetEvent(&recv_info->rx_release_evt, 0 , FALSE);
		wf_os_api_thread_destory(recv_info->rx_release_thread);
		recv_info->rx_release_thread = NULL;
	}		
#endif
	wf_recv_pkt_deinit(param);

	wf_free(recv_info);
}























