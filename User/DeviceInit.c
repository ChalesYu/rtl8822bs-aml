/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           DeviceInit.c
** Last modified Date:  2014-04-09
** Last Version:        v1.0
** Description:         设备参数初始化
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2014-04-09
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
  定义文件系统挂载表
*********************************************************************************************************/
#ifdef RT_USING_DFS_MNTTABLE
const struct dfs_mount_tbl mount_table[] = {
  {"sd0", "/mnt", "elm", 0, 0},
  {0}
};
#endif


/*********************************************************************************************************
** Function name:       DeviceParamInit
** Descriptions:        设备参数初始化，从配置文件中读取参数，如果不存在，则采用默认参数
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
** Descriptions:        重启系统
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
