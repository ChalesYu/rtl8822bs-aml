/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_memmap.h
** Last modified Date:  2013-04-01
** Last Version:        v1.0
** Description:         IO模块内存映射定义
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2013-04-01
** Version:             v1.0
** Descriptions:        The original version 初始版本
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/

#ifndef __ARCH_IO_MEMMAP_H__
#define __ARCH_IO_MEMMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
** 外设映射基地址定义
*********************************************************************************************************/
// D3 AHB4外设 
#define HSEM_BASE               0x58026400
#define ADC3_BASE               0x58026000
#define ADC3_COMMON_BASE        0x58026300
#define DMAMUX2_BASE            0x58025800
#define BDMA_BASE               0x58025400
#define CRC_BASE                0x58024C00
#define PWR_BASE                0x58024800
#define RCC_BASE                0x58024400
#define GPIOK_BASE              0x58022800
#define GPIOJ_BASE              0x58022400
#define GPIOI_BASE              0x58022000
#define GPIOH_BASE              0x58021C00
#define GPIOG_BASE              0x58021800
#define GPIOF_BASE              0x58021400
#define GPIOE_BASE              0x58021000
#define GPIOD_BASE              0x58020C00
#define GPIOC_BASE              0x58020800
#define GPIOB_BASE              0x58020400
#define GPIOA_BASE              0x58020000
 
// D3 APB4外设
#define SAI4_BASE               0x58005400  
#define IWDG1_BASE              0x58004800  
#define RTC_BASE                0x58004000  
#define VREF_BASE               0x58003C00  
#define COMP_BASE               0x58003800  
#define LPTIM5_BASE             0x58003000  
#define LPTIM4_BASE             0x58002C00  
#define LPTIM3_BASE             0x58002800
#define LPTIM2_BASE             0x58002400  
#define I2C4_BASE               0x58001C00
#define SPI6_BASE               0x58001400  
#define LPUART1_BASE            0x58000C00  
#define SYSCFG_BASE             0x58000400  
#define EXTI_BASE               0x58000000  
  
// D1 AHB3外设  
#define DLYB_SDMMC1_BASE        0x52008000  
#define SDMMC1_BASE             0x52007000  
#define DLYM_QUADSPI_BASE       0x52006000
#define QUADSPI_BASE            0x52005000 
#define FMC_BASE                0x52004000 
#define JPEG_BASE               0x52003000 
#define FLASH_BASE              0x52002000 
#define DMA2D_BASE              0x52001000 
#define MDMA_BASE               0x52000000 
#define AXIM_GPV_BASE           0x51000000 
  
// D1 APB3外设   
#define WWDG1_BASE              0x50003000 
#define LTDC_BASE               0x50001000
  
// D2 AHB2外设 
#define DLYB_SDMMC2_BASE        0x48022800 
#define SDMMC2_BASE             0x48022400
#define RNG_BASE                0x48021800 
#define HASH_BASE               0x48021400 
#define CRYP_BASE               0x48021000 
#define DCMI_BASE               0x48020000 
  
// D2 AHB1外设  
#define USB2_OTG_FS_BASE        0x40080000 
#define USB1_OTG_HS_BASE        0x40040000 
#define ETH0_BASE               0x40028000 
#define ADC1_BASE               0x40022000
#define ADC2_BASE               0x40022100
#define ADC12_COMMON_BASE       0x40022300
#define DMAMUX1_BASE            0x40020800 
#define DMA2_BASE               0x40020400 
#define DMA1_BASE               0x40020000 
  
// D2 APB2外设    
#define HRTIM_BASE              0x40017400
#define DFSDM1_BASE             0x40017000
#define SAI3_BASE               0x40016000
#define SAI2_BASE               0x40015C00
#define SAI1_BASE               0x40015800
#define SPI5_BASE               0x40015000
#define TIM17_BASE              0x40014800
#define TIM16_BASE              0x40014400 
#define TIM15_BASE              0x40014000
#define SPI4_BASE               0x40013400
#define SPI1_I2S1_BASE          0x40013000
#define USART6_BASE             0x40011400
#define USART1_BASE             0x40011000  
#define TIM8_BASE               0x40010400
#define TIM1_BASE               0x40010000
  
// D2 APB1外设   
#define CAN_RAM_BASE            0x4000AC00
#define CAN_CCU_BASE            0x4000A800 
#define FDCAN2_BASE             0x4000A400 
#define FDCAN1_BASE             0x4000A000 
#define MDIOS_BASE              0x40009400 
#define OPAMP_BASE              0x40009000 
#define SWPMI_BASE              0x40008800 
#define CRS_BASE                0x40008400
#define UART8_BASE              0x40007C00 
#define UART7_BASE              0x40007800 
#define DAC1_BASE               0x40007400 
#define HDMI_CEC_BASE           0x40006C00 
#define I2C3_BASE               0x40005C00 
#define I2C2_BASE               0x40005800 
#define I2C1_BASE               0x40005400
#define UART5_BASE              0x40005000 
#define UART4_BASE              0x40004C00 
#define USART3_BASE             0x40004800 
#define USART2_BASE             0x40004400 
#define SPDIFRX1_BASE           0x40004000 
#define SPI3_I2S3_BASE          0x40003C00 
#define SPI2_I2S2_BASE          0x40003800
#define LPTIM1_BASE             0x40002400 
#define TIM14_BASE              0x40002000 
#define TIM13_BASE              0x40001C00 
#define TIM12_BASE              0x40001800 
#define TIM7_BASE               0x40001400 
#define TIM6_BASE               0x40001000 
#define TIM5_BASE               0x40000C00
#define TIM4_BASE               0x40000800 
#define TIM3_BASE               0x40000400 
#define TIM2_BASE               0x40000000 
  
// 其它外设基地址 
#define UID_BASE                0x1FF1E800

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_MEMMAP_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
