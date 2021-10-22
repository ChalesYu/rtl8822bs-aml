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


#ifndef MD5_I_H
#define MD5_I_H
#define MD5_MAC_LEN 16

struct md5context {
  wf_u32 buf[4];
  wf_u32 bits[2];
  wf_u8 in[64];
};

void wf_md5_init(struct md5context *context);
void wf_md5_update(struct md5context *context, unsigned char const *buf,
               unsigned len);
void wf_md5_final(unsigned char digest[16], struct md5context *context);

int wf_hmac_md5_vector(const wf_u8 *key, size_t key_len, size_t num_elem,
		    const wf_u8 *addr[], const size_t *len, wf_u8 *mac);
int wf_hmac_md5(const wf_u8 *key, size_t key_len, const wf_u8 *data, size_t data_len,
	     wf_u8 *mac);
#endif
