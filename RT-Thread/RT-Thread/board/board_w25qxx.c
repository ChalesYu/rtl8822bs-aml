/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_w25qxx.c
** Last modified Date:  2015-06-03
** Last Version:        v1.00
** Description:         w25qxx������
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-06-03
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <dfs_posix.h>
//#include <spi/spi_flash_w25qxx.h>
#include <spi/spi_flash_w25qxx_mtd.h>

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_gpio.h"

#include "board_spi.h"

/*********************************************************************************************************
Ӳ������
*********************************************************************************************************/
// �õ�IO����ѡ������ʹ��GPIO����ʱ��
#define SPI_FLASH_CTRL_PERIPHERAL       RCC_PERIPHERAL_GPIOB

// SPI_FLASH SELӲ������
#define SPI_FLASH_SEL_BASE              GPIOB_BASE
#define SPI_FLASH_SEL_PIN               GPIO_PIN_10

/*********************************************************************************************************
** Function name:       rt_hw_w25qxx_init
** Descriptions:        w25qxx������ʼ��
** Input parameters:    None ��
** Output parameters:   None ��
** Returned value:      None ��
*********************************************************************************************************/
static int rt_hw_w25qxx_init(void)
{
  /*
  ** Step 1, ���ȳ�ʼ���õ�������ʱ��
  */
  SystemPeripheralEnable(RCC_PERIPHERAL_SYSCFG);
  SystemPeripheralEnable(SPI_FLASH_CTRL_PERIPHERAL);

  /*
  ** Step 2, ѡ��SPI��cs���ţ�ע��SPI�豸
  */
  {
    static struct rt_spi_device spi_device;
    static struct stm32_spi_cs  spi_cs;
    
    GPIOPinTypeGPIOOutput(SPI_FLASH_SEL_BASE, SPI_FLASH_SEL_PIN);
    GPIOPinSetBit(SPI_FLASH_SEL_BASE, SPI_FLASH_SEL_PIN);
    spi_cs.gpio_base = SPI_FLASH_SEL_BASE;
    spi_cs.gpio_pin = SPI_FLASH_SEL_PIN;
    rt_spi_bus_attach_device(&spi_device, "iospi10", "iospi1", (void*)&spi_cs);
  }
  
 /*
  ** Step 3, ��ʼ��spi flash
  */
  //w25qxx_init("flash0", "spi10");
  w25qxx_mtd_init("flash0", "iospi10");
  
//    /*
//  ** Step 2, ѡ��SPI��cs���ţ�ע��SPI�豸
//  */
//  {
//    static struct rt_spi_device spi_device;
//    static struct stm32_spi_cs  spi_cs;
//    
//    GPIOPinTypeGPIOOutput(SPI_FLASH_SEL_BASE, SPI_FLASH_SEL_PIN);
//    GPIOPinSetBit(SPI_FLASH_SEL_BASE, SPI_FLASH_SEL_PIN);
//    spi_cs.gpio_base = SPI_FLASH_SEL_BASE;
//    spi_cs.gpio_pin = SPI_FLASH_SEL_PIN;
//    rt_spi_bus_attach_device(&spi_device, "iospi10", "iospi1", (void*)&spi_cs);
//  }
//  
// /*
//  ** Step 3, ��ʼ��spi flash
//  */
//  w25qxx_init("flash0", "iospi10");

  return 0;
}

/*********************************************************************************************************
** �ṩ������shell
*********************************************************************************************************/
#ifdef FINSH_USING_MSH
#include <finsh.h>

/*********************************************************************************************************
** Function name:       hex_file
** Descriptions:        ��ӡһ���ļ�
** Input parameters:    None ��
** Output parameters:   None ��
** Returned value:      None ��
*********************************************************************************************************/
void hex_file(int argc, char** argv)
{
  int fd;
  int iReadBytes;
  int i;
  uint8_t *buffer;
  
  if(argc != 2) {
    rt_kprintf("Param error, eg:hex_file /test.txt\r\n");
    return;
  }
  
  buffer = rt_malloc(32);
  if(buffer == RT_NULL) {
    rt_kprintf("Can't malloc memory for print file\r\n");
    return;
  }
  
  fd = open(argv[1], O_RDONLY, 0);
  if(fd < 0) 
  {
    rt_kprintf("Can't open files\r\n");
    rt_free(buffer);
    return;
  }
  
  do{
    iReadBytes = read(fd, buffer, 32);
    if(iReadBytes > 0) {
      for(i=0; i<iReadBytes; i++) {
         rt_kprintf("%02X ", buffer[i]);
      }
      rt_kprintf("\r\n");
    }
  }while(iReadBytes == 32);
  rt_kprintf("hex printf file %s, size is %d\r\n",argv[1], lseek(fd, 0L, SEEK_END));
  rt_free(buffer);
  close(fd);
}

MSH_CMD_EXPORT(hex_file, Printf a hex file: hex_file <filename>);
#endif   /* FINSH_USING_MSH */
  
/*********************************************************************************************************
** �����Զ���ʼ������
*********************************************************************************************************/
INIT_DEVICE_EXPORT(rt_hw_w25qxx_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
