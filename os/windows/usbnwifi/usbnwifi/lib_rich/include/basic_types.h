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

#ifndef __BASIC_TYPES_H__
#define __BASIC_TYPES_H__

#define wl_warn_on(condition)    /*NOTHING*/

#define __attribute__(x)         /*NOTHING*/

#define _func_enter_             /*NOTHING*/
#define _func_exit_              /*NOTHING*/



typedef enum 
{ 
  _FALSE=0,
  _TRUE =1
}bool;


#define IN
#define OUT
#define VOID void

typedef signed int sint;

#ifndef	PVOID
typedef void *PVOID;
#endif


#define u64 unsigned long long

#define u32 unsigned int 
#define u16 unsigned short 
#define s32 int 
#define s16 short int 
#define u8  unsigned char 
#define s8  char

#define uint u32


typedef void (*proc_t) (void *);

//typedef __kernel_size_t SIZE_T;
//typedef __kernel_ssize_t SSIZE_T;
//#define FIELD_OFFSET(s,field)	((SSIZE_T)&((s*)(0))->field)
//define in ntdef.h
//#define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))
//#define UFIELD_OFFSET(type, field)    ((ULONG)(LONG_PTR)&(((type *)0)->field))

struct __queue
{
	LIST_ENTRY queue;
	KSPIN_LOCK lock;
};

typedef struct __queue _queue;


#define wl_min(a, b) ((a>b)?b:a)
#define wl_is_range_a_in_b(hi_a, lo_a, hi_b, lo_b) (((hi_a) <= (hi_b)) && ((lo_a) >= (lo_b)))
#define wl_is_range_overlap(hi_a, lo_a, hi_b, lo_b) (((hi_a) > (lo_b)) && ((lo_a) < (hi_b)))

#define MEM_ALIGNMENT_OFFSET	(sizeof (SIZE_T))
#define MEM_ALIGNMENT_PADDING	(sizeof(SIZE_T) - 1)

#define SIZE_PTR SIZE_T
#define SSIZE_PTR SSIZE_T

#define BIT_LEN_MASK_32(__BitLen) ((u32)(0xFFFFFFFF >> (32 - (__BitLen))))
#define BIT_LEN_MASK_16(__BitLen) ((u16)(0xFFFF >> (16 - (__BitLen))))
#define BIT_LEN_MASK_8(__BitLen) ((u8)(0xFF >> (8 - (__BitLen))))

#define BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen) ((u32)(BIT_LEN_MASK_32(__BitLen) << (__BitOffset)))
#define BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen) ((u16)(BIT_LEN_MASK_16(__BitLen) << (__BitOffset)))
#define BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen) ((u8)(BIT_LEN_MASK_8(__BitLen) << (__BitOffset)))

#define EF1Byte (u8)
#define EF2Byte le16_to_cpu
#define EF4Byte le32_to_cpu

#define ReadLE4Byte(_ptr)	le32_to_cpu(*((u32 *)(_ptr)))
#define ReadLE2Byte(_ptr)	le16_to_cpu(*((u16 *)(_ptr)))
#define ReadLE1Byte(_ptr)	(*((u8 *)(_ptr)))

#define ReadBEE4Byte(_ptr)	be32_to_cpu(*((u32 *)(_ptr)))
#define ReadBE2Byte(_ptr)	be16_to_cpu(*((u16 *)(_ptr)))
#define ReadBE1Byte(_ptr)	(*((u8 *)(_ptr)))

#define WriteLE4Byte(_ptr, _val)	(*((u32 *)(_ptr))) = cpu_to_le32(_val)
#define WriteLE2Byte(_ptr, _val)	(*((u16 *)(_ptr))) = cpu_to_le16(_val)
#define WriteLE1Byte(_ptr, _val)	(*((u8 *)(_ptr))) = ((u8)(_val))

#define WriteBE4Byte(_ptr, _val)	(*((u32 *)(_ptr))) = cpu_to_be32(_val)
#define WriteBE2Byte(_ptr, _val)	(*((u16 *)(_ptr))) = cpu_to_be16(_val)
#define WriteBE1Byte(_ptr, _val)	(*((u8 *)(_ptr))) = ((u8)(_val))

#define LE_P4BYTE_TO_HOST_4BYTE(__pStart) (le32_to_cpu(*((u32 *)(__pStart))))
#define LE_P2BYTE_TO_HOST_2BYTE(__pStart) (le16_to_cpu(*((u16 *)(__pStart))))
#define LE_P1BYTE_TO_HOST_1BYTE(__pStart) ((*((u8 *)(__pStart))))

#define BE_P4BYTE_TO_HOST_4BYTE(__pStart) (be32_to_cpu(*((u32 *)(__pStart))))
#define BE_P2BYTE_TO_HOST_2BYTE(__pStart) (be16_to_cpu(*((u16 *)(__pStart))))
#define BE_P1BYTE_TO_HOST_1BYTE(__pStart) ((*((u8 *)(__pStart))))

//move from drv_typedf.h
#define U32_MOVE_BITS(x, bit) ((u32)(((u32)x) << (bit)))
#define U32_GET_BITS(x, bit) ((u32)(((u32)x) & BIT(bit)) >> bit)

#define GET_UINT32( var, ByteNum ) \
    (u8)((u32)(((var) >>((ByteNum) * 8)) & 0x00FF))

#define MAKE_UINT32(Byte0, Byte1, Byte2, Byte3) \
    ((u32)((u32)((Byte0) & 0x00FF) \
        + ((u32)((Byte1) & 0x00FF) << 8) \
            + ((u32)((Byte2) & 0x00FF) << 16) \
                + ((u32)((Byte3) & 0x00FF) << 24)))

#define LE_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	((LE_P4BYTE_TO_HOST_4BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_32(__BitLen))

#define LE_BITS_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	((LE_P2BYTE_TO_HOST_2BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_16(__BitLen))

#define LE_BITS_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	((LE_P1BYTE_TO_HOST_1BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_8(__BitLen))

#define BE_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	((BE_P4BYTE_TO_HOST_4BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_32(__BitLen))

#define BE_BITS_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	((BE_P2BYTE_TO_HOST_2BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_16(__BitLen))

#define BE_BITS_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	((BE_P1BYTE_TO_HOST_1BYTE(__pStart) >> (__BitOffset)) & BIT_LEN_MASK_8(__BitLen))

#define LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	(LE_P4BYTE_TO_HOST_4BYTE(__pStart) & (~BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen)))

#define LE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	(LE_P2BYTE_TO_HOST_2BYTE(__pStart) & (~BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen)))

#define LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	(LE_P1BYTE_TO_HOST_1BYTE(__pStart) & ((u8)(~BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen))))

#define BE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	(BE_P4BYTE_TO_HOST_4BYTE(__pStart) & (~BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen)))

#define BE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	(BE_P2BYTE_TO_HOST_2BYTE(__pStart) & (~BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen)))

#define BE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	(BE_P1BYTE_TO_HOST_1BYTE(__pStart) & (~BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen)))

#define SET_BITS_TO_LE_4BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		if (__BitOffset == 0 && __BitLen == 32) \
			WriteLE4Byte(__pStart, __Value); \
		else { \
			WriteLE4Byte(__pStart, \
				LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
				| \
				((((u32)__Value) & BIT_LEN_MASK_32(__BitLen)) << (__BitOffset)) \
			); \
		} \
	} while (0)

#define SET_BITS_TO_LE_2BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		if (__BitOffset == 0 && __BitLen == 16) \
			WriteLE2Byte(__pStart, __Value); \
		else { \
			WriteLE2Byte(__pStart, \
				LE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
				| \
				((((u16)__Value) & BIT_LEN_MASK_16(__BitLen)) << (__BitOffset)) \
			); \
		} \
	} while (0)

#define SET_BITS_TO_LE_1BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		if (__BitOffset == 0 && __BitLen == 8) \
			WriteLE1Byte(__pStart, __Value); \
		else { \
			WriteLE1Byte(__pStart, \
				LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
				| \
				((((u8)__Value) & BIT_LEN_MASK_8(__BitLen)) << (__BitOffset)) \
			); \
		} \
	} while (0)

#define SET_BITS_TO_BE_4BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		if (__BitOffset == 0 && __BitLen == 32) \
			WriteBE4Byte(__pStart, __Value); \
		else { \
			WriteBE4Byte(__pStart, \
				BE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
				| \
				((((u32)__Value) & BIT_LEN_MASK_32(__BitLen)) << (__BitOffset)) \
			); \
		} \
	} while (0)

#define SET_BITS_TO_BE_2BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		if (__BitOffset == 0 && __BitLen == 16) \
			WriteBE2Byte(__pStart, __Value); \
		else { \
			WriteBE2Byte(__pStart, \
				BE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
				| \
				((((u16)__Value) & BIT_LEN_MASK_16(__BitLen)) << (__BitOffset)) \
			); \
		} \
	} while (0)

#define SET_BITS_TO_BE_1BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		if (__BitOffset == 0 && __BitLen == 8) \
			WriteBE1Byte(__pStart, __Value); \
		else { \
			WriteBE1Byte(__pStart, \
				BE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
				| \
				((((u8)__Value) & BIT_LEN_MASK_8(__BitLen)) << (__BitOffset)) \
			); \
		} \
	} while (0)

#define N_BYTE_ALIGMENT(__Value, __Aligment) ((__Aligment == 1) ? (__Value) : (((__Value + __Aligment - 1) / __Aligment) * __Aligment))

typedef unsigned char BOOLEAN, *PBOOLEAN;

#define TEST_FLAG(__Flag,__testFlag)		(((__Flag) & (__testFlag)) != 0)
#define SET_FLAG(__Flag, __setFlag)			((__Flag) |= __setFlag)
#define CLEAR_FLAG(__Flag, __clearFlag)		((__Flag) &= ~(__clearFlag))
#define CLEAR_FLAGS(__Flag)					((__Flag) = 0)
#define TEST_FLAGS(__Flag, __testFlags)		(((__Flag) & (__testFlags)) == (__testFlags))

#endif
