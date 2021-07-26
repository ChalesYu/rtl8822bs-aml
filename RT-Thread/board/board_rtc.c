/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_rtc.c
** Last modified Date:  2017-02-10
** Last Version:        V1.00
** Description:         STM32L4的RTC驱动
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2017-02-10
** Version:             V1.00
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

#include <string.h>
#include <time.h>

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_reg.h"
/*********************************************************************************************************
  驱动配置
*********************************************************************************************************/
//#define RTC_CLOCK_SOURCE_LSI
#define RTC_CLOCK_SOURCE_LSE

#if (!defined(RTC_CLOCK_SOURCE_LSI)) && (!defined(RTC_CLOCK_SOURCE_LSE))
#error "必须选择一个rtc的时钟源"
#endif

#if (defined(RTC_CLOCK_SOURCE_LSI)) && (defined(RTC_CLOCK_SOURCE_LSE))
#error "只能选择一个rtc的时钟源"
#endif
/*********************************************************************************************************
  全局变量定义
*********************************************************************************************************/
static struct rt_device rtc;

/*********************************************************************************************************
  下面是标准的外设驱动框架函数实现
*********************************************************************************************************/
static rt_err_t rt_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
    if (dev->rx_indicate != RT_NULL)
    {
        /* Open Interrupt */
    }

    return RT_EOK;
}

static rt_size_t rt_rtc_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    return 0;
}

static rt_err_t rt_rtc_control(rt_device_t dev, int cmd, void *args)
{
    time_t *time;
    rt_uint32_t rtc_time, rtc_date;
    struct tm time_temp;

    RT_ASSERT(dev != RT_NULL);
    memset(&time_temp, 0, sizeof(struct tm));

    switch (cmd)
    {
    case RT_DEVICE_CTRL_RTC_GET_TIME:
        time = (time_t *)args;

        /* Get the current Time */
        rtc_time = HWREG32(RTC_BASE + RTC_TR);
        rtc_date = HWREG32(RTC_BASE + RTC_DR);
        /* Years since 1900 : 0-99 range */
        time_temp.tm_year = (((rtc_date >> 20) & 0x0F) * 10)  + ((rtc_date >> 16) & 0x0F) + 2000 - 1900;
        /* Months *since* january 0-11 : RTC_Month_Date_Definitions 1 - 12 */
        time_temp.tm_mon = (((rtc_date >> 12) & 0x01) * 10)  + ((rtc_date >> 8) & 0x0F) - 1;
        /* Day of the month 1-31 : 1-31 range */
        time_temp.tm_mday = (((rtc_date >> 4) & 0x03) * 10)  + ((rtc_date >> 0) & 0x0F);
        /* Hours since midnight 0-23 : 0-23 range */
        time_temp.tm_hour =(((rtc_time >> 20) & 0x03) * 10)  + ((rtc_time >> 16) & 0x0F);
        /* Minutes 0-59 : the 0-59 range */
        time_temp.tm_min = (((rtc_time >> 12) & 0x07) * 10)  + ((rtc_time >> 8) & 0x0F);
        /* Seconds 0-59 : the 0-59 range */
        time_temp.tm_sec = (((rtc_time >> 4) & 0x07) * 10)  + ((rtc_time >> 0) & 0x0F);

        *time = mktime(&time_temp);
        break;

    case RT_DEVICE_CTRL_RTC_SET_TIME:
    {
        struct tm* time_new;
        time = (time_t *)args;
        time_new = localtime(time);
        
        /* 0-99 range              : Years since 1900 */
        time_new->tm_year = time_new->tm_year + 1900 - 2000;
        time_new->tm_mon = time_new->tm_mon + 1;
        time_new->tm_wday = time_new->tm_wday + 1;
        rtc_date = ((time_new->tm_year / 10) << 20) | ((time_new->tm_year % 10) << 16) |
          ((time_new->tm_mon / 10) << 12) | ((time_new->tm_mon % 10) << 8) |
            ((time_new->tm_mday / 10) << 4) | ((time_new->tm_mday % 10) << 0) |
              (time_new->tm_wday << 13);
        rtc_time =  ((time_new->tm_hour / 10) << 20) | ((time_new->tm_hour % 10) << 16) |    
          ((time_new->tm_min / 10) << 12) | ((time_new->tm_min % 10) << 8) |   
            ((time_new->tm_sec / 10) << 4) | ((time_new->tm_sec % 10) << 0);
        
        HWREG32(RTC_BASE + RTC_ISR) |= 1<<7;                        //init mode 
        while(!(HWREG32(RTC_BASE + RTC_ISR) & 0x00000040));         //wait INITF to be 1
        // 配置时间和日期
        HWREG32(RTC_BASE + RTC_TR) = rtc_time;
        HWREG32(RTC_BASE + RTC_DR) = rtc_date;
        HWREG32(RTC_BASE + RTC_ISR) &= ~(1<<7);                     //退出init模式
        rt_thread_delay(rt_tick_from_millisecond(100));
    }
    break;
    }

    return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rt_hw_rtc_init
** Descriptions:        初始化并注册设备驱动程序
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
int rt_hw_rtc_init(void)
{
  //rt_uint32_t AsynchPrediv = 0, SynchPrediv = 0;
  
  // 允许写RTC及backup寄存器
  HWREG32(PWR_BASE + PWR_CR1) |= (((rt_uint32_t)1)<< 8);
  
#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
    // 使能LSI，等待振荡稳定
    HWREG32(RCC_BASE + RCC_CSR) |= (rt_uint32_t)0x00000001;
    while(!(HWREG32(RCC_BASE + RCC_CSR) & 0x00000002));
    
    /* Select the RTC Clock Source */
    HWREG32(RCC_BASE + RCC_BDCR) &= 0xFFFFFCFF;
    HWREG32(RCC_BASE + RCC_BDCR) |= (((rt_uint32_t)2)<< 8);
    
    //    SynchPrediv = 0xFF;
    //    AsynchPrediv = 0x7F;
    /* Enable the RTC Clock */
    HWREG32(RCC_BASE + RCC_BDCR) |= (((rt_uint32_t)1)<< 15);
    
    // 输入rtc模块写保护密码,密码是stm32规定的
    HWREG32(RTC_BASE + RTC_WPR) = 0xCA;
    HWREG32(RTC_BASE + RTC_WPR) = 0x53;
    
    // 配置预分频器
    HWREG32(RTC_BASE + RTC_ISR) |= 1<<7;                        //init mode 
    while(!(HWREG32(RTC_BASE + RTC_ISR) & 0x00000040));         //wait INITF to be 1
    //编辑分频器时,必须分成两次写入,先写入synchronous prescaler,再写入asynchronous prescaler.
    HWREG32(RTC_BASE + RTC_PRER) = 0x000000F9;
    HWREG32(RTC_BASE + RTC_PRER) = 0x007F00F9; 
    HWREG32(RTC_BASE + RTC_ISR) &= ~(1<<7);                     //退出init模式

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
    // 设置外部LSE高驱动能力
    HWREG32(RCC_BASE + RCC_BDCR) &= 0xFFFFFFE7;
    HWREG32(RCC_BASE + RCC_BDCR) |= (3UL << 3);
    // 如果LSE没有打开，则配置打开; 反之，如果以配置过，正在工作，则不需要再次配置。
    if(!(HWREG32(RCC_BASE + RCC_BDCR) & 0x00000002)) {
      HWREG32(RCC_BASE + RCC_BDCR) |= (rt_uint32_t)0x00000001;
      while(!(HWREG32(RCC_BASE + RCC_BDCR) & 0x00000002));
    }
    
    /* Select the RTC Clock Source */
    HWREG32(RCC_BASE + RCC_BDCR) &= 0xFFFFFCFF;
    HWREG32(RCC_BASE + RCC_BDCR) |= (((rt_uint32_t)1)<< 8);
    
    //      SynchPrediv = 0xFF;
    //      AsynchPrediv = 0x7F;
    /* Enable the RTC Clock */
    HWREG32(RCC_BASE + RCC_BDCR) |= (((rt_uint32_t)1)<< 15);
    
    // 输入rtc模块写保护密码,密码是stm32规定的
    HWREG32(RTC_BASE + RTC_WPR) = 0xCA;
    HWREG32(RTC_BASE + RTC_WPR) = 0x53;
    
    // 配置预分频器
    HWREG32(RTC_BASE + RTC_ISR) |= 1<<7;                        //init mode 
    while(!(HWREG32(RTC_BASE + RTC_ISR) & 0x00000040));         //wait INITF to be 1
    //编辑分频器时,必须分成两次写入,先写入synchronous prescaler,再写入asynchronous prescaler.
    HWREG32(RTC_BASE + RTC_PRER) = 0x000000FF;
    HWREG32(RTC_BASE + RTC_PRER) = 0x007F00FF; 
    HWREG32(RTC_BASE + RTC_ISR) &= ~(1<<7);                     //退出init模式

#endif /* RTC_CLOCK_SOURCE_LSI */
    
    /* register rtc device */
    rtc.type	= RT_Device_Class_RTC;
    rtc.init 	= RT_NULL;
    rtc.open 	= rt_rtc_open;
    rtc.close	= RT_NULL;
    rtc.read 	= rt_rtc_read;
    rtc.write	= RT_NULL;
    rtc.control = rt_rtc_control;

    /* no private */
    rtc.user_data = RT_NULL;

    rt_device_register(&rtc, "rtc", RT_DEVICE_FLAG_RDWR);

    return 0;
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_BOARD_EXPORT(rt_hw_rtc_init);

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
