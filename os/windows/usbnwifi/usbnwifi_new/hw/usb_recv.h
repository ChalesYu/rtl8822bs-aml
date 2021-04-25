#ifndef __INC_RECEIVE_H
#define __INC_RECEIVE_H

#define MAX_RECEIVE_BUFFER_SIZE 32768

// This list is from the new linux driver.
// TODO: Complete the correspondence between the old 9083 rate list and the new rate list.

NDIS_STATUS
HwUsbRecvStop(
    PADAPTER adapter
    );
       
NDIS_STATUS
HwUsbRecvStart(
    PADAPTER adapter
    );

NDIS_STATUS
HwUsbAllocateRecvResources(
    __in PADAPTER adapter
    );

NDIS_STATUS
HwUsbFreeRecvResources(
    __in PADAPTER adapter
    );

VOID
HwUsbFreeRecvFragment(
                __in  PADAPTER adapter,
                __in  PNIC_RX_FRAGMENT NicFragment
                ) ;

VOID
HwUsbProcessReceivedPacket(
    PADAPTER adapter,
    PNIC_RX_FRAGMENT     Rfd
    );

#endif // #ifndef __INC_RECEIVE_H

