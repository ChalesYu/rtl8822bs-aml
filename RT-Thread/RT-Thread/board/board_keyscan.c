/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_keyscan.c
** Last modified Date:  2019-06-02
** Last Version:        v1.00
** Description:         按键中断驱动
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2019-06-02
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <string.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "hal/cortexm/hal_cortexm_api.h"
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_syscfg.h"
#include "hal/arch/inc/arch_io_exti.h"
#include "hal/arch/inc/arch_io_gpio.h"

/*********************************************************************************************************
** 按键接口定义
*********************************************************************************************************/
#define KEY_SCAN_PERIPH         RCC_PERIPHERAL_GPIOA    // 按键所在外设
#define KEY_SCAN_GPIO           GPIOA_BASE              // 按键所在GPIO基地址
#define KEY_SCAN_PIN            GPIO_PIN_4              // 按键所在引脚
#define KEY_SCAN_INT_NUM        ARCH_INT_EXTI4          // 按键中断编号
#define KEY_EXTI_GPIO_X         EXTI_GPIO_A             // 外部中断线组
#define KEY_EXTI_GPIO_LINE      4                      // 外部中断线编号


/*********************************************************************************************************
** Flash特性结构体定义
*********************************************************************************************************/
typedef struct {
    rt_sem_t   sem_keyscan;
}tStm32h7xx_keyscan_info;

/*********************************************************************************************************
** 驱动变量定义
*********************************************************************************************************/
static tStm32h7xx_keyscan_info __GtStm32h7xx_keyscan_info;


/*********************************************************************************************************
** Function name:       KeyScan_ISR
** Descriptions:        按键中断
** input parameters:    NONE
**                      NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void KeyScan_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  ExtiIntEventClear(KEY_EXTI_GPIO_LINE);
  rt_sem_release(__GtStm32h7xx_keyscan_info.sem_keyscan);
  
  /* leave interrupt */
  rt_interrupt_leave();
}

/*********************************************************************************************************
** Function name:       rhb_stm32_keyscan_init
** Descriptions:        初始化控制stm32f2xx keyscan驱动
** input parameters:    handle: 外设特性描述符
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static rt_err_t rhb_stm32_keyscan_init (rt_device_t dev)
{
    tStm32h7xx_keyscan_info *handle = dev->user_data;

    // 建立保护信号量
    handle->sem_keyscan = rt_sem_create("keyscan", 0, RT_IPC_FLAG_FIFO);
    
    // 初始化按键引脚
    SystemPeripheralEnable(RCC_PERIPHERAL_SYSCFG);
    SystemPeripheralEnable(KEY_SCAN_PERIPH);
    GPIOPinTypeGPIOInput(KEY_SCAN_GPIO, KEY_SCAN_PIN);

    // 初始化按键中断
    // 连接EXTI0到对应引脚
    SyscfgExtiLineConfig(KEY_EXTI_GPIO_X, KEY_EXTI_GPIO_LINE);
    // 配置EXTI
    ExtiIntEventRisingDisable(KEY_EXTI_GPIO_LINE);
    ExtiIntEventFallingEnable(KEY_EXTI_GPIO_LINE);
    ExtiEventDisable(KEY_EXTI_GPIO_LINE);
    ExtiIntEnable(KEY_EXTI_GPIO_LINE);
    
    // NVIC配置
    hal_int_register(KEY_SCAN_INT_NUM, KeyScan_ISR);
    hal_int_priority_set(KEY_SCAN_INT_NUM, 0x00);
    hal_int_enable(KEY_SCAN_INT_NUM);
    
    return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_keyscan_open
** Descriptions:        打开STM32片内Flash控制器
** input parameters:    dev:    设备驱动结构
**                      oflag： 打开按时
** output parameters:   NONE
** Returned value:      打开结果
*********************************************************************************************************/
static rt_err_t rhb_stm32_keyscan_open(rt_device_t dev, rt_uint16_t oflag)
{
  ExtiIntEnable(KEY_EXTI_GPIO_LINE);
  hal_int_enable(KEY_SCAN_INT_NUM);
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_keyscan_close
** Descriptions:        关闭片内Flash控制器
** input parameters:    dev: 设备驱动结构
** output parameters:   NONE
** Returned value:      关闭结果
*********************************************************************************************************/
static rt_err_t rhb_stm32_keyscan_close(rt_device_t dev)
{
  ExtiIntDisable(KEY_EXTI_GPIO_LINE);
  hal_int_disable(KEY_SCAN_INT_NUM);
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_keyscan_read
** Descriptions:        读按键状态，按键按下并谈起才返回，否则阻塞线程
** input parameters:    dev: 外设特性描述符
**                      pos:    数据偏移地址
**                      buffer: 读取数据存放地址
**                      size:  	 读取数据长度
** output parameters:   实际接收到的数据长度
*********************************************************************************************************/
static rt_size_t rhb_stm32_keyscan_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
  rt_uint32_t u32Count = 0;
  tStm32h7xx_keyscan_info *handle = dev->user_data;
  
  if((buffer == NULL) || (size <= 0)) {
    return 0;
  }
  
  // 等待按键事件发生
  rt_sem_take(handle->sem_keyscan, RT_WAITING_FOREVER);
  // 等待按键松开
  while(size--) {
    if(GPIOPinInputRead(KEY_SCAN_GPIO, KEY_SCAN_PIN) & KEY_SCAN_PIN) {
      break;
    }
    u32Count++;
    rt_thread_delay(1);
  }
  // 如果小于或等于1，说明是抖动，返回按键未被按下
  if(u32Count <= 1) {
    return 0;
  }
  
  // 返回按下的键，以及按下的时间长度，以ms为单位
  u32Count = u32Count * (1000 / RT_TICK_PER_SECOND);
  if (u32Count == 0) {
    u32Count = 1;
  }
  
  return u32Count;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_keyscan_write
** Descriptions:        写按键
** input parameters:    dev: 外设特性描述符
**                      pos:    数据偏移地址，对于串口，该值无效
**                      buffer: 发送数据存放地址
**                      size:  	 发送数据长度
** output parameters:   实际写入的数据长度
** 注意：写之前必须先擦除Flash
*********************************************************************************************************/
static rt_size_t rhb_stm32_keyscan_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
  return 0;
}

/*********************************************************************************************************
** Function name:       rrhb_stm32_keyscan_control
** Descriptions:        按键控制，擦除命令在这里实现
** input parameters:    dev: 	外设特性描述符
**                      cmd:	配置命令
**                      args: 	配置参数
** output parameters:   NONE
** Returned value:      控制结果
*********************************************************************************************************/
static rt_err_t rhb_stm32_keyscan_control (rt_device_t dev, int cmd, void *args)
{
  if(cmd != 0) {
    return RT_ERROR;
  }
  if(GPIOPinInputRead(KEY_SCAN_GPIO, KEY_SCAN_PIN) & KEY_SCAN_PIN) {
    *((rt_bool_t *)args) = RT_FALSE;
  } else {
    *((rt_bool_t *)args) = RT_TRUE;
  }
  
  return RT_EOK;
}

/*********************************************************************************************************
** 定义驱动结构，支持驱动到设备驱动中
*********************************************************************************************************/
static struct rt_device __gKeyScanDriver;

/*********************************************************************************************************
** Function name:       rt_stm32_keyscan_driver_init
** Descriptions:        stm32的GPIO按键驱动
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int rt_stm32_keyscan_driver_init(void)
{
  rt_device_t device;
  
  // 注册驱动
  device = &__gKeyScanDriver;
  device->type 		= RT_Device_Class_Char;  //RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_stm32_keyscan_init;
  device->open		= rhb_stm32_keyscan_open;
  device->close		= rhb_stm32_keyscan_close;
  device->read 		= rhb_stm32_keyscan_read;
  device->write 	= rhb_stm32_keyscan_write;
  device->control 	= rhb_stm32_keyscan_control;
  device->user_data	= (void *)&__GtStm32h7xx_keyscan_info;
  
  /* register a character device */
  rt_device_register(device, "keyscan", RT_DEVICE_FLAG_RDWR);
  
  return 0;
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_DEVICE_EXPORT(rt_stm32_keyscan_driver_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
