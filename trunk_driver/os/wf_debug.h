/*
 * wf_debug.h
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
#ifndef __WF_DEBUG_H__
#define __WF_DEBUG_H__


#ifndef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL   0
#endif

#define WF_DEBUG_DEBUG         0x01U
#define WF_DEBUG_INFO          0x02U
#define WF_DEBUG_WARN          0x04U
#define WF_DEBUG_ERROR         0x08U
#define WF_DEBUG_MASK          0x0FU

#ifdef __linux__

#define KERN_LEVELS         KERN_ALERT /* this use to set printk funcation output
                                          level with specify level. the set value
                                          should highter than system default console
                                          level(usually equal to KERN_WARNING). */

/*
 * The color for terminal (foreground)
 * BLACK    30
 * RED      31
 * GREEN    32
 * YELLOW   33
 * BLUE     34
 * PURPLE   35
 * CYAN     36
 * WHITE    37
 */
#ifdef WF_DEBUG_COLOR
#define _WF_DEBUG_HDR(lvl_name, color_n)    \
        printk(KERN_LEVELS "\033["#color_n"m["lvl_name"]")
#define _WF_DEBUG_END   printk("\033[0m\n")
#else
// #define _WF_DEBUG_HDR(lvl_name, color_n)   printk(KERN_LEVELS "["lvl_name"]")
// #define _WF_DEBUG_END   printk("\n")
#define _WF_DEBUG_HDR(lvl_name, color_n)   //printk(" ")
#define _WF_DEBUG_END   //printk(" ")
#endif

#if 1 //(WF_DEBUG_LEVEL & WF_DEBUG_DEBUG)
#define LOG_D(fmt, ...)   do {  _WF_DEBUG_HDR("D", 0);   \
                                printk(fmt, ##__VA_ARGS__); \
                                _WF_DEBUG_END;  \
                          }while(0)
#else
#define LOG_D(fmt, ...)
#endif

#if 1 //(WF_DEBUG_LEVEL & WF_DEBUG_INFO)
#define LOG_I(fmt, ...)   do {  _WF_DEBUG_HDR("I", 32);   \
                                printk(fmt, ##__VA_ARGS__); \
                                _WF_DEBUG_END;  \
                          }while(0)
#else
#define LOG_I(fmt, ...)
#endif

#if 1 //(WF_DEBUG_LEVEL & WF_DEBUG_WARN)
#define LOG_W(fmt, ...)   do {  _WF_DEBUG_HDR("W", 33);   \
                                printk(fmt, ##__VA_ARGS__); \
                                _WF_DEBUG_END;  \
                          }while(0)
#else
#define LOG_W(fmt, ...)
#endif

#if 1 //(WF_DEBUG_LEVEL & WF_DEBUG_ERROR)
#define LOG_E(fmt, ...)   do {  _WF_DEBUG_HDR("E", 31);   \
                                printk(fmt, ##__VA_ARGS__); \
                                _WF_DEBUG_END;  \
                          }while(0)
#else
#define LOG_E(fmt, ...)
#endif

#define     WF_ASSERT(EX)                                     \
if (!(EX))                                                    \
{                                                             \
    LOG_E("#EX assertion failed at function:%s, line number:%d \n", __FUNCTION__, __LINE__);\
    BUG();                                                 \
}

#include "wf_os_api.h"
static wf_inline void log_array (void *ptr, wf_u16 len)
{
    wf_u16 i = 0;
    wf_u16 num;
    wf_u8 *pdata = ptr;

#define NUM_PER_LINE    8
    printk(KERN_LEVELS "\r\n");
//  for (i = 0, num = len / NUM_PER_LINE; i < num;
//         i++, pdata = &pdata[i * NUM_PER_LINE]) {
    for (i = 0, num = len / NUM_PER_LINE; i < num;
         i++, pdata += 8)
    {
        printk(KERN_LEVELS "%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
               pdata[0], pdata[1], pdata[2], pdata[3],
               pdata[4], pdata[5], pdata[6], pdata[7]);
    }
    num = len % NUM_PER_LINE;
    if (num)
    {
        for (i = 0; i < num; i++)
        {
            printk(KERN_LEVELS "%02X", pdata[i]);
        }
    }
    printk(KERN_LEVELS "\r\n");
}

#endif


#ifdef _WIN32
#define LOG_D(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);
#define LOG_N(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);
#define LOG_I(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);
#define LOG_W(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);
#define LOG_E(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);

#define WF_ASSERT(EX)

#include "wf_os_api.h"
static wf_inline void log_array (void *ptr, wf_u16 len)
{
    wf_u16 i = 0;
    wf_u16 num;
    wf_u8 *pdata = ptr;

#define NUM_PER_LINE    8
    //  for (i = 0, num = len / NUM_PER_LINE; i < num;
    //         i++, pdata = &pdata[i * NUM_PER_LINE]) {
    DbgPrint("\r\n");
    for (i = 0, num = len / NUM_PER_LINE; i < num;
         i++, pdata += 8)
    {
        DbgPrint("%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                 pdata[0], pdata[1], pdata[2], pdata[3],
                 pdata[4], pdata[5], pdata[6], pdata[7]);
    }
    num = len % NUM_PER_LINE;
    if (num)
    {
        for (i = 0; i < num; i++)
        {
            DbgPrint("%02X", pdata[i]);
        }
    }
    DbgPrint( "\r\n");
}


#ifdef _WIN64
//define something for Windows (64-bit only)
#endif
#endif

#ifdef __RTTHREAD__
#if 1

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

#else

#define LOG_D(fmt,...)
#define LOG_N(fmt,...)
#define LOG_I(fmt,...)
#define LOG_W(fmt,...)
#define LOG_E(fmt,...)
#define WF_ASSERT(EX)

#endif
#endif

#ifdef __RTOS__

#ifndef WF_DEBUG_COLOR
#define WF_DEBUG_COLOR 0
#endif

#ifndef WF_LOG_PRINT
#define WF_LOG_PRINT(...)
#endif

/*
 * The color for terminal (foreground)
 * BLACK    30
 * RED      31
 * GREEN    32
 * YELLOW   33
 * BLUE     34
 * PURPLE   35
 * CYAN     36
 * WHITE    37
 */

#if WF_DEBUG_COLOR
#define WF_COLOR_BEGIN(lvl_name, color_n)   "\033["#color_n"m["lvl_name"]"
#define WF_COLOR_END                        "\033[0m""\r\n"
#else
#define WF_COLOR_BEGIN(lvl_name, color_n) "["lvl_name"]"
#define WF_COLOR_END                      "\n"
#endif

#if (WF_DEBUG_LEVEL & LEVEL_DEBUG)
#define LOG_D(fmt, ...) \
    WF_LOG_PRINT(WF_COLOR_BEGIN("D", 0) fmt WF_COLOR_END, ##__VA_ARGS__)
#else
#define LOG_D(fmt, ...)
#endif

#if (WF_DEBUG_LEVEL & LEVEL_INFO)
#define LOG_I(fmt, ...) \
    WF_LOG_PRINT(WF_COLOR_BEGIN("I", 32) fmt WF_COLOR_END, ##__VA_ARGS__)
#else
#define LOG_I(fmt, ...)
#endif

#if (WF_DEBUG_LEVEL & LEVEL_WARN)
#define LOG_W(fmt, ...) \
    WF_LOG_PRINT(WF_COLOR_BEGIN("W", 33) fmt WF_COLOR_END, ##__VA_ARGS__)
#else
#define LOG_W(fmt, ...)
#endif

#if (WF_DEBUG_LEVEL & LEVEL_ERROR)
#define LOG_E(fmt, ...) \
    WF_LOG_PRINT(WF_COLOR_BEGIN("E", 31) fmt WF_COLOR_END, ##__VA_ARGS__)
#else
#define LOG_E(fmt, ...)
#endif

#ifndef log_array
#define log_array(...)
#endif

#endif

#endif /* END OF __WF_DEBUG_H__ */

