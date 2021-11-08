/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwipopts.h
** Last modified Date:  2015-01-04
** Last Version:        v1.00
** Description:         lwip的配置文件
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-01-04
** Version:             v1.00
** Descriptions:
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
*********************************************************************************************************/
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/*********************************************************************************************************
** 首先包含rt thread的总的配置文件
*********************************************************************************************************/
#include <rtconfig.h>

/*********************************************************************************************************
** 编译器相关配置
*********************************************************************************************************/
//#define LWIP_NO_STDINT_H        1
#define BYTE_ORDER              LITTLE_ENDIAN

/*********************************************************************************************************
** 系统平台基本配置
*********************************************************************************************************/
#define NO_SYS                   	0             // 需要操作系统支持
#define LWIP_TIMERS                     !NO_SYS        // 使用lwip内部提供的定时功能
#define LWIP_TIMERS_CUSTOM              !LWIP_TIMERS  //　用户提供定时功能

// 下面是C库提供的函数支持，如果有效率更高的实现，可以定义并替换该宏的值
//#define MEMCPY(dst,src,len)             memcpy(dst,src,len)
//#define SMEMCPY(dst,src,len)            memcpy(dst,src,len)
//#define MEMMOVE(dst,src,len)            memmove(dst,src,len)

#include <stdlib.h>
#define LWIP_RAND                      rand

/*********************************************************************************************************
** Core locking
*********************************************************************************************************/
#define LWIP_MPU_COMPATIBLE             0            // 如果有MPU支持，可以使能该定义
#define LWIP_TCPIP_CORE_LOCKING         1            // lwip内核锁
#define LWIP_TCPIP_CORE_LOCKING_INPUT   0            // 接收锁
#define SYS_LIGHTWEIGHT_PROT           (NO_SYS==0)   // 是否使能系统临界区保护，操作系统下必须使能

/*********************************************************************************************************
** 内存管理设定参数
*********************************************************************************************************/
//　使用ｃ库函数malloc、free分配内存
#ifdef RT_LWIP_USING_RT_MEM
#define MEM_LIBC_MALLOC                 1
#define MEMP_MEM_MALLOC                 1
#else
#define MEM_LIBC_MALLOC                 0    
#define MEMP_MEM_MALLOC                 0
#endif

// 特殊定义，规定内存使用的地址，如果使用的STM32F407,则使用ccm ram
#define LWIP_RAM_HEAP_POINTER          0x20000000

// 使用lwip内部的内存分配算法
#define MEMP_MEM_INIT                   0
#define MEM_ALIGNMENT                   4    // 内存对齐方式，和CPU有关，ARM一般为4
#define MEM_SIZE                        (122 * 1024) // default is 1600
#define MEMP_OVERFLOW_CHECK             0
#define MEMP_SANITY_CHECK               0
#define MEM_SANITY_CHECK                0
// 用户自定义pool分配算法定义
#define MEM_USE_POOLS                   0
#define MEM_USE_POOLS_TRY_BIGGER_POOL   0
#define MEMP_USE_CUSTOM_POOLS           0

#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 1

/*********************************************************************************************************
** lwip内部pool分配相关定义
*********************************************************************************************************/
#define MEMP_NUM_PBUF                   32    // 最大的用于PBUF_ROM and PBUF_REF的pbuf数量
#define MEMP_NUM_RAW_PCB                16
#define MEMP_NUM_UDP_PCB                16
#define MEMP_NUM_TCP_PCB                16
#define MEMP_NUM_TCP_PCB_LISTEN         16
#define MEMP_NUM_TCP_SEG                128 //128 //16    iperf测速时可以配置大，配置为128
#define MEMP_NUM_ALTCP_PCB              MEMP_NUM_TCP_PCB
#define MEMP_NUM_REASSDATA              5   //每个重组包允许的最大分配数量
#define MEMP_NUM_FRAG_PBUF              15  //每个分片包允许的最大分配数量
#define MEMP_NUM_ARP_QUEUE              30
#define MEMP_NUM_IGMP_GROUP             8
//　timeout定时器数量定义
#define LWIP_NUM_SYS_TIMEOUT_INTERNAL_USE   (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + \
                                         LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_NUM_TIMEOUTS + \
                                         (LWIP_IPV6 * (1 + LWIP_IPV6_REASS + LWIP_IPV6_MLD))+\
                                         + 24)
#define MEMP_NUM_SYS_TIMEOUT            LWIP_NUM_SYS_TIMEOUT_INTERNAL_USE
#define MEMP_NUM_NETBUF                 24
#define MEMP_NUM_NETCONN                32
#define MEMP_NUM_SELECT_CB              16
#define MEMP_NUM_TCPIP_MSG_API          16
#define MEMP_NUM_TCPIP_MSG_INPKT        16
#define MEMP_NUM_NETDB                  3
#define MEMP_NUM_LOCALHOSTLIST          2
#define PBUF_POOL_SIZE                  32
//#define MEMP_NUM_API_MSG                MEMP_NUM_TCPIP_MSG_API
//#define MEMP_NUM_DNS_API_MSG            MEMP_NUM_TCPIP_MSG_API
//#define MEMP_NUM_SOCKET_SETGETSOCKOPT_DATA MEMP_NUM_TCPIP_MSG_API
//#define MEMP_NUM_NETIFAPI_MSG           MEMP_NUM_TCPIP_MSG_API

/*********************************************************************************************************
** ARP相关配置
*********************************************************************************************************/
#define LWIP_ARP                        1
#define ARP_TABLE_SIZE                  10
#define ARP_MAXAGE                      300  //　单位为秒
#define ARP_QUEUEING                    0
#define ARP_QUEUE_LEN                   3
#define ETHARP_SUPPORT_VLAN             0
#define LWIP_ETHERNET                   LWIP_ARP

//  很重要，6lowapn协议栈要配置大一些
#define ETH_PAD_SIZE                    0
#define ETHARP_SUPPORT_STATIC_ENTRIES   0
//#define ETHARP_TABLE_MATCH_NETIF        !LWIP_SINGLE_NETIF

/*********************************************************************************************************
** IP协议参数的设定——IP options
*********************************************************************************************************/
#define LWIP_IPV4                       1
#define IP_FORWARD                      1
#define IP_REASSEMBLY                   1
#define IP_FRAG                         1
#define IP_OPTIONS_ALLOWED              1

#define IP_REASS_MAXAGE                 15
#define IP_REASS_MAX_PBUFS              10
#define IP_DEFAULT_TTL                  255
#define IP_SOF_BROADCAST                0
#define IP_SOF_BROADCAST_RECV           0
// aodvh和6lowpan rpl路由协议需要定义为1
#define IP_FORWARD_ALLOW_TX_ON_RX_NETIF 0

/*********************************************************************************************************
** ICMP协议参数的设定——ICMP options
*********************************************************************************************************/
#define LWIP_ICMP                       1
//#define ICMP_TTL                       (IP_DEFAULT_TTL)
//#define LWIP_BROADCAST_PING             0
//#define LWIP_MULTICAST_PING             0

/*********************************************************************************************************
** RAW API相关定义
*********************************************************************************************************/
#define LWIP_RAW                       1
//#define RAW_TTL                       (IP_DEFAULT_TTL)

/*********************************************************************************************************
** DHCP参数配置——DHCP options
*********************************************************************************************************/
#define LWIP_DHCP                   	1
//#define DHCP_DOES_ARP_CHECK             ((LWIP_DHCP) && (LWIP_ARP))
#define LWIP_DHCP_CHECK_LINK_UP         0
#define LWIP_DHCP_BOOTP_FILE            0
#define LWIP_DHCP_GET_NTP_SRV           0
#define LWIP_DHCP_MAX_NTP_SERVERS       1
#define LWIP_DHCP_MAX_DNS_SERVERS       2 // DNS_MAX_SERVERS

/*********************************************************************************************************
** AUTOIP参数配置——AUTOIP options
*********************************************************************************************************/
#define LWIP_AUTOIP                     0
#define LWIP_DHCP_AUTOIP_COOP           0
#define LWIP_DHCP_AUTOIP_COOP_TRIES     9

/*********************************************************************************************************
** SNMP参数配置——SNMP options
*********************************************************************************************************/
//#define LWIP_MIB2_CALLBACKS             0

/*********************************************************************************************************
** IGMP参数配置——Multicast options 
*********************************************************************************************************/
//#define LWIP_MULTICAST_TX_OPTIONS       ((LWIP_IGMP || LWIP_IPV6_MLD) && (LWIP_UDP || LWIP_RAW))

/*********************************************************************************************************
** IGMP参数配置——IGMP options
*********************************************************************************************************/
#define LWIP_IGMP                       1

/*********************************************************************************************************
** DNS参数配置——DNS options
*********************************************************************************************************/
#define LWIP_DNS                        1
//#define DNS_TABLE_SIZE                  4
//#define DNS_MAX_NAME_LENGTH             256
//#define DNS_MAX_SERVERS                 2
//#define DNS_DOES_NAME_CHECK             1
////#define LWIP_DNS_SECURE (LWIP_DNS_SECURE_RAND_XID | LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT)
//#define DNS_LOCAL_HOSTLIST              0
//#define DNS_LOCAL_HOSTLIST_IS_DYNAMIC   0
//#define LWIP_DNS_SUPPORT_MDNS_QUERIES  0

/*********************************************************************************************************
** UDP参数配置——UDP options
*********************************************************************************************************/
#define LWIP_UDP                        1
//#define LWIP_UDPLITE                    0
//#define UDP_TTL                         (IP_DEFAULT_TTL)
#define LWIP_NETBUF_RECVINFO            1

/*********************************************************************************************************
** TCP参数配置——TCP options
*********************************************************************************************************/
#if 0
#define LWIP_TCP                        1
//#define TCP_TTL                         (IP_DEFAULT_TTL)
#define TCP_WND                         (8 * TCP_MSS) // (PBUF_POOL_SIZE * (PBUF_POOL_BUFSIZE - \
                                         (PBUF_LINK_HLEN + PBUF_IP_HLEN + PBUF_TRANSPORT_HLEN)))
#define TCP_MAXRTX                      12
#define TCP_SYNMAXRTX                   6
#define TCP_QUEUE_OOSEQ                 (LWIP_TCP)
#define LWIP_TCP_SACK_OUT               0
#define LWIP_TCP_MAX_SACK_NUM           4
#define TCP_MSS                         1500
#define TCP_CALCULATE_EFF_SEND_MSS      1
#define TCP_SND_BUF                     4096
//#define TCP_SND_QUEUELEN                ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))
//#define TCP_SNDLOWAT                    LWIP_MIN(LWIP_MAX(((TCP_SND_BUF)/2), (2 * TCP_MSS) + 1), (TCP_SND_BUF) - 1)
//#define TCP_SNDQUEUELOWAT               LWIP_MAX(((TCP_SND_QUEUELEN)/2), 5)
#define TCP_OOSEQ_MAX_BYTES             0
#define TCP_OOSEQ_MAX_PBUFS             0
#define TCP_LISTEN_BACKLOG              1
#define TCP_DEFAULT_LISTEN_BACKLOG      0xff
//#define TCP_OVERSIZE                    TCP_MSS
#define LWIP_TCP_TIMESTAMPS             0
//#define TCP_WND_UPDATE_THRESHOLD        LWIP_MIN((TCP_WND / 4), (TCP_MSS * 4))
#else    // 下面的配置需要更大的RAM，在iperf测速时可以使用
#define LWIP_TCP                        1
#define TCP_QUEUE_OOSEQ                 LWIP_TCP
#define TCP_MSS                         1500
#define TCP_SND_BUF                     (10*TCP_MSS)
#define TCP_SND_QUEUELEN                ((8 * TCP_SND_BUF)/TCP_MSS)
#define TCP_WND                         (11*TCP_MSS)
#endif

// 下面2个只能有一个为1
#define LWIP_EVENT_API                  0
#define LWIP_CALLBACK_API               1

#define LWIP_WND_SCALE                  0
#define TCP_RCV_SCALE                   0
#define LWIP_ALTCP                      0
#define LWIP_ALTCP_TLS                  0

/*********************************************************************************************************
** pbuf options
*********************************************************************************************************/
//#if defined LWIP_HOOK_VLAN_SET
//#define PBUF_LINK_HLEN                  (18 + ETH_PAD_SIZE)
//#else /* LWIP_HOOK_VLAN_SET */
#define PBUF_LINK_HLEN                  (14 + ETH_PAD_SIZE)
//#endif /* LWIP_HOOK_VLAN_SET */

#define PBUF_POOL_BUFSIZE                1580

/* LWIP_SUPPORT_CUSTOM_PBUF == 1: to pass directly MAC Rx buffers to the stack, no copy is needed */
#define LWIP_SUPPORT_CUSTOM_PBUF      1

//#define PBUF_LINK_ENCAPSULATION_HLEN    0
//#define LWIP_PBUF_REF_T                 u8_t

/*********************************************************************************************************
** Network Interfaces options
*********************************************************************************************************/
#define LWIP_SINGLE_NETIF               0  // 必须设置为0，否则系统框架将改变
#define LWIP_NETIF_HOSTNAME             0
#define LWIP_NETIF_API                  1
#define LWIP_NETIF_STATUS_CALLBACK      0
#define LWIP_NETIF_EXT_STATUS_CALLBACK  0
#define LWIP_NETIF_LINK_CALLBACK        0
#define LWIP_NETIF_REMOVE_CALLBACK      0
#define LWIP_NETIF_HWADDRHINT           1

#define LWIP_NETIF_TX_SINGLE_PBUF       0
#define LWIP_NUM_NETIF_CLIENT_DATA      0

/*********************************************************************************************************
** LOOPIF options
*********************************************************************************************************/
#define LWIP_NETIF_LOOPBACK             1
#define LWIP_LOOPIF_MULTICAST           0
#define LWIP_LOOPBACK_MAX_PBUFS         4

/*********************************************************************************************************
** PPP options
*********************************************************************************************************/
//#define PPP_SUPPORT                     1
//
////#define PPPOE_SUPPORT                   0
////#define PPPOL2TP_SUPPORT                0
//#define PPPOS_SUPPORT                   PPP_SUPPORT
////#define PPP_IPV6_SUPPORT               1
////#define IPV6CP_COMP                    1
//
//#if (PBUF_POOL_BUFSIZE >= 512)
//#define PPP_USE_PBUF_RAM                0
//#else
//#define PPP_USE_PBUF_RAM                1     // ppp内存分配从堆中分配。ppp缓存必须是一个缓冲区，不能链式
//#endif
//
////#define PPP_INPROC_IRQ_SAFE             0
//
//#define PAP_SUPPORT                 1      /* Set > 0 for PAP. */
//#define CHAP_SUPPORT                1      /* Set > 0 for CHAP. */
//#define MSCHAP_SUPPORT              1      /* Set > 0 for MSCHAP. */
//#define EAP_SUPPORT                 1      /* Set > 0 for EAP */
//#define CCP_SUPPORT                 1      /* Set > 0 for CCP */
//#define MPPE_SUPPORT                1      /* Set > 0 for CBCP  */
//#define CBCP_SUPPORT                0      /* CURRENTLY NOT SUPPORTED! DO NOT SET! */
//#define ECP_SUPPORT                 0      /* CURRENTLY NOT SUPPORTED! DO NOT SET! */
////#define DEMAND_SUPPORT              0      /* CURRENTLY NOT SUPPORTED! DO NOT SET! */
////#define LQR_SUPPORT                 0
//#define VJ_SUPPORT                  1      /* Set > 0 for VJ header compression. */
//
//
//// FSM超时参数配置
//#define FSM_DEFTIMEOUT          10      // FSM_DEFTIMEOUT: Timeout time in seconds
//#define FSM_DEFMAXTERMREQS      5       // FSM_DEFMAXTERMREQS: Maximum Terminate-Request transmissions
//#define FSM_DEFMAXCONFREQS      10      // FSM_DEFMAXCONFREQS: Maximum Configure-Request transmissions
//#define FSM_DEFMAXNAKLOOPS      5      // FSM_DEFMAXNAKLOOPS: Maximum number of nak loops
//#define UPAP_DEFTIMEOUT         10      // UPAP_DEFTIMEOUT: Timeout (seconds) for retransmitting req
//#define UPAP_DEFTRANSMITS       10      //Maximum number of auth-reqs to send
//
//
//#define PRINTPKT_SUPPORT            0

/*********************************************************************************************************
** Thread options
*********************************************************************************************************/
#define TCPIP_THREAD_NAME              "tcpip"
#define TCPIP_THREAD_STACKSIZE          3072
#define TCPIP_THREAD_PRIO               1
#define TCPIP_MBOX_SIZE                 32
#define LWIP_TCPIP_THREAD_ALIVE()
#define SLIPIF_THREAD_NAME             "slipif"
#define SLIPIF_THREAD_STACKSIZE         1024
#define SLIPIF_THREAD_PRIO              2
// 其它线程
#define DEFAULT_THREAD_NAME            "lwIP"
#define DEFAULT_THREAD_STACKSIZE        1024
#define DEFAULT_RAW_RECVMBOX_SIZE       32
#define DEFAULT_UDP_RECVMBOX_SIZE       32
#define DEFAULT_TCP_RECVMBOX_SIZE       32
#define DEFAULT_ACCEPTMBOX_SIZE         32

/*********************************************************************************************************
** Sequential API参数设定——Sequential layer options
*********************************************************************************************************/
#define LWIP_NETCONN                    1
#define LWIP_TCPIP_TIMEOUT              1
//#define LWIP_NETCONN_SEM_PER_THREAD     0
//#define LWIP_NETCONN_FULLDUPLEX         0

/*********************************************************************************************************
** Socket参数设定——Socket Options
*********************************************************************************************************/
#define LWIP_SOCKET                     1
#ifdef RT_USING_POSIX
#define LWIP_COMPAT_SOCKETS             0
#define LWIP_POSIX_SOCKETS_IO_NAMES     0
#else
#define LWIP_COMPAT_SOCKETS             1
#define LWIP_POSIX_SOCKETS_IO_NAMES     0
#endif
#define LWIP_SOCKET_OFFSET              0
#define LWIP_TCP_KEEPALIVE              1
#define LWIP_SO_SNDTIMEO                1
#define LWIP_SO_RCVTIMEO                1
//#define LWIP_SO_SNDRCVTIMEO_NONSTANDARD 0
#define LWIP_SO_RCVBUF                  1
#define LWIP_SO_LINGER                  1
//#define RECV_BUFSIZE_DEFAULT            INT_MAX
//#define LWIP_TCP_CLOSE_TIMEOUT_MS_DEFAULT 20000
#define SO_REUSE                        1
#define SO_REUSE_RXTOALL                1
//#define LWIP_FIONREAD_LINUXMODE         0
#define LWIP_SOCKET_SELECT              1
#define LWIP_SOCKET_POLL                0

/*********************************************************************************************************
** Statistics options
*********************************************************************************************************/
#define LWIP_STATS                      0

#if LWIP_STATS

#define LWIP_STATS_DISPLAY              0
#define LINK_STATS                      1
#define ETHARP_STATS                    (LWIP_ARP)
#define IP_STATS                        1
#define IPFRAG_STATS                    (IP_REASSEMBLY || IP_FRAG)
#define ICMP_STATS                      1
#define IGMP_STATS                      (LWIP_IGMP)
#define UDP_STATS                       (LWIP_UDP)
#define TCP_STATS                       (LWIP_TCP)
#define MEM_STATS                       ((MEM_LIBC_MALLOC == 0) && (MEM_USE_POOLS == 0))
#define MEMP_STATS                      (MEMP_MEM_MALLOC == 0)
#define SYS_STATS                       (NO_SYS == 0)
#define IP6_STATS                       (LWIP_IPV6)
#define ICMP6_STATS                     (LWIP_IPV6 && LWIP_ICMP6)
#define IP6_FRAG_STATS                  (LWIP_IPV6 && (LWIP_IPV6_FRAG || LWIP_IPV6_REASS))
#define MLD6_STATS                      (LWIP_IPV6 && LWIP_IPV6_MLD)
#define ND6_STATS                       (LWIP_IPV6)
#define MIB2_STATS                      0

#else

#define LINK_STATS                      0
#define ETHARP_STATS                    0
#define IP_STATS                        0
#define IPFRAG_STATS                    0
#define ICMP_STATS                      0
#define IGMP_STATS                      0
#define UDP_STATS                       0
#define TCP_STATS                       0
#define MEM_STATS                       0
#define MEMP_STATS                      0
#define SYS_STATS                       0
#define LWIP_STATS_DISPLAY              0
#define IP6_STATS                       0
#define ICMP6_STATS                     0
#define IP6_FRAG_STATS                  0
#define MLD6_STATS                      0
#define ND6_STATS                       0
#define MIB2_STATS                      0

#endif /* LWIP_STATS */

/*********************************************************************************************************
**  校验和参数配置——checksum options
*********************************************************************************************************/
#define LWIP_CHECKSUM_CTRL_PER_NETIF    0
#define CHECKSUM_BY_HARDWARE            0

#if (CHECKSUM_BY_HARDWARE > 0)
#define CHECKSUM_GEN_IP                 0
#define CHECKSUM_GEN_UDP                0
#define CHECKSUM_GEN_TCP                0
#define CHECKSUM_GEN_ICMP               0
#define CHECKSUM_GEN_ICMP6              0
#define CHECKSUM_CHECK_IP               0
#define CHECKSUM_CHECK_UDP              0
#define CHECKSUM_CHECK_TCP              0
#define CHECKSUM_CHECK_ICMP             0
#define CHECKSUM_CHECK_ICMP6            0
#else
#define CHECKSUM_GEN_IP                 1
#define CHECKSUM_GEN_UDP                1
#define CHECKSUM_GEN_TCP                1
#define CHECKSUM_GEN_ICMP               1
#define CHECKSUM_GEN_ICMP6              1
#define CHECKSUM_CHECK_IP               1
#define CHECKSUM_CHECK_UDP              1
#define CHECKSUM_CHECK_TCP              1
#define CHECKSUM_CHECK_ICMP             1
#define CHECKSUM_CHECK_ICMP6            1
#endif

#define LWIP_CHECKSUM_ON_COPY           0

/*********************************************************************************************************
** IPv6 options
*********************************************************************************************************/
#define LWIP_IPV6                       0

#define IPV6_REASS_MAXAGE               60
#define LWIP_IPV6_SCOPES                (LWIP_IPV6 && !LWIP_SINGLE_NETIF)
#define LWIP_IPV6_SCOPES_DEBUG          0
#define LWIP_IPV6_NUM_ADDRESSES         3
#define LWIP_IPV6_FORWARD               1
#define LWIP_IPV6_FRAG                  1
#define LWIP_IPV6_REASS                 (LWIP_IPV6)
#define LWIP_IPV6_SEND_ROUTER_SOLICIT   1
#define LWIP_IPV6_AUTOCONFIG            (LWIP_IPV6)
#define LWIP_IPV6_ADDRESS_LIFETIMES     (LWIP_IPV6_AUTOCONFIG)
#define LWIP_IPV6_DUP_DETECT_ATTEMPTS   1
//#define LWIP_ICMP6                      (LWIP_IPV6)
//#define LWIP_ICMP6_DATASIZE             8
//#define LWIP_ICMP6_HL                   255
//#define LWIP_IPV6_MLD                   (LWIP_IPV6)
//#define MEMP_NUM_MLD6_GROUP             4
//#define LWIP_ND6_QUEUEING               (LWIP_IPV6)
//#define MEMP_NUM_ND6_QUEUE              20
//#define LWIP_ND6_NUM_NEIGHBORS          10
//#define LWIP_ND6_NUM_DESTINATIONS       10
//#define LWIP_ND6_NUM_PREFIXES           5
//#define LWIP_ND6_NUM_ROUTERS            3
//#define LWIP_ND6_MAX_MULTICAST_SOLICIT  3
//#define LWIP_ND6_MAX_UNICAST_SOLICIT    3
//#define LWIP_ND6_MAX_ANYCAST_DELAY_TIME 1000
//#define LWIP_ND6_MAX_NEIGHBOR_ADVERTISEMENT  3
//#define LWIP_ND6_REACHABLE_TIME         30000
//#define LWIP_ND6_RETRANS_TIMER          1000
//#define LWIP_ND6_DELAY_FIRST_PROBE_TIME 5000
//#define LWIP_ND6_ALLOW_RA_UPDATES       1
//#define LWIP_ND6_TCP_REACHABILITY_HINTS 1
//#define LWIP_ND6_RDNSS_MAX_DNS_SERVERS  0

#define LWIP_IPV6_DHCP6                 0

/*********************************************************************************************************
** 定义最大的mac地址长度
*********************************************************************************************************/
#define NETIF_MAX_HWADDR_LEN            6U

/*********************************************************************************************************
** Hook options
*********************************************************************************************************/
//#define LWIP_HOOK_FILENAME "path/to/my/lwip_hooks.h"

//#define LWIP_HOOK_TCP_ISN(local_ip, local_port, remote_ip, remote_port)
//#define LWIP_HOOK_IP4_INPUT(pbuf, input_netif)
//#define LWIP_HOOK_IP4_ROUTE()
//#define LWIP_HOOK_IP4_ROUTE_SRC(src, dest)
//#define LWIP_HOOK_ETHARP_GET_GW(netif, dest)
//#define LWIP_HOOK_IP6_INPUT(pbuf, input_netif)
//#define LWIP_HOOK_IP6_ROUTE(src, dest)
//#define LWIP_HOOK_ND6_GET_GW(netif, dest)
//#define LWIP_HOOK_VLAN_CHECK(netif, eth_hdr, vlan_hdr)
//#define LWIP_HOOK_VLAN_SET(netif, p, src, dst, eth_type)
//#define LWIP_HOOK_MEMP_AVAILABLE(memp_t_type)
//#define LWIP_HOOK_UNKNOWN_ETH_PROTOCOL(pbuf, netif)
//#define LWIP_HOOK_DHCP_APPEND_OPTIONS(netif, dhcp, state, msg, msg_type, options_len_ptr)
//#define LWIP_HOOK_DHCP_PARSE_OPTION(netif, dhcp, state, msg, msg_type, option, len, pbuf, offset)

/*********************************************************************************************************
**  调试参数配置——Debugging options
*********************************************************************************************************/
#if  0
#define LWIP_DEBUG
#endif

//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_OFF
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_WARNING
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SERIOUS
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SEVERE

#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON
#define ETHARP_DEBUG                    LWIP_DBG_OFF
#define NETIF_DEBUG                     LWIP_DBG_OFF
#define PBUF_DEBUG                      LWIP_DBG_OFF
#define API_LIB_DEBUG                   LWIP_DBG_OFF
#define API_MSG_DEBUG                   LWIP_DBG_OFF
#define SOCKETS_DEBUG                   LWIP_DBG_OFF
#define ICMP_DEBUG                      LWIP_DBG_OFF
#define IGMP_DEBUG                      LWIP_DBG_OFF
#define INET_DEBUG                      LWIP_DBG_OFF
#define IP_DEBUG                        LWIP_DBG_OFF
#define IP_REASS_DEBUG                  LWIP_DBG_OFF
#define RAW_DEBUG                       LWIP_DBG_OFF
#define MEM_DEBUG                       LWIP_DBG_OFF
#define MEMP_DEBUG                      LWIP_DBG_OFF
#define SYS_DEBUG                       LWIP_DBG_OFF
#define TIMERS_DEBUG                    LWIP_DBG_OFF
#define TCP_DEBUG                       LWIP_DBG_OFF
#define TCP_INPUT_DEBUG                 LWIP_DBG_OFF
#define TCP_FR_DEBUG                    LWIP_DBG_OFF
#define TCP_RTO_DEBUG                   LWIP_DBG_OFF
#define TCP_CWND_DEBUG                  LWIP_DBG_OFF
#define TCP_WND_DEBUG                   LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG                LWIP_DBG_OFF
#define TCP_RST_DEBUG                   LWIP_DBG_OFF
#define TCP_QLEN_DEBUG                  LWIP_DBG_OFF
#define UDP_DEBUG                       LWIP_DBG_OFF
#define TCPIP_DEBUG                     LWIP_DBG_OFF
#define SLIP_DEBUG                      LWIP_DBG_OFF
#define DHCP_DEBUG                      LWIP_DBG_OFF
#define AUTOIP_DEBUG                    LWIP_DBG_OFF
#define DNS_DEBUG                       LWIP_DBG_OFF
#define IP6_DEBUG                       LWIP_DBG_OFF
#define PPP_DEBUG                       LWIP_DBG_OFF
#define LWIP_NAT_DEBUG                  LWIP_DBG_OFF

/*********************************************************************************************************
** Performance tracking options
*********************************************************************************************************/
//#define LWIP_PERF                       0

/*********************************************************************************************************
** 自开发协议栈相关配置
*********************************************************************************************************/

/*********************************************************************************************************
** HOOK函数头文件声明
*********************************************************************************************************/
#define LWIP_HOOK_FILENAME                    "lwip_hook.h"

/*********************************************************************************************************
** IP Route功能
*********************************************************************************************************/
#define LWIP_ROUTE                      1     // 使能支持路由表功能
#if LWIP_ROUTE
#define LWIP_HOOK_IP4_ROUTE(a)                  route_ip4_find((ip4_addr_t *)a)          
#define LWIP_HOOK_IP6_ROUTE(IP6_ADDR_ANY6, a)   route_ip6_find((ip6_addr_t *)a)
#endif

/*********************************************************************************************************
** DHCP服务器配置
*********************************************************************************************************/
#define LWIP_DHCP_SERVER                1
#define DHCP_OPTIONS_LEN                300  // 必须配置足够发送DHCP消息

/*********************************************************************************************************
** RPL路由协议使能配置, 必须同时使能IPv6
*********************************************************************************************************/
#define LWIP_RPL                        0
#define RPL_TIMER_PRECISION             40
#define RPL_STATS                       0
#define RPL_DEFAULT_LIFETIME_UNIT       20
#define RPL_DEFAULT_LIFETIME            60
//#define RPL_DAG_MC                      RPL_DAG_MC_NONE
//#define RPL_MOP_DEFAULT                 RPL_MOP_STORING_NO_MULTICAST
//#define RPL_RPLIF_DEBUG                 LWIP_DBG_ON
//#define RPL_TIMERCB_DEBUG               LWIP_DBG_ON
//#define RPL_ROUTE_DEBUG                 LWIP_DBG_ON
//#define RPL_PROTO_DEBUG                 LWIP_DBG_ON
//#define RPL_PARENT_DEBUG                LWIP_DBG_ON
//#define RPL_OF_DEBUG                    LWIP_DBG_ON
//#define RPL_DODAG_DEBUG                 LWIP_DBG_ON
//#define RPL_ROUTE_PRINT                  LWIP_DBG_ON
//#define RPL_PARENT_PRINT                 LWIP_DBG_ON

/*********************************************************************************************************
** nat配置,必须支持IPv4
*********************************************************************************************************/
#define LWIP_NAT                   0
#if (LWIP_NAT > 0)
#define NAT_MAX_ENTRY              1
#define LWIP_NAT_MALLOC            rt_malloc
#define LWIP_NAT_FREE              rt_free
#define NAT_ICMP_MAP_ID_START      10000
#define NAT_ICMP_MAP_ID_NUM        20
#define NAT_UDP_MAP_PORT_START     10000
#define NAT_UDP_MAP_PORT_NUM       100
#define NAT_TCP_MAP_PORT_START     10000
#define NAT_TCP_MAP_PORT_NUM       100
#define NAT_ICMP_TTL               3
#define NAT_UDP_TTL                8         
#define NAT_SYN_TTL                2          
#define NAT_RST_TTL                2           
#define NAT_TCP_TTL                8    
#endif

/*********************************************************************************************************
** naptpt配置,必须同时支持IPv4和IPv6
*********************************************************************************************************/
#define LWIP_NAPTPT                  0
#if (LWIP_NAPTPT > 0)
#define NAPTPT_ICMP_TTL              15
#define NAPTPT_UDP_TTL               60*3          
#define NAPTPT_SYN_TTL               15          
#define NAPTPT_RST_TTL               15           
#define NAPTPT_TCP_TTL               60*3    
#endif


#endif /* __LWIPOPTS_H__ */
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
