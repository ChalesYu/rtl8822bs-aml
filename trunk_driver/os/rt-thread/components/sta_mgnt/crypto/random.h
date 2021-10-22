/******************************************************************************
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of version 2 of the GNU General Public License as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
******************************************************************************/
#ifndef __RANDOM_H__
#define __RANDOM_H__
#define SHA1_MAC_LEN 20

#ifdef CONFIG_NO_RANDOM_POOL
#define random_init(e) do { } while (0)
#define random_deinit() do { } while (0)
#define wf_random_add_randomness(b, l) do { } while (0)
#define wf_random_get_bytes(b, l) wf_os_get_random((b), (l))
#define random_pool_ready() 1
#define random_mark_pool_ready() do { } while (0)
#else
int wf_random_get_bytes(void *buf, size_t len);

#endif

#endif
