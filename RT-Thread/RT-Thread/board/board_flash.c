/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_flash.c
** Last modified Date:  2015-06-08
** Last Version:        v1.00
** Description:         Ƭ��Flash����
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
** Flash����
*********************************************************************************************************/
#define FLASH_ADDR_BASE     0x08000000          // Flash��ʼ��ַ
#define FLASH_SECTOR_NUM    16                   // �ܵ���������
#define FLASH_MAX_ADDR      0x3FFFFF            // ��2M��С

/*********************************************************************************************************
** Flash���Խṹ�嶨��
*********************************************************************************************************/
typedef struct {
    rt_mutex_t flash_mutex;        // Flash���������壬��Ҫ���ڷ�ֹ���߳�ͬʱ����Flash
}tStm32h7xx_flash_info;

/*********************************************************************************************************
** �����Ŷ���
*********************************************************************************************************/
static const rt_uint16_t __GStmFlashSector[] = {
    FLASH_Sector_0,             // ��128K
    FLASH_Sector_1,             // ��128K
    FLASH_Sector_2,             // ��128K
    FLASH_Sector_3,             // ��128K
    FLASH_Sector_4,             // ��128K
    FLASH_Sector_5,             // ��128K
    FLASH_Sector_6,             // ��128K
    FLASH_Sector_7,             // ��128K
    FLASH_Sector_0,             // ��128K
    FLASH_Sector_1,             // ��128K
    FLASH_Sector_2,             // ��128K
    FLASH_Sector_3,             // ��128K
    FLASH_Sector_4,             // ��128K
    FLASH_Sector_5,             // ��128K
    FLASH_Sector_6,             // ��128K
    FLASH_Sector_7,             // ��128K
};
static tStm32h7xx_flash_info __GtStm32h7xx_flash_info;


/*********************************************************************************************************
** Function name:       rhb_stm32_flash_init
** Descriptions:        ��ʼ������stm32f2xx flash����
** input parameters:    handle: ��������������
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static rt_err_t rhb_stm32_flash_init (rt_device_t dev)
{
    tStm32h7xx_flash_info *flash_handle = dev->user_data;

    // ���������ź���
    flash_handle->flash_mutex = rt_mutex_create("flash", RT_IPC_FLAG_FIFO);

    return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_flash_open
** Descriptions:        ��STM32Ƭ��Flash������
** input parameters:    dev:    �豸�����ṹ
**                      oflag�� �򿪰�ʱ
** output parameters:   NONE
** Returned value:      �򿪽��
*********************************************************************************************************/
static rt_err_t rhb_stm32_flash_open(rt_device_t dev, rt_uint16_t oflag)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_flash_close
** Descriptions:        �ر�Ƭ��Flash������
** input parameters:    dev: �豸�����ṹ
** output parameters:   NONE
** Returned value:      �رս��
*********************************************************************************************************/
static rt_err_t rhb_stm32_flash_close(rt_device_t dev)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_flash_read
** Descriptions:        ��ָ����ַ������
** input parameters:    dev: ��������������
**                      pos:    ����ƫ�Ƶ�ַ
**                      buffer: ��ȡ���ݴ�ŵ�ַ
**                      size:  	 ��ȡ���ݳ���
** output parameters:   ʵ�ʽ��յ������ݳ���
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
** Descriptions:        дFlash����
** input parameters:    dev: ��������������
**                      pos:    ����ƫ�Ƶ�ַ�����ڴ��ڣ���ֵ��Ч
**                      buffer: �������ݴ�ŵ�ַ
**                      size:  	 �������ݳ���
** output parameters:   ʵ��д������ݳ���
** ע�⣺д֮ǰ�����Ȳ���Flash
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
  // ��֤��̵ĵ�32λ����
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
** Descriptions:        Flash���ƣ���������������ʵ��
** input parameters:    dev: 	��������������
**                      cmd:	��������
**                      args: 	���ò���
** output parameters:   NONE
** Returned value:      ���ƽ��
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
** ���������ṹ��֧���������豸������
*********************************************************************************************************/
static struct rt_device __gChipFlashDriver;

/*********************************************************************************************************
** Function name:       rt_stm32_flash_driver_init
** Descriptions:        stm32Ƭ��Flash����
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int rt_stm32_flash_driver_init(void)
{
  rt_device_t device;
  
  // ע������
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
** �����Զ���ʼ������
*********************************************************************************************************/
INIT_DEVICE_EXPORT(rt_stm32_flash_driver_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
