/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_usb_main.c

Abstract:
    Bus dependant layer USB specific function
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    01-01-2006    Created

Notes:

--*/

#include "pcomp.h"
#include "wf_debug.h"

UCHAR wf_usb_addr2pipe(UCHAR addr)
{
	UCHAR pipe;

	switch(addr) {
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

#define USB_RECV_CODE
/*++

Routine Description:

    ReadersFailedCallback is called to inform the driver that a
    continuous reader has reported an error while processing a read request.

Arguments:

    Pipe - handle to a framework pipe object.
    Status - NTSTATUS value that the pipe's I/O target returned.
    UsbdStatus - USBD_STATUS-typed status value that the pipe's I/O target returned.

Return Value:

    If TRUE, causes the framework to reset the USB pipe and then
        restart the continuous reader.
    If FASLE, the framework does not reset the device or restart
        the continuous reader.

    If this event is not registered, framework default action is to reset
    the pipe and restart the reader.

--*/
BOOLEAN wf_usb_recv_failed(WDFUSBPIPE Pipe, NTSTATUS Status, USBD_STATUS UsbdStatus)
{    
    UNREFERENCED_PARAMETER(Status);
    LOG_D("NTSTATUS 0x%x, UsbdStatus 0x%x", Status, UsbdStatus);
    
    if (UsbdStatus == USBD_STATUS_STALL_PID) {
        // Send a clear stall URB. Pipereset will clear the stall. 
        WdfUsbTargetPipeResetSynchronously(Pipe,
                                           NULL,
                                           NULL
                                           );
    
    }
    return TRUE;
}


VOID
wf_usb_recv_complete(
    WDFUSBPIPE Pipe,
    WDFMEMORY BufferHdl,
    size_t NumBytesRead,
    WDFCONTEXT Context
    )
{
	PADAPTER pAdpater;
    PUSB_DEVICE_CONTEXT     usb_dev_ctx;
	nic_info_st *nic_info;

    UNREFERENCED_PARAMETER(Pipe);
	pAdpater = (PADAPTER)Context;

	if(pAdpater == NULL) {
		return;
	}

	if(pAdpater->nic_info == NULL) {
		return;
	}

	nic_info = pAdpater->nic_info;

	if(WF_CANNOT_RUN(nic_info)) {
		LOG_E("device has stopped, finish read complete callback");
		return;
	}
	
    usb_dev_ctx = GetUsbDeviceContext(pAdpater->UsbDevice);

	wf_recv_complete_callback(pAdpater, BufferHdl, (ULONG)NumBytesRead, usb_dev_ctx->ReaderMdlSize);

	return;
}

NDIS_STATUS wf_usb_recv_reader_init(PADAPTER adapter, WDFUSBPIPE ReadPipe)
{
	NDIS_STATUS ndisStatus;
	NTSTATUS ntStatus;
	WDF_USB_CONTINUOUS_READER_CONFIG  config;
	CHAR		dummyMdlBuffer[1];
	PUSB_DEVICE_CONTEXT 	pUsbDeviceContext;

	pUsbDeviceContext = GetUsbDeviceContext(adapter->UsbDevice);
	//
	// Tell the framework that it's okay to read less than
	// MaximumPacketSize
	//
	WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(ReadPipe);
	WDF_USB_CONTINUOUS_READER_CONFIG_INIT(&config,
										  wf_usb_recv_complete,
										  adapter,	  // Context
										  MAX_RECEIVE_BUFFER_SIZE );  
	//
	// Reader requests are not posted to the target automatically.
	// Driver must explictly call WdfIoTargetStart to kick start the
	// reader.	In this sample, it's done in D0Entry.
	// By defaut, framework queues two requests to the target
	// endpoint. Driver can configure up to 10 requests with CONFIG macro.
	// I have tried from 4 to 10 readers and it hasn't impacted performance that much
	// TODO : Try adjusting the number of readers to see the optimal value
	//
	config.NumPendingReads = 0; 
	config.EvtUsbTargetPipeReadersFailed = wf_usb_recv_failed;
	//
	// The header is used to allocate an MDL which	maps the Virtual address
	// of the buffer which has the data read from device. By allocating the memory 
	// at initialization it doesn't have to be done at runtime. 
	//

	config.HeaderLength = MmSizeOfMdl(dummyMdlBuffer, MAX_RECEIVE_BUFFER_SIZE);
	pUsbDeviceContext->ReaderMdlSize = (ULONG)config.HeaderLength;
	ntStatus = WdfUsbTargetPipeConfigContinuousReader( ReadPipe,
													   &config
														);
	if (!NT_SUCCESS(ntStatus)) {
	   LOG_E("WdfUsbTargetPipeConfigContinuousReader failed with error %x", ntStatus);
	}
	
	NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
	return ntStatus;
}

#define USB_XMIT_CODE


void wf_usb_mgmt_fram_monitor(void *adapter, char *buff, wf_u32 buff_len)
{
	PADAPTER padapter = adapter;
	wf_ap_info_t *ap_info = padapter->ap_info;
	wf_u8 offset = 0;
	wf_u8 sub_type;
	wf_80211_mgmt_t *pmgmt;
	wf_u32 frame_type;
	KIRQL irq = 0;

	if(ap_info == NULL) {
		return;
	}

	if(!ap_info->valid) {
		//LOG_E("ap info is invalid!\n");
		return;
	}

	if(ap_info->assoc_req_len != 0) {
		return;
	}

	pmgmt = (wf_80211_mgmt_t *)(buff + TXDESC_SIZE);
	sub_type = wf_80211_get_frame_type(pmgmt->frame_control);
	frame_type = GET_HDR_Type(buff+TXDESC_SIZE);
	if(frame_type == MAC_FRAME_TYPE_MGT && sub_type == WF_80211_FRM_ASSOC_REQ) {
		KeAcquireSpinLock(&ap_info->lock, &irq);
		if(wf_memcmp(ap_info->scan_info.bssid, pmgmt->bssid, sizeof(wf_80211_bssid_t)) == 0) {
			wf_memcpy(ap_info->assoc_req, buff + TXDESC_SIZE + offset, 
				buff_len - offset - TXDESC_SIZE);
			ap_info->assoc_req_len = buff_len - offset - TXDESC_SIZE;
			//print_buffer("assoc_req", 0, buff, buff_len);
		}
		KeReleaseSpinLock(&ap_info->lock, irq);
	}
}


void wf_usb_xmit_complet_callback(void *pnic_info, void *frame)
{
	nic_info_st *nic_info = pnic_info;
	PADAPTER padapter = nic_info->hif_node;
	wf_usb_info_t *usb_info = padapter->usb_info;
	wf_usb_req_t *usb_req = frame;
	wf_data_que_t *ppend, *pfree;

	//nic send data notify
	if(usb_req->tx_callback_func) {
		usb_req->tx_callback_func(usb_req->tx_info, usb_req->param);
	}

	if(usb_req->addr == 3) {
		ppend = &usb_info->mgmt_pend;
		pfree = &usb_info->mgmt_free;
	} else {
		ppend = &usb_info->data_pend;
		pfree = &usb_info->data_free;
	}

	wf_pkt_data_enque(pfree, &usb_req->list, QUE_POS_HEAD);
	InterlockedDecrement(&usb_info->proc_cnt);
}


void
wf_usb_xmit_complete(
    __in WDFREQUEST  Request,
    __in WDFIOTARGET  Target,
    __in PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in WDFCONTEXT  Context
    )
/*++

Routine Description

    Request - Handle to the WDFREQUEST which was used to send data to the USB target.
    Target - Handle to the Iotarget to which teh Request was sent. Conceptually this
              is the BULK USB __out pipe(on of  Data or beacon) 
    CompletionParams - In case of USB this contains the USB status and amount of bytes transferred 
                       
    Context - This is the COntext we set in WdfRequestSend 


Arguments:

    
Return Value:

    
--*/

{
    PUSB_WRITE_REQ_CONTEXT writeContext;
    PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;
    NTSTATUS                    status;
	WDF_REQUEST_REUSE_PARAMS    params;

	UNREFERENCED_PARAMETER(Target);
    writeContext = (PUSB_WRITE_REQ_CONTEXT)Context;

    status = CompletionParams->IoStatus.Status;

    // For usb devices, we should look at the Usb.Completion param.
    usbCompletionParams = CompletionParams->Parameters.Usb.Completion;

    if (!NT_SUCCESS(status)) {
		LOG_E("Write failed: request 0x%x Status 0x%x UsbdStatus 0x%x",
        	Request, status, usbCompletionParams->UsbdStatus);
    }

	WDF_REQUEST_REUSE_PARAMS_INIT(&params, WDF_REQUEST_REUSE_NO_FLAGS, STATUS_SUCCESS);
    WdfRequestReuse(Request, &params);

	wf_usb_xmit_complet_callback(writeContext->nic_info, writeContext->frame);
}





NTSTATUS wf_usb_xmit_pkt_sync(PADAPTER         adapter, wf_usb_req_t *req)
{
	WDFREQUEST      writeRequest;
	WDFUSBPIPE      wdfUsbWritePipe;
	NTSTATUS        ntStatus = NDIS_STATUS_SUCCESS;
	PUSB_WRITE_REQ_CONTEXT writeContext;
	wf_usb_info_t *usb_info = adapter->usb_info;
	WDF_REQUEST_REUSE_PARAMS    params;

	writeRequest = req->request;
	if (writeRequest == NULL) {   // No requests left
		LOG_E("HwUsbSendPacketSync no req. left!!");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	writeContext = GetWriteRequestContext(writeRequest);;
	//writeContext->QueueType = QueueType;
	wdfUsbWritePipe = writeContext->UsbPipe;
	UsbBuildInterruptOrBulkTransferRequest(
		writeContext->Urb,
		sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
		writeContext->UsbdPipeHandle,
		req->data_buf,
		NULL,
		req->data_len,
		//USBD_TRANSFER_DIRECTION_OUT,
		USBD_TRANSFER_DIRECTION_OUT | USBD_SHORT_TRANSFER_OK,
		NULL);
	ntStatus = WdfUsbTargetPipeFormatRequestForUrb(wdfUsbWritePipe,
		writeRequest,
		writeContext->UrbMemory,
		NULL);

	if (!NT_SUCCESS(ntStatus)) {
		LOG_E("HwUsbSendPacketSync Failed to format requset for urb");
		ntStatus = STATUS_INSUFFICIENT_RESOURCES;
	}

	if(WdfRequestSend(writeRequest, writeContext->IoTarget, &usb_info->sync_req_option) == FALSE) {   // Failure - Return request
		ntStatus = WdfRequestGetStatus(writeRequest);
		LOG_E("sync send failed! error=%x", ntStatus);

		WDF_REQUEST_REUSE_PARAMS_INIT(&params, WDF_REQUEST_REUSE_NO_FLAGS, STATUS_SUCCESS);
        WdfRequestReuse(writeRequest, &params);
	}
	//LOG_D("HwUsbSendPacketSync finish---status[0x%08x]",ntStatus);
	return ntStatus;
}


NTSTATUS
wf_usb_xmit_pkt_async (
	PADAPTER    adapter,
    UCHAR           QueueType,
    ULONG           BufferLength,   // Number of bytes in pData buffer
    PVOID           Buffer,
    void			*frame
    )
{
    WDFREQUEST      writeRequest;
    WDFUSBPIPE      wdfUsbWritePipe;
    NTSTATUS        ntStatus = NDIS_STATUS_SUCCESS;
    PUSB_WRITE_REQ_CONTEXT writeContext;  
	wf_usb_req_t *req = frame;
	wf_usb_info_t *usb_info = adapter->usb_info;
	WDF_REQUEST_REUSE_PARAMS    params;

    writeRequest = req->request;
    if (writeRequest == NULL) {   // No requests left
        LOG_E("HwUsbSendPacketAsync no req. left!!");
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
	writeContext = GetWriteRequestContext(writeRequest);
    writeContext->QueueType =  QueueType;
	writeContext->frame = frame;
	writeContext->nic_info = adapter->nic_info;
    wdfUsbWritePipe = writeContext->UsbPipe;
    UsbBuildInterruptOrBulkTransferRequest(
                        writeContext->Urb,
                        sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                        writeContext->UsbdPipeHandle,
                        Buffer,
                        NULL,
                        BufferLength,
                        //USBD_TRANSFER_DIRECTION_OUT,
                        USBD_TRANSFER_DIRECTION_OUT | USBD_SHORT_TRANSFER_OK,
                        NULL);
    ntStatus = WdfUsbTargetPipeFormatRequestForUrb(wdfUsbWritePipe,
                                                   writeRequest,
                                                   writeContext->UrbMemory,
                                                   NULL  );
    
    if (!NT_SUCCESS(ntStatus)) {
        LOG_E("HwUsbSendPacketAsync Failed to format requset for urb");
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto error;
    }

    WdfRequestSetCompletionRoutine(writeRequest, wf_usb_xmit_complete, writeContext);    

    if (WdfRequestSend(writeRequest, writeContext->IoTarget, &usb_info->async_req_option ) == FALSE) {   // Failure - Return request
		ntStatus = WdfRequestGetStatus(writeRequest);
		LOG_E("HwUsbSendPacketAsync send failed, ntstatus=%x", ntStatus);
        goto error;
    }
    return ntStatus;

error:
	
	WDF_REQUEST_REUSE_PARAMS_INIT(&params, WDF_REQUEST_REUSE_NO_FLAGS, STATUS_SUCCESS);
    WdfRequestReuse(writeRequest, &params);
    return ntStatus;
}


int wf_usb_xmit_empty(void *adapter)
{
	PADAPTER padapter = adapter;
	wf_usb_info_t *usb_info = padapter->usb_info;

	if(IsListEmpty(&usb_info->data_pend.head) &&
		IsListEmpty(&usb_info->mgmt_pend.head)) {
		LOG_E("the tx resource is full");
		return 1;
	}

    return 0;
}

int wf_usb_xmit_insert(void *adapter,wf_u8 agg_num,char *buff, wf_u32 buff_len, wf_u32 addr, int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param)
{
	PADAPTER padapter = adapter;
	wf_usb_info_t *usb_info = padapter->usb_info;
	nic_info_st *nic_info = padapter->nic_info;
	PLIST_ENTRY 		plist;
	wf_usb_req_t 		*usb_req;
	UCHAR pipe;
	wf_data_que_t *ppend, *pfree;

	UNREFERENCED_PARAMETER(agg_num);

	if(usb_info == NULL || nic_info == NULL) {
		return -1;
	}

	if(WF_CANNOT_RUN(nic_info)) {
		return -1;
	}

	if(padapter->dev_state != WF_DEV_STATE_RUN) {
		return -1;
	}

	pipe = wf_usb_addr2pipe((UCHAR)addr);

	if(pipe == 3) {
		ppend = &usb_info->mgmt_pend;
		pfree = &usb_info->mgmt_free;
	} else {
		ppend = &usb_info->data_pend;
		pfree = &usb_info->data_free;
	}

	if(IsListEmpty(&pfree->head)) {
		LOG_E("the tx resource is empty");
		return -1;
	}

	//need use usb trans layer list
	plist = wf_pkt_data_deque(pfree, QUE_POS_HEAD);
	if(plist == NULL) {
		LOG_E("get tx resource failed!\n");
		return -1;
	}
	usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);
	InterlockedIncrement(&usb_info->proc_cnt);

	usb_req->data_buf 			= (UCHAR *)buff;
	usb_req->data_len 			= buff_len;
	usb_req->addr 				= pipe;
	usb_req->tx_callback_func 	= tx_callback_func;
	usb_req->tx_info 			= tx_info;
	usb_req->param 				= param;

	wf_usb_mgmt_fram_monitor(padapter, buff, buff_len);

	wf_pkt_data_enque(ppend, &usb_req->list, QUE_POS_TAIL);
	InterlockedDecrement(&usb_info->proc_cnt);	
	KeSetEvent(&usb_info->usb_evt, 0, FALSE);

    return 0;
}

void wf_usb_xmit_thread(PADAPTER         padapter)
{
	wf_usb_info_t *usb_info = NULL;
	PLIST_ENTRY plist = NULL;
	wf_usb_req_t *usb_req = NULL;
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	nic_info_st *nic_info = padapter->nic_info;
	wf_data_que_t *pdata_pend, *pmgmt_pend, *pdata_free, *pmgmt_free, *ppend, *pfree;
	
	usb_info = padapter->usb_info;
	pdata_pend = &usb_info->data_pend;
	pmgmt_pend = &usb_info->mgmt_pend;
	pdata_free = &usb_info->data_free;
	pmgmt_free = &usb_info->mgmt_free;
	
	while(1) {
		KeWaitForSingleObject(&usb_info->usb_evt, Executive, KernelMode, TRUE, NULL);

		if(usb_info->usb_thread->stop) {
			break;
		}
		
		while(!IsListEmpty(&pmgmt_pend->head) || !IsListEmpty(&pdata_pend->head)) {
			if(WF_CANNOT_RUN(nic_info)) {
		        break;
		    }

			if(!IsListEmpty(&pmgmt_pend->head)) {
				plist = wf_pkt_data_deque(pmgmt_pend, QUE_POS_HEAD);
			} else if(!IsListEmpty(&pdata_pend->head)) {
				plist = wf_pkt_data_deque(pdata_pend, QUE_POS_HEAD);
			} else {
				break;
			}
			if(plist == NULL) {
				LOG_E("get usb req list failed");
				break;
			}
			
			usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);
			InterlockedIncrement(&usb_info->proc_cnt);
			
			if(usb_req->data_buf == NULL) {
				LOG_E("data buffer is NULL");
				wf_usb_xmit_complet_callback(padapter->nic_info, usb_req);
				continue;
			}
			//LOG_D("data_addr=%x len=%d", usb_req->data_buf, usb_req->data_len);
			ret = wf_usb_xmit_pkt_async (padapter, usb_req->addr, usb_req->data_len, usb_req->data_buf, usb_req);
			if(ret != NDIS_STATUS_SUCCESS) {
				wf_usb_xmit_complet_callback(padapter->nic_info, usb_req);
			}
		}
	}

	wf_os_api_thread_exit(usb_info->usb_thread);
}

#define USB_INIT_CODE
/*++

Routine Description

     Do this for low/meduim q and for high q
    Create write requests,
    Allocate         URB Memory
    Allocate         WDFMEMORY objects
    Setup the assocition between them in the Request context.

Arguments:
    Nic - Pointer to the HW NIC structure. 
    MaxOutstanding -- Number of Control requests to allocate.
    
Return Value:

    
--*/
NTSTATUS wf_usb_ctrl_req_init(PADAPTER adapter, BYTE MaxOutstanding)
{
    WDF_OBJECT_ATTRIBUTES   requestAttributes;
    UCHAR                   requestIndex ;
    NTSTATUS                ntStatus;
    WDF_OBJECT_ATTRIBUTES   objectAttribs;
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_CONTROL_RESOURCES  controlResources;
    WDFIOTARGET             ioTarget;

    ntStatus = STATUS_SUCCESS;
    pUsbDeviceContext = GetUsbDeviceContext(adapter->UsbDevice);

    ioTarget = WdfUsbTargetDeviceGetIoTarget(adapter->UsbDevice);
    controlResources = &pUsbDeviceContext->ControlResources;

    WDF_OBJECT_ATTRIBUTES_INIT(&objectAttribs);  
    objectAttribs.ParentObject = adapter->WdfDevice;//Mp11GetWdfDevice(adapter);
    ntStatus = WdfSpinLockCreate(&objectAttribs, &controlResources->RequestArrayLock);

    if (!NT_SUCCESS(ntStatus)) {
        LOG_E("Could not create request lock: status(0x%08X)", ntStatus);
        return ntStatus;
    }

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&requestAttributes, USB_CONTROL_REQ_CONTEXT);
    requestAttributes.ParentObject = adapter->WdfDevice;//Mp11GetWdfDevice(adapter);

    for (requestIndex = 0; requestIndex < MaxOutstanding; requestIndex++)
    {
        WDFREQUEST* pNextRequest;
        PUSB_CONTROL_REQ_CONTEXT vendorRequestContext;
        WDF_USB_CONTROL_SETUP_PACKET  setupPacket;

        pNextRequest = &controlResources->RequestArray[requestIndex];

        ntStatus = WdfRequestCreate(&requestAttributes, ioTarget, pNextRequest);
        
        if (!NT_SUCCESS(ntStatus)) {
            LOG_E("Could not create request: status(0x%08X)", ntStatus);
            controlResources->MaxOutstandingRequests = requestIndex;
            controlResources->NextAvailableRequestIndex = requestIndex;
            return ntStatus;
        }
        //
        // By doing this we allocate resources like the IRP upfront eventhough 
        // we may not have all the data yer. 
        //
        ntStatus = WdfUsbTargetDeviceFormatRequestForControlTransfer(
																	 adapter->UsbDevice,
                                                                     *pNextRequest,
                                                                     &setupPacket,
                                                                     NULL,
                                                                     NULL);
        
        if(!NT_SUCCESS(ntStatus)) {
            LOG_E("WdfUsbTargetDeviceFormatRequestForControlTransfer: Failed - 0x%x", ntStatus);
            return ntStatus;
        }

        //
        // set REQUEST_CONTEXT  parameters.
        //
        vendorRequestContext = GetControlRequestContext(*pNextRequest);
        vendorRequestContext->NdisContext = adapter;
        //vendorRequestContext->Nic = Nic;

    }

    controlResources->MaxOutstandingRequests = MaxOutstanding;
    controlResources->NextAvailableRequestIndex = MaxOutstanding;

    return ntStatus;
}


void wf_usb_ctrl_req_deinit(PADAPTER adapter)
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_CONTROL_RESOURCES    controlResources;
    USHORT requestIndex;

    pUsbDeviceContext = GetUsbDeviceContext(adapter->UsbDevice);
    controlResources = &pUsbDeviceContext->ControlResources;

    //
    // Don't need a lock here because nobody can be making control request by the time this routine 
    // is called
    //
    for (requestIndex = 0; requestIndex < controlResources->MaxOutstandingRequests; requestIndex++) {
        if (controlResources->RequestArray[requestIndex] != NULL) {
            WdfObjectDelete(controlResources->RequestArray[requestIndex]);
            controlResources->RequestArray[requestIndex] = NULL;
        }
    }
    
    if (controlResources->RequestArrayLock != NULL) {
        WdfObjectDelete(controlResources->RequestArrayLock);
        controlResources->RequestArrayLock = NULL;
    }

}


void wf_usb_bulk_async_option_init(wf_usb_info_t *usb_info)
{
	WDF_REQUEST_SEND_OPTIONS_INIT(
                              &usb_info->async_req_option,
                              0
                              );
	
    // Timeout of less than 20 secs is less desirable.
    WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(
                             &usb_info->async_req_option,
                             WDF_REL_TIMEOUT_IN_SEC(4)
                             );
}


void wf_usb_bulk_sync_option_init(wf_usb_info_t *usb_info)
{
	WDF_REQUEST_SEND_OPTIONS_INIT(
                              &usb_info->sync_req_option,
                              WDF_REQUEST_SEND_OPTION_SYNCHRONOUS
                              );
	
    // Timeout of less than 20 secs is less desirable.
    WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(
                             &usb_info->sync_req_option,
                             WDF_REL_TIMEOUT_IN_SEC(4)
                             );
}


void wf_usb_ctrl_req_option_init(wf_usb_info_t *usb_info)
{
	WDF_REQUEST_SEND_OPTIONS_INIT(
                              &usb_info->ctrl_req_option,
                              WDF_REQUEST_SEND_OPTION_SYNCHRONOUS
                              );
	
    // Timeout of less than 20 secs is less desirable.
    WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(
                             &usb_info->ctrl_req_option,
                             WDF_REL_TIMEOUT_IN_SEC(4)
                             );
}



BOOLEAN wf_usb_bulk_req_init(PADAPTER        adapter, WDFUSBPIPE UsbPipe, wf_usb_req_t *usb_req)
{
    WDFIOTARGET             ioTarget;
    WDF_OBJECT_ATTRIBUTES   req_attr;
    NTSTATUS                ntStatus;
    WDFMEMORY               urbMemory;
    WDF_OBJECT_ATTRIBUTES   obj_attr;
    struct _URB_BULK_OR_INTERRUPT_TRANSFER *urbBuffer;
    CHAR                    dummyBuffer[1];  //Need a buffer to pass to UsbBuildInterruptOrBulkTransferRequest  
	WDFREQUEST 				*request; 
    USBD_PIPE_HANDLE        usbdPipeHandle;
    PUSB_WRITE_REQ_CONTEXT 	writeContext;

    ioTarget = WdfUsbTargetPipeGetIoTarget(UsbPipe);
    WDF_OBJECT_ATTRIBUTES_INIT(&obj_attr);
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&req_attr, USB_WRITE_REQ_CONTEXT);

	request = &usb_req->request;
	obj_attr.ParentObject = *request;
	
    ntStatus = WdfRequestCreate(&req_attr, ioTarget, request);
    if (!NT_SUCCESS(ntStatus)) {
        LOG_E("Could not create request: status(0x%08X)", ntStatus);
        return FALSE;
    }
    
    // Preallocate the request timer to prevent the request from failing while trying to send it.
    ntStatus = WdfRequestAllocateTimer(*request);
    if (!NT_SUCCESS(ntStatus)){
        LOG_E("Could not allocate timer for  request status(0x%08X)", ntStatus);
        return FALSE;
    }

    ntStatus = WdfMemoryCreate(&obj_attr, NonPagedPool, 0, 
    	sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER), &urbMemory, &urbBuffer);
    if (!NT_SUCCESS(ntStatus)) {
        LOG_E("Could not create request lock: status(0x%08X)", ntStatus);
        return FALSE;
    }
           
    usbdPipeHandle = WdfUsbTargetPipeWdmGetPipeHandle(UsbPipe);
    //
    // NOTE : call    UsbBuildInterruptOrBulkTransferRequest otherwise 
    //    WdfUsbTargetPipeFormatRequestForUrb    will assert 
    //  with *** Assertion failed: Urb->UrbHeader.Length >= sizeof(_URB_HEADER)
    //
    UsbBuildInterruptOrBulkTransferRequest(
                        (PURB )urbBuffer,
                        sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                        usbdPipeHandle,
                        dummyBuffer,
                        NULL,
                        sizeof(dummyBuffer),
                        USBD_TRANSFER_DIRECTION_OUT,
                        NULL);

    // By calling  WdfUsbTargetPipeFormatRequestForUrb the frameworks allocate a lot of resources
    // like the underlying IRP for the request and hence it is better to do it at initilization 
    // to prevent an avoidable  failure later. 
    ntStatus = WdfUsbTargetPipeFormatRequestForUrb(UsbPipe, *request, urbMemory, NULL);
    if(!NT_SUCCESS(ntStatus)) {
        LOG_E("WdfUsbTargetPipeFormatRequestForUrb: Failed - 0x%x", ntStatus);
        return FALSE;
    }

    // set REQUEST_CONTEXT  parameters.
    writeContext = GetWriteRequestContext(*request);
    writeContext->UrbMemory       = urbMemory;
    writeContext->Urb = (PURB )urbBuffer;
    writeContext->UsbdPipeHandle       = usbdPipeHandle;
    writeContext->UsbPipe               = UsbPipe;
    writeContext->NdisContext           = adapter;
    //writeContext->Nic                   = Nic;
    writeContext->IoTarget              = ioTarget;

    return TRUE;
}

void wf_usb_bulk_req_deinit(wf_usb_req_t *usb_req)
{
	if(usb_req->request != NULL) {
		WdfObjectDelete(usb_req->request);
	}
}


NDIS_STATUS wf_usb_xmit_init(void *param)
{
	PADAPTER padapter = param;
	wf_usb_info_t *usb_info = NULL;
	wf_u8 i;
	wf_usb_req_t *usb_req = NULL;
	wf_data_que_t *pdata_free, *pmgmt_free;
	PUSB_DEVICE_CONTEXT     usb_dev_ctx;

	LOG_D("start init xmit!\n");

	usb_info = wf_malloc(sizeof(wf_usb_info_t));
	if(usb_info == NULL) {
		LOG_E("malloc xmit info failed!\n");
		return NDIS_STATUS_FAILURE;
	}
	NdisZeroMemory(usb_info, sizeof(wf_usb_info_t));

	usb_dev_ctx = GetUsbDeviceContext(padapter->UsbDevice);
	padapter->usb_info = usb_info;
	usb_info->padapter = padapter;
	//ctrl transport init
	wf_usb_ctrl_req_option_init(usb_info);
	wf_usb_ctrl_req_init(padapter, USB_CTRL_REQ_DEPTH);

	//bulk transport init
	
	wf_usb_bulk_sync_option_init(usb_info);
	//for sync bulk transport
	wf_usb_bulk_req_init(padapter, usb_dev_ctx->OutputPipeHigh, &usb_info->sync_request);

	//for async bulk transport
	wf_data_que_init(&usb_info->data_pend);
	wf_data_que_init(&usb_info->data_free);
	wf_data_que_init(&usb_info->mgmt_pend);
	wf_data_que_init(&usb_info->mgmt_free);

	pdata_free = &usb_info->data_free;
	pmgmt_free = &usb_info->mgmt_free;

	wf_usb_bulk_async_option_init(usb_info);
	for(i=0; i<USB_DATA_REQ_DEPTH; i++) {
		usb_req = &usb_info->async_request[i];
		NdisZeroMemory(usb_req, sizeof(wf_usb_req_t));
		wf_usb_bulk_req_init(padapter, usb_dev_ctx->OutputPipeNormal, usb_req);
		wf_pkt_data_enque(pdata_free, &usb_req->list, QUE_POS_HEAD);
	}

	for(i=USB_DATA_REQ_DEPTH; i<USB_REQ_QUE_DEPTH; i++) {
		usb_req = &usb_info->async_request[i];
		NdisZeroMemory(usb_req, sizeof(wf_usb_req_t));
		wf_usb_bulk_req_init(padapter, usb_dev_ctx->OutputPipeHigh, usb_req);
		wf_pkt_data_enque(pmgmt_free, &usb_req->list, QUE_POS_HEAD);
	}

	//usb xmit thread
	KeInitializeEvent(&usb_info->usb_evt, SynchronizationEvent, FALSE);
	usb_info->usb_thread = wf_os_api_thread_create(NULL, "usb_thread", wf_usb_xmit_thread, padapter);
	if (NULL == usb_info->usb_thread) {
        LOG_E("[wf_mlme_init] create mlme thread failed");
        return NDIS_STATUS_FAILURE;
    }

	return NDIS_STATUS_SUCCESS;
}


void wf_usb_xmit_deinit(void *param)
{
	PADAPTER padapter = param;
	wf_usb_info_t *usb_info = padapter->usb_info;
	PLIST_ENTRY plist;
	//wf_xmit_pkt_t *pkt;
	wf_usb_req_t *usb_req;
	wf_data_que_t *pque[4];
	int i;

	LOG_D("start deinit xmit!\n");

	if(usb_info == NULL) {
		return;
	}

	pque[0] = &usb_info->data_free;
	pque[1] = &usb_info->data_pend;
	pque[2] = &usb_info->mgmt_free;
	pque[3] = &usb_info->mgmt_pend;

	if(usb_info->usb_thread != NULL) {
		usb_info->usb_thread->stop = wf_true;
		KeSetEvent(&usb_info->usb_evt, 0, FALSE);
		wf_os_api_thread_destory(usb_info->usb_thread);
		usb_info->usb_thread = NULL;
	}

	for(i=0; i<4; i++) {
		while(!IsListEmpty(&pque[i]->head)) {
			plist = wf_pkt_data_deque(pque[i], QUE_POS_HEAD);
			usb_req = CONTAINING_RECORD(plist, wf_usb_req_t, list);
			//need do deinit usb req
			wf_usb_bulk_req_deinit(usb_req);
		}
	}

	wf_usb_bulk_req_deinit(&usb_info->sync_request);

	wf_usb_ctrl_req_deinit(padapter);

	wf_free(usb_info);
	
}

NDIS_STATUS wf_usb_recv_init(PADAPTER adapter)
{
	PUSB_DEVICE_CONTEXT 	usb_dev_ctx;
	wf_usb_info_t *usb_info = adapter->usb_info;
	NDIS_STATUS ndisStatus;

	KeInitializeEvent(&usb_info->cmd_finish, SynchronizationEvent, FALSE);
	KeInitializeEvent(&usb_info->fw_finish, SynchronizationEvent, FALSE);
	KeInitializeEvent(&usb_info->reg_finish, SynchronizationEvent, FALSE);

	usb_dev_ctx = GetUsbDeviceContext(adapter->UsbDevice);

	ndisStatus = wf_usb_recv_reader_init(adapter, usb_dev_ctx->InputPipe);
	
	return ndisStatus;
}

void wf_usb_recv_deinit(PADAPTER adapter)
{
	UNREFERENCED_PARAMETER(adapter);
}



/*++

Routine Description:

    This helper routine selects the configuration, interface and
    creates a context for every pipe (end point) in that interface.

Arguments:

    Nic - Pointer to the HW NIC structure. 

Return Value:

    NT status value

--*/
NTSTATUS wf_usb_dev_create(PADAPTER adapter)
{
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS configParams;
    NTSTATUS                            ntStatus;
    WDFUSBPIPE                          pipe;
    WDF_USB_PIPE_INFORMATION            pipeInfo;
    UCHAR                               index;
    UCHAR                               numberConfiguredPipes;
    PUSB_DEVICE_CONTEXT                 usbDeviceContext;
    WDF_OBJECT_ATTRIBUTES               attributes;
    //WDFDEVICE                           WdfDevice;

    //
    // Create a USB device handle so that we can communicate with the
    // underlying USB stack. The WDFUSBDEVICE handle is used to query,
    // configure, and manage all aspects of the USB device.
    // These aspects include device properties, bus properties,
    // and I/O creation and synchronization.
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, USB_DEVICE_CONTEXT);
    //WdfDevice = Mp11GetWdfDevice(adapter);
    ntStatus = WdfUsbTargetDeviceCreate(adapter->WdfDevice,
                                &attributes,
                                &adapter->UsbDevice);
    
    if (!NT_SUCCESS(ntStatus)) {
        LOG_E("WdfUsbTargetDeviceCreate failed with Status code %!STATUS!", ntStatus);
        return ntStatus;
    }

    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_SINGLE_INTERFACE( &configParams);
    ntStatus = WdfUsbTargetDeviceSelectConfig(adapter->UsbDevice,
                                        WDF_NO_OBJECT_ATTRIBUTES,
                                        &configParams);
    
    if(!NT_SUCCESS(ntStatus)) {
        LOG_E("WdfUsbTargetDeviceSelectConfig failed %!STATUS!", ntStatus);
        return ntStatus;
    }
    
    usbDeviceContext = GetUsbDeviceContext(adapter->UsbDevice);
    usbDeviceContext->UsbInterface =
                configParams.Types.SingleInterface.ConfiguredUsbInterface;
    numberConfiguredPipes = configParams.Types.SingleInterface.NumberConfiguredPipes;

    if (numberConfiguredPipes > MAX_PIPES) {
        numberConfiguredPipes = MAX_PIPES;
    }
    
    usbDeviceContext->NumberOfPipes = numberConfiguredPipes;
    //
    // If we didn't find the 3 pipes, fail the start.
    //

    if(numberConfiguredPipes < 3) {
        ntStatus = STATUS_INVALID_DEVICE_STATE;
        LOG_E("Device is not configured properly %!STATUS!", ntStatus);
        return ntStatus;

    }

    //
    // Get pipe handles
    //
    for(index=0; index < numberConfiguredPipes; index++) {

        WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);

        pipe = WdfUsbInterfaceGetConfiguredPipe(usbDeviceContext->UsbInterface,
                                                index, //PipeIndex,
                                                &pipeInfo
                                                );
        //
        // Tell the framework that it's okay to read less than
        // MaximumPacketSize
        //
        WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(pipe);

        if(WdfUsbPipeTypeBulk == pipeInfo.PipeType ) {
            LOG_D("BulkInput Pipe is 0x%p direction : %s", pipe,
            	WdfUsbTargetPipeIsInEndpoint(pipe) ? "__in" : "__out");
            
            if (WdfUsbTargetPipeIsInEndpoint(pipe)) {
                usbDeviceContext->InputPipe = pipe;
            } 
            else {
                //
                // The out pipe normal is the first out end point so if we haven't found any then 
                //  it is the first one. 
                // 
                if (usbDeviceContext->OutputPipeNormal == NULL) {
                   usbDeviceContext->OutputPipeNormal = pipe;
                }
                else {
                    usbDeviceContext->OutputPipeHigh = pipe;
                }
            }

        }
        else{
           LOG_D(" Pipe is 0x%p type %x", pipe, pipeInfo.PipeType);
        }

    }
   
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = adapter->WdfDevice;//Mp11GetWdfDevice( adapter );

    ntStatus = WdfWaitLockCreate(&attributes, &usbDeviceContext->PipeStateLock);
    if (!NT_SUCCESS(ntStatus)) {
        LOG_E("Couldn't create PipeStateLock %!STATUS!", ntStatus);
        return ntStatus;

    }
    //ntStatus = AllocateControlRequests(adapter,  MAX_CONTROL_REQUESTS );
    return ntStatus;
}


NDIS_STATUS wf_usb_dev_start(PADAPTER padapter)
{
	PUSB_DEVICE_CONTEXT     usb_dev_ctx;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    NDIS_STATUS ndisStatus;
	
	// Since the pipe_lock is a WDFWAITLOCK, DISPATICH_LEVEL is not needed.        
    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
	
    usb_dev_ctx = GetUsbDeviceContext(padapter->UsbDevice);
    WdfWaitLockAcquire(usb_dev_ctx->PipeStateLock, NULL);
    ntStatus = WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(usb_dev_ctx->InputPipe));
    if (!NT_SUCCESS(ntStatus)) {
        WdfWaitLockRelease(usb_dev_ctx->PipeStateLock);
        LOG_E("Failed to start InputPipe"); 
        goto error;
    }
    
    ntStatus =  WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(usb_dev_ctx->OutputPipeHigh));
    if (!NT_SUCCESS(ntStatus)) {        
        WdfWaitLockRelease(usb_dev_ctx->PipeStateLock);
        LOG_E("Failed to start OutputPipeHigh");
        goto error;
    }
    
    ntStatus =  WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(usb_dev_ctx->OutputPipeNormal));
    if (!NT_SUCCESS(ntStatus)) {
        WdfWaitLockRelease(usb_dev_ctx->PipeStateLock);
        LOG_E("Failed to start OutputPipeNormal");
        goto error;
    }

    WdfWaitLockRelease(usb_dev_ctx->PipeStateLock);

error:    
    NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);

	LOG_D("sucess!!!");
	return ndisStatus;
}



NDIS_STATUS wf_usb_dev_stop(PADAPTER adapter)
{
    PUSB_DEVICE_CONTEXT     usb_dev_ctx;

    usb_dev_ctx = GetUsbDeviceContext(adapter->UsbDevice);

    WdfWaitLockAcquire(usb_dev_ctx->PipeStateLock, NULL);
	
    WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(usb_dev_ctx->InputPipe), 
		WdfIoTargetCancelSentIo);
    WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(usb_dev_ctx->OutputPipeNormal),
    	WdfIoTargetCancelSentIo);
    WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(usb_dev_ctx->OutputPipeHigh),
    	WdfIoTargetCancelSentIo);

    WdfWaitLockRelease(usb_dev_ctx->PipeStateLock);
    LOG_D(" Stop all pipes!");  

	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS wf_usb_dev_init(void *adapter)
{
	PADAPTER padapter = adapter;

	wf_usb_dev_create(padapter);

	wf_usb_xmit_init(padapter);

	wf_usb_recv_init(padapter);

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS wf_usb_dev_deinit(void *adapter)
{
	PADAPTER padapter = adapter;

	wf_usb_xmit_deinit(padapter);

	wf_usb_recv_deinit(padapter);

	return NDIS_STATUS_SUCCESS;
}


