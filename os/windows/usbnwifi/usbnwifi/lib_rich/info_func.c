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
#include "globals.h"
#include "ndis.h"
#include "hw_pcomp.h"
#include "hw_main.h"
#include "hw_Nic.h"
#include "hw_func.h"
#include "usb_main.h"
#include "bus_intf.h"
#include "8187_gen.h"

#include "malloc.h"

u32 Func_Of_Proc_Atoi(u8 * s)
{

	int num = 0, flag = 0;
	int i;
	for (i = 0; i <= strlen(s); i++) {
		if (s[i] >= '0' && s[i] <= '9')
			num = num * 10 + s[i] - '0';
		else if (s[0] == '-' && i == 0)
			flag = 1;
		else
			break;
	}

	if (flag == 1)
		num = num * -1;

	return (num);

}

inline u8 *Func_Of_Proc_Pre_Vmalloc(u32 sz)
{
	u8 *pbuf;
	pbuf = Func_Of_Proc_Pre_Malloc(sz);

	return pbuf;
}

inline u8 *Func_Of_Proc_Pre_Zvmalloc(u32 sz)
{
	u8 *pbuf;

	pbuf = Func_Of_Proc_Pre_Malloc(sz);
	if (pbuf != NULL)
		memset(pbuf, 0, sz);

	return pbuf;
}

inline void Func_Of_Proc_Pre_Vmfree(u8 * pbuf, u32 sz)
{

	free(pbuf);

}

u8 *Func_Of_Proc_Pre_Malloc(u32 sz)
{

    u8 *pbuf = NULL;
    /////////////////////////////////////////////	
    //  pbuf = malloc(sz);
    //	pbuf = kmalloc(sz, in_interrupt()? GFP_ATOMIC : GFP_KERNEL);
	UNICODE_STRING  str;
	str.Buffer = (PWCHAR)ExAllocatePoolWithTag(NonPagedPool, sz, 'MY');
	str.Length = sz;
    pbuf = str.Buffer;
    return pbuf;
}

u8 *Func_Of_Proc_Pre_Zmalloc(u32 sz)
{
	u8 *pbuf = Func_Of_Proc_Pre_Malloc(sz);

	if (pbuf != NULL) {
		memset(pbuf, 0, sz);
	}

	return pbuf;
}

void Func_Of_Proc_Pre_Mfree(u8 * pbuf, u32 sz)
{
	//kfree(pbuf);
	UNICODE_STRING  str;
	str.Buffer = pbuf;
	ExFreePool(str.Buffer);
	str.Buffer = NULL;
	str.Length = str.MaximumLength  = 0;
	return;
}

void Func_Of_Proc_Pre_Memcpy(void *dst, const void *src, u32 sz)
{
	memcpy(dst, src, sz);
}

//inline void Func_Of_Proc_Pre_Memmove(void *dst, const void *src, u32 sz)
void Func_Of_Proc_Pre_Memmove(void *dst, const void *src, u32 sz)
{
	memmove(dst, src, sz);
}

int Func_Of_Proc_Pre_Memcmp(void *dst, const void *src, u32 sz)
{

	if (!(memcmp(dst, src, sz)))
		return TRUE;
	else
		return FALSE;
}

void Func_Of_Proc_Pre_Memset(void *pbuf, int c, u32 sz)
{
	memset(pbuf, c, sz);
}

void Func_Of_Proc_Init_Listhead(LIST_ENTRY * list)
{

//	INIT_LIST_HEAD(list);
}

u32 Func_Of_Proc_Is_List_Empty(LIST_ENTRY * phead)
{
	return TRUE;
}

void Func_Of_Proc_List_Insert_Head(LIST_ENTRY * plist, LIST_ENTRY * phead)
{
//	list_add(plist, phead);
}

void Func_Of_Proc_List_Insert_Tail(LIST_ENTRY * plist, LIST_ENTRY * phead)
{
//	list_add_tail(plist, phead);
}

void Func_Of_Proc_Init_Timer(WDFTIMER * ptimer, void *pwadptdata, void *pfunc)
{
//	_wadptdata *wadptdata = (_wadptdata *) pwadptdata;

//	_init_timer(ptimer, wadptdata->pnetdev, pfunc, wadptdata);
}

void Func_Of_Proc_Pre_Init_Sema(KSEMAPHORE * sema, int init_val)
{
//	sema_init(sema, init_val);
}

void Func_Of_Procw_Free_Sema(KSEMAPHORE * sema)
{

}

void Func_Of_Proc_Pre_Up_Sema(KSEMAPHORE * sema)
{
	//up(sema);
}

u32 Func_Of_Proc_Down_Sema(KSEMAPHORE * sema)
{
	//if (down_interruptible(sema))
	//	return FALSE;
	//else
		return TRUE;
}

void Func_Of_Proc_Pre_Mutex_Init(NDIS_MUTEX * pmutex)
{
    NDIS_INIT_MUTEX(pmutex);        // Initialized to set
}


void Func_Of_Proc_Pre_Mutex_Free(NDIS_MUTEX * pmutex)
{

}

void Func_Of_Proc_Pre_Spinlock_Init(KSPIN_LOCK * plock)
{

}

void Func_Of_Proc_Pre_Spinlock_Free(KSPIN_LOCK * plock)
{

}

void Func_Of_Proc_Pre_Spinlock(KSPIN_LOCK * plock)
{
	//spin_lock(plock);
	//Need to add
	//NdisAcquireSpinLock(SpinLock);

}

void Func_Of_Proc_Pre_Spinunlock(KSPIN_LOCK * plock)
{
	//spin_unlock(plock);
	//Need to add
	//NdisReleaseSpinLock(SpinLock);
}

void Func_Of_Proc_Pre_Spinlock_Ex(KSPIN_LOCK * plock)
{
	//spin_lock(plock);
}

void Func_Of_Proc_Pre_Spinunlock_Ex(KSPIN_LOCK * plock)
{
	//spin_unlock(plock);
}

void Func_Of_Proc_Pre_Init_Queue(_queue * pqueue)
{
	Func_Of_Proc_Init_Listhead(&(pqueue->queue));
	Func_Of_Proc_Pre_Spinlock_Init(&(pqueue->lock));
}

void Func_Of_Proc_Deinit_Queue(_queue * pqueue)
{
	Func_Of_Proc_Pre_Spinlock_Free(&(pqueue->lock));
}

u32 Func_Of_Proc_Pre_Queue_Empty(_queue * pqueue)
{
	return (Func_Of_Proc_Is_List_Empty(&(pqueue->queue)));
}

u32 Func_Of_Proc_End_Of_Queue_Search(LIST_ENTRY * head, LIST_ENTRY * plist)
{
	if (head == plist)
		return TRUE;
	else
		return FALSE;
}

 
u32 Func_Of_Proc_Get_Current_Time(void)
{
    //直接获得ms
	LARGE_INTEGER tick_count;
	ULONG myInc = KeQueryTimeIncrement();
	KeQueryTickCount(&tick_count);
	tick_count.QuadPart *= myInc;
	tick_count.QuadPart /= 10000;
	return tick_count.LowPart;
}

s32 Func_Of_Proc_Get_Passing_Time_Ms(u32 start)
{   
    u32 CurrentTime;
    CurrentTime=Func_Of_Proc_Get_Current_Time();
	return (CurrentTime - start);
}

s32 Func_Of_Proc_Get_Time_Interval_Ms(u32 start, u32 end)
{
	return (end - start);
}



void Func_Of_Proc_Msleep_Os(int ms)
{
	NdisMSleep((unsigned int)ms);
}


void Func_Of_Proc_Usleep_Os(int us)
{


}

void Func_Of_Proc_Mdelay_Os(int ms)
{
	WDF_REL_TIMEOUT_IN_MS((ULONGLONG)ms);
}

void Func_Of_Proc_Udelay_Os(int us)
{
	WDF_REL_TIMEOUT_IN_US((ULONGLONG)us);
}


void Func_Of_Proc_Yield_Os(void)
{
	//yield();
	//think about it!!!
}


inline void Func_Of_Proc_Suspend_Lock_Init(void)
{
}

inline void Func_Of_Proc_Suspend_Lock_Uninit(void)
{
}

inline void Func_Of_Proc_Lock_Suspend(void)
{

}

inline void Func_Of_Proc_Unlock_Suspend(void)
{

}

inline void Func_Of_Proc_Resume_Lock_Suspend(void)
{

}

inline void Func_Of_Proc_Resume_Unlock_Suspend(void)
{


}

inline void Func_Of_Proc_Lock_Suspend_Timeout(u32 timeout_ms)
{

}

inline void Func_Of_Proc_Lock_Ext_Suspend_Timeout(u32 timeout_ms)
{

}

inline void Func_Of_Proc_Lock_Rx_Suspend_Timeout(u32 timeout_ms)
{

}

inline void Func_Of_Proc_Lock_Traffic_Suspend_Timeout(u32 timeout_ms)
{

}

inline void Func_Of_Proc_Lock_Resume_Scan_Timeout(u32 timeout_ms)
{

}
#if 0
inline void Func_Atomic_Of_Set(ATOMIC_T * v, int i)
{
	atomic_set(v, i);
}

inline int Func_Atomic_Of_Read(ATOMIC_T * v)
{
	return atomic_read(v);
}

inline void Func_Atomic_Of_Add(ATOMIC_T * v, int i)
{
	atomic_add(i, v);
}

inline void Func_Atomic_Of_Sub(ATOMIC_T * v, int i)
{
	atomic_sub(i, v);
}

inline void Func_Atomic_Of_Inc(ATOMIC_T * v)
{
	atomic_inc(v);
}

inline void Func_Atomic_Of_Dec(ATOMIC_T * v)
{
	atomic_dec(v);
}

inline int Func_Atomic_Of_Add_Return(ATOMIC_T * v, int i)
{
	return atomic_add_return(i, v);
}

inline int Func_Atomic_Of_Sub_Return(ATOMIC_T * v, int i)
{
	return atomic_sub_return(i, v);
}

inline int Func_Atomic_Of_Inc_Return(ATOMIC_T * v)
{
	return atomic_inc_return(v);
}

inline int Func_Atomic_Of_Dec_Return(ATOMIC_T * v)
{
	return atomic_dec_return(v);
}
#endif



////////////////////////////////////////
//not supported in VC2015 :-(
//need a solution
#if 0
__int64 _div128(
   __int64 highDividend,
   __int64 lowDividend,
   __int64 divisor,
   __int64 *remainder
);
#endif


u64 Func_Of_Proc_Modular64(u64 x, u64 y)
{
 //   u64 remainder;
//	return _udiv64(x, y,&remainder);
    UNREFERENCED_PARAMETER(x);
    UNREFERENCED_PARAMETER(y);
    MpTrace(COMP_EVENTS, DBG_NORMAL, ("===>%s,not finished!\n", __func__));
    return 0;
}

u64 Func_Of_Proc_Division64(u64 x, u64 y)
{
 //   u64 remainder;
//	_udiv64(x, y,&remainder);
//	return remainder;
    UNREFERENCED_PARAMETER(x);
    UNREFERENCED_PARAMETER(y);
    MpTrace(COMP_EVENTS, DBG_NORMAL, ("===>%s,not finished!\n", __func__));
    return 0;
}

inline u32 Func_Of_Proc_Random32(void)
{
    MpTrace(COMP_EVENTS, DBG_NORMAL, ("===>%s,not finished!\n", __func__));
//	return random32();
	return 0;
}

void Func_Of_Proc_Buf_Free(u8 ** buf, u32 * buf_len)
{
	u32 ori_len;

	if (!buf || !buf_len)
		return;

	ori_len = *buf_len;

	if (*buf) {
		u32 tmp_buf_len = *buf_len;
		*buf_len = 0;
		wl_mfree(*buf, tmp_buf_len);
		*buf = NULL;
	}
}

void Func_Of_Proc_Buf_Update(u8 ** buf, u32 * buf_len, u8 * src, u32 src_len)
{
	u32 ori_len = 0, dup_len = 0;
	u8 *ori = NULL;
	u8 *dup = NULL;

	if (!buf || !buf_len)
		return;

	if (!src || !src_len)
		goto keep_ori;

	dup = wl_malloc(src_len);
	if (dup) {
		dup_len = src_len;
		Func_Of_Proc_Pre_Memcpy(dup, src, dup_len);
	}

keep_ori:
	ori = *buf;
	ori_len = *buf_len;

	*buf_len = 0;
	*buf = dup;
	*buf_len = dup_len;

	if (ori && ori_len > 0)
		wl_mfree(ori, ori_len);
}

inline bool Func_Of_Proc_Cbuf_Full(struct wl_cbuf *cbuf)
{
	return (cbuf->write == cbuf->read - 1) ? TRUE : FALSE;
}

inline bool Func_Of_Proc_Cbuf_Empty(struct wl_cbuf * cbuf)
{
	return (cbuf->write == cbuf->read) ? TRUE : FALSE;
}

bool Func_Of_Proc_Cbuf_Push(struct wl_cbuf * cbuf, void *buf)
{
	if (Func_Of_Proc_Cbuf_Full(cbuf))
		return FALSE;

	if (0)
		MpTrace(COMP_EVENTS, DBG_NORMAL, 
		     ("%s on %u\n", __func__, cbuf->write));	
	cbuf->bufs[cbuf->write] = buf;
	cbuf->write = (cbuf->write + 1) % cbuf->size;

	return TRUE;
}

void *Func_Of_Proc_Cbuf_Pop(struct wl_cbuf *cbuf)
{
	void *buf;
	if (Func_Of_Proc_Cbuf_Empty(cbuf))
		return NULL;

	if (0)
		MpTrace(COMP_EVENTS, DBG_NORMAL, 
		     ("%s on %u\n", __func__, cbuf->read));	
	buf = cbuf->bufs[cbuf->read];
	cbuf->read = (cbuf->read + 1) % cbuf->size;

	return buf;
}

struct wl_cbuf *Func_Of_Proc_Cbuf_Alloc(u32 size)
{
	struct wl_cbuf *cbuf;

	cbuf = (struct wl_cbuf *)wl_malloc(sizeof(*cbuf) + sizeof(void *) * size);

	if (cbuf) {
		cbuf->write = cbuf->read = 0;
		cbuf->size = size;
	}

	return cbuf;
}

void Func_Of_Proc_Cbuf_Free(struct wl_cbuf *cbuf)
{
	wl_mfree((u8 *) cbuf, sizeof(*cbuf) + sizeof(void *) * cbuf->size);
}

//inline BOOLEAN Func_Ishexdigit(char chTmp)
BOOLEAN Func_Ishexdigit(char chTmp)
{
	if ((chTmp >= '0' && chTmp <= '9') ||
		(chTmp >= 'a' && chTmp <= 'f') || (chTmp >= 'A' && chTmp <= 'F'))
		return TRUE;
	else
		return FALSE;
}

//inline BOOLEAN Func_Is_Alpha(char chTmp)
BOOLEAN Func_Is_Alpha(char chTmp)
{
	if ((chTmp >= 'a' && chTmp <= 'z') || (chTmp >= 'A' && chTmp <= 'Z'))
		return TRUE;
	else
		return FALSE;
}

//inline char Func_Alpha_To_Upper(char c)
char Func_Alpha_To_Upper(char c)
{
	if ((c >= 'a' && c <= 'z'))
		c = 'A' + (c - 'a');
	return c;
}
