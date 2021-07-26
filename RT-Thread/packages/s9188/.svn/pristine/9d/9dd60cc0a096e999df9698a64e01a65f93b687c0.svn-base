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


#include "includes.h"

#include "common.h"
#include "crypto/aes_wrap.h"
#include "milenage.h"

int milenage_f1(const u8 * opc, const u8 * k, const u8 * _rand,
				const u8 * sqn, const u8 * amf, u8 * mac_a, u8 * mac_s)
{
	u8 tmp1[16], tmp2[16], tmp3[16];
	int i;

	for (i = 0; i < 16; i++)
		tmp1[i] = _rand[i] ^ opc[i];
	if (aes_128_encrypt_block(k, tmp1, tmp1))
		return -1;

	os_memcpy(tmp2, sqn, 6);
	os_memcpy(tmp2 + 6, amf, 2);
	os_memcpy(tmp2 + 8, tmp2, 8);

	for (i = 0; i < 16; i++)
		tmp3[(i + 8) % 16] = tmp2[i] ^ opc[i];
	for (i = 0; i < 16; i++)
		tmp3[i] ^= tmp1[i];

	if (aes_128_encrypt_block(k, tmp3, tmp1))
		return -1;
	for (i = 0; i < 16; i++)
		tmp1[i] ^= opc[i];
	if (mac_a)
		os_memcpy(mac_a, tmp1, 8);
	if (mac_s)
		os_memcpy(mac_s, tmp1 + 8, 8);
	return 0;
}

int milenage_f2345(const u8 * opc, const u8 * k, const u8 * _rand,
				   u8 * res, u8 * ck, u8 * ik, u8 * ak, u8 * akstar)
{
	u8 tmp1[16], tmp2[16], tmp3[16];
	int i;

	for (i = 0; i < 16; i++)
		tmp1[i] = _rand[i] ^ opc[i];
	if (aes_128_encrypt_block(k, tmp1, tmp2))
		return -1;

	for (i = 0; i < 16; i++)
		tmp1[i] = tmp2[i] ^ opc[i];
	tmp1[15] ^= 1;
	if (aes_128_encrypt_block(k, tmp1, tmp3))
		return -1;
	for (i = 0; i < 16; i++)
		tmp3[i] ^= opc[i];
	if (res)
		os_memcpy(res, tmp3 + 8, 8);
	if (ak)
		os_memcpy(ak, tmp3, 6);

	if (ck) {
		for (i = 0; i < 16; i++)
			tmp1[(i + 12) % 16] = tmp2[i] ^ opc[i];
		tmp1[15] ^= 2;
		if (aes_128_encrypt_block(k, tmp1, ck))
			return -1;
		for (i = 0; i < 16; i++)
			ck[i] ^= opc[i];
	}

	if (ik) {
		for (i = 0; i < 16; i++)
			tmp1[(i + 8) % 16] = tmp2[i] ^ opc[i];
		tmp1[15] ^= 4;
		if (aes_128_encrypt_block(k, tmp1, ik))
			return -1;
		for (i = 0; i < 16; i++)
			ik[i] ^= opc[i];
	}

	if (akstar) {
		for (i = 0; i < 16; i++)
			tmp1[(i + 4) % 16] = tmp2[i] ^ opc[i];
		tmp1[15] ^= 8;
		if (aes_128_encrypt_block(k, tmp1, tmp1))
			return -1;
		for (i = 0; i < 6; i++)
			akstar[i] = tmp1[i] ^ opc[i];
	}

	return 0;
}

void milenage_generate(const u8 * opc, const u8 * amf, const u8 * k,
					   const u8 * sqn, const u8 * _rand, u8 * autn, u8 * ik,
					   u8 * ck, u8 * res, size_t * res_len)
{
	int i;
	u8 mac_a[8], ak[6];

	if (*res_len < 8) {
		*res_len = 0;
		return;
	}
	if (milenage_f1(opc, k, _rand, sqn, amf, mac_a, NULL) ||
		milenage_f2345(opc, k, _rand, res, ck, ik, ak, NULL)) {
		*res_len = 0;
		return;
	}
	*res_len = 8;

	for (i = 0; i < 6; i++)
		autn[i] = sqn[i] ^ ak[i];
	os_memcpy(autn + 6, amf, 2);
	os_memcpy(autn + 8, mac_a, 8);
}

int milenage_auts(const u8 * opc, const u8 * k, const u8 * _rand,
				  const u8 * auts, u8 * sqn)
{
	u8 amf[2] = { 0x00, 0x00 };
	u8 ak[6], mac_s[8];
	int i;

	if (milenage_f2345(opc, k, _rand, NULL, NULL, NULL, NULL, ak))
		return -1;
	for (i = 0; i < 6; i++)
		sqn[i] = auts[i] ^ ak[i];
	if (milenage_f1(opc, k, _rand, sqn, amf, NULL, mac_s) ||
		os_memcmp_const(mac_s, auts + 6, 8) != 0)
		return -1;
	return 0;
}

int gsm_milenage(const u8 * opc, const u8 * k, const u8 * _rand, u8 * sres,
				 u8 * kc)
{
	u8 res[8], ck[16], ik[16];
	int i;

	if (milenage_f2345(opc, k, _rand, res, ck, ik, NULL, NULL))
		return -1;

	for (i = 0; i < 8; i++)
		kc[i] = ck[i] ^ ck[i + 8] ^ ik[i] ^ ik[i + 8];

#ifdef GSM_MILENAGE_ALT_SRES
	os_memcpy(sres, res, 4);
#else
	for (i = 0; i < 4; i++)
		sres[i] = res[i] ^ res[i + 4];
#endif
	return 0;
}

int milenage_check(const u8 * opc, const u8 * k, const u8 * sqn,
				   const u8 * _rand, const u8 * autn, u8 * ik, u8 * ck,
				   u8 * res, size_t * res_len, u8 * auts)
{
	int i;
	u8 mac_a[8], ak[6], rx_sqn[6];
	const u8 *amf;

	if (milenage_f2345(opc, k, _rand, res, ck, ik, ak, NULL))
		return -1;

	*res_len = 8;

	for (i = 0; i < 6; i++)
		rx_sqn[i] = autn[i] ^ ak[i];

	if (os_memcompare(rx_sqn, sqn, 6) <= 0) {
		u8 auts_amf[2] = { 0x00, 0x00 };
		if (milenage_f2345(opc, k, _rand, NULL, NULL, NULL, NULL, ak))
			return -1;
		for (i = 0; i < 6; i++)
			auts[i] = sqn[i] ^ ak[i];
		if (milenage_f1(opc, k, _rand, sqn, auts_amf, NULL, auts + 6))
			return -1;
		return -2;
	}

	amf = autn + 6;
	if (milenage_f1(opc, k, _rand, rx_sqn, amf, mac_a, NULL))
		return -1;


	if (os_memcmp_const(mac_a, autn + 8, 8) != 0) {
	
		return -1;
	}

	return 0;
}
