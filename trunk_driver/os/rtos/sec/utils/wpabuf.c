/*
* Dynamic data buffer
* Copyright (c) 2007-2012, Jouni Malinen <j@w1.fi>
*
* This software may be distributed under the terms of the BSD license.
* See README for more details.
*/

#include "common.h"
//#include "trace.h"
#include "wpabuf.h"
#include "sec/wpa.h"
#ifdef WPA_TRACE
#define WPABUF_MAGIC 0x51a974e3

struct wpabuf_trace
{
    unsigned int magic;
} __attribute__((aligned(8)));

static struct wpabuf_trace * wf_wpabuf_get_trace(const struct wpabuf *buf)
{
    return (struct wpabuf_trace *)
           ((const wf_u8 *) buf - sizeof(struct wpabuf_trace));
}
#endif /* WPA_TRACE */


/**
* wf_wpabuf_alloc - Allocate a wpabuf of the given size
* @len: Length for the allocated buffer
* Returns: Buffer to the allocated wpabuf or %NULL on failure
*/
struct wpabuf * wf_wpabuf_alloc(size_t len)
{
#ifdef WPA_TRACE
    struct wpabuf_trace *trace = os_zalloc(sizeof(struct wpabuf_trace) +
                                           sizeof(struct wpabuf) + len);
    struct wpabuf *buf;
    if (trace == NULL)
        return NULL;
    trace->magic = WPABUF_MAGIC;
    buf = (struct wpabuf *) (trace + 1);
#else /* WPA_TRACE */
    struct wpabuf *buf = os_zalloc(sizeof(struct wpabuf) + len);
    if (buf == NULL)
        return NULL;
#endif /* WPA_TRACE */

    buf->size = len;
    buf->buf = (wf_u8 *) (buf + 1);
    return buf;
}


struct wpabuf * wf_wpabuf_alloc_copy(const void *data, size_t len)
{
    struct wpabuf *buf = wf_wpabuf_alloc(len);
    if (buf)
        wf_wpabuf_put_data(buf, data, len);
    return buf;
}


/**
* wf_wpabuf_free - Free a wpabuf
* @buf: wpabuf buffer
*/
void wf_wpabuf_free(struct wpabuf *buf)
{
#ifdef WPA_TRACE
    struct wpabuf_trace *trace;
    if (buf == NULL)
        return;
    trace = wf_wpabuf_get_trace(buf);
    if (trace->magic != WPABUF_MAGIC)
    {
        wpa_printf(MSG_ERROR, "wf_wpabuf_free: invalid magic %x",
                   trace->magic);
        wpa_trace_show("wf_wpabuf_free magic mismatch");
        abort();
    }
    if (buf->flags & WPABUF_FLAG_EXT_DATA)
        os_free(buf->buf);
    os_free(trace);
#else /* WPA_TRACE */
    if (buf == NULL)
        return;
    if (buf->flags & WPABUF_FLAG_EXT_DATA)
        os_free(buf->buf);
    os_free(buf);
#endif /* WPA_TRACE */
}


void wf_wpabuf_clear_free(struct wpabuf *buf)
{
    if (buf)
    {
        os_memset(wf_wpabuf_mhead(buf), 0, wf_wpabuf_len(buf));
        wf_wpabuf_free(buf);
    }
}


void * wf_wpabuf_put(struct wpabuf *buf, size_t len)
{
    void *tmp = wf_wpabuf_mhead_u8(buf) + wf_wpabuf_len(buf);
    buf->used += len;
    //if (buf->used > buf->size) {
    //wf_wpabuf_overflow(buf, len);
    //}
    return tmp;
}



