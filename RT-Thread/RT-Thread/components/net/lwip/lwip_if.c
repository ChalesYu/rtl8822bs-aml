/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwip_if.c
** Last modified Date:  2015-01-09
** Last Version:        v1.00
** Description:         lwip��Ӧ�ýӿں���ʵ��
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

#ifdef RT_USING_LWIP

#include <string.h>
#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"
#include "lwip/init.h"
#include "lwip/inet.h"
#include "lwip/dhcp.h"
#include "lwip/mld6.h"
#if LWIP_RPL
#include "netif/rpl/rpl_if.h"
#include "netif/radio/lowpan_crypt.h"
#endif
#include "netif/etharp.h"

#if LWIP_DHCP_SERVER
#include "dhcp_server/dhcp_server.h"
#endif

#if LWIP_NAPTPT
#include "naptpt/naptpt_if.h"
#include "naptpt/naptpt_ip.h"
#endif

#if LWIP_NAT
#include "nat/nat_if.h"
#include "nat/nat_ip.h"
#endif

#if LWIP_ROUTE
#include "route/ip_route.h"
#endif

#include <string.h>
#include <stdio.h>

/*********************************************************************************************************
** Function name:       lwip_netif_find
** Descriptions:        ����ָ�����ֵ�����
** input parameters:    netif_name�� Ҫ���ҵ���������
** output parameters:   ��
** Returned value:      �ҵ���netif���Ҳ�������RT_NULL
*********************************************************************************************************/
struct netif * lwip_netif_find(char *netif_name)
{
  struct netif *netif;
  
  if (netif_name == RT_NULL) {
    return RT_NULL;
  }
  
  if(strlen(netif_name) > sizeof(netif->name))
  {
    rt_kprintf("network interface name too long!\r\n");
    return RT_NULL;
  }
  
  for(netif = netif_list; netif != NULL; netif = netif->next) {
    if(strncmp(netif_name, netif->name, sizeof(netif->name)) == 0) {
      //rt_kprintf("lwip_netif_find: found %c%c\n", netif_name[0], netif_name[1]);
      return netif;
    }
  }
  rt_kprintf("lwip_netif_find: didn't find %c%c\n", netif_name[0], netif_name[1]);
  return RT_NULL;
}

/*********************************************************************************************************
** Function name:       lwip_system_init
** Descriptions:        ��ʼ��lwipЭ��ջ
** input parameters:    ��
** output parameters:   ��
** Returned value:      0:  ��ʼ���ɹ�
** 			-1: 	��ʼ��ʧ��
*********************************************************************************************************/
int lwip_system_init(void)
{
  rt_kprintf("Init lwip tcp/ip stack!\n");
  
  tcpip_init(NULL, NULL);
  
  /* Init RPL */
#if LWIP_RPL
  rpl_init();
#endif
  
  rt_kprintf("lwIP-%d.%d.%d initialized!\n", LWIP_VERSION_MAJOR, LWIP_VERSION_MINOR, LWIP_VERSION_REVISION);
  
  return 0;
}
INIT_PREV_EXPORT(lwip_system_init);

/*********************************************************************************************************
** Function name:       if_down
** Descriptions:        �ر�����
** input parameters:    ifname: ��������
** output parameters:   ��
** Returned value:      RT_EOK: 	�رճɹ�
** 			-RT_ERROR: 	�ر�ʧ��
*********************************************************************************************************/
int if_down (const char *ifname)
{
  int            iError;
  struct netif  *pnetif;
  
  LWIP_NETIF_LOCK();                                                  /*  �����ٽ���                  */
  pnetif = lwip_netif_find((char *)ifname);
  if (pnetif) {
    netif_set_down(pnetif);
    iError = RT_EOK;
  } else {
    iError = -RT_ERROR;
  }
  LWIP_NETIF_UNLOCK();                                                /*  �˳��ٽ���                  */
  
  return  (iError);
}

/*********************************************************************************************************
** Function name:       if_up
** Descriptions:        ������
** input parameters:    ifname: ��������
** output parameters:   ��
** Returned value:      RT_EOK: 	�򿪳ɹ�
** 			-RT_ERROR: 	��ʧ��
*********************************************************************************************************/
int if_up (const char *ifname)
{
  int            iError;
  struct netif  *pnetif;
  
  LWIP_NETIF_LOCK();                                                  /*  �����ٽ���                  */
  pnetif = lwip_netif_find((char *)ifname);
  if (pnetif) {
    netif_set_up(pnetif);
    iError = RT_EOK;
  } else {
    iError = -RT_ERROR;
  }
  LWIP_NETIF_UNLOCK();                                                /*  �˳��ٽ���                  */
  
  return  (iError);
}

/*********************************************************************************************************
** Function name:       if_isup
** Descriptions:        �ж������Ƿ��
** input parameters:    ifname: ��������
** output parameters:   ��
** Returned value:      1: 	�����Ǵ򿪵�
** 			0: 	�����ǹرյ�
** 			-RT_ERROR: 	����������
*********************************************************************************************************/
int if_isup (const char *ifname)
{
  int            iRet;
  struct netif  *pnetif;
  
  LWIP_NETIF_LOCK();                                                  /*  �����ٽ���                  */
  pnetif = lwip_netif_find((char *)ifname);
  if (pnetif) {
    if (pnetif->flags & NETIF_FLAG_UP) {
      iRet = 1;
    } else {
      iRet = 0;
    }
  } else {
    iRet = -RT_ERROR;
  }
  LWIP_NETIF_UNLOCK();                                                /*  �˳��ٽ���                  */
  
  return  (iRet);
}

/*********************************************************************************************************
** Function name:       if_islink
** Descriptions:        �����Ƿ��Ѿ�����
** input parameters:    ifname: ��������
** output parameters:   ��
** Returned value:      1: 	�����Ǵ򿪵�
** 			0: 	�����ǹرյ�
** 			-RT_ERROR: 	����������
*********************************************************************************************************/
int if_islink (const char *ifname)
{
  int            iRet;
  struct netif  *pnetif;
  
  LWIP_NETIF_LOCK();                                                  /*  �����ٽ���                  */
  pnetif = lwip_netif_find((char *)ifname);
  if (pnetif) {
    if (pnetif->flags & NETIF_FLAG_LINK_UP) {
      iRet = 1;
    } else {
      iRet = 0;
    }
  } else {
    iRet = RT_ERROR;
  }
  LWIP_NETIF_UNLOCK();                                                /*  �˳��ٽ���                  */
  
  return  (iRet);
}

/*********************************************************************************************************
** Function name:       if_set_default
** Descriptions:        ����Ĭ������ӿ�
** input parameters:    ifname: ��������
** output parameters:   ��
** Returned value:      RT_EOK: 	�򿪳ɹ�
** 			-RT_ERROR: 	��ʧ��
*********************************************************************************************************/
int if_set_default (const char *ifname)
{
  int            iError;
  struct netif  *pnetif;
  
  LWIP_NETIF_LOCK();                                                  /*  �����ٽ���                  */
  pnetif = lwip_netif_find((char *)ifname);
  if (pnetif) {
    netif_set_default(pnetif);
    iError = RT_EOK;
  } else {
    iError = -RT_ERROR;
  }
  LWIP_NETIF_UNLOCK();                                                /*  �˳��ٽ���                  */
  
  return  (iError);
}

#if LWIP_DHCP
/*********************************************************************************************************
** Function name:       if_set_dhcp
** Descriptions:        �������� dhcp ѡ��
** input parameters:    ifname: ��������
** 			en:		1: ʹ�� dhcp  0: ���� dhcp
** output parameters:   ��
** Returned value:      RT_EOK: 	���óɹ�
** 			-RT_ERROR: 	����ʧ��
*********************************************************************************************************/
int if_set_dhcp (const char *ifname, int en)
{
  int            iRet = RT_EOK;
  struct netif  *pnetif;
  ip4_addr_t      inaddrNone;
  
  memset(&inaddrNone, sizeof(ip_addr_t), 0);
  
  LWIP_NETIF_LOCK();                                                  /*  �����ٽ���                  */
  pnetif = lwip_netif_find((char *)ifname);
  if (pnetif) {
    if (en) {
      dhcp_start(pnetif);
    } else {
      dhcp_stop(pnetif);                                             /*  ��� DHCP ��Լ, ͬʱֹͣ����*/
      netif_set_addr(pnetif, &inaddrNone, &inaddrNone,
                              &inaddrNone);
    }
  } else {
    iRet =  -RT_ERROR;
  }
  LWIP_NETIF_UNLOCK();                                                /*  �˳��ٽ���                  */
  
  return  (iRet);
}
#endif

#if LWIP_DHCP_SERVER
/*********************************************************************************************************
** Function name:       if_dhcp_server
** Descriptions:        ����DHCP����������
** input parameters:    ifname: ��������
** output parameters:   ��
** Returned value:      1: 	����DHCP�Ǵ򿪵�
** 			0: 	����DHCP�ǹرյ�
** 			-RT_ERROR: 	����������
*********************************************************************************************************/
int  if_dhcp_server (const char *ifname, const char *IP_Start, const char *IP_End)
{
  int           iError;
  ip4_addr_t     ip_start;
  ip4_addr_t     ip_end;
  struct netif  *pnetif;
  
  LWIP_NETIF_LOCK();                                                  /*  �����ٽ���                  */
  pnetif = lwip_netif_find((char *)ifname);
  if (pnetif) {
    ip4addr_aton(IP_Start, &ip_start);
    ip4addr_aton(IP_End, &ip_end);
    dhcp_server_start(pnetif,  &ip_start, &ip_end);
    iError = RT_EOK;
  } else {
    iError = -RT_ERROR;
  }
  LWIP_NETIF_UNLOCK();                                                /*  �˳��ٽ���                  */
  
  return  (iError);
}
#endif

#if LWIP_IPV4
/*********************************************************************************************************
** Function name:       set_if
** Descriptions:        ��������IP��ַ
** input parameters:    ifname: 	��������
** 			ip_addr: 	IP��ַ
** 			gw_addr:    ����
** 			nm_addr:	��������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void set_if(char* ifname, char* ip_addr, char* gw_addr, char* nm_addr)
{
  struct ip4_addr *ip;
  struct ip4_addr addr;
  struct netif  *pnetif;
  
  pnetif = lwip_netif_find((char *)ifname);
  if(pnetif == NULL) {
    rt_kprintf("Can't find network interface!\r\n");
    return;
  }
  
  ip = (struct ip4_addr *)&addr;
  
  /* set ip address */
  if ((ip_addr != RT_NULL) && ip4addr_aton(ip_addr, &addr))
  {
    netif_set_ipaddr(pnetif, ip);
  }
  
  /* set gateway address */
  if ((gw_addr != RT_NULL) && ip4addr_aton(gw_addr, &addr))
  {
    netif_set_gw(pnetif, ip);
  }
  
  /* set netmask address */
  if ((nm_addr != RT_NULL) && ip4addr_aton(nm_addr, &addr))
  {
    netif_set_netmask(pnetif, ip);
  }
}

/*********************************************************************************************************
** Function name:       get_if
** Descriptions:        ��ȡ����IP��ַ
** input parameters:    ifname: 	��������
** 			ip_addr: 	IP��ַ
** 			gw_addr:        ����
** 			nm_addr:	��������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void get_if(char* ifname, char* ip_addr, char* gw_addr, char* nm_addr)
{
  struct netif  *pnetif;
  
  pnetif = lwip_netif_find((char *)ifname);
  if(pnetif == NULL) {
    rt_kprintf("Can't find network interface!\r\n");
    return;
  }
  
  strcpy(ip_addr, ip4addr_ntoa((const ip4_addr_t *)&(pnetif->ip_addr)));
  strcpy(gw_addr, ip4addr_ntoa((const ip4_addr_t *)&(pnetif->gw)));
  strcpy(nm_addr, ip4addr_ntoa((const ip4_addr_t *)&(pnetif->netmask)));
}
#endif

/*********************************************************************************************************
** Function name:       get_mac
** Descriptions:        ��ȡ����mac��ַ
** input parameters:    ifname:  ��������
** 			mac: 	 ��ȡ����mac��ַ��ŵ�ַ
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void get_mac(char* ifname, char* mac)
{
  struct netif  *pnetif;
  
  pnetif = lwip_netif_find((char *)ifname);
  if(pnetif == NULL) {
    rt_kprintf("Can't find network interface!\r\n");
    return;
  }
  
  memcpy(mac, pnetif->hwaddr, pnetif->hwaddr_len);
}

#if LWIP_IPV6
/*********************************************************************************************************
** Function name:       set_if6
** Descriptions:        ��������IPv6��ַ
** input parameters:    ifname: 	��������
** 			ip6_addr: 	IPv6��ַ
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void set_if6(char* ifname, char* ip6_addr)
{
  ip_addr_t ipaddr6;
  struct netif  *pnetif;
  
  pnetif = lwip_netif_find((char *)ifname);
  
  if(pnetif == NULL) {
    rt_kprintf("Can't find network interface!\r\n");
    return;
  }
  
  /* set ip address */
  if ((ip6_addr != RT_NULL) && inet6_aton(ip6_addr, &ipaddr6))
  {
    pnetif->ip6_addr[1] = ipaddr6;
    netif_ip6_addr_set_state(pnetif, 1, IP6_ADDR_TENTATIVE);
  }
}

/*********************************************************************************************************
** Function name:       set_if6
** Descriptions:        ��������IPv6��ַ
** input parameters:    ifname: 	��������
** 			ip6_addr: 	IPv6��ַ
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void get_if6(char* ifname, char* ip6_local, char *ip6_global)
{
  struct netif  *pnetif;
  
  pnetif = lwip_netif_find((char *)ifname);
  
  if(pnetif == NULL) {
    rt_kprintf("Can't find network interface!\r\n");
    return;
  }
  
  if(ip6_local != NULL) {
    strcpy(ip6_local, ip6addr_ntoa( netif_ip6_addr(pnetif, 0)));
  }
  if(ip6_global != NULL) {
    strcpy(ip6_global, ip6addr_ntoa( netif_ip6_addr(pnetif, 1)));
  }
}

/*********************************************************************************************************
** Function name:       ip6_join_group
** Descriptions:        ������������ಥ��
** input parameters:    ifname: 	��������
** 			groupaddr: 	IPv6�ಥ��ַ
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void ip6_join_group(char* ifname, char *groupaddr)
{
  struct netif  *pnetif;
  ip_addr_t ipaddr6;
  
  pnetif = lwip_netif_find((char *)ifname);
  
  if(pnetif == NULL) {
    rt_kprintf("Can't find network interface!\r\n");
    return;
  }
  
  inet6_aton(groupaddr, &ipaddr6);
  if(ERR_OK == mld6_joingroup_netif(pnetif, (const ip6_addr_t *)&ipaddr6)) {
    rt_kprintf("Join %s into ipv6 Multicast list OK!\r\n", groupaddr);
  } else {
    rt_kprintf("Join %s into ipv6 Multicast list failed!\r\n", groupaddr);
  }
}

/*********************************************************************************************************
** Function name:       ip6_leave_group
** Descriptions:        ���������˳��ಥ��
** input parameters:    ifname: 	��������
** 			groupaddr: 	IPv6�ಥ��ַ
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void ip6_leave_group(char* ifname, char *groupaddr)
{
  struct netif  *pnetif;
  ip_addr_t ipaddr6;
  
  pnetif = lwip_netif_find((char *)ifname);
  
  if(pnetif == NULL) {
    rt_kprintf("Can't find network interface!\r\n");
    return;
  }
  
  inet6_aton(groupaddr, &ipaddr6);
  if(ERR_OK == mld6_leavegroup_netif(pnetif, (const ip6_addr_t *)&ipaddr6)) {
    rt_kprintf("Leave %s out ipv6 Multicast list OK!\r\n", groupaddr);
  } else {
    rt_kprintf("Leave %s out ipv6 Multicast list failed!\r\n", groupaddr);
  }
}
#endif

#if (LWIP_DNS && LWIP_IPV4)
/*********************************************************************************************************
** Function name:       set_dns
** Descriptions:        ����DNS��������ַ
** input parameters:    dns_server: 	DNS��������ַ
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
#include <lwip/dns.h>
void set_dns(char numdns, char* dns_server)
{
  ip_addr_t addr;
  
  if ((dns_server != RT_NULL) && ipaddr_aton(dns_server, &addr))
  {
    dns_setserver(numdns, &addr);
  }
}
/*********************************************************************************************************
** Function name:       get_dns
** Descriptions:        ��ȡDNS��������ַ
** input parameters:    numdns:  DNS��������ַ���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
char *get_dns(char numdns)
{
  //ip_addr_t *ip4_addr = dns_getserver(numdns);
  const ip_addr_t *ip4_addr = dns_getserver(numdns);
  return (ip4addr_ntoa((const ip4_addr_t *)(ip4_addr)));
}
#endif

#if LWIP_NAPTPT
/*********************************************************************************************************
** Function name:       naptpt_if_set
** Descriptions:        ����һ��naptpt�ӿ�
** input parameters:    wan��naptpt��WAN��
**                      lan��naptpt��LAN��
** output parameters:   ��
** Returned value:      ��ʼ�����
*********************************************************************************************************/
int naptpt_if_set(const char *wan, const char *lan)
{
  static rt_uint8_t naptpt = 0;
  struct naptpt_entry *entry;
  struct netif  *wan_netif;
  struct netif  *lan_netif;
  
  /*
  ** ��ʼ��naptpt���ܣ�ʹlwip֧��naptpt
  */
  if(naptpt == 0) {
    naptpt = 1;
    naptpt_init();
  }
  
  entry = rt_malloc(sizeof(struct naptpt_entry));
  if(entry == RT_NULL) {
    return RT_ENOMEM;
  }
  
  wan_netif = lwip_netif_find((char *)wan);
  if(wan_netif == NULL) {
    rt_kprintf("Can't find naptpt wan interface!\r\n");
    return -RT_ERROR;
  }
  lan_netif = lwip_netif_find((char *)lan);
  if(lan_netif == NULL) {
    rt_kprintf("Can't find naptpt lan interface!\r\n");
    return -RT_ERROR;
  }
  
  /*
  ** ��ʼ��naptpt�ӿڱ�־��������̫����ΪWAN�ڣ�LanPAN��ΪLAN��
  */
  entry->out_if = wan_netif;
  entry->in_if = lan_netif;
   
  /*
  ** ���NAT������
  */
  naptpt_add(entry);
  
  return RT_EOK;
}
#endif

#if LWIP_NAT
/*********************************************************************************************************
** Function name:       nat_if_set
** Descriptions:        ����һ��nat�ӿ�
** input parameters:    wan��nat��WAN��
**                      lan��nat��LAN��
** output parameters:   ��
** Returned value:      ��ʼ�����
*********************************************************************************************************/
int nat_if_set(const char *wan, const char *lan)
{
  static rt_uint8_t nat_init_flag = 0;
  struct nat_entry *entry;
  struct netif  *wan_netif;
  struct netif  *lan_netif;
  
  /*
  ** ��ʼ��nat���ܣ�ʹlwip֧��nat
  */
  if(nat_init_flag == 0) {
    nat_init_flag = 1;
    nat_init();
  }
  
  entry = rt_malloc(sizeof(struct nat_entry));
  if(entry == RT_NULL) {
    return RT_ENOMEM;
  }
  
  memset(entry, 0, sizeof(struct nat_entry));
  wan_netif = lwip_netif_find((char *)wan);
  if(wan_netif == NULL) {
    rt_kprintf("Can't find nat wan interface!\r\n");
    return -RT_ERROR;
  }
  lan_netif = lwip_netif_find((char *)lan);
  if(lan_netif == NULL) {
    rt_kprintf("Can't find nat lan interface!\r\n");
    return -RT_ERROR;
  }
  
  /*
  ** ��ʼ��naptpt�ӿڱ�־��������̫����ΪWAN�ڣ�LanPAN��ΪLAN��
  */
  entry->out_if = wan_netif;
  entry->in_if = lan_netif;
   
  /*
  ** ���NAT������
  */
  nat_add(entry);
  
  return RT_EOK;
}
#endif

#if LWIP_ROUTE
#if LWIP_IPV4
/*********************************************************************************************************
** Function name:       route4_add
** Descriptions:        ����һ��naptpt�ӿ�
** input parameters:    ifname��ָ������ӿ�
**                      ip_addr����ӵ�IP��ַ�����ַ
**                      nm_addr����ӵ���������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void route4_add(char* ifname, char* ip_addr, char* nm_addr)
{
  ip4_addr_t ip4_address;
  ip4_addr_t ip4_netmask;
  struct netif  *pnetif;
  
  pnetif = lwip_netif_find((char *)ifname);
  
  if(pnetif == NULL) {
    rt_kprintf("Can't find network interface!\r\n");
    return;
  }
  
  ip4addr_aton(ip_addr, &ip4_address);
  ip4addr_aton(nm_addr, &ip4_netmask);
  
  route_ip4_add(&ip4_address, &ip4_netmask, pnetif);
}
/*********************************************************************************************************
** Function name:       route6_add
** Descriptions:        ����һ��naptpt�ӿ�
** input parameters:    ip_addr����ӵ�IP��ַ�����ַ
**                      nm_addr����ӵ���������
** output parameters:   ��
** Returned value:      ��ʼ�����
*********************************************************************************************************/
void route4_delete(char* ip_addr, char* nm_addr)
{
  ip4_addr_t ip4_address;
  ip4_addr_t ip4_netmask;
  
  ip4addr_aton(ip_addr, &ip4_address);
  ip4addr_aton(nm_addr, &ip4_netmask);
  
  route_ip4_delete(&ip4_address, &ip4_netmask);
}
#endif

#if LWIP_IPV6
/*********************************************************************************************************
** Function name:       route6_add
** Descriptions:        ���IPv6·�ɱ�
** input parameters:    ifname��ָ������ӿ�
**                      ip6_addr����ӵ�IPv6��ַǰ׺
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void route6_add(char* ifname, char* ip6_addr)
{
  ip6_addr_t ip6_address;
  struct netif  *pnetif;
  
  pnetif = lwip_netif_find((char *)ifname);
  
  if(pnetif == NULL) {
    rt_kprintf("Can't find network interface!\r\n");
    return;
  }
  
  ip6addr_aton(ip6_addr, (ip6_addr_t *)&ip6_address);
  
  route_ip6_add(&ip6_address, pnetif);
}
/*********************************************************************************************************
** Function name:       route6_add
** Descriptions:        ɾ��IPv6·�ɱ�
** input parameters:    ip6_addr����ɾ����·�ɱ�����ǰ׺
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void route6_delete(char* ip6_addr)
{
  ip6_addr_t ip6_address;
  
  ip6addr_aton(ip6_addr, (ip6_addr_t *)&ip6_address);
  
  route_ip6_delete(&ip6_address);
}
#endif
#endif

#ifdef RT_USE_6LOWPAN
#if (LOWPAN_AES_CRYPT || LOWPAN_SIMPLE_CRYPT)
/*********************************************************************************************************
** Function name:       lowpan_crypt_set
** Descriptions:        ����һ��naptpt�ӿ�
** input parameters:    wan��naptpt��WAN��
**                      lan��naptpt��LAN��
** output parameters:   ��
** Returned value:      ��ʼ�����
*********************************************************************************************************/
int lowpan_crypt_set(const char *pan, rt_uint8_t mode, char *key)
{
  struct lowpanif  *lowpanif;
  
  lowpanif = (struct lowpanif  *)lwip_netif_find((char *)pan);
  if(lowpanif == NULL) {
    rt_kprintf("Can't find lowpan interface!\r\n");
    return -RT_ERROR;
  }
  
  lowpan_crypt_key_set(lowpanif,mode ,key);
  
  return RT_EOK;
}
#endif
#endif
/*********************************************************************************************************
**  �ṩ���û���shell����ʵ��
*********************************************************************************************************/
#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(if_down, set network down);
FINSH_FUNCTION_EXPORT(if_up, set network up);
#if LWIP_DHCP
FINSH_FUNCTION_EXPORT(if_set_dhcp, set dhcp);
#endif
#if LWIP_IPV4
FINSH_FUNCTION_EXPORT(set_if, set network interface address);
#endif
#if LWIP_IPV6
FINSH_FUNCTION_EXPORT(set_if6, set network interface IPv6 address);
#endif
#if LWIP_DNS
FINSH_FUNCTION_EXPORT(set_dns, set DNS server address);
#endif
void list_if(void)
{
  rt_ubase_t index;
  struct netif * netif;
  
  rt_enter_critical();
  
  netif = netif_list;
  
  while( netif != RT_NULL )
  {
    rt_kprintf("network interface: %c%c%s\n",
               netif->name[0],
               netif->name[1],
               (netif == netif_default)?" (Default)":"");
    rt_kprintf("MTU: %d\n", netif->mtu);
    rt_kprintf("MAC: ");
    for (index = 0; index < netif->hwaddr_len; index ++)
      rt_kprintf("%02x ", netif->hwaddr[index]);
    rt_kprintf("\nFLAGS:");
    if (netif->flags & NETIF_FLAG_UP) rt_kprintf(" UP");
    else rt_kprintf(" DOWN");
    if (netif->flags & NETIF_FLAG_LINK_UP) rt_kprintf(" LINK_UP");
    else rt_kprintf(" LINK_DOWN");
    if (netif->flags & NETIF_FLAG_ETHARP) rt_kprintf(" ETHARP");
    if (netif->flags & NETIF_FLAG_IGMP) rt_kprintf(" IGMP");
    rt_kprintf("\n");
#if LWIP_IPV4
    rt_kprintf("ip address: %s\n", ip4addr_ntoa((const ip4_addr_t *)&(netif->ip_addr)));
    rt_kprintf("gw address: %s\n", ip4addr_ntoa((const ip4_addr_t *)&(netif->gw)));
    rt_kprintf("net mask  : %s\n", ip4addr_ntoa((const ip4_addr_t *)&(netif->netmask)));
#else
    rt_kprintf("Not support IPv4\r\n");
#endif
#if LWIP_IPV6
    rt_kprintf("link local IPv6 address: %s\n", ip6addr_ntoa( netif_ip6_addr(netif, 0)));
    rt_kprintf("global IPv6 address: %s\n", ip6addr_ntoa( netif_ip6_addr(netif, 1)));
#else
    rt_kprintf("Not support IPv6\r\n");
#endif
    rt_kprintf("\r\n");
    netif = netif->next;
  }
  
#if LWIP_DNS
  {
    const ip_addr_t *ip_addr;
    
    for(index=0; index<DNS_MAX_SERVERS; index++)
    {
      ip_addr = dns_getserver(index);
      rt_kprintf("dns server #%d: %s\n", index, ipaddr_ntoa(ip_addr));
    }
  }
#endif /**< #if LWIP_DNS */
  
  rt_exit_critical();
}
FINSH_FUNCTION_EXPORT(list_if, list network interface information);

#if LWIP_TCP
#include <lwip/tcp.h>
#include <lwip/priv/tcp_priv.h>
static const char *const tcp_state_str[] = {
  "CLOSED",
  "LISTEN",
  "SYN_SENT",
  "SYN_RCVD",
  "ESTABLISHED",
  "FIN_WAIT_1",
  "FIN_WAIT_2",
  "CLOSE_WAIT",
  "CLOSING",
  "LAST_ACK",
  "TIME_WAIT"
};
void list_tcps(void)
{
  rt_uint32_t num = 0;
  struct tcp_pcb *pcb;
  char local_ip_str[16];
  char remote_ip_str[16];
  
  extern struct tcp_pcb *tcp_active_pcbs;
  extern union tcp_listen_pcbs_t tcp_listen_pcbs;
  extern struct tcp_pcb *tcp_tw_pcbs;
  extern const char *tcp_state_str[];
  
  rt_enter_critical();
  rt_kprintf("Active PCB states:\n");
  for(pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next)
  {
    strcpy(local_ip_str, ipaddr_ntoa((const ip_addr_t *)&(pcb->local_ip)));
    strcpy(remote_ip_str, ipaddr_ntoa((const ip_addr_t *)&(pcb->remote_ip)));
    
    rt_kprintf("#%d %s:%d <==> %s:%d snd_nxt 0x%08X rcv_nxt 0x%08X ",
               num++,
               local_ip_str,
               pcb->local_port,
               remote_ip_str,
               pcb->remote_port,
               pcb->snd_nxt,
               pcb->rcv_nxt);
    rt_kprintf("state: %s\n", tcp_state_str[pcb->state]);
  }
  
  rt_kprintf("Listen PCB states:\n");
  num = 0;
  for(pcb = (struct tcp_pcb *)tcp_listen_pcbs.pcbs; pcb != NULL; pcb = pcb->next)
  {
    rt_kprintf("#%d local port %d ", num++, pcb->local_port);
    rt_kprintf("state: %s\n", tcp_state_str[pcb->state]);
  }
  
  rt_kprintf("TIME-WAIT PCB states:\n");
  num = 0;
  for(pcb = tcp_tw_pcbs; pcb != NULL; pcb = pcb->next)
  {
    strcpy(local_ip_str, ipaddr_ntoa((const ip_addr_t *)&(pcb->local_ip)));
    strcpy(remote_ip_str,ipaddr_ntoa((const ip_addr_t *)&(pcb->remote_ip)));
    
    rt_kprintf("#%d %s:%d <==> %s:%d snd_nxt 0x%08X rcv_nxt 0x%08X ",
               num++,
               local_ip_str,
               pcb->local_port,
               remote_ip_str,
               pcb->remote_port,
               pcb->snd_nxt,
               pcb->rcv_nxt);
    rt_kprintf("state: %s\n", tcp_state_str[pcb->state]);
  }
  rt_exit_critical();
}
FINSH_FUNCTION_EXPORT(list_tcps, list all of tcp connections);
#endif 		// End of LWIP_TCP

#if LWIP_UDP
#include "lwip/udp.h"
void list_udps(void)
{
    struct udp_pcb *pcb;
    rt_uint32_t num = 0;
    char local_ip_str[16];
    char remote_ip_str[16];

    rt_enter_critical();
    rt_kprintf("Active UDP PCB states:\n");
    for (pcb = udp_pcbs; pcb != NULL; pcb = pcb->next)
    {
        strcpy(local_ip_str, ipaddr_ntoa(&(pcb->local_ip)));
        strcpy(remote_ip_str, ipaddr_ntoa(&(pcb->remote_ip)));

        rt_kprintf("#%d %d %s:%d <==> %s:%d \n",
                   num, (int)pcb->flags,
                   local_ip_str,
                   pcb->local_port,
                   remote_ip_str,
                   pcb->remote_port);

        num++;
    }
    rt_exit_critical();
}
FINSH_FUNCTION_EXPORT(list_udps, list all of udp connections);
#endif /* LWIP_UDP */
#endif		// End of RT_USING_FINSH


#ifdef FINSH_USING_MSH
int cmd_ifconfig(int argc, char **argv)
{
    if (argc == 1)
    {
        list_if();
    }
    else if (argc == 5)
    {
        rt_kprintf("config : %s\n", argv[1]);
        rt_kprintf("IP addr: %s\n", argv[2]);
        rt_kprintf("Gateway: %s\n", argv[3]);
        rt_kprintf("netmask: %s\n", argv[4]);
        set_if(argv[1], argv[2], argv[3], argv[4]);
    }
    else
    {
        rt_kprintf("bad parameter! e.g: ifconfig e0 192.168.1.30 192.168.1.1 255.255.255.0\n");
    }

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_ifconfig, __cmd_ifconfig, list the information of network interfaces);

#if  LWIP_DNS
#include <lwip/api.h>
#include <lwip/dns.h>
#include <lwip/ip_addr.h>
#include <lwip/init.h>

int cmd_dns(int argc, char **argv)
{
  if (argc == 1)
  {
    int index;
    
    const ip_addr_t *ip_addr;
    for(index=0; index<DNS_MAX_SERVERS; index++)
    {
      ip_addr = dns_getserver(index);
      rt_kprintf("dns server #%d: %s\n", index, ipaddr_ntoa(ip_addr));
    }
  }
  else if (argc == 2)
  {
    rt_kprintf("dns : %s\n", argv[1]);
    set_dns(0 , argv[1]);
  }
  else
  {
    rt_kprintf("bad parameter! e.g: dns 114.114.114.114\n");
  }
  return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_dns, __cmd_dns, list the information of dns);
#endif

#if LWIP_TCP || LWIP_UDP
int cmd_netstat(int argc, char **argv)
{
#ifdef LWIP_TCP
    list_tcps();
#endif
#ifdef LWIP_UDP
    list_udps();
#endif

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_netstat, __cmd_netstat, list the information of TCP / IP);
#endif
#endif          // End of FINSH_USING_MSH

#endif 		// End of RT_USING_LWIP
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
