#ifndef __MP_H__
#define __MP_H__

#include "common.h"

#include "mp_efuse.h"
#include "mp_trx.h"
#include "mp_reg.h"

#ifdef CONFIG_MP_MODE
#if defined(CONFIG_WEXT_PRIV)


typedef enum _MP_MODE_ {
	MP_OFF,
	MP_ON,
	MP_ERR,
	MP_CONTINUOUS_TX,
	MP_SINGLE_CARRIER_TX,
	MP_CARRIER_SUPPRISSION_TX,
	MP_SINGLE_TONE_TX,
	MP_PACKET_TX,
	MP_PACKET_RX,
	MP_TX_LCK,
	MP_MAC_LOOPBACK,
	MP_PHY_LOOPBACK,
} WF_MP_MODE;


typedef struct _wf_mp_info_st {
	wf_u32 mode;
	wf_u32 prev_fw_state;

	wf_u8 TID;
	wf_u32 tx_pktcount;
	wf_u32 pktInterval;
	wf_u32 pktLength;

    wf_u8  rx_start;
	wf_u32 rx_bssidpktcount;
	wf_u32 rx_pktcount;
	wf_u32 rx_pktcount_filter_out;
	wf_u32 rx_crcerrpktcount;
	wf_u32 rx_pktloss;

	wf_u8 channel;
	wf_u8 bandwidth;
	wf_u8 prime_channel_offset;
	wf_u8 txpoweridx;
	wf_u32 preamble;
	wf_u32 CrystalCap;

	wf_u16 antenna_tx;
	wf_u16 antenna_rx;

	wf_u8 check_mp_pkt;
    wf_u32 rateidx;
    
	wf_u8 bSetTxPower;
	wf_u8 mp_dm;
	wf_u8 mac_filter[WF_ETH_ALEN];
	wf_u8 bmac_filter;
	wf_u8 *pallocated_mp_xmitframe_buf;
	wf_u8 *pmp_xmtframe_buf;
	wf_u32 free_mp_xmitframe_cnt;
    wf_bool bStartContTx;
    wf_bool bCarrierSuppression;
	wf_bool bSingleTone;
    wf_bool bSetRxBssid;
    wf_bool rx_bindicatePkt;
    wf_bool bWLSmbCfg;
	wf_u8 *TXradomBuffer;

    wf_bool sta_connect_stats;
    wf_bool ap_connect_stats;

    wf_mp_tx tx;

    wf_80211_addr_t         network_macaddr;

    wf_u8         efuse_data_map[WF_EEPROM_MAX_SIZE];

	void * pnic_info;
    void *PktRxThread;

    wf_u32 sdio_ReceiveConfig;

}wf_mp_info_st;


typedef struct _wf_mp_antenna_select_ofdm {
	wf_u32 r_tx_antenna:4;
	wf_u32 r_ant_l:4;
	wf_u32 r_ant_non_ht:4;
	wf_u32 r_ant_ht1:4;
	wf_u32 r_ant_ht2:4;
	wf_u32 r_ant_ht_s1:4;
	wf_u32 r_ant_non_ht_s1:4;
	wf_u32 OFDM_TXSC:2;
	wf_u32 Reserved:2;
} wf_mp_antenna_select_ofdm;

typedef struct _wf_mp_antenna_select_cck {
	wf_u8 r_cckrx_enable_2:2;
	wf_u8 r_cckrx_enable:2;
	wf_u8 r_ccktx_enable:4;
} wf_mp_antenna_select_cck;

struct dbg_rx_counter {
	wf_u32 rx_pkt_ok;
	wf_u32 rx_pkt_crc_error;
	wf_u32 rx_pkt_drop;
	wf_u32 rx_ofdm_fa;
	wf_u32 rx_cck_fa;
	wf_u32 rx_ht_fa;
};




#define DRV_NAME "9188"
#define DRIVERVERSION "v1.1"
int wf_io_write_cmd_special(nic_info_st *nic_info, wf_u32 func_code, wf_u32 *recv,  int len, int offs);

int wf_mp(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra);

#define mcu_cmd_communicate_special wf_io_write_cmd_special

#endif
#endif

#endif
