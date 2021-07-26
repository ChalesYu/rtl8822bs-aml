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


#ifndef AES_SIV_H
#define AES_SIV_H

int aes_siv_encrypt(const u8 * key, const u8 * pw,
					size_t pwlen, size_t num_elem,
					const u8 * addr[], const size_t * len, u8 * out);
int aes_siv_decrypt(const u8 * key, const u8 * iv_crypt, size_t iv_c_len,
					size_t num_elem, const u8 * addr[], const size_t * len,
					u8 * out);

#endif
