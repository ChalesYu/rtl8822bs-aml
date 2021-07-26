/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwip_ethernet.c
** Last modified Date:  2015-01-09
** Last Version:        v1.00
** Description:         lwip的以太网驱动应用框架
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
#include <rtthread.h>

#ifdef RT_USE_ETHERNET

#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/snmp.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/ethip6.h"

#include "netif/etharp.h"
#include "lwip_if.h"
#include "lwip_ethernet.h"

/*********************************************************************************************************
**  以太网驱动特性配置
*********************************************************************************************************/
#ifdef RT_LWIP_NO_TX_THREAD
#define LWIP_NO_TX_THREAD
#endif

#ifdef RT_LWIP_NO_RX_THREAD
#define LWIP_NO_RX_THREAD
#endif

/*********************************************************************************************************
**  定义网络连接配置函数
*********************************************************************************************************/
#define netifapi_netif_set_link_up(n)      netifapi_netif_common(n, netif_set_link_up, NULL)
#define netifapi_netif_set_link_down(n)    netifapi_netif_common(n, netif_set_link_down, NULL)

/*********************************************************************************************************
**  如果需要发送线程，则配置发送线程相关参数
*********************************************************************************************************/
#ifndef LWIP_NO_TX_THREAD
/**
* Tx message structure for Ethernet interface
*/
struct eth_tx_msg
{
  struct netif *netif;
  struct pbuf 	*buf;
};

static struct rt_mailbox eth_tx_thread_mb;
static struct rt_thread eth_tx_thread;
#ifndef RT_LWIP_ETHTHREAD_PRIORITY
static char eth_tx_thread_mb_pool[32 * 4];
static char eth_tx_thread_stack[512];
#else
static char eth_tx_thread_mb_pool[RT_LWIP_ETHTHREAD_MBOX_SIZE * 4];
static char eth_tx_thread_stack[RT_LWIP_ETHTHREAD_STACKSIZE];
#endif
#endif

/*********************************************************************************************************
**  如果需要接收线程，则配置接收线程相关参数
*********************************************************************************************************/
#ifndef LWIP_NO_RX_THREAD
static struct rt_mailbox eth_rx_thread_mb;
static struct rt_thread eth_rx_thread;
#ifndef RT_LWIP_ETHTHREAD_PRIORITY
#define RT_ETHERNETIF_THREAD_PREORITY	0x90
static char eth_rx_thread_mb_pool[48 * 4];
static char eth_rx_thread_stack[512];
#else
#define RT_ETHERNETIF_THREAD_PREORITY	RT_LWIP_ETHTHREAD_PRIORITY
static char eth_rx_thread_mb_pool[RT_LWIP_ETHTHREAD_MBOX_SIZE * 4];
static char eth_rx_thread_stack[RT_LWIP_ETHTHREAD_STACKSIZE];
#endif
#endif

/*********************************************************************************************************
** Function name:       ethernetif_linkoutput
** Descriptions:        以太网发送数据帧
** input parameters:    netif:	网络接口标志
** 						p:		发送数据缓冲区
** output parameters:   无
** Returned value:      发送结果
*********************************************************************************************************/
static err_t ethernetif_linkoutput(struct netif *netif, struct pbuf *p)
{
#ifndef LWIP_NO_TX_THREAD
  struct eth_tx_msg msg;
  struct eth_device* enetif;
  
  RT_ASSERT(netif != RT_NULL);
  enetif = (struct eth_device*)netif->state;
  
  /* send a message to eth tx thread */
  msg.netif = netif;
  msg.buf   = p;
  if (rt_mb_send(&eth_tx_thread_mb, (rt_uint32_t) &msg) == RT_EOK)
  {
    /* waiting for ack */
    rt_sem_take(&(enetif->tx_ack), RT_WAITING_FOREVER);
  }
#else
  struct eth_device* enetif;
  
  RT_ASSERT(netif != RT_NULL);
  enetif = (struct eth_device*)netif->state;
  
  if (enetif->eth_tx(&(enetif->parent), p) != RT_EOK)
  {
    return ERR_IF;
  }
#endif
  return ERR_OK;
}

/*********************************************************************************************************
** Function name:       eth_netif_device_init
** Descriptions:        以太网网络接口初始化
** input parameters:    netif:	网络接口标志
** output parameters:   无
** Returned value:      初始化结果
*********************************************************************************************************/
static err_t eth_netif_device_init(struct netif *netif)
{
  struct eth_device *ethif;
  
  ethif = (struct eth_device*)netif->state;
  if (ethif != RT_NULL)
  {
    rt_device_t device;
    
    /* get device object */
    device = (rt_device_t) ethif;
    if (rt_device_init(device) != RT_EOK)
    {
      return ERR_IF;
    }
    
    /* set output */
#if LWIP_IPV4
    netif->output       = etharp_output;
#endif
#if LWIP_IPV6
    netif->output_ip6   = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput	= ethernetif_linkoutput;
    
    /* set name */
    netif->name[0] = ethif->parent.parent.name[0];
    netif->name[1] = ethif->parent.parent.name[1];
    
#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "hichard";
#endif /* LWIP_NETIF_HOSTNAME */
    
    /* copy device flags to netif flags */
    netif->flags = ethif->flags;
    
    /* set hw address to 6 */
    netif->hwaddr_len 	= 6;
    /* maximum transfer unit */
    netif->mtu  = ETHERNET_MTU;
    /* get hardware MAC address */
    rt_device_control(&(ethif->parent), NIOCTL_GADDR, netif->hwaddr);
    
    /*
    * Initialize the snmp variables and counters inside the struct netif.
    * The last argument should be replaced with your link speed, in units
    * of bits per second.
    */
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);
    
    /* set default netif */
    if (netif_default == RT_NULL) {
      netif_set_default(ethif->netif);
    }
    
    /* set interface up */
    netif_set_up(ethif->netif);
    
#if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_up(ethif->netif);
#endif
    
    return ERR_OK;
  }
  
  return ERR_IF;
}

/*********************************************************************************************************
** Function name:       eth_device_init
** Descriptions:        以太网网络设备初始化
** input parameters:    dev:	以太网驱动结构
** 						name:	设备名字
** output parameters:   无
** Returned value:      初始化结果
*********************************************************************************************************/
rt_err_t eth_device_init(struct eth_device * dev, char *name)
{
  rt_uint8_t flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
  struct netif* netif;
  
#if LWIP_IGMP
  /* IGMP support */
  flags |= NETIF_FLAG_IGMP;
#endif
  
  netif = (struct netif*) rt_malloc (sizeof(struct netif));
  if (netif == RT_NULL)
  {
    rt_kprintf("malloc netif failed\n");
    return -RT_ERROR;
  }
  rt_memset(netif, 0, sizeof(struct netif));
  
  /* set netif */
  dev->netif = netif;
  /* device flags, which will be set to netif flags when initializing */
  dev->flags = flags;
  /* link changed status of device */
  dev->link_changed = 0x00;
  dev->parent.type = RT_Device_Class_NetIf;
  /* register to RT-Thread device manager */
  rt_device_register(&(dev->parent), name, RT_DEVICE_FLAG_RDWR);
  rt_sem_init(&(dev->tx_ack), name, 0, RT_IPC_FLAG_FIFO);
  
  /* if tcp thread has been started up, we add this netif to the system */
  if (rt_thread_find("tcpip") != RT_NULL)
  {
#if LWIP_IPV4
    struct ip4_addr ipaddr, netmask, gw;
    
    memset(&ipaddr, 0, sizeof(struct ip4_addr));
    memset(&netmask, 0, sizeof(struct ip4_addr));
    memset(&gw, 0, sizeof(struct ip4_addr));
#endif
    
    netif_add(netif, 
#if LWIP_IPV4
              &ipaddr, &netmask, &gw, 
#endif
              dev, eth_netif_device_init, tcpip_input);
#if LWIP_IPV6
    netif_create_ip6_linklocal_address(netif, 1);
    netif_ip6_addr_set_state(netif, 0, IP6_ADDR_PREFERRED);
#endif
  }
  
  return RT_EOK;
}

/*********************************************************************************************************
** Function name:       eth_device_deinit
** Descriptions:        以太网收到数据发送邮箱
** input parameters:    dev:	以太网驱动结构
** output parameters:   无
** Returned value:      准备结果
*********************************************************************************************************/
void eth_device_deinit(struct eth_device *dev)
{
    struct netif* netif = dev->netif;

#if LWIP_DHCP
    dhcp_stop(netif);
    dhcp_cleanup(netif);
#endif
    netif_set_down(netif);
    netif_remove(netif);
    rt_device_close(&(dev->parent));
    rt_device_unregister(&(dev->parent));
    rt_sem_detach(&(dev->tx_ack));
    rt_free(netif);
}

#ifndef LWIP_NO_RX_THREAD
/*********************************************************************************************************
** Function name:       eth_device_ready
** Descriptions:        以太网收到数据发送邮箱
** input parameters:    dev:	以太网驱动结构
** output parameters:   无
** Returned value:      准备结果
*********************************************************************************************************/
rt_err_t eth_device_ready(struct eth_device* dev)
{
  if (dev->netif)
    /* post message to Ethernet thread */
    return rt_mb_send(&eth_rx_thread_mb, (rt_uint32_t)dev);
  else
    return ERR_OK; /* netif is not initialized yet, just return. */
}

/*********************************************************************************************************
** Function name:       eth_device_linkchange
** Descriptions:        以太网收到数据发送邮箱
** input parameters:    dev: 以太网驱动结构
**                      up:  改变结果，是否连接成功，1：表示连接成功；0：表示未连接
** output parameters:   无
** Returned value:      装填专递结果
*********************************************************************************************************/
rt_err_t eth_device_linkchange(struct eth_device* dev, rt_bool_t up)
{
  rt_uint32_t level;
  
  RT_ASSERT(dev != RT_NULL);
  
  level = rt_hw_interrupt_disable();
  dev->link_changed = 0x01;
  if (up == RT_TRUE)
    dev->link_status = 0x01;
  else
    dev->link_status = 0x00;
  rt_hw_interrupt_enable(level);
  
  /* post message to ethernet thread */
  return rt_mb_send(&eth_rx_thread_mb, (rt_uint32_t)dev);
}
#else
/* NOTE: please not use it in interrupt when no RxThread exist */
rt_err_t eth_device_linkchange(struct eth_device* dev, rt_bool_t up)
{
  if (up == RT_TRUE)
    netif_set_link_up(dev->netif);
  else
    netif_set_link_down(dev->netif);
  
  return RT_EOK;
}
#endif

#ifndef LWIP_NO_TX_THREAD
/*********************************************************************************************************
** Function name:       eth_tx_thread_entry
** Descriptions:        Ethernet Tx Thread
** input parameters:    parameter:	线程传递参数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
static void eth_tx_thread_entry(void* parameter)
{
  struct eth_tx_msg* msg;
  
  while (1)
  {
    if (rt_mb_recv(&eth_tx_thread_mb, (rt_uint32_t*)&msg, RT_WAITING_FOREVER) == RT_EOK)
    {
      struct eth_device* enetif;
      
      RT_ASSERT(msg->netif != RT_NULL);
      RT_ASSERT(msg->buf   != RT_NULL);
      
      enetif = (struct eth_device*)msg->netif->state;
      if (enetif != RT_NULL)
      {
        /* call driver's interface */
        if (enetif->eth_tx(&(enetif->parent), msg->buf) != RT_EOK)
        {
          rt_kprintf("transmit eth packet failed\n");
        }
      }
      
      /* send ACK */
      rt_sem_release(&(enetif->tx_ack));
    }
  }
}
#endif

#ifndef LWIP_NO_RX_THREAD
/*********************************************************************************************************
** Function name:       eth_rx_thread_entry
** Descriptions:        Ethernet Rx Thread
** input parameters:    parameter:	线程传递参数
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
static void eth_rx_thread_entry(void* parameter)
{
  struct eth_device* device;
  
  while (1)
  {
    if (rt_mb_recv(&eth_rx_thread_mb, (rt_ubase_t*)&device, RT_WAITING_FOREVER) == RT_EOK)
    {
      struct pbuf *p;
      
      /* check link status */
      if (device->link_changed)
      {
        int status;
        rt_uint32_t level;
        
        level = rt_hw_interrupt_disable();
        status = device->link_status;
        device->link_changed = 0x00;
        rt_hw_interrupt_enable(level);
        
        if (status)
          netif_set_link_up(device->netif);
        else
          netif_set_link_down(device->netif);
      }
      
      /* receive all of buffer */
      do
      {
        p = device->eth_rx(&(device->parent));
        if (p != RT_NULL)
        {
          /* notify to upper layer */
          if( device->netif->input(p, device->netif) != ERR_OK )
          {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: Input error\n"));
            pbuf_free(p);
          }
        }
      } while(p != RT_NULL);
    }
    else
    {
      LWIP_ASSERT("Should not happen!\n",0);
    }
  }
}
#endif

/*********************************************************************************************************
** Function name:       eth_system_device_init
** Descriptions:        以太网设备初始化，初始化后，开始支持以太网设备
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
int eth_system_device_init(void)
{
  rt_err_t result = RT_EOK;
  
  /* initialize Rx thread. */
#ifndef LWIP_NO_RX_THREAD
  /* initialize mailbox and create Ethernet Rx thread */
  result = rt_mb_init(&eth_rx_thread_mb, "erxmb",
                      &eth_rx_thread_mb_pool[0], sizeof(eth_rx_thread_mb_pool)/4,
                      RT_IPC_FLAG_FIFO);
  RT_ASSERT(result == RT_EOK);
  
  result = rt_thread_init(&eth_rx_thread, "erx", eth_rx_thread_entry, RT_NULL,
                          &eth_rx_thread_stack[0], sizeof(eth_rx_thread_stack),
                          RT_LWIP_ETHTHREAD_PRIORITY, 16);
  RT_ASSERT(result == RT_EOK);
  result = rt_thread_startup(&eth_rx_thread);
  RT_ASSERT(result == RT_EOK);
#endif
  
  /* initialize Tx thread */
#ifndef LWIP_NO_TX_THREAD
  /* initialize mailbox and create Ethernet Tx thread */
  result = rt_mb_init(&eth_tx_thread_mb, "etxmb",
                      &eth_tx_thread_mb_pool[0], sizeof(eth_tx_thread_mb_pool)/4,
                      RT_IPC_FLAG_FIFO);
  RT_ASSERT(result == RT_EOK);
  
  result = rt_thread_init(&eth_tx_thread, "etx", eth_tx_thread_entry, RT_NULL,
                          &eth_tx_thread_stack[0], sizeof(eth_tx_thread_stack),
                          RT_ETHERNETIF_THREAD_PREORITY, 16);
  RT_ASSERT(result == RT_EOK);
  
  result = rt_thread_startup(&eth_tx_thread);
  RT_ASSERT(result == RT_EOK);
#endif
  
  rt_kprintf("Init ethernet driver frame!\n");
  return (int)result;
}

INIT_DEVICE_EXPORT(eth_system_device_init);
#endif // End of RT_USE_ETHERNET
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
