/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_bdma.h
** Last modified Date:  2016-01-19
** Last Version:        v1.0
** Description:         BDMAģ��Ĵ�����װ����ʵ��������������STM32L4xx
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2016-01-19
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
#ifndef __ARCH_IO_BDMA_H__
#define __ARCH_IO_BDMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** ��Ҫ�ĺ궨��
*********************************************************************************************************/
//  DMA������ز�����Ϊ����DMAModeSet�Ĳ���ulConfig
#define BDMA_DIR_M_READ      0x00000010      //  �Ӵ洢��������֮Ϊ�����

#define BDMA_CIRC_EN         0x00000020      //  ִ��ѭ������ģʽ
#define BDMA_PINC_EN         0x00000040      //  ʹ�������ַ����ģʽ
#define BDMA_MINC_EN         0x00000080      //  ʹ�ܴ洢����ַ����ģʽ

#define BDMA_PSIZE_8         0x00000000      //  �������ݿ��Ϊ8λ
#define BDMA_PSIZE_16        0x00000100      //  �������ݿ��Ϊ16λ
#define BDMA_PSIZE_32        0x00000200      //  �������ݿ��Ϊ32λ
#define BDMA_PSIZE_RESERVED  0x00000300      //  �������ݿ�ȱ���

#define BDMA_MSIZE_8         0x00000000      //  �洢�����ݿ��Ϊ8λ
#define BDMA_MSIZE_16        0x00000400      //  �洢�����ݿ��Ϊ16λ
#define BDMA_MSIZE_32        0x00000800      //  �洢�����ݿ��Ϊ32λ
#define BDMA_MSIZE_RESERVED  0x00000C00      //  �洢�����ݿ�ȱ���
  
#define BDMA_MODE_NORMAL         ((rt_uint32_t)0x00000000)   //  ��������ģʽ
#define BDMA_MODE_CIRCULAR       ((rt_uint32_t)0x00000020)   //  ѭ������ģʽ

#define BDMA_PL_LOW          0x00000000      //  ͨ�����ȼ�Ϊ��
#define BDMA_PL_MEDIUM       0x00001000      //  ͨ�����ȼ�Ϊ��
#define BDMA_PL_HIGH         0x00002000      //  ͨ�����ȼ�Ϊ��
#define BDMA_PL_HITHEST      0x00003000      //  ͨ�����ȼ�Ϊ���

#define DMA_MTOM_EN         0x00004000      //  �洢�����洢������ģʽʹ��
//  ��ص��ж��źŶ��壬Ϊ�ж���غ����Ĳ����򷵻�ֵ
#define BDMA_INT_TC          0x00000002      //  ��������ж�
#define BDMA_INT_HT          0x00000004      //  ����һ��ʱ���ж�
#define BDMA_INT_TE          0x00000008      //  ��������ж�
#define BDMA_INT_GLOBAL      0x00000001      //  ȫ���жϣ�ֻҪ����������һ����������ͳ���
#define BDMA_INT_UAUAL       0x0000000A      //  ����һ��������ж� 

/*********************************************************************************************************
** �ⲿ����������
*********************************************************************************************************/
extern void BDMAModeSet(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulConfig);
extern void BDMAAddrSet(rt_uint32_t ulBase, rt_uint8_t ucChannel, 
                rt_uint32_t ulMemAddr, rt_uint32_t ulPeriAddr);
extern void BDMAMemAddr1Set(rt_uint32_t ulBase, rt_uint8_t ucChannel, rt_uint32_t ulMemAddr);
extern void BDMABufferSizeSet(rt_uint32_t ulBase, rt_uint8_t ucChannel, rt_uint16_t usSize);
extern rt_uint32_t BDMABufferSizeGet(rt_uint32_t ulBase, rt_uint8_t ucChannel);
extern void BDMAEnable(rt_uint32_t ulBase, rt_uint8_t ucChannel);
extern void BDMADisable(rt_uint32_t ulBase, rt_uint8_t ucChannel);
extern rt_bool_t BDMAIsEnabled(rt_uint32_t u32Base, rt_uint8_t ucChannel);
extern void BDMAIntEnable(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulFlags);
extern void BDMAIntDisable(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulFlags);
extern rt_uint32_t BDMAIntStatus(rt_uint32_t ulBase, rt_uint8_t ucChannel);
extern void BDMAIntClear(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulIntFlags);

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_BDMA_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
