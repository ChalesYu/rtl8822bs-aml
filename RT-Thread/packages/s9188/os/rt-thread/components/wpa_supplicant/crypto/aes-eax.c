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

int aes_128_eax_encrypt(const u8 * key, const u8 * nonce, size_t nonce_len,
						const u8 * hdr, size_t hdr_len,
						u8 * data, size_t data_len, u8 * tag)
{
	u8 *buf;
	size_t buf_len;
	u8 nonce_mac[AES_BLK_SZ], hdr_mac[AES_BLK_SZ], data_mac[AES_BLK_SZ];
	int i, ret = -1;

	if (nonce_len > data_len)
		buf_len = nonce_len;
	else
		buf_len = data_len;
	if (hdr_len > buf_len)
		buf_len = hdr_len;
	buf_len += 16;

	buf = os_malloc(buf_len);
	if (buf == NULL)
		return -1;

	os_memset(buf, 0, 15);

	buf[15] = 0;
	os_memcpy(buf + 16, nonce, nonce_len);
	if (omac1_aes_tilk_128(key, buf, 16 + nonce_len, nonce_mac))
		goto fail;

	buf[15] = 1;
	os_memcpy(buf + 16, hdr, hdr_len);
	if (omac1_aes_tilk_128(key, buf, 16 + hdr_len, hdr_mac))
		goto fail;

	if (aes_128_ctr_encrypt(key, nonce_mac, data, data_len))
		goto fail;
	buf[15] = 2;
	os_memcpy(buf + 16, data, data_len);
	if (omac1_aes_tilk_128(key, buf, 16 + data_len, data_mac))
		goto fail;

	for (i = 0; i < AES_BLK_SZ; i++)
		tag[i] = nonce_mac[i] ^ data_mac[i] ^ hdr_mac[i];

	ret = 0;
fail:
	bin_clear_free(buf, buf_len);

	return ret;
}

int aes_128_eax_decrypt(const u8 * key, const u8 * nonce, size_t nonce_len,
						const u8 * hdr, size_t hdr_len,
						u8 * data, size_t data_len, const u8 * tag)
{
	u8 *buf;
	size_t buf_len;
	u8 nonce_mac[AES_BLK_SZ], hdr_mac[AES_BLK_SZ], data_mac[AES_BLK_SZ];
	int i;

	if (nonce_len > data_len)
		buf_len = nonce_len;
	else
		buf_len = data_len;
	if (hdr_len > buf_len)
		buf_len = hdr_len;
	buf_len += 16;

	buf = os_malloc(buf_len);
	if (buf == NULL)
		return -1;

	os_memset(buf, 0, 15);

	buf[15] = 0;
	os_memcpy(buf + 16, nonce, nonce_len);
	if (omac1_aes_tilk_128(key, buf, 16 + nonce_len, nonce_mac)) {
		os_free(buf);
		return -1;
	}

	buf[15] = 1;
	os_memcpy(buf + 16, hdr, hdr_len);
	if (omac1_aes_tilk_128(key, buf, 16 + hdr_len, hdr_mac)) {
		os_free(buf);
		return -1;
	}

	buf[15] = 2;
	os_memcpy(buf + 16, data, data_len);
	if (omac1_aes_tilk_128(key, buf, 16 + data_len, data_mac)) {
		os_free(buf);
		return -1;
	}

	os_free(buf);

	for (i = 0; i < AES_BLK_SZ; i++) {
		if (tag[i] != (nonce_mac[i] ^ data_mac[i] ^ hdr_mac[i]))
			return -2;
	}

	return aes_128_ctr_encrypt(key, nonce_mac, data, data_len);
}
