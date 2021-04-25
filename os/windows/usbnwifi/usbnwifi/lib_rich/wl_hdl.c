/******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/
#include "globals.h"
#include "ndis.h"
#include "hw_pcomp.h"
#include "hw_main.h"
#include "hw_Nic.h"
#include "hw_func.h"
#include "usb_main.h"
#include "bus_intf.h"
#include "8187_gen.h"

#define MAX_wMBOX1_BOX_NUMS	4
#define MESSAGE_BOX_SIZE		4

#define wlan9086X_MAX_CMD_LEN	7
#define wlan9086X_EX_MESSAGE_BOX_SIZE	4

static void Func_Construct_Beacon(PNIC Nic, u8 * pframe, u32 * pLength)
{
	struct wl_ieee80211_hdr *pwlanhdr;
	u16 *fctrl;
	u32 rate_len, pktlen;
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
	u8 bc_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, bc_addr, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, query_bssid(cur_network), ETH_ALEN);

	SetSeqNum(pwlanhdr, 0);
	SetFrameSubType(pframe, WIFI_BEACON);

	pframe += sizeof(struct wl_ieee80211_hdr_3addr);
	pktlen = sizeof(struct wl_ieee80211_hdr_3addr);

	pframe += 8;
	pktlen += 8;

	Func_Of_Proc_Pre_Memcpy(pframe,
				(unsigned char
				 *)(do_query_data_from_ie(cur_network->IEs, BCN_INTERVAL)), 2);

	pframe += 2;
	pktlen += 2;

	Func_Of_Proc_Pre_Memcpy(pframe,
				(unsigned char
				 *)(do_query_data_from_ie(cur_network->IEs, CAPABILITY)), 2);

	pframe += 2;
	pktlen += 2;

	if ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) {
		pktlen += cur_network->IELength - sizeof(NDIS_802_11_FIXED_IEs);
		Func_Of_Proc_Pre_Memcpy(pframe, cur_network->IEs + sizeof(NDIS_802_11_FIXED_IEs),
					pktlen);

		goto _ConstructBeacon;
	}

	pframe =
		ie_to_set_func(pframe, _SSID_IE_, cur_network->Ssid.SsidLength,
				   cur_network->Ssid.Ssid, &pktlen);

	rate_len = rateset_len_to_get_func(cur_network->SupportedRates, 1);
	pframe =
		ie_to_set_func(pframe, _SUPPORTEDRATES_IE_, ((rate_len > 8) ? 8 : rate_len),
				   cur_network->SupportedRates, &pktlen);

	pframe =
		ie_to_set_func(pframe, _DSSET_IE_, 1,
				   (unsigned char *)&(cur_network->Configuration.DSConfig),
				   &pktlen);

	if ((pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) {
		u32 ATIMWindow;
		ATIMWindow = 0;
		pframe =
			ie_to_set_func(pframe, _IBSS_PARA_IE_, 2,
					   (unsigned char *)(&ATIMWindow), &pktlen);
	}

	if (rate_len > 8)
		pframe =
			ie_to_set_func(pframe, _EXT_SUPPORTEDRATES_IE_, (rate_len - 8),
					   (cur_network->SupportedRates + 8), &pktlen);

_ConstructBeacon:

	if ((pktlen + TXDESC_SIZE) > 512) {
		MpTrace(COMP_EVENTS, DBG_NORMAL,("beacon frame too large\n"));
		return;
	}

	*pLength = pktlen;

}

static void Func_Construct_Pspoll(PNIC Nic, u8 * pframe, u32 * pLength)
{
	struct wl_ieee80211_hdr *pwlanhdr;
	u16 *fctrl;
	u32 pktlen;
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;
	SetPwrMgt(fctrl);
	SetFrameSubType(pframe, WIFI_PSPOLL);

	SetDuration(pframe, (pmlmeinfo->aid | 0xc000));

	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, query_bssid(&(pmlmeinfo->network)), ETH_ALEN);

	Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);

	*pLength = 16;
}

static void Func_Construct_Null_Functiondata(PNIC Nic,
									  u8 * pframe,
									  u32 * pLength,
									  u8 * StaAddr,
									  u8 bQoS,
									  u8 AC, u8 bEosp, u8 bForcePowerSave)
{
	struct wl_ieee80211_hdr *pwlanhdr;
	u16 *fctrl;
	u32 pktlen;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct wlan_network *cur_network = &pmlmepriv->cur_network;
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	pwlanhdr = (struct wl_ieee80211_hdr *)pframe;

	fctrl = &pwlanhdr->frame_ctl;
	*(fctrl) = 0;
	if (bForcePowerSave)
		SetPwrMgt(fctrl);

	switch (cur_network->network.InfrastructureMode) {
	case Ndis802_11Infrastructure:
		SetToDs(fctrl);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, query_bssid(&(pmlmeinfo->network)),
					ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, StaAddr, ETH_ALEN);
		break;
	case Ndis802_11APMode:
		SetFrDs(fctrl);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, StaAddr, ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, query_bssid(&(pmlmeinfo->network)),
					ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, wadptdata_mac_addr(Nic), ETH_ALEN);
		break;
	case Ndis802_11IBSS:
	default:
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr1, StaAddr, ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr2, wadptdata_mac_addr(Nic), ETH_ALEN);
		Func_Of_Proc_Pre_Memcpy(pwlanhdr->addr3, query_bssid(&(pmlmeinfo->network)),
					ETH_ALEN);
		break;
	}

	SetSeqNum(pwlanhdr, 0);

	if (bQoS == TRUE) {
		struct wl_ieee80211_hdr_3addr_qos *pwlanqoshdr;

		SetFrameSubType(pframe, WIFI_QOS_DATA_NULL);

		pwlanqoshdr = (struct wl_ieee80211_hdr_3addr_qos *)pframe;
		SetPriority(&pwlanqoshdr->qc, AC);
		SetEOSP(&pwlanqoshdr->qc, bEosp);

		pktlen = sizeof(struct wl_ieee80211_hdr_3addr_qos);
	} else {
		SetFrameSubType(pframe, WIFI_DATA_NULL);

		pktlen = sizeof(struct wl_ieee80211_hdr_3addr);
	}

	*pLength = pktlen;
}

VOID Func_Check_Fw_Rsvdpage_Content(IN PNIC Nic)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u32 MaxBcnPageNum;

	if (pHalData->FwRsvdPageStartOffset != 0) {
	}
}

static void Func_Of_Set_Fwrsvdpage_Cmd(PNIC Nic,
										PRSVDPAGE_LOC rsvdpageloc)
{
	u8 u1wMBOX1RsvdPageParm[wMBOX1_RSVDPAGE_LOC_LEN] = { 0 };
	MpTrace(COMP_EVENTS, DBG_NORMAL, 
		("9086XRsvdPageLoc: ProbeRsp=%d PsPoll=%d Null=%d QoSNull=%d BTNull=%d\n",
		 rsvdpageloc->LocProbeRsp, rsvdpageloc->LocPsPoll,
		 rsvdpageloc->LocNullData, rsvdpageloc->LocQosNull,
		 rsvdpageloc->LocBTQosNull));
	
	SET_9086X_wMBOX1CMD_RSVDPAGE_LOC_PROBE_RSP(u1wMBOX1RsvdPageParm,
											rsvdpageloc->LocProbeRsp);
	SET_9086X_wMBOX1CMD_RSVDPAGE_LOC_PSPOLL(u1wMBOX1RsvdPageParm,
										 rsvdpageloc->LocPsPoll);
	SET_9086X_wMBOX1CMD_RSVDPAGE_LOC_NULL_DATA(u1wMBOX1RsvdPageParm,
											rsvdpageloc->LocNullData);
	SET_9086X_wMBOX1CMD_RSVDPAGE_LOC_QOS_NULL_DATA(u1wMBOX1RsvdPageParm,
												rsvdpageloc->LocQosNull);
	SET_9086X_wMBOX1CMD_RSVDPAGE_LOC_BT_QOS_NULL_DATA(u1wMBOX1RsvdPageParm,
												   rsvdpageloc->LocBTQosNull);

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_RSVD_PAGE, wMBOX1_RSVDPAGE_LOC_LEN,
				   u1wMBOX1RsvdPageParm);

}

static void Func_Of_Set_Fwaoacrsvdpage_Cmd(PNIC Nic,
											PRSVDPAGE_LOC rsvdpageloc)
{
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	u8 res = 0, count = 0;
#ifdef CONFIG_WOWLAN
	u8 u1wMBOX1AoacRsvdPageParm[wMBOX1_AOAC_RSVDPAGE_LOC_LEN] = { 0 };
	MpTrace(COMP_EVENTS, DBG_NORMAL, 
		("9086XAOACRsvdPageLoc: RWC=%d ArpRsp=%d NbrAdv=%d GtkRsp=%d GtkInfo=%d ProbeReq=%d NetworkList=%d\n",
		 rsvdpageloc->LocRemoteCtrlInfo, rsvdpageloc->LocArpRsp,
		 rsvdpageloc->LocNbrAdv, rsvdpageloc->LocGTKRsp,
		 rsvdpageloc->LocGTKInfo, rsvdpageloc->LocProbeReq,
		 rsvdpageloc->LocNetList));

	if (check_fwstate(pmlmepriv, _FW_LINKED)) {
		SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_REMOTE_WAKE_CTRL_INFO
			(u1wMBOX1AoacRsvdPageParm, rsvdpageloc->LocRemoteCtrlInfo);
		SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_ARP_RSP(u1wMBOX1AoacRsvdPageParm,
											 rsvdpageloc->LocArpRsp);
		SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_GTK_RSP(u1wMBOX1AoacRsvdPageParm,
											 rsvdpageloc->LocGTKRsp);
		SET_wMBOX1CMD_AOAC_RSVDPAGE_LOC_GTK_INFO(u1wMBOX1AoacRsvdPageParm,
											  rsvdpageloc->LocGTKInfo);

		Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_AOAC_RSVD_PAGE,
					   wMBOX1_AOAC_RSVDPAGE_LOC_LEN, u1wMBOX1AoacRsvdPageParm);

	} else {


	}

#endif
}

static void Func_Of_Set_Fwkeepalive_Cmd(PNIC Nic, u8 benable,
										 u8 pkt_type)
{
	u8 u1wMBOX1KeepAliveParm[wMBOX1_KEEP_ALIVE_CTRL_LEN] = { 0 };
	u8 adopt = 1;

	u8 check_period = 5;
	MpTrace(COMP_EVENTS, DBG_NORMAL, 
		("%s(): benable = %d\n", __func__, benable));

	SET_9086X_wMBOX1CMD_KEEPALIVE_PARM_ENABLE(u1wMBOX1KeepAliveParm, benable);
	SET_9086X_wMBOX1CMD_KEEPALIVE_PARM_ADOPT(u1wMBOX1KeepAliveParm, adopt);
	SET_9086X_wMBOX1CMD_KEEPALIVE_PARM_PKT_TYPE(u1wMBOX1KeepAliveParm, pkt_type);
	SET_9086X_wMBOX1CMD_KEEPALIVE_PARM_CHECK_PERIOD(u1wMBOX1KeepAliveParm,
												 check_period);

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_KEEP_ALIVE, wMBOX1_KEEP_ALIVE_CTRL_LEN,
				   u1wMBOX1KeepAliveParm);
}

static void Func_Of_Set_Fwdiscondecision_Cmd(PNIC Nic, u8 benable)
{
	u8 u1wMBOX1DisconDecisionParm[wMBOX1_DISCON_DECISION_LEN] = { 0 };
	u8 adopt = 1, check_period = 10, trypkt_num = 0;

	MpTrace(COMP_EVENTS, DBG_NORMAL, 
		("%s(): benable = %d\n", __func__, benable));

	SET_9086X_wMBOX1CMD_DISCONDECISION_PARM_ENABLE(u1wMBOX1DisconDecisionParm,
												benable);
	SET_9086X_wMBOX1CMD_DISCONDECISION_PARM_ADOPT(u1wMBOX1DisconDecisionParm, adopt);
	SET_9086X_wMBOX1CMD_DISCONDECISION_PARM_CHECK_PERIOD(u1wMBOX1DisconDecisionParm,
													  check_period);
	SET_9086X_wMBOX1CMD_DISCONDECISION_PARM_TRY_PKT_NUM(u1wMBOX1DisconDecisionParm,
													 trypkt_num);

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_DISCON_DECISION, wMBOX1_DISCON_DECISION_LEN,
				   u1wMBOX1DisconDecisionParm);
}

void Func_Of_Set_Fwmacidconfig_Cmd(PNIC Nic, u8 mac_id, u8 raid,
									u8 bw, u8 sgi, u32 mask)
{

	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u8 u1wMBOX1MacIdConfigParm[wMBOX1_MACID_CFG_LEN] = { 0 };
	MpTrace(COMP_EVENTS, DBG_NORMAL, 
		("%s(): mac_id=%d raid=0x%x bw=%d mask=0x%x\n", __func__, mac_id,
			raid, bw, mask));

	_func_enter_;

	SET_9086X_wMBOX1CMD_MACID_CFG_MACID(u1wMBOX1MacIdConfigParm, mac_id);
	SET_9086X_wMBOX1CMD_MACID_CFG_RAID(u1wMBOX1MacIdConfigParm, raid);
	SET_9086X_wMBOX1CMD_MACID_CFG_SGI_EN(u1wMBOX1MacIdConfigParm, (sgi) ? 1 : 0);
	SET_9086X_wMBOX1CMD_MACID_CFG_BW(u1wMBOX1MacIdConfigParm, bw);

	if (pHalData->bDisableTXPowerTraining) {
		SET_9086X_wMBOX1CMD_MACID_CFG_DISPT(u1wMBOX1MacIdConfigParm, 1);
		MpTrace(COMP_EVENTS, DBG_NORMAL, 
		    ("%s,Disable PWT by driver\n", __func__));

	} else {
		msg_get_var(Nic);

		if (msg_rw_val.bDisablePowerTraining) {
			SET_9086X_wMBOX1CMD_MACID_CFG_DISPT(u1wMBOX1MacIdConfigParm, 1);
			MpTrace(COMP_EVENTS, DBG_NORMAL, 
				("%s,Disable PWT by DM\n", __func__));
		}

	}

	SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK0(u1wMBOX1MacIdConfigParm,
										  (u8) (mask & 0x000000ff));
	SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK1(u1wMBOX1MacIdConfigParm,
										  (u8) ((mask & 0x0000ff00) >> 8));
	SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK2(u1wMBOX1MacIdConfigParm,
										  (u8) ((mask & 0x00ff0000) >> 16));
	SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK3(u1wMBOX1MacIdConfigParm,
										  (u8) ((mask & 0xff000000) >> 24));

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_MACID_CFG, wMBOX1_MACID_CFG_LEN,
				   u1wMBOX1MacIdConfigParm);

	_func_exit_;
}

static void Func_Of_Set_Fwrssisetting_Cmd(PNIC Nic, u8 * param)
{
	u8 u1wMBOX1RssiSettingParm[wMBOX1_RSSI_SETTING_LEN] = { 0 };
	u8 mac_id = *param;
	u8 rssi = *(param + 2);
	u8 uldl_state = 0;

	_func_enter_;

	SET_9086X_wMBOX1CMD_RSSI_SETTING_MACID(u1wMBOX1RssiSettingParm, mac_id);
	SET_9086X_wMBOX1CMD_RSSI_SETTING_RSSI(u1wMBOX1RssiSettingParm, rssi);
	SET_9086X_wMBOX1CMD_RSSI_SETTING_ULDL_STATE(u1wMBOX1RssiSettingParm, uldl_state);

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_RSSI_SETTING, wMBOX1_RSSI_SETTING_LEN,
				   u1wMBOX1RssiSettingParm);

	_func_exit_;
}

void Func_Of_Set_Fwapreqrpt_Cmd(PNIC Nic, u32 need_ack)
{
	u8 u1wMBOX1ApReqRptParm[wMBOX1_AP_REQ_TXRPT_LEN] = { 0 };
	u8 macid1 = 1, macid2 = 0;
	MpTrace(COMP_EVENTS, DBG_NORMAL, 
		("%s(): need_ack = %d\n", __func__, need_ack));

	SET_9086X_wMBOX1CMD_APREQRPT_PARM_MACID1(u1wMBOX1ApReqRptParm, macid1);
	SET_9086X_wMBOX1CMD_APREQRPT_PARM_MACID2(u1wMBOX1ApReqRptParm, macid2);

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_AP_REQ_TXRPT, wMBOX1_AP_REQ_TXRPT_LEN,
				   u1wMBOX1ApReqRptParm);
}

void Func_Of_Set_Fwpwrmode_Cmd(PNIC Nic, u8 psmode)
{
	int i;
	u8 smart_ps = 0;
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	u8 u1wMBOX1PwrModeParm[wMBOX1_PWRMODE_LEN] = { 0 };
	u8 PowerState = 0, awake_intvl = 1, byte5 = 0, rlbm = 0;
#ifdef CONFIG_P2P
	struct wifidirect_info *wdinfo = &(Nic->wdinfo);
#endif

	_func_enter_;

	pwrpriv->bMailboxSync = TRUE;

	MpTrace(COMP_EVENTS, DBG_NORMAL, 
		("%s(): FW LPS mode = %d, SmartPS=%d, dtim=%d\n", __func__,
				psmode, pwrpriv->smart_ps, pwrpriv->dtim));

	if (psmode == PS_MODE_MIN) {
		rlbm = 0;
		awake_intvl = 2;
		smart_ps = pwrpriv->smart_ps;
	} else if (psmode == PS_MODE_MAX) {
		rlbm = 1;
		awake_intvl = 2;
		smart_ps = pwrpriv->smart_ps;
	} else if (psmode == PS_MODE_DTIM) {
		if (pwrpriv->dtim > 0 && pwrpriv->dtim < 16)
			awake_intvl = pwrpriv->dtim + 1;
		else
			awake_intvl = 4;

		rlbm = 2;
		smart_ps = pwrpriv->smart_ps;
	} else {
		rlbm = 2;
		awake_intvl = 4;
		smart_ps = pwrpriv->smart_ps;
	}

#ifdef CONFIG_P2P
	if (!wl_p2p_chk_state(wdinfo, P2P_STATE_NONE)) {
		awake_intvl = 2;
		rlbm = 1;
	}
#endif

	if (psmode > 0) {
		{
			PowerState = 0x00;
			byte5 = 0x40;
		}
	} else {
		PowerState = 0x0C;
		byte5 = 0x40;
	}

	SET_9086X_wMBOX1CMD_PWRMODE_PARM_MODE(u1wMBOX1PwrModeParm, (psmode > 0) ? 1 : 0);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_SMART_PS(u1wMBOX1PwrModeParm, smart_ps);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_RLBM(u1wMBOX1PwrModeParm, rlbm);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_BCN_PASS_TIME(u1wMBOX1PwrModeParm, awake_intvl);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_ALL_QUEUE_UAPSD(u1wMBOX1PwrModeParm,
												  Nic->registrypriv.
												  uapsd_enable);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_PWR_STATE(u1wMBOX1PwrModeParm, PowerState);
	SET_9086X_wMBOX1CMD_PWRMODE_PARM_BYTE5(u1wMBOX1PwrModeParm, byte5);

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_SET_PWR_MODE, wMBOX1_PWRMODE_LEN,
				   u1wMBOX1PwrModeParm);

	pwrpriv->bMailboxSync = FALSE;

	_func_exit_;
}

void Func_Of_Set_Fwpstuneparam_Cmd(PNIC Nic)
{
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;
	u8 u1wMBOX1PsTuneParm[wMBOX1_PSTUNEPARAM_LEN] = { 0 };
	u8 bcn_to_limit = 10;
	u8 dtim_timeout = 5;
	u8 ps_timeout = 20;
	u8 dtim_period = 3;

	_func_enter_;

	SET_9086X_wMBOX1CMD_PSTUNE_PARM_BCN_TO_LIMIT(u1wMBOX1PsTuneParm, bcn_to_limit);
	SET_9086X_wMBOX1CMD_PSTUNE_PARM_DTIM_TIMEOUT(u1wMBOX1PsTuneParm, dtim_timeout);
	SET_9086X_wMBOX1CMD_PSTUNE_PARM_PS_TIMEOUT(u1wMBOX1PsTuneParm, ps_timeout);
	SET_9086X_wMBOX1CMD_PSTUNE_PARM_ADOPT(u1wMBOX1PsTuneParm, 1);
	SET_9086X_wMBOX1CMD_PSTUNE_PARM_DTIM_PERIOD(u1wMBOX1PsTuneParm, dtim_period);

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_PS_TUNING_PARA, wMBOX1_PSTUNEPARAM_LEN,
				   u1wMBOX1PsTuneParm);

	_func_exit_;
}

void Func_Of_Set_Fwbtmpoper_Cmd(PNIC Nic, u8 idx, u8 ver, u8 reqnum,
								 u8 * param)
{
	u8 u1wMBOX1BtMpOperParm[wMBOX1_BTMP_OPER_LEN] = { 0 };

	_func_enter_;
	MpTrace(COMP_EVENTS, DBG_NORMAL, 
		("%s: idx=%d ver=%d reqnum=%d param1=0x%02x param2=0x%02x\n",
			__func__, idx, ver, reqnum, param[0], param[1]));

	SET_9086X_wMBOX1CMD_BT_MPOPER_VER(u1wMBOX1BtMpOperParm, ver);
	SET_9086X_wMBOX1CMD_BT_MPOPER_REQNUM(u1wMBOX1BtMpOperParm, reqnum);
	SET_9086X_wMBOX1CMD_BT_MPOPER_IDX(u1wMBOX1BtMpOperParm, idx);
	SET_9086X_wMBOX1CMD_BT_MPOPER_PARAM1(u1wMBOX1BtMpOperParm, param[0]);
	SET_9086X_wMBOX1CMD_BT_MPOPER_PARAM2(u1wMBOX1BtMpOperParm, param[1]);
	SET_9086X_wMBOX1CMD_BT_MPOPER_PARAM3(u1wMBOX1BtMpOperParm, param[2]);

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_BT_MP_OPER, wMBOX1_BTMP_OPER_LEN,
				   u1wMBOX1BtMpOperParm);

	_func_exit_;
}

void Func_Of_Set_Fwpwrmodeinips_Cmd(PNIC Nic, u8 cmd_param)
{
	MpTrace(COMP_EVENTS, DBG_NORMAL,("%s()\n", __func__));

	cmd_param = cmd_param;

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_FWLPS_IN_IPS_, 1, &cmd_param);
}

static s32 Func_Of_Set_Fwlowpwrlps_Cmd(PNIC Nic, u8 enable)
{
	return FALSE;
}

extern void Func_Of_Proc_Chip_Hw_Set_Fwaoacrsvdpage_Cmd(PNIC Nic,
										   PRSVDPAGE_LOC rsvdpageloc);

void Func_Of_Download_Rsvd_Page(PNIC Nic, u8 mstatus)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;

	BOOLEAN bcn_valid = FALSE;
	u8 DLBcnCount = 0;
	u32 poll = 0;
	u8 val8;
	u8 v8;

	RSVDPAGE_LOC RsvdPageLoc;
	int ret;

	_func_enter_;
//	MpTrace(COMP_EVENTS, DBG_NORMAL, 
//		(": iface_type=%d mstatus(%x)\n",
//			get_iface_type(Nic), mstatus));

	if (mstatus == WP_MEDIA_CONNECT) {

		Func_Wf_Set_Fw_Lps_Config(Nic);

		DLBcnCount = 0;
		poll = 0;
		do {
			Func_Of_Proc_Chip_Hw_Set_Fw_Rsvd_Page(Nic, 0);

			DLBcnCount++;
			do {
				Func_Of_Proc_Yield_Os();
				Func_Of_Proc_Chip_Hw_Get_Hwreg(Nic, HW_VAR_BCN_VALID,
								  (u8 *) (&bcn_valid));
				poll++;
			} while (!bcn_valid && (poll % 10) != 0
					 && !WL_CANNOT_RUN(Nic));

		} while (!bcn_valid && DLBcnCount <= 100 && !WL_CANNOT_RUN(Nic));

		if (WL_CANNOT_RUN(Nic)) ;
		else if (!bcn_valid)
			{
			MpTrace(COMP_EVENTS, DBG_NORMAL, 
				(": 1 DL RSVD page failed! DLBcnCount:%u, poll:%u\n",
					DLBcnCount, poll));
			}
		else {
			pwrpriv->fw_psmode_iface_id = Nic->iface_id;
			MpTrace(COMP_EVENTS, DBG_NORMAL, 
				(": 1 DL RSVD page success! DLBcnCount:%u, poll:%u\n",
					 DLBcnCount, poll));
		}

		memset((u8 *) & RsvdPageLoc, 0, sizeof(RsvdPageLoc));

		RsvdPageLoc.LocProbeRsp = 0;
		RsvdPageLoc.LocPsPoll = 2;
		RsvdPageLoc.LocNullData = 3;
		RsvdPageLoc.LocQosNull = 4;
		RsvdPageLoc.LocBTQosNull = 0;

		if (check_fwstate(pmlmepriv, _FW_LINKED) == TRUE) {
			Func_Of_Proc_Chip_Hw_Set_Fwrsvdpage_Cmd(Nic, &RsvdPageLoc);
		}
#ifdef CONFIG_WOWLAN
		RsvdPageLoc.LocRemoteCtrlInfo = 6;
		RsvdPageLoc.LocArpRsp = 5;
#endif
		MpTrace(COMP_EVENTS, DBG_NORMAL, 
			("%s %d pwrctl->wowlan_mode  = %d\n", __func__, __LINE__,
				pwrpriv->wowlan_mode));

		if (pwrpriv->wowlan_mode == TRUE)
			Func_Of_Proc_Chip_Hw_Set_Fwaoacrsvdpage_Cmd(Nic, &RsvdPageLoc);

		ret =
			Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_LPS_SET, NULL,
										NULL, 0, 0);
		if (!ret) {

		}

	}

	_func_exit_;
}

void Func_Of_Set_Rssi_Cmd(PNIC Nic, u8 * param)
{
	Func_Of_Set_Fwrssisetting_Cmd(Nic, param);
}

void Func_Of_Set_Fwjoinbssrpt_Cmd(PNIC Nic, u8 mstatus)
{
	struct sta_info *psta = NULL;
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;

	if (mstatus == 1)
		Func_Of_Download_Rsvd_Page(Nic, WP_MEDIA_CONNECT);
}

void Func_Of_Add_Rateatid(PNIC Nic, u64 rate_bitmap, u8 * arg,
						   u8 rssi_level)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	struct macid_ctl_t *macid_ctl = &Nic->dvobjpriv.macid_ctl;
	struct sta_info *psta = NULL;
	u8 mac_id = arg[0];
	u8 raid = arg[1];
	u8 shortGI = arg[2];
	u8 bw;
	u32 bitmap = (u32) rate_bitmap;
	u32 mask = bitmap & 0x0FFFFFFF;

	if (mac_id < macid_ctl->num)
		psta = macid_ctl->sta[mac_id];
	if (psta == NULL) {
		MpTrace(COMP_EVENTS, DBG_NORMAL, 
			(" macid:%u, sta is NULL\n", mac_id));
		return;
	}

	bw = psta->bw_mode;

	if (rssi_level != DM_RATR_STA_INIT)
		mask = Func_Hw_Op_Get_Rate_Bitmap(Nic, mac_id, mask, rssi_level);
	MpTrace(COMP_EVENTS, DBG_NORMAL, 
		("%s(): mac_id=%d raid=0x%x bw=%d mask=0x%x\n", __func__, mac_id,
			raid, bw, mask));
	
	Func_Of_Set_Fwmacidconfig_Cmd(Nic, mac_id, raid, bw, shortGI, mask);
}

#ifdef CONFIG_P2P
void Func_Of_Set_P2P_Ps_Offload_Cmd(PNIC Nic, u8 p2p_ps_state)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;

	struct wifidirect_info *pwdinfo = &(Nic->wdinfo);
	struct P2P_PS_Offload_t *p2p_ps_offload =
		(struct P2P_PS_Offload_t *)(&pHalData->p2p_ps_offload);
	u8 i;

	_func_enter_;

#if 0

	switch (p2p_ps_state) {
	case P2P_PS_DISABLE:
		WL_INFO("P2P_PS_DISABLE\n");
		Func_Of_Proc_Pre_Memset(p2p_ps_offload, 0, 1);
		break;
	case P2P_PS_ENABLE:
		WL_INFO("P2P_PS_ENABLE\n");
		if (pwdinfo->ctwindow > 0) {
			p2p_ps_offload->CTWindow_En = 1;
			wl_write8(Nic, REG_P2P_CTWIN, pwdinfo->ctwindow);
		}

		for (i = 0; i < pwdinfo->noa_num; i++) {
			wl_write8(Nic, REG_NOA_DESC_SEL, (i << 4));
			if (i == 0)
				p2p_ps_offload->NoA0_En = 1;
			else
				p2p_ps_offload->NoA1_En = 1;

			wl_write32(Nic, REG_NOA_DESC_DURATION,
						pwdinfo->noa_duration[i]);

			wl_write32(Nic, REG_NOA_DESC_INTERVAL,
						pwdinfo->noa_interval[i]);

			wl_write32(Nic, REG_NOA_DESC_START,
						pwdinfo->noa_start_time[i]);

			wl_write8(Nic, REG_NOA_DESC_COUNT, pwdinfo->noa_count[i]);
		}

		if ((pwdinfo->opp_ps == 1) || (pwdinfo->noa_num > 0)) {
			wl_write8(Nic, REG_DUAL_TSF_RST, BIT(4));

			p2p_ps_offload->Offload_En = 1;

			if (pwdinfo->role == P2P_ROLE_GO) {
				p2p_ps_offload->role = 1;
				p2p_ps_offload->AllStaSleep = 0;
			} else
				p2p_ps_offload->role = 0;

			p2p_ps_offload->discovery = 0;
		}
		break;
	case P2P_PS_SCAN:
		WL_INFO("P2P_PS_SCAN\n");
		p2p_ps_offload->discovery = 1;
		break;
	case P2P_PS_SCAN_DONE:
		WL_INFO("P2P_PS_SCAN_DONE\n");
		p2p_ps_offload->discovery = 0;
		pwdinfo->p2p_ps_state = P2P_PS_ENABLE;
		break;
	default:
		break;
	}

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_P2P_PS_OFFLOAD, 1,
				   (u8 *) p2p_ps_offload);

#endif

	_func_exit_;

}
#endif

#ifdef CONFIG_TSF_RESET_OFFLOAD
u8 Func_Of_Reset_Tsf(PNIC Nic, u8 reset_port)
{
	u8 buf[2];
	u8 res = TRUE;

	_func_enter_;
	if (IFACE_PORT0 == reset_port) {
		buf[0] = 0x1;
		buf[1] = 0;

	} else {
		buf[0] = 0x0;
		buf[1] = 0x1;
	}

	Func_Mcu_Fill_wMBOX1_Fw(Nic, wMBOX1_9086X_RESET_TSF, 2, buf);

	_func_exit_;

	return res;
}
#endif
