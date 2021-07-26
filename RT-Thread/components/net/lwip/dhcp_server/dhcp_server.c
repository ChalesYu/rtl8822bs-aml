/**
* @file
* lwip dhcp server, must enable LWIP_DHCP to use the dhcp msg.
*
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
* Author: Ren.Haibo <habbyren@qq.com>
*
*/

#include "lwip/opt.h"

#if LWIP_IPV4 && LWIP_DHCP_SERVER /* don't build if not configured for use in lwipopts.h */

#include "lwip/stats.h"
#include "lwip/mem.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/def.h"
#include "lwip/dhcp.h"
#include <lwip/prot/dhcp.h>
#include <lwip/prot/iana.h>
#include "lwip/autoip.h"
#include "lwip/dns.h"
#include "netif/etharp.h"

#include "dhcp_server.h"

#include <string.h>

/** Mac address length  */
#define DHCP_MAX_HLEN                   6
/** dhcp default live time */
#define DHCP_DEFAULT_LIVE_TIME          0x80510100

/** dhcp pbuf len */
#define DHCP_PBUF_LEN                   1024

/** Minimum length for request before packet is parsed */
#define DHCP_MIN_REQUEST_LEN   		44

/**
* The dhcp client node struct.
*/
struct dhcp_client_node
{
  struct dhcp_client_node *next;
  u8_t chaddr[DHCP_MAX_HLEN];
  ip4_addr_t ipaddr;
  u32_t lease_end;
};

/**
* The dhcp server struct.
*/
struct dhcp_server {
  struct dhcp_server *next;
  struct netif *netif;
  struct udp_pcb *pcb;
  struct dhcp_client_node *node_list;
  ip4_addr_t start;
  ip4_addr_t end;
  ip4_addr_t current;
};

/**
* The dhcp server struct list.
*/
static struct dhcp_server *lw_dhcp_server;

/**
* find option from buffer.
*
* @param buf The buffer to find option
* @param len The buffer length
* @param option Which option to find
* @return dhcp option buffer
*/
static u8_t *
dhcp_server_option_find(u8_t *buf, u16_t len, u8_t option)
{
  u8_t *end = buf + len;
  while((buf < end) && (*buf != DHCP_OPTION_END)) {
    if(*buf == option) {
      return buf;
    }
    buf += (buf[1] + 2);
  }
  return NULL;
}

/**
* Find a dhcp client node by mac address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_find_by_mac(struct dhcp_server * dhcpserver, const u8_t *chaddr, u8_t hlen)
{
  struct dhcp_client_node *node;
  
  for(node=dhcpserver->node_list; node != NULL; node = node->next) {
    if(memcmp(node->chaddr, chaddr, hlen) == 0) {
      return node;
    }
  }
  
  return NULL;
}

/**
* Find a dhcp client node by ip address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_find_by_ip(struct dhcp_server * dhcpserver, const ip4_addr_t *ip)
{
  struct dhcp_client_node *node;
  
  for(node=dhcpserver->node_list; node != NULL; node = node->next) {
    if(ip4_addr_cmp(&node->ipaddr, ip)) {
      return node;
    }
  }
  
  return NULL;
}

/**
* Find a dhcp client node by ip address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_alloc(struct dhcp_server * dhcpserver, struct dhcp_msg *msg,
                  u8_t *opt_buf, u16_t len)
{
  u8_t *opt;
  u32_t ipaddr;
  struct dhcp_client_node *node;
  
  node = dhcp_client_find_by_mac(dhcpserver, msg->chaddr, msg->hlen);
  if(node != NULL) {
    return node;
  }
  
  opt = dhcp_server_option_find(opt_buf, len, DHCP_OPTION_REQUESTED_IP);
  if(opt != NULL) {
    node = dhcp_client_find_by_ip(dhcpserver, (ip4_addr_t *)(&opt[2]));
    if(node != NULL) {
      return node;
    }
  }
  
dhcp_alloc_again:
  node = dhcp_client_find_by_ip(dhcpserver, &dhcpserver->current);
  if(node != NULL) {
    ipaddr = (ntohl(dhcpserver->current.addr) + 1);
    if(ipaddr > ntohl(dhcpserver->end.addr)) {
      ipaddr = ntohl(dhcpserver->start.addr);
    }
    dhcpserver->current.addr = htonl(ipaddr);
    goto dhcp_alloc_again;
  }
  node = (struct dhcp_client_node *)mem_malloc(sizeof(struct dhcp_client_node));
  if (node == NULL) {
    return NULL;
  }
  SMEMCPY(node->chaddr, msg->chaddr, msg->hlen);
  node->ipaddr = dhcpserver->current;
  
  node->next = dhcpserver->node_list;
  dhcpserver->node_list = node;
  
  return node;
}

/**
* Find a dhcp client node by ip address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_find(struct dhcp_server * dhcpserver, struct dhcp_msg *msg,
                 u8_t *opt_buf, u16_t len)
{
  u8_t *opt;
  //u32_t ipaddr;
  struct dhcp_client_node *node;
  
  node = dhcp_client_find_by_mac(dhcpserver, msg->chaddr, msg->hlen);
  if(node != NULL) {
    return node;
  }
  
  opt = dhcp_server_option_find(opt_buf, len, DHCP_OPTION_REQUESTED_IP);
  if(opt != NULL) {
    node = dhcp_client_find_by_ip(dhcpserver, (ip4_addr_t *)(&opt[2]));
    if(node != NULL) {
      return node;
    }
  }
  
  return NULL;
}

/**
* If an incoming DHCP message is in response to us, then trigger the state machine
*/
static void
dhcp_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *recv_addr, u16_t port)
{
  struct dhcp_server *dhcp_server = (struct dhcp_server *)arg;
  struct dhcp_msg *msg;
  struct pbuf *q;
  u8_t *opt_buf;
  u8_t *opt;
  struct dhcp_client_node *node;
  u8_t msg_type;
  u16_t length;
  u32_t tmp;
  ip_addr_t addr = *recv_addr;
  
  //LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_server_recv(pbuf = %p) from DHCP client %"U16_F".%"U16_F".%"U16_F".%"U16_F" port %"U16_F"\n", (void*)p,
  //                                          ip4_addr1_16(addr), ip4_addr2_16(addr), ip4_addr3_16(addr), ip4_addr4_16(addr), port));
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("pbuf->len = %"U16_F"\n", p->len));
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("pbuf->tot_len = %"U16_F"\n", p->tot_len));
  /* prevent warnings about unused arguments */
  LWIP_UNUSED_ARG(pcb);
  LWIP_UNUSED_ARG(addr);
  LWIP_UNUSED_ARG(port);
  
  if (p->len < DHCP_MIN_REQUEST_LEN) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("DHCP request message or pbuf too short\n"));
    pbuf_free(p);
    return;
  }
  
  q = pbuf_alloc(PBUF_TRANSPORT, DHCP_PBUF_LEN, PBUF_RAM);
  if(q == NULL) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("pbuf_alloc dhcp_msg failed!\n"));
    pbuf_free(p);
    return;
  }
  if (q->tot_len < p->tot_len) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("pbuf_alloc dhcp_msg too small %d:%d\n", q->tot_len, p->tot_len));
    pbuf_free(p);
    return;
  }
  
  pbuf_copy(q, p);
  pbuf_free(p);
  
  msg = (struct dhcp_msg *)q->payload;
  if (msg->op != DHCP_BOOTREQUEST) {
    //LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("not a DHCP request message, but type %"U16_F"\n", (u16_t)reply_msg->op));
    goto free_pbuf_and_return;
  }
  
  if (msg->cookie != PP_HTONL(DHCP_MAGIC_COOKIE)) {
    goto free_pbuf_and_return;
  }
  
  if(msg->hlen > DHCP_MAX_HLEN) {
    goto free_pbuf_and_return;
  }
  
  opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;
  length = q->tot_len - DHCP_OPTIONS_OFS;
  opt = dhcp_server_option_find(opt_buf, length, DHCP_OPTION_MESSAGE_TYPE);
  if(opt) {
    msg_type = *(opt + 2);
    if(msg_type == DHCP_DISCOVER) {
      node = dhcp_client_alloc(dhcp_server, msg, opt_buf, length);
      if(node == NULL) {
        goto free_pbuf_and_return;
      }
      node->lease_end = DHCP_DEFAULT_LIVE_TIME;
      /* create dhcp offer and send */
      msg->op = DHCP_BOOTREPLY;
      msg->hops = 0;
      msg->secs = 0;
      SMEMCPY(&msg->siaddr, &(dhcp_server->netif->ip_addr), 4);
      msg->sname[0] = '\0';
      msg->file[0] = '\0';
      msg->cookie = PP_HTONL(DHCP_MAGIC_COOKIE);
      SMEMCPY(&msg->yiaddr, &node->ipaddr,4);
      
      opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;
      /* add msg type */
      *opt_buf++ = DHCP_OPTION_MESSAGE_TYPE;
      *opt_buf++ = 1;
      *opt_buf++ = DHCP_OFFER;
      /* add server id */
      *opt_buf++ = DHCP_OPTION_SERVER_ID;
      *opt_buf++ = 4;
      SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
      opt_buf += 4;
      
      /* add_lease_time */
      *opt_buf++ = DHCP_OPTION_LEASE_TIME;
      *opt_buf++ = 4;
      tmp = PP_HTONL(DHCP_DEFAULT_LIVE_TIME);
      SMEMCPY(opt_buf, &tmp, 4);
      opt_buf += 4;
      
      /* add config */
      *opt_buf++ = DHCP_OPTION_SUBNET_MASK;
      *opt_buf++ = 4;
      SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->netmask)->addr, 4);
      opt_buf += 4;
      
      *opt_buf++ = DHCP_OPTION_DNS_SERVER;
      *opt_buf++ = 4;
#if LWIP_DNS
      SMEMCPY(opt_buf, &ip_2_ip4(dns_getserver(0))->addr, 4);      
#else
      /* default use google's dns server */
      tmp = PP_HTONL(0x08080808);
      SMEMCPY(opt_buf, &tmp, 4);
#endif
      opt_buf += 4;
      
      *opt_buf++ = DHCP_OPTION_ROUTER;
      *opt_buf++ = 4;
      SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->ip_addr)->addr, 4);
      opt_buf += 4;
      
      /* add option end */
      *opt_buf++ = DHCP_OPTION_END;
      
      length = (u32_t)opt_buf - (u32_t)msg;
      if(length < q->tot_len) {
        pbuf_realloc(q, length);
      }
#if LWIP_IPV4 && LWIP_IPV6
      addr.u_addr.ip4.addr = INADDR_BROADCAST;
#else 
#if LWIP_IPV4
     addr.addr = INADDR_BROADCAST;
#endif
#endif
      udp_sendto_if(pcb, q, &addr, port, dhcp_server->netif);
    } else {
//      opt = dhcp_server_option_find(opt_buf, length, DHCP_OPTION_SERVER_ID);
//      if (!opt || ip4_addr_cmp((ip4_addr_t*)&opt[2], &dhcp_server->netif->ip_addr)) {
        if (msg_type == DHCP_REQUEST) {
          node = dhcp_client_find(dhcp_server, msg, opt_buf, length);
          if(node != NULL) {
            /* Send ack */
            node->lease_end = DHCP_DEFAULT_LIVE_TIME;
            /* create dhcp offer and send */
            msg->op = DHCP_BOOTREPLY;
            msg->hops = 0;
            msg->secs = 0;
            SMEMCPY(&msg->siaddr, &(dhcp_server->netif->ip_addr), 4);
            msg->sname[0] = '\0';
            msg->file[0] = '\0';
            msg->cookie = PP_HTONL(DHCP_MAGIC_COOKIE);
            SMEMCPY(&msg->yiaddr, &node->ipaddr, 4);
            opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;
            
            /* add msg type */
            *opt_buf++ = DHCP_OPTION_MESSAGE_TYPE;
            *opt_buf++ = 1;
            *opt_buf++ = DHCP_ACK;
            /* add server id */
            *opt_buf++ = DHCP_OPTION_SERVER_ID;
            *opt_buf++ = 4;
            SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
            opt_buf += 4;
            /* add_lease_time */
            *opt_buf++ = DHCP_OPTION_LEASE_TIME;
            *opt_buf++ = 4;
            tmp = PP_HTONL(DHCP_DEFAULT_LIVE_TIME);
            SMEMCPY(opt_buf, &tmp, 4);
            opt_buf += 4;
            /* add config */
            *opt_buf++ = DHCP_OPTION_SUBNET_MASK;
            *opt_buf++ = 4;
            SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->netmask)->addr, 4);
            opt_buf += 4;
            *opt_buf++ = DHCP_OPTION_DNS_SERVER;
            *opt_buf++ = 4;
#if LWIP_DNS
            SMEMCPY(opt_buf, &ip_2_ip4(dns_getserver(0))->addr, 4);      
#else
            /* default use google's dns server */
            tmp = PP_HTONL(0x08080808);
            SMEMCPY(opt_buf, &tmp, 4);
#endif
            opt_buf += 4;
            
            *opt_buf++ = DHCP_OPTION_ROUTER;
            *opt_buf++ = 4;
            SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->ip_addr)->addr, 4);
            opt_buf += 4;
            
            /* add option end */
            *opt_buf++ = DHCP_OPTION_END;
            
            length = (u32_t)opt_buf - (u32_t)msg;
            if(length < q->tot_len) {
              pbuf_realloc(q, length);
            }
#if LWIP_IPV4 && LWIP_IPV6
            addr.u_addr.ip4.addr = INADDR_BROADCAST;
#else 
#if LWIP_IPV4
            addr.addr = INADDR_BROADCAST;
#endif
#endif
            /* ipaddr.addr = INADDR_BROADCAST; */
            udp_sendto_if(pcb, q, &addr, port, dhcp_server->netif);
          } else {
            /* Send no ack */
            /* create dhcp offer and send */
            msg->op = DHCP_BOOTREPLY;
            msg->hops = 0;
            msg->secs = 0;
            SMEMCPY(&msg->siaddr, &(dhcp_server->netif->ip_addr), 4);
            msg->sname[0] = '\0';
            msg->file[0] = '\0';
            msg->cookie = PP_HTONL(DHCP_MAGIC_COOKIE);
            memset(&msg->yiaddr, 0, 4);
            opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;
            
            /* add msg type */
            *opt_buf++ = DHCP_OPTION_MESSAGE_TYPE;
            *opt_buf++ = 1;
            *opt_buf++ = DHCP_NAK;
            /* add server id */
            *opt_buf++ = DHCP_OPTION_SERVER_ID;
            *opt_buf++ = 4;
            SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
            opt_buf += 4;
            
            /* add option end */
            *opt_buf++ = DHCP_OPTION_END;
            length = (u32_t)opt_buf - (u32_t)msg;
            if(length < q->tot_len) {
              pbuf_realloc(q, length);
            }
#if LWIP_IPV4 && LWIP_IPV6
            addr.u_addr.ip4.addr = INADDR_BROADCAST;
#else 
#if LWIP_IPV4
            addr.addr = INADDR_BROADCAST;
#endif
#endif
            udp_sendto_if(pcb, q, &addr, port, dhcp_server->netif);
          }
        } else if (msg_type == DHCP_RELEASE) {
          struct dhcp_client_node *node_prev = NULL;
          
          for(node=dhcp_server->node_list; node != NULL; node = node->next) {
            if(memcmp(node->chaddr, msg->chaddr, msg->hlen) == 0) {
              if(node == dhcp_server->node_list) {
                dhcp_server->node_list = node->next;
              } else {
                node_prev->next = node->next;
              }
              break;
            }
            node_prev = node;
            node= node->next;
          }
          
          if(node != NULL) {
            mem_free(node);
          }
        } else if (msg_type ==  DHCP_DECLINE) {
          ;
        } else if (msg_type == DHCP_INFORM) {
          ;
        }
//      }
    }
  }
  
free_pbuf_and_return:
  pbuf_free(q);
}

/**
* start dhcp server for a netif
*
* @param netif The netif which use dhcp server
* @param start The Start IP address
* @param end The netif which use dhcp server
* @return lwIP error code
* - ERR_OK - No error
* - ERR_MEM - Out of memory
*/
err_t
dhcp_server_start(struct netif *netif, ip4_addr_t *start, ip4_addr_t *end)
{
  struct dhcp_server *dhcp_server;
  
  /* If this netif alreday use the dhcp server. */
  for(dhcp_server = lw_dhcp_server;
      dhcp_server != NULL;
      dhcp_server = dhcp_server->next) {
        if(dhcp_server->netif == netif) {
          dhcp_server->start = *start;
          dhcp_server->end = *end;
          dhcp_server->current = *start;
          return ERR_OK;
        }
      }
  
  dhcp_server = NULL;
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_server_start(): starting new DHCP server\n"));
  dhcp_server = (struct dhcp_server *)mem_malloc(sizeof(struct dhcp_server));
  if (dhcp_server == NULL) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_server_start(): could not allocate dhcp\n"));
    return ERR_MEM;
  }
  
  /* clear data structure */
  memset(dhcp_server, 0, sizeof(struct dhcp_server));
  
  /* store this dhcp server to list */
  dhcp_server->next = lw_dhcp_server;
  lw_dhcp_server = dhcp_server;
  dhcp_server->netif = netif;
  dhcp_server->node_list = NULL;
  dhcp_server->start = *start;
  dhcp_server->end = *end;
  dhcp_server->current = *start;
  
  /* allocate UDP PCB */
  dhcp_server->pcb = udp_new();
  if (dhcp_server->pcb == NULL) {
    LWIP_DEBUGF(DHCP_DEBUG  | LWIP_DBG_TRACE, ("dhcp_server_start(): could not obtain pcb\n"));
    return ERR_MEM;
  }
  
  ip_set_option(dhcp_server->pcb, SOF_BROADCAST);
  /* set up local and remote port for the pcb */
  udp_bind(dhcp_server->pcb, IP_ADDR_ANY, LWIP_IANA_PORT_DHCP_SERVER);
  //udp_connect(dhcp_server->pcb, IP_ADDR_ANY, LWIP_IANA_PORT_DHCP_CLIENT);
  /* set up the recv callback and argument */
  udp_recv(dhcp_server->pcb, dhcp_server_recv, dhcp_server);
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_server_start(): starting DHCP server\n"));
  
  return ERR_OK;
}

/**
* release dhcp server for a netif
*
* @param netif The netif which use dhcp server
*/
void
dhcp_server_release(struct netif *netif)
{
  struct dhcp_server *dhcp_server;
  struct dhcp_server *dhcp_server_prev = NULL;
  
  /* If this netif alreday use the dhcp server. */
  for(dhcp_server = lw_dhcp_server; dhcp_server != NULL;) {
    if(dhcp_server->netif == netif) {
      if(dhcp_server == lw_dhcp_server) {
        lw_dhcp_server = dhcp_server->next;
      } else {
        dhcp_server_prev->next = dhcp_server->next;
      }
      break;
    }
    dhcp_server_prev = dhcp_server;
    dhcp_server= dhcp_server->next;
  }
  
  if(dhcp_server == NULL) {
    return;
  }
  
  /* delete the udp pcb */
  if(dhcp_server->pcb) {
    udp_remove(dhcp_server->pcb);
  }
  /* free the buffer  */
  mem_free(dhcp_server);
}
#endif /* LWIP_DHCP_SERVER */
