#pragma once
#ifndef __WF_OS_API_FILE_H__

typedef struct wf_file_s {
	HANDLE handle;
	IO_STATUS_BLOCK io_status;
}wf_file_t;
typedef wf_file_t  wf_file;

wf_file *wf_os_api_file_open(const char *path);
int     wf_os_api_file_read(wf_file *file, loff_t offset, unsigned char *data, unsigned int size);
void    wf_os_api_file_close(wf_file *file);

#endif // !__WF_OS_API_FILE_H__
