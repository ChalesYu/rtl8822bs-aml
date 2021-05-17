#ifndef __IE_H__
#define __IE_H__
#define MCS_RATE_1R	(0x000000ff)


int wf_ie_cap_info_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info, wf_u16 cap_info);
int wf_ie_ssid_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info, wf_u8 *pie_data, wf_u8 len);
int wf_ie_supported_rates_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info, wf_u8 *pie_data, wf_u8 len);
int wf_ie_extend_supported_rates_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info, wf_u8 *pie_data, wf_u8 len);
int wf_ie_wmm_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info, wf_u8 *pie_data, wf_u8 len);
int wf_ie_wpa_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info, wf_u8 *pie_data, wf_u8 len);
int wf_ie_ht_capability_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info, wf_u8 *pie_data, wf_u8 len);
int wf_ie_ht_operation_info_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info, wf_u8 *pie_data, wf_u8 len);
int wf_ie_erp_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info, wf_u8 *pie_data, wf_u8 len);
int wf_ie_rsn_update(nic_info_st *nic_info, wdn_net_info_st *wdn_info, wf_u8 *pie_data, wf_u8 len);
int only_cckrates(unsigned char *rate, int ratelen);
int have_cckrates(unsigned char *rate, int ratelen);

wf_u8 * set_ie(wf_u8 * pbuf, wf_u8 index, wf_u8 len, wf_u8 * source, wf_u32 * frlen);
wf_u8 * set_fixed_ie(wf_u8 *pbuf, wf_u32 len, wf_u8 *source, wf_u16 *frlen);


#endif

