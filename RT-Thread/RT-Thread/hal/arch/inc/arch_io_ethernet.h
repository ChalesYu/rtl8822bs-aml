/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_ethernet.h
** Last modified Date:  2013-04-04
** Last Version:        v1.0
** Description:         ethernetģ��Ĵ�����װ����ʵ������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2013-04-04
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
#ifndef __ARCH_IO_ETHERNET_H__
#define __ARCH_IO_ETHERNET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** DMA�ж�Դ����
*********************************************************************************************************/
#define DMA_INTRUPT_NSE         0x00008000
#define DMA_INTRUPT_ASE         0x00004000
#define DMA_INTRUPT_CDEE        0x00002000
#define DMA_INTRUPT_FBEE        0x00001000
#define DMA_INTRUPT_ERIE        0x00000800
#define DMA_INTRUPT_ETIE        0x00000400
#define DMA_INTRUPT_RWTE        0x00000200
#define DMA_INTRUPT_RSE         0x00000100
#define DMA_INTRUPT_RBUE        0x00000080
#define DMA_INTRUPT_RIE         0x00000040
#define DMA_INTRUPT_TBUE        0x00000004
#define DMA_INTRUPT_TXSE        0x00000002
#define DMA_INTRUPT_TIE         0x00000001

/*********************************************************************************************************
** ��Ҫ�����ݽṹ����
*********************************************************************************************************/
#if defined ( __GNUC__ )
typedef struct __attribute__((packed))
#else 
typedef __packed struct
#endif
{
  rt_uint32_t DESC0;
  rt_uint32_t DESC1;
  rt_uint32_t DESC2;
  rt_uint32_t DESC3;
  rt_uint32_t BackupAddr0;      /* used to store rx buffer 1 address */
  rt_uint32_t BackupAddr1;      /* used to store rx buffer 2 address */
} tEthDMADesciptor;

// DMA�������ݻ������ṹ
typedef struct __tEthDMABuffer
{
  rt_uint8_t *buffer;                   /* buffer address */
  rt_uint32_t len;                      /* buffer length */
  struct __tEthDMABuffer *next;         /* Pointer to the next buffer in the list */	
} tEthDMABuffer;

/*********************************************************************************************************
** �ⲿ����������
*********************************************************************************************************/
extern void EthernetInit(rt_uint32_t u32Base, rt_uint32_t u32Clock, rt_uint32_t u32RecvLength);
extern void EthernetPHYIntISR(rt_uint32_t u32Base);
extern rt_uint32_t EthernetPHYRead(rt_uint32_t u32Base, rt_uint8_t u8PHYDevAddr, rt_uint8_t u8PHYRegAddr);
extern void EthernetPHYWrite(rt_uint32_t u32Base, rt_uint8_t u8PHYDevAddr, rt_uint8_t u8PHYRegAddr, rt_uint32_t u32Data);
extern void EthernetEnable(rt_uint32_t u32Base);
extern void EthernetDisable(rt_uint32_t u32Base);
extern void EthernetMACAddrSet(rt_uint32_t u32Base, rt_uint8_t *pu8MacAddr);
extern void EthernetMACAddrGet(rt_uint32_t u32Base, rt_uint8_t *pu8MacAddr);
extern void EthernetMACAddrExtSet(rt_uint32_t u32Base, rt_uint8_t u8Number, rt_uint8_t *pu8MacAddr);
extern void EthernetReset(rt_uint32_t u32Base);
extern rt_bool_t EthernetResetStatusGet(rt_uint32_t u32Base);
extern void EthernetDMAIntEnable(rt_uint32_t u32Base, rt_uint32_t u32Config);
extern void EthernetDMAIntDisable(rt_uint32_t u32Base, rt_uint32_t u32Config);


#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_ETHERNET_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
