#ifndef __P2P_FRAME_MGT_H__
#define __P2P_FRAME_MGT_H__

wf_u8 *p2p_ie_to_get_func(wf_u8 * in_ie, wf_s32 in_len, wf_u8 * p2p_ie, wf_u32 * p2p_ielen);
void p2p_provision_request_to_issue_func(nic_info_st *nic_info, wf_u8 * pssid, wf_u8 ussidlen,wf_u8 * pdev_raddr, wf_u8 flag);
void p2p_GO_request_to_issue_func(nic_info_st *nic_info, wf_u8 *raddr, wf_u8 flag);
void p2p_invitation_request_to_issue_func(nic_info_st *nic_info, wf_u8 * raddr, wf_u8 flag);
int probereq_p2p_to_pre_issue_func(nic_info_st *nic_info, wf_u8 * da);
wf_u8 *p2p_attr_content_to_get_func(wf_u8 * p2p_ie, wf_u32 p2p_ielen, wf_u8 target_attr_id, wf_u8 * buf_content, wf_u32 * len_content);

#endif
