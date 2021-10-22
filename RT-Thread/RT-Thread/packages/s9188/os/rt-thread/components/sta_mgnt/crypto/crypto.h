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
#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#define __must_check
#include "utils/common.h"
int wf_md4_vector(size_t num_elem, const wf_u8 * addr[], const size_t * len,
                  wf_u8 * mac);

int wf_md5_vector(size_t num_elem, const wf_u8 * addr[], const size_t * len,
                  wf_u8 * mac);

int wf_sha1_vector(size_t num_elem, const wf_u8 * addr[], const size_t * len,
                   wf_u8 * mac);

void des_encrypt(const wf_u8 * clear, const wf_u8 * key, wf_u8 * cypher);

void *wf_aes_init_enc(const wf_u8 * key, size_t len);

void wf_aes_enc(void *ctx, const wf_u8 * plain, wf_u8 * crypt);

void wf_aes_enc_deinit(void *ctx);

void *wf_aes_dec_init(const wf_u8 * key, size_t len);

void wf_aes_dec(void *ctx, const wf_u8 * crypt, wf_u8 * plain);

void wf_aes_dec_deinit(void *ctx);


#endif
