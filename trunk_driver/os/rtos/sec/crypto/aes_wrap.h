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


#ifndef AES_WRAP_H
#define AES_WRAP_H
#include "sec/wpa.h"
#include "common.h"

int omac1_aes_vector(const wf_u8 * key, size_t key_len,
					 size_t num_elem, const wf_u8 * addr[],
					 const size_t * len, wf_u8 * mac);
int omac1_aes_tilk_128_vector(const wf_u8 * key, size_t num_elem,
							  const wf_u8 * addr[], const size_t * len, wf_u8 * mac);
int omac1_aes_tilk_128(const wf_u8 * key, const wf_u8 * data, size_t data_len,
					   wf_u8 * mac);
int omac1_aes_256(const wf_u8 * key, const wf_u8 * data, size_t data_len, wf_u8 * mac);
int aes_128_encrypt_block(const wf_u8 * key, const wf_u8 * in, wf_u8 * out);
int aes_128_ctr_encrypt(const wf_u8 * key, const wf_u8 * nonce,
						wf_u8 * data, size_t data_len);
int aes_128_eax_encrypt(const wf_u8 * key,
						const wf_u8 * nonce, size_t nonce_len,
						const wf_u8 * hdr, size_t hdr_len,
						wf_u8 * data, size_t data_len, wf_u8 * tag);
int aes_128_eax_decrypt(const wf_u8 * key,
						const wf_u8 * nonce, size_t nonce_len,
						const wf_u8 * hdr, size_t hdr_len,
						wf_u8 * data, size_t data_len, const wf_u8 * tag);
int aes_128_cbc_encrypt(const wf_u8 * key, const wf_u8 * iv, wf_u8 * data,
						size_t data_len);
int aes_128_cbc_decrypt(const wf_u8 * key, const wf_u8 * iv, wf_u8 * data,
						size_t data_len);
int aes_gcm_ae(const wf_u8 * key, size_t key_len,
			   const wf_u8 * iv, size_t iv_len,
			   const wf_u8 * plain, size_t plain_len,
			   const wf_u8 * aad, size_t aad_len, wf_u8 * crypt, wf_u8 * tag);
int aes_gcm_ad(const wf_u8 * key, size_t key_len,
			   const wf_u8 * iv, size_t iv_len,
			   const wf_u8 * crypt, size_t crypt_len,
			   const wf_u8 * aad, size_t aad_len, const wf_u8 * tag, wf_u8 * plain);
int aes_gmac(const wf_u8 * key, size_t key_len,
			 const wf_u8 * iv, size_t iv_len,
			 const wf_u8 * aad, size_t aad_len, wf_u8 * tag);
int aes_ccm_ae(const wf_u8 * key, size_t key_len, const wf_u8 * nonce,
			   size_t M, const wf_u8 * plain, size_t plain_len,
			   const wf_u8 * aad, size_t aad_len, wf_u8 * crypt, wf_u8 * auth);
int aes_ccm_ad(const wf_u8 * key, size_t key_len, const wf_u8 * nonce,
			   size_t M, const wf_u8 * crypt, size_t crypt_len,
			   const wf_u8 * aad, size_t aad_len, const wf_u8 * auth, wf_u8 * plain);

#endif
