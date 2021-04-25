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

#ifndef _WL_RECV_H_
#define _WL_RECV_H_

#ifdef CONFIG_SINGLE_RECV_BUF
#define NR_RECVBUFF (1)
#else
#ifdef CONFIG_PLATFORM_IPC
#ifdef CONFIG_LOWMEM
#define NR_RECVBUFF (8)
#else
#define NR_RECVBUFF (16)
#endif
#else
#define NR_RECVBUFF (8)
#endif
#endif
#ifdef CONFIG_PLATFORM_IPC
#ifdef CONFIG_LOWMEM
#define NR_PREALLOC_RECV_SKB 8
#else
#define NR_PREALLOC_RECV_SKB 16
#endif
#else
#define NR_PREALLOC_RECV_SKB 8
#endif

#ifdef CONFIG_LOWMEM
#define NR_RECVFRAME 128
#else
#define NR_RECVFRAME 256
#endif

#define RXFRAME_ALIGN	8
#define RXFRAME_ALIGN_SZ	(1<<RXFRAME_ALIGN)

#define DRVINFO_SZ	4

#define MAX_RXFRAME_CNT	512
#define MAX_RX_NUMBLKS		(32)
#define RECVFRAME_HDR_ALIGN 128
#define MAX_CONTINUAL_NORXPACKET_COUNT 1

#define PHY_RSSI_SLID_WIN_MAX				100
#define PHY_LINKQUALITY_SLID_WIN_MAX		20

#define SNAP_SIZE sizeof(struct ieee80211_snap_hdr)

#define RX_MPDU_QUEUE				0
#define RX_CMD_QUEUE				1
#define RX_MAX_QUEUE				2
#define CHAN2FREQ(a) ((a < 14)?(2407+5*a):(5000+5*a))

#ifndef IEEE80211_RADIOTAP_MCS
#define IEEE80211_RADIOTAP_MCS 19
#endif

#ifndef IEEE80211_RADIOTAP_F_BADFCS
#define IEEE80211_RADIOTAP_F_BADFCS 0x40
#endif

static u8 SNAP_ETH_TYPE_IPX[2] = { 0x81, 0x37 };

static u8 SNAP_ETH_TYPE_APPLETALK_AARP[2] = { 0x80, 0xf3 };
static u8 SNAP_ETH_TYPE_APPLETALK_DDP[2] = { 0x80, 0x9b };
static u8 SNAP_ETH_TYPE_TDLS[2] = { 0x89, 0x0d };
static u8 SNAP_HDR_APPLETALK_DDP[3] = { 0x08, 0x00, 0x07 };

static u8 oui_8021h[] = { 0x00, 0x00, 0xf8 };
static u8 oui_rfc1042[] = { 0x00, 0x00, 0x00 };

#define MAX_SUBFRAME_COUNT	64
static u8 wl_rfc1042_header[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 };
static u8 wl_bridge_tunnel_header[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8 };


struct recv_reorder_ctrl {
//	_wadptdata *pwadptdata;
	u8 enable;
	u16 indicate_seq;
	u16 wend_b;
	u8 wsize_b;
	u8 ampdu_size;
//	_queue pending_recvframe_queue;
//	_timer reordering_ctrl_timer;
	u8 bReorderWaiting;
};

struct stainfo_rxcache {
	u16 tid_rxseq[16];
};

struct smooth_rssi_data {
	u32 elements[100];
	u32 index;
	u32 total_num;
	u32 total_val;
};

struct signal_stat {
	u8 update_req;
	u8 avg_val;
	u32 total_num;
	u32 total_val;
};

struct phy_info {
	u8 RxPWDBAll;
	u8 SignalQuality;
	s8 RxMIMOSignalQuality[4];
	u8 RxMIMOEVMdbm[4];
	u8 RxMIMOSignalStrength[4];
	s16 Cfo_short[4];
	s16 Cfo_tail[4];
	s8 RxPower;
	s8 RecvSignalPower;
	u8 BTRxRSSIPercentage;
	u8 SignalStrength;
	s8 RxPwr[4];
	s8 RxSNR[4];
	u8 BandWidth;
	u8 btCoexPwrAdjust;
};

#ifdef DBG_RX_SIGNAL_DISPLAY_RAW_DATA
struct rx_raw_rssi {
	u8 data_rate;
	u8 pwdball;
	s8 pwr_all;

	u8 mimo_signal_strength[4];
	u8 mimo_signal_quality[4];

	s8 ofdm_pwr[4];
	u8 ofdm_snr[4];

};
#endif

struct rx_pkt_attrib {
	u16 pkt_len;
	u8 physt;
	u8 drvinfo_sz;
	u8 shift_sz;
	u8 hdrlen;
	u8 to_fr_ds;
	u8 amsdu;
	u8 qos;
	u8 priority;
	u8 pw_save;
	u8 mdata;
	u16 seq_num;
	u8 frag_num;
	u8 mfrag;
	u8 order;
	u8 privacy;
	u8 bdecrypted;
	u8 encrypt;
	u8 iv_len;
	u8 icv_len;
	u8 crc_err;
	u8 icv_err;

	u16 eth_type;

	u8 dst[ETH_ALEN];
	u8 src[ETH_ALEN];
	u8 ta[ETH_ALEN];
	u8 ra[ETH_ALEN];
	u8 bssid[ETH_ALEN];

	u8 ack_policy;

	u8 tcpchk_valid;
	u8 ip_chkrpt;
	u8 tcp_chkrpt;
	u8 key_index;

	u8 data_rate;
	u8 bw;
	u8 stbc;
	u8 ldpc;
	u8 sgi;
	enum RX_PACKET_TYPE pkt_rpt_type;
	u32 tsfl;
	u32 MacIDValidEntry[2];

	struct phy_info phy_info;
};

#define SN_LESS(a, b)		(((a-b)&0x800)!=0)
#define SN_EQUAL(a, b)	(a == b)
#define REORDER_WAIT_TIME	(50)

#define RECVBUFF_ALIGN_SZ 8

#define RXDESC_SIZE	24

#define RXDESC_OFFSET RXDESC_SIZE

struct recv_stat {
	unsigned int rxdw0;

	unsigned int rxdw1;

#ifndef BUF_DESC_ARCH
	unsigned int rxdw4;

	unsigned int rxdw5;

#endif
};

#define EOR BIT(30)

struct recv_priv {
//	_lock lock;

	_queue free_recv_queue;
	_queue recv_pending_queue;
	_queue uc_swdec_pending_queue;

	u8 *pallocated_frame_buf;
	u8 *precv_frame_buf;

	uint free_recvframe_cnt;

//	_wadptdata *wadptdata;

	u32 bIsAnyNonBEPkts;
	u64 rx_bytes;
	u64 rx_pkts;
	u64 rx_drop;

	uint rx_icv_err;
	uint rx_largepacket_crcerr;
	uint rx_smallpacket_crcerr;
	uint rx_middlepacket_crcerr;

//	_sema allrxreturnevt;
	uint ff_hwaddr;
//	ATOMIC_T rx_pending_cnt;

//	struct tasklet_struct irq_prepare_beacon_tasklet;
//	struct tasklet_struct recv_tasklet;
//	struct sk_buff_head free_recv_skb_queue;
//	struct sk_buff_head rx_skb_queue;

	u8 *pallocated_recv_buf;
	u8 *precv_buf;
	_queue free_recv_buf_queue;
	u32 free_recv_buf_queue_cnt;

	_queue recv_buf_pending_queue;

	u8 is_signal_dbg;
	u8 signal_strength_dbg;

	u8 signal_strength;
	u8 signal_qual;
	s8 rssi;
#ifdef DBG_RX_SIGNAL_DISPLAY_RAW_DATA
	struct rx_raw_rssi raw_rssi_info;
#endif
	s16 noise;

#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
//	_timer signal_stat_timer;
	u32 signal_stat_sampling_interval;
	struct signal_stat signal_qual_data;
	struct signal_stat signal_strength_data;
#else
	struct smooth_rssi_data signal_qual_data;
	struct smooth_rssi_data signal_strength_data;
#endif
	u16 sink_udpport, pre_rtp_rxseq, cur_rtp_rxseq;
};

#if 0
#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
#define wl_set_signal_stat_timer(recvpriv) _set_timer(&(recvpriv)->signal_stat_timer, (recvpriv)->signal_stat_sampling_interval)
#endif

struct sta_recv_priv {

	_lock lock;
	sint option;

	_queue defrag_q;

	struct stainfo_rxcache rxcache;

};

struct recv_buf {
	_list list;

	_lock recvbuf_lock;

	u32 ref_cnt;

	PwADPTDATA wadptdata;

	u8 *pbuf;
	u8 *pallocated_buf;

	u32 len;
	u8 *phead;
	u8 *pdata;
	u8 *ptail;
	u8 *pend;

	PURB purb;
	dma_addr_t dma_transfer_addr;
	u32 alloc_sz;

	u8 irp_pending;
	int transfer_len;

	_pkt *pskb;
};

struct recv_frame_hdr {
	_list list;
#ifndef CONFIG_BSD_RX_USE_MBUF
	struct sk_buff *pkt;
	struct sk_buff *pkt_newalloc;
#else
	_pkt *pkt;
	_pkt *pkt_newalloc;
#endif

	_wadptdata *wadptdata;

	u8 fragcnt;

	int frame_tag;

	struct rx_pkt_attrib attrib;

	uint len;
	u8 *rx_head;
	u8 *rx_data;
	u8 *rx_tail;
	u8 *rx_end;

	void *precvbuf;

	struct sta_info *psta;

	struct recv_reorder_ctrl *preorder_ctrl;

};

union recv_frame {

	union {
		_list list;
		struct recv_frame_hdr hdr;
		uint mem[RECVFRAME_HDR_ALIGN >> 2];
	} u;

};
#endif
typedef enum _RX_PACKET_TYPE {
	NORMAL_RX,
	TX_REPORT1,
	TX_REPORT2,
	HIS_REPORT,
	wMBOX0_PACKET
} RX_PACKET_TYPE, *PRX_PACKET_TYPE;
#if 0
extern sint _do_init_rx_priv(_wadptdata * pwadptdata,
								struct recv_priv *precvpriv, u8 tag);
extern void _wl_unnew_rx_priv(struct recv_priv *precvpriv, u8 tag);
extern s32 wl_rx_process(union recv_frame *precv_frame);
void do_init_rx_timer(struct recv_reorder_ctrl *preorder_ctrl);
int do_sr_new_rxpkt(_wadptdata * pwadptdata, u8 * pdata, _pkt * pskb,
						   union recv_frame *precvframe);
extern union recv_frame *wl_new_rxpkt(_queue * pfree_recv_queue, u8 tag);
extern int wl_unnew_rxpkt(union recv_frame *precvframe,
							  _queue * pfree_recv_queue, u8 tag);

#define wl_dequeue_recvframe(queue) wl_new_rxpkt(queue, 1)

u32 do_unnew_swdec_pending_queue(_wadptdata * wadptdata, u8 tag);

__inline static u8 *get_rxmem(union recv_frame *precvframe)
{
	if (precvframe == NULL)
		return NULL;

	return precvframe->u.hdr.rx_head;
}

__inline static u8 *get_rx_status(union recv_frame *precvframe)
{

	return get_rxmem(precvframe);

}

__inline static u8 *get_recvframe_data(union recv_frame *precvframe)
{

	if (precvframe == NULL)
		return NULL;

	return precvframe->u.hdr.rx_data;

}

__inline static u8 *recvframe_push(union recv_frame *precvframe, sint sz)
{

	if (precvframe == NULL)
		return NULL;

	precvframe->u.hdr.rx_data -= sz;
	if (precvframe->u.hdr.rx_data < precvframe->u.hdr.rx_head) {
		precvframe->u.hdr.rx_data += sz;
		return NULL;
	}

	precvframe->u.hdr.len += sz;

	return precvframe->u.hdr.rx_data;

}

__inline static u8 *recvframe_pull(union recv_frame *precvframe, sint sz)
{

	if (precvframe == NULL)
		return NULL;

	precvframe->u.hdr.rx_data += sz;

	if (precvframe->u.hdr.rx_data > precvframe->u.hdr.rx_tail) {
		precvframe->u.hdr.rx_data -= sz;
		return NULL;
	}

	precvframe->u.hdr.len -= sz;

	return precvframe->u.hdr.rx_data;

}

__inline static u8 *recvframe_put(union recv_frame *precvframe, sint sz)
{

	unsigned char *prev_rx_tail;

	if (precvframe == NULL)
		return NULL;

	prev_rx_tail = precvframe->u.hdr.rx_tail;

	precvframe->u.hdr.rx_tail += sz;

	if (precvframe->u.hdr.rx_tail > precvframe->u.hdr.rx_end) {
		precvframe->u.hdr.rx_tail -= sz;
		return NULL;
	}

	precvframe->u.hdr.len += sz;

	return precvframe->u.hdr.rx_tail;

}

__inline static u8 *recvframe_pull_tail(union recv_frame *precvframe, sint sz)
{

	if (precvframe == NULL)
		return NULL;

	precvframe->u.hdr.rx_tail -= sz;

	if (precvframe->u.hdr.rx_tail < precvframe->u.hdr.rx_data) {
		precvframe->u.hdr.rx_tail += sz;
		return NULL;
	}

	precvframe->u.hdr.len -= sz;

	return precvframe->u.hdr.rx_tail;

}

__inline static _buffer *get_rxbuf_desc(union recv_frame *precvframe)
{
	_buffer *buf_desc;

	if (precvframe == NULL)
		return NULL;

	return buf_desc;
}

__inline static union recv_frame *rxmem_to_recvframe(u8 * rxmem)
{

	return (union recv_frame *)(((SIZE_PTR) rxmem >> RXFRAME_ALIGN) <<
								RXFRAME_ALIGN);

}

__inline static union recv_frame *pkt_to_recvframe(_pkt * pkt)
{

	u8 *buf_star;
	union recv_frame *precv_frame;
	precv_frame = rxmem_to_recvframe((unsigned char *)buf_star);

	return precv_frame;
}

__inline static u8 *pkt_to_recvmem(_pkt * pkt)
{

	union recv_frame *precv_frame = pkt_to_recvframe(pkt);

	return precv_frame->u.hdr.rx_head;

}

__inline static u8 *pkt_to_recvdata(_pkt * pkt)
{

	union recv_frame *precv_frame = pkt_to_recvframe(pkt);

	return precv_frame->u.hdr.rx_data;

}

__inline static sint get_recvframe_len(union recv_frame *precvframe)
{
	return precvframe->u.hdr.len;
}

__inline static s32 translate_percentage_to_dbm(u32 SignalStrengthIndex)
{
	s32 SignalPower;

#ifdef CONFIG_SIGNAL_SCALE_MAPPING
	SignalPower = (s32) ((SignalStrengthIndex + 1) >> 1);
	SignalPower -= 95;
#else
	SignalPower = SignalStrengthIndex - 100;
#endif

	return SignalPower;
}

struct sta_info;

extern void _do_init_sta_rx_priv(struct sta_recv_priv *psta_recvpriv,
									u8 tag);

extern void dispose_mgt_subtype_func(_wadptdata * pwadptdata,
								union recv_frame *precv_frame, u8 flag);
#endif
#endif
static u8 signal_stat_calc_profile[2][2] = {
	{ 4, 1 },
	{ 3, 7 }
};
