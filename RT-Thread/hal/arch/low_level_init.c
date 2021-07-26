/**************************************************
 *
 * This module contains the function `__low_level_init', a function
 * that is called before the `main' function of the program.  Normally
 * low-level initializations - such as setting the prefered interrupt
 * level or setting the watchdog - can be performed here.
 *
 * Note that this function is called before the data segments are
 * initialized, this means that this function cannot rely on the
 * values of global or static variables.
 *
 * When this function returns zero, the startup code will inhibit the
 * initialization of the data segments. The result is faster startup,
 * the drawback is that neither global nor static data will be
 * initialized.
 *
 * Copyright 1999-2004 IAR Systems. All rights reserved.
 *
 * $Revision: 21623 $
 *
 **************************************************/

#ifdef __cplusplus
extern "C" {
#endif

  /**
   * 基本配置
  */
#define  FSMC_NAND_EN         0
#define  FSMC_SRAM8_EN        0
#define  FSMC_SRAM16_NAND_EN  0
#define  FMC_SDRAM_EN         0

  /**
   * 包含需要的头文件
  */
#include <rthw.h>
#include "hal/cortexm/hal_cortexm_api.h"
#include "hal/cortexm/hal_io_fpu.h"
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_gpio.h"
#include "hal/arch/inc/arch_io_syscfg.h"
  

  /**
   * 以下是IAR提供的模板代码
  */
#pragma language=extended

__interwork int __low_level_init(void);

__interwork int __low_level_init(void)
{
    /*==================================*/
    /*  Initialize hardware.            */
  
#if __ARMVFP__
  /* Enable access to Floating-point coprocessor.         */
  HWREG32(HAL_FPU_CPACR) |= (((rt_uint32_t)0x0F) << 20);
  
  /* Disable automatic FP register content                */
  HWREG32(HAL_FPU_FPCCR) &= ~(((rt_uint32_t)1) << 31);
  /* Disable Lazy context switch                          */
  HWREG32(HAL_FPU_FPCCR) &= ~(((rt_uint32_t)1) << 0);
#endif
  
    /**
    * Step1, 初始化系统时钟
    */
    hal_int_priority_group_get(INT_NUM_PRIORITY_BITS);
    SystemClockInit();

    /**
    * Step2, 使能高速IO的配置
    */
    SystemPeripheralEnable(RCC_PERIPHERAL_SYSCFG);
    SyscfgCompensationCellCtrl(RT_TRUE);
#if (FSMC_NAND_EN > 0)
    /**
    * Step3, 初始化FSMC复用的GPIO，用于驱动NAND Flash
    *  I/O0 <--> PD14    R/B#     <--> PD6
    *  I/O1 <--> PD15    NRE      <--> PD4
    *  I/O2 <--> PD0     NE2/NCE3 <--> PG9
    *  I/O3 <--> PD1     CLE(A16) <--> PD11
    *  I/O4 <--> PE7     ALE(A17) <--> PD12
    *  I/O5 <--> PE8     NWE      <--> PD5
    *  I/O6 <--> PE9
    *  I/O7 <--> PE10
    */
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOD);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOE);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOG);

    // 设置IO为FSMC功能
    GPIOPinAFConfig(GPIOD_BASE, 14, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 15, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 7, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 8, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 9, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 10, GPIO_AF_FSMC);

    GPIOPinAFConfig(GPIOD_BASE, 6, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 4, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 9, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 11, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 12, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 5, GPIO_AF_FSMC);

    // PD0, PD1, PD4, PD5, PD6, PD11, PD12, PD14, PD15设置为AF push pull
    GPIOModeSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_PUPD_NOPULL);

    // PE7, PE8, PE9, PE10设置为AF push pull
    GPIOModeSet(GPIOE_BASE,
                GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOE_BASE,
                GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOE_BASE,
                GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOE_BASE,
                GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10,
                GPIO_PUPD_NOPULL);

    // PG9设置为AF push pull
    GPIOModeSet(GPIOG_BASE, GPIO_PIN_9, GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOG_BASE, GPIO_PIN_9, GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOG_BASE, GPIO_PIN_9, GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOG_BASE, GPIO_PIN_9, GPIO_PUPD_NOPULL);
#endif
    
    
#if (FSMC_SRAM16_NAND_EN > 0)
   /**
    * Step3, 初始化FSMC复用的GPIO，以及FSMC的SRAM控制器及NAND控制器
    *  D0-D1 <--> PD14-PD15    A0-A5   <--> PF0-PF5
    *  D2-D3 <--> PD0-PD1      A6-A9   <--> PF12-PF15
    *  D4-D12<--> PE7-PE15     A10-A15 <--> PG0-PG5
    *  D13-D15<--> PD8-PD10    A16-A18 <--> PD11-PD13
    *                          A19-A22 <--> PE3-PE6
    *  #OE     <--> PD4
    *  #WE     <--> PD5
    *
    *  SRAM
    *  NBL0    <--> PE0
    *  NBL1    <--> PE1
    *  #CE     <--> PG10  <---> FSMC_NE3， 地址范围：0x68000000-0x6BFFFFFF
    *
    *  NAND
    *  R/B    <--> PG6    <---> FSMC_INT2
    *  #CE    <--> PD7    <---> FSMC_NCE2， 地址范围：0x70000000-0x73FFFFFF
    *
    **/
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOD);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOE);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOF);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOG);

    // 设置IO为FSMC功能
    GPIOPinAFConfig(GPIOD_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 4, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 5, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 7, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 8, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 9, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 10, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 11, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 12, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 13, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 14, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 15, GPIO_AF_FSMC);

    GPIOPinAFConfig(GPIOE_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 3, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 4, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 5, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 6, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 7, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 8, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 9, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 10, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 11, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 12, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 13, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 14, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 15, GPIO_AF_FSMC);

    GPIOPinAFConfig(GPIOF_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 2, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 3, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 4, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 5, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 12, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 13, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 14, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 15, GPIO_AF_FSMC);
    
    GPIOPinAFConfig(GPIOG_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 2, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 3, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 4, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 5, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 6, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 10, GPIO_AF_FSMC);
    
    // PD口需要的设置为AF push pull
    GPIOModeSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 |
                GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |GPIO_PIN_11 | GPIO_PIN_12 | 
                GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 |
                GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |GPIO_PIN_11 | GPIO_PIN_12 | 
                GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 |
                GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |GPIO_PIN_11 | GPIO_PIN_12 | 
                GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 |
                GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |GPIO_PIN_11 | GPIO_PIN_12 | 
                GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_PUPD_NOPULL);

    // PE口需要的设置为AF push pull
    GPIOModeSet(GPIOE_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |GPIO_PIN_9 | GPIO_PIN_10 | 
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOE_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |GPIO_PIN_9 | GPIO_PIN_10 | 
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOE_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |GPIO_PIN_9 | GPIO_PIN_10 | 
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOE_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |GPIO_PIN_9 | GPIO_PIN_10 | 
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_PUPD_NOPULL);
    
    // PF口需要的设置为AF push pull
    GPIOModeSet(GPIOF_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOF_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOF_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOF_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_PUPD_NOPULL);
    
    
    // PG口需要的设置为AF push pull
    GPIOModeSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | 
                GPIO_PIN_5 | GPIO_PIN_10,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | 
                GPIO_PIN_5 | GPIO_PIN_10,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | 
                GPIO_PIN_5 | GPIO_PIN_10,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | 
                GPIO_PIN_5 | GPIO_PIN_10,
                GPIO_PUPD_NOPULL);
    
    // 使能FSMC的时钟
    SystemPeripheralEnable(RCC_PERIPHERAL_FSMC);
    
    // 开始配置16位SRAM的FSMC
    HWREG32(FSMC_BASE + FSMC_BCR3) = 0x00001011;   // 读写时序一样，不需要配置BWTR寄存器
    // HWREG32(FSMC_BASE + FSMC_BCR3) = 0x00005011; // 读写寄存器单独时序，需要配置BWTR寄存器的写时序
    HWREG32(FSMC_BASE + FSMC_BTR3) = 0x00000300;
    if(HWREG32(FSMC_BASE + FSMC_BCR3) & 0x4000) {
      HWREG32(FSMC_BASE + FSMC_BWTR3) = 0x00000300;
    } else {
      HWREG32(FSMC_BASE + FSMC_BWTR3) = 0x0FFFFFFF;
    }
    // 开始配置FSMC Nand控制器

#endif
    
#if (FSMC_SRAM8_EN > 0)
    /**
    * Step3, 初始化FSMC复用的GPIO，以及FSMC的SRAM控制器
    *  D0 <--> PD14    A0-A5   <--> PF0-PF5
    *  D1 <--> PD15    A6-A9   <--> PF12-PF15
    *  D2 <--> PD0     A10-A15 <--> PG0-PG5
    *  D3 <--> PD1     A16-A18 <--> PD11-PD13
    *  D4 <--> PE7     A19     <--> PE3
    *  D5 <--> PE8     #OE     <--> PD4
    *  D6 <--> PE9     #WE     <--> PD5
    *  D7 <--> PE10    #CS     <--> PD7   <---> FSMC_NE1， 地址范围：0x60000000-0x63FFFFFF
    */
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOD);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOE);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOF);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOG);

    // 设置IO为FSMC功能
    GPIOPinAFConfig(GPIOD_BASE, 14, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 15, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 7, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 8, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 9, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 10, GPIO_AF_FSMC);

    GPIOPinAFConfig(GPIOF_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 2, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 3, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 4, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 5, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 12, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 13, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 14, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 15, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 2, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 3, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 4, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 5, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 11, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 12, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 13, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 3, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 4, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 5, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 7, GPIO_AF_FSMC);
    
    // PD0, PD1, PD4, PD5, PD7, PD11, PD12, PD13, PD14, PD15设置为AF push pull
    GPIOModeSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 |
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_PUPD_NOPULL);

    // PE3, PE7, PE8, PE9, PE10设置为AF push pull
    GPIOModeSet(GPIOE_BASE,
                GPIO_PIN_3 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOE_BASE,
                GPIO_PIN_3 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOE_BASE,
                GPIO_PIN_3 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOE_BASE,
                GPIO_PIN_3 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10,
                GPIO_PUPD_NOPULL);
    
    // PF0, PF1, PF2, PF3, PF4, PF5, PF12, PF13, PF14, PF15设置为AF push pull
    GPIOModeSet(GPIOF_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOF_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOF_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOF_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_PUPD_NOPULL);
    // PG0, PG1, PG2, PG3, PG4, PG5设置为AF push pull
    GPIOModeSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
                GPIO_PUPD_NOPULL);
    
    // 开始配置8位SRAM的FSMC
    // 使能FSMC的时钟
    SystemPeripheralEnable(RCC_PERIPHERAL_FSMC);
    HWREG32(FSMC_BASE + FSMC_BCR1) = 0x00001005;
    HWREG32(FSMC_BASE + FSMC_BTR1) = 0x00010400;
    HWREG32(FSMC_BASE + FSMC_BWTR1) = 0x0fffffff;
#endif
    
#if (FMC_SDRAM_EN > 0)
  /*
   ****************************************************************************
   * PC3 --> SDCKE0  | PE0 --> NBL0  | PF0 --> A0    | PG0 --> A10            *
   * PC2 --> SDNE0   | PE1 --> NBL1
                     | PE7 --> D4    | PF1 --> A1    | PG1 --> A11            *
   * ----------------| PE8 --> D5    | PF2 --> A2    | PG2 --> A12            *
   * PC0 --> SDNWE   | PE9 --> D6    | PF3 --> A3    | PG4 --> BA0            *
   * ----------------| PE10--> D7    | PF4 --> A4    | PG5 --> BA1            *
   * PD0 --> D2      | PE11--> D8    | PF5 --> A5    | PG8 --> SDCLK          *
   * PD1 --> D3      | PE12--> D9    | PF11--> SDNRAS| PG15--> SDNCAS         *
   * PD8 --> D13     | PE13--> D10   | PF12--> A6    |                        *
   * PD9 --> D14     | PE14--> D11   | PF13--> A7    |                        *
   * PD10--> D15     | PE15--> D12   | PF14--> A8    |                        *
   * PD14--> D0      |---------------| PF15--> A9    |                        *
   * PD15--> D1      |                                                        *
   ****************************************************************************
   */
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOC);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOD);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOE);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOF);
    SystemPeripheralEnable(RCC_PERIPHERAL_GPIOG);

    // 设置IO为FSMC功能
    GPIOPinAFConfig(GPIOC_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOC_BASE, 2, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOC_BASE, 3, GPIO_AF_FSMC);

    
    GPIOPinAFConfig(GPIOD_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 8, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 9, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 10, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 14, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOD_BASE, 15, GPIO_AF_FSMC);
    
    GPIOPinAFConfig(GPIOE_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 1, GPIO_AF_FSMC); 
    GPIOPinAFConfig(GPIOE_BASE, 7, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 8, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 9, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 10, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 11, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 12, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 13, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 14, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOE_BASE, 15, GPIO_AF_FSMC);
    
    GPIOPinAFConfig(GPIOF_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 2, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 3, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 4, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 5, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 11, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 12, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 13, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 14, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOF_BASE, 15, GPIO_AF_FSMC);
    
    
    GPIOPinAFConfig(GPIOG_BASE, 0, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 1, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 2, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 4, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 5, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 8, GPIO_AF_FSMC);
    GPIOPinAFConfig(GPIOG_BASE, 15, GPIO_AF_FSMC);

    
    // 设置为AF push pull
    GPIOModeSet(GPIOC_BASE,GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3,GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOC_BASE,GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3,GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOC_BASE,GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3,GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOC_BASE,GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3,GPIO_PUPD_NOPULL);

    GPIOModeSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOD_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_PUPD_NOPULL);
    
    GPIOModeSet(GPIOE_BASE,
               GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | 
               GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOE_BASE,
               GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | 
               GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOE_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | 
               GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOE_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | 
               GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_PUPD_NOPULL);
    
    GPIOModeSet(GPIOF_BASE,
               GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | 
               GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOF_BASE,
               GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | 
               GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOF_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | 
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOF_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | 
                GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                GPIO_PUPD_NOPULL);

    GPIOModeSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15,
                GPIO_MODE_AF);
    GPIOOutputTypeSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15,
                GPIO_OUT_PP);
    GPIOOutputSpeedSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15,
                GPIO_SPEED_100MHZ);
    GPIOPullUpDownSet(GPIOG_BASE,
                GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15,
                GPIO_PUPD_NOPULL);
    
    // 开始配置8位SRAM的FSMC
    // 使能FSMC的时钟
    SystemPeripheralEnable(RCC_PERIPHERAL_FMC);
#if 1  // 160M的配置，SDRAM时钟为80M
    /* Configure and enable SDRAM bank1 */
    HWREG32(FMC_BASE + FMC_SDCR1) = 0x000029D9; //0x000029D9;
    HWREG32(FMC_BASE + FMC_SDTR1) = 0x02227472;     
    
    /* SDRAM initialization sequence */
    /* Clock enable command */
    HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000011; 
    while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
    
    /* Delay */
    rt_hw_us_delay(120000);
    
    /* PALL command */
    HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000012;  
    while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
    
    /* Auto refresh command */
    HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000073;
    while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
    HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000073;
    while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
    
    /* MRD register program */
    HWREG32(FMC_BASE + FMC_SDCMR) = 0x00046014;
    while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0); 
    
    /* Set refresh count */
    HWREG32(FMC_BASE + FMC_SDRTR) |= (604<<1);//(680<<1);
    
    /* Disable write protection */
    HWREG32(FMC_BASE + FMC_SDCR1) &= 0xFFFFFDFF;
#else   // 120M主频的配置，SDRAM时钟为60M
    /* Configure and enable SDRAM bank1 */
    HWREG32(FMC_BASE + FMC_SDCR1) = 0x000029D9; //0x000029D9;
    HWREG32(FMC_BASE + FMC_SDTR1) = 0x02227473;     
    
    /* SDRAM initialization sequence */
    /* Clock enable command */
    HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000011; 
    while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
    
    /* Delay */
    rt_hw_us_delay(120000);
    
    /* PALL command */
    HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000012;  
    while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
    
    /* Auto refresh command */
    HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000073;
    while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
    HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000073;
    while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
    
    /* MRD register program */
    HWREG32(FMC_BASE + FMC_SDCMR) = 0x00046014;
    while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0); 
    
    /* Set refresh count */
    HWREG32(FMC_BASE + FMC_SDRTR) |= (448<<1); // (448<<1);
    
    /* Disable write protection */
    HWREG32(FMC_BASE + FMC_SDCR1) &= 0xFFFFFDFF;
#endif
    
#endif
    
  /*==================================*/
  /* Choose if segment initialization */
  /* should be done or not.           */
  /* Return: 0 to omit seg_init       */
  /*         1 to run seg_init        */
  /*==================================*/
  return 1;
}

#pragma language=default

#ifdef __cplusplus
}
#endif
