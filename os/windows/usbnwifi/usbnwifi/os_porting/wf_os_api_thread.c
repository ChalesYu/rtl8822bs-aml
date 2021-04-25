#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"


void* wf_os_api_thread_create(void *ptid, char *name, void *func, void *param)
{
    wf_thread_t *pthread = ptid;
    HANDLE   ThreadHandle = NULL;
    NTSTATUS status = STATUS_SUCCESS;
    CLIENT_ID       ClientID = { 0 };

    LOG_D("start\n");
    status = PsCreateSystemThread(
        &ThreadHandle,     
        0,                 
        NULL,              
        NULL,              
        &ClientID,
        (PKSTART_ROUTINE)func,
        param);
    LOG_D("end\n");
    if (status != STATUS_SUCCESS) 
    {
        LOG_E("PsCreateSystemThread failed\n");
        return NULL;
    }
    else 
    {
        LOG_D("start\n");
        status = ObReferenceObjectByHandle(ThreadHandle, THREAD_ALL_ACCESS,
            *PsThreadType, KernelMode, &pthread->obj, NULL);
        LOG_D("end\n");
        pthread->stop = wf_false;
        pthread->tid = ThreadHandle;
        ZwClose(ThreadHandle);
        return pthread;
    }
}

int wf_os_api_thread_wakeup(void *ptid)
{
    wf_thread_t *pthread = ptid;
    return 0;
}

int wf_os_api_thread_destory(void *ptid)
{
    wf_thread_t *pthread = ptid;
    DbgPrint("####thread[%x][%x] send a stop signal\n", pthread->obj,
        pthread->tid);
    pthread->stop = wf_true;
    KeWaitForSingleObject(pthread->obj, Executive,
        KernelMode, wf_false, NULL);
    DbgPrint("####thread[%x][%x] stop signal exec end\n", pthread->obj,
        pthread->tid);

    return 0;
}


wf_bool wf_os_api_thread_wait_stop(void *ptid)
{
    wf_thread_t *pthread = ptid;
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