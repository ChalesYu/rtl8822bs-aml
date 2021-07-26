/****************************************Copyright (c)****************************************************
**                           湖 南 国 科 微 电 子 股 份 有 限 公 司
**                                http://www.gokemicro.com
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           mt7668.c
** Last modified Date:  2020-01-07
** Last Version:        V1.00
** Description:         MT7668驱动注册
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
#include <rtthread.h>
#include <rtdevice.h>

#include "./../os/wlan_rtthread.h"
/*********************************************************************************************************
** Function name:       mt7668_probe
** Descriptions:        mt7668驱动枚举到回调函数
** Input parameters:    card：SDIO卡结构
** Output parameters:   NONE
** Returned value:      错误码
*********************************************************************************************************/
static rt_int32_t mt7668_probe(struct rt_sdio_function *func, struct rt_sdio_device_id *id)
{
  rt_kprintf("Found mt7668 wlan card.\n");
  
  sdio_claim_host(func);
  sdio_enable_func(func);
  sdio_release_host(func);
  
  rt_wifi_probe(func);
  return 0;
}

/*********************************************************************************************************
** Function name:       mt7668_remove
** Descriptions:        mt7668驱动移除回调函数
** Input parameters:    card：SDIO卡结构
** Output parameters:   NONE
** Returned value:      错误码
*********************************************************************************************************/
static rt_int32_t mt7668_remove(struct rt_sdio_function *func)
{
  sdio_claim_host(func);
  sdio_disable_func(func);
  sdio_release_host(func);
  rt_wifi_remove();
  return 0;
}

/*********************************************************************************************************
** mt7668 sdio驱动结构定义
*********************************************************************************************************/
struct rt_sdio_device_id mt7668_id =
{
    SDIO_FUNC_CODE_WLAN,
    0x037A,    // 厂商ID
    0x7608,    // 芯片id
};
struct rt_sdio_driver mt7668_sdio_driver =
{
    "mt7668",
    mt7668_probe,
    mt7668_remove,
    &mt7668_id
};

/*********************************************************************************************************
** Function name:       mt7668_sdio_init
** Descriptions:        mt7668驱动注册
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      错误码
*********************************************************************************************************/
int mt7668_sdio_init(void)
{
  sdio_register_driver(&mt7668_sdio_driver);
  
  return 0;
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_DEVICE_EXPORT(mt7668_sdio_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/

