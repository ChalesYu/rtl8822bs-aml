/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_keyscan.c
** Last modified Date:  2019-06-02
** Last Version:        v1.00
** Description:         �����ж�����
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2019-06-02
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <string.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "hal/cortexm/hal_cortexm_api.h"
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_syscfg.h"
#include "hal/arch/inc/arch_io_exti.h"
#include "hal/arch/inc/arch_io_gpio.h"

/*********************************************************************************************************
** �����ӿڶ���
*********************************************************************************************************/
#define KEY_SCAN_PERIPH         RCC_PERIPHERAL_GPIOA    // ������������
#define KEY_SCAN_GPIO           GPIOA_BASE              // ��������GPIO����ַ
#define KEY_SCAN_PIN            GPIO_PIN_4              // ������������
#define KEY_SCAN_INT_NUM        ARCH_INT_EXTI4          // �����жϱ��
#define KEY_EXTI_GPIO_X         EXTI_GPIO_A             // �ⲿ�ж�����
#define KEY_EXTI_GPIO_LINE      4                      // �ⲿ�ж��߱��


/*********************************************************************************************************
** Flash���Խṹ�嶨��
*********************************************************************************************************/
typedef struct {
    rt_sem_t   sem_keyscan;
}tStm32h7xx_keyscan_info;

/*********************************************************************************************************
** ������������
*********************************************************************************************************/
static tStm32h7xx_keyscan_info __GtStm32h7xx_keyscan_info;


/*********************************************************************************************************
** Function name:       KeyScan_ISR
** Descriptions:        �����ж�
** input parameters:    NONE
**                      NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void KeyScan_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  ExtiIntEventClear(KEY_EXTI_GPIO_LINE);
  rt_sem_release(__GtStm32h7xx_keyscan_info.sem_keyscan);
  
  /* leave interrupt */
  rt_interrupt_leave();
}

/*********************************************************************************************************
** Function name:       rhb_stm32_keyscan_init
** Descriptions:        ��ʼ������stm32f2xx keyscan����
** input parameters:    handle: ��������������
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static rt_err_t rhb_stm32_keyscan_init (rt_device_t dev)
{
    tStm32h7xx_keyscan_info *handle = dev->user_data;

    // ���������ź���
    handle->sem_keyscan = rt_sem_create("keyscan", 0, RT_IPC_FLAG_FIFO);
    
    // ��ʼ����������
    SystemPeripheralEnable(RCC_PERIPHERAL_SYSCFG);
    SystemPeripheralEnable(KEY_SCAN_PERIPH);
    GPIOPinTypeGPIOInput(KEY_SCAN_GPIO, KEY_SCAN_PIN);

    // ��ʼ�������ж�
    // ����EXTI0����Ӧ����
    SyscfgExtiLineConfig(KEY_EXTI_GPIO_X, KEY_EXTI_GPIO_LINE);
    // ����EXTI
    ExtiIntEventRisingDisable(KEY_EXTI_GPIO_LINE);
    ExtiIntEventFallingEnable(KEY_EXTI_GPIO_LINE);
    ExtiEventDisable(KEY_EXTI_GPIO_LINE);
    ExtiIntEnable(KEY_EXTI_GPIO_LINE);
    
    // NVIC����
    hal_int_register(KEY_SCAN_INT_NUM, KeyScan_ISR);
    hal_int_priority_set(KEY_SCAN_INT_NUM, 0x00);
    hal_int_enable(KEY_SCAN_INT_NUM);
    
    return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_keyscan_open
** Descriptions:        ��STM32Ƭ��Flash������
** input parameters:    dev:    �豸�����ṹ
**                      oflag�� �򿪰�ʱ
** output parameters:   NONE
** Returned value:      �򿪽��
*********************************************************************************************************/
static rt_err_t rhb_stm32_keyscan_open(rt_device_t dev, rt_uint16_t oflag)
{
  ExtiIntEnable(KEY_EXTI_GPIO_LINE);
  hal_int_enable(KEY_SCAN_INT_NUM);
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_keyscan_close
** Descriptions:        �ر�Ƭ��Flash������
** input parameters:    dev: �豸�����ṹ
** output parameters:   NONE
** Returned value:      �رս��
*********************************************************************************************************/
static rt_err_t rhb_stm32_keyscan_close(rt_device_t dev)
{
  ExtiIntDisable(KEY_EXTI_GPIO_LINE);
  hal_int_disable(KEY_SCAN_INT_NUM);
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_keyscan_read
** Descriptions:        ������״̬���������²�̸��ŷ��أ����������߳�
** input parameters:    dev: ��������������
**                      pos:    ����ƫ�Ƶ�ַ
**                      buffer: ��ȡ���ݴ�ŵ�ַ
**                      size:  	 ��ȡ���ݳ���
** output parameters:   ʵ�ʽ��յ������ݳ���
*********************************************************************************************************/
static rt_size_t rhb_stm32_keyscan_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
  rt_uint32_t u32Count = 0;
  tStm32h7xx_keyscan_info *handle = dev->user_data;
  
  if((buffer == NULL) || (size <= 0)) {
    return 0;
  }
  
  // �ȴ������¼�����
  rt_sem_take(handle->sem_keyscan, RT_WAITING_FOREVER);
  // �ȴ������ɿ�
  while(size--) {
    if(GPIOPinInputRead(KEY_SCAN_GPIO, KEY_SCAN_PIN) & KEY_SCAN_PIN) {
      break;
    }
    u32Count++;
    rt_thread_delay(1);
  }
  // ���С�ڻ����1��˵���Ƕ��������ذ���δ������
  if(u32Count <= 1) {
    return 0;
  }
  
  // ���ذ��µļ����Լ����µ�ʱ�䳤�ȣ���msΪ��λ
  u32Count = u32Count * (1000 / RT_TICK_PER_SECOND);
  if (u32Count == 0) {
    u32Count = 1;
  }
  
  return u32Count;
}

/*********************************************************************************************************
** Function name:       rhb_stm32_keyscan_write
** Descriptions:        д����
** input parameters:    dev: ��������������
**                      pos:    ����ƫ�Ƶ�ַ�����ڴ��ڣ���ֵ��Ч
**                      buffer: �������ݴ�ŵ�ַ
**                      size:  	 �������ݳ���
** output parameters:   ʵ��д������ݳ���
** ע�⣺д֮ǰ�����Ȳ���Flash
*********************************************************************************************************/
static rt_size_t rhb_stm32_keyscan_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
  return 0;
}

/*********************************************************************************************************
** Function name:       rrhb_stm32_keyscan_control
** Descriptions:        �������ƣ���������������ʵ��
** input parameters:    dev: 	��������������
**                      cmd:	��������
**                      args: 	���ò���
** output parameters:   NONE
** Returned value:      ���ƽ��
*********************************************************************************************************/
static rt_err_t rhb_stm32_keyscan_control (rt_device_t dev, int cmd, void *args)
{
  if(cmd != 0) {
    return RT_ERROR;
  }
  if(GPIOPinInputRead(KEY_SCAN_GPIO, KEY_SCAN_PIN) & KEY_SCAN_PIN) {
    *((rt_bool_t *)args) = RT_FALSE;
  } else {
    *((rt_bool_t *)args) = RT_TRUE;
  }
  
  return RT_EOK;
}

/*********************************************************************************************************
** ���������ṹ��֧���������豸������
*********************************************************************************************************/
static struct rt_device __gKeyScanDriver;

/*********************************************************************************************************
** Function name:       rt_stm32_keyscan_driver_init
** Descriptions:        stm32��GPIO��������
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int rt_stm32_keyscan_driver_init(void)
{
  rt_device_t device;
  
  // ע������
  device = &__gKeyScanDriver;
  device->type 		= RT_Device_Class_Char;  //RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_stm32_keyscan_init;
  device->open		= rhb_stm32_keyscan_open;
  device->close		= rhb_stm32_keyscan_close;
  device->read 		= rhb_stm32_keyscan_read;
  device->write 	= rhb_stm32_keyscan_write;
  device->control 	= rhb_stm32_keyscan_control;
  device->user_data	= (void *)&__GtStm32h7xx_keyscan_info;
  
  /* register a character device */
  rt_device_register(device, "keyscan", RT_DEVICE_FLAG_RDWR);
  
  return 0;
}

/*********************************************************************************************************
** �����Զ���ʼ������
*********************************************************************************************************/
INIT_DEVICE_EXPORT(rt_stm32_keyscan_driver_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
