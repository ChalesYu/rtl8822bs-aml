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
#include "aes.h"
#include "aes_wrap.h"

int aes_128_encrypt_block(const u8 * key, const u8 * in, u8 * out)
{
	void *ctx;
	ctx = _init_aes_encrypt(key, 16);
	if (ctx == NULL)
		return -1;
	aes_encrypt(ctx, in, out);
	_encrypt_deinit_aes(ctx);
	return 0;
}
