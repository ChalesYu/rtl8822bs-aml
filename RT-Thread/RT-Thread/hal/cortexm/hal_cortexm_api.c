/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           hal_cortexm_api.c
** Last modified Date:  2019-03-15
** Last Version:        v1.00
** Description:         Cortex-M内核的api接口实现
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2019-03-15
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <string.h>
#include <rthw.h>
#include "hal_cortexm_reg.h"
#include "hal_cortexm_api.h"

/*********************************************************************************************************
全局变量声明 
*********************************************************************************************************/
extern void __hal_app_jump(rt_uint32_t u32AppAddr);

// 中断向量表定义，该表需要放在RAM起始地址
#pragma location=0x00000000
static __no_init void (*__GfnRAMVectors[MAX_HANDLERS])(void); // @ "VTABLE";

// 中断优先级分组值定义
static const rt_uint32_t __GpulPriority[] =
{
  HAL_NVIC_APINT_PRIGROUP_0_8, HAL_NVIC_APINT_PRIGROUP_1_7, HAL_NVIC_APINT_PRIGROUP_2_6,
  HAL_NVIC_APINT_PRIGROUP_3_5, HAL_NVIC_APINT_PRIGROUP_4_4, HAL_NVIC_APINT_PRIGROUP_5_3,
  HAL_NVIC_APINT_PRIGROUP_6_2, HAL_NVIC_APINT_PRIGROUP_7_1
};


/*********************************************************************************************************
** Function name:       IntDefaultHandler
** Descriptions:        默认的中断服务函数定义
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void IntDefaultHandler(void)
{
  // 进入死循环
  while(1)
  {
  }
}
/*********************************************************************************************************
** Function name:       hal_int_register
** Descriptions:        注册中断服务函数
** input parameters:    u32IntNo:   中断服务编号
**                      pfnHandler: 待注册的中断服务函数
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_int_register(rt_uint32_t u32IntNo, void (*pfnHandler)(void))
{
  rt_uint32_t i;
  rt_uint32_t u32Temp;
  
  if(HWREG32(HAL_SCB_VTOR) != (rt_uint32_t)__GfnRAMVectors) {
    // 将Flash中的向量表复制到RAM区的向量表中
    u32Temp = HWREG32(HAL_SCB_VTOR);
    for(i=0; i<HAL_EXTERN_INT_BASE; i++) {
      __GfnRAMVectors[i] = (void (*)(void))HWREG32((i * 4) + u32Temp);
    }
    
    for(; i<MAX_HANDLERS; i++) {
      __GfnRAMVectors[i] = IntDefaultHandler;
    }
    // 复制完毕，将向量表映射到RAM区
    HWREG32(HAL_SCB_VTOR) = (rt_uint32_t)__GfnRAMVectors;
  }
  
  // 注册中断服务函数
  if(pfnHandler != RT_NULL) {
    __GfnRAMVectors[u32IntNo] = pfnHandler;
  }
}

/*********************************************************************************************************
** Function name:       hal_int_unregister
** Descriptions:        注销中断服务函数
** input parameters:    u32IntNo:   中断服务编号
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_int_unregister(rt_uint32_t u32IntNo)
{
  __GfnRAMVectors[u32IntNo] = IntDefaultHandler;
}

/*********************************************************************************************************
** Function name:       hal_int_enable
** Descriptions:        使能中断
** input parameters:    u32IntNo:   中断服务编号
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_int_enable(rt_uint32_t u32IntNo)
{
  // 如果是内部中断
  if(u32IntNo < 16) {
    if(u32IntNo == HAL_INT_MPU){
      // Enable the MemManage interrupt.
      HWREG32(HAL_SCB_SHCSR) |= HAL_NVIC_SYS_HND_CTRL_MEM;
    } else if(u32IntNo == HAL_INT_BUS) {
      // Enable the bus fault interrupt.
      HWREG32(HAL_SCB_SHCSR) |= HAL_NVIC_SYS_HND_CTRL_BUS;
    } else if(u32IntNo == HAL_INT_USAGE) {
      // Enable the usage fault interrupt.
      HWREG32(HAL_SCB_SHCSR) |= HAL_NVIC_SYS_HND_CTRL_USAGE;
    } else if(u32IntNo == HAL_INT_SYSTICK) {
      // Enable the System Tick interrupt.
      HWREG32(HAL_NVIC_ST_CTRL) |= HAL_NVIC_ST_CTRL_INTEN;
    }
    return;
  }
  
  if(u32IntNo > 255) {
    return;
  }
  u32IntNo -= HAL_EXTERN_INT_BASE;
  HWREG32(HAL_NVIC_EN_BASE + ((u32IntNo / 32) * 4)) |= (1 << (u32IntNo % 32));
}

/*********************************************************************************************************
** Function name:       hal_int_disable
** Descriptions:        禁止中断
** input parameters:    u32IntNo:   中断服务编号
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_int_disable(rt_uint32_t u32IntNo)
{
  // 如果是内部中断
  if(u32IntNo < 16) {
    if(u32IntNo == HAL_INT_MPU){
      // Enable the MemManage interrupt.
      HWREG32(HAL_SCB_SHCSR) &= ~HAL_NVIC_SYS_HND_CTRL_MEM;
    } else if(u32IntNo == HAL_INT_BUS) {
      // Enable the bus fault interrupt.
      HWREG32(HAL_SCB_SHCSR) &= ~HAL_NVIC_SYS_HND_CTRL_BUS;
    } else if(u32IntNo == HAL_INT_USAGE) {
      // Enable the usage fault interrupt.
      HWREG32(HAL_SCB_SHCSR) &= ~HAL_NVIC_SYS_HND_CTRL_USAGE;
    } else if(u32IntNo == HAL_INT_SYSTICK) {
      // Enable the System Tick interrupt.
      HWREG32(HAL_NVIC_ST_CTRL) &= ~HAL_NVIC_ST_CTRL_INTEN;
    }
    return;
  }
  
  if(u32IntNo > 255) {
    return;
  }
  
  u32IntNo -= HAL_EXTERN_INT_BASE;
  HWREG32(HAL_NVIC_DIS_BASE + ((u32IntNo / 32) * 4)) |= (1 << (u32IntNo % 32));
}

/*********************************************************************************************************
** Function name:       hal_int_all_disable
** Descriptions:        禁止所有NVIC的中断
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_int_all_disable(void)
{
  // Disable the System Tick interrupt.
  HWREG32(HAL_NVIC_ST_CTRL) &= ~HAL_NVIC_ST_CTRL_INTEN;
  
  memset((void *)HAL_NVIC_DIS_BASE, 0, sizeof(rt_uint32_t) * 8);
}

/*********************************************************************************************************
** Function name:       hal_int_priority_group_set
** Descriptions:        设置中断优先级分组
** input parameters:    u32Bits:   可抢占优先级占用的位数
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_int_priority_group_set(rt_uint32_t u32Bits)
{
  HWREG32(HAL_SCB_AIRCR) = HAL_NVIC_APINT_VECTKEY | __GpulPriority[u32Bits];
}

/*********************************************************************************************************
** Function name:       hal_int_priority_group_get
** Descriptions:        获取中断优先级分组
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      可抢占优先级占用的位数
*********************************************************************************************************/
rt_uint32_t hal_int_priority_group_get(rt_uint32_t u32Bits)
{
  rt_uint32_t u32Temp, i;
  
  u32Temp = HWREG32(HAL_SCB_AIRCR) & HAL_NVIC_APINT_PRIGROUP_M;
  for(i=0; i<INT_NUM_PRIORITY; i++) {
    if(u32Temp == __GpulPriority[i]) {
      break;
    }
  }
  
  return i;
}

/*********************************************************************************************************
** Function name:       hal_int_priority_set
** Descriptions:        设置中断优先级
** input parameters:    u32IntNo:   中断编号
**                      u8Priority: 待设置的中断优先级
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_int_priority_set(rt_uint32_t u32IntNo, rt_uint8_t u8Priority)
{
  // 该函数不设置内核的中断，仅仅设置大于16的外部中断
  if((u32IntNo < 16) || (u32IntNo > 255)) {
    return;
  }
  u32IntNo -= HAL_EXTERN_INT_BASE;
  HWREG8(HAL_NVIC_PRI_BASE + u32IntNo) = u8Priority;
}

/*********************************************************************************************************
** Function name:       hal_int_priority_get
** Descriptions:        获取中断优先级
** input parameters:    u32IntNo:   中断编号
** output parameters:   NONE
** Returned value:      获取的中断优先级
*********************************************************************************************************/
rt_uint8_t hal_int_priority_get(rt_uint32_t u32IntNo)
{
  // 该函数不设置内核的中断，仅仅设置大于16的外部中断
  if((u32IntNo < 16) || (u32IntNo > 255)) {
    return 0;
  }
  u32IntNo -= HAL_EXTERN_INT_BASE;
  return (HWREG8(HAL_NVIC_PRI_BASE + u32IntNo));
}

/*********************************************************************************************************
** Function name:       hal_int_systick_init
** Descriptions:        系统tick初始化
** input parameters:    clksource:	tick的时钟源选择，0：外部时钟源；1：内部时钟源
**                      clock:		systick的时钟频率
**                      count: 		每秒中断的次数
**                      intprio：             中断优先级定义
**                      pfnHandler: 注册的中断服务函数
** output parameters:   NONE
** Returned value:      获取的中断优先级
*********************************************************************************************************/
void hal_int_systick_init(rt_uint32_t clksource, rt_uint32_t clock,
                          rt_uint32_t count, rt_uint8_t intprio,
                          void (*pfnHandler)(void))
{
  rt_uint32_t  cnts;
  
  
  cnts = clock / count;
  
  /* First, register the isr function */
  hal_int_register(HAL_INT_SYSTICK, pfnHandler);
  
  /* Set the timer. */
  HWREG32(HAL_NVIC_ST_RELOAD) = (cnts - 1);
  
  /* Set prio of SysTick handler to min prio. */
  HWREG8(HAL_NVIC_SYSTICK_PRIO) = intprio;
  
  /* Enable timer. */
  if(clksource) {
    HWREG32(HAL_NVIC_ST_CTRL) = ((rt_uint32_t)0x4 | (rt_uint32_t)0x1);
  } else {
    HWREG32(HAL_NVIC_ST_CTRL) = (rt_uint32_t)0x1;
  }
  
  /* Enable timer interrupt. */
  HWREG32(HAL_NVIC_ST_CTRL) |= (rt_uint32_t)0x2;
}

/*********************************************************************************************************
** Function name:       hal_mpu_disable
** Descriptions:        禁能MPU
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_mpu_disable(void)
{
  __asm("DMB");
  
  /* Disable fault exceptions */
  HWREG32(HAL_SCB_SHCSR) &= ~(((rt_uint32_t)1) << 16);

  /* Disable the MPU and clear the control register*/
  HWREG32(HAL_MPU_CTRL) = 0;
}

/*********************************************************************************************************
** Function name:       hal_mpu_enable
** Descriptions:        使能MPU
** input parameters:    u32Ctrl：  MPU控制参数，可以取下列参数
**                       MPU_HFNMI_PRIVDEF_NONE      //
**                       MPU_HARDFAULT_NMI           // 
**                       MPU_PRIVILEGED_DEFAULT      // 
**                       MPU_HFNMI_PRIVDEF           // 
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_mpu_enable(rt_uint32_t u32Ctrl)
{
  /* Enable the MPU */
  HWREG32(HAL_MPU_CTRL) = (u32Ctrl | (1UL << 0));

  /* Enable fault exceptions */
  HWREG32(HAL_SCB_SHCSR) |= (((rt_uint32_t)1) << 16);

  /* Ensure MPU setting take effects */
  __asm("DSB");
  __asm("ISB");
}

/*********************************************************************************************************
** Function name:       hal_mpu_region_config
** Descriptions:        MPU区域配置
** input parameters:    u8Number：  MPU配置区域编号，取0-15
**                      u32Addr：   MPU保护区域地址
**                      u32Config:  MPU保护区域属性，可以取下列几组值的或
**                      //  区域使能定义
**                          MPU_REGION_ENABLE              // 区域使能
**                          MPU_REGION_DISABLE             // 区域禁止
**
**                      // 保护的区域大小定义
**                          MPU_REGION_SIZE_32B
**                          MPU_REGION_SIZE_64B
**                          MPU_REGION_SIZE_128B
**                          MPU_REGION_SIZE_256B
**                          MPU_REGION_SIZE_512B
**                          MPU_REGION_SIZE_1KB
**                          MPU_REGION_SIZE_2KB
**                          MPU_REGION_SIZE_4KB
**                          MPU_REGION_SIZE_8KB
**                          MPU_REGION_SIZE_16KB
**                          MPU_REGION_SIZE_32KB
**                          MPU_REGION_SIZE_64KB
**                          MPU_REGION_SIZE_128KB
**                          MPU_REGION_SIZE_256KB
**                          MPU_REGION_SIZE_512KB
**                          MPU_REGION_SIZE_1MB
**                          MPU_REGION_SIZE_2MB
**                          MPU_REGION_SIZE_4MB
**                          MPU_REGION_SIZE_8MB
**                          MPU_REGION_SIZE_16MB
**                          MPU_REGION_SIZE_32MB
**                          MPU_REGION_SIZE_64MB
**                          MPU_REGION_SIZE_128MB
**                          MPU_REGION_SIZE_256MB
**                          MPU_REGION_SIZE_512MB
**                          MPU_REGION_SIZE_1GB
**                          MPU_REGION_SIZE_2GB
**                          MPU_REGION_SIZE_4GB
**
**                      // Flags for the sub-region disable
**                          MPU_SUB_RGN_DISABLE_0
**                          MPU_SUB_RGN_DISABLE_1
**                          MPU_SUB_RGN_DISABLE_2
**                          MPU_SUB_RGN_DISABLE_3
**                          MPU_SUB_RGN_DISABLE_4
**                          MPU_SUB_RGN_DISABLE_5
**                          MPU_SUB_RGN_DISABLE_6
**                          MPU_SUB_RGN_DISABLE_7
**
**                      // MPU_Access_Shareable CORTEX MPU Instruction Access Shareable
**                          MPU_ACCESS_SHAREABLE
**                          MPU_ACCESS_NOT_SHAREABLE

**                      // MPU_Access_Bufferable CORTEX MPU Instruction Access Bufferable
**                          MPU_ACCESS_BUFFERABLE
**                          MPU_ACCESS_NOT_BUFFERABLE
**
**                      // MPU_Access_Cacheable CORTEX MPU Instruction Access Cacheable
**                          MPU_ACCESS_CACHEABLE
**                          MPU_ACCESS_NOT_CACHEABLE
**
**                      // MPU_TEX_Levels MPU TEX Levels
**                          MPU_TEX_LEVEL0
**                          MPU_TEX_LEVEL1
**                          MPU_TEX_LEVEL2
**
**                      // MPU_Region_Permission_Attributes CORTEX MPU Region Permission Attributes
**                          MPU_REGION_NO_ACCESS
**                          MPU_REGION_PRIV_RW
**                          MPU_REGION_PRIV_RW_URO
**                          MPU_REGION_FULL_ACCESS
**                          MPU_REGION_PRIV_RO
**                          MPU_REGION_PRIV_RO_URO
**
**                      // MPU_Instruction_Access CORTEX MPU Instruction Access
**                          MPU_INSTRUCTION_ACCESS_ENABLE
**                          MPU_INSTRUCTION_ACCESS_DISABLE
**
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_mpu_region_config(rt_uint8_t u8Number, rt_uint32_t u32Addr, rt_uint32_t u32Config)
{
  /* Set the Region number */
  HWREG32(HAL_MPU_RNR) = u8Number;
  
  if (u32Config & MPU_REGION_ENABLE)
  {
    HWREG32(HAL_MPU_RBAR) = u32Addr;
    HWREG32(HAL_MPU_RASR) = u32Config;
  }
  else
  {
    HWREG32(HAL_MPU_RBAR) = 0x00;
    HWREG32(HAL_MPU_RASR) = 0x00;
  }
}

/*********************************************************************************************************
** Function name:       hal_app_jump
** Descriptions:        应用程序跳转，清楚NVIC的中断挂起标志位
** input parameters:    u32AppAddr: 跳转的地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void hal_app_jump(rt_uint32_t u32AppAddr)
{
  /**
  * Step1, 关闭中断
  */
  __asm("  CPSID   I\n");
  
  /**
  * Step2, 关闭NVIC中的所有中断
  */
  memset((void *)HAL_NVIC_DIS_BASE, 0, sizeof(rt_uint32_t) * 8);
  
    /**
  * Step3, 清空NVIC中已经Pending中的所有中断
  */
  memset((void *)HAL_NVIC_PEND_CLEAR_BASE, 0, sizeof(rt_uint32_t) * 8);
  
  /**
  * Step4, 清空Systick中断及已经挂起的中断
  */
  HWREG32(HAL_NVIC_ST_CTRL) = 0;
  HWREG32(HAL_NVIC_INT_CTRL) |= ((1UL << 25) | (1UL << 27));
  
  /**
  * Step5, 执行跳转流程
  */
  __hal_app_jump(u32AppAddr);
}

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
