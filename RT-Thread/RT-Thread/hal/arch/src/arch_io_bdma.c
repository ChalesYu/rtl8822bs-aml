/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_bdma.c
** Last modified Date:  2016-01-19
** Last Version:        v1.0
** Description:         dmaģ��Ĵ�����װ����ʵ��������������STM32L4xx
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
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_bdma.h"


/*********************************************************************************************************
** Function name:       BDMAModeSet
** Descriptions:        BDMAģ�鹤��ģʽ����
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
**                      ulConfig        DMA���ò���������ȡ���м�������Ļ�
**                      BDMA_DIR_M_READ            //  �Ӵ洢��������֮Ϊ�����
**                      BDMA_CIRC_EN               //  ִ��ѭ������ģʽ
**                      BDMA_PINC_EN               //  ʹ�������ַ����ģʽ
**                      BDMA_MINC_EN               //  ʹ�ܴ洢����ַ����ģʽ
**                      //  �����ĸ�����ȡ���е�һ��������ֵ���л�����
**                      BDMA_PSIZE_8               //  �������ݿ��Ϊ8λ
**                      BDMA_PSIZE_16              //  �������ݿ��Ϊ16λ
**                      BDMA_PSIZE_32              //  �������ݿ��Ϊ32λ
**                      BDMA_PSIZE_RESERVED        //  �������ݿ�ȱ���
**                      //  �����ĸ�����ȡ���е�һ��������ֵ���л�����
**                      BDMA_MSIZE_8               //  �洢�����ݿ��Ϊ8λ
**                      BDMA_MSIZE_16              //  �洢�����ݿ��Ϊ16λ
**                      BDMA_MSIZE_32              //  �洢�����ݿ��Ϊ32λ
**                      BDMA_MSIZE_RESERVED        //  �洢�����ݿ�ȱ���
**                      //  �����ĸ�����ȡ���е�һ��������ֵ���л�����
**                      BDMA_PL_LOW                //  ͨ�����ȼ�Ϊ��
**                      BDMA_PL_MEDIUM             //  ͨ�����ȼ�Ϊ��
**                      BDMA_PL_HIGH               //  ͨ�����ȼ�Ϊ��
**                      BDMA_PL_HITHEST            //  ͨ�����ȼ�Ϊ���
**                      //  ����2������ȡ���е�һ��������ֵ���л�����
**                      BDMA_MODE_NORMAL           //  ��������ģʽ
**                      BDMA_MODE_CIRCULAR         //  ѭ������ģʽ
**
**                      BDMA_MTOM_EN               //  �洢�����洢������ģʽʹ��
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAModeSet(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulConfig)
{
    rt_uint32_t ulTemp;
    ulTemp = HWREG32(ulBase + BDMA_CCR1 + (ucChannel)*0x14);
    ulTemp &= 0xFFFF800F;
    ulTemp |= ulConfig;
    HWREG32(ulBase + BDMA_CCR1+ (ucChannel)*0x14)= ulTemp;
}

/*********************************************************************************************************
** Function name:       BDMAAddrSet
** Descriptions:        BDMA����洢����ַ�������ַ����
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
**                      ulMemAddr       ָ��DMA����Ĵ洢����ַ
**                      ulPeriAddr      ָ��DMA����������ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAAddrSet(rt_uint32_t ulBase, rt_uint8_t ucChannel, 
                rt_uint32_t ulMemAddr, rt_uint32_t ulPeriAddr)
{
    HWREG32(ulBase + BDMA_CPAR1 + (ucChannel)*0x14)= ulPeriAddr;
    HWREG32(ulBase + BDMA_CM0AR1 + (ucChannel)*0x14)= ulMemAddr;
}

/*********************************************************************************************************
** Function name:       BDMAMemAddr1Set
** Descriptions:        BDMA����洢����ַ1����
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
**                      ulMemAddr       ָ��DMA����Ĵ洢����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAMemAddr1Set(rt_uint32_t ulBase, rt_uint8_t ucChannel, rt_uint32_t ulMemAddr)
{
    HWREG32(ulBase + BDMA_CM1AR1 + (ucChannel)*0x14)= ulMemAddr;
}

/*********************************************************************************************************
** Function name:       BDMABufferSizeSet
** Descriptions:        BDMAģ�鴫�����ݻ�������С����
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
**                      usSize          ָ������Ĵ�С
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMABufferSizeSet(rt_uint32_t ulBase, rt_uint8_t ucChannel, rt_uint16_t usSize)
{
    HWREG32(ulBase + BDMA_CNDTR1 + (ucChannel)*0x14)= usSize;
}

/*********************************************************************************************************
** Function name:       BDMABufferSizeGet
** Descriptions:        BDMAģ�鴫�����ݻ�������С����
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t BDMABufferSizeGet(rt_uint32_t ulBase, rt_uint8_t ucChannel)
{
    return HWREG32(ulBase + BDMA_CNDTR1 + (ucChannel)*0x14);
}

/*********************************************************************************************************
** Function name:       BDMAEnable
** Descriptions:        BDMAģ��ʹ��
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAEnable(rt_uint32_t ulBase, rt_uint8_t ucChannel)
{
    HWREG32(ulBase + BDMA_CCR1 + (ucChannel)*0x14) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       BDMAModeSet
** Descriptions:        BDMAģ�����
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMADisable(rt_uint32_t ulBase, rt_uint8_t ucChannel)
{
    HWREG32(ulBase + BDMA_CCR1 + (ucChannel)*0x14) &= 0xFFFFFFFE;
}

/*********************************************************************************************************
** Function name:       BDMAIsEnabled
** Descriptions:        �ж�BDMA�Ƿ�ʹ�ܴ���
** input parameters:    u32Base         DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
** output parameters:   NONE
** Returned value:      RT_TRUE:   ʹ����DMA����
**                      RT_FALSE:  ������DMA����
*********************************************************************************************************/
rt_bool_t BDMAIsEnabled(rt_uint32_t u32Base, rt_uint8_t ucChannel)
{
    if(HWREG32(u32Base + BDMA_CCR1 + (ucChannel)*0x14) & 0x00000001) {
        return RT_TRUE;
    } else {
        return RT_FALSE;
    }
}

/*********************************************************************************************************
** Function name:       DMAIntEnable
** Descriptions:        DMA�ж�ʹ��
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
**                      ulFlags         ָ��ʹ�ܵ��жϣ�����ȡ����ֵ�Ļ�
**                      DMA_INT_TC      //  ��������ж�
**                      DMA_INT_HT      //  ����һ��ʱ���ж�
**                      DMA_INT_TE      //  ��������ж�
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAIntEnable(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulFlags)
{
    HWREG32(ulBase + BDMA_CCR1 + (ucChannel)*0x14) |= ulFlags;
}

/*********************************************************************************************************
** Function name:       BDMAIntDisable
** Descriptions:        BDMA�жϽ���
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
**                      ulFlags         ָ��ʹ�ܵ��жϣ�����ȡ����ֵ�Ļ�
**                      BDMA_INT_TC      //  ��������ж�
**                      BDMA_INT_HT      //  ����һ��ʱ���ж�
**                      BDMA_INT_TE      //  ��������ж�
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAIntDisable(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulFlags)
{
    HWREG32(ulBase + BDMA_CCR1 + (ucChannel)*0x14) &= ~ulFlags;
}

/*********************************************************************************************************
** Function name:       BDMAIntStatus
** Descriptions:        BDMA�ж�״̬��ȡ
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
** output parameters:   NONE
** Returned value:      ��ǰͨ�����ж�״̬������Ϊ��������ֵ����������������DMA_INT_GLOBAL�Ļ�
**                      BDMA_INT_GLOBAL����ʾȫ���ж�״̬��ֻҪ��������ֵ��һ����������ͳ���
**                      BDMA_INT_TC      //  ��������ж�
**                      BDMA_INT_HT      //  ����һ��ʱ���ж�
**                      BDMA_INT_TE      //  ��������ж�
*********************************************************************************************************/
rt_uint32_t BDMAIntStatus(rt_uint32_t ulBase, rt_uint8_t ucChannel)
{
    rt_uint32_t ulTemp;
    ulTemp = HWREG32(ulBase + BDMA_ISR);
    ulTemp >>= ((ucChannel) * 4);
    
    return (ulTemp);
}

/*********************************************************************************************************
** Function name:       BDMAIntStatus
** Descriptions:        BDMA�ж����
** input parameters:    ulBase          DMAģ��Ļ���ַ
**                      ucChannel       DMAͨ����ţ�DMA1Ϊ0-7��DMA2Ϊ0-7
**                      ulIntFlags      ��ǰͨ�����ж�״̬������Ϊ��������ֵ�Ļ������DMA_INT_GLOBAL�Ļ�
**                      BDMA_INT_GLOBAL����ʾȫ���ж�״̬��ֻҪ��������ֵ��һ����������ͳ���
**                      BDMA_INT_TC      //  ��������ж�
**                      BDMA_INT_HT      //  ����һ��ʱ���ж�
**                      BDMA_INT_TE      //  ��������ж�
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void BDMAIntClear(rt_uint32_t ulBase, rt_uint8_t ucChannel,rt_uint32_t ulIntFlags)
{
    rt_uint32_t ulTemp = (ulIntFlags << ((ucChannel) * 4));
    
    HWREG32(ulBase + BDMA_IFCR) |= ulTemp;
    
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
