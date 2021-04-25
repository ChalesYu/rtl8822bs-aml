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

#ifndef __HAL_COM_PHYCFG_H__
#define __HAL_COM_PHYCFG_H__

typedef enum _RF_TX_NUM {
	RF_1TX = 0,
	RF_MAX_TX_NUM,
} RF_TX_NUM;

#define MAX_POWER_INDEX 		0x3F

typedef enum _REGULATION_TXPWR_LMT {
	TXPWR_LMT_FCC = 0,
	TXPWR_LMT_MKK = 1,
	TXPWR_LMT_ETSI = 2,
	TXPWR_LMT_WW = 3,

	TXPWR_LMT_MAX_REGULATION_NUM = 4
} REGULATION_TXPWR_LMT;

typedef struct _BB_REGISTER_DEFINITION {
	u32 rfintfs;

	u32 rfintfo;

	u32 rfintfe;

	u32 rf3wireOffset;

	u32 rfHSSIPara2;

	u32 rfLSSIReadBack;

	u32 rfLSSIReadBackPi;

} BB_REGISTER_DEFINITION_T, *PBB_REGISTER_DEFINITION_T;


VOID
Func_Chip_Bb_Settxpowerindexbyratesection(IN PNIC Nic,
								 IN u8 RFPath,
								 IN u8 Channel, IN u8 RateSection);


s8
Func_Chip_Bb_Pre_Gettxpowerbyrate(IN PNIC Nic,
					 IN u8 Band, IN u8 RFPath, IN u8 TxNum, IN u8 RateIndex);

VOID
Func_Chip_Bb_Settxpowerbyrate(IN PNIC Nic,
					 IN u8 Band,
					 IN u8 RFPath, IN u8 TxNum, IN u8 Rate, IN s8 Value);

VOID Func_Chip_Bb_Settxpowerlevelbypath(IN PNIC Nic, IN u8 channel, IN u8 path);

VOID
Func_Chip_Bb_Settxpowerindexbyratearray(IN PNIC Nic,
							   IN u8 RFPath,
							   IN CHANNEL_WIDTH BandWidth,
							   IN u8 Channel,
							   IN u8 * Rates, IN u8 RateArraySize);

VOID Func_Chip_Bb_Inittxpowerbyrate(IN PNIC Nic);


u8
Func_Chip_Bb_Gettxpowerindexbase(IN PNIC Nic,
						IN u8 RFPath,
						IN u8 Rate,
						IN CHANNEL_WIDTH BandWidth,
						IN u8 Channel, OUT PBOOLEAN bIn24G);

s8
Func_Chip_Bb_Gettxpowerlimit(IN PNIC Nic,
					IN u32 RegPwrTblSel,
					IN BAND_TYPE Band,
					IN CHANNEL_WIDTH Bandwidth,
					IN u8 RfPath, IN u8 DataRate, IN u8 Channel);


VOID Func_Chip_Bb_Inittxpowerlimit(IN PNIC Nic);

s8 Func_Chip_Bb_Gettxpowertrackingoffset(PNIC Nic, u8 Rate, u8 RFPath);

u8
Func_Chip_Bb_Gettxpowerindex(IN PNIC Nic,
					IN u8 RFPath,
					IN u8 Rate, IN CHANNEL_WIDTH BandWidth, IN u8 Channel);

VOID
Func_Chip_Bb_Settxpowerindex(IN PNIC Nic,
					IN u32 PowerIndex, IN u8 RFPath, IN u8 Rate);

bool Func_Chip_Bb_Is_Tx_Power_Limit_Needed(PNIC Nic);
bool Func_Chip_Bb_Is_Tx_Power_By_Rate_Needed(PNIC Nic);
int Func_Chip_Bb_Load_Tx_Power_By_Rate(PNIC Nic, u8 chk_file);
int Func_Chip_Bb_Load_Tx_Power_Limit(PNIC Nic, u8 chk_file);
void Func_Chip_Bb_Load_Tx_Power_Ext_Info(PNIC Nic, u8 chk_file);

#endif
