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


#ifndef MILENAGE_H
#define MILENAGE_H

void milenage_generate(const u8 * opc, const u8 * amf, const u8 * k,
					   const u8 * sqn, const u8 * _rand, u8 * autn, u8 * ik,
					   u8 * ck, u8 * res, size_t * res_len);
int milenage_auts(const u8 * opc, const u8 * k, const u8 * _rand,
				  const u8 * auts, u8 * sqn);
int gsm_milenage(const u8 * opc, const u8 * k, const u8 * _rand, u8 * sres,
				 u8 * kc);
int milenage_check(const u8 * opc, const u8 * k, const u8 * sqn,
				   const u8 * _rand, const u8 * autn, u8 * ik, u8 * ck,
				   u8 * res, size_t * res_len, u8 * auts);
int milenage_f1(const u8 * opc, const u8 * k, const u8 * _rand, const u8 * sqn,
				const u8 * amf, u8 * mac_a, u8 * mac_s);
int milenage_f2345(const u8 * opc, const u8 * k, const u8 * _rand, u8 * res,
				   u8 * ck, u8 * ik, u8 * ak, u8 * akstar);

#endif
