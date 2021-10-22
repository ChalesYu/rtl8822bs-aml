/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info--------------------------------------------------------------------------------

** File Name:           tlsf_malloc.h
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
#ifndef __TLSF_MALLOC_H__
#define __TLSF_MALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <rtthread.h>
/*********************************************************************************************************
  外部函数声明
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
