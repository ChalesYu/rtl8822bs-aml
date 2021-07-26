/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_dma.c
** Last modified Date:  2013-04-23
** Last Version:        v1.0
** Description:         dmaģ��Ĵ�����װ����ʵ������
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
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_dma.h"


/*********************************************************************************************************
  �ֲ���������
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       DMAModeConfigSet
** Descriptions:        DMAģ�鹤��ģʽ����
** input parameters:    u32Base:    DMAģ��Ļ���ַ
**                      u8Stream:   DMA��Stream��ţ�Ϊ0-7
**                      u32Config:  ����ֵ������ȡ���м�������Ļ�
**                      // 1�����ݴ��䷽��
**                      DMA_DIR_PeripheralToMemory        ���赽�ڴ�
**                      DMA_DIR_MemoryToPeripheral        �ڴ浽����
**                      DMA_DIR_MemoryToMemory            �ڴ浽�ڴ�
**                      // 2�������ַ��������
**                      DMA_PeripheralInc_Enable          �����ַ����ʹ��
**                      DMA_PeripheralInc_Disable         �����ַ��������
**                      // 3���ڴ��ַ��������
**                      DMA_MemoryInc_Enable              �ڴ��ַ����ʹ��
**                      DMA_MemoryInc_Disable             �ڴ��ַ��������
**                      // 4���������ݳ���
**                      DMA_PeripheralDataSize_Byte       1���ֽڳ�
**                      DMA_PeripheralDataSize_HalfWord   2���ֽڳ�
**                      DMA_PeripheralDataSize_Word       4���ֽڳ�
**                      // 5���ڴ����ݳ���
**                      DMA_MemoryDataSize_Byte           1���ֽڳ�
**                      DMA_MemoryDataSize_HalfWord       2���ֽڳ�
**                      DMA_MemoryDataSize_Word           4���ֽڳ�
**                      // 6��DMA��·ģʽ����
**                      DMA_Mode_Normal                   ����ģʽ
**                      DMA_Mode_Circu32ar                 ��·ģʽ
**                      // 7��DMA���ȼ�
**                      DMA_Priority_Low                  �����ȼ�
**                      DMA_Priority_Medium               �����ȼ�
**                      DMA_Priority_High                 �����ȼ�
**                      DMA_Priority_VeryHigh             �������ȼ�
**                      // 8���ڴ��burst����
**                      DMA_MemoryBurst_Single            ���δ���
**                      DMA_MemoryBurst_INC4              4��
**                      DMA_MemoryBurst_INC8              8��
**                      DMA_MemoryBurst_INC16             16��
**                      // 9�������burst����
**                      DMA_PeripheralBurst_Single        ���δ���
**                      DMA_PeripheralBurst_INC4          4��
**                      DMA_PeripheralBurst_INC8          8��
**                      DMA_PeripheralBurst_INC16         16��
**                      DMA_Double_Buffer_Mode_Enable     ʹ��Double Buffer mode 
**                      DMA_Double_Buffer_Mode_Disable    ��ֹDouble Buffer mode 
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAModeConfigSet(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32Config)
{
    rt_uint32_t u32Temp;
    u32Temp = HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18);
    u32Temp &= ~0xFFF37FC0;
    u32Temp |= u32Config;
    HWREG32(u32Base + DMA_SxCR+ ((rt_uint32_t)u8Stream)*0x18) = u32Temp;
}

/*********************************************************************************************************
** Function name:       DMAFIFOConfigSet
** Descriptions:        DMA��FIFO����
** input parameters:    u32Base:    DMAģ��Ļ���ַ
**                      u8Stream:   DMA��Stream��ţ�Ϊ0-7
**                      u32Config:  ����ֵ������ȡ���м�������Ļ�
**                      // 1��DMA��FIFO����
**                      DMA_FIFOMode_Disable              �ر�FIFO
**                      DMA_FIFOMode_Enable               ��FIFO
**                      // 2��DMA��FIFO�������
**                      DMA_FIFOThreshold_1QuarterFull    1/4  FIFO
**                      DMA_FIFOThreshold_HalfFull        2/4  FIFO
**                      DMA_FIFOThreshold_3QuartersFull   3/4  FIFO
**                      DMA_FIFOThreshold_Full            1    FIFO
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAFIFOConfigSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32Config)
{
    rt_uint32_t u32Temp;
    u32Temp = HWREG32(u32Base + DMA_SxFCR + ((rt_uint32_t)u8Stream)*0x18);
    u32Temp &= ~0xFFFFFFF7;
    u32Temp |= u32Config;
    HWREG32(u32Base + DMA_SxFCR+ ((rt_uint32_t)u8Stream)*0x18) = u32Temp;
}

/*********************************************************************************************************
** Function name:       DMAAddrSet
** Descriptions:        DMA����洢����ַ�������ַ����
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
**                      u32MemAddr      ָ��DMA����Ĵ洢����ַ
**                      u32PeriAddr     ָ��DMA����������ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAAddrSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32MemAddr, rt_uint32_t u32PeriAddr)
{
    HWREG32(u32Base + DMA_SxPAR + ((rt_uint32_t)u8Stream)*0x18) = u32PeriAddr;
    HWREG32(u32Base + DMA_SxMOAR + ((rt_uint32_t)u8Stream)*0x18) = u32MemAddr;
}

/*********************************************************************************************************
** Function name:       DMAMemoryAddrSet
** Descriptions:        DMA����洢����ַ���ã�Double bufferģʽ��ʹ�ã�����ѡ��ʹ�õĻ�����
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
**                      u8BufferNumber��ָ�����õĻ�����������Ϊ0����1
**                      u32MemAddr      ָ��DMA����Ĵ洢����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAMemoryAddrSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, 
                      rt_uint8_t u8BufferNumber, rt_uint32_t u32MemAddr)
{
  if(u8BufferNumber == 0) {
    HWREG32(u32Base + DMA_SxMOAR + ((rt_uint32_t)u8Stream)*0x18) = u32MemAddr;
  } else {
    HWREG32(u32Base + DMA_SxM1AR + ((rt_uint32_t)u8Stream)*0x18) = u32MemAddr;
  }
}


/*********************************************************************************************************
** Function name:       DMAMemoryAddrGet
** Descriptions:        DMA����洢����ַ��ȡ
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
**                      u8BufferNumber��ָ�����õĻ�����������Ϊ0����1
**                      u32MemAddr      ָ��DMA����Ĵ洢����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t DMAMemoryAddrGet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint8_t u8BufferNumber)
{
  if(u8BufferNumber == 0) {
    return HWREG32(u32Base + DMA_SxMOAR + ((rt_uint32_t)u8Stream)*0x18);
  } else {
    return HWREG32(u32Base + DMA_SxM1AR + ((rt_uint32_t)u8Stream)*0x18);
  }
}

/*********************************************************************************************************
** Function name:       DMACurrentMemoryTargetSet
** Descriptions:        ˫���������õ�ǰʹ�õ�DMA��������������DMA Disable�µ���
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
**                      u8BufferNumber��ָ�����õĻ�����������Ϊ0����1
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMACurrentMemoryTargetSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint8_t u8BufferNumber)
{
  if(u8BufferNumber == 0) {
     HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) &= ~(0x00080000UL);
  } else {
     HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) |= 0x00080000UL;
  }
}

/*********************************************************************************************************
** Function name:       DMACurrentMemoryTargetGet
** Descriptions:        ˫�����»�ȡ��ǰʹ�õ�DMA������
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
** output parameters:   NONE
** Returned value:      0��1
*********************************************************************************************************/
uint8_t DMACurrentMemoryTargetGet(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
  if(HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) & 0x00080000UL) {
    return 1;
  } else {
    return 0;
  }
}

/*********************************************************************************************************
** Function name:       DMABufferSizeSet
** Descriptions:        DMAģ�鴫�����ݻ�������С����
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
**                      u16Size         ָ������Ĵ�С
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMABufferSizeSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint16_t u16Size)
{
    HWREG32(u32Base + DMA_SxNDTR  + ((rt_uint32_t)u8Stream)*0x18) = u16Size;
}

/*********************************************************************************************************
** Function name:       DMAEnable
** Descriptions:        DMAģ��ʹ��
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAEnable(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    HWREG32(u32Base + DMA_SxCR+ ((rt_uint32_t)u8Stream)*0x18) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       DMADisable
** Descriptions:        DMAģ�����
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMADisable(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    HWREG32(u32Base + DMA_SxCR+ ((rt_uint32_t)u8Stream)*0x18) &= 0xFFFFFFFE;
}

/*********************************************************************************************************
** Function name:       DMAPeriphIncOffsetSizeConfig
** Descriptions:        Configures, when the PINC (Peripheral Increment address mode) bit is
**                      set, if the peripheral address shouled be incremented with the data
**                      size (configured with PSIZE bits) or by a fixed offset equal to 4
**                      (32-bit aligned addresses).
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
**                      u32Config       DMA�����ַ�����������ò���������ȥ����ֵ�е�һ����
**                      DMA_PINCOS_Psize            ������������PSIZE����
**                      DMA_PINCOS_WordAligned      �����ֶ���(4�ֽ�)
** output parameters:   NONE
** Returned value:      NONE
** This function has no effect if the Peripheral Increment mode is disabled.
*********************************************************************************************************/
void DMAPeriphIncOffsetSizeConfig(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32Config)
{
    rt_uint32_t u32Temp;

    u32Temp = HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18);
    u32Temp &= ~((rt_uint32_t)0x00008000);
    u32Temp |= u32Config;
    HWREG32(u32Base + DMA_SxCR+ ((rt_uint32_t)u8Stream)*0x18) = u32Temp;
}
/*********************************************************************************************************
** Function name:       DMAFlowControllerConfig
** Descriptions:        Configures, when the DMAy Streamx is disabled, the flow controller for
**                      the next transactions (Peripheral or Memory).
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
**                      u32Config       DMA�����ַ�����������ò���������ȥ����ֵ�е�һ����
**                      DMA_FlowCtrl_DMA            DMA����
**                      DMA_FlowCtrl_Peripheral     ��������
** output parameters:   NONE
** Returned value:      NONE
**                      Before enabling this feature, check if the used peripheral supports
**                      the Flow Controller mode or not.
*********************************************************************************************************/
void DMAFlowControllerConfig(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint32_t u32Config)
{
    rt_uint32_t u32Temp;

    u32Temp = HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18);
    u32Temp &= ~(((rt_uint32_t)0x00000020));
    u32Temp |= u32Config;
    HWREG32(u32Base + DMA_SxCR+ ((rt_uint32_t)u8Stream)*0x18) = u32Temp;
}

/*********************************************************************************************************
** Function name:       DMACurrDataCounterGet
** Descriptions:        ��ȡ��ǰ��DMA��������ֵ
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
** output parameters:   NONE
** Returned value:      ��ǰ��DMA��������ֵ
*********************************************************************************************************/
rt_uint16_t DMACurrDataCounterGet(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    return ((rt_uint16_t)(HWREG32(u32Base + DMA_SxNDTR  + ((rt_uint32_t)u8Stream)*0x18)));
}

/*********************************************************************************************************
** Function name:       DMACurrDataCounterSet
** Descriptions:        ���õ�ǰ��DMA��������ֵ
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
**                      u16Count        �����õļ�������ֵ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMACurrDataCounterSet(rt_uint32_t u32Base, rt_uint8_t u8Stream, rt_uint16_t u16Count)
{
    HWREG32(u32Base + DMA_SxNDTR  + ((rt_uint32_t)u8Stream)*0x18) = u16Count;
}

/*********************************************************************************************************
** Function name:       DMAIsEnabled
** Descriptions:        �ж�DMA�Ƿ�ʹ�ܴ��䣬the status of EN bit for the specified DMAy Streamx.
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
** output parameters:   NONE
** Returned value:      RT_TRUE:   ʹ����DMA����
**                      RT_FALSE:  ������DMA����
*********************************************************************************************************/
rt_bool_t DMAIsEnabled(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    if(HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) & 0x00000001) {
        return RT_TRUE;
    } else {
        return RT_FALSE;
    }
}

/*********************************************************************************************************
** Function name:       DMAIntEnable
** Descriptions:        DMA�ж�ʹ��
** input parameters:    u32Base             DMAģ��Ļ���ַ
**                      u8Stream            DMA��Stream��ţ�Ϊ0-7
**                      u32Flags            ָ��ʹ�ܵ��жϣ�����ȡ����ֵ�Ļ�
**                      DMA_INT_CONFIG_TC          ��������ж�
**                      DMA_INT_CONFIG_HT          ����һ��ʱ���ж�
**                      DMA_INT_CONFIG_TE          ��������ж�
**                      DMA_INT_CONFIG_DME         ֱ��ģʽ�����ж�
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAIntEnable(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32Flags)
{
    HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) |= u32Flags;
}

/*********************************************************************************************************
** Function name:       DMAIntDisable
** Descriptions:        DMA�жϽ���
** input parameters:    u32Base          DMAģ��Ļ���ַ
**                      u8Stream         DMA��Stream��ţ�Ϊ0-7
**                      u32Flags         ָ��ʹ�ܵ��жϣ�����ȡ����ֵ�Ļ�
**                      DMA_INT_CONFIG_TC          ��������ж�
**                      DMA_INT_CONFIG_HT          ����һ��ʱ���ж�
**                      DMA_INT_CONFIG_TE          ��������ж�
**                      DMA_INT_CONFIG_DME         ֱ��ģʽ�����ж�
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAIntDisable(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32Flags)
{
    HWREG32(u32Base + DMA_SxCR + ((rt_uint32_t)u8Stream)*0x18) &= ~u32Flags;
}

/*********************************************************************************************************
** Function name:       DMAFIFOIntEnable
** Descriptions:        DMA��FIFO�ж�ʹ��
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAFIFOIntEnable(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    HWREG32(u32Base + DMA_SxFCR + ((rt_uint32_t)u8Stream)*0x18) |= (rt_uint32_t)0x00000080;
}

/*********************************************************************************************************
** Function name:       DMAFIFOIntDisable
** Descriptions:        DMA��FIFO�жϽ���
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAFIFOIntDisable(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    HWREG32(u32Base + DMA_SxFCR + ((rt_uint32_t)u8Stream)*0x18) &= ~((rt_uint32_t)0x00000080);
}

/*********************************************************************************************************
** Function name:       DMAIntStatus
** Descriptions:        DMA�ж�״̬��ȡ
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      u8Stream        DMA��Stream��ţ�Ϊ0-7
** output parameters:   NONE
** Returned value:      ��ǰͨ�����ж�״̬������Ϊ���м���ֵ�����������
**                      DMA_INT_STATUS_TC          ��������ж�
**                      DMA_INT_STATUS_HT          ����һ��ʱ���ж�
**                      DMA_INT_STATUS_TE          ��������ж�
**                      DMA_INT_STATUS_DME         ֱ��ģʽ�����ж�
**                      DMA_INT_STATUS_FIFO        FIFO�����ж�
*********************************************************************************************************/
rt_uint32_t DMAIntStatus(rt_uint32_t u32Base, rt_uint8_t u8Stream)
{
    rt_uint8_t u8RegOdd = u8Stream / 4;
    rt_uint8_t u8RegOffset = u8RegOdd * 4;
    u8RegOdd =  u8Stream % 4;
    rt_uint32_t u32Temp;

    u32Temp = HWREG32(u32Base + DMA_LISR + u8RegOffset);
    switch(u8RegOdd)
    {
        case 0:
            u32Temp = (u32Temp & (rt_uint32_t)0x0000003D);
            break;
        case 1:
            u32Temp = ((u32Temp >> 6) & (rt_uint32_t)0x0000003D);
            break;
        case 2:
            u32Temp = ((u32Temp >> 16) & (rt_uint32_t)0x0000003D);
            break;
        case 3:
            u32Temp = ((u32Temp >> 22) & (rt_uint32_t)0x0000003D);
            break;
        default:
            break;
    }

    return (u32Temp);
}

/*********************************************************************************************************
** Function name:       DMAIntStatus
** Descriptions:        DMA�жϽ���
** input parameters:    u32Base          DMAģ��Ļ���ַ
**                      u8Stream         DMA��Stream��ţ�Ϊ0-7
**                      u32IntFlags      ��ǰͨ�����ж�״̬������Ϊ���м���ֵ�Ļ�
**                      DMA_INT_STATUS_TC          ��������ж�
**                      DMA_INT_STATUS_HT          ����һ��ʱ���ж�
**                      DMA_INT_STATUS_TE          ��������ж�
**                      DMA_INT_STATUS_DME         ֱ��ģʽ�����ж�
**                      DMA_INT_STATUS_FIFO        FIFO�����ж�
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void DMAIntClear(rt_uint32_t u32Base, rt_uint8_t u8Stream,rt_uint32_t u32IntFlags)
{
    rt_uint8_t u8RegOdd = u8Stream / 4;
    rt_uint8_t u8RegOffset = u8RegOdd * 4;
    u8RegOdd =  u8Stream % 4;
    rt_uint32_t u32Temp;

    u32Temp = HWREG32(u32Base + DMA_LIFCR + u8RegOffset);
    switch(u8RegOdd)
    {
        case 0:
            u32Temp |= (u32IntFlags & (rt_uint32_t)0x0000003D);
            break;
        case 1:
            u32Temp |= ((u32IntFlags & (rt_uint32_t)0x0000003D) << 6);
            break;
        case 2:
            u32Temp |= ((u32IntFlags & (rt_uint32_t)0x0000003D) << 16);
            break;
        case 3:
            u32Temp |= ((u32IntFlags & (rt_uint32_t)0x0000003D) << 22);
            break;
        default:
            break;
    }
    HWREG32(u32Base + DMA_LIFCR + u8RegOffset) = u32Temp;

}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
