/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_usart.h
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         usart��uartģ��Ĵ�����װ����ʵ������
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
#ifndef __ARCH_IO_USART_H__
#define __ARCH_IO_USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** ��Ҫ�ĺ궨��
*********************************************************************************************************/
//  �������ú���UsartConfigSet�Ĳ�������,�ú�����12λ������ж�д��Ĵ���
#define USART_STOP_0_5      0x00000001
#define USART_STOP_1        0x00000000
#define UASRT_STOP_1_5      0x00000003
#define USART_STOP_2        0x00000002

#define USART_WLEN_7        0x10000000
#define USART_WLEN_8        0x00000000
#define USART_WLEN_9        0x00001000

#define USART_PAR_NONE      0x00000000
#define USART_PAR_EVEN      0x00000400
#define USART_PAR_ODD       0x00000600

#define USART_CTSE_ENABLE   0x00000020
#define USART_CTSE_DISABLE  0x00000000

#define USART_RTSE_ENABLE   0x00000010
#define USART_RTSE_DISABLE  0x00000000

#define USART_DMA_TX        0x00000080          //  DMA���͹���
#define USART_DMA_RX        0x00000040          //  DMA���ܹ���
//  �����ж�Դ��Ϊ����UsartIntEnable()�Ĳ���
#define USART_INT_EIE       0x00000200          //  ֡�����ж�
#define USART_INT_PE        0x00000100          //  У�����
#define USART_INT_TX_EMPTY  0x00000080          //  ���ͻ��������ж�
#define USART_INT_TX_FINISH 0x00000040          //  ��������ж�
#define USART_INT_RX        0x00000020          //  �����ж�ʹ�ܣ�˵�����ռĴ�������������
#define USART_INT_IDLE      0x00000010          //  IDLE�ж�ʹ��

/*********************************************************************************************************
** �ⲿ����������
*********************************************************************************************************/
extern void UsartConfigSet(rt_uint32_t u32Base, rt_uint32_t u32Clock,
                            rt_uint32_t u32Baud, rt_uint32_t u32Config);
extern void UsartEnable(rt_uint32_t u32Base);
extern void UsartDisable(rt_uint32_t u32Base);
extern void UsartDMAEnable(rt_uint32_t u32Base,rt_uint32_t u32Flags);
extern void UsartDMADisable(rt_uint32_t u32Base, rt_uint32_t u32Flags);
extern void UsartIntEnable(rt_uint32_t u32Base,rt_uint32_t u32IntFlags);
extern void UsartIntDisable(rt_uint32_t u32Base,rt_uint32_t u32IntFlags);
extern rt_bool_t UsartSpaceAvial(rt_uint32_t u32Base);
extern void UsartCharPut(rt_uint32_t u32Base,rt_uint8_t u8Data);
extern rt_bool_t UsartCharsAvial(rt_uint32_t u32Base);
extern rt_uint32_t  UsartCharGet(rt_uint32_t u32Base);
extern void UsartPutS(rt_uint32_t u32Base,const char *s);
extern void UsartPutN(rt_uint32_t u32Base, const  char  *pStr , rt_uint16_t  u16Num);



#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_USART_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
