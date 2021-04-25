#ifndef __MP_EFUSE_H__
#define __MP_EFUSE_H__


int wf_mp_efuse_get(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
int wf_mp_efuse_set(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);

int wf_mp_logic_efuse_read(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
int wf_mp_logic_efuse_write(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);


#endif
