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

#ifndef _WL_HT_H_
#define _WL_HT_H_

struct ht_priv {
	u8 ht_option;
	u8 ampdu_enable;
	u8 tx_amsdu_enable;
	u8 bss_coexist;

	u32 tx_amsdu_maxlen;
	u32 rx_ampdu_maxlen;

	u8 rx_ampdu_min_spacing;

	u8 ch_offset;
	u8 sgi_20m;
	u8 sgi_40m;

	u8 agg_enable_bitmap;
	u8 candidate_tid_bitmap;

	u8 ldpc_cap;
	u8 stbc_cap;
	u8 smps_cap;

	struct wl_ieee80211_ht_cap ht_cap;

};
#define	LDPC_HT_ENABLE_RX			BIT0
#define	LDPC_HT_ENABLE_TX			BIT1
#define	LDPC_HT_CAP_TX				BIT3

#define	STBC_HT_ENABLE_RX			BIT0
#define	STBC_HT_ENABLE_TX			BIT1
#define	STBC_HT_CAP_TX				BIT3

#define SET_EXT_CAPABILITY_ELE_BSS_COEXIST(_pEleStart, _val)	SET_BITS_TO_LE_1BYTE(((u8 *)(_pEleStart)), 0, 1, _val)

#define GET_HT_CAP_ELE_LDPC_CAP(_pEleStart)				LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)), 0, 1)
#define GET_HT_CAP_ELE_CHL_WIDTH(_pEleStart)			LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)), 1, 1)
#define GET_HT_CAP_ELE_TX_STBC(_pEleStart)				LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)), 7, 1)
#define GET_HT_CAP_ELE_RX_STBC(_pEleStart)				LE_BITS_TO_1BYTE(((u8 *)(_pEleStart))+1, 0, 2)

#define SET_HT_CAP_ELE_CHL_WIDTH(_pEleStart, _val)			SET_BITS_TO_LE_1BYTE(((u8 *)(_pEleStart)), 1, 1, _val)
#define SET_HT_CAP_ELE_SHORT_GI20M(_pEleStart, _val)		SET_BITS_TO_LE_1BYTE(((u8 *)(_pEleStart)), 5, 1, _val)
#define SET_HT_CAP_ELE_SHORT_GI40M(_pEleStart, _val)		SET_BITS_TO_LE_1BYTE(((u8 *)(_pEleStart)), 6, 1, _val)
#define SET_HT_CAP_ELE_RX_STBC(_pEleStart, _val)			SET_BITS_TO_LE_1BYTE(((u8 *)(_pEleStart)) + 1, 0, 2, _val)

#define HT_CAP_ELE_SUP_MCS_SET(_pEleStart)				(((u8 *)(_pEleStart))+3)
#define HT_CAP_ELE_RX_MCS_MAP(_pEleStart)				HT_CAP_ELE_SUP_MCS_SET(_pEleStart)

#define GET_HT_OP_ELE_PRI_CHL(_pEleStart)					LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)), 0, 8)
#define SET_HT_OP_ELE_PRI_CHL(_pEleStart, _val)				SET_BITS_TO_LE_1BYTE(((u8 *)(_pEleStart)), 0, 8, _val)

#define GET_HT_OP_ELE_2ND_CHL_OFFSET(_pEleStart)			LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)) + 1, 0, 2)
#define GET_HT_OP_ELE_STA_CHL_WIDTH(_pEleStart)				LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)) + 1, 2, 1)
#define SET_HT_OP_ELE_2ND_CHL_OFFSET(_pEleStart, _val)		SET_BITS_TO_LE_1BYTE(((u8 *)(_pEleStart)) + 1, 0, 2, _val)
#define SET_HT_OP_ELE_STA_CHL_WIDTH(_pEleStart, _val)		SET_BITS_TO_LE_1BYTE(((u8 *)(_pEleStart)) + 1, 2, 1, _val)
#endif
