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


#if __linux__

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
#define _WF_DEBUG_HDR(lvl_name, color_n)   printk(KERN_LEVELS "["lvl_name"]")
#define _WF_DEBUG_END   printk("\n")
#endif

#if (WF_DEBUG_LEVEL & WF_DEBUG_DEBUG)
#define LOG_D(fmt, ...)   do {  _WF_DEBUG_HDR("D", 0);   \
                                printk(fmt, ##__VA_ARGS__); \
                                _WF_DEBUG_END;  \
                          }while(0)
#else
#define LOG_D(fmt, ...)
#endif

#if (WF_DEBUG_LEVEL & WF_DEBUG_INFO)
#define LOG_I(fmt, ...)   do {  _WF_DEBUG_HDR("I", 32);   \
                                printk(fmt, ##__VA_ARGS__); \
                                _WF_DEBUG_END;  \
                          }while(0)
#else
#define LOG_I(fmt, ...)
#endif

#if (WF_DEBUG_LEVEL & WF_DEBUG_WARN)
#define LOG_W(fmt, ...)   do {  _WF_DEBUG_HDR("W", 33);   \
                                printk(fmt, ##__VA_ARGS__); \
                                _WF_DEBUG_END;  \
                          }while(0)
#else
#define LOG_W(fmt, ...)
#endif

#if (WF_DEBUG_LEVEL & WF_DEBUG_ERROR)
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
//	for (i = 0, num = len / NUM_PER_LINE; i < num;
//         i++, pdata = &pdata[i * NUM_PER_LINE]) {
    for (i = 0, num = len / NUM_PER_LINE; i < num;
         i++, pdata += 8) {
    	printk(KERN_LEVELS "%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
    	       pdata[0], pdata[1], pdata[2], pdata[3],
    	       pdata[4], pdata[5], pdata[6], pdata[7]);
	}
	num = len % NUM_PER_LINE;
	if (num) {
    	for (i = 0; i < num; i++) {
            printk(KERN_LEVELS "%02X", pdata[i]);
    	}
	}
    printk(KERN_LEVELS "\r\n");
}


#define WF_NEED_CHANGE_DEBUG 0
#endif


#ifdef _WIN32
#define LOG_D(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__func__,__LINE__,##__VA_ARGS__);
#define LOG_N(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__func__,__LINE__,##__VA_ARGS__);
#define LOG_I(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__func__,__LINE__,##__VA_ARGS__);
#define LOG_W(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__func__,__LINE__,##__VA_ARGS__);
#define LOG_E(fmt,...) DbgPrint("[%s,%d] "fmt"\n",__func__,__LINE__,##__VA_ARGS__);

#define WF_ASSERT(EX)  
    
	#ifdef _WIN64
      //define something for Windows (64-bit only)
   #endif
#endif


#endif      /* END OF __WF_DEBUG_H__ */
