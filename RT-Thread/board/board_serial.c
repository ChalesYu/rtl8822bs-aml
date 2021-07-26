/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_serial.c
** Last modified Date:  2014-12-13
** Last Version:        v1.00
** Description:         串口驱动总的初始化文件
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2014-12-13
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

#include "board_serial.h"
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"
#include "hal/arch/inc/arch_io_usart.h"


/*********************************************************************************************************
串口硬件连接配置
*********************************************************************************************************/
#if defined( RT_USING_ALL_DMA_UART1) || defined(RT_USING_FRAME_UART1)//这个是RS485
#define UART1_GPIO_PERIPHERAL    RCC_PERIPHERAL_GPIOA
//#define UART1_GPIO_PERIPHERAL1   RCC_PERIPHERAL_GPIOD
#define UART1_UART_PERIPHERAL    RCC_PERIPHERAL_USART1
#define UART1_GPIO_PIN_TX        GPIOA_BASE,GPIO_PIN_9
#define UART1_GPIO_PIN_RX        GPIOA_BASE,GPIO_PIN_10
#define UART1_GPIO_NO_TX         GPIOA_BASE,9
#define UART1_GPIO_NO_RX         GPIOA_BASE,10

//#define RS485_CTRL1_BASE          GPIOA_BASE
//#define RS485_CTRL1_PIN           GPIO_PIN_8
#endif

#if defined( RT_USING_ALL_DMA_UART2) || defined(RT_USING_FRAME_UART2)
#define UART2_GPIO_PERIPHERAL    RCC_PERIPHERAL_GPIOD
//#define UART2_GPIO_PERIPHERAL1   RCC_PERIPHERAL_GPIOD
#define UART2_UART_PERIPHERAL   RCC_PERIPHERAL_USART2
#define UART2_GPIO_PIN_TX       GPIOD_BASE,GPIO_PIN_5
#define UART2_GPIO_PIN_RX       GPIOD_BASE,GPIO_PIN_6
#define UART2_GPIO_NO_TX        GPIOD_BASE,5
#define UART2_GPIO_NO_RX        GPIOD_BASE,6
#endif

#if defined( RT_USING_ALL_DMA_UART3) || defined(RT_USING_FRAME_UART3)
#define UART3_GPIO_PERIPHERAL    RCC_PERIPHERAL_GPIOD
//#define UART3_GPIO_PERIPHERAL1   RCC_PERIPHERAL_GPIOD
#define UART3_UART_PERIPHERAL   RCC_PERIPHERAL_USART3
#define UART3_GPIO_PIN_TX       GPIOD_BASE,GPIO_PIN_8
#define UART3_GPIO_PIN_RX       GPIOD_BASE,GPIO_PIN_9
#define UART3_GPIO_NO_TX        GPIOD_BASE,8
#define UART3_GPIO_NO_RX        GPIOD_BASE,9
#endif

#if defined( RT_USING_ALL_DMA_UART4) || defined(RT_USING_FRAME_UART4)
#define UART4_GPIO_PERIPHERAL    RCC_PERIPHERAL_GPIOC
//#define UART4_GPIO_PERIPHERAL1   RCC_PERIPHERAL_GPIOD
#define UART4_UART_PERIPHERAL   RCC_PERIPHERAL_UART4
#define UART4_GPIO_PIN_TX       GPIOC_BASE,GPIO_PIN_10
#define UART4_GPIO_PIN_RX       GPIOC_BASE,GPIO_PIN_11
#define UART4_GPIO_NO_TX        GPIOC_BASE,10
#define UART4_GPIO_NO_RX        GPIOC_BASE,11
#endif

#if defined( RT_USING_ALL_DMA_UART5) || defined(RT_USING_FRAME_UART5)
#define UART5_GPIO_PERIPHERAL    RCC_PERIPHERAL_GPIOC
#define UART5_GPIO_PERIPHERAL1   RCC_PERIPHERAL_GPIOD
#define UART5_UART_PERIPHERAL   RCC_PERIPHERAL_UART5
#define UART5_GPIO_PIN_TX       GPIOC_BASE,GPIO_PIN_12
#define UART5_GPIO_PIN_RX       GPIOD_BASE,GPIO_PIN_2
#define UART5_GPIO_NO_TX        GPIOC_BASE,12
#define UART5_GPIO_NO_RX        GPIOD_BASE,2
#endif

#if defined( RT_USING_ALL_DMA_UART6) || defined(RT_USING_FRAME_UART6)
#define UART6_GPIO_PERIPHERAL    RCC_PERIPHERAL_GPIOC
//#define UART6_GPIO_PERIPHERAL1   RCC_PERIPHERAL_GPIOD
#define UART6_UART_PERIPHERAL   RCC_PERIPHERAL_USART6
#define UART6_GPIO_PIN_TX       GPIOC_BASE,GPIO_PIN_6
#define UART6_GPIO_PIN_RX       GPIOC_BASE,GPIO_PIN_7
#define UART6_GPIO_NO_TX        GPIOC_BASE,6
#define UART6_GPIO_NO_RX        GPIOC_BASE,7

#define RS485_CTRL2_BASE          GPIOC_BASE
#define RS485_CTRL2_PIN           GPIO_PIN_8
#endif

#if defined( RT_USING_ALL_DMA_UART7) || defined(RT_USING_FRAME_UART7)
#define UART7_GPIO_PERIPHERAL    RCC_PERIPHERAL_GPIOE
//#define UART7_GPIO_PERIPHERAL1   RCC_PERIPHERAL_GPIOD
#define UART7_UART_PERIPHERAL   RCC_PERIPHERAL_USART7
#define UART7_GPIO_PIN_TX       GPIOE_BASE,GPIO_PIN_6
#define UART7_GPIO_PIN_RX       GPIOE_BASE,GPIO_PIN_7
#define UART7_GPIO_NO_TX        GPIOE_BASE,6
#define UART7_GPIO_NO_RX        GPIOE_BASE,7
#endif

/*********************************************************************************************************
** Function name:       rt_hw_usart_init
** Descriptions:        串口驱动初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
int rt_hw_usart_init(void)
{
  SystemPeripheralEnable(RCC_PERIPHERAL_DMA1);
  SystemPeripheralEnable(RCC_PERIPHERAL_DMA2);
#if defined( RT_USING_ALL_DMA_UART1) || defined(RT_USING_FRAME_UART1)
  SystemPeripheralEnable(UART1_GPIO_PERIPHERAL);
#ifdef UART1_GPIO_PERIPHERAL1
  SystemPeripheralEnable(UART1_GPIO_PERIPHERAL1);
#endif
  SystemPeripheralEnable(UART1_UART_PERIPHERAL);
  
  GPIOPinAFConfig(UART1_GPIO_NO_TX, GPIO_AF7_USART1);
  GPIOPinAFConfig(UART1_GPIO_NO_RX, GPIO_AF7_USART1);
  
  // Configure The Usart GPIO
  GPIOModeSet(UART1_GPIO_PIN_TX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART1_GPIO_PIN_TX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART1_GPIO_PIN_TX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART1_GPIO_PIN_TX,GPIO_PUPD_UP);
  
  GPIOModeSet(UART1_GPIO_PIN_RX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART1_GPIO_PIN_RX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART1_GPIO_PIN_RX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART1_GPIO_PIN_RX,GPIO_PUPD_UP);
  
//  // 485方向控制初始化
//  GPIOPinTypeGPIOOutput(RS485_CTRL1_BASE, RS485_CTRL1_PIN);
//  GPIOPinResetBit(RS485_CTRL1_BASE, RS485_CTRL1_PIN);
#endif
  
#if defined( RT_USING_ALL_DMA_UART2) || defined(RT_USING_FRAME_UART2)
  SystemPeripheralEnable(UART2_GPIO_PERIPHERAL);
#ifdef UART2_GPIO_PERIPHERAL1
  SystemPeripheralEnable(UART2_GPIO_PERIPHERAL1);
#endif
  SystemPeripheralEnable(UART2_UART_PERIPHERAL);
  
  GPIOPinAFConfig(UART2_GPIO_NO_TX, GPIO_AF7_USART2);
  GPIOPinAFConfig(UART2_GPIO_NO_RX, GPIO_AF7_USART2);
  
  // Configure The Usart GPIO
  GPIOModeSet(UART2_GPIO_PIN_TX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART2_GPIO_PIN_TX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART2_GPIO_PIN_TX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART2_GPIO_PIN_TX,GPIO_PUPD_UP);
  
  GPIOModeSet(UART2_GPIO_PIN_RX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART2_GPIO_PIN_RX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART2_GPIO_PIN_RX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART2_GPIO_PIN_RX,GPIO_PUPD_UP);
#endif
  
#if defined( RT_USING_ALL_DMA_UART3) || defined(RT_USING_FRAME_UART3)
  SystemPeripheralEnable(UART3_GPIO_PERIPHERAL);
#ifdef UART3_GPIO_PERIPHERAL1
  SystemPeripheralEnable(UART3_GPIO_PERIPHERAL1);
#endif
  SystemPeripheralEnable(UART3_UART_PERIPHERAL);
  
  GPIOPinAFConfig(UART3_GPIO_NO_TX, GPIO_AF7_USART3);
  GPIOPinAFConfig(UART3_GPIO_NO_RX, GPIO_AF7_USART3);
  
  // Configure The Usart GPIO
  GPIOModeSet(UART3_GPIO_PIN_TX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART3_GPIO_PIN_TX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART3_GPIO_PIN_TX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART3_GPIO_PIN_TX,GPIO_PUPD_UP);
  
  GPIOModeSet(UART3_GPIO_PIN_RX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART3_GPIO_PIN_RX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART3_GPIO_PIN_RX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART3_GPIO_PIN_RX,GPIO_PUPD_UP);
#endif
  
#if defined( RT_USING_ALL_DMA_UART4) || defined(RT_USING_FRAME_UART4)
  SystemPeripheralEnable(UART4_GPIO_PERIPHERAL);
#ifdef UART4_GPIO_PERIPHERAL1
  SystemPeripheralEnable(UART4_GPIO_PERIPHERAL1);
#endif
  SystemPeripheralEnable(UART4_UART_PERIPHERAL);
  
  GPIOPinAFConfig(UART4_GPIO_NO_TX, GPIO_AF8_UART4);
  GPIOPinAFConfig(UART4_GPIO_NO_RX, GPIO_AF8_UART4);
  
  // Configure The Usart GPIO
  GPIOModeSet(UART4_GPIO_PIN_TX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART4_GPIO_PIN_TX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART4_GPIO_PIN_TX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART4_GPIO_PIN_TX,GPIO_PUPD_UP);
  
  GPIOModeSet(UART4_GPIO_PIN_RX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART4_GPIO_PIN_RX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART4_GPIO_PIN_RX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART4_GPIO_PIN_RX,GPIO_PUPD_UP);
#endif
  
#if defined( RT_USING_ALL_DMA_UART5) || defined(RT_USING_FRAME_UART5)
  SystemPeripheralEnable(UART5_GPIO_PERIPHERAL);
#ifdef UART5_GPIO_PERIPHERAL1
  SystemPeripheralEnable(UART5_GPIO_PERIPHERAL1);
#endif
  SystemPeripheralEnable(UART5_UART_PERIPHERAL);
  
  GPIOPinAFConfig(UART5_GPIO_NO_TX, GPIO_AF8_UART5);
  GPIOPinAFConfig(UART5_GPIO_NO_RX, GPIO_AF8_UART5);
  
  // Configure The Usart GPIO
  GPIOModeSet(UART5_GPIO_PIN_TX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART5_GPIO_PIN_TX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART5_GPIO_PIN_TX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART5_GPIO_PIN_TX,GPIO_PUPD_UP);
  
  GPIOModeSet(UART5_GPIO_PIN_RX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART5_GPIO_PIN_RX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART5_GPIO_PIN_RX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART5_GPIO_PIN_RX,GPIO_PUPD_UP);
#endif
  
#if defined( RT_USING_ALL_DMA_UART6) || defined(RT_USING_FRAME_UART6)
  SystemPeripheralEnable(UART6_GPIO_PERIPHERAL);
#ifdef UART6_GPIO_PERIPHERAL1
  SystemPeripheralEnable(UART6_GPIO_PERIPHERAL1);
#endif
  SystemPeripheralEnable(UART6_UART_PERIPHERAL);
  
  GPIOPinAFConfig(UART6_GPIO_NO_TX, GPIO_AF7_USART6);
  GPIOPinAFConfig(UART6_GPIO_NO_RX, GPIO_AF7_USART6);
  
  // Configure The Usart GPIO
  GPIOModeSet(UART6_GPIO_PIN_TX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART6_GPIO_PIN_TX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART6_GPIO_PIN_TX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART6_GPIO_PIN_TX,GPIO_PUPD_UP);
  
  GPIOModeSet(UART6_GPIO_PIN_RX,GPIO_MODE_AF);
  GPIOOutputTypeSet(UART6_GPIO_PIN_RX,GPIO_OUT_PP);
  GPIOOutputSpeedSet(UART6_GPIO_PIN_RX,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(UART6_GPIO_PIN_RX,GPIO_PUPD_UP);
  
  // 485方向控制初始化
  GPIOPinTypeGPIOOutput(RS485_CTRL2_BASE, RS485_CTRL2_PIN);
  GPIOPinResetBit(RS485_CTRL2_BASE, RS485_CTRL2_PIN);
#endif
  
  return 0;
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_BOARD_EXPORT(rt_hw_usart_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
