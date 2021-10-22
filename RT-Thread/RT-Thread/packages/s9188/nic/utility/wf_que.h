/*
 * wf_que.h
 *
 * This file contains all the prototypes for the wf_que.c file
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __WF_QUE_H__
#define __WF_QUE_H__

typedef wf_list_t wf_que_list_t;
typedef struct
{
    wf_que_list_t head;
    wf_lock_t lock;
    wf_u32 cnt;
} wf_que_t;

wf_u32 wf_que_count (wf_que_t *pque);

wf_inline static wf_que_list_t *wf_que_list_head (wf_que_t *pque)
{
    return &pque->head;
}

wf_bool wf_que_is_empty (wf_que_t *pque);

wf_que_list_t *wf_que_head (wf_que_t *pque);

wf_que_list_t *wf_que_tail (wf_que_t *pque);

void wf_enque (wf_que_list_t *pnew, wf_que_list_t *pos, wf_que_t *pque);

wf_inline static
void wf_enque_next (wf_que_list_t *pnew, wf_que_list_t *pos, wf_que_t *pque)
{
    wf_enque(pnew, pos, pque);
}

void wf_enque_prev (wf_que_list_t *pnew, wf_que_list_t *pos, wf_que_t *pque);

wf_inline static void wf_enque_head (wf_que_list_t *pnew, wf_que_t *pque)
{
    wf_enque_next(pnew, wf_que_list_head(pque), pque);
}

wf_inline static void wf_enque_tail (wf_que_list_t *pnew, wf_que_t *pque)
{
    wf_enque_prev(pnew, wf_que_list_head(pque), pque);
}

wf_que_list_t *wf_deque (wf_que_list_t *pos, wf_que_t *pque);

wf_que_list_t *wf_deque_head (wf_que_t *pque);

wf_que_list_t *wf_deque_tail (wf_que_t *pque);

void wf_que_init (wf_que_t *pque, wf_lock_type_e lock_type);

wf_inline static void wf_que_deinit (wf_que_t *pque)
{
    wf_lock_term(&pque->lock);
}

#endif


