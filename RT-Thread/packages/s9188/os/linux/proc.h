/*
 * proc.h
 *
 * used for print logs
 *
 * Author: pansiwei
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
#ifndef __PROC_H__
#define __PROC_H__

#include "ndev_linux.h"

#define wf_register_proc_interface(_name, _show, _write) \
    { .name = _name, .show = _show, .write = _write}
#define wf_print_seq seq_printf

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24))
#define wf_proc_net proc_net
#else
extern struct net init_net;
#define wf_proc_net init_net.proc_net
#endif

struct wf_proc_handle{
	char *name;
	int (*show)(struct seq_file *, void *);
	ssize_t (*write)(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
};


#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0))
#define PDE_DATA(inode) PDE((inode))->data
#define proc_get_parent_data(inode) PDE((inode))->parent->data
#endif

typedef struct
{
    void *hif_info;
    struct proc_dir_entry *proc_root;
    char proc_name[32];
}wf_proc_st;

int wf_proc_init(void *nic_info);
void wf_proc_term(void *nic_info);

#endif

