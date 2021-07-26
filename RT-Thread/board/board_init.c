/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_init.c
** Last modified Date:  2014-12-23
** Last Version:        v1.00
** Description:         目标板初始化文件
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2014-12-23
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

#include "hal/cortexm/hal_cortexm_reg.h"
#include "hal/cortexm/hal_cortexm_api.h"

#include "arch/inc/arch_io_memmap.h"
#include "arch/inc/arch_io_reg.h"
#include "arch/inc/arch_io_rcc.h"
#include "arch/inc/arch_io_gpio.h"
#include "arch/inc/arch_io_syscfg.h"

#include "board_init.h"

/*********************************************************************************************************
  链接器中的一个段声明
*********************************************************************************************************/
#pragma section="HEAP"

/*********************************************************************************************************
  全局变量声明 
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       SysTick_ISR
** Descriptions:        系统Tick中断，用于操作系统的tick
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void SysTick_ISR(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

/*********************************************************************************************************
** Function name:       rt_hw_us_delay
** Descriptions:        us级别的延时
** input parameters:    us:  us参数
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
volatile void usdelay(rt_uint32_t u32Val)
{
  __asm("    subs    r0, #1\n"
        "    bne.n   usdelay\n"
        "    bx      lr");
}
void rt_hw_us_delay(rt_uint32_t us)
{
#if 0
  rt_uint32_t temp;
  
  temp = SystemSysClockGet();
  usdelay(((temp - 1) * us) / (2 * 1000000));
  rt_hw_cpu_icache_ops(RT_HW_CACHE_INVALIDATE, RT_NULL, 0);
#else
  rt_uint32_t start, now, delta, reload, us_tick;
  start = HWREG32(HAL_SYSTICK_CURRENT);
  reload = HWREG32(HAL_SYSTICK_RELOAD);
  us_tick = SystemSysTickClockGet() / 1000000UL;
  do
  {
    now = HWREG32(HAL_SYSTICK_CURRENT);
    delta = start >= now ? start - now : reload + start - now;
  }
  while (delta < us_tick * us);
#endif
}

/*********************************************************************************************************
** Function name:       rt_hw_app_jump
** Descriptions:        应用程序跳转，主要用于BootLoader到应用的跳转，或应用程序之间的跳转
** input parameters:    u32AppAddr:  跳转的应用程序地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void rt_hw_app_jump(rt_uint32_t u32AppAddr)
{
  /**
  * Step1, 关闭中断
  */
  rt_hw_interrupt_disable();
  
  /**
  * Step2, 如果需要的话，复位除GPIO、SYSCFG、PWR和QSPI外的所有外设
  */ 
  //SystemPeripheralAllReset();
  HWREG32(RCC_BASE + RCC_AHB3RSTR) = 0x7FFFBFFF;
  HWREG32(RCC_BASE + RCC_AHB1RSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_AHB2RSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_AHB4RSTR) = 0xFFFF0000;
  HWREG32(RCC_BASE + RCC_APB3RSTR) = 0xEFFFFFFF;
  HWREG32(RCC_BASE + RCC_APB1LRSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_APB1HRSTR) = 0xEFFFFFFF;
  HWREG32(RCC_BASE + RCC_APB2RSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_APB4RSTR) = 0xEFFFFFFD;
  
  HWREG32(RCC_BASE + RCC_AHB3RSTR) = 0;
  HWREG32(RCC_BASE + RCC_AHB1RSTR) = 0;
  HWREG32(RCC_BASE + RCC_AHB2RSTR) = 0;
  HWREG32(RCC_BASE + RCC_AHB4RSTR) = 0;
  HWREG32(RCC_BASE + RCC_APB3RSTR) = 0;
  HWREG32(RCC_BASE + RCC_APB1LRSTR) = 0;
  HWREG32(RCC_BASE + RCC_APB1HRSTR) = 0;
  HWREG32(RCC_BASE + RCC_APB2RSTR) = 0;
  HWREG32(RCC_BASE + RCC_APB4RSTR) = 0;
  
  /**
  * Step3, 执行跳转
  */
  rt_hw_cpu_icache_disable();
  rt_hw_cpu_dcache_disable();
  HWREG32(HAL_ACCESS_CACR) &= ~(((rt_uint32_t)1) << 2); //  禁止D-CACHE透写
  hal_app_jump(u32AppAddr);
}

/*********************************************************************************************************
** Function name:       rt_hw_board_init
** Descriptions:        初始化目标主板.主要完成的工作有：
**                      1.初始化系统主频
**                      2.初始化系统tick
**                      3.初始化系统堆空间
**                      4.初始化shell串口驱动
**                      5.设定控制台串口
**                      其它外设驱动注册都在组件层
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void rt_hw_board_init(void)
{
  uint32_t u32Temp;
  
  /**
  * Step1, 初始化处理器及系统时钟
  */
#if __ARMVFP__
  /* Enable access to Floating-point coprocessor.         */
  HWREG32(HAL_FPU_CPACR) |= (((rt_uint32_t)0x0F) << 20);
#endif
  rt_hw_cpu_icache_enable();
  rt_hw_cpu_dcache_enable();
  //HWREG32(HAL_ACCESS_CACR) |= (((rt_uint32_t)1) << 2); //  使能D-CACHE透写
  HWREG32(HAL_ACCESS_CACR) &= ~(((rt_uint32_t)1) << 2); //  禁止D-CACHE透写
  hal_int_priority_group_get(INT_NUM_PRIORITY_BITS);
  
  SystemClockInit();
  
  /**
  * Step2, 使能高速IO的配置
  */
  SystemPeripheralEnable(RCC_PERIPHERAL_SYSCFG);
  SyscfgCompensationCellCtrl(RT_TRUE);
  
  /*
  ** Step 3, 初始化系统tick定时器
  */
  hal_int_systick_init(1, SystemSysTickClockGet(), RT_TICK_PER_SECOND, 0xFF, SysTick_ISR);
  
//  /*
//  ** Step 4, 注册一个tick的hook函数，用于节省功耗
//  */
//  rt_thread_idle_sethook(hal_enter_sleep);
  
  /**
  * Step5, 初始化板级驱动组件
  */
#ifdef RT_USING_COMPONENTS_INIT
  rt_components_board_init();
#endif
  
  /**
  * Step6, 初始化堆空间
  */
#ifdef RT_USING_MEMHEAP_AS_HEAP
  rt_system_heap_init((void *)STM32_HEAP_START, (void*)STM32_HEAP_END);
  {
    static struct rt_memheap sram1_2_memheap;
    rt_memheap_init(&sram1_2_memheap, "sram1_2", (void*)0x30000000, 1024 * 256);
    
  }
  {
    static struct rt_memheap sram4_memheap;
    rt_memheap_init(&sram4_memheap, "sram4", (void*)0x38000000, 1024*64);
    
  }
//  {
//    static struct rt_memheap extram_memheap;
//    rt_memheap_init(&extram_memheap, "sdram", (void*)0x80000000, 1024*1024*32);
//  }
  //#if STM32_HEAP_RAM
  //  {
  //    static struct rt_memheap ccm_memheap;
  //    rt_memheap_init(&ccm_memheap, "ccm", (void*)STM32_HEAP_RAM_START, STM32_HEAP_RAM_END - STM32_HEAP_RAM_START);
  //  }
  //#endif
#else
#ifdef RT_USING_HEAP
  /* init memory system */
  rt_system_heap_init((void *)STM32_HEAP_START, (void*)STM32_HEAP_END);
#endif /* RT_USING_HEAP */
#endif /* RT_USING_MEMHEAP_AS_HEAP */
  
  /**
  * Step7, 设置控制台及shell的串口
  */
#ifdef RT_USING_CONSOLE
  rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
}

/*********************************************************************************************************
** Function name:       watchdog_feed_timer
** Descriptions:        看门狗喂狗
** input parameters:    parg: 定时器传入参数
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void watchdog_feed(void);
static void watchdog_feed_timer(void* parg)
{
    watchdog_feed();
}

/*********************************************************************************************************
** Function name:       watchdog_feed
** Descriptions:        看门狗喂狗
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static int watchdog_init(void)
{
//  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOA);
//  GPIOPinTypeGPIOOutput(GPIOA_BASE, GPIO_PIN_5);
//  GPIOPinToggleBit(GPIOA_BASE, GPIO_PIN_5);
//  {
//    rt_timer_t timer;
//    /* 创建定时器1 */
//    timer = rt_timer_create("wdog",
//                            watchdog_feed_timer,
//                            RT_NULL, RT_TICK_PER_SECOND + RT_TICK_PER_SECOND/3, RT_TIMER_FLAG_PERIODIC);
//    /* 启动定时器 */
//    if (timer != RT_NULL) rt_timer_start(timer);
//  }
  
//   iwdgAccessEnable(IWDG_BASE);                 //启动寄存器读写
//   iwdgPrescalerSet(IWDG_BASE, IWDG_DIV_256);   //32K时钟256分频
//   iwdgReloadSet(IWDG_BASE,256);                //计数器数值,2秒左右
//   iwdgReloadEnable(IWDG_BASE);                 //重启计数器
//   idwgEnable(IWDG_BASE);                       //启动看门狗
//  {
//    rt_timer_t timer;
//    /* 创建定时器1 */
//    timer = rt_timer_create("wdog",
//                            watchdog_feed_timer,
//                            RT_NULL, RT_TICK_PER_SECOND + RT_TICK_PER_SECOND/2, RT_TIMER_FLAG_PERIODIC);
//    /* 启动定时器 */
//    if (timer != RT_NULL) rt_timer_start(timer);
//  }

  return 0;
}
/*********************************************************************************************************
** Function name:       watchdog_feed
** Descriptions:        看门狗喂狗
** Input parameters:    None 无·
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
void watchdog_feed(void)
{
//  GPIOPinToggleBit(GPIOA_BASE, GPIO_PIN_5);
  //  iwdgReloadEnable(IWDG_BASE);
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_DEVICE_EXPORT(watchdog_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
