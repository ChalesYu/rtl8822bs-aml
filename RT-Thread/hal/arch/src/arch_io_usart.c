/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_usart.c
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
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_usart.h"


/*********************************************************************************************************
  �ֲ���������
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       UsartConfigSet
** Descriptions:        usartģ������
** input parameters:    u32Base  usartģ�����ַ
**                      u32Clock �ṩ��usartģ���ʱ��,����AHB����APB1��APB2��Ϊ�������ߵ�Ƶ��
**                      u32Baud  ͨ�Ų�����,ֱ��ָ������
**                      u32Config ������Ϣ������ȡ��������ֵ�Ļ�
**                      USART_STOP_0_5          //  0.5λֹͣλ
**                      USART_STOP_1            //  1λֹͣλ
**                      UASRT_STOP_1_5          //  1.5λֹͣλ
**                      USART_STOP_2            //  2λֹͣλ

**                      USART_WLEN_8            //  �ֳ�Ϊ8
**                      USART_WLEN_9            //  �ֳ�Ϊ9

**                      USART_PAR_NONE          //  ��У��
**                      USART_PAR_EVEN          //  żУ��
**                      USART_PAR_ODD           //  ��У��
**                      
**                      USART_CTSE_ENABLE       // ����CTSʹ��
**                      USART_CTSE_DISABLE      // ����CTS��ֹ
**                      
**                      USART_RTSE_ENABLE       // ����RTSʹ��
**                      USART_RTSE_DISABLE      // ����RTS��ֹ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartConfigSet(rt_uint32_t u32Base, rt_uint32_t u32Clock,
                    rt_uint32_t u32Baud, rt_uint32_t u32Config)
{
  rt_uint32_t u32Temp;
  
  // ���ô���ʱ��Ԥ��Ƶ
  HWREG32(u32Base + USART_PRESC) = 0x02;
  u32Clock /= 4;
  
  //  ����ֹͣλ�������üĴ���CR2
  u32Temp = HWREG32(u32Base + USART_CR2);
  u32Temp &= ~((rt_uint32_t)(0x3000));
  u32Temp |= ((u32Config & 0x03) << 12);
  HWREG32(u32Base + USART_CR2) = u32Temp;
  
  //  ���üĴ���CR1���������ֳ���У��
  u32Temp = HWREG32(u32Base + USART_CR1);
  u32Temp &= ~((rt_uint32_t)(0x1000170C));
  u32Temp |= u32Config;
  u32Temp |= 0x000C;    //  ʹ�ܷ��ͺͽ���
  HWREG32(u32Base + USART_CR1) = u32Temp;
  
  //  ע�⣺���ؿ���, ����CR3�Ĵ���
  u32Temp = HWREG32(u32Base + USART_CR3);
  u32Temp &= ~(0x0300);
  u32Temp |= (u32Config << 4);
  HWREG32(u32Base + USART_CR3) = u32Temp;
  
  //  ���ݲ����ʼ���BRR�Ĵ�����ֵ����д������
  //  �ȼ�����������
  if(u32Base != LPUART1_BASE) {
    if((HWREG32(u32Base + USART_CR1)) & 0x8000) {
      u32Temp = u32Clock * 2;
      u32Temp /= u32Baud;
    } else {
      u32Temp = u32Clock;
      u32Temp /= u32Baud;
    }
  } else {
    rt_uint64_t u64Temp = (rt_uint64_t)u32Clock * 256;
    u32Temp = u64Temp / u32Baud;
  }
  
  HWREG32(u32Base + USART_BRR) = u32Temp;
}

/*********************************************************************************************************
** Function name:       UsartEnable
** Descriptions:        usartģ��ʹ�ܣ�ʹ�ܺ�ģ�������������
** input parameters:    u32Base  usartģ�����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + USART_CR1) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       UsartDisable
** Descriptions:        usartģ����ܣ����ܺ�ģ�����ֹͣ����������������usartǰͨ���ȵ��øú���
** input parameters:    u32Base  usartģ�����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartDisable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + USART_CR1) &=  ~((rt_uint32_t)(0x0001));
}


/*********************************************************************************************************
** Function name:       UsartDMAEnable
** Descriptions:        usartģ��DMA����ʹ��
** input parameters:    u32Base  usartģ�����ַ
**                      u32Flags ָ��ʹ�ܵ�DMA���ܣ�����ȡ����ֵ
**                      USART_DMA_TX    //  DMA���͹���
**                      USART_DMA_RX    //  DMA���ܹ���
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartDMAEnable(rt_uint32_t u32Base,rt_uint32_t u32Flags)
{
  HWREG32(u32Base + USART_CR3) |= u32Flags;
}
/*********************************************************************************************************
** Function name:       UsartDMADisable
** Descriptions:        usartģ��DMA���ܽ���
** input parameters:    u32Base  usartģ�����ַ
**                      u32Flags ָ�����ܵ�DMA���ܣ�����ȡ����ֵ
**                      USART_DMA_TX    //  DMA���͹���
**                      USART_DMA_RX    //  DMA���ܹ���
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartDMADisable(rt_uint32_t u32Base, rt_uint32_t u32Flags)
{
  HWREG32(u32Base + USART_CR3) &= ~u32Flags;
}
/*********************************************************************************************************
** Function name:       UsartIntEnable
** Descriptions:        usartģ���ж�ʹ��
** input parameters:    u32Base:     usartģ�����ַ
**                      u32IntFlags: ��Ҫʹ�ܵ��ж�, ��������г����õ��жϣ������Ŀ���ͨ���Ĵ������ã�����
**                                  ȡ����ֵ�Ļ�
**                      USART_INT_EIE               //  ֡�����ж�
**                      USART_INT_PE                //  У�����
**                      USART_INT_TX_EMPTY          //  ���ͻ��������ж�
**                      USART_INT_TX_FINISH         //  ��������ж�
**                      USART_INT_RX                //  �����ж�ʹ�ܣ�˵�����ռĴ�������������
**                      USART_INT_IDLE              //  IDLE�ж�ʹ��
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartIntEnable(rt_uint32_t u32Base,rt_uint32_t u32IntFlags)
{
    HWREG32(u32Base + USART_CR1) |= (u32IntFlags & 0xFFFFFDFF);
    HWREG32(u32Base + USART_CR3) |= ((u32IntFlags >> 9) & 0x01UL);
}

/*********************************************************************************************************
** Function name:       UsartIntEnable
** Descriptions:        usartģ���жϽ���
** input parameters:    u32Base:     usartģ�����ַ
**                      u32IntFlags: ��Ҫ���ܵ��ж�, ��������г����õ��жϣ������Ŀ���ͨ���Ĵ������ã�����
**                                  ȡ����ֵ�Ļ�
**                      USART_INT_EIE               //  ֡�����ж�
**                      USART_INT_PE                //  У�����
**                      USART_INT_TX_EMPTY          //  ���ͻ��������ж�
**                      USART_INT_TX_FINISH         //  ��������ж�
**                      USART_INT_RX                //  �����ж�ʹ�ܣ�˵�����ռĴ�������������
**                      USART_INT_IDLE              //  IDLE�ж�ʹ��
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartIntDisable(rt_uint32_t u32Base,rt_uint32_t u32IntFlags)
{
    rt_uint32_t u32Temp;
    
    u32Temp = (u32IntFlags & 0xFFFFFDFF);
    HWREG32(u32Base + USART_CR1) &= ~u32Temp;
    u32Temp = ((u32IntFlags >> 9) & 0x01UL);
    HWREG32(u32Base + USART_CR3) &= ~u32Temp;
}

/*********************************************************************************************************
** Function name:       UsartSpaceAvial
** Descriptions:        �жϷ��ͼĴ����Ƿ�Ϊ�գ����Ƿ���Է�����һ������
** input parameters:    u32Base  usartģ�����ַ
** output parameters:   NONE
** Returned value:      RT_TRUE: ���Է�����һ������ ��RT_FALSE�����ܷ�����һ������
*********************************************************************************************************/
rt_bool_t UsartSpaceAvial(rt_uint32_t u32Base)
{
    if (HWREG32(u32Base + USART_ISR) & 0x0080)
        return RT_TRUE;
    else
        return RT_FALSE;
}

/*********************************************************************************************************
** Function name:       UsartCharPut
** Descriptions:        ��������
** input parameters:    u32Base  usartģ�����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartCharPut(rt_uint32_t u32Base,rt_uint8_t u8Data)
{
    HWREG32(u32Base + USART_TDR) = u8Data;           //  ������д�뷢�ͼĴ���
}

/*********************************************************************************************************
** Function name:       UsartCharsAvial
** Descriptions:        �жϽ��ռĴ������Ƿ��п�������
** input parameters:    u32Base  usartģ�����ַ
** output parameters:   NONE
** Returned value:      RT_TRUE: ���ܼĴ������п������ݣ����Զ���; RT_FALSE: ���ռĴ������޿�������
*********************************************************************************************************/
rt_bool_t UsartCharsAvial(rt_uint32_t u32Base)
{
    if(HWREG32(u32Base + USART_ISR) & 0x0020)
        return RT_TRUE;
    else
        return RT_FALSE;
}

/*********************************************************************************************************
** Function name:       UsartCharPut
** Descriptions:        �������ݣ����ӽ��ռĴ����ж�������
** input parameters:    u32Base  usartģ�����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t  UsartCharGet(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + USART_RDR) & 0x01FF);   //  �������ݣ���9Ϊ��Ч
}

/*********************************************************************************************************
** Function name:       UsartPutS
** Descriptions:        ����һ��ָ�����ַ���
** input parameters:    u32Base: usartģ�����ַ
**                      s:      ָ�����͵��ַ���
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartPutS(rt_uint32_t u32Base,const char *s)
{
    while (*s != '\0') {
       while(!(UsartSpaceAvial(u32Base))) ;          //  �ȴ����ͼĴ�������
       UsartCharPut(u32Base,*(s++));
    }
}
/*********************************************************************************************************
** Function name:       UsartPutS
** Descriptions:        ����һ��ָ�����ȵ��ַ���
** input parameters:    u32Base: usartģ�����ַ
**                      s:      ָ�������ַ����Ļ���ַ
**                      usNum:  ָ�������ַ����ĳ���
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartPutN(rt_uint32_t u32Base, const  char  *pStr , rt_uint16_t  u16Num)
{
    while(u16Num)
    {
        if(UsartSpaceAvial(u32Base))                 //  ������ͻ������пռ䣬����䷢�ͼĴ���
        {
            UsartCharPut(u32Base,*pStr++);
            u16Num--;
        }
    }
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
