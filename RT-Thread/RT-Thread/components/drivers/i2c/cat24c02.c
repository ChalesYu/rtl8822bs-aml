/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.6lowpanworld.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           cat24c02.c
** Last modified Date:  2015-06-19
** Last Version:        V1.00
** Description:         cat24c02����������
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-06-19
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
  24C02����غ궨��
*********************************************************************************************************/
#define  I2C_24C02_ADDR         0x50    //  �����������ϵĵ�ַ
#define  I2C_DEVICE_ADDR_MAX    0xFF    //  �����ڲ��洢��Ԫ����ַ,������2Kλ����256���ֽڣ�����ַΪ0xFF
#define  I2C_24C02_PAGE_SEZE    16      //  ҳ��С

/*********************************************************************************************************
** �����豸�Ľṹ�嶨��
*********************************************************************************************************/
struct cat24c02_device
{
    /* inherit from rt_device */
    struct rt_device parent;

    /* i2c mode */
    struct rt_i2c_bus_device * i2c_device;
};

/*********************************************************************************************************
** ������������
*********************************************************************************************************/
struct cat24c02_device cat24c02;

/* RT-Thread device interface */

static rt_err_t cat24c02_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t cat24c02_open(rt_device_t dev, rt_uint16_t oflag)
{


    return RT_EOK;
}

static rt_err_t cat24c02_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t cat24c02_control(rt_device_t dev, int cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);


    return RT_EOK;
}

static rt_size_t cat24c02_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    struct rt_i2c_msg msg[2];
    rt_uint8_t send_buffer[1];

    RT_ASSERT(cat24c02.i2c_device != RT_NULL);
    
    if((pos + size - 1) > I2C_DEVICE_ADDR_MAX) {   //   ָ����ַ��ʣ���ֽ�������Ҫ���ȡ���ֽ���
       size = I2C_DEVICE_ADDR_MAX - pos + 1;     //   �������ȡ��ʣ����ֽ�
    }
    
    send_buffer[0] = (rt_uint8_t)(pos);
    msg[0].addr = I2C_24C02_ADDR;
    msg[0].flags = RT_I2C_WR;
    msg[0].len = 1;
    msg[0].buf = send_buffer;

    msg[1].addr = I2C_24C02_ADDR;
    msg[1].flags = RT_I2C_RD;
    msg[1].len = size;
    msg[1].buf = buffer;

    rt_i2c_transfer(cat24c02.i2c_device, msg, 2);
    
    
    return size;
}

static rt_size_t cat24c02_write_once(rt_device_t dev, rt_uint32_t addr, const void* buffer, rt_size_t size)
{
  rt_uint8_t send_buffer[1];
  struct rt_i2c_msg msg[2];
    
  send_buffer[0] = (rt_uint8_t)(addr);
  msg[0].addr = I2C_24C02_ADDR;
  msg[0].flags = RT_I2C_WR;
  msg[0].len = 1;
  msg[0].buf = send_buffer;
  
  msg[1].addr = I2C_24C02_ADDR;
  msg[1].flags = RT_I2C_NO_START | RT_I2C_WR;
  msg[1].len = size;
  msg[1].buf = (rt_uint8_t *)buffer;
  rt_i2c_transfer(cat24c02.i2c_device, msg, 2);
  
  return size;
}

static rt_size_t cat24c02_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    rt_uint32_t u32NumOnce = 0;
    rt_uint32_t u32Count;
    rt_uint32_t u32Addr = pos;
    rt_uint8_t *u8Data = (rt_uint8_t *)buffer;

    RT_ASSERT(cat24c02.i2c_device != RT_NULL);
    
    if((pos + size - 1) > I2C_DEVICE_ADDR_MAX) {   //   ָ����ַ��ʣ���ֽ�������Ҫ���ȡ���ֽ���
       size = I2C_DEVICE_ADDR_MAX - pos + 1;       //   �������ȡ��ʣ����ֽ�
    }
    
    u32Count = size;
    u32NumOnce = (I2C_24C02_PAGE_SEZE - (u32Addr % I2C_24C02_PAGE_SEZE));
    if(u32NumOnce > 0)                           //  ���д��ַû�а�ҳ���룬��д������ĵ�ַ
    {
        if(u32Count > u32NumOnce) {
            cat24c02_write_once(dev, u32Addr, u8Data, u32NumOnce);
            u8Data += u32NumOnce;
            u32Addr += u32NumOnce;
            u32Count -= u32NumOnce;
            rt_thread_delay(3);
        } else {
            cat24c02_write_once(dev, u32Addr, u8Data, size);
            rt_thread_delay(3);
            return size;
        }
    }
    while((u32Count / I2C_24C02_PAGE_SEZE) > 0)      //  ��ҳ����д����
    {
        u32NumOnce = I2C_24C02_PAGE_SEZE;
        cat24c02_write_once(dev, u32Addr, u8Data, u32NumOnce);
        u8Data += u32NumOnce;
        u32Addr += u32NumOnce;
        u32Count -= u32NumOnce;
        rt_thread_delay(3);
    }
    
    if(u32Count > 0)                              //  дʣ�²���һҳ������
    {
        cat24c02_write_once(dev, u32Addr, u8Data, u32Count);
        rt_thread_delay(3);
    }
    
    return size;
}

rt_err_t cat24c02_hw_init(const char *cat24c02_name, const char * i2c_bus_device_name)
{

    struct rt_i2c_bus_device * i2c_device;

    i2c_device = rt_i2c_bus_device_find(i2c_bus_device_name);
    if(i2c_device == RT_NULL)
    {
        rt_kprintf("i2c bus device %s not found!\r\n", i2c_bus_device_name);
        return -RT_ENOSYS;
    }
    cat24c02.i2c_device = i2c_device;

    cat24c02.parent.type = RT_Device_Class_Char;
    cat24c02.parent.rx_indicate = RT_NULL;
    cat24c02.parent.tx_complete = RT_NULL;
    cat24c02.parent.user_data   = &cat24c02;

    cat24c02.parent.control = cat24c02_control;
    cat24c02.parent.init    = cat24c02_init;
    cat24c02.parent.open    = cat24c02_open;
    cat24c02.parent.close   = cat24c02_close;
    cat24c02.parent.read    =  cat24c02_read;
    cat24c02.parent.write   = cat24c02_write;
    
     /* register the device */
    return rt_device_register(&cat24c02.parent, cat24c02_name, 
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

