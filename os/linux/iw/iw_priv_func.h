#ifndef __IW_PRIV_FUNC_H__
#define __IW_PRIV_FUNC_H__

#include "ndev_linux.h"

#if defined(CONFIG_WEXT_PRIV)

int wf_iw_priv_reg(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_fwdl(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_tx(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_status(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);

int wf_iw_priv_write32(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_read32(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_get_ap_Info(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_set_pid(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_get_sensitivity(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_set_rf(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_get_rf(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_set_p2p(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_get_p2p(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_set_pm(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_test(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_send_test(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_fw_dowlond_test(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra);
int wf_iw_priv_phy_reg(nic_info_st *nic_info, wf_u32 time);
int wf_iw_priv_mac_reg(nic_info_st *nic_info, int time);


void null_data_wlan_hdr(nic_info_st * pnic_info, struct xmit_buf * pxmit_buf);
int wf_iw_priv_data_xmit(nic_info_st * pnic_info);
int wf_iw_deauth_ap (nic_info_st *pnic_info, wf_u16 reason);

int atoi(const char *nptr);

int wf_iw_reg_write(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
int wf_iw_reg_read(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);

int wf_iw_all_mac(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
int wf_iw_phy(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
int wf_iw_fwdl(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);

#endif

#endif
