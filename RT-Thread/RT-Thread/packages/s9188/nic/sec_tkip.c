/*
 * sec_tkip.c
 *
 * impliment tkip arithmetic used for WPA encryption
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "common.h"
#include "wf_debug.h"

#define RotR1(v16)      ((((v16) >> 1) & 0x7FFF) ^ (((v16) & 1) << 15))
#define Lo8(v16)        ((wf_u8) ((v16)         & 0x00FF))
#define Hi8(v16)        ((wf_u8) (((v16) >> 8)  & 0x00FF))
#define Lo16(v32)       ((wf_u16)((v32)         & 0xFFFF))
#define Hi16(v32)       ((wf_u16)(((v32) >> 16) & 0xFFFF))
#define Mk16(hi, lo)    ((lo) ^ (((wf_u16)(hi)) << 8))

#define TK16(N) Mk16(tk[2*(N)+1], tk[2*(N)])

#define _S_(v16) (Sbox1[0][Lo8(v16)] ^ Sbox1[1][Hi8(v16)])

#define PHASE1_LOOP_CNT 8
#define TA_SIZE         6
#define TK_SIZE         16
#define P1K_SIZE        10
#define RC4_KEY_SIZE    16

static const unsigned short Sbox1[2][256] = 
{
    {
        0xC6A5, 0xF884, 0xEE99, 0xF68D, 0xFF0D, 0xD6BD, 0xDEB1, 0x9154,
        0x6050, 0x0203, 0xCEA9, 0x567D, 0xE719, 0xB562, 0x4DE6, 0xEC9A,
        0x8F45, 0x1F9D, 0x8940, 0xFA87, 0xEF15, 0xB2EB, 0x8EC9, 0xFB0B,
        0x41EC, 0xB367, 0x5FFD, 0x45EA, 0x23BF, 0x53F7, 0xE496, 0x9B5B,
        0x75C2, 0xE11C, 0x3DAE, 0x4C6A, 0x6C5A, 0x7E41, 0xF502, 0x834F,
        0x685C, 0x51F4, 0xD134, 0xF908, 0xE293, 0xAB73, 0x6253, 0x2A3F,
        0x080C, 0x9552, 0x4665, 0x9D5E, 0x3028, 0x37A1, 0x0A0F, 0x2FB5,
        0x0E09, 0x2436, 0x1B9B, 0xDF3D, 0xCD26, 0x4E69, 0x7FCD, 0xEA9F,
        0x121B, 0x1D9E, 0x5874, 0x342E, 0x362D, 0xDCB2, 0xB4EE, 0x5BFB,
        0xA4F6, 0x764D, 0xB761, 0x7DCE, 0x527B, 0xDD3E, 0x5E71, 0x1397,
        0xA6F5, 0xB968, 0x0000, 0xC12C, 0x4060, 0xE31F, 0x79C8, 0xB6ED,
        0xD4BE, 0x8D46, 0x67D9, 0x724B, 0x94DE, 0x98D4, 0xB0E8, 0x854A,
        0xBB6B, 0xC52A, 0x4FE5, 0xED16, 0x86C5, 0x9AD7, 0x6655, 0x1194,
        0x8ACF, 0xE910, 0x0406, 0xFE81, 0xA0F0, 0x7844, 0x25BA, 0x4BE3,
        0xA2F3, 0x5DFE, 0x80C0, 0x058A, 0x3FAD, 0x21BC, 0x7048, 0xF104,
        0x63DF, 0x77C1, 0xAF75, 0x4263, 0x2030, 0xE51A, 0xFD0E, 0xBF6D,
        0x814C, 0x1814, 0x2635, 0xC32F, 0xBEE1, 0x35A2, 0x88CC, 0x2E39,
        0x9357, 0x55F2, 0xFC82, 0x7A47, 0xC8AC, 0xBAE7, 0x322B, 0xE695,
        0xC0A0, 0x1998, 0x9ED1, 0xA37F, 0x4466, 0x547E, 0x3BAB, 0x0B83,
        0x8CCA, 0xC729, 0x6BD3, 0x283C, 0xA779, 0xBCE2, 0x161D, 0xAD76,
        0xDB3B, 0x6456, 0x744E, 0x141E, 0x92DB, 0x0C0A, 0x486C, 0xB8E4,
        0x9F5D, 0xBD6E, 0x43EF, 0xC4A6, 0x39A8, 0x31A4, 0xD337, 0xF28B,
        0xD532, 0x8B43, 0x6E59, 0xDAB7, 0x018C, 0xB164, 0x9CD2, 0x49E0,
        0xD8B4, 0xACFA, 0xF307, 0xCF25, 0xCAAF, 0xF48E, 0x47E9, 0x1018,
        0x6FD5, 0xF088, 0x4A6F, 0x5C72, 0x3824, 0x57F1, 0x73C7, 0x9751,
        0xCB23, 0xA17C, 0xE89C, 0x3E21, 0x96DD, 0x61DC, 0x0D86, 0x0F85,
        0xE090, 0x7C42, 0x71C4, 0xCCAA, 0x90D8, 0x0605, 0xF701, 0x1C12,
        0xC2A3, 0x6A5F, 0xAEF9, 0x69D0, 0x1791, 0x9958, 0x3A27, 0x27B9,
        0xD938, 0xEB13, 0x2BB3, 0x2233, 0xD2BB, 0xA970, 0x0789, 0x33A7,
        0x2DB6, 0x3C22, 0x1592, 0xC920, 0x8749, 0xAAFF, 0x5078, 0xA57A,
        0x038F, 0x59F8, 0x0980, 0x1A17, 0x65DA, 0xD731, 0x84C6, 0xD0B8,
        0x82C3, 0x29B0, 0x5A77, 0x1E11, 0x7BCB, 0xA8FC, 0x6DD6, 0x2C3A,
    },

    { 
        0xA5C6, 0x84F8, 0x99EE, 0x8DF6, 0x0DFF, 0xBDD6, 0xB1DE, 0x5491,
        0x5060, 0x0302, 0xA9CE, 0x7D56, 0x19E7, 0x62B5, 0xE64D, 0x9AEC,
        0x458F, 0x9D1F, 0x4089, 0x87FA, 0x15EF, 0xEBB2, 0xC98E, 0x0BFB,
        0xEC41, 0x67B3, 0xFD5F, 0xEA45, 0xBF23, 0xF753, 0x96E4, 0x5B9B,
        0xC275, 0x1CE1, 0xAE3D, 0x6A4C, 0x5A6C, 0x417E, 0x02F5, 0x4F83,
        0x5C68, 0xF451, 0x34D1, 0x08F9, 0x93E2, 0x73AB, 0x5362, 0x3F2A,
        0x0C08, 0x5295, 0x6546, 0x5E9D, 0x2830, 0xA137, 0x0F0A, 0xB52F,
        0x090E, 0x3624, 0x9B1B, 0x3DDF, 0x26CD, 0x694E, 0xCD7F, 0x9FEA,
        0x1B12, 0x9E1D, 0x7458, 0x2E34, 0x2D36, 0xB2DC, 0xEEB4, 0xFB5B,
        0xF6A4, 0x4D76, 0x61B7, 0xCE7D, 0x7B52, 0x3EDD, 0x715E, 0x9713,
        0xF5A6, 0x68B9, 0x0000, 0x2CC1, 0x6040, 0x1FE3, 0xC879, 0xEDB6,
        0xBED4, 0x468D, 0xD967, 0x4B72, 0xDE94, 0xD498, 0xE8B0, 0x4A85,
        0x6BBB, 0x2AC5, 0xE54F, 0x16ED, 0xC586, 0xD79A, 0x5566, 0x9411,
        0xCF8A, 0x10E9, 0x0604, 0x81FE, 0xF0A0, 0x4478, 0xBA25, 0xE34B,
        0xF3A2, 0xFE5D, 0xC080, 0x8A05, 0xAD3F, 0xBC21, 0x4870, 0x04F1,
        0xDF63, 0xC177, 0x75AF, 0x6342, 0x3020, 0x1AE5, 0x0EFD, 0x6DBF,
        0x4C81, 0x1418, 0x3526, 0x2FC3, 0xE1BE, 0xA235, 0xCC88, 0x392E,
        0x5793, 0xF255, 0x82FC, 0x477A, 0xACC8, 0xE7BA, 0x2B32, 0x95E6,
        0xA0C0, 0x9819, 0xD19E, 0x7FA3, 0x6644, 0x7E54, 0xAB3B, 0x830B,
        0xCA8C, 0x29C7, 0xD36B, 0x3C28, 0x79A7, 0xE2BC, 0x1D16, 0x76AD,
        0x3BDB, 0x5664, 0x4E74, 0x1E14, 0xDB92, 0x0A0C, 0x6C48, 0xE4B8,
        0x5D9F, 0x6EBD, 0xEF43, 0xA6C4, 0xA839, 0xA431, 0x37D3, 0x8BF2,
        0x32D5, 0x438B, 0x596E, 0xB7DA, 0x8C01, 0x64B1, 0xD29C, 0xE049,
        0xB4D8, 0xFAAC, 0x07F3, 0x25CF, 0xAFCA, 0x8EF4, 0xE947, 0x1810,
        0xD56F, 0x88F0, 0x6F4A, 0x725C, 0x2438, 0xF157, 0xC773, 0x5197,
        0x23CB, 0x7CA1, 0x9CE8, 0x213E, 0xDD96, 0xDC61, 0x860D, 0x850F,
        0x90E0, 0x427C, 0xC471, 0xAACC, 0xD890, 0x0506, 0x01F7, 0x121C,
        0xA3C2, 0x5F6A, 0xF9AE, 0xD069, 0x9117, 0x5899, 0x273A, 0xB927,
        0x38D9, 0x13EB, 0xB32B, 0x3322, 0xBBD2, 0x70A9, 0x8907, 0xA733,
        0xB62D, 0x223C, 0x9215, 0x20C9, 0x4987, 0xFFAA, 0x7850, 0x7AA5,
        0x8F03, 0xF859, 0x8009, 0x171A, 0xDA65, 0x31D7, 0xC684, 0xB8D0,
        0xC382, 0xB029, 0x775A, 0x111E, 0xCB7B, 0xFCA8, 0xD66D, 0x3A2C,
    }
};


extern wf_u32 get_crc32(wf_u8 *buf, int len);
extern void arc4_init (struct arc4context *parc4ctx, wf_u8 *key, wf_u32 key_len);
extern void arc4_encrypt (struct arc4context *parc4ctx, wf_u8 *dest, wf_u8 *src, wf_u32 len);

static void phase1 (wf_u16 *p1k, const wf_u8 *tk, const wf_u8 *ta, wf_u32 iv32)
{
    int i;
    p1k[0] = Lo16(iv32);
    p1k[1] = Hi16(iv32);
    p1k[2] = Mk16(ta[1], ta[0]);
    p1k[3] = Mk16(ta[3], ta[2]);
    p1k[4] = Mk16(ta[5], ta[4]);

    for (i = 0; i < PHASE1_LOOP_CNT; i++)
    {
        p1k[0] += _S_(p1k[4] ^ TK16((i & 1) + 0));
        p1k[1] += _S_(p1k[0] ^ TK16((i & 1) + 2));
        p1k[2] += _S_(p1k[1] ^ TK16((i & 1) + 4));
        p1k[3] += _S_(p1k[2] ^ TK16((i & 1) + 6));
        p1k[4] += _S_(p1k[3] ^ TK16((i & 1) + 0));
        p1k[4] +=  (unsigned short)i;
    }
}

static void phase2 (wf_u8 *rc4key, const wf_u8 *tk, const wf_u16 *p1k, wf_u16 iv16)
{
    int  i;
    wf_u16 PPK[6];
    for (i = 0; i < 5; i++)
        PPK[i] = p1k[i];
    PPK[5]  =  p1k[4] + iv16;

    PPK[0] +=    _S_(PPK[5] ^ TK16(0));
    PPK[1] +=    _S_(PPK[0] ^ TK16(1));
    PPK[2] +=    _S_(PPK[1] ^ TK16(2));
    PPK[3] +=    _S_(PPK[2] ^ TK16(3));
    PPK[4] +=    _S_(PPK[3] ^ TK16(4));
    PPK[5] +=    _S_(PPK[4] ^ TK16(5));

    PPK[0] +=  RotR1(PPK[5] ^ TK16(6));
    PPK[1] +=  RotR1(PPK[0] ^ TK16(7));
    PPK[2] +=  RotR1(PPK[1]);
    PPK[3] +=  RotR1(PPK[2]);
    PPK[4] +=  RotR1(PPK[3]);
    PPK[5] +=  RotR1(PPK[4]);
    rc4key[0] = Hi8(iv16);
    rc4key[1] = (Hi8(iv16) | 0x20) & 0x7F;
    rc4key[2] = Lo8(iv16);
    rc4key[3] = Lo8((PPK[5] ^ TK16(0)) >> 1);

    for (i = 0; i < 6; i++)
    {
        rc4key[4 + 2 * i] = Lo8(PPK[i]);
        rc4key[5 + 2 * i] = Hi8(PPK[i]);
    }
}

int tkip_encrypt (struct xmit_frame *pxmitframe, wf_u8 *pdata, wf_u32 len)
{
    nic_info_st *pnic_info = pxmitframe->nic_info;
    sec_info_st *psec_info = (sec_info_st *)pnic_info->sec_info;
    wf_u8 *pframe;
    wf_u32 frame_len;
    wf_u8 *iv;
    wf_u32 iv_ofs;
    wf_u8 *payload;
    wf_u32 payload_ofs;
    wf_u32 payload_len;
    wf_u32 icv_ofs;
    wf_u8 *prwskey;
    wf_u16 pnl;
    wf_u32 pnh;
    wf_u8 ttakey[16];
    wf_u8 rc4key[16];
    wf_u8 crc[4];
    struct arc4context acr4_context;
    union pn48 dot11txpn;
    wf_u32  res = 0;

    pframe      = pdata + TXDESC_SIZE + PACKET_OFFSET_SZ;
    frame_len   = len - TXDESC_SIZE + PACKET_OFFSET_SZ;

    iv_ofs      = pxmitframe->hdrlen;
    payload_ofs = iv_ofs + pxmitframe->iv_len;
    payload_len = frame_len - (payload_ofs + pxmitframe->icv_len);
    icv_ofs     = payload_ofs + payload_len;

    if (pxmitframe->bmcast)
    {
        prwskey =
            psec_info->dot118021XGrpKey[psec_info->dot118021XGrpKeyid].skey;
    }
    else
    {
        prwskey = pxmitframe->dot118021x_UncstKey.skey;
    }
    iv = &pframe[iv_ofs];
    GET_TKIP_PN(iv, dot11txpn);
    pnl = (wf_u16)(dot11txpn.val);
    pnh = (wf_u32)(dot11txpn.val >> 16);
    phase1((wf_u16 *)&ttakey[0], prwskey, pxmitframe->pwlanhdr->addr2, pnh);
    phase2(&rc4key[0], prwskey, (wf_u16 *)&ttakey[0], pnl);

    payload = &pframe[payload_ofs];
    *((wf_u32 *)crc) = get_crc32(payload, payload_len);
    arc4_init(&acr4_context, rc4key, 16);
    arc4_encrypt(&acr4_context, payload, payload, payload_len);
    arc4_encrypt(&acr4_context, &pframe[icv_ofs], crc, pxmitframe->icv_len); /* icv */

    return res;
}


int tkip_decrypt (prx_pkt_t ppkt)
{
    nic_info_st *pnic_info = (nic_info_st *)ppkt->p_nic_info;
    sec_info_st *psec_info = pnic_info->sec_info;
    prx_pkt_info_t pkt_info = &ppkt->pkt_info;
    wdn_net_info_st *pwdn_info;
    wf_u8 *pframe;
    wf_u32 frame_len;
    wf_u8 key_id;
    wf_u8 *iv;
    wf_u32 iv_ofs;
    wf_u8 *payload;
    wf_u32 payload_ofs;
    wf_u32 payload_len;
    wf_u8 *icv;
    wf_u32 icv_ofs;
    wf_u32 icv_len;
    wf_u16 pnl;
    wf_u32 pnh;
    wf_u8 rc4key[16];
    wf_u8 ttakey[16];
    wf_u8 crc[4];
    struct arc4context acr4_context;
    wf_u8 *prwskey;
    union pn48 dot11txpn;
    int res = 0;

    pframe      = ppkt->pdata;
    frame_len   = ppkt->len;

    iv_ofs      = pkt_info->wlan_hdr_len;
    payload_ofs = iv_ofs + pkt_info->iv_len;
    payload_len = frame_len - (payload_ofs + pkt_info->icv_len);
    icv_ofs     = payload_ofs + payload_len;
    icv_len     = pkt_info->icv_len;

    if (IS_MCAST(pkt_info->rx_addr))
    {
        if (!psec_info->binstallGrpkey)
        {
            LOG_W("didn't install group key!!!!!!!!!!");
            res = -1;
            goto exit;
        }
        iv = &pframe[iv_ofs];
        key_id = ((iv[3] >> 6) & 0x3);
        prwskey = psec_info->dot118021XGrpKey[key_id].skey;
    }
    else
    {
        pwdn_info = wf_wdn_find_info(pnic_info, pkt_info->tx_addr);
        if (pwdn_info == NULL)
        {
            LOG_W("get wdn_info fail!!!!!!!!!!");
            res = -2;
            goto exit;
        }
        prwskey = &pwdn_info->dot118021x_UncstKey.skey[0];
    }
    iv = &pframe[iv_ofs];
    GET_TKIP_PN(iv, dot11txpn);
    pnl = (wf_u16)(dot11txpn.val);
    pnh = (wf_u32)(dot11txpn.val >> 16);
    phase1((wf_u16 *)&ttakey[0], prwskey, pkt_info->tx_addr, pnh);
    phase2(&rc4key[0], prwskey, (wf_u16 *)&ttakey[0], pnl);

    payload = &pframe[payload_ofs];
    arc4_init(&acr4_context, rc4key, 16);
    arc4_encrypt(&acr4_context, payload, payload, payload_len + pkt_info->icv_len);
    
    *((wf_u32 *)crc) = get_crc32(payload, payload_len);
    icv = &pframe[icv_ofs];
    if (wf_memcmp(crc, icv, icv_len))
    {
        LOG_W("icv error crc");
        res = -3;
        goto exit;
    }

exit:
    return res;
}


