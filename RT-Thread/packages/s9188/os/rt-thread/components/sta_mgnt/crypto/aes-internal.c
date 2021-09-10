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
#include "utils/common.h"
#include "wpa.h"
#include "aes.h"

extern const wf_u32 Te0_TO_TK[256];

extern const wf_u32 Td0_to_tk[256];

const wf_u8 tk_rcons[] = {
  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

int wf_aes_keysetupenc_rijndael(wf_u32 rk[], const wf_u8 cipherKey[], int keyBits)
{
  int i;
  wf_u32 temp;
  
  rk[0] = OBATIN_UINT32(cipherKey);
  rk[1] = OBATIN_UINT32(cipherKey + 4);
  rk[2] = OBATIN_UINT32(cipherKey + 8);
  rk[3] = OBATIN_UINT32(cipherKey + 12);
  
  if (keyBits == 128) {
    for (i = 0; i < 10; i++) {
      temp = rk[3];
      rk[4] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
        TE443(temp) ^ TE414(temp) ^ TILK_RCON(i);
      rk[5] = rk[1] ^ rk[4];
      rk[6] = rk[2] ^ rk[5];
      rk[7] = rk[3] ^ rk[6];
      rk += 4;
    }
    return 10;
  }
  
  rk[4] = OBATIN_UINT32(cipherKey + 16);
  rk[5] = OBATIN_UINT32(cipherKey + 20);
  
  if (keyBits == 192) {
    for (i = 0; i < 8; i++) {
      temp = rk[5];
      rk[6] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
        TE443(temp) ^ TE414(temp) ^ TILK_RCON(i);
      rk[7] = rk[1] ^ rk[6];
      rk[8] = rk[2] ^ rk[7];
      rk[9] = rk[3] ^ rk[8];
      if (i == 7)
        return 12;
      rk[10] = rk[4] ^ rk[9];
      rk[11] = rk[5] ^ rk[10];
      rk += 6;
    }
  }
  
  rk[6] = OBATIN_UINT32(cipherKey + 24);
  rk[7] = OBATIN_UINT32(cipherKey + 28);
  
  if (keyBits == 256) {
    for (i = 0; i < 7; i++) {
      temp = rk[7];
      rk[8] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
        TE443(temp) ^ TE414(temp) ^ TILK_RCON(i);
      rk[9] = rk[1] ^ rk[8];
      rk[10] = rk[2] ^ rk[9];
      rk[11] = rk[3] ^ rk[10];
      if (i == 6)
        return 14;
      temp = rk[11];
      rk[12] = rk[4] ^ TE411(temp) ^ TE422(temp) ^
        TE433(temp) ^ TE444(temp);
      rk[13] = rk[5] ^ rk[12];
      rk[14] = rk[6] ^ rk[13];
      rk[15] = rk[7] ^ rk[14];
      rk += 8;
    }
  }
  
  return -1;
}
