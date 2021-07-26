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


#ifndef SHA1_H
#define SHA1_H

#define SHA1_MAC_LEN 20
#include "common.h"
#include "wpa.h"
int hmac_sha1_vector(const u8 * key, size_t key_len, size_t num_elem,
					 const u8 * addr[], const size_t * len, u8 * mac);
int hmac_sha1(const u8 * key, size_t key_len, const u8 * data, size_t data_len,
			  u8 * mac);
int sha1_prf(const u8 * key, size_t key_len, const char *label,
			 const u8 * data, size_t data_len, u8 * buf, size_t buf_len);
int sha1_t_prf(const u8 * key, size_t key_len, const char *label,
			   const u8 * seed, size_t seed_len, u8 * buf, size_t buf_len);
int tls_prf_sha1_md5(const u8 * secret, size_t secret_len,
					 const char *label, const u8 * seed,
					 size_t seed_len, u8 * out, size_t outlen);
int pbkdf2_sha1(const char *passphrase, const u8 * ssid, size_t ssid_len,
				int iterations, u8 * buf, size_t buflen);

int hmac_sha384_vector(const u8 * key, size_t key_len, size_t num_elem,
					   const u8 * addr[], const size_t * len, u8 * mac);
#endif
