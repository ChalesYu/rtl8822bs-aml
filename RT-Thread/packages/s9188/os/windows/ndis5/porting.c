#include "pcomp.h"
#include "wf_debug.h"
#define DELAY_ONE_MICROSECOND   (-10)
#define DELAY_ONE_MILLISECOND   (DELAY_ONE_MICROSECOND*1000)

#if 0

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
	plock->OldIrql = (KIRQL)*pirqL;
	NdisReleaseSpinLock(plock);
	//spin_unlock_irqrestore(plock, *pirqL);
}

wf_inline void wf_lock_mutex_init(wf_lock_mutex *mtx)
{
	KeInitializeMutex(mtx, 0xffff);
}

wf_inline void wf_lock_mutex_lock(wf_lock_mutex *mtx)
{
	KeWaitForSingleObject(mtx, Executive, KernelMode, FALSE, NULL);
}

wf_inline void wf_lock_mutex_unlock(wf_lock_mutex *mtx)
{
	KeReleaseMutex(mtx, FALSE);
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

void *wf_malloc(wf_u32 size)
{
	void *ptr = NULL;

	if (NdisAllocateMemoryWithTag(&ptr, size, MP_MEMORY_TAG) == NDIS_STATUS_SUCCESS) {
		return ptr;
	}
	else {
		LOG_E("malloc memory failed! size=%d", size);
		NdisZeroMemory(ptr, size);
	}
}

void wf_free(void *ptr)
{
	if (ptr) {
		NdisFreeMemory(ptr, 0, 0);
		ptr = NULL;
	}
}

void wf_Msleep(wf_u32 MilliSecond)
{
	LARGE_INTEGER Interval = { 0 };
	Interval.QuadPart = DELAY_ONE_MILLISECOND;
	Interval.QuadPart *= MilliSecond;
	KeDelayExecutionThread(KernelMode, 0, &Interval);
}

void wf_Usleep(wf_u32 MilliSecond)
{
	LARGE_INTEGER Interval = { 0 };
	Interval.QuadPart = DELAY_ONE_MICROSECOND;
	Interval.QuadPart *= MilliSecond;
	KeDelayExecutionThread(KernelMode, 0, &Interval);
}

wf_u32 wf_os_api_rand32(void)
{
	LARGE_INTEGER time = { 0 };
	static wf_u32 local_cnt = 0;
	wf_u32 rand_val;

	//tick conut is update per 10 ms, so we add a local bias cnt
	KeQueryTickCount(&time);
	local_cnt++;
	rand_val = (wf_u32)time.QuadPart;
	rand_val += local_cnt;

	return rand_val;
}
void wf_os_api_ind_disconnect(void *arg, wf_u8 arg1)
{
	UNREFERENCED_PARAMETER(arg1);
	nic_info_st *pnic_info = (nic_info_st *)arg;
	PADAPTER pAdapter = (PADAPTER)pnic_info->hif_node;
	//wf_submit_disassoc_complete(pAdapter, DOT11_DISASSOC_REASON_OS);
	return;
}

void wf_os_api_enable_all_data_queue(void *arg)
{
	UNREFERENCED_PARAMETER(arg);

}

void wf_os_api_disable_all_data_queue(void *arg)
{
	UNREFERENCED_PARAMETER(arg);
}


void wf_os_api_ind_scan_done(void *arg, wf_bool arg1, wf_u8 arg2)
{
	nic_info_st *pnic_info = arg;
	PADAPTER padapter = pnic_info->hif_node;
	wf_mib_info_t *mib_info = padapter->mib_info;

	UNREFERENCED_PARAMETER(arg1);
	UNREFERENCED_PARAMETER(arg2);
//check
#if 0
	wf_submit_scan_complete(padapter);
	if (mib_info->AuthAlgorithm == DOT11_AUTH_ALGO_RSNA ||
		mib_info->AuthAlgorithm == DOT11_AUTH_ALGO_RSNA_PSK)
	{
		wf_check_PMKID_candidate(padapter);
	}
#endif
}

void wf_os_api_ind_connect(void *arg, wf_u8 arg1)
{
	nic_info_st *pnic_info = arg;
	PADAPTER padapter = pnic_info->hif_node;
	wdn_net_info_st *pwdn_info;

	UNREFERENCED_PARAMETER(arg1);
//check
#if 0
	//if the request is NULL, means it timeout
	if (padapter->PendedRequest != NULL) {
		wf_submit_assoc_complete(padapter, DOT11_ASSOC_STATUS_SUCCESS);
		wf_submit_connect_complete(padapter, DOT11_CONNECTION_STATUS_SUCCESS);
		Mp11CompletePendedRequest(padapter, NDIS_STATUS_SUCCESS);

		pwdn_info = wf_wdn_find_info(pnic_info, wf_wlan_get_cur_bssid(pnic_info));
		if (pwdn_info != NULL) {
			wf_submit_link_speed(padapter, wf_get_speed_by_raid(pwdn_info->raid));
		}
		else {
			LOG_E("pwdn is NULL");
		}
	}
#endif
}

void *wf_os_api_get_ars(void *nic_info)
{
	nic_info_st *nic_node = nic_info;
	ADAPTER *padt = nic_node->hif_node;
	return padt->ars;
}
void  wf_os_api_set_ars(void *nic_info, void *ars)
{
	nic_info_st *nic_node = nic_info;
	ADAPTER *padt = nic_node->hif_node;
	padt->ars = ars;
}
void *wf_os_api_get_odm(void *nic_info)
{
	nic_info_st *nic_node = nic_info;
	ADAPTER *padt = nic_node->hif_node;
	return padt->odm;
}
void  wf_os_api_set_odm(void *nic_info, void *odm)
{
	nic_info_st *nic_node = nic_info;
	ADAPTER *padt = nic_node->hif_node;
	padt->odm = odm;
}

void wf_os_api_sema_init(wf_os_api_sema_t *sema, int init_val)
{
	KeInitializeSemaphore(sema, init_val, WF_SEMA_LIMIT_MAX);
	return;
}

void wf_os_api_sema_free(wf_os_api_sema_t *sema)
{
	//UNREFERENCED_PARAMETER(sema);
	KeReleaseSemaphore(sema, IO_NO_INCREMENT, 1, FALSE);
	return;
}

void wf_os_api_sema_post(wf_os_api_sema_t *sema)
{
	KeReleaseSemaphore(sema, IO_NO_INCREMENT, 1, FALSE);
}

int wf_os_api_sema_wait(wf_os_api_sema_t *sema)
{
	NTSTATUS ret;
	ret = KeWaitForSingleObject(sema, Executive, KernelMode, FALSE, NULL);
	if (ret != STATUS_SUCCESS) {
		LOG_D("######status=%d", ret);
		return -1;
	}
	else {
		return 0;
	}
}

int wf_os_api_sema_try(wf_os_api_sema_t *sema)
{
	NTSTATUS ret;
	LARGE_INTEGER SemaWaitTime = { 0 };
	ret = KeWaitForSingleObject(sema, Executive, KernelMode, FALSE, &SemaWaitTime);
	if (ret != STATUS_SUCCESS) {
		return -1;
	}
	else {
		return 0;
	}
}
void wf_os_api_thread_affinity(wf_u8 cpu_id)
{
	KAFFINITY act_affinity, config_affinity = 0, temp = 1;
	ULONG max_processor = 0;
	INT i = 0;
	//wf_s8 i;
	act_affinity = KeQueryActiveProcessors();
	LOG_D("act=%x\n", act_affinity);
	for (i = 0; i < 32; i++)
	{
		if (act_affinity & 1 == 1)
			max_processor++;
		act_affinity = act_affinity >> 1;
	}
	LOG_D("max = %d\n", max_processor);
	//LOG_D("curr process=%d", KeGetCurrentProcessorIndex());
	//check
	//max_processor = KeQueryMaximumProcessorCount();
	if (cpu_id < max_processor) {
		if (act_affinity & temp << cpu_id) {
			config_affinity = (act_affinity & temp << cpu_id);
			KeSetSystemAffinityThread(config_affinity);
		}
	}
}

void* wf_os_api_thread_create(void *ptid, char *name, void *func, void *param)
{
	wf_thread_t *pthread = ptid;
	HANDLE   ThreadHandle = NULL;
	NTSTATUS status = STATUS_SUCCESS;
	CLIENT_ID       ClientID = { 0 };

	pthread = wf_malloc(sizeof(wf_thread_t));
	if (NULL == pthread)
	{
		LOG_E("wf_malloc failed");
		return NULL;
	}
	wf_memset(pthread, 0, sizeof(wf_thread_t));

	LOG_D("%s start\n", name);
	status = PsCreateSystemThread(
		&ThreadHandle,
		0,
		NULL,
		NULL,
		&ClientID,
		(PKSTART_ROUTINE)func,
		param);
	LOG_D("%s end\n", name);
	if (status != STATUS_SUCCESS)
	{
		LOG_E("PsCreateSystemThread failed\n");
		return NULL;
	}
	else
	{
		LOG_D("%s start\n", name);
		status = ObReferenceObjectByHandle(ThreadHandle, THREAD_ALL_ACCESS,
			*PsThreadType, KernelMode, &pthread->obj, NULL);
		LOG_D("%s end\n", name);
		pthread->stop = wf_false;
		pthread->tid = ThreadHandle;
		wf_memcpy(pthread->name, name, strlen(name));
		ZwClose(ThreadHandle);
		return pthread;
	}
}

int wf_os_api_thread_wakeup(void *ptid)
{
	UNREFERENCED_PARAMETER(ptid);
	//wf_thread_t *pthread = ptid;
	return 0;
}

int wf_os_api_thread_destory(void *ptid)
{
	wf_thread_t *pthread = ptid;

	LOG_D("####thread[%s] send a stop signal\n", pthread->name);
	pthread->stop = wf_true;
	KeWaitForSingleObject(pthread->obj, Executive,
		KernelMode, wf_false, NULL);
	LOG_D("####thread[%s] stop signal exec end\n", pthread->name);
	wf_free(pthread);
	pthread = NULL;
	return 0;
}


wf_bool wf_os_api_thread_wait_stop(void *ptid)
{
	wf_thread_t *pthread = ptid;

	if (pthread == NULL) {
		return wf_true;
	}

	if (pthread->stop == wf_true)
	{
		LOG_D("####thread[%x][%x] receive a stop signal\n", pthread->obj, pthread->tid);
		return wf_true;
	}
	else {
		return wf_false;
	}
}

void wf_os_api_thread_exit(void *ptid)
{
	wf_thread_t *pthread = ptid;
	LOG_D("####thread[%x][%x] stop thread!!!\n", pthread->obj,
		pthread->tid);
	PsTerminateSystemThread(STATUS_SUCCESS);
}

void KdeferredRoutine(KDPC *Dpc, PVOID DeferredContext,
	PVOID SystemArgument1, PVOID SystemArgument2)
{
	wf_os_api_timer_t *ptimer = DeferredContext;

	UNREFERENCED_PARAMETER(SystemArgument1);
	UNREFERENCED_PARAMETER(SystemArgument2);
	UNREFERENCED_PARAMETER(Dpc);

	if (ptimer && ptimer->fn)
	{
		ptimer->fn(ptimer->pdata);
	}
}

int wf_os_api_timer_reg(wf_os_api_timer_t *ptimer,
	void(*fn) (wf_os_api_timer_t  *), void *pdata)
{
	ptimer->fn = fn;
	ptimer->pdata = pdata;
	KeInitializeDpc(&ptimer->kdpc, KdeferredRoutine, ptimer);
	KeInitializeTimer(&ptimer->ktimer);

	return 0;
}

int wf_os_api_timer_set(wf_os_api_timer_t *ptimer, wf_u32 intv_ms)
{
	LARGE_INTEGER due_time = { 0 };

	due_time.QuadPart = DELAY_ONE_MILLISECOND;
	due_time.QuadPart *= intv_ms;
	return !KeSetTimer(&ptimer->ktimer,
		due_time,
		&ptimer->kdpc);
}

int wf_os_api_timer_unreg(wf_os_api_timer_t *ptimer)
{
	KeCancelTimer(&ptimer->ktimer);

	return 0;
}

wf_u64 wf_os_api_timestamp(void)
{
	LARGE_INTEGER CurrentCount;
	KeQueryTickCount(&CurrentCount);

	return CurrentCount.QuadPart;
}

wf_u32 wf_os_api_msecs_to_timestamp(wf_u32 msecs)
{
	wf_u32 num = WF_DIV_ROUND_CLOSEST(msecs * 10000, KeQueryTimeIncrement());

	return num ? num : 1;
}

wf_u32 wf_os_api_timestamp_to_msecs(wf_u32 timestamp)
{
	wf_u32 num = WF_DIV_ROUND_CLOSEST(timestamp * KeQueryTimeIncrement(), 10000);

	return num ? num : 1;
}

static void wf_workque_init(wf_work_struct *wk_que, void *dev, void *func, void *param)
{
	wk_que->driver_obj = dev;
	wk_que->function = func;
	wk_que->param = param;
	wk_que->que_type = DelayedWorkQueue;
	wk_que->work_item = IoAllocateWorkItem(dev);
	if (NULL == wk_que->work_item)
	{
		LOG_E("[%s] IoAllocateWorkItem failed\n", __func__);
		return;
	}
}

static void wf_workque_enque(wf_work_struct *wk_que)
{
	IoQueueWorkItem(
		wk_que->work_item,
		wk_que->function,
		wk_que->que_type,
		wk_que->param);
}
//check
static void wf_workque_deque(wf_work_struct *wk_que)
{
	//IoUninitializeWorkItem(wk_que->work_item);
}

static void wf_workque_destroy(wf_work_struct *wk_que)
{
	IoFreeWorkItem(wk_que->work_item);
}

int wf_os_api_workqueue_init(wf_workqueue_mgnt_st *arg, void *param)
{
	wf_workqueue_func_param_st *tparam = param;
	wf_workque_init(&arg->work, tparam->DriverObject, tparam->func, &arg->work);
	return 0;
}
int wf_os_api_workqueue_term(wf_workqueue_mgnt_st *arg)
{
	wf_workque_deque(&arg->work);
	wf_workque_destroy(&arg->work);
	return 0;
}
int wf_os_api_workqueue_work(wf_workqueue_mgnt_st *arg)
{
	wf_workque_enque(&arg->work);
	return 0;
}

static wf_workqueue_ops_st wf_gl_workqueue_ops =
{
	.workqueue_init = wf_os_api_workqueue_init,
	.workqueue_term = wf_os_api_workqueue_term,
	.workqueue_work = wf_os_api_workqueue_work,
};

void wf_os_api_workqueue_register(wf_workqueue_mgnt_st *wq, void *param)
{
	wq->ops = &wf_gl_workqueue_ops;
	wq->param = param;
	wq->ops->workqueue_init(wq, wq->param);
}
#endif