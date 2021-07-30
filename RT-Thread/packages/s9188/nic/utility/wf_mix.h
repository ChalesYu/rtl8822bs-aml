/*
 * wf_mix.h
 *
 * used for general use macro
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
#ifndef __WF_MIX__
#define __WF_MIX__

/* include */
#include "wf_typedef.h"

/* macro */

#ifndef WF_ARRAY_SIZE
#define WF_ARRAY_SIZE(arr)  (sizeof(arr) / sizeof(arr[0]))
#endif

#define WF_PCHAR_2_BE16(val)    ((wf_u16)((val)[0] << 8 | (val)[1]))
#define WF_PCHAR_2_LE16(val)    ((wf_u16)((val)[1] << 8 | (val)[0]))

#ifndef WF_BIT
#define WF_BIT(x)       (1ul << (x))
#endif
#ifndef WF_BITS
#define WF_BITS(m, n)   (((WF_BIT(m) - 1) | WF_BIT(m)) & (~((WF_BIT(n) - 1) | WF_BIT(n))))
#endif

#ifndef WF_DIV_ROUND_UP
#define WF_DIV_ROUND_UP(n, d)       (((n) + ((d) - 1)) / (d))
#endif
#ifndef WF_DIV_ROUND_CLOSEST
#define WF_DIV_ROUND_CLOSEST(n, d)  (((n) + ((d) / 2)) / (d))
#endif

#ifndef WF_RND4
#define WF_RND4(x)          ((((x) >> 2) + (((x) & 3) ?  1 : 0)) << 2)
#endif
#ifndef WF_RND8
#define WF_RND8(x)          ((((x) >> 3) + (((x) & 7) ?  1 : 0)) << 3)
#endif
#ifndef WF_RND512
#define WF_RND512(x)        ((((x) >> 9) + (((x) & 511) ?  1 : 0)) << 9)
#endif
#define WF_RND_MAX(sz, r)   (((sz) + ((r) - 1)) / (r) * (r))

#ifndef WF_MIN
#define WF_MIN(x, y)    ((x) < (y) ? (x) : (y))
#endif
#ifndef WF_MAX
#define WF_MAX(x, y)    ((x) < (y) ? (y) : (x))
#endif

/* type */
/* function declaration */

#endif

