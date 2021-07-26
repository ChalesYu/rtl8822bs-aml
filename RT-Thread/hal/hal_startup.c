/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           hal_startup.c
** Last modified Date:  2015-05-12
** Last Version:        V1.00
** Description:         启动代码实现
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
    函数声明
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
    复位时的中断向量表，应该放在片内Flash的起始地址
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
** Descriptions:        Nmi中断服务函数，当发生NMI异常时，执行该函数
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void
NmiISR(void)
{
    /*
    ** 直接进入一个死循环
    */
    while(1)
    {
    }
}

/*********************************************************************************************************
** Function name:       FaultISR
** Descriptions:        Fault中断服务函数，当发生Fault异常时，执行该函数
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
extern void HardFault_Handler();
static void
FaultISR(void)
{
    /*
    ** 直接进入一个死循环
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
** Descriptions:        默认中断服务函数，当未注册中断服务函数时，执行该函数
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void
IntDefaultHandler(void)
{
    /*
    ** 直接进入一个死循环
    */
    while(1)
    {
    }
}
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
