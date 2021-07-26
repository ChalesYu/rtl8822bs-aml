#ifndef __LPS_H__
#define __LPS_H__

#ifdef CONFIG_LPS
/***************************************************************
    Define
***************************************************************/
#define USE_M0_LPS_INTERFACES      (1)

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

#define TIMESTAMPE                 0
#define BCN_INTERVAL               1
#define CAPABILITY                 2

#define BCNQ_PAGE_NUM_9086X        0x08
#define BCNQ1_PAGE_NUM_9086X       0x08

#ifdef CONFIG_WOWLAN
#define WOWLAN_PAGE_NUM_9086X      0x07
#else
#define WOWLAN_PAGE_NUM_9086X      0x00
#endif

#define TX_TOTAL_PAGE_NUMBER_9086X      (0xFF - BCNQ_PAGE_NUM_9086X - BCNQ1_PAGE_NUM_9086X - WOWLAN_PAGE_NUM_9086X)
#define TX_PAGE_BOUNDARY_9086X          (TX_TOTAL_PAGE_NUMBER_9086X + 1)

#define PageNum(_Len, _Size)            (u32)(((_Len)/(_Size)) + ((_Len)&((_Size) - 1) ? 1:0))

#define wMBOX1_PWRMODE_LEN                                                  (7)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_PROBE_RSP(__pwMBOX1Cmd, __Value)         wf_set_bits_to_le_u8(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_PSPOLL(__pwMBOX1Cmd, __Value)            wf_set_bits_to_le_u8((__pwMBOX1Cmd)+1, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_NULL_DATA(__pwMBOX1Cmd, __Value)         wf_set_bits_to_le_u8((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_QOS_NULL_DATA(__pwMBOX1Cmd, __Value)     wf_set_bits_to_le_u8((__pwMBOX1Cmd)+3, 0, 8, __Value)
#define SET_wMBOX1CMD_RSVDPAGE_LOC_BT_QOS_NULL_DATA(__pwMBOX1Cmd, __Value)  wf_set_bits_to_le_u8((__pwMBOX1Cmd)+4, 0, 8, __Value)

#define wMBOX1_RSVDPAGE_LOC_LEN                                                         (5)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_MODE(__pwMBOX1Cmd, __Value)                    wf_set_bits_to_le_u8(__pwMBOX1Cmd, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_RLBM(__pwMBOX1Cmd, __Value)                    wf_set_bits_to_le_u8((__pwMBOX1Cmd)+1, 0, 4, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_SMART_PS(__pwMBOX1Cmd, __Value)                wf_set_bits_to_le_u8((__pwMBOX1Cmd)+1, 4, 4, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_BCN_PASS_TIME(__pwMBOX1Cmd, __Value)           wf_set_bits_to_le_u8((__pwMBOX1Cmd)+2, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_ALL_QUEUE_UAPSD(__pwMBOX1Cmd, __Value)         wf_set_bits_to_le_u8((__pwMBOX1Cmd)+3, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_BCN_EARLY_wMBOX0_RPT(__pwMBOX1Cmd, __Value)    wf_set_bits_to_le_u8((__pwMBOX1Cmd)+3, 2, 1, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_PWR_STATE(__pwMBOX1Cmd, __Value)               wf_set_bits_to_le_u8((__pwMBOX1Cmd)+4, 0, 8, __Value)
#define SET_9086X_wMBOX1CMD_PWRMODE_PARM_BYTE5(__pwMBOX1Cmd, __Value)                   wf_set_bits_to_le_u8((__pwMBOX1Cmd)+5, 0, 8, __Value)
#define GET_9086X_wMBOX1CMD_PWRMODE_PARM_MODE(__pwMBOX1Cmd)                             wf_le_bits_to_u8(__pwMBOX1Cmd, 0, 8)

/***************************************************************
    Typedef
***************************************************************/

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
    PWR_MODE_ACTIVE = 0,
    PWR_MODE_MIN,
    PWR_MODE_MAX,
    PWR_MODE_DTIM,
    PWR_MODE_VOIP,
    PWR_MODE_UAPSD_WMM,
    PWR_MODE_UAPSD,
    PWR_MODE_IBSS,
    PWR_MODE_WWLAN,
    PWR_Radio_Off,
    PWR_Card_Disable,
    PWR_MODE_NUM,
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

typedef struct
{
    wf_u8 lps_ctrl_type;
} mlme_lps_t;
#endif

// For nic_info->pwr_info
typedef struct pwr_info
{
    wf_bool bInSuspend;
#ifdef CONFIG_WOWLAN
    
#endif
#ifdef CONFIG_LPS
    wf_os_api_sema_t lock;
    wf_os_api_sema_t check_32k_lock;
    rf_power_state_st rf_pwr_state;
    wf_u32 lps_enter_cnts;
    wf_u32 lps_exit_cnts;
    wf_u8 lps_idle_cnts;
    wf_u8 pwr_mgnt;
    wf_u8 pwr_current_mode;
    volatile wf_u8 rpwm;
    wf_bool b_fw_current_in_ps_mode;
    wf_u8 smart_lps;
    wf_u64 delay_lps_last_timestamp;
    wf_u32 lps_deny;
    wf_bool b_power_saving;
    wf_bool b_mailbox_sync;
    atomic_t lps_spc_flag;
    wf_timer_t lps_timer;
#endif
}pwr_info_st;

/***************************************************************
    Function Declare
***************************************************************/
#ifdef CONFIG_LPS
void wf_lps_ctrl_wk_hdl(nic_info_st *pnic_info, wf_u8 lps_ctrl_type);
wf_u32 wf_lps_wakeup(nic_info_st *pnic_info, wf_u8 lps_ctrl_type, wf_bool enqueue);
wf_u32 wf_lps_sleep(nic_info_st *pnic_info, wf_u8 lps_ctrl_type, wf_bool enqueue);
wf_pt_rst_t wf_lps_sleep_mlme_monitor(wf_pt_t *pt, nic_info_st *pnic_info);
void wf_lps_ctrl_state_hdl(nic_info_st *pnic_info, wf_u8 lps_ctrl_type);
#endif
wf_s32 wf_lps_init(nic_info_st *pnic_info);
wf_s32 wf_lps_term(nic_info_st *pnic_info);

#endif


