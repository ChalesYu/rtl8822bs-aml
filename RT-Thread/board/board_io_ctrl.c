/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_io_ctrl.c
** Last modified Date:  2018-06-16
** Last Version:        v1.00
** Description:         IO控制驱动
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2018-06-16
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

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_gpio.h"

/*********************************************************************************************************
全局变量声明 
*********************************************************************************************************/
#define RCC_IO_CTRL_PERIPHERAL1      RCC_PERIPHERAL_GPIOB
//#define RCC_IO_CTRL_PERIPHERAL2      RCC_PERIPHERAL_IOPC
//#define RCC_IO_CTRLPERIPHERAL3      RCC_PERIPHERAL_IOPD
//#define RCC_IO_CTRL_PERIPHERAL4      RCC_PERIPHERAL_IOPE
//#define RCC_IO_CTRL_PERIPHERAL5      RCC_PERIPHERAL_IOPA

struct tIOCtrlInfo {
  rt_uint32_t u32Base;
  rt_uint16_t u16Pin;
};

// IO连接改变请修改此结构的值
const struct tIOCtrlInfo __gtIOCtrlArray[] = //常量结构体数组，在定义的时候直接赋值
{
  {GPIOB_BASE, GPIO_PIN_14},   // WIFI模块电源控制    PB14
};

/*********************************************************************************************************
** Function name:       rt_hw_io_ctrl_init
** Descriptions:        IO控制初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static int rt_hw_io_ctrl_init(void)
{
  rt_uint32_t num;
  rt_uint32_t i;
#ifdef RCC_IO_CTRL_PERIPHERAL1
  SystemPeripheralEnable(RCC_IO_CTRL_PERIPHERAL1);
#endif
  
#ifdef RCC_IO_CTRL_PERIPHERAL2
  SystemPeripheralEnable(RCC_IO_CTRL_PERIPHERAL2);
#endif

#ifdef RCC_IO_CTRL_PERIPHERAL3
  SystemPeripheralEnable(RCC_IO_CTRL_PERIPHERAL3);
#endif

#ifdef RCC_IO_CTRL_PERIPHERAL4
  SystemPeripheralEnable(RCC_IO_CTRL_PERIPHERAL4);
#endif
  
#ifdef RCC_IO_CTRL_PERIPHERAL5
  SystemPeripheralEnable(RCC_IO_CTRL_PERIPHERAL5);
#endif
  
  // 初始化IO
  num = sizeof(__gtIOCtrlArray) / sizeof(struct tIOCtrlInfo);
  for(i=0; i<num; i++) {
     GPIOPinTypeGPIOOutput(__gtIOCtrlArray[i].u32Base, __gtIOCtrlArray[i].u16Pin);
     GPIOPinSetBit(__gtIOCtrlArray[i].u32Base, __gtIOCtrlArray[i].u16Pin);
  }
  
  return 0;
}

/*********************************************************************************************************
** Function name:       rt_hw_io_ctrl_set
** Descriptions:        设置控制的IO为1
** Input parameters:    u8IO: 指定的IO
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
void rt_hw_io_ctrl_set(rt_uint8_t u8IO)
{
  rt_uint32_t num = sizeof(__gtIOCtrlArray) / sizeof(struct tIOCtrlInfo);
  if(u8IO >= num) {
    return;
  }

  GPIOPinSetBit(__gtIOCtrlArray[u8IO].u32Base, __gtIOCtrlArray[u8IO].u16Pin);
}

/*********************************************************************************************************
** Function name:       rt_hw_io_ctrl_reset
** Descriptions:        设置控制的IO为0
** Input parameters:    u8IO: 指定的IO
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
void rt_hw_io_ctrl_reset(rt_uint8_t u8IO)
{
  rt_uint32_t num = sizeof(__gtIOCtrlArray) / sizeof(struct tIOCtrlInfo);
  if(u8IO >= num) {
    return;
  }
  
  GPIOPinResetBit(__gtIOCtrlArray[u8IO].u32Base, __gtIOCtrlArray[u8IO].u16Pin);
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_BOARD_EXPORT(rt_hw_io_ctrl_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
