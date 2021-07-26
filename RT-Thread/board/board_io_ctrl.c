/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_io_ctrl.c
** Last modified Date:  2018-06-16
** Last Version:        v1.00
** Description:         IO��������
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2018-06-16
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

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_gpio.h"

/*********************************************************************************************************
ȫ�ֱ������� 
*********************************************************************************************************/
#define RCC_IO_CTRL_PERIPHERAL1      RCC_PERIPHERAL_GPIOB
//#define RCC_IO_CTRL_PERIPHERAL2      RCC_PERIPHERAL_IOPC
//#define RCC_IO_CTRLPERIPHERAL3      RCC_PERIPHERAL_IOPD
//#define RCC_IO_CTRL_PERIPHERAL4      RCC_PERIPHERAL_IOPE
//#define RCC_IO_CTRL_PERIPHERAL5      RCC_PERIPHERAL_IOPA

struct tIOCtrlInfo {
  rt_uint32_t u32Base;
  rt_uint16_t u16Pin;
};

// IO���Ӹı����޸Ĵ˽ṹ��ֵ
const struct tIOCtrlInfo __gtIOCtrlArray[] = //�����ṹ�����飬�ڶ����ʱ��ֱ�Ӹ�ֵ
{
  {GPIOB_BASE, GPIO_PIN_14},   // WIFIģ���Դ����    PB14
};

/*********************************************************************************************************
** Function name:       rt_hw_io_ctrl_init
** Descriptions:        IO���Ƴ�ʼ��
** Input parameters:    None ��
** Output parameters:   None ��
** Returned value:      None ��
*********************************************************************************************************/
static int rt_hw_io_ctrl_init(void)
{
  rt_uint32_t num;
  rt_uint32_t i;
#ifdef RCC_IO_CTRL_PERIPHERAL1
  SystemPeripheralEnable(RCC_IO_CTRL_PERIPHERAL1);
#endif
  
#ifdef RCC_IO_CTRL_PERIPHERAL2
  SystemPeripheralEnable(RCC_IO_CTRL_PERIPHERAL2);
#endif

#ifdef RCC_IO_CTRL_PERIPHERAL3
  SystemPeripheralEnable(RCC_IO_CTRL_PERIPHERAL3);
#endif

#ifdef RCC_IO_CTRL_PERIPHERAL4
  SystemPeripheralEnable(RCC_IO_CTRL_PERIPHERAL4);
#endif
  
#ifdef RCC_IO_CTRL_PERIPHERAL5
  SystemPeripheralEnable(RCC_IO_CTRL_PERIPHERAL5);
#endif
  
  // ��ʼ��IO
  num = sizeof(__gtIOCtrlArray) / sizeof(struct tIOCtrlInfo);
  for(i=0; i<num; i++) {
     GPIOPinTypeGPIOOutput(__gtIOCtrlArray[i].u32Base, __gtIOCtrlArray[i].u16Pin);
     GPIOPinSetBit(__gtIOCtrlArray[i].u32Base, __gtIOCtrlArray[i].u16Pin);
  }
  
  return 0;
}

/*********************************************************************************************************
** Function name:       rt_hw_io_ctrl_set
** Descriptions:        ���ÿ��Ƶ�IOΪ1
** Input parameters:    u8IO: ָ����IO
** Output parameters:   None ��
** Returned value:      None ��
*********************************************************************************************************/
void rt_hw_io_ctrl_set(rt_uint8_t u8IO)
{
  rt_uint32_t num = sizeof(__gtIOCtrlArray) / sizeof(struct tIOCtrlInfo);
  if(u8IO >= num) {
    return;
  }

  GPIOPinSetBit(__gtIOCtrlArray[u8IO].u32Base, __gtIOCtrlArray[u8IO].u16Pin);
}

/*********************************************************************************************************
** Function name:       rt_hw_io_ctrl_reset
** Descriptions:        ���ÿ��Ƶ�IOΪ0
** Input parameters:    u8IO: ָ����IO
** Output parameters:   None ��
** Returned value:      None ��
*********************************************************************************************************/
void rt_hw_io_ctrl_reset(rt_uint8_t u8IO)
{
  rt_uint32_t num = sizeof(__gtIOCtrlArray) / sizeof(struct tIOCtrlInfo);
  if(u8IO >= num) {
    return;
  }
  
  GPIOPinResetBit(__gtIOCtrlArray[u8IO].u32Base, __gtIOCtrlArray[u8IO].u16Pin);
}

/*********************************************************************************************************
** �����Զ���ʼ������
*********************************************************************************************************/
INIT_BOARD_EXPORT(rt_hw_io_ctrl_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
