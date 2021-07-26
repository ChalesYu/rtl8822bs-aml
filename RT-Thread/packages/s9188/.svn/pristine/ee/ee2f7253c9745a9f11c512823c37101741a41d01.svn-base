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

int aes_128_ctr_encrypt(const u8 * key, const u8 * nonce,
						u8 * data, size_t data_len)
{
	void *ctx;
	size_t j, len, left = data_len;
	int i;
	u8 *pos = data;
	u8 counter[AES_BLK_SZ], buf[AES_BLK_SZ];

	ctx = _init_aes_encrypt(key, 16);
	if (ctx == NULL)
		return -1;
	os_memcpy(counter, nonce, AES_BLK_SZ);

	while (left > 0) {
		aes_encrypt(ctx, counter, buf);

		len = (left < AES_BLK_SZ) ? left : AES_BLK_SZ;
		for (j = 0; j < len; j++)
			pos[j] ^= buf[j];
		pos += len;
		left -= len;

		for (i = AES_BLK_SZ - 1; i >= 0; i--) {
			counter[i]++;
			if (counter[i])
				break;
		}
	}
	_encrypt_deinit_aes(ctx);
	return 0;
}
