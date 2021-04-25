
/* include */
#include "wf_os_api.h"

/* macro */

/* type */

/* function declaration */

wf_inline void wf_os_api_sema_init (wf_os_api_sema_t *sema, int init_val)
{
    sema_init(sema, init_val);
}

wf_inline void wf_os_api_sema_free (wf_os_api_sema_t *sema)
{
}

wf_inline void wf_os_api_sema_post (wf_os_api_sema_t *sema)
{
    up(sema);
}

wf_inline int wf_os_api_sema_wait (wf_os_api_sema_t *sema)
{
    return down_interruptible(sema) ? -1 : 0;
}

wf_inline int wf_os_api_sema_try (wf_os_api_sema_t *sema)
{
    return down_trylock(sema) ? -1 : 0;
}

