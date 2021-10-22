#include "wf_os_api.h"
#include "os.h"
#include "crypto.h"

int wf_os_get_random(unsigned char *buf, size_t len)
{
  int i, j;
  int random_num;
  char tmp[16];
  size_t keylen = 4;
  const wf_u8 *addr = (const wf_u8 *)&random_num;
  random_num = wf_os_api_timestamp();
  
  wf_md5_vector(1, &addr, (const size_t *)&keylen, (wf_u8 *) tmp);
  for (i = 0, j = 0; i < len; i++) {
    buf[i] = tmp[j++];
    if (j >= 16) {
      random_num++;
      wf_md5_vector(1, &addr, (const size_t *)&keylen, (wf_u8 *) tmp);
      j = 0;
    }
  }
  return 0;
}


int wf_os_get_reltime(struct os_reltime *t)
{
  wf_u32 tick, ms;
  
  tick = wf_os_api_timestamp();
  ms = (tick * 1000)/ WF_HZ;
  t->sec = ms / 1000;
  t->usec = (ms % 1000) * 1000;
  
  return 0;
}


void wf_os_bin_clear_free(void *bin, size_t len)
{
  if (bin) {
    os_memset(bin, 0, len);
    os_free(bin);
  }
}


/**
* wf_os_inc_byte_array - Increment arbitrary length byte array by one
* @counter: Pointer to byte array
* @len: Length of the counter in bytes
*
* This function increments the last byte of the counter by one and continues
* rolling over to more significant bytes if the byte was incremented from
* 0xff to 0x00.
*/
void wf_os_inc_byte_array(wf_u8 *counter, size_t len)
{
  int pos = len - 1;
  while (pos >= 0) {
    counter[pos]++;
    if (counter[pos] != 0)
      break;
    pos--;
  }
}


int wf_os_memcmp_const(const void *a, const void *b, size_t len)
{
  const wf_u8 *aa = a;
  const wf_u8 *bb = b;
  size_t i;
  wf_u8 res;
  
  for (res = 0, i = 0; i < len; i++)
    res |= aa[i] ^ bb[i];
  
  return res;
}


