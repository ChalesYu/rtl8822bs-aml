/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_fmc.c
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         fsmcģ��Ĵ�����װ����ʵ������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2019-03-15
** Version:             v1.0
** Descriptions:        The original version ��ʼ�汾
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
  �ֲ���������
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       FmcNandInit
** Descriptions:        NAND Flash��ʼ����Flash�ͺ�ΪMT29F2G08ABA��ʱ�������ο��������ֲ�
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
** ע�⣺MT29F2G08ABA������BANK3
** ��Ҫ���� ALE to RE delay��  10ns
**          CLE to RE delay��  10ns
*********************************************************************************************************/
void FmcNandInit(void)
{
    rt_uint32_t u32Temp;

    // ʹ��FSMC��ʱ��
    SystemPeripheralEnable(RCC_PERIPHERAL_FMC);

    // �Ƚ�BANK3��NAND���ƼĴ����ָ�Ϊ��λ��ֵ
    HWREG32(FMC_BASE + FMC_PCR3) = 0x00000018;
    HWREG32(FMC_BASE + FMC_SR3) = 0x00000040;
    HWREG32(FMC_BASE + FMC_PMEM3) = 0xFCFCFCFC;
    HWREG32(FMC_BASE + FMC_PATT3) = 0xFCFCFCFC;

    // ����PCR3�Ĵ���
    u32Temp = ((rt_uint32_t)0x00000002 |     // ʹ��Waitfeature
              (rt_uint32_t)0x00000008 |      // ��ģ�����NAND Flash
              (rt_uint32_t)0x00000000 |      // 8λ��NAND Flash
              (rt_uint32_t)0x00000000 |      // �ȹر�ECC
              ((rt_uint32_t)1 << 9) |        // ����CLE to RE delay
              ((rt_uint32_t)1 << 13) |       // ����ALE to RE delay
              ((rt_uint32_t)3 << 17));       // ����ECC page size, 2048�ֽ�
    HWREG32(FMC_BASE + FMC_PCR3) = u32Temp;

    // ����FSMC_PMEM3�Ĵ���
    u32Temp = (((rt_uint32_t)0 << 0) |       // ����Common memory x setup time
              ((rt_uint32_t)1 << 8) |        // ����Common memory x wait time
              ((rt_uint32_t)0 << 16) |       // ����Common memory x hold time
              ((rt_uint32_t)2 << 24));       // ����Common memory x databus HiZ time
    HWREG32(FMC_BASE + FMC_PMEM3) = u32Temp;

    // ����FSMC_PATT3�Ĵ���
    u32Temp = (((rt_uint32_t)0 << 0) |       // ����Attribute memory x setup time
              ((rt_uint32_t)1 << 8) |        // ����Attribute memory x wait time
              ((rt_uint32_t)0 << 16) |       // ����Attribute memory x hold time
              ((rt_uint32_t)2 << 24));       // ����Attribute memory x databus HiZ time
    HWREG32(FMC_BASE + FMC_PATT3) = u32Temp;
}

/*********************************************************************************************************
** Function name:       FmcNandEnable
** Descriptions:        NAND Flashģ��ʹ��
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
** ע�⣺MT29F2G08ABA������BANK3
*********************************************************************************************************/
void FmcNandEnable(void)
{
    HWREG32(FMC_BASE + FMC_PCR3) |= (rt_uint32_t)0x00000004;
}

/*********************************************************************************************************
** Function name:       FmcNandEccEnable
** Descriptions:        NAND FlashӲ��ECCʹ��
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
** ע�⣺MT29F2G08ABA������BANK3
*********************************************************************************************************/
void FmcNandEccEnable(void)
{
    HWREG32(FMC_BASE + FMC_PCR3) |= (rt_uint32_t)0x00000040;
}

/*********************************************************************************************************
** Function name:       FmcNandEccRead
** Descriptions:        ��ȡ��ǰ��ECC�Ĵ�����ֵ
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      ��ǰ��ECCֵ
** ע�⣺MT29F2G08ABA������BANK3
*********************************************************************************************************/
rt_uint32_t FmcNandEccRead(void)
{
    return (HWREG32(FMC_BASE + FMC_ECCR3));
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
