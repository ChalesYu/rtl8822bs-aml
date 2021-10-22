/*
 * memdbg.h
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
#ifndef __MEMDBG_H__
#define __MEMDBG_H__

#ifdef MEMDBG_ENABLE

typedef struct mem_debug_info_st_
{
    void *mem;
    wf_u32 mem_sz;
    wf_bool use_flag;
    wf_bool free_flag;
    char alloc_filename[16];
    wf_u32 alloc_lineno;
    char free_filename[16];
    wf_u32 free_lineno;    
}mem_debug_info_st;

void memdbg_kfree(void *mem,const char *filename, wf_u32 fileno);
void *memdbg_kmalloc(wf_u32 mem_size, const char *filename,wf_u32 fileno);
void memdbg_check(void);
wf_s32 memdbg_init(void);
wf_s32 memdbg_term(void);
#endif

#endif
