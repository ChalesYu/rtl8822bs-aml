/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-10-30     hichard      first version
 */
#include <rtthread.h>
#include <drivers/usb_host.h>
#include "hcdc.h"
#include "hcdc_tty.h"

#ifdef RT_USBH_CDC
static struct uclass_driver cdc_driver;
static rt_list_t cdc_protocal_list;

/**
 * This function will do get line code to the usb cdc device
 *
 * @param intf the interface instance.
 * @line_coding the line coding data.
 * 
 * @return the error code, RT_EOK on successfully.
*/
rt_err_t rt_usbh_cdc_get_line_code(cdc_intf_t *cdc_intf, cdc_line_coding_t *line_coding)
{
    struct uinstance   *device;    
    struct urequest     setup;
    int                 timeout = USB_TIMEOUT_BASIC;
    struct uhintf       *intf;
    
    RT_ASSERT(cdc_intf != RT_NULL);
    RT_DEBUG_LOG(RT_DEBUG_USB, ("rtusbh_cdc_get_line_conding\r\n"));

    intf = cdc_intf->user_data;
    device = intf->device;

    /* construct the request */
    setup.request_type = USB_REQ_TYPE_DIR_IN | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE;
    setup.bRequest     = CDC_GET_LINE_CODING;
    setup.wValue       = 0;
    setup.wIndex       = intf->intf_desc->bInterfaceNumber;
    setup.wLength      = sizeof(cdc_line_coding_t);
    
    /* do control transfer request */
    if(rt_usb_hcd_setup_xfer(device->hcd, device->pipe_ep0_out, &setup, timeout) != 8) {
        return -RT_EIO;
    }
    
    if(rt_usb_hcd_pipe_xfer(device->hcd, device->pipe_ep0_in, line_coding->buffer,
        sizeof(cdc_line_coding_t), timeout) != sizeof(cdc_line_coding_t)) {
        return -RT_EIO;
    }
    
    if(rt_usb_hcd_pipe_xfer(device->hcd, device->pipe_ep0_out, RT_NULL, 0, timeout) != 0) {
        return -RT_EIO;
    }
    
    return RT_EOK;
}

/**
 * This function will do set line code to the usb cdc device
 *
 * @param intf the interface instance.
 * @line_coding the line coding data.
 * 
 * @return the error code, RT_EOK on successfully.
*/
rt_err_t rt_usbh_cdc_set_line_code(cdc_intf_t *cdc_intf, cdc_line_coding_t *line_coding)
{
    struct uinstance   *device;    
    struct urequest     setup;
    int                 timeout = USB_TIMEOUT_BASIC;
    struct uhintf      *intf;
    
    RT_ASSERT(cdc_intf != RT_NULL);
    RT_DEBUG_LOG(RT_DEBUG_USB, ("rtusbh_cdc_set_line_code\r\n"));

    intf = cdc_intf->user_data;
    device = intf->device;
	
    /* construct the request */
    setup.request_type = USB_REQ_TYPE_DIR_OUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE;
    setup.bRequest     = CDC_SET_LINE_CODING;
    setup.wValue       = 0;
    setup.wIndex       = intf->intf_desc->bInterfaceNumber;
    setup.wLength      = sizeof(cdc_line_coding_t);
    
    /* do control transfer request */
    if(rt_usb_hcd_setup_xfer(device->hcd, device->pipe_ep0_out, &setup, timeout) != 8) {
        return -RT_EIO;
    }
    
    if(rt_usb_hcd_pipe_xfer(device->hcd, device->pipe_ep0_out, line_coding->buffer, 
        sizeof(cdc_line_coding_t), timeout) != sizeof(cdc_line_coding_t)) {
        return -RT_EIO;
    }
    
    if(rt_usb_hcd_pipe_xfer(device->hcd, device->pipe_ep0_in, RT_NULL, 0, timeout) != 0) {
        return -RT_EIO;
    }
    
    return RT_EOK;
}

/**
 * This function will do set the line state to the usb cdc device
 *
 * @param intf the interface instance.
 * @line_coding the line coding data.
 * 
 * @return the error code, RT_EOK on successfully.
*/
rt_err_t rt_usbh_cdc_set_control_line_state(cdc_intf_t *cdc_intf, uint32_t wValue)
{
    struct uinstance   *device;    
    struct urequest     setup;
    int                 timeout = USB_TIMEOUT_BASIC;
    struct uhintf      *intf;
    
    RT_ASSERT(cdc_intf != RT_NULL);
    RT_DEBUG_LOG(RT_DEBUG_USB, ("rtusbh_cdc_set_control_line_state\r\n"));

    intf = cdc_intf->user_data;
    device = intf->device;
	
    /* construct the request */
    setup.request_type = USB_REQ_TYPE_DIR_OUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE;
    setup.bRequest     = CDC_SET_CONTROL_LINE_STATE;
    setup.wValue       = wValue;
    setup.wIndex       = intf->intf_desc->bInterfaceNumber;
    setup.wLength      = 0;
    
    /* do control transfer request */
    if(rt_usb_hcd_setup_xfer(device->hcd, device->pipe_ep0_out, &setup, timeout) != 8) {
        return -RT_EIO;
    }
    
    if(rt_usb_hcd_pipe_xfer(device->hcd, device->pipe_ep0_in, RT_NULL, 0, timeout) != 0) {
        return -RT_EIO;
    }
    
    return RT_EOK;
}

/**
 * This function will do set the line state to the usb cdc device
 *
 * @param intf the interface instance.
 * @duration_in_ms the line break time.
 * 
 * @return the error code, RT_EOK on successfully.
*/
rt_err_t rt_usbh_cdc_set_break(cdc_intf_t *cdc_intf, uint16_t duration_in_ms)
{
    struct uinstance   *device;    
    struct urequest     setup;
    int                 timeout = USB_TIMEOUT_BASIC;
    struct uhintf      *intf;
    
    RT_ASSERT(cdc_intf != RT_NULL);
    RT_DEBUG_LOG(RT_DEBUG_USB, ("rtusbh_cdc_set_control_line_state\r\n"));

    intf = cdc_intf->user_data;
    device = intf->device;
	
    /* construct the request */
    setup.request_type = USB_REQ_TYPE_DIR_OUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE;
    setup.bRequest     = CDC_SEND_BREAK;
    setup.wValue       = duration_in_ms;
    setup.wIndex       = intf->intf_desc->bInterfaceNumber;
    setup.wLength      = 0;
    
    /* do control transfer request */
    if(rt_usb_hcd_setup_xfer(device->hcd, device->pipe_ep0_out, &setup, timeout) != 8) {
        return -RT_EIO;
    }
    if(rt_usb_hcd_pipe_xfer(device->hcd, device->pipe_ep0_out, RT_NULL, 0, timeout) != 0) {
      return -RT_EIO;
    }
    if(rt_usb_hcd_pipe_xfer(device->hcd, device->pipe_ep0_in, RT_NULL, 0, timeout) != 0) {
        return -RT_EIO;
    }
    
    return RT_EOK;
}

/**
 * This function will register specified cdc protocal to protocal list
 *
 * @param protocal the specified protocal.
 * 
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_usbh_cdc_interface_register(cdc_intf_t *cdc_intf)
{
    RT_ASSERT(cdc_intf != RT_NULL);

    if (cdc_intf == RT_NULL) return -RT_ERROR;

    /* insert class driver into driver list */
    rt_list_insert_after(&cdc_protocal_list, &(cdc_intf->list));
    
    return RT_EOK;    
}

/**
 * This function will find specified cdc protocal from protocal list
 *
 * @param protocal the protocal id.
 * 
 * @return the found protocal or RT_NULL if there is no this protocal.
 */
cdc_intf_t *rt_usbh_cdc_interface_find(int interface)
{
    struct rt_list_node   *node;
    cdc_intf_t            *cdc_intf;

    /* try to find protocal object */
    for (node = cdc_protocal_list.next; node != &cdc_protocal_list; node = node->next) {
        cdc_intf = (cdc_intf_t *)rt_list_entry(node, cdc_intf_t, list);
        if (cdc_intf->interface == interface) return cdc_intf;
    }

    /* not found */
    return RT_NULL;
}


/**
 * This function will run cdc class driver when usb device is detected and identified
 *  as a cdc class device, it will continue the enumulate process.
 *
 * @param arg the argument.
 * 
 * @return the error code, RT_EOK on successfully.
 */
static rt_err_t rt_usbh_cdc_enable(void* arg)
{
    rt_err_t            result = RT_EOK;
    cdc_intf_t         *cdc_intf;    
    int                 interface;
    struct uhintf      *intf    = (struct uhintf*)arg;
    
    /* parameter check */
    if(intf == RT_NULL) {
        rt_kprintf("the interface is not available\n");
        return -RT_EIO;
    }
    interface = intf->intf_desc->bInterfaceNumber;
    cdc_intf = rt_usbh_cdc_interface_find(interface);    
    if(cdc_intf == RT_NULL) {
        intf->user_data = RT_NULL;        
        return RT_EOK;
    }
    intf->user_data = cdc_intf;  
    rt_kprintf("find cdc interface %d\n", interface);

    for(int i = 0; i < intf->intf_desc->bNumEndpoints; i++) {    
        uep_desc_t       ep_desc;

        /* get endpoint descriptor */        
        rt_usbh_get_endpoint_descriptor(intf->intf_desc, i, &ep_desc);
        if(ep_desc == RT_NULL) {
            RT_DEBUG_LOG(RT_DEBUG_USB, ("rt_usbh_get_endpoint_descriptor error\r\n"));
            return -RT_ERROR;
        }
        if(USB_EP_ATTR(ep_desc->bmAttributes) != USB_EP_ATTR_BULK) {
            continue;
        }
        if (ep_desc->bEndpointAddress & USB_DIR_IN) {
            result = rt_usb_hcd_alloc_pipe(intf->device->hcd, &cdc_intf->pipe_in, intf->device, ep_desc); 
        } else {
            result = rt_usb_hcd_alloc_pipe(intf->device->hcd, &cdc_intf->pipe_out, intf->device, ep_desc); 
        }
        if(result != RT_EOK) {
            break;
        }
    }
    
#ifdef RT_USBH_CDC_TTY
    rt_ttyusb_register(cdc_intf, intf);
//    {
//      cdc_line_coding_t cdc_line_coding;
//      rt_usbh_cdc_get_line_code(cdc_intf, &cdc_line_coding);
//      rt_usbh_cdc_set_control_line_state(cdc_intf, 0);
//      rt_usbh_cdc_set_line_code(cdc_intf, &cdc_line_coding);
//      rt_usbh_cdc_get_line_code(cdc_intf, &cdc_line_coding);
//    }
#endif

    cdc_intf->status = (result == RT_EOK) ? RT_TRUE : RT_FALSE;

    return result;
}

/**
 * This function will be invoked when usb device plug out is detected and it would clean 
 * and release all hub class related resources.
 *
 * @param arg the argument.
 * 
 * @return the error code, RT_EOK on successfully.
 */
static rt_err_t rt_usbh_cdc_disable(void* arg)
{
    cdc_intf_t           *cdc_intf;    
    struct uhintf        *intf    = (struct uhintf*)arg;

    RT_ASSERT(intf != RT_NULL);
    RT_DEBUG_LOG(RT_DEBUG_USB, ("rt_usbh_cdc_disable\n"));

    cdc_intf = (cdc_intf_t *)intf->user_data;
    if(cdc_intf != RT_NULL) {
      cdc_intf->status = RT_FALSE;
#ifdef RT_USBH_CDC_TTY
      rt_ttyusb_unregister(cdc_intf);
#endif
      if(cdc_intf->pipe_in != RT_NULL) {
        rt_usb_hcd_free_pipe(intf->device->hcd,cdc_intf->pipe_in);
        cdc_intf->pipe_in = RT_NULL;
      }
      if(cdc_intf->pipe_out != RT_NULL) {
        rt_usb_hcd_free_pipe(intf->device->hcd,cdc_intf->pipe_out);
        cdc_intf->pipe_out = RT_NULL;
      }
    }

    return RT_EOK;
}

/**
 * This function will register cdc class driver to the usb class driver manager.
 * and it should be invoked in the usb system initialization.
 * 
 * @return the error code, RT_EOK on successfully.
 */
ucd_t rt_usbh_class_driver_cdc(void)
{
    rt_list_init(&cdc_protocal_list);
  
    cdc_driver.class_code = USB_CLASS_CDC_CODE;
    
    cdc_driver.enable = rt_usbh_cdc_enable;
    cdc_driver.disable = rt_usbh_cdc_disable;

    return &cdc_driver;
}

#endif
