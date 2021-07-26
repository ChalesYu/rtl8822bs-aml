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


#include "includes.h"

#include "common.h"
#include "sha1.h"
#include "crypto.h"

int sha1_t_prf(const u8 * key, size_t key_len, const char *label,
			   const u8 * seed, size_t seed_len, u8 * buf, size_t buf_len)
{
	unsigned char counter = 0;
	size_t pos, plen;
	u8 hash[SHA1_MAC_LEN];
	size_t label_len = os_string_length(label);
	u8 output_len[2];
	const unsigned char *addr[5];
	size_t len[5];

	addr[0] = hash;
	len[0] = 0;
	addr[1] = (unsigned char *)label;
	len[1] = label_len + 1;
	addr[2] = seed;
	len[2] = seed_len;
	addr[3] = output_len;
	len[3] = 2;
	addr[4] = &counter;
	len[4] = 1;

	output_len[0] = (buf_len >> 8) & 0xff;
	output_len[1] = buf_len & 0xff;
	pos = 0;
	while (pos < buf_len) {
		counter++;
		plen = buf_len - pos;
		if (hmac_sha1_vector(key, key_len, 5, addr, len, hash))
			return -1;
		if (plen >= SHA1_MAC_LEN) {
			os_memcpy(&buf[pos], hash, SHA1_MAC_LEN);
			pos += SHA1_MAC_LEN;
		} else {
			os_memcpy(&buf[pos], hash, plen);
			break;
		}
		len[0] = SHA1_MAC_LEN;
	}

	os_memset(hash, 0, SHA1_MAC_LEN);

	return 0;
}
