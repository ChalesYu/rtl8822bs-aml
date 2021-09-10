#include "common.h"
#include "wpa_common.h"
#include "beacon.h"
#include "ap.h"

static inline unsigned short hostapd_wpa_swap_16(unsigned short v)
{
  return ((v & 0xff) << 8) | (v >> 8);
}

wf_u16 hostapd_own_capab_info(nic_info_st *pnic_info)
{
  int capab = WLAN_CAPABILITY_ESS;
  int privacy;
  int i;
  
  capab |= WLAN_CAPABILITY_SHORT_PREAMBLE;
  
  //capab |= WLAN_CAPABILITY_SHORT_SLOT_TIME;
  
  return capab;
}

static wf_u8 * hostapd_eid_ds_params(nic_info_st *pnic_info, wf_u8 *eid, int *len)
{
  *eid++ = WLAN_EID_DS_PARAMS;
  *eid++ = 1;
  *eid++ = 6;
  *len = *len + 3;
  return eid;
}
#define bswap_16(a) ((((u16) (a) << 8) & 0xff00) | (((u16) (a) >> 8) & 0xff))

int ieee802_11_build_ap_params(nic_info_st *pnic_info,
                               char *ssid, int ssid_len, wf_u8 *ie)
{
  hw_info_st *hw_info = pnic_info->hw_info;
  int i=0;
  wf_u8 *tail = NULL;
  int head_len = 0;
  wf_u8 *resp = NULL;
  size_t resp_len = 0;
  
  
  wf_u16 capab_info;
  wf_u8 *pos, *tailpos, *csa_pos;
  wf_u8 *head = ie;
  le16 intv = wf_le16_to_cpu(100);
  le16 capainfo;
  head = head + 8;
  head_len += 8;
  wf_memcpy(head,&intv,2);
  head = head + 2;
  head_len += 2;
  
  /* hardware or low-level driver will setup seq_ctrl and timestamp */
  capab_info = hostapd_own_capab_info(pnic_info);
  capainfo = wf_le16_to_cpu(capab_info);
  wf_memcpy(head,&capainfo,2);
  head = head + 2;
  head_len += 2;
  
  /* SSID */
  *head++ = WLAN_EID_SSID;
  *head++ = ssid_len;
  wf_memcpy(head, ssid, ssid_len);
  head += ssid_len;
  head_len = head_len + ssid_len + 2;
  
  
  /* Supported rates */
  *head++ = WLAN_EID_SUPP_RATES;
  *head++ = 8;
  for(i = 0; i < 8; i++)
  {
    *head++ = hw_info->datarate[i];
  }
  head_len += (i + 2);
  
  /* DS Params */
  *head++ = WLAN_EID_DS_PARAMS;
  *head++ = 1;
  *head++ = 6;
  head_len += 3;
  
  
  /* Extended supported rates */
  *head++ = WLAN_EID_EXT_SUPP_RATES;
  *head++ = 4;
  for(i = 0; i < 4; i++)
  {
    *head++ = hw_info->datarate[8 + i];
  }
  head_len += 6;
  
  /* Wi-Fi Alliance WMM */
  //tailpos = hostapd_eid_wmm(hapd, tailpos);
  
  return head_len;
}

