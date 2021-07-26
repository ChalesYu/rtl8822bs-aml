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
#include "sock_proto.h"

#if defined(RT_USING_LWIP) && defined(RT_USING_POSIX)

/*********************************************************************************************************
** Function name:       dfs_getsocket
** Descriptions:        文件描述符到sock proto私有数据的转换
** input parameters:    fd： 文件描述符
** output parameters:   无
** Returned value:      转换的sock proto priv
*********************************************************************************************************/
static struct sock_proto_priv *dfs_getsocket(int fd)
{
  struct dfs_fd *_dfs_fd; 
  struct sock_proto_priv *proto_priv = NULL;
  
  _dfs_fd = fd_get(fd);
  if (_dfs_fd == NULL) return NULL;
  
  if (_dfs_fd->type != FT_SOCKET) proto_priv = NULL;
  else proto_priv = _dfs_fd->data;
  
  fd_put(_dfs_fd); /* put this dfs fd */
  return proto_priv;
}

/*********************************************************************************************************
** Function name:       dfs_net_ioctl
** Descriptions:        文件系统ioctl
** input parameters:    file: 文件描述结构
**                      cmd：  控制命令
**                      args： 控制参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
static int dfs_net_ioctl(struct dfs_fd* file, int cmd, void* args)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv =  file->data;
  if(proto_priv == NULL) {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->ioctlsocket(proto_priv->socket, cmd, args);
}

/*********************************************************************************************************
** Function name:       dfs_net_read
** Descriptions:        文件系统read
** input parameters:    file: 文件描述结构
**                      buf： 读取数据存放缓冲区
**                      count： 缓冲区大小
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
static int dfs_net_read(struct dfs_fd* file, void *buf, size_t count)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv =  file->data;
  if(proto_priv == NULL) {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->recv(proto_priv->socket, buf, count, 0);
}

/*********************************************************************************************************
** Function name:       dfs_net_write
** Descriptions:        文件系统write
** input parameters:    file: 文件描述结构
**                      buf： 写数据存放缓冲区
**                      count： 缓冲区大小
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
static int dfs_net_write(struct dfs_fd *file, const void *buf, size_t count)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv =  file->data;
  if(proto_priv == NULL) {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->send(proto_priv->socket, buf, count, 0);
}

/*********************************************************************************************************
** Function name:       dfs_net_close
** Descriptions:        文件系统close
** input parameters:    file: 文件描述结构
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
static int dfs_net_close(struct dfs_fd* file)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv =  file->data;
  if(proto_priv == NULL) {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->closesocket(proto_priv->socket);
}

/*********************************************************************************************************
** Function name:       dfs_net_poll
** Descriptions:        文件系统poll
** input parameters:    file: 文件描述结构
**                      req： poll结构
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
static int dfs_net_poll(struct dfs_fd *file, struct rt_pollreq *req)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv =  file->data;
  if(proto_priv == NULL) {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->poll(file, req);
}

/*********************************************************************************************************
** 定义文件系统映射结构
*********************************************************************************************************/
static const struct dfs_file_ops _net_fops = 
{
    NULL,    /* open     */
    dfs_net_close,
    dfs_net_ioctl,
    dfs_net_read,
    dfs_net_write,
    NULL,
    NULL,    /* lseek    */
    NULL,    /* getdents */
    dfs_net_poll,
};

/*********************************************************************************************************
** Function name:       dfs_net_get_fops
** Descriptions:        文件系统映射结构获取
** input parameters:    无
** output parameters:   无
** Returned value:      文件系统映射结构体
*********************************************************************************************************/
const struct dfs_file_ops *dfs_net_get_fops(void)
{
    return &_net_fops;
}

/*********************************************************************************************************
** Function name:       socket
** Descriptions:        bsd socket socket函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      socket文件描述符
*********************************************************************************************************/
int socket(int domain, int type, int protocol)
{
  /* create a BSD socket */
  int fd;
  int socket;
  struct dfs_fd *d;
  struct sock_proto_ops *ops;
  struct sock_proto_priv *proto_priv;
  
  proto_priv = rt_calloc(1, sizeof(struct sock_proto_priv));
  if(proto_priv == NULL) {
    rt_set_errno(-ENOMEM);
    return -1;
  }
  
  /* get sock proto ops */
  ops = sock_proto_lookup(domain);
  if(ops == NULL) {
    rt_set_errno(-EIO);
    return -1;
  }
  
  /* allocate a fd */
  fd = fd_new();
  if (fd < 0)
  {
    rt_set_errno(-ENOMEM);
    return -1;
  }
  d = fd_get(fd);
  
  /* create socket  and then put it to the dfs_fd */
  socket = ops->socket(domain, type, protocol);
  if (socket >= 0)
  {
    /* this is a socket fd */
    d->type = FT_SOCKET;
    d->path = NULL;
    
    d->fops = dfs_net_get_fops();
    
    d->flags = O_RDWR; /* set flags as read and write */
    d->size = 0;
    d->pos = 0;
    
    /* set sock proto priv to the data of dfs_fd */
    proto_priv->socket = socket;
    proto_priv->ops = ops;
    d->data = (void *) proto_priv;
  }
  else
  {
    /* release fd */
    fd_put(d);
    fd_put(d);
    
    rt_set_errno(-ENOMEM);
    
    return -1;
  }
  
  /* release the ref-count of fd */
  fd_put(d);
  
  return fd;
}
RTM_EXPORT(socket);

/*********************************************************************************************************
** Function name:       bind
** Descriptions:        bsd socket bind函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int bind(int s, const struct sockaddr *name, socklen_t namelen)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if(proto_priv != NULL) {
    return proto_priv->ops->bind(proto_priv->socket, name, namelen);
  } else {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
}
RTM_EXPORT(bind);

/*********************************************************************************************************
** Function name:       accept
** Descriptions:        bsd socket accept函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
  int new_socket = -1;
  struct sock_proto_priv *proto_priv;
  struct sock_proto_priv *proto_priv_new;
  
  proto_priv = dfs_getsocket(s);
  if(proto_priv == NULL) {
    return -1;
  }
  
  new_socket = proto_priv->ops->accept(proto_priv->socket, addr, addrlen);
  if (new_socket != -1)
  {
    /* this is a new socket, create it in file system fd */
    int fd;
    struct dfs_fd *d;
    
    proto_priv_new = rt_calloc(1, sizeof(struct sock_proto_priv));
    if(proto_priv == NULL) {
      rt_set_errno(-ENOMEM);
      proto_priv->ops->closesocket(new_socket);
      return -1;
    }
    
    /* allocate a fd */
    fd = fd_new();
    if (fd < 0)
    {
      rt_set_errno(-ENOMEM);
      rt_free(proto_priv_new);
      proto_priv->ops->closesocket(new_socket);
      return -1;
    }
    
    d = fd_get(fd);
    if(d)
    {
      /* this is a socket fd */
      d->type = FT_SOCKET;
      d->path = NULL;
      
      d->fops = dfs_net_get_fops();
      
      d->flags = O_RDWR; /* set flags as read and write */
      d->size = 0;
      d->pos = 0;
      
      /* set socket to the data of dfs_fd */
      proto_priv_new->socket = new_socket;
      proto_priv_new->ops = proto_priv->ops;
      d->data = (void *) proto_priv_new;
      
      /* release the ref-count of fd */
      fd_put(d);
      
      return fd;
    }
    
    rt_set_errno(-ENOMEM);
    rt_free(proto_priv_new);
    proto_priv->ops->closesocket(new_socket);
    return -1;
  }
  
  return -1;
}
RTM_EXPORT(accept);

/*********************************************************************************************************
** Function name:       shutdown
** Descriptions:        bsd socket shutdown函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int shutdown(int s, int how)
{
    int error = 0;
    int socket = -1;
    struct dfs_fd *d;
    struct sock_proto_priv *proto_priv;

    proto_priv = dfs_getsocket(s);
    if (proto_priv == NULL)
    {
        rt_set_errno(-ENOTSOCK);
        return -1;
    }

    d = fd_get(s);
    if (d == NULL)
    {
        rt_set_errno(-EBADF);
        return -1;
    }
    
    if (proto_priv->ops->shutdown(proto_priv->socket, how) == 0)
    {
        error = 0;
    }
    else
    {
        rt_set_errno(-ENOTSOCK);
        error = -1;
    }
    fd_put(d);

    return error;
}
RTM_EXPORT(shutdown);

/*********************************************************************************************************
** Function name:       getpeername
** Descriptions:        bsd socket getpeername函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int getpeername(int s, struct sockaddr *name, socklen_t *namelen)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->getpeername(proto_priv->socket, name, namelen);
}
RTM_EXPORT(getpeername);

/*********************************************************************************************************
** Function name:       getsockname
** Descriptions:        bsd socket getsockname函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->getsockname(proto_priv->socket, name, namelen);
}
RTM_EXPORT(getsockname);

/*********************************************************************************************************
** Function name:       getsockopt
** Descriptions:        bsd socket getsockopt函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->getsockopt(proto_priv->socket, level, optname, optval, optlen);
}
RTM_EXPORT(getsockopt);

/*********************************************************************************************************
** Function name:       setsockopt
** Descriptions:        bsd socket setsockopt函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->setsockopt(proto_priv->socket, level, optname, optval, optlen);
}
RTM_EXPORT(setsockopt);

/*********************************************************************************************************
** Function name:       connect
** Descriptions:        bsd socket connect函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int connect(int s, const struct sockaddr *name, socklen_t namelen)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->connect(proto_priv->socket, name, namelen);
}
RTM_EXPORT(connect);

/*********************************************************************************************************
** Function name:       listen
** Descriptions:        bsd socket listen函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int listen(int s, int backlog)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->listen(proto_priv->socket, backlog);
}
RTM_EXPORT(listen);

/*********************************************************************************************************
** Function name:       recv
** Descriptions:        bsd socket recv函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int recv(int s, void *mem, size_t len, int flags)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->recv(proto_priv->socket, mem, len, flags);
}
RTM_EXPORT(recv);

/*********************************************************************************************************
** Function name:       recvfrom
** Descriptions:        bsd socket recvfrom函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int recvfrom(int s, void *mem, size_t len, int flags,
             struct sockaddr *from, socklen_t *fromlen)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->recvfrom(proto_priv->socket, mem, len, flags, from, fromlen);
}
RTM_EXPORT(recvfrom);

/*********************************************************************************************************
** Function name:       recvmsg
** Descriptions:        bsd socket recvmsg函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int recvmsg(int s, struct msghdr *message, int flags)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->recvmsg(proto_priv->socket,message, flags);
}
RTM_EXPORT(recvfrom);

/*********************************************************************************************************
** Function name:       send
** Descriptions:        bsd socket send函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int send(int s, const void *dataptr, size_t size, int flags)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->send(proto_priv->socket, dataptr, size, flags);
}
RTM_EXPORT(send);

/*********************************************************************************************************
** Function name:       sendto
** Descriptions:        bsd socket sendto函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int sendto(int s, const void *dataptr, size_t size, int flags,
           const struct sockaddr *to, socklen_t tolen)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->sendto(proto_priv->socket, dataptr, size, flags, to, tolen);
}
RTM_EXPORT(sendto);

/*********************************************************************************************************
** Function name:       sendmsg
** Descriptions:        bsd socket sendmsg函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int sendmsg(int s, const struct msghdr *message, int flags)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->sendmsg(proto_priv->socket, message, flags);
}
RTM_EXPORT(sendmsg);

/*********************************************************************************************************
** Function name:       closesocket
** Descriptions:        bsd socket closesocket函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int closesocket(int s)
{
  int error = 0;
  struct dfs_fd *d;
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  d = fd_get(s);
  if (d == RT_NULL)
  {
    rt_set_errno(-EBADF);
    return -1;
  }
  
  if (proto_priv->ops->closesocket(proto_priv->socket) == 0)
  {
    error = 0;
  }
  else
  {
    rt_set_errno(-ENOTSOCK);
    error = -1;
  }
  
  /* socket has been closed, delete it from file system fd */
  rt_free(proto_priv);
  fd_put(d);
  fd_put(d);
  
  return error;
}
RTM_EXPORT(closesocket);

/*********************************************************************************************************
** Function name:       ioctlsocket
** Descriptions:        bsd socket ioctlsocket函数实现
** input parameters:    标准socket参数
** output parameters:   无
** Returned value:      执行结果
*********************************************************************************************************/
int ioctlsocket(int s, long cmd, void *arg)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv = dfs_getsocket(s);
  if (proto_priv == NULL)
  {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->ioctlsocket(proto_priv->socket, cmd, arg);
}
RTM_EXPORT(ioctlsocket);

#endif  // End of RT_USING_LWIP && RT_USING_POSIX
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
