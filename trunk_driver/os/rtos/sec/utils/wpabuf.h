/*
* Dynamic data buffer
* Copyright (c) 2007-2012, Jouni Malinen <j@w1.fi>
*
* This software may be distributed under the terms of the BSD license.
* See README for more details.
*/

#ifndef WPABUF_H
#define WPABUF_H

/* wpabuf::buf is a pointer to external data */
#define WPABUF_FLAG_EXT_DATA BIT(0)

/*
* Internal data structure for wpabuf. Please do not touch this directly from
* elsewhere. This is only defined in header file to allow inline functions
* from this file to access data.
*/
struct wpabuf
{
    size_t size; /* total size of the allocated buffer */
    size_t used; /* length of data in the buffer */
    wf_u8 *buf; /* pointer to the head of the buffer */
    unsigned int flags;
    /* optionally followed by the allocated buffer */
};


struct wpabuf * wf_wpabuf_alloc(size_t len);
struct wpabuf * wf_wpabuf_alloc_copy(const void *data, size_t len);
void wf_wpabuf_free(struct wpabuf *buf);
void wf_wpabuf_clear_free(struct wpabuf *buf);
void * wf_wpabuf_put(struct wpabuf *buf, size_t len);

/**
* wf_wpabuf_size - Get the currently allocated size of a wpabuf buffer
* @buf: wpabuf buffer
* Returns: Currently allocated size of the buffer
*/
static inline size_t wf_wpabuf_size(const struct wpabuf *buf)
{
    return buf->size;
}

/**
* wf_wpabuf_len - Get the current length of a wpabuf buffer data
* @buf: wpabuf buffer
* Returns: Currently used length of the buffer
*/
static inline size_t wf_wpabuf_len(const struct wpabuf *buf)
{
    return buf->used;
}


/**
* wf_wpabuf_head - Get pointer to the head of the buffer data
* @buf: wpabuf buffer
* Returns: Pointer to the head of the buffer data
*/
static inline const void * wf_wpabuf_head(const struct wpabuf *buf)
{
    return buf->buf;
}

/**
* wf_wpabuf_mhead - Get modifiable pointer to the head of the buffer data
* @buf: wpabuf buffer
* Returns: Pointer to the head of the buffer data
*/
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
    {
        wf_memcpy(wf_wpabuf_put(buf, len), data, len);
    }
}

#endif /* WPABUF_H */
