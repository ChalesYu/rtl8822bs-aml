/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_gpio.c
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         gpioģ��Ĵ�����װ����ʵ������
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
#include "hal/arch/inc/arch_io_gpio.h"


/*********************************************************************************************************
  �ֲ���������
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       GPIOModeSet
** Descriptions:        GPIO����ģʽ����
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
**                      u32Mode�� ���õĹ���ģʽ������ȡ����ֵ��
**                      GPIO_MODE_IN        // GPIO����ģʽ
**                      GPIO_MODE_OUT       // GPIO���ģʽ
**                      GPIO_MODE_AF        // GPIO���踴��ģʽ
**                      GPIO_MODE_AN        // GPIOģ��ģʽ��������ADC��DAC����ʱ�õ�
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOModeSet(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint32_t u32Mode)
{
    rt_uint32_t i;

    for(i=0; i<16; i++)
    {
        if(u32Pins & (1<<i)) {
            HWREG32(u32Base + GPIOx_MODER) &= ~((rt_uint32_t)0x00000003 << (2 * i));
            HWREG32(u32Base + GPIOx_MODER) |= (u32Mode << (2 * i));
            u32Pins &= ~(1<<i);
        }

        if(!(u32Pins & 0xFFFFFFFFF)) {
            break;
        }
    }
}

/*********************************************************************************************************
** Function name:       GPIOOutputTypeSet
** Descriptions:        GPIO���ŵ������������
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
**                      u32Mode�� ���õĹ���ģʽ������ȡ����ֵ��
**                      GPIO_OUT_PP     // GPIO����ģʽ
**                      GPIO_OUT_OD     // GPIO��©ģʽ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOOutputTypeSet(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint32_t u32Mode)
{
    rt_uint32_t i;

    for(i=0; i<16; i++)
    {
        if(u32Pins & (1<<i)) {
            if(u32Mode == GPIO_OUT_PP) {
                HWREG32(u32Base + GPIOx_OTYPER) &= ~((rt_uint32_t)1 << i);
            } else {
                HWREG32(u32Base + GPIOx_OTYPER) |= ((rt_uint32_t)1 << i);
            }
            u32Pins &= ~(1<<i);
        }

        if(!(u32Pins & 0xFFFFFFFFF)) {
            break;
        }
    }
}

/*********************************************************************************************************
** Function name:       GPIOOutputSpeedSet
** Descriptions:        GPIO���ŵ�����ٶ�����
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
**                      u32Mode�� ���õĹ���ģʽ������ȡ����ֵ��
**                      GPIO_SPEED_2MHZ     // ����
**                      GPIO_SPEED_25MHZ    // ����
**                      GPIO_SPEED_50MHZ    // ����
**                      GPIO_SPEED_100MHZ   // ���� on 30 pF (80 MHz Output max speed on 15 pF)
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOOutputSpeedSet(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint32_t u32Mode)
{
    rt_uint32_t i;

    for(i=0; i<16; i++)
    {
        if(u32Pins & (1<<i)) {
            HWREG32(u32Base + GPIOx_OSPEEDR) &= ~((rt_uint32_t)0x00000003 << (2 * i));
            HWREG32(u32Base + GPIOx_OSPEEDR) |= (u32Mode << (2 * i));
            u32Pins &= ~(1<<i);
        }

        if(!(u32Pins & 0xFFFFFFFFF)) {
            break;
        }
    }
}

/*********************************************************************************************************
** Function name:       GPIOPullUpDownSet
** Descriptions:        GPIO���ŵ�������������
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
**                      u32Mode�� ���õĹ���ģʽ������ȡ����ֵ��
**                      GPIO_PUPD_NOPULL    // ������������
**                      GPIO_PUPD_UP        // ����
**                      GPIO_PUPD_DOWN      // ����
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPullUpDownSet(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint32_t u32Mode)
{
    rt_uint32_t i;

    for(i=0; i<16; i++)
    {
        if(u32Pins & (1<<i)) {
            HWREG32(u32Base + GPIOx_PUPDR) &= ~((rt_uint32_t)0x00000003 << (2 * i));
            HWREG32(u32Base + GPIOx_PUPDR) |= (u32Mode << (2 * i));
            u32Pins &= ~(1<<i);
        }

        if(!(u32Pins & 0xFFFFFFFFF)) {
            break;
        }
    }
}
/*********************************************************************************************************
** Function name:       GPIOPinInputRead
** Descriptions:        GPIO���Ŷ�ȡ����ȡIDR�Ĵ���
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
** output parameters:   NONE
** Returned value:      IO״̬��Ϊλͼ��־
*********************************************************************************************************/
rt_uint32_t GPIOPinInputRead(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    return (HWREG32(u32Base + GPIOx_IDR) & u32Pins);
}


/*********************************************************************************************************
** Function name:       GPIOPinOutputRead
** Descriptions:        GPIO���Ŷ�ȡ����ȡODR�Ĵ���
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
** output parameters:   NONE
** Returned value:      IO״̬��Ϊλͼ��־
*********************************************************************************************************/
rt_uint32_t GPIOPinOutputRead(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    return (HWREG32(u32Base + GPIOx_ODR) & u32Pins);
}

/*********************************************************************************************************
** Function name:       GPIOPinWrite
** Descriptions:        GPIO���
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Value��д��GPIO��ֵ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinWrite(rt_uint32_t u32Base, rt_uint32_t u32Value)
{
    HWREG32(u32Base + GPIOx_ODR) = u32Value;
}

/*********************************************************************************************************
** Function name:       GPIOPinSetBit
** Descriptions:        ָ����IO��������Ϊ��
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinSetBit(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    HWREG16(u32Base + GPIOx_BSRR_SET) = u32Pins;
}

/*********************************************************************************************************
** Function name:       GPIOPinResetBit
** Descriptions:        ָ����IO��������Ϊ��
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinResetBit(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    HWREG16(u32Base + GPIOx_BSRR_RESET) = u32Pins;
}

/*********************************************************************************************************
** Function name:       GPIOPinToggleBit
** Descriptions:        ��תָ����IO����
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinToggleBit(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    HWREG32(u32Base + GPIOx_ODR) ^= u32Pins;
}

/*********************************************************************************************************
** Function name:       GPIOPinWriteBit
** Descriptions:        дGPIO��ָ������
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
**                      u8State:  Ϊ״̬��Ϊ0����1
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinWriteBit(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint8_t u8State)
{
    if(0 == u8State){
        HWREG16(u32Base + GPIOx_BSRR_RESET) = u32Pins;
    } else {
        HWREG16(u32Base + GPIOx_BSRR_SET) = u32Pins;
    }
}

/*********************************************************************************************************
** Function name:       GPIOPinReadBit
** Descriptions:        ��GPIO��ָ������
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
** output parameters:   NONE
** Returned value:      ����״̬
*********************************************************************************************************/
rt_uint8_t GPIOPinReadBit(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
  return (!!(HWREG32(u32Base + GPIOx_IDR) & u32Pins));
}

/*********************************************************************************************************
** Function name:       GPIOPinAFConfig
** Descriptions:        AF��������
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pin��  ָ�����õ�GPIO���ţ�ȡ0-15
**                      u32AFMode:  AF���ܣ�����ȡ���ù��ܣ� GPIO_AF_xx��һЩֵ������ο�arch_io_gpio.h
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinAFConfig(rt_uint32_t u32Base, rt_uint32_t u32Pin, rt_uint32_t u32AFMode)
{
    rt_uint32_t i;
    rt_uint32_t u32AFReg[2] = {GPIOx_AFRL, GPIOx_AFRH};

    i = u32Pin / 8;
    u32Pin %= 8;

    HWREG32(u32Base + u32AFReg[i]) &= ~((rt_uint32_t)0x0000000F << (4 * u32Pin));
    HWREG32(u32Base + u32AFReg[i]) |= (u32AFMode << (4 * u32Pin));
}

/*********************************************************************************************************
** Function name:       GPIOPinTypeGPIOOutput
** Descriptions:        ����ָ����GPIO����Ϊ�������ģʽ
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinTypeGPIOOutput(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    GPIOModeSet(u32Base, u32Pins,GPIO_MODE_OUT);
    GPIOOutputTypeSet(u32Base, u32Pins, GPIO_OUT_PP);
    GPIOOutputSpeedSet(u32Base, u32Pins, GPIO_SPEED_VERY_HIGH);
    GPIOPullUpDownSet(u32Base, u32Pins, GPIO_PUPD_NOPULL);
}

/*********************************************************************************************************
** Function name:       GPIOPinTypeGPIOInput
** Descriptions:        ����ָ����GPIO����Ϊ��������ģʽ
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinTypeGPIOInput(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    GPIOModeSet(u32Base, u32Pins, GPIO_MODE_IN);
    GPIOPullUpDownSet(u32Base, u32Pins, GPIO_PUPD_NOPULL);
}

/*********************************************************************************************************
** Function name:       GPIOPinTypeGPIOAF
** Descriptions:        ����GPIO����Ϊָ����AF���ܣ�ֻ��һ��һ������
** input parameters:    u32Base:  ָ��������GPIOģ�����ַ
**                      u32Pins�� ָ��������GPIO����
**                      u32Pin��  ָ�����õ�GPIO���ţ�ȡ0-15
**                      u32AFMode:  AF���ܣ�����ȡ���ù��ܣ� GPIO_AF_xx��һЩֵ������ο�arch_io_gpio.h
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinTypeGPIOAF(rt_uint32_t u32Base, rt_uint32_t u32Pins, rt_uint32_t u32Pin, rt_uint32_t u32AFMode)
{
    GPIOPinAFConfig(u32Base, u32Pin, u32AFMode);

    GPIOModeSet(u32Base,u32Pins,GPIO_MODE_AF);
    GPIOOutputTypeSet(u32Base,u32Pins,GPIO_OUT_PP);
    GPIOOutputSpeedSet(u32Base,u32Pins,GPIO_SPEED_VERY_HIGH);
    GPIOPullUpDownSet(u32Base,u32Pins,GPIO_PUPD_NOPULL);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
