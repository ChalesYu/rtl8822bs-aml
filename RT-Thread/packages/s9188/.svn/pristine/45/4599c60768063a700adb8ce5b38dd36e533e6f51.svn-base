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


#ifndef SHA384_H
#define SHA384_H

#define SHA384_MAC_LEN 48
#include "utils/common.h"
int hmac_sha384_vector(const u8 * key, size_t key_len, size_t num_elem,
					   const u8 * addr[], const size_t * len, u8 * mac);
int hmac_sha384(const u8 * key, size_t key_len, const u8 * data,
				size_t data_len, u8 * mac);
void sha384_prf(const u8 * key, size_t key_len, const char *label,
				const u8 * data, size_t data_len, u8 * buf, size_t buf_len);
void sha384_prf_bits(const u8 * key, size_t key_len, const char *label,
					 const u8 * data, size_t data_len, u8 * buf,
					 size_t buf_len_bits);

#endif
