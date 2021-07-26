/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.6lowpanworld.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           pcf85163.c
** Last modified Date:  2015-06-20
** Last Version:        V1.00
** Description:         RTC时钟芯片pcf85163的驱动程序
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-06-20
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

/*********************************************************************************************************
  PCF85163的相关宏定义
*********************************************************************************************************/
#define  I2C_PCF85163_ADDR         0x51    //  器件在总线上的地址

/*********************************************************************************************************
** 表征设备的结构体定义
*********************************************************************************************************/
struct pcf85163_device
{
    /* inherit from rt_device */
    struct rt_device parent;

    /* i2c mode */
    struct rt_i2c_bus_device * i2c_device;
};

/*********************************************************************************************************
** 定义驱动变量
*********************************************************************************************************/
struct pcf85163_device pcf85163;

/* RT-Thread device interface */
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
    struct pcf85163_device *rtc = dev->user_data;
    struct tm time_temp;
    struct rt_i2c_msg msg[2];
    rt_uint8_t send_buffer[1];
    rt_uint8_t pTimeData[7] = {0};

    RT_ASSERT(dev != RT_NULL);
    memset(&time_temp, 0, sizeof(struct tm));

    switch (cmd)
    {
    case RT_DEVICE_CTRL_RTC_GET_TIME:
        time = (time_t *)args;
        
        /* Get the current Time */
        send_buffer[0] = 2;
        msg[0].addr = I2C_PCF85163_ADDR;
        msg[0].flags = RT_I2C_WR;
        msg[0].len = 1;
        msg[0].buf = send_buffer;
        
        msg[1].addr = I2C_PCF85163_ADDR;
        msg[1].flags = RT_I2C_RD;
        msg[1].len = 7;
        msg[1].buf = pTimeData;
        
        rt_i2c_transfer(rtc->i2c_device, msg, 2); 
        /* Years since 1900 : 0-99 range */
        time_temp.tm_year = ((pTimeData[6] >> 4) & 0x0F)*10 + (pTimeData[6] & 0x0F) + 2000 - 1900;
        /* Months *since* january 0-11 : RTC_Month_Date_Definitions 1 - 12 */
        time_temp.tm_mon = ((pTimeData[5] >> 4) & 0x01)*10 + (pTimeData[5] & 0x0F) - 1;
        /* Day of the month 1-31 : 1-31 range */
        time_temp.tm_mday = ((pTimeData[3] >> 4) & 0x03)*10 + (pTimeData[3] & 0x0F);
        /* Hours since midnight 0-23 : 0-23 range */
        time_temp.tm_hour = ((pTimeData[2] >> 4) & 0x03)*10 + (pTimeData[2] & 0x0F);
        /* Minutes 0-59 : the 0-59 range */
        time_temp.tm_min = ((pTimeData[1] >> 4) & 0x07)*10 + (pTimeData[1] & 0x0F);
        /* Seconds 0-59 : the 0-59 range */
        time_temp.tm_sec = ((pTimeData[0] >> 4) & 0x07)*10 + (pTimeData[0] & 0x0F);

        *time = mktime(&time_temp);
        break;

    case RT_DEVICE_CTRL_RTC_SET_TIME:
    {
        struct tm* time_new;
        time = (time_t *)args;
        time_new = localtime(time);
        
        // 将给定的时间换算成BCD码
        time_new->tm_year = time_new->tm_year + 1900 - 2000;
        pTimeData[6] = (((time_new->tm_year / 10) << 4) | (time_new->tm_year % 10));
        time_new->tm_mon = time_new->tm_mon + 1;
        pTimeData[5] = (((time_new->tm_mon / 10) << 4) | (time_new->tm_mon % 10));
        pTimeData[4] = time_new->tm_wday + 1;
        pTimeData[3] = (((time_new->tm_mday / 10) << 4) | (time_new->tm_mday % 10));
        pTimeData[2] = (((time_new->tm_hour / 10) << 4) | (time_new->tm_hour % 10));
        pTimeData[1] = (((time_new->tm_min / 10) << 4) | (time_new->tm_min % 10));
        pTimeData[0] = (((time_new->tm_sec / 10) << 4) | (time_new->tm_sec % 10));
        
        /* Set the current Time */
        send_buffer[0] = 2;
        msg[0].addr = I2C_PCF85163_ADDR;
        msg[0].flags = RT_I2C_WR;
        msg[0].len = 1;
        msg[0].buf = send_buffer;
        
        msg[1].addr = I2C_PCF85163_ADDR;
        msg[1].flags = RT_I2C_NO_START | RT_I2C_WR;;
        msg[1].len = 7;
        msg[1].buf = pTimeData;
        
        rt_i2c_transfer(rtc->i2c_device, msg, 2); 
    }
    break;
    }

    return RT_EOK;
}

rt_err_t pcf85163_rtc_init(const char * i2c_bus_device_name)
{

    struct rt_i2c_bus_device * i2c_device;

    i2c_device = rt_i2c_bus_device_find(i2c_bus_device_name);
    if(i2c_device == RT_NULL)
    {
        rt_kprintf("i2c bus device %s not found!\r\n", i2c_bus_device_name);
        return -RT_ENOSYS;
    }
    pcf85163.i2c_device = i2c_device;

    pcf85163.parent.type = RT_Device_Class_RTC;
    pcf85163.parent.rx_indicate = RT_NULL;
    pcf85163.parent.tx_complete = RT_NULL;
    
    
    pcf85163.parent.init    = RT_NULL;
    pcf85163.parent.open    = rt_rtc_open;
    pcf85163.parent.close   = RT_NULL;
    pcf85163.parent.read    = rt_rtc_read;
    pcf85163.parent.write   = RT_NULL;
    pcf85163.parent.control = rt_rtc_control;
    
    pcf85163.parent.user_data   = &pcf85163;

     /* register the device */
    return rt_device_register(&pcf85163.parent, "rtc", RT_DEVICE_FLAG_RDWR);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

