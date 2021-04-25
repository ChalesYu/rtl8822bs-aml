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

#ifndef __WL_EFUSE_H__
#define __WL_EFUSE_H__

#define	EFUSE_ERROE_HANDLE		1

#define	PG_STATE_HEADER 		0x01
#define	PG_STATE_WORD_0		0x02
#define	PG_STATE_WORD_1		0x04
#define	PG_STATE_WORD_2		0x08
#define	PG_STATE_WORD_3		0x10
#define	PG_STATE_DATA			0x20

#define	PG_SWBYTE_H			0x01
#define	PG_SWBYTE_L			0x02

#define	PGPKT_DATA_SIZE		8

#define	EFUSE_WIFI				0
#define	EFUSE_BT				1

enum _EFUSE_DEF_TYPE {
	TYPE_EFUSE_MAX_SECTION = 0,
	TYPE_EFUSE_REAL_CONTENT_LEN = 1,
	TYPE_AVAILABLE_EFUSE_BYTES_BANK = 2,
	TYPE_AVAILABLE_EFUSE_BYTES_TOTAL = 3,
	TYPE_EFUSE_MAP_LEN = 4,
	TYPE_EFUSE_PROTECT_BYTES_BANK = 5,
	TYPE_EFUSE_CONTENT_LEN_BANK = 6,
};

//tangjian define hw_eeprom.h
//#define EEPROM_MAX_SIZE      512

#define		EFUSE_MAX_MAP_LEN		512

#define		EFUSE_MAX_HW_SIZE		512
#define		EFUSE_MAX_SECTION_BASE	16

#define EXT_HEADER(header) ((header & 0x1F ) == 0x0F)
#define ALL_WORDS_DISABLED(wde)	((wde & 0x0F) == 0x0F)
#define GET_HDR_OFFSET_2_0(header) ( (header & 0xE0) >> 5)

#define		EFUSE_MAX_BANK			4
#define		EFUSE_MAX_BT_BANK		(EFUSE_MAX_BANK-1)

typedef enum {
	ERR_SUCCESS = 0,
	ERR_DRIVER_FAILURE,
	ERR_IO_FAILURE,
	ERR_WI_TIMEOUT,
	ERR_WI_BUSY,
	ERR_BAD_FORMAT,
	ERR_INVALID_DATA,
	ERR_NOT_ENOUGH_SPACE,
	ERR_WRITE_PROTECT,
	ERR_READ_BACK_FAIL,
	ERR_OUT_OF_RANGE
} ERROR_CODE;

typedef struct _EFUSE_HAL {
	u8 fakeEfuseBank;
	u32 fakeEfuseUsedBytes;
	u8 fakeEfuseContent[EFUSE_MAX_HW_SIZE];
	u8 fakeEfuseInitMap[EFUSE_MAX_MAP_LEN];
	u8 fakeEfuseModifiedMap[EFUSE_MAX_MAP_LEN];
	u32 EfuseUsedBytes;
	u8 EfuseUsedPercentage;

	ERROR_CODE Status;

} EFUSE_HAL, *PEFUSE_HAL;

u8 EFUSE_Read1Byte(PNIC Nic, u16 Address);

#define MAC_HIDDEN_MAX_BW_NUM 8
extern const u8 _mac_hidden_max_bw_to_hal_bw_cap[];
#define mac_hidden_max_bw_to_hal_bw_cap(max_bw) (((max_bw) >= MAC_HIDDEN_MAX_BW_NUM) ? 0 : _mac_hidden_max_bw_to_hal_bw_cap[(max_bw)])

#define MAC_HIDDEN_PROTOCOL_NUM 4
extern const u8 _mac_hidden_proto_to_hal_proto_cap[];
#define mac_hidden_proto_to_hal_proto_cap(proto) (((proto) >= MAC_HIDDEN_PROTOCOL_NUM) ? 0 : _mac_hidden_proto_to_hal_proto_cap[(proto)])

u8 mac_hidden_wl_func_to_hal_wl_func(u8 func);

#endif
