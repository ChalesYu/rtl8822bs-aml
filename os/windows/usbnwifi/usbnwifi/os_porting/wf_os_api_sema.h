#pragma once
#ifndef __WF_OS_API_SEMA_H__

typedef struct wf_event_s {
	KEVENT evt;
	LARGE_INTEGER timeout;
}wf_event_t;
typedef wf_event_t wf_sema;
void wf_os_api_sema_init(wf_sema *sema, int init_val);
void wf_os_api_sema_free(wf_sema *sema);
void wf_os_api_sema_post(wf_sema *sema);
int  wf_os_api_sema_wait(wf_sema *sema);
int  wf_os_api_sema_try(wf_sema *sema);


#endif // !__WF_OS_API_SEMA_H__
