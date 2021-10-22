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

int wf_hmac_md5_vector(const wf_u8 * key, size_t key_len, size_t num_elem,
                       const wf_u8 * addr[], const size_t * len, wf_u8 * mac);
int wf_hmac_md5(const wf_u8 * key, size_t key_len, const wf_u8 * data, size_t data_len,
                wf_u8 * mac);

#endif
