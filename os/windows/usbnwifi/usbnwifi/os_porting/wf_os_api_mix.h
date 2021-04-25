#pragma once
#ifndef __WF_OS_API_MIX_H__

#ifndef WF_OFFSETOF
#define WF_OFFSETOF(type, field)        ((SIZE_T)&((type *)0)->field)
#endif

#ifndef WF_FIELD_SIZEOF
#define WF_FIELD_SIZEOF(type, field)    (sizeof(((type *)0)->field))
#endif

#ifndef WF_CONTAINER_OF
#define WF_CONTAINER_OF(ptr, type, field) \
    ((type *)((wf_u8 *)ptr - WF_OFFSETOF(type, field)))
#endif

typedef unsigned long wf_irq;
typedef int wf_tasklet;
typedef int loff_t;

#define wf_packed


void *wf_malloc(wf_u32 size);
void wf_free(void *ptr);
void wf_Msleep(wf_u32 MilliSecond);
void wf_Usleep(wf_u32 MilliSecond);
wf_u32 wf_os_api_rand32(void);
void wf_os_api_set_scan_finish_flag(void *arg);
void wf_os_api_set_connect_flag(void *arg, wf_u8 *bssid);
void wf_os_api_set_disconnect_flag(void *arg);
void wf_os_api_enable_all_data_queue(void *arg);
void wf_os_api_disable_all_data_queue(void *arg);

#define WF_TIMESTAMP               wf_win_tick_count()
#define WF_HZ                      wf_win_hz()

#define do_div(n,base)             (n%base)
#define wf_yield()                   (wf_Msleep(0))
#define wf_inline                  __inline

#define wf_memcpy                  memcpy
#define wf_memcmp                  memcmp
#define wf_memset                  memset
#define wf_mdelay                  wf_Msleep
#define wf_udelay                  wf_Usleep
#define wf_msleep                  wf_Msleep

#define wf_kzalloc(sz)             wf_malloc(sz)
#define wf_kfree                   wf_free
#define wf_vmalloc                 wf_malloc
#define wf_vfree                   wf_free
#define wf_alloc_skb(sz)
#define wf_free_skb


#define ___constant_swab16(x) ((wf_u16)(				\
	(((wf_u16)(x) & (wf_u16)0x00ffU) << 8) |			\
	(((wf_u16)(x) & (wf_u16)0xff00U) >> 8)))

#define ___constant_swab32(x) ((wf_u32)(				\
	(((wf_u32)(x) & (wf_u32)0x000000ffUL) << 24) |		\
	(((wf_u32)(x) & (wf_u32)0x0000ff00UL) <<  8) |		\
	(((wf_u32)(x) & (wf_u32)0x00ff0000UL) >>  8) |		\
	(((wf_u32)(x) & (wf_u32)0xff000000UL) >> 24)))

#define ___constant_swab64(x) ((wf_u64)(				\
	(((wf_u64)(x) & (wf_u64)0x00000000000000ffULL) << 56) |	\
	(((wf_u64)(x) & (wf_u64)0x000000000000ff00ULL) << 40) |	\
	(((wf_u64)(x) & (wf_u64)0x0000000000ff0000ULL) << 24) |	\
	(((wf_u64)(x) & (wf_u64)0x00000000ff000000ULL) <<  8) |	\
	(((wf_u64)(x) & (wf_u64)0x000000ff00000000ULL) >>  8) |	\
	(((wf_u64)(x) & (wf_u64)0x0000ff0000000000ULL) >> 24) |	\
	(((wf_u64)(x) & (wf_u64)0x00ff000000000000ULL) >> 40) |	\
	(((wf_u64)(x) & (wf_u64)0xff00000000000000ULL) >> 56)))

#define ___constant_swahw32(x) ((wf_u32)(			\
	(((wf_u32)(x) & (wf_u32)0x0000ffffUL) << 16) |		\
	(((wf_u32)(x) & (wf_u32)0xffff0000UL) >> 16)))

#define ___constant_swahb32(x) ((wf_u32)(			\
	(((wf_u32)(x) & (wf_u32)0x00ff00ffUL) << 8) |		\
	(((wf_u32)(x) & (wf_u32)0xff00ff00UL) >> 8)))

#define __constant_cpu_to_le64(x) ((  wf_u64)___constant_swab64((x)))
#define __constant_le64_to_cpu(x) ___constant_swab64((  wf_u64)(wf_u64)(x))
#define __constant_cpu_to_le32(x) ((  wf_u32)___constant_swab32((x)))
#define __constant_le32_to_cpu(x) ___constant_swab32((  wf_u32)(wf_u32)(x))
#define __constant_cpu_to_le16(x) ((  wf_u16)___constant_swab16((x)))
#define __constant_le16_to_cpu(x) ___constant_swab16((  wf_u16)(wf_u16)(x))

#define __cpu_to_le64(x) ((  wf_u64)___constant_swab64((x)))
#define __le64_to_cpu(x) ___constant_swab64((  wf_u64)(wf_u64)(x))
#define __cpu_to_le32(x) ((  wf_u32)___constant_swab32((x)))
#define __le32_to_cpu(x) ___constant_swab32((  wf_u32)(wf_u32)(x))
#define __cpu_to_le16(x) ((  wf_u16)___constant_swab16((x)))
#define __le16_to_cpu(x) ___constant_swab16((  wf_u16)(wf_u16)(x))
#define __cpu_to_be64(x) ((  wf_u64)(wf_u64)(x))
#define __be64_to_cpu(x) ((  wf_u64)(wf_u64)(x))
#define __cpu_to_be32(x) ((  wf_u32)(wf_u32)(x))
#define __be32_to_cpu(x) ((  wf_u32)(wf_u32)(x))
#define __cpu_to_be16(x) ((  wf_u16)(wf_u16)(x))
#define __be16_to_cpu(x) ((  wf_u16)(wf_u16)(x)) 

#define htonl(x) __cpu_to_be32(x)
#define ntohl(x) __be32_to_cpu(x)
#define htons(x) __cpu_to_be16(x)
#define ntohs(x) __be16_to_cpu(x)

#define le64_to_cpu __le64_to_cpu
#define cpu_to_le32 __cpu_to_le32
#define le32_to_cpu __le32_to_cpu
#define cpu_to_le16 __cpu_to_le16
#define le16_to_cpu __le16_to_cpu
#define cpu_to_be64 __cpu_to_be64
#define be64_to_cpu __be64_to_cpu
#define cpu_to_be32 __cpu_to_be32
#define be32_to_cpu __be32_to_cpu
#define cpu_to_be16 __cpu_to_be16
#define be16_to_cpu __be16_to_cpu



#define wf_le16_to_cpu      le16_to_cpu
#define wf_cpu_to_le16      cpu_to_le16
#define wf_be16_to_cpu      be16_to_cpu
#define wf_cpu_to_be16      cpu_to_be16

#define wf_le32_to_cpu      le32_to_cpu
#define wf_cpu_to_le32      cpu_to_le32
#define wf_be32_to_cpu      be32_to_cpu
#define wf_cpu_to_be32      cpu_to_be32

#define wf_le64_to_cpu      le64_to_cpu
#define wf_cpu_to_le64      cpu_to_le64
#define wf_be64_to_cpu      be64_to_cpu
#define wf_cpu_to_be64      cpu_to_be64
#endif
