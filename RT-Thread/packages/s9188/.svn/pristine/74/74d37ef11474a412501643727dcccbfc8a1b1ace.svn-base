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


#ifndef MD5_H
#define MD5_H

#define MD5_MAC_LEN 16

int hmac_md5_vector(const u8 * key, size_t key_len, size_t num_elem,
					const u8 * addr[], const size_t * len, u8 * mac);
int hmac_md5(const u8 * key, size_t key_len, const u8 * data, size_t data_len,
			 u8 * mac);

#endif
