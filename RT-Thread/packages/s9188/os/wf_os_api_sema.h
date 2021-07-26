
#ifndef __WF_OS_API_SEMA_H__
#define __WF_OS_API_SEMA_H__

void wf_os_api_sema_init (wf_os_api_sema_t *sema, int init_val);
void wf_os_api_sema_free (wf_os_api_sema_t *sema);
void wf_os_api_sema_post (wf_os_api_sema_t *sema);
int wf_os_api_sema_wait (wf_os_api_sema_t *sema);
int wf_os_api_sema_try (wf_os_api_sema_t *sema);


#endif

