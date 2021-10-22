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
#include "crypto.h"
#include "wpa.h"
#include "random.h"

int wf_hmac_sha1_vector(const wf_u8 * key, size_t key_len, size_t num_elem,
                        const wf_u8 * addr[], const size_t * len, wf_u8 * mac)
{
  unsigned char k_pad[64];
  unsigned char tk[20];
  const wf_u8 *_addr[6];
  size_t _len[6], i;
  int ret;
  
  if (num_elem > 5) {
    
    return -1;
  }
  
  if (key_len > 64) {
    if (wf_sha1_vector(1, &key, &key_len, tk))
      return -1;
    key = tk;
    key_len = 20;
  }
  
  os_memset(k_pad, 0, sizeof(k_pad));
  os_memcpy(k_pad, key, key_len);
  
  for (i = 0; i < 64; i++)
    k_pad[i] ^= 0x36;
  
  _addr[0] = k_pad;
  _len[0] = 64;
  for (i = 0; i < num_elem; i++) {
    _addr[i + 1] = addr[i];
    _len[i + 1] = len[i];
  }
  if (wf_sha1_vector(1 + num_elem, _addr, _len, mac))
    return -1;
  
  os_memset(k_pad, 0, sizeof(k_pad));
  os_memcpy(k_pad, key, key_len);
  
  for (i = 0; i < 64; i++)
    k_pad[i] ^= 0x5c;
  
  _addr[0] = k_pad;
  _len[0] = 64;
  _addr[1] = mac;
  _len[1] = SHA1_MAC_LEN;
  ret = wf_sha1_vector(2, _addr, _len, mac);
  os_memset(k_pad, 0, sizeof(k_pad));
  return ret;
}

int wf_hmac_sha1(const wf_u8 * key, size_t key_len, const wf_u8 * data, size_t data_len,
                 wf_u8 * mac)
{
  return wf_hmac_sha1_vector(key, key_len, 1, &data, &data_len, mac);
}
