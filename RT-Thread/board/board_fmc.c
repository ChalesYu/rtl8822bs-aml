/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_fmc.c
** Last modified Date:  2017-04-18
** Last Version:        V1.00
** Description:         �ⲿ���߽ӿڳ�ʼ�����������
**                      SDRAM�� ����ʹ�õĻ���SDRAM���ͺţ�W9825G6KH
**                      NAND Flash��������
**                      SRAM��      ������
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2017-04-18
** Version:             V1.00
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

#include <string.h>

#include "hal/cortexm/hal_cortexm_api.h"
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_gpio.h"
#include "hal/arch/inc/arch_io_syscfg.h"

/*********************************************************************************************************
�������� 
*********************************************************************************************************/
#define  FSMC_SRAM16_EN         0
#define  FMC_SDRAM_EN           1

/*********************************************************************************************************
** Function name:       rt_hw_fmc_pin_init
** Descriptions:        ��ʼ��ʹ�õ�ʹ�õ�FMC����,���ͬʱʹ��RAM��NAND������Ҫһ��һ����ʼ��
** Input parameters:    NONE
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static int rt_hw_fmc_pin_init(void)
{
  /*
   * SDRAM��������
  ****************************************************************************
  * PC3 --> SDCKE0  | PE0 --> NBL0  | PF0 --> A0    | PG0 --> A10            *
  * PC2 --> SDNE0   | PE1 --> NBL1
  * PC0 --> SDNWE   | PE7 --> D4    | PF1 --> A1    | PG1 --> A11            *
  * ----------------| PE8 --> D5    | PF2 --> A2    | PG2 --> A12            *
  * ----------------l| PE9 --> D6   | PF3 --> A3    | PG4 --> BA0            *
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
  SystemPeripheralEnable(RCC_PERIPHERAL_FMC);
  
  // ����IOΪFSMC����
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
  
  
  // ����ΪAF push pull
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
  
  return 0;
}

#if (FSMC_SRAM16_EN > 0) 
/*********************************************************************************************************
** Function name:       rt_hw_fmc_sram16_init
** Descriptions:        SRAM��������ʼ��������ʹ��ISSI�뵼���2Mbytes�洢�������ͺţ�IS61WV102416BLL
** Input parameters:    NONE
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static int rt_hw_fmc_sram16_init(void)
{
  // ��ʼ����16λSRAM��FSMC
  HWREG32(FSMC_BASE + FSMC_BCR3) = 0x00001011;   // ��дʱ��һ��������Ҫ����BWTR�Ĵ���
  // HWREG32(FSMC_BASE + FSMC_BCR3) = 0x00005011; // ��д�Ĵ�������ʱ����Ҫ����BWTR�Ĵ�����дʱ��
  HWREG32(FSMC_BASE + FSMC_BTR3) = 0x00000300;
  if(HWREG32(FSMC_BASE + FSMC_BCR3) & 0x4000) {
    HWREG32(FSMC_BASE + FSMC_BWTR3) = 0x00000300;
  } else {
    HWREG32(FSMC_BASE + FSMC_BWTR3) = 0x0FFFFFFF;
  }
  
  return 0;
}
#endif

#if (FMC_SDRAM_EN > 0)
/*********************************************************************************************************
** Function name:       rt_hw_fmc_sdram_init
** Descriptions:        SDRAM��������ʼ��������ʹ�û���SDRAM���ͺţ�W9825G6KH
** Input parameters:    NONE
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static int rt_hw_fmc_sdram_init(void)
{
#if  1
  // 192M�����ã�SDRAMʱ��Ϊ96M
  /* Configure and enable SDRAM bank1 */
  HWREG32(FMC_BASE + FMC_SDCR1) = 0x00000BD9;  // ע��оƬbug������ʹ��RBURSTλ
  HWREG32(FMC_BASE + FMC_SDTR1) = 0x01115561;     
  
  /* SDRAM initialization sequence */
  /* Clock enable command */
  HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000011; 
  while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
  
  /* ��ʱ����200us */
  rt_hw_us_delay(300);
  
  /* PALL command */
  HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000012;  
  while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
  
  /* Auto refresh command */
  HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000113;
  while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
  HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000113;
  while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
  
  /* MRD register program */
  HWREG32(FMC_BASE + FMC_SDCMR) = 0x00046614;
  while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0); 
  
  /* Set refresh count */
  HWREG32(FMC_BASE + FMC_SDRTR) |= (730<<1);
  
  /* Disable write protection */
  HWREG32(FMC_BASE + FMC_SDCR1) &= 0xFFFFFDFF;
#else
  // 180M�����ã�SDRAMʱ��Ϊ90M
  /* Configure and enable SDRAM bank1 */
  HWREG32(FMC_BASE + FMC_SDCR1) = 0x00000BD9;  // ע��оƬbug������ʹ��RBURSTλ
  HWREG32(FMC_BASE + FMC_SDTR1) = 0x01115561;     
  
  /* SDRAM initialization sequence */
  /* Clock enable command */
  HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000011; 
  while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
  
   /* ��ʱ����200us */
  rt_hw_us_delay(300);
  
  /* PALL command */
  HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000012;  
  while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
  
  /* Auto refresh command */
  HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000113;
  while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
  HWREG32(FMC_BASE + FMC_SDCMR) = 0x00000113;
  while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0);
  
  /* MRD register program */
  HWREG32(FMC_BASE + FMC_SDCMR) = 0x00046614;
  while((HWREG32(FMC_BASE + FMC_SDSR) & 0x00000020) != 0); 
  
  /* Set refresh count */
  HWREG32(FMC_BASE + FMC_SDRTR) |= (700<<1);
  
  /* Disable write protection */
  HWREG32(FMC_BASE + FMC_SDCR1) &= 0xFFFFFDFF;
#endif
  
  return 0;
}
#endif

/*********************************************************************************************************
** Function name:       rt_hw_fmc_init
** Descriptions:        FMC��ʼ��
** Input parameters:    NONE
** Output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static int rt_hw_fmc_init(void)
{
  /**
  * Step1, ��ʼ������
  */
  rt_hw_fmc_pin_init();
  
  /**
  * Step2, ��ʼ����Ҫ�Ĵ洢��
  */
#if (FSMC_SRAM16_EN > 0) 
  rt_hw_fmc_sram16_init();
#endif
  
#if (FMC_SDRAM_EN > 0)
  uint32_t u32Temp;
  // ��SDRAM��ַ��0xC0000000ӳ�䵽0x80000000
  u32Temp = HWREG32(SYSCFG_BASE + SYSCFG_MEMRMP);
  u32Temp &= 0xFFFFF3FF;
  u32Temp |= 0x00000400;
  HWREG32(SYSCFG_BASE + SYSCFG_MEMRMP) = u32Temp;
  
  rt_hw_fmc_sdram_init();
#endif 
  
  return 0;
}

/*********************************************************************************************************
** �����Զ���ʼ������
*********************************************************************************************************/
INIT_BOARD_EXPORT(rt_hw_fmc_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/

