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

#ifndef _WL_EVENT_H_
#define _WL_EVENT_H_

struct survey_event {
	WLAN_BSSID_EX bss;
};

struct surveydone_event {
	unsigned int bss_cnt;

};

struct joinbss_event {
	struct wlan_network network;
};

struct stassoc_event {
	unsigned char macaddr[6];
};

struct stadel_event {
	unsigned char macaddr[6];
	unsigned char rsvd[2];
	unsigned char locally_generated;
	int mac_id;
};

struct wmm_event {
	unsigned char wmm;
};

#define GEN_EVT_CODE(event)	event ## _EVT_

struct fwevent {
	u32 parmsize;
	void (*event_callback) (PNIC Nic, u8 * pbuf);
};

#endif
