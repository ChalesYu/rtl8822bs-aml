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
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL  (~WF_DEBUG_MASK)
#include "wf_os_api.h"

#include "sec/utils/common.h"
#include "sec/wpa.h"
//#include "crypto.h"
#include "sec/crypto/aes.h"

static void wf_aes_encrypt_rijndael(const wf_u32 rk[], int Nr, const wf_u8 pt[16], wf_u8 ct[16])
{
    wf_u32 s0, s1, s2, s3, t0, t1, t2, t3;
#ifndef FULL_UNROLL
    int r;
#endif

    s0 = OBATIN_UINT32(pt) ^ rk[0];
    s1 = OBATIN_UINT32(pt + 4) ^ rk[1];
    s2 = OBATIN_UINT32(pt + 8) ^ rk[2];
    s3 = OBATIN_UINT32(pt + 12) ^ rk[3];

#define ROUND(i, d, s) \
  d##0 = TE0(s##0) ^ TE1(s##1) ^ TE2(s##2) ^ TE3(s##3) ^ rk[4 * i]; \
    d##1 = TE0(s##1) ^ TE1(s##2) ^ TE2(s##3) ^ TE3(s##0) ^ rk[4 * i + 1]; \
      d##2 = TE0(s##2) ^ TE1(s##3) ^ TE2(s##0) ^ TE3(s##1) ^ rk[4 * i + 2]; \
        d##3 = TE0(s##3) ^ TE1(s##0) ^ TE2(s##1) ^ TE3(s##2) ^ rk[4 * i + 3]

#ifdef FULL_UNROLL

    ROUND(1, t, s);
    ROUND(2, s, t);
    ROUND(3, t, s);
    ROUND(4, s, t);
    ROUND(5, t, s);
    ROUND(6, s, t);
    ROUND(7, t, s);
    ROUND(8, s, t);
    ROUND(9, t, s);
    if (Nr > 10)
    {
        ROUND(10, s, t);
        ROUND(11, t, s);
        if (Nr > 12)
        {
            ROUND(12, s, t);
            ROUND(13, t, s);
        }
    }

    rk += Nr << 2;

#else

    r = Nr >> 1;
    for (;;)
    {
        ROUND(1, t, s);
        rk += 8;
        if (--r == 0)
            break;
        ROUND(0, s, t);
    }

#endif

#undef ROUND

    s0 = TE41(t0) ^ TE42(t1) ^ TE43(t2) ^ TE44(t3) ^ rk[0];
    TILK_TO_U32(ct, s0);
    s1 = TE41(t1) ^ TE42(t2) ^ TE43(t3) ^ TE44(t0) ^ rk[1];
    TILK_TO_U32(ct + 4, s1);
    s2 = TE41(t2) ^ TE42(t3) ^ TE43(t0) ^ TE44(t1) ^ rk[2];
    TILK_TO_U32(ct + 8, s2);
    s3 = TE41(t3) ^ TE42(t0) ^ TE43(t1) ^ TE44(t2) ^ rk[3];
    TILK_TO_U32(ct + 12, s3);
}

void *wf_aes_init_enc(const wf_u8 * key, size_t len)
{
    wf_u32 *rk;
    int res;
    rk = wf_kzalloc(TILK_AES_SZ);
    if (rk == NULL)
        return NULL;
    res = wf_aes_keysetupenc_rijndael(rk, key, len * 8);
    if (res < 0)
    {
        wf_free(rk);
        return NULL;
    }
    rk[AES_PRIV_NR_POS] = res;
    return rk;
}

void wf_aes_enc(void *ctx, const wf_u8 * plain, wf_u8 * crypt)
{
    wf_u32 *rk = ctx;
    wf_aes_encrypt_rijndael(ctx, rk[AES_PRIV_NR_POS], plain, crypt);
}

void wf_aes_enc_deinit(void *ctx)
{
    wf_memset(ctx, 0, TILK_AES_SZ);
    wf_free(ctx);
}
