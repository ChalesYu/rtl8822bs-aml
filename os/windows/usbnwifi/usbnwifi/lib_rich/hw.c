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
#include "globals.h"
#include "ndis.h"
#include "hw_pcomp.h"
#include "hw_main.h"
#include "hw_Nic.h"
#include "hw_func.h"
#include "usb_main.h"
#include "bus_intf.h"
#include "8187_gen.h"

#define is_primary_wadptdata(wadptdata) (1)

extern
u32 Func_Of_Hw_Deinit(PNIC Nic);
extern
u32 Func_Of_Hw_Init(PNIC Nic);
typedef enum _EXTEND_wMBOX0_EVT {
	EXTEND_wMBOX0_DBG_PRINT = 0
} EXTEND_wMBOX0_EVT;

#define	GET_wMBOX0_MAC_HIDDEN_RPT_UUID_X(_data)			LE_BITS_TO_1BYTE(((u8 *)(_data)) + 0, 0, 8)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_UUID_Y(_data)			LE_BITS_TO_1BYTE(((u8 *)(_data)) + 1, 0, 8)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_UUID_Z(_data)			LE_BITS_TO_1BYTE(((u8 *)(_data)) + 2, 0, 5)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_UUID_CRC(_data)			LE_BITS_TO_2BYTE(((u8 *)(_data)) + 2, 5, 11)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_HCI_TYPE(_data)			LE_BITS_TO_1BYTE(((u8 *)(_data)) + 4, 0, 4)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_PACKAGE_TYPE(_data)		LE_BITS_TO_1BYTE(((u8 *)(_data)) + 4, 4, 4)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_WL_FUNC(_data)			LE_BITS_TO_1BYTE(((u8 *)(_data)) + 5, 0, 4)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_HW_STYPE(_data)			LE_BITS_TO_1BYTE(((u8 *)(_data)) + 5, 4, 4)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_BW(_data)				LE_BITS_TO_1BYTE(((u8 *)(_data)) + 6, 0, 3)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_ANT_NUM(_data)			LE_BITS_TO_1BYTE(((u8 *)(_data)) + 6, 5, 3)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_80211_PROTOCOL(_data)	LE_BITS_TO_1BYTE(((u8 *)(_data)) + 7, 2, 2)
#define	GET_wMBOX0_MAC_HIDDEN_RPT_NIC_ROUTER(_data)		LE_BITS_TO_1BYTE(((u8 *)(_data)) + 7, 6, 2)

#ifndef DBG_wMBOX0_MAC_HIDDEN_RPT_HANDLE
#define DBG_wMBOX0_MAC_HIDDEN_RPT_HANDLE 0
#endif

#define MAC_HIDDEN_RPT_LEN 8

static VOID Func_Config_Chip_Outep_Process(IN PNIC Nic, IN u8 NumOutPipe)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	pHalData->OutEpQueueSel = 0;
	pHalData->OutEpNumber = 0;

	switch (NumOutPipe) {
	case 4:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_LQ | TX_SELE_NQ;
		pHalData->OutEpNumber = 4;
		break;
	case 3:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_LQ | TX_SELE_NQ;
		pHalData->OutEpNumber = 3;
		break;
	case 2:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_NQ;
		pHalData->OutEpNumber = 2;
		break;
	case 1:
		pHalData->OutEpQueueSel = TX_SELE_HQ;
		pHalData->OutEpNumber = 1;
		break;
	default:
		break;

	}
	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s OutEpQueueSel(0x%02x), OutEpNumber(%d)\n", __FUNCTION__,
		pHalData->OutEpQueueSel, pHalData->OutEpNumber));
}

static BOOLEAN Func_Chip_Hw_Usb_Set_Queue_Pipe_Mapping_Usb(IN PNIC Nic,
												 IN u8 NumInPipe,
												 IN u8 NumOutPipe)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	BOOLEAN result = FALSE;

	Func_Config_Chip_Outep_Process(Nic, NumOutPipe);

	if (1 == pHalData->OutEpNumber) {
		if (1 != NumInPipe)
			return result;
	}

	result = Func_Chip_Hw_Mappingoutpipe(Nic, NumOutPipe);

	return result;

}

void Func_Of_Proc_Chip_Hw_Chip_Configure(PNIC Nic)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	//struct dvobj_priv *pdvobjpriv = wadptdata_to_dvobj(Nic);

#if 0
	if (IS_HIGH_SPEED_USB(Nic)) {
		pHalData->UsbBulkOutSize = USB_HIGH_SPEED_BULK_SIZE;
	} else {
		pHalData->UsbBulkOutSize = USB_FULL_SPEED_BULK_SIZE;
	}

	pHalData->interfaceIndex = pdvobjpriv->InterfaceNumber;
#else
    //need to change
	pHalData->UsbBulkOutSize = USB_HIGH_SPEED_BULK_SIZE;
#endif

#ifdef CONFIG_USB_TX_AGGREGATION
	pHalData->UsbTxAggMode = 1;
	pHalData->UsbTxAggDescNum = 0x6;
#endif

#ifdef CONFIG_USB_RX_AGGREGATION
	pHalData->UsbRxAggMode = USB_RX_AGG_USB;
#ifdef CONFIG_PLATFORM_IPC
	pHalData->UsbRxAggBlockCount = 0x3;
	pHalData->UsbRxAggBlockTimeout = 0x8;
	pHalData->UsbRxAggPageCount = 0xc;
	pHalData->UsbRxAggPageTimeout = 0x8;
#else
	pHalData->UsbRxAggBlockCount = 0x5;
	pHalData->UsbRxAggBlockTimeout = 0x20;
	pHalData->UsbRxAggPageCount = 0xF;
	pHalData->UsbRxAggPageTimeout = 0x20;
#endif
#endif

#if 0 //need to check
	Func_Chip_Hw_Usb_Set_Queue_Pipe_Mapping_Usb(Nic,
									  pdvobjpriv->RtNumInPipes,
									  pdvobjpriv->RtNumOutPipes);
#endif
}

static void Func_Chip_Hw_Efuseparseidcode(IN PNIC Nic, IN u8 * hwinfo)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u16 EEPROMId;

	EEPROMId = le16_to_cpu(*((u16 *) hwinfo));
	if (EEPROMId != WLT_EEPROM_ID) {
		MpTrace(COMP_RICHD, DBG_NORMAL, ("EEPROM ID(%#x) is invalid!!\n", EEPROMId));
		pHalData->bautoload_fail_flag = TRUE;
	} else
		pHalData->bautoload_fail_flag = FALSE;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("EEPROM ID=0x%04x\n", EEPROMId));
}

static VOID
Func_Chip_Hw_Efuseparsepidvid_Process(IN PNIC Nic,
							IN u8 * hwinfo, IN BOOLEAN AutoLoadFail)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	if (AutoLoadFail) {
		pHalData->EEPROMVID = 0;
		pHalData->EEPROMPID = 0;
	} else {
		pHalData->EEPROMVID = le16_to_cpu(*(u16 *) & hwinfo[EEPROM_VID_9086XU]);
		pHalData->EEPROMPID = le16_to_cpu(*(u16 *) & hwinfo[EEPROM_PID_9086XU]);

	}
	MpTrace(COMP_RICHD, DBG_NORMAL, ("EEPROM VID = 0x%4x\n", pHalData->EEPROMVID));
	MpTrace(COMP_RICHD, DBG_NORMAL, ("EEPROM PID = 0x%4x\n", pHalData->EEPROMPID));
}

int Func_Chip_Hw_Config_Macaddr(PNIC Nic, bool autoload_fail)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
	u8 addr[ETH_ALEN];
	int addr_offset = Func_Chip_Hw_Efuse_Macaddr_Offset(Nic);
	u8 *hw_addr = NULL;
	int ret = TRUE;

	if (autoload_fail)
		goto bypass_hw_pg;

	if (addr_offset != -1){
		hw_addr = &hal_data->efuse_eeprom_data[addr_offset];
		/* We have to report MAC Address to Nic permanent address in first initialization
		*	or else this infomation will be lost when reset.
		*  This value will be sent to the adapter permanent address later.
		*/
		Func_Of_Proc_Pre_Memcpy(Nic->CardInfo.PermanentAddress, hw_addr, ETH_ALEN);
	}

	if (!hw_addr) {
		if (Func_Chip_Hw_Getphyefusemacaddr(Nic, addr) == TRUE) ///tangjian
			hw_addr = addr;
	}

	if (hw_addr && invalid_mac_address_to_check_func(hw_addr, TRUE, 1) == FALSE) {
		Func_Of_Proc_Pre_Memcpy(hal_data->EEPROMMACAddr, hw_addr, ETH_ALEN);
		goto exit;
	}

bypass_hw_pg:

	Func_Of_Proc_Pre_Memset(hal_data->EEPROMMACAddr, 0, ETH_ALEN);
	ret = FALSE;

exit:
	return ret;
}

static void
Func_Chip_Hw_Readpowervaluefromprom_Process(IN PNIC Nic,
								 IN PTxPowerInfo24G pwrInfo24G,
								 IN u8 * PROMContent, IN BOOLEAN AutoLoadFail)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u4Byte eeAddr = EEPROM_TX_PWR_INX_9086X, group, TxCount = 0;

	Func_Of_Proc_Pre_Memset(pwrInfo24G, 0, sizeof(TxPowerInfo24G));

	if (0xFF == PROMContent[eeAddr + 1])
		AutoLoadFail = TRUE;

	if (AutoLoadFail) {
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s(): Use Default value!\n", __func__));
		for (group = 0; group < MAX_CHNL_GROUP_24G; group++) {
			pwrInfo24G->IndexCCK_Base[0][group] = EEPROM_DEFAULT_24G_CCK_INDEX;
			pwrInfo24G->IndexBW40_Base[0][group] = EEPROM_DEFAULT_24G_OFDM_INDEX;
		}
		for (TxCount = 0; TxCount < MAX_TX_COUNT; TxCount++) {
			if (TxCount == 0) {
				pwrInfo24G->BW20_Diff[0][0] = EEPROM_DEFAULT_24G_HT20_DIFF;
				pwrInfo24G->OFDM_Diff[0][0] = EEPROM_DEFAULT_24G_OFDM_DIFF;
			} else {
				pwrInfo24G->BW20_Diff[0][TxCount] = EEPROM_DEFAULT_DIFF;
				pwrInfo24G->BW40_Diff[0][TxCount] = EEPROM_DEFAULT_DIFF;
				pwrInfo24G->CCK_Diff[0][TxCount] = EEPROM_DEFAULT_DIFF;
				pwrInfo24G->OFDM_Diff[0][TxCount] = EEPROM_DEFAULT_DIFF;
			}
		}

		return;
	}

	pHalData->bTXPowerDataReadFromEEPORM = TRUE;

	for (group = 0; group < MAX_CHNL_GROUP_24G; group++) {
		pwrInfo24G->IndexCCK_Base[0][group] = PROMContent[eeAddr++];
		if (pwrInfo24G->IndexCCK_Base[0][group] == 0xFF)
			pwrInfo24G->IndexCCK_Base[0][group] = EEPROM_DEFAULT_24G_CCK_INDEX;
	}
	for (group = 0; group < MAX_CHNL_GROUP_24G - 1; group++) {
		pwrInfo24G->IndexBW40_Base[0][group] = PROMContent[eeAddr++];
		if (pwrInfo24G->IndexBW40_Base[0][group] == 0xFF)
			pwrInfo24G->IndexBW40_Base[0][group] = EEPROM_DEFAULT_24G_OFDM_INDEX;
	}
	for (TxCount = 0; TxCount < MAX_TX_COUNT; TxCount++) {
		if (TxCount == 0) {
			pwrInfo24G->BW40_Diff[0][TxCount] = 0;
			pwrInfo24G->BW20_Diff[0][TxCount] =
				(PROMContent[eeAddr] & 0xf0) >> 4;
			if (pwrInfo24G->BW20_Diff[0][TxCount] & BIT3)
				pwrInfo24G->BW20_Diff[0][TxCount] |= 0xF0;

			pwrInfo24G->OFDM_Diff[0][TxCount] = (PROMContent[eeAddr] & 0x0f);
			if (pwrInfo24G->OFDM_Diff[0][TxCount] & BIT3)
				pwrInfo24G->OFDM_Diff[0][TxCount] |= 0xF0;

			pwrInfo24G->CCK_Diff[0][TxCount] = 0;
			eeAddr++;
		} else {
			pwrInfo24G->BW40_Diff[0][TxCount] =
				(PROMContent[eeAddr] & 0xf0) >> 4;
			if (pwrInfo24G->BW40_Diff[0][TxCount] & BIT3)
				pwrInfo24G->BW40_Diff[0][TxCount] |= 0xF0;

			pwrInfo24G->BW20_Diff[0][TxCount] = (PROMContent[eeAddr] & 0x0f);
			if (pwrInfo24G->BW20_Diff[0][TxCount] & BIT3)
				pwrInfo24G->BW20_Diff[0][TxCount] |= 0xF0;

			eeAddr++;

			pwrInfo24G->OFDM_Diff[0][TxCount] =
				(PROMContent[eeAddr] & 0xf0) >> 4;
			if (pwrInfo24G->OFDM_Diff[0][TxCount] & BIT3)
				pwrInfo24G->OFDM_Diff[0][TxCount] |= 0xF0;

			pwrInfo24G->CCK_Diff[0][TxCount] = (PROMContent[eeAddr] & 0x0f);
			if (pwrInfo24G->CCK_Diff[0][TxCount] & BIT3)
				pwrInfo24G->CCK_Diff[0][TxCount] |= 0xF0;

			eeAddr++;
		}
	}

	eeAddr += (14 + 10);
}

static BOOLEAN Func_Chip_Hw_Get_Chnl_Group_Process(IN u8 Channel, OUT u8 * pGroup)
{
	BOOLEAN bIn24G = TRUE;

	if (Channel <= 14) {
		bIn24G = TRUE;

		if (1 <= Channel && Channel <= 2)
			*pGroup = 0;
		else if (3 <= Channel && Channel <= 5)
			*pGroup = 1;
		else if (6 <= Channel && Channel <= 8)
			*pGroup = 2;
		else if (9 <= Channel && Channel <= 11)
			*pGroup = 3;
		else if (12 <= Channel && Channel <= 14)
			*pGroup = 4;
		else
			MpTrace(COMP_RICHD, DBG_NORMAL, ("==>%s in 2.4 G, but Channel %d in Group not found\n",__func__,
				 Channel));
	}

	return bIn24G;
}

static void
Func_Chip_Hw_Efuseparsetxpowerinfo_Process(IN PNIC Nic,
								IN u8 * PROMContent, IN BOOLEAN AutoLoadFail)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	TxPowerInfo24G pwrInfo24G;
	u8 rfPath, ch, group, TxCount = 1;

	Func_Chip_Hw_Readpowervaluefromprom_Process(Nic, &pwrInfo24G, PROMContent,
									 AutoLoadFail);
	for (ch = 0; ch < CENTER_CH_2G_NUM; ch++) {
		Func_Chip_Hw_Get_Chnl_Group_Process(ch + 1, &group);

		if (ch == 14 - 1) {
			pHalData->Index24G_CCK_Base[0][ch] = pwrInfo24G.IndexCCK_Base[0][5];
			pHalData->Index24G_BW40_Base[0][ch] =
				pwrInfo24G.IndexBW40_Base[0][group];
		} else {
			pHalData->Index24G_CCK_Base[0][ch] =
				pwrInfo24G.IndexCCK_Base[0][group];
			pHalData->Index24G_BW40_Base[0][ch] =
				pwrInfo24G.IndexBW40_Base[0][group];
		}
		MpTrace(COMP_RICHD, DBG_NORMAL, ("======= ChannelIndex %d, Group %d=======\n", ch, group));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Index24G_CCK_Base[0][%d] = 0x%x\n", ch,
				   pHalData->Index24G_CCK_Base[0][ch]));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("Index24G_BW40_Base[0][%d] = 0x%x\n", ch,
				   pHalData->Index24G_BW40_Base[0][ch]));
	}

	for (TxCount = 0; TxCount < MAX_TX_COUNT; TxCount++) {
		pHalData->CCK_24G_Diff[0][TxCount] = pwrInfo24G.CCK_Diff[0][TxCount];
		pHalData->OFDM_24G_Diff[0][TxCount] = pwrInfo24G.OFDM_Diff[0][TxCount];
		pHalData->BW20_24G_Diff[0][TxCount] = pwrInfo24G.BW20_Diff[0][TxCount];
		pHalData->BW40_24G_Diff[0][TxCount] = pwrInfo24G.BW40_Diff[0][TxCount];

		MpTrace(COMP_RICHD, DBG_NORMAL, ("--------------------------------------- 2.4G ---------------------------------------\n"));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("CCK_24G_Diff[][%d]= %d\n", TxCount,
				   pHalData->CCK_24G_Diff[0][TxCount]));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("OFDM_24G_Diff[0][%d]= %d\n", TxCount,
				   pHalData->OFDM_24G_Diff[0][TxCount]));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("BW20_24G_Diff[0][%d]= %d\n", TxCount,
				   pHalData->BW20_24G_Diff[0][TxCount]));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("BW40_24G_Diff[0][%d]= %d\n", TxCount,
				   pHalData->BW40_24G_Diff[0][TxCount]));
	}

	if (!AutoLoadFail) {
		pHalData->EEPROMRegulatory =
			(PROMContent[EEPROM_RF_BOARD_OPTION_9086X] & 0x7);
		if (PROMContent[EEPROM_RF_BOARD_OPTION_9086X] == 0xFF)
			pHalData->EEPROMRegulatory = (EEPROM_DEFAULT_BOARD_OPTION & 0x7);
	} else
		pHalData->EEPROMRegulatory = 0;
	MpTrace(COMP_RICHD, DBG_NORMAL, ("EEPROMRegulatory = 0x%x\n", pHalData->EEPROMRegulatory));

}

static VOID
Func_Chip_Hw_Efuseparsechnlplan_Process(IN PNIC Nic,
							 IN u8 * hwinfo, IN BOOLEAN AutoLoadFail)
{
	Nic->mlmepriv.ChannelPlan =
		Func_Chip_Hw_Com_Config_Channel_Plan(Nic,
									hwinfo ? &hwinfo[EEPROM_COUNTRY_CODE_9086X]
									: NULL,
									hwinfo ? hwinfo[EEPROM_ChannelPlan_9086X] :
									0xFF, Nic->registrypriv.alpha2,
									Nic->registrypriv.channel_plan,
									WL_CHPLAN_WORLD_NULL, AutoLoadFail);
}

static void
Func_Chip_Hw_Efuseparsethermalmeter_Process(PNIC Nic,
								 u8 * PROMContent, u8 AutoLoadFail)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

	if (FALSE == AutoLoadFail)
		pHalData->EEPROMThermalMeter = PROMContent[EEPROM_THERMAL_METER_9086X];
	else
		pHalData->EEPROMThermalMeter = EEPROM_Default_ThermalMeter_9086X;

	if ((pHalData->EEPROMThermalMeter == 0xff) || (TRUE == AutoLoadFail)) {
		u1Byte bAPKThermalMeterIgnore;
		u32 tmp;
		bAPKThermalMeterIgnore = TRUE;
		tmp = (u32) bAPKThermalMeterIgnore;
		if (!Func_Mcu_Universal_Func_Interface
			(Nic, UMSG_OPS_HAL_MSG_SET_APK_THERMAL_METER_IGNORE, &tmp, NULL,
			 1, 0)) {
	        MpTrace(COMP_RICHD, DBG_NORMAL, ("==>%s==>Func_Mcu_Universal_Func_Interface fail\n", __func__));			 
			return;
		}

		pHalData->EEPROMThermalMeter = EEPROM_Default_ThermalMeter_9086X;
	}
	MpTrace(COMP_RICHD, DBG_NORMAL, ("EEPROM ThermalMeter=0x%x\n", pHalData->EEPROMThermalMeter));
}

static VOID
Func_Chip_Hw_Efuseparsepowersavingmode_Process(IN PNIC Nic,
									IN u8 * hwinfo, IN BOOLEAN AutoLoadFail)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	struct pwrctrl_priv *pwrctl = &Nic->pwrctl_priv;
	u8 tmpvalue;

	if (AutoLoadFail) {
		pwrctl->bHWPowerdown = FALSE;
		pwrctl->bSupportRemoteWakeup = FALSE;
	} else {

		if (Nic->registrypriv.hwpdn_mode == 2)
			pwrctl->bHWPowerdown = (hwinfo[EEPROM_FEATURE_OPTION_9086X] & BIT4);
		else
			pwrctl->bHWPowerdown = Nic->registrypriv.hwpdn_mode;

		pwrctl->bSupportRemoteWakeup =
			(hwinfo[EEPROM_USB_OPTIONAL_FUNCTION0_9086XU] & BIT1) ? TRUE :
			FALSE;

		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s...bHWPwrPindetect(%x)-bHWPowerdown(%x) ,bSupportRemoteWakeup(%x)\n",
			 __FUNCTION__, pwrctl->bHWPwrPindetect, pwrctl->bHWPowerdown,
			 pwrctl->bSupportRemoteWakeup));
		
		MpTrace(COMP_RICHD, DBG_NORMAL, ("### PS params=>  power_mgnt(%x),usbss_enable(%x) ###\n",
				Nic->registrypriv.power_mgnt,
				Nic->registrypriv.usbss_enable));
	}
}

static VOID
Func_Chip_Hw_Efuseparseeepromver_Process(IN PNIC Nic,
							  IN u8 * hwinfo, IN BOOLEAN AutoLoadFail)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	if (!AutoLoadFail)
		pHalData->EEPROMVersion = hwinfo[EEPROM_VERSION_9086X];
	else
		pHalData->EEPROMVersion = 1;

}

static VOID
Func_Chip_Hw_Efuseparsecustomerid_Process(IN PNIC Nic,
							   IN u8 * hwinfo, IN BOOLEAN AutoLoadFail)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	if (!AutoLoadFail)
		pHalData->EEPROMCustomerID = hwinfo[EEPROM_CustomID_9086X];
	else
		pHalData->EEPROMCustomerID = 0;
	
	MpTrace(COMP_RICHD, DBG_NORMAL, ("EEPROM Customer ID: 0x%2x\n", pHalData->EEPROMCustomerID));
}

static VOID
Func_Chip_Hw_Efuseparsextal_Process(IN PNIC Nic,
						 IN u8 * hwinfo, IN BOOLEAN AutoLoadFail)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	if (!AutoLoadFail) {
		pHalData->CrystalCap = hwinfo[EEPROM_XTAL_9086X];
		if (pHalData->CrystalCap == 0xFF)
			pHalData->CrystalCap = EEPROM_Default_CrystalCap_9086X;
	} else
		pHalData->CrystalCap = EEPROM_Default_CrystalCap_9086X;
	
	MpTrace(COMP_RICHD, DBG_NORMAL, ("EEPROM CrystalCap: 0x%2x\n", pHalData->CrystalCap));
}

static void Func_Chip_Hw_Efuseparsekfreedata_Process(IN PNIC Nic,
								   IN u8 * PROMContent, IN BOOLEAN AutoloadFail)
{
#ifdef CONFIG_RF_POWER_TRIM
#define THERMAL_K_MEAN_OFFSET_9086X 5

	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	struct kfree_data_t *kfree_data = &pHalData->kfree_data;
	u8 pg_pwrtrim = 0xFF, pg_therm = 0xFF;

	pg_pwrtrim = EFUSE_Read1Byte(Nic, PPG_BB_GAIN_2G_TXA_OFFSET_9086X);
	pg_therm = EFUSE_Read1Byte(Nic, PPG_THERMAL_OFFSET_9086X);

	if (pg_pwrtrim != 0xFF) {
		kfree_data->bb_gain[0][0]
			= KFREE_BB_GAIN_2G_TX_OFFSET(pg_pwrtrim &
										 PPG_BB_GAIN_2G_TX_OFFSET_MASK);
		kfree_data->flag |= KFREE_FLAG_ON;
	}

	if (pg_therm != 0xFF) {
		kfree_data->thermal
			=
			KFREE_THERMAL_OFFSET(pg_therm & PPG_THERMAL_OFFSET_MASK) -
			THERMAL_K_MEAN_OFFSET_9086X;
		if (GET_PG_KFREE_THERMAL_K_ON_9086X(PROMContent))
			kfree_data->flag |= KFREE_FLAG_THERMAL_K_ON;
	}

	if (kfree_data->flag & KFREE_FLAG_THERMAL_K_ON)
		pHalData->EEPROMThermalMeter -= kfree_data->thermal;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("kfree Pwr Trim flag:%u\n", kfree_data->flag));
	if (kfree_data->flag & KFREE_FLAG_ON)
		MpTrace(COMP_RICHD, DBG_NORMAL, ("bb_gain:%d\n", kfree_data->bb_gain[0][0]));	
	if (kfree_data->flag & KFREE_FLAG_THERMAL_K_ON)
		MpTrace(COMP_RICHD, DBG_NORMAL, ("thermal:%d\n", kfree_data->thermal));

#endif
}

static int Func_Dev_To_Host_Mac_Hidden_Rpt_Hdl(PNIC Nic, u8 * data, u8 len)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(Nic);
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(Nic);
	int ret = FALSE;

	u32 uuid;
	u8 uuid_x;
	u8 uuid_y;
	u8 uuid_z;
	u16 uuid_crc;

	u8 hci_type;
	u8 package_type;
	u8 wl_func;
	u8 hw_stype;
	u8 bw;
	u8 ant_num;
	u8 protocol;
	u8 nic;

	int i;

	if (len < MAC_HIDDEN_RPT_LEN) {
		MpTrace(COMP_RICHD, DBG_NORMAL, ( "%s len(%u) < %d\n", __func__, len,
				   MAC_HIDDEN_RPT_LEN));
		goto exit;
	}

	uuid_x = GET_wMBOX0_MAC_HIDDEN_RPT_UUID_X(data);
	uuid_y = GET_wMBOX0_MAC_HIDDEN_RPT_UUID_Y(data);
	uuid_z = GET_wMBOX0_MAC_HIDDEN_RPT_UUID_Z(data);
	uuid_crc = GET_wMBOX0_MAC_HIDDEN_RPT_UUID_CRC(data);

	hci_type = GET_wMBOX0_MAC_HIDDEN_RPT_HCI_TYPE(data);
	package_type = GET_wMBOX0_MAC_HIDDEN_RPT_PACKAGE_TYPE(data);

	wl_func = GET_wMBOX0_MAC_HIDDEN_RPT_WL_FUNC(data);
	hw_stype = GET_wMBOX0_MAC_HIDDEN_RPT_HW_STYPE(data);

	bw = GET_wMBOX0_MAC_HIDDEN_RPT_BW(data);
	ant_num = GET_wMBOX0_MAC_HIDDEN_RPT_ANT_NUM(data);

	protocol = GET_wMBOX0_MAC_HIDDEN_RPT_80211_PROTOCOL(data);
	nic = GET_wMBOX0_MAC_HIDDEN_RPT_NIC_ROUTER(data);

	if (DBG_wMBOX0_MAC_HIDDEN_RPT_HANDLE) {
		for (i = 0; i < len; i++)
           MpTrace(COMP_RICHD, DBG_NORMAL, ("%s: 0x%02X\n", __func__, *(data + i)));
		
        MpTrace(COMP_RICHD, DBG_NORMAL, ("uuid x:0x%02x y:0x%02x z:0x%x crc:0x%x\n", uuid_x, uuid_y,
				uuid_z, uuid_crc));
        MpTrace(COMP_RICHD, DBG_NORMAL, ("hci_type:0x%x\n", hci_type));
        MpTrace(COMP_RICHD, DBG_NORMAL, ("package_type:0x%x\n", package_type));
        
        MpTrace(COMP_RICHD, DBG_NORMAL, ("wl_func:0x%x\n", wl_func));
        MpTrace(COMP_RICHD, DBG_NORMAL, ("hw_stype:0x%x\n", hw_stype));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("bw:0x%x\n", bw));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("ant_num:0x%x\n", ant_num));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("protocol:0x%x\n", protocol));
		MpTrace(COMP_RICHD, DBG_NORMAL, ("nic:0x%x\n", nic));
	}
	
	hal_data->PackageType = package_type;
	hal_spec->wl_func &= mac_hidden_wl_func_to_hal_wl_func(wl_func);
	hal_spec->bw_cap &= mac_hidden_max_bw_to_hal_bw_cap(bw);
	hal_spec->nss_num = wl_min(hal_spec->nss_num, ant_num);
	hal_spec->proto_cap &= mac_hidden_proto_to_hal_proto_cap(protocol);

	ret = TRUE;

exit:
	return ret;
}

static int Func_Chip_Hw_Read_Mac_Hidden_Rpt(PNIC Nic)
{
	int ret = FALSE;
	int ret_fwdl;
	u8 mac_hidden_rpt[MAC_HIDDEN_RPT_LEN] = { 0 };
//	u32 start = Func_Of_Proc_Get_Current_Time();
	u32 cnt = 0;
	u32 timeout_ms = 800;
	u32 min_cnt = 10;
	u8 id = wMBOX0_MAC_HIDDEN_RPT + 1;
	int i;
	u32 outbox[MAC_HIDDEN_RPT_LEN + 1] = { 0 };

	HwPlatformIOWrite1Byte(Nic, REG_wMBOX0EVT_MSG_NORMAL, id);
	
	ret_fwdl = Func_Of_Proc_Chip_Hw_Fw_Dl(Nic, FALSE);
	if (ret_fwdl != TRUE)
		goto mac_hidden_rpt_hdl;

//	start = Func_Of_Proc_Get_Current_Time();
	do {
		cnt++;
		id = HwPlatformIORead1Byte(Nic, REG_wMBOX0EVT_MSG_NORMAL);
		if (id == wMBOX0_MAC_HIDDEN_RPT )
			break;
		Func_Of_Proc_Msleep_Os(10);
//	} while (Func_Of_Proc_Get_Passing_Time_Ms(start) < timeout_ms || cnt < min_cnt);
    } while(1); 

	if (id == wMBOX0_MAC_HIDDEN_RPT) {
		for (i = 0; i < MAC_HIDDEN_RPT_LEN; i++)
			mac_hidden_rpt[i] =
				HwPlatformIORead1Byte(Nic, REG_wMBOX0EVT_MSG_NORMAL + 2 + i);
	}

mac_hidden_rpt_hdl:
	Func_Dev_To_Host_Mac_Hidden_Rpt_Hdl(Nic, mac_hidden_rpt, MAC_HIDDEN_RPT_LEN);

	if (ret_fwdl == TRUE && id == wMBOX0_MAC_HIDDEN_RPT)
		ret = TRUE;

exit:
	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s %s! (%u, %dms), fwdl:%d, id:0x%02x\n", __func__,
			(ret == TRUE) ? "OK" : "Fail", cnt,
			0x0, ret_fwdl, id));
	        //Func_Of_Proc_Get_Passing_Time_Ms(start), ret_fwdl, id));

	return ret;
}

static VOID Func_Initwadptdatavariablesbyprom_Process(IN PNIC Nic)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	struct hal_spec_t *phal_spec = GET_HAL_SPEC(Nic);
	u32 *pread;
	u32 len;
	int intger;
	int remainder;
	int offset = 0;
	int word_len;
	int i;
	u32 res[4] = { 0 };
#ifndef CONFIG_DIRECT_PWRCTL                                                                                    
        u32 msg_box[2] = { 0 };                                                                                 
#else                                                                                                           
        u32 msg_box[28] = { 0 };                                                                                
                                                                                                                
        u32 power[26] = {0x24,0x25,0x26,0x27,0x27,0x28,0x2d,0x2e,0x2e,0x2f,0x2f,0x30,0x2d,0x2e,0x2e,0x2f,0x2f,0x30,0x2b,0x2c,0x2c,0x2d,0x2e,0x19,0x04,0xad};
                                                                                                                
#endif  
	u32 tmp;
	u8 *hwinfo = NULL;

	if (sizeof(pHalData->efuse_eeprom_data) < HWSET_MAX_SIZE_9086X)
		MpTrace(COMP_RICHD, DBG_NORMAL, ("[WARNING] size of efuse_eeprom_data is less than HWSET_MAX_SIZE_9086X!\n"));

	hwinfo = pHalData->efuse_eeprom_data;

	pread = (u32 *) hwinfo;
	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_HAL_EFUSEMAP_LEN, NULL, &len, 0, 1)) {
		    MpTrace(COMP_RICHD, DBG_NORMAL, ("Error :===>%s,fail,code :UMSG_OPS_HAL_EFUSEMAP_LEN!!!\n",
				__func__));

		return;
	}

	//WL_INFO("[MC]===> len= %d\n", len);
	word_len = len / 4;
	intger = word_len / MAILBOX_MAX_RDLEN;
	remainder = word_len % MAILBOX_MAX_RDLEN;

	for (i = 0; i < intger; i++) {
		if (!Func_Mcu_Special_Func_Interface
			(Nic, UMSG_OPS_HAL_EFUSEMAP, pread, MAILBOX_MAX_RDLEN, offset)) {
			return;
		} else {
			offset = offset + MAILBOX_MAX_RDLEN;
		}
		pread = pread + MAILBOX_MAX_RDLEN;
	}

	if (remainder > 0) {
		if (!Func_Mcu_Special_Func_Interface
			(Nic, UMSG_OPS_HAL_EFUSEMAP, pread, remainder, offset)) {
			return;
		}
	}
#if 0
	for (i = 0; i < 512; i++)
		WL_INFO("%02X%s", hwinfo[i], (((i + 1) % 16) == 0) ? "\n" : " ");
#endif

	Func_Chip_Hw_Efuseparseidcode(Nic, hwinfo);
	Func_Chip_Hw_Efuseparsepidvid_Process(Nic, hwinfo,
								pHalData->bautoload_fail_flag);
	Func_Chip_Hw_Efuseparseeepromver_Process(Nic, hwinfo,
								  pHalData->bautoload_fail_flag);
	Func_Chip_Hw_Config_Macaddr(Nic, pHalData->bautoload_fail_flag);

        msg_box[1] = (u32) pHalData->bautoload_fail_flag;
#ifndef CONFIG_DIRECT_PWRCTL
        msg_box[0] = 0;
		MpTrace(COMP_RICHD, DBG_NORMAL, ("drv running in normal mode\n"));
	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_HAL_EFUSE_BAUTOLOAD_FLAG, msg_box, res, 2, 4)) {
				MpTrace(COMP_RICHD, DBG_NORMAL, 
					("Error :===>%s,fail,code :WLAN_OPS_DXX0_HAL_EEPORM_BAUTOLOAD_FLAG!!!\n",
                         __func__));
                return;
        }
#else
        msg_box[0] = 1;
		MpTrace(COMP_RICHD, DBG_NORMAL, ("drv running in PWRCTL mode\n"));
        for (i = 0; i < 26; i++)
                msg_box[i + 2] = power[i];

	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_HAL_EFUSE_BAUTOLOAD_FLAG, msg_box, res, 28, 4)) {
		        MpTrace(COMP_RICHD, DBG_NORMAL, 
					("Error :===>%s,fail,code :WLAN_OPS_DXX0_HAL_EEPORM_BAUTOLOAD_FLAG!!!\n",
                         __func__));
                return;
        }

#endif

#if 0
	for (i = 0; i < 512; i++)
		WL_INFO("%02X%s", hwinfo[i], (((i + 1) % 16) == 0) ? "\n" : " ");
#endif

	Func_Chip_Hw_Efuseparsetxpowerinfo_Process(Nic, hwinfo,
									pHalData->bautoload_fail_flag);

	Func_Chip_Hw_Efuseparsechnlplan_Process(Nic, hwinfo,
								 pHalData->bautoload_fail_flag);
	Func_Chip_Hw_Efuseparsethermalmeter_Process(Nic, hwinfo,
									 pHalData->bautoload_fail_flag);
	Func_Chip_Hw_Efuseparsepowersavingmode_Process(Nic, hwinfo,
										pHalData->bautoload_fail_flag);

	Func_Chip_Hw_Efuseparseeepromver_Process(Nic, hwinfo,
								  pHalData->bautoload_fail_flag);
	Func_Chip_Hw_Efuseparsecustomerid_Process(Nic, hwinfo,
								   pHalData->bautoload_fail_flag);
	Func_Chip_Hw_Efuseparsextal_Process(Nic, hwinfo, pHalData->bautoload_fail_flag);

	Func_Chip_Hw_Efuseparsekfreedata_Process(Nic, hwinfo,
								  pHalData->bautoload_fail_flag);
	Func_Chip_Hw_Read_Mac_Hidden_Rpt(Nic);

	pHalData->PackageType = res[0];
	phal_spec->wl_func &= res[1];
	phal_spec->bw_cap &= res[2];
	phal_spec->nss_num = res[3];
    MpTrace(COMP_RICHD, DBG_NORMAL, ("%s(): REPLACEMENT = %x\n", __func__, 0));
}

void Func_Of_Proc_Chip_Hw_Read_Chip_Info(PNIC Nic)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	u32 start = Func_Of_Proc_Get_Current_Time();
	u8 eeValue;
	u32 value32;
	#ifdef SimpleTest
		Nic->registrypriv.mp_mode = 1;
		Nic->registrypriv.RegPwrTrimEnable = 1;
	#endif
	MpTrace(COMP_RICHD, DBG_NORMAL,("%s start!\n", __func__));
	Func_To_Get_Eeprom_Size_Process(Nic);

	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_HAL_EFSUSESEL, NULL, NULL, 0, 0)) {
	    MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Error :===>%s,fail,code :UMSG_OPS_HAL_EFSUSESEL!!!\n", __func__));
		return;
	}

	pHalData->rf_chip = 4;

	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_HAL_EFUSETYPE, NULL, &value32, 0, 1)) {
	    MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Error :===>%s,fail,code :UMSG_OPS_HAL_EFUSETYPE!!!\n", __func__));
		return;
	}
	eeValue = (u8) value32;
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("[MC]==> eeValue = %d\n", eeValue));
	pHalData->EepromOrEfuse = (eeValue & EEPROMSEL) ? TRUE : FALSE;
	pHalData->bautoload_fail_flag = (eeValue & EEPROM_EN) ? FALSE : TRUE;
	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("Boot from %s, Autoload %s !\n",
			(pHalData->EepromOrEfuse ? "EEPROM" : "EFUSE"),
			(pHalData->bautoload_fail_flag ? "Fail" : "OK")));

	Func_Initwadptdatavariablesbyprom_Process(Nic);

	MpTrace(COMP_RICHD, DBG_NORMAL, 
		("%s in %d ms\n", __func__, Func_Of_Proc_Get_Passing_Time_Ms(start)));
}

void Func_Of_Proc_Chip_Hw_Read_Chip_Version(PNIC Nic)
{
	u32 value32;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s start\n",__func__));

	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_READ_VERSION, NULL, &value32, 0, 1)) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("Error :===>%s,fail,code :UMSG_OPS_READ_VERSION!!!\n", __func__));
		return;
	}

	MpTrace(COMP_RICHD, DBG_NORMAL, ("Read_Chip_Version OK!\n"));

	HwPlatformIOWrite4Byte(Nic, 0x24, 0x350016ef);


	Func_Of_Proc_Chip_Hw_Config_Rftype(Nic);
	MpTrace(COMP_RICHD, DBG_NORMAL, ("%s end\n",__func__));	

}

void Func_Of_Proc_Chip_Hw_Def_Value_Init(PNIC Nic)
{
	PHAL_DATA_TYPE pHalData;
	u8 i;
	pHalData = GET_HAL_DATA(Nic);

	if (!is_primary_wadptdata(Nic))
		return;

	pHalData->fw_ractrl = FALSE;
	if (!Nic->pwrctl_priv.bkeepfwalive)
		pHalData->LastHMEBoxNum = 0;

	Nic->registrypriv.wireless_mode = WIRELESS_11BG_24N;

	pHalData->bIQKInitialized = FALSE;

	pHalData->IntrMask[0] = (u32) (0);

	pHalData->IntrMask[1] = (u32) (0);

	pHalData->EfuseUsedBytes = 0;
	pHalData->EfuseUsedPercentage = 0;
#ifdef HAL_EFUSE_MEMORY
	pHalData->EfuseHal.fakeEfuseBank = 0;
	pHalData->EfuseHal.fakeEfuseUsedBytes = 0;
	Func_Of_Proc_Pre_Memset(pHalData->EfuseHal.fakeEfuseContent, 0xFF, EFUSE_MAX_HW_SIZE);
	Func_Of_Proc_Pre_Memset(pHalData->EfuseHal.fakeEfuseInitMap, 0xFF, EFUSE_MAX_MAP_LEN);
	Func_Of_Proc_Pre_Memset(pHalData->EfuseHal.fakeEfuseModifiedMap, 0xFF,
				EFUSE_MAX_MAP_LEN);
#endif

	Func_Of_Proc_Init_Hal_Com_Default_Value(Nic);

	{
#if 0	//////need to check	
		struct dvobj_priv *dvobj = wadptdata_to_dvobj(Nic);
		struct hal_spec_t *hal_spec = GET_HAL_SPEC(Nic);

		dvobj->macid_ctl.num = wl_min(hal_spec->macid_num, MACID_NUM_SW_LIMIT);

		dvobj->cam_ctl.sec_cap = hal_spec->sec_cap;
		dvobj->cam_ctl.num =
			wl_min(hal_spec->sec_cam_ent_num, SEC_CAM_ENT_NUM_SW_LIMIT);
#endif
	}
}

u8 Func_Of_Proc_Chip_Hw_Data_Init(PNIC Nic)
{
#if 0
		if (is_primary_wadptdata(Nic)) {
		Nic->HalData = malloc(sizeof(HAL_DATA_TYPE));
		if (Nic->HalData == NULL) {
			MpTrace(COMP_RICHD, DBG_NORMAL, 
				("cant not alloc memory for HAL DATA \n"));
			return FALSE;
		}
	}
#endif
	return TRUE;
}

void Func_Of_Proc_Chip_Hw_Data_Deinit(PNIC Nic)
{
#if 0
	if (is_primary_wadptdata(Nic)) {

		if (Nic->HalData!=NULL) {
			free(&Nic->HalData, sizeof(HAL_DATA_TYPE));
			Nic->HalData = NULL;
		}
	}
#endif
}

void Func_Of_Proc_Chip_Hw_Free_Data(PNIC Nic)
{
	Func_Of_Proc_Chip_Hw_Data_Deinit(Nic);
}

void Func_Of_Proc_Chip_Hw_Dm_Init(PNIC Nic)
{
	if (is_primary_wadptdata(Nic)) {
		PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

		Func_Of_Init_Dm_Priv(Nic);

		Func_Of_Proc_Pre_Spinlock_Init(&pHalData->IQKSpinLock);

		Func_Chip_Bb_Load_Tx_Power_Ext_Info(Nic, 1);
	}
}

void Func_Of_Proc_Chip_Hw_Dm_Deinit(PNIC Nic)
{
	if (is_primary_wadptdata(Nic)) {
		PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

		Func_Of_Deinit_Dm_Priv(Nic);

		Func_Of_Proc_Pre_Spinlock_Free(&pHalData->IQKSpinLock);
	}
}
#define PWR_CMD_READ			0x00

#define PWR_CMD_WRITE			0x01

#define PWR_CMD_POLLING			0x02

#define PWR_CMD_DELAY			0x03

#define PWR_CMD_END				0x04

#define PWR_BASEADDR_MAC		0x00
#define PWR_BASEADDR_USB		0x01
#define PWR_BASEADDR_PCIE		0x02
#define PWR_BASEADDR_SDIO		0x03

#define	PWR_INTF_SDIO_MSK		BIT(0)
#define	PWR_INTF_USB_MSK		BIT(1)
#define	PWR_INTF_PCI_MSK		BIT(2)
#define	PWR_INTF_ALL_MSK		(BIT(0)|BIT(1)|BIT(2)|BIT(3))

#define	PWR_FAB_TSMC_MSK		BIT(0)
#define	PWR_FAB_UMC_MSK			BIT(1)
#define	PWR_FAB_ALL_MSK			(BIT(0)|BIT(1)|BIT(2)|BIT(3))

#define	PWR_CUT_TESTCHIP_MSK	BIT(0)
#define	PWR_CUT_A_MSK			BIT(1)
#define	PWR_CUT_B_MSK			BIT(2)
#define	PWR_CUT_C_MSK			BIT(3)
#define	PWR_CUT_D_MSK			BIT(4)
#define	PWR_CUT_E_MSK			BIT(5)
#define	PWR_CUT_F_MSK			BIT(6)
#define	PWR_CUT_G_MSK			BIT(7)
#define	PWR_CUT_ALL_MSK			0xFF

typedef enum _PWRSEQ_CMD_DELAY_UNIT_ {
	PWRSEQ_DELAY_US,
	PWRSEQ_DELAY_MS,
} PWRSEQ_DELAY_UNIT;

typedef struct _WL_PWR_CFG_ {
	u16 offset;
	u8 cut_msk;
	u8 fab_msk:4;
	u8 interface_msk:4;
	u8 base:4;
	u8 cmd:4;
	u8 msk;
	u8 value;
} WLAN_PWR_CFG, *PWLAN_PWR_CFG;

#define GET_PWR_CFG_OFFSET(__PWR_CMD)		__PWR_CMD.offset
#define GET_PWR_CFG_CUT_MASK(__PWR_CMD)		__PWR_CMD.cut_msk
#define GET_PWR_CFG_FAB_MASK(__PWR_CMD)		__PWR_CMD.fab_msk
#define GET_PWR_CFG_INTF_MASK(__PWR_CMD)	__PWR_CMD.interface_msk
#define GET_PWR_CFG_BASE(__PWR_CMD)			__PWR_CMD.base
#define GET_PWR_CFG_CMD(__PWR_CMD)			__PWR_CMD.cmd
#define GET_PWR_CFG_MASK(__PWR_CMD)			__PWR_CMD.msk
#define GET_PWR_CFG_VALUE(__PWR_CMD)		__PWR_CMD.value
#define	wlan9086X_TRANS_CARDEMU_TO_ACT_STEPS	13
#define	wlan9086X_TRANS_ACT_TO_CARDEMU_STEPS	15
#define	wlan9086X_TRANS_CARDEMU_TO_SUS_STEPS	14
#define	wlan9086X_TRANS_SUS_TO_CARDEMU_STEPS	15
#define	wlan9086X_TRANS_CARDEMU_TO_PDN_STEPS	15
#define	wlan9086X_TRANS_PDN_TO_CARDEMU_STEPS	15
#define	wlan9086X_TRANS_ACT_TO_LPS_STEPS		11
#define	wlan9086X_TRANS_LPS_TO_ACT_STEPS		13
#define	wlan9086X_TRANS_ACT_TO_SWLPS_STEPS		21
#define	wlan9086X_TRANS_SWLPS_TO_ACT_STEPS		14
#define	wlan9086X_TRANS_END_STEPS		1


#define wlan9086X_TRANS_CARDDIS_TO_CARDEMU													\
																\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_WRITE, BIT0, 0}, /*Set SDIO suspend local register*/	\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_POLLING, BIT1, BIT1}, /*wait power state to suspend*/\
							\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT3|BIT4, 0}, 	\
	{0x00C4, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT4, 0},

#define wlan9086X_TRANS_CARDEMU_TO_ACT 														\
																\
								\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT2, 0},	\
	{0x0006, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, BIT1, BIT1},	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT7, 0},	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, (BIT3), 0},	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT0, BIT0},	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, BIT0, 0},	 \
	{0x0027, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xff, 0x35},

#define wlan9086X_TRANS_END															\
																\
								\
	{0xFFFF, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,0,PWR_CMD_END, 0, 0},



WLAN_PWR_CFG wll9086X_card_enable_flow[wlan9086X_TRANS_ACT_TO_CARDEMU_STEPS +
									   wlan9086X_TRANS_CARDEMU_TO_PDN_STEPS +
									   wlan9086X_TRANS_END_STEPS] = {
	wlan9086X_TRANS_CARDDIS_TO_CARDEMU wlan9086X_TRANS_CARDEMU_TO_ACT
		wlan9086X_TRANS_END
};


u8 HalPwrSeqCmdParsing(PNIC Nic,
					   u8 CutVersion,
					   u8 FabVersion,
					   u8 InterfaceType, WLAN_PWR_CFG PwrSeqCmd[])
{
	WLAN_PWR_CFG PwrCfgCmd = { 0 };
	u8 bPollingBit = FALSE;
	u32 AryIdx = 0;
	u8 value = 0;
	u32 offset = 0;
	u32 pollingCount = 0;
	u32 maxPollingCnt = 5000;

	do {
		PwrCfgCmd = PwrSeqCmd[AryIdx];

			if ((GET_PWR_CFG_FAB_MASK(PwrCfgCmd) & FabVersion) &&
			(GET_PWR_CFG_CUT_MASK(PwrCfgCmd) & CutVersion) &&
			(GET_PWR_CFG_INTF_MASK(PwrCfgCmd) & InterfaceType)) {
			switch (GET_PWR_CFG_CMD(PwrCfgCmd)) {
			case PWR_CMD_READ:
				MpTrace(COMP_RICHD, DBG_NORMAL, 
					("HalPwrSeqCmdParsing: PWR_CMD_READ\n"));
				break;

			case PWR_CMD_WRITE:
				MpTrace(COMP_RICHD, DBG_NORMAL, 
					("HalPwrSeqCmdParsing: PWR_CMD_WRITE\n"));
				offset = GET_PWR_CFG_OFFSET(PwrCfgCmd);

				value = HwPlatformIORead1Byte(Nic, offset);

				value = value & (~(GET_PWR_CFG_MASK(PwrCfgCmd)));
				value =
					value | (GET_PWR_CFG_VALUE(PwrCfgCmd) &
							 GET_PWR_CFG_MASK(PwrCfgCmd));

				HwPlatformIOWrite1Byte(Nic, offset, value);

				break;

			case PWR_CMD_POLLING:
				MpTrace(COMP_RICHD, DBG_NORMAL, 
					("HalPwrSeqCmdParsing: PWR_CMD_POLLING\n"));
				
				bPollingBit = FALSE;
				offset = GET_PWR_CFG_OFFSET(PwrCfgCmd);

				do {
					value = HwPlatformIORead1Byte(Nic, offset);

					value = value & GET_PWR_CFG_MASK(PwrCfgCmd);
					if (value ==
						(GET_PWR_CFG_VALUE(PwrCfgCmd) &
						 GET_PWR_CFG_MASK(PwrCfgCmd)))
						bPollingBit = TRUE;
					else
						Func_Of_Proc_Mdelay_Os(10);

					if (pollingCount++ > maxPollingCnt) {
						MpTrace(COMP_RICHD, DBG_NORMAL, 
							("HalPwrSeqCmdParsing: Fail to polling Offset[%#x]=%02x\n",
									   offset, value));
						return FALSE;
					}
				} while (!bPollingBit);

				break;

			case PWR_CMD_DELAY:
				MpTrace(COMP_RICHD, DBG_NORMAL, ("HalPwrSeqCmdParsing: PWR_CMD_DELAY\n"));
				
				if (GET_PWR_CFG_VALUE(PwrCfgCmd) == PWRSEQ_DELAY_US)
					Func_Of_Proc_Mdelay_Os(GET_PWR_CFG_OFFSET(PwrCfgCmd));
				else
					Func_Of_Proc_Mdelay_Os(GET_PWR_CFG_OFFSET(PwrCfgCmd) * 1000);
				break;

			case PWR_CMD_END:
				MpTrace(COMP_RICHD, DBG_NORMAL, ("HalPwrSeqCmdParsing: PWR_CMD_END\n"));
				return TRUE;
				break;

			default:
				MpTrace(COMP_RICHD, DBG_NORMAL, ("HalPwrSeqCmdParsing: Unknown CMD!!\n"));
				break;
			}
		}

		AryIdx++;
	} while (1);

	return TRUE;
}


#define REG_AFE_XTAL_CTRL				    0x0024
#define REG_CR_9086X						0x0100
#define HCI_TXDMA_EN			BIT(0)
#define HCI_RXDMA_EN			BIT(1)
#define TXDMA_EN				BIT(2)
#define RXDMA_EN				BIT(3)
#define PROTOCOL_EN				BIT(4)
#define SCHEDULE_EN				BIT(5)
#define MACTXEN					BIT(6)
#define MACRXEN					BIT(7)
#define ENSWBCN					BIT(8)
#define ENSEC					BIT(9)
#define CALTMR_EN				BIT(10)


static u32 _InitPowerOn_9086XU(PNIC Nic)
{

	u32 status = TRUE;
	u16 value16 = 0;
	u8 value8 = 0;
	u32 value32;

	if (!HalPwrSeqCmdParsing
		(Nic, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK,
		 wll9086X_card_enable_flow))
		return FALSE;

	HwPlatformIOWrite1Byte(Nic, REG_CR_9086X, 0x00);
	value16 = HwPlatformIORead2Byte(Nic, REG_CR_9086X);
	value16 |= (HCI_TXDMA_EN | HCI_RXDMA_EN | TXDMA_EN | RXDMA_EN
				| PROTOCOL_EN | SCHEDULE_EN | ENSEC | CALTMR_EN);
	HwPlatformIOWrite2Byte(Nic, REG_CR_9086X, value16);

	return status;
}

u32 Func_Of_Proc_Chip_Hw_Power_On(PNIC Nic)
{
//	struct registry_priv *regsty = &Nic->registrypriv;
	u32 status = TRUE;
	u16 value16 = 0;
	u8 value8 = 0;
	u32 value32;
	int i = 0;

	u8 powerdzf;
	u8 cnt = 0;

	
	HwPlatformIOWrite1Byte(Nic, 0xac, 0x06);
	HwPlatformIOWrite1Byte(Nic, 0x98, 0xFF);
	HwPlatformIOWrite1Byte(Nic, 0x99, 0xFF);
	HwPlatformIOWrite1Byte(Nic, 0x9a, 0x01);

	
#if 0
	powerdzf = HwPlatformIORead1Byte(Nic, 0xac);
	powerdzf |= BIT0;
	HwPlatformIOWrite1Byte(Nic, 0xac, powerdzf);

	Func_Of_Proc_Mdelay_Os(1);

	while (!(HwPlatformIORead1Byte(Nic, 0xac) & BIT4)) {
		++cnt;
		if (cnt >= 50) {
			WL_INFO("InitPowerOn_9086XU FAIL!\n");
			return FALSE;
		}

		Func_Of_Proc_Mdelay_Os(1);
		WL_INFO_L1("[MC] cnt = %d\n", cnt);
	}
#else

    status = _InitPowerOn_9086XU(Nic);
#endif
//	pdvobjpriv->power_status = false;



  return status;

}
#define wlan9086X_TRANS_ACT_TO_LPS														\
																\
								\
	{0x0139, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT0, BIT0},	\
	{0x0522, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0xFF},	\
	{0x05F8, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, 0xFF, 0},	\
	{0x05F9, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, 0xFF, 0},	\
	{0x05FA, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, 0xFF, 0},	\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT0, 0},	\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_DELAY, 0, PWRSEQ_DELAY_US},	\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT1, 0},	\
	{0x0100, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0x3F},	\
	{0x0101, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT1, 0},	\
	{0x0553, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT5, BIT5},
#define wlan9086X_TRANS_ACT_TO_CARDEMU													\
																\
								\
	{0x001F, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0},	\
	{0x004E, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT7, 0},\
	{0x0027, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xff, 0x34}, \
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT1, BIT1}, 	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, BIT1, 0}, 	\

#define wlan9086X_TRANS_CARDEMU_TO_CARDDIS													\
																\
								\
	{0x0007, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0x00}, 	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK|PWR_INTF_SDIO_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT3|BIT4, BIT3}, 	\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_WRITE, BIT0, BIT0}, 	\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_POLLING, BIT1, 0},  \
	{0x00C4, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT4, BIT4},



WLAN_PWR_CFG wll9086X_enter_lps_flow[wlan9086X_TRANS_ACT_TO_LPS_STEPS +
									 wlan9086X_TRANS_END_STEPS] = {
	wlan9086X_TRANS_ACT_TO_LPS wlan9086X_TRANS_END
};

WLAN_PWR_CFG wll9086X_card_disable_flow[wlan9086X_TRANS_ACT_TO_CARDEMU_STEPS +
										wlan9086X_TRANS_CARDEMU_TO_PDN_STEPS +
										wlan9086X_TRANS_END_STEPS] = {
	wlan9086X_TRANS_ACT_TO_CARDEMU wlan9086X_TRANS_CARDEMU_TO_CARDDIS
		wlan9086X_TRANS_END
};


#define REG_SYS_FUNC_EN_9086X			0x0002
#define REG_MCUFWDL_9086X				0x0080
#define REG_TX_RPT_CTRL					0x04EC
VOID CardDisablewlan9086XU(PNIC Nic)
{
	u8 u1bTmp;
#ifdef CONFIG_FORCE_241FFF
        HwPlatformIOWrite4Byte(Nic, REG_AFE_XTAL_CTRL, 0x35001fff);
#endif
	u1bTmp = HwPlatformIORead1Byte(Nic, REG_TX_RPT_CTRL);
	HwPlatformIOWrite1Byte(Nic, REG_TX_RPT_CTRL, u1bTmp & (~BIT1));

	HwPlatformIOWrite1Byte(Nic, REG_CR_9086X, 0x0);

	HalPwrSeqCmdParsing(Nic, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,
						PWR_INTF_USB_MSK, wll9086X_enter_lps_flow);

	u1bTmp = HwPlatformIORead1Byte(Nic, REG_SYS_FUNC_EN_9086X + 1);
	HwPlatformIOWrite1Byte(Nic, REG_SYS_FUNC_EN_9086X + 1, (u1bTmp & (~BIT2)));

	HwPlatformIOWrite1Byte(Nic, REG_MCUFWDL_9086X, 0x00);

	HalPwrSeqCmdParsing(Nic, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,
						PWR_INTF_USB_MSK, wll9086X_card_disable_flow);

	Nic->bFWReady = FALSE;
}

void Func_Of_Proc_Chip_Hw_Power_Off(PNIC Nic)
{
	u8 u1bTmp;

	u8 powerdzf;
	u8 cnt = 0;

	struct macid_ctl_t *macid_ctl = &Nic->dvobjpriv.macid_ctl;

	Func_Of_Proc_Pre_Memset(macid_ctl->wmbox1_msr, 0, MACID_NUM_SW_LIMIT);

//	pdvobjpriv->power_status = true;

#if 0
	powerdzf = HwPlatformIORead1Byte(Nic, 0xac);
	powerdzf &= (~BIT0);
	HwPlatformIOWrite1Byte(Nic, 0xac, powerdzf);

	Func_Of_Proc_Mdelay_Os(1);

	while ((HwPlatformIORead1Byte(Nic, 0xac) & BIT4)) {
		++cnt;
		if (cnt >= 50) {
			WL_INFO("CardDisablewlan9086XU FAIL!\n");
			return;
		}

		Func_Of_Proc_Mdelay_Os(1);
		WL_INFO("cnt = %d\n", cnt);
	}
#else
	MpTrace(COMP_RICHD, DBG_NORMAL,("==> %s\n", __func__));

	CardDisablewlan9086XU(Nic);
#endif
	Nic->bFWReady = FALSE;

}

void Func_Of_Proc_Chip_Hw_Init_Opmode(PNIC Nic)
{
	NDIS_802_11_NETWORK_INFRASTRUCTURE networkType =
		Ndis802_11InfrastructureMax;
	struct mlme_priv *pmlmepriv = &(Nic->mlmepriv);
	sint fw_state;

	fw_state = get_fwstate(pmlmepriv);

	if (fw_state & WIFI_ADHOC_STATE)
		networkType = Ndis802_11IBSS;
	else if (fw_state & WIFI_STATION_STATE)
		networkType = Ndis802_11Infrastructure;
	else if (fw_state & WIFI_AP_STATE)
		networkType = Ndis802_11APMode;
	else
		return;
#if 0
	proc_setopmode_cmd_func(Nic, networkType, FALSE);
#endif
}

uint Func_Of_Proc_Chip_Hw_Init(PNIC Nic)
{
	uint status = TRUE;
	u32 var;

	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	int i;

	status = Func_Of_Hw_Init(Nic);

	if (status == TRUE) {
		pHalData->hw_init_completed = TRUE;

		if (Nic->registrypriv.notch_filter == 1) {
			var = 1;
			Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_NOTCH_FILTER, &var,
										NULL, 1, 0);
		}
#if 0   //need to change
		for (i = 0; i < dvobj->iface_nums; i++)
			sl_sec_restore_wep_key(dvobj->Nics[i], 1);
#endif

		
#ifndef SimpleTest
//		hw_mlme_ext_to_init_func(1, Nic);
#endif

#if 0
		Func_Of_Proc_Chip_Hw_Init_Opmode(Nic);

#ifdef CONFIG_RF_POWER_TRIM
		Func_Of_Proc_Bb_Rf_Gain_Offset(Nic);
#endif
#endif
	    MpTrace(COMP_RICHD, DBG_NORMAL, ("%s: hal__init OK\n",__func__));
	} else {
		pHalData->hw_init_completed = FALSE;
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s: hal__init fail\n",__func__));
	}


	MpTrace(COMP_RICHD, DBG_NORMAL, ("-wll908621x_hal_init:status=0x%x\n", status));
	return status;

}

uint Func_Of_Proc_Chip_Hw_Deinit(PNIC Nic)
{
	uint status = TRUE;

	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	int i;
	_func_enter_;

	status = Func_Of_Hw_Deinit(Nic);

	if (status == TRUE) {
		pHalData->hw_init_completed = FALSE;
	} else {
	    MpTrace(COMP_RICHD, DBG_NORMAL, ("\n Func_Of_Proc_Chip_Hw_Deinit: hal_init fail\n"));	
	}

	_func_exit_;

	return status;
}

void Func_Of_Proc_Chip_Hw_Get_Hwreg(PNIC Nic, u8 variable, u8 * val)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

	_func_enter_;

	switch (variable) {
	default:
		Func_Mcu_Get_Hwreg_Process(Nic, variable, val);
		break;
	}

	_func_exit_;
}

void Func_Of_Proc_Chip_Hw_Set_Hwreg(PNIC Nic, u8 variable, u8 * val)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

	_func_enter_;

	switch (variable) {
	case HW_VAR_RXDMA_AGG_PG_TH:
#ifdef CONFIG_USB_RX_AGGREGATION
		{
			u8 threshold = *val;

			if (threshold == 0)
				threshold = pHalData->UsbRxAggPageCount;
			Func_Mcu_Set_Hwreg_Process(Nic, HW_VAR_RXDMA_AGG_PG_TH, &threshold);
		}
#endif
		break;

	case HW_VAR_SET_RPWM:
		HwPlatformIOWrite1Byte(Nic, REG_USB_HRPWM, *val);
		break;

	case HW_VAR_TRIGGER_GPIO_0:
		Func_Mcu_Hal_Set_Hwreg(Nic, HW_VAR_TRIGGER_GPIO_0, 0, 0);
		break;

	default:
		Func_Mcu_Set_Hwreg_Process(Nic, variable, val);
		break;
	}

	_func_exit_;
}

static void Func_Fw_Process_wMBOX0_Event(PNIC Nic, u8 * wmbox0Buf, int len)
{
	u8 index = 0;
	int ret = FALSE;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	int w_len = len / 4;
	int b_len = len % 4;
	int i;
	int id = 0;
	int buf_id = 0;
	int buf_len = 0;
	int _integer;
	int _remainder;
	u32 *buf = NULL;
	u8 *phead = NULL;
	u8 *pwrite = NULL;
	u32 offset = 0;
	int send_len = MAILBOX_MAX_TXLEN - 1;

	if (wmbox0Buf == NULL) {
	    MpTrace(COMP_RICHD, DBG_NORMAL, ("%s wmbox0buff is NULL\n", __func__));
		return;
	}

	buf_len = w_len + (b_len > 0 ? 1 : 0);
	if (buf_len <= 0) {
		goto exit;
	}

    //need to change

	pwrite = phead = wl_zmalloc(buf_len * 4);
	
	
	if (!phead) {
		goto exit;
	}
	memcpy(phead, wmbox0Buf, len);

	_integer = buf_len / send_len;

	_remainder = buf_len % send_len;

	offset = 0;

	for (i = 0; i < _integer; i++) {

		id = 0;

		buf = (u32 *) wl_zmalloc(send_len * 4);

		if (!buf) {
			goto exit;
		}

		if (send_len) {
			memcpy((u8 *) & buf[id], pwrite, send_len * 4);
			pwrite = pwrite + send_len * 4;
		}

		ret =
			Func_Mcu_Send_Buffer_Interface(Nic, UMSG_OPS_PHYDM_wMBOX0_CONTENT_PARS,
									 buf, send_len, offset, len);
		if (!ret) {
			
			MpTrace(COMP_RICHD, DBG_NORMAL, ("Error :===>%s,fail,code :UMSG_OPS_PHYDM_wMBOX0_CONTENT_PARS!!!\n",
				 __func__));

			goto exit;
		}
		offset = offset + send_len * 4;

		wl_mfree((u8 *) buf, send_len * 4);

		buf = NULL;
		
		MpTrace(COMP_RICHD, DBG_NORMAL, ("%s total len %d pkt len %d offset = %d", __func__, len,
				send_len, offset));
	}

	send_len = _remainder;

	if (send_len > 0) {

		buf = (u32 *) wl_zmalloc(send_len * 4);

		if (!buf) {
			goto exit;
		}

		id = 0;

		if (send_len) {
			memcpy((u8 *) & buf[id], pwrite, send_len * 4);
			pwrite = pwrite + send_len * 4;
		}

		ret =
			Func_Mcu_Send_Buffer_Interface(Nic, UMSG_OPS_PHYDM_wMBOX0_CONTENT_PARS,
									 buf, send_len, offset, len);
		if (!ret) {
			MpTrace(COMP_RICHD, DBG_NORMAL, 
				("Error :===>%s,fail,code :UMSG_OPS_PHYDM_wMBOX0_CONTENT_PARS!!!\n",
				 __func__));
			
			goto exit;
		}
		offset = offset + send_len * 4;

	}
exit:
	if (buf)
		wl_mfree((u8 *) buf, send_len * 4);
	if (phead)
		wl_mfree((u8 *) phead, buf_len * 4);
}

void Func_Of_Proc_Chip_Hw_Set_Hwreg_With_Buf(PNIC Nic, u8 * pbuffer, int length)
{
	wMBOX0_EVT_HDR C2hEvent;
	u8 *tmpBuf = NULL;
#ifdef CONFIG_WOWLAN
	struct pwrctrl_priv *pwrpriv = wadptdata_to_pwrctl(Nic);

	if (pwrpriv->wowlan_mode == TRUE) {
		
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("%s(): return because wowolan_mode==TRUE! CMDID=%d\n", __func__,
				pbuffer[0]));
		return;
	}
#endif

	Func_Fw_Process_wMBOX0_Event(Nic, pbuffer, length);

	return;
}

u8 Func_Of_Proc_Chip_Hw_Get_Def_Var(PNIC Nic, HAL_DEF_VARIABLE eVariable,
					   PVOID pValue)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u8 bResult = TRUE;

	switch (eVariable) {
	case HAL_DEF_IS_SUPPORT_ANT_DIV:
		break;

	case HAL_DEF_DRVINFO_SZ:
		*((u32 *) pValue) = DRVINFO_SZ;
		break;
	case HAL_DEF_MAX_RECVBUF_SZ:
		*((u32 *) pValue) = MAX_RECVBUF_SZ;
		break;
	case HAL_DEF_RX_PACKET_OFFSET:
		*((u32 *) pValue) = RXDESC_SIZE + DRVINFO_SZ * 8;
		break;
	case HW_VAR_MAX_RX_AMPDU_FACTOR:
		*((HT_CAP_AMPDU_FACTOR *) pValue) = MAX_AMPDU_FACTOR_64K;
		break;
	default:
		bResult = Func_To_Get_Hal_Def_Var_Process(Nic, eVariable, pValue);
		break;
	}

	return bResult;
}




















void Func_Of_Proc_Chip_Hw_Set_Odm_Var(PNIC Nic, HAL_MSG_VARIABLE eVariable,
						 PVOID pValue1, BOOLEAN bSet)
{
	msg_var_req_t msg_var_req;
	int ret;

	if (eVariable == HAL_MSG_INITIAL_GAIN) {

		u8 rx_gain = *((u8 *) (pValue1));
		u32 buf[3];

		buf[1] = 0;
		buf[2] = rx_gain;
		if (rx_gain == 0xff) {
			buf[0] = BIT1;
			ret =
				Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_MSG_PAUSEIG, buf,
											NULL, 3, 0);
			if (!ret) {
				MpTrace(COMP_RICHD, DBG_NORMAL,
					("Error :===>%s,fail,code :UMSG_OPS_MSG_PAUSEIG!!!\n",
						__func__));
				return;
			}
		} else {
			buf[0] = BIT0;
			ret =
				Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_MSG_PAUSEIG, buf,
											NULL, 3, 0);
			if (!ret) {
				MpTrace(COMP_RICHD, DBG_NORMAL,
					("Error :===>%s,fail,code :UMSG_OPS_MSG_PAUSEIG!!!\n",
						__func__));
				return;
			}
		}

		return;
	}
	msg_var_req.var = eVariable;
	msg_var_req.msg = (SIZE_T) pValue1;
	msg_var_req.set = (SIZE_T) bSet;

#if 0
	Func_Set_Msg_Var_Req(Nic, &msg_var_req);
#endif

}

void Func_Of_Proc_Chip_Hw_Get_Odm_Var(PNIC Nic, HAL_MSG_VARIABLE eVariable,
						 PVOID pValue1)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	switch (eVariable) {
	case HAL_MSG_DBG_FLAG:
		*((u8Byte *) pValue1) = (msg_rw_val.dbg_cmp & 0xFF);
		break;
	case HAL_MSG_DBG_LEVEL:
		*((u4Byte *) pValue1) = msg_rw_val.dbg_level;
		break;

#ifdef CONFIG_AUTO_CHNL_SEL_NHM
	case HAL_MSG_AUTO_CHNL_SEL:
		{
			MpTrace(COMP_RICHD, DBG_NORMAL,
				("[ACS-" ADPT_FMT "] HAL_MSG_AUTO_CHNL_SEL: GET_BEST_CHAN\n",
					ADPT_ARG(Nic)));
			
			if (IsSupported24G(Nic->registrypriv.wireless_mode))
				*((u8 *) (pValue1)) =
					MSG_GetAutoChannelSelectResult(pmsgpriv, BAND_ON_2_4G);
		}
		break;
#endif

	default:
		break;
	}
}

s32 Func_Of_Proc_Chip_Hw_Fw_Dl(PNIC Nic, u8 wowlan)
{
	int ret;
	u32 status;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_DW_FW, NULL, &status,
									0, 1);
	if (!status) {
		MpTrace(COMP_RICHD, DBG_NORMAL,("fw go fail!\n"));
		Nic->bFWReady = FALSE;
		pHalData->fw_ractrl = FALSE;
		return ret;
	} else {
		MpTrace(COMP_RICHD, DBG_NORMAL,("fw go sucess!\n"));
		Nic->bFWReady = TRUE;
		pHalData->fw_ractrl = TRUE;
	}

	return ret;
}

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
void Func_Of_Proc_Chip_Hw_Clear_Interrupt(PNIC Nic)
{
}
#endif

#if 0
u32 Func_Of_Proc_Chip_Hw_Inirp_Init(PNIC Nic)
{
	u8 i;
	struct recv_buf *precvbuf;
	uint status;
	struct dvobj_priv *pdev = wadptdata_to_dvobj(Nic);
	struct recv_priv *precvpriv = &(Nic->recvpriv);

	_func_enter_;
	status = TRUE;

	if (!is_primary_wadptdata(Nic))
		goto exit;

	precvpriv->ff_hwaddr = RECV_BULK_IN_ADDR;

	precvbuf = (struct recv_buf *)precvpriv->precv_buf;
	for (i = 0; i < NR_RECVBUFF; i++) {
		if (Func_Usb_Read_Port
			(Nic, precvpriv->ff_hwaddr, 0,
			 (unsigned char *)precvbuf) == FALSE) {
			status = FALSE;
			goto exit;
		}

		precvbuf++;
		precvpriv->free_recv_buf_queue_cnt--;
	}

exit:

	_func_exit_;

	return status;
}


u32 Func_Of_Proc_Chip_Hw_Inirp_Deinit(PNIC Nic)
{

	if (is_primary_wadptdata(Nic))
		return TRUE;

	MpTrace(COMP_RICHD, DBG_NORMAL,("\n ===> usb_rx_deinit\n"));

	wl_read_port_cancel(Nic);
	return TRUE;
}
#endif
#if 0
s32 Func_Of_Proc_Chip_Hw_Mgnt_Xmit(PNIC Nic, struct xmit_frame * pmgntframe)
{
	s32 ret = FALSE;
	u8 *pframe, subtype;
	struct wl_ieee80211_hdr *pwlanhdr;
	struct sta_info *psta;
	struct sta_priv *pstapriv = &Nic->stapriv;

	proc_mgntframe_attrib_addr_update_func(Nic, pmgntframe, 1);
	pframe = (u8 *) (pmgntframe->buf_addr) + TXDESC_OFFSET;
	subtype = GetFrameSubType(pframe);

#ifdef CONFIG_IEEE80211W
	if (Nic->securitypriv.binstallBIPkey == TRUE
		&& (subtype == WIFI_DEAUTH || subtype == WIFI_DISASSOC
			|| subtype == WIFI_ACTION)) {
		if (IS_MCAST(pmgntframe->attrib.ra)
			&& pmgntframe->attrib.key_type != IEEE80211W_NO_KEY) {
			pmgntframe->attrib.encrypt = _BIP_;
		} else if (pmgntframe->attrib.key_type != IEEE80211W_NO_KEY) {
			psta = do_query_stainfo(pstapriv, pmgntframe->attrib.ra, 1);
			if (psta && psta->bpairwise_key_installed == TRUE) {
				pmgntframe->attrib.encrypt = _AES_;
				pmgntframe->attrib.bswenc = TRUE;
			} else {
				MpTrace(COMP_RICHD, DBG_NORMAL,
					("%s, %d, bpairwise_key_installed is FALSE\n", __func__,
						__LINE__));
				goto no_mgmt_coalesce;
			}
		}
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("encrypt=%d, bswenc=%d\n", pmgntframe->attrib.encrypt,
				pmgntframe->attrib.bswenc));

		do_mgmt_txp_coa(pmgntframe, Nic, pmgntframe->pkt, 1);
	}
#endif
no_mgmt_coalesce:
	ret = s_tx_mgnt(pmgntframe, Nic, 1);
	return ret;
}
#endif

u8 ra_sGI_get(struct sta_info *psta, u8 pad)
{
	u8 sgi = _FALSE, sgi_20m = _FALSE, sgi_40m = _FALSE;

#ifdef CONFIG_80211N_HT
	if (pad) {
		sgi_20m = psta->htpriv.sgi_20m;
		sgi_40m = psta->htpriv.sgi_40m;
	}
#endif
	if (psta->bw_mode == CHANNEL_WIDTH_40) {
		sgi = sgi_40m;
	} else {
		sgi = sgi_20m;
	}

	return sgi;
}

static void Func_Update_Hal_Ra_Mask_Process(PNIC Nic, struct sta_info *psta, u8 rssi_level)
{
	u32 mask, rate_bitmap;
	u8 shortGIrate = FALSE;
	//struct sta_info *psta = NULL;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct macid_ctl_t *macid_ctl = &Nic->dvobjpriv.macid_ctl;
	u32 mac_id = psta->mac_id;

	//if (mac_id < macid_ctl->num)
		//psta = macid_ctl->sta[mac_id];
	if (psta == NULL) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			(" macid:%u, sta is NULL\n", mac_id));
		return;
	}
	MpTrace(COMP_RICHD, DBG_NORMAL,("sta is %x\n", psta));

	shortGIrate = ra_sGI_get(psta, 1);


#if 1
	mask = psta->ra_mask = 0xfff;
	rate_bitmap = 0xffffffff;
	rate_bitmap = Func_Hw_Op_Get_Rate_Bitmap(Nic, mac_id, mask, rssi_level);
	MpTrace(COMP_RICHD, DBG_NORMAL,
		("%s => mac_id:%d, networkType:0x%02x, mask:0x%08x\n\t ==> rssi_level:%d, rate_bitmap:0x%08x,fwctrl:%x\n",
		 __func__, mac_id, psta->wireless_mode, mask, rssi_level, rate_bitmap,pHalData->fw_ractrl));

	mask &= rate_bitmap;

	if (pHalData->fw_ractrl == TRUE)
		Func_Of_Set_Fwmacidconfig_Cmd(Nic, mac_id, psta->raid,
									   psta->bw_mode, shortGIrate, mask);

	pHalData->INIDATA_RATE[mac_id] = psta->init_rate;
	MpTrace(COMP_RICHD, DBG_NORMAL,
		("%s(): mac_id=%d raid=0x%x bw=%d mask=0x%x init_rate=0x%x\n",
			__func__, mac_id, psta->raid, psta->bw_mode, mask, psta->init_rate));
#endif
}

void Func_Of_Proc_Chip_Hw_Update_Ra_Mask(struct sta_info *psta, u8 rssi_level)
{
	PNIC Nic;
	struct mlme_priv *pmlmepriv;

	if (!psta)
		return;

	Nic = psta->Nic;

	MpTrace(COMP_RICHD, DBG_NORMAL, ("RICHD:%s=====psta=%x,Nic:%x\n", __func__,psta,Nic));

	pmlmepriv = &(Nic->mlmepriv);

//#ifdef CONFIG_AP_MODE
//	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == TRUE) {
//		add_ratid_func(Nic, psta, rssi_level, 1);
//	} else {
//		Func_Update_Hal_Ra_Mask_Process(Nic, psta->mac_id, rssi_level);
//	}
//#else
    Func_Update_Hal_Ra_Mask_Process(Nic, psta, rssi_level);
//#endif
}

#ifdef CONFIG_AP_MODE
void Func_Of_Proc_Chip_Hw_Add_Ra_Tid(PNIC Nic, u64 bitmap, u8 * arg,
						u8 rssi_level)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	struct macid_ctl_t *macid_ctl = &Nic->dvobjpriv.macid_ctl;

	struct sta_info *psta = NULL;
	u8 mac_id = arg[0];
	u8 raid = arg[1];
	u8 shortGI = arg[2];
	u8 bw;
	u32 rate_bitmap = (u32) bitmap;
	u32 mask = rate_bitmap & 0x0FFFFFFF;

	if (mac_id < macid_ctl->num)
		psta = macid_ctl->sta[mac_id];
	if (psta == NULL) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
		    ( " macid:%u, sta is NULL\n", mac_id));
		return;
	}

	bw = psta->bw_mode;

	if (rssi_level != DM_RATR_STA_INIT)
		mask = Func_Hw_Op_Get_Rate_Bitmap(Nic, mac_id, mask, rssi_level);
	
	MpTrace(COMP_RICHD, DBG_NORMAL,
		("%s(): mac_id=%d raid=0x%x bw=%d mask=0x%x\n", __func__, mac_id,
			raid, bw, mask));
	Func_Of_Set_Fwmacidconfig_Cmd(Nic, mac_id, raid, bw, shortGI, mask);

}
#endif
#if 0
void Func_Of_Proc_Chip_Hw_Start_Thread(PNIC Nic)
{
}

void Func_Of_Proc_Chip_Hw_Stop_Thread(PNIC Nic)
{
}
#endif

static u32 Chip_Bb_Calculatebitshift(u32 BitMask)
{
	u32 i;

	for (i = 0; i <= 31; i++) {
		if (((BitMask >> i) & 0x1) == 1)
			break;
	}

	return (i);
}

u32 Func_Of_Proc_Chip_Hw_Read_Bbreg(PNIC Nic, u32 RegAddr, u32 BitMask)
{
	u32 ReturnValue = 0, OriginalValue, BitShift;
	u16 BBWaitCounter = 0;

	OriginalValue = HwPlatformIORead4Byte(Nic, RegAddr);
	BitShift = Chip_Bb_Calculatebitshift(BitMask);
	ReturnValue = (OriginalValue & BitMask) >> BitShift;

	return (ReturnValue);
}

void Func_Of_Proc_Chip_Hw_Write_Bbreg(PNIC Nic, u32 RegAddr, u32 BitMask,
						 u32 Data)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	u32 OriginalValue, BitShift;

	if (BitMask != bMaskDWord) {
		OriginalValue = HwPlatformIORead4Byte(Nic, RegAddr);
		BitShift = Chip_Bb_Calculatebitshift(BitMask);
		Data = ((OriginalValue & (~BitMask)) | ((Data << BitShift) & BitMask));
	}

	HwPlatformIOWrite4Byte(Nic, RegAddr, Data);
}

static u32
Func_Chip_Bb_Rf_Serial_Read_Process(IN PNIC Nic, IN u32 eRFPath, IN u32 Offset)
{
	u32 retValue = 0;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	BB_REGISTER_DEFINITION_T *pPhyReg = &pHalData->PHYRegDef;
	u32 NewOffset;
	u32 tmplong, tmplong2;
	u8 RfPiEnable = 0;
	u4Byte MaskforPhySet = 0;
	int i = 0;

    
	NDIS_WAIT_FOR_MUTEX(&Nic->dvobjpriv.rf_read_reg_mutex);

	Offset &= 0xff;

	NewOffset = Offset;

	tmplong2 =
		PHY_QueryBBReg(Nic, rFPGA0_XA_HSSIParameter2 | MaskforPhySet,
					   bMaskDWord);
	tmplong2 =
		(tmplong2 & (~bLSSIReadAddress)) | (NewOffset << 23) | bLSSIReadEdge;
	PHY_SetBBReg(Nic, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord,
				 tmplong2 & (~bLSSIReadEdge));

	tmplong2 =
		PHY_QueryBBReg(Nic, rFPGA0_XA_HSSIParameter2 | MaskforPhySet,
					   bMaskDWord);
	PHY_SetBBReg(Nic, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord,
				 tmplong2 & (~bLSSIReadEdge));
	PHY_SetBBReg(Nic, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord,
				 tmplong2 | bLSSIReadEdge);

	Func_Of_Proc_Udelay_Os(10);

	for (i = 0; i < 2; i++)
		Func_Of_Proc_Udelay_Os(50);
	Func_Of_Proc_Udelay_Os(10);

	RfPiEnable =
		(u1Byte) PHY_QueryBBReg(Nic,
								rFPGA0_XA_HSSIParameter1 | MaskforPhySet, BIT8);

	if (RfPiEnable) {
		retValue =
			PHY_QueryBBReg(Nic, pPhyReg->rfLSSIReadBackPi | MaskforPhySet,
						   bLSSIReadBackData);

	} else {
		retValue =
			PHY_QueryBBReg(Nic, pPhyReg->rfLSSIReadBack | MaskforPhySet,
						   bLSSIReadBackData);

	}

	NDIS_RELEASE_MUTEX(&Nic->dvobjpriv.rf_read_reg_mutex);

	return retValue;

}

u32 Func_Chip_Bb_Calculatebitshift(u32 BitMask)
{
	u32 i;

	for (i = 0; i <= 31; i++) {
		if (((BitMask >> i) & 0x1) == 1)
			break;
	}

	return (i);
}

u32 Func_Of_Proc_Chip_Hw_Read_Rfreg(PNIC Nic, u32 eRFPath, u32 RegAddr,
					   u32 BitMask)
{
	u32 Original_Value, Readback_Value, BitShift;

	Original_Value = Func_Chip_Bb_Rf_Serial_Read_Process(Nic, eRFPath, RegAddr);

	BitShift = Chip_Bb_Calculatebitshift(BitMask);
	Readback_Value = (Original_Value & BitMask) >> BitShift;

	return Readback_Value;
}

static VOID
Func_Chip_Bb_Rfserial_Write_Process(IN PNIC Nic,
						IN u32 eRFPath, IN u32 Offset, IN u32 Data)
{
	u32 DataAndAddr = 0;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);
	BB_REGISTER_DEFINITION_T *pPhyReg = &pHalData->PHYRegDef;
	u32 NewOffset;

	Offset &= 0xff;

	NewOffset = Offset;

	DataAndAddr = ((NewOffset << 20) | (Data & 0x000fffff)) & 0x0fffffff;

	PHY_SetBBReg(Nic, pPhyReg->rf3wireOffset, bMaskDWord, DataAndAddr);
}

void Func_Of_Proc_Chip_Hw_Write_Rfreg(PNIC Nic, u32 eRFPath, u32 RegAddr,
						 u32 BitMask, u32 Data)
{
	u32 Original_Value, BitShift;

	if (BitMask != bRFRegOffsetMask) {
		Original_Value = Func_Chip_Bb_Rf_Serial_Read_Process(Nic, eRFPath, RegAddr);
		BitShift = Chip_Bb_Calculatebitshift(BitMask);
		Data = ((Original_Value & (~BitMask)) | (Data << BitShift));
	}

	Func_Chip_Bb_Rfserial_Write_Process(Nic, eRFPath, RegAddr, Data);
}

void Func_Of_Proc_Chip_Hw_Set_Chnl_Bw(PNIC Nic, u8 channel,
						 CHANNEL_WIDTH Bandwidth, u8 Offset40)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	Func_Chip_Bb_Handle_Sw_Chnl_And_Set_Bw_Process(Nic, TRUE, TRUE, channel, Bandwidth,
								  Offset40, channel);
}

void Func_Of_Proc_Chip_Hw_Dm_Watchdog(PNIC Nic)
{
	if (!is_primary_wadptdata(Nic))
		return;

	Func_Of_Haldmwatchdog(Nic);

}

bool Func_Of_Proc_Chip_Hw_wMBOX0_Valid(PNIC Nic, u8 * buf)
{
	bool ret = FALSE;

	wl_warn_on(1);

	return ret;
}

s32 Func_Of_Proc_Chip_Hw_wMBOX0_Evt_Read(PNIC Nic, u8 * buf)
{
	s32 ret = FALSE;

	wl_warn_on(1);

	return ret;
}

s32 Func_Of_Proc_Chip_Hw_wMBOX0_Handler(PNIC Nic, u8 * buf)
{
	struct wmbox0_evt_hdr_88xx *pC2hEvent = (struct wmbox0_evt_hdr_88xx *)buf;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	s32 ret = TRUE;
	u8 index = 0;

	if (pC2hEvent == NULL) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("%s(): pC2hEventis NULL\n", __func__));
		ret = FALSE;
		goto exit;
	}

	switch (pC2hEvent->id) {
	case wMBOX0_DBG:{
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			("wmbox0_handler_9086x: %s\n", pC2hEvent->payload));
		}
		break;

	case wMBOX0_CCX_TX_RPT:
		break;

	default:
		break;
	}

exit:
	return ret;
}

bool Func_Of_Proc_Chip_Hw_wMBOX0_Id_Filter_Ccx(u8 * buf)
{
	struct wmbox0_evt_hdr_88xx *wmbox0_evt = (struct wmbox0_evt_hdr_88xx *)buf;
	s32 ret = FALSE;
	if (wmbox0_evt->id == wMBOX0_CCX_TX_RPT)
		ret = TRUE;

	return ret;
}

s32 Func_Of_Proc_Chip_Hw_Is_Disable_Sw_Channel_Plan(PNIC Nic)
{
	return GET_HAL_DATA(Nic)->bDisableSWChannelPlan;
}

s32 Func_Of_Proc_Chip_Hw_Macid_Sleep(PNIC Nic, u8 macid)
{
	struct macid_ctl_t *macid_ctl = &Nic->dvobjpriv.macid_ctl;
	u8 support;

	support = FALSE;
	Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_MACID_SLEEP, &support);
	if (FALSE == support)
		return FALSE;

	if (macid >= macid_ctl->num) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			( ": Invalid macid(%u)\n",
				   macid));
		return FALSE;
	}

	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_MACID_SLEEP, &macid);

	return TRUE;
}

s32 Func_Of_Proc_Chip_Hw_Macid_Wakeup(PNIC Nic, u8 macid)
{
	struct macid_ctl_t *macid_ctl = &Nic->dvobjpriv.macid_ctl;
	u8 support;

	support = FALSE;
	Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HAL_DEF_MACID_SLEEP, &support);
	if (FALSE == support)
		return FALSE;

	if (macid >= macid_ctl->num) {
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			( ": Invalid macid(%u)\n",
				   macid));
		return FALSE;
	}

	Func_Of_Proc_Chip_Hw_Set_Hwreg(Nic, HW_VAR_MACID_WAKEUP, &macid);

	return TRUE;
}

s32 Func_Of_Proc_Chip_Hw_Fill_wMBOX1_Cmd(PNIC Nic, u8 ElementID, u32 CmdLen,
						 u8 * pCmdBuffer)
{
//	_wadptdata *pri_wadptdata = GET_PRIMARY_wADPTDATA(Nic);

	if (Nic->bFWReady == TRUE)
		return Func_Mcu_Fill_wMBOX1_Fw(Nic, ElementID, CmdLen, pCmdBuffer);
	else if (Nic->registrypriv.mp_mode == 0)
		MpTrace(COMP_RICHD, DBG_NORMAL, 
			( " FW doesn't exit when no MP mode, by pass wMBOX1 id:0x%02x\n",
				   ElementID));
	return FALSE;
}

/////////////////////////////////////
//tangjian
void Func_Of_Proc_Chip_Hw_Fw_Correct_Bcn(PNIC Nic)
{
//	if (Nic->HalFunc.fw_correct_bcn)
//		Nic->HalFunc.fw_correct_bcn(Nic);
}



