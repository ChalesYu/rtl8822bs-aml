
/* include */
#include "common.h"
#include "wf_debug.h"

/* macro */
#if 0
#define MSG_DBG(fmt, ...)       LOG_D("[%s]"fmt, __func__, ##__VA_ARGS__)
#define MSG_ARRAY(data, len)    log_array(data, len)
#else
#define MSG_DBG(fmt, ...)
#define MSG_ARRAY(data, len)
#endif
#define MSG_WARN(fmt, ...)      LOG_E("[%s]"fmt, __func__, ##__VA_ARGS__)
#define MSG_INFO(fmt, ...)      LOG_I("[%s]"fmt, __func__, ##__VA_ARGS__)

/* type */

/* function  */

int _msg_pop (wf_msg_t *pmsg)
{
    if (pmsg->pque)
    {
        if (wf_deque(&pmsg->list, pmsg->pque) == NULL)
        {
            MSG_WARN("deque fail !!!!!!!!");
            return -1;
        }
        pmsg->pque = NULL;
    }

    return 0;
}

int _msg_push (wf_que_t *pque, wf_que_list *pos, wf_msg_t *pmsg)
{
    _msg_pop(pmsg); /* todo: check and leave from orignal queue(free or pend) */
    pmsg->pque = pque;
    wf_enque(&pmsg->list, pos, pque);

    return 0;
}

int wf_msg_new (wf_msg_que_t *pmsg_que, wf_msg_tag_t tag, wf_msg_t **pnew_msg)
{
    wf_que_list *plist;
    int rst;

    if (pmsg_que == NULL || pnew_msg == NULL)
    {
        return -1;
    }

    wf_lock_lock(&pmsg_que->lock);
    if (wf_que_is_empty(&pmsg_que->free))
    {
        rst = -2;
        goto exit;
    }
    wf_list_for_each(plist, wf_que_head(&pmsg_que->free))
    {
        wf_msg_t *pmsg = wf_list_entry(plist, wf_msg_t, list);
        if (pmsg->tag == tag)
        {
            rst = _msg_pop(pmsg);
            *pnew_msg = pmsg;
            goto exit;
        }
    }
    rst = -3;

exit:
    wf_lock_unlock(&pmsg_que->lock);
    if (rst)
    {
        *pnew_msg = NULL;
    }
    return rst;
}

int wf_msg_push (wf_msg_que_t *pmsg_que, wf_msg_t *pmsg)
{
    wf_que_list *pos;
    int rst;

    if (pmsg_que == NULL || pmsg == NULL)
    {
        return -1;
    }

    wf_lock_lock(&pmsg_que->lock);
    wf_list_for_each_prev(pos, wf_que_head(&pmsg_que->pend))
    {
        wf_msg_t *pmsg_tmp = wf_list_entry(pos, wf_msg_t, list);
        /* todo: compare use domain and priority field constitute value
        in message betwen pend queue and input specified message. */
        if ((pmsg->tag     | WF_MSG_TAG_ID_MSK) >=
            (pmsg_tmp->tag | WF_MSG_TAG_ID_MSK))
        {
            break;
        }
    }
    rst = _msg_push(&pmsg_que->pend, pos, pmsg);
    wf_lock_unlock(&pmsg_que->lock);

    return rst;
}

int wf_msg_push_head (wf_msg_que_t *pmsg_que, wf_msg_t *pmsg)
{
    wf_que_list *pos;
    int rst;

    if (pmsg_que == NULL || pmsg == NULL)
    {
        return -1;
    }

    wf_lock_lock(&pmsg_que->lock);
    wf_list_for_each(pos, wf_que_head(&pmsg_que->pend))
    {
        wf_msg_t *pmsg_tmp = wf_list_entry(pos, wf_msg_t, list);
        if (WF_MSG_TAG_DOM(pmsg_tmp->tag) >= WF_MSG_TAG_DOM(pmsg->tag))
        {
            break;
        }
    }
    rst = _msg_push(&pmsg_que->pend, wf_list_prev(pos), pmsg);
    wf_lock_unlock(&pmsg_que->lock);

    return rst;
}

int msg_get (wf_msg_que_t *pmsg_que, wf_msg_t **pmsg,
             wf_bool bpop, wf_bool btail)
{
    wf_que_list *plist;
    int rst = 0;

    if (pmsg_que == NULL || pmsg == NULL)
    {
        return -1;
    }

    wf_lock_lock(&pmsg_que->lock);
    if (wf_que_is_empty(&pmsg_que->pend))
    {
        rst = -2;
        goto exit;
    }

    plist = btail ? wf_list_prev(wf_que_head(&pmsg_que->pend)) :
                    wf_list_next(wf_que_head(&pmsg_que->pend));
    *pmsg = wf_list_entry(plist, wf_msg_t, list);
    if (bpop)
    {
        rst = _msg_pop(*pmsg);
    }

exit:
    wf_lock_unlock(&pmsg_que->lock);
    if (rst)
    {
        *pmsg = NULL;
    }
    return rst;
}

int msg_get_dom (wf_msg_que_t *pmsg_que, wf_msg_tag_dom_t dom, wf_msg_t **pmsg,
                 wf_bool bpop, wf_bool btail)
{
    wf_que_list *pos;
    int rst;

    wf_lock_lock(&pmsg_que->lock);
    if (btail)
    {
        wf_list_for_each_prev(pos, wf_que_head(&pmsg_que->pend))
        {
            wf_msg_t *pmsg_tmp = wf_list_entry(pos, wf_msg_t, list);
            if (WF_MSG_TAG_DOM(pmsg_tmp->tag) == dom)
            {
                rst = bpop ? _msg_pop(pmsg_tmp) : 0;
                *pmsg = pmsg_tmp;
                goto exit;
            }
        }
    }
    else
    {
        wf_list_for_each(pos, wf_que_head(&pmsg_que->pend))
        {
            wf_msg_t *pmsg_tmp = wf_list_entry(pos, wf_msg_t, list);
            if (WF_MSG_TAG_DOM(pmsg_tmp->tag) == dom)
            {
                rst = bpop ? _msg_pop(pmsg_tmp) : 0;
                *pmsg = pmsg_tmp;
                goto exit;
            }
        }
    }
    rst = -1;

exit :
    wf_lock_unlock(&pmsg_que->lock);
    if (rst)
    {
        *pmsg = NULL;
    }
    return rst;
}

int wf_msg_del (wf_msg_que_t *pmsg_que, wf_msg_t *pmsg)
{
    wf_que_list *pos;
    int rst;

    if (pmsg_que == NULL || pmsg == NULL)
    {
        return -1;
    }

    wf_lock_lock(&pmsg_que->lock);
    pos = wf_list_prev(wf_que_head(&pmsg_que->free));
    rst = _msg_push(&pmsg_que->free, pos, pmsg);
    wf_lock_unlock(&pmsg_que->lock);

    return rst;
}
int wf_msg_alloc (wf_msg_que_t *pmsg_que,
                  wf_msg_tag_t tag, wf_u32 size, wf_u8 num)
{
    wf_u8 i;
    for (i = 0; i < num; i++)
    {
        wf_msg_t *pmsg = wf_kzalloc(sizeof(wf_msg_t) + size);
        if (pmsg == NULL)
        {
            return -1;
        }
        pmsg->pque = NULL;
        pmsg->tag = tag;
        pmsg->len = 0;
        wf_msg_del(pmsg_que, pmsg);
    }

    return 0;
}

int wf_msg_free (wf_msg_que_t *pmsg_que)
{
    wf_list_t *pos, *pnext;

    wf_lock_lock(&pmsg_que->lock);
    wf_list_for_each_safe(pos, pnext, wf_que_head(&pmsg_que->pend))
    {
        wf_kfree(wf_list_entry(pos, wf_msg_t, list));
    }

    wf_list_for_each_safe(pos, pnext, wf_que_head(&pmsg_que->free))
    {
        wf_kfree(wf_list_entry(pos, wf_msg_t, list));
    }
    wf_lock_unlock(&pmsg_que->lock);

    return 0;
}

