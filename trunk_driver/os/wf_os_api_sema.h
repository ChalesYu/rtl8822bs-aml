/*
 * wf_os_api_sema.h
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
#ifndef __WF_OS_API_SEMA_H__
#define __WF_OS_API_SEMA_H__

void wf_os_api_sema_init (wf_os_api_sema_t *sema, int init_val);
void wf_os_api_sema_free (wf_os_api_sema_t *sema);
void wf_os_api_sema_post (wf_os_api_sema_t *sema);
int wf_os_api_sema_wait (wf_os_api_sema_t *sema);
int wf_os_api_sema_try (wf_os_api_sema_t *sema);


#endif

