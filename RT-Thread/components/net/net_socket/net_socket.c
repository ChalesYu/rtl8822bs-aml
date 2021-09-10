/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwip_socket.c
** Last modified Date:  2019-10-11
** Last Version:        v1.00
** Description:         lwip��socketӳ��ʵ��
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
** Descriptions:        �ļ���������sock proto˽�����ݵ�ת��
** input parameters:    fd�� �ļ�������
** output parameters:   ��
** Returned value:      ת����sock proto priv
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
** Descriptions:        �ļ�ϵͳioctl
** input parameters:    file: �ļ������ṹ
**                      cmd��  ��������
**                      args�� ���Ʋ���
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        �ļ�ϵͳread
** input parameters:    file: �ļ������ṹ
**                      buf�� ��ȡ���ݴ�Ż�����
**                      count�� ��������С
** output parameters:   ��
** Returned value:      ִ�н��
*********************************************************************************************************/
static int dfs_net_read(struct dfs_fd* file, void *buf, size_t count)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv =  file->data;
  if(proto_priv == NULL) {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->recvfrom(proto_priv->socket, buf, count, 0, NULL, NULL);
}

/*********************************************************************************************************
** Function name:       dfs_net_write
** Descriptions:        �ļ�ϵͳwrite
** input parameters:    file: �ļ������ṹ
**                      buf�� д���ݴ�Ż�����
**                      count�� ��������С
** output parameters:   ��
** Returned value:      ִ�н��
*********************************************************************************************************/
static int dfs_net_write(struct dfs_fd *file, const void *buf, size_t count)
{
  struct sock_proto_priv *proto_priv;
  
  proto_priv =  file->data;
  if(proto_priv == NULL) {
    rt_set_errno(-ENOTSOCK);
    return -1;
  }
  
  return proto_priv->ops->sendto(proto_priv->socket, buf, count, 0, NULL, 0);
}

/*********************************************************************************************************
** Function name:       dfs_net_close
** Descriptions:        �ļ�ϵͳclose
** input parameters:    file: �ļ������ṹ
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        �ļ�ϵͳpoll
** input parameters:    file: �ļ������ṹ
**                      req�� poll�ṹ
** output parameters:   ��
** Returned value:      ִ�н��
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
** �����ļ�ϵͳӳ��ṹ
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
** Descriptions:        �ļ�ϵͳӳ��ṹ��ȡ
** input parameters:    ��
** output parameters:   ��
** Returned value:      �ļ�ϵͳӳ��ṹ��
*********************************************************************************************************/
const struct dfs_file_ops *dfs_net_get_fops(void)
{
    return &_net_fops;
}

/*********************************************************************************************************
** Function name:       socket
** Descriptions:        bsd socket socket����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      socket�ļ�������
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
** Descriptions:        bsd socket bind����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket accept����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket shutdown����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket getpeername����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket getsockname����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket getsockopt����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket setsockopt����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket connect����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket listen����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket recv����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
  
  return proto_priv->ops->recvfrom(proto_priv->socket, mem, len, flags, NULL, NULL);
}
RTM_EXPORT(recv);

/*********************************************************************************************************
** Function name:       recvfrom
** Descriptions:        bsd socket recvfrom����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket recvmsg����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket send����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
  
  return proto_priv->ops->sendto(proto_priv->socket, dataptr, size, flags, NULL, 0);
}
RTM_EXPORT(send);

/*********************************************************************************************************
** Function name:       sendto
** Descriptions:        bsd socket sendto����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket sendmsg����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket closesocket����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
** Descriptions:        bsd socket ioctlsocket����ʵ��
** input parameters:    ��׼socket����
** output parameters:   ��
** Returned value:      ִ�н��
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
