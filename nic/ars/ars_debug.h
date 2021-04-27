#ifndef __ARS_DEBUG_H__
#define __ARS_DEBUG_H__
#define ODM_DBG_OFF                 1

//
//  Fatal bug. 
//  For example, Tx/Rx/IO locked up, OS hangs, memory access violation, 
//  resource allocation failed, unexpected HW behavior, HW BUG and so on.
//
#define ODM_DBG_SERIOUS             2

//
//  Abnormal, rare, or unexpeted cases.
//  For example, IRP/Packet/OID canceled, device suprisely unremoved and so on.
//
#define ODM_DBG_WARNING             3

//
//  Normal case with useful information about current SW or HW state. 
//  For example, Tx/Rx descriptor to fill, Tx/Rx descriptor completed status, 
//  SW protocol state change, dynamic mechanism state change and so on.
//
#define ODM_DBG_LOUD                    4

//
//  Normal case with detail execution flow or information.
//
#define ODM_DBG_TRACE                   5


/*FW DBG MSG*/
#define RATE_DECISION   BIT(0)
#define INIT_RA_TABLE   BIT(1)
#define RATE_UP         BIT(2)
#define RATE_DOWN       BIT(3)
#define TRY_DONE        BIT(4)
#define F_RATE_AP_RPT   BIT(7)

//-----------------------------------------------------------------------------
// Define the tracing components
//
//-----------------------------------------------------------------------------
//BB Functions
#define ODM_COMP_DIG                    BIT(0)  
#define ODM_COMP_RA_MASK                BIT(1)  
#define ODM_COMP_DYNAMIC_TXPWR      BIT(2)
#define ODM_COMP_FA_CNT             BIT(3)
#define ODM_COMP_RSSI_MONITOR       BIT(4)
#define ODM_COMP_CCK_PD             BIT(5)
#define ODM_COMP_ANT_DIV                BIT(6)
#define ODM_COMP_PWR_SAVE           BIT(7)
#define ODM_COMP_PWR_TRAIN          BIT(8)
#define ODM_COMP_RATE_ADAPTIVE      BIT(9)
#define ODM_COMP_PATH_DIV           BIT(10)
#define ODM_COMP_PSD                    BIT(11)
#define ODM_COMP_DYNAMIC_PRICCA     BIT(12)
#define ODM_COMP_RXHP                   BIT(13)
#define ODM_COMP_MP                 BIT(14)
#define ODM_COMP_CFO_TRACKING       BIT(15)
#define ODM_COMP_ACS                    BIT(16)
#define PHYDM_COMP_ADAPTIVITY       BIT(17)
#define PHYDM_COMP_RA_DBG           BIT(18)
#define PHYDM_COMP_TXBF             BIT(19)
//MAC Functions
#define ODM_COMP_EDCA_TURBO         BIT(20)
#define ODM_COMP_EARLY_MODE         BIT(21)
#define ODM_FW_DEBUG_TRACE          BIT(22)
//RF Functions
#define ODM_COMP_TX_PWR_TRACK       BIT(24)
#define ODM_COMP_RX_GAIN_TRACK      BIT(25)
#define ODM_COMP_CALIBRATION            BIT(26)
//Common Functions
#define ODM_PHY_CONFIG              BIT(28)
#define BEAMFORMING_DEBUG               BIT(29)
#define ODM_COMP_COMMON             BIT(30)
#define ODM_COMP_INIT                   BIT(31)
#define ODM_COMP_NOISY_DETECT       BIT(32)

typedef struct _ODM_Phy_Dbg_Info_
{
    //ODM Write,debug info
    wf_s8       RxSNRdB[4];
    wf_u32      NumQryPhyStatus;
    wf_u32      NumQryPhyStatusCCK;
    wf_u32      NumQryPhyStatusOFDM;
    wf_u8       NumQryBeaconPkt;
    //Others
    wf_s32      RxEVM[4];
    
}ODM_PHY_DBG_INFO_T;

typedef struct debug_priv
{
    wf_u32 dbg_sdio_free_irq_error_cnt;
    wf_u32 dbg_sdio_alloc_irq_error_cnt;
    wf_u32 dbg_sdio_free_irq_cnt;
    wf_u32 dbg_sdio_alloc_irq_cnt;
    wf_u32 dbg_sdio_deinit_error_cnt;
    wf_u32 dbg_sdio_init_error_cnt;
    wf_u32 dbg_suspend_error_cnt;
    wf_u32 dbg_suspend_cnt;
    wf_u32 dbg_resume_cnt;
    wf_u32 dbg_resume_error_cnt;
    wf_u32 dbg_deinit_fail_cnt;
    wf_u32 dbg_carddisable_cnt;
    wf_u32 dbg_carddisable_error_cnt;
    wf_u32 dbg_ps_insuspend_cnt;
    wf_u32  dbg_dev_unload_inIPS_cnt;
    wf_u32 dbg_wow_leave_ps_fail_cnt;
    wf_u32 dbg_scan_pwr_state_cnt;
    wf_u32 dbg_downloadfw_pwr_state_cnt;
    wf_u32 dbg_fw_read_ps_state_fail_cnt;
    wf_u32 dbg_leave_ips_fail_cnt;
    wf_u32 dbg_leave_lps_fail_cnt;
    wf_u32 dbg_h2c_leave32k_fail_cnt;
    wf_u32 dbg_diswow_dload_fw_fail_cnt;
    wf_u32 dbg_enwow_dload_fw_fail_cnt;
    wf_u32 dbg_ips_drvopen_fail_cnt;
    wf_u32 dbg_poll_fail_cnt;
    wf_u32 dbg_rpwm_toogle_cnt;
    wf_u32 dbg_rpwm_timeout_fail_cnt;
    wf_u32 dbg_sreset_cnt;
    wf_u64 dbg_rx_fifo_last_overflow;
    wf_u64 dbg_rx_fifo_curr_overflow;
    wf_u64 dbg_rx_fifo_diff_overflow;
    wf_u64 dbg_rx_ampdu_drop_count;
    wf_u64 dbg_rx_ampdu_forced_indicate_count;
    wf_u64 dbg_rx_ampdu_loss_count;
    wf_u64 dbg_rx_dup_mgt_frame_drop_count;
    wf_u64 dbg_rx_ampdu_window_shift_cnt;
    wf_u64 dbg_rx_conflic_mac_addr_cnt;
}debug_priv_st;


typedef struct ars_dbg_info_st_
{
    ODM_PHY_DBG_INFO_T dbg_info;
    debug_priv_st  dbg_priv;
    wf_u64          DebugComponents;
    wf_u32          DebugLevel;
    wf_bool         fw_buff_is_enpty;
    wf_u8           pre_c2h_seq;
}ars_dbg_info_st;

void ars_debug_init(void *ars);

#endif
