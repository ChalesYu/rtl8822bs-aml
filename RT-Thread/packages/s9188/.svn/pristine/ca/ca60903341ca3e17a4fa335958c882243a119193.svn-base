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
#include "aes_siv.h"

static const u8 zero[AES_BLK_SZ];

static void dbl(u8 * pad)
{
	int i, carry;

	carry = pad[0] & 0x80;
	for (i = 0; i < AES_BLK_SZ - 1; i++)
		pad[i] = (pad[i] << 1) | (pad[i + 1] >> 7);
	pad[AES_BLK_SZ - 1] <<= 1;
	if (carry)
		pad[AES_BLK_SZ - 1] ^= 0x87;
}

static void xor(u8 * a, const u8 * b)
{
	int i;

	for (i = 0; i < AES_BLK_SZ; i++)
		*a++ ^= *b++;
}

static void xorend(u8 * a, int alen, const u8 * b, int blen)
{
	int i;

	if (alen < blen)
		return;

	for (i = 0; i < blen; i++)
		a[alen - blen + i] ^= b[i];
}

static void pad_block(u8 * pad, const u8 * addr, size_t len)
{
	os_memset(pad, 0, AES_BLK_SZ);
	os_memcpy(pad, addr, len);

	if (len < AES_BLK_SZ)
		pad[len] = 0x80;
}

static int aes_s2v(const u8 * key, size_t num_elem, const u8 * addr[],
				   size_t * len, u8 * mac)
{
	u8 tmp[AES_BLK_SZ], tmp2[AES_BLK_SZ];
	u8 *buf = NULL;
	int ret;
	size_t i;

	if (!num_elem) {
		os_memcpy(tmp, zero, sizeof(zero));
		tmp[AES_BLK_SZ - 1] = 1;
		return omac1_aes_tilk_128(key, tmp, sizeof(tmp), mac);
	}

	ret = omac1_aes_tilk_128(key, zero, sizeof(zero), tmp);
	if (ret)
		return ret;

	for (i = 0; i < num_elem - 1; i++) {
		ret = omac1_aes_tilk_128(key, addr[i], len[i], tmp2);
		if (ret)
			return ret;

		dbl(tmp);
		xor(tmp, tmp2);
	}
	if (len[i] >= AES_BLK_SZ) {
		buf = os_malloc(len[i]);
		if (!buf)
			return -ENOMEM;

		os_memcpy(buf, addr[i], len[i]);
		xorend(buf, len[i], tmp, AES_BLK_SZ);
		ret = omac1_aes_tilk_128(key, buf, len[i], mac);
		bin_clear_free(buf, len[i]);
		return ret;
	}

	dbl(tmp);
	pad_block(tmp2, addr[i], len[i]);
	xor(tmp, tmp2);

	return omac1_aes_tilk_128(key, tmp, sizeof(tmp), mac);
}

int aes_siv_encrypt(const u8 * key, const u8 * pw,
					size_t pwlen, size_t num_elem,
					const u8 * addr[], const size_t * len, u8 * out)
{
	const u8 *_addr[6];
	size_t _len[6];
	const u8 *k1 = key, *k2 = key + 16;
	u8 v[AES_BLK_SZ];
	size_t i;
	u8 *iv, *crypt_pw;

	if (num_elem > ARRAY_SIZE(_addr) - 1)
		return -1;

	for (i = 0; i < num_elem; i++) {
		_addr[i] = addr[i];
		_len[i] = len[i];
	}
	_addr[num_elem] = pw;
	_len[num_elem] = pwlen;

	if (aes_s2v(k1, num_elem + 1, _addr, _len, v))
		return -1;

	iv = out;
	crypt_pw = out + AES_BLK_SZ;

	os_memcpy(iv, v, AES_BLK_SZ);
	os_memcpy(crypt_pw, pw, pwlen);

	v[8] &= 0x7f;
	v[12] &= 0x7f;
	return aes_128_ctr_encrypt(k2, v, crypt_pw, pwlen);
}

int aes_siv_decrypt(const u8 * key, const u8 * iv_crypt, size_t iv_c_len,
					size_t num_elem, const u8 * addr[], const size_t * len,
					u8 * out)
{
	const u8 *_addr[6];
	size_t _len[6];
	const u8 *k1 = key, *k2 = key + 16;
	size_t crypt_len;
	size_t i;
	int ret;
	u8 iv[AES_BLK_SZ];
	u8 check[AES_BLK_SZ];

	if (iv_c_len < AES_BLK_SZ || num_elem > ARRAY_SIZE(_addr) - 1)
		return -1;
	crypt_len = iv_c_len - AES_BLK_SZ;

	for (i = 0; i < num_elem; i++) {
		_addr[i] = addr[i];
		_len[i] = len[i];
	}
	_addr[num_elem] = out;
	_len[num_elem] = crypt_len;

	os_memcpy(iv, iv_crypt, AES_BLK_SZ);
	os_memcpy(out, iv_crypt + AES_BLK_SZ, crypt_len);

	iv[8] &= 0x7f;
	iv[12] &= 0x7f;

	ret = aes_128_ctr_encrypt(k2, iv, out, crypt_len);
	if (ret)
		return ret;

	ret = aes_s2v(k1, num_elem + 1, _addr, _len, check);
	if (ret)
		return ret;
	if (memcmp(check, iv_crypt, AES_BLK_SZ) == 0)
		return 0;

	return -1;
}
