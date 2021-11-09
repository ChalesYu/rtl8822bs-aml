/*
 * wf_80211.h
 *
 * This file contains all the prototypes for the wf_80211.c file
 *
 * Author: luozhi
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
#ifndef __WF_80211_H__
#define __WF_80211_H__

#include "wf_typedef.h"
/* ETH Header */
#define WF_ETH_ALEN               (6)
#define WF_ETH_HLEN               (14)
#define WF_EEPROM_MAX_SIZE        (512)
#define WF_SSID_LEN               (32)
#define WF_MAX_BITRATES           (8)
#define WF_RATES_NUM              (13)
#define WF_MCS_NUM                (16)
#define WF_MAX_CHANNEL_NUM        (14)
#define WF_MAX_WPA_IE_LEN         (256)
#define WF_MAX_WPS_IE_LEN         (512)
#define WF_MAX_P2P_IE_LEN         (256)
#define WF_MAX_WFD_IE_LEN         (128)
#define WF_TID_NUM                16
#define WF_ETH_P_IP               0x0800
#define WF_ETH_P_ARP              0x0806
#define WF_ETH_P_ATALK            0x809B
#define WF_ETH_P_AARP             0x80F3
#define WF_ETH_P_8021Q            0x8100
#define WF_ETH_P_IPX              0x8137
#define WF_ETH_P_IPV6             0x86DD
#define WF_ETH_P_PPP_DISC         0x8863
#define WF_ETH_P_PPP_SES          0x8864

#define WF_IS_MCAST(mac)            (mac[0] & 0x01)
#define WF_IP_MCAST_MAC(mac)        ((mac[0]==0x01)&&(mac[1]==0x00)&&(mac[2]==0x5e))
#define WF_ICMPV6_MCAST_MAC(mac)    ((mac[0]==0x33)&&(mac[1]==0x33)&&(mac[2]!=0xff))
#define WF_BROADCAST_MAC_ADDR(mac)  (((mac[0] == 0xff) && (mac[1] == 0xff) && \
     (mac[2] == 0xff) && (mac[3] == 0xff) && (mac[4] == 0xff) && (mac[5] == 0xff)))

#define WF_IPV4_SRC(_iphdr)             (((wf_u8 *)(_iphdr)) + 12)
#define WF_IPV4_DST(_iphdr)             (((wf_u8 *)(_iphdr)) + 16)
#define WF_GET_IPV4_IHL(_iphdr)         wf_be_bits_to_u8(((wf_u8 *)(_iphdr)) + 0, 0, 4)
#define WF_GET_IPV4_PROTOCOL(_iphdr)    wf_be_bits_to_u8(((wf_u8 *)(_iphdr)) + 9, 0, 8)
#define WF_GET_IPV4_TOS(_iphdr)         wf_be_bits_to_u8(((wf_u8 *)(_iphdr)) + 1, 0, 8)

#define WF_GET_UDP_SRC(_udphdr)         wf_be_bits_to_u16(((wf_u8 *)(_udphdr)) + 0, 0, 16)
#define WF_GET_UDP_DST(_udphdr)         wf_be_bits_to_u16(((wf_u8 *)(_udphdr)) + 2, 0, 16)

#define WF_TCP_SRC(_tcphdr)             (((wf_u8 *)(_tcphdr)) + 0)
#define WF_TCP_DST(_tcphdr)             (((wf_u8 *)(_tcphdr)) + 2)
#define WF_GET_TCP_FIN(_tcphdr)         wf_be_bits_to_u8(((wf_u8 *)(_tcphdr)) + 13, 0, 1)
#define WF_GET_TCP_SYN(_tcphdr)         wf_be_bits_to_u8(((wf_u8 *)(_tcphdr)) + 13, 1, 1)
#define WF_GET_TCP_ACK(_tcphdr)         wf_be_bits_to_u8(((wf_u8 *)(_tcphdr)) + 13, 4, 1)

#define WF_MAC_FMT     "%02x:%02x:%02x:%02x:%02x:%02x"
#define WF_MAC_ARG(x)  ((wf_u8*)(x))[0],((wf_u8*)(x))[1],((wf_u8*)(x))[2],((wf_u8*)(x))[3],((wf_u8*)(x))[4],((wf_u8*)(x))[5]

#define MAX_SUBFRAME_COUNT  64


/*
 * ieee802.11 MAC frame type define
 */

#define MAC_ADDR_LEN                          6

/* MAC frame type */
#define WF_80211_FTYPE_MGMT                   0x0000
#define WF_80211_FTYPE_CTL                    0x0004
#define WF_80211_FTYPE_DATA                   0x0008
#define WF_80211_FTYPE_EXT                    0x000C
/* management subtype */
#define WF_80211_STYPE_ASSOC_REQ              0x0000
#define WF_80211_STYPE_ASSOC_RESP             0x0010
#define WF_80211_STYPE_REASSOC_REQ            0x0020
#define WF_80211_STYPE_REASSOC_RESP           0x0030
#define WF_80211_STYPE_PROBE_REQ              0x0040
#define WF_80211_STYPE_PROBE_RESP             0x0050
#define WF_80211_STYPE_BEACON                 0x0080
#define WF_80211_STYPE_ATIM                   0x0090
#define WF_80211_STYPE_DISASSOC               0x00A0
#define WF_80211_STYPE_AUTH                   0x00B0
#define WF_80211_STYPE_DEAUTH                 0x00C0
#define WF_80211_STYPE_ACTION                 0x00D0
/* control subtype */
#define WF_80211_STYPE_CTL_EXT                0x0060
#define WF_80211_STYPE_BACK_REQ               0x0080
#define WF_80211_STYPE_BACK                   0x0090
#define WF_80211_STYPE_PSPOLL                 0x00A0
#define WF_80211_STYPE_RTS                    0x00B0
#define WF_80211_STYPE_CTS                    0x00C0
#define WF_80211_STYPE_ACK                    0x00D0
#define WF_80211_STYPE_CFEND                  0x00E0
#define WF_80211_STYPE_CFENDACK               0x00F0
/* data subtype */
#define WF_80211_STYPE_DATA                   0x0000
#define WF_80211_STYPE_DATA_CFACK             0x0010
#define WF_80211_STYPE_DATA_CFPOLL            0x0020
#define WF_80211_STYPE_DATA_CFACKPOLL         0x0030
#define WF_80211_STYPE_NULLFUNC               0x0040
#define WF_80211_STYPE_CFACK                  0x0050
#define WF_80211_STYPE_CFPOLL                 0x0060
#define WF_80211_STYPE_CFACKPOLL              0x0070
#define WF_80211_STYPE_QOS_DATA               0x0080
#define WF_80211_STYPE_QOS_DATA_CFACK         0x0090
#define WF_80211_STYPE_QOS_DATA_CFPOLL        0x00A0
#define WF_80211_STYPE_QOS_DATA_CFACKPOLL     0x00B0
#define WF_80211_STYPE_QOS_NULLFUNC           0x00C0
#define WF_80211_STYPE_QOS_CFACK              0x00D0
#define WF_80211_STYPE_QOS_CFPOLL             0x00E0
#define WF_80211_STYPE_QOS_CFACKPOLL          0x00F0
/* frame type */
typedef enum
{
    /* management frame */
    WF_80211_FRM_ASSOC_REQ          = WF_80211_FTYPE_MGMT | WF_80211_STYPE_ASSOC_REQ,
    WF_80211_FRM_ASSOC_RESP         = WF_80211_FTYPE_MGMT | WF_80211_STYPE_ASSOC_RESP,
    WF_80211_FRM_REASSOC_REQ        = WF_80211_FTYPE_MGMT | WF_80211_STYPE_REASSOC_REQ,
    WF_80211_FRM_REASSOC_RESP       = WF_80211_FTYPE_MGMT | WF_80211_STYPE_REASSOC_RESP,
    WF_80211_FRM_PROBE_REQ          = WF_80211_FTYPE_MGMT | WF_80211_STYPE_PROBE_REQ,
    WF_80211_FRM_PROBE_RESP         = WF_80211_FTYPE_MGMT | WF_80211_STYPE_PROBE_RESP,
    WF_80211_FRM_BEACON             = WF_80211_FTYPE_MGMT | WF_80211_STYPE_BEACON,
    WF_80211_FRM_ATIM               = WF_80211_FTYPE_MGMT | WF_80211_STYPE_ATIM,
    WF_80211_FRM_DISASSOC           = WF_80211_FTYPE_MGMT | WF_80211_STYPE_DISASSOC,
    WF_80211_FRM_AUTH               = WF_80211_FTYPE_MGMT | WF_80211_STYPE_AUTH,
    WF_80211_FRM_DEAUTH             = WF_80211_FTYPE_MGMT | WF_80211_STYPE_DEAUTH,
    WF_80211_FRM_ACTION             = WF_80211_FTYPE_MGMT | WF_80211_STYPE_ACTION,
    /* control frame */
    WF_80211_FRM_CTL_EXT            = WF_80211_FTYPE_CTL | WF_80211_STYPE_CTL_EXT,
    WF_80211_FRM_BACK_REQ           = WF_80211_FTYPE_CTL | WF_80211_STYPE_BACK_REQ,
    WF_80211_FRM_BACK               = WF_80211_FTYPE_CTL | WF_80211_STYPE_BACK,
    WF_80211_FRM_PSPOLL             = WF_80211_FTYPE_CTL | WF_80211_STYPE_PSPOLL,
    WF_80211_FRM_RTS                = WF_80211_FTYPE_CTL | WF_80211_STYPE_RTS,
    WF_80211_FRM_CTS                = WF_80211_FTYPE_CTL | WF_80211_STYPE_CTS,
    WF_80211_FRM_ACK                = WF_80211_FTYPE_CTL | WF_80211_STYPE_ACK,
    WF_80211_FRM_CFEND              = WF_80211_FTYPE_CTL | WF_80211_STYPE_CFEND,
    WF_80211_FRM_CFENDACK           = WF_80211_FTYPE_CTL | WF_80211_STYPE_CFENDACK,
    /* data frame */
    WF_80211_FRM_DATA               = WF_80211_FTYPE_DATA | WF_80211_STYPE_DATA,
    WF_80211_FRM_DATA_CFACK         = WF_80211_FTYPE_DATA | WF_80211_STYPE_DATA_CFACK,
    WF_80211_FRM_DATA_CFPOLL        = WF_80211_FTYPE_DATA | WF_80211_STYPE_DATA_CFPOLL,
    WF_80211_FRM_DATA_CFACKPOLL     = WF_80211_FTYPE_DATA | WF_80211_STYPE_DATA_CFACKPOLL,
    WF_80211_FRM_NULLFUNC           = WF_80211_FTYPE_DATA | WF_80211_STYPE_NULLFUNC,
    WF_80211_FRM_CFACK              = WF_80211_FTYPE_DATA | WF_80211_STYPE_CFACK,
    WF_80211_FRM_CFPOLL             = WF_80211_FTYPE_DATA | WF_80211_STYPE_CFPOLL,
    WF_80211_FRM_CFACKPOLL          = WF_80211_FTYPE_DATA | WF_80211_STYPE_CFACKPOLL,
    WF_80211_FRM_QOS_DATA           = WF_80211_FTYPE_DATA | WF_80211_STYPE_QOS_DATA,
    WF_80211_FRM_QOS_DATA_CFACK     = WF_80211_FTYPE_DATA | WF_80211_STYPE_QOS_DATA_CFACK,
    WF_80211_FRM_QOS_DATA_CFPOLL    = WF_80211_FTYPE_DATA | WF_80211_STYPE_QOS_DATA_CFPOLL,
    WF_80211_FRM_QOS_DATA_CFACKPOLL = WF_80211_FTYPE_DATA | WF_80211_STYPE_QOS_DATA_CFACKPOLL,
    WF_80211_FRM_QOS_NULLFUNC       = WF_80211_FTYPE_DATA | WF_80211_STYPE_QOS_NULLFUNC,
    WF_80211_FRM_QOS_CFACK          = WF_80211_FTYPE_DATA | WF_80211_STYPE_QOS_CFACK,
    WF_80211_FRM_QOS_CFPOLL         = WF_80211_FTYPE_DATA | WF_80211_STYPE_QOS_CFPOLL,
    WF_80211_FRM_QOS_CFACKPOLL      = WF_80211_FTYPE_DATA | WF_80211_STYPE_QOS_CFACKPOLL,
} wf_80211_frame_e;

/* MAC frame types and subtypes */
#define MAC_FRAME_TYPE_MGT                      0
#define MAC_FRAME_TYPE_CTRL                     WF_80211_FTYPE_CTL
#define MAC_FRAME_TYPE_DATA                     WF_80211_FTYPE_DATA
#define MAC_FRAME_TYPE_QOS_DATA                 (WF_80211_FTYPE_DATA | WF_80211_STYPE_QOS_DATA)

/* Duration/ID field */
#define MASK_DI_DURATION                        BITS(0, 14)
#define MASK_DI_AID                             BITS(0, 13)
#define MASK_DI_AID_MSB                         BITS(14, 15)
#define MASK_DI_CFP_FIXED_VALUE                 BIT(15)

/* Sequence Control field */
#define MASK_SC_SEQ_NUM                         BITS(4, 15)
#define MASK_SC_SEQ_NUM_OFFSET                  4
#define MASK_SC_FRAG_NUM                        BITS(0, 3)
#define INVALID_SEQ_CTRL_NUM                    0x000F

/* QoS Control field */
#define TID_NUM                                 16
#define TID_MASK                                BITS(0, 3)
#define EOSP                                    BIT(4)
#define ACK_POLICY                              BITS(5, 6)
#define A_MSDU_PRESENT                          BIT(7)

#define MASK_QC_TID                             BITS(0, 3)
#define MASK_QC_EOSP                            BIT(4)
#define MASK_QC_EOSP_OFFSET                     4
#define MASK_QC_ACK_POLICY                      BITS(5, 6)
#define MASK_QC_ACK_POLICY_OFFSET               5
#define MASK_QC_A_MSDU_PRESENT                  BIT(7)


#define HT_CTRL_LEN                             4

/* HT Control field */
#define HT_CTRL_LINK_ADAPTATION_CTRL            BITS(0, 15)
#define HT_CTRL_CALIBRATION_POSITION            BITS(16, 17)
#define HT_CTRL_CALIBRATION_SEQUENCE            BITS(18, 19)
#define HT_CTRL_CSI_STEERING                    BITS(22, 23)
#define HT_CTRL_NDP_ANNOUNCEMENT                BIT(24)
#define HT_CTRL_AC_CONSTRAINT                   BIT(30)
#define HT_CTRL_RDG_MORE_PPDU                   BIT(31)

#define LINK_ADAPTATION_CTRL_TRQ                BIT(1)
#define LINK_ADAPTATION_CTRL_MAI_MRQ            BIT(2)
#define LINK_ADAPTATION_CTRL_MAI_MSI            BITS(3, 5)
#define LINK_ADAPTATION_CTRL_MFSI               BITS(6, 8)
#define LINK_ADAPTATION_CTRL_MFB_ASELC_CMD      BITS(9, 11)
#define LINK_ADAPTATION_CTRL_MFB_ASELC_DATA     BITS(12, 15)

/* Ack Policy subfield */
#define ACK_POLICY_NORMAL_ACK_IMPLICIT_BA_REQ   0
#define ACK_POLICY_NO_ACK                       1
#define ACK_POLICY_NO_EXPLICIT_ACK_PSMP_ACK     2
#define ACK_POLICY_BA                           3

/* FCS field */
#define FCS_LEN                                 4



/* frame control field desc 2bytes */
#define PROTOCL_VER         BITS(0,1)
#define TYPE                BITS(2,3)
#define SUB_TYPE            BITS(4,7)
#define TO_DS               BIT(8)
#define FROM_DS             BIT(9)
#define MORE_FRAG           BIT(10)
#define RETRY               BIT(11)
#define PWRMGT              BIT(12)
#define MORE_DATA           BIT(13)
#define PROTECTED           BIT(14)
#define ORDER               BIT(15)

#define GET_HDR_ProtolVer(pbuf)     (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & PROTOCL_VER)
#define GET_HDR_Type(pbuf)          (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & TYPE)
#define GET_HDR_SubTpye(pbuf)       (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & SUB_TYPE)
#define GET_HDR_To_From_DS(pbuf)    (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & (TO_DS | FROM_DS))
#define GET_HDR_More_Frag(pbuf)     (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & MORE_FRAG)
#define GET_HDR_Retry(pbuf)         (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & RETRY)
#define GET_HDR_PWR_MNG(pbuf)       (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & PWRMGT)
#define GET_HDR_More_Data(pbuf)     (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & MORE_DATA)
#define GET_HDR_Protected(pbuf)     (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & PROTECTED)
#define GET_HDR_Order(pbuf)         (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & ORDER)

#define GET_ADDR1(pbuf)             ((wf_u8 *)(wf_u8 *)pbuf + 4 )
#define GET_ADDR2(pbuf)             ((wf_u8 *)(wf_u8 *)pbuf + 10)
#define GET_ADDR3(pbuf)             ((wf_u8 *)(wf_u8 *)pbuf + 16)
#define GET_ADDR4(pbuf)             ((wf_u8 *)(wf_u8 *)pbuf + 24)

#define GET_Seq_Num(pbuf)           (wf_cpu_to_le16(*(unsigned short *)((wf_u8 *)(pbuf) + 22)))
#define GET_Seq_Quence(pbuf)        (wf_cpu_to_le16(*(unsigned short *)((wf_u8 *)(pbuf) + 22)) >> 4)
#define GET_Frag_Num(pbuf)          (wf_cpu_to_le16(*(unsigned short *)((wf_u8 *)(pbuf) + 22)) & 0x0F)

#define GET_TID(pbuf)               (wf_le16_to_cpu(*(unsigned short *)(pbuf)) & 0x0F)
#define GET_Ack_Policy(pbuf)        (wf_le16_to_cpu(*(unsigned short *)(pbuf)) >> 5 & 0x03)
#define GET_Amsdu(pbuf)             (wf_le16_to_cpu(*(unsigned short *)(pbuf)) >> 7 & 0x01)



/*******************************************************************************
 *                        ieee802.11 frame format define
 ******************************************************************************/
/*
 * MAC header fields
 */
typedef wf_u16 wf_80211_frame_ctrl_t;
#define WF_80211_FCTL_VERS              0x0003
#define WF_80211_FCTL_FTYPE             0x000C
#define WF_80211_FCTL_STYPE             0x00F0
#define WF_80211_FCTL_TODS              0x0100
#define WF_80211_FCTL_FROMDS            0x0200
#define WF_80211_FCTL_MOREFRAGS         0x0400
#define WF_80211_FCTL_RETRY             0x0800
#define WF_80211_FCTL_PM                0x1000
#define WF_80211_FCTL_MOREDATA          0x2000
#define WF_80211_FCTL_PROTECTED         0x4000
#define WF_80211_FCTL_ORDER             0x8000
#define WF_80211_FCTL_CTL_EXT           0x0F00
/* control extension - for WF_80211_FRM_CTL_EXT */
#define WF_80211_CTL_EXT_POLL           0x0200
#define WF_80211_CTL_EXT_SPR            0x0300
#define WF_80211_CTL_EXT_GRANT          0x0400
#define WF_80211_CTL_EXT_DMG_CTS        0x0500
#define WF_80211_CTL_EXT_DMG_DTS        0x0600
#define WF_80211_CTL_EXT_SSW            0x0800
#define WF_80211_CTL_EXT_SSW_FBACK      0x0900
#define WF_80211_CTL_EXT_SSW_ACK        0x0A00


typedef wf_u16 wf_80211_duration_t;
typedef wf_u8 wf_80211_addr_t[MAC_ADDR_LEN];
typedef wf_80211_addr_t wf_80211_bssid_t;

typedef wf_u16 wf_80211_seq_ctrl_t;
#define WF_80211_SCTL_FRAG_MASK         0x000F
#define WF_80211_SCTL_FRAG_SHIFT        0
#define WF_80211_SCTL_SEQ_MASK          0xFFF0
#define WF_80211_SCTL_SEQ_SHIFT         4

/*
 * management frame
 */
/* no element fields */
typedef wf_u64 wf_80211_mgmt_timestamp_t;
typedef wf_u16 wf_80211_mgmt_beacon_interval_t;
typedef wf_u16 wf_80211_mgmt_capab_t;
#define WF_80211_MGMT_CAPAB_ESS                 (1<<0)
#define WF_80211_MGMT_CAPAB_IBSS                (1<<1)
#define WF_80211_MGMT_CAPAB_CF_POLLABLE         (1<<2)
#define WF_80211_MGMT_CAPAB_CF_POLL_REQUEST     (1<<3)
#define WF_80211_MGMT_CAPAB_PRIVACY             (1<<4)
#define WF_80211_MGMT_CAPAB_SHORT_PREAMBLE      (1<<5)
#define WF_80211_MGMT_CAPAB_PBCC                (1<<6)
#define WF_80211_MGMT_CAPAB_CHANNEL_AGILITY     (1<<7)
/* 802.11h */
#define WF_80211_MGMT_CAPAB_SPECTRUM_MGMT       (1<<8)
#define WF_80211_MGMT_CAPAB_QOS                 (1<<9)
#define WF_80211_MGMT_CAPAB_SHORT_SLOT_TIME     (1<<10)
#define WF_80211_MGMT_CAPAB_APSD                (1<<11)
#define WF_80211_MGMT_CAPAB_RADIO_MEASURE       (1<<12)
#define WF_80211_MGMT_CAPAB_DSSS_OFDM           (1<<13)
#define WF_80211_MGMT_CAPAB_DEL_BACK            (1<<14)
#define WF_80211_MGMT_CAPAB_IMM_BACK            (1<<15)
/*
 * A mesh STA sets the ESS and IBSS capability bits to zero.
 * however, this holds true for p2p probe responses (in the p2p_find
 * phase) as well.
 */
#define WF_80211_CAPAB_IS_MESH_STA_BSS(cap)  \
    (!((cap) & (WF_80211_MGMT_CAPAB_ESS | WF_80211_MGMT_CAPAB_IBSS)))

#define WF_80211_CAPAB_IS_IBSS(cap)  \
    (!((cap) & WF_80211_MGMT_CAPAB_ESS) && ((cap) & WF_80211_MGMT_CAPAB_IBSS))

/* Status codes */
typedef enum
{
    WF_80211_STATUS_SUCCESS                                     = 0,
    WF_80211_STATUS_UNSPECIFIED_FAILURE                         = 1,
    WF_80211_STATUS_CAPS_UNSUPPORTED                            = 10,
    WF_80211_STATUS_REASSOC_NO_ASSOC                            = 11,
    WF_80211_STATUS_ASSOC_DENIED_UNSPEC                         = 12,
    WF_80211_STATUS_NOT_SUPPORTED_AUTH_ALG                      = 13,
    WF_80211_STATUS_UNKNOWN_AUTH_TRANSACTION                    = 14,
    WF_80211_STATUS_CHALLENGE_FAIL                              = 15,
    WF_80211_STATUS_AUTH_TIMEOUT                                = 16,
    WF_80211_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA                 = 17,
    WF_80211_STATUS_ASSOC_DENIED_RATES                          = 18,
    /* 802.11b */
    WF_80211_STATUS_ASSOC_DENIED_NOSHORTPREAMBLE                = 19,
    WF_80211_STATUS_ASSOC_DENIED_NOPBCC                         = 20,
    WF_80211_STATUS_ASSOC_DENIED_NOAGILITY                      = 21,
    /* 802.11h */
    WF_80211_STATUS_ASSOC_DENIED_NOSPECTRUM                     = 22,
    WF_80211_STATUS_ASSOC_REJECTED_BAD_POWER                    = 23,
    WF_80211_STATUS_ASSOC_REJECTED_BAD_SUPP_CHAN                = 24,
    /* 802.11g */
    WF_80211_STATUS_ASSOC_DENIED_NOSHORTTIME                    = 25,
    WF_80211_STATUS_ASSOC_DENIED_NODSSSOFDM                     = 26,
    /* 802.11w */
    WF_80211_STATUS_ASSOC_REJECTED_TEMPORARILY                  = 30,
    WF_80211_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION          = 31,
    /* 802.11i */
    WF_80211_STATUS_INVALID_IE                                  = 40,
    WF_80211_STATUS_INVALID_GROUP_CIPHER                        = 41,
    WF_80211_STATUS_INVALID_PAIRWISE_CIPHER                     = 42,
    WF_80211_STATUS_INVALID_AKMP                                = 43,
    WF_80211_STATUS_UNSUPP_RSN_VERSION                          = 44,
    WF_80211_STATUS_INVALID_RSN_IE_CAP                          = 45,
    WF_80211_STATUS_CIPHER_SUITE_REJECTED                       = 46,
    /* 802.11e */
    WF_80211_STATUS_UNSPECIFIED_QOS                             = 32,
    WF_80211_STATUS_ASSOC_DENIED_NOBANDWIDTH                    = 33,
    WF_80211_STATUS_ASSOC_DENIED_LOWACK                         = 34,
    WF_80211_STATUS_ASSOC_DENIED_UNSUPP_QOS                     = 35,
    WF_80211_STATUS_REQUEST_DECLINED                            = 37,
    WF_80211_STATUS_INVALID_QOS_PARAM                           = 38,
    WF_80211_STATUS_CHANGE_TSPEC                                = 39,
    WF_80211_STATUS_WAIT_TS_DELAY                               = 47,
    WF_80211_STATUS_NO_DIRECT_LINK                              = 48,
    WF_80211_STATUS_STA_NOT_PRESENT                             = 49,
    WF_80211_STATUS_STA_NOT_QSTA                                = 50,
    /* 802.11s */
    WF_80211_STATUS_ANTI_CLOG_REQUIRED                          = 76,
    WF_80211_STATUS_FCG_NOT_SUPP                                = 78,
    WF_80211_STATUS_STA_NO_TBTT                                 = 78,
    /* 802.11ad */
    WF_80211_STATUS_REJECTED_WITH_SUGGESTED_CHANGES             = 39,
    WF_80211_STATUS_REJECTED_FOR_DELAY_PERIOD                   = 47,
    WF_80211_STATUS_REJECT_WITH_SCHEDULE                        = 83,
    WF_80211_STATUS_PENDING_ADMITTING_FST_SESSION               = 86,
    WF_80211_STATUS_PERFORMING_FST_NOW                          = 87,
    WF_80211_STATUS_PENDING_GAP_IN_BA_WINDOW                    = 88,
    WF_80211_STATUS_REJECT_U_PID_SETTING                        = 89,
    WF_80211_STATUS_REJECT_DSE_BAND                             = 96,
    WF_80211_STATUS_DENIED_WITH_SUGGESTED_BAND_AND_CHANNEL      = 99,
    WF_80211_STATUS_DENIED_DUE_TO_SPECTRUM_MANAGEMENT           = 103,
} wf_80211_statuscode_e;

/* Reason codes */
typedef enum
{
    WF_80211_REASON_UNSPECIFIED                     = 1,
    WF_80211_REASON_PREV_AUTH_NOT_VALID             = 2,
    WF_80211_REASON_DEAUTH_LEAVING                  = 3,
    WF_80211_REASON_DISASSOC_DUE_TO_INACTIVITY      = 4,
    WF_80211_REASON_DISASSOC_AP_BUSY                = 5,
    WF_80211_REASON_CLASS2_FRAME_FROM_NONAUTH_STA   = 6,
    WF_80211_REASON_CLASS3_FRAME_FROM_NONASSOC_STA  = 7,
    WF_80211_REASON_DISASSOC_STA_HAS_LEFT           = 8,
    WF_80211_REASON_STA_REQ_ASSOC_WITHOUT_AUTH      = 9,
    /* 802.11h */
    WF_80211_REASON_DISASSOC_BAD_POWER             = 10,
    WF_80211_REASON_DISASSOC_BAD_SUPP_CHAN         = 11,
    /* 802.11i */
    WF_80211_REASON_INVALID_IE                     = 13,
    WF_80211_REASON_MIC_FAILURE                    = 14,
    WF_80211_REASON_4WAY_HANDSHAKE_TIMEOUT         = 15,
    WF_80211_REASON_GROUP_KEY_HANDSHAKE_TIMEOUT    = 16,
    WF_80211_REASON_IE_DIFFERENT                   = 17,
    WF_80211_REASON_INVALID_GROUP_CIPHER           = 18,
    WF_80211_REASON_INVALID_PAIRWISE_CIPHER        = 19,
    WF_80211_REASON_INVALID_AKMP                   = 20,
    WF_80211_REASON_UNSUPP_RSN_VERSION             = 21,
    WF_80211_REASON_INVALID_RSN_IE_CAP             = 22,
    WF_80211_REASON_IEEE8021X_FAILED               = 23,
    WF_80211_REASON_CIPHER_SUITE_REJECTED          = 24,
    /* TDLS (802.11z) */
    WF_80211_REASON_TDLS_TEARDOWN_UNREACHABLE      = 25,
    WF_80211_REASON_TDLS_TEARDOWN_UNSPECIFIED      = 26,
    /* 802.11e */
    WF_80211_REASON_DISASSOC_UNSPECIFIED_QOS       = 32,
    WF_80211_REASON_DISASSOC_QAP_NO_BANDWIDTH      = 33,
    WF_80211_REASON_DISASSOC_LOW_ACK               = 34,
    WF_80211_REASON_DISASSOC_QAP_EXCEED_TXOP       = 35,
    WF_80211_REASON_QSTA_LEAVE_QBSS                = 36,
    WF_80211_REASON_QSTA_NOT_USE                   = 37,
    WF_80211_REASON_QSTA_REQUIRE_SETUP             = 38,
    WF_80211_REASON_QSTA_TIMEOUT                   = 39,
    WF_80211_REASON_QSTA_CIPHER_NOT_SUPP           = 45,
    /* 802.11s */
    WF_80211_REASON_MESH_PEER_CANCELED             = 52,
    WF_80211_REASON_MESH_MAX_PEERS                 = 53,
    WF_80211_REASON_MESH_CONFIG                    = 54,
    WF_80211_REASON_MESH_CLOSE                     = 55,
    WF_80211_REASON_MESH_MAX_RETRIES               = 56,
    WF_80211_REASON_MESH_CONFIRM_TIMEOUT           = 57,
    WF_80211_REASON_MESH_INVALID_GTK               = 58,
    WF_80211_REASON_MESH_INCONSISTENT_PARAM        = 59,
    WF_80211_REASON_MESH_INVALID_SECURITY          = 60,
    WF_80211_REASON_MESH_PATH_ERROR                = 61,
    WF_80211_REASON_MESH_PATH_NOFORWARD            = 62,
    WF_80211_REASON_MESH_PATH_DEST_UNREACHABLE     = 63,
    WF_80211_REASON_MAC_EXISTS_IN_MBSS             = 64,
    WF_80211_REASON_MESH_CHAN_REGULATORY           = 65,
    WF_80211_REASON_MESH_CHAN                      = 66,
} wf_80211_reasoncode_e;


/* element fields */
typedef enum
{
    WF_80211_MGMT_EID_SSID                           = 0,
    WF_80211_MGMT_EID_SUPP_RATES                     = 1,
    WF_80211_MGMT_EID_FH_PARAMS                      = 2,  /* reserved now */
    WF_80211_MGMT_EID_DS_PARAMS                      = 3,
    WF_80211_MGMT_EID_CF_PARAMS                      = 4,
    WF_80211_MGMT_EID_TIM                            = 5,
    WF_80211_MGMT_EID_IBSS_PARAMS                    = 6,
    WF_80211_MGMT_EID_COUNTRY                        = 7,
    /* 8, 9 reserved */
    WF_80211_MGMT_EID_REQUEST                        = 10,
    WF_80211_MGMT_EID_QBSS_LOAD                      = 11,
    WF_80211_MGMT_EID_EDCA_PARAM_SET                 = 12,
    WF_80211_MGMT_EID_TSPEC                          = 13,
    WF_80211_MGMT_EID_TCLAS                          = 14,
    WF_80211_MGMT_EID_SCHEDULE                       = 15,
    WF_80211_MGMT_EID_CHALLENGE                      = 16,
    /* 17-31 reserved for challenge text extension */
    WF_80211_MGMT_EID_PWR_CONSTRAINT                 = 32,
    WF_80211_MGMT_EID_PWR_CAPABILITY                 = 33,
    WF_80211_MGMT_EID_TPC_REQUEST                    = 34,
    WF_80211_MGMT_EID_TPC_REPORT                     = 35,
    WF_80211_MGMT_EID_SUPPORTED_CHANNELS             = 36,
    WF_80211_MGMT_EID_CHANNEL_SWITCH                 = 37,
    WF_80211_MGMT_EID_MEASURE_REQUEST                = 38,
    WF_80211_MGMT_EID_MEASURE_REPORT                 = 39,
    WF_80211_MGMT_EID_QUIET                          = 40,
    WF_80211_MGMT_EID_IBSS_DFS                       = 41,
    WF_80211_MGMT_EID_ERP_INFO                       = 42,
    WF_80211_MGMT_EID_TS_DELAY                       = 43,
    WF_80211_MGMT_EID_TCLAS_PROCESSING               = 44,
    WF_80211_MGMT_EID_HT_CAPABILITY                  = 45,
    WF_80211_MGMT_EID_QOS_CAPA                       = 46,
    /* 47 reserved for Broadcom */
    WF_80211_MGMT_EID_RSN                            = 48,
    WF_80211_MGMT_EID_802_15_COEX                    = 49,
    WF_80211_MGMT_EID_EXT_SUPP_RATES                 = 50,
    WF_80211_MGMT_EID_AP_CHAN_REPORT                 = 51,
    WF_80211_MGMT_EID_NEIGHBOR_REPORT                = 52,
    WF_80211_MGMT_EID_RCPI                           = 53,
    WF_80211_MGMT_EID_MOBILITY_DOMAIN                = 54,
    WF_80211_MGMT_EID_FAST_BSS_TRANSITION            = 55,
    WF_80211_MGMT_EID_TIMEOUT_INTERVAL               = 56,
    WF_80211_MGMT_EID_RIC_DATA                       = 57,
    WF_80211_MGMT_EID_DSE_REGISTERED_LOCATION        = 58,
    WF_80211_MGMT_EID_SUPPORTED_REGULATORY_CLASSES   = 59,
    WF_80211_MGMT_EID_EXT_CHANSWITCH_ANN             = 60,
    WF_80211_MGMT_EID_HT_OPERATION                   = 61,
    WF_80211_MGMT_EID_SECONDARY_CHANNEL_OFFSET       = 62,
    WF_80211_MGMT_EID_BSS_AVG_ACCESS_DELAY           = 63,
    WF_80211_MGMT_EID_ANTENNA_INFO                   = 64,
    WF_80211_MGMT_EID_RSNI                           = 65,
    WF_80211_MGMT_EID_MEASUREMENT_PILOT_TX_INFO      = 66,
    WF_80211_MGMT_EID_BSS_AVAILABLE_CAPACITY         = 67,
    WF_80211_MGMT_EID_BSS_AC_ACCESS_DELAY            = 68,
    WF_80211_MGMT_EID_TIME_ADVERTISEMENT             = 69,
    WF_80211_MGMT_EID_RRM_ENABLED_CAPABILITIES       = 70,
    WF_80211_MGMT_EID_MULTIPLE_BSSID                 = 71,
    WF_80211_MGMT_EID_BSS_COEX_2040                  = 72,
    WF_80211_MGMT_EID_BSS_INTOLERANT_CHL_REPORT      = 73,
    WF_80211_MGMT_EID_OVERLAP_BSS_SCAN_PARAM         = 74,
    WF_80211_MGMT_EID_RIC_DESCRIPTOR                 = 75,
    WF_80211_MGMT_EID_MMIE                           = 76,
    WF_80211_MGMT_EID_ASSOC_COMEBACK_TIME            = 77,
    WF_80211_MGMT_EID_EVENT_REQUEST                  = 78,
    WF_80211_MGMT_EID_EVENT_REPORT                   = 79,
    WF_80211_MGMT_EID_DIAGNOSTIC_REQUEST             = 80,
    WF_80211_MGMT_EID_DIAGNOSTIC_REPORT              = 81,
    WF_80211_MGMT_EID_LOCATION_PARAMS                = 82,
    WF_80211_MGMT_EID_NON_TX_BSSID_CAP               = 83,
    WF_80211_MGMT_EID_SSID_LIST                      = 84,
    WF_80211_MGMT_EID_MULTI_BSSID_IDX                = 85,
    WF_80211_MGMT_EID_FMS_DESCRIPTOR                 = 86,
    WF_80211_MGMT_EID_FMS_REQUEST                    = 87,
    WF_80211_MGMT_EID_FMS_RESPONSE                   = 88,
    WF_80211_MGMT_EID_QOS_TRAFFIC_CAPA               = 89,
    WF_80211_MGMT_EID_BSS_MAX_IDLE_PERIOD            = 90,
    WF_80211_MGMT_EID_TSF_REQUEST                    = 91,
    WF_80211_MGMT_EID_TSF_RESPOSNE                   = 92,
    WF_80211_MGMT_EID_WNM_SLEEP_MODE                 = 93,
    WF_80211_MGMT_EID_TIM_BCAST_REQ                  = 94,
    WF_80211_MGMT_EID_TIM_BCAST_RESP                 = 95,
    WF_80211_MGMT_EID_COLL_IF_REPORT                 = 96,
    WF_80211_MGMT_EID_CHANNEL_USAGE                  = 97,
    WF_80211_MGMT_EID_TIME_ZONE                      = 98,
    WF_80211_MGMT_EID_DMS_REQUEST                    = 99,
    WF_80211_MGMT_EID_DMS_RESPONSE                   = 100,
    WF_80211_MGMT_EID_LINK_ID                        = 101,
    WF_80211_MGMT_EID_WAKEUP_SCHEDUL                 = 102,
    /* 103 reserved */
    WF_80211_MGMT_EID_CHAN_SWITCH_TIMING             = 104,
    WF_80211_MGMT_EID_PTI_CONTROL                    = 105,
    WF_80211_MGMT_EID_PU_BUFFER_STATUS               = 106,
    WF_80211_MGMT_EID_INTERWORKING                   = 107,
    WF_80211_MGMT_EID_ADVERTISEMENT_PROTOCOL         = 108,
    WF_80211_MGMT_EID_EXPEDITED_BW_REQ               = 109,
    WF_80211_MGMT_EID_QOS_MAP_SET                    = 110,
    WF_80211_MGMT_EID_ROAMING_CONSORTIUM             = 111,
    WF_80211_MGMT_EID_EMERGENCY_ALERT                = 112,
    WF_80211_MGMT_EID_MESH_CONFIG                    = 113,
    WF_80211_MGMT_EID_MESH_ID                        = 114,
    WF_80211_MGMT_EID_LINK_METRIC_REPORT             = 115,
    WF_80211_MGMT_EID_CONGESTION_NOTIFICATION        = 116,
    WF_80211_MGMT_EID_PEER_MGMT                      = 117,
    WF_80211_MGMT_EID_CHAN_SWITCH_PARAM              = 118,
    WF_80211_MGMT_EID_MESH_AWAKE_WINDOW              = 119,
    WF_80211_MGMT_EID_BEACON_TIMING                  = 120,
    WF_80211_MGMT_EID_MCCAOP_SETUP_REQ               = 121,
    WF_80211_MGMT_EID_MCCAOP_SETUP_RESP              = 122,
    WF_80211_MGMT_EID_MCCAOP_ADVERT                  = 123,
    WF_80211_MGMT_EID_MCCAOP_TEARDOWN                = 124,
    WF_80211_MGMT_EID_GANN                           = 125,
    WF_80211_MGMT_EID_RANN                           = 126,
    WF_80211_MGMT_EID_EXT_CAPABILITY                 = 127,
    /* 128, 129 reserved for Agere */
    WF_80211_MGMT_EID_PREQ                           = 130,
    WF_80211_MGMT_EID_PREP                           = 131,
    WF_80211_MGMT_EID_PERR                           = 132,
    /* 133-136 reserved for Cisco */
    WF_80211_MGMT_EID_PXU                            = 137,
    WF_80211_MGMT_EID_PXUC                           = 138,
    WF_80211_MGMT_EID_AUTH_MESH_PEER_EXCH            = 139,
    WF_80211_MGMT_EID_MIC                            = 140,
    WF_80211_MGMT_EID_DESTINATION_URI                = 141,
    WF_80211_MGMT_EID_UAPSD_COEX                     = 142,
    WF_80211_MGMT_EID_WAKEUP_SCHEDULE                = 143,
    WF_80211_MGMT_EID_EXT_SCHEDULE                   = 144,
    WF_80211_MGMT_EID_STA_AVAILABILITY               = 145,
    WF_80211_MGMT_EID_DMG_TSPEC                      = 146,
    WF_80211_MGMT_EID_DMG_AT                         = 147,
    WF_80211_MGMT_EID_DMG_CAP                        = 148,
    /* 149 reserved for Cisco */
    WF_80211_MGMT_EID_CISCO_VENDOR_SPECIFIC          = 150,
    WF_80211_MGMT_EID_DMG_OPERATION                  = 151,
    WF_80211_MGMT_EID_DMG_BSS_PARAM_CHANGE           = 152,
    WF_80211_MGMT_EID_DMG_BEAM_REFINEMENT            = 153,
    WF_80211_MGMT_EID_CHANNEL_MEASURE_FEEDBACK       = 154,
    /* 155-156 reserved for Cisco */
    WF_80211_MGMT_EID_AWAKE_WINDOW                   = 157,
    WF_80211_MGMT_EID_MULTI_BAND                     = 158,
    WF_80211_MGMT_EID_ADDBA_EXT                      = 159,
    WF_80211_MGMT_EID_NEXT_PCP_LIST                  = 160,
    WF_80211_MGMT_EID_PCP_HANDOVER                   = 161,
    WF_80211_MGMT_EID_DMG_LINK_MARGIN                = 162,
    WF_80211_MGMT_EID_SWITCHING_STREAM               = 163,
    WF_80211_MGMT_EID_SESSION_TRANSITION             = 164,
    WF_80211_MGMT_EID_DYN_TONE_PAIRING_REPORT        = 165,
    WF_80211_MGMT_EID_CLUSTER_REPORT                 = 166,
    WF_80211_MGMT_EID_RELAY_CAP                      = 167,
    WF_80211_MGMT_EID_RELAY_XFER_PARAM_SET           = 168,
    WF_80211_MGMT_EID_BEAM_LINK_MAINT                = 169,
    WF_80211_MGMT_EID_MULTIPLE_MAC_ADDR              = 170,
    WF_80211_MGMT_EID_U_PID                          = 171,
    WF_80211_MGMT_EID_DMG_LINK_ADAPT_ACK             = 172,
    /* 173 reserved for Symbol */
    WF_80211_MGMT_EID_MCCAOP_ADV_OVERVIEW            = 174,
    WF_80211_MGMT_EID_QUIET_PERIOD_REQ               = 175,
    /* 176 reserved for Symbol */
    WF_80211_MGMT_EID_QUIET_PERIOD_RESP              = 177,
    /* 178-179 reserved for Symbol */
    /* 180 reserved for ISO/IEC 20011 */
    WF_80211_MGMT_EID_EPAC_POLICY                    = 182,
    WF_80211_MGMT_EID_CLISTER_TIME_OFF               = 183,
    WF_80211_MGMT_EID_INTER_AC_PRIO                  = 184,
    WF_80211_MGMT_EID_SCS_DESCRIPTOR                 = 185,
    WF_80211_MGMT_EID_QLOAD_REPORT                   = 186,
    WF_80211_MGMT_EID_HCCA_TXOP_UPDATE_COUNT         = 187,
    WF_80211_MGMT_EID_HL_STREAM_ID                   = 188,
    WF_80211_MGMT_EID_GCR_GROUP_ADDR                 = 189,
    WF_80211_MGMT_EID_ANTENNA_SECTOR_ID_PATTERN      = 190,
    WF_80211_MGMT_EID_VHT_CAPABILITY                 = 191,
    WF_80211_MGMT_EID_VHT_OPERATION                  = 192,
    WF_80211_MGMT_EID_EXTENDED_BSS_LOAD              = 193,
    WF_80211_MGMT_EID_WIDE_BW_CHANNEL_SWITCH         = 194,
    WF_80211_MGMT_EID_VHT_TX_POWER_ENVELOPE          = 195,
    WF_80211_MGMT_EID_CHANNEL_SWITCH_WRAPPER         = 196,
    WF_80211_MGMT_EID_AID                            = 197,
    WF_80211_MGMT_EID_QUIET_CHANNEL                  = 198,
    WF_80211_MGMT_EID_OPMODE_NOTIF                   = 199,

    WF_80211_MGMT_EID_VENDOR_SPECIFIC                = 221,
    WF_80211_MGMT_EID_QOS_PARAMETER                  = 222,
} wf_80211_mgmt_eid_e;

/* Action category code */
typedef enum
{
    WF_80211_CATEGORY_SPECTRUM_MGMT                 = 0,
    WF_80211_CATEGORY_QOS                           = 1,
    WF_80211_CATEGORY_DLS                           = 2,
    WF_80211_CATEGORY_BACK                          = 3,
    WF_80211_CATEGORY_PUBLIC                        = 4,
    WF_80211_CATEGORY_RADIO_MEASUREMENT             = 5,
    WF_80211_CATEGORY_HT                            = 7,
    WF_80211_CATEGORY_SA_QUERY                      = 8,
    WF_80211_CATEGORY_PROTECTED_DUAL_OF_ACTION      = 9,
    WF_80211_CATEGORY_WNM                           = 10,
    WF_80211_CATEGORY_WNM_UNPROTECTED               = 11,
    WF_80211_CATEGORY_TDLS                          = 12,
    WF_80211_CATEGORY_MESH_ACTION                   = 13,
    WF_80211_CATEGORY_MULTIHOP_ACTION               = 14,
    WF_80211_CATEGORY_SELF_PROTECTED                = 15,
    WF_80211_CATEGORY_DMG                           = 16,
    WF_80211_CATEGORY_WMM                           = 17,
    WF_80211_CATEGORY_FST                           = 18,
    WF_80211_CATEGORY_UNPROT_DMG                    = 20,
    WF_80211_CATEGORY_VHT                           = 21,
    WF_80211_CATEGORY_VENDOR_SPECIFIC_PROTECTED     = 126,
    WF_80211_CATEGORY_P2P                           = 127,
} wf_80211_category_e;

/* SPECTRUM_MGMT action code */
typedef enum
{
    WF_80211_ACTION_SPCT_MSR_REQ        = 0,
    WF_80211_ACTION_SPCT_MSR_RPRT       = 1,
    WF_80211_ACTION_SPCT_TPC_REQ        = 2,
    WF_80211_ACTION_SPCT_TPC_RPRT       = 3,
    WF_80211_ACTION_SPCT_CHL_SWITCH     = 4,
} wf_80211_spectrum_mgmt_actioncode_e;

/* HT action codes */
typedef enum
{
    WF_80211_HT_ACTION_NOTIFY_CHANWIDTH     = 0,
    WF_80211_HT_ACTION_SMPS                 = 1,
    WF_80211_HT_ACTION_PSMP                 = 2,
    WF_80211_HT_ACTION_PCO_PHASE            = 3,
    WF_80211_HT_ACTION_CSI                  = 4,
    WF_80211_HT_ACTION_NONCOMPRESSED_BF     = 5,
    WF_80211_HT_ACTION_COMPRESSED_BF        = 6,
    WF_80211_HT_ACTION_ASEL_IDX_FEEDBACK    = 7,
} wf_80211_ht_actioncode_e;

/* VHT action codes */
typedef enum
{
    WF_80211_VHT_ACTION_COMPRESSED_BF   = 0,
    WF_80211_VHT_ACTION_GROUPID_MGMT    = 1,
    WF_80211_VHT_ACTION_OPMODE_NOTIF    = 2,
} wf_80211_vht_actioncode_e;

/* Self Protected Action codes */
typedef enum
{
    WF_80211_SP_RESERVED                = 0,
    WF_80211_SP_MESH_PEERING_OPEN       = 1,
    WF_80211_SP_MESH_PEERING_CONFIRM    = 2,
    WF_80211_SP_MESH_PEERING_CLOSE      = 3,
    WF_80211_SP_MGK_INFORM              = 4,
    WF_80211_SP_MGK_ACK                 = 5,
} wf_80211_self_protected_actioncode_e;

/* Mesh action codes */
typedef enum
{
    WF_80211_MESH_ACTION_LINK_METRIC_REPORT,
    WF_80211_MESH_ACTION_HWMP_PATH_SELECTION,
    WF_80211_MESH_ACTION_GATE_ANNOUNCEMENT,
    WF_80211_MESH_ACTION_CONGESTION_CONTROL_NOTIFICATION,
    WF_80211_MESH_ACTION_MCCA_SETUP_REQUEST,
    WF_80211_MESH_ACTION_MCCA_SETUP_REPLY,
    WF_80211_MESH_ACTION_MCCA_ADVERTISEMENT_REQUEST,
    WF_80211_MESH_ACTION_MCCA_ADVERTISEMENT,
    WF_80211_MESH_ACTION_MCCA_TEARDOWN,
    WF_80211_MESH_ACTION_TBTT_ADJUSTMENT_REQUEST,
    WF_80211_MESH_ACTION_TBTT_ADJUSTMENT_RESPONSE,
} wf_80211_mesh_actioncode_t;


typedef struct
{
    wf_u8 element_id;
    wf_u8 len;
    wf_u8 data[0];
} wf_80211_mgmt_ie_t;

/* U-APSD queue for WMM IEs sent by AP */
#define WF_80211_MGMT_WMM_IE_AP_QOSINFO_UAPSD               (1<<7)
#define WF_80211_MGMT_WMM_IE_AP_QOSINFO_PARAM_SET_CNT_MASK  0x0f
/* U-APSD queues for WMM IEs sent by STA */
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_AC_VO      (1<<0)
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_AC_VI      (1<<1)
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_AC_BK      (1<<2)
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_AC_BE      (1<<3)
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_AC_MASK    0x0f
/* U-APSD max SP length for WMM IEs sent by STA */
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_SP_ALL     0x00
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_SP_2       0x01
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_SP_4       0x02
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_SP_6       0x03
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_SP_MASK    0x03
#define WF_80211_MGMT_WMM_IE_STA_QOSINFO_SP_SHIFT   5

/* 802.11n HT capability MSC set */
#define WF_80211_MGMT_HT_MCS_RX_HIGHEST_MASK            0x3ff
#define WF_80211_MGMT_HT_MCS_TX_DEFINED                 0x01
#define WF_80211_MGMT_HT_MCS_TX_RX_DIFF                 0x02
/* value 0 == 1 stream etc */
#define WF_80211_MGMT_HT_MCS_TX_MAX_STREAMS_MASK        0x0C
#define WF_80211_MGMT_HT_MCS_TX_MAX_STREAMS_SHIFT       2
#define WF_80211_MGMT_HT_MCS_TX_MAX_STREAMS             4
#define WF_80211_MGMT_HT_MCS_TX_UNEQUAL_MODULATION      0x10
/*
 * 802.11n D5.0 20.3.5 / 20.6 says:
 * - indices 0 to 7 and 32 are single spatial stream
 * - 8 to 31 are multiple spatial streams using equal modulation
 *   [8..15 for two streams, 16..23 for three and 24..31 for four]
 * - remainder are multiple spatial streams using unequal modulation
 */
#define WF_80211_MGMT_HT_MCS_UNEQUAL_MODULATION_START   33
#define WF_80211_MGMT_HT_MCS_UNEQUAL_MODULATION_START_BYTE \
    (WF_80211_MGMT_HT_MCS_UNEQUAL_MODULATION_START / 8)


/* 802.11n HT capabilities masks (for cap_info) */
#define WF_80211_MGMT_HT_CAP_LDPC_CODING                0x0001
#define WF_80211_MGMT_HT_CAP_SUP_WIDTH_20_40            0x0002
#define WF_80211_MGMT_HT_CAP_SM_PS                      0x000C
#define WF_80211_MGMT_HT_CAP_SM_PS_SHIFT                2
#define WF_80211_MGMT_HT_CAP_GRN_FLD                    0x0010
#define WF_80211_MGMT_HT_CAP_SGI_20                     0x0020
#define WF_80211_MGMT_HT_CAP_SGI_40                     0x0040
#define WF_80211_MGMT_HT_CAP_TX_STBC                    0x0080
#define WF_80211_MGMT_HT_CAP_RX_STBC                    0x0300
#define WF_80211_MGMT_HT_CAP_RX_STBC_1R                 0x0100
#define WF_80211_MGMT_HT_CAP_RX_STBC_2R                 0x0200
#define WF_80211_MGMT_HT_CAP_RX_STBC_3R                 0x0300
#define WF_80211_MGMT_HT_CAP_RX_STBC_SHIFT              8
#define WF_80211_MGMT_HT_CAP_DELAY_BA                   0x0400
#define WF_80211_MGMT_HT_CAP_MAX_AMSDU                  0x0800
#define WF_80211_MGMT_HT_CAP_DSSSCCK40                  0x1000
#define WF_80211_MGMT_HT_CAP_RESERVED                   0x2000
#define WF_80211_MGMT_HT_CAP_40MHZ_INTOLERANT           0x4000
#define WF_80211_MGMT_HT_CAP_LSIG_TXOP_PROT             0x8000
/* 802.11n HT capability AMPDU settings (for ampdu_params_info) */
#define WF_80211_MGMT_HT_AMPDU_PARM_FACTOR              0x03
#define WF_80211_MGMT_HT_AMPDU_PARM_DENSITY             0x1C
#define WF_80211_MGMT_HT_AMPDU_PARM_DENSITY_SHIFT       2
/* 802.11n HT extended capabilities masks (for extended_ht_cap_info) */
#define WF_80211_MGMT_HT_EXT_CAP_PCO                    0x0001
#define WF_80211_MGMT_HT_EXT_CAP_PCO_TIME               0x0006
#define WF_80211_MGMT_HT_EXT_CAP_PCO_TIME_SHIFT         1
#define WF_80211_MGMT_HT_EXT_CAP_MCS_FB                 0x0300
#define WF_80211_MGMT_HT_EXT_CAP_MCS_FB_SHIFT           8
#define WF_80211_MGMT_HT_EXT_CAP_HTC_SUP                0x0400
#define WF_80211_MGMT_HT_EXT_CAP_RD_RESPONDER           0x0800
/*
 * Maximum length of AMPDU that the STA can receive in high-throughput (HT).
 * Length = 2 ^ (13 + max_ampdu_length_exp) - 1 (octets)
 */
enum wf_80211_mgmt_ht_max_ampdu_length_exp
{
    WF_80211_MGMT_HT_MAX_AMPDU_8K   = 0,
    WF_80211_MGMT_HT_MAX_AMPDU_16K  = 1,
    WF_80211_MGMT_HT_MAX_AMPDU_32K  = 2,
    WF_80211_MGMT_HT_MAX_AMPDU_64K  = 3,
} ;
#define WF_80211_MGMT_HT_MAX_AMPDU_FACTOR                13
/* Minimum MPDU start spacing */
enum wf_80211_mgmt_ht_min_mpdu_spacing
{
    WF_80211_MGMT_HT_MPDU_DENSITY_NONE  = 0, /* No restriction */
    WF_80211_MGMT_HT_MPDU_DENSITY_0_25  = 1, /* 1/4 usec */
    WF_80211_MGMT_HT_MPDU_DENSITY_0_5   = 2, /* 1/2 usec */
    WF_80211_MGMT_HT_MPDU_DENSITY_1     = 3, /* 1 usec */
    WF_80211_MGMT_HT_MPDU_DENSITY_2     = 4, /* 2 usec */
    WF_80211_MGMT_HT_MPDU_DENSITY_4     = 5, /* 4 usec */
    WF_80211_MGMT_HT_MPDU_DENSITY_8     = 6, /* 8 usec */
    WF_80211_MGMT_HT_MPDU_DENSITY_16    = 7, /* 16 usec */
} ;

/* for ht_param */
#define WF_80211_MGMT_HT_OP_PARAM_CHA_SEC_OFFSET            0x03
#define WF_80211_MGMT_HT_OP_PARAM_CHA_SEC_NONE              0x00
#define WF_80211_MGMT_HT_OP_PARAM_CHA_SEC_ABOVE             0x01
#define WF_80211_MGMT_HT_OP_PARAM_CHA_SEC_BELOW             0x03
#define WF_80211_MGMT_HT_OP_PARAM_CHAN_WIDTH_ANY            0x04
#define WF_80211_MGMT_HT_OP_PARAM_RIFS_MODE                 0x08
/* for operation_mode */
#define WF_80211_MGMT_HT_OP_MODE_PROTECTION                 0x0003
#define WF_80211_MGMT_HT_OP_MODE_PROTECTION_NONE            0
#define WF_80211_MGMT_HT_OP_MODE_PROTECTION_NONMEMBER       1
#define WF_80211_MGMT_HT_OP_MODE_PROTECTION_20MHZ           2
#define WF_80211_MGMT_HT_OP_MODE_PROTECTION_NONHT_MIXED     3
#define WF_80211_MGMT_HT_OP_MODE_NON_GF_STA_PRSNT           0x0004
#define WF_80211_MGMT_HT_OP_MODE_NON_HT_STA_PRSNT           0x0010
/* for stbc_param */
#define WF_80211_MGMT_HT_STBC_PARAM_DUAL_BEACON             0x0040
#define WF_80211_MGMT_HT_STBC_PARAM_DUAL_CTS_PROT           0x0080
#define WF_80211_MGMT_HT_STBC_PARAM_STBC_BEACON             0x0100
#define WF_80211_MGMT_HT_STBC_PARAM_LSIG_TXOP_FULLPROT      0x0200
#define WF_80211_MGMT_HT_STBC_PARAM_PCO_ACTIVE              0x0400
#define WF_80211_MGMT_HT_STBC_PARAM_PCO_PHASE               0x0800



#define WF_80211_MAX_SSID_LEN       32
typedef wf_u8 wf_80211_mgmt_ssid_t[WF_80211_MAX_SSID_LEN + 1];

#define WF_80211_IES_SIZE_MAX               768
#define WF_WLAN_MGMT_TAG_PROBEREQ_P2P_SIZE_MAX \
    (WF_OFFSETOF(wf_80211_mgmt_t, probe_req.variable) + WF_80211_IES_SIZE_MAX)

#define WF_80211_MGMT_BEACON_SIZE_MAX \
    (WF_OFFSETOF(wf_80211_mgmt_t, beacon.variable) + WF_80211_IES_SIZE_MAX)
#define WF_80211_MGMT_PROBERSP_SIZE_MAX \
        (WF_OFFSETOF(wf_80211_mgmt_t, probe_resp.variable) + WF_80211_IES_SIZE_MAX)
#define WF_80211_MGMT_AUTH_SIZE_MAX \
    (WF_OFFSETOF(wf_80211_mgmt_t, auth_seq3) + sizeof(struct auth_seq3_ie) + 12) /* +12: fix for apple smart remote terminal */
#define WF_80211_MGMT_DEAUTH_SIZE_MAX \
    (WF_OFFSETOF(wf_80211_mgmt_t, deauth) + sizeof(struct deauth_ie))
#define WF_80211_MGMT_ASSOC_SIZE_MAX \
    (WF_OFFSETOF(wf_80211_mgmt_t, assoc_req.variable) + WF_80211_IES_SIZE_MAX)
#define WF_80211_MGMT_DISASSOC_SIZE_MAX \
    (WF_OFFSETOF(wf_80211_mgmt_t, disassoc) + sizeof(struct disassoc_ie))

/* cipher suite selectors */
#define WF_80211_RSN_CIPHER_SUITE_USE_GROUP     0x000FAC00
#define WF_80211_RSN_CIPHER_SUITE_WEP40         0x000FAC01
#define WF_80211_RSN_CIPHER_SUITE_TKIP          0x000FAC02
/* reserved: 0x000FAC03 */
#define WF_80211_RSN_CIPHER_SUITE_CCMP          0x000FAC04
#define WF_80211_RSN_CIPHER_SUITE_WEP104        0x000FAC05
#define WF_80211_RSN_CIPHER_SUITE_AES_CMAC      0x000FAC06
#define WF_80211_RSN_CIPHER_SUITE_GCMP          0x000FAC08
#define WF_80211_RSN_CIPHER_SUITE_GCMP_256      0x000FAC09
#define WF_80211_RSN_CIPHER_SUITE_CCMP_256      0x000FAC0A
#define WF_80211_RSN_CIPHER_SUITE_BIP_GMAC_128  0x000FAC0B
#define WF_80211_RSN_CIPHER_SUITE_BIP_GMAC_256  0x000FAC0C
#define WF_80211_RSN_CIPHER_SUITE_BIP_CMAC_256  0x000FAC0D

#define WF_80211_RSN_CIPHER_SUITE_SMS4          0x00147201

/* AKM suite selectors */
#define WF_80211_AKM_SUITE_8021X                0x000FAC01
#define WF_80211_AKM_SUITE_PSK                  0x000FAC02
#define WF_80211_AKM_SUITE_8021X_SHA256         0x000FAC05
#define WF_80211_AKM_SUITE_PSK_SHA256           0x000FAC06
#define WF_80211_AKM_SUITE_TDLS                 0x000FAC07
#define WF_80211_AKM_SUITE_SAE                  0x000FAC08
#define WF_80211_AKM_SUITE_FT_OVER_SAE          0x000FAC09

#define WF_80211_MAX_KEY_LEN                    32

#define WF_80211_PMKID_LEN                      16

#define WF_80211_OUI_WFA                        0x506f9a
#define WF_80211_OUI_TYPE_WFA_P2P               9
#define WF_80211_OUI_MICROSOFT                  0x0050f2
#define WF_80211_OUI_TYPE_MICROSOFT_WPA         1
#define WF_80211_OUI_TYPE_MICROSOFT_WMM         2
#define WF_80211_OUI_TYPE_MICROSOFT_WPS         4

/* Authentication algorithms */
typedef enum
{
    WF_80211_AUTH_ALGO_OPEN = 0,
    WF_80211_AUTH_ALGO_SHARED_KEY,
    WF_80211_AUTH_ALGO_FT,
    WF_80211_AUTH_ALGO_SAE,
    WF_80211_AUTH_ALGO_LEAP = 128,
    WF_80211_AUTH_ALGO_AUTO,
} wf_80211_auth_algo_e;

typedef enum
{
    WF_80211_AUTH_SEQ_1 = 1,
    WF_80211_AUTH_SEQ_2,
    WF_80211_AUTH_SEQ_3,
    WF_80211_AUTH_SEQ_4,
} wf_80211_auth_seq_e;

typedef enum
{
    WF_80211_HIDDEN_SSID_NOT_IN_USE = 0,
    WF_80211_HIDDEN_SSID_ZERO_LEN,
    WF_80211_HIDDEN_SSID_ZERO_CONTENTS,
} wf_80211_hidden_ssid_e;

#define WF_80211_AUTH_CHALLENGE_LEN             128

wf_u8 *wf_wlan_get_ie(wf_u8 * pbuf, wf_s32 index, wf_s32 * len, wf_s32 limit);
wf_u8 *wf_wlan_get_wps_ie(wf_u8 *temp_ie, wf_u32 temp_len, wf_u8 *wps_ie, wf_u32 *ie_len);
wf_u8 *wf_wlan_get_wps_attr(wf_u8 * wps_ie, wf_u32 wps_ielen, wf_u16 target_attr_id, wf_u8 * buf_attr, wf_u32 * len_attr, wf_u8 flag);
wf_u8 *wf_wlan_get_wps_attr_content(wf_u8 flag, wf_u8 * wps_ie, wf_u32 wps_ielen, wf_u16 target_attr_id, wf_u8 * buf_content, wf_u32 * len_content);


static wf_inline
wf_80211_frame_e wf_80211_get_frame_type (wf_80211_frame_ctrl_t fc)
{
    return (wf_80211_frame_e)(wf_le16_to_cpu(fc) & (WF_80211_FCTL_FTYPE | WF_80211_FCTL_STYPE));
}

static wf_inline
void wf_80211_set_frame_type (wf_80211_frame_ctrl_t *pfc, wf_80211_frame_e type)
{
    wf_80211_frame_ctrl_t tmp = *pfc;

    tmp &= ~(WF_80211_FCTL_FTYPE | WF_80211_FCTL_STYPE);
    tmp |= type;
    *pfc = tmp;
}

static wf_inline
int wf_80211_get_protected (wf_80211_frame_ctrl_t fc)
{
    return !!(wf_le16_to_cpu(fc) & WF_80211_FCTL_PROTECTED);
}

static wf_inline
void wf_80211_set_protected (wf_80211_frame_ctrl_t *pfc)
{
    *pfc |= WF_80211_FCTL_PROTECTED;
}

static wf_inline
wf_bool wf_80211_has_order (wf_80211_frame_ctrl_t pfc)
{
    return (wf_bool)(!!(pfc & wf_cpu_to_le16(WF_80211_FCTL_ORDER)));
}

int wf_80211_mgmt_ies_search (void *pies, wf_u16 ies_len, wf_u8 cmp_id,
                              wf_80211_mgmt_ie_t **ppie);


int wf_80211_mgmt_ies_search_with_oui (void *pies, wf_u16 ies_len,
                                       wf_u8 cmp_id, wf_u8 *oui,
                                       wf_80211_mgmt_ie_t **ppie);


wf_u8 *wf_80211_set_fixed_ie(wf_u8 *pbuf, wf_u32 len, wf_u8 *source, wf_u16 *frlen);



struct ieee80211_snap_hdr
{
    wf_u8 dsap;
    wf_u8 ssap;
    wf_u8 ctrl;
    wf_u8 oui[3];
};

#define SNAP_HDR_SIZE   sizeof(struct ieee80211_snap_hdr)



wf_bool is_snap_hdr(wf_u8 *phdr);
wf_u8 *get_bssid(wf_u8 *pbuf);
int wf_80211_mgmt_wpa_parse (void *pwpa, wf_u16 len,
                             wf_u32 *pmulticast_cipher, wf_u32 *punicast_cipher);
int wf_80211_mgmt_wpa_survey (void *data, wf_u16 data_len,
                              void **pwpa_ie, wf_u16 *pwpa_ie_len,
                              wf_u32 *pmulticast_cipher, wf_u32 *punicast_cipher);
int wf_80211_mgmt_rsn_parse (void *prsn, wf_u16 len,
                             wf_u32 *pgroup_cipher, wf_u32 *pairwise_cipher);
int wf_80211_mgmt_rsn_survey (void *data, wf_u16 data_len,
                              void **prsn_ie, wf_u16 *prsn_ie_len,
                              wf_u32 *pgroup_cipher, wf_u32 *pairwise_cipher);
int wf_80211_mgmt_wmm_parse (void *pwmm, wf_u16 len);
int wf_wlan_get_sec_ie(wf_u8 *in_ie, wf_u32 in_len,
                       wf_u8 *rsn_ie, wf_u16 *rsn_len,
                       wf_u8 *wpa_ie, wf_u16 *wpa_len,
                       wf_u8 flag);


wf_u8 wf_wlan_check_is_wps_ie(wf_u8 *ie_ptr, wf_u32 *wps_ielen);


#define WF_80211_CMD_SET_WPA_PARAM                                                      1
#define WF_80211_CMD_SET_WPA_IE                                                         2
#define WF_80211_CMD_SET_ENCRYPTION                                                     3
#define WF_80211_CMD_MLME                                                               4

enum MGN_RATE
{
    MGN_1M = 0x02,
    MGN_2M = 0x04,
    MGN_5_5M = 0x0B,
    MGN_6M = 0x0C,
    MGN_9M = 0x12,
    MGN_11M = 0x16,
    MGN_12M = 0x18,
    MGN_18M = 0x24,
    MGN_24M = 0x30,
    MGN_36M = 0x48,
    MGN_48M = 0x60,
    MGN_54M = 0x6C,
    MGN_MCS32 = 0x7F,
    MGN_MCS0,
    MGN_MCS1,
    MGN_MCS2,
    MGN_MCS3,
    MGN_MCS4,
    MGN_MCS5,
    MGN_MCS6,
    MGN_MCS7,
    MGN_UNKNOWN
};

enum RATEID_IDX
{
    RATEID_IDX_BGN_40M_1SS = 1,
    RATEID_IDX_BGN_20M_1SS_BN = 3,
    RATEID_IDX_GN_N1SS = 5,
    RATEID_IDX_BG = 6,
    RATEID_IDX_G = 7,
    RATEID_IDX_B = 8,
    RATEID_IDX_MIX1 = 11,
    RATEID_IDX_MIX2 = 12,
};

enum WF_CAPABILITY
{
    cIBSS = 0x0002,
    cShortPreamble = 0x0020,
    cShortSlotTime = 0x0400,
};

enum VCS_TYPE
{
    NONE_VCS,
    RTS_CTS,
    CTS_TO_SELF
};

enum
{
    WF_HOSTAPD_FLUSH                    = 1,
    WF_HOSTAPD_ADD_STA                  = 2,
    WF_HOSTAPD_REMOVE_STA               = 3,
    WF_HOSTAPD_GET_INFO_STA             = 4,
    WF_HOSTAPD_GET_WPAIE_STA            = 5,
    WF_SET_ENCRYPTION                   = 6,
    WF_GET_ENCRYPTION                   = 7,
    WF_HOSTAPD_SET_FLAGS_STA            = 8,
    WF_HOSTAPD_GET_RID                  = 9,
    WF_HOSTAPD_SET_RID                  = 10,
    WF_HOSTAPD_SET_ASSOC_AP_ADDR        = 11,
    WF_HOSTAPD_SET_GENERIC_ELEMENT      = 12,
    WF_HOSTAPD_MLME                     = 13,
    WF_HOSTAPD_SCAN_REQ                 = 14,
    WF_HOSTAPD_STA_CLEAR_STATS          = 15,
    WF_HOSTAPD_SET_BEACON               = 16,
    WF_HOSTAPD_SET_WPS_BEACON           = 17,
    WF_HOSTAPD_SET_WPS_PROBE_RESP       = 18,
    WF_HOSTAPD_SET_WPS_ASSOC_RESP       = 19,
    WF_HOSTAPD_SET_HIDDEN_SSID          = 20,
    WF_HOSTAPD_SET_MACADDR_ACL          = 21,
    WF_HOSTAPD_ACL_ADD_STA              = 22,
    WF_HOSTAPD_ACL_REMOVE_STA           = 23,
};


#define SEC_KEY_1         (1<<0)
#define SEC_KEY_2         (1<<1)
#define SEC_KEY_3         (1<<2)
#define SEC_KEY_4         (1<<3)
#define SEC_ACTIVE_KEY    (1<<4)
#define SEC_AUTH_MODE     (1<<5)
#define SEC_UNICAST_GROUP (1<<6)
#define SEC_LEVEL         (1<<7)
#define SEC_ENABLED       (1<<8)

#define SEC_LEVEL_0      0
#define SEC_LEVEL_1      1
#define SEC_LEVEL_2      2
#define SEC_LEVEL_2_CKIP 3
#define SEC_LEVEL_3      4

#define WEP_KEYS 4  /* WEP�?�?4组�?��?*/
#define WEP_KEY_LEN 13

#define IEEE_CRYPT_ALG_NAME_LEN         16

#define CIPHER_SUITE_NONE     WF_BIT(0)
#define CIPHER_SUITE_WEP40    WF_BIT(1)
#define CIPHER_SUITE_WEP104   WF_BIT(2)
#define CIPHER_SUITE_TKIP     WF_BIT(3)
#define CIPHER_SUITE_CCMP     WF_BIT(4)

enum WIFI_STATUS_CODE
{
    _STATS_SUCCESSFUL_          = 0,
    _STATS_FAILURE_             = 1,
    _STATS_SEC_DISABLED_        = 5,
    _STATS_NOT_IN_SAME_BSS_     = 7,
    _STATS_CAP_FAIL_            = 10,
    _STATS_NO_ASOC_             = 11,
    _STATS_OTHER_               = 12,
    _STATS_NO_SUPP_ALG_         = 13,
    _STATS_OUT_OF_AUTH_SEQ_     = 14,
    _STATS_CHALLENGE_FAIL_      = 15,
    _STATS_AUTH_TIMEOUT_        = 16,
    _STATS_UNABLE_HANDLE_STA_   = 17,
    _STATS_RATE_FAIL_           = 18,
    _STATS_REFUSED_TEMPORARILY_ = 30,
    _STATS_DECLINE_REQ_         = 37,
    _STATS_INVALID_PARAMETERS_  = 38,
    _STATS_INVALID_RSNIE_       = 72,
};


typedef struct
{
    wf_u32 cmd;
    wf_80211_addr_t sta_addr;
    union
    {
        struct
        {
            wf_u8 name;
            wf_u32 value;
        } wpa_param;
        struct
        {
            wf_u32 len;
            wf_u8 reserved[32];
            wf_u8 data[0];
        } wpa_ie;
        struct
        {
            int command;
            int reason_code;
        } mlme;
        struct
        {
            wf_u8 alg[IEEE_CRYPT_ALG_NAME_LEN];
            wf_u8 set_tx;
            wf_u32 err;
            wf_u8 idx;
            wf_u8 seq[8]; /* sequence counter (set: RX, get: TX) */
            wf_u16 key_len;
            wf_u8 key[0];
        } crypt;
#ifdef CFG_ENABLE_AP_MODE
        struct
        {
            wf_u8 reserved[2]; /* for set max_num_sta */
            wf_u8 buf[0];
        } bcn_ie;
#endif
    } u;
} ieee_param;


enum NETWORK_TYPE
{
    WIRELESS_INVALID = 0,
    WIRELESS_11B = WF_BIT(0),
    WIRELESS_11G = WF_BIT(1),
    WIRELESS_11_24N = WF_BIT(3),
    WIRELESS_AUTO = WF_BIT(5),

    WIRELESS_11BG = (WIRELESS_11B | WIRELESS_11G),
    WIRELESS_11G_24N = (WIRELESS_11G | WIRELESS_11_24N),
    WIRELESS_11B_24N = (WIRELESS_11B | WIRELESS_11_24N),
    WIRELESS_11BG_24N = (WIRELESS_11B | WIRELESS_11G | WIRELESS_11_24N),

    WIRELESS_MODE_MAX = (WIRELESS_11B | WIRELESS_11G | WIRELESS_11_24N),
};


#define IEEE80211_CCK_RATE_1MB              0x02
#define IEEE80211_CCK_RATE_2MB              0x04
#define IEEE80211_CCK_RATE_5MB              0x0B
#define IEEE80211_CCK_RATE_11MB             0x16
#define IEEE80211_OFDM_RATE_LEN             8
#define IEEE80211_OFDM_RATE_6MB             0x0C
#define IEEE80211_OFDM_RATE_9MB             0x12
#define IEEE80211_OFDM_RATE_12MB            0x18
#define IEEE80211_OFDM_RATE_18MB            0x24
#define IEEE80211_OFDM_RATE_24MB            0x30
#define IEEE80211_OFDM_RATE_36MB            0x48
#define IEEE80211_OFDM_RATE_48MB            0x60
#define IEEE80211_OFDM_RATE_54MB            0x6C
#define IEEE80211_BASIC_RATE_MASK           0x80

#define IEEE80211_CCK_RATE_1MB_MASK         (1<<0)
#define IEEE80211_CCK_RATE_2MB_MASK         (1<<1)
#define IEEE80211_CCK_RATE_5MB_MASK         (1<<2)
#define IEEE80211_CCK_RATE_11MB_MASK        (1<<3)
#define IEEE80211_OFDM_RATE_6MB_MASK        (1<<4)
#define IEEE80211_OFDM_RATE_9MB_MASK        (1<<5)
#define IEEE80211_OFDM_RATE_12MB_MASK       (1<<6)
#define IEEE80211_OFDM_RATE_18MB_MASK       (1<<7)
#define IEEE80211_OFDM_RATE_24MB_MASK       (1<<8)
#define IEEE80211_OFDM_RATE_36MB_MASK       (1<<9)
#define IEEE80211_OFDM_RATE_48MB_MASK       (1<<10)
#define IEEE80211_OFDM_RATE_54MB_MASK       (1<<11)

#define IEEE80211_CCK_RATES_MASK            0x0000000F
#define IEEE80211_CCK_BASIC_RATES_MASK  (IEEE80211_CCK_RATE_1MB_MASK | \
    IEEE80211_CCK_RATE_2MB_MASK)
#define IEEE80211_CCK_DEFAULT_RATES_MASK    (IEEE80211_CCK_BASIC_RATES_MASK | \
        IEEE80211_CCK_RATE_5MB_MASK | \
        IEEE80211_CCK_RATE_11MB_MASK)

#define IEEE80211_OFDM_RATES_MASK       0x00000FF0
#define IEEE80211_OFDM_BASIC_RATES_MASK (IEEE80211_OFDM_RATE_6MB_MASK | \
    IEEE80211_OFDM_RATE_12MB_MASK | \
    IEEE80211_OFDM_RATE_24MB_MASK)
#define IEEE80211_OFDM_DEFAULT_RATES_MASK   (IEEE80211_OFDM_BASIC_RATES_MASK | \
    IEEE80211_OFDM_RATE_9MB_MASK  | \
    IEEE80211_OFDM_RATE_18MB_MASK | \
    IEEE80211_OFDM_RATE_36MB_MASK | \
    IEEE80211_OFDM_RATE_48MB_MASK | \
    IEEE80211_OFDM_RATE_54MB_MASK)
#define IEEE80211_DEFAULT_RATES_MASK (IEEE80211_OFDM_DEFAULT_RATES_MASK | \
                                IEEE80211_CCK_DEFAULT_RATES_MASK)

#define IEEE80211_NUM_OFDM_RATES            8
#define IEEE80211_NUM_CCK_RATES             4
#define IEEE80211_OFDM_SHIFT_MASK_A         4
#define IEEE80211_ADDR3_LEN                 24

#define NDIS_802_11_LENGTH_SSID             32
#define NDIS_802_11_LENGTH_RATES            8
#define NDIS_802_11_LENGTH_RATES_EX         16

#define mac_addr_equal(pdst,psrc)   \
((!memcmp(pdst, psrc, 6)) ? wf_true : wf_false)

#define array_equal(pdst,psrc,size) \
((!memcmp(pdst, psrc, size)) ? wf_true : wf_false)

#define is_mcast_addr(addr) \
(((addr[0] != 0xff) && (0x01 & addr[0])) ? wf_true : wf_false)

#define is_bcast_addr(addr) \
(((addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff) ? wf_true : wf_false)

#define is_zero_addr(addr)  \
((addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]) == 0x0 ? wf_true : wf_false)

wf_inline static wf_bool wf_80211_is_mcast_addr (wf_80211_addr_t addr)
{
    return (wf_bool)((addr[0] != 0xFF) && (addr[0] & 0x01));
}

wf_inline static wf_bool wf_80211_is_bcast_addr (wf_80211_addr_t addr)
{
    return (wf_bool)((addr[0] & addr[1] & addr[2] &
                      addr[3] & addr[4] & addr[5]) == 0xFF);
}

wf_inline static wf_bool wf_80211_is_zero_addr (wf_80211_addr_t addr)
{
    return (wf_bool)((addr[0] | addr[1] | addr[2] |
                      addr[3] | addr[4] | addr[5]) == 0x00);
}

wf_inline static wf_bool wf_80211_is_valid_bssid (wf_80211_bssid_t bssid)
{
    return (wf_bool)(!wf_80211_is_bcast_addr(bssid) &&
                     !wf_80211_is_zero_addr(bssid) &&
                     !wf_80211_is_mcast_addr(bssid));
}

wf_inline static
wf_bool wf_80211_is_same_addr (wf_80211_addr_t addr1, wf_80211_addr_t addr2)
{
    return (wf_bool)(!wf_memcmp(addr1, addr2, sizeof(wf_80211_addr_t)));
}



int wf_ch_2_freq(int ch);
int freq_2_ch(int freq);

#define WF_MAX_WPA_IE_LEN         (256)
#define WF_MAX_WPS_IE_LEN         (512)
#define WF_MAX_P2P_IE_LEN         (256)
#define WF_MAX_WFD_IE_LEN         (128)
#define RSN_HEADER_LEN            4
#define RSN_SELECTOR_LEN          4

#define WPA_SELECTOR_LEN 4



#if defined(_WIN32) || defined(_WIN64)
#pragma pack(1)
#endif

struct wf_ethhdr
{
    wf_u8  dest[WF_ETH_ALEN];
    wf_u8  src[WF_ETH_ALEN];
    wf_u16 type;
} wf_packed;


struct wl_ieee80211_hdr
{
    wf_u16 frame_ctl;
    wf_u16 duration_id;
    wf_u8 addr1[MAC_ADDR_LEN];
    wf_u8 addr2[MAC_ADDR_LEN];
    wf_u8 addr3[MAC_ADDR_LEN];
    wf_u16 seq_ctl;
    wf_u8 addr4[MAC_ADDR_LEN];
} wf_packed;


struct wl_ieee80211_hdr_3addr
{
    wf_u16 frame_ctl;
    wf_u16 duration_id;
    wf_u8 addr1[MAC_ADDR_LEN];
    wf_u8 addr2[MAC_ADDR_LEN];
    wf_u8 addr3[MAC_ADDR_LEN];
    wf_u16 seq_ctl;
} wf_packed;

struct wl_ieee80211_ht_oper_info
{
    unsigned char control_chan;
    unsigned char ht_param;
    unsigned short operation_mode;
    unsigned short stbc_param;
    unsigned char basic_set[16];
} wf_packed;

struct ADDBA_request
{
    unsigned char dialog_token;
    unsigned short BA_para_set;
    unsigned short BA_timeout_value;
    unsigned short ba_starting_seqctrl;
} wf_packed;

typedef struct
{
    wf_80211_frame_ctrl_t frame_control;
    wf_80211_duration_t duration;
    wf_80211_addr_t da;
    wf_80211_addr_t sa;
    wf_80211_bssid_t bssid;
    wf_80211_seq_ctrl_t seq_ctrl;
    union
    {
        /* beacon */
        struct beacon_ie
        {
            wf_80211_mgmt_timestamp_t timestamp;
            wf_80211_mgmt_beacon_interval_t intv;
            wf_80211_mgmt_capab_t capab;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params, TIM */
            wf_u8 variable[0];
        } wf_packed beacon, probe_resp;
        /* probe */
        struct
        {
            /* only variable items: SSID, Supported rates */
            wf_u8 variable[1];
        } wf_packed probe_req;
        /* auth */
        struct auth_ie
        {
            wf_u16 auth_alg;
            wf_u16 auth_transaction;
            wf_u16 status_code;
            /* possibly followed Challenge text */
            wf_u8 variable[0];
        } wf_packed auth;
        struct auth_seq3_ie
        {
            wf_u32 iv;
            wf_u16 auth_alg;
            wf_u16 auth_transaction;
            wf_u16 status_code;
            /* possibly followed Challenge text */
            wf_u8 variable[WF_OFFSETOF(wf_80211_mgmt_ie_t, data) + WF_80211_AUTH_CHALLENGE_LEN];
            wf_u32 icv;
        } wf_packed auth_seq3;
        struct deauth_ie
        {
            wf_u16 reason_code;
        } wf_packed deauth;
        /* assoc */
        struct disassoc_ie
        {
            wf_u16 reason_code;
        } wf_packed disassoc;
        struct assoc_req_ie
        {
            wf_u16 capab_info;
            wf_u16 listen_interval;
            /* followed by SSID and Supported rates */
            wf_u8 variable[0];
        } wf_packed assoc_req;
        struct
        {
            wf_u16 capab_info;
            wf_u16 status_code;
            wf_u16 aid;
            /* followed by Supported rates */
            wf_u8 variable[0];
        } wf_packed assoc_resp;
        struct
        {
            wf_u16 capab_info;
            wf_u16 listen_interval;
            wf_80211_addr_t current_ap;
            /* followed by SSID and Supported rates */
            wf_u8 variable[0];
        } wf_packed reassoc_req;
        struct
        {
            wf_u16 capab_info;
            wf_u16 status_code;
            wf_u16 aid;
            /* followed by Supported rates */
            wf_u8 variable[0];
        } wf_packed reassoc_resp;
        struct action_ie
        {
            wf_u8 action_category;
            wf_u8 action_field;
            wf_u8 variable[0];
        } wf_packed action;
        wf_u8 null_func[1];
    };
} wf_packed wf_80211_mgmt_t;

/*
 * data frame
 */
typedef struct
{
    wf_80211_frame_ctrl_t frame_control;
    wf_80211_duration_t duration;
    wf_80211_addr_t addr1;
    wf_80211_addr_t addr2;
    wf_80211_bssid_t addr3;
    wf_80211_seq_ctrl_t seq_ctrl;
    wf_u8 body[0];
} wf_packed wf_80211_data_t;


/* snap header */
typedef struct wf_80211_snap_header
{
    wf_u8 dsap;
    wf_u8 ssap;
    wf_u8 ctrl;
    wf_u8 oui[3];
} wf_packed wf_80211_snap_header_st;

/**
 * wf_80211_mgmt_ht_operation_t - DSSS Parameter Set element
 *
 * This structure is the "DSSS Parameter" as
 * described in 802.11n-2016 9.4.2.4
 */
typedef struct
{
    wf_u8 current_channel;
} wf_packed wf_80211_mgmt_dsss_parameter_t;

typedef struct
{
    wf_80211_frame_ctrl_t frame_control;
    wf_u16 duration_id;
    wf_80211_addr_t addr1;
    wf_80211_addr_t addr2;
    wf_80211_addr_t addr3;
    wf_u16 seq_ctrl;
    wf_80211_addr_t addr4;
} wf_packed wf_80211_hdr_t;

typedef struct
{
    wf_80211_frame_ctrl_t frame_control;
    wf_u16 duration_id;
    wf_80211_addr_t addr1;
    wf_80211_addr_t addr2;
    wf_80211_addr_t addr3;
    wf_u16 seq_ctrl;
} wf_packed wf_80211_hdr_3addr_t;

typedef wf_u16 wf_80211_aid;
typedef struct
{
    wf_80211_frame_ctrl_t frame_control;
    wf_80211_duration_t duration_id;
    wf_80211_addr_t addr1;
    wf_80211_addr_t addr2;
    wf_80211_addr_t addr3;
    wf_u16 seq_ctrl;
    wf_u16 qos_ctrl;
} wf_packed wf_80211_qos_hdr_t;


/* Channel switch timing */
typedef struct
{
    wf_u16 switch_time;
    wf_u16 switch_timeout;
} wf_packed wf_80211_ch_switch_timing_t;


/* Management MIC information element (IEEE 802.11w) */
typedef struct
{
    wf_u8 element_id;
    wf_u8 length;
    wf_u16 key_id;
    wf_u8 sequence_number[6];
    wf_u8 mic[8];
} wf_packed wf_80211_mmie_t;

/* Management MIC information element (IEEE 802.11w) for GMAC and CMAC-256 */
typedef struct
{
    wf_u8 element_id;
    wf_u8 length;
    wf_u16 key_id;
    wf_u8 sequence_number[6];
    wf_u8 mic[16];
} wf_packed wf_80211_mmie_16_t;

typedef struct
{
    wf_u8 element_id;
    wf_u8 len;
    wf_u8 oui[3];
    wf_u8 oui_type;
} wf_packed wf_80211_vendor_ie_t;

typedef struct
{
    wf_u8 aci_aifsn; /* AIFSN, ACM, ACI */
    wf_u8 cw; /* ECWmin, ECWmax (CW = 2^ECW - 1) */
    wf_u16 txop_limit;
} wf_packed wf_80211_wmm_ac_param_t;

typedef struct
{
    wf_u8 element_id; /* Element ID: 221 (0xdd); */
    wf_u8 len; /* Length: 24 */
    /* required fields for WMM version 1 */
    wf_u8 oui[3]; /* 00:50:f2 */
    wf_u8 oui_type; /* 2 */
    wf_u8 oui_subtype; /* 0 */
    wf_u8 version; /* 1 for WMM version 1.0 */
    wf_u8 qos_info; /* AP/STA specific QoS info */
    wf_u8 reserved; /* 0 */
    /* AC_BE, AC_BK, AC_VI, AC_VO */
    wf_80211_wmm_ac_param_t ac[4];
} wf_packed wf_80211_wmm_param_ie_t;

typedef struct
{
    wf_u8 element_id; /* Element ID: 221 (0xdd); */
    wf_u8 len; /* Length: 24 */
    wf_u8 oui[3]; /* 50:6F:9A */
    wf_u8 oui_type; /* Identifying the type or version of P2P IE. Setting to 0x09 indicates Wi-Fi Alliance P2P v1.0. */
    wf_u8 p2p_attrs[0];/*One of more P2P attributes appear in the P2P IE.*/
} wf_packed wf_80211_p2p_param_ie_t;

/* mcs element */
/**
 * wf_80211_mgmt_ht_cap_mcs_info_t - MCS information
 * @rx_mask: RX mask
 * @rx_highest: highest supported RX rate. If set represents
 *  the highest supported RX data rate in units of 1 Mbps.
 *  If this field is 0 this value should not be used to
 *  consider the highest RX data rate supported.
 * @tx_params: TX parameters
 */
#define WF_80211_MGMT_HT_MCS_MASK_LEN                   10
typedef struct
{
    wf_u8 rx_mask[WF_80211_MGMT_HT_MCS_MASK_LEN];
    wf_u16 rx_highest;
    wf_u8 tx_params;
    wf_u8 reserved[3];
} wf_packed wf_80211_mgmt_ht_cap_mcs_info_t;

/**
 * struct ieee80211_ht_cap - HT capabilities
 *
 * This structure is the "HT capabilities element" as
 * described in 802.11n D5.0 7.3.2.57
 */
typedef struct
{
    wf_u16 cap_info;
    wf_u8 ampdu_params_info;
    /* 16 bytes MCS information */
    union
    {
        wf_u8 supp_mcs_set[WF_MCS_NUM];
        wf_80211_mgmt_ht_cap_mcs_info_t mcs_info;
    };
    wf_u16 extended_ht_cap_info;
    wf_u32 tx_BF_cap_info;
    wf_u8 antenna_selection_info;
} wf_packed wf_80211_mgmt_ht_cap_t;

/**
 * struct ieee80211_ht_operation - HT operation IE
 *
 * This structure is the "HT operation element" as
 * described in 802.11n-2009 7.3.2.57
 */
typedef struct
{
    wf_u8 primary_chan;
    wf_u8 ht_param;
    wf_u16 operation_mode;
    wf_u16 stbc_param;
    wf_u8 basic_set[16];
} wf_packed wf_80211_mgmt_ht_operation_t;

#define SET_CFG80211_REPORT_MGMT(w, t, v) (w->report_mgmt |= (v?BIT(t >> 4):0))
#define GET_CFG80211_REPORT_MGMT(w, t) ((w->report_mgmt & BIT(t >> 4)) > 0)
#define CLR_CFG80211_REPORT_MGMT(w, t, v) (w->report_mgmt &= (~BIT(t >> 4)))

#if defined(_WIN32) || defined(_WIN64)
#pragma pack()
#endif

#endif

