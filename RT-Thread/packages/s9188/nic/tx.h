#ifndef __TX_H__
#define __TX_H__

#ifdef CONFIG_SOFT_TX_AGGREGATION
#define MAX_XMITBUF_SZ      (16*1024)
#else
#define MAX_XMITBUF_SZ      (2048)
#endif
#define MAX_AGG_NUM         (32)

#define TX_AGG_QUEUE_ENABLE (0)

#define NR_XMITBUFF         (4)
#ifndef NR_XMITFRAME
#define NR_XMITFRAME        256
#endif

#ifdef USB_XMITBUF_ALIGN_SZ
#define XMITBUF_ALIGN_SZ    (USB_XMITBUF_ALIGN_SZ)
#else
#define XMITBUF_ALIGN_SZ    512
#endif

#define MAX_XMIT_EXTBUF_SZ  (1536)

#define MAX_CMDBUF_SZ       (5120)

#define MAX_NUMBLKS         (1)

#define XMIT_VO_QUEUE       (0)
#define XMIT_VI_QUEUE       (1)
#define XMIT_BE_QUEUE       (2)
#define XMIT_BK_QUEUE       (3)

#define VO_QUEUE_INX        0
#define VI_QUEUE_INX        1
#define BE_QUEUE_INX        2
#define BK_QUEUE_INX        3
#define BCN_QUEUE_INX       4
#define MGT_QUEUE_INX       5
#define HIGH_QUEUE_INX      6
#define TXCMD_QUEUE_INX     7
#define CMD_QUEUE_INX       8
#define READ_QUEUE_INX      9

#define QSLT_BEACON         0x8
#define QSLT_HIGH           0x9
#define QSLT_MGNT           0xA

#define AC0_IDX 0
#define AC1_IDX 1
#define AC2_IDX 2
#define AC3_IDX 3
#define AC4_IDX 4
#define AC5_IDX 5
#define ACX_IDX AC2_IDX


#define HW_QUEUE_ENTRY      8

#define SN_LESS(a, b)   (((a-b)&0x800)!=0)
#define SN_EQUAL(a, b)  (a == b)


#define WEP_IV(iv, dot11txpn, keyidx)\
do{\
    iv[0] = dot11txpn._byte_.TSC0;\
    iv[1] = dot11txpn._byte_.TSC1;\
    iv[2] = dot11txpn._byte_.TSC2;\
    iv[3] = ((keyidx & 0x3)<<6);\
    dot11txpn.val = (dot11txpn.val == 0xffffff) ? 0: (dot11txpn.val+1);\
}while(0)

#define TKIP_IV(iv, dot11txpn, keyidx)\
do{\
    iv[0] = dot11txpn._byte_.TSC1;\
    iv[1] = (dot11txpn._byte_.TSC1 | 0x20) & 0x7f;\
    iv[2] = dot11txpn._byte_.TSC0;\
    iv[3] = WF_BIT(5) | ((keyidx & 0x3)<<6);\
    iv[4] = dot11txpn._byte_.TSC2;\
    iv[5] = dot11txpn._byte_.TSC3;\
    iv[6] = dot11txpn._byte_.TSC4;\
    iv[7] = dot11txpn._byte_.TSC5;\
    dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0: (dot11txpn.val+1);\
}while(0)

#define AES_IV(iv, dot11txpn, keyidx)\
do{\
    iv[0] = dot11txpn._byte_.TSC0;\
    iv[1] = dot11txpn._byte_.TSC1;\
    iv[2] = 0;\
    iv[3] = BIT(5) | ((keyidx & 0x3)<<6);\
    iv[4] = dot11txpn._byte_.TSC2;\
    iv[5] = dot11txpn._byte_.TSC3;\
    iv[6] = dot11txpn._byte_.TSC4;\
    iv[7] = dot11txpn._byte_.TSC5;\
    dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0: (dot11txpn.val+1);\
}while(0)

#ifdef CONFIG_RICHV200
#ifndef TXDESC_OFFSET_NEW
#define TXDESC_OFFSET_NEW      20
#endif
#define TXDESC_SIZE            TXDESC_OFFSET_NEW
#define PACKET_OFFSET_SZ       0
#define TXDESC_OFFSET          TXDESC_OFFSET_NEW

#else

#define TXDESC_SIZE         40

#define PACKET_OFFSET_SZ    (0)
#define TXDESC_OFFSET       (TXDESC_SIZE + PACKET_OFFSET_SZ)

#endif

#define TXDESC_40_BYTES

struct tx_desc {
    wf_u32 txdw0;
    wf_u32 txdw1;
    wf_u32 txdw2;
    wf_u32 txdw3;
    wf_u32 txdw4;
    wf_u32 txdw5;
    wf_u32 txdw6;
    wf_u32 txdw7;

#if defined(TXDESC_40_BYTES) || defined(TXDESC_64_BYTES)
    wf_u32 txdw8;
    wf_u32 txdw9;
#endif

#ifdef TXDESC_64_BYTES
    wf_u32 txdw10;
    wf_u32 txdw11;

    wf_u32 txdw12;
    wf_u32 txdw13;
    wf_u32 txdw14;
    wf_u32 txdw15;
#endif
};

union txdesc {
    struct tx_desc txdesc;
    wf_u32 value[TXDESC_SIZE >> 2];
};

#define WLANHDR_OFFSET      64

#define NULL_FRAMETAG       (0x0)
#define DATA_FRAMETAG       0x01
#define L2_FRAMETAG         0x02
#define MGNT_FRAMETAG       0x03
#define AMSDU_FRAMETAG      0x04

#define EII_FRAMETAG        0x05
#define IEEE8023_FRAMETAG   0x06

#define MP_FRAMETAG         0x07

#define TXAGG_FRAMETAG      0x08

struct xmit_buf {
    wf_list_t list;
    struct nic_info *nic_info;
    wf_u8 *pallocated_buf;
    wf_u8 *pbuf;
    wf_u8 *ptail;
    wf_u8 agg_num;
    wf_u16 pg_num;
    void *priv_data;
    wf_u16 flags;
    wf_u32 alloc_sz;
    wf_u32 ff_hwaddr;
    wf_u8 wlan_hdr_len;
    wf_u8 iv_len;
    wf_u8 icv_len;
    wf_u8 privacy;
    wf_u8 encrypt_algo;
    wf_u16 pkt_len;
    wf_u8 send_flag;
    wf_u8  buffer_id;
    wf_timer_t time;
    wf_u8 qsel;
    wf_u16 con_len; //condition len
};

struct xmit_frame {
    wf_list_t list;

    wf_u16 ether_type;
    wf_bool bmcast;
    wf_u8 dhcp_pkt;
    wf_u16 seqnum;
    wf_u16 pkt_hdrlen;
    wf_u16 hdrlen;
    wf_u32 pktlen;
    wf_u32 last_txcmdsz;
    wf_u8 nr_frags;
    wf_u8 encrypt_algo;
    wf_u8 bswenc;
    wf_u8 iv_len;
    wf_u8 icv_len;
    wf_u8 iv[18];
    wf_u8 icv[16];
    wf_u8 priority;
    wf_u8 qsel;
    wf_u8 ampdu_en;
    wf_u8 vcs_mode;
    wf_u8 key_idx;
    wf_u8 ht_en;

    union Keytype dot11tkiptxmickey;
    union Keytype dot118021x_UncstKey;

    wf_80211_data_t *pwlanhdr;
    wdn_net_info_st *pwdn;

    void *pkt;

    int frame_tag;

    struct nic_info *nic_info;

    wf_u8 *buf_addr;

    struct xmit_buf *pxmitbuf;

    wf_u8 agg_num;
    wf_s8 pkt_offset;
    wf_u16 frame_id;
#ifdef CONFIG_XMIT_ACK
    wf_u8 ack_report;
#endif

};

enum cmdbuf_type {
 CMDBUF_BEACON = 0x00,
 CMDBUF_RSVD,
 CMDBUF_MAX
};

typedef struct tx_info {
    wf_lock_spin lock;
    wf_u64 tx_bytes;
    wf_u64 tx_pkts;
    wf_u64 tx_drop;
    wf_u64 tx_mgnt_pkts;
    // wf_u64 tx_data_pkts;
    // wf_u64 last_tx_pkts;

    wf_u8 xmitFrameCtl;

    // used to stop or restart send to hif queue
    wf_u16 xmit_stop_flag;   

    /* define allocated xmit_frame memory */
    wf_u8 *pallocated_frame_buf;
    wf_u8 *pxmit_frame_buf;
    wf_s32 free_xmitframe_cnt;
    wf_que_t xmit_frame_queue;
    wf_que_t agg_frame_queue;

    /* define allocated xmit_buf memory */
    wf_u8 *pallocated_xmitbuf;
    wf_u8 *pxmitbuf;
    wf_u32 free_xmitbuf_cnt;
    wf_que_t xmit_buf_queue;

    /* define mgmt frame allocated xmit_buf memory */
    wf_u8 *pallocated_xmit_extbuf;
    wf_u8 *pxmit_extbuf;
    wf_u32 free_xmit_extbuf_cnt;
    wf_que_t xmit_extbuf_queue;

    /* pending queue */
    wf_lock_t pending_lock;
    wf_u32 pending_frame_cnt;
    wf_que_t pending_frame_queue;

    nic_info_st *nic_info;

    // /* define the ip packet qos count */
    // int beq_cnt;
    // int bkq_cnt;
    // int viq_cnt;
    // int voq_cnt;

    // wf_u32 tx_accnt[10];

#ifdef CONFIG_LPS
    struct xmit_buf pcmd_xmitbuf[CMDBUF_MAX]; // LPS need
#endif

    /* define other needed  */
#ifdef CONFIG_XMIT_ACK
    int ack_tx;
    wf_lock_mutex ack_tx_mutex;
    struct submit_ctx ack_tx_ops;
    wf_u8 seq_no;
#endif
} tx_info_st;

extern wf_u8 wf_ra_sGI_get(wdn_net_info_st *pwdn, wf_u8 pad);
extern wf_u8 wf_chk_qos(wf_u8 acm_mask, wf_u8 priority, wf_u8 pad);
extern wf_bool wf_need_stop_queue(nic_info_st *nic_info);
extern wf_bool wf_need_wake_queue(nic_info_st *nic_info);
extern int wf_tx_info_init(nic_info_st *nic_info);
extern int wf_tx_info_term(nic_info_st *nic_info);
extern struct xmit_buf *wf_xmit_buf_new(tx_info_st *tx_info);
extern wf_bool wf_xmit_buf_delete(tx_info_st *tx_info, struct xmit_buf *pxmitbuf);
extern struct xmit_buf *wf_xmit_extbuf_new(tx_info_st *tx_info);
extern wf_bool wf_xmit_extbuf_delete(tx_info_st *tx_info, struct xmit_buf *pxmitbuf);
extern struct xmit_frame *wf_xmit_frame_new(tx_info_st *tx_info);
extern wf_bool wf_xmit_frame_delete(tx_info_st *tx_info, struct xmit_frame * pxmitframe);
extern wf_bool wf_tx_data_check(nic_info_st *nic_info);
extern int wf_tx_msdu(nic_info_st *nic_info, wf_u8 *msdu_buf, int msdu_len, void *pkt);
extern int wf_nic_beacon_xmit(nic_info_st *nic_info, struct xmit_buf *pxmitbuf, wf_u16 len);
extern int wf_nic_mgmt_frame_xmit(nic_info_st *nic_info, wdn_net_info_st *wdn, struct xmit_buf *pxmitbuf, wf_u16 len);
extern int wf_nic_mgmt_frame_xmit_with_ack(nic_info_st *nic_info, wdn_net_info_st *wdn, struct xmit_buf *pxmitbuf, wf_u16 len);
extern int wf_tx_pending_frame_xmit(nic_info_st *nic_info);

extern struct xmit_frame *wf_tx_data_getqueue(tx_info_st *tx_info);
extern struct xmit_frame *wf_tx_data_dequeue(tx_info_st *tx_info);
extern void wf_tx_data_enqueue_tail(tx_info_st *tx_info, struct xmit_frame *pxmitframe);
extern void wf_tx_data_enqueue_head(tx_info_st *tx_info, struct xmit_frame *pxmitframe);
extern wf_bool wf_xmit_frame_enqueue(tx_info_st *tx_info, struct xmit_frame * pxmitframe);

extern void wf_tx_agg_enqueue_head(tx_info_st *tx_info, struct xmit_frame *pxmitframe);
extern struct xmit_frame *wf_tx_agg_dequeue(tx_info_st *tx_info);

extern wf_u32 wf_get_wlan_pkt_size(struct xmit_frame *pxmitframe);

#ifdef CONFIG_LPS
extern struct xmit_frame* wf_xmit_cmdframe_new(tx_info_st *tx_info, enum cmdbuf_type buf_type, u8 tag);
#endif

#ifdef CONFIG_SOFT_TX_AGGREGATION
extern void wf_tx_agg_num_fill(wf_u16 agg_num, wf_u8 * pbuf);
extern wf_u32 wf_nic_get_tx_max_len(nic_info_st *nic_info, struct xmit_frame *pxmitframe);
extern int wf_nic_tx_qsel_check(wf_u8 pre_qsel, wf_u8 next_qsel);
#endif
#ifdef CONFIG_RICHV200
void wf_txdesc_chksum(wf_u8 *ptx_desc);
#else
void wf_txdesc_chksum(struct tx_desc *ptxdesc);
#endif
wf_u32 wf_quary_addr(wf_u8 qsel);
wf_u8 wf_mrate_to_hwrate(wf_u8 rate);
wf_u8 wf_hwrate_to_mrate(wf_u8 rate);

wf_bool wf_tx_txdesc_init(struct xmit_frame *pxmitframe, wf_u8 * pmem, wf_s32 sz,
                           wf_bool bagg_pkt, wf_u8 dum);

wf_bool wf_xmit_frame_init(nic_info_st *nic_info, struct xmit_frame *pxmitframe, wf_u8 *msdu_buf, int msdu_len);

wf_bool wf_tx_msdu_to_mpdu(nic_info_st *nic_info, struct xmit_frame *pxmitframe, wf_u8 *msdu_buf, int msdu_len);
void wf_tx_stats_cnt(nic_info_st *nic_info, struct xmit_frame *pxmitframe, int sz);
void wf_tx_xmit_stop(nic_info_st *nic_info);
void wf_tx_xmit_start(nic_info_st *nic_info);
void wf_tx_xmit_pending_queue_clear(nic_info_st *nic_info);
int wf_tx_xmit_hif_queue_empty(nic_info_st *nic_info);

#define WF_PAGE_NUM(_Len, _Size) (wf_u32)(((_Len)/(_Size)) + ((_Len)&((_Size) - 1) ? 1:0))

#endif
