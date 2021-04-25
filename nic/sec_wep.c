#include "common.h"
#include "wf_debug.h"

static int bcrc32initialized = 0;
static wf_u32 crc32_table[256];


extern void arc4_init (struct arc4context *parc4ctx, wf_u8 *key, wf_u32 key_len);
extern void arc4_encrypt (struct arc4context *parc4ctx, wf_u8 *dest, wf_u8 *src, wf_u32 len);

static wf_inline wf_u8 crc32_reverseBit (wf_u8 data)
{
    return ((wf_u8) ((data << 7) & 0x80) | ((data << 5) & 0x40) |
            ((data << 3) & 0x20) | ((data << 1) & 0x10) | ((data >> 1) & 0x08) |
            ((data >> 3) & 0x04) | ((data >> 5) & 0x02) | ((data >> 7) & 0x01));
}

static wf_inline void init_crc32 (void)
{
    if (bcrc32initialized == 1)
        return;
    else
    {
        wf_s32 i, j;
        wf_u32 c;
        wf_u8 *p = (wf_u8 *) & c, *p1;
        wf_u8 k;

        c = 0x12340000;

        for (i = 0; i < 256; ++i)
        {
            k = crc32_reverseBit((wf_u8) i);
            for (c = ((wf_u32) k) << 24, j = 8; j > 0; --j)
            {
                c = c & 0x80000000 ? (c << 1) ^ (0x04c11db7) : (c << 1);
            }
            p1 = (wf_u8 *) & crc32_table[i];

            p1[0] = crc32_reverseBit(p[3]);
            p1[1] = crc32_reverseBit(p[2]);
            p1[2] = crc32_reverseBit(p[1]);
            p1[3] = crc32_reverseBit(p[0]);
        }
        bcrc32initialized = 1;
    }
}

wf_u32 get_crc32(wf_u8 *buf, int len)
{
    wf_u8 *p;
    wf_u32 crc;

    if (bcrc32initialized == 0)
        init_crc32();

    crc = 0xffffffff;

    for (p = buf; len > 0; ++p, --len)
    {
        crc = crc32_table[(crc ^ *p) & 0xff] ^ (crc >> 8);
    }
    return ~crc;
}

int wf_wep_encrypt_auth (nic_info_st *pnic_info,
                         wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    sec_info_st *psec_info = (sec_info_st *)pnic_info->sec_info;
    wf_u8 *pframe = (void *)pmgmt;
    wf_u32 iv_ofs;
    wf_u32 payload_ofs;
    wf_u8 icv_len;
    wf_u32 payload_len;
    wf_u32 icv_ofs;
    wf_u8 *piv;
    wf_u32 *picv;
    wf_u8 *payload;
    wf_u8 key_index;
    wf_u32 keylength;
    wf_u8 wepkey[16];
    struct arc4context arc4_context;

    /* WLAN HEAD | IV | MSDU | ICV
       24B         4B   N      4B */
    iv_ofs = WF_OFFSETOF(wf_80211_mgmt_t, auth_seq3.iv);
    payload_ofs = WF_OFFSETOF(wf_80211_mgmt_t, auth_seq3.auth_alg);
    icv_len = WF_FIELD_SIZEOF(wf_80211_mgmt_t, auth_seq3.icv);
    payload_len = mgmt_len - payload_ofs - icv_len;
    icv_ofs = payload_ofs + payload_len;
    piv = &pframe[iv_ofs];
    picv = (void *)&pframe[icv_ofs];
    payload = &pframe[payload_ofs];

    /* create key */
    key_index = ((piv[3] >> 6) & 0x3); /* Pad[0:5]KeyID[6:7] */
    keylength = psec_info->dot11DefKeylen[key_index];
    memcpy(&wepkey[0], piv, 3); /* IV0~2 */
    memcpy(&wepkey[3], &psec_info->dot11DefKey[key_index].skey[0], keylength); /* share key */

    /* calculater icv */
    picv[0] = wf_cpu_to_le32(get_crc32(payload, payload_len));

    /* encrypt MSDU+ICV use key */
    arc4_init(&arc4_context, wepkey, 3 + keylength); /* 3: IV0~2 */
    arc4_encrypt(&arc4_context, payload, payload, payload_len + icv_len);

    return 0;
}

int wf_wep_decrypt_auth (nic_info_st *pnic_info,
                         wf_80211_mgmt_t *pmgmt, wf_u16 mgmt_len)
{
    sec_info_st *psec_info = (sec_info_st *)pnic_info->sec_info;
    wf_u8 *pframe = (void *)pmgmt;
    wf_u32 iv_ofs;
    wf_u32 payload_ofs;
    wf_u8 icv_len;
    wf_u32 payload_len;
    wf_u32 icv_ofs;
    wf_u8 *piv;
    wf_u8 *payload;
    wf_u8 icv[4];
    wf_u8 key_index;
    wf_u32 keylength;
    wf_u8 wepkey[16];
    struct arc4context arc4_context;

    /* WLAN HEAD | IV | MSDU | ICV
       24B         4B   N      4B */
    iv_ofs = WF_OFFSETOF(wf_80211_mgmt_t, auth_seq3.iv);
    payload_ofs = WF_OFFSETOF(wf_80211_mgmt_t, auth_seq3.auth_alg);
    icv_len = WF_FIELD_SIZEOF(wf_80211_mgmt_t, auth_seq3.icv);
    payload_len = mgmt_len - payload_ofs - icv_len;
    icv_ofs = payload_ofs + payload_len;
    piv = &pframe[iv_ofs];
    payload = &pframe[payload_ofs];

    /* create key */
    key_index = ((piv[3] >> 6) & 0x3); /* Pad[0:5]KeyID[6:7] */
    keylength = psec_info->dot11DefKeylen[key_index];
    memcpy(&wepkey[0], piv, 3); /* IV0~2 */
    memcpy(&wepkey[3], &psec_info->dot11DefKey[key_index].skey[0], keylength); /* share key */

    /* decrypt MSDU+ICV use key */
    payload = &pframe[payload_ofs];
    arc4_init(&arc4_context, wepkey, 3 + keylength); /* 3: IV0~2 */
    arc4_encrypt(&arc4_context, payload, payload, payload_len + icv_len);

    /* calculate crc use MSDU field */
    *((wf_u32 *)icv) = le32_to_cpu(get_crc32(payload, payload_len));
    /* complare crc */
    if (wf_memcmp(icv, &pframe[icv_ofs], icv_len))
    {
        LOG_W("icv mismatching");
        return -1;
    }

    return 0;
}

int wep_encrypt (struct xmit_frame *pxmitframe, wf_u8 *pdata, wf_u32 len)
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
    wf_u8 wepkey[16];
    wf_u32 keylength;
    wf_u32 key_index;
    wf_u8 crc[4];
    struct arc4context arc4_context;
    wf_u32  res = 0;

    if (pxmitframe->encrypt_algo != _WEP40_ && pxmitframe->encrypt_algo != _WEP104_)
    {
        LOG_W("not wep encrypt!!!!!!!!!!!");
        res = -1;
        goto exit;
    }

    /* get frame point(skip TXD feld) */
    pframe      = pdata + TXDESC_SIZE + PACKET_OFFSET_SZ;
    frame_len   = len - (TXDESC_SIZE + PACKET_OFFSET_SZ);

    /* WLAN HEAD | IV | MSDU | ICV
       24B         4B   N      4B */
    iv_ofs      = pxmitframe->hdrlen;
    payload_ofs = iv_ofs + pxmitframe->iv_len;
    payload_len = frame_len - (payload_ofs + pxmitframe->icv_len);
    icv_ofs     = payload_ofs + payload_len;

    /* generate keystream */
    iv = &pframe[iv_ofs];
    key_index = ((iv[3] >> 6) & 0x3); /* Pad[0:5]KeyID[6:7] */
    keylength = psec_info->dot11DefKeylen[key_index];
    wf_memcpy(&wepkey[0], iv, 3); /* 3: IV0~2 */
    wf_memcpy(&wepkey[3], &psec_info->dot11DefKey[key_index].skey[0], keylength);

    /* calculate icv code */
    payload = &pframe[payload_ofs];
    *((wf_u32 *)crc) = cpu_to_le32(get_crc32(payload, payload_len));
    /* encrypt data */
    arc4_init(&arc4_context, wepkey, 3 + keylength); /* 3: IV0~2 */
    arc4_encrypt(&arc4_context, payload, payload, payload_len); /* msdu fild */
    arc4_encrypt(&arc4_context, &pframe[icv_ofs], crc, pxmitframe->icv_len); /* icv filed */

exit:
    return res;
}

int wep_decrypt (prx_pkt_t prx_pkt)
{
    nic_info_st *pnic_info = (nic_info_st *)prx_pkt->p_nic_info;
    sec_info_st *psec_info = (sec_info_st *)pnic_info->sec_info;
    prx_pkt_info_t pkt_info = &prx_pkt->pkt_info;
    wf_u8 *pframe = prx_pkt->pdata;
    wf_u8 wepkey[16];
    wf_u8 *iv;
    wf_u32 iv_ofs;
    wf_u8 *payload;
    wf_u32 payload_ofs;
    wf_u32 payload_len;
    wf_u32 icv_ofs;
    wf_u8 crc[4];
    wf_u8 key_index;
    wf_u32 keylength;
    struct arc4context arc4_context;
    int ret = 0;

    /* WLAN HEAD | IV | MSDU | ICV
       24B         4B   N      4B */
    iv_ofs = pkt_info->wlan_hdr_len;
    payload_ofs = iv_ofs + pkt_info->iv_len;
    payload_len = prx_pkt->len - (payload_ofs + pkt_info->icv_len);
    icv_ofs = payload_ofs + payload_len;

    /* create key */
    iv = &pframe[iv_ofs];
    key_index = ((iv[3] >> 6) & 0x3); /* Pad[0:5]KeyID[6:7] */
    keylength = psec_info->dot11DefKeylen[key_index];
    memcpy(&wepkey[0], iv, 3); /* IV0~2 */
    memcpy(&wepkey[3], &psec_info->dot11DefKey[key_index].skey[0], keylength); /* share key */

    /* decrypt MSDU+ICV use key */
    payload = &pframe[payload_ofs];
    arc4_init(&arc4_context, wepkey, 3 + keylength); /* 3: IV0~2 */
    arc4_encrypt(&arc4_context, payload, payload, payload_len + pkt_info->icv_len);

    /* calculate crc use MSDU field */
    *((wf_u32 *)crc) = le32_to_cpu(get_crc32(payload, payload_len));
    /* complare crc */
    if (wf_memcmp(crc, &pframe[icv_ofs], pkt_info->icv_len))
    {
        LOG_W("icv mismatching");
        ret = -1;
    }

exit:
    return ret;
}

