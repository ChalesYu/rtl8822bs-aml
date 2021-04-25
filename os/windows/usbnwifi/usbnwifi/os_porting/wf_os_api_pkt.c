#include <time.h>
#include <Ntifs.h>
#include "wf_os_api.h"
#include "wf_debug.h"

wf_inline wf_u8 *wf_os_api_pkt_data(wf_pkt *pkt)
{
	//return pkt->data;
}

wf_inline void wf_os_api_pktfile_open(struct pkt_file *pfile, wf_pkt *pktptr)
{
	pfile->pkt = pktptr;
	//pfile->cur_addr = pfile->buf_start = pktptr->data;
	//pfile->pkt_len = pfile->buf_len = pktptr->len;

	pfile->cur_buffer = pfile->buf_start;
}

wf_inline wf_u32 wf_os_api_read_pktfile(struct pkt_file *pfile, wf_u8 *buffer, wf_u32 buf_len)
{
	wf_u32 len = 0;

	/* get remainder length first */
	len = (pfile->buf_len - ((SIZE_PTR)(pfile->cur_addr) - (SIZE_PTR)(pfile->buf_start)));
	len = (buf_len > len) ? len : buf_len;

	if (buffer)
	{
		//skb_copy_bits(pfile->pkt, pfile->buf_len - pfile->pkt_len, buffer, len);
	}

	pfile->cur_addr += len;
	pfile->pkt_len -= len;

	return len;
}

wf_inline wf_bool wf_os_api_pktfile_is_end(struct pkt_file *pfile)
{
	if (pfile->pkt_len == 0)
	{
		return wf_true;
	}

	return wf_false;
}

void wf_os_api_do_finish_pkt(struct net_device *ndev, wf_pkt *pkt, wf_bool bWakeupQue)
{

}
