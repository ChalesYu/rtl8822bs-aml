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


#ifdef CONFIG_WOWLAN
#include "comm_wmbox1.h"
#endif

/*
    There are 6 HW Power States:
    0: POFF--Power Off
    1: PDN--Power Down
    2: CARDEMU--Card Emulation
    3: ACT--Active Mode
    4: LPS--Low Power State
    5: SUS--Suspend
    
    The transision from different states are defined below
    TRANS_CARDEMU_TO_ACT
    TRANS_ACT_TO_CARDEMU
    TRANS_CARDEMU_TO_SUS
    TRANS_SUS_TO_CARDEMU
    TRANS_CARDEMU_TO_PDN
    TRANS_ACT_TO_LPS
    TRANS_LPS_TO_ACT
*/

u32 Func_Of_Hw_Init(PNIC Nic)
{
	u8 value8 = 0, u1bRegCR;
	int ret = FALSE;
	u32 boundary, status = TRUE;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Nic);

	struct pwrctrl_priv *pwrctrlpriv = &Nic->pwrctl_priv;
	struct registry_priv *pregistrypriv = &Nic->registrypriv;
	wp_rf_power_state eRfPowerStateToSet;
	u32 NavUpper = WiFiNavUpperUs;
	u32 value32;
	u32 mail_box[56] = { 0 };
	u32 mail_box_in;
	u32 is_dw = 0;
	
//	u32 init_start_time = Func_Of_Proc_Get_Current_Time();


#ifdef SimpleTest
	is_dw = 0;
#else
	is_dw = 1;
#endif

	_func_enter_;
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_INIT_STEP0, &is_dw,
									NULL, 1, 0);
	
#ifndef SimpleTest
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,("h2m is err! \n"));
		Nic->bFWReady = _FALSE;
		pHalData->fw_ractrl = _FALSE;

	} else {
		MpTrace(COMP_RICHD, DBG_NORMAL,("h2m is run! \n"));
		Nic->bFWReady = _TRUE;
		pHalData->fw_ractrl = _TRUE;
	}
#endif

	if(pwrctrlpriv->reg_rfoff == _TRUE)
		pwrctrlpriv->rf_pwrstate = rf_off;

	pHalData->PHYRegDef.rfintfs = 0x870;

	pHalData->PHYRegDef.rfintfo = 0x860;

	pHalData->PHYRegDef.rfintfe = 0x860;

	pHalData->PHYRegDef.rf3wireOffset = 0x840;
	pHalData->PHYRegDef.rfHSSIPara2 = 0x824;
	pHalData->PHYRegDef.rfLSSIReadBack = 0x8a0;
	pHalData->PHYRegDef.rfLSSIReadBackPi = 0x8b8;

	mail_box[0] = 0;
	mail_box_in = Nic->mlmepriv.fw_state;
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_FW_INIT, &mail_box_in,
									mail_box, 1, 9);

	if (ret) {
		if (mail_box[0] == _TRUE) {
			pHalData->ReceiveConfig = mail_box[1];
			//pHalData->RfRegChnlVal[0] = mail_box[8];
			MpTrace(COMP_RICHD, DBG_NORMAL,("msg UMSG_OPS_HAL_FW_INIT is OK!\n \n"));
		} else {
			return FALSE;
		}

	} else {
		MpTrace(COMP_RICHD, DBG_NORMAL,("msg UMSG_OPS_HAL_FW_INIT is fail!\n \n"));
	}


	pHalData->bSetPowLevel=_TRUE;
	Func_Of_Proc_Chip_Hw_Set_Chnl_Bw(Nic, Nic->registrypriv.channel,
						CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE);
	

	invalidate_cam_all(Nic, 1);

	HwPlatformIOWrite1Byte(Nic, REG_HWSEQ_CTRL, 0xFF);

	HwPlatformIOWrite4Byte(Nic, REG_BAR_MODE_CTRL, 0x0201ffff);

	Func_Init_Hal_Dm(Nic);

	Func_Wf_Config_Xmit(Nic, WF_XMIT_AGG_MAXNUMS, 0x1F);
#ifdef FPGA_TEST
	Func_Wf_Config_Xmit(Nic, WF_XMIT_OFFSET, 36);
#else
	Func_Wf_Config_Xmit(Nic, WF_XMIT_OFFSET, 40);
#endif
	Func_Wf_Config_Xmit(Nic, WF_XMIT_PKT_OFFSET, 0);

	{
		pwrctrlpriv->rf_pwrstate = rf_on;

		if (pwrctrlpriv->rf_pwrstate == rf_on) {
			struct pwrctrl_priv *pwrpriv;
			u32 start_time;
			u8 restore_iqk_rst;
			u8 wmbox1CmdBuf;

			pwrpriv = &Nic->pwrctl_priv;

			Func_Chip_Bb_Lccalibrate_Process(Nic);

			restore_iqk_rst =
				(pwrpriv->bips_processing == _TRUE) ? _TRUE : _FALSE;
			Func_Chip_Bb_Iqcalibrate_Process(Nic, _FALSE, restore_iqk_rst);
			pHalData->bIQKInitialized = _TRUE;
		}
	}
	
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_UPDATE_TX_FIFO, NULL,
									NULL, 0, 0);
	if (!ret) {
		MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_HAL_UPDATE_TX_FIFO!!!\n",
				__func__));
		goto exit;
	}
	
	MpTrace(COMP_RICHD, DBG_NORMAL,("UMSG_OPS_HAL_UPDATE_TX_FIFO OK!!!\n"));

#ifdef RENEW_EEPROM_THERMAL
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_UPDATE_THERMAL, NULL,
									NULL, 0, 0);
	if (!ret) {
    	MpTrace(COMP_RICHD, DBG_NORMAL,
			("Error :===>%s,fail,code :UMSG_OPS_HAL_UPDATE_THERMAL!!!\n",
				__func__));
		goto exit;
	}
#endif

exit:

	_func_exit_;

	return status;
}

static VOID Func_Set_Usb_Suspend(IN PNIC Nic)
{
	u32 value32;

	value32 = HwPlatformIORead4Byte(Nic, REG_APS_FSMCO);

	value32 |= AFSM_HSUS;
	HwPlatformIOWrite4Byte(Nic, REG_APS_FSMCO, value32);

}

static void Func_Of_Hw_Power_Down(PNIC Nic)
{
}

u32 Func_Of_Hw_Deinit(PNIC Nic)
{
	struct pwrctrl_priv *pwrctl = &Nic->pwrctl_priv;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);
	int ret = FALSE;
	MpTrace(COMP_RICHD, DBG_NORMAL,("==> %s\n", __func__));
	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_DEINIT, NULL, NULL, 0,
									0);
	if (!ret) {
    	MpTrace(COMP_RICHD, DBG_NORMAL, ("Error :===>%s,fail,code :UMSG_OPS_HAL_DEINIT!!!\n", __func__));
		return FALSE;
	}

#ifdef CONFIG_MP_INCLUDED
	if (Adapter->registrypriv.mp_mode == 1)
		MPT_DeInitAdapter(Nic);
#endif

	{
		if (wl_is_hw_init_completed(Nic)
			&& Nic->registrypriv.mp_mode != 1) {
			Func_Of_Proc_Chip_Hw_Power_Off(Nic);

			if ((pwrctl->bHWPwrPindetect) && (pwrctl->bHWPowerdown))
				Func_Of_Hw_Power_Down(Nic);
		}
		pHalData->bMacPwrCtrlOn = _FALSE;
	}
	return TRUE;
}
