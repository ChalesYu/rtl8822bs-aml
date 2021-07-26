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


#ifndef DES_I_H
#define DES_I_H

struct des3_key_s {
	u32 ek[3][32];
	u32 dk[3][32];
};

void des_key_setup(const u8 * key, u32 * ek, u32 * dk);
void des_block_encrypt(const u8 * plain, const u32 * ek, u8 * crypt);
void des_block_decrypt(const u8 * crypt, const u32 * dk, u8 * plain);

void des3_key_setup(const u8 * key, struct des3_key_s *dkey);
void des3_encrypt(const u8 * plain, const struct des3_key_s *key, u8 * crypt);
void des3_decrypt(const u8 * crypt, const struct des3_key_s *key, u8 * plain);

#endif
