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
#include "sec/crypto/random.h"
//#include "crypto.h"

int wf_sha1_prf(const wf_u8 * key, size_t key_len, const char *label,
             const wf_u8 * data, size_t data_len, wf_u8 * buf, size_t buf_len)
{
  wf_u8 counter = 0;
  size_t pos, plen;
  wf_u8 hash[SHA1_MAC_LEN];
  size_t label_len = os_string_length(label) + 1;
  const unsigned char *addr[3];
  size_t len[3];

  addr[0] = (wf_u8 *) label;
  len[0] = label_len;
  addr[1] = data;
  len[1] = data_len;
  addr[2] = &counter;
  len[2] = 1;

  pos = 0;
  while (pos < buf_len) {
    plen = buf_len - pos;
    if (plen >= SHA1_MAC_LEN) {
      if (wf_hmac_sha1_vector(key, key_len, 3, addr, len, &buf[pos]))
        return -1;
      pos += SHA1_MAC_LEN;
    } else {
      if (wf_hmac_sha1_vector(key, key_len, 3, addr, len, hash))
        return -1;
      wf_memcpy(&buf[pos], hash, plen);
      break;
    }
    counter++;
  }
  wf_memset(hash, 0, sizeof(hash));

  return 0;
}
