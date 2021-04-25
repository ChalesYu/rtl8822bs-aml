/******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef __OSDEP_SERVICE_H_
#define __OSDEP_SERVICE_H_

#ifndef BIT
#define BIT(x)	( 1 << (x))
#endif

#if 0
enum mstat_f {
	MSTAT_TYPE_VIR = 0x00,
	MSTAT_TYPE_PHY = 0x01,
	MSTAT_TYPE_SKB = 0x02,
	MSTAT_TYPE_USB = 0x03,
	MSTAT_TYPE_MAX = 0x04,

	MSTAT_FUNC_UNSPECIFIED = 0x00 << 8,
	MSTAT_FUNC_IO = 0x01 << 8,
	MSTAT_FUNC_TX_IO = 0x02 << 8,
	MSTAT_FUNC_RX_IO = 0x03 << 8,
	MSTAT_FUNC_TX = 0x04 << 8,
	MSTAT_FUNC_RX = 0x05 << 8,
	MSTAT_FUNC_CFG_VENDOR = 0x06 << 8,
	MSTAT_FUNC_MAX = 0x07 << 8,
};

#define mstat_tf_idx(flags) ((flags)&0xff)
#define mstat_ff_idx(flags) (((flags)&0xff00) >> 8)

typedef enum mstat_status {
	MSTAT_ALLOC_SUCCESS = 0,
	MSTAT_ALLOC_FAIL,
	MSTAT_FREE
} MSTAT_STATUS;


#define Func_Of_Proc_Mstat_Update(flag, status, sz) do {} while(0)
#endif

u8 *Func_Of_Proc_Pre_Vmalloc(u32 sz);
u8 *Func_Of_Proc_Pre_Zvmalloc(u32 sz);
void Func_Of_Proc_Pre_Vmfree(u8 * pbuf, u32 sz);
u8 *Func_Of_Proc_Pre_Zmalloc(u32 sz);
u8 *Func_Of_Proc_Pre_Malloc(u32 sz);
void Func_Of_Proc_Pre_Mfree(u8 * pbuf, u32 sz);

#define wl_vmalloc(sz)			Func_Of_Proc_Pre_Malloc((sz))
#define wl_zvmalloc(sz)			Func_Of_Proc_Pre_Zmalloc((sz))
#define wl_vmfree(pbuf, sz)		Func_Of_Proc_Pre_Mfree((pbuf), (sz))
#define wl_vmalloc_f(sz, mstat_f)			Func_Of_Proc_Pre_Malloc((sz))
#define wl_zvmalloc_f(sz, mstat_f)		Func_Of_Proc_Pre_Zmalloc((sz))
#define wl_vmfree_f(pbuf, sz, mstat_f)	Func_Of_Proc_Pre_Mfree((pbuf), (sz))

#define wl_malloc(sz)			Func_Of_Proc_Pre_Malloc((sz))
#define wl_zmalloc(sz)			Func_Of_Proc_Pre_Zmalloc((sz))
#define wl_mfree(pbuf, sz)		Func_Of_Proc_Pre_Mfree((pbuf), (sz))
#define wl_malloc_f(sz, mstat_f)			Func_Of_Proc_Pre_Malloc((sz))
#define wl_zmalloc_f(sz, mstat_f)			Func_Of_Proc_Pre_Zmalloc((sz))
#define wl_mfree_f(pbuf, sz, mstat_f)		Func_Of_Proc_Pre_Mfree((pbuf), (sz))


 void Func_Of_Proc_Pre_Memcpy(void *dec, const void *sour, u32 sz);
 void Func_Of_Proc_Pre_Memmove(void *dst, const void *src, u32 sz);
 int Func_Of_Proc_Pre_Memcmp(void *dst, const void *src, u32 sz);
 void Func_Of_Proc_Pre_Memset(void *pbuf, int c, u32 sz);

 void Func_Of_Proc_Init_Listhead(LIST_ENTRY * list);
 u32 Func_Of_Proc_Is_List_Empty(LIST_ENTRY * phead);
 void Func_Of_Proc_List_Insert_Head(LIST_ENTRY * plist, LIST_ENTRY * phead);
 void Func_Of_Proc_List_Insert_Tail(LIST_ENTRY * plist, LIST_ENTRY * phead);

 void wl_list_delete(LIST_ENTRY * plist);

 void Func_Of_Proc_Pre_Init_Sema(KSEMAPHORE * sema, int init_val);
 void Func_Of_Procw_Free_Sema(KSEMAPHORE * sema);
 void Func_Of_Proc_Pre_Up_Sema(KSEMAPHORE * sema);
 u32 Func_Of_Proc_Down_Sema(KSEMAPHORE * sema);
 void Func_Of_Proc_Pre_Mutex_Init(NDIS_MUTEX * pmutex);
 void Func_Of_Proc_Pre_Mutex_Free(NDIS_MUTEX * pmutex);

 void Func_Of_Proc_Pre_Spinlock_Init(KSPIN_LOCK * plock);
 void Func_Of_Proc_Pre_Spinlock_Free(KSPIN_LOCK * plock);
 void Func_Of_Proc_Pre_Spinlock(KSPIN_LOCK * plock);
 void Func_Of_Proc_Pre_Spinunlock(KSPIN_LOCK * plock);
 void Func_Of_Proc_Pre_Spinlock_Ex(KSPIN_LOCK * plock);
 void Func_Of_Proc_Pre_Spinunlock_Ex(KSPIN_LOCK * plock);

 void Func_Of_Proc_Pre_Init_Queue(_queue * pqueue);
 void Func_Of_Proc_Deinit_Queue(_queue * pqueue);
 u32 Func_Of_Proc_Pre_Queue_Empty(_queue * pqueue);
 u32 Func_Of_Proc_End_Of_Queue_Search(LIST_ENTRY * queue, LIST_ENTRY * pelement);

 u32 Func_Of_Proc_Get_Current_Time(void);
 u32 Func_Of_Proc_Systime_To_Ms(u32 systime);
 u32 Func_Of_Proc_Ms_To_Systime(u32 ms);
 s32 Func_Of_Proc_Get_Passing_Time_Ms(u32 start);
 s32 Func_Of_Proc_Get_Time_Interval_Ms(u32 start, u32 end);

 void Func_Of_Proc_Msleep_Os(int ms);
 void Func_Of_Proc_Usleep_Os(int us);

 u32 Func_Of_Proc_Atoi(u8 * s);


 void Func_Of_Proc_Mdelay_Os(int ms);
 void Func_Of_Proc_Udelay_Os(int us);
 void Func_Of_Proc_Yield_Os(void);

 void Func_Of_Proc_Init_Timer(WDFTIMER * ptimer, void *pwadptdata, void *pfunc);

__inline static unsigned char _cancel_timer_ex(WDFTIMER * ptimer)
{
//	return del_timer_sync(ptimer);
	return 1;
}

static __inline void thread_enter(char *name)
{

}

__inline static void flush_signals_thread(void)
{

}

__inline static void wl_dump_stack(void)
{

}



#define _RND(sz, r) ((((sz)+((r)-1))/(r))*(r))
#define RND4(x)	(((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)

__inline static u32 _RND4(u32 sz)
{

	u32 val;

	val = ((sz >> 2) + ((sz & 3) ? 1 : 0)) << 2;

	return val;

}

__inline static u32 _RND8(u32 sz)
{

	u32 val;

	val = ((sz >> 3) + ((sz & 7) ? 1 : 0)) << 3;

	return val;

}

__inline static u32 _RND128(u32 sz)
{

	u32 val;

	val = ((sz >> 7) + ((sz & 127) ? 1 : 0)) << 7;

	return val;

}

__inline static u32 _RND256(u32 sz)
{

	u32 val;

	val = ((sz >> 8) + ((sz & 255) ? 1 : 0)) << 8;

	return val;

}

__inline static u32 _RND512(u32 sz)
{

	u32 val;

	val = ((sz >> 9) + ((sz & 511) ? 1 : 0)) << 9;

	return val;

}

__inline static u32 bitshift(u32 bitmask)
{
	u32 i;

	for (i = 0; i <= 31; i++)
		if (((bitmask >> i) & 0x1) == 1)
			break;

	return i;
}

#define wl_min(a, b) ((a>b)?b:a)
#define wl_is_range_a_in_b(hi_a, lo_a, hi_b, lo_b) (((hi_a) <= (hi_b)) && ((lo_a) >= (lo_b)))
#define wl_is_range_overlap(hi_a, lo_a, hi_b, lo_b) (((hi_a) > (lo_b)) && ((lo_a) < (hi_b)))

#ifndef MAC_FMT
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
#ifndef MAC_ARG
#define MAC_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2],((u8*)(x))[3],((u8*)(x))[4],((u8*)(x))[5]
#endif

 void Func_Of_Proc_Suspend_Lock_Init(void);
 void Func_Of_Proc_Suspend_Lock_Uninit(void);
 void Func_Of_Proc_Lock_Suspend(void);
 void Func_Of_Proc_Unlock_Suspend(void);
 void Func_Of_Proc_Lock_Suspend_Timeout(u32 timeout_ms);
 void Func_Of_Proc_Lock_Ext_Suspend_Timeout(u32 timeout_ms);
 void Func_Of_Proc_Lock_Rx_Suspend_Timeout(u32 timeout_ms);
 void Func_Of_Proc_Lock_Traffic_Suspend_Timeout(u32 timeout_ms);
 void Func_Of_Proc_Lock_Resume_Scan_Timeout(u32 timeout_ms);
 void Func_Of_Proc_Resume_Lock_Suspend(void);
 void Func_Of_Proc_Resume_Unlock_Suspend(void);
#ifdef CONFIG_AP_WOWLAN
 void wl_softap_lock_suspend(void);
 void wl_softap_unlock_suspend(void);
#endif

#if 0
 void Func_Atomic_Of_Set(ATOMIC_T * v, int i);
 int Func_Atomic_Of_Read(ATOMIC_T * v);
 void Func_Atomic_Of_Add(ATOMIC_T * v, int i);
 void Func_Atomic_Of_Sub(ATOMIC_T * v, int i);
 void Func_Atomic_Of_Inc(ATOMIC_T * v);
 void Func_Atomic_Of_Dec(ATOMIC_T * v);
 int Func_Atomic_Of_Add_Return(ATOMIC_T * v, int i);
 int Func_Atomic_Of_Sub_Return(ATOMIC_T * v, int i);
 int Func_Atomic_Of_Inc_Return(ATOMIC_T * v);
 int Func_Atomic_Of_Dec_Return(ATOMIC_T * v);

 void Func_Of_Proc_Free_Netdev(struct net_device *netdev);
#endif

 u64 Func_Of_Proc_Modular64(u64 x, u64 y);
 u64 Func_Of_Proc_Division64(u64 x, u64 y);
 u32 Func_Of_Proc_Random32(void);

#define WL_GET_BE16(a) ((u16) (((a)[0] << 8) | (a)[1]))
#define WL_PUT_BE16(a, val)			\
	do {					\
		(a)[0] = ((u16) (val)) >> 8;	\
		(a)[1] = ((u16) (val)) & 0xff;	\
	} while (0)

#define WL_GET_LE16(a) ((u16) (((a)[1] << 8) | (a)[0]))
#define WL_PUT_LE16(a, val)			\
	do {					\
		(a)[1] = ((u16) (val)) >> 8;	\
		(a)[0] = ((u16) (val)) & 0xff;	\
	} while (0)

#define WL_GET_BE24(a) ((((u32) (a)[0]) << 16) | (((u32) (a)[1]) << 8) | \
			 ((u32) (a)[2]))
#define WL_PUT_BE24(a, val)					\
	do {							\
		(a)[0] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[2] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define WL_GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
			 (((u32) (a)[2]) << 8) | ((u32) (a)[3]))
#define WL_PUT_BE32(a, val)					\
	do {							\
		(a)[0] = (u8) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[2] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[3] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define WL_GET_LE32(a) ((((u32) (a)[3]) << 24) | (((u32) (a)[2]) << 16) | \
			 (((u32) (a)[1]) << 8) | ((u32) (a)[0]))
#define WL_PUT_LE32(a, val)					\
	do {							\
		(a)[3] = (u8) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[2] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[0] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define WL_GET_BE64(a) ((((u64) (a)[0]) << 56) | (((u64) (a)[1]) << 48) | \
			 (((u64) (a)[2]) << 40) | (((u64) (a)[3]) << 32) | \
			 (((u64) (a)[4]) << 24) | (((u64) (a)[5]) << 16) | \
			 (((u64) (a)[6]) << 8) | ((u64) (a)[7]))
#define WL_PUT_BE64(a, val)				\
	do {						\
		(a)[0] = (u8) (((u64) (val)) >> 56);	\
		(a)[1] = (u8) (((u64) (val)) >> 48);	\
		(a)[2] = (u8) (((u64) (val)) >> 40);	\
		(a)[3] = (u8) (((u64) (val)) >> 32);	\
		(a)[4] = (u8) (((u64) (val)) >> 24);	\
		(a)[5] = (u8) (((u64) (val)) >> 16);	\
		(a)[6] = (u8) (((u64) (val)) >> 8);	\
		(a)[7] = (u8) (((u64) (val)) & 0xff);	\
	} while (0)

#define WL_GET_LE64(a) ((((u64) (a)[7]) << 56) | (((u64) (a)[6]) << 48) | \
			 (((u64) (a)[5]) << 40) | (((u64) (a)[4]) << 32) | \
			 (((u64) (a)[3]) << 24) | (((u64) (a)[2]) << 16) | \
			 (((u64) (a)[1]) << 8) | ((u64) (a)[0]))

void Func_Of_Proc_Buf_Free(u8 ** buf, u32 * buf_len);
void Func_Of_Proc_Buf_Update(u8 ** buf, u32 * buf_len, u8 * src, u32 src_len);

struct wl_cbuf {
	u32 write;
	u32 read;
	u32 size;
	void *bufs[0];
};

bool Func_Of_Proc_Cbuf_Full(struct wl_cbuf *cbuf);
bool Func_Of_Proc_Cbuf_Empty(struct wl_cbuf *cbuf);
bool Func_Of_Proc_Cbuf_Push(struct wl_cbuf *cbuf, void *buf);
void *Func_Of_Proc_Cbuf_Pop(struct wl_cbuf *cbuf);
struct wl_cbuf *Func_Of_Proc_Cbuf_Alloc(u32 size);
void Func_Of_Proc_Cbuf_Free(struct wl_cbuf *cbuf);

BOOLEAN Func_Ishexdigit(char chTmp);
BOOLEAN Func_Is_Alpha(char chTmp);
char Func_Alpha_To_Upper(char c);

#endif
