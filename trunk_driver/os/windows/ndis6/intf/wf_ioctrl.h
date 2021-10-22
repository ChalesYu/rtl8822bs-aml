/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    mpcontrol.h

Abstract:

    Constants and types to access the NDISPROT driver.
    Users must also include ntddndis.h

Environment:

    User/Kernel mode.

Revision History:

    
--*/

#ifndef __WF_IOCTRL__
#define __WF_IOCTRL__

//
// Simple Mutual Exclusion constructs used in preference to
// using KeXXX calls since we don't have Mutex calls in NDIS.
// These can only be called at passive IRQL.
//

#define FSCTL_NDISMP_BASE      FILE_DEVICE_NETWORK

#define _NDISPROT_CTL_CODE(_Function, _Method, _Access)  \
            CTL_CODE(FSCTL_NDISMP_BASE, _Function, _Method, _Access)

#define IOCTL_NDISMP_GET_MAC_DATA   \
            _NDISPROT_CTL_CODE(0x200, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_SET_MAC_DATA   \
            _NDISPROT_CTL_CODE(0x201, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_GET_RF_DATA   \
            _NDISPROT_CTL_CODE(0x202, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_SET_RF_DATA   \
            _NDISPROT_CTL_CODE(0x203, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_GET_CCK_DATA   \
            _NDISPROT_CTL_CODE(0x204, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_SET_CCK_DATA   \
            _NDISPROT_CTL_CODE(0x205, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_GET_OFDM_DATA   \
            _NDISPROT_CTL_CODE(0x206, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_SET_OFDM_DATA   \
            _NDISPROT_CTL_CODE(0x207, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

                                              
    
//
//  Structure to go with IOCTL_NDISPROT_QUERY_OID_VALUE.
//  The Data part is of variable length, determined by
//  the input buffer length passed to DeviceIoControl.
//
typedef struct _NDISMP_GET_DATA
{
    ULONG           Size;
    ULONG           Address;
    UCHAR           Data[sizeof(ULONG)];

} NDISMP_GET_DATA, *PNDISMP_GET_DATA;





#define NT_DEVICE_NAME          L"\\Device\\NdisWifi"
#define DOS_DEVICE_NAME         L"\\DosDevices\\NdisWifi"


typedef struct _NIC_MUTEX
{
    ULONG                   Counter;
    ULONG                   ModuleAndLine;  // useful for debugging

} NIC_MUTEX, *PNIC_MUTEX;

#define NIC_INIT_MUTEX(_pMutex)                                 \
{                                                               \
    (_pMutex)->Counter = 0;                                     \
    (_pMutex)->ModuleAndLine = 0;                               \
}

#define NIC_ACQUIRE_MUTEX(_pMutex)                              \
{                                                               \
    while (NdisInterlockedIncrement((PLONG)&((_pMutex)->Counter)) != 1)\
    {                                                           \
        NdisInterlockedDecrement((PLONG)&((_pMutex)->Counter));        \
        NdisMSleep(10000);                                      \
    }                                                           \
    (_pMutex)->ModuleAndLine = ('I' << 16) | __LINE__;\
}

#define NIC_RELEASE_MUTEX(_pMutex)                              \
{                                                               \
    (_pMutex)->ModuleAndLine = 0;                               \
    NdisInterlockedDecrement((PLONG)&(_pMutex)->Counter);              \
}

typedef struct _CONTROL_DEVICE_EXTENSION {
    PADAPTER    Adapter;
}   CONTROL_DEVICE_EXTENSION, *PCONTROL_DEVICE_EXTENSION ;

NDIS_STATUS
NICRegisterDevice(
    NDIS_HANDLE NdisMpHandle ,
    PADAPTER AdapterParam
    );

VOID
NICDeregisterDevice(
    PADAPTER AdapterParam
    );

DRIVER_DISPATCH NICDispatch;
 
#endif // __NUIOUSER__H


