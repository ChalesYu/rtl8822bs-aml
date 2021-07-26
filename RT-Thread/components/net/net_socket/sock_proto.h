/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           sock_proto.h
** Last modified Date:  2019-10-11
** Last Version:        v1.00
** Description:         sock proto功能实现
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2019-10-11
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#ifndef __SOCK_PROTO_H__
#define __SOCK_PROTO_H__

#ifdef __cplusplus
extern "C" {
#endif
  
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <lwip/sockets.h>


#define DBG_SECTION_NAME	"SOCK"
#define DBG_LEVEL		 DBG_INFO
#include <rtdbg.h>
  
#ifndef SOCK_PROTO_NUM   
#define SOCK_PROTO_NUM     2
#endif

/*********************************************************************************************************
**  sock proto协议结构
*********************************************************************************************************/
struct sock_proto_ops {
    int family;
    int (*socket)     (int domain, int type, int protocol);
    int (*closesocket)(int s);
    int (*bind)       (int s, const struct sockaddr *name, socklen_t namelen);
    int (*listen)     (int s, int backlog);
    int (*connect)    (int s, const struct sockaddr *name, socklen_t namelen);
    int (*accept)     (int s, struct sockaddr *addr, socklen_t *addrlen);
    int (*sendto)     (int s, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
    int (*recvfrom)   (int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
    int (*send)       (int s, const void *dataptr, size_t size, int flags);
    int (*recv)       (int s, void *mem, size_t len, int flags);
    int (*sendmsg)    (int s, const struct msghdr *message, int flags);
    int (*recvmsg)    (int s, struct msghdr *message, int flags);
    int (*getsockopt) (int s, int level, int optname, void *optval, socklen_t *optlen);
    int (*setsockopt) (int s, int level, int optname, const void *optval, socklen_t optlen);
    int (*shutdown)   (int s, int how);
    int (*getpeername)(int s, struct sockaddr *name, socklen_t *namelen);
    int (*getsockname)(int s, struct sockaddr *name, socklen_t *namelen);
    int (*ioctlsocket)(int s, long cmd, void *arg);
    int (*poll)       (struct dfs_fd *file, struct rt_pollreq *req);
};

/*********************************************************************************************************
** socket结构到文件IO的映射私有结构定义
*********************************************************************************************************/
struct sock_proto_priv {
  int socket;
  struct sock_proto_ops *ops;
};

/*********************************************************************************************************
**  实现的外部函数声明
*********************************************************************************************************/
extern int sock_proto_register(const struct sock_proto_ops *ops);
extern struct sock_proto_ops *sock_proto_lookup(int family);

#ifdef __cplusplus
    }
#endif      // __cplusplus
    
#endif      // __SOCK_PROTO_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
