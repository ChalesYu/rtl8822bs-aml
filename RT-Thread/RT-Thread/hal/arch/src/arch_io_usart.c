/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_usart.c
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         usart及uart模块寄存器封装函数实现声明
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
#include "hal/arch/inc/arch_io_usart.h"


/*********************************************************************************************************
  局部变量定义
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       UsartConfigSet
** Descriptions:        usart模块配置
** input parameters:    u32Base  usart模块基地址
**                      u32Clock 提供给usart模块的时钟,挂在AHB或者APB1或APB2，为各自总线的频率
**                      u32Baud  通信波特率,直接指定即可
**                      u32Config 配置信息，可以取下列三组值的或
**                      USART_STOP_0_5          //  0.5位停止位
**                      USART_STOP_1            //  1位停止位
**                      UASRT_STOP_1_5          //  1.5位停止位
**                      USART_STOP_2            //  2位停止位

**                      USART_WLEN_8            //  字长为8
**                      USART_WLEN_9            //  字长为9

**                      USART_PAR_NONE          //  无校验
**                      USART_PAR_EVEN          //  偶校验
**                      USART_PAR_ODD           //  奇校验
**                      
**                      USART_CTSE_ENABLE       // 流控CTS使能
**                      USART_CTSE_DISABLE      // 流控CTS禁止
**                      
**                      USART_RTSE_ENABLE       // 流控RTS使能
**                      USART_RTSE_DISABLE      // 流控RTS禁止
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartConfigSet(rt_uint32_t u32Base, rt_uint32_t u32Clock,
                    rt_uint32_t u32Baud, rt_uint32_t u32Config)
{
  rt_uint32_t u32Temp;
  
  // 设置串口时钟预分频
  HWREG32(u32Base + USART_PRESC) = 0x02;
  u32Clock /= 4;
  
  //  设置停止位，即配置寄存器CR2
  u32Temp = HWREG32(u32Base + USART_CR2);
  u32Temp &= ~((rt_uint32_t)(0x3000));
  u32Temp |= ((u32Config & 0x03) << 12);
  HWREG32(u32Base + USART_CR2) = u32Temp;
  
  //  配置寄存器CR1，即配置字长及校验
  u32Temp = HWREG32(u32Base + USART_CR1);
  u32Temp &= ~((rt_uint32_t)(0x1000170C));
  u32Temp |= u32Config;
  u32Temp |= 0x000C;    //  使能发送和接收
  HWREG32(u32Base + USART_CR1) = u32Temp;
  
  //  注意：流控控制, 配置CR3寄存器
  u32Temp = HWREG32(u32Base + USART_CR3);
  u32Temp &= ~(0x0300);
  u32Temp |= (u32Config << 4);
  HWREG32(u32Base + USART_CR3) = u32Temp;
  
  //  根据波特率计算BRR寄存器的值，并写入其中
  //  先计算整数部分
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
** Descriptions:        usart模块使能，使能后模块才能正常工作
** input parameters:    u32Base  usart模块基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + USART_CR1) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       UsartDisable
** Descriptions:        usart模块禁能，禁能后模块才能停止工作，在重新配置usart前通常先调用该函数
** input parameters:    u32Base  usart模块基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartDisable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + USART_CR1) &=  ~((rt_uint32_t)(0x0001));
}


/*********************************************************************************************************
** Function name:       UsartDMAEnable
** Descriptions:        usart模块DMA功能使能
** input parameters:    u32Base  usart模块基地址
**                      u32Flags 指定使能的DMA功能，可以取下列值
**                      USART_DMA_TX    //  DMA发送功能
**                      USART_DMA_RX    //  DMA接受功能
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartDMAEnable(rt_uint32_t u32Base,rt_uint32_t u32Flags)
{
  HWREG32(u32Base + USART_CR3) |= u32Flags;
}
/*********************************************************************************************************
** Function name:       UsartDMADisable
** Descriptions:        usart模块DMA功能禁能
** input parameters:    u32Base  usart模块基地址
**                      u32Flags 指定禁能的DMA功能，可以取下列值
**                      USART_DMA_TX    //  DMA发送功能
**                      USART_DMA_RX    //  DMA接受功能
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartDMADisable(rt_uint32_t u32Base, rt_uint32_t u32Flags)
{
  HWREG32(u32Base + USART_CR3) &= ~u32Flags;
}
/*********************************************************************************************************
** Function name:       UsartIntEnable
** Descriptions:        usart模块中断使能
** input parameters:    u32Base:     usart模块基地址
**                      u32IntFlags: 需要使能的中断, 这里仅仅列出常用的中断，其它的可以通过寄存器设置，可以
**                                  取下列值的或
**                      USART_INT_EIE               //  帧错误中断
**                      USART_INT_PE                //  校验错误
**                      USART_INT_TX_EMPTY          //  发送缓冲区空中断
**                      USART_INT_TX_FINISH         //  发送完成中断
**                      USART_INT_RX                //  接收中断使能，说明接收寄存器中有数据了
**                      USART_INT_IDLE              //  IDLE中断使能
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
** Descriptions:        usart模块中断禁能
** input parameters:    u32Base:     usart模块基地址
**                      u32IntFlags: 需要禁能的中断, 这里仅仅列出常用的中断，其它的可以通过寄存器设置，可以
**                                  取下列值的或
**                      USART_INT_EIE               //  帧错误中断
**                      USART_INT_PE                //  校验错误
**                      USART_INT_TX_EMPTY          //  发送缓冲区空中断
**                      USART_INT_TX_FINISH         //  发送完成中断
**                      USART_INT_RX                //  接收中断使能，说明接收寄存器中有数据了
**                      USART_INT_IDLE              //  IDLE中断使能
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
** Descriptions:        判断发送寄存器是否为空，即是否可以发送下一个数据
** input parameters:    u32Base  usart模块基地址
** output parameters:   NONE
** Returned value:      RT_TRUE: 可以发送下一个数据 ；RT_FALSE：不能发送下一个数据
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
** Descriptions:        发送数据
** input parameters:    u32Base  usart模块基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartCharPut(rt_uint32_t u32Base,rt_uint8_t u8Data)
{
    HWREG32(u32Base + USART_TDR) = u8Data;           //  将数据写入发送寄存器
}

/*********************************************************************************************************
** Function name:       UsartCharsAvial
** Descriptions:        判断接收寄存器中是否有可用数据
** input parameters:    u32Base  usart模块基地址
** output parameters:   NONE
** Returned value:      RT_TRUE: 接受寄存器中有可用数据，可以读出; RT_FALSE: 接收寄存器中无可用数据
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
** Descriptions:        接收数据，即从接收寄存器中读出数据
** input parameters:    u32Base  usart模块基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t  UsartCharGet(rt_uint32_t u32Base)
{
    return (HWREG32(u32Base + USART_RDR) & 0x01FF);   //  读出数据，低9为有效
}

/*********************************************************************************************************
** Function name:       UsartPutS
** Descriptions:        发送一个指定的字符串
** input parameters:    u32Base: usart模块基地址
**                      s:      指定发送的字符串
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartPutS(rt_uint32_t u32Base,const char *s)
{
    while (*s != '\0') {
       while(!(UsartSpaceAvial(u32Base))) ;          //  等待发送寄存器可用
       UsartCharPut(u32Base,*(s++));
    }
}
/*********************************************************************************************************
** Function name:       UsartPutS
** Descriptions:        发送一个指定长度的字符串
** input parameters:    u32Base: usart模块基地址
**                      s:      指定发送字符串的基地址
**                      usNum:  指定发送字符串的长度
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void UsartPutN(rt_uint32_t u32Base, const  char  *pStr , rt_uint16_t  u16Num)
{
    while(u16Num)
    {
        if(UsartSpaceAvial(u32Base))                 //  如果发送缓冲区有空间，则填充发送寄存器
        {
            UsartCharPut(u32Base,*pStr++);
            u16Num--;
        }
    }
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
