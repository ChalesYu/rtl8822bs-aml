/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.6lowpanworld.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_io_spi.c
** Last modified Date:  2015-04-25
** Last Version:        v1.00
** Description:         IO模拟spi主机的驱动程序
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-04-25
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
#include "board_io_spi.h"

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"

/*********************************************************************************************************
** SPI硬件连接配置
*********************************************************************************************************/
#ifdef RT_USING_IO_SPI1
#define IO_SPI1_GPIO_PERIPHERAL   RCC_PERIPHERAL_GPIOB
#define IO_SPI1_GPIO_PERIPHERAL1   RCC_PERIPHERAL_GPIOD
#define IO_SPI1_GPIO_PERIPHERAL2   RCC_PERIPHERAL_GPIOE
//#define IO_SPI1_GPIO_PERIPHERAL3   RCC_PERIPHERAL_GPIOB

#define IO_SPI1_CLK_GPIO_BASE     GPIOB_BASE
#define IO_SPI1_CLK_GPIO_PIN      GPIO_PIN_2
#define IO_SPI1_MOSI_GPIO_BASE    GPIOD_BASE
#define IO_SPI1_MOSI_GPIO_PIN     GPIO_PIN_11
#define IO_SPI1_MISO_GPIO_BASE    GPIOD_BASE
#define IO_SPI1_MISO_GPIO_PIN     GPIO_PIN_12

#define IO_SPI1_CLK_H()      (HWREG16(IO_SPI1_CLK_GPIO_BASE + GPIOx_BSRR_SET) = IO_SPI1_CLK_GPIO_PIN)     //  将时钟线CLK置高
#define IO_SPI1_CLK_L()      (HWREG16(IO_SPI1_CLK_GPIO_BASE + GPIOx_BSRR_RESET) = IO_SPI1_CLK_GPIO_PIN)   //  将时钟线CLK置低
#define IO_SPI1_MOSI_H()     (HWREG16(IO_SPI1_MOSI_GPIO_BASE + GPIOx_BSRR_SET) = IO_SPI1_MOSI_GPIO_PIN)   //  将时钟线MOSI置高
#define IO_SPI1_MOSI_L()     (HWREG16(IO_SPI1_MOSI_GPIO_BASE + GPIOx_BSRR_RESET) = IO_SPI1_MOSI_GPIO_PIN) //  将时钟线MOSI置低

// MISO为输入引脚
#define IO_SPI1_MISO_STATE() (HWREG32(IO_SPI1_MISO_GPIO_BASE + GPIOx_IDR) & IO_SPI1_MISO_GPIO_PIN)        //  读MISO线的状态
#endif


/*********************************************************************************************************
stm32下的spi特性结构
*********************************************************************************************************/
struct stm32_io_spi_bus
{
  struct rt_spi_bus parent;
};

/*********************************************************************************************************
** Function name:       configure
** Descriptions:        SPI配置
** Input parameters:    device: 	SPI设备描述符
** 			configure: 	配置参数
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration)
{
  return RT_EOK;
};

/*********************************************************************************************************
** Function name:       configure
** Descriptions:        SPI配置
** Input parameters:    device: 	SPI设备描述符
** 						configure: 	配置参数
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message)
{
  //struct rt_spi_configuration * config = &device->config;
  struct stm32_spi_cs * stm32_spi_cs = device->parent.user_data;
  rt_uint32_t size = message->length;
  
  /* take CS */
  if(message->cs_take)
  {
    HWREG16(stm32_spi_cs->gpio_base + GPIOx_BSRR_RESET) = stm32_spi_cs->gpio_pin;
  }
  {
    const rt_uint8_t * send_ptr = message->send_buf;
    rt_uint8_t * recv_ptr = message->recv_buf;
    rt_uint8_t i;
    while(size--) {
      rt_uint8_t data = 0xFF;
      rt_uint8_t u8Rtn = 0;
      if(send_ptr != RT_NULL) {
        data = *send_ptr++;
      }
      
      // 循环移出一个字节的数据
      for(i=0; i<8; i++)
      {
        u8Rtn <<= 1;
        IO_SPI1_CLK_L();
        if((data & 0x80) != 0)  {
          IO_SPI1_MOSI_H();
        } else {
          IO_SPI1_MOSI_L();
        }
        if(IO_SPI1_MISO_STATE() != 0)  {
          u8Rtn |= 0x01;
        }
        data <<= 1;
        IO_SPI1_CLK_H();
      }
      
      if(recv_ptr != RT_NULL) {
        *recv_ptr++ = u8Rtn;
      }
    }
  }
  
  /* release CS */
  if(message->cs_release)
  {
    HWREG16(stm32_spi_cs->gpio_base + GPIOx_BSRR_SET) = stm32_spi_cs->gpio_pin;
  }
  
  return message->length;
};

/*********************************************************************************************************
定义驱动注册结构
*********************************************************************************************************/
static struct rt_spi_ops stm32_io_spi1_ops =
{
  configure,
  xfer
};

#ifdef RT_USING_IO_SPI1
static struct stm32_io_spi_bus stm32_io_spi1_bus;
#endif /* #ifdef RT_USING_IO_SPI1 */
/*********************************************************************************************************
** Function name:       rt_hw_io_spi_init
** Descriptions:        SPI驱动初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
int rt_hw_io_spi_init(void)
{
  struct stm32_io_spi_bus * stm32_spi;
  
#ifdef RT_USING_IO_SPI1
  SystemPeripheralEnable(IO_SPI1_GPIO_PERIPHERAL);
  
#ifdef IO_SPI1_GPIO_PERIPHERAL1
  SystemPeripheralEnable(IO_SPI1_GPIO_PERIPHERAL1);
#endif
#ifdef IO_SPI1_GPIO_PERIPHERAL2
  SystemPeripheralEnable(IO_SPI1_GPIO_PERIPHERAL2);
#endif
#ifdef IO_SPI1_GPIO_PERIPHERAL3
  SystemPeripheralEnable(IO_SPI1_GPIO_PERIPHERAL3);
#endif
  
  GPIOPinTypeGPIOOutput(GPIOE_BASE, GPIO_PIN_2);
  GPIOPinTypeGPIOOutput(GPIOD_BASE, GPIO_PIN_13);
  GPIOPinSetBit(GPIOE_BASE, GPIO_PIN_2);
  GPIOPinSetBit(GPIOD_BASE, GPIO_PIN_13);
  
  GPIOPinTypeGPIOOutput(IO_SPI1_CLK_GPIO_BASE, IO_SPI1_CLK_GPIO_PIN);
  GPIOPinTypeGPIOOutput(IO_SPI1_MOSI_GPIO_BASE, IO_SPI1_MOSI_GPIO_PIN);
  GPIOPinTypeGPIOInput(IO_SPI1_MISO_GPIO_BASE, IO_SPI1_MISO_GPIO_PIN);
    
  IO_SPI1_CLK_H();
  IO_SPI1_MOSI_H();
  
#endif
  
#ifdef RT_USING_IO_SPI1
  stm32_spi = &stm32_io_spi1_bus;
  rt_spi_bus_register(&stm32_spi->parent, "iospi1", &stm32_io_spi1_ops);
#endif
  
   return 0;
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_BOARD_EXPORT(rt_hw_io_spi_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
