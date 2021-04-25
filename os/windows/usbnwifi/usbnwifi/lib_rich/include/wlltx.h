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

#ifndef __wlan9086X_XMIT_H__
#define __wlan9086X_XMIT_H__

#ifndef __INC_HAL9086XDESC_H
#define __INC_HAL9086XDESC_H

#define GET_RX_STATUS_DESC_PKT_LEN_9086X(__pRxStatusDesc)			LE_BITS_TO_4BYTE( __pRxStatusDesc, 0, 14)
#define GET_RX_STATUS_DESC_CRC32_9086X(__pRxStatusDesc) 		    LE_BITS_TO_4BYTE( __pRxStatusDesc, 14, 1)
#define GET_RX_STATUS_DESC_SECURITY_9086X(__pRxStatusDesc)			LE_BITS_TO_4BYTE( __pRxStatusDesc, 20, 3)
#define GET_RX_STATUS_DESC_QOS_9086X(__pRxStatusDesc)				LE_BITS_TO_4BYTE( __pRxStatusDesc, 23, 1)
#define GET_RX_STATUS_DESC_PHY_STATUS_9086X(__pRxStatusDesc)		LE_BITS_TO_4BYTE( __pRxStatusDesc, 26, 1)

#define GET_RX_STATUS_DESC_TID_9086X(__pRxStatusDesc) 				LE_BITS_TO_4BYTE(__pRxStatusDesc +4,  0, 4)
#define GET_RX_STATUS_DESC_AMSDU_9086X(__pRxStatusDesc)				LE_BITS_TO_4BYTE(__pRxStatusDesc +4,  4, 1)
#define GET_RX_STATUS_DESC_MORE_DATA_9086X(__pRxStatusDesc)			LE_BITS_TO_4BYTE( __pRxStatusDesc+4,  5, 1)
#define GET_RX_STATUS_DESC_MORE_FRAG_9086X(__pRxStatusDesc)			LE_BITS_TO_4BYTE( __pRxStatusDesc+4,  6, 1)

#define GET_RX_STATUS_DESC_SEQ_9086X(__pRxStatusDesc)					LE_BITS_TO_4BYTE( __pRxStatusDesc+8, 0, 12)
#define GET_RX_STATUS_DESC_FRAG_9086X(__pRxStatusDesc)				LE_BITS_TO_4BYTE( __pRxStatusDesc+8, 12, 4)
#define GET_RX_STATUS_DESC_RPT_SEL_9086X(__pRxStatusDesc)			LE_BITS_TO_4BYTE( __pRxStatusDesc+8, 28, 1)

// Add for Phy status. 2020/12/21
#define GET_RX_STATUS_DESC_C2H_VALID_9086X(__pRxStatusDesc)			LE_BITS_TO_4BYTE( __pRxStatusDesc+4,  7, 1)
#ifdef CONFIG_USB_RX_AGGREGATION
#define GET_RX_STATUS_DESC_USB_AGG_PKTNUM_9086X(__pRxStatusDesc)	LE_BITS_TO_4BYTE( __pRxStatusDesc+12, 16, 8)
#endif

#define GET_RX_STATUS_DESC_RXRATE_9086X(__pRxDesc)          LE_BITS_TO_4BYTE( __pRxDesc+16, 0, 6)

#define XMIT_OLD_TEST   0

#define WF_TX_DESC_PKT_SIZE_9086X(__pTxDesc, __Value) 	  				SET_BITS_TO_LE_4BYTE(__pTxDesc, 0, 16, __Value)

#define WF_TX_DESC_BMC_9086X(__pTxDesc, __Value) 	 	  				SET_BITS_TO_LE_4BYTE(__pTxDesc, 16, 1, __Value)

#define WF_TX_DESC_AGG_BREAK_9086X(__pTxDesc, __Value) 	  				SET_BITS_TO_LE_4BYTE(__pTxDesc, 17, 1, __Value)

#define WF_TX_DESC_SPE_RPT_9086X(__pTxDesc, __Value) 	  				SET_BITS_TO_LE_4BYTE(__pTxDesc, 18, 1, __Value)

#define WF_TX_DESC_DATARATE_FB_CFG_9086X(__pTxDesc, __Value)			SET_BITS_TO_LE_4BYTE(__pTxDesc, 25, 1, __Value)

#define WF_TX_DESC_RTSRATE_FB_CFG_9086X(__pTxDesc, __Value)		   		SET_BITS_TO_LE_4BYTE(__pTxDesc, 26, 1, __Value)

#define WF_TX_DESC_RTSRATE_CFG_9086X(__pTxDesc, __Value)		 		SET_BITS_TO_LE_4BYTE(__pTxDesc, 27, 1, __Value)

#define WF_TX_DESC_MINISPACE_CFG_9086X(__pTxDesc, __Value)		 		SET_BITS_TO_LE_4BYTE(__pTxDesc, 28, 1, __Value)

#define WF_TX_DESC_AGGNUM_CFG_9086X(__pTxDesc, __Value)					SET_BITS_TO_LE_4BYTE(__pTxDesc, 29, 1, __Value)

#define WF_TX_DESC_RSVD_9086X(__pTxDesc, __Value) 						SET_BITS_TO_LE_4BYTE(__pTxDesc, 30, 1, __Value)

#define WF_TX_DESC_AGG_ENABLE_9086X(__pTxDesc, __Value)   SET_BITS_TO_LE_4BYTE(__pTxDesc + 4, 0, 1, __Value)
#define WF_TX_DESC_SEC_TYPE_9086X(__pTxDesc, __Value)     SET_BITS_TO_LE_4BYTE(__pTxDesc + 4, 1, 3, __Value)

#define WF_TX_DESC_QUEUE_SEL_9086X(__pTxDesc, __Value)	  SET_BITS_TO_LE_4BYTE(__pTxDesc + 4, 8, 5, __Value)
#define WF_TX_DESC_RETRY_LIMIT_ENABLE_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc + 4, 14, 1, __Value)
#define WF_TX_DESC_USE_RATE_9086X(__pTxDesc, __Value)     SET_BITS_TO_LE_4BYTE(__pTxDesc + 4, 15, 1, __Value)

#define WF_TX_DESC_RATE_ID_9086X(__pTxDesc, __Value)      SET_BITS_TO_LE_4BYTE(__pTxDesc + 4, 16, 4, __Value)
#define WF_TX_DESC_RATE_ADP_9086X(__pTxDesc, __Value)     SET_BITS_TO_LE_4BYTE(__pTxDesc + 4, 20, 4, __Value)

#define WF_TX_DESC_MACID_9086X(__pTxDesc, __Value)        SET_BITS_TO_LE_4BYTE(__pTxDesc + 4, 24, 7, __Value)

#define WF_TX_DESC_SW_DEFINE_9086X(__pTxDesc, __Value)    SET_BITS_TO_LE_4BYTE(__pTxDesc + 8, 0, 12, __Value)
#define WF_TX_DESC_HWSEQ_EN_9086X(__pTxDesc, __Value)     SET_BITS_TO_LE_4BYTE(__pTxDesc + 8, 12, 1, __Value)
#define WF_TX_DESC_SEQ_9086X(__pTxDesc, __Value)    	  SET_BITS_TO_LE_4BYTE(__pTxDesc + 8, 13, 12, __Value)

#define WF_TX_DESC_DATA_RETRY_LIMIT_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc + 8, 25, 2, __Value)

#define WF_TX_DESC_MBSSID_9086X(__pTxDesc, __Value) 	  SET_BITS_TO_LE_4BYTE(__pTxDesc + 8, 27, 1, __Value)
#define WF_TX_DESC_CTS2SELF_9086X(__pTxDesc, __Value)      SET_BITS_TO_LE_4BYTE(__pTxDesc+ 8, 28, 1, __Value)
#define WF_TX_DESC_RTS_ENABLE_9086X(__pTxDesc, __Value)    SET_BITS_TO_LE_4BYTE(__pTxDesc+ 8, 29, 1, __Value)
#define WF_TX_DESC_HW_RTS_ENABLE_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+ 8, 30, 1, __Value)

#define WF_TX_DESC_USB_TXAGG_NUM_9086X(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 8, 8, __Value)
#define WF_TX_DESC_TX_RATE_9086X(__pTxDesc, __Value)		    SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 16, 7, __Value)
#define WF_TX_DESC_DATA_BW_9086X(__pTxDesc, __Value)			SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 24, 1, __Value)
#define WF_TX_DESC_DATA_SC_9086X(__pTxDesc, __Value) 	 	 	SET_BITS_TO_LE_4BYTE(__pTxDesc + 12, 25, 2, __Value)
#define WF_TX_DESC_DATA_SHORT_9086X(__pTxDesc, __Value)   		SET_BITS_TO_LE_4BYTE(__pTxDesc + 12, 27, 1, __Value)
#define WF_TX_DESC_RTS_SC_9086X(__pTxDesc, __Value)       		SET_BITS_TO_LE_4BYTE(__pTxDesc + 12, 28, 2, __Value)
#define WF_TX_DESC_RTS_SHORT_9086X(__pTxDesc, __Value) 	 	 	SET_BITS_TO_LE_4BYTE(__pTxDesc + 12, 30, 1, __Value)

#if 1

#define SET_TX_DESC_PKT_SIZE_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc, 0, 16, __Value)
#define SET_TX_DESC_OFFSET_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc, 16, 8, __Value)
#define SET_TX_DESC_LAST_SEG_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc, 26, 1, __Value)
#define SET_TX_DESC_FIRST_SEG_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc, 27, 1, __Value)
#define SET_TX_DESC_OWN_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc, 31, 1, __Value)

#define SET_TX_DESC_MACID_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 0, 7, __Value)
#define SET_TX_DESC_QUEUE_SEL_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 8, 5, __Value)
#define SET_TX_DESC_RATE_ID_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 16, 5, __Value)
#define SET_TX_DESC_SEC_TYPE_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 22, 2, __Value)
#define SET_TX_DESC_PKT_OFFSET_9086X(__pTxDesc, __Value)		SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 24, 5, __Value)

#define SET_TX_DESC_AGG_BREAK_9086X(__pTxDesc, __Value) 				SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 16, 1, __Value)
#define SET_TX_DESC_BT_INT_9086X(__pTxDesc, __Value)			SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 23, 1, __Value)

#define SET_TX_DESC_HWSEQ_SEL_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 6, 2, __Value)
#define SET_TX_DESC_USE_RATE_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 8, 1, __Value)
#define SET_TX_DESC_DISABLE_FB_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 10, 1, __Value)
#define SET_TX_DESC_NAV_USE_HDR_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 15, 1, __Value)

#define SET_TX_DESC_TX_RATE_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 0, 7, __Value)

#define SET_TX_DESC_DATA_SHORT_9086X(__pTxDesc, __Value)	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 4, 1, __Value)
#define SET_TX_DESC_DATA_BW_9086X(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 5, 2, __Value)

#define SET_TX_DESC_HWSEQ_EN_9086X(__pTxDesc, __Value)			SET_BITS_TO_LE_4BYTE(__pTxDesc+32, 15, 1, __Value)

#define SET_TX_DESC_SEQ_9086X(__pTxDesc, __Value)					SET_BITS_TO_LE_4BYTE(__pTxDesc+36, 12, 12, __Value)

#endif
#endif


void Func_Of_Update_Txdesc(PNIC Nic, struct xmit_frame *pxmitframe, u8 * pmem);

#if 0 
s32 s_tx_mgnt(struct xmit_frame *pmgntframe, PwADPTDATA pwadptdata,
						u8 tag);
#endif

#endif
