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
#include "aes.h"
#include "aes_wrap.h"

static void mulx_gf(u8 * pad)
{
	int i, carry;

	carry = pad[0] & 0x80;
	for (i = 0; i < AES_BLK_SZ - 1; i++)
		pad[i] = (pad[i] << 1) | (pad[i + 1] >> 7);
	pad[AES_BLK_SZ - 1] <<= 1;
	if (carry)
		pad[AES_BLK_SZ - 1] ^= 0x87;
}

int omac1_aes_vector(const u8 * key, size_t key_len, size_t num_elem,
					 const u8 * addr[], const size_t * len, u8 * mac)
{
	void *ctx;
	u8 cbc[AES_BLK_SZ], pad[AES_BLK_SZ];
	const u8 *pos, *end;
	size_t i, e, left, total_len;

	ctx = _init_aes_encrypt(key, key_len);
	if (ctx == NULL)
		return -1;
	os_memset(cbc, 0, AES_BLK_SZ);

	total_len = 0;
	for (e = 0; e < num_elem; e++)
		total_len += len[e];
	left = total_len;

	e = 0;
	pos = addr[0];
	end = pos + len[0];

	while (left >= AES_BLK_SZ) {
		for (i = 0; i < AES_BLK_SZ; i++) {
			cbc[i] ^= *pos++;
			if (pos >= end) {
				if (i + 1 == AES_BLK_SZ && left == AES_BLK_SZ)
					break;
				e++;
				pos = addr[e];
				end = pos + len[e];
			}
		}
		if (left > AES_BLK_SZ)
			aes_encrypt(ctx, cbc, cbc);
		left -= AES_BLK_SZ;
	}

	os_memset(pad, 0, AES_BLK_SZ);
	aes_encrypt(ctx, pad, pad);
	mulx_gf(pad);

	if (left || total_len == 0) {
		for (i = 0; i < left; i++) {
			cbc[i] ^= *pos++;
			if (pos >= end) {
				if (i + 1 == left)
					break;
				e++;
				pos = addr[e];
				end = pos + len[e];
			}
		}
		cbc[left] ^= 0x80;
		mulx_gf(pad);
	}

	for (i = 0; i < AES_BLK_SZ; i++)
		pad[i] ^= cbc[i];
	aes_encrypt(ctx, pad, mac);
	_encrypt_deinit_aes(ctx);
	return 0;
}

int omac1_aes_tilk_128_vector(const u8 * key, size_t num_elem,
							  const u8 * addr[], const size_t * len, u8 * mac)
{
	return omac1_aes_vector(key, 16, num_elem, addr, len, mac);
}

int omac1_aes_tilk_128(const u8 * key, const u8 * data, size_t data_len,
					   u8 * mac)
{
	return omac1_aes_tilk_128_vector(key, 1, &data, &data_len, mac);
}

int omac1_aes_256(const u8 * key, const u8 * data, size_t data_len, u8 * mac)
{
	return omac1_aes_vector(key, 32, 1, &data, &data_len, mac);
}
