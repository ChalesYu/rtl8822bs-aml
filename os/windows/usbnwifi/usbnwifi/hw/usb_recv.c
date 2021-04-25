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


#include "hw_pcomp.h"

#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int

#include "hw_ethnt.h"
#include "8187_gen.h"
#include "8187_reg.h"
#include "usb_recv.h"
#include "usb_main.h"

#define MIN_PACKET_SIZE_802_11      14  // such as ACK)

//#define eqMacAddr(a,b)                      ( ((a)[0]==(b)[0] && (a)[1]==(b)[1] && (a)[2]==(b)[2] && (a)[3]==(b)[3] && (a)[4]==(b)[4] && (a)[5]==(b)[5]) ? 1:0 )

#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)
#pragma warning(disable:4244)
/*
 *  Note:   Encryption overhead handling
 *              1. MPDU tail overhead will be removed before defragment(ex. ICV)
 *              2. MPDU head overhead(except first fragment) will be removed in defragment (ex. IV)
 *              3. MSDU tail overhead will be removed after defragment (ex. MIC)
 *              4. MSDU head overhead will be removed in TranslateRxPacketHeader (ex. IV for first fragment, CKIP head overhead)
*/

EVT_WDF_USB_READER_COMPLETION_ROUTINE ContReadCompletionCallback;

static u8 a[8] = {0xe6,0xb4,0x04,0x0b,0x13,0x08,0xdf,0x3e};
#define	GET_C2H_TX_RPT_LIFE_TIME_OVER(_Header)	LE_BITS_TO_1BYTE((_Header + 0), 6, 1)
#define	GET_C2H_TX_RPT_RETRY_OVER(_Header)	    LE_BITS_TO_1BYTE((_Header + 0), 7, 1)
#define C2H_ID(_c2h)		LE_BITS_TO_1BYTE(((u8*)(_c2h)), 0, 8)
#define C2H_SEQ(_c2h)		LE_BITS_TO_1BYTE(((u8*)(_c2h)) + 1, 0, 8)
#define C2H_PAYLOAD(_c2h)	(((u8*)(_c2h)) + 2)
VOID
FillNicFragment(
    __in  PNIC        Nic,
    PNIC_RX_FRAGMENT    pNicFragment,
    __in  BOOLEAN     bRawReceive
    );

EVT_WDF_USB_READERS_FAILED ReadersFailedCallback;

BOOLEAN
ReadersFailedCallback(
    WDFUSBPIPE      Pipe,
    NTSTATUS        Status,
    USBD_STATUS     UsbdStatus
    )
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
{    
    UNREFERENCED_PARAMETER(Status);
    MpTrace(COMP_RECV, DBG_SERIOUS,("ReadersFailedCallback NTSTATUS 0x%x, UsbdStatus 0x%x\n",
            Status,
            UsbdStatus));
    
    if (UsbdStatus == USBD_STATUS_STALL_PID) {
        //
        // Send a clear stall URB. Pipereset will clear the stall. 
        //
        WdfUsbTargetPipeResetSynchronously(Pipe,
                                           NULL,
                                           NULL
                                           );
    
    }
    return TRUE;
}


NDIS_STATUS
SetUpContinousReader(
    PNIC Nic,
    WDFUSBPIPE  ReadPipe
    )

{
    NDIS_STATUS ndisStatus;
    NTSTATUS ntStatus;
    WDF_USB_CONTINUOUS_READER_CONFIG  config;
    CHAR        dummyMdlBuffer[1];
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;

    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    //
    // Tell the framework that it's okay to read less than
    // MaximumPacketSize
    //
    WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(ReadPipe);
    WDF_USB_CONTINUOUS_READER_CONFIG_INIT(&config,
                                          ContReadCompletionCallback,
                                          Nic,    // Context
                                          MAX_RECEIVE_BUFFER_SIZE );  
    //
    // Reader requests are not posted to the target automatically.
    // Driver must explictly call WdfIoTargetStart to kick start the
    // reader.  In this sample, it's done in D0Entry.
    // By defaut, framework queues two requests to the target
    // endpoint. Driver can configure up to 10 requests with CONFIG macro.
    // I have tried from 4 to 10 readers and it hasn't impacted performance that much
    // TODO : Try adjusting the number of readers to see the optimal value
    //
    config.NumPendingReads = MAX_READS; 
    config.EvtUsbTargetPipeReadersFailed = ReadersFailedCallback;
    //
    // The header is used to allocate an MDL which  maps the Virtual address
    // of the buffer which has the data read from device. By allocating the memory 
    // at initialization it doesn't have to be done at runtime. 
    //
#pragma prefast(disable:6202)
    config.HeaderLength = MmSizeOfMdl(dummyMdlBuffer, MAX_RECEIVE_BUFFER_SIZE);
    pUsbDeviceContext->ReaderMdlSize = (ULONG)config.HeaderLength;
    ntStatus = WdfUsbTargetPipeConfigContinuousReader( ReadPipe,
                                                       &config
                                                        );

    if (!NT_SUCCESS(ntStatus)) {
       MpTrace(COMP_RECV, DBG_SERIOUS,("WdfUsbTargetPipeConfigContinuousReader failed with error %x",ntStatus));
    }
    
    NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
    return ntStatus;
}



NDIS_STATUS
HwUsbAllocateRecvResources(
    __in PNIC Nic
    )
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    NDIS_STATUS ndisStatus;

    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    pUsbDeviceContext->DeviceRundown = FALSE;
    pUsbDeviceContext->ReaderCount = 1;    //bias the count to 1
    KeInitializeEvent(&pUsbDeviceContext->ReaderWaitEvent, SynchronizationEvent, FALSE);  
    //
    // this should be called once the USB device is created
    //
    ndisStatus = SetUpContinousReader(Nic, pUsbDeviceContext->InputPipe);
    return ndisStatus;
}


NDIS_STATUS
HwUsbRecvStart(
    PNIC Nic)
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    NTSTATUS ntStatus;
    NDIS_STATUS ndisStatus;

    ntStatus = STATUS_SUCCESS;
    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    WdfWaitLockAcquire(pUsbDeviceContext->PipeStateLock, NULL);
    ntStatus = WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(pUsbDeviceContext->InputPipe));

    if (!NT_SUCCESS(ntStatus)) {
        WdfWaitLockRelease(pUsbDeviceContext->PipeStateLock);
        MpTrace(COMP_SEND, DBG_SERIOUS,("Failed to start InputPipe\n")); 
        goto error;
    }
    
    WdfWaitLockRelease(pUsbDeviceContext->PipeStateLock);

error:    
    NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);   
    return ndisStatus;
}



NDIS_STATUS
HwUsbRecvStop(
    PNIC Nic
    )
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;

    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    WdfWaitLockAcquire(pUsbDeviceContext->PipeStateLock, NULL);
    WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(pUsbDeviceContext->InputPipe), WdfIoTargetCancelSentIo);
    WdfWaitLockRelease(pUsbDeviceContext->PipeStateLock);
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
HwUsbFreeRecvResources(
    __in PNIC Nic
    )
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    NDIS_STATUS             status;
    LONG                    readerCount;
    //
    // stop the continous reader if it hasn't already and
    //
    status = HwUsbRecvStop(Nic);

    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    pUsbDeviceContext->DeviceRundown = TRUE;
    //
    // We need to make sure all the Continous reader callbacks 
    // have been processed before we return to prevent 
    // leaking WDFMEMORY objects since we take a reference on 
    // these in the Continous reader callback.
    // NOTE:  We initialized the  ReaderCount to 1 so we can 
    // decrement the count here
    //
    readerCount = InterlockedDecrement(&pUsbDeviceContext->ReaderCount);
    if (readerCount != 0) {
        MpTrace(COMP_RECV, DBG_NORMAL,("Waiting for reader Cb's to complete\n"));
        KeWaitForSingleObject(&pUsbDeviceContext->ReaderWaitEvent,
                                       Executive,
                                       KernelMode,
                                       TRUE,
                                       NULL);
    }
    return status;                  
}


VOID
HwUsbProcessReceivedPacket(
    PNIC    Nic,
    PNIC_RX_FRAGMENT pNicFragment
    )
{

    UNREFERENCED_PARAMETER(Nic);
    //
    // The Rx desc. is located at the end of the packet. Get the Rx_Desc
    // TODO: Avoid copying the desciptor. Just store a pointer to it. 
    //

	//RICH: status£¨4byte 0/1£© + rxdes(4byte 2-5) + physts 32byte + payload(data)

	u8 *RxDesc;
    RxDesc = (u8*)pNicFragment->RxBuffer;


	
    QueryRxDescStatus8187( Nic, RxDesc, &pNicFragment->RxDescRichD);

	u8 PhySts;
	PhySts = (u8) GET_RX_STATUS_DESC_PHY_STATUS_9086X(RxDesc);
	
    RX_STATUS_DESC_RICHD *pdesc;
	pdesc = &pNicFragment->RxDescRichD;

    // Deal with c2h related packet. 
    // TODO: Process aggregated packets. 2020/12/22
    if(PhySts==1)
    {
        u16 pkt_len;
        u8 notice;
        notice = (u8) GET_RX_STATUS_DESC_C2H_VALID_9086X(RxDesc);
        if(notice == TRUE )
        {
            pkt_len = pdesc->Length + RTL8187_HWRXDESC_HEADER_LEN + RTL8187_HWRXSTATUS_HEADER_LEN;
            wf_rx_notice_process(pNicFragment->RxBuffer, pkt_len);
        }
    }
	//Different length depend on with or withnot phy status
	if(PhySts==1)
	{
	     pNicFragment->DataSize = pNicFragment->PacketLength  = pdesc->Length;
      /* pNicFragment->DataSize = pNicFragment->PacketLength  = 
	   	             RxDesc.Length - RTL8187_HWRXDESC_HEADER_LEN-RTL8187_HWRXSTATUS_HEADER_LEN;*/
	   	             
	   pNicFragment->RxBuffer = (PETH_RX_BUFFER_STRUC)(Add2Ptr(pNicFragment->RxBuffer,RTL8187_HWRXDESC_HEADER_LEN+RTL8187_HWRXSTATUS_HEADER_LEN));
	   //MpTrace(COMP_RECV, DBG_NORMAL,("PhySts: PacketLength=%d\n",pNicFragment->PacketLength));

	}else
		{
		   pNicFragment->DataSize = pNicFragment->PacketLength  = pdesc->Length;
           /*pNicFragment->DataSize = pNicFragment->PacketLength  = RxDesc.Length - RTL8187_HWRXDESC_HEADER_LEN;*/
		   
		   pNicFragment->RxBuffer = (PETH_RX_BUFFER_STRUC)(Add2Ptr(pNicFragment->RxBuffer,RTL8187_HWRXDESC_HEADER_LEN));
	   	   MpTrace(COMP_RECV, DBG_NORMAL,("NO-PhySts: PacketLength=%d\n",pNicFragment->PacketLength));
	    }
	if(PhySts==1)
	{
		u8* pphy_status = RxDesc + RXDESC_OFFSET;
		Func_Chip_Bb_Status_Query_Send(Nic, pNicFragment, pphy_status,pdesc->Length);
	}
	
}

VOID
FreeNicFragmentLocked(
    PNIC    Nic,
    PNIC_RX_FRAGMENT     pNicFragment
    )
{
    InsertTailList(&Nic->RxInfo.RxFragList, &pNicFragment->FragmentList);
    Nic->RxInfo.RxFragListLen++;
    ASSERT(Nic->RxInfo.RxFragListLen <= Nic->RxInfo.RxTotalDescNum);
}

VOID
FreeNicFragment(
    PNIC    Nic,
    PNIC_RX_FRAGMENT     pNicFragment
    )
{
    NdisAcquireSpinLock(&Nic->RxInfo.ListLock);
    FreeNicFragmentLocked(Nic,pNicFragment);
    NdisReleaseSpinLock(&Nic->RxInfo.ListLock);
}



PNIC_RX_FRAGMENT
GetNicFragmentLocked(
    PNIC Nic
    ) {
    PNIC_RX_FRAGMENT pNicFragment;
    PLIST_ENTRY      ple;

    if(IsListEmpty(&Nic->RxInfo.RxFragList)) {
        pNicFragment= NULL;
        MpTrace(COMP_RECV, DBG_SERIOUS,("Run out of nic fragments\n"));
    }
    else
    {
        ple = RemoveHeadList(&Nic->RxInfo.RxFragList);
        Nic->RxInfo.RxFragListLen--;
        pNicFragment = CONTAINING_RECORD(ple, NIC_RX_FRAGMENT, FragmentList);
    }

    return pNicFragment;    
}


PNIC_RX_FRAGMENT
GetNicFragment(
    PNIC Nic
    ) {
    PNIC_RX_FRAGMENT pNicFragment;
    
    NdisAcquireSpinLock(&Nic->RxInfo.ListLock);
    pNicFragment = GetNicFragmentLocked(Nic);
    NdisReleaseSpinLock(&Nic->RxInfo.ListLock);
    return pNicFragment;
}

VOID
ContReadCompletionCallback(
    WDFUSBPIPE Pipe,
    WDFMEMORY BufferHdl,
    size_t NumBytesRead,
    WDFCONTEXT Context
    )
/*++

Routine Description
    NOTE: In raw mode (DOT11_OPERATION_MODE_NETWORK_MONITOR) - no reassembly is performed.
    We add a reference to the buffer that I get from thw framework since the 
    Continous reader would free the buffer if we didn't have reference.    

Arguments:
    Pipe - Handle to the Receive EP.
    BufferHdl - Handle to a Frameowrk allocated MemoryObject. This is freed on 
                return from this call unless we add a reference to it. 
    NumBytesRead - By the framework continous reader. 
    Context - Context set when we Configured the USB continous reader.
    
Return Value:

    
--*/

{
    PVOID buffer;
    PNIC    Nic ;
    PNIC_RX_FRAGMENT pNicFragment;
    PDOT11_MGMT_HEADER  pMgmtPktHeader;
    PUCHAR  tempBuffer;
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    LONG             readerCount;
	u16 NumBytesProc = 0, Offset;

    UNREFERENCED_PARAMETER(Pipe);

        
    Nic  = (PNIC)Context;  
    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    //
    // The device is being rundown so bail out.
    //
    InterlockedIncrement(&pUsbDeviceContext->ReaderCount);
    if (pUsbDeviceContext->DeviceRundown) {
        InterlockedDecrement(&pUsbDeviceContext->ReaderCount);
        return;
    }

    //
    // The buffer we get points to the header and not the actual data read. 
    // The header is used to allocate an MDL which  maps the Virtual address
    // of the buffer which has the data read from device.  
    //
    tempBuffer =  WdfMemoryGetBuffer(BufferHdl, NULL) ;
    buffer = tempBuffer + pUsbDeviceContext->ReaderMdlSize; 

    if (NumBytesRead == 0 || tempBuffer == NULL) {
        MpTrace(COMP_RECV, DBG_SERIOUS,("ContReadCompletionCallback recvd 0 bytes or NULL buffer\n"));
        goto out;
    }

    //
    // 14(MinPkt, such as ACK) includes 4 byte CRC + 16(Rx_desc),)
    //
    if ( ( NumBytesRead > MAX_RECEIVE_BUFFER_SIZE) ||
        (NumBytesRead <
           ( MIN_PACKET_SIZE_802_11 +  RTL8187_HWRXDESC_HEADER_LEN) ) ) {
        MpTrace(COMP_RECV, DBG_SERIOUS, ("8187 Recv invalid USB bulk in length:%d buffer 0x%p\n",
                                          NumBytesRead, buffer));
       // ASSERT(FALSE);
        goto out;
    }
	do {
		pNicFragment = GetNicFragment(Nic);

		if (pNicFragment == NULL) {
			goto out;
		}

		//
		// initalize Nic Fragment
		//
		pNicFragment->RxBuffer = (PETH_RX_BUFFER_STRUC)buffer;
		HwUsbProcessReceivedPacket(Nic, pNicFragment);
		if (0 == pNicFragment->DataSize || 1600 < pNicFragment->DataSize) {
			Hw11ReturnFragment(Nic, pNicFragment, FALSE);
			break;
		}
		Offset = (pNicFragment->DataSize + (pNicFragment->attrib.physt? RTL8187_HWRXDESC_HEADER_LEN + RTL8187_HWRXSTATUS_HEADER_LEN : RTL8187_HWRXDESC_HEADER_LEN));
		
		pMgmtPktHeader = Hw11GetFragmentDataStart(pNicFragment);
		/*
			MpTrace(COMP_RICHD, DBG_NORMAL,("    Type=%d Subtype=%d\n",
											 pMgmtPktHeader->FrameControl.Type,
											 pMgmtPktHeader->FrameControl.Subtype));
		 */
		if ((pMgmtPktHeader->FrameControl.Type == DOT11_FRAME_TYPE_MANAGEMENT) &&
			(pMgmtPktHeader->FrameControl.Subtype == DOT11_MGMT_SUBTYPE_PROBE_REQUEST)) {


        if (pNicFragment->DataSize >= sizeof(DOT11_MGMT_HEADER)) {
            Hw11HandleTimingCriticalPacket(Nic,
                                           pNicFragment);            
        }

        // pass FALSE for Hw11ReturnFragment as KMDF does not guarantee the function is executed at dispatch level
        Hw11ReturnFragment(Nic, pNicFragment, FALSE);
        goto out;

    }
	

    pNicFragment->Mdl = (PMDL)(tempBuffer);
	MmInitializeMdl(pNicFragment->Mdl, pNicFragment->RxBuffer, pNicFragment->DataSize);
    //
    // Prepare the MDL so the physical page mappings are done
    //
    MmBuildMdlForNonPagedPool(pNicFragment->Mdl);
    WdfSpinLockAcquire(Nic->RxInfo.RecvLock);      
    NdisAcquireSpinLock(&Nic->RxInfo.PendingListLock);
    //
    // Insert the fragment in a list which is processed by the Mp routine.
    // Once the fragments are getting freed this list can be checked and 
    //  the list entries removed. 
    //
    InsertTailList(&Nic->RxInfo.PendingList, &pNicFragment->FragmentList);
    NdisReleaseSpinLock(&Nic->RxInfo.PendingListLock);
    WdfObjectReference(BufferHdl); 
    pNicFragment->BufferHdl = BufferHdl;   
		Mp11ReadCompletionCallback(Nic->Adapter, pNicFragment, 0);
		WdfSpinLockRelease(Nic->RxInfo.RecvLock);
		
		Offset = (u16)_RND8(Offset);
		NumBytesProc += Offset;
		buffer = (PVOID)((PUCHAR)(buffer) + Offset);
		
	} while (NumBytesRead > NumBytesProc);

out:
    readerCount = InterlockedDecrement(&pUsbDeviceContext->ReaderCount);
    if (readerCount == 0) {
        KeSetEvent(&pUsbDeviceContext->ReaderWaitEvent, IO_NO_INCREMENT, FALSE);
    }

    return;
}

VOID
HwUsbFreeRecvFragment(
                __in  PNIC Nic,
                __in  PNIC_RX_FRAGMENT NicFragment
                ) 
{
    UNREFERENCED_PARAMETER(Nic);
    
    if (NicFragment->Mdl) {
       MmPrepareMdlForReuse(NicFragment->Mdl);
       NicFragment->Mdl = NULL;
    }

    //
    // Note: we release the refernce on the Memory handle 
    // after calling  MmPrepareMdlForReuse because NicFragment->Mdl is contained 
    // in the  memory object which we are about to dereference. 
    //
    if (NicFragment->BufferHdl) {
        WdfObjectDereference(NicFragment->BufferHdl);
        NicFragment->BufferHdl = NULL;
    }
    
    if (NicFragment->RxBuffer) {
       NicFragment->RxBuffer = NULL;
    }
}

inline int wf_rx_notice_process(u8 *pbuf, u16 skb_len)
{
    int ret,i,c2h_len, tx_ret;
    u16 seqNum;
    u8 id;
    u8 seq;
    u8 plen;
    u8 *payload;
    u8 changeBuf[256];

    c2h_len = skb_len - RXDESC_SIZE;
    
    if ( c2h_len > 256 || c2h_len < 3)
    {
        return -1;
    }

    for (i = 0; i < c2h_len; i++)
    {
        changeBuf[i] = (pbuf+RXDESC_SIZE)[i]^a[i%8];
    }
    
    ret = rx_c2h_pkt_hdr_parse(changeBuf, c2h_len, &id, &seq, &plen, &payload);

    if (ret == 0)
    {
        if (id== 0x3)
        {
            tx_ret = rx_txDataRpt_parse(payload, plen, &seqNum);
            if (tx_ret == 0)
            {
                MpTrace(COMP_TESTING, DBG_SERIOUS, ("[c2h] tx data[%d] ack report success !!\n",seqNum));
            }
            else
            {
                MpTrace(COMP_TESTING, DBG_SERIOUS, ("[c2h] tx data[%d] ack report failed !!\n",seqNum));
            }
        }
        else if (id == 0x10)
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("[c2h] chbw switch complete !!\n"));
        }
        else if (id == 0x0c)
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("*************************\n"));
            rx_c2h_ra_report_handler(payload, plen);
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("*************************\n"));
        }
        else
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("[c2h] c2h id = %d!!\n", id));
        }
    }

    return 0;
}

static int rx_c2h_pkt_hdr_parse(u8 *buf, u16 len, u8 *id, u8 *seq, u8 *plen, u8 **payload)
{
	if (!buf || len > 256 || len < 3)
		return -1;

	*id = C2H_ID(buf);
	*seq = C2H_SEQ(buf);
	*plen = len - 2;
	*payload = C2H_PAYLOAD(buf);

	return 0;
}

static int rx_txDataRpt_parse(u8 *pdata, u8 len, u16 *seqNum)
{
    int ret;
    
	*seqNum = *(pdata + 6);

	if (GET_C2H_TX_RPT_RETRY_OVER(pdata) | GET_C2H_TX_RPT_LIFE_TIME_OVER(pdata))
    {   
		ret = -1;
	
	}
    else
    {
		ret = 0;
    }

    return ret;
}
void rx_c2h_ra_report_handler(u8 *cmd_buf, u8 cmd_len)
{
	u8	macid = cmd_buf[1];
	u8	rate = cmd_buf[0];
	u8	curr_ra_ratio = 0xff;
	u8	curr_bw = 0xff;
	u8	rate_idx = rate & 0x7f; /*remove bit7 SGI*/
	u8	rate_order;

	if (cmd_len >= 7) 
    {
		curr_ra_ratio = cmd_buf[5];
		curr_bw = cmd_buf[6];
        MpTrace(COMP_TESTING, DBG_SERIOUS, ("RA retry ratio: [%d]:\n", curr_ra_ratio));
	}
	
	if (cmd_buf[3] != 0) 
    {
		if (cmd_buf[3] == 0xff) 
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("FW Level: Fix rate[%d]:\n", macid));
		} 
        else if (cmd_buf[3] == 1) 
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("Try Success[%d]:\n", macid));
		} 
        else if (cmd_buf[3] == 2) 
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("Try Fail & Try Again[%d]:\n", macid));
		} 
        else if (cmd_buf[3] == 3) 
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("rate Back[%d]:\n", macid));
		} 
        else if (cmd_buf[3] == 4) 
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("start rate by RSSI[%d]:\n", macid));
		} 
        else if (cmd_buf[3] == 5) 
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("Try rate[%d]:\n", macid));
		}
	}
	
    MpTrace(COMP_TESTING, DBG_SERIOUS, ("Tx rate Update[%d]:\n", macid));
	phydm_print_rate(rate);
	
}
void phydm_print_rate(u8	rate)
{
	u8		legacy_table[12] = {1, 2, 5, 11, 6, 9, 12, 18, 24, 36, 48, 54};
	u8		rate_idx = rate & 0x7f; /*remove bit7 SGI*/
	u8		vht_en = (rate_idx >= ODM_RATEVHTSS1MCS0) ? 1 : 0;
	u8		b_sgi = (rate & 0x80) >> 7;

    if (rate_idx >= ODM_RATEMCS8)
    {
        MpTrace(COMP_TESTING, DBG_SERIOUS, ("unsupport rate\n"));
    }
    else
    {
        if ((rate_idx >= ODM_RATE1M)&&(rate_idx < ODM_RATE6M))
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("[CCK] %dM\n"));
        }
        else if ((rate_idx >= ODM_RATE6M)&&(rate_idx < ODM_RATEMCS0))
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("[OFDM] %dM\n"));
        }
        else
        {
            MpTrace(COMP_TESTING, DBG_SERIOUS, ("[MCS] index:%d\n"));
        }
    }
}


































