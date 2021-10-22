#ifndef __MP_REG_H__
#define __MP_REG_H__


int wf_mp_read_bb(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra);
int wf_mp_write_bb(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra);

int wf_mp_read_rf(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra);
int wf_mp_write_rf(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra);

int wf_mp_reg_read(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);
int wf_mp_reg_write(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);


wf_u32 mp_set_rf(nic_info_st *pnic_info);

wf_u32 wf_mp_read_bbreg(nic_info_st *pnic_info, wf_u32 RegAddr, wf_u32 BitMask);
int    wf_mp_write_bbreg(nic_info_st *pnic_info, wf_u32 RegAddr, wf_u32 BitMask,wf_u32 Data);
wf_u32 wf_mp_read_rfreg(nic_info_st *pnic_info, wf_u32 RegAddr, wf_u32 BitMask);
int   wf_mp_write_rfreg(nic_info_st *pnic_info, wf_u32 eRFPath, wf_u32 RegAddr,wf_u32 BitMask, wf_u32 Data);



#endif
