#ifndef __LPS_H__
#define __LPS_H__

#ifdef CONFIG_LPS
/***************************************************************
    Define
***************************************************************/
#define USE_M0_LPS_INTERFACES      (1)

#define _SSID_IE_                  (0)
#define _SUPPORTEDRATES_IE_        (1)
#define _DSSET_IE_                 (3)
#define _EXT_SUPPORTEDRATES_IE_    (50)

#define ETH_ADDRESS_LEN            (6)
#define ETH_HEADER_LEN             (14)

#define PS_DPS                     BIT(0)
#define PS_LCLK                    (PS_DPS)
#define PS_RF_OFF                  BIT(1)
#define PS_ALL_ON                  BIT(2)
#define PS_ST_ACTIVE               BIT(3)

#define PS_ISR_ENABLE              BIT(4)
#define PS_IMR_ENABLE              BIT(5)
#define PS_ACK                     BIT(6)
#define PS_TOGGLE                  BIT(7)

#define PS_STATE_MASK              (0x0F)
#define PS_STATE_HW_MASK           (0x07)
#define PS_SEQ_MASK                (0xc0)

#define PS_STATE(x)                (PS_STATE_MASK & (x))
#define PS_STATE_HW(x)             (PS_STATE_HW_MASK & (x))
#define PS_SEQ(x)                  (PS_SEQ_MASK & (x))

#define PS_STATE_S0                (PS_DPS)
#define PS_STATE_S1                (PS_LCLK)
#define PS_STATE_S2                (PS_RF_OFF)
#define PS_STATE_S3                (PS_ALL_ON)
#define PS_STATE_S4                ((PS_ST_ACTIVE) | (PS_ALL_ON))

#define PS_IS_RF_ON(x)             ((x) & (PS_ALL_ON))
#define PS_IS_ACTIVE(x)            ((x) & (PS_ST_ACTIVE))
#define CLR_PS_STATE(x)            ((x) = ((x) & (0xF0)))

#define LPS_DELAY_TIME             1*HZ

#define REG_RCR                    0x0608

#define BCNQ_PAGE_NUM_9086X        0x08
#ifdef CONFIG_CONCURRENT_MODE
#define BCNQ1_PAGE_NUM_9086X       0x08
#else
#define BCNQ1_PAGE_NUM_9086X       0x00
#endif
#ifdef CONFIG_WOWLAN
#define WOWLAN_PAGE_NUM_9086X      0x07
#else
#define WOWLAN_PAGE_NUM_9086X      0x00
#endif

#define TX_TOTAL_PAGE_NUMBER_9086X      (0xFF - BCNQ_PAGE_NUM_9086X - BCNQ1_PAGE_NUM_9086X - WOWLAN_PAGE_NUM_9086X)
#define TX_PAGE_BOUNDARY_9086X          (TX_TOTAL_PAGE_NUMBER_9086X + 1)

#define PageNum(_Len, _Size)            (u32)(((_Len)/(_Size)) + ((_Len)&((_Size) - 1) ? 1:0))

#define wMBOX1_PWRMODE_LEN                                                  (7)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_PROBE_RSP(__pwMBOX1Cmd, __Value)         SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_PSPOLL(__pwMBOX1Cmd, __Value)            SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_NULL_DATA(__pwMBOX1Cmd, __Value)         SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_QOS_NULL_DATA(__pwMBOX1Cmd, __Value)     SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_BT_QOS_NULL_DATA(__pwMBOX1Cmd, __Value)  SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+4, 0, 8, __Value)

#define wMBOX1_RSVDPAGE_LOC_LEN                                                         (5)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_MODE(__pwMBOX1Cmd, __Value)                    SET_BITS_TO_LE_1BYTE(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_RLBM(__pwMBOX1Cmd, __Value)                    SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 0, 4, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_SMART_PS(__pwMBOX1Cmd, __Value)                SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+1, 4, 4, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_BCN_PASS_TIME(__pwMBOX1Cmd, __Value)           SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_ALL_QUEUE_UAPSD(__pwMBOX1Cmd, __Value)         SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_BCN_EARLY_wMBOX0_RPT(__pwMBOX1Cmd, __Value)    SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+3, 2, 1, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_PWR_STATE(__pwMBOX1Cmd, __Value)               SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+4, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_BYTE5(__pwMBOX1Cmd, __Value)                   SET_BITS_TO_LE_1BYTE((__pwMBOX1Cmd)+5, 0, 8, __Value)
#define GET_9086X_wMBOX1CMD_PWRMODE_PARM_MODE(__pwMBOX1Cmd)                             LE_BITS_TO_1BYTE(__pwMBOX1Cmd, 0, 8)

/***************************************************************
    Typedef
***************************************************************/
typedef struct _NDIS_802_11_FIXED_IEs {
    u8 Timestamp[8];
    u16 BeaconInterval;
    u16 Capabilities;
} NDIS_802_11_FIXED_IEs, *PNDIS_802_11_FIXED_IEs;


/* Design for pwrctrl_priv.ips_deny, 32 bits for 32 reasons at most */
typedef enum _PS_DENY_REASON {
    PS_DENY_DRV_INITIAL = 0,
    PS_DENY_SCAN,
    PS_DENY_JOIN,
    PS_DENY_DISCONNECT,
    PS_DENY_SUSPEND,
    PS_DENY_IOCTL,
    PS_DENY_MGNT_TX,
    PS_DENY_MONITOR_MODE,
    PS_DENY_BEAMFORMING,        /* Beamforming */
    PS_DENY_DRV_REMOVE = 30,
    PS_DENY_OTHERS = 31
} PS_DENY_REASON;

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
    LPS_CTRL_NO_LINKED = 11,
    LPS_CTRL_MAX = 12
};

enum Power_Mgnt {
    PS_MODE_ACTIVE = 0,
    PS_MODE_MIN,
    PS_MODE_MAX,
    PS_MODE_DTIM,
    PS_MODE_VOIP,
    PS_MODE_UAPSD_WMM,
    PS_MODE_UAPSD,
    PS_MODE_IBSS,
    PS_MODE_WWLAN,
    PM_Radio_Off,
    PM_Card_Disable,
    PS_MODE_NUM,
};

typedef enum {
    rf_on,
    rf_sleep,
    rf_off,
    rf_max
} rf_power_state_st;

typedef struct _RSVDPAGE_LOC {
        u8 LocProbeRsp;
        u8 LocPsPoll;
        u8 LocNullData;
        u8 LocQosNull;
        u8 LocBTQosNull;
        u8 LocApOffloadBCN;
    } RSVDPAGE_LOC, *PRSVDPAGE_LOC;

// For nic_info->pwr_info
typedef struct pwr_info
{
    wf_os_api_sema_t lock;
    wf_os_api_sema_t check_32k_lock;
    rf_power_state_st rf_pwr_state;
    wf_u32 lps_enter_cnts;
    wf_u32 lps_exit_cnts;
    wf_u8 lps_idle_cnts;
//    wf_bool b_in_suspend;
//    wf_bool b_internal_auto_suspend;
//    wf_bool b_keep_fw_alive;
//    wf_bool b_ips_processing;
    wf_u8 pwr_state_check_cnts;
    wf_u8 pwr_mgnt;
    wf_u8 pwr_current_mode;
    volatile wf_u8 rpwm;
    volatile wf_u8 wfprs;
    wf_u8 bcn_ant_mode;
    wf_bool b_fw_current_in_ps_mode;
    wf_u8 smart_lps;
//    wf_bool b_leisure_lps;
    wf_u32 delay_lps_last_timestamp;
    wf_u32 lps_deny;
    wf_bool b_power_saving;
    wf_bool b_mailbox_sync;
    atomic_t lps_spc_flag;
}pwr_info_st;

/***************************************************************
    Static Function Declare
***************************************************************/
static bool st_check_nic_state(nic_info_st *pnic_info, wf_u32 check_state);
static void st_enter_lps(nic_info_st *pnic_info, char *msg);
static void st_exit_lps(nic_info_st *pnic_info, char *msg);
inline static wf_u32 st_get_current_time(void);
inline static wf_u32 st_ms_to_systime(wf_u32 ms);
inline static wf_u32 st_systime_to_ms(wf_u32 systime);
inline static void _st_init_lps_lock(wf_os_api_sema_t *sema);
inline static void _st_enter_lps_lock(wf_os_api_sema_t *sema);
inline static void _st_exit_lps_lock(wf_os_api_sema_t *sema);
static int st_check_lps_ok(nic_info_st *pnic_info);
static void st_set_lps_mode_hw(nic_info_st *pnic_info, wf_u8 bcn_ant_mode, wf_u8 ps_mode, wf_u8 smart_ps, const char* msg);
static void st_set_rpwm_hw(nic_info_st *pnic_info, wf_u8 lps_state);

// Configure register
static void st_set_lps_hw_reg(nic_info_st *pnic_info, wf_u8 type, wf_u8 in_value);
static void st_set_fw_join_bss_rpt_cmd(nic_info_st *pnic_info, wf_u8 value);
static void st_hal_set_fw_rsvd_page(nic_info_st *pnic_info);
static wf_s32 st_get_lps_hw_reg(nic_info_st *pnic_info, wf_u8 type, wf_u8 *out_value);
static wf_u8* st_query_data_from_ie(wf_u8 * ie, wf_u8 type);
static wf_u8 * st_ie_to_set_func(wf_u8 * pbuf, int index, wf_u32 len, wf_u8 * source, wf_u32 * frlen);
static void st_rsvd_page_chip_hw_construct_beacon(nic_info_st *pnic_info, wf_u8 *frame_index_ptr, wf_u32 *length_out);
static void st_rsvd_page_chip_hw_construct_pspoll(nic_info_st *pnic_info, wf_u8 *frame_index_ptr, wf_u32 *length_out);
static void st_rsvd_page_chip_hw_construct_nullfunctiondata(nic_info_st *pnic_info, wf_u8 *frame_index_ptr, wf_u32 *length_out,
                                                            wf_u8 *addr_start_ptr, wf_bool b_qos, wf_u8 ac, wf_u8 eosp, wf_bool b_force_power_save);
static void st_rsvd_page_mgntframe_attrib_update(nic_info_st * pnic_info, struct pkt_attrib *pattrib);
static void st_fill_fake_txdesc(nic_info_st *pnic_info, wf_u8 *tx_des_start_addr, wf_u32 pkt_len,
                                      wf_bool is_ps_poll, wf_bool is_bt_qos_null, wf_bool is_dataframe);
static wf_s32 st_mgntframe_xmit(nic_info_st * pnic_info, struct xmit_frame *mgnt_frame_ptr);
static wf_s32 st_chip_hw_mgnt_xmit(nic_info_st * pnic_info, struct xmit_frame * pmgntframe);
static wf_s32 st_rsvd_page_h2c_loc_set(nic_info_st * nic_info, PRSVDPAGE_LOC rsvdpageloc);
static void st_set_fw_power_mode(nic_info_st *pnic_info, wf_u8 lps_mode);
static wf_bool st_mpdu_send_complete_cb(nic_info_st *nic_info, struct xmit_buf *pxmitbuf);
static wf_bool st_mpdu_insert_sending_queue(nic_info_st *nic_info, struct xmit_frame *pxmitframe, wf_bool ack);

/***************************************************************
    Function Declare
***************************************************************/

wf_s32 wf_lps_init(nic_info_st *pnic_info);
wf_s32 wf_lps_wakeup(nic_info_st *pnic_info, wf_u8 lps_ctrl_type, wf_bool enqueue);
void wf_lps_deny(nic_info_st *pnic_info, PS_DENY_REASON reason);
void wf_lps_deny_cancel(nic_info_st *pnic_info, PS_DENY_REASON reason);
wf_s32 wf_lps_sleep(nic_info_st *pnic_info, wf_u8 lps_ctrl_type, wf_bool enqueue);
void wf_lps_sleep_mlme_monitor(nic_info_st *pnic_info);
void wf_lps_ctrl_wk_hdl(nic_info_st *pnic_info, wf_u8 lps_ctrl_type);

#endif
#endif


