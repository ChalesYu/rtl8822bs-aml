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
#include "wpa.h"
#include "sha256.h"
#include "sha256_i.h"
#include "crypto.h"

int sha256_vect(size_t num_elem, const u8 * addr[], const size_t * len,
				u8 * mac)
{
	struct sha256_state_tk ctx;
	size_t i;

	_init_sha256(&ctx);
	for (i = 0; i < num_elem; i++)
		if (sha256_handle(&ctx, addr[i], len[i]))
			return -1;
	if (done_sha256(&ctx, mac))
		return -1;
	return 0;
}

static const unsigned long K[64] = {
	0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
	0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
	0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
	0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
	0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
	0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
	0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
	0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
	0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
	0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
	0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
	0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
	0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};


#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))
#define S(x, n)         RORc((x), (n))
#define R(x, n)         (((x)&0xFFFFFFFFUL)>>(n))
#define Sig_ma0(x)       (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sig_ma1(x)       (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define GAMMA0(x)       (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define GAMMA1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

static int compress_sha256(struct sha256_state_tk *md, unsigned char *buf)
{
	u32 S[8], W[64], t0, t1;
	u32 t;
	int i;

	for (i = 0; i < 8; i++) {
		S[i] = md->state[i];
	}

	for (i = 0; i < 16; i++)
		W[i] = TILK_WPA_READ_BE32(buf + (4 * i));

	for (i = 16; i < 64; i++) {
		W[i] = GAMMA1(W[i - 2]) + W[i - 7] + GAMMA0(W[i - 15]) + W[i - 16];
	}

#define RND(a,b,c,d,e,f,g,h,i)                          \
	t0 = h + Sig_ma1(e) + Ch(e, f, g) + K[i] + W[i];	\
	t1 = Sig_ma0(a) + Maj(a, b, c);			\
	d += t0;					\
	h  = t0 + t1;

	for (i = 0; i < 64; ++i) {
		RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i);
		t = S[7];
		S[7] = S[6];
		S[6] = S[5];
		S[5] = S[4];
		S[4] = S[3];
		S[3] = S[2];
		S[2] = S[1];
		S[1] = S[0];
		S[0] = t;
	}

	for (i = 0; i < 8; i++) {
		md->state[i] = md->state[i] + S[i];
	}
	return 0;
}

void _init_sha256(struct sha256_state_tk *md)
{
	md->curlen = 0;
	md->length = 0;
	md->state[0] = 0x6A09E667UL;
	md->state[1] = 0xBB67AE85UL;
	md->state[2] = 0x3C6EF372UL;
	md->state[3] = 0xA54FF53AUL;
	md->state[4] = 0x510E527FUL;
	md->state[5] = 0x9B05688CUL;
	md->state[6] = 0x1F83D9ABUL;
	md->state[7] = 0x5BE0CD19UL;
}

int sha256_handle(struct sha256_state_tk *md, const unsigned char *in,
				  unsigned long inlen)
{
	unsigned long n;

	if (md->curlen >= sizeof(md->buf))
		return -1;

	while (inlen > 0) {
		if (md->curlen == 0 && inlen >= SHA256_BLOCK_SIZE) {
			if (compress_sha256(md, (unsigned char *)in) < 0)
				return -1;
			md->length += SHA256_BLOCK_SIZE * 8;
			in += SHA256_BLOCK_SIZE;
			inlen -= SHA256_BLOCK_SIZE;
		} else {
			n = MIN(inlen, (SHA256_BLOCK_SIZE - md->curlen));
			os_memcpy(md->buf + md->curlen, in, n);
			md->curlen += n;
			in += n;
			inlen -= n;
			if (md->curlen == SHA256_BLOCK_SIZE) {
				if (compress_sha256(md, md->buf) < 0)
					return -1;
				md->length += 8 * SHA256_BLOCK_SIZE;
				md->curlen = 0;
			}
		}
	}

	return 0;
}

int done_sha256(struct sha256_state_tk *md, unsigned char *out)
{
	int i;

	if (md->curlen >= sizeof(md->buf))
		return -1;

	md->length += md->curlen * 8;

	md->buf[md->curlen++] = (unsigned char)0x80;

	if (md->curlen > 56) {
		while (md->curlen < SHA256_BLOCK_SIZE) {
			md->buf[md->curlen++] = (unsigned char)0;
		}
		compress_sha256(md, md->buf);
		md->curlen = 0;
	}

	while (md->curlen < 56) {
		md->buf[md->curlen++] = (unsigned char)0;
	}

	TILK_WPA_TO_BE64(md->buf + 56, md->length);
	compress_sha256(md, md->buf);

	for (i = 0; i < 8; i++)
		TILK_WPA_TO_BE32(out + (4 * i), md->state[i]);

	return 0;
}
