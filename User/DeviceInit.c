/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           DeviceInit.c
** Last modified Date:  2014-04-09
** Last Version:        v1.0
** Description:         �豸������ʼ��
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2014-04-09
** Version:             v1.0
** Descriptions:        The original version ��ʼ�汾
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <dfs_fs.h>
#include <sys/socket.h>
#include <lwip_if.h>

#include "optparse/optparse.h"

#include "Main.h"

/*********************************************************************************************************
  �����ļ�ϵͳ���ر�
*********************************************************************************************************/
#ifdef RT_USING_DFS_MNTTABLE
const struct dfs_mount_tbl mount_table[] = {
  {"sd0", "/mnt", "elm", 0, 0},
  {0}
};
#endif


/*********************************************************************************************************
** Function name:       DeviceParamInit
** Descriptions:        �豸������ʼ�����������ļ��ж�ȡ��������������ڣ������Ĭ�ϲ���
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int DeviceParamInit(void)
{
   return 0;
}

/*********************************************************************************************************
** Function name:       reboot
** Descriptions:        ����ϵͳ
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
int reboot(int argc, char **argv)
{
  if (argc == 1)
  {
    rt_kprintf("\r\n\r\n");
    rt_thread_mdelay(100);
    rt_hw_cpu_reset();
  }
  else
  {
    rt_kprintf("no need parameter! \r\n");
  }
  
  return 0;
}
#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT(reboot,  reboot the cpu);
#endif /* FINSH_USING_MSH */

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
