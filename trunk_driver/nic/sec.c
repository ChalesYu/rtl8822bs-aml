/*
 * sec.c
 *
 * used for impliment IEEE80211 data frame code and decode logic process
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

#define SEC_DBG(fmt, ...)       LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define SEC_WARN(fmt, ...)      LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define SEC_ARRAY(data, len)    log_array(data, len)

extern int tkip_encrypt (struct xmit_frame *pxmitframe, wf_u8 *pdata, wf_u32 len);
extern int tkip_decrypt (prx_pkt_t ppkt);

extern int wep_encrypt (struct xmit_frame *pxmitframe, wf_u8 *pdata, wf_u32 len);
extern int wep_decrypt (prx_pkt_t prx_pkt);


static wf_u32 get_usecmic (wf_u8 *p)
{
    wf_s32 i;
    wf_u32 res = 0;

    for (i = 0; i < 4; i++)
    {
        res |= ((wf_u32) (*p++)) << (8 * i);
    }

    return res;
}

static void put_usecmic (wf_u8 *p, wf_u32 val)
{
    long i;

    for (i = 0; i < 4; i++)
    {
        *p++ = (wf_u8) (val & 0xff);
        val >>= 8;
    }
}

static void clr_secmic (struct mic_data *pmicdata)
{
    pmicdata->M = 0;
    pmicdata->nBytesInM = 0;
    pmicdata->L = pmicdata->K0;
    pmicdata->R = pmicdata->K1;
}

void wf_sec_mic_set_key (struct mic_data *pmicdata, wf_u8 *key)
{
    pmicdata->K0 = get_usecmic(key);
    pmicdata->K1 = get_usecmic(key + 4);
    clr_secmic(pmicdata);
}

static void mic_append_byte (struct mic_data *pmicdata, wf_u8 b)
{
    pmicdata->M |= ((unsigned long)b) << (8 * pmicdata->nBytesInM);
    pmicdata->nBytesInM++;
    if (pmicdata->nBytesInM >= 4)
    {
        pmicdata->L ^= pmicdata->M;
        pmicdata->R ^= ROL32(pmicdata->L, 17);
        pmicdata->L += pmicdata->R;
        pmicdata->R ^=
            ((pmicdata->L & 0xff00ff00) >> 8) | ((pmicdata->
                    L & 0x00ff00ff) << 8);
        pmicdata->L += pmicdata->R;
        pmicdata->R ^= ROL32(pmicdata->L, 3);
        pmicdata->L += pmicdata->R;
        pmicdata->R ^= ROR32(pmicdata->L, 2);
        pmicdata->L += pmicdata->R;
        pmicdata->M = 0;
        pmicdata->nBytesInM = 0;
    }
}

void wf_sec_mic_append (struct mic_data *pmicdata, wf_u8 *src, wf_u32 nbytes)
{
    while (nbytes > 0)
    {
        mic_append_byte(pmicdata, *src++);
        nbytes--;
    }
}

void wf_sec_get_mic (struct mic_data *pmicdata, wf_u8 *dst)
{
    mic_append_byte(pmicdata, 0x5a);
    mic_append_byte(pmicdata, 0);
    mic_append_byte(pmicdata, 0);
    mic_append_byte(pmicdata, 0);
    mic_append_byte(pmicdata, 0);
    while (pmicdata->nBytesInM != 0)
    {
        mic_append_byte(pmicdata, 0);
    }
    put_usecmic(dst, pmicdata->L);
    put_usecmic(dst + 4, pmicdata->R);
    clr_secmic(pmicdata);
}


int wf_sec_encrypt (void *xmitframe, wf_u8 *pdata, wf_u32 len)
{
    int res = 0;
    struct xmit_frame *pxmitframe = (struct xmit_frame *)xmitframe;

    if (pxmitframe == NULL || pdata == NULL || len == 0)
    {
        res = -1;
        goto exit;
    }

    switch (pxmitframe->encrypt_algo)
    {
        case _TKIP_ :
            res = tkip_encrypt(pxmitframe, pdata, len);
            break;

        case _WEP40_ :
        case _WEP104_ :
            res = wep_encrypt(pxmitframe, pdata, len);
            break;

        case _NO_PRIVACY_ :
        case _AES_ :
        default :
            break;
    }

exit:
    return res;
}

int wf_sec_decryptor (void *ptr)
{
    prx_pkt_t prx_pkt = ptr;
    prx_pkt_info_t prx_pkt_info = &prx_pkt->pkt_info;
    int res = 0;

    if (!!GET_HDR_Protected(prx_pkt->pdata))
    {
        switch (prx_pkt_info->encrypt_algo)
        {
            case _WEP40_:
            case _WEP104_:
                res = wep_decrypt(prx_pkt);
                break;

            case _TKIP_:
                res = tkip_decrypt(prx_pkt);
                break;

            case _AES_:
            default:
                break;
        }
    }

    prx_pkt_info->bdecrypted = wf_true;

    return res;
}


int wf_sec_info_init (nic_info_st *nic_info)
{
    sec_info_st *sec_info;

    SEC_DBG("sec_info init");
    sec_info = wf_kzalloc(sizeof(sec_info_st));
    if (sec_info == NULL)
    {
        SEC_WARN("malloc sec_info failed");
        nic_info->sec_info = NULL;
        return -1;
    }

    nic_info->sec_info = sec_info;
    return 0;
}

int wf_sec_info_term (nic_info_st *nic_info)
{
    sec_info_st *sec_info = nic_info->sec_info;

    if (sec_info == NULL)
    {
        return 0;
    }
    LOG_D("[wf_sec_info_term] start");

    if (sec_info)
    {
        wf_kfree(sec_info);
        nic_info->sec_info = NULL;
    }

    LOG_D("[wf_sec_info_term] end");

    return 0;
}

