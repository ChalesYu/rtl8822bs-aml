/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_ethernet.c
** Last modified Date:  2015-01-12
** Last Version:        v1.00
** Description:         STM32F2xx的以太网驱动程序
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-01-12
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

#include "lwipopts.h"
#include <lwip/stats.h>
#include "lwip/sys.h"
#include <lwip_if.h>
#include <lwip_ethernet.h>

#include "board_ethernet.h"

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_syscfg.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"
#include "hal/arch/inc/arch_io_ethernet.h"

#include "lan8720/lan8720.h"

/*********************************************************************************************************
** 	驱动配置
*********************************************************************************************************/
#define MII_MODE    0
#define RMII_MODE   1
#if (((MII_MODE == 0) && (RMII_MODE == 0))  ||  \
((MII_MODE > 0) && (RMII_MODE > 0)))
#error "MII和RMII模式只能支持一种，并且一定要选择一种"
#endif

/*********************************************************************************************************
** 	驱动结构配置
*********************************************************************************************************/
/* MAC地址长度   */
#define MAX_ADDR_LEN            6
/* 定义以太网包的最大长度    */
#define ETH_MAX_PACKET_SIZE     1528
// 以太网缓冲区长度
#define ETH_MAX_BUFFER_SIZE     1524
/* 定义发送接收描述符的个数  */
#define ETH_RX_DESC_CNT         4
#define ETH_TX_DESC_CNT         4

/*********************************************************************************************************
** Ethernet DMA描述符定义
*********************************************************************************************************/
#if defined ( __ICCARM__ )  /*!< IAR Compiler */
/* Ethernet Rx DMA Descriptors */
#pragma location=0x30040000
static tEthDMADesciptor  DMARxDscrTab[ETH_RX_DESC_CNT]; 
/* Ethernet Tx DMA Descriptors */
#pragma location=0x300400C0
static tEthDMADesciptor  DMATxDscrTab[ETH_TX_DESC_CNT]; 
/* Ethernet Receive Buffers */
#pragma location=0x30040200
static rt_uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; 
static rt_uint8_t Tx_Buff[ETH_TX_DESC_CNT][ETH_MAX_PACKET_SIZE]; 

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */
/* Ethernet Rx DMA Descriptors */
static __attribute__((at(0x30040000))) tEthDMADesciptor  DMARxDscrTab[ETH_RX_DESC_CNT]; 
/* Ethernet Tx DMA Descriptors */
static __attribute__((at(0x30040060))) tEthDMADesciptor  DMATxDscrTab[ETH_TX_DESC_CNT];
/* Ethernet Receive Buffer */
static __attribute__((at(0x30040200))) rt_uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; 

#elif defined ( __GNUC__ ) /* GNU Compiler */ 
/* Ethernet Rx DMA Descriptors */
static tEthDMADesciptor DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection")));
/* Ethernet Tx DMA Descriptors */
static tEthDMADesciptor DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));
/* Ethernet Receive Buffers */
static rt_uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE] __attribute__((section(".RxArraySection")));

#endif

/*********************************************************************************************************
** 定义STM32以太网设备驱动描述结构
*********************************************************************************************************/
struct rt_stm32_eth
{
  /* inherit from ethernet device */
  struct eth_device parent;
  
  /* interface address info. */
  rt_uint8_t  dev_addr[MAX_ADDR_LEN];      // hw address
  
  tEthDMADesciptor  *DMATxDscrTab;         //  定义发送数据描述符
  tEthDMADesciptor  *DMARxDscrTab;         //  定义接收数据描述符
  rt_uint8_t DMATxDescCurrent;             //  当前使用的发送描述符下标
  rt_uint8_t DMARxDescCurrent;             //  当前使用的接收描述符下标
  
  // 发送和接收数据包定义
  rt_uint32_t SendBytes;
  rt_uint32_t RecvBytes;
  rt_uint32_t SendPacket;
  rt_uint32_t RecvPacket;
};

/*********************************************************************************************************
** 全局变量定义
*********************************************************************************************************/
static struct rt_stm32_eth stm32_eth_device;

/*********************************************************************************************************
** Function name:       EthernetGPIOInit
** Descriptions:        以太网硬件GPIO连接初始化，MAC接口工作在MII或RMII模式
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
** MII硬件连接：        MII_TXD0   <--> PB12     MII_RXD0   <--> PC4
**                      MII_TXD1   <--> PB13     MII_RXD1   <--> PC5
**                      MII_TXD2   <--> PC2      MII_RXD2   <--> PB0
**                      MII_TXD3   <--> PB8      MII_RXD3   <--> PB1
**                      MII_TX_EN  <--> PB11     MII_RX_ER  <--> PB10
**                      MII_TX_CLK <--> PC3      MII_RX_DV  <--> PA7
**                      MII_COL    <--> PA3      MII_RX_CLK <--> PA1
**                                               MII_CRS    <--> PA0
**
**                      MII_MDC    <--> PC1
**                      MII_MDIO   <--> PA2
**
** RMII硬件连接：       MII_TXD0   <--> PB12     MII_RXD0   <--> PC4
**                      MII_TXD1   <--> PB13     MII_RXD1   <--> PC5
**                      MII_TX_EN  <--> PB11     MII_RX_ER  <--> PB10
**                                               MII_RX_DV  <--> PA7
**                                               MII_RX_CLK <--> PA1
**
**                      MII_MDC    <--> PC1
**                      MII_MDIO   <--> PA2
** PHY的软件服务控制    PHY_Reset  <--> PA0 低电平复位
** MCO时钟输出          PA8        <--> 如果使用可以作为PHY芯片的时钟
*********************************************************************************************************/
static void  EthernetGPIOInit(void)
{
#if (RMII_MODE > 0)
  // 设置以太网工作在MII模式，必须在以太网时钟使能前调用
  SystemPeripheralEnable(RCC_PERIPHERAL_SYSCFG);
  SyscfgEthMACInterfaceConfig(ETH_MAC_INTERFACE_RMII);
  // 以太网复用的IO时钟使能
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOA);
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOB);
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOC);
  
//  // 配置PA8为MCO输出，为以太网PHY提供时钟
//  GPIOPinAFConfig(GPIOA_BASE, 8, GPIO_AF_MCO);
//  GPIOModeSet(GPIOA_BASE, GPIO_PIN_8,GPIO_MODE_AF);
//  GPIOOutputTypeSet(GPIOA_BASE, GPIO_PIN_8, GPIO_OUT_PP);
//  GPIOOutputSpeedSet(GPIOA_BASE, GPIO_PIN_8,GPIO_SPEED_100MHZ);
//  GPIOPullUpDownSet(GPIOA_BASE, GPIO_PIN_8,GPIO_PUPD_NOPULL);
  
//  // 软件控制复位以太网PHY
//  GPIOModeSet(GPIOA_BASE, GPIO_PIN_0,GPIO_MODE_OUT);
//  GPIOOutputTypeSet(GPIOA_BASE, GPIO_PIN_0, GPIO_OUT_PP);
//  GPIOOutputSpeedSet(GPIOA_BASE, GPIO_PIN_0, GPIO_SPEED_VERY_HIGH);
//  GPIOPullUpDownSet(GPIOA_BASE, GPIO_PIN_0, GPIO_PUPD_NOPULL);
//  GPIOPinResetBit(GPIOA_BASE, GPIO_PIN_0);
//  rt_thread_delay(1);
//  GPIOPinSetBit(GPIOA_BASE, GPIO_PIN_0);
//  rt_thread_delay(1);
//  
//  // 延时一会儿，等待复位完成
//  rt_thread_delay(20);
  
  // 配置GPIO复用引脚的以太网RMII功能
  GPIOPinAFConfig(GPIOB_BASE, 12, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOB_BASE, 13, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOB_BASE, 11, GPIO_AF_ETH);
  
  
  GPIOPinAFConfig(GPIOC_BASE, 4, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOC_BASE, 5, GPIO_AF_ETH);
//  GPIOPinAFConfig(GPIOB_BASE, 10, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOA_BASE, 7, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOA_BASE, 1, GPIO_AF_ETH);
  
  GPIOPinAFConfig(GPIOC_BASE, 1, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOA_BASE, 2, GPIO_AF_ETH);
  
  // 配置GPIO复用引脚的以太网RMII功能
  // Configure PA1, PA2 and PA7
  GPIOModeSet(GPIOA_BASE,
              GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7,
              GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOA_BASE,
                    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7,
                    GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOA_BASE,
                     GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7,
                     GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOA_BASE,
                    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7,
                    GPIO_PUPD_NOPULL);
  
  // Configure PB10, PB11, PB12, PB13
  GPIOModeSet(GPIOB_BASE,
              /*GPIO_PIN_10 | */GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
              GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOB_BASE,
                    /*GPIO_PIN_10 | */GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
                    GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOB_BASE,
                     /*GPIO_PIN_10 | */GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
                     GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOB_BASE,
                    /*GPIO_PIN_10 | */GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
                    GPIO_PUPD_NOPULL);
  // Configure PC1, PC4, PC5
  GPIOModeSet(GPIOC_BASE,
              GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5,
              GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOC_BASE,
                    GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5,
                    GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOC_BASE,
                     GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5,
                     GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOC_BASE,
                    GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5,
                    GPIO_PUPD_NOPULL);
#endif
  
#if (MII_MODE > 0)
  // 设置以太网工作在MII模式，必须在以太网时钟使能前调用
  SystemPeripheralEnable(RCC_PERIPHERAL_SYSCFG);
  SyscfgEthMACInterfaceConfig(ETH_MAC_INTERFACE_MII);
  // 以太网复用的IO时钟使能
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOA);
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOB);
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOC);
  
  // 配置PA8为MCO输出，为以太网PHY提供时钟
  GPIOPinAFConfig(GPIOA_BASE, 8, GPIO_AF_MCO);
  GPIOModeSet(GPIOA_BASE, GPIO_PIN_8,GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOA_BASE, GPIO_PIN_8, GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOA_BASE, GPIO_PIN_8,GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOA_BASE, GPIO_PIN_8,GPIO_PUPD_NOPULL);
  
  // 软件控制复位以太网PHY
  GPIOModeSet(GPIOA_BASE, GPIO_PIN_0,GPIO_MODE_OUT);
  GPIOOutputTypeSet(GPIOA_BASE, GPIO_PIN_0, GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOA_BASE, GPIO_PIN_0, GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOA_BASE, GPIO_PIN_0, GPIO_PUPD_NOPULL);
  GPIOPinResetBit(GPIOA_BASE, GPIO_PIN_0);
  rt_thread_delay(1);
  GPIOPinSetBit(GPIOA_BASE, GPIO_PIN_0);
  rt_thread_delay(1);
  
  // 配置GPIO复用引脚的以太网MII功能
  GPIOPinAFConfig(GPIOB_BASE, 12, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOB_BASE, 13, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOC_BASE, 2, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOB_BASE, 8, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOB_BASE, 11, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOC_BASE, 3, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOA_BASE, 3, GPIO_AF_ETH);
  
  GPIOPinAFConfig(GPIOC_BASE, 4, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOC_BASE, 5, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOB_BASE, 0, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOB_BASE, 1, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOB_BASE, 10, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOA_BASE, 7, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOA_BASE, 1, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOA_BASE, 0, GPIO_AF_ETH);
  
  GPIOPinAFConfig(GPIOC_BASE, 1, GPIO_AF_ETH);
  GPIOPinAFConfig(GPIOA_BASE, 2, GPIO_AF_ETH);
  
  // 配置GPIO复用引脚的以太网MII功能
  // Configure PA0, PA1, PA2, PA3 and PA7
  GPIOModeSet(GPIOA_BASE,
              GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7,
              GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOA_BASE,
                    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7,
                    GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOA_BASE,
                     GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7,
                     GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOA_BASE,
                    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7,
                    GPIO_PUPD_NOPULL);
  
  // Configure PB0, PB1, PB8, PB10, PB11, PB12, PB13
  GPIOModeSet(GPIOB_BASE,
              GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_10 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
                GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOB_BASE,
                    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_10 |
                      GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
                      GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOB_BASE,
                     GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_10 |
                       GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
                       GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOB_BASE,
                    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_10 |
                      GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13,
                      GPIO_PUPD_NOPULL);
  // Configure PC1, PC2, PC3, PC4, PC5
  GPIOModeSet(GPIOC_BASE,
              GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
              GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOC_BASE,
                    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
                    GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOC_BASE,
                     GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
                     GPIO_SPEED_VERY_HIGH);
  GPIOPullUpDownSet(GPIOC_BASE,
                    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
                    GPIO_PUPD_NOPULL);
#endif
}

/*********************************************************************************************************
** Function name:       EthernetIntHandler
** Descriptions:        以太网中断服务函数
** Input parameters:    NONE
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
static void EthernetIntHandler(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  /* Packet received */
  if(HWREG32(ETH0_BASE + ETH_DMACSR) & (DMA_INTRUPT_RIE))
  {
    if(HWREG32(ETH0_BASE + ETH_DMACIER) & (DMA_INTRUPT_RIE))
    {      
      eth_device_ready(&(stm32_eth_device.parent));
      
      /* Clear the Eth DMA Rx IT pending bits */
      HWREG32(ETH0_BASE + ETH_DMACSR) = DMA_INTRUPT_RIE | DMA_INTRUPT_NSE;
    }
  }
  
//  /* Packet transmitted */
//  if (__HAL_ETH_DMA_GET_IT(heth, ETH_DMACSR_TI))
//  {
//    if(__HAL_ETH_DMA_GET_IT_SOURCE(heth, ETH_DMACIER_TIE)) 
//    {    
//      /* Transfer complete callback */
//      HAL_ETH_TxCpltCallback(heth);
//      
//      /* Clear the Eth DMA Tx IT pending bits */
//      __HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMACSR_TI | ETH_DMACSR_NIS);
//    }
//  }
//  
//  
//  /* ETH DMA Error */
//  if(__HAL_ETH_DMA_GET_IT(heth, ETH_DMACSR_AIS))
//  {
//    if(__HAL_ETH_DMA_GET_IT_SOURCE(heth, ETH_DMACIER_AIE))
//    {
//      heth->ErrorCode |= HAL_ETH_ERROR_DMA;
//      
//      /* if fatal bus error occured */
//      if (__HAL_ETH_DMA_GET_IT(heth, ETH_DMACSR_FBE))
//      {
//        /* Get DMA error code  */
//        heth->DMAErrorCode = __HAL_ETH_DMA_GET_IT(heth, (ETH_DMACSR_FBE | ETH_DMACSR_TPS | ETH_DMACSR_RPS));
//        
//        /* Disable all interrupts */
//        __HAL_ETH_DMA_DISABLE_IT(heth, ETH_DMACIER_NIE | ETH_DMACIER_AIE);
//        
//        /* Set HAL state to ERROR */
//        heth->gState = HAL_ETH_STATE_ERROR;
//      }
//      else
//      {
//        /* Get DMA error status  */
//        heth->DMAErrorCode = __HAL_ETH_DMA_GET_IT(heth, (ETH_DMACSR_CDE | ETH_DMACSR_ETI | ETH_DMACSR_RWT |
//                                                         ETH_DMACSR_RBU | ETH_DMACSR_AIS));
//        
//        /* Clear the interrupt summary flag */
//        __HAL_ETH_DMA_CLEAR_IT(heth, (ETH_DMACSR_CDE | ETH_DMACSR_ETI | ETH_DMACSR_RWT |
//                                      ETH_DMACSR_RBU | ETH_DMACSR_AIS));
//      }
//      
//      /* Ethernet Error callback */
//      HAL_ETH_DMAErrorCallback(heth);
//    }
//  }
//  
//  /* ETH MAC Error IT */
//  if(__HAL_ETH_MAC_GET_IT(heth, (ETH_MACIER_RXSTSIE | ETH_MACIER_TXSTSIE)))
//  {
//    /* Get MAC Rx Tx status and clear Status register pending bit */
//    heth->MACErrorCode = READ_REG(heth->Instance->MACRXTXSR);
//    
//    heth->gState = HAL_ETH_STATE_ERROR;
//    
//    /* Ethernet PMT callback */
//    HAL_ETH_MACErrorCallback(heth);
//    
//    heth->MACErrorCode = (uint32_t)(0x0U);
//  } 
//  
//  /* ETH PMT IT */
//  if(__HAL_ETH_MAC_GET_IT(heth, ETH_MAC_PMT_IT))
//  {
//    /* Get MAC Wake-up source and clear the status register pending bit */
//    heth->MACWakeUpEvent = READ_BIT(heth->Instance->MACPCSR, (ETH_MACPCSR_RWKPRCVD | ETH_MACPCSR_MGKPRCVD));
//    
//    /* Ethernet PMT callback */
//    HAL_ETH_PMTCallback(heth);
//    
//    heth->MACWakeUpEvent = (uint32_t)(0x0U);
//  }
//  
//  /* ETH EEE IT */
//  if(__HAL_ETH_MAC_GET_IT(heth, ETH_MAC_LPI_IT))
//  {
//    /* Get MAC LPI interrupt source and clear the status register pending bit */
//    heth->MACLPIEvent = READ_BIT(heth->Instance->MACPCSR, 0x0000000FU);
//    
//    /* Ethernet EEE callback */
//    HAL_ETH_EEECallback(heth);
//    
//    heth->MACLPIEvent = (uint32_t)(0x0U);
//  }
//  
//  /* check ETH WAKEUP exti flag */
//  if(__HAL_ETH_WAKEUP_EXTI_GET_FLAG(ETH_WAKEUP_EXTI_LINE) != RESET)
//  {
//    /* Clear ETH WAKEUP Exti pending bit */
//    __HAL_ETH_WAKEUP_EXTI_CLEAR_FLAG(ETH_WAKEUP_EXTI_LINE);
//    /* ETH WAKEUP interrupt user callback */
//    HAL_ETH_WakeUpCallback(heth);
//  }
  
  /* leave interrupt */
  rt_interrupt_leave();
}

/*********************************************************************************************************
** Function name:       rt_stm32_eth_open
** Descriptions:        IO设备驱动初始化以太网
** Input parameters:    dev: 设备驱动描述符
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static rt_err_t rt_stm32_eth_init(rt_device_t dev)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rt_stm32_eth_open
** Descriptions:        IO设备驱动打开以太网
** Input parameters:    dev: 设备驱动描述符
** 						oflag: 打开参数
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static rt_err_t rt_stm32_eth_open(rt_device_t dev, rt_uint16_t oflag)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rt_stm32_eth_open
** Descriptions:        IO设备驱关闭以太网
** Input parameters:    dev: 设备驱动描述符
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static rt_err_t rt_stm32_eth_close(rt_device_t dev)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rt_stm32_eth_read
** Descriptions:        IO设备驱动读数据
** Input parameters:    dev: 设备驱动描述符
** 						pos: 读出位置
** 						buffer:  读出数据缓冲区
** 						size:	   读数据大小
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static rt_size_t rt_stm32_eth_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
  rt_set_errno(-RT_ENOSYS);
  return 0;
}

/*********************************************************************************************************
** Function name:       rt_stm32_eth_write
** Descriptions:        IO设备驱动写数据
** Input parameters:    dev: 设备驱动描述符
** 						pos: 写入位置
** 						buffer:  带写数据缓冲区
** 						size:	 写入数据大小
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static rt_size_t rt_stm32_eth_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
  rt_set_errno(-RT_ENOSYS);
  return 0;
}

/*********************************************************************************************************
** Function name:       rt_stm32_eth_write
** Descriptions:        IO设备驱动控制
** Input parameters:    dev: 设备驱动描述符
** 						cmd: 控制命令
** 						args:参数或返回值存放地址
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static rt_err_t rt_stm32_eth_control(rt_device_t dev, int cmd, void *args)
{
  switch(cmd)
  {
  case NIOCTL_GADDR:
    /* get mac address */
    if(args) {
      rt_memcpy(args, stm32_eth_device.dev_addr, 6);
    } else {
      return -RT_ERROR;
    }
    break;
  case NIOCTL_SENDB:
    if(args) {
      *((rt_uint32_t *)args) = stm32_eth_device.SendBytes;
    } else {
      return -RT_ERROR;
    }
    break;
  case NIOCTL_RECVB:
    if(args) {
      *((rt_uint32_t *)args) = stm32_eth_device.RecvBytes;
    } else {
      return -RT_ERROR;
    }
    break;
  case NIOCTL_SENDP:
    if(args) {
      *((rt_uint32_t *)args) = stm32_eth_device.SendPacket;
    } else {
      return -RT_ERROR;
    }
    break;
  case NIOCTL_RECVP:
    if(args) {
      *((rt_uint32_t *)args) = stm32_eth_device.RecvPacket;
    } else {
      return -RT_ERROR;
    }
    break;
  default :
    break;
  }
  
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rt_stm32_eth_tx
** Descriptions:        发送一帧以太网数据
** Input parameters:    dev: 设备驱动描述符
**                      p:   发送数据缓冲区
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
rt_err_t rt_stm32_eth_tx( rt_device_t dev, struct pbuf* p)
{
  rt_uint16_t l;
  struct pbuf *q;
  struct rt_stm32_eth *pStm32Eth = dev->user_data;
  volatile tEthDMADesciptor *dmatxdesc = (tEthDMADesciptor *)&(pStm32Eth->DMATxDscrTab[pStm32Eth->DMATxDescCurrent]);
  rt_uint8_t *buffer =  &Tx_Buff[pStm32Eth->DMATxDescCurrent][0];
  
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if(dmatxdesc->DESC3 & 0x80000000UL) {
    /* Return ERROR: OWN bit set */
    return RT_ERROR;
  }
  
#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
  
  for(q = p, l = 0; q != RT_NULL; q = q->next)
  {
    memcpy((rt_uint8_t*)&buffer[l], q->payload, q->len);
    l = l + q->len;
  }
  
  /* Set header or buffer 1 address */
  dmatxdesc->DESC0 = (rt_uint32_t)buffer;
  /* Set header or buffer 1 Length */
  dmatxdesc->DESC2 &= ~((rt_uint32_t)0x00003FFF);
  dmatxdesc->DESC2 |= p->tot_len;
  // 只使用一个DMA缓冲区，当前描述符的第二个设置为空
  dmatxdesc->DESC1 = 0;
  /* Set buffer 2 Length */
  dmatxdesc->DESC2 &= ~((rt_uint32_t)0x3FFF0000);
  
  // 设置发送特性
  {
    dmatxdesc->DESC3 &= ~((rt_uint32_t)0x00007FFF);
    dmatxdesc->DESC3 |= p->tot_len;
    
#if (CHECKSUM_BY_HARDWARE > 0)
    dmatxdesc->DESC3 |= ((rt_uint32_t)0x00030000);
    
#else
    dmatxdesc->DESC3 &= ~((rt_uint32_t)0x00030000);
#endif
      
    // 使能以太网自动插入CRC校验
    dmatxdesc->DESC3 &= ~((rt_uint32_t)0x0C000000);
  }
  
  /* Mark it as First Descriptor */
  dmatxdesc->DESC3 |= 0x20000000UL;
  /* Mark it as NORMAL descriptor */
  dmatxdesc->DESC3 &= ~0x40000000UL;
  /* set OWN bit of FIRST descriptor */
  dmatxdesc->DESC3 |= 0x80000000UL;
  
//  if(bIntMode != RT_FALSE)
//  {
//    /* Set Interrupt on completition bit */
//    dmatxdesc->DESC2 |= 0x80000000UL;    
//  }
//  else
//  {    
//    /* Clear Interrupt on completition bit */
//    dmatxdesc->DESC2 &= ~0x80000000UL;  
//  }
      
  /* Mark it as LAST descriptor */
  dmatxdesc->DESC3 |= 0x10000000UL;
  
  /* Clean and Invalidate data cache */
  rt_hw_cpu_dcache_ops((RT_HW_CACHE_FLUSH | RT_HW_CACHE_INVALIDATE), RT_NULL, 0);
  
  /* Start transmission */  
  pStm32Eth->DMATxDescCurrent = (pStm32Eth->DMATxDescCurrent + 1) % ETH_TX_DESC_CNT;
  /* issue a poll command to Tx DMA by writing address of next immediate free descriptor */
  HWREG32(ETH0_BASE + ETH_DMACTXDTPR) = (rt_uint32_t)(&pStm32Eth->DMATxDscrTab[pStm32Eth->DMATxDescCurrent]);
  
  pStm32Eth->SendBytes += p->tot_len;
  pStm32Eth->SendPacket += 1;
  
#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
  
  LINK_STATS_INC(link.xmit);
  
  /* Return SUCCESS */
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rt_stm32_eth_rx
** Descriptions:        接收一帧以太网数据
** Input parameters:    dev: 设备驱动描述符
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
struct pbuf *rt_stm32_eth_rx(rt_device_t dev)
{
  rt_uint16_t framelength = 0;
  rt_uint16_t l;
  struct pbuf *p, *q;
  rt_uint8_t *buffer;
  struct rt_stm32_eth *pStm32Eth = dev->user_data;
  volatile tEthDMADesciptor *dmarxdesc = (tEthDMADesciptor *)&(pStm32Eth->DMARxDscrTab[pStm32Eth->DMARxDescCurrent]);
  
  /* Clean and Invalidate data cache */
  rt_hw_cpu_dcache_ops((RT_HW_CACHE_FLUSH | RT_HW_CACHE_INVALIDATE), RT_NULL, 0);
  
  //  如果描述符被DMA占用，那么直接返回，不需要读取数据
  if (dmarxdesc->DESC3 & 0x80000000) {  
    return RT_NULL;
  }
  
  if((dmarxdesc->DESC3 & 0x10000000) && (dmarxdesc->DESC3 & 0x00007FFF)) {
     framelength = (dmarxdesc->DESC3 & 0x00007FFF);
     buffer = (rt_uint8_t *)(dmarxdesc->BackupAddr0);
  } else {
    dmarxdesc->DESC0 = dmarxdesc->BackupAddr0;
    dmarxdesc->DESC3 = 0x01000000UL;
    
    dmarxdesc->DESC3 |= 0x80000000UL;
    dmarxdesc->DESC3 |= 0x40000000UL;
    
    pStm32Eth->DMARxDescCurrent = (pStm32Eth->DMARxDescCurrent + 1) % ETH_RX_DESC_CNT;
    dmarxdesc = (tEthDMADesciptor *)&(pStm32Eth->DMARxDscrTab[pStm32Eth->DMARxDescCurrent]);
    HWREG32(ETH0_BASE + ETH_DMACRXDTPR) = (rt_uint32_t)dmarxdesc;
    return RT_NULL;
  }
  
  /* We allocate a pbuf chain of pbufs from the pool. */
#if ETH_PAD_SIZE
  framelength += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif
  p = pbuf_alloc(PBUF_RAW, framelength, PBUF_POOL);
  if (p != NULL)
  {
#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    for (q = p, l = 0; q != NULL; q = q->next)
    {
      memcpy((rt_uint8_t*)q->payload, (rt_uint8_t*)&buffer[l], q->len);
      l = l + q->len;
    }
    
#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    /* Adjust the link statistics */
    LINK_STATS_INC(link.recv);
    pStm32Eth->RecvBytes += framelength;
    pStm32Eth->RecvPacket += 1;
  } else {
    /* Adjust the link statistics */
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
  }
  
  // 将DMA描述符还给DMA占用
  dmarxdesc->DESC0 = dmarxdesc->BackupAddr0;
  dmarxdesc->DESC3 = 0x01000000UL;
    
  dmarxdesc->DESC3 |= 0x80000000UL;
  dmarxdesc->DESC3 |= 0x40000000UL;
  
  pStm32Eth->DMARxDescCurrent = (pStm32Eth->DMARxDescCurrent + 1) % ETH_RX_DESC_CNT;
  dmarxdesc = (tEthDMADesciptor *)&(pStm32Eth->DMARxDscrTab[pStm32Eth->DMARxDescCurrent]);
  HWREG32(ETH0_BASE + ETH_DMACRXDTPR) = (rt_uint32_t)dmarxdesc;
  
  return p;
}

/*********************************************************************************************************
** Function name:       rt_hw_ethernet_init
** Descriptions:        以太网驱动初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
#define   DevID_SNo0       (*((rt_uint32_t *)UID_BASE + UID0));
#define   DevID_SNo1       (*((rt_uint32_t *)UID_BASE + UID1));
#define   DevID_SNo2       (*((rt_uint32_t *)UID_BASE + UID2));
void rt_hw_ethernet_init(rt_uint8_t *mac)
{
  rt_uint32_t i;
  
  /*
  ** Step 1, 获取mac地址
  */
  if(mac == RT_NULL) {
    stm32_eth_device.dev_addr[0] = 0x00;
    stm32_eth_device.dev_addr[1] = 0x60;
    stm32_eth_device.dev_addr[2] = 0x6e;
    {
      rt_uint32_t cpu_id[3] = {0};
      cpu_id[2] = DevID_SNo2; cpu_id[1] = DevID_SNo1; cpu_id[0] = DevID_SNo0;
      
      // generate MAC addr from 96bit unique ID (only for test)
      stm32_eth_device.dev_addr[3] = (rt_uint8_t)((cpu_id[0]>>16)&0xFF);
      stm32_eth_device.dev_addr[4] = (rt_uint8_t)((cpu_id[0]>>8)&0xFF);
      stm32_eth_device.dev_addr[5] = (rt_uint8_t)(cpu_id[0]&0xFF);
    }
  } else {
    stm32_eth_device.dev_addr[0] = mac[0];
    stm32_eth_device.dev_addr[1] = mac[1];
    stm32_eth_device.dev_addr[2] = mac[2];
    stm32_eth_device.dev_addr[3] = mac[3];
    stm32_eth_device.dev_addr[4] = mac[4];
    stm32_eth_device.dev_addr[5] = mac[5];
  }
  
  /*
  ** Step 2, 初始化以太网描述实例
  */
  stm32_eth_device.DMATxDscrTab = DMATxDscrTab;
  stm32_eth_device.DMARxDscrTab = DMARxDscrTab;
  
  /*
  ** Step 3, 初始化硬件连接GPIO
  */
  EthernetGPIOInit();
  
  /*
  ** Step 4, 中断配置，注册中断服务函数
  */
  hal_int_register(ARCH_INT_ETH, EthernetIntHandler);
  hal_int_priority_set(ARCH_INT_ETH, 0xFF);
  hal_int_enable(ARCH_INT_ETH);
  
  /*
  ** Step 5, 打开mac时钟
  */
  SystemPeripheralEnable(RCC_PERIPHERAL_ETH_MAC);
  SystemPeripheralEnable(RCC_PERIPHERAL_ETH_MACRX);
  SystemPeripheralEnable(RCC_PERIPHERAL_ETH_MACTX);
  
  /*
  ** Step 6, 软件复位MAC所有子系统的寄存器和逻辑电路,等待复位完成
  */
  EthernetReset(ETH0_BASE);
  while(EthernetResetStatusGet(ETH0_BASE));
  
  /*
  ** Step 7, MAC控制器的初始化
  */
  EthernetInit(ETH0_BASE,SystemPeripheralClockGet(RCC_PERIPHERAL_ETH_MAC), ETH_MAX_BUFFER_SIZE);
  EthernetMACAddrSet(ETH0_BASE, (rt_uint8_t *)&stm32_eth_device.dev_addr[0]);
  EthernetDMAIntEnable(ETH0_BASE,DMA_INTRUPT_RIE | DMA_INTRUPT_NSE);
  
  /*
  ** Step 8, DMA Tx Descriptors Configuration
  */
  {
    tEthDMADesciptor *dmatxdesc;
    
    /* Fill each DMATxDesc descriptor with the right values */   
    for(i=0; i < ETH_TX_DESC_CNT; i++)
    {
      dmatxdesc = &stm32_eth_device.DMATxDscrTab[i];
      
      dmatxdesc->DESC0 = 0;
      dmatxdesc->DESC1 = 0;
      dmatxdesc->DESC2 = 0;
      dmatxdesc->DESC3 = 0;
    }
    
    stm32_eth_device.DMATxDescCurrent = 0;
    
    /* Set Transmit Descriptor Ring Length */
    HWREG32(ETH0_BASE + ETH_DMACTXDRLR) = (ETH_TX_DESC_CNT - 1);
    
    /* Set Transmit Descriptor List Address */
    HWREG32(ETH0_BASE + ETH_DMACTXDLAR) = ((rt_uint32_t)(stm32_eth_device.DMATxDscrTab));
    
    /* Set Transmit Descriptor Tail pointer */
    HWREG32(ETH0_BASE + ETH_DMACTXDTPR) = ((rt_uint32_t)(stm32_eth_device.DMATxDscrTab));
  }
  
  /*
  ** Step 9, DMA Rx Descriptors Configuration
  */
  {
    tEthDMADesciptor *dmarxdesc;
    
    for(i = 0; i < ETH_RX_DESC_CNT; i++)
    {
      dmarxdesc =  &stm32_eth_device.DMARxDscrTab[i];
      
      dmarxdesc->DESC0 = 0;
      dmarxdesc->DESC1 = 0;
      dmarxdesc->DESC2 = 0;
      dmarxdesc->DESC3 = 0;
      dmarxdesc->BackupAddr0 = 0;
      dmarxdesc->BackupAddr1 = 0;
    }
    
    stm32_eth_device.DMARxDescCurrent = 0;
    
    /* Set Receive Descriptor Ring Length */
    HWREG32(ETH0_BASE + ETH_DMACRXDRLR) = (ETH_RX_DESC_CNT - 1);
    
    /* Set Receive Descriptor List Address */
    HWREG32(ETH0_BASE + ETH_DMACRXDLAR) = ((rt_uint32_t)&stm32_eth_device.DMARxDscrTab[0]);
    
    /* Set Receive Descriptor Tail pointer Address */
    HWREG32(ETH0_BASE + ETH_DMACRXDTPR) = ((rt_uint32_t)&stm32_eth_device.DMARxDscrTab[ETH_RX_DESC_CNT - 1]);
  }
  
  /*
  ** Step 10, 初始化接收DMA
  */
  for(i = 0; i < ETH_RX_DESC_CNT; i++)
  {
    tEthDMADesciptor *dmarxdesc = &stm32_eth_device.DMARxDscrTab[i];
    
    /* write buffer address to RDES0 */
    dmarxdesc->DESC0 = (rt_uint32_t)&Rx_Buff[i][0];
    /* store buffer address */
    dmarxdesc->BackupAddr0 = (rt_uint32_t)&Rx_Buff[i][0];
    /* set buffer address valid bit to RDES3 */
    dmarxdesc->DESC3 |= ((rt_uint32_t)0x01000000);
    
    /* Interrupt Enabled on Completion  */
    dmarxdesc->DESC3 |= ((rt_uint32_t)0x40000000);
      
    /* set OWN bit to RDES3 */
    dmarxdesc->DESC3 |= ((rt_uint32_t)0x80000000);
  }
  
  
  /*
  ** Step 11, 初始化PHY
  */
  lan8720_init();
  
  /*
  ** Step 12, 启动以太网
  */
  EthernetEnable(ETH0_BASE);
  
  /*
  ** Step 13, 设备驱动注册
  */
  stm32_eth_device.parent.parent.init       = rt_stm32_eth_init;
  stm32_eth_device.parent.parent.open       = rt_stm32_eth_open;
  stm32_eth_device.parent.parent.close      = rt_stm32_eth_close;
  stm32_eth_device.parent.parent.read       = rt_stm32_eth_read;
  stm32_eth_device.parent.parent.write      = rt_stm32_eth_write;
  stm32_eth_device.parent.parent.control    = rt_stm32_eth_control;
  stm32_eth_device.parent.parent.user_data  = &stm32_eth_device;
  
  stm32_eth_device.parent.eth_rx     = rt_stm32_eth_rx;
  stm32_eth_device.parent.eth_tx     = rt_stm32_eth_tx;
  
  /* register eth device */
  eth_device_init(&(stm32_eth_device.parent), "e0");
  netif_set_link_up(stm32_eth_device.parent.netif);
}


/**
 * Get the nmp info
 */
/*********************************************************************************************************
** Function name:       rt_hw_ethernet_init
** Descriptions:        以太网驱动初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
struct ethernetif_nmp *ethernetif_nmp_get(void)
{
  return (struct ethernetif_nmp *)(&(stm32_eth_device.SendBytes));
}

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
