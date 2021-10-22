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
#include "sec/utils/common.h"
#include "sec/crypto/aes.h"
//#include "wf_aes_wrap.h"
#include "sec/wpa.h"
#include "sec/crypto/crypto.h"
int wf_aes_unwrap(const wf_u8 * kek, size_t kek_len, int n, const wf_u8 * cipher,
               wf_u8 * plain)
{
  wf_u8 a[8], *r, b[AES_BLK_SZ];
  int i, j;
  void *ctx;
  unsigned int t;

  os_memcpy(a, cipher, 8);
  r = plain;
  os_memcpy(r, cipher + 8, 8 * n);

  ctx = wf_aes_dec_init(kek, kek_len);
  if (ctx == NULL)
    return -1;

  for (j = 5; j >= 0; j--) {
    r = plain + (n - 1) * 8;
    for (i = n; i >= 1; i--) {
      os_memcpy(b, a, 8);
      t = n * j + i;
      b[7] ^= t;
      b[6] ^= t >> 8;
      b[5] ^= t >> 16;
      b[4] ^= t >> 24;

      os_memcpy(b + 8, r, 8);
      wf_aes_dec(ctx, b, b);
      os_memcpy(a, b, 8);
      os_memcpy(r, b + 8, 8);
      r -= 8;
    }
  }
  wf_aes_dec_deinit(ctx);

  for (i = 0; i < 8; i++) {
    if (a[i] != 0xa6)
      return -1;
  }

  return 0;
}
