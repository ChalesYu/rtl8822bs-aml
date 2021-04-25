#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"


void wf_os_api_thread_affinity(wf_u8 cpu_id)
{
	KAFFINITY act_affinity, config_affinity = 0;
	wf_u8 max_processor;
	wf_s8 i;

	LOG_D("curr process=%d", KeGetCurrentProcessorIndex()); 

	max_processor = KeQueryMaximumProcessorCount();
	if(cpu_id < max_processor) {
		act_affinity = KeQueryActiveProcessors();
		LOG_D("act=%x", act_affinity);

		if(act_affinity & 1<<cpu_id) {
			config_affinity = (act_affinity & 1<<cpu_id);
			act_affinity = KeSetSystemAffinityThreadEx(config_affinity);
			LOG_D("rlt=%x, cfg=%x", act_affinity, config_affinity);
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
	DbgPrint("Thread creation is shielded.");
	return NULL;
}

int wf_os_api_thread_wakeup(void *ptid)
{
    wf_thread_t *pthread = ptid;
    return 0;
}

int wf_os_api_thread_destory(void *ptid)
{
    wf_thread_t *pthread = ptid;

	//LOG_D("curr_irql=%d", KeGetCurrentIrql());

	//if(KeGetCurrentIrql() < DISPATCH_LEVEL) {
	//	LOG_D("curr_irql=%d", KeGetCurrentIrql());
	//}
	
    DbgPrint("####thread[%s] send a stop signal\n", pthread->name);
    pthread->stop = wf_true;
    KeWaitForSingleObject(pthread->obj, Executive,
        KernelMode, wf_false, NULL);
    DbgPrint("####thread[%s] stop signal exec end\n", pthread->name);
	wf_free(pthread);
	pthread = NULL;
    return 0;
}


wf_bool wf_os_api_thread_wait_stop(void *ptid)
{
    wf_thread_t *pthread = ptid;

	if(pthread == NULL) {
		return wf_true;
	}
	
    if (pthread->stop == wf_true) 
    {
        DbgPrint("####thread[%x][%x] receive a stop signal\n", pthread->obj, pthread->tid);
        return wf_true;
    }
    else {
        return wf_false;
    }
}


void wf_os_api_thread_exit(void *ptid)
{
    wf_thread_t *pthread = ptid;
    DbgPrint("####thread[%x][%x] stop thread!!!\n", pthread->obj,
        pthread->tid);
    PsTerminateSystemThread(STATUS_SUCCESS);
}
