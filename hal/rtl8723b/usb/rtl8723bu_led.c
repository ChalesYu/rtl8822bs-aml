/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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
 *****************************************************************************/
/* #include "drv_types.h" */
#include "rtl8723b_hal.h"
#ifdef CONFIG_RTW_SW_LED

/* ********************************************************************************
 * LED object.
 * ******************************************************************************** */


/* ********************************************************************************
 *	Prototype of protected function.
 * ******************************************************************************** */

/* ********************************************************************************
 * LED_819xUsb routines.
 * ******************************************************************************** */

/*
 *	Description:
 *		Turn on LED according to LedPin specified.
 *   */
void
SwLedOn_8723BU(
	_adapter			*padapter,
	PLED_USB		pLed
)
{
	u8	LedCfg;
	/* HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter); */

	if (RTW_CANNOT_RUN(padapter))
		return;

	if (RT_GetInterfaceSelection(padapter) == INTF_SEL2_MINICARD ||
	    RT_GetInterfaceSelection(padapter) == INTF_SEL3_USB_Solo ||
	    RT_GetInterfaceSelection(padapter) == INTF_SEL4_USB_Combo) {
		LedCfg = rtw_read8(padapter, REG_LEDCFG2);
		switch (pLed->LedPin) {
		case LED_PIN_GPIO0:
			break;

		case LED_PIN_LED0:

			LedCfg = rtw_read8(padapter, REG_LEDCFG2);
			rtw_write8(padapter, REG_LEDCFG2, (LedCfg & 0xf0) | BIT5 | BIT6); /* SW control led0 on. */
			break;

		case LED_PIN_LED1:
			rtw_write8(padapter, REG_LEDCFG2, (LedCfg & 0x0f) | BIT5); /* SW control led1 on. */
			break;

		default:
			break;

		}
	} else {
		switch (pLed->LedPin) {
		case LED_PIN_GPIO0:
			break;

		case LED_PIN_LED0:
		case LED_PIN_LED1:
		case LED_PIN_LED2:
			LedCfg = rtw_read8(padapter, REG_LEDCFG2) & 0x20;
			rtw_write8(padapter, REG_LEDCFG2, (LedCfg & ~(BIT3)) | BIT5); /* SW control led2 on. */

			break;

		default:
			break;
		}
	}
	pLed->bLedOn = _TRUE;

}


/*
 *	Description:
 *		Turn off LED according to LedPin specified.
 *   */
void
SwLedOff_8723BU(
	_adapter			*padapter,
	PLED_USB		pLed
)
{
	u8	LedCfg;
	/* HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter); */
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

	if (RTW_CANNOT_RUN(padapter))
		//goto exit;
		return;

//exit:
	if (RT_GetInterfaceSelection(padapter) == INTF_SEL2_MINICARD ||
	    RT_GetInterfaceSelection(padapter) == INTF_SEL3_USB_Solo ||
	    RT_GetInterfaceSelection(padapter) == INTF_SEL4_USB_Combo) {
		LedCfg = rtw_read8(padapter, REG_LEDCFG2);
		switch (pLed->LedPin) {
		case LED_PIN_GPIO0:
			break;

		case LED_PIN_LED0:
			if (pHalData->bLedOpenDrain == _TRUE) {
				LedCfg &= 0x90; /* Set to software control. */
				rtw_write8(padapter, REG_LEDCFG2, (LedCfg | BIT3));
				LedCfg = rtw_read8(padapter, REG_MAC_PINMUX_CFG);
				LedCfg &= 0xFE;
				rtw_write8(padapter, REG_MAC_PINMUX_CFG, LedCfg);
			} else
				rtw_write8(padapter, REG_LEDCFG2, (LedCfg | BIT3 | BIT5 | BIT6));
			break;

		case LED_PIN_LED1:
			LedCfg &= 0x0f; /* Set to software control. */
			rtw_write8(padapter, REG_LEDCFG2, (LedCfg | BIT3));
			break;

		default:
			break;

		}
	} else {
		switch (pLed->LedPin) {
		case LED_PIN_GPIO0:
			break;

		case LED_PIN_LED0:
		case LED_PIN_LED1:
		case LED_PIN_LED2:
			LedCfg = rtw_read8(padapter, REG_LEDCFG2);
			LedCfg &= 0x20; /* Set to software control. */
			rtw_write8(padapter, REG_LEDCFG2, (LedCfg | BIT3 | BIT5));

			break;

		default:
			break;
		}
	}
	pLed->bLedOn = _FALSE;

}

/* ********************************************************************************
 * Interface to manipulate LED objects.
 * ******************************************************************************** */

/* ********************************************************************************
 * Default LED behavior.
 * ******************************************************************************** */

/*
 *	Description:
 *		Initialize all LED_871x objects.
 *   */
void
rtl8723bu_InitSwLeds(
	_adapter	*padapter
)
{
	struct led_priv *pledpriv = adapter_to_led(padapter);

	pledpriv->LedControlHandler = LedControlUSB;

	pledpriv->SwLedOn = SwLedOn_8723BU;
	pledpriv->SwLedOff = SwLedOff_8723BU;

	InitLed(padapter, &(pledpriv->SwLed0), LED_PIN_LED0);
	InitLed(padapter, &(pledpriv->SwLed1), LED_PIN_LED1);
	InitLed(padapter, &(pledpriv->SwLed2), LED_PIN_LED2);
}


/*
 *	Description:
 *		DeInitialize all LED_819xUsb objects.
 *   */
void
rtl8723bu_DeInitSwLeds(
	_adapter	*padapter
)
{
	struct led_priv	*ledpriv = adapter_to_led(padapter);

	DeInitLed(&(ledpriv->SwLed0));
	DeInitLed(&(ledpriv->SwLed1));
}
#endif
