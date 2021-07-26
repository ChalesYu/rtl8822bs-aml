/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_init.c
** Last modified Date:  2014-12-23
** Last Version:        v1.00
** Description:         Ŀ����ʼ���ļ�
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
  �������е�һ��������
*********************************************************************************************************/
#pragma section="HEAP"

/*********************************************************************************************************
  ȫ�ֱ������� 
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       SysTick_ISR
** Descriptions:        ϵͳTick�жϣ����ڲ���ϵͳ��tick
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
** Descriptions:        us�������ʱ
** input parameters:    us:  us����
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
** Descriptions:        Ӧ�ó�����ת����Ҫ����BootLoader��Ӧ�õ���ת����Ӧ�ó���֮�����ת
** input parameters:    u32AppAddr:  ��ת��Ӧ�ó����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void rt_hw_app_jump(rt_uint32_t u32AppAddr)
{
  /**
  * Step1, �ر��ж�
  */
  rt_hw_interrupt_disable();
  
  /**
  * Step2, �����Ҫ�Ļ�����λ��GPIO��SYSCFG��PWR��QSPI�����������
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
  * Step3, ִ����ת
  */
  rt_hw_cpu_icache_disable();
  rt_hw_cpu_dcache_disable();
  HWREG32(HAL_ACCESS_CACR) &= ~(((rt_uint32_t)1) << 2); //  ��ֹD-CACHE͸д
  hal_app_jump(u32AppAddr);
}

/*********************************************************************************************************
** Function name:       rt_hw_board_init
** Descriptions:        ��ʼ��Ŀ������.��Ҫ��ɵĹ����У�
**                      1.��ʼ��ϵͳ��Ƶ
**                      2.��ʼ��ϵͳtick
**                      3.��ʼ��ϵͳ�ѿռ�
**                      4.��ʼ��shell��������
**                      5.�趨����̨����
**                      ������������ע�ᶼ�������
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void rt_hw_board_init(void)
{
  uint32_t u32Temp;
  
  /**
  * Step1, ��ʼ����������ϵͳʱ��
  */
#if __ARMVFP__
  /* Enable access to Floating-point coprocessor.         */
  HWREG32(HAL_FPU_CPACR) |= (((rt_uint32_t)0x0F) << 20);
#endif
  rt_hw_cpu_icache_enable();
  rt_hw_cpu_dcache_enable();
  //HWREG32(HAL_ACCESS_CACR) |= (((rt_uint32_t)1) << 2); //  ʹ��D-CACHE͸д
  HWREG32(HAL_ACCESS_CACR) &= ~(((rt_uint32_t)1) << 2); //  ��ֹD-CACHE͸д
  hal_int_priority_group_get(INT_NUM_PRIORITY_BITS);
  
  SystemClockInit();
  
  /**
  * Step2, ʹ�ܸ���IO������
  */
  SystemPeripheralEnable(RCC_PERIPHERAL_SYSCFG);
  SyscfgCompensationCellCtrl(RT_TRUE);
  
  /*
  ** Step 3, ��ʼ��ϵͳtick��ʱ��
  */
  hal_int_systick_init(1, SystemSysTickClockGet(), RT_TICK_PER_SECOND, 0xFF, SysTick_ISR);
  
//  /*
//  ** Step 4, ע��һ��tick��hook���������ڽ�ʡ����
//  */
//  rt_thread_idle_sethook(hal_enter_sleep);
  
  /**
  * Step5, ��ʼ���弶�������
  */
#ifdef RT_USING_COMPONENTS_INIT
  rt_components_board_init();
#endif
  
  /**
  * Step6, ��ʼ���ѿռ�
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
  * Step7, ���ÿ���̨��shell�Ĵ���
  */
#ifdef RT_USING_CONSOLE
  rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
}

/*********************************************************************************************************
** Function name:       watchdog_feed_timer
** Descriptions:        ���Ź�ι��
** input parameters:    parg: ��ʱ���������
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
** Descriptions:        ���Ź�ι��
** Input parameters:    None ��
** Output parameters:   None ��
** Returned value:      None ��
*********************************************************************************************************/
static int watchdog_init(void)
{
//  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOA);
//  GPIOPinTypeGPIOOutput(GPIOA_BASE, GPIO_PIN_5);
//  GPIOPinToggleBit(GPIOA_BASE, GPIO_PIN_5);
//  {
//    rt_timer_t timer;
//    /* ������ʱ��1 */
//    timer = rt_timer_create("wdog",
//                            watchdog_feed_timer,
//                            RT_NULL, RT_TICK_PER_SECOND + RT_TICK_PER_SECOND/3, RT_TIMER_FLAG_PERIODIC);
//    /* ������ʱ�� */
//    if (timer != RT_NULL) rt_timer_start(timer);
//  }
  
//   iwdgAccessEnable(IWDG_BASE);                 //�����Ĵ�����д
//   iwdgPrescalerSet(IWDG_BASE, IWDG_DIV_256);   //32Kʱ��256��Ƶ
//   iwdgReloadSet(IWDG_BASE,256);                //��������ֵ,2������
//   iwdgReloadEnable(IWDG_BASE);                 //����������
//   idwgEnable(IWDG_BASE);                       //�������Ź�
//  {
//    rt_timer_t timer;
//    /* ������ʱ��1 */
//    timer = rt_timer_create("wdog",
//                            watchdog_feed_timer,
//                            RT_NULL, RT_TICK_PER_SECOND + RT_TICK_PER_SECOND/2, RT_TIMER_FLAG_PERIODIC);
//    /* ������ʱ�� */
//    if (timer != RT_NULL) rt_timer_start(timer);
//  }

  return 0;
}
/*********************************************************************************************************
** Function name:       watchdog_feed
** Descriptions:        ���Ź�ι��
** Input parameters:    None �ޡ�
** Output parameters:   None ��
** Returned value:      None ��
*********************************************************************************************************/
void watchdog_feed(void)
{
//  GPIOPinToggleBit(GPIOA_BASE, GPIO_PIN_5);
  //  iwdgReloadEnable(IWDG_BASE);
}

/*********************************************************************************************************
** �����Զ���ʼ������
*********************************************************************************************************/
INIT_DEVICE_EXPORT(watchdog_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
