/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwip_if.h
** Last modified Date:  2015-01-09
** Last Version:        v1.00
** Description:         lwip的应用接口函数实现
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
#ifndef __LWIP_IF_H__
#define __LWIP_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/opt.h"

/*********************************************************************************************************
**  驱动命令使用定义
*********************************************************************************************************/
#define NIOCTL_GADDR		0x01
#define NIOCTL_SENDB		0x02
#define NIOCTL_RECVB		0x03
#define NIOCTL_SENDP		0x04
#define NIOCTL_RECVP		0x05

/*********************************************************************************************************
**  使用6lowpan的时候参数结构定义
*********************************************************************************************************/
struct lowpan_param
{
    rt_uint8_t u8MacAddr[6];
    rt_uint8_t u8Channel;
    rt_uint16_t u16PanID;

    rt_uint8_t u8RplRoot;
    rt_uint8_t u8RplInstanceID;
    rt_uint16_t u16Ocp;
    rt_uint8_t digID[16];
    rt_uint8_t RplPrefix[16];
};

/*********************************************************************************************************
**  实现的外部函数声明
*********************************************************************************************************/
extern int if_down (const char *ifname);
extern int if_up (const char *ifname);
extern int if_isup (const char *ifname);
extern int if_islink (const char *ifname);
extern int if_set_default (const char *ifname);
extern int if_set_dhcp (const char *ifname, int en);
extern int if_dhcp_server (const char *ifname, const char *IP_Start, const char *IP_End);
#if LWIP_IPV4
extern void set_if(char* netif_name, char* ip_addr, char* gw_addr, char* nm_addr);
extern void get_if(char* ifname, char* ip_addr, char* gw_addr, char* nm_addr);
#endif
extern void get_mac(char* ifname, char* mac);
#if LWIP_IPV6
extern void set_if6(char* netif_name, char* ip6_addr);
extern void get_if6(char* ifname, char* ip6_local, char *ip6_global);
extern void ip6_join_group(char* ifname, char *groupaddr);
extern void ip6_leave_group(char* ifname, char *groupaddr);
#endif
#if LWIP_DNS
extern void set_dns(char numdns, char* dns_server);
extern char *get_dns(char numdns);
#endif

#if LWIP_ROUTE
#if LWIP_IPV4
extern void route4_add(char* ifname, char* ip_addr, char* nm_addr);
extern void route4_delete(char* ip_addr, char* nm_addr);
#endif
#if LWIP_IPV6
extern void route6_add(char* ifname, char* ip6_addr);
extern void route6_delete(char* ip6_addr);
#endif
#endif
#if LWIP_NAPTPT
extern int naptpt_if_set(const char *wan, const char *lan);
#endif
#if LWIP_NAT
extern int nat_if_set(const char *wan, const char *lan);
#endif
extern int lowpan_crypt_set(const char *pan, rt_uint8_t mode, char *key);

#ifdef __cplusplus
    }
#endif      // __cplusplus
    
#endif      // __LWIP_IF_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
