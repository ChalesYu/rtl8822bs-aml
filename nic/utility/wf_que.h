
#ifndef __WF_QUE_H__
#define __WF_QUE_H__

/* include */

/* macro */

/* type */
typedef wf_list_t wf_que_list_t;
typedef struct
{
    wf_que_list_t head;
    wf_lock_t lock;
    wf_u32 cnt;
} wf_que_t;

/* function declaration */
wf_inline static wf_bool wf_que_is_empty (wf_que_t *pque)
{
    return wf_list_is_empty(&pque->head);
}

wf_inline static wf_que_list_t *wf_que_head (wf_que_t *pque)
{
    return &pque->head;
}

void wf_enque (wf_list_t *plist, wf_list_t *pos, wf_que_t *pque);

wf_inline static void wf_enque_head (wf_que_list_t *plist, wf_que_t *pque)
{
    wf_enque(plist, &pque->head, pque);
}

wf_inline static void wf_enque_tail (wf_que_list_t *plist, wf_que_t *pque)
{
    wf_enque(plist, wf_list_prev(&pque->head), pque);
}

wf_list_t *wf_deque (wf_list_t *plist, wf_que_t *pque);

wf_inline static wf_que_list_t *wf_deque_head (wf_que_t *pque)
{
    return wf_deque(wf_list_next(&pque->head), pque);
}

wf_inline static wf_que_list_t *wf_deque_tail (wf_que_t *pque)
{
    return wf_deque(wf_list_prev(&pque->head), pque);
}

void wf_que_init (wf_que_t *pque, wf_lock_type_e lock_type);

wf_inline static void wf_que_deinit (wf_que_t *pque)
{
    wf_lock_term(&pque->lock);
}

#endif /* __WF_QUE_H__ */

