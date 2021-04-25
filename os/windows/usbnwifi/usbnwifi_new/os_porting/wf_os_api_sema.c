
#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"

/*
	The semaphore mechanisms are different in Linux and Windows. 
*/

void wf_os_api_sema_init(wf_os_api_sema_t *sema, int init_val)
{
	KeInitializeSemaphore(sema, init_val, WF_SEMA_LIMIT_MAX);
	return;
}

void wf_os_api_sema_free(wf_os_api_sema_t *sema)
{
	UNREFERENCED_PARAMETER(sema);
	return;
}

void wf_os_api_sema_post(wf_os_api_sema_t *sema)
{
	KeReleaseSemaphore(sema, IO_NO_INCREMENT,1, FALSE);
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
	LARGE_INTEGER SemaWaitTime = {0};
	ret = KeWaitForSingleObject(sema, Executive, KernelMode, FALSE, &SemaWaitTime);
	if (ret != STATUS_SUCCESS) {
		return -1;
	}
	else {
		return 0;
	}
}
