/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.6lowpanworld.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           socket.h
** Last modified Date:  2019-10-11
** Last Version:        v1.00
** Description:         socket函数声明
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
#ifndef __SYS_SOCKET_H__
#define __SYS_SOCKET_H__

#include <rtthread.h>
#include <lwip/sockets.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef RT_USING_POSIX
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int bind(int s, const struct sockaddr *name, socklen_t namelen);
int shutdown(int s, int how);
int getpeername(int s, struct sockaddr *name, socklen_t *namelen);
int getsockname(int s, struct sockaddr *name, socklen_t *namelen);
int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
int connect(int s, const struct sockaddr *name, socklen_t namelen);
int listen(int s, int backlog);
int recv(int s, void *mem, size_t len, int flags);
int recvfrom(int s, void *mem, size_t len, int flags,
      struct sockaddr *from, socklen_t *fromlen);
int recvmsg(int s, struct msghdr *message, int flags);
int send(int s, const void *dataptr, size_t size, int flags);
int sendto(int s, const void *dataptr, size_t size, int flags,
    const struct sockaddr *to, socklen_t tolen);
int sendmsg(int s, const struct msghdr *message, int flags);
int socket(int domain, int type, int protocol);
int closesocket(int s);
int ioctlsocket(int s, long cmd, void *arg);

#define inet_ntop(af,src,dst,size)      lwip_inet_ntop(af,src,dst,size)
#define inet_pton(af,src,dst)           lwip_inet_pton(af,src,dst)

#endif /* RT_USING_POSIX */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SOCKET_H_ */
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
