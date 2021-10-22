/*
 * usb.c
 *
 * used for .....
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include <linux/usb.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/skbuff.h>

#include "wf_os_api.h"
#include "wf_list.h"
#include "usb.h"
#include "hif.h"
#include "power.h"

#define USB_DBG(fmt, ...)       LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define USB_INFO(fmt, ...)      LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define USB_WARN(fmt, ...)      LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define USB_ERROR(fmt, ...)     LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)

#define WLAN_USB_VENDOR_ID  0x2310
#define WLAN_USB_PRODUCT_ID 0x9086
#define MAX_USBCTRL_VENDORREQ_TIMES 3
#define MAX_CONTINUAL_IO_ERR 4
#define N_BYTE_ALIGMENT(value, aligment) ((aligment == 1) ? (value) : (((value + aligment - 1) / aligment) * aligment))

static int wf_usb_read_port(hif_node_st *hif_node, wf_u32 addr, wf_u8 *rdata, wf_u32 rlen);
static int wf_usb_read(struct hif_node_ *node, unsigned char flag, unsigned int addr, char *data, int datalen);

static int usb_ctrl_read(struct usb_device *pusb_dev, char *ctlBuf, unsigned int addr, char *data, int datalen);
static int usb_ctrl_write(struct usb_device *pusb_dev, char *ctlBuf, unsigned int addr, char *data, int datalen);

static inline int endpoint_is_int_out(const struct
                                      usb_endpoint_descriptor *epd)
{
    return (usb_endpoint_is_int_out(epd));
}

static inline int endpoint_is_int_in(const struct
                                     usb_endpoint_descriptor *epd)
{
    return (usb_endpoint_is_bulk_out(epd));
}

static inline int endpoint_is_bulk_in(const struct
                                      usb_endpoint_descriptor *epd)
{
    return (usb_endpoint_is_bulk_in(epd));
}

static inline int endpoint_is_bulk_out(const struct
                                       usb_endpoint_descriptor *epd)
{
    return (usb_endpoint_is_bulk_out(epd));
}

static inline int endpoint_num(const struct usb_endpoint_descriptor *epd)
{
    return usb_endpoint_num(epd);
}

int wf_endpoint_init(struct usb_interface *pusb_intf, hif_usb_mngt *pusb_mngt)
{
    int status                                      = 0;
    struct usb_host_config *phost_cfg               = NULL;
    struct usb_config_descriptor *pcfg_desc         = NULL;
    struct usb_host_interface *phost_itface         = NULL;
    struct usb_interface_descriptor *pitface_desc   = NULL;
    struct usb_host_endpoint *phost_endpt           = NULL;
    struct usb_endpoint_descriptor *pendpt_desc     = NULL;
    struct usb_device *pusb_dev                     = NULL;
    wf_u8 i                                         = 0;

    pusb_mngt->pusb_intf = pusb_intf;
    pusb_mngt->pusb_dev = interface_to_usbdev(pusb_intf);

    pusb_mngt->n_in_pipes = 0;
    pusb_mngt->n_out_pipes = 0;
    pusb_dev = pusb_mngt->pusb_dev;

    phost_cfg = pusb_dev->actconfig;
    pcfg_desc = &phost_cfg->desc;

    phost_itface = &pusb_intf->altsetting[0];
    pitface_desc = &phost_itface->desc;

    pusb_mngt->n_interfaces = pcfg_desc->bNumInterfaces;
    pusb_mngt->intface_num = pitface_desc->bInterfaceNumber;
    pusb_mngt->n_endpoints = pitface_desc->bNumEndpoints;

    for (i = 0; i < pusb_mngt->n_endpoints; i++)
    {
        phost_endpt = &phost_itface->endpoint[i];
        if (phost_endpt)
        {
            pendpt_desc = &phost_endpt->desc;

            if (endpoint_is_bulk_in(pendpt_desc))
            {
                USB_DBG("EP_IN  = num[%d]", endpoint_num(pendpt_desc));

                pusb_mngt->in_endp_addr[pusb_mngt->n_in_pipes] = endpoint_num(pendpt_desc);
                pusb_mngt->i_bulk_pipe_sz[pusb_mngt->n_in_pipes] =  pendpt_desc->bLength;
                pusb_mngt->n_in_pipes++;
            }
            else if (endpoint_is_bulk_out(pendpt_desc))
            {
                USB_DBG("EP_OUT = num[%d]", endpoint_num(pendpt_desc));
                pusb_mngt->out_endp_addr[pusb_mngt->n_out_pipes] = endpoint_num(pendpt_desc);
                pusb_mngt->o_bulk_pipe_sz[pusb_mngt->n_out_pipes] = pendpt_desc->wMaxPacketSize;
                pusb_mngt->n_out_pipes++;
            }
        }
    }

    switch (pusb_dev->speed)
    {
        case USB_SPEED_LOW:
            USB_DBG("USB_SPEED_LOW\n");
            pusb_mngt->usb_speed = WF_USB_SPEED_1_1;
            break;
        case USB_SPEED_FULL:
            USB_DBG("USB_SPEED_FULL\n");
            pusb_mngt->usb_speed = WF_USB_SPEED_1_1;
            break;
        case USB_SPEED_HIGH:
            USB_DBG("USB_SPEED_HIGH\n");
            pusb_mngt->usb_speed = WF_USB_SPEED_2;
            break;
        default:
            USB_WARN("USB_SPEED_UNKNOWN(%x)\n", pusb_dev->speed);
            pusb_mngt->usb_speed = WF_USB_SPEED_UNKNOWN;
            break;
    }

    if (pusb_mngt->usb_speed == WF_USB_SPEED_UNKNOWN)
    {
        return -1;
    }

    usb_get_dev(pusb_dev);

    return status;
}

int wf_endpoint_deinit(struct usb_interface *pusb_intf)
{
    hif_node_st  *hif_node = usb_get_intfdata(pusb_intf);
    hif_usb_mngt *pusb_mngt = &hif_node->u.usb;

    usb_set_intfdata(pusb_intf, NULL);
    if (pusb_mngt)
    {
        if ((pusb_mngt->n_interfaces != 2 && pusb_mngt->n_interfaces != 3)
            || (pusb_mngt->intface_num == 1))
        {
            if (interface_to_usbdev(pusb_intf)->state != USB_STATE_NOTATTACHED)
            {
                LOG_W("usb attached..., try to reset usb device\n");
                usb_reset_device(interface_to_usbdev(pusb_intf));
            }
        }
    }

    usb_put_dev(interface_to_usbdev(pusb_intf));

    return 0;
}


int wf_usb_init(struct hif_node_  *hif_node)
{
    return 0;
}

int wf_usb_deinit(struct hif_node_  *hif_node)
{
    USB_DBG("usb_deinit");
    return 0;
}


static inline unsigned int wf_usb_ffaddr2pipe(hif_usb_mngt *usb, wf_u32 addr)
{
    unsigned int pipe   = 0;
    wf_u8 ep_num        = 0;

    if (addr == READ_QUEUE_INX)
    {
        return usb_rcvbulkpipe(usb->pusb_dev, usb->in_endp_addr[0]);
    }

    switch(addr)
    {
        case BE_QUEUE_INX:
        case BK_QUEUE_INX:
        case VI_QUEUE_INX:
        case VO_QUEUE_INX:
            ep_num = usb->out_endp_addr[1];
            break;
        case CMD_QUEUE_INX:  // hjy
#ifdef CONFIG_RICHV300
            /* rich 300*/
            ep_num = usb->out_endp_addr[3];
#else
            ep_num = usb->out_endp_addr[1];
#endif
            break;

        //case CMD_QUEUE_INX:
        case MGT_QUEUE_INX:
        case BCN_QUEUE_INX:
        case HIGH_QUEUE_INX:
            ep_num = usb->out_endp_addr[0];
            break;

        default:
            ep_num = usb->out_endp_addr[1];
            break;
    }

    pipe = usb_sndbulkpipe(usb->pusb_dev, ep_num);

    if ((addr != BE_QUEUE_INX) && (addr != MGT_QUEUE_INX))
    {
        //USB_DBG("USB write addr:%d  ep_num:%d", addr, ep_num);
    }

    return pipe;

}

static inline void usb_write_port_complete(struct urb *purb)
{
    int ret                         = 0;
    data_queue_node_st  * qnode     = (data_queue_node_st *)purb->context;
    hif_node_st   *hnode            = (hif_node_st*)qnode->hif_node;
    data_queue_mngt_st *dqm         = &hnode->trx_pipe;

    if (0 == purb->status) //usb work well
    {
        qnode->state = TX_STATE_COMPETE;
        if(qnode->tx_callback_func)
        {
            ret = qnode->tx_callback_func(qnode->tx_info, qnode->param);
        }

        //qnode->u.purb = NULL;
        wf_data_queue_insert(&dqm->free_tx_queue, qnode);
    }
    else
    {
        LOG_I("[%s]:usb write work bad, urb->status:%d", __func__, purb->status);
        switch(purb->status)
        {
            case -EINVAL:
                USB_INFO("[%s] EINVAL", __func__);;
                break;
            case -EPIPE:
                USB_INFO("[%s] EPIPE", __func__);;
                break;
            case -ENODEV:
                USB_INFO("[%s] ENODEV", __func__);;
                break;
            case -ESHUTDOWN:
                USB_INFO("[%s] ESHUTDOWN", __func__);;
                break;

            case -ENOENT:
                USB_INFO("[%s] ENOENT", __func__);
                break;
            case -EPROTO:
                USB_INFO("[%s] EPROTO", __func__);;
                break;
            case -EILSEQ:
                USB_INFO("[%s] EILSEQ", __func__);;
                break;
            case -ETIME:
                USB_INFO("[%s] ETIME", __func__);;
                break;
            case -ECOMM:
                USB_INFO("[%s] ECOMM", __func__);;
                break;
            case -EOVERFLOW:
                USB_INFO("[%s] EOVERFLOW", __func__);;
                break;
            case -EINPROGRESS:
                USB_INFO("ERROR: URB IS IN PROGRESS!");
                break;
            default:
                USB_INFO("[%s] default", __func__);;
                break;
        }
    }

    // if (purb)
    // {
    //     usb_free_urb(purb);
    // }

}

static inline void usb_read_port_complete(struct urb * purb)
{
    data_queue_node_st  * qnode     = (data_queue_node_st *)purb->context;
    hif_node_st   *hnode            = (hif_node_st*)qnode->hif_node;
    struct   sk_buff *skb           = NULL;
    int ret                         = -1;
    hif_usb_mngt * pusb_mngt        = &(hnode->u.usb);
    skb = (struct sk_buff *)qnode->buff;

    if (0 == purb->status) //usb work well
    {
#ifdef CONFIG_RICHV200
#define MIN_RXD_SIZE      16
#else
#define MIN_RXD_SIZE      24
#endif
        //USB_DBG("usb recv length is %d", purb->actual_length);
        if (purb->actual_length < MIN_RXD_SIZE)
        {
            skb_reset_tail_pointer(skb);
            skb->len = 0;

            wf_usb_read_port(hnode, READ_QUEUE_INX, (wf_u8*)qnode, WF_MAX_RECV_BUFF_LEN_USB);
        }
        else//this is normal way, the data has been read to qnode->buffer
        {
            pusb_mngt->blk_continue_io_error    = 0;
            qnode->real_size = purb->actual_length;
            skb_put(skb, purb->actual_length);
            WF_ASSERT(qnode->buff);

            if (hnode->nic_info[0] != NULL)
            {
                ret = wf_rx_data_len_check(hnode->nic_info[0], skb->data, skb->len);
            }
            else
            {
                ret = -1;
            }

            if (ret == -1)
            {
                if (skb)
                {
                    skb_reset_tail_pointer(skb);
                    skb->len = 0;
                    wf_usb_read_port(hnode, READ_QUEUE_INX, (wf_u8*)qnode, WF_MAX_RECV_BUFF_LEN_USB);
                }
            }
            else
            {
                if (wf_rx_data_type(skb->data) == WF_PKT_TYPE_FRAME)
                {
                    skb_queue_tail(&hnode->trx_pipe.rx_queue, skb);

                    if (skb_queue_len(&hnode->trx_pipe.rx_queue) <= 1)
                    {
                        wf_tasklet_hi_sched(&hnode->trx_pipe.recv_task);
                    }

                    qnode->buff = NULL;
                    wf_usb_read_port(hnode, READ_QUEUE_INX, (wf_u8*)qnode, WF_MAX_RECV_BUFF_LEN_USB);

                }
                else
                {
#ifdef CONFIG_RICHV200
                    if (wf_rx_cmd_check(skb->data, skb->len) == 0)
                    {
                        switch(wf_rx_data_type(skb->data))
                        {
                            case WF_PKT_TYPE_CMD:
                                wf_hif_bulk_cmd_post(hnode, skb->data, skb->len);
                                break;

                            case WF_PKT_TYPE_FW:
                                wf_hif_bulk_fw_post(hnode, skb->data, skb->len);
                                break;

                            case WF_PKT_TYPE_REG:
                                wf_hif_bulk_reg_post(hnode, skb->data, skb->len);
                                break;

                            default:
                                USB_WARN("recv rxd type error");
                                break;
                        }



                    }
#endif

                    if (skb)
                    {
                        skb_reset_tail_pointer(skb);
                        skb->len = 0;
                        wf_usb_read_port(hnode, READ_QUEUE_INX, (wf_u8*)qnode, WF_MAX_RECV_BUFF_LEN_USB);
                    }

                }
            }

        }
    }
    else//usb work bad
    {
        skb_queue_tail(&hnode->trx_pipe.free_rx_queue_skb, skb);
        wf_data_queue_insert(&hnode->trx_pipe.free_rx_queue, qnode);
        //LOG_I("[%s]:usb read work bad, urb->status:%d  node_id:%d", __func__, purb->status, qnode->node_id);
        switch(purb->status)
        {
            case -EINVAL:
            {
                USB_WARN("[%s] EINVAL", __func__);
            }
            case -EPIPE:
            {
                USB_WARN("[%s] EPIPE", __func__);
            }
            case -ENODEV:
            {
                USB_WARN("[%s] ENODEV", __func__);
            }
            case -ESHUTDOWN:
            {
                USB_WARN("[%s] ESHUTDOWN", __func__);
            }
            case -ENOENT:
            {
                USB_WARN("[%s] ENOENT", __func__);
                hnode->dev_removed = wf_true;
                break;
            }
            case -EPROTO:
            {
                USB_WARN("[%s] EPROTO", __func__);
            }
            case -EILSEQ:
            {
                USB_WARN("[%s] EILSEQ", __func__);
            }
            case -ETIME:
            {
                USB_WARN("[%s] ETIME", __func__);
            }
            case -ECOMM:
            {
                USB_WARN("[%s] ECOMM", __func__);
            }
            case -EOVERFLOW:
            {
                USB_WARN("[%s] EOVERFLOW", __func__);
                break;
            }
            case -EINPROGRESS:
            {
                USB_WARN("ERROR: URB IS IN PROGRESS!");
                break;
            }
            default:
            {
                USB_WARN("Unknown status:%d", purb->status);
                break;
            }
        }
        purb->status = 0;
    }


}

static int wf_usb_read_port(hif_node_st *hif_node, wf_u32 addr, wf_u8 *rdata, wf_u32 rlen)
{
    int ret                         = 0;
    data_queue_node_st * qnode      = (data_queue_node_st *)rdata;
    unsigned int pipe               = 0;
    struct usb_device *pusbd        = hif_node->u.usb.pusb_dev;
    struct urb * purb               = qnode->u.purb;
    struct sk_buff *pskb            = NULL;
    data_queue_mngt_st *trxq        = (data_queue_mngt_st *)&hif_node->trx_pipe;
    //LOG_D("qnode->node_id:%d", qnode->node_id);

    if ((hm_get_mod_removed() == wf_false) && (hif_node->dev_removed == wf_true))
    {
        return WF_RETURN_FAIL;
    }

    trxq->rx_queue_cnt++;
    qnode->state = TX_STATE_FLOW_CTL;
    if (qnode->buff == NULL)
    {
        qnode->buff = (wf_u8*)skb_dequeue(&hif_node->trx_pipe.free_rx_queue_skb);
    }

    if(NULL == qnode->buff)
    {
        if(hif_node->trx_pipe.alloc_cnt < HIF_MAX_ALLOC_CNT)
        {
            LOG_W("[%s] wf_alloc_skb again", __func__);
            hif_node->trx_pipe.alloc_cnt++;
            wf_hif_queue_alloc_skb(&hif_node->trx_pipe.free_rx_queue_skb, hif_node->hif_type);
        }
        else
        {
            LOG_W("[%s] wf_alloc_skb skip", __func__);
        }

        wf_data_queue_insert(&hif_node->trx_pipe.free_rx_queue, qnode);
        return WF_RETURN_OK;
    }

    pipe    = wf_usb_ffaddr2pipe(&hif_node->u.usb, addr);
    pskb = (struct sk_buff *)qnode->buff;
    usb_fill_bulk_urb(purb, pusbd, pipe, pskb->data,  WF_MAX_RECV_BUFF_LEN_USB, usb_read_port_complete, qnode);
    ret = usb_submit_urb(purb, GFP_ATOMIC);
    if (ret && ret != (-EPERM))
    {
        LOG_E("cannot submit rx in-token(ret = %d), urb_status = %d\n", ret, purb->status);
        if (pskb)
        {
            skb_reset_tail_pointer(pskb);
            pskb->len = 0;
            skb_queue_tail(&hif_node->trx_pipe.free_rx_queue_skb, pskb);

            qnode->buff = NULL;
            wf_data_queue_insert(&hif_node->trx_pipe.free_rx_queue, qnode);
        }

        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

static int wf_usb_write_port(hif_node_st *hif_node, wf_u32 addr, wf_u8 *sdata, wf_u32 slen)
{

    int ret                                 = 0;
    data_queue_node_st * data_queue_node    = (data_queue_node_st *)sdata;
    unsigned int pipe                       = 0;
    struct usb_device *pusbd                = hif_node->u.usb.pusb_dev;
    struct urb * purb                       = data_queue_node->u.purb;
    data_queue_mngt_st *trxq                = (data_queue_mngt_st *)&hif_node->trx_pipe;
    nic_info_st *nic_info                   = (nic_info_st *)hif_node->nic_info[0];

#if 0
    {
        wf_u16 icmp_seq = 0;
        static wf_u16 icmp_seq_recoder = 0;

        if (*(data_queue_node->buff + TXDESC_SIZE + 43) == 1)
        {
            icmp_seq = (*(data_queue_node->buff + TXDESC_SIZE + 60) << 8) | (*(data_queue_node->buff + TXDESC_SIZE + 61));

            if (icmp_seq != 0)
                icmp_seq_recoder = icmp_seq;

            if (icmp_seq_recoder == icmp_seq)
            {
                USB_INFO("[%s, %d] icmp_seq:%d", __func__, __LINE__, icmp_seq);
                icmp_seq_recoder++;
            }
            else
            {
                USB_WARN("[%s, %d] icmp_seq error:%d", __func__, __LINE__, icmp_seq);
            }

        }
    }
#endif

    trxq->tx_queue_cnt++;
    pipe = wf_usb_ffaddr2pipe(&hif_node->u.usb, addr);

    // purb = usb_alloc_urb(0, GFP_KERNEL);
    // data_queue_node->u.purb = purb;

    usb_fill_bulk_urb(purb, pusbd, pipe, data_queue_node->buff,  slen, usb_write_port_complete, data_queue_node);

    purb->transfer_flags |= URB_ZERO_PACKET;
    data_queue_node->state = TX_STATE_SENDING;
    ret = usb_submit_urb(purb, GFP_ATOMIC);

    if (!ret)
    {
        ret = WF_RETURN_OK;
    }
    else
    {
        switch(ret)
        {
            case -ENODEV:
                nic_info->is_driver_stopped = wf_true;
                break;
            default:
                break;
        }
    }
    if(ret != WF_RETURN_OK)
    {
        data_queue_node->tx_callback_func(data_queue_node->tx_info, data_queue_node->param);
        LOG_I("usb write port failed, free buf");
    }

    return ret;
}


static int usb_ctrl_write(struct usb_device *pusb_dev, char *ctlBuf, unsigned int addr, char *data, int datalen)
{
    wf_u8 brequest      = USB_REQUEST_SET_ADDRESS;
    wf_u8 brequesttype  = USB_REQUEST_TYPE_VENDOR_OUT;
    wf_u16 wvalue       = addr;
    wf_u16 windex       = 0;
    wf_u16 wlength      = datalen;
    wf_u16 timeout      = USB_CONTROL_MSG_TIMEOUT;
    int ret             = 0;
    wf_u32 pipe         = usb_sndctrlpipe(pusb_dev, 0);
    wf_u8 retryCnt      = 0;

    WF_ASSERT(data != NULL);
    WF_ASSERT(pusb_dev != NULL);

    if(datalen > USB_CONTROL_MSG_BF_LEN)
    {
        USB_WARN("datalen > USB_CONTROL_MSG_BF_LEN");
        return -1;
    }

    memset(ctlBuf, 0, datalen);
    memcpy(ctlBuf, data, datalen);

    for (retryCnt = 0; retryCnt < MAX_USBCTRL_VENDORREQ_TIMES; retryCnt++)
    {
        ret = usb_control_msg(pusb_dev, pipe, brequest, brequesttype, wvalue, windex, \
                              ctlBuf, wlength, timeout);
        if(ret == datalen)
        {
            return 0;
        }
        else
        {
            if (ret == (-ESHUTDOWN) || ret == -ENODEV)
            {
                LOG_E("usb_control_msg error. need ShutDown!!");
                return -1;
            }

            LOG_W("usb_control_msg: retry send %d times", retryCnt);
        }
    }

    return -1;
}


static int usb_ctrl_read(struct usb_device *pusb_dev, char *ctlBuf, unsigned int addr, char *data, int datalen)
{
    wf_u8 brequest      = USB_REQUEST_SET_ADDRESS;
    wf_u8 brequesttype  = USB_REQUEST_TYPE_VENDOR_IN;
    wf_u16 wvalue       = addr;
    wf_u16 windex       = 0;
    wf_u16 wlength      = datalen;
    wf_u16 timeout      = USB_CONTROL_MSG_TIMEOUT;
    int ret             = 0;
    wf_u32 pipe         = usb_rcvctrlpipe(pusb_dev, 0);
    int vendorreq_times = 0;

    WF_ASSERT(ctlBuf != NULL);
    WF_ASSERT(pusb_dev != NULL);
    WF_ASSERT(data != NULL);

    if(datalen > USB_CONTROL_MSG_BF_LEN)
    {
        USB_WARN("datalen > USB_CONTROL_MSG_BF_LEN");
        return -1;
    }

    wf_memset(ctlBuf, 0, datalen);
    while(++vendorreq_times <= MAX_USBCTRL_VENDORREQ_TIMES)
    {
        ret = usb_control_msg(pusb_dev, pipe, brequest, brequesttype, wvalue, windex, ctlBuf, wlength, timeout);
        if(ret == datalen)
        {
            wf_memcpy(data, ctlBuf, datalen);
            ret = 0;
            break;
        }
        else
        {
            USB_DBG("reg 0x%x, usb %s %u fail, status:%d value = 0x%x, vendorreq_times:%d\n", addr, "read", datalen, ret, *(wf_u32 *) data, vendorreq_times);
        }
    }

    return ret;
}


#define BULK_SYNC_TIMEOUT   1000
static int usb_bulk_write_sync(struct hif_node_ *node, unsigned int addr, char *data, int datalen)
{
    wf_u32 pipe                 = 0;
    int status                  = 0;
    int actual_len              = 0;
    hif_usb_mngt * pusb_mngt    = &(node->u.usb);
    struct usb_device *pusb_dev = pusb_mngt->pusb_dev;

    WF_ASSERT(pusb_mngt != NULL);
    WF_ASSERT(data != NULL);
    WF_ASSERT(pusb_dev != NULL);

    if(datalen == 0)
    {
        USB_WARN("wirte len = 0");
        return -1;
    }

//    LOG_D("before write :");
//    LOG_D("reg 60E8 == %x", hif_io_read32(node, 0x60E8, NULL));
//    LOG_D("reg 60fc == %x", hif_io_read32(node, 0x60fc, NULL));
//    LOG_D("reg 60f8 == %x", hif_io_read32(node, 0x60f8, NULL));
//    LOG_D("reg 60f4 == %x\n\n", hif_io_read32(node, 0x60f4, NULL));
    pipe = wf_usb_ffaddr2pipe(&node->u.usb, addr);
    status = usb_bulk_msg(pusb_dev, pipe, data, datalen, &actual_len, BULK_SYNC_TIMEOUT);

//    LOG_D("after write :");
//    LOG_D("reg 60E8 == %x", hif_io_read32(node, 0x60E8, NULL));
//    LOG_D("reg 60fc == %x", hif_io_read32(node, 0x60fc, NULL));
//    LOG_D("reg 60f8 == %x", hif_io_read32(node, 0x60f8, NULL));
//    LOG_D("reg 60f4 == %x\n\n", hif_io_read32(node, 0x60f4, NULL));
    if (status)
    {
        USB_INFO("-->usb_write_port error, errno no is %d", status);
        return -1;
    }

    return actual_len;
}

static int wf_usb_write(struct hif_node_ *node, unsigned char flag, unsigned int addr, char *data, int datalen)
{
    int ret                     = 0;
    hif_usb_mngt *pusb_mngt     = &(node->u.usb);
    wf_u8 *pchar                = pusb_mngt->ctrl_msg_buffer;
    struct usb_device *pusb_dev = pusb_mngt->pusb_dev;

    if(hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
    {
        return -1;
    }
    else
    {
        if(WF_USB_CTL_MSG == flag)
        {
            ret = usb_ctrl_write(pusb_dev, pchar, addr, data, datalen);
        }
        else if(WF_USB_BLK_SYNC == flag)
        {
            ret = usb_bulk_write_sync(node, addr, data, datalen);
        }
        else
        {
            ret = wf_usb_write_port(node, addr, data, datalen);
        }
    }
    return ret;
}

static int wf_usb_read(struct hif_node_ *node, unsigned char flag, unsigned int addr, char *data, int datalen)
{
    int ret                     = 0;
    hif_usb_mngt *pusb_mngt     = &(node->u.usb);
    wf_u8 *pchar                = pusb_mngt->ctrl_msg_buffer;
    struct usb_device * pusb_dev= pusb_mngt->pusb_dev;

    if(hm_get_mod_removed() == wf_false && node->dev_removed == wf_true)
    {
        return -1;
    }
    else
    {
        if(WF_USB_CTL_MSG == flag)
        {
            ret = usb_ctrl_read(pusb_dev, pchar, addr, data, datalen);
        }
        else
        {
            ret = wf_usb_read_port(node, addr, data, datalen);
        }
    }

    return ret;
}

static int wf_usb_show(struct hif_node_ *node)
{
    return 0;
}


static struct hif_node_ops  usb_node_ops=
{
    .hif_read                   = wf_usb_read,
    .hif_show                   = wf_usb_show,
    .hif_write                  = wf_usb_write,
    .hif_init                   = wf_usb_init,
    .hif_exit                   = wf_usb_deinit,
    .hif_tx_queue_insert        = wf_tx_queue_insert,
    .hif_tx_queue_empty         = wf_tx_queue_empty,
};

static int wf_usb_probe(struct usb_interface *pusb_intf, const struct usb_device_id *pdid)
{
    hif_node_st  *hif_node  = NULL;
    wf_u8 *pctrl_buffer     = NULL;
    hif_usb_mngt *pusb_mngt = NULL;
    //wf_u32 version          = 0;
    int ret                 = 0;

    LOG_D("************USB CONNECT*************");

    USB_DBG("[usb] match usb_device !!");

    pusb_mngt = wf_kzalloc(sizeof(hif_usb_mngt));
    if(NULL == pusb_mngt)
    {
        LOG_E("wf_kzalloc for usb_mngt failed");
        ret = -ENODEV;
        goto exit;
    }

    ret = wf_endpoint_init(pusb_intf, pusb_mngt);
    if (ret < 0)
    {
        USB_WARN("[usb] wf_endpoint_init error");
        ret = -ENODEV;
        goto exit;
    }

    pctrl_buffer = wf_kzalloc(USB_CONTROL_MSG_BF_LEN);
    if(NULL == pctrl_buffer)
    {
        USB_WARN("[usb] no memmory for usb hif transmit");
        ret = -ENODEV;
        goto exit;
    }

    /* create hif_node */
    hif_node_register(&hif_node, HIF_USB, &usb_node_ops);
    if (NULL == hif_node)
    {
        USB_WARN("[usb] hif_node_register for HIF_USB failed");
        ret = 0;
        goto exit;
    }

    /*add usb handle into hif_node */
    pusb_mngt->usb_id = hm_new_usb_id(NULL);
    pusb_mngt->ctrl_msg_buffer = pctrl_buffer;
    memcpy(&hif_node->u.usb, pusb_mngt, sizeof(hif_usb_mngt));

    /* save hif_node in usb_intf */
    usb_set_intfdata(pusb_intf, hif_node);

    /* insert dev on hif_node */
    if (hif_dev_insert(hif_node) < 0)
    {
        USB_WARN("[usb] hif dev insert error !!");
        ret = 0;
        goto exit;
    }

exit :
    if (pusb_mngt)
    {
        wf_kfree(pusb_mngt);
    }

    if (ret < 0)
    {
        if (pctrl_buffer)
        {
            wf_kfree(pctrl_buffer);
        }
    }

    return ret;
}

static void wf_usb_disconnect(struct usb_interface *pusb_intf)
{
    hif_node_st *hif_node   = usb_get_intfdata(pusb_intf);
    int ret                 = 0;

    LOG_D("************USB DISCONNECT*************");

    /* ndev unregister should been do first */
    ndev_unregister_all(hif_node->nic_info, hif_node->nic_number);

    hif_dev_removed(hif_node);

    if (NULL != hif_node)
    {
        ret = hm_del_usb_id(hif_node->u.usb.usb_id);
        if(ret)
        {
            USB_WARN("hm_del_usb_id [%d] failed", hif_node->u.usb.usb_id);
        }
        wf_kfree(hif_node->u.usb.ctrl_msg_buffer);
        wf_endpoint_deinit(pusb_intf);
        hif_node_unregister(hif_node);
    }
    else
    {
        USB_WARN("[usb] wf_usb_disconnect failed");
    }
}

static int wf_usb_suspend(struct usb_interface *pusb_intf, pm_message_t message)
{
    hif_node_st  *hif_node = usb_get_intfdata(pusb_intf);
    hif_usb_mngt *pusb_mngt = &hif_node->u.usb;
    nic_info_st *pnic_info  = NULL;
    pwr_info_st *ppwr_info  = NULL;
    hif_node_st *usb_node   = NULL;
    wf_u32 i = 0;

    USB_DBG();

    if (pusb_mngt == NULL)
    {
        USB_ERROR("null point");
        return WF_RETURN_FAIL;
    }

    usb_node = container_of(pusb_mngt, hif_node_st, u.usb);

    do
    {
        pnic_info = usb_node->nic_info[i];
        if(pnic_info == NULL)
        {
            i++;
            if(i == MAX_NIC)
            {
                USB_ERROR("nic_info is NULL");
                return WF_RETURN_FAIL;
            }
        }
        else
        {
            break;
        }
    }
    while(MAX_NIC - i);

    ppwr_info = pnic_info->pwr_info;
    if(ppwr_info == NULL)
    {
        USB_DBG("ppwr_info == NULL\n");
    }

    if (ppwr_info->bInSuspend == wf_true)
    {
        USB_WARN("bInSuspend = %d, in suspend!!!", ppwr_info->bInSuspend);
        return 0;
    }

    ppwr_info->bInSuspend = wf_true;

    if(nic_suspend(pnic_info) < 0)
    {
        USB_ERROR("nic_susprnd fail !!!");
        return WF_RETURN_FAIL;
    }

    if(power_suspend(hif_node) < 0)
    {
        USB_ERROR("power_suspend fail !!!");
        return WF_RETURN_FAIL;
    }

    return 0;
}

static int wf_usb_resume(struct usb_interface *pusb_intf)
{
    hif_node_st  *hif_node = usb_get_intfdata(pusb_intf);
    hif_usb_mngt *pusb_mngt = &hif_node->u.usb;
    nic_info_st *pnic_info  = NULL;
    pwr_info_st *ppwr_info  = NULL;
    hif_node_st *usb_node   = NULL;
    wf_u32 i = 0;

    USB_DBG();

    if (pusb_mngt == NULL)
    {
        USB_ERROR("null point");
        return WF_RETURN_FAIL;
    }

    usb_node = container_of(pusb_mngt, hif_node_st, u.usb);
    if(usb_node == NULL)
    {
        return WF_RETURN_FAIL;
    }

    do
    {
        pnic_info = usb_node->nic_info[i];
        if(pnic_info == NULL)
        {
            i++;
            if(i == MAX_NIC)
            {
                USB_ERROR("nic_info is NULL");
                return WF_RETURN_FAIL;
            }
        }
        else
        {
            USB_DBG("i == %d\n", i);
            break;
        }
    }
    while(MAX_NIC - i);

    ppwr_info = pnic_info->pwr_info;
    if(ppwr_info->bInSuspend == wf_false)
    {
        LOG_E("bInSuspend = %d", ppwr_info->bInSuspend);
        return WF_RETURN_FAIL;
    }

    ppwr_info->bInSuspend = wf_false;

    if(power_resume(hif_node))
    {
        USB_ERROR("power_resume false !!");
        return WF_RETURN_FAIL;
    }

    if(nic_resume(pnic_info) < 0)
    {
        USB_ERROR("===>nic_resume error");
        return WF_RETURN_FAIL;
    }

    return 0;
}

static void wf_usb_shutdown(struct device *dev)
{
    struct usb_interface *usb_intf = container_of(dev, struct usb_interface, dev);
    hif_node_st *hif_node   = usb_get_intfdata(usb_intf);

    USB_DBG("usb_shutdown");

    power_off(hif_node);
}

static struct usb_device_id wf_usb_id_tbl[] =
{

    {
        USB_DEVICE_AND_INTERFACE_INFO(WLAN_USB_VENDOR_ID, WLAN_USB_PRODUCT_ID, 0xff, 0xff, 0xff), .
        driver_info = 1
    },
    {}
};

MODULE_DEVICE_TABLE(usb, wf_usb_id_tbl);


static struct usb_driver wf_usb_driver =
{
    .name           =   "s9083h",       /* driver name */
    .id_table       =   wf_usb_id_tbl,
    .probe          =   wf_usb_probe,
    .disconnect     =   wf_usb_disconnect,
    .suspend        =   wf_usb_suspend,
    .resume         =   wf_usb_resume,
    .reset_resume   =   wf_usb_resume,
    .drvwrap.driver.shutdown    = wf_usb_shutdown,
    .supports_autosuspend       = 1,
};

int usb_init(void)
{
    int ret = 0;
    USB_DBG("[usb] usb_init!!\n");
    ret = usb_register(&wf_usb_driver);

    if(ret != 0)
    {
        USB_INFO("usb_register failed");
    }
    return ret;
}

int usb_exit(void)
{
    int ret = 0;
    USB_DBG("[usb] usb_exit!!\n");
    usb_deregister(&wf_usb_driver);
    return ret;
}
