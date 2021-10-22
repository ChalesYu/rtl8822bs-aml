/*
 * wf_typedef.h
 *
 * used for .....
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

typedef unsigned char      wf_u8;
typedef unsigned short     wf_u16;
typedef unsigned int       wf_u32;
typedef unsigned long long wf_u64;

typedef signed char        wf_s8;
typedef signed short       wf_s16;
typedef signed int         wf_s32;
typedef signed long long   wf_s64;

typedef unsigned long      wf_ptr;


#ifndef NULL
#define NULL ((void *)0)
#endif

typedef enum
{
    wf_false = 0,
    wf_true  = 1
}wf_bool;


#ifndef WF_RETURN_OK
#define WF_RETURN_OK        0
#endif
#ifndef WF_RETURN_FAIL
#define WF_RETURN_FAIL     (-1)
#endif

#ifndef WF_RETURN_REMOVED_FAIL
#define WF_RETURN_REMOVED_FAIL     (-2)
#endif

#ifndef WF_RETURN_CMD_BUSY
#define WF_RETURN_CMD_BUSY     (-3)
#endif


#endif

