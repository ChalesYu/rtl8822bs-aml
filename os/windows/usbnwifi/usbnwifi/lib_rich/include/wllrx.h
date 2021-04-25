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

#ifndef __wlan9086X_RECV_H__
#define __wlan9086X_RECV_H__

#ifndef MAX_RECVBUF_SZ
	#ifdef CONFIG_MINIMAL_MEMORY_USAGE
		#define MAX_RECVBUF_SZ (4000)
	#else
		#ifdef CONFIG_PLATFORM_IPC
			#ifdef CONFIG_LOWMEM
				#define MAX_RECVBUF_SZ  (12288)
			#else
				#define MAX_RECVBUF_SZ (16384)
			#endif
		#else
			#define MAX_RECVBUF_SZ (32768)
		#endif
	#endif
#endif

//void Func_Of_Query_Rx_Desc_Status(union recv_frame *precvframe, u8 * pdesc);
//for compile
#endif
