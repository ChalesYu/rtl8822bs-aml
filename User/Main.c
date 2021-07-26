/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           Main.c
** Last modified Date:  2014-12-23
** Last Version:        V1.00
** Description:         ������ģ�壬ϵͳ����ִ���ļ�
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2014-12-23
** Version:             V1.00
** Descriptions:
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
*********************************************************************************************************/
#include "includes.h"

/*********************************************************************************************************
  ȫ�ֱ�������
*********************************************************************************************************/

/*********************************************************************************************************
  ���غ�������
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       main
** Descriptions:        c�������������
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      0����ȷ���أ����κ�����
*********************************************************************************************************/
int main(void)
{
  /*
  ** Step 1, ��ʼ��Ӧ�ò���
  */
  // ��ʱһ�£��ȴ�����Ӳ�����趼�������
  rt_thread_delay(RT_TICK_PER_SECOND / 5);
  DeviceParamInit();
  
  /*
  ** Step 2, ��ʼ����̫��
  */
  rt_hw_ethernet_init(RT_NULL);
  //if_set_dhcp("e0", 1);
  set_if("e0","192.168.169.250", "192.168.169.1","255.255.255.0");
  set_dns(0, "223.5.5.5");
 
  /*
  ** Step3, ��ʼ������Ӧ��
  */ 

  for(;;)
  {
    rt_hw_led_toggle(0);
    rt_thread_delay(RT_TICK_PER_SECOND);
  }
  
  // return 0;
}

/*********************************************************************************************************
** Function name:       mnt_init
** Descriptions:        �ļ�ϵͳ����
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int mnt_init(void)
{
  if (dfs_mount("flash0", "/", "lfs", 0, 0) == 0)
  {
    rt_kprintf("flash0 mount to / ok!\r\n");
  }
  else
  {
    rt_kprintf("flash0 mount to / failed!, fatmat and try again!\n");
    
    /* fatmat filesystem. */
    dfs_mkfs("lfs", "flash0");
    
    /* re-try mount. */
    if (dfs_mount("flash0", "/", "lfs", 0, 0) == 0)
    {
      rt_kprintf("flash0 mount to / ok!\r\n");
    } else {
      rt_kprintf("flash0 can't mount to /, and can't format\r\n");
      for(;;)
      {
        rt_hw_led_toggle(0);
        rt_thread_delay(RT_TICK_PER_SECOND * 2);
      }
    }
  }
  
  return 0;
}
INIT_ENV_EXPORT(mnt_init);

/*********************************************************************************************************
** Function name:       md5_calc
** Descriptions:        md5У��
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
#if 0
#include <netif/ppp/polarssl/md5.h>
int md5_calc(int argc, char **argv)
{
#if 1
  static uint8_t buffer[512];
  if (argc == 2)
  {
    int i;
    int fd;
    int iBytesRead;
    uint8_t md5_buff[16];
    md5_context md5_ctx;
    
    fd = open(argv[1], O_RDONLY, 0);
    if(fd < 0) {
      rt_kprintf("[%s]: open file error\r\n", __func__);
      return -1;
    }
    
    md5_starts(&md5_ctx);
    while(1) {
        iBytesRead = read(fd, buffer, 512);
        if(iBytesRead > 0) {
          md5_update(&md5_ctx, (unsigned char const *)buffer, iBytesRead);
        } else {
          break;
        }
    }
    md5_finish(&md5_ctx, md5_buff);
    close(fd);
    
    rt_kprintf("file: %s md5 result:%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n",
               argv[1],
    md5_buff[0], md5_buff[1],md5_buff[2],md5_buff[3],md5_buff[4],md5_buff[5],md5_buff[6],md5_buff[7],md5_buff[8],
    md5_buff[9], md5_buff[10],md5_buff[11],md5_buff[12],md5_buff[13],md5_buff[14],md5_buff[15]);
  }
  else
  {
    rt_kprintf("bad parameter! eg:md5_calc /flash/collect.hnf\r\n");
  }
#else
    static uint8_t buffer[512];
  if (argc == 2)
  {
    int i;
    int iBytesRead;
    uint8_t md5_buff[16];
    md5_context md5_ctx;
    FILE *fp;
    
    fp = fopen(argv[1], "rb");
    if(fp == NULL) {
      rt_kprintf("[%s]: open file error\r\n", __func__);
      return -1;
    }
    
    md5_starts(&md5_ctx);
    while(1) {
        iBytesRead = fread(buffer,1, 512, fp);
        if(iBytesRead > 0) {
          md5_update(&md5_ctx, (unsigned char const *)buffer, iBytesRead);
        } else {
          break;
        }
    }
    md5_finish(&md5_ctx, md5_buff);
    fclose(fp);
    
    rt_kprintf("file: %s md5 result:%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n",
               argv[1],
    md5_buff[0], md5_buff[1],md5_buff[2],md5_buff[3],md5_buff[4],md5_buff[5],md5_buff[6],md5_buff[7],md5_buff[8],
    md5_buff[9], md5_buff[10],md5_buff[11],md5_buff[12],md5_buff[13],md5_buff[14],md5_buff[15]);
  }
  else
  {
    rt_kprintf("bad parameter! eg:md5_calc /flash/collect.hnf\r\n");
  }
#endif
  
  return 0;
}
#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(md5_calc,  calc file md5 value);
#endif /* FINSH_USING_MSH */
#endif

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
