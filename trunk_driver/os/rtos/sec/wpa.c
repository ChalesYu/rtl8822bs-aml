/*
* WPA Supplicant - WPA state machine and EAPOL-Key processing
* Copyright (c) 2003-2015, Jouni Malinen <j@w1.fi>
* Copyright(c) 2015 Intel Deutschland GmbH
*
* This software may be distributed under the terms of the BSD license.
* See README for more details.
*/
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL  (~WF_DEBUG_MASK)
#include "common.h"
#include "trx/tx_rtos.h"

#include "sec/utils/common.h"
#include "crypto/aes.h"
#include "crypto/random.h"
#include "sec/wpa.h"
#include "crypto/sha1.h"
#include "crypto/rc4.h"
#include "common/wpa_common.h"
#include "utils/os.h"

static const wf_u8 null_rsc[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

extern struct wpa_supplicant wpa_s_obj;
extern struct wpa_config wpaconfig;

/*frame send interface songqiang add*/
static int wf_wpa_sm_ether_send(struct wpa_sm *sm, const wf_u8 *dest,wf_u16 proto, wf_u8 *buf, size_t len)
{
#if 0
    wf_u16 pkt_len;
    wf_u8 *buffer;
    nic_info_st *pnic_info = sm->pnic_info;
    wlan_dev_t *wlan = pnic_info->ndev;
    wlan_priv_st *wlan_priv = wlan->user_data;
    struct pbuf *p;

    pkt_len = sizeof(struct wf_ethhdr) + len;
    p = pbuf_alloc(PBUF_RAW, pkt_len, PBUF_POOL);
    if (NULL == p)
    {
        LOG_W("no memory for ethernet packet send");
        return -1;
    }

    buffer = p->payload;
    wf_memcpy(buffer, dest, MAC_ADDR_LEN);
    buffer += MAC_ADDR_LEN;
    wf_memcpy(buffer, wlan_priv->hw_addr, MAC_ADDR_LEN);
    buffer += MAC_ADDR_LEN;
    *buffer++ = (proto >> 8) & 0xFF;
    *buffer++ = proto & 0xFF;
    wf_memcpy(buffer, buf, len);

    rt_wlan_prot_transfer_dev(wlan,p,pkt_len);
    pbuf_free(p);
#endif
    nic_info_st *nic_info = sm->pnic_info;
    wf_u8 * pframe;
    struct wl_ieee80211_hdr * pwlanhdr = NULL;
    wf_u8 *pos;
    wdn_net_info_st *wdn_info;
    wf_u8 *buffer;
    wf_u32 pkt_len;

    wdn_info = wf_wdn_find_info(nic_info,wf_wlan_get_cur_bssid(nic_info));
    if (wdn_info == NULL)
    {
        return -1;
    }
    LOG_D ("wf_wpa_sm_ether_send:%d",wdn_info->qos_option);
    wdn_info->qos_option = 1;
    pkt_len = sizeof(struct wf_ethhdr) + len;
    buffer = wf_kzalloc (pkt_len);
    if (buffer == NULL)
    {
        return -1;
    }
    pos = buffer;
    wf_memcpy(buffer, dest, MAC_ADDR_LEN);
    buffer += MAC_ADDR_LEN;
    wf_memcpy(buffer, nic_to_local_addr(nic_info), MAC_ADDR_LEN);
    buffer += MAC_ADDR_LEN;
    *buffer++ = (proto >> 8) & 0xFF;
    *buffer++ = proto & 0xFF;
    wf_memcpy(buffer, buf, len);

    wf_tx_msdu(nic_info, pos, pkt_len, pos);
    tx_work_wake(nic_info);

    return 0;
}


enum wpa_states wf_wpa_sm_get_state()
{
    return wpa_s_obj.wpa_state;
}

void wf_wpa_sm_set_state(struct wpa_sm *sm, enum wpa_states state)
{
    wpa_s_obj.wpa_state = state;
    if (wpa_s_obj.last_eapol_matches_bssid && WPA_DISCONNECTED == state)
    {
        wpa_s_obj.last_eapol_matches_bssid = 0;
    }
}


void wf_eap_notify_success(struct eap_sm *sm)
{
    if (sm)
    {
        sm->decision = DECISION_COND_SUCC;
        sm->EAP_state = EAP_SUCCESS;
    }
}



const wf_u8 *wf_eap_get_eapkeydata(struct eap_sm *sm, size_t * len)
{
    if (sm == NULL || sm->eapKeyData == NULL)
    {
        *len = 0;
        return NULL;
    }

    *len = sm->eapKeyDataLen;
    return sm->eapKeyData;
}


int wf_eapol_sm_get_key(struct eapol_sm *sm, wf_u8 * key, size_t len)
{
    const wf_u8 *eap_key;
    size_t eap_len;

    if (sm == NULL )
    {
        wpa_printf(MSG_DEBUG, "EAPOL: EAP key not available");
        return -1;
    }
    eap_key = wf_eap_get_eapkeydata(sm->eap, &eap_len);
    if (eap_key == NULL)
    {
        wpa_printf(MSG_DEBUG, "EAPOL: Failed to get eapKeyData");
        return -1;
    }
    if (len > eap_len)
    {
        wpa_printf(MSG_DEBUG, "EAPOL: Requested key length (%lu) not "
                   "available (len=%lu)",
                   (unsigned long)len, (unsigned long)eap_len);
        return eap_len;
    }
    wf_memcpy(key, eap_key, len);
    wpa_printf(MSG_DEBUG, "EAPOL: Successfully fetched key (len=%lu)",
               (unsigned long)len);
    return 0;
}


void wf_eapol_sm_notify_cached(struct eapol_sm *sm)
{
    if (sm == NULL)
        return;
    wpa_printf(MSG_DEBUG, "EAPOL: PMKSA caching was used - skip EAPOL");
    sm->eapSuccess = TRUE;
    wf_eap_notify_success(sm->eap);
}


void wf_eapol_sm_notify_tx_eapol_key(struct eapol_sm *sm)
{
    if (sm)
        sm->dot1xSuppEapolFramesTx++;
}
/**
* wf_wpa_eapol_key_send - Send WPA/RSN EAPOL-Key message
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @kck: Key Confirmation Key (KCK, part of PTK)
* @kck_len: KCK length in octets
* @ver: Version field from Key Info
* @dest: Destination address for the frame
* @proto: Ethertype (usually ETH_P_EAPOL)
* @msg: EAPOL-Key message
* @msg_len: Length of message
* @key_mic: Pointer to the buffer to which the EAPOL-Key MIC is written
* Returns: >= 0 on success, < 0 on failure
*/
int wf_wpa_eapol_key_send(struct wpa_sm *sm, const wf_u8 *kck, size_t kck_len,
                          int ver, const wf_u8 *dest, wf_u16 proto,
                          wf_u8 *msg, size_t msg_len, wf_u8 *key_mic)
{
    int ret = -1;
    size_t mic_len = wf_wpa_mic_len(sm->key_mgmt);

    int i=0;
    if (is_zero_ether_addr(dest) && is_zero_ether_addr(sm->bssid))
    {
        /*
        * Association event was not yet received; try to fetch
        * BSSID from the driver.
        */

        dest = sm->bssid;


    }

    if (key_mic &&
        wf_wpa_eapol_key_mic(kck, kck_len, 2, ver, msg, msg_len,
                             key_mic))
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_ERROR,
                "WPA: Failed to generate EAPOL-Key version %d key_mgmt 0x%x MIC",
                ver, sm->key_mgmt);
        goto out;
    }

    ret = wf_wpa_sm_ether_send(sm, dest, proto, msg, msg_len);
    wf_eapol_sm_notify_tx_eapol_key(sm->eapol);
out:
    wf_free(msg);
    return ret;
}

#define ALLOC_MAGIC 0xa84ef1b2
#define FREED_MAGIC 0x67fd487a

struct wpa_ssid *wf_wpa_config_add_network(struct wpa_config *config)
{
    int id;
    struct wpa_ssid *ssid, *last = NULL;

    id = -1;
    ssid = config->ssid;
    while (ssid)
    {
        if (ssid->id > id)
            id = ssid->id;
        last = ssid;
        ssid = ssid->next;
    }
    id++;

    ssid = wf_kzalloc(sizeof(*ssid));
    if (ssid == NULL)
        return NULL;

    return ssid;
}


void wf_wpa_config_set_network_defaults(struct wpa_ssid *ssid)
{
    ssid->proto = DEFAULT_PROTO;
    ssid->pairwise_cipher = DEFAULT_PAIRWISE;
    ssid->group_cipher = DEFAULT_GROUP;
    ssid->key_mgmt = 2;
    ssid->bg_scan_period = DEFAULT_BG_SCAN_PERIOD;
    ssid->proactive_key_caching = -1;
    ssid->mac_addr = -1;
}



int wf_wpa_supplicant_ctrl_iface_add_network(struct wpa_supplicant *wpa_s)
{
    struct wpa_ssid *ssid;
    int ret = 0;

    ssid = wf_wpa_config_add_network(wpa_s->conf);
    if (ssid == NULL)
        return -1;

    ssid->disabled = 1;
    wf_wpa_config_set_network_defaults(ssid);
    wpa_s->wpa->proto = DEFAULT_PROTO;
    return ret;
}


static wf_u8 * wf_wpa_sm_alloc_eapol(const struct wpa_supplicant *wpa_s, wf_u8 type,
                                     const void *data, wf_u16 data_len,
                                     size_t *msg_len, void **data_pos)
{
    struct ieee802_1x_hdr *hdr;

    *msg_len = sizeof(*hdr) + data_len;
    hdr = wf_kzalloc(*msg_len);
    if (hdr == NULL)
        return NULL;
    hdr->version = 1;
    hdr->type = type;
    hdr->length = host_to_be16(data_len);

    if (data)
        wf_memcpy(hdr + 1, data, data_len);
    else
        wf_memset(hdr + 1, 0, data_len);


    if (data_pos)
        *data_pos = hdr + 1;

    return (wf_u8 *) hdr;
}



/**
* wf_wpa_sm_key_request - Send EAPOL-Key Request
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @error: Indicate whether this is an Michael MIC error report
* @pairwise: 1 = error report for pairwise packet, 0 = for group packet
*
* Send an EAPOL-Key Request to the current authenticator. This function is
* used to request rekeying and it is usually called when a local Michael MIC
* failure is detected.
*/
void wf_wpa_sm_key_request(struct wpa_sm *sm, int error, int pairwise)
{
    size_t mic_len, hdrlen, rlen;
    struct wpa_eapol_key *reply;
    struct wpa_eapol_key_192 *reply192;
    int key_info, ver;
    wf_u8 bssid[ETH_ALEN], *rbuf, *key_mic;

    if (sm->key_mgmt == WPA_KEY_MGMT_OSEN ||
        wpa_key_mgmt_suite_b(sm->key_mgmt))
        ver = WPA_KEY_INFO_TYPE_AKM_DEFINED;
    else if (wpa_key_mgmt_ft(sm->key_mgmt) ||
             wpa_key_mgmt_sha256(sm->key_mgmt))
        ver = WPA_KEY_INFO_TYPE_AES_128_CMAC;
    else if (sm->pairwise_cipher != WPA_CIPHER_TKIP)
        ver = WPA_KEY_INFO_TYPE_HMAC_SHA1_AES;
    else
        ver = WPA_KEY_INFO_TYPE_HMAC_MD5_RC4;

    mic_len = wf_wpa_mic_len(sm->key_mgmt);
    hdrlen = mic_len == 24 ? sizeof(*reply192) : sizeof(*reply);
    rbuf = wf_wpa_sm_alloc_eapol((struct wpa_supplicant*)sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                                 hdrlen, &rlen, (void *) &reply);
    if (rbuf == NULL)
        return;
    reply192 = (struct wpa_eapol_key_192 *) reply;

    reply->type = (sm->proto == WPA_PROTO_RSN ||
                   sm->proto == WPA_PROTO_OSEN) ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info = WPA_KEY_INFO_REQUEST | ver;
    if (sm->ptk_set)
        key_info |= WPA_KEY_INFO_MIC | WPA_KEY_INFO_SECURE;
    if (error)
        key_info |= WPA_KEY_INFO_ERROR;
    if (pairwise)
        key_info |= WPA_KEY_INFO_KEY_TYPE;
    WPA_PUT_BE16(reply->key_info, key_info);
    WPA_PUT_BE16(reply->key_length, 0);
    wf_memcpy(reply->replay_counter, sm->request_counter,
              WPA_REPLAY_COUNTER_LEN);
    wf_os_inc_byte_array(sm->request_counter, WPA_REPLAY_COUNTER_LEN);

    if (mic_len == 24)
        WPA_PUT_BE16(reply192->key_data_length, 0);
    else
        WPA_PUT_BE16(reply->key_data_length, 0);
    if (!(key_info & WPA_KEY_INFO_MIC))
        key_mic = NULL;
    else
        key_mic = reply192->key_mic; /* same offset in reply */

    wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
            "WPA: Sending EAPOL-Key Request (error=%d "
            "pairwise=%d ptk_set=%d len=%lu)",
            error, pairwise, sm->ptk_set, (unsigned long) rlen);
    wf_wpa_eapol_key_send(sm, sm->ptk.kck, sm->ptk.kck_len, ver, bssid,
                          ETH_P_EAPOL, rbuf, rlen, key_mic);
}

int wf_wpa_supplicant_send_2_of_4(struct wpa_sm *sm, const unsigned char *dst,
                                  const struct wpa_eapol_key *key,
                                  int ver, const wf_u8 *nonce,
                                  const wf_u8 *wpa_ie, size_t wpa_ie_len,
                                  struct wpa_ptk *ptk)
{
    size_t mic_len, hdrlen, rlen;
    struct wpa_eapol_key *reply;
    struct wpa_eapol_key_192 *reply192;
    wf_u8 *rbuf, *key_mic;
    wf_u8 *rsn_ie_buf = NULL;
    nic_info_st *pnic_info = (nic_info_st *)sm->pnic_info;
    sec_info_st *sec_info = pnic_info->sec_info;
    int i=0;
    if (wpa_ie == NULL)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING, "WPA: No wpa_ie set - "
                "cannot generate msg 2/4");
        return -1;
    }


    wpa_hexdump(MSG_DEBUG, "WPA: WPA IE for msg 2/4", wpa_ie, wpa_ie_len);

    mic_len = wf_wpa_mic_len(sm->key_mgmt);
    hdrlen = mic_len == 24 ? sizeof(*reply192) : sizeof(*reply);
    rbuf = wf_wpa_sm_alloc_eapol((struct wpa_supplicant *)sm, IEEE802_1X_TYPE_EAPOL_KEY,
                                 NULL, hdrlen + wpa_ie_len,
                                 &rlen, (void *) &reply);
    if (rbuf == NULL)
    {
        wf_free(rsn_ie_buf);
        return -1;
    }

    reply192 = (struct wpa_eapol_key_192 *) reply;
    if (sec_info->rsn_enable == 1)
    {
        reply->type = EAPOL_KEY_TYPE_RSN;
    }
    else if (sec_info->wpa_enable == 1)
    {
        reply->type = EAPOL_KEY_TYPE_WPA;
    }

    WPA_PUT_BE16(reply->key_info,
                 ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC);
    if (sm->proto == WPA_PROTO_RSN || sm->proto == WPA_PROTO_OSEN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        wf_memcpy(reply->key_length, key->key_length, 2);
    wf_memcpy(reply->replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);
    wpa_hexdump(MSG_DEBUG, "WPA: Replay Counter", reply->replay_counter,
                WPA_REPLAY_COUNTER_LEN);

    key_mic = reply192->key_mic;

    if (mic_len == 24)
    {
        WPA_PUT_BE16(reply192->key_data_length, wpa_ie_len);
        wf_memcpy(reply192 + 1, wpa_ie, wpa_ie_len);
    }
    else
    {
        WPA_PUT_BE16(reply->key_data_length, wpa_ie_len);
        wf_memcpy(reply + 1, wpa_ie, wpa_ie_len);
    }

    wf_free(rsn_ie_buf);

    wf_memcpy(reply->key_nonce, nonce, WPA_NONCE_LEN);


    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: Sending EAPOL-Key 2/4");
    return wf_wpa_eapol_key_send(sm, ptk->kck, ptk->kck_len, ver, dst,
                                 ETH_P_EAPOL, rbuf, rlen, key_mic);
}


static int wf_wpa_derive_ptk(struct wpa_sm *sm, const unsigned char *src_addr,
                             const struct wpa_eapol_key *key, struct wpa_ptk *ptk)
{


    return wf_wpa_pmk_to_ptk(sm->pmk, sm->pmk_len, "Pairwise key expansion",
                             sm->own_addr, src_addr, sm->snonce,
                             key->key_nonce, ptk, sm->key_mgmt,
                             sm->pairwise_cipher);
}

static int wf_wpa_parse_generic(const wf_u8 * pos, const wf_u8 * end,
                                struct wpa_eapol_ie_parse *ie)
{
    if (pos[1] == 0)
        return 1;

    if (pos[1] >= 6 &&
        RSN_SELECTOR_GET(pos + 2) == WPA_OUI_TYPE &&
        pos[2 + WPA_LEN_SELECTOR] == 1 && pos[2 + WPA_LEN_SELECTOR + 1] == 0)
    {
        ie->wpa_ie = pos;
        ie->wpa_ie_len = pos[1] + 2;
        wpa_hexdump(MSG_DEBUG, "WPA: WPA IE in EAPOL-Key",
                    ie->wpa_ie, ie->wpa_ie_len);
        return 0;
    }

    if (pos + 1 + _SELECTOR_LEN_TO_RSN < end &&
        pos[1] >= _SELECTOR_LEN_TO_RSN + TK_PMK_ID_LEN &&
        RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_PMKID)
    {
        ie->pmkid = pos + 2 + _SELECTOR_LEN_TO_RSN;
        wpa_hexdump(MSG_DEBUG, "WPA: PMKID in EAPOL-Key", pos, pos[1] + 2);
        return 0;
    }

    if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
        RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_GROUPKEY)
    {
        ie->gtk = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->gtk_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump_key(MSG_DEBUG, "WPA: GTK in EAPOL-Key", pos, pos[1] + 2);
        return 0;
    }

    if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
        RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_MAC_ADDR)
    {
        ie->mac_addr = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->mac_addr_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump(MSG_DEBUG, "WPA: MAC Address in EAPOL-Key",
                    pos, pos[1] + 2);
        return 0;
    }
#ifdef CONFIG_PEERKEY
    if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
        RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_SMK)
    {
        ie->smk = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->smk_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump_key(MSG_DEBUG, "WPA: SMK in EAPOL-Key", pos, pos[1] + 2);
        return 0;
    }

    if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
        RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_NONCE)
    {
        ie->nonce = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->nonce_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump(MSG_DEBUG, "WPA: Nonce in EAPOL-Key", pos, pos[1] + 2);
        return 0;
    }

    if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
        RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_LIFETIME)
    {
        ie->lifetime = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->lifetime_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump(MSG_DEBUG, "WPA: Lifetime in EAPOL-Key", pos, pos[1] + 2);
        return 0;
    }

    if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
        RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_ERROR)
    {
        ie->error = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->error_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump(MSG_DEBUG, "WPA: Error in EAPOL-Key", pos, pos[1] + 2);
        return 0;
    }
#endif

    return 0;
}


static int wf_wpa_parse_vendor_specific(const wf_u8 * pos, const wf_u8 * end,
                                        struct wpa_eapol_ie_parse *ie)
{
    unsigned int oui;

    if (pos[1] < 4)
    {
        wpa_printf(MSG_MSGDUMP, "Too short vendor specific IE ignored (len=%u)",
                   pos[1]);
        return 1;
    }

    oui = WPA_GET_BE24(&pos[2]);
    if (oui == OUI_MS && pos[5] == WMM_TYP_OUI && pos[1] > 4)
    {
        if (pos[6] == WMM_CHILDTYPE_INFORMATION_ELEMENT_OUI)
        {
            ie->wmm = &pos[2];
            ie->wmm_len = pos[1];
            wpa_hexdump(MSG_DEBUG, "WPA: WMM IE", ie->wmm, ie->wmm_len);
        }
        else if (pos[6] == WMM_CHILDTYPE_PARAMETER_ELEMENT_OUI)
        {
            ie->wmm = &pos[2];
            ie->wmm_len = pos[1];
            wpa_hexdump(MSG_DEBUG, "WPA: WMM Parameter Element",
                        ie->wmm, ie->wmm_len);
        }
    }
    return 0;
}



int wf_wpa_supplicant_parse_ies(const wf_u8 * buf, size_t len,
                                struct wpa_eapol_ie_parse *ie)
{
    const wf_u8 *pos, *end;
    int ret = 0;

    wf_memset(ie, 0, sizeof(*ie));
    for (pos = buf, end = pos + len; pos + 1 < end; pos += 2 + pos[1])
    {
        if (pos[0] == 0xdd && ((pos == buf + len - 1) || pos[1] == 0))
        {

            break;
        }
        if (pos + 2 + pos[1] > end)
        {
            wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key Key Data "
                       "underflow (ie=%d len=%d pos=%d)",
                       pos[0], pos[1], (int)(pos - buf));
            wpa_hexdump_key(MSG_DEBUG, "WPA: Key Data", buf, len);
            ret = -1;
            break;
        }
        if (*pos == WLAN_RSN_EID)
        {
            ie->rsn_ie = pos;
            ie->rsn_ie_len = pos[1] + 2;

        }
        else if (*pos == WLAN_MOBILITY_FIELD_EID &&
                 pos[1] >= sizeof(struct rsn_mdie))
        {
            ie->mdie = pos;
            ie->mdie_len = pos[1] + 2;

        }
        else if (*pos == WLAN_FAST_BSS_CHANGE_EID &&
                 pos[1] >= sizeof(struct rsn_ftie))
        {
            ie->ftie = pos;
            ie->ftie_len = pos[1] + 2;

        }
        else if (*pos == WLAN_OVERTIME_INTERVAL_EID && pos[1] >= 5)
        {
            if (pos[2] == WLAN_TIMEOUT_REASSOC_DEADLINE)
            {
                ie->reassoc_deadline = pos;
                wpa_hexdump(MSG_DEBUG, "WPA: Reassoc Deadline "
                            "in EAPOL-Key", ie->reassoc_deadline, pos[1] + 2);
            }
            else if (pos[2] == WLAN_TIMEOUT_KEY_LIFETIME)
            {
                ie->key_lifetime = pos;
                wpa_hexdump(MSG_DEBUG, "WPA: KeyLifetime "
                            "in EAPOL-Key", ie->key_lifetime, pos[1] + 2);
            }
            else
            {
                wpa_hexdump(MSG_DEBUG, "WPA: Unrecognized "
                            "EAPOL-Key Key Data IE", pos, 2 + pos[1]);
            }
        }
        else if (*pos == WLAN_EID_LINK_ID)
        {
            if (pos[1] >= 18)
            {
                ie->lnkid = pos;
                ie->lnkid_len = pos[1] + 2;
            }
        }
        else if (*pos == WLAN_EID_EXT_CAPAB)
        {
            ie->ext_capab = pos;
            ie->ext_capab_len = pos[1] + 2;
        }
        else if (*pos == WLAN_SUPP_RATES_EID)
        {
            ie->supp_rates = pos;
            ie->supp_rates_len = pos[1] + 2;
        }
        else if (*pos == WLAN_EXT_SUPP_RATES_EID)
        {
            ie->ext_supp_rates = pos;
            ie->ext_supp_rates_len = pos[1] + 2;
        }
        else if (*pos == WLAN_HT_CAP_EID &&
                 pos[1] >= sizeof(struct ieee80211_ht_capabilities))
        {
            ie->ht_capabilities = pos + 2;
        }
        else if (*pos == WLAN_EID_VHT_AID)
        {
            if (pos[1] >= 2)
                ie->aid = WPA_GET_LE16(pos + 2) & 0x3fff;
        }
        else if (*pos == WLAN_EID_VHT_CAP &&
                 pos[1] >= sizeof(struct ieee80211_vht_capabilities))
        {
            ie->vht_capabilities = pos + 2;
        }
        else if (*pos == WLAN_EID_QOS && pos[1] >= 1)
        {
            ie->qosinfo = pos[2];
        }
        else if (*pos == WLAN_SUSTAINS_CHANNELS_EID)
        {
            ie->supp_channels = pos + 2;
            ie->supp_channels_len = pos[1];
        }
        else if (*pos == WLAN_EID_SUPPORTED_OPERATING_CLASSES)
        {

            if (pos[1] >= 2 && pos[1] <= 253)
            {
                ie->supp_oper_classes = pos + 2;
                ie->supp_oper_classes_len = pos[1];
            }
        }
        else if (*pos == WLAN_VENDOR_SPECIFIC_EID)
        {
            ret = wf_wpa_parse_generic(pos, end, ie);
            if (ret < 0)
                break;
            if (ret > 0)
            {
                ret = 0;
                break;
            }

            ret = wf_wpa_parse_vendor_specific(pos, end, ie);
            if (ret < 0)
                break;
            if (ret > 0)
            {
                ret = 0;
                break;
            }
        }
        else
        {
            wpa_hexdump(MSG_DEBUG, "WPA: Unrecognized EAPOL-Key "
                        "Key Data IE", pos, 2 + pos[1]);
        }
    }

    return ret;
}


static void wf_wpa_supplicant_process_1_of_4(struct wpa_sm *sm,
        const unsigned char *src_addr,
        const struct wpa_eapol_key *key,
        wf_u16 ver, const wf_u8 *key_data,
        size_t key_data_len)
{
    struct wpa_eapol_ie_parse ie;
    struct wpa_ptk *ptk;
    int res;
    wf_u8 *kde, *kde_buf = NULL;
    size_t kde_len;
    nic_info_st *pnic_info = (nic_info_st *)sm->pnic_info;
    sec_info_st *sec_info = pnic_info->sec_info;
    wf_80211_mgmt_ie_t *pie = NULL;
    int i=0;
    wf_wpa_sm_set_state(sm, WPA_4WAY_HANDSHAKE);

    wf_memset(&ie, 0, sizeof(ie));

    if (sm->proto == WPA_PROTO_RSN || sm->proto == WPA_PROTO_OSEN)
    {
        /* RSN: msg 1/4 should contain PMKID for the selected PMK */
        wpa_hexdump(MSG_DEBUG, "RSN: msg 1/4 key data",
                    key_data, key_data_len);
        if (wf_wpa_supplicant_parse_ies(key_data, key_data_len, &ie) < 0)
            goto failed;
        if (ie.pmkid)
        {
            wpa_hexdump(MSG_DEBUG, "RSN: PMKID from "
                        "Authenticator", ie.pmkid, PMKID_LEN);
        }
    }

#if 0
    res = wpa_supplicant_get_pmk(sm, src_addr, ie.pmkid);
    if (res == -2)
    {
        wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "RSN: Do not reply to "
                "msg 1/4 - requesting full EAP authentication");
        return;
    }
    if (res)
        return;
#endif
    if (sm->renew_snonce)
    {
        if (wf_random_get_bytes(sm->snonce, WPA_NONCE_LEN))
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: Failed to get random data for SNonce");
            goto failed;
        }
        sm->renew_snonce = 0;
        wpa_hexdump(MSG_DEBUG, "WPA: Renewed SNonce",
                    sm->snonce, WPA_NONCE_LEN);
    }

    /* Calculate PTK which will be stored as a temporary PTK until it has
    * been verified when processing message 3/4. */
    ptk = &sm->tptk;
    wf_wpa_derive_ptk(sm, src_addr, key, ptk);
    if (sm->pairwise_cipher == WPA_CIPHER_TKIP)
    {
        wf_u8 buf[8];
        /* Supplicant: swap tx/rx Mic keys */
        wf_memcpy(buf, &ptk->tk[16], 8);
        wf_memcpy(&ptk->tk[16], &ptk->tk[24], 8);
        wf_memcpy(&ptk->tk[24], buf, 8);
        wf_memset(buf, 0, sizeof(buf));
    }
    sm->tptk_set = 1;
    pie = (wf_80211_mgmt_ie_t *)sec_info->supplicant_ie;

    sm->assoc_wpa_ie_len = pie->len +2;

    //memcpy(sm->assoc_wpa_ie,sec_info->supplicant_ie,sm->assoc_wpa_ie_len);
    sm->assoc_wpa_ie = sec_info->supplicant_ie;
    kde = sm->assoc_wpa_ie ;
    kde_len = sm->assoc_wpa_ie_len;

#ifdef CONFIG_P2P
    if (sm->p2p)
    {
        kde_buf = wf_kzalloc(kde_len + 2 + RSN_SELECTOR_LEN + 1);
        if (kde_buf)
        {
            wf_u8 *pos;
            wpa_printf(MSG_DEBUG, "P2P: Add IP Address Request KDE "
                       "into EAPOL-Key 2/4");
            wf_memcpy(kde_buf, kde, kde_len);
            kde = kde_buf;
            pos = kde + kde_len;
            *pos++ = WLAN_EID_VENDOR_SPECIFIC;
            *pos++ = RSN_SELECTOR_LEN + 1;
            RSN_SELECTOR_PUT(pos, WFA_KEY_DATA_IP_ADDR_REQ);
            pos += RSN_SELECTOR_LEN;
            *pos++ = 0x01;
            kde_len = pos - kde;
        }
    }
#endif /* CONFIG_P2P */

    if (wf_wpa_supplicant_send_2_of_4(sm, sm->bssid, key, ver, sm->snonce,
                                      kde, kde_len, ptk) < 0)
        goto failed;

    wf_free(kde_buf);
    wf_memcpy(sm->anonce, key->key_nonce, WPA_NONCE_LEN);
    return;

failed:
    wf_free(kde_buf);

}


static void wf_wpa_supplicant_key_neg_complete(struct wpa_sm *sm,
        const wf_u8 *addr, int secure)
{
    wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
            "WPA: Key negotiation completed with "
            MACSTR " [PTK=%s GTK=%s]", MAC2STR(addr),
            wf_wpa_cipher_txt(sm->pairwise_cipher),
            wf_wpa_cipher_txt(sm->group_cipher));
    wf_wpa_sm_set_state(sm, WPA_COMPLETED);


    if (sm->cur_pmksa && sm->cur_pmksa->opportunistic)
    {
        wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
                "RSN: Authenticator accepted "
                "opportunistic PMKSA entry - marking it valid");
        sm->cur_pmksa->opportunistic = 0;
    }

#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt))
    {
        /* Prepare for the next transition */
        wpa_ft_prepare_auth_request(sm, NULL);
    }
#endif /* CONFIG_IEEE80211R */
}


static void wf_wpa_sm_rekey_ptk(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_sm *sm = eloop_ctx;
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: Request PTK rekeying");
    wf_wpa_sm_key_request(sm, 0, 1);
}

static int wf_wpa_sm_set_key(struct wpa_sm *sm,enum wpa_alg alg,
                             const wf_u8 * addr, int key_idx, int set_tx,
                             const wf_u8 * seq, size_t seq_len,
                             const wf_u8 * key, size_t key_len)
{
    return wpa_s_obj.driver->set_key(sm->pnic_info,key_idx, set_tx, addr, (wf_u32) alg, seq,
                                     seq_len, key, key_len);
}

static int wf_wpa_supplicant_install_ptk(struct wpa_sm *sm,
        const struct wpa_eapol_key *key)
{
    int keylen, rsclen;
    enum wpa_alg alg;
    const wf_u8 *key_rsc;



    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "WPA: Installing PTK to the driver");

    if (sm->pairwise_cipher == WPA_CIPHER_NONE)
    {
        wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: Pairwise Cipher "
                "Suite: NONE - do not use pairwise keys");
        return 0;
    }

    if (!wf_wpa_cipher_valid_pairwise(sm->pairwise_cipher))
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: Unsupported pairwise cipher %d",
                sm->pairwise_cipher);
        return -1;
    }

    alg = wf_wpa_cipher_to_alg(sm->pairwise_cipher);
    keylen = wf_wpa_cipher_key_len(sm->pairwise_cipher);
    rsclen = wf_wpa_cipher_rsc_len(sm->pairwise_cipher);

    if (sm->proto == WPA_PROTO_RSN || sm->proto == WPA_PROTO_OSEN)
    {
        key_rsc = null_rsc;
    }
    else
    {
        key_rsc = key->key_rsc;
        wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, rsclen);
    }

    if (wf_wpa_sm_set_key(sm, alg, sm->bssid, 0, 1, key_rsc, rsclen,
                          sm->ptk.tk, keylen) < 0)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: Failed to set PTK to the "
                "driver (alg=%d keylen=%d bssid=" MACSTR ")",
                alg, keylen, MAC2STR(sm->bssid));
        return -1;
    }

    /* TK is not needed anymore in supplicant */
    wf_memset(sm->ptk.tk, 0, WPA_TK_MAX_LEN);


    return 0;
}


static int wf_wpa_supplicant_check_group_cipher(struct wpa_sm *sm,
        int group_cipher,
        int keylen, int maxkeylen,
        int *key_rsc_len,
        enum wpa_alg *alg)
{
    int klen;

    *alg = wf_wpa_cipher_to_alg(group_cipher);
    if (*alg == WPA_ALG_NONE)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: Unsupported Group Cipher %d",
                group_cipher);
        return -1;
    }
    *key_rsc_len = wf_wpa_cipher_rsc_len(group_cipher);

    klen = wf_wpa_cipher_key_len(group_cipher);
    if (keylen != klen || maxkeylen < klen)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: Unsupported %s Group Cipher key length %d (%d)",
                wf_wpa_cipher_txt(group_cipher), keylen, maxkeylen);
        return -1;
    }
    return 0;
}



static int wf_wpa_supplicant_install_gtk(struct wpa_sm *sm,
        const struct wpa_gtk_data *gd,
        const wf_u8 *key_rsc)
{
    const wf_u8 *_gtk = gd->gtk;
    wf_u8 gtk_buf[32];

    wpa_hexdump_key(MSG_DEBUG, "WPA: Group Key", gd->gtk, gd->gtk_len);
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "WPA: Installing GTK to the driver (keyidx=%d tx=%d len=%d)",
            gd->keyidx, gd->tx, gd->gtk_len);
    wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, gd->key_rsc_len);
    if (sm->group_cipher == WPA_CIPHER_TKIP)
    {
        /* Swap Tx/Rx keys for Michael MIC */
        wf_memcpy(gtk_buf, gd->gtk, 16);
        wf_memcpy(gtk_buf + 16, gd->gtk + 24, 8);
        wf_memcpy(gtk_buf + 24, gd->gtk + 16, 8);
        _gtk = gtk_buf;
    }
    if (sm->pairwise_cipher == WPA_CIPHER_NONE)
    {
        if (wf_wpa_sm_set_key(sm, gd->alg, NULL,
                              gd->keyidx, 1, key_rsc, gd->key_rsc_len,
                              _gtk, gd->gtk_len) < 0)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: Failed to set GTK to the driver "
                    "(Group only)");
            wf_memset(gtk_buf, 0, sizeof(gtk_buf));
            return -1;
        }
    }
    else if (wf_wpa_sm_set_key(sm, gd->alg, broadcast_ether_addr,
                               gd->keyidx, gd->tx, key_rsc, gd->key_rsc_len,
                               _gtk, gd->gtk_len) < 0)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: Failed to set GTK to "
                "the driver (alg=%d keylen=%d keyidx=%d)",
                gd->alg, gd->gtk_len, gd->keyidx);
        wf_memset(gtk_buf, 0, sizeof(gtk_buf));
        return -1;
    }
    wf_memset(gtk_buf, 0, sizeof(gtk_buf));

    return 0;
}


static int wf_wpa_supplicant_gtk_tx_bit_workaround(const struct wpa_sm *sm,
        int tx)
{
    if (tx && sm->pairwise_cipher != WPA_CIPHER_NONE)
    {
        /* Ignore Tx bit for GTK if a pairwise key is used. One AP
        * seemed to set this bit (incorrectly, since Tx is only when
        * doing Group Key only APs) and without this workaround, the
        * data connection does not work because wpa_supplicant
        * configured non-zero keyidx to be used for unicast. */
        wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                "WPA: Tx bit set for GTK, but pairwise "
                "keys are used - ignore Tx bit");
        return 0;
    }
    return tx;
}


static int wf_wpa_supplicant_rsc_relaxation(const struct wpa_sm *sm,
        const wf_u8 *rsc)
{
    int rsclen;


    rsclen = wf_wpa_cipher_rsc_len(sm->group_cipher);

    /*
    * Try to detect RSC (endian) corruption issue where the AP sends
    * the RSC bytes in EAPOL-Key message in the wrong order, both if
    * it's actually a 6-byte field (as it should be) and if it treats
    * it as an 8-byte field.
    * An AP model known to have this bug is the Sapido RB-1632.
    */
    if (rsclen == 6 && ((rsc[5] && !rsc[0]) || rsc[6] || rsc[7]))
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "RSC %02x%02x%02x%02x%02x%02x%02x%02x is likely bogus, using 0",
                rsc[0], rsc[1], rsc[2], rsc[3],
                rsc[4], rsc[5], rsc[6], rsc[7]);

        return 1;
    }

    return 0;
}


static int wf_wpa_supplicant_pairwise_gtk(struct wpa_sm *sm,
        const struct wpa_eapol_key *key,
        const wf_u8 *gtk, size_t gtk_len,
        int key_info)
{
    struct wpa_gtk_data gd;
    const wf_u8 *key_rsc;

    /*
    * IEEE Std 802.11i-2004 - 8.5.2 EAPOL-Key frames - Figure 43x
    * GTK KDE format:
    * KeyID[bits 0-1], Tx [bit 2], Reserved [bits 3-7]
    * Reserved [bits 0-7]
    * GTK
    */

    wf_memset(&gd, 0, sizeof(gd));
    wpa_hexdump_key(MSG_DEBUG, "RSN: received GTK in pairwise handshake",
                    gtk, gtk_len);

    if (gtk_len < 2 || gtk_len - 2 > sizeof(gd.gtk))
        return -1;

    gd.keyidx = gtk[0] & 0x3;
    gd.tx = wf_wpa_supplicant_gtk_tx_bit_workaround(sm,
            !!(gtk[0] & BIT(2)));
    gtk += 2;
    gtk_len -= 2;

    wf_memcpy(gd.gtk, gtk, gtk_len);
    gd.gtk_len = gtk_len;

    key_rsc = key->key_rsc;
    if (wf_wpa_supplicant_rsc_relaxation(sm, key->key_rsc))
        key_rsc = null_rsc;
    wf_wpa_supplicant_install_gtk(sm, &gd, key_rsc);
    if (sm->group_cipher != WPA_CIPHER_GTK_NOT_USED &&
        (wf_wpa_supplicant_check_group_cipher(sm, sm->group_cipher,
                gtk_len, gtk_len,
                &gd.key_rsc_len, &gd.alg) ||
         wf_wpa_supplicant_install_gtk(sm, &gd, key_rsc)))
    {
        wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
                "RSN: Failed to install GTK");
        wf_memset(&gd, 0, sizeof(gd));
        return -1;
    }
    wf_memset(&gd, 0, sizeof(gd));

    wf_wpa_supplicant_key_neg_complete(sm, sm->bssid,
                                       key_info & WPA_KEY_INFO_SECURE);
    return 0;
}


static int wf_ieee80211w_set_keys(struct wpa_sm *sm,
                                  struct wpa_eapol_ie_parse *ie)
{
#ifdef CONFIG_IEEE80211W
    if (!wpa_cipher_valid_mgmt_group(sm->mgmt_group_cipher))
        return 0;

    if (ie->igtk)
    {
        size_t len;
        const struct wpa_igtk_kde *igtk;
        wf_u16 keyidx;
        len = wf_wpa_cipher_key_len(sm->mgmt_group_cipher);
        if (ie->igtk_len != WPA_IGTK_KDE_PREFIX_LEN + len)
            return -1;
        igtk = (const struct wpa_igtk_kde *) ie->igtk;
        keyidx = WPA_GET_LE16(igtk->keyid);
        wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: IGTK keyid %d "
                "pn %02x%02x%02x%02x%02x%02x",
                keyidx, MAC2STR(igtk->pn));
        wpa_hexdump_key(MSG_DEBUG, "WPA: IGTK",
                        igtk->igtk, len);
        if (keyidx > 4095)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: Invalid IGTK KeyID %d", keyidx);
            return -1;
        }
        if (wf_wpa_sm_set_key(sm, wf_wpa_cipher_to_alg(sm->mgmt_group_cipher),
                              broadcast_ether_addr,
                              keyidx, 0, igtk->pn, sizeof(igtk->pn),
                              igtk->igtk, len) < 0)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: Failed to configure IGTK to the driver");
            return -1;
        }
    }

    return 0;
#else /* CONFIG_IEEE80211W */
    return 0;
#endif /* CONFIG_IEEE80211W */
}


static void wf_wpa_report_ie_mismatch(struct wpa_sm *sm,
                                      const char *reason, const wf_u8 *src_addr,
                                      const wf_u8 *wpa_ie, size_t wpa_ie_len,
                                      const wf_u8 *rsn_ie, size_t rsn_ie_len)
{
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING, "WPA: %s (src=" MACSTR ")",
            reason, MAC2STR(src_addr));

    if (sm->ap_wpa_ie)
    {
        wpa_hexdump(MSG_INFO, "WPA: WPA IE in Beacon/ProbeResp",
                    sm->ap_wpa_ie, sm->ap_wpa_ie_len);
    }
    if (wpa_ie)
    {
        if (!sm->ap_wpa_ie)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                    "WPA: No WPA IE in Beacon/ProbeResp");
        }
        wpa_hexdump(MSG_INFO, "WPA: WPA IE in 3/4 msg",
                    wpa_ie, wpa_ie_len);
    }

    if (sm->ap_rsn_ie)
    {
        wpa_hexdump(MSG_INFO, "WPA: RSN IE in Beacon/ProbeResp",
                    sm->ap_rsn_ie, sm->ap_rsn_ie_len);
    }
    if (rsn_ie)
    {
        if (!sm->ap_rsn_ie)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                    "WPA: No RSN IE in Beacon/ProbeResp");
        }
        wpa_hexdump(MSG_INFO, "WPA: RSN IE in 3/4 msg",
                    rsn_ie, rsn_ie_len);
    }

}



static int wf_wpa_supplicant_validate_ie(struct wpa_sm *sm,
        const unsigned char *src_addr,
        struct wpa_eapol_ie_parse *ie)
{
    if (sm->ap_wpa_ie == NULL && sm->ap_rsn_ie == NULL)
    {
        wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
                "WPA: No WPA/RSN IE for this AP known. "
                "Trying to get from scan results");

    }

    if (ie->wpa_ie == NULL && ie->rsn_ie == NULL &&
        (sm->ap_wpa_ie || sm->ap_rsn_ie))
    {
        wf_wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                                  "with IE in Beacon/ProbeResp (no IE?)",
                                  src_addr, ie->wpa_ie, ie->wpa_ie_len,
                                  ie->rsn_ie, ie->rsn_ie_len);
        return -1;
    }
#if 0
    printf("wpa_ie_len:%d\n",ie->wpa_ie_len);
    sm->ap_wpa_ie_len = ie->wpa_ie_len;
    memcpy(sm->ap_wpa_ie,ie->wpa_ie,ie->wpa_ie_len);
    if ((ie->wpa_ie && sm->ap_wpa_ie &&
         (ie->wpa_ie_len != sm->ap_wpa_ie_len ||
          os_memcmp(ie->wpa_ie, sm->ap_wpa_ie, ie->wpa_ie_len) != 0)) ||
        (ie->rsn_ie && sm->ap_rsn_ie &&
         wpa_compare_rsn_ie(wpa_key_mgmt_ft(sm->key_mgmt),
                            sm->ap_rsn_ie, sm->ap_rsn_ie_len,
                            ie->rsn_ie, ie->rsn_ie_len)))
    {
        wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                               "with IE in Beacon/ProbeResp",
                               src_addr, ie->wpa_ie, ie->wpa_ie_len,
                               ie->rsn_ie, ie->rsn_ie_len);
        return -1;
    }
#endif
    if (sm->proto == WPA_PROTO_WPA &&
        ie->rsn_ie && sm->ap_rsn_ie == NULL && sm->rsn_enabled)
    {
        wf_wpa_report_ie_mismatch(sm, "Possible downgrade attack "
                                  "detected - RSN was enabled and RSN IE "
                                  "was in msg 3/4, but not in "
                                  "Beacon/ProbeResp",
                                  src_addr, ie->wpa_ie, ie->wpa_ie_len,
                                  ie->rsn_ie, ie->rsn_ie_len);
        return -1;
    }

#ifdef CONFIG_IEEE80211R
    if (wpa_key_mgmt_ft(sm->key_mgmt) &&
        wpa_supplicant_validate_ie_ft(sm, src_addr, ie) < 0)
        return -1;
#endif /* CONFIG_IEEE80211R */

    return 0;
}


/**
* wf_wpa_supplicant_send_4_of_4 - Send message 4 of WPA/RSN 4-Way Handshake
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @dst: Destination address for the frame
* @key: Pointer to the EAPOL-Key frame header
* @ver: Version bits from EAPOL-Key Key Info
* @key_info: Key Info
* @ptk: PTK to use for keyed hash and encryption
* Returns: >= 0 on success, < 0 on failure
*/
int wf_wpa_supplicant_send_4_of_4(struct wpa_sm *sm, const unsigned char *dst,
                                  const struct wpa_eapol_key *key,
                                  wf_u16 ver, wf_u16 key_info,
                                  struct wpa_ptk *ptk)
{
    size_t mic_len, hdrlen, rlen;
    struct wpa_eapol_key *reply;
    struct wpa_eapol_key_192 *reply192;
    wf_u8 *rbuf, *key_mic;

    mic_len = wf_wpa_mic_len(sm->key_mgmt);
    hdrlen = mic_len == 24 ? sizeof(*reply192) : sizeof(*reply);
    rbuf = wf_wpa_sm_alloc_eapol((struct wpa_supplicant *)sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                                 hdrlen, &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;
    reply192 = (struct wpa_eapol_key_192 *) reply;

    reply->type = (sm->proto == WPA_PROTO_RSN ||
                   sm->proto == WPA_PROTO_OSEN) ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info &= WPA_KEY_INFO_SECURE;
    key_info |= ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC;
    WPA_PUT_BE16(reply->key_info, key_info);
    if (sm->proto == WPA_PROTO_RSN || sm->proto == WPA_PROTO_OSEN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        wf_memcpy(reply->key_length, key->key_length, 2);
    wf_memcpy(reply->replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);

    key_mic = reply192->key_mic; /* same offset for reply and reply192 */
    if (mic_len == 24)
        WPA_PUT_BE16(reply192->key_data_length, 0);
    else
        WPA_PUT_BE16(reply->key_data_length, 0);

    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: Sending EAPOL-Key 4/4");
    return wf_wpa_eapol_key_send(sm, ptk->kck, ptk->kck_len, ver, dst,
                                 ETH_P_EAPOL, rbuf, rlen, key_mic);
}


static void wf_wpa_supplicant_process_3_of_4(struct wpa_sm *sm,
        const struct wpa_eapol_key *key,
        wf_u16 ver, const wf_u8 *key_data,
        size_t key_data_len)
{
    wf_u16 key_info, keylen;
    struct wpa_eapol_ie_parse ie;

    wf_wpa_sm_set_state(sm, WPA_4WAY_HANDSHAKE);
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: RX message 3 of 4-Way "
            "Handshake from " MACSTR " (ver=%d)", MAC2STR(sm->bssid), ver);

    key_info = WPA_GET_BE16(key->key_info);

    wpa_hexdump(MSG_DEBUG, "WPA: IE KeyData", key_data, key_data_len);
    if (wf_wpa_supplicant_parse_ies(key_data, key_data_len, &ie) < 0)
        goto failed;
    if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: GTK IE in unencrypted key data");
        goto failed;
    }
#ifdef CONFIG_IEEE80211W
    if (ie.igtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: IGTK KDE in unencrypted key data");
        goto failed;
    }

    if (ie.igtk &&
        wpa_cipher_valid_mgmt_group(sm->mgmt_group_cipher) &&
        ie.igtk_len != WPA_IGTK_KDE_PREFIX_LEN +
        (unsigned int) wf_wpa_cipher_key_len(sm->mgmt_group_cipher))
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: Invalid IGTK KDE length %lu",
                (unsigned long) ie.igtk_len);
        goto failed;
    }
#endif /* CONFIG_IEEE80211W */

    if (wf_wpa_supplicant_validate_ie(sm, sm->bssid, &ie) < 0)
        goto failed;

    if (os_memcmp(sm->anonce, key->key_nonce, WPA_NONCE_LEN) != 0)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: ANonce from message 1 of 4-Way Handshake "
                "differs from 3 of 4-Way Handshake - drop packet (src="
                MACSTR ")", MAC2STR(sm->bssid));
        goto failed;
    }

    keylen = WPA_GET_BE16(key->key_length);
    if (keylen != wf_wpa_cipher_key_len(sm->pairwise_cipher))
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: Invalid %s key length %d (src=" MACSTR
                ")", wf_wpa_cipher_txt(sm->pairwise_cipher), keylen,
                MAC2STR(sm->bssid));
        goto failed;
    }

#ifdef CONFIG_P2P
    if (ie.ip_addr_alloc)
    {
        wf_memcpy(sm->p2p_ip_addr, ie.ip_addr_alloc, 3 * 4);
        wpa_hexdump(MSG_DEBUG, "P2P: IP address info",
                    sm->p2p_ip_addr, sizeof(sm->p2p_ip_addr));
    }
#endif /* CONFIG_P2P */

    if (wf_wpa_supplicant_send_4_of_4(sm, sm->bssid, key, ver, key_info,
                                      &sm->ptk) < 0)
    {
        goto failed;
    }

    /* SNonce was successfully used in msg 3/4, so mark it to be renewed
    * for the next 4-Way Handshake. If msg 3 is received again, the old
    * SNonce will still be used to avoid changing PTK. */
    sm->renew_snonce = 1;

    if (key_info & WPA_KEY_INFO_INSTALL)
    {
        if (wf_wpa_supplicant_install_ptk(sm, key))
            goto failed;
    }


    wf_wpa_sm_set_state(sm, WPA_GROUP_HANDSHAKE);

    if (sm->group_cipher == WPA_CIPHER_GTK_NOT_USED)
    {
        wf_wpa_supplicant_key_neg_complete(sm, sm->bssid,
                                           key_info & WPA_KEY_INFO_SECURE);
    }
    else if (ie.gtk &&
             wf_wpa_supplicant_pairwise_gtk(sm, key,
                                            ie.gtk, ie.gtk_len, key_info) < 0)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                "RSN: Failed to configure GTK");
        goto failed;
    }

    if (wf_ieee80211w_set_keys(sm, &ie) < 0)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                "RSN: Failed to configure IGTK");
        goto failed;
    }


    sm->msg_3_of_4_ok = 1;
    return;

failed:
    return;
}


static int wf_wpa_supplicant_process_1_of_2_rsn(struct wpa_sm *sm,
        const wf_u8 *keydata,
        size_t keydatalen,
        wf_u16 key_info,
        struct wpa_gtk_data *gd)
{
    int maxkeylen;
    struct wpa_eapol_ie_parse ie;

    wpa_hexdump(MSG_DEBUG, "RSN: msg 1/2 key data", keydata, keydatalen);
    if (wf_wpa_supplicant_parse_ies(keydata, keydatalen, &ie) < 0)
        return -1;
    if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: GTK IE in unencrypted key data");
        return -1;
    }
    if (ie.gtk == NULL)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                "WPA: No GTK IE in Group Key msg 1/2");
        return -1;
    }
    maxkeylen = gd->gtk_len = ie.gtk_len - 2;

    if (wf_wpa_supplicant_check_group_cipher(sm, sm->group_cipher,
            gd->gtk_len, maxkeylen,
            &gd->key_rsc_len, &gd->alg))
        return -1;

    wpa_hexdump_key(MSG_DEBUG, "RSN: received GTK in group key handshake",
                    ie.gtk, ie.gtk_len);
    gd->keyidx = ie.gtk[0] & 0x3;
    gd->tx = wf_wpa_supplicant_gtk_tx_bit_workaround(sm,
             !!(ie.gtk[0] & BIT(2)));
    if (ie.gtk_len - 2 > sizeof(gd->gtk))
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                "RSN: Too long GTK in GTK IE (len=%lu)",
                (unsigned long) ie.gtk_len - 2);
        return -1;
    }
    wf_memcpy(gd->gtk, ie.gtk + 2, ie.gtk_len - 2);

    if (wf_ieee80211w_set_keys(sm, &ie) < 0)
        wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                "RSN: Failed to configure IGTK");

    return 0;
}


static int wf_wpa_supplicant_process_1_of_2_wpa(struct wpa_sm *sm,
        const struct wpa_eapol_key *key,
        const wf_u8 *key_data,
        size_t key_data_len, wf_u16 key_info,
        wf_u16 ver, struct wpa_gtk_data *gd)
{
    size_t maxkeylen;
    wf_u16 gtk_len;

    gtk_len = WPA_GET_BE16(key->key_length);
    if (gtk_len == 32)
    {
        sm->group_cipher = WPA_CIPHER_TKIP;
    }
    else if (gtk_len == 16)
    {
        sm->group_cipher = WPA_CIPHER_CCMP;
    }
    maxkeylen = key_data_len;
    if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES)
    {
        if (maxkeylen < 8)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                    "WPA: Too short maxkeylen (%lu)",
                    (unsigned long) maxkeylen);
            return -1;
        }
        maxkeylen -= 8;
    }

    if (gtk_len > maxkeylen ||
        wf_wpa_supplicant_check_group_cipher(sm, sm->group_cipher,
                gtk_len, maxkeylen,
                &gd->key_rsc_len, &gd->alg))
        return -1;

    gd->gtk_len = gtk_len;
    gd->keyidx = (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) >>
                 WPA_KEY_INFO_KEY_INDEX_SHIFT;
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 && sm->ptk.kek_len == 16)
    {
#ifdef CONFIG_NO_RC4
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: RC4 not supported in the build");
        return -1;
#else /* CONFIG_NO_RC4 */
        wf_u8 ek[32];
        if (key_data_len > sizeof(gd->gtk))
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: RC4 key data too long (%lu)",
                    (unsigned long) key_data_len);
            return -1;
        }
        wf_memcpy(ek, key->key_iv, 16);
        wf_memcpy(ek + 16, sm->ptk.kek, sm->ptk.kek_len);
        wf_memcpy(gd->gtk, key_data, key_data_len);
        if (wf_rc4_skip(ek, 32, 256, gd->gtk, key_data_len))
        {
            wf_memset(ek, 0, sizeof(ek));
            wpa_msg(sm->ctx->msg_ctx, MSG_ERROR,
                    "WPA: RC4 failed");
            return -1;
        }
        wf_memset(ek, 0, sizeof(ek));
#endif /* CONFIG_NO_RC4 */
    }
    else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES)
    {
        if (maxkeylen % 8)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: Unsupported AES-WRAP len %lu",
                    (unsigned long) maxkeylen);
            return -1;
        }
        if (maxkeylen > sizeof(gd->gtk))
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: AES-WRAP key data "
                    "too long (keydatalen=%lu maxkeylen=%lu)",
                    (unsigned long) key_data_len,
                    (unsigned long) maxkeylen);
            return -1;
        }
        if (wf_aes_unwrap(sm->ptk.kek, sm->ptk.kek_len, maxkeylen / 8,
                          key_data, gd->gtk))
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: AES unwrap failed - could not decrypt "
                    "GTK");
            return -1;
        }
    }
    else
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: Unsupported key_info type %d", ver);
        return -1;
    }
    gd->tx = wf_wpa_supplicant_gtk_tx_bit_workaround(
                 sm, !!(key_info & WPA_KEY_INFO_TXRX));
    return 0;
}


static int wf_wpa_supplicant_send_2_of_2(struct wpa_sm *sm,
        const struct wpa_eapol_key *key,
        int ver, wf_u16 key_info)
{
    size_t mic_len, hdrlen, rlen;
    struct wpa_eapol_key *reply;
    struct wpa_eapol_key_192 *reply192;
    wf_u8 *rbuf, *key_mic;

    mic_len = wf_wpa_mic_len(sm->key_mgmt);
    hdrlen = mic_len == 24 ? sizeof(*reply192) : sizeof(*reply);
    rbuf = wf_wpa_sm_alloc_eapol((struct wpa_supplicant *)sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                                 hdrlen, &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;
    reply192 = (struct wpa_eapol_key_192 *) reply;

    reply->type = (sm->proto == WPA_PROTO_RSN ||
                   sm->proto == WPA_PROTO_OSEN) ?
                  EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info &= WPA_KEY_INFO_KEY_INDEX_MASK;
    key_info |= ver | WPA_KEY_INFO_MIC | WPA_KEY_INFO_SECURE;
    WPA_PUT_BE16(reply->key_info, key_info);
    if (sm->proto == WPA_PROTO_RSN || sm->proto == WPA_PROTO_OSEN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        wf_memcpy(reply->key_length, key->key_length, 2);
    wf_memcpy(reply->replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);

    key_mic = reply192->key_mic; /* same offset for reply and reply192 */
    if (mic_len == 24)
        WPA_PUT_BE16(reply192->key_data_length, 0);
    else
        WPA_PUT_BE16(reply->key_data_length, 0);

    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: Sending EAPOL-Key 2/2");
    return wf_wpa_eapol_key_send(sm, sm->ptk.kck, sm->ptk.kck_len, ver,
                                 sm->bssid, ETH_P_EAPOL, rbuf, rlen, key_mic);
}


static void wf_wpa_supplicant_process_1_of_2(struct wpa_sm *sm,
        const unsigned char *src_addr,
        const struct wpa_eapol_key *key,
        const wf_u8 *key_data,
        size_t key_data_len, wf_u16 ver)
{
    wf_u16 key_info;
    int rekey, ret;
    struct wpa_gtk_data gd;
    const wf_u8 *key_rsc;

    if (!sm->msg_3_of_4_ok)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                "WPA: Group Key Handshake started prior to completion of 4-way handshake");
        goto failed;
    }

    wf_memset(&gd, 0, sizeof(gd));

    rekey = wf_wpa_sm_get_state() == WPA_COMPLETED;
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: RX message 1 of Group Key "
            "Handshake from " MACSTR " (ver=%d)", MAC2STR(src_addr), ver);

    key_info = WPA_GET_BE16(key->key_info);

    if (sm->proto == WPA_PROTO_RSN || sm->proto == WPA_PROTO_OSEN)
    {
        ret = wf_wpa_supplicant_process_1_of_2_rsn(sm, key_data,
                key_data_len, key_info,
                &gd);
    }
    else
    {
        ret = wf_wpa_supplicant_process_1_of_2_wpa(sm, key, key_data,
                key_data_len,
                key_info, ver, &gd);
    }

    wf_wpa_sm_set_state(sm, WPA_GROUP_HANDSHAKE);

    if (ret)
        goto failed;

    key_rsc = key->key_rsc;
    if (wf_wpa_supplicant_rsc_relaxation(sm, key->key_rsc))
        key_rsc = null_rsc;

    if (wf_wpa_supplicant_install_gtk(sm, &gd, key_rsc) ||
        wf_wpa_supplicant_send_2_of_2(sm, key, ver, key_info) < 0)
        goto failed;
    wf_memset(&gd, 0, sizeof(gd));

    if (rekey)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_INFO, "WPA: Group rekeying "
                "completed with " MACSTR " [GTK=%s]",
                MAC2STR(sm->bssid), wf_wpa_cipher_txt(sm->group_cipher));
        wf_wpa_sm_set_state(sm, WPA_COMPLETED);
    }
    else
    {
        wf_wpa_supplicant_key_neg_complete(sm, sm->bssid,
                                           key_info &
                                           WPA_KEY_INFO_SECURE);
    }


    return;

failed:
    wf_memset(&gd, 0, sizeof(gd));
}


static int wf_wpa_supplicant_verify_eapol_key_mic(struct wpa_sm *sm,
        struct wpa_eapol_key_192 *key,
        wf_u16 ver,
        const wf_u8 *buf, size_t len)
{
    wf_u8 mic[WPA_EAPOL_KEY_MIC_MAX_LEN];
    int ok = 0;
    size_t mic_len = wf_wpa_mic_len(sm->key_mgmt);
    int i=0;

    wf_memcpy(mic, key->key_mic, mic_len);
    if (sm->tptk_set)
    {
        wf_memset(key->key_mic, 0, mic_len);


        wf_wpa_eapol_key_mic(sm->tptk.kck, sm->tptk.kck_len, sm->key_mgmt,
                             ver, buf, len, key->key_mic);


        if (wf_os_memcmp_const(mic, key->key_mic, mic_len) != 0)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: Invalid EAPOL-Key MIC "
                    "when using TPTK - ignoring TPTK");
        }
        else
        {
            ok = 1;
            sm->tptk_set = 0;
            sm->ptk_set = 1;
            wf_memcpy(&sm->ptk, &sm->tptk, sizeof(sm->ptk));
            wf_memset(&sm->tptk, 0, sizeof(sm->tptk));
        }
    }

    if (!ok && sm->ptk_set)
    {
        wf_memset(key->key_mic, 0, mic_len);
        wf_wpa_eapol_key_mic(sm->ptk.kck, sm->ptk.kck_len, sm->key_mgmt,
                             ver, buf, len, key->key_mic);
        if (wf_os_memcmp_const(mic, key->key_mic, mic_len) != 0)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: Invalid EAPOL-Key MIC - "
                    "dropping packet");
            return -1;
        }
        ok = 1;
    }

    if (!ok)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: Could not verify EAPOL-Key MIC - "
                "dropping packet");
        return -1;
    }

    wf_memcpy(sm->rx_replay_counter, key->replay_counter,
              WPA_REPLAY_COUNTER_LEN);
    sm->rx_replay_counter_set = 1;
    return 0;
}


/* Decrypt RSN EAPOL-Key key data (RC4 or AES-WRAP) */
static int wf_wpa_supplicant_decrypt_key_data(struct wpa_sm *sm,
        struct wpa_eapol_key *key, wf_u16 ver,
        wf_u8 *key_data, size_t *key_data_len)
{
    if (!sm->ptk_set)
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: PTK not available, cannot decrypt EAPOL-Key Key "
                "Data");
        return -1;
    }

    /* Decrypt key data here so that this operation does not need
    * to be implemented separately for each message type. */
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 && sm->ptk.kek_len == 16)
    {
#ifdef CONFIG_NO_RC4
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: RC4 not supported in the build");
        return -1;
#else /* CONFIG_NO_RC4 */
        wf_u8 ek[32];
        wf_memcpy(ek, key->key_iv, 16);
        wf_memcpy(ek + 16, sm->ptk.kek, sm->ptk.kek_len);
        if (wf_rc4_skip(ek, 32, 256, key_data, *key_data_len))
        {
            wf_memset(ek, 0, sizeof(ek));
            wpa_msg(sm->ctx->msg_ctx, MSG_ERROR,
                    "WPA: RC4 failed");
            return -1;
        }
        wf_memset(ek, 0, sizeof(ek));
#endif /* CONFIG_NO_RC4 */
    }
    else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES ||
             ver == WPA_KEY_INFO_TYPE_AES_128_CMAC ||
             sm->key_mgmt == WPA_KEY_MGMT_OSEN ||
             wpa_key_mgmt_suite_b(sm->key_mgmt))
    {
        wf_u8 *buf;
        if (*key_data_len < 8 || *key_data_len % 8)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: Unsupported AES-WRAP len %u",
                    (unsigned int) *key_data_len);
            return -1;
        }
        *key_data_len -= 8; /* AES-WRAP adds 8 bytes */
        buf = wf_kzalloc(*key_data_len);
        if (buf == NULL)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: No memory for AES-UNWRAP buffer");
            return -1;
        }
        if (wf_aes_unwrap(sm->ptk.kek, sm->ptk.kek_len, *key_data_len / 8,
                          key_data, buf))
        {
            wf_os_bin_clear_free(buf, *key_data_len);
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: AES unwrap failed - "
                    "could not decrypt EAPOL-Key key data");
            return -1;
        }
        wf_memcpy(key_data, buf, *key_data_len);
        wf_os_bin_clear_free(buf, *key_data_len);
        WPA_PUT_BE16(key->key_data_length, *key_data_len);
    }
    else
    {
        wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                "WPA: Unsupported key_info type %d", ver);
        return -1;
    }
    wpa_hexdump_key(MSG_DEBUG, "WPA: decrypted EAPOL-Key key data",
                    key_data, *key_data_len);
    return 0;
}

static wf_u8 *wf_wpa_add_kde(wf_u8 * pos, wf_u32 kde, const wf_u8 * data, size_t data_len)
{
    *pos++ = WLAN_VENDOR_SPECIFIC_EID;
    *pos++ = _SELECTOR_LEN_TO_RSN + data_len;
    TILK_SELE_CTOR_RSN__PUT(pos, kde);
    pos += _SELECTOR_LEN_TO_RSN;
    wf_memcpy(pos, data, data_len);
    pos += data_len;
    return pos;
}



static wf_u8 *wf_wpa_add_ie(wf_u8 * pos, const wf_u8 * ie, size_t ie_len)
{
    wf_memcpy(pos, ie, ie_len);
    return pos + ie_len;
}

/**
* wf_wpa_sm_rx_eapol - Process received WPA EAPOL frames
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @src_addr: Source MAC address of the EAPOL packet
* @buf: Pointer to the beginning of the EAPOL data (EAPOL header)
* @len: Length of the EAPOL frame
* Returns: 1 = WPA EAPOL-Key processed, 0 = not a WPA EAPOL-Key, -1 failure
*
* This function is called for each received EAPOL frame. Other than EAPOL-Key
* frames can be skipped if filtering is done elsewhere. wf_wpa_sm_rx_eapol() is
* only processing WPA and WPA2 EAPOL-Key frames.
*
* The received EAPOL-Key packets are validated and valid packets are replied
* to. In addition, key material (PTK, GTK) is configured at the end of a
* successful key handshake.
*/
int wf_wpa_sm_rx_eapol(struct wpa_sm *sm, const wf_u8 *src_addr,
                       const wf_u8 *buf, size_t len)
{
    size_t plen, data_len, key_data_len;
    const struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    struct wpa_eapol_key_192 *key192;
    wf_u16 key_info, ver;
    wf_u8 *tmp = NULL;
    int ret = -1;
    struct wpa_peerkey *peerkey = NULL;
    wf_u8 *key_data;
    size_t mic_len, keyhdrlen;
    nic_info_st *pnic_info = (nic_info_st *)sm->pnic_info;
    sec_info_st *sec_info = pnic_info->sec_info;

#ifdef CONFIG_IEEE80211R
    sm->ft_completed = 0;
#endif /* CONFIG_IEEE80211R */

    mic_len = wf_wpa_mic_len(sm->key_mgmt);
    keyhdrlen = mic_len == 24 ? sizeof(*key192) : sizeof(*key);

    hdr = (const struct ieee802_1x_hdr *) buf;
    plen = be_to_host16(hdr->length);
    data_len = plen + sizeof(*hdr);
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "IEEE 802.1X RX: version=%d type=%d length=%lu",
            hdr->version, hdr->type, (unsigned long) plen);

    if (hdr->version < EAPOL_VERSION)
    {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAPOL_KEY)
    {
        wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
                "WPA: EAPOL frame (type %u) discarded, "
                "not a Key frame", hdr->type);
        ret = 0;
        goto out;
    }


    /*
    * Make a copy of the frame since we need to modify the buffer during
    * MAC validation and Key Data decryption.
    */
    tmp = wf_kzalloc(data_len);
    if (tmp == NULL)
        goto out;
    wf_memcpy(tmp, buf, data_len);
    key = (struct wpa_eapol_key *) (tmp + sizeof(struct ieee802_1x_hdr));
    key192 = (struct wpa_eapol_key_192 *)
             (tmp + sizeof(struct ieee802_1x_hdr));
    if (mic_len == 24)
        key_data = (wf_u8 *) (key192 + 1);
    else
        key_data = (wf_u8 *) (key + 1);

    if (key->type != EAPOL_KEY_TYPE_WPA && key->type != EAPOL_KEY_TYPE_RSN)
    {
        wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
                "WPA: EAPOL-Key type (%d) unknown, discarded",
                key->type);
        ret = 0;
        goto out;
    }

    if (mic_len == 24)
        key_data_len = WPA_GET_BE16(key192->key_data_length);
    else
        key_data_len = WPA_GET_BE16(key->key_data_length);

    key_info = WPA_GET_BE16(key->key_info);
    ver = key_info & WPA_KEY_INFO_TYPE_MASK;
    if ((key_info & WPA_KEY_INFO_MIC) &&
        wf_wpa_supplicant_verify_eapol_key_mic(sm, key192, ver, tmp, data_len))
        return -1;


    if ((sec_info->rsn_enable == 1) &&
        (key_info & WPA_KEY_INFO_ENCR_KEY_DATA))
    {
        if (wf_wpa_supplicant_decrypt_key_data(sm, key, ver, key_data,
                                               &key_data_len))
            goto out;
    }

    if (key_info & WPA_KEY_INFO_KEY_TYPE)
    {
        if (key_info & WPA_KEY_INFO_KEY_INDEX_MASK)
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: Ignored EAPOL-Key (Pairwise) with "
                    "non-zero key index");
            goto out;
        }
        if (key_info & WPA_KEY_INFO_MIC)
        {
            /* 3/4 4-Way Handshake */
            wf_wpa_supplicant_process_3_of_4(sm, key, ver, key_data,
                                             key_data_len);
        }
        else
        {
            /* 1/4 4-Way Handshake */
            wf_wpa_supplicant_process_1_of_4(sm, src_addr, key,
                                             ver, key_data,
                                             key_data_len);
        }
    }
    else
    {
        if (key_info & WPA_KEY_INFO_MIC)
        {
            /* 1/2 Group Key Handshake */
            wf_wpa_supplicant_process_1_of_2(sm, src_addr, key,
                                             key_data, key_data_len,
                                             ver);
        }
        else
        {
            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                    "WPA: EAPOL-Key (Group) without Mic bit - "
                    "dropped");
        }
    }

    ret = 1;

out:
    wf_os_bin_clear_free(tmp, data_len);
    return ret;
}



/**
* wf_wpa_sm_init - Initialize WPA state machine
* @ctx: Context pointer for callbacks; this needs to be an allocated buffer
* Returns: Pointer to the allocated WPA state machine data
*
* This function is used to allocate a new WPA state machine and the returned
* value is passed to all WPA state machine calls.
*/
struct wpa_sm * wf_wpa_sm_init(struct wpa_supplicant *wpa_s)
{
    struct wpa_sm *sm;

    sm = wf_kzalloc(sizeof(*sm));
    if (sm == NULL)
        return NULL;
    sm->renew_snonce = 1;

    sm->dot11RSNAConfigPMKLifetime = 43200;
    sm->dot11RSNAConfigPMKReauthThreshold = 70;
    sm->dot11RSNAConfigSATimeout = 60;

    return sm;
}


void wf_wpa_sm_drop_sa(struct wpa_sm *sm)
{
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: Clear old PMK and PTK");
    sm->ptk_set = 0;
    sm->tptk_set = 0;
    wf_memset(sm->pmk, 0, sizeof(sm->pmk));
    wf_memset(&sm->ptk, 0, sizeof(sm->ptk));
    wf_memset(&sm->tptk, 0, sizeof(sm->tptk));
#ifdef CONFIG_IEEE80211R
    wf_memset(sm->xxkey, 0, sizeof(sm->xxkey));
    wf_memset(sm->pmk_r0, 0, sizeof(sm->pmk_r0));
    wf_memset(sm->pmk_r1, 0, sizeof(sm->pmk_r1));
#endif /* CONFIG_IEEE80211R */
}


/**
* wf_wpa_sm_deinit - Deinitialize WPA state machine
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
*/
void wf_wpa_sm_deinit(struct wpa_sm *sm)
{
    if (sm == NULL)
        return;

    wf_free(sm->assoc_wpa_ie);
    wf_free(sm->ap_wpa_ie);
    wf_free(sm->ap_rsn_ie);
    wf_wpa_sm_drop_sa(sm);
    wf_free(sm->ctx);
    wf_free(sm);
}


/**
* wf_wpa_sm_set_fast_reauth - Set fast reauthentication (EAP) enabled/disabled
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @fast_reauth: Whether fast reauthentication (EAP) is allowed
*/
void wf_wpa_sm_set_fast_reauth(struct wpa_sm *sm, int fast_reauth)
{
    if (sm)
        sm->fast_reauth = fast_reauth;
}


/**
* wf_wpa_sm_set_scard_ctx - Set context pointer for smartcard callbacks
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @scard_ctx: Context pointer for smartcard related callback functions
*/
void wf_wpa_sm_set_scard_ctx(struct wpa_sm *sm, void *scard_ctx)
{
    if (sm == NULL)
        return;
    sm->scard_ctx = scard_ctx;
}


/**
* wf_wpa_sm_set_config - Notification of current configration change
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @config: Pointer to current network configuration
*
* Notify WPA state machine that configuration has changed. config will be
* stored as a backpointer to network configuration. This can be %NULL to clear
* the stored pointed.
*/
void wf_wpa_sm_set_config(struct wpa_sm *sm, struct rsn_supp_config *config)
{
    if (!sm)
        return;

    if (config)
    {
        sm->network_ctx = config->network_ctx;
        sm->peerkey_enabled = config->peerkey_enabled;
        sm->allowed_pairwise_cipher = config->allowed_pairwise_cipher;
        sm->proactive_key_caching = config->proactive_key_caching;
        sm->eap_workaround = config->eap_workaround;
        sm->eap_conf_ctx = config->eap_conf_ctx;
        if (config->ssid)
        {
            wf_memcpy(sm->ssid, config->ssid, config->ssid_len);
            sm->ssid_len = config->ssid_len;
        }
        else
            sm->ssid_len = 0;
        sm->wpa_ptk_rekey = config->wpa_ptk_rekey;
    }
    else
    {
        sm->network_ctx = NULL;
        sm->peerkey_enabled = 0;
        sm->allowed_pairwise_cipher = 0;
        sm->proactive_key_caching = 0;
        sm->eap_workaround = 0;
        sm->eap_conf_ctx = NULL;
        sm->ssid_len = 0;
        sm->wpa_ptk_rekey = 0;
        sm->p2p = 0;
    }
}


/**
* wf_wpa_sm_set_own_addr - Set own MAC address
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @addr: Own MAC address
*/
void wf_wpa_sm_set_own_addr(struct wpa_sm *sm, const wf_u8 *addr)
{
    if (sm)
        wf_memcpy(sm->own_addr, addr, ETH_ALEN);
}


/**
* wf_wpa_sm_set_ifname - Set network interface name
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @ifname: Interface name
* @bridge_ifname: Optional bridge interface name (for pre-auth)
*/
void wf_wpa_sm_set_ifname(struct wpa_sm *sm, const char *ifname,
                          const char *bridge_ifname)
{
    if (sm)
    {
        sm->ifname = ifname;
        sm->bridge_ifname = bridge_ifname;
    }
}


/**
* wf_wpa_sm_set_eapol - Set EAPOL state machine pointer
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @eapol: Pointer to EAPOL state machine allocated with eapol_sm_init()
*/
void wf_wpa_sm_set_eapol(struct wpa_sm *sm, struct eapol_sm *eapol)
{
    if (sm)
        sm->eapol = eapol;
}


/**
* wf_wpa_sm_set_param - Set WPA state machine parameters
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @param: Parameter field
* @value: Parameter value
* Returns: 0 on success, -1 on failure
*/
int wf_wpa_sm_set_param(struct wpa_sm *sm, enum wpa_sm_conf_params param,
                        unsigned int value)
{
    int ret = 0;

    if (sm == NULL)
        return -1;

    switch (param)
    {
        case RSNA_PMK_LIFETIME:
            if (value > 0)
                sm->dot11RSNAConfigPMKLifetime = value;
            else
                ret = -1;
            break;
        case RSNA_PMK_REAUTH_THRESHOLD:
            if (value > 0 && value <= 100)
                sm->dot11RSNAConfigPMKReauthThreshold = value;
            else
                ret = -1;
            break;
        case RSNA_SA_TIMEOUT:
            if (value > 0)
                sm->dot11RSNAConfigSATimeout = value;
            else
                ret = -1;
            break;
        case WPA_PARAM_PROTO:
            sm->proto = value;
            break;
        case WPA_PARAM_PAIRWISE:
            sm->pairwise_cipher = value;
            break;
        case WPA_PARAM_GROUP:
            sm->group_cipher = value;
            break;
        case WPA_PARAM_KEY_MGMT:
            sm->key_mgmt = value;
            break;
#ifdef CONFIG_IEEE80211W
        case WPA_PARAM_MGMT_GROUP:
            sm->mgmt_group_cipher = value;
            break;
#endif /* CONFIG_IEEE80211W */
        case WPA_PARAM_RSN_ENABLED:
            sm->rsn_enabled = value;
            break;
        case WPA_PARAM_MFP:
            sm->mfp = value;
            break;
        default:
            break;
    }

    return ret;
}


static int wf_wpa_gen_wpa_ie_rsn(wf_u8 * rsn_ie, size_t rsn_ie_len,
                                 int pairwise_cipher, int group_cipher,
                                 int key_mgmt, int mgmt_group_cipher,
                                 struct wpa_sm *sm)
{
    wf_u8 *pos;
    struct rsn_hdr_ie *hdr;
    wf_u16 capab;
    wf_u32 suite;

    if (rsn_ie_len < sizeof(*hdr) + _SELECTOR_LEN_TO_RSN +
        2 + _SELECTOR_LEN_TO_RSN + 2 + _SELECTOR_LEN_TO_RSN + 2 +
        (sm->cur_pmksa ? 2 + TK_PMK_ID_LEN : 0))
    {
        wpa_printf(MSG_DEBUG, "RSN: Too short IE buffer (%lu bytes)",
                   (unsigned long)rsn_ie_len);
        return -1;
    }

    hdr = (struct rsn_hdr_ie *)rsn_ie;
    hdr->elem_id = WLAN_RSN_EID;
    TILK_WPA_TO_LE16(hdr->version, RSN_VERSION);
    pos = (wf_u8 *) (hdr + 1);

    suite = wf_wpa_cipher_to_suite(WPA_RSN_PROTO, group_cipher);
    if (suite == 0)
    {
        wpa_printf(MSG_WARNING, "Invalid group cipher (%d).", group_cipher);
        return -1;
    }
    TILK_SELE_CTOR_RSN__PUT(pos, suite);
    pos += _SELECTOR_LEN_TO_RSN;

    *pos++ = 1;
    *pos++ = 0;
    suite = wf_wpa_cipher_to_suite(WPA_RSN_PROTO, pairwise_cipher);
    if (suite == 0 ||
        (!wf_wpa_cipher_valid_pairwise(pairwise_cipher) &&
         pairwise_cipher != WPA_NONE_CIPHER))
    {
        wpa_printf(MSG_WARNING, "Invalid pairwise cipher (%d).",
                   pairwise_cipher);
        return -1;
    }
    TILK_SELE_CTOR_RSN__PUT(pos, suite);
    pos += _SELECTOR_LEN_TO_RSN;

    *pos++ = 1;
    *pos++ = 0;
    if (key_mgmt == WPA_IEEE8021X_KEY_MGMT)
    {
        TILK_SELE_CTOR_RSN__PUT(pos, TILK_AKMGMT_UNSPEC_802_1X_FOR_RSN);
    }
    else if (key_mgmt == WPA_PSK_KEY_MGMT)
    {
        TILK_SELE_CTOR_RSN__PUT(pos, TILK_AKM_PSK_OVER_802_1X_FOR_RSN);
    }
    else if (key_mgmt == WPA_KEY_MGMT_CCKM)
    {
        TILK_SELE_CTOR_RSN__PUT(pos, RSN_AUTH_KEY_MGMT_CCKM);
    }
    else if (key_mgmt == WPA_KEY_MGMT_IEEE8021X_SUITE_B_192)
    {
        TILK_SELE_CTOR_RSN__PUT(pos, RSN_AUTH_KEY_MGMT_802_1X_SUITE_B_192);
    }
    else if (key_mgmt == WPA_KEY_MGMT_IEEE8021X_SUITE_B)
    {
        TILK_SELE_CTOR_RSN__PUT(pos, RSN_AUTH_KEY_MGMT_802_1X_SUITE_B);
    }
    else
    {
        wpa_printf(MSG_WARNING, "Invalid key management type (%d).", key_mgmt);
        return -1;
    }
    pos += _SELECTOR_LEN_TO_RSN;

    capab = 0;
    TILK_WPA_TO_LE16(pos, capab);
    pos += 2;

    if (sm->cur_pmksa)
    {

        *pos++ = 1;
        *pos++ = 0;

        wf_memcpy(pos, sm->cur_pmksa->pmkid, TK_PMK_ID_LEN);
        pos += TK_PMK_ID_LEN;
    }

    hdr->len = (pos - rsn_ie) - 2;

    WPA_ASSERT((size_t) (pos - rsn_ie) <= rsn_ie_len);

    return pos - rsn_ie;
}


static int wf_wpa_gen_wpa_ie_wpa(wf_u8 * wpa_ie, size_t wpa_ie_len,
                                 int pairwise_cipher, int group_cipher,
                                 int key_mgmt)
{
    wf_u8 *pos;
    struct wpa_hdr_lan *hdr;
    wf_u32 suite;

    if (wpa_ie_len < sizeof(*hdr) + WPA_LEN_SELECTOR +
        2 + WPA_LEN_SELECTOR + 2 + WPA_LEN_SELECTOR)
        return -1;

    hdr = (struct wpa_hdr_lan *)wpa_ie;
    hdr->elem_id = WLAN_VENDOR_SPECIFIC_EID;
    TILK_SELE_CTOR_RSN__PUT(hdr->oui, WPA_OUI_TYPE);
    TILK_WPA_TO_LE16(hdr->version, WPA_VERSION);
    pos = (wf_u8 *) (hdr + 1);

    suite = wf_wpa_cipher_to_suite(WPA_WPA_PROTO, group_cipher);
    if (suite == 0)
    {
        wpa_printf(MSG_WARNING, "Invalid group cipher (%d).", group_cipher);
        return -1;
    }
    TILK_SELE_CTOR_RSN__PUT(pos, suite);
    pos += WPA_LEN_SELECTOR;

    *pos++ = 1;
    *pos++ = 0;
    suite = wf_wpa_cipher_to_suite(WPA_WPA_PROTO, pairwise_cipher);
    if (suite == 0 ||
        (!wf_wpa_cipher_valid_pairwise(pairwise_cipher) &&
         pairwise_cipher != WPA_NONE_CIPHER))
    {
        wpa_printf(MSG_WARNING, "Invalid pairwise cipher (%d).",
                   pairwise_cipher);
        return -1;
    }
    TILK_SELE_CTOR_RSN__PUT(pos, suite);
    pos += WPA_LEN_SELECTOR;

    *pos++ = 1;
    *pos++ = 0;
    if (key_mgmt == WPA_IEEE8021X_KEY_MGMT)
    {
        TILK_SELE_CTOR_RSN__PUT(pos, _AUTH_KEY_MGMT_UNSPEC_802_1X_TO_WPA);
    }
    else if (key_mgmt == WPA_PSK_KEY_MGMT)
    {
        TILK_SELE_CTOR_RSN__PUT(pos, _AUTH_KEY_MGMT_PSK_OVER_802_1X_TO_WPA);
    }
    else if (key_mgmt == WPA_WPA_NONE_KEY_MGMT)
    {
        TILK_SELE_CTOR_RSN__PUT(pos, _AUTH_KEY_MGMT_NONE_TO_WPA);
    }
    else if (key_mgmt == WPA_KEY_MGMT_CCKM)
    {
        TILK_SELE_CTOR_RSN__PUT(pos, WPA_AUTH_KEY_MGMT_CCKM);
    }
    else
    {
        wpa_printf(MSG_WARNING, "Invalid key management type (%d).", key_mgmt);
        return -1;
    }
    pos += WPA_LEN_SELECTOR;

    hdr->len = (pos - wpa_ie) - 2;

    WPA_ASSERT((size_t) (pos - wpa_ie) <= wpa_ie_len);

    return pos - wpa_ie;
}


int wf_wpa_gen_wpa_ie(struct wpa_sm *sm, wf_u8 * wpa_ie, size_t wpa_ie_len)
{
    if (sm->proto == WPA_RSN_PROTO)
        return wf_wpa_gen_wpa_ie_rsn(wpa_ie, wpa_ie_len,
                                     sm->pairwise_cipher,
                                     sm->group_cipher,
                                     sm->key_mgmt, sm->mgmt_group_cipher, sm);
    else
        return wf_wpa_gen_wpa_ie_wpa(wpa_ie, wpa_ie_len,
                                     sm->pairwise_cipher,
                                     sm->group_cipher, sm->key_mgmt);
}


int wf_wpa_parse_wpa_ie(const wf_u8 * wpa_ie, size_t wpa_ie_len,
                        struct wpa_ie_data *data)
{
    if (wpa_ie_len >= 1 && wpa_ie[0] == WLAN_RSN_EID)
        return wf_wpa_parse_wpa_ie_rsn(wpa_ie, wpa_ie_len, data);
    if (wpa_ie_len >= 6 && wpa_ie[0] == WLAN_VENDOR_SPECIFIC_EID &&
        wpa_ie[1] >= 4 && TILK_WPA_READ_BE32(&wpa_ie[2]) == OSEN_IE_VENDOR_TYPE)
        return wf_wpa_parse_wpa_ie_rsn(wpa_ie, wpa_ie_len, data);
    else
        return wf_wpa_parse_wpa_ie_wpa(wpa_ie, wpa_ie_len, data);
}


/**
* wf_wpa_sm_parse_own_wpa_ie - Parse own WPA/RSN IE
* @sm: Pointer to WPA state machine data from wf_wpa_sm_init()
* @data: Pointer to data area for parsing results
* Returns: 0 on success, -1 if IE is not known, or -2 on parsing failure
*
* Parse the contents of the own WPA or RSN IE from (Re)AssocReq and write the
* parsed data into data.
*/
int wf_wpa_sm_parse_own_wpa_ie(struct wpa_sm *sm, struct wpa_ie_data *data)
{
    if (sm == NULL)
        return -1;

    if (sm->assoc_wpa_ie == NULL)
    {
        wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
                "WPA: No WPA/RSN IE available from association info");
        return -1;
    }
    if (wf_wpa_parse_wpa_ie(sm->assoc_wpa_ie, sm->assoc_wpa_ie_len, data))
        return -2;
    return 0;
}

void wf_wpa_sm_set_ptk_kck_kek(struct wpa_sm *sm,
                               const wf_u8 *ptk_kck, size_t ptk_kck_len,
                               const wf_u8 *ptk_kek, size_t ptk_kek_len)
{
    if (ptk_kck && ptk_kck_len <= WPA_KCK_MAX_LEN)
    {
        wf_memcpy(sm->ptk.kck, ptk_kck, ptk_kck_len);
        sm->ptk.kck_len = ptk_kck_len;
        wpa_printf(MSG_DEBUG, "Updated PTK KCK");
    }
    if (ptk_kek && ptk_kek_len <= WPA_KEK_MAX_LEN)
    {
        wf_memcpy(sm->ptk.kek, ptk_kek, ptk_kek_len);
        sm->ptk.kek_len = ptk_kek_len;
        wpa_printf(MSG_DEBUG, "Updated PTK KEK");
    }
    sm->ptk_set = 1;
}

int wf_wpa_eapol_sm_rx_eapol(struct eapol_sm *sm, const wf_u8 * src, const wf_u8 * buf,
                             size_t len)
{
    const struct ieee802_1x_hdr *hdr;
    const struct ieee802_1x_eapol_key *key;
    int data_len;
    int res = 1;
    size_t plen;

    if (sm == NULL)
        return 0;
    sm->dot1xSuppEapolFramesRx++;
    if (len < sizeof(*hdr))
    {
        sm->dot1xSuppInvalidEapolFramesRx++;
        return 0;
    }
    hdr = (const struct ieee802_1x_hdr *)buf;
    sm->dot1xSuppLastEapolFrameVersion = hdr->version;
    wf_memcpy(sm->dot1xSuppLastEapolFrameSource, src, ETH_ALEN);
    if (hdr->version < EAPOL_VERSION)
    {

    }
    plen = be_to_host16(hdr->length);
    if (plen > len - sizeof(*hdr))
    {
        sm->dot1xSuppEapLengthErrorFramesRx++;
        return 0;
    }
    data_len = plen + sizeof(*hdr);

    switch (hdr->type)
    {
        case IEEE802_1X_TYPE_EAP_PACKET:
            if (sm->conf.workaround)
            {

                const struct eap_hdr *ehdr = (const struct eap_hdr *)(hdr + 1);
                if (plen >= sizeof(*ehdr) && ehdr->code == 10)
                {
                    break;
                }
            }

            wf_wpabuf_free(sm->eapReqData);
            sm->eapReqData = wf_wpabuf_alloc_copy(hdr + 1, plen);
            if (sm->eapReqData)
            {
                wpa_printf(MSG_DEBUG, "EAPOL: Received EAP-Packet " "frame");
                sm->eapolEap = TRUE;

            }
            break;
        case IEEE802_1X_TYPE_EAPOL_KEY:
            if (plen < sizeof(*key))
            {
                wpa_printf(MSG_DEBUG, "EAPOL: Too short EAPOL-Key "
                           "frame received");
                break;
            }
            key = (const struct ieee802_1x_eapol_key *)(hdr + 1);
            if (key->type == EAPOL_KEY_TYPE_WPA || key->type == EAPOL_KEY_TYPE_RSN)
            {

                wpa_printf(MSG_DEBUG, "EAPOL: Ignoring WPA EAPOL-Key "
                           "frame in EAPOL state machines");
                res = 0;
                break;
            }
            if (key->type != EAPOL_KEY_TYPE_RC4)
            {
                wpa_printf(MSG_DEBUG, "EAPOL: Ignored unknown "
                           "EAPOL-Key type %d", key->type);
                break;
            }
            wf_free(sm->last_rx_key);
            sm->last_rx_key = wf_kzalloc(data_len);
            if (sm->last_rx_key)
            {
                wpa_printf(MSG_DEBUG, "EAPOL: Received EAPOL-Key " "frame");
                wf_memcpy(sm->last_rx_key, buf, data_len);
                sm->last_rx_key_len = data_len;
                sm->rxKey = TRUE;

            }
            break;
        default:
            wpa_printf(MSG_DEBUG, "EAPOL: Received unknown EAPOL type %d",
                       hdr->type);
            sm->dot1xSuppInvalidEapolFramesRx++;
            break;
    }

    return res;
}

void wf_wpa_supplicant_rx_eapol(struct wpa_supplicant *wpa_s, const wf_u8 * src_addr,
                                const wf_u8 * buf, size_t len)
{
    wpa_s->last_eapol_matches_bssid =
        os_memcompare(src_addr, wpa_s->bssid, ETH_ALEN) == 0;

    if (wpa_s->key_mgmt == WPA_NONE_KEY_MGMT)
    {
        return;
    }

    if (wpa_s->eapol_received == 0 &&
        (!(wpa_s->drv_flags & WPA_DRIVER_FLAGS_4WAY_HANDSHAKE) ||
         !wpa_key_mgmt_wpa_psk(wpa_s->key_mgmt) ||
         wpa_s->wpa_state != WPA_COMPLETED) &&
        (wpa_s->current_ssid == NULL ||
         wpa_s->current_ssid->mode != IEEE80211_MODE_IBSS))
    {

        int timeout = 10;

        if (wpa_key_mgmt_wpa_ieee8021x(wpa_s->key_mgmt) ||
            wpa_s->key_mgmt == WPA_IEEE8021X_NO_WPA_KEY_MGMT ||
            wpa_s->key_mgmt == WPA_KEY_MGMT_WPS)
        {

            timeout = 70;
        }
        timeout = timeout;
    }
    wpa_s->eapol_received++;

    if (wpa_s->countermeasures)
    {
        return;
    }

    wf_memcpy(wpa_s->last_eapol_src, src_addr, ETH_ALEN);
    if (!wpa_key_mgmt_wpa_psk(wpa_s->key_mgmt) &&
        wf_wpa_eapol_sm_rx_eapol(wpa_s->eapol, src_addr, (buf + 14), (len - 14)) > 0)
        return;
    if (!(wpa_s->drv_flags & WPA_DRIVER_FLAGS_4WAY_HANDSHAKE))
        wf_wpa_sm_rx_eapol(wpa_s->wpa, src_addr, (buf + 14), (len -14));
    else if (wpa_key_mgmt_wpa_ieee8021x(wpa_s->key_mgmt))
    {

    }
}


