/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-01     hichard      first version
 */

#ifndef __HCDC_TTY_H__
#define __HCDC_TTY_H__

#include <rtthread.h>

#if defined(RT_USING_USB_HOST) && defined(RT_USBH_CDC)
/* 
 * extern function
 */
void rt_ttyusb_register(cdc_intf_t *cdc, void *data);
void rt_ttyusb_unregister(cdc_intf_t *cdc);

#endif
#endif

