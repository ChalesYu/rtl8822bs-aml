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

void tls_prf_sha256(const u8 * secret, size_t secret_len, const char *label,
					const u8 * seed, size_t seed_len, u8 * out, size_t outlen)
{
	size_t clen;
	u8 A[SHA256_LEN_MAC];
	u8 P[SHA256_LEN_MAC];
	size_t pos;
	const unsigned char *addr[3];
	size_t len[3];

	addr[0] = A;
	len[0] = SHA256_LEN_MAC;
	addr[1] = (unsigned char *)label;
	len[1] = strlen(label);
	addr[2] = seed;
	len[2] = seed_len;

	vector_hmac_sha256(secret, secret_len, 2, &addr[1], &len[1], A);

	pos = 0;
	while (pos < outlen) {
		vector_hmac_sha256(secret, secret_len, 3, addr, len, P);
		hmac_sha256(secret, secret_len, A, SHA256_LEN_MAC, A);

		clen = outlen - pos;
		if (clen > SHA256_LEN_MAC)
			clen = SHA256_LEN_MAC;
		os_memcpy(out + pos, P, clen);
		pos += clen;
	}
}
