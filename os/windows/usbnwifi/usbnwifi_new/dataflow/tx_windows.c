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
#include "wf_os_api.h"
#include "wf_debug.h"
//#include "pcomp.h"
#include "pcomp.h"
#include "common.h"

#include "tx_windows.h"
#include "rx_windows.h"
#include "wf_oids_adapt.h"


typedef enum wf_queue_map_e{
	WIN_QUE_VO 		= 0,
	WIN_QUE_VI 		= 1,
	WIN_QUE_BE 		= 2,
	WIN_QUE_BK 		= 3,

	WIN_QUE_BCN 	= 4,
	WIN_QUE_MGMT 	= 5,
	WIN_QUE_HIGH 	= 6,
	WIN_QUE_CMD 	= 8
}wf_queue_map_e;


#define DATA_FRAME_HDR_SHORT    24  /* ALL data frame present */
#define ADDRESS4_LEN            6   /* if To/From_DS=1,address4 present */
#define QOS_CONTRL_LEN          2   /* if qos field present in subtype field */
#define HT_CONTRL_LEN           4


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
		DbgPrint("%02x ", src[i]);
	}
	DbgPrint("\n");
}

#define USB_REQUEST_CODE
extern NTSTATUS
wf_usb_send_pkt_async (
	PADAPTER    adapter,
    UCHAR           QueueType,
    ULONG           BufferLength,   // Number of bytes in pData buffer
    PVOID           Buffer,
    void			*frame
    );


int wf_xmit_addr2pipe(int addr)
{
	int pipe;

	switch(addr)
    {
    case WIN_QUE_BE:
    case WIN_QUE_BK:
    case WIN_QUE_VI:
    case WIN_QUE_VO:
    case WIN_QUE_CMD:  // hjy
        pipe = 1;
        break;

    //case CMD_QUEUE_INX:
    case WIN_QUE_MGMT:
    case WIN_QUE_BCN:
    case WIN_QUE_HIGH:
        pipe = 3;
        break;

    default:
        pipe = 1;
        break;
    }

	return pipe;
}

PLIST_ENTRY wf_xmit_enqueue(LIST_ENTRY *head, LIST_ENTRY *node, KSPIN_LOCK *lock, wf_u8 direction)
{
	if(direction) {
		ExInterlockedInsertTailList(head, node, lock);
	} else {
		
	}
}

PLIST_ENTRY wf_xmit_dequeue(LIST_ENTRY *head, KSPIN_LOCK *lock, wf_u8 direction)
{
	return ExInterlockedRemoveHeadList(head, lock);
}

void wf_xmit_complet_callback(void *pnic_info, void *frame)
{
	nic_info_st *nic_info = pnic_info;
	PADAPTER padapter = nic_info->hif_node;
	wf_xmit_info_t *xmit_info = padapter->xmit_info;
	wf_usb_req_t *usb_req = frame;
	wf_u32 use_time;
	static wf_u64 all_time = 0;
	static wf_u32 all_cnt = 0;
	static wf_u32 filter_cnt = 0;
	static wf_u32 avg_time = 0;
	wf_u32 cnt_th = 1;

	//nic send data notify
	if(usb_req->tx_callback_func) {
		usb_req->tx_callback_func(usb_req->tx_info, usb_req->param);
	}

	if(usb_req->addr == 3) {
		InterlockedDecrement(&xmit_info->usb_mgmt_cnt);
	} else {
		InterlockedDecrement(&xmit_info->usb_data_cnt);
	}
	InterlockedIncrement(&xmit_info->usb_free_cnt);

	all_cnt++;

	if(all_cnt > 20000) {
		DbgPrint("m=%d, d=%d, f=%d, p=%d, f=%d\n",
			xmit_info->usb_mgmt_cnt, xmit_info->usb_data_cnt, xmit_info->usb_free_cnt,
			xmit_info->data_pend_cnt, xmit_info->data_free_cnt);
		all_cnt = 0;
	}
	
	ExInterlockedInsertTailList(&xmit_info->usb_free_head, &usb_req->list, &xmit_info->usb_free_lock);
}

void wf_xmit_mgmt_fram_monitor(void *adapter, wf_u8 *buff, wf_u32 buff_len)
{
	PADAPTER padapter = adapter;
	wf_ap_info_t *ap_info = padapter->ap_info;
	wf_u8 offset = 0;
	wf_u8 *ptr, sub_type;
	wf_80211_mgmt_t *pmgmt;
	wf_u32 frame_type;

	if(ap_info == NULL) {
		return;
	}

	if(ap_info->scan_info == NULL) {
		return;
	}

	if(ap_info->assoc_req_len != 0) {
		return;
	}

	pmgmt = buff + TXDESC_SIZE;
	sub_type = wf_80211_get_frame_type(pmgmt->frame_control);
	frame_type = GET_HDR_Type(buff+TXDESC_SIZE);
	if(frame_type == MAC_FRAME_TYPE_MGT && sub_type == WF_80211_FRM_ASSOC_REQ) {
		if(memcmp(ap_info->scan_info->bssid, pmgmt->bssid, sizeof(wf_80211_bssid_t)) == 0) {
			ap_info->assoc_req_len = buff_len - offset - TXDESC_SIZE;
			memcpy(ap_info->assoc_req, buff + TXDESC_SIZE + offset, 
				buff_len - offset - TXDESC_SIZE);
			//print_buffer("assoc_req", 0, buff, buff_len);
		}
	}
}


int wf_xmit_list_empty(void *adapter)
{
	PADAPTER padapter = adapter;
	wf_xmit_info_t *xmit_info = padapter->xmit_info;

	if(IsListEmpty(&xmit_info->usb_mgmt_head) && IsListEmpty(&xmit_info->usb_data_head)) {
		LOG_E("the tx resource is full");
		return 1;
	}

    return 0;
}

int wf_xmit_list_insert(void *adapter,wf_u8 agg_num,char *buff, wf_u32 buff_len, wf_u32 addr, int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param)
{
	PADAPTER padapter = adapter;
	wf_xmit_info_t *xmit_info = padapter->xmit_info;
	PLIST_ENTRY 		plist;
	wf_usb_req_t 		*usb_req;

	if(IsListEmpty(&xmit_info->usb_free_head)) {
		LOG_E("the tx resource is empty");
		return -1;
	}

	//need use usb trans layer list
	plist = ExInterlockedRemoveHeadList(&xmit_info->usb_free_head, &xmit_info->usb_free_lock);
	if(plist == NULL) {
		LOG_E("get tx resource failed!\n");
		return -1;
	}
	
	usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);

	usb_req->data_buf 			= buff;
	usb_req->data_len 			= buff_len;
	usb_req->addr 				= wf_xmit_addr2pipe(addr);
	usb_req->tx_callback_func 	= tx_callback_func;
	usb_req->tx_info 			= tx_info;
	usb_req->param 				= param;

	wf_xmit_mgmt_fram_monitor(padapter, buff, buff_len);

	if(usb_req->addr == 3) {
		InterlockedIncrement(&xmit_info->usb_mgmt_cnt);
		ExInterlockedInsertTailList(&xmit_info->usb_mgmt_head, &usb_req->list, &xmit_info->usb_mgmt_lock);
	} else {
		InterlockedIncrement(&xmit_info->usb_data_cnt);
		ExInterlockedInsertTailList(&xmit_info->usb_data_head, &usb_req->list, &xmit_info->usb_data_lock);
	}
	InterlockedDecrement(&xmit_info->usb_free_cnt);
	KeSetEvent(&xmit_info->usb_evt, 0, FALSE);

    return 0;
}

void wf_usb_xmit_thread(PADAPTER         padapter)
{
	wf_xmit_info_t *xmit_info = NULL;
	PLIST_ENTRY plist = NULL;
	wf_usb_req_t *usb_req = NULL;
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	UCHAR *buffer, pipe;
	nic_info_st *nic_info = padapter->nic_info;
	
	xmit_info = padapter->xmit_info;
	while(1) {
		KeWaitForSingleObject(&xmit_info->usb_evt, Executive, KernelMode, TRUE, NULL);

		if(xmit_info->usb_thread->stop) {
			break;
		}
		
		while(!IsListEmpty(&xmit_info->usb_mgmt_head) || !IsListEmpty(&xmit_info->usb_data_head)) {
			if(WF_CANNOT_RUN(nic_info)) {
		        break;
		    }

			if(!IsListEmpty(&xmit_info->usb_mgmt_head)) {
				plist = ExInterlockedRemoveHeadList(&xmit_info->usb_mgmt_head, &xmit_info->usb_mgmt_lock);
			} else if(!IsListEmpty(&xmit_info->usb_data_head)) {
				plist = ExInterlockedRemoveHeadList(&xmit_info->usb_data_head, &xmit_info->usb_data_lock);
			} else {
				break;
			}
			
			usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);
			if(usb_req->data_buf == NULL) {
				LOG_E("data buffer is NULL");
				ExInterlockedInsertTailList(&xmit_info->usb_free_head, &usb_req->list, &xmit_info->usb_free_lock);
				continue;
			}
			//LOG_D("data_addr=%x len=%d", usb_req->data_buf, usb_req->data_len);
			ret = wf_usb_send_pkt_async (padapter, usb_req->addr, usb_req->data_len, usb_req->data_buf, usb_req);
			if(ret != NDIS_STATUS_SUCCESS) {
				wf_xmit_complet_callback(padapter->nic_info, usb_req);
			}
		}
	}

	wf_os_api_thread_exit(xmit_info->usb_thread);
}


#define XMIT_DATA_CODE

int wf_xmit_get_info(wf_xmit_pkt_t *ppkt, wf_u16 *peth_type, wf_u16 *phdr_len)
{
    wf_u8 *pbuf = ppkt->ptempbuf;
	wf_u16 wlan_hdr_len;
    wf_u8 rmv_len;
    wf_u8 data_len;
    wf_u8 snap_hdr_offset;
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
    rmv_len = snap_hdr_offset + (bsnap ? SNAP_HDR_SIZE : 0);

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

NDIS_STATUS wf_xmit_pkt_wlan2eth(PADAPTER padapter, void *ppkt)
{
	wf_xmit_pkt_t *pkt = ppkt;
	struct wf_ethhdr *etherhdr;
	data_frame_header_t hdr_80211;
	wf_u16 eth_type, hdr_len, msdu_len;
	wf_u16 eth_type_temp;
	UCHAR *ptr;

	//save 802.11 mac header
	NdisMoveMemory(&hdr_80211, pkt->ptempbuf, sizeof(hdr_80211));

	wf_xmit_get_info(pkt, &eth_type, &hdr_len);
	eth_type_temp = ntohs((unsigned short)eth_type);
	//LOG_D("eth_type=[%x][%x], hdr_len=%d", eth_type, eth_type_temp, hdr_len);

	etherhdr = pkt->ptempbuf + hdr_len - sizeof(struct wf_ethhdr);
	msdu_len = pkt->data_len - hdr_len + sizeof(struct wf_ethhdr);
	pkt->data_len = msdu_len;
	pkt->ptempbuf = (UCHAR *)etherhdr;
	pkt->xframe.nic_info = padapter->nic_info;

	NdisMoveMemory(&etherhdr->type, &eth_type, 2);
	NdisMoveMemory(etherhdr->dest, GetAddr3Ptr(&hdr_80211), MAC_ADDR_LEN);
	NdisMoveMemory(etherhdr->src, GetAddr2Ptr(&hdr_80211), MAC_ADDR_LEN);
	
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_xmit_fill_attrib(PADAPTER padapter, void *ppkt)
{
	wf_xmit_pkt_t *pkt = ppkt;
	struct wf_ethhdr *etherhdr;
	data_frame_header_t hdr_80211;
	wf_u16 eth_type, hdr_len, msdu_len;
	wf_u16 eth_type_temp;
	UCHAR *ptr;

	//save 802.11 mac header
	NdisMoveMemory(&hdr_80211, pkt->ptempbuf, sizeof(hdr_80211));

	wf_xmit_get_info(pkt, &eth_type, &hdr_len);
	eth_type_temp = ntohs((unsigned short)eth_type);
	//LOG_D("eth_type=[%x][%x], hdr_len=%d", eth_type, eth_type_temp, hdr_len);

	etherhdr = pkt->ptempbuf + hdr_len - sizeof(struct wf_ethhdr);
	msdu_len = pkt->data_len - hdr_len + sizeof(struct wf_ethhdr);
	pkt->data_len = msdu_len;
	pkt->ptempbuf = (UCHAR *)etherhdr;
	pkt->xframe.nic_info = padapter->nic_info;

	NdisMoveMemory(&etherhdr->type, &eth_type, 2);
	NdisMoveMemory(etherhdr->dest, GetAddr3Ptr(&hdr_80211), MAC_ADDR_LEN);
	NdisMoveMemory(etherhdr->src, GetAddr2Ptr(&hdr_80211), MAC_ADDR_LEN);
	//if(iphdr->proto == 0x02) {
	//	print_buffer("802.3 header", 0, pkt->ptempbuf, pkt->data_len);
	//}
	//use 802.3 mac header fill pkt attr
	wf_xmit_frame_init(padapter->nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len);
	
	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS wf_xmit_copy_nbl2buffer(PNET_BUFFER_LIST nbl, void *pkt_buff)
{
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

	if(IsListEmpty(&xmit_info->free_head)) {
		return -4;
	}

	return 0;
}


static int wf_xmit_add_ba(nic_info_st *nic_info, struct xmit_frame *pxmitframe)
{
	wf_u8 *mem_addr,issued;
	wf_u32 ff_hwaddr;
	wf_bool ret = wf_true;
	wf_bool inner_ret = wf_true;
	wf_bool blast;
	int t, sz, w_sz, pull = 0;
	//struct xmit_buf *pxmitbuf = pxmitframe->pxmitbuf;
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


static wf_bool wf_xmit_send_complete(nic_info_st *nic_info, wf_xmit_pkt_t *pkt)
{
	PADAPTER padapter = nic_info->hif_node;
	wf_xmit_info_t *xmit_info = padapter->xmit_info;

	if(pkt == NULL) {
		LOG_E("pkt is NULL");
		return wf_false;
	}

	pkt->ptempbuf = NULL;
	pkt->is_preproc = wf_false;
	InterlockedIncrement(&xmit_info->data_free_cnt);
	InterlockedDecrement(&xmit_info->data_pend_cnt);
	ExInterlockedInsertTailList(&xmit_info->free_head, &pkt->list, &xmit_info->data_free_lock);
	//if(pkt->nbl != NULL)
	//	NdisMSendNetBufferListsComplete(padapter->MiniportAdapterHandle, pkt->nbl, pkt->dispatchlevel);
    return wf_true;
}

//need modify with linux driver
static wf_bool wf_xmit_send_frag(nic_info_st *nic_info, wf_xmit_pkt_t *pkt, wf_bool ack)
{
    wf_u8 val;
    wf_u32 curTime,endTime,timeout;
    wf_u8 *mem_addr;
    wf_u32 ff_hwaddr;
    wf_bool bRet = wf_true;
    int ret;
    wf_bool inner_ret = wf_true;
    wf_bool blast = wf_false;
    int t, sz, w_sz, pull = 0;
    struct xmit_frame *pxmitframe = &pkt->xframe;
    hw_info_st *hw_info = nic_info->hw_info;
    //tx_info_st *tx_info = nic_info->tx_info;
    sec_info_st *sec_info = nic_info->sec_info;
    mlme_state_e state;
    wf_u32  txlen = 0;

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
            ret = wf_io_write_data(nic_info, 1, mem_addr, w_sz,
                                          ff_hwaddr,(void *)wf_xmit_send_complete, nic_info, pkt);
        }
        else
        {
            ret = wf_io_write_data(nic_info, 1, mem_addr, w_sz,
                                          ff_hwaddr, NULL, nic_info, NULL);
			
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
void wf_xmit_ndis_nbl(void *miniport_ctx, void *nbl, ULONG flag)
{
    PADAPTER            pAdapter = (PADAPTER)miniport_ctx;
    PNET_BUFFER_LIST    curr_nbl = NULL, next_nbl = NULL;
    BOOLEAN             DispatchLevel = flag & NDIS_SEND_FLAGS_DISPATCH_LEVEL ? TRUE : FALSE;
	wf_xmit_info_t 		*xmit_info = pAdapter->xmit_info;
	NDIS_STATUS     	failStatus = NDIS_STATUS_SUCCESS;
	PLIST_ENTRY 		plist;
	wf_xmit_pkt_t 			*pkt = NULL;
	nic_info_st *nic_info = pAdapter->nic_info;
	mlme_info_t *mlme_info = nic_info->mlme_info;
    hw_info_st *hw_info = nic_info->hw_info;
	int addbaRet = -1;
	UCHAR pkt_buff[2500] = {0};
	wf_s32 ret, nbl_num;
	ULONG xmit_flag;

	ret = wf_xmit_cannot_send(pAdapter);
	if(ret) {
		failStatus = NDIS_STATUS_RESOURCES;
		//LOG_E("can't send packet, ret=%d\n", ret);
		goto ERROR_DEAL;
	}

	if(nbl == NULL) {
		failStatus = NDIS_STATUS_RESOURCES;
		LOG_E("net buffer list is NULL\n");
		return;
	}

	//nbl_num = 0;
    for(curr_nbl = nbl, next_nbl = curr_nbl->Next; curr_nbl != NULL; curr_nbl = next_nbl) {
		next_nbl = curr_nbl->Next; 

		pkt = NULL;
		
		if(curr_nbl->FirstNetBuffer == NULL) {
			failStatus = NDIS_STATUS_RESOURCES;
			LOG_E("tx buffer is NULL");
			goto ERROR_DEAL;
		}
        
        if(IsListEmpty(&xmit_info->free_head)) {
			failStatus = NDIS_STATUS_RESOURCES;
			LOG_E("we have no tx resource");
			goto ERROR_DEAL;
		}

		plist = ExInterlockedRemoveHeadList(&xmit_info->free_head, &xmit_info->data_free_lock);
		if(plist == NULL) {
			failStatus = NDIS_STATUS_RESOURCES;
			LOG_E("get tx buffer failed!");
			goto ERROR_DEAL;
		}
		//InterlockedDecrement(&xmit_info->data_free_cnt);
		//xmit_info->data_free_cnt--;
		pkt = CONTAINING_RECORD(plist, wf_xmit_pkt_t, list);

		pkt->ptempbuf = pkt->tempbuffer;

		failStatus = wf_xmit_copy_nbl2buffer(curr_nbl, pkt);

		xmit_flag = (NDIS_CURRENT_IRQL()==DISPATCH_LEVEL) ? NDIS_SEND_COMPLETE_FLAGS_DISPATCH_LEVEL : 0;
		curr_nbl->Next = NULL;
		NET_BUFFER_LIST_STATUS(curr_nbl) = failStatus;
		NdisMSendNetBufferListsComplete(pAdapter->MiniportAdapterHandle, curr_nbl, xmit_flag);
		if(failStatus != NDIS_STATUS_SUCCESS) {
			continue;
		}
#if 0
		wf_xmit_fill_attrib(pAdapter, pkt);

		pkt->xframe.buf_addr = pkt->buffer;

		if(pkt->xframe.attrib.priority > 15) {
			failStatus = NDIS_STATUS_RESOURCES;
        	LOG_E("the tx priority error!\n");
			goto ERROR_DEAL;
        }

		if (mlme_info->link_info.num_tx_ok_in_period_with_tid[pkt->xframe.attrib.qsel] > 100 && (hw_info->ba_enable == wf_true)) {
            addbaRet = wf_xmit_add_ba(nic_info, &pkt->xframe);
            if (addbaRet == 0) {
                failStatus = NDIS_STATUS_RESOURCES;
	        	LOG_E("the tx priority error!\n");
				goto ERROR_DEAL;
            }
        }

		ret = wf_tx_msdu_to_mpdu(nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len);
		if (ret != wf_true) {
			failStatus = NDIS_STATUS_RESOURCES;
        	LOG_E("the tx priority error!\n");
			goto ERROR_DEAL;
		}

		ret = wf_xmit_send_frag(nic_info, pkt, wf_false);
		if(ret != NDIS_STATUS_SUCCESS) {
			failStatus = NDIS_STATUS_RESOURCES;
        	LOG_E("xmit packet error!\n");
			goto ERROR_DEAL;
		}
#else
		InterlockedIncrement(&xmit_info->data_pend_cnt);
		InterlockedDecrement(&xmit_info->data_free_cnt);
		ExInterlockedInsertTailList(&xmit_info->pend_head, &pkt->list, &xmit_info->data_pend_lock);
		//xmit_info->data_pend_cnt++;
		//xmit_info->data_free_cnt--;
		KeSetEvent(&xmit_info->tx_evt, 0, FALSE);
#endif
		
		//ExInterlockedInsertTailList(&xmit_info->pend_head, &pkt->list, &xmit_info->txque_lock);
		//WdfWorkItemEnqueue(xmit_info->tx_workitem);
    }

	return;
	
ERROR_DEAL:

	//if curr nbl is not NULL, means set fail status from curr position
	if(curr_nbl == NULL)
		curr_nbl = nbl;
	for( ; curr_nbl != NULL; curr_nbl = curr_nbl->Next) {
    	NET_BUFFER_LIST_STATUS(curr_nbl) = failStatus;
    }

	if(pkt != NULL) {
		//InterlockedIncrement(&xmit_info->data_free_cnt);
		ExInterlockedInsertTailList(&xmit_info->free_head, &pkt->list, &xmit_info->data_free_lock);
	}

    if(nbl != NULL) {
		NdisMSendNetBufferListsComplete(pAdapter->MiniportAdapterHandle, nbl, DispatchLevel);
    }
}

void wf_xmit_data_thread(PADAPTER         padapter)
{
	PLIST_ENTRY plist = NULL;
	NDIS_STATUS ndis_ret = NDIS_STATUS_SUCCESS;
	wf_bool ret = wf_true;
	wf_xmit_pkt_t *pkt = NULL;
	mlme_state_e state;
	nic_info_st *nic_info = padapter->nic_info;
	mlme_info_t *mlme_info = nic_info->mlme_info;
    hw_info_st *hw_info = nic_info->hw_info;
	wf_xmit_info_t *xmit_info = padapter->xmit_info;
	int addbaRet = -1;

	while(1) {
		KeWaitForSingleObject(&xmit_info->tx_evt, Executive, KernelMode, TRUE, NULL);

		if(xmit_info->tx_thread->stop) {
			break;
		}
	
		while(!IsListEmpty(&xmit_info->pend_head)) {

			if(WF_CANNOT_RUN(nic_info)) {
		        break;
		    }
			
			wf_mlme_get_state(nic_info, &state);
	        if (state == MLME_STATE_SCAN){
	            break;
	        }
			
			plist = ExInterlockedRemoveHeadList(&xmit_info->pend_head, &xmit_info->data_pend_lock);
			if(plist == NULL) {
				continue;
			}
			pkt = CONTAINING_RECORD(plist, wf_xmit_pkt_t, list);

			//if the packet send is break in BA request process, we no need exec preproc
			if(!pkt->is_preproc) {
				wf_xmit_pkt_wlan2eth(padapter, pkt);
				wf_xmit_frame_init(padapter->nic_info, &pkt->xframe, pkt->ptempbuf, pkt->data_len);

				pkt->xframe.buf_addr = pkt->buffer;

				if(pkt->xframe.priority > 15) {
		        	LOG_E("the tx priority error!\n");
					ndis_ret = NDIS_STATUS_FAILURE;
					break;
		        }

				pkt->is_preproc = wf_true;
			}			

			if (mlme_info->link_info.num_tx_ok_in_period_with_tid[pkt->xframe.qsel] > 100 && (hw_info->ba_enable == wf_true)) {
	            addbaRet = wf_xmit_add_ba(nic_info, &pkt->xframe);
	            if (addbaRet == 0) {
		        	LOG_I("Send Msg to MLME for starting BA!!");
					ExInterlockedInsertHeadList(&xmit_info->pend_head, &pkt->list, &xmit_info->data_pend_lock);
					xmit_info->data_pend_cnt++;
					break;
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
	        	LOG_E("xmit packet error!\n");
				ndis_ret = NDIS_STATUS_FAILURE;
				break;
			}

			pkt = NULL;
			ndis_ret = NDIS_STATUS_SUCCESS;
		}

	ERROR_DEAL:
		if(ndis_ret != NDIS_STATUS_SUCCESS && pkt != NULL) {
			pkt->ptempbuf = NULL;
			pkt->is_preproc = wf_false;
			InterlockedIncrement(&xmit_info->data_free_cnt);
			InterlockedDecrement(&xmit_info->data_pend_cnt);
			ExInterlockedInsertTailList(&xmit_info->free_head, &pkt->list, &xmit_info->data_free_lock);
		}
	}

	wf_os_api_thread_exit(xmit_info->tx_thread);
}


void wf_xmit_thread_create(PADAPTER pAdapter)
{
	wf_xmit_info_t *xmit_info = pAdapter->xmit_info;

	KeInitializeEvent(&xmit_info->usb_evt, SynchronizationEvent, FALSE);

	xmit_info->usb_thread = wf_os_api_thread_create(NULL, "usb_thread", wf_usb_xmit_thread, pAdapter);
	if (NULL == xmit_info->usb_thread)
    {
        LOG_E("[wf_mlme_init] create mlme thread failed");
        return -1;
    }

	KeInitializeEvent(&xmit_info->tx_evt, SynchronizationEvent, FALSE);

	xmit_info->tx_thread = wf_os_api_thread_create(NULL, "xmit_thread", wf_xmit_data_thread, pAdapter);
	if (NULL == xmit_info->tx_thread)
    {
        LOG_E("[wf_mlme_init] create mlme thread failed");
        return -1;
    }
}


void wf_xmit_init(void *param)
{
	PADAPTER padapter = param;
	wf_xmit_info_t *xmit_info = NULL;
	WDF_OBJECT_ATTRIBUTES attributes;
	wf_u8 i;
	wf_xmit_pkt_t *pkt = NULL;// in 9188 driver, the wf_pkt_t need replaced by xmit_frame
	wf_usb_req_t *usb_req = NULL;

	LOG_D("start init xmit!\n");

	xmit_info = wf_malloc(sizeof(wf_xmit_info_t));
	if(xmit_info == NULL) {
		LOG_E("malloc xmit info failed!\n");
		return;
	}

	padapter->xmit_info = xmit_info;
	xmit_info->padapter = padapter;
	
	KeInitializeSpinLock(&xmit_info->data_pend_lock);
	KeInitializeSpinLock(&xmit_info->data_free_lock);
	InitializeListHead(&xmit_info->pend_head);
	InitializeListHead(&xmit_info->free_head);

	KeInitializeSpinLock(&xmit_info->usb_free_lock);
	KeInitializeSpinLock(&xmit_info->usb_mgmt_lock);
	KeInitializeSpinLock(&xmit_info->usb_data_lock);
	InitializeListHead(&xmit_info->usb_free_head);
	InitializeListHead(&xmit_info->usb_mgmt_head);
	InitializeListHead(&xmit_info->usb_data_head);

	xmit_info->usb_free_cnt = 0;
	xmit_info->usb_mgmt_cnt = 0;
	xmit_info->usb_data_cnt = 0;

	xmit_info->data_pend_cnt = 0;
	xmit_info->data_free_cnt = 0;
#if 1
	for(i=0; i<XMIT_QUEUE_DEPTH; i++) {
		pkt = &xmit_info->packet[i];
		NdisZeroMemory(pkt, sizeof(wf_xmit_pkt_t));
		pkt->xframe.frame_tag = DATA_FRAMETAG;
		pkt->xframe.pkt_offset = (PACKET_OFFSET_SZ / 8);
		InterlockedIncrement(&xmit_info->data_free_cnt);
		ExInterlockedInsertTailList(&xmit_info->free_head, &pkt->list, &xmit_info->data_free_lock);
	}

	for(i=0; i<USB_REQ_QUE_DEPTH; i++) {
		usb_req = &xmit_info->usb_request[i];
		NdisZeroMemory(usb_req, sizeof(wf_usb_req_t));
		InterlockedIncrement(&xmit_info->usb_free_cnt);
		ExInterlockedInsertTailList(&xmit_info->usb_free_head, &usb_req->list, &xmit_info->usb_free_lock);
	}
#endif

	wf_xmit_thread_create(padapter);
}

void wf_xmit_deinit(void *param)
{
	PADAPTER padapter = param;
	wf_xmit_info_t *xmit_info = padapter->xmit_info;
	PLIST_ENTRY plist;
	wf_xmit_pkt_t *pkt;
	wf_usb_req_t *usb_req;

	LOG_D("start deinit xmit!\n");

	if(xmit_info == NULL) {
		return;
	}

	if(xmit_info->tx_thread != NULL) {
		xmit_info->tx_thread->stop = wf_true;
		KeSetEvent(&xmit_info->tx_evt, 0, FALSE);
		wf_os_api_thread_destory(xmit_info->tx_thread);
		xmit_info->tx_thread = NULL;
	}

	if(xmit_info->usb_thread != NULL) {
		xmit_info->usb_thread->stop = wf_true;
		KeSetEvent(&xmit_info->usb_evt, 0, FALSE);
		wf_os_api_thread_destory(xmit_info->usb_thread);
		xmit_info->usb_thread = NULL;
	}

	

	while(!IsListEmpty(&xmit_info->pend_head)) {
		plist = ExInterlockedRemoveHeadList(&xmit_info->pend_head, &xmit_info->data_pend_lock);
		pkt = CONTAINING_RECORD(plist, wf_xmit_pkt_t, list);
	}
	while(!IsListEmpty(&xmit_info->free_head)) {
		plist = ExInterlockedRemoveHeadList(&xmit_info->free_head, &xmit_info->data_free_lock);
		pkt = CONTAINING_RECORD(plist, wf_xmit_pkt_t, list);
	}

	while(!IsListEmpty(&xmit_info->usb_data_head)) {
		plist = ExInterlockedRemoveHeadList(&xmit_info->usb_data_head, &xmit_info->usb_data_lock);
		usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);
	}
	while(!IsListEmpty(&xmit_info->usb_mgmt_head)) {
		plist = ExInterlockedRemoveHeadList(&xmit_info->usb_mgmt_head, &xmit_info->usb_mgmt_lock);
		usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);
	}
	while(!IsListEmpty(&xmit_info->usb_free_head)) {
		plist = ExInterlockedRemoveHeadList(&xmit_info->usb_free_head, &xmit_info->usb_free_lock);
		usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);
	}

	wf_free(xmit_info);
	
}


