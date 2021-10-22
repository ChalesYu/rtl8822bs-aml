/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           tcp_client_example.c
** Last modified Date:  2015-11-14
** Last Version:        v1.0
** Description:         TCP客户端例子
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2015-11-14
** Version:             v1.0
** Descriptions:        The original version 初始版本
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/
#include <includes.h>

/*********************************************************************************************************
** 全局配置
*********************************************************************************************************/
#define TCP_SERVER_ADDRESS    "192.168.100.10"   //"192.168.100.10"   // 39.108.222.205
#define TCP_SERVER_PORT       "8080"

/*********************************************************************************************************
** Function name:       tcp_app_thread
** Descriptions:        tcp例子程序
** input parameters:    *parg
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
static uint8_t tcp_buffer[1024];
void tcp_app_thread(void *parg)
{
    int rv;
    int sockfd;
    int recv_len, snd_len;
    struct addrinfo hints, *servinfo, *p;
    //char ipstr[INET6_ADDRSTRLEN];
    char ipstr[40];
    struct timeval timeout;

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    /*
    ** Step 1, 域名解析
    */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version， AF_UNSPEC is best
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(TCP_SERVER_ADDRESS, TCP_SERVER_PORT, &hints, &servinfo)) != 0)
    {
        rt_kprintf( "getaddrinfo: get dns error\r\n");
        return;
    }

    /*
    ** Step 2, 根据域名解析结果连接服务器
    */
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        void *addr;
        char *ipver;
        if (p->ai_family == AF_INET)   // IPv4
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
            if(ipv4->sin_addr.s_addr == 0) {
              rt_kprintf( "dns get ipv4 addr none\r\n");
              continue;
            }
        }
#if LWIP_IPV6
        else     // IPv6
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
#endif
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        rt_kprintf(" %s: %s\n", ipver, ipstr);

        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            rt_kprintf( "create socket error\r\n");
            continue;
        }

        /* set receive and send timeout option */
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *) &timeout,
                   sizeof(timeout));
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (void *) &timeout,
                   sizeof(timeout));
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            closesocket(sockfd);
            rt_kprintf( "connect server error\r\n");
            continue;
        }
        break; // if we get here, we must have connected successfully
    }

    if(p == NULL)
    {
        rt_kprintf( "dns error, can't connect to server\r\n");
        freeaddrinfo(servinfo); // free the linked list
        return;
    }
    freeaddrinfo(servinfo); // free the linked list

    /*
    ** Step 3, 进入数据收发循环
    */
    while (1)
    {
        recv_len = recv(sockfd, tcp_buffer, 1024, 0);
        if (recv_len == 0)
        {
            rt_kprintf("received error,close the socket.\r\n");
            break;
        }
        if(-1 == recv_len)
        {
            if(!(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
            {
                rt_kprintf("received error,close the socket.\r\n");
                break;
            }
        }

        if(recv_len > 0)
        {

            /* 发送数据到sock连接 */
            snd_len = send(sockfd, tcp_buffer, recv_len, 0);
            if (snd_len == 0)
            {
                rt_kprintf("send error,close the socket.\r\n");
                break;
            }
            if(-1 == snd_len)
            {
                if(!(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
                {
                    rt_kprintf("send error,close the socket.\r\n");
                    break;
                }
            }
        }
    }
    closesocket(sockfd);
    return;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
int tcp_client(int argc, char **argv)
{
    rt_thread_t tid;

    if (argc == 1)
    {
        tid = rt_thread_create("tcp", tcp_app_thread, RT_NULL, 2048, 10, 20);
        if (tid != RT_NULL) {
            rt_thread_startup(tid);
            rt_kprintf("create tcp test thread ok!\r\n");
        }
    }

    return 0;
}
MSH_CMD_EXPORT(tcp_client, create tcp client example);
#endif /* FINSH_USING_MSH */
/*********************************************************************************************************
** End of File
*********************************************************************************************************/
