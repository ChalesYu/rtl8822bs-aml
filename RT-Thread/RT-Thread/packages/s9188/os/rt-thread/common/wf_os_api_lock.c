/*
 * wf_os_api_lock.c
 *
 * os lock realization.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "wf_typedef.h"
#include "wf_os_api.h"


static wf_inline void wf_lock_spin_lock (wf_lock_spin *plock)
{
    rt_hw_spin_lock(plock);
}

static wf_inline void wf_lock_spin_unlock (wf_lock_spin *plock)
{
    rt_hw_spin_unlock(plock);
}

static wf_inline void wf_lock_spin_init (wf_lock_spin *plock)
{
#ifdef RT_USING_SMP
    rt_hw_spin_lock_init(plock);
#endif
}

static wf_inline void wf_lock_spin_free (wf_lock_spin *plock) {}

static wf_inline void wf_lock_bh_lock (wf_lock_spin *plock)
{
    rt_hw_spin_lock(plock);
}

static wf_inline void wf_lock_bh_unlock (wf_lock_spin *plock)
{
    rt_hw_spin_unlock(plock);
}

static wf_inline void wf_lock_irq_lock (wf_lock_spin *plock, wf_irq *pirqL)
{
  *pirqL = rt_hw_interrupt_disable();
#ifdef RT_USING_SMP
  rt_hw_spin_lock(plock);
#endif
}

static wf_inline void wf_lock_irq_unlock (wf_lock_spin *plock, wf_irq *pirqL)
{
#ifdef RT_USING_SMP
    rt_hw_spin_unlock(plock);
#endif
    rt_hw_interrupt_enable(*pirqL);
}

static wf_inline void wf_lock_mutex_lock (wf_lock_mutex *mtx)
{
    rt_mutex_take(mtx, RT_WAITING_FOREVER);
}

static wf_inline void wf_lock_mutex_unlock (wf_lock_mutex *mtx)
{
    rt_mutex_release(mtx);
}

static wf_inline void wf_lock_mutex_init (wf_lock_mutex *mtx)
{
  static int seq = 0;
  char name[RT_NAME_MAX] = {0};
  sprintf(name, "wmtx%d", seq++);
  rt_mutex_init(mtx, name, RT_IPC_FLAG_FIFO);
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
            return rt_mutex_take(&plock->lock_mutex, 0);

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
            rt_mutex_detach(&plock->lock_mutex);
            break;

        case WF_LOCK_TYPE_NONE :
        default:
            break;
    }
}

