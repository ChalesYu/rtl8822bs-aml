/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info--------------------------------------------------------------------------------

** File Name:           tlsf_malloc.h
** Last modified Date:  2018-09-03
** Last Version:        v1.0
** Description:         tlsf�ڴ�����㷨�ṩ��Ӧ�õĺ���ʵ��
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2018-09-03
** Version:             v1.0
** Descriptions:        tlsf�ڴ�����㷨�ṩ��Ӧ�õĺ���ʵ��
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/
#ifndef __TLSF_MALLOC_H__
#define __TLSF_MALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <rtthread.h>
/*********************************************************************************************************
  �ⲿ��������
*********************************************************************************************************/
extern void vp_mem_add_pool(uint8_t *addr, size_t size);
extern void *vp_mem_malloc(size_t size);
extern void *vp_mem_realloc(void *mem, size_t size);
extern void *vp_mem_alloc_align(size_t size, size_t align);
extern void vp_mem_free(void *mem);

#ifdef __cplusplus
    }           // __cplusplus
#endif

#endif          // endif of __TLSF_MALLOC_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
