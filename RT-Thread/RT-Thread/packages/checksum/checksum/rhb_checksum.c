/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info--------------------------------------------------------------------------------

** File Name:           rhb_CRC.c
** Last modified Date:  2010-8-3
** Last Version:        v1.0
** Description:         软件CRC校验算法函数声明头文件
** 
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2010-8-8
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
#include "rhb_checksum.h"

/*********************************************************************************************************
** Function name:       rhb_checksum8
** Descriptions:        checksum8的校验
** input parameters:    pucMsg：指定数据起始地址
**                      ulLen： 指定数据长度
**                      u8Starter: 计算器的初值，当一次检验所有的数据时，设置为0
** output parameters:   NONE
** Returned value:      checksum8校验结果
*********************************************************************************************************/
rt_uint8_t rhb_checksum8(rt_uint8_t *pu8Msg, rt_uint32_t u32Len, rt_uint8_t u8Starter)
{
    rt_uint32_t i;
    
    for(i=0; i<u32Len; i++)
    {
        u8Starter += *pu8Msg++;
    }

    return u8Starter;
}

/*********************************************************************************************************
** Function name:       rhb_checksum16
** Descriptions:        checksum16的校验
** input parameters:    pucMsg：指定数据起始地址
**                      ulLen： 指定数据长度
**                      u16Starter: 计算器的初值，当一次检验所有的数据时，设置为0
** output parameters:   NONE
** Returned value:      checksum8校验结果
*********************************************************************************************************/
rt_uint16_t rhb_checksum16(rt_uint8_t *pu8Msg, rt_uint32_t u32Len, rt_uint16_t u16Starter)
{
    rt_uint32_t i;
    
    for(i=0; i<u32Len; i++)
    {
        u16Starter += *pu8Msg++;
    }

    return u16Starter;
}
/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/
