
/* include */
#include "common.h"

/* macro */

/* type */

/* function  */
void wf_enque (wf_list_t *plist, wf_list_t *pos, wf_que_t *pque)
{
    wf_lock_lock(&pque->lock);

    wf_list_insert_head(plist, pos);
    pque->cnt++;

    wf_lock_unlock(&pque->lock);
}

wf_list_t *wf_deque (wf_list_t *plist, wf_que_t *pque)
{
    if (wf_que_is_empty(pque))
    {
        return NULL;
    }

    wf_lock_lock(&pque->lock);

    wf_list_delete(plist);
    pque->cnt--;

    wf_lock_unlock(&pque->lock);

    return plist;
}

void wf_que_init (wf_que_t *pque, wf_lock_type_e lock_type)
{
    wf_list_init(&pque->head);
    wf_lock_init(&pque->lock, lock_type);
    pque->cnt = 0;
}

