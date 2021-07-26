/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2018-11-01     hichard      first version
*/
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "lwip/def.h"
#include "hcdc.h"

#ifdef RT_USBH_CDC_TTY

/**
* This function realize the device driver init
*
* @param dev the device driver struct.
* 
* @return the error code, RT_EOK on successfully.
*/
static rt_err_t _rt_ttyusb_init(rt_device_t dev)
{
  return RT_EOK;
}

/**
* This function realize the device driver open
*
* @param dev the device driver struct.
* @param oflag open flag, it is not valid here. 
* 
* @return the error code, RT_EOK on successfully.
*/
static rt_err_t _rt_ttyusb_open(rt_device_t dev, rt_uint16_t oflag)
{
  cdc_intf_t *cdc = (cdc_intf_t *)dev->user_data;
  
  if(cdc->status == RT_FALSE) {
    rt_set_errno(-ENXIO);
    return 0;
  }
  
//  rt_usbh_cdc_set_control_line_state(cdc, 0);
  return RT_EOK;
}

/**
* This function realize the device driver close
*
* @param dev the device driver struct. 
* 
* @return the error code, RT_EOK on successfully.
*/
static rt_err_t _rt_ttyusb_close(rt_device_t dev)
{
  return RT_EOK;
}

/**
* This function realize the device driver read
*
* @param dev the device driver struct. 
* 
* @return the error code, RT_EOK on successfully.
*/
static rt_size_t _rt_ttyusb_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
  rt_size_t temp;
  int len;
  cdc_intf_t *cdc = (cdc_intf_t *)dev->user_data;
  
  RT_ASSERT(dev != RT_NULL);
  
  if(cdc->status == RT_FALSE) {
    rt_set_errno(-ENXIO);
    return 0;
  }
  
//  /* read size must be multiple usb max packaet size */
//  temp = size % (cdc->pipe_in->ep.wMaxPacketSize);
//  if(temp != 0) {
//    size = size - temp;
//  } 
  
  if(size == 0) {
    return 0;
  }
  rt_mutex_take(&cdc->tty_in_lock, RT_WAITING_FOREVER);
  len = rt_usb_hcd_pipe_xfer(cdc->pipe_in->inst->hcd, cdc->pipe_in, buffer, size, cdc->timeout);
  rt_mutex_release(&cdc->tty_in_lock);
  
  if(len < 0) {
    len = 0;
  }
  return len;
}

/**
* This function realize the device driver write
*
* @param dev the device driver struct. 
* 
* @return the error code, RT_EOK on successfully.
*/
static rt_size_t _rt_ttyusb_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
  int len;
  cdc_intf_t *cdc = (cdc_intf_t *)dev->user_data;
  
  RT_ASSERT(dev != RT_NULL);
  
  if(cdc->status == RT_FALSE) {
    rt_set_errno(-ENXIO);
    return 0;
  }
  
  rt_mutex_take(&cdc->tty_out_lock, RT_WAITING_FOREVER);
  len = rt_usb_hcd_pipe_xfer(cdc->pipe_out->inst->hcd, cdc->pipe_out, (void *)buffer, size, cdc->timeout);
  rt_mutex_release(&cdc->tty_out_lock);
  
  if(len < 0) {
    len = 0;
  }
  return len; 
}

/**
* This function realize the device driver control
*
* @param dev the device driver struct. 
* 
* @return the error code, RT_EOK on successfully.
*/

static rt_err_t _rt_ttyusb_control (rt_device_t dev, int cmd, void *args)
{
  rt_err_t err_code = RT_EOK;
  rt_base_t level;
  cdc_intf_t *cdc = (cdc_intf_t *)dev->user_data;
  
  RT_ASSERT(dev != RT_NULL);
  
  switch (cmd)
  {
  case RT_DEVICE_CTRL_SUSPEND:
    {
      /* suspend device */
      dev->flag |= RT_DEVICE_FLAG_SUSPENDED;
      break;
    }
  case RT_DEVICE_CTRL_RESUME:
    {
      /* resume device */
      dev->flag &= ~RT_DEVICE_FLAG_SUSPENDED;
      break;
    }
  case RT_DEVICE_CTRL_CONFIG_SET:
    {
//      cdc_line_coding_t cdc_config;
//      struct serial_configure *pconfig = args;
//      if(args == RT_NULL) {
//        break;
//      }
//      cdc_config.line_coding.dwDTERate = PP_HTONL(pconfig->baud_rate);
//      cdc_config.line_coding.bCharFormat = pconfig->stop_bits;
//      cdc_config.line_coding.bParityType = pconfig->parity;
//      cdc_config.line_coding.bDataBits = pconfig->data_bits;
//      rt_usbh_cdc_set_line_code(cdc, &cdc_config);
      break;
    }
  case RT_DEVICE_CTRL_CONFIG_GET:
    {
//      cdc_line_coding_t cdc_config;
//      struct serial_configure *pconfig = args;
//      
//      if(args == RT_NULL) {
//        err_code = RT_ERROR;
//        break;
//      }
//      
//      rt_usbh_cdc_get_line_code(cdc, &cdc_config);
//      pconfig->baud_rate = PP_HTONL(cdc_config.line_coding.dwDTERate);
//      pconfig->parity = cdc_config.line_coding.bDataBits;
//      pconfig->baud_rate = cdc_config.line_coding.bParityType;
//      pconfig->stop_bits = cdc_config.line_coding.bCharFormat;
      break;
    }
  case RT_DEVICE_CRTL_TIMEOUT:
    {
      rt_uint32_t count = *((rt_uint32_t *)args);
      rt_uint32_t temp;
      
      if(args == RT_NULL) {
        err_code = RT_ERROR;
        break;
      }
      
      // 将ms转换为时钟tick
      if(count == 0) {
        cdc->timeout = RT_WAITING_FOREVER;
      } else {
        temp = 1000/RT_TICK_PER_SECOND;
        count = (count % temp)?((count / temp) + 1):(count / temp);
        /* disable interrupt */
        level = rt_hw_interrupt_disable();
        cdc->timeout = count;
        /* enable interrupt */
        rt_hw_interrupt_enable(level);
      }
      break;
    }
  case RT_DEVICE_CTRL_INTERVAL:
    {
      rt_uint32_t count = *((rt_uint32_t *)args);
      rt_uint32_t temp;
      
      if(args == RT_NULL) {
        err_code = RT_ERROR;
        break;
      }
      
      // 将ms转换为时钟tick
      if(count == 0) {
        cdc->interval = 10;
      } else {
        temp = 1000/RT_TICK_PER_SECOND;
        count = (count % temp)?((count / temp) + 1):(count / temp);
        /* disable interrupt */
        level = rt_hw_interrupt_disable();
        cdc->interval = count;
        /* enable interrupt */
        rt_hw_interrupt_enable(level);
      }
      break;
    }
  default:
    err_code = RT_ERROR;
    break;
  }
  
  return err_code;
}

/**
* This function realize the device driver register
*
* @param dev the device driver struct. 
* 
* @return none
*/
void rt_ttyusb_register(cdc_intf_t *cdc, void *data)
{
  char name[RT_NAME_MAX];
    
  cdc->tty_dev.type        = RT_Device_Class_Char;
  cdc->tty_dev.rx_indicate = RT_NULL;
  cdc->tty_dev.tx_complete = RT_NULL;
  cdc->tty_dev.init        = _rt_ttyusb_init;
  cdc->tty_dev.open        = _rt_ttyusb_open;
  cdc->tty_dev.close       = _rt_ttyusb_close;
  cdc->tty_dev.read        = _rt_ttyusb_read;
  cdc->tty_dev.write       = _rt_ttyusb_write;
  cdc->tty_dev.control     = _rt_ttyusb_control;
  cdc->tty_dev.user_data   = cdc;
  cdc->user_data           = data;
  
  /* register a character device */
  rt_snprintf(name, RT_NAME_MAX, "ttyUSB%d", cdc->tty_number);
  rt_device_register(&cdc->tty_dev, name, RT_DEVICE_FLAG_RDWR);
}

/**
* This function realize the device driver unregister
*
* @param dev the device driver struct. 
* 
* @return none
*/
void rt_ttyusb_unregister(cdc_intf_t *cdc)
{
  rt_device_unregister(&cdc->tty_dev);
}

/**
* This function realize the device driver init
*
* @param dev the device driver struct. 
* 
* @return the error code, RT_EOK on successfully.
*/
int rt_ttyusb_driver_init(void)
{
#ifdef RT_USE_TTYUSB0
  {
    static cdc_intf_t ttyusb0;
    
    ttyusb0.interface = 0;
    ttyusb0.tty_number = 0;
    ttyusb0.status = RT_FALSE;
    ttyusb0.timeout = 0;
    if(ttyusb0.timeout == 0) {
      ttyusb0.timeout = RT_WAITING_FOREVER;
    }
    if(ttyusb0.interval <= 0) {
      ttyusb0.interval = 10;
    }
    rt_mutex_init(&ttyusb0.tty_in_lock, "ttyin0", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&ttyusb0.tty_out_lock, "ttyout0", RT_IPC_FLAG_FIFO);
    
    rt_usbh_cdc_interface_register(&ttyusb0);
  }
#endif 
  
#ifdef RT_USE_TTYUSB1
  {
    static cdc_intf_t ttyusb1;
    
    ttyusb1.interface = 1;
    ttyusb1.tty_number = 1;
    ttyusb1.status = RT_FALSE;
    ttyusb1.timeout = 0;
    if(ttyusb1.timeout == 0) {
      ttyusb1.timeout = RT_WAITING_FOREVER;
    }
    if(ttyusb1.interval <= 0) {
      ttyusb1.interval = 10;
    }
    rt_mutex_init(&ttyusb1.tty_in_lock, "ttyin1", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&ttyusb1.tty_out_lock, "ttyout1", RT_IPC_FLAG_FIFO);
    
    rt_usbh_cdc_interface_register(&ttyusb1);
  }
#endif 
  
#ifdef RT_USE_TTYUSB2
  {
    static cdc_intf_t ttyusb2;
    
    ttyusb2.interface = 2;
    ttyusb2.tty_number = 2;
    ttyusb2.status = RT_FALSE;
    ttyusb2.timeout = 0;
    if(ttyusb2.timeout == 0) {
      ttyusb2.timeout = RT_WAITING_FOREVER;
    }
    if(ttyusb2.interval <= 0) {
      ttyusb2.interval = 10;
    }
    rt_mutex_init(&ttyusb2.tty_in_lock, "ttyin2", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&ttyusb2.tty_out_lock, "ttyout2", RT_IPC_FLAG_FIFO);
    
    rt_usbh_cdc_interface_register(&ttyusb2);
  }
#endif

#ifdef RT_USE_TTYUSB3
  {
    static cdc_intf_t ttyusb3;
    
    ttyusb3.interface = 3;
    ttyusb3.tty_number = 3;
    ttyusb3.status = RT_FALSE;
    ttyusb3.timeout = 0;
    if(ttyusb3.timeout == 0) {
      ttyusb3.timeout = RT_WAITING_FOREVER;
    }
    if(ttyusb3.interval <= 0) {
      ttyusb3.interval = 10;
    }
    rt_mutex_init(&ttyusb3.tty_in_lock, "ttyin3", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&ttyusb3.tty_out_lock, "ttyout3", RT_IPC_FLAG_FIFO);
    
    rt_usbh_cdc_interface_register(&ttyusb3);
  }
#endif
  
#ifdef RT_USE_TTYUSB4
  {
    static cdc_intf_t ttyusb4;
    
    ttyusb4.interface = 4;
    ttyusb4.tty_number = 4;
    ttyusb4.status = RT_FALSE;
    ttyusb4.timeout = 0;
    if(ttyusb4.timeout == 0) {
      ttyusb4.timeout = RT_WAITING_FOREVER;
    }
    if(ttyusb4.interval <= 0) {
      ttyusb4.interval = 10;
    }
    rt_mutex_init(&ttyusb4.tty_in_lock, "ttyin4", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&ttyusb4.tty_out_lock, "ttyout4", RT_IPC_FLAG_FIFO);
    
    rt_usbh_cdc_interface_register(&ttyusb4);
  }
#endif 
  
#ifdef RT_USE_TTYUSB5
  {
    static cdc_intf_t ttyusb5;
    
    ttyusb5.interface = 5;
    ttyusb5.tty_number = 5;
    ttyusb5.status = RT_FALSE;
    ttyusb5.timeout = 0;
    if(ttyusb5.timeout == 0) {
      ttyusb5.timeout = RT_WAITING_FOREVER;
    }
    if(ttyusb5.interval <= 0) {
      ttyusb5.interval = 10;
    }
    rt_mutex_init(&ttyusb5.tty_in_lock, "ttyin5", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&ttyusb5.tty_out_lock, "ttyout5", RT_IPC_FLAG_FIFO);
    
    rt_usbh_cdc_interface_register(&ttyusb5);
  }
#endif 
  
#ifdef RT_USE_TTYUSB6
  {
    static cdc_intf_t ttyusb6;
    
    ttyusb6.interface = 6;
    ttyusb6.tty_number = 6;
    ttyusb6.status = RT_FALSE;
    ttyusb6.timeout = 0;
    if(ttyusb6.timeout == 0) {
      ttyusb6.timeout = RT_WAITING_FOREVER;
    }
    if(ttyusb6.interval <= 0) {
      ttyusb6.interval = 10;
    }
    rt_mutex_init(&ttyusb6.tty_in_lock, "ttyin6", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&ttyusb6.tty_out_lock, "ttyout6", RT_IPC_FLAG_FIFO);
    
    rt_usbh_cdc_interface_register(&ttyusb6);
  }
#endif 
  
 return 0;
}

INIT_ENV_EXPORT(rt_ttyusb_driver_init);
#endif
