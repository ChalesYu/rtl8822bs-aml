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


//#include "bsp.h"
//#include "type.h"
#include "common.h"
#include "sha384.h"
#include "crypto.h"
#include "wpa.h"

void sha384_prf(const u8 * key, size_t key_len, const char *label,
				const u8 * data, size_t data_len, u8 * buf, size_t buf_len)
{
	sha384_prf_bits(key, key_len, label, data, data_len, buf, buf_len * 8);
}

void sha384_prf_bits(const u8 * key, size_t key_len, const char *label,
					 const u8 * data, size_t data_len, u8 * buf,
					 size_t buf_len_bits)
{
	u16 counter = 1;
	size_t pos, plen;
	u8 hash[SHA384_MAC_LEN];
	const u8 *addr[4];
	size_t len[4];
	u8 counter_le[2], length_le[2];
	size_t buf_len = (buf_len_bits + 7) / 8;

	addr[0] = counter_le;
	len[0] = 2;
	addr[1] = (u8 *) label;
	len[1] = os_string_length(label);
	addr[2] = data;
	len[2] = data_len;
	addr[3] = length_le;
	len[3] = sizeof(length_le);

	TILK_WPA_TO_LE16(length_le, buf_len_bits);
	pos = 0;
	while (pos < buf_len) {
		plen = buf_len - pos;
		TILK_WPA_TO_LE16(counter_le, counter);
		if (plen >= SHA384_MAC_LEN) {
			hmac_sha384_vector(key, key_len, 4, addr, len, &buf[pos]);
			pos += SHA384_MAC_LEN;
		} else {
			hmac_sha384_vector(key, key_len, 4, addr, len, hash);
			os_memcpy(&buf[pos], hash, plen);
			pos += plen;
			break;
		}
		counter++;
	}

	if (buf_len_bits % 8) {
		u8 mask = 0xff << (8 - buf_len_bits % 8);
		buf[pos - 1] &= mask;
	}

	os_memset(hash, 0, sizeof(hash));
}
