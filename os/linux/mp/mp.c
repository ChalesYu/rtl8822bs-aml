#include "mp.h"

#include "wf_debug.h"
#include "iw_priv_func.h"

#ifdef CONFIG_MP_MODE
#if defined(CONFIG_WEXT_PRIV)

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


static wf_bool mp_test_enable = wf_false;



int wf_io_write_cmd_special(nic_info_st *nic_info, wf_u32 func_code, wf_u32 *recv,  int len, int offs)
{
    int ret = 0;
    wf_u32 mailbox_reg_addr = MAILBOX_ARG_START;
    int revclen;

    //LOG_I("[%s] func_code:0x%08x",__func__, func_code);
    if (len > MAILBOX_MAX_RDLEN)
    {
        LOG_E("discard func %d because rd len = %d too big\r\n", func_code,len);
        return WF_RETURN_FAIL;
    }

    revclen = len;

    ret = wf_io_write32(nic_info, MAILBOX_REG_START, func_code);
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
        return ret;
    }

    ret = wf_io_write32(nic_info, mailbox_reg_addr, len);
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
        return ret;
    }

    mailbox_reg_addr += MAILBOX_WORD_LEN;
    ret = wf_io_write32(nic_info, mailbox_reg_addr, offs);

    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
        return ret;
    }
    mailbox_reg_addr += MAILBOX_WORD_LEN;

    ret = wf_mcu_cmd_get_status(nic_info,func_code);
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_mcu_cmd_get_status failed",__func__,__LINE__);
        return ret;
    }
    else if(WF_RETURN_REMOVED_FAIL == ret)
    {
        LOG_W("[%s,%d] removed driver or device warning",__func__,__LINE__);
        return ret;
    }

    mailbox_reg_addr = MAILBOX_ARG_START;

    while (len--)
    {

        *recv++ = wf_io_read32(nic_info, mailbox_reg_addr,NULL);
        mailbox_reg_addr += 4;
    }


    LOG_I("func_code:0x%x, sendlen:%d, recvlen:%d\n",func_code, 0, revclen );
    return ret;
}

static int mp_hw_init(nic_info_st *pnic_info)
{
    wf_u32 inbuff;
    int ret;

    /* set sysclk */ /* avoid fw modify the reg */
    {
        wf_u32 value32;


        value32 = wf_io_read32(pnic_info, 0x24,NULL);
        ret = wf_io_write32(pnic_info, 0x24, 0x350016ef);
        if( WF_RETURN_FAIL == ret)
        {
            LOG_E("[%s] DXX0_EN_ADDR failed, check!!!",__func__);
            return ret;
        }
        value32 = wf_io_read32(pnic_info, 0x24,NULL);
        LOG_I("clk value32:0x%x",value32);
    }

    /* mp start : adjust RF*/
    inbuff = 1;
    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_CTXRATE, &inbuff, 1, NULL, 0);
    if(ret == WF_RETURN_FAIL)
    {
       MP_WARN("set reg fail");
       return -1;
    }

    /* set RF path */
    mp_set_rf(pnic_info);

    /* set rcr 0, adjust bb reg*/
    if (NIC_USB == pnic_info->nic_type)
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_INIT, NULL, 0, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_INIT, NULL, 0, NULL, 0);
        wf_mcu_mp_bb_rf_gain_offset(pnic_info);
    }
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
        return -1;
    }

    return WF_RETURN_OK;
}

static wf_u32 mp_init(nic_info_st *pnic_info)
{
	wf_mp_info_st *pmppriv;
    wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
    wf_mp_tx *pmptx ;
    rx_info_t *rx_info = pnic_info->rx_info;
    struct xmit_frame *pattrib;

    pmppriv = wf_kzalloc(sizeof(wf_mp_info_st));
    if (pmppriv == NULL)
    {
        LOG_E("[SCAN] malloc scan_param_st failed");
        return -1;
    }
    else
    {
        pnic_info->mp_info = pmppriv;
    }


    rx_info->rx_crcerr_pkt = 0;
    rx_info->rx_pkts = 0;
    rx_info->rx_total_pkts = 0;
	wf_memset(pmppriv, 0, sizeof(wf_mp_info_st));
//    phw_info->use_fixRate = wf_true;

	pmppriv->mode = MP_OFF;

	pmppriv->channel = 1;
	pmppriv->bandwidth = CHANNEL_WIDTH_20;
	pmppriv->prime_channel_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	pmppriv->rateidx = 1;
	pmppriv->txpoweridx = 0x2A;

	pmppriv->antenna_tx = 8;
	pmppriv->antenna_rx = 8;

	pmppriv->check_mp_pkt = 0;

	pmppriv->tx_pktcount = 0;

	pmppriv->rx_bssidpktcount = 0;
	pmppriv->rx_pktcount = 0;
	pmppriv->rx_crcerrpktcount = 0;

	pmppriv->network_macaddr[0] = 0xb4;
	pmppriv->network_macaddr[1] = 0x04;
	pmppriv->network_macaddr[2] = 0x18;
	pmppriv->network_macaddr[3] = 0x00;
	pmppriv->network_macaddr[4] = 0x00;
	pmppriv->network_macaddr[5] = 0x02;

	pmppriv->bSetRxBssid = wf_false;
	pmppriv->bWLSmbCfg = wf_true;


	wf_memcpy(pcur_network->mac_addr, pmppriv->network_macaddr, WF_ETH_ALEN);


	pcur_network->ssid.length = 8;
	wf_memcpy(pcur_network->ssid.data, "mp_908621x", pcur_network->ssid.length);

	pmppriv->tx.payload = 2;

	pmppriv->mp_dm = 0;
	pmppriv->tx.stop = 1;
	pmppriv->bSetTxPower = 0;
	pmppriv->pktInterval = 1;
	pmppriv->pktLength = 1000;
    pmptx = &pmppriv->tx;

	pattrib = &pmptx->attrib;
	wf_memset(pattrib, 0, sizeof(struct xmit_frame));
	wf_memset(pmptx->desc, 0, TXDESC_SIZE);
#ifdef CONFIG_80211N_HT
	pattrib.ht_en = 1;
#endif

	pattrib->ether_type = 0x8712;
	//wf_memset(pmppriv->dst, 0xFF, WF_ETH_ALEN);

	pattrib->hdrlen = WLAN_HDR_A3_LEN;
	//pattrib->subtype = WIFI_DATA;
	pattrib->priority = 0;
	pattrib->qsel = pattrib->priority;
	pattrib->nr_frags = 1;
	pattrib->encrypt_algo = _NO_PRIVACY_;
	//pattrib->qos_en = wf_false;

	pattrib->pktlen = 1500;
	pattrib->nic_info = pnic_info;
	pmppriv->antenna_tx = 8;
	pmppriv->antenna_rx = 8;
    pnic_info->is_up = 1;

	pmppriv->pnic_info = pnic_info;

    mp_hw_init(pnic_info);

    wf_mlme_set_connect(pnic_info, wf_true);
    wf_mcu_set_op_mode(pnic_info, WF_INFRA_MODE);
    wf_mcu_set_usb_agg_normal(pnic_info,WIRELESS_11BG_24N);

	return 0;
}


static int mp_term(nic_info_st *nic_info)
{
    wf_mp_info_st *pmppriv = nic_info->mp_info;

    if (pmppriv == NULL)
        return 0;

    if (pmppriv)
    {
        wf_kfree(pmppriv);
        nic_info->mp_info=NULL;
    }
    //nic_info->is_up = 0;
    //complete_and_exit(NULL, 0);
    LOG_D("[mp_term]");

    return 0;
}

static int mp_test_start(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    hw_info_st *phw_info = pnic_info->hw_info;
    wf_mp_info_st *pmp_info;
	wf_u32 inbuff;
	int ret;
    if ((pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped))
    {
		return -1;
	}

    LOG_D("set test=start");

    if(phw_info->mp_mode == 0)
    {
        phw_info->mp_mode = 1;
        mp_init(pnic_info);
    }

    pmp_info = pnic_info->mp_info;
    if(pmp_info->mode == MP_OFF)
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_DIS_DM, NULL, 0, NULL, 0);
        if(ret == WF_RETURN_FAIL)
        {
            MP_WARN("set reg fail");
			return -1;
        }

        wf_mcu_msg_body_set_ability(pnic_info,ODM_DIS_ALL_FUNC, 0);
        wf_mcu_msg_body_set_ability(pnic_info,ODM_FUNC_SET,ODM_RF_CALIBRATION);

        pmp_info->antenna_tx = 8;
        pmp_info->antenna_rx = 8;
        pmp_info->bStartContTx = wf_false;
        pmp_info->bCarrierSuppression = wf_false;
        pmp_info->bSingleTone = wf_false;

		pmp_info->mode = MP_ON;
    }

    pmp_info->bmac_filter = wf_false;

	inbuff = 0x26;
    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MSG_WRITE_DIG, &inbuff, 1, NULL, 0);
    if(ret == WF_RETURN_FAIL)
    {
        MP_WARN("set reg fail");
		return WF_RETURN_FAIL;
    }

	return 0;
}


static int mp_test_stop(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    hw_info_st *phw_info = pnic_info->hw_info;
    wf_mp_info_st *pmp_info = pnic_info->mp_info;
	int ret = 0;
	wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    if ((pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped))
    {
		return -1;
	}
    LOG_D("set test=stop");

    if(phw_info->mp_mode == 1)
    {
        phw_info->mp_mode = 0;
    }
    if(pmp_info != NULL)
    {
        pmp_info->tx.stop = 1;
        wf_msleep(100);
	    if(pmp_info->mode == MP_ON)
	    {
		    if(pmp_info->network_macaddr == NULL)
		    {
			    MP_WARN("mp_info addr error, check!!!!");
			    return WF_RETURN_FAIL;
		    }
        }
	    wf_memset(pcur_network,0,sizeof(wf_wlan_network_t));
	    pnic_info->nic_state &= (~(WIFI_MP_STATE));
        mp_term(pnic_info);
    }
    sprintf(extra, "test_stop %s\n", ret == 0 ? "ok" : "fail");
	return 0;
}


static int mp_rfthermalmeter_get(struct net_device *dev,struct iw_request_info *info,struct iw_point *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	wf_u8 temp;
    wf_u32 value;

    if (NIC_USB == pnic_info->nic_type)
    {
    	temp = wf_mp_read_rfreg(pnic_info, 0x42, 0x0000FC00);

    	*extra = 0;
        LOG_D("temp:%x",temp);
    	sprintf(extra, "thermal=%02x", temp);
    }
    else
    {
        mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_TEMP_GET, NULL, 0, &value, 1);
        temp = (wf_u8)value;
        LOG_D("temp:%x",temp);
    	sprintf(extra, "thermal=%02x", temp);
    }
	wrqu->length = strlen(extra);


	return 0;
}

static int mp_rfthermalmeter_set(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
    wf_u32 ret;
    if (NIC_USB == pnic_info->nic_type)
    {
        ret = wf_mp_write_rfreg(pnic_info, 0, 0x42, BIT(17) | BIT(16), 0x03);
    }
    else
    {
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_TEMP_SET, NULL, 0, NULL, 0);
    }
    *extra = 0;

    sprintf(extra, "set thermal %s",ret == 0 ? "ok" : "fail");
	wrqu->length = strlen(extra);
    return 0;
}



static int mp_freq_get(struct net_device *dev,struct iw_request_info *info,struct iw_point *wrqu, char *extra)
{
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	wf_u16 temp;
    wf_u32 h2m_msg[2] = { 0 };
	wf_u16 value_16;
	wf_u8 h;
	wf_u8 l;
    MP_DBG();

	*extra = 0;
    if (NIC_USB == pnic_info->nic_type)
    {
    	temp = wf_mp_read_bbreg(pnic_info, 0x0024, 0x0001F800);
		LOG_D("VALE:%x",temp);
    	sprintf(extra, "freq=%02x", temp);
    }
    else
    {
        mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_FREQ_GET, NULL, 0, h2m_msg, 2);
        h = (wf_u8)h2m_msg[0];
        l = (wf_u8)h2m_msg[1];
        value_16 = ((wf_u16)h<<8)|(wf_u16)l;
        sprintf(extra, "freq=%04x", value_16);
        MP_DBG("freq=%04x", value_16);
    }
	wrqu->length = strlen(extra);

	return 0;
}


static int mp_freq_set(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
	wf_u32 temp = 0, ct = 0, xt = 0, retValue = 1;
    wf_u32 sdio_buf[3] = {0};
	int v;
	wf_u8 input[wrqu->length];
	wf_u8 *str;
	char *ptr = NULL;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return WF_RETURN_FAIL;

	ptr = input;
	str = strsep(&ptr, "=");

	if (strlen(ptr) > 4)
		return WF_RETURN_FAIL;

	sprintf(input, "0x" "%s", ptr);
	sscanf(input, "%x", &v);
	printk("0x%x\n", v);

	//ct = (wf_u8) (v >> 8);
	xt = (wf_u8) v;
    if (NIC_USB == pnic_info->nic_type)
    {
        if (( xt > 0) && (xt <= 0x3f)) {
        	temp = xt;
			LOG_D("xt:%x",xt);
			LOG_D("temp:%x",temp);
        	retValue = wf_mp_write_bbreg(pnic_info, 0x0024, 0x0001F800, temp);
        }
    }
    else
    {
        sdio_buf[0] = (wf_u8)ct;
        sdio_buf[1] = (wf_u8)xt;
        sdio_buf[2] = 0;
        retValue = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_FREQ_SET, sdio_buf, 3, NULL, 0);
    }
    if(retValue == 0)
    {
	    sprintf(extra, "set freq ok");
    }
    else
    {
        sprintf(extra, "set freq fail");
    }
	wrqu->length = strlen(extra);

	return 0;

}


static int mp_link_status_get(struct net_device *dev,struct iw_request_info *info,struct iw_point *wrqu, char *extra)
{
	ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_mp_info_st *mp_info = pnic_info->mp_info;
	wf_u8 bLinked = wf_false;
    MP_DBG();

    if((mp_info->ap_connect_stats == wf_true) || (mp_info->sta_connect_stats == wf_true))
    {
        bLinked = wf_true;
    }

	*extra = 0;

	sprintf(extra, "bLinked=%02x", bLinked);

	wrqu->length = strlen(extra);

	return 0;
}

static int mp_drv_version_get(struct net_device *dev,struct iw_request_info *info,struct iw_point *wrqu, char *extra)
{
	*extra = 0;
	sprintf(extra, "\r\n%s %s\r\n\n", DRV_NAME, DRIVERVERSION);
	wrqu->length = strlen(extra);

    return 0;
}



static int mp_set_rate(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
	wf_mp_info_st *pmp_info = pnic_info->mp_info;
	wf_u32 rate = 1;
	wf_u8 input[wrqu->length];
    wf_u32 ret = 0;

	wf_u8 *str;
	wf_u32 inbuff;
	char *ptr = NULL;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return WF_RETURN_FAIL;
	ptr = input;

	str = strsep(&ptr, "=");
	rate = atoi(ptr);
    LOG_D("set rate=%d",rate);


	wf_memset(extra, 0, wrqu->length);

    pmp_info->rateidx = wf_mp_rate_to_rateidx(rate);

    /* mp start :  adjust RF  b mode*/
	if(pmp_info->rateidx < DESC_RATE6M )
	{
		inbuff = 0;
        ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_CTXRATE, &inbuff, 1, NULL, 0);
        if(ret == WF_RETURN_FAIL)
        {
            MP_WARN("set reg fail");
            return -1;
        }
	}
   sprintf(extra, "Set data rate rate:0x%x -> rateIdx:%d  %s", rate, pmp_info->rateidx,ret == 0 ? "ok" : "fail");

	wrqu->length = strlen(extra);

	return 0;

}


static int mp_phy_set_txpower(nic_info_st *pnic_info, wf_u32 txpower)
{
    wf_u32 inbuf = txpower;

    return mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_PROSET_TXPWR_1, &(inbuf), 1, NULL, 0);
}

static int mp_set_tx_power(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
	wf_mp_info_st *pmp_info = pnic_info->mp_info;

	wf_u32 idx_a = 0;
	int MsetPower = 1;
	wf_u8 input[wrqu->length];
	int ret = 0;
	wf_u8 *str;
	char *ptr = NULL;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return WF_RETURN_FAIL;

	ptr = input;

	str = strsep(&ptr, "=");

	MsetPower = strncmp(input, "off", 3);

	if (MsetPower == 0)
    {
		pmp_info->bSetTxPower = 0;
		sprintf(extra, "Test Set power off");
	}
    else
    {
		idx_a = atoi(ptr);

		pmp_info->txpoweridx = (wf_u8) idx_a;

		printk("============> %d\n", idx_a);
		pmp_info->bSetTxPower = 1;
        LOG_D("set tx_power0=%d", idx_a);

        ret = mp_phy_set_txpower(pnic_info, idx_a);
	}
    sprintf(extra, "Set power:%d  %s", idx_a,ret == 0 ? "ok" : "fail");

	wrqu->length = strlen(extra);
	return 0;
}

static int mp_set_rf_path(nic_info_st *pnic_info)
{
	wf_u32 buf[4] = { 0 };
    wf_u32 sdio_buf[5] = {0};
	wf_mp_info_st *pmp_info = pnic_info->mp_info;
	wf_u32 ulAntennaTx, ulAntennaRx;
	wf_mp_antenna_select_ofdm *p_ofdm_tx;
	wf_mp_antenna_select_cck *p_cck_txrx;
	wf_u8 r_rx_antenna_ofdm = 0, r_ant_select_cck_val = 0;
	wf_u32  r_ant_select_ofdm_val = 0, r_ofdm_tx_en_val = 0;
	int ret = 0;

	ulAntennaTx = pmp_info->antenna_tx;
	ulAntennaRx = pmp_info->antenna_rx;

	p_ofdm_tx = (wf_mp_antenna_select_ofdm *) & r_ant_select_ofdm_val;
	p_cck_txrx = (wf_mp_antenna_select_cck *) & r_ant_select_cck_val;

	p_ofdm_tx->r_ant_ht1 = 0x1;
	p_ofdm_tx->r_ant_ht2 = 0x2;
	p_ofdm_tx->r_ant_non_ht = 0x3;

	p_ofdm_tx->r_tx_antenna = 0x1;
	r_ofdm_tx_en_val = 0x1;
	p_ofdm_tx->r_ant_l = 0x1;
	p_ofdm_tx->r_ant_ht_s1 = 0x1;
	p_ofdm_tx->r_ant_non_ht_s1 = 0x1;
	p_cck_txrx->r_ccktx_enable = 0x8;
	r_ofdm_tx_en_val = 0x3;

//    LOG_D("antennatx:%d",ulAntennaTx);
	if (NIC_USB == pnic_info->nic_type)
	{
		ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_ANT_TX, &ulAntennaTx, 1, NULL, 0);
	}
	else
	{
		ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_ANT_TX, NULL, 0, NULL, 1);
	}
	if(ret == WF_RETURN_FAIL)
	{
		MP_WARN("set reg fail");
		return -1;
	}

	r_rx_antenna_ofdm = 0x1;
	p_cck_txrx->r_cckrx_enable = 0x0;
	p_cck_txrx->r_cckrx_enable_2 = 0x0;

	buf[0] = r_ant_select_ofdm_val;
	buf[1] = r_ofdm_tx_en_val;
	buf[2] = (wf_u32) r_rx_antenna_ofdm;
	buf[3] = (wf_u32) r_ant_select_cck_val;
//    LOG_D("antennarx:%d",buf[0]);
//    LOG_D("antennarx:%d",buf[1]);
//    LOG_D("antennarx:%d",buf[2]);
//    LOG_D("antennarx:%d",buf[3]);

	if (NIC_USB == pnic_info->nic_type)
	{
		ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_ANT_RX, buf, 4, NULL, 0);
	}
	else
	{
	    sdio_buf[0] = 1;
        sdio_buf[1] = 0;
        sdio_buf[2] = 0x3;
        sdio_buf[3] = 0x1;
        sdio_buf[4] = 0;
		ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_MP_SET_ANT_RX, sdio_buf, 5, NULL, 1);
	}
	if(ret == WF_RETURN_FAIL)
	{
		MP_WARN("set reg fail");
		return -1;
	}
    return 0;
}

static int mp_set_antenna(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
	wf_mp_info_st *pmp_info = pnic_info->mp_info;
	wf_u8 input[wrqu->length];
	wf_u16 antenna = 0;
    wf_u32 ret;

	wf_u8 *str;
	char *ptr = NULL;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -1;
	ptr = input;
	str = strsep(&ptr, "=");
	antenna = atoi(ptr);
    LOG_D("set tx_ant=%d",antenna);
	pmp_info->antenna_rx = 8;
	pmp_info->antenna_tx = 8;

	ret = mp_set_rf_path(pnic_info);
    sprintf(extra, "switch Tx antenna to %s  %s", ptr,ret == 0 ? "ok" : "fail");
	return 0;
}



static int mp_set_gi(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
	wf_mp_info_st *pmp_info = pnic_info->mp_info;

	wf_u8 input[wrqu->length];
	wf_u32 gi = 0;
    wf_u32 ret;
	wf_u8 *str;
	char *ptr = NULL;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return WF_RETURN_FAIL;


	ptr = input;

	str = strsep(&ptr, "=");

	gi = atoi(ptr);
    LOG_D("set gi=%d",gi);

    if(pmp_info == NULL)
    {
        MP_WARN("mp_info NULL");
        return WF_RETURN_FAIL;
    }
	if (gi == 0)
    {
		pmp_info->preamble = PREAMBLE_SHORT;
    }
    else if (gi == 1)
    {
		pmp_info->preamble = PREAMBLE_LONG;
    }

    ret = wf_mcu_set_preamble(pnic_info, pmp_info->preamble);
    if(ret == WF_RETURN_FAIL)
    {
        sprintf(extra, "set gi fail");
    }
    else
    {
        sprintf(extra, "set gi ok");
    }
	return 0;
}


static int mp_set_bw(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	struct iw_point *wrqu = (struct iw_point *)wdata;
	local_info_st *local_info = pnic_info->local_info;
    wf_mp_info_st *mp_info = pnic_info->mp_info;

    wf_u32 ret;
	wf_u32 bandwidth = 0;
	wf_u8 input[wrqu->length];
	wf_u8 *str;
	char *ptr = NULL;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return WF_RETURN_FAIL;

	ptr = input;
	str = strsep(&ptr, "=");
	bandwidth = atoi(ptr);

    LOG_D("set bw=%d",bandwidth);

	wf_memset(extra, 0, wrqu->length);

    if(bandwidth == 1)
    {
        mp_info->bandwidth = CHANNEL_WIDTH_40;

    }else if(bandwidth == 0)
    {
        mp_info->bandwidth = CHANNEL_WIDTH_20;
    }

    ret = wf_hw_info_set_channnel_bw(pnic_info, mp_info->channel, mp_info->bandwidth, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

	sprintf(extra, "change bw %d to bw %d  %s\n", local_info->bw,bandwidth,ret == 0 ? "ok" : "fail");

	wrqu->length = strlen(extra);
    return 0;
}

static int mp_set_channel(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
	wf_wlan_info_t *pwlan_info = pnic_info->wlan_info;
	wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;
	struct iw_point *wrqu = (struct iw_point *)wdata;
    wf_mp_info_st *mp_info = pnic_info->mp_info;
	wf_u8 input[wrqu->length];
	wf_u32 channel = 1;
	wf_u8 *str;
    wf_u32 ret;
	char *ptr = NULL;
    if ((pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped))
    {
		return -1;
	}

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return WF_RETURN_FAIL;
	ptr = input;
	str = strsep(&ptr, "=");

	channel = atoi(ptr);
    LOG_D("set  channel=%d",channel);

    wf_memset(extra, 0, wrqu->length);
	sprintf(extra, "Change channel %d to channel %d", pcur_network->channel,
			channel);

	mp_info->channel = channel;

    ret = wf_hw_info_set_channnel_bw(pnic_info, mp_info->channel, mp_info->bandwidth, HAL_PRIME_CHNL_OFFSET_DONT_CARE);
    sprintf(extra, "set channel %s\n", ret == 0 ? "ok" : "fail");

    /* bb and iq calibrate */
    wf_mcu_handle_bb_lccalibrate(pnic_info);
    wf_mcu_handle_bb_iq_calibrate(pnic_info, mp_info->channel);

	wrqu->length = strlen(extra);
	return 0;

}





static int mp_wifi_get(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra)
{
    struct iw_point *wrqu;
	char *pch, *token, *tmp[3] = { 0x00, 0x00, 0x00 };
	int err, i;
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    wf_u32 inbuff1[2];
    wf_u32 ret = 0;

	wrqu = (struct iw_point *)wdata;
	err = 0;

	if (copy_from_user(extra, wrqu->pointer, wrqu->length))
		return WF_RETURN_FAIL;

	pch = extra;
	MP_DBG(": in=%s\n", extra);
	i = 0;
	while ((token = strsep(&pch, ",")) != NULL) {
		if (i > 2)
			break;
		tmp[i] = token;
		i++;
	}

	token = strsep(&tmp[0], "=");

    if(strncmp(token, "cmd", 3) == 0)
    {
        inbuff1[0] = BIT(0);/*level*/
        inbuff1[1] = BIT(13);/*module*/
        if (NIC_USB == pnic_info->nic_type)
        {
            LOG_D("test------------------");
            //ret = mcu_cmd_communicate(pnic_info, M0_OPS_HAL_DBGLOG_CONFIG, inbuff1, 2, NULL, 0);
        }
        else
        {
//                  ret = mcu_cmd_communicate(pnic_info, WLAN_OPS_DXX0_READ_VERSION, &efuse_code, 0, version, 1);
        }
        if(ret == WF_RETURN_FAIL)
        {
            MP_WARN("set reg fail");
            return WF_RETURN_FAIL;
        }
        return WF_RETURN_OK;
    }


	if (strcmp(token, "freq") == 0) {
		MP_DBG("get case freq\n");
		mp_freq_get(dev, info, wrqu, extra);
	}

	if (strcmp(token, "thermal") == 0) {
		MP_DBG("get case temp\n");
		mp_rfthermalmeter_get(dev, info, wrqu, extra);
	}

	if (strcmp(token, "bLinked") == 0) {
		MP_DBG("get case bLinked\n");
		mp_link_status_get(dev, info, wrqu, extra);
	}

	if (strcmp(token, "drv_version") == 0) {
		MP_DBG("get case current drv version\n");
		mp_drv_version_get(dev, info, wrqu, extra);
	}


	return err;
}

static int mp_wifi_set(struct net_device *dev,struct iw_request_info *info,union iwreq_data *wdata, char *extra)
{
	struct iw_point *wrqu;
	char *pch,*token, *tmp[3] = { 0x00, 0x00, 0x00 };
	int err, i;
    wf_u32 ret;
	wrqu = (struct iw_point *)wdata;
	err = 0;

	if (copy_from_user(extra, wrqu->pointer, wrqu->length))
    {
        MP_WARN("copy_from_user fail");
		return WF_RETURN_FAIL;
    }

	pch = extra;
	i = 0;
	while ((token = strsep(&pch, ",")) != NULL)
    {
		if (i > 2)
			break;
		tmp[i] = token;
		i++;
	}

	if (strcmp(tmp[0], "test=start") == 0)
    {
		ret = mp_test_start(dev, info, wdata, extra);
        wf_memset(extra, 0, wrqu->length);
	    sprintf(extra, "test_start %s\n", ret == 0 ? "ok" : "fail");
        mp_test_enable = wf_true;
	}

    if(mp_test_enable == wf_false)
    {
        MP_WARN("Please <iwpriv wlanX set test=start> first");
        sprintf(extra, "Please <iwpriv wlanX set test=start> first");
        wrqu->length = strlen(extra);
        return WF_RETURN_FAIL;
    }

	if (strcmp(tmp[0], "test=stop") == 0)
    {
		ret = mp_test_stop(dev, info, wdata, extra);
        wf_memset(extra, 0, wrqu->length);
        sprintf(extra, "test_stop %s\n", ret == 0 ? "ok" : "fail");
        mp_test_enable = wf_false;
	}

	token = strsep(&tmp[0], "=");

	if (strcmp(token, "channel") == 0)
    {
		mp_set_channel(dev, info, wdata, extra);
	}
	else if (strcmp(token, "bw") == 0)
    {
		mp_set_bw(dev, info, wdata, extra);
	}
	else if (strcmp(token, "gi") == 0)
    {
		mp_set_gi(dev, info, wdata, extra);
	}
	else if (strcmp(token, "tx_ant") == 0)
    {
		mp_set_antenna(dev, info, wdata, extra);
	}
	else if (strcmp(token, "tx_power0") == 0)
    {
		mp_set_tx_power(dev, info, wdata, extra);
	}
	else if (strcmp(token, "rate") == 0)
    {
		mp_set_rate(dev, info, wdata, extra);
	}
    else if (strcmp(token, "freq") == 0)
    {
		mp_freq_set(dev, info, wdata, extra);
	}
	else if (strcmp(token, "thermal") == 0)
    {
		mp_rfthermalmeter_set(dev, info, wdata, extra);
	}
	else if (strcmp(token, "tx") == 0)
    {
		wf_mp_test_tx(dev, info, wdata, extra);
	}
	else if (strcmp(token, "rx") == 0)
    {
		wf_mp_test_rx(dev, info, wdata, extra);
	}
	else if (strcmp(token, "stats") == 0)
    {
		wf_mp_stats(dev, info, wdata, extra);
    }
	else if (strcmp(token, "fwdl") == 0)
    {
		wf_mp_fw_download(dev, info, wdata, extra);
    }
	else if (strcmp(token, "cmddl") == 0)
    {
		wf_mp_cmd_download(dev, info, wdata, extra);
    }
    else
    {
        MP_DBG("other token :%s",token);
    }

    wrqu->length = strlen(extra);

	return err;
}







int wf_mp_set(struct net_device *dev,  struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    struct iw_point *wrqu = (struct iw_point *)wdata;
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    MP_DBG();

    if (extra == NULL) {
		wrqu->length = 0;
		return -EIO;
	}
    if(pnic_info == NULL)
    {
        MP_WARN("nic NULL");
        return -ENETDOWN;
    }
    return 0;
}

int wf_mp(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
    struct iw_point *wrqu = (struct iw_point *)wdata;
	wf_u32 subcmd = wrqu->flags;
    ndev_priv_st *pndev_priv = netdev_priv(dev);
    nic_info_st *pnic_info = pndev_priv->nic;
    if ((pnic_info->is_surprise_removed) || (pnic_info->is_driver_stopped))
    {
		return -1;
	}

    if(pnic_info == NULL)
    {
        MP_WARN("nic NULL");
        return -ENETDOWN;
    }
    switch(subcmd)
    {
        case IW_PRV_SET:
            mp_wifi_set(dev,info,wdata,extra);
            break;
        case IW_PRV_GET:
            mp_wifi_get(dev,info,wdata,extra);
            break;

        case IW_PRV_EFUSE_GET_PHY:
            wf_mp_efuse_get(dev,info,wdata,extra);
            break;
        case IW_PRV_EFUSE_SET_PHY:
            wf_mp_efuse_set(dev,info,wdata,extra);
            break;
        case IW_PRV_EFUSE_GET_LOGIC:
            wf_mp_logic_efuse_read(dev,info,wdata,extra);
            break;
        case IW_PRV_EFUSE_SET_LOGIC:
            wf_mp_logic_efuse_write(dev,info,wdata,extra);
            break;

        //case IW_PRV_WRITE_REG:
            //wf_mp_reg_write(dev,info,wdata,extra);
            //break;
        //case IW_PRV_READ_REG:
            //wf_mp_reg_read(dev,info,wdata,extra);
            //break;
        case IW_PRV_WRITE_BB:
            wf_mp_write_bb(dev,info,wdata,extra);
            break;
        case IW_PRV_READ_BB:
            wf_mp_read_bb(dev,info,wdata,extra);
            break;
        case IW_PRV_READ_RF:
            wf_mp_read_rf(dev,info,wdata,extra);
            break;
        case IW_PRV_WRITE_RF:
            wf_mp_write_rf(dev,info,wdata,extra);
            break;

        default :
            MP_WARN("CMD error");
            break;
    }
return 0;
}



#endif
#endif
