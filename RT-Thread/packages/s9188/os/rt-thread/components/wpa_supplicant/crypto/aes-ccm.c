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

static void xor_aes_block(u8 * dst, const u8 * src)
{
	u32 *d = (u32 *) dst;
	u32 *s = (u32 *) src;
	*d++ ^= *s++;
	*d++ ^= *s++;
	*d++ ^= *s++;
	*d++ ^= *s++;
}

static void aes_ccm_auth_start(void *aes, size_t M, size_t L, const u8 * nonce,
							   const u8 * aad, size_t aad_len, size_t plain_len,
							   u8 * x)
{
	u8 aad_buf[2 * AES_BLK_SZ];
	u8 b[AES_BLK_SZ];

	b[0] = aad_len ? 0x40 : 0;
	b[0] |= (((M - 2) / 2) << 3);
	b[0] |= (L - 1);
	os_memcpy(&b[1], nonce, 15 - L);
	WPA_PUT_BE16(&b[AES_BLK_SZ - L], plain_len);

	aes_encrypt(aes, b, x);

	if (!aad_len)
		return;

	WPA_PUT_BE16(aad_buf, aad_len);
	os_memcpy(aad_buf + 2, aad, aad_len);
	os_memset(aad_buf + 2 + aad_len, 0, sizeof(aad_buf) - 2 - aad_len);

	xor_aes_block(aad_buf, x);
	aes_encrypt(aes, aad_buf, x);

	if (aad_len > AES_BLK_SZ - 2) {
		xor_aes_block(&aad_buf[AES_BLK_SZ], x);
		aes_encrypt(aes, &aad_buf[AES_BLK_SZ], x);
	}
}

static void aes_ccm_auth(void *aes, const u8 * data, size_t len, u8 * x)
{
	size_t last = len % AES_BLK_SZ;
	size_t i;

	for (i = 0; i < len / AES_BLK_SZ; i++) {
		xor_aes_block(x, data);
		data += AES_BLK_SZ;
		aes_encrypt(aes, x, x);
	}
	if (last) {
		for (i = 0; i < last; i++)
			x[i] ^= *data++;
		aes_encrypt(aes, x, x);
	}
}

static void aes_ccm_encr_start(size_t L, const u8 * nonce, u8 * a)
{
	a[0] = L - 1;
	os_memcpy(&a[1], nonce, 15 - L);
}

static void aes_ccm_encr(void *aes, size_t L, const u8 * in, size_t len,
						 u8 * out, u8 * a)
{
	size_t last = len % AES_BLK_SZ;
	size_t i;

	for (i = 1; i <= len / AES_BLK_SZ; i++) {
		WPA_PUT_BE16(&a[AES_BLK_SZ - 2], i);
		aes_encrypt(aes, a, out);
		xor_aes_block(out, in);
		out += AES_BLK_SZ;
		in += AES_BLK_SZ;
	}
	if (last) {
		WPA_PUT_BE16(&a[AES_BLK_SZ - 2], i);
		aes_encrypt(aes, a, out);
		for (i = 0; i < last; i++)
			*out++ ^= *in++;
	}
}

static void aes_ccm_encr_auth(void *aes, size_t M, u8 * x, u8 * a, u8 * auth)
{
	size_t i;
	u8 tmp[AES_BLK_SZ];

	WPA_PUT_BE16(&a[AES_BLK_SZ - 2], 0);
	aes_encrypt(aes, a, tmp);
	for (i = 0; i < M; i++)
		auth[i] = x[i] ^ tmp[i];
}

static void aes_ccm_decr_auth(void *aes, size_t M, u8 * a, const u8 * auth,
							  u8 * t)
{
	size_t i;
	u8 tmp[AES_BLK_SZ];

	WPA_PUT_BE16(&a[AES_BLK_SZ - 2], 0);
	aes_encrypt(aes, a, tmp);
	for (i = 0; i < M; i++)
		t[i] = auth[i] ^ tmp[i];
}

int aes_ccm_ae(const u8 * key, size_t key_len, const u8 * nonce,
			   size_t M, const u8 * plain, size_t plain_len,
			   const u8 * aad, size_t aad_len, u8 * crypt, u8 * auth)
{
	const size_t L = 2;
	void *aes;
	u8 x[AES_BLK_SZ], a[AES_BLK_SZ];

	if (aad_len > 30 || M > AES_BLK_SZ)
		return -1;

	aes = _init_aes_encrypt(key, key_len);
	if (aes == NULL)
		return -1;

	aes_ccm_auth_start(aes, M, L, nonce, aad, aad_len, plain_len, x);
	aes_ccm_auth(aes, plain, plain_len, x);

	aes_ccm_encr_start(L, nonce, a);
	aes_ccm_encr(aes, L, plain, plain_len, crypt, a);
	aes_ccm_encr_auth(aes, M, x, a, auth);

	_encrypt_deinit_aes(aes);

	return 0;
}

int aes_ccm_ad(const u8 * key, size_t key_len, const u8 * nonce,
			   size_t M, const u8 * crypt, size_t crypt_len,
			   const u8 * aad, size_t aad_len, const u8 * auth, u8 * plain)
{
	const size_t L = 2;
	void *aes;
	u8 x[AES_BLK_SZ], a[AES_BLK_SZ];
	u8 t[AES_BLK_SZ];

	if (aad_len > 30 || M > AES_BLK_SZ)
		return -1;

	aes = _init_aes_encrypt(key, key_len);
	if (aes == NULL)
		return -1;

	aes_ccm_encr_start(L, nonce, a);
	aes_ccm_decr_auth(aes, M, a, auth, t);

	aes_ccm_encr(aes, L, crypt, crypt_len, plain, a);

	aes_ccm_auth_start(aes, M, L, nonce, aad, aad_len, crypt_len, x);
	aes_ccm_auth(aes, plain, crypt_len, x);

	_encrypt_deinit_aes(aes);

	if (os_memcmp_const(x, t, M) != 0) {
		return -1;
	}

	return 0;
}
