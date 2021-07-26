#ifndef __USB_REG_H__
#define __USB_REG_H__

#define WF_USB_BLK_PIPE_NORMAL 1
#define WF_USB_BLK_PIPE_HIGH   2
#define USB_CTRL_MAX_LENGTH    64
#define USB_CONTROL_MSG_BF_LEN		(254+16)

enum WF_USB_OPERATION_FLAG
{
	WF_USB_CTL_MSG = 0,
	WF_USB_BLK_ASYNC = 1,
	WF_USB_BLK_SYNC = 2,
	WF_USB_NET_PIP = 3,
};


INT32 wf_usb_read(PADAPTER pAdapter, UCHAR flag, UINT addr, PVOID Buffer, INT datalen);
INT32 wf_usb_write(PADAPTER pAdapter, UCHAR flag, UINT addr, PVOID Buffer, INT datalen);

UCHAR wf_usb_read8(PADAPTER pAdapter, ULONG offset);
USHORT wf_usb_read16(PADAPTER pAdapter, ULONG offset);
UINT32 wf_usb_read32(PADAPTER pAdapter, ULONG offset);

VOID wf_usb_write8(PADAPTER pAdapter,ULONG addr, UCHAR value);
VOID wf_usb_write16(PADAPTER pAdapter,ULONG addr, USHORT value);
VOID wf_usb_write32(PADAPTER pAdapter,ULONG addr, UINT value);

#endif

