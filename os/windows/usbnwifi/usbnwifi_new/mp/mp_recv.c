/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Mp_Recv.c

Abstract:
    Mp layer receive functions

Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "pcomp.h"
#include "wf_debug.h"

#if DOT11_TRACE_ENABLED
#include "Mp_Recv.tmh"
#endif

//
// Macros that will be used only by this file
//

#define MP_ALLOCATE_MDL(_BufferVa, _BufferLength)   \
    (IoAllocateMdl((PVOID)_BufferVa, (ULONG)_BufferLength, FALSE, FALSE, NULL))

#define MP_MAP_MDL_PHYSICAL_PAGES(_Mdl)         \
    MmBuildMdlForNonPagedPool((PMDL)_Mdl)

#define MP_FREE_MDL(_Mdl)     IoFreeMdl(_Mdl)

#define MP_SET_RECEIVE_CONTEXT(_NBL, _RecvContext)  (NET_BUFFER_LIST_INFO(_NBL, MediaSpecificInformation) = _RecvContext)

#define MP_REMOVE_RX_MSDU_FROM_LIST(_Adapter)           ((PMP_RX_MSDU) InterlockedPopEntrySList(&_Adapter->RxMSDUFreeList))
#define MP_ADD_RX_MSDU_TO_LIST(_Adapter, _Rxd)          InterlockedPushEntrySList(&_Adapter->RxMSDUFreeList, &_Rxd->Link.SListLink)

#define MP_ALLOCATE_RX_MSDU_FROM_POOL(_Adapter)           \
    MP_REMOVE_RX_MSDU_FROM_LIST(_Adapter);       \
    MP_DECREMENT_AVAILABLE_RX_MSDU(_Adapter);

#define MP_FREE_RX_MSDU_TO_POOL(_Adapter, _Rxd)         \
    MP_ADD_RX_MSDU_TO_LIST(_Adapter, _Rxd);     \
    MP_INCREMENT_AVAILABLE_RX_MSDU(_Adapter);

#define MP_SET_RX_MSDU_IN_NBL(_NBL,_Rxd)    (*((PMP_RX_MSDU*)&NET_BUFFER_LIST_MINIPORT_RESERVED(_NBL)) = _Rxd)
#define MP_GET_RX_MSDU_FROM_NBL(_NBL)       (*((PMP_RX_MSDU*)&NET_BUFFER_LIST_MINIPORT_RESERVED(_NBL)))
#define MP_RX_FRAGMENT_GET_OFFSET(_MpFragment)       \
    ((_MpFragment)->Mdl->ByteOffset )

#define MP_RX_FRAGMENT_SET_SIZE(_MpFragment, _Length)       \
    ((_MpFragment)->Mdl->ByteCount = _Length)
#define MP_RX_FRAGMENT_SET_OFFSET(_MpFragment, _Offset)    \
    (_MpFragment)->Mdl->ByteOffset = _Offset;         \
    (_MpFragment)->Mdl->MappedSystemVa = ((PCHAR)(_MpFragment)->Mdl->StartVa) + (_Offset)

/*
 * \internal
 * Matches an Rx MSDUs in Reassembly with the provided sequence number and MAC
 * address. If both the values match, the macro returns true, else returns
 * false
 */
#define MP_REASSEMBLY_RX_MSDU_MATCH(_ReassemblyRxMSDU, _SequenceNumber, _MacAddress)    \
    ((_ReassemblyRxMSDU->SequenceNumber == _SequenceNumber) &&                 \
      (MP_COMPARE_MAC_ADDRESS(                                              \
            _ReassemblyRxMSDU->PeerMacAddress,                                 \
            _MacAddress)))

VOID
Mp11ReadCompletionCallback(
    PADAPTER Adapter,
    PNIC_RX_FRAGMENT NicFragment,
    size_t NumBytesRead
    )
{
    Adapter->ReceiveHandlerFunction(Adapter, NicFragment, NumBytesRead);
}

/**
 * This function releases all resources that were allocated for the Rx MSDU
 * during MpInitializeRxMSDU.
 *
 * \param pAdapter      The adapter context for this miniport
 * \param pMpRxd        The Rx MSDU to be initialized
 * \sa MpInitializeRxMSDU
 */
INLINE VOID
MpTerminateRxMSDU(
    __in PADAPTER     pAdapter,
    __in PMP_RX_MSDU  pMpRxd
    )
{
    UNREFERENCED_PARAMETER(pAdapter);
    MPVERIFY(pMpRxd);

    //
    // Free the NetBufferList
    //
    if (pMpRxd->NetBufferList)
    {
        NdisFreeNetBufferList(pMpRxd->NetBufferList);
    }
}


/**
 * This function will initialize an Rx MSDU and prepare it so it can store NBL
 * information and can be indicated up to NDIS. The allocations include the
 * allocation of NBL and associated Receive Context. A pointer to the
 * Rx MSDU is stored in the NetBufferList as well.
 *
 * \param pAdapter      Context for the adapter for this hardware
 * \param pMpRxd        The Miniport RX_MSDU that is to be initialized
 * \param pNicRxd       The NIC_RX_MSDU that is associated with the pMpRxd
 * \return NDIS_STATUS_SUCCESS if all goes well, else the appropriate failure
 * \sa MpTerminateRxMSDU, MpInitializeReceiveEngine
 */
INLINE NDIS_STATUS
MpInitializeRxMSDU(
    __in PADAPTER     pAdapter,
    __in PMP_RX_MSDU  pMpRxd
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_EXTSTA_RECV_CONTEXT  RecvContext;

    MPVERIFY(pMpRxd);

    do
    {
        //
        // Clear out the Rx MSDU structure
        //
        NdisZeroMemory(pMpRxd, MP_RX_MSDU_MAX_SIZE);

        //
        // Allocate a NetBufferList for this Rx MSDU
        //
        pMpRxd->NetBufferList = NdisAllocateNetBufferList(
                                pAdapter->NetBufferListPool,
                                MP_RECEIVE_NBL_CONTEXT_SIZE,
                                0
                                );

        if (pMpRxd->NetBufferList == NULL)
        {
            //
            // Failed to allocate the NBL. Running low on resources
            //
            LOG_E("Failed to allocate NBL for the associated Rx MSDU");
            ndisStatus = NDIS_STATUS_RESOURCES;
            break;
        }

        //
        // Set the Receive Context as Media Specific Info for this NBL
        //
        RecvContext = MP_RX_MSDU_RECV_CONTEXT(pMpRxd);
        MP_SET_RECEIVE_CONTEXT(pMpRxd->NetBufferList, RecvContext);

        //
        // Store a reference to Rx MSDU in NBL to help on return and in debugging
        //
        MP_SET_RX_MSDU_IN_NBL(pMpRxd->NetBufferList, pMpRxd);
    } while (FALSE);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pMpRxd)
        {
            MpTerminateRxMSDU(pAdapter, pMpRxd);
        }

        // As safety in case we try to accidentally use this Rx MSDU
        NdisZeroMemory(pMpRxd, MP_RX_MSDU_MAX_SIZE);
    }

    return ndisStatus;
}


/**
 * This function is called when a Nic Rx MSDU descriptor is to be associated with a
 * Mp Rx MSDU descriptor. This association is expected to remain static throughout
 * the duration of the lifetime of the Rx MSDU descriptor
 *
 * This function is expected to be called when the Nic is setting up its own
 * Rx MSDU. Being in the hot path, Rx MSDU associations are not expected to change except
 * at Intialize (associate) and Halt (Disassociate) time.
 *
 * \param pAdapter          The context for this miniport
 * \param pNicRxd           The Rx MSDU that is to be Associated
 * \sa MpFreeRxMSDU, MpReInitializeRxMSDU
 */
NDIS_STATUS
MpAllocateRxMSDU(
    __in  PADAPTER        pAdapter,
    __out PMP_RX_MSDU*    ppMpRxd
    )
{
    NDIS_STATUS         ndisStatus;
    PMP_RX_MSDU         pMpRxd;
    NDIS_ERROR_CODE     ErrorCode = NDIS_STATUS_SUCCESS;

    MPASSERTMSG(
        "Lookaside List is NULL. This could be because Hw11 did not call Mp11InitializeReceiveEngine\n",
        pAdapter->RxMSDULookasideList
        );

    do
    {
        //
        // Allocate an Rx MSDU from the Lookaside list
        //
        pMpRxd = (PMP_RX_MSDU) NdisAllocateFromNPagedLookasideList(pAdapter->RxMSDULookasideList);
        if (pMpRxd == NULL)
        {
            LOG_E("Failed to allocate MP_RX_MSDU from lookaside list");
            ndisStatus = NDIS_STATUS_RESOURCES;
            ErrorCode = NDIS_ERROR_CODE_OUT_OF_RESOURCES;
            break;
        }

        //
        // Initialize it. Populates the NDIS_PACKET and the NDIS_BUFFER inside it.
        //
        ndisStatus = MpInitializeRxMSDU(pAdapter, pMpRxd);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            LOG_E("Failed to Initialize MP_RX_MSDU %p", pMpRxd);
            ErrorCode = NDIS_ERROR_CODE_OUT_OF_RESOURCES;   // Only reason why initialize will fail
            break;
        }

        //
        // This RX_MSDU was successfully associated.
        //
        *ppMpRxd = pMpRxd;
    } while(FALSE);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pMpRxd != NULL)
            NdisFreeToNPagedLookasideList(pAdapter->RxMSDULookasideList, pMpRxd);
        if (ErrorCode != NDIS_STATUS_SUCCESS)
        {
            NdisWriteErrorLogEntry(
                pAdapter->MiniportAdapterHandle,
                ErrorCode,
                0
                );
        }
    }

    return ndisStatus;
}


/**
 * This function is called by the Hw portion of the driver to disassociate a
 * Mp Rx MSDU from the Nic Rx MSDU that it was previously associated with, This
 * function will release all resources allocated for the Mp Rx MSDU during the
 * association process.
 *
 * This function is expected to be called when the Nic is cleaning up its own
 * Rx MSDU. Being in the hot path, Rx MSDU associations are not expected to change except
 * at Intialize (associate) and Halt (Disassociate) time. However, growing and
 * shrinking of RX_MSDU pool can occur at run time so we can expect this API to get
 * called during normal running of the driver
 *
 * \param pAdapter          The context for this miniport
 * \param pMpRxd            The Rx MSDU that is to be disassociated
 * \sa MpAllocateRxMSDU, MpReInitializeRxMSDU, MpDropRxMSDU, MpReturnRxMSDU
 */
INLINE VOID
MpFreeRxMSDU(
    __in PADAPTER         pAdapter,
    __in PMP_RX_MSDU      pMpRxd
    )
{
    MPVERIFY(pMpRxd);

    //
    // Deallocate the MP Rx MSDU associated with this Nic Rx MSDU
    //
    MpTerminateRxMSDU(pAdapter, pMpRxd);

    //
    // Now free the Rx MSDU itself
    //
    NdisFreeToNPagedLookasideList(
        pAdapter->RxMSDULookasideList,
        pMpRxd
        );

}


/**
 * This function is used to ReInitialize an Rx MSDU in preparation of a future
 * reuse. On a MiniportReturnNetBufferLists, we will need to reinitialize the
 * Rx MSDUs so that we can reuse it for indicating other frames in the future.
 *
 * \param pMpRxd     The Rx MSDU to be reinitialized
 * \sa MpAllocateRxMSDU
 */
INLINE VOID
MpReInitializeRxMSDU(
    __in PMP_RX_MSDU  pMpRxd
    )
{
    NdisZeroMemory(&(pMpRxd->Flags), (sizeof(MP_RX_MSDU) - FIELD_OFFSET(MP_RX_MSDU, Flags)));
}


/**
 * This internal functions an Rx MSDU to the miniport. This is called when NDIS returns
 * previously indicated Rx MSDUs to the miniport or when an Rx MSDU being reassembled is
 * failed during receive operation and the resources need to be returned to the
 * miniport.
 *
 * \param pAdapter          The adapter context for this miniport
 * \param pMpRxd            The Rx MSDU being returned
 * \param DispatchLevel     TRUE if IRQL is currently DISPATCH
 * \sa MPReturnNetBufferLists, MpDropRxMSDU, MpFreeRxMSDU
 */
INLINE VOID
MpReturnRxMSDU(
    __in  PADAPTER     pAdapter,
    __in  PMP_RX_MSDU  pMpRxd,
    __in  BOOLEAN      DispatchLevel
    )
{
    ULONG i;
    PDOT11_EXTSTA_RECV_CONTEXT  pRecvContext;
    PMP_RX_FRAGMENT             pMpFragment = NULL;
    PNET_BUFFER                 CurrentNetBuffer;

    UNREFERENCED_PARAMETER(DispatchLevel);
    do
    {
        pRecvContext = &pMpRxd->Dot11RecvContext;

        //
        // References to the fragments are present in the Rx MSDU. Go through
        // each fragment and return it to Hw11.
        //
        CurrentNetBuffer = NET_BUFFER_LIST_FIRST_NB(pMpRxd->NetBufferList);
        MPASSERT(CurrentNetBuffer);
        NET_BUFFER_DATA_LENGTH(CurrentNetBuffer) = 0;
        NET_BUFFER_DATA_OFFSET(CurrentNetBuffer) = 0;
        NET_BUFFER_CURRENT_MDL_OFFSET(CurrentNetBuffer) = 0;

        for (i=0; i<(ULONG)pRecvContext->usNumberOfMPDUsReceived; i++)
        {

            //
            // Restore the MDL's ByteCount and ByteOffset
            //
            //pMpFragment = Hw11GetMpFragment(pMpRxd->Fragments[i]);
            MP_RX_FRAGMENT_SET_OFFSET(pMpFragment, pMpFragment->OrigByteOffset);
            MP_RX_FRAGMENT_SET_SIZE(pMpFragment, pMpFragment->OrigByteCount);
            NDIS_MDL_LINKAGE(pMpFragment->Mdl) = NULL;

            //
            // Return this fragment back to Hw11
            //
            //Hw11ReturnFragment(pAdapter->Nic, pMpRxd->Fragments[i], DispatchLevel);
        }

        // Clear out the NetBuffer chain in the NBL. Not required but for safety.
        NET_BUFFER_LIST_FIRST_NB(pMpRxd->NetBufferList) = NULL;

        // Clear out the structure for reuse
        MpReInitializeRxMSDU(pMpRxd);

        //
        // Add this Rx MSDU back to the free list of descriptors
        //
        MP_FREE_RX_MSDU_TO_POOL(pAdapter, pMpRxd);
    }while(FALSE);
}


/**
 * This is an internal helper function. This function determines whether an
 * Rx Fragment can be indicated above to NDIS. The reason for not indicating an Rx fragment to
 * NDIS could be:
 * 1. The Rx fragment is deemed bad/corrupt by the NIC.
 * 2. There is no packet filter set.
 * 3. The Hw11 needs to filter this packet out in software.
 * 4. Miniport is in or transitioning to a low power state.
 *
 * \param pAdapter      The adapter context for this miniport
 * \param pNicFragment  The received fragment
 * \return TRUE if the fragment can be indicated to OS, else false
 * \sa MpHandleDefaultReceiveInterrupt
 */
INLINE BOOLEAN
MpCanIndicateFragment(
    __in PADAPTER             pAdapter,
    __in PNIC_RX_FRAGMENT     pNicFragment
    )
{

    //
    // Let the HW function filter this fragment if needed
    //
#if 0
    if (!Hw11FilterFragment(pAdapter->Nic, pNicFragment))
    {
        //ghy change MpTrace(COMP_RECV, DBG_TRACE,  ("Dropping the packet as it has been filtered by hardware\n"));
        return FALSE;
    }
#endif
    //
    // If device is in low power state, we cannot indicate packets.
    //
    if (MP_IS_LOW_POWER_STATE(pAdapter->DevicePowerState))
    {
        LOG_E("Packet dropped since device is in low power state");
        return FALSE;
    }

    //
    // This fragment can be indicated up to NDIS
    //

    return TRUE;
}


/**
 * Internal helper function to help with reassembly. For each fragment received
 * belonging to an Rx MSDU, this function will add it to the appropriate position.
 * If the fragment cannot be added for any reason, this function will return
 * the appropriate error code.
 *
 * \param pMpRxd            The Rx MSDU in which the fragments are to be assembled
 * \param pNicFragment      The fragment we just received
 * \param pFragmentHdr      The 802.11 header of the received fragment
 * \param usFragmentSize    The size, in bytes, of the received fragment
 * \param ucFragNumber      The 802.11 fragment number of this frag
 * \return NDIS_STATUS_SUCCESS if frag added successfully and the reassembly
 * operation got completed when the frag was added. Will return NDIS_STATUS_PENDING
 * if frag was added successfully but we have still not received all MPDUs for
 * this MSDU. Else, a failure code is returned.
 *
 * \sa MpHandleDefaultReceiveInterrupt, MpPrepareRxMSDUForIndication
 */
INLINE NDIS_STATUS
MpAddRxFragmentToMSDU(
    __in  PMP_RX_MSDU                 pMpRxd,
    __in  PNIC_RX_FRAGMENT            pNicFragment,
    __in  PDOT11_MAC_HEADER           pFragmentHdr,
    __in  USHORT                      usFragmentSize,
    __in  UCHAR                       ucFragNumber
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_PENDING;
    UNREFERENCED_PARAMETER(usFragmentSize);

    // TODO: Add the sliding window logic here based on Per Mac address
    // stats to weed out as many duplicates as possible. This should be optional
    // since hardware could do this as well.

    do
    {
        //
        // If this is a duplicate fragment, drop it
        //
        if (pMpRxd->Fragments[ucFragNumber] != NULL)
        {
            LOG_E("Duplicate packet received!!!");
            ndisStatus = NDIS_STATUS_NOT_ACCEPTED;
            break;
        }

        //
        // If a fragment in this 802.11 packet was lost, fail the entire packet
        //
        if (ucFragNumber > pMpRxd->FragmentCount)
        {
            LOG_E("A fragment in the packet was lost. Received fragment %d, "
                "current fragment count %d. Dropping packet", ucFragNumber, pMpRxd->FragmentCount);
            ndisStatus = NDIS_STATUS_FAILURE;
            break;
        }

        //
        // Is this the last fragment in the frame, we are done
        //
        if (pFragmentHdr->FrameControl.MoreFrag == 0)
        {
            //
            // Tell the caller that the Rx MSDU is now complete.
            //
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        else
        {
            if(pFragmentHdr->FrameControl.Type != DOT11_FRAME_TYPE_DATA)
            {
                //
                // Only data frames can be fragmented. Drop the frame if it is suspicious.
                //
                LOG_E("Error! A non-data frame was received which is fragmented!");
                ndisStatus = NDIS_STATUS_FAILURE;
                break;
            } 
            else
            {
                ndisStatus = NDIS_STATUS_PENDING;
            }
        }

        //
        // This is a good fragment. Add it to the MSDU
        //
        pMpRxd->Fragments[ucFragNumber] = pNicFragment;
        pMpRxd->FragmentCount++;
        
    } while(FALSE);

    return ndisStatus;
}

/**
 * This function is called when all fragments for an Rx MSDU have been reassembled
 * and the Rx MSDU can be indicated up to the OS.
 *
 * \param pAdapter      The adapter context for the miniport
 * \param pMpRxd        The Rx MSDU that just got completed
 * \param pFragmentHdr  The 802.11 header for the last MPDU in the MSDU
 * \return NDIS_STATUS_SUCCESS if preparation succeeds, else failure
 * \sa MpHandleDefaultReceiveInterrupt
 */
NDIS_STATUS
MpPrepareRxMSDUForIndication(
    __in  PADAPTER    pAdapter,
    __in  PMP_RX_MSDU pMpRxd,
    __in  PDOT11_MAC_HEADER   pFragmentHdr
    )
{
    PNET_BUFFER         NetBuffer = NULL;
    PMDL                CurrentMDL = NULL, LastMDL = NULL;
    USHORT              HeaderLength = 0;
    USHORT              FragMDLDataLength = 0;

    PNIC_RX_FRAGMENT    pLastFragment;
    PMP_RX_FRAGMENT     pMpFragment;
    USHORT              i;

#if 0
    do
    {
        //
        // Verify that atleast one fragment is present
        //
        MPVERIFY(pMpRxd->FragmentCount > 0);

        //
        // Construct the MSDU (NB) from the received MPDUs (MDLs)
        //

        //
        // Get the NET_BUFFER from the first MP_RX_MSDU in the chain. This will be used
        // for all fragments in the NET_BUFFER
        //
//        pMpFragment = Hw11GetMpFragment(pMpRxd->Fragments[0]);
//        NetBuffer = pMpFragment->NetBuffer;
//        NET_BUFFER_DATA_LENGTH(NetBuffer) = Hw11GetFragmentDataSize(pMpRxd->Fragments[0]);
        NET_BUFFER_NEXT_NB(NetBuffer) = NULL;
        NET_BUFFER_LIST_FIRST_NB(pMpRxd->NetBufferList) = NetBuffer;

        //
        // Setup the first MDL
        //
        CurrentMDL = pMpFragment->Mdl;
        KeFlushIoBuffers(CurrentMDL, TRUE, TRUE);
        NdisQueryMdlOffset(CurrentMDL, &pMpFragment->OrigByteOffset, &pMpFragment->OrigByteCount);

        //
        // Get the length of 802.11 header so that we can remove it from the fragments
        //
        HeaderLength = DOT11_DATA_SHORT_HEADER_SIZE;
        if ((pFragmentHdr->FrameControl.FromDS == 1) &&
            (pFragmentHdr->FrameControl.ToDS == 1))
        {
            HeaderLength = DOT11_DATA_LONG_HEADER_SIZE;     // Long header
        }

        //
        // Append the MDLs from the remaining fragment MP_RX_MSDUs to this MDL chain
        //
        LastMDL = CurrentMDL;
        for (i=1; i<pMpRxd->FragmentCount; i++)
        {
            //
            // Prepare fragment MDL
            //
//            pMpFragment = Hw11GetMpFragment(pMpRxd->Fragments[i]);
            CurrentMDL = pMpFragment->Mdl;
            KeFlushIoBuffers(CurrentMDL, TRUE, TRUE);

            //
            // Adjust the MDL offsets to ignore the headers and any data offset (saving original)
            //
            NdisQueryMdlOffset(CurrentMDL, &pMpFragment->OrigByteOffset, &pMpFragment->OrigByteCount);
//            FragMDLDataLength = Hw11GetFragmentDataSize(pMpRxd->Fragments[i]) - HeaderLength;
//            pMpFragment->Mdl->ByteOffset += Hw11GetFragmentDataOffset(pMpRxd->Fragments[i]) + HeaderLength;
            pMpFragment->Mdl->MappedSystemVa = ((PCHAR)pMpFragment->Mdl->StartVa) + pMpFragment->Mdl->ByteOffset;
            MP_RX_FRAGMENT_SET_SIZE(pMpFragment, FragMDLDataLength);

            //
            // Add MDL to chain and update total length
            //
            NET_BUFFER_DATA_LENGTH(NetBuffer) += FragMDLDataLength;
            NDIS_MDL_LINKAGE(LastMDL) = CurrentMDL;

            LastMDL = CurrentMDL;
        }

        //
        // Done with all the fragments. Set the last MDL's next to NULL
        //
        NDIS_MDL_LINKAGE(LastMDL) = NULL;

        //
        // Because the miniport reserved field doubles up for timestamp, we have
        // over written the RX_MSDU pointer in that field. Now that packet is reassembled,
        // the timestamp is no longer required. Put the Rx MSDU back in the MP Reserved
        //
        MP_SET_RX_MSDU_IN_NBL(pMpRxd->NetBufferList, pMpRxd);

        //
        // Setup the field in the Dot11RecvContext data structure
        // We will get some of the receive context information for the last
        // fragment and apply it to the entire frame.
        //
        pLastFragment = pMpRxd->Fragments[pMpRxd->FragmentCount - 1];

        pMpRxd->Dot11RecvContext.usNumberOfMPDUsReceived = pMpRxd->FragmentCount;
        pMpRxd->Dot11RecvContext.uReceiveFlags = 0;
        //
        // Allow hardware to fill in the relevant Receive context
        //
        //Hw11FillReceiveContext(pAdapter->Nic, pLastFragment, &pMpRxd->Dot11RecvContext);

    } while(FALSE);
#endif

    return NDIS_STATUS_SUCCESS;
}

/**
 * This function is called when a raw RX MPDU is ready to be indicated
 * up to the OS
 *
 * \param pAdapter      The adapter context for the miniport
 * \param pMpRxd        The Rx MSDU structure that just got completed.
 *                      This would only contain one MPDU (and not yet
 *                      be reassembled
 * \return NDIS_STATUS_SUCCESS if preparation succeeds, else failure
 * \sa MpHandleRawReceiveInterrupt
 */
NDIS_STATUS
MpPrepareRawMPDUForIndication(
    __in  PADAPTER    pAdapter,
    __in  PMP_RX_MSDU pMpRxd
    )
{
    PNET_BUFFER         CurrentNetBuffer = NULL;
    
#if 0
	PMP_RX_FRAGMENT     pMpFragment;

    //
    // Verify that only one fragment is present
    //
    MPVERIFY(pMpRxd->FragmentCount == 1);

    //
    // Construct a NBL for the received MPDUs
    //

    //
    // Get the NetBuffer describing the received MPDU
    //
    //pMpFragment = Hw11GetMpFragment(pMpRxd->Fragments[0]);
    //CurrentNetBuffer = pMpFragment->NetBuffer;
    KeFlushIoBuffers(pMpFragment->Mdl, TRUE, TRUE);
    NdisQueryMdlOffset(pMpFragment->Mdl, &pMpFragment->OrigByteOffset, &pMpFragment->OrigByteCount);

    //
    // Prepare this NetBuffer for indication in its NBL
    //
//    NET_BUFFER_DATA_LENGTH(CurrentNetBuffer) = Hw11GetFragmentDataSize(pMpRxd->Fragments[0]);
    NET_BUFFER_LIST_FIRST_NB(pMpRxd->NetBufferList) = CurrentNetBuffer;
    NET_BUFFER_NEXT_NB(CurrentNetBuffer) = NULL;

    //
    // Because the miniport reserved field doubles up for timestamp, we have
    // over written the RX_MSDU pointer in that field. Now that packet is reassembled,
    // the timestamp is no longer required. Put the Rx MSDU back in the MP Reserved
    //
    MP_SET_RX_MSDU_IN_NBL(pMpRxd->NetBufferList, pMpRxd);

    //
    // Allow hardware to fill in the relevant Receive context
    //
    //Hw11FillReceiveContext(pAdapter->Nic, pMpRxd->Fragments[0], &pMpRxd->Dot11RecvContext);

    //
    // Set raw mode flag
    //
    pMpRxd->Dot11RecvContext.uReceiveFlags |= DOT11_RECV_FLAG_RAW_PACKET;
    pMpRxd->Dot11RecvContext.usNumberOfMPDUsReceived = 1;
#endif
    return NDIS_STATUS_SUCCESS;
}


/**
 * Helper function called when we want to drop the Rx MSDU returning all the fragments
 * and their resources back to the miniport.
 *
 * \param pAdpater      The adapter context for this miniport
 * \param pMpRxd        The Rx MSDU being dropped
 * \param DispatchLevel TRUE if IRQL is DISPATCH
 * \sa MpReturnRxMSDU, MpFreeRxMSDU
 */
INLINE VOID
MpDropRxMSDU(
    __in  PADAPTER        pAdapter,
    __in  PMP_RX_MSDU     pMpRxd,
    __in  BOOLEAN         DispatchLevel
    )
{
    ULONG i;

    //
    // Return all fragments in this Rx MSDU to the hardware.
    //
    for(i=0; i<pMpRxd->FragmentCount; i++)
    {
        //
        // This causes repeated spinlock acquires but that is inexpensive
        // to do at DISPATCH IRQL.
        //
        //Hw11ReturnFragment(pAdapter->Nic, pMpRxd->Fragments[i], DispatchLevel);
    }

    // Clear out the structure for reuse
    MpReInitializeRxMSDU(pMpRxd);
}


/**
 * Called when the miniport determines that a received fragment (MPDU) is part of
 * a bigger MSDU and we have to wait till all MPDU's have been received for this
 * miniport.
 *
 * \param pAdapter      The adapter context for this miniport
 * \param pMpRxd        The Rx MSDU in which all the fragments will be assembled for this MSDU
 * \param DispatchLevel TRUE if current IRQL is DISPATCH
 * \return NDIS_STATUS_SUCCESS if added successfully, else failure
 * \sa MpRemoveMSDUFromReassemblyLine
 */
NDIS_STATUS
MpAddPartialMSDUToReassemblyLine(
    __in  PADAPTER    pAdapter,
    __in  PMP_RX_MSDU pMpRxd,
    __in  BOOLEAN     DispatchLevel
    )
{
    USHORT           i;

    //
    // If we are running low on reassembly line space,
    // expire some ReceiveLifeTime packets
    //
    if (pAdapter->TotalRxMSDUInReassembly >= MP_REASSEMBLY_LINE_LOW_RESOURCES_THRESHOLD)
    {
        MpExpireReassemblingPackets(pAdapter, DispatchLevel);
    }

    for(i=0; i<MP_MAX_REASSEMBLY_LINE_SIZE; i++)
    {
        if(pAdapter->ReassemblyLine[i] == NULL)
        {
            //
            // Found an empty spot for this Rx MSDU in the assembly line
            //
            pAdapter->ReassemblyLine[i] = pMpRxd;
            pAdapter->TotalRxMSDUInReassembly++;

            //
            // store position index in the Rx MSDU & Timestamp this MSDU
            //
            pMpRxd->ReassemblyLinePosition = i;
            MP_TIMESTAMP_NBL(pMpRxd->NetBufferList, pAdapter->MaxRxLifeTime);

            //
            // The Most Recently Used Assembly Rx MSDU becomes this one
            // This gives us an optimization by avoiding search
            // as we expect next receive to belong to this MSDU
            //
            pAdapter->MRUReassemblyRxMSDU = pMpRxd;

            return NDIS_STATUS_SUCCESS;
        }
    }

    return NDIS_STATUS_RESOURCES;
}


/**
 * Called when an Rx MSDU previously added to Reassembly line has to be removed. The MSDU
 * may have been assembled successfully or an unrecoverable error may have caused us
 * to drop all fragments in this MSDU
 *
 * \param pAdapter      The adapter context for this miniport
 * \param pMpRxd        The Rx MSDU to be dropped
 * \sa MpAddPartialMSDUToReassemblyLine
 */
VOID
MpRemoveMSDUFromReassemblyLine(
    __in  PADAPTER    pAdapter,
    __in  PMP_RX_MSDU pMpRxd
    )
{
    //
    // The position of this Rx MSDU in the Reassembly line is
    // stored in the MSDU. Use that to quickly remove the
    // Rx MSDU from reassembly line
    //
    MPVERIFY(pAdapter->ReassemblyLine[pMpRxd->ReassemblyLinePosition] == pMpRxd);
    pAdapter->ReassemblyLine[pMpRxd->ReassemblyLinePosition] = NULL;
    pAdapter->TotalRxMSDUInReassembly--;

    //
    // If this is the MRU Reassembly MSDU, clear that
    //
    if (pAdapter->MRUReassemblyRxMSDU == pMpRxd)
        pAdapter->MRUReassemblyRxMSDU = NULL;
}


/**
 * This function searches for an Rx MSDU in the reassembly line given the
 * sequence number and the Fragment Hdr (to match MAC address) of an 802.11 packet
 *
 * \param pAdapter          The adapter context for this miniport
 * \param pFragmentHdr      802.11 header of a received fragment for which we need a match
 * \param usSequenceNumber  The Sequence Number we will use to match the RX_MSDU
 * \param
 * \return
 * \sa
 */
INLINE PMP_RX_MSDU
MpFindPartialMSDUInReassemblyLine(
    __in PADAPTER     pAdapter,
    __in PMP_DOT11_MGMT_DATA_MAC_HEADER pFragmentHdr,
    __in USHORT       usSequenceNumber
    )
{
    PMP_RX_MSDU pMpRxd;
    USHORT      i;

    //
    // First check the MRU Reassembly MSDU.
    // Make sure there is some Rx MSDU in reassembly
    //
    if (pAdapter->TotalRxMSDUInReassembly == 0)
    {
        MPVERIFY(pAdapter->MRUReassemblyRxMSDU == NULL);
        return NULL;
    }
    else
    {
        pMpRxd = pAdapter->MRUReassemblyRxMSDU;
    }

    if (pMpRxd && MP_REASSEMBLY_RX_MSDU_MATCH(
            pMpRxd,
            usSequenceNumber,
            &pFragmentHdr->Address2
            ))
    {
        //
        // Match found. Optimal search.
        //
        return pMpRxd;
    }
    else
    {
        //
        // We will have to go through the reassembly line and
        // search manually
        //
        for(i=0; i<MP_MAX_REASSEMBLY_LINE_SIZE; i++)
        {
            pMpRxd = pAdapter->ReassemblyLine[i];
            if (pMpRxd && MP_REASSEMBLY_RX_MSDU_MATCH(
                    pMpRxd,
                    usSequenceNumber,
                    &pFragmentHdr->Address2
                    ))
            {
                //
                // Found the match through manual search
                // Set this as the MRU and return it
                //
                LOG_D("*** UNOPTIMAL SRCH ***");
                pAdapter->MRUReassemblyRxMSDU = pMpRxd;
                return pMpRxd;
            }
        }

        return NULL;
    }
}


/**
 * This function goes through the Reassembly line and drops all Rx MSDUs that
 * have we were not able to assemble in the RX_LIFETIME of the packet. This
 * function needs to be run periodically to help get rid of any orphan Rx MSDUs in
 * the reassembly line that can never be completed.
 *
 * \param pAdapter          The adapter context for this miniport
 * \param DispatchLevel     TRUE if IRQL is DISPATCH
 * \sa MpAddPartialMSDUToReassemblyLine
 */
INLINE VOID
MpExpireReassemblingPackets(
    __in  PADAPTER    pAdapter,
    __in  BOOLEAN     DispatchLevel
    )
{
    ULONG           i, NumReassemblies;
    PMP_RX_MSDU     pMpRxd;
    LARGE_INTEGER   CurrentTickCount;

    NumReassemblies = pAdapter->TotalRxMSDUInReassembly;   // for optimization

    //
    // Get the current time
    //
    KeQueryTickCount(&CurrentTickCount);

    for(i=0; (NumReassemblies>0 && i<MP_REASSEMBLY_LINE_LOW_RESOURCES_THRESHOLD); i++)
    {
        if ((pMpRxd = pAdapter->ReassemblyLine[i]) != NULL)
        {
            //
            // If this packet has exceeded the MaxRxLifetime, expire it
            //
            if (CurrentTickCount.QuadPart > MP_TIMESTAMP_FIELD(pMpRxd->NetBufferList)->QuadPart)
            {
                LOG_D("Expiring Rx MSDU with Seq: %d", pMpRxd->SequenceNumber);
                MpRemoveMSDUFromReassemblyLine(pAdapter, pMpRxd);
                MpDropRxMSDU(pAdapter, pMpRxd, DispatchLevel);
                MP_FREE_RX_MSDU_TO_POOL(pAdapter, pMpRxd);
                MP_INCREMENT_REASSEMBLY_FAILED_COUNT(pAdapter);
            }

            //
            // We found one more Reassembly.
            //
            NumReassemblies--;
        }
    }
}



/**
 * This method is called by the underlying Hw11 when it needs to associate a
 * NIC_RX_FRAGMENT data structure with MP_RX_FRAGMENT. Typically, a single fragment
 * will correspond to a single Hardware Receive buffer for the Hw11. Since,
 * an 802.11 frame can consist of multiple fragments, Hw11 should add each
 * fragment to the Rx MSDU and then indicate the Rx MSDU when the frame is completely
 * assembled. The Mp functions will go through the fragments and add them
 * appropriately to the NDIS_PACKET that will be indicated to NDIS above.
 *
 * Please note that the Hw11 must keep a reference to the MP_RX_FRAGMENT (returned
 * in ppMpFragment) saved since Mp function will query for it at later point using
 * Hw11GetMpFragment
 *
 *
 * \param pAdapter      The adapter context for this miniport
 * \param Fragment      The NIC_RX_FRAGMENT to be associated
 * \param ppMpFragment  The MpFragment associated is returned in this variable.
 * \return NDIS_STATUS_SUCCESS on success, else the appropriate error.
 * \sa Mp11DisassociateRxFragment, Hw11GetMpFragment, Hw11GetFragment
 */
NDIS_STATUS
 Mp11AssociateRxFragment(
    __in  PADAPTER         pAdapter,
    __in  PNIC_RX_FRAGMENT Fragment,
    __deref_out_opt PMP_RX_FRAGMENT* ppMpFragment
    )
{
    NDIS_STATUS     ndisStatus;
    //PVOID           pSysVa;
    PMP_RX_MSDU     pMpRxd = NULL;
    PMP_RX_FRAGMENT pMpFragment;

    UNREFERENCED_PARAMETER(Fragment);

    //
    // Allocate an MDL for this Rx MSDU and map it to
    // the memory where Hw11 will place the data.
    // Also allocate an MP_RX_MSDU for each fragment that is allocated.
    // In the extreme case that all received fragments are complete
    // packets, we need 1 Rx MSDU per fragment.
    // \warning The sample driver makes an assumption about the
    // underlying driver being able to indicate on MPDU in a single
    // MDL. This assumption should hold true for almost all hardware
    // but if there is any out there that does not fit the model, these
    // functions will have to be modified accordingly
    //
    do
    {

        pMpFragment = NULL;
        //
        // Allocate a MPFRAGMENT to associate with the NICFRAGMENT
        //
        MP_ALLOCATE_MEMORY(
            pAdapter->MiniportAdapterHandle,
            &pMpFragment,
            sizeof(MP_RX_FRAGMENT),
            MP_MEMORY_TAG
            );
        if (pMpFragment == NULL)
        {
            LOG_E("Failed to allocate MPFRAGMENT");
            ndisStatus = NDIS_STATUS_RESOURCES;
            break;
        }
        NdisZeroMemory(pMpFragment, sizeof(MP_RX_FRAGMENT));

        //
        // NOTE: Unlike PCI Allocation of MDL's, NetBuffers, physical page mappings 
        // is not done here and instead we map MDL's 
        //  when we receive a USB completion.
        //
        *ppMpFragment = pMpFragment;

        ndisStatus = MpAllocateRxMSDU(pAdapter, &pMpRxd);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            break;
        }

        //
        // Insert this Rx MSDU in the free list and increment Free Rx MSDU count
        // Also increment the count of total Rx MSDU allocated
        //
        MP_FREE_RX_MSDU_TO_POOL(pAdapter, pMpRxd);
        MP_INCREMENT_TOTAL_RX_MSDU_ALLOCATED(pAdapter);
    } while (FALSE);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pMpFragment)
        {
            pMpFragment->Mdl = NULL;
            if (pMpFragment->NetBuffer != NULL) {
                NdisFreeNetBuffer(pMpFragment->NetBuffer);
                pMpFragment->NetBuffer = NULL;
            }
            MP_FREE_MEMORY(pMpFragment);
            *ppMpFragment = NULL;
        }
    }

    return ndisStatus;
}

VOID
Mp11FreeRxNetBuffer(
    __in PNIC_RX_FRAGMENT pNicFragment
    )
{
#if 0
	PMP_RX_FRAGMENT pMpFragment;// = Hw11GetMpFragment(pNicFragment);
    
    MPVERIFY(pMpFragment);
    if (pMpFragment->NetBuffer != NULL)
    {
        NdisFreeNetBuffer(pMpFragment->NetBuffer);
        pMpFragment->NetBuffer = NULL;
    }
#endif
}

/**
 * This function is called to disassociate a  previously associated fragment.
 * Typically, this will happen during MiniportHalt.
 *
 * \param pAdapter      The adapter context for this miniport
 * \param pNicFragment  The Nic Fragment to be disassociated.
 * \sa Mp11AssociateRxFragment
 */
VOID
Mp11DisassociateRxFragment(
    __in PADAPTER         pAdapter,
    __in PNIC_RX_FRAGMENT pNicFragment
    )
{
    PMP_RX_FRAGMENT  pMpFragment;
    UNREFERENCED_PARAMETER(pAdapter);

#if 0
//    pMpFragment = Hw11GetMpFragment(pNicFragment);
    MPVERIFY(pMpFragment);
    //
    // Free the Fragment and its associated NetBuffer
    //
    //
    // Note: this is different from PCI since the MDL is
    //  allocated by hw so there is no need to free it.  
    //
    pMpFragment->Mdl = NULL;
    if (pMpFragment->NetBuffer != NULL) {
        NdisFreeNetBuffer(pMpFragment->NetBuffer);
        pMpFragment->NetBuffer = NULL;

    }
    MP_FREE_MEMORY(pMpFragment);
#endif
}


/**
 * This function allocates and initializes all the resources needed for receives to
 * occur. This function will also call into Hw11 function to allow shared memory and
 * other allocations and initializations specific to the hardware to take place.
 * If an error occurs during initialization, a entry will be added to the Event Log
 * indicating the error.
 *
 * This function assumes that the TotalRxMSDU and MaxRxMSDU fields of the Adapter are valid
 * and setup as requested by the user.
 *
 * \param pAdapter      The Adapter Context associated with this hardware
 * \uNumFragments       The number of fragments to be used.
 * \return NDIS_STATUS_SUCCESS, else appropriate error code
 * \sa MpInitializeSendEngine, MpInitializeRxMSDU, Hw11InitializeReceive,
 *     Mp11TerminateReceiveEngine
 */
NDIS_STATUS
Mp11InitializeReceiveEngine(
    __in  PADAPTER            pAdapter,
    __in ULONG                uNumFragments
    )
{
    NDIS_STATUS                         ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_ERROR_CODE                     ErrorCode = NDIS_STATUS_SUCCESS;
    NET_BUFFER_LIST_POOL_PARAMETERS     NBLPoolParameters;
    NET_BUFFER_POOL_PARAMETERS          NBPoolParameters;

    UNREFERENCED_PARAMETER(uNumFragments);

    MPVERIFY(uNumFragments == pAdapter->TotalRxMSDU);
    MPVERIFY(MP_REASSEMBLY_LINE_LOW_RESOURCES_THRESHOLD <= MP_MAX_REASSEMBLY_LINE_SIZE);

    do
    {
        //
        // Allocate and initialize the lookaside list from which
        // RX_MSDU's will be allocated
        //
        MP_ALLOCATE_MEMORY(pAdapter->MiniportAdapterHandle, &pAdapter->RxMSDULookasideList, sizeof(NPAGED_LOOKASIDE_LIST), MP_MEMORY_TAG);
        if (pAdapter->RxMSDULookasideList == NULL)
        {
            LOG_E("Failed to allocate NonPaged Lookaside list for MP Rx MSDU's");
            ErrorCode = NDIS_ERROR_CODE_OUT_OF_RESOURCES;
            ndisStatus = NDIS_STATUS_RESOURCES;
            break;
        }

        NdisInitializeNPagedLookasideList(
            pAdapter->RxMSDULookasideList,
            NULL,
            NULL,
            0,
            MP_RX_MSDU_MAX_SIZE,
            MP_MEMORY_TAG,
            0
            );

        //
        // Allocate the NBL Pool
        //
        NdisZeroMemory(&NBLPoolParameters, sizeof(NET_BUFFER_LIST_POOL_PARAMETERS));
        NBLPoolParameters.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
        NBLPoolParameters.Header.Revision = NET_BUFFER_LIST_POOL_PARAMETERS_REVISION_1;
        NBLPoolParameters.Header.Size = sizeof(NET_BUFFER_LIST_POOL_PARAMETERS);
        NBLPoolParameters.fAllocateNetBuffer = FALSE;
        NBLPoolParameters.ContextSize = MP_RECEIVE_NBL_CONTEXT_SIZE;
        NBLPoolParameters.PoolTag = MP_RX_NET_BUFFER_LIST_POOL_TAG;
        NBLPoolParameters.DataSize = 0;

        pAdapter->NetBufferListPool = NdisAllocateNetBufferListPool(
            pAdapter->MiniportAdapterHandle,
            &NBLPoolParameters
            );
        if (pAdapter->NetBufferListPool == NULL)
        {
            LOG_E("Failed to allocate NetBufferList Pool");
            ndisStatus = NDIS_STATUS_RESOURCES;
            ErrorCode = NDIS_ERROR_CODE_OUT_OF_RESOURCES;
            break;
        }

        NdisZeroMemory(&NBPoolParameters, sizeof(NET_BUFFER_POOL_PARAMETERS));
        NBPoolParameters.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
        NBPoolParameters.Header.Revision = NET_BUFFER_POOL_PARAMETERS_REVISION_1;
        NBPoolParameters.Header.Size = sizeof(NET_BUFFER_POOL_PARAMETERS);
        NBPoolParameters.PoolTag = MP_RX_NET_BUFFER_POOL_TAG;
        NBPoolParameters.DataSize = 0;
        pAdapter->NetBufferPool = NdisAllocateNetBufferPool(
            pAdapter->MiniportAdapterHandle,
            &NBPoolParameters
            );
        if (pAdapter->NetBufferPool == NULL)
        {
            LOG_E("Failed to allocate NetBuffer Pool");
            ndisStatus = NDIS_STATUS_RESOURCES;
            ErrorCode = NDIS_ERROR_CODE_OUT_OF_RESOURCES;
            break;
        }

        //
        // Initialize the List Head for the Free List and Reassembly List of MSDU's
        //
        InitializeSListHead(&pAdapter->RxMSDUFreeList);

        //
        // Other variable initializations
        //
        pAdapter->ReassemblyLineCleanupCountdown = MP_REASSEMBLY_CLEANUP_COUNTDOWN_VALUE;
    } while (FALSE);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        //
        // This function will safely cleanup all allocated resources
        //
        Mp11TerminateReceiveEngine(pAdapter);

        if (ErrorCode != NDIS_STATUS_SUCCESS)
        {
            NdisWriteErrorLogEntry(
                pAdapter->MiniportAdapterHandle,
                ErrorCode,
                0
                );
        }
    }

    return ndisStatus;
}


VOID
Mp11TerminateReceiveEngine(
    __in PADAPTER pAdapter
    )
{
    PMP_RX_MSDU pMpRxd;

    //
    // Release all the receive related resources
    //
    while (ExQueryDepthSList(&pAdapter->RxMSDUFreeList) != 0)
    {
        pMpRxd = (PMP_RX_MSDU) MP_ALLOCATE_RX_MSDU_FROM_POOL(pAdapter);
        MpFreeRxMSDU(pAdapter, pMpRxd);
    }

    if (pAdapter->RxMSDULookasideList)
    {
        NdisDeleteNPagedLookasideList(pAdapter->RxMSDULookasideList);
        MP_FREE_MEMORY(pAdapter->RxMSDULookasideList);
        pAdapter->RxMSDULookasideList = NULL;
    }

    if (pAdapter->NetBufferListPool)
    {
        NdisFreeNetBufferListPool(pAdapter->NetBufferListPool);
        pAdapter->NetBufferListPool = NULL;
    }

    if (pAdapter->NetBufferPool)
    {
        NdisFreeNetBufferPool(pAdapter->NetBufferPool);
        pAdapter->NetBufferPool = NULL;
    }
}

VOID
MpAdjustReceiveHandler(
    __in PADAPTER         pAdapter
    )
{
	BOOLEAN safeMode = FALSE;

    if (pAdapter->OperationMode == DOT11_OPERATION_MODE_EXTENSIBLE_STATION)
    {    	
        if (safeMode)
        {
            // Safe mode
            pAdapter->ReceiveHandlerFunction = MpHandleSafeModeReceiveInterrupt;
        }
        else	
        {
            // Default behavior
            pAdapter->ReceiveHandlerFunction = MpHandleDefaultReceiveInterrupt;            
        }
    }
    else
    {
        // Network monitoring mode
        pAdapter->ReceiveHandlerFunction = MpHandleRawReceiveInterrupt;
    }
}

/**
 * Internal debugging helper function
 */
BOOLEAN
MpDebugMatchPacketType(
    __in PADAPTER             pAdapter,
    __in PDOT11_MAC_HEADER    pFragmentHdr
    )
{
    UCHAR   Type = DOT11_FRAME_TYPE_DATA;
    UCHAR   SubType = DOT11_DATA_SUBTYPE_DATA;
    BOOLEAN bDirectedPacket = FALSE;
    DOT11_MAC_ADDRESS   Address2 = {0x00, 0x02, 0x44, 0xB5, 0x8F, 0x63};   // Generally the AP's address

    if (Type != pFragmentHdr->FrameControl.Type)
    {
        return FALSE;
    }

    if (SubType != pFragmentHdr->FrameControl.Subtype)
    {
        return FALSE;
    }

    // Compare address 1
    if (bDirectedPacket)
    {
        if (!MP_COMPARE_MAC_ADDRESS(pFragmentHdr->Address1, pAdapter->CurrentAddress))
        {
            return FALSE;
        }
    }
    else
    {
        if (!ETH_IS_BROADCAST(pFragmentHdr->Address1))
        {
            return FALSE;
        }
    }

    // if applicable, compare address 2
    if (Type != DOT11_FRAME_TYPE_CONTROL)
    {
        PMP_DOT11_MGMT_DATA_MAC_HEADER  pHeader = (PMP_DOT11_MGMT_DATA_MAC_HEADER)pFragmentHdr;
        if (!MP_COMPARE_MAC_ADDRESS(pHeader->Address2, Address2))
        {
            return FALSE;
        }
    }

    return TRUE;
}



/**
 * This function is called by NDIS when the protocol above returns NetBufferLists
 * previously indicated by this miniport.
 *
 * \param MiniportAdapterContext    The adapter context for this miniport
 * \param NetBufferLists            The NBLs that was previously indicated to NDIS
 * \param ReturnFlags               Flags for return information (dispatch level, etc)
 * \sa Hw11ReturnFragment
 */
VOID
MPReturnNetBufferLists(
    __in  NDIS_HANDLE         MiniportAdapterContext,
    __in  PNET_BUFFER_LIST    NetBufferLists,
    __in  ULONG               ReturnFlags
    )
{
    PADAPTER            pAdapter = (PADAPTER) MiniportAdapterContext;
    PNET_BUFFER_LIST    CurrentNetBufferList, NextNetBufferList = NULL;
    PMP_RX_MSDU         pMpRxd;
    ULONG               PercentMSDUListUnunsed;
    BOOLEAN             DispatchLevel = ReturnFlags & NDIS_RETURN_FLAGS_DISPATCH_LEVEL ? TRUE : FALSE;
	LOG_D("NDIS returns NetBufferLists indicated before.");
    for (CurrentNetBufferList = NetBufferLists;
          CurrentNetBufferList != NULL;
          CurrentNetBufferList = NextNetBufferList)
    {
        pMpRxd = MP_GET_RX_MSDU_FROM_NBL(CurrentNetBufferList);

        //
        // Check if we need to shrink the Rx MSDU list.
        // We have been sampling how many Rx MSDUs remain unused at every
        // two second interval. If the average number of free Rx MSDUs remain
        // above a threshold for a statistically significant time interval,
        // we will free this Rx MSDU in order to shrink the MSDU list.
        //
        if (pAdapter->RxMSDUListSampleTicks > MP_RX_MSDU_LIST_SAMPLING_PERIOD &&
            pAdapter->TotalRxMSDUAllocated > pAdapter->TotalRxMSDU)
        {
            //
            // Determine the percentage of Rx MSDU list that has remained underutilized
            // for last RxMSDUListSampleTicks time interval
            //
            PercentMSDUListUnunsed = (pAdapter->NumRxMSDUNotUtilized * 100) / (pAdapter->RxMSDUListSampleTicks * pAdapter->TotalRxMSDUAllocated);

            //
            // Restart sampling for the next MP_RX_MSDU_LIST_SAMPLING_PERIOD of time
            //
            pAdapter->RxMSDUListSampleTicks = 0;
            pAdapter->NumRxMSDUNotUtilized = 0;

            if (PercentMSDUListUnunsed > MP_RX_MSDU_LIST_UNDER_UTILIZATION_THRESHOLD)
            {
                LOG_D("Shrinking the Rx MSDU pool. Current Size: %d   Percentage Under Utilization: %d",
                                pAdapter->TotalRxMSDUAllocated, PercentMSDUListUnunsed);
                //
                // The threshold was exceeded, lets free this MSDU
                //
                MPVERIFY(pMpRxd->FragmentCount > 0);

                //
                // We will free the last fragment in this Rx MSDU and return
                // the rest of the fragments to the Hw11 for reuse
                //
                #if 0
                Hw11FreeFragment(
                    pAdapter->Nic,
                    pMpRxd->Fragments[--pMpRxd->FragmentCount]
                    );
				#endif
                NextNetBufferList = NET_BUFFER_LIST_NEXT_NBL(CurrentNetBufferList);
                MpDropRxMSDU(pAdapter, pMpRxd, DispatchLevel);
                MpFreeRxMSDU(pAdapter, pMpRxd);
                MP_DECREMENT_TOTAL_RX_MSDU_ALLOCATED(pAdapter);
                break;
            }
        }

        //
        // Shrinking not needed. Just reclaim the MSDU. This also returns the
        // fragments to the Hw11.
        //
        MPVERIFY(pMpRxd->NetBufferList == CurrentNetBufferList);
        MPVERIFY(MP_TEST_FLAG(pMpRxd, MP_RX_MSDU_NBL_INDICATED));
        NextNetBufferList = NET_BUFFER_LIST_NEXT_NBL(CurrentNetBufferList);
        MpReturnRxMSDU(pAdapter, pMpRxd, DispatchLevel);

        //
        // Decrement pending receives and if down to zero, signal the event
        // Halt handler could be waiting on this event.
        //

        if (MP_DECREMENT_PENDING_RECEIVES(pAdapter) == 0) {        
            NdisSetEvent(&pAdapter->AllReceivesHaveReturnedEvent);
        }
        
    } while(FALSE);
}



/**
 * This function is called during an InterruptHandle DPC to check
 * for raw packet receives. This is only called when we are in the
 * raw receive mode
 *
 * \param pAdapter      The adapter context for the miniport
 * \sa MPHandleInterrupt
 */
VOID
MpHandleRawReceiveInterrupt(
    __in PADAPTER         pAdapter,
    PNIC_RX_FRAGMENT    pNicFragment,
    __in size_t           Size
    )
{
    ULONG               NumMSDUToIndicate = 0, NumLowResourcePackets = 0;
    ULONG               OldPendingRecvCount, i, NumActiveReceivers;
    USHORT              usFragmentSize, usFragmentOffset, finalOffset;
    PMP_RX_MSDU         pMpRxd = NULL;
    //PNIC                pNic = pAdapter->Nic;
    PMP_RX_FRAGMENT     pMpFragment;
    PNET_BUFFER_LIST    ReceivedNBLChain = NULL, LastReceivedNBL = NULL;
    PNET_BUFFER_LIST    LowResourceNBLChain = NULL, LastLowResourceNBL = NULL;
    BOOLEAN             bGrowRxMSDUPool = FALSE;
    NDIS_STATUS         ndisStatus;

    UNREFERENCED_PARAMETER(Size);
    //
    // We have to make sure that the we can receive right now. If a Pause or reset
    // is in progress, we should not indicate packets up to the protocol anymore.
    // Since this function is not protected by a spinlock (for perf reasons), we have
    // to use Ref Counts to make sure that this DPC is not running if Pause or reset
    // etc. is running.
    //
    NumActiveReceivers = MP_INCREMENT_ACTIVE_RECEIVERS(pAdapter);
    if (NumActiveReceivers <= 1) {
        //
        // Either a pause or reset (MiniportReset or Dot11Reset) is in progress.
        // We will not process this receive. Reduce the active receiver count
        // as this DPC is now done.
        //
        MP_DECREMENT_ACTIVE_RECEIVERS(pAdapter);
        //
        // NOTE: Different from PCI we dont return the fragment here.
        //
        return;
    }

    //
    // This receiving DPC is done. Reduce the Active Receiver count.
    //
    MP_DECREMENT_ACTIVE_RECEIVERS(pAdapter);
}



/**
 * This function is called during an InterruptHandle DPC to check if a
 * fragment has been received by the miniport that can be indicated up
 * to the OS.
 *
 * \param pAdapter      The adapter context for the miniport
 * \sa MPHandleInterrupt
 */
VOID
MpHandleDefaultReceiveInterrupt(
    __in PADAPTER         pAdapter,
    PNIC_RX_FRAGMENT    NicFragment,
    __in size_t           Size
    )
{
    ULONG               NumMSDUToIndicate = 0, NumLowResourcePackets = 0;
    ULONG               OldPendingRecvCount, i, NumActiveReceivers;
    USHORT              usSequenceNumber, usFragmentSize, usFragmentOffset;
    UCHAR               ucFragNumber;
    PMP_RX_MSDU         pMpRxd = NULL;
    //PNIC                pNic = pAdapter->Nic;
    PNIC_RX_FRAGMENT    pNicFragment;
    PMP_RX_FRAGMENT     pMpFragment;
    PNET_BUFFER_LIST    ReceivedNBLChain = NULL, LastReceivedNBL = NULL;
    PNET_BUFFER_LIST    LowResourceNBLChain = NULL, LastLowResourceNBL = NULL;
    BOOLEAN             bGrowRxMSDUPool = FALSE;
    PDOT11_MAC_HEADER   pFragmentHdr;
    PMP_DOT11_MGMT_DATA_MAC_HEADER  pShortHdr;
    NDIS_STATUS         ndisStatus;

    UNREFERENCED_PARAMETER(Size);
    UNREFERENCED_PARAMETER(NicFragment);

    //
    // We have to make sure that the we can receive right now. If a Pause or reset
    // is in progress, we should not indicate packets up to the protocol anymore.
    // Since this function is not protected by a spinlock (for perf reasons), we have
    // to use Ref Counts to make sure that this DPC is not running if Pause or reset
    // etc. is running.
    //
    NumActiveReceivers = MP_INCREMENT_ACTIVE_RECEIVERS(pAdapter);
    
    if (NumActiveReceivers <= 1) {
        //
        // Either a pause or reset (MiniportReset or Dot11Reset) is in progress.
        // We will not process this receive. Reduce the active receiver count
        // as this DPC is now done.
        //
        MP_DECREMENT_ACTIVE_RECEIVERS(pAdapter);
        //
        // NOTE: Different from PCI we dont treturn the fragment here.
        //
        return;
    }    

    //
    // This receiving DPC is done. Reduce the Active Receiver count.
    //
    MP_DECREMENT_ACTIVE_RECEIVERS(pAdapter);
}


/**
 * This function is called during an InterruptHandle DPC in Safe Mode 
 * to check if a fragment has been received by the miniport that can be 
 * indicated up to the OS.
 * 
 * \param pAdapter      The adapter context for the miniport
 * \sa MPHandleInterrupt
 */
VOID
MpHandleSafeModeReceiveInterrupt(
    __in PADAPTER         pAdapter,
    PNIC_RX_FRAGMENT    NicFragment,
    __in size_t           Size
    )
{
    ULONG               NumMSDUToIndicate = 0, NumLowResourcePackets = 0;
    ULONG               OldPendingRecvCount, i, NumActiveReceivers;
    USHORT              usFragmentSize, usFragmentOffset, finalOffset;
    PMP_RX_MSDU         pMpRxd = NULL;
    //PNIC                pNic = pAdapter->Nic;
    PNIC_RX_FRAGMENT    pNicFragment;
    PMP_RX_FRAGMENT     pMpFragment;
    PNET_BUFFER_LIST    ReceivedNBLChain = NULL, LastReceivedNBL = NULL;
    PNET_BUFFER_LIST    LowResourceNBLChain = NULL, LastLowResourceNBL = NULL;
    BOOLEAN             bGrowRxMSDUPool = FALSE;
    PDOT11_MAC_HEADER   pFragmentHdr;
    NDIS_STATUS         ndisStatus;

    UNREFERENCED_PARAMETER(Size);
    UNREFERENCED_PARAMETER(NicFragment);
    
    //
    // We have to make sure that the we can receive right now. If a Pause or reset
    // is in progress, we should not indicate packets up to the protocol anymore.
    // Since this function is not protected by a spinlock (for perf reasons), we have
    // to use Ref Counts to make sure that this DPC is not running if Pause or reset
    // etc. is running.
    //
    NumActiveReceivers = MP_INCREMENT_ACTIVE_RECEIVERS(pAdapter);
    if (NumActiveReceivers <= 1)
    {
        //
        // Either a pause or reset (MiniportReset or Dot11Reset) is in progress.
        // We will not process this receive. Reduce the active receiver count
        // as this DPC is now done.
        //
        MP_DECREMENT_ACTIVE_RECEIVERS(pAdapter);
        return;
    }
    
    //
    // This receiving DPC is done. Reduce the Active Receiver count.
    //
    MP_DECREMENT_ACTIVE_RECEIVERS(pAdapter);
}


