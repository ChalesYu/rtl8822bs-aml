/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_usart_all_dma.c
** Last modified Date:  2014-12-13
** Last Version:        v1.00
** Description:         ���������������ͺͽ��ն�������DMAģʽ,�������ĳ�ʼ��ֻ�ܹ��ڲ���ϵͳ��������߳��С�
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

#include "hal/cortexm/hal_cortexm_api.h"

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_dmamux.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"
#include "hal/arch/inc/arch_io_usart.h"

#include "board_serial.h"

#if defined(RT_USING_ALL_DMA_UART1) || defined(RT_USING_ALL_DMA_UART2) || defined(RT_USING_ALL_DMA_UART3) || \
defined(RT_USING_ALL_DMA_UART4) || defined(RT_USING_ALL_DMA_UART5) || defined(RT_USING_ALL_DMA_UART6) || \
defined(RT_USING_ALL_DMA_UART7) || defined(RT_USING_ALL_DMA_UART8)

/*********************************************************************************************************
** Ӳ������
*********************************************************************************************************/
#ifdef RT_USING_ALL_DMA_UART1
#define UART1_INT_NO            ARCH_INT_USART1
#define UART1_TX_DMA_USE_NO     0, 42
#define UART1_RX_DMA_USE_NO     1, 41
#define UART1_TX_DMA_INT_NO     ARCH_INT_DMA1_STREAM0
#define UART1_RX_DMA_INT_NO     ARCH_INT_DMA1_STREAM1
#endif

#ifdef RT_USING_ALL_DMA_UART2
#define UART2_INT_NO            ARCH_INT_USART2
#define UART2_TX_DMA_USE_NO     2, 44
#define UART2_RX_DMA_USE_NO     3, 43
#define UART2_TX_DMA_INT_NO     ARCH_INT_DMA1_STREAM2
#define UART2_RX_DMA_INT_NO     ARCH_INT_DMA1_STREAM3
#endif

#ifdef RT_USING_ALL_DMA_UART3
#define UART3_INT_NO            ARCH_INT_USART3
#define UART3_TX_DMA_USE_NO     4, 46
#define UART3_RX_DMA_USE_NO     5, 45
#define UART3_TX_DMA_INT_NO     ARCH_INT_DMA1_STREAM4
#define UART3_RX_DMA_INT_NO     ARCH_INT_DMA1_STREAM5
#endif

#ifdef RT_USING_ALL_DMA_UART4
#define UART4_INT_NO            ARCH_INT_UART4
#define UART4_TX_DMA_USE_NO     6, 64
#define UART4_RX_DMA_USE_NO     7, 63
#define UART4_TX_DMA_INT_NO     ARCH_INT_DMA1_STREAM6
#define UART4_RX_DMA_INT_NO     ARCH_INT_DMA1_STREAM7
#endif

#ifdef RT_USING_ALL_DMA_UART5
#define UART5_INT_NO            ARCH_INT_UART5
#define UART5_TX_DMA_USE_NO     8, 66
#define UART5_RX_DMA_USE_NO     9, 65
#define UART5_TX_DMA_INT_NO     ARCH_INT_DMA2_STREAM0
#define UART5_RX_DMA_INT_NO     ARCH_INT_DMA2_STREAM1
#endif

#ifdef RT_USING_ALL_DMA_UART6
#define UART6_INT_NO            ARCH_INT_USART6
#define UART6_TX_DMA_USE_NO     10, 72
#define UART6_RX_DMA_USE_NO     11, 71
#define UART6_TX_DMA_INT_NO     ARCH_INT_DMA2_STREAM2
#define UART6_RX_DMA_INT_NO     ARCH_INT_DMA2_STREAM3
#endif

#ifdef RT_USING_ALL_DMA_UART7
#define UART7_INT_NO            ARCH_INT_UART7
#define UART7_TX_DMA_USE_NO     12, 80
#define UART7_RX_DMA_USE_NO     13, 79
#define UART7_TX_DMA_INT_NO     ARCH_INT_DMA2_STREAM4
#define UART7_RX_DMA_INT_NO     ARCH_INT_DMA2_STREAM5
#endif

#ifdef RT_USING_ALL_DMA_UART8
#define UART8_INT_NO            ARCH_INT_UART8
#define UART8_TX_DMA_USE_NO     14, 82
#define UART8_RX_DMA_USE_NO     15, 81
#define UART8_TX_DMA_INT_NO     ARCH_INT_DMA2_STREAM6
#define UART8_RX_DMA_INT_NO     ARCH_INT_DMA2_STREAM7
#endif

/*********************************************************************************************************
** ��Ҫ�ĺ궨��
*********************************************************************************************************/
#define Serial_RxBuf_Size       128                 //  ���ڽ�����FIFO�Ĵ�С

/*********************************************************************************************************
** �������ڵĽṹ�嶨��
*********************************************************************************************************/
//  ���շ��;�����DMA�������Զ���
typedef struct
{
  rt_uint32_t base;                             //  ģ�����ַ
  rt_uint16_t  u16WhichBus; rt_uint32_t u32Ctrl;  //  �����������ԣ�����ȷ������ʱ��
  rt_uint32_t  u32GPIOBase;                       // ��Ϊ0������RS485���Ƶ�GPIO����ַ
  rt_uint16_t  u16Pin;                            // RS485���Ƶ����ű��
  rt_mutex_t   send_mutex;                        //  ���̷߳��ͱ���
  rt_mutex_t   recv_mutex;                        //  ���߳̽��ձ���
  rt_uint32_t TxDMA_base;                       //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  rt_uint8_t  TxDMA_Stream;                     //  ����DMA Sream���
  struct rt_completion Rx_Comp;                //  ������������
  struct rt_completion Tx_Comp;                //  ������������
  rt_uint32_t RxDMA_base;                 	//  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  rt_uint8_t   RxDMA_Stream;                	//  ����DMA Sream���
  int     rx_len;                         	//  ��ǰ���յ������ݸ���
  int     rx_tol_len;                     	//  �ܹ���Ҫ���ܵ����ݸ���
  int     rx_dma_value;                   	//  ����DMA���õ�ֵ
  rt_uint8_t   buf[Serial_RxBuf_Size];     	//  ������FIFO
  int     buf_read;                       	//  ���ն�ָ��
  int     buf_write;                      	//  ���ջ���дָ��
  rt_uint32_t  RecvTimeoutSave;            	//  ���ճ�ʱ���������棬�������ñ���
  rt_uint32_t  RecvTimeout;                 	//  ���ճ�ʱ������
  rt_uint8_t   TimeOutIndex;                    //  DMA��ȡ�±�
  rt_uint32_t  RxDMAValue[2];              	//  ����DMA�ĵ�ǰֵ
  rt_uint32_t  RecvIntervalTimeSave;            //  ����ʱ����
  rt_uint32_t  RecvIntervalTime;                //  ����ʱ����������
  struct serial_configure serial_Config;  	//  ��ǰ��������
} stm32_serial_DMA_info;

/*********************************************************************************************************
ȫ�ֱ�������
*********************************************************************************************************/
#ifdef RT_USING_ALL_DMA_UART1
#define RS485_CTRL1_BASE          GPIOA_BASE
#define RS485_CTRL1_PIN           GPIO_PIN_8
stm32_serial_DMA_info tStm32Usart1Info = {
  USART1_BASE,                        //  ģ�����ַ
  RCC_PERIPHERAL_USART1,              //  �����������ԣ�����ȷ������ʱ��
  RS485_CTRL1_BASE,                   // ��Ϊ0������RS485���Ƶ�GPIO����ַ
  RS485_CTRL1_PIN,                    // RS485���Ƶ����ű��
  RT_NULL,                            //  ���̷߳��ͱ���
  RT_NULL,                            //  ���߳̽��ձ���
  DMA1_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  0,                                  //  ����DMA Sream���
  {0},                                //  ������������
  {0},                                //  ������������
  DMA1_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  1,                                  //  ����DMA Sream���
  0,                                  //  ��ǰ���յ������ݸ���
  0,                                  //  �ܹ���Ҫ���ܵ����ݸ���
  0,                                  //  ����DMA���õ�ֵ
  {0},                                //  ������FIFO
  0,                                  //  ���ն�ָ��
  0,                                  //  ���ջ���дָ��
  0,                                  //  ���ճ�ʱ���������棬�������ñ���
  0,                                  //  ���ճ�ʱ������
  2,                                  //  DMA��ȡ�±�
  {0},                                //  ����DMA�ĵ�ǰֵ
  0,                                  //  ����ʱ����
  0,                                  //  ����ʱ����������
  {0},                                //  ��ǰ��������
};
#endif

#ifdef RT_USING_ALL_DMA_UART2
stm32_serial_DMA_info tStm32Usart2Info = {
  USART2_BASE,                        //  ģ�����ַ
  RCC_PERIPHERAL_USART2,              //  �����������ԣ�����ȷ������ʱ��
  0,                                  // ��Ϊ0������RS485���Ƶ�GPIO����ַ
  0,                                  // RS485���Ƶ����ű��
  RT_NULL,                            //  ���̷߳��ͱ���
  RT_NULL,                            //  ���߳̽��ձ���
  DMA1_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  2,                                  //  ����DMA Sream���
  {0},                         	      //  ������������
  {0},                        	      //  ������������
  DMA1_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  3,                                  //  ����DMA Sream���
  0,                                  //  ��ǰ���յ������ݸ���
  0,                                  //  �ܹ���Ҫ���ܵ����ݸ���
  0,                                  //  ����DMA���õ�ֵ
  {0},                                //  ������FIFO
  0,                                  //  ���ն�ָ��
  0,                                  //  ���ջ���дָ��
  0,                                  //  ���ճ�ʱ���������棬�������ñ���
  0,                                  //  ���ճ�ʱ������
  2,                                  //  DMA��ȡ�±�
  {0},                                //  ����DMA�ĵ�ǰֵ
  0,                                  //  ����ʱ����
  0,                                  //  ����ʱ����������
  {0},                                //  ��ǰ��������
};
#endif

#ifdef RT_USING_ALL_DMA_UART3
stm32_serial_DMA_info tStm32Usart3Info = {
  USART3_BASE,                        //  ģ�����ַ
  RCC_PERIPHERAL_USART3,              //  �����������ԣ�����ȷ������ʱ��
  0,                                  // ��Ϊ0������RS485���Ƶ�GPIO����ַ
  0,                                  // RS485���Ƶ����ű��
  RT_NULL,                            //  ���̷߳��ͱ���
  RT_NULL,                            //  ���߳̽��ձ���
  DMA1_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  4,                                  //  ����DMA Sream���
  {0},                         	      //  ������������
  {0},                        	      //  ������������
  DMA1_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  5,                                  //  ����DMA Sream���
  0,                                  //  ��ǰ���յ������ݸ���
  0,                                  //  �ܹ���Ҫ���ܵ����ݸ���
  0,                                  //  ����DMA���õ�ֵ
  {0},                                //  ������FIFO
  0,                                  //  ���ն�ָ��
  0,                                  //  ���ջ���дָ��
  0,                                  //  ���ճ�ʱ���������棬�������ñ���
  0,                                  //  ���ճ�ʱ������
  2,                                  //  DMA��ȡ�±�
  {0},                                //  ����DMA�ĵ�ǰֵ
  0,                                  //  ����ʱ����
  0,                                  //  ����ʱ����������
  {0},                                //  ��ǰ��������
};
#endif

#ifdef RT_USING_ALL_DMA_UART4
stm32_serial_DMA_info tStm32Usart4Info = {
  UART4_BASE,                        //  ģ�����ַ
  RCC_PERIPHERAL_UART4,              //  �����������ԣ�����ȷ������ʱ��
  0,                                  // ��Ϊ0������RS485���Ƶ�GPIO����ַ
  0,                                  // RS485���Ƶ����ű��
  RT_NULL,                            //  ���̷߳��ͱ���
  RT_NULL,                            //  ���߳̽��ձ���
  DMA1_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  6,                                  //  ����DMA Sream���
  {0},                         	      //  ������������
  {0},                        	      //  ������������
  DMA1_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  7,                                  //  ����DMA Sream���
  0,                                  //  ��ǰ���յ������ݸ���
  0,                                  //  �ܹ���Ҫ���ܵ����ݸ���
  0,                                  //  ����DMA���õ�ֵ
  {0},                                //  ������FIFO
  0,                                  //  ���ն�ָ��
  0,                                  //  ���ջ���дָ��
  0,                                  //  ���ճ�ʱ���������棬�������ñ���
  0,                                  //  ���ճ�ʱ������
  2,                                  //  DMA��ȡ�±�
  {0},                                //  ����DMA�ĵ�ǰֵ
  0,                                  //  ����ʱ����
  0,                                  //  ����ʱ����������
  {0},                                //  ��ǰ��������
};
#endif

#ifdef RT_USING_ALL_DMA_UART5
stm32_serial_DMA_info tStm32Usart5Info = {
  UART5_BASE,                        //  ģ�����ַ
  RCC_PERIPHERAL_UART5,              //  �����������ԣ�����ȷ������ʱ��
  0,                                  // ��Ϊ0������RS485���Ƶ�GPIO����ַ
  0,                                  // RS485���Ƶ����ű��
  RT_NULL,                            //  ���̷߳��ͱ���
  RT_NULL,                            //  ���߳̽��ձ���
  DMA2_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  0,                                  //  ����DMA Sream���
  {0},                         	      //  ������������
  {0},                        	      //  ������������
  DMA2_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  1,                                  //  ����DMA Sream���
  0,                                  //  ��ǰ���յ������ݸ���
  0,                                  //  �ܹ���Ҫ���ܵ����ݸ���
  0,                                  //  ����DMA���õ�ֵ
  {0},                                //  ������FIFO
  0,                                  //  ���ն�ָ��
  0,                                  //  ���ջ���дָ��
  0,                                  //  ���ճ�ʱ���������棬�������ñ���
  0,                                  //  ���ճ�ʱ������
  2,                                  //  DMA��ȡ�±�
  {0},                                //  ����DMA�ĵ�ǰֵ
  0,                                  //  ����ʱ����
  0,                                  //  ����ʱ����������
  {0},                                //  ��ǰ��������
};
#endif

#ifdef RT_USING_ALL_DMA_UART6
#define RS485_CTRL2_BASE          GPIOC_BASE
#define RS485_CTRL2_PIN           GPIO_PIN_8
stm32_serial_DMA_info tStm32Usart6Info = {
  USART6_BASE,                        //  ģ�����ַ
  RCC_PERIPHERAL_USART6,              //  �����������ԣ�����ȷ������ʱ��
  RS485_CTRL2_BASE,                   // ��Ϊ0������RS485���Ƶ�GPIO����ַ
  RS485_CTRL2_PIN,                    // RS485���Ƶ����ű��
  RT_NULL,                            //  ���̷߳��ͱ���
  RT_NULL,                            //  ���߳̽��ձ���
  DMA2_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  2,                                  //  ����DMA Sream���
  {0},                         	      //  ������������
  {0},                        	      //  ������������
  DMA2_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  3,                                  //  ����DMA Sream���
  0,                                  //  ��ǰ���յ������ݸ���
  0,                                  //  �ܹ���Ҫ���ܵ����ݸ���
  0,                                  //  ����DMA���õ�ֵ
  {0},                                //  ������FIFO
  0,                                  //  ���ն�ָ��
  0,                                  //  ���ջ���дָ��
  0,                                  //  ���ճ�ʱ���������棬�������ñ���
  0,                                  //  ���ճ�ʱ������
  2,                                  //  DMA��ȡ�±�
  {0},                                //  ����DMA�ĵ�ǰֵ
  0,                                  //  ����ʱ����
  0,                                  //  ����ʱ����������
  {0},                                //  ��ǰ��������
};
#endif

#ifdef RT_USING_ALL_DMA_UART7
#define RS485_CTRL7_BASE          GPIOE_BASE
#define RS485_CTRL7_PIN           GPIO_PIN_14
stm32_serial_DMA_info tStm32Usart7Info = {
  UART7_BASE,                        //  ģ�����ַ
  RCC_PERIPHERAL_UART7,              //  �����������ԣ�����ȷ������ʱ��
  RS485_CTRL7_BASE,                   // ��Ϊ0������RS485���Ƶ�GPIO����ַ
  RS485_CTRL7_PIN,                    // RS485���Ƶ����ű��
  RT_NULL,                            //  ���̷߳��ͱ���
  RT_NULL,                            //  ���߳̽��ձ���
  DMA2_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  4,                                  //  ����DMA Sream���
  {0},                         	      //  ������������
  {0},                        	      //  ������������
  DMA2_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  5,                                  //  ����DMA Sream���
  0,                                  //  ��ǰ���յ������ݸ���
  0,                                  //  �ܹ���Ҫ���ܵ����ݸ���
  0,                                  //  ����DMA���õ�ֵ
  {0},                                //  ������FIFO
  0,                                  //  ���ն�ָ��
  0,                                  //  ���ջ���дָ��
  0,                                  //  ���ճ�ʱ���������棬�������ñ���
  0,                                  //  ���ճ�ʱ������
  2,                                  //  DMA��ȡ�±�
  {0},                                //  ����DMA�ĵ�ǰֵ
  0,                                  //  ����ʱ����
  0,                                  //  ����ʱ����������
  {0},                                //  ��ǰ��������
};
#endif

#ifdef RT_USING_ALL_DMA_UART8
#define RS485_CTRL8_BASE          GPIOD_BASE
#define RS485_CTRL8_PIN           GPIO_PIN_15
stm32_serial_DMA_info tStm32Usart8Info = {
  UART8_BASE,                        //  ģ�����ַ
  RCC_PERIPHERAL_UART8,              //  �����������ԣ�����ȷ������ʱ��
  RS485_CTRL8_BASE,                   // ��Ϊ0������RS485���Ƶ�GPIO����ַ
  RS485_CTRL8_PIN,                    // RS485���Ƶ����ű��
  RT_NULL,                            //  ���̷߳��ͱ���
  RT_NULL,                            //  ���߳̽��ձ���
  DMA2_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  6,                                  //  ����DMA Sream���
  {0},                         	      //  ������������
  {0},                        	      //  ������������
  DMA2_BASE,                          //  ����DMA����ַ������ΪDMA1_BASE��DMA2_BASE
  7,                                  //  ����DMA Sream���
  0,                                  //  ��ǰ���յ������ݸ���
  0,                                  //  �ܹ���Ҫ���ܵ����ݸ���
  0,                                  //  ����DMA���õ�ֵ
  {0},                                //  ������FIFO
  0,                                  //  ���ն�ָ��
  0,                                  //  ���ջ���дָ��
  0,                                  //  ���ճ�ʱ���������棬�������ñ���
  0,                                  //  ���ճ�ʱ������
  2,                                  //  DMA��ȡ�±�
  {0},                                //  ����DMA�ĵ�ǰֵ
  0,                                  //  ����ʱ����
  0,                                  //  ����ʱ����������
  {0},                                //  ��ǰ��������
};
#endif

/*********************************************************************************************************
** �����Ƿ�����������ݶ��еĺ�
*********************************************************************************************************/
#define Serial_Buffer_Empty(a)    \
(((a->buf_read) == (a->buf_write)) ? RT_TRUE : RT_FALSE)

#define Serial_Buffer_Full(a)     \
((((a->buf_write + 1) % Serial_RxBuf_Size) == (a->buf_read)) ? RT_TRUE : RT_FALSE)

/*********************************************************************************************************
** Function name:       rhb_serial_DMA_timer
** Descriptions:        ���ڽ��ճ�ʱ�жϺ�������Ҫ��ϵͳtick�Ĺ��Ӻ����е���
** input parameters:    parg: ��ʱ���������
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void rhb_serial_DMA_timer(void* parg)
{
  rt_device_t dev = parg;
  stm32_serial_DMA_info *serial_handle = dev->user_data;
  
  // ���DMAδʹ��ֱ�ӷ���
  if(!(DMAIsEnabled(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream))) {
    return;
  }
  
  // ��ȡ��ǰDMA��������ֵ
  serial_handle->TimeOutIndex = (serial_handle->TimeOutIndex + 1) % 2;
  serial_handle->RxDMAValue[serial_handle->TimeOutIndex] =
    DMACurrDataCounterGet(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream);
  
  // ���������ȡ�����ζ�һ������˵��û�������յ�����ʱ��
  if(serial_handle->RxDMAValue[0] == serial_handle->RxDMAValue[1]) {
    // ���DMA���������ʼֵ��ͬ��˵���������յ�
    if(serial_handle->RxDMAValue[0] != serial_handle->rx_dma_value) {
      if(serial_handle->RecvIntervalTime > 0) {
         serial_handle->RecvIntervalTime--;
      } else {
        UsartDMADisable(serial_handle->base,USART_DMA_RX);
        UsartIntEnable(serial_handle->base,USART_INT_RX);
        serial_handle->rx_dma_value = serial_handle->rx_dma_value - serial_handle->RxDMAValue[0];
        DMADisable(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream);
      }
    } else {
      if(serial_handle->RecvTimeoutSave  > 0) {
        if(serial_handle->RecvTimeout > 0) {
          serial_handle->RecvTimeout--;
        } else {
          UsartDMADisable(serial_handle->base,USART_DMA_RX);
          UsartIntEnable(serial_handle->base,USART_INT_RX);
          serial_handle->rx_dma_value = 0;
          DMADisable(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream);
        }
      }
    }
  } else {
    serial_handle->RecvIntervalTime = serial_handle->RecvIntervalTimeSave;
  }
}

/*********************************************************************************************************
** Function name:       rhb_serial_DMA_init
** Descriptions:        ��ʼ�����ڣ��ú����ڴ򿪴���ʱ����
** input parameters:    dev: �豸�����ṹ
** output parameters:   NONE
** Returned value:      RHB_ERR_OK: ���ڳ�ʼ���ɹ�;  ����, ���ڳ�ʼ��ʧ��
*********************************************************************************************************/
static rt_err_t rhb_serial_DMA_init (rt_device_t dev)
{
  rt_uint32_t u32Clock;
  rt_timer_t timer;
  stm32_serial_DMA_info *serial_handle = dev->user_data;
  
  RT_ASSERT(dev != RT_NULL);
  
  // ��ʼ�����ͱ�������
  serial_handle->send_mutex = rt_mutex_create("txMutex", RT_IPC_FLAG_FIFO);
  // ��ʼ�����ձ�������
  serial_handle->recv_mutex = rt_mutex_create("rxMutex", RT_IPC_FLAG_FIFO);
  // ��ʼ����������
  rt_completion_init(&serial_handle->Rx_Comp);
  
  // ��ʼ����������
  rt_completion_init(&serial_handle->Tx_Comp);
  
  // ��ȡ����ʱ��
  u32Clock = SystemPeripheralClockGet(serial_handle->u16WhichBus, serial_handle->u32Ctrl);
  // ���ô���
  UsartConfigSet(serial_handle->base, u32Clock, 115200,
                 (USART_WLEN_8 | USART_PAR_NONE | USART_STOP_1));
  
  serial_handle->serial_Config.baud_rate = 115200;
  serial_handle->serial_Config.data_bits = DATA_BITS_8;
  serial_handle->serial_Config.parity = PARITY_NONE;
  serial_handle->serial_Config.stop_bits = STOP_BITS_1;
  serial_handle->RxDMAValue[0] = 0;
  serial_handle->RxDMAValue[1] = 1;
  
  // ����DMAģʽ����
  DMADisable(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream);
  DMAModeConfigSet(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream,
                     DMA_DIR_MemoryToPeripheral |
                       DMA_PeripheralInc_Disable |
                         DMA_MemoryInc_Enable |
                           DMA_PeripheralDataSize_Byte |
                             DMA_MemoryDataSize_Byte |
                               DMA_Mode_Normal |
                                 DMA_Priority_High |
                                   DMA_MemoryBurst_Single |
                                     DMA_PeripheralBurst_Single);
  DMAFIFOConfigSet(serial_handle->TxDMA_base, serial_handle->TxDMA_Stream,
                   DMA_FIFOMode_Enable | DMA_FIFOThreshold_Full);
  DMAIntClear(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream, DMA_INT_STATUS_ALL);
  DMAIntDisable(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream, DMA_INT_CONFIG_ALL);
  UsartDMAEnable(serial_handle->base,USART_DMA_TX);
  
  // ����DMAģʽ����
  DMADisable(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream);
  DMAModeConfigSet(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream,
                     DMA_DIR_PeripheralToMemory |
                       DMA_PeripheralInc_Disable |
                         DMA_MemoryInc_Enable |
                           DMA_PeripheralDataSize_Byte |
                             DMA_MemoryDataSize_Byte |
                               DMA_Mode_Normal |
                                 DMA_Priority_High |
                                   DMA_MemoryBurst_Single |
                                     DMA_PeripheralBurst_Single);
  DMAFIFOConfigSet(serial_handle->RxDMA_base, serial_handle->RxDMA_Stream,
                   DMA_FIFOMode_Enable | DMA_FIFOThreshold_Full);
  DMAIntClear(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream, DMA_INT_STATUS_ALL);
  DMAIntEnable(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream, DMA_INT_CONFIG_USUAL);
  UsartDMADisable(serial_handle->base,USART_DMA_RX);
  
  /* ������ʱ��1 */
  timer = rt_timer_create("ttyTm",
                          rhb_serial_DMA_timer,
                          dev, 1, RT_TIMER_FLAG_PERIODIC);
  /* ������ʱ�� */
  if (timer != RT_NULL) rt_timer_start(timer);
  
  UsartIntEnable(serial_handle->base,USART_INT_RX | USART_INT_PE | USART_INT_EIE) ;
  UsartEnable(serial_handle->base);
  
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_serial_DMA_open
** Descriptions:        �򿪴��ڣ���ʱδ��
** input parameters:    dev: �豸�����ṹ
** output parameters:   NONE
** Returned value:      RHB_ERR_OK: ���ڳ�ʼ���ɹ�;  ����, ���ڳ�ʼ��ʧ��
*********************************************************************************************************/
static rt_err_t rhb_serial_DMA_open(rt_device_t dev, rt_uint16_t oflag)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_serial_DMA_close
** Descriptions:        �رմ���
** input parameters:    dev: �豸�����ṹ
** output parameters:   NONE
** Returned value:      RHB_ERR_OK: ���ڳ�ʼ���ɹ�;  ����, ���ڳ�ʼ��ʧ��
*********************************************************************************************************/
static rt_err_t rhb_serial_DMA_close(rt_device_t dev)
{
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       rhb_serial_DMA_read
** Descriptions:        �Ӵ��ڶ�ȡָ���ֽڵ�����,�������ж��е���
** input parameters:    dev: ��������������
**                      pos:    ����ƫ�Ƶ�ַ�����ڴ��ڣ���ֵ��Ч
**                      buffer: ��ȡ���ݴ�ŵ�ַ
**                      size:  	 ��ȡ���ݳ���
** output parameters:   ʵ�ʽ��յ������ݳ���
*********************************************************************************************************/
static rt_size_t rhb_serial_DMA_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
  rt_err_t err_code = RT_EOK;;
  rt_size_t RtnLen = 0;
  rt_uint8_t *pData = buffer;
  rt_base_t level;
  stm32_serial_DMA_info *serial_handle = dev->user_data;
  
  RT_ASSERT(dev != RT_NULL);
  
  if((buffer == RT_NULL) || ((size) <= 0)) {
    err_code = -RT_ERROR;
    rt_set_errno(err_code);
    return 0;
  }
  
  rt_mutex_take(serial_handle->recv_mutex, RT_WAITING_FOREVER);
  
  /* disable interrupt */
  level = rt_hw_interrupt_disable();
  serial_handle->rx_tol_len = size;
  serial_handle->rx_dma_value = size;
  serial_handle->RxDMAValue[0] = 0;
  serial_handle->RxDMAValue[1] = 1;
  if(!Serial_Buffer_Empty(serial_handle))
  {
    RtnLen = ((serial_handle->buf_write > serial_handle->buf_read)?(serial_handle->buf_write - serial_handle->buf_read):\
      (Serial_RxBuf_Size-serial_handle->buf_read+serial_handle->buf_write));
    
    if((serial_handle->rx_tol_len) > RtnLen)
    {
      serial_handle->rx_dma_value -= RtnLen;
      UsartIntDisable(serial_handle->base,USART_INT_RX);
      DMADisable(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream);
      DMAAddrSet(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream,
                 (rt_uint32_t)((rt_uint32_t)buffer + RtnLen),(serial_handle->base + USART_RDR));
      DMABufferSizeSet(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream,serial_handle->rx_dma_value);
      UsartDMAEnable(serial_handle->base,USART_DMA_RX);
      DMAEnable(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream);
      serial_handle->rx_len = RtnLen;
    }
  } else {
    UsartIntDisable(serial_handle->base,USART_INT_RX);
    DMADisable(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream);
    DMAAddrSet(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream,
               (rt_uint32_t)(buffer),(serial_handle->base + USART_RDR));
    DMABufferSizeSet(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream,serial_handle->rx_tol_len);
    UsartDMAEnable(serial_handle->base,USART_DMA_RX);
    DMAEnable(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream);
    serial_handle->rx_len = 0;
  }
  /* enable interrupt */
  rt_hw_interrupt_enable(level);
  
  if((serial_handle->rx_tol_len) <= RtnLen)
  {
    RtnLen = (serial_handle->rx_tol_len);
    while(RtnLen--)
    {
      *pData++ = serial_handle->buf[serial_handle->buf_read];
      serial_handle->buf_read = (serial_handle->buf_read + 1) % Serial_RxBuf_Size;
    }
    serial_handle->rx_len = 0;
    serial_handle->rx_tol_len = 0;
    rt_set_errno(err_code);
    rt_mutex_release(serial_handle->recv_mutex);
    return size;
  } else {
    while(RtnLen--)
    {
      *pData++ = serial_handle->buf[serial_handle->buf_read];
      serial_handle->buf_read = (serial_handle->buf_read + 1) % Serial_RxBuf_Size;
    }
  }
  
  if(serial_handle->RecvTimeoutSave > 0) {
    /* disable interrupt */
    level = rt_hw_interrupt_disable();
    serial_handle->RecvTimeout = serial_handle->RecvTimeoutSave;
    /* enable interrupt */
    rt_hw_interrupt_enable(level);
  }
  
  if(serial_handle->RecvIntervalTimeSave > 0) {
    /* disable interrupt */
    level = rt_hw_interrupt_disable();
    serial_handle->RecvIntervalTime = serial_handle->RecvIntervalTimeSave;
    /* enable interrupt */
    rt_hw_interrupt_enable(level);
  }
  
  // �ȴ��ź���
  rt_completion_wait(&serial_handle->Rx_Comp,RT_WAITING_FOREVER);
  /* disable interrupt */
  level = rt_hw_interrupt_disable();
  RtnLen = serial_handle->rx_len;
  serial_handle->rx_len = 0;
  serial_handle->rx_tol_len = 0;
  /* enable interrupt */
  rt_hw_interrupt_enable(level);
  
  rt_set_errno(err_code);
  rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, buffer, RtnLen);
//  rt_hw_cpu_dcache_ops((RT_HW_CACHE_FLUSH | RT_HW_CACHE_INVALIDATE), buffer, RtnLen);
  rt_mutex_release(serial_handle->recv_mutex);
  return RtnLen;
}

/*********************************************************************************************************
** Function name:       rhb_serial_DMA_write
** Descriptions:        �Ӵ��ڷ���ָ���ֽڵ����ݣ����߲���DMA���ͣ����߲����жϷ���
** input parameters:    dev: ��������������
**                      pos:    ����ƫ�Ƶ�ַ�����ڴ��ڣ���ֵ��Ч
**                      buffer: �������ݴ�ŵ�ַ
**                      size:  	 �������ݳ���
** output parameters:   ʵ�ʷ��͵����ݳ���
*********************************************************************************************************/
static rt_size_t rhb_serial_DMA_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
  rt_err_t err_code = RT_EOK;;
  stm32_serial_DMA_info *serial_handle = dev->user_data;
  
  RT_ASSERT(dev != RT_NULL);
  
  if((buffer == RT_NULL) || (size <= 0)) {
    err_code = -RT_ERROR;
    rt_set_errno(err_code);
    return 0;
  }
  
  rt_mutex_take(serial_handle->send_mutex, RT_WAITING_FOREVER);
  rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, (void *)buffer, size);
//  rt_hw_cpu_dcache_ops((RT_HW_CACHE_FLUSH | RT_HW_CACHE_INVALIDATE), (void *)buffer, size);
  if(serial_handle->u32GPIOBase != 0) {
    HWREG16(serial_handle->u32GPIOBase + GPIOx_BSRR_SET) = serial_handle->u16Pin;
    HWREG32(serial_handle->base + USART_ISR) &=  ~((rt_uint32_t)0x0040);
  } 
  DMADisable(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream);
  DMAAddrSet(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream,
             (rt_uint32_t)(buffer),(serial_handle->base + USART_TDR));
  DMABufferSizeSet(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream,size);
  DMAIntEnable(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream, DMA_INT_CONFIG_USUAL);
  DMAEnable(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream);
//  while(0 != DMACurrDataCounterGet(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream))
//  {
//    ; //rt_thread_delay(1);
//  }
  // �ȴ��������
  rt_completion_wait(&serial_handle->Tx_Comp,RT_WAITING_FOREVER);
  DMAIntDisable(serial_handle->TxDMA_base,serial_handle->TxDMA_Stream, DMA_INT_CONFIG_ALL);
  if(serial_handle->u32GPIOBase != 0) {
    // �ȴ����������
    while(!(HWREG32(serial_handle->base + USART_ISR) & 0x0040));
    HWREG16(serial_handle->u32GPIOBase + GPIOx_BSRR_RESET) = serial_handle->u16Pin;
  }
  rt_set_errno(err_code);
  rt_mutex_release(serial_handle->send_mutex);
  return size;
}

/*********************************************************************************************************
** Function name:       rhb_serial_control
** Descriptions:        ���ڿ��ƣ����������ò����ʣ����ݸ�ʽ��
** input parameters:    dev: 	��������������
**                      cmd:	��������
**                      args: 	���ò���
** output parameters:   NONE
** Returned value:      NONE
**  ע�⣺�Է����жϵ��жϲ���������������һ�������ж��Ƿ�ʹ�ܣ�һ���Ƿ��з����жϵ������������DMA���ͣ�
**  �ڴ����жϲ���Ҫʹ�ܣ������жϵĴ���Ҳ�����ڲ���Ҫִ�С�
*********************************************************************************************************/
static rt_err_t rhb_serial_control (rt_device_t dev, int cmd, void *args)
{
  rt_err_t err_code = RT_EOK;
  rt_base_t level;
  stm32_serial_DMA_info *serial_handle = dev->user_data;
  
  RT_ASSERT(dev != RT_NULL);
  
  switch (cmd)
  {
  case RT_DEVICE_CTRL_SUSPEND:
    {
      /* suspend device */
      dev->flag |= RT_DEVICE_FLAG_SUSPENDED;
      UsartDisable(serial_handle->base);
      break;
    }
    
  case RT_DEVICE_CTRL_RESUME:
    {
      /* resume device */
      dev->flag &= ~RT_DEVICE_FLAG_SUSPENDED;
      UsartEnable(serial_handle->base);
      break;
    }
  case RT_DEVICE_CTRL_CONFIG_SET:
    {
      struct serial_configure *pconfig = args;
      rt_uint32_t u32Config = 0;
      
      if(args == RT_NULL) {
        err_code = RT_ERROR;
        break;
      }
      // ��ʼ����
      if(pconfig->parity == PARITY_NONE) {
        u32Config = USART_WLEN_8 | USART_PAR_NONE;
      } else {
        if(pconfig->data_bits == DATA_BITS_7) {
          u32Config = USART_WLEN_8;
        } else if(pconfig->data_bits == DATA_BITS_8) {
          u32Config = USART_WLEN_9;
        } else {
          err_code = RT_ERROR;
          break;
        }
        
        if(pconfig->parity == PARITY_ODD) {
          u32Config |= USART_PAR_ODD;
        } else if(pconfig->parity == PARITY_EVEN){
          u32Config |= USART_PAR_EVEN;
        } else {
          err_code = RT_ERROR;
          break;
        }
      }
      
      if(pconfig->stop_bits == STOP_BITS_1) {
        u32Config |= USART_STOP_1;
      } else if(pconfig->stop_bits == STOP_BITS_2) {
        u32Config |= USART_STOP_2;
      } else {
        err_code = RT_ERROR;
        break;
      }
      
      /* disable interrupt */
      level = rt_hw_interrupt_disable();
      UsartDisable(serial_handle->base);
      rt_uint32_t u32Clock;
      // ��ȡ����ʱ��
      u32Clock = SystemPeripheralClockGet(serial_handle->u16WhichBus, serial_handle->u32Ctrl);
      UsartConfigSet(serial_handle->base, u32Clock, pconfig->baud_rate, u32Config);
      UsartIntEnable(serial_handle->base,USART_INT_PE | USART_INT_EIE);
      UsartEnable(serial_handle->base);
      serial_handle->serial_Config = *pconfig;
      /* enable interrupt */
      rt_hw_interrupt_enable(level);
      break;
    }
  case RT_DEVICE_CTRL_CONFIG_GET:
    {
      struct serial_configure *pconfig = args;
      
      if(args == RT_NULL) {
        err_code = RT_ERROR;
        break;
      }
      
      /* disable interrupt */
      level = rt_hw_interrupt_disable();
      *pconfig = serial_handle->serial_Config;
      /* enable interrupt */
      rt_hw_interrupt_enable(level);
      break;
    }
  case RT_DEVICE_CRTL_TIMEOUT:
    {
      rt_uint32_t count = *((rt_uint32_t *)args);
      rt_uint32_t temp;
      
      if(args == RT_NULL) {
        err_code = RT_ERROR;
        break;
      }
      
      // ��msת��Ϊʱ��tick
      temp = 1000/RT_TICK_PER_SECOND;
      count = (count % temp)?((count / temp) + 1):(count / temp);
      /* disable interrupt */
      level = rt_hw_interrupt_disable();
      serial_handle->RecvTimeoutSave = count;
      /* enable interrupt */
      rt_hw_interrupt_enable(level);
      break;
    }
  case RT_DEVICE_CTRL_INTERVAL:
   {
      rt_uint32_t count = *((rt_uint32_t *)args);
      rt_uint32_t temp;
      
      if(args == RT_NULL) {
        err_code = RT_ERROR;
        break;
      }
      
      // ��msת��Ϊʱ��tick
      temp = 1000/RT_TICK_PER_SECOND;
      count = (count % temp)?((count / temp) + 1):(count / temp);
      /* disable interrupt */
      level = rt_hw_interrupt_disable();
      serial_handle->RecvIntervalTimeSave = count;
      /* enable interrupt */
      rt_hw_interrupt_enable(level);
      break;
    }
  default:
    err_code = RT_ERROR;
    break;
  }
  
  return err_code;
}

/*********************************************************************************************************
** Function name:       rhb_serial_all_dma_ISR
** Descriptions:        �����жϴ��������ú�����Ҫ���ж������
** input parameters:    handle���������Խṹ��
** output parameters:   NONE
** Returned value:      NONE
**  ע�⣺�Է����жϵ��жϲ���������������һ�������ж��Ƿ�ʹ�ܣ�һ���Ƿ��з����жϵ������������DMA���ͣ�
**  �ڴ����жϲ���Ҫʹ�ܣ������жϵĴ���Ҳ�����ڲ���Ҫִ�С�
*********************************************************************************************************/
static void rhb_serial_all_dma_ISR(stm32_serial_DMA_info *handle)
{
  rt_uint32_t u32Temp = 0;
  rt_base_t level;
  stm32_serial_DMA_info *serial_handle = handle;
  
  u32Temp = u32Temp;
  
  if(HWREG32(serial_handle->base + USART_ISR) & 0x0001) {             //  �����У�����
     HWREG32(serial_handle->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
  
  if(HWREG32(serial_handle->base + USART_ISR) & 0x0002) {             //  �����֡����
     HWREG32(serial_handle->base + USART_ICR) = 0xFFFFFFFF;
     return;
  }
    
  
  if(!(HWREG32(serial_handle->base + USART_CR1) & USART_INT_RX))  // ��������ж�δʹ��
  {
    HWREG32(serial_handle->base + USART_ICR) = 0xFFFFFFFF;
    return;
  }
  
  if(HWREG32(serial_handle->base + USART_ISR) & 0x0020)              //  ����ǽ����ж�
  {
    // ��������˽��ճ�ʱ������ʱֵ��ֵ���Ժ���Ҫ��Ҫ
    if(serial_handle->RecvTimeoutSave > 0) {
      /* disable interrupt */
      level = rt_hw_interrupt_disable();
      serial_handle->RecvTimeout = serial_handle->RecvTimeoutSave;
      /* enable interrupt */
      rt_hw_interrupt_enable(level);
    }
    
    if(!Serial_Buffer_Full(serial_handle))                //  ���FIFO����
    {
      serial_handle->buf[serial_handle->buf_write] = (rt_uint8_t)(HWREG32(serial_handle->base + USART_RDR) & 0x01FF);
      serial_handle->buf_write = (serial_handle->buf_write + 1) % Serial_RxBuf_Size;
    } else {
      u32Temp = (HWREG32(serial_handle->base + USART_RDR) & 0x01FF);   // �������������
    }
  }
  
  if(HWREG32(serial_handle->base + USART_ISR) & 0x0008)              //  ��������ݹ�����
  {
    u32Temp = (HWREG32(serial_handle->base + USART_RDR) & 0x01FF);   // �������������
    HWREG32(serial_handle->base + USART_ICR) = 0xFFFFFFFF;
  }
}

#ifdef RT_USING_ALL_DMA_UART1
static void Usart1_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_all_dma_ISR(&tStm32Usart1Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART2
static void Usart2_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_all_dma_ISR(&tStm32Usart2Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART3
static void Usart3_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_all_dma_ISR(&tStm32Usart3Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART4
static void Usart4_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_all_dma_ISR(&tStm32Usart4Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART5
static void Usart5_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_all_dma_ISR(&tStm32Usart5Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART6
static void Usart6_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_all_dma_ISR(&tStm32Usart6Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART7
static void Usart7_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_all_dma_ISR(&tStm32Usart7Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART8
static void Usart8_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_all_dma_ISR(&tStm32Usart8Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
/*********************************************************************************************************
** Function name:       rhb_serial_tx_dma_ISR
** Descriptions:        DMA�����жϴ���
** input parameters:    handle���������Խṹ��
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void rhb_serial_tx_dma_ISR(stm32_serial_DMA_info *handle)
{
  rt_uint32_t u32Status;
  stm32_serial_DMA_info *serial_handle = handle;
    
  u32Status = DMAIntStatus(serial_handle->TxDMA_base, serial_handle->TxDMA_Stream);
  DMAIntClear(serial_handle->TxDMA_base, serial_handle->TxDMA_Stream, u32Status);
  
  rt_completion_done(&serial_handle->Tx_Comp);
}

#ifdef RT_USING_ALL_DMA_UART1
static void Usart1_TX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_tx_dma_ISR(&tStm32Usart1Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART2
static void Usart2_TX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_tx_dma_ISR(&tStm32Usart2Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART3
static void Usart3_TX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_tx_dma_ISR(&tStm32Usart3Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART4
static void Usart4_TX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_tx_dma_ISR(&tStm32Usart4Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART5
static void Usart5_TX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_tx_dma_ISR(&tStm32Usart5Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART6
static void Usart6_TX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_tx_dma_ISR(&tStm32Usart6Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART7
static void Usart7_TX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_tx_dma_ISR(&tStm32Usart7Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART8
static void Usart8_TX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_tx_dma_ISR(&tStm32Usart8Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
/*********************************************************************************************************
** Function name:       rhb_serial_rx_dma_ISR
** Descriptions:        DMA�����жϴ���
** input parameters:    handle���������Խṹ��
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void rhb_serial_rx_dma_ISR(stm32_serial_DMA_info *handle)
{
  rt_uint32_t u32Status;
  stm32_serial_DMA_info *serial_handle = handle;
  
  u32Status = DMAIntStatus(serial_handle->RxDMA_base, serial_handle->RxDMA_Stream);
  DMAIntClear(serial_handle->RxDMA_base, serial_handle->RxDMA_Stream, u32Status);
  
  UsartDMADisable(serial_handle->base,USART_DMA_RX);
  UsartIntEnable(serial_handle->base,USART_INT_RX);
  //DMADisable(serial_handle->RxDMA_base,serial_handle->RxDMA_Stream);
  serial_handle->rx_len += serial_handle->rx_dma_value;
  rt_completion_done(&serial_handle->Rx_Comp);
}

#ifdef RT_USING_ALL_DMA_UART1
static void Usart1_RX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_rx_dma_ISR(&tStm32Usart1Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART2
static void Usart2_RX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_rx_dma_ISR(&tStm32Usart2Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART3
static void Usart3_RX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_rx_dma_ISR(&tStm32Usart3Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART4
static void Usart4_RX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_rx_dma_ISR(&tStm32Usart4Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART5
static void Usart5_RX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_rx_dma_ISR(&tStm32Usart5Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART6
static void Usart6_RX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_rx_dma_ISR(&tStm32Usart6Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART7
static void Usart7_RX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_rx_dma_ISR(&tStm32Usart7Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif
#ifdef RT_USING_ALL_DMA_UART8
static void Usart8_RX_DMA_ISR(void)
{
  /* enter interrupt */
  rt_interrupt_enter();
  
  rhb_serial_rx_dma_ISR(&tStm32Usart8Info);
  
  /* leave interrupt */
  rt_interrupt_leave();
}
#endif

/*********************************************************************************************************
** ���������ṹ��֧���������豸������
*********************************************************************************************************/
#ifdef RT_USING_ALL_DMA_UART1
static struct rt_device __gUart1Driver;
#endif
#ifdef RT_USING_ALL_DMA_UART2
static struct rt_device __gUart2Driver;
#endif
#ifdef RT_USING_ALL_DMA_UART3
static struct rt_device __gUart3Driver;
#endif
#ifdef RT_USING_ALL_DMA_UART4
static struct rt_device __gUart4Driver;
#endif
#ifdef RT_USING_ALL_DMA_UART5
static struct rt_device __gUart5Driver;
#endif
#ifdef RT_USING_ALL_DMA_UART6
static struct rt_device __gUart6Driver;
#endif
#ifdef RT_USING_ALL_DMA_UART7
static struct rt_device __gUart7Driver;
#endif
#ifdef RT_USING_ALL_DMA_UART8
static struct rt_device __gUart8Driver;
#endif

#endif
/*********************************************************************************************************
** Function name:       rt_usart_dma_driver_init
** Descriptions:        ����������ʼ����������Ҫ����ע���ж�
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int rt_usart_dma_driver_init(void)
{
#if defined(RT_USING_ALL_DMA_UART1) || defined(RT_USING_ALL_DMA_UART2) || defined(RT_USING_ALL_DMA_UART3) || \
defined(RT_USING_ALL_DMA_UART4) || defined(RT_USING_ALL_DMA_UART5) || defined(RT_USING_ALL_DMA_UART6) || \
defined(RT_USING_ALL_DMA_UART7) || defined(RT_USING_ALL_DMA_UART8)
  rt_device_t device;
#endif  

#ifdef RT_USING_ALL_DMA_UART1   //"ttys0"  �������RS232
  //  ʹ�õ�DMAͨ��ѡ��
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART1_TX_DMA_USE_NO);
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART1_RX_DMA_USE_NO);
 
  // ע�ᴮ���жϺ���
  hal_int_register(UART1_INT_NO, Usart1_ISR);
  hal_int_priority_set(UART1_INT_NO,0x00);
  hal_int_enable(UART1_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART1_TX_DMA_INT_NO, Usart1_TX_DMA_ISR);
  hal_int_priority_set(UART1_TX_DMA_INT_NO,0x00);
  hal_int_enable(UART1_TX_DMA_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART1_RX_DMA_INT_NO, Usart1_RX_DMA_ISR);
  hal_int_priority_set(UART1_RX_DMA_INT_NO,0x00);
  hal_int_enable(UART1_RX_DMA_INT_NO);
  
  // ע������
  device = &__gUart1Driver;
  device->type 		= RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_serial_DMA_init;
  device->open		= rhb_serial_DMA_open;
  device->close		= rhb_serial_DMA_close;
  device->read 		= rhb_serial_DMA_read;
  device->write 		= rhb_serial_DMA_write;
  device->control 	= rhb_serial_control;
  device->user_data	= (void *)&tStm32Usart1Info;
  
  /* register a character device */
  rt_device_register(device, "ttyS0", RT_DEVICE_FLAG_RDWR);
#endif
  
#ifdef RT_USING_ALL_DMA_UART2
  //  ʹ�õ�DMAͨ��ѡ��
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART2_TX_DMA_USE_NO);
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART2_RX_DMA_USE_NO);
  
  // ע�ᴮ���жϺ���
  hal_int_register(UART2_INT_NO, Usart2_ISR);
  hal_int_priority_set(UART2_INT_NO,0x00);
  hal_int_enable(UART2_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART2_TX_DMA_INT_NO, Usart2_TX_DMA_ISR);
  hal_int_priority_set(UART2_TX_DMA_INT_NO,0x00);
  hal_int_enable(UART2_TX_DMA_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART2_RX_DMA_INT_NO, Usart2_RX_DMA_ISR);
  hal_int_priority_set(UART2_RX_DMA_INT_NO,0x00);
  hal_int_enable(UART2_RX_DMA_INT_NO);
  
  // ע������
  device = &__gUart2Driver;
  device->type 		= RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_serial_DMA_init;
  device->open		= rhb_serial_DMA_open;
  device->close		= rhb_serial_DMA_close;
  device->read 		= rhb_serial_DMA_read;
  device->write     = rhb_serial_DMA_write;
  device->control 	= rhb_serial_control;
  device->user_data	= &tStm32Usart2Info;
  
  /* register a character device */
  rt_device_register(device, "ttyS1", RT_DEVICE_FLAG_RDWR);
#endif
  
#ifdef RT_USING_ALL_DMA_UART3
  //  ʹ�õ�DMAͨ��ѡ��
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART3_TX_DMA_USE_NO);
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART3_RX_DMA_USE_NO);
  
  // ע�ᴮ���жϺ���
  hal_int_register(UART3_INT_NO, Usart3_ISR);
  hal_int_priority_set(UART3_INT_NO,0x00);
  hal_int_enable(UART3_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART3_TX_DMA_INT_NO, Usart3_TX_DMA_ISR);
  hal_int_priority_set(UART3_TX_DMA_INT_NO,0x00);
  hal_int_enable(UART3_TX_DMA_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART3_RX_DMA_INT_NO, Usart3_RX_DMA_ISR);
  hal_int_priority_set(UART3_RX_DMA_INT_NO,0x00);
  hal_int_enable(UART3_RX_DMA_INT_NO);
  
  // ע������
  device = &__gUart3Driver;
  device->type 		= RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_serial_DMA_init;
  device->open		= rhb_serial_DMA_open;
  device->close		= rhb_serial_DMA_close;
  device->read 		= rhb_serial_DMA_read;
  device->write 	= rhb_serial_DMA_write;
  device->control 	= rhb_serial_control;
  device->user_data	= &tStm32Usart3Info;
  
  /* register a character device */
  rt_device_register(device, "ttyS2", RT_DEVICE_FLAG_RDWR);
#endif
  
#ifdef RT_USING_ALL_DMA_UART4
  //  ʹ�õ�DMAͨ��ѡ��
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART4_TX_DMA_USE_NO);
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART4_RX_DMA_USE_NO);
  
  // ע�ᴮ���жϺ���
  hal_int_register(UART4_INT_NO, Usart4_ISR);
  hal_int_priority_set(UART4_INT_NO,0x00);
  hal_int_enable(UART4_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART4_TX_DMA_INT_NO, Usart4_TX_DMA_ISR);
  hal_int_priority_set(UART4_TX_DMA_INT_NO,0x00);
  hal_int_enable(UART4_TX_DMA_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART4_RX_DMA_INT_NO, Usart4_RX_DMA_ISR);
  hal_int_priority_set(UART4_RX_DMA_INT_NO,0x00);
  hal_int_enable(UART4_RX_DMA_INT_NO);
  
  // ע������
  device = &__gUart4Driver;
  device->type 		= RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_serial_DMA_init;
  device->open		= rhb_serial_DMA_open;
  device->close		= rhb_serial_DMA_close;
  device->read 		= rhb_serial_DMA_read;
  device->write 		= rhb_serial_DMA_write;
  device->control 	= rhb_serial_control;
  device->user_data	= &tStm32Usart4Info;
  
  /* register a character device */
  rt_device_register(device, "ttyS3", RT_DEVICE_FLAG_RDWR);
#endif
  
#ifdef RT_USING_ALL_DMA_UART5
  //  ʹ�õ�DMAͨ��ѡ��
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART5_TX_DMA_USE_NO);
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART5_RX_DMA_USE_NO);
  
  // ע�ᴮ���жϺ���
  hal_int_register(UART5_INT_NO, Usart5_ISR);
  hal_int_priority_set(UART5_INT_NO,0x00);
  hal_int_enable(UART5_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART5_TX_DMA_INT_NO, Usart5_TX_DMA_ISR);
  hal_int_priority_set(UART5_TX_DMA_INT_NO,0x00);
  hal_int_enable(UART5_TX_DMA_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART5_RX_DMA_INT_NO, Usart5_RX_DMA_ISR);
  hal_int_priority_set(UART5_RX_DMA_INT_NO,0x00);
  hal_int_enable(UART5_RX_DMA_INT_NO);
  
  // ע������
  device = &__gUart5Driver;
  device->type 		= RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_serial_DMA_init;
  device->open		= rhb_serial_DMA_open;
  device->close		= rhb_serial_DMA_close;
  device->read 		= rhb_serial_DMA_read;
  device->write 		= rhb_serial_DMA_write;
  device->control 	= rhb_serial_control;
  device->user_data	= &tStm32Usart5Info;
  
  /* register a character device */
  rt_device_register(device, "ttyS4", RT_DEVICE_FLAG_RDWR);
#endif
  
#ifdef RT_USING_ALL_DMA_UART6
  //  ʹ�õ�DMAͨ��ѡ��
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART6_TX_DMA_USE_NO);
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART6_RX_DMA_USE_NO);
  
  // ע�ᴮ���жϺ���
  hal_int_register(UART6_INT_NO, Usart6_ISR);
  hal_int_priority_set(UART6_INT_NO,0x00);
  hal_int_enable(UART6_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART6_TX_DMA_INT_NO, Usart6_TX_DMA_ISR);
  hal_int_priority_set(UART6_TX_DMA_INT_NO,0x00);
  hal_int_enable(UART6_TX_DMA_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART6_RX_DMA_INT_NO, Usart6_RX_DMA_ISR);
  hal_int_priority_set(UART6_RX_DMA_INT_NO,0x00);
  hal_int_enable(UART6_RX_DMA_INT_NO);
  
  // ע������
  device = &__gUart6Driver;
  device->type 		= RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_serial_DMA_init;
  device->open		= rhb_serial_DMA_open;
  device->close		= rhb_serial_DMA_close;
  device->read 		= rhb_serial_DMA_read;
  device->write         = rhb_serial_DMA_write;
  device->control 	= rhb_serial_control;
  device->user_data	= &tStm32Usart6Info;
  
  /* register a character device */
  rt_device_register(device, "ttyS5", RT_DEVICE_FLAG_RDWR);
#endif
  
#ifdef RT_USING_ALL_DMA_UART7
  //  ʹ�õ�DMAͨ��ѡ��
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART7_TX_DMA_USE_NO);
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART7_RX_DMA_USE_NO);
  
  // ע�ᴮ���жϺ���
  hal_int_register(UART7_INT_NO, Usart7_ISR);
  hal_int_priority_set(UART7_INT_NO,0x00);
  hal_int_enable(UART7_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART7_TX_DMA_INT_NO, Usart7_TX_DMA_ISR);
  hal_int_priority_set(UART7_TX_DMA_INT_NO,0x00);
  hal_int_enable(UART7_TX_DMA_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART7_RX_DMA_INT_NO, Usart7_RX_DMA_ISR);
  hal_int_priority_set(UART7_RX_DMA_INT_NO,0x00);
  hal_int_enable(UART7_RX_DMA_INT_NO);
  
  // ע������
  device = &__gUart7Driver;
  device->type 		= RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_serial_DMA_init;
  device->open		= rhb_serial_DMA_open;
  device->close		= rhb_serial_DMA_close;
  device->read 		= rhb_serial_DMA_read;
  device->write         = rhb_serial_DMA_write;
  device->control 	= rhb_serial_control;
  device->user_data	= &tStm32Usart7Info;
  
  /* register a character device */
  rt_device_register(device, "ttyS6", RT_DEVICE_FLAG_RDWR);
#endif
  
#ifdef RT_USING_ALL_DMA_UART8
  //  ʹ�õ�DMAͨ��ѡ��
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART8_TX_DMA_USE_NO);
  DMAMuxRequestIDSet(DMAMUX1_BASE, UART8_RX_DMA_USE_NO);
  
  // ע�ᴮ���жϺ���
  hal_int_register(UART8_INT_NO, Usart8_ISR);
  hal_int_priority_set(UART8_INT_NO,0x00);
  hal_int_enable(UART8_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART8_TX_DMA_INT_NO, Usart8_TX_DMA_ISR);
  hal_int_priority_set(UART8_TX_DMA_INT_NO,0x00);
  hal_int_enable(UART8_TX_DMA_INT_NO);
  
  // ע�ᴮ��DMA�����жϺ���
  hal_int_register(UART8_RX_DMA_INT_NO, Usart8_RX_DMA_ISR);
  hal_int_priority_set(UART8_RX_DMA_INT_NO,0x00);
  hal_int_enable(UART8_RX_DMA_INT_NO);
  
  // ע������
  device = &__gUart8Driver;
  device->type 		= RT_Device_Class_Char;
  device->rx_indicate = RT_NULL;
  device->tx_complete = RT_NULL;
  device->init 		= rhb_serial_DMA_init;
  device->open		= rhb_serial_DMA_open;
  device->close		= rhb_serial_DMA_close;
  device->read 		= rhb_serial_DMA_read;
  device->write         = rhb_serial_DMA_write;
  device->control 	= rhb_serial_control;
  device->user_data	= &tStm32Usart8Info;
  
  /* register a character device */
  rt_device_register(device, "ttyS7", RT_DEVICE_FLAG_RDWR);
#endif
  
  return 0;
}

/*********************************************************************************************************
** �����Զ���ʼ������
*********************************************************************************************************/
INIT_DEVICE_EXPORT(rt_usart_dma_driver_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
