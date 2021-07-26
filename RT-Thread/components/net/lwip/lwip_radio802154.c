/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwip_radio802154.c
** Last modified Date:  2015-01-09
** Last Version:        v1.00
** Description:         lwip的IEEE 802.15.4的射频驱动应用框架
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-01-09
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

#ifdef RT_USE_6LOWPAN

#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"
#include "lwip/ethip6.h"

/*********************************************************************************************************
  6LoWPAN的支持必须使能IPv6
*********************************************************************************************************/
#if LWIP_IPV6

#include "netif/radio/csma_mac.h"
#include "netif/radio/null_mac.h"
#include "netif/radio/null_rdc.h"
#include "netif/radio/radio_driver.h"
#include "netif/radio/lowpan_compress.h"
#include "netif/radio/lowpan_if.h"
#include "netif/rpl/rpl_if.h"
#include "lwip_if.h"
#include "lwip_radio802154.h"

/*********************************************************************************************************
**  如果需要接收线程，则配置接收线程相关参数
*********************************************************************************************************/
static struct rt_mailbox lowpan_rx_thread_mb;
static struct rt_thread lowpan_rx_thread;
#ifndef RT_LWIP_6LOWPANTHREAD_PRIORITY
#define RT_6LOWPANIF_THREAD_PREORITY		0x90
static char lowpan_rx_thread_mb_pool[48 * 4];
static char lowpan_rx_thread_stack[512];
#else
#define RT_6LOWPANIF_THREAD_PREORITY		RT_LWIP_6LOWPANTHREAD_PRIORITY
static char lowpan_rx_thread_mb_pool[RT_LWIP_6LOWPANTHREAD_MBOX_SIZE * 4];
static char lowpan_rx_thread_stack[RT_LWIP_6LOWPANTHREAD_STACKSIZE];
#endif

/*********************************************************************************************************
** Function name:       lowpan_device_init
** Descriptions:        6LoWPAN网络设备初始化
** input parameters:    dev:	6LoWPAN驱动结构
** 			name:	设备名字
** output parameters:   无
** Returned value:      初始化结果
*********************************************************************************************************/
rt_err_t lowpan_device_init(struct lowpan_device * dev, const char *name)
{
    dev->parent.type = RT_Device_Class_NetIf;
    /* register to RT-Thread device manager */
    rt_device_register(&(dev->parent), name, RT_DEVICE_FLAG_RDWR);
    if (rt_device_init((rt_device_t)dev) != RT_EOK)
    {
    	return ERR_IF;
    }

#if LWIP_RPL && LWIP_IPV4
    dev->lowpanif.netif.output = NULL;
#endif
    /* if tcp thread has been started up, we add this netif to the system */
    if (rt_thread_find("tcpip") != RT_NULL)
    {
#if LWIP_IPV4
        ip4_addr_t ipaddr, netmask, gw;

        IP4_ADDR(&ipaddr, 127, 0, 0, 1);
        IP4_ADDR(&netmask, 255, 0, 0, 0);
        IP4_ADDR(&gw, 127, 0, 0, 0);

        lowpan_netif_add(&(dev->lowpanif), netif_add, &ipaddr, &netmask, &gw);
#else
        lowpan_netif_add(&(dev->lowpanif), netif_add);
#endif
    }

    /* set name */
    dev->lowpanif.netif.name[0] = name[0];
    dev->lowpanif.netif.name[1] = name[1];

#if LWIP_RPL
    dev->lowpanif.netif.output_ip6 = rpl_lowpan_netif_output6;
#endif

    lowpan_netif_set_up(&dev->lowpanif);

#if LWIP_RPL
    netif_add_in_rpl(&(dev->lowpanif.netif));
#endif
    return RT_EOK;
}

/*********************************************************************************************************
** Function name:       lowpan_device_ready
** Descriptions:        6LoWPAN收到数据发送邮箱
** input parameters:    dev:	6LoWPAN驱动结构
** output parameters:   无
** Returned value:      准备结果
*********************************************************************************************************/
rt_err_t lowpan_device_ready(struct lowpan_device* dev)
{
	return rt_mb_send(&lowpan_rx_thread_mb, (rt_uint32_t)dev);
}

/*********************************************************************************************************
** Function name:       eth_rx_thread_entry
** Descriptions:        Ethernet Rx Thread
** input parameters:    parameter:	线程传递参数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
static void lowpan_rx_thread_entry(void* parameter)
{
    struct lowpan_device* device;
    struct lowpanif *lowpanif;

    while (1)
    {
        if (rt_mb_recv(&lowpan_rx_thread_mb, (rt_uint32_t*)&device, RT_WAITING_FOREVER) == RT_EOK)
        {
            struct pbuf *p;
            lowpanif = &device->lowpanif;

            p = lowpanif->radio_driver->read(lowpanif);
            if(p != NULL) {
            	RDC_DRIVER_INPUT(lowpanif, p);
            }
        }
        else
        {
            LWIP_ASSERT("Should not happen!\n",0);
        }
    }
}

/*********************************************************************************************************
** Function name:       lowpan_system_device_init
** Descriptions:        6LoWPAN设备初始化，初始化后，开始支持以太网设备
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int lowpan_system_device_init(void)
{
    rt_err_t result;

    /* initialize mailbox and create Ethernet Rx thread */
    result = rt_mb_init(&lowpan_rx_thread_mb, "panmb",
                        &lowpan_rx_thread_mb_pool[0], sizeof(lowpan_rx_thread_mb_pool)/4,
                        RT_IPC_FLAG_FIFO);
    RT_ASSERT(result == RT_EOK);

    result = rt_thread_init(&lowpan_rx_thread, "panrx", lowpan_rx_thread_entry, RT_NULL,
                            &lowpan_rx_thread_stack[0], sizeof(lowpan_rx_thread_stack),
                            RT_LWIP_6LOWPANTHREAD_PRIORITY, 16);
    RT_ASSERT(result == RT_EOK);
    result = rt_thread_startup(&lowpan_rx_thread);
    RT_ASSERT(result == RT_EOK);

    rt_kprintf("Init 6lowpan driver framework!\n");
	return (int)result;
}
INIT_DEVICE_EXPORT(lowpan_system_device_init);

#endif // End of LWIP_IPV6
#endif // End of RT_USE_6LOWPAN
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
