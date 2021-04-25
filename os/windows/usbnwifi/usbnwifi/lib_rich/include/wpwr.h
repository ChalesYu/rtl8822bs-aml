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

#ifndef __WL_PWRCTRL_H_
#define __WL_PWRCTRL_H_

#define FW_PWR0	0
#define FW_PWR1 	1
#define FW_PWR2 	2
#define FW_PWR3 	3

#define HW_PWR0	7
#define HW_PWR1 	6
#define HW_PWR2 	2
#define HW_PWR3	0
#define HW_PWR4	8

#define FW_PWRMSK	0x7

#define XMIT_ALIVE	BIT(0)
#define RECV_ALIVE	BIT(1)
#define CMD_ALIVE	BIT(2)
#define EVT_ALIVE	BIT(3)

#ifdef CONFIG_WOWLAN
#define MAX_WKFM_NUM 16
#define MAX_WKFM_SIZE	16
#define MAX_WKFM_PATTERN_SIZE	128
#define WKFMCAM_ADDR_NUM 6
#define WKFMCAM_SIZE 24
enum pattern_type {
	PATTERN_BROADCAST = 0,
	PATTERN_MULTICAST,
	PATTERN_UNICAST,
	PATTERN_VALID,
	PATTERN_INVALID,
};

typedef struct wltpriv_pattern {
	int len;
	char content[MAX_WKFM_PATTERN_SIZE];
	char mask[MAX_WKFM_SIZE];
} wltpriv_pattern_t;

struct wltwow_pattern {
	u16 crc;
	u8 type;
	u32 mask[4];
};
#endif

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


#define PS_DPS				BIT(0)
#define PS_LCLK				(PS_DPS)
#define PS_RF_OFF			BIT(1)
#define PS_ALL_ON			BIT(2)
#define PS_ST_ACTIVE		BIT(3)

#define PS_ISR_ENABLE		BIT(4)
#define PS_IMR_ENABLE		BIT(5)
#define PS_ACK				BIT(6)
#define PS_TOGGLE			BIT(7)

#define PS_STATE_MASK		(0x0F)
#define PS_STATE_HW_MASK	(0x07)
#define PS_SEQ_MASK			(0xc0)

#define PS_STATE(x)		(PS_STATE_MASK & (x))
#define PS_STATE_HW(x)	(PS_STATE_HW_MASK & (x))
#define PS_SEQ(x)		(PS_SEQ_MASK & (x))

#define PS_STATE_S0		(PS_DPS)
#define PS_STATE_S1		(PS_LCLK)
#define PS_STATE_S2		(PS_RF_OFF)
#define PS_STATE_S3		(PS_ALL_ON)
#define PS_STATE_S4		((PS_ST_ACTIVE) | (PS_ALL_ON))

#define PS_IS_RF_ON(x)	((x) & (PS_ALL_ON))
#define PS_IS_ACTIVE(x)	((x) & (PS_ST_ACTIVE))
#define CLR_PS_STATE(x)	((x) = ((x) & (0xF0)))

struct reportpwrstate_parm {
	unsigned char mode;
	unsigned char state;
	unsigned short rsvd;
};

typedef PRKSEMAPHORE _pwrlock;

__inline static void _init_pwrlock(_pwrlock * plock)
{
	//Func_Of_Proc_Pre_Init_Sema(plock, 1);
#if 0
	NTKERNELAPI  VOID  
    KeInitializeSemaphore (  
    IN PRKSEMAPHORE Semaphore,  
    IN LONG Count,  
    IN LONG Limit  
    );  
#endif
}

__inline static void _enter_pwrlock(_pwrlock * plock)
{
	//Func_Of_Proc_Down_Sema(plock);
}

__inline static void _exit_pwrlock(_pwrlock * plock)
{
	//Func_Of_Proc_Pre_Up_Sema(plock);
#if 0	
	NTKERNELAPI  
    LONG  
    KeReleaseSemaphore (  
    IN PRKSEMAPHORE Semaphore,  
    IN KPRIORITY Increment,  
    IN LONG Adjustment,  
    IN BOOLEAN Wait  
    );  
#endif
}

#define LPS_DELAY_TIME	1*HZ

#define EXE_PWR_NONE	0x01
#define EXE_PWR_IPS		0x02
#define EXE_PWR_LPS		0x04

typedef enum _wp_rf_power_state {
	rf_on,
	rf_sleep,
	rf_off,
	rf_max
} wp_rf_power_state;

#define	WP_RF_OFF_LEVL_ASPM			BIT(0)
#define	WP_RF_OFF_LEVL_CLK_REQ		BIT(1)
#define	WP_RF_OFF_LEVL_PCI_D3			BIT(2)
#define	WP_RF_OFF_LEVL_HALT_NIC		BIT(3)
#define	WP_RF_OFF_LEVL_FREE_FW		BIT(4)
#define	WP_RF_OFF_LEVL_FW_32K		BIT(5)
#define	WP_RF_PS_LEVEL_ALWAYS_ASPM	BIT(6)
#define	WP_RF_LPS_DISALBE_2R			BIT(30)
#define	WP_RF_LPS_LEVEL_ASPM			BIT(31)

#define	WP_IN_PS_LEVEL(ppsc, _PS_FLAG)		((ppsc->cur_ps_level & _PS_FLAG) ? _TRUE : _FALSE)
#define	WP_CLEAR_PS_LEVEL(ppsc, _PS_FLAG)	(ppsc->cur_ps_level &= (~(_PS_FLAG)))
#define	WP_SET_PS_LEVEL(ppsc, _PS_FLAG)		(ppsc->cur_ps_level |= _PS_FLAG)

enum _PS_BBRegBackup_ {
	PSBBREG_RF0 = 0,
	PSBBREG_RF1,
	PSBBREG_RF2,
	PSBBREG_AFE0,
	PSBBREG_TOTALCNT
};

typedef enum _PS_DENY_REASON {
	PS_DENY_DRV_INITIAL = 0,
	PS_DENY_SCAN,
	PS_DENY_JOIN,
	PS_DENY_DISCONNECT,
	PS_DENY_SUSPEND,
	PS_DENY_IOCTL,
	PS_DENY_MGNT_TX,
	PS_DENY_DRV_REMOVE = 30,
	PS_DENY_OTHERS = 31
} PS_DENY_REASON;


struct pwrctrl_priv {
	_pwrlock lock;
	_pwrlock check_32k_lock;
	volatile u8 rpwm;
	volatile u8 wfprs;
	volatile u8 tog;
	volatile u8 wfprs_tog;

	u8 pwr_mode;
	u8 smart_ps;
	u8 bcn_ant_mode;
	u8 dtim;

	u32 alives;

	u8 bpower_saving;

	u8 b_hw_radio_off;
	u8 reg_rfoff;
	u8 reg_pdnmode;
	u32 rfoff_reason;

	u32 cur_ps_level;
	u32 reg_rfps_level;

	uint lps_enter_cnts;
	uint lps_leave_cnts;

	uint bips_processing;
	u32 ips_deny_time;

	u32 ps_deny;

	u8 ps_processing;

	u8 fw_psmode_iface_id;
	u8 bLeisurePs;
	u8 LpsIdleCount;
	u8 power_mgnt;
	u8 org_power_mgnt;
	u8 bFwCurrentInPSMode;
	u8 bMailboxSync;
	u32 DelayLPSLastTimeStamp;
	s32 pnp_current_pwr_state;
	u8 pnp_bstop_trx;

	u8 bInternalAutoSuspend;
	u8 bInSuspend;
	u8 bSupportRemoteWakeup;
	u8 wowlan_wake_reason;
	u8 wowlan_ap_mode;
	u8 wowlan_mode;
	u8 wowlan_p2p_mode;
	u8 wowlan_pno_enable;
#ifdef CONFIG_WOWLAN
	u8 wowlan_txpause_status;
	u8 wowlan_pattern;
	u8 wowlan_pattern_idx;
	u64 wowlan_fw_iv;
	struct wltpriv_pattern patterns[MAX_WKFM_NUM];

#endif
	WDFTIMER pwr_state_check_timer;
	int pwr_state_check_interval;
	u8 pwr_state_check_cnts;

	int ps_flag;

	wp_rf_power_state rf_pwrstate;
	wp_rf_power_state change_rfpwrstate;

	u8 bHWPowerdown;
	u8 bHWPwrPindetect;
	u8 bkeepfwalive;
	u8 brfoffbyhw;
	unsigned long PS_BBRegBackup[PSBBREG_TOTALCNT];
};

#define WL_PWR_STATE_CHK_INTERVAL 2000

#define _wl_set_pwr_state_check_timer(pwrctl, ms) \
	do { \
 \
		_set_timer(&(pwrctl)->pwr_state_check_timer, (ms)); \
	} while(0)

#define wl_set_pwr_state_check_timer(pwrctl) \
	_wl_set_pwr_state_check_timer((pwrctl), (pwrctl)->pwr_state_check_interval)

extern void left_aps(PNIC Nic, u8 tag);
extern void left_aps_direct(PNIC Nic, u8 tag);

void do_ps_work(PNIC Nic, u8 tag);
void chk_pwr_stat_handler(WL_TIMER_HDL_ARGS);

#ifdef CONFIG_AUTOSUSPEND
int Func_Autoresume_Enter(PNIC Nic);
#endif

int query_state_fw_ps(PNIC Nic, u8 tag);

#ifdef CONFIG_LPS
void do_in_LPS(PNIC Nic, const char *msg, u8 tag);
void do_out_LPS(PNIC Nic, const char *msg, u8 tag);
void do_set_ps_mode(u8 smart_ps, u8 bcn_ant_mode, PNIC Nic,
					 u8 ps_mode, const char *msg);
void wl_set_fw_in_ips_mode(PNIC Nic, u8 enable);
#endif


#define chk_earlysuspend_registered(pwrpriv) _FALSE
#define chk_do_late_resume(pwrpriv) _FALSE
#define do_set_late_resume(pwrpriv, enable) do {} while (0)
#define do_register_bf_suspend(pwrpriv) do {} while (0)
#define do_unregister_bf_suspend(pwrpriv) do {} while (0)


int _pwr_wakeup(PNIC Nic, u32 ips_deffer_ms, const char *caller,
					u8 tag);
#define wl_pwr_wakeup(wadptdata) _pwr_wakeup(wadptdata, WL_PWR_STATE_CHK_INTERVAL, __FUNCTION__, 1)
#define wl_pwr_wakeup_ex(wadptdata, ips_deffer_ms) _pwr_wakeup(wadptdata, ips_deffer_ms, __FUNCTION__, 1)
int do_set_pm_lps(PNIC Nic, u8 mode, u8 tag);

void do_deny_ps(PNIC Nic, PS_DENY_REASON reason, u8 tag);
void do_cancel_deny_ps(PNIC Nic, PS_DENY_REASON reason, u8 tag);

#if defined(CONFIG_WOWLAN)
void do_query_sec_iv(PNIC Nic, u8 * pcur_dot11txpn, u8 * StaAddr,
					u8 tag);
void do_set_sec_pn(PNIC Nic);
bool do_chk_pattern_valid(u8 * input, u8 len, u8 tag);
bool wl_write_to_pkt_list(PNIC Nic, u8 idx,
							 struct wltwow_pattern *content, u8 tag);

bool wl_read_from_pkt_list(PNIC Nic, u8 idx, u8 tag);
void priv_pattern_prt(PNIC Nic, u8 idx, u8 tag);
void clean_pattern_work(PNIC Nic, u8 tag);
#endif
#endif
