/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_fmc.c
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         fsmc模块寄存器封装函数实现声明
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
#include "hal/arch/inc/arch_io_fmc.h"


/*********************************************************************************************************
  局部变量定义
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       FmcNandInit
** Descriptions:        NAND Flash初始化，Flash型号为MT29F2G08ABA，时序参数请参考该数据手册
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
** 注意：MT29F2G08ABA连接在BANK3
** 主要参数 ALE to RE delay：  10ns
**          CLE to RE delay：  10ns
*********************************************************************************************************/
void FmcNandInit(void)
{
    rt_uint32_t u32Temp;

    // 使能FSMC的时钟
    SystemPeripheralEnable(RCC_PERIPHERAL_FMC);

    // 先将BANK3的NAND控制寄存器恢复为复位的值
    HWREG32(FMC_BASE + FMC_PCR3) = 0x00000018;
    HWREG32(FMC_BASE + FMC_SR3) = 0x00000040;
    HWREG32(FMC_BASE + FMC_PMEM3) = 0xFCFCFCFC;
    HWREG32(FMC_BASE + FMC_PATT3) = 0xFCFCFCFC;

    // 配置PCR3寄存器
    u32Temp = ((rt_uint32_t)0x00000002 |     // 使能Waitfeature
              (rt_uint32_t)0x00000008 |      // 该模块外接NAND Flash
              (rt_uint32_t)0x00000000 |      // 8位的NAND Flash
              (rt_uint32_t)0x00000000 |      // 先关闭ECC
              ((rt_uint32_t)1 << 9) |        // 配置CLE to RE delay
              ((rt_uint32_t)1 << 13) |       // 配置ALE to RE delay
              ((rt_uint32_t)3 << 17));       // 配置ECC page size, 2048字节
    HWREG32(FMC_BASE + FMC_PCR3) = u32Temp;

    // 配置FSMC_PMEM3寄存器
    u32Temp = (((rt_uint32_t)0 << 0) |       // 配置Common memory x setup time
              ((rt_uint32_t)1 << 8) |        // 配置Common memory x wait time
              ((rt_uint32_t)0 << 16) |       // 配置Common memory x hold time
              ((rt_uint32_t)2 << 24));       // 配置Common memory x databus HiZ time
    HWREG32(FMC_BASE + FMC_PMEM3) = u32Temp;

    // 配置FSMC_PATT3寄存器
    u32Temp = (((rt_uint32_t)0 << 0) |       // 配置Attribute memory x setup time
              ((rt_uint32_t)1 << 8) |        // 配置Attribute memory x wait time
              ((rt_uint32_t)0 << 16) |       // 配置Attribute memory x hold time
              ((rt_uint32_t)2 << 24));       // 配置Attribute memory x databus HiZ time
    HWREG32(FMC_BASE + FMC_PATT3) = u32Temp;
}

/*********************************************************************************************************
** Function name:       FmcNandEnable
** Descriptions:        NAND Flash模块使能
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
** 注意：MT29F2G08ABA连接在BANK3
*********************************************************************************************************/
void FmcNandEnable(void)
{
    HWREG32(FMC_BASE + FMC_PCR3) |= (rt_uint32_t)0x00000004;
}

/*********************************************************************************************************
** Function name:       FmcNandEccEnable
** Descriptions:        NAND Flash硬件ECC使能
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
** 注意：MT29F2G08ABA连接在BANK3
*********************************************************************************************************/
void FmcNandEccEnable(void)
{
    HWREG32(FMC_BASE + FMC_PCR3) |= (rt_uint32_t)0x00000040;
}

/*********************************************************************************************************
** Function name:       FmcNandEccRead
** Descriptions:        读取当前的ECC寄存器的值
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      当前的ECC值
** 注意：MT29F2G08ABA连接在BANK3
*********************************************************************************************************/
rt_uint32_t FmcNandEccRead(void)
{
    return (HWREG32(FMC_BASE + FMC_ECCR3));
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
