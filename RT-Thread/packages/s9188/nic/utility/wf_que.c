
/* include */
#include "common.h"

/* macro */
#define QUE_IS_EMPTY(pque)  wf_list_is_empty(wf_que_list_head(pque))
#define QUE_HEAD(pque)      wf_list_next(wf_que_list_head(pque))
#define QUE_TAIL(pque)      wf_list_prev(wf_que_list_head(pque))

/* type */

/* function  */
wf_u32 wf_que_count (wf_que_t *pque)
{
    wf_u32 cnt;

    wf_lock_lock(&pque->lock);
    cnt = pque->cnt;
    wf_lock_unlock(&pque->lock);

    return cnt;
}

wf_bool wf_que_is_empty (wf_que_t *pque)
{
    wf_bool is_empty;

    wf_lock_lock(&pque->lock);
    is_empty = QUE_IS_EMPTY(pque);
    wf_lock_unlock(&pque->lock);

    return is_empty;
}

wf_que_list_t *wf_que_head (wf_que_t *pque)
{
    wf_que_list_t *plist;

    wf_lock_lock(&pque->lock);
    plist = QUE_IS_EMPTY(pque) ? NULL : QUE_HEAD(pque);
    wf_lock_unlock(&pque->lock);

    return plist;
}

wf_que_list_t *wf_que_tail (wf_que_t *pque)
{
    wf_que_list_t *plist;

    wf_lock_lock(&pque->lock);
    plist = QUE_IS_EMPTY(pque) ? NULL : QUE_TAIL(pque);
    wf_lock_unlock(&pque->lock);

    return plist;
}

void wf_enque (wf_que_list_t *pnew, wf_que_list_t *pos, wf_que_t *pque)
{
    wf_lock_lock(&pque->lock);

    wf_list_insert_next(pnew, pos);
    pque->cnt++;

    wf_lock_unlock(&pque->lock);
}

void wf_enque_prev (wf_que_list_t *pnew, wf_que_list_t *pos, wf_que_t *pque)
{
    wf_lock_lock(&pque->lock);

    wf_list_insert_next(pnew, wf_list_prev(pos));
    pque->cnt++;

    wf_lock_unlock(&pque->lock);
}

wf_que_list_t *wf_deque (wf_que_list_t *pos, wf_que_t *pque)
{
    wf_que_list_t *plist;

    wf_lock_lock(&pque->lock);

    if (QUE_IS_EMPTY(pque))
    {
        plist = NULL;
    }
    else
    {
        wf_list_delete(pos);
        pque->cnt--;
        plist = pos;
    }

    wf_lock_unlock(&pque->lock);

    return plist;
}

wf_que_list_t *wf_deque_head (wf_que_t *pque)
{
    wf_que_list_t *plist;

    wf_lock_lock(&pque->lock);

    if (QUE_IS_EMPTY(pque))
    {
        plist = NULL;
    }
    else
    {
        wf_list_delete(plist = QUE_HEAD(pque));
        pque->cnt--;
    }

    wf_lock_unlock(&pque->lock);

    return plist;
}

wf_que_list_t *wf_deque_tail (wf_que_t *pque)
{
    wf_que_list_t *plist;

    wf_lock_lock(&pque->lock);

    if (QUE_IS_EMPTY(pque))
    {
        plist = NULL;
    }
    else
    {
        wf_list_delete(plist = QUE_TAIL(pque));
        pque->cnt--;
    }

    wf_lock_unlock(&pque->lock);

    return plist;
}

void wf_que_init (wf_que_t *pque, wf_lock_type_e lock_type)
{
    wf_list_init(&pque->head);
    wf_lock_init(&pque->lock, lock_type);
    pque->cnt = 0;
}


