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


#ifndef DH_GROUP5_H
#define DH_GROUP5_H

void *dh5_init(struct wpabuf **priv, struct wpabuf **publ);
void *dh5_init_fixed(const struct wpabuf *priv, const struct wpabuf *publ);
struct wpabuf *dh5_derive_shared(void *ctx, const struct wpabuf *peer_public,
								 const struct wpabuf *own_private);
void dh5_free(void *ctx);

#endif
