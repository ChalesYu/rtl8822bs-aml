/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef DFS_SELECT_H__
#define DFS_SELECT_H__

#include <libc/libc_fdset.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RT_USING_POSIX
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
#endif

#ifdef __cplusplus
}
#endif

#endif

