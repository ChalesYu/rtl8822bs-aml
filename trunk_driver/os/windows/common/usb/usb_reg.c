#include "pcomp.h"
#include "wf_debug.h"

int wf_usb_err_status_map(NTSTATUS status)
{
	int local_status = WF_RETURN_FAIL;

	switch(status) {
	case STATUS_NO_SUCH_DEVICE:
		local_status = WF_RETURN_REMOVED_FAIL;
		break;
	default:
		local_status = WF_RETURN_FAIL;
		break;
	}

	return local_status;
}

WDFREQUEST wf_usb_ctrl_req_get(PADAPTER adapter)
{
    WDFREQUEST controlRequest;
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_CONTROL_RESOURCES    controlResources;

    controlRequest = NULL;
    pUsbDeviceContext = GetUsbDeviceContext(adapter->UsbDevice);
    controlResources = &pUsbDeviceContext->ControlResources;
    WdfSpinLockAcquire(controlResources->RequestArrayLock);

    if (controlResources->NextAvailableRequestIndex != 0) {   // Request is available
        --(controlResources->NextAvailableRequestIndex);
        controlRequest = controlResources->RequestArray[controlResources->NextAvailableRequestIndex];
        controlResources->RequestArray[controlResources->NextAvailableRequestIndex] = NULL;
    }

    WdfSpinLockRelease(controlResources->RequestArrayLock);

    //
    // TODO : If we run out of control requests then allocate on the fly 
    // instead of returning NULL
    //
    return controlRequest;
}



VOID
wf_usb_ctrl_req_release(
    __in PADAPTER adapter,
    WDFREQUEST Request
    )

/*++

Routine Description

    Returns a request back to the available array

Arguments:
    Nic - Pointer to the HW NIC structure. 
    Request - Request to be freed.
    
Return Value:
    VOID
    
--*/
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_CONTROL_RESOURCES    controlResources;
    WDF_REQUEST_REUSE_PARAMS    params;

    pUsbDeviceContext = GetUsbDeviceContext(adapter->UsbDevice);
    controlResources = &pUsbDeviceContext->ControlResources;
    WDF_REQUEST_REUSE_PARAMS_INIT(&params, WDF_REQUEST_REUSE_NO_FLAGS, STATUS_SUCCESS);
    WdfRequestReuse(Request, &params);
    WdfSpinLockAcquire(controlResources->RequestArrayLock);
    ASSERTMSG("Control Request list is already full!",
              controlResources->NextAvailableRequestIndex < controlResources->MaxOutstandingRequests
               );
    controlResources->RequestArray[controlResources->NextAvailableRequestIndex++] = Request;
    WdfSpinLockRelease(controlResources->RequestArrayLock);

}


void
wf_usb_write_reg_complete(
    __in WDFREQUEST  Request,
    __in WDFIOTARGET  Target,
    __in PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in WDFCONTEXT  Context
    )
/*++

Routine Description

    Returns a request back to the available array

Arguments:
    Request - The request which completed
    Target - The USB target to which the request was sent. In this case
             it is the Control EP.
    CompletionParams - Has the status and othe information about the 
                        USB request.
                        
    Context - The ciontext which set when the Request was sent. 
    
Return Value:

    
--*/

{
    PUSB_CONTROL_REQ_CONTEXT vendorRequestContext;
    PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;
    NTSTATUS                    status;

    UNREFERENCED_PARAMETER(Target);

    status = CompletionParams->IoStatus.Status;

    //
    // For usb devices, we should look at the Usb.Completion param.
    //
    usbCompletionParams = CompletionParams->Parameters.Usb.Completion;

    if (!NT_SUCCESS(status)) {
    	LOG_E("VendorRequest failed: request Status 0x%x UsbdStatus 0x%x",
           	status, usbCompletionParams->UsbdStatus);
    }

    vendorRequestContext = (PUSB_CONTROL_REQ_CONTEXT)Context;
    //
    // inform NDIS
    //
    //3 Check Normal Queue(This should be transparent for upper layer)

    wf_usb_ctrl_req_release(vendorRequestContext->NdisContext, Request);

}


/*++

Routine Description

    Send an asynchronous request on the pipe

Arguments:
    Nic - Pointer to the HW NIC structure. 
    Request - Request to be used to send the Vendor COntrol request.
    Value - Value to be used in the Control Transfer Setup packet.
    Length - Amount of data to be sent.
    Buffer - Buffer which has the data to be sent.
    Index - Index to be used in the Control Transfer Setup packet.
    
Return Value:
    NTSTATUS
    
--*/
NTSTATUS
wf_usb_write_reg_async (
    __in PADAPTER adapter,
    __in BYTE Request,
    __in USHORT Value,
    __in ULONG Length,
    __in PVOID Buffer,
    __in USHORT Index
    )
{
    WDFREQUEST              controlRequest;
    WDFIOTARGET             ioTarget ;
    NTSTATUS                ntStatus;
    PUSB_CONTROL_REQ_CONTEXT vendorRequestContext;
    WDF_USB_CONTROL_SETUP_PACKET    controlSetupPacket;
    WDFMEMORY  memoryObject;

    LOG_D( "--> ");
    controlRequest = wf_usb_ctrl_req_get(adapter);
    
    if (controlRequest == NULL) {   // No requests left
        LOG_E(" Out of control request");
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    vendorRequestContext = GetControlRequestContext(controlRequest);
    //
    // Copy the buffer which is allocated on the stack  to the heap allocated 
    // Request context because we will be sending an async request. 
    //
    NdisMoveMemory(vendorRequestContext->Data, (PUCHAR)Buffer, Length); 

    WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
                                        BmRequestHostToDevice,
                                        BmRequestToDevice,
                                        Request, // Request
                                        Value, // Value
                                        Index); // Index
    //
    // Create a valid handle to memory object
    //
    ntStatus = WdfMemoryCreatePreallocated(
                        WDF_NO_OBJECT_ATTRIBUTES,   // Attributes 
                        (PVOID)vendorRequestContext->Data,                      
                        Length,                 // BufferSize 
                        &memoryObject                      // Memory 
                        );

    if(!NT_SUCCESS(ntStatus)) {
    	LOG_E("WdfMemoryCreatePreallocated: Failed - 0x%x", ntStatus);
        return ntStatus;
    }
    ntStatus = WdfUsbTargetDeviceFormatRequestForControlTransfer(
                                                                 adapter->UsbDevice,
                                                                 controlRequest,
                                                                 &controlSetupPacket,
                                                                 memoryObject,
                                                                 NULL
                                                                  );

    if(!NT_SUCCESS(ntStatus)) {
    	LOG_E("WdfUsbTargetDeviceFormatRequestForControlTransfer: Failed - 0x%x", ntStatus);
        return ntStatus;
    }

    WdfRequestSetCompletionRoutine(controlRequest, wf_usb_write_reg_complete, vendorRequestContext);

    ioTarget = WdfUsbTargetDeviceGetIoTarget(adapter->UsbDevice);

    if (WdfRequestSend(controlRequest, ioTarget, 0) == FALSE) {   // Failure - Return request
        LOG_E("WdfRequestSend: Failed - 0x%x", ntStatus);
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto error;
    }

    return ntStatus;

error:
    wf_usb_ctrl_req_release(adapter, controlRequest);
    LOG_E("<-- HwUsbSendVendorControlPacketAsync");


    return ntStatus;
}



/*++

Routine Description

	This routine reads a register 

Arguments:

	Nic - One of our device extensions
	
Return Value:

	NT status value

--*/
NTSTATUS 
wf_usb_read_reg(
	__in PADAPTER adapter,
	__in BYTE Request, 
	__in USHORT Value,
	__in ULONG Length,
	__out PVOID Buffer
	)
{
	NTSTATUS status;
	WDF_USB_CONTROL_SETUP_PACKET	controlSetupPacket;
	WDF_MEMORY_DESCRIPTOR memDesc;
	ULONG	 bytesTransferred;
	
	PAGED_CODE();
   // MpTrace (COMP_OID, DBG_TRACE, ( "--> SendVendorSpecificCommand\n"));
	WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
										   BmRequestDeviceToHost,
										   BmRequestToDevice,
										   Request, // Request
										   Value, // Value
										   0); // Index 													   
	WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc, 
									  Buffer, 
									  Length);

	controlSetupPacket.Packet.bm.Byte = 0xc0;
	controlSetupPacket.Packet.bRequest = 0x05;
	controlSetupPacket.Packet.wIndex.Value = 0x0;
	controlSetupPacket.Packet.wValue.Value = Value;
	controlSetupPacket.Packet.wLength = (USHORT)Length;

	status = WdfUsbTargetDeviceSendControlTransferSynchronously(
										adapter->UsbDevice, 
										WDF_NO_HANDLE, // Optional WDFREQUEST
										NULL, // PWDF_REQUEST_SEND_OPTIONS
										&controlSetupPacket,
										&memDesc,
										&bytesTransferred);
	
	if(!NT_SUCCESS(status)) {
		LOG_E("Failed! status=0x%x, val=0x%x, len=%d", status, Value, Length);
		status = wf_usb_err_status_map(status);
	} else {
		status = WF_RETURN_OK;
	}
	return status;
}

/*++

Routine Description

    
Arguments:

    Nic - One of our device extensions
    
Return Value:

    NT status value

--*/
NTSTATUS 
wf_usb_write_reg(
    __in PADAPTER adapter,
    __in BYTE Request, 
    __in USHORT Value,
    __in ULONG Length,
    __in PVOID Buffer,
    __in USHORT Index    
    )
{
    NTSTATUS status;
    WDF_USB_CONTROL_SETUP_PACKET    controlSetupPacket;
    WDF_MEMORY_DESCRIPTOR memDesc;
    ULONG    bytesTransferred;
	wf_usb_info_t *usb_info = adapter->usb_info;
    
    PAGED_CODE();    
	
    WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
                                        BmRequestHostToDevice,
                                        BmRequestToDevice,
                                        Request, // Request
                                        Value, // Value
                                        Index); // Index                                                        
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc, 
                                      Buffer, 
                                      Length);    

	controlSetupPacket.Packet.bm.Byte = 0x40;
	controlSetupPacket.Packet.bRequest = 0x05;
	controlSetupPacket.Packet.wIndex.Value = 0x0;
	controlSetupPacket.Packet.wValue.Value = Value;
	controlSetupPacket.Packet.wLength = (USHORT)Length;

    status = WdfUsbTargetDeviceSendControlTransferSynchronously(
                                        adapter->UsbDevice, 
                                        NULL, // Optional WDFREQUEST
                                        &usb_info->ctrl_req_option, // PWDF_REQUEST_SEND_OPTIONS
                                        &controlSetupPacket,
                                        &memDesc,
                                        &bytesTransferred);

    if(!NT_SUCCESS(status)) {
        LOG_E("Failed! status=0x%x, val=0x%x, len=%d", status, Value, Length);
		status = wf_usb_err_status_map(status);
    } else {
		status = WF_RETURN_OK;
	}
    
    return status;
}


INT32 wf_usb_read(PADAPTER pAdapter, UCHAR flag, UINT addr, PVOID Buffer, INT datalen)
{
	int ret = -1;
	UCHAR temp_buffer4usb[USB_CONTROL_MSG_BF_LEN] = { 0 };

	if (flag == WF_USB_CTL_MSG) {
		ret = wf_usb_read_reg(pAdapter, 0x05, (USHORT)(addr & 0x0000ffff), datalen, temp_buffer4usb);
		if(ret == WF_RETURN_OK) {
			wf_memcpy(Buffer, temp_buffer4usb, datalen);
		}
 	}
	
	return ret;
}


INT32 wf_usb_write(PADAPTER pAdapter, UCHAR flag, UINT addr, PVOID Buffer, INT datalen)
{
	int ret = -1;
	wf_usb_info_t *usb_info = pAdapter->usb_info;
	wf_usb_req_t *req;
	UCHAR temp_buffer4usb[USB_CTRL_MAX_LENGTH] = { 0 };

	if (flag == WF_USB_CTL_MSG) {
		if (datalen > USB_CTRL_MAX_LENGTH) {
			LOG_E("Invalid USB control transmission write out length: %d. (Max value is 64.)", datalen);
			return ret;
		}

		wf_memcpy(temp_buffer4usb, Buffer, USB_CTRL_MAX_LENGTH);
		if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
			ret = wf_usb_write_reg_async(pAdapter, 0x05, (USHORT)(addr & 0x0000ffff), datalen, temp_buffer4usb, 0);
		}
		else {
			ret = wf_usb_write_reg(pAdapter, 0x05, (USHORT)(addr & 0x0000ffff), datalen, temp_buffer4usb, 0);
		}
	}
	else if (flag == WF_USB_BLK_ASYNC) {
		ret = wf_usb_xmit_insert(pAdapter, 0, Buffer, datalen, addr, NULL, NULL, NULL);
		//HwUsbSendPacketAsync(pAdapter, pipe, datalen, Buffer);
	}
	else if (flag == WF_USB_BLK_SYNC) {
		req = &usb_info->sync_request;
		req->data_buf = Buffer;
		req->data_len = datalen;
		ret = wf_usb_xmit_pkt_sync(pAdapter, req);
	}
	else {
		LOG_E("Invalid USB write type. ----CODE:%d.", flag);
	}
	return ret;
}


UCHAR wf_usb_read8(PADAPTER pAdapter,ULONG offset)
{
	UCHAR              data = 0xff;
    NTSTATUS           status;

    status = wf_usb_read_reg(
                         pAdapter,
                         0x05,  //STATNDARD_REQUEST
                         (USHORT)offset,
                         sizeof(UCHAR),
                         &data
                         );
    
    if (!NT_SUCCESS(status)) {
		//LOG_E("reg read failed! off=%x, data=%x", offset, data);
         data = 0xff;
    }
    
    return data;  
}

USHORT wf_usb_read16(PADAPTER pAdapter,ULONG offset)
{
	USHORT              data = 0xffff;
    NTSTATUS            status;

    status = wf_usb_read_reg(
                         pAdapter,
                         0x05,  //STATNDARD_REQUEST
                         (USHORT)offset,
                         sizeof(USHORT),
                         &data
                         );
    
    if (!NT_SUCCESS(status)) {
         data = 0xffff;
    }

    return data;
}

UINT32 wf_usb_read32(PADAPTER pAdapter,ULONG offset)
{
	ULONG              data = 0xffffffff;
    NTSTATUS            status;

    status = wf_usb_read_reg(
                         pAdapter,
                         0x05,  //STATNDARD_REQUEST
                         (USHORT)offset,
                         sizeof(ULONG),
                         &data
                         );
    if (!NT_SUCCESS(status)) {
         data = 0xffffffff;
    }

    return data;  
}

int wf_usb_write8(PADAPTER pAdapter,ULONG addr, UCHAR value)
{
	UCHAR              data = 0xff;
    USHORT             index;

    index =0;
    data = value;
    
    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
		return wf_usb_write_reg_async(pAdapter, 0x05, (USHORT)addr, sizeof(UCHAR), &data, index);
    } else {
        return wf_usb_write_reg(pAdapter, 0x05, (USHORT)addr, sizeof(UCHAR), &data, index);    
    }
}

int wf_usb_write16(PADAPTER pAdapter,ULONG addr, USHORT value)
{
	ULONG              data = 0xffff;
    USHORT             index;

    index =0;
    data = value;
    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
		return wf_usb_write_reg_async(pAdapter, 0x05, (USHORT)addr, sizeof(USHORT), &data, index);
    } else {
        return wf_usb_write_reg(pAdapter, 0x05, (USHORT)addr, sizeof(USHORT), &data, index);
    }
}

int wf_usb_write32(PADAPTER pAdapter,ULONG addr, UINT value)
{
	ULONG              data = 0xffffffff;
    USHORT             index;

    index =0;
    data = value;                       
        
    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
		return wf_usb_write_reg_async(pAdapter, 0x05, (USHORT)addr, sizeof(ULONG), &data, index);
    } else {
    	return wf_usb_write_reg(pAdapter, 0x05, (USHORT)addr, sizeof(ULONG), &data, index) ;
    }
}


