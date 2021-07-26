/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.6lowpanworld.com
**                                http://hichard.taobao.com
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           cpu_usage.c
** Last modified Date:  2018-12-23
** Last Version:        V1.00
** Description:         CPU使用率统计
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
#include <rtthread.h>
#include <rthw.h>

#define CPU_USAGE_CALC_TICK    10
#define CPU_USAGE_LOOP        100

static rt_uint8_t  cpu_usage_major = 0, cpu_usage_minor= 0;
static rt_uint32_t total_count = 0;

static void cpu_usage_idle_hook(void)
{
  rt_tick_t tick;
  rt_uint32_t count;
  volatile rt_uint32_t loop;
  
  if (total_count == 0)
  {
    /* get total count */
    rt_enter_critical();
    tick = rt_tick_get();
    while(rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
    {
      total_count ++;
      loop = 0;
      
      while (loop < CPU_USAGE_LOOP) loop ++;
    }
    rt_exit_critical();
  }
  
  count = 0;
  /* get CPU usage */
  tick = rt_tick_get();
  while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
  {
    count ++;
    loop  = 0;
    while (loop < CPU_USAGE_LOOP) loop ++;
  }
  
  /* calculate major and minor */
  if (count < total_count)
  {
    count = total_count - count;
    cpu_usage_major = (count * 100) / total_count;
    cpu_usage_minor = ((count * 100) % total_count) * 100 / total_count;
  }
  else
  {
    total_count = count;
    
    /* no CPU usage */
    cpu_usage_major = 0;
    cpu_usage_minor = 0;
  }
}

void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor)
{
  RT_ASSERT(major != RT_NULL);
  RT_ASSERT(minor != RT_NULL);
  
  *major = cpu_usage_major;
  *minor = cpu_usage_minor;
}

int cpu_usage_init(void)
{
  /* set idle thread hook */
  rt_thread_idle_sethook(cpu_usage_idle_hook);
  
  return 0;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>

int cpu_usage(int argc, char** argv)
{
  uint8_t cpu_usage_major, cpu_usage_minor;
  float cpu_usage;
  
  cpu_usage_get(&cpu_usage_major, &cpu_usage_minor);
  cpu_usage = (float) cpu_usage_major + ((float) cpu_usage_minor) / 100.0f;
  
  printf("cpu usage: %.2f%\r\n", cpu_usage);
  
  return 0;
}
MSH_CMD_EXPORT(cpu_usage, print cpu usage);
#endif /* FINSH_USING_MSH */
/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_APP_EXPORT(cpu_usage_init);

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
