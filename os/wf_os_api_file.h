
#ifndef __WF_OS_API_FILE_H__
#define __WF_OS_API_FILE_H__

wf_file *wf_os_api_file_open(const char *path);
int     wf_os_api_file_read(wf_file *file, loff_t offset, unsigned char *data, unsigned int size);
// int     wf_os_api_file_readline (wf_file *file, loff_t *offset, unsigned char *data, unsigned int size);
size_t  wf_os_api_file_size (wf_file *file);
void    wf_os_api_file_close(wf_file *file);

#endif

