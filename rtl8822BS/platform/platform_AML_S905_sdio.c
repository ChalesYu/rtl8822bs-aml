/******************************************************************************
 *
 * Copyright(c) 2016 - 2017 Realtek Corporation. All rights reserved.
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
#include <linux/version.h>	/* Linux vresion */
#include <linux/printk.h>       /* printk() */
#include <linux/delay.h>	/* msleep() */

extern void sdio_reinit(void);
extern void extern_wifi_set_enable(int is_on);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
extern void wifi_teardown_dt(void);
extern int wifi_setup_dt(void);
#endif /* kernel < 3.14.0 */

#ifdef CONFIG_RTW_SDIO_OOB_INT
extern int wifi_irq_num(void);
int irq_wlan_oob = 0;
#endif /* CONFIG_RTW_SDIO_OOB_INT */

/*
 * Return:
 *	0:	power on successfully
 *	others: power on failed
 */
int platform_wifi_power_on(void)
{
	int ret = 0;


#ifdef CONFIG_RTW_SDIO_OOB_INT
	irq_wlan_oob = 	wifi_irq_num();
	pr_info("%s: SDIO OOB IRQ(%d)\n", __FUNCTION__, irq_wlan_oob);
	if (irq_wlan_oob <= 0)
		return -1;
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
	ret = wifi_setup_dt();
	if (ret) {
		printk("%s: setup dt failed!!(%d)\n", __func__, ret);
		return -1;
	}
#endif /* kernel < 3.14.0 */

#if 0 /* Seems redundancy? Already done before insert driver */
	printk("######%s: \n",__func__);
	extern_wifi_set_enable(0);
	msleep(500);
	extern_wifi_set_enable(1);
	msleep(500);
	sdio_reinit();
#endif

	return ret;
}

void platform_wifi_power_off(void)
{
#ifdef CONFIG_RTW_SDIO_OOB_INT
	irq_wlan_oob = 0;
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
	wifi_teardown_dt();
#endif /* kernel < 3.14.0 */
}

#ifdef CONFIG_RTW_SDIO_OOB_INT
int platform_wifi_get_oob_irq(void)
{
	int irq = 0;


	irq = irq_wlan_oob;

	return irq;
}
#endif /* CONFIG_RTW_SDIO_OOB_INT */

