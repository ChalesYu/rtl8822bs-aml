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

/*********************************************************************************************************
** Function name:       tcp_app_thread
** Descriptions:        tcp例子程序
** input parameters:    *parg
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
static uint8_t udp_buffer[2048];
void udp_app_thread(void *parg)
{
  int s;
  struct sockaddr_in myAddr, destAddr;
  //struct in6_addr ia6 = IN6ADDR_ANY_INIT;
  uint32_t u32CliLen;
  int iRecv;
  //uint32 count = 0;

  myAddr.sin_family = AF_INET;
  myAddr.sin_port = htons(8000);
  myAddr.sin_addr.s_addr = htonl(INADDR_ANY);;
  u32CliLen = sizeof(destAddr);

  s = socket(PF_INET,SOCK_DGRAM,0);
  bind(s,(const struct sockaddr *)&myAddr,sizeof(myAddr));

  destAddr.sin_family = AF_INET;
  destAddr.sin_port = htons(8000);
  destAddr.sin_addr.s_addr = inet_addr("192.168.1.25");

  while(1)
  {
     iRecv = recvfrom(s,udp_buffer,2048,0, (struct sockaddr *)&destAddr,&u32CliLen);
     if(iRecv > 0) {
        sendto(s,udp_buffer,iRecv,0,
              ( const struct sockaddr *)&destAddr,sizeof(destAddr));
     }
  }
  //close(s);
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
int udp_loop(int argc, char **argv)
{
    rt_thread_t tid;

    if (argc == 1)
    {
        tid = rt_thread_create("udp", udp_app_thread, RT_NULL, 2048, 10, 20);
        if (tid != RT_NULL) {
            rt_thread_startup(tid);
            rt_kprintf("create udp loop test thread ok!\r\n");
        }
    }

    return 0;
}
MSH_CMD_EXPORT(udp_loop, create udp loop test example);
#endif /* FINSH_USING_MSH */
/*********************************************************************************************************
** End of File
*********************************************************************************************************/
