/*
 * tx.h
 *
 * used for data frame xmit
 *
 * Author: renhaibo
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#ifndef __TX_H__
#define __TX_H__

#ifndef MAX_XMITBUF_SZ
#ifdef CONFIG_SOFT_TX_AGGREGATION
#define MAX_XMITBUF_SZ      (8*1024)
#else
#define MAX_XMITBUF_SZ      (2048)
#endif
#endif

#define MAX_AGG_NUM         (32)

#define TX_AGG_QUEUE_ENABLE (0)

#ifndef XMIT_DATA_BUFFER_CNT
#define XMIT_DATA_BUFFER_CNT (8)
#endif

#ifndef XMIT_MGMT_BUFFER_CNT
#define XMIT_MGMT_BUFFER_CNT (8)
#endif

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

/* CCK Rates, TxHT = 0 */
#define DESC_RATE1M					0x00
#define DESC_RATE2M					0x01
#define DESC_RATE5_5M				0x02
#define DESC_RATE11M				0x03

/* OFDM Rates, TxHT = 0 */
#define DESC_RATE6M					0x04
#define DESC_RATE9M					0x05
#define DESC_RATE12M				0x06
#define DESC_RATE18M				0x07
#define DESC_RATE24M				0x08
#define DESC_RATE36M				0x09
#define DESC_RATE48M				0x0A
#define DESC_RATE54M				0x0B

/* MCS Rates, TxHT = 1 */
#define DESC_RATEMCS0				0x0C
#define DESC_RATEMCS1				0x0D
#define DESC_RATEMCS2				0x0E
#define DESC_RATEMCS3				0x0F
#define DESC_RATEMCS4				0x10
#define DESC_RATEMCS5				0x11
#define DESC_RATEMCS6				0x12
#define DESC_RATEMCS7				0x13
#define DESC_RATEMCS8				0x14
#define DESC_RATEMCS9				0x15
#define DESC_RATEMCS10				0x16
#define DESC_RATEMCS11				0x17
#define DESC_RATEMCS12				0x18
#define DESC_RATEMCS13				0x19
#define DESC_RATEMCS14				0x1A
#define DESC_RATEMCS15				0x1B
#define DESC_RATEMCS16				0x1C
#define DESC_RATEMCS17				0x1D
#define DESC_RATEMCS18				0x1E
#define DESC_RATEMCS19				0x1F
#define DESC_RATEMCS20				0x20
#define DESC_RATEMCS21				0x21
#define DESC_RATEMCS22				0x22
#define DESC_RATEMCS23				0x23
#define DESC_RATEMCS24				0x24
#define DESC_RATEMCS25				0x25
#define DESC_RATEMCS26				0x26
#define DESC_RATEMCS27				0x27
#define DESC_RATEMCS28				0x28
#define DESC_RATEMCS29				0x29
#define DESC_RATEMCS30				0x2A
#define DESC_RATEMCS31				0x2B

#ifdef CONFIG_RICHV100
#define GET_RX_STATUS_DESC_PKT_LEN_9086X(__pRxStatusDesc)           wf_le_bits_to_u32( __pRxStatusDesc, 0, 14)
#define GET_RX_STATUS_DESC_CRC32_9086X(__pRxStatusDesc)             wf_le_bits_to_u32( __pRxStatusDesc, 14, 1)
#define GET_RX_STATUS_DESC_SECURITY_9086X(__pRxStatusDesc)          wf_le_bits_to_u32( __pRxStatusDesc, 20, 3)
#define GET_RX_STATUS_DESC_QOS_9086X(__pRxStatusDesc)               wf_le_bits_to_u32( __pRxStatusDesc, 23, 1)
#define GET_RX_STATUS_DESC_PHY_STATUS_9086X(__pRxStatusDesc)        wf_le_bits_to_u32( __pRxStatusDesc, 26, 1)

#define GET_RX_STATUS_DESC_TID_9086X(__pRxStatusDesc)               wf_le_bits_to_u32(__pRxStatusDesc +4,  0, 4)
#define GET_RX_STATUS_DESC_AMSDU_9086X(__pRxStatusDesc)             wf_le_bits_to_u32(__pRxStatusDesc +4,  4, 1)
#define GET_RX_STATUS_DESC_MORE_DATA_9086X(__pRxStatusDesc)         wf_le_bits_to_u32( __pRxStatusDesc+4,  5, 1)
#define GET_RX_STATUS_DESC_MORE_FRAG_9086X(__pRxStatusDesc)         wf_le_bits_to_u32( __pRxStatusDesc+4,  6, 1)

#define GET_RX_STATUS_DESC_SEQ_9086X(__pRxStatusDesc)               wf_le_bits_to_u32( __pRxStatusDesc+8, 0, 12)
#define GET_RX_STATUS_DESC_FRAG_9086X(__pRxStatusDesc)              wf_le_bits_to_u32( __pRxStatusDesc+8, 12, 4)
#define GET_RX_STATUS_DESC_RPT_SEL_9086X(__pRxStatusDesc)           wf_le_bits_to_u32( __pRxStatusDesc+8, 28, 1)

#ifdef CONFIG_USB_RX_AGGREGATION
#define GET_RX_STATUS_DESC_USB_AGG_PKTNUM_9086X(__pRxStatusDesc)    wf_le_bits_to_u32( __pRxStatusDesc+12, 16, 8)
#endif

#define GET_RX_STATUS_DESC_RXRATE_9086X(__pRxDesc)                  wf_le_bits_to_u32( __pRxDesc+16, 0, 6)

#define WF_TX_DESC_PKT_SIZE_9086X(__pTxDesc, __Value)               wf_set_bits_to_le_u32(__pTxDesc, 0, 16, __Value)

#define WF_TX_DESC_BMC_9086X(__pTxDesc, __Value)                    wf_set_bits_to_le_u32(__pTxDesc, 16, 1, __Value)

#define WF_TX_DESC_AGG_BREAK_9086X(__pTxDesc, __Value)              wf_set_bits_to_le_u32(__pTxDesc, 17, 1, __Value)

#define WF_TX_DESC_SPE_RPT_9086X(__pTxDesc, __Value)                wf_set_bits_to_le_u32(__pTxDesc, 18, 1, __Value)

#define WF_TX_DESC_DATARATE_FB_CFG_9086X(__pTxDesc, __Value)        wf_set_bits_to_le_u32(__pTxDesc, 25, 1, __Value)

#define WF_TX_DESC_RTSRATE_FB_CFG_9086X(__pTxDesc, __Value)         wf_set_bits_to_le_u32(__pTxDesc, 26, 1, __Value)

#define WF_TX_DESC_RTSRATE_CFG_9086X(__pTxDesc, __Value)            wf_set_bits_to_le_u32(__pTxDesc, 27, 1, __Value)

#define WF_TX_DESC_MINISPACE_CFG_9086X(__pTxDesc, __Value)          wf_set_bits_to_le_u32(__pTxDesc, 28, 1, __Value)

#define WF_TX_DESC_AGGNUM_CFG_9086X(__pTxDesc, __Value)             wf_set_bits_to_le_u32(__pTxDesc, 29, 1, __Value)

#define WF_TX_DESC_RSVD_9086X(__pTxDesc, __Value)                   wf_set_bits_to_le_u32(__pTxDesc, 30, 1, __Value)

#define WF_TX_DESC_AGG_ENABLE_9086X(__pTxDesc, __Value)             wf_set_bits_to_le_u32(__pTxDesc + 4, 0, 1, __Value)
#define WF_TX_DESC_SEC_TYPE_9086X(__pTxDesc, __Value)               wf_set_bits_to_le_u32(__pTxDesc + 4, 1, 3, __Value)

#define WF_TX_DESC_QUEUE_SEL_9086X(__pTxDesc, __Value)              wf_set_bits_to_le_u32(__pTxDesc + 4, 8, 5, __Value)
#define WF_TX_DESC_RETRY_LIMIT_ENABLE_9086X(__pTxDesc, __Value)     wf_set_bits_to_le_u32(__pTxDesc + 4, 14, 1, __Value)
#define WF_TX_DESC_USE_RATE_9086X(__pTxDesc, __Value)               wf_set_bits_to_le_u32(__pTxDesc + 4, 15, 1, __Value)

#define WF_TX_DESC_RATE_ID_9086X(__pTxDesc, __Value)                wf_set_bits_to_le_u32(__pTxDesc + 4, 16, 4, __Value)
#define WF_TX_DESC_RATE_ADP_9086X(__pTxDesc, __Value)               wf_set_bits_to_le_u32(__pTxDesc + 4, 20, 4, __Value)

#define WF_TX_DESC_MACID_9086X(__pTxDesc, __Value)                  wf_set_bits_to_le_u32(__pTxDesc + 4, 24, 7, __Value)

#define WF_TX_DESC_SW_DEFINE_9086X(__pTxDesc, __Value)              wf_set_bits_to_le_u32(__pTxDesc + 8, 0, 12, __Value)
#define WF_TX_DESC_HWSEQ_EN_9086X(__pTxDesc, __Value)               wf_set_bits_to_le_u32(__pTxDesc + 8, 12, 1, __Value)
#define WF_TX_DESC_SEQ_9086X(__pTxDesc, __Value)                    wf_set_bits_to_le_u32(__pTxDesc + 8, 13, 12, __Value)

#define WF_TX_DESC_DATA_RETRY_LIMIT_9086X(__pTxDesc, __Value)       wf_set_bits_to_le_u32(__pTxDesc + 8, 25, 2, __Value)

#define WF_TX_DESC_MBSSID_9086X(__pTxDesc, __Value)                 wf_set_bits_to_le_u32(__pTxDesc + 8, 27, 1, __Value)
#define WF_TX_DESC_CTS2SELF_9086X(__pTxDesc, __Value)               wf_set_bits_to_le_u32(__pTxDesc+ 8, 28, 1, __Value)
#define WF_TX_DESC_RTS_ENABLE_9086X(__pTxDesc, __Value)             wf_set_bits_to_le_u32(__pTxDesc+ 8, 29, 1, __Value)
#define WF_TX_DESC_HW_RTS_ENABLE_9086X(__pTxDesc, __Value)          wf_set_bits_to_le_u32(__pTxDesc+ 8, 30, 1, __Value)

#define WF_TX_DESC_USB_TXAGG_NUM_9086X(__pTxDesc, __Value)          wf_set_bits_to_le_u32(__pTxDesc+12, 8, 8, __Value)
#define WF_TX_DESC_TX_RATE_9086X(__pTxDesc, __Value)                wf_set_bits_to_le_u32(__pTxDesc+12, 16, 7, __Value)
#define WF_TX_DESC_DATA_BW_9086X(__pTxDesc, __Value)                wf_set_bits_to_le_u32(__pTxDesc+12, 24, 1, __Value)
#define WF_TX_DESC_DATA_SC_9086X(__pTxDesc, __Value)                wf_set_bits_to_le_u32(__pTxDesc + 12, 25, 2, __Value)
#define WF_TX_DESC_DATA_SHORT_9086X(__pTxDesc, __Value)             wf_set_bits_to_le_u32(__pTxDesc + 12, 27, 1, __Value)
#define WF_TX_DESC_RTS_SC_9086X(__pTxDesc, __Value)                 wf_set_bits_to_le_u32(__pTxDesc + 12, 28, 2, __Value)
#define WF_TX_DESC_RTS_SHORT_9086X(__pTxDesc, __Value)              wf_set_bits_to_le_u32(__pTxDesc + 12, 30, 1, __Value)

#define SET_TX_DESC_PKT_SIZE_9086X(__pTxDesc, __Value)              wf_set_bits_to_le_u32(__pTxDesc, 0, 16, __Value)
#define SET_TX_DESC_OFFSET_9086X(__pTxDesc, __Value)                wf_set_bits_to_le_u32(__pTxDesc, 16, 8, __Value)
#define SET_TX_DESC_LAST_SEG_9086X(__pTxDesc, __Value)              wf_set_bits_to_le_u32(__pTxDesc, 26, 1, __Value)
#define SET_TX_DESC_FIRST_SEG_9086X(__pTxDesc, __Value)             wf_set_bits_to_le_u32(__pTxDesc, 27, 1, __Value)
#define SET_TX_DESC_OWN_9086X(__pTxDesc, __Value)                   wf_set_bits_to_le_u32(__pTxDesc, 31, 1, __Value)

#define SET_TX_DESC_MACID_9086X(__pTxDesc, __Value)                 wf_set_bits_to_le_u32(__pTxDesc+4, 0, 7, __Value)
#define SET_TX_DESC_QUEUE_SEL_9086X(__pTxDesc, __Value)             wf_set_bits_to_le_u32(__pTxDesc+4, 8, 5, __Value)
#define SET_TX_DESC_RATE_ID_9086X(__pTxDesc, __Value)               wf_set_bits_to_le_u32(__pTxDesc+4, 16, 5, __Value)
#define SET_TX_DESC_SEC_TYPE_9086X(__pTxDesc, __Value)              wf_set_bits_to_le_u32(__pTxDesc+4, 22, 2, __Value)
#define SET_TX_DESC_PKT_OFFSET_9086X(__pTxDesc, __Value)            wf_set_bits_to_le_u32(__pTxDesc+4, 24, 5, __Value)

#define SET_TX_DESC_AGG_BREAK_9086X(__pTxDesc, __Value)             wf_set_bits_to_le_u32(__pTxDesc+8, 16, 1, __Value)
#define SET_TX_DESC_BT_INT_9086X(__pTxDesc, __Value)                wf_set_bits_to_le_u32(__pTxDesc+8, 23, 1, __Value)

#define SET_TX_DESC_HWSEQ_SEL_9086X(__pTxDesc, __Value)             wf_set_bits_to_le_u32(__pTxDesc+12, 6, 2, __Value)
#define SET_TX_DESC_USE_RATE_9086X(__pTxDesc, __Value)              wf_set_bits_to_le_u32(__pTxDesc+12, 8, 1, __Value)
#define SET_TX_DESC_DISABLE_FB_9086X(__pTxDesc, __Value)            wf_set_bits_to_le_u32(__pTxDesc+12, 10, 1, __Value)
#define SET_TX_DESC_NAV_USE_HDR_9086X(__pTxDesc, __Value)           wf_set_bits_to_le_u32(__pTxDesc+12, 15, 1, __Value)

#define SET_TX_DESC_TX_RATE_9086X(__pTxDesc, __Value)               wf_set_bits_to_le_u32(__pTxDesc+16, 0, 7, __Value)

#define SET_TX_DESC_DATA_SHORT_9086X(__pTxDesc, __Value)            wf_set_bits_to_le_u32(__pTxDesc+20, 4, 1, __Value)
#define SET_TX_DESC_DATA_BW_9086X(__pTxDesc, __Value)               wf_set_bits_to_le_u32(__pTxDesc+20, 5, 2, __Value)

#define SET_TX_DESC_HWSEQ_EN_9086X(__pTxDesc, __Value)              wf_set_bits_to_le_u32(__pTxDesc+32, 15, 1, __Value)

#define SET_TX_DESC_SEQ_9086X(__pTxDesc, __Value)                   wf_set_bits_to_le_u32(__pTxDesc+36, 12, 12, __Value)

#endif



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
	wf_u16 ether_type;
	wf_u8 icmp_pkt;
    wf_u16 con_len; //condition len
};

struct xmit_frame {
    wf_list_t list;

    wf_u16 ether_type;
    wf_bool bmcast;
    wf_u8 dhcp_pkt;
	wf_u8 icmp_pkt;
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
    wf_lock_t lock;
    wf_u64 tx_bytes;
    wf_u64 tx_pkts;
    wf_u64 tx_drop;
    wf_u64 tx_mgnt_pkts;

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
extern void wf_tx_frame_queue_clear(nic_info_st *nic_info);

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
