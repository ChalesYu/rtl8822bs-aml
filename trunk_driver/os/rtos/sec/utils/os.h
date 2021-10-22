/*
 * OS specific functions
 * Copyright (c) 2005-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef OS_H
#define OS_H

#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "wf_typedef.h"
#include "os_utility.h"
typedef long os_time_t;

struct os_time {
	os_time_t sec;
	os_time_t usec;
};

struct os_reltime {
	os_time_t sec;
	os_time_t usec;
};
int wf_os_get_reltime(struct os_reltime *t);
void wf_os_bin_clear_free(void *bin, size_t len);
void wf_os_inc_byte_array(wf_u8 *counter, size_t len);
int wf_os_memcmp_const(const void *a, const void *b, size_t len);
int wf_os_get_random(unsigned char *buf, size_t len);

#endif /* OS_H */
