/*
 * p2p_wowlan.h
 *
 * used for .....
 *
 * Author: zenghua
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __P2P_WOWLAN_H__
#define __P2P_WOWLAN_H__

enum P2P_WOWLAN_RECV_FRAME_TYPE {
	P2P_WOWLAN_RECV_NEGO_REQ = 0,
	P2P_WOWLAN_RECV_INVITE_REQ = 1,
	P2P_WOWLAN_RECV_PROVISION_REQ = 2,
};

struct p2p_wowlan_info 
{
	wf_u8 is_trigger;
	enum P2P_WOWLAN_RECV_FRAME_TYPE wowlan_recv_frame_type;
	wf_u8 wowlan_peer_addr[WF_ETH_ALEN];
    wf_u16 wowlan_peer_wpsconfig;
    wf_u8 wowlan_peer_is_persistent;
    wf_u8 wowlan_peer_invitation_type;
};

#endif
