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
#include "common.h"
#include "sha256.h"
#include "crypto.h"
#include "wpa.h"

int vector_hmac_sha256(const u8 * key, size_t key_len, size_t num_elem,
					   const u8 * addr[], const size_t * len, u8 * mac)
{
	unsigned char k_pad[64];
	unsigned char tk[32];
	const u8 *_addr[6];
	size_t _len[6], i;

	if (num_elem > 5) {

		return -1;
	}

	if (key_len > 64) {
		if (sha256_vect(1, &key, &key_len, tk) < 0)
			return -1;
		key = tk;
		key_len = 32;
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
	if (sha256_vect(1 + num_elem, _addr, _len, mac) < 0)
		return -1;

	os_memset(k_pad, 0, sizeof(k_pad));
	os_memcpy(k_pad, key, key_len);

	for (i = 0; i < 64; i++)
		k_pad[i] ^= 0x5c;

	_addr[0] = k_pad;
	_len[0] = 64;
	_addr[1] = mac;
	_len[1] = SHA256_LEN_MAC;
	return sha256_vect(2, _addr, _len, mac);
}

int hmac_sha256(const u8 * key, size_t key_len, const u8 * data,
				size_t data_len, u8 * mac)
{
	return vector_hmac_sha256(key, key_len, 1, &data, &data_len, mac);
}
