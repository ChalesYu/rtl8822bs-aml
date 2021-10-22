/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_iwdg.c
** Last modified Date:  2013-04-02
** Last Version:        v1.0
** Description:         gpioģ��Ĵ�����װ����ʵ������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2013-04-02
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
#include "hal/arch/inc/arch_io_iwdg.h"

/*********************************************************************************************************
** Function name:       iwdgAccessEnable
** Descriptions:        ʹ�ܶ������Ź����ʣ��ú����������ȵ��ò��ܷ��ʱ�ļĴ���
** input parameters:    u32Base:     �������Ź�ģ��Ļ���ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void iwdgAccessEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + IWDG_KR) = 0x5555;
}

/*********************************************************************************************************
** Function name:       iwdgAccessEnable
** Descriptions:        ���ܶ������Ź�����
** input parameters:    u32Base:     �������Ź�ģ��Ļ���ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void iwdgAccessDisable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + IWDG_KR) = 0x0000;
}

/*********************************************************************************************************
** Function name:       iwdgPrescalerSet
** Descriptions:        ����Ԥ��Ƶֵ
** input parameters:    u32Base:     �������Ź�ģ��Ļ���ַ
**                      u8Div:      Ԥ��Ƶֵ������ȥ����ֵ�е�һ����
**                      IWDG_DIV_4              // 4��Ƶ
**                      IWDG_DIV_8              // 8��Ƶ
**                      IWDG_DIV_16             // 16��Ƶ
**                      IWDG_DIV_32             // 32��Ƶ
**                      IWDG_DIV_64             // 64��Ƶ
**                      IWDG_DIV_128            // 128��Ƶ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void iwdgPrescalerSet(rt_uint32_t u32Base, rt_uint8_t u8Div)
{
    HWREG32(u32Base + IWDG_PR) = u8Div;
}

/*********************************************************************************************************
** Function name:       iwdgReloadSet
** Descriptions:        ���ö������Ź�����װ��ֵ
** input parameters:    u32Base:     �������Ź�ģ��Ļ���ַ
**                      u16Count:    ����װ�ص�ֵ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void iwdgReloadSet(rt_uint32_t u32Base,rt_uint16_t u16Count)
{
    HWREG16(u32Base + IWDG_RLR) = u16Count;
}

/*********************************************************************************************************
** Function name:       iwdgReloadEnable
** Descriptions:        ʹ������װ�ض������Ź��ļ���ֵ
** input parameters:    u32Base:     �������Ź�ģ��Ļ���ַ
**                      ulCount:    ����װ�ص�ֵ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void iwdgReloadEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + IWDG_KR) = 0xAAAA;
}

/*********************************************************************************************************
** Function name:       idwgEnable
** Descriptions:        ʹ�ܶ������Ź������Ź���ʼ����
** input parameters:    u32Base:     �������Ź�ģ��Ļ���ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void idwgEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + IWDG_KR) = 0xCCCC;
}

/*********************************************************************************************************
** Function name:       idwgStatusGet
** Descriptions:        ��ȡ�������Ź���״̬���Ƿ�ʹ��
** input parameters:    u32Base:     �������Ź�ģ��Ļ���ַ
** output parameters:   NONE
** Returned value:      ���ض������Ź���״̬�������0λΪ1����ʾ���Ź�Ԥ��Ƶֵ���£���֮��δ���£�
**                      �����1δΪ1����ʾ��װ��ֵ���£���֮��δ���¡�
*********************************************************************************************************/
rt_uint8_t idwgStatusGet(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + IWDG_SR) & 0x03);
}
/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/

