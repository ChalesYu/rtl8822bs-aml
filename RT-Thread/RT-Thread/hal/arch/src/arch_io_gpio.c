/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_gpio.c
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         gpio模块寄存器封装函数实现声明
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2019-03-15
** Version:             v1.0
** Descriptions:        The original version 初始版本
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
  局部变量定义
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       GPIOModeSet
** Descriptions:        GPIO工作模式设置
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
**                      u32Mode： 设置的工作模式，可以取下列值：
**                      GPIO_MODE_IN        // GPIO输入模式
**                      GPIO_MODE_OUT       // GPIO输出模式
**                      GPIO_MODE_AF        // GPIO外设复用模式
**                      GPIO_MODE_AN        // GPIO模拟模式，当用作ADC或DAC引脚时用到
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
** Descriptions:        GPIO引脚的输出类型配置
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
**                      u32Mode： 设置的工作模式，可以取下列值：
**                      GPIO_OUT_PP     // GPIO推挽模式
**                      GPIO_OUT_OD     // GPIO开漏模式
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
** Descriptions:        GPIO引脚的输出速度配置
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
**                      u32Mode： 设置的工作模式，可以取下列值：
**                      GPIO_SPEED_2MHZ     // 低速
**                      GPIO_SPEED_25MHZ    // 中速
**                      GPIO_SPEED_50MHZ    // 快速
**                      GPIO_SPEED_100MHZ   // 高速 on 30 pF (80 MHz Output max speed on 15 pF)
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
** Descriptions:        GPIO引脚的上拉下拉配置
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
**                      u32Mode： 设置的工作模式，可以取下列值：
**                      GPIO_PUPD_NOPULL    // 无上拉和下拉
**                      GPIO_PUPD_UP        // 上拉
**                      GPIO_PUPD_DOWN      // 下拉
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
** Descriptions:        GPIO引脚读取，读取IDR寄存器
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
** output parameters:   NONE
** Returned value:      IO状态，为位图标志
*********************************************************************************************************/
rt_uint32_t GPIOPinInputRead(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    return (HWREG32(u32Base + GPIOx_IDR) & u32Pins);
}


/*********************************************************************************************************
** Function name:       GPIOPinOutputRead
** Descriptions:        GPIO引脚读取，读取ODR寄存器
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
** output parameters:   NONE
** Returned value:      IO状态，为位图标志
*********************************************************************************************************/
rt_uint32_t GPIOPinOutputRead(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    return (HWREG32(u32Base + GPIOx_ODR) & u32Pins);
}

/*********************************************************************************************************
** Function name:       GPIOPinWrite
** Descriptions:        GPIO输出
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Value：写到GPIO的值
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinWrite(rt_uint32_t u32Base, rt_uint32_t u32Value)
{
    HWREG32(u32Base + GPIOx_ODR) = u32Value;
}

/*********************************************************************************************************
** Function name:       GPIOPinSetBit
** Descriptions:        指定是IO引脚设置为高
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinSetBit(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    HWREG16(u32Base + GPIOx_BSRR_SET) = u32Pins;
}

/*********************************************************************************************************
** Function name:       GPIOPinResetBit
** Descriptions:        指定是IO引脚设置为低
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinResetBit(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    HWREG16(u32Base + GPIOx_BSRR_RESET) = u32Pins;
}

/*********************************************************************************************************
** Function name:       GPIOPinToggleBit
** Descriptions:        翻转指定的IO引脚
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void GPIOPinToggleBit(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
    HWREG32(u32Base + GPIOx_ODR) ^= u32Pins;
}

/*********************************************************************************************************
** Function name:       GPIOPinWriteBit
** Descriptions:        写GPIO的指定引脚
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
**                      u8State:  为状态，为0或者1
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
** Descriptions:        读GPIO的指定引脚
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
** output parameters:   NONE
** Returned value:      引脚状态
*********************************************************************************************************/
rt_uint8_t GPIOPinReadBit(rt_uint32_t u32Base, rt_uint32_t u32Pins)
{
  return (!!(HWREG32(u32Base + GPIOx_IDR) & u32Pins));
}

/*********************************************************************************************************
** Function name:       GPIOPinAFConfig
** Descriptions:        AF功能配置
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pin：  指定配置的GPIO引脚，取0-15
**                      u32AFMode:  AF功能，可以取复用功能， GPIO_AF_xx的一些值，具体参看arch_io_gpio.h
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
** Descriptions:        设置指定的GPIO引脚为推挽输出模式
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
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
** Descriptions:        设置指定的GPIO引脚为浮空输入模式
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
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
** Descriptions:        设置GPIO引脚为指定的AF功能，只能一个一个配置
** input parameters:    u32Base:  指定操作的GPIO模块基地址
**                      u32Pins： 指定操作的GPIO引脚
**                      u32Pin：  指定配置的GPIO引脚，取0-15
**                      u32AFMode:  AF功能，可以取复用功能， GPIO_AF_xx的一些值，具体参看arch_io_gpio.h
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
