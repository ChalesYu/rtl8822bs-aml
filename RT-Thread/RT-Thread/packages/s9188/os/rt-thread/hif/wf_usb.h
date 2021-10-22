/*
 * wf_usb.h
 *
 * usb wifi card register and io operate function declare.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef _WF__USB_H__
#define _WF__USB_H__

#define RECV_BULK_IN_ADDR		0x80
#define RECV_INT_IN_ADDR		0x81


#define USB_REQUEST_TYPE_VENDOR_IN 	(0xC0)
#define USB_REQUEST_TYPE_VENDOR_OUT (0x40)

#define USB_REQUEST_GET_STATUS	 	(0)
#define USB_REQUEST_SET_ADDRESS 	(5)


#define USB_CONTROL_MSG_TIMEOUT		(400)
#define USB_CONTROL_MSG_BF_LEN		(254+16)

#define MAX_IN_EP	2
#define MAX_OUT_EP	4
#define MAX_PRE_ALLOC_URB	4

#define MAX_IN_EP_DEVELOP	1
#define MAX_OUT_EP_DEVELOP	2

#ifndef WF_MIN
#define WF_MIN(x,y)	( (x) < (y) ? x : y )
#endif

#ifndef BIT
#define BIT(x)	(1 << (x))
#endif

enum WF_USB_OPERATION_FLAG
{
	WF_USB_CTL_MSG  	= 0,
	WF_USB_BLK_ASYNC 	= 1,
	WF_USB_BLK_SYNC 	= 2,
    WF_USB_NET_PIP      = 3,
};
enum WF_USB_SPEED
{
    WF_USB_SPEED_UNKNOWN = 0,
    WF_USB_SPEED_1_1 = 1,
    WF_USB_SPEED_2 = 2,
    WF_USB_SPEED_3 = 3,
};

typedef enum WF_USB_STATUS
{
    STA_SUCCESS = 0,
    STA_FAIL=1,
} USB_INIT_STATUS;

struct urb_struct
{
    wf_u8 used;
//    struct urb * purb;
    wf_list_t list;
};

struct tx_ctrl
{
    wf_u8 free_urb_cnt;
    wf_u8 pipe_idx;
};

typedef struct hif_usb_management
{
    wf_u8  n_in_pipes;
    wf_u8  n_out_pipes;
    wf_u8  n_interfaces;
    wf_u8  n_endpoints;
} hif_usb_mngt;


int usb_init(void);
int usb_exit(void);

#endif











