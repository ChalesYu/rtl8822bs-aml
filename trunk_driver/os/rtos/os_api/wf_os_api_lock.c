
/* include */
#include "common.h"

/* macro */

/* type */

/* function declaration */

OS_STATIC_INLINE void lock_spin_lock (wf_lock_spin_t *plock)
{
    plock->val_irq = wf_enter_critical();
}

OS_STATIC_INLINE void lock_spin_unlock (wf_lock_spin_t *plock)
{
    wf_exit_critical(plock->val_irq);
}


OS_STATIC_INLINE void lock_bh_lock (wf_lock_spin_t *plock)
{
    lock_spin_lock(plock);
}

OS_STATIC_INLINE void lock_bh_unlock (wf_lock_spin_t *plock)
{
    lock_spin_unlock(plock);
}


OS_STATIC_INLINE void lock_irq_lock (wf_lock_spin_t *plock)
{
    lock_spin_lock(plock);
}

OS_STATIC_INLINE void lock_irq_unlock (wf_lock_spin_t *plock)
{
    lock_spin_unlock(plock);
}


OS_STATIC_INLINE void lock_mutex_lock (wf_lock_mutex mtx)
{
    wf_mutex_lock(mtx);
}

OS_STATIC_INLINE int wf_lock_mutex_try_lock (wf_lock_mutex mtx)
{
    return wf_mutex_try_lock(mtx);
}

OS_STATIC_INLINE void lock_mutex_unlock (wf_lock_mutex mtx)
{
    wf_mutex_unlock(mtx);
}

OS_STATIC_INLINE void lock_mutex_init (wf_lock_mutex *mtx)
{
    wf_mutex_new(mtx);
}

OS_STATIC_INLINE void lock_mutex_free (wf_lock_mutex mtx)
{
    wf_mutex_free(mtx);
}


void wf_lock_lock (wf_lock_t *plock)
{
    OS_ASSERT(plock);

    switch (plock->lock_type)
    {
        case WF_LOCK_TYPE_MUTEX :
            lock_mutex_lock(plock->lock_mutex);
            break;

        case WF_LOCK_TYPE_BH :
            lock_bh_lock(&plock->lock_spin);
            break;

        case WF_LOCK_TYPE_SPIN :
            lock_spin_lock(&plock->lock_spin);
            break;

        case WF_LOCK_TYPE_IRQ :
            lock_irq_lock(&plock->lock_spin);
            break;

        case WF_LOCK_TYPE_NONE :
        default :
            break;
    }
}

int wf_lock_trylock (wf_lock_t *plock)
{
    if (!plock)
    {
        return -1;
    }

    switch (plock->lock_type)
    {
        case WF_LOCK_TYPE_MUTEX :
            return wf_lock_mutex_try_lock(plock->lock_mutex);

        default :
            break;
    }

    return -2;
}

void wf_lock_unlock (wf_lock_t *plock)
{
    OS_ASSERT(plock);

    switch (plock->lock_type)
    {
        case WF_LOCK_TYPE_MUTEX :
            lock_mutex_unlock(plock->lock_mutex);
            break;

        case WF_LOCK_TYPE_BH :
            lock_bh_unlock(&plock->lock_spin);
            break;

        case WF_LOCK_TYPE_SPIN :
            lock_spin_unlock(&plock->lock_spin);
            break;

        case WF_LOCK_TYPE_IRQ :
            lock_irq_unlock(&plock->lock_spin);
            break;

        case WF_LOCK_TYPE_NONE :
        default :
            break;
    }
}

void wf_lock_init (wf_lock_t *plock, wf_lock_type_e lock_type)
{
    OS_ASSERT(plock);

    switch (lock_type)
    {
        case WF_LOCK_TYPE_MUTEX :
            lock_mutex_init(&plock->lock_mutex);
            break;

        default :
            break;
    }
    plock->lock_type = lock_type;
}

void wf_lock_term (wf_lock_t *plock)
{
    OS_ASSERT(plock);

    switch (plock->lock_type)
    {
        case WF_LOCK_TYPE_MUTEX :
            lock_mutex_free(plock->lock_mutex);
            break;

        default :
            break;
    }
}

