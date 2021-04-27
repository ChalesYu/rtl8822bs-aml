#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"


wf_inline void wf_lock_spin_init(wf_lock_spin *plock)
{
	NdisAllocateSpinLock(plock);
}

void wf_lock_spin_free(wf_lock_spin *plock)
{
	NdisFreeSpinLock(plock);
}

wf_inline void wf_lock_spin_lock(wf_lock_spin *plock)
{
	NdisAcquireSpinLock(plock);
}

wf_inline void wf_lock_spin_unlock(wf_lock_spin *plock)
{
	NdisReleaseSpinLock(plock);
}

wf_inline void wf_lock_bh_lock(wf_lock_spin *plock)
{
	NdisAcquireSpinLock(plock);
}

wf_inline void wf_lock_bh_unlock(wf_lock_spin *plock)
{
	NdisReleaseSpinLock(plock);
}

wf_inline void wf_lock_irq_lock(wf_lock_spin *plock, wf_irq *pirqL)
{
	NdisAcquireSpinLock(plock);
	*pirqL = plock->OldIrql;
	//spin_lock_irqsave(plock, *pirqL);
}

wf_inline void wf_lock_irq_unlock(wf_lock_spin *plock, wf_irq *pirqL)
{
	plock->OldIrql = *pirqL;
	NdisReleaseSpinLock(plock);
	//spin_unlock_irqrestore(plock, *pirqL);
}

wf_inline void wf_lock_mutex_init(wf_lock_mutex *mtx)
{
	NDIS_INIT_MUTEX(mtx);
	//mutex_init(mtx);
}

wf_inline void wf_lock_mutex_lock(wf_lock_mutex *mtx)
{
	NDIS_WAIT_FOR_MUTEX(mtx);
	//mutex_lock(mtx);
}

wf_inline void wf_lock_mutex_unlock(wf_lock_mutex *mtx)
{
	NDIS_RELEASE_MUTEX(mtx);
	//mutex_unlock(mtx);
}

void wf_lock_init(wf_lock_t *lock, wf_lock_type_e lock_type)
{
	lock->lock_type = lock_type;
	if (WF_LOCK_TYPE_NONE == lock_type)
	{
		return;
	}
	else if (WF_LOCK_TYPE_MUTEX == lock_type)
	{
		wf_lock_mutex_init(&lock->lock_mutex);
	}
	else
	{
		wf_lock_spin_init(&lock->lock_spin.lock);
	}
}

void wf_lock_lock(wf_lock_t *lock)
{
	if (WF_LOCK_TYPE_NONE == lock->lock_type)
	{
		return;
	}
	else if (WF_LOCK_TYPE_MUTEX == lock->lock_type)
	{
		wf_lock_mutex_lock(&lock->lock_mutex);
	}
	else if (WF_LOCK_TYPE_BH == lock->lock_type)
	{
		wf_lock_bh_lock(&lock->lock_spin.lock);
	}
	else if (WF_LOCK_TYPE_SPIN == lock->lock_type)
	{
		wf_lock_spin_lock(&lock->lock_spin.lock);
	}
	else if (WF_LOCK_TYPE_IRQ == lock->lock_type)
	{
		wf_lock_irq_lock(&lock->lock_spin.lock, &lock->lock_spin.val_irq);
	}
}
void wf_lock_unlock(wf_lock_t *lock)
{
	if (WF_LOCK_TYPE_NONE == lock->lock_type)
	{
		return;
	}
	else if (WF_LOCK_TYPE_MUTEX == lock->lock_type)
	{
		wf_lock_mutex_unlock(&lock->lock_mutex);
	}
	else if (WF_LOCK_TYPE_BH == lock->lock_type)
	{
		wf_lock_bh_unlock(&lock->lock_spin.lock);
	}
	else if (WF_LOCK_TYPE_SPIN == lock->lock_type)
	{
		wf_lock_spin_unlock(&lock->lock_spin.lock);
	}
	else if (WF_LOCK_TYPE_IRQ == lock->lock_type)
	{
		wf_lock_irq_unlock(&lock->lock_spin.lock, &lock->lock_spin.val_irq);
	}
}
void wf_lock_term(wf_lock_t *lock)
{
	if (WF_LOCK_TYPE_NONE == lock->lock_type)
	{
		return;
	}
	else if (WF_LOCK_TYPE_MUTEX == lock->lock_type)
	{
		//mutex_destroy(&lock->lock_mutex);
	}
	else
	{

	}

}
