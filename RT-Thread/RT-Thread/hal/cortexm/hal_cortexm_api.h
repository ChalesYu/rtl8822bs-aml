/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           hal_cortexm_api.h
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
#ifndef __HAL_CORTEXM_API_H__
#define __HAL_CORTEXM_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
**  定义最大允许的本平台的中断向量表个数
*********************************************************************************************************/
#define  MAX_HANDLERS           156

/*********************************************************************************************************
**  本平台的中断优先级划分, 该定义仅仅针对CortexM系列处理器
*********************************************************************************************************/
#define INT_NUM_PRIORITY       8
#define INT_NUM_PRIORITY_BITS  4
#define INT_PRIORITY_BIT_START (INT_NUM_PRIORITY - INT_NUM_PRIORITY_BITS)

/*********************************************************************************************************
**  Cortex-M3的内部中断
*********************************************************************************************************/
#define HAL_INT_NMI             2           // NMI fault
#define HAL_INT_HARD            3           // Hard fault
#define HAL_INT_MPU             4           // MPU fault
#define HAL_INT_BUS             5           // Bus fault
#define HAL_INT_USAGE           6           // Usage fault
#define HAL_INT_SVCALL          11          // SVCall
#define HAL_INT_DEBUG           12          // Debug monitor
#define HAL_INT_PENDSV          14          // PendSV
#define HAL_INT_SYSTICK         15          // System Tick

/*********************************************************************************************************
**  Cortex-M3的外部中断起始编号
*********************************************************************************************************/
#define HAL_EXTERN_INT_BASE     16          // 外部中断从16开始

/*********************************************************************************************************
** 启动系统复位的宏函数定义
*********************************************************************************************************/
#define  hal_cpu_reset()   (HAL_WRITE_UINT32(0xE000ED0C,0x05FA0004))
  
/*********************************************************************************************************
** MPU特性参数定义
*********************************************************************************************************/ 
#define  MPU_HFNMI_PRIVDEF_NONE      ((rt_uint32_t)0x00000000)
#define  MPU_HARDFAULT_NMI           ((rt_uint32_t)0x00000002)
#define  MPU_PRIVILEGED_DEFAULT      ((rt_uint32_t)0x00000004)
#define  MPU_HFNMI_PRIVDEF           ((rt_uint32_t)0x00000006)

/*********************************************************************************************************
** MPU控制参数定义
*********************************************************************************************************/
//  区域使能定义
#define  MPU_REGION_ENABLE              0x00000001UL
#define  MPU_REGION_DISABLE             0x00000000UL

// 保护的区域大小定义
#define MPU_REGION_SIZE_32B             (0x04UL << 1)
#define MPU_REGION_SIZE_64B             (0x05UL << 1)
#define MPU_REGION_SIZE_128B            (0x06UL << 1)
#define MPU_REGION_SIZE_256B            (0x07UL << 1)
#define MPU_REGION_SIZE_512B            (0x08UL << 1)
#define MPU_REGION_SIZE_1KB             (0x09UL << 1)
#define MPU_REGION_SIZE_2KB             (0x0AUL << 1)
#define MPU_REGION_SIZE_4KB             (0x0BUL << 1)
#define MPU_REGION_SIZE_8KB             (0x0CUL << 1)
#define MPU_REGION_SIZE_16KB            (0x0DUL << 1)
#define MPU_REGION_SIZE_32KB            (0x0EUL << 1)
#define MPU_REGION_SIZE_64KB            (0x0FUL << 1)
#define MPU_REGION_SIZE_128KB           (0x10UL << 1)
#define MPU_REGION_SIZE_256KB           (0x11UL << 1)
#define MPU_REGION_SIZE_512KB           (0x12UL << 1)
#define MPU_REGION_SIZE_1MB             (0x13UL << 1)
#define MPU_REGION_SIZE_2MB             (0x14UL << 1)
#define MPU_REGION_SIZE_4MB             (0x15UL << 1)
#define MPU_REGION_SIZE_8MB             (0x16UL << 1)
#define MPU_REGION_SIZE_16MB            (0x17UL << 1)
#define MPU_REGION_SIZE_32MB            (0x18UL << 1)
#define MPU_REGION_SIZE_64MB            (0x19UL << 1)
#define MPU_REGION_SIZE_128MB           (0x1AUL << 1)
#define MPU_REGION_SIZE_256MB           (0x1BUL << 1)
#define MPU_REGION_SIZE_512MB           (0x1CUL << 1)
#define MPU_REGION_SIZE_1GB             (0x1DUL << 1)
#define MPU_REGION_SIZE_2GB             (0x1EUL << 1)
#define MPU_REGION_SIZE_4GB             (0x1FUL << 1)

// Flags for the sub-region disable
#define MPU_SUB_RGN_DISABLE_0           0x00000100UL
#define MPU_SUB_RGN_DISABLE_1           0x00000200UL
#define MPU_SUB_RGN_DISABLE_2           0x00000400UL
#define MPU_SUB_RGN_DISABLE_3           0x00000800UL
#define MPU_SUB_RGN_DISABLE_4           0x00001000UL
#define MPU_SUB_RGN_DISABLE_5           0x00002000UL
#define MPU_SUB_RGN_DISABLE_6           0x00004000UL
#define MPU_SUB_RGN_DISABLE_7           0x00008000UL

// MPU_Access_Shareable CORTEX MPU Instruction Access Shareable
#define MPU_ACCESS_SHAREABLE            (0x01UL << 18)
#define MPU_ACCESS_NOT_SHAREABLE        (0x00UL << 18)

// MPU_Access_Bufferable CORTEX MPU Instruction Access Bufferable
#define MPU_ACCESS_BUFFERABLE           (0x01UL << 16)
#define MPU_ACCESS_NOT_BUFFERABLE       (0x00UL << 16)

// MPU_Access_Cacheable CORTEX MPU Instruction Access Cacheable
#define MPU_ACCESS_CACHEABLE            (0x01UL << 17)
#define MPU_ACCESS_NOT_CACHEABLE        (0x00UL << 17)

// MPU_TEX_Levels MPU TEX Levels
#define MPU_TEX_LEVEL0                  (0x00UL << 19)
#define MPU_TEX_LEVEL1                  (0x01UL << 19)
#define MPU_TEX_LEVEL2                  (0x02UL << 19)

// MPU_Region_Permission_Attributes CORTEX MPU Region Permission Attributes
#define MPU_REGION_NO_ACCESS            (0x00UL << 24)
#define MPU_REGION_PRIV_RW              (0x01UL << 24)
#define MPU_REGION_PRIV_RW_URO          (0x02UL << 24)
#define MPU_REGION_FULL_ACCESS          (0x03UL << 24)
#define MPU_REGION_PRIV_RO              (0x05UL << 24)
#define MPU_REGION_PRIV_RO_URO          (0x06UL << 24)

// MPU_Instruction_Access CORTEX MPU Instruction Access
#define MPU_INSTRUCTION_ACCESS_ENABLE   (0x00UL << 28)
#define MPU_INSTRUCTION_ACCESS_DISABLE  (0x01UL << 28)


/*********************************************************************************************************
**  实现的外部函数声明
*********************************************************************************************************/
extern rt_uint8_t hal_enter_critical(rt_uint8_t u8BasePriMask);
extern void hal_exit_critical(rt_uint8_t u8Previous);
extern void hal_enter_sleep(void);
extern void hal_app_jump(rt_uint32_t u32AppAddr);

extern void hal_int_register(rt_uint32_t u32IntNo, void (*pfnHandler)(void));
extern void hal_int_unregister(rt_uint32_t u32IntNo);
extern void hal_int_enable(rt_uint32_t u32IntNo);
extern void hal_int_disable(rt_uint32_t u32IntNo);
extern void hal_int_all_disable(void);
extern void hal_int_priority_group_set(rt_uint32_t u32Bits);
extern rt_uint32_t hal_int_priority_group_get(rt_uint32_t u32Bits);
extern void hal_int_priority_set(rt_uint32_t u32IntNo, rt_uint8_t u8Priority);
extern rt_uint8_t hal_int_priority_get(rt_uint32_t u32IntNo);
extern void hal_int_systick_init(rt_uint32_t clksource, rt_uint32_t clock,
		  rt_uint32_t count, rt_uint8_t intprio,
		  void (*pfnHandler)(void));
extern void hal_mpu_disable(void);
extern void hal_mpu_enable(rt_uint32_t u32Ctrl);
extern void hal_mpu_region_config(rt_uint8_t u8Number, rt_uint32_t u32Addr, rt_uint32_t u32Config);

#ifdef __cplusplus
    }
#endif      // __cplusplus
    
#endif      // __HAL_CORTEXM_API_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
