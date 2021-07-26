
/* include */
#include "wf_os_api.h"

/* macro */

/* type */

/* function declaration */

wf_inline void wf_os_api_sema_init (wf_os_api_sema_t *sema, int init_val)
{
  static int seq = 0;
  char name[RT_NAME_MAX] = {0};
  sprintf(name, "wsem%d", seq++);
  
  rt_sem_init(sema, name, init_val, RT_IPC_FLAG_FIFO);
}

wf_inline void wf_os_api_sema_free (wf_os_api_sema_t *sema)
{
    rt_sem_detach(sema);
}

wf_inline void wf_os_api_sema_post (wf_os_api_sema_t *sema)
{
    rt_sem_release(sema);
}

wf_inline int wf_os_api_sema_wait (wf_os_api_sema_t *sema)
{
  return ((RT_EOK == rt_sem_take(sema, RT_WAITING_FOREVER))?0:-1);
}

wf_inline int wf_os_api_sema_try (wf_os_api_sema_t *sema)
{
  return ((RT_EOK == rt_sem_trytake(sema))?0:-1);
}

