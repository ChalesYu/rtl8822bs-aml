/*
 * wf_os_api_file.h
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
#ifndef __WF_OS_API_FILE_H__
#define __WF_OS_API_FILE_H__

wf_file *wf_os_api_file_open(const char *path);
int     wf_os_api_file_read(wf_file *file, loff_t offset, unsigned char *data, unsigned int size);
int     wf_os_api_file_readline(wf_file *file, loff_t *offset, unsigned char *data, unsigned int size);
size_t  wf_os_api_file_size (wf_file *file);
void    wf_os_api_file_close(wf_file *file);

int wf_os_api_file_write(wf_file *file, loff_t offset, unsigned char *data, unsigned int size);


#endif

