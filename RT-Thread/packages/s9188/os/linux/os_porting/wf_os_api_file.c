/*
 * wf_os_api_file.c
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

wf_file *wf_os_api_file_open (const char *path)
{
    wf_file *filp = NULL;
    int err = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
    filp = filp_open(path, O_RDONLY, 0);
    if (IS_ERR(filp))
    {
        err = PTR_ERR(filp);
        return NULL;
    }
#else
    mm_segment_t oldfs;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, O_RDONLY, 0);
    set_fs(oldfs);
    if (IS_ERR(filp))
    {
        err = PTR_ERR(filp);
        return NULL;
    }
#endif

    return filp;
}

int wf_os_api_file_read (wf_file *file, loff_t offset,
                         unsigned char *data, unsigned int size)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)
    mm_segment_t oldfs;
#endif
    int ret;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 114)
    ret = kernel_read(file, data, size, &offset);
#else
    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
#endif
    return ret;
}

// int wf_os_api_file_readline (wf_file *file, loff_t *offset,
//                          unsigned char *data, unsigned int size)
// {
// #if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)
//     mm_segment_t oldfs;
// #endif
//     int ret;
//     int num_read;
//     unsigned char *eol;

// #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 114)
//     num_read = kernel_read(file, data, size, offset);
//     if(num_read <= 0) {
//         return 0;
//     }
//     eol = strstr(data, "\r\n");
//     if (eol != NULL) {
//         *eol++ = '\0';
//         *eol++ = '\0';
//         ret = (size_t)(eol - data);
//         *offset -= (num_read - ret);
//     } else {
//         return -1;
//     }
// #else
//     oldfs = get_fs();
//     set_fs(get_ds());

//     num_read = vfs_read(file, data, size, offset);
//     if(num_read <= 0) {
//         set_fs(oldfs);
//         return 0;
//     }
//     eol = strstr(data, "\r\n");
//     if (eol != NULL) {
//         *eol++ = '\0';
//         *eol++ = '\0';
//         ret = (size_t)(eol - data);
//         *offset -= (num_read - ret);
//     } else {
//         return -1;
//     }

//     set_fs(oldfs);
// #endif
//     return ret;
// }

size_t wf_os_api_file_size (wf_file *file)
{
    return vfs_llseek(file, 0L, SEEK_END);
}


wf_inline void wf_os_api_file_close (wf_file *file)
{
    filp_close(file, NULL);
}

