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




static int wl_chip_version = 0x00;
static int wl_rfintfs = HWPI;
static int wl_lbkmode = 0;

static int wl_network_mode = Ndis802_11IBSS;
static int wl_channel = 1;
static int wl_wireless_mode = WIRELESS_MODE_MAX;
static int wl_vwltcarrier_sense = AUTO_VCS;
static int wl_vcs_type = RTS_CTS;
#ifdef CONFIG_RTSCTS
static int wl_rts_thresh = 500;
#else
static int wl_rts_thresh = 2347;
#endif
static int wl_frag_thresh = 2346;
static int wl_preamble = PREAMBLE_LONG;
static int wl_scan_mode = 1;
static int wl_adhoc_tx_pwr = 1;
static int wl_soft_ap = 0;
#ifdef CONFIG_POWER_SAVING
static int wl_power_mgnt = PS_MODE_MAX;
#else
static int wl_power_mgnt = PS_MODE_ACTIVE;
#endif

static int wl_smart_ps = 2;

static int wl_usb_rxagg_mode = 2;
static int wl_radio_enable = 1;
static int wl_long_retry_lmt = 7;
static int wl_short_retry_lmt = 7;
static int wl_busy_thresh = 40;
static int wl_ack_policy = NORMAL_ACK;

static int wl_mp_mode = 0;

static int wl_software_encrypt = 0;
static int wl_software_decrypt = 0;

static int wl_acm_method = 0;

static int wl_wmm_enable = 1;
static int wl_uapsd_enable = 0;
static int wl_uapsd_max_sp = NO_LIMIT;
static int wl_uapsd_acbk_en = 0;
static int wl_uapsd_acbe_en = 0;
static int wl_uapsd_acvi_en = 0;
static int wl_uapsd_acvo_en = 0;

static int wl_pwrtrim_enable = 0;

#ifdef CONFIG_80211N_HT
int wl_ht_enable = 1;


#if defined(CONFIG_BW_20M) 
int wl_bw_mode = 0; //fix by chuck ,bw : 1-> ,40mhz -> 20mhz
#else
int wl_bw_mode = 1; //fix by chuck ,bw : 1-> ,40mhz -> 20mhz
#endif

int wl_ampdu_enable = 1;
static int wl_rx_stbc = 1;
static int wl_ampdu_amsdu = 0;
static int wl_short_gi = 0xf;
static int wl_ldpc_cap = 0x00;
static int wl_stbc_cap = 0x13;

#endif

static int wl_lowrate_two_xmit = 1;

static int wl_rf_config = 0xF;

static int wl_low_power = 0;

static int wl_special_rf_path = 1;

static char wl_country_unspecified[] = { 0xFF, 0xFF, 0x00 };

static char *wl_country_code = wl_country_unspecified;

static int wl_channel_plan = WL_CHPLAN_MAX;

#ifdef CONFIG_FULL_CH_IN_P2P_HANDSHAKE
static int wl_full_ch_in_p2p_handshake = 1;
#else
static int wl_full_ch_in_p2p_handshake = 0;
#endif

static int wl_AcceptAddbaReq = TRUE;

static int wl_antdiv_cfg = 2;
static int wl_antdiv_type = 0;

static int wl_switch_usb3 = FALSE;

#ifdef CONFIG_USB_AUTOSUSPEND
static int wl_enusbss = 1;
#else
static int wl_enusbss = 0;
#endif

#ifdef CONFIG_HW_PWRP_DETECTION
static int wl_hwpwrp_detect = 1;
#else
static int wl_hwpwrp_detect = 0;
#endif

static int wl_hw_wps_pbc = 1;

#ifdef CONFIG_TX_MCAST2UNI
int wl_mc2u_disable = 0;
#endif

#ifdef CONFIG_80211D
static int wl_80211d = 0;
#endif

#ifdef CONFIG_SPECIAL_SETTING_FOR_FUNAI_TV
static int wl_force_ant = 2;
static int wl_force_igi = 0;
#endif

#ifdef CONFIG_QOS_OPTIMIZATION
static int wl_qos_opt_enable = 1;
#else
static int wl_qos_opt_enable = 0;
#endif


#ifdef CONFIG_AUTO_CHNL_SEL_NHM
static int wl_acs_mode = 1;
static int wl_acs_auto_scan = 0;
#endif

char *ifname = "wlan%d";
char *if2name = "wlan%d";
char *wl_initmac = 0;

#ifdef CONFIG_LAYER2_ROAMING
uint wl_max_roaming_times = 2;
#endif

static uint wl_notch_filter = 0;
static uint wl_hiq_filter = CONFIG_WL_HIQ_FILTER;
static uint wl_adaptivity_mode = CONFIG_WL_ADAPTIVITY_MODE;
static uint wl_adaptivity_dml = CONFIG_WL_ADAPTIVITY_DML;
static uint wl_adaptivity_dc_backoff = CONFIG_WL_ADAPTIVITY_DC_BACKOFF;
static int wl_adaptivity_th_l2h_ini = CONFIG_WL_ADAPTIVITY_TH_L2H_INI;
static int wl_adaptivity_th_edcca_hl_diff = CONFIG_WL_ADAPTIVITY_TH_EDCCA_HL_DIFF;
static uint wl_amplifier_type_2g = CONFIG_WL_AMPLIFIER_TYPE_2G;
static uint wl_RFE_type = CONFIG_WL_RFE_TYPE;
static uint wl_GLNA_type = CONFIG_WL_GLNA_TYPE;
static uint wl_TxBBSwing_2G = 0xFF;
static uint wl_OffEfuseMask = 1;
static uint wl_FileMaskEfuse = 0;
static uint wl_pll_ref_clk_sel = CONFIG_WL_PLL_REF_CLK_SEL;

#if defined(CONFIG_CALIBRATE_TX_POWER_BY_REGULATORY)
int wl_tx_pwr_lmt_enable = 1;
int wl_tx_pwr_by_rate = 1;
#else

static int wl_tx_pwr_lmt_enable = 0;
static int wl_tx_pwr_by_rate = 0;

#endif

static int wl_target_tx_pwr_2g_a[RATE_SECTION_NUM] =
	CONFIG_WL_TARGET_TX_PWR_2G_A;
static int wl_target_tx_pwr_2g_a_num = 0;

int Func_Netdev_Open(struct net_device *pnetdev);
int Func_Pre_Netdev_Open(struct net_device *pnetdev);
static int Func_Netdev_Close(struct net_device *pnetdev);

void Func_Of_Proc_Regsty_Load_Target_Tx_Power(struct registry_priv *regsty)
{
	int rs;
	int *target_tx_pwr;

	target_tx_pwr = wl_target_tx_pwr_2g_a;

	for (rs = CCK; rs < RATE_SECTION_NUM; rs++)
		regsty->target_tx_pwr_2g[0][rs] = target_tx_pwr[rs];

}

uint Func_Loadparam(PNIC Nic)
{
	uint status = TRUE;
	struct registry_priv *registry_par = &Nic->registrypriv;

	_func_enter_;

	registry_par->chip_version = (u8) wl_chip_version;
	registry_par->rfintfs = (u8) wl_rfintfs;
	registry_par->lbkmode = (u8) wl_lbkmode;
	registry_par->network_mode = (u8) wl_network_mode;

	Func_Of_Proc_Pre_Memcpy(registry_par->ssid.Ssid, "ANY", 3);
	registry_par->ssid.SsidLength = 3;

	registry_par->channel = (u8) wl_channel;
	registry_par->wireless_mode = (u8) wl_wireless_mode;

	if (IsSupported24G(registry_par->wireless_mode)
		&& (registry_par->channel > 14)) {
		registry_par->channel = 1;
	}

	registry_par->vwltcarrier_sense = (u8) wl_vwltcarrier_sense;
	registry_par->vcs_type = (u8) wl_vcs_type;
	registry_par->rts_thresh = (u16) wl_rts_thresh;
	registry_par->frag_thresh = (u16) wl_frag_thresh;
	registry_par->preamble = (u8) wl_preamble;
	registry_par->scan_mode = (u8) wl_scan_mode;
	registry_par->adhoc_tx_pwr = (u8) wl_adhoc_tx_pwr;
	registry_par->soft_ap = (u8) wl_soft_ap;
	registry_par->smart_ps = (u8) wl_smart_ps;
	registry_par->power_mgnt = (u8) wl_power_mgnt;
	registry_par->radio_enable = (u8) wl_radio_enable;
	registry_par->long_retry_lmt = (u8) wl_long_retry_lmt;
	registry_par->short_retry_lmt = (u8) wl_short_retry_lmt;
	registry_par->busy_thresh = (u16) wl_busy_thresh;
	registry_par->ack_policy = (u8) wl_ack_policy;
	registry_par->mp_mode = (u8) wl_mp_mode;
	registry_par->software_encrypt = (u8) wl_software_encrypt;
	registry_par->software_decrypt = (u8) wl_software_decrypt;

	registry_par->acm_method = (u8) wl_acm_method;
	registry_par->usb_rxagg_mode = (u8) wl_usb_rxagg_mode;

	registry_par->wmm_enable = (u8) wl_wmm_enable;
	registry_par->uapsd_enable = (u8) wl_uapsd_enable;
	registry_par->uapsd_max_sp = (u8) wl_uapsd_max_sp;
	registry_par->uapsd_acbk_en = (u8) wl_uapsd_acbk_en;
	registry_par->uapsd_acbe_en = (u8) wl_uapsd_acbe_en;
	registry_par->uapsd_acvi_en = (u8) wl_uapsd_acvi_en;
	registry_par->uapsd_acvo_en = (u8) wl_uapsd_acvo_en;

	registry_par->RegPwrTrimEnable = (u8) wl_pwrtrim_enable;

#ifdef CONFIG_80211N_HT
	registry_par->ht_enable = (u8) wl_ht_enable;
	registry_par->bw_mode = (u8) wl_bw_mode;
	registry_par->ampdu_enable = (u8) wl_ampdu_enable;
	registry_par->rx_stbc = (u8) wl_rx_stbc;
	registry_par->ampdu_amsdu = (u8) wl_ampdu_amsdu;
	registry_par->short_gi = (u8) wl_short_gi;
	registry_par->ldpc_cap = (u8) wl_ldpc_cap;
	registry_par->stbc_cap = (u8) wl_stbc_cap;
#endif

	registry_par->lowrate_two_xmit = (u8) wl_lowrate_two_xmit;
	registry_par->rf_config = (u8) wl_rf_config;
	registry_par->low_power = (u8) wl_low_power;

	if (strlen(wl_country_code) != 2
		|| Func_Is_Alpha(wl_country_code[0]) == FALSE
		|| Func_Is_Alpha(wl_country_code[1]) == FALSE) {
		if (wl_country_code != wl_country_unspecified)
			MpTrace(COMP_RICH_INIT, DBG_NORMAL, 
			    ("%s discard wl_country_code not in alpha2\n", __func__));
		Func_Of_Proc_Pre_Memset(registry_par->alpha2, 0xFF, 2);
	} else
		Func_Of_Proc_Pre_Memcpy(registry_par->alpha2, wl_country_code, 2);

	registry_par->channel_plan = (u8) wl_channel_plan;
	registry_par->special_rf_path = (u8) wl_special_rf_path;

	registry_par->full_ch_in_p2p_handshake = (u8) wl_full_ch_in_p2p_handshake;

	registry_par->bAcceptAddbaReq = (u8) wl_AcceptAddbaReq;

	registry_par->antdiv_cfg = (u8) wl_antdiv_cfg;
	registry_par->antdiv_type = (u8) wl_antdiv_type;

	registry_par->switch_usb3 = (u8) wl_switch_usb3;

#ifdef CONFIG_AUTOSUSPEND
	registry_par->usbss_enable = (u8) wl_enusbss;
#endif

	registry_par->hw_wps_pbc = (u8) wl_hw_wps_pbc;

#ifdef CONFIG_LAYER2_ROAMING
	registry_par->max_roaming_times = (u8) wl_max_roaming_times;
#endif

#ifdef CONFIG_80211D
	registry_par->enable80211d = (u8) wl_80211d;
#endif

	MpTrace(COMP_RICH_INIT, DBG_NORMAL, 
		(registry_par->ifname, 16, "%s", ifname));

	MpTrace(COMP_RICH_INIT, DBG_NORMAL, 
		(registry_par->if2name, 16, "%s", if2name));

	registry_par->notch_filter = (u8) wl_notch_filter;

#ifdef CONFIG_SPECIAL_SETTING_FOR_FUNAI_TV
	registry_par->force_ant = (u8) wl_force_ant;
	registry_par->force_igi = (u8) wl_force_igi;
#endif

	registry_par->pll_ref_clk_sel = (u8) wl_pll_ref_clk_sel;

	registry_par->RegEnableTxPowerLimit = (u8) wl_tx_pwr_lmt_enable;
	registry_par->RegEnableTxPowerByRate = (u8) wl_tx_pwr_by_rate;

	Func_Of_Proc_Regsty_Load_Target_Tx_Power(registry_par);

	registry_par->RegPowerBase = 14;
	registry_par->TxBBSwing_2G = (s8) wl_TxBBSwing_2G;
	registry_par->bEn_RFE = 1;
	registry_par->RFE_Type = (u8) wl_RFE_type;
	registry_par->AmplifierType_2G = (u8) wl_amplifier_type_2g;
	registry_par->GLNA_Type = (u8) wl_GLNA_type;
	registry_par->qos_opt_enable = (u8) wl_qos_opt_enable;

	registry_par->hiq_filter = (u8) wl_hiq_filter;

	registry_par->adaptivity_mode = (u8) wl_adaptivity_mode;
	registry_par->adaptivity_dml = (u8) wl_adaptivity_dml;
	registry_par->adaptivity_dc_backoff = (u8) wl_adaptivity_dc_backoff;
	registry_par->adaptivity_th_l2h_ini = (s8) wl_adaptivity_th_l2h_ini;
	registry_par->adaptivity_th_edcca_hl_diff =
		(s8) wl_adaptivity_th_edcca_hl_diff;

	registry_par->boffefusemask = (u8) wl_OffEfuseMask;
	registry_par->bFileMaskEfuse = (u8) wl_FileMaskEfuse;
#ifdef CONFIG_AUTO_CHNL_SEL_NHM
	registry_par->acs_mode = (u8) wl_acs_mode;
	registry_par->acs_auto_scan = (u8) wl_acs_auto_scan;
#endif
	_func_exit_;

	return status;
}

#if 0
static int Func_Of_Proc_Net_Set_Mac_Address(struct net_device *pnetdev, void *addr)
{
	_wadptdata *Nic = (_wadptdata *) wl_netdev_priv(pnetdev);
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct sockaddr *sa = (struct sockaddr *)addr;
	int ret = -1;

	if ((pnetdev->flags & IFF_UP) == TRUE) {
		WL_INFO(FUNC_ADPT_FMT ": The net_device's is not in down state\n",
				FUNC_ADPT_ARG(Nic));

		return ret;
	}

	if (check_fwstate(pmlmepriv, _FW_UNDER_LINKING) ||
		check_fwstate(pmlmepriv, _FW_LINKED) ||
		check_fwstate(pmlmepriv, _FW_UNDER_SURVEY)) {
		WL_INFO(FUNC_ADPT_FMT ": The net_device's is not idle currently\n",
				FUNC_ADPT_ARG(Nic));

		return ret;
	}

	if (invalid_mac_address_to_check_func(sa->sa_data, FALSE, 1) == TRUE) {
		WL_INFO(FUNC_ADPT_FMT ": Invalid Mac Addr for " MAC_FMT "\n",
				FUNC_ADPT_ARG(Nic), MAC_ARG(sa->sa_data));

		return ret;
	}

	Func_Of_Proc_Pre_Memcpy(wadptdata_mac_addr(Nic), sa->sa_data, ETH_ALEN);
	Func_Of_Proc_Pre_Memcpy(pnetdev->dev_addr, sa->sa_data, ETH_ALEN);

	do_deny_ps(Nic, PS_DENY_IOCTL, 1);
	left_aps_direct(Nic, 1);
	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_MAC_ADDR, sa->sa_data);
	do_cancel_deny_ps(Nic, PS_DENY_IOCTL, 1);

	WL_INFO(FUNC_ADPT_FMT ": Set Mac Addr to " MAC_FMT " Successfully\n",
			FUNC_ADPT_ARG(Nic), MAC_ARG(sa->sa_data));

	ret = 0;

	return ret;
}

static struct net_device_stats *Func_Of_Proc_Net_Get_Stats(struct net_device *pnetdev)
{
	_wadptdata *Nic = (_wadptdata *) wl_netdev_priv(pnetdev);
	struct xmit_priv *pxmitpriv = &(Nic->xmitpriv);
	struct recv_priv *precvpriv = &(Nic->recvpriv);

	Nic->stats.tx_packets = pxmitpriv->tx_pkts;
	Nic->stats.rx_packets = precvpriv->rx_pkts;
	Nic->stats.tx_dropped = pxmitpriv->tx_drop;
	Nic->stats.rx_dropped = precvpriv->rx_drop;
	Nic->stats.tx_bytes = pxmitpriv->tx_bytes;
	Nic->stats.rx_bytes = precvpriv->rx_bytes;

	return &Nic->stats;
}

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,35))
static const u16 wl_1d_to_queue[8] = { 2, 3, 3, 2, 1, 1, 0, 0 };

static unsigned int Func_Of_Proc_Classify8021D(struct sk_buff *skb)
{
	unsigned int dscp;

	if (skb->priority >= 256 && skb->priority <= 263)
		return skb->priority - 256;

	switch (skb->protocol) {
	case htons(ETH_P_IP):
		dscp = ip_hdr(skb)->tos & 0xfc;
		break;
	default:
		return 0;
	}

	return dscp >> 5;
}

static u16 Func_Of_Proc_Select_Queue(struct net_device *dev, struct sk_buff *skb
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
							, void *accel_priv
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
							, select_queue_fallback_t fallback
#endif
#endif
	)
{
	_wadptdata *Nic = wl_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;

	skb->priority = Func_Of_Proc_Classify8021D(skb);

	if (pmlmepriv->acm_mask != 0) {
		skb->priority = chk_qos(pmlmepriv->acm_mask, skb->priority, 1);
	}

	return wl_1d_to_queue[skb->priority];
}

u16 Func_Of_Proc_Recv_Select_Queue(struct sk_buff * skb)
{
	struct iphdr *piphdr;
	unsigned int dscp;
	u16 eth_type;
	u32 priority;
	u8 *pdata = skb->data;

	Func_Of_Proc_Pre_Memcpy(&eth_type, pdata + (ETH_ALEN << 1), 2);

	switch (eth_type) {
	case htons(ETH_P_IP):

		piphdr = (struct iphdr *)(pdata + ETH_HLEN);

		dscp = piphdr->tos & 0xfc;

		priority = dscp >> 5;

		break;
	default:
		priority = 0;
	}

	return wl_1d_to_queue[priority];

}

#endif
static int Func_Of_Proc_Ndev_Notifier_Call(struct notifier_block *nb,
								  unsigned long state, void *ptr)
{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(3,11,0))
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
#else
	struct net_device *dev = ptr;
#endif

	if (dev == NULL)
		return NOTIFY_DONE;

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,29))
	if (dev->netdev_ops == NULL)
		return NOTIFY_DONE;

	if (dev->netdev_ops->ndo_do_ioctl == NULL)
		return NOTIFY_DONE;

	if (dev->netdev_ops->ndo_do_ioctl != Func_Of_Proc_Ioctl)
#else
	if (dev->do_ioctl == NULL)
		return NOTIFY_DONE;

	if (dev->do_ioctl != Func_Of_Proc_Ioctl)
#endif
		return NOTIFY_DONE;

	WL_INFO_L1(FUNC_NDEV_FMT " state:%lu\n", FUNC_NDEV_ARG(dev),
			   state);

	switch (state) {
	case NETDEV_CHANGENAME:
		break;
	}

	return NOTIFY_DONE;
}

static struct notifier_block wl_ndev_notifier = {
	.notifier_call = Func_Of_Proc_Ndev_Notifier_Call,
};

int Func_Of_Proc_Ndev_Notifier_Register(void)
{
	return register_netdevice_notifier(&wl_ndev_notifier);
}

void Func_Of_Proc_Ndev_Notifier_Unregister(void)
{
	unregister_netdevice_notifier(&wl_ndev_notifier);
}

int Func_Of_Proc_Ndev_Init(struct net_device *dev)
{
	_wadptdata *wadptdata = wl_netdev_priv(dev);

	WL_INFO_L1(FUNC_ADPT_FMT " if%d mac_addr=" MAC_FMT "\n",
			   FUNC_ADPT_ARG(wadptdata), (wadptdata->iface_id + 1),
			   MAC_ARG(dev->dev_addr));
	strncpy(wadptdata->old_ifname, dev->name, IFNAMSIZ);
	wadptdata->old_ifname[IFNAMSIZ - 1] = '\0';
	wl_wadptdata_proc_init(dev);

	return 0;
}

void Func_Of_Proc_Ndev_Uninit(struct net_device *dev)
{
	_wadptdata *wadptdata = wl_netdev_priv(dev);

	WL_INFO_L1(FUNC_ADPT_FMT " if%d\n", FUNC_ADPT_ARG(wadptdata),
			   (wadptdata->iface_id + 1));
	wl_wadptdata_proc_deinit(dev);
}

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,29))
static const struct net_device_ops wl_netdev_ops = {
	.ndo_init = Func_Of_Proc_Ndev_Init,
	.ndo_uninit = Func_Of_Proc_Ndev_Uninit,
	.ndo_open = Func_Pre_Netdev_Open,
	.ndo_stop = Func_Netdev_Close,
	.ndo_start_xmit = wl_do_tx_entry,
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,35))
	.ndo_select_queue = Func_Of_Proc_Select_Queue,
#endif
	.ndo_set_mac_address = Func_Of_Proc_Net_Set_Mac_Address,
	.ndo_get_stats = Func_Of_Proc_Net_Get_Stats,
	.ndo_do_ioctl = Func_Of_Proc_Ioctl,
};
#endif

int Func_Of_Proc_Init_Netdev_Name(struct net_device *pnetdev, const char *ifname)
{
	_wadptdata *Nic = wl_netdev_priv(pnetdev);

	if (dev_alloc_name(pnetdev, ifname) < 0) {
		WL_INFO_L2(("dev_alloc_name, fail! \n"));
	}

	netif_carrier_off(pnetdev);

	return 0;
}

static void Func_Of_Proc_Hook_If_Ops(struct net_device *ndev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	ndev->netdev_ops = &wl_netdev_ops;
#else
	ndev->init = Func_Of_Proc_Ndev_Init;
	ndev->uninit = Func_Of_Proc_Ndev_Uninit;
	ndev->open = Func_Pre_Netdev_Open;
	ndev->stop = Func_Netdev_Close;
	ndev->hard_start_xmit = wl_do_tx_entry;
	ndev->set_mac_address = Func_Of_Proc_Net_Set_Mac_Address;
	ndev->get_stats = Func_Of_Proc_Net_Get_Stats;
	ndev->do_ioctl = Func_Of_Proc_Ioctl;
#endif
}

struct net_device *Func_Of_Proc_Init_Netdev(_wadptdata * old_pwadptdata)
{
	_wadptdata *Nic;
	struct net_device *pnetdev;

	WL_INFO_L2(("+init_net_dev\n"));

	if (old_pwadptdata != NULL)
		pnetdev =
			Func_Of_Proc_Alloc_Etherdev_With_Old_Priv(sizeof(_wadptdata),
											 (void *)old_pwadptdata);
	else
		pnetdev = Func_Of_Proc_Alloc_Etherdev(sizeof(_wadptdata));

	if (!pnetdev)
		return NULL;

	Nic = wl_netdev_priv(pnetdev);
	Nic->pnetdev = pnetdev;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	SET_MODULE_OWNER(pnetdev);
#endif

	Func_Of_Proc_Hook_If_Ops(pnetdev);

	pnetdev->watchdog_timeo = HZ * 3;

#ifdef CONFIG_WIRELESS_EXT
	pnetdev->wireless_handlers = (struct iw_handler_def *)&wl_handlers_def;
#endif

	return pnetdev;
}

int Func_Of_Proc_Os_Ndev_Alloc(_wadptdata * wadptdata)
{
	int ret = FALSE;
	struct net_device *ndev = NULL;

	ndev = Func_Of_Proc_Init_Netdev(wadptdata);
	if (ndev == NULL) {
		wl_warn_on(1);
		goto exit;
	}
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0)
	SET_NETDEV_DEV(ndev, dvobj_to_dev(wadptdata_to_dvobj(wadptdata)));
#endif

	ret = TRUE;

free_ndev:
	if (ret != TRUE && ndev)
		Func_Of_Proc_Free_Netdev(ndev);
exit:
	return ret;
}

void Func_Of_Proc_Os_Ndev_Free(_wadptdata * wadptdata)
{
	if (wadptdata->pnetdev) {
		Func_Of_Proc_Free_Netdev(wadptdata->pnetdev);
		wadptdata->pnetdev = NULL;
	}
}

static int Func_Of_Proc_Os_Ndev_Register(_wadptdata * wadptdata, char *name)
{
	int ret = TRUE;
	struct net_device *ndev = wadptdata->pnetdev;

	Func_Of_Proc_Init_Netdev_Name(ndev, name);

	Func_Of_Proc_Pre_Memcpy(ndev->dev_addr, wadptdata_mac_addr(wadptdata), ETH_ALEN);

	if (register_netdev(ndev) != 0) {
		WL_INFO(FUNC_NDEV_FMT " if%d Failed!\n", FUNC_NDEV_ARG(ndev),
				(wadptdata->iface_id + 1));
		ret = FALSE;
	}

exit:
	return ret;
}

static void Func_Of_Proc_Os_Ndev_Unregister(_wadptdata * wadptdata)
{
	struct net_device *netdev = NULL;

	if (wadptdata == NULL)
		return;

	wadptdata->ndev_unregistering = 1;

	netdev = wadptdata->pnetdev;

	if ((wadptdata->DriverState != DRIVER_DISAPPEAR) && netdev)
		unregister_netdev(netdev);

	wadptdata->ndev_unregistering = 0;
}

int Func_Of_Proc_Os_Ndev_Init(_wadptdata * wadptdata, char *name)
{
	int ret = FALSE;

	if (Func_Of_Proc_Os_Ndev_Alloc(wadptdata) != TRUE)
		goto exit;

	if (Func_Of_Proc_Os_Ndev_Register(wadptdata, name) != TRUE)
		goto os_ndev_free;

	ret = TRUE;

os_ndev_free:
	if (ret != TRUE)
		Func_Of_Proc_Os_Ndev_Free(wadptdata);
exit:
	return ret;
}

void Func_Of_Proc_Os_Ndev_Deinit(_wadptdata * wadptdata)
{
	Func_Of_Proc_Os_Ndev_Unregister(wadptdata);
	Func_Of_Proc_Os_Ndev_Free(wadptdata);
}

static int Func_Of_Proc_Os_Ndevs_Alloc(struct dvobj_priv *dvobj)
{
	int i, status = TRUE;
	_wadptdata *wadptdata;


	for (i = 0; i < dvobj->iface_nums; i++) {

		if (i >= IFACE_ID_MAX) {
			WL_INFO_L1("%s %d >= IFACE_ID_MAX\n", __func__, i);
			wl_warn_on(1);
			continue;
		}

		wadptdata = dvobj->pwadptdatas[i];
		if (wadptdata && !wadptdata->pnetdev) {
			status = Func_Of_Proc_Os_Ndev_Alloc(wadptdata);
			if (status != TRUE) {
				wl_warn_on(1);
				break;
			}
		}
	}

	if (status != TRUE) {
		for (; i >= 0; i--) {
			wadptdata = dvobj->pwadptdatas[i];
			if (wadptdata && wadptdata->pnetdev)
				Func_Of_Proc_Os_Ndev_Free(wadptdata);
		}
	}
exit:
	return status;
}

void Func_Of_Proc_Os_Ndevs_Free(struct dvobj_priv *dvobj)
{
	int i;
	_wadptdata *wadptdata = NULL;

	for (i = 0; i < dvobj->iface_nums; i++) {

		if (i >= IFACE_ID_MAX) {
			WL_INFO_L1("%s %d >= IFACE_ID_MAX\n", __func__, i);
			wl_warn_on(1);
			continue;
		}

		wadptdata = dvobj->pwadptdatas[i];

		if (wadptdata == NULL)
			continue;

		Func_Of_Proc_Os_Ndev_Free(wadptdata);
	}

}

u32 Func_Of_Proc_Start_Drv_Threads(PNIC Nic)
{
	u32 _status = TRUE;

	WL_INFO_L2(("+Func_Of_Proc_Start_Drv_Threads\n"));

	if (is_primary_wadptdata(Nic)) {
		Nic->cmdThread =
			kthread_run(proc_cmd_thread_func, Nic, "WL_CMD_THREAD");
		if (IS_ERR(Nic->cmdThread))
			_status = FALSE;
		else
			Func_Of_Proc_Down_Sema(&Nic->cmdpriv.terminate_cmdthread_sema);
	}

#ifdef CONFIG_EVENT_THREAD_MODE
	Nic->evtThread =
		kthread_run(event_thread, Nic, "WL_EVENT_THREAD");
	if (IS_ERR(Nic->evtThread))
		_status = FALSE;
#endif

	Func_Of_Proc_Chip_Hw_Start_Thread(Nic);
	return _status;

}

void Func_Of_Proc_Stop_Drv_Threads(PNIC Nic)
{
	WL_INFO_L2("+Func_Of_Proc_Stop_Drv_Threads\n");

	if (is_primary_wadptdata(Nic))
		proc_stop_cmd_thread_func(Nic, 1);

#ifdef CONFIG_EVENT_THREAD_MODE
	Func_Of_Proc_Pre_Up_Sema(&Nic->evtpriv.evt_notify);
	if (Nic->evtThread) {
		Func_Of_Proc_Down_Sema(&Nic->evtpriv.terminate_evtthread_sema);
	}
#endif

	Func_Of_Proc_Chip_Hw_Stop_Thread(Nic);
}
#endif
static void Func_Of_Proc_Init_Registrypriv_Dev_Network(PNIC Nic)
{
	u8 *myhwaddr = wadptdata_mac_addr(Nic);
	struct registry_priv *pregistrypriv = &Nic->registrypriv;
	WLAN_BSSID_EX *pdev_network = &pregistrypriv->dev_network;

	_func_enter_;

	Func_Of_Proc_Pre_Memcpy(pdev_network->MacAddress, myhwaddr, ETH_ALEN);

	Func_Of_Proc_Pre_Memcpy(&pdev_network->Ssid, &pregistrypriv->ssid,
				sizeof(NDIS_802_11_SSID));

	pdev_network->Configuration.Length = sizeof(NDIS_802_11_CONFIGURATION);
	pdev_network->Configuration.BeaconPeriod = 100;
	pdev_network->Configuration.FHConfig.Length = 0;
	pdev_network->Configuration.FHConfig.HopPattern = 0;
	pdev_network->Configuration.FHConfig.HopSet = 0;
	pdev_network->Configuration.FHConfig.DwellTime = 0;

	_func_exit_;
}

static u8 Func_Of_Proc_Init_Default_Value(PNIC Nic);
static u8 Func_Of_Proc_Init_Default_Value(PNIC Nic)
{
	u8 ret = TRUE;
	struct registry_priv *pregistrypriv = &Nic->registrypriv;
	struct xmit_priv *pxmitpriv = &Nic->xmitpriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct security_priv *psecuritypriv = &Nic->securitypriv;

	pxmitpriv->vcs_setting = pregistrypriv->vwltcarrier_sense;
	pxmitpriv->vcs = pregistrypriv->vcs_type;
	pxmitpriv->vcs_type = pregistrypriv->vcs_type;
	pxmitpriv->frag_len = pregistrypriv->frag_thresh;

	pmlmepriv->scan_mode = SCAN_ACTIVE;

#ifdef CONFIG_80211N_HT
	pmlmepriv->htpriv.ampdu_enable = FALSE;
#endif

	psecuritypriv->binstallGrpkey = FALSE;
	psecuritypriv->sw_encrypt = pregistrypriv->software_encrypt;
	psecuritypriv->sw_decrypt = pregistrypriv->software_decrypt;

	psecuritypriv->dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
	psecuritypriv->dot11PrivacyAlgrthm = _NO_PRIVACY_;

	psecuritypriv->dot11PrivacyKeyIndex = 0;

	psecuritypriv->dot118021XGrpPrivacy = _NO_PRIVACY_;
	psecuritypriv->dot118021XGrpKeyid = 1;

	psecuritypriv->ndisauthtype = Ndis802_11AuthModeOpen;
	psecuritypriv->ndisencryptstatus = Ndis802_11WEPDisabled;

	Func_Of_Proc_Init_Registrypriv_Dev_Network(Nic);
#if 0
	//in mlme.c
	do_rgp_dev_network_renew(Nic, 1);
#endif
	Func_Of_Proc_Chip_Hw_Def_Value_Init(Nic);

//	WL_ENABLE_FUNC(Nic, DF_RX_BIT);
//	WL_ENABLE_FUNC(Nic, DF_TX_BIT);

//	Nic->bLinkInfoDump = 0;
	Nic->bNotifyChannelChange = FALSE;
#ifdef CONFIG_P2P
	Nic->bShowGetP2PState = 1;
#endif

	Nic->setband = WIFI_FREQUENCY_BAND_AUTO;
	Nic->fix_rate = 0xFF;
	Nic->data_fb = 0;
	Nic->driver_ampdu_spacing = 0xFF;
	Nic->driver_rx_ampdu_factor = 0xFF;
	Nic->driver_rx_ampdu_spacing = 0xFF;
	Nic->fix_rx_ampdu_accept = RX_AMPDU_ACCEPT_INVALID;
	Nic->fix_rx_ampdu_size = RX_AMPDU_SIZE_INVALID;
#ifdef DBG_RX_COUNTER_DUMP
	Nic->dump_rx_cnt_mode = 0;
	Nic->drv_rx_cnt_ok = 0;
	Nic->drv_rx_cnt_crcerror = 0;
	Nic->drv_rx_cnt_drop = 0;
#endif

	return ret;
}

#if 0
struct dvobj_priv *Func_Devobj_Init(void)
{
	struct dvobj_priv *pdvobj = NULL;

	if ((pdvobj = (struct dvobj_priv *)wl_zmalloc(sizeof(*pdvobj))) == NULL) {
		return NULL;
	}

	Func_Of_Proc_Pre_Mutex_Init(&pdvobj->hw_init_mutex);
	Func_Of_Proc_Pre_Mutex_Init(&pdvobj->wmbox1_fwcmd_mutex);
	Func_Of_Proc_Pre_Mutex_Init(&pdvobj->setch_mutex);
	Func_Of_Proc_Pre_Mutex_Init(&pdvobj->setbw_mutex);
	Func_Of_Proc_Pre_Mutex_Init(&pdvobj->rf_read_reg_mutex);
	Func_Of_Proc_Pre_Mutex_Init(&pdvobj->umsg_mutex);
	Func_Of_Proc_Pre_Mutex_Init(&pdvobj->power_mutex);

	pdvobj->processing_dev_remove = FALSE;

	Func_Atomic_Of_Set(&pdvobj->disable_func, 0);

	spin_lock_init(&pdvobj->macid_ctl.lock);
	spin_lock_init(&pdvobj->cam_ctl.lock);
	Func_Of_Proc_Pre_Mutex_Init(&pdvobj->cam_ctl.sec_cam_access_mutex);

	return pdvobj;

}


void Func_Devobj_Deinit(struct dvobj_priv *pdvobj)
{
	if (!pdvobj)
		return;

	Func_Of_Proc_Pre_Mutex_Free(&pdvobj->hw_init_mutex);
	Func_Of_Proc_Pre_Mutex_Free(&pdvobj->wmbox1_fwcmd_mutex);
	Func_Of_Proc_Pre_Mutex_Free(&pdvobj->setch_mutex);
	Func_Of_Proc_Pre_Mutex_Free(&pdvobj->setbw_mutex);
	Func_Of_Proc_Pre_Mutex_Free(&pdvobj->rf_read_reg_mutex);
	Func_Of_Proc_Pre_Mutex_Free(&pdvobj->power_mutex);
	Func_Of_Proc_Pre_Mutex_Free(&pdvobj->umsg_mutex);

	Func_Of_Proc_Pre_Mutex_Free(&pdvobj->cam_ctl.sec_cam_access_mutex);

	wl_mfree((u8 *) pdvobj, sizeof(*pdvobj));
}
#endif

static u8 Func_Of_Proc_Reset_Drv_Sw(PNIC Nic)
{

	u8 ret8 = TRUE;
#if 0
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct pwrctrl_priv *pwrctrlpriv = &Nic->pwrctl_priv;

//	if (is_primary_wadptdata(Nic))
		Func_Of_Proc_Chip_Hw_Def_Value_Init(Nic);

//	WL_ENABLE_FUNC(Nic, DF_RX_BIT);
//	WL_ENABLE_FUNC(Nic, DF_TX_BIT);
#if 0
	Nic->bLinkInfoDump = 0;
#endif

	Nic->xmitpriv.tx_pkts = 0;
	Nic->recvpriv.rx_pkts = 0;

	pmlmepriv->LinkDetectInfo.bBusyTraffic = FALSE;

	pmlmepriv->LinkDetectInfo.TrafficTransitionCount = 0;
	pmlmepriv->LinkDetectInfo.LowPowerTransitionCount = 0;

	_clr_fwstate_(pmlmepriv, _FW_UNDER_SURVEY | _FW_UNDER_LINKING);

	pwrctrlpriv->pwr_state_check_cnts = 0;

	mlmeext_set_scan_state(&Nic->mlmeextpriv, SCAN_DISABLE);

#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
	wl_set_signal_stat_timer(&Nic->recvpriv);
#endif
#endif
	return ret8;
}

static void Func_Set_Odm_Defvalue(PNIC Nic)
{
	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_HAL_MSG_INIT_DEFAULT_VALUE, NULL, NULL, 0, 0)) {
		MpTrace(COMP_RICH_INIT, DBG_NORMAL,
			("==>%s==> fail\n", __func__));
		return;
	}
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,	
		    ("==>%s==> success\n", __func__));
}

static void Func_Of_Proc_Init_Pwrctrl_Priv(PNIC Nic, u8 tag)
{
	int i = 0;
	u8 val8 = 0;
	struct pwrctrl_priv *pwrctrlpriv = &Nic->pwrctl_priv;

#if defined(CONFIG_CONCURRENT_MODE)
	if (Nic->wadptdata_type != PRIMARY_wADPTDATA)
		return;
#endif

	_func_enter_;
	if (tag) {
		_init_pwrlock(&pwrctrlpriv->lock);
		_init_pwrlock(&pwrctrlpriv->check_32k_lock);
		pwrctrlpriv->rf_pwrstate = rf_on;

		pwrctrlpriv->lps_leave_cnts = 0;
		pwrctrlpriv->lps_enter_cnts = 0;
		pwrctrlpriv->bInSuspend = FALSE;
		pwrctrlpriv->bkeepfwalive = FALSE;
		pwrctrlpriv->pwr_state_check_cnts = 0;
		pwrctrlpriv->bInternalAutoSuspend = FALSE;
		pwrctrlpriv->bips_processing = FALSE;
		pwrctrlpriv->pwr_state_check_interval = WL_PWR_STATE_CHK_INTERVAL;

		pwrctrlpriv->LpsIdleCount = 0;
		if (Nic->registrypriv.mp_mode == 1)
			pwrctrlpriv->power_mgnt = PS_MODE_ACTIVE;
		else
			pwrctrlpriv->power_mgnt = Nic->registrypriv.power_mgnt;
		pwrctrlpriv->bLeisurePs =
			(PS_MODE_ACTIVE != pwrctrlpriv->power_mgnt) ? TRUE : FALSE;

		pwrctrlpriv->rpwm = 0;
		pwrctrlpriv->dtim = 0;
		pwrctrlpriv->tog = 0x80;
		pwrctrlpriv->bcn_ant_mode = 0;
		pwrctrlpriv->wfprs = PS_STATE_S4;
		pwrctrlpriv->pwr_mode = PS_MODE_ACTIVE;
		pwrctrlpriv->bFwCurrentInPSMode = FALSE;
		pwrctrlpriv->smart_ps = Nic->registrypriv.smart_ps;

#if 0
		Func_Of_Proc_Init_Timer(&pwrctrlpriv->pwr_state_check_timer, Nic,
					   chk_pwr_stat_handler);
#endif

		pwrctrlpriv->wowlan_mode = FALSE;
		pwrctrlpriv->wowlan_ap_mode = FALSE;
		pwrctrlpriv->wowlan_p2p_mode = FALSE;



#ifdef CONFIG_WOWLAN
		pwrctrlpriv->wowlan_pattern_idx = 0;
		for (i = 0; i < MAX_WKFM_NUM; i++) {
			memset(pwrctrlpriv->patterns[i].content, '\0',
				   sizeof(pwrctrlpriv->patterns[i].content));
			memset(pwrctrlpriv->patterns[i].mask, '\0',
				   sizeof(pwrctrlpriv->patterns[i].mask));
			pwrctrlpriv->patterns[i].len = 0;
		}

#endif
	}
	_func_exit_;

}

static void Func_Of_Proc_Free_Pwrctrl_Priv(PNIC Nic, u8 tag)
{
	struct pwrctrl_priv *pwrctrlpriv = &Nic->pwrctl_priv;

#if defined(CONFIG_CONCURRENT_MODE)
	if (wadptdata->wadptdata_type != PRIMARY_wADPTDATA)
		return;
#endif

	_func_enter_;



	_func_exit_;
}

u8 Func_Of_Proc_Init_Drv_Sw(PNIC Nic)
{

	u8 ret8 = TRUE;

	_func_enter_;

	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("+Func_Of_Proc_Init_Drv_Sw\n"));

    ret8 = Func_Of_Proc_Init_Default_Value(Nic);

#if 0
	if ((cmd_priv_to_init_func(&Nic->cmdpriv)) == FALSE) {
		MpTrace(COMP_RICH_INIT, DBG_NORMAL,("\n Can't init cmd_priv\n"));
		ret8 = FALSE;
		goto exit;
	}

//	Nic->cmdpriv.Nic = Nic;

	if ((evt_priv_to_init_func(&Nic->evtpriv)) == FALSE) {
		MpTrace(COMP_RICH_INIT, DBG_NORMAL,("\n Can't init evt_priv\n"));
		ret8 = FALSE;
		goto exit;
	}

	if (do_admin_priv_init(Nic, 1) == FALSE) {
		MpTrace(COMP_RICH_INIT, DBG_NORMAL,("\n Can't init mlme_priv\n"));
		ret8 = FALSE;
		goto exit;
	}
#ifdef CONFIG_P2P
	wifidirect_timers_to_init_func(Nic, 1);
	wifidirect_info_init_func(Nic, P2P_ROLE_DISABLE, 1);
	global_wifidirect_info_reset_func(Nic, 1);
#endif

	if (mlme_ext_priv_to_init_func(Nic, 1) == FALSE) {
		MpTrace(COMP_RICH_INIT, DBG_NORMAL,("\n Can't init mlme_ext_priv\n"));
		ret8 = FALSE;
		goto exit;
	}


	if (_init_tx_priv(&Nic->xmitpriv, Nic, 1) == FALSE) {
		MpTrace(COMP_RICH_INIT, DBG_NORMAL,("Can't _init_tx_priv\n"));
		ret8 = FALSE;
		goto exit;
	}

	if (_do_init_rx_priv(Nic, &Nic->recvpriv, 1) == FALSE) {
		MpTrace(COMP_RICH_INIT, DBG_NORMAL,("Can't _do_init_rx_priv\n"));
		ret8 = FALSE;
		goto exit;
	}
	Func_Of_Proc_Pre_Spinlock_Init(&Nic->security_key_mutex);

	if (_wl_do_sta_priv_init(&Nic->stapriv, 1) == FALSE) {
		MpTrace(COMP_RICH_INIT, DBG_NORMAL,("Can't _wl_do_sta_priv_init\n"));
		ret8 = FALSE;
		goto exit;
	}
#endif


#if 0
	wl_do_bcmc_stainfo_init(Nic, 1);
#endif
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("1\n"));
	Func_Of_Proc_Init_Pwrctrl_Priv(Nic, 1);
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("2\n"));	
	Func_Set_Odm_Defvalue(Nic);
	Func_Of_Proc_Chip_Hw_Dm_Init(Nic);
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("3\n"));	
	
#if 0	
	Func_Of_Proc_Pre_Spinlock_Init(&Nic->br_ext_lock);
#endif

exit:
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("-Func_Of_Proc_Init_Drv_Sw\n"));
	_func_exit_;

	return ret8;

}

#ifdef CONFIG_WOWLAN
void Func_Of_Proc_Cancel_Dynamic_Chk_Timer(PNIC Nic)
{
	_cancel_timer_ex(&Nic->mlmepriv.dynamic_chk_timer);
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("Func_Of_Proc_Cancel_All_Timer:cancel dynamic_chk_timer! \n"));	
}
#endif

void Func_Of_Proc_Cancel_All_Timer(PNIC Nic)
{
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("+Func_Of_Proc_Cancel_All_Timer\n"));	

	_cancel_timer_ex(&Nic->mlmepriv.assoc_timer);
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("Func_Of_Proc_Cancel_All_Timer:cancel association timer complete!\n"));	

	_cancel_timer_ex(&Nic->mlmepriv.scan_to_timer);
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("Func_Of_Proc_Cancel_All_Timer:cancel scan_to_timer!\n"));	

	_cancel_timer_ex(&Nic->mlmepriv.dynamic_chk_timer);
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("Func_Of_Proc_Cancel_All_Timer:cancel dynamic_chk_timer!\n"));	

	_cancel_timer_ex(&(Nic->pwrctl_priv.pwr_state_check_timer));


#ifdef CONFIG_SET_SCAN_DENY_TIMER
	_cancel_timer_ex(&Nic->mlmepriv.set_scan_deny_timer);
	do_clear_scan_deny(Nic);
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,
		("Func_Of_Proc_Cancel_All_Timer:cancel set_scan_deny_timer! \n"));	

#endif

#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
//	_cancel_timer_ex(&Nic->recvpriv.signal_stat_timer);
#endif
	Func_Of_Proc_Chip_Hw_Dm_Deinit(Nic);

}

u8 Func_Of_Proc_Free_Drv_Sw(PNIC Nic)
{
	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("==>Func_Of_Proc_Free_Drv_Sw"));	

#ifdef CONFIG_P2P
	{
		struct wifidirect_info *pwdinfo = &Nic->wdinfo;
		if (!wl_p2p_chk_state(pwdinfo, P2P_STATE_NONE)) {
			_cancel_timer_ex(&pwdinfo->find_phase_timer);
			_cancel_timer_ex(&pwdinfo->restore_p2p_state_timer);
			_cancel_timer_ex(&pwdinfo->pre_tx_scan_timer);
#ifdef CONFIG_CONCURRENT_MODE
			_cancel_timer_ex(&pwdinfo->ap_p2p_switch_timer);
#endif
			wl_p2p_set_state(pwdinfo, P2P_STATE_NONE);
		}
	}
#endif
//	Func_Of_Proc_Pre_Spinlock_Free(&Nic->security_key_mutex);

//	Func_Of_Proc_Pre_Spinlock_Free(&Nic->br_ext_lock);

//	proc_mlme_ext_deinit_func(&Nic->mlmeextpriv, 1);

//	preproc_free_cmd_priv_func(&Nic->cmdpriv);

//	preproc_free_evt_priv_func(&Nic->evtpriv);

//	do_admin_priv_unnew(&Nic->mlmepriv, 1);

#if 0
	_unnew_tx_priv_func(&Nic->xmitpriv, 1);

	_wl_do_unnew_sta_priv(&Nic->stapriv, 1);

	_wl_unnew_rx_priv(&Nic->recvpriv, 1);
#endif

	Func_Of_Proc_Free_Pwrctrl_Priv(Nic, 1);

	Func_Of_Proc_Chip_Hw_Free_Data(Nic);

	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("<==Func_Of_Proc_Free_Drv_Sw\n"));	

#if 0
	if (Nic->rereg_nd_name_priv.old_pnetdev) {
		free_netdev(Nic->rereg_nd_name_priv.old_pnetdev);
		Nic->rereg_nd_name_priv.old_pnetdev = NULL;
	}

//	if (Nic->pbuddy_wadptdata != NULL) {
//		Nic->pbuddy_wadptdata->pbuddy_wadptdata = NULL;
//	}

	MpTrace(COMP_RICH_INIT, DBG_NORMAL,("-Func_Of_Proc_Free_Drv_Sw\n"));	
#endif
	return TRUE;

}


#if 0
#ifdef CONFIG_CONCURRENT_MODE
static int Func__Netdev_If2_Open(struct net_device *pnetdev)
{
	_wadptdata *Nic = (_wadptdata *) wl_netdev_priv(pnetdev);
	_wadptdata *primary_pwadptdata = Nic->pbuddy_wadptdata;

	WL_INFO("+9083hu_drv - if2_open, bup=%d\n", Nic->bup);

	if (primary_pwadptdata->bup == FALSE
		|| !wl_is_hw_init_completed(primary_pwadptdata))
		Func_Netdev_Open(primary_pwadptdata->pnetdev);

	if (Nic->bup == FALSE && primary_pwadptdata->bup == TRUE &&
		wl_is_hw_init_completed(primary_pwadptdata)) {
		Nic->bFWReady = primary_pwadptdata->bFWReady;

		if (Func_Of_Proc_Start_Drv_Threads(Nic) == FALSE) {
			goto netdev_if2_open_error;
		}

		if (Nic->intf_start) {
			Nic->intf_start(Nic);
		}

		Nic->bup = TRUE;

	}

	Nic->net_closed = FALSE;

	wl_netif_wake_queue(pnetdev);

	WL_INFO("-9083hu_drv - if2_open, bup=%d\n", Nic->bup);
	return 0;

netdev_if2_open_error:

	Nic->bup = FALSE;

	netif_carrier_off(pnetdev);
	wl_netif_stop_queue(pnetdev);

	return (-1);

}

int Func_Netdev_If2_Open(struct net_device *pnetdev)
{
	int ret;
	_wadptdata *Nic = (_wadptdata *) wl_netdev_priv(pnetdev);
	struct pwrctrl_priv *pwrctrlpriv = wadptdata_to_pwrctl(Nic);

	if (pwrctrlpriv->bInSuspend == TRUE) {
		WL_INFO("+9083hu_drv - Func_Netdev_If2_Open, bInSuspend=%d\n",
				pwrctrlpriv->bInSuspend);
		return 0;
	}

	_enter_critical_mutex(&(wadptdata_to_dvobj(Nic)->hw_init_mutex), NULL);
	ret = Func__Netdev_If2_Open(pnetdev);
	_exit_critical_mutex(&(wadptdata_to_dvobj(Nic)->hw_init_mutex), NULL);

	return ret;
}

static int Func_Netdev_If2_Close(struct net_device *pnetdev)
{
	_wadptdata *Nic = (_wadptdata *) wl_netdev_priv(pnetdev);
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;

	Nic->net_closed = TRUE;
	pmlmepriv->LinkDetectInfo.bBusyTraffic = FALSE;

	if (pnetdev) {
		wl_netif_stop_queue(pnetdev);
	}
#ifdef CONFIG_P2P
	if (!wl_p2p_chk_role(&Nic->wdinfo, P2P_ROLE_DISABLE))
		enable_of_p2p_func(Nic, P2P_ROLE_DISABLE, 1);
#endif


	return 0;
}

#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,29))
static const struct net_device_ops wl_netdev_if2_ops = {
	.ndo_init = Func_Of_Proc_Ndev_Init,
	.ndo_uninit = Func_Of_Proc_Ndev_Uninit,
	.ndo_open = Func_Netdev_If2_Open,
	.ndo_stop = Func_Netdev_If2_Close,
	.ndo_start_xmit = wl_do_tx_entry,
	.ndo_set_mac_address = Func_Of_Proc_Net_Set_Mac_Address,
	.ndo_get_stats = Func_Of_Proc_Net_Get_Stats,
	.ndo_do_ioctl = Func_Of_Proc_Ioctl,
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,35))
	.ndo_select_queue = Func_Of_Proc_Select_Queue,
#endif
};
#endif

static void Func_Of_Proc_Hook_If2_Ops(struct net_device *ndev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	ndev->netdev_ops = &wl_netdev_if2_ops;
#else
	ndev->init = Func_Of_Proc_Ndev_Init;
	ndev->uninit = Func_Of_Proc_Ndev_Uninit;
	ndev->open = Func_Netdev_If2_Open;
	ndev->stop = Func_Netdev_If2_Close;
	ndev->set_mac_address = Func_Of_Proc_Net_Set_Mac_Address;
#endif
}

_wadptdata *Func_Of_Proc_Drv_If2_Init(_wadptdata * primary_pwadptdata)
{
	int res = FALSE;
	_wadptdata *Nic = NULL;
	struct dvobj_priv *pdvobjpriv;
	u8 mac[ETH_ALEN];

	Nic = (_wadptdata *) wl_zvmalloc(sizeof(*Nic));
	if (Nic == NULL)
		goto exit;

	if (Func_Loadparam(Nic) != TRUE)
		goto free_wadptdata;

	Func_Of_Proc_Pre_Memcpy(Nic, primary_pwadptdata, sizeof(*Nic));

	Nic->bup = FALSE;
	Nic->net_closed = TRUE;
	Nic->dir_dev = NULL;
	Nic->dir_msg = NULL;

	Nic->isprimary = FALSE;
	Nic->wadptdata_type = SECONDARY_wADPTDATA;
	Nic->pbuddy_wadptdata = primary_pwadptdata;
	Nic->iface_id = IFACE_ID1;
#ifndef CONFIG_HWPORT_SWAP
	Nic->iface_type = IFACE_PORT1;
#else
	Nic->iface_type = IFACE_PORT0;
#endif

	pdvobjpriv = wadptdata_to_dvobj(Nic);
	pdvobjpriv->pwadptdatas[pdvobjpriv->iface_nums++] = Nic;

	Nic->intf_start = primary_pwadptdata->intf_start;
	Nic->intf_stop = primary_pwadptdata->intf_stop;

	if (Func_Of_Proc_Init_Drv_Sw(Nic) != TRUE)
		goto free_drv_sw;

	Func_Of_Proc_Pre_Memcpy(mac, wadptdata_mac_addr(primary_pwadptdata), ETH_ALEN);

	mac[0] |= BIT(1);

	Func_Of_Proc_Pre_Memcpy(wadptdata_mac_addr(Nic), mac, ETH_ALEN);
	wifidirect_addrs_to_init_func(Nic, wadptdata_mac_addr(Nic),
							  wadptdata_mac_addr(Nic), 1);

	primary_pwadptdata->pbuddy_wadptdata = Nic;

	res = TRUE;

free_drv_sw:
	if (res != TRUE && Nic)
		Func_Of_Proc_Free_Drv_Sw(Nic);
free_wadptdata:
	if (res != TRUE && Nic) {
		wl_vmfree((u8 *) Nic, sizeof(*Nic));
		Nic = NULL;
	}
exit:
	return Nic;
}

void Func_Of_Proc_Drv_If2_Free(_wadptdata * if2)
{
	_wadptdata *Nic = if2;

	if (Nic == NULL)
		return;

	Func_Of_Proc_Free_Drv_Sw(Nic);

	Func_Of_Proc_Os_Ndev_Free(Nic);

	wl_vmfree((u8 *) Nic, sizeof(_wadptdata));
}

void Func_Of_Proc_Drv_If2_Stop(_wadptdata * if2)
{
	_wadptdata *Nic = if2;
	struct net_device *pnetdev = NULL;

	if (Nic == NULL)
		return;

	if (Nic->bup == TRUE) {
#ifdef CONFIG_XMIT_ACK
		if (Nic->xmitpriv.ack_tx)
			wl_finish_ack_tx(WL_SCTX_DONE_DRV_STOP, &Nic->xmitpriv, 1);
#endif

		if (Nic->intf_stop) {
			Nic->intf_stop(Nic);
		}

		Func_Of_Proc_Stop_Drv_Threads(Nic);

		Nic->bup = FALSE;
	}

	Func_Of_Proc_Cancel_All_Timer(Nic);
}
#endif

int Func_Of_Proc_Os_Ndevs_Register(struct dvobj_priv *dvobj)
{
	int i, status = TRUE;
	struct registry_priv *regsty = dvobj_to_regsty(dvobj);
	_wadptdata *wadptdata;


	for (i = 0; i < dvobj->iface_nums; i++) {

		if (i >= IFACE_ID_MAX) {
			WL_INFO_L1("%s %d >= IFACE_ID_MAX\n", __func__, i);
			wl_warn_on(1);
			continue;
		}

		wadptdata = dvobj->pwadptdatas[i];
		if (wadptdata) {
			char *name;

			if (wadptdata->iface_id == IFACE_ID0)
				name = regsty->ifname;
			else if (wadptdata->iface_id == IFACE_ID1)
				name = regsty->if2name;
			else
				name = "wlan%d";

#ifdef CONFIG_CONCURRENT_MODE
			switch (wadptdata->wadptdata_type) {
			case SECONDARY_wADPTDATA:
				Func_Of_Proc_Hook_If2_Ops(wadptdata->pnetdev);
				break;
			}
#endif

			status = Func_Of_Proc_Os_Ndev_Register(wadptdata, name);

			if (status != TRUE) {
				wl_warn_on(1);
				break;
			}
		}
	}

	if (status != TRUE) {
		for (; i >= 0; i--) {
			wadptdata = dvobj->pwadptdatas[i];
			if (wadptdata)
				Func_Of_Proc_Os_Ndev_Unregister(wadptdata);
		}
	}
exit:
	return status;
}

void Func_Of_Proc_Os_Ndevs_Unregister(struct dvobj_priv *dvobj)
{
	int i;
	_wadptdata *wadptdata = NULL;

	for (i = 0; i < dvobj->iface_nums; i++) {
		wadptdata = dvobj->pwadptdatas[i];

		if (wadptdata == NULL)
			continue;

		Func_Of_Proc_Os_Ndev_Unregister(wadptdata);
	}

}

int Func_Of_Proc_Os_Ndevs_Init(struct dvobj_priv *dvobj)
{
	int ret = FALSE;

	if (Func_Of_Proc_Os_Ndevs_Alloc(dvobj) != TRUE)
		goto exit;

	if (Func_Of_Proc_Os_Ndevs_Register(dvobj) != TRUE)
		goto os_ndevs_free;

	ret = TRUE;

os_ndevs_free:
	if (ret != TRUE)
		Func_Of_Proc_Os_Ndevs_Free(dvobj);
exit:
	return ret;
}

void Func_Of_Proc_Os_Ndevs_Deinit(struct dvobj_priv *dvobj)
{
	Func_Of_Proc_Os_Ndevs_Unregister(dvobj);
	Func_Of_Proc_Os_Ndevs_Free(dvobj);
}

void Func_Netdev_Br_Init(struct net_device *netdev)
{
	_wadptdata *wadptdata = (_wadptdata *) wl_netdev_priv(netdev);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35))
	rcu_read_lock();
#endif

	{
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 35))
		if (netdev->br_port)
#else
		if (rcu_dereference(wadptdata->pnetdev->rx_handler_data))
#endif
		{
			struct net_device *br_netdev;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24))
			br_netdev = dev_get_by_name(CONFIG_BR_EXT_BRNAME);
#else
			struct net *devnet = NULL;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
			devnet = netdev->nd_net;
#else
			devnet = dev_net(netdev);
#endif

			br_netdev = dev_get_by_name(devnet, CONFIG_BR_EXT_BRNAME);
#endif

			if (br_netdev) {
				memcpy(wadptdata->br_mac, br_netdev->dev_addr, ETH_ALEN);
				dev_put(br_netdev);
			} else
				WL_INFO_L1("%s()-%d: dev_get_by_name(%s) failed!", __FUNCTION__,
					   __LINE__, CONFIG_BR_EXT_BRNAME);
		}

		wadptdata->ethBrExtInfo.addPPPoETag = 1;
	}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35))
	rcu_read_unlock();
#endif
}

int Func_Netdev_Open(struct net_device *pnetdev)
{
	uint status;
	_wadptdata *Nic = (_wadptdata *) wl_netdev_priv(pnetdev);
	struct pwrctrl_priv *pwrctrlpriv = wadptdata_to_pwrctl(Nic);

	WL_INFO_L2(("+9083hu_drv - dev_open\n"));
	WL_INFO("+9083hu_drv - drv_open, bup=%d\n", Nic->bup);

	Nic->netif_up = TRUE;

	if (pwrctrlpriv->ps_flag == TRUE) {
		Nic->net_closed = FALSE;
		goto netdev_open_normal_process;
	}

	if (Nic->bup == FALSE) {
		wl_clr_surprise_removed(Nic);
		wl_clr_drv_stopped(Nic);

		status = Func_Of_Proc_Chip_Hw_Init(Nic);
		if (status == FALSE) {
			WL_INFO_L2(("wll908621x_hal_init(): Can't init h/w!\n"));
			goto netdev_open_error;
		}

		WL_INFO("MAC Address = " MAC_FMT "\n", MAC_ARG(pnetdev->dev_addr));

		status = Func_Of_Proc_Start_Drv_Threads(Nic);
		if (status == FALSE) {
			WL_INFO("Initialize driver software resource Failed!\n");
			goto netdev_open_error;
		}

		if (Nic->intf_start) {
			Nic->intf_start(Nic);
		}

		Nic->bup = TRUE;
		pwrctrlpriv->bips_processing = FALSE;

	}
	Nic->net_closed = FALSE;

	_set_timer(&Nic->mlmepriv.dynamic_chk_timer, 2000);

	wl_set_pwr_state_check_timer(pwrctrlpriv);

	wl_netif_wake_queue(pnetdev);

	Func_Netdev_Br_Init(pnetdev);

netdev_open_normal_process:

#ifdef CONFIG_CONCURRENT_MODE
	{
		_wadptdata *sec_wadptdata = Nic->pbuddy_wadptdata;
		if (sec_wadptdata && (sec_wadptdata->bup == FALSE))
			Func__Netdev_If2_Open(sec_wadptdata->pnetdev);
	}
#endif

	WL_INFO_L2(("-9083hu_drv - dev_open\n"));
	WL_INFO("-9083hu_drv - drv_open, bup=%d\n", Nic->bup);

	return 0;

netdev_open_error:

	Nic->bup = FALSE;

	netif_carrier_off(pnetdev);
	wl_netif_stop_queue(pnetdev);

	WL_INFO_L2("-9083hu_drv - dev_open, fail!\n");
	WL_INFO("-9083hu_drv - drv_open fail, bup=%d\n", Nic->bup);

	return (-1);

}

int Func_Pre_Netdev_Open(struct net_device *pnetdev)
{
	int ret;
	_wadptdata *Nic = (_wadptdata *) wl_netdev_priv(pnetdev);
	struct pwrctrl_priv *pwrctrlpriv = wadptdata_to_pwrctl(Nic);

	if (pwrctrlpriv->bInSuspend == TRUE) {
		WL_INFO("+9083hu_drv - drv_open, bInSuspend=%d\n",
				pwrctrlpriv->bInSuspend);
		return 0;
	}

	_enter_critical_mutex(&(wadptdata_to_dvobj(Nic)->hw_init_mutex), NULL);
	ret = Func_Netdev_Open(pnetdev);
	_exit_critical_mutex(&(wadptdata_to_dvobj(Nic)->hw_init_mutex), NULL);

	return ret;
}

void Func_Of_Proc_Ips_Dev_Unload(PNIC Nic)
{
	struct net_device *pnetdev = (struct net_device *)Nic->pnetdev;
	struct xmit_priv *pxmitpriv = &(Nic->xmitpriv);
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	WL_INFO("====> %s...\n", __FUNCTION__);
	{
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_FIFO_CLEARN_UP, 0);

		if (Nic->intf_stop) {
			Nic->intf_stop(Nic);
		}
	}

	if (!wl_is_surprise_removed(Nic))
		Func_Of_Proc_Chip_Hw_Deinit(Nic);

}

int Func_Pm_Netdev_Open(struct net_device *pnetdev, u8 bnormal)
{
	int status = 0;

	_wadptdata *Nic = (_wadptdata *) wl_netdev_priv(pnetdev);

	if (TRUE == bnormal) {
		_enter_critical_mutex(&(wadptdata_to_dvobj(Nic)->hw_init_mutex),
							  NULL);
		status = Func_Netdev_Open(pnetdev);
		_exit_critical_mutex(&(wadptdata_to_dvobj(Nic)->hw_init_mutex),
							 NULL);
	}

	return status;
}

static int Func_Netdev_Close(struct net_device *pnetdev)
{
	_wadptdata *Nic = (_wadptdata *) wl_netdev_priv(pnetdev);
	struct pwrctrl_priv *pwrctl = wadptdata_to_pwrctl(Nic);
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	WL_INFO_L2(("+9083hu_drv - drv_close\n"));

	if (pwrctl->bInternalAutoSuspend == TRUE) {
		if (pwrctl->rf_pwrstate == rf_off)
			pwrctl->ps_flag = TRUE;
	}
	Nic->net_closed = TRUE;
	Nic->netif_up = FALSE;
	pmlmepriv->LinkDetectInfo.bBusyTraffic = FALSE;

	if (pwrctl->rf_pwrstate == rf_on) {
		WL_INFO("(2)9083hu_drv - drv_close, bup=%d, hw_init_completed=%s\n",
				Nic->bup,
				wl_is_hw_init_completed(Nic) ? "TRUE" : "FALSE");

		if (pnetdev) {
			wl_netif_stop_queue(pnetdev);
		}
#ifndef CONFIG_ANDROID
		left_aps(Nic, 1);
		proc_disassoc_cmd_func(Nic, 500, FALSE);
		wl_disconnect_flag(Nic, 0, FALSE, 1);
		do_assoc_resources_unnew(Nic, 1, 1);
		do_network_queue_unnew(Nic, TRUE, 1);
#endif
	}

	{
		db_cleanup_of_nat25_func(Nic, 1);
	}

#ifdef CONFIG_P2P
	if (!wl_p2p_chk_role(&Nic->wdinfo, P2P_ROLE_DISABLE))
		enable_of_p2p_func(Nic, P2P_ROLE_DISABLE, 1);
#endif

	WL_INFO_L2("-9083hu_drv - drv_close\n");
	WL_INFO("-9083hu_drv - drv_close, bup=%d\n", Nic->bup);

	return 0;

}

int Func_Pm_Netdev_Close(struct net_device *pnetdev, u8 bnormal)
{
	int status = 0;

	status = Func_Netdev_Close(pnetdev);

	return status;
}

void Func_Of_Proc_Ndev_Destructor(struct net_device *ndev)
{
	WL_INFO(FUNC_NDEV_FMT "\n", FUNC_NDEV_ARG(ndev));

	free_netdev(ndev);
}

#ifdef CONFIG_ARP_KEEP_ALIVE
struct route_info {
	struct in_addr dst_addr;
	struct in_addr src_addr;
	struct in_addr gateway;
	unsigned int dev_index;
};

static void Func_Parse_Routes(struct nlmsghdr *nl_hdr, struct route_info *rt_info)
{
	struct rtmsg *rt_msg;
	struct rtattr *rt_attr;
	int rt_len;

	rt_msg = (struct rtmsg *)NLMSG_DATA(nl_hdr);
	if ((rt_msg->rtm_family != AF_INET) || (rt_msg->rtm_table != WP_TABLE_MAIN))
		return;

	rt_attr = (struct rtattr *)RTM_RTA(rt_msg);
	rt_len = RTM_PAYLOAD(nl_hdr);

	for (; RTA_OK(rt_attr, rt_len); rt_attr = RTA_NEXT(rt_attr, rt_len)) {
		switch (rt_attr->rta_type) {
		case RTA_OIF:
			rt_info->dev_index = *(int *)RTA_DATA(rt_attr);
			break;
		case RTA_GATEWAY:
			rt_info->gateway.s_addr = *(u_int *) RTA_DATA(rt_attr);
			break;
		case RTA_PREFSRC:
			rt_info->src_addr.s_addr = *(u_int *) RTA_DATA(rt_attr);
			break;
		case RTA_DST:
			rt_info->dst_addr.s_addr = *(u_int *) RTA_DATA(rt_attr);
			break;
		}
	}
}

static int Func_Route_Dump(u32 * gw_addr, int *gw_index)
{
	int err = 0;
	struct socket *sock;
	struct {
		struct nlmsghdr nlh;
		struct rtgenmsg g;
	} req;
	struct msghdr msg;
	struct iovec iov;
	struct sockaddr_nl nladdr;
	mm_segment_t oldfs;
	char *pg;
	int size = 0;

	err = sock_create(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE, &sock);
	if (err) {
		WL_INFO_L1(": Could not create a datagram socket, error = %d\n", -ENXIO);
		return err;
	}

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;

	req.nlh.nlmsg_len = sizeof(req);
	req.nlh.nlmsg_type = RTM_GETROUTE;
	req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
	req.nlh.nlmsg_pid = 0;
	req.g.rtgen_family = AF_INET;

	iov.iov_base = &req;
	iov.iov_len = sizeof(req);

	msg.msg_name = &nladdr;
	msg.msg_namelen = sizeof(nladdr);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
	iov_iter_init(&msg.msg_iter, WRITE, &iov, 1, sizeof(req));
#else
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
#endif
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = MSG_DONTWAIT;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
	err = sock_sendmsg(sock, &msg);
#else
	err = sock_sendmsg(sock, &msg, sizeof(req));
#endif
	set_fs(oldfs);

	if (err < 0)
		goto out_sock;

	pg = (char *)__get_free_page(GFP_KERNEL);
	if (pg == NULL) {
		err = -ENOMEM;
		goto out_sock;
	}
#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
restart:
#endif

	for (;;) {
		struct nlmsghdr *h;

		iov.iov_base = pg;
		iov.iov_len = PAGE_SIZE;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
		iov_iter_init(&msg.msg_iter, READ, &iov, 1, PAGE_SIZE);
#endif

		oldfs = get_fs();
		set_fs(KERNEL_DS);
		err = sock_recvmsg(sock, &msg, PAGE_SIZE, MSG_DONTWAIT);
		set_fs(oldfs);

		if (err < 0)
			goto out_sock_pg;

		if (msg.msg_flags & MSG_TRUNC) {
			err = -ENOBUFS;
			goto out_sock_pg;
		}

		h = (struct nlmsghdr *)pg;

		while (NLMSG_OK(h, err)) {
			struct route_info rt_info;
			if (h->nlmsg_type == NLMSG_DONE) {
				err = 0;
				goto done;
			}

			if (h->nlmsg_type == NLMSG_ERROR) {
				struct nlmsgerr *errm = (struct nlmsgerr *)NLMSG_DATA(h);
				err = errm->error;
				WL_ERR("NLMSG error: %d\n", errm->error);
				goto done;
			}

			if (h->nlmsg_type == RTM_GETROUTE) {
				WL_INFO_L1("RTM_GETROUTE: NLMSG: %d\n", h->nlmsg_type);
			}
			if (h->nlmsg_type != RTM_NEWROUTE) {
				WL_INFO_L1("NLMSG: %d\n", h->nlmsg_type);
				err = -EINVAL;
				goto done;
			}

			memset(&rt_info, 0, sizeof(struct route_info));
			Func_Parse_Routes(h, &rt_info);
			if (!rt_info.dst_addr.s_addr && rt_info.gateway.s_addr
				&& rt_info.dev_index) {
				*gw_addr = rt_info.gateway.s_addr;
				*gw_index = rt_info.dev_index;

			}
			h = NLMSG_NEXT(h, err);
		}

		if (err) {
			WL_INFO_L1("!!!Remnant of size %d %d %d\n", err, h->nlmsg_len,
				   h->nlmsg_type);
			err = -EINVAL;
			break;
		}
	}

done:
#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
	if (!err && req.g.rtgen_family == AF_INET) {
		req.g.rtgen_family = AF_INET6;

		iov.iov_base = &req;
		iov.iov_len = sizeof(req);

		msg.msg_name = &nladdr;
		msg.msg_namelen = sizeof(nladdr);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
		iov_iter_init(&msg.msg_iter, WRITE, &iov, 1, sizeof(req));
#else
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
#endif
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		msg.msg_flags = MSG_DONTWAIT;

		oldfs = get_fs();
		set_fs(KERNEL_DS);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
		err = sock_sendmsg(sock, &msg);
#else
		err = sock_sendmsg(sock, &msg, sizeof(req));
#endif
		set_fs(oldfs);

		if (err > 0)
			goto restart;
	}
#endif

out_sock_pg:
	free_page((unsigned long)pg);

out_sock:
	sock_release(sock);
	return err;
}

static int Func_Arp_Query(unsigned char *haddr, u32 paddr, struct net_device *dev)
{
	struct neighbour *neighbor_entry;
	int ret = 0;

	neighbor_entry = neigh_lookup(&arp_tbl, &paddr, dev);

	if (neighbor_entry != NULL) {
		neighbor_entry->used = jiffies;
		if (neighbor_entry->nud_state & NUD_VALID) {
			Func_Of_Proc_Pre_Memcpy(haddr, neighbor_entry->ha, dev->addr_len);
			ret = 1;
		}
		neigh_release(neighbor_entry);
	}
	return ret;
}

static int Func_To_Get_Defaultgw(u32 * ip_addr, char mac[])
{
	int gw_index = 0;
	struct net_device *gw_dev = NULL;

	Func_Route_Dump(ip_addr, &gw_index);

	if (!(*ip_addr) || !gw_index) {
		WL_INFO("No default GW \n");
		return -1;
	}

	gw_dev = dev_get_by_index(&init_net, gw_index);

	if (gw_dev == NULL) {
		WL_INFO("get Oif Device Fail \n");
		return -1;
	}

	if (!Func_Arp_Query(mac, *ip_addr, gw_dev)) {
		WL_INFO("arp query failed\n");
		dev_put(gw_dev);
		return -1;

	}
	dev_put(gw_dev);

	return 0;
}

int Func_Of_Proc_Gw_Addr_Query(PNIC Nic)
{
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct pwrctrl_priv *pwrctl = wadptdata_to_pwrctl(Nic);
	u32 gw_addr = 0;
	unsigned char gw_mac[32] = { 0 };
	int i;
	int res;

	res = Func_To_Get_Defaultgw(&gw_addr, gw_mac);
	if (!res) {
		pmlmepriv->gw_ip[0] = gw_addr & 0xff;
		pmlmepriv->gw_ip[1] = (gw_addr & 0xff00) >> 8;
		pmlmepriv->gw_ip[2] = (gw_addr & 0xff0000) >> 16;
		pmlmepriv->gw_ip[3] = (gw_addr & 0xff000000) >> 24;
		Func_Of_Proc_Pre_Memcpy(pmlmepriv->gw_mac_addr, gw_mac, 6);
		WL_INFO("%s Gateway Mac:\t" MAC_FMT "\n", __FUNCTION__,
				MAC_ARG(pmlmepriv->gw_mac_addr));
		WL_INFO("%s Gateway IP:\t" IP_FMT "\n", __FUNCTION__,
				IP_ARG(pmlmepriv->gw_ip));
	} else {
		WL_INFO("Get Gateway IP/MAC fail!\n");
	}

	return res;
}
#endif

void Func_Of_Proc_Dev_Unload(PNIC Nic)
{
	struct net_device *pnetdev = (struct net_device *)Nic->pnetdev;
	struct pwrctrl_priv *pwrctl = wadptdata_to_pwrctl(Nic);
	struct dvobj_priv *pobjpriv = Nic->dvobj;
	struct debug_priv *pdbgpriv = &pobjpriv->drv_dbg;
	struct cmd_priv *pcmdpriv = &Nic->cmdpriv;
	u8 cnt = 0;

	WL_INFO_L2(("+%s\n", __FUNCTION__));

	if (Nic->bup == TRUE) {
		WL_INFO("===> %s\n", __FUNCTION__);

		wl_set_drv_stopped(Nic);
#ifdef CONFIG_XMIT_ACK
		if (Nic->xmitpriv.ack_tx)
			wl_finish_ack_tx(WL_SCTX_DONE_DRV_STOP, &Nic->xmitpriv, 1);
#endif

		if (Nic->intf_stop)
			Nic->intf_stop(Nic);

		WL_INFO_L2("@ Func_Of_Proc_Dev_Unload: stop intf complete!\n");

		if (!pwrctl->bInternalAutoSuspend)
			Func_Of_Proc_Stop_Drv_Threads(Nic);

		while (Func_Atomic_Of_Read(&(pcmdpriv->cmdthd_running)) == TRUE) {
			if (cnt > 5) {
				WL_INFO("stop cmdthd timeout\n");
				break;
			} else {
				cnt++;
				WL_INFO("cmdthd is running(%d)\n", cnt);
				Func_Of_Proc_Msleep_Os(10);
			}
		}

		WL_INFO_L2("@ %s: stop thread complete!\n", __FUNCTION__);

		WL_INFO("[MC] %s %d\n", __FUNCTION__, __LINE__);

		if (!wl_is_surprise_removed(Nic)) {
#ifdef CONFIG_WOWLAN
			if (pwrctl->bSupportRemoteWakeup == TRUE &&
				pwrctl->wowlan_mode == TRUE) {
				WL_INFO_L1
					("%s bSupportRemoteWakeup==TRUE  do not run Func_Of_Proc_Chip_Hw_Deinit()\n",
					 __FUNCTION__);
			} else
#endif
			{
				WL_INFO("[MC] %s %d\n", __FUNCTION__, __LINE__);
				Func_Of_Proc_Chip_Hw_Deinit(Nic);
			}
			wl_set_surprise_removed(Nic);
		}
		WL_INFO_L2("@ %s: deinit hal complelt!\n", __FUNCTION__);

		Nic->bup = FALSE;

		WL_INFO("<=== %s\n", __FUNCTION__);
	} else {
		WL_INFO_L2("%s: bup==FALSE\n", __FUNCTION__);
		WL_INFO("%s: bup==FALSE\n", __FUNCTION__);
	}

	Func_Of_Proc_Cancel_All_Timer(Nic);
	WL_INFO_L2("-%s\n", __FUNCTION__);
}

static int Func_Of_Proc_Suspend_Free_Assoc_Resource(PNIC Nic)
{
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct net_device *pnetdev = Nic->pnetdev;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &Nic->wdinfo;
#endif

	WL_INFO("==> " FUNC_ADPT_FMT " entry....\n", FUNC_ADPT_ARG(Nic));

	if (wl_chk_roam_flags(Nic, WL_ROAM_ON_RESUME)) {
		if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)
			&& check_fwstate(pmlmepriv, _FW_LINKED)
#ifdef CONFIG_P2P
			&& wl_p2p_chk_state(pwdinfo, P2P_STATE_NONE)
#endif
			) {
			WL_INFO("%s %s(" MAC_FMT "), length:%d assoc_ssid.length:%d\n",
					__FUNCTION__, pmlmepriv->cur_network.network.Ssid.Ssid,
					MAC_ARG(pmlmepriv->cur_network.network.MacAddress),
					pmlmepriv->cur_network.network.Ssid.SsidLength,
					pmlmepriv->assoc_ssid.SsidLength);
			do_set_to_roam(Nic, 1);
		}
	}

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)
		&& check_fwstate(pmlmepriv, _FW_LINKED)) {
		proc_disassoc_cmd_func(Nic, 0, FALSE);
		wl_disconnect_flag(Nic, 0, FALSE, 1);
	}
#ifdef CONFIG_AP_MODE
	else if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		flush_sta_func(Nic, TRUE, 1);
	}
#endif

	do_assoc_resources_unnew(Nic, 1, 1);

#ifdef CONFIG_AUTOSUSPEND
	if (is_primary_wadptdata(Nic)
		&& (!wadptdata_to_pwrctl(Nic)->bInternalAutoSuspend))
#endif
		do_network_queue_unnew(Nic, TRUE, 1);

	if (check_fwstate(pmlmepriv, _FW_UNDER_SURVEY)) {
		WL_INFO_L1("%s: fw_under_survey\n", __func__);
		wl_os_scan_finish_mark(Nic, 1);
		clr_fwstate(pmlmepriv, _FW_UNDER_SURVEY);
	}

	if (check_fwstate(pmlmepriv, _FW_UNDER_LINKING) == TRUE) {
		WL_INFO_L1("%s: fw_under_linking\n", __FUNCTION__);
		wl_disconnect_flag(Nic, 0, FALSE, 1);
	}

	WL_INFO("<== " FUNC_ADPT_FMT " exit....\n", FUNC_ADPT_ARG(Nic));
	return TRUE;
}

#ifdef CONFIG_WOWLAN
int Func_Of_Proc_Suspend_Wow(PNIC Nic)
{
	u8 ch, bw, offset;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct net_device *pnetdev = Nic->pnetdev;
#ifdef CONFIG_CONCURRENT_MODE
	struct net_device *pbuddy_netdev = Nic->pbuddy_wadptdata->pnetdev;
#endif
	struct dvobj_priv *psdpriv = Nic->dvobj;
	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;
	struct pwrctrl_priv *pwrpriv = wadptdata_to_pwrctl(Nic);
	struct wowlan_ioctl_param poidparam;
	u8 ps_mode;
	int ret = TRUE;

	WL_INFO("==> " FUNC_ADPT_FMT " entry....\n", FUNC_ADPT_ARG(Nic));

	WL_INFO("wowlan_mode: %d\n", pwrpriv->wowlan_mode);
	WL_INFO("wowlan_pno_enable: %d\n", pwrpriv->wowlan_pno_enable);
#ifdef CONFIG_P2P_WOWLAN
	WL_INFO("wowlan_p2p_enable: %d\n", pwrpriv->wowlan_p2p_enable);
#endif

	if (pwrpriv->wowlan_mode == TRUE) {

		if (pnetdev)
			wl_netif_stop_queue(pnetdev);
#ifdef CONFIG_CONCURRENT_MODE
		if (pbuddy_netdev) {
			netif_carrier_off(pbuddy_netdev);
			wl_netif_stop_queue(pbuddy_netdev);
		}
#endif
		wl_set_drv_stopped(Nic);
		Func_Of_Proc_Stop_Drv_Threads(Nic);
#ifdef CONFIG_CONCURRENT_MODE
		if (wl_partner_wadptdata_up(Nic))
			Func_Of_Proc_Stop_Drv_Threads(Nic->pbuddy_wadptdata);
#endif
		wl_clr_drv_stopped(Nic);

		if (Nic->intf_free_irq)
			Nic->intf_free_irq(wadptdata_to_dvobj(Nic));

#ifdef CONFIG_RUNTIME_PORT_SWITCH
		if (check_port_switch_func(Nic, 1)) {
			WL_INFO(" ### PORT SWITCH ### \n");
			Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_PORT_SWITCH, NULL);
		}
#endif

		poidparam.subcode = WOWLAN_ENABLE;
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_WOWLAN, (u8 *) & poidparam);
		if (wl_chk_roam_flags(Nic, WL_ROAM_ON_RESUME)) {
			if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)
				&& check_fwstate(pmlmepriv, _FW_LINKED)) {
				WL_INFO("%s %s(" MAC_FMT "), length:%d assoc_ssid.length:%d\n",
						__FUNCTION__, pmlmepriv->cur_network.network.Ssid.Ssid,
						MAC_ARG(pmlmepriv->cur_network.network.MacAddress),
						pmlmepriv->cur_network.network.Ssid.SsidLength,
						pmlmepriv->assoc_ssid.SsidLength);

				do_set_to_roam(Nic, 0);
			}
		}

		WL_INFO_L1("%s: wowmode suspending\n", __func__);

		if (check_fwstate(pmlmepriv, _FW_UNDER_SURVEY) == TRUE) {
			WL_INFO_L1("%s: fw_under_survey\n", __func__);
			wl_os_scan_finish_mark(Nic, 1);
			clr_fwstate(pmlmepriv, _FW_UNDER_SURVEY);
		}

		if (proc_get_ch_setting_union_func(Nic, &ch, &bw, &offset, 1) != 0) {
			WL_INFO(FUNC_ADPT_FMT
					" back to linked/linking union - ch:%u, bw:%u, offset:%u\n",
					FUNC_ADPT_ARG(Nic), ch, bw, offset);
			do_set_chabw(Nic, ch, offset, bw);
		}
#ifdef CONFIG_CONCURRENT_MODE
		if (wl_partner_wadptdata_up(Nic)) {
			Func_Of_Proc_Suspend_Free_Assoc_Resource(Nic->pbuddy_wadptdata);
		}
#endif

		if (pwrpriv->wowlan_pno_enable) {
			WL_INFO_L1("%s: pno: %d\n", __func__, pwrpriv->wowlan_pno_enable);
		}
#ifdef CONFIG_LPS
		else
			do_set_ps_mode(0, 0, Nic, PS_MODE_MAX, "WOWLAN");
#endif

	} else {
		WL_INFO_L1("%s: ### ERROR ### wowlan_mode=%d\n", __FUNCTION__,
				   pwrpriv->wowlan_mode);
	}
	WL_INFO("<== " FUNC_ADPT_FMT " exit....\n", FUNC_ADPT_ARG(Nic));
	return ret;
}
#endif

#ifdef CONFIG_AP_WOWLAN
static int Func_Of_Proc_Suspend_Ap_Wow(PNIC Nic)
{
	u8 ch, bw, offset;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct net_device *pnetdev = Nic->pnetdev;
#ifdef CONFIG_CONCURRENT_MODE
	struct net_device *pbuddy_netdev;
#endif
	struct dvobj_priv *psdpriv = Nic->dvobj;
	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;
	struct pwrctrl_priv *pwrpriv = wadptdata_to_pwrctl(Nic);
	struct wowlan_ioctl_param poidparam;
	u8 ps_mode;
	int ret = TRUE;

	WL_INFO("==> " FUNC_ADPT_FMT " entry....\n", FUNC_ADPT_ARG(Nic));

	pwrpriv->wowlan_ap_mode = TRUE;

	WL_INFO("wowlan_ap_mode: %d\n", pwrpriv->wowlan_ap_mode);

	if (pnetdev)
		wl_netif_stop_queue(pnetdev);
#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic)) {
		pbuddy_netdev = Nic->pbuddy_wadptdata->pnetdev;
		if (pbuddy_netdev)
			wl_netif_stop_queue(pbuddy_netdev);
	}
#endif
	wl_set_drv_stopped(Nic);
	Func_Of_Proc_Stop_Drv_Threads(Nic);
#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic))
		Func_Of_Proc_Stop_Drv_Threads(Nic->pbuddy_wadptdata);
#endif
	wl_clr_drv_stopped(Nic);

	if (Nic->intf_free_irq)
		Nic->intf_free_irq(wadptdata_to_dvobj(Nic));

#ifdef CONFIG_RUNTIME_PORT_SWITCH
	if (check_port_switch_func(Nic, 1)) {
		WL_INFO(" ### PORT SWITCH ### \n");
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_PORT_SWITCH, NULL);
	}
#endif

	poidparam.subcode = WOWLAN_AP_ENABLE;
	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_WOWLAN, (u8 *) & poidparam);

	WL_INFO_L1("%s: wowmode suspending\n", __func__);

#ifdef CONFIG_CONCURRENT_MODE
	if (do_chk_partner_fwstate(Nic, WIFI_AP_STATE) == TRUE) {
		if (proc_get_ch_setting_union_func
			(Nic->pbuddy_wadptdata, &ch, &bw, &offset, 1) != 0) {
			WL_INFO(FUNC_ADPT_FMT
					" back to linked/linking union - ch:%u, bw:%u, offset:%u\n",
					FUNC_ADPT_ARG(Nic->pbuddy_wadptdata), ch, bw, offset);
			do_set_chabw(Nic->pbuddy_wadptdata, ch, offset, bw);
		}
		Func_Of_Proc_Suspend_Free_Assoc_Resource(Nic);
	} else {
		if (proc_get_ch_setting_union_func(Nic, &ch, &bw, &offset, 1) != 0) {
			WL_INFO(FUNC_ADPT_FMT
					" back to linked/linking union - ch:%u, bw:%u, offset:%u\n",
					FUNC_ADPT_ARG(Nic), ch, bw, offset);
			do_set_chabw(Nic, ch, offset, bw);
		}
		Func_Of_Proc_Suspend_Free_Assoc_Resource(Nic->pbuddy_wadptdata);
	}
#else
	if (proc_get_ch_setting_union_func(Nic, &ch, &bw, &offset, 1) != 0) {
		WL_INFO(FUNC_ADPT_FMT
				" back to linked/linking union - ch:%u, bw:%u, offset:%u\n",
				FUNC_ADPT_ARG(Nic), ch, bw, offset);
		do_set_chabw(Nic, ch, offset, bw);
	}
#endif

#ifdef CONFIG_LPS
	do_set_ps_mode(0, 0, Nic, PS_MODE_MIN, "AP-WOWLAN");
#endif

	WL_INFO("<== " FUNC_ADPT_FMT " exit....\n", FUNC_ADPT_ARG(Nic));
	return ret;
}
#endif

static int Func_Of_Proc_Suspend_Normal(PNIC Nic)
{
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct net_device *pnetdev = Nic->pnetdev;
#ifdef CONFIG_CONCURRENT_MODE
	struct net_device *pbuddy_netdev = Nic->pbuddy_wadptdata->pnetdev;
#endif
	struct pwrctrl_priv *pwrpriv = wadptdata_to_pwrctl(Nic);
	int ret = TRUE;

	WL_INFO("==> " FUNC_ADPT_FMT " entry....\n", FUNC_ADPT_ARG(Nic));
	if (pnetdev) {
		netif_carrier_off(pnetdev);
		wl_netif_stop_queue(pnetdev);
	}
#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic)) {
		pbuddy_netdev = Nic->pbuddy_wadptdata->pnetdev;
		netif_carrier_off(pbuddy_netdev);
		wl_netif_stop_queue(pbuddy_netdev);
	}
#endif

	Func_Of_Proc_Suspend_Free_Assoc_Resource(Nic);

#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic)) {
		Func_Of_Proc_Suspend_Free_Assoc_Resource(Nic->pbuddy_wadptdata);
	}
#endif
	if ((wadptdata_to_pwrctl(Nic)->rf_pwrstate == rf_off)) {
		WL_INFO_L1("%s: ### ERROR #### driver in IPS ####ERROR###!!!\n",
				   __FUNCTION__);

	}
#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic)) {
		Func_Of_Proc_Dev_Unload(Nic->pbuddy_wadptdata);
	}
#endif
	Func_Of_Proc_Dev_Unload(Nic);

	if (Nic->intf_deinit)
		Nic->intf_deinit(wadptdata_to_dvobj(Nic));

	WL_INFO("<== " FUNC_ADPT_FMT " exit....\n", FUNC_ADPT_ARG(Nic));
	return ret;
}

int Func_Of_Proc_Suspend_Common(PNIC Nic)
{
	struct dvobj_priv *psdpriv = Nic->dvobj;
	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(psdpriv);
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;

	int ret = 0;
	u32 start_time = Func_Of_Proc_Get_Current_Time();

	WL_INFO_L1(" suspend start\n");
	WL_INFO("==> %s (%s:%d)\n", __FUNCTION__, current->comm, current->pid);

	pdbgpriv->dbg_suspend_cnt++;

	pwrpriv->bInSuspend = TRUE;

	while (pwrpriv->bips_processing == TRUE)
		Func_Of_Proc_Msleep_Os(1);

	if ((!Nic->bup) || WL_CANNOT_RUN(Nic)) {
		WL_INFO("%s bup=%d bDriverStopped=%s bSurpriseRemoved = %s\n", __func__,
				Nic->bup, wl_is_drv_stopped(Nic) ? "True" : "False",
				wl_is_surprise_removed(Nic) ? "True" : "False");
		pdbgpriv->dbg_suspend_error_cnt++;
		goto exit;
	}
	do_deny_ps(Nic, PS_DENY_SUSPEND, 1);

	Func_Of_Proc_Cancel_All_Timer(Nic);
#ifdef CONFIG_CONCURRENT_MODE
	if (Nic->pbuddy_wadptdata) {
		Func_Of_Proc_Cancel_All_Timer(Nic->pbuddy_wadptdata);
	}
#endif

	left_aps_direct(Nic, 1);

	proc_stop_cmd_thread_func(Nic, 1);

	do_cancel_deny_ps(Nic, PS_DENY_SUSPEND, 1);

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == TRUE
#ifdef CONFIG_CONCURRENT_MODE
		&& do_chk_partner_fwstate(Nic, WIFI_AP_STATE) == FALSE
#endif
		) {
#ifdef CONFIG_WOWLAN
		if (check_fwstate(pmlmepriv, _FW_LINKED)) {
			pwrpriv->wowlan_mode = TRUE;
		} else if (pwrpriv->wowlan_pno_enable == TRUE) {
			pwrpriv->wowlan_mode |= pwrpriv->wowlan_pno_enable;
		}
#ifdef CONFIG_P2P_WOWLAN
		if (!wl_p2p_chk_state(&Nic->wdinfo, P2P_STATE_NONE)
			|| P2P_ROLE_DISABLE != Nic->wdinfo.role) {
			pwrpriv->wowlan_p2p_mode = TRUE;
		}
		if (TRUE == pwrpriv->wowlan_p2p_mode)
			pwrpriv->wowlan_mode |= pwrpriv->wowlan_p2p_mode;
#endif

		if (pwrpriv->wowlan_mode == TRUE)
			Func_Of_Proc_Suspend_Wow(Nic);
		else
			Func_Of_Proc_Suspend_Normal(Nic);

#else
		Func_Of_Proc_Suspend_Normal(Nic);
#endif
	} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == TRUE
#ifdef CONFIG_CONCURRENT_MODE
			   && do_chk_partner_fwstate(Nic, WIFI_AP_STATE) == FALSE
#endif
		) {
#ifdef CONFIG_AP_WOWLAN
		Func_Of_Proc_Suspend_Ap_Wow(Nic);
#else
		Func_Of_Proc_Suspend_Normal(Nic);
#endif
#ifdef CONFIG_CONCURRENT_MODE
	} else if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == TRUE
			   && do_chk_partner_fwstate(Nic, WIFI_AP_STATE) == TRUE) {
#ifdef CONFIG_AP_WOWLAN
		Func_Of_Proc_Suspend_Ap_Wow(Nic);
#else
		Func_Of_Proc_Suspend_Normal(Nic);
#endif
#endif
	} else {
		Func_Of_Proc_Suspend_Normal(Nic);
	}

	WL_INFO_L1("wl suspend success in %d ms\n",
			   Func_Of_Proc_Get_Passing_Time_Ms(start_time));

exit:
	WL_INFO("<===  %s return %d.............. in %dms\n", __FUNCTION__, ret,
			Func_Of_Proc_Get_Passing_Time_Ms(start_time));

	return ret;
}

#ifdef CONFIG_WOWLAN
static int Func_Of_Proc_Resume_Process_Wow(PNIC Nic)
{
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct net_device *pnetdev = Nic->pnetdev;
#ifdef CONFIG_CONCURRENT_MODE
	struct net_device *pbuddy_netdev;
#endif
	struct pwrctrl_priv *pwrpriv = wadptdata_to_pwrctl(Nic);
	struct dvobj_priv *psdpriv = Nic->dvobj;
	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;
	struct wowlan_ioctl_param poidparam;
	struct sta_info *psta = NULL;
	int ret = TRUE;
	_func_enter_;

	WL_INFO("==> " FUNC_ADPT_FMT " entry....\n", FUNC_ADPT_ARG(Nic));

	if (Nic) {
		pnetdev = Nic->pnetdev;
		pwrpriv = wadptdata_to_pwrctl(Nic);
	} else {
		pdbgpriv->dbg_resume_error_cnt++;
		ret = -1;
		goto exit;
	}

	if (WL_CANNOT_RUN(Nic)) {
		WL_INFO("%s pdapter %p bDriverStopped %s bSurpriseRemoved %s\n",
				__func__, Nic,
				wl_is_drv_stopped(Nic) ? "True" : "False",
				wl_is_surprise_removed(Nic) ? "True" : "False");
		goto exit;
	}

	if (pwrpriv->wowlan_mode == TRUE) {
#ifdef CONFIG_LPS
		do_set_ps_mode(0, 0, Nic, PS_MODE_ACTIVE, "WOWLAN");
#endif

		pwrpriv->bFwCurrentInPSMode = FALSE;

		if ((Nic->intf_alloc_irq)
			&& (Nic->intf_alloc_irq(wadptdata_to_dvobj(Nic)) !=
				TRUE)) {
			ret = -1;
			WL_INFO_L2(("%s: sdio_alloc_irq Failed!!\n", __FUNCTION__));
			goto exit;
		}

		poidparam.subcode = WOWLAN_DISABLE;
		Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_WOWLAN, (u8 *) & poidparam);

#ifdef CONFIG_CONCURRENT_MODE
		Func_Of_Proc_Reset_Drv_Sw(Nic->pbuddy_wadptdata);
#endif

		psta =
			do_query_stainfo(&Nic->stapriv, get_bssid(&Nic->mlmepriv),
							1);
		if (psta) {
			Func_Of_Proc_Chip_Hw_Update_Ra_Mask(psta, psta->rssi_level);
		}

		wl_clr_drv_stopped(Nic);
		WL_INFO("%s: wowmode resuming, DriverStopped:%s\n", __func__,
				wl_is_drv_stopped(Nic) ? "True" : "False");
		Func_Of_Proc_Start_Drv_Threads(Nic);

#ifdef CONFIG_CONCURRENT_MODE
		if (Nic->pbuddy_wadptdata)
			Func_Of_Proc_Start_Drv_Threads(Nic->pbuddy_wadptdata);
#endif

		if (Nic->intf_start) {
			Nic->intf_start(Nic);
		}
#ifdef CONFIG_CONCURRENT_MODE
		if (wl_partner_wadptdata_up(Nic)) {
			Nic->pbuddy_wadptdata->intf_start(Nic->pbuddy_wadptdata);
		}

		if (wl_partner_wadptdata_up(Nic)) {
			pbuddy_netdev = Nic->pbuddy_wadptdata->pnetdev;

			if (pbuddy_netdev) {
				netif_device_attach(pbuddy_netdev);
				netif_carrier_on(pbuddy_netdev);
			}
		}
#endif

		if (pnetdev) {
			wl_netif_wake_queue(pnetdev);
		}
	} else {

		WL_INFO_L1("%s: ### ERROR ### wowlan_mode=%d\n", __FUNCTION__,
				   pwrpriv->wowlan_mode);
	}

	if (Nic->pid[1] != 0) {
		WL_INFO("pid[1]:%d\n", Nic->pid[1]);
		wl_signal_process(Nic->pid[1], SIGUSR2);
	}

	if (wl_chk_roam_flags(Nic, WL_ROAM_ON_RESUME)) {
		if (pwrpriv->wowlan_wake_reason == FWDecisionDisconnect ||
			pwrpriv->wowlan_wake_reason == Rx_DisAssoc ||
			pwrpriv->wowlan_wake_reason == Rx_DeAuth) {

			WL_INFO("%s: disconnect reason: %02x\n", __func__,
					pwrpriv->wowlan_wake_reason);
			wl_disconnect_flag(Nic, 0, FALSE, 1);

			rpt_sta_media_status(Nic,
									 do_query_stainfo(&Nic->stapriv,
													 get_bssid(&Nic->
															   mlmepriv), 1), 0,
									 1);

			do_assoc_resources_unnew(Nic, 1, 1);
			pmlmeinfo->state = WIFI_FW_NULL_STATE;

		} else {
			WL_INFO("%s: do roaming\n", __func__);
			wl_roaming(Nic, NULL);
		}
	}

	if (pwrpriv->wowlan_wake_reason == FWDecisionDisconnect) {
		Func_Of_Proc_Lock_Ext_Suspend_Timeout(2000);
	}

	if (pwrpriv->wowlan_wake_reason == Rx_GTK ||
		pwrpriv->wowlan_wake_reason == Rx_DisAssoc ||
		pwrpriv->wowlan_wake_reason == Rx_DeAuth) {
		Func_Of_Proc_Lock_Ext_Suspend_Timeout(8000);
	}

	if (pwrpriv->wowlan_wake_reason == RX_PNOWakeUp) {
		Func_Of_Proc_Lock_Ext_Suspend_Timeout(10000);
	}

	if (pwrpriv->wowlan_mode == TRUE) {
		pwrpriv->bips_processing = FALSE;
		_set_timer(&Nic->mlmepriv.dynamic_chk_timer, 2000);
		wl_set_pwr_state_check_timer(pwrpriv);
	} else {
		WL_INFO_L1("do not reset timer\n");
	}

	pwrpriv->wowlan_mode = FALSE;

	pwrpriv->wowlan_wake_reason = 0;

exit:
	WL_INFO("<== " FUNC_ADPT_FMT " exit....\n", FUNC_ADPT_ARG(Nic));
	_func_exit_;
	return ret;
}
#endif

#ifdef CONFIG_AP_WOWLAN
static int Func_Of_Proc_Resume_Process_Ap_Wow(PNIC Nic)
{
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;
	struct net_device *pnetdev = Nic->pnetdev;
#ifdef CONFIG_CONCURRENT_MODE
	struct net_device *pbuddy_netdev;
#endif
	struct pwrctrl_priv *pwrpriv = wadptdata_to_pwrctl(Nic);
	struct dvobj_priv *psdpriv = Nic->dvobj;
	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;
	struct wowlan_ioctl_param poidparam;
	struct sta_info *psta = NULL;
	int ret = TRUE;
	u8 ch, bw, offset;
	_func_enter_;

	WL_INFO("==> " FUNC_ADPT_FMT " entry....\n", FUNC_ADPT_ARG(Nic));

	if (Nic) {
		pnetdev = Nic->pnetdev;
		pwrpriv = wadptdata_to_pwrctl(Nic);
	} else {
		pdbgpriv->dbg_resume_error_cnt++;
		ret = -1;
		goto exit;
	}

#ifdef CONFIG_LPS
	do_set_ps_mode(0, 0, Nic, PS_MODE_ACTIVE, "AP-WOWLAN");
#endif

	pwrpriv->bFwCurrentInPSMode = FALSE;

	if ((Nic->intf_alloc_irq)
		&& (Nic->intf_alloc_irq(wadptdata_to_dvobj(Nic)) != TRUE)) {
		ret = -1;
		WL_INFO_L2("%s: sdio_alloc_irq Failed!!\n", __FUNCTION__);
		goto exit;
	}

	poidparam.subcode = WOWLAN_AP_DISABLE;
	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_WOWLAN, (u8 *) & poidparam);
	pwrpriv->wowlan_ap_mode = FALSE;

	wl_clr_drv_stopped(Nic);
	WL_INFO("%s: wowmode resuming, DriverStopped:%s\n", __func__,
			wl_is_drv_stopped(Nic) ? "True" : "False");
	Func_Of_Proc_Start_Drv_Threads(Nic);

#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic))
		Func_Of_Proc_Start_Drv_Threads(Nic->pbuddy_wadptdata);
#endif

#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic)) {
		if (proc_get_ch_setting_union_func
			(Nic->pbuddy_wadptdata, &ch, &bw, &offset, 1) != 0) {
			WL_INFO(FUNC_ADPT_FMT
					" back to linked/linking union - ch:%u, bw:%u, offset:%u\n",
					FUNC_ADPT_ARG(Nic->pbuddy_wadptdata), ch, bw, offset);
			do_set_chabw(Nic->pbuddy_wadptdata, ch, offset, bw);
		}
	} else {
		WL_INFO(FUNC_ADPT_FMT
				" back to linked/linking union - ch:%u, bw:%u, offset:%u\n",
				FUNC_ADPT_ARG(Nic), ch, bw, offset);
		do_set_chabw(Nic, ch, offset, bw);
		Func_Of_Proc_Reset_Drv_Sw(Nic->pbuddy_wadptdata);
	}
#else
	if (proc_get_ch_setting_union_func(Nic, &ch, &bw, &offset, 1) != 0) {
		WL_INFO(FUNC_ADPT_FMT
				" back to linked/linking union - ch:%u, bw:%u, offset:%u\n",
				FUNC_ADPT_ARG(Nic), ch, bw, offset);
		do_set_chabw(Nic, ch, offset, bw);
	}
#endif

	if (Nic->intf_start) {
		Nic->intf_start(Nic);
	}
#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic)) {
		Nic->pbuddy_wadptdata->intf_start(Nic->pbuddy_wadptdata);
	}
#endif

#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic)) {
		pbuddy_netdev = Nic->pbuddy_wadptdata->pnetdev;
		if (pbuddy_netdev) {
			wl_netif_wake_queue(pbuddy_netdev);
		}
	}
#endif

	if (pnetdev) {
		wl_netif_wake_queue(pnetdev);
	}

	if (Nic->pid[1] != 0) {
		WL_INFO("pid[1]:%d\n", Nic->pid[1]);
		wl_signal_process(Nic->pid[1], SIGUSR2);
	}

	if (pwrpriv->wowlan_wake_reason == AP_WakeUp)
		Func_Of_Proc_Lock_Ext_Suspend_Timeout(8000);

	pwrpriv->bips_processing = FALSE;
	_set_timer(&Nic->mlmepriv.dynamic_chk_timer, 2000);
	wl_set_pwr_state_check_timer(pwrpriv);
	pwrpriv->wowlan_wake_reason = 0;
exit:
	WL_INFO("<== " FUNC_ADPT_FMT " exit....\n", FUNC_ADPT_ARG(Nic));
	_func_exit_;
	return ret;
}
#endif

static int Func_Of_Proc_Resume_Process_Normal(PNIC Nic)
{
	struct net_device *pnetdev;
#ifdef CONFIG_CONCURRENT_MODE
	struct net_device *pbuddy_netdev;
#endif
	struct pwrctrl_priv *pwrpriv;
	struct mlme_priv *pmlmepriv;
	struct dvobj_priv *psdpriv;
	struct debug_priv *pdbgpriv;

	int ret = TRUE;
	_func_enter_;

	if (!Nic) {
		ret = -1;
		goto exit;
	}

	pnetdev = Nic->pnetdev;
	pwrpriv = wadptdata_to_pwrctl(Nic);
	pmlmepriv = &Nic->mlmepriv;
	psdpriv = Nic->dvobj;
	pdbgpriv = &psdpriv->drv_dbg;

	WL_INFO("==> " FUNC_ADPT_FMT " entry....\n", FUNC_ADPT_ARG(Nic));
	if ((Nic->intf_init)
		&& (Nic->intf_init(wadptdata_to_dvobj(Nic)) != TRUE)) {
		ret = -1;
		WL_INFO_L2("%s: initialize SDIO Failed!!\n", __FUNCTION__);
		goto exit;
	}
	if ((Nic->intf_alloc_irq)
		&& (Nic->intf_alloc_irq(wadptdata_to_dvobj(Nic)) != TRUE)) {
		ret = -1;
		WL_INFO_L2("%s: sdio_alloc_irq Failed!!\n", __FUNCTION__);
		goto exit;
	}

	Func_Of_Proc_Reset_Drv_Sw(Nic);
#ifdef CONFIG_CONCURRENT_MODE
	Func_Of_Proc_Reset_Drv_Sw(Nic->pbuddy_wadptdata);
#endif

	pwrpriv->bkeepfwalive = FALSE;

	WL_INFO("bkeepfwalive(%x)\n", pwrpriv->bkeepfwalive);
	if (Func_Pm_Netdev_Open(pnetdev, TRUE) != 0) {
		ret = -1;
		pdbgpriv->dbg_resume_error_cnt++;
		goto exit;
	}

	netif_device_attach(pnetdev);
	netif_carrier_on(pnetdev);

#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic)) {
		pbuddy_netdev = Nic->pbuddy_wadptdata->pnetdev;

		netif_device_attach(pbuddy_netdev);
		netif_carrier_on(pbuddy_netdev);
	}
#endif

	if (Nic->pid[1] != 0) {
		WL_INFO("pid[1]:%d\n", Nic->pid[1]);
		wl_signal_process(Nic->pid[1], SIGUSR2);
	}

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
		WL_INFO(FUNC_ADPT_FMT " fwstate:0x%08x - WIFI_STATION_STATE\n",
				FUNC_ADPT_ARG(Nic), get_fwstate(pmlmepriv));

		if (wl_chk_roam_flags(Nic, WL_ROAM_ON_RESUME))
			wl_roaming(Nic, NULL);

	} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		WL_INFO(FUNC_ADPT_FMT " fwstate:0x%08x - WIFI_AP_STATE\n",
				FUNC_ADPT_ARG(Nic), get_fwstate(pmlmepriv));
		restore_ap_network_func(Nic, 1);
	} else if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE)) {
		WL_INFO(FUNC_ADPT_FMT " fwstate:0x%08x - WIFI_ADHOC_STATE\n",
				FUNC_ADPT_ARG(Nic), get_fwstate(pmlmepriv));
	} else {
		WL_INFO(FUNC_ADPT_FMT " fwstate:0x%08x - ???\n",
				FUNC_ADPT_ARG(Nic), get_fwstate(pmlmepriv));
	}

#ifdef CONFIG_CONCURRENT_MODE
	if (wl_partner_wadptdata_up(Nic)) {
		_wadptdata *buddy = Nic->pbuddy_wadptdata;
		struct mlme_priv *buddy_mlme = &Nic->pbuddy_wadptdata->mlmepriv;
		if (check_fwstate(buddy_mlme, WIFI_STATION_STATE)) {
			WL_INFO(FUNC_ADPT_FMT " fwstate:0x%08x - WIFI_STATION_STATE\n",
					FUNC_ADPT_ARG(buddy), get_fwstate(buddy_mlme));

			if (wl_chk_roam_flags(buddy, WL_ROAM_ON_RESUME))
				wl_roaming(buddy, NULL);

		} else if (check_fwstate(buddy_mlme, WIFI_AP_STATE)) {
			WL_INFO(FUNC_ADPT_FMT " fwstate:0x%08x - WIFI_AP_STATE\n",
					FUNC_ADPT_ARG(buddy), get_fwstate(buddy_mlme));
			restore_ap_network_func(buddy, 1);
		} else if (check_fwstate(buddy_mlme, WIFI_ADHOC_STATE)) {
			WL_INFO(FUNC_ADPT_FMT " fwstate:0x%08x - WIFI_ADHOC_STATE\n",
					FUNC_ADPT_ARG(buddy), get_fwstate(buddy_mlme));
		} else {
			WL_INFO(FUNC_ADPT_FMT " fwstate:0x%08x - ???\n",
					FUNC_ADPT_ARG(buddy), get_fwstate(buddy_mlme));
		}
	}
#endif

	WL_INFO("<== " FUNC_ADPT_FMT " exit....\n", FUNC_ADPT_ARG(Nic));

exit:
	_func_exit_;
	return ret;
}

int Func_Of_Proc_Resume_Common(PNIC Nic)
{
	int ret = 0;
	u32 start_time = Func_Of_Proc_Get_Current_Time();
	struct pwrctrl_priv *pwrpriv = &Nic->pwrctl_priv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;

	_func_enter_;

	if (pwrpriv->bInSuspend == FALSE)
		return 0;

    MpTrace(COMP_RICH_INIT, DBG_NORMAL, ("resume start\n"));
    MpTrace(COMP_RICH_INIT, DBG_NORMAL, ("==> %s (%s:%d)\n", __FUNCTION__, current->comm, current->pid));
	
	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == TRUE
#ifdef CONFIG_CONCURRENT_MODE
		&& do_chk_partner_fwstate(Nic, WIFI_AP_STATE) == FALSE
#endif
		) {
#ifdef CONFIG_WOWLAN
		if (pwrpriv->wowlan_mode == TRUE)
			Func_Of_Proc_Resume_Process_Wow(Nic);
		else
			Func_Of_Proc_Resume_Process_Normal(Nic);
#else
		Func_Of_Proc_Resume_Process_Normal(Nic);
#endif

	} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == TRUE
#ifdef CONFIG_CONCURRENT_MODE
			   && do_chk_partner_fwstate(Nic, WIFI_AP_STATE) == FALSE
#endif
		) {
#ifdef CONFIG_AP_WOWLAN
		Func_Of_Proc_Resume_Process_Ap_Wow(Nic);
#else
		Func_Of_Proc_Resume_Process_Normal(Nic);
#endif
#ifdef CONFIG_CONCURRENT_MODE
	} else if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == TRUE
			   && do_chk_partner_fwstate(Nic, WIFI_AP_STATE) == TRUE) {
#ifdef CONFIG_AP_WOWLAN
		Func_Of_Proc_Resume_Process_Ap_Wow(Nic);
#else
		Func_Of_Proc_Resume_Process_Normal(Nic);
#endif
#endif
	} else {
		Func_Of_Proc_Resume_Process_Normal(Nic);
	}

	if (pwrpriv) {
		pwrpriv->bInSuspend = FALSE;
	}
	WL_INFO_L1("%s:%d in %d ms\n", __FUNCTION__, ret,
			   Func_Of_Proc_Get_Passing_Time_Ms(start_time));

	_func_exit_;

	return ret;
}
#endif
