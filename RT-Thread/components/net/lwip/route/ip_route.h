/**
 * @file
 * lwip route list module
 * This is a sample route list implementation, which is supported to ipv4 and ipv6.
 */
/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Ren.Haibo <habbyren@qq.com>
 *
 */
#ifndef __LWIP_ROUTE_H__
#define __LWIP_ROUTE_H__

#include "lwip/opt.h"

#if LWIP_ROUTE /* don't build if not configured for use in lwipopts.h */

#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/ip.h"
#include "lwip/ip6_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The following functions is the application layer interface to the
   route code. */
#if LWIP_IPV4
err_t  route_ip4_add(ip4_addr_t *ip_addr, ip4_addr_t *netmask, struct netif *inp);
err_t  route_ip4_delete(ip4_addr_t *ip_addr, ip4_addr_t *netmask);
struct netif * route_ip4_find(ip4_addr_t *ip_addr);
#endif


#if LWIP_IPV6
err_t  route_ip6_add(ip6_addr_t *ip6_addr, struct netif *inp);
err_t  route_ip6_delete(ip6_addr_t *ip6_addr);
struct netif *route_ip6_find(ip6_addr_t *ip6_addr);
#endif  /* LWIP_IPV6 */


#ifdef __cplusplus
}
#endif

#endif /* LWIP_UDP */

#endif /* __LWIP_UDP_H__ */
