/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.6lowpanworld.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           at24c512.c
** Last modified Date:  2018-06-16
** Last Version:        V1.00
** Description:         AT24C512的驱动程序
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2018-06-16
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

/*********************************************************************************************************
  AT24C512的相关宏定义
*********************************************************************************************************/
#define  I2C_24C512_ADDR        0x50    //  器件在总线上的地址
#define  I2C_DEVICE_ADDR_MAX    0xFFFF  //  器件内部存储单元最大地址,器件共512K位，即65536个字节，最大地址为0xFFFF
#define  I2C_24C512_PAGE_SEZE   8       //  页大小

/*********************************************************************************************************
** 表征设备的结构体定义
*********************************************************************************************************/
struct at24c512_device
{
    /* inherit from rt_device */
    struct rt_device parent;

    /* i2c mode */
    struct rt_i2c_bus_device * i2c_device;
};

/*********************************************************************************************************
** 定义驱动变量
*********************************************************************************************************/
struct at24c512_device at24c512;

/* RT-Thread device interface */

static rt_err_t at24c512_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t at24c512_open(rt_device_t dev, rt_uint16_t oflag)
{


    return RT_EOK;
}

static rt_err_t at24c512_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t at24c512_control(rt_device_t dev, int cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);


    return RT_EOK;
}

static rt_size_t at24c512_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    struct rt_i2c_msg msg[2];
    rt_uint8_t send_buffer[2];

    RT_ASSERT(at24c512.i2c_device != RT_NULL);
    
    if((pos + size - 1) > I2C_DEVICE_ADDR_MAX) {   //   指定地址的剩余字节数不够要求读取的字节数
       size = I2C_DEVICE_ADDR_MAX - pos + 1;     //   则仅仅读取完剩余的字节
    }
    
    send_buffer[0] = (rt_uint8_t)((pos >> 8) & 0xFF);
    send_buffer[1] = (rt_uint8_t)(pos & 0xFF);
    msg[0].addr = I2C_24C512_ADDR;
    msg[0].flags = RT_I2C_WR;
    msg[0].len = 2;
    msg[0].buf = send_buffer;

    msg[1].addr = I2C_24C512_ADDR;
    msg[1].flags = RT_I2C_RD;
    msg[1].len = size;
    msg[1].buf = buffer;

    rt_i2c_transfer(at24c512.i2c_device, msg, 2);
    
    
    return size;
}

static rt_size_t at24c512_write_once(rt_device_t dev, rt_uint32_t addr, const void* buffer, rt_size_t size)
{
  rt_uint8_t send_buffer[2];
  struct rt_i2c_msg msg[2];
    
  send_buffer[0] = (rt_uint8_t)((addr >> 8) & 0xFF);
  send_buffer[1] = (rt_uint8_t)(addr & 0xFF);
  msg[0].addr = I2C_24C512_ADDR;
  msg[0].flags = RT_I2C_WR;
  msg[0].len = 2;
  msg[0].buf = send_buffer;
  
  msg[1].addr = I2C_24C512_ADDR;
  msg[1].flags = RT_I2C_NO_START | RT_I2C_WR;
  msg[1].len = size;
  msg[1].buf = (rt_uint8_t *)buffer;
  rt_i2c_transfer(at24c512.i2c_device, msg, 2);
  
  return size;
}

static rt_size_t at24c512_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    rt_uint32_t u32NumOnce = 0;
    rt_uint32_t u32Count;
    rt_uint32_t u32Addr = pos;
    rt_uint8_t *u8Data = (rt_uint8_t *)buffer;

    RT_ASSERT(at24c512.i2c_device != RT_NULL);
    
    if((pos + size - 1) > I2C_DEVICE_ADDR_MAX) {   //   指定地址的剩余字节数不够要求读取的字节数
       size = I2C_DEVICE_ADDR_MAX - pos + 1;       //   则仅仅读取完剩余的字节
    }
    
    u32Count = size;
    u32NumOnce = (I2C_24C512_PAGE_SEZE - (u32Addr % I2C_24C512_PAGE_SEZE));
    if(u32NumOnce > 0)                           //  如果写地址没有按页对齐，则写到对齐的地址
    {
        if(u32Count > u32NumOnce) {
            at24c512_write_once(dev, u32Addr, u8Data, u32NumOnce);
            u8Data += u32NumOnce;
            u32Addr += u32NumOnce;
            u32Count -= u32NumOnce;
            rt_thread_delay(3);
        } else {
            at24c512_write_once(dev, u32Addr, u8Data, size);
            rt_thread_delay(3);
            return size;
        }
    }
    while((u32Count / I2C_24C512_PAGE_SEZE) > 0)      //  按页对齐写数据
    {
        u32NumOnce = I2C_24C512_PAGE_SEZE;
        at24c512_write_once(dev, u32Addr, u8Data, u32NumOnce);
        u8Data += u32NumOnce;
        u32Addr += u32NumOnce;
        u32Count -= u32NumOnce;
        rt_thread_delay(3);
    }
    
    if(u32Count > 0)                              //  写剩下不到一页的数据
    {
        at24c512_write_once(dev, u32Addr, u8Data, u32Count);
        rt_thread_delay(3);
    }
    
    return size;
}

rt_err_t at24c512_hw_init(const char *at24c512_name, const char * i2c_bus_device_name)
{

    struct rt_i2c_bus_device * i2c_device;

    i2c_device = rt_i2c_bus_device_find(i2c_bus_device_name);
    if(i2c_device == RT_NULL)
    {
        rt_kprintf("i2c bus device %s not found!\r\n", i2c_bus_device_name);
        return -RT_ENOSYS;
    }
    at24c512.i2c_device = i2c_device;

    at24c512.parent.type = RT_Device_Class_Char;
    at24c512.parent.rx_indicate = RT_NULL;
    at24c512.parent.tx_complete = RT_NULL;
    at24c512.parent.user_data   = &at24c512;

    at24c512.parent.control = at24c512_control;
    at24c512.parent.init    = at24c512_init;
    at24c512.parent.open    = at24c512_open;
    at24c512.parent.close   = at24c512_close;
    at24c512.parent.read    =  at24c512_read;
    at24c512.parent.write   = at24c512_write;
    
     /* register the device */
    return rt_device_register(&at24c512.parent, at24c512_name, 
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

