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


#ifndef DH_GROUPS_H
#define DH_GROUPS_H
#include "wpa.h"
#include "common.h"
struct dh_group {
	int id;
	const u8 *generator;
	size_t generator_len;
	const u8 *prime;
	size_t prime_len;
	const u8 *order;
	size_t order_len;
	unsigned int safe_prime:1;
};

const struct dh_group *dh_groups_get(int id);
struct wpabuf *dh_init(const struct dh_group *dh, struct wpabuf **priv);
struct wpabuf *dh_derive_shared(const struct wpabuf *peer_public,
								const struct wpabuf *own_private,
								const struct dh_group *dh);

#endif
