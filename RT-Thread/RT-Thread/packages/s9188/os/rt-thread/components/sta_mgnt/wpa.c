/*
* WPA Supplicant - WPA state machine and EAPOL-Key processing
* Copyright (c) 2003-2015, Jouni Malinen <j@w1.fi>
* Copyright(c) 2015 Intel Deutschland GmbH
*
* This software may be distributed under the terms of the BSD license.
* See README for more details.
*/

#include "utils/common.h"
#include "crypto/aes.h"
#include "crypto/random.h"
#include "wpa.h"
#include "common/wpa_common.h"
#include "sha1.h"
#include "utils/wpabuf.h"
#include "rc4.h"
#include "common/wpa_common.h"

#include "wlan_dev/wf_wlan_dev.h"
#include "wf_debug.h"

static const wf_u8 null_rsc[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

extern struct wpa_supplicant wpa_s_obj;
extern struct wpa_config wpaconfig;

static int wf_wpa_sm_ether_send(struct wpa_sm *sm, const wf_u8 *dest,wf_u16 proto, wf_u8 *buf, size_t len)
{
  wf_u16 pkt_len;
  wf_u8 *buffer;
  nic_info_st *pnic_info = sm->pnic_info;
  struct rt_wlan_device *wlan = pnic_info->ndev;
  struct rt_wlan_priv *wlan_priv = wlan->user_data;
  struct pbuf *p;
  
  pkt_len = sizeof(struct wf_ethhdr) + len;
  p = pbuf_alloc(PBUF_RAW, pkt_len, PBUF_POOL);
  if (NULL == p) {
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
  return 0;
}


enum wpa_states wf_wpa_sm_get_state()
{
  return wpa_s_obj.wpa_state;
}

void wf_wpa_sm_set_state(struct wpa_sm *sm, enum wpa_states state)
{
  wpa_s_obj.wpa_state = state;   
  if(wpa_s_obj.last_eapol_matches_bssid && WPA_DISCONNECTED == state)
  {
    wpa_s_obj.last_eapol_matches_bssid = 0;
  }
}


void wf_eap_notify_success(struct eap_sm *sm)
{
  if (sm) {
    sm->decision = DECISION_COND_SUCC;
    sm->EAP_state = EAP_SUCCESS;
  }
}



const wf_u8 *wf_eap_get_eapkeydata(struct eap_sm *sm, size_t * len)
{
  if (sm == NULL || sm->eapKeyData == NULL) {
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
  
  if (sm == NULL ) {
    wpa_printf(MSG_DEBUG, "EAPOL: EAP key not available");
    return -1;
  }
  eap_key = wf_eap_get_eapkeydata(sm->eap, &eap_len);
  if (eap_key == NULL) {
    wpa_printf(MSG_DEBUG, "EAPOL: Failed to get eapKeyData");
    return -1;
  }
  if (len > eap_len) {
    wpa_printf(MSG_DEBUG, "EAPOL: Requested key length (%lu) not "
               "available (len=%lu)",
               (unsigned long)len, (unsigned long)eap_len);
    return eap_len;
  }
  os_memcpy(key, eap_key, len);
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

int wf_wpa_eapol_key_send(struct wpa_sm *sm, const wf_u8 *kck, size_t kck_len,
                          int ver, const wf_u8 *dest, wf_u16 proto,
                          wf_u8 *msg, size_t msg_len, wf_u8 *key_mic)
{
  int ret = -1;
  size_t mic_len = wf_wpa_mic_len(sm->key_mgmt);
  
  int i=0;
  if (is_zero_ether_addr(dest) && is_zero_ether_addr(sm->bssid)) {
    dest = sm->bssid;
  }
  if (key_mic &&
      wf_wpa_eapol_key_mic(kck, kck_len, 2, ver, msg, msg_len,
                           key_mic)) {
                             wpa_msg(sm->ctx->msg_ctx, MSG_ERROR,
                                     "WPA: Failed to generate EAPOL-Key version %d key_mgmt 0x%x MIC",
                                     ver, sm->key_mgmt);
                             goto out;
                           }
  
  ret = wf_wpa_sm_ether_send(sm, dest, proto, msg, msg_len);
  wf_eapol_sm_notify_tx_eapol_key(sm->eapol);
out:
  os_free(msg);
  return ret;
}



int wf_wpa_config_add_prio_network(struct wpa_config *config,
                                   struct wpa_ssid *ssid)
{
  int prio;
  struct wpa_ssid *prev, **nlist;
  
  for (prio = 0; prio < config->num_prio; prio++) {
    prev = config->pssid[prio];
    if (prev->priority == ssid->priority) {
      while (prev->pnext)
        prev = prev->pnext;
      prev->pnext = ssid;
      return 0;
    }
  }
  
  nlist = os_realloc_array(config->pssid, config->num_prio + 1,
                           sizeof(struct wpa_ssid *));
  if (nlist == NULL)
    return -1;
  
  for (prio = 0; prio < config->num_prio; prio++) {
    if (nlist[prio]->priority < ssid->priority) {
      os_memmove(&nlist[prio + 1], &nlist[prio],
                 (config->num_prio - prio) * sizeof(struct wpa_ssid *));
      break;
    }
  }
  
  nlist[prio] = ssid;
  config->num_prio++;
  config->pssid = nlist;
  
  return 0;
}

int wf_wpa_config_update_prio_list(struct wpa_config *config)
{
  struct wpa_ssid *ssid;
  int ret = 0;
  
  os_free(config->pssid);
  config->pssid = NULL;
  config->num_prio = 0;
  
  ssid = config->ssid;
  if (ssid) {
    ssid->pnext = NULL;
    if (wf_wpa_config_add_prio_network(config, ssid) < 0)
      ret = -1;
    ssid = ssid->next;
  }
  
  return ret;
}

struct wpa_ssid *wf_wpa_config_add_network(struct wpa_config *config)
{
  int id;
  struct wpa_ssid *ssid, *last = NULL;
  
  id = -1;
  ssid = config->ssid;
  while (ssid) {
    if (ssid->id > id)
      id = ssid->id;
    last = ssid;
    ssid = ssid->next;
  }
  id++;
  
  ssid = os_zalloc(sizeof(*ssid));
  if (ssid == NULL)
    return NULL;
  ssid->id = id;
  dl_list_init(&ssid->psk_list);
  if (last)
    last->next = ssid;
  else
    config->ssid = ssid;
  
  wf_wpa_config_update_prio_list(config);
  
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
  hdr = os_malloc(*msg_len);
  if (hdr == NULL)
    return NULL;
  hdr->version = 1;
  hdr->type = type;
  hdr->length = host_to_be16(data_len);
  
  if (data)
    os_memcpy(hdr + 1, data, data_len);
  else
    os_memset(hdr + 1, 0, data_len);
  
  
  if (data_pos)
    *data_pos = hdr + 1;
  
  return (wf_u8 *) hdr;
}

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
os_memcpy(reply->replay_counter, sm->request_counter,
          WPA_REPLAY_COUNTER_LEN);
wf_os_inc_byte_array(sm->request_counter, WPA_REPLAY_COUNTER_LEN);

if (mic_len == 24)
WPA_PUT_BE16(reply192->key_data_length, 0);
else
WPA_PUT_BE16(reply->key_data_length, 0);
if (!(key_info & WPA_KEY_INFO_MIC))
key_mic = NULL;
else
key_mic = reply192->key_mic;

wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
        "WPA: Sending EAPOL-Key Request (error=%d "
          "pairwise=%d ptk_set=%d len=%lu)",
          error, pairwise, sm->ptk_set, (unsigned long) rlen);
wf_wpa_eapol_key_send(sm, sm->ptk.kck, sm->ptk.kck_len, ver, bssid,
                      ETH_P_EAPOL, rbuf, rlen, key_mic);
}

struct rsn_pmksa_cache_entry * wf_wpa_pmksa_cache_get(struct rsn_pmksa_cache *pmksa,
                                                      const wf_u8 *aa, const wf_u8 *pmkid,
                                                      const void *network_ctx)
{
  struct rsn_pmksa_cache_entry *entry = pmksa->pmksa;
  while (entry) {
    if ((aa == NULL || os_memcmp(entry->aa, aa, ETH_ALEN) == 0) &&
        (pmkid == NULL ||
         os_memcmp(entry->pmkid, pmkid, PMKID_LEN) == 0) &&
          (network_ctx == NULL || network_ctx == entry->network_ctx))
      return entry;
    entry = entry->next;
  }
  return NULL;
}

static void wf_wpa_pmksa_cache_free_entry(struct rsn_pmksa_cache_entry *entry)
{
  wf_os_bin_clear_free(entry, sizeof(*entry));
}   

static void wf_pmksa_cache_free_entry(struct rsn_pmksa_cache *pmksa,
                                      struct rsn_pmksa_cache_entry *entry,
                                      enum pmksa_free_reason reason)
{
  
  pmksa->pmksa_count--;
  pmksa->free_cb(entry, pmksa->ctx, reason);
  wf_wpa_pmksa_cache_free_entry(entry);
}


void wf_pmksa_cache_clear_current(struct wpa_sm *sm)
{
  if (sm == NULL)
    return;
  sm->cur_pmksa = NULL;
}

void wf_wpa_pmksa_cache_flush(struct rsn_pmksa_cache *pmksa, void *network_ctx,
                              const wf_u8 *pmk, size_t pmk_len)
{
  struct rsn_pmksa_cache_entry *entry, *prev = NULL, *tmp;
  int removed = 0;
  
  entry = pmksa->pmksa;
  while (entry) {
    if ((entry->network_ctx == network_ctx ||
         network_ctx == NULL) &&
        (pmk == NULL ||
         (pmk_len == entry->pmk_len &&
          os_memcmp(pmk, entry->pmk, pmk_len) == 0))) {
            wpa_printf(MSG_DEBUG, "RSN: Flush PMKSA cache entry "
                       "for " MACSTR, MAC2STR(entry->aa));
            if (prev)
              prev->next = entry->next;
            else
              pmksa->pmksa = entry->next;
            tmp = entry;
            entry = entry->next;
            wf_pmksa_cache_free_entry(pmksa, tmp, PMKSA_FREE);
            removed++;
          } else {
            prev = entry;
            entry = entry->next;
          }
  }
  
}

struct rsn_pmksa_cache_entry *
wf_wpa_pmksa_cache_add(struct rsn_pmksa_cache *pmksa, const wf_u8 *pmk, size_t pmk_len,
                       const wf_u8 *pmkid, const wf_u8 *kck, size_t kck_len,
                       const wf_u8 *aa, const wf_u8 *spa, void *network_ctx, int akmp)
{
  struct rsn_pmksa_cache_entry *entry, *pos, *prev;
  struct os_reltime now;
  
  if (pmk_len > PMK_LEN_MAX)
    return NULL;
  
  if (wpa_key_mgmt_suite_b(akmp) && !kck)
    return NULL;
  
  entry = os_zalloc(sizeof(*entry));
  if (entry == NULL)
    return NULL;
  os_memcpy(entry->pmk, pmk, pmk_len);
  entry->pmk_len = pmk_len;
  if (pmkid)
    os_memcpy(entry->pmkid, pmkid, PMKID_LEN);
  else if (akmp == WPA_KEY_MGMT_IEEE8021X_SUITE_B_192)
    rsn_pmkid_suite_b_192(kck, kck_len, aa, spa, entry->pmkid);
  else if (wpa_key_mgmt_suite_b(akmp))
  {
  }
  else
    wf_wpa_rsn_pmkid(pmk, pmk_len, aa, spa, entry->pmkid,
                     wpa_key_mgmt_sha256(akmp));
  wf_os_get_reltime(&now);
  entry->expiration = now.sec + pmksa->sm->dot11RSNAConfigPMKLifetime;
  entry->reauth_time = now.sec + pmksa->sm->dot11RSNAConfigPMKLifetime *
    pmksa->sm->dot11RSNAConfigPMKReauthThreshold / 100;
  entry->akmp = akmp;
  os_memcpy(entry->aa, aa, ETH_ALEN);
  entry->network_ctx = network_ctx;
  
  pos = pmksa->pmksa;
  prev = NULL;
  while (pos) {
    if (os_memcmp(aa, pos->aa, ETH_ALEN) == 0) {
      if (pos->pmk_len == pmk_len &&
          wf_os_memcmp_const(pos->pmk, pmk, pmk_len) == 0 &&
            wf_os_memcmp_const(pos->pmkid, entry->pmkid,
                               PMKID_LEN) == 0) {
                                 wpa_printf(MSG_DEBUG, "WPA: reusing previous "
                                            "PMKSA entry");
                                 os_free(entry);
                                 return pos;
                               }
      if (prev == NULL)
        pmksa->pmksa = pos->next;
      else
        prev->next = pos->next;
      
      wpa_printf(MSG_DEBUG, "RSN: Replace PMKSA entry for "
                 "the current AP and any PMKSA cache entry "
                   "that was based on the old PMK");
      if (!pos->opportunistic)
        wf_wpa_pmksa_cache_flush(pmksa, network_ctx, pos->pmk,
                                 pos->pmk_len);
      wf_pmksa_cache_free_entry(pmksa, pos, PMKSA_REPLACE);
      break;
    }
    prev = pos;
    pos = pos->next;
  }
  
  if (pmksa->pmksa_count >= 32 && pmksa->pmksa) {
    pos = pmksa->pmksa;
    
    if (pos == pmksa->sm->cur_pmksa) {
      pos = pos->next;
      pmksa->pmksa->next = pos ? pos->next : NULL;
    } else
      pmksa->pmksa = pos->next;
    
    if (pos) {
      wpa_printf(MSG_DEBUG, "RSN: removed the oldest idle "
                 "PMKSA cache entry (for " MACSTR ") to "
                   "make room for new one",
                   MAC2STR(pos->aa));
      wf_pmksa_cache_free_entry(pmksa, pos, PMKSA_FREE);
    }
  }
  
  pos = pmksa->pmksa;
  prev = NULL;
  while (pos) {
    if (pos->expiration > entry->expiration)
      break;
    prev = pos;
    pos = pos->next;
  }
  if (prev == NULL) {
    entry->next = pmksa->pmksa;
    pmksa->pmksa = entry;
  } else {
    entry->next = prev->next;
    prev->next = entry;
  }
  pmksa->pmksa_count++;
  wpa_printf(MSG_DEBUG, "RSN: Added PMKSA cache entry for " MACSTR
             " network_ctx=%p", MAC2STR(entry->aa), network_ctx);
  
  return entry;
}

void wf_wpa_sm_set_pmk_from_pmksa(struct wpa_sm *sm)
{
  if (sm == NULL)
    return;
  
  if (sm->cur_pmksa) {
    sm->pmk_len = sm->cur_pmksa->pmk_len;
    os_memcpy(sm->pmk, sm->cur_pmksa->pmk, sm->pmk_len);
  } else {
    sm->pmk_len = PMK_LEN;
    os_memset(sm->pmk, 0, PMK_LEN);
  }
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
  if (wpa_ie == NULL) {
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
  if (rbuf == NULL) {
    os_free(rsn_ie_buf);
    return -1;
  }
  
  reply192 = (struct wpa_eapol_key_192 *) reply;
  if(sec_info->rsn_enable == 1)
  {
    reply->type = EAPOL_KEY_TYPE_RSN;
  }
  else if(sec_info->wpa_enable == 1)
  {
    reply->type = EAPOL_KEY_TYPE_WPA;
  }
  
  WPA_PUT_BE16(reply->key_info,
               ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC);
  if (sm->proto == WPA_PROTO_RSN || sm->proto == WPA_PROTO_OSEN)
    WPA_PUT_BE16(reply->key_length, 0);
  else
    os_memcpy(reply->key_length, key->key_length, 2);
  os_memcpy(reply->replay_counter, key->replay_counter,
            WPA_REPLAY_COUNTER_LEN);
  wpa_hexdump(MSG_DEBUG, "WPA: Replay Counter", reply->replay_counter,
              WPA_REPLAY_COUNTER_LEN);
  
  key_mic = reply192->key_mic;
  
  if (mic_len == 24) {
    WPA_PUT_BE16(reply192->key_data_length, wpa_ie_len);
    os_memcpy(reply192 + 1, wpa_ie, wpa_ie_len);
  } else {
    WPA_PUT_BE16(reply->key_data_length, wpa_ie_len);
    os_memcpy(reply + 1, wpa_ie, wpa_ie_len);
  }
  
  os_free(rsn_ie_buf);
  
  os_memcpy(reply->key_nonce, nonce, WPA_NONCE_LEN);
  
  
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
        pos[2 + WPA_LEN_SELECTOR] == 1 && pos[2 + WPA_LEN_SELECTOR + 1] == 0) {
          ie->wpa_ie = pos;
          ie->wpa_ie_len = pos[1] + 2;
          wpa_hexdump(MSG_DEBUG, "WPA: WPA IE in EAPOL-Key",
                      ie->wpa_ie, ie->wpa_ie_len);
          return 0;
	}
  
  if (pos + 1 + _SELECTOR_LEN_TO_RSN < end &&
      pos[1] >= _SELECTOR_LEN_TO_RSN + TK_PMK_ID_LEN &&
        RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_PMKID) {
          ie->pmkid = pos + 2 + _SELECTOR_LEN_TO_RSN;
          wpa_hexdump(MSG_DEBUG, "WPA: PMKID in EAPOL-Key", pos, pos[1] + 2);
          return 0;
	}
  
  if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
      RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_GROUPKEY) {
        ie->gtk = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->gtk_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump_key(MSG_DEBUG, "WPA: GTK in EAPOL-Key", pos, pos[1] + 2);
        return 0;
      }
  
  if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
      RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_MAC_ADDR) {
        ie->mac_addr = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->mac_addr_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump(MSG_DEBUG, "WPA: MAC Address in EAPOL-Key",
                    pos, pos[1] + 2);
        return 0;
      }
  
  if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
      RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_SMK) {
        ie->smk = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->smk_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump_key(MSG_DEBUG, "WPA: SMK in EAPOL-Key", pos, pos[1] + 2);
        return 0;
      }
  
  if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
      RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_NONCE) {
        ie->nonce = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->nonce_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump(MSG_DEBUG, "WPA: Nonce in EAPOL-Key", pos, pos[1] + 2);
        return 0;
      }
  
  if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
      RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_LIFETIME) {
        ie->lifetime = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->lifetime_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump(MSG_DEBUG, "WPA: Lifetime in EAPOL-Key", pos, pos[1] + 2);
        return 0;
      }
  
  if (pos[1] > _SELECTOR_LEN_TO_RSN + 2 &&
      RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_ERROR) {
        ie->error = pos + 2 + _SELECTOR_LEN_TO_RSN;
        ie->error_len = pos[1] - _SELECTOR_LEN_TO_RSN;
        wpa_hexdump(MSG_DEBUG, "WPA: Error in EAPOL-Key", pos, pos[1] + 2);
        return 0;
      }
  
  
  return 0;
}

static int wf_wpa_parse_vendor_specific(const wf_u8 * pos, const wf_u8 * end,
                                        struct wpa_eapol_ie_parse *ie)
{
  unsigned int oui;
  
  if (pos[1] < 4) {
    wpa_printf(MSG_MSGDUMP, "Too short vendor specific IE ignored (len=%u)",
               pos[1]);
    return 1;
  }
  
  oui = WPA_GET_BE24(&pos[2]);
  if (oui == OUI_MS && pos[5] == WMM_TYP_OUI && pos[1] > 4) {
    if (pos[6] == WMM_CHILDTYPE_INFORMATION_ELEMENT_OUI) {
      ie->wmm = &pos[2];
      ie->wmm_len = pos[1];
      wpa_hexdump(MSG_DEBUG, "WPA: WMM IE", ie->wmm, ie->wmm_len);
    } else if (pos[6] == WMM_CHILDTYPE_PARAMETER_ELEMENT_OUI) {
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
  
  os_memset(ie, 0, sizeof(*ie));
  for (pos = buf, end = pos + len; pos + 1 < end; pos += 2 + pos[1]) {
    if (pos[0] == 0xdd && ((pos == buf + len - 1) || pos[1] == 0)) {
      
      break;
    }
    if (pos + 2 + pos[1] > end) {
      wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key Key Data "
                 "underflow (ie=%d len=%d pos=%d)",
                 pos[0], pos[1], (int)(pos - buf));
      wpa_hexdump_key(MSG_DEBUG, "WPA: Key Data", buf, len);
      ret = -1;
      break;
    }
    if (*pos == WLAN_RSN_EID) {
      ie->rsn_ie = pos;
      ie->rsn_ie_len = pos[1] + 2;
      
    } else if (*pos == WLAN_MOBILITY_FIELD_EID &&
               pos[1] >= sizeof(struct rsn_mdie)) {
                 ie->mdie = pos;
                 ie->mdie_len = pos[1] + 2;
                 
               } else if (*pos == WLAN_FAST_BSS_CHANGE_EID &&
                          pos[1] >= sizeof(struct rsn_ftie)) {
                            ie->ftie = pos;
                            ie->ftie_len = pos[1] + 2;
                            
                          } else if (*pos == WLAN_OVERTIME_INTERVAL_EID && pos[1] >= 5) {
                            if (pos[2] == WLAN_TIMEOUT_REASSOC_DEADLINE) {
                              ie->reassoc_deadline = pos;
                              wpa_hexdump(MSG_DEBUG, "WPA: Reassoc Deadline "
                                          "in EAPOL-Key", ie->reassoc_deadline, pos[1] + 2);
                            } else if (pos[2] == WLAN_TIMEOUT_KEY_LIFETIME) {
                              ie->key_lifetime = pos;
                              wpa_hexdump(MSG_DEBUG, "WPA: KeyLifetime "
                                          "in EAPOL-Key", ie->key_lifetime, pos[1] + 2);
                            } else {
                              wpa_hexdump(MSG_DEBUG, "WPA: Unrecognized "
                                          "EAPOL-Key Key Data IE", pos, 2 + pos[1]);
                            }
                          } else if (*pos == WLAN_EID_LINK_ID) {
                            if (pos[1] >= 18) {
                              ie->lnkid = pos;
                              ie->lnkid_len = pos[1] + 2;
                            }
                          } else if (*pos == WLAN_EID_EXT_CAPAB) {
                            ie->ext_capab = pos;
                            ie->ext_capab_len = pos[1] + 2;
                          } else if (*pos == WLAN_SUPP_RATES_EID) {
                            ie->supp_rates = pos;
                            ie->supp_rates_len = pos[1] + 2;
                          } else if (*pos == WLAN_EXT_SUPP_RATES_EID) {
                            ie->ext_supp_rates = pos;
                            ie->ext_supp_rates_len = pos[1] + 2;
                          } else if (*pos == WLAN_HT_CAP_EID &&
                                     pos[1] >= sizeof(struct ieee80211_ht_capabilities)) {
                                       ie->ht_capabilities = pos + 2;
                                     } else if (*pos == WLAN_EID_VHT_AID) {
                                       if (pos[1] >= 2)
                                         ie->aid = WPA_GET_LE16(pos + 2) & 0x3fff;
                                     } else if (*pos == WLAN_EID_VHT_CAP &&
                                                pos[1] >= sizeof(struct ieee80211_vht_capabilities)) {
                                                  ie->vht_capabilities = pos + 2;
                                                } else if (*pos == WLAN_EID_QOS && pos[1] >= 1) {
                                                  ie->qosinfo = pos[2];
                                                } else if (*pos == WLAN_SUSTAINS_CHANNELS_EID) {
                                                  ie->supp_channels = pos + 2;
                                                  ie->supp_channels_len = pos[1];
                                                } else if (*pos == WLAN_EID_SUPPORTED_OPERATING_CLASSES) {
                                                  
                                                  if (pos[1] >= 2 && pos[1] <= 253) {
                                                    ie->supp_oper_classes = pos + 2;
                                                    ie->supp_oper_classes_len = pos[1];
                                                  }
                                                } else if (*pos == WLAN_VENDOR_SPECIFIC_EID) {
                                                  ret = wf_wpa_parse_generic(pos, end, ie);
                                                  if (ret < 0)
                                                    break;
                                                  if (ret > 0) {
                                                    ret = 0;
                                                    break;
                                                  }
                                                  
                                                  ret = wf_wpa_parse_vendor_specific(pos, end, ie);
                                                  if (ret < 0)
                                                    break;
                                                  if (ret > 0) {
                                                    ret = 0;
                                                    break;
                                                  }
                                                } else {
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
  
  os_memset(&ie, 0, sizeof(ie));
  
  if (sm->proto == WPA_PROTO_RSN || sm->proto == WPA_PROTO_OSEN) {
    wpa_hexdump(MSG_DEBUG, "RSN: msg 1/4 key data",
                key_data, key_data_len);
    if (wf_wpa_supplicant_parse_ies(key_data, key_data_len, &ie) < 0)
      goto failed;
    if (ie.pmkid) {
      wpa_hexdump(MSG_DEBUG, "RSN: PMKID from "
                  "Authenticator", ie.pmkid, PMKID_LEN);
    }
  }
  
  if (sm->renew_snonce) {
    if (wf_random_get_bytes(sm->snonce, WPA_NONCE_LEN)) {
      wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
              "WPA: Failed to get random data for SNonce");
      goto failed;
    }
    sm->renew_snonce = 0;
    wpa_hexdump(MSG_DEBUG, "WPA: Renewed SNonce",
                sm->snonce, WPA_NONCE_LEN);
  }
  
  ptk = &sm->tptk;
  wf_wpa_derive_ptk(sm, src_addr, key, ptk);
  if (sm->pairwise_cipher == WPA_CIPHER_TKIP) {
    wf_u8 buf[8];
    os_memcpy(buf, &ptk->tk[16], 8);
    os_memcpy(&ptk->tk[16], &ptk->tk[24], 8);
    os_memcpy(&ptk->tk[24], buf, 8);
    os_memset(buf, 0, sizeof(buf));
  }
  sm->tptk_set = 1;
  pie = (wf_80211_mgmt_ie_t *)sec_info->supplicant_ie;
  
  sm->assoc_wpa_ie_len = pie->len +2;
  
  //memcpy(sm->assoc_wpa_ie,sec_info->supplicant_ie,sm->assoc_wpa_ie_len);
  sm->assoc_wpa_ie = sec_info->supplicant_ie;
  kde = sm->assoc_wpa_ie ;
  kde_len = sm->assoc_wpa_ie_len;
  
  
  if (wf_wpa_supplicant_send_2_of_4(sm, sm->bssid, key, ver, sm->snonce,
                                    kde, kde_len, ptk) < 0)
    goto failed;
  
  os_free(kde_buf);
  os_memcpy(sm->anonce, key->key_nonce, WPA_NONCE_LEN);
  return;
  
failed:
  os_free(kde_buf);
  
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
  
  
  if (sm->cur_pmksa && sm->cur_pmksa->opportunistic) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "RSN: Authenticator accepted "
              "opportunistic PMKSA entry - marking it valid");
    sm->cur_pmksa->opportunistic = 0;
  }
  
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
  
  if (sm->pairwise_cipher == WPA_CIPHER_NONE) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: Pairwise Cipher "
            "Suite: NONE - do not use pairwise keys");
    return 0;
  }
  
  if (!wf_wpa_cipher_valid_pairwise(sm->pairwise_cipher)) {
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
            "WPA: Unsupported pairwise cipher %d",
            sm->pairwise_cipher);
    return -1;
  }
  
  alg = wf_wpa_cipher_to_alg(sm->pairwise_cipher);
  keylen = wf_wpa_cipher_key_len(sm->pairwise_cipher);
  rsclen = wf_wpa_cipher_rsc_len(sm->pairwise_cipher);
  
  if (sm->proto == WPA_PROTO_RSN || sm->proto == WPA_PROTO_OSEN) {
    key_rsc = null_rsc;
  } else {
    key_rsc = key->key_rsc;
    wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, rsclen);
  }
  
  if (wf_wpa_sm_set_key(sm, alg, sm->bssid, 0, 1, key_rsc, rsclen,
                        sm->ptk.tk, keylen) < 0) {
                          wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                                  "WPA: Failed to set PTK to the "
                                    "driver (alg=%d keylen=%d bssid=" MACSTR ")",
                                    alg, keylen, MAC2STR(sm->bssid));
                          return -1;
                        }
  
  os_memset(sm->ptk.tk, 0, WPA_TK_MAX_LEN);
  
  
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
  if (*alg == WPA_ALG_NONE) {
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
            "WPA: Unsupported Group Cipher %d",
            group_cipher);
    return -1;
  }
  *key_rsc_len = wf_wpa_cipher_rsc_len(group_cipher);
  
  klen = wf_wpa_cipher_key_len(group_cipher);
  if (keylen != klen || maxkeylen < klen) {
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
  if (sm->group_cipher == WPA_CIPHER_TKIP) {
    os_memcpy(gtk_buf, gd->gtk, 16);
    os_memcpy(gtk_buf + 16, gd->gtk + 24, 8);
    os_memcpy(gtk_buf + 24, gd->gtk + 16, 8);
    _gtk = gtk_buf;
  }
  if (sm->pairwise_cipher == WPA_CIPHER_NONE) {
    if (wf_wpa_sm_set_key(sm, gd->alg, NULL,
                          gd->keyidx, 1, key_rsc, gd->key_rsc_len,
                          _gtk, gd->gtk_len) < 0) {
                            wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                                    "WPA: Failed to set GTK to the driver "
                                      "(Group only)");
                            os_memset(gtk_buf, 0, sizeof(gtk_buf));
                            return -1;
                          }
  } else if (wf_wpa_sm_set_key(sm, gd->alg, broadcast_ether_addr,
                               gd->keyidx, gd->tx, key_rsc, gd->key_rsc_len,
                               _gtk, gd->gtk_len) < 0) {
                                 wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
                                         "WPA: Failed to set GTK to "
                                           "the driver (alg=%d keylen=%d keyidx=%d)",
                                           gd->alg, gd->gtk_len, gd->keyidx);
                                 os_memset(gtk_buf, 0, sizeof(gtk_buf));
                                 return -1;
                               }
  os_memset(gtk_buf, 0, sizeof(gtk_buf));
  
  return 0;
}


static int wf_wpa_supplicant_gtk_tx_bit_workaround(const struct wpa_sm *sm,
                                                   int tx)
{
  if (tx && sm->pairwise_cipher != WPA_CIPHER_NONE) {
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
  
  if (rsclen == 6 && ((rsc[5] && !rsc[0]) || rsc[6] || rsc[7])) {
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
  
  os_memset(&gd, 0, sizeof(gd));
  wpa_hexdump_key(MSG_DEBUG, "RSN: received GTK in pairwise handshake",
                  gtk, gtk_len);
  
  if (gtk_len < 2 || gtk_len - 2 > sizeof(gd.gtk))
    return -1;
  
  gd.keyidx = gtk[0] & 0x3;
  gd.tx = wf_wpa_supplicant_gtk_tx_bit_workaround(sm,
                                                  !!(gtk[0] & BIT(2)));
  gtk += 2;
  gtk_len -= 2;
  
  os_memcpy(gd.gtk, gtk, gtk_len);
  gd.gtk_len = gtk_len;
  
  key_rsc = key->key_rsc;
  if (wf_wpa_supplicant_rsc_relaxation(sm, key->key_rsc))
    key_rsc = null_rsc;
  wf_wpa_supplicant_install_gtk(sm, &gd, key_rsc);
  if (sm->group_cipher != WPA_CIPHER_GTK_NOT_USED &&
      (wf_wpa_supplicant_check_group_cipher(sm, sm->group_cipher,
                                            gtk_len, gtk_len,
                                            &gd.key_rsc_len, &gd.alg) ||
       wf_wpa_supplicant_install_gtk(sm, &gd, key_rsc))) {
         wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
                 "RSN: Failed to install GTK");
         os_memset(&gd, 0, sizeof(gd));
         return -1;
       }
  os_memset(&gd, 0, sizeof(gd));
  
  wf_wpa_supplicant_key_neg_complete(sm, sm->bssid,
                                     key_info & WPA_KEY_INFO_SECURE);
  return 0;
}


static void wf_wpa_report_ie_mismatch(struct wpa_sm *sm,
                                      const char *reason, const wf_u8 *src_addr,
                                      const wf_u8 *wpa_ie, size_t wpa_ie_len,
                                      const wf_u8 *rsn_ie, size_t rsn_ie_len)
{
  wpa_msg(sm->ctx->msg_ctx, MSG_WARNING, "WPA: %s (src=" MACSTR ")",
          reason, MAC2STR(src_addr));
  
  if (sm->ap_wpa_ie) {
    wpa_hexdump(MSG_INFO, "WPA: WPA IE in Beacon/ProbeResp",
                sm->ap_wpa_ie, sm->ap_wpa_ie_len);
  }
  if (wpa_ie) {
    if (!sm->ap_wpa_ie) {
      wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
              "WPA: No WPA IE in Beacon/ProbeResp");
    }
    wpa_hexdump(MSG_INFO, "WPA: WPA IE in 3/4 msg",
                wpa_ie, wpa_ie_len);
  }
  
  if (sm->ap_rsn_ie) {
    wpa_hexdump(MSG_INFO, "WPA: RSN IE in Beacon/ProbeResp",
                sm->ap_rsn_ie, sm->ap_rsn_ie_len);
  }
  if (rsn_ie) {
    if (!sm->ap_rsn_ie) {
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
  if (sm->ap_wpa_ie == NULL && sm->ap_rsn_ie == NULL) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "WPA: No WPA/RSN IE for this AP known. "
              "Trying to get from scan results");
    
  }
  
  if (ie->wpa_ie == NULL && ie->rsn_ie == NULL &&
      (sm->ap_wpa_ie || sm->ap_rsn_ie)) {
        wf_wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                                  "with IE in Beacon/ProbeResp (no IE?)",
                                  src_addr, ie->wpa_ie, ie->wpa_ie_len,
                                  ie->rsn_ie, ie->rsn_ie_len);
        return -1;
      }
  if (sm->proto == WPA_PROTO_WPA &&
      ie->rsn_ie && sm->ap_rsn_ie == NULL && sm->rsn_enabled) {
        wf_wpa_report_ie_mismatch(sm, "Possible downgrade attack "
                                  "detected - RSN was enabled and RSN IE "
                                    "was in msg 3/4, but not in "
                                      "Beacon/ProbeResp",
                                      src_addr, ie->wpa_ie, ie->wpa_ie_len,
                                      ie->rsn_ie, ie->rsn_ie_len);
        return -1;
      }
  
  return 0;
}

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
os_memcpy(reply->key_length, key->key_length, 2);
os_memcpy(reply->replay_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);

key_mic = reply192->key_mic;
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
  if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
            "WPA: GTK IE in unencrypted key data");
    goto failed;
  }
  
  if (wf_wpa_supplicant_validate_ie(sm, sm->bssid, &ie) < 0)
    goto failed;
  
  if (os_memcmp(sm->anonce, key->key_nonce, WPA_NONCE_LEN) != 0) {
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
            "WPA: ANonce from message 1 of 4-Way Handshake "
              "differs from 3 of 4-Way Handshake - drop packet (src="
                MACSTR ")", MAC2STR(sm->bssid));
    goto failed;
  }
  
  keylen = WPA_GET_BE16(key->key_length);
  if (keylen != wf_wpa_cipher_key_len(sm->pairwise_cipher)) {
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
            "WPA: Invalid %s key length %d (src=" MACSTR
              ")", wf_wpa_cipher_txt(sm->pairwise_cipher), keylen,
              MAC2STR(sm->bssid));
    goto failed;
  }
  
  
  if (wf_wpa_supplicant_send_4_of_4(sm, sm->bssid, key, ver, key_info,
                                    &sm->ptk) < 0) {
                                      goto failed;
                                    }
  sm->renew_snonce = 1;
  
  if (key_info & WPA_KEY_INFO_INSTALL) {
    if (wf_wpa_supplicant_install_ptk(sm, key))
      goto failed;
  }
  
  
  wf_wpa_sm_set_state(sm, WPA_GROUP_HANDSHAKE);
  
  if (sm->group_cipher == WPA_CIPHER_GTK_NOT_USED) {
    wf_wpa_supplicant_key_neg_complete(sm, sm->bssid,
                                       key_info & WPA_KEY_INFO_SECURE);
  } else if (ie.gtk &&
             wf_wpa_supplicant_pairwise_gtk(sm, key,
                                            ie.gtk, ie.gtk_len, key_info) < 0) {
                                              wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
                                                      "RSN: Failed to configure GTK");
                                              goto failed;
                                            }
  
  
  if (sm->proto == WPA_PROTO_RSN && wpa_key_mgmt_suite_b(sm->key_mgmt)) {
    struct rsn_pmksa_cache_entry *sa;
    
    sa = wf_wpa_pmksa_cache_add(sm->pmksa, sm->pmk, sm->pmk_len, NULL,
                                sm->ptk.kck, sm->ptk.kck_len,
                                sm->bssid, sm->own_addr,
                                sm->network_ctx, sm->key_mgmt);
    if (!sm->cur_pmksa)
      sm->cur_pmksa = sa;
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
  if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
            "WPA: GTK IE in unencrypted key data");
    return -1;
  }
  if (ie.gtk == NULL) {
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
  if (ie.gtk_len - 2 > sizeof(gd->gtk)) {
    wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
            "RSN: Too long GTK in GTK IE (len=%lu)",
            (unsigned long) ie.gtk_len - 2);
    return -1;
  }
  os_memcpy(gd->gtk, ie.gtk + 2, ie.gtk_len - 2);
  
  
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
  if(gtk_len == 32)
  {
    sm->group_cipher = WPA_CIPHER_TKIP;
  }
  else if(gtk_len == 16)
  {
    sm->group_cipher = WPA_CIPHER_CCMP;
  }
  maxkeylen = key_data_len;
  if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
    if (maxkeylen < 8) {
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
  if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 && sm->ptk.kek_len == 16) {
    wf_u8 ek[32];
    if (key_data_len > sizeof(gd->gtk)) {
      wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
              "WPA: RC4 key data too long (%lu)",
              (unsigned long) key_data_len);
      return -1;
    }
    os_memcpy(ek, key->key_iv, 16);
    os_memcpy(ek + 16, sm->ptk.kek, sm->ptk.kek_len);
    os_memcpy(gd->gtk, key_data, key_data_len);
    if (wf_rc4_skip(ek, 32, 256, gd->gtk, key_data_len)) {
      os_memset(ek, 0, sizeof(ek));
      return -1;
    }
    os_memset(ek, 0, sizeof(ek));
    
  } else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
    if (maxkeylen % 8) {
      return -1;
    }
    if (maxkeylen > sizeof(gd->gtk)) {
      return -1;
    }
    if (wf_aes_unwrap(sm->ptk.kek, sm->ptk.kek_len, maxkeylen / 8,
                      key_data, gd->gtk)) {
                        return -1;
                      }
  } else {
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
os_memcpy(reply->key_length, key->key_length, 2);
os_memcpy(reply->replay_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);

key_mic = reply192->key_mic;
if (mic_len == 24)
WPA_PUT_BE16(reply192->key_data_length, 0);
else
WPA_PUT_BE16(reply->key_data_length, 0);

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
  
  if (!sm->msg_3_of_4_ok) {
    goto failed;
  }
  
  os_memset(&gd, 0, sizeof(gd));
  
  rekey = wf_wpa_sm_get_state() == WPA_COMPLETED;
  
  key_info = WPA_GET_BE16(key->key_info);
  
  if (sm->proto == WPA_PROTO_RSN || sm->proto == WPA_PROTO_OSEN) {
    ret = wf_wpa_supplicant_process_1_of_2_rsn(sm, key_data,
                                               key_data_len, key_info,
                                               &gd);
  } else {
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
  os_memset(&gd, 0, sizeof(gd));
  
  if (rekey) {
    wf_wpa_sm_set_state(sm, WPA_COMPLETED);
  } else {
    wf_wpa_supplicant_key_neg_complete(sm, sm->bssid,
                                       key_info &
                                         WPA_KEY_INFO_SECURE);
  }
  
  
  return;
  
failed:
  os_memset(&gd, 0, sizeof(gd));
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
  
  os_memcpy(mic, key->key_mic, mic_len);
  if (sm->tptk_set) {
    os_memset(key->key_mic, 0, mic_len);
    
    
    wf_wpa_eapol_key_mic(sm->tptk.kck, sm->tptk.kck_len, sm->key_mgmt,
                         ver, buf, len, key->key_mic);
    
    
    if (wf_os_memcmp_const(mic, key->key_mic, mic_len) != 0) {
      wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
              "WPA: Invalid EAPOL-Key MIC "
                "when using TPTK - ignoring TPTK");
    } else {
      ok = 1;
      sm->tptk_set = 0;
      sm->ptk_set = 1;
      os_memcpy(&sm->ptk, &sm->tptk, sizeof(sm->ptk));
      os_memset(&sm->tptk, 0, sizeof(sm->tptk));
    }
  }
  
  if (!ok && sm->ptk_set) {
    os_memset(key->key_mic, 0, mic_len);
    wf_wpa_eapol_key_mic(sm->ptk.kck, sm->ptk.kck_len, sm->key_mgmt,
                         ver, buf, len, key->key_mic);
    if (wf_os_memcmp_const(mic, key->key_mic, mic_len) != 0) {
      wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
              "WPA: Invalid EAPOL-Key MIC - "
                "dropping packet");
      return -1;
    }
    ok = 1;
  }
  
  if (!ok) {
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
            "WPA: Could not verify EAPOL-Key MIC - "
              "dropping packet");
    return -1;
  }
  
  os_memcpy(sm->rx_replay_counter, key->replay_counter,
            WPA_REPLAY_COUNTER_LEN);
  sm->rx_replay_counter_set = 1;
  return 0;
}

static int wf_wpa_supplicant_decrypt_key_data(struct wpa_sm *sm,
                                              struct wpa_eapol_key *key, wf_u16 ver,
                                              wf_u8 *key_data, size_t *key_data_len)
{
  if (!sm->ptk_set) {
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
            "WPA: PTK not available, cannot decrypt EAPOL-Key Key "
              "Data");
    return -1;
  }
  
  if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 && sm->ptk.kek_len == 16) {
    wf_u8 ek[32];
    os_memcpy(ek, key->key_iv, 16);
    os_memcpy(ek + 16, sm->ptk.kek, sm->ptk.kek_len);
    if (wf_rc4_skip(ek, 32, 256, key_data, *key_data_len)) {
      os_memset(ek, 0, sizeof(ek));
      wpa_msg(sm->ctx->msg_ctx, MSG_ERROR,
              "WPA: RC4 failed");
      return -1;
    }
    os_memset(ek, 0, sizeof(ek));
  } else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES ||
             ver == WPA_KEY_INFO_TYPE_AES_128_CMAC ||
               sm->key_mgmt == WPA_KEY_MGMT_OSEN ||
                 wpa_key_mgmt_suite_b(sm->key_mgmt)) {
                   wf_u8 *buf;
                   if (*key_data_len < 8 || *key_data_len % 8) {
                     return -1;
                   }
                   *key_data_len -= 8;
                   buf = os_malloc(*key_data_len);
                   if (buf == NULL) {
                     return -1;
                   }
                   if (wf_aes_unwrap(sm->ptk.kek, sm->ptk.kek_len, *key_data_len / 8,
                                     key_data, buf)) {
                                       wf_os_bin_clear_free(buf, *key_data_len);
                                       return -1;
                                     }
                   os_memcpy(key_data, buf, *key_data_len);
                   wf_os_bin_clear_free(buf, *key_data_len);
                   WPA_PUT_BE16(key->key_data_length, *key_data_len);
                 } else {
                   return -1;
                 }
  return 0;
}


static int wf_wpa_supplicant_process_smk_error(struct wpa_sm *sm,
                                               const unsigned char *src_addr,
                                               const struct wpa_eapol_key *key,
                                               size_t extra_len)
{
  struct wpa_eapol_ie_parse kde;
  struct rsn_error_kde error;
  wf_u8 peer[ETH_ALEN];
  wf_u16 error_type;
  
  wpa_printf(MSG_DEBUG, "RSN: Received SMK Error");
  
  if (!sm->peerkey_enabled || sm->proto != WPA_RSN_PROTO) {
    wpa_printf(MSG_DEBUG, "RSN: SMK handshake not allowed for "
               "the current network");
    return -1;
  }
  
  if (wf_wpa_supplicant_parse_ies((const wf_u8 *)(key + 1), extra_len, &kde) < 0) {
    wpa_printf(MSG_INFO, "RSN: Failed to parse KDEs in SMK Error");
    return -1;
  }
  
  if (kde.error == NULL || kde.error_len < sizeof(error)) {
    wpa_printf(MSG_INFO, "RSN: No Error KDE in SMK Error");
    return -1;
  }
  
  if (kde.mac_addr && kde.mac_addr_len >= ETH_ALEN)
    os_memcpy(peer, kde.mac_addr, ETH_ALEN);
  else
    os_memset(peer, 0, ETH_ALEN);
  os_memcpy(&error, kde.error, sizeof(error));
  error_type = be_to_host16(error.error_type);
  wpa_msg(sm->ctx->msg_ctx, MSG_INFO,
          "RSN: SMK Error KDE received: MUI %d error_type %d peer "
            MACSTR, be_to_host16(error.mui), error_type, MAC2STR(peer));
  
  
  return 0;
}

static wf_u8 *wf_wpa_add_kde(wf_u8 * pos, wf_u32 kde, const wf_u8 * data, size_t data_len)
{
  *pos++ = WLAN_VENDOR_SPECIFIC_EID;
  *pos++ = _SELECTOR_LEN_TO_RSN + data_len;
  TILK_SELE_CTOR_RSN__PUT(pos, kde);
  pos += _SELECTOR_LEN_TO_RSN;
  os_memcpy(pos, data, data_len);
  pos += data_len;
  return pos;
}

static int wf_wpa_supplicant_send_smk_error(struct wpa_sm *sm, const wf_u8 * dst,
                                            const wf_u8 * peer,
                                            wf_u16 mui, wf_u16 error_type, int ver)
{
  size_t rlen;
  struct wpa_eapol_key *err;
  struct wpa_eapol_key_192 *err192;
  struct rsn_error_kde error;
  wf_u8 *rbuf, *pos;
  size_t kde_len;
  wf_u16 key_info;
  
  kde_len = 2 + _SELECTOR_LEN_TO_RSN + sizeof(error);
  if (peer)
    kde_len += 2 + _SELECTOR_LEN_TO_RSN + ETH_ALEN;
  
  rbuf = wf_wpa_sm_alloc_eapol((const struct wpa_supplicant *)sm, IEEE802_1X_TYPE_EAPOL_KEY,
                               NULL, sizeof(*err) + kde_len, &rlen,
                               (void *)&err);
  if (rbuf == NULL)
    return -1;
  err192 = (struct wpa_eapol_key_192 *)err;
  
  err->type = EAPOL_KEY_TYPE_RSN;
  key_info = ver | WPA_KEY_INFO_SMK_MESSAGE | WPA_KEY_INFO_MIC |
    WPA_KEY_INFO_SECURE | WPA_KEY_INFO_ERROR | WPA_KEY_INFO_REQUEST;
  WPA_PUT_BE16(err->key_info, key_info);
  WPA_PUT_BE16(err->key_length, 0);
  os_memcpy(err->replay_counter, sm->request_counter, WPA_REPLAY_COUNTER_LEN);
  wf_os_inc_byte_array(sm->request_counter, WPA_REPLAY_COUNTER_LEN);
  
  WPA_PUT_BE16(err->key_data_length, (wf_u16) kde_len);
  pos = (wf_u8 *) (err + 1);
  
  if (peer) {
    
    pos = wf_wpa_add_kde(pos, RSN_KEY_DATA_MAC_ADDR, peer, ETH_ALEN);
  }
  
  error.mui = host_to_be16(mui);
  error.error_type = host_to_be16(error_type);
  wf_wpa_add_kde(pos, RSN_KEY_DATA_ERROR, (wf_u8 *) & error, sizeof(error));
  
  if (peer) {
    wpa_printf(MSG_DEBUG, "RSN: Sending EAPOL-Key SMK Error (peer "
               MACSTR " mui %d error_type %d)",
               MAC2STR(peer), mui, error_type);
  } else {
    wpa_printf(MSG_DEBUG, "RSN: Sending EAPOL-Key SMK Error "
               "(mui %d error_type %d)", mui, error_type);
  }
  
  wf_wpa_eapol_key_send(sm, sm->ptk.kck, sm->ptk.kck_len, ver, dst,
                        ETH_P_EAPOL, rbuf, rlen, err192->key_mic);
  
  return 0;
}



static wf_u8 *wf_wpa_add_ie(wf_u8 * pos, const wf_u8 * ie, size_t ie_len)
{
  os_memcpy(pos, ie, ie_len);
  return pos + ie_len;
}

static int wf_wpa_supplicant_send_smk_m3(struct wpa_sm *sm,
                                         const unsigned char *src_addr,
                                         const struct wpa_eapol_key *key,
                                         int ver, struct wpa_peerkey *peerkey)
{
  size_t rlen;
  struct wpa_eapol_key *reply;
  struct wpa_eapol_key_192 *reply192;
  wf_u8 *rbuf, *pos;
  size_t kde_len;
  wf_u16 key_info;
  
  kde_len = peerkey->rsnie_p_len +
    2 + _SELECTOR_LEN_TO_RSN + ETH_ALEN +
      2 + _SELECTOR_LEN_TO_RSN + WPA_LEN_NONCE;
  
  rbuf = wf_wpa_sm_alloc_eapol((const struct wpa_supplicant *)sm, IEEE802_1X_TYPE_EAPOL_KEY,
                               NULL, sizeof(*reply) + kde_len, &rlen,
                               (void *)&reply);
  if (rbuf == NULL)
    return -1;
  reply192 = (struct wpa_eapol_key_192 *)reply;
  
  reply->type = EAPOL_KEY_TYPE_RSN;
  key_info = ver | WPA_KEY_INFO_SMK_MESSAGE | WPA_KEY_INFO_MIC |
    WPA_KEY_INFO_SECURE;
  WPA_PUT_BE16(reply->key_info, key_info);
  WPA_PUT_BE16(reply->key_length, 0);
  os_memcpy(reply->replay_counter, key->replay_counter,
            WPA_REPLAY_COUNTER_LEN);
  
  os_memcpy(reply->key_nonce, peerkey->pnonce, WPA_LEN_NONCE);
  
  WPA_PUT_BE16(reply->key_data_length, (wf_u16) kde_len);
  pos = (wf_u8 *) (reply + 1);
  
  pos = wf_wpa_add_ie(pos, peerkey->rsnie_p, peerkey->rsnie_p_len);
  
  pos = wf_wpa_add_kde(pos, RSN_KEY_DATA_MAC_ADDR, peerkey->addr, ETH_ALEN);
  
  wf_wpa_add_kde(pos, RSN_KEY_DATA_NONCE, peerkey->inonce, WPA_LEN_NONCE);
  
  wpa_printf(MSG_DEBUG, "RSN: Sending EAPOL-Key SMK M3");
  wf_wpa_eapol_key_send(sm, sm->ptk.kck, sm->ptk.kck_len, ver, src_addr,
                        ETH_P_EAPOL, rbuf, rlen, reply192->key_mic);
  
  return 0;
}


static int wf_wpa_supplicant_process_smk_m2(struct wpa_sm *sm,
                                            const unsigned char *src_addr,
                                            const struct wpa_eapol_key *key,
                                            size_t extra_len, int ver)
{
  struct wpa_peerkey *peerkey;
  struct wpa_eapol_ie_parse kde;
  struct wpa_ie_data ie;
  int cipher;
  struct rsn_hdr_ie *hdr;
  wf_u8 *pos;
  
  wpa_printf(MSG_DEBUG, "RSN: Received SMK M2");
  
  if (!sm->peerkey_enabled || sm->proto != WPA_RSN_PROTO) {
    wpa_printf(MSG_INFO, "RSN: SMK handshake not allowed for "
               "the current network");
    return -1;
  }
  
  if (wf_wpa_supplicant_parse_ies((const wf_u8 *)(key + 1), extra_len, &kde) < 0) {
    wpa_printf(MSG_INFO, "RSN: Failed to parse KDEs in SMK M2");
    return -1;
  }
  
  if (kde.rsn_ie == NULL || kde.mac_addr == NULL ||
      kde.mac_addr_len < ETH_ALEN) {
        wpa_printf(MSG_INFO, "RSN: No RSN IE or MAC address KDE in " "SMK M2");
        return -1;
      }
  
  wpa_printf(MSG_DEBUG, "RSN: SMK M2 - SMK initiator " MACSTR,
             MAC2STR(kde.mac_addr));
  
  if (kde.rsn_ie_len > PEERKEY_MAX_IE_LEN) {
    wpa_printf(MSG_INFO, "RSN: Too long Initiator RSN IE in SMK " "M2");
    return -1;
  }
  
  if (wf_wpa_parse_wpa_ie_rsn(kde.rsn_ie, kde.rsn_ie_len, &ie) < 0) {
    wpa_printf(MSG_INFO, "RSN: Failed to parse RSN IE in SMK M2");
    return -1;
  }
  
  cipher = wf_wpa_pick_pairwise_cipher(ie.pairwise_cipher &
                                       sm->allowed_pairwise_cipher, 0);
  if (cipher < 0) {
    wpa_printf(MSG_INFO, "RSN: No acceptable cipher in SMK M2");
    wf_wpa_supplicant_send_smk_error(sm, src_addr, kde.mac_addr,
                                     STK_MUI_SMK, STK_ERR_CPHR_NS, ver);
    return -1;
  }
  wpa_printf(MSG_DEBUG, "RSN: Using %s for PeerKey", wf_wpa_cipher_txt(cipher));
  
  peerkey = os_zalloc(sizeof(*peerkey));
  if (peerkey == NULL)
    return -1;
  os_memcpy(peerkey->addr, kde.mac_addr, ETH_ALEN);
  os_memcpy(peerkey->inonce, key->key_nonce, WPA_LEN_NONCE);
  os_memcpy(peerkey->rsnie_i, kde.rsn_ie, kde.rsn_ie_len);
  peerkey->rsnie_i_len = kde.rsn_ie_len;
  peerkey->cipher = cipher;
  peerkey->akmp = ie.key_mgmt;
  
  if (wf_os_get_random(peerkey->pnonce, WPA_LEN_NONCE)) {
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
            "WPA: Failed to get random data for PNonce");
    os_free(peerkey);
    return -1;
  }
  
  hdr = (struct rsn_hdr_ie *)peerkey->rsnie_p;
  hdr->elem_id = WLAN_RSN_EID;
  TILK_WPA_TO_LE16(hdr->version, RSN_VERSION);
  pos = (wf_u8 *) (hdr + 1);
  
  TILK_SELE_CTOR_RSN__PUT(pos, TILK_CIPHER_SUITE_CCMP_FOR_RSN);
  pos += _SELECTOR_LEN_TO_RSN;
  
  TILK_WPA_TO_LE16(pos, 1);
  pos += 2;
  TILK_SELE_CTOR_RSN__PUT(pos, wf_wpa_cipher_to_suite(WPA_RSN_PROTO, cipher));
  pos += _SELECTOR_LEN_TO_RSN;
  
  hdr->len = (pos - peerkey->rsnie_p) - 2;
  peerkey->rsnie_p_len = pos - peerkey->rsnie_p;
  wpa_hexdump(MSG_DEBUG, "WPA: RSN IE for SMK handshake",
              peerkey->rsnie_p, peerkey->rsnie_p_len);
  
  wf_wpa_supplicant_send_smk_m3(sm, src_addr, key, ver, peerkey);
  
  peerkey->next = sm->peerkey;
  sm->peerkey = peerkey;
  
  return 0;
}


static int wf_wpa_supplicant_process_smk_m5(struct wpa_sm *sm,
                                            const unsigned char *src_addr,
                                            const struct wpa_eapol_key *key,
                                            int ver,
                                            struct wpa_peerkey *peerkey,
                                            struct wpa_eapol_ie_parse *kde)
{
  int cipher;
  struct wpa_ie_data ie;
  
  wpa_printf(MSG_DEBUG, "RSN: Received SMK M5 (Peer " MACSTR ")",
             MAC2STR(kde->mac_addr));
  if (kde->rsn_ie == NULL || kde->rsn_ie_len > PEERKEY_MAX_IE_LEN ||
      wf_wpa_parse_wpa_ie_rsn(kde->rsn_ie, kde->rsn_ie_len, &ie) < 0) {
        wpa_printf(MSG_INFO, "RSN: No RSN IE in SMK M5");
        
        return -1;
      }
  
  if (os_memcompare(key->key_nonce, peerkey->inonce, WPA_LEN_NONCE) != 0) {
    wpa_printf(MSG_INFO, "RSN: Key Nonce in SMK M5 does "
               "not match with INonce used in SMK M1");
    return -1;
  }
  
  if (os_memcompare(kde->smk + PMK_LEN, peerkey->inonce, WPA_LEN_NONCE) != 0) {
    wpa_printf(MSG_INFO, "RSN: INonce in SMK KDE does not "
               "match with the one used in SMK M1");
    return -1;
  }
  
  os_memcpy(peerkey->rsnie_p, kde->rsn_ie, kde->rsn_ie_len);
  peerkey->rsnie_p_len = kde->rsn_ie_len;
  os_memcpy(peerkey->pnonce, kde->nonce, WPA_LEN_NONCE);
  
  cipher = wf_wpa_pick_pairwise_cipher(ie.pairwise_cipher &
                                       sm->allowed_pairwise_cipher, 0);
  if (cipher < 0) {
    wpa_printf(MSG_INFO, "RSN: SMK Peer STA " MACSTR " selected "
               "unacceptable cipher", MAC2STR(kde->mac_addr));
    wf_wpa_supplicant_send_smk_error(sm, src_addr, kde->mac_addr,
                                     STK_MUI_SMK, STK_ERR_CPHR_NS, ver);
    
    return -1;
  }
  wpa_printf(MSG_DEBUG, "RSN: Using %s for PeerKey", wf_wpa_cipher_txt(cipher));
  peerkey->cipher = cipher;
  
  return 0;
}


static int wf_wpa_supplicant_process_smk_m4(struct wpa_peerkey *peerkey,
                                            struct wpa_eapol_ie_parse *kde)
{
  wpa_printf(MSG_DEBUG, "RSN: Received SMK M4 (Initiator " MACSTR ")",
             MAC2STR(kde->mac_addr));
  
  if (os_memcompare(kde->smk + PMK_LEN, peerkey->pnonce, WPA_LEN_NONCE) != 0) {
    wpa_printf(MSG_INFO, "RSN: PNonce in SMK KDE does not "
               "match with the one used in SMK M3");
    return -1;
  }
  
  if (os_memcompare(kde->nonce, peerkey->inonce, WPA_LEN_NONCE) != 0) {
    wpa_printf(MSG_INFO, "RSN: INonce in SMK M4 did not "
               "match with the one received in SMK M2");
    return -1;
  }
  
  return 0;
}


static void wf_rsn_smkid(const wf_u8 * smk, const wf_u8 * pnonce, const wf_u8 * mac_p,
                         const wf_u8 * inonce, const wf_u8 * mac_i, wf_u8 * smkid, int akmp)
{
  char *title = "SMK Name";
  const wf_u8 *addr[5];
  const size_t len[5] = { 8, WPA_LEN_NONCE, ETH_ALEN, WPA_LEN_NONCE,
  ETH_ALEN
  };
  unsigned char hash[32];
  
  addr[0] = (wf_u8 *) title;
  addr[1] = pnonce;
  addr[2] = mac_p;
  addr[3] = inonce;
  addr[4] = mac_i;
  
  wf_hmac_sha1_vector(smk, PMK_LEN, 5, addr, len, hash);
  os_memcpy(smkid, hash, TK_PMK_ID_LEN);
}


static void wf_wpa_supplicant_send_stk_1_of_4(struct wpa_sm *sm,
                                              struct wpa_peerkey *peerkey)
{
  size_t mlen;
  struct wpa_eapol_key *msg;
  wf_u8 *mbuf;
  size_t kde_len;
  wf_u16 key_info, ver;
  
  kde_len = 2 + _SELECTOR_LEN_TO_RSN + TK_PMK_ID_LEN;
  
  mbuf = wf_wpa_sm_alloc_eapol((const struct wpa_supplicant *)sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                               sizeof(*msg) + kde_len, &mlen,
                               (void *)&msg);
  if (mbuf == NULL)
    return;
  
  msg->type = EAPOL_KEY_TYPE_RSN;
  
  if (peerkey->cipher != WPA_CIPHER_TKIP)
    ver = WPA_KEY_INFO_TYPE_HMAC_SHA1_AES;
  else
    ver = WPA_KEY_INFO_TYPE_HMAC_MD5_RC4;
  
  key_info = ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_ACK;
  WPA_PUT_BE16(msg->key_info, key_info);
  
  if (peerkey->cipher != WPA_CIPHER_TKIP)
    WPA_PUT_BE16(msg->key_length, 16);
  else
    WPA_PUT_BE16(msg->key_length, 32);
  
  os_memcpy(msg->replay_counter, peerkey->replay_counter,
            WPA_REPLAY_COUNTER_LEN);
  wf_os_inc_byte_array(peerkey->replay_counter, WPA_REPLAY_COUNTER_LEN);
  
  WPA_PUT_BE16(msg->key_data_length, kde_len);
  wf_wpa_add_kde((wf_u8 *) (msg + 1), RSN_KEY_DATA_PMKID,
                 peerkey->smkid, TK_PMK_ID_LEN);
  
  if (wf_os_get_random(peerkey->inonce, WPA_LEN_NONCE)) {
    wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
            "RSN: Failed to get random data for INonce (STK)");
    os_free(mbuf);
    return;
  }
  wpa_hexdump(MSG_DEBUG, "RSN: INonce for STK 4-Way Handshake",
              peerkey->inonce, WPA_LEN_NONCE);
  os_memcpy(msg->key_nonce, peerkey->inonce, WPA_LEN_NONCE);
  
  wpa_printf(MSG_DEBUG, "RSN: Sending EAPOL-Key STK 1/4 to " MACSTR,
             MAC2STR(peerkey->addr));
  wf_wpa_eapol_key_send(sm, NULL, 0, ver, peerkey->addr, ETH_P_EAPOL,
                        mbuf, mlen, NULL);
}


static int wf_wpa_supplicant_process_smk_m45(struct wpa_sm *sm,
                                             const unsigned char *src_addr,
                                             const struct wpa_eapol_key *key,
                                             size_t extra_len, int ver)
{
  struct wpa_peerkey *peerkey;
  struct wpa_eapol_ie_parse kde;
  wf_u32 lifetime;
  
  if (!sm->peerkey_enabled || sm->proto != WPA_RSN_PROTO) {
    wpa_printf(MSG_DEBUG, "RSN: SMK handshake not allowed for "
               "the current network");
    return -1;
  }
  
  if (wf_wpa_supplicant_parse_ies((const wf_u8 *)(key + 1), extra_len, &kde) < 0) {
    wpa_printf(MSG_INFO, "RSN: Failed to parse KDEs in SMK M4/M5");
    return -1;
  }
  
  if (kde.mac_addr == NULL || kde.mac_addr_len < ETH_ALEN ||
      kde.nonce == NULL || kde.nonce_len < WPA_LEN_NONCE ||
        kde.smk == NULL || kde.smk_len < PMK_LEN + WPA_LEN_NONCE ||
          kde.lifetime == NULL || kde.lifetime_len < 4) {
            wpa_printf(MSG_INFO, "RSN: No MAC Address, Nonce, SMK, or "
                       "Lifetime KDE in SMK M4/M5");
            return -1;
          }
  
  for (peerkey = sm->peerkey; peerkey; peerkey = peerkey->next) {
    if (os_memcompare(peerkey->addr, kde.mac_addr, ETH_ALEN) == 0 &&
        os_memcompare(peerkey->initiator ? peerkey->inonce :
                        peerkey->pnonce, key->key_nonce, WPA_LEN_NONCE) == 0)
      break;
  }
  if (peerkey == NULL) {
    wpa_printf(MSG_INFO, "RSN: No matching SMK handshake found "
               "for SMK M4/M5: peer " MACSTR, MAC2STR(kde.mac_addr));
    return -1;
  }
  
  if (peerkey->initiator) {
    if (wf_wpa_supplicant_process_smk_m5(sm, src_addr, key, ver,
                                         peerkey, &kde) < 0)
      return -1;
  } else {
    if (wf_wpa_supplicant_process_smk_m4(peerkey, &kde) < 0)
      return -1;
  }
  
  os_memcpy(peerkey->smk, kde.smk, PMK_LEN);
  peerkey->smk_complete = 1;
  wpa_hexdump_key(MSG_DEBUG, "RSN: SMK", peerkey->smk, PMK_LEN);
  lifetime = TILK_WPA_READ_BE32(kde.lifetime);
  wpa_printf(MSG_DEBUG, "RSN: SMK lifetime %u seconds", lifetime);
  if (lifetime > 1000000000)
    lifetime = 1000000000;
  peerkey->lifetime = lifetime;
  if (peerkey->initiator) {
    wf_rsn_smkid(peerkey->smk, peerkey->pnonce, peerkey->addr,
                 peerkey->inonce, sm->own_addr, peerkey->smkid, peerkey->akmp);
    wf_wpa_supplicant_send_stk_1_of_4(sm, peerkey);
  } else {
    wf_rsn_smkid(peerkey->smk, peerkey->pnonce, sm->own_addr,
                 peerkey->inonce, peerkey->addr, peerkey->smkid,
                 peerkey->akmp);
  }
  wpa_hexdump(MSG_DEBUG, "RSN: SMKID", peerkey->smkid, TK_PMK_ID_LEN);
  
  return 0;
}


void wf_peerkey_rx_eapol_smk(struct wpa_sm *sm, const wf_u8 * src_addr,
                             struct wpa_eapol_key *key, size_t extra_len,
                             wf_u16 key_info, wf_u16 ver)
{
  if (key_info & WPA_KEY_INFO_ERROR) {
    
    wf_wpa_supplicant_process_smk_error(sm, src_addr, key, extra_len);
  } else if (key_info & WPA_KEY_INFO_ACK) {
    
    wf_wpa_supplicant_process_smk_m2(sm, src_addr, key, extra_len, ver);
  } else {
    
    wf_wpa_supplicant_process_smk_m45(sm, src_addr, key, extra_len, ver);
  }
}

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
  
  
  mic_len = wf_wpa_mic_len(sm->key_mgmt);
  keyhdrlen = mic_len == 24 ? sizeof(*key192) : sizeof(*key);
  
  if (len < sizeof(*hdr) + keyhdrlen) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "WPA: EAPOL frame too short to be a WPA "
              "EAPOL-Key (len %lu, expecting at least %lu)",
              (unsigned long) len,
              (unsigned long) sizeof(*hdr) + keyhdrlen);
    return 0;
  }
  
  hdr = (const struct ieee802_1x_hdr *) buf;
  plen = be_to_host16(hdr->length);
  data_len = plen + sizeof(*hdr);
  wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
          "IEEE 802.1X RX: version=%d type=%d length=%lu",
          hdr->version, hdr->type, (unsigned long) plen);
  
  if (hdr->version < EAPOL_VERSION) {
    ;
  }
  if (hdr->type != IEEE802_1X_TYPE_EAPOL_KEY) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "WPA: EAPOL frame (type %u) discarded, "
              "not a Key frame", hdr->type);
    ret = 0;
    goto out;
  }
  wpa_hexdump(MSG_MSGDUMP, "WPA: RX EAPOL-Key", buf, len);
  if (plen > len - sizeof(*hdr) || plen < keyhdrlen) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "WPA: EAPOL frame payload size %lu "
              "invalid (frame size %lu)",
              (unsigned long) plen, (unsigned long) len);
    ret = 0;
    goto out;
  }
  if (data_len < len) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "WPA: ignoring %lu bytes after the IEEE 802.1X data",
            (unsigned long) len - data_len);
  }
  
  tmp = os_malloc(data_len);
  if (tmp == NULL)
    goto out;
  os_memcpy(tmp, buf, data_len);
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
  
  if (key_data_len > plen - keyhdrlen) {
    wpa_msg(sm->ctx->msg_ctx, MSG_INFO, "WPA: Invalid EAPOL-Key "
            "frame - key_data overflow (%u > %u)",
            (unsigned int) key_data_len,
            (unsigned int) (plen - keyhdrlen));
    goto out;
  }
  key_info = WPA_GET_BE16(key->key_info);
  ver = key_info & WPA_KEY_INFO_TYPE_MASK;
  if ((key_info & WPA_KEY_INFO_MIC) && 
      wf_wpa_supplicant_verify_eapol_key_mic(sm, key192, ver, tmp, data_len))
    return -1;
  
  
  if ((sec_info->rsn_enable == 1) &&
      (key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
        if (wf_wpa_supplicant_decrypt_key_data(sm, key, ver, key_data,
                                               &key_data_len))
          goto out;
      }
  
  if (key_info & WPA_KEY_INFO_KEY_TYPE) {
    if (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) {
      wpa_msg(sm->ctx->msg_ctx, MSG_WARNING,
              "WPA: Ignored EAPOL-Key (Pairwise) with "
                "non-zero key index");
      goto out;
    }
    if (key_info & WPA_KEY_INFO_MIC) {
      wf_wpa_supplicant_process_3_of_4(sm, key, ver, key_data,
                                       key_data_len);
    } else {
      wf_wpa_supplicant_process_1_of_4(sm, src_addr, key,
                                       ver, key_data,
                                       key_data_len);
    }
  } else if (key_info & WPA_KEY_INFO_SMK_MESSAGE) {
    wf_peerkey_rx_eapol_smk(sm, src_addr, key, key_data_len, key_info,
                            ver);
  } else {
    if (key_info & WPA_KEY_INFO_MIC) {
      wf_wpa_supplicant_process_1_of_2(sm, src_addr, key,
                                       key_data, key_data_len,
                                       ver);
    } else {
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




static void wf_wpa_sm_pmksa_free_cb(struct rsn_pmksa_cache_entry *entry,
                                    void *ctx, enum pmksa_free_reason reason)
{
  struct wpa_sm *sm = ctx;
  int deauth = 0;
  
  wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "RSN: PMKSA cache entry free_cb: "
          MACSTR " reason=%d", MAC2STR(entry->aa), reason);
  
  if (sm->cur_pmksa == entry) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "RSN: %s current PMKSA entry",
            reason == PMKSA_REPLACE ? "replaced" : "removed");
    wf_pmksa_cache_clear_current(sm);
    
    if (reason != PMKSA_REPLACE)
      deauth = 1;
  }
  
  if (reason == PMKSA_EXPIRE &&
      (sm->pmk_len == entry->pmk_len &&
       os_memcmp(sm->pmk, entry->pmk, sm->pmk_len) == 0)) {
         wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
                 "RSN: deauthenticating due to expired PMK");
         wf_pmksa_cache_clear_current(sm);
         deauth = 1;
       }
  
  if (deauth) {
    os_memset(sm->pmk, 0, sizeof(sm->pmk));
  }
}


struct rsn_pmksa_cache
*wf_wpa_pmksa_cache_init(void (*free_cb)
                         (struct rsn_pmksa_cache_entry * entry, void *ctx,
                          enum pmksa_free_reason reason), void *ctx,
                         struct wpa_sm *sm)
{
  struct rsn_pmksa_cache *pmksa;
  
  pmksa = os_zalloc(sizeof(*pmksa));
  if (pmksa) {
    pmksa->free_cb = free_cb;
    pmksa->ctx = ctx;
    pmksa->sm = sm;
  }
  
  return pmksa;
}

struct wpa_sm * wf_wpa_sm_init(struct wpa_supplicant *wpa_s)
{
  struct wpa_sm *sm;
  
  sm = os_zalloc(sizeof(*sm));
  if (sm == NULL)
    return NULL;
  dl_list_init(&sm->pmksa_candidates);
  sm->renew_snonce = 1;
  
  sm->dot11RSNAConfigPMKLifetime = 43200;
  sm->dot11RSNAConfigPMKReauthThreshold = 70;
  sm->dot11RSNAConfigSATimeout = 60;
  
  sm->pmksa = wf_wpa_pmksa_cache_init(wf_wpa_sm_pmksa_free_cb, sm, sm);
  if (sm->pmksa == NULL) {
    wpa_msg(sm->ctx->msg_ctx, MSG_ERROR,
            "RSN: PMKSA cache initialization failed");
    os_free(sm);
    return NULL;
  }
  
  return sm;
}


void wf_wpa_sm_drop_sa(struct wpa_sm *sm)
{
  wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: Clear old PMK and PTK");
  sm->ptk_set = 0;
  sm->tptk_set = 0;
  os_memset(sm->pmk, 0, sizeof(sm->pmk));
  os_memset(&sm->ptk, 0, sizeof(sm->ptk));
  os_memset(&sm->tptk, 0, sizeof(sm->tptk));
}

void wf_wpa_sm_deinit(struct wpa_sm *sm)
{
  if (sm == NULL)
    return;
  
  os_free(sm->assoc_wpa_ie);
  os_free(sm->ap_wpa_ie);
  os_free(sm->ap_rsn_ie);
  wf_wpa_sm_drop_sa(sm);
  os_free(sm->ctx);
  os_free(sm);
}

void wf_wpa_sm_notify_assoc(struct wpa_sm *sm, const wf_u8 *bssid)
{
  int clear_ptk = 1;
  
  if (sm == NULL)
    return;
  
  wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
          "WPA: Association event - clear replay counter");
  os_memcpy(sm->bssid, bssid, ETH_ALEN);
  os_memset(sm->rx_replay_counter, 0, WPA_REPLAY_COUNTER_LEN);
  sm->rx_replay_counter_set = 0;
  sm->renew_snonce = 1;
  
  
  
  if (clear_ptk) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG, "WPA: Clear old PTK");
    sm->ptk_set = 0;
    os_memset(&sm->ptk, 0, sizeof(sm->ptk));
    sm->tptk_set = 0;
    os_memset(&sm->tptk, 0, sizeof(sm->tptk));
  }
  
  
}

void wf_wpa_sm_notify_disassoc(struct wpa_sm *sm)
{
  ;
  wf_pmksa_cache_clear_current(sm);
  if (wf_wpa_sm_get_state() == WPA_4WAY_HANDSHAKE)
    sm->dot11RSNA4WayHandshakeFailures++;
  
  wf_wpa_sm_drop_sa(sm);
  
  sm->msg_3_of_4_ok = 0;
}

void wf_wpa_sm_set_pmk(struct wpa_sm *sm, const wf_u8 *pmk, size_t pmk_len,
                       const wf_u8 *pmkid, const wf_u8 *bssid)
{
  if (sm == NULL)
    return;
  
  sm->pmk_len = pmk_len;
  os_memcpy(sm->pmk, pmk, pmk_len);
  
  
  if (bssid) {
    wf_wpa_pmksa_cache_add(sm->pmksa, pmk, pmk_len, pmkid, NULL, 0,
                           bssid, sm->own_addr,
                           sm->network_ctx, sm->key_mgmt);
  }
}

void wf_wpa_sm_set_fast_reauth(struct wpa_sm *sm, int fast_reauth)
{
  if (sm)
    sm->fast_reauth = fast_reauth;
}

void wf_wpa_sm_set_scard_ctx(struct wpa_sm *sm, void *scard_ctx)
{
  if (sm == NULL)
    return;
  sm->scard_ctx = scard_ctx;
}

void wf_wpa_sm_set_config(struct wpa_sm *sm, struct rsn_supp_config *config)
{
  if (!sm)
    return;
  
  if (config) {
    sm->network_ctx = config->network_ctx;
    sm->peerkey_enabled = config->peerkey_enabled;
    sm->allowed_pairwise_cipher = config->allowed_pairwise_cipher;
    sm->proactive_key_caching = config->proactive_key_caching;
    sm->eap_workaround = config->eap_workaround;
    sm->eap_conf_ctx = config->eap_conf_ctx;
    if (config->ssid) {
      os_memcpy(sm->ssid, config->ssid, config->ssid_len);
      sm->ssid_len = config->ssid_len;
    } else
      sm->ssid_len = 0;
    sm->wpa_ptk_rekey = config->wpa_ptk_rekey;
    sm->p2p = config->p2p;
  } else {
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

void wf_wpa_sm_set_own_addr(struct wpa_sm *sm, const wf_u8 *addr)
{
  if (sm)
    os_memcpy(sm->own_addr, addr, ETH_ALEN);
}

void wf_wpa_sm_set_ifname(struct wpa_sm *sm, const char *ifname,
                          const char *bridge_ifname)
{
  if (sm) {
    sm->ifname = ifname;
    sm->bridge_ifname = bridge_ifname;
  }
}

void wf_wpa_sm_set_eapol(struct wpa_sm *sm, struct eapol_sm *eapol)
{
  if (sm)
    sm->eapol = eapol;
}

int wf_wpa_sm_set_param(struct wpa_sm *sm, enum wpa_sm_conf_params param,
                        unsigned int value)
{
  int ret = 0;
  
  if (sm == NULL)
    return -1;
  
  switch (param) {
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

int wf_wpa_sm_get_status(struct wpa_sm *sm, char *buf, size_t buflen,
                         int verbose)
{
  char *pos = buf, *end = buf + buflen;
  int ret;
  
  ret = os_snprintf(pos, end - pos,
                    "pairwise_cipher=%s\n"
                      "group_cipher=%s\n"
                        "key_mgmt=%s\n",
                        wf_wpa_cipher_txt(sm->pairwise_cipher),
                        wf_wpa_cipher_txt(sm->group_cipher),
                        wf_wpa_key_mgmt_txt(sm->key_mgmt, sm->proto));
  if (os_snprintf_error(end - pos, ret))
    return pos - buf;
  pos += ret;
  
  if (sm->mfp != NO_MGMT_FRAME_PROTECTION && sm->ap_rsn_ie) {
    struct wpa_ie_data rsn;
    if (wf_wpa_parse_wpa_ie_rsn(sm->ap_rsn_ie, sm->ap_rsn_ie_len, &rsn)
        >= 0 &&
          rsn.capabilities & (WPA_CAPABILITY_MFPR |
                              WPA_CAPABILITY_MFPC)) {
                                ret = os_snprintf(pos, end - pos, "pmf=%d\n",
                                                  (rsn.capabilities &
                                                   WPA_CAPABILITY_MFPR) ? 2 : 1);
                                if (os_snprintf_error(end - pos, ret))
                                  return pos - buf;
                                pos += ret;
                              }
  }
  
  return pos - buf;
}


int wf_wpa_sm_pmf_enabled(struct wpa_sm *sm)
{
  struct wpa_ie_data rsn;
  
  if (sm->mfp == NO_MGMT_FRAME_PROTECTION || !sm->ap_rsn_ie)
    return 0;
  
  if (wf_wpa_parse_wpa_ie_rsn(sm->ap_rsn_ie, sm->ap_rsn_ie_len, &rsn) >= 0 &&
      rsn.capabilities & (WPA_CAPABILITY_MFPR | WPA_CAPABILITY_MFPC))
    return 1;
  
  return 0;
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
        (sm->cur_pmksa ? 2 + TK_PMK_ID_LEN : 0)) {
          wpa_printf(MSG_DEBUG, "RSN: Too short IE buffer (%lu bytes)",
                     (unsigned long)rsn_ie_len);
          return -1;
	}
  
  hdr = (struct rsn_hdr_ie *)rsn_ie;
  hdr->elem_id = WLAN_RSN_EID;
  TILK_WPA_TO_LE16(hdr->version, RSN_VERSION);
  pos = (wf_u8 *) (hdr + 1);
  
  suite = wf_wpa_cipher_to_suite(WPA_RSN_PROTO, group_cipher);
  if (suite == 0) {
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
       pairwise_cipher != WPA_NONE_CIPHER)) {
         wpa_printf(MSG_WARNING, "Invalid pairwise cipher (%d).",
                    pairwise_cipher);
         return -1;
       }
  TILK_SELE_CTOR_RSN__PUT(pos, suite);
  pos += _SELECTOR_LEN_TO_RSN;
  
  *pos++ = 1;
  *pos++ = 0;
  if (key_mgmt == WPA_IEEE8021X_KEY_MGMT) {
    TILK_SELE_CTOR_RSN__PUT(pos, TILK_AKMGMT_UNSPEC_802_1X_FOR_RSN);
  } else if (key_mgmt == WPA_PSK_KEY_MGMT) {
    TILK_SELE_CTOR_RSN__PUT(pos, TILK_AKM_PSK_OVER_802_1X_FOR_RSN);
  } else if (key_mgmt == WPA_KEY_MGMT_CCKM) {
    TILK_SELE_CTOR_RSN__PUT(pos, RSN_AUTH_KEY_MGMT_CCKM);
  } else if (key_mgmt == WPA_KEY_MGMT_IEEE8021X_SUITE_B_192) {
    TILK_SELE_CTOR_RSN__PUT(pos, RSN_AUTH_KEY_MGMT_802_1X_SUITE_B_192);
  } else if (key_mgmt == WPA_KEY_MGMT_IEEE8021X_SUITE_B) {
    TILK_SELE_CTOR_RSN__PUT(pos, RSN_AUTH_KEY_MGMT_802_1X_SUITE_B);
  } else {
    wpa_printf(MSG_WARNING, "Invalid key management type (%d).", key_mgmt);
    return -1;
  }
  pos += _SELECTOR_LEN_TO_RSN;
  
  capab = 0;
  TILK_WPA_TO_LE16(pos, capab);
  pos += 2;
  
  if (sm->cur_pmksa) {
    
    *pos++ = 1;
    *pos++ = 0;
    
    os_memcpy(pos, sm->cur_pmksa->pmkid, TK_PMK_ID_LEN);
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
  if (suite == 0) {
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
       pairwise_cipher != WPA_NONE_CIPHER)) {
         wpa_printf(MSG_WARNING, "Invalid pairwise cipher (%d).",
                    pairwise_cipher);
         return -1;
       }
  TILK_SELE_CTOR_RSN__PUT(pos, suite);
  pos += WPA_LEN_SELECTOR;
  
  *pos++ = 1;
  *pos++ = 0;
  if (key_mgmt == WPA_IEEE8021X_KEY_MGMT) {
    TILK_SELE_CTOR_RSN__PUT(pos, _AUTH_KEY_MGMT_UNSPEC_802_1X_TO_WPA);
  } else if (key_mgmt == WPA_PSK_KEY_MGMT) {
    TILK_SELE_CTOR_RSN__PUT(pos, _AUTH_KEY_MGMT_PSK_OVER_802_1X_TO_WPA);
  } else if (key_mgmt == WPA_WPA_NONE_KEY_MGMT) {
    TILK_SELE_CTOR_RSN__PUT(pos, _AUTH_KEY_MGMT_NONE_TO_WPA);
  } else if (key_mgmt == WPA_KEY_MGMT_CCKM) {
    TILK_SELE_CTOR_RSN__PUT(pos, WPA_AUTH_KEY_MGMT_CCKM);
  } else {
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

int wf_wpa_sm_set_assoc_wpa_ie_default(struct wpa_sm *sm, wf_u8 *wpa_ie,
                                       size_t *wpa_ie_len)
{
  int res;
  
  if (sm == NULL)
    return -1;
  res = wf_wpa_gen_wpa_ie(sm, wpa_ie, *wpa_ie_len);
  if (res < 0)
    return -1;
  *wpa_ie_len = res;
  
  wpa_hexdump(MSG_DEBUG, "WPA: Set own WPA IE default",
              wpa_ie, *wpa_ie_len);
  
  if (sm->assoc_wpa_ie == NULL) {
    sm->assoc_wpa_ie = os_malloc(*wpa_ie_len);
    if (sm->assoc_wpa_ie == NULL)
      return -1;
    
    os_memcpy(sm->assoc_wpa_ie, wpa_ie, *wpa_ie_len);
    sm->assoc_wpa_ie_len = *wpa_ie_len;
  }
  
  return 0;
}

int wf_wpa_sm_set_assoc_wpa_ie(struct wpa_sm *sm, const wf_u8 *ie, size_t len)
{
  if (sm == NULL)
    return -1;
  
  os_free(sm->assoc_wpa_ie);
  if (ie == NULL || len == 0) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "WPA: clearing own WPA/RSN IE");
    sm->assoc_wpa_ie = NULL;
    sm->assoc_wpa_ie_len = 0;
  } else {
    wpa_hexdump(MSG_DEBUG, "WPA: set own WPA/RSN IE", ie, len);
    sm->assoc_wpa_ie = os_malloc(len);
    if (sm->assoc_wpa_ie == NULL)
      return -1;
    
    os_memcpy(sm->assoc_wpa_ie, ie, len);
    sm->assoc_wpa_ie_len = len;
  }
  
  return 0;
}

int wf_wpa_sm_set_ap_wpa_ie(struct wpa_sm *sm, const wf_u8 *ie, size_t len)
{
  if (sm == NULL)
    return -1;
  
  os_free(sm->ap_wpa_ie);
  if (ie == NULL || len == 0) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "WPA: clearing AP WPA IE");
    sm->ap_wpa_ie = NULL;
    sm->ap_wpa_ie_len = 0;
  } else {
    wpa_hexdump(MSG_DEBUG, "WPA: set AP WPA IE", ie, len);
    sm->ap_wpa_ie = os_malloc(len);
    if (sm->ap_wpa_ie == NULL)
      return -1;
    
    os_memcpy(sm->ap_wpa_ie, ie, len);
    sm->ap_wpa_ie_len = len;
  }
  
  return 0;
}

int wf_wpa_sm_set_ap_rsn_ie(struct wpa_sm *sm, const wf_u8 *ie, size_t len)
{
  if (sm == NULL)
    return -1;
  
  os_free(sm->ap_rsn_ie);
  if (ie == NULL || len == 0) {
    wpa_dbg(sm->ctx->msg_ctx, MSG_DEBUG,
            "WPA: clearing AP RSN IE");
    sm->ap_rsn_ie = NULL;
    sm->ap_rsn_ie_len = 0;
  } else {
    wpa_hexdump(MSG_DEBUG, "WPA: set AP RSN IE", ie, len);
    sm->ap_rsn_ie = os_malloc(len);
    if (sm->ap_rsn_ie == NULL)
      return -1;
    
    os_memcpy(sm->ap_rsn_ie, ie, len);
    sm->ap_rsn_ie_len = len;
  }
  
  return 0;
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

int wf_wpa_sm_parse_own_wpa_ie(struct wpa_sm *sm, struct wpa_ie_data *data)
{
  if (sm == NULL)
    return -1;
  
  if (sm->assoc_wpa_ie == NULL) {
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
  if (ptk_kck && ptk_kck_len <= WPA_KCK_MAX_LEN) {
    os_memcpy(sm->ptk.kck, ptk_kck, ptk_kck_len);
    sm->ptk.kck_len = ptk_kck_len;
    wpa_printf(MSG_DEBUG, "Updated PTK KCK");
  }
  if (ptk_kek && ptk_kek_len <= WPA_KEK_MAX_LEN) {
    os_memcpy(sm->ptk.kek, ptk_kek, ptk_kek_len);
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
  if (len < sizeof(*hdr)) {
    sm->dot1xSuppInvalidEapolFramesRx++;
    return 0;
  }
  hdr = (const struct ieee802_1x_hdr *)buf;
  sm->dot1xSuppLastEapolFrameVersion = hdr->version;
  os_memcpy(sm->dot1xSuppLastEapolFrameSource, src, ETH_ALEN);
  if (hdr->version < EAPOL_VERSION) {
    
  }
  plen = be_to_host16(hdr->length);
  if (plen > len - sizeof(*hdr)) {
    sm->dot1xSuppEapLengthErrorFramesRx++;
    return 0;
  }
  data_len = plen + sizeof(*hdr);
  
  switch (hdr->type) {
  case IEEE802_1X_TYPE_EAP_PACKET:
    if (sm->conf.workaround) {
      
      const struct eap_hdr *ehdr = (const struct eap_hdr *)(hdr + 1);
      if (plen >= sizeof(*ehdr) && ehdr->code == 10) {
        break;
      }
    }
    
    wf_wpabuf_free(sm->eapReqData);
    sm->eapReqData = wf_wpabuf_alloc_copy(hdr + 1, plen);
    if (sm->eapReqData) {
      wpa_printf(MSG_DEBUG, "EAPOL: Received EAP-Packet " "frame");
      sm->eapolEap = TRUE;
      
    }
    break;
  case IEEE802_1X_TYPE_EAPOL_KEY:
    if (plen < sizeof(*key)) {
      wpa_printf(MSG_DEBUG, "EAPOL: Too short EAPOL-Key "
                 "frame received");
      break;
    }
    key = (const struct ieee802_1x_eapol_key *)(hdr + 1);
    if (key->type == EAPOL_KEY_TYPE_WPA || key->type == EAPOL_KEY_TYPE_RSN) {
      
      wpa_printf(MSG_DEBUG, "EAPOL: Ignoring WPA EAPOL-Key "
                 "frame in EAPOL state machines");
      res = 0;
      break;
    }
    if (key->type != EAPOL_KEY_TYPE_RC4) {
      wpa_printf(MSG_DEBUG, "EAPOL: Ignored unknown "
                 "EAPOL-Key type %d", key->type);
      break;
    }
    os_free(sm->last_rx_key);
    sm->last_rx_key = os_malloc(data_len);
    if (sm->last_rx_key) {
      wpa_printf(MSG_DEBUG, "EAPOL: Received EAPOL-Key " "frame");
      os_memcpy(sm->last_rx_key, buf, data_len);
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
  
  wpa_dbg(wpa_s, MSG_DEBUG, "RX EAPOL from " MACSTR, MAC2STR(src_addr));
  wpa_hexdump(MSG_MSGDUMP, "RX EAPOL", buf, len);
  
  
  wpa_s->last_eapol_matches_bssid =
    os_memcompare(src_addr, wpa_s->bssid, ETH_ALEN) == 0;
  
  //if (wpa_s->ap_mode_start) {
  //wpa_supplicant_ap_rx_eapol(wpa_s, src_addr, buf, len);
  //return;
  //}
  
  if (wpa_s->key_mgmt == WPA_NONE_KEY_MGMT) {
    wpa_dbg(wpa_s, MSG_DEBUG, "Ignored received EAPOL frame since "
            "no key management is configured");
    return;
  }
  
  if (wpa_s->eapol_received == 0 &&
      (!(wpa_s->drv_flags & WPA_DRIVER_FLAGS_4WAY_HANDSHAKE) ||
       !wpa_key_mgmt_wpa_psk(wpa_s->key_mgmt) ||
         wpa_s->wpa_state != WPA_COMPLETED) &&
        (wpa_s->current_ssid == NULL ||
         wpa_s->current_ssid->mode != IEEE80211_MODE_IBSS)) {
           
           int timeout = 10;
           
           if (wpa_key_mgmt_wpa_ieee8021x(wpa_s->key_mgmt) ||
               wpa_s->key_mgmt == WPA_IEEE8021X_NO_WPA_KEY_MGMT ||
                 wpa_s->key_mgmt == WPA_KEY_MGMT_WPS) {
                   
                   timeout = 70;
                 }
           timeout = timeout;
         }
  wpa_s->eapol_received++;
  
  if (wpa_s->countermeasures) {
    wpa_msg(wpa_s, MSG_INFO, "WPA: Countermeasures - dropped "
            "EAPOL packet");
    return;
  }
  
  os_memcpy(wpa_s->last_eapol_src, src_addr, ETH_ALEN);
  if (!wpa_key_mgmt_wpa_psk(wpa_s->key_mgmt) &&
      wf_wpa_eapol_sm_rx_eapol(wpa_s->eapol, src_addr, (buf + 14), (len - 14)) > 0)
    return;
  if (!(wpa_s->drv_flags & WPA_DRIVER_FLAGS_4WAY_HANDSHAKE))
    wf_wpa_sm_rx_eapol(wpa_s->wpa, src_addr, (buf + 14), (len -14));
  else if (wpa_key_mgmt_wpa_ieee8021x(wpa_s->key_mgmt)) {
    
  }
}


