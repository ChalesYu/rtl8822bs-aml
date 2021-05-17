
#ifndef __WF_LIST_H__
#define __WF_LIST_H__

/* include */

/* macro */
#define wf_list_entry(ptr, type, field)     WF_CONTAINER_OF(ptr, type, field)
#define wf_list_for_each(pos, head) \
    for (pos = (head)->pnext; pos != (head); pos = pos->pnext)
#define wf_list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)
#define wf_list_for_each_safe(pos, n, head) \
	for (pos = (head)->pnext, n = pos->pnext; pos != (head); \
		 pos = n, n = pos->pnext)
#define wf_list_for_each_safe_prev(pos, p, head) \
	for (pos = (head)->prev, p = pos->prev; pos != (head); \
		 pos = p, p = pos->prev)

/* type */
typedef struct WF_LIST
{
    struct WF_LIST *pnext, *prev;
} wf_list_t;

/* function declaration */
wf_inline static wf_list_t *wf_list_next (wf_list_t *pos)
{
    return pos->pnext;
}

wf_inline static wf_list_t *wf_list_prev (wf_list_t *pos)
{
    return pos->prev;
}

wf_inline static wf_bool wf_list_is_empty (wf_list_t *phead)
{
    return wf_list_next(phead) == phead;
}

wf_inline static void wf_list_insert (wf_list_t *pnew, wf_list_t *prev, wf_list_t *pnext)
{
    pnext->prev = pnew;
    pnew->pnext = pnext;
    pnew->prev  = prev;
    prev->pnext = pnew;
}

wf_inline static void wf_list_insert_next (wf_list_t *pnew, wf_list_t *pos)
{
    wf_list_insert(pnew, pos, wf_list_next(pos));
}

wf_inline static void wf_list_insert_prev (wf_list_t *pnew, wf_list_t *pos)
{
    wf_list_insert(pnew, wf_list_prev(pos), pos);
}

wf_inline static void wf_list_insert_head (wf_list_t *pnew, wf_list_t *phead)
{
    wf_list_insert_next(pnew, phead);
}

wf_inline static void wf_list_insert_tail (wf_list_t *pnew, wf_list_t *phead)
{
    wf_list_insert_prev(pnew, phead);
}

wf_inline static void wf_list_del (wf_list_t *prev, wf_list_t *pnext)
{
    pnext->prev = prev;
    prev->pnext = pnext;
}

wf_inline static void wf_list_init (wf_list_t *plist)
{
    plist->pnext = plist->prev = plist;
}

wf_inline static void wf_list_delete (wf_list_t *pos)
{
    wf_list_del(wf_list_prev(pos), wf_list_next(pos));
    wf_list_init(pos);
}

#endif /* END OF __WF_LIST_H__ */

