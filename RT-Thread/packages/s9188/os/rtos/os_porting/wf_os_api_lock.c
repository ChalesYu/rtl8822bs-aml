
/* include */
#include "common.h"

/* macro */

/* type */

/* function declaration */

OS_STATIC_INLINE void lock_spin_lock (wf_lock_spin *plock)
{
    OS_UNUSED(plock);

    portENTER_CRITICAL();
}

OS_STATIC_INLINE void lock_spin_unlock (wf_lock_spin *plock)
{
    OS_UNUSED(plock);

    portEXIT_CRITICAL();
}

OS_STATIC_INLINE void lock_spin_init (wf_lock_spin *plock)
{
    OS_UNUSED(plock);
}

OS_STATIC_INLINE void lock_spin_free (wf_lock_spin *plock)
{
    OS_UNUSED(plock);
}

OS_STATIC_INLINE void lock_bh_lock (wf_lock_spin *plock)
{
    lock_spin_lock(plock);
}

OS_STATIC_INLINE void lock_bh_unlock (wf_lock_spin *plock)
{
    lock_spin_unlock(plock);
}

OS_STATIC_INLINE void lock_irq_lock (wf_lock_spin *plock, wf_irq *pirqL)
{
    OS_UNUSED(plock);

    *pirqL = portSET_INTERRUPT_MASK_FROM_ISR();
}

OS_STATIC_INLINE void lock_irq_unlock (wf_lock_spin *plock, wf_irq *pirqL)
{
    OS_UNUSED(plock);

    portCLEAR_INTERRUPT_MASK_FROM_ISR(*pirqL);
}

OS_STATIC_INLINE void lock_mutex_lock (wf_lock_mutex *mtx)
{
    OS_WARN_ON(osMutexAcquire((osMutexId_t)*mtx, osWaitForever) != osOK);
}

OS_STATIC_INLINE int wf_lock_mutex_try_lock (wf_lock_mutex *mtx)
{
    return (osMutexAcquire((osMutexId_t)*mtx, 0) == osOK) ? 0 : -1;
}

OS_STATIC_INLINE void lock_mutex_unlock (wf_lock_mutex *mtx)
{
    OS_WARN_ON(osMutexRelease((osMutexId_t)*mtx) != osOK);
}

OS_STATIC_INLINE void lock_mutex_init (wf_lock_mutex *mtx)
{
    osMutexAttr_t mutex_attr =
    {
        .attr_bits = (osMutexRecursive | osMutexPrioInherit),
    };

    *mtx = (wf_lock_mutex)osMutexNew(&mutex_attr);

    OS_WARN_ON(!mtx);
}

OS_STATIC_INLINE void lock_mutex_free (wf_lock_mutex *mtx)
{
    OS_WARN_ON(osMutexDelete((osMutexId_t)*mtx) != osOK);
}

void wf_lock_lock (wf_lock_t *plock)
{
    OS_ASSERT(plock);

    switch (plock->lock_type)
    {
        case WF_LOCK_TYPE_MUTEX :
            lock_mutex_lock(&plock->lock_mutex);
            break;

        case WF_LOCK_TYPE_BH :
            lock_bh_lock(&plock->lock_spin.lock);
            break;

        case WF_LOCK_TYPE_SPIN :
            lock_spin_lock(&plock->lock_spin.lock);
            break;

        case WF_LOCK_TYPE_IRQ :
            lock_irq_lock(&plock->lock_spin.lock,&plock->lock_spin.val_irq);
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
            return wf_lock_mutex_try_lock(&plock->lock_mutex);

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
            lock_mutex_unlock(&plock->lock_mutex);
            break;

        case WF_LOCK_TYPE_BH :
            lock_bh_unlock(&plock->lock_spin.lock);
            break;

        case WF_LOCK_TYPE_SPIN :
            lock_spin_unlock(&plock->lock_spin.lock);
            break;

        case WF_LOCK_TYPE_IRQ :
            lock_irq_unlock(&plock->lock_spin.lock,&plock->lock_spin.val_irq);
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

        case WF_LOCK_TYPE_NONE :
        default :
            lock_spin_init(&plock->lock_spin.lock);
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
            lock_mutex_free(&plock->lock_mutex);
            break;

        case WF_LOCK_TYPE_SPIN :
            lock_spin_free(&plock->lock_spin.lock);
            break;

        case WF_LOCK_TYPE_NONE :
        default :
            break;
    }
}

