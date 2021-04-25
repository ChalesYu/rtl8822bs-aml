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

#ifndef __WL_CMD_H_
#define __WL_CMD_H_

#define wMBOX0_MEM_SZ (16*1024)

#define FREE_CMDOBJ_SZ	128

#define MAX_CMDSZ	1024
#define MAX_RSPSZ	512
#define MAX_EVTSZ	1024

#define CMDBUFF_ALIGN_SZ 512


struct cmd_obj {
//	_wadptdata *pwadptdata;
	u16 cmdcode;                          //cmdcode
	u8 res;                               //wMBOX1 reponse such as wMBOX1_SUCCESS
	u8 *parmbuf;                          //parameter cmd buffer
	u32 cmdsz;                            //cmd buffer size
	u8 *rsp;                              //reponse buffer
	u32 rspsz;                            //reponse buffer size
	struct submit_ctx *sctx;              //time info
	u8 no_io;                              //not importent
	LIST_ENTRY list;
};

enum {
	WL_CMDF_DIRECTLY = BIT0,
	WL_CMDF_WAIT_ACK = BIT1,
};

struct cmd_priv {
	KSEMAPHORE cmd_queue_sema;
	KSEMAPHORE terminate_cmdthread_sema;
	_queue cmd_queue;
	u8 cmd_seq;
	u8 *cmd_buf;                        /* shall be non-paged, and 4 bytes aligned */
	u8 *cmd_allocated_buf;              /* allocated buffer pool*/
	u8 *rsp_buf;
	u8 *rsp_allocated_buf;
	u32 cmd_issued_cnt;
	u32 cmd_done_cnt;
	u32 rsp_cnt;
//	ATOMIC_T cmdthd_running;
    u32 cmdthd_running;
	u8 stop_req;
//	_wadptdata *pwadptdata;
	NDIS_MUTEX  sctx_mutex;
};

struct evt_priv {

#define CONFIG_wMBOX0_WK

#ifdef CONFIG_wMBOX0_WK
	WORK_QUEUE_ITEM wmbox0_wk;
	bool wmbox0_wk_alive;
	struct wl_cbuf *wmbox0_queue;
#define wMBOX0_QUEUE_MAX_LEN 10
#endif

//	ATOMIC_T event_seq;
	u32 event_seq;
	u8 *evt_buf;
	u8 *evt_allocated_buf;
	u32 evt_done_cnt;

};


#define init_h2fwcmd_w_parm_no_rsp(pcmd, pparm, code) \
do {\
Func_Of_Proc_Init_Listhead(&pcmd->list);\
pcmd->cmdcode = code;\
pcmd->parmbuf = (u8 *)(pparm);\
pcmd->cmdsz = sizeof (*pparm);\
pcmd->rsp = NULL;\
pcmd->rspsz = 0;\
} while(0)

#define init_h2fwcmd_w_parm_no_parm_rsp(pcmd, code) \
do {\
Func_Of_Proc_Init_Listhead(&pcmd->list);\
pcmd->cmdcode = code;\
pcmd->parmbuf = NULL;\
pcmd->cmdsz = 0;\
pcmd->rsp = NULL;\
pcmd->rspsz = 0;\
} while(0)

struct wmbox0_evt_hdr {
	u8 id:4;
	u8 plen:4;
	u8 seq;
	u8 payload[0];
};

struct wmbox0_evt_hdr_88xx {
	u8 id;
	u8 seq;
	u8 payload[12];
	u8 plen;
	u8 trigger;
};

#define wmbox0_evt_valid(wmbox0_evt) ((wmbox0_evt)->id || (wmbox0_evt)->plen)

struct P2P_PS_Offload_t {
	u8 Offload_En:1;
	u8 role:1;
	u8 CTWindow_En:1;
	u8 NoA0_En:1;
	u8 NoA1_En:1;
	u8 AllStaSleep:1;
	u8 discovery:1;
	u8 rsvd:1;
};

struct P2P_PS_CTWPeriod_t {
	u8 CTWPeriod;
};

#ifdef CONFIG_P2P_WOWLAN

struct P2P_WoWlan_Offload_t {
	u8 Disconnect_Wkup_Drv:1;
	u8 role:2;
	u8 Wps_Config[2];
};

#endif

u32 preproc_enqueue_cmd_func(struct cmd_priv *pcmdpriv, struct cmd_obj *obj);
struct cmd_obj *preproc_dequeue_cmd_func(struct cmd_priv *pcmdpriv);
void proc_free_cmd_obj_func(struct cmd_obj *pcmd);

//need change
//void proc_stop_cmd_thread_func(PNIC Nic, u8 flag);
//thread_return proc_cmd_thread_func(thread_context context);

u32 cmd_priv_to_init_func(struct cmd_priv *pcmdpriv);
void preproc_free_cmd_priv_func(struct cmd_priv *pcmdpriv);
u32 evt_priv_to_init_func(struct evt_priv *pevtpriv);
void preproc_free_evt_priv_func(struct evt_priv *pevtpriv);

void proc_cmd_clr_isr_func(struct cmd_priv *pcmdpriv);
void proc_evt_notify_isr_func(struct evt_priv *pevtpriv);
#ifdef CONFIG_P2P
u8 protocol_p2p_wk_cmd_func(PNIC Nic, int intCmdType);
#endif

enum wl_drvextra_cmd_id {
	NONE_WK_CID,
	STA_MSTATUS_RPT_WK_CID,
	DYNAMIC_CHK_WK_CID,
	DM_CTRL_WK_CID,
	PBC_POLLING_WK_CID,
	POWER_SAVING_CTRL_WK_CID,
	LPS_CTRL_WK_CID,
	ANT_SELECT_WK_CID,
	P2P_PS_WK_CID,
	P2P_PROTO_WK_CID,
	CHECK_HIQ_WK_CID,
	INTEl_WIDI_WK_CID,
	wMBOX0_WK_CID,
	RTP_TIMER_CFG_WK_CID,
	RESET_SECURITYPRIV,
	FREE_ASSOC_RESOURCES,
	DM_IN_LPS_WK_CID,
	DM_RA_MSK_WK_CID,
	BEAMFORMING_WK_CID,
	LPS_CHANGE_DTIM_CID,
	BTINFO_WK_CID,
	DFS_MASTER_WK_CID,
	SESSION_TRACKER_WK_CID,
	EN_HW_UPDATE_TSF_WK_CID,
	MP_CMD_WK_CID,
	MAX_WK_CID,
	PHY_STATUS_CID
};

enum LPS_CTRL_TYPE {
	LPS_CTRL_SCAN = 0,
	LPS_CTRL_JOINBSS = 1,
	LPS_CTRL_CONNECT = 2,
	LPS_CTRL_DISCONNECT = 3,
	LPS_CTRL_SPECIAL_PACKET = 4,
	LPS_CTRL_LEAVE = 5,
	LPS_CTRL_TRAFFIC_BUSY = 6,
	LPS_CTRL_TX_TRAFFIC_LEAVE = 7,
	LPS_CTRL_RX_TRAFFIC_LEAVE = 8,
	LPS_CTRL_ENTER = 9,
	LPS_CTRL_LEAVE_CFG80211_PWRMGMT = 10,
};

enum STAKEY_TYPE {
	GROUP_KEY = 0,
	UNICAST_KEY = 1,
	TDLS_KEY = 2,
};

enum RFINTFS {
	SWSI,
	HWSI,
	HWPI,
};

struct usb_suspend_parm {
	u32 action;
};

struct joinbss_parm {
	WLAN_BSSID_EX network;
};

struct disconnect_parm {
	u32 deauth_timeout_ms;
};

struct createbss_parm {
	bool adhoc;

	s16 req_ch;
	u8 req_bw;
	u8 req_offset;
};

struct setopmode_parm {
	u8 mode;
	u8 rsvd[3];
};

#define WL_SSID_SCAN_AMOUNT 9
#define WL_CHANNEL_SCAN_AMOUNT (14+37)
struct sitesurvey_parm {
	sint scan_mode;
	u8 ssid_num;
	u8 ch_num;
	NDIS_802_11_SSID ssid[WL_SSID_SCAN_AMOUNT];
	struct wl_ieee80211_channel ch[WL_CHANNEL_SCAN_AMOUNT];
};

struct setauth_parm {
	u8 mode;
	u8 _1x;
	u8 rsvd[2];
};

struct setkey_parm {
	u8 algorithm;
	u8 keyid;
	u8 grpkey;
	u8 set_tx;
	u8 key[16];
};

struct set_stakey_parm {
	u8 addr[ETH_ALEN];
	u8 algorithm;
	u8 keyid;
	u8 key[16];
};

struct set_stakey_rsp {
	u8 addr[ETH_ALEN];
	u8 keyid;
	u8 rsvd;
};

struct set_assocsta_parm {
	u8 addr[ETH_ALEN];
};

struct set_assocsta_rsp {
	u8 cam_id;
	u8 rsvd[3];
};

struct del_assocsta_parm {
	u8 addr[ETH_ALEN];
};

struct setstapwrstate_parm {
	u8 staid;
	u8 status;
	u8 hwaddr[6];
};

struct setbasicrate_parm {
	u8 basicrates[NumRates];
};

struct getbasicrate_parm {
	u32 rsvd;
};

struct getbasicrate_rsp {
	u8 basicrates[NumRates];
};

struct setdatarate_parm {
#ifdef MP_FIRMWARE_OFFLOAD
	u32 curr_rateidx;
#else
	u8 mac_id;
	u8 datarates[NumRates];
#endif
};

struct getdatarate_parm {
	u32 rsvd;

};
struct getdatarate_rsp {
	u8 datarates[NumRates];
};

struct setphyinfo_parm {
	struct regulatory_class class_sets[1];
	u8 status;
};

struct getphyinfo_parm {
	u32 rsvd;
};

struct getphyinfo_rsp {
	struct regulatory_class class_sets[1];
	u8 status;
};

struct setphy_parm {
	u8 rfchannel;
	u8 modem;
};

struct getphy_parm {
	u32 rsvd;

};
struct getphy_rsp {
	u8 rfchannel;
	u8 modem;
};

struct readBB_parm {
	u8 offset;
};
struct readBB_rsp {
	u8 value;
};

struct readTSSI_parm {
	u8 offset;
};
struct readTSSI_rsp {
	u8 value;
};

struct readMAC_parm {
	u8 len;
	u32 addr;
};

struct writeBB_parm {
	u8 offset;
	u8 value;
};

struct readRF_parm {
	u8 offset;
};
struct readRF_rsp {
	u32 value;
};

struct writeRF_parm {
	u32 offset;
	u32 value;
};

struct getrfintfs_parm {
	u8 rfintfs;
};

struct Tx_Beacon_param {
	WLAN_BSSID_EX network;
};

struct drvextra_cmd_parm {
	int ec_id;
	int type;
	int size;
	unsigned char *pbuf;
};

struct setantenna_parm {
	u8 tx_antset;
	u8 rx_antset;
	u8 tx_antenna;
	u8 rx_antenna;
};

struct enrateadaptive_parm {
	u32 en;
};

struct settxagctbl_parm {
	u32 txagc[MAX_RATES_LENGTH];
};

struct gettxagctbl_parm {
	u32 rsvd;
};
struct gettxagctbl_rsp {
	u32 txagc[MAX_RATES_LENGTH];
};

struct setagcctrl_parm {
	u32 agcctrl;
};

struct setssup_parm {
	u32 ss_ForceUp[MAX_RATES_LENGTH];
};

struct getssup_parm {
	u32 rsvd;
};
struct getssup_rsp {
	u8 ss_ForceUp[MAX_RATES_LENGTH];
};

struct setssdlevel_parm {
	u8 ss_DLevel[MAX_RATES_LENGTH];
};

struct getssdlevel_parm {
	u32 rsvd;
};
struct getssdlevel_rsp {
	u8 ss_DLevel[MAX_RATES_LENGTH];
};

struct setssulevel_parm {
	u8 ss_ULevel[MAX_RATES_LENGTH];
};

struct getssulevel_parm {
	u32 rsvd;
};
struct getssulevel_rsp {
	u8 ss_ULevel[MAX_RATES_LENGTH];
};

struct setcountjudge_parm {
	u8 count_judge[MAX_RATES_LENGTH];
};

struct getcountjudge_parm {
	u32 rsvd;
};
struct getcountjudge_rsp {
	u8 count_judge[MAX_RATES_LENGTH];
};

struct setratable_parm {
	u8 ss_ForceUp[NumRates];
	u8 ss_ULevel[NumRates];
	u8 ss_DLevel[NumRates];
	u8 count_judge[NumRates];
};

struct getratable_parm {
	uint rsvd;
};
struct getratable_rsp {
	u8 ss_ForceUp[NumRates];
	u8 ss_ULevel[NumRates];
	u8 ss_DLevel[NumRates];
	u8 count_judge[NumRates];
};

struct gettxretrycnt_parm {
	unsigned int rsvd;
};
struct gettxretrycnt_rsp {
	unsigned long tx_retrycnt;
};

struct getrxretrycnt_parm {
	unsigned int rsvd;
};
struct getrxretrycnt_rsp {
	unsigned long rx_retrycnt;
};

struct getbcnokcnt_parm {
	unsigned int rsvd;
};
struct getbcnokcnt_rsp {
	unsigned long bcnokcnt;
};

struct getbcnerrcnt_parm {
	unsigned int rsvd;
};
struct getbcnerrcnt_rsp {
	unsigned long bcnerrcnt;
};

struct getcurtxpwrlevel_parm {
	unsigned int rsvd;
};
struct getcurtxpwrlevel_rsp {
	unsigned short tx_power;
};

struct setprobereqextraie_parm {
	unsigned char e_id;
	unsigned char ie_len;
	unsigned char ie[0];
};

struct setassocreqextraie_parm {
	unsigned char e_id;
	unsigned char ie_len;
	unsigned char ie[0];
};

struct setproberspextraie_parm {
	unsigned char e_id;
	unsigned char ie_len;
	unsigned char ie[0];
};

struct setassocrspextraie_parm {
	unsigned char e_id;
	unsigned char ie_len;
	unsigned char ie[0];
};

struct addBaReq_parm {
	unsigned int tid;
	u8 addr[ETH_ALEN];
};

struct addBaRsp_parm {
	unsigned int tid;
	unsigned int start_seq;
	u8 addr[ETH_ALEN];
	u8 status;
	u8 size;
};

struct set_ch_parm {
	u8 ch;
	u8 bw;
	u8 ch_offset;
};

#ifdef MP_FIRMWARE_OFFLOAD
struct SetTxPower_parm {
	u8 TxPower;
};

struct SwitchAntenna_parm {
	u16 antenna_tx;
	u16 antenna_rx;
	u8 cck_txrx;
};

struct SetCrystalCap_parm {
	u32 curr_crystalcap;
};

struct SetSingleCarrierTx_parm {
	u8 bStart;
};

struct SetSingleToneTx_parm {
	u8 bStart;
	u8 curr_rfpath;
};

struct SetCarrierSuppressionTx_parm {
	u8 bStart;
	u32 curr_rateidx;
};

struct SetContinuousTx_parm {
	u8 bStart;
	u8 CCK_flag;
	u32 curr_rateidx;
};

struct SwitchBandwidth_parm {
	u8 curr_bandwidth;
};

#endif

struct SetChannelPlan_param {
	const struct country_chplan *country_ent;
	u8 channel_plan;
};

struct LedBlink_param {
	PVOID pLed;
};

struct SetChannelSwitch_param {
	u8 new_ch_no;
};

struct TDLSoption_param {
	u8 addr[ETH_ALEN];
	u8 option;
};

struct RunInThread_param {
	void (*func) (void *);
	void *context;
};

#define GEN_CMD_CODE(cmd)	cmd ## _CMD_

#define wMBOX1_RSP_OFFSET			512

#define wMBOX1_SUCCESS			0x00
#define wMBOX1_SUCCESS_RSP			0x01
#define wMBOX1_DUPLICATED			0x02
#define wMBOX1_DROPPED			0x03
#define wMBOX1_PARAMETERS_ERROR		0x04
#define wMBOX1_REJECTED			0x05
#define wMBOX1_CMD_OVERFLOW		0x06
#define wMBOX1_RESERVED			0x07
#define wMBOX1_ENQ_HEAD			0x08
#define wMBOX1_ENQ_HEAD_FAIL		0x09

extern u8 proc_setassocsta_cmd_func(PNIC Nic, u8 * mac_addr);
extern u8 proc_setstandby_cmd_func(PNIC Nic, uint action);
u8 proc_sitesurvey_cmd_func(PNIC Nic, NDIS_802_11_SSID * ssid,
					  int ssid_num, struct wl_ieee80211_channel *ch,
					  int ch_num);

u8 proc_create_ibss_cmd_func(PNIC Nic, int flags);
u8 proc_startbss_cmd_func(PNIC Nic, int flags);
u8 proc_change_bss_chbw_cmd_func(PNIC Nic, int flags, u8 req_ch, u8 req_bw,
						   u8 req_offset);

extern u8 proc_setphy_cmd_func(PNIC Nic, u8 modem, u8 ch);

struct sta_info;
extern u8 proc_setstakey_cmd_func(PNIC Nic, struct sta_info *sta,
							u8 key_type, bool enqueue);
extern u8 proc_clearstakey_cmd_func(PNIC Nic, struct sta_info *sta,
							  u8 enqueue, u8 flag);

extern u8 proc_joinbss_cmd_func(PNIC Nic, struct wlan_network *pnetwork);
u8 proc_disassoc_cmd_func(PNIC Nic, u32 deauth_timeout_ms, bool enqueue);
extern u8 proc_setopmode_cmd_func(PNIC Nic,
							NDIS_802_11_NETWORK_INFRASTRUCTURE networktype,
							bool enqueue);
extern u8 proc_setdatarate_cmd_func(PNIC Nic, u8 * rateset, u8 flag);
extern u8 proc_setbasicrate_cmd_func(PNIC Nic, u8 * rateset);
extern void proc_usb_catc_trigger_cmd_func(PNIC Nic, const char *caller);
extern u8 proc_setbbreg_cmd_func(PNIC Nic, u8 offset, u8 val);
extern u8 proc_setrfreg_cmd_func(PNIC Nic, u8 offset, u32 val);
extern u8 proc_getbbreg_cmd_func(PNIC Nic, u8 offset, u8 * pval);
extern u8 proc_getrfreg_cmd_func(PNIC Nic, u8 offset, u8 * pval);
extern u8 wl_setrfintfs_cmd(PNIC Nic, u8 mode);
extern u8 proc_setrttbl_cmd_func(PNIC Nic,
						   struct setratable_parm *prate_table);
extern u8 proc_getrttbl_cmd_func(PNIC Nic, struct getratable_rsp *pval);

extern u8 wl_gettssi_cmd(PNIC Nic, u8 offset, u8 * pval);
extern u8 wl_setfwdig_cmd(PNIC Nic, u8 type);
extern u8 wl_setfwra_cmd(PNIC Nic, u8 type);

extern u8 proc_addbareq_cmd_func(PNIC Nic, u8 tid, u8 * addr, u8 flag);
extern u8 proc_addbarsp_cmd_func(PNIC Nic, u8 * addr, u16 tid, u8 status,
						   u8 size, u16 start_seq, u8 flag);
extern u8 proc_reset_securitypriv_cmd_func(PNIC Nic, u8 flag);
extern u8 proc_free_assoc_resources_cmd_func(PNIC Nic, u8 flag);
extern u8 proc_dynamic_chk_wk_cmd_func(PNIC Nic, u8 flag);

u8 proc_lps_ctrl_wk_cmd_func(PNIC Nic, u8 lps_ctrl_type, u8 enqueue);
u8 proc_dm_in_lps_wk_cmd_func(PNIC Nic);
u8 proc_lps_change_dtim_cmd_func(PNIC Nic, u8 dtim, u8 flag);

#if (RATE_ADAPTIVE_SUPPORT==1)
u8 proc_rpt_timer_cfg_cmd_func(PNIC Nic, u16 minRptTime);
#endif

u8 proc_dm_ra_mask_wk_cmd_func(PNIC Nic, u8 * psta, u8 flag);

extern u8 proc_ps_cmd_func(PNIC Nic, u8 flag);

#ifdef CONFIG_AP_MODE
u8 proc_chk_hi_queue_cmd_func(PNIC Nic, u8 flag);
#endif

u8 update_enable_hw_tsf_cmd_func(PNIC Nic, u8 flag);

u8 ch_cmd_to_set_func(PNIC Nic, u8 ch, u8 bw, u8 ch_offset, u8 enqueue,
				  u8 flag);

u8 proc_set_chplan_cmd_func(PNIC Nic, int flags, u8 chplan, u8 swconfig);
u8 country_cmd_to_set_func(PNIC Nic, int flags, const char *country_code,
					   u8 swconfig, u8 flag);

extern u8 proc_led_blink_cmd_func(PNIC Nic, PVOID pLed);
extern u8 csa_cmd_to_set_func(PNIC Nic, u8 new_ch_no);
extern u8 proc_tdls_cmd_func(PNIC Nic, const u8 * addr, u8 option);

u8 proc_mp_cmd_func(PNIC Nic, u8 mp_cmd_id, u8 flags);

extern u8 proc_wmbox0_packet_wk_cmd_func(PNIC Nic, u8 * pbuf, u16 length,
								u8 flag);
extern u8 proc_wmbox0_wk_cmd_func(PNIC Nic, u8 * wmbox0_evt, u8 flag);

u8 proc_run_in_thread_cmd_func(PNIC Nic, void (*func) (void *),
						 void *context);

u8 chk_session_tracker_cmd_func(PNIC Nic, struct sta_info *sta);
u8 add_session_tracker_cmd_func(PNIC Nic, struct sta_info *sta,
						   u8 * local_naddr, u8 * local_port, u8 * remote_naddr,
						   u8 * remote_port);
u8 del_session_tracker_cmd_func(PNIC Nic, struct sta_info *sta,
						   u8 * local_naddr, u8 * local_port, u8 * remote_naddr,
						   u8 * remote_port);

u8 proc_drvextra_cmd_func(PNIC Nic, unsigned char *pbuf);

extern void proc_survey_cmd_callback_func(PNIC Nic, struct cmd_obj *pcmd);
extern void callback_disassoc_cmd_func(PNIC Nic,
									  struct cmd_obj *pcmd);
extern void callback_joinbss_cmd_func(PNIC Nic, struct cmd_obj *pcmd);
void proc_create_ibss_post_func(PNIC Nic, int status, u8 flag);
extern void callback_getbbrfreg_cmdrsp_callback_func(PNIC Nic,
										   struct cmd_obj *pcmd);
extern void callback_readtssi_cmdrsp_func(PNIC Nic,
										 struct cmd_obj *pcmd);

extern void callback_setstaKey_cmdrsp_func(PNIC Nic,
										  struct cmd_obj *pcmd);
extern void callback_setassocsta_cmdrsp_func(PNIC Nic,
											struct cmd_obj *pcmd);
extern void wl_getrttbl_cmdrsp_callback(PNIC Nic,
										 struct cmd_obj *pcmd);
extern void callback_getmacreg_cmdrsp_func(PNIC Nic,
										  struct cmd_obj *pcmd);

struct _cmd_callback {
	u32 cmd_code;
	void (*callback) (PNIC Nic, struct cmd_obj * cmd);
};

enum wl_wmbox1_cmd {
	GEN_CMD_CODE(_Read_MACREG),
	GEN_CMD_CODE(_Write_MACREG),
	GEN_CMD_CODE(_Read_BBREG),
	GEN_CMD_CODE(_Write_BBREG),
	GEN_CMD_CODE(_Read_RFREG),
	GEN_CMD_CODE(_Write_RFREG),
	GEN_CMD_CODE(_Read_EEPROM),
	GEN_CMD_CODE(_Write_EEPROM),
	GEN_CMD_CODE(_Read_EFUSE),
	GEN_CMD_CODE(_Write_EFUSE),

	GEN_CMD_CODE(_Read_CAM),
	GEN_CMD_CODE(_Write_CAM),
	GEN_CMD_CODE(_setBCNITV),
	GEN_CMD_CODE(_setMBIDCFG),
	GEN_CMD_CODE(_JoinBss),
	GEN_CMD_CODE(_DisConnect),
	GEN_CMD_CODE(_CreateBss),
	GEN_CMD_CODE(_SetOpMode),
	GEN_CMD_CODE(_SiteSurvey),
	GEN_CMD_CODE(_SetAuth),

	GEN_CMD_CODE(_SetKey),
	GEN_CMD_CODE(_SetStaKey),
	GEN_CMD_CODE(_SetAssocSta),
	GEN_CMD_CODE(_DelAssocSta),
	GEN_CMD_CODE(_SetStaPwrState),
	GEN_CMD_CODE(_SetBasicRate),
	GEN_CMD_CODE(_GetBasicRate),
	GEN_CMD_CODE(_SetDataRate),
	GEN_CMD_CODE(_GetDataRate),
	GEN_CMD_CODE(_SetPhyInfo),

	GEN_CMD_CODE(_GetPhyInfo),
	GEN_CMD_CODE(_SetPhy),
	GEN_CMD_CODE(_GetPhy),
	GEN_CMD_CODE(_readRssi),
	GEN_CMD_CODE(_readGain),
	GEN_CMD_CODE(_SetAtim),
	GEN_CMD_CODE(_SetPwrMode),
	GEN_CMD_CODE(_JoinbssRpt),
	GEN_CMD_CODE(_SetRaTable),
	GEN_CMD_CODE(_GetRaTable),

	GEN_CMD_CODE(_GetCCXReport),
	GEN_CMD_CODE(_GetDTMReport),
	GEN_CMD_CODE(_GetTXRateStatistics),
	GEN_CMD_CODE(Func_Set_Usb_Suspend),
	GEN_CMD_CODE(_SetH2cLbk),
	GEN_CMD_CODE(_AddBAReq),
	GEN_CMD_CODE(_SetChannel),
	GEN_CMD_CODE(_SetTxPower),
	GEN_CMD_CODE(_SwitchAntenna),
	GEN_CMD_CODE(_SetCrystalCap),
	GEN_CMD_CODE(_SetSingleCarrierTx),

	GEN_CMD_CODE(_SetSingleToneTx),
	GEN_CMD_CODE(_SetCarrierSuppressionTx),
	GEN_CMD_CODE(_SetContinuousTx),
	GEN_CMD_CODE(_SwitchBandwidth),
	GEN_CMD_CODE(_TX_Beacon),

	GEN_CMD_CODE(_Set_MLME_EVT),
	GEN_CMD_CODE(_Set_Drv_Extra),
	GEN_CMD_CODE(_Set_wMBOX1_MSG),

	GEN_CMD_CODE(_SetChannelPlan),
	GEN_CMD_CODE(_LedBlink),

	GEN_CMD_CODE(_SetChannelSwitch),
	GEN_CMD_CODE(_TDLS),
	GEN_CMD_CODE(_ChkBMCSleepq),

	GEN_CMD_CODE(_RunInThreadCMD),
	GEN_CMD_CODE(_AddBARsp),

	GEN_CMD_CODE(_PHYSTATUS),

	GEN_CMD_CODE(_SET_MSG_VAR),

	MAX_wMBOX1CMD
};

#define _GetMACReg_CMD_ _Read_MACREG_CMD_
#define _SetMACReg_CMD_ _Write_MACREG_CMD_
#define _GetBBReg_CMD_		_Read_BBREG_CMD_
#define _SetBBReg_CMD_ 		_Write_BBREG_CMD_
#define _GetRFReg_CMD_ 		_Read_RFREG_CMD_
#define _SetRFReg_CMD_ 		_Write_RFREG_CMD_

#ifdef _WL_CMD_C_
struct _cmd_callback wl_cmd_callback[] = {
	{GEN_CMD_CODE(_Read_MACREG), &callback_getmacreg_cmdrsp_func},
	{GEN_CMD_CODE(_Write_MACREG), NULL},
	{GEN_CMD_CODE(_Read_BBREG), &callback_getbbrfreg_cmdrsp_callback_func},
	{GEN_CMD_CODE(_Write_BBREG), NULL},
	{GEN_CMD_CODE(_Read_RFREG), &callback_getbbrfreg_cmdrsp_callback_func},
	{GEN_CMD_CODE(_Write_RFREG), NULL},
	{GEN_CMD_CODE(_Read_EEPROM), NULL},
	{GEN_CMD_CODE(_Write_EEPROM), NULL},
	{GEN_CMD_CODE(_Read_EFUSE), NULL},
	{GEN_CMD_CODE(_Write_EFUSE), NULL},

	{GEN_CMD_CODE(_Read_CAM), NULL},
	{GEN_CMD_CODE(_Write_CAM), NULL},
	{GEN_CMD_CODE(_setBCNITV), NULL},
	{GEN_CMD_CODE(_setMBIDCFG), NULL},
	{GEN_CMD_CODE(_JoinBss), &callback_joinbss_cmd_func},
	{GEN_CMD_CODE(_DisConnect), &callback_disassoc_cmd_func},
	{GEN_CMD_CODE(_CreateBss), NULL},
	{GEN_CMD_CODE(_SetOpMode), NULL},
	{GEN_CMD_CODE(_SiteSurvey), &proc_survey_cmd_callback_func},
	{GEN_CMD_CODE(_SetAuth), NULL},

	{GEN_CMD_CODE(_SetKey), NULL},
	{GEN_CMD_CODE(_SetStaKey), &callback_setstaKey_cmdrsp_func},
	{GEN_CMD_CODE(_SetAssocSta), &callback_setassocsta_cmdrsp_func},
	{GEN_CMD_CODE(_DelAssocSta), NULL},
	{GEN_CMD_CODE(_SetStaPwrState), NULL},
	{GEN_CMD_CODE(_SetBasicRate), NULL},
	{GEN_CMD_CODE(_GetBasicRate), NULL},
	{GEN_CMD_CODE(_SetDataRate), NULL},
	{GEN_CMD_CODE(_GetDataRate), NULL},
	{GEN_CMD_CODE(_SetPhyInfo), NULL},

	{GEN_CMD_CODE(_GetPhyInfo), NULL},
	{GEN_CMD_CODE(_SetPhy), NULL},
	{GEN_CMD_CODE(_GetPhy), NULL},
	{GEN_CMD_CODE(_readRssi), NULL},
	{GEN_CMD_CODE(_readGain), NULL},
	{GEN_CMD_CODE(_SetAtim), NULL},
	{GEN_CMD_CODE(_SetPwrMode), NULL},
	{GEN_CMD_CODE(_JoinbssRpt), NULL},
	{GEN_CMD_CODE(_SetRaTable), NULL},
	{GEN_CMD_CODE(_GetRaTable), NULL},

	{GEN_CMD_CODE(_GetCCXReport), NULL},
	{GEN_CMD_CODE(_GetDTMReport), NULL},
	{GEN_CMD_CODE(_GetTXRateStatistics), NULL},
	{GEN_CMD_CODE(Func_Set_Usb_Suspend), NULL},
	{GEN_CMD_CODE(_SetH2cLbk), NULL},
	{GEN_CMD_CODE(_AddBAReq), NULL},
	{GEN_CMD_CODE(_SetChannel), NULL},
	{GEN_CMD_CODE(_SetTxPower), NULL},
	{GEN_CMD_CODE(_SwitchAntenna), NULL},
	{GEN_CMD_CODE(_SetCrystalCap), NULL},
	{GEN_CMD_CODE(_SetSingleCarrierTx), NULL},

	{GEN_CMD_CODE(_SetSingleToneTx), NULL},
	{GEN_CMD_CODE(_SetCarrierSuppressionTx), NULL},
	{GEN_CMD_CODE(_SetContinuousTx), NULL},
	{GEN_CMD_CODE(_SwitchBandwidth), NULL},
	{GEN_CMD_CODE(_TX_Beacon), NULL},

	{GEN_CMD_CODE(_Set_MLME_EVT), NULL},
	{GEN_CMD_CODE(_Set_Drv_Extra), NULL},
	{GEN_CMD_CODE(_Set_wMBOX1_MSG), NULL},
	{GEN_CMD_CODE(_SetChannelPlan), NULL},
	{GEN_CMD_CODE(_LedBlink), NULL},

	{GEN_CMD_CODE(_SetChannelSwitch), NULL},
	{GEN_CMD_CODE(_TDLS), NULL},
	{GEN_CMD_CODE(_ChkBMCSleepq), NULL},

	{GEN_CMD_CODE(_RunInThreadCMD), NULL},
	{GEN_CMD_CODE(_AddBARsp), NULL},
};
#endif

#define CMD_FMT "cmd=%d,%d,%d"
#define CMD_ARG(cmd) \
	(cmd)->cmdcode, \
	(cmd)->cmdcode == GEN_CMD_CODE(_Set_Drv_Extra) ? ((struct drvextra_cmd_parm *)(cmd)->parmbuf)->ec_id : ((cmd)->cmdcode == GEN_CMD_CODE(_Set_MLME_EVT) ? ((struct wMBOX0Event_Header *)(cmd)->parmbuf)->ID : 0), \
	(cmd)->cmdcode == GEN_CMD_CODE(_Set_Drv_Extra) ? ((struct drvextra_cmd_parm *)(cmd)->parmbuf)->type : 0

#endif
