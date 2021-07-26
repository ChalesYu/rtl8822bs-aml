#include "common.h"
#include "wf_debug.h"

#ifdef CONFIG_MP_MODE

extern wf_bool mp_test_enable;
#endif
static int prv_hardware_init(nic_info_st *nic_info)
{
    int ret=0;
    //wf_u8 mac_temp[WF_ETH_ALEN];

    LOG_D("[NIC] prv_hardware_init - entry");

    nic_info->is_driver_stopped = wf_false;
    nic_info->is_surprise_removed = wf_false;

    /* tx  init */
    if (wf_tx_info_init(nic_info) < 0)
    {
        LOG_E("===>wf_tx_info_init error");
        return WF_RETURN_FAIL;
    }

    /* rx  init */
    if (wf_rx_init(nic_info))
    {
        LOG_E("===>wf_rx_init error");
        return WF_RETURN_FAIL;
    }

    if (nic_info->virNic==wf_false)
    {
        /* fw download */
        if (wf_fw_download(nic_info) < 0)
        {
            LOG_E("===>wf_fw_download error, exit!!");
            return WF_RETURN_FAIL;
        }
    }

    /* hw info init */
    if (wf_hw_info_init(nic_info) < 0)
    {
        LOG_E("===>wf_hw_info_init error");
        return WF_RETURN_FAIL;
    }

    /* get hw default cfg */
    if (wf_hw_info_get_default_cfg(nic_info) < 0)
    {
        LOG_E("===>wf_hw_info_get_default_cfg error");
        return WF_RETURN_FAIL;
    }

    /* local info init */
    if (wf_local_cfg_init(nic_info) < 0)
    {
        LOG_E("===>wf_local_cfg_init error");
        return WF_RETURN_FAIL;
    }

    /* get local default cfg */
    if (wf_local_cfg_get_default(nic_info) < 0)
    {
        LOG_E("===>wf_local_cfg_get_default error");
        return WF_RETURN_FAIL;
    }

    /* init hardware by default cfg */
    if (wf_hw_info_set_default_cfg(nic_info) < 0)
    {
        LOG_E("===>wf_hw_info_set_default_cfg error");
        return WF_RETURN_FAIL;
    }

    /* configure */
    if (wf_local_cfg_set_default(nic_info) < 0)
    {
        LOG_E("===>wf_local_cfg_set_default error");
        return WF_RETURN_FAIL;
    }

    nic_info->ndev_num++;

    LOG_D("[NIC] prv_hardware_init - exit");

    return ret;
}


static int prv_hardware_term(nic_info_st *nic_info)
{
    LOG_D("[NIC] prv_hardware_term - entry");

    /* rx term */
    if (wf_rx_term(nic_info) < 0)
    {
        LOG_E("===>wf_rx_term error");
        return WF_RETURN_FAIL;
    }

    /* tx term */
    if (wf_tx_info_term(nic_info) < 0)
    {
        LOG_E("===>wf_tx_info_term error");
        return WF_RETURN_FAIL;
    }


    /* hw info term */
    if (wf_hw_info_term(nic_info) < 0)
    {
        LOG_E("===>wf_hw_info_term error");
    }

    /* local configure term */
    if (wf_local_cfg_term(nic_info) < 0)
    {
        LOG_E("===>wf_local_cfg_term error");
        return WF_RETURN_FAIL;
    }

    nic_info->ndev_num--;

    LOG_D("[NIC] prv_hardware_term - exit");

    return WF_RETURN_OK;
}

int nic_init(nic_info_st *nic_info)
{
    LOG_D("[NIC] nic_init - start");

    wf_os_api_sema_init(&nic_info->cmd_sema, 1);

    /* hardware init by chip */
    if (prv_hardware_init(nic_info) < 0)
    {
        LOG_E("===>prv_hardware_init error");
        return WF_RETURN_FAIL;
    }

    /*p2p*/
#ifdef WF_CONFIG_P2P
    if (wf_p2p_init(nic_info) < 0)
    {
        LOG_E("===>wf_p2p_init error");
        return WF_RETURN_FAIL;
    }
#endif

#ifdef CFG_ENABLE_ADHOC_MODE
    if (wf_adhoc_init(nic_info) < 0)
    {
        LOG_E("===>wf_adhoc_init error");
        return WF_RETURN_FAIL;
    }

#endif

    /*wdn init*/
    if (wf_wdn_init(nic_info) < 0)
    {
        LOG_E("===>wf_wdn_init error");
        return WF_RETURN_FAIL;
    }

    /* scan init */
    if (wf_scan_init(nic_info) < 0)
    {
        LOG_E("===>wf_scan_info_init error");
        return WF_RETURN_FAIL;
    }

    /* auth init */
    if (wf_auth_init(nic_info) < 0)
    {
        LOG_E("===>wf_auth_init error");
        return WF_RETURN_FAIL;
    }

    /* assoc init */
    if (wf_assoc_init(nic_info) < 0)
    {
        LOG_E("===>wf_assoc_init error");
        return WF_RETURN_FAIL;
    }

    /* sec init */
    if (wf_sec_info_init(nic_info) < 0)
    {
        LOG_E("===>wf_sec_info_init error");
        return WF_RETURN_FAIL;
    }

    /* pwr_info init  */
    if (wf_lps_init(nic_info) < 0)
    {
        LOG_E("===>wf_lps_init error");
        return WF_RETURN_FAIL;
    }

#if  defined (CONFIG_ARS_DRIVER_SUPPORT)
    if(ars_init(nic_info) < 0)
    {
        LOG_E("===>ars_init error");
        return WF_RETURN_FAIL;
    }
#elif defined (CONFIG_ARS_FIRMWARE_SUPPORT)

    /*odm mangment init*/
    if (wf_odm_mgnt_init(nic_info) < 0)
    {
        LOG_E("===>wf_odm_mgnt_init error");
        return WF_RETURN_FAIL;
    }

#endif

    /* wlan init */
    if (wf_wlan_mgmt_init(nic_info) < 0)
    {
        LOG_E("===>wf_wlan_init error");
        return WF_RETURN_FAIL;
    }
#ifdef CFG_ENABLE_AP_MODE
    /* ap init */
    if (wf_ap_init(nic_info) < 0)
    {
        LOG_E("===>wf_ap_init error");
        return WF_RETURN_FAIL;
    }
#endif

    /* mlme init */
    if (wf_mlme_init(nic_info) < 0)
    {
        LOG_E("===>wf_mlme_init error");
        return WF_RETURN_FAIL;
    }

    nic_info->is_init_commplete = wf_true;
    LOG_D("[NIC] nic_init - end");
    return WF_RETURN_OK;

}



int nic_term(nic_info_st *nic_info)
{
    //wf_wlan_info_t *pwlan_info = nic_info->wlan_info;
    //wf_wlan_network_t *cur_network = &pwlan_info->cur_network;

    LOG_D("[NIC] nic_term - start");
#ifdef CONFIG_MP_MODE

    mp_test_enable = wf_false;
#endif

    /* mlme term */
    if (wf_mlme_term(nic_info) < 0)
    {
        LOG_E("===>wf_mlme_term error");
        return WF_RETURN_FAIL;
    }

#ifdef CFG_ENABLE_AP_MODE
    if (wf_ap_work_stop(nic_info) < 0)
    {
        LOG_E("===>wf_ap_work_stop error");
        return WF_RETURN_FAIL;
    }
#endif

    /* wlan term */
    if (wf_wlan_mgmt_term(nic_info) < 0)
    {
        LOG_E("===>wf_wlan_term error");
        return WF_RETURN_FAIL;
    }

#if defined CONFIG_ARS_DRIVER_SUPPORT
    if (ars_term(nic_info) < 0)
    {
        LOG_E("===>ars_term error");
        return WF_RETURN_FAIL;
    }
#elif defined CONFIG_ARS_FIRMWARE_SUPPORT
    /* odm term */
    if (wf_odm_mgnt_term(nic_info) < 0)
    {
        LOG_E("===>wf_odm_mgnt_term error");
        return WF_RETURN_FAIL;
    }
#endif

    /* pwr_info term  */
    if (wf_lps_term(nic_info) < 0)
    {
        LOG_E("===>wf_lps_term error");
        return WF_RETURN_FAIL;
    }

    /* sec term */
    if (wf_sec_info_term(nic_info) < 0)
    {
        LOG_E("===>wf_sec_info_term error");
        return WF_RETURN_FAIL;
    }

    /* assoc term */
    if (wf_assoc_term(nic_info) < 0)
    {
        LOG_E("===>wf_assoc_term error");
        return WF_RETURN_FAIL;
    }


    /* auth term */
    if (wf_auth_term(nic_info) < 0)
    {
        LOG_E("===>wf_auth_term error");
        return WF_RETURN_FAIL;
    }

    /* scan term */
    if (wf_scan_term(nic_info) < 0)
    {
        LOG_E("===>wf_scan_term error");
        return WF_RETURN_FAIL;
    }

    /*wdn term*/
    if (wf_wdn_term(nic_info) < 0)
    {
        LOG_E("===>wf_wdn_term error");
        return WF_RETURN_FAIL;
    }



#ifdef CFG_ENABLE_ADHOC_MODE
    if (wf_adhoc_term(nic_info) < 0)
    {
        LOG_E("===>wf_adhoc_term error");
        return WF_RETURN_FAIL;
    }
#endif


#ifdef WF_CONFIG_P2P
        if (wf_p2p_term(nic_info) < 0)
        {
            LOG_E("===>wf_p2p_term error");
            return WF_RETURN_FAIL;
        }
#endif


    /* hardware term */
    if (prv_hardware_term(nic_info) < 0)
    {
        LOG_E("prv_hardware_term, fail!");
        return WF_RETURN_FAIL;
    }


#ifdef CONFIG_RICHV200
    wf_mcu_reset_chip(nic_info);
#endif


    LOG_D("[NIC] nic_term - end");
    return WF_RETURN_OK;
}


int nic_enable(nic_info_st *nic_info)
{
    //int ret = 0;
    if(NULL == nic_info)
    {
        LOG_I("[NIC] nic_info is null");
        return WF_RETURN_OK;
    }

    LOG_I("[NIC] nic_enable :"WF_MAC_FMT, WF_MAC_ARG(nic_to_local_addr(nic_info)));

    if( 0 == nic_info->is_up)
    {
        wf_mcu_enable_xmit(nic_info);
        nic_info->is_up = wf_true;
    }
#if defined CONFIG_ARS_DRIVER_SUPPORT
    //ars to do
#elif defined CONFIG_ARS_FIRMWARE_SUPPORT
    {
        odm_mgnt_st *odm = nic_info->odm;
        wf_os_api_timer_set(&odm->odm_wdg_timer, 5000);
    }
#endif
    return WF_RETURN_OK;
}


int nic_disable(nic_info_st *nic_info)
{
    int ret = WF_RETURN_FAIL;

    LOG_D("[%d] nic_disable",nic_info->ndev_id);

    if(nic_info->is_up)
    {
        if(wf_local_cfg_get_work_mode(nic_info) == WF_MASTER_MODE)
        {
#ifdef CFG_ENABLE_AP_MODE
            wf_ap_work_stop(nic_info);
#endif
        }

        if(WF_CANNOT_RUN(nic_info))
        {
            wf_mlme_abort(nic_info);
        }
        else
        {
            wf_mlme_abort(nic_info);
        }

        nic_info->is_up = wf_false;
        ret = WF_RETURN_OK;
    }

    return ret;
}


int nic_suspend(nic_info_st *pnic_info)
{
    mlme_info_t *pmlme_info = pnic_info->mlme_info;
    wf_wlan_mgmt_info_t *pwlan_mgmt_info = pnic_info->wlan_mgmt_info;
    rx_info_t *rx_info = pnic_info->rx_info;
    recv_ba_ctrl_st *ba_ctl = NULL;
    wf_u8 tid = 0;

    if (WF_CANNOT_RUN(pnic_info))
    {
        LOG_I("is_driver_stopped=%s is_surprise_removed = %s\n",
                 pnic_info->is_driver_stopped ? "True" : "False",
                 pnic_info->is_surprise_removed ? "True" : "False");
        return 0;
    }

    for (tid = 0; tid < TID_NUM; tid++)
    {
        ba_ctl = &rx_info->ba_ctl[tid];
        wf_os_api_timer_unreg(&ba_ctl->reordering_ctrl_timer);
    }

    /* destory mlme thread */
    {
        wf_mlme_abort(pnic_info);

        if (pmlme_info->tid)
        {
            wf_os_api_thread_destory(pmlme_info->tid);
            pmlme_info->tid = 0;
        }
    }

    /* destory wlan_mgmt thread */
    {
        wf_msg_que_t *pmsg_que = &pwlan_mgmt_info->msg_que;
        wf_msg_t *pnew_msg;
        int rst;

        rst = wf_msg_new(pmsg_que, WF_WLAN_MGMT_TAG_UNINSTALL, &pnew_msg);
        if (rst)
        {
            LOG_E("new message fail, error code: %d", rst);
        }
        wf_msg_push(pmsg_que, pnew_msg);
        
        /* destory thread */
        if (pwlan_mgmt_info->tid)
        {
            wf_os_api_thread_destory(pwlan_mgmt_info->tid);
            pwlan_mgmt_info->tid = 0;
        }
    }

#ifdef CFG_ENABLE_AP_MODE
    if (wf_ap_work_stop(pnic_info) < 0)
    {
        LOG_E("===>wf_ap_work_stop error");
        return -1;
    }
#endif

    /* rx term */
    if (wf_rx_term(pnic_info) < 0)
    {
        LOG_E("===>wf_rx_term error");
        return WF_RETURN_FAIL;
    }

    /* tx term */
    if (wf_tx_info_term(pnic_info) < 0)
    {
        LOG_E("===>wf_tx_info_term error");
        return WF_RETURN_FAIL;
    }

    if(pnic_info->is_up == wf_true)
    {
        pnic_info->is_up = wf_false;
        pnic_info->is_driver_stopped = wf_true;
    }
    
    LOG_D("suspend success!!");

    return WF_RETURN_OK;
}

int nic_resume(nic_info_st *nic_info)
{
    if(nic_info->is_up == wf_false)
    {
        nic_info->is_up = wf_true;
        nic_info->is_driver_stopped = wf_false;
        nic_info->is_surprise_removed = wf_false;
    }

    /* tx  init */
    if (wf_tx_info_init(nic_info) < 0)
    {
        LOG_E("===>wf_tx_info_init error");
        return WF_RETURN_FAIL;
    }

    /* rx  init */
    if (wf_rx_init(nic_info))
    {
        LOG_E("===>wf_rx_init error");
        return WF_RETURN_FAIL;
    }

    {
        int i = 0;

        while(wf_fw_download(nic_info))
        {
            i++;
            if(i == 3)
            {
                return WF_RETURN_FAIL;
            }
            LOG_I("===>wf_fw_download error, try again!!!");
        }
    }

    /* init hardware by default cfg */
    if (wf_hw_info_set_default_cfg(nic_info) < 0)
    {
        LOG_E("===>wf_hw_info_set_default_cfg error");
        return WF_RETURN_FAIL;
    }

    /* configure */
    if (wf_local_cfg_set_default(nic_info) < 0)
    {
        LOG_E("===>wf_local_cfg_set_default error");
        return WF_RETURN_FAIL;
    }

    nic_info->ndev_num++;

    /* create thread for rx frame handle */
    if(wf_auth_init(nic_info) < 0)
    {
        LOG_E("===>wf_auth_init error");
        return -1;
    }
    /* wlan init */
    if (wf_wlan_mgmt_init(nic_info) < 0)
    {
        LOG_E("===>wf_wlan_init error");
        return -1;
    }
    /* mlme init */
    if (wf_mlme_init(nic_info) < 0)
    {
        LOG_E("===>wf_mlme_init error");
        return -1;
    }

#ifdef CFG_ENABLE_AP_MODE
    if (wf_ap_init(nic_info) < 0)
    {
        LOG_E("===>wf_ap_work_start error");
        return -1;
    }
#endif

    wf_os_api_enable_all_data_queue(nic_info->ndev);

    return WF_RETURN_OK;
}

int nic_shutdown(nic_info_st *nic_info)
{
    nic_info->is_driver_stopped = wf_true;

    wf_scan_stop(nic_info);


    return WF_RETURN_OK;
}

wf_u8 *nic_to_local_addr(nic_info_st *nic_info)
{
    hw_info_st *hw_info = nic_info->hw_info;

    if (hw_info == NULL)
    {
        return NULL;
    }

    return hw_info->macAddr;
}

