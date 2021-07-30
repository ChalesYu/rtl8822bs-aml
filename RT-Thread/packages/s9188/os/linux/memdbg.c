/*
 * memdbg.c
 *
 * used for .....
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include "wf_debug.h"
#ifdef MEMDBG_ENABLE

#define MEMDBG_TABLE_SIZE (2048)

mem_debug_info_st *gl_mem_table = NULL;
wf_u64 gl_mem_alloc_size        = 0;
wf_u16 gl_mem_index             = 0;

void memdbg_check(void)
{
    int i=0;
    for(i=0;i<MEMDBG_TABLE_SIZE;i++)
    {
        if(wf_false == gl_mem_table[i].free_flag && wf_true == gl_mem_table[i].use_flag)
        //if(wf_true == gl_mem_table[i].use_flag)
        {
            LOG_I("[%d],[%s,%d],[%s,%d] %s",
                i,
                gl_mem_table[i].alloc_filename,
                gl_mem_table[i].alloc_lineno,
                gl_mem_table[i].free_filename,
                gl_mem_table[i].free_lineno,
                gl_mem_table[i].free_flag == wf_true?"free":"not free"
                );
        }
    }

    LOG_I("gl_mem_alloc_size:%lld Byte",gl_mem_alloc_size);
    
}
void memdbg_kfree(void *mem, const char *filename, wf_u32 fileno)
{
    int i=0;
    wf_u16 mem_index = 0;
    wf_bool find_flag = wf_false;
    
    for(i=0;i<MEMDBG_TABLE_SIZE;i++)
    {
        if(wf_true == gl_mem_table[i].use_flag && gl_mem_table[i].mem == mem)
        {
            find_flag = wf_true;
            mem_index = i;
        }
    }
    if(wf_true == find_flag)
    {
        gl_mem_table[mem_index].free_lineno = fileno;
        wf_memcpy(gl_mem_table[mem_index].free_filename,filename,16);
        gl_mem_table[mem_index].free_flag = wf_true;
        gl_mem_alloc_size -= gl_mem_table[mem_index].mem_sz;
        kfree(gl_mem_table[mem_index].mem);
        gl_mem_table[mem_index].mem = NULL;
        gl_mem_table[mem_index].mem_sz = 0;
        
    }
    else
    {
        LOG_E("[%s,%d] not find addr:%p",filename,fileno,mem);
    }
    
}
void *memdbg_kmalloc(wf_u32 mem_size, const char *filename,wf_u32 fileno)
{
    if(gl_mem_index >= MEMDBG_TABLE_SIZE)
    {
        LOG_E("[%s,%d] %d > MEMDBG_TABLE_SIZE",__func__,__LINE__,gl_mem_index);
        return NULL;
    }
    gl_mem_table[gl_mem_index].use_flag = wf_true;
    gl_mem_table[gl_mem_index].alloc_lineno = fileno;
    wf_memcpy(gl_mem_table[gl_mem_index].alloc_filename,filename,16);
    gl_mem_table[gl_mem_index].free_flag = wf_false;
    gl_mem_table[gl_mem_index].mem = kzalloc(mem_size, in_interrupt()? GFP_ATOMIC : GFP_KERNEL);
    gl_mem_table[gl_mem_index].mem_sz = mem_size;
    gl_mem_alloc_size += mem_size;
    gl_mem_index++;
    
    return gl_mem_table[gl_mem_index-1].mem;
}

wf_s32 memdbg_init(void)
{
    gl_mem_table = kzalloc(MEMDBG_TABLE_SIZE*sizeof(mem_debug_info_st), in_interrupt()? GFP_ATOMIC : GFP_KERNEL);
    if(NULL == gl_mem_table)
    {
        LOG_E("kzalloc for gl_mem_table failed");
        return -1;
    }
    
    return 0;
}
wf_s32 memdbg_term(void)
{
    memdbg_check();
    
    if(NULL!= gl_mem_table)
    {
        kfree(gl_mem_table);
        gl_mem_table = NULL;
    }
    
    return 0;
}

#endif
