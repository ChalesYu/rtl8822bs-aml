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


#ifndef CRYPTO_H
#define CRYPTO_H

#define __must_check
//#include "bsp.h"
//#include "type.h"
#include "utils/common.h"
int md4_vector(size_t num_elem, const u8 * addr[], const size_t * len,
			   u8 * mac);

int md5_vector(size_t num_elem, const u8 * addr[], const size_t * len,
			   u8 * mac);

int sha1_vector(size_t num_elem, const u8 * addr[], const size_t * len,
				u8 * mac);

int __must_check fips186_2_prf(const u8 * seed, size_t seed_len, u8 * x,
							   size_t xlen);

int sha256_vect(size_t num_elem, const u8 * addr[], const size_t * len,
				u8 * mac);

void des_encrypt(const u8 * clear, const u8 * key, u8 * cypher);

void *_init_aes_encrypt(const u8 * key, size_t len);

void aes_encrypt(void *ctx, const u8 * plain, u8 * crypt);

void _encrypt_deinit_aes(void *ctx);

void *aes_decrypt_init(const u8 * key, size_t len);

void aes_decrypt(void *ctx, const u8 * crypt, u8 * plain);

void aes_decrypt_deinit(void *ctx);

enum crypto_hash_alg {
	CRYPTO_HASH_ALG_MD5, CRYPTO_HASH_ALG_SHA1,
	CRYPTO_HASH_ALG_HMAC_MD5, CRYPTO_HASH_ALG_HMAC_SHA1,
	CRYPTO_HASH_ALG_SHA256, CRYPTO_HASH_ALG_HMAC_SHA256
};

struct crypto_hash;

struct crypto_hash *crypto_hash_init(enum crypto_hash_alg alg, const u8 * key,
									 size_t key_len);

void crypto_hash_update(struct crypto_hash *ctx, const u8 * data, size_t len);

int crypto_hash_finish(struct crypto_hash *ctx, u8 * hash, size_t * len);

enum crypto_cipher_alg {
	CRYPTO_CIPHER_NULL = 0, CRYPTO_CIPHER_ALG_AES, CRYPTO_CIPHER_ALG_3DES,
	CRYPTO_CIPHER_ALG_DES, CRYPTO_CIPHER_ALG_RC2, CRYPTO_CIPHER_ALG_RC4
};

struct crypto_cipher;

struct crypto_cipher *crypto_cipher_init(enum crypto_cipher_alg alg,
										 const u8 * iv, const u8 * key,
										 size_t key_len);

int __must_check crypto_cipher_encrypt(struct crypto_cipher *ctx,
									   const u8 * plain, u8 * crypt,
									   size_t len);

int __must_check crypto_cipher_decrypt(struct crypto_cipher *ctx,
									   const u8 * crypt, u8 * plain,
									   size_t len);

void crypto_cipher_deinit(struct crypto_cipher *ctx);

struct crypto_public_key;
struct crypto_private_key;

struct crypto_public_key *crypto_public_key_import(const u8 * key, size_t len);

struct crypto_public_key *crypto_public_key_import_parts(const u8 * n,
														 size_t n_len,
														 const u8 * e,
														 size_t e_len);

struct crypto_private_key *crypto_private_key_import(const u8 * key,
													 size_t len,
													 const char *passwd);

struct crypto_public_key *crypto_public_key_from_cert(const u8 * buf,
													  size_t len);

int __must_check crypto_public_key_encrypt_pkcs1_v15(struct crypto_public_key
													 *key, const u8 * in,
													 size_t inlen, u8 * out,
													 size_t * outlen);

int __must_check crypto_private_key_decrypt_pkcs1_v15(struct crypto_private_key
													  *key, const u8 * in,
													  size_t inlen, u8 * out,
													  size_t * outlen);

int __must_check crypto_private_key_sign_pkcs1(struct crypto_private_key *key,
											   const u8 * in, size_t inlen,
											   u8 * out, size_t * outlen);

void crypto_public_key_free(struct crypto_public_key *key);

void crypto_private_key_free(struct crypto_private_key *key);

int __must_check crypto_public_key_decrypt_pkcs1(struct crypto_public_key *key,
												 const u8 * crypt,
												 size_t crypt_len, u8 * plain,
												 size_t * plain_len);

int __must_check crypto_global_init(void);

void crypto_global_deinit(void);

int __must_check crypto_mod_exp(const u8 * base, size_t base_len,
								const u8 * power, size_t power_len,
								const u8 * modulus, size_t modulus_len,
								u8 * result, size_t * result_len);

int rc4_skip(const u8 * key, size_t keylen, size_t skip,
			 u8 * data, size_t data_len);

int crypto_get_random(void *buf, size_t len);

struct crypto_bignum;

struct crypto_bignum *crypto_bignum_init(void);

struct crypto_bignum *crypto_bignum_init_set(const u8 * buf, size_t len);

void crypto_bignum_deinit(struct crypto_bignum *n, int clear);

int crypto_bignum_to_bin(const struct crypto_bignum *a,
						 u8 * buf, size_t buflen, size_t padlen);

int crypto_bignum_add(const struct crypto_bignum *a,
					  const struct crypto_bignum *b, struct crypto_bignum *c);

int crypto_bignum_mod(const struct crypto_bignum *a,
					  const struct crypto_bignum *b, struct crypto_bignum *c);

int crypto_bignum_exptmod(const struct crypto_bignum *a,
						  const struct crypto_bignum *b,
						  const struct crypto_bignum *c,
						  struct crypto_bignum *d);

int crypto_bignum_inverse(const struct crypto_bignum *a,
						  const struct crypto_bignum *b,
						  struct crypto_bignum *c);

int crypto_bignum_sub(const struct crypto_bignum *a,
					  const struct crypto_bignum *b, struct crypto_bignum *c);

int crypto_bignum_div(const struct crypto_bignum *a,
					  const struct crypto_bignum *b, struct crypto_bignum *c);

int crypto_bignum_mulmod(const struct crypto_bignum *a,
						 const struct crypto_bignum *b,
						 const struct crypto_bignum *c,
						 struct crypto_bignum *d);

int crypto_bignum_cmp(const struct crypto_bignum *a,
					  const struct crypto_bignum *b);

int crypto_bignum_bits(const struct crypto_bignum *a);

int crypto_bignum_is_zero(const struct crypto_bignum *a);

int crypto_bignum_is_one(const struct crypto_bignum *a);

int crypto_bignum_legendre(const struct crypto_bignum *a,
						   const struct crypto_bignum *p);

struct crypto_ec;

struct crypto_ec *crypto_ec_init(int group);

void crypto_ec_deinit(struct crypto_ec *e);

size_t crypto_ec_prime_len(struct crypto_ec *e);

size_t crypto_ec_prime_len_bits(struct crypto_ec *e);

const struct crypto_bignum *crypto_ec_get_prime(struct crypto_ec *e);

const struct crypto_bignum *crypto_ec_get_order(struct crypto_ec *e);

struct crypto_ec_point;

struct crypto_ec_point *crypto_ec_point_init(struct crypto_ec *e);

void crypto_ec_point_deinit(struct crypto_ec_point *p, int clear);

int crypto_ec_point_to_bin(struct crypto_ec *e,
						   const struct crypto_ec_point *point, u8 * x, u8 * y);

struct crypto_ec_point *crypto_ec_point_from_bin(struct crypto_ec *e,
												 const u8 * val);

int crypto_ec_point_add(struct crypto_ec *e, const struct crypto_ec_point *a,
						const struct crypto_ec_point *b,
						struct crypto_ec_point *c);

int crypto_ec_point_mul(struct crypto_ec *e, const struct crypto_ec_point *p,
						const struct crypto_bignum *b,
						struct crypto_ec_point *res);

int crypto_ec_point_invert(struct crypto_ec *e, struct crypto_ec_point *p);

int crypto_ec_point_solve_y_coord(struct crypto_ec *e,
								  struct crypto_ec_point *p,
								  const struct crypto_bignum *x, int y_bit);

struct crypto_bignum *crypto_ec_point_compute_y_sqr(struct crypto_ec *e,
													const struct crypto_bignum
													*x);

int crypto_ec_point_is_at_infinity(struct crypto_ec *e,
								   const struct crypto_ec_point *p);

int crypto_ec_point_is_on_curve(struct crypto_ec *e,
								const struct crypto_ec_point *p);

int crypto_ec_point_cmp(const struct crypto_ec *e,
						const struct crypto_ec_point *a,
						const struct crypto_ec_point *b);

#endif
