/*
 * wf_debug.h
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
#ifndef __WF_DEBUG_H__
#define __WF_DEBUG_H__

#ifdef __RTTHREAD__

#define DBG_ENABLE
#define DBG_COLOR
#define DBG_SECTION_NAME    "WF"
#define DBG_LVL              DBG_LOG
#include <rtdbg.h>
#define WF_ASSERT    RT_ASSERT

//static inline void log_array_print(void *ptr, wf_u16 len)
//{
//    wf_u16 i = 0;
//    wf_u16 num;
//    wf_u8 *pdata = ptr;
//
//#define NUM_PER_LINE    8
//    rt_kprintf("\r\n");
//    for (i = 0, num = len / NUM_PER_LINE; i < num; i++, pdata += 8)
//    {
//        rt_kprintf("%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
//                 pdata[0], pdata[1], pdata[2], pdata[3],
//                 pdata[4], pdata[5], pdata[6], pdata[7]);
//    }
//    num = len % NUM_PER_LINE;
//    if (num)
//    {
//        for (i = 0; i < num; i++)
//        {
//            rt_kprintf("%02X", pdata[i]);
//        }
//    }
//    rt_kprintf("\r\n");
//}

#endif

#endif      /* END OF __WF_DEBUG_H__ */

