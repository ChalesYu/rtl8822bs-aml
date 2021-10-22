
#ifndef __BEACON_H__
#define __BEACON_H__

struct ap_beacon_ie{
  wf_u16 beacon_int;
  wf_u16 capab_info;
  /* followed by some of SSID, Supported rates,
  * FH Params, DS Params, CF Params, IBSS Params, TIM */
  wf_u8 variable[];
};

int ieee802_11_build_ap_params(nic_info_st *pnic_info,
			       char *ssid, int ssid_len, wf_u8 *ie);

#endif

