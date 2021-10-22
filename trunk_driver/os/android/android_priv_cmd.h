#ifndef __ANDROID_PRIV_CMD_H__
#define __ANDROID_PRIV_CMD_H__

#include "ndev_linux.h"

#define DRIVER_VERSION "9188_v1.1"
#define WIFI_FREQUENCY_BAND_AUTO 0
#define WIFI_FREQUENCY_BAND_2GHZ 2
#ifndef MAC_FMT
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
#ifndef MAC_ARG
#define MAC_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2],((u8*)(x))[3],((u8*)(x))[4],((u8*)(x))[5]
#endif


typedef struct android_usr_cmd_buff_
{
    char *buf;
    int used_len;
    int total_len;
} android_usr_cmd_buff;

#ifdef CONFIG_COMPAT
typedef struct compat_android_usr_cmd_buff_
{
	compat_uptr_t buf;
	int used_len;
	int total_len;
} compat_android_usr_cmd_buff;
#endif

enum ANDROID_WIFI_CMD 
{
	ANDROID_WIFI_CMD_START,
	ANDROID_WIFI_CMD_STOP,
	ANDROID_WIFI_CMD_SCAN_ACTIVE,
	ANDROID_WIFI_CMD_SCAN_PASSIVE,
	ANDROID_WIFI_CMD_RSSI,
	ANDROID_WIFI_CMD_LINKSPEED,
	ANDROID_WIFI_CMD_RXFILTER_START,
	ANDROID_WIFI_CMD_RXFILTER_STOP,
	ANDROID_WIFI_CMD_RXFILTER_ADD,
	ANDROID_WIFI_CMD_RXFILTER_REMOVE,
	ANDROID_WIFI_CMD_BTCOEXSCAN_START,
	ANDROID_WIFI_CMD_BTCOEXSCAN_STOP,
	ANDROID_WIFI_CMD_BTCOEXMODE,
	ANDROID_WIFI_CMD_SETSUSPENDOPT,
	ANDROID_WIFI_CMD_P2P_DEV_ADDR,
	ANDROID_WIFI_CMD_SETFWPATH,
	ANDROID_WIFI_CMD_SETBAND,
	ANDROID_WIFI_CMD_GETBAND,
	ANDROID_WIFI_CMD_COUNTRY,
	ANDROID_WIFI_CMD_P2P_SET_NOA,
	ANDROID_WIFI_CMD_P2P_GET_NOA,
	ANDROID_WIFI_CMD_P2P_SET_PS,
	ANDROID_WIFI_CMD_SET_AP_WPS_P2P_IE,

	ANDROID_WIFI_CMD_MIRACAST,

#ifdef CONFIG_PNO_SUPPORT
	ANDROID_WIFI_CMD_PNOSSIDCLR_SET,
	ANDROID_WIFI_CMD_PNOSETUP_SET,
	ANDROID_WIFI_CMD_PNOENABLE_SET,
	ANDROID_WIFI_CMD_PNODEBUG_SET,
#endif

	ANDROID_WIFI_CMD_MACADDR,

	ANDROID_WIFI_CMD_BLOCK_SCAN,
	ANDROID_WIFI_CMD_BLOCK,

	ANDROID_WIFI_CMD_WFD_ENABLE,
	ANDROID_WIFI_CMD_WFD_DISABLE,

	ANDROID_WIFI_CMD_WFD_SET_TCPPORT,
	ANDROID_WIFI_CMD_WFD_SET_MAX_TPUT,
	ANDROID_WIFI_CMD_WFD_SET_DEVTYPE,
	ANDROID_WIFI_CMD_CHANGE_DTIM,
	ANDROID_WIFI_CMD_HOSTAPD_SET_MACADDR_ACL,
	ANDROID_WIFI_CMD_HOSTAPD_ACL_ADD_STA,
	ANDROID_WIFI_CMD_HOSTAPD_ACL_REMOVE_STA,
	ANDROID_WIFI_CMD_P2P_DISABLE,
	ANDROID_WIFI_CMD_DRIVERVERSION,
	ANDROID_WIFI_CMD_MAX,
	ANDROID_WIFI_CMD_SETSUSPENDMODE
};

typedef struct android_cmd_handle_st_ android_cmd_handle_st;

typedef int (*cmd_handle)(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle);
struct android_cmd_handle_st_
{
    char *cmd_name;
    int cmd_id;
    cmd_handle func;
};

#ifdef CONFIG_PNO_SUPPORT
#define PNO_TLV_PREFIX			'S'
#define PNO_TLV_VERSION			'1'
#define PNO_TLV_SUBVERSION 		'2'
#define PNO_TLV_RESERVED		'0'
#define PNO_TLV_TYPE_SSID_IE	'S'
#define PNO_TLV_TYPE_TIME		'T'
#define PNO_TLV_FREQ_REPEAT		'R'
#define PNO_TLV_FREQ_EXPO_MAX	'M'

#define MAX_PNO_LIST_COUNT 16
#define MAX_SCAN_LIST_COUNT 14
#define MAX_HIDDEN_AP 8


typedef struct cmd_tlv {
	char prefix;
	char version;
	char subver;
	char reserved;
} cmd_tlv_t;

#ifdef CONFIG_PNO_SET_DEBUG
char pno_in_example[] = {
	'P', 'N', 'O', 'S', 'E', 'T', 'U', 'P', ' ',
	'S', '1', '2', '0',
	'S',
	0x05,
	'd', 'l', 'i', 'n', 'k',
	'S',
	0x06,
	'B', 'U', 'F', 'B', 'U', 'F',
	'S',
	0x20,
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
		'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '!', '@', '#',
		'$', '%', '^',
	'S',
	0x0a,
	'!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
	'T',
	'0', '5',
	'R',
	'2',
	'M',
	'2',
	0x00
};
#endif

typedef struct pno_nlo_info {
	u32 fast_scan_period;
	u8 ssid_num;
	u8 hidden_ssid_num;
	u32 slow_scan_period;
	u32 fast_scan_iterations;
	u8 ssid_length[MAX_PNO_LIST_COUNT];
	u8 ssid_cipher_info[MAX_PNO_LIST_COUNT];
	u8 ssid_channel_info[MAX_PNO_LIST_COUNT];
	u8 loc_probe_req[MAX_HIDDEN_AP];
} pno_nlo_info_t;

typedef struct pno_ssid {
	u32 SSID_len;
	u8 SSID[32];
} pno_ssid_t;

typedef struct pno_ssid_list {
	pno_ssid_t node[MAX_PNO_LIST_COUNT];
} pno_ssid_list_t;

typedef struct pno_scan_channel_info {
	u8 channel;
	u8 tx_power;
	u8 timeout;
	u8 active;
} pno_scan_channel_info_t;

typedef struct pno_scan_info {
	u8 enableRFE;
	u8 period_scan_time;
	u8 periodScan;
	u8 orig_80_offset;
	u8 orig_40_offset;
	u8 orig_bw;
	u8 orig_ch;
	u8 channel_num;
	u64 rfe_type;
	pno_scan_channel_info_t ssid_channel_info[MAX_SCAN_LIST_COUNT];
} pno_scan_info_t;
#endif




#endif
