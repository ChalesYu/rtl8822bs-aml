
/* include */
#include "wf_os_api.h"

/* macro */

/* type */

/* function declaration */

wf_file *wf_os_api_file_open (const char *path)
{
    return fopen(path, "rb");
}

int wf_os_api_file_read (wf_file *file, loff_t offset,
                         unsigned char *data, unsigned int size)
{
  int length;
  fseek(file, offset, SEEK_SET);
  length = fread(data,1, size, file);
  if(length != size) {
    rt_kprintf("[%s]: read file length error", __func__);
  }
  return length;
}

int wf_os_api_file_readline(wf_file *file, loff_t *offset,
                             unsigned char *data, unsigned int size)
{
  
  int ret;
  int num_read;
  unsigned char *eol;
  
  fseek(file, *offset, SEEK_SET);
  num_read =  fread(data,1, size, file);
  if(num_read <= 0) {
    return 0;
  }
  eol = (unsigned char *)strstr((char const *)data, "\r\n");
  if (eol != NULL) {
    *eol++ = '\0';
    *eol++ = '\0';
    ret = (size_t)(eol - data);
    *offset = ftell(file) - (num_read - ret);
  } else {
    return -1;
  }
  
  return ret;
}

size_t wf_os_api_file_size (wf_file *file)
{
    return fseek(file, 0L, SEEK_END);
}


wf_inline void wf_os_api_file_close (wf_file *file)
{
    fclose(file);
}

