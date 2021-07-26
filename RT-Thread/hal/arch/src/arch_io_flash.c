/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_flash.c
** Last modified Date:  2013-04-04
** Last Version:        v1.0
** Description:         flash模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2013-04-04
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
#include "hal/arch/inc/arch_io_flash.h"

extern void watchdog_feed(void);
/*********************************************************************************************************
** 需要的宏定义
*********************************************************************************************************/
// Flash解锁密钥
#define FLASH_KEY1              ((rt_uint32_t)0x45670123U)
#define FLASH_KEY2              ((rt_uint32_t)0xCDEF89ABU)
#define FLASH_OPT_KEY1          ((rt_uint32_t)0x08192A3BU)
#define FLASH_OPT_KEY2          ((rt_uint32_t)0x4C5D6E7FU)

// Flash操作SIZE定义
#define FLASH_PSIZE_BYTE        ((rt_uint32_t)0x00000000)
#define FLASH_PSIZE_HALF_WORD   ((rt_uint32_t)0x00000100)
#define FLASH_PSIZE_WORD        ((rt_uint32_t)0x00000200)
#define FLASH_PSIZE_DOUBLE_WORD ((rt_uint32_t)0x00000300)
#define CR_PSIZE_MASK           ((rt_uint32_t)0xFFFFFCFF)

// 扇区号屏蔽位定义
#define FLASH_SECTOR_MASK       ((rt_uint32_t)0xFFFFFF07)

/*********************************************************************************************************
** Flash地址定义
*********************************************************************************************************/
#define FLASH_BANK1_BASE        ((rt_uint32_t)0x08000000) /*!< Base address of : (up to 1 MB) Flash Bank1 accessible over AXI */ 
#define FLASH_BANK2_BASE        ((rt_uint32_t)0x08100000) /*!< Base address of : (up to 1 MB) Flash Bank2 accessible over AXI */ 
#define FLASH_END               ((rt_uint32_t)0x081FFFFF) /*!< FLASH end address                                              */

#define FLASH_MAX_SIZE          0x200000  /* 2MB */
#define FLASH_BANK_SIZE         (FLASH_SIZE >> 1)   /* 1MB */
#define FLASH_SECTOR_SIZE       0x00020000 /* 128 KB */

/*********************************************************************************************************
** Function name:       FlashWaitForLastOperation
** Descriptions:        等待Flash上次操作完成，一般指写或擦除操作
** input parameters:    u32Bank:  操作的Flash Bank
** output parameters:   NONE
** Returned value:      -1: 操作失败； 0：操作成功
*********************************************************************************************************/
int FlashWaitForLastOperation(rt_uint32_t u32Bank)
{
  rt_uint32_t bsyflag; 
  
  if(u32Bank == FLASH_BANK_1)
  {
    bsyflag = 0x00000001U | 0x00000004U | 0x00000002U;
    
    if((HWREG32(FLASH_BASE + FLASH_OPTCR1) & 0x80000000U) == 0)
    {
      while(HWREG32(FLASH_BASE + FLASH_SR1) & bsyflag) {
        watchdog_feed();
      }
    }
    else
    {
      while(HWREG32(FLASH_BASE + FLASH_SR2) & bsyflag) {
        watchdog_feed();
      }
    }      
  }
  else
  {
    bsyflag = 0x00000001U | 0x00000004U | 0x00000002U;
    
    if((HWREG32(FLASH_BASE + FLASH_OPTCR1) & 0x80000000U) == 0)
    {
      while(HWREG32(FLASH_BASE + FLASH_SR2) & bsyflag) {
        watchdog_feed();
      }
    }
    else
    {
      while(HWREG32(FLASH_BASE + FLASH_SR1) & bsyflag) {
        watchdog_feed();
      }
    }    
  }
  
  if((u32Bank == FLASH_BANK_1) && ((HWREG32(FLASH_BASE + FLASH_SR1) & 0x07EE0000)))
  {
    HWREG32(FLASH_BASE + FLASH_CCR1) = 0x07EE0000;
    return -1;
  }
  else if((u32Bank == FLASH_BANK_2) && ((HWREG32(FLASH_BASE + FLASH_SR2) & 0x07EE0000)))
  {
    HWREG32(FLASH_BASE + FLASH_CCR2) = 0x07EE0000;
    return -1;
  }
  
  return 0;
}

/*********************************************************************************************************
** Function name:       FlashSetLatency
** Descriptions:        设置Flash等待周期，需要根据手册的参数表配置
** input parameters:    u8Latency:  设置的等待周期，可以取下列值：
**                      FLASH_Latency_0: FLASH Zero Latency cycle
**                      FLASH_Latency_1: FLASH One Latency cycle
**                      FLASH_Latency_2: FLASH Two Latency cycles
**                      FLASH_Latency_3: FLASH Three Latency cycles
**                      FLASH_Latency_4: FLASH Four Latency cycles
**                      FLASH_Latency_5: FLASH Five Latency cycles
**                      FLASH_Latency_6: FLASH Six Latency cycles
**                      FLASH_Latency_7: FLASH Seven Latency cycles
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void FlashSetLatency(rt_uint8_t u8Latency)
{
  /* Perform Byte access to FLASH_ACR[3:0] to set the Latency value */
  HWREG32(FLASH_BASE + FLASH_ACR1) = u8Latency;
  HWREG32(FLASH_BASE + FLASH_ACR2) = u8Latency;
}

/*********************************************************************************************************
** Function name:       FlashUnlock
** Descriptions:        对flash模块解锁，解锁后可以擦除及编程Flash
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void FlashUnlock(void)
{
  if((HWREG32(FLASH_BASE + FLASH_CR1) & 0x00000001U) && (HWREG32(FLASH_BASE + FLASH_CR2) & 0x00000001U)) {
    /* Authorize the FLASH A Registers access */
    HWREG32(FLASH_BASE + FLASH_KEYR1) = FLASH_KEY1;
    HWREG32(FLASH_BASE + FLASH_KEYR1) = FLASH_KEY2;
    
    /* Authorize the FLASH B Registers access */
    HWREG32(FLASH_BASE + FLASH_KEYR2) = FLASH_KEY1;
    HWREG32(FLASH_BASE + FLASH_KEYR2) = FLASH_KEY2;
  }
}

/*********************************************************************************************************
** Function name:       FlashLock
** Descriptions:        对flash模块加锁，加锁后flash不允许擦除及编程
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void FlashLock(void)
{
  HWREG32(FLASH_BASE + FLASH_CR1) |= 0x00000001U;
  HWREG32(FLASH_BASE + FLASH_CR2) |= 0x00000001U;
}

/*********************************************************************************************************
** 注意：下面的Flash操作函数按照字(4字节)操作进行，外部供电必须满足2.7V—3.6V
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       FlashEraseSector
** Descriptions:        擦除指定的扇区
** input parameters:    u32Bank:   操作的Flash Bank,可以取下列值：
**                      FLASH_BANK_1            // BANK1
**                      FLASH_BANK_2            // BANK2
**                      u32Sector: 待擦除的扇区号，可以去下列值：
**                      FLASH_Sector_0          // Sector Number 0
**                      FLASH_Sector_1          // Sector Number 1
**                      FLASH_Sector_2          // Sector Number 2
**                      FLASH_Sector_3          // Sector Number 3
**                      FLASH_Sector_4          // Sector Number 4
**                      FLASH_Sector_5          // Sector Number 5
**                      FLASH_Sector_6          // Sector Number 6
**                      FLASH_Sector_7          // Sector Number 7
** output parameters:   NONE
** Returned value:      RT_TRUE: 编程成功； RT_FALSE：编程出现错误
** 注意：这里Flash操作函数按照字(4字节)操作进行，外部供电必须满足2.7V—3.6V
*********************************************************************************************************/
rt_bool_t FlashEraseSector(rt_uint32_t u32Bank, rt_uint32_t u32Sector)
{
  int status;
  
  // 执行擦除命令
  if(u32Bank & FLASH_BANK_1) {
    HWREG32(FLASH_BASE + FLASH_CR1) &= ~((rt_uint32_t)((3U << 4) | (7U << 8)));
    HWREG32(FLASH_BASE + FLASH_CR1) |= ((1U << 2) | (3U << 4) | (u32Sector << 8));
    HWREG32(FLASH_BASE + FLASH_CR1) |= (1U << 7);
  }
  
  if(u32Bank & FLASH_BANK_2) {
    HWREG32(FLASH_BASE + FLASH_CR2) &= ~((rt_uint32_t)((3U << 4) | (7U << 8)));
    HWREG32(FLASH_BASE + FLASH_CR2) |= ((1U << 2) | (3U << 4) | (u32Sector << 8));
    HWREG32(FLASH_BASE + FLASH_CR2) |= (1U << 7);
  }
  
  // 等待Flash不忙
  status = FlashWaitForLastOperation(u32Bank);
  // 清空Flash操作状态
  if(u32Bank & FLASH_BANK_1) {
    HWREG32(FLASH_BASE + FLASH_CR1) &= ~((rt_uint32_t)((1U << 2) | (7U << 8)));
  }
  // 清空Flash操作状态
  if(u32Bank & FLASH_BANK_2) {
    HWREG32(FLASH_BASE + FLASH_CR2) &= ~((rt_uint32_t)((1U << 2) | (7U << 8)));
  }
  
  if(status < 0) {
    return RT_FALSE;
  }
  
  return RT_TRUE;
}

/*********************************************************************************************************
** Function name:       FlashProgram256Bit
** Descriptions:        编程Flash的一个256bit，即32个字节
** input parameters:    Address: specifies the address to be programmed.
**                      This parameter can be any address in Program memory zone or in OTP zone.
**                      DataAddress: specifies the data's address to be programmed.
** output parameters:   NONE
** Returned value:      RT_TRUE: 编程成功； RT_FALSE：编程出现错误
** 注意：这里Flash操作函数按照字(32字节，即256bit)操作进行，外部供电必须满足2.7V—3.6V
*********************************************************************************************************/
rt_bool_t FlashProgram256Bit(rt_uint32_t Address, rt_uint32_t DataAddress)
{
  int status;
  volatile rt_uint64_t *dest_addr = (volatile rt_uint64_t *)Address;
  volatile rt_uint64_t *src_addr = (volatile rt_uint64_t *)DataAddress;
  rt_uint32_t u32Bank;
  rt_uint8_t row_index = 4;
  
  if((Address >= FLASH_BANK1_BASE) && (Address < (FLASH_BANK1_BASE + FLASH_BANK_SIZE))) {
    u32Bank = FLASH_BANK_1;
  } else {
    u32Bank = FLASH_BANK_2;
  }
  
  status = FlashWaitForLastOperation(u32Bank);
  if(status >= 0) {
    if(u32Bank == FLASH_BANK_1) {
      /* Clear bank 1 pending flags (if any) */ 
      HWREG32(FLASH_BASE + FLASH_CCR1) = 0x07EE0000U;
      /* Set PG bit */
      HWREG32(FLASH_BASE + FLASH_CR1) |= 0x00000002U;
    } else {
      /* Clear bank 1 pending flags (if any) */ 
      HWREG32(FLASH_BASE + FLASH_CCR2) = 0x07EE0000U;
      /* Set PG bit */
      HWREG32(FLASH_BASE + FLASH_CR2) |= 0x00000002U;
    }
    /* Program the 256 bits flash word */
    do
    {
      *dest_addr++ = *src_addr++;
    } while (--row_index != 0);
    
    asm("  DSB");
    
    status = FlashWaitForLastOperation(u32Bank);
    if(u32Bank == FLASH_BANK_1) {
      /* Check FLASH End of Operation flag  */
      if (HWREG32(FLASH_BASE + FLASH_SR1) & 0x00010000U)
      {
        HWREG32(FLASH_BASE + FLASH_CCR1) = 0x00010000U;
      }
      /* Set PG bit */
      HWREG32(FLASH_BASE + FLASH_CR1) &= ~0x00000002U;
    } else {
      /* Check FLASH End of Operation flag  */
      if (HWREG32(FLASH_BASE + FLASH_SR2) & 0x00010000U)
      {
        HWREG32(FLASH_BASE + FLASH_CCR2) = 0x00010000U;
      }
      /* Set PG bit */
      HWREG32(FLASH_BASE + FLASH_CR2) &= ~0x00000002U;
    }
  }
  
  if(status < 0) {
    return RT_FALSE;
  }
  
  return RT_TRUE;
}
/*********************************************************************************************************
** Function name:       FlashProgram
** Descriptions:        向Flash的指定地址编程指定字节的数据
** input parameters:    u32Addr:    指定编程地址
**                      DataAddress:编程数据缓冲区指针
**                      i32Size:    指定编程的字节数，必须是32的倍数
** output parameters:   NONE
** Returned value:      RT_TRUE：写成功； RT_FALSE: 出现了错误，写失败
*********************************************************************************************************/
rt_bool_t FlashProgram(rt_uint32_t u32Addr, rt_uint32_t DataAddress, int i32Size)
{
  rt_bool_t status;
  
  if ((i32Size < 32) || (u32Addr < 0x08000000) || ((i32Size % 32) != 0)) {
    return RT_FALSE;
  }
  
  do {
    status = FlashProgram256Bit(u32Addr,DataAddress);
    u32Addr += 32;
    DataAddress += 32;
    i32Size -= 32;
  } while((status == RT_TRUE) && (i32Size > 0));
  
  return status;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
