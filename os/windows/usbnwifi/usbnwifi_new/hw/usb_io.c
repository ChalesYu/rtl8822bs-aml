#include "pcomp.h"
#include "common.h"
#include "wf_debug.h"
extern NTSTATUS
HwUsbSendPacketSync(
	PADAPTER    adapter,
	ULONG           BufferLength,
	PVOID           Buffer
);

INT32 wf_usb_io_read(PADAPTER pAdapter, UCHAR flag, UINT addr, PVOID Buffer, INT datalen)
{
	if (flag == WF_USB_CTL_MSG) {
		switch (datalen)
		{
		case 1 :
			*((PUCHAR)Buffer) = wf_usb_io_read8(pAdapter, addr);
			break;
		case 2 :
			*((PUSHORT)Buffer) = wf_usb_io_read16(pAdapter, addr);
			break;
		case 4 :
			*((PUINT)Buffer) = wf_usb_io_read32(pAdapter, addr);
			break;
		default:
			{
			ASSERT(KeGetCurrentIrql() <= PASSIVE_LEVEL);
			UCHAR temp_buffer4usb[USB_CONTROL_MSG_BF_LEN] = { 0 };
			HwReadMacSieRegister(pAdapter, 0x05, (USHORT)(addr & 0x0000ffff), datalen, temp_buffer4usb);
			memcpy(Buffer, temp_buffer4usb, datalen);
			}
			break;
		}
	}
	else if (flag == WF_USB_BLK_ASYNC) {
		LOG_E("Error! USB read type:%d is not supported yet.", flag);
		// TODO:                  2021/03/08
	}
	else if (flag == WF_USB_BLK_SYNC) {
		// TODO:                  2021/03/08
		LOG_E("Error! USB read type:%d is not supported yet.", flag);
	}
	else {
		LOG_E("Invalid USB read type. ----CODE:%d.", flag);
	}
	return 0;
}
INT32 wf_usb_io_write(PADAPTER pAdapter, UCHAR flag, UINT addr, PVOID Buffer, INT datalen)
{
	if (flag == WF_USB_CTL_MSG) {
		if (datalen > USB_CTRL_MAX_LENGTH) {
			LOG_E("Invalid USB control transmission write out length: %d. (Max value is 64.)", datalen);
			return -1;
		}
		switch (datalen)
		{
		case 1:
			wf_usb_io_write8(pAdapter, addr, *((PUCHAR)Buffer));
			break;
		case 2:
			wf_usb_io_write16(pAdapter, addr, *((PUSHORT)Buffer));
			break;
		case 4:
			wf_usb_io_write32(pAdapter, addr, *((PUINT)Buffer));
			break;
		default:
			{
			UCHAR temp_buffer4usb[USB_CTRL_MAX_LENGTH] = { 0 };
			memcpy(temp_buffer4usb, Buffer, USB_CTRL_MAX_LENGTH);
			if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
				HwUsbSendVendorControlPacketAsync(pAdapter, 0x05, (USHORT)(addr & 0x0000ffff), datalen, temp_buffer4usb, 0);

			}
			else {
				HwWriteMacSieRegister(pAdapter, 0x05, (USHORT)(addr & 0x0000ffff), datalen, temp_buffer4usb, 0);
			}
			}
			break;
		}
	}
	else if (flag == WF_USB_BLK_ASYNC) {
		switch (addr)
		{
		case BE_QUEUE_INX:
		case BK_QUEUE_INX:
		case VI_QUEUE_INX:
		case VO_QUEUE_INX:
		case CMD_QUEUE_INX:
			wf_usb_write_normal(pAdapter, datalen, Buffer);
			break;

			//case CMD_QUEUE_INX:
		case MGT_QUEUE_INX:
		case BCN_QUEUE_INX:
		case HIGH_QUEUE_INX:
			wf_usb_write_high(pAdapter, datalen, Buffer);
			break;

		default:
			wf_usb_write_normal(pAdapter, datalen, Buffer);
			break;
		}
	}
	else if (flag == WF_USB_BLK_SYNC) {
		// TODO:                  2021/03/08
		wf_usb_write_sync(pAdapter, datalen, Buffer);
	}
	else {
		LOG_E("Invalid USB write type. ----CODE:%d.", flag);
	}
	return 0;
}

NTSTATUS wf_usb_write_sync(PADAPTER pAdapter, ULONG bufferLength, PVOID Buffer)
{
	NTSTATUS ntStatus = NDIS_STATUS_SUCCESS;
	ntStatus = HwUsbSendPacketSync(pAdapter, bufferLength, Buffer);
	return ntStatus;
}

/** USB bulk transfer asynchronously using OutputPipeHigh.
*/
NTSTATUS wf_usb_write_high(PADAPTER pAdapter, ULONG bufferLength, PVOID Buffer)
{
	NTSTATUS ntStatus = NDIS_STATUS_SUCCESS;
	ntStatus = HwUsbSendPacketAsync(pAdapter, 3, bufferLength, Buffer);
    return ntStatus;
}

/** USB bulk transfer asynchronously using OutputPipeNormal.
*/
NTSTATUS wf_usb_write_normal(PADAPTER pAdapter, ULONG bufferLength, PVOID Buffer)
{
	NTSTATUS ntStatus = NDIS_STATUS_SUCCESS;
	ntStatus = HwUsbSendPacketAsync(pAdapter, 1, bufferLength, Buffer);
	return ntStatus;
}

UCHAR wf_usb_io_read8(PADAPTER pAdapter,ULONG offset)
{
    return HwPlatformIORead1Byte(pAdapter, offset);
}

USHORT wf_usb_io_read16(PADAPTER pAdapter,ULONG offset)
{
    return HwPlatformIORead2Byte(pAdapter, offset);
}

UINT32 wf_usb_io_read32(PADAPTER pAdapter,ULONG offset)
{
    return HwPlatformIORead4Byte(pAdapter, offset);
}

VOID wf_usb_io_write8(PADAPTER pAdapter,ULONG addr, UCHAR value)
{
    HwPlatformIOWrite1Byte(pAdapter , addr, value);
}

VOID wf_usb_io_write16(PADAPTER pAdapter,ULONG addr, USHORT value)
{
    HwPlatformIOWrite2Byte(pAdapter , addr, value);
}
VOID wf_usb_io_write32(PADAPTER pAdapter,ULONG addr, UINT value)
{
    HwPlatformIOWrite4Byte(pAdapter , addr, value);
}

