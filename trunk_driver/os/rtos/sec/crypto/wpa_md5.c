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
#include "sec/utils/common.h"
#include "wpa_md5.h"
#include "sec/wpa.h"
#include "sec/crypto/crypto.h"

int wf_hmac_md5_vector(const wf_u8 * key, size_t key_len, size_t num_elem,
                       const wf_u8 * addr[], const size_t * len, wf_u8 * mac)
{
    wf_u8 k_pad[64];
    wf_u8 tk[16];
    const wf_u8 *_addr[6];
    size_t i, _len[6];
    int res;

    if (num_elem > 5)
    {

        return -1;
    }

    if (key_len > 64)
    {
        if (wf_md5_vector(1, &key, &key_len, tk))
            return -1;
        key = tk;
        key_len = 16;
    }

    os_memset(k_pad, 0, sizeof(k_pad));
    os_memcpy(k_pad, key, key_len);

    for (i = 0; i < 64; i++)
        k_pad[i] ^= 0x36;

    _addr[0] = k_pad;
    _len[0] = 64;
    for (i = 0; i < num_elem; i++)
    {
        _addr[i + 1] = addr[i];
        _len[i + 1] = len[i];
    }
    if (wf_md5_vector(1 + num_elem, _addr, _len, mac))
        return -1;

    os_memset(k_pad, 0, sizeof(k_pad));
    os_memcpy(k_pad, key, key_len);

    for (i = 0; i < 64; i++)
        k_pad[i] ^= 0x5c;

    _addr[0] = k_pad;
    _len[0] = 64;
    _addr[1] = mac;
    _len[1] = MD5_MAC_LEN;
    res = wf_md5_vector(2, _addr, _len, mac);
    os_memset(k_pad, 0, sizeof(k_pad));
    os_memset(tk, 0, sizeof(tk));
    return res;
}

int wf_hmac_md5(const wf_u8 * key, size_t key_len, const wf_u8 * data, size_t data_len,
                wf_u8 * mac)
{
    return wf_hmac_md5_vector(key, key_len, 1, &data, &data_len, mac);
}
