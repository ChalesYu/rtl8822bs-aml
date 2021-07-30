/*
 * wf_os_api_lock.c
 *
 * used for .....
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
/* include */
#include "wf_typedef.h"
#include "wf_os_api.h"

/* macro */

/* type */

/* function declaration */

wf_inline void wf_lock_spin_lock (wf_lock_spin *plock)
{
    spin_lock(plock);
}

wf_inline void wf_lock_spin_unlock (wf_lock_spin *plock)
{
    spin_unlock(plock);
}

wf_inline void wf_lock_spin_init (wf_lock_spin *plock)
{
    spin_lock_init(plock);
}

wf_inline void wf_lock_spin_free (wf_lock_spin *plock) {}

wf_inline void wf_lock_bh_lock (wf_lock_spin *plock)
{
    spin_lock_bh(plock);
}

wf_inline void wf_lock_bh_unlock (wf_lock_spin *plock)
{
    spin_unlock_bh(plock);
}

wf_inline void wf_lock_irq_lock (wf_lock_spin *plock, wf_irq *pirqL)
{
    spin_lock_irqsave(plock, *pirqL);
}

wf_inline void wf_lock_irq_unlock (wf_lock_spin *plock, wf_irq *pirqL)
{
    spin_unlock_irqrestore(plock, *pirqL);
}

wf_inline void wf_lock_mutex_lock (wf_lock_mutex *mtx)
{
    mutex_lock(mtx);
}

wf_inline int wf_lock_mutex_try_lock (wf_lock_mutex *mtx)
{
    return mutex_trylock(mtx) ? 0 : -1;
}

wf_inline void wf_lock_mutex_unlock (wf_lock_mutex *mtx)
{
    mutex_unlock(mtx);
}

wf_inline void wf_lock_mutex_init (wf_lock_mutex *mtx)
{
    mutex_init(mtx);
}

void wf_lock_lock (wf_lock_t *plock)
{
    switch (plock->lock_type)
    {
        case WF_LOCK_TYPE_MUTEX :
            wf_lock_mutex_lock(&plock->lock_mutex);
            break;

        case WF_LOCK_TYPE_BH :
            wf_lock_bh_lock(&plock->lock_spin.lock);
            break;

        case WF_LOCK_TYPE_SPIN :
            wf_lock_spin_lock(&plock->lock_spin.lock);
            break;

        case WF_LOCK_TYPE_IRQ :
            wf_lock_irq_lock(&plock->lock_spin.lock,&plock->lock_spin.val_irq);
            break;

        case WF_LOCK_TYPE_NONE :
        default :
            break;
    }
}

int wf_lock_trylock (wf_lock_t *plock)
{
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
    switch (plock->lock_type)
    {
        case WF_LOCK_TYPE_MUTEX :
            wf_lock_mutex_unlock(&plock->lock_mutex);
            break;

        case WF_LOCK_TYPE_BH :
            wf_lock_bh_unlock(&plock->lock_spin.lock);
            break;

        case WF_LOCK_TYPE_SPIN :
            wf_lock_spin_unlock(&plock->lock_spin.lock);
            break;

        case WF_LOCK_TYPE_IRQ :
            wf_lock_irq_unlock(&plock->lock_spin.lock,&plock->lock_spin.val_irq);
            break;

        case WF_LOCK_TYPE_NONE :
        default :
            break;
    }
}

void wf_lock_init (wf_lock_t *plock, wf_lock_type_e lock_type)
{
    switch (lock_type)
    {
        case WF_LOCK_TYPE_MUTEX :
            wf_lock_mutex_init(&plock->lock_mutex);
            break;

        case WF_LOCK_TYPE_NONE :
        default :
            wf_lock_spin_init(&plock->lock_spin.lock);
            break;
    }
    plock->lock_type = lock_type;
}

void wf_lock_term (wf_lock_t *plock)
{
    switch (plock->lock_type)
    {
        case WF_LOCK_TYPE_MUTEX :
            mutex_destroy(&plock->lock_mutex);
            break;

        case WF_LOCK_TYPE_NONE :
        default :
            break;
    }
}

