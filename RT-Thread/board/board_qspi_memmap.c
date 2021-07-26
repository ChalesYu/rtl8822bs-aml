/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_qspi_memmap.c
** Last modified Date:  2019-03-23
** Last Version:        v1.00
** Description:         QSPI存储器映射模式配置
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2019-03-23
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
#include <rtdevice.h>
#include <drivers/serial.h>
#include <drivers/spi.h>

#include "board_spi.h"

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"
#include "hal/arch/inc/arch_io_qspi.h"

/*********************************************************************************************************
** Function name:       rt_hw_qspi_init
** Descriptions:        QSPI驱动存储器映射初始化，使用FLASH W25Q16
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
int rt_hw_qspi_init(void)
{
  /*
  ** Step 1, 是能外设
  */
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOB);
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOD);
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOE);
  SystemPeripheralEnable(RCC_PERIPHERAL_QSPI);
  
  /*
  ** Step 2, 配置复用功能
  */
  GPIOPinAFConfig(GPIOB_BASE,2, GPIO_AF9_QSPI);
  GPIOPinAFConfig(GPIOB_BASE,10, GPIO_AF9_QSPI);
  GPIOPinAFConfig(GPIOD_BASE,11, GPIO_AF9_QSPI);
  GPIOPinAFConfig(GPIOD_BASE,12, GPIO_AF9_QSPI);
  GPIOPinAFConfig(GPIOD_BASE,13, GPIO_AF9_QSPI);
  GPIOPinAFConfig(GPIOE_BASE,2, GPIO_AF9_QSPI);
  
  /*
  ** Step 3, 配置GPIO
  */
 /* QUADSPI_CLK      PB2, AF9  */
  /* QUADSPI_BK1_NCS  PB10, AF9 */
  GPIOModeSet(GPIOB_BASE, GPIO_PIN_2 | GPIO_PIN_10, GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOB_BASE, GPIO_PIN_2 | GPIO_PIN_10,GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOB_BASE, GPIO_PIN_2 | GPIO_PIN_10,GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(GPIOB_BASE, GPIO_PIN_2 | GPIO_PIN_10,GPIO_PUPD_UP);
  
  /* QUADSPI_BK1_IO0  PD11, AF9 */
  /* QUADSPI_BK1_IO1  PD12, AF9 */
  /* QUADSPI_BK1_IO3  PD13, AF9 */
  GPIOModeSet(GPIOD_BASE, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOD_BASE, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOD_BASE, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(GPIOD_BASE, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_PUPD_UP);
  
  /* QUADSPI_BK1_IO2  PE2, AF9 */
  GPIOModeSet(GPIOE_BASE, GPIO_PIN_2, GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOE_BASE, GPIO_PIN_2, GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOE_BASE, GPIO_PIN_2, GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(GPIOE_BASE, GPIO_PIN_2, GPIO_PUPD_UP);
  
  /*
  ** Step 4, 配置QUAD
  */ 
  SystemPeripheralReset(RCC_PERIPHERAL_QSPI);
  QSPIInit(QUADSPI_BASE, QSPI_CLOCK_PRESCALER(2) | QSPI_AND_MATCH_MODE | QSPI_POLL_STOP_BY_DISABLE_QSPI |\
    QSPI_FIFO_THRESHOLD(1) | QSPI_SELECTED_FLASH1 | QSPI_DUAL_FLASH_DISABLE | QSPI_SAMPLE_NO_SHIFT | QSPI_TIMEOUT_DISABLE);
  QSPIEnable(QUADSPI_BASE);
  QSPIDeviceConfig(QUADSPI_BASE, QSPI_FLASH_MEMORY_SIZE(21) | QSPI_CS_HIGH_TIME(3) | QSPI_CKMODE_0);
  //QSPICommunicationConfig(QUADSPI_BASE,)
  //　先复位Flash
  HWREG32(QUADSPI_BASE + QUADSPI_CCR) = 0x00000166;
  rt_thread_delay(2);
  HWREG32(QUADSPI_BASE + QUADSPI_CCR) = 0x00000199;
  rt_thread_delay(2);
  
  //  设置映射模式
  HWREG32(QUADSPI_BASE + QUADSPI_CCR) = 0x0F10EDEB;
  return 0;
}


/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_PREV_EXPORT(rt_hw_qspi_init);


/*********************************************************************************************************
END FILE
*********************************************************************************************************/
