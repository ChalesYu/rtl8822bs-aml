
#ifndef __WF_OS_API_LOCK_H__
#define __WF_OS_API_LOCK_H__

typedef spinlock_t              wf_lock_spin;

typedef struct
{
	wf_lock_spin lock;
	wf_irq val_irq;
} wf_lock_spin_t;
typedef enum
{
	WF_LOCK_TYPE_NONE = 0,
	WF_LOCK_TYPE_MUTEX,
	WF_LOCK_TYPE_SPIN,
	WF_LOCK_TYPE_BH,
	WF_LOCK_TYPE_IRQ,

	WF_LOCK_TYPE_MAX,
} wf_lock_type_e;
typedef struct
{
	union
	{
		wf_lock_mutex lock_mutex;
		wf_lock_spin_t lock_spin;
	};
	wf_lock_type_e lock_type;
} wf_lock_t;


void wf_lock_lock(wf_lock_t *plock);
void wf_lock_unlock(wf_lock_t *plock);
void wf_lock_init(wf_lock_t *plock, wf_lock_type_e lock_type);
void wf_lock_term(wf_lock_t *plock);

void wf_lock_mutex_init(wf_lock_mutex *mtx);
void wf_lock_mutex_unlock(wf_lock_mutex *mtx);
void wf_lock_mutex_lock(wf_lock_mutex *mtx);
void wf_lock_spin_lock(wf_lock_spin *plock);
void wf_lock_spin_unlock(wf_lock_spin *plock);
void wf_lock_spin_init(wf_lock_spin *plock);
void wf_lock_spin_free(wf_lock_spin *plock);
void wf_lock_bh_lock(wf_lock_spin *plock);
void wf_lock_bh_unlock(wf_lock_spin *plock);
void wf_lock_irq_lock(wf_lock_spin *plock, wf_irq *pirqL);
void wf_lock_irq_unlock(wf_lock_spin *plock, wf_irq *pirqL);

#endif

