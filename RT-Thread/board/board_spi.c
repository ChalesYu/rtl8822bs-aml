/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_spi.c
** Last modified Date:  2019-03-18
** Last Version:        v1.00
** Description:         spi接口的驱动程序
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2019-03-18
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
#include "hal/arch/inc/arch_io_dmamux.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"
#include "hal/arch/inc/arch_io_spi.h"

/*********************************************************************************************************
** 驱动配置，是否使用DMA
*********************************************************************************************************/
#define RT_SPI_DMA_USING        0

/*********************************************************************************************************
** SPI硬件连接配置
*********************************************************************************************************/
#ifdef RT_USING_SPI1
// use PB3、PB4、PB5
#define SPI1_GPIO_PERIPHERAL   RCC_PERIPHERAL_GPIOB
//#define SPI1_GPIO_PERIPHERAL1  RCC_PERIPHERAL_GPIOB
#define SPI1_SPI_PERIPHERAL    RCC_PERIPHERAL_SPI1
#define SPI1_GPIO_PIN_CLK      GPIOB_BASE, GPIO_PIN_3
#define SPI1_GPIO_PIN_MISO     GPIOB_BASE, GPIO_PIN_4
#define SPI1_GPIO_PIN_MOSI     GPIOB_BASE, GPIO_PIN_5
#define SPI1_GPIO_NO_CLK       GPIOB_BASE, 3
#define SPI1_GPIO_NO_MISO      GPIOB_BASE, 4
#define SPI1_GPIO_NO_MOSI      GPIOB_BASE, 5

#define SPI1_TX_DMA_USE_NO     12, 38
#define SPI1_RX_DMA_USE_NO     13, 37
#define SPI1_TX_DMA_INT_NO     ARCH_INT_DMA2_STREAM4
#define SPI1_RX_DMA_INT_NO     ARCH_INT_DMA2_STREAM5
#endif

#ifdef RT_USING_SPI2
// use PC10、PC11、PC12
#define SPI2_GPIO_PERIPHERAL   RCC_PERIPHERAL_GPIOB
#define SPI2_SPI_PERIPHERAL    RCC_PERIPHERAL_SPI2
#define SPI2_GPIO_PIN_CLK      GPIOB_BASE, GPIO_PIN_10
#define SPI2_GPIO_PIN_MISO     GPIOB_BASE, GPIO_PIN_14
#define SPI2_GPIO_PIN_MOSI     GPIOB_BASE, GPIO_PIN_15
#define SPI2_GPIO_NO_CLK       GPIOB_BASE, 10
#define SPI2_GPIO_NO_MISO      GPIOB_BASE, 14
#define SPI2_GPIO_NO_MOSI      GPIOB_BASE, 15
#endif

#ifdef RT_USING_SPI3
// use PC10、PC11、PC12
#define SPI3_GPIO_PERIPHERAL   RCC_PERIPHERAL_GPIOC
#define SPI3_SPI_PERIPHERAL    RCC_PERIPHERAL_SPI3
#define SPI3_GPIO_PIN_CLK      GPIOC_BASE, GPIO_PIN_10
#define SPI3_GPIO_PIN_MISO     GPIOC_BASE, GPIO_PIN_11
#define SPI3_GPIO_PIN_MOSI     GPIOC_BASE, GPIO_PIN_12
#define SPI3_GPIO_NO_CLK       GPIOC_BASE, 10
#define SPI3_GPIO_NO_MISO      GPIOC_BASE, 11
#define SPI3_GPIO_NO_MOSI      GPIOC_BASE, 12
#endif

#ifdef RT_USING_SPI4
// use PE2、PE5、PE6
#define SPI4_GPIO_PERIPHERAL   RCC_PERIPHERAL_GPIOE
#define SPI4_SPI_PERIPHERAL    RCC_PERIPHERAL_SPI4
#define SPI4_GPIO_PIN_CLK      GPIOE_BASE, GPIO_PIN_2
#define SPI4_GPIO_PIN_MISO     GPIOE_BASE, GPIO_PIN_5
#define SPI4_GPIO_PIN_MOSI     GPIOE_BASE, GPIO_PIN_6
#define SPI4_GPIO_NO_CLK       GPIOE_BASE, 2
#define SPI4_GPIO_NO_MISO      GPIOE_BASE, 5
#define SPI4_GPIO_NO_MOSI      GPIOE_BASE, 6
#endif

#ifdef RT_USING_SPI5
// use PF7、PF8、PF9
#define SPI5_GPIO_PERIPHERAL   RCC_PERIPHERAL_GPIOF
#define SPI5_SPI_PERIPHERAL    RCC_PERIPHERAL_SPI5
#define SPI5_GPIO_PIN_CLK      GPIOF_BASE, GPIO_PIN_7
#define SPI5_GPIO_PIN_MISO     GPIOF_BASE, GPIO_PIN_8
#define SPI5_GPIO_PIN_MOSI     GPIOF_BASE, GPIO_PIN_9
#define SPI5_GPIO_NO_CLK       GPIOF_BASE, 7
#define SPI5_GPIO_NO_MISO      GPIOF_BASE, 8
#define SPI5_GPIO_NO_MOSI      GPIOF_BASE, 9
#endif

/*********************************************************************************************************
stm32下的spi特性结构
*********************************************************************************************************/
struct stm32_spi_bus
{
  struct rt_spi_bus parent;
  rt_uint16_t   u16WhichBus; rt_uint32_t u32Ctrl;       //  外设总线属性，用于确定外设时钟
  rt_uint32_t spi_base;                                 //  SPI基地址
#if  (RT_SPI_DMA_USING > 0)
  rt_uint32_t TxDMA_base;                               //  发送DMA基地址，可以为DMA1_BASE或DMA2_BASE
  rt_uint8_t  TxDMA_Stream;                             //  发送DMA Sream编号
  rt_uint32_t RxDMA_base;                 	        //  接收DMA基地址，可以为DMA1_BASE或DMA2_BASE
  rt_uint8_t  RxDMA_Stream;                	        //  接受DMA Sream编号
  struct rt_completion complete;                       //  数据完成阻塞
#endif
};

/*********************************************************************************************************
** Function name:       configure
** Descriptions:        SPI配置
** Input parameters:    device: 	SPI设备描述符
** 						configure: 	配置参数
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration)
{
  rt_uint32_t u32Config1 = 0;
  rt_uint32_t u32Config2 = 0;
  struct stm32_spi_bus * stm32_spi_bus = (struct stm32_spi_bus *)device->bus;
  
  /* data_width */
  if((configuration->data_width < 4) || (configuration->data_width > 32))
  {
    return RT_EIO;
  } else {
    u32Config1 = SPI_DATA_LEGNTH_BIT(configuration->data_width);
  }
  
  /* baudrate */
  {
    rt_uint32_t SPI_CLOCK;
    rt_uint32_t max_hz;
    
    max_hz = configuration->max_hz;
    SPI_CLOCK = SystemPeripheralClockGet(stm32_spi_bus->u16WhichBus, stm32_spi_bus->u32Ctrl);
    
    if(max_hz >= SPI_CLOCK/2)
    {
      u32Config1 |= SPI_Baud_Prescaler_2;
    }
    else if(max_hz >= SPI_CLOCK/4)
    {
      u32Config1 |= SPI_Baud_Prescaler_4;
    }
    else if(max_hz >= SPI_CLOCK/8)
    {
      u32Config1 |= SPI_Baud_Prescaler_8;
    }
    else if(max_hz >= SPI_CLOCK/16)
    {
      u32Config1 |= SPI_Baud_Prescaler_16;
    }
    else if(max_hz >= SPI_CLOCK/32)
    {
      u32Config1 |= SPI_Baud_Prescaler_32;
    }
    else if(max_hz >= SPI_CLOCK/64)
    {
      u32Config1 |= SPI_Baud_Prescaler_64;
    }
    else if(max_hz >= SPI_CLOCK/128)
    {
      u32Config1 |= SPI_Baud_Prescaler_128;
    }
    else
    {
      /*  min prescaler 256 */
      u32Config1 |= SPI_Baud_Prescaler_256;
    }
  } /* baudrate */
  
  u32Config2 = SPI_SSOE_DISABLE;
  /* CPOL */
  if(configuration->mode & RT_SPI_CPOL)
  {
    u32Config2 |= SPI_CPOL_HIGH;
  }
  else
  {
    u32Config2 |= SPI_CPOL_LOW;
  }
  
  /* CPHA */
  if(configuration->mode & RT_SPI_CPHA)
  {
    u32Config2 |= SPI_CPHA_2EDGE;
  }
  else
  {
    u32Config2 |= SPI_CPHA_1EDGE;
  }
  
  /* MSB or LSB */
  if(configuration->mode & RT_SPI_MSB)
  {
    u32Config2 |= SPI_FIRST_MSB;
  }
  else
  {
    u32Config2 |= SPI_FIRST_LSB;
  }
  
  u32Config2 |= SPI_MODE_MASTER;
  u32Config2 |= SPI_PROTOCOL_MOTOROLA;
  
  /* init SPI */
  SPIDisable(stm32_spi_bus->spi_base);
  SPIInit(stm32_spi_bus->spi_base, u32Config1, u32Config2);
  SPIEnable(stm32_spi_bus->spi_base);
  
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
  struct stm32_spi_bus * stm32_spi_bus = (struct stm32_spi_bus *)device->bus;
  struct rt_spi_configuration * config = &device->config;
  //SPI_TypeDef * SPI = stm32_spi_bus->SPI;
  struct stm32_spi_cs * stm32_spi_cs = device->parent.user_data;
  rt_uint32_t size = message->length;
  
  /* take CS */
  if(message->cs_take)
  {
    HWREG16(stm32_spi_cs->gpio_base + GPIOx_BSRR_RESET) = stm32_spi_cs->gpio_pin;
  }
#if  (RT_SPI_DMA_USING > 0)
  if((message->length > 32) && (config->data_width <= 8))
  {
      static rt_uint8_t spi_dummy = 0xFF;
      rt_uint8_t recv_temp;
      
      //　初始化阻塞compelete
      rt_completion_init(&stm32_spi_bus->complete);
//      HWREG32(stm32_spi_bus->spi_base + SPI_CR2) |= message->length;
      HWREG32(stm32_spi_bus->spi_base + SPI2S_CR1) |= 0x00000200;  // 启动传输
      
      // 先配置RX
      DMADisable(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream);
      if(message->recv_buf != RT_NULL) {
        DMAModeConfigSet(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream,
                         DMA_DIR_PeripheralToMemory |
                           DMA_PeripheralInc_Disable |
                             DMA_MemoryInc_Enable |
                               DMA_PeripheralDataSize_Byte |
                                 DMA_MemoryDataSize_Byte |
                                   DMA_Mode_Normal |
                                     DMA_Priority_High |
                                       DMA_MemoryBurst_Single |
                                         DMA_PeripheralBurst_Single);
        DMAAddrSet(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream,
                 (rt_uint32_t)(message->recv_buf),(stm32_spi_bus->spi_base + SPI2S_RXDR));
      } else {
        DMAModeConfigSet(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream,
                         DMA_DIR_PeripheralToMemory |
                           DMA_PeripheralInc_Disable |
                             DMA_MemoryInc_Disable |
                               DMA_PeripheralDataSize_Byte |
                                 DMA_MemoryDataSize_Byte |
                                   DMA_Mode_Normal |
                                     DMA_Priority_High |
                                       DMA_MemoryBurst_Single |
                                         DMA_PeripheralBurst_Single);
        DMAAddrSet(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream,
                   (rt_uint32_t)(&recv_temp),(stm32_spi_bus->spi_base + SPI2S_RXDR));
      }
      DMAFIFOConfigSet(stm32_spi_bus->RxDMA_base, stm32_spi_bus->RxDMA_Stream,
                       DMA_FIFOMode_Disable | DMA_FIFOThreshold_Full);
      DMAIntClear(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream, DMA_INT_STATUS_ALL);
      DMAIntEnable(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream, DMA_INT_CONFIG_USUAL);
      DMABufferSizeSet(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream,message->length);
      DMAEnable(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream);
      
      // 在配置TX
      DMADisable(stm32_spi_bus->TxDMA_base,stm32_spi_bus->TxDMA_Stream);
      if(message->send_buf != RT_NULL) {
        DMAModeConfigSet(stm32_spi_bus->TxDMA_base,stm32_spi_bus->TxDMA_Stream,
                         DMA_DIR_MemoryToPeripheral |
                           DMA_PeripheralInc_Disable |
                             DMA_MemoryInc_Enable |
                               DMA_PeripheralDataSize_Byte |
                                 DMA_MemoryDataSize_Byte |
                                   DMA_Mode_Normal |
                                     DMA_Priority_High |
                                       DMA_MemoryBurst_Single |
                                         DMA_PeripheralBurst_Single);
        DMAAddrSet(stm32_spi_bus->TxDMA_base,stm32_spi_bus->TxDMA_Stream,
             (rt_uint32_t)(message->send_buf),(stm32_spi_bus->spi_base + SPI2S_TXDR));
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, (void *)message->send_buf, message->length);
      } else {
        DMAModeConfigSet(stm32_spi_bus->TxDMA_base,stm32_spi_bus->TxDMA_Stream,
                         DMA_DIR_MemoryToPeripheral |
                           DMA_PeripheralInc_Disable |
                             DMA_MemoryInc_Disable |
                               DMA_PeripheralDataSize_Byte |
                                 DMA_MemoryDataSize_Byte |
                                   DMA_Mode_Normal |
                                     DMA_Priority_High |
                                       DMA_MemoryBurst_Single |
                                         DMA_PeripheralBurst_Single);
        DMAAddrSet(stm32_spi_bus->TxDMA_base,stm32_spi_bus->TxDMA_Stream,
             (rt_uint32_t)(&spi_dummy),(stm32_spi_bus->spi_base + SPI2S_TXDR));
  
      }
      DMAFIFOConfigSet(stm32_spi_bus->TxDMA_base, stm32_spi_bus->TxDMA_Stream,
                       DMA_FIFOMode_Disable | DMA_FIFOThreshold_Full);
      DMAIntClear(stm32_spi_bus->TxDMA_base,stm32_spi_bus->TxDMA_Stream, DMA_INT_STATUS_ALL);
      DMABufferSizeSet(stm32_spi_bus->TxDMA_base,stm32_spi_bus->TxDMA_Stream,message->length);
      DMAEnable(stm32_spi_bus->TxDMA_base,stm32_spi_bus->TxDMA_Stream);
      
      //  开启DMA传输
      SPI_I2S_DMAEnable(stm32_spi_bus->spi_base,SPI_I2S_DMA_RX | SPI_I2S_DMA_TX);
      
      //  等待传输完成
      rt_completion_wait(&stm32_spi_bus->complete,RT_WAITING_FOREVER);
      if(message->recv_buf != RT_NULL) {
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, message->recv_buf, message->length);
      }
      //while(DMACurrDataCounterGet(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream) != 0);
      DMAIntDisable(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream, DMA_INT_CONFIG_USUAL);
      SPI_I2S_DMADisable(stm32_spi_bus->spi_base,SPI_I2S_DMA_RX | SPI_I2S_DMA_TX);
  }
  else
#endif
  {
    if(config->data_width <= 8)
    {
      const rt_uint8_t * send_ptr = message->send_buf;
      rt_uint8_t * recv_ptr = message->recv_buf;
      
      while(size--)
      {
        rt_uint8_t data = 0xFF;
        
        if(send_ptr != RT_NULL)
        {
          data = *send_ptr++;
        }
        
//        HWREG32(stm32_spi_bus->spi_base + SPI2S_CR1) |= 0x00000001;
        HWREG32(stm32_spi_bus->spi_base + SPI2S_CR1) |= 0x00000200;  // 启动传输
        HWREG8(stm32_spi_bus->spi_base + SPI2S_TXDR) = data;
        // 等待接收寄存器不空
        while(!(HWREG32(stm32_spi_bus->spi_base + SPI2S_SR) & 0x01));
        data = HWREG8(stm32_spi_bus->spi_base + SPI2S_RXDR);
        HWREG32(stm32_spi_bus->spi_base + SPI2S_IFCR) |= (3 << 3);  //EOTC和TXTFC置1,清除EOT和TXTFC位 
//        HWREG32(stm32_spi_bus->spi_base + SPI2S_CR1) &= ~0x00000001;
        if(recv_ptr != RT_NULL)
        {
          *recv_ptr++ = data;
        }
      }
    }
    else if(config->data_width <= 16)
    {
      const rt_uint16_t * send_ptr = message->send_buf;
      rt_uint16_t * recv_ptr = message->recv_buf;
      
      while(size--)
      {
        rt_uint16_t data = 0xFFFF;
        
        if(send_ptr != RT_NULL)
        {
          data = *send_ptr++;
        }
        
//        HWREG32(stm32_spi_bus->spi_base + SPI2S_CR1) |= 0x00000001;
        HWREG32(stm32_spi_bus->spi_base + SPI2S_CR1) |= 0x00000200;  // 启动传输
        HWREG16(stm32_spi_bus->spi_base + SPI2S_TXDR) = data;
        // 等待接收寄存器不空
        while(!(HWREG32(stm32_spi_bus->spi_base + SPI2S_SR) & 0x01));
        data = HWREG16(stm32_spi_bus->spi_base + SPI2S_RXDR);
        HWREG32(stm32_spi_bus->spi_base + SPI2S_IFCR) |= (3 << 3);  //EOTC和TXTFC置1,清除EOT和TXTFC位 
//        HWREG32(stm32_spi_bus->spi_base + SPI2S_CR1) &= ~0x00000001;
        
        if(recv_ptr != RT_NULL)
        {
          *recv_ptr++ = data;
        }
      }
    }
    else if(config->data_width <= 32)
    {
      const rt_uint32_t * send_ptr = message->send_buf;
      rt_uint32_t * recv_ptr = message->recv_buf;
      
      while(size--)
      {
        rt_uint32_t data = 0xFFFFFFFF;
        
        if(send_ptr != RT_NULL)
        {
          data = *send_ptr++;
        }
        
//        HWREG32(stm32_spi_bus->spi_base + SPI2S_CR1) |= 0x00000001;
        HWREG32(stm32_spi_bus->spi_base + SPI2S_CR1) |= 0x00000200;  // 启动传输
        HWREG32(stm32_spi_bus->spi_base + SPI2S_TXDR) = data;
        // 等待接收寄存器不空
        while(!(HWREG32(stm32_spi_bus->spi_base + SPI2S_SR) & 0x01));
        data = HWREG32(stm32_spi_bus->spi_base + SPI2S_RXDR);
        HWREG32(stm32_spi_bus->spi_base + SPI2S_IFCR) |= (3 << 3);  //EOTC和TXTFC置1,清除EOT和TXTFC位 
//        HWREG32(stm32_spi_bus->spi_base + SPI2S_CR1) &= ~0x00000001;
        
        if(recv_ptr != RT_NULL)
        {
          *recv_ptr++ = data;
        }
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

#if  (RT_SPI_DMA_USING > 0)
/*********************************************************************************************************
** Function name:       rt_spi_rx_isr_handler
** Descriptions:        中断服务程序执行流程
** Input parameters:    stm32_spi_bus:  指定的SPI总线
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static void rt_spi_rx_isr_handler(struct stm32_spi_bus * stm32_spi_bus)
{
  rt_uint32_t u32Status;
  
  u32Status = DMAIntStatus(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream);
  DMAIntClear(stm32_spi_bus->RxDMA_base,stm32_spi_bus->RxDMA_Stream, u32Status);
  
  rt_completion_done(&stm32_spi_bus->complete);
}
#endif

/*********************************************************************************************************
定义驱动注册结构
*********************************************************************************************************/
static struct rt_spi_ops stm32_spi_ops =
{
  configure,
  xfer
};

#ifdef RT_USING_SPI1
static struct stm32_spi_bus stm32_spi_bus_1 = 
{
  {0},                                          //  父类
  SPI1_SPI_PERIPHERAL,                          //  外设总线属性，用于确定外设时钟
  SPI1_I2S1_BASE,                               //  SPI基地址
#if  (RT_SPI_DMA_USING > 0)
  DMA2_BASE,                                    //  发送DMA基地址，可以为DMA1_BASE或DMA2_BASE
  4,                                            //  发送DMA Sream编号
  DMA2_BASE,                                    //  接收DDMA基地址，可以为DMA1_BASE或DMA2_BASE
  5,                                            //  接收DDMA Sream编号
  {0}                                           //  完成阻塞
#endif
};
#endif /* #ifdef RT_USING_SPI1 */

#ifdef RT_USING_SPI2
static struct stm32_spi_bus stm32_spi_bus_2 = 
{
  {0},
  SPI2_SPI_PERIPHERAL,
  SPI2_I2S2_BASE
};
#endif /* #ifdef RT_USING_SPI2 */

#ifdef RT_USING_SPI3
static struct stm32_spi_bus stm32_spi_bus_3 = 
{
  {0},
  SPI3_SPI_PERIPHERAL,
  SPI3_I2S3_BASE
};
#endif /* #ifdef RT_USING_SPI3 */

#ifdef RT_USING_SPI4
static struct stm32_spi_bus stm32_spi_bus_4 = 
{
  {0},
  SPI4_SPI_PERIPHERAL,
  SPI4_BASE
};
#endif /* #ifdef RT_USING_SPI4 */

#ifdef RT_USING_SPI5
static struct stm32_spi_bus stm32_spi_bus_5 = 
{
  {0},
  SPI5_SPI_PERIPHERAL,
  SPI5_BASE
};
#endif /* #ifdef RT_USING_SPI3 */

#if  (RT_SPI_DMA_USING > 0)
#ifdef RT_USING_SPI1
static void SPI1_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rt_spi_rx_isr_handler(&stm32_spi_bus_1);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif

#ifdef RT_USING_SPI2
static void SPI2_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rt_spi_rx_isr_handler(&stm32_spi_bus_2);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif

#ifdef RT_USING_SPI3
static void SPI3_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rt_spi_rx_isr_handler(&stm32_spi_bus_3);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif

#ifdef RT_USING_SPI4
static void SPI4_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rt_spi_rx_isr_handler(&stm32_spi_bus_4);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif

#ifdef RT_USING_SPI5
static void SPI5_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rt_spi_rx_isr_handler(&stm32_spi_bus_5);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#endif

/*********************************************************************************************************
** Function name:       rt_hw_spi_init
** Descriptions:        SPI驱动初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
int rt_hw_spi_init(void)
{
  struct stm32_spi_bus * stm32_spi;

#if  (RT_SPI_DMA_USING > 0)
  SystemPeripheralEnable(RCC_PERIPHERAL_DMA1);
  SystemPeripheralEnable(RCC_PERIPHERAL_DMA2);
#endif
#ifdef RT_USING_SPI1
  SystemPeripheralEnable(SPI1_GPIO_PERIPHERAL);
  SystemPeripheralEnable(SPI1_SPI_PERIPHERAL);
  
#ifdef SPI1_GPIO_PERIPHERAL1
  SystemPeripheralEnable(SPI1_GPIO_PERIPHERAL1);
#endif
  
  SystemPeripheralReset(SPI1_SPI_PERIPHERAL);
  
  /* Configure the appropriate pins to be SSI instead of GPIO */
  GPIOPinAFConfig(SPI1_GPIO_NO_CLK, GPIO_AF_SPI1);
  GPIOPinAFConfig(SPI1_GPIO_NO_MISO, GPIO_AF_SPI1);
  GPIOPinAFConfig(SPI1_GPIO_NO_MOSI, GPIO_AF_SPI1);
  
  // clk
  GPIOModeSet(SPI1_GPIO_PIN_CLK, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI1_GPIO_PIN_CLK, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI1_GPIO_PIN_CLK, GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(SPI1_GPIO_PIN_CLK, GPIO_PUPD_UP);
  
  // miso
  GPIOModeSet(SPI1_GPIO_PIN_MISO, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI1_GPIO_PIN_MISO, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI1_GPIO_PIN_MISO, GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(SPI1_GPIO_PIN_MISO, GPIO_PUPD_UP);
  
  // mosi
  GPIOModeSet(SPI1_GPIO_PIN_MOSI, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI1_GPIO_PIN_MOSI, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI1_GPIO_PIN_MOSI, GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(SPI1_GPIO_PIN_MOSI, GPIO_PUPD_UP);
#endif
  
#ifdef RT_USING_SPI2
  SystemPeripheralEnable(SPI2_GPIO_PERIPHERAL);
  SystemPeripheralEnable(SPI2_SPI_PERIPHERAL);
  
#ifdef SPI2_GPIO_PERIPHERAL1
  SystemPeripheralEnable(SPI3_GPIO_PERIPHERAL1);
#endif
  
  /* Configure the appropriate pins to be SSI instead of GPIO */
  GPIOPinAFConfig(SPI2_GPIO_NO_CLK, GPIO_AF_SPI2);
  GPIOPinAFConfig(SPI2_GPIO_NO_MISO, GPIO_AF_SPI2);
  GPIOPinAFConfig(SPI2_GPIO_NO_MOSI, GPIO_AF_SPI2);
  
  // clk
  GPIOModeSet(SPI2_GPIO_PIN_CLK, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI2_GPIO_PIN_CLK, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI2_GPIO_PIN_CLK, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI2_GPIO_PIN_CLK, GPIO_PUPD_NOPULL);
  
  // miso
  GPIOModeSet(SPI2_GPIO_PIN_MISO, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI2_GPIO_PIN_MISO, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI2_GPIO_PIN_MISO, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI2_GPIO_PIN_MISO, GPIO_PUPD_NOPULL);
  
  // mosi
  GPIOModeSet(SPI2_GPIO_PIN_MOSI, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI2_GPIO_PIN_MOSI, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI2_GPIO_PIN_MOSI, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI2_GPIO_PIN_MOSI, GPIO_PUPD_NOPULL);
#endif
  
#ifdef RT_USING_SPI3
  SystemPeripheralEnable(SPI3_GPIO_PERIPHERAL);
  SystemPeripheralEnable(SPI3_SPI_PERIPHERAL);
  
#ifdef SPI3_GPIO_PERIPHERAL1
  SystemPeripheralEnable(SPI3_GPIO_PERIPHERAL1);
#endif
  
  /* Configure the appropriate pins to be SSI instead of GPIO */
  GPIOPinAFConfig(SPI3_GPIO_NO_CLK, GPIO_AF_SPI3);
  GPIOPinAFConfig(SPI3_GPIO_NO_MISO, GPIO_AF_SPI3);
  GPIOPinAFConfig(SPI3_GPIO_NO_MOSI, GPIO_AF_SPI3);
  
  // clk
  GPIOModeSet(SPI3_GPIO_PIN_CLK, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI3_GPIO_PIN_CLK, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI3_GPIO_PIN_CLK, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI3_GPIO_PIN_CLK, GPIO_PUPD_NOPULL);
  
  // miso
  GPIOModeSet(SPI3_GPIO_PIN_MISO, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI3_GPIO_PIN_MISO, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI3_GPIO_PIN_MISO, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI3_GPIO_PIN_MISO, GPIO_PUPD_NOPULL);
  
  // mosi
  GPIOModeSet(SPI3_GPIO_PIN_MOSI, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI3_GPIO_PIN_MOSI, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI3_GPIO_PIN_MOSI, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI3_GPIO_PIN_MOSI, GPIO_PUPD_NOPULL);
#endif
  
#ifdef RT_USING_SPI4
  SystemPeripheralEnable(SPI4_GPIO_PERIPHERAL);
  SystemPeripheralEnable(SPI4_SPI_PERIPHERAL);
  
#ifdef SPI4_GPIO_PERIPHERAL1
  SystemPeripheralEnable(SPI4_GPIO_PERIPHERAL1);
#endif
  
  /* Configure the appropriate pins to be SSI instead of GPIO */
  GPIOPinAFConfig(SPI4_GPIO_NO_CLK, GPIO_AF_SPI4);
  GPIOPinAFConfig(SPI4_GPIO_NO_MISO, GPIO_AF_SPI4);
  GPIOPinAFConfig(SPI4_GPIO_NO_MOSI, GPIO_AF_SPI4);
  
  // clk
  GPIOModeSet(SPI4_GPIO_PIN_CLK, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI4_GPIO_PIN_CLK, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI4_GPIO_PIN_CLK, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI4_GPIO_PIN_CLK, GPIO_PUPD_NOPULL);
  
  // miso
  GPIOModeSet(SPI4_GPIO_PIN_MISO, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI4_GPIO_PIN_MISO, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI4_GPIO_PIN_MISO, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI4_GPIO_PIN_MISO, GPIO_PUPD_NOPULL);
  
  // mosi
  GPIOModeSet(SPI4_GPIO_PIN_MOSI, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI4_GPIO_PIN_MOSI, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI4_GPIO_PIN_MOSI, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI4_GPIO_PIN_MOSI, GPIO_PUPD_NOPULL);
#endif
  
#ifdef RT_USING_SPI5
  SystemPeripheralEnable(SPI5_GPIO_PERIPHERAL);
  SystemPeripheralEnable(SPI5_SPI_PERIPHERAL);
  
#ifdef SPI5_GPIO_PERIPHERAL1
  SystemPeripheralEnable(SPI5_GPIO_PERIPHERAL1);
#endif
  
  /* Configure the appropriate pins to be SSI instead of GPIO */
  GPIOPinAFConfig(SPI5_GPIO_NO_CLK, GPIO_AF_SPI5);
  GPIOPinAFConfig(SPI5_GPIO_NO_MISO, GPIO_AF_SPI5);
  GPIOPinAFConfig(SPI5_GPIO_NO_MOSI, GPIO_AF_SPI5);
  
  // clk
  GPIOModeSet(SPI5_GPIO_PIN_CLK, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI5_GPIO_PIN_CLK, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI5_GPIO_PIN_CLK, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI5_GPIO_PIN_CLK, GPIO_PUPD_NOPULL);
  
  // miso
  GPIOModeSet(SPI5_GPIO_PIN_MISO, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI5_GPIO_PIN_MISO, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI5_GPIO_PIN_MISO, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI5_GPIO_PIN_MISO, GPIO_PUPD_NOPULL);
  
  // mosi
  GPIOModeSet(SPI5_GPIO_PIN_MOSI, GPIO_MODE_AF);
  GPIOOutputTypeSet(SPI5_GPIO_PIN_MOSI, GPIO_OUT_PP);
  GPIOOutputSpeedSet(SPI5_GPIO_PIN_MOSI, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(SPI5_GPIO_PIN_MOSI, GPIO_PUPD_NOPULL);
#endif
  
#ifdef RT_USING_SPI1
#if  (RT_SPI_DMA_USING > 0)
  //  使用的DMA通道选择
  DMAMuxRequestIDSet(DMAMUX1_BASE, SPI1_TX_DMA_USE_NO);
  DMAMuxRequestIDSet(DMAMUX1_BASE, SPI1_RX_DMA_USE_NO);
  
  hal_int_register(SPI1_RX_DMA_INT_NO, SPI1_ISR);
  hal_int_priority_set(SPI1_RX_DMA_INT_NO,0x01);
  hal_int_enable(SPI1_RX_DMA_INT_NO);
#endif
  
  stm32_spi = &stm32_spi_bus_1;
  stm32_spi->spi_base = SPI1_I2S1_BASE;
#if  (RT_SPI_DMA_USING > 0)
  // 初始化阻塞
  rt_completion_init(&stm32_spi->complete);
#endif
  rt_spi_bus_register(&stm32_spi->parent, "spi1", &stm32_spi_ops);
#endif
  
#ifdef RT_USING_SPI2
  stm32_spi = &stm32_spi_bus_2;
  stm32_spi->spi_base = SPI2_I2S2_BASE;
  rt_spi_bus_register(&stm32_spi->parent, "spi2", &stm32_spi_ops);
#endif
  
#ifdef RT_USING_SPI3
  stm32_spi = &stm32_spi_bus_3;
  stm32_spi->spi_base = SPI3_I2S3_BASE;
  rt_spi_bus_register(&stm32_spi->parent, "spi3", &stm32_spi_ops);
#endif
  
#ifdef RT_USING_SPI4
  stm32_spi = &stm32_spi_bus_4;
  stm32_spi->spi_base = SPI4_BASE;
  rt_spi_bus_register(&stm32_spi->parent, "spi4", &stm32_spi_ops);
#endif
  
#ifdef RT_USING_SPI5
  stm32_spi = &stm32_spi_bus_5;
  stm32_spi->spi_base = SPI5_BASE;
  rt_spi_bus_register(&stm32_spi->parent, "spi5", &stm32_spi_ops);
#endif
  
  return 0;
}


/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_PREV_EXPORT(rt_hw_spi_init);


/*********************************************************************************************************
END FILE
*********************************************************************************************************/
