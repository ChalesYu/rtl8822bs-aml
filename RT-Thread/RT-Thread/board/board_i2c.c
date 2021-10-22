/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_spi.c
** Last modified Date:  2014-12-13
** Last Version:        v1.00
** Description:         spi接口的驱动程序
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2014-12-13
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/serial.h>
#include <drivers/spi.h>

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"

#include "board_i2c.h"

#ifdef RT_USING_I2C_BITOPS
/*********************************************************************************************************
** SPI硬件连接配置
*********************************************************************************************************/
#ifdef RT_USING_I2C1
// use PB6 PB7
#define I2C1_GPIO_PERIPHERAL    RCC_PERIPHERAL_GPIOB
//#define I2C1_GPIO_PERIPHERAL1   RCC_PERIPHERAL_GPIOD
#define I2C1_GPIO_SCL_BASE      GPIOB_BASE
#define I2C1_GPIO_SDA_BASE      GPIOB_BASE
#define I2C1_GPIO_SCL_PIN       GPIO_PIN_6
#define I2C1_GPIO_SDA_PIN       GPIO_PIN_7

#define I2C1_SCL_H()            (HWREG16(I2C1_GPIO_SCL_BASE + GPIOx_BSRR_SET) = I2C1_GPIO_SCL_PIN) 
#define I2C1_SCL_L()            (HWREG16(I2C1_GPIO_SCL_BASE + GPIOx_BSRR_RESET) = I2C1_GPIO_SCL_PIN)
#define I2C1_SDA_H()            (HWREG16(I2C1_GPIO_SDA_BASE + GPIOx_BSRR_SET) = I2C1_GPIO_SDA_PIN) 
#define I2C1_SDA_L()            (HWREG16(I2C1_GPIO_SDA_BASE + GPIOx_BSRR_RESET) = I2C1_GPIO_SDA_PIN)
#define I2C1_SCL_GET()          !!(HWREG32(I2C1_GPIO_SCL_BASE + GPIOx_IDR) & I2C1_GPIO_SCL_PIN)
#define I2C1_SDA_GET()          !!(HWREG32(I2C1_GPIO_SDA_BASE + GPIOx_IDR) & I2C1_GPIO_SDA_PIN)

#endif

#ifdef RT_USING_I2C2
// use PE2、PE3
#define I2C2_GPIO_PERIPHERAL    RCC_PERIPHERAL_GPIOB
//#define I2C2_GPIO_PERIPHERAL1   RCC_PERIPHERAL_GPIOD
#define I2C2_GPIO_SCL_BASE      GPIOB_BASE
#define I2C2_GPIO_SDA_BASE      GPIOB_BASE
#define I2C2_GPIO_SCL_PIN       GPIO_PIN_8
#define I2C2_GPIO_SDA_PIN       GPIO_PIN_9

#define I2C2_SCL_H()            (HWREG16(I2C2_GPIO_SCL_BASE + GPIOx_BSRR_SET) = I2C2_GPIO_SCL_PIN) 
#define I2C2_SCL_L()            (HWREG16(I2C2_GPIO_SCL_BASE + GPIOx_BSRR_RESET) = I2C2_GPIO_SCL_PIN)
#define I2C2_SDA_H()            (HWREG16(I2C2_GPIO_SDA_BASE + GPIOx_BSRR_SET) = I2C2_GPIO_SDA_PIN) 
#define I2C2_SDA_L()            (HWREG16(I2C2_GPIO_SDA_BASE + GPIOx_BSRR_RESET) = I2C2_GPIO_SDA_PIN)
#define I2C2_SCL_GET()          !!(HWREG32(I2C2_GPIO_SCL_BASE + GPIOx_IDR) & I2C2_GPIO_SCL_PIN)
#define I2C2_SDA_GET()          !!(HWREG32(I2C2_GPIO_SDA_BASE + GPIOx_IDR) & I2C2_GPIO_SDA_PIN)
#endif

#ifdef RT_USING_I2C3
// use PC10、PC11
#endif


#ifdef RT_USING_I2C1
/*********************************************************************************************************
** Function name:       rt_hw_i2c_init
** Descriptions:        i2c驱动初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static struct rt_i2c_bus_device i2c_device0;

static void gpio0_set_sda(void *data, rt_int32_t state)
{
    if (state)
    {
        I2C1_SDA_H();
    }
    else
    {
        I2C1_SDA_L();
    }
}

static void gpio0_set_scl(void *data, rt_int32_t state)
{
    if (state)
    {
        I2C1_SCL_H();
    }
    else
    {
        I2C1_SCL_L(); 
    }
}

static rt_int32_t gpio0_get_sda(void *data)
{
    return I2C1_SDA_GET();
}

static rt_int32_t gpio0_get_scl(void *data)
{
    return I2C1_SCL_GET();
}

static const struct rt_i2c_bit_ops bit_ops0 =
{
    RT_NULL,
    gpio0_set_sda,
    gpio0_set_scl,
    gpio0_get_sda,
    gpio0_get_scl,

    rt_hw_us_delay,

    2,
    100
};
#endif /* endif of RT_USING_I2C1 */

#ifdef RT_USING_I2C2
/*********************************************************************************************************
** Function name:       rt_hw_i2c_init
** Descriptions:        i2c驱动初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static struct rt_i2c_bus_device i2c_device1;

static void gpio1_set_sda(void *data, rt_int32_t state)
{
    if (state)
    {
        I2C2_SDA_H();
    }
    else
    {
        I2C2_SDA_L();
    }
}

static void gpio1_set_scl(void *data, rt_int32_t state)
{
    if (state)
    {
        I2C2_SCL_H();
    }
    else
    {
        I2C2_SCL_L(); 
    }
}

static rt_int32_t gpio1_get_sda(void *data)
{
    return I2C2_SDA_GET();
}

static rt_int32_t gpio1_get_scl(void *data)
{
    return I2C2_SCL_GET();
}

static const struct rt_i2c_bit_ops bit_ops1 =
{
    RT_NULL,
    gpio1_set_sda,
    gpio1_set_scl,
    gpio1_get_sda,
    gpio1_get_scl,

    rt_hw_us_delay,

    2,
    100
};
#endif /* endif of RT_USING_I2C2 */

/*********************************************************************************************************
** Function name:       rt_hw_i2c_init
** Descriptions:        i2c驱动初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
int rt_hw_i2c_init(void)
{
#ifdef RT_USING_I2C1
  SystemPeripheralEnable(I2C1_GPIO_PERIPHERAL);
  
#ifdef I2C1_GPIO_PERIPHERAL1
  SystemPeripheralEnable(I2C1_GPIO_PERIPHERAL1);
#endif
  
  // scl
  GPIOModeSet(I2C1_GPIO_SCL_BASE, I2C1_GPIO_SCL_PIN, GPIO_MODE_OUT);
  GPIOOutputTypeSet(I2C1_GPIO_SCL_BASE, I2C1_GPIO_SCL_PIN, GPIO_OUT_OD);
  GPIOOutputSpeedSet(I2C1_GPIO_SCL_BASE, I2C1_GPIO_SCL_PIN, GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(I2C1_GPIO_SCL_BASE, I2C1_GPIO_SCL_PIN, GPIO_PUPD_UP);
  GPIOPinSetBit(I2C1_GPIO_SCL_BASE, I2C1_GPIO_SCL_PIN);
  
  // sda
  GPIOModeSet(I2C1_GPIO_SDA_BASE, I2C1_GPIO_SDA_PIN, GPIO_MODE_OUT);
  GPIOOutputTypeSet(I2C1_GPIO_SDA_BASE, I2C1_GPIO_SDA_PIN, GPIO_OUT_OD);
  GPIOOutputSpeedSet(I2C1_GPIO_SDA_BASE, I2C1_GPIO_SDA_PIN, GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(I2C1_GPIO_SDA_BASE, I2C1_GPIO_SDA_PIN, GPIO_PUPD_UP);
  GPIOPinSetBit(I2C1_GPIO_SDA_BASE, I2C1_GPIO_SDA_PIN);
  
  rt_memset((void *)&i2c_device0, 0, sizeof(struct rt_i2c_bus_device));
  i2c_device0.priv = (void *)&bit_ops0;
  rt_i2c_bit_add_bus(&i2c_device0, "i2c0");
#endif
  
#ifdef RT_USING_I2C2
  SystemPeripheralEnable(I2C2_GPIO_PERIPHERAL);
  
#ifdef I2C2_GPIO_PERIPHERAL1
  SystemPeripheralEnable(I2C2_GPIO_PERIPHERAL1);
#endif
  
  // scl
  GPIOModeSet(I2C2_GPIO_SCL_BASE, I2C2_GPIO_SCL_PIN, GPIO_MODE_OUT);
  GPIOOutputTypeSet(I2C2_GPIO_SCL_BASE, I2C2_GPIO_SCL_PIN, GPIO_OUT_OD);
  GPIOOutputSpeedSet(I2C2_GPIO_SCL_BASE, I2C2_GPIO_SCL_PIN, GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(I2C2_GPIO_SCL_BASE, I2C2_GPIO_SCL_PIN, GPIO_PUPD_UP);
  GPIOPinSetBit(I2C2_GPIO_SCL_BASE, I2C2_GPIO_SCL_PIN);
  
  // sda
  GPIOModeSet(I2C2_GPIO_SDA_BASE, I2C2_GPIO_SDA_PIN, GPIO_MODE_OUT);
  GPIOOutputTypeSet(I2C2_GPIO_SDA_BASE, I2C2_GPIO_SDA_PIN, GPIO_OUT_OD);
  GPIOOutputSpeedSet(I2C2_GPIO_SDA_BASE, I2C2_GPIO_SDA_PIN, GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(I2C2_GPIO_SDA_BASE, I2C2_GPIO_SDA_PIN, GPIO_PUPD_UP);
  GPIOPinSetBit(I2C2_GPIO_SDA_BASE, I2C2_GPIO_SDA_PIN);
  
  rt_memset((void *)&i2c_device1, 0, sizeof(struct rt_i2c_bus_device));
  i2c_device1.priv = (void *)&bit_ops1;
  rt_i2c_bit_add_bus(&i2c_device1, "i2c1");
#endif
  
#ifdef RT_USING_I2C3
#endif
  
  return 0;
}
#endif   /* end of RT_USING_I2C_BITOPS */

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_PREV_EXPORT(rt_hw_i2c_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
