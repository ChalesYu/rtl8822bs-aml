/****************************************Copyright (c)****************************************************
**                           湖 南 国 科 微 电 子 股 份 有 限 公 司
**                                http://www.gokemicro.com
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           gk_sdio.h
** Last modified Date:  2020-01-07
** Last Version:        V1.00
** Description:         驱动sdio接口实现
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2020-01-07
** Version:             V1.00
** Descriptions:
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
*********************************************************************************************************/
#ifndef __GK_SDIO_H__
#define __GK_SDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>
#include "./../os/wlan_rtthread.h"
#include "./../os/gl_typedef.h"
  
/*********************************************************************************************************
**  外部函数声明
*********************************************************************************************************/
extern void glSetHifInfo(gk_wlan_info *prwlan_info, uint32_t u32Cookie);
extern rt_bool_t glBusInit(void *pvData);
extern int glBusSetIrq(gk_wlan_info *prwlan_info, void *pfisr);
extern void glBusFreeIrq(gk_wlan_info *prwlan_info);
extern rt_bool_t glIsReadClearReg(uint32_t u32Address);
extern rt_bool_t kalDevRegRead(gk_wlan_info *prwlan_info, uint32_t u32Register, uint32_t *pu32Value);
extern rt_bool_t kalDevRegWrite(gk_wlan_info *prwlan_info, uint32_t u32Register, uint32_t u32Value);
extern rt_bool_t kalDevRegRead_mac(gk_wlan_info *prwlan_info, uint32_t u32Register, uint32_t *pu32Value);
extern rt_bool_t kalDevRegWrite_mac(gk_wlan_info *prwlan_info, uint32_t u32Register, uint32_t u32Value);
extern rt_bool_t kalDevPortRead(gk_wlan_info *prwlan_info,
	       uint16_t u16Port, uint32_t u32Len,uint8_t *pu8Buf, uint32_t u32ValidOutBufSize);
extern rt_bool_t kalDevPortWrite(gk_wlan_info *prwlan_info,
		uint16_t u16Port, uint32_t u32Len, uint8_t *pu8Buf, uint32_t u32ValidInBufSize);
extern void kalDevReadIntStatus(gk_wlan_info *prwlan_info, uint32_t *pu32IntStatus);
extern rt_bool_t kalDevWriteWithSdioCmd52(gk_wlan_info *prwlan_info, uint32_t u32Addr, uint8_t u8Data);


#ifdef __cplusplus
    }
#endif      // __cplusplus
    
#endif      // __GK_SDIO_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
