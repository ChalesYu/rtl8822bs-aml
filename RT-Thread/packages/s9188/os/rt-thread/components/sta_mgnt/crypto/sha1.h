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
#ifndef __SHA1_I_H__
#define __SHA1_I_H__

struct sha1context {
  wf_u32 state[5];
  wf_u32 count[2];
  unsigned char buffer[64];
};

void wf_sha1_init(struct sha1context *context);
void wf_sha1_update(struct sha1context *context, const void *data, wf_u32 len);
void wf_sha1_final(unsigned char digest[20], struct sha1context *context);
void wf_sha1_transform(wf_u32 state[5], const unsigned char buffer[64]);

int wf_hmac_sha1_vector(const wf_u8 * key, size_t key_len, size_t num_elem,
                        const wf_u8 * addr[], const size_t * len, wf_u8 * mac);
int wf_hmac_sha1(const wf_u8 * key, size_t key_len, const wf_u8 * data, size_t data_len,
                 wf_u8 * mac);
int wf_sha1_prf(const wf_u8 * key, size_t key_len, const char *label,
                const wf_u8 * data, size_t data_len, wf_u8 * buf, size_t buf_len);
int sha1_t_prf(const wf_u8 * key, size_t key_len, const char *label,
               const wf_u8 * seed, size_t seed_len, wf_u8 * buf, size_t buf_len);
int tls_prf_sha1_md5(const wf_u8 * secret, size_t secret_len,
                     const char *label, const wf_u8 * seed,
                     size_t seed_len, wf_u8 * out, size_t outlen);
int wf_pbkdf2_sha1(const char *passphrase, const wf_u8 * ssid, size_t ssid_len,
                   int iterations, wf_u8 * buf, size_t buflen);

int hmac_sha384_vector(const wf_u8 * key, size_t key_len, size_t num_elem,
                       const wf_u8 * addr[], const size_t * len, wf_u8 * mac);
#endif
