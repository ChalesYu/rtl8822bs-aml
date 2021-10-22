
/* include */
#include "common.h"

/* macro */

/* type */

/* function declaration */

OS_INLINE void wf_os_api_sema_init (wf_os_api_sema_t *sema, int init_val)
{
    wf_sema_new(sema, init_val);
}

OS_INLINE void wf_os_api_sema_free (wf_os_api_sema_t *sema)
{
    wf_sema_free(*sema);
}

OS_INLINE void wf_os_api_sema_post (wf_os_api_sema_t *sema)
{
    wf_sema_post(*sema);
}

OS_INLINE int wf_os_api_sema_wait (wf_os_api_sema_t *sema)
{
    return wf_sema_pend(*sema, ~0ul);
}

OS_INLINE int wf_os_api_sema_try (wf_os_api_sema_t *sema)
{
    return wf_sema_try(*sema);
}

