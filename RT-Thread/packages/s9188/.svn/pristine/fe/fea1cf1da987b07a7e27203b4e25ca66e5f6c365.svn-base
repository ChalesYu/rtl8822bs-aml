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


#ifndef AES_H
#define AES_H
#include "utils/common.h"
#define AES_BLK_SZ 16

/* Because array size can't be a const in C, the following two are macros.
   Both sizes are in bytes. */
#define AES_MAXNR 14


/* This should be a hidden type, but EVP requires that the size be known */
struct aes_key_st {
    unsigned int rd_key[4 *(AES_MAXNR + 1)];
    int rounds;
};
typedef struct aes_key_st AES_KEY;


void *_init_aes_encrypt(const u8 * key, size_t len);
void aes_encrypt(void *ctx, const u8 * plain, u8 * crypt);
void _encrypt_deinit_aes(void *ctx);
void *aes_decrypt_init(const u8 * key, size_t len);
void aes_decrypt(void *ctx, const u8 * crypt, u8 * plain);
void aes_decrypt_deinit(void *ctx);

#endif
