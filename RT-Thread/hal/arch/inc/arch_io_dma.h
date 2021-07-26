/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_dma.h
** Last modified Date:  2013-04-23
** Last Version:        v1.0
** Description:         DMAģ��Ĵ�����װ����ʵ������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2013-04-23
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
#ifndef __ARCH_IO_DMA_H__
#define __ARCH_IO_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** ��Ҫ�ĺ궨��
*********************************************************************************************************/
/*********************************************************************************************************
** DMA������ز�����Ϊ����DMAModeSet�Ĳ���u32Config
*********************************************************************************************************/
// 1�����ݴ��䷽��
#define DMA_DIR_PeripheralToMemory        ((rt_uint32_t)0x00000000)
#define DMA_DIR_MemoryToPeripheral        ((rt_uint32_t)0x00000040)
#define DMA_DIR_MemoryToMemory            ((rt_uint32_t)0x00000080)
// 2�������ַ��������
#define DMA_PeripheralInc_Enable          ((rt_uint32_t)0x00000200)
#define DMA_PeripheralInc_Disable         ((rt_uint32_t)0x00000000)
// 3���ڴ��ַ��������
#define DMA_MemoryInc_Enable              ((rt_uint32_t)0x00000400)
#define DMA_MemoryInc_Disable             ((rt_uint32_t)0x00000000)
// 4���������ݳ���
#define DMA_PeripheralDataSize_Byte       ((rt_uint32_t)0x00000000)
#define DMA_PeripheralDataSize_HalfWord   ((rt_uint32_t)0x00000800)
#define DMA_PeripheralDataSize_Word       ((rt_uint32_t)0x00001000)
// 5���ڴ����ݳ���
#define DMA_MemoryDataSize_Byte           ((rt_uint32_t)0x00000000)
#define DMA_MemoryDataSize_HalfWord       ((rt_uint32_t)0x00002000)
#define DMA_MemoryDataSize_Word           ((rt_uint32_t)0x00004000)
// 6��DMA��·ģʽ����
#define DMA_Mode_Normal                   ((rt_uint32_t)0x00000000)
#define DMA_Mode_Circular                 ((rt_uint32_t)0x00000100)
// 7��DMA���ȼ�
#define DMA_Priority_Low                  ((rt_uint32_t)0x00000000)
#define DMA_Priority_Medium               ((rt_uint32_t)0x00010000)
#define DMA_Priority_High                 ((rt_uint32_t)0x00020000)
#define DMA_Priority_VeryHigh             ((rt_uint32_t)0x00030000)
// 8���ڴ��burst����
#define DMA_MemoryBurst_Single            ((rt_uint32_t)0x00000000)
#define DMA_MemoryBurst_INC4              ((rt_uint32_t)0x00800000)
#define DMA_MemoryBurst_INC8              ((rt_uint32_t)0x01000000)
#define DMA_MemoryBurst_INC16             ((rt_uint32_t)0x01800000)
// 9�������burst����
#define DMA_PeripheralBurst_Single        ((rt_uint32_t)0x00000000)
#define DMA_PeripheralBurst_INC4          ((rt_uint32_t)0x00200000)
#define DMA_PeripheralBurst_INC8          ((rt_uint32_t)0x00400000)
#define DMA_PeripheralBurst_INC16         ((rt_uint32_t)0x00600000)
// 10��Double ģʽʹ������
#define DMA_Double_Buffer_Mode_Enable     ((rt_uint32_t)0x00040000) 
#define DMA_Double_Buffer_Mode_Disable    ((rt_uint32_t)0x00000000) 

/*********************************************************************************************************
** DMA������ز�����Ϊ����DMAFIFOConfigSet�Ĳ���u32Config
*********************************************************************************************************/
// 1��DMA��FIFO����
#define DMA_FIFOMode_Disable              ((rt_uint32_t)0x00000000)
#define DMA_FIFOMode_Enable               ((rt_uint32_t)0x00000004)
// 2��DMA��FIFO�������
#define DMA_FIFOThreshold_1QuarterFull    ((rt_uint32_t)0x00000000)
#define DMA_FIFOThreshold_HalfFull        ((rt_uint32_t)0x00000001)
#define DMA_FIFOThreshold_3QuartersFull   ((rt_uint32_t)0x00000002)
#define DMA_FIFOThreshold_Full            ((rt_uint32_t)0x00000003)
// 3��DMA_fifo_status_level
#define DMA_FIFOStatus_Less1QuarterFull   ((rt_uint32_t)0x00000000 << 3)
#define DMA_FIFOStatus_1QuarterFull       ((rt_uint32_t)0x00000001 << 3)
#define DMA_FIFOStatus_HalfFull           ((rt_uint32_t)0x00000002 << 3)
#define DMA_FIFOStatus_3QuartersFull      ((rt_uint32_t)0x00000003 << 3)
#define DMA_FIFOStatus_Empty              ((rt_uint32_t)0x00000004 << 3)
#define DMA_FIFOStatus_Full               ((rt_uint32_t)0x00000005 << 3)

/*********************************************************************************************************
** DMA������ز�����DMA�����ַ����ģʽ�������ò���
*********************************************************************************************************/
#define DMA_PINCOS_Psize                  ((rt_uint32_t)0x00000000)
#define DMA_PINCOS_WordAligned            ((rt_uint32_t)0x00008000)

/*********************************************************************************************************
** DMA������ز�������������
*********************************************************************************************************/
#define DMA_FlowCtrl_DMA                  ((rt_uint32_t)0x00000000)
#define DMA_FlowCtrl_Peripheral           ((rt_uint32_t)0x00000020)

/*********************************************************************************************************
** DMA�ж�Դ״̬����
*********************************************************************************************************/
#define DMA_INT_CONFIG_TC                 ((rt_uint32_t)0x00000010)      //  ��������ж�
#define DMA_INT_CONFIG_HT                 ((rt_uint32_t)0x00000008)      //  ����һ��ʱ���ж�
#define DMA_INT_CONFIG_TE                 ((rt_uint32_t)0x00000004)      //  ��������ж�
#define DMA_INT_CONFIG_DME                ((rt_uint32_t)0x00000002)      //  ֱ��ģʽ�����ж�
#define DMA_INT_CONFIG_ALL                ((rt_uint32_t)0x0000001e)      //  �����ж�
#define DMA_INT_CONFIG_USUAL              ((rt_uint32_t)0x00000016)      //  ����һ��������ж�

/*********************************************************************************************************
** DMA�ж�Դ״̬����
*********************************************************************************************************/
#define DMA_INT_STATUS_TC                 ((rt_uint32_t)0x00000020)      //  ��������ж�
#define DMA_INT_STATUS_HT                 ((rt_uint32_t)0x00000010)      //  ����һ��ʱ���ж�
#define DMA_INT_STATUS_TE                 ((rt_uint32_t)0x00000008)      //  ��������ж�
#define DMA_INT_STATUS_DME                ((rt_uint32_t)0x00000004)      //  ֱ��ģʽ�����ж�
#define DMA_INT_STATUS_FIFO               ((rt_uint32_t)0x00000001)      //  FIFO�����ж�
#define DMA_INT_STATUS_ALL                ((rt_uint32_t)0x0000003d)      //  �����ж�

/*********************************************************************************************************
** �ⲿ����������
*********************************************************************************************************/
extern void DMAModeConfigSet(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32Config);
extern void DMAFIFOConfigSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32Config);
extern void DMAAddrSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32MemAddr, rt_uint32_t u32PeriAddr);
extern void DMAMemoryAddrSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, 
                      rt_uint8_t u8BufferNumber, rt_uint32_t u32MemAddr);
extern rt_uint32_t DMAMemoryAddrGet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint8_t u8BufferNumber);
extern void DMACurrentMemoryTargetSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint8_t u8BufferNumber);
extern uint8_t DMACurrentMemoryTargetGet(rt_uint32_t u32Base, rt_uint8_t u8Stream);
extern void DMABufferSizeSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint16_t u16Size);
extern void DMAEnable(rt_uint32_t u32Base, rt_uint8_t u8Stream);
extern void DMADisable(rt_uint32_t u32Base, rt_uint8_t u8Stream);
extern void DMAPeriphIncOffsetSizeConfig(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32Config);
extern void DMAFlowControllerConfig(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32Config);
extern rt_uint16_t DMACurrDataCounterGet(rt_uint32_t u32Base, rt_uint8_t u8Stream);
extern void DMACurrDataCounterSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint16_t u16Count);
extern rt_bool_t DMAIsEnabled(rt_uint32_t u32Base, rt_uint8_t u8Stream);
extern void DMAIntEnable(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32Flags);
extern void DMAIntDisable(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32Flags);
extern void DMAFIFOIntEnable(rt_uint32_t u32Base, rt_uint8_t u8Stream);
extern void DMAFIFOIntDisable(rt_uint32_t u32Base, rt_uint8_t u8Stream);
extern rt_uint32_t DMAIntStatus(rt_uint32_t u32Base, rt_uint8_t u8Stream);
extern void DMAIntClear(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32IntFlags);

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_DMA_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
