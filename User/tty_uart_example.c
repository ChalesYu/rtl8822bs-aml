/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           tty_uart_example.c
** Last modified Date:  2019-05-01
** Last Version:        V1.00
** Description:         TTY串口测试例程
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2019-05-01
** Version:             V1.00
** Descriptions:
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
*********************************************************************************************************/
#include <stdio.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

/*********************************************************************************************************
  全局变量定义
*********************************************************************************************************/
typedef struct {
  int len;
  uint8_t buffer[1024];
}tty_handle_buffer;

rt_mailbox_t tty_mailbox;

/*********************************************************************************************************
** Function name:       tty_test_thread
** Descriptions:        串口测试线程
** input parameters:    parg：  线程传入参数
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void tty_send_thread(void *parg)
{
  tty_handle_buffer *tty_buffer;
  rt_device_t handle = parg;
  
  for(;;)
  {
    if(rt_mb_recv(tty_mailbox, (rt_ubase_t*)&tty_buffer, RT_WAITING_FOREVER) == RT_EOK) {
      if(tty_buffer != RT_NULL) {
        rt_device_write(handle, 0, tty_buffer->buffer, tty_buffer->len);
        rt_free(tty_buffer);
      }
    }
  }
}

/*********************************************************************************************************
** Function name:       tty_test_thread
** Descriptions:        串口测试线程
** input parameters:    parg：  线程传入参数
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void tty_test_thread(void *parg)
{
  const char *name = parg;
  tty_handle_buffer *tty_buffer;
  rt_device_t handle;
  rt_thread_t tid;
  uint32_t len;
  struct serial_configure cfg = RT_SERIAL_CONFIG_DEFAULT;
  
  tty_mailbox = rt_mb_create("tty_m", 8, RT_IPC_FLAG_FIFO);
  if(tty_mailbox == RT_NULL) {
    return;
  }

  handle = rt_device_find(name);
  if(handle == RT_NULL) {
    printf("Can't find %s\r\n", name);
    return;
  }
  rt_device_open(handle,RT_DEVICE_OFLAG_RDWR);
  cfg.baud_rate = 115200;
  cfg.parity = PARITY_ODD;
  rt_device_control(handle, RT_DEVICE_CTRL_CONFIG_SET, &cfg);
  len = 2000;
  rt_device_control(handle, RT_DEVICE_CRTL_TIMEOUT, &len);
  len = 100;
  rt_device_control(handle, RT_DEVICE_CTRL_INTERVAL, &len);
  
  {
    tid = rt_thread_create("tty_s", tty_send_thread, handle, 2048, 8, 20);
    
    if (tid != RT_NULL) {
      rt_thread_startup(tid);
    }
  }
  
  tty_buffer = RT_NULL;
  for(;;)
  {
    if(tty_buffer == RT_NULL) {
      tty_buffer = rt_malloc(sizeof(tty_handle_buffer));
    }
    
    if(tty_buffer == RT_NULL) {
      rt_thread_delay(2);
      continue;
    }
    
    tty_buffer->len = rt_device_read(handle, 0, tty_buffer->buffer, 1024);
    if(tty_buffer->len > 0) {
      rt_mb_send_wait(tty_mailbox, (rt_uint32_t)(tty_buffer), RT_WAITING_FOREVER);
      tty_buffer = RT_NULL;
    }
  }
}

/*********************************************************************************************************
** Function name:       tty_example_init
** Descriptions:        串口应用例子初始化
** input parameters:    name:  要打开的串口名字
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void tty_example_init(const char *name)
{
  rt_thread_t tid;

  tid = rt_thread_create("tty_r", tty_test_thread, (void *)name, 2048, 9, 20);

  if (tid != RT_NULL) {
    rt_thread_startup(tid);
  }
}


#ifdef FINSH_USING_MSH
#include <finsh.h>
int tty_test(int argc, char **argv)
{
  rt_thread_t tid;
  
  if (argc == 2)
  {
    tty_example_init(argv[1]);
    rt_kprintf("tty test thread create ok\r\n");
  } else {
    rt_kprintf("tty test thread create failed, eg: tty_test ttyS0\r\n");
  }
  
  
  return 0;
}
MSH_CMD_EXPORT(tty_test, create tty serial test example);
#endif /* FINSH_USING_MSH */

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
