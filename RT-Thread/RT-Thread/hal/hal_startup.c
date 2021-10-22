/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           hal_startup.c
** Last modified Date:  2015-05-12
** Last Version:        V1.00
** Description:         ��������ʵ��
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-05-12
** Version:             V1.00
** Descriptions:
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
*********************************************************************************************************/

/*********************************************************************************************************
    Enable the IAR extensions for this source file.
*********************************************************************************************************/
#pragma language=extended

#include "rtdef.h"
/*********************************************************************************************************
    ��������
*********************************************************************************************************/
static void NmiISR(void);
static void FaultISR(void);
static void IntDefaultHandler(void);
void PendSV_Handler(void);

/*********************************************************************************************************
    The entry point for the application startup code.
*********************************************************************************************************/
extern void __iar_program_start(void);


/*********************************************************************************************************
 A union that describes the entries of the vector table.  The union is needed
 since the first entry is the stack pointer and the remainder are function
 pointers.
*********************************************************************************************************/
typedef union
{
    void (*pfnHandler)(void);
    void * __ptr;
}
uVectorEntry;

#pragma segment="CSTACK"


/*********************************************************************************************************
    ��λʱ���ж�������Ӧ�÷���Ƭ��Flash����ʼ��ַ
*********************************************************************************************************/
__root const uVectorEntry __vector_table[] @ ".intvec" =
{
    { .__ptr = __sfe( "CSTACK" ) },         //  The initial stack pointer
    __iar_program_start,                    //  The reset handler
    NmiISR,                                  // The NMI handler
    FaultISR,                               // The hard fault handler
    IntDefaultHandler,                      // The MPU fault handler
    IntDefaultHandler,                      // The bus fault handler
    IntDefaultHandler,                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // SVCall handler
    IntDefaultHandler,                      // Debug monitor handler
    0,                                      // Reserved
    PendSV_Handler,                         // The PendSV handler
    IntDefaultHandler,                      // The SysTick handler
};

/*********************************************************************************************************
** Function name:       NmiSR
** Descriptions:        Nmi�жϷ�������������NMI�쳣ʱ��ִ�иú���
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void
NmiISR(void)
{
    /*
    ** ֱ�ӽ���һ����ѭ��
    */
    while(1)
    {
    }
}

/*********************************************************************************************************
** Function name:       FaultISR
** Descriptions:        Fault�жϷ�������������Fault�쳣ʱ��ִ�иú���
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
extern void HardFault_Handler();
static void
FaultISR(void)
{
    /*
    ** ֱ�ӽ���һ����ѭ��
    */
  //  HardFault_Handler();
//    list_if();
//    list_thread();
//    cmd_netstat();
    while(1)
    {
    }
}

/*********************************************************************************************************
** Function name:       IntDefaultHandler
** Descriptions:        Ĭ���жϷ���������δע���жϷ�����ʱ��ִ�иú���
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void
IntDefaultHandler(void)
{
    /*
    ** ֱ�ӽ���һ����ѭ��
    */
    while(1)
    {
    }
}
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
