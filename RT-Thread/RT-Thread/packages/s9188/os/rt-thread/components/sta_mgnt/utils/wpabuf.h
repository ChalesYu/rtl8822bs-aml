/*
* Dynamic data buffer
* Copyright (c) 2007-2012, Jouni Malinen <j@w1.fi>
*
* This software may be distributed under the terms of the BSD license.
* See README for more details.
*/

#ifndef WPABUF_H
#define WPABUF_H

#define WPABUF_FLAG_EXT_DATA BIT(0)

struct wpabuf {
  size_t size;
  size_t used;
  wf_u8 *buf;
  unsigned int flags;
};


struct wpabuf * wf_wpabuf_alloc(size_t len);
struct wpabuf * wf_wpabuf_alloc_copy(const void *data, size_t len);
void wf_wpabuf_free(struct wpabuf *buf);
void wf_wpabuf_clear_free(struct wpabuf *buf);
void * wf_wpabuf_put(struct wpabuf *buf, size_t len);

static inline size_t wf_wpabuf_size(const struct wpabuf *buf)
{
  return buf->size;
}

static inline size_t wf_wpabuf_len(const struct wpabuf *buf)
{
  return buf->used;
}

static inline const void * wf_wpabuf_head(const struct wpabuf *buf)
{
  return buf->buf;
}

static inline void * wf_wpabuf_mhead(struct wpabuf *buf)
{
  return buf->buf;
}

static inline wf_u8 * wf_wpabuf_mhead_u8(struct wpabuf *buf)
{
  return (wf_u8 *) wf_wpabuf_mhead(buf);
}

static inline void wf_wpabuf_put_data(struct wpabuf *buf, const void *data,
                                      size_t len)
{
  if (data)
    memcpy(wf_wpabuf_put(buf, len), data, len);
}

#endif
