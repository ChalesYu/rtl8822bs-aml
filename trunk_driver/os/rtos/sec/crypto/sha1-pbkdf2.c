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

static int wf_pbkdf2_sha1_f(const char *passphrase, const wf_u8 * ssid,
                            size_t ssid_len, int iterations, unsigned int count,
                            wf_u8 * digest)
{
  unsigned char tmp[SHA1_MAC_LEN], tmp2[SHA1_MAC_LEN];
  int i, j;
  unsigned char count_buf[4];
  const wf_u8 *addr[2];
  size_t len[2];
  size_t passphrase_len = os_string_length(passphrase);

  addr[0] = ssid;
  len[0] = ssid_len;
  addr[1] = count_buf;
  len[1] = 4;

  count_buf[0] = (count >> 24) & 0xff;
  count_buf[1] = (count >> 16) & 0xff;
  count_buf[2] = (count >> 8) & 0xff;
  count_buf[3] = count & 0xff;
  if (wf_hmac_sha1_vector((wf_u8 *) passphrase, passphrase_len, 2, addr, len, tmp))
    return -1;
  wf_memcpy(digest, tmp, SHA1_MAC_LEN);

  for (i = 1; i < iterations; i++) {
    if (wf_hmac_sha1((wf_u8 *) passphrase, passphrase_len, tmp,
                     SHA1_MAC_LEN, tmp2))
      return -1;
    wf_memcpy(tmp, tmp2, SHA1_MAC_LEN);
    for (j = 0; j < SHA1_MAC_LEN; j++)
      digest[j] ^= tmp2[j];
  }

  return 0;
}
int wf_pbkdf2_sha1(const wf_u8 *passphrase, const wf_u8 * ssid, size_t ssid_len,
                   int iterations, wf_u8 * buf, size_t buflen)
{
  unsigned int count = 0;
  unsigned char *pos = buf;
  size_t left = buflen, plen;
  unsigned char digest[SHA1_MAC_LEN];

  while (left > 0) {
    count++;
    if (wf_pbkdf2_sha1_f((char const*)passphrase, ssid, ssid_len, iterations,
                         count, digest))
      return -1;
    plen = left > SHA1_MAC_LEN ? SHA1_MAC_LEN : left;
    wf_memcpy(pos, digest, plen);
    pos += plen;
    left -= plen;
  }

  return 0;
}

