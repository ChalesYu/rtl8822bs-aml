/*
 * wf_os_api_sema.c
 *
 * used for .....
 *
 * Author: luozhi
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
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

