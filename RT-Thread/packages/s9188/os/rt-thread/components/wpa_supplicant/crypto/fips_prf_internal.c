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


#include "includes.h"

#include "common.h"
#include "sha1.h"
#include "sha1_i.h"
#include "crypto.h"

int fips186_2_prf(const u8 * seed, size_t seed_len, u8 * x, size_t xlen)
{
	u8 xkey[64];
	u32 t[5], _t[5];
	int i, j, m, k;
	u8 *xpos = x;
	u32 carry;

	if (seed_len < sizeof(xkey))
		os_memset(xkey + seed_len, 0, sizeof(xkey) - seed_len);
	else
		seed_len = sizeof(xkey);

	os_memcpy(xkey, seed, seed_len);
	t[0] = 0x67452301;
	t[1] = 0xEFCDAB89;
	t[2] = 0x98BADCFE;
	t[3] = 0x10325476;
	t[4] = 0xC3D2E1F0;

	m = xlen / 40;
	for (j = 0; j < m; j++) {
		for (i = 0; i < 2; i++) {

			os_memcpy(_t, t, 20);
			SHA1Transform(_t, xkey);
			_t[0] = host_to_be32(_t[0]);
			_t[1] = host_to_be32(_t[1]);
			_t[2] = host_to_be32(_t[2]);
			_t[3] = host_to_be32(_t[3]);
			_t[4] = host_to_be32(_t[4]);
			os_memcpy(xpos, _t, 20);

			carry = 1;
			for (k = 19; k >= 0; k--) {
				carry += xkey[k] + xpos[k];
				xkey[k] = carry & 0xff;
				carry >>= 8;
			}

			xpos += SHA1_MAC_LEN;
		}
	}

	return 0;
}
