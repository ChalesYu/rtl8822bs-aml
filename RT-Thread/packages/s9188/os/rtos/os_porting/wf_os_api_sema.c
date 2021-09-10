
/* include */
#include "common.h"

/* macro */

/* type */

/* function declaration */

OS_INLINE void wf_os_api_sema_init (wf_os_api_sema_t *sema, int init_val)
{
    *sema = (wf_os_api_sema_t)osSemaphoreNew(~0ul, init_val, NULL);
}

OS_INLINE void wf_os_api_sema_free (wf_os_api_sema_t *sema)
{
    OS_WARN_ON(osSemaphoreDelete(*sema) != osOK);
}

OS_INLINE void wf_os_api_sema_post (wf_os_api_sema_t *sema)
{
    OS_WARN_ON(osSemaphoreRelease(*sema) != osOK);
}

OS_INLINE int wf_os_api_sema_wait (wf_os_api_sema_t *sema)
{
    return (osSemaphoreAcquire(*sema, osWaitForever) != osOK) ? -1 : 0;
}

OS_INLINE int wf_os_api_sema_try (wf_os_api_sema_t *sema)
{
    return (osSemaphoreAcquire(*sema, 0) != osOK) ? -1 : 0;
}

