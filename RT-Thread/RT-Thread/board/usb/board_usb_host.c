/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_usb_host.c
** Last modified Date:  2018-10-26
** Last Version:        v1.00
** Description:         USB主机驱动程序
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2018-10-26
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <rtthread.h>
#include <rtdevice.h>

#ifdef RT_USING_LWIP
#include "lwip/mem.h"
#endif

#include "hal/cortexm/hal_cortexm_api.h"

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"
#include "hal/arch/inc/arch_io_usb.h"

#include "stm32_usb_otg.h"
#include "stm32_usb_otg_host.h"

/*********************************************************************************************************
** 调试输出宏定义
*********************************************************************************************************/
#define USB_HOST_DEBUG   1

#if (USB_HOST_DEBUG > 0)
#define usb_host_trace(fmt, ...)           rt_kprintf(fmt, ##__VA_ARGS__)
#else
#define usb_host_trace(fmt, ...)
#endif

/*********************************************************************************************************
** 设备驱动配置
*********************************************************************************************************/
#define RT_USING_USB_OTG_FS_HOST
//#define RT_USING_USB_OTG_HS_HOST
//#define RT_USING_USB_OTG_HS_ULPI

/*********************************************************************************************************
** USB硬件连接配置
*********************************************************************************************************/
#ifdef  RT_USING_USB_OTG_FS_HOST
#define USB_OTG_FS_GPIO_PERIPHERAL      RCC_PERIPHERAL_GPIOA
//#define USB_OTG_FS_GPIO_PERIPHERAL1     RCC_PERIPHERAL_GPIOD
#define USB_OTG_FS_USB_PERIPHERAL       RCC_PERIPHERAL_USB2OTG
#define USB_OTG_FS_GPIO_PIN_DP          GPIOA_BASE,GPIO_PIN_11
#define USB_OTG_FS_GPIO_PIN_DM          GPIOA_BASE,GPIO_PIN_12
#define USB_OTG_FS_GPIO_NO_DP           GPIOA_BASE,11
#define USB_OTG_FS_GPIO_NO_DM           GPIOA_BASE,12

//// 如果使用GPIO控制VBUS
//#define USB_OTG_FS_HOST_VBUS_BASE       GPIOA_BASE
//#define USB_OTG_FS_HOST_VBUS_PIN        GPIO_PIN_9
#endif

#ifdef  RT_USING_USB_OTG_HS_HOST
#ifdef  RT_USING_USB_OTG_HS_ULPI

#define USB_OTG_HS_GPIO_PERIPHERAL      RCC_PERIPHERAL_GPIOA
#define USB_OTG_HS_GPIO_PERIPHERAL1     RCC_PERIPHERAL_GPIOB
#define USB_OTG_HS_GPIO_PERIPHERAL2     RCC_PERIPHERAL_GPIOC
#define USB_OTG_HS_USB_PERIPHERAL       RCC_PERIPHERAL_USB1OTG
#define USB_OTG_HS_ULPI_PERIPHERAL      RCC_PERIPHERAL_USB1ULPI

/*
** USB ULPI硬件连接：   ULPI_D0   <--> PA3     ULPI_CLK    <-->  PA5
**                      ULPI_D1   <--> PB0     ULPI_STP    <-->  PC0
**                      ULPI_D2   <--> PB1     ULPI_DIR    <-->  PC2
**                      ULPI_D3   <--> PB10    ULPI_NXT    <-->  PC3
**                      ULPI_D4   <--> PB11    
**                      ULPI_D5   <--> PB12
**                      ULPI_D6   <--> PB13
**                      ULPI_D7   <--> PB5
**   普通GPIO控制复位： ULPI_RST    <-->  PB6
*/

#else

#define USB_OTG_HS_GPIO_PERIPHERAL      RCC_PERIPHERAL_GPIOA
//#define USB_OTG_HS_GPIO_PERIPHERAL1     RCC_PERIPHERAL_GPIOD
#define USB_OTG_HS_USB_PERIPHERAL       RCC_PERIPHERAL_USB1OTG
#define USB_OTG_HS_GPIO_PIN_DP          GPIOA_BASE,GPIO_PIN_11
#define USB_OTG_HS_GPIO_PIN_DM          GPIOA_BASE,GPIO_PIN_12
#define USB_OTG_HS_GPIO_NO_DP           GPIOA_BASE,11
#define USB_OTG_HS_GPIO_NO_DM           GPIOA_BASE,12

#endif

//// 如果使用GPIO控制VBUS
//#define USB_OTG_HS_HOST_VBUS_BASE       GPIOA_BASE
//#define USB_OTG_HS_HOST_VBUS_PIN        GPIO_PIN_9
#endif

/*********************************************************************************************************
** USB特征结构变量定义
*********************************************************************************************************/
#ifdef  RT_USING_USB_OTG_FS_HOST
stm32_usb_host_t stm32_usb_fs = 
{
  .base = USB2_OTG_FS_BASE,             //  模块基地址
  .port = 1,                            //  端口编号, usb中的设备编号，不同的USB控制器不能相同
  .phy_interface = USB_OTG_EMBEDDED_PHY,//  使用的PHY接口
  .phy_speed = USB_OTG_SPEED_FULL,      //  USB速度
  .channel_num = 16,                    //  channel数量
  .state = USB_HOST_PORT_DUMMY,         //  Host port初始状态
  .hc = {0},                            //  host channel array 
  .nptx_pending = 0,                    //  Non periodic pending trasmit/out pipe
  .ptx_pending = 0,                     //  Periodic pending trasmit/out pipe
  .user_data = RT_NULL                  //  User data for host 
};
#endif

#ifdef  RT_USING_USB_OTG_HS_HOST
stm32_usb_host_t stm32_usb_hs = 
{
  .base = USB1_OTG_HS_BASE,             //  模块基地址
#ifdef RT_USING_USB_OTG_FS_HOST
  .port = 2,                            //  端口编号, usb中的设备编号，不同的USB控制器不能相同
#else 
  .port = 1,
#endif
  .phy_interface = USB_OTG_ULPI_PHY,    //  使用的PHY接口
  .phy_speed = USB_OTG_SPEED_HIGH,      //  USB速度
  .channel_num = 16,                    //  channel数量
  .state = USB_HOST_PORT_DUMMY,         //  Host port初始状态
  .hc = {0},                            //  host channel array 
  .nptx_pending = 0,                    //  Non periodic pending trasmit/out pipe
  .ptx_pending = 0,                     //  Periodic pending trasmit/out pipe
  .user_data = RT_NULL                  //  User data for host 
};
#endif

/*********************************************************************************************************
** Function name:       usb_otg_fs_isr
** Descriptions:        主机中断服务程序
** input parameters:    hcd:    USB主机结构
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
#ifdef  RT_USING_USB_OTG_FS_HOST
void usb_otg_fs_isr(void)
{
  rt_interrupt_enter();
  usb_otg_host_handler(&stm32_usb_fs);
  rt_interrupt_leave();
}
#endif

#ifdef  RT_USING_USB_OTG_HS_HOST
void usb_otg_hs_isr(void)
{
  rt_interrupt_enter();
  usb_otg_host_handler(&stm32_usb_hs);
  rt_interrupt_leave();
}
#endif

/*********************************************************************************************************
** Function name:       usb_host_port_changed_callback
** Descriptions:        主机Port状态改变回调函数
** input parameters:    host:       USB主机结构
**                      new_state： 改变后的状态
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void usb_host_port_changed_callback(stm32_usb_host_t* host, host_port_state_t new_state)
{
  uhcd_t uhcd = (uhcd_t)host->user_data;
  
  switch(new_state)
  {
  case USB_HOST_PORT_CONNECTED:
    rt_kprintf("usb%d connnect\r\n", host->port);
    rt_usbh_root_hub_connect_handler(uhcd, host->port, (usb_port_get_speed(host)==USB_OTG_SPEED_HIGH)?RT_TRUE:RT_FALSE);
    break;
  case USB_HOST_PORT_ENABLED:
    rt_kprintf("usb%d enabled\r\n", host->port);
    break;
  case USB_HOST_PORT_DISCONNECTED:
    rt_kprintf("usb%d disconnnect\r\n", host->port);
    rt_usbh_root_hub_disconnect_handler(uhcd, host->port);
    break;
  case USB_HOST_PORT_DISABLED:
    rt_kprintf("usb%d disabled\r\n", host->port);
    break;
  default:
    break;
  }
}

/*********************************************************************************************************
** Function name:       hcd_sof_callback
** Descriptions:        主机sof回调
** input parameters:    hcd:    USB主机结构
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void usb_host_sof_callback(stm32_usb_host_t* host)
{
//   usb_host_trace("usb%d sof callback\r\n", host->port);
}

/*********************************************************************************************************
** Function name:       rt_drv_reset_port
** Descriptions:        复位端口
** input parameters:    port:    指定复位的USB端口
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static rt_err_t rt_drv_reset_port(rt_uint8_t port)
{
  usb_host_trace("reset port usb%d\r\n", port);
#ifdef  RT_USING_USB_OTG_FS_HOST
  if(port == stm32_usb_fs.port) {
    usb_port_reset(&stm32_usb_fs);
  }
#endif
  
#ifdef  RT_USING_USB_OTG_HS_HOST
  if(port == stm32_usb_hs.port) {
     usb_port_reset(&stm32_usb_hs);
  }
#endif
  

  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rt_drv_pipe_xfer
** Descriptions:        USB数据传输
** input parameters:    pipe:    传输通道
**                      token:   端点服务类型，可以取值：0: HC_PID_SETUP / 1: HC_PID_DATA1
**                      buffer:  传输数据缓冲区
**                      nbytes:  传输数据长度
**                      timeout: 传输超时时间
** output parameters:   NONE
** Returned value:      实际传输的数据长度
*********************************************************************************************************/
static int rt_drv_pipe_xfer(upipe_t pipe, rt_uint8_t token, void *buffer, int nbytes, int timeout)
{
  //uhcd_t uhcd = pipe->inst->hcd;
  uint8_t *aligned_bufer = RT_NULL;
  stm32_usb_host_t* host = pipe->inst->hcd->parent.user_data;
  stm32_usb_host_channel_t* hc = &host->hc[pipe->pipe_index];
  channel_state_t s;
  rt_tick_t start = rt_tick_get();

  if(host->state != USB_HOST_PORT_ENABLED) {
    return -1;
  }
  
  if((uint32_t)buffer & 0x03) {
#ifdef RT_USING_LWIP
     aligned_bufer = mem_malloc(nbytes);
#else
     aligned_bufer = rt_malloc(nbytes);
#endif
     if(aligned_bufer == RT_NULL) {
       return -1;
     }
     if(!((pipe->ep.bEndpointAddress & 0x80) >> 7)) {
       memcpy(aligned_bufer, buffer, nbytes);
     }
  }
  if(aligned_bufer == RT_NULL) {
    usb_otg_host_xfer_data(host, pipe->pipe_index, (token==USBH_PID_SETUP)?0:1, buffer, nbytes);
  } else {
    usb_otg_host_xfer_data(host, pipe->pipe_index, (token==USBH_PID_SETUP)?0:1, aligned_bufer, nbytes);
  }
  usb_host_xfer_wait(host, pipe->pipe_index, timeout);
  s = (channel_state_t)hc->state;
//  if((s != USB_CS_TRANSFER_COMPLETE) && (nbytes != 0)){ 
  if((s != USB_CS_TRANSFER_COMPLETE)){ 
//    usb_host_trace("usb%d recv error\r\n", host->port);
//    if((pipe->ep.bEndpointAddress & 0x80) >> 7) {
//      if(s != USB_CS_INIT) {
//         rt_kprintf("recv time tick is %d, state is %d\r\n", rt_tick_get() - start, s);
//      }
//    } 
//    else {
//      rt_kprintf("send time tick is %d, state is %d, status 0x%08x, msk 0x%08x\r\n", rt_tick_get() - start, s,
//                 HWREG32(host->base + USB_OTG_HCINT0 + 0x20UL * pipe->pipe_index),
//                 HWREG32(host->base + USB_OTG_HCINTMSK0 + 0x20UL * pipe->pipe_index));
//    }
    if(aligned_bufer != RT_NULL) {
#ifdef RT_USING_LWIP
      mem_free(aligned_bufer);
#else
      rt_free(aligned_bufer);
#endif
    }
    return -1;
  }
  if((pipe->ep.bEndpointAddress & 0x80) >> 7) {
    if(aligned_bufer == RT_NULL) {
      rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, buffer, hc->count);
    } else {
      rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, aligned_bufer, hc->count);
      memcpy(buffer, aligned_bufer, hc->count);
#ifdef RT_USING_LWIP
      mem_free(aligned_bufer);
#else
      rt_free(aligned_bufer);
#endif
    }
    return hc->count;
  } else {
    if(aligned_bufer != RT_NULL) {
#ifdef RT_USING_LWIP
      mem_free(aligned_bufer);
#else
      rt_free(aligned_bufer);
#endif
    }
    return nbytes;
  }
}

/*********************************************************************************************************
** Function name:       rt_drv_pipe_fs_xfer
** Descriptions:        USB数据传输
** input parameters:    pipe:    传输通道
**                      token:   端点服务类型，可以取值：0: HC_PID_SETUP / 1: HC_PID_DATA1
**                      buffer:  传输数据缓冲区
**                      nbytes:  传输数据长度
**                      timeout: 传输超时时间
** output parameters:   NONE
** Returned value:      实际传输的数据长度
*********************************************************************************************************/
static int rt_drv_pipe_fs_xfer(upipe_t pipe, rt_uint8_t token, void *buffer, int nbytes, int timeout)
{
  //uhcd_t uhcd = pipe->inst->hcd;
  stm32_usb_host_t* host = pipe->inst->hcd->parent.user_data;
  stm32_usb_host_channel_t* hc = &host->hc[pipe->pipe_index];
  channel_state_t s;
  rt_tick_t start = rt_tick_get();

  if(host->state != USB_HOST_PORT_ENABLED) {
    return -1;
  }
  
  usb_otg_host_xfer_data(host, pipe->pipe_index, (token==USBH_PID_SETUP)?0:1, buffer, nbytes);
  usb_host_xfer_wait(host, pipe->pipe_index, timeout);
  s = (channel_state_t)hc->state;
  if((s != USB_CS_TRANSFER_COMPLETE)){ 
    //    usb_host_trace("usb%d recv error\r\n", host->port);
    if((pipe->ep.bEndpointAddress & 0x80) >> 7) {
      if(s != USB_CS_NAK) {
         rt_kprintf("recv time tick is %d, state is %d\r\n", rt_tick_get() - start, s);
      }
    } 
    else {
      rt_kprintf("send time tick is %d, state is %d, status 0x%08x, msk 0x%08x\r\n", rt_tick_get() - start, s,
                 HWREG32(host->base + USB_OTG_HCINT0 + 0x20UL * pipe->pipe_index),
                 HWREG32(host->base + USB_OTG_HCINTMSK0 + 0x20UL * pipe->pipe_index));
    }
    return -1;
  }
  if((pipe->ep.bEndpointAddress & 0x80) >> 7) {
    return hc->count;
  } else {
    return nbytes;
  }
}

/*********************************************************************************************************
** Function name:       rt_drv_open_pipe
** Descriptions:        打开pipe
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static rt_err_t rt_drv_open_pipe(upipe_t pipe)
{
  //uhcd_t uhcd = pipe->inst->hcd;
  uint8_t speed;
  stm32_usb_host_t *host = pipe->inst->hcd->parent.user_data;
  
  uint8_t hc_num = usb_otg_host_get_free_ch(host);
  if(hc_num < host->channel_num){
    pipe->pipe_index = hc_num;
    speed = usb_port_get_speed(host);
    usb_host_channel_init(host, pipe->pipe_index, 
                          pipe->inst->address, pipe->ep.bEndpointAddress, 
                          pipe->ep.bmAttributes, pipe->ep.wMaxPacketSize, speed);
    usb_host_trace("usb%d alloc pipe %d\r\n", host->port, hc_num);
    return RT_EOK;
  }
  return -RT_ERROR;
}

/*********************************************************************************************************
** Function name:       rt_drv_close_pipe
** Descriptions:        关闭pipe
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static rt_err_t rt_drv_close_pipe(upipe_t pipe)
{
  //uhcd_t uhcd = pipe->inst->hcd;
  stm32_usb_host_t *host = pipe->inst->hcd->parent.user_data;
  
  if(pipe->pipe_index < host->channel_num){
     usb_host_channel_deinit(host, pipe->pipe_index);
  }
  usb_host_trace("usb%d free pipe %d\r\n", host->port, pipe->pipe_index);
  return RT_EOK;
}

/*********************************************************************************************************
** 定义驱动操作结构
*********************************************************************************************************/
struct uhcd_ops _uhcd_ops =
{
    rt_drv_reset_port,
    rt_drv_pipe_xfer,
    rt_drv_open_pipe,
    rt_drv_close_pipe,
};

struct uhcd_ops _uhcd_fs_ops =
{
    rt_drv_reset_port,
    rt_drv_pipe_xfer, //rt_drv_pipe_xfer, //rt_drv_pipe_fs_xfer,
    rt_drv_open_pipe,
    rt_drv_close_pipe,
};

/*********************************************************************************************************
** Function name:       rt_drv_usb_host_init
** Descriptions:        设备初始化函数
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static rt_err_t rt_drv_usb_host_init(rt_device_t device)
{
  stm32_usb_host_t *host = (stm32_usb_host_t *)device->user_data;
  uint32_t u32Base = host->base;
  
  if(host->phy_speed == USB_OTG_SPEED_HIGH) {
    usb_otg_core_init(u32Base,host->phy_interface, RT_FALSE, RT_TRUE);
  } else {
    usb_otg_core_init(u32Base,host->phy_interface, RT_FALSE, RT_TRUE);
  }
  // Force to host mode
  usb_otg_mode_set(u32Base,USB_OTG_MODE_HOST);
  usb_otg_host_init(u32Base, host->phy_speed, host->channel_num);
  
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       stm_usbh_register
** Descriptions:        设备驱动注册
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int stm_usbh_register(void)
{
  SystemPeripheralEnable(RCC_PERIPHERAL_SYSCFG);
#ifdef RT_USING_USB_OTG_FS_HOST
  uhcd_t ufshcd = (uhcd_t)rt_malloc(sizeof(struct uhcd));
  RT_ASSERT(ufshcd != RT_NULL);
  
  /**
  * Step1, 使能外设引脚
  */
  SystemPeripheralEnable(USB_OTG_FS_GPIO_PERIPHERAL);
#ifdef USB_OTG_FS_GPIO_PERIPHERAL1
  SystemPeripheralEnable(USB_OTG_FS_GPIO_PERIPHERAL1);
#endif
  SystemPeripheralEnable(USB_OTG_FS_USB_PERIPHERAL);
  
  GPIOPinAFConfig(USB_OTG_FS_GPIO_NO_DP, GPIO_AF_OTG_FS);
  GPIOPinAFConfig(USB_OTG_FS_GPIO_NO_DM, GPIO_AF_OTG_FS);
  
  // Configure The USB GPIO
  GPIOModeSet(USB_OTG_FS_GPIO_PIN_DP,GPIO_MODE_AF);
  GPIOOutputTypeSet(USB_OTG_FS_GPIO_PIN_DP,GPIO_OUT_PP);
  GPIOOutputSpeedSet(USB_OTG_FS_GPIO_PIN_DP,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(USB_OTG_FS_GPIO_PIN_DP,GPIO_PUPD_NOPULL);
  
  GPIOModeSet(USB_OTG_FS_GPIO_PIN_DM,GPIO_MODE_AF);
  GPIOOutputTypeSet(USB_OTG_FS_GPIO_PIN_DM,GPIO_OUT_PP);
  GPIOOutputSpeedSet(USB_OTG_FS_GPIO_PIN_DM,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(USB_OTG_FS_GPIO_PIN_DM,GPIO_PUPD_NOPULL);
  
//  // 开启USB稳压器
//  HWREG32(PWR_BASE + PWR_CR3) |= (1UL << 25);
//  while(!(HWREG32(PWR_BASE + PWR_CR3) & ((rt_uint32_t)1 << 26)));
  
    //使能USB VDD3电压检测
    HWREG32(PWR_BASE + PWR_CR3) |= (1UL << 24);
  
  // 如果使用GPIO控制VBUS
#if  defined(USB_OTG_FS_HOST_VBUS_BASE) && (USB_OTG_FS_HOST_VBUS_BASE > 0) 
#endif
  
  SystemPeripheralReset(USB_OTG_FS_USB_PERIPHERAL);
  
  /**
  * Step2, 注册中断向量
  */
  hal_int_register(ARCH_INT_OTG_FS, usb_otg_fs_isr);
  hal_int_priority_set(ARCH_INT_OTG_FS,0x10);
  hal_int_enable(ARCH_INT_OTG_FS);
  
  /**
  * Step3, 注册设备驱动
  */
  rt_memset((void *)ufshcd, 0, sizeof(struct uhcd));
  ufshcd->parent.type = RT_Device_Class_USBHost;
  ufshcd->parent.init = rt_drv_usb_host_init;
  ufshcd->parent.user_data = &stm32_usb_fs;
  ufshcd->ops = &_uhcd_fs_ops;
  ufshcd->num_ports = 1;
  stm32_usb_fs.user_data = ufshcd;
  rt_device_register((rt_device_t)ufshcd, "usbh", 0);
  rt_usb_host_init();
#endif
  
#ifdef RT_USING_USB_OTG_HS_HOST
  uhcd_t uhshcd = (uhcd_t)rt_malloc(sizeof(struct uhcd));
  RT_ASSERT(uhshcd != RT_NULL);
  
  /**
  * Step1, 使能外设引脚
  */
#ifdef  RT_USING_USB_OTG_HS_ULPI
  SystemPeripheralEnable(USB_OTG_HS_GPIO_PERIPHERAL);
#ifdef USB_OTG_HS_GPIO_PERIPHERAL1
  SystemPeripheralEnable(USB_OTG_HS_GPIO_PERIPHERAL1);
#endif
#ifdef USB_OTG_HS_GPIO_PERIPHERAL2
  SystemPeripheralEnable(USB_OTG_HS_GPIO_PERIPHERAL2);
#endif
  SystemPeripheralEnable(USB_OTG_HS_USB_PERIPHERAL);
  SystemPeripheralEnable(USB_OTG_HS_ULPI_PERIPHERAL);
  
  // 软件控制复位ULPI PHY
  GPIOModeSet(GPIOB_BASE, GPIO_PIN_6,GPIO_MODE_OUT);
  GPIOOutputTypeSet(GPIOB_BASE, GPIO_PIN_6, GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOB_BASE, GPIO_PIN_6, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOB_BASE, GPIO_PIN_6, GPIO_PUPD_NOPULL);
  GPIOPinSetBit(GPIOB_BASE, GPIO_PIN_6);
  rt_thread_delay(1);
  
  GPIOPinAFConfig(GPIOA_BASE, 3, GPIO_AF10_ULPI);
  GPIOPinAFConfig(GPIOA_BASE, 5, GPIO_AF10_ULPI);
  
  GPIOPinAFConfig(GPIOB_BASE, 0, GPIO_AF10_ULPI);
  GPIOPinAFConfig(GPIOB_BASE, 1, GPIO_AF10_ULPI);
  GPIOPinAFConfig(GPIOB_BASE, 5, GPIO_AF10_ULPI);
  GPIOPinAFConfig(GPIOB_BASE, 10, GPIO_AF10_ULPI);
  GPIOPinAFConfig(GPIOB_BASE, 11, GPIO_AF10_ULPI);
  GPIOPinAFConfig(GPIOB_BASE, 12, GPIO_AF10_ULPI);
  GPIOPinAFConfig(GPIOB_BASE, 13, GPIO_AF10_ULPI);

  GPIOPinAFConfig(GPIOC_BASE, 0, GPIO_AF10_ULPI);
  GPIOPinAFConfig(GPIOC_BASE, 2, GPIO_AF10_ULPI);
  GPIOPinAFConfig(GPIOC_BASE, 3, GPIO_AF10_ULPI);
  
 // Configure PA3，PA5
  GPIOModeSet(GPIOA_BASE,
              GPIO_PIN_3 | GPIO_PIN_5,
              GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOA_BASE,
                    GPIO_PIN_3 | GPIO_PIN_5,
                    GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOA_BASE,
                     GPIO_PIN_3 | GPIO_PIN_5,
                     GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOA_BASE,
                    GPIO_PIN_3 | GPIO_PIN_5,
                    GPIO_PUPD_NOPULL);
  
  // Configure PB0,PB1,PB5,PB6,PB10,PB11,PB12,PB13
  GPIOModeSet(GPIOB_BASE,
              GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
              GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOB_BASE,
                    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
                    GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOB_BASE,
                     GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
                     GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOB_BASE,
                    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
                    GPIO_PUPD_NOPULL);
  
  // Configure PC0, PC2,PC3
  GPIOModeSet(GPIOC_BASE,
              GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3,
              GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOC_BASE,
                    GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3,
                    GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOC_BASE,
                     GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3,
                     GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOC_BASE,
                    GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3,
                    GPIO_PUPD_NOPULL);
  
#else
  
  SystemPeripheralEnable(USB_OTG_HS_GPIO_PERIPHERAL);
#ifdef USB_OTG_HS_GPIO_PERIPHERAL1
  SystemPeripheralEnable(USB_OTG_HS_GPIO_PERIPHERAL1);
#endif
  SystemPeripheralEnable(USB_OTG_HS_USB_PERIPHERAL);
  
  GPIOPinAFConfig(USB_OTG_HS_GPIO_NO_DP, GPIO_AF_OTG_HS);
  GPIOPinAFConfig(USB_OTG_HS_GPIO_NO_DM, GPIO_AF_OTG_HS);
  
  // Configure The USB GPIO
  GPIOModeSet(USB_OTG_HS_GPIO_PIN_DP,GPIO_MODE_AF);
  GPIOOutputTypeSet(USB_OTG_HS_GPIO_PIN_DP,GPIO_OUT_PP);
  GPIOOutputSpeedSet(USB_OTG_HS_GPIO_PIN_DP,GPIO_SPEED_100MHZ);
  GPIOPullUpDownSet(USB_OTG_HS_GPIO_PIN_DP,GPIO_PUPD_UP);
  
  GPIOModeSet(USB_OTG_HS_GPIO_PIN_DM,GPIO_MODE_AF);
  GPIOOutputTypeSet(USB_OTG_HS_GPIO_PIN_DM,GPIO_OUT_PP);
  GPIOOutputSpeedSet(USB_OTG_HS_GPIO_PIN_DM,GPIO_SPEED_100MHZ);
  GPIOPullUpDownSet(USB_OTG_HS_GPIO_PIN_DM,GPIO_PUPD_UP);
#endif
  
  // 如果使用GPIO控制VBUS
#if  defined(USB_OTG_HS_HOST_VBUS_BASE) && (USB_OTG_HS_HOST_VBUS_BASE > 0) 
#endif
  
  /**
  * Step2, 注册中断向量
  */
  hal_int_register(ARCH_INT_OTG_HS, usb_otg_hs_isr);
  hal_int_priority_set(ARCH_INT_OTG_HS,0);
  hal_int_enable(ARCH_INT_OTG_HS);
  
  /**
  * Step3, 注册设备驱动
  */
  rt_memset((void *)uhshcd, 0, sizeof(struct uhcd));
  uhshcd->parent.type = RT_Device_Class_USBHost;
  uhshcd->parent.init = rt_drv_usb_host_init;
  uhshcd->parent.user_data = &stm32_usb_hs;
  if(stm32_usb_hs.phy_speed != USB_OTG_SPEED_HIGH) {
    _uhcd_ops.pipe_xfer = rt_drv_pipe_fs_xfer;
  }
  uhshcd->ops = &_uhcd_ops;
  uhshcd->num_ports = 1;
  stm32_usb_hs.user_data = uhshcd;
  rt_device_register((rt_device_t)uhshcd, "usbh", 0);
  rt_usb_host_init();
  GPIOPinResetBit(GPIOB_BASE, GPIO_PIN_6);
#endif
  
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       usb_speed
** Descriptions:        获取usb主机速度
** input parameters:    NONE       
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void usb_speed(void)
{
  uint32_t u32Temp;
  
  u32Temp = usb_port_get_speed(&stm32_usb_fs);
  switch(u32Temp) {
  case USB_OTG_SPEED_HIGH:
    printf("usb in high speed mode\r\n");
    break;
  case USB_OTG_SPEED_FULL:
    printf("usb in full speed mode\r\n");
    break;
  case USB_OTG_SPEED_LOW:
    printf("usb in low speed mode\r\n");
    break;
  default:
    printf("usb in full speed mode\r\n");
  }
}
#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(usb_speed, get usb host speed );
#endif /* FINSH_USING_MSH */

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_DEVICE_EXPORT(stm_usbh_register);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
