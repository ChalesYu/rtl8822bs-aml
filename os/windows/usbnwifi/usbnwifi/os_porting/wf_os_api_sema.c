
#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"

void wf_os_api_sema_init(wf_sema *sema, int init_val)
{
	KeInitializeEvent(&sema->evt, SynchronizationEvent, FALSE);
}

void wf_os_api_sema_free(wf_sema *sema)
{

}

void wf_os_api_sema_post(wf_sema *sema)
{
	KPRIORITY priority = 0;

	KeSetEvent(&sema->evt, priority, FALSE);
}

int wf_os_api_sema_wait(wf_sema *sema)
{
	KeWaitForSingleObject(&sema->evt,
		Executive, KernelMode, TRUE, NULL);

	return 0;
}

int wf_os_api_sema_try(wf_sema *sema)
{
	NTSTATUS ret;
	sema->timeout.QuadPart = 0;

	ret = KeWaitForSingleObject(&sema->evt,
		Executive, KernelMode, TRUE, &sema->timeout);
	if (ret != STATUS_SUCCESS) {
		return -1;
	}
	else {
		return 0;
	}
}
