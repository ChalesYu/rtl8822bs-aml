/*
 * wpa_supplicant/hostapd / common helper functions, etc.
 * Copyright (c) 2002-2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef __STA_COMMON_H__
#define __STA_COMMON_H__

#include "os.h"

/* Define platform specific integer types */

static inline wf_u16 WPA_GET_BE16(const wf_u8 *a)
{
	return (a[0] << 8) | a[1];
}

static inline void WPA_PUT_BE16(wf_u8 *a, wf_u16 val)
{
	a[0] = val >> 8;
	a[1] = val & 0xff;
}

static inline wf_u16 WPA_GET_LE16(const wf_u8 *a)
{
	return (a[1] << 8) | a[0];
}

static inline void WPA_PUT_LE16(wf_u8 *a, wf_u16 val)
{
	a[1] = val >> 8;
	a[0] = val & 0xff;
}

static inline wf_u32 WPA_GET_BE24(const wf_u8 *a)
{
	return (a[0] << 16) | (a[1] << 8) | a[2];
}

static inline void WPA_PUT_BE24(wf_u8 *a, wf_u32 val)
{
	a[0] = (val >> 16) & 0xff;
	a[1] = (val >> 8) & 0xff;
	a[2] = val & 0xff;
}

static inline wf_u32 WPA_GET_BE32(const wf_u8 *a)
{
	return ((wf_u32) a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3];
}

static inline void WPA_PUT_BE32(wf_u8 *a, wf_u32 val)
{
	a[0] = (val >> 24) & 0xff;
	a[1] = (val >> 16) & 0xff;
	a[2] = (val >> 8) & 0xff;
	a[3] = val & 0xff;
}

static inline wf_u32 WPA_GET_LE32(const wf_u8 *a)
{
	return ((wf_u32) a[3] << 24) | (a[2] << 16) | (a[1] << 8) | a[0];
}

static inline void WPA_PUT_LE32(wf_u8 *a, wf_u32 val)
{
	a[3] = (val >> 24) & 0xff;
	a[2] = (val >> 16) & 0xff;
	a[1] = (val >> 8) & 0xff;
	a[0] = val & 0xff;
}

static inline wf_u64 WPA_GET_BE64(const wf_u8 *a)
{
	return (((wf_u64) a[0]) << 56) | (((wf_u64) a[1]) << 48) |
		(((wf_u64) a[2]) << 40) | (((wf_u64) a[3]) << 32) |
		(((wf_u64) a[4]) << 24) | (((wf_u64) a[5]) << 16) |
		(((wf_u64) a[6]) << 8) | ((wf_u64) a[7]);
}

static inline void WPA_PUT_BE64(wf_u8 *a, wf_u64 val)
{
	a[0] = val >> 56;
	a[1] = val >> 48;
	a[2] = val >> 40;
	a[3] = val >> 32;
	a[4] = val >> 24;
	a[5] = val >> 16;
	a[6] = val >> 8;
	a[7] = val & 0xff;
}

static inline wf_u64 WPA_GET_LE64(const wf_u8 *a)
{
	return (((wf_u64) a[7]) << 56) | (((wf_u64) a[6]) << 48) |
		(((wf_u64) a[5]) << 40) | (((wf_u64) a[4]) << 32) |
		(((wf_u64) a[3]) << 24) | (((wf_u64) a[2]) << 16) |
		(((wf_u64) a[1]) << 8) | ((wf_u64) a[0]);
}

static inline void WPA_PUT_LE64(wf_u8 *a, wf_u64 val)
{
	a[7] = val >> 56;
	a[6] = val >> 48;
	a[5] = val >> 40;
	a[4] = val >> 32;
	a[3] = val >> 24;
	a[2] = val >> 16;
	a[1] = val >> 8;
	a[0] = val & 0xff;
}

#define ETH_ALEN             6
#define ETH_HLEN             14
#define ETH_P_ALL            0x0003
#define ETH_P_IP             0x0800
#define ETH_P_80211_ENCAP    0x890d /* TDLS comes under this category */
#define ETH_P_PAE            0x888E /* Port Access Entity (IEEE 802.1X) */
#define ETH_P_EAPOL          ETH_P_PAE
#define ETH_P_RSN_PREAUTH    0x88c7
#define ETH_P_RRB            0x890D
#define PRINTF_FORMAT(a,b)   __attribute__ ((format (printf, (a), (b))))
#define STRUCT_PACKED        __attribute__ ((packed))

/*
 * Definitions for sparse validation
 * (http://kernel.org/pub/linux/kernel/people/josh/sparse/)
 */
#define __force
#define __bitwise

typedef wf_u16 __bitwise be16;
typedef wf_u16 __bitwise le16;
typedef wf_u32 __bitwise be32;
typedef wf_u32 __bitwise le32;
typedef wf_u64 __bitwise be64;
typedef wf_u64 __bitwise le64;      
typedef wf_u16 __bitwise sum16;
typedef wf_u32 __bitwise wsum; 


#define SSID_MAX_LEN 32


#define wpa_unicode2ascii_inplace(s) do { } while (0)
#define wpa_strdup_tchar(s) strdup((s))


static inline int is_zero_ether_addr(const wf_u8 *a)
{
	return !(a[0] | a[1] | a[2] | a[3] | a[4] | a[5]);
}

static inline int is_broadcast_ether_addr(const wf_u8 *a)
{
	return (a[0] & a[1] & a[2] & a[3] & a[4] & a[5]) == 0xff;
}

static inline int is_multicast_ether_addr(const wf_u8 *a)
{
	return a[0] & 0x01;
}

#define broadcast_ether_addr (const wf_u8 *) "\xff\xff\xff\xff\xff\xff"


#endif /* __STA_COMMON_H__ */
