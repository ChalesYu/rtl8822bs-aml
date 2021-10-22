/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_rcc.c
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         rcc模块寄存器封装函数实现
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
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"

/*********************************************************************************************************
  时钟配置宏转变
*********************************************************************************************************/
#if (RCC_CLOCK_SYSCLK_DIV == 1)
#define RCC_CLOCK_SYSCLK_DIV_VALUE   (0UL<<8)
#elif (RCC_CLOCK_SYSCLK_DIV == 2)
#define RCC_CLOCK_SYSCLK_DIV_VALUE   (8UL<<8)
#elif (RCC_CLOCK_SYSCLK_DIV == 4)
#define RCC_CLOCK_SYSCLK_DIV_VALUE   (9UL<<8)
#elif (RCC_CLOCK_SYSCLK_DIV == 8)
#define RCC_CLOCK_SYSCLK_DIV_VALUE   (10UL<<8)
#elif (RCC_CLOCK_SYSCLK_DIV == 16)
#define RCC_CLOCK_SYSCLK_DIV_VALUE   (11UL<<8)
#elif (RCC_CLOCK_SYSCLK_DIV == 64)
#define RCC_CLOCK_SYSCLK_DIV_VALUE   (12UL<<8)
#elif (RCC_CLOCK_SYSCLK_DIV == 128)
#define RCC_CLOCK_SYSCLK_DIV_VALUE   (13UL<<8)
#elif (RCC_CLOCK_SYSCLK_DIV == 256)
#define RCC_CLOCK_SYSCLK_DIV_VALUE   (14UL<<8)
#elif (RCC_CLOCK_SYSCLK_DIV == 512)
#define RCC_CLOCK_SYSCLK_DIV_VALUE   (15UL<<8)
#else
#error "错误的SYS CLK除数配置"
#endif

#if (RCC_CLOCK_AHB_DIV == 1)
#define RCC_CLOCK_AHB_DIV_VALUE   (0UL<<0)
#elif (RCC_CLOCK_AHB_DIV == 2)
#define RCC_CLOCK_AHB_DIV_VALUE   (8UL<<0)
#elif (RCC_CLOCK_AHB_DIV == 4)
#define RCC_CLOCK_AHB_DIV_VALUE   (9UL<<0)
#elif (RCC_CLOCK_AHB_DIV == 8)
#define RCC_CLOCK_AHB_DIV_VALUE   (10UL<<0)
#elif (RCC_CLOCK_AHB_DIV == 16)
#define RCC_CLOCK_AHB_DIV_VALUE   (11UL<<0)
#elif (RCC_CLOCK_AHB_DIV == 64)
#define RCC_CLOCK_AHB_DIV_VALUE   (12UL<<0)
#elif (RCC_CLOCK_AHB_DIV == 128)
#define RCC_CLOCK_AHB_DIV_VALUE   (13UL<<0)
#elif (RCC_CLOCK_AHB_DIV == 256)
#define RCC_CLOCK_AHB_DIV_VALUE   (14UL<<0)
#elif (RCC_CLOCK_AHB_DIV == 512)
#define RCC_CLOCK_AHB_DIV_VALUE   (15UL<<0)
#else
#error "错误的AHB CLK除数配置"
#endif

#if (RCC_CLOCK_APB3_DIV == 1)
#define RCC_CLOCK_APB3_DIV_VALUE  (0UL<<4)
#elif (RCC_CLOCK_APB3_DIV == 2)
#define RCC_CLOCK_APB3_DIV_VALUE  (4UL<<4)
#elif (RCC_CLOCK_APB3_DIV == 4)
#define RCC_CLOCK_APB3_DIV_VALUE  (5UL<<4)
#elif (RCC_CLOCK_APB3_DIV == 8)
#define RCC_CLOCK_APB3_DIV_VALUE  (6UL<<4)
#elif (RCC_CLOCK_APB3_DIV == 16)
#define RCC_CLOCK_APB3_DIV_VALUE  (7UL<<4)
#else
#error "错误的APB3 CLK除数配置"
#endif

#if (RCC_CLOCK_APB2_DIV == 1)
#define RCC_CLOCK_APB2_DIV_VALUE  (0UL<<8)
#elif (RCC_CLOCK_APB2_DIV == 2)
#define RCC_CLOCK_APB2_DIV_VALUE  (4UL<<8)
#elif (RCC_CLOCK_APB2_DIV == 4)
#define RCC_CLOCK_APB2_DIV_VALUE  (5UL<<8)
#elif (RCC_CLOCK_APB2_DIV == 8)
#define RCC_CLOCK_APB2_DIV_VALUE  (6UL<<8)
#elif (RCC_CLOCK_APB2_DIV == 16)
#define RCC_CLOCK_APB2_DIV_VALUE  (7UL<<8)
#else
#error "错误的APB2 CLK除数配置"
#endif

#if (RCC_CLOCK_APB1_DIV == 1)
#define RCC_CLOCK_APB1_DIV_VALUE  (0UL<<4)
#elif (RCC_CLOCK_APB1_DIV == 2)
#define RCC_CLOCK_APB1_DIV_VALUE  (4UL<<4)
#elif (RCC_CLOCK_APB1_DIV == 4)
#define RCC_CLOCK_APB1_DIV_VALUE  (5UL<<4)
#elif (RCC_CLOCK_APB1_DIV == 8)
#define RCC_CLOCK_APB1_DIV_VALUE  (6UL<<4)
#elif (RCC_CLOCK_APB1_DIV == 16)
#define RCC_CLOCK_APB1_DIV_VALUE  (7UL<<4)
#else
#error "错误的APB1 CLK除数配置"
#endif

#if (RCC_CLOCK_APB4_DIV == 1)
#define RCC_CLOCK_APB4_DIV_VALUE  (0UL<<4)
#elif (RCC_CLOCK_APB4_DIV == 2)
#define RCC_CLOCK_APB4_DIV_VALUE  (4UL<<4)
#elif (RCC_CLOCK_APB4_DIV == 4)
#define RCC_CLOCK_APB4_DIV_VALUE  (5UL<<4)
#elif (RCC_CLOCK_APB4_DIV == 8)
#define RCC_CLOCK_APB4_DIV_VALUE  (6UL<<4)
#elif (RCC_CLOCK_APB4_DIV == 16)
#define RCC_CLOCK_APB4_DIV_VALUE  (7UL<<4)
#else
#error "错误的APB4 CLK除数配置"
#endif

/*********************************************************************************************************
  局部变量定义
*********************************************************************************************************/
//static __no_init rt_uint32_t __GSysClock;
//static __no_init rt_uint32_t __GAHBClock;
//static __no_init rt_uint32_t __GAPB1Clock;
//static __no_init rt_uint32_t __GAPB2Clock;

static  rt_uint32_t __GSysClock;
static  rt_uint32_t __GAHBClock;
static  rt_uint32_t __GAPB1Clock;
static  rt_uint32_t __GAPB2Clock;
static  rt_uint32_t __GAPB3Clock;
static  rt_uint32_t __GAPB4Clock;
static  rt_uint32_t __GPLL1_Q_Clock;
static  rt_uint32_t __GPLL1_R_Clock;

#if ((RCC_PLL_SRC_USING == RCC_PLL_SRC_HSI) || (RCC_SYSCLK_USING == RCC_SYSCLK_USE_HSI))
static const rt_uint32_t __GHSIRangeTable[4] = 
{64000000,32000000,16000000,8000000};
#endif

// 外设复位寄存器描述
static const rt_uint32_t __GSysPeripheralResetReg[] = {
    RCC_AHB3RSTR,
    RCC_AHB1RSTR,
    RCC_AHB2RSTR,
    RCC_AHB4RSTR,
    RCC_APB3RSTR,
    RCC_APB1LRSTR,
    RCC_APB1HRSTR,
    RCC_APB2RSTR,
    RCC_APB4RSTR
};

// 外设时钟时能寄存器描述
static const rt_uint32_t __GSysPeripheralEnableReg[] = {
    RCC_AHB3ENR,
    RCC_AHB1ENR,
    RCC_AHB2ENR,
    RCC_AHB4ENR,
    RCC_APB3ENR,
    RCC_APB1LENR,
    RCC_APB1HENR,
    RCC_APB2ENR,
    RCC_APB4ENR
};

// 睡眠模式外设时钟时能寄存器描述
static const rt_uint32_t __GSysPeripheralLPModeEnableReg[] = {
    RCC_AHB3LPENR,
    RCC_AHB1LPENR,
    RCC_AHB2LPENR,
    RCC_AHB4LPENR,
    RCC_APB3LPENR,
    RCC_APB1LLPENR,
    RCC_APB1HLPENR,
    RCC_APB2LPENR,
    RCC_APB4LPENR
};

/*********************************************************************************************************
** Function name:       SystemFlashConfig
** Descriptions:        Configure Flash prefetch, Instruction cache, Data cache and wait state
** input parameters:    hclkclock  AXI CLK时钟频率。VOS1电压为scale1，工作在1.15到1.26
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void SystemFlashConfig(rt_uint32_t hclkclock)
{
  hclkclock /= 1000000;
  if(hclkclock <= 70) {
    HWREG32(FLASH_BASE + FLASH_ACR1) = ((0<<4) | 0x00);
    HWREG32(FLASH_BASE + FLASH_ACR2) = ((0<<4) | 0x00);
  }
  else if(hclkclock <= 140) {
    HWREG32(FLASH_BASE + FLASH_ACR1) = ((1<<4) | 0x01);
    HWREG32(FLASH_BASE + FLASH_ACR2) = ((1<<4) | 0x01);
  }
  else if(hclkclock <= 210) {
    HWREG32(FLASH_BASE + FLASH_ACR1) = ((2<<4) | 0x02);
    HWREG32(FLASH_BASE + FLASH_ACR2) = ((2<<4) | 0x02);
  }
  else if(hclkclock <= 225) {
    HWREG32(FLASH_BASE + FLASH_ACR1) = ((2<<4) | 0x03);
    HWREG32(FLASH_BASE + FLASH_ACR2) = ((2<<4) | 0x03);
  } else {
    HWREG32(FLASH_BASE + FLASH_ACR1) = ((2<<4) | 0x07);
    HWREG32(FLASH_BASE + FLASH_ACR2) = ((2<<4) | 0x07);
  }
}

/*********************************************************************************************************
** Function name:       SystemClockInit
** Descriptions:        系统时钟初始化
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SystemClockInit(void)
{
#if  (RCC_CLOCK_CONFIG_EN > 0)
  rt_uint32_t u32InputClock = 0;
  /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set HSION bit */
  HWREG32(RCC_BASE + RCC_CR) |= 0x00000001;
  HWREG32(RCC_BASE + RCC_CFGR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_CR) &= (uint32_t)0xEAF6ED7F;
  HWREG32(RCC_BASE + RCC_D1CFGR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_D2CFGR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_D3CFGR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_PLLCKSELR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_PLLCFGR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_PLL1DIVR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_PLL1FRACR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_PLL2DIVR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_PLL2FRACR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_PLL3DIVR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_PLL3FRACR) = 0x00000000;
  HWREG32(RCC_BASE + RCC_CR) &= (uint32_t)0xFFFBFFFF;
  // 关闭时钟所有中断
  HWREG32(RCC_BASE + RCC_CIER) = 0x00000000;
  
  /* Change  the switch matrix read issuing capability to 1 for the AXI SRAM target (Target 7) */
  HWREG32(0x51008108) = 0x00000001;
  
  // Supply configuration update enable
  HWREG32(PWR_BASE + PWR_CR3) &= ~((rt_uint32_t)1 << 2);
  u32InputClock = HWREG32(PWR_BASE + PWR_CR3);
  
  // 允许工作哎最高主频
  HWREG32(PWR_BASE + PWR_D3CR) |= ((rt_uint32_t)3 << 14);
  
  // 等待内核电压稳定
  while(!(HWREG32(PWR_BASE + PWR_D3CR) & 0x00002000));
  
  // Configure Flash wait state
  HWREG32(FLASH_BASE + FLASH_ACR1) = ((2<<4) | 0x0F);
  HWREG32(FLASH_BASE + FLASH_ACR2) = ((2<<4) | 0x0F);
  
  // AHB CLK
  HWREG32(RCC_BASE + RCC_D1CFGR) = RCC_CLOCK_SYSCLK_DIV_VALUE | RCC_CLOCK_AHB_DIV_VALUE | RCC_CLOCK_APB3_DIV_VALUE;
  // PCLK2
  HWREG32(RCC_BASE + RCC_D2CFGR) = RCC_CLOCK_APB1_DIV_VALUE | RCC_CLOCK_APB2_DIV_VALUE;
  // PCLK1
  HWREG32(RCC_BASE + RCC_D3CFGR) = RCC_CLOCK_APB4_DIV_VALUE;
    
#if (RCC_PLL_SRC_USING == RCC_PLL_SRC_NO)
  
 // HWREG32(RCC_BASE + RCC_PLLCFGR) &= 0xFFFFFFFC;
#if (RCC_SYSCLK_USING == RCC_SYSCLK_USE_HSI)
  u32InputClock = __GHSIRangeTable[RCC_HSI_USING];
  __GSysClock = u32InputClock;
  HWREG32(RCC_BASE + RCC_CR) |= (1UL << 0) | (RCC_HSI_USING << 3);
  while(!(HWREG32(RCC_BASE + RCC_CR) & 0x00000020));
  while(!(HWREG32(RCC_BASE + RCC_CR) & 0x00000004));
#elif (RCC_SYSCLK_USING == RCC_SYSCLK_USE_CSI)
  u32InputClock = 4000000;
  __GSysClock = u32InputClock;
  HWREG32(RCC_BASE + RCC_CR) |= (1 << 7);
  while(!(HWREG32(RCC_BASE + RCC_CR) & 0x00000100));
  HWREG32(RCC_BASE + RCC_CFGR) |= 0x00000001;
  while((HWREG32(RCC_BASE + RCC_CFGR) & 0x00000038) != 0x00000008);
#elif (RCC_SYSCLK_USING == RCC_SYSCLK_USE_HSE)
  u32InputClock = RCC_HSE_INPUT_CLOCK;
  __GSysClock = u32InputClock;
  HWREG32(RCC_BASE + RCC_CR) |= (1 << 16) | (1 << 19);
  while(!(HWREG32(RCC_BASE + RCC_CR) & 0x00020000));
  HWREG32(RCC_BASE + RCC_CFGR) |= 0x00000002;
  while((HWREG32(RCC_BASE + RCC_CFGR) & 0x000000038) != 0x00000010);
#elif (RCC_SYSCLK_USING == RCC_SYSCLK_USE_PLL)
#error "未使能PLL，不能使用PLL输出作为系统时钟！"
#endif

#elif (RCC_PLL_SRC_USING == RCC_PLL_SRC_HSI)
  u32InputClock = __GHSIRangeTable[RCC_HSI_USING];
  HWREG32(RCC_BASE + RCC_CR) |= (1UL << 0) | (RCC_HSI_USING << 3);
  while(!(HWREG32(RCC_BASE + RCC_CR) & 0x00000020));
  while(!(HWREG32(RCC_BASE + RCC_CR) & 0x00000004));
  
  // 配置PLL1 P时钟输出，配置PLL1 Q时钟输出, 配置PLL1 R时钟输出
  HWREG32(RCC_BASE + RCC_PLLCFGR) |=  ((1 << 16) | (1 << 17) | (1 << 18));
  
  // 配置PLL输入系数
  HWREG32(RCC_BASE + RCC_PLLCFGR) |= (RCC_PLL1_SRC_RANGE_USING << 2);
  HWREG32(RCC_BASE + RCC_PLLCKSELR) |= (RCC_CLOCK_PLL1_SRC_DIV << 4) | (0 << 0);
  HWREG32(RCC_BASE + RCC_PLL1FRACR) = 0;
  
  // 配置PLL
  HWREG32(RCC_BASE + RCC_PLL1DIVR) |= ((RCC_CLOCK_PLL1_MUL - 1) | ((RCC_CLOCK_PLL1_P - 1) << 9) |\
                                   ((RCC_CLOCK_PLL1_Q - 1) << 16) | ((RCC_CLOCK_PLL1_R - 1) << 24));
  // 使能PLL
  HWREG32(RCC_BASE + RCC_CR) |= ((rt_uint32_t)1 << 24);
  // 等待PLL锁定
  while(!(HWREG32(RCC_BASE + RCC_CR) & ((rt_uint32_t)1 << 25)));
  // 选择PLL1P输出作为系统时钟源
  HWREG32(RCC_BASE + RCC_CFGR) |= 0x00000003;
  while((HWREG32(RCC_BASE + RCC_CFGR) & 0x000000038) != 0x000000018);
  __GSysClock = u32InputClock /RCC_CLOCK_PLL1_SRC_DIV;
  __GSysClock *=  RCC_CLOCK_PLL1_MUL;
  __GPLL1_Q_Clock = __GSysClock / RCC_CLOCK_PLL1_Q;
  __GPLL1_R_Clock = __GSysClock / RCC_CLOCK_PLL1_R;
  __GSysClock /= RCC_CLOCK_PLL1_P;
  
#elif (RCC_PLL_SRC_USING == RCC_PLL_SRC_CSI)
   u32InputClock = 4000000;
  HWREG32(RCC_BASE + RCC_CR) |= (1 << 7);
  while(!(HWREG32(RCC_BASE + RCC_CR) & 0x00000100));
  
  // 配置PLL1 P时钟输出，配置PLL1 Q时钟输出, 配置PLL1 R时钟输出
  HWREG32(RCC_BASE + RCC_PLLCFGR) |=  ((1 << 16) | (1 << 17) | (1 << 18));
  
  // 配置PLL输入系数
  HWREG32(RCC_BASE + RCC_PLLCFGR) |= (RCC_PLL1_SRC_RANGE_USING << 2);
  HWREG32(RCC_BASE + RCC_PLLCKSELR) |= (RCC_CLOCK_PLL1_SRC_DIV << 4) | (1 << 0);
  HWREG32(RCC_BASE + RCC_PLL1FRACR) = 0;
  
  // 配置PLL
  HWREG32(RCC_BASE + RCC_PLL1DIVR) |= ((RCC_CLOCK_PLL1_MUL - 1) | ((RCC_CLOCK_PLL1_P - 1) << 9) |\
                                   ((RCC_CLOCK_PLL1_Q - 1) << 16) | ((RCC_CLOCK_PLL1_R - 1) << 24));
  // 使能PLL
  HWREG32(RCC_BASE + RCC_CR) |= ((rt_uint32_t)1 << 24);
  // 等待PLL锁定
  while(!(HWREG32(RCC_BASE + RCC_CR) & ((rt_uint32_t)1 << 25)));
  // 选择PLL1P输出作为系统时钟源
  HWREG32(RCC_BASE + RCC_CFGR) |= 0x00000003;
  while((HWREG32(RCC_BASE + RCC_CFGR) & 0x000000038) != 0x000000018);
  __GSysClock = u32InputClock / RCC_CLOCK_PLL1_SRC_DIV;
  __GSysClock *=  RCC_CLOCK_PLL1_MUL;
  __GPLL1_Q_Clock = __GSysClock / RCC_CLOCK_PLL1_Q;
  __GPLL1_R_Clock = __GSysClock / RCC_CLOCK_PLL1_R;
  __GSysClock /= RCC_CLOCK_PLL1_P;
  
#elif (RCC_PLL_SRC_USING == RCC_PLL_SRC_HSE)
  u32InputClock = RCC_HSE_INPUT_CLOCK;
  HWREG32(RCC_BASE + RCC_CR) |= (1 << 16) | (1 << 19);
  while(!(HWREG32(RCC_BASE + RCC_CR) & 0x00020000));
  
  // 配置PLL1 P时钟输出，配置PLL1 Q时钟输出, 配置PLL1 R时钟输出
  HWREG32(RCC_BASE + RCC_PLLCFGR) |=  ((1 << 16) | (1 << 17) | (1 << 18));
  
  // 配置PLL输入系数
  HWREG32(RCC_BASE + RCC_PLLCFGR) |= (RCC_PLL1_SRC_RANGE_USING << 2);
  HWREG32(RCC_BASE + RCC_PLLCKSELR) |= (RCC_CLOCK_PLL1_SRC_DIV << 4) | (2 << 0);
  HWREG32(RCC_BASE + RCC_PLL1FRACR) = 0;
  
  // 配置PLL
  HWREG32(RCC_BASE + RCC_PLL1DIVR) |= ((RCC_CLOCK_PLL1_MUL - 1) | ((RCC_CLOCK_PLL1_P - 1) << 9) |\
                                   ((RCC_CLOCK_PLL1_Q - 1) << 16) | ((RCC_CLOCK_PLL1_R - 1) << 24));
  // 使能PLL
  HWREG32(RCC_BASE + RCC_CR) |= ((rt_uint32_t)1 << 24);
  // 等待PLL锁定
  while(!(HWREG32(RCC_BASE + RCC_CR) & ((rt_uint32_t)1 << 25)));
  // 选择PLL1P输出作为系统时钟源
  HWREG32(RCC_BASE + RCC_CFGR) |= 0x00000003;
  while((HWREG32(RCC_BASE + RCC_CFGR) & 0x000000038) != 0x000000018);
  __GSysClock = u32InputClock / RCC_CLOCK_PLL1_SRC_DIV;
  __GSysClock *=  RCC_CLOCK_PLL1_MUL;
  __GPLL1_Q_Clock = __GSysClock / RCC_CLOCK_PLL1_Q;
  __GPLL1_R_Clock = __GSysClock / RCC_CLOCK_PLL1_R;
  __GSysClock /= RCC_CLOCK_PLL1_P;
#endif
  
  __GSysClock  = __GSysClock / RCC_CLOCK_SYSCLK_DIV;
  __GAHBClock = __GSysClock / RCC_CLOCK_AHB_DIV;
  __GAPB1Clock = __GAHBClock / RCC_CLOCK_APB1_DIV;
  __GAPB2Clock = __GAHBClock / RCC_CLOCK_APB2_DIV;
  __GAPB3Clock = __GAHBClock / RCC_CLOCK_APB3_DIV;
  __GAPB4Clock = __GAHBClock / RCC_CLOCK_APB4_DIV;
  
  // 根据主频重新配置flash等待周期和预取指
  SystemFlashConfig(__GAHBClock); 
#endif

#if (RCC_USING_HSI48 > 0)
  HWREG32(RCC_BASE + RCC_CR) |= (1UL << 12);
  while(!(HWREG32(RCC_BASE + RCC_CR) & 0x00002000));
  //CRSEN=1,使能CRS
  HWREG32(RCC_BASE + RCC_APB1HENR) |= (1UL << 1);
  //CRSRST=1,复位CRS
  HWREG32(RCC_BASE + RCC_APB1HRSTR) |= (1UL << 1);
  //CRSRST=0,取消复位 
  HWREG32(RCC_BASE + RCC_APB1HRSTR) &= ~(1UL << 1);
  //SYNCSRC[1:0]=0,选择USB2 SOF作为SYNC信号
  HWREG32(CRS_BASE + CRS_CFGR) &= ~(3UL << 28);
  //HWREG32(CRS_BASE + CRS_CFGR) |= (1UL << 28);
  //CEN和AUTIOTRIMEN都为1,使能自动校准以及计数器 
   HWREG32(CRS_BASE + CRS_CR) |= (3UL << 5);
   
   // 设置USB使用HSI48的时钟
   HWREG32(RCC_BASE + RCC_D2CCIP2R) &= ~((rt_uint32_t)(3UL << 20));
   HWREG32(RCC_BASE + RCC_D2CCIP2R) |= (3UL << 20);
#else
  HWREG32(RCC_BASE + RCC_CR) &= ~(1UL << 12);
  
#if (RCC_PLL_SRC_USING == RCC_PLL_SRC_NO)
  #error "必须使能PLL,才能使用PLL3 Q时钟作为48M时钟"
#endif
  
  // 配置PLL3 P时钟输出，配置PLL3 Q时钟输出, 配置PLL1 R时钟输出
  HWREG32(RCC_BASE + RCC_PLLCFGR) |=  ((1 << 22) | (1 << 23) | (1 << 24));
  
  // 配置PLL输入系数
  HWREG32(RCC_BASE + RCC_PLLCFGR) |= (RCC_PLL3_SRC_RANGE_USING << 10);
  HWREG32(RCC_BASE + RCC_PLLCKSELR) |= (RCC_CLOCK_PLL3_SRC_DIV << 20);
  HWREG32(RCC_BASE + RCC_PLL3FRACR) = 0;
  
  // 配置PLL
  HWREG32(RCC_BASE + RCC_PLL3DIVR) |= ((RCC_CLOCK_PLL3_MUL - 1) | ((RCC_CLOCK_PLL3_P - 1) << 9) |\
                                   ((RCC_CLOCK_PLL3_Q - 1) << 16) | ((RCC_CLOCK_PLL3_R - 1) << 24));
  // 使能PLL
  HWREG32(RCC_BASE + RCC_CR) |= ((rt_uint32_t)1 << 28);
  // 等待PLL锁定
  while(!(HWREG32(RCC_BASE + RCC_CR) & ((rt_uint32_t)1 << 29)));
  // 设置USB使用PLL3 Q的时钟
  HWREG32(RCC_BASE + RCC_D2CCIP2R) &= ~((rt_uint32_t)(3UL << 20));
  HWREG32(RCC_BASE + RCC_D2CCIP2R) |= (2UL << 20);
#endif
  
 //CSION=1,使能CSI,为IO补偿单元提供时钟
 HWREG32(RCC_BASE + RCC_CR) |= (1 << 7);

//#if (RCC_USING_LSI > 0)
//  SystemSysLSIOpen();
////#else
////  SystemSysLSIClose();
//#endif
  
//#if (RCC_USING_LSE > 0)
//  SystemSysLSEOpen();
//#else
//  SystemSysLSEClose();
//#endif
}

/*********************************************************************************************************
** Function name:       SystemSysClockGet
** Descriptions:        获取系统主频
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      系统主频
*********************************************************************************************************/
rt_uint32_t SystemSysClockGet(void)
{
    return __GSysClock;
}

/*********************************************************************************************************
** Function name:       SystemSysTickClockGet
** Descriptions:        获取System tick总线时钟频率
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      __GSysClock/8
*********************************************************************************************************/
rt_uint32_t SystemSysTickClockGet(void)
{
  return __GSysClock; ///8;
}

/*********************************************************************************************************
** Function name:       SystemAHBClockGet
** Descriptions:        获取AHB总线时钟频率
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      AHB时钟频率
*********************************************************************************************************/
rt_uint32_t SystemAHBClockGet(void)
{
    return __GAHBClock;
}

/*********************************************************************************************************
** Function name:       SystemAPB1ClockGet
** Descriptions:        获取APB1总线时钟频率
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      APB1时钟频率
*********************************************************************************************************/
rt_uint32_t SystemAPB1ClockGet(void)
{
    return __GAPB1Clock;
}

/*********************************************************************************************************
** Function name:       SystemAPB2ClockGet
** Descriptions:        获取APB2总线时钟频率
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      APB2时钟频率
*********************************************************************************************************/
rt_uint32_t SystemAPB2ClockGet(void)
{
    return __GAPB2Clock;
}

/*********************************************************************************************************
** Function name:       SystemAPB3ClockGet
** Descriptions:        获取APB3总线时钟频率
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      APB2时钟频率
*********************************************************************************************************/
rt_uint32_t SystemAPB3ClockGet(void)
{
    return __GAPB3Clock;
}

/*********************************************************************************************************
** Function name:       SystemAPB4ClockGet
** Descriptions:        获取APB4总线时钟频率
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      APB2时钟频率
*********************************************************************************************************/
rt_uint32_t SystemAPB4ClockGet(void)
{
    return __GAPB4Clock;
}

/*********************************************************************************************************
** Function name:       SystemPeripheralReset
** Descriptions:        复位外设
** input parameters:    u16WhichBus:  外设位于那条总线
**                      u32Ctrl：外设控制位
** output parameters:   NONE
** Returned value:      NONE
注意：两个参数有一个统一的宏定义，具体取值可以参考arch_io_rcc.h中的定义
*********************************************************************************************************/
void SystemPeripheralReset(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl)
{
    HWREG32(RCC_BASE + __GSysPeripheralResetReg[u16WhichBus & 0x00FF]) |= u32Ctrl;
    HWREG32(RCC_BASE + __GSysPeripheralResetReg[u16WhichBus & 0x00FF]) &= ~u32Ctrl;
}

/*********************************************************************************************************
** Function name:       SystemPeripheralAllReset
** Descriptions:        复位CPU的所有外设
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void SystemPeripheralAllReset(void)
{
  HWREG32(RCC_BASE + RCC_AHB3RSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_AHB1RSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_AHB2RSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_AHB4RSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_APB3RSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_APB1LRSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_APB1HRSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_APB2RSTR) = 0xFFFFFFFF;
  HWREG32(RCC_BASE + RCC_APB4RSTR) = 0xFFFFFFFF;
  
  HWREG32(RCC_BASE + RCC_AHB3RSTR) = 0;
  HWREG32(RCC_BASE + RCC_AHB1RSTR) = 0;
  HWREG32(RCC_BASE + RCC_AHB2RSTR) = 0;
  HWREG32(RCC_BASE + RCC_AHB4RSTR) = 0;
  HWREG32(RCC_BASE + RCC_APB3RSTR) = 0;
  HWREG32(RCC_BASE + RCC_APB1LRSTR) = 0;
  HWREG32(RCC_BASE + RCC_APB1HRSTR) = 0;
  HWREG32(RCC_BASE + RCC_APB2RSTR) = 0;
  HWREG32(RCC_BASE + RCC_APB4RSTR) = 0;
}

/*********************************************************************************************************
** Function name:       SystemPeripheralEnable
** Descriptions:        使能外设时钟
** input parameters:    u16WhichBus:  外设位于那条总线
**                      u32Ctrl：外设控制位
** output parameters:   NONE
** Returned value:      NONE
注意：两个参数有一个统一的宏定义，具体取值可以参考arch_io_rcc.h中的定义
*********************************************************************************************************/
void SystemPeripheralEnable(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl)
{
    HWREG32(RCC_BASE + __GSysPeripheralEnableReg[u16WhichBus & 0x00FF]) |= u32Ctrl;
}

/*********************************************************************************************************
** Function name:       SystemPeripheralDisable
** Descriptions:        禁能外设时钟
** input parameters:    u16WhichBus:  外设位于那条总线
**                      u32Ctrl：外设控制位
** output parameters:   NONE
** Returned value:      NONE
注意：两个参数有一个统一的宏定义，具体取值可以参考arch_io_rcc.h中的定义
*********************************************************************************************************/
void SystemPeripheralDisable(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl)
{
    HWREG32(RCC_BASE + __GSysPeripheralEnableReg[u16WhichBus & 0x00FF]) &= ~u32Ctrl;
}

/*********************************************************************************************************
** Function name:       SystemPeripheralLPModeEnable
** Descriptions:        使能睡眠模式外设时钟
** input parameters:    u16WhichBus:  外设位于那条总线
**                      u32Ctrl：外设控制位
** output parameters:   NONE
** Returned value:      NONE
注意：两个参数有一个统一的宏定义，具体取值可以参考arch_io_rcc.h中的定义
*********************************************************************************************************/
void SystemPeripheralLPModeEnable(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl)
{
    HWREG32(RCC_BASE + __GSysPeripheralLPModeEnableReg[u16WhichBus + 0x00FF]) |= u32Ctrl;
}

/*********************************************************************************************************
** Function name:       SystemPeripheralLPModeDisable
** Descriptions:        禁能睡眠模式外设时钟
** input parameters:    u16WhichBus:  外设位于那条总线
**                      u32Ctrl：外设控制位
** output parameters:   NONE
** Returned value:      NONE
注意：两个参数有一个统一的宏定义，具体取值可以参考arch_io_rcc.h中的定义
*********************************************************************************************************/
void SystemPeripheralLPModeDisable(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl)
{
    HWREG32(RCC_BASE + __GSysPeripheralLPModeEnableReg[u16WhichBus & 0x00FF]) &= ~u32Ctrl;
}

/*********************************************************************************************************
** Function name:       SystemPeripheralClockGet
** Descriptions:        获取指定的外设时钟
** input parameters:    u8WhichBus:  外设位于那条总线
**                      u32Ctrl：外设控制位
** output parameters:   NONE
** Returned value:      NONE
注意：两个参数有一个统一的宏定义，具体取值可以参考arch_io_rcc.h中的定义
*********************************************************************************************************/
rt_uint32_t SystemPeripheralClockGet(rt_uint16_t u16WhichBus, rt_uint32_t u32Ctrl)
{
  switch((u16WhichBus >> 8) & 0x00FF)
  {
    // SDMMC, 采用PPL1 Q时钟
  case 0x02:
  case 0x0E:
    return __GPLL1_Q_Clock;
    // FDCAN, 采用HSE
  case 0x3B:
    return RCC_HSE_INPUT_CLOCK;
    // SPDIFRX,, 采用PPL1 Q时钟
  case 0x2E:
    return __GPLL1_Q_Clock;
    // SPI1、SPI2、SPI3,, 采用PPL1 Q时钟
  case 0x2F:
  case 0x30:
  case 0x4A:
    return __GPLL1_Q_Clock;
    // SAI1、SAI2、SAI3, SAI4、DFSDM1, 采用PPL1 Q时钟
  case 0x41:
  case 0x42:
  case 0x43:
  case 0x44:
  case 0x4F:
    return __GPLL1_Q_Clock;
    // HDMICEC，采用LSE时钟
  case 26:
    return 32768;
    //  USBOTG 1 and 2, 采用HSI48时钟
  case 0x08:
  case 0x09:
    return 48000000;
    // RNG模块，配置为PLL1 Q时钟
  case 0x0F:
    HWREG32(RCC_BASE + RCC_D2CCIP2R) &= ~((rt_uint32_t)(3UL << 8));
    HWREG32(RCC_BASE + RCC_D2CCIP2R) |= (1UL << 8);
    return __GPLL1_Q_Clock;
    // SAR ADC时钟，这里以后用到在补充，需要用到PLL2或者PLL3
  case 0x0B:
  case 0x14:
    // 使用PLL3 R时钟
    HWREG32(RCC_BASE + RCC_D3CCIPR) &= ~((rt_uint32_t)(3UL << 16));
    HWREG32(RCC_BASE + RCC_D3CCIPR) |= (1UL << 16);
    return ((RCC_HSE_INPUT_CLOCK / RCC_CLOCK_PLL3_SRC_DIV) * RCC_CLOCK_PLL3_MUL) / RCC_CLOCK_PLL3_R;
  default:
    switch(u16WhichBus & 0x00FF) {
    case 0:
    case 1:
    case 2:
    case 3:
      return SystemAHBClockGet();
    case 4:
      return SystemAPB3ClockGet();
    case 5:
    case 6:
      return SystemAPB1ClockGet();
    case 7:
      return SystemAPB2ClockGet();
    case 8:
      return SystemAPB4ClockGet();
    default:
      break;
    }
  }
  
  return 0;
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
