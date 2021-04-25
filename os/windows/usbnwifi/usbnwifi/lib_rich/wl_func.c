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


#include "msg.h"
#include "malloc.h"


////////////////////////////////////////

#define VENDOR_CMD_MAX_DATA_LEN	254

static int usb_setN(PNIC Nic, unsigned int addr, unsigned int length, unsigned char * pdata)
{
	unsigned char request;
	unsigned char requesttype;
	unsigned short wvalue;
	unsigned short index;
	unsigned short len;
	unsigned char buf[VENDOR_CMD_MAX_DATA_LEN] = { 0 };
	int ret;

	request = 0x05;
	requesttype = 0x00;
	index = 0;

	wvalue = (unsigned short)(addr & 0x0000ffff);

	len = (u16)length;
	Func_Of_Proc_Pre_Memcpy(buf, pdata, len);
	ret = HwWriteMacSieRegister(Nic, request, wvalue, len, buf, index);;
	//ret = HwUsbSendVendorControlPacketAsync(Nic, request, wvalue, len,buf, index);
	return ret;

}

#ifdef CONFIG_CONCURRENT_MODE

static int Func_To_Get_Buddy_Fwstate(PNIC Nic)
{

	if (Nic == NULL)
		return WIFI_FW_NO_EXIST;

	if (Nic->pbuddy_Nic== NULL)
		return WIFI_FW_NO_EXIST;

	return Nic->pbuddy_Nic->mlmepriv.fw_state;

}

static int Func_To_Get_Buddy_Mlmestate(PNIC Nic)
{
	if (Nic == NULL)
		return _HW_STATE_NO_EXIST_;

	if (Nic->pbuddy_Nic == NULL)
		return _HW_STATE_NO_EXIST_;

	return Nic->pbuddy_Nic->mlmeextpriv.mlmext_info.state;
}
#endif


void Func_Mcu_Hw_Var_Set_Opmode(PNIC Nic, u8 variable, u8 * val)
{
	UNREFERENCED_PARAMETER(variable);
	u8 mode = *((u8 *) val);
	u32 tmp[5] = { 0 };
	int ret = FALSE;

	tmp[0] = mode;
#ifdef CONFIG_CONCURRENT_MODE
	tmp[1] = Nic->iface_type;
    tmp[2] = Func_To_Get_Buddy_Mlmestate(Nic);
    tmp[3] = Func_To_Get_Buddy_Fwstate(Nic);
#else
	tmp[1] = 0;
    tmp[2] = _HW_STATE_NO_EXIST_;
    tmp[3] = WIFI_FW_NO_EXIST;
#endif

	tmp[4]  = BIT0 | BIT2 | BIT31; 

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HW_SET_OP_MODE, tmp, NULL,
									5, 0);
	if (!ret) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("===>%s,fail!\n", __func__));
	}

}
void Func_Mcu_Hw_Var_Set_Macaddr(PNIC Nic, u8 variable, u8 * val)
{
	UNREFERENCED_PARAMETER(variable);
	u8 idx = 0;

	u32 ret = FALSE;
	u32 var[7] = { 0 };

#ifdef CONFIG_CONCURRENT_MODE
	var[0] = Nic->iface_type;
#else
	var[0] = 0;
#endif
	for (idx = 0; idx < 6; idx++)
		var[idx + 1] = val[idx];

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_SET_MAC, var, NULL, 7,
									0);
	if (!ret) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("==>%s==>Func_Mcu_Universal_Func_Interface fail\n", __func__));
	}
}

void Func_Mcu_Hw_Var_Set_Bssid(PNIC Nic, u8 variable, u8 * val)
{
	UNREFERENCED_PARAMETER(variable);
	u8 idx = 0;
	u32 reg_bssid;
	u32 var[7] = { 0 };
	u32 ret = FALSE;

#ifdef CONFIG_CONCURRENT_MODE
	if (Nic->iface_type == IFACE_PORT1)
		reg_bssid = REG_BSSID1;
	else
#endif
	{
		reg_bssid = REG_BSSID;
	}

	var[0] = reg_bssid;

	for (idx = 0; idx < 6; idx++)
		var[idx + 1] = val[idx];

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_SET_BSSID, var, NULL,
									7, 0);

	if (!ret) {

		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("==>%s==>Func_Mcu_Universal_Func_Interface fail\n", __func__));
		return;
	}
}

void Func_Mcu_Hw_Var_Set_Bcn_Func(PNIC Nic, u8 variable, u8 * val)
{
	UNREFERENCED_PARAMETER(variable);

	u32 var[2] = { 0 };
	int ret = FALSE;

	var[0] = *(u8 *) val;
#ifdef CONFIG_CONCURRENT_MODE
	var[1] = Nic->iface_type;
#else
	var[1] = 0;
#endif

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_SET_BCN, var, NULL, 2,
									0);

	if (!ret) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("==>%s==>Func_Mcu_Universal_Func_Interface fail\n", __func__));

	}
}

void Func_Mcu_Hw_Var_Set_Correct_Tsf(PNIC Nic, u8 variable, u8 * val)
{
	UNREFERENCED_PARAMETER(variable);
	UNREFERENCED_PARAMETER(val);
	u64 tsf;
	int ret = FALSE;
	u32 res[5] = { 0 };

	struct mlme_ext_priv *pmlmeext;
	struct mlme_ext_info *pmlmeinfo;

	pmlmeext = &Nic->mlmeextpriv;
	pmlmeinfo = &pmlmeext->mlmext_info;

	tsf =
		pmlmeext->TSFValue - Func_Of_Proc_Modular64(pmlmeext->TSFValue,
										   (pmlmeinfo->bcn_interval * 1024)) -
		1024;

	res[0] = (u32) tsf;
	res[1] = tsf >> 32;
#ifdef CONFIG_CONCURRENT_MODE
	res[2] = Nic->iface_type;
    res[3] = pmlmeinfo->state;
    res[4] = Func_To_Get_Buddy_Fwstate(Nic);

#else
	res[2] = IFACE_PORT0;
    res[3] = _HW_STATE_NO_EXIST_;
    res[4] = WIFI_FW_NO_EXIST;
#endif

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HW_SET_CORRECT_TSF, res,
									NULL, 5, 0);

	if (!ret) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("===>%s,fail!\n", __func__));

	}

}

void Func_Mcu_Hw_Var_Set_Mlme_Disconnect(PNIC Nic, u8 variable, u8 * val)
{
	UNREFERENCED_PARAMETER(variable);
	UNREFERENCED_PARAMETER(val);
	int ret = FALSE;
	int tmp[2] = { 0 };

#ifdef CONFIG_CONCURRENT_MODE
	tmp[0] = Func_To_Get_Buddy_Mlmestate(Nic);
	tmp[1] = Nic->iface_type;
#else
	tmp[0] = _HW_STATE_NO_EXIST_;
	tmp[1] = IFACE_PORT0;
#endif

	ret = Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HW_SET_MLME_DISCONNECT,
									tmp, NULL, 2, 0);
	if (!ret) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("===>%s,fail!\n", __func__));
	}

}

void Func_Mcu_Hw_Var_Set_Mlme_Sitesurvey(PNIC Nic, u8 variable, u8 * val)
{
	UNREFERENCED_PARAMETER(variable);
	UNREFERENCED_PARAMETER(val);

	u32 value_rcr, rcr_clear_bit, reg_bcn_ctl;
	u16 value_rxfltmap2;
	u8 val8;
	PHAL_DATA_TYPE pHalData;
	struct mlme_priv *pmlmepriv;
	u8 ap_num;

	u32 arg[7] = { 0 };
	u32 res = 0;

	int ret = FALSE;

	u32 buddy_reg_bcn_ctl = 0x00;

	pHalData = GET_HAL_DATA(Nic);
	pmlmepriv = &Nic->mlmepriv;

#if 0 //need change
	preproc_dev_iface_status_func(Nic, NULL, NULL, NULL, &ap_num, NULL);
#else
    ap_num = 1;
#endif

	arg[0] = *val;

#ifdef CONFIG_CONCURRENT_MODE
	arg[1] = Nic->iface_type;
    arg[5] = Func_To_Get_Buddy_Fwstate(Nic);

#else
	arg[1] = IFACE_PORT0;
    arg[5] = WIFI_FW_NO_EXIST;
#endif
    arg[2] = ap_num;
    arg[3] = pmlmepriv->fw_state;
    arg[6] = FALSE;	

    if (do_chk_linked(Nic, 1))
    	arg[4] = TRUE;
    else
    	arg[4] = FALSE;

	if (arg[5] != WIFI_FW_NO_EXIST) {
		if (Nic->pbuddy_Nic)
			arg[6] = do_chk_linked(Nic->pbuddy_Nic, 1);
	}

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HW_SET_MLME_SITE, arg,
									NULL, 7, 0);
	if (!ret) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("===>%s,fial!", __func__));
		return;
	}

	if (*val) {

		if (do_chk_linked(Nic, 1) &&
			check_fwstate(pmlmepriv, WIFI_AP_STATE) != TRUE) {
			Nic->mlmeextpriv.en_hw_update_tsf = FALSE;
		}
#ifdef CONFIG_CONCURRENT_MODE
		if (do_chk_linked(Nic->pbuddy_Nic, 1) &&
			check_fwstate(&Nic->pbuddy_Nic->mlmepriv,
						  WIFI_AP_STATE) != TRUE) {
			Nic->pbuddy_Nic->mlmeextpriv.en_hw_update_tsf = FALSE;
		}
#endif

	} else {
		if (do_chk_linked(Nic, 1) &&
			check_fwstate(pmlmepriv, WIFI_AP_STATE) != TRUE)
			Nic->mlmeextpriv.en_hw_update_tsf = TRUE;

#ifdef CONFIG_CONCURRENT_MODE
		if (do_chk_linked(Nic->pbuddy_Nic, 1) &&
			check_fwstate(&Nic->pbuddy_Nic->mlmepriv,
						  WIFI_AP_STATE) != TRUE)

			Nic->pbuddy_Nic->mlmeextpriv.en_hw_update_tsf = TRUE;
#endif

#if 0     //not finished
		if (ap_num) {
			int i;
			_wadptdata *iface;

			for (i = 0; i < dvobj->iface_nums; i++) {
				iface = dvobj->pwadptdatas[i];
				if (!iface)
					continue;

				if (check_fwstate(&iface->mlmepriv, WIFI_AP_STATE) == TRUE
					&& check_fwstate(&iface->mlmepriv,
									 WIFI_ASOC_STATE) == TRUE) {
					iface->mlmepriv.update_bcn = TRUE;
#ifndef CONFIG_INTERRUPT_BASED_TXBCN
					proc_tx_beacon_func(iface, NULL);
#endif

				}
			}
		}
#endif

	}

}

void Func_Mcu_Hw_Var_Set_Mlme_Join(PNIC Nic, u8 variable, u8 * val)
{
	UNREFERENCED_PARAMETER(variable);
	
	u8 RetryLimit;
	u32 type[5] = { 0 };
	int ret = FALSE;
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Nic);

#ifdef CONFIG_CONCURRENT_MODE
	PwADPTDATA pbuddy_wadptdata;
	struct mlme_ext_priv *pbuddy_mlmeext;
	struct mlme_ext_info *pbuddy_mlmeinfo;
#endif

	RetryLimit = 0x30;

	type[0] = val[0];
#ifdef CONFIG_CONCURRENT_MODE
	type[1] = Nic->iface_type;
    type[3] = Func_To_Get_Buddy_Mlmestate(Nic);
    type[4] = Func_To_Get_Buddy_Fwstate(Nic);

#else
	type[1] = IFACE_PORT0;
    type[3] = _HW_STATE_NO_EXIST_;
    type[4] = WIFI_FW_NO_EXIST;

#endif
	type[2] = Nic->mlmepriv.fw_state;

	MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("type[0] = 0x%x,type[1] = 0x%x\n", type[0], type[1]));

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HW_SET_MLME_JOIN, type,
									NULL, 5, 0);
	if (!ret) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("===>%s,fail!\n", __func__));

	}
}

void Func_Mcu_Hw_Var_Set_wMBOX1_Fw_Pwrmode(PNIC Nic, u8 variable, u8 * val)
{
	UNREFERENCED_PARAMETER(variable);
	u8 psmode = *val;

	if (psmode != PS_MODE_ACTIVE) {
		int ret = FALSE;
		u32 tmp = TRUE;
		ret =
			Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_MSG_RF_SAVE, &tmp,
										NULL, 1, 0);
		if (!ret) {
			MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("===>%s,fail!\n", __func__));
			return;
		}
	}
	Func_Of_Set_Fwpwrmode_Cmd(Nic, psmode);
}

void Func_Mcu_Hw_Var_Set_Hw_Update_Tsf(PNIC Nic)
{
#if 0
	u16 reg_bcn_ctl;
	u32 tmp[2] = { 0 };
	int ret = FALSE;
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;
	struct mlme_priv *pmlmepriv = &Nic->mlmepriv;

	if (!Nic->pmlmeext.en_hw_update_tsf)
		return;

#ifdef CONFIG_CONCURRENT_MODE
	tmp[0] = Nic->iface_type;
#else
	tmp[0] = IFACE_PORT0;
#endif
	if (do_chk_linked(Nic, 1) &&
		check_fwstate(Nic->pmlmepriv, WIFI_AP_STATE) != TRUE)
		tmp[1] = 1;
	else
		tmp[1] = 0;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HW_UPDATE_TSF, tmp, NULL,
									2, 0);

	if (!ret) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("===>%s,fail!\n", __func__));
		return;

	}

	Nic->pmlmeext.en_hw_update_tsf = FALSE;
#endif
}

void Func_Hw_Var_Set_Fwlps_Rfon(PNIC Nic, u8 variable, u8 * val)
{
	UNREFERENCED_PARAMETER(variable);
	u32 valRCR;
	int ret = FALSE;
	u32 value;

	if ((Nic->CardInfo.SurpriseRemoved) ||
		(Nic->pwrctl_priv.rf_pwrstate == rf_off)) {

		*val = TRUE;
	} else {

		valRCR = HwPlatformIORead4Byte(Nic, REG_RCR);

		valRCR &= 0x00070000;
		if (valRCR)
			*val = FALSE;
		else
			*val = TRUE;
	}
}


static int Func_Waitmcu_Intdown(PNIC Nic, u32 func_code, int timeout)
{
	int ret = FALSE;
	int ts = 0;
	if (!timeout || !Nic)
		return FALSE;
	MAILBOX_SET_INTFINISH(Nic);
	MAILBOX_TGIRGER_INT(Nic);
	while (TRUE) {
		if(Nic->CardInfo.SurpriseRemoved == TRUE)
			return ret;

		ret = MAILBOX_GET_INTFINISH(Nic);
		if (ret) {
			break;
		}

		if (++ts >= timeout) {
			break;
		}

		Func_Of_Proc_Msleep_Os(1);

	}
	return ret;
}

#define UMSG_EN_ADDR 0x00e4
#define UMSG_START_ADDR 0x00e8

#define UMSG_INTERRUPT 0x03f0

static void Func_Reset_Mcu(PNIC Nic)
{
	u32 u4Tmp;
	HwPlatformIOWrite4Byte(Nic, UMSG_EN_ADDR, 0x0);
	u4Tmp = HwPlatformIORead4Byte(Nic, 0x0094);
	HwPlatformIOWrite4Byte(Nic, 0x0094, 0x0);
}

static void Func_Clock_Mcu_En(PNIC Nic)
{
	u32 u4Tmp;
	u4Tmp = HwPlatformIORead4Byte(Nic, 0x0094);
	HwPlatformIOWrite4Byte(Nic, 0x0094, 0x6);
}

static void Func_Mcu_En(PNIC Nic)
{
	u32 u4Tmp;
	u4Tmp = HwPlatformIORead4Byte(Nic, UMSG_EN_ADDR);
	HwPlatformIOWrite4Byte(Nic, UMSG_EN_ADDR, 0x1);
}

#define UMSG_LEN   267
#define WR_NUM	4


u32 Func_Mcu_Go(PNIC Nic)
{
	u32 i, j = 0;
	int block_len = 64;
	u8 data[64];
	u32 block;
	int remain;
	u32 fw_addr = UMSG_START_ADDR;
	u32 u4Tmp, umsg_len;
	u32 start, end;


	MpTrace(COMP_RICH_FUNC, DBG_NORMAL,("1\n"));
    Func_Reset_Mcu(Nic);
	MpTrace(COMP_RICH_FUNC, DBG_NORMAL,("2\n"));
    Func_Clock_Mcu_En(Nic);
	MpTrace(COMP_RICH_FUNC, DBG_NORMAL,("3\n"));	
    Func_Mcu_En(Nic);
	MpTrace(COMP_RICH_FUNC, DBG_NORMAL,("4\n"));
	

	umsg_len = sizeof(wl_msg) / sizeof(wl_msg[0]);

	start = Func_Of_Proc_Get_Current_Time();

	block = umsg_len / block_len;
	remain = umsg_len % block_len;
    
    for (i = 0; i < block; i++) {
    	usb_setN(Nic, UMSG_START_ADDR, block_len, &wl_msg[i * block_len]);
    }
    if (remain)
    	usb_setN(Nic, UMSG_START_ADDR, remain, &wl_msg[i * block_len]);
	
    end = Func_Of_Proc_Get_Current_Time();
	MpTrace(COMP_RICH_FUNC, DBG_NORMAL,
		 ("===>passtime:  %d ms\n", Func_Of_Proc_Get_Time_Interval_Ms(start, end)));

	MpTrace(COMP_RICH_FUNC, DBG_NORMAL,("5\n"));
    HwPlatformIOWrite4Byte(Nic, UMSG_EN_ADDR, 0x2);
    HwPlatformIOWrite4Byte(Nic, 0x0094, 0x7);
	MpTrace(COMP_RICH_FUNC, DBG_NORMAL,("6\n"));

	return TRUE;
}


int Func_Mcu_Message_Up(PNIC Nic)
{
    int ret = FALSE;
	
    if (Nic->pwrctl_priv.bFwCurrentInPSMode) {

		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, 
			("discard func %s because ps\r\n", __func__));
    	return 0;
    }
	
	NDIS_WAIT_FOR_MUTEX(&Nic->MsgMutex);

    while (TRUE) {
	
		if(Nic->CardInfo.SurpriseRemoved == TRUE)
    		return ret;
    
    	ret = MAILBOX_GET_INTFINISH(Nic);
    	if (ret) {
    		break;
    	}
    }
    NDIS_RELEASE_MUTEX(&Nic->MsgMutex);

    if (!ret) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL,
			 ("discard func %d because ps\r\n", __func__));
    	return (-1);
    }
    return ret;

}

int Func_Mcu_Send_Buffer_Interface(PNIC Nic, u32 func_code, u32 * send,
							 int send_len, int offs, int total_len)
{
    int ret;
    u32 mailbox_reg_addr = MAILBOX_ARG_START;
	  
    if (Nic->pwrctl_priv.bFwCurrentInPSMode) {
    	MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("discard func %d because ps\r\n", func_code));
    	return 1;
    }

	NDIS_WAIT_FOR_MUTEX(&Nic->MsgMutex);

    HwPlatformIOWrite4Byte(Nic, MAILBOX_REG_FUNC, func_code);
    
    HwPlatformIOWrite4Byte(Nic, mailbox_reg_addr, send_len);
    mailbox_reg_addr += MAILBOX_WORD_LEN;
    
    HwPlatformIOWrite4Byte(Nic, mailbox_reg_addr, offs);
    mailbox_reg_addr += MAILBOX_WORD_LEN;
    
    HwPlatformIOWrite4Byte(Nic, mailbox_reg_addr, total_len);
    mailbox_reg_addr += MAILBOX_WORD_LEN;
    
    while ((send_len--) && send) {
    	HwPlatformIOWrite4Byte(Nic, mailbox_reg_addr, *send++);
    	mailbox_reg_addr += MAILBOX_WORD_LEN;
    }
    
    ret = Func_Waitmcu_Intdown(Nic, func_code, UMSG_NORMAL_INT_MAXTIME);
    
	NDIS_RELEASE_MUTEX(&Nic->MsgMutex);

    return ret;
}

int Func_Mcu_Special_Func_Interface(PNIC Nic, u32 func_code, u32 * rsp,
							  int len, int offs)
{
	int ret;
	u32 mailbox_reg_addr = MAILBOX_ARG_START;

	if (Nic->pwrctl_priv.bFwCurrentInPSMode) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("discard func %d because ps\r\n", func_code));
		return 0;
	}

	if (len > MAILBOX_MAX_RDLEN) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("discard func %d because rd len = %d too big\r\n", func_code,
				len));
		return 0;
	}

	NDIS_WAIT_FOR_MUTEX(&Nic->MsgMutex);

	HwPlatformIOWrite4Byte(Nic, MAILBOX_REG_FUNC, func_code);
	HwPlatformIOWrite4Byte(Nic, mailbox_reg_addr, len);
	mailbox_reg_addr += MAILBOX_WORD_LEN;
	HwPlatformIOWrite4Byte(Nic, mailbox_reg_addr, offs);
	mailbox_reg_addr += MAILBOX_WORD_LEN;
	ret = Func_Waitmcu_Intdown(Nic, func_code, UMSG_NORMAL_INT_MAXTIME);
	mailbox_reg_addr = MAILBOX_ARG_START;
	if (ret) {
		while (len--) {
			*rsp++ = HwPlatformIORead4Byte(Nic, mailbox_reg_addr);
			mailbox_reg_addr += 4;
		}
	}
	
	NDIS_RELEASE_MUTEX(&Nic->MsgMutex);

	return ret;
}


int Func_Mcu_Universal_Func_Interface(PNIC Nic, u32 func_code, u32 * send,
								u32 * recv, int send_len, int recv_len)
{


	int ret;
	UINT32 mailbox_reg_addr = MAILBOX_ARG_START;
	
	if ((send_len > MAILBOX_MAX_TXLEN) || (recv_len > MAILBOX_MAX_RDLEN)) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("[umsg_func_inter err===> send_len:0x%08x,recv_len:0x%08x]\n",
			send_len, recv_len));
		return (-1);
	}
	NDIS_WAIT_FOR_MUTEX(&Nic->MsgMutex);
	HwPlatformIOWrite4Byte(Nic, MAILBOX_REG_FUNC, func_code);
	HwPlatformIOWrite4Byte(Nic, mailbox_reg_addr, send_len);
	mailbox_reg_addr += MAILBOX_WORD_LEN;
	HwPlatformIOWrite4Byte(Nic, mailbox_reg_addr, recv_len);
	mailbox_reg_addr += MAILBOX_WORD_LEN;
#if 0
	if (Nic->pwrctl_priv.bFwCurrentInPSMode && Nic->pwrctl_priv.bMailboxSync == FALSE) {

		if (func_code == UMSG_OPS_HAL_WRITEVAR_MSG ||
			func_code == UMSG_OPS_HAL_READVAR_MSG ||
			func_code == UMSG_OPS_HAL_MSG_WDG) {
			return 0;
		}
	}
#endif

	while ((send_len--) && send) {
		HwPlatformIOWrite4Byte(Nic, mailbox_reg_addr, *send++);
		mailbox_reg_addr += MAILBOX_WORD_LEN;
	}
	ret = Func_Waitmcu_Intdown(Nic, func_code, UMSG_NORMAL_INT_MAXTIME);

	if (!ret)
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("[func ===> %s,func_code=0x%08x]\n", __func__, func_code));

	mailbox_reg_addr = MAILBOX_ARG_START;
	if (ret) {
		while ((recv_len--) && recv) {
			*recv++ = HwPlatformIORead4Byte(Nic, mailbox_reg_addr);
			mailbox_reg_addr += MAILBOX_WORD_LEN;
		}
	}

	NDIS_RELEASE_MUTEX(&Nic->MsgMutex);
	
	return ret;


}

u32 Func_Mcu_Hal_Set_Hwreg(PNIC Nic, u32 variable, u8 * val, int len)
{


	u32 ret = FALSE;
	int i = 0;
	u32 u4Tmp[50]; 

	if (len > MAILBOX_MAX_TXLEN) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("%s %d len = %d\r\n", __func__, __LINE__, len));
		return FALSE;
	}
	/*u4Tmp = (u32 *) Func_Of_Proc_Pre_Malloc((len + 1) * 4);*/

	if (len > 50) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("%s fail,len > 50, len = %d\r\n", __func__, len));
		return FALSE;
	}

	u4Tmp[0] = variable;
	for (i = 0; i < len; ++i)
		u4Tmp[i + 1] = val[i];

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_SET_HWREG, u4Tmp, NULL,
									len + 1, 0);
	if (!ret) {

    	MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("Error: ===>%s fail: code : %d\n", __func__, variable));
		/*Func_Of_Proc_Pre_Mfree((u8 *) u4Tmp,(len + 1) * 4);*/
		return FALSE;
	}

	/*Func_Of_Proc_Pre_Mfree((u8 *) u4Tmp,(len + 1) * 4);*/
	return ret;


}

u32 Func_Mcu_Hal_Get_Hwreg(PNIC Nic, u8 variable, u8 * val, int len)
{

	u32 ret = FALSE;
	int i = 0;
	u32 u4Tmp;
	u32 value;
	u4Tmp = (u32) variable;

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_GET_HWREG, &u4Tmp,
									&value, 1, 1);
	if (!ret) {
    	MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("Error: ===>%s fail: code :UMSG_OPS_HAL_GET_HWREG\n", __func__));
		return FALSE;
	}
	for (i = 0; i < len; i++)
		val[i] = (u8) ((value >> (i * 8)) & 0xff);

	return ret;

}

s32 Func_Mcu_Fill_wMBOX1_Fw(PNIC Nic, u8 ElementID, u32 CmdLen, u8 * pCmdBuffer)
{

	u32 *u4Tmp;
	u32 ret = FALSE;
	u32 i = 0;

#if 0
	if (wl_is_surprise_removed(pwadptdata))
		return FALSE;
#endif

	if (CmdLen + 2 > MAILBOX_MAX_TXLEN) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("%s %d len = %d\r\n", __func__, __LINE__, CmdLen + 2));
		return FALSE;
	}

	u4Tmp = (u32 *) Func_Of_Proc_Pre_Malloc((CmdLen + 2) * 4);

	///////////////////////////////////////////clear 0
	
	if (!u4Tmp) {
		return (-2);
	}

	u4Tmp[0] = ElementID;
	u4Tmp[1] = CmdLen;

	for (i = 0; i < CmdLen; i++)
		u4Tmp[i + 2] = pCmdBuffer[i];

	ret =
		Func_Mcu_Universal_Func_Interface(Nic, UMSG_OPS_HAL_wMBOX1_CMD, u4Tmp, NULL,
									CmdLen + 2, 0);
	if (!ret) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, ("Error: ===>%s fail: code :Func_Mcu_Fill_wMBOX1_Fw\n", __func__));
		Func_Of_Proc_Pre_Mfree((u8 *) u4Tmp,(CmdLen + 2) * 4);
		return (-1);
	}
	if (u4Tmp)
		Func_Of_Proc_Pre_Mfree((u8 *) u4Tmp,(CmdLen + 2) * 4);
	return ret;

}

void Func_Set_Concurrent_Func(PNIC Nic)
{
	u32 tmp;
	tmp = 0;
#ifdef CONFIG_CONCURRENT_MODE
	tmp = 1;
#endif
	MpTrace(COMP_RICH_FUNC, DBG_NORMAL, 
	       ("chuck ===>%s ,tmp = %d\n", __func__, tmp));

	if (!Func_Mcu_Universal_Func_Interface
		(Nic, UMSG_OPS_HAL_CONFIG_CONCURRENT_MODE, &tmp, NULL, 1, 0)) {
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, 
			("==>%s==> fail\n", __func__));
		return;
	}
}











////////////////////////////////
//in mlme.c
sint do_chk_linked(PNIC Nic, u8 tag)
{
   	return TRUE;
}

u16 do_query_capability(WLAN_BSSID_EX * bss)
{
	u16 val;
	_func_enter_;

	Func_Of_Proc_Pre_Memcpy((u8 *) & val, do_query_data_from_ie(bss->IEs, CAPABILITY), 2);

	_func_exit_;
	return le16_to_cpu(val);
}

////////////////////////////////
//in wl_ext.c
static WP_CHANNEL_PLAN_MAP WL_ChannelPlanMap[] = {
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_ETSI1, TXPWR_LMT_ETSI},
	{WL_RD_2G_ETSI1, TXPWR_LMT_ETSI},
	{WL_RD_2G_ETSI1, TXPWR_LMT_ETSI},
	{WL_RD_2G_MKK1, TXPWR_LMT_MKK},
	{WL_RD_2G_MKK1, TXPWR_LMT_MKK},
	{WL_RD_2G_ETSI1, TXPWR_LMT_ETSI},
	{WL_RD_2G_MKK1, TXPWR_LMT_MKK},
	{WL_RD_2G_MKK1, TXPWR_LMT_WW},
	{WL_RD_2G_WORLD, TXPWR_LMT_WW},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_ETSI1, TXPWR_LMT_ETSI},
	{WL_RD_2G_FCC1, TXPWR_LMT_WW},
	{WL_RD_2G_FCC1, TXPWR_LMT_ETSI},
	{WL_RD_2G_FCC1, TXPWR_LMT_ETSI},
	{WL_RD_2G_ETSI1, TXPWR_LMT_MKK},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_ETSI1, TXPWR_LMT_MKK},
	{WL_RD_2G_WORLD, TXPWR_LMT_WW},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_MKK1, TXPWR_LMT_MKK},
	{WL_RD_2G_NULL, TXPWR_LMT_ETSI},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},

	{WL_RD_2G_WORLD, TXPWR_LMT_WW},
	{WL_RD_2G_ETSI1, TXPWR_LMT_ETSI},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_MKK1, TXPWR_LMT_MKK},
	{WL_RD_2G_ETSI2, TXPWR_LMT_ETSI},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_MKK1, TXPWR_LMT_MKK},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_WORLD, TXPWR_LMT_FCC},
	{WL_RD_2G_FCC2, TXPWR_LMT_FCC},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_WORLD, TXPWR_LMT_FCC},
	{WL_RD_2G_WORLD, TXPWR_LMT_FCC},
	{WL_RD_2G_WORLD, TXPWR_LMT_FCC},
	{WL_RD_2G_WORLD, TXPWR_LMT_FCC},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_MKK1, TXPWR_LMT_MKK},
	{WL_RD_2G_MKK1, TXPWR_LMT_MKK},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_GLOBAL, TXPWR_LMT_WW},
	{WL_RD_2G_ETSI1, TXPWR_LMT_ETSI},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
	{WL_RD_2G_MKK2, TXPWR_LMT_MKK},
	{WL_RD_2G_WORLD, TXPWR_LMT_ETSI},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_NULL, TXPWR_LMT_WW},
	{WL_RD_2G_FCC1, TXPWR_LMT_FCC},
};

static WP_CHANNEL_PLAN_MAP WL_CHANNEL_PLAN_MAP_WK_WLAN_DEFINE = {
	WL_RD_2G_WORLD, TXPWR_LMT_FCC
};

bool check_chplan_is_empty_func(u8 id, u8 flag)
{
	WP_CHANNEL_PLAN_MAP *chplan_map;

	if (id == WL_CHPLAN_WK_WLAN_DEFINE)
		chplan_map = &WL_CHANNEL_PLAN_MAP_WK_WLAN_DEFINE;
	else
		chplan_map = &WL_ChannelPlanMap[id];

	if (flag) {
		if (chplan_map->Index2G == WL_RD_2G_NULL)
			return _TRUE;
	}
	return _FALSE;
}


bool check_rx_ampdu_is_accept_func(PNIC Nic, u8 flag)
{
	bool accept;

	if (flag) {
		if (Nic->fix_rx_ampdu_accept != RX_AMPDU_ACCEPT_INVALID) {
			accept = Nic->fix_rx_ampdu_accept;
			goto exit;
		}
	}
	if (!mlmeext_chk_scan_state(&Nic->mlmeextpriv, SCAN_DISABLE)
		&& !mlmeext_chk_scan_state(&Nic->mlmeextpriv, SCAN_COMPLETE)
		&& Nic->mlmeextpriv.sitesurvey_res.rx_ampdu_accept !=
		RX_AMPDU_ACCEPT_INVALID) {
		accept = Nic->mlmeextpriv.sitesurvey_res.rx_ampdu_accept;
		goto exit;
	}

	accept = Nic->mlmeextpriv.mlmext_info.bAcceptAddbaReq;

exit:
	return accept;
}


int hw_mlme_ext_to_init_func(u8 flag, PNIC Nic)
{
	struct mlme_ext_priv *pmlmeext = &Nic->mlmeextpriv;

	if (flag) {
		do_set_chabw(Nic, pmlmeext->cur_channel,
						   pmlmeext->cur_ch_offset, pmlmeext->cur_bwmode);
	}
	return TRUE;
}


void proc_mgntframe_attrib_update_func(PNIC Nic, struct pkt_attrib *pattrib)
{
	u8 wireless_mode;
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);
	struct xmit_priv *pxmitpriv = &Nic->xmitpriv;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &Nic->stapriv;
	struct sta_info *pbcmc_sta = NULL;

//	pbcmc_sta = do_query_bcmc_stainfo(Nic, 1);

	pattrib->hdrlen = 24;
	pattrib->nr_frags = 1;
	pattrib->priority = 7;

	if (pbcmc_sta)
		pattrib->mac_id = pbcmc_sta->mac_id;
	else {
		pattrib->mac_id = 0;
		MpTrace(COMP_RICH_FUNC, DBG_NORMAL, 
			   ("mgmt use mac_id 0 will affect RA\n"));
	}
	pattrib->qsel = QSLT_MGNT;

	pattrib->pktlen = 0;

	if (pmlmeext->tx_rate == IEEE80211_CCK_RATE_1MB)
		wireless_mode = WIRELESS_11B;
	else
		wireless_mode = WIRELESS_11G;
	pattrib->raid =
		(wireless_mode & WIRELESS_11B) ? RATEID_IDX_B : RATEID_IDX_G;
	pattrib->rate = pmlmeext->tx_rate;

	pattrib->encrypt = _NO_PRIVACY_;

	pattrib->qos_en = _FALSE;
	pattrib->ht_en = _FALSE;
	pattrib->bwmode = CHANNEL_WIDTH_20;
	pattrib->ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	pattrib->sgi = _FALSE;

	pattrib->seqnum = pmlmeext->mgnt_seq;

	pattrib->retry_ctrl = _TRUE;

	pattrib->mbssid = 0;
	pattrib->hw_ssn_sel = pxmitpriv->hw_ssn_seq_no;

}


void proc_mgnt_tx_rate_update_func(PNIC Nic, u8 rate)
{
	struct mlme_ext_priv *pmlmeext = &(Nic->mlmeextpriv);

	pmlmeext->tx_rate = rate;
}



u8 proc_rx_ampdu_size_func(PNIC Nic, u8 flag)
{
	u8 size;
	HT_CAP_AMPDU_FACTOR max_rx_ampdu_factor;

	if (Nic->fix_rx_ampdu_size != RX_AMPDU_SIZE_INVALID) {
		size = Nic->fix_rx_ampdu_size;
		goto exit;
	}

	if (!mlmeext_chk_scan_state(&Nic->mlmeextpriv, SCAN_DISABLE)
		&& !mlmeext_chk_scan_state(&Nic->mlmeextpriv, SCAN_COMPLETE)
		&& Nic->mlmeextpriv.sitesurvey_res.rx_ampdu_size !=
		RX_AMPDU_SIZE_INVALID) {
		size = Nic->mlmeextpriv.sitesurvey_res.rx_ampdu_size;
		goto exit;
	}

	if (Nic->driver_rx_ampdu_factor != 0xFF)
		max_rx_ampdu_factor =
			(HT_CAP_AMPDU_FACTOR) Nic->driver_rx_ampdu_factor;
	else
		Func_Of_Proc_Chip_Hw_Get_Def_Var(Nic, HW_VAR_MAX_RX_AMPDU_FACTOR,
							&max_rx_ampdu_factor);
	switch (max_rx_ampdu_factor) {
	case MAX_AMPDU_FACTOR_64K:
		size = 64;
		break;
	case MAX_AMPDU_FACTOR_32K:
		size = 32;
		break;
	case MAX_AMPDU_FACTOR_16K:
		size = 16;
		break;
	case MAX_AMPDU_FACTOR_8K:
		size = 8;
		break;
	default:
		size = 64;
		break;
	}

exit:

	if (flag) {
		if (size > 127)
			size = 127;
	}
	return size;
}

