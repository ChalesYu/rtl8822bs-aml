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
#include "wpa.h"
#include "common.h"
#include "aes.h"
#include "aes_wrap.h"

int aes_128_cbc_encrypt(const u8 * key, const u8 * iv, u8 * data,
						size_t data_len)
{
	void *ctx;
	u8 cbc[AES_BLK_SZ];
	u8 *pos = data;
	int i, j, blocks;

	ctx = _init_aes_encrypt(key, 16);
	if (ctx == NULL)
		return -1;
	os_memcpy(cbc, iv, AES_BLK_SZ);

	blocks = data_len / AES_BLK_SZ;
	for (i = 0; i < blocks; i++) {
		for (j = 0; j < AES_BLK_SZ; j++)
			cbc[j] ^= pos[j];
		aes_encrypt(ctx, cbc, cbc);
		os_memcpy(pos, cbc, AES_BLK_SZ);
		pos += AES_BLK_SZ;
	}
	_encrypt_deinit_aes(ctx);
	return 0;
}

int aes_128_cbc_decrypt(const u8 * key, const u8 * iv, u8 * data,
						size_t data_len)
{
	void *ctx;
	u8 cbc[AES_BLK_SZ], tmp[AES_BLK_SZ];
	u8 *pos = data;
	int i, j, blocks;

	ctx = aes_decrypt_init(key, 16);
	if (ctx == NULL)
		return -1;
	os_memcpy(cbc, iv, AES_BLK_SZ);

	blocks = data_len / AES_BLK_SZ;
	for (i = 0; i < blocks; i++) {
		os_memcpy(tmp, pos, AES_BLK_SZ);
		aes_decrypt(ctx, pos, pos);
		for (j = 0; j < AES_BLK_SZ; j++)
			pos[j] ^= cbc[j];
		os_memcpy(cbc, tmp, AES_BLK_SZ);
		pos += AES_BLK_SZ;
	}
	aes_decrypt_deinit(ctx);
	return 0;
}
