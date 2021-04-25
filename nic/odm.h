#ifndef __ODM_H__
#define __ODM_H__

#define ONE_KEY_VERSION 0
#define CHIP51_DIRECT_ACCESS 0
/*mbox1*/
#define wMBOX1_MACID_CFG_LEN            7
#define wMBOX1_MEDIA_STATUS_RPT_LEN     3

/*chip 51 reg */
#define REG_FMETHR                      0x01C8
#define REG_HMETFR                      0x01CC

#define REG_HMEBOX_0                    0x01D0
#define REG_HMEBOX_1                    0x01D4
#define REG_HMEBOX_2                    0x01D8
#define REG_HMEBOX_3                    0x01DC

#define REG_HMEBOX_EXT0_8188F           0x01F0
#define REG_HMEBOX_EXT1_8188F           0x01F4
#define REG_HMEBOX_EXT2_8188F           0x01F8
#define REG_HMEBOX_EXT3_8188F           0x01FC

/**/
#define RTL8188F_MAX_CMD_LEN            7
#define RTL8188F_EX_MESSAGE_BOX_SIZE    4
#define MESSAGE_BOX_SIZE                4
#define MAX_H2C_BOX_NUMS                4







/*macid config*/
#define SET_9086X_wMBOX1CMD_MACID_CFG_MACID(__pwMBOX1Cmd, __Value)      SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RAID(__pwMBOX1Cmd, __Value)       SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 0, 5, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_SGI_EN(__pwMBOX1Cmd, __Value)     SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+1, 7, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_BW(__pwMBOX1Cmd, __Value)     SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 0, 2, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_NO_UPDATE(__pwMBOX1Cmd, __Value)      SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 3, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_DISPT(__pwMBOX1Cmd, __Value)      SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 6, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_DISRA(__pwMBOX1Cmd, __Value)      SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+2, 7, 1, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK0(__pwMBOX1Cmd, __Value)     SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+3, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK1(__pwMBOX1Cmd, __Value)     SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+4, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK2(__pwMBOX1Cmd, __Value)     SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+5, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_MACID_CFG_RATE_MASK3(__pwMBOX1Cmd, __Value)     SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd+6, 0, 8, __Value)


/*MEDIA STATUS*/
#define SET_wMBOX1CMD_MSRRPT_PARM_OPMODE(__pwMBOX1Cmd, __Value)         SET_BITS_TO_LE_1BYTE(((wf_u8 *)(__pwMBOX1Cmd)), 0, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MACID_IND(__pwMBOX1Cmd, __Value)      SET_BITS_TO_LE_1BYTE(((wf_u8 *)(__pwMBOX1Cmd)), 1, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MIRACAST(__pwMBOX1Cmd, __Value)           SET_BITS_TO_LE_1BYTE(((wf_u8 *)(__pwMBOX1Cmd)), 2, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MIRACAST_SINK(__pwMBOX1Cmd, __Value)  SET_BITS_TO_LE_1BYTE(((wf_u8 *)(__pwMBOX1Cmd)), 3, 1, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_ROLE(__pwMBOX1Cmd, __Value)               SET_BITS_TO_LE_1BYTE(((wf_u8 *)(__pwMBOX1Cmd)), 4, 4, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MACID(__pwMBOX1Cmd, __Value)          SET_BITS_TO_LE_1BYTE(((wf_u8 *)(__pwMBOX1Cmd)) + 1, 0, 8, (__Value))
#define SET_wMBOX1CMD_MSRRPT_PARM_MACID_END(__pwMBOX1Cmd, __Value)      SET_BITS_TO_LE_1BYTE(((wf_u8 *)(__pwMBOX1Cmd)) + 2, 0, 8, (__Value))

/*role*/
#define wMBOX1_MSR_ROLE_RSVD    0
#define wMBOX1_MSR_ROLE_STA     1
#define wMBOX1_MSR_ROLE_AP      2
#define wMBOX1_MSR_ROLE_GC      3
#define wMBOX1_MSR_ROLE_GO      4
#define wMBOX1_MSR_ROLE_ADHOC   6
#define wMBOX1_MSR_ROLE_MAX     7

typedef enum _HAL_MSG_VARIABLE 
{
    HAL_MSG_STA_INFO,
    HAL_MSG_P2P_STATE,
    HAL_MSG_WIFI_DISPLAY_STATE,
    HAL_MSG_NOISE_MONITOR,
    HAL_MSG_REGULATION,
    HAL_MSG_INITIAL_GAIN,
    HAL_MSG_FA_CNT_DUMP,
    HAL_MSG_DBG_FLAG,
    HAL_MSG_DBG_LEVEL,
    HAL_MSG_RX_INFO_DUMP,
#ifdef CONFIG_AUTO_CHNL_SEL_NHM
    HAL_MSG_AUTO_CHNL_SEL,
#endif
} ODM_MSG_VARIABLE;


enum wmbox1_cmd_9086X 
{
    wMBOX1_9086X_RSVD_PAGE = 0x00,
    wMBOX1_9086X_MEDIA_STATUS_RPT = 0x01, //use
    wMBOX1_9086X_SCAN_ENABLE = 0x02,
    wMBOX1_9086X_KEEP_ALIVE = 0x03,
    wMBOX1_9086X_DISCON_DECISION = 0x04,
    wMBOX1_9086X_PSD_OFFLOAD = 0x05,
    wMBOX1_9086X_AP_OFFLOAD = 0x08,
    wMBOX1_9086X_BCN_RSVDPAGE = 0x09,
    wMBOX1_9086X_PROBERSP_RSVDPAGE = 0x0A,
    wMBOX1_9086X_FCS_RSVDPAGE = 0x10,
    wMBOX1_9086X_FCS_INFO = 0x11,
    wMBOX1_9086X_AP_WOW_GPIO_CTRL = 0x13,

    wMBOX1_9086X_SET_PWR_MODE = 0x20,
    wMBOX1_9086X_PS_TUNING_PARA = 0x21,
    wMBOX1_9086X_PS_TUNING_PARA2 = 0x22,
    wMBOX1_9086X_P2P_LPS_PARAM = 0x23,
    wMBOX1_9086X_P2P_PS_OFFLOAD = 0x24,
    wMBOX1_9086X_PS_SCAN_ENABLE = 0x25,
    wMBOX1_9086X_SAP_PS_ = 0x26,
    wMBOX1_9086X_INACTIVE_PS_ = 0x27,
    wMBOX1_9086X_FWLPS_IN_IPS_ = 0x28,

    wMBOX1_9086X_MACID_CFG = 0x40, //use
    wMBOX1_9086X_TXBF = 0x41,
    wMBOX1_9086X_RSSI_SETTING = 0x42,
    wMBOX1_9086X_AP_REQ_TXRPT = 0x43,
    wMBOX1_9086X_INIT_RATE_COLLECT = 0x44,
    wMBOX1_9086X_RA_PARA_ADJUST = 0x46,

    wMBOX1_9086X_B_TYPE_TDMA = 0x60,
    wMBOX1_9086X_BT_INFO = 0x61,
    wMBOX1_9086X_FORCE_BT_TXPWR = 0x62,
    wMBOX1_9086X_BT_IGNORE_WLANACT = 0x63,
    wMBOX1_9086X_DAC_SWING_VALUE = 0x64,
    wMBOX1_9086X_ANT_SEL_RSV = 0x65,
    wMBOX1_9086X_WL_OPMODE = 0x66,
    wMBOX1_9086X_BT_MP_OPER = 0x67,
    wMBOX1_9086X_BT_CONTROL = 0x68,
    wMBOX1_9086X_BT_WIFI_CTRL = 0x69,
    wMBOX1_9086X_BT_FW_PATCH = 0x6A,
    wMBOX1_9086X_BT_WLAN_CALIBRATION = 0x6D,

    wMBOX1_9086X_WOWLAN = 0x80,
    wMBOX1_9086X_REMOTE_WAKE_CTRL = 0x81,
    wMBOX1_9086X_AOAC_GLOBAL_INFO = 0x82,
    wMBOX1_9086X_AOAC_RSVD_PAGE = 0x83,
    wMBOX1_9086X_AOAC_RSVD_PAGE2 = 0x84,
    wMBOX1_9086X_D0_SCAN_OFFLOAD_CTRL = 0x85,
    wMBOX1_9086X_D0_SCAN_OFFLOAD_INFO = 0x86,
    wMBOX1_9086X_CHNL_SWITCH_OFFLOAD = 0x87,
    wMBOX1_9086X_P2P_OFFLOAD_RSVD_PAGE = 0x8A,
    wMBOX1_9086X_P2P_OFFLOAD = 0x8B,

    wMBOX1_9086X_RESET_TSF = 0xC0,
    wMBOX1_9086X_MAXID,
};



/*phy status*/
typedef struct
{
    wf_u8 gain:7, trsw:1;

} recv_agc_st;

typedef struct
{
    recv_agc_st path_agc[2];
    wf_u8 ch_corr[2];
    wf_u8 cck_sig_qual_ofdm_pwdb_all;
    wf_u8 cck_agc_rpt_ofdm_cfosho_a;
    wf_u8 cck_rpt_b_ofdm_cfosho_b;
    wf_u8 rsvd_1;
    wf_u8 noise_power_db_msb;
    wf_s8 path_cfotail[2];
    wf_u8 pcts_mask[2];
    wf_s8 stream_rxevm[2];
    wf_u8 path_rxsnr[2];
    wf_u8 noise_power_db_lsb;
    wf_u8 rsvd_2[3];
    wf_u8 stream_csi[2];
    wf_u8 stream_target_csi[2];
    wf_s8 sig_evm;
    wf_u8 rsvd_3;
    wf_u8 antsel_rx_keep_2:1;
    wf_u8 sgi_en:1;
    wf_u8 rxsc:2;
    wf_u8 idle_long:1;
    wf_u8 r_ant_train_en:1;
    wf_u8 ant_sel_b:1;
    wf_u8 ant_sel:1;

} recv_phy_status_st;


/*host to m0 odm phystatus packet*/
typedef struct odm_h2mcu_phystatus_st_ {
    wf_u8 DataRate;
    wf_u8 StationID;
    wf_u8 bPacketMatchBSSID;
    wf_u8 bPacketToSelf;
    wf_u8 bPacketBeacon;
    wf_u8 bToSelf;
    wf_u8 cck_agc_rpt_ofdm_cfosho_a;
    wf_u8 cck_sig_qual_ofdm_pwdb_all;
    wf_u8 gain;
    wf_u8 path_rxsnr;
    wf_s8 stream_rxevm;
    wf_s8 path_cfotail;
    wf_u32 bcn_cnt;
    wf_u32 rsvd;
    //wf_u8 Scaninfo_bssid[WF_ETH_ALEN];
} odm_h2mcu_phystatus_st;

typedef struct odm_watchdog_param_st_
{
    wf_u8 reserved;
}odm_watchdog_param_st;

typedef struct phy_cali_ {
    wf_u8 TxPowerTrackControl;
    wf_s8 Remnant_CCKSwingIdx;
    wf_s8 Remnant_OFDMSwingIdx;
    wf_u8 rsvd;
} phy_cali_t;

struct odm_ht_priv {
    wf_u8 ht_option;
    wf_u8 ampdu_enable;
    wf_u8 tx_amsdu_enable;
    wf_u8 bss_coexist;

    wf_u32 tx_amsdu_maxlen;
    wf_u32 rx_ampdu_maxlen;

    wf_u8 rx_ampdu_min_spacing;

    wf_u8 ch_offset;
    wf_u8 sgi_20m;
    wf_u8 sgi_40m;

    wf_u8 agg_enable_bitmap;
    wf_u8 candidate_tid_bitmap;

    wf_u8 ldpc_cap;
    wf_u8 stbc_cap;
    wf_u8 smps_cap;
    wf_80211_mgmt_ht_cap_t ht_cap;

};

typedef struct odm_wdn_info_st_ {
    wf_u8  bUsed;
    wf_u32 mac_id;
    wf_u8  hwaddr[MAC_ADDR_LEN];
    wf_u8  ra_rpt_linked;
    wf_u8  wireless_mode;
    wf_u8  rssi_level;
    wf_u8  ra_change;
    struct odm_ht_priv htpriv;
} odm_wdn_info_st;


/*sync to mcu, odm msg*/
typedef struct odm_msg_st_ {
    wf_u64 tx_bytes;
    wf_u64 rx_bytes;
    wf_u32 cur_wireless_mode;
    wf_u32 CurrentBandType;
    wf_u32 ForcedDataRate;
    wf_u32 nCur40MhzPrimeSC;
    wf_u32 dot11PrivacyAlgrthm;
    wf_u32 CurrentChannelBW;
    wf_u32 CurrentChannel;
    wf_u32 net_closed;
    wf_u32 u1ForcedIgiLb;
    wf_u32 bScanInProcess;
    wf_u32 bpower_saving;
    wf_u32 traffic_stat_cur_tx_tp;
    wf_u32 traffic_stat_cur_rx_tp;
    wf_u32 msgWdgStateVal;
    wf_u32 ability;
    wf_u32 Rssi_Min;
    wf_u32 dig_CurIGValue;
    wf_u32 wifi_direct;
    wf_u32 wifi_display;
    wf_u64 dbg_cmp;
    wf_u32 dbg_level;
    wf_u32 PhyRegPgVersion;
    wf_u32 PhyRegPgValueType;
    phy_cali_t phy_cali;
    wf_u32 bDisablePowerTraining;
    wf_u32 fw_state;
    wf_u32 sta_count;
} odm_msg_st;

typedef struct
{
    wf_u8 *buf;
    wf_u32 buf_len;
}odm_c2h_comm_st;

typedef union
{
    odm_h2mcu_phystatus_st odm_phystatus;
}odm_mcu_cmd_param_u;

typedef struct
{
    wf_que_list node;
    odm_mcu_cmd_param_u param;
    int (*cmd_func) (nic_info_st *nic_info, odm_mcu_cmd_param_u *param );
}odm_mcu_cmd_st;

typedef struct wf_odm_phy_info_
{
    wf_u32 cck_agc_rpt_ofdm_cfosho_a;
    wf_u32 cck_sig_qual_ofdm_pwdb_all;
    wf_u32 gain;
    wf_u32 path_rxsnr;
    wf_s32 stream_rxevm;
    wf_s32 path_cfotail;
    phy_status_st phy_status;
    wf_u8 raw_phystatus[32];
    wf_s32 rx_rate;
    wf_u8  is_cck_rate;
    wf_u8 cck_lna_index;
    wf_u8 cck_vga_index;
    wf_u8 rx_pwdb_all;
    wf_u8  SignalQuality;                  /* in 0-100 index. */
    wf_s8  RxMIMOSignalQuality[4];       /* per-path's EVM */
    wf_u8  RxMIMOEVMdbm[4];                /* per-path's EVM dbm */
    wf_u8 RxMIMOSignalStrength[4];         /* in 0~100 index */
    short  Cfo_short[4];                /* per-path's Cfo_short */
    short  Cfo_tail[4];                 /* per-path's Cfo_tail */
    wf_s8    RxPower;                    /* in dBm Translate from PWdB */
    wf_s8    RecvSignalPower;            /* Real power in dBm for this packet, no beautification and aggregation. Keep this raw info to be used for the other procedures. */
    wf_u8 BTRxRSSIPercentage;
    wf_u8      SignalStrength;             /* in 0-100 index. */
    wf_s8    RxPwr[4];                   /* per-path's pwdb */
    wf_s8    RxSNR[4];                   /* per-path's SNR   */
}odm_phy_info_st;
#define ODM_WDN_INFO_SIZE  (32)
#define ODM_NAME_LEN    (32)
typedef struct odm_mgnt_st_
{
    void *nic_info;
    odm_msg_st odm_msg;
    odm_wdn_info_st wdn[ODM_WDN_INFO_SIZE];
    odm_phy_info_st phy[ODM_WDN_INFO_SIZE];
    wf_bool    phystatus_enable;
    wf_s32 send_flag_val;
    wf_u32 wdg_exec_cnt;
    wf_u32 bcn_cnt;
    wf_u32 last_bcn_cnt;
    
    wf_u32 backup_ability;
    wf_u8 LastHMEBoxNum;
    wf_lock_mutex chip_op_mutex;
    wf_que_t  odm_queue; //mcu cmd handle queue
    wf_os_api_sema_t   odm_sema;

    wf_s8 odm_name[ODM_NAME_LEN];
    void *odm_tid;
    wf_os_api_timer_t odm_wdg_timer;
}odm_mgnt_st;


typedef enum ODM_ABILITY_OPS_ {
    ODM_DIS_ALL_FUNC,
    ODM_FUNC_SET,
    ODM_FUNC_CLR,
    ODM_FUNC_BACKUP,
    ODM_FUNC_RESTORE,
} ODM_ABILITY_OPS;

typedef enum ODM_SUPPORT_ABILITY_ENUM_
{
    ODM_BB_DIG = WF_BIT(0),
    ODM_BB_RA_MASK = WF_BIT(1),
    ODM_BB_DYNAMIC_TXPWR = WF_BIT(2), //use
    ODM_BB_FA_CNT = WF_BIT(3),
    ODM_BB_RSSI_MONITOR = WF_BIT(4),
    ODM_BB_CCK_PD = WF_BIT(5),
    ODM_BB_ANT_DIV = WF_BIT(6),
    ODM_BB_PWR_SAVE = WF_BIT(7),
    ODM_BB_PWR_TRAIN = WF_BIT(8),
    ODM_BB_RATE_ADAPTIVE = WF_BIT(9),
    ODM_BB_PATH_DIV = WF_BIT(10),
    ODM_BB_PSD = WF_BIT(11),
    ODM_BB_RXHP = WF_BIT(12),
    ODM_BB_ADAPTIVITY = WF_BIT(13),
    ODM_BB_CFO_TRACKING = WF_BIT(14),
    ODM_BB_NHM_CNT = WF_BIT(15),   //use
    ODM_BB_PRIMARY_CCA = WF_BIT(16),
    ODM_BB_TXBF = WF_BIT(17),

    ODM_MAC_EDCA_TURBO = WF_BIT(20),
    ODM_MAC_EARLY_MODE = WF_BIT(21),

    ODM_RF_TX_PWR_TRACK = WF_BIT(24),
    ODM_RF_RX_GAIN_TRACK = WF_BIT(25),
    ODM_RF_CALIBRATION = WF_BIT(26),

} ODM_SUPPORT_ABILITY_ENUM;


wf_s32 wf_set_odm_default(nic_info_st *nic_info);
wf_s32 wf_set_odm_init(nic_info_st * nic_info);
wf_s32 wf_get_odm_msg(nic_info_st *nic_info);
wf_s32 wf_set_odm_msg(nic_info_st *nic_info);
wf_s32 wf_set_odm_dig(nic_info_st *nic_info, wf_bool init_gain, wf_u32 rx_gain);
wf_s32 wf_odm_sync_msg(nic_info_st *nic_info,ODM_MSG_VARIABLE ops,  wf_u32 val);

wf_s32 wf_odm_calc_str_and_qual(nic_info_st *nic_info, wf_u8 *phystatus,  wf_u8 *mac_frame, void *prx_pkt);
wf_s32 wf_odm_set_ability(nic_info_st *nic_info,ODM_ABILITY_OPS ops,  wf_u32 ability);
wf_s32 wf_odm_set_rfconfig(nic_info_st *nic_info, wf_u8 mac_id, wf_u8 raid, wf_u8 bw, wf_u8 sgi, wf_u32 mask);
wf_s32 wf_odm_update_wdn_info(nic_info_st *nic_info);
wf_s32 wf_odm_disconnect_media_status(nic_info_st *nic_info,wdn_net_info_st *wdn_net_info);
wf_s32 wf_odm_connect_media_status(nic_info_st *nic_info,wdn_net_info_st *wdn_net_info);
wf_s32 FillH2CCmd(nic_info_st *nic_info, wf_u8 ElementID, wf_u32 CmdLen, wf_u8 *pCmdBuffer);
wf_s32 wf_mcu_odm_init_msg(nic_info_st *nic_info, odm_msg_st *msg);
wf_s32 wf_mcu_handle_bb_lccalibrate(nic_info_st *nic_info);
wf_s32 wf_mcu_handle_bb_iq_calibrate(nic_info_st *nic_info, wf_u8 channel);
wf_s32 wf_mcu_get_odm_wdn_info(nic_info_st *nic_info, wf_u32 wdn_id);
wf_s32 wf_mcu_set_odm_wdn_info(nic_info_st *nic_info, wf_u16 wdn_id);
wf_s32 wf_mcu_get_rate_bitmap(nic_info_st *nic_info,wdn_net_info_st *wdn_net_info,wf_u32 *rate_bitmap);
#ifdef CFG_ENABLE_AP_MODE
wf_s32 wf_ap_odm_connect_media_status(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info);
wf_s32 wf_ap_odm_disconnect_media_status(nic_info_st *pnic_info,wdn_net_info_st *pwdn_info);
#endif
wf_s32 wf_odm_update(nic_info_st *nic_info, wdn_net_info_st *wdn_net_info);

wf_s32 wf_odm_mgnt_init(nic_info_st *nic_info);
wf_s32 wf_odm_mgnt_term(nic_info_st *nic_info);

wf_s32 signal_scale_mapping(int current_sig);
wf_s32 translate_percentage_to_dbm(wf_u32 SignalStrengthIndex);
void wf_odm_handle_phystatus(nic_info_st *nic_info, recv_phy_status_st *rps, wf_u8 *pbuf,prx_pkt_t ppt);

#endif
