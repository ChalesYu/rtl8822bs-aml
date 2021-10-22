/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_led.h
** Last modified Date:  2017-09-29
** Last Version:        v1.00
** Description:         LED指示灯驱动
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2017-09-29
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
#define RCC_LED_PERIPHERAL1      RCC_PERIPHERAL_GPIOC
//#define RCC_LED_PERIPHERAL2      RCC_PERIPHERAL_GPIOE
//#define RCC_LED_PERIPHERAL3      RCC_PERIPHERAL_GPIOC
//#define RCC_LED_PERIPHERAL4      RCC_PERIPHERAL_GPIOD
//#define RCC_LED_PERIPHERAL5      RCC_PERIPHERAL_GPIOE

struct tLedInfo {
  rt_uint32_t u32Base;
  rt_uint16_t u16Pin;
};

// IO连接改变请修改此结构的值
const struct tLedInfo __gtLedCtrlArray[] = //常量结构体数组，在定义的时候直接赋值
{
  {GPIOC_BASE, GPIO_PIN_0},
  {GPIOC_BASE, GPIO_PIN_2},
  {GPIOC_BASE, GPIO_PIN_3}
};
/*********************************************************************************************************
** Function name:       rt_hw_led_init
** Descriptions:        led控制初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
int rt_hw_led_init(void)
{
  rt_uint32_t num;
  rt_uint32_t i;
#ifdef RCC_LED_PERIPHERAL1
  SystemPeripheralEnable(RCC_LED_PERIPHERAL1);
#endif
  
#ifdef RCC_LED_PERIPHERAL2
  SystemPeripheralEnable(RCC_LED_PERIPHERAL2);
#endif

#ifdef RCC_LED_PERIPHERAL3
  SystemPeripheralEnable(RCC_LED_PERIPHERAL3);
#endif

#ifdef RCC_LED_PERIPHERAL4
  SystemPeripheralEnable(RCC_LED_PERIPHERAL4);
#endif
  
#ifdef RCC_LED_PERIPHERAL5
  SystemPeripheralEnable(RCC_LED_PERIPHERAL5);
#endif
  
  // 初始化IO
  num = sizeof(__gtLedCtrlArray) / sizeof(struct tLedInfo);
  for(i=0; i<num; i++) {
     GPIOPinTypeGPIOOutput(__gtLedCtrlArray[i].u32Base, __gtLedCtrlArray[i].u16Pin);
     GPIOPinSetBit(__gtLedCtrlArray[i].u32Base, __gtLedCtrlArray[i].u16Pin);
  }
     
  return 0;
}

/*********************************************************************************************************
** Function name:       rt_hw_relay_on
** Descriptions:        打开指定的继电器
** Input parameters:    relay: 指定的继电器
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
void rt_hw_led_on(rt_uint8_t led)
{
  rt_uint32_t num = sizeof(__gtLedCtrlArray) / sizeof(struct tLedInfo);
  if(led >= num) {
    return;
  }

  GPIOPinResetBit(__gtLedCtrlArray[led].u32Base, __gtLedCtrlArray[led].u16Pin);
}

/*********************************************************************************************************
** Function name:       rt_hw_relay_off
** Descriptions:        关闭指定的继电器
** Input parameters:    relay: 指定的继电器
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
void rt_hw_led_off(rt_uint8_t led)
{
  rt_uint32_t num = sizeof(__gtLedCtrlArray) / sizeof(struct tLedInfo);
  if(led >= num) {
    return;
  }
  
  GPIOPinSetBit(__gtLedCtrlArray[led].u32Base, __gtLedCtrlArray[led].u16Pin);
}

/*********************************************************************************************************
** Function name:       rt_hw_relay_toggle
** Descriptions:        翻转指定的继电器
** Input parameters:    relay: 指定的继电器
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
void rt_hw_led_toggle(rt_uint8_t led)
{
  rt_uint32_t num = sizeof(__gtLedCtrlArray) / sizeof(struct tLedInfo);
  if(led >= num) {
    return;
  }
  
  GPIOPinToggleBit(__gtLedCtrlArray[led].u32Base, __gtLedCtrlArray[led].u16Pin);
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_BOARD_EXPORT(rt_hw_led_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
