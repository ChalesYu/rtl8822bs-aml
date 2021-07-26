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
#include "sha256.h"

int hmac_sha256_kdf(const u8 * secret, size_t secret_len,
					const char *label, const u8 * seed, size_t seed_len,
					u8 * out, size_t outlen)
{
	u8 T[SHA256_LEN_MAC];
	u8 iter = 1;
	const unsigned char *addr[4];
	size_t len[4];
	size_t pos, clen;

	addr[0] = T;
	len[0] = SHA256_LEN_MAC;
	addr[1] = (const unsigned char *)label;
	len[1] = strlen(label) + 1;
	addr[2] = seed;
	len[2] = seed_len;
	addr[3] = &iter;
	len[3] = 1;

	if (vector_hmac_sha256(secret, secret_len, 3, &addr[1], &len[1], T) < 0)
		return -1;

	pos = 0;
	for (;;) {
		clen = outlen - pos;
		if (clen > SHA256_LEN_MAC)
			clen = SHA256_LEN_MAC;
		os_memcpy(out + pos, T, clen);
		pos += clen;

		if (pos == outlen)
			break;

		if (iter == 255) {
			os_memset(out, 0, outlen);
			os_memset(T, 0, SHA256_LEN_MAC);
			return -1;
		}
		iter++;

		if (vector_hmac_sha256(secret, secret_len, 4, addr, len, T) < 0) {
			os_memset(out, 0, outlen);
			os_memset(T, 0, SHA256_LEN_MAC);
			return -1;
		}
	}

	os_memset(T, 0, SHA256_LEN_MAC);
	return 0;
}
