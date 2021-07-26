#ifndef __USB_H__
#define __USB_H__

#define RECV_BULK_IN_ADDR		0x80
#define RECV_INT_IN_ADDR		0x81


/* usb control transfer request type */
#define USB_REQUEST_TYPE_VENDOR_IN 	(0xC0)
#define USB_REQUEST_TYPE_VENDOR_OUT (0x40)

/* usb control transfer request */
#define USB_REQUEST_GET_STATUS	 	(0)
#define USB_REQUEST_SET_ADDRESS 	(5)


#define USB_CONTROL_MSG_TIMEOUT		(400)
#define USB_CONTROL_MSG_BF_LEN		(254+16)

#define MAX_IN_EP	2
#define MAX_OUT_EP	4
#define MAX_PRE_ALLOC_URB	4

/* develop period */
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
    struct urb * purb;
    wf_list_t list;
};

/*
xmit related
*/
struct tx_ctrl
{
    spinlock_t lock;
    struct urb_struct   urb_buf[MAX_PRE_ALLOC_URB];
    wf_list_t    free_urb_queue;
    wf_u8 free_urb_cnt;
    wf_u8 pipe_idx;
};

typedef struct hif_usb_management
{
    struct usb_interface *pusb_intf;
    struct usb_device *pusb_dev;
    wf_u8  n_in_pipes;
    wf_u8  n_out_pipes;
    wf_u8  n_interfaces;
    wf_u8  n_endpoints;

    wf_u8  intface_num;
    wf_u8  chip_type;

    wf_u8  in_endp_addr[MAX_IN_EP];  /* in endpoint address */
    wf_u8  out_endp_addr[MAX_OUT_EP]; /* out endpoint address */

    wf_u32 i_int_pipe;

    wf_u32 i_bulk_pipe[MAX_IN_EP];
    wf_u16 i_bulk_pipe_sz[MAX_IN_EP];

    wf_u32 i_ctrl_pipe;
    wf_u32 o_ctrl_pipe;

    wf_u32 o_bulk_pipe[MAX_OUT_EP];
    wf_u16 o_bulk_pipe_sz[MAX_OUT_EP];

    wf_u16 ep_num[MAX_IN_EP + MAX_OUT_EP];

    enum WF_USB_SPEED usb_speed;
    wf_u8 usb_id;
    wf_u8 *ctrl_msg_buffer;    //[USB_CONTROL_MSG_BF_LEN];
    wf_u8 ctl_continue_io_error;
    wf_u8 blk_continue_io_error;
} hif_usb_mngt;


int usb_init(void);
int usb_exit(void);

#endif











