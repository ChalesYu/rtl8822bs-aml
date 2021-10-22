/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_usart_frame.c
** Last modified Date:  2014-12-26
** Last Version:        v1.00
** Description:         串口驱动——按照操作系统的串口驱动框架编写
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2014-12-26
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/serial.h>

#include "hal/cortexm/hal_cortexm_api.h"

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"
#include "hal/arch/inc/arch_io_usart.h"

#include "board_serial.h"

#if defined(RT_USING_FRAME_UART1) || defined(RT_USING_FRAME_UART2) || defined(RT_USING_FRAME_UART3) || \
defined(RT_USING_FRAME_UART4) || defined(RT_USING_FRAME_UART5) || defined(RT_USING_FRAME_UART6)
/*********************************************************************************************************
** 表征串口的结构体定义
*********************************************************************************************************/
//  接收发送均采用DMA串口特性定义
typedef struct
{
  rt_uint32_t  base;                       		//  模块基地址
  rt_uint16_t   u16WhichBus; rt_uint32_t u32Ctrl;  	//  外设总线属性，用于确定外设时钟
  rt_uint8_t   int_number;                        //  串口中断序号
} stm32_serial_info;

/*********************************************************************************************************
** Function name:       stm32_configure
** Descriptions:        串口配置
** input parameters:    serial:  串口驱动框架结构
** 						cfg:     串口配置参数
** output parameters:   NONE
** Returned value:      错误码
*********************************************************************************************************/
static rt_err_t stm32_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
  stm32_serial_info *serial_info;
  rt_uint32_t u32Config = 0;
  rt_uint32_t u32Clock;
  
  RT_ASSERT(serial != RT_NULL);
  RT_ASSERT(cfg != RT_NULL);
  
  serial_info = (stm32_serial_info *)serial->parent.user_data;
  
  // 开始配置
  if(cfg->parity == PARITY_NONE) {
    u32Config = USART_WLEN_8 | USART_PAR_NONE;
  } else {
    if(cfg->data_bits == DATA_BITS_7) {
      u32Config = USART_WLEN_8;
    } else if(cfg->data_bits == DATA_BITS_8) {
      u32Config = USART_WLEN_9;
    } else {
      return RT_ERROR;
    }
    
    if(cfg->parity == PARITY_ODD) {
      u32Config |= USART_PAR_ODD;
    } else if(cfg->parity == PARITY_EVEN){
      u32Config |= USART_PAR_EVEN;
    } else {
      return RT_ERROR;
    }
  }
  
  if(cfg->stop_bits == STOP_BITS_1) {
    u32Config |= USART_STOP_1;
  } else if(cfg->stop_bits == STOP_BITS_2) {
    u32Config |= USART_STOP_2;
  } else {
    return RT_ERROR;
  }
  
  UsartDisable(serial_info->base);
  // 获取外设时钟
  u32Clock = SystemPeripheralClockGet(serial_info->u16WhichBus, serial_info->u32Ctrl);
  UsartConfigSet(serial_info->base, u32Clock, cfg->baud_rate, u32Config);
  if(serial_info->base == LPUART1_BASE) {
    HWREG32(LPUART1_BASE + USART_CR1) |= 0x00000002;
  }
  //  HWREG32(serial_info->base + USART_CR3) |= (1UL << 12);   //  关闭溢出使能控制
  UsartIntEnable(serial_info->base,USART_INT_RX | USART_INT_PE | USART_INT_EIE);
  UsartEnable(serial_info->base);
  
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       stm32_configure
** Descriptions:        串口控制
** input parameters:    serial:  串口驱动框架结构
**						cmd:	配置命令
**                      args: 	配置参数
** output parameters:   NONE
** Returned value:      错误码
*********************************************************************************************************/
static rt_err_t stm32_control(struct rt_serial_device *serial, int cmd, void *arg)
{
  stm32_serial_info *serial_info;
  
  RT_ASSERT(serial != RT_NULL);
  
  serial_info = (stm32_serial_info *)serial->parent.user_data;
  
  switch (cmd)
  {
  case RT_DEVICE_CTRL_CLR_INT:
    /* disable rx irq */
    hal_int_disable(serial_info->int_number);
    break;
  case RT_DEVICE_CTRL_SET_INT:
    /* enable rx irq */
    hal_int_enable(serial_info->int_number);
    break;
  }
  
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       stm32_putc
** Descriptions:        串口发送一个字节的数据
** input parameters:    serial:  串口驱动框架结构
**						c:       待发送的数据
** output parameters:   NONE
** Returned value:      1: 发送成功
*********************************************************************************************************/
static int stm32_putc(struct rt_serial_device *serial, char c)
{
  stm32_serial_info *serial_info;
  
  RT_ASSERT(serial != RT_NULL);
  
  serial_info = (stm32_serial_info *)serial->parent.user_data;
  
  while(!(HWREG32(serial_info->base + USART_ISR) & 0x0080));
  HWREG32(serial_info->base + USART_TDR) = c;
  
  return 1;
}

/*********************************************************************************************************
** Function name:       stm32_getc
** Descriptions:        串口接收一个字节的数据
** input parameters:    serial:  串口驱动框架结构
**						c:       待发送的数据
** output parameters:   NONE
** Returned value:      1: 发送成功
*********************************************************************************************************/
static int stm32_getc(struct rt_serial_device *serial)
{
  int ch;
  stm32_serial_info *serial_info;
  
  RT_ASSERT(serial != RT_NULL);
  
  serial_info = (stm32_serial_info *)serial->parent.user_data;
  
  ch = -1;
  if(HWREG32(serial_info->base + USART_ISR) & 0x0020) {
    ch = (HWREG32(serial_info->base + USART_RDR) & 0x01FF);   //  读出数据，低9为有效
  }
  
//  if(HWREG32(serial_info->base + USART_ISR) & 0x0008) {
//    HWREG32(serial_info->base + USART_ICR) |= 0x0008;
//  }
  
  return ch;
}

/*********************************************************************************************************
** 定义基本操作结构
*********************************************************************************************************/
static const struct rt_uart_ops stm32_uart_ops =
{
  stm32_configure,
  stm32_control,
  stm32_putc,
  stm32_getc,
};

#if defined(RT_USING_FRAME_UART1)
/* UART1 device driver structure */
stm32_serial_info __gUart1Info =
{
  USART1_BASE,                            //  模块基地址
  RCC_PERIPHERAL_USART1,                  //  外设总线属性，用于确定外设时钟
  ARCH_INT_USART1,                        //  中断序号
};
struct rt_serial_device __gSerial1;

void USART1_frame_ISR(void)
{
  stm32_serial_info *serial_info = &__gUart1Info;
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0001) {             //  如果是校验错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0002) {             //  如果是帧错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(!(HWREG32(serial_info->base + USART_CR1) & USART_INT_RX))  // 如果接收中断未使能
  {
    return;
  }
  
  /* enter interrupt */
  rt_interrupt_enter();
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0020)              //  如果是接受中断
  {
    rt_hw_serial_isr(&__gSerial1, RT_SERIAL_EVENT_RX_IND);
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0008)              //  如果是数据过载了
  {
    rt_hw_serial_isr(&__gSerial1, RT_SERIAL_EVENT_RX_IND);
    HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
  }
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif /* RT_USING_FRAME_UART1 */

#if defined(RT_USING_FRAME_UART2)
/* UART2 device driver structure */
stm32_serial_info __gUart2Info =
{
  USART2_BASE,                            //  模块基地址
  RCC_PERIPHERAL_USART2,                  //  外设总线属性，用于确定外设时钟
  ARCH_INT_USART2,                        //  中断序号
};
struct rt_serial_device __gSerial2;

void USART2_frame_ISR(void)
{
  stm32_serial_info *serial_info = &__gUart2Info;
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0001) {             //  如果是校验错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0002) {             //  如果是帧错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(!(HWREG32(serial_info->base + USART_CR1) & USART_INT_RX))  // 如果接收中断未使能
  {
    return;
  }
  
  /* enter interrupt */
  rt_interrupt_enter();
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0020)              //  如果是接受中断
  {
    rt_hw_serial_isr(&__gSerial2, RT_SERIAL_EVENT_RX_IND);
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0008)              //  如果是数据过载了
  {
    rt_hw_serial_isr(&__gSerial2, RT_SERIAL_EVENT_RX_IND);
    HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
  }
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif /* RT_USING_FRAME_UART2 */

#if defined(RT_USING_FRAME_UART3)
/* UART3 device driver structure */
stm32_serial_info __gUart3Info =
{
  USART3_BASE,                            //  模块基地址
  RCC_PERIPHERAL_USART3,                  //  外设总线属性，用于确定外设时钟
  ARCH_INT_USART3,                        //  中断序号
};
struct rt_serial_device __gSerial3;

void USART3_frame_ISR(void)
{
  stm32_serial_info *serial_info = &__gUart3Info;
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0001) {             //  如果是校验错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0002) {             //  如果是帧错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(!(HWREG32(serial_info->base + USART_CR1) & USART_INT_RX))  // 如果接收中断未使能
  {
    return;
  }
  
  /* enter interrupt */
  rt_interrupt_enter();
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0020)              //  如果是接受中断
  {
    rt_hw_serial_isr(&__gSerial3, RT_SERIAL_EVENT_RX_IND);
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0008)              //  如果是数据过载了
  {
    rt_hw_serial_isr(&__gSerial3, RT_SERIAL_EVENT_RX_IND);
    HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
  }
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif /* RT_USING_FRAME_UART3 */

#if defined(RT_USING_FRAME_UART4)
/* UART2 device driver structure */
stm32_serial_info __gUart4Info =
{
  UART4_BASE,                            //  模块基地址
  RCC_PERIPHERAL_UART4,                  //  外设总线属性，用于确定外设时钟
  ARCH_INT_UART4,                        //  中断序号
};
struct rt_serial_device __gSerial4;

void USART4_frame_ISR(void)
{
  stm32_serial_info *serial_info = &__gUart4Info;
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0001) {             //  如果是校验错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0002) {             //  如果是帧错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(!(HWREG32(serial_info->base + USART_CR1) & USART_INT_RX))  // 如果接收中断未使能
  {
    return;
  }
  
  /* enter interrupt */
  rt_interrupt_enter();
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0020)              //  如果是接受中断
  {
    rt_hw_serial_isr(&__gSerial4, RT_SERIAL_EVENT_RX_IND);
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0008)              //  如果是数据过载了
  {
    rt_hw_serial_isr(&__gSerial4, RT_SERIAL_EVENT_RX_IND);
    HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
  }
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif /* RT_USING_FRAME_UART4 */

#if defined(RT_USING_FRAME_UART5)
/* UART2 device driver structure */
stm32_serial_info __gUart5Info =
{
  UART5_BASE,                            //  模块基地址
  RCC_PERIPHERAL_UART5,                  //  外设总线属性，用于确定外设时钟
  ARCH_INT_UART5,                        //  中断序号
};
struct rt_serial_device __gSerial5;

void USART5_frame_ISR(void)
{
  stm32_serial_info *serial_info = &__gUart5Info;
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0001) {             //  如果是校验错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0002) {             //  如果是帧错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(!(HWREG32(serial_info->base + USART_CR1) & USART_INT_RX))  // 如果接收中断未使能
  {
    return;
  }
  
  /* enter interrupt */
  rt_interrupt_enter();
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0020)              //  如果是接受中断
  {
    rt_hw_serial_isr(&__gSerial5, RT_SERIAL_EVENT_RX_IND);
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0008)              //  如果是数据过载了
  {
    rt_hw_serial_isr(&__gSerial5, RT_SERIAL_EVENT_RX_IND);
    HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
  }
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif /* RT_USING_FRAME_UART5 */

#if defined(RT_USING_FRAME_UART6)
/* UART1 device driver structure */
stm32_serial_info __gUart6Info =
{
  USART6_BASE,                            //  模块基地址
  RCC_PERIPHERAL_USART6,                  //  外设总线属性，用于确定外设时钟
  ARCH_INT_USART6,                        //  中断序号
};
struct rt_serial_device __gSerial6;

void USART6_frame_ISR(void)
{
  stm32_serial_info *serial_info = &__gUart6Info;
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0001) {             //  如果是校验错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0002) {             //  如果是帧错误
     HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(!(HWREG32(serial_info->base + USART_CR1) & USART_INT_RX))  // 如果接收中断未使能
  {
    return;
  }
  
  /* enter interrupt */
  rt_interrupt_enter();
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0020)              //  如果是接受中断
  {
    rt_hw_serial_isr(&__gSerial6, RT_SERIAL_EVENT_RX_IND);
  }
  
  if(HWREG32(serial_info->base + USART_ISR) & 0x0008)              //  如果是数据过载了
  {
    rt_hw_serial_isr(&__gSerial6, RT_SERIAL_EVENT_RX_IND);
    HWREG32(serial_info->base + USART_ICR) = 0xFFFFFFFF;
  }
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif /* RT_USING_FRAME_UART6 */
#endif
/*********************************************************************************************************
** Function name:       rt_usart_frame_driver_init
** Descriptions:        串口驱动初始化函数，主要用于注册中断,注册设备驱动
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int rt_usart_frame_driver_init(void)
{
#if defined(RT_USING_FRAME_UART1) || defined(RT_USING_FRAME_UART2) || defined(RT_USING_FRAME_UART3) || \
defined(RT_USING_FRAME_UART4) || defined(RT_USING_FRAME_UART5) || defined(RT_USING_FRAME_UART6)
  stm32_serial_info *serial_info;
  struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
#endif
  
#ifdef RT_USING_FRAME_UART1
  serial_info = &__gUart1Info;
  
  config.baud_rate = 115200;
  __gSerial1.ops    = &stm32_uart_ops;
  __gSerial1.config = config;
  
  SystemPeripheralEnable(serial_info->u16WhichBus, serial_info->u32Ctrl);
  
  // 注册串口中断函数
  hal_int_register(serial_info->int_number, USART1_frame_ISR);
  hal_int_priority_set(serial_info->int_number,0x01 << INT_PRIORITY_BIT_START);
  hal_int_enable(serial_info->int_number);
  
  /* register UART1 device */
  rt_hw_serial_register(&__gSerial1, "ttyS0",
                        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                        serial_info);
#endif
  
#ifdef RT_USING_FRAME_UART2
  serial_info = &__gUart2Info;
  
  config.baud_rate = 115200;
  __gSerial2.ops    = &stm32_uart_ops;
  __gSerial2.config = config;
  
  SystemPeripheralEnable(serial_info->u16WhichBus, serial_info->u32Ctrl);
  
  // 注册串口中断函数
  hal_int_register(serial_info->int_number, USART2_frame_ISR);
  hal_int_priority_set(serial_info->int_number,0x01 << INT_PRIORITY_BIT_START);
  hal_int_enable(serial_info->int_number);
  
  /* register UART1 device */
  rt_hw_serial_register(&__gSerial2, "ttyS1",
                        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                        serial_info);
#endif

#ifdef RT_USING_FRAME_UART3
  serial_info = &__gUart3Info;
  
  config.baud_rate = 115200;
  __gSerial3.ops    = &stm32_uart_ops;
  __gSerial3.config = config;
  
  SystemPeripheralEnable(serial_info->u16WhichBus, serial_info->u32Ctrl);
  
  // 注册串口中断函数
  hal_int_register(serial_info->int_number, USART3_frame_ISR);
  hal_int_priority_set(serial_info->int_number,0x01 << INT_PRIORITY_BIT_START);
  hal_int_enable(serial_info->int_number);
  
  /* register UART1 device */
  rt_hw_serial_register(&__gSerial3, "ttyS2",
                        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                        serial_info);
#endif
  
#ifdef RT_USING_FRAME_UART4
  serial_info = &__gUart4Info;
  
  config.baud_rate = 115200;
  __gSerial4.ops    = &stm32_uart_ops;
  __gSerial4.config = config;
  
  SystemPeripheralEnable(serial_info->u16WhichBus, serial_info->u32Ctrl);
  
  // 注册串口中断函数
  hal_int_register(serial_info->int_number, USART4_frame_ISR);
  hal_int_priority_set(serial_info->int_number,0x01 << INT_PRIORITY_BIT_START);
  hal_int_enable(serial_info->int_number);
  
  /* register UART1 device */
  rt_hw_serial_register(&__gSerial4, "ttyS3",
                        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                        serial_info);
#endif
  
#ifdef RT_USING_FRAME_UART5
  serial_info = &__gUart5Info;
  
  config.baud_rate = 115200;
  __gSerial5.ops    = &stm32_uart_ops;
  __gSerial5.config = config;
  
  SystemPeripheralEnable(serial_info->u16WhichBus, serial_info->u32Ctrl);
  
  // 注册串口中断函数
  hal_int_register(serial_info->int_number, USART5_frame_ISR);
  hal_int_priority_set(serial_info->int_number,0x01 << INT_PRIORITY_BIT_START);
  hal_int_enable(serial_info->int_number);
  
  /* register UART1 device */
  rt_hw_serial_register(&__gSerial5, "ttyS4",
                        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                        serial_info);
#endif
  
#ifdef RT_USING_FRAME_UART6
  serial_info = &__gUart6Info;
  
  config.baud_rate = 115200;
  __gSerial6.ops    = &stm32_uart_ops;
  __gSerial6.config = config;
  
  SystemPeripheralEnable(serial_info->u16WhichBus, serial_info->u32Ctrl);
  
  // 注册串口中断函数
  hal_int_register(serial_info->int_number, USART6_frame_ISR);
  hal_int_priority_set(serial_info->int_number,0x01 << INT_PRIORITY_BIT_START);
  hal_int_enable(serial_info->int_number);
  
  /* register UART6 device */
  rt_hw_serial_register(&__gSerial6, "ttyS5",
                        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                        serial_info);
#endif
  
  return 0;
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_BOARD_EXPORT(rt_usart_frame_driver_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
