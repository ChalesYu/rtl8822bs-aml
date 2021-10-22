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
#include "common.h"
#include "sha1.h"
#include "wpa.h"
#include "random.h"

#define POOL_WORDS 32
#define POOL_WORDS_MASK (POOL_WORDS - 1)
#define POOL_TAP1 26
#define POOL_TAP2 20
#define POOL_TAP3 14
#define POOL_TAP4 7
#define POOL_TAP5 1
#define EXTRACT_LEN 16
#define MIN_READY_MARK 2

static wf_u32 pool[POOL_WORDS];
static unsigned int input_rotate = 0;
static unsigned int pool_pos = 0;
static wf_u8 dummy_key[20];
static unsigned int own_pool_ready = 0;
#define RANDOM_ENTROPY_SIZE 20
static char *random_entropy_file = NULL;
static int random_entropy_file_read = 0;

#define MIN_COLLECT_ENTROPY 1000
static unsigned int entropy = 0;
static unsigned int total_collected = 0;


static wf_u32 wf_rol32(wf_u32 x, wf_u32 y)
{
  return (x << (y & 31)) | (x >> (32 - (y & 31)));
}

static void wf_random_mix_pool(const void *buf, size_t len)
{
  static const wf_u32 twist[8] = {
    0x00000000, 0x3b6e20c8, 0x76dc4190, 0x4db26158,
    0xedb88320, 0xd6d6a3e8, 0x9b64c2b0, 0xa00ae278
  };
  const wf_u8 *pos = buf;
  wf_u32 w;
  
  
  while (len--) {
    w = wf_rol32(*pos++, input_rotate & 31);
    input_rotate += pool_pos ? 7 : 14;
    pool_pos = (pool_pos - 1) & POOL_WORDS_MASK;
    w ^= pool[pool_pos];
    w ^= pool[(pool_pos + POOL_TAP1) & POOL_WORDS_MASK];
    w ^= pool[(pool_pos + POOL_TAP2) & POOL_WORDS_MASK];
    w ^= pool[(pool_pos + POOL_TAP3) & POOL_WORDS_MASK];
    w ^= pool[(pool_pos + POOL_TAP4) & POOL_WORDS_MASK];
    w ^= pool[(pool_pos + POOL_TAP5) & POOL_WORDS_MASK];
    pool[pool_pos] = (w >> 3) ^ twist[w & 7];
  }
}

static void wf_random_extract(wf_u8 * out)
{
  unsigned int i;
  wf_u8 hash[SHA1_MAC_LEN];
  wf_u32 *hash_ptr;
  wf_u32 buf[POOL_WORDS / 2];
  
  wf_hmac_sha1(dummy_key, sizeof(dummy_key), (const wf_u8 *)pool,
               sizeof(pool), hash);
  wf_random_mix_pool(hash, sizeof(hash));
  
  for (i = 0; i < POOL_WORDS / 2; i++)
    buf[i] = pool[(pool_pos - i) & POOL_WORDS_MASK];
  wf_hmac_sha1(dummy_key, sizeof(dummy_key), (const wf_u8 *)buf, sizeof(buf), hash);
  
  hash_ptr = (wf_u32 *) hash;
  hash_ptr[0] ^= hash_ptr[4];
  os_memcpy(out, hash, EXTRACT_LEN);
}

int wf_random_get_bytes(void *buf, size_t len)
{
  int ret;
  wf_u8 *bytes = buf;
  size_t left;
  
  ret = wf_os_get_random(buf, len);
  
  left = len;
  while (left) {
    size_t siz, i;
    wf_u8 tmp[EXTRACT_LEN];
    wf_random_extract(tmp);
    
    siz = left > EXTRACT_LEN ? EXTRACT_LEN : left;
    for (i = 0; i < siz; i++)
      *bytes++ ^= tmp[i];
    left -= siz;
  }
  
  
  
  if (entropy < len)
    entropy = 0;
  else
    entropy -= len;
  
  return ret;
}


