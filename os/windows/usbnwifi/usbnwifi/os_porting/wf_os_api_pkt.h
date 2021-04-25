#pragma once
#ifndef __WF_OS_API_PKT_H__
typedef int wf_pkt;
struct pkt_file
{
    wf_pkt *pkt;
    int pkt_len;
    wf_u8 *cur_buffer;
    wf_u8 *buf_start;
    wf_u8 *cur_addr;
    int buf_len;
};

wf_u8  *wf_os_api_pkt_data(wf_pkt *pkt);
void    wf_os_api_pktfile_open(struct pkt_file *pfile, wf_pkt *pktptr);
wf_u32  wf_os_api_read_pktfile(struct pkt_file *pfile, wf_u8 *buffer, wf_u32 buf_len);
wf_bool wf_os_api_pktfile_is_end(struct pkt_file *pfile);
void    wf_os_api_do_finish_pkt(struct net_device *ndev, wf_pkt *pkt, wf_bool bWakeupQue);

#endif // !__WF_OS_API_PKT_H__
