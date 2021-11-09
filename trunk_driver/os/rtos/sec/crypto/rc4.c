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
//#include "crypto.h"
#include "sec/wpa.h"

#define S_SWAP(a,b) do { wf_u8 t = S[a]; S[a] = S[b]; S[b] = t; } while(0)

int wf_rc4_skip(const wf_u8 * key, size_t keylen, size_t skip,
             wf_u8 * data, size_t data_len)
{
  wf_u32 i, j, k;
  wf_u8 S[256], *pos;
  size_t kpos;

  for (i = 0; i < 256; i++)
    S[i] = i;
  j = 0;
  kpos = 0;
  for (i = 0; i < 256; i++) {
    j = (j + S[i] + key[kpos]) & 0xff;
    kpos++;
    if (kpos >= keylen)
      kpos = 0;
    S_SWAP(i, j);
  }

  i = j = 0;
  for (k = 0; k < skip; k++) {
    i = (i + 1) & 0xff;
    j = (j + S[i]) & 0xff;
    S_SWAP(i, j);
  }

  pos = data;
  for (k = 0; k < data_len; k++) {
    i = (i + 1) & 0xff;
    j = (j + S[i]) & 0xff;
    S_SWAP(i, j);
    *pos++ ^= S[(S[i] + S[j]) & 0xff];
  }

  return 0;
}
