/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwip_socket.c
** Last modified Date:  2019-10-11
** Last Version:        v1.00
** Description:         lwip的socket映射实现
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
#include <rthw.h>
#include <rtthread.h>

#if defined(RT_USING_LWIP) && defined(RT_USING_POSIX)

#include <string.h>
#include <stdio.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/api.h>
#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/priv/sockets_priv.h>

#include <dfs_poll.h>

#include "sock_proto.h"

/*********************************************************************************************************
** 外部函数声明
*********************************************************************************************************/
extern struct lwip_sock *lwip_tryget_socket(int s);

/*********************************************************************************************************
** Function name:       event_callback
** Descriptions:        lwip socket事件处理
** input parameters:    conn：指定网络连接
**                      evt： 网络事件
**                      len： 预留参数
** output parameters:   无
** Returned value:      找到的netif；找不到返回RT_NULL
*********************************************************************************************************/
static void event_callback(struct netconn *conn, enum netconn_evt evt, u16_t len)
{
  int s;
  struct lwip_sock *sock;
  uint32_t event = 0;
  SYS_ARCH_DECL_PROTECT(lev);
  
  LWIP_UNUSED_ARG(len);
  
  /* Get socket */
  if (conn)
  {
    s = conn->callback_arg.socket;
    if (s < 0)
    {
      /* Data comes in right away after an accept, even though
      * the server task might not have created a new socket yet.
      * Just count down (or up) if that's the case and we
      * will use the data later. Note that only receive events
      * can happen before the new socket is set up. */
      SYS_ARCH_PROTECT(lev);
      if (conn->callback_arg.socket < 0)
      {
        if (evt == NETCONN_EVT_RCVPLUS)
        {
          conn->callback_arg.socket--;
        }
        SYS_ARCH_UNPROTECT(lev);
        return;
      }
      s = conn->callback_arg.socket;
      SYS_ARCH_UNPROTECT(lev);
    }
    
    sock = lwip_tryget_socket(s);
    if (!sock)
    {
      return;
    }
  }
  else
  {
    return;
  }
  
  SYS_ARCH_PROTECT(lev);
  /* Set event as required */
  switch (evt)
  {
  case NETCONN_EVT_RCVPLUS:
    sock->rcvevent++;
    break;
  case NETCONN_EVT_RCVMINUS:
    sock->rcvevent--;
    break;
  case NETCONN_EVT_SENDPLUS:
    sock->sendevent = 1;
    break;
  case NETCONN_EVT_SENDMINUS:
    sock->sendevent = 0;
    break;
  case NETCONN_EVT_ERROR:
    sock->errevent = 1;
    break;
  default:
    LWIP_ASSERT("unknown event", 0);
    break;
  }
  
  if ((void*)(sock->lastdata.pbuf) || (sock->rcvevent > 0))
    event |= POLLIN;
  if (sock->sendevent)
    event |= POLLOUT;
  if (sock->errevent)
    event |= POLLERR;
  
  SYS_ARCH_UNPROTECT(lev);
  
  if (event)
  {
    rt_wqueue_wakeup(&sock->wait_head, (void*) event);
  }
}

/*********************************************************************************************************
** Function name:       inet_lwip_socket
** Descriptions:        重构lwip socket函数，使用wqueue
** input parameters:    参数同lwip socket
** output parameters:   无
** Returned value:      socket描述符
*********************************************************************************************************/
int inet_lwip_socket(int domain, int type, int protocol)
{
  int socket;
  
  socket = lwip_socket(domain, type, protocol);
  if (socket >= 0)
  {
    struct lwip_sock *lwsock;
    
    lwsock = lwip_tryget_socket(socket);
    lwsock->conn->callback = event_callback;
    
    rt_wqueue_init(&lwsock->wait_head);
  }
  
  return socket;
}

/*********************************************************************************************************
** Function name:       inet_lwip_accept
** Descriptions:        重构lwip accept函数，使用wqueue
** input parameters:    参数同lwip_accept
** output parameters:   无
** Returned value:      socket描述符
*********************************************************************************************************/
int inet_lwip_accept(int socket, struct sockaddr *addr, socklen_t *addrlen)
{
  int new_socket;
  
  new_socket = lwip_accept(socket, addr, addrlen);
  if (new_socket >= 0)
  {
    struct lwip_sock *lwsock;
    
    lwsock = lwip_tryget_socket(new_socket);
    
    rt_wqueue_init(&lwsock->wait_head);
  }
  
  return new_socket;
}

/*********************************************************************************************************
** Function name:       inet_lwip_ioctlsocket
** Descriptions:        重构lwip_ioctlsocket函数，使用wqueue
** input parameters:    参数同lwip_ioctlsocket
** output parameters:   无
** Returned value:      socket描述符
*********************************************************************************************************/
int inet_lwip_ioctlsocket(int socket, long cmd, void *arg)
{
  switch (cmd)
  {
  case F_GETFL:
  case F_SETFL:
    return lwip_fcntl(socket, cmd, (int) arg); 
    
  default:
    return lwip_ioctl(socket, cmd, arg);
  }
}

/*********************************************************************************************************
** Function name:       inet_lwip_poll
** Descriptions:        重构lwip_poll函数，使用wqueue
** input parameters:    参数同lwip_poll
** output parameters:   无
** Returned value:      socket描述符
*********************************************************************************************************/
int inet_lwip_poll(struct dfs_fd *file, struct rt_pollreq *req)
{
  int mask = 0;
  int s;
  struct lwip_sock *sock;
  struct sock_proto_priv *sock_ops;
  
  sock_ops = file->data;
  if(sock_ops != NULL) {
    s = sock_ops->socket;
    sock = lwip_tryget_socket(s);
    if (sock != NULL)
    {
      rt_base_t level;
      
      rt_poll_add(&sock->wait_head, req);
      
      level = rt_hw_interrupt_disable();
      
      if ((void*)(sock->lastdata.pbuf) || sock->rcvevent)
      {
        mask |= POLLIN;
      }
      if (sock->sendevent)
      {
        mask |= POLLOUT;
      }
      if (sock->errevent)
      {
        mask |= POLLERR;
        /* clean error event */
        sock->errevent = 0;
      }
      rt_hw_interrupt_enable(level);
    }
  }
  
  return mask;
}

/*********************************************************************************************************
**  sock proto协议注册结构定义
*********************************************************************************************************/
static const struct sock_proto_ops lwip_pf_inet_ops =
{
  .family = PF_INET,
  .socket = inet_lwip_socket,
  .closesocket = lwip_close,
  .bind = lwip_bind,
  .listen = lwip_listen,
  .connect = lwip_connect,
  .accept = inet_lwip_accept,
  .sendto = (int (*)(int, const void *, size_t, int, const struct sockaddr *, socklen_t))lwip_sendto,
  .recvfrom = (int (*)(int, void *, size_t, int, struct sockaddr *, socklen_t *))lwip_recvfrom,
  .send   = (int (*)(int, const void *, size_t, int))lwip_send,
  .recv   = (int (*)(int, void *, size_t, int))lwip_recv,
  .sendmsg = (int (*)(int, const struct msghdr *, int))lwip_sendmsg,
  .recvmsg = (int (*)(int, struct msghdr *, int))lwip_recvmsg,
  .getsockopt = lwip_getsockopt,
  .setsockopt = lwip_setsockopt,
  .shutdown = lwip_shutdown,
  .getpeername = lwip_getpeername,
  .getsockname = lwip_getsockname,
  .ioctlsocket = inet_lwip_ioctlsocket,
  .poll = inet_lwip_poll
};

static const struct sock_proto_ops lwip_pf_inet6_ops =
{
  .family = PF_INET6,
  .socket = inet_lwip_socket,
  .closesocket = lwip_close,
  .bind = lwip_bind,
  .listen = lwip_listen,
  .connect = lwip_connect,
  .accept = inet_lwip_accept,
  .sendto = (int (*)(int, const void *, size_t, int, const struct sockaddr *, socklen_t))lwip_sendto,
  .recvfrom = (int (*)(int, void *, size_t, int, struct sockaddr *, socklen_t *))lwip_recvfrom,
  .send   = (int (*)(int, const void *, size_t, int))lwip_send,
  .recv   = (int (*)(int, void *, size_t, int))lwip_recv,
  .sendmsg = (int (*)(int, const struct msghdr *, int))lwip_sendmsg,
  .recvmsg = (int (*)(int, struct msghdr *, int))lwip_recvmsg,
  .getsockopt = lwip_getsockopt,
  .setsockopt = lwip_setsockopt,
  .shutdown = lwip_shutdown,
  .getpeername = lwip_getpeername,
  .getsockname = lwip_getsockname,
  .ioctlsocket = inet_lwip_ioctlsocket,
  .poll = inet_lwip_poll
};

/*********************************************************************************************************
** Function name:       lwip_sock_init
** Descriptions:        lwip sock proto协议注册初始化
** input parameters:    无
** output parameters:   无
** Returned value:      注册结果
*********************************************************************************************************/
static int lwip_sock_init(void)
{
  sock_proto_register(&lwip_pf_inet_ops);
  sock_proto_register(&lwip_pf_inet6_ops);
  return 0;
}
INIT_ENV_EXPORT(lwip_sock_init);

#endif       // End of defined(RT_USING_LWIP) && defined(RT_USING_POSIX)
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
