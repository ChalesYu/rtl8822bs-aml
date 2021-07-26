/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_flash.c
** Last modified Date:  2015-06-08
** Last Version:        v1.00
** Description:         片内Flash驱动
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2014-12-13
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
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_flash.h"

/*********************************************************************************************************
** Flash配置
*********************************************************************************************************/
#define FLASH_ADDR_BASE     0x08000000          // Flash起始地址
#define FLASH_SECTOR_NUM    16                   // 总的扇区个数
#define FLASH_MAX_ADDR      0x3FFFFF            // 共2M大小

/*********************************************************************************************************
** Flash特性结构体定义
*********************************************************************************************************/
typedef struct {
    rt_mutex_t flash_mutex;        // Flash保护互斥体，主要用于防止多线程同时操作Flash
}tStm32h7xx_flash_info;

/*********************************************************************************************************
** 扇区号定义
*********************************************************************************************************/
static const rt_uint16_t __GStmFlashSector[] = {
    FLASH_Sector_0,             // 共128K
    FLASH_Sector_1,             // 共128K
    FLASH_Sector_2,             // 共128K
    FLASH_Sector_3,             // 共128K
    FLASH_Sector_4,             // 共128K
    FLASH_Sector_5,             // 共128K
    FLASH_Sector_6,             // 共128K
    FLASH_Sector_7,             // 共128K
    FLASH_Sector_0,             // 共128K
    FLASH_Sector_1,             // 共128K
    FLASH_Sector_2,             // 共128K
    FLASH_Sector_3,             // 共128K
    FLASH_Sector_4,             // 共128K
    FLASH_Sector_5,             // 共128K
    FLASH_Sector_6,             // 共128K
    FLASH_Sector_7,             // 共128K
};
static tStm32h7xx_flash_info __GtStm32h7xx_flash_info;


/*********************************************************************************************************
** Function name:       rhb_stm32_flash_init
** Descriptions:        初始化控制stm32f2xx flash驱动
** input parameters:    handle: 外设特性描述符
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static rt_err_t rhb_stm32_flash_init (rt_device_t dev)
{
    tStm32h7xx_flash_info *flash_handle = dev->user_data;

    // 建立保护信号量
    flash_handle->flash_mutex = rt_mutex_create("flash", RT_IPC_FLAG_FIFO);

    return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_flash_open
** Descriptions:        打开STM32片内Flash控制器
** input parameters:    dev:    设备驱动结构
**                      oflag： 打开按时
** output parameters:   NONE
** Returned value:      打开结果
*********************************************************************************************************/
static rt_err_t rhb_stm32_flash_open(rt_device_t dev, rt_uint16_t oflag)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_flash_close
** Descriptions:        关闭片内Flash控制器
** input parameters:    dev: 设备驱动结构
** output parameters:   NONE
** Returned value:      关闭结果
*********************************************************************************************************/
static rt_err_t rhb_stm32_flash_close(rt_device_t dev)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_flash_read
** Descriptions:        读指定地址的数据
** input parameters:    dev: 外设特性描述符
**                      pos:    数据偏移地址
**                      buffer: 读取数据存放地址
**                      size:  	 读取数据长度
** output parameters:   实际接收到的数据长度
*********************************************************************************************************/
static rt_size_t rhb_stm32_flash_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
  tStm32h7xx_flash_info *flash_handle = dev->user_data;
  
  if((size == 0) || (pos < FLASH_ADDR_BASE) || (pos > (FLASH_ADDR_BASE + FLASH_MAX_ADDR))) {
     return 0;
  }
  
  rt_mutex_take(flash_handle->flash_mutex, 0);
  
  if((pos + size - 1) > (FLASH_ADDR_BASE + FLASH_MAX_ADDR)) {
    size = (FLASH_ADDR_BASE + FLASH_MAX_ADDR) - pos + 1;
  }
  memcpy(buffer, (rt_uint8_t *)pos, size);
  
  rt_mutex_release(flash_handle->flash_mutex);
  
  return size;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_flash_write
** Descriptions:        写Flash数据
** input parameters:    dev: 外设特性描述符
**                      pos:    数据偏移地址，对于串口，该值无效
**                      buffer: 发送数据存放地址
**                      size:  	 发送数据长度
** output parameters:   实际写入的数据长度
** 注意：写之前必须先擦除Flash
*********************************************************************************************************/
static rt_size_t rhb_stm32_flash_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
  tStm32h7xx_flash_info *flash_handle = dev->user_data;
  
  if((size == 0) || (pos < FLASH_ADDR_BASE) || (pos > (FLASH_ADDR_BASE + FLASH_MAX_ADDR))) {
     return 0;
  }
  
  if((pos + size - 1) > (FLASH_ADDR_BASE + FLASH_MAX_ADDR)) {
    size = (FLASH_ADDR_BASE + FLASH_MAX_ADDR) - pos + 1;
  }
  // 保证编程的的32位对齐
  size = ((size % 4) == 0)?(size):(size + 4 - (size % 4));
  
  rt_mutex_take(flash_handle->flash_mutex, 0);
  
  FlashUnlock();
  FlashProgram(pos, (rt_uint32_t)buffer, size);
  FlashLock();
  
  rt_mutex_release(flash_handle->flash_mutex);
  
  return size;
}

/*********************************************************************************************************
** Function name:       rrhb_stm32_flash_control
** Descriptions:        Flash控制，擦除命令在这里实现
** input parameters:    dev: 	外设特性描述符
**                      cmd:	配置命令
**                      args: 	配置参数
** output parameters:   NONE
** Returned value:      控制结果
*********************************************************************************************************/
static rt_err_t rhb_stm32_flash_control (rt_device_t dev, int cmd, void *args)
{
  rt_uint32_t sec_addr = *((rt_uint32_t *)args);
  tStm32h7xx_flash_info *flash_handle = dev->user_data;
  rt_bool_t result;
  
  
  if(sec_addr > (FLASH_SECTOR_NUM - 1)) {
     return RT_ERROR;
  }
  
  rt_mutex_take(flash_handle->flash_mutex, 0);
  
  FlashUnlock();
  if(sec_addr < 8) {
    result = FlashEraseSector(FLASH_BANK_1,__GStmFlashSector[sec_addr]);
  } else {
    result = FlashEraseSector(FLASH_BANK_2,__GStmFlashSector[sec_addr]);
  }
  FlashLock();
  
  rt_mutex_release(flash_handle->flash_mutex);
  if(result == RT_FALSE) {
     return RT_ERROR;
  }
  return RT_EOK;
}

/*********************************************************************************************************
** 定义驱动结构，支持驱动到设备驱动中
*********************************************************************************************************/
static struct rt_device __gChipFlashDriver;

/*********************************************************************************************************
** Function name:       rt_stm32_flash_driver_init
** Descriptions:        stm32片内Flash驱动
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int rt_stm32_flash_driver_init(void)
{
  rt_device_t device;
  
  // 注册驱动
  device = &__gChipFlashDriver;
  device->type 		= RT_Device_Class_Block;  //RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_stm32_flash_init;
  device->open		= rhb_stm32_flash_open;
  device->close		= rhb_stm32_flash_close;
  device->read 		= rhb_stm32_flash_read;
  device->write 	= rhb_stm32_flash_write;
  device->control 	= rhb_stm32_flash_control;
  device->user_data	= (void *)&__GtStm32h7xx_flash_info;
  
  /* register a character device */
  rt_device_register(device, "flash", RT_DEVICE_FLAG_RDWR);
  
  return 0;
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_DEVICE_EXPORT(rt_stm32_flash_driver_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
