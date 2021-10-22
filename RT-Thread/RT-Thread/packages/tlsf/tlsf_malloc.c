/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info--------------------------------------------------------------------------------

** File Name:           tlsf_malloc.c
** Last modified Date:  2018-09-03
** Last Version:        v1.0
** Description:         tlsf内存分配算法提供给应用的函数实现
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2018-09-03
** Version:             v1.0
** Descriptions:        tlsf内存分配算法提供给应用的函数实现
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "tlsf.h"

#ifdef PKG_USING_TLSF

/*********************************************************************************************************
  配置使用的内存区地址
*********************************************************************************************************/
#define TLSF_MALLOC_MEM_ADDR    (0x10000000 + 32 * 1024)
#define TLSF_MALLOC_MEM_SIZE     32*1024

/*********************************************************************************************************
  全局变量定义
*********************************************************************************************************/
static tlsf_t* __gtlsf;
static struct rt_mutex __glock_mutex;

/*********************************************************************************************************
  必要的宏定义
*********************************************************************************************************/
#define TLSF_LOCK()             rt_mutex_take(&__glock_mutex, RT_WAITING_FOREVER);
#define TLSF_UNLOCK()           rt_mutex_release(&__glock_mutex);

/*********************************************************************************************************
** Function name:       __vp_mem_init
** Descriptions:        tlsf内存分配初始化，添加内存池
** input parameters:    addr: 内存起始地址
**                      size：内存大小
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void __vp_mem_init(uint8_t *addr, size_t size)
{
  RT_DEBUG_NOT_IN_INTERRUPT;
  
  __gtlsf = tlsf_create_with_pool(addr,size);
  if(__gtlsf == RT_NULL) {
    rt_kprintf("Can't create pool for tlsf\r\n");
    return ;
  }
  rt_mutex_init(&__glock_mutex, "tlsf", RT_IPC_FLAG_FIFO);
}

/*********************************************************************************************************
** Function name:       vp_mem_add_pool
** Descriptions:        tlsf为内存池增加内存
** input parameters:    addr: 内存起始地址
**                      size：内存大小
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void vp_mem_add_pool(uint8_t *addr, size_t size)
{
  TLSF_LOCK();
  tlsf_add_pool(__gtlsf, addr, size);	
  TLSF_UNLOCK();
}

/*********************************************************************************************************
** Function name:       vp_mem_malloc
** Descriptions:        tlsf分配内存
** input parameters:    ize：待分配的内存大小
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void *vp_mem_malloc(size_t size)
{
  void *ptr;
  
  TLSF_LOCK();
  ptr = tlsf_malloc(__gtlsf, size);	
  TLSF_UNLOCK();
  
  return ptr;
}

/*********************************************************************************************************
** Function name:       vp_mem_realloc
** Descriptions:        tlsf分配内存realloc
** input parameters:    addr: 内存起始地址
**                      size：内存大小
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void *vp_mem_realloc(void *mem, size_t size)
{
  void *ptr;
  
  TLSF_LOCK();
  ptr = tlsf_realloc(__gtlsf, mem, size);	
  TLSF_UNLOCK();
  
  return ptr;
}

/*********************************************************************************************************
** Function name:       vp_mem_alloc_align
** Descriptions:        tlsf按照指定对齐分配内存
** input parameters:    size：待分配的内存大小
**                      align:多少字节对齐
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void *vp_mem_alloc_align(size_t size, size_t align)
{
  void *ptr;
  
  TLSF_LOCK();
  ptr = tlsf_memalign(__gtlsf, align, size);	
  TLSF_UNLOCK();
  
  return ptr;
}

/*********************************************************************************************************
** Function name:       vp_mem_free
** Descriptions:        tlsf释放内存
** input parameters:    mem: 待释放的内存
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void vp_mem_free(void *mem)
{
  TLSF_LOCK();
  tlsf_free(__gtlsf, mem);	
  TLSF_UNLOCK();
}

/*********************************************************************************************************
** Function name:       cmd_tlsf_free
** Descriptions:        查看tlsf内存分配信息
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      0：正确返回，无任何意义
*********************************************************************************************************/
int cmd_tlsf_free(int argc, char **argv)
{
  rt_kprintf("total memory: %d\n", TLSF_MALLOC_MEM_SIZE);
//  rt_kprintf("used memory : %d\n", tlsf_pool_overhead());
//  rt_kprintf("maximum allocated memory: %d\n", tlsf_alloc_overhead());
  
  return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_tlsf_free, __cmd_tlsf_free, list the tlsf memmory info);

/*********************************************************************************************************
** Function name:       vp_mem_init
** Descriptions:        tlsf内存管理初始化
** input parameters:    NONE
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static int vp_mem_init(void)
{
  __vp_mem_init((uint8_t *)TLSF_MALLOC_MEM_ADDR, TLSF_MALLOC_MEM_SIZE);
  return 0;
}


/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_PREV_EXPORT(vp_mem_init);

#endif  // end of PKG_USING_TLSF
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
