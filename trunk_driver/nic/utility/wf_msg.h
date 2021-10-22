/*
 * wf_msg.h
 *
 * This file contains all the prototypes for the wf_msg.c file
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#ifndef __WF_MSG_H__
#define __WF_MSG_H__

/* macro */

/* |-------------tag--------------|
   |-domain-|--priority--|---id---|
   |b15  b12|b11       b7|b6    b0| */
#define WF_MSG_TAG_DOM_OFS      12
#define WF_MSG_TAG_DOM_MSK      0xF
#define WF_MSG_TAG_PRI_OFS      7
#define WF_MSG_TAG_PRI_MSK      0x1F
#define WF_MSG_TAG_ID_OFS       0
#define WF_MSG_TAG_ID_MSK       0x7F
#define WF_MSG_TAG_SET(dom, pri, id) \
    ((((dom) & WF_MSG_TAG_DOM_MSK) << WF_MSG_TAG_DOM_OFS) | \
     (((pri) & WF_MSG_TAG_PRI_MSK) << WF_MSG_TAG_PRI_OFS) | \
     (((id)  & WF_MSG_TAG_ID_MSK)  << WF_MSG_TAG_ID_OFS))
#define WF_MSG_TAG_DOM(tag) \
    ((wf_msg_tag_dom_t)(((tag) >> WF_MSG_TAG_DOM_OFS) & WF_MSG_TAG_DOM_MSK))
#define WF_MSG_TAG_PRI(tag) \
    ((wf_msg_tag_pri_t)(((tag) >> WF_MSG_TAG_PRI_OFS) & WF_MSG_TAG_PRI_MSK))
#define WF_MSG_TAG_ID(tag) \
    ((wf_msg_tag_id_t)(((tag) >> WF_MSG_TAG_ID_OFS) & WF_MSG_TAG_ID_MSK))

/* type */
typedef struct
{
    wf_lock_t lock;
    wf_que_t pend;
    wf_que_t free;
} wf_msg_que_t;

typedef wf_u16 wf_msg_tag_t;
typedef wf_u8  wf_msg_tag_dom_t;
typedef wf_u8  wf_msg_tag_pri_t;
typedef wf_u8  wf_msg_tag_id_t;

typedef struct
{
    wf_que_list_t list;
    wf_que_t *pque;
    wf_u32 alloc_value_size;
    wf_msg_tag_t tag;
    wf_u32 len;
    wf_u8 value[0];
} wf_msg_t;

/* function */

wf_inline static wf_u32 wf_msg_count (wf_msg_que_t *pmsg_que)
{
    return pmsg_que->pend.cnt;
}

wf_inline static wf_bool wf_msg_is_empty (wf_msg_que_t *pmsg_que)
{
    return wf_que_is_empty(&pmsg_que->pend);
}

int wf_msg_new (wf_msg_que_t *pmsg_que, wf_msg_tag_t tag, wf_msg_t **pnew_msg);
int wf_msg_push (wf_msg_que_t *pmsg_que, wf_msg_t *pmsg);
int wf_msg_push_head (wf_msg_que_t *pmsg_que, wf_msg_t *pmsg);
int msg_get (wf_msg_que_t *pmsg_que, wf_msg_t **pmsg,
             wf_bool bpop, wf_bool btail);

wf_inline static int wf_msg_get (wf_msg_que_t *pmsg_que, wf_msg_t **pmsg)
{
    return msg_get(pmsg_que, pmsg, wf_false, wf_false);
}

wf_inline static int wf_msg_get_tail (wf_msg_que_t *pmsg_que, wf_msg_t **pmsg)
{
    return msg_get(pmsg_que, pmsg, wf_false, wf_true);
}

wf_inline static int wf_msg_pop (wf_msg_que_t *pmsg_que, wf_msg_t **pmsg)
{
    return msg_get(pmsg_que, pmsg, wf_true, wf_false);
}

wf_inline static int wf_msg_pop_tail (wf_msg_que_t *pmsg_que, wf_msg_t **pmsg)
{
    return msg_get(pmsg_que, pmsg, wf_true, wf_true);
}

int msg_get_dom (wf_msg_que_t *pmsg_que, wf_msg_tag_dom_t dom, wf_msg_t **pmsg,
                 wf_bool bpop, wf_bool btail);

wf_inline static
int wf_msg_get_dom (wf_msg_que_t *pmsg_que,
                    wf_msg_tag_dom_t dom, wf_msg_t **pmsg)
{
    return msg_get_dom(pmsg_que, dom, pmsg, wf_false, wf_false);
}

wf_inline static
int wf_msg_get_dom_tail (wf_msg_que_t *pmsg_que,
                         wf_msg_tag_dom_t dom, wf_msg_t **pmsg)
{
    return msg_get_dom(pmsg_que, dom, pmsg, wf_false, wf_true);
}

wf_inline static
int wf_msg_pop_dom (wf_msg_que_t *pmsg_que,
                    wf_msg_tag_dom_t dom, wf_msg_t **pmsg)
{
    return msg_get_dom(pmsg_que, dom, pmsg, wf_true, wf_false);
}

wf_inline static
int wf_msg_pop_dom_tail (wf_msg_que_t *pmsg_que,
                         wf_msg_tag_dom_t dom, wf_msg_t **pmsg)
{
    return msg_get_dom(pmsg_que, dom, pmsg, wf_true, wf_true);
}

int wf_msg_del (wf_msg_que_t *pmsg_que, wf_msg_t *pmsg);
int wf_msg_alloc (wf_msg_que_t *pmsg_que,
                  wf_msg_tag_t tag, wf_u32 size, wf_u8 num);
int wf_msg_free (wf_msg_que_t *pmsg_que);

wf_inline static void wf_msg_init (wf_msg_que_t *pmsg_que)
{
    wf_lock_init(&pmsg_que->lock, WF_LOCK_TYPE_IRQ);
    wf_que_init(&pmsg_que->pend, WF_LOCK_TYPE_NONE);
    wf_que_init(&pmsg_que->free, WF_LOCK_TYPE_NONE);
}

wf_inline static void wf_msg_deinit (wf_msg_que_t *pmsg_que)
{
    wf_msg_free(pmsg_que);
    wf_lock_term(&pmsg_que->lock);
    wf_que_deinit(&pmsg_que->pend);
    wf_que_deinit(&pmsg_que->free);
}

#endif

