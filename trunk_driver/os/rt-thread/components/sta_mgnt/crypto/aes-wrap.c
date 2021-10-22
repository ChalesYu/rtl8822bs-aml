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
#include "utils/common.h"
#include "wpa.h"
#include "aes.h"
#include "crypto.h"

int wf_aes_wrap(const wf_u8 * kek, size_t kek_len, int n, const wf_u8 * plain,
                wf_u8 * cipher)
{
  wf_u8 *a, *r, b[AES_BLK_SZ];
  int i, j;
  void *ctx;
  unsigned int t;
  
  a = cipher;
  r = cipher + 8;
  
  os_memset(a, 0xa6, 8);
  os_memcpy(r, plain, 8 * n);
  
  ctx = wf_aes_init_enc(kek, kek_len);
  if (ctx == NULL)
    return -1;
  
  for (j = 0; j <= 5; j++) {
    r = cipher + 8;
    for (i = 1; i <= n; i++) {
      os_memcpy(b, a, 8);
      os_memcpy(b + 8, r, 8);
      wf_aes_enc(ctx, b, b);
      os_memcpy(a, b, 8);
      t = n * j + i;
      a[7] ^= t;
      a[6] ^= t >> 8;
      a[5] ^= t >> 16;
      a[4] ^= t >> 24;
      os_memcpy(r, b + 8, 8);
      r += 8;
    }
  }
  wf_aes_enc_deinit(ctx);
  
  return 0;
}
