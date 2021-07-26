#ifndef __WF_HW_INFO_H__
#define __WF_HW_INFO_H__


#ifdef CONFIG_RICHV200
#define RXDESC_SIZE 24
#else
#define RXDESC_SIZE 24
#endif
#define DRVINFO_SZ  4

#ifndef MAX_RECVBUF_SZ
#ifdef CONFIG_MINIMAL_MEMORY_USAGE
#define MAX_RECVBUF_SZ (4000)
#else
#ifdef CONFIG_PLATFORM_IPC
#ifdef CONFIG_LOWMEM
#define MAX_RECVBUF_SZ  (12288)
#else
#define MAX_RECVBUF_SZ (16384)
#endif
#else
#define MAX_RECVBUF_SZ (32768)
#endif
#endif
#endif

#define CENTER_CH_2G_NUM		14
#define MAX_TX_COUNT            4

#define TX_PWR_BY_RATE_NUM_BAND			1
#define TX_PWR_BY_RATE_NUM_RF			1
#define TX_PWR_BY_RATE_NUM_RATE			20

#define MAX_REGULATION_NUM						4
#define MAX_RF_PATH_NUM_IN_POWER_LIMIT_TABLE	4
#define MAX_2_4G_BANDWIDTH_NUM					2
#define MAX_RATE_SECTION_NUM					10


typedef enum _REGULATION_TXPWR_LMT
{
    TXPWR_LMT_FCC = 0,
    TXPWR_LMT_MKK = 1,
    TXPWR_LMT_ETSI = 2,
    TXPWR_LMT_WW = 3,

    TXPWR_LMT_MAX_REGULATION_NUM = 4
} REGULATION_TXPWR_LMT;


#ifdef CONFIG_WOWLAN
#define MAX_WKFM_NUM 16
#define MAX_WKFM_SIZE   16
#define MAX_WKFM_PATTERN_SIZE   128
#define WKFMCAM_ADDR_NUM 6
#define WKFMCAM_SIZE 24
enum pattern_type
{
    PATTERN_BROADCAST = 0,
    PATTERN_MULTICAST,
    PATTERN_UNICAST,
    PATTERN_VALID,
    PATTERN_INVALID,
};

typedef struct wltpriv_pattern
{
    int len;
    char content[MAX_WKFM_PATTERN_SIZE];
    char mask[MAX_WKFM_SIZE];
} wltpriv_pattern_t;

struct wltwow_pattern
{
    wf_u16 crc;
    wf_u8 type;
    wf_u32 mask[4];
};
#endif



typedef enum _CHANNEL_WIDTH
{
    CHANNEL_WIDTH_20 = 0,
    CHANNEL_WIDTH_40 = 1,
    CHANNEL_WIDTH_MAX = 2,
} CHANNEL_WIDTH;

typedef enum _EXTCHNL_OFFSET
{
    EXTCHNL_OFFSET_NO_EXT = 0,
    EXTCHNL_OFFSET_UPPER = 1,
    EXTCHNL_OFFSET_NO_DEF = 2,
    EXTCHNL_OFFSET_LOWER = 3,
} EXTCHNL_OFFSET;

typedef enum _HT_DATA_SC
{
    HT_DATA_SC_DONOT_CARE = 0,
    HT_DATA_SC_20_UPPER_OF_40MHZ = 1,
    HT_DATA_SC_20_LOWER_OF_40MHZ = 2,
} HT_DATA_SC;


#define REG_HWSEQ_CTRL                  0x0423
#define REG_BAR_MODE_CTRL               0x04CC

#define WF_XMIT_CTL             0x22C
#define WF_XMIT_AGG_MAXNUMS     0x01
#define WF_XMIT_AMPDU_DENSITY   0x02
#define WF_XMIT_OFFSET          0x04
#define WF_XMIT_PKT_OFFSET      0x08
#define WF_XMIT_ALL             0xFF



typedef enum HAL_PRIME_CH_OFFSET_
{
    HAL_PRIME_CHNL_OFFSET_DONT_CARE = 0,
    HAL_PRIME_CHNL_OFFSET_LOWER     = 1,
    HAL_PRIME_CHNL_OFFSET_UPPER     = 2,
} HAL_PRIME_CH_OFFSET;


/*efuse*/
#define EEPROM_TX_PWR_INX_9086X                 0x10
#define EEPROM_ChannelPlan_9086X                0xBA
#define EEPROM_XTAL_9086X                       0xB9
#define EEPROM_THERMAL_METER_9086X              0x22
#define EEPROM_RF_BOARD_OPTION_9086X            0xC1
#define EEPROM_FEATURE_OPTION_9086X             0xC2
#define EEPROM_VERSION_9086X                    0xC4
#define EEPROM_CustomID_9086X                   0xC5
#define EEPROM_COUNTRY_CODE_9086X               0xCB
#define EEPROM_MAC_ADDR_9086XU                  0xD7
#define EEPROM_VID_9086XU                       0xD0
#define EEPROM_PID_9086XU                       0xD2
#define EEPROM_USB_OPTIONAL_FUNCTION0_9086XU    0xD4
#define EEPROM_CHANNEL_PLAN_BY_HW_MASK          0x80
#define EEPROM_Default_ThermalMeter             0x12
#define EEPROM_Default_ThermalMeter_9086X       0x18
#define EEPROM_Default_CrystalCap_9086X         0x20
#define EEPROM_DEFAULT_24G_INDEX                0x2D
#define EEPROM_DEFAULT_24G_HT20_DIFF            0X02

#define EEPROM_DEFAULT_24G_CCK_INDEX            0x25
#define EEPROM_DEFAULT_24G_OFDM_INDEX           0x2A
#define EEPROM_DEFAULT_24G_OFDM_DIFF            0X02


#define EEPROM_DEFAULT_DIFF                     0XFE
#define EEPROM_DEFAULT_BOARD_OPTION             0x00

typedef enum _HT_CAP_AMPDU_FACTOR
{
    MAX_AMPDU_FACTOR_8K = 0,
    MAX_AMPDU_FACTOR_16K = 1,
    MAX_AMPDU_FACTOR_32K = 2,
    MAX_AMPDU_FACTOR_64K = 3,
} HT_CAP_AMPDU_FACTOR;

typedef enum _HT_CAP_AMPDU_DENSITY
{
    AMPDU_DENSITY_VALUE_0 = 0,
    AMPDU_DENSITY_VALUE_1 = 1,
    AMPDU_DENSITY_VALUE_2 = 2,
    AMPDU_DENSITY_VALUE_3 = 3,
    AMPDU_DENSITY_VALUE_4 = 4,
    AMPDU_DENSITY_VALUE_5 = 5,
    AMPDU_DENSITY_VALUE_6 = 6,
    AMPDU_DENSITY_VALUE_7 = 7,
} HT_CAP_AMPDU_DENSITY;


/*hardware register variable*/
typedef enum _HW_REG_VARIABLES
{
    HW_VAR_MEDIA_STATUS,
    HW_VAR_MEDIA_STATUS1,
    HW_VAR_SET_OPMODE,
    HW_VAR_MAC_ADDR,
    HW_VAR_BSSID,
    HW_VAR_INIT_RTS_RATE,
    HW_VAR_BASIC_RATE,
    HW_VAR_TXPAUSE,
    HW_VAR_BCN_FUNC,
    HW_VAR_CORRECT_TSF,
    HW_VAR_CHECK_BSSID,
    HW_VAR_MLME_DISCONNECT,
    HW_VAR_MLME_SITESURVEY,
    HW_VAR_MLME_JOIN,
    HW_VAR_ON_RCR_AM,
    HW_VAR_OFF_RCR_AM,
    HW_VAR_BEACON_INTERVAL,
    HW_VAR_SLOT_TIME,
    HW_VAR_RESP_SIFS,
    HW_VAR_ACK_PREAMBLE,
    HW_VAR_SEC_CFG,
    HW_VAR_SEC_DK_CFG,
    HW_VAR_BCN_VALID,
    HW_VAR_RF_TYPE,
    HW_VAR_CAM_EMPTY_ENTRY,
    HW_VAR_CAM_INVALID_ALL,
    HW_VAR_AC_PARAM_VO,
    HW_VAR_AC_PARAM_VI,
    HW_VAR_AC_PARAM_BE,
    HW_VAR_AC_PARAM_BK,
    HW_VAR_ACM_CTRL,
    HW_VAR_AMPDU_MIN_SPACE,
    HW_VAR_AMPDU_FACTOR,
    HW_VAR_RXDMA_AGG_PG_TH,
    HW_VAR_SET_RPWM,
    HW_VAR_wFPRS,
    HW_VAR_wMBOX1_FW_PWRMODE,
    HW_VAR_wMBOX1_PS_TUNE_PARAM,
    HW_VAR_wMBOX1_FW_JOINBSSRPT,
    HW_VAR_FWLPS_RF_ON,
    HW_VAR_wMBOX1_FW_P2P_PS_OFFLOAD,
    HW_VAR_TRIGGER_GPIO_0,
    HW_VAR_PRIV_0,
    HW_VAR_PRIV_1,
    HW_VAR_SWITCH_EPHY_WoWLAN,
    HW_VAR_EFUSE_USAGE,
    HW_VAR_EFUSE_BYTES,
    HW_VAR_PRIV_2,
    HW_VAR_PRIV_3,  //30
    HW_VAR_FIFO_CLEARN_UP,
    HW_VAR_RESTORE_HW_SEQ,
    HW_VAR_CHECK_TXBUF,
    HW_VAR_PRIV_4,
    HW_VAR_APFM_ON_MAC,
    HW_VAR_HCI_SUS_STATE,
    HW_VAR_SYS_CLKR,
    HW_VAR_NAV_UPPER,
    HW_VAR_wMBOX0_HANDLE,
    HW_VAR_RPT_TIMER_SETTING,
    HW_VAR_TX_RPT_MAX_MACID,
    HW_VAR_CHK_HI_QUEUE_EMPTY,
    HW_VAR_DL_BCN_SEL,
    HW_VAR_AMPDU_MAX_TIME,
    HW_VAR_WIRELESS_MODE,
    HW_VAR_USB_MODE,
    HW_VAR_PORT_SWITCH,
    HW_VAR_DO_IQK,
    HW_VAR_DM_IN_LPS,
    HW_VAR_SET_REQ_FW_PS,
    HW_VAR_FW_PS_STATE,
    HW_VAR_SOUNDING_ENTER,
    HW_VAR_SOUNDING_LEAVE,
    HW_VAR_SOUNDING_RATE,
    HW_VAR_SOUNDING_STATUS,
    HW_VAR_SOUNDING_FW_NDPA,
    HW_VAR_SOUNDING_CLK,
    HW_VAR_HW_REG_TIMER_INIT,
    HW_VAR_HW_REG_TIMER_RESTART,
    HW_VAR_HW_REG_TIMER_START,
    HW_VAR_HW_REG_TIMER_STOP,
    HW_VAR_DL_RSVD_PAGE,
    HW_VAR_MACID_LINK,
    HW_VAR_MACID_NOLINK,
    HW_VAR_MACID_SLEEP,
    HW_VAR_MACID_WAKEUP,
    HW_VAR_DUMP_MAC_QUEUE_INFO,
    HW_VAR_ASIX_IOT,
    HW_VAR_EN_HW_UPDATE_TSF,
    HW_VAR_CH_SW_NEED_TO_TAKE_CARE_IQK_INFO,
    HW_VAR_CH_SW_IQK_INFO_BACKUP,
    HW_VAR_CH_SW_IQK_INFO_RESTORE,
    HW_VAR_BCN_VALID1,
    HW_VAR_WOWLAN,
    HW_VAR_WAKEUP_REASON,
    HW_VAR_RPWM_TOG,
    HW_SET_GPIO_WL_CTRL,
} HW_VARIABLES;


typedef enum _WP_CHANNEL_DOMAIN
{
    WF_CHPLAN_FCC = 0x00,
    WF_CHPLAN_IC = 0x01,
    WF_CHPLAN_ETSI = 0x02,
    WF_CHPLAN_SPAIN = 0x03,
    WF_CHPLAN_FRANCE = 0x04,
    WF_CHPLAN_MKK = 0x05,
    WF_CHPLAN_MKK1 = 0x06,
    WF_CHPLAN_ISRAEL = 0x07,
    WF_CHPLAN_TELEC = 0x08,
    WF_CHPLAN_GLOBAL_DOAMIN = 0x09,
    WF_CHPLAN_WORLD_WIDE_13 = 0x0A,
    WF_CHPLAN_TAIWAN = 0x0B,
    WF_CHPLAN_CHINA = 0x0C,
    WF_CHPLAN_SINGAPORE_INDIA_MEXICO = 0x0D,
    WF_CHPLAN_KOREA = 0x0E,
    WF_CHPLAN_TURKEY = 0x0F,
    WF_CHPLAN_JAPAN = 0x10,
    WF_CHPLAN_FCC_NO_DFS = 0x11,
    WF_CHPLAN_JAPAN_NO_DFS = 0x12,
    WF_CHPLAN_WORLD_WIDE_5G = 0x13,
    WF_CHPLAN_TAIWAN_NO_DFS = 0x14,

    WF_CHPLAN_WORLD_NULL = 0x20,
    WF_CHPLAN_ETSI1_NULL = 0x21,
    WF_CHPLAN_FCC1_NULL = 0x22,
    WF_CHPLAN_MKK1_NULL = 0x23,
    WF_CHPLAN_ETSI2_NULL = 0x24,
    WF_CHPLAN_FCC1_FCC1 = 0x25,
    WF_CHPLAN_WORLD_ETSI1 = 0x26,
    WF_CHPLAN_MKK1_MKK1 = 0x27,
    WF_CHPLAN_WORLD_KCC1 = 0x28,
    WF_CHPLAN_WORLD_FCC2 = 0x29,
    WF_CHPLAN_FCC2_NULL = 0x2A,
    WF_CHPLAN_WORLD_FCC3 = 0x30,
    WF_CHPLAN_WORLD_FCC4 = 0x31,
    WF_CHPLAN_WORLD_FCC5 = 0x32,
    WF_CHPLAN_WORLD_FCC6 = 0x33,
    WF_CHPLAN_FCC1_FCC7 = 0x34,
    WF_CHPLAN_WORLD_ETSI2 = 0x35,
    WF_CHPLAN_WORLD_ETSI3 = 0x36,
    WF_CHPLAN_MKK1_MKK2 = 0x37,
    WF_CHPLAN_MKK1_MKK3 = 0x38,
    WF_CHPLAN_FCC1_NCC1 = 0x39,
    WF_CHPLAN_FCC1_NCC2 = 0x40,
    WF_CHPLAN_GLOBAL_NULL = 0x41,
    WF_CHPLAN_ETSI1_ETSI4 = 0x42,
    WF_CHPLAN_FCC1_FCC2 = 0x43,
    WF_CHPLAN_FCC1_NCC3 = 0x44,
    WF_CHPLAN_WORLD_ETSI5 = 0x45,
    WF_CHPLAN_FCC1_FCC8 = 0x46,
    WF_CHPLAN_WORLD_ETSI6 = 0x47,
    WF_CHPLAN_WORLD_ETSI7 = 0x48,
    WF_CHPLAN_WORLD_ETSI8 = 0x49,
    WF_CHPLAN_WORLD_ETSI9 = 0x50,
    WF_CHPLAN_WORLD_ETSI10 = 0x51,
    WF_CHPLAN_WORLD_ETSI11 = 0x52,
    WF_CHPLAN_FCC1_NCC4 = 0x53,
    WF_CHPLAN_WORLD_ETSI12 = 0x54,
    WF_CHPLAN_FCC1_FCC9 = 0x55,
    WF_CHPLAN_WORLD_ETSI13 = 0x56,
    WF_CHPLAN_FCC1_FCC10 = 0x57,
    WF_CHPLAN_MKK2_MKK4 = 0x58,
    WF_CHPLAN_WORLD_ETSI14 = 0x59,
    WF_CHPLAN_FCC1_FCC5 = 0x60,

    WF_CHPLAN_MAX,
    WF_CHPLAN_WK_WLAN_DEFINE = 0x7F,
} WP_CHANNEL_DOMAIN, *WTL_CHANNEL_DOMAIN;

typedef enum _WP_CHANNEL_DOMAIN_2G
{
    WF_RD_2G_NULL = 0,
    WF_RD_2G_WORLD = 1,
    WF_RD_2G_ETSI1 = 2,
    WF_RD_2G_FCC1 = 3,
    WF_RD_2G_MKK1 = 4,
    WF_RD_2G_ETSI2 = 5,
    WF_RD_2G_GLOBAL = 6,
    WF_RD_2G_MKK2 = 7,
    WF_RD_2G_FCC2 = 8,

    WF_RD_2G_MAX,
} WP_CHANNEL_DOMAIN_2G, *WTL_CHANNEL_DOMAIN_2G;


/* hw_info_st */
typedef struct
{
    wf_u32 qual; /* dbm */
    wf_u32 level; /* dbm */
    wf_u32 noise; /* dbm */
    wf_u32 updated;
} rf_quality_st;
typedef struct
{
    wf_u8 num; /* The channel number. */
    wf_s32 freq; /* channel frequence */
} channel_info_st;


typedef struct hw_register_st_
{
    wf_u32 rf_reg_chnl_val; //it could not used ,because it is zero in USB
    wf_u32 cam_invalid;
    wf_u8  channel;
} hw_register_st;

typedef struct wireless_info_st_
{
    /* iw_get_range */
    wf_u32              throughput; /* the maximum benchmarked TCP/IP throughput */
    rf_quality_st       max_qual; /* Quality of the link */
    rf_quality_st       avg_qual; /* the average/typical values of the quality indicator */
    wf_u8               num_bitrates;
    wf_s32              min_frag; /* Max frag threshold */
    wf_s32              max_frag; /* Max frag threshold */
    wf_u16              pm_capa;
    channel_info_st    *pchannel_tab;
    wf_u16              num_channels; /* Number of channels [0; num - 1] */
    wf_u32              enc_capa; /* the security capability to network manager */
    wf_u8               scan_capa; /* Scan capabilities */
    wf_s32              bitrate[WF_MAX_BITRATES];

} wireless_info_st;


/*important data stored in efuse*/
typedef struct efuse_data_st_
{
    wf_u16 id;
    wf_u16 vid;//vendor id
    wf_u16 pid;//product id
} efuse_data_st;

#define MAX_CHANNEL_NUM         14
typedef enum
{
    SCAN_TYPE_PASSIVE,
    SCAN_TYPE_ACTIVE,

    SCAN_TYPE_MAX,
} scan_type_e;

typedef struct
{
    wf_u8 channel_num;
    scan_type_e scan_type;
} wf_channel_info_t;

#define	MAX_CHNL_GROUP_24G		6
typedef struct 
{
	wf_u8 IndexCCK_Base[1][MAX_CHNL_GROUP_24G];
	wf_u8 IndexBW40_Base[1][MAX_CHNL_GROUP_24G];
	wf_s8 CCK_Diff[1][MAX_TX_COUNT];
	wf_s8 OFDM_Diff[1][MAX_TX_COUNT];
	wf_s8 BW20_Diff[1][MAX_TX_COUNT];
	wf_s8 BW40_Diff[1][MAX_TX_COUNT];
} wf_txpower_info_t;

typedef struct hardware_info_struct_
{
    wf_u8 mp_mode;   // unknown, but need use

    wf_u8 chip_version;
    wf_u8 macAddr[WF_ETH_ALEN];

    wf_bool       bautoload_flag;
    wf_bool       efuse_sel;
    wf_u8         efuse_data_map[WF_EEPROM_MAX_SIZE];
    efuse_data_st efuse;
    wf_u8         efuse_read_flag;

    wireless_info_st *wireless_info;
    hw_register_st hw_reg;

    wf_u16 rts_thresh;
    wf_u16 frag_thresh;
    wf_u8  vcs_en;
    wf_u8  vcs_type;    //virtual carrier sense
    wf_u8  use_fixRate;
    wf_u8  tx_data_rpt;
    wf_u8  ba_enable;
    wf_u8  use_drv_odm;
    wf_u8  dot80211n_support;
    wf_u8  cbw40_support;
    wf_u8  sm_ps_support; /* SM Power Save. 0(static mode) 1(dynamic) 3(disabled or not supported) */
    wf_u8  sm_ps_mode; /* SM Power Save */
    wf_u8  rf_type;
    wf_u8  wdn_sleep_support;
    wf_u8  ldpc_support;
    wf_u8  tx_stbc_support;
    wf_u8  rx_stbc_support;
    wf_u8  rx_stbc_num;
    wf_u32 rx_packet_offset;
    wf_u32 max_recvbuf_sz;
    wf_u8  max_rx_ampdu_factor;
    wf_u8  best_ampdu_density;
    wf_s32 UndecoratedSmoothedPWDB;
    wf_u8  channel_plan;
    wf_channel_info_t channel_set[MAX_CHANNEL_NUM];
    wf_u8  max_chan_nums;
    wf_u8  Regulation2_4G;

    wf_u8  datarate[WF_RATES_NUM];
    wf_u8  default_supported_mcs_set[WF_MCS_NUM];

    wf_u8 bTXPowerDataReadFromEEPORM;
    wf_u8 EEPROMRegulatory;

    wf_u8 Index24G_CCK_Base[1][CENTER_CH_2G_NUM];
	wf_u8 Index24G_BW40_Base[1][CENTER_CH_2G_NUM];
	wf_s8 CCK_24G_Diff[1][MAX_TX_COUNT];
	wf_s8 OFDM_24G_Diff[1][MAX_TX_COUNT];
	wf_s8 BW20_24G_Diff[1][MAX_TX_COUNT];
	wf_s8 BW40_24G_Diff[1][MAX_TX_COUNT];
    
    wf_s8 TxPwrByRateOffset[TX_PWR_BY_RATE_NUM_BAND]
            [TX_PWR_BY_RATE_NUM_RF]
            [TX_PWR_BY_RATE_NUM_RF]
            [TX_PWR_BY_RATE_NUM_RATE];

    wf_s8 TxPwrLimit_2_4G[MAX_REGULATION_NUM]
		    [MAX_2_4G_BANDWIDTH_NUM]
		    [MAX_RATE_SECTION_NUM]
		    [CENTER_CH_2G_NUM]
		    [1];

} hw_info_st;


int wf_hw_info_init(nic_info_st *nic_info);
int wf_hw_info_get_default_cfg(nic_info_st *nic_info);
int wf_hw_info_set_default_cfg(nic_info_st *nic_info);
int wf_hw_info_term(nic_info_st *nic_info);
int wf_hw_info_set_channnel_bw(nic_info_st *nic_info, wf_u8 channel, CHANNEL_WIDTH cw, wf_u8 offset);
wf_u8 do_query_center_ch(wf_u8 chnl_bw, wf_u8 channel, wf_u8 chnl_offset);
int channel_init (nic_info_st *pnic_info);

#endif
