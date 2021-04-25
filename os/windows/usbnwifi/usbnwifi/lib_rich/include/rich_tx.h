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

#ifndef _WL_XMIT_H_
#define _WL_XMIT_H_

#ifdef CONFIG_USB_TX_AGGREGATION
#define MAX_XMITBUF_SZ	(20480)
#else
#define MAX_XMITBUF_SZ	(2048)
#endif

#define NR_XMITBUFF	(4)
#define NR_XMITFRAME    256 

#ifdef USB_XMITBUF_ALIGN_SZ
#define XMITBUF_ALIGN_SZ (USB_XMITBUF_ALIGN_SZ)
#else
#define XMITBUF_ALIGN_SZ 512
#endif

#define MAX_XMIT_EXTBUF_SZ	(1536)

#define NR_XMIT_EXTBUFF	(32)

#define MAX_CMDBUF_SZ	(5120)

#define MAX_NUMBLKS		(1)

#define XMIT_VO_QUEUE (0)
#define XMIT_VI_QUEUE (1)
#define XMIT_BE_QUEUE (2)
#define XMIT_BK_QUEUE (3)

#define VO_QUEUE_INX		0
#define VI_QUEUE_INX		1
#define BE_QUEUE_INX		2
#define BK_QUEUE_INX		3
#define BCN_QUEUE_INX		4
#define MGT_QUEUE_INX		5
#define HIGH_QUEUE_INX		6
#define TXCMD_QUEUE_INX	7

#define HW_QUEUE_ENTRY	8

#define WEP_IV(pattrib_iv, dot11txpn, keyidx)\
do{\
	pattrib_iv[0] = dot11txpn._byte_.TSC0;\
	pattrib_iv[1] = dot11txpn._byte_.TSC1;\
	pattrib_iv[2] = dot11txpn._byte_.TSC2;\
	pattrib_iv[3] = ((keyidx & 0x3)<<6);\
	dot11txpn.val = (dot11txpn.val == 0xffffff) ? 0: (dot11txpn.val+1);\
}while(0)

#define TKIP_IV(pattrib_iv, dot11txpn, keyidx)\
do{\
	pattrib_iv[0] = dot11txpn._byte_.TSC1;\
	pattrib_iv[1] = (dot11txpn._byte_.TSC1 | 0x20) & 0x7f;\
	pattrib_iv[2] = dot11txpn._byte_.TSC0;\
	pattrib_iv[3] = BIT(5) | ((keyidx & 0x3)<<6);\
	pattrib_iv[4] = dot11txpn._byte_.TSC2;\
	pattrib_iv[5] = dot11txpn._byte_.TSC3;\
	pattrib_iv[6] = dot11txpn._byte_.TSC4;\
	pattrib_iv[7] = dot11txpn._byte_.TSC5;\
	dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0: (dot11txpn.val+1);\
}while(0)

#define AES_IV(pattrib_iv, dot11txpn, keyidx)\
do{\
	pattrib_iv[0] = dot11txpn._byte_.TSC0;\
	pattrib_iv[1] = dot11txpn._byte_.TSC1;\
	pattrib_iv[2] = 0;\
	pattrib_iv[3] = BIT(5) | ((keyidx & 0x3)<<6);\
	pattrib_iv[4] = dot11txpn._byte_.TSC2;\
	pattrib_iv[5] = dot11txpn._byte_.TSC3;\
	pattrib_iv[6] = dot11txpn._byte_.TSC4;\
	pattrib_iv[7] = dot11txpn._byte_.TSC5;\
	dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0: (dot11txpn.val+1);\
}while(0)

#define HWXMIT_ENTRY	4

#define TXDESC_SIZE 	40

#define PACKET_OFFSET_SZ (0)
#define TXDESC_OFFSET (TXDESC_SIZE + PACKET_OFFSET_SZ)

enum TXDESC_SC {
	SC_DONT_CARE = 0x00,
	SC_UPPER = 0x01,
	SC_LOWER = 0x02,
	SC_DUPLICATE = 0x03
};

#define TXDESC_40_BYTES

struct tx_desc {
	unsigned int txdw0;
	unsigned int txdw1;
	unsigned int txdw2;
	unsigned int txdw3;
	unsigned int txdw4;
	unsigned int txdw5;
	unsigned int txdw6;
	unsigned int txdw7;

#if defined(TXDESC_40_BYTES) || defined(TXDESC_64_BYTES)
	unsigned int txdw8;
	unsigned int txdw9;
#endif

#ifdef TXDESC_64_BYTES
	unsigned int txdw10;
	unsigned int txdw11;

	unsigned int txdw12;
	unsigned int txdw13;
	unsigned int txdw14;
	unsigned int txdw15;
#endif
};

union txdesc {
	struct tx_desc txdesc;
	unsigned int value[TXDESC_SIZE >> 2];
};

struct hw_xmit {
	_queue *sta_queue;
	int accnt;
};

struct pkt_attrib {
	u8 type;
	u8 subtype;
	u8 bswenc;
	u8 dhcp_pkt;
	u16 ether_type;
	u16 seqnum;
	u8 hw_ssn_sel;
	u16 pkt_hdrlen;
	u16 hdrlen;
	u32 pktlen;
	u32 last_txcmdsz;
	u8 nr_frags;
	u8 encrypt;
	u8 iv_len;
	u8 icv_len;
	u8 iv[18];
	u8 icv[16];
	u8 priority;
	u8 ack_policy;
	u8 mac_id;
	u8 vcs_mode;
	u8 dst[ETH_ALEN];
	u8 src[ETH_ALEN];
	u8 ta[ETH_ALEN];
	u8 ra[ETH_ALEN];
	u8 key_idx;
	u8 qos_en;
	u8 ht_en;
	u8 raid;
	u8 bwmode;
	u8 ch_offset;
	u8 sgi;
	u8 ampdu_en;
	u8 ampdu_spacing;
	u8 mdata;
	u8 pctrl;
	u8 triggered;
	u8 qsel;
	u8 order;
	u8 eosp;
	u8 rate;
	u8 intel_proxim;
	u8 retry_ctrl;
	u8 mbssid;
	u8 ldpc;
	u8 stbc;
	struct sta_info *psta;

	u8 rtsen;
	u8 cts2self;
	union Keytype dot11tkiptxmickey;
	union Keytype dot118021x_UncstKey;

	u8 key_type;

	u8 icmp_pkt;

	u8 rsvd_packet;
};

#define WLANHDR_OFFSET	64

#define NULL_FRAMETAG		(0x0)
#define DATA_FRAMETAG		0x01
#define L2_FRAMETAG		0x02
#define MGNT_FRAMETAG		0x03
#define AMSDU_FRAMETAG	0x04

#define EII_FRAMETAG		0x05
#define IEEE8023_FRAMETAG  0x06

#define MP_FRAMETAG		0x07

#define TXAGG_FRAMETAG 	0x08

enum {
	XMITBUF_DATA = 0,
	XMITBUF_MGNT = 1,
	XMITBUF_CMD = 2,
};


struct submit_ctx {
	u32 submit_time;               //Get_Current_Time when submit 
	u32 timeout_ms;                //Set tx timeout 
	int status;                    //WL_SCTX_DONE_SUCCESS or others
//	struct completion done;
//  need to change
};

enum {
	WL_SCTX_SUBMITTED = -1,
	WL_SCTX_DONE_SUCCESS = 0,
	WL_SCTX_DONE_UNKNOWN,
	WL_SCTX_DONE_TIMEOUT,
	WL_SCTX_DONE_BUF_ALLOC,
	WL_SCTX_DONE_BUF_FREE,
	WL_SCTX_DONE_WRITE_PORT_ERR,
	WL_SCTX_DONE_TX_DESC_NA,
	WL_SCTX_DONE_TX_DENY,
	WL_SCTX_DONE_CCX_PKT_FAIL,
	WL_SCTX_DONE_DRV_STOP,
	WL_SCTX_DONE_DEV_REMOVE,
	WL_SCTX_DONE_CMD_ERROR,
};

void do_init_sctx(struct submit_ctx *sctx, int timeout_ms, u8 tag);
int wl_sctx_wait(struct submit_ctx *sctx, const char *msg, u8 tag);
void do_stx_done_err(struct submit_ctx **sctx, int status, u8 tag);

#if 0
struct xmit_buf {
	LIST_ENTRY list;
//	_wadptdata *pwadptdata;
	u8 *pallocated_buf;
	u8 *pbuf;
	void *priv_data;
	u16 buf_tag;
	u16 flags;
	u32 alloc_sz;
	u32 len;
	struct submit_ctx *sctx;
	u32 ff_hwaddr;
//	PURB pxmit_urb[8];
	dma_addr_t dma_transfer_addr;
	u8 bpending[8];
	sint last[8];

};
#endif
struct xmit_frame {
	LIST_ENTRY list;

	struct pkt_attrib attrib;

//	_pkt *pkt;

	int frame_tag;

//	_wadptdata *pwadptdata;

	u8 *buf_addr;

//	struct xmit_buf *pxmitbuf;

#ifdef CONFIG_USB_TX_AGGREGATION
	u8 agg_num;
#endif
	s8 pkt_offset;

#ifdef CONFIG_XMIT_ACK
	u8 ack_report;
#endif

	u8 *alloc_addr;
	u8 ext_tag;

};

struct tx_servq {
	LIST_ENTRY tx_pending;
	_queue sta_pending;
	int qcnt;
};

struct sta_xmit_priv {
	KSPIN_LOCK   lock;
	sint option;
	sint apsd_setting;

	struct tx_servq be_q;
	struct tx_servq bk_q;
	struct tx_servq vi_q;
	struct tx_servq vo_q;
	LIST_ENTRY legacy_dz;
	LIST_ENTRY apsd;

	u16 txseq_tid[16];

};

struct hw_txqueue {
	volatile sint head;
	volatile sint tail;
	volatile sint free_sz;
	volatile sint free_cmdsz;
	volatile sint txsz[8];
	uint ff_hwaddr;
	uint cmd_hwaddr;
	sint ac_tag;
};

struct agg_pkt_info {
	u16 offset;
	u16 pkt_len;
};

enum cmdbuf_type {
	CMDBUF_BEACON = 0x00,
	CMDBUF_RSVD,
	CMDBUF_MAX
};

struct xmit_priv {

	KSPIN_LOCK   lock;

	KSEMAPHORE xmit_sema;
	KSEMAPHORE terminate_xmitthread_sema;

	_queue be_pending;
	_queue bk_pending;
	_queue vi_pending;
	_queue vo_pending;
	_queue bm_pending;

	u8 *pallocated_frame_buf;
	u8 *pxmit_frame_buf;
	uint free_xmitframe_cnt;
	_queue free_xmit_queue;

	u8 *xframe_ext_alloc_addr;
	u8 *xframe_ext;
	uint free_xframe_ext_cnt;
	_queue free_xframe_ext_queue;

	uint frag_len;

//	_wadptdata *wadptdata;

	u8 vcs_setting;
	u8 vcs;
	u8 vcs_type;

	u64 tx_bytes;
	u64 tx_pkts;
	u64 tx_drop;
	u64 last_tx_pkts;

	struct hw_xmit *hwxmits;
	u8 hwxmit_entry;

	u8 wmm_para_seq[4];

	KSEMAPHORE tx_retevt;
	u8 txirp_cnt;

//	struct tasklet_struct xmit_tasklet;
	int beq_cnt;
	int bkq_cnt;
	int viq_cnt;
	int voq_cnt;

	_queue free_xmitbuf_queue;
	_queue pending_xmitbuf_queue;
	u8 *pallocated_xmitbuf;
	u8 *pxmitbuf;
	uint free_xmitbuf_cnt;

	_queue free_xmit_extbuf_queue;
	u8 *pallocated_xmit_extbuf;
	u8 *pxmit_extbuf;
	uint free_xmit_extbuf_cnt;

//	struct xmit_buf pcmd_xmitbuf[CMDBUF_MAX];
	u8 hw_ssn_seq_no;
	u16 nqos_ssn;

#ifdef CONFIG_XMIT_ACK
	int ack_tx;
	NDIS_MUTEX ack_tx_mutex;
	struct submit_ctx ack_tx_ops;
	u8 seq_no;
#endif
	KSPIN_LOCK   lock_sctx;
};

#if 0
struct pkt_file {
	_pkt *pkt;
	SIZE_T pkt_len;
	_buffer *cur_buffer;
	u8 *buf_start;
	u8 *cur_addr;
	SIZE_T buf_len;
};

extern int _wl_do_tx_entry(_nic_hdl pnetdev, _pkt * pkt);
extern int wl_do_tx_entry(_pkt * pkt, _nic_hdl pnetdev);

void do_tx_schedule(_wadptdata * pwadptdata, u8 tag);

extern struct xmit_frame *__wnew_cmdtxp(struct xmit_priv *pxmitpriv,
												   enum cmdbuf_type buf_type,
												   u8 tag);
#define wl_alloc_cmdxmitframe(p) __wnew_cmdtxp(p, CMDBUF_RSVD, 1)

#define wl_alloc_bcnxmitframe(p) __wnew_cmdtxp(p, CMDBUF_BEACON, 1)

extern struct xmit_buf *wnew_txbuf_ext(struct xmit_priv *pxmitpriv,
											  u8 tag);

extern struct xmit_buf *wnew_txbuf(struct xmit_priv *pxmitpriv, u8 tag);
extern s32 unnew_txbuf_func(struct xmit_priv *pxmitpriv,
							struct xmit_buf *pxmitbuf, u8 tag);

extern struct xmit_frame *wnew_txp(struct xmit_priv *pxmitpriv,
											  u8 tag);
extern void do_init_txp(struct xmit_frame *pxframe, u8 tag);
extern s32 unnew_txp_func(struct xmit_priv *pxmitpriv,
							  struct xmit_frame *pxmitframe, u8 tag);

#ifdef CONFIG_IEEE80211W
extern s32 do_mgmt_txp_coa(struct xmit_frame *pxmitframe,
									   _wadptdata * pwadptdata, _pkt * pkt, u8 tag);
#endif
s32 _wl_init_hw_txqueue(struct hw_txqueue *phw_txqueue, u8 ac_tag);

s32 _init_tx_priv(struct xmit_priv *pxmitpriv, _wadptdata * pwadptdata,
						u8 be);
void _unnew_tx_priv_func(struct xmit_priv *pxmitpriv, u8 pad);

bool txp_hfilter(struct xmit_frame *xmitframe, u8 tag);
void quit_sta_tx_(struct sta_info *psta, _wadptdata * pwadptdata, u8 tag);
void wait_to_tx(struct sta_info *psta, _wadptdata * pwadptdata, u8 tag);

u8 ra_sGI_get(struct sta_info *psta, u8 pad);

u8 chk_qos(u8 acm_mask, u8 priority, u8 pad);

void wl_finish_ack_tx(int status, struct xmit_priv *pxmitpriv, u8 tag);
#endif
#endif
