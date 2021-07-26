/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_rcc.h
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         rcc模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2019-03-15
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
#ifndef __ARCH_IO_RCC_H__
#define __ARCH_IO_RCC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
  时钟模块配置定义
*********************************************************************************************************/
#define RCC_CLOCK_CONFIG_EN             1           //  是否需要初始化时钟，如果bootloader已经初始化，这里为0 

// 系统时钟源选择
#define RCC_SYSCLK_USE_HSI              0
#define RCC_SYSCLK_USE_CSI              1
#define RCC_SYSCLK_USE_HSE              2
#define RCC_SYSCLK_USE_PLL1             3

// 系统时钟选选择配置，取上面4个值中的一个
#define RCC_SYSCLK_USING                RCC_SYSCLK_USE_PLL1 //RCC_SYSCLK_USE_HSI //RCC_SYSCLK_USE_CSI

// HSI配置定义
#define RCC_HSI_64MHZ                 0
#define RCC_HSI_32MHZ                 1
#define RCC_HSI_16MHZ                 2
#define RCC_HSI_8MHZ                  3

// HSI时钟选选择配置，取上面4个值中的一个，如果不使用HSI，可以不选择
#define RCC_HSI_USING                 RCC_HSI_64MHZ

// HSE配置
#define RCC_HSE_INPUT_CLOCK           8000000    //  外部输入振荡器的频率

// PLL配置
// PLL输入源参数蒂尼
#define RCC_PLL_SRC_HSI                 0 // HSI输入给PLL
#define RCC_PLL_SRC_CSI                 1 // CSI输入给PLL
#define RCC_PLL_SRC_HSE                 2 // HSE输入给PLL
#define RCC_PLL_SRC_NO                  3 // PLL无输入 

// PLL输入时钟选选择配置，取上面4个值中的一个，不使用PLL时无效
#define RCC_PLL_SRC_USING              RCC_PLL_SRC_HSE

// PLL输入频率范围配置
#define RCC_PLL_SRC_RANGE_1_2M          0UL    // PLL输入频率范围1M-2M
#define RCC_PLL_SRC_RANGE_2_4M          1UL    // PLL输入频率范围2M-4M
#define RCC_PLL_SRC_RANGE_4_8M          2UL    // PLL输入频率范围4M-8M
#define RCC_PLL_SRC_RANGE_8_16M         3UL    // PLL输入频率范围8M-16M

// PLL1配置
#define RCC_PLL1_SRC_RANGE_USING        RCC_PLL_SRC_RANGE_4_8M // PLL输入时钟范围选选择配置，取上面4个值中的一个，不使用PLL时无效
#define RCC_CLOCK_PLL1_SRC_DIV          2     //　PLL输入分频，用来满足PLL输入时钟范围的需求
#define RCC_CLOCK_PLL1_MUL              200   //  PLL乘数，得到VCO时钟, 4-512的数
#define RCC_CLOCK_PLL1_P                2     //  PLL分频系数P，取2-128, 得到系统时钟
#define RCC_CLOCK_PLL1_Q                4     //  PLL分频系数Q，取2-128
#define RCC_CLOCK_PLL1_R                2     //　PLL分频系数R，取2-128

//// PLL2配置
//#define RCC_PLL2_SRC_RANGE_USING        RCC_PLL_SRC_RANGE_4_8M // PLL输入时钟范围选选择配置，取上面4个值中的一个，不使用PLL时无效
//#define RCC_CLOCK_PLL2_SRC_DIV          5     //　PLL输入分频，用来满足PLL输入时钟范围的需求
//#define RCC_CLOCK_PLL2_MUL              160   //  PLL乘数，得到VCO时钟
//#define RCC_CLOCK_PLL2_P                7     //  PLL分频系数P，取2-128
//#define RCC_CLOCK_PLL2_Q                2     //  PLL分频系数Q，取2-128
//#define RCC_CLOCK_PLL2_R                2     //　PLL分频系数R，取2-128

// PLL3配置
#define RCC_PLL3_SRC_RANGE_USING        RCC_PLL_SRC_RANGE_1_2M // PLL输入时钟范围选选择配置，取上面4个值中的一个，不使用PLL时无效
#define RCC_CLOCK_PLL3_SRC_DIV          8     //　PLL输入分频，用来满足PLL输入时钟范围的需求
#define RCC_CLOCK_PLL3_MUL              336   //  PLL乘数，得到VCO时钟
#define RCC_CLOCK_PLL3_P                2     //  PLL分频系数P，取2-128
#define RCC_CLOCK_PLL3_Q                7     //  PLL分频系数Q，取2-128
#define RCC_CLOCK_PLL3_R                10     //　PLL分频系数R，取2-128

// 配置各总线时钟
#define RCC_CLOCK_SYSCLK_DIV            1    // 系统时钟分频，可以取{ 1 2 4 8 16 64 128 256 512 }
#define RCC_CLOCK_AHB_DIV               2    // AHB分频，可以取{ 1 2 4 8 16 64 128 256 512 }，从SYSCLK而来
#define RCC_CLOCK_APB3_DIV              2    // APB3分频，可以取{ 1 2 4 8 16}， 从AHB CLK分频而来
#define RCC_CLOCK_APB2_DIV              2    // APB3分频，可以取{ 1 2 4 8 16}， 从AHB CLK分频而来
#define RCC_CLOCK_APB1_DIV              2    // APB3分频，可以取{ 1 2 4 8 16}， 从AHB CLK分频而来
#define RCC_CLOCK_APB4_DIV              4    // APB3分频，可以取{ 1 2 4 8 16}， 从AHB CLK分频而来
  
////　是否打卡开LSI, 设置为0，关闭LSI，反之打开LSI
//#define RCC_USING_LSI                   1
////　是否打卡开LSE, 设置为0，关闭LSE，反之打开LSE
//#define RCC_USING_LSE    

// 是否打开HSI48,设置为0，关闭HSI48，反之打开hsi48
#define RCC_USING_HSI48                0
  
/*********************************************************************************************************
** 外设外设寄存器控制，用于控制外设时钟。宏定义为双参数，第一个分为高8bit和低8bit，
** 低8bit表示为那个总线，可以取下列值：
** 0x00: AHB3
** 0x01: AHB1
** 0x02: AHB2
** 0x03: AHB4
** 0x04: APB3
** 0x05: APB1L
** 0x06: APB1H
** 0x07: APB2
** 0x08: APB4
** 高8bit是外设编号，当获取时钟时可能有用
** 第二个参数为在控制寄存器的位置
*********************************************************************************************************/
// AHB3外设
#define RCC_PERIPHERAL_CPU              0x0100, (1UL<<31)
#define RCC_PERIPHERAL_SDMMC1           0x0200, (1UL<<16)
#define RCC_PERIPHERAL_QSPI             0x0300, (1UL<<14)
#define RCC_PERIPHERAL_FMC              0x0400, (1UL<<12)
#define RCC_PERIPHERAL_JPGDEC           0x0500, (1UL<<5)
#define RCC_PERIPHERAL_DMA2D            0x0600, (1UL<<4)
#define RCC_PERIPHERAL_MDMA             0x0700, (1UL<<0)  
  
// AHB1外设
#define RCC_PERIPHERAL_USB2OTG          0x0801, (1UL<<27)
#define RCC_PERIPHERAL_USB1ULPI         0x0901, (1UL<<26)
#define RCC_PERIPHERAL_USB1OTG          0x0901, (1UL<<25)
#define RCC_PERIPHERAL_ETH_MACRX        0x0A01, (1UL<<17)
#define RCC_PERIPHERAL_ETH_MACTX        0x0A01, (1UL<<16)
#define RCC_PERIPHERAL_ETH_MAC          0x0A01, (1UL<<15)
#define RCC_PERIPHERAL_ADC12            0x0B01, (1UL<<5)
#define RCC_PERIPHERAL_DMA2             0x0C01, (1UL<<1)
#define RCC_PERIPHERAL_DMA1             0x0D01, (1UL<<0)

// AHB2外设
#define RCC_PERIPHERAL_SRAM3            0x0002, (1UL<<31)   //  获取时钟不重要，高8bit为0
#define RCC_PERIPHERAL_SRAM2            0x0002, (1UL<<30)   //  获取时钟不重要，高8bit为0
#define RCC_PERIPHERAL_SRAM1            0x0002, (1UL<<29)   //  获取时钟不重要，高8bit为0
#define RCC_PERIPHERAL_SDMMC2           0x0E02, (1UL<<9)
#define RCC_PERIPHERAL_RNG              0x0F02, (1UL<<6)
#define RCC_PERIPHERAL_HASH             0x1002, (1UL<<5)
#define RCC_PERIPHERAL_CRYPT            0x1102, (1UL<<4)
#define RCC_PERIPHERAL_CAMITF           0x1202, (1UL<<0)
  
// AHB4外设
#define RCC_PERIPHERAL_BKPRAM           0x0003, (1UL<<28)   //  获取时钟不重要，高8bit为0
#define RCC_PERIPHERAL_HSEM             0x1303, (1UL<<25)
#define RCC_PERIPHERAL_ADC3             0x1403, (1UL<<24)
#define RCC_PERIPHERAL_BDMA             0x1503, (1UL<<21)
#define RCC_PERIPHERAL_CRC              0x1603, (1UL<<19)
#define RCC_PERIPHERAL_GPIOK            0x1703, (1UL<<10)
#define RCC_PERIPHERAL_GPIOJ            0x1803, (1UL<<9)
#define RCC_PERIPHERAL_GPIOI            0x1903, (1UL<<8)
#define RCC_PERIPHERAL_GPIOH            0x1A03, (1UL<<7)
#define RCC_PERIPHERAL_GPIOG            0x1B03, (1UL<<6)
#define RCC_PERIPHERAL_GPIOF            0x1C03, (1UL<<5)
#define RCC_PERIPHERAL_GPIOE            0x1D03, (1UL<<4)
#define RCC_PERIPHERAL_GPIOD            0x1E03, (1UL<<3)
#define RCC_PERIPHERAL_GPIOC            0x1F03, (1UL<<2)
#define RCC_PERIPHERAL_GPIOB            0x2003, (1UL<<1)
#define RCC_PERIPHERAL_GPIOA            0x2103, (1UL<<0)
  
// APB3外设
#define RCC_PERIPHERAL_WWDG1            0x0004, (1UL<<6)   //  获取时钟不重要，高8bit为0
#define RCC_PERIPHERAL_LTDC             0x2204, (1UL<<3)  
  
// APB1外设L 
#define RCC_PERIPHERAL_UART8            0x2305, (1UL<<31)
#define RCC_PERIPHERAL_UART7            0x2405, (1UL<<30)  
#define RCC_PERIPHERAL_DAC12            0x2505, (1UL<<29)  
#define RCC_PERIPHERAL_HDMICEC          0x2605, (1UL<<27)
#define RCC_PERIPHERAL_I2C3             0x2705, (1UL<<23)
#define RCC_PERIPHERAL_I2C2             0x2805, (1UL<<22)
#define RCC_PERIPHERAL_I2C1             0x2905, (1UL<<21)
#define RCC_PERIPHERAL_UART5            0x2A05, (1UL<<20)
#define RCC_PERIPHERAL_UART4            0x2B05, (1UL<<19)
#define RCC_PERIPHERAL_USART3           0x2C05, (1UL<<18)
#define RCC_PERIPHERAL_USART2           0x2D05, (1UL<<17)
#define RCC_PERIPHERAL_SPDIFRX          0x2E05, (1UL<<16)
#define RCC_PERIPHERAL_SPI3             0x2F05, (1UL<<15)  
#define RCC_PERIPHERAL_SPI2             0x3005, (1UL<<14)
#define RCC_PERIPHERAL_LPTIM1           0x3105, (1UL<<9)
#define RCC_PERIPHERAL_TIM14            0x3205, (1UL<<8)
#define RCC_PERIPHERAL_TIM13            0x3305, (1UL<<7)
#define RCC_PERIPHERAL_TIM12            0x3405, (1UL<<6)
#define RCC_PERIPHERAL_TIM7             0x3505, (1UL<<5)
#define RCC_PERIPHERAL_TIM6             0x3605, (1UL<<4)
#define RCC_PERIPHERAL_TIM5             0x3705, (1UL<<3)
#define RCC_PERIPHERAL_TIM4             0x3805, (1UL<<2)
#define RCC_PERIPHERAL_TIM3             0x3905, (1UL<<1)
#define RCC_PERIPHERAL_TIM2             0x3A05, (1UL<<0)

// APB1外设H 
#define RCC_PERIPHERAL_FDCAN            0x3B06, (1UL<<8)
#define RCC_PERIPHERAL_MDIOS            0x3C06, (1UL<<5)  
#define RCC_PERIPHERAL_OPAMP            0x3D06, (1UL<<4)  
#define RCC_PERIPHERAL_SWPMI            0x3E06, (1UL<<2)
#define RCC_PERIPHERAL_CRS              0x3F06, (1UL<<1)
  
// APB2外设
#define RCC_PERIPHERAL_HRTIM            0x4007, (1UL<<29)
#define RCC_PERIPHERAL_DFSDM1           0x4107, (1UL<<28)
#define RCC_PERIPHERAL_SAI3             0x4207, (1UL<<24)
#define RCC_PERIPHERAL_SAI2             0x4307, (1UL<<23)
#define RCC_PERIPHERAL_SAI1             0x4407, (1UL<<22)
#define RCC_PERIPHERAL_SPI5             0x4507, (1UL<<20)
#define RCC_PERIPHERAL_TIM17            0x4607, (1UL<<18)
#define RCC_PERIPHERAL_TIM16            0x4707, (1UL<<17)
#define RCC_PERIPHERAL_TIM15            0x4807, (1UL<<16)
#define RCC_PERIPHERAL_SPI4             0x4907, (1UL<<13)
#define RCC_PERIPHERAL_SPI1             0x4A07, (1UL<<12)
#define RCC_PERIPHERAL_USART6           0x4B07, (1UL<<5)
#define RCC_PERIPHERAL_USART1           0x4C07, (1UL<<4)
#define RCC_PERIPHERAL_TIM8             0x4D07, (1UL<<1)
#define RCC_PERIPHERAL_TIM1             0x4E07, (1UL<<0)
  
// APB4外设
#define RCC_PERIPHERAL_SAI4             0x4F08, (1UL<<21)
#define RCC_PERIPHERAL_RTCAPB           0x0008, (1UL<<16)   //  获取时钟不重要，高8bit为0 
#define RCC_PERIPHERAL_VREF             0x5008, (1UL<<15)
#define RCC_PERIPHERAL_COMP12           0x5108, (1UL<<14)
#define RCC_PERIPHERAL_LPTIM5           0x5208, (1UL<<12)
#define RCC_PERIPHERAL_LPTIM4           0x5308, (1UL<<11)
#define RCC_PERIPHERAL_LPTIM3           0x5408, (1UL<<10)
#define RCC_PERIPHERAL_LPTIM2           0x5508, (1UL<<9)
#define RCC_PERIPHERAL_I2C4             0x5608, (1UL<<7)
#define RCC_PERIPHERAL_SPI6             0x5708, (1UL<<5)
#define RCC_PERIPHERAL_LPUART1          0x5808, (1UL<<3)
#define RCC_PERIPHERAL_SYSCFG           0x5908, (1UL<<1)


/*********************************************************************************************************
** 外部函数的声明
*********************************************************************************************************/
extern void SystemClockInit(void);
extern void SystemI2SPLLSet(rt_uint32_t PLLI2SN, rt_uint32_t PLLI2SR);
extern void SystemI2SClockConfig(rt_uint32_t RCC_I2SCLKSource);
extern void SystemI2SPLLEnable(void);
extern void SystemI2SPLLDisable(void);
extern rt_uint32_t SystemSysClockGet(void);
extern rt_uint32_t SystemSysTickClockGet(void);
extern rt_uint32_t SystemAHBClockGet(void);
extern rt_uint32_t SystemAPB1ClockGet(void);
extern rt_uint32_t SystemAPB2ClockGet(void);
extern void SystemPeripheralReset(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl);
extern void SystemPeripheralAllReset(void);
extern void SystemPeripheralEnable(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl);
extern void SystemPeripheralDisable(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl);
extern void SystemPeripheralLPModeEnable(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl);
extern void SystemPeripheralLPModeDisable(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl);
extern rt_uint32_t SystemPeripheralClockGet(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl);

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_RCC_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
