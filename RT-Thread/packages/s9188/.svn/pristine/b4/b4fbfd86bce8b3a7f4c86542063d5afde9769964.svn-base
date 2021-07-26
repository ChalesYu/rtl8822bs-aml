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

#if 1
#include "pcomp.h"
#include "wf_debug.h"


#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

ULONG
RF_ReadReg(
    __in  UCHAR      offset
    );

#pragma NDIS_PAGEABLE_FUNCTION(NICRegisterDevice)
#pragma NDIS_PAGEABLE_FUNCTION(NICDeregisterDevice)
#pragma NDIS_PAGEABLE_FUNCTION(NICDispatch)

#define CONTROL_DEVICE_OBJECT_NAME_LENGTH 64

NDIS_STATUS
NICRegisterDevice(
    NDIS_HANDLE NdisMpHandle ,
    PADAPTER AdapterParam
    )
/*++

Routine Description:

    Register an ioctl interface - a device object to be used for this
    purpose is created by NDIS when we call NdisMRegisterDevice.

    This routine is called whenever a new miniport instance is
    initialized. 

    NOTE: do not call this from DriverEntry; it will prevent the driver
    from being unloaded (e.g. on uninstall).

Arguments:

    None

Return Value:

    NDIS_STATUS_SUCCESS if we successfully register a device object.

--*/
{
    NDIS_STATUS         Status = NDIS_STATUS_SUCCESS;
    PDRIVER_DISPATCH    DispatchTable[IRP_MJ_MAXIMUM_FUNCTION+1];
    NDIS_DEVICE_OBJECT_ATTRIBUTES  DeviceObjectAttributes;
    PCONTROL_DEVICE_EXTENSION   controlDeviceObjectExtension;
    static UCHAR               i = 0;
    DECLARE_UNICODE_STRING_SIZE(DeviceName, CONTROL_DEVICE_OBJECT_NAME_LENGTH);
    DECLARE_UNICODE_STRING_SIZE(DeviceLinkUnicodeString, CONTROL_DEVICE_OBJECT_NAME_LENGTH);

    PAGED_CODE();
    
    MpEntry;
   
    NdisZeroMemory(DispatchTable, (IRP_MJ_MAXIMUM_FUNCTION+1) * sizeof(PDRIVER_DISPATCH));       
    DispatchTable[IRP_MJ_CREATE] = NICDispatch;
    DispatchTable[IRP_MJ_CLEANUP] = NICDispatch;
    DispatchTable[IRP_MJ_CLOSE] = NICDispatch;
    DispatchTable[IRP_MJ_DEVICE_CONTROL] = NICDispatch;
    NdisZeroMemory(&DeviceObjectAttributes, sizeof(NDIS_DEVICE_OBJECT_ATTRIBUTES));

    DeviceObjectAttributes.Header.Type = NDIS_OBJECT_TYPE_DEFAULT; // type implicit from the context
    DeviceObjectAttributes.Header.Revision = NDIS_DEVICE_OBJECT_ATTRIBUTES_REVISION_1;
    DeviceObjectAttributes.Header.Size = sizeof(NDIS_DEVICE_OBJECT_ATTRIBUTES);
    DeviceObjectAttributes.MajorFunctions = &DispatchTable[0];
    DeviceObjectAttributes.ExtensionSize = sizeof(CONTROL_DEVICE_EXTENSION);
    DeviceObjectAttributes.DefaultSDDLString = NULL;
    DeviceObjectAttributes.DeviceClassGuid = 0;

    //
    // Repeatedly try to create a named device object until we run out of buffer
    // space or we succeed.
    //
    do {
        Status = RtlUnicodeStringPrintf(&DeviceName, L"%s%d", NT_DEVICE_NAME, i);
        
        if (!NT_SUCCESS(Status)) {
            return Status;
        }
        
        Status = RtlUnicodeStringPrintf(&DeviceLinkUnicodeString, L"%s%d", DOS_DEVICE_NAME, i++);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        DeviceObjectAttributes.DeviceName = &DeviceName;
        DeviceObjectAttributes.SymbolicName = &DeviceLinkUnicodeString;

        //
        // Create a device with no device extension
        //
         Status = NdisRegisterDeviceEx(
                NdisMpHandle, 
                &DeviceObjectAttributes,
                &AdapterParam->ControlDeviceObject,
                &AdapterParam->NdisControlDeviceHandle
                );
    } while (NDIS_STATUS_NOT_SUPPORTED == Status && (i < 20) );
   
    
    if (Status == NDIS_STATUS_SUCCESS) {
        controlDeviceObjectExtension = NdisGetDeviceReservedExtension(AdapterParam->ControlDeviceObject);
        controlDeviceObjectExtension->Adapter  = AdapterParam;
    }
            

    MpExit;
    return (Status);
}


NTSTATUS
NICDispatch(
    __in PDEVICE_OBJECT           DeviceObject,
    __in PIRP                     Irp
    )
/*++
Routine Description:

    Process IRPs sent to this device.

Arguments:

    DeviceObject - pointer to a device object
    Irp      - pointer to an I/O Request Packet

Return Value:

    NTSTATUS - STATUS_SUCCESS always - change this when adding
    real code to handle ioctls.

--*/
{
    PIO_STACK_LOCATION  irpStack;
    NTSTATUS            status = STATUS_SUCCESS;
    ULONG               inlen;
    PNDISMP_GET_DATA    data;
    ULONG               info;
    PCONTROL_DEVICE_EXTENSION   controlDeviceObjectExtension;    
    PADAPTER            pAdapter;

    PAGED_CODE();
    
    controlDeviceObjectExtension = NdisGetDeviceReservedExtension(DeviceObject);
    pAdapter =  controlDeviceObjectExtension->Adapter;
    
    info = 0;
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    LOG_D("==>NICDispatch %d", irpStack->MajorFunction);

    switch (irpStack->MajorFunction)
    {
        case IRP_MJ_CREATE:
            break;
        
        case IRP_MJ_CLEANUP:
            break;
        
        case IRP_MJ_CLOSE:
            break;        
        
        case IRP_MJ_DEVICE_CONTROL: 
        {

          data = (PNDISMP_GET_DATA)Irp->AssociatedIrp.SystemBuffer;  
          inlen = irpStack->Parameters.DeviceIoControl.InputBufferLength;
          
          switch (irpStack->Parameters.DeviceIoControl.IoControlCode) 
          {

            //
            // Add code here to handle ioctl commands.
            //
            case IOCTL_NDISMP_GET_MAC_DATA:
                LOG_D("Received  IOCTL input buffer len %x",inlen);

                switch (data->Size) {
                    case 1:
                    data->Data[0] = wf_usb_read8(pAdapter,data->Address);
                    break;
    
                    case 2:
                    (*(PUSHORT)(&data->Data)) = wf_usb_read16(pAdapter,data->Address);
                    break;
    
                    case 4:
                    (*(PULONG)(&data->Data)) = wf_usb_read32(pAdapter, data->Address);
                    LOG_D("return value is  %x",(*(PULONG)(&data->Data)));

                    break;
    
                    default:
                        ASSERT(FALSE);

                }
                info = sizeof(NDISMP_GET_DATA);
                break;

              case IOCTL_NDISMP_SET_MAC_DATA:
                  LOG_D("Received  IOCTL input buffer len %x",inlen);

                  switch (data->Size) {
                      case 1:
                      wf_usb_write8(pAdapter,data->Address,data->Data[0] );
                      break;

                      case 2:
                      wf_usb_write16(pAdapter,data->Address, (*(PUSHORT)(&data->Data)));
                      LOG_D("write value is  %x",(*(PUSHORT)(&data->Data)));
                      break;

                      case 4:
                      wf_usb_write32(pAdapter, data->Address,(*(PULONG)(&data->Data)) );
                      LOG_D("write value is  %x",(*(PULONG)(&data->Data)));

                      break;

                      default:
                          ASSERT(FALSE);

                  }
                  info = sizeof(NDISMP_GET_DATA);
                  break;
                  
              case IOCTL_NDISMP_GET_RF_DATA:
                  LOG_D("Received  IOCTL input buffer len %x",inlen);
                  //(*(PULONG)(&data->Data)) = RF_ReadReg(pAdapter, (UCHAR)data->Address);                   
                  info = sizeof(NDISMP_GET_DATA);
                  break;

              case IOCTL_NDISMP_SET_RF_DATA:
                  LOG_D("Received  IOCTL input buffer len %x",inlen);
                  break;
                  
              case IOCTL_NDISMP_GET_CCK_DATA:
                  LOG_D("Received  IOCTL input buffer len %x",inlen);
                  wf_usb_write32(pAdapter, 0xff7c, (0x01000000 | data->Address));
                  data->Data[0]  = wf_usb_read8(pAdapter, 0xff7e);
                  info = sizeof(NDISMP_GET_DATA);
                  break;

              case IOCTL_NDISMP_SET_CCK_DATA:
                  LOG_D("Received  IOCTL input buffer len %x",inlen);
                  break;
                  
              case IOCTL_NDISMP_GET_OFDM_DATA:
                  LOG_D("Received  IOCTL input buffer len %x",inlen);
                  wf_usb_write32(pAdapter, 0xff7c, (0x00000000 | data->Address));
                  data->Data[0]  = wf_usb_read8(pAdapter, 0xff7e);
                  info = sizeof(NDISMP_GET_DATA);
                  break;

              case IOCTL_NDISMP_SET_OFDM_DATA:
                  LOG_D("Received  IOCTL input buffer len %x",inlen);
                  break;

            default:
                status = STATUS_UNSUCCESSFUL;
                break;
          }
          break;  
        }
        default:
            break;
    }
    Irp->IoStatus.Information = info;
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    MpExit;
    return status;
} 


VOID
NICDeregisterDevice(
    PADAPTER AdpaterParam
    )
/*++

Routine Description:

    Deregister the ioctl interface. This is called whenever a miniport
    instance is halted. When the last miniport instance is halted, we
    request NDIS to delete the device object

Arguments:

    NdisDeviceHandle - Handle returned by NdisMRegisterDevice

Return Value:

    NDIS_STATUS_SUCCESS if everything worked ok

--*/
{
    PAGED_CODE();
    MpEntry;

    //
    // All miniport instances have been halted.
    // Deregister the control device.
    //

    if (AdpaterParam->NdisControlDeviceHandle != NULL) {
        NdisDeregisterDeviceEx(AdpaterParam->NdisControlDeviceHandle);
        AdpaterParam->NdisControlDeviceHandle = NULL;
    }
   
    MpExit;
    return ;
    
}

#endif


