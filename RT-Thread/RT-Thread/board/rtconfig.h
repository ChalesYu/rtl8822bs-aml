/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           rtconfig.h
** Last modified Date:  2015-01-09
** Last Version:        v1.00
** Description:         RT-Thread config file
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-01-09
** Version:             v1.00
** Descriptions:
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
*********************************************************************************************************/
#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
**  ����ϵͳ�������
*********************************************************************************************************/
/* RT_NAME_MAX*/
#define RT_NAME_MAX	8

/* RT_ALIGN_SIZE*/
#define RT_ALIGN_SIZE	4

/* PRIORITY_MAX */
#define RT_THREAD_PRIORITY_MAX	32

/* Tick per Second */
#define RT_TICK_PER_SECOND	200

/* SECTION: RT_DEBUG */
/* Thread Debug */
#define RT_DEBUG
#define RT_USING_OVERFLOW_CHECK

/* Using Hook */
#define RT_USING_HOOK

/* Using Software Timer */
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO		4
#define RT_TIMER_THREAD_STACK_SIZE	512

/* SECTION: IPC */
/* Using Semaphore*/
#define RT_USING_SEMAPHORE

/* Using Mutex */
#define RT_USING_MUTEX

/* Using Event */
#define RT_USING_EVENT

/* Using MailBox */
#define RT_USING_MAILBOX

/* Using Message Queue */
#define RT_USING_MESSAGEQUEUE

/* SECTION: Memory Management */
/* Using Memory Pool Management*/
#define RT_USING_MEMPOOL

/* Using Dynamic Heap Management */
#define RT_USING_HEAP
 
/* Using MemHeap as Dynamic Heap Management */
#define RT_USING_MEMHEAP
#define RT_USING_MEMHEAP_AS_HEAP

/* Using Small MM */
#define RT_USING_SMALL_MEM
  
/*********************************************************************************************************
**  CPU�������
*********************************************************************************************************/
#define RT_USING_CPU_FFS     // Cortex-m�ṩ�����ȼ��㷨��ָ�����ʹ�ܣ���ߵ���Ч��

/*********************************************************************************************************
**  ����������
*********************************************************************************************************/
// <bool name="RT_USING_COMPONENTS_INIT" description="Using RT-Thread components initialization" default="true" />
#define RT_USING_COMPONENTS_INIT
  
/*********************************************************************************************************
**  ʹ���û�������main����
*********************************************************************************************************/
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE   2048
#define RT_MAIN_THREAD_PRIORITY     0

/*********************************************************************************************************
**  IO�豸�������
*********************************************************************************************************/
/* SECTION: Device System */
/* Using Device System */
#define RT_USING_DEVICE
// <bool name="RT_USING_DEVICE_IPC" description="Using device communication" default="true" />
#define RT_USING_DEVICE_IPC
// <bool name="RT_USING_SERIAL" description="Using Serial" default="true" />
#define RT_USING_SERIAL
  
/*********************************************************************************************************
**  ʹ��rtc�Ļ��붨��
*********************************************************************************************************/
#define RT_USING_RTC
//#define RT_USING_SOFT_RTC

/*********************************************************************************************************
**  ʹ��I2C�����豸
*********************************************************************************************************/
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
  
/*********************************************************************************************************
**  ʹ��SPI�����豸
*********************************************************************************************************/
#define RT_USING_SPI
  
/*********************************************************************************************************
**  ʹ��mtd���
*********************************************************************************************************/
//#define RT_USING_MTD_NAND     // mtd nand���ʹ��
#define RT_USING_MTD_NOR      // mtd nor���ʹ��

/*********************************************************************************************************
**  ʹ����Ƶ���
*********************************************************************************************************/
//#define RT_USING_AUDIO 
  
/*********************************************************************************************************
**  ʹ��SDIO
*********************************************************************************************************/
#define RT_USING_SDIO
#define RT_MMCSD_STACK_SIZE         2048
#define RT_SDIO_THREAD_PRIORITY     3
#define RT_SDIO_STACK_SIZE          2048

 
/*********************************************************************************************************
**  ʹ��USB����
*********************************************************************************************************/
//#define RT_USING_USB_HOST
//#define RT_USBH_CDC
//#define USB_CLASS_CDC_CODE      0xFF  //0x02  //0xFF
//#define USB_SUB_CLASS_CDC_CODE  0x00
//#define RT_USBH_CDC_TTY
//#define RT_USE_TTYUSB0
//#define RT_USE_TTYUSB1
//#define RT_USE_TTYUSB2
//#define RT_USE_TTYUSB3
//#define RT_USE_TTYUSB4
//#define RT_USE_TTYUSB5
  
/*********************************************************************************************************
**  WIFI�������
*********************************************************************************************************/
#define RT_USING_WIFI
#define RT_WLAN_DEVICE_STA_NAME         "wlan0"
#define RT_WLAN_DEVICE_AP_NAME          "wlan1"
#define RT_WLAN_SSID_MAX_LENGTH         32
#define RT_WLAN_PASSWORD_MAX_LENGTH     32
#define RT_WLAN_DEV_EVENT_NUM           2
#define RT_WLAN_MANAGE_ENABLE
#define RT_WLAN_SCAN_WAIT_MS            10000
#define RT_WLAN_CONNECT_WAIT_MS         10000
#define RT_WLAN_SCAN_SORT
#define RT_WLAN_MSH_CMD_ENABLE
#define RT_WLAN_AUTO_CONNECT_ENABLE
#define AUTO_CONNECTION_PERIOD_MS       2000
#define RT_WLAN_CFG_ENABLE
#define RT_WLAN_CFG_INFO_MAX            3
#define RT_WLAN_PROT_ENABLE
#define RT_WLAN_PROT_NAME_LEN           8
#define RT_WLAN_PROT_MAX                2
#define RT_WLAN_DEFAULT_PROT            "lwip"
#define RT_WLAN_PROT_LWIP_ENABLE
#define RT_WLAN_PROT_LWIP_NAME          "lwip"
#define RT_WLAN_PROT_LWIP_PBUF_FORCE
#define RT_WLAN_WORK_THREAD_ENABLE
#define RT_WLAN_WORKQUEUE_THREAD_NAME   "wlan"
#define RT_WLAN_WORKQUEUE_THREAD_SIZE   2048
#define RT_WLAN_WORKQUEUE_THREAD_PRIO   15
//#define RT_WLAN_DEBUG
//#define RT_WLAN_CMD_DEBUG
//#define RT_WLAN_MGNT_DEBUG
//#define RT_WLAN_DEV_DEBUG
//#define RT_WLAN_PROT_DEBUG
//#define RT_WLAN_CFG_DEBUG
//#define RT_WLAN_LWIP_DEBUG

/*********************************************************************************************************
**  ����̨�������
*********************************************************************************************************/
/* SECTION: Console options */
#define RT_USING_CONSOLE
/* the buffer size of console*/
#define RT_CONSOLEBUF_SIZE	        128
// <string name="RT_CONSOLE_DEVICE_NAME" description="The device name for console" default="uart1" />
#define RT_CONSOLE_DEVICE_NAME	    "ttyS0"

/*********************************************************************************************************
**  shell�����������
*********************************************************************************************************/
#define RT_USING_FINSH
#define FINSH_THREAD_NAME               "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES             5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY           28
#define FINSH_THREAD_STACK_SIZE         16*1024 //4096
#define FINSH_CMD_SIZE                  256
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_MSH_ONLY
#define FINSH_ARG_MAX                   16

/*********************************************************************************************************
**  ��׼C���������
*********************************************************************************************************/
#define RT_USING_LIBC
// <bool name="RT_USING_PTHREADS" description="Using POSIX threads library" default="true" />

/*********************************************************************************************************
**  POSIX֧��
*********************************************************************************************************/
#define RT_USING_POSIX
//#define RT_USING_PTHREADS

/*********************************************************************************************************
**  �ļ�ϵͳ�������
*********************************************************************************************************/
/* SECTION: device filesystem */
#define RT_USING_DFS

// ʹ���Զ����ر�
#define RT_USING_DFS_MNTTABLE

#define RT_USING_DFS_DEVFS

  /* Use Using working directory */
#define DFS_USING_WORKDIR
 
  /* the max number of filesystem types */
#define DFS_FILESYSTEM_TYPES_MAX        4
  
  /* the max number of mounted filesystem */
#define DFS_FILESYSTEMS_MAX             3
  /* the max number of opened files       */
#define DFS_FD_MAX                      64

/*********************************************************************************************************
**  elm fat�������
*********************************************************************************************************/
#define RT_USING_DFS_ELMFAT
/* Use exFat, support fat8, fat16, fat32, fat64.  */
#define RT_DFS_ELM_USE_EXFAT
/* Reentrancy (thread safe) of the FatFs module.  */
#define RT_DFS_ELM_REENTRANT
/* Number of volumes (logical drives) to be used. */
#define RT_DFS_ELM_DRIVES               2
#define RT_DFS_ELM_USE_LFN              3
//#define RT_DFS_ELM_LFN_UNICODE
#define RT_DFS_ELM_CODE_PAGE            437
// <bool name="RT_DFS_ELM_CODE_PAGE_FILE" description="Using OEM code page file" default="false" />
#define RT_DFS_ELM_CODE_PAGE_FILE
#define RT_DFS_ELM_MAX_LFN              128
/* Maximum sector size to be handled. */
#define RT_DFS_ELM_MAX_SECTOR_SIZE      512

/*********************************************************************************************************
**  uffs�������
*********************************************************************************************************/
///* configration for uffs, more to see dfs_uffs.h and uffs_config.h */
//#define RT_USING_DFS_UFFS
//#define RT_CONFIG_UFFS_ECC_MODE  UFFS_ECC_HW_AUTO
///* enable this ,you need provide a mark_badblock/check_block funciton */
///* #define RT_UFFS_USE_CHECK_MARK_FUNCITON */
  
/*********************************************************************************************************
**  �����������
*********************************************************************************************************/
/* SECTION: lwip, a lighwight TCP/IP protocol stack */
#define RT_USING_LWIP
#define RT_LWIP_DHCP
/* LwIP uses RT-Thread Memory Management */
//#define RT_LWIP_USING_RT_MEM
/* the number of simulatenously active TCP connections*/
//#define RT_LWIP_TCP_PCB_NUM	5
#ifdef RT_USING_POSIX 
#define RT_USING_SAL
#define SAL_USING_POSIX
#endif
  
#define LWIP_USING_DHCPD

/*********************************************************************************************************
**  ��̫��֧��
*********************************************************************************************************/
#define RT_USE_ETHERNET
#define RT_LWIP_NO_TX_THREAD
//#define RT_LWIP_NO_RX_THREAD
/* ethernet if thread options */
#define RT_LWIP_ETHTHREAD_PRIORITY		2
#define RT_LWIP_ETHTHREAD_MBOX_SIZE		16
#define RT_LWIP_ETHTHREAD_STACKSIZE		512

/*********************************************************************************************************
**  PPPOS֧�֣�����ʹ��lwip ppp����
*********************************************************************************************************/
//#define RT_USE_PPPOS
//#define RT_LWIP_PPPOS_PRIORITY		3
//#define RT_LWIP_PPPOS_STACKSIZE		2048

/*********************************************************************************************************
**  6LoWPAN֧��
*********************************************************************************************************/
//#define RT_USE_6LOWPAN
//#define RT_LWIP_6LOWPANTHREAD_PRIORITY		3
//#define RT_LWIP_6LOWPANTHREAD_MBOX_SIZE		16
//#define RT_LWIP_6LOWPANTHREAD_STACKSIZE		512

/*********************************************************************************************************
**  RT GUI�������
*********************************************************************************************************/
/* SECTION: RT-Thread/GUI */
/* #define RT_USING_RTGUI */

/* name length of RTGUI object */
#define RTGUI_NAME_MAX		12
/* support 16 weight font */
#define RTGUI_USING_FONT16
/* support Chinese font */
#define RTGUI_USING_FONTHZ
/* use DFS as file interface */
#define RTGUI_USING_DFS_FILERW
/* use font file as Chinese font */
#define RTGUI_USING_HZ_FILE
/* use Chinese bitmap font */
#define RTGUI_USING_HZ_BMP
/* use small size in RTGUI */
#define RTGUI_USING_SMALL_SIZE
/* use mouse cursor */
/* #define RTGUI_USING_MOUSE_CURSOR */
/* default font size in RTGUI */
#define RTGUI_DEFAULT_FONT_SIZE	16

/* image support */
/* #define RTGUI_IMAGE_XPM */
/* #define RTGUI_IMAGE_BMP */
  
/*********************************************************************************************************
**  ����������һЩ����
*********************************************************************************************************/ 
#define PKG_NETUTILS_PING
#define PKG_NETUTILS_IPERF
//#define PKG_NETUTILS_TELNET
//  NTP��������
#define PKG_NETUTILS_NTP
#define NTP_HOSTNAME                   "ntp.aliyun.com"
  
//  TLSF�ڴ�����㷨
#define PKG_USING_TLSF
  
/*********************************************************************************************************
**  core mark�ֹܷ���
*********************************************************************************************************/ 
//#define COREMARK_USE_FLOAT
//#define CORE_MARK_HAS_FLOAT     1
#define COMPILER_VERSION        "iar ewarm"
#define COREMARK_ITERATIONS     36000
  
/*********************************************************************************************************
**  vi��������
*********************************************************************************************************/ 
#define VI_SANDBOX_SIZE_KB   20 /* KB */
#define VI_MAX_LEN           4096
//#define VI_ENABLE_8BIT
#define VI_ENABLE_COLON
#define VI_ENABLE_COLON_EXPAND
#define VI_ENABLE_YANKMARK
#define VI_ENABLE_SEARCH
#define VI_ENABLE_DOT_CMD
#define VI_ENABLE_READONLY
#define VI_ENABLE_SETOPTS
#define VI_ENABLE_SET
//#define VI_ENABLE_WIN_RESIZE  
#define VI_ENABLE_VI_ASK_TERMINAL
#define VI_ENABLE_UNDO
#define VI_ENABLE_UNDO_QUEUE
#define VI_UNDO_QUEUE_MAX   256
#define VI_ENABLE_VERBOSE_STATUS
  
  
/*********************************************************************************************************
**  MQTTЭ��ջ����
*********************************************************************************************************/ 
// ��������ĵ��������
#define MQTT_MAX_MESSAGE_HANDLERS    4
//// ����tsl���ܴ��䣬��Ҫ����mbedtls
//#define MQTT_USING_TLS
  
/*********************************************************************************************************
**  WEB������webnet����
*********************************************************************************************************/ 
#define WEBNET_PRIORITY                 11           // web�������߳����ȼ�
#define WEBNET_THREAD_STACKSIZE         (2 * 1024)   // web�������߳�ջ��С

// ��������
#define WEBNET_PORT             80              // web�������˿�
#define WEBNET_CONN_MAX         4               // web���������֧��������
#define WEBNET_ROOT             "/webnet"       // web����������Ŀ¼ 
  
// ����ѡ��
//#define WEBNET_USING_LOG
#define WEBNET_USING_ASP
#define WEBNET_USING_AUTH
#define WEBNET_USING_CGI
#define WEBNET_USING_DAV
#define WEBNET_USING_INDEX
#define WEBNET_USING_ALIAS
//#define WEBNET_USING_LUA
#define WEBNET_USING_SSI
#define WEBNET_USING_UPLOAD
  

/*********************************************************************************************************
**  S9188��������
*********************************************************************************************************/ 
#define PKG_USING_S9188
#define PKG_S9188_USING_SDIO
#define PKG_S9188_FILE_FIRMWARE        "/lib/firmware/fw_9188_r1751.bin"
#define PKG_S9188_FILE_CFG             "/etc/wifi.cfg"
#define PKG_S9188_MODE_STA
#define PKG_S9188_XMIT_FRAME_NUM       32
#define PKG_S9188_XMIT_DATA_NUM        8
#define PKG_S9188_RX_MEM_NUM           16

#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif		 // __RTTHREAD_CFG_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
