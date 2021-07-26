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
#include "crypto.h"
//#include "tls/rsa.h"
//#include "tls/pkcs1.h"
//#include "tls/pkcs8.h"

struct crypto_public_key;
struct crypto_private_key;

struct crypto_public_key *crypto_public_key_import(const u8 * key, size_t len)
{
	return (struct crypto_public_key *)
		crypto_rsa_import_public_key(key, len);
}

struct crypto_public_key *crypto_public_key_import_parts(const u8 * n,
														 size_t n_len,
														 const u8 * e,
														 size_t e_len)
{
	return (struct crypto_public_key *)
		crypto_rsa_import_public_key_parts(n, n_len, e, e_len);
}

struct crypto_private_key *crypto_private_key_import(const u8 * key,
													 size_t len,
													 const char *passwd)
{
	struct crypto_private_key *res;

	res = pkcs8_key_import(key, len);
	if (res)
		return res;

	if (passwd) {
		res = pkcs8_enc_key_import(key, len, passwd);
		if (res)
			return res;
	}

	wpa_printf(MSG_DEBUG, "Trying to parse PKCS #1 encoded RSA private " "key");
	return (struct crypto_private_key *)
		crypto_rsa_import_private_key(key, len);
}

struct crypto_public_key *crypto_public_key_from_cert(const u8 * buf,
													  size_t len)
{
	return NULL;
}

int crypto_public_key_encrypt_pkcs1_v15(struct crypto_public_key *key,
										const u8 * in, size_t inlen,
										u8 * out, size_t * outlen)
{
	return pkcs1_encrypt(2, (struct crypto_rsa_key *)key,
						 0, in, inlen, out, outlen);
}

int crypto_private_key_decrypt_pkcs1_v15(struct crypto_private_key *key,
										 const u8 * in, size_t inlen,
										 u8 * out, size_t * outlen)
{
	return pkcs1_v15_private_key_decrypt((struct crypto_rsa_key *)key,
										 in, inlen, out, outlen);
}

int crypto_private_key_sign_pkcs1(struct crypto_private_key *key,
								  const u8 * in, size_t inlen,
								  u8 * out, size_t * outlen)
{
	return pkcs1_encrypt(1, (struct crypto_rsa_key *)key,
						 1, in, inlen, out, outlen);
}

void crypto_public_key_free(struct crypto_public_key *key)
{
	crypto_rsa_free((struct crypto_rsa_key *)key);
}

void crypto_private_key_free(struct crypto_private_key *key)
{
	crypto_rsa_free((struct crypto_rsa_key *)key);
}

int crypto_public_key_decrypt_pkcs1(struct crypto_public_key *key,
									const u8 * crypt, size_t crypt_len,
									u8 * plain, size_t * plain_len)
{
	return pkcs1_decrypt_public_key((struct crypto_rsa_key *)key,
									crypt, crypt_len, plain, plain_len);
}
