/******************************************************************************
 *
 * Copyright(c) 2013 - 2017 Realtek Corporation. All rights reserved.
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
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#ifndef CONFIG_PLATFORM_OPS
/*
 * Return:
 *	0:	power on successfully
 *	others: power on failed
 */
int platform_wifi_power_on(void)
{
	int ret = 0;


	return ret;
}

void platform_wifi_power_off(void)
{
}

#ifdef CONFIG_RTW_SDIO_OOB_INT
int platform_wifi_get_oob_irq(void)
{
	return 0;
}
#endif /* CONFIG_RTW_SDIO_OOB_INT */

#endif /* !CONFIG_PLATFORM_OPS */
