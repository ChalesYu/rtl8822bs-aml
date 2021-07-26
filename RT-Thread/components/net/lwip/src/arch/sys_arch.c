/*
* Copyright (c) 2001-2003 Swedish Institute of Computer Science.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
* This file is part of the lwIP TCP/IP stack.
*
* Author: Adam Dunkels <adam@sics.se>
*
*/
/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.6lowpanworld.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           sys_arch.c
** Last modified Date:  2015-01-09
** Last Version:        V1.00
** Description:         lwip��rt thread����ϵͳ�����
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-01-09
** Version:             V1.00
** Descriptions:
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
*********************************************************************************************************/
#include <rtthread.h>

#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"
#include "lwip/err.h"
#include "arch/sys_arch.h"
#include "lwip/debug.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sio.h"
#include <lwip/init.h>

#include <string.h>

/*********************************************************************************************************
**  ���廥���壬�û�lwip�Ľӿڱ���
*********************************************************************************************************/
rt_mutex_t __gNetifMutex;

/*********************************************************************************************************
** Function name:       sys_sem_new
** Descriptions:        ����������һ���ź���.
** input parameters:    count: �ź����ĳ�ʼ״̬
** output parameters:   ��
** Returned value:      ERR_OK: �����ɹ��� ��֮������ʧ��
*********************************************************************************************************/
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  static unsigned short counter = 0;
  char tname[RT_NAME_MAX];
  sys_sem_t tmpsem;
  
  RT_DEBUG_NOT_IN_INTERRUPT;
  
  rt_snprintf(tname, RT_NAME_MAX, "%s%d", SYS_LWIP_SEM_NAME, counter);
  counter ++;
  
  tmpsem = rt_sem_create(tname, count, RT_IPC_FLAG_FIFO);
  if (tmpsem == RT_NULL)
    return ERR_MEM;
  else
  {
    *sem = tmpsem;
    
    return ERR_OK;
  }
}

/*********************************************************************************************************
** Function name:       sys_sem_free
** Descriptions:        ɾ��һ���ź���
** input parameters:    ָ��Ҫɾ�����ź���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sys_sem_free(sys_sem_t *sem)
{
  RT_DEBUG_NOT_IN_INTERRUPT;
  rt_sem_delete(*sem);
}

/*********************************************************************************************************
** Function name:       sys_sem_signal
** Descriptions:        �����ź���
** input parameters:    Ҫ�����źŵ��¼���־
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sys_sem_signal(sys_sem_t *sem)
{
  rt_sem_release(*sem);
}

/*********************************************************************************************************
** Function name:       sys_arch_sem_wait
** Descriptions:        �ȴ��ź�����������ǰ�߳�
** input parameters:    sem: ָ��Ҫ���͵��ź���
**                      timeout: ָ��Ҫ�ȴ���ʱ�䣨��λΪms)��
**                      Ϊ0��	�̻߳�һֱ������ֱ���ܵ�ָ�����źţ�
**                      ��0��	ָ���̵߳ȴ�ʱ��
** output parameters:   ��
** Returned value:      0�� ��ָ����ʱ���ڵȵ�ָ���ź�
**                      SYS_ARCH_TIMEOUT:��ָ����ʱ����û�еȵ�ָ���ź�
**                      ������ �ȴ����ź�����Ҫ��ʱ��
*********************************************************************************************************/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  rt_err_t ret;
  s32_t t;
  u32_t tick;
  
  RT_DEBUG_NOT_IN_INTERRUPT;
  
  /* get the begin tick */
  tick = rt_tick_get();
  if (timeout == 0)
    t = RT_WAITING_FOREVER;
  else
  {
    /* convert msecond to os tick */
    if (timeout < (1000/RT_TICK_PER_SECOND))
      t = 1;
    else
      t = timeout / (1000/RT_TICK_PER_SECOND);
  }
  
  ret = rt_sem_take(*sem, t);
  if(ret != RT_EOK) {
    return SYS_ARCH_TIMEOUT;
  } else {
    /* get elapse msecond */
    tick = rt_tick_get() - tick;
    
    /* convert tick to msecond */
    tick = tick * (1000 / RT_TICK_PER_SECOND);
    if (tick == 0) {
      tick = 1;
    }
    
    return tick;
  } 
}
/*********************************************************************************************************
** Function name:       sys_sem_valid
** Descriptions:        �ж��ź����Ƿ�����Ч���ź���
** input parameters:    sem: Ҫ�жϵ��ź���
** output parameters:   ��
** Returned value:      ����Ч���ź���������1����֮������0
*********************************************************************************************************/
#ifndef sys_sem_valid
int sys_sem_valid(sys_sem_t *sem)
{
  return (int)(*sem);
}
#endif

/*********************************************************************************************************
** Function name:       sys_sem_set_invalid
** Descriptions:        ��ָ�����ź�����Ϊ��Ч
** input parameters:    sem: Ҫ�жϵ��ź���
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
#ifndef sys_sem_set_invalid
void sys_sem_set_invalid(sys_sem_t *sem)
{
  *sem = RT_NULL;
}
#endif

/*********************************************************************************************************
** Function name:       sys_init
** Descriptions:        ����LwIP��˵���ĵ���������ʼ��sys_arch��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void  sys_init(void)
{
  __gNetifMutex = rt_mutex_create("netif", RT_IPC_FLAG_FIFO);
  if (__gNetifMutex == RT_NULL) {
    rt_kprintf("lwIP can't init!,netif mutex create fails!\n");
  }
}

/*********************************************************************************************************
** Function name:       sys_mbox_new
** Descriptions:        ����һ���յ�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      != SYS_MBOX_NULL: ��������ɹ�������һ��ָ�����������ָ��
**                      SYS_MBOX_NULL��   ��������ʧ��
*********************************************************************************************************/
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
  static unsigned short counter = 0;
  char tname[RT_NAME_MAX];
  sys_mbox_t tmpmbox;
  
  RT_DEBUG_NOT_IN_INTERRUPT;
  
  rt_snprintf(tname, RT_NAME_MAX, "%s%d", SYS_LWIP_MBOX_NAME, counter);
  counter ++;
  
  tmpmbox = rt_mb_create(tname, size, RT_IPC_FLAG_FIFO);
  if (tmpmbox != RT_NULL)
  {
    *mbox = tmpmbox;
    
    return ERR_OK;
  }
  
  return ERR_MEM;
}

/*********************************************************************************************************
** Function name:       sys_mbox_free
** Descriptions:        �ͷ����䣬������黹������
** input parameters:    mbox:Ҫ�黹�ͷŵ�����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sys_mbox_free(sys_mbox_t *mbox)
{
  RT_DEBUG_NOT_IN_INTERRUPT;
  
  rt_mb_delete(*mbox);
  
  return;
}

/*********************************************************************************************************
** Function name:       sys_mbox_post
** Descriptions:        �������䣬����ϢͶ�ݵ�ָ��������.������䲻����ȷ���ݣ������������̡�
** input parameters:    mbox:ָ��ҪͶ�ݵ�����
**                      msg: ָ��ҪͶ�ݵ���Ϣ
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  RT_DEBUG_NOT_IN_INTERRUPT;
  
  rt_mb_send_wait(*mbox, (rt_uint32_t)msg, RT_WAITING_FOREVER);
  
  return;
}

/*********************************************************************************************************
** Function name:       sys_mbox_trypost
** Descriptions:        �������䣬����ϢͶ�ݵ�ָ�������䡣�ú��������������̡�
** input parameters:    mbox:ָ��ҪͶ�ݵ�����
**                      msg: ָ��ҪͶ�ݵ���Ϣ
** output parameters:   ��
** Returned value:      ������������ģ�����ERR_MEM;��֮��������ȷͶ�ݣ�����ERR_OK��
*********************************************************************************************************/
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  if (rt_mb_send(*mbox, (rt_uint32_t)msg) == RT_EOK)
    return ERR_OK;
  
  return ERR_MEM;
}

/*********************************************************************************************************
** Function name:       sys_arch_mbox_fetch
** Descriptions:        ��ָ�������������Ϣ���ú����������߳�
** input parameters:    mbox:ָ��������Ϣ������
**                      ttimeout: ָ���ȴ����ʱ�䣬��λΪms
** output parameters:   msg:������յ�����Ϣָ��
** Returned value:      0�� ��ָ����ʱ�����յ���Ϣ
**                      SYS_ARCH_TIMEOUT:��ָ����ʱ����û���յ���Ϣ
*********************************************************************************************************/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  rt_err_t ret;
  s32_t t;
  u32_t tick;
  
  RT_DEBUG_NOT_IN_INTERRUPT;
  
  /* get the begin tick */
  tick = rt_tick_get();
  
  if(timeout == 0)
    t = RT_WAITING_FOREVER;
  else
  {
    /* convirt msecond to os tick */
    if (timeout < (1000/RT_TICK_PER_SECOND))
      t = 1;
    else
      t = timeout / (1000/RT_TICK_PER_SECOND);
  }
  
  ret = rt_mb_recv(*mbox, (rt_ubase_t *)msg, t);
  if(ret != RT_EOK) {
    return SYS_ARCH_TIMEOUT;
  }
  
  /* get elapse msecond */
  tick = rt_tick_get() - tick;
  
  /* convert tick to msecond */
  tick = tick * (1000 / RT_TICK_PER_SECOND);
  if (tick == 0)
    tick = 1;
  
  return tick;
}

/*********************************************************************************************************
** Function name:       sys_arch_mbox_tryfetch
** Descriptions:        ��ָ�������������Ϣ���ú������������߳�
** input parameters:    mbox:ָ��������Ϣ������
** output parameters:   msg:������յ�����Ϣָ��
** Returned value:      0�� �����д�����Ϣ
**                      SYS_MBOX_EMPTY:������û����Ϣ
*********************************************************************************************************/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  int ret;
  
  ret = rt_mb_recv(*mbox, (rt_ubase_t *)msg, 0);
  if((ret == RT_EOK) && (msg != RT_NULL)) {
    return 0;
  } else {
    return SYS_MBOX_EMPTY;
  }
}

/*********************************************************************************************************
** Function name:       sys_mbox_valid
** Descriptions:        ��������Ƿ�����Ч������
** input parameters:    mbox: Ҫ�жϵ�����
** output parameters:   ��
** Returned value:      ����Ч�����䣬����1����֮������0
*********************************************************************************************************/
#ifndef sys_mbox_valid
int sys_mbox_valid(sys_mbox_t *mbox)
{
  return (int)(*mbox);
}
#endif

/*********************************************************************************************************
** Function name:       sys_mbox_set_invalid
** Descriptions:        ��ָ������Ч��Ϊ��Ч
** input parameters:    mbox: ָ��Ҫ���������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
#ifndef sys_mbox_set_invalid
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  *mbox = RT_NULL;
}
#endif

#if (!LWIP_COMPAT_MUTEX)
/*********************************************************************************************************
** Function name:       sys_mutex_new
** Descriptions:        Create a new mutex
** input parameters:    mutex: pointer to the mutex to create
** output parameters:   ��
** Returned value:      RR_OK: �����ɹ��� ��֮������ʧ��
*********************************************************************************************************/
err_t sys_mutex_new(sys_mutex_t *mutex)
{
  static unsigned short counter = 0;
  char tname[RT_NAME_MAX];
  sys_mutex_t tmpmutex;
  
  RT_DEBUG_NOT_IN_INTERRUPT;
  
  rt_snprintf(tname, RT_NAME_MAX, "%s%d", SYS_LWIP_MUTEX_NAME, counter);
  counter ++;
  
  tmpmutex = rt_mutex_create(tname, RT_IPC_FLAG_FIFO);
  if (tmpmutex == RT_NULL)
    return ERR_MEM;
  else
  {
    *mutex = tmpmutex;
    
    return ERR_OK;
  }
}

/*********************************************************************************************************
** Function name:       sys_mutex_lock
** Descriptions:        Lock a mutex
** input parameters:    mutex: the mutex to lock
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sys_mutex_lock(sys_mutex_t *mutex)
{
  RT_DEBUG_NOT_IN_INTERRUPT;
  rt_mutex_take(*mutex, RT_WAITING_FOREVER);
  
  return;
}

/*********************************************************************************************************
** Function name:       sys_mutex_unlock
** Descriptions:        Unlock a mutex
** input parameters:    mutex: the mutex to unlock
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sys_mutex_unlock(sys_mutex_t *mutex)
{
  rt_mutex_release(*mutex);
}

/*********************************************************************************************************
** Function name:       sys_mutex_free
** Descriptions:        Delete a semaphore
** input parameters:    mutex: the mutex to delete
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sys_mutex_free(sys_mutex_t *mutex)
{
  RT_DEBUG_NOT_IN_INTERRUPT;
  
  rt_mutex_delete(*mutex);
}

#ifndef sys_mutex_valid
/*********************************************************************************************************
** Function name:       sys_mutex_valid
** Descriptions:        Check if a mutex is valid/allocated:
** input parameters:    mutex: the mutex to check
** output parameters:   ��
** Returned value:      return 1 for valid, 0 for invalid
*********************************************************************************************************/
int sys_mutex_valid(sys_mutex_t *mutex)
{
  return (int)(*mutex);
}

#endif

#ifndef sys_mutex_set_invalid
/** Set a mutex invalid so that sys_mutex_valid returns 0 */
/*********************************************************************************************************
** Function name:       sys_mutex_set_invalid
** Descriptions:        Set a mutex invalid so that sys_mutex_valid returns 0
** input parameters:    mutex: the mutex to set
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
  *mutex = RT_NULL;
}
#endif

#endif /* LWIP_COMPAT_MUTEX */

/*********************************************************************************************************
** Function name:       sys_jiffies
** Descriptions:        ��ȡ��ǰ��ϵͳʱ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ���ص�ǰ��ϵͳtick��
*********************************************************************************************************/
#ifndef sys_jiffies
u32_t sys_jiffies(void)
{
  return rt_tick_get();
}
#endif

/*********************************************************************************************************
** Function name:       sys_now
** Descriptions:        ��ȡ��ǰ��ϵͳʱ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ���ص�ǰ��ϵͳʱ�䣬��msΪ��λ
*********************************************************************************************************/
u32_t sys_now(void)
{
  return rt_tick_get() * (1000 / RT_TICK_PER_SECOND);
}

/*********************************************************************************************************
** Function name:       sys_arch_assert
** Descriptions:        ����ʹ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void sys_arch_assert(const char *file, int line)
{
  rt_kprintf("\nAssertion: %d in %s, thread %s\n",
             line, file, rt_thread_self()->name);
  RT_ASSERT(0);
}

/*********************************************************************************************************
** Function name:       sys_thread_new
** Descriptions:        ����һ���µ�����
** input parameters:    name:   	�߳�����
**                      thread: 	�߳���ڵ�ַ
**                      arg:    	���ݸ����̵߳Ĳ���
**                      stacksize:	�̶߳�ջ��С
**                      prio:   	�߳����ȼ�
** output parameters:   ��
** Returned value:      �߳̾��
*********************************************************************************************************/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
  rt_thread_t t;
  
  RT_DEBUG_NOT_IN_INTERRUPT;
  
  /* create thread */
  t = rt_thread_create(name, thread, arg, stacksize, prio, 20);
  RT_ASSERT(t != RT_NULL);
  
  /* startup thread */
  rt_thread_startup(t);
  
  return t;
}

#if SYS_LIGHTWEIGHT_PROT
/*
* This function is used to lock access to critical sections when lwipopt.h
* defines SYS_LIGHTWEIGHT_PROT. It disables interrupts and returns a value
* indicating the interrupt enable state when the function entered. This
* value must be passed back on the matching call to sys_arch_unprotect().
*
* @return the interrupt level when the function was entered.
*
*/
sys_prot_t
sys_arch_protect(void)
{
  sys_prot_t level;
  
  /* disable interrupt */
  level = rt_hw_interrupt_disable();
  
  return level;
}

/*
* This function is used to unlock access to critical sections when lwipopt.h
* defines SYS_LIGHTWEIGHT_PROT. It enables interrupts if the value of the lev
* parameter indicates that they were enabled when the matching call to
* sys_arch_protect() was made.
*
* @param pval is the interrupt level when the matching protect function was
* called
*
*/
void
sys_arch_unprotect(sys_prot_t pval)
{
  /* enable interrupt */
  rt_hw_interrupt_enable(pval);
  
  return;
}
#endif /* SYS_LIGHTWEIGHT_PROT */


#if PPP_SUPPORT
u32_t sio_read(sio_fd_t fd, u8_t *buf, u32_t size)
{
  u32_t len;
  
  RT_ASSERT(fd != RT_NULL);
  
  len = rt_device_read((rt_device_t)fd, 0, buf, size);
  if (len <= 0)
    return 0;
  
  return len;
}

u32_t sio_write(sio_fd_t fd, u8_t *buf, u32_t size)
{
  RT_ASSERT(fd != RT_NULL);
  
  return rt_device_write((rt_device_t)fd, 0, buf, size);
}

void sio_read_abort(sio_fd_t fd)
{
  rt_kprintf("read_abort\n");
}
#endif

/*
* export bsd socket symbol for RT-Thread Application Module
*/
#if LWIP_SOCKET
#include <lwip/sockets.h>
RTM_EXPORT(lwip_accept);
RTM_EXPORT(lwip_bind);
RTM_EXPORT(lwip_shutdown);
RTM_EXPORT(lwip_getpeername);
RTM_EXPORT(lwip_getsockname);
RTM_EXPORT(lwip_getsockopt);
RTM_EXPORT(lwip_setsockopt);
RTM_EXPORT(lwip_close);
RTM_EXPORT(lwip_connect);
RTM_EXPORT(lwip_listen);
RTM_EXPORT(lwip_recv);
RTM_EXPORT(lwip_read);
RTM_EXPORT(lwip_recvfrom);
RTM_EXPORT(lwip_send);
RTM_EXPORT(lwip_sendto);
RTM_EXPORT(lwip_socket);
RTM_EXPORT(lwip_write);
RTM_EXPORT(lwip_select);
RTM_EXPORT(lwip_ioctl);
RTM_EXPORT(lwip_fcntl);

RTM_EXPORT(lwip_htons);
RTM_EXPORT(lwip_ntohs);
RTM_EXPORT(lwip_htonl);
RTM_EXPORT(lwip_ntohl);

RTM_EXPORT(ipaddr_aton);
RTM_EXPORT(ipaddr_ntoa);

#if LWIP_DNS
#include <lwip/netdb.h>
RTM_EXPORT(lwip_gethostbyname);
RTM_EXPORT(lwip_gethostbyname_r);
RTM_EXPORT(lwip_freeaddrinfo);
RTM_EXPORT(lwip_getaddrinfo);
#endif

#endif

#if LWIP_DHCP
#include <lwip/dhcp.h>
RTM_EXPORT(dhcp_start);
RTM_EXPORT(dhcp_renew);
RTM_EXPORT(dhcp_stop);
#endif

#if LWIP_NETIF_API
#include <lwip/netifapi.h>
RTM_EXPORT(netifapi_netif_set_addr);
#endif
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
