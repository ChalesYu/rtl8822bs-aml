/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_dmamux.c
** Last modified Date:  2019-03-24
** Last Version:        v1.0
** Description:         DMA MUXģ�麯��������ʵ��DMAͨ��������ѡ��
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2019-03-24
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
#include "hal/arch/inc/arch_io_dmamux.h"

/*********************************************************************************************************
** Function name:       DMAMuxRequestIDSet
** Descriptions:        DMA����ID���ã���������DMA������Դ������DMA1 Stream0ΪUSART1��TX����
** input parameters:    ulBase:         DMAģ��Ļ���ַ
**                      u32Channel:     DMAͨ����ţ�DMAMUX1Ϊ0-15��DMAMUX2Ϊ0-7
**                      u32RequestID:   DMA������ID��������ο��ֲ�
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAMuxRequestIDSet(rt_uint32_t u32Base, rt_uint32_t u32Channel,rt_uint32_t u32RequestID)
{ 
  HWREG32(u32Base + DMAMUX_C0CR + 4 * u32Channel) = u32RequestID;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
