/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwip_socket.h
** Last modified Date:  2019-10-11
** Last Version:        v1.00
** Description:          lwip��socketӳ��ʵ��
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
#ifndef __LWIP_SOCKET_H__
#define __LWIP_SOCKET_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
**  ʵ�ֵ��ⲿ��������
*********************************************************************************************************/
extern int inet_lwip_socket(int domain, int type, int protocol);
extern int inet_lwip_accept(int socket, struct sockaddr *addr, socklen_t *addrlen);
extern int inet_lwip_ioctlsocket(int socket, long cmd, void *arg);
extern int inet_lwip_poll(struct dfs_fd *file, struct rt_pollreq *req);

#ifdef __cplusplus
    }
#endif      // __cplusplus
    
#endif      // __LWIP_SOCKET_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
