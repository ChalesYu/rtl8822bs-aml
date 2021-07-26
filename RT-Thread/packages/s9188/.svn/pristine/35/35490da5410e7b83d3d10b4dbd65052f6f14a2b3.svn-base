#include "wf_debug.h"
#include "iw_priv_func.h"
#include "mp.h"

#ifdef CONFIG_MP_MODE
#if defined(CONFIG_WEXT_PRIV)
extern wf_bool mp_test_enable;

#if 0
#define MP_DBG(fmt, ...)        LOG_D("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define MP_ARRAY(data, len)     log_array(data, len)
#else
#define MP_DBG(fmt, ...)
#define MP_ARRAY(data, len)
#endif
#define MP_INFO(fmt, ...)       LOG_I("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define MP_WARN(fmt, ...)       LOG_W("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define MP_ERROR(fmt, ...)      LOG_E("[%s:%d]"fmt, __func__, __LINE__, ##__VA_ARGS__)

#define RCR_APPFCS				BIT(31)
#define RCR_APP_MIC				BIT(30)
#define RCR_APP_ICV				BIT(29)
#define RCR_APP_PHYST_RXFF		BIT(28)
#define RCR_HTC_LOC_CTRL		BIT(14)
#define RCR_AMF					BIT(13)
#define RCR_ADF					BIT(11)
#define RCR_ACF                 BIT(12)
#define RCR_ACRC32				BIT(8)
#define RCR_CBSSID_BCN			BIT(7)
#define RCR_CBSSID_DATA		    BIT(6)
#define RCR_APWRMGT             BIT(5)
#define RCR_AB					BIT(3)
#define RCR_AM					BIT(2)
#define RCR_APM					BIT(1)
#define RCR_AAP					BIT(0)

#define N_BYTE_ALIGMENT(value, aligment) ((aligment == 1) ? (value) : (((value + aligment - 1) / aligment) * aligment))


wf_u8 mac_test[6] = {0xc8,0x2e,0x47,0xbc,0xbb,0xdf};

int wf_mp_rate_to_rateidx(wf_u32 rate)
{
	int ret_rate = MGN_1M;

	switch (rate) {
	case WL_MGN_1M:
		ret_rate = DESC_RATE1M;
		break;
	case WL_MGN_2M:
		ret_rate = DESC_RATE2M;
		break;
	case WL_MGN_5_5M:
		ret_rate = DESC_RATE5_5M;
		break;
	case WL_MGN_6M:
		ret_rate = DESC_RATE6M;
		break;
	case WL_MGN_11M:
		ret_rate = DESC_RATE11M;
		break;
	case WL_MGN_9M:
		ret_rate = DESC_RATE9M;
		break;
	case WL_MGN_12M:
		ret_rate = DESC_RATE12M;
		break;
	case WL_MGN_18M:
		ret_rate = DESC_RATE18M;
		break;
	case WL_MGN_24M:
		ret_rate = DESC_RATE24M;
		break;
	case WL_MGN_36M:
		ret_rate = DESC_RATE36M;
		break;
	case WL_MGN_48M:
		ret_rate = DESC_RATE48M;
		break;
	case WL_MGN_54M:
		ret_rate = DESC_RATE54M;
		break;
	case WL_MGN_MCS0:
		ret_rate = DESC_RATEMCS0;
		break;
	case WL_MGN_MCS1:
		ret_rate = DESC_RATEMCS1;
		break;
	case WL_MGN_MCS2:
		ret_rate = DESC_RATEMCS2;
		break;
	case WL_MGN_MCS3:
		ret_rate = DESC_RATEMCS3;
		break;
	case WL_MGN_MCS4:
		ret_rate = DESC_RATEMCS4;
		break;
	case WL_MGN_MCS5:
		ret_rate = DESC_RATEMCS5;
		break;
	case WL_MGN_MCS6:
		ret_rate = DESC_RATEMCS6;
		break;
	case WL_MGN_MCS7:
		ret_rate = DESC_RATEMCS7;
		break;

	}
	return ret_rate;

}

static wf_u32 mp_random32(void)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
	return prandom_u32();
#elif (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18))
	wf_u32 random_int;
	get_random_bytes(&random_int, 4);
	return random_int;
#else
	return random32();
#endif
}

static wf_u8 mp_trx_key_of_char2num_func(wf_u8 ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	else if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else
		return 0xff;
}

static wf_u8 mp_trx_key_of_2char2num_func(wf_u8 hch, wf_u8 lch)
{
	return ((mp_trx_key_of_char2num_func(hch) << 4) | mp_trx_key_of_char2num_func(lch));
}


static wf_u32 mp_tx_destaddr(nic_info_st *pnic_info,wf_u8 *extra)
{
    //struct pkt_attrib *pattrib;
	wf_u32 jj, kk = 0;

	if (strlen(extra) < 5)
		return WF_RETURN_FAIL;

    for (jj = 0, kk = 0; jj < 6; jj++, kk += 3)
    	{}
		//pmp_info->dst[jj] = mp_trx_key_of_2char2num_func(extra[kk], extra[kk + 1]);
    return 0;
}

static void mp_tx_desc_fill(struct xmit_frame *pxmitframe, wf_u8 *pbuf, wf_bool bSendAck)
{
     nic_info_st *nic_info = pxmitframe->nic_info;
     wf_mp_info_st *mp_info = nic_info->mp_info;

#ifdef CONFIG_RICHV200
     wf_memcpy(pbuf, mp_info->tx.desc, TXDESC_SIZE);

     /* set for data type */
     wf_set_bits_to_le_u32(pbuf, 0, 2, TYPE_DATA);

     /* set PKT_LEN */
     wf_set_bits_to_le_u32(pbuf + 8, 0, 16, pxmitframe->last_txcmdsz);

     /* set HWSEQ_EN */
     wf_set_bits_to_le_u32(pbuf, 18, 1, 1);
	 /*set bmc*/
	 wf_set_bits_to_le_u32(pbuf + 12, 14, 1, 1);

#else
     wf_memcpy(pbuf, mp_info->tx.desc, TXDESC_SIZE);

     WF_TX_DESC_PKT_SIZE_9086X(pbuf, pxmitframe->last_txcmdsz);

     WF_TX_DESC_BMC_9086X(pbuf, 1);

     WF_TX_DESC_HWSEQ_EN_9086X(pbuf, 1);
#endif

}


static wf_bool mp_tx_desc_update(struct xmit_frame *pxmitframe, wf_u8 *pbuf)
{

     mp_tx_desc_fill(pxmitframe, pbuf, wf_false);

#ifdef CONFIG_RICHV200
     wf_txdesc_chksum(pbuf);
#else
     wf_txdesc_chksum((struct tx_desc *)pbuf);
#endif

    return wf_false;
}

static wf_bool mp_tx_send_complete_cb(nic_info_st *nic_info, struct xmit_buf *pxmitbuf)
{
    tx_info_st *tx_info = nic_info->tx_info;

    //LOG_D("mp_tx_send_complete_cb pxmitbuf:%p",pxmitbuf);
    wf_xmit_buf_delete(tx_info, pxmitbuf);

    return wf_true;
}




static wf_bool mp_tx_sending_queue(nic_info_st *nic_info, struct xmit_frame *pxmitframe, wf_bool ack)
{
    wf_u8 *mem_addr;
    wf_u32 ff_hwaddr;
    wf_bool bRet = wf_true;
    int ret;
    wf_bool inner_ret = wf_true;
    wf_bool blast = wf_false;
    int t, sz, w_sz, pull = 0;
    struct xmit_buf *pxmitbuf = pxmitframe->pxmitbuf;
    hw_info_st *hw_info = nic_info->hw_info;
    wf_u32  txlen = 0;
	//int i=0;

    mem_addr = pxmitframe->buf_addr;

    for (t = 0; t < pxmitframe->nr_frags; t++)
        {
         if (inner_ret != wf_true && ret == wf_true)
             ret = wf_false;
         if (t != (pxmitframe->nr_frags - 1))
         {
             LOG_D("pattrib->nr_frags=%d\n", pxmitframe->nr_frags);
             sz = hw_info->frag_thresh;
             sz = sz - 4 - 0; /* 4: wlan head filed????????? */
         }
         else
         {
             /* no frag */
             blast = wf_true;
             sz = pxmitframe->last_txcmdsz;
         }

         pull = mp_tx_desc_update(pxmitframe, mem_addr);
         if (pull)
         {
             mem_addr += PACKET_OFFSET_SZ; /* pull txdesc head */
             pxmitframe->buf_addr = mem_addr;
             w_sz = sz + TXDESC_SIZE;
         }
         else
         {
             w_sz = sz + TXDESC_SIZE + PACKET_OFFSET_SZ;
         }

		//for(i=0;i<60;i++)
		{
			//LOG_D("0x%x",*(mem_addr+i));
			
		}
		//LOG_E("txd end!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
         ff_hwaddr = wf_quary_addr(pxmitframe->qsel);

         txlen = TXDESC_SIZE + pxmitframe->last_txcmdsz;
         pxmitbuf->pg_num   += (txlen+127)/128;
         wf_timer_set(&pxmitbuf->time, 0);
         if(blast)
         {
             ret = wf_io_write_data(nic_info, 1, mem_addr, w_sz,
                                           ff_hwaddr,(void *)mp_tx_send_complete_cb, nic_info, pxmitbuf);
         }
         else
         {
             ret = wf_io_write_data(nic_info, 1, mem_addr, w_sz,
                                           ff_hwaddr, NULL, nic_info, pxmitbuf);
         }

         if (WF_RETURN_FAIL == ret)
         {
             bRet = wf_false;
             break;
         }

         wf_tx_stats_cnt(nic_info, pxmitframe, sz);

         mem_addr += w_sz;
         mem_addr = (wf_u8 *) WF_RND4(((SIZE_PTR) (mem_addr)));
    }

 return bRet;
}



static int mp_xmit_packet_thread(void *nic_info)
{
    nic_info_st *pnic_info = (nic_info_st *)nic_info;
	struct xmit_frame *xmitframe;
    struct xmit_buf *xmitBuf;
	wf_list_t list;
	wf_mp_info_st *pmp_priv;
	tx_info_st *pxmitpriv;
    int ret = wf_false;
    wf_bool bufAlocFailed = wf_false;
    pmp_priv = pnic_info->mp_info;
	pxmitpriv = (tx_info_st *)pnic_info->tx_info;

    LOG_I("[%s]: mp->tx.count: %d, mp->tx.stop:%d", __func__, pmp_priv->tx.count, pmp_priv->tx.stop);
	while (1)
    {
        if ((pnic_info->is_driver_stopped == wf_true) || (pnic_info->is_surprise_removed == wf_true))
        {
		    pmp_priv->tx.stop = 1;
        }

		if ((pmp_priv->tx.count != 0) && (pmp_priv->tx.sended >= pmp_priv->tx.count))
        {
            pmp_priv->tx.stop = 1;
        }

		if (pmp_priv->tx.stop == 1)
        {
			break;
        }

        xmitBuf = wf_xmit_buf_new(pxmitpriv);
        if (xmitBuf != NULL)
        {
            xmitframe = wf_xmit_frame_new(pxmitpriv);
            if (xmitframe != NULL)
            {
                bufAlocFailed = wf_false;
            }
            else
            {
                wf_xmit_buf_delete(pxmitpriv, xmitBuf);
                xmitBuf = NULL;

                bufAlocFailed = wf_true;
            }
        }
        else
        {
            bufAlocFailed = wf_true;
        }

        if (bufAlocFailed == wf_true)
        {
            wf_msleep(1);
			continue;
        }
		list = xmitframe->list;
        wf_memcpy((xmitframe), &(pmp_priv->tx.attrib), sizeof(struct xmit_frame));
		xmitframe->list = list;
        xmitframe->frame_tag = MP_FRAMETAG;
        xmitframe->pxmitbuf = xmitBuf;
        xmitframe->buf_addr = xmitBuf->pbuf;
        xmitBuf->priv_data = xmitframe;

		wf_memcpy((wf_u8 *) (xmitBuf->pbuf + TXDESC_OFFSET), pmp_priv->tx.buf, pmp_priv->tx.write_size);
        xmitBuf->pkt_len = pmp_priv->tx.write_size;
        //wf_memcpy(&(xmitframe->attrib), &(pmp_priv->tx.attrib), sizeof(struct pkt_attrib));
        ret = mp_tx_sending_queue(pnic_info, xmitframe, wf_false);
        if (wf_false == ret)
        {
            LOG_I("mp_tx_sending_queue...failed");
            wf_xmit_buf_delete(pxmitpriv, xmitBuf);
        } else {
            pmp_priv->tx.sended++;
            pmp_priv->tx_pktcount++;
            // LOG_I("mp_tx_sending_queue...OK, count is %d", pmp_priv->tx.sended);
        }
        wf_xmit_frame_delete(pxmitpriv, xmitframe);
        
		wf_msleep(pmp_priv->pktInterval);
	}

    LOG_I("[%s]: mp tx test ok, send count is %d", __func__, pmp_priv->tx.sended);
	wf_kfree(pmp_priv->tx.pallocated_buf);
	pmp_priv->tx.pallocated_buf = NULL;
    complete_and_exit(NULL, 0);
}


static void mp_test_fill_tx_desc(nic_info_st *pnic_info)
{
     wf_mp_info_st *pmp_priv = pnic_info->mp_info;
     struct xmit_frame *pattrib = &(pmp_priv->tx.attrib);
     u8 *ptxdesc = pmp_priv->tx.desc;

	 
#ifndef CONFIG_RICHV200

     WF_TX_DESC_AGG_BREAK_9086X(ptxdesc, 1);
     WF_TX_DESC_MACID_9086X(ptxdesc, 0);
     WF_TX_DESC_QUEUE_SEL_9086X(ptxdesc, pattrib->qsel);

     if (pmp_priv->bandwidth == CHANNEL_WIDTH_20)
        WF_TX_DESC_RATE_ID_9086X(ptxdesc, RATEID_IDX_BGN_20M_1SS_BN);
     else
        WF_TX_DESC_RATE_ID_9086X(ptxdesc, RATEID_IDX_BGN_40M_1SS);

     WF_TX_DESC_SEQ_9086X(ptxdesc, pattrib->seqnum);
     WF_TX_DESC_HWSEQ_EN_9086X(ptxdesc, 1);
     WF_TX_DESC_USE_RATE_9086X(ptxdesc, 1);

     if (pmp_priv->preamble)
     {
         if (pmp_priv->rateidx <= DESC_RATE54M)
             WF_TX_DESC_DATA_SHORT_9086X(ptxdesc, 1);
     }

     if (pmp_priv->bandwidth == CHANNEL_WIDTH_40)
         WF_TX_DESC_DATA_BW_9086X(ptxdesc, 1);

     WF_TX_DESC_TX_RATE_9086X(ptxdesc, pmp_priv->rateidx);
#else
	/*set mac id*/
	 wf_set_bits_to_le_u32(ptxdesc + 16, 0, 5, 0);

	 /* set TX RATE */
	 wf_set_bits_to_le_u32(ptxdesc + 8, 18, 7, pmp_priv->rateidx);
	 LOG_D("pmp_priv->rateidx:0x%x",pmp_priv->rateidx);
	/* set USE_RATE */
	wf_set_bits_to_le_u32(ptxdesc + 8, 16, 1, 1);
	/* set SEQ */
	wf_set_bits_to_le_u32(ptxdesc, 19, 12, pattrib->seqnum);
	if (pmp_priv->bandwidth == CHANNEL_WIDTH_40)
	{
    	/* set DBW */
		wf_set_bits_to_le_u32(ptxdesc + 16, 12, 1, CHANNEL_WIDTH_40);
		/* set RATE ID, mgmt frame use 802.11 B, the number is 0 */
		wf_set_bits_to_le_u32(ptxdesc + 16, 6, 3, RATEID_IDX_BGN_20M_1SS_BN);
	}
	else
	{
		/* set DBW */
		wf_set_bits_to_le_u32(ptxdesc + 16, 12, 1, CHANNEL_WIDTH_20);
		/* set RATE ID, mgmt frame use 802.11 B, the number is 0 */
		wf_set_bits_to_le_u32(ptxdesc + 16, 6, 3, RATEID_IDX_BGN_40M_1SS);
	}

	 if (pmp_priv->preamble)
     {
         if (pmp_priv->rateidx <= DESC_RATE54M)
			 wf_set_bits_to_le_u32(ptxdesc + 8, 17, 1, 1);
     }
	 wf_set_bits_to_le_u32(ptxdesc + 12, 19, 1, 1);
	 
	 /* set QOS QUEUE  */
	 wf_set_bits_to_le_u32(ptxdesc + 12, 6, 5, pattrib->seqnum);
#endif
	 
	 // #if 1
	 // 	LOG_D("Mgmt frame length is %d,  txd:", len);
	 // 	for(raid=0; raid<len; raid++)
	 // 	{
	 // 		if((raid !=0 ) && (raid % 4) == 0)
	 // 		{
	 // 			printk("\n");
	 // 		}
	 // 		printk("%02x  ", pbuf[raid]);
	 // 	}
	 // 	printk("\n");
	 // #endif

}



static void mp_tx_packet(nic_info_st *pnic_info)
{
	wf_u8 *ptr, *pkt_start, *pkt_end;
	wf_u32 pkt_size, i;
	struct wl_ieee80211_hdr *hdr;
	wf_u8 payload;
	struct xmit_frame *pattrib;
	wf_mp_info_st *pmp_priv;
    hw_info_st *hw_info =(hw_info_st *) pnic_info->hw_info;
	pmp_priv = pnic_info->mp_info;

	if (pmp_priv->tx.stop)
		return;
	pmp_priv->tx.sended = 0;
	pmp_priv->tx.stop = 0;
	pmp_priv->tx_pktcount = 0;
#if 0
	if(pmp_priv->bandwidth == CHANNEL_WIDTH_40)
	{
		LOG_D("SET RF 0X18");
		data = 0x400| pmp_priv->channel;
		LOG_D("------------data :%x ",data);
		Func_Chip_Bb_Rfserial_Write_Process(pnic_info,0,0x18,data);
	}
#endif
	pattrib = &pmp_priv->tx.attrib;
	//wf_memcpy(pattrib->src, hw_info->macAddr, WF_ETH_ALEN);
	//wf_memcpy(pattrib->ta, pattrib->src, WF_ETH_ALEN);
	//wf_memcpy(pattrib->bssid, mac_test, WF_ETH_ALEN);

    //LOG_I("[mp_tx_packet]:pattrib->ra:"WF_MAC_FMT, WF_MAC_ARG(pattrib->ra));
    #if 0
    pattrib->pwdn = wf_wdn_find_info(pnic_info, pattrib->ra);
    if(pattrib->pwdn == NULL)
    {
        MP_WARN("%s wdn not find",__func__);
    }
    #else
    pattrib->pwdn = NULL;
    #endif

	pattrib->last_txcmdsz = pattrib->hdrlen + pattrib->pktlen;

	pkt_size = pattrib->last_txcmdsz;

	if (pmp_priv->tx.pallocated_buf)
		wf_kfree(pmp_priv->tx.pallocated_buf);
	pmp_priv->tx.write_size = pkt_size;
	pmp_priv->tx.buf_size = pkt_size + XMITBUF_ALIGN_SZ;
	pmp_priv->tx.pallocated_buf = wf_kzalloc(pmp_priv->tx.buf_size);
	if (pmp_priv->tx.pallocated_buf == NULL) {
		MP_WARN("%s: malloc(%d) fail!!\n", __func__, pmp_priv->tx.buf_size);
		return;
	}
	pmp_priv->tx.buf =
		(wf_u8 *) N_BYTE_ALIGMENT((SIZE_PTR) (pmp_priv->tx.pallocated_buf),
							   XMITBUF_ALIGN_SZ);
	ptr = pmp_priv->tx.buf;

	wf_memset(pmp_priv->tx.desc, 0, TXDESC_SIZE);
	pkt_start = ptr;
	pkt_end = pkt_start + pkt_size;

	mp_test_fill_tx_desc(pnic_info);

	hdr = (struct wl_ieee80211_hdr *)pkt_start;
	SetFrameSubType(&hdr->frame_ctl, WIFI_DATA);
	//SetToDs(&hdr->frame_ctl);
	wf_memset(hdr->addr1, 0xFF, WF_ETH_ALEN);
	wf_memcpy(hdr->addr2, hw_info->macAddr, WF_ETH_ALEN);
	wf_memcpy(hdr->addr3, mac_test, WF_ETH_ALEN);

	ptr = pkt_start + pattrib->hdrlen;

	switch (pmp_priv->tx.payload) {
	case 0:
		payload = 0x00;
		break;
	case 1:
		payload = 0x5a;
		break;
	case 2:
		payload = 0xa5;
		break;
	case 3:
		payload = 0xff;
		break;
	default:
		payload = 0x00;
		break;
	}
	pmp_priv->TXradomBuffer = wf_kzalloc(4096);
	if (pmp_priv->TXradomBuffer == NULL) {
		MP_WARN("mp create random buffer fail!\n");
		return;
	}

	for (i = 0; i < 4096; i++)
		pmp_priv->TXradomBuffer[i] = mp_random32() % 0xFF;

	wf_memcpy(ptr, pmp_priv->TXradomBuffer, pkt_end - ptr);
	wf_kfree(pmp_priv->TXradomBuffer);

    pmp_priv->tx.PktTxThread = kthread_run(mp_xmit_packet_thread,pnic_info,"WF_MP_TX_THREAD");
	if (IS_ERR(pmp_priv->tx.PktTxThread))
		MP_WARN("Create PktTx Thread Fail !!!!!\n");

	return;
}

static void mp_set_lck(nic_info_st *pnic_info)
{
    mcu_msg_body_st msg_rw_val;
    wf_u32 ret = -1;
    wf_u32 timeout = 2000, timecount = 0;
    if (NIC_USB == pnic_info->nic_type)
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_READVAR_MSG, NULL, 0, (wf_u32 *) & msg_rw_val, sizeof(msg_rw_val) / 4);
    }
    else
    {
//          ret = mcu_cmd_communicate(pnic_info, WLAN_OPS_DXX0_READ_VERSION, &efuse_code, 0, version, 1);
    }
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return ;
    }

	if (!(msg_rw_val.ability & BIT(26)))
	    return;

	while (msg_rw_val.bScanInProcess && timecount < timeout)
    {
	    wf_msleep(50);
	    timecount += 50;
	    if (NIC_USB == pnic_info->nic_type)
        {
            ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_READVAR_MSG, NULL, 0, (wf_u32 *) & msg_rw_val, sizeof(msg_rw_val) / 4);
        }
        else
        {
//          ret = mcu_cmd_communicate(pnic_info, WLAN_OPS_DXX0_READ_VERSION, &efuse_code, 0, version, 1);
        }
        if(ret == WF_RETURN_FAIL)
        {
            MP_WARN("set reg fail");
            return ;
        }
	}

	if (NIC_USB == pnic_info->nic_type)
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_CALI_LLC, NULL, 0, NULL, 0);
    }
    else
    {
//          ret = mcu_cmd_communicate(pnic_info, WLAN_OPS_DXX0_READ_VERSION, &efuse_code, 0, version, 1);
    }
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return ;
    }
}

static int mp_tx_test_stop_process(nic_info_st *pnic_info)
{
    wf_u32 inbuff;
    int ret;

    inbuff = 0x26;
    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MSG_WRITE_DIG, &inbuff, 1, NULL, 0);
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return -1;
    }

    return WF_RETURN_OK;
}


static int mp_tx_test_start_process(nic_info_st *pnic_info)
{
    wf_u32 inbuff;
    int ret;

    inbuff = 0x7f;
    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MSG_WRITE_DIG, &inbuff, 1, NULL, 0);
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return -1;
    }

    return WF_RETURN_OK;
}

static void mp_set_rx_filter(nic_info_st *pnic_info, wf_u8 bStartRx, wf_u8 bssidFT, wf_u8 bAB)
{
	wf_u32 inbuff[2] = { 0 };
    wf_u32 ReceiveConfig;
    wf_u32 ret = -1;

    if (bStartRx)
    {
		ReceiveConfig = RCR_APM | RCR_AM | RCR_AB | RCR_APP_ICV | RCR_AMF | RCR_HTC_LOC_CTRL | RCR_APP_MIC | RCR_APP_PHYST_RXFF;
        if (bssidFT == 1)
        {
            ReceiveConfig |= RCR_CBSSID_DATA;
        }

        ReceiveConfig |= RCR_ACRC32;
        
        inbuff[0] = 1;
        inbuff[1] = ReceiveConfig;
	}
    else
	{
        inbuff[0] = 0;
        inbuff[1] = 0;
	}

    if (NIC_USB == pnic_info->nic_type)
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_PRX, inbuff, 2, NULL, 0);
    }

    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return ;
    }

}

static void mp_sdio_set_rx_filter(nic_info_st *pnic_info, wf_u8 bStartRx, wf_u8 bAB)
{
    wf_mp_info_st *mp_info = pnic_info->mp_info;
    wf_u32 ret;
    wf_u32 outbuf;
    wf_u32 inbuf[4] = {0};
    wf_u32 ReceiveConfig;

    if(bStartRx == 1)
    {
        #if 0
        send_msg[0] = 1;
        for(i=0;i<6;i++)
        {
            send_msg[i+1] = mp_info->network_macaddr[i];
        }
        
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_FW_INIT, send_msg, 7, mail_box, 9);
        if(mail_box[0] == wf_true)
        {
            mp_info->sdio_ReceiveConfig = mail_box[1];
        }
        #endif
        
        ReceiveConfig = RCR_APM | RCR_AM | RCR_AB | RCR_APP_ICV | RCR_AMF | RCR_HTC_LOC_CTRL | RCR_APP_MIC | RCR_APP_PHYST_RXFF;
    }
    else
    {
        ReceiveConfig = 0;
    }
    inbuf[0] = bStartRx;
    inbuf[1] = mp_info->bSetRxBssid;
    inbuf[2] = ReceiveConfig;
    inbuf[3] = bAB;
    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_PRX, inbuf,4, &outbuf, 1);
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return ;
    }
    mp_info->sdio_ReceiveConfig = outbuf;

}

static int mp_trx_test_pretx_proc(nic_info_st *pnic_info, wf_u8 bStartTest, char *extra)
{
    wf_mp_info_st *pmp_info = pnic_info->mp_info;
    wf_u32 inbuff;
    wf_u32 inbuff1[2] = {0};
    wf_u32 ret;
    wf_u32 loopback;
    if ((pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped))
    {
		return -1;
	}

    switch (pmp_info->mode)
    {
    	case MP_PHY_LOOPBACK:
    	{
            LOG_I("[%s]: trx mode is MP_PHY_LOOPBACK", __func__);
			if (bStartTest == 0)
			{
				pmp_info->tx.stop = 1;
				pmp_info->rx_start = 0;
                
				if (NIC_USB == pnic_info->nic_type)
                {
                    mp_set_rx_filter(pnic_info, 0, 0, wf_false);
                }
                else
                {
                    mp_sdio_set_rx_filter(pnic_info, 0, wf_false);
                }
                
				loopback = wf_io_read32(pnic_info, 0xd00,NULL);
				LOG_D("loopback:%x",loopback);
				loopback &= ~(0x1000000);
				LOG_D("loopback:%x",loopback);
				wf_io_write32(pnic_info, 0xd00, loopback);

				mp_tx_test_stop_process(pnic_info);

			}
			else if (pmp_info->tx.stop == 1)
			{

				pmp_info->tx.stop = 0;

                loopback = wf_io_read32(pnic_info, 0xd00,NULL);
				LOG_D("bStartTest loopback:%x",loopback);
				loopback |= 0x1000000;
				LOG_D("bStartTest loopback:%x",loopback);
				wf_io_write32(pnic_info, 0xd00, loopback);

                pmp_info->rx_start = 1;

                if (NIC_USB == pnic_info->nic_type)
                {
                    mp_set_rx_filter(pnic_info, 1, 0, wf_false);
                }
                else
                {
                    mp_sdio_set_rx_filter(pnic_info, 1, wf_false);
                }
                
                mp_tx_test_start_process(pnic_info);
                mp_tx_packet(pnic_info);
			}
			else
			{
				return WF_RETURN_FAIL;
			}

		}
		break;
        case MP_MAC_LOOPBACK:
        {
            LOG_I("[%s]: trx mode is MP_MAC_LOOPBACK", __func__);
            if (bStartTest == 0)
            {
                pmp_info->tx.stop = 1;
                pmp_info->rx_start = 0;

                if (NIC_USB == pnic_info->nic_type)
                {
                    mp_set_rx_filter(pnic_info, 0, 0, wf_false);
                }
                else
                {
                    mp_sdio_set_rx_filter(pnic_info, 0, wf_false);
                }

                loopback = wf_io_read32(pnic_info, 0x100,NULL);
                LOG_D("loopback:%x",loopback);
                loopback &= ~(BIT(24) | BIT(25) | BIT(27));
                LOG_D("loopback:%x",loopback);
                wf_io_write32(pnic_info, 0x100, loopback);

                mp_tx_test_stop_process(pnic_info);
            }
            else if (pmp_info->tx.stop == 1)
            {

                pmp_info->tx.stop = 0;

                loopback = wf_io_read32(pnic_info, 0x100,NULL);
	            LOG_D("loopback:%x",loopback);
	            loopback |= (BIT(24) | BIT(25) | BIT(27));
	            LOG_D("loopback:%x",loopback);
	            wf_io_write32(pnic_info, 0x100, loopback);

                pmp_info->rx_start = 1;

                if (NIC_USB == pnic_info->nic_type)
                {
                    mp_set_rx_filter(pnic_info, 1, 0, wf_false);
                }
                else
                {
                    mp_sdio_set_rx_filter(pnic_info, 1, wf_false);
                }

                mp_tx_test_start_process(pnic_info);
                mp_tx_packet(pnic_info);

            }
            else
            {
                return WF_RETURN_FAIL;
            }

            return 0;
        }
        break;

	    case MP_PACKET_TX:
        {
            LOG_I("[%s]: trx mode is MP_PACKET_TX", __func__);
            if (bStartTest == 0)
            {
                pmp_info->tx.stop = 1;

                mp_tx_test_stop_process(pnic_info);
            }
            else if (pmp_info->tx.stop == 1)
            {

                pmp_info->tx.stop = 0;

                mp_tx_test_start_process(pnic_info);
                mp_tx_packet(pnic_info);

            }
            else
            {
                return WF_RETURN_FAIL;
            }

            return 0;
         }
         break;

	case MP_SINGLE_TONE_TX:
        {
            LOG_I("[%s]: trx mode is MP_SINGLE_TONE_TX", __func__);
			
			if(pmp_info->rateidx <= DESC_RATE11M)
			{
				sprintf(extra,"%s","single rate error");
				break;
			}
    		if (bStartTest == 1)
            {
    			sprintf(extra,
    					"%s\nStart continuous DA=ffffffffffff len=%d\n infinite=yes.",
    					extra, pmp_info->tx.attrib.pktlen);
                inbuff = 1;
                ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_SINGLETONETX, &inbuff, 1, NULL, 0);
				pmp_info->tx.stop = 0;
				mp_tx_test_start_process(pnic_info);
                mp_tx_packet(pnic_info);
            }
            else
            {
                inbuff = 0;
                ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_SINGLETONETX, &inbuff, 1, NULL, 0);
                mp_tx_test_stop_process(pnic_info);
				pmp_info->tx.stop = 1;
            }
        }
		break;

    case MP_SINGLE_CARRIER_TX:
        {
            LOG_I("[%s]: trx mode is MP_SINGLE_CARRIER_TX", __func__);
    		if (bStartTest == 1)
            {
    			sprintf(extra,
    					"%s\nStart continuous DA=ffffffffffff len=%d\n infinite=yes.",
    					extra, pmp_info->tx.attrib.pktlen);

                inbuff = 1;
                ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_SINGLECARRTX, &inbuff, 1, NULL, 0);
				mp_tx_test_start_process(pnic_info);
				pmp_info->tx.stop = 0;
                mp_tx_packet(pnic_info);
            }
            else
            {
                inbuff = 0;
                ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_SINGLECARRTX, &inbuff, 1, NULL, 0);
				mp_tx_test_stop_process(pnic_info);
				pmp_info->tx.stop = 1;
            }
        }
		break;

	case MP_CONTINUOUS_TX:
        {
            LOG_I("[%s]: trx mode is MP_CONTINUOUS_TX", __func__);
    		if (bStartTest == 1)
            {
    			sprintf(extra,
    					"%s\nStart continuous DA=ffffffffffff len=%d\n infinite=yes.",
    					extra, pmp_info->tx.attrib.pktlen);

        		if(pmp_info->rateidx <= DESC_RATE11M)
                {
                    inbuff1[0] = 1;
                    inbuff1[1] = pmp_info->rateidx;
                    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_CCKCTX, inbuff1, 2, NULL, 0);
                }
                else if(pmp_info->rateidx >= DESC_RATE6M)
                {
                    inbuff = 1;
                    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_OFDMCTX, &inbuff, 1, NULL, 0);
                }
            }
            else
            {
                if(pmp_info->rateidx <= DESC_RATE11M)
                {
                    inbuff1[0] = 0;
                    inbuff1[1] = pmp_info->rateidx;
                    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_CCKCTX, inbuff1, 2, NULL, 0);
                }
                else if(pmp_info->rateidx >= DESC_RATE6M)
                {
                    inbuff = 0;
                    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_OFDMCTX, &inbuff, 1, NULL, 0);
                }
            }
        }
        break;

	case MP_CARRIER_SUPPRISSION_TX:
        {
            LOG_I("[%s]: trx mode is MP_CARRIER_SUPPRISSION_TX", __func__);
    		if (bStartTest == 1)
            {
    			if (pmp_info->rateidx <= DESC_RATE11M)
    			{
    				sprintf(extra,
    						"%s\nStart continuous DA=ffffffffffff len=%d\n infinite=yes.",
    						extra, pmp_info->tx.attrib.pktlen);
    			}
    			else
    			{
    				sprintf(extra,
    						"%s\nSpecify carrier suppression but not CCK rate",
    						extra);
					break;
    			}

                inbuff1[0] = 1;
                inbuff1[1] = pmp_info->rateidx;
                ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_CARRSUPPTX, inbuff1, 2, NULL, 0);
				mp_tx_test_start_process(pnic_info);
				pmp_info->tx.stop = 0;
                mp_tx_packet(pnic_info);
    		}
            else
            {
        	    inbuff1[0] = 0;
                inbuff1[1] = pmp_info->rateidx;
                ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_CARRSUPPTX, inbuff1, 2, NULL, 0);
				 mp_tx_test_stop_process(pnic_info);
				pmp_info->tx.stop = 1;
            }
        }
		break;

	case MP_TX_LCK:
        {
            LOG_I("[%s]: trx mode is MP_TX_LCK", __func__);
		    mp_set_lck(pnic_info);
        }
        break;

	default:
		sprintf(extra, "Error! Continuous-Tx is not on-going.");
		return WF_RETURN_FAIL;
	}

    #if 0
	if (bStartTest == 1 && pmp_info->mode != MP_ON)
    {
		if (pmp_info->tx.stop == 0)
        {
			pmp_info->tx.stop = 1;
			wf_msleep(5);
		}
#ifdef CONFIG_80211N_HT
		pmp_info->tx.attrib.ht_en = 1;
#endif
		pmp_info->tx.stop = 0;
		pmp_info->tx.count = 1;
		mp_tx_packet(pnic_info);
	}
    else
    {
		pmp_info->mode = MP_ON;
    }
    #endif

	return 0;
}

static void mp_dump_mac_rx_counters(nic_info_st *pnic_info,struct dbg_rx_counter *rx_counter)
{
    wf_u32 outbuff[6];
    wf_u32 ret;
	if (!rx_counter) {
		MP_WARN("rx_counter NULL");
		return;
	}

    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_MACRXCOUNT, NULL, 0, outbuff, 6);
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return ;
    }
    rx_counter->rx_pkt_ok = outbuff[0];
    rx_counter->rx_pkt_crc_error = outbuff[1];
    rx_counter->rx_cck_fa = outbuff[2];
    rx_counter->rx_ofdm_fa = outbuff[3];
    rx_counter->rx_ht_fa = outbuff[4];
    rx_counter->rx_pkt_drop = outbuff[5];
}

static void mp_dump_phy_rx_counters(nic_info_st *pnic_info,struct dbg_rx_counter *rx_counter)
{
	wf_u32 outbuff[4];
    wf_u32 ret;
	if (!rx_counter) {
		MP_WARN("rx_counter NULL");
		return;
	}

    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_PHYRXCOUNT, NULL, 0, outbuff, 4);
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return ;
    }

	rx_counter->rx_pkt_ok = outbuff[0];
	rx_counter->rx_pkt_crc_error = outbuff[1];
	rx_counter->rx_ofdm_fa = outbuff[2];
	rx_counter->rx_cck_fa = outbuff[3];
}


int wf_mp_rx_common_process( nic_info_st *pnic_info, wf_u8 *pktBuf, wf_u32 pktLen)
{
    int ret = 0;
    wf_mp_info_st *pmp_priv = pnic_info->mp_info;
    rx_pkt_t pkt;
	wf_u8 *data;
	static int rx_cnt=0;
    if (pmp_priv == NULL)
        return -1;

    if (pmp_priv->rx_start == 0)
        return -1;

    pkt.p_nic_info = pnic_info;
    wf_rx_rxd_prase(pktBuf, &pkt);

    //LOG_D("[RXD] pkt_len:%d",pkt.pkt_info.pkt_len);    
    if(!(++rx_cnt % 500))
	{
		LOG_D("total rx_cnt=%d",rx_cnt);
    }
    
#ifdef CONFIG_RICHV200
	data = pktBuf + pkt.pkt_info.hif_hdr_len;
#else
	data = pktBuf + 24 + 32;
#endif

    if (pmp_priv->mode == MP_MAC_LOOPBACK)
    {
    	if(wf_memcmp(GET_ADDR3(data),mac_test,6) == 0)
    	{
    		pmp_priv->rx_pktcount++;
    	}
		//LOG_D("bssid:"WF_MAC_FMT,WF_MAC_ARG(get_bssid(data)));
    }
    else
    {
        pmp_priv->rx_pktcount++;
    }
    
    return ret;
}






int wf_mp_test_rx(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
    wf_mp_info_st *pmp_priv = pnic_info->mp_info;
    int bStartRx = 0, bStopRx = 0, bQueryRx = 0, bQueryPhy = 0, bQueryMac = 0, bSetBssid = 0;
    int bmac_filter = 0, bmon = 0, bSmpCfg = 0;
    wf_u8 input[wrqu->length];
    char *pch, *token, *tmp[2] = { 0x00, 0x00 };
    wf_u32 i = 0, jj = 0, kk = 0, cnts = 0, ret;
    struct dbg_rx_counter rx_counter;
    wf_u32 txok, txfail, rxok, rxfail, rxfilterout;
    wf_u8 *str;
    char *ptr = NULL;
    wf_mp_tx *pmptx;
    if (copy_from_user(input, wrqu->pointer, wrqu->length))
        return WF_RETURN_FAIL;

    MP_DBG("%s: %s\n", __func__, input);
    if(pmp_priv == NULL)
    {
        MP_WARN("mp_info not init");
        return WF_RETURN_FAIL;
    }
    pmptx = &pmp_priv->tx;

    ptr = input;
    str = strsep(&ptr, "=");
    bStartRx = (strncmp(ptr, "start", 5) == 0) ? 1 : 0;
    bStopRx = (strncmp(ptr, "stop", 4) == 0) ? 1 : 0;
    bQueryRx = (strncmp(ptr, "query", 5) == 0) ? 1 : 0;
    bQueryPhy = (strncmp(ptr, "phy", 3) == 0) ? 1 : 0;
    bQueryMac = (strncmp(ptr, "mac", 3) == 0) ? 1 : 0;
    bSetBssid = (strncmp(ptr, "setbssid=", 8) == 0) ? 1 : 0;
    bmac_filter = (strncmp(ptr, "accept_mac", 10) == 0) ? 1 : 0;
    bmon = (strncmp(ptr, "mon=", 4) == 0) ? 1 : 0;
    bSmpCfg = (strncmp(ptr, "smpcfg=", 7) == 0) ? 1 : 0;

    if (bSetBssid == 1) {
        pch = ptr;
        while ((token = strsep(&pch, "=")) != NULL)
        {
            if (i > 1)
                break;
            tmp[i] = token;
            i++;
        }
        if ((tmp[0] != NULL) && (tmp[1] != NULL))
        {
            cnts = strlen(tmp[1]) / 2;
            if (cnts < 1)
                return WF_RETURN_FAIL;
            MP_DBG("%s: cnts=%d\n", __func__, cnts);
            MP_DBG("%s: data=%s\n", __func__, tmp[1]);
            for (jj = 0, kk = 0; jj < cnts; jj++, kk += 2)
             {
                pmp_priv->network_macaddr[jj] = mp_trx_key_of_2char2num_func(tmp[1][kk], tmp[1][kk + 1]);
                MP_DBG("network_macaddr[%d]=%x\n", jj,pmp_priv->network_macaddr[jj]);
            }
        } else
            return WF_RETURN_FAIL;

        pmp_priv->bSetRxBssid = wf_true;
    }

    if (bmac_filter) {
        pmp_priv->bmac_filter = bmac_filter;
        pch = input;
        while ((token = strsep(&pch, "=")) != NULL) {
            if (i > 1)
                break;
            tmp[i] = token;
            i++;
        }
        if ((tmp[0] != NULL) && (tmp[1] != NULL)) {
            cnts = strlen(tmp[1]) / 2;
            if (cnts < 1)
                return WF_RETURN_FAIL;
            MP_DBG("%s: cnts=%d\n", __func__, cnts);
            MP_DBG("%s: data=%s\n", __func__, tmp[1]);
            for (jj = 0, kk = 0; jj < cnts; jj++, kk += 2) {
                pmp_priv->mac_filter[jj] = mp_trx_key_of_2char2num_func(tmp[1][kk], tmp[1][kk + 1]);
                MP_DBG("%s mac_filter[%d]=%x\n", __func__, jj,pmp_priv->mac_filter[jj]);
            }
        } else
            return WF_RETURN_FAIL;

    }

    if (bStartRx)
    {
        sprintf(extra, "start ok");

        if (NIC_USB == pnic_info->nic_type)
        {
            mp_set_rx_filter(pnic_info, bStartRx, 0, wf_false);
        }
        else
        {
            mp_sdio_set_rx_filter(pnic_info, bStartRx, wf_false);
        }
            sprintf(extra, "Received packet OK:%d CRC error:%d ,Filter out:%d",
                pmp_priv->rx_pktcount,
                pmp_priv->rx_crcerrpktcount,
                pmp_priv->rx_pktcount_filter_out);

        pmp_priv->rx_start = 1;

    }
    else if (bStopRx)
    {

        pmp_priv->rx_start = 0;
        if (NIC_USB == pnic_info->nic_type)
        {
            mp_set_rx_filter(pnic_info, bStartRx, 0, wf_false);
        }
        else
        {
            mp_sdio_set_rx_filter(pnic_info, bStartRx, wf_false);
        }
        pmp_priv->bmac_filter = wf_false;
        sprintf(extra, "Received packet OK:%d CRC error:%d ,Filter out:%d",
                pmp_priv->rx_pktcount,
                pmp_priv->rx_crcerrpktcount,
                pmp_priv->rx_pktcount_filter_out);

    }
    else if (bQueryRx)
    {
        txok = pmptx->sended;
        txfail = 0;
            rxok = pmp_priv->rx_pktcount;
        rxfail = pmp_priv->rx_crcerrpktcount;
        rxfilterout = pmp_priv->rx_pktcount_filter_out;

        wf_memset(extra, '\0', 128);

        sprintf(extra,
                "Tx OK:%d, Tx Fail:%d, Rx OK:%d, CRC error:%d ,Rx Filter out:%d\n",
                txok, txfail, rxok, rxfail, rxfilterout);


    }
    else if (bQueryPhy)
    {
        wf_memset(&rx_counter, 0, sizeof(struct dbg_rx_counter));
        mp_dump_phy_rx_counters(pnic_info, &rx_counter);

        MP_DBG("%s: OFDM_FA =%d\n", __func__, rx_counter.rx_ofdm_fa);
        MP_DBG("%s: CCK_FA =%d\n", __func__, rx_counter.rx_cck_fa);
        sprintf(extra, "Phy Received packet OK:%d CRC error:%d FA Counter: %d",
                rx_counter.rx_pkt_ok, rx_counter.rx_pkt_crc_error,
                rx_counter.rx_cck_fa + rx_counter.rx_ofdm_fa);

    }
    else if (bQueryMac)
    {
        wf_memset(&rx_counter, 0, sizeof(struct dbg_rx_counter));
        mp_dump_mac_rx_counters(pnic_info, &rx_counter);
        sprintf(extra,
                "Mac Received packet OK: %d , CRC error: %d , Drop Packets: %d\n",
                rx_counter.rx_pkt_ok, rx_counter.rx_pkt_crc_error,
                rx_counter.rx_pkt_drop);

    }

    if (bmon == 1)
    {
        ret = sscanf(input, "mon=%d", &bmon);
        pmp_priv->rx_bindicatePkt = wf_true;
        sprintf(extra, "Indicating Receive Packet to network start\n");

    }

    if (bSmpCfg == 1)
    {
        ret = sscanf(input, "smpcfg=%d", &bSmpCfg);

        if (bSmpCfg == 1) {
            pmp_priv->bWLSmbCfg = wf_true;
            sprintf(extra, "Indicate By Simple Config Format\n");
            if (NIC_USB == pnic_info->nic_type)
            {
                mp_set_rx_filter(pnic_info, wf_true, 0, wf_true);
            }
            else
            {
                mp_sdio_set_rx_filter(pnic_info,wf_true,wf_true);
            }
        } else {
            pmp_priv->bWLSmbCfg = wf_false;
            sprintf(extra, "Indicate By Normal Format\n");
            if (NIC_USB == pnic_info->nic_type)
            {
                mp_set_rx_filter(pnic_info, wf_true, 0, wf_false);
            }
            else
            {
                mp_sdio_set_rx_filter(pnic_info,wf_true,wf_false);
            }
        }
    }

    wrqu->length = strlen(extra) + 1;

    return 0;

}




int wf_mp_test_tx(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
	wf_mp_info_st *pmp_info = pnic_info->mp_info;
    struct xmit_frame *pattrib = &pmp_info->tx.attrib;
    wf_u8 input[wrqu->length];

    wf_u32 pkTx = 1;
	wf_u32 countPkTx = 1, cotuTx = 1, CarrSprTx = 1, scTx = 1, sgleTx = 1, stop = 1;
	wf_u32 bStartTest = 1;
	wf_u32 count = 0, pktinterval = 0, len = 0;
	wf_u8 status;
	wf_u32 lck = 1;
    wf_u8 maclpk = 1;
	wf_u8 phylpk = 1;
	wf_u8 *str;

    char *pch;
    if ((pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped))
    {
		return -1;
	}

    if (copy_from_user(input, wrqu->pointer, wrqu->length))
        return WF_RETURN_FAIL;

    LOG_D("set %s",input);

    pch = input;
	str = strsep(&pch, "=");
	countPkTx = strncmp(pch, "count=", 5);
	cotuTx = strncmp(pch, "background", 10);
	CarrSprTx = strncmp(pch, "carr", 4);
	scTx = strncmp(pch, "sc", 2);
	sgleTx = strncmp(pch, "single", 6);
	pkTx = strncmp(pch, "frame", 5);
	lck = strncmp(pch, "lck", 3);
	stop = strncmp(pch, "stop", 4);
    maclpk = strncmp(pch, "mac_loopback", 12);
	phylpk = strncmp(pch, "phy_loopback", 12);

    if (sscanf(pch, "mac_loopback,count=%d", &count) > 0)
    {
		printk("count=%d\n", count);
    }
    if (sscanf(pch, "phy_loopback,count=%d", &count) > 0)
    {
		printk("count=%d\n", count);
    }
	if (sscanf(pch, "frame,count=%d", &count) > 0)
    {
		printk("count=%d\n", count);
    }
	if (sscanf(pch, "pktinterval=%d", &pktinterval) > 0)
    {
		printk("pktinterval=%d\n", pktinterval);
    }
	if (sscanf(pch, "frame,len=%d", &len) > 0)
    {
		printk("len=%d\n", len);
    }
	if (sscanf(pch, "frame,len=%d,count=%d", &len, &count) > 0)
    {
		printk("len=%d,count=%d\n", len, count);
    }

	if (wf_memcmp(pch, "destmac=", 8))
    {
		wrqu->length -= 8;
		mp_tx_destaddr(pnic_info,&pch[8]);
		sprintf(extra, "Set dest mac OK !\n");
	}

	//wf_memset(extra, '\0', strlen(extra));

	if (pktinterval != 0)
    {
		sprintf(extra, "Pkt Interval = %d", pktinterval);
		pmp_info->pktInterval = pktinterval;

	}

	if (len != 0)
    {
		sprintf(extra, "Pkt len = %d", len);
		pattrib->pktlen = len;

	}

    pmp_info->tx.count = count;

    if (pkTx == 0 || countPkTx == 0)
    {
        LOG_I("[%s]: mp trx mode: MP_PACKET_TX", __func__);
        pmp_info->mode = MP_PACKET_TX;
    }

    if (sgleTx == 0)
    {
        LOG_I("[%s]: mp trx mode: MP_SINGLE_TONE_TX", __func__);
        pmp_info->mode = MP_SINGLE_TONE_TX;
    }

    if (scTx == 0)
    {
        LOG_I("[%s]: mp trx mode: MP_SINGLE_CARRIER_TX", __func__);
        pmp_info->mode = MP_SINGLE_CARRIER_TX;
    }

    if (cotuTx == 0)
    {
        LOG_I("[%s]: mp trx mode: MP_CONTINUOUS_TX", __func__);
        pmp_info->mode = MP_CONTINUOUS_TX;
    }

    if (CarrSprTx == 0)
    {
        LOG_I("[%s]: mp trx mode: MP_CARRIER_SUPPRISSION_TX", __func__);
        pmp_info->mode = MP_CARRIER_SUPPRISSION_TX;
    }

    if (maclpk == 0)
    {
        LOG_I("[%s]: mp trx mode: MP_MAC_LOOPBACK", __func__);
        pmp_info->mode = MP_MAC_LOOPBACK;
    }

	if (phylpk == 0)
    {
        LOG_I("[%s]: mp trx mode: MP_PHY_LOOPBACK", __func__);
        pmp_info->mode = MP_PHY_LOOPBACK;
    }
    if (lck == 0)
    {
        LOG_I("[%s]: mp trx mode: MP_TX_LCK", __func__);
        pmp_info->mode = MP_TX_LCK;
    }

    if (stop == 0)
    {
		bStartTest = 0;
        LOG_I("[%s]: mp trx stop tx", __func__);
		sprintf(extra, "Stop Tx");
	}
    else
    {
		bStartTest = 1;
        LOG_I("[%s]: mp trx start tx", __func__);
        sprintf(extra, "Start Tx");
	}

	status = mp_trx_test_pretx_proc(pnic_info, bStartTest, extra);

    sprintf(extra, "Tx %s",status == 0?"ok":"fail");

	//wrqu->length = strlen(extra);
	return status;
}




int wf_mp_stats(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;

    int ret = 0;
	bool bReset = 0, bQuery = 0;
	wf_mp_info_st *pmp_priv;
	wf_u8 input[wrqu->length];
	wf_u32 txok, txfail, rxok, rxfail, rxfilterout;
	wf_u32 flag = 0x03;
	wf_u8 *str;
	char *ptr = NULL;
    wf_mp_tx *pmptx;
    pmp_priv = pnic_info->mp_info;
    pmptx = &pmp_priv->tx;


	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		ret = WF_RETURN_FAIL;
	ptr = input;
	str = strsep(&ptr, "=");
	bReset = (strncmp(ptr, "reset", 5) == 0) ? 1 : 0;
	bQuery = (strncmp(ptr, "query", 5) == 0) ? 1 : 0;

    if (bReset == 1)
    {
		pmp_priv->tx.sended = 0;
		pmp_priv->tx_pktcount = 0;
		pmp_priv->rx_pktcount = 0;
		pmp_priv->rx_pktcount_filter_out = 0;
		pmp_priv->rx_crcerrpktcount = 0;

        if (NIC_USB == pnic_info->nic_type)
        {
            ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_RESETCOUNT, &flag, 1, NULL, 0);
        }
        else
        {
            /* reset phy count */
            ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_RESET_PHY_RX_COUNTERS, NULL, 0, NULL, 0);
            /* reset mac count */
            ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_RESET_MAC_RX_COUNTERS, NULL, 0, NULL, 0);
        }
        if(ret == WF_RETURN_FAIL)
        {
            MP_WARN("set reg fail");
            return -1;
        }
		wf_memset(extra, 0, wrqu->length);
		sprintf(extra, "stats reset %s",ret == 0?"ok":"fail");
	}
    else if (bQuery == 1)
    {
		txok = pmptx->sended;
		txfail = 0;
		rxok = pmp_priv->rx_pktcount;
		rxfail = pmp_priv->rx_crcerrpktcount;
		rxfilterout = pmp_priv->rx_pktcount_filter_out;

		wf_memset(extra, '\0', 128);

		sprintf(extra,
				"Tx OK:%d, Tx Fail:%d, Rx OK:%d, CRC error:%d ,Rx Filter out:%d\n",
				txok, txfail, rxok, rxfail, rxfilterout);
	}
    else
    {
		ret = WF_RETURN_FAIL;
    }
	wrqu->length = strlen(extra);

	return ret;
}
int wf_mp_cmd_download(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
	wf_u32 count;
	wf_u32 ret;
	wf_u8 *str;
	char *ptr = NULL;
	wf_u8 data[40] = "cmdtest";
	wf_u8 out_buf[40] = {0};
	wf_u32 failed_times=0;
	wf_u32 ok_times=0;
	wf_u8 input[wrqu->length];
	LOG_D("%s",__func__);
	
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		ret = WF_RETURN_FAIL;
	ptr = input;
	str = strsep(&ptr, "=");
	if(ptr == NULL)
	{
		sprintf(extra, "parameter error");
		return 0;
	}
	sscanf(ptr,"%d",&count);

	for(;count>0;count--)
	{
		wf_memset(out_buf,0,10);
		ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_CMD_TEST, (wf_u32 *)data, 10, (wf_u32 *)out_buf, 10);
		if (WF_RETURN_FAIL == ret)
    	{
    		LOG_E("[%s] UMSG_OPS_CMD_TEST failed", __func__);
        	return ret;
    	}
		LOG_D("%s",out_buf);
		if(wf_memcmp(data,out_buf,10) == 0)
		{
			LOG_D("OK");
			ok_times++;
		}
		else
		{
			LOG_D("FAIL");
			failed_times++;
		}
		wf_msleep(5);
	}
	sprintf(extra, "cmddl:%d ok:%d fail:%d",(ok_times+failed_times),ok_times,failed_times);
	return 0;
}

int wf_mp_fw_download(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
	wf_u32 count = 0;
	wf_u32 ret;
	wf_u8 *str;
	char *ptr = NULL;
	wf_u32 failed_times=0;
	wf_u32 ok_times=0;
	wf_u8 input[wrqu->length];
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		ret = WF_RETURN_FAIL;
	ptr = input;
	str = strsep(&ptr, "=");
	if(ptr == NULL)
	{
		sprintf(extra, "parameter error");
		return 0;
	}
	sscanf(ptr,"%d",&count);
	for(;count>0;count--)
	{
		ret = wf_fw_download(pnic_info);
		if(ret != 0)
		{
			LOG_D("wf_fw_download fail!!!!");
			failed_times++;
		}
		else
		{
			ok_times++;
		}
		wf_msleep(5);
	}
	sprintf(extra, "fwdl:%d ok:%d fail:%d",(ok_times+failed_times),ok_times,failed_times);
	return 0;
}

#endif
#endif
