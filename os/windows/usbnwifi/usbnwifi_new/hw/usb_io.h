#ifndef __INC_USB_IO_H
#define __INC_USB_IO_H


enum WF_USB_OPERATION_FLAG
{
	WF_USB_CTL_MSG = 0,
	WF_USB_BLK_ASYNC = 1,
	WF_USB_BLK_SYNC = 2,
	WF_USB_NET_PIP = 3,
};

#define WF_USB_BLK_PIPE_NORMAL 1
#define WF_USB_BLK_PIPE_HIGH   2
#define USB_CTRL_MAX_LENGTH    64
#define USB_CONTROL_MSG_BF_LEN		(254+16)

INT32 wf_usb_io_read(PADAPTER pAdapter, UCHAR flag, UINT addr, PVOID Buffer, INT datalen);
INT32 wf_usb_io_write(PADAPTER pAdapter, UCHAR flag, UINT addr, PVOID Buffer, INT datalen);

int hif_write_firmware(PADAPTER pAdapter, UCHAR which, UCHAR *firmware, UINT len);
//int hif_write_cmd(PADAPTER pAdapter, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len);
/**
*    USB bulk out.
*/
NTSTATUS wf_usb_write_high(PADAPTER pAdapter, ULONG bufferLength, PVOID Buffer);
NTSTATUS wf_usb_write_normal(PADAPTER pAdapter, ULONG bufferLength, PVOID Buffer);
NTSTATUS wf_usb_write_sync(PADAPTER pAdapter, ULONG bufferLength, PVOID Buffer);

/**
*    USB control in and out.
*/

// TODO: Maybe we should change the return types in the future.  2021/03/03
UCHAR wf_usb_io_read8(PADAPTER pAdapter, ULONG offset);
USHORT wf_usb_io_read16(PADAPTER pAdapter, ULONG offset);
UINT32 wf_usb_io_read32(PADAPTER pAdapter, ULONG offset);

VOID wf_usb_io_write8(PADAPTER pAdapter,ULONG addr, UCHAR value);
VOID wf_usb_io_write16(PADAPTER pAdapter,ULONG addr, USHORT value);
VOID wf_usb_io_write32(PADAPTER pAdapter,ULONG addr, UINT value);


#endif
