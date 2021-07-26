#ifndef __RX_H__
#define __RX_H__

#define RX_REORDER_ENABLE (1)

#define ETHERNET_HEADER_SIZE                14  /* A-MSDU header ï¼? DA(6)+SA(6)+Length(2) */
#define LLC_HEADER_SIZE                     6

#ifdef CONFIG_RICHV200
#define HIF_HDR_LEN 56
#else
#define HIF_HDR_LEN 56
#endif // CONFIG_RICHV200

#define ETH_P_IP    0x0800
#define ETH_P_ARP   0x0806
#define ETH_P_ATALK 0x809B
#define ETH_P_AARP  0x80F3
#define ETH_P_8021Q 0x8100
#define ETH_P_IPX   0x8137
#define ETH_P_IPV6  0x86DD
#define ETH_P_PPP_DISC  0x8863
#define ETH_P_PPP_SES   0x8864

#define IPV4_SRC(_iphdr)            (((wf_u8 *)(_iphdr)) + 12)
#define IPV4_DST(_iphdr)            (((wf_u8 *)(_iphdr)) + 16)
#define GET_IPV4_IHL(_iphdr)        wf_be_bits_to_u8(((wf_u8 *)(_iphdr)) + 0, 0, 4)
#define GET_IPV4_PROTOCOL(_iphdr)   wf_be_bits_to_u8(((wf_u8 *)(_iphdr)) + 9, 0, 8)

#define GET_UDP_SRC(_udphdr)            wf_be_bits_to_u16(((wf_u8 *)(_udphdr)) + 0, 0, 16)
#define GET_UDP_DST(_udphdr)            wf_be_bits_to_u16(((wf_u8 *)(_udphdr)) + 2, 0, 16)

#define TCP_SRC(_tcphdr)                (((wf_u8 *)(_tcphdr)) + 0)
#define TCP_DST(_tcphdr)                (((wf_u8 *)(_tcphdr)) + 2)
#define GET_TCP_FIN(_tcphdr)            wf_be_bits_to_u8(((wf_u8 *)(_tcphdr)) + 13, 0, 1)
#define GET_TCP_SYN(_tcphdr)            wf_be_bits_to_u8(((wf_u8 *)(_tcphdr)) + 13, 1, 1)
#define GET_TCP_ACK(_tcphdr)            wf_be_bits_to_u8(((wf_u8 *)(_tcphdr)) + 13, 4, 1)
typedef enum {
    STA_TO_STA = 0,
    STA_TO_DS  = 1,
    DS_TO_STA  = 2,
    MESH_TO_MESH = 3,
} To_From_Ds_t;

#ifndef BITS
#define BITS(m,n)   (~(BIT(m)-1) & ((BIT(n)-1) | BIT(n)))
#endif

#ifndef BIT
#define BIT(x)  (1 << (x))
#endif

#define CHAN2FREQ(a) ((a < 14)?(2407+5*a):(5000+5*a))
#define PKT_TO_NIC_INFO(ppkt)   (nic_info_st *)(ppkt->p_nic_info)

#ifndef MAX_RXBUF_SZ
#define MAX_RXBUF_SZ (32768)
#endif

#ifndef NR_RECVBUFF
#define NR_RECVBUFF 8
#endif

#ifdef CONFIG_RICHV200

#ifndef RXD_SIZE
#define RXD_SIZE    24
#endif

#else

#ifndef RXD_SIZE
    #ifndef LONG_TXD
    #define RXD_SIZE    24 //12
#else
#define RXD_SIZE    24
#endif
#endif

#endif // CONFIG_RICHV200

#define MAX_PKT_NUM     20
//#define MAC_ADDR_LEN  6

#define CFG_COPY_DATA_IN_COMPLETE_FUNC  1

enum phydm_ctrl_info_rate {
    ODM_RATE1M          = 0x00,
    ODM_RATE2M          = 0x01,
    ODM_RATE5_5M        = 0x02,
    ODM_RATE11M         = 0x03,
/* OFDM Rates, TxHT = 0 */
    ODM_RATE6M          = 0x04,
    ODM_RATE9M          = 0x05,
    ODM_RATE12M         = 0x06,
    ODM_RATE18M         = 0x07,
    ODM_RATE24M         = 0x08,
    ODM_RATE36M         = 0x09,
    ODM_RATE48M         = 0x0A,
    ODM_RATE54M         = 0x0B,
/* MCS Rates, TxHT = 1 */
    ODM_RATEMCS0            = 0x0C,
    ODM_RATEMCS1            = 0x0D,
    ODM_RATEMCS2            = 0x0E,
    ODM_RATEMCS3            = 0x0F,
    ODM_RATEMCS4            = 0x10,
    ODM_RATEMCS5            = 0x11,
    ODM_RATEMCS6            = 0x12,
    ODM_RATEMCS7            = 0x13,
    ODM_RATEMCS8            = 0x14,
    ODM_RATEMCS9            = 0x15,
    ODM_RATEMCS10           = 0x16,
    ODM_RATEMCS11           = 0x17,
    ODM_RATEMCS12           = 0x18,
    ODM_RATEMCS13           = 0x19,
    ODM_RATEMCS14           = 0x1A,
    ODM_RATEMCS15           = 0x1B,
    ODM_RATEMCS16           = 0x1C,
    ODM_RATEMCS17           = 0x1D,
    ODM_RATEMCS18           = 0x1E,
    ODM_RATEMCS19           = 0x1F,
    ODM_RATEMCS20           = 0x20,
    ODM_RATEMCS21           = 0x21,
    ODM_RATEMCS22           = 0x22,
    ODM_RATEMCS23           = 0x23,
    ODM_RATEMCS24           = 0x24,
    ODM_RATEMCS25           = 0x25,
    ODM_RATEMCS26           = 0x26,
    ODM_RATEMCS27           = 0x27,
    ODM_RATEMCS28           = 0x28,
    ODM_RATEMCS29           = 0x29,
    ODM_RATEMCS30           = 0x2A,
    ODM_RATEMCS31           = 0x2B,
    ODM_RATEVHTSS1MCS0  = 0x2C,
    ODM_RATEVHTSS1MCS1  = 0x2D,
    ODM_RATEVHTSS1MCS2  = 0x2E,
    ODM_RATEVHTSS1MCS3  = 0x2F,
    ODM_RATEVHTSS1MCS4  = 0x30,
    ODM_RATEVHTSS1MCS5  = 0x31,
    ODM_RATEVHTSS1MCS6  = 0x32,
    ODM_RATEVHTSS1MCS7  = 0x33,
    ODM_RATEVHTSS1MCS8  = 0x34,
    ODM_RATEVHTSS1MCS9  = 0x35,
    ODM_RATEVHTSS2MCS0  = 0x36,
    ODM_RATEVHTSS2MCS1  = 0x37,
    ODM_RATEVHTSS2MCS2  = 0x38,
    ODM_RATEVHTSS2MCS3  = 0x39,
    ODM_RATEVHTSS2MCS4  = 0x3A,
    ODM_RATEVHTSS2MCS5  = 0x3B,
    ODM_RATEVHTSS2MCS6  = 0x3C,
    ODM_RATEVHTSS2MCS7  = 0x3D,
    ODM_RATEVHTSS2MCS8  = 0x3E,
    ODM_RATEVHTSS2MCS9  = 0x3F,
    ODM_RATEVHTSS3MCS0  = 0x40,
    ODM_RATEVHTSS3MCS1  = 0x41,
    ODM_RATEVHTSS3MCS2  = 0x42,
    ODM_RATEVHTSS3MCS3  = 0x43,
    ODM_RATEVHTSS3MCS4  = 0x44,
    ODM_RATEVHTSS3MCS5  = 0x45,
    ODM_RATEVHTSS3MCS6  = 0x46,
    ODM_RATEVHTSS3MCS7  = 0x47,
    ODM_RATEVHTSS3MCS8  = 0x48,
    ODM_RATEVHTSS3MCS9  = 0x49,
    ODM_RATEVHTSS4MCS0  = 0x4A,
    ODM_RATEVHTSS4MCS1  = 0x4B,
    ODM_RATEVHTSS4MCS2  = 0x4C,
    ODM_RATEVHTSS4MCS3  = 0x4D,
    ODM_RATEVHTSS4MCS4  = 0x4E,
    ODM_RATEVHTSS4MCS5  = 0x4F,
    ODM_RATEVHTSS4MCS6  = 0x50,
    ODM_RATEVHTSS4MCS7  = 0x51,
    ODM_RATEVHTSS4MCS8  = 0x52,
    ODM_RATEVHTSS4MCS9  = 0x53,
};

/* version is always 0 */
#define PKTHDR_RADIOTAP_VERSION	0

/* see the radiotap website for the descriptions */
enum wf_radiotap_presence {
	WF_RADIOTAP_TSFT = 0,
	WF_RADIOTAP_FLAGS = 1,
	WF_RADIOTAP_RATE = 2,
	WF_RADIOTAP_CHANNEL = 3,
	WF_RADIOTAP_FHSS = 4,
	WF_RADIOTAP_DBM_ANTSIGNAL = 5,
	WF_RADIOTAP_DBM_ANTNOISE = 6,
	WF_RADIOTAP_LOCK_QUALITY = 7,
	WF_RADIOTAP_TX_ATTENUATION = 8,
	WF_RADIOTAP_DB_TX_ATTENUATION = 9,
	WF_RADIOTAP_DBM_TX_POWER = 10,
	WF_RADIOTAP_ANTENNA = 11,
	WF_RADIOTAP_DB_ANTSIGNAL = 12,
	WF_RADIOTAP_DB_ANTNOISE = 13,
	WF_RADIOTAP_RX_FLAGS = 14,
	WF_RADIOTAP_TX_FLAGS = 15,
	WF_RADIOTAP_RTS_RETRIES = 16,
	WF_RADIOTAP_DATA_RETRIES = 17,
	/* 18 is XChannel, but it's not defined yet */
	WF_RADIOTAP_MCS = 19,
	WF_RADIOTAP_AMPDU_STATUS = 20,
	WF_RADIOTAP_VHT = 21,
	WF_RADIOTAP_TIMESTAMP = 22,

	/* valid in every it_present bitmap, even vendor namespaces */
	WF_RADIOTAP_RADIOTAP_NAMESPACE = 29,
	WF_RADIOTAP_VENDOR_NAMESPACE = 30,
	WF_RADIOTAP_EXT = 31
};

/* for IEEE80211_RADIOTAP_FLAGS */
enum wf_radiotap_flags {
	WF_RADIOTAP_F_CFP = 0x01,
	WF_RADIOTAP_F_SHORTPRE = 0x02,
	WF_RADIOTAP_F_WEP = 0x04,
	WF_RADIOTAP_F_FRAG = 0x08,
	WF_RADIOTAP_F_FCS = 0x10,
	WF_RADIOTAP_F_DATAPAD = 0x20,
	WF_RADIOTAP_F_BADFCS = 0x40,
};

/* for IEEE80211_RADIOTAP_CHANNEL */
enum wf_radiotap_channel_flags {
	WF_CHAN_CCK = 0x0020,
	WF_CHAN_OFDM = 0x0040,
	WF_CHAN_2GHZ = 0x0080,
	WF_CHAN_5GHZ = 0x0100,
	WF_CHAN_DYN = 0x0400,
	WF_CHAN_HALF = 0x4000,
	WF_CHAN_QUARTER = 0x8000,
};


/*
rx descriptor
*/
struct rx_desc {
    unsigned int rxdw0;
    unsigned int rxdw1;
    unsigned int rxdw2;
#if defined LONG_TXD
    unsigned int rxdw3;
    unsigned int rxdw4;
    unsigned int rxdw5;
#endif
};

typedef enum PKT_TYPE {
    WF_PKT_TYPE_CMD         = 0x00,
    WF_PKT_TYPE_FW          = 0x01,
    WF_PKT_TYPE_REG         = 0x02,
    WF_PKT_TYPE_FRAME       = 0x03,
    WF_PKT_TYPE_MAX
} PKT_TYPE_T;

typedef enum MAC_FRAME_TYPE {
    _MAC_FRAME_TYPE_MGMT_ = 0,
    _MAC_FRAME_TYPE_CTRL_ = 1,
    _MAC_FRAME_TYPE_DATA_ = 2,
} MAC_FRAME_TYPE_T;

/* rx_desc_detail */
struct rx_desc_detail {
    /* DW0 */
    unsigned int data_type      :   2;  /* bit0~bit1 */     /* data or cmd */
    unsigned int cmd_index      :   8;  /* bit2~bit9 */     /* cmd index */
    unsigned int rsvd0_0        :   9;  /* bit10~bit18 */   /* reserved */
    unsigned int crc32_check    :   1;  /* bit19 */         /* crc32 check flag */
    unsigned int seq_num        :   12; /* bit20~bit31 */   /* sequence number */
    /* DW1 */
    unsigned int pkt_len        :   14; /* bit0~bit13 */    /* packet length */
    unsigned int rsvd1_0        :   9;  /* bit14~bit22 */   /* reserved */
    unsigned int swdec          :   1;  /* bit23 */         /* swdec */
    unsigned int security       :   3;  /* bit24~bit26 */   /* encryption type */
    unsigned int qos            :   1;  /* bit27 */         /* enable qos */
    unsigned int qos_pri        :   4;  /* bit28~bit31 */   /* qos priority */
    /* DW2 */
    unsigned int rx_rate        :   7;  /* bit0~bit6 */     /* rx rate */
    unsigned int rsvd2_0        :   10; /* bit7~bit16 */    /* reserved */
    unsigned int amsdu          :   1;  /* bit17 */         /* it's a amsdu pkt */
    unsigned int more_data      :   1;  /* bit18 */         /* more data to receive */
    unsigned int more_frag      :   1;  /* bit19 */         /* more fragment */
    unsigned int frag_num           :   4;  /* bit20~bit23 */   /* fragment number */
    unsigned int usb_agg_pkt_num    :   8;  /* bit24~bit31 */    /* pkt number */

#if defined LONG_TXD
    /* DW3 */
    unsigned int pattern_match      :   1;  /* bit0 */      /* pattern match */
    unsigned int unicast_match      :   1;  /* bit1 */      /* unicast match */
    unsigned int magic_match        :   1;  /* bit2 */      /* magic match */
    unsigned int rx_is_qos          :   1;  /* bit3 */      /* rx is qos  */
    unsigned int mc                 :   1;  /* bit4 */      /* multicast pkt */
    unsigned int bc                 :   1;  /* bit5 */      /* broadcast pkt */
    unsigned int rxid_match         :   1;  /* bit6 */      /* rx id match */
    unsigned int paggr              :   1;  /* bit7 */      /* */
    unsigned int a1_fit             :   4;  /* bit8~bit11*/    /* f1 fit index */
    unsigned int chkerr             :   1;  /* bit12 */     /* rx check sum enable */
    unsigned int ipver              :   1;  /* bit13 */     /* */
    unsigned int is_tcpudp          :   1;  /* bit14*/      /* if tcp or udp */
    unsigned int chk_vlk            :   1;  /* bit15 */     /* check sum enable */
    unsigned int pam                :   1;  /* bit16 */     /*  */
    unsigned int pwr                :   1;  /* bit17 */     /* power managment */
    unsigned int macid_vld          :   1;  /* bit18*/      /* magic valid */
    unsigned int icv                :   1;  /* bit19 */     /*  */
    unsigned int lseg               :   1;  /* bit20 */     /*  */
    unsigned int fseg               :   1;  /* bit21 */     /*  */
    unsigned int eor                :   1;  /* bit22*/      /*  */
    unsigned int own                :   1;  /* bit23 */     /*  */
    unsigned int drvinfo_size       :   4;  /* bit24~bit27 */       /*  */
    unsigned int shift              :   2;  /* bit28~bit29 */       /*  */
    unsigned int phy_status         :   1;  /* bit30 */     /*  phy status */
    unsigned int rpt_sel            :   1;  /* bit31 */     /* rx magic packet */
    /* DW4 */
    unsigned int rsvd4_0        :   7;  /* bit0~bit6 */     /*  */
    unsigned int splcp          :   1;  /* bit7 */          /*  */
    unsigned int ldpc           :   1;  /* bit8 */          /*  */
    unsigned int stbc           :   1;  /* bit9 */          /*   */
    unsigned int splcp_2        :   1;  /* bit10 */     /* multicast pkt */
    unsigned int ldpc_2         :   1;  /* bit11 */     /* broadcast pkt */
    unsigned int stbc_2         :   1;  /* bit12 */     /* rx id match */
    unsigned int bw             :   2;  /* bit13~bit14 */       /* bandwidth */
    unsigned int htc            :   1;  /* bit15 */             /* 11n thc */
    unsigned int eosp           :   1;  /* bit16 */             /* */
    unsigned int bssid_fit      :   2;  /* bit17~bit18 */       /* */
    unsigned int macid          :   7;  /* bit19~bit25 */       /* rx check sum enable */
    unsigned int wlanhd_iv_len  :   6;  /* bit13 */             /* */
    /* DW5 */
    unsigned int tsfl;

#endif

};

#ifdef CONFIG_RICHV200
/* new rxd */
struct rxd_detail_new {
   /* DW0 */
   unsigned int data_type     : 2;       /* bit0~bit1 */
   unsigned int cmd_index     : 8;       /* bit2~bit9 */
   unsigned int rvd0_0        : 4;       /* bit10~bit13 */
   unsigned int drvinfo_size  : 4;       /* bit14~bit17 */
   unsigned int phy_status    : 1;       /* bit18 */
   unsigned int crc32         : 1;       /* bit19 */
   unsigned int rvd0_1        : 12;      /* bit20~bit31 */
   /* DW1 */
   unsigned int pkt_len       : 14;      /* bit0~bit13 */
   unsigned int rvd1_0        : 9;       /* bit14~bit22 */
   unsigned int swdec         : 1;       /* bit23 */
   unsigned int encrypt_algo  : 3;       /* bit24~bit26 */
   unsigned int qos           : 1;       /* bit27 */
   unsigned int tid           : 4;       /* bit28~bit31 */
   /* DW2 */
   unsigned int rx_rate       : 7;      /* bit0~bit6 */
   unsigned int rvd2_0        : 8;      /* bit7~bit14 */
   unsigned int notice        : 1;      /* bit15 */
   unsigned int rpt_sel       : 1;      /* bit16 */
   unsigned int amsdu         : 1;      /* bit17 */
   unsigned int more_data     : 1;      /* bit18 */
   unsigned int more_frag     : 1;      /* bit19 */
   unsigned int frag          : 4;      /* bit20~bit23 */
   unsigned int usb_agg_pktnum : 8;     /* bit24~bit31 */
   /* DW3 */
   unsigned int mac_id        : 5;      /* bit0~bit4 */
   unsigned int rsvd3_0       : 27;     /* bit5~bit31 */
   /* DW4 */
   unsigned int seq           : 12;     /* bit0~bit11 */
   unsigned int rsvd4_0       : 20;     /* bit12~bit31 */
};
#else
/* org rxd */
struct rxd_detail_org {
    /* DW0 */
    unsigned int pkt_len    :   14;     /* bit0~bit13 */
    unsigned int crc32      :   1;      /* bit14 */
    unsigned int rvd0_0     :   1;      /* bit15 */
    unsigned int drvinfo_size   :   4;  /* bit16~bit19 */
    unsigned int encrypt_algo   :   3;      /* bit20~bit22 */
    unsigned int qos        :   1;      /* bit23 */
    unsigned int shift      :   2;      /* bit24~bit25 */
    unsigned int phy_status :   1;      /* bit26 */
    unsigned int rvd0_1     :   5;      /* bit27~bit13 */
    /* DW1 */
    unsigned int tid        :   4;      /* bit0~bit3 */
    unsigned int amsdu      :   1;      /* bit4 */
    unsigned int more_data  :   1;      /* bit5 */
    unsigned int more_frag  :   1;      /* bit6 */
    unsigned int notice     :   1;      /* bit7 */
    unsigned int rsvd1_0    :   24;     /* bit8~bit31 */
    /* DW2 */
    unsigned int seq        :   12;     /* bit0~bit11 */
    unsigned int frag       :   4;      /* bit12~bit15 */
    unsigned int rsvd2_0    :   12;     /* bit16~bit27 */
    unsigned int rpt_sel    :   1;      /* bit28 */
    unsigned int rsvd2_1    :   3;      /* bit29~bit31 */
    /* DW3 */
    unsigned int rsvd3_0        :   16; /* bit0~bit15 */
    unsigned int usb_agg_pktnum :   8;  /* bit16~bit23 */
    unsigned int rsvd3_1        :   8;  /* bit24~bit31 */
    /* DW4 */
    unsigned int rx_rate    :   6;      /* bit0~bit6 */
    unsigned int rsvd4_0    :   26;     /* bit7~bit31 */
};

typedef struct _RXD_STRUCT {
    unsigned int PktLen:14;
    unsigned int Crc32:1;
    unsigned int Reserve00:5;
    unsigned int Security:3;
    unsigned int Qos:1;
    unsigned int Reserve01:2;
    unsigned int PhyStatus:1;
    unsigned int Reserve02:5;

    unsigned int Tid:4;
    unsigned int Amsdu:1;
    unsigned int MoreData:1;
    unsigned int MoreFrag:1;
    unsigned int Reserve10:25;

    unsigned int Seq:12;
    unsigned int Frag:4;
    unsigned int IsQos:1;
    unsigned int Reserve20:1;
    unsigned int WlanHdIvLen:6;
    unsigned int Reserve21:4;
    unsigned int RptSel:1;
    unsigned int Reserve22:3;

    unsigned int RateBit:7;
    unsigned int Reserve30:3;
    unsigned int Htc:1;
    unsigned int Eosp:1;
    unsigned int BssidFit:2;
    unsigned int Reserve31:2;
    unsigned int Reserve33:13;
    unsigned int PattenMatch:1;
    unsigned int UnicastMatch:1;
    unsigned int MagIcMatch:1;

    unsigned int Rate:6;
    unsigned int Reserve40:26;

    unsigned int Tsfl:32;

    unsigned int BufferAddr:32;

    unsigned int BufferAddr64:32;

} SDIO_RXD_STRUCT;
#endif

#define RX_DESC_GET_FIELD(_rxdescfield,_mask,_offset) \
    (( _rxdescfield >> _offset ) & _mask)

#define RX_DESC_MASK_DATA_TYPE      BITS(0,1)
#define RX_DESC_MASK_CMD_INDEX      BITS(0,7)
#define RX_DESC_MASK_CRC32_ERROR    BIT(0)
#define RX_DESC_MASK_SEQ            BITS(0,11)
#define RX_DESC_MASK_PKT_LEN        BITS(0,13)
#define RX_DESC_MASK_SWDEC          BIT(0)
#define RX_DESC_MASK_SECURITY       BITS(0,2)
#define RX_DESC_MASK_QOS            BIT(0)
#define RX_DESC_MASK_SEQ_PRI        BITS(0,3)
#define RX_DESC_MASK_PKT_LEN        BITS(0,13)
#define RX_DESC_MASK_RX_RATE        BITS(0,6)
#define RX_DESC_MASK_AMSDU          BIT(0)


#define RX_DESC_MASK_MORE_DATA      BIT(0)
#define RX_DESC_MASK_MORE_FLAG      BIT(0)
#define RX_DESC_MASK_FLAG           BITS(0,3)
#define RX_DESC_MASK__AGG_PKTNUM    BITS(0,7)


/*
the stutus of every pkt
*/
typedef struct rx_pkt_info {
    wf_u8 pkt_type;
    wf_u8 cmd_index;
    wf_u8 crc_check;
    wf_u16 seq_num;
    wf_u16 pkt_len;
    wf_u8 hif_hdr_len;     /* rxd , drvinfo,shift_sz total 56byts*/
    wf_u8 wlan_hdr_len;    /* 802.11 frame header length  */
    wf_u8 sw_decrypt;
    /* software decrypt */
    wf_u8 qos_flag;        /* qos frame flag */
    wf_u8 qos_pri;         /* priority */
    wf_u16 rx_rate;
    wf_u8 amsdu;
    wf_u8 more_data;
    wf_u8 more_frag;
    wf_u8 frag_num;
    wf_u8 frame_type;
    wf_u8 bdecrypted;
    wf_u8 encrypt_algo;  /* 0:open mode ; others:encrypt */
    wf_u8 iv_len;
    wf_u8 icv_len;
    wf_u8 crc_err;
    wf_u8 icv_err;
    wf_u16 eth_type;
    wf_u8 phy_status;
    wf_u8 usb_agg_pktnum;
    wf_u8 dst_addr[MAC_ADDR_LEN];
    wf_u8 src_addr[MAC_ADDR_LEN];
    wf_u8 tx_addr[MAC_ADDR_LEN];
    wf_u8 rx_addr[MAC_ADDR_LEN];
    wf_u8 bssid[MAC_ADDR_LEN];

    wf_u8 ack_policy;
    wf_u8 tcpchk_valid;
    wf_u8 ip_chkrpt;
    wf_u8 tcp_chkrpt;
    wf_u8 key_index;
    wf_u8 bw;
    wf_u8 stbc;
    wf_u8 ldpc;
    wf_u32 tsfl;
    wf_u8 sgi;
} rx_pkt_info_t,* prx_pkt_info_t;

/* there 2bytes  */
typedef struct normal_mac_hdr {
    unsigned short protocol_ver : 2;
    unsigned short type         : 2;
    unsigned short subtype      : 4;
    unsigned short to_ds        : 1;
    unsigned short from_ds      : 1;
    unsigned short mfrag        : 1;
    unsigned short retry        : 1;
    unsigned short pwr_mng      : 1;
    unsigned short mdata        : 1;
    unsigned short bprotected   : 1;
    unsigned short order    : 1;
    unsigned short duration;
    unsigned char  addr1[6];
    unsigned char  addr2[6];
    unsigned char  addr3[6];
    unsigned short  seq_ctrl;
    unsigned char  addr4[6];
    unsigned short qos_ctrl;
} normal_mac_hdr_t,* pnormal_mac_hdr_t;

typedef struct rx_statu {
    wf_u8 signal_strength;
    wf_u8 signal_qual;
} rx_status_t;

typedef struct
{
    wf_list_t node;
    wf_u32 addr;
    wf_u64 index;
    wf_u8 sorce_mode;//0:hif,1:rx
}skb_mgt_info_st;

#define RX_REORDER_THREAD_EN   (0)
#define RX_REORDER_NAME_LEN    (32)

typedef struct rx_info {
    wf_u8 * prx_pkt_buf_alloc;
    que_t free_rx_pkt_list; /* list for rx_pkt that note in use*/
    que_t recv_rx_pkt_list; /* list for rx pkt which is used */
    que_t rx_mgmt_frame_defrag_list; /* list for fragmentation frame which is not defragment yet */
    void * p_nic_info;
    rx_status_t rx_sta;
    que_t disc_defrag_q;

    wf_u64 rx_bytes;
    wf_u64 rx_total_pkts;
    wf_u64 rx_pkts;
    wf_u64 rx_drop;
    wf_u64 rx_mgnt_pkt;
    wf_u64 rx_data_pkt;
    wf_u64 rx_crcerr_pkt;
    wf_u32 m0_rxbuf[3];

    recv_ba_ctrl_st  ba_ctl[TID_NUM];
#if RX_REORDER_THREAD_EN
    char rx_reorder_name[RX_REORDER_NAME_LEN];
    void *rx_reorder_tid;
    wf_os_api_sema_t   rx_reorder_sema;
    wf_que_t  rx_reorder_queue;
    wf_lock_t op_lock;
#endif
} rx_info_t, * prx_info_t;




typedef struct phy_status {
    wf_u8 signal_strength;
    wf_u8 signal_qual;
    wf_s8 rssi;
} phy_status_st;
/*
a rx_pkt structure describe the packet attribute
*/
typedef struct rx_pkt {
    que_entry_t entry;
    void * p_hif_node;    /* point to struct hif_node structure */
    void * p_nic_info;    /* point to struct nic_info structure */
    prx_info_t prx_info;
    struct rx_pkt_info pkt_info;
    phy_status_st phy_status;
    wf_u8  rxd_raw_buf[RXDESC_SIZE];
    void * pskb;
    wf_u8 * pbuf;
    wf_u32 len;
    wf_u8 *phead;
    wf_u8 *pdata;
    wf_u8 *ptail;
    wf_u8 *pend;
    wdn_net_info_st *wdn_info;
} rx_pkt_t, * prx_pkt_t;

typedef struct
{
    wf_list_t list;
    wf_u16 seq_num;
    void * pskb;
}rx_reorder_queue_st;

#if RX_REORDER_THREAD_EN

typedef struct rx_reorder_node_
{
    wf_que_list_t list;
    rx_pkt_t pkt;
}rx_reorder_node_t;

wf_s32 wf_rx_reorder_queue_insert(rx_pkt_t *pkt);
wf_s32 wf_rx_reorder_queue_remove(rx_info_t *rx_info, rx_reorder_node_t **rx_reorder_node);

#endif



/**
 * struct rx_radiotap_header - base radiotap header
 */
struct rx_radiotap_header {
	/**
	 * @it_version: radiotap version, always 0
	 */
	wf_u8 it_version;

	/**
	 * @it_pad: padding (or alignment)
	 */
    wf_u8 it_pad;

	/**
	 * @it_len: overall radiotap header length
	 */
	wf_u16 it_len;

	/**
	 * @it_present: (first) present word
	 */
	wf_u32 it_present;
} wf_packed;

typedef enum
{
    REORDER_DROP = -1,
    REORDER_SEND = 0,
    REORDER_ENQUE = 1,
    REORDER_ENQUE_SEND = 2,
    REORDER_DEQUE_SEND = 3,
}REORDER_STATE;

int wf_rx_init(nic_info_st *nic_info);
int wf_rx_term(nic_info_st *nic_info);
int wf_rx_common_process(prx_pkt_t ppkt);
int wf_rx_data_len_check(nic_info_st *pnic_info,wf_u8 *pbuf, wf_u16 skb_len);
int wf_rx_notice_process(wf_u8 *pbuf, wf_u16 skb_len);
int wf_rx_cmd_check(wf_u8 *pbuf, wf_u16 skb_len);

wf_u8 calc_rx_rate(wf_u8 rx_rate);

wf_u16     wf_rx_get_pkt_len_and_check_valid(wf_u8 *buf,  wf_u16 remain, wf_bool *valid, wf_bool *notice);
PKT_TYPE_T wf_rx_data_type(wf_u8 *pbuf);
void       wf_rx_rxd_prase(wf_u8 *pbuf, struct rx_pkt *prx_pkt);
int wf_rx_action_ba_ctl_init(nic_info_st *nic_info);

rx_reorder_queue_st *rx_free_reorder_dequeue(recv_ba_ctrl_st *ba_ctl);
int rx_free_reorder_enqueue(recv_ba_ctrl_st *ba_ctl, rx_reorder_queue_st *node);
int rx_free_reorder_empty(recv_ba_ctrl_st *ba_ctl);
void rx_do_update_expect_seq(wf_u16 seq_num, recv_ba_ctrl_st   *ba_order);
int rx_pending_reorder_is_empty(recv_ba_ctrl_st   *ba_order);
int rx_pending_reorder_enqueue(wf_u16 current_seq, void *pskb, recv_ba_ctrl_st   *ba_order);
rx_reorder_queue_st * rx_pending_reorder_dequeue(recv_ba_ctrl_st   *ba_order);
rx_reorder_queue_st *rx_pending_reorder_getqueue(recv_ba_ctrl_st   *ba_order);
int rx_do_chk_expect_seq(wf_u16 seq_num, recv_ba_ctrl_st   *ba_order);
int wf_rx_action_ba_ctl_deinit(nic_info_st *nic_info);
wf_s32 wf_rx_ba_reinit(nic_info_st *nic_io, wf_u8 tid);

int rx_pending_reorder_get_cnt(recv_ba_ctrl_st   *ba_order);
void rx_reorder_timeout_handle(wf_os_api_timer_t * timer);

void wf_rx_ba_all_reinit(nic_info_st *nic_info);
int rx_reorder_upload(recv_ba_ctrl_st   *ba_order);
void wf_rx_data_reorder_core(rx_pkt_t *pkt);

int rx_check_data_frame_valid(prx_pkt_t prx_pkt);
int rx_check_mngt_frame_valid(prx_pkt_t prx_pkt);

wf_s32 wf_rx_calc_str_and_qual(nic_info_st *nic_info, wf_u8 *phystatus,  wf_u8 *mac_frame, void *prx_pkt);

#endif

