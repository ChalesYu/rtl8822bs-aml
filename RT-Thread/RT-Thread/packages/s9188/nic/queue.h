/*
 * queue.h
 *
 * used for .....
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#ifndef __QUEUE_H__
#define __QUEUE_H__
#if 1

typedef struct _que_entry_t {
	struct _que_entry_t *pnext;
	struct _que_entry_t *pprev;
} que_entry_t, *p_que_entry_t;

typedef struct _que_t {
	p_que_entry_t phead;
	p_que_entry_t ptail;
	unsigned int num_elem;
} que_t, *p_que_t;


#define queue_initialize(pqueue) \
	{ \
	    (pqueue)->phead = (p_que_entry_t)NULL; \
	    (pqueue)->ptail = (p_que_entry_t)NULL; \
	    (pqueue)->num_elem = 0; \
	}

#define queue_is_empty(pqueue)             (((p_que_t)(pqueue))->phead == (p_que_entry_t)NULL)

#define queue_is_not_empty(pqueue)         ((pqueue)->num_elem > 0)

#define queue_get_head(pqueue)             ((pqueue)->phead)

#define queue_get_tail(pqueue)             ((pqueue)->ptail)

#define queue_get_next_entry(pque_entry)  ((pque_entry)->pnext)

#define queue_insert_head(pqueue, pque_entry) \
	{ \
	    WF_ASSERT(pqueue); \
	    WF_ASSERT(pque_entry); \
	    (pque_entry)->pnext = (pqueue)->phead; \
	    (pqueue)->phead = (pque_entry); \
	    if ((pqueue)->ptail == (p_que_entry_t)NULL) { \
		(pqueue)->ptail = (pque_entry); \
	    } \
	    ((pqueue)->num_elem)++; \
	}

#define queue_insert_tail(pqueue, pque_entry) \
	{ \
	    WF_ASSERT(pqueue); \
	    WF_ASSERT(pque_entry); \
	    (pque_entry)->pnext = (p_que_entry_t)NULL; \
	    if ((pqueue)->ptail) { \
		((pqueue)->ptail)->pnext = (pque_entry); \
	    } else { \
		(pqueue)->phead = (pque_entry); \
	    } \
	    (pqueue)->ptail = (pque_entry); \
	    ((pqueue)->num_elem)++; \
	}

#define queue_remove_head(pqueue, pque_entry, _p_type) \
	{ \
		WF_ASSERT(pqueue); \
		pque_entry = (_p_type)((pqueue)->phead); \
		if (pque_entry) { \
			(pqueue)->phead = ((p_que_entry_t)(pque_entry))->pnext; \
			if ((pqueue)->phead == (p_que_entry_t)NULL) { \
				(pqueue)->ptail = (p_que_entry_t)NULL; \
			} \
			((p_que_entry_t)(pque_entry))->pnext = (p_que_entry_t)NULL; \
			((pqueue)->num_elem)--; \
		} \
	}

#define queue_move_all(pdest_que, psrc_que) \
	{ \
		WF_ASSERT(pdest_que); \
		WF_ASSERT(psrc_que); \
	    *(p_que_t)pdest_que = *(p_que_t)psrc_que; \
	    queue_initialize(psrc_que); \
	}

#define queue_concatenate_queues(pdest_que, psrc_que) \
	{ \
	    WF_ASSERT(pdest_que); \
	    WF_ASSERT(psrc_que); \
		if (psrc_que->num_elem > 0) { \
			if ((pdest_que)->ptail) { \
				((pdest_que)->ptail)->pnext = (psrc_que)->phead; \
			} else { \
				(pdest_que)->phead = (psrc_que)->phead; \
			} \
			(pdest_que)->ptail = (psrc_que)->ptail; \
			((pdest_que)->num_elem) += ((psrc_que)->num_elem); \
			queue_initialize(psrc_que); \
	    } \
	}

#define queue_concatenate_queues_head(pdest_que, psrc_que) \
	{ \
		WF_ASSERT(pdest_que); \
		WF_ASSERT(psrc_que); \
		if (psrc_que->num_elem > 0) { \
			((psrc_que)->ptail)->pnext = (pdest_que)->phead; \
			(pdest_que)->phead = (psrc_que)->phead; \
			((pdest_que)->num_elem) += ((psrc_que)->num_elem); \
			if ((pdest_que)->ptail == NULL) {                 \
				(pdest_que)->ptail = (psrc_que)->ptail;      \
			}  \
			queue_initialize(psrc_que); \
		} \
	}

#endif
#endif
