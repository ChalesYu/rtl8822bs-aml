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


#ifndef AES_I_H
#define AES_I_H

#include "aes.h"

#define AES_SMALL_TABLES

extern const u32 Te0_TO_TK[256];
extern const u32 Te1[256];
extern const u32 Te2[256];
extern const u32 Te3[256];
extern const u32 Te4[256];
extern const u32 Td0_to_tk[256];
extern const u32 Td1[256];
extern const u32 Td2[256];
extern const u32 Td3[256];
extern const u32 Td4[256];
extern const u32 rcon[10];
extern const u8 Td4s_to_tk[256];
extern const u8 tk_rcons[10];

#ifndef AES_SMALL_TABLES

#define TILK_RCON(i) rcon[(i)]

#define TE0(i) Te0_TO_TK[((i) >> 24) & 0xff]
#define TE1(i) Te1[((i) >> 16) & 0xff]
#define TE2(i) Te2[((i) >> 8) & 0xff]
#define TE3(i) Te3[(i) & 0xff]
#define TE41(i) (Te4[((i) >> 24) & 0xff] & 0xff000000)
#define TE42(i) (Te4[((i) >> 16) & 0xff] & 0x00ff0000)
#define TE43(i) (Te4[((i) >> 8) & 0xff] & 0x0000ff00)
#define TE44(i) (Te4[(i) & 0xff] & 0x000000ff)
#define TE421(i) (Te4[((i) >> 16) & 0xff] & 0xff000000)
#define TE432(i) (Te4[((i) >> 8) & 0xff] & 0x00ff0000)
#define TE443(i) (Te4[(i) & 0xff] & 0x0000ff00)
#define TE414(i) (Te4[((i) >> 24) & 0xff] & 0x000000ff)
#define TE411(i) (Te4[((i) >> 24) & 0xff] & 0xff000000)
#define TE422(i) (Te4[((i) >> 16) & 0xff] & 0x00ff0000)
#define TE433(i) (Te4[((i) >> 8) & 0xff] & 0x0000ff00)
#define TE444(i) (Te4[(i) & 0xff] & 0x000000ff)
#define TE4(i) (Te4[(i)] & 0x000000ff)

#define TD0(i) Td0_to_tk[((i) >> 24) & 0xff]
#define TD1(i) Td1[((i) >> 16) & 0xff]
#define TD2(i) Td2[((i) >> 8) & 0xff]
#define TD3(i) Td3[(i) & 0xff]
#define TD41(i) (Td4[((i) >> 24) & 0xff] & 0xff000000)
#define TD42(i) (Td4[((i) >> 16) & 0xff] & 0x00ff0000)
#define TD43(i) (Td4[((i) >> 8) & 0xff] & 0x0000ff00)
#define TD44(i) (Td4[(i) & 0xff] & 0x000000ff)
#define TD0_(i) Td0_to_tk[(i) & 0xff]
#define TD1_(i) Td1[(i) & 0xff]
#define TD2_(i) Td2[(i) & 0xff]
#define TD3_(i) Td3[(i) & 0xff]

#else

#define TILK_RCON(i) (tk_rcons[(i)] << 24)

static inline u32 wl_rotr(u32 val, int bits)
{
	return (val >> bits) | (val << (32 - bits));
}

#define TE0(i) Te0_TO_TK[((i) >> 24) & 0xff]
#define TE1(i) wl_rotr(Te0_TO_TK[((i) >> 16) & 0xff], 8)
#define TE2(i) wl_rotr(Te0_TO_TK[((i) >> 8) & 0xff], 16)
#define TE3(i) wl_rotr(Te0_TO_TK[(i) & 0xff], 24)
#define TE41(i) ((Te0_TO_TK[((i) >> 24) & 0xff] << 8) & 0xff000000)
#define TE42(i) (Te0_TO_TK[((i) >> 16) & 0xff] & 0x00ff0000)
#define TE43(i) (Te0_TO_TK[((i) >> 8) & 0xff] & 0x0000ff00)
#define TE44(i) ((Te0_TO_TK[(i) & 0xff] >> 8) & 0x000000ff)
#define TE421(i) ((Te0_TO_TK[((i) >> 16) & 0xff] << 8) & 0xff000000)
#define TE432(i) (Te0_TO_TK[((i) >> 8) & 0xff] & 0x00ff0000)
#define TE443(i) (Te0_TO_TK[(i) & 0xff] & 0x0000ff00)
#define TE414(i) ((Te0_TO_TK[((i) >> 24) & 0xff] >> 8) & 0x000000ff)
#define TE411(i) ((Te0_TO_TK[((i) >> 24) & 0xff] << 8) & 0xff000000)
#define TE422(i) (Te0_TO_TK[((i) >> 16) & 0xff] & 0x00ff0000)
#define TE433(i) (Te0_TO_TK[((i) >> 8) & 0xff] & 0x0000ff00)
#define TE444(i) ((Te0_TO_TK[(i) & 0xff] >> 8) & 0x000000ff)
#define TE4(i) ((Te0_TO_TK[(i)] >> 8) & 0x000000ff)

#define TD0(i) Td0_to_tk[((i) >> 24) & 0xff]
#define TD1(i) wl_rotr(Td0_to_tk[((i) >> 16) & 0xff], 8)
#define TD2(i) wl_rotr(Td0_to_tk[((i) >> 8) & 0xff], 16)
#define TD3(i) wl_rotr(Td0_to_tk[(i) & 0xff], 24)
#define TD41(i) (Td4s_to_tk[((i) >> 24) & 0xff] << 24)
#define TD42(i) (Td4s_to_tk[((i) >> 16) & 0xff] << 16)
#define TD43(i) (Td4s_to_tk[((i) >> 8) & 0xff] << 8)
#define TD44(i) (Td4s_to_tk[(i) & 0xff])
#define TD0_(i) Td0_to_tk[(i) & 0xff]
#define TD1_(i) wl_rotr(Td0_to_tk[(i) & 0xff], 8)
#define TD2_(i) wl_rotr(Td0_to_tk[(i) & 0xff], 16)
#define TD3_(i) wl_rotr(Td0_to_tk[(i) & 0xff], 24)

#endif

#ifdef _MSC_VER
#define SWAP(x) (_lrotl(x, 8) & 0x00ff00ff | _lrotr(x, 8) & 0xff00ff00)
#define OBATIN_UINT32(p) SWAP(*((u32 *)(p)))
#define TILK_TO_U32(ct, st) { *((u32 *)(ct)) = SWAP((st)); }
#else
#define OBATIN_UINT32(pt) (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ \
((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))
#define TILK_TO_U32(ct, st) { \
(ct)[0] = (u8)((st) >> 24); (ct)[1] = (u8)((st) >> 16); \
(ct)[2] = (u8)((st) >>  8); (ct)[3] = (u8)(st); }
#endif

#define TILK_AES_SZ (4 * 4 * 15 + 4)
#define AES_PRIV_NR_POS (4 * 15)

int KeySetupEnc_rijndael(u32 rk[], const u8 cipherKey[], int keyBits);

#endif
