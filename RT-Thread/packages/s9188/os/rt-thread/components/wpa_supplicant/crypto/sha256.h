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


#ifndef SHA256_H
#define SHA256_H

#define SHA256_LEN_MAC 32

#include "utils/common.h"

int vector_hmac_sha256(const u8 * key, size_t key_len, size_t num_elem,
					   const u8 * addr[], const size_t * len, u8 * mac);
int hmac_sha256(const u8 * key, size_t key_len, const u8 * data,
				size_t data_len, u8 * mac);
void prf_sha256(const u8 * key, size_t key_len, const char *label,
				const u8 * data, size_t data_len, u8 * buf, size_t buf_len);
void sha256_prf_bits(const u8 * key, size_t key_len, const char *label,
					 const u8 * data, size_t data_len, u8 * buf,
					 size_t buf_len_bits);
void tls_prf_sha256(const u8 * secret, size_t secret_len,
					const char *label, const u8 * seed, size_t seed_len,
					u8 * out, size_t outlen);
int hmac_sha256_kdf(const u8 * secret, size_t secret_len,
					const char *label, const u8 * seed, size_t seed_len,
					u8 * out, size_t outlen);

#endif
