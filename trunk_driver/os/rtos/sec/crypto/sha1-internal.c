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


//#include "bsp.h"
//#include "type.h"
#include "wf_os_api.h"
#include "sec/wpa.h"
#include "sec/crypto/sha1.h"
//#include "md5.h"
//#include "crypto.h"

typedef struct sha1context wf_sha1_ctx;

void wf_sha1_transform(wf_u32 state[5], const unsigned char buffer[64]);

#define CONFIG_CRYPTO_INTERNAL

#ifdef CONFIG_CRYPTO_INTERNAL

int wf_sha1_vector(size_t num_elem, const wf_u8 * addr[], const size_t * len,
                wf_u8 * mac)
{
  wf_sha1_ctx ctx;
  size_t i;

  wf_sha1_init(&ctx);
  for (i = 0; i < num_elem; i++)
    wf_sha1_update(&ctx, addr[i], len[i]);
  wf_sha1_final(mac, &ctx);
  return 0;
}
#endif

#define SHA1HANDSOFF

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#ifndef WORDS_BIGENDIAN
#define blk0(i) (block->l[i] = (rol(block->l[i], 24) & 0xFF00FF00) | \
(rol(block->l[i], 8) & 0x00FF00FF))
#else
#define blk0(i) block->l[i]
#endif
#define blk(i) (block->l[i & 15] = rol(block->l[(i + 13) & 15] ^ \
block->l[(i + 8) & 15] ^ block->l[(i + 2) & 15] ^ block->l[i & 15], 1))

#define R0(v,w,x,y,z,i) \
z += ((w & (x ^ y)) ^ y) + blk0(i) + 0x5A827999 + rol(v, 5); \
  w = rol(w, 30);
#define R1(v,w,x,y,z,i) \
  z += ((w & (x ^ y)) ^ y) + blk(i) + 0x5A827999 + rol(v, 5); \
    w = rol(w, 30);
#define R2(v,w,x,y,z,i) \
    z += (w ^ x ^ y) + blk(i) + 0x6ED9EBA1 + rol(v, 5); w = rol(w, 30);
#define R3(v,w,x,y,z,i) \
    z += (((w | x) & y) | (w & x)) + blk(i) + 0x8F1BBCDC + rol(v, 5); \
      w = rol(w, 30);
#define R4(v,w,x,y,z,i) \
      z += (w ^ x ^ y) + blk(i) + 0xCA62C1D6 + rol(v, 5); \
	w=rol(w, 30);

void wf_sha1_transform(wf_u32 state[5], const unsigned char buffer[64])
{
  wf_u32 a, b, c, d, e;
  typedef union {
    unsigned char c[64];
    wf_u32 l[16];
  } CHAR64LONG16;
  CHAR64LONG16 *block;
#ifdef SHA1HANDSOFF
  CHAR64LONG16 workspace;
  block = &workspace;
  wf_memcpy(block, buffer, 64);
#else
  block = (CHAR64LONG16 *) buffer;
#endif

  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];

  R0(a, b, c, d, e, 0);
  R0(e, a, b, c, d, 1);
  R0(d, e, a, b, c, 2);
  R0(c, d, e, a, b, 3);
  R0(b, c, d, e, a, 4);
  R0(a, b, c, d, e, 5);
  R0(e, a, b, c, d, 6);
  R0(d, e, a, b, c, 7);
  R0(c, d, e, a, b, 8);
  R0(b, c, d, e, a, 9);
  R0(a, b, c, d, e, 10);
  R0(e, a, b, c, d, 11);
  R0(d, e, a, b, c, 12);
  R0(c, d, e, a, b, 13);
  R0(b, c, d, e, a, 14);
  R0(a, b, c, d, e, 15);
  R1(e, a, b, c, d, 16);
  R1(d, e, a, b, c, 17);
  R1(c, d, e, a, b, 18);
  R1(b, c, d, e, a, 19);
  R2(a, b, c, d, e, 20);
  R2(e, a, b, c, d, 21);
  R2(d, e, a, b, c, 22);
  R2(c, d, e, a, b, 23);
  R2(b, c, d, e, a, 24);
  R2(a, b, c, d, e, 25);
  R2(e, a, b, c, d, 26);
  R2(d, e, a, b, c, 27);
  R2(c, d, e, a, b, 28);
  R2(b, c, d, e, a, 29);
  R2(a, b, c, d, e, 30);
  R2(e, a, b, c, d, 31);
  R2(d, e, a, b, c, 32);
  R2(c, d, e, a, b, 33);
  R2(b, c, d, e, a, 34);
  R2(a, b, c, d, e, 35);
  R2(e, a, b, c, d, 36);
  R2(d, e, a, b, c, 37);
  R2(c, d, e, a, b, 38);
  R2(b, c, d, e, a, 39);
  R3(a, b, c, d, e, 40);
  R3(e, a, b, c, d, 41);
  R3(d, e, a, b, c, 42);
  R3(c, d, e, a, b, 43);
  R3(b, c, d, e, a, 44);
  R3(a, b, c, d, e, 45);
  R3(e, a, b, c, d, 46);
  R3(d, e, a, b, c, 47);
  R3(c, d, e, a, b, 48);
  R3(b, c, d, e, a, 49);
  R3(a, b, c, d, e, 50);
  R3(e, a, b, c, d, 51);
  R3(d, e, a, b, c, 52);
  R3(c, d, e, a, b, 53);
  R3(b, c, d, e, a, 54);
  R3(a, b, c, d, e, 55);
  R3(e, a, b, c, d, 56);
  R3(d, e, a, b, c, 57);
  R3(c, d, e, a, b, 58);
  R3(b, c, d, e, a, 59);
  R4(a, b, c, d, e, 60);
  R4(e, a, b, c, d, 61);
  R4(d, e, a, b, c, 62);
  R4(c, d, e, a, b, 63);
  R4(b, c, d, e, a, 64);
  R4(a, b, c, d, e, 65);
  R4(e, a, b, c, d, 66);
  R4(d, e, a, b, c, 67);
  R4(c, d, e, a, b, 68);
  R4(b, c, d, e, a, 69);
  R4(a, b, c, d, e, 70);
  R4(e, a, b, c, d, 71);
  R4(d, e, a, b, c, 72);
  R4(c, d, e, a, b, 73);
  R4(b, c, d, e, a, 74);
  R4(a, b, c, d, e, 75);
  R4(e, a, b, c, d, 76);
  R4(d, e, a, b, c, 77);
  R4(c, d, e, a, b, 78);
  R4(b, c, d, e, a, 79);

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;

  a = b = c = d = e = 0;
#ifdef SHA1HANDSOFF
  wf_memset(block, 0, 64);
#endif
}

void wf_sha1_init(wf_sha1_ctx * context)
{

  context->state[0] = 0x67452301;
  context->state[1] = 0xEFCDAB89;
  context->state[2] = 0x98BADCFE;
  context->state[3] = 0x10325476;
  context->state[4] = 0xC3D2E1F0;
  context->count[0] = context->count[1] = 0;
}

void wf_sha1_update(wf_sha1_ctx * context, const void *_data, wf_u32 len)
{
  wf_u32 i, j;
  const unsigned char *data = _data;


  j = (context->count[0] >> 3) & 63;
  if ((context->count[0] += len << 3) < (len << 3))
    context->count[1]++;
  context->count[1] += (len >> 29);
  if ((j + len) > 63) {
    wf_memcpy(&context->buffer[j], data, (i = 64 - j));
    wf_sha1_transform(context->state, context->buffer);
    for (; i + 63 < len; i += 64) {
      wf_sha1_transform(context->state, &data[i]);
    }
    j = 0;
  } else
    i = 0;
  wf_memcpy(&context->buffer[j], &data[i], len - i);

}

void wf_sha1_final(unsigned char digest[20], wf_sha1_ctx * context)
{
  wf_u32 i;
  unsigned char finalcount[8];

  for (i = 0; i < 8; i++) {
    finalcount[i] = (unsigned char)
      ((context->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
  }
  wf_sha1_update(context, (unsigned char *)"\200", 1);
  while ((context->count[0] & 504) != 448) {
    wf_sha1_update(context, (unsigned char *)"\0", 1);
  }
  wf_sha1_update(context, finalcount, 8);
  for (i = 0; i < 20; i++) {
    digest[i] = (unsigned char)
      ((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
  }

  i = 0;
  wf_memset(context->buffer, 0, 64);
  wf_memset(context->state, 0, 20);
  wf_memset(context->count, 0, 8);
  wf_memset(finalcount, 0, 8);
}
