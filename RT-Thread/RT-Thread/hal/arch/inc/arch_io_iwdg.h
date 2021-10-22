/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_iwdg.h
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
#ifndef __ARCH_IO_IDWG_H__
#define __ARCH_IO_IDWG_H__

#include <rthw.h>

/*********************************************************************************************************
** ��Ҫ�ĺ궨��
*********************************************************************************************************/
//  ʱ�ӷ�Ƶ����
#define IWDG_DIV_4              0x00
#define IWDG_DIV_8              0x01
#define IWDG_DIV_16             0x02
#define IWDG_DIV_32             0x03
#define IWDG_DIV_64             0x04
#define IWDG_DIV_128            0x05
#define IWDG_DIV_256            0x06

/*********************************************************************************************************
** �ⲿ����������
*********************************************************************************************************/
extern void iwdgAccessEnable(rt_uint32_t u32Base);
extern void iwdgAccessDisable(rt_uint32_t u32Base);
extern void iwdgPrescalerSet(rt_uint32_t u32Base, rt_uint8_t u8Div);
extern void iwdgReloadSet(rt_uint32_t u32Base,rt_uint16_t u16Count);
extern void iwdgReloadEnable(rt_uint32_t u32Base);
extern void idwgEnable(rt_uint32_t u32Base);
extern rt_uint8_t idwgStatusGet(rt_uint32_t u32Base);

#endif // endif of __IDWG_H__
/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/
