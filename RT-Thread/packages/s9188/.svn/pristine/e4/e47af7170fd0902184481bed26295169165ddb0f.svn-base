#include "wf_os_api.h"
#include "os.h"

int os_get_random(unsigned char *buf, size_t len)
{
  int i, j;
  int random_num;
  char tmp[16];
  int keylen = 4;
  const char *addr = (const char *)&random_num;
  random_num = wf_os_api_timestamp();
  
  md5_vector(1, &addr, (const int *)&keylen, (char *) tmp);
  for (i = 0, j = 0; i < len; i++) {
    buf[i] = tmp[j++];
    if (j >= 16) {
      random_num++;
      md5_vector(1, &addr, (const int *)&keylen, (char *) tmp);
      j = 0;
    }
  }
  return 0;
}


int os_get_reltime(struct os_reltime *t)
{
  wf_u32 tick, ms;
  
  tick = wf_os_api_timestamp();
  ms = (tick * 1000)/ WF_HZ;
  t->sec = ms / 1000;
  t->usec = (ms % 1000) * 1000;
  
  return 0;
}


void bin_clear_free(void *bin, size_t len)
{
	if (bin) {
		os_memset(bin, 0, len);
		os_free(bin);
	}
}


/**
 * inc_byte_array - Increment arbitrary length byte array by one
 * @counter: Pointer to byte array
 * @len: Length of the counter in bytes
 *
 * This function increments the last byte of the counter by one and continues
 * rolling over to more significant bytes if the byte was incremented from
 * 0xff to 0x00.
 */
void inc_byte_array(u8 *counter, size_t len)
{
	int pos = len - 1;
	while (pos >= 0) {
		counter[pos]++;
		if (counter[pos] != 0)
			break;
		pos--;
	}
}


int os_memcmp_const(const void *a, const void *b, size_t len)
{
	const u8 *aa = a;
	const u8 *bb = b;
	size_t i;
	u8 res;

	for (res = 0, i = 0; i < len; i++)
		res |= aa[i] ^ bb[i];

	return res;
}


