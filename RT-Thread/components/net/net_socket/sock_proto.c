/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           sock_proto.h
** Last modified Date:  2019-10-11
** Last Version:        v1.00
** Description:         sock proto����ʵ��
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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "sock_proto.h"

/*********************************************************************************************************
** ȫ�ֱ�������
*********************************************************************************************************/
static const struct sock_proto_ops *sock_proto_operation_table[SOCK_PROTO_NUM];
static struct rt_mutex socklock;

/*********************************************************************************************************
** Function name:       sock_proto_init
** Descriptions:        ����Э�鹦�ܳ�ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      0
*********************************************************************************************************/
static int sock_proto_init(void)
{
    /* clear sock operations table */
    memset((void *)sock_proto_operation_table, 0, sizeof(sock_proto_operation_table));

    /* create sock proto lock */
    rt_mutex_init(&socklock, "socklock", RT_IPC_FLAG_FIFO);
    
    return 0;
}
INIT_COMPONENT_EXPORT(sock_proto_init);

/*********************************************************************************************************
** Function name:       sock_lock
** Descriptions:        ����Э�黥����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
static void sock_lock(void)
{
    rt_err_t result = -RT_EBUSY;

    while (result == -RT_EBUSY)
    {
        result = rt_mutex_take(&socklock, RT_WAITING_FOREVER);
    }

    if (result != RT_EOK)
    {
        RT_ASSERT(0);
    }
}

/*********************************************************************************************************
** Function name:       sock_unlock
** Descriptions:        ����Э�黥����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
static void sock_unlock(void)
{
    rt_mutex_release(&socklock);
}

/*********************************************************************************************************
** Function name:       sock_proto_register
** Descriptions:        ע��һ������Э��ջ������
** input parameters:    ops�� ����Э��ջ������
** output parameters:   ��
** Returned value:      0���ɹ��� ������ʧ��
*********************************************************************************************************/
int sock_proto_register(const struct sock_proto_ops *ops)
{
    int ret = RT_EOK;
    const struct sock_proto_ops **empty = NULL;
    const struct sock_proto_ops **iter;

    /* lock sock proto */
    sock_lock();
    /* check if this sock proto was already registered */
    for (iter = &sock_proto_operation_table[0];
           iter < &sock_proto_operation_table[SOCK_PROTO_NUM]; iter ++)
    {
        /* find out an empty filesystem type entry */
        if (*iter == NULL)
            (empty == NULL) ? (empty = iter) : 0;
        else if ((*iter)->family == ops->family)
        {
            rt_set_errno(-EEXIST);
            ret = -1;
            break;
        }
    }

    /* save the sock proto's operations */
    if (empty == NULL)
    {
        rt_set_errno(-ENOSPC);
        LOG_E("There is no space to register this sock family: (%d).", ops->family);
        ret = -1;
    }
    else if (ret == RT_EOK)
    {
        *empty = ops;
    }

    sock_unlock();
    return ret;
}

/*********************************************************************************************************
** Function name:       sock_proto_lookup
** Descriptions:        ע��һ������Э��ջ������
** input parameters:    ���ҵ�Э���
** output parameters:   ��
** Returned value:      ���ҵ�������������ṹָ��
*********************************************************************************************************/
struct sock_proto_ops *sock_proto_lookup(int family)
{
    const struct sock_proto_ops **iter;
    struct sock_proto_ops *empty = NULL;

    /* lock sock proto */
    sock_lock();
    /* check if this sock proto was already registered */
    for (iter = &sock_proto_operation_table[0];
           iter < &sock_proto_operation_table[SOCK_PROTO_NUM]; iter ++)
    {
      if(*iter != NULL) {
        if((*iter)->family == family) {
           empty = (struct sock_proto_ops *)(*iter);
           break;
        }
      }
    }

    sock_unlock();
    return empty;
}

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
