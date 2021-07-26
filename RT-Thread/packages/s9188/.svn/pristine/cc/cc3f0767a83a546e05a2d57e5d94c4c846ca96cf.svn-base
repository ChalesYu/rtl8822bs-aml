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


//#include "bsp.h"
//#include "type.h"
#include "common.h"
#include "crypto.h"
#include "sha1.h"
#include "wpa.h"
#include "random.h"

#define POOL_WORDS 32
#define POOL_WORDS_MASK (POOL_WORDS - 1)
#define POOL_TAP1 26
#define POOL_TAP2 20
#define POOL_TAP3 14
#define POOL_TAP4 7
#define POOL_TAP5 1
#define EXTRACT_LEN 16
#define MIN_READY_MARK 2

static u32 pool[POOL_WORDS];
static unsigned int input_rotate = 0;
static unsigned int pool_pos = 0;
static u8 dummy_key[20];
#ifdef __linux__
static size_t dummy_key_avail = 0;
static int random_fd = -1;
#endif
static unsigned int own_pool_ready = 0;
#define RANDOM_ENTROPY_SIZE 20
static char *random_entropy_file = NULL;
static int random_entropy_file_read = 0;

#define MIN_COLLECT_ENTROPY 1000
static unsigned int entropy = 0;
static unsigned int total_collected = 0;

static void random_write_entropy(void);

static u32 __ROL32(u32 x, u32 y)
{
	return (x << (y & 31)) | (x >> (32 - (y & 31)));
}

static void random_mix_pool(const void *buf, size_t len)
{
	static const u32 twist[8] = {
		0x00000000, 0x3b6e20c8, 0x76dc4190, 0x4db26158,
		0xedb88320, 0xd6d6a3e8, 0x9b64c2b0, 0xa00ae278
	};
	const u8 *pos = buf;
	u32 w;


	while (len--) {
		w = __ROL32(*pos++, input_rotate & 31);
		input_rotate += pool_pos ? 7 : 14;
		pool_pos = (pool_pos - 1) & POOL_WORDS_MASK;
		w ^= pool[pool_pos];
		w ^= pool[(pool_pos + POOL_TAP1) & POOL_WORDS_MASK];
		w ^= pool[(pool_pos + POOL_TAP2) & POOL_WORDS_MASK];
		w ^= pool[(pool_pos + POOL_TAP3) & POOL_WORDS_MASK];
		w ^= pool[(pool_pos + POOL_TAP4) & POOL_WORDS_MASK];
		w ^= pool[(pool_pos + POOL_TAP5) & POOL_WORDS_MASK];
		pool[pool_pos] = (w >> 3) ^ twist[w & 7];
	}
}

static void random_extract(u8 * out)
{
	unsigned int i;
	u8 hash[SHA1_MAC_LEN];
	u32 *hash_ptr;
	u32 buf[POOL_WORDS / 2];

	hmac_sha1(dummy_key, sizeof(dummy_key), (const u8 *)pool,
			  sizeof(pool), hash);
	random_mix_pool(hash, sizeof(hash));

	for (i = 0; i < POOL_WORDS / 2; i++)
		buf[i] = pool[(pool_pos - i) & POOL_WORDS_MASK];
	hmac_sha1(dummy_key, sizeof(dummy_key), (const u8 *)buf, sizeof(buf), hash);

	hash_ptr = (u32 *) hash;
	hash_ptr[0] ^= hash_ptr[4];
	os_memcpy(out, hash, EXTRACT_LEN);
}

void random_add_randomness(const void *buf, size_t len)
{
	struct os_time t;
	static unsigned int count = 0;

	count++;
	if (entropy > MIN_COLLECT_ENTROPY && (count & 0x3ff) != 0) {

		return;
	}
	

	os_get_time(&t);
	
	random_mix_pool(&t, sizeof(t));
	random_mix_pool(buf, len);

	entropy++;
	total_collected++;
}

int random_get_bytes(void *buf, size_t len)
{
	int ret;
	u8 *bytes = buf;
	size_t left;



	ret = os_get_random(buf, len);
	

	left = len;
	while (left) {
		size_t siz, i;
		u8 tmp[EXTRACT_LEN];
		random_extract(tmp);
		
		siz = left > EXTRACT_LEN ? EXTRACT_LEN : left;
		for (i = 0; i < siz; i++)
			*bytes++ ^= tmp[i];
		left -= siz;
	}

#ifdef CONFIG_FIPS

	bytes = buf;
	left = len;
	while (left) {
		size_t siz, i;
		u8 tmp[EXTRACT_LEN];
		if (crypto_get_random(tmp, sizeof(tmp)) < 0) {
			wpa_printf(MSG_ERROR, "random: No entropy available "
					   "for generating strong random bytes");
			return -1;
		}
		wpa_hexdump_key(MSG_EXCESSIVE, "random from crypto module",
						tmp, sizeof(tmp));
		siz = left > EXTRACT_LEN ? EXTRACT_LEN : left;
		for (i = 0; i < siz; i++)
			*bytes++ ^= tmp[i];
		left -= siz;
	}
#endif


	if (entropy < len)
		entropy = 0;
	else
		entropy -= len;

	return ret;
}

int random_pool_ready(void)
{
#ifdef __linux__
	int fd;
	ssize_t res;

	if (dummy_key_avail == sizeof(dummy_key))
		return 1;

	fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		wpa_printf(MSG_ERROR, "random: Cannot open /dev/random: %s",
				   strerror(errno));
		return -1;
	}

	res = read(fd, dummy_key + dummy_key_avail,
			   sizeof(dummy_key) - dummy_key_avail);
	if (res < 0) {
		wpa_printf(MSG_ERROR, "random: Cannot read from /dev/random: "
				   "%s", strerror(errno));
		res = 0;
	}
	wpa_printf(MSG_DEBUG, "random: Got %u/%u bytes from "
			   "/dev/random", (unsigned)res,
			   (unsigned)(sizeof(dummy_key) - dummy_key_avail));
	dummy_key_avail += res;
	close(fd);

	if (dummy_key_avail == sizeof(dummy_key)) {
		if (own_pool_ready < MIN_READY_MARK)
			own_pool_ready = MIN_READY_MARK;
		random_write_entropy();
		return 1;
	}

	wpa_printf(MSG_INFO, "random: Only %u/%u bytes of strong "
			   "random data available from /dev/random",
			   (unsigned)dummy_key_avail, (unsigned)sizeof(dummy_key));

	if (own_pool_ready >= MIN_READY_MARK ||
		total_collected + 10 * own_pool_ready > MIN_COLLECT_ENTROPY) {
		wpa_printf(MSG_INFO, "random: Allow operation to proceed "
				   "based on internal entropy");
		return 1;
	}

	wpa_printf(MSG_INFO, "random: Not enough entropy pool available for "
			   "secure operations");
	return 0;
#else

	return 1;
#endif
}

void random_mark_pool_ready(void)
{
	own_pool_ready++;
	
	random_write_entropy();
}

#ifdef __linux__

static void random_close_fd(void)
{
	if (random_fd >= 0) {
		eloop_unregister_read_sock(random_fd);
		close(random_fd);
		random_fd = -1;
	}
}

static void random_read_fd(int sock, void *eloop_ctx, void *sock_ctx)
{
	ssize_t res;

	if (dummy_key_avail == sizeof(dummy_key)) {
		random_close_fd();
		return;
	}

	res = read(sock, dummy_key + dummy_key_avail,
			   sizeof(dummy_key) - dummy_key_avail);
	if (res < 0) {
		wpa_printf(MSG_ERROR, "random: Cannot read from /dev/random: "
				   "%s", strerror(errno));
		return;
	}

	wpa_printf(MSG_DEBUG, "random: Got %u/%u bytes from /dev/random",
			   (unsigned)res, (unsigned)(sizeof(dummy_key) - dummy_key_avail));
	dummy_key_avail += res;

	if (dummy_key_avail == sizeof(dummy_key)) {
		random_close_fd();
		if (own_pool_ready < MIN_READY_MARK)
			own_pool_ready = MIN_READY_MARK;
		random_write_entropy();
	}
}

#endif

static void random_read_entropy(void)
{
	char *buf;
	size_t len;

	if (!random_entropy_file)
		return;

	buf = os_readfile(random_entropy_file, &len);
	if (buf == NULL)
		return;

	if (len != 1 + RANDOM_ENTROPY_SIZE) {
		
		os_free(buf);
		return;
	}

	own_pool_ready = (u8) buf[0];
	random_add_randomness(buf + 1, RANDOM_ENTROPY_SIZE);
	random_entropy_file_read = 1;
	os_free(buf);
	
}

static void random_write_entropy(void)
{
	char buf[RANDOM_ENTROPY_SIZE];
	FILE *f;
	u8 opr;
	int fail = 0;

	if (!random_entropy_file)
		return;

	if (random_get_bytes(buf, RANDOM_ENTROPY_SIZE) < 0)
		return;

	f = fopen(random_entropy_file, "wb");
	if (f == NULL) {
		
		return;
	}

	opr = own_pool_ready > 0xff ? 0xff : own_pool_ready;
	if (fwrite(&opr, 1, 1, f) != 1 ||
		fwrite(buf, RANDOM_ENTROPY_SIZE, 1, f) != 1)
		fail = 1;
	fclose(f);
	if (fail) {
		
		return;
	}

	
}

void random_init(const char *entropy_file)
{
	os_free(random_entropy_file);
	if (entropy_file)
		random_entropy_file = os_strdup(entropy_file);
	else
		random_entropy_file = NULL;
	random_read_entropy();

#ifdef __linux__
	if (random_fd >= 0)
		return;

	random_fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
	if (random_fd < 0) {
		wpa_printf(MSG_ERROR, "random: Cannot open /dev/random: %s",
				   strerror(errno));
		return;
	}
	wpa_printf(MSG_DEBUG, "random: Trying to read entropy from " "/dev/random");

	eloop_register_read_sock(random_fd, random_read_fd, NULL, NULL);
#endif

	random_write_entropy();
}

void random_deinit(void)
{
#ifdef __linux__
	random_close_fd();
#endif
	random_write_entropy();
	os_free(random_entropy_file);
	random_entropy_file = NULL;
}
